// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Driver for the microcontroller (MCU) fronting Realtek or Broadcom PoE
 * PSE chips. Both vendors' MCUs speak a closely related 12-byte fixed-frame
 * management protocol; this driver covers both via a per-dialect opcode
 * table and response parsers.
 *
 * Many PoE switch designs put a dedicated microcontroller in front of the
 * actual PoE PSE silicon: the host CPU talks to the MCU over I2C/SMBus or
 * UART, and the MCU in turn manages the PSE chips on the board. The MCU
 * speaks a small message-based protocol (12-byte fixed-size frames; opcode
 * + arg + 9 payload bytes + checksum). The PSE chips themselves are not
 * accessed directly; everything goes through MCU commands.
 *
 * This driver targets that architecture for the Realtek-family protocol.
 * Two dialects are supported: Realtek MCUs managing RTL823x/RTL8239* PSE
 * chips, and Broadcom MCUs managing BCM590xx PSE chips. The two share
 * frame format and a sum-mod-256 checksum but diverge on opcode numbers
 * and on a few response layouts; this is handled by the per-dialect
 * opcode table and parser hooks.
 *
 * Out of scope: PSE chips that are interfaced directly from the host
 * without a management MCU, MCU designs that speak an unrelated protocol
 * family, and "dumb PSE" modes where no host control is wired up at all.
 * Those, if and when they show up in the kernel, belong in separate
 * drivers under drivers/net/pse-pd/.
 *
 * This core module implements the protocol, decoding/encoding of MCU
 * responses, and the pse_controller_ops integration. Transport modules
 * (realtek-pse-i2c, realtek-pse-uart) provide the send/recv callbacks.
 */

#include <linux/bitfield.h>
#include <linux/cleanup.h>
#include <linux/delay.h>
#include <linux/fwnode.h>
#include <linux/gpio/consumer.h>
#include <linux/minmax.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/property.h>
#include <linux/pse-pd/pse.h>
#include <linux/unaligned.h>

#include "realtek-pse.h"

/* Parsed MCU response structures (decoded from rtpse_mcu_msg replies) */

struct rtpse_mcu_info {
	u8 max_ports;
	bool system_enable;
	u16 device_id;
	u8 sw_ver;
	u8 mcu_type;
	u8 config_status;
	u8 ext_ver;
};

struct rtpse_mcu_ext_config {
	u8 uvlo;
	u8 ovlo;
	bool prealloc_enable;
	u8 num_of_pses;
};

struct rtpse_port_status {
	u8 sts1;
	u8 sts2;
	u8 sts3;
};

struct rtpse_port_measurement {
	u16 voltage_raw;	/* 64.45mV/LSB */
	u16 current_raw;	/* 1mA/LSB */
	u16 temperature_raw;	/* T(mC) = 1250 * (220 - raw) */
	u16 power_raw;		/* 100mW/LSB */
};

struct rtpse_port_config {
	bool enable;
	u8 function_mode;
	u8 detection_type;
	u8 cls_type;
	u8 disconnect_type;
	u8 pair_type;
};

struct rtpse_port_ext_config {
	u8 inrush_mode;
	u8 limit_type;
	u8 max_power;
	u8 priority;
	u8 chip_addr;
	u8 channel;
};

/* Helpers and basic functions */

static inline void rtpse_mcu_msg_init(struct rtpse_mcu_msg *msg, u8 opcode)
{
	memset(msg, 0xff, sizeof(*msg));
	msg->opcode = opcode;
}

static u8 rtpse_checksum(const u8 *buf, size_t len)
{
	u8 sum = 0;
	while (len--)
		sum += *buf++;
	return sum;
}

static int rtpse_do_xfer(struct rtpse_ctrl *pse, struct rtpse_mcu_msg *req,
			 struct rtpse_mcu_msg *resp)
{
	int ret;

	req->checksum = rtpse_checksum((u8 *)req, RTPSE_MCU_MSG_SIZE - 1);

	scoped_guard(mutex, &pse->mutex) {
		ret = pse->transport->send(pse, req);
		if (ret)
			return ret;

		/*
		 * The MCU needs a fixed amount of time between receiving a request
		 * and having the response ready, regardless of how the bytes get to
		 * us. Pace the transaction here so each transport can keep its recv
		 * path simple: a single bounded wait rather than a generic retry.
		 */
		msleep(RTPSE_MCU_RESPONSE_MS);

		memset(resp, 0, sizeof(*resp));
		ret = pse->transport->recv(pse, req, resp);
		if (ret)
			return ret;
	}

