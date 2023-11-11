/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#ifndef _PHL_REGULATION_6G_H_
#define _PHL_REGULATION_6G_H_

bool regu_set_domain_6g(void *phl, u8 domain, enum regulation_rsn reason);
void regu_get_chnlplan_6g(struct rtw_regulation *rg,
				enum rtw_regulation_query type,
				struct rtw_regulation_chplan *plan);
bool regu_valid_domain_6g(u8 domain);
bool regu_query_domain_6g_channel(u8 domain, enum band_type band, u8 ch,
			enum ch_property *prop);
u8 regu_get_domain_regulation_6g(u8 domain);
void regu_convert_ch6g(u8 unii_6g, struct rtw_regulation *rg,
				u32 *ch_cnt, struct rtw_regulation_channel *rch,
				u32 ch, u32 passive, u8 max_num, u8 ch_start);

#endif /* _PHL_REGULATION_6G_H_ */
