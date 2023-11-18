/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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

#include "halbb_precomp.h"

u8 halbb_ex_cn_report(struct bb_info * bb)
{
	u8 rpt = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		 rpt = halbb_ex_cn_report_8852b(bb);
		 break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt =  halbb_ex_cn_report_8852c(bb);
		break;
	#endif
	default:
		break;
	}

	return rpt;
}

u8 halbb_ex_evm_1ss_report(struct bb_info * bb)
{
	u8 rpt = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		 rpt = halbb_ex_evm_1ss_report_8852b(bb);
		 break;
	#endif
	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_ex_evm_1ss_report_8851b(bb);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ex_evm_1ss_report_8852c(bb);
		break;
	#endif
	default:
		break;
	}

	return rpt;
}

u8 halbb_ex_evm_max_report(struct bb_info * bb)
{
	u8 rpt = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		 rpt = halbb_ex_evm_max_report_8852b(bb);
		 break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ex_evm_max_report_8852c(bb);
		break;
	#endif
	default:
		break;
	}

	return rpt;
}

u8 halbb_ex_evm_min_report(struct bb_info * bb)
{
	u8 rpt = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		 rpt = halbb_ex_evm_min_report_8852b(bb);
		 break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ex_evm_min_report_8852c(bb);
		break;
	#endif
	default:
		break;
	}

	return rpt;
}

u8 halbb_ch_2_band(struct bb_info *bb, u8 fc_ch)
{
	u8 band = 0;

	// 2G Band: (0)
	// 5G Band: (1):Low, (2): Mid, (3):High
	if (fc_ch >= 0 && fc_ch <= 14)
		band = 0;
	else if (fc_ch >= 36 && fc_ch <= 64)
		band = 1;
	else if (fc_ch >= 100 && fc_ch <= 144)
		band = 2;
	else if (fc_ch >= 149 && fc_ch <= 177)
		band = 3;

	return band;
}

#ifdef BB_8852B_SUPPORT

bool halbb_rf_sw_si_test(struct bb_info *bb, enum rf_path rx_path, u8 reg_addr, int ch_idx)
{
	u32 channel_change[3] = {0x1, 0x24, 0x99};
	u32 ofdm_rx = 0x0, reg_value_0 = 0x0, reg_value_1 = 0x0;
	ofdm_rx = (u32)rx_path;

	ch_idx = ch_idx % 3;

		if (ofdm_rx == RF_PATH_A) {
			halbb_write_rf_reg_8852b_a(bb, RF_PATH_A, reg_addr, 0x3ff, channel_change[ch_idx]);

			reg_value_0 = halbb_read_rf_reg_8852b_a(bb, RF_PATH_A, reg_addr, 0x3ff);
			BB_DBG(bb, DBG_PHY_CONFIG, "read_value (%d) = %x\n", ofdm_rx, reg_value_0);

			if (reg_value_0 == channel_change[ch_idx]){
				return true;
			} else {
				return false;
			}

		} else if (ofdm_rx == RF_PATH_B) {

			halbb_write_rf_reg_8852b_a(bb, RF_PATH_B, reg_addr, 0x3ff, channel_change[ch_idx]);

			reg_value_1 = halbb_read_rf_reg_8852b_a(bb, RF_PATH_B, reg_addr, 0x3ff);
			BB_DBG(bb, DBG_PHY_CONFIG, "read_value (%d) = %x\n", ofdm_rx, reg_value_1);

			if (reg_value_1 == channel_change[ch_idx]){
				return true;
			} else {
				return false;
			}

		} else {

			halbb_write_rf_reg_8852b_a(bb, RF_PATH_B, reg_addr, 0x3ff, channel_change[ch_idx]);
			halbb_write_rf_reg_8852b_a(bb, RF_PATH_A, reg_addr, 0x3ff, channel_change[ch_idx]);

			reg_value_0 = halbb_read_rf_reg_8852b_a(bb, RF_PATH_A, reg_addr, 0x3ff);
			reg_value_1 = halbb_read_rf_reg_8852b_a(bb, RF_PATH_B, reg_addr, 0x3ff);
			BB_DBG(bb, DBG_PHY_CONFIG, "read_value (%d) = %x\n", ofdm_rx, reg_value_0);
			BB_DBG(bb, DBG_PHY_CONFIG, "read_value (%d) = %x\n", ofdm_rx, reg_value_1);

			if ((reg_value_0 == channel_change[ch_idx]) && (reg_value_1 == channel_change[ch_idx])) {
				return true;
			} else {
				return false;
			}

		}
}

#endif
u16 halbb_get_csi_buf_idx(struct bb_info *bb, u8 buf_idx, u8 txsc_idx)

{
	u8 table_size = 0;
	u8 i;
	u8 txsc_2_buf_idx_160[][2] = {{0, 0}, //BW all
				      {8, 1}, //20M
				      {6, 3},
				      {4, 2},
				      {2, 4},
				      {1, 5},
				      {3, 7},
				      {5, 6},
				      {7, 8},
				      {12, 9}, //40M
				      {10, 10},
				      {9, 11},
				      {11, 12},
				      {14, 13}, //80M
				      {13, 14}};
	u8 txsc_2_buf_idx_080[][2] = {{0, 0}, //BW all
				      {4, 1}, //20M
				      {2, 3},
				      {1, 2},
				      {3, 4},
				      {10, 9}, //40M
				      {9, 10}};
	u8 csi_sub_idx = 0xff;
	u16 rpt_val = 0;

	if (bb->ic_type & BB_IC_MAX_BW_160) {
		table_size = sizeof(txsc_2_buf_idx_160) / (sizeof(u8) * 2);
		for (i = 0; i < table_size; i++) {
			if (txsc_2_buf_idx_160[i][0] == txsc_idx) {
				csi_sub_idx = txsc_2_buf_idx_160[i][1];
				break;
			}
		}
	} else {
		table_size = sizeof(txsc_2_buf_idx_080) / (sizeof(u8) * 2);
		for (i = 0; i < table_size; i++) {
			if (txsc_2_buf_idx_080[i][0] == txsc_idx) {
				csi_sub_idx = txsc_2_buf_idx_080[i][1];
				break;
			}
		}
	}

	BB_DBG(bb, DBG_DBG_API, "%02d -> %02d\n", txsc_idx, csi_sub_idx);

	if (csi_sub_idx == 0xff) {
		rpt_val = 0xff;
	} else {
		rpt_val = (buf_idx << 6) | (csi_sub_idx << 2);
	}

	return  rpt_val;
}

u16 halbb_cfg_cmac_tx_ant(struct bb_info *bb, enum rf_path tx_path)
{
	u16 val = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		val = halbb_cfg_cmac_tx_ant_8852a_2(bb, tx_path);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		val = halbb_cfg_cmac_tx_ant_8852b(bb, tx_path);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		val = halbb_cfg_cmac_tx_ant_8852c(bb, tx_path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		val = halbb_cfg_cmac_tx_ant_8192xb(bb, tx_path);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		val = halbb_cfg_cmac_tx_ant_8851b(bb, tx_path);
		break;
	#endif

	default:
		break;
	}
	return val;
}


void halbb_gpio_ctrl_dump(struct bb_info *bb)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_gpio_ctrl_dump_8852a_2(bb);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		//halbb_gpio_ctrl_dump_8852b(bb);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_gpio_ctrl_dump_8852c(bb);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_gpio_ctrl_dump_8192xb(bb);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_gpio_ctrl_dump_8851b(bb);
		break;
	#endif

	default:
		break;
	}
}

void halbb_gpio_rfm(struct bb_info *bb, enum bb_path path,
		    enum bb_rfe_src_sel src, bool dis_tx_gnt_wl,
		    bool active_tx_opt, bool act_bt_en, u8 rfm_output_val)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_gpio_rfm_8852a_2(bb, path, src, dis_tx_gnt_wl, active_tx_opt,
			               act_bt_en, rfm_output_val);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_gpio_rfm_8852c(bb, path, src, dis_tx_gnt_wl, active_tx_opt,
			             act_bt_en, rfm_output_val);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_gpio_rfm_8192xb(bb, path, src, dis_tx_gnt_wl, active_tx_opt,
			             act_bt_en, rfm_output_val);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_gpio_rfm_8851b(bb, path, src, dis_tx_gnt_wl, active_tx_opt,
				     act_bt_en, rfm_output_val);
		break;
	#endif

	default:
		break;
	}
}

void halbb_gpio_trsw_table(struct bb_info *bb, enum bb_path path,
			   bool path_en, bool trsw_tx, bool trsw_rx,
			   bool trsw, bool trsw_b)
{

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_gpio_trsw_table_8852a_2(bb, path, path_en, trsw_tx,
					      trsw_rx, trsw, trsw_b);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_gpio_trsw_table_8852c(bb, path, path_en, trsw_tx,
					    trsw_rx, trsw, trsw_b);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_gpio_trsw_table_8192xb(bb, path, path_en, trsw_tx,
					    trsw_rx, trsw, trsw_b);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_gpio_trsw_table_8851b(bb, path, path_en, trsw_tx,
					    trsw_rx, trsw, trsw_b);
		break;
	#endif

	default:
		break;
	}

}

void halbb_gpio_setting_all(struct bb_info *bb, u8 rfe_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_gpio_setting_init_8852a_2(bb);
		halbb_gpio_setting_all_8852a_2(bb, rfe_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:

		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_gpio_setting_init_8852c(bb);
		halbb_gpio_setting_all_8852c(bb, rfe_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_gpio_setting_init_8192xb(bb);
		halbb_gpio_setting_all_8192xb(bb, rfe_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_gpio_setting_init_8851b(bb);
		halbb_gpio_setting_all_8851b(bb, rfe_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_gpio_setting(struct bb_info *bb, u8 gpio_idx, enum bb_path path,
			bool inv, enum bb_rfe_src_sel src)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_gpio_setting_8852a_2(bb, gpio_idx, path, inv, src);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:

		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_gpio_setting_8852c(bb, gpio_idx, path, inv, src);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_gpio_setting_8192xb(bb, gpio_idx, path, inv, src);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_gpio_setting_8851b(bb, gpio_idx, path, inv, src);
		break;
	#endif

	default:
		break;
	}
}

void halbb_gpio_setting_init(struct bb_info *bb)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_gpio_setting_init_8852a_2(bb);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:

		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_gpio_setting_init_8852c(bb);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_gpio_setting_init_8192xb(bb);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_gpio_setting_init_8851b(bb);
		break;
	#endif

	default:
		break;
	}
}

bool halbb_tx_cfr_byrate_sup(struct bb_info *bb)
{
	bool rpt = false;

	switch (bb->ic_type) {

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = true;
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	return rpt;
}

void halbb_pre_agc_en(struct bb_info *bb, bool enable)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_pre_agc_en_8852a_2(bb, enable);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_pre_agc_en_8852b(bb, enable);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_pre_agc_en_8852c(bb, enable);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_pre_agc_en_8192xb(bb, enable);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_pre_agc_en_8851b(bb, enable);
		break;
	#endif

	default:
		break;
	}
}

