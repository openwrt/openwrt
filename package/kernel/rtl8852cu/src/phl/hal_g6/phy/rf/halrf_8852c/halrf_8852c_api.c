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

u8 halrf_get_thermal_8852c(struct rf_info *rf, enum rf_path rf_path)
{
	u8 thermal = 0;

	halrf_wrf(rf, rf_path, 0x42, BIT(19), 0x1);
	halrf_wrf(rf, rf_path, 0x42, BIT(19), 0x0);
	halrf_wrf(rf, rf_path, 0x42, BIT(19), 0x1);

	halrf_delay_us(rf, 200);


	thermal = (u8)halrf_rrf(rf, rf_path, 0x42, 0x0007e);
	return thermal;
}

void halrf_set_syn01_8852c(struct rf_info *rf, bool dbcc_en)
{
	if (dbcc_en == 1) {
		/*SYN0 on + SYN1 on*/
		halrf_wrf(rf, RF_PATH_A, 0xA0, (BIT(3) | BIT(2)), 0x3);
		halrf_wrf(rf, RF_PATH_B, 0xA0, (BIT(3) | BIT(2)), 0x3);
		/*halrf_by_rate_dpd_8852c(rf, HW_PHY_1);*/
	} else if (dbcc_en == 0) {
		/*SYN0 on + SYN1 off*/
		halrf_wrf(rf, RF_PATH_A, 0xA0, (BIT(3) | BIT(2)), 0x3);
		halrf_wrf(rf, RF_PATH_B, 0xA0, (BIT(3) | BIT(2)), 0x0);
	}
}

u32 halrf_mac_get_pwr_reg_8852c(struct rf_info *rf, enum phl_phy_idx phy,
	u32 addr, u32 mask)
{
	struct rtw_hal_com_t *hal = rf->hal_com;
	u32 result, ori_val, bit_shift, reg_val;

	result = rtw_hal_mac_get_pwr_reg(hal, phy, addr, &ori_val);
	if (result)
		RF_WARNING("=======>%s Get MAC(0x%x) fail, error code=%d\n",
			__func__, addr, result);
	else
		RF_DBG(rf, DBG_RF_POWER, "Get MAC(0x%x) ok!!! 0x%08x\n",
			addr, ori_val);

	bit_shift = halrf_cal_bit_shift(mask);
	reg_val = (ori_val & mask) >> bit_shift;

	return reg_val;
}

u32 halrf_mac_set_pwr_reg_8852c(struct rf_info *rf, enum phl_phy_idx phy,
	u32 addr, u32 mask, u32 val)
{
	struct rtw_hal_com_t *hal = rf->hal_com;
	u32 result;

	result = rtw_hal_mac_write_msk_pwr_reg(hal, phy, addr, mask, val);
	if (result) {
		RF_WARNING("=======>%s Set MAC(0x%x[0x%08x]) fail, error code=%d\n",
			__func__, addr, mask, result);
		return false;
	} else
		RF_DBG(rf, DBG_RF_POWER, "Set MAC(0x%x[0x%08x])=0x%08x ok!!! \n",
			addr, mask, val);

	return result;
}

