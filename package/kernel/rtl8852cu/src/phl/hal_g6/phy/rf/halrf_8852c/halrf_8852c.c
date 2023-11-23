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
#include "../halrf_precomp.h"
#ifdef RF_8852C_SUPPORT
void halrf_adc_fifo_rst_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx, u8 path)
{
	halrf_wreg(rf, 0x20fc, BIT(16) << path, 0x1);
	halrf_wreg(rf, 0x20fc, BIT(20) << path, 0x0);
	halrf_wreg(rf, 0x20fc, BIT(24) << path, 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28) << path, 0x0);
	halrf_delay_us(rf, 10);	
	halrf_wreg(rf, 0x20fc, BIT(20) << path, 0x1);
	halrf_wreg(rf, 0x20fc, BIT(28) << path, 0x1);
	
}

bool halrf_bw_setting_8852c(struct rf_info *rf, enum rf_path path, enum channel_width bw, bool is_dav)
{
	u32 rf_reg18 = 0;
	u32 reg_reg18_addr = 0x0;

	RF_DBG(rf, DBG_RF_RFK, "[RFK]===> %s\n", __func__);	
	if(is_dav)
		reg_reg18_addr =0x18;
	else
		reg_reg18_addr = 0x10018;

	rf_reg18 = halrf_rrf(rf, path, reg_reg18_addr, MASKRF);
	/*==== [Error handling] ====*/
	if (rf_reg18 == INVALID_RF_DATA) {		
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Invalid RF_0x18 for Path-%d\n", path);
		return false;
	}
	rf_reg18 &= ~(BIT(11) | BIT(10));
	/*==== [Switch bandwidth] ====*/
	switch (bw) {
		case CHANNEL_WIDTH_5:
		case CHANNEL_WIDTH_10:
		case CHANNEL_WIDTH_20:
			rf_reg18 |= (BIT(11) | BIT(10));
			//halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x3);
			//halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xf);
			break;
		case CHANNEL_WIDTH_40:
			rf_reg18 |= BIT(11);			
			//halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x3);
			//halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xf);
			break;
		case CHANNEL_WIDTH_80:
			rf_reg18 |= BIT(10);
			//halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x2);
			//halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xd);
			break;	
		case CHANNEL_WIDTH_160:
			//halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x1);
			//halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xb);
			break;
		default:
			RF_DBG(rf, DBG_RF_RFK, "[RFK] Fail to set CH\n");
			break;
	}

	/*==== [Write RF register] ====*/
	rf_reg18 = (u32)(rf_reg18 & 0xf0fff) | BIT(12);
	halrf_wrf(rf, path, reg_reg18_addr, MASKRF, rf_reg18);
	RF_DBG(rf, DBG_RF_RFK, "[RFK] set %x at path%d, %x =0x%x\n",bw, path, reg_reg18_addr, halrf_rrf(rf, path, reg_reg18_addr, MASKRF));
	return true;
}

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
bool halrf_bw_setting_io_ofld_8852c(struct rf_info *rf, enum phl_phy_idx phy, enum channel_width bw)
{
//	u32 rf_reg18 = 0;
//	u32 reg_reg18_addr = 0x0;
	u8 i = 0;
	u8 kpath, path;
	u32 tmp = 0;

	u32 rf_reg[2][2] = {{0}}; //path, regvalue
	u32 reg_reg_addr[2] = {0x18, 0x10018};

	kpath = halrf_kpath_8852c(rf, phy);

	RF_DBG(rf, DBG_RF_RFK, "[RFK]===> %s\n", __func__);	
#if 0
	if(is_dav)
		reg_reg18_addr =0x18;
	else
		reg_reg18_addr = 0x10018;

	rf_reg18 = halrf_rrf(rf, path, reg_reg18_addr, MASKRF);
#endif
	rf_reg[RF_PATH_A][0] = halrf_rrf(rf, RF_PATH_A, reg_reg_addr[0], MASKRF);
	rf_reg[RF_PATH_A][1] = halrf_rrf(rf, RF_PATH_A, reg_reg_addr[1], MASKRF);
	rf_reg[RF_PATH_B][0] = halrf_rrf(rf, RF_PATH_B, reg_reg_addr[0], MASKRF);
	rf_reg[RF_PATH_B][1] = halrf_rrf(rf, RF_PATH_B, reg_reg_addr[1], MASKRF);

	for (path = 0; path < 2; path++) {
		if (kpath & BIT(path)) {
			
			for (i = 0; i < 2; i++) {
				/*==== [Error handling] ====*/
				if (rf_reg[path][i] == INVALID_RF_DATA) {		
					RF_DBG(rf, DBG_RF_RFK, "[RFK] Invalid RF_0x18 for Path-%d\n", path);
					return false;
				}
				rf_reg[path][i] &= ~(BIT(11) | BIT(10));
				/*==== [Switch bandwidth] ====*/
				switch (bw) {
					case CHANNEL_WIDTH_5:
					case CHANNEL_WIDTH_10:
					case CHANNEL_WIDTH_20:
						rf_reg[path][i] |= (BIT(11) | BIT(10));
					//	halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x3);
					//	halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xf);
						break;
					case CHANNEL_WIDTH_40:
						rf_reg[path][i] |= BIT(11);			
					//	halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x3);
					//	halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xf);
						break;
					case CHANNEL_WIDTH_80:
						rf_reg[path][i] |= BIT(10);
					//	halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x2);
					//	halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xd);
						break;	
					case CHANNEL_WIDTH_160:
					//	halrf_wreg(rf, 0xc0d4 |( path << 8), BIT(27) | BIT(26), 0x1);
					//	halrf_wreg(rf, 0xc0d8 |( path << 8), 0x00001e0, 0xb);
						break;
					default:
						RF_DBG(rf, DBG_RF_RFK, "[RFK] Fail to set CH\n");
						break;
				}

				/*==== [Write RF register] ====*/
				rf_reg[path][i] = (u32)(rf_reg[path][i] & 0xf0fff) | BIT(12);
				halrf_wrf(rf, path, reg_reg_addr[i], MASKRF, rf_reg[path][i]);
				RF_DBG(rf, DBG_RF_RFK, "[RFK] set %x at path%d, %x =0x%x\n",bw, path, reg_reg_addr[i], halrf_rrf(rf, path, reg_reg_addr[i], MASKRF));
			}
			//==========
			if(!rf->hal_com->dbcc_en) {
				if((path == RF_PATH_B) && (rf->hal_com->cv == CAV)){
					halrf_wrf(rf, RF_PATH_B, 0x05, 0x00001, 0x0);
					//tmp = halrf_rrf(rf, RF_PATH_A, 0x18, 0xfffff);
					tmp = rf_reg[RF_PATH_A][0];
					halrf_wrf(rf, RF_PATH_B, 0x19, 0x00030, 0x3);
					halrf_wrf(rf, RF_PATH_B, 0x18, 0xfffff, tmp);
					halrf_delay_us(rf, 100);
					halrf_wrf(rf, RF_PATH_B, 0x5, 0x00001, 0x1);
				}
			}
			//==========
		}
		RF_DBG(rf, DBG_RF_RFK, "[RFK] BW: %d\n", bw);
		RF_DBG(rf, DBG_RF_RFK, "[RFK] 0x18 = 0x%x\n", halrf_rrf(rf, path, 0x18, MASKRF));
	}


	return true;
}

#endif


bool halrf_ctrl_bw_8852c(struct rf_info *rf, enum phl_phy_idx phy, enum channel_width bw)
{
#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	bool is_dav;
	u8 path;
	u32 tmp = 0;
#endif

	u8 kpath;
	
	//RF_DBG(rf, DBG_RF_RFK, "[RFK]===> %s\n", __func__);

	/*==== Error handling ====*/
	if (bw >= CHANNEL_WIDTH_MAX ) {
		RF_DBG(rf, DBG_RF_RFK,"[RFK]Fail to switch bw(bw:%d)\n", bw);
		return false;
	}

	kpath = halrf_kpath_8852c(rf, phy);

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_bw_setting_io_ofld_8852c(rf, phy, bw);	
#else
	for (path = 0; path < 2; path++) {
		if (kpath & BIT(path)) {
			is_dav = true;
			halrf_bw_setting_8852c(rf, path, bw, is_dav);
			is_dav = false;
			halrf_bw_setting_8852c(rf, path, bw, is_dav);
			if(!rf->hal_com->dbcc_en) {
				if((path == RF_PATH_B) && (rf->hal_com->cv == CAV)){
					halrf_wrf(rf, RF_PATH_B, 0x05, 0x00001, 0x0);
					tmp = halrf_rrf(rf, RF_PATH_A, 0x18, 0xfffff);
					halrf_wrf(rf, RF_PATH_B, 0x19, 0x00030, 0x3);
					halrf_wrf(rf, RF_PATH_B, 0x18, 0xfffff, tmp);
					halrf_delay_us(rf, 100);
					halrf_wrf(rf, RF_PATH_B, 0x5, 0x00001, 0x1);
				}
			}
			RF_DBG(rf, DBG_RF_RFK, "[RFK] BW: %d\n", bw);
			RF_DBG(rf, DBG_RF_RFK, "[RFK] 0x18 = 0x%x\n", halrf_rrf(rf, path, 0x18, MASKRF));
		}
	}
#endif


	RF_DBG(rf, DBG_RF_RFK, "[RFK] Toggle IQG\n");

	//halrf_write_fwofld_start(rf);		/*FW Offload Start*/
	
	halrf_wrf(rf, RF_PATH_A, 0xdd, 0x00008, 0x1);
	halrf_delay_us(rf, 20);
	halrf_wrf(rf, RF_PATH_A, 0xa0, 0x00040, 0x0);
	halrf_delay_us(rf, 2);
	halrf_wrf(rf, RF_PATH_A, 0xa0, 0x00040, 0x1);
	halrf_delay_us(rf, 2);
	halrf_wrf(rf, RF_PATH_A, 0xdd, 0x00008, 0x0);
	halrf_delay_us(rf, 20);

	halrf_write_fwofld_end(rf);		/*FW Offload End*/

	return true;
}

