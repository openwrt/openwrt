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
#define _HAL_BTC_C_
#include "../hal_headers_le.h"
#include "../hal_headers.h"
#include "hal_btc.h"
#include "halbtc_fw.h"
#include "halbtc_def.h"
#include "halbtc_action.h"

#ifdef CONFIG_BTCOEX

/* [31:24] -> main-version
 * [23:16] -> sub-version
 * [15:8]  -> Hot-Fix-version
 * [7:0]   -> branch ID, ex: 0x00-> Main-Branch
 * Modify bt_8852x.c chip_8852x member: btcx_desired, wlcx_desired if required
 * btcx_desired: BT FW coex version -> main-version + 1 if update.
 * wlcx_desired: WL FW coex version -> sub-version + 1 if update
 */
const u32 coex_ver = 0x07040413;

static struct btc_ops _btc_ops = {
	_send_fw_cmd,
	_ntfy_power_on,
	_ntfy_power_off,
	_ntfy_init_coex,
	_ntfy_scan_start,
	_ntfy_scan_finish,
	_ntfy_switch_band,
	_ntfy_specific_packet,
	_ntfy_role_info,
	_ntfy_radio_state,
	_ntfy_customerize,
	_ntfy_wl_rfk,
	_ntfy_wl_sta,
	_ntfy_fwinfo,
	_ntfy_timer
};

#define _update_dbcc_band(phy_idx) \
	btc->cx.wl.dbcc_info.real_band[phy_idx] =\
	(btc->cx.wl.scan_info.phy_map & BIT(phy_idx) ?\
	btc->cx.wl.dbcc_info.scan_band[phy_idx] :\
	btc->cx.wl.dbcc_info.op_band[phy_idx])

/******************************************************************************
 *
 * coex internal functions
 *
 *****************************************************************************/
static void _set_btc_timer(struct btc_t *btc, u16 tmr_id, u32 ms)
{
	struct btc_tmr *btmr = NULL;

	if (tmr_id < BTC_TIMER_MAX) {
		btmr = &btc->timer[tmr_id];
		_os_set_timer(halcom_to_drvpriv(btc->hal), &btmr->tmr, ms);
	}
}

static void _btmr_stop(struct btc_t *btc)
{
	struct btc_tmr *btmr = NULL;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s(): stop btc timers!!\n",
		  __func__);

	btc->tmr_stop = true;

	for (i = 0; i < BTC_TIMER_MAX; i++) {
		btmr = &btc->timer[i];
		_os_cancel_timer(halcom_to_drvpriv(btc->hal), &btmr->tmr);
	}
	_os_cancel_timer(halcom_to_drvpriv(btc->hal), &btc->delay_tmr);
}

static void _btmr_release(struct btc_t *btc)
{
	struct btc_tmr *btmr = NULL;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): release btc timers!!\n", __func__);

	for (i = 0; i < BTC_TIMER_MAX; i++) {
		btmr = &btc->timer[i];
		_os_release_timer(halcom_to_drvpriv(btc->hal), &btmr->tmr);
	}
	_os_release_timer(halcom_to_drvpriv(btc->hal), &btc->delay_tmr);
}

static void _btmr_start(struct btc_t *btc)
{
	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): start btc periodic timer!!\n", __func__);

	btc->tmr_stop = false;

	/* Wait 2 sec for phl starting then periodic timer will be started */
	_os_set_timer(halcom_to_drvpriv(btc->hal),
			&btc->delay_tmr, BTC_DELAYED_PERIODIC_TIME);
}

static void _delay_tmr_cb(void *ctx)
{
	struct btc_t *btc = NULL;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s() !!\n", __func__);

	if (!ctx)
		return;

	btc = (struct btc_t *)ctx;
	_set_btc_timer(btc, BTC_TIMER_PERIODIC, BTC_PERIODIC_TIME);
}

static void _btmr_cb(void *ctx)
{
	struct btc_tmr *btmr = NULL;
	struct btc_t *btc = NULL;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s(), ctx = 0x%p !!\n",
		  __func__, ctx);

	if (!ctx)
		return;

	btmr = (struct btc_tmr *)ctx;
	btc = (struct btc_t *)btmr->btc;

	if (!btc->tmr_init || btc->tmr_stop)
		return;

	hal_btc_send_event(btc,  btc->cx.wl.pta_req_mac, (u8 *)btmr,
			   sizeof(struct btc_tmr *),  BTC_HMSG_TMR_EN);
}

static void _btmr_init(struct btc_t *btc)
{
	struct btc_tmr *btmr = NULL;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s(): !!\n", __func__);

	for (i = 0; i < BTC_TIMER_MAX; i++) {
		btmr = &btc->timer[i];
		btmr->btc = btc;
		btmr->id = i;
		PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
			  "[BTC], init btc timer(%d) = 0x%p !!\n", i, btmr);
		_os_init_timer(halcom_to_drvpriv(btc->hal), &btmr->tmr,
			       _btmr_cb, btmr, "btc_tmr");
	}

	_os_init_timer(halcom_to_drvpriv(btc->hal), &btc->delay_tmr,
		       _delay_tmr_cb, btc, NULL);

	btc->tmr_init = true;
	btc->tmr_stop = true;
}

static void _btmr_deinit(struct btc_t *btc)
{
	if (btc->tmr_init) {
		_btmr_stop(btc);
		_btmr_release(btc);
		btc->tmr_init = false;
	}
}

static void
_send_fw_cmd(struct btc_t *btc, u8 h2c_class, u8 h2c_func, u8 *param, u16 len)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct rtw_g6_h2c_hdr hdr = {0};

	hdr.h2c_class = h2c_class;
	hdr.h2c_func = h2c_func;
	hdr.type = H2CB_TYPE_DATA;
	hdr.content_len = len;
	hdr.done_ack = 1;

	if (!wl->status.map.init_ok) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_ERR_,
			  "[BTC], %s(): return by btc not init!!\n", __func__);
		btc->fwinfo.cnt_h2c_fail++;
		return;
	}

	if ((wl->status.map.rf_off_pre == 1 && wl->status.map.rf_off == 1) ||
	    (wl->status.map.lps_pre == BTC_LPS_RF_OFF &&
	     wl->status.map.lps == BTC_LPS_RF_OFF)) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_ERR_,
			  "[BTC], %s(): return by wl off!!\n", __func__);
		btc->fwinfo.cnt_h2c_fail++;
		return;
	}

	btc->fwinfo.cnt_h2c++;

	if (rtw_hal_mac_send_h2c(h, &hdr, (u32 *)param) != 0) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
			  "[BTC], %s(): return by rtw_hal_mac_send_h2c!!\n",
			  __func__);
		btc->fwinfo.cnt_h2c_fail++;
	}
}

u32 _read_cx_reg(struct btc_t *btc, u32 offset)
{
	u32 val = 0;
	rtw_hal_mac_coex_reg_read(btc->hal, offset, &val);

	return val;
}

u32 _read_cx_ctrl(struct btc_t *btc)
{
	u32 val = 0;
	rtw_hal_mac_get_coex_ctrl(btc->hal, &val);

	return val;
}

u32 _read_scbd(struct btc_t *btc)
{
	const struct btc_chip *chip = btc->chip;
	u32 scbd_val = 0;

	if (!chip->scbd)
		return 0;

	btc->cx.cnt_bt[BTC_BCNT_SCBDREAD]++;
	rtw_hal_mac_get_scoreboard(btc->hal, &scbd_val);
	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], read scbd : 0x%08x \n", scbd_val);
	return scbd_val;
}

void _write_scbd(struct btc_t *btc, u32 val, bool state)
{
	const struct btc_chip *chip = btc->chip;
	struct btc_wl_info *wl = &btc->cx.wl;
	u32 scbd_val = 0;
	u8 force_exec = false;

	if (!chip->scbd)
		return;

	/* only use bit23~0 */
	scbd_val = (state ? (wl->scbd | val) : (wl->scbd & (~val)));

	if (val & BTC_WSCB_ACTIVE || val & BTC_WSCB_ON)
		force_exec = true;

	/* Just update wl->scbd and set wl->scbd_change,
	 * moved "Write scoreboard I/O" to  _action_common()
	 * _write_scbd will be executed if run_coex()
	 */
	if (scbd_val != wl->scbd || force_exec) {
		wl->scbd = scbd_val;
		wl->scbd_change = 1;
	}
}

static u8
_update_rssi_state(struct btc_t *btc, u8 pre_state, u8 rssi, u8 thresh)
{
	u8 next_state, tol = btc->chip->rssi_tol;

	if (pre_state == BTC_RSSI_ST_LOW ||
	    pre_state == BTC_RSSI_ST_STAY_LOW) {
		if (rssi >= (thresh + tol))
			next_state = BTC_RSSI_ST_HIGH;
		else
			next_state = BTC_RSSI_ST_STAY_LOW;
	} else {
		if (rssi < thresh)
			next_state = BTC_RSSI_ST_LOW;
		else
			next_state = BTC_RSSI_ST_STAY_HIGH;
	}

	return next_state;
}

void _write_bt_reg(struct btc_t *btc, u8 reg_type, u16 addr, u32 val)
{
	u8 buf[4] = {0};

	/* set write address */
	buf[0] = reg_type;
	buf[1] = addr & bMASKB0;
	buf[2] = (addr & bMASKB1) >> 8;
	hal_btc_fw_set_bt(btc, SET_BT_WREG_ADDR, 3, buf);

	/* set write value */
	buf[0] = val & bMASKB0;
	buf[1] = (val & bMASKB1) >> 8;
	buf[2] = (val & bMASKB2) >> 16;
	buf[3] = (val & bMASKB3) >> 23;
	hal_btc_fw_set_bt(btc, SET_BT_WREG_VAL, 4, buf);
}

void _read_bt_reg(struct btc_t *btc, u8 reg_type, u16 addr)
{
	/* this function is only for API call.
	 * If BTC should use hal_btc_fw_set_monreg to read bt reg.
	 */
	u8 buf[3] = {0};

	/* set write address */
	buf[0] = reg_type;
	buf[1] = addr & bMASKB0;
	buf[2] = (addr & bMASKB1) >> 8;
	hal_btc_fw_set_bt(btc, SET_BT_RREG_ADDR, sizeof(buf), buf);

	/* To do wait FW event -> BTF_EVNT_BT_REG*/
}

static void _set_bt_query_dev_list(struct btc_t *btc, u8 sub_id)
{
	u8 buf = 0;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): query bt dev info\n", __func__);

	buf = sub_id;
	hal_btc_fw_set_bt(btc, SET_BT_QUERY_DEV_LIST, 1, &buf);
}

static void _set_bt_query_dev_info(struct btc_t *btc, u8 sub_id, u8 handle)
{
	u8 buf[2] = {0};

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): query bt dev info\n", __func__);

	buf[0] = sub_id;
	buf[1] = handle;
	hal_btc_fw_set_bt(btc, SET_BT_QUERY_DEV_INFO, 2, buf);
}

