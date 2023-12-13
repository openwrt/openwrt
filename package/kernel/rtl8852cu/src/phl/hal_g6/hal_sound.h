/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _HAL_SOUND_H_
#define _HAL_SOUND_H_

#ifdef CONFIG_PHL_CMD_BF

#define HAL_NPDA_RU_IDX_START 0
#define HAL_NPDA_RU_IDX_END_20MHZ 8
#define HAL_NPDA_RU_IDX_END_40MHZ 17
#define HAL_NPDA_RU_IDX_END_80MHZ 36
#define HAL_NPDA_RU_IDX_END_160MHZ 73

#define HAL_SND_HE_BFRP_STA_SZ 5
#define HAL_SND_HE_NDPA_STA_SZ 4
#define HAL_SND_VHT_NDPA_STA_SZ 2

#define HAL_SND_VHT_NDPA_FRM_CTRL 0x54
#define HAL_SND_HE_NDPA_FRM_CTRL 0x54
#define HAL_SND_VHT_BFRP_FRM_CTRL 0x44
#define HAL_SND_HE_BFRP_FRM_CTRL 0x24 /* Trigger Frame */

#define HAL_SND_TRIG_INFO_BFRP 0x1

enum hal_snd_pkt_sel {
	HAL_SND_PKT_SEL_UST_NDPA = 0,
	HAL_SND_PKT_SEL_BST_NDPA =1,
	HAL_SND_PKT_SEL_LAST_NDP = 2,
	HAL_SND_PKT_SEL_MID_NDP = 3,
	HAL_SND_PKT_SEL_MID_BFRP = 4,
	HAL_SND_PKT_SEL_LAST_BFRP = 5,
	HAL_SND_PKT_SEL_MAX
};

enum hal_snd_pkt_ndpa_type {
	HAL_SND_PKT_NDPA_HT = 1,
	HAL_SND_PKT_NDPA_VHT = 2,
	HAL_SND_PKT_NDPA_HE = 3,
	HAL_SND_PKT_NDPA_MAX
};


struct hal_snd_pkt_xmit_para {
	enum channel_width bw;
	enum rtw_data_rate rate;
	enum rtw_gi_ltf gi_ltf;
	u8 stbc;
	u8 ldpc;
};

struct hal_snd_obj {
	struct hal_snd_pkt_xmit_para ndpa_xpara;
	struct hal_snd_pkt_xmit_para bfrp_xpara;
};

enum rtw_hal_status hal_snd_obj_init(void *hal);
enum rtw_hal_status hal_snd_obj_deinit(void *hal);
#else
#define hal_snd_obj_init(_h) RTW_HAL_STATUS_SUCCESS
#define hal_snd_obj_deinit(_h) RTW_HAL_STATUS_SUCCESS
#endif

#endif
