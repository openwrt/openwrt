/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _PHL_CHAN_INFO_DEF_H_
#define _PHL_CHAN_INFO_DEF_H_

#ifdef CONFIG_PHL_CHANNEL_INFO

#define CHAN_INFO_MAX_SIZE 65535
#define MAX_CHAN_INFO_PKT_KEEP 2
#define CHAN_INFO_PKT_TOTAL MAX_CHAN_INFO_PKT_KEEP + 1
#define MAX_CHAN_INFO_CLIENT PHL_MACID_MAX_NUM
#define MAX_CHAN_INFO_CLIENT_ARR_SZ (MAX_CHAN_INFO_CLIENT >> 3)

struct csi_header_t {
	u8 mac_addr[6];			/* mdata: u8 ta[6]? */
	u32 hw_assigned_timestamp;	/* mdata: u32 freerun_cnt */
	u8 channel;			/* Drv define */
	u8 bandwidth;			/* mdata: u8 bw */
	u16 rx_data_rate;		/* mdata: u16 rx_rate */
	u8 nc;				/* ch_rpt_hdr_info */
	u8 nr;				/* ch_rpt_hdr_info */
	u16 num_sub_carrier;		/* Drv define*/
	u8 num_bit_per_tone;		/* Drv define per I/Q */
	u8 avg_idle_noise_pwr;		/* ch_rpt_hdr_info */
	u8 evm[2];			/* ch_rpt_hdr_info */
	u8 rssi[2];			/* phy_info_rpt */
	u32 csi_data_length;		/* ch_rpt_hdr_info */
	u8 rxsc;			/* phy_info_rpt */
	u8 ch_matrix_report;		/* mdata: u8 get_ch_info */
	u8 csi_valid;			/* ch_rpt_hdr_info */
};

struct chan_info_t {
	_os_list list;
	u8* chan_info_buffer;
	u32 length;
	struct csi_header_t csi_header;
};

struct rx_chan_info_pool {
	struct chan_info_t channl_info_pkt[CHAN_INFO_PKT_TOTAL];
	_os_list idle;
	_os_list busy;
	_os_lock idle_lock;	/* spinlock */
	_os_lock busy_lock;	/* spinlock */
	u32 idle_cnt;
	u32 busy_cnt;
};

enum phl_chinfo_action {
	CHINFO_ACT_EN,/*enable or disable*/
	CHINFO_ACT_CFG,
	CHINFO_ACT_MAX
};

enum phl_chinfo_group_num {
	CHINFO_GROUP_NUM_1 = 0,
	CHINFO_GROUP_NUM_2,
	CHINFO_GROUP_NUM_4,
	CHINFO_GROUP_NUM_16,
	CHINFO_GROUP_NUM_MAX
};

enum phl_chinfo_mode {
	CHINFO_MODE_ACK,
	CHINFO_MODE_MACID,
	CHINFO_MODE_NDP,
	CHINFO_MODE_MAX,
};

/*
Compression (I/Q bits)
CHINFO_ACCU_1BYTE: S(8,4),
CHINFO_ACCU_2BYTES: S(16,12)
*/
enum phl_chinfo_accuracy {
	CHINFO_ACCU_1BYTE = 0,
	CHINFO_ACCU_2BYTES,
	CHINFO_ACCU_MAX,
};

enum phl_chinfo_enable_mode {
	CHINFO_EN_LIGHT_MODE= 0,
	CHINFO_EN_RICH_MODE,
	CHINFO_EN_AUTO_MODE,
	CHINFO_EN_RICH_MAX,
};

struct rtw_chinfo_action_parm {
	struct rtw_phl_stainfo_t *sta;
	enum phl_chinfo_group_num group_num;
	enum phl_chinfo_mode mode;
	enum phl_chinfo_enable_mode enable_mode;
	enum phl_chinfo_action act;
	enum phl_chinfo_accuracy accuracy;
#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
	u32 ele_bitmap;
#endif
	u8 enable;
	u16 trig_period;
	u8 tx_nss;
	u8 rx_nss;
#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
	/* decided by core layer */
	u8 chk_ack_rate;
#endif
};

struct rtw_chinfo_cur_parm {
	struct rtw_chinfo_action_parm action_parm;
#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
	enum rtw_data_rate  rate;
#endif

	u8 macid_bitmap[MAX_CHAN_INFO_CLIENT_ARR_SZ];
	u8 num;
};
#endif
#endif /*_PHL_CHAN_INFO_DEF_H_*/
