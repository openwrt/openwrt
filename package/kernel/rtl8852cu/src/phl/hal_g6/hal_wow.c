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
#define _HAL_WOW_C_
#include "hal_headers.h"

#ifdef CONFIG_WOWLAN

#define MAX_POLLING_WOW_FW_STS_CNT 1000 /* 50ms */

static enum rtw_hal_status _hal_check_wow_fw_ready(void *hal, u8 func_en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;
	void* drv_priv = hal_to_drvpriv(hal_info);
	u8 fw_status = 0;
	u32 poll_cnt = 0;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : start polling... (func_en %d)\n", __func__, func_en);

	/* polling fw status */
	while (1) {

		if (poll_cnt >= MAX_POLLING_WOW_FW_STS_CNT) {
			PHL_ERR("%s polling fw status timeout !!!\n", __func__);
			hstats = RTW_HAL_STATUS_FAILURE;
			break;
		}

		hstats = rtw_hal_mac_get_wow_fw_status(hal_info, &fw_status,
							func_en);

		if (RTW_HAL_STATUS_SUCCESS != hstats) {

			PHL_ERR("%s : status %u\n", __func__, fw_status);
			break;
		}

		if (fw_status) {
			hstats = RTW_HAL_STATUS_SUCCESS;

			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
				"%s : polling count %u\n", __func__, poll_cnt);
			break;
		} else {

			_os_delay_us(drv_priv, 50);

		}
		poll_cnt++;

	}

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : finish polling... (func_en %d)\n", __func__, func_en);

	return hstats;
}

enum rtw_hal_status rtw_hal_get_wake_rsn(void *hal, enum rtw_mac_wow_wake_reason *wake_rsn, u8 *reset)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;
	u8 mac_rsn = 0;

	hstats = rtw_hal_mac_get_wake_rsn(hal_info, &mac_rsn, reset);
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s : wake rsn from mac in hex 0x%x (reset %d).\n",
				__func__, mac_rsn, *reset);

	if (RTW_HAL_STATUS_SUCCESS != hstats)
		PHL_ERR("%s : rtw_hal_mac_get_wake_rsn failed.\n", __func__);
	else
		*wake_rsn = mac_rsn;

	return hstats;
}

enum rtw_hal_status rtw_hal_cfg_wow_sleep(void *hal, u8 sleep)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;

	hstats = rtw_hal_mac_cfg_wow_sleep(hal_info, sleep);

	if (RTW_HAL_STATUS_SUCCESS != hstats)
		PHL_ERR("%s : sleep %u\n", __func__, sleep);

	return hstats;
}

enum rtw_hal_status rtw_hal_get_wow_aoac_rpt(void *hal, struct rtw_aoac_report *aoac_info, u8 rx_ready)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	FUNCIN();

	/* get aoac report */
	hstatus = rtw_hal_mac_get_aoac_rpt(hal_info, aoac_info, rx_ready);

	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s(): failed with status(%u)\n", __func__, hstatus);

	FUNCOUT();

	return hstatus;
}


enum rtw_hal_status rtw_hal_reset_pkt_ofld_state(void *hal)
{

	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstats = RTW_HAL_STATUS_FAILURE;

	hstats = rtw_hal_mac_reset_pkt_ofld_state(hal_info);

	if (RTW_HAL_STATUS_SUCCESS != hstats)
		PHL_ERR("%s : failed \n", __func__);

	return hstats;
}

#ifdef CONFIG_PCI_HCI
enum rtw_hal_status rtw_hal_wow_cfg_txdma(void *hal, u8 en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_trx_ops *trx_ops = hal_info->trx_ops;

	PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s : enable %d.\n", __func__, en);

	trx_ops->cfg_wow_txdma(hal_info, en);

	return RTW_HAL_STATUS_SUCCESS;
}
#endif

enum rtw_hal_status
rtw_hal_wow_cfg_nlo(void *hal, enum SCAN_OFLD_OP op, u16 mac_id,
                    u8 hw_band, u8 hw_port, struct rtw_nlo_info *cfg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct scan_ofld_info info = {0};
	u64 tsf;
	u32 tsf_h = 0, tsf_l = 0;

	info.operation = op;

	if (op == SCAN_OFLD_OP_RPT || op == SCAN_OFLD_OP_STOP)
		goto end;

	if (cfg->delay != 0) {
		hstatus = rtw_hal_mac_get_tsf(hal, hw_band, hw_port, &tsf_h, &tsf_l);
		if (hstatus == RTW_HAL_STATUS_SUCCESS) {
			tsf = ((u64)tsf_h << 32) | tsf_l;
			tsf += cfg->delay * 1000;
			tsf_h = tsf >> 32;
			tsf_l = (u32)tsf;
			info.tsf_high = tsf_h;
			info.tsf_low = tsf_l;
		} else {
			PHL_WARN("%s Getting TSF failed \n", __func__);
		}

	}
	/* configure scan offload */
	info.period = cfg->period;
	info.slow_period = cfg->slow_period;
	info.cycle = cfg->cycle;

	if (info.period == 0 && info.slow_period == 0) {
		info.mode = SCAN_OFLD_MD_ONCE;
	} else if (info.period == 0) {
		info.mode = SCAN_OFLD_MD_SEEMLESS;
	} else if (info.slow_period == 0) {
		info.mode = SCAN_OFLD_MD_PD;
	} else {
		info.mode = SCAN_OFLD_MD_PD_SLOW;
	}

end:

	hstatus = rtw_hal_mac_scan_ofld(hal_info, (u8)mac_id,
				hw_band, hw_port, &info);

	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		return hstatus;

	return hstatus;
}

