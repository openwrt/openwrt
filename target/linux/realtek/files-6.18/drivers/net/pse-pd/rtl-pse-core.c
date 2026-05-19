// SPDX-License-Identifier: GPL-2.0-or-later

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

#include "rtl-pse.h"

/* Parsed MCU response structures (decoded from rtl_pse_msg replies) */

struct rtl_pse_mcu_info {
	u8 max_ports;
	bool system_enable;
	u16 device_id;
	u8 sw_ver;
	u8 mcu_type;
	u8 config_status;
	u8 ext_ver;
};

struct rtl_pse_mcu_ext_config {
	u8 uvlo;
	u8 ovlo;
	bool prealloc_enable;
	u8 num_of_pses;
};

struct rtl_pse_port_status {
	u8 sts1;
	u8 sts2;
	u8 sts3;
	u8 sts4;
	u8 sts5;
	u8 sts6;
	u8 sts7;
	u8 sts8;
};

struct rtl_pse_port_measurement {
	u16 voltage_raw;	/* 64.45mV/LSB */
	u16 current_raw;	/* 1mA/LSB */
	u16 temperature_raw;	/* T(mC) = 1250 * (220 - raw) */
	u16 power_raw;		/* 100mW/LSB */
};

struct rtl_pse_port_config {
	bool enable;
	u8 function_mode;
	u8 detection_type;
	u8 cls_type;
	u8 disconnect_type;
	u8 pair_type;
};

struct rtl_pse_port_ext_config {
	u8 inrush_mode;
	u8 limit_type;
	u8 max_power;
	u8 priority;
	u8 chip_addr;
	u8 channel;
};

/* Helpers and basic functions */

static inline void rtl_pse_msg_init(struct rtl_pse_msg *msg, u8 opcode)
{
	memset(msg, 0xff, sizeof(*msg));
	msg->opcode = opcode;
}

static u8 rtl_pse_checksum(const u8 *buf, size_t len)
{
	u8 sum = 0;
	while (len--)
		sum += *buf++;
	return sum;
}

static int rtl_pse_do_xfer(struct rtl_pse *pse, struct rtl_pse_msg *req,
			   struct rtl_pse_msg *resp)
{
	int ret;

	req->checksum = rtl_pse_checksum((u8 *)req, RTL_PSE_MSG_SIZE - 1);

	guard(mutex)(&pse->mutex);

	ret = pse->transport->send(pse, req);
	if (ret)
		return ret;

	/*
	 * The MCU needs a fixed amount of time between receiving a request
	 * and having the response ready, regardless of how the bytes get to
	 * us. Pace the transaction here so each transport can keep its recv
	 * path simple: a single bounded wait rather than a generic retry.
	 */
	msleep(RTL_PSE_MCU_RESPONSE_MS);

	memset(resp, 0, sizeof(*resp));
	ret = pse->transport->recv(pse, req, resp);
	if (ret)
		return ret;

	if (resp->opcode != req->opcode ||
	    resp->checksum != rtl_pse_checksum((u8 *)resp, RTL_PSE_MSG_SIZE - 1))
		return -EBADMSG;

	return 0;
}

static int rtl_pse_port_query(struct rtl_pse_port *port, u8 opcode,
			      struct rtl_pse_msg *resp)
{
	struct rtl_pse_msg req;
	int ret;

	rtl_pse_msg_init(&req, opcode);
	req.payload[0] = port->idx;

	ret = rtl_pse_do_xfer(port->pse, &req, resp);
	if (ret)
		return ret;

	if (resp->payload[0] != port->idx)
		return -EIO;

	return 0;
}

static int rtl_pse_port_cmd(struct rtl_pse_port *port, u8 opcode, u8 arg)
{
	struct rtl_pse_msg req, resp;
	int ret;

	rtl_pse_msg_init(&req, opcode);
	req.payload[0] = port->idx;
	req.payload[1] = arg;

	ret = rtl_pse_do_xfer(port->pse, &req, &resp);
	if (ret)
		return ret;

	if (resp.payload[0] != port->idx || resp.payload[1] != 0)
		return -EIO;

	return 0;
}