static void _reset_btc_var(struct btc_t *btc, u8 type)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_cx *cx = &btc->cx;
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_bt_info *bt = &cx->bt;
	struct btc_bt_link_info *bt_linfo = &bt->link_info;
	struct btc_wl_link_info *wl_linfo = wl->link_info;
	u8 i;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s()\n", __func__);

	/* Reset Coex variable */
	if (type & BTC_RESET_CX)
		hal_mem_set(h, cx, 0, sizeof(struct btc_cx));
	else if (type & BTC_RESET_BTINFO) /* only for BT enable */
		hal_mem_set(h, bt, 0, sizeof(struct btc_bt_info));

	hal_mem_set(h, &wl->rf_para, 0xfe, sizeof(struct btc_rf_para));
	hal_mem_set(h, &bt->rf_para, 0xfe, sizeof(struct btc_rf_para));

	if (type & BTC_RESET_CTRL)
		hal_mem_set(h, &btc->ctrl, 0, sizeof(struct btc_ctrl));

	/* Init Coex variables that are not zero */
	if (type & BTC_RESET_DM) {
		hal_mem_set(h, dm, 0, sizeof(struct btc_dm));
		hal_mem_set(h, bt_linfo->rssi_state, 0, BTC_BT_RSSI_THMAX);

		for (i = 0; i < BTC_WL_MAX_ROLE_NUMBER; i++)
			hal_mem_set(h, wl_linfo[i].rssi_state, 0,
				    BTC_WL_RSSI_THMAX);

		/* set the slot_now table to original */
		_tdma_cpy(&dm->tdma_now, &t_def[CXTD_OFF]);
		_tdma_cpy(&dm->tdma, &t_def[CXTD_OFF]);
		_slots_cpy(dm->slot_now, s_def);
		_slots_cpy(dm->slot, s_def);

		hal_mem_cpy(h, &dm->fddt_info.train.t_ctrl, &ft_tctrl_def,
			    sizeof(ft_tctrl_def));
		hal_mem_cpy(h, &dm->fddt_info.train.b_chk, &ft_bchk_def,
			    sizeof(ft_bchk_def));
		hal_mem_cpy(h, &dm->fddt_info.train.f_chk, &ft_fchk_def,
			    sizeof(ft_fchk_def));

		hal_mem_cpy(h, dm->fddt_info.train.cell_ul, cell_ul_def,
			    sizeof(cell_ul_def));
		hal_mem_cpy(h, dm->fddt_info.train.cell_dl, cell_dl_def,
			    sizeof(cell_dl_def));

		btc->policy_len = 0;
		btc->bt_req_len[HW_PHY_0] = 0;
		btc->bt_req_len[HW_PHY_1] = 0;
		btc->hubmsg_cnt = 0;

		dm->coex_info_map = BTC_COEX_INFO_ALL;
		dm->wl_tx_limit.tx_time = BTC_MAX_TX_TIME_DEF;
		dm->wl_tx_limit.tx_retry = BTC_MAX_TX_RETRY_DEF;
		dm->fddt_info.type = BTC_FDDT_TYPE_AUTO;
	}

	if (type & BTC_RESET_BT_PSD_DM) {
		hal_mem_set(h, &btc->bt_psd_dm, 0,
			    sizeof(struct btc_bt_psd_dm));
		btc->bt_psd_dm.aiso_method = BTC_AISO_M_ALL;
		btc->bt_psd_dm.aiso_method_final = BTC_AISO_M46_AVG;
		btc->bt_psd_dm.wl_ch_last = 0xFF;
	}

	if (type & BTC_RESET_MDINFO)
		hal_mem_set(h, &btc->mdinfo, 0, sizeof(struct btc_module));
}

static bool _chk_wl_rfk_request(struct btc_t *btc)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_bt_info *bt = &cx->bt;

	_update_bt_scbd(btc, true);

	cx->cnt_wl[BTC_WCNT_RFK_REQ]++;

	if ((bt->rfk_info.map.run || bt->rfk_info.map.req) &&
	    (!bt->rfk_info.map.timeout)) {
		cx->cnt_wl[BTC_WCNT_RFK_REJECT]++;
		return BTC_WRFK_REJECT;
	} else {
		cx->cnt_wl[BTC_WCNT_RFK_GO]++;
		return BTC_WRFK_ALLOW;
	}
}

