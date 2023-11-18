/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2021, Realtek Semiconductor Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * Neither the name of the Realtek nor the names of its contributors may
 *     be used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "halbb_precomp.h"
#include "halbb_dbg_cmd_table.h"

void halbb_bbcr_rw_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	u32 val[10] = {0};
	u32 addr = 0 , mask = MASKDWORD, val_32 = 0;
	u32 write_val = 0;
	u8 mask_m = 31, mask_l = 0;

	HALBB_SCAN(input[1], DCMD_DECIMAL, &val[0]);

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "r dw {reg_DW}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "r {bit_M} {bit_L} {reg_DW}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "w dw {reg_DW} {val}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "w {bit_M} {bit_L} {reg_DW} {val}\n");
		return;
	}

	if (_os_strcmp(input[2], "dw") == 0) {
		HALBB_SCAN(input[3], DCMD_HEX, &val[0]);
		HALBB_SCAN(input[4], DCMD_HEX, &val[1]);
		addr = val[0];
		write_val = val[1];
	} else {
		HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
		HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
		HALBB_SCAN(input[4], DCMD_HEX, &val[2]);
		HALBB_SCAN(input[5], DCMD_HEX, &val[3]);
		
		mask_m = (u8)val[0];
		mask_l = (u8)val[1];
		addr = val[2];
		write_val = val[3];

		mask = (u32)halbb_gen_mask(mask_m, mask_l);
		if (mask == 0) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Error mask = 0x%x\n", mask);
		}
	}

	if (addr % 4) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Error DW offset = 0x%x\n", addr);
			return;
	}

	if (_os_strcmp(input[1], "r") == 0) {
		val_32 = halbb_get_reg(bb, addr, mask);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[R] Reg0x%x[%02d:%02d] = 0x%x\n", addr, mask_m, mask_l, val_32);
	} else if (_os_strcmp(input[1], "w") == 0) {
		halbb_set_reg(bb, addr, mask, write_val);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[W] Reg0x%x[%02d:%02d] = 0x%x\n", addr, mask_m, mask_l, write_val);

		halbb_delay_ms(bb, 1);

		val_32 = halbb_get_reg(bb, addr, mask);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[READ_BACK] Reg0x%x[%02d:%02d] = 0x%x\n", addr, mask_m, mask_l, val_32);
	}
#if 0
	else if (_os_strcmp(input[1], "rf_r") == 0) {
		val_32 = halbb_rf_get_bb_reg(bb, addr, mask);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[RFR] Reg0x%x[%02d:%02d] = 0x%x\n", addr, mask_m, mask_l, val_32);
	} else if (_os_strcmp(input[1], "rf_w") == 0) {
		halbb_rf_set_bb_reg(bb, addr, mask, write_val);	
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[RFW] Reg0x%x[%02d:%02d] = 0x%x\n", addr, mask_m, mask_l, write_val);

		halbb_delay_ms(bb, 1);

		val_32 = halbb_get_reg(bb, addr, mask);	
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[READ_BACK] Reg0x%x[%02d:%02d] = 0x%x\n", mask_m, mask_l, addr, val_32);
	}
#endif
}