bool halrf_ch_setting_8852c(struct rf_info *rf,   enum rf_path path, u8 central_ch, enum band_type band, bool is_dav)
{
	u32 rf_reg18 = 0;	
	u32 reg_reg18_addr = 0x0;

	RF_DBG(rf, DBG_RF_RFK, "[RFK]===> %s\n", __func__);
	
	if(is_dav)
		reg_reg18_addr = 0x18;
	else
		reg_reg18_addr = 0x10018;

	rf_reg18 = halrf_rrf(rf, path, reg_reg18_addr, MASKRF);
	/*==== [Error handling] ====*/
	if (rf_reg18 == INVALID_RF_DATA) {
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Invalid RF_0x18 for Path-%d\n", path);
		return false;
	}
	//is_2g_ch = (band == BAND_ON_24G) ? true : false;
	/*==== [Set RF Reg 0x18] ====*/
	rf_reg18 &= ~0x3e3ff; /*[17:16] =0,[15:12] =0001, ][9:8]=0,[7:0]=0*/

	rf_reg18 |= central_ch; /* Channel*/
	
	switch(band) {
	case BAND_ON_24G :
		/*==== [2G Setting] ====*/
		RF_DBG(rf, DBG_RF_RFK, "[RFK] 2G Setting\n");
		break;
	case BAND_ON_5G :		
		/*==== [5G Setting] ====*/
		RF_DBG(rf, DBG_RF_RFK, "[RFK] 5G Setting\n");
		rf_reg18 |= (BIT(16) | BIT(8)) ;
		break;	
	case BAND_ON_6G :
		/*==== [6G Setting] ====*/
		RF_DBG(rf, DBG_RF_RFK, "[RFK] 6G Setting\n");
		rf_reg18 |= (BIT(17) | BIT(16));
		break;
	default :
		break;
	}
	
	rf_reg18 = (u32)(rf_reg18 & 0xf0fff) | BIT(12);
	halrf_wrf(rf, path, reg_reg18_addr, MASKRF, rf_reg18);
	halrf_delay_us(rf, 100);
	//halrf_wrf(rf, path, 0xcf, BIT(0), 0);
	//halrf_wrf(rf, path, 0xcf, BIT(0), 1);
	RF_DBG(rf, DBG_RF_RFK, "[RFK] CH: %d for Path-%d, reg0x%x = 0x%x\n", central_ch, path, reg_reg18_addr, halrf_rrf(rf, path, reg_reg18_addr, MASKRF));
	return true;
}


#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
bool halrf_ch_setting_io_ofld_8852c(struct rf_info *rf, u8 kpath, u8 central_ch, enum band_type band)
{
//	u32 rf_reg18 = 0;	
//	u32 reg_reg18_addr = 0x0;
	u8 i = 0;
	u8 path = 0;

	u32 rf_reg[2][2] = {{0}}; //path, regvalue
	u32 reg_reg_addr[2] = {0x18, 0x10018};

	RF_DBG(rf, DBG_RF_RFK, "[RFK]===> %s\n", __func__);
	
//	if(is_dav)
//		reg_reg18_addr = 0x18;
//	else
//		reg_reg18_addr = 0x10018;

	rf_reg[RF_PATH_A][0] = halrf_rrf(rf, RF_PATH_A, reg_reg_addr[0], MASKRF);
	rf_reg[RF_PATH_A][1] = halrf_rrf(rf, RF_PATH_A, reg_reg_addr[1], MASKRF);
	rf_reg[RF_PATH_B][0] = halrf_rrf(rf, RF_PATH_B, reg_reg_addr[0], MASKRF);
	rf_reg[RF_PATH_B][1] = halrf_rrf(rf, RF_PATH_B, reg_reg_addr[1], MASKRF);

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	for (path = 0; path < 2; path++) {		
		if (kpath & BIT(path)) {
			for (i = 0; i < 2; i++) {
				/*==== [Error handling] ====*/
				if (rf_reg[path][i] == INVALID_RF_DATA) {
					RF_DBG(rf, DBG_RF_RFK, "[RFK] Invalid RF_0x18 for Path-%d\n", path);
					return false;
				}
				//is_2g_ch = (band == BAND_ON_24G) ? true : false;
				/*==== [Set RF Reg 0x18] ====*/
				rf_reg[path][i] &= ~0x3e3ff; /*[17:16] =0,[15:12] =0001, ][9:8]=0,[7:0]=0*/

				rf_reg[path][i] |= central_ch; /* Channel*/
					
				switch(band) {
				case BAND_ON_24G :
					/*==== [2G Setting] ====*/
					RF_DBG(rf, DBG_RF_RFK, "[RFK] 2G Setting\n");
					break;
				case BAND_ON_5G :		
					/*==== [5G Setting] ====*/
					RF_DBG(rf, DBG_RF_RFK, "[RFK] 5G Setting\n");
					rf_reg[path][i] |= (BIT(16) | BIT(8)) ;
					break;	
				case BAND_ON_6G :
					/*==== [6G Setting] ====*/
					RF_DBG(rf, DBG_RF_RFK, "[RFK] 6G Setting\n");
					rf_reg[path][i] |= (BIT(17) | BIT(16));
					break;
				default :
					break;
				}
					
				rf_reg[path][i] = (u32)(rf_reg[path][i] & 0xf0fff) | BIT(12);
				halrf_wrf(rf, path, reg_reg_addr[i], MASKRF, rf_reg[path][i]);
				halrf_delay_us(rf, 100);
				//halrf_wrf(rf, path, 0xcf, BIT(0), 0);
				//halrf_wrf(rf, path, 0xcf, BIT(0), 1);
				RF_DBG(rf, DBG_RF_RFK, "[RFK] CH: %d for Path-%d, reg0x%x = 0x%x\n", central_ch, path, reg_reg_addr[i], halrf_rrf(rf, path, reg_reg_addr[i], MASKRF));
			}
		}
	}				

	halrf_write_fwofld_end(rf);		/*FW Offload End*/
	
	return true;
}
#endif


bool halrf_ctrl_ch_8852c(struct rf_info *rf,  enum phl_phy_idx phy, u8 central_ch, enum band_type band)
{
	u8 kpath;

#ifndef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	u8 path;
	bool is_dav;
#endif

	//RF_DBG(rf, DBG_RF_RFK, "[RFK]===> %s\n", __func__);

	/*==== Error handling ====*/
	
	if (band != BAND_ON_6G) {
		if ((central_ch > 14 && central_ch < 36) ||
			    (central_ch > 64 && central_ch < 100) ||
			    (central_ch > 144 && central_ch < 149) ||
			central_ch > 177 ) {
			RF_DBG(rf, DBG_RF_RFK, "[RFK] Invalid 2G/5G CH:%d \n", central_ch);
			return false;
		}
	} else {
		if ((central_ch > 253) ||(central_ch == 2)){
			RF_DBG(rf, DBG_RF_RFK, "[RFK] Invalid 6G CH:%d \n", central_ch);
			return false;
		}
	}
	
	kpath = halrf_kpath_8852c(rf, phy);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_ch_setting_io_ofld_8852c(rf, kpath, central_ch, band);
#else
	for (path = 0; path < 2; path++) {
		if (kpath & BIT(path)) {
			is_dav = true;
			halrf_ch_setting_8852c(rf, path, central_ch, band, is_dav);
			is_dav = false;
			halrf_ch_setting_8852c(rf, path, central_ch, band, is_dav);	
		}
	}
#endif


	RF_DBG(rf, DBG_RF_RFK, "[RFK] CH: %d\n", central_ch);

	return true;
}