void _set_fddt_cell_by_antiso(struct btc_t *btc)
{
#ifdef BTC_FDDT_TRAIN_SUPPORT

	struct rtw_hal_com_t *h = btc->hal;
	struct btc_dm *dm = &btc->dm;
	u8 ant_iso_id = 0;
	/* for uplink -> Ant_iso: 25/30/35/40 x WL-RSSI for BT-RSSI-level0 */
	struct btc_fddt_cell cell_ul_bt0[4][5] = {
	{{0,0,0,0},   {0,0,0,0},   {0,0,0,0},   {-2,8,10,4}, {-5,5,10,4}},
	{{0,0,0,0},   {0,0,0,0},   {0,0,0,0},   {-2,8,10,4}, {-5,5,10,4}},
	{{0,0,0,0},   {0,0,0,0},   {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{0,0,0,0},   {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}} };

	struct btc_fddt_cell cell_ul_bt1[4][5] = {
	{{0,0,0,0},   {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{0,0,0,0},   {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{5,15,10,4}, {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{5,15,10,4}, {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}} };

	struct btc_fddt_cell cell_ul_bt2[4][5] = {
	{{15,15,10,4}, {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{15,15,10,4}, {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{5,15,10,4},  {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}},
	{{5,15,10,4},  {5,15,10,4}, {3,13,10,4}, {-2,8,10,4}, {-5,5,10,4}} };

	struct btc_fddt_cell cell_dl_iso25[5][5] = { /* BT-RSSI x WL-RSSI */
	{{0,0,0,0},    {0,0,0,0},    {0,0,0,0},    {0,0,0,0},   {0,10,15,6}},
	{{10,15,20,5}, {10,15,20,5}, {5,15,20,5},  {0,10,20,5}, {-5,5,20,5}},
	{{10,15,25,5}, {10,15,25,5}, {10,15,25,5}, {5,15,25,5}, {0,10,25,5}},
	{{10,15,25,4}, {10,15,25,4}, {10,15,25,5}, {5,15,25,5}, {0,10,25,5}},
	{{10,15,30,4}, {10,15,30,4}, {10,15,30,4}, {5,15,30,5}, {0,10,30,5}} };

	struct btc_fddt_cell cell_dl_iso30[5][5] = { /* BT-RSSI x WL-RSSI */
	{{0,0,0,0},    {0,0,0,0},    {0,0,0,0},    {5,15,15,6},  {5,15,15,6}},
	{{10,15,20,5}, {10,15,20,5}, {5,15,20,5},  {0,10,20,5},  {-5,5,20,5}},
	{{10,15,25,5}, {10,15,25,5}, {10,15,25,5}, {10,15,25,5}, {5,15,25,5}},
	{{10,15,25,5}, {10,15,25,5}, {10,15,25,5}, {5,15,25,5},  {5,15,25,5}},
	{{10,15,25,4}, {10,15,25,4}, {10,15,25,4}, {10,15,25,5}, {5,15,25,5}} };

	struct btc_fddt_cell cell_dl_iso35[5][5] = { /* BT-RSSI x WL-RSSI */
	{{0,0,0,0},    {0,0,0,0},    {5,15,15,7},  {5,15,15,7},  {5,15,15,7}},
	{{10,15,15,5}, {10,15,15,5}, {5,15,15,5},  {0,10,15,5},  {-5,5,15,5}},
	{{10,15,20,5}, {10,15,20,5}, {10,15,20,5}, {10,15,20,5}, {5,15,20,5}},
	{{10,15,20,5}, {10,15,20,5}, {10,15,20,5}, {5,15,20,5},  {5,15,20,5}},
	{{10,15,20,4}, {10,15,20,4}, {10,15,20,4}, {10,15,20,5}, {5,15,20,5}} };

	struct btc_fddt_cell cell_dl_iso40[5][5] = { /* BT-RSSI x WL-RSSI */
	{{0,0,0,0},    {10,15,10,7}, {10,15,10,7}, {5,15,10,7},  {5,15,10,7}},
	{{10,15,15,5}, {10,15,15,5}, {5,15,15,5},  {0,10,15,5},  {-5,5,15,5}},
	{{10,15,20,5}, {10,15,20,5}, {10,15,20,5}, {10,15,20,5}, {5,15,20,5}},
	{{10,15,20,5}, {10,15,20,5}, {10,15,20,5}, {5,15,20,5},  {5,15,20,5}},
	{{10,15,20,4}, {10,15,20,4}, {10,15,20,4}, {10,15,20,5}, {5,15,20,5}} };

        if (btc->mdinfo.ant.isolation <= 20) {
		hal_mem_cpy(h, dm->fddt_info.train.cell_ul, cell_ul_def,
			    sizeof(cell_ul_def));
		hal_mem_cpy(h, dm->fddt_info.train.cell_dl, cell_dl_def,
			    sizeof(cell_dl_def));
		return;
        }

	if (btc->mdinfo.ant.isolation <= 25)
		ant_iso_id = 0;
        else if (btc->mdinfo.ant.isolation <= 30)
		ant_iso_id = 1;
	else if (btc->mdinfo.ant.isolation <= 35)
		ant_iso_id = 2;
	else
		ant_iso_id = 3;

	/* set uplink cell if BT-RSSI-Level-0 for Ant_iso: 25/30/35/40 */
	hal_mem_cpy(h, dm->fddt_info.train.cell_ul[0], cell_ul_bt0[ant_iso_id],
		    5 * sizeof(struct btc_fddt_cell));
	hal_mem_cpy(h, dm->fddt_info.train.cell_ul[1], cell_ul_bt1[ant_iso_id],
		    5 * sizeof(struct btc_fddt_cell));
	hal_mem_cpy(h, dm->fddt_info.train.cell_ul[2], cell_ul_bt2[ant_iso_id],
		    5 * sizeof(struct btc_fddt_cell));

        /* set downlink cell for Ant_iso: 25/30/35/40 */
	switch(ant_iso_id) {
	case 0:
            hal_mem_cpy(h, dm->fddt_info.train.cell_dl, cell_dl_iso25,
			sizeof(cell_dl_iso25));
	    break;
	case 1:
            hal_mem_cpy(h, dm->fddt_info.train.cell_dl, cell_dl_iso30,
			sizeof(cell_dl_iso30));
	    break;
	case 2:
            hal_mem_cpy(h, dm->fddt_info.train.cell_dl, cell_dl_iso35,
			sizeof(cell_dl_iso35));
	    break;
	case 3:
            hal_mem_cpy(h, dm->fddt_info.train.cell_dl, cell_dl_iso40,
			sizeof(cell_dl_iso40));
	    break;
	}
#endif
}

void _set_init_info(struct btc_t *btc)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &btc->cx.wl;

	dm->init_info.wl_only = (u8)dm->wl_only;
	dm->init_info.bt_only = (u8)dm->bt_only;
	dm->init_info.wl_init_ok = (u8)wl->status.map.init_ok;
	dm->init_info.dbcc_en = btc->hal->dbcc_en;
	dm->init_info.cx_other = btc->cx.other.type;
	dm->init_info.wl_guard_ch = btc->chip->afh_guard_ch;
	dm->init_info.module = btc->mdinfo;

	/* original tx power, no Tx power adjust */
	hal_btc_fw_set_drv_info(btc, CXDRVINFO_INIT);
	hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
}

void _update_role_link_mode(struct btc_t *btc, bool client_joined, u32 noa)
{
	struct btc_wl_role_info *wl_rinfo = &btc->cx.wl.role_info;
	struct btc_wl_role_info_bpos wl_role = wl_rinfo->role_map.role;
	u32 type = BTC_WLMROLE_NONE, dur = 0;

	/* if no client_joined, don't care P2P-GO/AP role */
	if ((wl_role.p2p_go || wl_role.ap) && !client_joined) {
		if (wl_rinfo->link_mode == BTC_WLINK_2G_SCC) {
			wl_rinfo->link_mode = BTC_WLINK_2G_STA;
			wl_rinfo->connect_cnt--;
		} else if (wl_rinfo->link_mode == BTC_WLINK_2G_GO ||
			wl_rinfo->link_mode == BTC_WLINK_2G_AP) {
			wl_rinfo->link_mode = BTC_WLINK_NOLINK;
			wl_rinfo->connect_cnt--;
		}
	}

	/* Identify 2-Role type  */
	if (wl_rinfo->connect_cnt >= 2 &&
	    (wl_rinfo->link_mode == BTC_WLINK_2G_SCC ||
	     wl_rinfo->link_mode == BTC_WLINK_2G_MCC ||
	     wl_rinfo->link_mode == BTC_WLINK_25G_MCC ||
	     wl_rinfo->link_mode == BTC_WLINK_5G)) {
		if (wl_role.p2p_go || wl_role.ap)
			type = noa? BTC_WLMROLE_STA_GO_NOA : BTC_WLMROLE_STA_GO;
		else if (wl_role.p2p_gc)
			type = noa? BTC_WLMROLE_STA_GC_NOA : BTC_WLMROLE_STA_GC;
		else
			type = BTC_WLMROLE_STA_STA;

		dur = noa;
	}

	wl_rinfo->mrole_type = type;
	wl_rinfo->mrole_noa_duration = dur;

	PHL_INFO("[BTC], %s(): link_mode=%d, connect_cnt=%d, mtype=%d,"
		 " client_joined=%d, noa=%d!!\n", __func__, wl_rinfo->link_mode,
		 wl_rinfo->connect_cnt, type, client_joined, noa);
}

u8 _get_role_link_mode(struct btc_t *btc, u8 role)
{
	switch(role) {
	case PHL_RTYPE_STATION:
		return BTC_WLINK_2G_STA;
	case PHL_RTYPE_P2P_GO:
		return BTC_WLINK_2G_GO;
	case PHL_RTYPE_P2P_GC:
		return BTC_WLINK_2G_GC;
	case PHL_RTYPE_AP:
		return BTC_WLINK_2G_AP;
	default:
		return BTC_WLINK_OTHER;
	}
}

u8 _get_wl_role_idx(struct btc_t *btc, u8 role)
{
	struct btc_wl_role_info *wl_rinfo = &btc->cx.wl.role_info;
	u8 i, rid = 0;

	for (i = 0; i < BTC_WL_MAX_ROLE_NUMBER; i++) {
		if (wl_rinfo->active_role[i].role == role) {
			if (!wl_rinfo->dbcc_en ||
			    wl_rinfo->active_role[i].phy ==
			    wl_rinfo->dbcc_2g_phy)
			break;
		}
	}

	rid = i;
	return rid; /*cation: return BTC_WL_MAX_ROLE_NUMBER if role not found */
}

void _set_wl_req_mac(struct btc_t *btc, u8 mac_id)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_chip_ops *ops = btc->chip->ops;

	if (mac_id == wl->pta_req_mac || mac_id >= HW_PHY_MAX)
		return;

	if (ops && ops->wl_req_mac)
		ops->wl_req_mac(btc, mac_id);

	wl->pta_req_mac = mac_id;
}

static bool _chk_role_ch_group(struct btc_t *btc, struct rtw_chan_def r1,
			       struct rtw_chan_def r2)
{
	if (r1.chan != r2.chan) { /* primary ch is different */
		return false;
	} else if (r1.bw == CHANNEL_WIDTH_40 && r2.bw == CHANNEL_WIDTH_40) {
		if (r1.offset != r2.offset)
			return false;
	}

	return true;  /* true-> the r1/r2 channel are grouped */
}

static u8 _chk_dbcc(struct btc_t *btc, struct rtw_chan_def* ch,
		    u8* phy, u8* role, u8* dbcc_2g_phy)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	u8 j, k, dbcc_2g_cid = 0, dbcc_2g_cid2 = 0;
	bool is_2g_ch_exist = false, is_multi_role_in_2g_phy = false;

	/* find out the 2G-PHY by connect-id ->ch  */
	for (j = 0; j < wl_rinfo->connect_cnt; j++) {
		if ((ch+j)->center_ch <= 14) {
			is_2g_ch_exist = true;
			break;
		}
	}

	/* If no any 2G-port exist, it's impossible because 5G-exclude */
	if (!is_2g_ch_exist)
		return BTC_WLINK_OTHER;

	dbcc_2g_cid = j;
	*dbcc_2g_phy = *(phy+dbcc_2g_cid);

	/* connect_cnt <= 2 */
	if (wl_rinfo->connect_cnt < BTC_TDMA_WLROLE_MAX)
		 return (_get_role_link_mode(btc, (*role+dbcc_2g_cid)));

	/* find the other-port in the 2G-PHY, ex: PHY-0:6G, PHY1: mcc/scc */
	for (k = 0; k < wl_rinfo->connect_cnt; k++) {
		if (k == dbcc_2g_cid)
			continue;

		if (*(phy+k) == *dbcc_2g_phy) {
			is_multi_role_in_2g_phy = true;
			dbcc_2g_cid2 = k;
			break;
		}
	}

	/* Single-role in 2G-PHY */
	if (!is_multi_role_in_2g_phy)
		return (_get_role_link_mode(btc, (*role+dbcc_2g_cid)));

	/* 2-role in 2G-PHY */
	if ((ch+dbcc_2g_cid2)->center_ch > 14)
		return BTC_WLINK_25G_MCC;
	else if (_chk_role_ch_group(btc, *(ch+dbcc_2g_cid), *(ch+dbcc_2g_cid2)))
		return BTC_WLINK_2G_SCC;
	else
		return BTC_WLINK_2G_MCC;
}

static void _update_wl_info(struct btc_t *btc, u8 rid, enum link_state reason)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_link_info *wl_linfo = wl->link_info;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	struct btc_wl_dbcc_info *wl_dinfo = &wl->dbcc_info;
	struct btc_wl_active_role *act_role = NULL;
	struct rtw_hal_com_t *h = btc->hal;
	struct rtw_chan_def cid_ch[BTC_WL_MAX_ROLE_NUMBER];
	u8 i, cnt = 0, mac_id = HW_PHY_0;
	u8 cnt_2g = 0, cnt_5g = 0, mode = BTC_WLINK_NOLINK;
	u8 cid_phy[BTC_WL_MAX_ROLE_NUMBER] = {0};
	u8 cid_role[BTC_WL_MAX_ROLE_NUMBER] = {0};
	u8 dbcc_2g_phy = HW_PHY_MAX, phy_now = 0, phy_dbcc;
	u32 noa_duration = 0, sz = sizeof(struct rtw_chan_def);
	bool b2g = false, b5g = false, client_joined = false;

	hal_mem_set(h, wl_rinfo, 0, sizeof(struct btc_wl_role_info));
	hal_mem_set(h, wl_dinfo, 0, sizeof(struct btc_wl_dbcc_info));
	hal_mem_set(h, cid_ch, 0, BTC_WL_MAX_ROLE_NUMBER * sz);

	for (i = 0; i < BTC_WL_MAX_ROLE_NUMBER; i++) {
		/* check if role active? */
		if (!wl_linfo[i].active ||
		     wl_linfo[i].phy >= HW_PHY_MAX ||
		     wl_linfo[i].pid >= BTC_WL_MAX_ROLE_NUMBER)
			continue;

		/* Extract wl->link_info to wl->role_info
		 * wl->link_info[i] --> i = role_index (0~4)
		 * wl->link_info[i].pid --> pid = port_index
  		 * wl->role_info.active_role[i] --> i = role_id
		 */
		act_role = &wl_rinfo->active_role[i];
		act_role->role = wl_linfo[i].role;

		/* check if role connect? */
		if (wl_linfo[i].connected == MLME_NO_LINK) {
			act_role->connected = 0;
			if (rid == i && act_role->role == PHL_RTYPE_STATION)
				btc->dm.leak_ap = 0;
			continue;
		} else if (wl_linfo[i].connected == MLME_LINKED) {
			act_role->connected = 1;

			if (rid == i && act_role->role == PHL_RTYPE_STATION)
				hal_mem_cpy(h, wl->bssid, wl_linfo[i].mac_addr,
					    sizeof(wl->bssid));
		}

		cnt++;

		act_role->pid = wl_linfo[i].pid;
		act_role->phy = wl_linfo[i].phy;
		act_role->band = wl_linfo[i].chdef.band;
		act_role->ch = wl_linfo[i].chdef.center_ch;
		act_role->bw = wl_linfo[i].chdef.bw;
		act_role->noa = (u8)wl_linfo[i].noa;
		act_role->noa_duration = wl_linfo[i].noa_duration/1000;
		hal_mem_cpy(h, &cid_ch[cnt-1], &wl_linfo[i].chdef, sz);
		cid_phy[cnt-1] = wl_linfo[i].phy;
		cid_role[cnt-1] = wl_linfo[i].role;
		wl_rinfo->role_map.val |= BIT(wl_linfo[i].role);

		if (rid == i)
			phy_now = act_role->phy;

		/* only if client connect for p2p-Go/AP */
		if ((wl_linfo[i].role == PHL_RTYPE_P2P_GO ||
		     wl_linfo[i].role == PHL_RTYPE_AP) &&
		     wl_linfo[i].client_cnt > 1)
			client_joined = true;

		/* only one noa-role exist */
		if (act_role->noa && act_role->noa_duration > 0) {
			noa_duration = act_role->noa_duration;
		}

		/* Check dbcc band, the last band may overwrite the former role,
		 * This will be modified in the following code.
		 */
		if (h->dbcc_en) {
			phy_dbcc = wl_linfo[i].phy;
			wl_dinfo->role[phy_dbcc] |= BIT(wl_linfo[i].role);
			wl_dinfo->op_band[phy_dbcc] = wl_linfo[i].chdef.band;
		}

		if (wl_linfo[i].chdef.band != BAND_ON_24G) {
			cnt_5g++;
			b5g = true;
		} else {
			if (wl_linfo[i].role == PHL_RTYPE_P2P_GO ||
			    wl_linfo[i].role == PHL_RTYPE_P2P_GC ||
			    wl_linfo[i].role == PHL_RTYPE_AP)
				wl_rinfo->p2p_2g = 1;

			cnt_2g++;
			b2g = true;
		}
	}

	wl_rinfo->connect_cnt = cnt;

	/* Be careful to change the following sequence!! */
	if (cnt == 0) {
		mode = BTC_WLINK_NOLINK;
		wl_rinfo->role_map.role.none = 1;
	} else if (!b2g && b5g) {
		mode = BTC_WLINK_5G;
	} else if (wl_rinfo->role_map.role.nan) {
		mode = BTC_WLINK_2G_NAN;
	} else if (cnt > BTC_TDMA_WLROLE_MAX) {
		mode = BTC_WLINK_OTHER;
	} else if (h->dbcc_en) {
		mode = _chk_dbcc(btc, cid_ch, cid_phy, cid_role, &dbcc_2g_phy);

		/* correct 2G-located PHY band for gnt ctrl */
		wl_dinfo->op_band[dbcc_2g_phy] = BAND_ON_24G;
	} else if (b2g && b5g && cnt == 2) {
		mode = BTC_WLINK_25G_MCC;
	} else if (!b5g && cnt == 2) { /* cnt_connect = 2 */
		if (_chk_role_ch_group(btc, cid_ch[0], cid_ch[cnt - 1]))
			mode = BTC_WLINK_2G_SCC;
		else
			mode = BTC_WLINK_2G_MCC;
	} else if (!b5g && cnt == 1) { /* cnt_connect = 1 */
		mode = _get_role_link_mode(btc, cid_role[0]);
	}

	wl_rinfo->link_mode = mode;
	_update_role_link_mode(btc, client_joined, noa_duration);

	if (reason == PHL_EN_DBCC_START ||
	    reason == PHL_DIS_DBCC_START) {
		wl->status.map.dbccing = 1;
	} else if (reason == PHL_EN_DBCC_DONE ||
		   reason == PHL_DIS_DBCC_DONE) {
		wl->status.map.dbccing = 0;
	} else if (!h->dbcc_en ||
		   (phy_now == dbcc_2g_phy && !wl->status.map.dbccing)) {
		if (reason == PHL_LINK_STARTED ||
		    reason == PHL_EN_LINK_START ||
		    reason == PHL_DIS_LINK_START)
			wl->status.map.connecting = 1;
		else
			wl->status.map.connecting = 0;

		if (reason == PHL_LINK_STOPPED ||
		    reason == PHL_EN_LINK_DONE)
			wl->status.map._4way = 0;
	}

	if (wl_rinfo->dbcc_en != (u32)h->dbcc_en) {
		wl_rinfo->dbcc_chg = 1;
		wl_rinfo->dbcc_en = (u32)h->dbcc_en;
		btc->cx.cnt_wl[BTC_WCNT_DBCC_CHG]++;
	}

	if (h->dbcc_en) {
		wl_rinfo->dbcc_2g_phy = (u32)dbcc_2g_phy;

		if (dbcc_2g_phy == HW_PHY_1)
			mac_id = HW_PHY_1;

		_update_dbcc_band(HW_PHY_0);
		_update_dbcc_band(HW_PHY_1);
	}

	_set_wl_req_mac(btc, mac_id);
	hal_btc_fw_set_drv_info(btc, CXDRVINFO_ROLE);

	PHL_INFO("[BTC], %s(): rid=%d, reason=%d, dbcc_en=%d, pta_mac=%d,"
		 " phy_now=%d, dbcc_2g_phy=%d\n", __func__, rid, reason,
		 wl->pta_req_mac, h->dbcc_en, phy_now, dbcc_2g_phy);

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): dbccing=%d, connecting=%d, _4way=%d\n",
		  __func__, wl->status.map.dbccing, wl->status.map.connecting,
		  wl->status.map._4way);
}