void halrf_wlan_tx_power_control_8852c(struct rf_info *rf,
	enum phl_phy_idx phy, enum phl_pwr_ctrl pwr_ctrl_idx,
	u32 tx_power_val, bool enable)
{
	u32 phy_tmp;

	RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] ==>%s phy=%d pwr_ctrl_idx=%d tx_power_val=%d enable=%d\n",
		__func__, phy, pwr_ctrl_idx, tx_power_val, enable);

	if (rf->hal_com->dbcc_en)
		phy_tmp = phy;
	else
		phy_tmp = HW_PHY_0;

	if (enable == false) {
		if (pwr_ctrl_idx == ALL_TIME_CTRL) {
			/*all-time control Disable*/
			halrf_mac_set_pwr_reg_8852c(rf, phy_tmp, 0xd200, 0x3ff, 0x0);
		} else if (pwr_ctrl_idx == GNT_TIME_CTRL) {
			/*GNT_BT control Disable*/
			halrf_mac_set_pwr_reg_8852c(rf, phy_tmp, 0xd220, BIT(1), 0x0);
			halrf_mac_set_pwr_reg_8852c(rf, phy_tmp, 0xd220, 0xff8, 0x0);
		}
	} else {
		if (pwr_ctrl_idx == ALL_TIME_CTRL) {
			halrf_mac_set_pwr_reg_8852c(rf, phy_tmp, 0xd200, 0x3ff,
				((tx_power_val & 0x1ff) | BIT(9)));
		} else if (pwr_ctrl_idx == GNT_TIME_CTRL) {
			halrf_mac_set_pwr_reg_8852c(rf, phy_tmp, 0xd220, BIT(1), 0x1);
			halrf_mac_set_pwr_reg_8852c(rf, phy_tmp, 0xd220, 0xff8, tx_power_val & 0x1ff);
		} else {
			RF_WARNING("[Pwr Ctrl] ==>%s (pwr_ctrl_idx = %d) don't exist !!!\n",
				__func__, pwr_ctrl_idx);
		}
	}
}

bool halrf_wl_tx_power_control_8852c(struct rf_info *rf, u32 tx_power_val)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	u32 result; 
	s32 tmp_pwr;
	u8 phy = 0;
	u32 all_time_control = 0;
	u32 gnt_bt_control = 0;

	RF_DBG(rf, DBG_RF_POWER, "=======>%s\n", __func__);

	all_time_control = tx_power_val & 0xffff;
	gnt_bt_control = (tx_power_val & 0xffff0000) >> 16;

	RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl]tx_power_val=0x%x   all_time_control=0x%x   gnt_bt_control=0x%x\n",
		tx_power_val, all_time_control, gnt_bt_control);

	if (all_time_control == 0xffff) {
		/*Coex Disable*/
		pwr->coex_pwr_ctl_enable = false;
		pwr->coex_pwr = 0;
		RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] Coex Disable all_time_control=0xffff!!!\n");
	} else if (all_time_control == 0xeeee) {
		/*DPK Disable*/
		pwr->dpk_pwr_ctl_enable = false;
		pwr->dpk_pwr = 0;
		RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] DPK Disable all_time_control=0xeeee\n");
	} else {
		if (all_time_control & BIT(15)) {
			/*DPK*/
			pwr->dpk_pwr_ctl_enable = true;
			pwr->dpk_pwr = all_time_control & 0x1ff;

			RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] DPK Enable Set pwr->dpk_pwr = %d\n",
				pwr->dpk_pwr);
		} else {
			/*Coex*/
			pwr->coex_pwr_ctl_enable = true;
			pwr->coex_pwr = all_time_control & 0x1ff;

			RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] Coex Enable Set pwr->coex_pwr = %d\n",
				pwr->coex_pwr);
		}
	}

	if (pwr->coex_pwr_ctl_enable == true && pwr->dpk_pwr_ctl_enable == false) {
		tmp_pwr = pwr->coex_pwr;

		RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] coex_pwr_ctl_enable=true dpk_pwr_ctl_enable=false tmp_pwr=%d\n",
			tmp_pwr);
	} else if (pwr->coex_pwr_ctl_enable == false && pwr->dpk_pwr_ctl_enable == true) {
		tmp_pwr = pwr->dpk_pwr;

		RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] coex_pwr_ctl_enable=false dpk_pwr_ctl_enable=true tmp_pwr=%d\n",
			tmp_pwr);
	} else if (pwr->coex_pwr_ctl_enable == true && pwr->dpk_pwr_ctl_enable == true) {
		if (pwr->coex_pwr > pwr->dpk_pwr)
			tmp_pwr = pwr->dpk_pwr;
		else
			tmp_pwr = pwr->coex_pwr;

		RF_DBG(rf, DBG_RF_POWER, "[Pwr Ctrl] coex_pwr_ctl_enable=true dpk_pwr_ctl_enable=true tmp_pwr=%d\n",
			tmp_pwr);
	} else
		tmp_pwr = 0;

	if (pwr->coex_pwr_ctl_enable == false && pwr->dpk_pwr_ctl_enable == false) {
		/*all-time control Disable*/
		result = halrf_mac_set_pwr_reg_8852c(rf, phy, 0xd200, 0x3ff, 0x0);

		if (result) {
			RF_WARNING("=======>%s Set MAC(0xd200) fail, error code=%d\n",
				__func__, result);
			return false;
		} else {
			RF_DBG(rf, DBG_RF_POWER, "Set MAC(0xd200) ok!!!\n");
			rf->is_coex = false;
		}
	} else {
		/*all-time control*/
		result = halrf_mac_set_pwr_reg_8852c(rf, phy, 0xd200, 0x3ff, ((tmp_pwr & 0x1ff) | BIT(9)));
		if (result) {
			RF_WARNING("=======>%s Set MAC(0xd200) fail, error code=%d\n",
				__func__, result);
			return false;
		} else {
			RF_DBG(rf, DBG_RF_POWER, "Set MAC(0xd200) ok!!!\n");
			rf->is_coex = true;
		}
	}

	if (gnt_bt_control == 0xffff) {
		/*GNT_BT control*/

		RF_DBG(rf, DBG_RF_POWER, "=======>%s   gnt_bt_control = 0x%x\n",
			__func__, gnt_bt_control);

		result = halrf_mac_set_pwr_reg_8852c(rf, phy, 0xd220, BIT(1), 0x0);
		result = halrf_mac_set_pwr_reg_8852c(rf, phy, 0xd220, 0xff8, 0x0);
		if (result) {
			RF_WARNING("=======>%s Set MAC(0xd220) fail, error code=%d\n",
				__func__, result);
			return false;
		} else {
			RF_DBG(rf, DBG_RF_POWER, "Set MAC(0xd220) ok!!!\n");
			rf->is_coex = false;
		}
	} else {
		/*GNT_BT control*/

		RF_DBG(rf, DBG_RF_POWER, "=======>%s   gnt_bt_control = 0x%x\n",
			__func__, gnt_bt_control);

		result = halrf_mac_set_pwr_reg_8852c(rf, phy, 0xd220, BIT(1), 0x1);
		result = halrf_mac_set_pwr_reg_8852c(rf, phy, 0xd220, 0xff8, gnt_bt_control & 0x1ff);
		if (result) {
			RF_WARNING("=======>%s Set MAC(0xd220) fail, error code=%d\n",
				__func__, result);
			return false;
		} else {
			RF_DBG(rf, DBG_RF_POWER, "Set MAC(0xd220) ok!!!\n");
			rf->is_coex = true;
		}
	}

	return true;
}