void halrf_set_lo_8852c(struct rf_info *rf, bool is_on, enum rf_path path)
{
	u32 rf18;
	bool not2g = true;

	rf18 = halrf_rrf(rf, RF_PATH_A, 0x18, MASKRF);
	
	if (((rf18 & (BIT(16) | BIT(8))) == 0) && ((rf18 & (BIT(17) | BIT(16))) == 0))
		not2g = false;

	if (is_on) {
		if (not2g) {
			halrf_rf_direct_cntrl_8852c(rf, path, false);
			halrf_wrf(rf, path, 0x0, 0x30000, 0x2);
			halrf_wrf(rf, path, 0x11, MASKRF, 0x30);
			halrf_wrf(rf, path, 0x62, BIT(4), 0x1);
		} else {
			halrf_rf_direct_cntrl_8852c(rf, path, false);
			halrf_wrf(rf, path, 0x0, 0x30000, 0x2);
			halrf_wrf(rf, path, 0x11, MASKRF, 0x60);
			halrf_wrf(rf, path, 0x58, BIT(1), 0x1);
		}
	} else {
		halrf_wrf(rf, path, 0x62, BIT(4), 0x0);
		halrf_wrf(rf, path, 0x58, BIT(1), 0x0);
		halrf_rf_direct_cntrl_8852c(rf, path, true);
	}
}

void halrf_rf_direct_cntrl_8852c(struct rf_info *rf, enum rf_path path, bool is_bybb)
{
	if (is_bybb)
		halrf_wrf(rf, path, 0x5, BIT(0), 0x1);
	else
		halrf_wrf(rf, path, 0x5, BIT(0), 0x0);
}

void halrf_drf_direct_cntrl_8852c(struct rf_info *rf, enum rf_path path, bool is_bybb)
{
	if (is_bybb)
		halrf_wrf(rf, path, 0x10005, BIT(0), 0x1);
	else
		halrf_wrf(rf, path, 0x10005, BIT(0), 0x0);
}


void halrf_lo_test_8852c(struct rf_info *rf, bool is_on, enum rf_path path)
{
	switch (path) {
		case RF_PATH_A:
			halrf_set_lo_8852c(rf, is_on, RF_PATH_A);
			halrf_set_lo_8852c(rf, false, RF_PATH_B);
			break;
		case RF_PATH_B:
			halrf_set_lo_8852c(rf, false, RF_PATH_A);
			halrf_set_lo_8852c(rf, is_on, RF_PATH_B);
			break;
		case RF_PATH_AB:
			halrf_set_lo_8852c(rf, is_on, RF_PATH_A);
			halrf_set_lo_8852c(rf, is_on, RF_PATH_B);
			break;
		default:
			break;
	}
}

u8 halrf_kpath_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx) {

	RF_DBG(rf, DBG_RF_RFK, "[RFK]dbcc_en: %x,  PHY%d\n", rf->hal_com->dbcc_en, phy_idx);

	if (!rf->hal_com->dbcc_en) {
		return RF_AB;
	} else {
		if (phy_idx == HW_PHY_0)
			return RF_A;
		else
			return RF_B;
	}
}

void _rx_dck_info_8852c(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, bool is_afe)
{
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	rx_dck->is_afe = is_afe;
	rx_dck->loc[path].cur_band = rf->hal_com->band[phy].cur_chandef.band;
	rx_dck->loc[path].cur_bw = rf->hal_com->band[phy].cur_chandef.bw;
	rx_dck->loc[path].cur_ch = rf->hal_com->band[phy].cur_chandef.center_ch;

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] ==== S%d RX DCK (%s / CH%d / %s / by %s)====\n", path,
		rx_dck->loc[path].cur_band == 0 ? "2G" :
		(rx_dck->loc[path].cur_band == 1 ? "5G" : "6G"),
		rx_dck->loc[path].cur_ch,
	       rx_dck->loc[path].cur_bw == 0 ? "20M" :
	       (rx_dck->loc[path].cur_bw == 1 ? "40M" : 
	       	(rx_dck->loc[path].cur_bw == 2 ? "80M" : "160M")),
	       	rx_dck->is_afe ? "AFE" : "RFC");	
}

void _rx_dck_value_rewrite_8852c(struct rf_info *rf, enum rf_path path, u8 addr, u8 val_i, u8 val_q)
{
	u32 ofst_val;;

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] rewrite val_i = 0x%x, val_q = 0x%x\n", val_i, val_q);

	ofst_val = ((val_q >> 1) << 6) | (val_i >> 1); /*[15:9] -> [15:10]*/

	halrf_wrf(rf, path, 0xec, BIT(19), 0x1);
	halrf_wrf(rf, path, 0xf0, BIT(16), 0x1);
	halrf_wrf(rf, path, 0x92, BIT(1), 0x1);
	halrf_wrf(rf, path, 0x33, MASKBYTE0, addr);
	halrf_wrf(rf, path, 0x3f, MASKRF, ofst_val);
	halrf_wrf(rf, path, 0x3f, MASKRF, ofst_val);
	halrf_wrf(rf, path, 0x92, BIT(1), 0x0);
	halrf_wrf(rf, path, 0xf0, BIT(16), 0x0);
	halrf_wrf(rf, path, 0xec, BIT(19), 0x0);

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Final val_i = 0x%x, val_q = 0x%x\n",
		(ofst_val & 0x3f) << 1, (ofst_val >> 6) << 1);
}

bool _rx_dck_rek_check(struct rf_info *rf, enum rf_path path)
{
	u8 addr_bs[2] = {0x0, 0x10};
	u8 addr[2] = {0xc, 0x1c};
	u8 i_even_bs[2], q_even_bs[2]; /*0x0, 0x10*/
	u8 i_odd_bs[2], q_odd_bs[2];   /*0x1, 0x11*/
	u8 i_even[2], q_even[2]; /*0xc, 0x1c*/
	u8 i_odd[2], q_odd[2];	 /*0xd, 0x1d*/
	u8 i, th = 10;
	bool chk_fail = false;

	halrf_write_fwofld_end(rf); 	/*FW Offload End*/

	for (i = 0; i < 2; i++) {
		halrf_wrf(rf, path, 0x00, 0x07c00, addr_bs[i]); /*[14:10]*/
		i_even_bs[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);	/*[15:9]*/
		q_even_bs[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);	/*[15:9]*/
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_even_bs/ q_even_bs = 0x%x/ 0x%x\n",
			addr_bs[i], i_even_bs[i], q_even_bs[i]);

		halrf_wrf(rf, path, 0x00, 0x07c00, addr[i]); /*[14:10]*/
		i_even[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);  /*[15:9]*/
		q_even[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);  /*[15:9]*/
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_even/ q_even = 0x%x/ 0x%x\n",
			addr[i], i_even[i], q_even[i]);

		if ((HALRF_ABS(i_even_bs[i], i_even[i]) > th) || (HALRF_ABS(q_even_bs[i], q_even[i]) > th)) {
			chk_fail = true;
			break;
		}

		halrf_wrf(rf, path, 0x00, 0x07c00, addr_bs[i] + 1); /*[14:10]*/
		i_odd_bs[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);  /*[15:9]*/
		q_odd_bs[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);  /*[15:9]*/
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_odd_bs/ q_odd_bs = 0x%x/ 0x%x\n",
			addr_bs[i] + 1, i_odd_bs[i], q_odd_bs[i]);

		halrf_wrf(rf, path, 0x00, 0x07c00, addr[i] + 1); /*[14:10]*/
		i_odd[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);  /*[15:9]*/
		q_odd[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);  /*[15:9]*/
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_odd/ q_odd = 0x%x/ 0x%x\n",
			addr[i] + 1, i_odd[i], q_odd[i]);

		if ((HALRF_ABS(i_odd_bs[i], i_odd[i]) > th) || (HALRF_ABS(q_odd_bs[i], q_odd[i]) > th)) {
			chk_fail = true;
			break;
		}
	}

	halrf_write_fwofld_start(rf);	/*FW Offload Start*/

	return chk_fail;
}

void _rx_dck_check_8852c(struct rf_info *rf, enum rf_path path, u8 addr,
				u8 val_i_bs, u8 val_q_bs, u8 val_i, u8 val_q)
{
	u8 th = 10;

	if ((HALRF_ABS(val_i_bs, val_i) < th) && (HALRF_ABS(val_q_bs, val_q) < th)) {
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] offset check PASS!!\n");
		return;
	}

	if (HALRF_ABS(val_i_bs, val_i) >= th) {
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] val_i over TH (0x%x / 0x%x)\n",
			val_i_bs, val_i);
		val_i = val_i_bs;
	}

	if (HALRF_ABS(val_q_bs, val_q) >= th) {
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] val_q over TH (0x%x / 0x%x)\n",
			val_q_bs, val_q);
		val_q = val_q_bs;
	}

	_rx_dck_value_rewrite_8852c(rf, path, addr, val_i, val_q);
}