void _run_coex(struct btc_t *btc, const char *reason)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_bt_info *bt = &cx->bt;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	struct btc_wl_smap *wl_smap = &wl->status.map;
	u8 mode = wl_rinfo->link_mode, rpt;
	u32 len = _os_strlen((u8 *)reason) + 1;

	PHL_INFO("[BTC], %s(): reason = %s, mode=%d\n", __func__, reason, mode);
	len = (len < BTC_RSN_MAXLEN) ? len : BTC_RSN_MAXLEN;
	_rsn_cpy(dm->run_reason, (char*)reason, len);
	_update_dm_step(btc, reason);
	_update_btc_state_map(btc);

	rpt = !(dm->cnt_dm[BTC_DCNT_RUN] % (BTC_RPT_PERIOD/BTC_PERIODIC_TIME));

	/* if link exist nhm in _ntfy_wl_sta */
	if (mode == BTC_WLINK_NOLINK && rpt) {
		_get_wl_nhm_dbm(btc);
		_get_wl_cn_report(btc);
	}

	/* Be careful to change the following function sequence!! */
	if (btc->ctrl.manual) {
		PHL_INFO("[BTC], %s(): return for Manual CTRL!!\n", __func__);
		return;
	}

	if (btc->ctrl.igno_bt &&
	    (run_rsn("_update_bt_info") || run_rsn("_update_bt_scbd"))) {
		PHL_INFO("[BTC], %s(): return for Stop Coex DM!!\n", __func__);
		return;
	}

	if (!wl_smap->init_ok) {
		PHL_INFO("[BTC], %s(): return for WL init fail!!\n", __func__);
		return;
	}

	if (wl_smap->rf_off_pre == wl_smap->rf_off &&
	    wl_smap->lps_pre == wl_smap->lps) {
	    	if (run_rsn("_ntfy_power_off") ||
		    run_rsn("_ntfy_radio_state")) {
			PHL_INFO("[BTC], %s(): return"
				 " WL rf off state no change\n", __func__);
			return;
	    	}

		if (wl_smap->rf_off == 1 || wl_smap->lps == BTC_LPS_RF_OFF) {
			PHL_INFO("[BTC], %s(): return"
				 " for WL rf off state!!\n", __func__);
			return;
		}
	}

	dm->cnt_dm[BTC_DCNT_RUN]++;
	dm->freerun = false;
	dm->fddt_train = BTC_FDDT_DISABLE;
	btc->ctrl.igno_bt = false;
	bt->scan_rx_low_pri = false;

	if (btc->ctrl.always_freerun) {
		_action_freerun(btc);
		btc->ctrl.igno_bt = true;
	} else if (dm->wl_only) {
		_action_wl_only(btc);
		btc->ctrl.igno_bt = true;
	} else if (wl_smap->rf_off || wl_smap->lps || dm->bt_only) {
		_action_wl_off(btc);
		btc->ctrl.igno_bt = true;
	} else if (run_rsn("_ntfy_init_coex")) {
		_action_wl_init(btc);
	} else if (!cx->bt.enable.now && !cx->other.type) {
		_action_bt_off(btc);
	} else if (cx->bt.whql_test) {
		_action_bt_whql(btc);
	} else if (wl->rfk_info.state != BTC_WRFK_STOP) {
		_action_wl_rfk(btc);
	} else if (cx->state_map == BTC_WLINKING || wl_smap->scan) {
		_action_wl_scan(btc);
	} else {
		switch (mode) {
		case BTC_WLINK_NOLINK:
			_action_wl_nc(btc);
			break;
		case BTC_WLINK_2G_STA:
			_action_wl_2g_sta(btc);
			break;
		case BTC_WLINK_2G_AP:
			bt->scan_rx_low_pri = true;
			_action_wl_2g_ap(btc);
			break;
		case BTC_WLINK_2G_GO:
			bt->scan_rx_low_pri = true;
			_action_wl_2g_go(btc);
			break;
		case BTC_WLINK_2G_GC:
			bt->scan_rx_low_pri = true;
			_action_wl_2g_gc(btc);
			break;
		case BTC_WLINK_2G_SCC:
			bt->scan_rx_low_pri = true;
			_action_wl_2g_scc(btc);
			break;
		case BTC_WLINK_2G_MCC:
			bt->scan_rx_low_pri = true;
			_action_wl_2g_mcc(btc);
			break;
		case BTC_WLINK_25G_MCC:
			bt->scan_rx_low_pri = true;
			_action_wl_25g_mcc(btc);
			break;
		case BTC_WLINK_5G:
			_action_wl_5g(btc);
			break;
		case BTC_WLINK_2G_NAN:
			_action_wl_2g_nan(btc);
			break;
		default:
			_action_wl_other(btc);
			break;
		}
	}

	_action_common(btc);
}

static void _update_offload_runinfo(struct btc_t *btc, u8 *buf, u32 len)
{

}

void _update_btc_state_map(struct btc_t *btc)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_smap wl = cx->wl.status.map;
	struct btc_bt_smap bt = cx->bt.link_info.status.map;

	if (wl.connecting || wl._4way || wl.roaming || wl.dbccing) {
		cx->state_map = BTC_WLINKING;
	} else if (wl.scan) { /* wl scan */
		if (bt.inq_pag)
			cx->state_map = BTC_WSCAN_BSCAN;
		else
			cx->state_map = BTC_WSCAN_BNOSCAN;
	} else if (wl.busy) { /* only busy */
		if (bt.inq_pag)
			cx->state_map = BTC_WBUSY_BSCAN;
		else
			cx->state_map = BTC_WBUSY_BNOSCAN;
	} else { /* wl idle */
		if (bt.inq_pag)
			cx->state_map = BTC_WIDLE_BSCAN;
		else
			cx->state_map = BTC_WIDLE;
	}
}

void _update_bt_scbd(struct btc_t *btc, bool only_update)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_bt_info *bt = &cx->bt;
	u32 val, any_bt_connect;
	bool status_change = false, bt_link_change = false;

	if (!btc->chip->scbd)
		return;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s()\n", __func__);
	val = _read_scbd(btc);

	if (val == 0xffffffff) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_ERR_,
			  "[BTC], %s return by invalid scbd value\n", __func__);
		return;
	}

	if (!(val & BTC_BSCB_ON))
		bt->enable.now = 0;
	else
		bt->enable.now = 1;

	if (bt->enable.now != bt->enable.last) {
		status_change = true;
		bt_link_change = true;
	}

	/* reset bt info if bt re-enable */
	if (bt->enable.now && !bt->enable.last) {
		_reset_btc_var(btc, BTC_RESET_BTINFO);
		cx->cnt_bt[BTC_BCNT_REENABLE]++;
		bt->enable.now = 1;
	}

	bt->enable.last = bt->enable.now;
	bt->scbd = val;
	bt->mbx_avl = !!(val & BTC_BSCB_ACT);

        if (bt->whql_test != (u32)(!!(val & BTC_BSCB_WHQL)))
		status_change = true;

	bt->whql_test = !!(val & BTC_BSCB_WHQL);
	bt->btg_type = (val & BTC_BSCB_BT_S1 ? BTC_BT_BTG: BTC_BT_ALONE);
	bt->link_info.a2dp_desc.exist = !!(val & BTC_BSCB_A2DP_ACT);

	bt->lna_constrain = 4 + !!(val & BTC_BSCB_BT_LNAB0) +
			    (!!(val & BTC_BSCB_BT_LNAB1)) * 2;

	/* if rfk run 1->0 */
	if (bt->rfk_info.map.run && !(val & BTC_BSCB_RFK_RUN))
		status_change = true;

	bt->rfk_info.map.run  = !!(val & BTC_BSCB_RFK_RUN);
	bt->rfk_info.map.req = !!(val & BTC_BSCB_RFK_REQ);
	bt->hi_lna_rx = !!(val & BTC_BSCB_BT_HILNA);
	any_bt_connect = !!(val & BTC_BSCB_BT_CONNECT);

	/* if connect change */
	if (bt->link_info.status.map.connect != any_bt_connect) {
		status_change = true;
		bt_link_change = true;
	}

	bt->link_info.status.map.connect = any_bt_connect;
	bt->run_patch_code = !!(val & BTC_BSCB_PATCH_CODE);

	if (bt_link_change) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
			  "[BTC], %s: bt status change!!\n", __func__);
		hal_btc_send_event(btc,  btc->cx.wl.pta_req_mac,
				   NULL, 0, BTC_HMSG_BT_LINK_CHG);
	}

	if (bt->enable.now && (bt->ver_info.fw == 0 ||
	    ((bt->ver_info.fw & 0xfff0) == 0x8850 && bt->run_patch_code)))
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_VER_INFO, 1);
	else
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_VER_INFO, 0);

	if (!only_update && status_change)
		_run_coex(btc, __func__);
}

