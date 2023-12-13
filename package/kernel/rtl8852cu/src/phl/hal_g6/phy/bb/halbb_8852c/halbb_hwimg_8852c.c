/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

#include "../halbb_precomp.h"
#include "halbb_hwimg_raw_data_8852c.h"

#ifdef BB_8852C_SUPPORT

bool halbb_sel_headline_8852c(struct bb_info *bb, u32 *array, u32 array_len,
			      u8 *headline_size, u8 *headline_idx)
{
	bool case_match = false;
	u32 cut_drv = (u32)bb->hal_com->cv;
	u32 rfe_drv = (u32)bb->phl_com->dev_cap.rfe_type;
	u32 cut_para = 0, rfe_para = 0;
	u32 compare_target = 0;
	u32 cut_max = 0;
	u32 i = 0;

	*headline_idx = 0;
	*headline_size = 0;

	if (bb->bb_dbg_i.cr_dbg_mode_en) {
		rfe_drv = bb->bb_dbg_i.rfe_type_curr_dbg;
		cut_drv = bb->bb_dbg_i.cut_curr_dbg;
	}

	BB_DBG(bb, DBG_INIT, "{RFE, Cart}={%d, %d}, dbg_en=%d\n",
	       rfe_drv, cut_drv, bb->bb_dbg_i.cr_dbg_mode_en);

	while ((i + 1) < array_len) {
		if ((array[i] >> 28) != 0xf) {
			*headline_size = (u8)i;
			break;
		}
		BB_DBG(bb, DBG_INIT, "array[%02d]=0x%08x, array[%02d]=0x%08x\n",
		       i, array[i], i+1, array[i+1]);
		i += 2;
	}

	BB_DBG(bb, DBG_INIT, "headline_size=%d\n", i);

	if (i == 0)
		return true;

	/*case_idx:1 {RFE:Match, CUT:Match}*/
	compare_target = ((rfe_drv & 0xff) << 16) | (cut_drv & 0xff);
	BB_DBG(bb, DBG_INIT, "[1] CHK {RFE:Match, CUT:Match}\n");
	for (i = 0; i < *headline_size; i += 2) {
		if ((array[i] & 0x0fffffff) == compare_target) {
			*headline_idx = (u8)(i >> 1);
			return true;
		}
	}
	BB_DBG(bb, DBG_INIT, "\t fail\n");

	/*case_idx:2 {RFE:Match, CUT:Dont care}*/
	compare_target = ((rfe_drv & 0xff) << 16) | (DONT_CARE_8852C & 0xff);
	BB_DBG(bb, DBG_INIT, "[2] CHK {RFE:Match, CUT:Dont_Care}\n");
	for (i = 0; i < *headline_size; i += 2) {
		if ((array[i] & 0x0fffffff) == compare_target) {
			*headline_idx = (u8)(i >> 1);
			return true;
		}
	}
	BB_DBG(bb, DBG_INIT, "\t fail\n");

	/*case_idx:3 {RFE:Match, CUT:Max_in_table}*/
	BB_DBG(bb, DBG_INIT, "[3] CHK {RFE:Match, CUT:Max_in_Table}\n");
	for (i = 0; i < *headline_size; i += 2) {
		rfe_para = (array[i] & 0x00ff0000) >> 16; 
		cut_para = array[i] & 0x0ff;
		if (rfe_para == rfe_drv) {
			if (cut_para >= cut_max) {
				cut_max = cut_para;
				*headline_idx = (u8)(i >> 1);
				BB_DBG(bb, DBG_INIT, "cut_max:%d\n", cut_max);
				case_match = true;
			}
		}
	}
	if (case_match) {
		return true;
	}
	BB_DBG(bb, DBG_INIT, "\t fail\n");

	/*case_idx:4 {RFE:Dont Care, CUT:Max_in_table}*/
	BB_DBG(bb, DBG_INIT, "[4] CHK {RFE:Dont_Care, CUT:Max_in_Table}\n");
	for (i = 0; i < *headline_size; i += 2) {
		rfe_para = (array[i] & 0x00ff0000) >> 16; 
		cut_para = array[i] & 0x0ff;
		if (rfe_para == DONT_CARE_8852C) {
			if (cut_para >= cut_max) {
				cut_max = cut_para;
				*headline_idx = (u8)(i >> 1);
				BB_DBG(bb, DBG_INIT, "cut_max:%d\n", cut_max);
				case_match = true;
			}
		}
	}
	if (case_match) {
		return true;
	}
	BB_DBG(bb, DBG_INIT, "\t fail\n");

	/*case_idx:5 {RFE:Not_Match, CUT:Not_Match}*/
	BB_DBG(bb, DBG_INIT, "[5] CHK {RFE:Not_Match, CUT:Not_Match}\n");
	BB_DBG(bb, DBG_INIT, "\t all fail\n");
	return false;
}