void _rx_dck_read_offset_8852c(struct rf_info *rf, enum rf_path path)
{
	u8 addr_bs[2] = {0x0, 0x10};
	u8 addr[2] = {0xc, 0x1c};
	u8 i_even_bs[2], q_even_bs[2]; /*0x0, 0x10*/
	u8 i_odd_bs[2], q_odd_bs[2];   /*0x1, 0x11*/
	u8 i_even[2], q_even[2]; /*0xc, 0x1c*/
	u8 i_odd[2], q_odd[2];	 /*0xd, 0x1d*/

	u8 i;

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK]===> %s\n", __func__);

	for (i = 0; i < 2; i++) {
		halrf_write_fwofld_end(rf); 	/*FW Offload End*/

		halrf_wrf(rf, path, 0x00, 0x07c00, addr_bs[i]); /*[14:10]*/
		i_even_bs[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);	/*[15:9]*/
		q_even_bs[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);	/*[15:9]*/

		halrf_wrf(rf, path, 0x00, 0x07c00, addr_bs[i] + 1); /*[14:10]*/
		i_odd_bs[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);  /*[15:9]*/
		q_odd_bs[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);  /*[15:9]*/

		halrf_wrf(rf, path, 0x00, 0x07c00, addr[i]); /*[14:10]*/
		i_even[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);  /*[15:9]*/
		q_even[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);  /*[15:9]*/

		halrf_wrf(rf, path, 0x00, 0x07c00, addr[i] + 1); /*[14:10]*/
		i_odd[i] = (u8)halrf_rrf(rf, path, 0x92, 0x0FE00);  /*[15:9]*/
		q_odd[i] = (u8)halrf_rrf(rf, path, 0x93, 0x0FE00);  /*[15:9]*/

		halrf_write_fwofld_start(rf);	/*FW Offload Start*/

		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_even_bs/ q_even_bs = 0x%x/ 0x%x\n",
			addr_bs[i], i_even_bs[i], q_even_bs[i]);
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_even/ q_even = 0x%x/ 0x%x\n",
			addr[i], i_even[i], q_even[i]);
		_rx_dck_check_8852c(rf, path, addr[i], i_even_bs[i], q_even_bs[i], i_even[i], q_even[i]);

		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_odd_bs/ q_odd_bs = 0x%x/ 0x%x\n",
			addr_bs[i] + 1, i_odd_bs[i], q_odd_bs[i]);
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] Gain[0x%x] i_odd/ q_odd = 0x%x/ 0x%x\n",
			addr[i] + 1, i_odd[i], q_odd[i]);
		_rx_dck_check_8852c(rf, path, addr[i] + 1, i_odd_bs[i], q_odd_bs[i], i_odd[i], q_odd[i]);
	}
}

void halrf_rx_dck_toggle_8852c(struct rf_info *rf, enum rf_path path)
{
	u32 cnt = 0;

	halrf_wrf(rf, path, 0x92, BIT(0), 0x0);
	halrf_wrf(rf, path, 0x92, BIT(0), 0x1);

	cnt = 0 ;
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
//#if 0	
	if (!halrf_polling_rf(rf, path, 0x93, BIT(5), 0x1, 500)) {
		RF_DBG(rf, DBG_RF_FW, "[RX_DCK][IO_OFLD]  S%d polling 0x93[5]= 1 in IO queue Fail\n",
		path);
	}
	else
		RF_DBG(rf, DBG_RF_FW, "[RX_DCK][IO_OFLD] S%d polling 0x93[5]= 1 in IO queue OK\n",
		path);
#else
	while ((halrf_rrf(rf, path, 0x93, BIT(5)) == 0x0) && (cnt < 500)) {
			halrf_delay_us(rf, 2);
		cnt++;
		}

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] S%d RXDCK finish (cnt = %d)\n",
		path, cnt);
#endif
	
		halrf_wrf(rf, path, 0x92, BIT(0), 0x0);

}

void halrf_set_rx_dck_8852c(struct rf_info *rf, enum phl_phy_idx phy, enum rf_path path, bool is_afe)
{
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	u8 phy_map, res;

	phy_map = (BIT(phy) << 4) | BIT(path);

	_rx_dck_info_8852c(rf, phy, path, is_afe);

	halrf_wrf(rf, path, 0x93, 0x0000f, 0x0); /*0: from RFC; 1: from AFE*/
#if 0
	halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_RXDCK, RFK_ONESHOT_START);
#endif
	halrf_rx_dck_toggle_8852c(rf, path);
#if 0
		halrf_btc_rfk_ntfy(rf, phy_map, RF_BTC_RXDCK, RFK_ONESHOT_STOP);
#endif
#if 0
	RF_DBG(rf, DBG_RF_RFK, "[RX_DCK] 0x92 = 0x%x, 0x93 = 0x%x\n",
	       halrf_rrf(rf, path, 0x92, MASKRF),
	       halrf_rrf(rf, path, 0x93, MASKRF));
#endif
	if (rx_dck->is_auto_res) {
		res = (u8)halrf_rrf(rf, path, 0x92, BIT(7) | BIT(6) | BIT(5));
		RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] S%d RXDCK resolution = 0x%x\n", path, res);
#if 1
		if (res > 1) {
			halrf_wrf(rf, path, 0x8f, BIT(11) | BIT(10) | BIT(9), res);
			halrf_rx_dck_toggle_8852c(rf, path);
			halrf_wrf(rf, path, 0x8f, BIT(11) | BIT(10) | BIT(9), 0x1);
		}
#endif
	}
}

void halrf_rx_dck_8852c(struct rf_info *rf, enum phl_phy_idx phy, bool is_afe) 
{
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	u8 path, kpath;
	u32 rf_0 = 0, rf_5;

	kpath = halrf_kpath_8852c(rf, phy);

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] ****** RXDCK Start (Ver: 0x%x, Cv: %d) ******\n",
		RXDCK_VER_8852C, rf->hal_com->cv);

	for (path = 0; path < 2; path++) {
		if (kpath & BIT(path)) {
			rf_5 = halrf_rrf(rf, path, 0x5, MASKRF);
			if ((rf_5 & BIT(0)) == 0)
				rf_0 = halrf_rrf(rf, path, 0x0, MASKRF);

			halrf_write_fwofld_start(rf);	/*FW Offload Start*/

			if (rf->is_tssi_mode[path]) 
				halrf_wreg(rf, 0x5818 + (path << 13), BIT(30), 0x1); /*TSSI pause*/

			halrf_wrf(rf, path, 0x5, BIT(0), 0x0);
			halrf_wrf(rf, path, 0x00, MASKRFMODE, RF_RX);
			halrf_wrf(rf, path, 0x00, BIT(1), rf->hal_com->dbcc_en);
			halrf_set_rx_dck_8852c(rf, phy, path, is_afe);

			_rx_dck_read_offset_8852c(rf, path);

			halrf_wrf(rf, path, 0x5, MASKRF, rf_5);
			if ((rf_5 & BIT(0)) == 0)
				halrf_wrf(rf, path, 0x0, MASKRF, rf_0);

			if (rf->is_tssi_mode[path])
				halrf_wreg(rf, 0x5818 + (path << 13), BIT(30), 0x0); /*TSSI resume*/

			rx_dck->ther_rxdck[path] = halrf_get_thermal_8852c(rf, path);

			halrf_write_fwofld_end(rf); 	/*FW Offload End*/		
		}
	}
}

void halrf_thermal_rx_dck_8852c(struct rf_info *rf, enum phl_phy_idx phy, bool is_afe) 
{
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	u8 path, kpath;
	u32 rf_0 = 0, rf_5;
	bool is_fail;

	kpath = halrf_kpath_8852c(rf, phy);

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] ****** RXDCK Start (Ver: 0x%x, Cv: %d) ******\n",
		RXDCK_VER_8852C, rf->hal_com->cv);

	for (path = 0; path < 2; path++) {
		if (kpath & BIT(path)) {
			rx_dck->rek_cnt[path] = 0;
			rf_5 = halrf_rrf(rf, path, 0x5, MASKRF);
			if ((rf_5 & BIT(0)) == 0)
				rf_0 = halrf_rrf(rf, path, 0x0, MASKRF);

			halrf_write_fwofld_start(rf);	/*FW Offload Start*/

			if (rf->is_tssi_mode[path]) 
				halrf_wreg(rf, 0x5818 + (path << 13), BIT(30), 0x1); /*TSSI pause*/

			halrf_wrf(rf, path, 0x5, BIT(0), 0x0);
			halrf_wrf(rf, path, 0x00, MASKRFMODE, RF_RX);
			halrf_wrf(rf, path, 0x00, BIT(1), rf->hal_com->dbcc_en);
			halrf_set_rx_dck_8852c(rf, phy, path, is_afe);

			is_fail = _rx_dck_rek_check(rf, path);

			while (is_fail == true && rx_dck->rek_cnt[path] < 3) {
				halrf_set_rx_dck_8852c(rf, phy, path, is_afe);
				rx_dck->rek_cnt[path]++;
				is_fail = _rx_dck_rek_check(rf, path);
				RF_DBG(rf, DBG_RF_RXDCK, "rx_dck->rek_cnt[%d]=%d", path, rx_dck->rek_cnt[path]);
			}

			if (is_fail)
				_rx_dck_read_offset_8852c(rf, path);

			halrf_wrf(rf, path, 0x5, MASKRF, rf_5);
			if ((rf_5 & BIT(0)) == 0)
				halrf_wrf(rf, path, 0x0, MASKRF, rf_0);

			if (rf->is_tssi_mode[path])
				halrf_wreg(rf, 0x5818 + (path << 13), BIT(30), 0x0); /*TSSI resume*/

			rx_dck->ther_rxdck[path] = halrf_get_thermal_8852c(rf, path);

			halrf_write_fwofld_end(rf); 	/*FW Offload End*/
		}
	}
}