/******************************************************************************
 *
 * coexistence operations for external notifications
 *
 *****************************************************************************/
static void _ntfy_power_on(struct btc_t *btc)
{
	/* no action for power on, beacuse power-on move halmac API
	* the _ntfy_power_on = _ntfy_init_coex
	*/
}

static void _ntfy_power_off(struct btc_t *btc)
{
	struct btc_wl_info *wl = &btc->cx.wl;

	PHL_INFO("[BTC], %s()\n", __func__);
	btc->dm.cnt_notify[BTC_NCNT_POWER_OFF]++;

	wl->status.map.rf_off = 1;
	wl->status.map.busy = 0;
	wl->status.map.lps = BTC_LPS_OFF;
	_write_scbd(btc, BTC_WSCB_ALL, false);
	hal_btc_fw_en_rpt(btc, RPT_EN_ALL, 0);

	_run_coex(btc, __func__);
	wl->status.map.rf_off_pre = wl->status.map.rf_off;
}

static void _ntfy_init_coex(struct btc_t *btc, u8 mode)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_chip_ops *ops = btc->chip->ops;

	PHL_INFO("[BTC], %s(): mode=%d\n", __func__, mode);
	dm->cnt_notify[BTC_NCNT_INIT_COEX]++;
	btc->ctrl.always_freerun = (mode == BTC_MODE_COTX? 1 : 0);
	dm->wl_only = (mode == BTC_MODE_WL? 1 : 0);
	dm->bt_only = (mode == BTC_MODE_BT? 1 : 0);
	wl->status.map.rf_off = (mode == BTC_MODE_WLOFF? 1 : 0);

	if (!wl->status.map.init_ok) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_ERR_,
			  "[BTC], %s(): return for WL init fail!!\n", __func__);
		dm->error.map.init = true;
		return;
	}

	/* Setup RF front end type from EFuse RFE type*/
	if (ops && ops->set_rfe)
		ops->set_rfe(btc);

	if (ops && ops->init_cfg)
		ops->init_cfg(btc);

	_write_scbd(btc, BTC_WSCB_ACTIVE | BTC_WSCB_ON | BTC_WSCB_BTLOG, true);
	_update_bt_scbd(btc, true);

#if (BTC_WL_MAX_ROLE_NUMBER < MAX_WIFI_ROLE_NUMBER)
	dm->error.map.role_num_mismatch = true;
	PHL_INFO("[BTC], %s(): ROLE_NUMBER conflict!!\n", __func__);
#endif

	/* check PTA control owner to avoid BT coex issue */
	dm->pta_owner = _read_cx_ctrl(btc);
	if (dm->pta_owner == BTC_CTRL_BY_WL && btc->chip->para_ver != 0x1) {
		PHL_INFO("[BTC], %s(): PTA owner warning!!\n", __func__);
		dm->error.map.pta_owner = true;
	}

	_set_init_info(btc);
	_set_fddt_cell_by_antiso(btc);
	/* original tx power, no Tx power adjust */
	_set_wl_tx_power(btc, BTC_WL_DEF_TX_PWR);
	hal_btc_fw_set_slots(btc, CXST_MAX, dm->slot);
	hal_btc_fw_set_monreg(btc);
	_run_coex(btc, __func__);
	_btmr_start(btc);
}

static void _ntfy_scan_start(struct btc_t *btc, u8 phy_idx, u8 band)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;

	PHL_INFO("[BTC], %s(): phy_idx=%d, band=%d\n", __func__, phy_idx, band);
	btc->dm.cnt_notify[BTC_NCNT_SCAN_START]++;

	wl->scan_info.band[phy_idx] = band;
	wl->scan_info.phy_map |= BIT(phy_idx);

	if (wl_rinfo->dbcc_en) {
		wl->dbcc_info.scan_band[phy_idx] = band;
		_update_dbcc_band(phy_idx);

		if (wl_rinfo->dbcc_2g_phy != phy_idx) /* return if not-2G PHY */
			return;
	}

	wl->status.map.scan = true;
	hal_btc_fw_set_drv_info(btc, CXDRVINFO_SCAN);

	_run_coex(btc, __func__);
}

static void _ntfy_scan_finish(struct btc_t *btc, u8 phy_idx)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;

	PHL_INFO("[BTC], %s(): phy_idx=%d\n", __func__, phy_idx);
	btc->dm.cnt_notify[BTC_NCNT_SCAN_FINISH]++;

	wl->status.map.scan = false;
	wl->scan_info.phy_map &= ~(u8)BIT(phy_idx);
	hal_btc_fw_set_drv_info(btc, CXDRVINFO_SCAN);

	if (wl_rinfo->dbcc_en)
		_update_dbcc_band(phy_idx);

	_run_coex(btc, __func__);
}

static void _ntfy_switch_band(struct btc_t *btc, u8 phy_idx, u8 band)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;

	PHL_INFO("[BTC], %s(): phy_idx=%d, band=%d\n", __func__, phy_idx, band);
	btc->dm.cnt_notify[BTC_NCNT_SWITCH_BAND]++;

#if 0
	wl->scan_info.band[phy_idx] = band;
	wl->scan_info.phy_map |= BIT(phy_idx);
	hal_btc_fw_set_drv_info(btc, CXDRVINFO_SCAN);
#endif

	if (wl_rinfo->dbcc_en) {
		/* wl->dbcc_info.scan_band[phy_idx] = band; */
		_update_dbcc_band(phy_idx);
	}

	_run_coex(btc, __func__);
}

static void _ntfy_specific_packet(struct btc_t *btc, u8 pkt_type)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &cx->wl;
	u32 cnt;

	if (btc->bt_req_en)
		return;

	switch (pkt_type) {
	case PKT_EVT_DHCP:
		cnt = ++cx->cnt_wl[BTC_WCNT_DHCP];
		PHL_INFO("[BTC], %s(): pkt_type=%d, DHCP cnt=%d \n",
			 __func__, pkt_type, cnt);
		wl->status.map.connecting = true;
		_set_btc_timer(btc, BTC_TIMER_WL_SPECPKT, BTC_SPECPKT_MAXT);
		break;
	case PKT_EVT_EAPOL_START:
		cnt = ++cx->cnt_wl[BTC_WCNT_EAPOL];
		PHL_INFO("[BTC], %s(): pkt_type=%d, EAPOL_Start cnt=%d \n",
			 __func__, pkt_type, cnt);
		wl->status.map._4way = 1;
		break;
	case PKT_EVT_EAPOL_END:
		cnt = ++cx->cnt_wl[BTC_WCNT_EAPOL];
		PHL_INFO("[BTC], %s(): pkt_type=%d, EAPOL_End cnt=%d \n",
			 __func__, pkt_type, cnt);
		wl->status.map._4way = 0;
		break;
	default:
	case PKT_EVT_ARP:
		cnt = ++cx->cnt_wl[BTC_WCNT_ARP];
		PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s(): pkt_type=%d,"
			  " ARP cnt=%d\n", __func__, pkt_type, cnt);
		return;
	}

	btc->dm.cnt_notify[BTC_NCNT_SPECIAL_PACKET]++;
	_run_coex(btc, __func__);
}

void _update_dm_step(struct btc_t *btc, const char *strin)
{
	struct btc_dm *dm = &btc->dm;
	u32 store_index = 0, len = _os_strlen((u8 *)strin) + 1;

	dm->dm_step.cnt++;
	if (dm->dm_step.cnt == 0)
		dm->dm_step.cnt = 1;

	store_index = ((dm->dm_step.cnt-1) % BTC_DM_MAXSTEP);
	len = (len < BTC_RSN_MAXLEN) ? len : BTC_RSN_MAXLEN;
	_rsn_cpy(dm->dm_step.step[store_index], (char*)strin, len);
}

static u8 _update_bt_rssi_level(struct btc_t *btc, u8 rssi)
{
	struct btc_bt_info *bt = &btc->cx.bt;
	u8 *rssi_st = NULL;
	u8 rssi_th = 0, j, rssi_level = 0;

	/* for rssi locate in which {40, 36, 31, 28}
	 * if rssi >= 40% (-60dBm) --> rssi_level = 4
	 * if 36% <= rssi < 40%    --> rssi_level = 3
	 * if 31% <= rssi < 36%    --> rssi_level = 2
	 * if 28% <= rssi < 31%    --> rssi_level = 1
	 * if rssi < 28%           --> rssi_level = 0
	 */

	/* check if rssi across bt_rssi_thres boundary */
	for (j = 0; j < BTC_BT_RSSI_THMAX; j++) {
		rssi_th = btc->chip->bt_rssi_thres[j];
		rssi_st = &bt->link_info.rssi_state[j];

		*rssi_st = _update_rssi_state(btc, *rssi_st, rssi, rssi_th);

		if (BTC_RSSI_HIGH(*rssi_st)) {
			rssi_level = BTC_BT_RSSI_THMAX - j;
			break;
		}
	}

	return rssi_level;
}