void halbb_set_gain_error(struct bb_info *bb, u8 central_ch, enum band_type band, enum rf_path path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_set_gain_error_8852a_2(bb, central_ch, path);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_set_gain_error_8852b(bb, central_ch);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_set_gain_error_8852c(bb, central_ch, band, path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_set_gain_error_8192xb(bb, central_ch, band);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_set_gain_error_8851b(bb, central_ch);
		break;
	#endif

	default:
		break;
	}
}

u8 halbb_stop_ic_trx(struct bb_info *bb, u8 set_type)
{
	struct rtw_hal_com_t	*hal_i = bb->hal_com;
	u8 i = 0;
	bool trx_idle_success = false;
	u32 dp = 0;

	if (set_type == HALBB_SET) {
	/*[Stop TRX]---------------------------------------------------------*/
		if (bb->ic_type & BB_IC_AX_SERIES) {
			/*set debug port to 0x0*/
			if (!halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
				return HALBB_SET_FAIL;
			}
			halbb_set_bb_dbg_port_ip(bb, DBGPORT_IP_TD);
			halbb_set_bb_dbg_port(bb, 0x205);

			for (i = 0; i < 100; i++) {
				dp = halbb_get_bb_dbg_port_val(bb);
				/* CCA_all && mux_state*/
				if ((dp & 0x80000f00) == 0) {
					BB_DBG(bb, DBG_DBG_API, "Stop trx wait for (%d) times\n", i);
					trx_idle_success = true;
					break;
				}
				halbb_delay_ms(bb, 1);
			}
			halbb_release_bb_dbg_port(bb);
		}

		if (trx_idle_success) {
			/*pause all TX queue*/
			rtw_hal_tx_pause(hal_i, 0, true, PAUSE_RSON_PSD);
			rtw_hal_tx_pause(hal_i, 1, true, PAUSE_RSON_PSD);
			halbb_ctrl_rx_cca(bb, false, HW_PHY_0);
			halbb_ctrl_rx_cca(bb, false, HW_PHY_1);
		} else {
			return HALBB_SET_FAIL;
		}

		return HALBB_SET_SUCCESS;

	} else { /*@if (set_type == HALBB_REVERT)*/
		/*Release all TX queue*/
		rtw_hal_tx_pause(hal_i, 0, false, PAUSE_RSON_PSD);
		rtw_hal_tx_pause(hal_i, 1, false, PAUSE_RSON_PSD);
		halbb_ctrl_rx_cca(bb, true, HW_PHY_0);
		halbb_ctrl_rx_cca(bb, true, HW_PHY_1);

		return HALBB_SET_SUCCESS;
	}
}

u8 halbb_get_txsc(struct bb_info *bb, u8 pri_ch, u8 central_ch,
		  enum channel_width cbw, enum channel_width dbw)
{
	u8 txsc_idx = 0;
	u8 tmp = 0;
	u8 ofst = 0;

	if ((cbw == dbw) || (cbw == CHANNEL_WIDTH_20)) {
		txsc_idx = 0;
		BB_DBG(bb, DBG_PHY_CONFIG, "[TXSC] TxSC_idx = %d\n", txsc_idx);
		return txsc_idx;
	}

	switch (cbw) {
		case CHANNEL_WIDTH_40:
			txsc_idx = pri_ch > central_ch ? 1 : 2;
			break;
		case CHANNEL_WIDTH_80:
			if (dbw == CHANNEL_WIDTH_20) {
				if (pri_ch > central_ch)
					txsc_idx = (pri_ch - central_ch) >> 1;
				else
					txsc_idx = ((central_ch - pri_ch) >> 1) + 1;
			} else {
				txsc_idx = pri_ch > central_ch ? 9 : 10;
			}
			break;
		case CHANNEL_WIDTH_160:
			if (pri_ch > central_ch)
				tmp = (pri_ch - central_ch) >> 1;
			else
				tmp = ((central_ch - pri_ch) >> 1) + 1;

			if (dbw == CHANNEL_WIDTH_20) {
				txsc_idx = tmp;
			} else if (dbw == CHANNEL_WIDTH_40) {
				if ((tmp == 1) || (tmp == 3))
					txsc_idx = 9;
				else if ((tmp == 5) || (tmp == 7))
					txsc_idx = 11;
				else if ((tmp == 2) || (tmp == 4))
					txsc_idx = 10;
				else if ((tmp == 6) || (tmp == 8))
					txsc_idx = 12;
				else
					return 0xff;
			} else {
				txsc_idx = pri_ch > central_ch ? 13 : 14;
			}
			break;
		case CHANNEL_WIDTH_80_80:
			if (dbw == CHANNEL_WIDTH_20) {
				if (pri_ch > central_ch)
					txsc_idx = (10 - (pri_ch - central_ch)) >> 1;
				else
					txsc_idx = ((central_ch - pri_ch) >> 1) + 5;
			} else if (dbw == CHANNEL_WIDTH_40) {
				txsc_idx = pri_ch > central_ch ? 10 : 12;
			} else {
				txsc_idx = 14;
			}
		default:
			break;
	}
	BB_DBG(bb, DBG_PHY_CONFIG, "[TXSC] TxSC_idx = %d\n", txsc_idx);
	return txsc_idx;
}

u8 halbb_get_txsb(struct bb_info *bb, u8 pri_ch, u8 central_ch,
		    enum channel_width cbw, enum channel_width dbw)
{
	u8 txsb_idx = 0;
	u8 tmp = 0;
	u8 ofst = 0;

	if ((cbw == dbw) || (cbw == CHANNEL_WIDTH_20)) {
		txsb_idx = 0;
		BB_DBG(bb, DBG_PHY_CONFIG, "[TXSB] TxSB_idx = %d\n", txsb_idx);
		return txsb_idx;
	}

	switch (cbw) {
		case CHANNEL_WIDTH_40:
			txsb_idx = pri_ch > central_ch ? 1 : 0;
			break;
		case CHANNEL_WIDTH_80:
			if (dbw == CHANNEL_WIDTH_20)
				txsb_idx = (pri_ch - central_ch + 6) / 4;
			else
				txsb_idx = pri_ch > central_ch ? 1 : 0;
			break;
		case CHANNEL_WIDTH_160:
			if (dbw == CHANNEL_WIDTH_20)
				txsb_idx = (pri_ch - central_ch + 14) / 4;
			else if (dbw == CHANNEL_WIDTH_40)
				txsb_idx = (pri_ch - central_ch + 12) / 8;
			else
				txsb_idx = pri_ch > central_ch ? 1 : 0;
			break;
#ifdef BB_8922A_DVLP_SPF
		case CHANNEL_WIDTH_320:
			if (dbw == CHANNEL_WIDTH_20)
				txsb_idx = (pri_ch - central_ch + 30) / 4;
			else if (dbw == CHANNEL_WIDTH_40)
				txsb_idx = (pri_ch - central_ch + 28) / 8;
			else if (dbw == CHANNEL_WIDTH_80)
				txsb_idx = (pri_ch - central_ch + 24) / 16;
			else
				txsb_idx = pri_ch > central_ch ? 1 : 0;
			break;
#endif
		default:
			break;
	}
	BB_DBG(bb, DBG_PHY_CONFIG, "[TXSB] TxSB_idx = %d\n", txsb_idx);
	return txsb_idx;
}

void halbb_bb_reset_all(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_bb_reset_all_8852a_2(bb, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_reset_bb_phy(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_bb_reset_8852a_2(bb, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		#ifdef HALBB_FW_OFLD_SUPPORT
		if (halbb_check_fw_ofld(bb)) {
			halbb_fwofld_bb_reset_8852b(bb, phy_idx);
		} else {
			halbb_bb_reset_8852b(bb, phy_idx);
		}
		#else
		halbb_bb_reset_8852b(bb, phy_idx);
		#endif
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		#ifdef HALBB_FW_OFLD_SUPPORT
		if (halbb_check_fw_ofld(bb)) {
			//halbb_fwofld_bb_reset_8852c(bb, phy_idx);
		} else {
			halbb_bb_reset_8852c(bb, phy_idx);
		}
		#else
		halbb_bb_reset_8852c(bb, phy_idx);
		#endif
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_bb_reset_8192xb(bb, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_bb_reset_8851b(bb, phy_idx);
		break;
	#endif

	default:
		break;
	}

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif
}

void halbb_reset_bb(struct bb_info *bb)
{
	halbb_reset_bb_phy(bb, HW_PHY_0);
#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en)
		halbb_reset_bb_phy(bb, HW_PHY_1);
#endif
}

void halbb_tssi_bb_reset(struct bb_info *bb)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_tssi_bb_reset_8852a_2(bb);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:

		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_tssi_bb_reset_8852c(bb);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_tssi_bb_reset_8192xb(bb);
		break;
	#endif

	default:
		break;
	}
}

u32 halbb_read_rf_reg(struct bb_info *bb, enum rf_path path, u32 addr, u32 mask)
{
	u32 val = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		val = halbb_read_rf_reg_8852a_2(bb, path, addr, mask);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		val = halbb_read_rf_reg_8852b(bb, path, addr, mask);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		val = halbb_read_rf_reg_8852c(bb, path, addr, mask);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		val = halbb_read_rf_reg_8192xb(bb, path, addr, mask);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		val = halbb_read_rf_reg_8851b(bb, path, addr, mask);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		val = halbb_read_rf_reg_8922a(bb, path, addr, mask);
		break;
	#endif

	default:
		val = 0;
		break;
	}

	return val;
}

