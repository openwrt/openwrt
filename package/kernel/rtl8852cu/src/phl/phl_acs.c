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
#define _PHL_ACS_C_
#include "phl_headers.h"

#ifdef CONFIG_RTW_ACS
struct rtw_ch phl_acs_chnl_tbl[MAX_ACS_INFO] = {
	/* let invalid entry in the first */
	{BAND_MAX, 0},
	{BAND_ON_24G, 1},
	{BAND_ON_24G, 2},
	{BAND_ON_24G, 3},
	{BAND_ON_24G, 4},
	{BAND_ON_24G, 5},
	{BAND_ON_24G, 6},
	{BAND_ON_24G, 7},
	{BAND_ON_24G, 8},
	{BAND_ON_24G, 9},
	{BAND_ON_24G, 10},
	{BAND_ON_24G, 11},
	{BAND_ON_24G, 12},
	{BAND_ON_24G, 13},
	{BAND_ON_24G, 14},
	{BAND_ON_5G, 36},
	{BAND_ON_5G, 40},
	{BAND_ON_5G, 44},
	{BAND_ON_5G, 48},
	{BAND_ON_5G, 52},
	{BAND_ON_5G, 56},
	{BAND_ON_5G, 60},
	{BAND_ON_5G, 64},
	{BAND_ON_5G, 100},
	{BAND_ON_5G, 104},
	{BAND_ON_5G, 108},
	{BAND_ON_5G, 112},
	{BAND_ON_5G, 116},
	{BAND_ON_5G, 120},
	{BAND_ON_5G, 124},
	{BAND_ON_5G, 128},
	{BAND_ON_5G, 132},
	{BAND_ON_5G, 136},
	{BAND_ON_5G, 140},
	{BAND_ON_5G, 144},
	{BAND_ON_5G, 149},
	{BAND_ON_5G, 153},
	{BAND_ON_5G, 157},
	{BAND_ON_5G, 161},
	{BAND_ON_5G, 165},
	{BAND_ON_5G, 169},
	{BAND_ON_5G, 173},
	{BAND_ON_5G, 177},
	{BAND_ON_6G, 1},
	{BAND_ON_6G, 5},
	{BAND_ON_6G, 9},
	{BAND_ON_6G, 13},
	{BAND_ON_6G, 17},
	{BAND_ON_6G, 21},
	{BAND_ON_6G, 25},
	{BAND_ON_6G, 29},
	{BAND_ON_6G, 33},
	{BAND_ON_6G, 37},
	{BAND_ON_6G, 41},
	{BAND_ON_6G, 45},
	{BAND_ON_6G, 49},
	{BAND_ON_6G, 53},
	{BAND_ON_6G, 57},
	{BAND_ON_6G, 61},
	{BAND_ON_6G, 65},
	{BAND_ON_6G, 69},
	{BAND_ON_6G, 73},
	{BAND_ON_6G, 77},
	{BAND_ON_6G, 81},
	{BAND_ON_6G, 85},
	{BAND_ON_6G, 89},
	{BAND_ON_6G, 93},
	{BAND_ON_6G, 97},
	{BAND_ON_6G, 101},
	{BAND_ON_6G, 105},
	{BAND_ON_6G, 109},
	{BAND_ON_6G, 113},
	{BAND_ON_6G, 117},
	{BAND_ON_6G, 121},
	{BAND_ON_6G, 125},
	{BAND_ON_6G, 129},
	{BAND_ON_6G, 133},
	{BAND_ON_6G, 137},
	{BAND_ON_6G, 141},
	{BAND_ON_6G, 145},
	{BAND_ON_6G, 149},
	{BAND_ON_6G, 153},
	{BAND_ON_6G, 157},
	{BAND_ON_6G, 161},
	{BAND_ON_6G, 165},
	{BAND_ON_6G, 169},
	{BAND_ON_6G, 173},
	{BAND_ON_6G, 177},
	{BAND_ON_6G, 181},
	{BAND_ON_6G, 185},
	{BAND_ON_6G, 189},
	{BAND_ON_6G, 193},
	{BAND_ON_6G, 197},
	{BAND_ON_6G, 201},
	{BAND_ON_6G, 205},
	{BAND_ON_6G, 209},
	{BAND_ON_6G, 213},
	{BAND_ON_6G, 217},
	{BAND_ON_6G, 221},
	{BAND_ON_6G, 225},
	{BAND_ON_6G, 229},
	{BAND_ON_6G, 233},
};