void halrf_rx_dck_onoff_8852c(struct rf_info *rf, bool is_enable)
{
	u8 path;

	for (path = 0; path < 2; path++) {
		halrf_wrf(rf, path, 0x93, BIT(0), !is_enable);
		if (!is_enable) {
			halrf_wrf(rf, path, 0x92, 0xFFE00, 0x440); /*[19:9]*/
			halrf_wrf(rf, path, 0x93, 0xFFE00, 0x440); /*[19:9]*/
		}
	}
}

void halrf_rxdck_track_8852c(
	struct rf_info *rf)
{
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] S0 delta_ther = %d (0x%x / 0x%x@k)\n",
		rf->cur_ther_s0 - rx_dck->ther_rxdck[RF_PATH_A],
		rf->cur_ther_s0, rx_dck->ther_rxdck[RF_PATH_A]);
	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] S1 delta_ther = %d (0x%x / 0x%x@k)\n",
		rf->cur_ther_s1 - rx_dck->ther_rxdck[RF_PATH_B],
		rf->cur_ther_s1, rx_dck->ther_rxdck[RF_PATH_B]);

	if (!rf->hal_com->dbcc_en) {
		if (HALRF_ABS(rf->cur_ther_s0, rx_dck->ther_rxdck[RF_PATH_A]) >= 12 ||
		    HALRF_ABS(rf->cur_ther_s1, rx_dck->ther_rxdck[RF_PATH_B]) >= 12)
			halrf_thermal_rx_dck_trigger(rf, HW_PHY_0, false); /*ReK S0+S1*/
	} else {
		if (HALRF_ABS(rf->cur_ther_s0, rx_dck->ther_rxdck[RF_PATH_A]) >= 12)
			halrf_thermal_rx_dck_trigger(rf, HW_PHY_0, false); /*ReK S0 only*/
		if (HALRF_ABS(rf->cur_ther_s1, rx_dck->ther_rxdck[RF_PATH_B]) >= 12)
			halrf_thermal_rx_dck_trigger(rf, HW_PHY_1, false); /*ReK S1 only*/
	}
}

void halrf_rck_8852c(struct rf_info *rf, enum rf_path path)
{
	u8 cnt = 0;
	u32 rf_reg5;
	u32 rck_val = 0;

	RF_DBG(rf, DBG_RF_RFK, "[RCK] ====== S%d RCK ======\n", path);

	rf_reg5 = halrf_rrf(rf, path, 0x5, MASKRF);

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	halrf_wrf(rf, path, 0x5, BIT(0), 0x0);
	halrf_wrf(rf, path, 0x0, MASKRFMODE, RF_RX);
	
	RF_DBG(rf, DBG_RF_RFK, "[RCK] RF0x00 = 0x%05x\n", halrf_rrf(rf, path, 0x00, MASKRF));

	/*RCK trigger*/
	halrf_wrf(rf, path, 0x1b, MASKRF, 0x00240);

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	if (!halrf_polling_rf(rf, path, 0x1c, BIT(3), 0x1, cnt)) {
		RF_DBG(rf, DBG_RF_RFK, "[RCK] RF 0x1c[3] timeout !!!\n");
	}
	cnt = 0;
#else
	cnt = 0;
	while ((halrf_rrf(rf, path, 0x1c, BIT(3)) == 0x00) && (cnt < 10)) {
		halrf_delay_us(rf, 2);
		cnt++;
	}
#endif

	halrf_write_fwofld_end(rf);		/*FW Offload End*/

	rck_val = halrf_rrf(rf, path, 0x1b, 0x07C00); /*[14:10]*/

	RF_DBG(rf, DBG_RF_RFK, "[RCK] rck_val = 0x%x, count = %d\n", rck_val, cnt);

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	halrf_wrf(rf, path, 0x1b, MASKRF, rck_val);

	halrf_wrf(rf, path, 0x5, MASKRF, rf_reg5);

	halrf_write_fwofld_end(rf);		/*FW Offload End*/

	RF_DBG(rf, DBG_RF_RFK, "[RCK] RF 0x1b = 0x%x\n",
	       halrf_rrf(rf, path, 0x1b, MASKRF));
}

void iqk_backup_8852c(struct rf_info *rf, enum rf_path path) 
{
	return;
}

void halrf_bf_config_rf_8852c(struct rf_info *rf)
{
                halrf_wrf(rf, RF_PATH_A, 0xef, BIT(19), 0x1);
                halrf_wrf(rf, RF_PATH_A, 0x33, 0xf, 0x1);
                halrf_wrf(rf, RF_PATH_A, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_A, 0x3f, MASKRF, 0x0020c);
                halrf_wrf(rf, RF_PATH_A, 0x33, 0xf, 0x2);
                halrf_wrf(rf, RF_PATH_A, 0x3e, MASKRF, 0x007ff);
                halrf_wrf(rf, RF_PATH_A, 0x3f, MASKRF, 0x80214);
                halrf_wrf(rf, RF_PATH_A, 0x33, 0xf, 0x3);
                halrf_wrf(rf, RF_PATH_A, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_A, 0x3f, MASKRF, 0x0d79c);
                halrf_wrf(rf, RF_PATH_A, 0x33, 0xf, 0xa);
                halrf_wrf(rf, RF_PATH_A, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_A, 0x3f, MASKRF, 0x0d743);
                halrf_wrf(rf, RF_PATH_A, 0x33, 0xf, 0xb);
                halrf_wrf(rf, RF_PATH_A, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_A, 0x3f, MASKRF, 0x0d723);
                halrf_wrf(rf, RF_PATH_A, 0xef, BIT(19), 0x0);

                halrf_wrf(rf, RF_PATH_B, 0xef, BIT(19), 0x1);
                halrf_wrf(rf, RF_PATH_B, 0x33, 0xf, 0x1);
                halrf_wrf(rf, RF_PATH_B, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_B, 0x3f, MASKRF, 0x00208);
                halrf_wrf(rf, RF_PATH_B, 0x33, 0xf, 0x2);
                halrf_wrf(rf, RF_PATH_B, 0x3e, MASKRF, 0x007ff);
                halrf_wrf(rf, RF_PATH_B, 0x3f, MASKRF, 0x80214);
                halrf_wrf(rf, RF_PATH_B, 0x33, 0xf, 0x3);
                halrf_wrf(rf, RF_PATH_B, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_B, 0x3f, MASKRF, 0x0d79c);
                halrf_wrf(rf, RF_PATH_B, 0x33, 0xf, 0xa);
                halrf_wrf(rf, RF_PATH_B, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_B, 0x3f, MASKRF, 0x0d743);
                halrf_wrf(rf, RF_PATH_B, 0x33, 0xf, 0xb);
                halrf_wrf(rf, RF_PATH_B, 0x3e, MASKRF, 0x00630);
                halrf_wrf(rf, RF_PATH_B, 0x3f, MASKRF, 0x0d723);
                halrf_wrf(rf, RF_PATH_B, 0xef, BIT(19), 0x0);
}

bool halrf_rfk_reg_check_8852c(struct rf_info *rf)
{
	u32 i, reg, temp;
	bool fail = false;

	for (i = 0; i < KIP_REG; i++) {
		reg = 0x8000 + i*4;
		if (((reg >= 0x8000 && reg < 0x8300) || 
			(reg >= 0x8500 && reg < 0x9b60) || 
			(reg >= 0x9d00 && reg < 0xa360) ||
			(reg >= 0xa400 && reg < 0xa444) ||
			(reg >= 0xa500 && reg < 0xa880) ||
			(reg >= 0xa900 && reg < 0xac80) ||
			(reg >= 0xb100)) &&
			(reg != 0x8014) &&
			(reg != 0x80f8) && (reg != 0x80fc) &&
			(reg != 0x81f8) && (reg != 0x81fc) &&
			(reg != 0x82f8) && (reg != 0x82fc) &&
			(reg != 0xb1f8) && (reg != 0xb1fc) &&
			(reg != 0x81b4) && (reg != 0x82b4)) {
			temp = halrf_rreg(rf, reg, MASKDWORD);
			if (rf->rfk_reg[i] != temp) {
				RF_DBG(rf, DBG_RF_RFK,
					"[RFK]reg 0x%x b 0x%x/a 0x%x\n",
					reg,
					rf->rfk_reg[i],
					temp);
#if 0
				for (t =0; t < 10; t++) {
					temp = halrf_rreg(rf, reg, MASKDWORD);
					RF_DBG(rf, DBG_RF_RFK, "[RFK]reg 0x%x mismatch: before 0x%x/after 0x%x\n", reg, rf->rfk_reg[i], temp);
					halrf_delay_ms(rf, 1);
				}
#endif
				fail = true;
				rf->rfk_check_fail_count++;
			}
		}
	}
	return fail;
}