	/*
	 * Explicit MCU error opcodes (observed on the BCM dialect; harmless
	 * to check for RTL too). Catch these before the generic opcode/CRC
	 * mismatch path so callers see a meaningful errno.
	 */
	switch (resp->opcode) {
	case 0xfd:	return -EBADE;		/* request incomplete */
	case 0xfe:	return -EBADMSG;	/* MCU-reported checksum error */
	case 0xff:	return -EAGAIN;		/* MCU not ready */
	}

	if (resp->opcode != req->opcode ||
	    resp->checksum != rtpse_checksum((u8 *)resp, RTPSE_MCU_MSG_SIZE - 1))
		return -EBADMSG;

	return 0;
}

static int rtpse_port_query(struct rtpse_port *port, u8 opcode, struct rtpse_mcu_msg *resp)
{
	struct rtpse_mcu_msg req;
	int ret;

	rtpse_mcu_msg_init(&req, opcode);
	req.payload[0] = port->idx;

	ret = rtpse_do_xfer(port->pse, &req, resp);
	if (ret)
		return ret;

	if (resp->payload[0] != port->idx)
		return -EIO;

	return 0;
}

static int rtpse_port_cmd(struct rtpse_port *port, u8 opcode, u8 arg)
{
	struct rtpse_mcu_msg req, resp;
	int ret;

	rtpse_mcu_msg_init(&req, opcode);
	req.payload[0] = port->idx;
	req.payload[1] = arg;

	ret = rtpse_do_xfer(port->pse, &req, &resp);
	if (ret)
		return ret;

	if (resp.payload[0] != port->idx || resp.payload[1] != 0)
		return -EIO;

	return 0;
}

/* Global operations */

static int rtpse_mcu_get_info(struct rtpse_ctrl *pse, struct rtpse_mcu_info *info)
{
	struct rtpse_mcu_msg req, resp;
	const struct rtpse_opcode *opc;
	int ret;

	opc = &pse->chip->dialect->opcode[RTPSE_CMD_MCU_GET_SYSTEM_INFO];
	if (!opc->valid)
		return -EOPNOTSUPP;

	rtpse_mcu_msg_init(&req, opc->op);
	ret = rtpse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	return pse->chip->dialect->parse_system_info(resp.payload, info);
}

static int rtpse_mcu_get_ext_config(struct rtpse_ctrl *pse, struct rtpse_mcu_ext_config *config)
{
	struct rtpse_mcu_msg req, resp;
	const struct rtpse_opcode *opc;
	int ret;

	opc = &pse->chip->dialect->opcode[RTPSE_CMD_MCU_GET_EXT_CONFIG];
	if (!opc->valid)
		return -EOPNOTSUPP;

	rtpse_mcu_msg_init(&req, opc->op);
	ret = rtpse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	config->uvlo = resp.payload[0];
	config->ovlo = resp.payload[5];
	config->prealloc_enable = (resp.payload[1] == 0x1);
	config->num_of_pses = resp.payload[6];

	return 0;
}

static int rtpse_set_global_state(struct rtpse_ctrl *pse, bool enable)
{
	struct rtpse_mcu_msg req, resp;
	const struct rtpse_opcode *opc;
	int ret;

	opc = &pse->chip->dialect->opcode[RTPSE_CMD_MCU_SET_GLOBAL_STATE];
	if (!opc->valid)
		return -EOPNOTSUPP;

	rtpse_mcu_msg_init(&req, opc->op);
	req.payload[0] = enable ? 0x1 : 0x0;

	ret = rtpse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	return (resp.payload[0] == 0x0) ? 0 : -EIO;
}

__maybe_unused
static int rtpse_global_reset(struct rtpse_ctrl *pse)
{
	struct rtpse_mcu_msg req, resp;
	const struct rtpse_opcode *opc;
	int ret;

	opc = &pse->chip->dialect->opcode[RTPSE_CMD_MCU_GLOBAL_RESET];
	if (!opc->valid)
		return -EOPNOTSUPP;

	rtpse_mcu_msg_init(&req, opc->op);
	req.payload[0] = 0x1;

	ret = rtpse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	return (resp.payload[0] == 0x0) ? 0 : -EIO;
}

/* Port operations */

static int rtpse_port_get_status(struct rtpse_port *port, struct rtpse_port_status *status)
{
	const struct rtpse_opcode *opc;
	struct rtpse_mcu_msg resp;
	int ret;

	opc = &port->pse->chip->dialect->opcode[RTPSE_CMD_PORT_GET_STATUS];
	if (!opc->valid)
		return -EOPNOTSUPP;

	ret = rtpse_port_query(port, opc->op, &resp);
	if (ret)
		return ret;

	status->sts1 = resp.payload[1];
	status->sts2 = resp.payload[2];
	status->sts3 = resp.payload[3];

	return 0;
}

