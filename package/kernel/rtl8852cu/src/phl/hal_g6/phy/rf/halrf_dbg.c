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

#include "halrf_precomp.h"

void halrf_dbg_setting_init(struct rf_info *rf)
{
	rf->fw_dbg_component = 0;
	rf->dbg_component =
	/*DBG_RF_TX_PWR_TRACK	| */
	/*DBG_RF_IQK		| */
	/*DBG_RF_LCK		| */
	/*DBG_RF_DPK		| */
	/*DBG_RF_TXGAPK	| */
	/*DBG_RF_DACK		| */
	/*DBG_RF_DPK_TRACK	| */
	/*DBG_RF_RXDCK		| */
	/*DBG_RF_RFK		| */
	/*DBG_RF_MP		| */
	/*DBG_RF_TMP		| */
	/*DBG_RF_INIT		| */
	/*DBG_RF_POWER		|  */
	/*DBG_RF_RXGAINK	| */
	/*DBG_RF_THER_TRIM	| */
	/*DBG_RF_PABIAS_TRIM	| */
	/*DBG_RF_TSSI_TRIM	| */
	/*DBG_RF_PSD		| */
	/*DBG_RF_CHK		| */
	/*DBG_RF_XTAL_TRACK 	| */
	/*DBG_RF_FW		| */
	/*DBG_RF_OP5K_TRACK	| */
	/*DBG_RF_OP5K		| */
	0;

	rf->cmn_dbg_msg_cnt = HALRF_WATCHDOG_PERIOD;
	rf->cmn_dbg_msg_period = HALRF_WATCHDOG_PERIOD;
}

void halrf_iqk_log(struct rf_info *rf)
{
#if 0
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	/* IQK INFO */
	RF_DBG(rf, DBG_RF_IQK, "%-20s\n", "====== IQK Info ======");
	RF_DBG(rf, DBG_RF_IQK, "%-20s: %d %d\n", "iqk count / fail count",
	       iqk_info->iqk_cnt, iqk_info->iqk_fail_cnt);
	RF_DBG(rf, DBG_RF_IQK, "%-20s: %s\n", "segment_iqk",
	       (iqk_info->segment_iqk) ? "True" : "False");
#endif
}

void halrf_lck_log(struct rf_info *rf)
{
	RF_DBG(rf, DBG_RF_LCK, "%-20s\n", "====== LCK Info ======");
}