enum rtw_hal_status
rtw_hal_wow_cfg_nlo_chnl_list(void *hal, struct rtw_nlo_info *cfg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	u8 i = 0;

	if (cfg->channel_num == 0)
		return RTW_HAL_STATUS_FAILURE;

	for (i = 0; i < cfg->channel_num; i++) {
		/* set channel to mac */
		cfg->channel_list[i].probe_req_id = cfg->probe_req_id;
		hstatus = rtw_hal_mac_scan_ofld_add_ch(hal_info, HW_BAND_0,
						&cfg->channel_list[i],
						i == (cfg->channel_num-1) ? true : false);
		if (hstatus != RTW_HAL_STATUS_SUCCESS)
			return hstatus;
	}

	return hstatus;
}

enum rtw_hal_status rtw_hal_wow_init(struct rtw_phl_com_t *phl_com, void *hal,
                                     struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	FUNCIN_WSTS(hal_status);

	/* download wowlan fw and do related tasks needed after redownload fw */
	hal_status = hal_ops->hal_wow_init(phl_com, hal_info, sta);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return hal_status;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s successfully done.\n", __func__);

	FUNCOUT_WSTS(hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_wow_deinit(struct rtw_phl_com_t *phl_com, void *hal,
                                       struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);

	FUNCIN_WSTS(hal_status);

	/* download wowlan fw and do related tasks needed after redownload fw */
	hal_status = hal_ops->hal_wow_deinit(phl_com, hal_info, sta);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return hal_status;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s successfully done.\n", __func__);

	FUNCOUT_WSTS(hal_status);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_wow_func_en(struct rtw_phl_com_t *phl_com, void *hal, u16 macid,
                    struct rtw_hal_wow_cfg *cfg)
{

	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	FUNCIN();

	do {
		/* config wow cam : pattern match */
#ifndef RTW_WKARD_WOW_SKIP_WOW_CAM_CONFIG
		hstatus = rtw_hal_mac_cfg_wow_cam(hal_info, macid, true,
		                                  cfg->pattern_match_info);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			break;
#endif
		/* gtk offload */
		if (cfg->gtk_ofld_cfg->gtk_en) {
			hstatus = rtw_hal_mac_cfg_gtk_ofld(hal_info, macid, true,
			                                   cfg->gtk_ofld_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* arp offload */
		if (cfg->arp_ofld_cfg->arp_en) {
			hstatus = rtw_hal_mac_cfg_arp_ofld(hal_info, macid, true,
			                                   cfg->arp_ofld_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* ndp offload */
		if (cfg->ndp_ofld_cfg->ndp_en) {
			hstatus = rtw_hal_mac_cfg_ndp_ofld(hal_info, macid, true,
			                                   cfg->ndp_ofld_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* config keep alive */
		if (cfg->keep_alive_cfg->keep_alive_en) {
			hstatus = rtw_hal_mac_cfg_keep_alive(hal_info, macid, true,
			                                     cfg->keep_alive_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* config disconnect detection */
		if (cfg->disc_det_cfg->disc_det_en) {
			hstatus = rtw_hal_mac_cfg_disc_dec(hal_info, macid, true,
			                                   cfg->disc_det_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* realwow offload */
		if (cfg->realwow_cfg->realwow_en) {
			hstatus = rtw_hal_mac_cfg_realwow(hal_info, macid, true,
			                                  cfg->realwow_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* config gpio */
		if (cfg->wow_gpio->d2h_gpio_info.dev2hst_gpio_en) {
			hstatus = rtw_hal_mac_cfg_dev2hst_gpio(hal_info, true, cfg->wow_gpio);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
		/* network list offload */
		if (cfg->nlo_cfg->nlo_en) {
			hstatus = rtw_hal_mac_cfg_nlo(hal_info, macid, true, cfg->nlo_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}

		/* config period wake */
		if (cfg->periodic_wake_cfg->periodic_wake_en) {
			hstatus = rtw_hal_mac_cfg_periodic_wake(hal_info, macid, true,
								cfg->periodic_wake_cfg);
			if (RTW_HAL_STATUS_SUCCESS != hstatus)
				break;
		}
	} while(0);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s status(%u).\n", __func__, hstatus);

	FUNCOUT();

	return hstatus;
}

enum rtw_hal_status
rtw_hal_wow_func_dis(struct rtw_phl_com_t *phl_com, void *hal, u16 macid,
                     struct rtw_hal_wow_cfg *cfg)
{

	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	FUNCIN();

	/* cancel pattern match */
#ifndef RTW_WKARD_WOW_SKIP_WOW_CAM_CONFIG
	hstatus = rtw_hal_mac_cfg_wow_cam(hal_info, macid, false, NULL);
	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_wow_cam failed \n");
#endif
	/* cancel gtk offload */
	if (cfg->gtk_ofld_cfg->gtk_en) {
		hstatus = rtw_hal_mac_cfg_gtk_ofld(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_gtk_ofld failed \n");
	}
	/* cancel arp offload */
	if (cfg->arp_ofld_cfg->arp_en) {
		hstatus = rtw_hal_mac_cfg_arp_ofld(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_arp_ofld failed \n");
	}
	/* cancel ndp offload */
	if (cfg->ndp_ofld_cfg->ndp_en) {
		hstatus = rtw_hal_mac_cfg_ndp_ofld(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_ndp_ofld failed \n");
	}
	/* disable keep alive */
	if (cfg->keep_alive_cfg->keep_alive_en) {
		hstatus = rtw_hal_mac_cfg_keep_alive(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_keep_alive failed \n");
	}
	/* disable disconect detection */
	if (cfg->disc_det_cfg->disc_det_en) {
		hstatus = rtw_hal_mac_cfg_disc_dec(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_disc_dec failed \n");
	}
	/* realwow offload */
	if (cfg->realwow_cfg->realwow_en) {
		hstatus = rtw_hal_mac_cfg_realwow(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_realwow_ofld failed \n");
	}
	/* config gpio */
	if (cfg->wow_gpio->d2h_gpio_info.dev2hst_gpio_en) {
		hstatus = rtw_hal_mac_cfg_dev2hst_gpio(hal_info, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_dev2hst_gpio failed \n");
	}
	/* nlo */
	if (cfg->nlo_cfg->nlo_en) {
		hstatus = rtw_hal_mac_cfg_nlo(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_nlo failed \n");
	}
	/* config period wake */
	if (cfg->periodic_wake_cfg->periodic_wake_en) {
		hstatus = rtw_hal_mac_cfg_periodic_wake(hal_info, macid, false, NULL);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_periodic_wake failed \n");
	}
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s status(%u).\n", __func__, hstatus);

	FUNCOUT();

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_wow_func_start(struct rtw_phl_com_t *phl_com, void *hal, u16 macid, struct rtw_hal_wow_cfg *cfg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	do {
		hstatus = rtw_hal_mac_cfg_wow_wake(hal_info, macid, true, cfg->wow_wake_cfg);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			break;
		/* poll fw status */
		hstatus = _hal_check_wow_fw_ready(hal_info, 1);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			break;
	} while(0);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s status(%u).\n", __func__, hstatus);

	return hstatus;
}

enum rtw_hal_status rtw_hal_wow_func_stop(struct rtw_phl_com_t *phl_com, void *hal, u16 macid)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	/* config wow ctrl */
	hstatus = rtw_hal_mac_cfg_wow_wake(hal_info, macid, false, NULL);
	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] rtw_hal_mac_cfg_wow_wake failed \n");

	hstatus = _hal_check_wow_fw_ready(hal_info, 0);
	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] _hal_poll_wow_fw_status failed \n");

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s status(%u).\n", __func__, hstatus);

	return hstatus;
}

enum rtw_hal_status rtw_hal_set_wowlan(struct rtw_phl_com_t *phl_com, void *hal, u8 enter)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	hal_status = rtw_hal_mac_set_wowlan(hal_info, enter);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s : status(%u).\n", __func__, hal_status);

	return hal_status;
}

static enum rtw_hal_status _wow_chk_txq_empty(struct hal_info_t *hal_info, u8 *empty)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_wow_chk_txq_empty(hal_info, empty);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s : status(%u).\n", __func__, hal_status);

	return hal_status;
}

static enum rtw_hal_status _wow_wde_drop(struct hal_info_t *hal_info, u8 band)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_wow_wde_drop(hal_info, band);

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s : status(%u).\n", __func__, hal_status);

	return hal_status;
}

#define MAX_WOW_DROP_HWTX_TRYCNT 3
enum rtw_hal_status rtw_hal_wow_drop_tx(void *hal, u8 band)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 empty = 0;
	u8 i = 0;

	for (i = 0; i < MAX_WOW_DROP_HWTX_TRYCNT; i++) {

		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : chk/drop for the %d time\n", __func__, i + 1);

		if (RTW_HAL_STATUS_SUCCESS != _wow_chk_txq_empty(hal_info, &empty))
			PHL_WARN("[wow] _wow_chk_txq_empty failed.\n");

		/* force drop wde if txq is not empty */
		if (!empty)
			_wow_wde_drop(hal_info, band);
		else
			break;
	}

	if (!empty) {
		PHL_WARN("[wow] %s : chk/drop fail!\n", __func__);
	} else {
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s : chk/drop ok.\n", __func__);
		hal_status = RTW_HAL_STATUS_SUCCESS;
	}

	return hal_status;
}

#endif /* CONFIG_WOWLAN */
