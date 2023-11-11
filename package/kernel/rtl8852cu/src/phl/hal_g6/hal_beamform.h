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
#ifndef __HAL_BEAMFORM_H__
#define __HAL_BEAMFORM_H__

#define BF_MU_IDX_INVALID 0xFF

#define BF_CAP_HT_BFEE BIT(0)
#define BF_CAP_HT_BFER BIT(1)
#define BF_CAP_VHT_BFEE BIT(2)
#define BF_CAP_VHT_BFER BIT(3)
#define BF_CAP_HE_BFEE BIT(4)
#define BF_CAP_HE_BFER BIT(5)

#define IS_SUPPORT_ACT_AS_BFER(_cap) \
	(_cap & BF_CAP_HE_BFER) || (_cap & BF_CAP_VHT_BFER) \
	|| (_cap & BF_CAP_HT_BFER)

#define IS_SUPPORT_ACT_AS_BFEE(_cap) \
	(_cap & BF_CAP_HE_BFEE) || (_cap & BF_CAP_VHT_BFEE) \
	|| (_cap & BF_CAP_HT_BFEE)

//B[0:7] = {6M, 9M, 12M, 18M, 24M, 36M, 48M, 54M}
//B[8:15] = HT MCS0~MCS7
//B[16:23] = VHT 1SSMCS0~MCS7
//B[24:31] = HE 1SSMCS0~MCS7
enum hal_bf_rrsc_rata {
	HAL_BF_RRSC_6M = 0,
	HAL_BF_RRSC_9M = 1,
	HAL_BF_RRSC_12M,
	HAL_BF_RRSC_18M,
	HAL_BF_RRSC_24M,
	HAL_BF_RRSC_36M,
	HAL_BF_RRSC_48M,
	HAL_BF_RRSC_54M,
	HAL_BF_RRSC_HT_MSC0,
	HAL_BF_RRSC_HT_MSC1,
	HAL_BF_RRSC_HT_MSC2,
	HAL_BF_RRSC_HT_MSC3,
	HAL_BF_RRSC_HT_MSC4,
	HAL_BF_RRSC_HT_MSC5,
	HAL_BF_RRSC_HT_MSC6,
	HAL_BF_RRSC_HT_MSC7,
	HAL_BF_RRSC_VHT_MSC0,
	HAL_BF_RRSC_VHT_MSC1,
	HAL_BF_RRSC_VHT_MSC2,
	HAL_BF_RRSC_VHT_MSC3,
	HAL_BF_RRSC_VHT_MSC4,
	HAL_BF_RRSC_VHT_MSC5,
	HAL_BF_RRSC_VHT_MSC6,
	HAL_BF_RRSC_VHT_MSC7,
	HAL_BF_RRSC_HE_MSC0,
	HAL_BF_RRSC_HE_MSC1,
	HAL_BF_RRSC_HE_MSC2,
	HAL_BF_RRSC_HE_MSC3,
	HAL_BF_RRSC_HE_MSC4,
	HAL_BF_RRSC_HE_MSC5,
	HAL_BF_RRSC_HE_MSC6,
	HAL_BF_RRSC_HE_MSC7 = 31,
	HAL_BF_RRSC_MAX = 32
};

enum hal_bf_role {
	HAL_BF_ROLE_BFEE,
	HAL_BF_ROLE_BFER
};

enum hal_bfee_type {
	HAL_BFEE_SU,
	HAL_BFEE_MU
};

struct hal_sumu_entry {
	_os_list list;
	enum hal_bfee_type type;
	u8 idx;
	u8 snd_sts; /*0: Fail or Not Sound, 1: Sound Success*/
};
struct hal_bf_entry {
	_os_list list;
	u8 bf_idx;
	u16 macid;
	u16 aid12;
	u8 band;
	u16 csi_buf;
	/* csi_buf for swap mode */
	bool en_swap;
	u16 csi_buf_swap;
	u8 couter; /*for swap*/
	struct hal_sumu_entry *bfee;
};

enum hal_bf_forced_mode{
	HAL_BF_FIX_M_DISABLE = 0,
	HAL_BF_FIX_M_SU = 1,
	HAL_BF_FIX_M_MU = 2
};
struct hal_bf_fixed_m_para {
	enum hal_bf_forced_mode fix_m;
	u8 gid;
	u8 fix_resp;
	u8 fix_prot;
	enum rtw_hal_protection_type prot_type;
	enum rtw_hal_ack_resp_type resp_type;
	struct rtw_hal_muba_info fix_ba_info;
};

struct hal_bf_obj {
	/*tx bf entry*/
	u8 max_bf_entry_nr;
	struct hal_bf_entry *bf_entry;
	_os_list bf_idle_list;
	_os_list bf_busy_list;
	u8 num_idle_bf_entry;

	/*su entry*/
	u8 max_su_bfee_nr;
	struct hal_sumu_entry *su_entry;
	_os_list su_idle_list;
	_os_list su_busy_list;
	u8 num_idle_su_entry;

	/*mu entry*/
	u8 max_mu_bfee_nr;
	struct hal_sumu_entry *mu_entry;
	_os_list mu_idle_list;
	_os_list mu_busy_list;
	u8 num_idle_mu_entry;

	_os_lock bf_lock;

	u8 self_bf_cap[2];
	/*for fixed mode*/
	struct hal_bf_fixed_m_para fixed_para;
};


enum rtw_hal_status hal_bf_init(
	struct hal_info_t *hal_info,
	u8 bf_entry_nr,
	u8 su_entry_nr,
	u8 mu_entry_nr);

void hal_bf_deinit(struct hal_info_t *hal_info);


struct hal_bf_entry *
hal_bf_query_idle_bf_entry(
	struct hal_info_t *hal_info,
	bool mu);


enum rtw_hal_status
hal_bf_release_target_bf_entry(
	struct hal_info_t *hal_info,
	void *entry);


enum rtw_hal_status hal_bf_hw_mac_init_bfee(
	struct hal_info_t *hal_info,
	u8 band);

enum rtw_hal_status hal_bf_hw_mac_init_bfer(
	struct hal_info_t *hal_info,
	u8 band);

enum rtw_hal_status hal_bf_set_entry_hwcfg(
	struct hal_info_t *hal_info, void *entry);

void hal_bf_update_entry_snd_sts(struct hal_info_t *hal_info, void *entry);

enum rtw_hal_status hal_bf_cfg_swbf_entry(struct rtw_phl_stainfo_t *sta,
					  bool swap);

enum rtw_hal_status
hal_bf_set_mu_sta_fw(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_bf_set_fix_mode(void *hal, bool mu, bool he);

bool rtw_hal_bf_chk_bf_type(void *hal_info,
	struct rtw_phl_stainfo_t *sta, bool mu);

enum rtw_hal_status
hal_bf_set_bfee_csi_para(struct hal_info_t *hal_info, bool cr_cctl,
			 struct rtw_phl_stainfo_t *sta);

#endif