static void _update_bt_info(struct btc_t *btc, u8 *buf, u32 len)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_cx *cx = &btc->cx;
	struct btc_bt_info *bt = &cx->bt;
	struct btc_bt_link_info *b = &bt->link_info;
	struct btc_bt_hfp_desc *hfp = &b->hfp_desc;
	struct btc_bt_hid_desc *hid = &b->hid_desc;
	struct btc_bt_a2dp_desc *a2dp = &b->a2dp_desc;
	struct btc_bt_pan_desc *pan = &b->pan_desc;
	union btc_btinfo btinfo;
	bool bt_link_change = false;

	if (buf[BTC_BTINFO_L1] != BTC_BT_INFO_LEN)
		return;

	/* return if bt info match last bt-info  */
	if (!hal_mem_cmp(h, bt->raw_info, buf, BTC_BTINFO_MAX)) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
			  "[BTC], %s return by bt-info duplicate!\n", __func__);
		cx->cnt_bt[BTC_BCNT_INFOSAME]++;
		return;
	}

	hal_mem_cpy(h, bt->raw_info, buf, BTC_BTINFO_MAX);

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s: bt_info[2]=0x%02x\n", __func__, bt->raw_info[2]);

	/* reset to mo-connect before update */
	b->profile_cnt.last = b->profile_cnt.now;
	b->relink.last = b->relink.now;
	a2dp->exist_last = a2dp->exist;
	b->multi_link.last = b->multi_link.now;
	bt->inq_pag.last = bt->inq_pag.now;
	b->profile_cnt.now = 0;
	hid->type = 0;

	/* ======= parse raw info low-Byte2 ======= */
	btinfo.val = bt->raw_info[BTC_BTINFO_L2];
	b->status.map.connect = btinfo.lb2.connect;
	b->status.map.sco_busy = btinfo.lb2.sco_busy;
	b->status.map.acl_busy = btinfo.lb2.acl_busy;
	b->status.map.inq_pag = btinfo.lb2.inq_pag;
	bt->inq_pag.now = btinfo.lb2.inq_pag;
	cx->cnt_bt[BTC_BCNT_INQPAG] += !!(bt->inq_pag.now && !bt->inq_pag.last);

	hfp->exist = btinfo.lb2.hfp;
	b->profile_cnt.now += (u8)hfp->exist;
	hid->exist = btinfo.lb2.hid;
	b->profile_cnt.now += (u8)hid->exist;
	a2dp->exist = btinfo.lb2.a2dp;
	b->profile_cnt.now += (u8)a2dp->exist;
	pan->exist = btinfo.lb2.pan;
	b->profile_cnt.now += (u8)pan->exist;

	/* ======= parse raw info low-Byte3 ======= */
	btinfo.val = bt->raw_info[BTC_BTINFO_L3];

	cx->cnt_bt[BTC_BCNT_RETRY] += btinfo.lb3.retry;
	b->cqddr = btinfo.lb3.cqddr;
	cx->cnt_bt[BTC_BCNT_INQ] += !!(btinfo.lb3.inq && !bt->inq);
	bt->inq = btinfo.lb3.inq;
	cx->cnt_bt[BTC_BCNT_PAGE] += !!(btinfo.lb3.pag && !bt->pag);
	bt->pag = btinfo.lb3.pag;

	b->status.map.mesh_busy = btinfo.lb3.mesh_busy;
	/* ======= parse raw info high-Byte0 ======= */
	btinfo.val = bt->raw_info[BTC_BTINFO_H0];
	/* raw val is dBm unit, translate from -100~ 0dBm to 0~100%*/
	b->rssi = btc->chip->ops->bt_rssi(btc, btinfo.hb0.rssi);
	bt->rssi_level = _update_bt_rssi_level(btc, (u8)b->rssi);
	btc->dm.fddt_trx_info.bt_rssi = bt->rssi_level;

	/* ======= parse raw info high-Byte1 ======= */
	btinfo.val = bt->raw_info[BTC_BTINFO_H1];
	b->status.map.ble_connect = btinfo.hb1.ble_connect;
	if (btinfo.hb1.ble_connect)
		hid->type |= (hid->exist? BTC_HID_BLE : BTC_HID_RCU);

	cx->cnt_bt[BTC_BCNT_REINIT] += !!(btinfo.hb1.reinit && !bt->reinit);
	bt->reinit = btinfo.hb1.reinit;
	cx->cnt_bt[BTC_BCNT_RELINK] += !!(btinfo.hb1.relink && !b->relink.now);
	b->relink.now = btinfo.hb1.relink;
	cx->cnt_bt[BTC_BCNT_IGNOWL] += !!(btinfo.hb1.igno_wl && !bt->igno_wl);
	bt->igno_wl = btinfo.hb1.igno_wl;

	hid->type |= (btinfo.hb1.voice? BTC_HID_RCU_VOICE : 0);
	bt->ble_scan_en = btinfo.hb1.ble_scan;

	cx->cnt_bt[BTC_BCNT_ROLESW] += !!(btinfo.hb1.role_sw && !b->role_sw);
	b->role_sw = btinfo.hb1.role_sw;

	b->multi_link.now = btinfo.hb1.multi_link;
	if (b->multi_link.now != b->multi_link.last)
		bt_link_change = true;

	/* ======= parse raw info high-Byte2 ======= */
	btinfo.val = bt->raw_info[BTC_BTINFO_H2];
	pan->active = !!btinfo.hb2.pan_active;

	cx->cnt_bt[BTC_BCNT_AFH] += !!(btinfo.hb2.afh_update && !b->afh_update);
	b->afh_update = btinfo.hb2.afh_update;
	a2dp->active = btinfo.hb2.a2dp_active;
	b->slave_role = btinfo.hb2.slave;
	hid->slot_info = btinfo.hb2.hid_slot;

	if (hid->pair_cnt != btinfo.hb2.hid_cnt)
		bt_link_change = true;

	hid->pair_cnt = btinfo.hb2.hid_cnt;
	hid->type |= (hid->slot_info == BTC_HID_218? BTC_HID_218 : BTC_HID_418);

	/* ======= parse raw info high-Byte3 ======= */
	btinfo.val = bt->raw_info[BTC_BTINFO_H3];
	a2dp->bitpool = btinfo.hb3.a2dp_bitpool;

	cx->cnt_bt[BTC_BCNT_RATECHG] += !!(b->tx_3M ^ (u32)btinfo.hb3.tx_3M);
	b->tx_3M = (u32)btinfo.hb3.tx_3M;
	a2dp->sink = btinfo.hb3.a2dp_sink;

	if (bt->igno_wl && !cx->wl.status.map.rf_off)
		_set_bt_ignore_wl_act(btc, false);

	if (b->profile_cnt.now)
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_AFH_MAP, 1);
	else
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_AFH_MAP, 0);

	if (b->status.map.ble_connect)
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_AFH_MAP_LE, 1);
	else
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_AFH_MAP_LE, 0);

	if (bt_link_change) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
			  "[BTC], %s: bt link change!!\n", __func__);
		hal_btc_send_event(btc,  btc->cx.wl.pta_req_mac,
				  NULL, 0, BTC_HMSG_BT_LINK_CHG);
	}

	/* reset after A2DP stop->play */
	if (!a2dp->exist_last && a2dp->exist) {
		a2dp->vendor_id = 0;
		a2dp->flush_time = 0;
		a2dp->play_latency = 1;
		_set_btc_timer(btc, BTC_TIMER_BT_A2DPPLAY,
			       BTC_A2DP_RESUME_MAXT);
	}

	if (a2dp->exist && (a2dp->flush_time == 0 || a2dp->vendor_id == 0 ||
	    a2dp->play_latency == 1))
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_DEVICE_INFO, 1);
	else
		hal_btc_fw_en_rpt(btc, RPT_EN_BT_DEVICE_INFO, 0);

	_run_coex(btc, __func__);
}

static void _update_bt_dev_info(struct btc_t *btc, u8 *buf, u32 len)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_bt_info *bt = &cx->bt;
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_bt_link_info *b = &bt->link_info;
	struct btc_bt_hid_desc *hid = &b->hid_desc;
	struct btc_bt_a2dp_desc *a2dp = &b->a2dp_desc;
	struct btc_bt_pan_desc *pan = &b->pan_desc;
	struct btc_bt_mb_devinfo *src = (struct btc_bt_mb_devinfo *)buf;
	u8 i;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC] %s(): BT DEV INFO SubID=0x%x:"
		  "[0x%x, 0x%x, 0x%x, 0x%x, 0x%x] \n", __func__,
		  src->sub_id, src->data[0], src->data[1],
		  src->data[2], src->data[3], src->data[4]);

	switch (src->sub_id) {
	case BTC_BT_INFO_HID_LIST:
		if (hal_mem_cmp(h, hid->hid_list, src->data + 1,
				sizeof(hid->hid_list)) == 0)
			return;

		hid->hid_cnt = src->data[0];
		hal_mem_cpy(h, hid->hid_list, src->data + 1,
			    sizeof(hid->hid_list));
		hal_mem_set(h, hid->hids, 0, sizeof(hid->hids));
		for (i = 0; i < hid->hid_cnt; i++)
			hid->hids[i].handle_id = hid->hid_list[i];

		break;
	case BTC_BT_INFO_A2DP_LIST:
		if (hal_mem_cmp(h, a2dp->a2dp_list, src->data+1,
				sizeof(a2dp->a2dp_list)) == 0)
			return;

		a2dp->a2dp_cnt = src->data[0];
		hal_mem_cpy(h, a2dp->a2dp_list, src->data+1,
			    sizeof(a2dp->a2dp_list));
		hal_mem_set(h, a2dp->a2dps, 0, sizeof(a2dp->a2dps));

		for (i = 0; i < a2dp->a2dp_cnt; i++)
			a2dp->a2dps[i].handle_id = a2dp->a2dp_list[i];

		break;
	case BTC_BT_INFO_PAN_LIST:
		if (hal_mem_cmp(h, pan->pan_list, src->data+1,
				sizeof(pan->pan_list)) == 0)
			return;

		pan->pan_cnt = src->data[0];
		hal_mem_cpy(h, pan->pan_list, src->data+1,
			    sizeof(pan->pan_list));
		hal_mem_set(h, pan->pans, 0, sizeof(pan->pans));

		for (i = 0; i < pan->pan_cnt; i++)
			pan->pans[i].handle_id = pan->pan_list[i];

		break;
	case BTC_BT_INFO_HID_VENDOR_INFO:
		for (i = 0; i < BTC_BT_INFO_LINK_MAX; i++) {
			if (src->data[0] != hid->hids[i].handle_id)
				continue;

			hid->hids[i].vendor_id = src->data[1];
			break;
		}
		break;
	case BTC_BT_INFO_HID_DEVICE_NAME:
		break;
	case BTC_BT_INFO_HID_BASIC_INFO:
		for (i = 0; i < BTC_BT_INFO_LINK_MAX; i++) {
			if (src->data[0] != hid->hids[i].handle_id)
				continue;

			/*role: 1=Master, type: 0=Legacy, sniff: 0=active*/
			hid->hids[i].role = !!(src->data[1] & BIT(0));
			hid->hids[i].type = !!(src->data[1] & BIT(1));
			hid->hids[i].sniff = !!(src->data[1] & BIT(2));
			hid->hids[i].interval = src->data[2];
			hid->hids[i].window = src->data[3];
			break;
		}
		break;
	case BTC_BT_INFO_A2DP_BASIC_INFO:
		for (i = 0; i < BTC_BT_INFO_LINK_MAX; i++) {
			if (src->data[0] != a2dp->a2dps[i].handle_id)
				continue;

			a2dp->a2dps[i].role = !!(src->data[1] & BIT(0));
			a2dp->a2dps[i].type = !!(src->data[1] & BIT(1));
			a2dp->a2dps[i].sniff = !!(src->data[1] & BIT(2));
			break;
		}
		break;
	case BTC_BT_INFO_PAN_BASIC_INFO:
		for (i = 0; i < BTC_BT_INFO_LINK_MAX; i++) {
			if (src->data[0] != pan->pans[i].handle_id)
				continue;

			pan->pans[i].role = !!(src->data[1] & BIT(0));
			pan->pans[i].type = !!(src->data[1] & BIT(1));
			pan->pans[i].sniff = !!(src->data[1] & BIT(2));
			pan->pans[i].direct = !!(src->data[2] & BIT(0));
			break;
		}
		break;
	default:
		break;
	}
}

static void _ntfy_role_info(struct btc_t *btc, u8 rid,
			    struct btc_wl_link_info *info,
			    enum link_state reason)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_link_info *wlinfo = NULL;
	u8 link_mode_ori = wl->role_info.link_mode;
	u8 pta_req_mac_ori = wl->pta_req_mac;

	PHL_INFO("[BTC], %s(), role_id=%d, reason=%d\n", __func__, rid, reason);

	btc->dm.cnt_notify[BTC_NCNT_ROLE_INFO]++;

	if (rid >= BTC_WL_MAX_ROLE_NUMBER)
		return;

	wlinfo = &wl->link_info[rid];

	hal_mem_cpy(h, wlinfo, info, sizeof(struct btc_wl_link_info));

	/* refresh wifi info */
	_update_wl_info(btc, rid, reason);

	if (wl->role_info.link_mode != link_mode_ori)
		wl->role_info.link_mode_chg = 1;

	if (wl->pta_req_mac != pta_req_mac_ori)
		wl->pta_reg_mac_chg = 1;

	if (reason == PHL_EN_DBCC_START || reason == PHL_DIS_DBCC_START)
		btc->dm.tdma_instant_excute = 1;

	_run_coex(btc, __func__);

	wl->role_info.link_mode_chg = 0;
	wl->pta_reg_mac_chg = 0;
}