void phl_acs_mntr_trigger(struct phl_info_t *phl_i, enum phl_band_idx band_idx,
				struct phl_acs_parm *parm)
{
	struct acs_mntr_parm mntr_parm = {0};

	if (parm->idx == 0 ||
		parm->idx >= MAX_ACS_INFO) {
		PHL_ERR("[ACS][%s] invalid idx:%d\n", __func__, parm->idx);
		return;
	}
	mntr_parm.mntr_time = parm->monitor_time;
	mntr_parm.nhm_incld_cca = parm->nhm_include_cca;

	rtw_hal_acs_mntr_trigger(phl_i->hal, band_idx, &mntr_parm);
}

void phl_acs_mntr_result(struct phl_info_t *phl_info,
			enum phl_band_idx band_idx, struct phl_acs_parm *parm)
{
	struct phl_acs_info *acs_info = (struct phl_acs_info *)phl_info->acs_info;

	if (parm->idx == 0 ||
		parm->idx >= MAX_ACS_INFO) {
		PHL_ERR("[ACS][%s] invalid idx:%d\n", __func__, parm->idx);
		return;
	}

	rtw_hal_acs_mntr_result(phl_info->hal, band_idx, &acs_info->rpt[parm->idx]);
}

enum rtw_phl_status phl_acs_info_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);

	PHL_INFO("==> phl_acs_info_init()\n");

	if (NULL == phl_info->acs_info) {
		phl_info->acs_info = _os_mem_alloc(drv,	sizeof(struct phl_acs_info));
		if (NULL == phl_info->acs_info) {
			PHL_WARN("phl_acs_info_init(): Failed to allocate phl_acs_info\n");
			goto exit;
		}
		_os_mem_set(phl_to_drvpriv(phl_info), phl_info->acs_info,
				0, sizeof(struct phl_acs_info));
	} else {
		PHL_WARN("phl_acs_info_init(): Duplicate init1, please check code\n");
	}

	pstatus = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_INFO("<== phl_acs_info_init(): pstatus:%d\n", pstatus);
	return pstatus;
}

void phl_acs_info_deinit(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);

	PHL_INFO("==> phl_acs_info_deinit()\n");

	if (NULL == phl_info->acs_info)
		goto exit;

	_os_mem_free(drv, phl_info->acs_info, sizeof(struct phl_acs_info));
	phl_info->acs_info = NULL;

exit:
	PHL_INFO("<== phl_acs_info_deinit()\n");
}

enum rtw_phl_status rtw_phl_get_acs_info(void *phl, struct rtw_acs_info_parm *parm)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_acs_info *acs_info = (struct phl_acs_info *)phl_info->acs_info;
	struct acs_mntr_rpt *rpt;

	if (parm->idx == 0 ||
		parm->idx >= MAX_ACS_INFO) {
		PHL_ERR("[ACS][%s] invalid idx:%d\n", __func__, parm->idx);
		return RTW_PHL_STATUS_INVALID_PARAM;
	}

	rpt = &acs_info->rpt[parm->idx];

	parm->band = (u8)phl_acs_chnl_tbl[parm->idx].band;
	parm->ch = phl_acs_chnl_tbl[parm->idx].ch;

	_os_mem_cpy(phl_to_drvpriv(phl_info), (u8 *)&parm->rpt,
		rpt, sizeof(struct acs_mntr_rpt));

	return RTW_PHL_STATUS_SUCCESS;
}

u8 rtw_phl_get_acs_chnl_tbl_idx(void *phl, enum band_type band, u8 channel)
{
	u8 idx;

	for (idx = 0; idx < MAX_ACS_INFO; idx++) {
		if (phl_acs_chnl_tbl[idx].band == band &&
			phl_acs_chnl_tbl[idx].ch == channel)
			break;
	}

	return (idx == MAX_ACS_INFO) ? 0 : idx;
}
#endif /* CONFIG_RTW_ACS */

void rtw_phl_get_env_rpt(void *phl,
                         struct rtw_env_report *env_rpt,
                         u8 hw_band)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);

	rtw_hal_env_rpt(hal_com, env_rpt, hw_band);

}