void halbb_bb_td_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct bb_dbg_info *dbg = &bb->bb_dbg_i;
	u32 val[10] = {0};
	u32 tmp = 0;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "pop_en {en} {phl_phy_idx}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "set_pd_low {val} {bw} {phl_phy_idx}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "per {phl_phy_idx}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "pfd set {type}\n");
		#ifdef HALBB_TDMA_CR_SUPPORT
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "tdma_cr en\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "tdma_cr cr {cr_idx} {cr_mask} {val_0} {val_1}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "tdma_cr period {p0_ms} {p1_ms}\n");
		#endif
		return;
	}

	HALBB_SCAN(input[2], DCMD_DECIMAL, &val[0]);
	HALBB_SCAN(input[3], DCMD_DECIMAL, &val[1]);
	HALBB_SCAN(input[4], DCMD_DECIMAL, &val[2]);

	if (_os_strcmp(input[1], "pop_en") == 0) {
		halbb_pop_en(bb, (bool)val[0], bb->bb_phy_idx);
	} else if (_os_strcmp(input[1], "set_pd_low") == 0) {
		halbb_set_pd_lower_bound(bb, (u8)val[0], (enum channel_width)val[1], bb->bb_phy_idx);
	#ifdef HALBB_TDMA_CR_SUPPORT
	} else if (_os_strcmp(input[1], "tdma_cr") == 0) {
		if (_os_strcmp(input[2], "en") == 0) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			if (dbg->tdma_cr_idx == 0) {
				BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Err tdma_cr_idx = 0\n");
				return;
			}

			dbg->tdma_cr_en = (bool)val[0];
			halbb_tdma_cr_sel_main(bb);
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "tdma_cr_en=%d\n", dbg->tdma_cr_en);
		} else if (_os_strcmp(input[2], "period") == 0) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			HALBB_SCAN(input[4], DCMD_DECIMAL, &val[1]);
			dbg->tdma_cr_period_0 = val[0];
			dbg->tdma_cr_period_1 = val[1];
			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "period 0/1= {%d, %d} ms\n",
				    dbg->tdma_cr_period_0,
				    dbg->tdma_cr_period_1);
		} else if (_os_strcmp(input[2], "cr") == 0) {
			HALBB_SCAN(input[3], DCMD_HEX, &val[0]);
			HALBB_SCAN(input[4], DCMD_HEX, &val[1]);
			HALBB_SCAN(input[5], DCMD_HEX, &val[2]);
			HALBB_SCAN(input[6], DCMD_HEX, &val[3]);

			dbg->tdma_cr_idx = val[0];
			dbg->tdma_cr_mask = val[1];
			dbg->tdma_cr_val_0 = val[2];
			dbg->tdma_cr_val_1 = val[3];

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "Reg 0x%x[0x%x] = {0x%x, 0x%x}\n",
				    dbg->tdma_cr_idx, dbg->tdma_cr_mask,
				    dbg->tdma_cr_val_0, dbg->tdma_cr_val_1);
		}
	#endif
	#ifdef BB_8852A_2_SUPPORT
	} else if (_os_strcmp(input[1], "per") == 0) {
		halbb_get_per_8852a_2(bb, bb->bb_phy_idx);
	} else if (_os_strcmp(input[1], "pfd") == 0) {
		if (bb->ic_type != BB_RTL8852A)
			return;

		if (_os_strcmp(input[2], "set") == 0) {
			HALBB_SCAN(input[3], DCMD_DECIMAL, &val[0]);
			halbb_manual_pkt_fmt_sel_8852a_2(bb, (enum bb_pfd_fmt_type)val[0]);

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "pfd_fmt_type=%d\n", val[0]);
		} 
	#endif
	}
}

void halbb_bb_fd_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	u32 val[10] = {0};

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "{fd dbg}\n");
		return;
	}
}

/*void halbb_fw_trace_en_h2c(struct bb_info *bb, bool enable,
			   u32 fw_dbg_comp, u32 monitor_mode, u32 macid)
{
	u8 h2c_parameter[7] = {0};
	u8 cmd_length;

	h2c_parameter[0] = enable;
	h2c_parameter[1] = (u8)(fw_dbg_comp & MASKBYTE0);
	h2c_parameter[2] = (u8)((fw_dbg_comp & MASKBYTE1) >> 8);
	h2c_parameter[3] = (u8)((fw_dbg_comp & MASKBYTE2) >> 16);
	h2c_parameter[4] = (u8)((fw_dbg_comp & MASKBYTE3) >> 24);
	h2c_parameter[5] = (u8)monitor_mode;
	h2c_parameter[6] = (u8)macid;
	cmd_length = 7;

	BB_DBG(bb, DBG_FW_INFO,
		"[H2C] FW_debug_en: (( %d )), mode: (( %d )), macid: (( %d ))\n",
		enable, monitor_mode, macid);

	//odm_fill_h2c_cmd(bb, PHYDM_H2C_FW_TRACE_EN, cmd_length, h2c_parameter);
}*/