s8 halrf_get_ther_protected_threshold_8852c(struct rf_info *rf)
{
	u8 tmp_a, tmp_b, tmp;
	u8 therml_max = 0x32;

	tmp_a = halrf_get_thermal(rf, RF_PATH_A);
	tmp_b = halrf_get_thermal(rf, RF_PATH_B);

#ifdef HALRF_THERMAL_PROTECT_SUPPORT
	if (rf->phl_com->dev_sw_cap.thermal_threshold != 0xff)
		therml_max = rf->phl_com->dev_sw_cap.thermal_threshold;
#endif

	if (tmp_a > tmp_b)
		tmp = tmp_a;
	else
		tmp = tmp_b;

	if (tmp > therml_max)
		return -1;	/*Tx duty reduce*/
	else if (tmp < therml_max - 2)
		return 1;	/*Tx duty up*/
	else 
		return 0;	/*Tx duty the same*/
}

s8 halrf_xtal_tracking_offset_8852c(struct rf_info *rf,
					enum phl_phy_idx phy)
{
	struct halrf_xtal_info *xtal_trk = &rf->xtal_track;
	u8 thermal_a = 0xff, thermal_b = 0xff;
	u8 tmp_a, tmp_b, tmp;
	s8 xtal_ofst = 0;

	RF_DBG(rf, DBG_RF_XTAL_TRACK, "======>%s   phy=%d\n",
		__func__, phy);