static int rtpse_port_get_measurement(struct rtpse_port *port,
				      struct rtpse_port_measurement *measurement)
{
	const struct rtpse_opcode *opc;
	struct rtpse_mcu_msg resp;
	int ret;

	opc = &port->pse->chip->dialect->opcode[RTPSE_CMD_PORT_GET_POWER_STATS];
	if (!opc->valid)
		return -EOPNOTSUPP;

	ret = rtpse_port_query(port, opc->op, &resp);
	if (ret)
		return ret;

	measurement->voltage_raw = get_unaligned_be16(&resp.payload[1]);
	measurement->current_raw = get_unaligned_be16(&resp.payload[3]);
	measurement->temperature_raw = get_unaligned_be16(&resp.payload[5]);
	measurement->power_raw = get_unaligned_be16(&resp.payload[7]);

	return 0;
}

static int rtpse_port_get_config(struct rtpse_port *port,
				 struct rtpse_port_config *config)
{
	const struct rtpse_opcode *opc;
	struct rtpse_mcu_msg resp;
	int ret;

	opc = &port->pse->chip->dialect->opcode[RTPSE_CMD_PORT_GET_CONFIG];
	if (!opc->valid)
		return -EOPNOTSUPP;

	ret = rtpse_port_query(port, opc->op, &resp);
	if (ret)
		return ret;

	config->enable = (resp.payload[1] == 1);
	config->function_mode = resp.payload[2];
	config->detection_type = resp.payload[3];
	config->cls_type = resp.payload[4];
	config->disconnect_type = resp.payload[5];
	config->pair_type = resp.payload[6];

	return 0;
}

static int rtpse_port_get_ext_config(struct rtpse_port *port,
				     struct rtpse_port_ext_config *config)
{
	const struct rtpse_opcode *opc;
	struct rtpse_mcu_msg resp;
	int ret;

	opc = &port->pse->chip->dialect->opcode[RTPSE_CMD_PORT_GET_EXT_CONFIG];
	if (!opc->valid)
		return -EOPNOTSUPP;

	ret = rtpse_port_query(port, opc->op, &resp);
	if (ret)
		return ret;

	config->inrush_mode = resp.payload[1];
	config->limit_type = resp.payload[2];
	config->max_power = resp.payload[3];
	config->priority = resp.payload[4];
	config->chip_addr = resp.payload[5];
	config->channel = resp.payload[6];

	return 0;
}

static int rtpse_port_set_state(struct rtpse_port *port, bool enable)
{
	const struct rtpse_opcode *opc;

	opc = &port->pse->chip->dialect->opcode[RTPSE_CMD_PORT_ENABLE];
	if (!opc->valid)
		return -EOPNOTSUPP;

	return rtpse_port_cmd(port, opc->op, enable ? 0x1 : 0x0);
}

/* PSE controller ops */

static struct rtpse_port *rtpse_port_from_pcdev(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_ctrl *pse = to_rtpse_ctrl(pcdev);

	if (id < 0 || id >= pse->nr_ports)
		return NULL;

	return &pse->ports[id];
}

static int rtpse_port_get_admin_state(struct pse_controller_dev *pcdev, int id,
				      struct pse_admin_state *admin_state)
{
	struct rtpse_port_config config;
	struct rtpse_port *port;
	int ret;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_config(port, &config);
	if (ret)
		return ret;

	admin_state->c33_admin_state = config.enable ? ETHTOOL_C33_PSE_ADMIN_STATE_ENABLED :
						       ETHTOOL_C33_PSE_ADMIN_STATE_DISABLED;
	return 0;
}

static int rtpse_port_get_pw_status(struct pse_controller_dev *pcdev, int id,
				    struct pse_pw_status *pw_status)
{
	struct rtpse_port_status status;
	struct rtpse_port *port;
	int ret;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_status(port, &status);
	if (ret)
		return ret;

	switch (status.sts1) {
	case RTPSE_PORT_STS_DISABLED:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_DISABLED;
		break;
	case RTPSE_PORT_STS_SEARCHING:
	case RTPSE_PORT_STS_REQUESTING:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_SEARCHING;
		break;
	case RTPSE_PORT_STS_DELIVERING:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_DELIVERING;
		break;
	case RTPSE_PORT_STS_FAULT:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_FAULT;
		break;
	default:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_UNKNOWN;
		break;
	}

	return 0;
}