void halbb_cmn_msg_setting(struct bb_info *bb, char input[][16], u32 *_used,
			   char *output, u32 *_out_len)
{
	u32 val[3] = {0};

	if (_os_strcmp(input[2], "period") == 0) {
		HALBB_SCAN(input[3], DCMD_HEX, &val[0]);
		bb->cmn_dbg_msg_period = (u8)val[0];

		if (bb->cmn_dbg_msg_period < HALBB_WATCHDOG_PERIOD)
			bb->cmn_dbg_msg_period = HALBB_WATCHDOG_PERIOD;

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Cmn_Msg] Period=%d\n", bb->cmn_dbg_msg_period);
	} else if (_os_strcmp(input[2], "comp") == 0) {
		HALBB_SCAN(input[3], DCMD_HEX, &val[0]);
		bb->cmn_dbg_msg_component = (u16)val[0];
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Cmn_Msg] Component=0x%x\n", bb->cmn_dbg_msg_component);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "Err\n");
	}
}

void halbb_trace_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	u64 pre_debug_components, one = 1;
	u64 comp = 0;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val[10] = {0};
	u8 i = 0;

	for (i = 0; i < 5; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &val[i]);
	}
	comp = bb->dbg_component;
	pre_debug_components = bb->dbg_component;

	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "\n================================\n");
	if (val[0] == 100 ||
	    (_os_strcmp(input[1], "-h") == 0)) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "{dbg_comp:0~31} {1:en, 2,dis}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "clean\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "30 period {sec}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "30 comp {val}\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "[DBG MSG] Component Selection\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "================================\n");
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "00. (( %s ))RA\n",
			 ((comp & DBG_RA) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "01. (( %s ))FA_CNT\n",
			 ((comp & DBG_FA_CNT) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "02. (( %s ))RSSI_MNTR\n",
			 ((comp & DBG_HALBB_FUN_RSVD_2) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "03. (( %s ))DFS\n",
			 ((comp & DBG_DFS) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "04. (( %s ))EDCCA\n",
			 ((comp & DBG_EDCCA) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "05. (( %s ))ENV_MNTR\n",
			 ((comp & DBG_ENV_MNTR) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "06. (( %s ))CFO_TRK\n",
			 ((comp & DBG_CFO_TRK) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "07. (( %s ))PWR_CTRL\n",
			 ((comp & DBG_PWR_CTRL) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "08. (( %s ))RUA_TBL\n",
			 ((comp & DBG_RUA_TBL) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "09. (( %s ))F_AUTO_DBG\n",
			 ((comp & DBG_AUTO_DBG) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "10. (( %s ))ANT_DIV\n",
			 ((comp & DBG_ANT_DIV) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "11. (( %s ))DIG\n",
			 ((comp & DBG_DIG) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "12. (( %s ))PATH_DIV\n",
			 ((comp & DBG_PATH_DIV) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "13. (( %s ))UL_TB_CTRL\n",
			 ((comp & BIT(13)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "14. (( %s ))TBD\n",
			 ((comp & BIT(14)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "15. (( %s ))TBD\n",
			 ((comp & BIT(15)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "16. (( %s ))TBD\n",
			 ((comp & BIT(16)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "17. (( %s ))TBD\n",
			 ((comp & BIT(17)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "18. (( %s ))SNIFFER\n",
			 ((comp & BIT(18)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "19. (( %s ))CH_INFO\n",
			 ((comp & BIT(19)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "20. (( %s ))PHY_STATUS\n",
			 ((comp & DBG_PHY_STS) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "21. (( %s ))TBD\n",
			 ((comp & BIT(21)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "22. (( %s ))FW_INFO\n",
			 ((comp & DBG_FW_INFO) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "23. (( %s ))COMMON_FLOW\n",
			 ((comp & DBG_COMMON_FLOW) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "24. (( %s ))IC_API\n",
			 ((comp & DBG_IC_API) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "25. (( %s ))DBG_API\n",
			 ((comp & DBG_DBG_API) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "26. (( %s ))DBCC\n",
			 ((comp & DBG_DBCC) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "27. (( %s ))DM_SUMMARY\n",
			 ((comp & DBG_DM_SUMMARY) ? ("V") : (".")));		
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "28. (( %s ))PHY_CONFIG\n",
			 ((comp & DBG_PHY_CONFIG) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "29. (( %s ))INIT\n",
			 ((comp & DBG_INIT) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "30. (( %s ))COMMON\n",
			 ((comp & DBG_CMN) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "    *period {%d sec}\n", bb->cmn_dbg_msg_period);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			    "    *comp {0x%x}\n", bb->cmn_dbg_msg_component);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "31. (( %s ))TBD\n",
			 ((comp & BIT(31)) ? ("V") : (".")));
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "================================\n");

	} else if (val[0] == 101 ||
		(_os_strcmp(input[1], "clean") == 0)) {
		bb->dbg_component = 0;
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Disable all debug components\n");
	} else {
		if (val[1] == 1) /*@enable*/
			bb->dbg_component |= (one << val[0]);
		else if (val[1] == 2) /*@disable*/
			bb->dbg_component &= ~(one << val[0]);
		else {
			if (BIT(val[0]) == DBG_CMN) {
				halbb_cmn_msg_setting(bb, input, &used, output, &out_len);
			} else {
				BB_DBG_CNSL(out_len, used, output + used, out_len - used,
					 "[Warning]  1:on,  2:off\n");
			}
		}
	}
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "pre-DbgComponents = 0x%llx\n", pre_debug_components);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "Curr-DbgComponents = 0x%llx\n", bb->dbg_component);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		 "================================\n");

	*_used = used;
	*_out_len = out_len;
}

u32 halbb_get_multiple(u8 pow, u8 base)
{
	u8 i;
	u32 return_value = 1;

	for (i = 0; i < pow; i++)
		return_value *= base; /*base ^ pow*/

	return	return_value;
}

u32 halbb_str_2_dec(u8 val)
{
	if (val >= 0x30 && val <= 0x39) /*0~9*/
		return	(val - 0x30);
	else if (val >= 0x41 && val <= 0x46) /*A~F*/
		return	(val - 0x41 + 10);
	else if (val >= 0x61 && val <= 0x66) /*a~f*/
		return	(val - 0x61 + 10);
	else
		return	1;
}

void halbb_scanf(char *in, enum bb_scanf_type type, u32 *out)
{
	char buff[DCMD_SCAN_LIMIT];
	u32 multiple = 1;
	u8 text_num = 0;
	u8 base = 10;
	u8 i = 0, j = 0;

	*out = 0;

	for (i = 0; i < DCMD_SCAN_LIMIT; i++) {
		/*BB_TRACE("pInput[%d] = %x\n", i, in[i]);*/
		if (in[i] != 0x0) {	/* 0x0 = NULL. */
			buff[i] = in[i];
			continue;
		}

		if (type == DCMD2_CHAR) {
			*out = *in;
			break;
		}

		base =  (type == DCMD2_DECIMAL) ? 10 : 16;
		text_num = i;

		for (j = 0; j < text_num; j++) {
			/*BB_TRACE("text_num=%d, sbuff[j]=%d, j=%d, value=%d\n",text_num, buff[j], j, base);*/
			multiple = halbb_get_multiple(text_num - 1 - j, base);
			*out += halbb_str_2_dec(buff[j]) * multiple;
			/*BB_TRACE("[%d]*pOutput = %d\n", j, *out);*/
		}
		break;
	}
}

void halbb_cmd_parser(struct bb_info *bb_0, char input_in[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct bb_info *bb = bb_0;
	struct bb_echo_cmd_info	*echo_cmd = &bb_0->bb_cmn_hooker->bb_echo_cmd_i;
	char input[MAX_ARGC][MAX_ARGV];
	u32 used = 0;
	u8 id = 0;
	u32 var1[10] = {0};
	u32 i;
	u32 halbb_ary_size = echo_cmd->cmd_size;
	u32 directory = 0;
	char char_temp = ' ';
	enum phl_phy_idx  phy_idx_tmp = HW_PHY_0;

	//BB_TRACE("[IN ] %s %s %s %s %s\n", input_in[0], input_in[1], input_in[2], input_in[3], input_in[4]);

	if (0 == halbb_mem_cmp(bb_0, input_in[0], (void*)"bb", 2)) {
		phy_idx_tmp = (0 == _os_strcmp((input_in[0] + 2), "1")) ? HW_PHY_1 : HW_PHY_0;
		BB_TRACE("phy_idx_tmp = %d\n", phy_idx_tmp);
		halbb_mem_cpy(bb_0, input, &input_in[1], ((MAX_ARGC - 1) * MAX_ARGV));
	} else if (0 == halbb_mem_cmp(bb_0, input_in[0], (void*)"0", 1)) {
		phy_idx_tmp = HW_PHY_0;
		halbb_mem_cpy(bb_0, input, &input_in[1], ((MAX_ARGC - 1) * MAX_ARGV));
	} else if (0 == halbb_mem_cmp(bb_0, input_in[0], (void*)"1", 1)) {
		phy_idx_tmp = HW_PHY_1;
		halbb_mem_cpy(bb_0, input, &input_in[1], ((MAX_ARGC - 1) * MAX_ARGV));
	} else {
		halbb_mem_cpy(bb_0, input, &input_in[0], (MAX_ARGC * MAX_ARGV));
	}

	//BB_TRACE("[OUT] %s %s %s %s %s\n", input[0], input[1], input[2], input[3], input[4]);

	#ifdef HALBB_DBCC_SUPPORT
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "->\n");
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx_tmp);
	BB_DBG_CNSL(out_len, used, output + used, out_len - used,
		    "[%d] echo phy_idx=%d\n", bb->bb_phy_idx, phy_idx_tmp);
	#endif

	if (halbb_ary_size == 0)
		return;

	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < halbb_ary_size; i++) {
			if (_os_strcmp(halbb_cmd_i[i].name, input[0]) == 0) {
				id = halbb_cmd_i[i].id;

				BB_DBG_CNSL(out_len, used, output + used, out_len - used,
					    "[%s]===>\n", halbb_cmd_i[i].name);
				break;
			}
		}
		if (i == halbb_ary_size) {
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "HALBB command not found!\n");
			return;
		}
	}

	switch (id) {
	case HALBB_HELP: {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "BB cmd ==>\n");

		for (i = 0; i < halbb_ary_size - 2; i++)
			BB_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %-5d: %s\n", i, halbb_cmd_i[i + 2].name);
	} break;

	case HALBB_DEMO:
		/*echo bb demo 12 3b abcde -10*/

		HALBB_SCAN(input[1], DCMD_DECIMAL, &directory);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Decimal value = %d\n", directory);

		HALBB_SCAN(input[2], DCMD_HEX, &directory);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Hex value = 0x%x\n", directory);

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "String = %s\n", input[3]);

		HALBB_SCAN(input[4], "%d", &directory);
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "value_u32 = %d\n", (u32)directory);

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "value_s32 = %d\n", (s32)directory);

		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "value_hex = 0x%x\n", directory);
		break;

	case HALBB_CMN_DBG:
		halbb_cmn_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_REG_RW:
		halbb_bbcr_rw_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_STASISTICS:
		#ifdef HALBB_STATISTICS_SUPPORT
		halbb_crc32_cnt_dbg(bb, input, &used, output, &out_len);
		#endif
		break;

	case HALBB_DBG_PORT:
		halbb_dbgport_dbg(bb, input, &used, output, &out_len);
		break;

	#ifdef HALBB_RA_SUPPORT	
	case HALBB_RA:
		halbb_ra_dbg(bb, input, &used, output, &out_len);
		break;
	#endif

	case HALBB_TRACE:
		halbb_trace_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_MP_DBG:
		halbb_mp_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_SUPPORT_ABILITY:
		halbb_supportability_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_IC_API:
		halbb_ic_api_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_SPUR_SUPP:
		halbb_spur_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_PROFILE:
		halbb_basic_profile_dbg(bb, &used, output, &out_len);
		break;
	case HALBB_TX_PW:
		halbb_pwr_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_LA_MODE:
		#ifdef HALBB_LA_MODE_SUPPORT
		halbb_la_cmd_dbg(bb, input, &used, output, &out_len);
		#endif
		break;

	case HALBB_PSD:
		#ifdef HALBB_PSD_SUPPORT
		halbb_psd_dbg(bb, input, &used, output, &out_len);
		#endif
		break;

	case HALBB_DUMP_REG:
		halbb_dump_reg_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_CFO_TRK:
		#ifdef HALBB_CFO_TRK_SUPPORT
		halbb_cfo_trk_dbg(bb, input, &used, output, &out_len);
		#endif
		break;

	case HALBB_UL_TB:
		#ifdef HALBB_UL_TB_CTRL_SUPPORT
		halbb_ul_tb_dbg(bb, input, &used, output, &out_len);
		#endif
		break;

#if 0
	case HALBB_AUTO_DBG:
		#ifdef HALBB_AUTO_DEGBUG
		halbb_auto_dbg_console(bb, input, &used, output, &out_len);
		#endif
		break;
#endif

	case HALBB_DD_DBG:
		halbb_dd_dump_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_SHOW_RXRATE:
		halbb_show_rx_rate(bb, input, &used, output, &out_len);
		break;
#if 0
	case HALBB_NBI_EN:
		halbb_nbi_debug(bb, input, &used, output, &out_len);
		break;

	case HALBB_CSI_MASK_EN:
		halbb_csi_debug(bb, input, &used, output, &out_len);
		break;
#endif
	
	case HALBB_DFS_DBG:
		#ifdef HALBB_DFS_SUPPORT
		halbb_dfs_debug(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_DIG:
		#ifdef HALBB_DIG_SUPPORT
		halbb_dig_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_NHM:
		#ifdef NHM_SUPPORT
		halbb_nhm_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_CLM:
		#ifdef CLM_SUPPORT
		halbb_clm_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_IFS_CLM:
		#ifdef IFS_CLM_SUPPORT
		halbb_ifs_clm_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_FAHM:
		#ifdef FAHM_SUPPORT
		halbb_fahm_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_EDCCA_CLM:
		#ifdef EDCCA_CLM_SUPPORT
		halbb_edcca_clm_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_EDCCA:
		#ifdef HALBB_EDCCA_SUPPORT
		halbb_edcca_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
	case HALBB_ENV_MNTR:
		#ifdef HALBB_ENV_MNTR_SUPPORT
		halbb_env_mntr_dbg(bb, input, &used, output, &out_len);
		#endif
		break;
#if 0
	case HALBB_BB_INFO:
		halbb_bb_hw_dbg_info(bb, input, &used, output, &out_len);
		break;
	
	case HALBB_H2C:
		halbb_h2C_debug(bb, input, &used, output, &out_len);
		break;

	case HALBB_ADAPTIVITY_DBG:
		#ifdef HALBB_SUPPORT_ADAPTIVITY
		halbb_adaptivity_debug(bb, input, &used, output, &out_len);
		#endif
		break;
#endif
	case HALBB_STA_INFO:
		halbb_sta_info_dbg(bb, input, &used, output, &out_len);
		break;

	case HALBB_PAUSE_FUNC:
		halbb_pause_func_dbg(bb, input, &used, output, &out_len);
		break;
#if 0
	case HALBB_PER_TONE_EVM:
		halbb_per_tone_evm(bb, input, &used, output, &out_len);
		break;

	#ifdef CONFIG_DYNAMIC_TX_TWR
	case HALBB_DYN_TXPWR:
		halbb_dtp_debug(bb, input, &used, output, &out_len);
		break;
	#endif
#endif
	case HALBB_PHY_STATUS:
		halbb_physts_dbg(bb, input, &used, output, &out_len);
		break;
#if 0
#ifdef HALBB_DCC_ENHANCE
	case HALBB_DCC:
		halbb_dig_cckpd_coex_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#endif
#ifdef HALBB_PMAC_TX_SETTING_SUPPORT
	case HALBB_PMAC_TX:
		halbb_pmac_tx_dbg(bb, input, &used, output, &out_len);
		break;
#endif
	case HALBB_FW_DBG:
		halbb_fw_dbg(bb, input, &used, output, &out_len);
		break;
#ifdef HALBB_CH_INFO_SUPPORT
	case HALBB_CH_INFO:
		halbb_ch_info_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#ifdef HALBB_RUA_SUPPORT
	case HALBB_RUA_TBL:
		halbb_rua_tbl_dbg(bb, input, &used, output, &out_len);
		break;
#endif
	case HALBB_TD_CFG:
		halbb_bb_td_dbg(bb, input, &used, output, &out_len);
		break;
	case HALBB_FD_CFG:
		halbb_bb_fd_dbg(bb, input, &used, output, &out_len);
		break;
#ifdef HALBB_DBCC_SUPPORT
	case HALBB_DBCC:
		halbb_dbcc_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#ifdef HALBB_ANT_DIV_SUPPORT
	case HALBB_ANT_DIV:
			halbb_antdiv_dbg(bb, input, &used, output, &out_len);
			break;
#endif
#ifdef HALBB_DYN_CSI_RSP_SUPPORT
	case HALBB_DCR_DBG:
		halbb_dyn_csi_rsp_dbg(bb, input, &used, output, &out_len);
		break;
#endif
	case HALBB_RX_GAIN_TABLE:
		halbb_rx_gain_table_dbg(bb, input, &used, output, &out_len);
		break;
	case HALBB_RX_OP1DB_TABLE:
		halbb_rx_op1db_table_dbg(bb, input, &used, output, &out_len);
		break;
	case HALBB_HW_SETTING:
		halbb_ic_hw_setting_dbg(bb, input, &used, output, &out_len);
		break;
#ifdef HALBB_PATH_DIV_SUPPORT
	case HALBB_PATH_DIV:
		halbb_pathdiv_dbg(bb, input, &used, output, &out_len);
		break;
#endif

	case HALBB_DTP:
		halbb_pwr_ctrl_dbg(bb, input, &used, output, &out_len);
		break;
	case HALBB_TX_INFO:
		halbb_basic_dbg_msg_tx_dbg_reg_cnsl(bb, &used, output, &out_len);
		break;
#ifdef HALBB_DYN_1R_CCA_SUPPORT
	case HALBB_DYN_1R_CCA:
		halbb_dyn_1r_cca_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#ifdef	HALBB_CNSL_CMN_INFO_SUPPORT
	case HALBB_CMN_INFO:
		halbb_basic_dbg_message_cnsl_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#ifdef HALBB_SNIF_SUPPORT
	case HALBB_SNIFFER_MODE:
		halbb_snif_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#ifdef HALBB_DYN_DTR_SUPPORT
	case HALBB_DTR_DBG:
		halbb_dyn_dtr_dbg(bb, input, &used, output, &out_len);
		break;
#endif
#ifdef HALBB_FW_OFLD_SUPPORT
	case HALBB_FW_OFLD:
		halbb_fw_ofld_dbg(bb, input, &used, output, &out_len);
		break;
#endif

	case HALBB_MATH:
		halbb_math_dbg(bb, input, &used, output, &out_len);
		break;
	default:
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Do not support this command\n");
		break;
	}

	//BB_DBG_CNSL(out_len, used, output + used, out_len - used,
	//	    "[OUT] bb_phy_idx=%d\n", bb->bb_phy_idx);
	
}

void halbb_cmd_parser_init(struct bb_info *bb)
{
	struct bb_echo_cmd_info	*echo_cmd = &bb->bb_cmn_hooker->bb_echo_cmd_i;

	echo_cmd->cmd_size = sizeof(halbb_cmd_i) / sizeof(struct halbb_cmd_info);
	echo_cmd->echo_phy_idx = HW_PHY_0;
}

s32 halbb_cmd(struct bb_info *bb, char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = _os_strsep(&input, ", ");
		if (token) {
			if (_os_strlen(token) <= MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);

	halbb_cmd_parser(bb, argv, argc, output, out_len);

	return 0;
}

void halbb_fwdbg_trace(struct bb_info *bb, u32 dbg_comp, u8 fw_trace_en)
{
	struct bb_fw_dbg_cmn_info *bb_fwdbg = &bb->bb_fwdbg_i;
	u32 *bb_h2c = (u32 *) bb_fwdbg;
	u8 cmdlen = sizeof(struct bb_fw_dbg_cmn_info);
	bool ret_val = false;
	u8 ret_v0, ret_v1, ret_v2;
	
	/* Set fwdbg api, mac api need driver package*/
	ret_v0 = rtw_hal_fw_log_cfg(bb->hal_com, FL_CFG_OP_SET, FL_CFG_TYPE_LEVEL, FL_LV_LOUD);
	ret_v1 = rtw_hal_fw_log_cfg(bb->hal_com, FL_CFG_OP_SET, FL_CFG_TYPE_OUTPUT, FL_OP_C2H);
	ret_v2 = rtw_hal_fw_log_cfg(bb->hal_com, FL_CFG_OP_SET, FL_CFG_TYPE_COMP, FL_COMP_BB);
	/* Set fwbb debug component */
	bb_fwdbg->fw_dbg_comp[0] = (u8) (dbg_comp&0x000000ff);
	bb_fwdbg->fw_dbg_comp[1] = (u8) (dbg_comp&0x0000ff00);
	bb_fwdbg->fw_dbg_comp[2] = (u8) (dbg_comp&0x00ff0000);
	bb_fwdbg->fw_dbg_comp[3] = (u8) (dbg_comp&0xff000000);
	bb_fwdbg->fw_dbg_trace = fw_trace_en;
	BB_DBG(bb, DBG_FW_INFO, "FW TRACE: %x %x\n", bb_h2c[0], bb_h2c[1]);
	ret_val = halbb_fill_h2c_cmd(bb, cmdlen, DM_H2C_FWTRACE, HALBB_H2C_DM, bb_h2c);
}

void halbb_fw_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	char help[] = "-h";
	u8 i;
	u32 val[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;

	if (_os_strcmp(input[1], help) == 0) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "{fw_dbg (dbg_trace=> 1:enable, 2:disable) (dbg_comp)}\n");
		goto out;
	}
	for (i = 0; i < 5; i++) {
		if (input[i + 1])
			HALBB_SCAN(input[i + 1], DCMD_DECIMAL, &val[i]);
	}
	if (val[0] == 1) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[fw_dbg] Enable : dbg_comp = %x\n", val[1]);
		halbb_fwdbg_trace(bb, val[1], 1);
	} else if (val[0] == 2) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[fw_dbg] Disable : dbg_comp clear\n");
		halbb_fwdbg_trace(bb, 0, 0);
	} else if (val[0] == 3) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[fw_dbg] Trigger h2c ==> c2h debug\n");
		halbb_test_h2c_c2h_flow(bb);
	} else if (val[0] == 4) {
		BB_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[fw_dbg] Set h2c get tx statistic\n");
		rtw_halbb_query_txsts(bb, (u16)val[1], (u16)val[2]);
	}

out:
	*_used = used;
	*_out_len = out_len;
} 

