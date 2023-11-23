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

#ifdef HALBB_DYN_CSI_RSP_SUPPORT

bool halbb_dyn_csi_rsp_rlt_get(struct bb_info *bb){
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	
	BB_DBG(bb, DBG_DCR, "CSI Rsp Rlt = %d.\n", bf->is_csi_rsp_en);
	return bf->is_csi_rsp_en;
}

void halbb_csi_rsp_rlt(struct bb_info *bb, bool en)
{
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	enum dcr_csi_rsp;

	bool ret=false;
	
	if (en == bf->is_csi_rsp_en)
		return;
	
	bf->is_csi_rsp_en = en;
#if 0
	if (en) {
		BB_DBG(bb, DBG_DCR,"[Enable CSI Rsp.]\n");
		//Enable CSI Rsp.
		//val = hal_read8(bb->hal_com,  0xcd80);
		//val = val | 0x7;         
		//hal_write8(bb->hal_com, 0xcd80, val);
		//ret = rtw_hal_mac_ax_init_bf_role(bb->hal_com, 0, bb->bb_phy_idx);
		//return dcr_csi_rsp_dis;
	} else {
		BB_DBG(bb, DBG_DCR,"[Disable CSI Rsp.]\n");
		//Disable CSI Rsp.
		//val = hal_read8(bb->hal_com,  0xcd80);
		//val = val & ~(0x7);         
		//hal_write8(bb->hal_com, 0xcd80, val);
		//ret = rtw_hal_mac_ax_deinit_bfee(bb->hal_com, bb->bb_phy_idx);
		//return dcr_csi_rsp_en;
	}
#endif
}

bool halbb_dcr_is_he_connect(struct bb_info *bb) {
	struct rtw_phl_stainfo_t *sta;
	struct bb_link_info *link = &bb->bb_link_i;
	bool rlt = false;
	
	if (!link->is_linked) {
		return false;
	}

	if (!link->is_one_entry_only)
		return false;

	sta = bb->phl_sta_info[bb->bb_link_i.one_entry_macid];
	
	if (sta && sta->wmode & WLAN_MD_11AX) {
		BB_DBG(bb, DBG_DCR, "AX Support!!!!");
		rlt = true;

	}
	return rlt;
}	

void halbb_dcr_config_ch_info_he(struct bb_info *bb) {
	struct bb_ch_rpt_info *ch_rpt = &bb->bb_ch_rpt_i;
	struct bb_ch_info_cr_cfg_info *cfg = &ch_rpt->bb_ch_info_cr_cfg_i;
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	enum channel_width bw = bb->hal_com->band[0].cur_chandef.bw;

	if (bf->dcr_bw == bw)
		return;

	bf->dcr_bw = bw;
	BB_DBG(bb, DBG_IC_API, "dcr_bw=%d\n", bf->dcr_bw);

	if (bw == CHANNEL_WIDTH_80) {
		cfg->ch_i_grp_num_he = 3;
	} else if (bw == CHANNEL_WIDTH_40) {
		cfg->ch_i_grp_num_he = 2;
	} else { /*if (bw == CHANNEL_WIDTH_20)*/
		if (bb->ic_type & (BB_RTL8852B | BB_RTL8851B))
			cfg->ch_i_grp_num_he = 1;
		else
			cfg->ch_i_grp_num_he = 2;
	}
	BB_DBG(bb, DBG_IC_API, "grp_num_he=%d\n", cfg->ch_i_grp_num_he);

	cfg->ch_i_cmprs = 1;
	cfg->ch_i_ele_bitmap = 0x303; /*Nr X Nc: 2 X 2*/

}

