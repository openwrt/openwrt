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
#ifndef _PHL_MCC_DEF_H_
#define _PHL_MCC_DEF_H_
/* MCC definition for public usage */
#ifdef CONFIG_MCC_SUPPORT

struct phl_com_mcc_info {
	struct rtw_phl_mcc_ops ops;
};

struct phl_mcc_dur_lim_req_info {
	enum rtw_phl_mcc_dur_lim_tag tag;
	bool enable;
	u32 start_t_h; /*start time of Prohibit slot*/
	u32 start_t_l;
	u32 dur; /*Prohibit time slot, unit: us*/
	u32 intvl; /*Prohibit internval, unit us*/
};

struct phl_tdmra_dur_change_info {
	bool bt_role;
	u8 hw_band;
	struct rtw_wifi_role_t *role;
	u16 dur;
};

void rtw_phl_mcc_watchdog(struct phl_info_t *phl, u8 band_idx);

void rtw_phl_mcc_sta_entry_change(struct phl_info_t *phl,
					struct rtw_wifi_role_t *wrole);

void phl_mcc_ap_client_notify(struct phl_info_t *phl,
			struct rtw_wifi_role_t *wrole, enum link_state state);

enum rtw_phl_status rtw_phl_mcc_dur_lim_change(struct phl_info_t *phl,
				struct rtw_wifi_role_t *wrole,
				struct phl_mcc_dur_lim_req_info *lim_req);

bool rtw_phl_mcc_inprogress(struct phl_info_t *phl, u8 band_idx);

enum rtw_phl_status rtw_phl_tdmra_recovery(struct phl_info_t *phl,
					u8 band_idx);

enum rtw_phl_status
rtw_phl_mcc_enable(struct phl_info_t *phl,
                   struct rtw_wifi_role_t *cur_role,
                   struct rtw_wifi_role_link_t *cur_rlink);

enum rtw_phl_status
rtw_phl_mcc_disable(struct phl_info_t *phl,
                    struct rtw_wifi_role_t *spec_role,
                    struct rtw_wifi_role_link_t *spec_rlink);

enum rtw_phl_status rtw_phl_tdmra_duration_change(struct phl_info_t *phl,
			struct phl_tdmra_dur_change_info *info);

enum rtw_phl_status
rtw_phl_tdmra_enable(struct phl_info_t *phl,
                     struct rtw_wifi_role_t *cur_role,
                     struct rtw_wifi_role_link_t *cur_rlink);

enum rtw_phl_status
rtw_phl_tdmra_disable(struct phl_info_t *phl,
                      struct rtw_wifi_role_t *spec_role,
                      struct rtw_wifi_role_link_t *spec_rlink);

enum rtw_phl_status rtw_phl_mcc_init_ops(struct phl_info_t *phl, struct rtw_phl_mcc_ops *ops);

enum rtw_phl_status rtw_phl_mcc_init(struct phl_info_t *phl);

void rtw_phl_mcc_deinit(struct phl_info_t *phl);

bool rtw_phl_mcc_get_dbg_info(struct phl_info_t *phl, u8 band_idx,
				enum rtw_phl_mcc_dbg_type type, void *info);

#else /* CONFIG_MCC_SUPPORT ==0 */
#define rtw_phl_mcc_watchdog(_phl, _band_idx)
#define phl_mcc_ap_client_notify(_phl, _wrole, _state)
#define rtw_phl_mcc_enable(_phl,_cur_role, _cur_rlink) RTW_PHL_STATUS_FAILURE
#define rtw_phl_mcc_disable(_phl,_spec_role,_spec_rlink) RTW_PHL_STATUS_FAILURE
#define rtw_phl_mcc_init_ops(_phl, _ops) RTW_PHL_STATUS_FAILURE
#define rtw_phl_mcc_init(_phl) RTW_PHL_STATUS_FAILURE
#define rtw_phl_mcc_deinit(_phl)
#define rtw_phl_mcc_inprogress(_phl, _band_idx) false
#define rtw_phl_tdmra_recovery(_phl, _band_idx) RTW_PHL_STATUS_SUCCESS
#define rtw_phl_tdmra_duration_change(_phl, _info) RTW_PHL_STATUS_FAILURE
#define rtw_phl_tdmra_enable(_phl, _cur_role, _cur_rlink) RTW_PHL_STATUS_FAILURE
#define rtw_phl_tdmra_disable(_phl, _spec_role, _spec_rlink) RTW_PHL_STATUS_FAILURE
#define rtw_phl_mcc_sta_entry_change(_phl, _wrole);

#endif /* CONFIG_MCC_SUPPORT */
#endif /*_PHL_MCC_DEF_H_*/
