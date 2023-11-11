/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "halbb_precomp.h"

#ifdef HALBB_DYN_DTR_SUPPORT

void halbb_dyn_dtr_en(struct bb_info *bb, bool en)
{
	
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;

	bb_dyn_dtr->dyn_dtr_en = en;
}

void halbb_dyn_dtr_init(struct bb_info *bb)
{
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;

	bb_dyn_dtr->dyn_dtr_en = false;
	bb_dyn_dtr->dyn_dtr_rssi_th_5g = 67;
	bb_dyn_dtr->dyn_dtr_rssi_th_2g = 67;
	bb_dyn_dtr->dtr_period_cnt = 15;
	bb_dyn_dtr->dyn_dtr_rssi_th_bkf_5g = 2;
	bb_dyn_dtr->dyn_dtr_rssi_th_bkf_2g = 2;

	bb_dyn_dtr->dyn_dtr_acc_en = (dev->rfe_type < 50) ? true : false;
	bb_dyn_dtr->dtr_trig_by_timer_en = false;
}

void halbb_dtr_acc_io_en(struct bb_info *bb)
{
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;

	BB_DBG(bb, DBG_DBG_API, "[%s]===>\n", __func__);


	if (!bb_dyn_dtr->dtr_trig_by_timer_en)
		return;

	halbb_dyn_dtr_main(bb);
	halbb_cfg_timers(bb, BB_SET_TIMER, &bb->bb_dyn_dtr_i.dtr_timer_i);

}

void halbb_dtr_acc_callback(void *context)
{
	struct bb_info *bb = (struct bb_info *)context;
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;
	struct halbb_timer_info *timer = &bb->bb_dyn_dtr_i.dtr_timer_i;

	BB_DBG(bb, DBG_DBG_API, "[%s]===>\n", __func__);

	timer->timer_state = BB_TIMER_IDLE;

	if (bb->phl_com->hci_type == RTW_HCI_PCIE)
		halbb_dtr_acc_io_en(bb);
	else
		rtw_hal_cmd_notify(bb->phl_com, MSG_EVT_NOTIFY_BB, (void *)(&timer->event_idx), bb->bb_phy_idx);
}

void halbb_dtr_acc_timer_init(struct bb_info *bb)
{
	struct halbb_timer_info *timer = &bb->bb_dyn_dtr_i.dtr_timer_i;

	BB_DBG(bb, DBG_DBG_API, "[%s]\n", __func__);

	timer->event_idx = BB_EVENT_TIMER_DTR;
	timer->timer_state = BB_TIMER_IDLE;

	halbb_init_timer(bb, &timer->timer_list, halbb_dtr_acc_callback, bb, "halbb_dtr_timer");
}

void halbb_dtr_deinit(struct bb_info *bb)
{
	BB_DBG(bb, DBG_DBG_API, "halbb_dtr_deinit");
}

bool halbb_dtr_acc_mode_en(struct bb_info *bb)
{
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;

	if (!bb_dyn_dtr->dyn_dtr_acc_en)
		return false;

	halbb_dtr_acc_io_en(bb);

	return bb_dyn_dtr->dtr_trig_by_timer_en;

}

void halbb_dyn_dtr_main(struct bb_info *bb)
{
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;
	struct rtw_hw_band *hw_band = &bb->hal_com->band[0];
	u8 dyn_dtr_rssi, dyn_dtr_rssi_th_bkf;

	if (!bb_dyn_dtr->dyn_dtr_en) {
		halbb_set_reg(bb, 0x4454, BIT(9), 0x0);
		return;
	}

	if (!link->is_linked)
		return;

	if (!link->is_one_entry_only)
		return;

	BB_DBG(bb, DBG_DBG_API, "[%s]\n", __func__);

	if (hw_band->cur_chandef.band == BAND_ON_24G){
		dyn_dtr_rssi = bb->bb_dyn_dtr_i.dyn_dtr_rssi_th_2g;
		dyn_dtr_rssi_th_bkf = bb->bb_dyn_dtr_i.dyn_dtr_rssi_th_bkf_2g;
	}else{
		dyn_dtr_rssi = bb->bb_dyn_dtr_i.dyn_dtr_rssi_th_5g;
		dyn_dtr_rssi_th_bkf = bb->bb_dyn_dtr_i.dyn_dtr_rssi_th_bkf_5g;
	}
	if ((bb->bb_physts_i.bb_physts_rslt_hdr_i.rssi_avg >> 1) > dyn_dtr_rssi) {
		// RSSI > th, enable DTR for 1ss 1R 
		halbb_set_reg(bb, 0x4454, BIT(9), 0x1);
		BB_DBG(bb, DBG_DBG_API, "[DYN DTR] rssi=0x%x, Enable, 1ss w/ 1R\n", bb->bb_physts_i.bb_physts_rslt_hdr_i.rssi_avg >> 1);
	} else if ((bb->bb_physts_i.bb_physts_rslt_hdr_i.rssi_avg >> 1) < (dyn_dtr_rssi - dyn_dtr_rssi_th_bkf)) {
		// RSSI < th - 2, disable DTR for 1ss 2R
		halbb_set_reg(bb, 0x4454, BIT(9), 0x0);
		BB_DBG(bb, DBG_DBG_API, "[DYN DTR] rssi=0x%x, Disable, 1ss w/ 2R\n", bb->bb_physts_i.bb_physts_rslt_hdr_i.rssi_avg >> 1);
	}

}