	tmp_a = halrf_get_thermal(rf, RF_PATH_A);
	tmp_b = halrf_get_thermal(rf, RF_PATH_B);
	halrf_efuse_get_info(rf, EFUSE_INFO_RF_THERMAL_A, &thermal_a, 1);
	halrf_efuse_get_info(rf, EFUSE_INFO_RF_THERMAL_B, &thermal_b, 1);

	if (thermal_a == 0xff || thermal_b == 0xff ||
		thermal_a == 0x0 || thermal_b == 0x0) {
		RF_DBG(rf, DBG_RF_XTAL_TRACK, "======>%s PG ThermalA=%d ThermalB=%d\n",
			__func__, thermal_a, thermal_b);
		return 0;
	}

	if (tmp_a > tmp_b) {
		if (tmp_a > thermal_a) {
			tmp = tmp_a - thermal_a;
			if (tmp >= DELTA_SWINGIDX_SIZE)
				tmp = DELTA_SWINGIDX_SIZE - 1;
			xtal_ofst = xtal_trk->delta_swing_xtal_table_idx_p[tmp];
		} else {
			tmp = thermal_a - tmp_a;
			if (tmp >= DELTA_SWINGIDX_SIZE)
				tmp = DELTA_SWINGIDX_SIZE - 1;
			xtal_ofst = xtal_trk->delta_swing_xtal_table_idx_n[tmp];
		}
	} else {
		if (tmp_b > thermal_b) {
			tmp = tmp_b - thermal_b;
			if (tmp >= DELTA_SWINGIDX_SIZE)
				tmp = DELTA_SWINGIDX_SIZE - 1;
			xtal_ofst = xtal_trk->delta_swing_xtal_table_idx_p[tmp];
		} else {
			tmp = thermal_b - tmp_b;
			if (tmp >= DELTA_SWINGIDX_SIZE)
				tmp = DELTA_SWINGIDX_SIZE - 1;
			xtal_ofst = xtal_trk->delta_swing_xtal_table_idx_n[tmp];
		}
	}

	RF_DBG(rf, DBG_RF_XTAL_TRACK, "PG ThermalA=%d   ThermalA=%d\n",
		thermal_a, tmp_a);

	RF_DBG(rf, DBG_RF_XTAL_TRACK, "PG ThermalB=%d   ThermalB=%d\n",
		thermal_b, tmp_b);

	RF_DBG(rf, DBG_RF_XTAL_TRACK, "xtal_ofst[%d]=%d\n",
		tmp, xtal_ofst);

	return xtal_ofst;
}
void halrf_syn1_onoff_8852c(struct rf_info *rf, enum phl_phy_idx phy, u8 path, bool syn1_turn_on)
{
	if(path != RF_PATH_B)
		return;
	
	if (syn1_turn_on) {
		//Re-write LUT_SYNMODE_S1 to turn on SYN1 when exit 2x2 mode (DBCC or 1x1)
		halrf_wrf(rf, RF_PATH_B, 0xED, MASKRF, 0x00010);      //LUT write enable ED[4]=1

		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00001);     //write register WA 33[2:0]=001
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000E);     //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=1
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00002);     //write register WA 33[2:0]=010
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000E);     //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=1
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00003);     //write register WA 33[2:0]=011
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000E);     //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=1
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00005);     //write register WA 33[2:0]=101
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000D);     //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=1
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00006);     //write register WA 33[2:0]=110
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000D);     //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=1
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00007);     //write register WA 33[2:0]=111
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000D);     //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=1

		halrf_wrf(rf, RF_PATH_B, 0xED, MASKRF, 0x00000);      //LUT write disable ED[4]=0
	} else {
		//Re-write LUT_SYNMODE_S1 to turn off SYN1 at 2x2 mode
		//need to go back to default LUT_SYNMODE_S1 table if it is not 2x2 mode
		halrf_wrf(rf, RF_PATH_B, 0xED, MASKRF, 0x00010);	    //LUT write enable ED[4]=1
		
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00001);	   //write register WA 33[2:0]=001
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000A);	   //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=0
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00002);	   //write register WA 33[2:0]=010
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000A);	   //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=0
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00003);	   //write register WA 33[2:0]=011
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000A);	   //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=0
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00005);	   //write register WA 33[2:0]=101
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000A);	   //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=0
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00006);	   //write register WA 33[2:0]=110
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000A);	   //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=0
		halrf_wrf(rf, RF_PATH_B, 0x33, MASKRF, 0x00007);	   //write register WA 33[2:0]=111
		halrf_wrf(rf, RF_PATH_B, 0x3F, MASKRF, 0x0000A);	   //write register WD 3F[3:0]=1010: 3F[2]=POW_SYN=0
		
		halrf_wrf(rf, RF_PATH_B, 0xED, MASKRF, 0x00000);	    //LUT write disable ED[4]=0
	}
	return;
}