static int rtpse_port_get_pw_class(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port_status status;
	struct rtpse_port *port;
	int ret;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_status(port, &status);
	if (ret)
		return ret;

	/*
	 * sts2 carries detection+classification only when sts1 is not a
	 * fault state; in fault states it encodes the fault type instead.
	 * Treat the two reserved sts1 codes (0x3, 0x5) as faults too, since
	 * the datasheet hints at "other fault" beyond the explicit 0x4.
	 */
	switch (status.sts1) {
	case RTPSE_PORT_STS_DISABLED:
	case RTPSE_PORT_STS_SEARCHING:
	case RTPSE_PORT_STS_DELIVERING:
	case RTPSE_PORT_STS_REQUESTING:
		return port->pse->chip->dialect->parse_port_class(&status);
	default:
		return 0;
	}
}

static int rtpse_port_get_actual_pw(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port_measurement measurement;
	struct rtpse_port *port;
	int ret;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_measurement(port, &measurement);
	if (ret)
		return ret;

	/* 100mW per LSB */
	return measurement.power_raw * 100U;
}

static int rtpse_port_get_voltage(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port_measurement measurement;
	struct rtpse_port *port;
	int ret;
	u32 uV;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_measurement(port, &measurement);
	if (ret)
		return ret;

	/* 64.45mV per LSB */
	uV = (u32)measurement.voltage_raw * 64450U;
	return min_t(u32, uV, INT_MAX);
}

static int rtpse_port_enable(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port *port;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	return rtpse_port_set_state(port, true);
}

static int rtpse_port_disable(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port *port;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	return rtpse_port_set_state(port, false);
}

static int rtpse_port_get_pw_limit(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port_ext_config config;
	struct rtpse_port *port;
	int ret;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_ext_config(port, &config);
	if (ret)
		return ret;

	return config.max_power * port->pse->chip->pw_read_lsb_mW;
}

static int rtpse_port_set_pw_limit(struct pse_controller_dev *pcdev, int id, int max_mW)
{
	const struct rtpse_opcode *type_opc, *val_opc;
	const struct rtpse_chip_info *chip;
	struct rtpse_port *port;
	unsigned int prg_val;
	int ret;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;
	if (max_mW < 0 || max_mW > port->max_mW)
		return -ERANGE;

	chip = port->pse->chip;
	type_opc = &chip->dialect->opcode[RTPSE_CMD_PORT_SET_POWER_LIMIT_TYPE];
	val_opc = &chip->dialect->opcode[chip->pw_set_cmd];
	if (!type_opc->valid || !val_opc->valid)
		return -EOPNOTSUPP;

	/*
	 * Switch the port to user-defined limit mode first, then program the
	 * limit value. If the second cmd fails, the port is left in
	 * user-defined mode but with the previous limit value; the next
	 * successful set_pw_limit call recovers it.
	 */
	ret = rtpse_port_cmd(port, type_opc->op, RTPSE_PORT_PW_LIMIT_TYPE_USER);
	if (ret)
		return ret;

	prg_val = min_t(unsigned int, max_mW / chip->pw_set_lsb_mW, 0xff);

	return rtpse_port_cmd(port, val_opc->op, prg_val);
}

static int rtpse_port_get_pw_limit_ranges(struct pse_controller_dev *pcdev, int id,
					  struct pse_pw_limit_ranges *out)
{
	struct ethtool_c33_pse_pw_limit_range *range;
	struct rtpse_port *port;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;

	range = kzalloc(sizeof(*range), GFP_KERNEL);
	if (!range)
		return -ENOMEM;

	range[0].min = 0;
	range[0].max = port->max_mW;

	out->c33_pw_limit_ranges = range;
	return 1;
}

static int rtpse_port_get_prio(struct pse_controller_dev *pcdev, int id)
{
	struct rtpse_port *port = rtpse_port_from_pcdev(pcdev, id);
	struct rtpse_port_ext_config config;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtpse_port_get_ext_config(port, &config);
	if (ret)
		return ret;

	return config.priority;
}

static int rtpse_port_set_prio(struct pse_controller_dev *pcdev, int id, unsigned int prio)
{
	const struct rtpse_opcode *opc;
	struct rtpse_port *port;

	port = rtpse_port_from_pcdev(pcdev, id);
	if (!port)
		return -EINVAL;
	if (prio > RTPSE_PORT_MAX_PRIORITY)
		return -ERANGE;

	opc = &port->pse->chip->dialect->opcode[RTPSE_CMD_PORT_SET_PRIORITY];
	if (!opc->valid)
		return -EOPNOTSUPP;

	return rtpse_port_cmd(port, opc->op, prio);
}