bool halbb_write_rf_reg(struct bb_info *bb, enum rf_path path, u32 addr, u32 mask,
			u32 data)
{
	bool rpt = true;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_write_rf_reg_8852a_2(bb, path, addr, mask, data);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_write_rf_reg_8852b(bb, path, addr, mask, data);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_write_rf_reg_8852c(bb, path, addr, mask, data);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_write_rf_reg_8192xb(bb, path, addr, mask, data);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_write_rf_reg_8851b(bb, path, addr, mask, data);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_write_rf_reg_8922a(bb, path, addr, mask, data);
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	return rpt;
}

bool halbb_rf_set_bb_reg(struct bb_info *bb, u32 addr, u32 mask, u32 data)
{
	bool rpt = false;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_rf_write_bb_reg_8852a_2(bb, addr, mask, data);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_rf_write_bb_reg_8852b(bb, addr, mask, data);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_rf_write_bb_reg_8852c(bb, addr, mask, data);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_rf_write_bb_reg_8192xb(bb, addr, mask, data);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_rf_write_bb_reg_8851b(bb, addr, mask, data);
		break;
	#endif

	default:
		break;
	}

	if (bb->bb_dbg_i.cr_recorder_rf_en)
		BB_TRACE("[RF][W] 0x%04x[0x%08x] = 0x%08x\n", addr, mask, data);

	if (!rpt)
		BB_WARNING("[%s][IQK]error IO 0x%x\n", __func__, addr);

	return rpt;
}

u32 halbb_rf_get_bb_reg(struct bb_info *bb, u32 addr, u32 mask)
{
	u32 val = 0;

	val = halbb_get_reg(bb, addr, mask);

	if (bb->bb_dbg_i.cr_recorder_rf_en)
		BB_TRACE("[RF][R] 0x%04x[0x%08x] = 0x%08x\n", addr, mask, val);

	return val;
}

void halbb_dfs_en(struct bb_info *bb, bool en)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_dfs_en_8852a_2(bb, en);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		//halbb_dfs_en_8852b(bb, en);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_dfs_en_8852c(bb, en);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_dfs_en_8192xb(bb, en);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_dfs_en_8922a(bb, en, HW_PHY_0);
		halbb_dfs_en_8922a(bb, en, HW_PHY_1);
		break;
	#endif

	default:
		break;
	}
}

void halbb_adc_en(struct bb_info *bb, bool en)
{
	enum phl_phy_idx phy_idx = HW_PHY_0;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_adc_en_8852a_2(bb, en);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_adc_en_8852b(bb, en);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_adc_en_8852c(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_adc_en_8192xb(bb, en);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_adc_en_8851b(bb, en);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_adc_en_8922a(bb, en, HW_PHY_0);
		halbb_adc_en_8922a(bb, en, HW_PHY_1);
		break;
	#endif

	default:
		break;
	}
}