void halrf_txck_force_8852c(struct rf_info *rf, enum rf_path path, bool force, enum dac_ck ck)
{
	halrf_wreg(rf, 0x12a0 | (path <<13), BIT(15), 0x0);

	if (!force)
		return;

	halrf_wreg(rf, 0x12a0 | (path <<13), 0x7000, ck);
	halrf_wreg(rf, 0x12a0 | (path <<13), BIT(15), 0x1);
}

void halrf_rxck_force_8852c(struct rf_info *rf, enum rf_path path, bool force, enum adc_ck ck)
{
	halrf_wreg(rf, 0x12a0 | (path <<13), BIT(19), 0x0);

	if (!force)
		return;

	halrf_wreg(rf, 0x12a0 | (path <<13), 0x70000, ck);
	halrf_wreg(rf, 0x12a0 | (path <<13), BIT(19), 0x1);
}

void halrf_set_gpio_8852c(struct rf_info *rf, enum phl_phy_idx phy)
{
	u8 rfe_type = rf->phl_com->dev_cap.rfe_type;

	RF_DBG(rf, DBG_RF_INIT, "===>%s   rfe_type=%d   phy=%d\n",
		__func__, rfe_type, phy);

	if (rfe_type == 51 || rfe_type == 52) {
		/*Set BB GPIO Setting*/
		/*PathA=0x1*/
		halrf_gpio_setting(rf, 10, 0x1, true, HALRF_TRSW_RFM);
		halrf_gpio_setting(rf, 7, 0x1, false, HALRF_LNAON_RFM);
		halrf_gpio_setting(rf, 6, 0x1, false, HALRF_PAPE_RFM);
		/*PathB=0x2*/
		halrf_gpio_setting(rf, 1, 0x2, true, HALRF_TRSW_RFM);
		halrf_gpio_setting(rf, 2, 0x2, false, HALRF_LNAON_RFM);
		halrf_gpio_setting(rf, 3, 0x2, false, HALRF_PAPE_RFM);

		/*Set MAC GPIO Setting*/
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 1);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 2);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 3);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 6);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 7);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 10);
	} 

	if (rfe_type == 53 || rfe_type == 54 || rfe_type == 63 || rfe_type == 64) {
		/*Set BB GPIO Setting*/
		/*PathA=0x1*/
		halrf_gpio_setting(rf, 10, 0x1, true, HALRF_TRSW_RFM);
		halrf_gpio_setting(rf, 7, 0x1, true, HALRF_LNAON_RFM);
		halrf_gpio_setting(rf, 6, 0x1, false, HALRF_PAPE_RFM);
		/*PathB=0x2*/
		halrf_gpio_setting(rf, 1, 0x2, true, HALRF_TRSW_RFM);
		halrf_gpio_setting(rf, 2, 0x2, true, HALRF_LNAON_RFM);
		halrf_gpio_setting(rf, 3, 0x2, false, HALRF_PAPE_RFM);
		
		/*Set MAC GPIO Setting*/
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 1);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 2);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 3);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 6);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 7);
		halrf_set_gpio_func(rf, RTW_MAC_GPIO_WL_RFE_CTRL, 10);
	}
}


u32 bkup_kip_reg_8852c[] = {0x813c, 0x8124, 0x8120, 0xc0d4, 0xc0d8};
u32 bkup_bb_reg_8852c[] = {0x2344};
u32 bkup_rf_reg_8852c[] = {0x5, 0x10005, 0xdf};