bool halbb_dcr_get_ch_raw_info(struct bb_info *bb, bool is_csi_en)
{	
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	struct bb_ch_info_physts_info *ch_physts = &bb->bb_ch_rpt_i.bb_ch_info_physts_i;
	bool rpt = false;
	
	if (is_csi_en) {
		BB_DBG(bb, DBG_DCR, "CSI Rsp enable need to disable for CH Est.\n");
		/* Disable CSI Rsp*/
		rtw_hal_mac_ax_deinit_bfee(bb->hal_com, bb->bb_phy_idx);
		halbb_delay_ms(bb, bf->ch_est_dly);
	}

	rpt = halbb_ch_info_wait_from_physts(bb, bf->get_phy_sts_dly, bf->get_phy_sts_dly*5, HE_PKT, false);

	if (rpt && ch_physts->ch_info_len < 200)
		rpt = false;

	if (is_csi_en) {
		BB_DBG(bb, DBG_DCR, "Restore CSI Rsp.\n");
		/* enable CSI Rsp*/
		rtw_hal_mac_ax_init_bf_role(bb->hal_com, 0, bb->bb_phy_idx);
	}

	return rpt;
}

bool halbb_dcr_en(struct bb_info *bb, bool en){
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	bool ret = true;
	u32 id = bb->phl_com->id.id & 0xFFFF;

	if (id == 0x209 || id == 0x309) {
	    BB_DBG(bb, DBG_DCR, "DCR_en=%d, cid=0x%x\n", en, id);
	} else {
		return false;
	}

	if (en) {
		//Allocate Buffer
		//ret = halbb_ch_info_buf_alloc(bb);
		//if (ret) {
			bf->dyn_csi_rsp_en = true;
		//}
	} else {
		//halbb_ch_info_buf_rls(bb);
		halbb_dcr_reset(bb);
		bf->dyn_csi_rsp_en = false;
	}

	return ret;
}

void halbb_dcr_init(struct bb_info *bb)
{
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;

	BB_DBG(bb, DBG_DCR, "%s\n", __func__);

	bf->dyn_csi_rsp_en = false;
	bf->is_csi_rsp_en = true;
	bf->ch_est_dly= 50;
	bf->get_phy_sts_dly = 5;
	bf->max_est_tone_num = 54;
	bf->ch_chk_cnt = 0;
	bf->dyn_csi_rsp_dbg_en = 0;
	bf->dcr_bw = CHANNEL_WIDTH_MAX;
}

void halbb_dcr_reset(struct bb_info *bb)
{
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	
	bf->ch_chk_cnt = 0;
	bf->is_csi_rsp_en = true;
}

bool halbb_dcr_abort(struct bb_info *bb)
{
	struct bb_link_info *link = &bb->bb_link_i;
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;

	if (!bf->dyn_csi_rsp_en) {
		BB_DBG(bb, DBG_DCR,"[Dyn CSI RSP DISABLE]\n");
		halbb_csi_rsp_rlt(bb, true);
		return true;
	}

	if (!link->is_one_entry_only) {
		BB_DBG(bb, DBG_DCR,"[is_one_entry_only = 0]\n");
		halbb_csi_rsp_rlt(bb, true);
		return true;
	}

	if (!link->is_linked) {
		if (link->first_disconnect)
			halbb_dcr_reset(bb);
		return true;
	}

	if(bf->dyn_csi_rsp_dbg_en == 1){
		BB_DBG(bb, DBG_DCR,"[Disable by echo cmd for dbg]\n");
		halbb_csi_rsp_rlt(bb, true);
		return true;
	}

	if (!halbb_dcr_is_he_connect(bb)) {
		//Disable if not HE Mode
		BB_DBG(bb, DBG_DCR, "DCR disable cause not in HE mode!\n");
		halbb_csi_rsp_rlt(bb, true);
		return true;
	}

	return false;
}