static const char *rtl_pse_mcu_type_str(unsigned int mcu_type)
{
	switch (mcu_type) {
	case 0x00:
		return "GigaDevice GD32F310";
	case 0x01:
		return "GigaDevice GD32F230";
	case 0x02:
		return "GigaDevice GD32F303";
	case 0x03:
		return "GigaDevice GD32F103";
	case 0x04:
		return "GigaDevice GD32E103";
	case 0x10:
		return "Nuvoton M0516";
	case 0x11:
		return "Nuvoton M0564";
	case 0x12:
		return "Nuvoton NUC029";
	default:
		return "unknown";
	}
}

/* Global operations */

static int rtl_pse_mcu_get_info(struct rtl_pse *pse, struct rtl_pse_mcu_info *info)
{
	struct rtl_pse_msg req, resp;
	int ret;

	rtl_pse_msg_init(&req, RTL_PSE_MCU_GET_SYSTEM_INFO);
	ret = rtl_pse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	info->max_ports = resp.payload[1];
	info->system_enable = (resp.payload[2] == 0x1);
	info->device_id = get_unaligned_be16(&resp.payload[3]);
	info->sw_ver = resp.payload[5];
	info->mcu_type = resp.payload[6];
	info->config_status = resp.payload[7];
	info->ext_ver = resp.payload[8];

	return 0;
}

static int rtl_pse_mcu_get_ext_config(struct rtl_pse *pse,
				      struct rtl_pse_mcu_ext_config *config)
{
	struct rtl_pse_msg req, resp;
	int ret;

	rtl_pse_msg_init(&req, RTL_PSE_MCU_GET_EXT_CONFIG);
	ret = rtl_pse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	config->uvlo = resp.payload[0];
	config->ovlo = resp.payload[5];
	config->prealloc_enable = (resp.payload[1] == 0x1);
	config->num_of_pses = resp.payload[6];

	return 0;
}

static int rtl_pse_set_global_state(struct rtl_pse *pse, bool enable)
{
	struct rtl_pse_msg req, resp;
	int ret;

	rtl_pse_msg_init(&req, RTL_PSE_MCU_SET_GLOBAL_STATE);
	req.payload[0] = enable ? 0x1 : 0x0;

	ret = rtl_pse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	return (resp.payload[0] == 0x0) ? 0 : -EIO;
}

__maybe_unused
static int rtl_pse_global_reset(struct rtl_pse *pse)
{
	struct rtl_pse_msg req, resp;
	int ret;

	rtl_pse_msg_init(&req, RTL_PSE_MCU_GLOBAL_RESET);
	req.payload[0] = 0x1;

	ret = rtl_pse_do_xfer(pse, &req, &resp);
	if (ret)
		return ret;

	return (resp.payload[0] == 0x0) ? 0 : -EIO;
}

/* Port operations */

static int rtl_pse_port_get_status(struct rtl_pse_port *port,
				   struct rtl_pse_port_status *status)
{
	struct rtl_pse_msg resp;
	int ret;

	ret = rtl_pse_port_query(port, RTL_PSE_PORT_GET_STATUS, &resp);
	if (ret)
		return ret;

	status->sts1 = resp.payload[1];
	status->sts2 = resp.payload[2];
	status->sts3 = resp.payload[3];
	/* other stsX are reserved */

	return 0;
}

static int rtl_pse_port_get_measurement(struct rtl_pse_port *port,
					struct rtl_pse_port_measurement *measurement)
{
	struct rtl_pse_msg resp;
	int ret;

	ret = rtl_pse_port_query(port, RTL_PSE_PORT_GET_POWER_STATS, &resp);
	if (ret)
		return ret;

	measurement->voltage_raw = get_unaligned_be16(&resp.payload[1]);
	measurement->current_raw = get_unaligned_be16(&resp.payload[3]);
	measurement->temperature_raw = get_unaligned_be16(&resp.payload[5]);
	measurement->power_raw = get_unaligned_be16(&resp.payload[7]);

	return 0;
}