void halbb_adc_ctrl_en(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_adc_en_8852a_2(bb, en);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_adc_en_8852b(bb, en);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_adc_en_8852c(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_adc_en_8192xb(bb, en);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_adc_en_8851b(bb, en);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_adc_en_8922a(bb, en, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

bool halbb_adc_cfg(struct bb_info *bb, enum channel_width bw, enum rf_path path,
		   enum phl_phy_idx phy_idx)
{
	bool rpt = false;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_adc_cfg_8852c(bb, bw, path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_adc_cfg_8922a(bb, bw, path, phy_idx);
		break;
	#endif

	default:
		break;
	}

	return rpt;
}

void halbb_tssi_cont_en(struct bb_info *bb, bool en, enum rf_path path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_tssi_cont_en_8852a_2(bb, en, path);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_tssi_cont_en_8852b(bb, en, path);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_tssi_cont_en_8852c(bb, en, path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_tssi_cont_en_8192xb(bb, en, path);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_tssi_cont_en_8851b(bb, en, path);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_tssi_cont_en_8922a(bb, en, path);
		break;
	#endif

	default:
		break;
	}
}

void halbb_bb_reset_cmn(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
#ifdef CONFIG_FW_IO_OFLD_SUPPORT
	halbb_fwofld_cfgcr_start(bb);
#endif
	if (en) {
		/*disable DFS/TSSI*/
		halbb_dfs_en(bb, false);
		if (!bb->hal_com->dbcc_en){
			halbb_tssi_cont_en(bb, false, RF_PATH_A);
			halbb_tssi_cont_en(bb, false, RF_PATH_B);
		}else{
			if(phy_idx == HW_PHY_0)
				halbb_tssi_cont_en(bb, false, RF_PATH_A);
			else
				halbb_tssi_cont_en(bb, false, RF_PATH_B);
		}
		/*disable ADC*/
		halbb_adc_en(bb, false);
		/* wait 40us*/
#ifdef CONFIG_FW_IO_OFLD_SUPPORT
		halbb_fw_delay(bb, 40);
#else
		_os_delay_us(bb->hal_com->drv_priv, 40);
#endif
		/* reset BB*/
		halbb_bb_reset_en(bb, false, phy_idx);
	} else {
		/*enable ADC*/
		halbb_adc_en(bb, true);
		/*enable DFS/TSSI*/
		halbb_dfs_en(bb, true);
		if (!bb->hal_com->dbcc_en) {
			halbb_tssi_cont_en(bb, true, RF_PATH_A);
			halbb_tssi_cont_en(bb, true, RF_PATH_B);
		}else{
			if(phy_idx == HW_PHY_0)
				halbb_tssi_cont_en(bb, true, RF_PATH_A);
			else
				halbb_tssi_cont_en(bb, true, RF_PATH_B);
		}
		/*BB reset set to 1*/
		halbb_bb_reset_en(bb, true, phy_idx);
	}
#ifdef CONFIG_FW_IO_OFLD_SUPPORT
	halbb_fwofld_cfgcr_end(bb);
#endif
}
void halbb_bb_reset_en(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_bb_reset_en_8852a_2(bb, en, phy_idx);
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_bb_reset_en_8852b(bb, en, phy_idx);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_bb_reset_en_8852c(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_bb_reset_en_8192xb(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_bb_reset_en_8851b(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_bb_reset_en_8922a(bb, en, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

enum rtw_hal_status
halbb_config_cmac_tbl(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i,
			void *cctrl,
			void *cctl_info_mask)
{
	enum rtw_hal_status ret = RTW_HAL_STATUS_FAILURE;

	switch (bb->bb_80211spec) {

	#ifdef HALBB_COMPILE_AX_SERIOUS
	case BB_AX_IC:
		ret = halbb_config_cmac_tbl_ax(bb, phl_sta_i, cctrl, cctl_info_mask);
		break;
	#endif

	#ifdef HALBB_COMPILE_BE_SERIES
	case BB_BE_IC:
		ret = halbb_config_cmac_tbl_be(bb, phl_sta_i, cctrl, cctl_info_mask);
		break;
	#endif
	default:
		break;
	}
	return ret;
}

void halbb_ctrl_rf_mode(struct bb_info *bb, enum phl_rf_mode mode)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_rf_mode_8852a_2(bb, mode);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_rf_mode_8852b(bb, mode);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_rf_mode_8852c(bb, mode);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_ctrl_rf_mode_8192xb(bb, mode);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_rf_mode_8851b(bb, mode);
		break;
	#endif

	default:
		break;
	}
}

void halbb_ctrl_rf_mode_rx_path(struct bb_info *bb, enum rf_path rx_path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_rf_mode_rx_path_8852b(bb, rx_path);
		break;
	#endif

	default:
		break;
	}
}
bool halbb_ctrl_tx_path_bb_afe_map(struct bb_info *bb, u8 mapping_idx)
{
	bool rpt = true;

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_cfgcr_start(bb);
	#endif
	switch (bb->ic_type) {

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_ctrl_tx_path_bb_afe_map_8922a(bb, mapping_idx);
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	return rpt;
}

bool halbb_ctrl_rx_path(struct bb_info *bb, enum rf_path rx_path,
			       enum phl_phy_idx phy_idx
)
{
	bool rpt = true;

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_start(bb);
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_ctrl_rx_path_8852a_2(bb, rx_path, bb->hal_com->dbcc_en);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_ctrl_rx_path_8852b(bb, rx_path);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ctrl_rx_path_8852c(bb, rx_path, bb->hal_com->dbcc_en);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_ctrl_rx_path_8192xb(bb, rx_path);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_ctrl_rx_path_8851b(bb, rx_path);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		#ifdef BB_8922A_DVLP_SPF
		rpt = halbb_ctrl_rx_path_8922a(bb, rx_path, phy_idx);
		#endif
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_end(bb);
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif
	return rpt;
}

bool halbb_ctrl_tx_path_pmac(struct bb_info *bb, enum rf_path tx_path,
			     enum phl_phy_idx phy_idx)
{
	bool rpt = true;

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_start(bb);
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_ctrl_tx_path_8852a_2(bb, tx_path);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_ctrl_tx_path_8852b(bb, tx_path);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ctrl_tx_path_8852c(bb, tx_path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_ctrl_tx_path_8192xb(bb, tx_path);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_ctrl_tx_path_8851b(bb, tx_path);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_ctrl_tx_path_pmac_8922a(bb, tx_path, phy_idx);
		break;
	#endif

	default:
		rpt = false;
		break;
	}
	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_cfgcr_end(bb);
	#endif

	return rpt;
}

bool halbb_ctrl_tx_path(struct bb_info *bb, enum rf_path tx_path,
			enum phl_phy_idx phy_idx)
{
	bool rpt = true;

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_cfgcr_start(bb);
	#endif
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_ctrl_tx_path_8852a_2(bb, tx_path);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_ctrl_tx_path_8852b(bb, tx_path);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ctrl_tx_path_8852c(bb, tx_path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_ctrl_tx_path_8192xb(bb, tx_path);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_ctrl_tx_path_8851b(bb, tx_path);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_ctrl_tx_path_8922a(bb, tx_path, phy_idx);
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_end(bb);
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif
	return rpt;
}

void halbb_ctrl_trx_path(struct bb_info *bb_0, enum rf_path tx_path, u8 tx_nss,
			 enum rf_path rx_path, u8 rx_nss)
{
	struct bb_info *bb = bb_0;
#ifdef HALBB_DBCC_SUPPORT
	enum phl_phy_idx phy_idx = HW_PHY_0;

	/*Only take 2SS-IC into consideration*/
	if (bb->hal_com->dbcc_en && !bb->bb_cmn_hooker->ic_dual_phy_support) {
		if (tx_path >= RF_PATH_AB || rx_path >= RF_PATH_AB ||
		    tx_nss != 1 || rx_nss != 1 ||
		    tx_path != rx_path ) {
			BB_WARNING("[%s]tx_path=%d, rx_path=%d\n", __func__, tx_path, rx_path);
			return;
		}
		phy_idx = (rx_path == RF_PATH_A) ? HW_PHY_0 : HW_PHY_1;
	}

	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
#endif

	BB_DBG(bb, DBG_DBCC, "[%s]tx_path=%d, tx_nss=%d, rx_path=%d, rx_nss=%d\n",
	       __func__, tx_path, tx_nss, rx_path, rx_nss);

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_start(bb);
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_trx_path_8852a_2(bb, tx_path, tx_nss, rx_path, rx_nss);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_trx_path_8852b(bb, tx_path, tx_nss, rx_path, rx_nss);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_trx_path_8852c(bb, tx_path, tx_nss, rx_path, rx_nss);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_ctrl_trx_path_8192xb(bb, tx_path, tx_nss, rx_path, rx_nss);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_trx_path_8851b(bb, tx_path, tx_nss, rx_path, rx_nss);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_ctrl_trx_path_8922a(bb, tx_path, tx_nss, rx_path, rx_nss);
		break;
	#endif

	default:
		break;
	}

#ifdef HALBB_RUA_SUPPORT
	halbb_trxpath_notif(bb, tx_path, rx_path);
#endif

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_end(bb);
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif
}

u16 halbb_fc_mapping(struct bb_info *bb, enum band_type band, u8 central_ch)
{
	u16 central_freq = 0;

	if (band == BAND_ON_6G) {
		central_freq = 5955 + (central_ch - 1) * 5;
	} else if (band == BAND_ON_5G) {
		central_freq = 5180 + (central_ch - 36) * 5;
	} else { // band == BAND_ON_2G
		if (central_ch == 14)
			central_freq = 2484;
		else
			central_freq = 2412 + (central_ch - 1) * 5;
	}

	return central_freq;
}

u8 halbb_get_prim_sb (struct bb_info *bb, u8 central_ch, u8 pri_ch, enum channel_width bw)
{
	u8 prim_sb = 0;
	u8 prisb_cal_ofst[5] = {0, 2, 6, 14, 30};

	prim_sb = (prisb_cal_ofst[bw] + pri_ch - central_ch) / 4;

	return prim_sb;
}

bool halbb_ctrl_bw(struct bb_info *bb, u8 pri_ch_idx, enum band_type band, enum channel_width bw,
		   enum phl_phy_idx phy_idx)
{
/*For MP Use Only*/
	bool rpt = true;
	struct bb_api_info *bb_api = &bb->bb_api_i;

	bb_api->pri_ch_idx = pri_ch_idx;
	bb_api->bw = bw;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_ctrl_bw_8852a_2(bb, pri_ch_idx, band, bw, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_ctrl_bw_8852b(bb, pri_ch_idx, bw, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_ctrl_bw_8852c(bb, pri_ch_idx, bw, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_ctrl_bw_8192xb(bb, pri_ch_idx, bw, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_ctrl_bw_8851b(bb, pri_ch_idx, bw, phy_idx);
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	return rpt;
}

bool halbb_ctrl_bw_ch(struct bb_info *bb_0, u8 pri_ch, u8 central_ch_seg0,
		      u8 central_ch_seg1, enum band_type band,
		      enum channel_width bw, enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	bool rpt = true;

	BB_DBG(bb, DBG_DBCC, "[%s] CH_pri/fc=%03d/%03d, bw=%dM, phy_idx=%d\n",
	       __func__, pri_ch, central_ch_seg0, 20 << bw, phy_idx);

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
#endif

	bb->bb_api_i.central_ch = central_ch_seg0;
	bb->bb_api_i.band = band;
	bb->bb_api_i.bw = bw;
	bb->bb_api_i.pri_ch_idx = pri_ch;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		#ifdef HALBB_FW_OFLD_SUPPORT
		if (halbb_check_fw_ofld(bb)) {
			BB_WARNING("Do FW offload at Channel switch\n");
			rpt = halbb_fwofld_bw_ch_8852a_2(bb, pri_ch, central_ch_seg0,
							 bw, phy_idx);
			BB_WARNING("Finish FW offload at Channel switch\n");
		}
		else
		#endif
			rpt = halbb_ctrl_bw_ch_8852a_2(bb, pri_ch, central_ch_seg0, bw,
						       band, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		#ifdef HALBB_FW_OFLD_SUPPORT
		if (halbb_check_fw_ofld(bb)) {
			rpt = halbb_fwofld_ctrl_bw_ch_8852b(bb, pri_ch, central_ch_seg0, bw,
										band, phy_idx);
		} else {
			rpt = halbb_ctrl_bw_ch_8852b(bb, pri_ch, central_ch_seg0, bw,
					     band, phy_idx);
		}
		#else
		rpt = halbb_ctrl_bw_ch_8852b(bb, pri_ch, central_ch_seg0, bw,
					     band, phy_idx);
		#endif
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		#ifdef HALBB_FW_OFLD_SUPPORT
		if (halbb_check_fw_ofld(bb)) {
			BB_WARNING("Do FW offload at Channel switch Start\n");
			rpt = halbb_fwofld_ctrl_bw_ch_8852c(bb, pri_ch,
							    central_ch_seg0,
							    bw, band, phy_idx);
			BB_WARNING("Do FW offload at Channel switch End\n");
		} else {
		rpt = halbb_ctrl_bw_ch_8852c(bb, pri_ch, central_ch_seg0, bw,
						     band, phy_idx);
		}
		#else
			rpt = halbb_ctrl_bw_ch_8852c(bb, pri_ch, central_ch_seg0, bw,
						     band, phy_idx);
		#endif
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_ctrl_bw_ch_8192xb(bb, pri_ch, central_ch_seg0, bw,
					     band, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_ctrl_bw_ch_8851b(bb, pri_ch, central_ch_seg0, bw,
					     band, phy_idx);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_ctrl_bw_ch_8922a(bb, pri_ch, central_ch_seg0, bw,
					     band, phy_idx);
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	#ifdef HALBB_FW_OFLD_SUPPORT
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif

	return rpt;
}

void halbb_ctrl_rx_cca(struct bb_info *bb, bool cca_en, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_rx_cca_8852a_2(bb, cca_en, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_rx_cca_8852b(bb, cca_en, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_rx_cca_8852c(bb, cca_en, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_ctrl_rx_cca_8192xb(bb, cca_en, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_rx_cca_8851b(bb, cca_en, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

bool halbb_query_cck_en(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	bool cck_en = false;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		cck_en = halbb_query_cck_en_8852a_2(bb, phy_idx, 3);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		//halbb_query_cck_en_8852b(bb, cck_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		cck_en = halbb_query_cck_en_8852c(bb, phy_idx, 3);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		//halbb_query_cck_en_8192xb(bb, cck_enable, phy_idx);
		break;
	#endif

	default:
		break;
	}
	return cck_en;
}

void halbb_ctrl_cck_en(struct bb_info *bb, bool cck_enable,
		       enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_cck_en_8852a_2(bb, cck_enable, phy_idx, 3);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_cck_en_8852b(bb, cck_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_cck_en_8852c(bb, cck_enable, phy_idx, 3);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_ctrl_cck_en_8192xb(bb, cck_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_cck_en_8851b(bb, cck_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_ctrl_cck_en_8922a(bb, cck_enable, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_ctrl_ofdm_en(struct bb_info *bb, bool ofdm_enable,
			enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_ofdm_en_8852a_2(bb, ofdm_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_ofdm_en_8852b(bb, ofdm_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_ofdm_en_8852c(bb, ofdm_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_ctrl_ofdm_en_8192xb(bb, ofdm_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_ofdm_en_8851b(bb, ofdm_enable, phy_idx);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_ctrl_ofdm_en_8922a(bb, ofdm_enable, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_ctrl_btg(struct bb_info *bb, bool btg)
{
	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_start(bb);
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_btg_8852a_2(bb, btg);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_btg_8852b(bb, btg);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_btg_8852c(bb, btg);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:

		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_btg_8851b(bb, btg);
		break;
	#endif

	default:
		break;
	}

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_end(bb);
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif
}

void halbb_ctrl_btc_preagc(struct bb_info *bb, bool bt_en)
{
	u8 band = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.band;

	bb->bt_en = bt_en; /*backup for ch_bw switch*/

	if (band != BAND_ON_24G)
		bt_en = false;

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_start(bb);
	halbb_fwofld_bitmap_en(bb, true, FW_OFLD_BB_API);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ctrl_btc_preagc_8852a_2(bb, bt_en);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ctrl_btc_preagc_8852b(bb, bt_en);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ctrl_btc_preagc_8852c(bb, bt_en);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:

		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ctrl_btc_preagc_8851b(bb, bt_en);
		break;
	#endif

	default:
		break;
	}

	#ifdef HALBB_FW_OFLD_SUPPORT
	//halbb_fwofld_cfgcr_end(bb);
	halbb_fwofld_bitmap_en(bb, false, FW_OFLD_BB_API);
	#endif
}

void halbb_clk_en(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_clk_en_8852c(bb, en, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_pwr_en(struct bb_info *bb, bool en)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_pwr_en_8852c(bb, en);
		break;
	#endif

	default:
		break;
	}
}

void halbb_pop_en(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_pop_en_8852a_2(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_pop_en_8852b(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_pop_en_8852c(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_pop_en_8192xb(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_pop_en_8851b(bb, en, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

bool halbb_querry_pop_en(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	bool rpt = true;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_querry_pop_en_8852a_2(bb, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_querry_pop_en_8852b(bb, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_querry_pop_en_8852c(bb, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_querry_pop_en_8192xb(bb, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_querry_pop_en_8851b(bb, phy_idx);
		break;
	#endif

	default:
		rpt = false;
		break;
	}

	return rpt;
}

bool halbb_set_pd_lower_bound(struct bb_info *bb, u8 bound,
			      enum channel_width bw, enum phl_phy_idx phy_idx)
{
	bool rpt = true;
	struct bb_api_info *bb_api = &bb->bb_api_i;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_set_pd_lower_bound_8852a_2(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_set_pd_lower_bound_8852b(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_set_pd_lower_bound_8852c(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_set_pd_lower_bound_8192xb(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_set_pd_lower_bound_8851b(bb, bound, bw, phy_idx);
		break;
	#endif
	default:
		rpt = false;
		break;
	}

	return rpt;
}

bool halbb_set_pd_lower_bound_cck(struct bb_info *bb, u8 bound,
			      enum channel_width bw, enum phl_phy_idx phy_idx)
{
	bool rpt = true;
	struct bb_api_info *bb_api = &bb->bb_api_i;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_set_pd_lower_bound_cck_8852a_2(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_set_pd_lower_bound_cck_8852b(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_set_pd_lower_bound_cck_8852c(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_set_pd_lower_bound_cck_8192xb(bb, bound, bw, phy_idx);
		break;
	#endif
	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_set_pd_lower_bound_cck_8851b(bb, bound, bw, phy_idx);
		break;
	#endif
	default:
		rpt = false;
		break;
	}

	return rpt;
}

u8 halbb_querry_pd_lower_bound(struct bb_info *bb, bool get_en_info,
			       enum phl_phy_idx phy_idx)
{
	u8 rpt = 0;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_querry_pd_lower_bound_8852a_2(bb, get_en_info, phy_idx);
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_querry_pd_lower_bound_8852b(bb, get_en_info, phy_idx);
		break;
	#endif
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_querry_pd_lower_bound_8852c(bb, get_en_info, phy_idx);
		break;
	#endif
	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_querry_pd_lower_bound_8192xb(bb, get_en_info, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_querry_pd_lower_bound_8851b(bb, get_en_info, phy_idx);
		break;
	#endif

	default:
		rpt = 0;
		break;
	}

	return rpt;
}

u8 halbb_get_losel(struct bb_info *bb)
{
	u8 rpt = 0xff;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		rpt = halbb_get_losel_8852a_2(bb);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_get_losel_8852c(bb);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rpt = halbb_get_losel_8192xb(bb);
		break;
	#endif

	default:
		rpt = 0xff;
		break;
	}

	return rpt;
}

bool halbb_lps_info(struct bb_info *bb, u16 mac_id)
{
	bool rpt = true;

	switch (bb->ic_type) {

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rpt = halbb_lps_info_8852b(bb, mac_id);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rpt = halbb_lps_info_8852c(bb, mac_id);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rpt = halbb_lps_info_8851b(bb, mac_id);
		break;
	#endif

	default:
		break;
	}

	return rpt;
}

void halbb_set_ant(struct bb_info *bb, u8 ant)
{
	switch (bb->ic_type) {

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_set_ant_8851b(bb, ant);
		break;
	#endif

	default:
		break;
	}
}

void halbb_syn_sel(struct bb_info *bb, enum rf_path path, bool val,
		   enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_syn_sel_8922a(bb, path, val, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

bool halbb_mlo_cfg(struct bb_info *bb, enum bb_mlo_mode_info mode)
{
	bool rpt = true;

	switch (bb->ic_type) {

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_mlo_cfg_8922a(bb, mode);
		break;
	#endif

	default:
		break;
	}

	return rpt;
}

#ifdef BB_8922A_DVLP_SPF
bool halbb_ctrl_mlo(struct bb_info *bb, enum mlo_dbcc_mode_type mode)
{
	bool rpt = true;

	switch (bb->ic_type) {

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		rpt = halbb_ctrl_mlo_8922a(bb, mode);
		break;
	#endif

	default:
		break;
	}

	return rpt;
}
#endif

void halbb_set_digital_pwr_comp(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_set_digital_pwr_comp_8852c(bb, en, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_set_digital_pwr_comp_8192xb(bb, en, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_set_igi(struct bb_info *bb, u8 lna_idx, bool tia_idx, u8 rxbb_idx,
		   enum rf_path path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_set_igi_8852a_2(bb, lna_idx, tia_idx, rxbb_idx, path);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_set_igi_8852b(bb, lna_idx, tia_idx, rxbb_idx, path);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_set_igi_8852c(bb, lna_idx, tia_idx, rxbb_idx, path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_set_igi_8192xb(bb, lna_idx, tia_idx, rxbb_idx, path);
		break;
	#endif


	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_set_igi_8851b(bb, lna_idx, tia_idx, rxbb_idx, path);
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_set_igi_8922a(bb, lna_idx, tia_idx, rxbb_idx, path);
		break;
	#endif

	default:
		break;
	}
}

void halbb_set_tx_pow_pattern_shap(struct bb_info *bb, u8 ch,
				   bool is_ofdm, enum phl_phy_idx phy_idx) {

	struct rtw_tpu_info *tpu = &bb->hal_com->band[phy_idx].rtw_tpu_i;
	u8 shape_idx = tpu->tx_ptrn_shap_idx;
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halbb_fwofld_cfgcr_start(bb);
#endif
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		if (is_ofdm)
			halbb_tx_triangular_shap_cfg_8852a_2(bb, shape_idx, phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		if (is_ofdm)
			halbb_tx_triangular_shap_cfg_8852b(bb, shape_idx, phy_idx);
		else
			halbb_tx_dfir_shap_cck_8852b(bb, ch, shape_idx, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		if (is_ofdm)
			halbb_tx_triangular_shap_cfg_8852c(bb, shape_idx, phy_idx);
		else
			halbb_tx_dfir_shap_cck_8852c(bb, ch, shape_idx, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		if (is_ofdm)
			halbb_tx_triangular_shap_cfg_8192xb(bb, shape_idx, phy_idx);
		else
			halbb_tx_dfir_shap_cck_8192xb(bb, ch, shape_idx, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		if (is_ofdm)
			halbb_tx_triangular_shap_cfg_8851b(bb, shape_idx, phy_idx);
		else
			halbb_tx_dfir_shap_cck_8851b(bb, ch, shape_idx, phy_idx);
		break;
	#endif

	default:
		break;
	}
#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
	halbb_fwofld_cfgcr_end(bb);
#endif
}

void halbb_set_tx_pow_per_path_lmt(struct bb_info *bb, s16 pwr_lmt_a, s16 pwr_lmt_b)
{
	switch (bb->ic_type) {
	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_set_tx_pow_per_path_lmt_8852c(bb, pwr_lmt_a, pwr_lmt_b);
		break;
	#endif

	default:
		break;
	}
}

u32 halbb_set_tx_pow_chk_cw_boundary(struct bb_info *bb, s16 pw_s10_3)
{
	s16 rf_pw_cw = 0;
	u32 pw_cw = pw_s10_3;

	rf_pw_cw = (pw_s10_3 & 0x1F8) >> 3;

	if (rf_pw_cw > 63) {
		pw_cw = (63 << 3) | (pw_s10_3 & 0x7); /*upper bound (+24dBm)*/
		BB_WARNING("[%s] rf_pw_cw=%d > 63\n", __func__, rf_pw_cw);
	} else if (rf_pw_cw < 15) {
		pw_cw = (15 << 3) | (pw_s10_3 & 0x7); /*lower bound (-24dBm)*/
		BB_WARNING("[%s] rf_pw_cw=%d < 15\n", __func__, rf_pw_cw);
	}

	return pw_cw;
}

void halbb_set_tx_pow_ref_and_cw(struct bb_info *bb, s16 pw_dbm_ofdm, /*s(9,2)*/
				  s16 pw_dbm_cck, s8 ofst_path_b,/*s(8,3)*/
				  u8 base_cw_0db, u16 tssi_16dBm_cw,
				  u16 *ofdm_cw, u16 *cck_cw,
				  enum phl_phy_idx phy_idx)
{
	u32 pw_cw = 0, pw_cw_b = 0;
	u32 val_ofdm = 0, val_cck = 0;
	u32 val_ofdm_b = 0, val_cck_b = 0;
	s16 pw_s10_3 = 0;
	u32 tssi_ofst_cw = 0, tssi_ofst_cw_b = 0;
	bool cfg_path_a = true, cfg_path_b = true;

	BB_DBG(bb, DBG_DBG_API,
	       "pw_dbm_ofdm=%d, pw_dbm_cck=%d, ofst_path_b=%d, base_cw_0db=%d, tssi_16dBm_cw=%d\n",
		pw_dbm_ofdm, pw_dbm_cck, ofst_path_b, base_cw_0db, tssi_16dBm_cw);

	/*==== [OFDM] =======================================================*/
	pw_s10_3 = (pw_dbm_ofdm * 2) + (s16)(base_cw_0db * 8);

	pw_cw = halbb_set_tx_pow_chk_cw_boundary(bb, pw_s10_3);
	pw_cw_b = halbb_set_tx_pow_chk_cw_boundary(bb, pw_s10_3 + ofst_path_b);

	/* ===[Set TSSI Offset]===*/
	/*
	172 = 300 -  (55 - 39) * 8;
	tssi_ofst_cw = tssi_16dBm_cw -  (tx_pow_16dBm_ref_cw - tx_pow_ref_cw) * 8;
	             = tssi_16dBm_cw + tx_pow_ref * 8  - tx_pow_16dBm_ref * 8
	*/
	tssi_ofst_cw = (u32)((s16)tssi_16dBm_cw + (pw_dbm_ofdm * 2) - (16 * 8));
	tssi_ofst_cw_b = tssi_ofst_cw + ofst_path_b;

	BB_DBG(bb, DBG_DBG_API, "[OFDM][A]tssi_ofst_cw=%d, rf_cw=0x%x, bb_cw=0x%x\n", tssi_ofst_cw, pw_cw >> 3, pw_cw & 0x7);
	BB_DBG(bb, DBG_DBG_API, "[OFDM][B]tssi_ofst_cw=%d, rf_cw=0x%x, bb_cw=0x%x\n", tssi_ofst_cw_b, pw_cw_b >> 3, pw_cw_b & 0x7);


	*ofdm_cw = (u16)pw_cw;
	val_ofdm = tssi_ofst_cw << 18 | pw_cw << 9 | (u32)(pw_dbm_ofdm & 0x1ff);
	val_ofdm_b = tssi_ofst_cw_b << 18 | pw_cw_b << 9 | (u32)(pw_dbm_ofdm & 0x1ff);

	/*==== [CCK] =========================================================*/
	pw_s10_3 = (pw_dbm_cck * 2) + (s16)(base_cw_0db * 8);

	pw_cw = halbb_set_tx_pow_chk_cw_boundary(bb, pw_s10_3);
	pw_cw_b = halbb_set_tx_pow_chk_cw_boundary(bb, pw_s10_3 + ofst_path_b);

	/* ===[Set TSSI Offset]===*/
	/*
	172 = 300 -  (55 - 39) * 8;
	tssi_ofst_cw = tssi_16dBm_cw -  (tx_pow_16dBm_ref_cw - tx_pow_ref_cw) * 8;
	             = tssi_16dBm_cw + tx_pow_ref * 8  - tx_pow_16dBm_ref * 8
	*/
	tssi_ofst_cw = (u32)((s16)tssi_16dBm_cw + (pw_dbm_cck * 2) - (16 * 8));
	tssi_ofst_cw_b = tssi_ofst_cw + ofst_path_b;

	BB_DBG(bb, DBG_DBG_API, "[CCK][A] tssi_ofst_cw=%d, rf_cw=0x%x, bb_cw=0x%x\n", tssi_ofst_cw, pw_cw >> 3, pw_cw & 0x7);
	BB_DBG(bb, DBG_DBG_API, "[CCK][B] tssi_ofst_cw=%d, rf_cw=0x%x, bb_cw=0x%x\n", tssi_ofst_cw_b, pw_cw_b >> 3, pw_cw_b & 0x7);

	*cck_cw = (u16)pw_cw;
	val_cck = tssi_ofst_cw << 18 | pw_cw << 9 | (u32)(pw_dbm_cck & 0x1ff);
	val_cck_b = tssi_ofst_cw_b << 18 | pw_cw_b << 9 | (u32)(pw_dbm_cck & 0x1ff);

	/*==== [SET BB CR] ===================================================*/
#ifdef HALBB_DBCC_SUPPORT
	if (bb->hal_com->dbcc_en) {
		if (phy_idx == HW_PHY_0)
			cfg_path_b = false; /*path-A only for phy-0, no need to set path-B*/
		else if (phy_idx == HW_PHY_1)
			cfg_path_a = false; /*path-B only for phy-1, no need to set path-A*/
	}
#endif
	/*path-A*/
	if (cfg_path_a) {
		halbb_set_reg(bb, 0x5804, 0x7FFFFFF, val_ofdm);
		halbb_set_reg(bb, 0x5808, 0x7FFFFFF, val_cck);
	}
	/*path-B*/
	if (cfg_path_b) {
		halbb_set_reg(bb, 0x7804, 0x7FFFFFF, val_ofdm_b);
		halbb_set_reg(bb, 0x7808, 0x7FFFFFF, val_cck_b);
	}
}

void halbb_set_tx_pow_ref(struct bb_info *bb, enum phl_phy_idx phy_idx)
{
	struct rtw_tpu_info *tpu = &bb->hal_com->band[phy_idx].rtw_tpu_i;

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:

		rtw_hal_mac_write_pwr_ref_reg(bb->hal_com, (enum phl_band_idx)phy_idx);

		//halbb_set_tx_pow_ref_8852a_2
		halbb_set_tx_pow_ref_and_cw(bb, tpu->ref_pow_ofdm,
					     tpu->ref_pow_cck,
					     tpu->ofst_int,
					     tpu->base_cw_0db,
					     tpu->tssi_16dBm_cw,
					     &tpu->ref_pow_ofdm_cw,
					     &tpu->ref_pow_cck_cw,
					     phy_idx);
		break;
	#endif

	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		rtw_hal_mac_write_pwr_ref_reg(bb->hal_com, (enum phl_band_idx)phy_idx);
		//halbb_set_tx_pow_ref_8852b
		halbb_set_tx_pow_ref_and_cw(bb, tpu->ref_pow_ofdm,
					     tpu->ref_pow_cck,
					     tpu->ofst_int,
					     tpu->base_cw_0db,
					     tpu->tssi_16dBm_cw,
					     &tpu->ref_pow_ofdm_cw,
					     &tpu->ref_pow_cck_cw,
					     phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		rtw_hal_mac_write_pwr_ref_reg(bb->hal_com, (enum phl_band_idx)phy_idx);

		//halbb_set_tx_pow_ref_8852c
		halbb_set_tx_pow_ref_and_cw(bb, tpu->ref_pow_ofdm,
					   tpu->ref_pow_cck,
					   tpu->ofst_int,
					   tpu->base_cw_0db,
					   tpu->tssi_16dBm_cw,
					   &tpu->ref_pow_ofdm_cw,
					   &tpu->ref_pow_cck_cw,
					   phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		rtw_hal_mac_write_pwr_ref_reg(bb->hal_com, (enum phl_band_idx)phy_idx);

		halbb_set_tx_pow_ref_8192xb(bb, tpu->ref_pow_ofdm,
					    tpu->ref_pow_cck,
					    tpu->ofst_int,
					    tpu->base_cw_0db,
					    tpu->tssi_16dBm_cw,
					    &tpu->ref_pow_ofdm_cw,
					    &tpu->ref_pow_cck_cw,
					    phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		rtw_hal_mac_write_pwr_ref_reg(bb->hal_com, (enum phl_band_idx)phy_idx);

		halbb_set_tx_pow_ref_8851b(bb, tpu->ref_pow_ofdm,
					     tpu->ref_pow_cck,
					     tpu->ofst_int,
					     tpu->base_cw_0db,
					     tpu->tssi_16dBm_cw,
					     &tpu->ref_pow_ofdm_cw,
					     &tpu->ref_pow_cck_cw,
					     phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_normal_efuse_verify(struct bb_info *bb, s8 rx_gain_offset,
			       enum rf_path rx_path, enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_normal_efuse_verify_8852a_2(bb, rx_gain_offset, rx_path, phy_idx);
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_normal_efuse_verify_8852b(bb, rx_gain_offset, rx_path, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_normal_efuse_verify_8852c(bb, rx_gain_offset, rx_path, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_normal_efuse_verify_8192xb(bb, rx_gain_offset, rx_path, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_normal_efuse_verify_8851b(bb, rx_gain_offset, rx_path, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_normal_efuse_verify_cck(struct bb_info *bb, s8 rx_gain_offset,
				   enum rf_path rx_path,
				   enum phl_phy_idx phy_idx)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_normal_efuse_verify_cck_8852a_2(bb, rx_gain_offset,
						      rx_path, phy_idx);
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_normal_efuse_verify_cck_8852b(bb, rx_gain_offset,
						      rx_path, phy_idx);
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_normal_efuse_verify_cck_8852c(bb, rx_gain_offset,
						      rx_path, phy_idx);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_normal_efuse_verify_cck_8192xb(bb, rx_gain_offset,
						      rx_path, phy_idx);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_normal_efuse_verify_cck_8851b(bb, rx_gain_offset,
						      rx_path, phy_idx);
		break;
	#endif

	default:
		break;
	}
}

void halbb_rx_gain_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_show_gain_err_8852c(bb, input, _used, output, _out_len);
		break;
	#endif

	default:
		break;
	}
}

void halbb_rx_op1db_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_rx_op1db_dbg_8852c(bb, input, _used, output, _out_len);
		break;
	#endif

	default:
		break;
	}
}

void halbb_agc_fix_gain(struct bb_info *bb, bool enable, u8 lna_idx, u8 tia_idx,
			u8 rxbb_idx, enum rf_path path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_agc_fix_gain_8852c(bb, enable, lna_idx, tia_idx, rxbb_idx,
					 path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		break;
	#endif

	#ifdef BB_8922A_SUPPORT
	case BB_RTL8922A:
		halbb_agc_fix_gain_8922a(bb, enable, lna_idx, tia_idx, rxbb_idx,
					 path);
		break;
	#endif

	default:
		break;
	}
}

void halbb_agc_tia_shrink(struct bb_info *bb, bool shrink_en, bool shrink_init,
			  enum rf_path path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_agc_tia_shrink_8852c(bb, shrink_en, shrink_init, path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		break;
	#endif

	default:
		break;
	}
}

void halbb_agc_step_en(struct bb_info *bb, bool pre_pd_agc_en,
		       bool linear_agc_en, bool post_pd_agc_en,
		       bool nlgc_agc_en, enum rf_path path)
{
	switch (bb->ic_type) {

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_agc_step_en_8852c(bb, pre_pd_agc_en, linear_agc_en,
					post_pd_agc_en, nlgc_agc_en, path);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		break;
	#endif

	default:
		break;
	}
}

void halbb_ic_hw_setting_non_io(struct bb_info *bb)
{
#ifdef HALBB_DYN_1R_CCA_SUPPORT
	halbb_dyn_1r_cca_rst(bb);
#endif
}

void halbb_ic_hw_setting_low_io(struct bb_info *bb)
{
#ifdef HALBB_DYN_1R_CCA_SUPPORT
	halbb_dyn_1r_cca_rst(bb);
#endif
}

void halbb_ic_hw_setting_dbcc(struct bb_info *bb)
{
	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		if (halbb_get_reg(bb, 0x700, BIT(24)) == 1) {
			BB_DBG(bb, DBG_DBCC, "[%s] Set 0x700[24] = 0\n", __func__);
			halbb_set_reg(bb, 0x700, BIT(24), 0);
		}
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		if (halbb_get_reg(bb, 0x700, BIT(24)) == 1) {
			BB_DBG(bb, DBG_DBCC, "[%s] Set 0x700[24] = 0\n", __func__);
			halbb_set_reg(bb, 0x700, BIT(24), 0);
		}
		break;
	#endif

	default:
		break;
	}
}

void halbb_ic_hw_setting(struct bb_info *bb)
{

	#ifdef HALBB_DYN_1R_CCA_SUPPORT
	halbb_dyn_1r_cca(bb);
	#endif

	switch (bb->ic_type) {

	#ifdef BB_8852A_2_SUPPORT
	case BB_RTL8852A:
		halbb_ic_hw_setting_8852a_2(bb);
		halbb_dyn_mu_bypass_vht_sigb_8852a_2(bb);
		#ifdef HALBB_DYN_CSI_RSP_SUPPORT
		halbb_dyn_csi_rsp_main(bb);
		#endif
		#ifdef BB_DYN_CFO_TRK_LOP
		halbb_dyn_cfo_trk_loop(bb);
		#endif
		break;
	#endif
	#ifdef BB_8852B_SUPPORT
	case BB_RTL8852B:
		halbb_ic_hw_setting_8852b(bb);
		halbb_dyn_mu_bypass_vht_sigb_8852b(bb);
		#ifdef HALBB_DYN_CSI_RSP_SUPPORT
		halbb_dyn_csi_rsp_main(bb);
		#endif
		#ifdef BB_DYN_CFO_TRK_LOP
		halbb_dyn_cfo_trk_loop(bb);
		#endif
		#ifdef HALBB_DYN_DTR_SUPPORT
		halbb_dyn_dtr_watchdog(bb);
		#endif
		break;
	#endif

	#ifdef BB_8852C_SUPPORT
	case BB_RTL8852C:
		halbb_ic_hw_setting_8852c(bb);
		break;
	#endif

	#ifdef BB_8192XB_SUPPORT
	case BB_RTL8192XB:
		halbb_ic_hw_setting_8192xb(bb);
		break;
	#endif

	#ifdef BB_8851B_SUPPORT
	case BB_RTL8851B:
		halbb_ic_hw_setting_8851b(bb);
		break;
	#endif

	default:
		break;
	}
}

void halbb_ic_hw_setting_dbg(struct bb_info *bb, char input[][16],
			     u32 *_used, char *output, u32 *_out_len)
{
	u32 val[5] = {0};
	u8 i = 0, j = 0;
#ifdef BB_DYN_CFO_TRK_LOP
	struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
	struct bb_dyn_cfo_trk_lop_info *dctl = &cfo_trk->bb_dyn_cfo_trk_lop_i;
#endif

	if (_os_strcmp(input[1], "-h") == 0) {
#ifdef BB_DYN_CFO_TRK_LOP
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "cfo_trk en {0/1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "cfo_trk force {0:SNR, 1:link}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "cfo_trk para {0:SNR, 1:link} {data_val} {pilot_val}\n");
#endif
#ifdef BB_DYN_DTR
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			"dyn_dtr en {0/1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			"dyn_dtr rssi_th {val}\n");
#endif
		return;
	}

#ifdef BB_DYN_CFO_TRK_LOP
	if (_os_strcmp(input[1], "cfo_trk") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);

		if (_os_strcmp(input[2], "en") == 0) {
			halbb_dyn_cfo_trk_loop_en(bb, (bool)val[0]);
		} else if (_os_strcmp(input[2], "force") == 0) {
			halbb_dyn_cfo_trk_loop_en(bb, false);

			if (val[0] == 0)
				halbb_cfo_trk_loop_cr_cfg(bb, DCTL_SNR);
			else if (val[0] == 1)
				halbb_cfo_trk_loop_cr_cfg(bb, DCTL_LINK);
			else
				return;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 	    "Force state=%d\n", val[0]);
		} else if (_os_strcmp(input[2], "para") == 0) {
			HALBB_SCAN(input[4], DCMD_HEX, &val[1]);
			HALBB_SCAN(input[5], DCMD_HEX, &val[2]);

			if (val[0] >= 2)
				return;

			dctl->bb_cfo_trk_lop_cr_i[val[0]].dctl_data = (u8)val[1];
			dctl->bb_cfo_trk_lop_cr_i[val[0]].dctl_pilot = (u8)val[2];

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 	    "state:%d, dctl_data=0x%x, dctl_pilot=0x%x\n", val[0],
			 	    dctl->bb_cfo_trk_lop_cr_i[val[0]].dctl_data,
			 	    dctl->bb_cfo_trk_lop_cr_i[val[0]].dctl_pilot);

		} else if (_os_strcmp(input[2], "snr_th") == 0) {
			HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);

			dctl->dctl_snr_th_l = (u16)val[0] << RSSI_MA_H;
			dctl->dctl_snr_th_h = (u16)val[1] << RSSI_MA_H;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 	    "dctl_snr_th_{l/h}={%d/%d}\n",
			 	    val[0], val[1]);
		}
	} else
#endif
#ifdef BB_DYN_1R_CCA
	if (_os_strcmp(input[1], "1r_cca") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
		if (_os_strcmp(input[2], "en") == 0) {
			halbb_dyn_1r_cca_en_8852a_2(bb, (bool)val[0]);
		} else if (_os_strcmp(input[2], "force") == 0) {
			halbb_dyn_1r_cca_en_8852a_2(bb, false);

			if (val[0] == 1)
				cca_path = RF_PATH_A;
			else if (val[0] == 2)
				cca_path = RF_PATH_B;
			else if (val[0] == 3)
				cca_path = RF_PATH_AB;
			else
				return;
			halbb_dyn_1r_cca_cfg_8852a_2(bb, cca_path);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Force cca_path=%d\n", cca_path);
		} else if (_os_strcmp(input[2], "diff_th") == 0) {
			bb->bb_8852a_2_i.dyn_1r_cca_rssi_diff_th= (u16)(val[0] << 5);
		} else if (_os_strcmp(input[2], "min_th") == 0) {
			bb->bb_8852a_2_i.dyn_1r_cca_rssi_min_th= (u16)(val[0] << 5);
		}

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Dyn 1R CCA]en=%d, diff_th=%d, min_th=%d\n",
			    bb->bb_8852a_2_i.dyn_1r_cca_en,
			    bb->bb_8852a_2_i.dyn_1r_cca_rssi_diff_th >> 5,
			    bb->bb_8852a_2_i.dyn_1r_cca_rssi_min_th >> 5);
	} else
#endif

#ifdef BB_DYN_DTR
	if (_os_strcmp(input[1], "dyn_dtr") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
		if (_os_strcmp(input[2], "en") == 0) {
			halbb_dyn_dtr_en(bb, (bool)val[0]);
			if (val[0] == 1)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
									"DYN DTR Enable\n");
			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					"DYN DTR Disable\n");
		} else if (_os_strcmp(input[2], "rssi_th") == 0) {
				bb->bb_dyn_dtr_i.dyn_dtr_rssi_th = (u8)(val[0]);
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Dyn DTR]en=%d, rssi_th=%d\n",
			    bb->bb_dyn_dtr_i.dyn_dtr_en,
			    bb->bb_dyn_dtr_i.dyn_dtr_rssi_th);
		}
	} else
#endif
	{
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			"Set Err\n");
	}
}

void halbb_ic_api_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 tmp = 0, i = 0;
	u32 j = 0;
	#ifdef BB_8852B_SUPPORT
	bool judge_f;
	#endif

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "trx_path {tx_path} {tx_nss} {rx_path} {rx_nss}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "igi ({lna_idx (0~6)} {tia_idx (0~1)} {rxbb_idx (0~31)} {path (0~1)})\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Rx_setting({patch_idx (0:default, 1:patch-1, 2:patch-2)})\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "rfe dump\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "rfe all {rfe_idx}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "rfe cmn {gpio_idx} {path} {inv} {src}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "rfe rfm {path} {src} {dis_tx} {ac_tx} {ac_bt} {val}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "rfe trsw {path} {path_en} {trsw_tx} {trsw_rx} {trsw} {trsw_b}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "rf sw_si rw {run_idx} {rx_path} {reg_addr}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "gain_show {amode / gmode} path\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "rx_htmcs0 {enable} {phy_idx}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "op1db_show {amode / gmode} path\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "agc [default | fix_gain | tia_shrink | step_en | -h] ...\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "pwr_comp_en {en} {phy_idx}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "pwr_ctrl {-h} / {...}\n");
	} else if (_os_strcmp(input[1], "trx_path") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[3]);
		halbb_ctrl_trx_path(bb, (enum rf_path)val[0], (u8)val[1],
				    (enum rf_path)val[2], (u8)val[3]);

	} else if (_os_strcmp(input[1], "igi") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[3]);

		if (val[0] > 6) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "Invalid LNA index!\n");
		} else if (val[1] > 1) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "Invalid TIA index!\n");
		} else if (val[2] > 31) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "Invalid RxBB index!\n");
		} else {
			halbb_set_igi(bb, (u8)val[0], (bool)val[1], (u8)val[2],
				      (enum rf_path)val[3]);
		}
	} else if (_os_strcmp(input[1], "rx_setting") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		if (val[0] > 2) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "Invalid Patch index!\n");
			return;
		}
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "Rx setting Patch-%d Success!\n", val[0]);
	}  else if (_os_strcmp(input[1], "rfe") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[6], DCMD_DECIMAL, &val[3]);
		HALBB_SCAN(input[7], DCMD_DECIMAL, &val[4]);
		HALBB_SCAN(input[8], DCMD_DECIMAL, &val[5]);
		if (_os_strcmp(input[2], "dump") == 0) {
			halbb_gpio_ctrl_dump(bb);
		} else if (_os_strcmp(input[2], "all") == 0) {
			//HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			halbb_gpio_setting_all(bb, (u8)val[0]);
		} else if (_os_strcmp(input[2], "trsw") == 0) {
			halbb_gpio_trsw_table(bb, (enum bb_path)val[0],
					     (bool)val[1], (bool)val[2],
					     (bool)val[3], (bool)val[4],
					     (bool)val[5]);
		} else if (_os_strcmp(input[2], "rfm") == 0) {
			halbb_gpio_rfm(bb, (enum bb_path)val[0],
				       (enum bb_rfe_src_sel)val[1], (bool)val[2],
				       (bool)val[3], (bool)val[4], (u8)val[5]);
		} else if (_os_strcmp(input[2], "cmn") == 0) {
			halbb_gpio_setting(bb, (u8)val[0], (enum bb_path)val[1],
					   (bool)val[2],
					   (enum bb_rfe_src_sel)val[3]);
		}
	}  else if (_os_strcmp(input[1], "agc") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[6], DCMD_DECIMAL, &val[3]);
		HALBB_SCAN(input[7], DCMD_DECIMAL, &val[4]);
		if (_os_strcmp(input[2], "default") == 0) {
			// Default Auto gain
			halbb_agc_fix_gain(bb, 0, 6, 1, 31, RF_PATH_A);
			halbb_agc_fix_gain(bb, 0, 6, 1, 31, RF_PATH_B);
			// Default TIA Shrink
			halbb_agc_tia_shrink(bb, true, false, RF_PATH_A);
			halbb_agc_tia_shrink(bb, true, false, RF_PATH_B);
			// Default AGC Step setting
			halbb_agc_step_en(bb, true, true, true, true, RF_PATH_A);
			halbb_agc_step_en(bb, true, true, true, true, RF_PATH_B);
		} else if (_os_strcmp(input[2], "fix_gain") == 0) {
			halbb_agc_fix_gain(bb, (bool)val[0], (u8)val[1],
					   (u8)val[2], (u8)val[3],
					   (enum rf_path)val[4]);
		} else if (_os_strcmp(input[2], "tia_shrink") == 0) {
			halbb_agc_tia_shrink(bb, (bool)val[0], (bool)val[1],
					     (enum rf_path)val[2]);
		} else if (_os_strcmp(input[2], "step_en") == 0) {
			halbb_agc_step_en(bb, (bool)val[0], (bool)val[1],
					  (bool)val[2], (bool)val[3],
					  (enum rf_path)val[4]);
		} else if (_os_strcmp(input[2], "-h") == 0) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "default\n");
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "fix_gain {en} {lna} {tia} {rxbb} {path: 0/1}\n");
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "tia_shrink {en} {init} {path: 0/1}\n");
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "step_en {pre_pd_agc_en} {linear_agc_en} {post_pd_agc_en} {nlgc_agc_en} {path: 0/1}\n");
		}
	}  else if (_os_strcmp(input[1], "gain_show") == 0) {
		halbb_rx_gain_dbg(bb, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], "op1db_show") == 0) {
		halbb_rx_op1db_dbg(bb, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], "dbg") == 0) {
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[5], DCMD_DECIMAL, &val[2]);
		#ifdef BB_8852A_2_SUPPORT
		if (_os_strcmp(input[2], "pop_en") == 0) {
			halbb_pop_en(bb, (bool)val[0],
					     (enum phl_phy_idx)val[1]);
		} else if (_os_strcmp(input[2], "set_pd_low") == 0) {
			halbb_set_pd_lower_bound(bb, (u8)val[0],
						 (enum channel_width)val[1],
						 (enum phl_phy_idx)val[2]);
		} else if (_os_strcmp(input[2], "per") == 0) {
			halbb_get_per_8852a_2(bb, (enum phl_phy_idx)val[0]);
		}
		#endif
	} else if (_os_strcmp(input[1], "pwr_comp_en") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		halbb_set_digital_pwr_comp(bb, (bool)val[0], (enum phl_phy_idx)val[1]);
	} else if (_os_strcmp(input[1], "gain_ofst") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);
		halbb_normal_efuse_verify(bb, (s8)val[0], (enum rf_path)val[1], (enum phl_phy_idx)val[2]);
	#ifdef BB_8852C_SUPPORT
	} else if (_os_strcmp(input[1], "rx_htmcs0") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		halbb_fix_rx_htmcs0_8852c(bb, (bool)val[0], (enum phl_phy_idx)val[1]);
	#endif
	#ifdef BB_8852B_SUPPORT
	} else if (_os_strcmp(input[1], "rf_sw_si_rw") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);
		HALBB_SCAN(input[5], DCMD_HEX, &val[3]);

		if (val[0] > 2) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
					"[SW_SI] Do not run test!\n");
			return;
		} else if (val[1] > 10000) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
					"[SW_SI] Out-of test range!\n");
			return;
		} else if (val[2] > 3) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
					"[SW_SI] Wrong path setting!\n");
			return;
		} else if (val[3] > 0xff) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
					"[SW_SI] Wrong addr setting!\n");
			return;
		} else {

			while (j < val[1]) {
				judge_f = halbb_rf_sw_si_test(bb, (enum rf_path)val[2], (u8)val[3], j);

				if (!judge_f){
					BB_WARNING("[%s]  while ocunter = %d\n", __func__, j);
					break;
				} else {
					BB_WARNING("[%s]  while ocunter = %d\n", __func__, j);
				}

				j++;
			}

		}
	#endif
	} else if (_os_strcmp(input[1], "pwr_ctrl") == 0) {
		if (_os_strcmp(input[2], "-h") == 0) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "bb_fifo_en {enable} {phy_idx}\n");
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "bb_clk_en {enable} {phy_idx}\n");
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "bb_pwr_en {enable}\n");
		} else if (_os_strcmp(input[2], "bb_fifo_en") == 0){
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
			halbb_adc_ctrl_en(bb, (bool)val[0], (enum phl_phy_idx)val[1]);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "fifo_en=%d, phy_idx=%d\n", val[0], val[1]);
		} else if (_os_strcmp(input[2], "bb_clk_en") == 0) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
			halbb_clk_en(bb, (bool)val[0], (enum phl_phy_idx)val[1]);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "clk_en=%d, phy_idx=%d\n", val[0], val[1]);
		} else if (_os_strcmp(input[2], "bb_pwr_en") == 0) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			halbb_pwr_en(bb, (bool)val[0]);
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "pwr_en=%d\n", val[0]);
		}
	}