void halrf_support_ability(struct rf_info *rf, char input[][16], u32 *_used,
				 char *output, u32 *_out_len)
{
	u32 value[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i;

	for (i = 0; i < 5; i++)
		if (input[i + 1])
			_os_sscanf(input[i + 1], "%d", &value[i]);

	if (value[0] == 100) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[RF Supportability]\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "00. (( %s ))Power Tracking\n",
			 ((rf->support_ability & HAL_RF_TX_PWR_TRACK) ?
			 ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "01. (( %s ))IQK\n",
			 ((rf->support_ability & HAL_RF_IQK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "02. (( %s ))LCK\n",
			 ((rf->support_ability & HAL_RF_LCK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "03. (( %s ))DPK\n",
			 ((rf->support_ability & HAL_RF_DPK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "04. (( %s ))HAL_RF_TXGAPK\n",
			 ((rf->support_ability & HAL_RF_TXGAPK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "05. (( %s ))HAL_RF_DACK\n",
			 ((rf->support_ability & HAL_RF_DACK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "06. (( %s ))DPK_TRACK\n",
			 ((rf->support_ability & HAL_RF_DPK_TRACK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "07. (( %s ))HAL_2GBAND_SHIFT\n",
			 ((rf->support_ability & HAL_2GBAND_SHIFT) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "08. (( %s ))HAL_RF_RXDCK\n",
			 ((rf->support_ability & HAL_RF_RXDCK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "09. (( %s ))NONE\n", ".");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "10. (( %s ))NONE\n", ".");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "11. (( %s ))NONE\n", ".");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "12. (( %s ))NONE\n", ".");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "13. (( %s ))HAL_RF_THER_TRIM\n",
			 ((rf->support_ability & HAL_RF_THER_TRIM) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "14. (( %s ))HAL_RF_PABIAS_TRIM\n",
			 ((rf->support_ability & HAL_RF_PABIAS_TRIM) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "15. (( %s ))HAL_RF_TSSI_TRIM\n",
			 ((rf->support_ability & HAL_RF_TSSI_TRIM) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "16. (( %s ))NONE\n", ".");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "17. (( %s ))HAL_RF_TSSI_TRK\n",
			 ((rf->support_ability & HAL_RF_TSSI_TRK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "18. (( %s ))HAL_RF_XTAL_TRACK\n",
			 ((rf->support_ability & HAL_RF_XTAL_TRACK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "19. (( %s ))HAL_RF_TX_SHAPE\n",
			 ((rf->support_ability & HAL_RF_TX_SHAPE) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "20. (( %s ))HAL_RF_OP5K_TRACK\n",
			 ((rf->support_ability & HAL_RF_OP5K_TRACK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "21. (( %s ))HAL_RF_OP5K\n",
			 ((rf->support_ability & HAL_RF_OP5K) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "22. (( %s ))HAL_RF_TPE_CTRL\n",
			 ((rf->support_ability & HAL_RF_TPE_CTRL) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "23. (( %s ))HAL_RF_RXDCK_TRACK\n",
			 ((rf->support_ability & HAL_RF_RXDCK_TRACK) ? ("V") :
			 (".")));
	} else {
		if (value[1] == 1) /* enable */
			rf->support_ability |= BIT(value[0]);
		else if (value[1] == 2) /* disable */
			rf->support_ability &= ~(BIT(value[0]));
		else
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "[Warning!!!]  1:enable,  2:disable\n");
	}
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "Curr-RF_supportability =  0x%x\n\n", rf->support_ability);

	*_used = used;
	*_out_len = out_len;
}

void halrf_dbg_trace(struct rf_info *rf, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	u32 one = 1;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 value[10] = {0};
	u8 i;

	for (i = 0; i < 5; i++)
		if (input[i + 1])
			_os_sscanf(input[i + 1], "%d", &value[i]);

	if (value[0] == 100) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[RF Debug Trace Selection]\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "00. (( %s ))TX_PWR_TRACK\n",
			 ((rf->dbg_component & DBG_RF_TX_PWR_TRACK) ? ("V") :
			 (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "01. (( %s ))IQK\n",
			 ((rf->dbg_component & DBG_RF_IQK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "02. (( %s ))LCK\n",
			 ((rf->dbg_component & DBG_RF_LCK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "03. (( %s ))DPK\n",
			 ((rf->dbg_component & DBG_RF_DPK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "04. (( %s ))TXGAPK\n",
			 ((rf->dbg_component & DBG_RF_TXGAPK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "05. (( %s ))DACK\n",
			 ((rf->dbg_component & DBG_RF_DACK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "06. (( %s ))DPK_TRACK\n",
			 ((rf->dbg_component & DBG_RF_DPK_TRACK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "08. (( %s ))RXDCK\n",
			 ((rf->dbg_component & DBG_RF_RXDCK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "09. (( %s ))RFK\n",
			 ((rf->dbg_component & DBG_RF_RFK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "10. (( %s ))INIT\n",
			 ((rf->dbg_component & DBG_RF_INIT) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "11. (( %s ))POWER\n",
			 ((rf->dbg_component & DBG_RF_POWER) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "16. (( %s ))PSD\n",
			 ((rf->dbg_component & DBG_RF_PSD) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "18. (( %s ))XTAL TRK\n",
			 ((rf->dbg_component & DBG_RF_XTAL_TRACK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "20. (( %s ))OP5K TRK\n",
			 ((rf->dbg_component & DBG_RF_OP5K_TRACK) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "21. (( %s ))OP5K\n",
			 ((rf->dbg_component & DBG_RF_OP5K) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "28. (( %s ))FW\n",
			 ((rf->dbg_component & DBG_RF_FW) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "29. (( %s ))MP\n",
			 ((rf->dbg_component & DBG_RF_MP) ? ("V") : (".")));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "30. (( %s ))TMP\n",
			 ((rf->dbg_component & DBG_RF_TMP) ? ("V") : (".")));

	} else if (value[0] == 101) {
		rf->dbg_component = 0;
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Disable all DBG COMP\n");
	} else {
		if (value[1] == 1) /*enable*/
			rf->dbg_component |= (one << value[0]);
		else if (value[1] == 2) /*disable*/
			rf->dbg_component &= ~(one << value[0]);
	}
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "Curr-RF_Dbg_Comp = 0x%x\n", rf->dbg_component);

	*_used = used;
	*_out_len = out_len;
}

void halrf_dump_rfk_reg(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 addr = 0;

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		      "===================[ RFK Reg start ]===================\n");

	for (addr = 0x8000; addr < 0xa000; addr += 0x10)
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			    " 0x%x : 0x%08x  0x%08x  0x%08x  0x%08x\n", addr,
			    halrf_rreg(rf, addr, MASKDWORD),
			    halrf_rreg(rf, addr + 0x4, MASKDWORD),
			    halrf_rreg(rf, addr + 0x8, MASKDWORD),
			    halrf_rreg(rf, addr + 0xc, MASKDWORD));

	*_used = used;
	*_out_len = out_len;
}

void _halrf_dpk_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 used = *_used;
	u32 out_len = *_out_len;
	char *ic_name = NULL;
	u32 dpk_ver = 0;
	u32 rf_para = 0;
	u32 rfk_init_ver = 0;
	u8 path, kidx;
	u32 rf_para_min = 0;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		ic_name = "8852A";
		dpk_ver = DPK_VER_8852A;
		rf_para_min = 16;
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		ic_name = "8852B";
		dpk_ver = DPK_VER_8852B;
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		dpk_ver = DPK_VER_8852C;
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		ic_name = "8851B";
		dpk_ver = DPK_VER_8851B;
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		dpk_ver = DPK_VER_8832BR;
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		dpk_ver = DPK_VER_8192XB;
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		ic_name = "8852BP";
		dpk_ver = DPK_VER_8852BP;
		break;
#endif
	default:
		break;
	}

	rf_para = halrf_get_radio_reg_ver(rf);
	rfk_init_ver = halrf_get_nctl_reg_ver(rf);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ DPK info %s ]===============\n", ic_name);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "DPK Ver", dpk_ver);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d (%s)\n",
		 "RF Para Ver", rf_para, rf_para >= rf_para_min ? "match" : "mismatch");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "RFK init ver", rfk_init_ver);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d / %d (RFE type:%d)\n",
		 "Ext_PA 2G / 5G / 6G", rf->fem.epa_2g, rf->fem.epa_5g, rf->fem.epa_6g,
		 rf->phl_com->dev_cap.rfe_type);

	if (dpk->bp[0][0].ch == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, "\n %-25s\n",
			"No DPK had been done before!!!");
		*_used = used;
		*_out_len = out_len;
		return;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d / %d\n",
		 "DPK Cal / OK / Reload", dpk->dpk_cal_cnt, dpk->dpk_ok_cnt,
		 dpk->dpk_reload_cnt);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "BT IQK timeout", rf->is_bt_iqk_timeout ? "Yes" : "No");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 "DPK processing time", dpk->dpk_time);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "DPD status", dpk->is_dpk_enable ? "Enable" : "Disable");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "DPD track status", dpk->is_dpk_track_en ? "Enable" : "Disable");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %s\n",
		 "DBCC / TSSI", rf->hal_com->dbcc_en ? "On" : "Off",
		 rf->is_tssi_mode[0] ? "On" : "Off");

	for (path = 0; path < KPATH; path++) {
		for (kidx = 0; kidx < DPK_BKUP_NUM; kidx++) {
			if (dpk->bp[path][kidx].ch == 0)
				break;

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "=============== S%d[%d] ===============\n", path, kidx);
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %d / %s\n",
				 "Band / CH / BW", dpk->bp[path][kidx].band == 0 ? "2G" : (dpk->bp[path][kidx].band == 1 ? "5G" : "6G"),
				 dpk->bp[path][kidx].ch,
				 dpk->bp[path][kidx].bw == 0 ? "20M" : (dpk->bp[path][kidx].bw == 1 ? "40M" : 
				 (dpk->bp[path][kidx].bw == 2 ? "80M" : "160M")));

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
				 "DPK result", dpk->bp[path][kidx].path_ok ? "OK" : "Fail");

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
				 "ReK_cnt[0] / ReK_cnt[1]", dpk->rek_cnt[path][0], dpk->rek_cnt[path][1]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x / 0x%x / 0x%x / 0x%x/\n",
				 "ReK[0] Check", dpk->rek_chk[path][0][0], dpk->rek_chk[path][0][1], dpk->rek_chk[path][0][2],
				 dpk->rek_chk[path][0][3], dpk->rek_chk[path][0][4]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x / 0x%x / 0x%x / 0x%x/\n",
				 "ReK[1] Check", dpk->rek_chk[path][1][0], dpk->rek_chk[path][1][1], dpk->rek_chk[path][1][2],
				 dpk->rek_chk[path][1][3], dpk->rek_chk[path][1][4]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
				 "DPK TxAGC / Gain Scaling", dpk->bp[path][kidx].txagc_dpk, dpk->bp[path][kidx].gs);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
				 "Corr (idx/val)", dpk->corr_idx[path][kidx], dpk->corr_val[path][kidx]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "DPK RXIQC", dpk->dpk_rxiqc[path]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
				 "DC (I/Q)", dpk->dc_i[path][kidx], dpk->dc_q[path][kidx]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
				 "IDL_Sync / DC", dpk->dpk_sync[path], dpk->dpk_dciq[path]);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
				 "LDL_OV / RXBB_OV", dpk->ov_flag[path], dpk->rxbb_ov[path]);
		}
	}
	*_used = used;
	*_out_len = out_len;

}

void halrf_dpk_read_rc_mtx(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len, u32 path)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 addr = 0;
	char *ic_name = NULL;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		break;
#endif
	default:
		break;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ Read %s S%d RC_Mtx ]===============\n", ic_name, path);

	for (addr = 0x0; addr < 0x1b2; addr++)
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			    " 0x%03x | 0x%08x 0x%08x\n", addr,
			    dpk->rc_mtx[path][addr][0],
			    dpk->rc_mtx[path][addr][1]);

	*_used = used;
	*_out_len = out_len;
}

void halrf_dpk_read_rx_sram(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len, u32 path)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 addr = 0;
	char *ic_name = NULL;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		break;
#endif
	default:
		break;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ Read %s S%d RX_SRAM ]===============\n", ic_name, path);

	for (addr = 0x0; addr < 0x200; addr++)
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			    " 0x%03x | 0x%08x\n", addr, dpk->rx_sram[path][addr]);

	*_used = used;
	*_out_len = out_len;
}

void halrf_dpk_read_coef(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len, u32 path, bool is_first)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 addr = 0;
	char *ic_name = NULL;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		break;
#endif
	default:
		break;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ Read %s S%d RX_SRAM ]===============\n", ic_name, path);

	for (addr = 0; addr < 28; addr++)
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			    " 0x%02x | 0x%08x\n", addr, dpk->dpk_coef[path][is_first][addr]);

	*_used = used;
	*_out_len = out_len;
}

void halrf_dpk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 1
	char *cmd[8] = {"-h", "on", "off", "info", "trigger", "rc_mtx", "rx_sram", "coef"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0, val_1;
	u8 i;

	if (!(rf->support_ability & HAL_RF_DPK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK is Unsupported!!!\n");
		return;
	}

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 7; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK is Enabled!!\n");
		halrf_dpk_onoff(rf, true);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK is Disabled!!\n");
		halrf_dpk_onoff(rf, false);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		_halrf_dpk_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[4]) == 0){
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " DPK Trigger start!!\n");
		_os_sscanf(input[2], "%d", &val);
		halrf_dpk_trigger(rf, val, false);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " DPK Trigger finish!!\n");
	} else if (_os_strcmp(input[1], cmd[5]) == 0){
		_os_sscanf(input[2], "%d", &val);
		halrf_dpk_read_rc_mtx(rf, input, &used, output, &out_len, val);
	} else if (_os_strcmp(input[1], cmd[6]) == 0){
		_os_sscanf(input[2], "%d", &val);
		halrf_dpk_read_rx_sram(rf, input, &used, output, &out_len, val);
	} else if (_os_strcmp(input[1], cmd[7]) == 0){
		_os_sscanf(input[2], "%d", &val);
		_os_sscanf(input[3], "%d", &val_1);
		halrf_dpk_read_coef(rf, input, &used, output, &out_len, val, (bool)val_1);
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
#endif
}

void halrf_dpk_track_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 1
	struct halrf_dpk_info *dpk = &rf->dpk;

	char *cmd[4] = {"-h", "on", "off", "status"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;

	if (!(rf->support_ability & HAL_RF_DPK_TRACK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK_TRACK is Unsupported!!!\n");
		return;
	}

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 4; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK_TRACK is Enabled!!\n");
		halrf_dpk_track_onoff(rf, true);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK_TRACK is Disabled!!\n");
		halrf_dpk_track_onoff(rf, false);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK_TRACK is %s!!\n", dpk->is_dpk_track_en ? "Enable" : "Disable");
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
#endif
}

void halrf_rx_dck_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	u32 used = *_used;
	u32 out_len = *_out_len;
	char *ic_name = NULL;
	u32 rxdck_ver = 0;
	u8 path;
	u32 addr = 0;
	u32 reg_05[KPATH];

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		ic_name = "8852A";
		rxdck_ver = RXDCK_VER_8852A;
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		ic_name = "8852B";
		rxdck_ver = RXDCK_VER_8852B;
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		rxdck_ver = RXDCK_VER_8852C;
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		ic_name = "8851B";
		rxdck_ver = RXDCK_VER_8851B;
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		rxdck_ver = RXDCK_VER_8832BR;
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		rxdck_ver = RXDCK_VER_8192XB;
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		ic_name = "8192XB";
		rxdck_ver = RXDCK_VER_8852BP;
		break;
#endif
		default:
		break;
	}

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ RX_DCK info %s ]===============\n", ic_name);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "RX_DCK Ver", rxdck_ver);
	
	if (rx_dck->loc[0].cur_ch == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, "\n %-25s\n",
			"No RX_DCK had been done before!!!");

		*_used = used;
		*_out_len = out_len;
		return;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 "RX_DCK processing time", rx_dck->rxdck_time);

	for (path = 0; path < KPATH; path++) {
		if (rx_dck->loc[path].cur_ch == 0)
			break;

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 " S%d:", path);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s/ %d/ %s/ %s/ 0x%x\n",
			 "Band/ CH/ BW/ Cal/ Ther", rx_dck->loc[path].cur_band == 0 ? "2G" :
			(rx_dck->loc[path].cur_band == 1 ? "5G" : "6G"),
			rx_dck->loc[path].cur_ch,
		        rx_dck->loc[path].cur_bw == 0 ? "20M" :
		        (rx_dck->loc[path].cur_bw == 1 ? "40M" : 
			(rx_dck->loc[path].cur_bw == 2 ? "80M" : "160M")),
		       	rx_dck->is_afe ? "AFE" : "RFC", rx_dck->ther_rxdck[path]);
	}

	for (path = 0; path < KPATH; path++) {
		if (rx_dck->loc[path].cur_ch == 0)
			break;

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n---------------[ S%d DCK Value ]---------------\n", path);
		reg_05[path] = halrf_rrf(rf, path, 0x5, MASKRF);
		halrf_wrf(rf, path, 0x5, BIT(0), 0x0);
		halrf_wrf(rf, path, 0x00, MASKRFMODE, RF_RX);

		for (addr = 0; addr < 0x20; addr++) {
			halrf_wrf(rf, path, 0x00, 0x07c00, addr); /*[14:10]*/
			if (hal_i->chip_id == CHIP_WIFI6_8852C ||
			    hal_i->chip_id == CHIP_WIFI6_8852BP ||
			    hal_i->chip_id == CHIP_WIFI6_8851B
#ifdef RF_8192XB_SUPPORT
			    || hal_i->chip_id == CHIP_WIFI6_8192XB
#endif
			    )
				RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "0x%02x | 0x%02x/ 0x%02x   0x%02x/ 0x%02x\n", addr,
				    halrf_rrf(rf, path, 0x92, 0xF0000),  /*[19:16]*/
				    halrf_rrf(rf, path, 0x92, 0x0FE00),  /*[15:9]*/
				    halrf_rrf(rf, path, 0x93, 0xF0000),  /*[19:16]*/
				    halrf_rrf(rf, path, 0x93, 0x0FE00)); /*[15:9]*/
			else
				RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				    "0x%02x | 0x%02x/ 0x%02x   0x%02x/ 0x%02x\n", addr,
				    halrf_rrf(rf, path, 0x92, 0xF0000),  /*[19:16]*/
				    halrf_rrf(rf, path, 0x92, 0x0FC00),  /*[15:10]*/
				    halrf_rrf(rf, path, 0x93, 0xF0000),  /*[19:16]*/
				    halrf_rrf(rf, path, 0x93, 0x0FC00)); /*[15:10]*/
		}
		halrf_wrf(rf, path, 0x5, BIT(0), reg_05[path]);
	}
	*_used = used;
	*_out_len = out_len;
}

void halrf_get_rx_dck_value(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	u32 val_1 = 0, val_2 = 0;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 reg_05[KPATH];

	_os_sscanf(input[2], "%d", &val_1); /*RF path*/
	_os_sscanf(input[3], "%x", &val_2); /*RF 0x0*/

	if (val_1 > 3) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 " Wrong path number!!\n");
		return;
	}

	reg_05[val_1] = halrf_rrf(rf, val_1, 0x5, MASKRF);

	halrf_wrf(rf, val_1, 0x5, BIT(0), 0x0);
	halrf_wrf(rf, val_1, 0x00, MASKRF, val_2);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 " Input S%d RF0x00 = 0x%x\n", val_1, val_2);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 " Get I : RXBB / TIA = 0x%x / 0x%x\n",
		 halrf_rrf(rf, val_1, 0x92, 0xF0000),  /*[19:16]*/
		 halrf_rrf(rf, val_1, 0x92, 0x0FC00));  /*[15:10]*/
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 " Get Q : RXBB / TIA = 0x%x / 0x%x\n",
		 halrf_rrf(rf, val_1, 0x93, 0xF0000),  /*[19:16]*/
		 halrf_rrf(rf, val_1, 0x93, 0x0FC00));  /*[15:10]*/

	halrf_wrf(rf, val_1, 0x5, BIT(0), reg_05[val_1]);

	*_used = used;
	*_out_len = out_len;
}