static const struct pse_controller_ops rtpse_ops = {
	.pi_get_admin_state = rtpse_port_get_admin_state,
	.pi_get_pw_status = rtpse_port_get_pw_status,
	.pi_get_pw_class = rtpse_port_get_pw_class,
	.pi_get_actual_pw = rtpse_port_get_actual_pw,
	.pi_enable = rtpse_port_enable,
	.pi_disable = rtpse_port_disable,
	.pi_get_voltage = rtpse_port_get_voltage,
	.pi_get_pw_limit = rtpse_port_get_pw_limit,
	.pi_set_pw_limit = rtpse_port_set_pw_limit,
	.pi_get_pw_limit_ranges = rtpse_port_get_pw_limit_ranges,
	.pi_get_prio = rtpse_port_get_prio,
	.pi_set_prio = rtpse_port_set_prio,
};

static int rtpse_discover_ports(struct rtpse_ctrl *pse, u8 nr_ports)
{
	struct fwnode_handle *pis __free(fwnode_handle) = NULL;
	DECLARE_BITMAP(ports, RTPSE_MAX_PORTS) = { };
	u32 port, max_power;

	pse->nr_ports = nr_ports;
	pse->ports = devm_kcalloc(pse->dev, pse->nr_ports, sizeof(*pse->ports),
				  GFP_KERNEL);
	if (!pse->ports)
		return -ENOMEM;

	for (int i = 0; i < pse->nr_ports; i++) {
		pse->ports[i].pse = pse;
		pse->ports[i].idx = i;
		pse->ports[i].max_mW = pse->chip->max_mW_per_port;
	}

	pis = fwnode_get_named_child_node(dev_fwnode(pse->dev), "pse-pis");
	if (!pis)
		return 0;

	fwnode_for_each_child_node_scoped(pis, child) {
		if (fwnode_property_read_u32(child, "reg", &port)) {
			dev_warn(pse->dev, "pse-pi missing reg property\n");
			continue;
		}

		if (port >= pse->nr_ports) {
			dev_warn(pse->dev, "pse-pi reg=%u out of range (nr_ports=%u)\n",
				 port, pse->nr_ports);
			continue;
		}

		if (test_bit(port, ports)) {
			dev_warn(pse->dev, "pse-pi reg=%u duplicate definition\n", port);
			continue;
		}

		if (fwnode_property_read_u32(child, "max-power-milliwatt", &max_power))
			max_power = pse->chip->max_mW_per_port;

		if (max_power > pse->chip->max_mW_per_port) {
			dev_warn(pse->dev,
				 "pse-pi reg=%u: max-power-milliwatt=%u exceeds chip cap %u, clamping\n",
				 port, max_power, pse->chip->max_mW_per_port);
			max_power = pse->chip->max_mW_per_port;
		}

		pse->ports[port].max_mW = max_power;
		__set_bit(port, ports);
	}

	return 0;
}

static int rtpse_discover(struct rtpse_ctrl *pse, struct rtpse_mcu_info *info)
{
	struct rtpse_mcu_ext_config ext_config;
	int ret;

	/*
	 * MCU might refuse to talk yet if we're early in boot. Defer probing to try
	 * again later. Usually, one of the 3 subsequent attempts succeeds.
	 */
	ret = rtpse_mcu_get_info(pse, info);
	if (ret == -ETIMEDOUT)
		return -EPROBE_DEFER;
	if (ret)
		return dev_err_probe(pse->dev, ret, "failed to read MCU info\n");

	if (info->device_id != pse->chip->device_id)
		dev_warn(pse->dev,
			 "DT compatible (id 0x%x) does not match MCU-reported id 0x%x\n",
			 pse->chip->device_id, info->device_id);

	if (!info->max_ports || info->max_ports > RTPSE_MAX_PORTS)
		return dev_err_probe(pse->dev, -EINVAL,
				     "MCU reports invalid port count %u\n", info->max_ports);

	ret = rtpse_mcu_get_ext_config(pse, &ext_config);
	if (ret)
		return dev_err_probe(pse->dev, ret, "failed to read MCU ext config\n");

	ret = rtpse_discover_ports(pse, info->max_ports);
	if (ret)
		return ret;

	dev_info(pse->dev, "%s MCU, %s (id 0x%04x), %u ports across %u PSE chip(s)\n",
		 pse->chip->dialect->mcu_type_str(info->mcu_type), pse->chip->name,
		 info->device_id, info->max_ports, ext_config.num_of_pses);
	return 0;
}

static void rtpse_regulator_disable(void *data)
{
	regulator_disable(data);
}

