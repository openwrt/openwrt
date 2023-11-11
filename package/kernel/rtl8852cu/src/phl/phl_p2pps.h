/******************************************************************************
 *
 * Copyright(c)2019 Realtek Corporation.
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
#ifndef _PHL_P2PPS_H_
#define _PHL_P2PPS_H_
#ifdef CONFIG_PHL_P2PPS
#ifdef RTW_WKARD_P2PPS_REFINE

enum rtw_phl_status
phl_p2pps_init(struct phl_info_t* phl_info);

void
phl_p2pps_deinit(struct phl_info_t* phl_info);

void phl_p2pps_query_noa_with_cnt255(struct phl_info_t* phl_info,
                                     struct rtw_wifi_role_t *w_role,
                                     struct rtw_wifi_role_link_t *rlink,
                                     struct rtw_phl_noa_desc *desc);

enum rtw_phl_status
phl_cmd_noa_disable_hdl(struct phl_info_t *phl, u8 *param);

enum rtw_phl_status
phl_noa_update(struct phl_info_t *phl_i,
	struct rtw_phl_noa_desc *in_desc);

void
phl_p2pps_tsf32_tog_handler(struct phl_info_t* phl_info);

void
phl_p2pps_noa_resume_all(struct phl_info_t *phl,
	struct rtw_wifi_role_t *w_role);
void
phl_p2pps_noa_all_role_resume(struct phl_info_t *phl_info,
	u8 band_idx);

void
phl_p2pps_noa_pause_all(struct phl_info_t *phl,
	struct rtw_wifi_role_t *w_role);
void
phl_p2pps_noa_all_role_pause(struct phl_info_t *phl_info,
	u8 band_idx);

void
phl_p2pps_noa_disable_all(struct phl_info_t *phl,
                          struct rtw_wifi_role_t *w_role,
                          struct rtw_wifi_role_link_t *rlink);

enum rtw_phl_status
rtw_phl_p2pps_noa_update(void *phl,
	struct rtw_phl_noa_desc *in_desc);
#endif

void
phl_p2pps_ap_client_notify(struct phl_info_t *phl,
		struct rtw_wifi_role_t *wrole, enum link_state lstate, u16 client_macid);


#else
#define phl_p2pps_init(_phl_info) RTW_PHL_STATUS_SUCCESS
#define phl_p2pps_deinit(_phl_info)
#define rtw_phl_p2pps_noa_update(_phl, _in_desc) RTW_PHL_STATUS_SUCCESS
#define phl_p2pps_tsf32_tog_handler(_phl_info)

#define phl_p2pps_query_noa_with_cnt255(_phl, _wrole, _rlink, _desc)
#define phl_p2pps_noa_disable_all(_phl, _w_role, _rlink)
#define phl_p2pps_noa_resume_all(_phl, _w_role)
#define phl_p2pps_noa_all_role_resume(_phl, _band_idx)
#define phl_p2pps_noa_pause_all(_phl, _w_role)
#define phl_p2pps_noa_all_role_pause(_phl, _band_idx)
#endif
#endif