void halrf_rx_dck_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char *cmd[6] = {"-h", "on", "off", "info", "trigger", "get"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val_1 = 0, val_2 = 0;
	u8 i;

	if (!(rf->support_ability & HAL_RF_RXDCK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RX_DCK is Unsupported!!!\n");
		return;
	}

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 6; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RX_DCK is Enabled!!\n");
		halrf_rx_dck_onoff(rf, true);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RX_DCK is Disabled!!\n");
		halrf_rx_dck_onoff(rf, false);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		halrf_rx_dck_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[4]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RX DCK Trigger\n");
		_os_sscanf(input[2], "%d", &val_1);
		_os_sscanf(input[3], "%d", &val_2);
		halrf_rx_dck_trigger(rf, val_1, (bool)val_2);
	} else if (_os_strcmp(input[1], cmd[5]) == 0) {
		halrf_get_rx_dck_value(rf, input, &used, output, &out_len);
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");
	*_used = used;
	*_out_len = out_len;
}

void halrf_dack_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dack_info *dack = &rf->dack;

	u32 used = *_used;
	u32 out_len = *_out_len;
	char *ic_name = NULL;
	u32 dack_ver = 0;
	u32 rf_para = 0;
	u32 rfk_init_ver = 0;
	u8 i;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		ic_name = "8852A";
		dack_ver = DACK_VER_8852AB;
		rf_para = halrf_get_radio_reg_ver(rf);
		break;
#endif
		default:
		break;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ DACK info %s ]===============\n", ic_name);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "DACK Ver", dack_ver);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "RF Para Ver", rf_para);

	if (dack->dack_cnt == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, "\n %-25s\n",
			 "No DACK had been done before!!!");
	return;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "DACK count", dack->dack_cnt);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 "DACK processing time", dack->dack_time);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-60s = %d / %d / %d / %d / %d / %d\n",
		 "DACK timeout(ADDCK_0/ADDCK_1/DADCK_0/DADCK_1/MSBK_0/MSBK_1):",
		 dack->addck_timeout[0], dack->addck_timeout[1],
		 dack->dadck_timeout[0], dack->dadck_timeout[1],
		 dack->msbk_timeout[0], dack->msbk_timeout[1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "DACK Fail(last)", (dack->dack_fail) ? "TRUE" : "FALSE");		
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "===============[ ADDCK result ]===============\n");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x \n",
		 "S0_I/ S0_Q", dack->addck_d[0][0], dack->addck_d[0][1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x \n",
		 "S1_I/ S1_Q", dack->addck_d[1][0], dack->addck_d[1][1]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "===============[ DADCK result ]===============\n");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x \n",
		 "S0_I/ S0_Q", dack->dadck_d[0][0], dack->dadck_d[0][1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x \n",
		 "S1_I/ S1_Q", dack->dadck_d[1][0], dack->dadck_d[1][1]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "===============[ biask result ]===============\n");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x \n",
		 "S0_I/ S0_Q", dack->biask_d[0][0], dack->biask_d[0][1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x \n",
		 "S1_I/ S1_Q", dack->biask_d[1][0], dack->biask_d[1][1]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "===============[ MSBK result ]===============\n");
	for (i = 0; i < 16; i++) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" %s [%2d] = 0x%x/ 0x%x/ 0x%x/ 0x%x\n",
			 "S0_I/S0_Q/S1_I/S1_Q",
			 i,
			 dack->msbk_d[0][0][i], dack->msbk_d[0][1][i],
			 dack->msbk_d[1][0][i], dack->msbk_d[1][1][i]);
	}

	*_used = used;
	*_out_len = out_len;
}

void halrf_dack_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 1
	char *cmd[5] = {"-h", "on", "off", "info", "trigger"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;

	if (!(rf->support_ability & HAL_RF_DACK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DACK is Unsupported!!!\n");
		return;
	}

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 5; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK is Enabled!!\n");
		halrf_dack_onoff(rf, true);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "DPK is Disabled!!\n");
		halrf_dack_onoff(rf, false);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		halrf_dack_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[4]) == 0){
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " DACKTrigger start!!\n");
		_os_sscanf(input[1], "%d", &val);
		halrf_dack_trigger(rf, true);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " DACK Trigger finish!!\n");
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
#endif
}

void _halrf_tssi_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_tssi_info *tssi_info = &rf->tssi;
	struct halrf_dpk_info *dpk = &rf->dpk;
	u8 channel = rf->hal_com->band[0].cur_chandef.center_ch;
	u32 bw = rf->hal_com->band[0].cur_chandef.bw;
	u32 band = rf->hal_com->band[0].cur_chandef.band;
	u8 txsc_ch = rf->hal_com->band[0].cur_chandef.chan;

	u32 used = *_used;
	u32 out_len = *_out_len;
	char *ic_name = NULL;
	u32 tssi_ver = 0;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		ic_name = "8852A";
		tssi_ver = TSSI_VER_8852A;
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		ic_name = "8852B";
		tssi_ver = TSSI_VER_8852B;
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		tssi_ver = TSSI_VER_8852C;
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		tssi_ver = TSSI_VER_8832BR;
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		tssi_ver = TSSI_VER_8192XB;
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		ic_name = "8852BP";
		tssi_ver = TSSI_VER_8852BP;
		break;
#endif

	default:
		break;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ TSSI info %s ]===============\n", ic_name);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "TSSI Ver", tssi_ver);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d / %d (RFE type:%d)\n",
		 "Ext_PA 2G / 5G / 6G", rf->fem.epa_2g, rf->fem.epa_5g, rf->fem.epa_6g,
		 rf->phl_com->dev_cap.rfe_type);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s/%s\n",
		 "TSSI Status A/B",
		 rf->is_tssi_mode[RF_PATH_A] ? "Enable" : "Disable",
		 rf->is_tssi_mode[RF_PATH_B] ? "Enable" : "Disable");

#if 0
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s/%s\n",
		 "Slope Status A/B",
		 rf->tssi_slope_type[RF_PATH_A] == TSSI_SLOPE_DEFAULT ? "Enable" :
		 	(rf->tssi_slope_type[RF_PATH_A] == TSSI_SLOPE_ON ? "Force On" : "Disable"),
		 rf->tssi_slope_type[RF_PATH_B] == TSSI_SLOPE_DEFAULT ? "Enable" :
		 	(rf->tssi_slope_type[RF_PATH_B] == TSSI_SLOPE_ON ? "Force On" : "Disable"));
#else
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s/%s\n",
		"Slope Status A/B",
		halrf_rreg(rf, 0x581c, 0x00100000) ? "Enable" : "Disable",
		halrf_rreg(rf, 0x781c, 0x00100000) ? "Enable" : "Disable" );
