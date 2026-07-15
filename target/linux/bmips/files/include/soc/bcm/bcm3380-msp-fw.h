/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __SOC_BCM3380_MSP_FW_H
#define __SOC_BCM3380_MSP_FW_H

#define MSP_4KE_CONFIG_OFFSET		0x0700
#define MSP_4KE_CONFIG_MAGIC		0x4d535046
#define MSP_4KE_CONFIG_VERSION		1
#define MSP_4KE_ALIVE_MAGIC		0x4b454f4b
#define MSP_4KE_DQM_CFG_VALUE		0x10000000
#define MSP_4KE_LAN_TX_HEADER		0x04208000

#ifndef __ASSEMBLER__
struct msp_4ke_config {
	u32 magic;
	u32 version;
	u32 ioproc_base;
	u32 host_mbox_in_offset;
	u32 host_mbox_out_offset;
	u32 dqm_cfg_offset;
	u32 dqm_cfg_value;
	u32 in_msg_status_offset;
	u32 in_msg_data_offset;
	u32 rx_queue_status_offset;
	u32 rx_queue_data_offset;
	u32 dqm_not_empty_status_offset;
	u32 rx_high_queue;
	u32 tx_high_queue;
	u32 tx_normal_queue;
	u32 tx_header;
	u32 reserved[16];
};
#endif

#endif /* __SOC_BCM3380_MSP_FW_H */