bool halbb_dcr_ch_est(struct bb_info *bb, u16 *addr)
{
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	u32 ix;
	u32 h11h22, h12h21, chdiff;
	s32 utility = 0;
	bool iscablelink = false;

	for (ix = 0; ix < bf->max_est_tone_num; ix++) {
#if 0
		BB_DBG(bb, DBG_DCR,"Tone Group Idx = %d\n",ix);
		BB_DBG(bb, DBG_DCR,"H11 = %s%d.%d + %sj%d.%d",
			  ((addr[ix*8+0] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+0]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+0]) & 0xfff, 12),
			  ((addr[ix*8+1] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+1]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+1]) & 0xfff, 12));
		BB_DBG(bb, DBG_DCR,"H22 = %s%d.%d + %sj%d.%d",
			  ((addr[ix*8+6] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+6]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+6]) & 0xfff, 12),
			  ((addr[ix*8+7] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+7]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+7]) & 0xfff, 12));
		BB_DBG(bb, DBG_DCR,"H12 = %s%d.%d + %sj%d.%d",
			  ((addr[ix*8+2] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+2]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+2]) & 0xfff, 12),
			  ((addr[ix*8+3] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+3]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+3]) & 0xfff, 12));
		BB_DBG(bb, DBG_DCR,"H21 = %s%d.%d + %sj%d.%d",
			  ((addr[ix*8+4] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+4]) >> 12),
			  halbb_show_fraction_num((u16)ABS_16(addr[ix*8+4]) & 0xfff, 12),
			  ((addr[ix*8+4] & BIT15)!= 0) ? "-": " ",
			  ((u16)ABS_16(addr[ix*8+5]) >> 12),
			   halbb_show_fraction_num((u16)ABS_16(addr[ix*8+5]) & 0xfff, 12));
#endif
		h11h22 = ((u16)ABS_16(addr[ix*8+0]))+((u16)ABS_16(addr[ix*8+1]))
			+((u16)ABS_16(addr[ix*8+6]))+((u16)ABS_16(addr[ix*8+7]));
		h12h21 =((u16)ABS_16(addr[ix*8+2]))+((u16)ABS_16(addr[ix*8+3]))
			+((u16)ABS_16(addr[ix*8+4]))+((u16)ABS_16(addr[ix*8+5]));
		
		if (h11h22 != 0 && h12h21 != 0) {
			chdiff = DIFF_2(h11h22, h12h21);
			//BB_DBG(bb, DBG_DCR, "H11H22 = %d, H12H22= %d ,Diff= %d\n", h11h22, h12h21, chdiff);
			if (chdiff > 10000)
				utility += 2;
			else if (chdiff > 5000)
				utility += 1;
			else
				utility -= 2;
		}
	}

	if (utility > 0)
		iscablelink = true;

	BB_DBG(bb, DBG_DCR, "utility = %d, isCableLink = %d\n", 
	       utility, iscablelink);

	return iscablelink;
}

void halbb_dyn_csi_rsp_main(struct bb_info *bb)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	struct bb_ch_info_raw_info *buf = &bb->bb_ch_rpt_i.bb_ch_info_raw_i;
	struct bb_ch_info_physts_info *physts = &bb->bb_ch_rpt_i.bb_ch_info_physts_i;
	u16 pkt_cnt_ss = 0;
	//u8 i = 0, j =0;
	//u8 rate_num = bb->num_rf_path, ss_ofst = 0;
	u8 ret = dcr_csi_rsp_en;
	bool iscablelink = false, is_csi_rsp_en = true;

	if (halbb_dcr_abort(bb))
		return;

#if 1
	if (pkt_cnt->he_pkt_not_zero) {
		halbb_dcr_config_ch_info_he(bb);
		pkt_cnt_ss = pkt_cnt->pkt_cnt_t;
	}
#else
	//Cal HE RX PKT CNT
	for (i = 0; i < rate_num; i++) {
		ss_ofst = HE_VHT_NUM_MCS * i;
		for (j = 0; j < HE_VHT_NUM_MCS ; j++) {
			pkt_cnt_ss += pkt_cnt->pkt_cnt_he[ss_ofst + j];
			
		}
	}