#endif
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %s\n",
		 "DBCC / DPK", rf->hal_com->dbcc_en ? "On" : "Off",
		 dpk->is_dpk_enable ? "On" : "Off");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %d / %s / %d\n",
		 "Band / CH / BW / TXSC", band == BAND_ON_24G ? "2G" : (band == BAND_ON_5G ? "5G" : "6G"),
		 channel,
		 bw == 0 ? "20M" : (bw == 1 ? "40M" : (bw == 2 ? "80M" : "160M")),
		 txsc_ch);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d.%d / %d.%d / %d.%d (ms)\n",
		 "Total / Alimk / Slope",
		 tssi_info->tssi_total_time / 1000, tssi_info->tssi_total_time % 1000,
		 tssi_info->tssi_alimk_time / 1000, tssi_info->tssi_alimk_time % 1000,
		 tssi_info->tssi_slope_time / 1000, tssi_info->tssi_slope_time % 1000);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s : DE(%d) = EFUSE(%d) + Trim(%d)\n",
		 "TSSI DE CCK A",
		 tssi_info->curr_tssi_cck_de[RF_PATH_A],
		 tssi_info->curr_tssi_efuse_cck_de[RF_PATH_A],
		 tssi_info->curr_tssi_trim_de[RF_PATH_A]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s : DE(%d) = EFUSE(%d) + Trim(%d)\n",
		 "TSSI DE CCK B",
		 tssi_info->curr_tssi_cck_de[RF_PATH_B],
		 tssi_info->curr_tssi_efuse_cck_de[RF_PATH_B],
		 tssi_info->curr_tssi_trim_de[RF_PATH_B]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s : DE(%d) = EFUSE(%d) + Trim(%d)\n",
		 "TSSI DE OFDM A",
		 tssi_info->curr_tssi_ofdm_de[RF_PATH_A],
		 tssi_info->curr_tssi_efuse_ofdm_de[RF_PATH_A],
		 tssi_info->curr_tssi_trim_de[RF_PATH_A]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s : DE(%d) = EFUSE(%d) + Trim(%d)\n",
		 "TSSI DE OFDM B",
		 tssi_info->curr_tssi_ofdm_de[RF_PATH_B],
		 tssi_info->curr_tssi_efuse_ofdm_de[RF_PATH_B],
		 tssi_info->curr_tssi_trim_de[RF_PATH_B]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
		 "Slope CCK Gain Diff A/B",
		 halrf_rreg(rf, 0x562c, 0x0000ff00),
		 halrf_rreg(rf, 0x762c, 0x0000ff00));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
		 "Slope CCK CW Diff A/B",
		 halrf_rreg(rf, 0x561c, 0x000001ff),
		 halrf_rreg(rf, 0x761c, 0x000001ff));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
		 "Slope OFDM Gain Diff A/B",
		 halrf_rreg(rf, 0x5624, 0x0000ff00),
		 halrf_rreg(rf, 0x7624, 0x0000ff00));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
		 "Slope OFDM CW Diff A/B",
		 halrf_rreg(rf, 0x5610, 0x0003fe00),
		 halrf_rreg(rf, 0x7610, 0x0003fe00));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "TSSI HP Rst Cnt", tssi_info->high_pwr_rst_cnt);

	halrf_get_tssi_info(rf, input, &used, output, &out_len);

	*_used = used;
	*_out_len = out_len;

}

void halrf_tssi_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char *cmd[8] = {"-h", "on", "off", "info", "trk", "trigger", "final", "slope"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0, val1 = 0;
	u8 i;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 8; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TSSI is Enabled!!\n");
		halrf_tssi_enable(rf, val);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TSSI is Disabled!!\n");
		halrf_tssi_disable(rf, val);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		_halrf_tssi_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[4]) == 0) {
		halrf_get_tssi_trk_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[5]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TSSI Trigger start!!\n");
		_os_sscanf(input[2], "%d", &val);
		halrf_tssi_trigger(rf, val, true);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TSSI Trigger finish, TSSI ON!!!\n");
	} else if (_os_strcmp(input[1], cmd[6]) == 0){
		_os_sscanf(input[1], "%d", &val);
		_os_sscanf(input[2], "%d", &val1);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TSSI Final Path%s\n", (val1 == RF_PATH_A) ? "A" : "B");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TSSI Final Path%s Result:%d\n",
				 (val1 == RF_PATH_A) ? "A" : "B",
				 halrf_tssi_get_final(rf, val, (u8)val1));
	} else if (_os_strcmp(input[1], cmd[7]) == 0) {	/*slope*/
#if 0
		if (_os_strcmp(input[2], "default") == 0)
			halrf_tssi_slope_onoff(rf , TSSI_SLOPE_DEFAULT);
		else if (_os_strcmp(input[2], "on") == 0)
			halrf_tssi_slope_onoff(rf , TSSI_SLOPE_ON);
		else if (_os_strcmp(input[2], "off") == 0)
			halrf_tssi_slope_onoff(rf , TSSI_SLOPE_OFF);
		else
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " default\n on\n off\n\n");

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"TSSI Slope State A / B : %s / %s\n",
			rf->tssi_slope_type[RF_PATH_A] == TSSI_SLOPE_DEFAULT ? "Enable" :
				(rf->tssi_slope_type[RF_PATH_A] == TSSI_SLOPE_ON ? "Force On" : "Disable"),
			rf->tssi_slope_type[RF_PATH_B] == TSSI_SLOPE_DEFAULT ? "Enable" :
				(rf->tssi_slope_type[RF_PATH_B] == TSSI_SLOPE_ON ? "Force On" : "Disable"));
#else
		if (_os_strcmp(input[2], "on") == 0)
			halrf_tssi_slope_onoff(rf , TSSI_SLOPE_ON);
		else if (_os_strcmp(input[2], "off") == 0)
			halrf_tssi_slope_onoff(rf , TSSI_SLOPE_OFF);
		else
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " on\n off\n\n");

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"TSSI Slope State A / B : %s / %s\n",
			rf->tssi_slope_type[RF_PATH_A] == TSSI_SLOPE_ON ? "Enable" : "Disable",
			rf->tssi_slope_type[RF_PATH_B] == TSSI_SLOPE_ON ? "Enable" : "Disable");
#endif
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
}

static void _halrf_iqk_info(struct rf_info *rf, char input[][16], u32 *_used,
			    char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	u32 used = *_used;
	u32 out_len = *_out_len;
	char *ic_name = NULL;
	u32 ver = 0;
	u32 rfk_init_ver = 0;
	u8 tmp = iqk_info->iqk_table_idx[0];

	//RF_DBG(rf, DBG_RF_IQK, "[IQK]===>%s\n", __func__);
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		ic_name = "8852A";
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		ic_name = "8852B";
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		break;
#endif
	default:
		break;
	}
	
	ver = halrf_get_iqk_ver(rf);
	rfk_init_ver = halrf_get_nctl_reg_ver(rf);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ IQK info %s ]===============\n", ic_name);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "IQK Version", ver);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "RFK init ver", rfk_init_ver);	
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d / %d\n",
		 "IQK Cal / Fail / Reload", iqk_info->iqk_times, iqk_info->iqk_fail_cnt,
		 iqk_info->reload_cnt);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %d / %s\n",
		 "S0 Band / CH / BW",  iqk_info->iqk_band[0]== 0 ? "2G" : (iqk_info->iqk_band[0] == 1 ? "5G" : "6G"),
		 iqk_info->iqk_ch[0],
		 iqk_info->iqk_bw[0] == 0 ? "20M" : (iqk_info->iqk_bw[0] == 1 ? "40M" : "80M"));	
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S0 NB/WB TXIQK", iqk_info->is_wb_txiqk[0]? "WBTXK" : "NBTXK");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S0 NB/WB RXIQK", iqk_info->is_wb_rxiqk[0]? "WBRXK" : "NBRXK");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S0 LOK status", (iqk_info->lok_cor_fail[0][0] | iqk_info->lok_fin_fail[0][0]) ? "Fail" : "Pass");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S0 TXK status", iqk_info->iqk_tx_fail[0][0]? "Fail" : "Pass");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S0 RXK status", iqk_info->iqk_rx_fail[0][0]? "Fail" : "Pass");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x/ %x\n",
		 "S0 LOK iDACK/VBUF", iqk_info->lok_idac[tmp][0], iqk_info->lok_vbuf[tmp][0]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
		 "S0 TXK XYM", iqk_info->nb_txcfir[0]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
		 "S0 RXK XYM", iqk_info->nb_rxcfir[0]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %d / %s\n",
		 "S1 Band / CH / BW",  iqk_info->iqk_band[1]== 0 ? "2G" : (iqk_info->iqk_band[1] == 1 ? "5G" : "6G"),
		 iqk_info->iqk_ch[1],
		 iqk_info->iqk_bw[1] == 0 ? "20M" : (iqk_info->iqk_bw[1] == 1 ? "40M" : "80M"));
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S1 NB/WB TXIQK", iqk_info->is_wb_txiqk[1]? "WBTXK" : "NBTXK");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S1 NB/WB RXIQK", iqk_info->is_wb_rxiqk[1]? "WBRXK" : "NBRXK");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S1 LOK status", (iqk_info->lok_cor_fail[0][1] | iqk_info->lok_fin_fail[0][1]) ? "Fail" : "Pass");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S1 TXK status", iqk_info->iqk_tx_fail[0][1]? "Fail" : "Pass");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "S1 RXK status", iqk_info->iqk_rx_fail[0][1]? "Fail" : "Pass");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x/  %x\n",
		 "S1 LOK iDACK/VBUF", iqk_info->lok_idac[tmp][1], iqk_info->lok_vbuf[tmp][1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
		 "S1 TXK XYM", iqk_info->nb_txcfir[1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
		 "S1 RXK XYM", iqk_info->nb_rxcfir[1]);
	*_used = used;
	*_out_len = out_len;
	return;
}
void halrf_iqk_bypass_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 1
	char *cmd[4] = {"-h", "lok", "txk", "rxk"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;

	if (!(rf->support_ability & HAL_RF_IQK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "IQK is Unsupported!!!\n");
		return;
	}
	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 4; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "LOK is Bypass!!\n");
		halrf_iqk_lok_bypass(rf, 0x0);
		halrf_iqk_lok_bypass(rf, 0x1);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TOK is Bypass!!\n");		
		halrf_iqk_tx_bypass(rf, 0x0);
		halrf_iqk_tx_bypass(rf, 0x1);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RXK is Bypass!!\n");		
		halrf_iqk_rx_bypass(rf, 0x0);
		halrf_iqk_rx_bypass(rf, 0x1);
	} else {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");
	}
	*_used = used;
	*_out_len = out_len;
#endif
}

