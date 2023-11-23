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

#ifndef _HALBB_PMAC_SETTING_H_
#define _HALBB_PMAC_SETTING_H_

/*  ============================================================
 			Enumeration
    ============================================================
 */

enum halbb_pmac_mode {
	NONE_TEST,
	PKTS_TX,
	PKTS_RX,
	CONT_TX,
	FW_TRIG_TX,
	OFDM_SINGLE_TONE_TX,
	CCK_CARRIER_SIPPRESSION_TX
};
/*  ============================================================
 		     Function Prototype
    ============================================================
*/

struct bb_info;

void halbb_pmac_tx_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);

#endif
