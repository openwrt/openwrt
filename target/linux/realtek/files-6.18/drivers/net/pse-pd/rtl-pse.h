// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef _RTL_PSE_H
#define _RTL_PSE_H

#include <linux/container_of.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/pse-pd/pse.h>
#include <linux/types.h>

#define RTL_PSE_DEVICE_ID_RTL8238B	0x0138
#define RTL_PSE_DEVICE_ID_RTL8238C	0x0238
#define RTL_PSE_DEVICE_ID_RTL8239	0x0039
#define RTL_PSE_DEVICE_ID_RTL8239C	0x0139

#define RTL_PSE_MAX_PORTS	48
#define RTL_PSE_MSG_SIZE	12

/*
 * Time the MCU itself needs between accepting a request and having a
 * response ready. These are properties of the MCU firmware, not of the
 * underlying transport: the core paces transactions by RTL_PSE_MCU_RESPONSE_MS
 * and both transports size their per-transaction recv ceiling from
 * RTL_PSE_MCU_RESPONSE_MAX_MS, since some commands are documented as
 * needing up to ~1s to produce a reply.
 */
#define RTL_PSE_MCU_RESPONSE_MS		25
#define RTL_PSE_MCU_RESPONSE_MAX_MS	1000

/* protocol opcodes */
#define RTL_PSE_MCU_GLOBAL_RESET		0x02
#define RTL_PSE_MCU_SET_GLOBAL_STATE		0x00
#define RTL_PSE_MCU_SET_POWER_BUDGET		0x04
#define RTL_PSE_MCU_SET_POWER_MGMT_MODE		0x10
#define RTL_PSE_MCU_GET_SYSTEM_INFO		0x40
#define RTL_PSE_MCU_GET_POWER_STATS		0x41
#define RTL_PSE_MCU_GET_EXT_CONFIG		0x4a

#define RTL_PSE_PORT_ENABLE			0x01
#define RTL_PSE_PORT_SET_AUTO_POWERUP		0x08
#define RTL_PSE_PORT_SET_DETECTION_TYPE		0x09
#define RTL_PSE_PORT_SET_POE_MODE		0x0c
#define RTL_PSE_PORT_SET_DISCONNECT_TYPE	0x0f
#define RTL_PSE_PORT_SET_POWER_LIMIT_TYPE	0x12
#define RTL_PSE_PORT_SET_POWER_LIMIT		0x13
#define RTL_PSE_PORT_SET_POWER_LIMIT_EXT	0x14
#define RTL_PSE_PORT_SET_PRIORITY		0x15
#define RTL_PSE_PORT_GET_STATUS			0x42
#define RTL_PSE_PORT_GET_POWER_STATS		0x44
#define RTL_PSE_PORT_GET_CONFIG			0x48
#define RTL_PSE_PORT_GET_EXT_CONFIG		0x49

#define RTL_PSE_PORT_STS_DISABLED		0x00
#define RTL_PSE_PORT_STS_SEARCHING		0x01
#define RTL_PSE_PORT_STS_DELIVERING		0x02
#define RTL_PSE_PORT_STS_FAULT			0x04
#define RTL_PSE_PORT_STS_REQUESTING		0x06

struct rtl_pse_msg {
	u8 opcode;
	union {
		u8 seq_num;
		u8 subcommand;
	};
	u8 payload[9];
	u8 checksum;
} __packed;

struct rtl_pse_chip_info {
	const char *name;
	u16 device_id;
	u32 max_mW_per_port;
	u8 pw_set_cmd;		/* opcode used by set_pw_limit */
	u32 pw_set_lsb_mW;	/* LSB of pw_set_cmd value, in mW */
	u32 pw_read_lsb_mW;	/* LSB of ext_config.max_power read-back, in mW */
};

struct rtl_pse;

struct rtl_pse_port {
	struct rtl_pse *pse;
	unsigned int idx;
	u32 max_mW;
};

struct rtl_pse_transport_ops {
	int (*send)(struct rtl_pse *pse, const struct rtl_pse_msg *req);
	int (*recv)(struct rtl_pse *pse, const struct rtl_pse_msg *req,
		    struct rtl_pse_msg *resp);
};

struct rtl_pse {
	struct device *dev;
	struct pse_controller_dev pcdev;
	struct mutex mutex;
	const struct rtl_pse_chip_info *chip;

	unsigned int nr_ports;
	struct rtl_pse_port *ports __counted_by(nr_ports);

	const struct rtl_pse_transport_ops *transport;
};

static inline struct rtl_pse *to_rtl_pse(struct pse_controller_dev *pcdev)
{
	return container_of(pcdev, struct rtl_pse, pcdev);
}

int rtl_pse_register(struct rtl_pse *pse);

extern const struct rtl_pse_chip_info rtl8238b_info;
extern const struct rtl_pse_chip_info rtl8239_info;
extern const struct rtl_pse_chip_info rtl8239c_info;

#define RTL_PSE_OF_MATCH_ENTRIES \
	{ .compatible = "realtek,rtl8238b", .data = &rtl8238b_info }, \
	{ .compatible = "realtek,rtl8239",  .data = &rtl8239_info  }, \
	{ .compatible = "realtek,rtl8239c", .data = &rtl8239c_info }, \
	{ /* sentinel */ }

#endif