static void _ntfy_radio_state(struct btc_t *btc, u8 rf_state)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_chip_ops *ops = btc->chip->ops;
	u32 val = 0;

	PHL_INFO("[BTC], %s(): rf_state =%d\n", __func__, rf_state);
	btc->dm.cnt_notify[BTC_NCNT_RADIO_STATE]++;

	switch(rf_state) {
	case BTC_RFCTRL_WL_OFF: /* WL radio-off */
		wl->status.map.rf_off = 1;
		wl->status.map.lps = BTC_LPS_OFF;
		wl->status.map.busy = 0;
		break;
	case BTC_RFCTRL_WL_ON: /* Exit LPS */
	default:
		wl->status.map.rf_off = 0;
		wl->status.map.lps = BTC_LPS_OFF;
		break;
	case BTC_RFCTRL_LPS_WL_ON: /* LPS-Protocol (RFon) */
		wl->status.map.rf_off = 0;
		wl->status.map.lps = BTC_LPS_RF_ON;
		wl->status.map.busy = 0;
		break;
	case BTC_RFCTRL_FW_CTRL: /*  LPS-PG, LPS-CG */
		wl->status.map.rf_off = 0;
		wl->status.map.lps = BTC_LPS_RF_OFF;
		wl->status.map.busy = 0;
		break;
	}

	if (rf_state == BTC_RFCTRL_WL_ON) {
		/* Clear bt counte hang for LPS/IPS leave */
		hal_btc_fw_en_rpt(btc, RPT_EN_MREG, 1);
		val = BTC_WSCB_ACTIVE | BTC_WSCB_ON | BTC_WSCB_BTLOG;
		_write_scbd(btc, val, true);
		_update_bt_scbd(btc, true);
		if (ops && ops->init_cfg)
			ops->init_cfg(btc);
	} else {
		hal_btc_fw_en_rpt(btc, RPT_EN_ALL, 0);

		/* for 52B BT only isolated issue
		 * Move WL2BT scbd[1] to WL FW AOAC RF on/off
		 */
		if (rf_state == BTC_RFCTRL_WL_OFF)
			_write_scbd(btc, BTC_WSCB_ALL, false);
		else if (rf_state == BTC_RFCTRL_LPS_WL_ON &&
			 wl->status.map.lps_pre != BTC_LPS_OFF)
			_update_bt_scbd(btc, true);
	}

	btc->dm.cnt_dm[BTC_DCNT_BTCNT_HANG] = 0;

	if (wl->status.map.lps_pre == BTC_LPS_OFF &&
	    wl->status.map.lps_pre != wl->status.map.lps)
		btc->dm.tdma_instant_excute = 1;
	else
		btc->dm.tdma_instant_excute = 0;

	_run_coex(btc, __func__);
	btc->dm.tdma_instant_excute = 0;
	wl->status.map.rf_off_pre = wl->status.map.rf_off;
	wl->status.map.lps_pre = wl->status.map.lps;
}

static void _ntfy_customerize(struct btc_t *btc, u8 type, u16 len, u8 *buf)
{
	struct btc_bt_info *bt = &btc->cx.bt;

	if (!buf)
		return;

	PHL_INFO("[BTC], %s !! \n", __func__);
	btc->dm.cnt_notify[BTC_NCNT_CUSTOMERIZE]++;

	switch (type) {
	case PHL_BTC_CNTFY_BTINFO:
		if (len != 1)
			return;
		buf[0] = bt->raw_info[BTC_BTINFO_L2];
		break;
	}
}

static u8 _ntfy_wl_rfk(struct btc_t *btc, u8 phy_path, u8 type, u8 state)
{
	struct btc_wl_rfk_info *rfk = &btc->cx.wl.rfk_info;
	bool result = BTC_WRFK_REJECT;

	rfk->type = type;
	rfk->path_map = phy_path & BTC_RFK_PATH_MAP;
	rfk->phy_map = (phy_path & BTC_RFK_PHY_MAP) >> 4;
	rfk->band = (phy_path & BTC_RFK_BAND_MAP) >> 6;
	state &= (BIT(0) | BIT(1));

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s()_1: phy=0x%x, path=0x%x, type=%d, state=%d\n",
		  __func__, rfk->phy_map, rfk->path_map, type, state);

	switch (state) {
	case BTC_WRFK_START:
		result = _chk_wl_rfk_request(btc);
		rfk->state = (result? BTC_WRFK_START : BTC_WRFK_STOP);
		btc->dm.cnt_notify[BTC_NCNT_WL_RFK]++;

		rfk->start_time = _os_get_cur_time_ms();
		break;
	case BTC_WRFK_ONESHOT_START:
	case BTC_WRFK_ONESHOT_STOP:
		return BTC_WRFK_ALLOW;
	case BTC_WRFK_STOP:
		result = BTC_WRFK_ALLOW;
		rfk->state = BTC_WRFK_STOP;

		rfk->proc_time = phl_get_passing_time_ms(rfk->start_time);
		break;
	}

	/* Only update coex for RFK START and STOP
	 * because Start -> OneSHOT_START time is short
	 */

	if (result == BTC_WRFK_ALLOW) {
		_run_coex(btc, __func__);

		if (rfk->state == BTC_WRFK_START) /* wait 300ms for time-out */
			_set_btc_timer(btc, BTC_TIMER_WL_RFKTO, BTC_WRFK_MAXT);
	}

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s()_2: rfk_cnt=%d, result=%d\n",
		  __func__, btc->dm.cnt_notify[BTC_NCNT_WL_RFK], result);

	return result;
}

static void _ntfy_wl_sta(struct btc_t *btc, struct rtw_stats *phl_stats,
			 u8 ntfy_num, struct rtw_phl_stainfo_t *sta[],
			 u8 reason)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_dm *dm = &btc->dm;
	struct btc_module *module = &btc->mdinfo;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	struct btc_traffic *linfo_t = NULL;
	struct btc_wl_link_info *linfo = NULL;
	struct rtw_phl_rainfo ra_info = {0};
	u8 i, j, busy = 0, dir = 0, rssi_map = 0, role_idx = 0;
	u8 busy_all = 0, dir_all = 0, rssi_map_all = 0, rssi_th = 0;
	u8 *rssi_st = NULL;
	bool is_sta_change = false, is_traffic_change = false;
	u16 last_tx_rate, last_rx_rate, last_tx_lvl, last_rx_lvl;
	u32 chk_intvl = 0, pass_time = 0, last_tx_time, last_rx_time;
	s32 dt;
	u64 last_tx_byte, last_rx_byte;
	s64 dbyte;

	/* rssi_map = 4 bits for rssi locate in which {60, 50, 40, 30}
	 * if rssi >= 60% (-50dBm) --> map = 4b'0000 --> rssi_level = 4
	 * if 50% <= rssi < 60%    --> map = 4b'0001 --> rssi_level = 3
	 * if 40% <= rssi < 50%    --> map = 4b'0011 --> rssi_level = 2
	 * if 30% <= rssi < 40%    --> map = 4b'0111 --> rssi_level = 1
	 * if rssi < 20%           --> map = 4b'1111 --> rssi_level = 0
	 */

	dm->cnt_notify[BTC_NCNT_WL_STA]++;

	for (i = 0; i < ntfy_num; i++) {
		/* Extract btc_wl_stat_info from rtw_phl_stainfo_t */
		hal_mem_set(h, &ra_info, 0, sizeof(ra_info));
		role_idx = sta[i]->wrole->id;

		if (role_idx >= BTC_WL_MAX_ROLE_NUMBER)
			continue;

		linfo = &wl->link_info[role_idx];
		linfo_t = &wl->link_info[role_idx].stat.traffic;

		if (linfo->connected == MLME_NO_LINK) {
			linfo->rx_rate_drop_cnt = 0;
			continue;
		}

		/* save the last tx/rx rate/level for compare */
		last_tx_rate = linfo_t->tx_rate;
		last_rx_rate = linfo_t->rx_rate;
		last_tx_lvl = (u16)linfo_t->tx_lvl;
		last_rx_lvl = (u16)linfo_t->rx_lvl;
		last_tx_time = linfo_t->tx_time;
		last_rx_time = linfo_t->rx_time;
		last_tx_byte = linfo_t->tx_byte;
		last_rx_byte = linfo_t->rx_byte;

		/* transfer btc_wl_stat_info to btc_wl_link_info */
		linfo->stat.rssi = sta[i]->hal_sta->rssi_stat.rssi >> 1;
		linfo_t->tx_lvl = phl_stats->tx_traffic.lvl;
		linfo_t->tx_sts = phl_stats->tx_traffic.sts;
#if 0
		linfo_t->tx_1ss_limit = sta[i]->hal_sta->ra_info.ra_nss_limit;
#endif
		linfo_t->rx_lvl = phl_stats->rx_traffic.lvl;
		linfo_t->rx_sts = phl_stats->rx_traffic.sts;
		linfo_t->tx_time = phl_stats->last_tx_time_ms;
		linfo_t->rx_time = phl_stats->last_rx_time_ms;
		linfo_t->tx_byte = phl_stats->tx_byte_total;
		linfo_t->rx_byte = phl_stats->rx_byte_total;

		if (RTW_HAL_STATUS_SUCCESS ==
		    rtw_hal_bb_query_rainfo(h, sta[i]->hal_sta, &ra_info))
			linfo_t->tx_rate = ra_info.rate;
		else
			linfo_t->tx_rate = RTW_DATA_RATE_MAX;

		linfo_t->rx_rate = h->trx_stat.rx_rate_plurality;

		rssi_map = 0;

		/* check if rssi across wl_rssi_thres boundary */
		for (j = 0; j < BTC_WL_RSSI_THMAX; j++) {
			if (module->ant.type == BTC_ANT_SHARED && j == 0)
				rssi_th = BTC_WL_RSSI_MAX_BTG;
			else
				rssi_th = btc->chip->wl_rssi_thres[j];

			rssi_st = &linfo->rssi_state[j];
			*rssi_st = _update_rssi_state(btc, *rssi_st,
						      linfo->stat.rssi,
						      rssi_th);

			/* fill rssi bit map 0~3 if rssi < threshold  */
			if (BTC_RSSI_LOW(*rssi_st))
				rssi_map |= BIT(j);

			if (module->ant.type == BTC_ANT_DEDICATED &&
			    BTC_RSSI_CHANGE(*rssi_st))
				is_sta_change = true;
		}

		/* OR STA/GC role rssi map */
		if (linfo->role == PHL_RTYPE_STATION ||
		    linfo->role == PHL_RTYPE_P2P_GC) {
			rssi_map_all |= rssi_map;

			dt = linfo_t->tx_time - last_tx_time;
			dt += (dt < 0 ? 0xffffffff : 0);
			dbyte = linfo_t->tx_byte - last_tx_byte;
			dbyte += (dbyte < 0 ? 0xffffffffffffffff : 0);
			dt *= 125; /* unit: bytes/ms * 8bit/1000us = 1/125 */

			if (dt > 0)
				linfo_t->tx_tp = (u32)_os_division64(dbyte, dt);

			dt = linfo_t->rx_time - last_rx_time;
			dt += (dt < 0 ? 0xffffffff : 0);
			dbyte = linfo_t->rx_byte - last_rx_byte;
			dbyte += (dbyte < 0 ? 0xffffffffffffffff : 0);
			dt *= 125; /* unit: bytes/ms * 8bit/1000us = 1/125 */

			if (dt > 0)
				linfo_t->rx_tp = (u32)_os_division64(dbyte, dt);

			dm->fddt_trx_info.tx_lvl = linfo_t->tx_lvl;
			dm->fddt_trx_info.rx_lvl = linfo_t->rx_lvl;
			dm->fddt_trx_info.tx_rate = linfo_t->tx_rate;
			dm->fddt_trx_info.rx_rate = linfo_t->rx_rate;
			dm->fddt_trx_info.tx_tp = linfo_t->tx_tp;
			dm->fddt_trx_info.rx_tp = linfo_t->rx_tp;
		}

		pass_time = phl_get_passing_time_ms(linfo->busy_t);

		if (wl_rinfo->link_mode == BTC_WLINK_NOLINK) {
			busy = 0;
			dir = TRAFFIC_DL;
		} else if (linfo_t->tx_lvl != RTW_TFC_IDLE ||
			   linfo_t->rx_lvl != RTW_TFC_IDLE) {
			/* record busy_t as the last busy time */
			linfo->busy_t = _os_get_cur_time_ms();

			/* set busy once idle->busy immediately */
			busy = 1;

			if ((linfo_t->tx_lvl > linfo_t->rx_lvl) ||
			     linfo_t->rx_tp < BTC_WL_TP_LOW_THRES)
				dir = TRAFFIC_UL;
			else
				dir = TRAFFIC_DL;
		} else if (pass_time > BTC_BUSY2IDLE_THRES) {
			/*set idle if busy -> idle after BTC_BUSY2IDLE_THRES*/
			busy = 0;
			dir = TRAFFIC_DL;
		} else { /* if idle time not longer than BTC_BUSY2IDLE_THRES */
			busy = linfo->busy;
			dir = linfo->dir;
		}

		if (linfo->busy != busy || linfo->dir != dir) {
			linfo->busy = busy;
			linfo->dir = dir;
		}

		/* OR all role busy/dir state */
		busy_all |= linfo->busy;
		dir_all |= BIT(linfo->dir);

		if (linfo_t->rx_rate <= RTW_DATA_RATE_CCK2 &&
		    last_rx_rate > RTW_DATA_RATE_CCK2 &&
		    linfo_t->rx_lvl > 0)
			linfo->rx_rate_drop_cnt++;

		if (last_tx_rate != linfo_t->tx_rate ||
		    last_rx_rate != linfo_t->rx_rate ||
		    last_tx_lvl != linfo_t->tx_lvl ||
		    last_rx_lvl != linfo_t->rx_lvl)
			is_traffic_change = true;

		wl_rinfo->active_role[role_idx].tx_lvl = (u16)linfo_t->tx_lvl;
		wl_rinfo->active_role[role_idx].rx_lvl = (u16)linfo_t->rx_lvl;
		wl_rinfo->active_role[role_idx].tx_rate = linfo_t->tx_rate;
		wl_rinfo->active_role[role_idx].rx_rate = linfo_t->rx_rate;
	}

	wl->rssi_level = 4;
	for (j = BTC_WL_RSSI_THMAX; j > 0; j--) {
		/* set RSSI level 4 ~ 0 if rssi bit map match */
		if (rssi_map_all & BIT(j-1)) {
			wl->rssi_level = BTC_WL_RSSI_THMAX - j;
			break;
		}
	}

	chk_intvl = BTC_RPT_PERIOD/BTC_PERIODIC_TIME;
	chk_intvl += dm->cnt_dm[BTC_DCNT_WL_STA_LAST];

	if (dm->cnt_notify[BTC_NCNT_WL_STA] >= chk_intvl) {
		_get_wl_nhm_dbm(btc);
		_get_wl_cn_report(btc);
		//_get_wl_evm_report(btc);
		dm->cnt_dm[BTC_DCNT_WL_STA_LAST] = chk_intvl;
	}

	dm->fddt_trx_info.wl_rssi = wl->rssi_level;