int rtpse_register(struct rtpse_ctrl *pse)
{
	struct gpio_desc *enable_gpio;
	struct rtpse_mcu_info info;
	int ret;

	BUILD_BUG_ON(sizeof(struct rtpse_mcu_msg) != RTPSE_MCU_MSG_SIZE);

	ret = devm_mutex_init(pse->dev, &pse->mutex);
	if (ret)
		return ret;

	pse->chip = device_get_match_data(pse->dev);
	if (!pse->chip)
		return dev_err_probe(pse->dev, -ENODEV, "missing chip match data\n");

	/*
	 * Catch a dialect that forgot to set one of the required hooks at
	 * probe time, rather than NULL-deref'ing later from a fast path.
	 */
	if (!pse->chip->dialect ||
	    !pse->chip->dialect->parse_system_info ||
	    !pse->chip->dialect->parse_port_class ||
	    !pse->chip->dialect->mcu_type_str)
		return dev_err_probe(pse->dev, -EINVAL,
				     "dialect for chip %s is incomplete\n",
				     pse->chip->name);

	pse->poe_supply = devm_regulator_get(pse->dev, "power");
	if (IS_ERR(pse->poe_supply))
		return dev_err_probe(pse->dev, PTR_ERR(pse->poe_supply),
				     "failed to get PoE supply\n");

	enable_gpio = devm_gpiod_get_optional(pse->dev, "enable", GPIOD_OUT_HIGH);
	if (IS_ERR(enable_gpio))
		return dev_err_probe(pse->dev, PTR_ERR(enable_gpio),
				     "failed to get enable gpio\n");

	ret = rtpse_discover(pse, &info);
	if (ret)
		return ret;

	if (!info.system_enable) {
		ret = rtpse_set_global_state(pse, true);
		/* Dialects without a global-state concept (e.g. BCM) return
		 * -EOPNOTSUPP; treat that as "no separate enable required". */
		if (ret && ret != -EOPNOTSUPP)
			return dev_err_probe(pse->dev, ret,
					     "failed to enable PSE system\n");
	}

	ret = regulator_enable(pse->poe_supply);
	if (ret)
		return dev_err_probe(pse->dev, ret, "failed to enable PoE supply\n");

	ret = devm_add_action_or_reset(pse->dev, rtpse_regulator_disable, pse->poe_supply);
	if (ret)
		return ret;

	/*
	 * Depending on the MCU firmware configuration (which might be different
	 * for every board), it isn't known whether the PoE subsystem is active or
	 * inactive by default. At this stage, the PSE chips might already deliver
	 * power to PDs without any explicit enable.
	 */

	pse->pcdev.owner    = THIS_MODULE;
	pse->pcdev.ops      = &rtpse_ops;
	pse->pcdev.dev      = pse->dev;
	pse->pcdev.types    = ETHTOOL_PSE_C33;
	pse->pcdev.nr_lines = pse->nr_ports;
	pse->pcdev.pis_prio_max = 3;
	pse->pcdev.supp_budget_eval_strategies = PSE_BUDGET_EVAL_STRAT_DYNAMIC;

	return devm_pse_controller_register(pse->dev, &pse->pcdev);
}
EXPORT_SYMBOL_GPL(rtpse_register);

static int rtpse_rtl_parse_system_info(const u8 *payload, struct rtpse_mcu_info *info)
{
	info->max_ports = payload[1];
	info->system_enable = (payload[2] == 0x1);
	info->device_id = get_unaligned_be16(&payload[3]);
	info->sw_ver = payload[5];
	info->mcu_type = payload[6];
	info->config_status = payload[7];
	info->ext_ver = payload[8];
	return 0;
}

static int rtpse_rtl_parse_port_class(const struct rtpse_port_status *status)
{
	/* Class lives in the upper nibble of sts2. */
	return FIELD_GET(GENMASK(7, 4), status->sts2);
}

static const char *rtpse_rtl_mcu_type_str(unsigned int mcu_type)
{
	switch (mcu_type) {
	case 0x00:	return "GigaDevice GD32F310";
	case 0x01:	return "GigaDevice GD32F230";
	case 0x02:	return "GigaDevice GD32F303";
	case 0x03:	return "GigaDevice GD32F103";
	case 0x04:	return "GigaDevice GD32E103";
	case 0x10:	return "Nuvoton M0516";
	case 0x11:	return "Nuvoton M0564";
	case 0x12:	return "Nuvoton NUC029";
	default:	return "unknown";
	}
}

static int rtpse_bcm_parse_system_info(const u8 *payload, struct rtpse_mcu_info *info)
{
	info->max_ports = payload[1];
	/* BCM has no explicit system_enable byte; the closest analog is the
	 * "remote enable" bit in the system-status flags at payload[7]. */
	info->system_enable = !!(payload[7] & BIT(2));
	info->device_id = get_unaligned_be16(&payload[3]);
	info->sw_ver = payload[5];
	info->mcu_type = payload[6];
	info->config_status = payload[7];
	info->ext_ver = payload[8];
	return 0;
}

static int rtpse_bcm_parse_port_class(const struct rtpse_port_status *status)
{
	/* BCM puts the detected class in payload[3] (== sts3) directly.
	 * Mask to the low nibble; class is 0..8 and any high bits would be
	 * noise. */
	return status->sts3 & 0x0f;
}