#endif

	BB_DBG(bb, DBG_DCR,"[CH Est. dly = %d,Get Physts dly = %d, rpt get = %d, CH chk cnt =%d, HE Pkt=%03d]\n",
		bf->ch_est_dly, bf->get_phy_sts_dly, physts->ch_info_state,
		bf->ch_chk_cnt, pkt_cnt_ss);

	if ((bf->ch_chk_cnt >= 3) && (pkt_cnt_ss > 400)) {
		BB_DBG(bb, DBG_DCR,"[No Need to Chk Ch and Rx Cnt > 400]\n");
		halbb_csi_rsp_rlt(bb, true);
		return;
	
	}

	if (!halbb_dcr_get_ch_raw_info(bb, bf->is_csi_rsp_en)) { /*get report fail*/
		BB_DBG(bb, DBG_DCR,"[Failed To Get Report]\n");
		if (bf->ch_chk_cnt > 0)
			bf->ch_chk_cnt--;
		//halbb_csi_rsp_rlt(bb, true);
		return;
	}

	//CSI RAW INFO Get
	BB_DBG(bb, DBG_DCR,"[Rpt Get !!!]\n");
	if (bf->ch_chk_cnt >= 3) {
		BB_DBG(bb, DBG_DCR,"[No Need to Check Channel]\n");
		halbb_csi_rsp_rlt(bb, true);
		return;
	}

	//check if cable link
	iscablelink = halbb_dcr_ch_est(bb, buf->octet);
	halbb_mem_set(bb, buf->octet, 0, buf->ch_info_buf_len);

	if (iscablelink) {
		BB_DBG(bb, DBG_DCR,"[CSI Rsp. Disable !!!]\n");
		is_csi_rsp_en = false;

		if (bf->ch_chk_cnt > 0)
			bf->ch_chk_cnt--;
		ret = dcr_csi_rsp_dis;
	} else {
		BB_DBG(bb, DBG_DCR,"[CSI Rsp. Enable !!!]\n");
		is_csi_rsp_en = true;
		if (bf->ch_chk_cnt < 3)
			bf->ch_chk_cnt++;
		ret = dcr_csi_rsp_en;
	}
	halbb_csi_rsp_rlt(bb, is_csi_rsp_en);
}

bool halbb_dcr_rssi_chk(struct bb_info *bb)
{
	u8 rssi_min = bb->bb_ch_i.rssi_min >> 1;
	bool csi_enable = true;

	/*When RSSI below -75 dBm, CSI feedback turned off*/
	if (rssi_min < 35)
		csi_enable = false;

	return csi_enable;
}

void halbb_dyn_csi_rsp_dbg(struct bb_info *bb, char input[][16], 
				    u32 *_used, char *output, u32 *_out_len)
{
	struct bb_ch_info_physts_info *physts = &bb->bb_ch_rpt_i.bb_ch_info_physts_i;
	struct bf_ch_raw_info *bf = &bb->bb_cmn_hooker->bf_ch_raw_i;
	u32 val[10] = {0};
	bool dcr_en = false;
	bool bool_tmp;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &val[0]);

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "{0: Config Channel Est Delay Unit:ms\n");
	} else if (val[0] == 1) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[1]);
		if (val[1] == 1){
			bf->dyn_csi_rsp_dbg_en = 0;
		} else {
			bf->dyn_csi_rsp_dbg_en = 1;
		}
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "Dynamic CSI Rsp Enable = %d\n",
			    bf->dyn_csi_rsp_dbg_en);
	} else if (val[0] == 2) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "Config Trainning Delay \n");
		bf->ch_est_dly = (u32)val[1];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "Trainning Delay = %d ms \n",
			    bf->ch_est_dly);
	} else if (val[0] == 3) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "Config Get PHY STS Delay \n");

		bf->get_phy_sts_dly = (u32)val[1];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "GetPhySTS = %d ms \n",
			    bf->get_phy_sts_dly);

	} else if (val[0] == 4) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[1]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "Config Max Tone Cnt \n");

		bf->max_est_tone_num = (u32)val[1];

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, 
			    *_out_len - *_used, "Config Max Tone Cnt = %d \n",
				    bf->max_est_tone_num);
	} else if (val[0] == 5) {
		bool_tmp = halbb_dcr_is_he_connect(bb);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "is_he_connect=%d\n", bool_tmp);
	}
}
#endif
