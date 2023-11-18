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
#ifndef _PHL_MR_COEX_H_
#define _PHL_MR_COEX_H_

#ifdef CONFIG_MR_COEX_SUPPORT
#define get_mr_coex_i(_phl, _band)	(&get_band_ctrl(_phl, _band)->mr_coex_i)

#define set_prohibit(_phl, _band, _en, _rsn)\
	do {\
		get_mr_coex_i(_phl, _band)->prohibit = _en;\
		get_mr_coex_i(_phl, _band)->proh_rsn = _rsn;\
	} while(0)

#define get_proh_rsn(_phl, _band) (get_mr_coex_i(_phl, _band)->proh_rsn)

#define is_mrcx_prohibit(_phl, _band) (get_mr_coex_i(_phl, _band)->prohibit == true)


u8
rtw_phl_mr_coex_query_inprogress(void *phl,
				 u8 hw_band,
				 enum rtw_phl_mr_coex_chk_inprocess_type check_type);

enum rtw_phl_status
phl_mr_coex_query_role_time_slot_lim(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_wifi_role_link_t *rlink,
				struct phl_mcc_dur_lim_req_info *limit_req_info);

enum rtw_phl_status
phl_register_mr_coex_module(struct phl_info_t *phl);


#ifdef CONFIG_PHL_P2PPS
bool phl_mr_coex_noa_dur_lim_change(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_wifi_role_link_t *rlink,
				struct rtw_phl_noa_desc *noa_desc);
#endif

#endif /* CONFIG_MR_COEX_SUPPORT */
#endif  /*_PHL_MR_COEX_H_*/