static const char *rtpse_bcm_mcu_type_str(unsigned int mcu_type)
{
	switch (mcu_type) {
	case 0x00:	return "ST Micro ST32F100";
	case 0x01:	return "Nuvoton M05xx LAN";
	case 0x02:	return "ST Micro STF030C8";
	case 0x03:	return "Nuvoton M058SAN";
	case 0x04:	return "Nuvoton NUC122";
	default:	return "unknown";
	}
}

/*
 * Opcode tables below intentionally cover the full known protocol surface
 * for each dialect, including commands the core does not (yet) call. They
 * document protocol coverage rather than tracking current core consumers,
 * so additions in the PSE framework or this driver can wire up new ops
 * without having to rediscover the opcode values.
 */
static const struct rtpse_mcu_dialect rtpse_dialect_rtk = {
	.parse_system_info = rtpse_rtl_parse_system_info,
	.parse_port_class  = rtpse_rtl_parse_port_class,
	.mcu_type_str      = rtpse_rtl_mcu_type_str,
	.opcode = {
		[RTPSE_CMD_MCU_SET_GLOBAL_STATE]	= RTPSE_OP(0x00),
		[RTPSE_CMD_MCU_GLOBAL_RESET]		= RTPSE_OP(0x02),
		[RTPSE_CMD_MCU_SET_POWER_BUDGET]	= RTPSE_OP(0x04),
		[RTPSE_CMD_MCU_SET_POWER_MGMT_MODE]	= RTPSE_OP(0x10),
		[RTPSE_CMD_MCU_GET_SYSTEM_INFO]		= RTPSE_OP(0x40),
		[RTPSE_CMD_MCU_GET_POWER_STATS]		= RTPSE_OP(0x41),
		[RTPSE_CMD_MCU_GET_EXT_CONFIG]		= RTPSE_OP(0x4a),

		[RTPSE_CMD_PORT_ENABLE]			= RTPSE_OP(0x01),
		[RTPSE_CMD_PORT_SET_AUTO_POWERUP]	= RTPSE_OP(0x08),
		[RTPSE_CMD_PORT_SET_DETECTION_TYPE]	= RTPSE_OP(0x09),
		[RTPSE_CMD_PORT_SET_POE_MODE]		= RTPSE_OP(0x0c),
		[RTPSE_CMD_PORT_SET_DISCONNECT_TYPE]	= RTPSE_OP(0x0f),
		[RTPSE_CMD_PORT_SET_POWER_LIMIT_TYPE]	= RTPSE_OP(0x12),
		[RTPSE_CMD_PORT_SET_POWER_LIMIT]	= RTPSE_OP(0x13),
		[RTPSE_CMD_PORT_SET_POWER_LIMIT_EXT]	= RTPSE_OP(0x14),
		[RTPSE_CMD_PORT_SET_PRIORITY]		= RTPSE_OP(0x15),
		[RTPSE_CMD_PORT_GET_STATUS]		= RTPSE_OP(0x42),
		[RTPSE_CMD_PORT_GET_POWER_STATS]	= RTPSE_OP(0x44),
		[RTPSE_CMD_PORT_GET_CONFIG]		= RTPSE_OP(0x48),
		[RTPSE_CMD_PORT_GET_EXT_CONFIG]		= RTPSE_OP(0x49),
	},
};

static const struct rtpse_mcu_dialect rtpse_dialect_bcm = {
	.parse_system_info = rtpse_bcm_parse_system_info,
	.parse_port_class  = rtpse_bcm_parse_port_class,
	.mcu_type_str      = rtpse_bcm_mcu_type_str,
	.opcode = {
		[RTPSE_CMD_MCU_SET_POWER_BUDGET]	= RTPSE_OP(0x18),
		[RTPSE_CMD_MCU_SET_POWER_MGMT_MODE]	= RTPSE_OP(0x17),
		[RTPSE_CMD_MCU_GET_SYSTEM_INFO]		= RTPSE_OP(0x20),
		[RTPSE_CMD_MCU_GET_POWER_STATS]		= RTPSE_OP(0x23),
		[RTPSE_CMD_MCU_GET_EXT_CONFIG]		= RTPSE_OP(0x2b),

		[RTPSE_CMD_PORT_ENABLE]			= RTPSE_OP(0x00),
		[RTPSE_CMD_PORT_SET_DETECTION_TYPE]	= RTPSE_OP(0x10),
		[RTPSE_CMD_PORT_SET_POE_MODE]		= RTPSE_OP(0x1c),
		[RTPSE_CMD_PORT_SET_DISCONNECT_TYPE]	= RTPSE_OP(0x13),
		[RTPSE_CMD_PORT_SET_POWER_LIMIT_TYPE]	= RTPSE_OP(0x15),
		[RTPSE_CMD_PORT_SET_POWER_LIMIT]	= RTPSE_OP(0x16),
		[RTPSE_CMD_PORT_SET_PRIORITY]		= RTPSE_OP(0x1a),
		[RTPSE_CMD_PORT_GET_STATUS]		= RTPSE_OP(0x21),
		[RTPSE_CMD_PORT_GET_POWER_STATS]	= RTPSE_OP(0x30),
		[RTPSE_CMD_PORT_GET_CONFIG]		= RTPSE_OP(0x25),
		[RTPSE_CMD_PORT_GET_EXT_CONFIG]		= RTPSE_OP(0x26),
	},
};