u32 bkup_kip_val_8852c[RF_PATH_MAX_NUM][RF_BACKUP_KIP_REG_MAX_NUM] = {{0x0}};
u32 bkup_bb_val_8852c[RF_BACKUP_BB_REG_MAX_NUM] = {0x0};
u32 bkup_rf_val_8852c[RF_PATH_MAX_NUM][RF_BACKUP_RF_REG_MAX_NUM] = {{0x0}};

void do_bkup_kip_8852c(struct rf_info *rf, u8 path)
{
	u8 i;
	u32 num = ARRAY_SIZE(bkup_kip_reg_8852c);

	for (i = 0; i < num; i++) {
		if (i >= RF_BACKUP_KIP_REG_MAX_NUM) {
			RF_DBG(rf, DBG_RF_RFK,
			       "[RFK] %s backup size not enough\n", __func__);
			break;
		}
		bkup_kip_val_8852c[path][i] = halrf_rreg(rf, bkup_kip_reg_8852c[i] + (path << 8), MASKDWORD);
		
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Backup KIP(S%d) 0x%x = %x\n", path, bkup_kip_reg_8852c[i] + (path << 8), bkup_kip_val_8852c[path][i]);
	}
}

void do_bkup_bb_8852c(struct rf_info *rf)
{
	u32 i;
	u32 num = ARRAY_SIZE(bkup_bb_reg_8852c);

	for (i = 0; i < num; i++) {
		if (i >= RF_BACKUP_BB_REG_MAX_NUM) {
			RF_DBG(rf, DBG_RF_RFK,
			       "[RFK] %s backup size not enough\n", __func__);
			break;
		}
		bkup_bb_val_8852c[i] =
			halrf_rreg(rf, bkup_bb_reg_8852c[i], MASKDWORD);

		RF_DBG(rf, DBG_RF_RFK, "[RFK]backup bb reg : %x, value =%x\n",
		      bkup_bb_reg_8852c[i], bkup_bb_val_8852c[i]);
	}
}


void do_bkup_rf_8852c(struct rf_info *rf, u8 path)
{
	u8 i;
	u32 num = ARRAY_SIZE(bkup_rf_reg_8852c);

	for (i = 0; i < num; i++) {
		if (i >= RF_BACKUP_RF_REG_MAX_NUM) {
			RF_DBG(rf, DBG_RF_RFK,
			       "[RFK] %s backup size not enough\n", __func__);
			break;
		}
		bkup_rf_val_8852c[path][i] = halrf_rrf(rf, path, bkup_rf_reg_8852c[i], MASKRF);
		
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Backup RF S%d 0x%x = %x\n",
			path, bkup_rf_reg_8852c[i], bkup_rf_val_8852c[path][i]);
	}
}

void do_reload_kip_8852c(struct rf_info *rf, u8 path) 
{
	u8 i;
	u32 num = ARRAY_SIZE(bkup_kip_reg_8852c);

	for (i = 0; i < num; i++) {
		halrf_wreg(rf, bkup_kip_reg_8852c[i] + (path << 8), MASKDWORD, bkup_kip_val_8852c[path][i]);
		
		RF_DBG(rf, DBG_RF_RFK, "[RFK] Reload KIP(S%d) 0x%x = %x\n",path, bkup_kip_reg_8852c[i] + (path << 8),
			bkup_kip_val_8852c[path][i]);
	}
}

void do_reload_bb_8852c(struct rf_info *rf)
{
	u32 i;
	u32 num = ARRAY_SIZE(bkup_bb_reg_8852c);

	for (i = 0; i < num; i++) {
		halrf_wreg(rf,  bkup_bb_reg_8852c[i], MASKDWORD, bkup_bb_val_8852c[i]);

		RF_DBG(rf, DBG_RF_RFK, "[RFK] Reload BB 0x%x = 0x%x\n",
		       bkup_bb_reg_8852c[i], bkup_bb_val_8852c[i]);
	}
}


