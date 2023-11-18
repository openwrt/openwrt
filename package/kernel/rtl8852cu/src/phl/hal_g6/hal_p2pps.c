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
#define _HAL_P2PPS_C_
#include "hal_headers.h"

#ifdef CONFIG_PHL_P2PPS
#define TSF32_TOG_EARLY_T 2000 /*2ms*/

enum rtw_hal_status rtw_hal_noa_enable(void *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid)
{
	enum rtw_hal_status ret = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 cur_en_desc_num = noa_info->en_desc_num;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NoA]%s: macid(0x%x), cur_en_desc_num(%d)\n",
		__func__, macid, cur_en_desc_num);
	if (cur_en_desc_num == 0) {
		ret = rtw_hal_mac_noa_init(hal_info, noa_info, in_desc, macid);
	} else {
		ret = rtw_hal_mac_noa_update(hal_info, noa_info, in_desc, macid);
	}
	return ret;
}

enum rtw_hal_status rtw_hal_noa_disable(void *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid)
{
	enum rtw_hal_status ret = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 cur_en_desc_num = noa_info->en_desc_num;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NoA]%s(): cur_en_desc_num(%d)\n",
		__func__, cur_en_desc_num);
	if (cur_en_desc_num == 1) {
		ret = rtw_hal_mac_noa_terminate(hal_info, noa_info, in_desc,
						macid);
	} else {
		ret = rtw_hal_mac_noa_remove(hal_info, noa_info, in_desc, macid);
	}
	return ret;
}

enum rtw_hal_status
rtw_hal_tsf32_tog_enable(void *hal,
                         struct rtw_wifi_role_link_t *rlink)
{
	enum rtw_hal_status h_stat = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = hal;

	h_stat = rtw_hal_mac_tsf32_tog_enable(hal_info,
	                                      rlink->hw_band,
	                                      rlink->hw_port,
	                                      TSF32_TOG_EARLY_T);

	return h_stat;
}

enum rtw_hal_status
rtw_hal_tsf32_tog_disable(void *hal,
                         struct rtw_wifi_role_link_t *rlink)
{
	enum rtw_hal_status h_stat = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = hal;

	h_stat = rtw_hal_mac_tsf32_tog_disable(hal_info,
	                                       rlink->hw_band,
	                                       rlink->hw_port);

	return h_stat;
}

enum rtw_hal_status rtw_hal_get_tsf32_tog_rpt(void *hal,
	struct rtw_phl_tsf32_tog_rpt *rpt)
{
	struct hal_info_t *hal_info = hal;

	return rtw_hal_mac_get_tsf32_tog_rpt(hal_info, rpt);
}

enum rtw_hal_status rtw_hal_noa_sta_macid_up(void *hal,
			u16 wrole_macid, bool join, u8 *bitmap, u32 bitmap_len)
{
	return rtw_hal_mac_p2p_macid_ctrl(hal, join, wrole_macid, bitmap,
						bitmap_len);
}

#endif

