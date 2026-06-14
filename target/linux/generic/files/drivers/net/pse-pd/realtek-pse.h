// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef _REALTEK_PSE_H
#define _REALTEK_PSE_H

#include <linux/container_of.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/pse-pd/pse.h>
#include <linux/types.h>

#define RTPSE_DEVICE_ID_RTL8238B	0x0138
#define RTPSE_DEVICE_ID_RTL8239		0x0039
#define RTPSE_DEVICE_ID_RTL8239C	0x0139
#define RTPSE_DEVICE_ID_BCM59011	0xe011
#define RTPSE_DEVICE_ID_BCM59111	0xe111
#define RTPSE_DEVICE_ID_BCM59121	0xe121

#define RTPSE_MAX_PORTS			48
#define RTPSE_MCU_MSG_SIZE		12

/*
 * Time the MCU itself needs between accepting a request and having a
 * response ready. These are properties of the MCU firmware, not of the
 * underlying transport: the core paces transactions by RTPSE_MCU_RESPONSE_MS
 * and both transports size their per-transaction recv ceiling from
 * RTPSE_MCU_RESPONSE_MAX_MS, since some commands are documented as
 * needing up to ~1s to produce a reply.
 */
#define RTPSE_MCU_RESPONSE_MS			25
#define RTPSE_MCU_RESPONSE_MAX_MS		1000

#define RTPSE_PORT_STS_DISABLED			0x00
#define RTPSE_PORT_STS_SEARCHING		0x01
#define RTPSE_PORT_STS_DELIVERING		0x02
#define RTPSE_PORT_STS_FAULT			0x04
#define RTPSE_PORT_STS_REQUESTING		0x06

/* RTPSE_PORT_SET_POWER_LIMIT_TYPE values */
#define RTPSE_PORT_PW_LIMIT_TYPE_USER		0x02

#define RTPSE_PORT_MAX_PRIORITY			3

struct rtpse_mcu_msg {
	u8 opcode;
	u8 seq_num;
	u8 payload[9];
	u8 checksum;
} __packed;

enum rtpse_cmd {
	RTPSE_CMD_MCU_SET_GLOBAL_STATE,
	RTPSE_CMD_MCU_GLOBAL_RESET,
	RTPSE_CMD_MCU_SET_POWER_BUDGET,
	RTPSE_CMD_MCU_SET_POWER_MGMT_MODE,
	RTPSE_CMD_MCU_GET_SYSTEM_INFO,
	RTPSE_CMD_MCU_GET_POWER_STATS,
	RTPSE_CMD_MCU_GET_EXT_CONFIG,

	RTPSE_CMD_PORT_ENABLE,
	RTPSE_CMD_PORT_SET_AUTO_POWERUP,
	RTPSE_CMD_PORT_SET_DETECTION_TYPE,
	RTPSE_CMD_PORT_SET_POE_MODE,
	RTPSE_CMD_PORT_SET_DISCONNECT_TYPE,
	RTPSE_CMD_PORT_SET_POWER_LIMIT_TYPE,
	RTPSE_CMD_PORT_SET_POWER_LIMIT,
	RTPSE_CMD_PORT_SET_POWER_LIMIT_EXT,
	RTPSE_CMD_PORT_SET_PRIORITY,
	RTPSE_CMD_PORT_GET_STATUS,
	RTPSE_CMD_PORT_GET_POWER_STATS,
	RTPSE_CMD_PORT_GET_CONFIG,
	RTPSE_CMD_PORT_GET_EXT_CONFIG,

	RTPSE_NUM_CMDS,
};

struct rtpse_opcode {
	u8 op;
	bool valid;
};

/* Shorthand for the designated-initializer entries in dialect opcode tables. */
#define RTPSE_OP(opc)	{ .op = (opc), .valid = true }

/* Forward-declared so dialects can supply response parsers. */
struct rtpse_mcu_info;
struct rtpse_port_status;

struct rtpse_mcu_dialect {
	struct rtpse_opcode opcode[RTPSE_NUM_CMDS];

	/*
	 * Response parsers. Each dialect must supply its own; the core calls
	 * these unconditionally rather than carrying a default that would
	 * silently mis-decode bytes from a dialect that forgot to set them.
	 */
	int (*parse_system_info)(const u8 *payload, struct rtpse_mcu_info *info);
	int (*parse_port_class)(const struct rtpse_port_status *status);
	const char *(*mcu_type_str)(unsigned int mcu_type);
};

struct rtpse_chip_info {
	const char *name;
	const struct rtpse_mcu_dialect *dialect;
	u16 device_id;
	u32 max_mW_per_port;
	enum rtpse_cmd pw_set_cmd;	/* command used by set_pw_limit */
	u32 pw_set_lsb_mW;		/* LSB of pw_set_cmd value, in mW */
	u32 pw_read_lsb_mW;		/* LSB of ext_config.max_power read-back, in mW */
	bool i2c_proto_dt_required;	/* I2C framing must come from DT; otherwise assume SMBus */
};

struct rtpse_ctrl;

struct rtpse_port {
	struct rtpse_ctrl *pse;
	unsigned int idx;
	u32 max_mW;
};

struct rtpse_transport_ops {
	int (*send)(struct rtpse_ctrl *pse, const struct rtpse_mcu_msg *req);
	int (*recv)(struct rtpse_ctrl *pse, const struct rtpse_mcu_msg *req,
		    struct rtpse_mcu_msg *resp);
};

struct rtpse_ctrl {
	struct device *dev;
	struct pse_controller_dev pcdev;
	struct mutex mutex;
	const struct rtpse_chip_info *chip;
	const struct rtpse_transport_ops *transport;

	struct regulator *poe_supply;

	unsigned int nr_ports;
	struct rtpse_port *ports __counted_by(nr_ports);
};

static inline struct rtpse_ctrl *to_rtpse_ctrl(struct pse_controller_dev *pcdev)
{
	return container_of(pcdev, struct rtpse_ctrl, pcdev);
}

int rtpse_register(struct rtpse_ctrl *pse);

extern const struct rtpse_chip_info rtl8238b_info;
extern const struct rtpse_chip_info rtl8239_info;
extern const struct rtpse_chip_info rtl8239c_info;
extern const struct rtpse_chip_info bcm59011_info;
extern const struct rtpse_chip_info bcm59111_info;
extern const struct rtpse_chip_info bcm59121_info;

#define RTPSE_OF_MATCH_ENTRIES \
	{ .compatible = "realtek,rtl8238b", .data = &rtl8238b_info }, \
	{ .compatible = "realtek,rtl8239",  .data = &rtl8239_info  }, \
	{ .compatible = "realtek,rtl8239c", .data = &rtl8239c_info }, \
	{ .compatible = "brcm,bcm59011",    .data = &bcm59011_info }, \
	{ .compatible = "brcm,bcm59111",    .data = &bcm59111_info }, \
	{ .compatible = "brcm,bcm59121",    .data = &bcm59121_info }, \
	{ /* sentinel */ }

#endif