#if 0
	else if (_os_strcmp(input[1], "sc_idx") == 0) {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		for (i = 0; i <= 15; i++) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "rpt(0, %d) = 0x%x\n", i,
				    halbb_get_csi_buf_idx(bb, (u8)val[0], i));
		}
	}
#endif

	*_used = used;
	*_out_len = out_len;
}

#ifdef HALBB_DIG_MCC_SUPPORT
u8 halbb_upd_mcc_macid(struct bb_info *bb, struct bb_mcc_i *mi)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	u8 i = 0, role_ch = 0, band_idx = MCC_BAND_NUM;

	BB_DBG(bb, DBG_DIG, "<====== %s ======>\n", __func__);

	role_ch = mi->chandef->center_ch;

	for (i = 0; i < MCC_BAND_NUM; i++) {
		if (mcc_dm->mcc_rf_ch[i].center_ch == role_ch) {
			band_idx = i;
			break;
		}
	}

	if (band_idx == MCC_BAND_NUM) {
		BB_WARNING("%s, band_idx = %d", __func__, band_idx);
		return HALBB_SET_FAIL;
	}

	if (mi->type == PHL_RTYPE_AP || mi->type == PHL_RTYPE_P2P_GO) {
		mcc_dm->softap_macid = mi->self_macid;
		BB_DBG(bb, DBG_DIG, "SoftAP macid = %d\n",
		       mcc_dm->softap_macid);
	}

	return HALBB_SET_SUCCESS;
}


