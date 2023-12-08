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
#ifndef _PHL_SW_CAP_H_
#define _PHL_SW_CAP_H_

enum rtw_phl_status
phl_sw_cap_init(struct rtw_phl_com_t* phl_com);

enum rtw_phl_status
phl_sw_cap_deinit(struct rtw_phl_com_t* phl_com);

enum rtw_phl_status
phl_init_protocol_cap(struct phl_info_t *phl_info,
                      struct rtw_wifi_role_t *wifi_role,
                      struct rtw_wifi_role_link_t *rlink);

enum rtw_phl_status
phl_init_role_cap(struct phl_info_t *phl_info,
                  struct rtw_wifi_role_t *wifi_role);

enum rtw_phl_status
phl_init_rlink_cap(struct phl_info_t *phl_info,
                   struct rtw_wifi_role_link_t *rlink);

void phl_init_proto_stbc_cap(struct rtw_wifi_role_link_t *rlink,
		struct phl_info_t *phl_info,
		struct protocol_cap_t *proto_role_cap);

void rtw_phl_init_free_para_buf(struct rtw_phl_com_t *phl_com);

#endif  /*_PHL_SW_CAP_H_*/