static int rtl_pse_port_get_config(struct rtl_pse_port *port,
				   struct rtl_pse_port_config *config)
{
	struct rtl_pse_msg resp;
	int ret;

	ret = rtl_pse_port_query(port, RTL_PSE_PORT_GET_CONFIG, &resp);
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

static int rtl_pse_port_get_ext_config(struct rtl_pse_port *port,
				       struct rtl_pse_port_ext_config *config)
{
	struct rtl_pse_msg resp;
	int ret;

	ret = rtl_pse_port_query(port, RTL_PSE_PORT_GET_EXT_CONFIG, &resp);
	if (ret)
		return ret;

	config->inrush_mode = resp.payload[1];
	config->limit_type = resp.payload[2];
	config->max_power = resp.payload[3];
	config->priority = resp.payload[4];

	config->channel = resp.payload[6];

	return 0;
}

static int rtl_pse_port_set_state(struct rtl_pse_port *port, bool enable)
{
	return rtl_pse_port_cmd(port, RTL_PSE_PORT_ENABLE, enable ? 0x1 : 0x0);
}

/* PSE controller ops */

static struct rtl_pse_port *rtl_pse_port_from_pcdev(struct pse_controller_dev *pcdev,
						    int id)
{
	struct rtl_pse *pse = to_rtl_pse(pcdev);

	if (id < 0 || id >= pse->nr_ports)
		return NULL;

	return &pse->ports[id];
}

static int rtl_pse_port_get_admin_state(struct pse_controller_dev *pcdev, int id,
					struct pse_admin_state *admin_state)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_config config;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_config(port, &config);
	if (ret)
		return ret;

	admin_state->c33_admin_state = config.enable ? ETHTOOL_C33_PSE_ADMIN_STATE_ENABLED :
						       ETHTOOL_C33_PSE_ADMIN_STATE_DISABLED;
	return 0;
}

static int rtl_pse_port_get_pw_status(struct pse_controller_dev *pcdev, int id,
				      struct pse_pw_status *pw_status)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_status status;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_status(port, &status);
	if (ret)
		return ret;

	switch (status.sts1) {
	case RTL_PSE_PORT_STS_DISABLED:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_DISABLED;
		break;
	case RTL_PSE_PORT_STS_SEARCHING:
	case RTL_PSE_PORT_STS_REQUESTING:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_SEARCHING;
		break;
	case RTL_PSE_PORT_STS_DELIVERING:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_DELIVERING;
		break;
	case RTL_PSE_PORT_STS_FAULT:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_FAULT;
		break;
	default:
		pw_status->c33_pw_status = ETHTOOL_C33_PSE_PW_D_STATUS_UNKNOWN;
		break;
	}

	return 0;
}

static int rtl_pse_port_get_pw_class(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_status status;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_status(port, &status);
	if (ret)
		return ret;

	/*
	 * sts2 carries detection+classification only when sts1 is not a
	 * fault state; in fault states it encodes the fault type instead.
	 * Treat the two reserved sts1 codes (0x3, 0x5) as faults too, since
	 * the datasheet hints at "other fault" beyond the explicit 0x4.
	 */
	switch (status.sts1) {
	case RTL_PSE_PORT_STS_DISABLED:
	case RTL_PSE_PORT_STS_SEARCHING:
	case RTL_PSE_PORT_STS_DELIVERING:
	case RTL_PSE_PORT_STS_REQUESTING:
		return FIELD_GET(GENMASK(7, 4), status.sts2);
	default:
		return 0;
	}
}

static int rtl_pse_port_get_actual_pw(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_measurement measurement;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_measurement(port, &measurement);
	if (ret)
		return ret;

	/* 100mW per LSB */
	return measurement.power_raw * 100U;
}

static int rtl_pse_port_get_voltage(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_measurement measurement;
	int ret;
	u32 uV;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_measurement(port, &measurement);
	if (ret)
		return ret;

	/* 64.45mV per LSB */
	uV = (u32)measurement.voltage_raw * 64450U;
	return min_t(u32, uV, INT_MAX);
}

static int rtl_pse_port_enable(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);

	if (!port)
		return -EINVAL;

	return rtl_pse_port_set_state(port, true);
}

