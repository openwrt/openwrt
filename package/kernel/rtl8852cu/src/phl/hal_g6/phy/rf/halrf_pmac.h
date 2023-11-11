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
#ifndef _HALRF_PMAC_H_
#define _HALRF_PMAC_H_
/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
struct rf_pmac_tx_info {
	enum packet_format_t ppdu;
	u8 mcs;
	u8 bw;
	u8 nss;
	u8 long_preamble_en;
	u8 gi;
	u16 case_id;
	s16 dbm;
	u16 txagc_cw;
	u8 is_cck;
	u16 cnt;
	u16 time;
	u16 period;
	u16 length;
};

/*@--------------------------[Prptotype]-------------------------------------*/
void halrf_set_pmac_cw(struct rf_info *rf, enum rf_path path, u16 txagc_cw, bool en);

void halrf_set_pmac_tx(struct rf_info *rf, enum phl_phy_idx phy_idx,
			enum rf_path path, struct rf_pmac_tx_info *tx,
			u8 enable, bool by_cw);

void halrf_set_pseudo_cw(struct rf_info *rf, enum rf_path path,
			u16 txagc_cw, bool en);

#if 0
void halrf_set_pmac_tx(struct rf_info *rf, enum phl_phy_idx phy_idx,
						enum rf_path path, u8 ppdu_type, u8 case_id, s16 dbm,
						u8 enable, u8 is_cck, u16 cnt ,u16 time, u16 period);
#endif

#endif
