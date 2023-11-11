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
#ifndef _PHL_ROLE_H_
#define _PHL_ROLE_H_

/* reason of suspended role */
enum phl_role_susp_rsn {
	PHL_ROLE_SUSPEND_RSN_WOW = 0,
	PHL_ROLE_SUSPEND_RSN_DEV_SUSP,
	PHL_ROLE_SUSPEND_RSN_RF_OFF,
	PHL_ROLE_SUSPEND_RSN_MAX
};

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_register_mrc_module(struct phl_info_t *phl_info);
#endif

struct rtw_wifi_role_t *
phl_get_wrole_by_ridx(struct phl_info_t *phl_info, u8 rold_idx);

struct rtw_wifi_role_link_t *
phl_get_rlink_by_hw_band(struct rtw_wifi_role_t *wrole,
                         u8 hw_band);

struct rtw_wifi_role_t *
phl_get_wrole_by_addr(struct phl_info_t *phl_info, u8 *mac_addr);

enum rtw_phl_status
phl_role_noa_notify(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole);
enum rtw_phl_status
phl_role_ap_client_notify(struct phl_info_t *phl_info,
			struct rtw_wifi_role_t *wrole, u8 link_sts, u16 client_macid);

enum rtw_phl_status
phl_role_notify(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole);

enum rtw_phl_status
phl_role_recover(struct phl_info_t *phl_info);
enum rtw_phl_status
phl_cmd_role_recover(struct phl_info_t *phl_info);
enum rtw_phl_status
phl_role_suspend(struct phl_info_t *phl_info, enum phl_role_susp_rsn rsn);
enum rtw_phl_status
phl_cmd_role_suspend(struct phl_info_t *phl_info, enum phl_role_susp_rsn rsn);

#ifdef RTW_PHL_BCN
enum rtw_phl_status
rtw_phl_free_bcn_entry(void *phl, struct rtw_wifi_role_link_t *rlink);

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_issue_bcn_hdl(struct phl_info_t *phl_info, u8 *param);
enum rtw_phl_status
phl_cmd_stop_bcn_hdl(struct phl_info_t *phl_info, u8 *param);
#endif
#endif

u16 phl_role_get_bcn_intvl(struct phl_info_t *phl,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink);

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_wifi_role_start_hdl(struct phl_info_t *phl_info, u8 *param);
enum rtw_phl_status
phl_wifi_role_chg_hdl(struct phl_info_t *phl_info, u8 *param);
enum rtw_phl_status
phl_wifi_role_stop_hdl(struct phl_info_t *phl_info, u8 *param);
enum rtw_phl_status
phl_cmd_get_cur_tsf_hdl(struct phl_info_t *phl_info,
	struct rtw_phl_port_tsf *tsf);
#endif

enum rtw_phl_status
phl_wifi_role_change(struct phl_info_t *phl_info,
                     struct rtw_wifi_role_t *wrole,
                     struct rtw_wifi_role_link_t *rlink,
                     enum wr_chg_id chg_id,
                     void *chg_info
);
enum rtw_phl_status
phl_wifi_role_macid_all_pause(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole, bool pause);

#ifdef CONFIG_DBCC_SUPPORT
enum rtw_phl_status
phl_wifi_role_realloc_band(struct phl_info_t *phl_info,
			   struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink,
                           enum phl_band_idx new_band);
#endif
#endif  /*_PHL_ROLE_H_*/