void halrf_rfk_reg_reload_8852c(struct rf_info *rf)
{
	u32 i, reg;

#ifdef CF_PHL_BB_CTRL_RX_CCA
	halrf_bb_ctrl_rx_cca(rf, false, HW_PHY_0);
#else
	RF_WARNING("[RF]%s !! no CF_PHL_BB_CTRL_RX_CCA\n", __FUNCTION__);
#endif
	halrf_wreg(rf, 0x0a70, 0x3e0, 0x1f);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000005);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000110);
	halrf_wreg(rf, 0x81d8, MASKDWORD, 0x00010001);
	halrf_wreg(rf, 0x82d8, MASKDWORD, 0x00010001);
	RF_DBG(rf, DBG_RF_RFK, "[RFK]KIP_REG = %d\n", KIP_REG);
	for (i = 0; i < KIP_REG; i++) {
		reg = 0x8000 + i*4;
		halrf_wreg(rf, reg, MASKDWORD, rf->rfk_reg[i]);
	}


	
	halrf_wreg(rf, 0x81d8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00010003);
	halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00000002);
	halrf_wreg(rf, 0x82d8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00010003);
	halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00000002);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000004);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
	halrf_wreg(rf, 0x0a70, 0x3e0, 0x0);
#ifdef CF_PHL_BB_CTRL_RX_CCA
	halrf_bb_ctrl_rx_cca(rf, true, HW_PHY_0);
#else
	RF_WARNING("[RF]%s !! no CF_PHL_BB_CTRL_RX_CCA\n", __FUNCTION__);
#endif
}



void halrf_rfk_reg_backup_8852c(struct rf_info *rf)
{
	u32 i, reg;

#ifdef CF_PHL_BB_CTRL_RX_CCA
	halrf_bb_ctrl_rx_cca(rf, false, HW_PHY_0);
#else
	RF_WARNING("[RF]%s !! no CF_PHL_BB_CTRL_RX_CCA\n", __FUNCTION__);
#endif
	halrf_wreg(rf, 0x0a70, 0x3e0, 0x1f);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000005);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000110);
	halrf_wreg(rf, 0x81d8, MASKDWORD, 0x00010001);
	halrf_wreg(rf, 0x82d8, MASKDWORD, 0x00010001);
	RF_DBG(rf, DBG_RF_RFK, "[RFK]KIP_REG = %d\n", KIP_REG);
	for (i = 0; i < KIP_REG; i++) {
		reg = 0x8000 + i*4;
		rf->rfk_reg[i] = halrf_rreg(rf, reg, MASKDWORD);
	}

	rf->rfk_reg[1791] = rf->rfk_reg[1344];
	rf->rfk_reg[2303] = rf->rfk_reg[1856];

	rf->rfk_reg[2623] = rf->rfk_reg[2368];
	rf->rfk_reg[2879] = rf->rfk_reg[2624];

	for (i = 0; i < KIP_REG; i++) {
		reg = 0x8000 + i*4;
		RF_DBG(rf, DBG_RF_RFK,
			"[RFK]bk reg 0x%x = 0x%x\n",
			reg,
			rf->rfk_reg[i]);
	}

	halrf_wreg(rf, 0x81d8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00010003);
	halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00000002);
	halrf_wreg(rf, 0x82d8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00010003);
	halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00000002);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000004);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
	halrf_wreg(rf, 0x0a70, 0x3e0, 0x0);
#ifdef CF_PHL_BB_CTRL_RX_CCA
	halrf_bb_ctrl_rx_cca(rf, true, HW_PHY_0);
#else
	RF_WARNING("[RF]%s !! no CF_PHL_BB_CTRL_RX_CCA\n", __FUNCTION__);
#endif
}

bool halrf_rfk_reg_check_fail_8852c(struct rf_info *rf)
{
	bool fail = false;

#ifdef CF_PHL_BB_CTRL_RX_CCA
	halrf_bb_ctrl_rx_cca(rf, false, HW_PHY_0);
#else
	RF_WARNING("[RF]%s !! no CF_PHL_BB_CTRL_RX_CCA\n", __FUNCTION__);
#endif
	rf->rfk_check_fail_count = 0;
	halrf_wreg(rf, 0x0a70, 0x3e0, 0x1f);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000005);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000110);
	halrf_wreg(rf, 0x81d8, MASKDWORD, 0x00010001);
	halrf_wreg(rf, 0x82d8, MASKDWORD, 0x00010001);
	fail = halrf_rfk_reg_check_8852c(rf);
	halrf_wreg(rf, 0x81d8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00010003);	
	halrf_wreg(rf, 0x81dc, MASKDWORD, 0x00000002);
	halrf_wreg(rf, 0x82d8, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00010003);
	halrf_wreg(rf, 0x82dc, MASKDWORD, 0x00000002);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000004);
	halrf_wreg(rf, 0x8080, MASKDWORD, 0x00000000);
	halrf_wreg(rf, 0x8088, MASKDWORD, 0x80000000);
	halrf_wreg(rf, 0x0a70, 0x3e0, 0x0);
	RF_DBG(rf, DBG_RF_RFK,
		"[RFK]fail count = %d\n",
		rf->rfk_check_fail_count);
#ifdef CF_PHL_BB_CTRL_RX_CCA
	halrf_bb_ctrl_rx_cca(rf, true, HW_PHY_0);
#else
	RF_WARNING("[RF]%s !! no CF_PHL_BB_CTRL_RX_CCA\n", __FUNCTION__);
#endif
	return fail;
}

static u32 r_d[] = {0x18, 0xb2, 0xc5};
static u32 r_m[] = {0xfffff, 0xfffff, 0x8000};

void halrf_rfc_reg_backup_8852c(struct rf_info *rf)
{
	u8 i;

	for (i = 0; i < 3; i++) {
		rf->rfc_reg[0][i] = halrf_rrf(rf, RF_PATH_A, r_d[i], r_m[i]);
		rf->rfc_reg[1][i] = halrf_rrf(rf, RF_PATH_B, r_d[i], r_m[i]);  
	}
}

bool halrf_rfc_reg_check_fail_8852c(struct rf_info *rf)
{
	u8 i;
	u32 temp;
	bool fail = false;

	for (i = 0; i < 3; i++) {
		temp = halrf_rrf(rf, RF_PATH_A, r_d[i], r_m[i]);
		if (rf->rfc_reg[0][i] !=temp) {
			RF_DBG(rf, DBG_RF_RFK,
				"[RFK]S0 0x%x mask=0x%x, old=0x%x, new=0x%x\n",
				r_d[i],
				r_m[i],
				rf->rfc_reg[0][i],
				temp);
			fail = true;
		}
	}
	for (i = 0; i < 3; i++) {
		temp = halrf_rrf(rf, RF_PATH_B, r_d[i], r_m[i]);
		if (rf->rfc_reg[1][i] !=temp) {
			RF_DBG(rf, DBG_RF_RFK,
				"[RFK]S1 0x%x mask=0x%x, old=0x%x, new=0x%x\n",
				r_d[i],
				r_m[i],
				rf->rfc_reg[1][i],
				temp);
			fail = true;
		}
	}
	return fail;
}




void halrf_set_rxbb_bw_8852c(struct rf_info *rf, enum channel_width bw, enum rf_path path)
{
	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	halrf_wrf(rf, path, 0xee, BIT(2), 0x1);
	halrf_wrf(rf, path, 0x33, 0x0001F, 0xa); /*[4:0]*/

	if (bw == CHANNEL_WIDTH_20)
		halrf_wrf(rf, path, 0x3f, 0x0003F, 0x1b); /*[5:0]*/
	else if (bw == CHANNEL_WIDTH_40)
		halrf_wrf(rf, path, 0x3f, 0x0003F, 0x13); /*[5:0]*/
	else if (bw == CHANNEL_WIDTH_80)
		halrf_wrf(rf, path, 0x3f, 0x0003F, 0xb); /*[5:0]*/
	else if (bw == CHANNEL_WIDTH_160)
		halrf_wrf(rf, path, 0x3f, 0x0003F, 0x3); /*[5:0]*/
	else
		halrf_wrf(rf, path, 0x3f, 0x0003F, 0x3); /*[5:0]*/

	RF_DBG(rf, DBG_RF_RFK, "[RFK] RXBB BW set 0x3F = 0x%x\n",
		halrf_rrf(rf, path, 0x3f, 0x0003F));

	halrf_wrf(rf, path, 0xee, BIT(2), 0x0);

	halrf_write_fwofld_end(rf);		/*FW Offload End*/
}