void do_reload_rf_8852c(struct rf_info *rf, u8 path)
{
	u8 i;
	u32 num = ARRAY_SIZE(bkup_rf_reg_8852c);

	for (i = 0; i < num; i++) {
		halrf_wrf(rf, path, bkup_rf_reg_8852c[i], MASKRF, bkup_rf_val_8852c[path][i]);
		
			RF_DBG(rf, DBG_RF_RFK, "[RFK] Reload RF S%d 0x%x = %x\n",
				path, bkup_rf_reg_8852c[i], bkup_rf_val_8852c[path][i]);
	}
}

void halrf_set_regulation_from_driver_8852c(struct rf_info *rf,
	u8 regulation_idx)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	pwr->regulation_idx = regulation_idx;
	RF_DBG(rf, DBG_RF_POWER, "Set regulation_idx=%d\n", pwr->regulation_idx);
}

bool halrf_set_dbcc_8852c(struct rf_info *rf, bool dbcc_en)
{
	halrf_set_syn01_8852c(rf, dbcc_en);
	if (dbcc_en) {
		halrf_set_ref_power_to_struct(rf, HW_PHY_1);
	}

	return true;
}

void halrf_adie_pow_ctrl_8852c(struct rf_info *rf, bool rf_off, bool others_off)
{

	if (rf_off) {
		halrf_wrf(rf, RF_PATH_A, 0x5, MASKRF,0x0);
		halrf_wrf(rf, RF_PATH_B, 0x5, MASKRF,0x0);
		halrf_wrf(rf, RF_PATH_A, 0x0, MASKRFMODE,0x0);
		halrf_wrf(rf, RF_PATH_B, 0x0, MASKRFMODE,0x0);
	} else {
		halrf_wrf(rf, RF_PATH_A, 0x5, MASKRF,0x1);
		halrf_wrf(rf, RF_PATH_B, 0x5, MASKRF,0x1);
	}
	
	if (others_off) {
		/*ARFC off*/
		/*XTAL off*/
		rtw_hal_mac_set_xsi((rf)->hal_com, 0x0, 0x0);
	} else {
		/*ARFC on*/
		/*XTAL on*/
		rtw_hal_mac_set_xsi((rf)->hal_com, 0x0, 0xff);
	}
}

void halrf_afe_pow_ctrl_8852c(struct rf_info *rf, bool adda_off, bool pll_off)
{
	if (adda_off) {
		halrf_wmac(rf, 0x14, 0xc0000, 0x0);
		halrf_wreg(rf, 0x12b8, BIT(30), 0x1);
		halrf_wreg(rf, 0x32b8, BIT(30), 0x1);
		halrf_wreg(rf, 0x030c, 0xff000000, 0xe3);
		halrf_wreg(rf, 0x032c, 0xffff0000, 0x8049);
		halrf_wreg(rf, 0x032c, BIT(22), 0x0);
		halrf_wreg(rf, 0x030c, 0x0f000000, 0xc);
		halrf_wreg(rf, 0x030c, 0xe0000000, 0x0);
		halrf_wreg(rf, 0x032c, 0x60000000, 0x3);
		halrf_wreg(rf, 0xc0e4, 0x300, 0x0);
		halrf_wreg(rf, 0xc1e4, 0x300, 0x0);
	} else {
		halrf_wreg(rf, 0x12b8, BIT(30), 0x0);
		halrf_wreg(rf, 0x32b8, BIT(30), 0x0);
		halrf_wreg(rf, 0xc0e4, 0x300, 0x3);
		halrf_wreg(rf, 0xc1e4, 0x300, 0x3);
	}

	if (pll_off) {
		halrf_wmac(rf, 0x248, 0x7, 0x0);
		halrf_wmac(rf, 0x24c, 0x1, 0x0);
		halrf_wmac(rf, 0x14, 0x30000, 0x0);
		halrf_wmac(rf, 0x280, 0x7f, 0x0);
		halrf_wmac(rf, 0x14, 0x3000000, 0x0);
	} else {
		halrf_wmac(rf, 0x248, 0x7, 0x7);
		halrf_wmac(rf, 0x24c, 0x1, 0x2);
		halrf_wmac(rf, 0x14, 0x30000, 0x3);
		halrf_wmac(rf, 0x280, 0x7f, 0x7f);
		halrf_wmac(rf, 0x14, 0x3000000, 0x3);
	}
}
#endif