void halbb_mcc_stop(struct bb_info *bb)
{
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;
	u8 i = 0, j = 0;

	BB_DBG(bb, DBG_DIG, "<====== %s ======>\n", __func__);

	if (mcc_dm->mcc_status_en == false)
		return;

	mcc_dm->mcc_status_en = false;

	for (i = 0; i < MCC_BAND_NUM; i++) {
		mcc_dm->sta_cnt[i] = 0;
		mcc_dm->mcc_rf_ch[i].chan = INVALID_INIT_VAL;
		mcc_dm->mcc_rf_ch[i].center_ch = INVALID_INIT_VAL;
	}
}

u8 halbb_mcc_start(struct bb_info *bb, struct bb_mcc_i *mi_1,
		   struct bb_mcc_i *mi_2)
{
	u8 ret = HALBB_SET_FAIL;
	struct halbb_mcc_dm *mcc_dm = &bb->mcc_dm;

	BB_DBG(bb, DBG_DIG, "<====== %s ======>\n", __func__);

	halbb_mem_cpy(bb, &mcc_dm->mcc_rf_ch[MCC_BAND_1], mi_1->chandef,
		      sizeof(struct rtw_chan_def));

	halbb_mem_cpy(bb, &mcc_dm->mcc_rf_ch[MCC_BAND_2], mi_2->chandef,
		      sizeof(struct rtw_chan_def));

