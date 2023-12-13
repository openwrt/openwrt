/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_SOUNDING_H_
#define _MAC_AX_SOUNDING_H_

#include "../type.h"
#include "fwcmd.h"
#include "trxcfg.h"

#define CSI_MAX_BUFFER_IDX		0xF
#define SOUNDING_STS_MAX_IDX		0x15
#define CSI_RRSC_BMAP			0x29292911
#define BFRP_RX_STANDBY_TIMER		0x0
#define BFRP_RX_STANDBY_TIMER_V1	0x0
#define NDP_RX_STANDBY_TIMER		0xFF
#define PATCH_NDP_RX_STANDBY_TIMER	0x0
#define CSI_INIT_RATE_HE		0x0
#define CSI_INIT_RATE_VHT		0x0
#define CSI_INIT_RATE_HT		0x0
#define HT_PAYLOAD_OFFSET		0x10
#define VHT_PAYLOAD_OFFSET		0xd
#define HE_PAYLOAD_OFFSET		0xf
#define CSI_SH				0x4
#define SND_SH				0x2
#define SND_MEE_CFG	 (B_AX_BFMEE_BFPARAM_SEL | B_AX_BFMEE_USE_NSTS | \
			 B_AX_BFMEE_CSI_FORCE_RETE_EN | B_AX_BFMEE_BFINF0_NR | \
			 B_AX_BFMEE_BFINFO0_NC)
#define MAX_SNDTXCMDINFO_NUM		0x4
#define MAX_FWCMD_SND_LEN 600

enum FrameExchangeType {
	FRAME_EXCHANGE_SND_AC_SU = 31,
	FRAME_EXCHANGE_SND_AC_MU_BFRP1 = 32,
	FRAME_EXCHANGE_SND_AC_MU_BFRP2 = 33,
	FRAME_EXCHANGE_SND_AC_MU_BFRP3 = 34,
	FRAME_EXCHANGE_SND_AX_SU = 35,
	FRAME_EXCHANGE_SND_AX_MU_BFRP1 = 36,
	FRAME_EXCHANGE_SND_AX_MU_BFRP2 = 37,
	FRAME_EXCHANGE_SND_N_SU = 38
};

enum SND_F2P_TYPE {
	SNDF2P_ONCE = 0,
	SNDF2P_ADD = 1,
	SNDF2P_DEL = 2
};

enum SND_NDPA_MODE {
	SND_NDPA_NORM = 0,
	SND_NDPA_PATCH_STA = 1
};

/**
 * @addtogroup Sounding
 * @{
 */
/**
 * @brief mac_get_csi_buffer_index
 *
 * @param *adapter
 * @param band
 * @param csi_buffer_id
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_csi_buffer_index(struct mac_ax_adapter *adapter, u8 band,
			     u8 csi_buffer_id);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_set_csi_buffer_index
 *
 * @param *adapter
 * @param band
 * @param macid
 * @param csi_buffer_id
 * @param buffer_idx
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_csi_buffer_index(struct mac_ax_adapter *adapter, u8 band,
			     u8 macid, u16 csi_buffer_id, u16 buffer_idx);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_get_snd_sts_index
 *
 * @param *adapter
 * @param band
 * @param index
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_get_snd_sts_index(struct mac_ax_adapter *adapter, u8 band, u8 index);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_set_snd_sts_index
 *
 * @param *adapter
 * @param band
 * @param macid
 * @param index
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_snd_sts_index(struct mac_ax_adapter *adapter, u8 band, u8 macid,
			  u8 index);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_init_snd_mer
 *
 * @param *adapter
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_init_snd_mer(struct mac_ax_adapter *adapter, u8 band);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_init_snd_mee
 *
 * @param *adapter
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_init_snd_mee(struct mac_ax_adapter *adapter, u8 band);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_csi_force_rate
 *
 * @param *adapter
 * @param band
 * @param ht_rate
 * @param vht_rate
 * @param he_rate
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_csi_force_rate(struct mac_ax_adapter *adapter, u8 band, u8 ht_rate,
		       u8 vht_rate, u8 he_rate);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_csi_rrsc
 *
 * @param *adapter
 * @param band
 * @param rrsc
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_csi_rrsc(struct mac_ax_adapter *adapter, u8 band, u32 rrsc);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_set_mu_table
 *
 * @param *adapter
 * @param *mu_table
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_mu_table(struct mac_ax_adapter *adapter,
		     struct mac_mu_table *mu_table);
/**
 * @}
 */

/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_set_csi_para_reg
 *
 * @param *adapter
 * @param *csi_para
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_csi_para_reg(struct mac_ax_adapter *adapter,
			 struct mac_reg_csi_para *csi_para);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_set_csi_para_cctl
 *
 * @param *adapter
 * @param *csi_para
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_csi_para_cctl(struct mac_ax_adapter *adapter,
			  struct mac_cctl_csi_para *csi_para);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_hw_snd_pause_release
 *
 * @param *adapter
 * @param band
 * @param pr
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_hw_snd_pause_release(struct mac_ax_adapter *adapter, u8 band, u8 pr);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_bypass_snd_sts
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_bypass_snd_sts(struct mac_ax_adapter *adapter);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_deinit_mee
 *
 * @param *adapter
 * @param band
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_deinit_mee(struct mac_ax_adapter *adapter, u8 band);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_snd_sup
 *
 * @param *adapter
 * @param *bf_sup
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_snd_sup(struct mac_ax_adapter *adapter, struct mac_bf_sup *bf_sup);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_gid_pos
 *
 * @param *adapter
 * @param *mac_gid_pos
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_gidpos(struct mac_ax_adapter *adapter, struct mac_gid_pos *mu_gid);
/**
 * @}
 */

/**
 * @addtogroup Sounding
 * @{
 */

/**
 * @brief mac_set_snd_para_v1
 *
 * @param *adapter
 * @param *snd_info
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_snd_para(struct mac_ax_adapter *adapter,
		     struct mac_ax_fwcmd_snd *snd_info);
/**
 * @}
 */

#endif