void halrf_rxbb_bw_8852c(struct rf_info *rf, enum phl_phy_idx phy, enum channel_width bw)
{
	u8 kpath, path;

	kpath = halrf_kpath_8852c(rf, phy);
	
	for (path = 0; path < 2; path++) {
		if ((kpath & BIT(path)) && (rf->pre_rxbb_bw[path] != bw)) {
			halrf_set_rxbb_bw_8852c(rf, bw, path);
			rf->pre_rxbb_bw[path] = bw;
		} else
			RF_DBG(rf, DBG_RF_RFK,
			       "[RFK] S%d RXBB BW unchanged (pre_bw = 0x%x)\n",
			       path, rf->pre_rxbb_bw[path]);
	}
}

void halrf_disconnect_notify_8852c(struct rf_info *rf, struct rtw_chan_def *chandef) {

	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u8 ch;
	
	RF_DBG(rf, DBG_RF_RFK, "[IQK]===>%s\n", __func__);
#if 0
	/*[IQK disconnect]*/
	for (ch = 0; ch < 2; ch++) {
		for (path = 0; path < KPATH; path++) {
			if (iqk_info->iqk_mcc_ch[ch][path] == chandef->center_ch)
				iqk_info->iqk_mcc_ch[ch][path] = 0x0;
		}

	}
#endif
	/*TXGAPK*/
	for (ch = 0; ch < 2; ch++) {		
		if (txgapk_info->txgapk_mcc_ch[ch] == chandef->center_ch)
				txgapk_info->txgapk_mcc_ch[ch] = 0x0;
	}

	/*mcc info*/
	for (ch = 0; ch < 2; ch++) {		
		if ((mcc_info->ch[ch] == chandef->center_ch) && (mcc_info->band[ch] == chandef->band)) {
			mcc_info->ch[ch] = 0x0;
			mcc_info->band[ch] = 0x0;
		}
				
	}
	
}

bool halrf_check_mcc_ch_8852c(struct rf_info *rf, struct rtw_chan_def *chandef) {

	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u8 ch;
	bool check = false;

	RF_DBG(rf, DBG_RF_RFK, "[RFK]===>%s, center_ch(%d)\n", __func__, chandef->center_ch);
	/*[IQK check_mcc_ch]*/
	for (ch = 0; ch < 2; ch++) {
			if ((mcc_info->ch[ch] == chandef->center_ch) && ( mcc_info->band[ch] == chandef->band)) {
				check = true;
				return check;
			}
	}
	return check;
}

void halrf_fw_ntfy_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx) {
	u8 i = 0x0;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u32 data_to_fw[6] = {0};
	u16 len = (u16) (sizeof(data_to_fw) / sizeof(u32))*4;

	data_to_fw[0] = (u32) mcc_info->ch[0];
	data_to_fw[1] = (u32) mcc_info->ch[1];
	data_to_fw[2] = (u32) mcc_info->band[0];
	data_to_fw[3] = (u32) mcc_info->band[1];
	data_to_fw[4] = rf->hal_com->band[phy_idx].cur_chandef.center_ch;
	data_to_fw[5] = rf->hal_com->band[phy_idx].cur_chandef.band;

	RF_DBG(rf, DBG_RF_RFK, "[IQK] len = 0x%x\n", len);
	//for (i =0; i < 5; i++)
	for (i =0; i < 6; i++)
		RF_DBG(rf, DBG_RF_RFK, "[IQK] data_to_fw[%x] = 0x%x\n", i, data_to_fw[i]);

	halrf_fill_h2c_cmd(rf, len, FWCMD_H2C_GET_MCCCH, 0xa, H2CB_TYPE_DATA, (u32 *) data_to_fw);	

	return;
}

void halrf_before_one_shot_enable_8852c(struct rf_info *rf) {	

#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
#else
	if (halrf_rreg(rf, 0xbff8, MASKBYTE0) != 0x0) {
		
		halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
		halrf_delay_us(rf, 1);
		RF_DBG(rf, DBG_RF_RFK, "======> before set one-shot bit, 0x%x= 0x%x\n", 0x8010, halrf_rreg(rf, 0x8010, MASKDWORD));
	}
#endif
	/* set 0x80d4[21:16]=0x03 (before oneshot NCTL) to get report later */
	//halrf_wreg(rf, 0x80d4, 0x003F0000, 0x03);
		
	
}

bool halrf_one_shot_nctl_done_check_8852c(struct rf_info *rf, enum rf_path path) {	
	/* for check status */
	u32 r_bff8 = 0;
	u32 r_80fc = 0;
	bool is_ready = false;
	u16 count = 1;

	rf->nctl_ck_times[0] = 0;
	rf->nctl_ck_times[1] = 0;


#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	r_bff8 = 0x0;
	r_80fc = 0;
	count = 1;

	if (!halrf_polling_bb(rf, 0xbff8, MASKBYTE0, 0x55, 2000)) {
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD] %s 0xbff8 == 0x55 Timeout !!!\n", __func__);
		is_ready = false;
	} else { //Polling ok
		halrf_delay_us(rf, 10);
		is_ready = true;
		RF_DBG(rf, DBG_RF_FW, "[IO OFLD] %s polling 0xbff8 == 0x55 OK!!!\n", __func__);
	}

	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);
#else	
	/* for 0xbff8 check NCTL DONE */
	while (count < 2000) {	
		/* r_bff8 = 0; */
		r_bff8 = halrf_rreg(rf, 0xbff8, MASKBYTE0);
				
		if (r_bff8 == 0x55) {
			is_ready = true;
			break;
		}	
	
		halrf_delay_us(rf, 10);
		count++;
	}
	halrf_delay_us(rf, 1);
	rf->nctl_ck_times[0] = count;
	
	RF_DBG(rf, DBG_RF_RFK, "======> path = %d, check 0xBFF8[7:0] = 0x%x, IsReady = %d, ReadTimes = %d,delay 1 us\n", path, r_bff8, is_ready, count);

	/* for 0x80fc check NCTL DONE */	
	/* for 52A last line 0x9c00 */
	/* for 52B last line 0x8000 */
	count = 1;
	is_ready = false;
	while (count < 2000) {	
		/* r_80fc = 0; */
		r_80fc = halrf_rreg(rf, 0x80fc, MASKLWORD);
	
		if (r_80fc == NCTL_FINAL_LINE_8852C) {
			is_ready = true;
			break;
		}	
			
		halrf_delay_us(rf, 1);
		count++;
	}
	halrf_delay_us(rf, 1);
	rf->nctl_ck_times[1] = count;
		
	halrf_wreg(rf, 0x8010, 0x000000ff, 0x00);

	RF_DBG(rf, DBG_RF_RFK, "======> check 0x80fc[15:0] = 0x%x, IsReady = %d, ReadTimes = %d, 0x%x= 0x%x \n", r_80fc, is_ready, count, 0x8010, halrf_rreg(rf, 0x8010, MASKDWORD) ); 
#endif

	return is_ready;
}

bool halrf_do_one_shot_8852c(struct rf_info *rf, enum rf_path path, u32 nctl_addr, u32 mask, u32 process_id)
{

	halrf_before_one_shot_enable_8852c(rf);
	halrf_wreg(rf, nctl_addr, mask, process_id); //set cal_path, process id

	return halrf_one_shot_nctl_done_check_8852c(rf, path);
}

bool halrf_set_ch_lck_8852c(struct rf_info *rf, u32 path, u32 val)
{
	u32 c = 0;
	bool timeout = false;

	halrf_wrf(rf, path, 0xd3, BIT(8), 0x1);
	halrf_wrf(rf, path, 0x18, MASKRF, val);
	halrf_wrf(rf, path, 0xd3, BIT(8), 0x0);
	while (c < 1000) {
		if (halrf_rrf(rf, path, 0xb7, BIT(8)) == 0)
			break;
		c++;
		halrf_delay_us(rf, 1);
	}

	if (c == 1000) {
		timeout = true;
		RF_WARNING("[LCK]LCK timeout\n");
	}
	return timeout;
}