	ret = halbb_upd_mcc_macid(bb, mi_1);
	if (ret != HALBB_SET_SUCCESS)
		goto exit;

	ret = halbb_upd_mcc_macid(bb, mi_2);
	if (ret != HALBB_SET_SUCCESS)
		goto exit;

	mcc_dm->mcc_status_en = true;
	ret = HALBB_SET_SUCCESS;
exit:
	return ret;

}
#endif

#ifdef HALBB_PATH_DIV_SUPPORT
u8 halbb_ctrl_tx_path_div(struct bb_info *bb, enum bb_path tx_path_1ss)
{
	struct bb_pathdiv_info *bb_path_div = &bb->bb_path_div_i;

	BB_DBG(bb, DBG_DBG_API, "<====== %s ======>\n", __func__);

	switch (tx_path_1ss) {
		case BB_PATH_A:
			bb_path_div->path_sel_1ss= BB_PATH_A;
			break;

		case BB_PATH_B:
			bb_path_div->path_sel_1ss= BB_PATH_B;
			break;

		case BB_PATH_C:
			bb_path_div->path_sel_1ss= BB_PATH_C;
			break;

		case BB_PATH_D:
			bb_path_div->path_sel_1ss= BB_PATH_D;
			break;

		case BB_PATH_AUTO:
			bb_path_div->path_sel_1ss= BB_PATH_AUTO;
			break;

		default:
			bb_path_div->path_sel_1ss = BB_PATH_NON;
			bb->support_ability &= ~BB_PATH_DIV;
			break;
	}

	BB_DBG(bb, DBG_DBG_API, "Tx path 1ss = 0x%x\n", bb_path_div->path_sel_1ss);

	return HALBB_SET_SUCCESS;
}

#endif