const struct rtpse_chip_info rtl8238b_info = {
	.device_id = RTPSE_DEVICE_ID_RTL8238B,
	.dialect = &rtpse_dialect_rtk,
	.max_mW_per_port = 30000,
	.name = "RTL8238B",
	.pw_read_lsb_mW = 200,
	.pw_set_cmd = RTPSE_CMD_PORT_SET_POWER_LIMIT,
	.pw_set_lsb_mW = 200,
	.i2c_proto_dt_required = true,
};
EXPORT_SYMBOL_GPL(rtl8238b_info);

const struct rtpse_chip_info rtl8239_info = {
	.device_id = RTPSE_DEVICE_ID_RTL8239,
	.dialect = &rtpse_dialect_rtk,
	.max_mW_per_port = 90000,
	.name = "RTL8239",
	.pw_read_lsb_mW = 400,
	.pw_set_cmd = RTPSE_CMD_PORT_SET_POWER_LIMIT_EXT,
	.pw_set_lsb_mW = 400,
	.i2c_proto_dt_required = true,
};
EXPORT_SYMBOL_GPL(rtl8239_info);

const struct rtpse_chip_info rtl8239c_info = {
	.device_id = RTPSE_DEVICE_ID_RTL8239C,
	.dialect = &rtpse_dialect_rtk,
	.max_mW_per_port = 90000,
	.name = "RTL8239C",
	.pw_read_lsb_mW = 400,
	.pw_set_cmd = RTPSE_CMD_PORT_SET_POWER_LIMIT_EXT,
	.pw_set_lsb_mW = 400,
	.i2c_proto_dt_required = true,
};
EXPORT_SYMBOL_GPL(rtl8239c_info);

/*
 * Broadcom PoE MCU variants. All three speak the same dialect; per-chip
 * fields are filled from the public protocol notes:
 *   https://svanheule.net/switches/software/broadcom_poe_control_protocol
 * which gives us device IDs and the 0.2W power-limit unit. Per-chip
 * max_mW_per_port is not quoted there -- left at a conservative TODO
 * placeholder until verified on hardware.
 */
const struct rtpse_chip_info bcm59011_info = {
	.device_id = RTPSE_DEVICE_ID_BCM59011,
	.dialect = &rtpse_dialect_bcm,
	.max_mW_per_port = 30000,	/* TODO: verify on hardware */
	.name = "BCM59011",
	.pw_read_lsb_mW = 200,
	.pw_set_cmd = RTPSE_CMD_PORT_SET_POWER_LIMIT,
	.pw_set_lsb_mW = 200,
};
EXPORT_SYMBOL_GPL(bcm59011_info);

const struct rtpse_chip_info bcm59111_info = {
	.device_id = RTPSE_DEVICE_ID_BCM59111,
	.dialect = &rtpse_dialect_bcm,
	.max_mW_per_port = 30000,	/* TODO: verify on hardware */
	.name = "BCM59111",
	.pw_read_lsb_mW = 200,
	.pw_set_cmd = RTPSE_CMD_PORT_SET_POWER_LIMIT,
	.pw_set_lsb_mW = 200,
};
EXPORT_SYMBOL_GPL(bcm59111_info);

const struct rtpse_chip_info bcm59121_info = {
	.device_id = RTPSE_DEVICE_ID_BCM59121,
	.dialect = &rtpse_dialect_bcm,
	.max_mW_per_port = 60000,	/* 802.3bt Type 3 */
	.name = "BCM59121",
	.pw_read_lsb_mW = 200,
	.pw_set_cmd = RTPSE_CMD_PORT_SET_POWER_LIMIT,
	.pw_set_lsb_mW = 200,
};
EXPORT_SYMBOL_GPL(bcm59121_info);

MODULE_DESCRIPTION("Driver for MCU fronting Realtek/Broadcom PoE PSE chips (core)");
MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_LICENSE("GPL");