void halrf_lck_8852c(struct rf_info *rf)
{
	u32 temp05[2], temp18, temp00[2], i;

	if (!rf->hal_com->dbcc_en) {
		RF_DBG(rf, DBG_RF_LCK, "[LCK]DO LCK\n");		
		temp05[0] = halrf_rrf(rf, 0, 0x5, MASKRF);
		temp05[1] = halrf_rrf(rf, 1, 0x5, MASKRF);
		temp00[0] = halrf_rrf(rf, 0, 0x0, MASKRF);
		temp00[1] = halrf_rrf(rf, 1, 0x0, MASKRF);
		temp18 = halrf_rrf(rf, 0, 0x18, MASKRF);
		halrf_wrf(rf, 0, 0x0, MASKRFMODE, 0x3);
		halrf_wrf(rf, 1, 0x0, MASKRFMODE, 0x3);
		halrf_wrf(rf, 0, 0x5, MASKRF, 0x0);
		halrf_wrf(rf, 1, 0x5, MASKRF, 0x0);
		halrf_set_ch_lck_8852c(rf, 0, temp18);
		halrf_wrf(rf, 0, 0x0, MASKRF, temp00[0]);
		halrf_wrf(rf, 1, 0x0, MASKRF, temp00[1]);
		halrf_wrf(rf, 0, 0x5, MASKRF, temp05[0]);
		halrf_wrf(rf, 1, 0x5, MASKRF, temp05[1]);
	} else {
		RF_DBG(rf, DBG_RF_LCK, "[LCK]DO DBCC LCK\n");
		for(i = 0; i < 2; i++) {
			temp05[i] = halrf_rrf(rf, i, 0x5, MASKRF);
			temp00[i] = halrf_rrf(rf, 0, 0x0, MASKRF);
			temp18 = halrf_rrf(rf, i, 0x18, MASKRF);
			halrf_wrf(rf, i, 0x0, MASKRFMODE, 0x3);
			halrf_wrf(rf, i, 0x5, MASKRF, 0x0);
			halrf_set_ch_lck_8852c(rf, i, temp18);
			halrf_wrf(rf, i, 0x0, MASKRF, temp00[i]);
			halrf_wrf(rf, i, 0x5, MASKRF, temp05[i]);
		}
	}
	rf->lck_ther_s0 = rf->cur_ther_s0;
	rf->lck_ther_s1 = rf->cur_ther_s1;
}

void halrf_lck_tracking_8852c(struct rf_info *rf)
{
	u32 temp;

	RF_DBG(rf, DBG_RF_LCK,
		"[LCK]curT_s0(%d), curT_s1(%d), lckT_s0(%d), lckT_s1(%d)\n",
		rf->cur_ther_s0, rf->cur_ther_s1,
		rf->lck_ther_s0, rf->lck_ther_s1);
	if (((rf->lck_ther_s0 == 0) && (rf->lck_ther_s1 == 0))) {
		halrf_lck_trigger(rf);
		return;
	}
	if (HALRF_ABS(rf->lck_ther_s0, rf->cur_ther_s0) >
		HALRF_ABS(rf->lck_ther_s1, rf->cur_ther_s1)) {
		temp = HALRF_ABS(rf->lck_ther_s0, rf->cur_ther_s0);
	}else {
		temp = HALRF_ABS(rf->lck_ther_s1, rf->cur_ther_s1);
	}

	RF_DBG(rf, DBG_RF_LCK, "[LCK]MAX_delta = %d, LCK_TH=%x\n",
		temp, LCK_TH_8852C);

	if(temp >= LCK_TH_8852C)		
		halrf_lck_trigger(rf);
}

static u32 check_rfc_reg_8852c[] = {0xfe, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x10000, 0x1, 0x10001,
	0x5, 0x10005, 0x8, 0x18, 0x10018,
	0x2, 0x10002, 0x11, 0x10011, 0x53,
	0x10055,	0x58, 0x63, 0x6e, 0x6f,
	0x7e, 0x7f, 0x80, 0x81, 0x8d,
	0x8f, 0x90, 0x92, 0x93, 0xa0,
	0xb2, 0xc5};
static u32 check_dack_reg_8852c[] = {0x12a0, 0x32a0, 0x12b8, 0x32b8, 0x030c,
	0x032c, 0xc000, 0xc004, 0xc020, 0xc024,
	0xc100, 0xc104, 0xc120, 0xc124, 0xc0d4,
	0xc1d4, 0xc0f0, 0xc0f4, 0xc1f0, 0xc1f4,
	0xc05c, 0xc080, 0xc048, 0xc06c, 0xc060,
	0xc084, 0xc15c, 0xc180, 0xc148, 0xc16c,
	0xc160, 0xc184, 0xc040, 0xc064, 0xc140,
	0xc164};
static u32 check_iqk_reg_8852c[] = {0x8000, 0x8004, 0x8008, 0x8080, 0x808c,
	0x8120, 0x8124, 0x8138, 0x813c, 0x81dc,
	0x8220, 0x8224, 0x8238, 0x823c, 0x82dc,
	0x9fe0, 0x9fe4, 0x9fe8, 0x9fec, 0x9f30,
	0x9f40, 0x9f50, 0x9f60, 0x9f70, 0x9f80,
	0x9f90, 0x9fa0};
static u32 check_dpk_reg_8852c[] = {0x80b0, 0x81bc, 0x82bc, 0x81b4, 0x82b4,
	0x81c4, 0x82c4, 0x81c8, 0x82c8, 0x58d4,
	0x78d4};
static u32 check_tssi_reg_8852c[] = {0x0304, 0x5818, 0x581c, 0x5820, 0x1c60,
	0x1c44, 0x5838, 0x5630, 0x5634, 0x58f8,
	0x12c0, 0x120c, 0x1c04, 0x1c0c, 0x1c18,
	0x7630, 0x7634, 0x7818, 0x781c, 0x7820,
	0x3c60, 0x3c44, 0x7838, 0x78f8, 0x32c0,
	0x320c, 0x3c04, 0x3c0c, 0x3c18};

void halrf_quick_checkrf_8852c(struct rf_info *rf)
{
	u32 path, temp, i;
	u32	len = sizeof(check_rfc_reg_8852c) / sizeof(u32);
	u32	*add = (u32 *)check_rfc_reg_8852c;

	/*check RFC*/
	RF_TRACE("======RFC======\n");
	for (path = 0; path < 2; path++) {
		for (i = 0; i < len; i++ ) {
			temp = halrf_rrf(rf, path, add[i], MASKRF);
			RF_TRACE("RF%d 0x%x = 0x%x\n", path, add[i], temp);
		}
	}
	/*check DACK*/
	RF_TRACE("======DACK======\n");
	len = sizeof(check_dack_reg_8852c) / sizeof(u32);
	add = check_dack_reg_8852c;
	for (i = 0; i < len; i++ ) {
		temp = halrf_rreg(rf, add[i], MASKDWORD);
		RF_TRACE("0x%x = 0x%x\n", add[i], temp);
	}
	/*check IQK*/
	RF_TRACE("======IQK======\n");
	len = sizeof(check_iqk_reg_8852c) / sizeof(u32);
	add = check_iqk_reg_8852c;

	for (i = 0; i < len; i++ ) {
		temp = halrf_rreg(rf, add[i], MASKDWORD);
		RF_TRACE("0x%x = 0x%x\n", add[i], temp);
	}
	/*check DPK*/
	RF_TRACE("======DPK======\n");
	len = sizeof(check_dpk_reg_8852c) / sizeof(u32);
	add = check_dpk_reg_8852c;
	for (i = 0; i < len; i++ ) {
		temp = halrf_rreg(rf, add[i], MASKDWORD);
		RF_TRACE("0x%x = 0x%x\n", add[i], temp);
	}
	/*check TSSI*/
	RF_TRACE("======TSSI======\n");
	len = sizeof(check_tssi_reg_8852c) / sizeof(u32);
	add = check_tssi_reg_8852c;
	for (i = 0; i < len; i++ ) {
		temp = halrf_rreg(rf, add[i], MASKDWORD);
		RF_TRACE("0x%x = 0x%x\n", add[i], temp);
	}
}

static u32 backup_mac_reg_8852c[] = {0x0};
static u32 backup_bb_reg_8852c[] = {0xc0d4,0xc0d8, 0xc0e8, 0xc1d4,0xc1d8, 0xc1e8};
static u32 backup_rf_reg_8852c[] = {0xdc, 0xef, 0xde, 0x0, 0x1e, 0x5, 0x10005};

static struct halrf_iqk_ops iqk_ops= {
    .iqk_kpath = halrf_kpath_8852c,
    .iqk_mcc_page_sel = iqk_mcc_page_sel_8852c,
    .iqk_get_ch_info = iqk_get_ch_info_8852c,
    .iqk_preset = iqk_preset_8852c,
    .iqk_macbb_setting = iqk_macbb_setting_8852c,
    .iqk_start_iqk = iqk_start_iqk_8852c,
    .iqk_restore = iqk_restore_8852c,
    .iqk_afebb_restore = iqk_afebb_restore_8852c,  
};

struct rfk_iqk_info rf_iqk_hwspec_8852c = {
  	.rf_iqk_ops = &iqk_ops,
	.rf_max_path_num = 2,
	.rf_iqk_version = iqk_version_8852c,
	.rf_iqk_ch_num = 2,
	.rf_iqk_path_num = 2,

#if 0
	.backup_mac_reg = backup_mac_reg_8852c,
	.backup_mac_reg_num = ARRAY_SIZE(backup_mac_reg_8852c),
#else
	.backup_mac_reg = backup_mac_reg_8852c,
	.backup_mac_reg_num = 0,
#endif

    	.backup_bb_reg = backup_bb_reg_8852c,
    	.backup_bb_reg_num = ARRAY_SIZE(backup_bb_reg_8852c),
    	.backup_rf_reg = backup_rf_reg_8852c,
    	.backup_rf_reg_num = ARRAY_SIZE(backup_rf_reg_8852c),
};

#endif