void halrf_iqk_klog_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 1
	char *cmd[6] = {"-h", "fft", "sram", "xym", "cfir",  "off"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;

	if (!(rf->support_ability & HAL_RF_IQK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "IQK is Unsupported!!!\n");
		return;
	}
	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 6; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "FFT message turn on!!\n");
		halrf_iqk_fft_enable(rf, true);	
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Sram message turn on!!\n");		
		halrf_iqk_sram_enable(rf, true);	
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "XYM message turn on!!\n");		
		halrf_iqk_xym_enable(rf, true);
	} else if (_os_strcmp(input[1], cmd[4]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "CFIR message turn on!!\n");		
		halrf_iqk_cfir_enable(rf, true);	
	} else if (_os_strcmp(input[1], cmd[4]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "all message turn off!!\n");		
		halrf_iqk_fft_enable(rf, false);		
		halrf_iqk_sram_enable(rf, false);
		halrf_iqk_xym_enable(rf, false);
		halrf_iqk_cfir_enable(rf, false);
	} else {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");
	}
	*_used = used;
	*_out_len = out_len;
#endif
}
void halrf_iqk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char *cmd[7] = {"-h", "on", "off", "info", "trigger", "nbiqk", "rxevm"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;
	u8 rxevm = 0x0;

	if (!(rf->support_ability & HAL_RF_IQK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "IQK is Unsupported!!!\n");
		return;
	}
	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 7; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "IQK is Enabled!!\n");
		halrf_iqk_onoff(rf, true);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "IQK is Disabled!!\n");
		halrf_iqk_onoff(rf, false);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		_halrf_iqk_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[4]) == 0){
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " WBIQK Trigger start!!\n");		
		halrf_nbiqk_enable(rf, false); 		
		halrf_iqk_trigger(rf, HW_PHY_0, false);
	} else if (_os_strcmp(input[1], cmd[5]) == 0){
		halrf_nbiqk_enable(rf, true);
		halrf_iqk_trigger(rf, val, false);
		halrf_nbiqk_enable(rf, false);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 " NBIQK Trigger!!\n");
	} else if (_os_strcmp(input[1], cmd[6]) == 0){
		rxevm = halrf_iqk_get_rxevm( rf);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" RXEVM = -%d dB!!\n", rxevm);
	} else {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");
	}
	*_used = used;
	*_out_len = out_len;
}

void halrf_pwr_table_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	char *cmd[14] = {"-h", "rate", "limit", "limit_ru", "set_all",
			"set", "txshape", "constraint", "coex", "force_reg",
			"rate_pwr_ctl", "pwr_lmt_6g", "antgain", "ant"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u32 tmp, tmp1, phy = HW_PHY_0;
	u8 i;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 14; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		_os_sscanf(input[2], "%d", &phy);
		if (phy >= HW_PHY_MAX)
			phy = HW_PHY_0;
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Power by rate info\n");
		halrf_pwr_by_rate_info(rf, input, &used, output, &out_len, phy);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		_os_sscanf(input[2], "%d", &phy);
		if (phy >= HW_PHY_MAX)
			phy = HW_PHY_0;
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Power limit info\n");
		halrf_pwr_limit_info(rf, input, &used, output, &out_len, phy);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		_os_sscanf(input[2], "%d", &phy);
		if (phy >= HW_PHY_MAX)
			phy = HW_PHY_0;
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Power limit RU info\n");
		halrf_pwr_limit_ru_info(rf, input, &used, output, &out_len, phy);
	} else if (_os_strcmp(input[1], cmd[4]) == 0) {
		pwr->fix_power[RF_PATH_A] = false;
		pwr->fix_power_dbm[RF_PATH_A] = 0;

		pwr->fix_power[RF_PATH_B] = false;
		pwr->fix_power_dbm[RF_PATH_B] = 0;

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Reset Power by Rate, Power limit and Power Limit RU to Default\n");
		halrf_set_power(rf, HW_PHY_0, PWR_BY_RATE);
		halrf_set_power(rf, HW_PHY_0, PWR_LIMIT);
		halrf_set_power(rf, HW_PHY_0, PWR_LIMIT_RU);

		if (rf->hal_com->dbcc_en) {
			halrf_set_power(rf, HW_PHY_1, PWR_BY_RATE);
			halrf_set_power(rf, HW_PHY_1, PWR_LIMIT);
			halrf_set_power(rf, HW_PHY_1, PWR_LIMIT_RU);
		}
	} else if (_os_strcmp(input[1], cmd[5]) == 0) {
		_os_sscanf(input[2], "%d", &tmp);

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set Power by Rate, Power limit and Power Limit RU %d.%ddBm\n",
			 tmp / 2,  tmp * 10 / 2 % 10);

		halrf_set_fix_power_to_struct(rf, HW_PHY_0, (s8)tmp);

		if (rf->hal_com->dbcc_en)
			halrf_set_fix_power_to_struct(rf, HW_PHY_1, (s8)tmp);
	} else if (_os_strcmp(input[1], cmd[6]) == 0) {
		_os_sscanf(input[2], "%d", &tmp);

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set TX Shape = 255, disable Set Tx shape function\n");

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set TX Shape = %d\n", tmp);

		halrf_set_tx_shape(rf, (u8)tmp);
	} else if (_os_strcmp(input[1], cmd[7]) == 0) {
		_os_sscanf(input[2], "%d", &tmp);
		_os_sscanf(input[3], "%d", &tmp1);

		halrf_set_power_constraint(rf, HW_PHY_0, (u16)tmp, true);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Set TX Constraint = %d.%ddB\n", tmp / 100, tmp % 100);

		if (rf->hal_com->dbcc_en) {
			halrf_set_power_constraint(rf, HW_PHY_1, (u16)tmp1, true);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Set TX Constraint PHY1 = %d.%ddB\n", tmp1 / 100, tmp1 % 100);
		}
	} else if (_os_strcmp(input[1], cmd[8]) == 0) {
		u32 tmp2, phy;

		_os_sscanf(input[2], "%d", &phy);
		_os_sscanf(input[3], "%d", &tmp);
		_os_sscanf(input[4], "%d", &tmp1);
		_os_sscanf(input[5], "%d", &tmp2);

		halrf_wlan_tx_power_control(rf, phy, tmp, tmp1, (bool)tmp2);

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "halrf_wlan_tx_power_control   phy=%d   pwr_ctrl_idx=%d   tx_power_val=%d   enable=%d\n",
			 phy, tmp, tmp1, tmp2);
	} else if (_os_strcmp(input[1], cmd[9]) == 0) {
		char *subcmd[3] = {"-h", "info", "set"};
		u32 enable, reg_2g_len, reg_5g_len, reg_6g_len;
		u32 tmp = 0;
		u8 reg[3] = {0};

		if (_os_strcmp(input[2], subcmd[0]) == 0) {
			for (i = 1; i < 3; i++)
				RF_DBG_CNSL(out_len, used, output + used, out_len - used, "  %s\n", subcmd[i]);
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				"set command : enable, reg_2g_len, reg_5g_len, reg_6g_len, reg0, reg1, reg2\n");
		} else if (_os_strcmp(input[2], subcmd[1]) == 0) {
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				" %-30s = %d\n", "reg_2g_len", pwr->reg_2g_len);
			for (i = 0; i < pwr->reg_2g_len; i++) {
				RF_DBG_CNSL(out_len, used, output + used, out_len - used,
					" %-30s = %s\n", "Set Force Reg 2G", pw_lmt_regu_type_str(pwr->reg_array_2g[i]));
			}

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				" %-30s = %d\n", "reg_5g_len", pwr->reg_5g_len);

			for (i = 0; i < pwr->reg_5g_len; i++) {
				RF_DBG_CNSL(out_len, used, output + used, out_len - used,
					" %-30s = %s\n", "Set Force Reg 5G", pw_lmt_regu_type_str(pwr->reg_array_5g[i]));
			}

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				" %-30s = %d\n", "reg_6g_len", pwr->reg_6g_len);

			for (i = 0; i < pwr->reg_6g_len; i++) {
				RF_DBG_CNSL(out_len, used, output + used, out_len - used,
					" %-30s = %s\n", "Set Force Reg 6G", pw_lmt_regu_type_str(pwr->reg_array_6g[i]));
			}
		} else if (_os_strcmp(input[2], subcmd[2]) == 0) {
			_os_sscanf(input[3], "%d", &enable);
			_os_sscanf(input[4], "%d", &reg_2g_len);
			_os_sscanf(input[5], "%d", &reg_5g_len);
			_os_sscanf(input[6], "%d", &reg_6g_len);

			_os_sscanf(input[7], "%d", &tmp);
			reg[0] = (u8)tmp;
			_os_sscanf(input[8], "%d", &tmp);
			reg[1] = (u8)tmp;
			_os_sscanf(input[9], "%d", &tmp);
			reg[2] = (u8)tmp;

			halrf_force_regulation(rf, (bool)enable,
				reg, (u8)reg_2g_len,
				reg, (u8)reg_5g_len,
				reg, (u8)reg_6g_len);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Force Regulation : enable=%d reg_2g_len=%d reg_5g_len=%d reg_6g_len=%d\n",
				 enable, reg_2g_len, reg_5g_len, reg_6g_len);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "Regulation Setting ==>\n  %d  %s\n  %d  %s\n  %d  %s\n",
				 reg[0], pw_lmt_regu_type_str(reg[0]),
				 reg[1], pw_lmt_regu_type_str(reg[1]),
				 reg[2], pw_lmt_regu_type_str(reg[2]));
		}
	} else if (_os_strcmp(input[1], cmd[10]) == 0) {
		_os_sscanf(input[2], "%d", &tmp);
		_os_sscanf(input[3], "%d", &tmp1);

		halrf_control_tx_rate_power(rf, HW_PHY_0, (s32)tmp);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"Set TX Rate Power Control = %s%d.%ddB\n",
			((s32)(tmp) < 0) ? "-" : "",
			((s32)(tmp) < 0) ? (tmp * -1 / 100) : (tmp / 100),
			((s32)(tmp) < 0) ? (tmp * -1 % 100) : (tmp % 100));

		if (rf->hal_com->dbcc_en) {
			halrf_control_tx_rate_power(rf, HW_PHY_1, (s32)tmp1);

			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				"Set TX Rate Power Control PHY1 = %s%d.%ddB\n",	 
				((s32)(tmp) < 0) ? "-" : "",
				((s32)(tmp) < 0) ? (tmp * -1 / 100) : (tmp / 100),
				((s32)(tmp) < 0) ? (tmp * -1 % 100) : (tmp % 100));
		}
	} else if (_os_strcmp(input[1], cmd[11]) == 0) {
		_os_sscanf(input[2], "%d", &tmp);

#if 0	
		if (tmp == PW_LMT_6G_LOW)
			halrf_wifi_event_notify(rf, MSG_EVT_SET_PWR_LIMIT_LOW, HW_PHY_0);
		else if (tmp == PW_LMT_6G_STD)
			halrf_wifi_event_notify(rf, MSG_EVT_SET_PWR_LIMIT_STD, HW_PHY_0);
#endif
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" PW_LMT_6G_LOW = 0\n PW_LMT_6G_STD = 1\n PW_LMT_6G_VLOW = 2\n");
	} else if (_os_strcmp(input[1], cmd[12]) == 0) {
		u32 regu, ag_2g, ag_5g, ag_6g;
		struct rtw_phl_regu_dyn_ant_gain regu_info = {0};

		_os_sscanf(input[2], "%d", &regu);
		_os_sscanf(input[3], "%d", &ag_2g);
		_os_sscanf(input[3], "%d", &ag_5g);
		_os_sscanf(input[5], "%d", &ag_6g);

		regu_info.regu = (u8)regu;
		regu_info.ag_2g_comp = (s8)ag_2g;
		regu_info.ag_5g_comp = (s8)ag_5g;
		regu_info.ag_6g_comp = (s8)ag_6g;

		halrf_set_dynamic_ant_gain(rf, HW_PHY_0, &regu_info);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"Set Dynamic Ant Gain Control reg=%d  2g=%d  5g=%d  6g=%d\n",
			regu, ag_2g, ag_5g, ag_6g);
	} else if (_os_strcmp(input[1], cmd[13]) == 0) {
		struct halrf_pwr_info *pwr = &rf->pwr_info;
		struct rtw_tpu_info *tpu = &rf->hal_com->band[HW_PHY_0].rtw_tpu_i;
		struct rtw_phl_ext_pwr_lmt_info *ext_pwr_info = &rf->hal_com->band[HW_PHY_0].rtw_tpu_i.ext_pwr_lmt_i;
		u32 ext_pwr_lmt_en, ant_type;
		u32 ant1, ant2;
			
		_os_sscanf(input[2], "%d", &ext_pwr_lmt_en);
		_os_sscanf(input[3], "%d", &ant_type);
		_os_sscanf(input[4], "%d", &ant1);
		_os_sscanf(input[5], "%d", &ant2);

		ext_pwr_info->ext_pwr_lmt_2_4g[0] = (s8)ant1;
		ext_pwr_info->ext_pwr_lmt_5g_band1[0] = (s8)ant1;
		ext_pwr_info->ext_pwr_lmt_5g_band2[0] = (s8)ant1;
		ext_pwr_info->ext_pwr_lmt_5g_band3[0] = (s8)ant1;
		ext_pwr_info->ext_pwr_lmt_5g_band4[0] = (s8)ant1;

		ext_pwr_info->ext_pwr_lmt_2_4g[1] = (s8)ant2;
		ext_pwr_info->ext_pwr_lmt_5g_band1[1] = (s8)ant2;
		ext_pwr_info->ext_pwr_lmt_5g_band2[1] = (s8)ant2;
		ext_pwr_info->ext_pwr_lmt_5g_band3[1] = (s8)ant2;
		ext_pwr_info->ext_pwr_lmt_5g_band4[1] = (s8)ant2;

		tpu->ext_pwr_lmt_en = (bool)ext_pwr_lmt_en;

		halrf_set_pwr_lmt_main_or_aux(rf, (u8)ant_type);

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"Set Ant1 Ant2 ext_pwr_lmt_en=%d  ant_type=%d  ant1=%d  ant2=%d\n",
			tpu->ext_pwr_lmt_en, pwr->ant_type,
			ext_pwr_info->ext_pwr_lmt_2_4g[0], ext_pwr_info->ext_pwr_lmt_2_4g[1]);
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
}