void halbb_dyn_dtr_watchdog(struct bb_info *bb)
{
	if (halbb_dtr_acc_mode_en(bb))
		return;

	BB_DBG(bb, DBG_DBG_API, "[%s]\n", __func__);
	halbb_dyn_dtr_main(bb);
	BB_DBG(bb, DBG_DBG_API, "\n");
}

void halbb_dyn_dtr_dbg(struct bb_info *bb, char input[][16], u32 *_used,
				      char *output, u32 *_out_len)
{
	struct bb_dyn_dtr_info *bb_dyn_dtr = &bb->bb_dyn_dtr_i;
	u32 var[10] = {0};
	bool	timer_en_pre;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "dtr_en {0:disable, 1:enable}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "rssi_th {0:2g, 1:5g} {th value} {th_bkf value}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "acc {en} {ms} {period_cnt}\n");

		return;
	}
	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);

	if (_os_strcmp(input[1], "dtr_en") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		halbb_dyn_dtr_en(bb, (bool)var[0]);
		if (var[0] == 1)
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					"DYN DTR Enable\n");
		else
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					"DYN DTR Disable\n");

	} else if (_os_strcmp(input[1], "rssi_th") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[2]);

		if (var[0] == 0) {
			bb_dyn_dtr->dyn_dtr_rssi_th_2g = (u8)(var[1]);
			bb_dyn_dtr->dyn_dtr_rssi_th_bkf_2g = (u8)(var[2]);
		} else if (var[0] == 1) {
			bb_dyn_dtr->dyn_dtr_rssi_th_5g = (u8)(var[1]);
			bb_dyn_dtr->dyn_dtr_rssi_th_bkf_5g = (u8)(var[2]);
		}

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			   		"[Dyn DTR]en=%d, rssi_th_2g=%d, rssi_th_bkf_2g=%d,rssi_th_5g=%d, rssi_th_bkf_5g=%d\n",
					bb_dyn_dtr->dyn_dtr_en,
					bb_dyn_dtr->dyn_dtr_rssi_th_2g,
					bb_dyn_dtr->dyn_dtr_rssi_th_bkf_2g,
					bb_dyn_dtr->dyn_dtr_rssi_th_5g,
					bb_dyn_dtr->dyn_dtr_rssi_th_bkf_5g);
	} else if (_os_strcmp(input[1], "acc") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &var[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &var[2]);

		timer_en_pre = bb_dyn_dtr->dtr_trig_by_timer_en;
		bb_dyn_dtr->dtr_trig_by_timer_en = (bool)var[0];

		if (var[1] > 2000)
			bb_dyn_dtr->dtr_timer_i.cb_time = 2000;
		else if (var[1] < 5)
			bb_dyn_dtr->dtr_timer_i.cb_time = 5;
		else
			bb_dyn_dtr->dtr_timer_i.cb_time = (u16)var[1];

		bb_dyn_dtr->dtr_period_cnt = (bool)var[2];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "timer=%d ms, period_cnt=%d cnt, trig_by_timer_en=%d\n",
			    bb_dyn_dtr->dtr_timer_i.cb_time, bb_dyn_dtr->dtr_period_cnt, bb_dyn_dtr->dtr_trig_by_timer_en);

		if (!timer_en_pre && bb_dyn_dtr->dtr_trig_by_timer_en) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Start callback]\n");
			halbb_dtr_acc_io_en(bb);
		}
	}


}

#endif