static int rtl_pse_port_disable(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);

	if (!port)
		return -EINVAL;

	return rtl_pse_port_set_state(port, false);
}

static int rtl_pse_port_get_pw_limit(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_ext_config config;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_ext_config(port, &config);
	if (ret)
		return ret;

	return config.max_power * port->pse->chip->pw_read_lsb_mW;
}

static int rtl_pse_port_set_pw_limit(struct pse_controller_dev *pcdev, int id, int max_mW)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	const struct rtl_pse_chip_info *chip;
	unsigned int prg_val;
	int ret;

	if (!port)
		return -EINVAL;

	if (max_mW < 0 || max_mW > port->max_mW)
		return -ERANGE;

	chip = port->pse->chip;

	/*
	 * Switch the port to user-defined limit mode first, then program the
	 * limit value. If the second cmd fails, the port is left in
	 * user-defined mode but with the previous limit value; the next
	 * successful set_pw_limit call recovers it.
	 */
	ret = rtl_pse_port_cmd(port, RTL_PSE_PORT_SET_POWER_LIMIT_TYPE, 0x2);
	if (ret)
		return ret;

	prg_val = min_t(unsigned int, max_mW / chip->pw_set_lsb_mW, 0xff);

	return rtl_pse_port_cmd(port, chip->pw_set_cmd, prg_val);
}

static int rtl_pse_port_get_pw_limit_ranges(struct pse_controller_dev *pcdev, int id,
					    struct pse_pw_limit_ranges *out)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct ethtool_c33_pse_pw_limit_range *range;

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

static int rtl_pse_port_get_prio(struct pse_controller_dev *pcdev, int id)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);
	struct rtl_pse_port_ext_config config;
	int ret;

	if (!port)
		return -EINVAL;

	ret = rtl_pse_port_get_ext_config(port, &config);
	if (ret)
		return ret;

	return config.priority;
}

static int rtl_pse_port_set_prio(struct pse_controller_dev *pcdev, int id,
				 unsigned int prio)
{
	struct rtl_pse_port *port = rtl_pse_port_from_pcdev(pcdev, id);

	if (!port)
		return -EINVAL;

	if (prio > 0x3)
		return -ERANGE;

	return rtl_pse_port_cmd(port, RTL_PSE_PORT_SET_PRIORITY, prio);
}

static const struct pse_controller_ops rtl_pse_ops = {
	.pi_get_admin_state = rtl_pse_port_get_admin_state,
	.pi_get_pw_status = rtl_pse_port_get_pw_status,
	.pi_get_pw_class = rtl_pse_port_get_pw_class,
	.pi_get_actual_pw = rtl_pse_port_get_actual_pw,
	.pi_enable = rtl_pse_port_enable,
	.pi_disable = rtl_pse_port_disable,
	.pi_get_voltage = rtl_pse_port_get_voltage,
	.pi_get_pw_limit = rtl_pse_port_get_pw_limit,
	.pi_set_pw_limit = rtl_pse_port_set_pw_limit,
	.pi_get_pw_limit_ranges = rtl_pse_port_get_pw_limit_ranges,
	.pi_get_prio = rtl_pse_port_get_prio,
	.pi_set_prio = rtl_pse_port_set_prio,
};

static int rtl_pse_setup_ports(struct rtl_pse *pse, u8 nr_ports)
{
	struct fwnode_handle *pis __free(fwnode_handle) = NULL;
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
			dev_warn(pse->dev,
				 "pse-pi reg=%u out of range (nr_ports=%u)\n",
				 port, pse->nr_ports);
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
	}

	return 0;
}