void halrf_rfk_check_reg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 1
	char *cmd[4] = {"-h", "backup", "check", "reload"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 3; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RFK backup!!\n");
		halrf_rfk_reg_backup(rf);
		halrf_rfc_reg_backup(rf);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RFK check!!\n");
#if 1		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[RFK]DACK reg check : %s \n", (halrf_dack_reg_check_fail(rf))  ? "FAIL" : "OK");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[RFK]RFK reg check : %s \n", (halrf_rfk_reg_check_fail(rf))?"FAIL" : "OK"); 
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[RFK]RFC reg check : %s \n", (halrf_rfc_reg_check_fail(rf))?"FAIL" : "OK"); 
#endif
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RFK reload!!\n");
		halrf_rfk_reg_reload(rf);
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
#endif
}

void halrf_test_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
#if 0

	u32 used = *_used;
	u32 out_len = *_out_len;

	u32 val[10] = {0};
	u8 i;

	for (i = 0; i < 5; i++) {
		if (input[i + 1])
			HALRF_SCAN(input[i + 1], DCMD_DECIMAL, &val[i]);
	}

	if (val[0] == 0) {
		halrf_rfk_chl_thermal(rf, (u8)val[1], (u8)val[2]);
	}else if (val[0] == 1) {
		halrf_rfk_recovery_chl_thermal(rf, (u8)val[1], (u8)val[2]);
	}

	*_used = used;
	*_out_len = out_len;
#endif
}

void _halrf_gapk_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u8 i;
	u8 channel = rf->hal_com->band[0].cur_chandef.center_ch;
	u32 bw = rf->hal_com->band[0].cur_chandef.bw;
	u32 band = rf->hal_com->band[0].cur_chandef.band;

	u32 used = *_used;
	u32 out_len = *_out_len;
	char *ic_name = NULL;
	u32 txgapk_ver = 0;
	u32 rf_para = 0;
	u32 rfk_init_ver = 0;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		ic_name = "8852A";
		txgapk_ver = TXGAPK_VER_8852A;
		rf_para = halrf_get_radio_reg_ver(rf);
		rfk_init_ver = halrf_get_nctl_reg_ver(rf);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		ic_name = "8852B";
		txgapk_ver = TXGAPK_VER_8852B;
		rf_para = halrf_get_radio_reg_ver(rf);
		rfk_init_ver = halrf_get_nctl_reg_ver(rf);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		ic_name = "8852C";
		txgapk_ver = TXGAPK_VER_8852C;
		rf_para = halrf_get_radio_reg_ver(rf);
		rfk_init_ver = halrf_get_nctl_reg_ver(rf);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		ic_name = "8832BR";
		txgapk_ver = TXGAPK_VER_8832BR;
		rf_para = halrf_get_radio_reg_ver(rf);
		rfk_init_ver = halrf_get_nctl_reg_ver(rf);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		ic_name = "8192XB";
		txgapk_ver = TXGAPK_VER_8192XB;
		rf_para = halrf_get_radio_reg_ver(rf);
		rfk_init_ver = halrf_get_nctl_reg_ver(rf);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		ic_name = "8852BP";
		txgapk_ver = TXGAPK_VER_8852BP;
		rf_para = halrf_get_radio_reg_ver(rf);
		rfk_init_ver = halrf_get_nctl_reg_ver(rf);
		break;
#endif

	default:
		break;
	}

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ TxGapK info %s ]===============\n", ic_name);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "TxGapK Ver", txgapk_ver);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "RF Para Ver", rf_para);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
		 "RFK init ver", rfk_init_ver);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 "TxGapK processing time", txgapk_info->txgapk_time);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d / %d (RFE type:%d)\n",
		 "Ext_PA 2G / 5G / 6G", rf->fem.epa_2g, rf->fem.epa_5g, rf->fem.epa_6g,
		 rf->phl_com->dev_cap.rfe_type);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s / %d / %s\n",
		 "Band / CH / BW", band == BAND_ON_24G ? "2G" : (band == BAND_ON_5G ? "5G" : "6G"),
		 channel,
		 bw == 0 ? "20M" : (bw == 1 ? "40M" : "80M"));

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "=======================\n");
	/* table info */
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		 "iqk_info->iqk_mcc_ch[0][0]/[0][1]", iqk_info->iqk_mcc_ch[0][0], iqk_info->iqk_mcc_ch[0][1]);	
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		 "iqk_info->iqk_mcc_ch[1][0]/[1][1]", iqk_info->iqk_mcc_ch[1][0], iqk_info->iqk_mcc_ch[1][1]);		 
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		 "iqk_info->iqk_table_idx[0]/[1]", iqk_info->iqk_table_idx[0], iqk_info->iqk_table_idx[1]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "txgapk_info->txgapk_mcc_ch[0]", txgapk_info->txgapk_mcc_ch[0]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "txgapk_info->txgapk_mcc_ch[1]", txgapk_info->txgapk_mcc_ch[1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "txgapk_info->txgapk_table_idx", txgapk_info->txgapk_table_idx);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "txgapk_info->ch", txgapk_info->ch[0]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "mcc_info->ch[0]", mcc_info->ch[0]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "mcc_info->ch[1]", mcc_info->ch[1]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "mcc_info->table_idx", mcc_info->table_idx);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "mcc_info->band[0]", mcc_info->band[0]);
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
		 "mcc_info->band[1]", mcc_info->band[1]);
		 

	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "===============[ TxGapK result ]===============\n");
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %s\n",
		 "TXGapK OK(last)", (txgapk_info->is_txgapk_ok) ? "TRUE" : "FALSE");

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x / 0x%x\n",
		 "Read0x8010 Befr /Aftr GapK", txgapk_info->r0x8010[0], txgapk_info->r0x8010[1]);
	
	RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "[ NCTL Done Check Times R_0xbff / R_0x80fc ]\n");

	/* txgapk_info->txgapk_chk_cnt[2][2][2]; */ /* path */ /* track pwr */ /* 0xbff8 0x80fc*/
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		"Path_0 Track", txgapk_info->txgapk_chk_cnt[0][TXGAPK_TRACK][0], txgapk_info->txgapk_chk_cnt[0][TXGAPK_TRACK][1]);				
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		"Path_0 PWR",txgapk_info->txgapk_chk_cnt[0][TXGAPK_PWR][0], txgapk_info->txgapk_chk_cnt[0][TXGAPK_PWR][1]);			
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		"Path_0 IQKBK", txgapk_info->txgapk_chk_cnt[0][TXGAPK_IQKBK][0], txgapk_info->txgapk_chk_cnt[0][TXGAPK_IQKBK][1]);

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		"Path_1 Track", txgapk_info->txgapk_chk_cnt[1][TXGAPK_TRACK][0], txgapk_info->txgapk_chk_cnt[1][TXGAPK_TRACK][1]);				
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		"Path_1 PWR", txgapk_info->txgapk_chk_cnt[1][TXGAPK_PWR][0], txgapk_info->txgapk_chk_cnt[1][TXGAPK_PWR][1]);			
	RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d / %d\n",
		"Path_1 IQKBK", txgapk_info->txgapk_chk_cnt[1][TXGAPK_IQKBK][0], txgapk_info->txgapk_chk_cnt[1][TXGAPK_IQKBK][1]);


	for (i = 0; i < 17; i++) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" %s [%2d] = 0x%02x/ 0x%02x/ 0x%02x/ 0x%02x\n",
			 "S0: Trk_d/Trk_ta/Pwr_d/Pwr_ta",
			 i,
			 txgapk_info->track_d[0][i]&0xff, txgapk_info->track_ta[0][i]&0xff,
			 txgapk_info->power_d[0][i]&0xff, txgapk_info->power_ta[0][i]&0xff);
	}
	for (i = 0; i < 17; i++) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" %s [%2d] = 0x%02x/ 0x%02x/ 0x%02x/ 0x%02x\n",
			 "S1: Trk_d/Trk_ta/Pwr_d/Pwr_ta",
			 i,
			 txgapk_info->track_d[1][i]&0xff, txgapk_info->track_ta[1][i]&0xff,
			 txgapk_info->power_d[1][i]&0xff, txgapk_info->power_ta[1][i]&0xff);
	}
		
	*_used = used;
	*_out_len = out_len;

}

