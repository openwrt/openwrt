/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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
#ifndef __HALBB_DYN_DTR_H__
#define __HALBB_DYN_DTR_H__

struct bb_dyn_dtr_info {
	bool			dtr_trig_by_timer_en;
	bool			dyn_dtr_en;
	bool			dyn_dtr_acc_en;
	u8				dyn_dtr_rssi_th_2g;
	u8				dyn_dtr_rssi_th_5g;
	u8				dtr_period_cnt;
	u8				dyn_dtr_rssi_th_bkf_2g;
	u8				dyn_dtr_rssi_th_bkf_5g;

	struct halbb_timer_info dtr_timer_i;
};

struct bb_info;

void halbb_dtr_acc_io_en(struct bb_info *bb);
void halbb_dtr_acc_timer_init(struct bb_info *bb);
void halbb_dtr_deinit(struct bb_info *bb);

void halbb_dyn_dtr_en(struct bb_info *bb, bool en);
void halbb_dyn_dtr_init(struct bb_info *bb);
void halbb_dyn_dtr_main(struct bb_info *bb);
void halbb_dyn_dtr_watchdog(struct bb_info *bb);
void halbb_dyn_dtr_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);

#endif