void halbb_flag_2_default_8852c(bool *is_matched, bool *find_target)
{
	*is_matched = true;
	*find_target = false;
}

bool halbb_cfg_bbcr_ax_8852c(struct bb_info *bb, bool is_form_folder,
			  u32 folder_len, u32 *folder_array,
			  enum phl_phy_idx phy_idx)
{
	bool is_matched, find_target;
	u32 cfg_target = 0, cfg_para = 0;
	u32 i = 0;
	u32 array_len = 0;
	u32 *array = NULL;
	u32 v1 = 0, v2 = 0;
	u8 h_size = 0;
	u8 h_idx = 0;
	bool ret = false;

	BB_DBG(bb, DBG_INIT, "===> %s\n", __func__);

	if (is_form_folder) {
		array_len = folder_len;
		array = folder_array;
	} else {
		array_len = sizeof(array_mp_8852c_phy_reg) / sizeof(u32);
		array = (u32 *)array_mp_8852c_phy_reg;
	}

	BB_DBG(bb, DBG_INIT, "form_folder=%d, len=%d, dbcc_en=%d, phy_idx=%d\n",
	       is_form_folder, array_len, bb->hal_com->dbcc_en, phy_idx);

	if (!halbb_sel_headline_8852c(bb, array, array_len, &h_size, &h_idx)) {
		BB_WARNING("[%s]Invalid BB CR Pkg\n", __func__);
		return false;
	}
	BB_DBG(bb, DBG_INIT, "h_size = %d, h_idx = %d\n", h_size, h_idx);

	if (h_size != 0) {
		cfg_target = array[h_idx << 1] & 0x0fffffff;
	}

	i += h_size;

	BB_DBG(bb, DBG_INIT, "cfg_target = 0x%x\n", cfg_target);
	BB_DBG(bb, DBG_INIT, "array[i] = 0x%x, array[i+1] = 0x%x\n", array[i], array[i + 1]);

	halbb_flag_2_default_8852c(&is_matched, &find_target);
	#ifdef HALBB_FW_OFLD_SUPPORT
	if (halbb_check_fw_ofld(bb))
		BB_WARNING("Becareful it is fwofld mode in BB init !!\n");
	#endif
	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];
		i += 2;

		switch (v1 >> 28) {
		case IF_8852C:
		case ELSE_IF_8852C:
			cfg_para = v1 & 0x0fffffff;
			BB_DBG(bb, DBG_INIT, "*if (rfe=%d, cart=%d)\n",
			       (cfg_para & 0xff0000) >> 16, cfg_para & 0xff);
			break;
		case ELSE_8852C:
			BB_DBG(bb, DBG_INIT, "*else\n");
			is_matched = false;
			if (!find_target) {
				BB_WARNING("Init BBCR Fail in Reg 0x%x\n", array[i]);
				return false;
			}
			break;
		case END_8852C:
			BB_DBG(bb, DBG_INIT, "*endif\n");
			halbb_flag_2_default_8852c(&is_matched, &find_target);
			break;
		case CHK_8852C:
			/*Check this para meets driver's requirement or not*/
			if (find_target) {
				BB_DBG(bb, DBG_INIT, "\t skip\n");
				is_matched = false;
				break;
			}

			if (cfg_para == cfg_target) {
				is_matched = true;
				find_target = true;
			} else {
				is_matched = false;
				find_target = false;
			}
			BB_DBG(bb, DBG_INIT, "\t match=%d\n", is_matched);
			break;
		default:
			if (is_matched) 
				#if 0//def HALBB_FW_OFLD_SUPPORT
				ret = halbb_fwcfg_bb_phy_8852c(bb, v1, v2, phy_idx);
				#else
				halbb_cfg_bb_phy_8852c(bb, v1, v2, phy_idx);
				#endif
			break;
		}
	}
	BB_DBG(bb, DBG_INIT, "BBCR Init Success\n\n");
	#ifdef HALBB_FW_OFLD_SUPPORT
	return ret;
	#else
	return true;
	#endif
}