void halrf_txgapk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char *cmd[5] = {"-h", "on", "off", "info", "trigger"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;

	if (!(rf->support_ability & HAL_RF_TXGAPK)) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TXGAPK is Unsupported!!!\n");
		return;
	}

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 5; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TXGAPK is Enabled!!\n");
		halrf_gapk_enable(rf, val);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "TXGAPK is Disabled!!\n");
		halrf_gapk_disable(rf, val);
	} else if (_os_strcmp(input[1], cmd[3]) == 0) {
		_halrf_gapk_info(rf, input, &used, output, &out_len);
	} else if (_os_strcmp(input[1], cmd[4]) == 0){
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TXGAPK Trigger start!!\n");
		_os_sscanf(input[1], "%d", &val);
		halrf_gapk_trigger(rf, val, true);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "TXGAPK Trigger finish, TXGAPK ON!!!\n");
	} else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
}

void halrf_dump_rf_reg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char *cmd[1] = {"-h"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u32 i;
	u32 start_addr = 0, end_addr = 0, range_value = 0, path = 0;

	_os_sscanf(input[1], "%x", &path);
	_os_sscanf(input[2], "%x", &start_addr);
	_os_sscanf(input[3], "%x", &range_value);
	end_addr = start_addr + range_value;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " Command parameters :\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " <rf_path>		: 0/1/2/3 = rf-A/B/C/D\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " <offset>		: rf start offset (HEX)\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " <num>			: number of offset to dump (HEX)\n");
	} else if (path < RTW_PHL_MAX_RF_PATH) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" Dump RF Register Path:%d 0x%X ~ 0x%X\n", path, start_addr, end_addr - 1);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" See Result in DebugView or Debug Log\n");

		for (i = start_addr; i < end_addr; i = i + 4) {
			if (!(i % 4)) {
				RF_DBG_VAST(out_len, used, output + used, out_len - used,
					" RF-%d (0x%02X) = ", path, i);
			}

			RF_DBG_VAST(out_len, used, output + used, out_len - used,
				"%05X  %05X  %05X  %05X\n",
				halrf_rrf(rf, path, i, 0xfffff),
				halrf_rrf(rf, path, i + 1, 0xfffff),
				halrf_rrf(rf, path, i + 2, 0xfffff),
				halrf_rrf(rf, path, i + 3, 0xfffff));
		}

		RF_DBG_VAST(out_len, used, output + used, out_len - used, "\n\n\n");

		for (i = start_addr; i < end_addr; i++) {
			RF_DBG_VAST(out_len, used, output + used, out_len - used,
				" RF-%d (0x%02X) = %05X\n", path, i,
				halrf_rrf(rf, path, i, 0xfffff));
		}
	} else {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" echo rf dump -h\n");
	}

	*_used = used;
	*_out_len = out_len;
}

void halrf_hwtx_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	u32 value[10] = {0};
	u8 i;

	for (i = 0; i < 4; i++)
		if (input[i + 1])
			_os_sscanf(input[i + 1], "%d", &value[i]);

	if (_os_strcmp(input[1], "-h") == 0) {
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "echo rf hwtx enable path cnt dB\n");
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "Enable / Disable = 1 / 0\n");
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "PathA / PathB = 0 / 1\n");
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "EX:echo rf hwtx 1 0 0 10\n");
	}

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		"==>Enable:%d	Path:%d   Count:%d   Power:%ddB\n", value[0], value[1], value[2], value[3]);

	//halrf_tssi_hw_tx_8852a(rf, 0, path, cnt, dbm, HT_MF_FMT, 0, enable);
	if (value[0] == 1) {
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "HW TX Start\n");

		halrf_btc_rfk_ntfy(rf, (BIT(HW_PHY_0) << 4), RF_BTC_TSSI, RFK_START);
		halrf_tmac_tx_pause(rf, HW_PHY_0, true);

		halrf_hw_tx(rf, (u8)value[1], (u16)value[2],
			(s16)(value[3] * 4), HT_MF_FMT, 0, 1);
	}

	if (value[0] == 0) {
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "HW TX Stop\n");

		halrf_hw_tx(rf, (u8)value[1], (u16)value[2],
			(s16)(value[3] * 4), HT_MF_FMT, 0, 0);

		halrf_tx_mode_switch(rf, HW_PHY_0, 0);

		halrf_tmac_tx_pause(rf, HW_PHY_0, false);
		halrf_btc_rfk_ntfy(rf, (BIT(HW_PHY_0) << 4), RF_BTC_TSSI, RFK_STOP);
	}
}

void halrf_kfree_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct halrf_kfree_info *kfree = &rf->kfree_info;

	char *cmd[3] = {"-h", "info", "efuse"};
	u32 val = 0;
	u8 i, tmp;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 3; i++)
			RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		halrf_kfree_get_info(rf, input, _used, output, _out_len);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		if (_os_strcmp(input[2], "-h") == 0)
			RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 " EX: echo rf kfree efuse 0x5dc\n");
		else {
			_os_sscanf(input[2], "%x", &val);
			halrf_phy_efuse_get_info(rf, val, 1, &tmp);
			RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
					 "Physical Efuse 0x%X=0x%X\n", val, tmp);
		}
	} else
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 " No CMD find!!\n");
}