int rtl_pse_register(struct rtl_pse *pse)
{
	struct rtl_pse_mcu_ext_config ext_config;
	struct rtl_pse_mcu_info info;
	struct gpio_desc *enable_gpio;
	int ret;

	BUILD_BUG_ON(sizeof(struct rtl_pse_msg) != RTL_PSE_MSG_SIZE);

	ret = devm_mutex_init(pse->dev, &pse->mutex);
	if (ret)
		return ret;

	pse->chip = device_get_match_data(pse->dev);
	if (!pse->chip)
		return dev_err_probe(pse->dev, -ENODEV, "missing chip match data\n");

	enable_gpio = devm_gpiod_get_optional(pse->dev, "enable", GPIOD_OUT_HIGH);
	if (IS_ERR(enable_gpio))
		return dev_err_probe(pse->dev, PTR_ERR(enable_gpio),
				     "failed to get enable gpio\n");

	/*
	 * MCU might time out here if we're early in boot. Defer probing to try
	 * again later. Usually, one of the 3 subsequent attempts succeed.
	 */
	ret = rtl_pse_mcu_get_info(pse, &info);
	if (ret == -ETIMEDOUT)
		return -EPROBE_DEFER;
	if (ret)
		return dev_err_probe(pse->dev, ret, "MCU not responding\n");

	if (info.device_id != pse->chip->device_id)
		dev_warn(pse->dev,
			 "DT compatible (id 0x%x) does not match MCU-reported id 0x%x\n",
			 pse->chip->device_id, info.device_id);

	if (!info.max_ports || info.max_ports > RTL_PSE_MAX_PORTS)
		return dev_err_probe(pse->dev, -EINVAL,
				     "MCU reports invalid port count %u\n", info.max_ports);

	ret = rtl_pse_mcu_get_ext_config(pse, &ext_config);
	if (ret)
		return dev_err_probe(pse->dev, ret, "failed to read MCU ext config\n");

	if (!info.system_enable) {
		ret = rtl_pse_set_global_state(pse, true);
		if (ret)
			return dev_err_probe(pse->dev, ret,
					     "failed to enable PSE system\n");
	}

	ret = rtl_pse_setup_ports(pse, info.max_ports);
	if (ret)
		return ret;

	dev_info(pse->dev,
		 "%s MCU, %s (id 0x%04x), %u ports across %u PSE chip(s)\n",
		 rtl_pse_mcu_type_str(info.mcu_type), pse->chip->name,
		 info.device_id, info.max_ports, ext_config.num_of_pses);

	pse->pcdev.owner    = THIS_MODULE;
	pse->pcdev.ops      = &rtl_pse_ops;
	pse->pcdev.dev      = pse->dev;
	pse->pcdev.types    = ETHTOOL_PSE_C33;
	pse->pcdev.nr_lines = pse->nr_ports;

	return devm_pse_controller_register(pse->dev, &pse->pcdev);
}
EXPORT_SYMBOL_GPL(rtl_pse_register);

const struct rtl_pse_chip_info rtl8238b_info = {
	.device_id = RTL_PSE_DEVICE_ID_RTL8238B,
	.max_mW_per_port = 30000,
	.name = "RTL8238B",
	.pw_read_lsb_mW = 200,
	.pw_set_cmd = RTL_PSE_PORT_SET_POWER_LIMIT,
	.pw_set_lsb_mW = 200,
};
EXPORT_SYMBOL_GPL(rtl8238b_info);

const struct rtl_pse_chip_info rtl8239_info = {
	.device_id = RTL_PSE_DEVICE_ID_RTL8239,
	.max_mW_per_port = 90000,
	.name = "RTL8239",
	.pw_read_lsb_mW = 400,
	.pw_set_cmd = RTL_PSE_PORT_SET_POWER_LIMIT_EXT,
	.pw_set_lsb_mW = 400,
};
EXPORT_SYMBOL_GPL(rtl8239_info);

const struct rtl_pse_chip_info rtl8239c_info = {
	.device_id = RTL_PSE_DEVICE_ID_RTL8239C,
	.max_mW_per_port = 90000,
	.name = "RTL8239C",
	.pw_read_lsb_mW = 400,
	.pw_set_cmd = RTL_PSE_PORT_SET_POWER_LIMIT_EXT,
	.pw_set_lsb_mW = 400,
};
EXPORT_SYMBOL_GPL(rtl8239c_info);

MODULE_DESCRIPTION("Realtek PoE PSE controller core");
MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_LICENSE("GPL");