bool halbb_cfg_bb_gain_ax_8852c(struct bb_info *bb, bool is_form_folder,
				  u32 folder_len, u32 *folder_array)
{
	bool is_matched, find_target;
	u32 cfg_target = 0, cfg_para = 0;
	u32 i = 0;
	u32 array_len = 0;
	u32 *array = NULL;
	u32 v1 = 0, v2 = 0;
	u8 h_size = 0;
	u8 h_idx = 0;

	BB_DBG(bb, DBG_INIT, "===> %s\n", __func__);

	if (is_form_folder) {
		array_len = folder_len;
		array = folder_array;
	} else {
		array_len = sizeof(array_mp_8852c_phy_reg_gain) / sizeof(u32);
		array = (u32 *)array_mp_8852c_phy_reg_gain;
	}

	BB_DBG(bb, DBG_INIT, "GAIN_TABLE_form_folder=%d, len=%d\n",
	       is_form_folder, array_len);

	if (!halbb_sel_headline_8852c(bb, array, array_len, &h_size, &h_idx)) {
		BB_WARNING("[%s]Invalid BB CR Pkg\n", __func__);
		return false;
	}
	BB_DBG(bb, DBG_INIT, "h_size = %d, h_idx = %d\n", h_size, h_idx);

	if (h_size != 0) {
		cfg_target = array[h_idx << 1] & 0x0fffffff;
	}

	i += h_size;

	BB_DBG(bb, DBG_INIT, "cfg_target = 0x%x\n", cfg_target);
	BB_DBG(bb, DBG_INIT, "array[i] = 0x%x, array[i+1] = 0x%x\n", array[i], array[i + 1]);

	halbb_flag_2_default_8852c(&is_matched, &find_target);
	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];
		i += 2;

		switch (v1 >> 28) {
		case IF_8852C:
		case ELSE_IF_8852C:
			cfg_para = v1 & 0x0fffffff;
			BB_DBG(bb, DBG_INIT, "*if (rfe=%d, cart=%d)\n",
			       (cfg_para & 0xff0000) >> 16, cfg_para & 0xff);
			break;
		case ELSE_8852C:
			BB_DBG(bb, DBG_INIT, "*else\n");
			is_matched = false;
			if (!find_target) {
				BB_WARNING("Init BBCR Fail in Reg 0x%x\n", array[i]);
				return false;
			}
			break;
		case END_8852C:
			BB_DBG(bb, DBG_INIT, "*endif\n");
			halbb_flag_2_default_8852c(&is_matched, &find_target);
			break;
		case CHK_8852C:
			/*Check this para meets driver's requirement or not*/
			if (find_target) {
				BB_DBG(bb, DBG_INIT, "\t skip\n");
				is_matched = false;
				break;
			}

			if (cfg_para == cfg_target) {
				is_matched = true;
				find_target = true;
			} else {
				is_matched = false;
				find_target = false;
			}
			BB_DBG(bb, DBG_INIT, "\t match=%d\n", is_matched);
			break;
		default:
			if (is_matched)
				halbb_cfg_bb_gain_8852c(bb, v1, v2);
			break;
		}
	}
	BB_DBG(bb, DBG_INIT, "BBCR gain Init Success\n\n");
	return true;
}

u32
halbb_get_8852c_phy_reg_ver(void)
{
		return (u32)BB_REG_RELEASE_VERSION_8852C;
}

#endif /* end of HWIMG_SUPPORT*/