void halrf_chl_rfk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	u8 idx = 6;
	char *cmd[6] = {"-h","info", "time", "ic", "radio_init", "nctl_init"};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val = 0;
	u8 i;
	char *ic_name = NULL;

	u8 dack_ver = 0;
	u8 rxdck_ver = 0;
	u8 txgapk_ver = 0;
	u8 tssi_ver = 0;
	u8 dpk_ver = 0;
		
	struct halrf_dack_info *dack = &rf->dack;
	struct halrf_gapk_info *txgapk_info = &rf->gapk;
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	struct halrf_tssi_info *tssi_info = &rf->tssi;
	struct halrf_dpk_info *dpk = &rf->dpk;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < idx; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		//DACK
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[DACK]] ====>\n");
		if (!(rf->support_ability & HAL_RF_DACK))
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, "DACK is Unsupported!!!\n");
		else	
			halrf_dack_info(rf, input, &used, output, &out_len);
		//IQK
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[IQK]] ====>\n");
		if (!(rf->support_ability & HAL_RF_IQK))
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, "IQK is Unsupported!!!\n");
		else
			_halrf_iqk_info(rf, input, &used, output, &out_len);		
		//DPK
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[DPK]] ====>\n");
		if (!(rf->support_ability & HAL_RF_DPK))
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, "DPK is Unsupported!!!\n");
		else	
			_halrf_dpk_info(rf, input, &used, output, &out_len);	
		//DCK
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[RXDCK]] ====>\n");
		if (!(rf->support_ability & HAL_RF_RXDCK))
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, "RXDCK is Unsupported!!!\n");
		else			
			halrf_rx_dck_info(rf, input, &used, output, &out_len);	
		//TXGAPK
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[TXGAPK]] ====>\n");
		if (!(rf->support_ability & HAL_RF_TXGAPK))
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, "TXGAPK is Unsupported!!!\n");
		else 	
			_halrf_gapk_info(rf, input, &used, output, &out_len);	
		//TSSI
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[TSSI]] ====>\n");
		if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, "TX_PWR_TRACK is Unsupported!!!\n");
		else	
			_halrf_tssi_info(rf, input, &used, output, &out_len);	
		//PWR TABLE RATE
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[PWR TABLE RATE]] PHY0====>\n");
		halrf_pwr_by_rate_info(rf, input, &used, output, &out_len, HW_PHY_0);

		if (rf->hal_com->dbcc_en) {
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[PWR TABLE RATE]] is DBCC, PHY1====>\n");
			halrf_pwr_by_rate_info(rf, input, &used, output, &out_len, HW_PHY_1);
		}
		//PWR TABLE LIMIT
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[PWR TABLE LIMIT]] PHY0====>\n");
		halrf_pwr_limit_info(rf, input, &used, output, &out_len, HW_PHY_0);

		if (rf->hal_com->dbcc_en) {
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[PWR TABLE LIMIT]] is DBCC,PHY1====>\n");
			halrf_pwr_limit_info(rf, input, &used, output, &out_len, HW_PHY_1);
		}
		//THERMAL
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,"\n------->\n[[THERMAL]] ====>\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Thermal A: %d\n", halrf_get_thermal(rf, RF_PATH_A));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Thermal B: %d\n", halrf_get_thermal(rf, RF_PATH_B)); 

		RF_DBG_CNSL(out_len, used, output + used, out_len - used," ==== Processing Time =========\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
			 "DACK processing time", dack->dack_time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 	"TxGapK processing time", txgapk_info->txgapk_time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 	"RxDCK processing time", rx_dck->rxdck_time);		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
				 "IQK processing time", iqk_info->time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d.%dms / %d.%dms\n",
				 "Total / Alimk Time",
				 tssi_info->tssi_total_time / 1000, tssi_info->tssi_total_time % 1000,
				 tssi_info->tssi_alimk_time / 1000, tssi_info->tssi_alimk_time % 1000);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
				 "DPK processing time", dpk->dpk_time);
	}else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used," ==== Processing Time =========\n");
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
			 "init_rf_reg time", rf->init_rf_reg_time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
			 "set_ch_bw time", rf->set_ch_bw_time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
			 "DACK processing time", dack->dack_time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 	"TxGapK processing time", txgapk_info->txgapk_time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
		 	"RxDCK processing time", rx_dck->rxdck_time);		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
				 "IQK processing time", iqk_info->time);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d.%dms / %d.%dms\n",
				 "Total / Alimk Time",
				 tssi_info->tssi_total_time / 1000, tssi_info->tssi_total_time % 1000,
				 tssi_info->tssi_alimk_time / 1000, tssi_info->tssi_alimk_time % 1000);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d ms\n",
				 "DPK processing time", dpk->dpk_time);
	}else if (_os_strcmp(input[1], cmd[3]) == 0) {
		switch (rf->ic_type) {
	#ifdef RF_8852A_SUPPORT
		case RF_RTL8852A:
			ic_name = "RF_RTL8852A";		
			dack_ver = DACK_VER_8852A;
			rxdck_ver =  RXDCK_VER_8852A;
			txgapk_ver = TXGAPK_VER_8852A;
			tssi_ver = TSSI_VER_8852A;
			dpk_ver = DPK_VER_8852A;
			break;
	#endif
	#ifdef RF_8852B_SUPPORT
		case RF_RTL8852B:
			ic_name = "RF_RTL8852B";		
			dack_ver = DACK_VER_8852B;
			rxdck_ver =  RXDCK_VER_8852B;
			txgapk_ver = TXGAPK_VER_8852B;
			tssi_ver = TSSI_VER_8852B;
			dpk_ver = DPK_VER_8852B;
			break;
	#endif
	#ifdef RF_8852C_SUPPORT
		case RF_RTL8852C:
			ic_name = "RF_RTL8852C";
			dack_ver = DACK_VER_8852C;
			rxdck_ver =  RXDCK_VER_8852C;
			txgapk_ver = TXGAPK_VER_8852C;
			tssi_ver = TSSI_VER_8852C;
			dpk_ver = DPK_VER_8852C;
			break;
	#endif
	#ifdef RF_8832BR_SUPPORT
		case RF_RTL8832BR:
			ic_name = "RF_RTL8832BR";
			dack_ver = DACK_VER_8832BR;
			rxdck_ver =  RXDCK_VER_8832BR;
			txgapk_ver = TXGAPK_VER_8832BR;
			tssi_ver = TSSI_VER_8832BR;
			dpk_ver = DPK_VER_8832BR;
			break;
	#endif
	#ifdef RF_8192XB_SUPPORT
		case RF_RTL8192XB:
			ic_name = "RF_RTL8192XB";
			dack_ver = DACK_VER_8192XB;
			rxdck_ver =  RXDCK_VER_8192XB;
			txgapk_ver = TXGAPK_VER_8192XB;
			tssi_ver = TSSI_VER_8192XB;
			dpk_ver = DPK_VER_8192XB;
			break;
	#endif
	#ifdef RF_8852BP_SUPPORT
		case RF_RTL8852BP:
			ic_name = "RF_RTL8852BP";
			dack_ver = DACK_VER_8852BP;
			rxdck_ver =  RXDCK_VER_8852BP;
			txgapk_ver = TXGAPK_VER_8852BP;
			tssi_ver = TSSI_VER_8852BP;
			dpk_ver = DPK_VER_8852BP;
			break;
	#endif
	#ifdef RF_8832CRVU_SUPPORT
		case RF_RTL8832CR_VU:
			ic_name = "RF_RTL8832CR_VU";
			dack_ver = DACK_VER_8832CRVU;
			rxdck_ver =  RXDCK_VER_8832CRVU;
			txgapk_ver = TXGAPK_VER_8832CRVU;
			tssi_ver = TSSI_VER_8832CRVU;
			dpk_ver = DPK_VER_8832CRVU;
			break;
	#endif
	#ifdef RF_8832BRVT_SUPPORT
		case RF_RTL8832BR_VT:
			ic_name = "RF_RTL8832BR_VT";
			dack_ver = DACK_VER_8832BRVT;
			rxdck_ver =  RXDCK_VER_8832BRVT;
			txgapk_ver = TXGAPK_VER_8832BRVT;
			tssi_ver = TSSI_VER_8832BRVT;
			dpk_ver = DPK_VER_8832BRVT;
			break;
	#endif

		default:
			ic_name = "no_info";
			break;
		}

		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n===============[ rf->ic_type = 0x%x, %s ]===============\n", rf->ic_type, ic_name);
		
		if (rf->use_sub_did) {
			RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "Use Sub_did", rf->sub_did);
		}

		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "DACK VER", dack_ver);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "RXDCK VER", rxdck_ver);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "TXGAPK VER", txgapk_ver);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "IQK VER", halrf_get_iqk_ver(rf));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "TSSI VER", tssi_ver);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "DPK VER", dpk_ver);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = 0x%x\n",
				 "RFK Init(NCTL) VER", halrf_get_nctl_reg_ver(rf));

		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
				 "radio_ver_from_reg", halrf_get_radio_ver_from_reg(rf));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
				 "extra_para_ver_from_reg", halrf_get_extra_para_ver_from_reg(rf));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
				 "rfe_type_ver_from_reg", halrf_get_rfe_type_ver_from_reg(rf));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %d\n",
				 "rfe_type_ver_from_PHL", rf->phl_com->dev_cap.rfe_type);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " %-25s = %x\n",
				 "fem_id_from_reg", halrf_get_fem_id_from_reg(rf));
	}else if (_os_strcmp(input[1], cmd[4]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " radio init: halrf_config_radio(rf, phy)!!\n");
		halrf_config_radio(rf, 0);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " radio init DONE.\n");

	} else if (_os_strcmp(input[1], cmd[5]) == 0) {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " nctl init: halrf_config_nctl_reg(rf)!!\n");
		halrf_config_nctl_reg(rf);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used, " nctl init DONE.\n");

	}else
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 " No CMD find!!\n");

	*_used = used;
	*_out_len = out_len;
}

void _halrf_op5k_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct halrf_op5k_info *op5k = &rf->op5k_info;
	u8 channel = rf->hal_com->band[0].cur_chandef.center_ch;
	u32 bw = rf->hal_com->band[0].cur_chandef.bw;
	u32 band = rf->hal_com->band[0].cur_chandef.band;
	u8 txsc_ch = rf->hal_com->band[0].cur_chandef.chan;
	u8 i;

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		 "\n===============[ OP5K info ]===============\n");

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, " %-25s = %s / %d / %s / %d\n",
		 "Band / CH / BW / TXSC", band == BAND_ON_24G ? "2G" : (band == BAND_ON_5G ? "5G" : "6G"),
		 channel,
		 bw == 0 ? "20M" : (bw == 1 ? "40M" : (bw == 2 ? "80M" : "160M")),
		 txsc_ch);

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, " %-25s = 0x%x / 0x%x\n",
		 "Original OP5 A / B", op5k->op5k_backup[RF_PATH_A], op5k->op5k_backup[RF_PATH_B]);

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, " %-25s = 0x%x / 0x%x\n",
		 "Final OP5 A / B", op5k->rst_cnt_final[RF_PATH_A], op5k->rst_cnt_final[RF_PATH_B]);

	RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, " %-25s = 0x%x / 0x%x\n",
		 "Base Thermal A / B", op5k->thermal_op5k[RF_PATH_A], op5k->thermal_op5k[RF_PATH_B]);

	for (i = 1; i < OP5K_RESET_CNT_DATA; i++)
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, " %-30s  %d ==> 0x%x / 0x%x\n",
			 "Reset Cnt Result Idx / A / B", i, op5k->rst_cnt[RF_PATH_A][i], op5k->rst_cnt[RF_PATH_B][i]);	

}

void halrf_op5k_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	u8 idx = 3;
	char *cmd[3] = {"-h", "info", "trigger"};
	u32 val = 0;
	u8 i;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < idx; i++)
			RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		_halrf_op5k_info(rf, input, _used, output, _out_len);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		halrf_op5k_trigger(rf);
	} else
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 " No CMD find!!\n");

}

void halrf_rfk_dbg_cmd(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char *cmd[3] = {"-h", "info", "trigger"};
	u32 val = 0;
	u8 i;

	if (_os_strcmp(input[1], cmd[0]) == 0) {
		for (i = 1; i < 3; i++)
			RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 "  %s\n", cmd[i]);
	} else if (_os_strcmp(input[1], cmd[1]) == 0) {
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "\n===============[ RFK info ]===============\n");
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "RFK Total Time %d.%dms\n", rf->rfk_total_time / 1000, rf->rfk_total_time % 1000);
	} else if (_os_strcmp(input[1], cmd[2]) == 0) {
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "RFK Trigger Start !!!\n");
		_os_sscanf(input[2], "%d", &val);
		if (val >= HW_PHY_MAX)
			val = HW_PHY_0;
		halrf_chl_rfk_trigger(rf, val, true);
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "RFK Trigger End !!!\n");
	} else
		RF_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				 " No CMD find!!\n");

}