#ifdef BTC_FDDT_TRAIN_SUPPORT
	/* for TDD/FDD packet estimation in WL FW */
	if (dm->fddt_train == BTC_FDDT_ENABLE)
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_TRX);
#endif

	if (wl->status.map.busy != (u32)busy_all ||
	    wl->status.map.traffic_dir != (u32)dir_all) {
		wl->status.map.busy = (u32)busy_all;
		wl->status.map.traffic_dir = (u32)dir_all;
		is_sta_change = true;
	}

	if (is_sta_change) {
		_write_scbd(btc, BTC_WSCB_WLBUSY, (bool)(!!busy_all));
		_run_coex(btc, __func__);
	}
}

static void _ntfy_fwinfo(struct btc_t *btc, u8 *buf, u32 len, u8 cls, u8 func)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;

	if (!buf || !len)
		return;

	btc->dm.cnt_notify[BTC_NCNT_FWINFO]++;
	pfwinfo->cnt_c2h++;

	if (cls == BTFC_FW_EVENT) {
		switch (func) {
		case BTF_EVNT_RPT:
		case BTF_EVNT_BUF_OVERFLOW:
			pfwinfo->event[func]++;
			hal_btc_fw_event(btc, func, buf, len);
			break;
		case BTF_EVNT_BT_INFO:
			btc->cx.cnt_bt[BTC_BCNT_INFOUPDATE]++;
			_update_bt_info(btc, buf, len);
			break;
		case BTF_EVNT_BT_SCBD:
			btc->cx.cnt_bt[BTC_BCNT_SCBDUPDATE]++;
			_update_bt_scbd(btc, false);
			break;
		case BTF_EVNT_BT_PSD:
			_bt_psd_update(btc, buf, len);
			break;
		case BTF_EVNT_BT_REG:
			btc->dbg.rb_done = true;
			btc->dbg.rb_val = ((buf[3] << 24) | (buf[2] << 16) |
					   (buf[1] << 8) | (buf[0]));
			break;
		case BTF_EVNT_C2H_LOOPBACK:
			btc->dbg.rb_done = true;
			btc->dbg.rb_val = buf[0];
			break;
		case BTF_EVNT_CX_RUNINFO:
			btc->dm.cnt_dm[BTC_DCNT_CX_RUNINFO]++;
			_update_offload_runinfo(btc, buf, len);
			break;
		case BTF_EVNT_BT_DEV_INFO:
			_update_bt_dev_info(btc, buf, len);
			break;
		}
	}
}

static void _ntfy_timer(struct btc_t *btc, u16 tmr_id)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_bt_a2dp_desc *a2dp = &cx->bt.link_info.a2dp_desc;
	bool is_sta_change = false;

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): tmr_id =%d\n", __func__, tmr_id);

	dm->cnt_notify[BTC_NCNT_TIMER]++;

	switch (tmr_id) {
	case BTC_TIMER_PERIODIC:
		/* start next periodic timer */
		_set_btc_timer(btc, BTC_TIMER_PERIODIC, BTC_PERIODIC_TIME);
		break;
	case BTC_TIMER_WL_RFKTO:
		if (wl->rfk_info.state == BTC_WRFK_STOP)
			return;

		cx->cnt_wl[BTC_WCNT_RFK_TIMEOUT]++;
		dm->error.map.wl_rfk_timeout = true;
		wl->rfk_info.state = BTC_WRFK_STOP;
		/* _write_scbd(btc, BTC_WSCB_WLRFK, false); */
		is_sta_change = true;
		break;
	case BTC_TIMER_WL_SPECPKT:
		wl->status.map._4way = 0;
		wl->status.map.connecting = 0;
		is_sta_change = true;
		break;
	case BTC_TIMER_BT_A2DPPLAY:
		a2dp->play_latency = 0;
		is_sta_change = true;
		break;
	}

	if (is_sta_change)
		_run_coex(btc, __func__);
}

/******************************************************************************
 *
 * coexistence extern functions
 *
 *****************************************************************************/
/*
 * btc related sw initialization
 */
bool hal_btc_init(struct btc_t *btc)
{
	switch (btc->hal->chip_id) {
#ifdef BTC_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		PHL_INFO("[BTC], %s(): Init 8852A!!\n", __func__);
		btc->chip = &chip_8852a;
		break;
#endif
#if defined(BTC_8852B_SUPPORT) || defined(BTC_8852BP_SUPPORT) || defined(BTC_8851B_SUPPORT)
	case CHIP_WIFI6_8852B:
		PHL_INFO("[BTC], %s(): Init 8852B!!\n", __func__);
		btc->chip = &chip_8852b;
		break;
	case CHIP_WIFI6_8852BP:
		PHL_INFO("[BTC], %s(): Init 8852BP!!\n", __func__);
		btc->chip = &chip_8852b;
		break;
	case CHIP_WIFI6_8851B:
		PHL_INFO("[BTC], %s(): Init 8851B!!\n", __func__);
		btc->chip = &chip_8852b;
		break;
#endif
#ifdef BTC_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		PHL_INFO("[BTC], %s(): Init 8852C!!\n", __func__);
		btc->chip = &chip_8852c;
		break;
#endif
	default:
		PHL_ERR("[BTC], %s(): no matched IC!!\n", __func__);
		btc->cx.wl.status.map.init_ok = false;
		return false;
	}

	_reset_btc_var(btc, BTC_RESET_ALL);
	_btmr_init(btc);

	_rsn_cpy(btc->dm.run_reason, "None", _os_strlen((u8 *)"None")+1);
	_act_cpy(btc->dm.run_action, "None", _os_strlen((u8 *)"None")+1);

	btc->hal->btc_vc.btc_ver = coex_ver;
	btc->ops = &_btc_ops;
	btc->mlen = BTC_MSG_MAXLEN;
	btc->ctrl.igno_bt = true;
	btc->cx.wl.status.map.init_ok = true;

	return true;
}

void hal_btc_deinit(struct btc_t *btc)
{
	_btmr_deinit(btc);
}

#endif

#ifdef BTC_CONFIG_FW_IO_OFLD_SUPPORT
void _btc_io_ofld (struct btc_t *btc, u32 srcpath, u32 addr, u32 mask,
		       u32 value, bool trigger)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct rtw_mac_cmd cmd = {0};
	u32 ret;
	u8 src_grp;

	/* 0: BB, 1:RF, 2:MAC */
	src_grp = srcpath & 0x3;
	if (src_grp == RTW_MAC_RF_CMD_OFLD)
		cmd.rf_path = srcpath >> 8;

	cmd.src = src_grp;
	cmd.type = RTW_MAC_WRITE_OFLD;
	cmd.lc = trigger;
	cmd.offset = (u16)addr;
	cmd.value = value;
	cmd.mask = mask;
	ret = rtw_hal_mac_add_cmd_ofld(h, &cmd);
}
#endif

