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
#ifndef __HALBB_PRECOMP_H__
#define __HALBB_PRECOMP_H__

#include "halbb_cfg_ic.h"

/*---[Define Only] ----------------------------------------------------------*/
#include "../../hal_headers_le.h"
#include "halbb_types.h"
#include "halbb_features.h"
#include "halbb_ic_hw_info.h"
#include "halbb_ic_sw_info.h"
#include "halbb_interface.h"
#include "halbb_dbg.h"
#include "halbb_dbg_cnsl_out.h"
#include "halbb_rua_tbl_ex.h"

#if (PLATFOM_IS_LITTLE_ENDIAN)
	#include "halbb_physts_ie_l_endian.h"
	#include "halbb_physts_ie_l_endian_7.h"
	#include "halbb_ra_l_endian.h"
	#include "halbb_rua_tbl_l_endian.h"
	#include "halbb_plcp_tx_l_endian.h"
#else
	#include "halbb_physts_ie_b_endian.h"
	#include "halbb_physts_ie_b_endian_7.h"
	#include "halbb_ra_b_endian.h"
	#include "halbb_rua_tbl_b_endian.h"
	#include "halbb_plcp_tx_b_endian.h"
#endif

/*---[Include structure & prototype] ----------------------------------------*/
#ifdef BB_8852A_2_SUPPORT
	#include "halbb_8852a_2/halbb_cr_info_8852a_2.h"
	#include "halbb_8852a_2/halbb_8852a_2.h"
	#include "halbb_8852a_2/halbb_8852a_2_api.h"
	#include "halbb_8852a_2/halbb_8852a_2_api_ex.h"
	#include "halbb_8852a_2/halbb_hwimg_8852a_2.h"
	#include "halbb_8852a_2/halbb_reg_cfg_8852a_2.h"
	#include "halbb_8852a_2/halbb_version_rtl8852a_2.h"
#endif

#ifdef BB_8852B_SUPPORT
	#include "halbb_8852b/halbb_cr_info_8852b.h"
	#include "halbb_8852b/halbb_8852b.h"
	#include "halbb_8852b/halbb_8852b_api.h"
	#include "halbb_8852b/halbb_8852b_api_ex.h"
	#include "halbb_8852b/halbb_8852b_fwofld_api.h"
	#include "halbb_8852b/halbb_hwimg_8852b.h"
	#include "halbb_8852b/halbb_reg_cfg_8852b.h"
	#include "halbb_8852b/halbb_version_rtl8852b.h"
#endif

#ifdef BB_8852C_SUPPORT
	#include "halbb_8852c/halbb_cr_info_8852c.h"
	#include "halbb_8852c/halbb_8852c.h"
	#include "halbb_8852c/halbb_8852c_api.h"
	#include "halbb_8852c/halbb_8852c_api_ex.h"
	#include "halbb_8852c/halbb_8852c_fwofld_api.h"
	#include "halbb_8852c/halbb_hwimg_8852c.h"
	#include "halbb_8852c/halbb_reg_cfg_8852c.h"
	#include "halbb_8852c/halbb_version_rtl8852c.h"
	#include "halbb_8852c/halbb_8852c_dcra.h"
	#include "halbb_8852c/halbb_8852c_dcra_ex.h"
#endif

#ifdef BB_8192XB_SUPPORT
	#include "halbb_8192xb/halbb_cr_info_8192xb.h"
	#include "halbb_8192xb/halbb_8192xb.h"
	#include "halbb_8192xb/halbb_8192xb_api.h"
	#include "halbb_8192xb/halbb_8192xb_api_ex.h"
	#include "halbb_8192xb/halbb_hwimg_8192xb.h"
	#include "halbb_8192xb/halbb_reg_cfg_8192xb.h"
	#include "halbb_8192xb/halbb_version_rtl8192xb.h"
#endif

#ifdef BB_8922A_SUPPORT
	#include "halbb_8922a/halbb_cr_info_8922a.h"
	#include "halbb_8922a/halbb_8922a.h"
	#include "halbb_8922a/halbb_8922a_api.h"
        #include "halbb_8922a/halbb_hwimg_8922a.h"
        #include "halbb_8922a/halbb_reg_cfg_8922a.h"
        #include "halbb_8922a/halbb_version_rtl8922a.h"
	#include "bbmcu/bbmcu_precomp.h"
#endif

#ifdef BB_8851B_SUPPORT
	#include "halbb_8851b/halbb_cr_info_8851b.h"
	#include "halbb_8851b/halbb_8851b.h"
	#include "halbb_8851b/halbb_8851b_api.h"
	#include "halbb_8851b/halbb_8851b_api_ex.h"
	#include "halbb_8851b/halbb_8851b_fwofld_api.h"
	#include "halbb_8851b/halbb_hwimg_8851b.h"
	#include "halbb_8851b/halbb_reg_cfg_8851b.h"
	#include "halbb_8851b/halbb_version_rtl8851b.h"
#endif

#include "halbb_ra.h"
#include "halbb_ra_ex.h"
#include "halbb_hw_cfg.h"
#include "halbb_hw_cfg_ex.h"
#include "halbb_api.h"
#include "halbb_api_ex.h"
#include "halbb_interface_ex.h"
#include "halbb_math_lib_ex.h"
#include "halbb_math_lib.h"
#include "halbb_dbg_cmd.h"
#include "halbb_dbg_cmd_ex.h"
#include "halbb_physts_ex.h"
#include "halbb_physts.h"
#include "halbb_physts_7.h"
#include "halbb_cmn_rpt_ex.h"
#include "halbb_cmn_rpt.h"
#include "halbb_init.h"
#include "halbb_init_ex.h"
#include "halbb_pmac_setting.h"
#include "halbb_pmac_setting_ex.h"
#include "halbb_outsrc_def.h"
#include "halbb_plcp_tx.h"
#include "halbb_plcp_tx_7.h"
#include "halbb_plcp_gen.h"
#include "halbb_la_mode_ex.h"
#include "halbb_la_mode.h"
#include "halbb_psd.h"
#include "halbb_ul_tb_ctrl.h"
#include "halbb_pwr_ctrl_ex.h"
#include "halbb_pwr_ctrl.h"
#include "halbb_mp_ex.h"
#include "halbb_dfs.h"
#include "halbb_dfs_ex.h"
#include "halbb_cfo_trk.h"
#include "halbb_edcca.h"
#include "halbb_edcca_ex.h"
#include "halbb_env_mntr_ex.h"
#include "halbb_env_mntr.h"
#include "halbb_dig_ex.h"
#include "halbb_dig.h"
#include "halbb_dyn_csi_rsp.h"
#include "halbb_dyn_csi_rsp_ex.h"
#include "halbb_ant_div.h"
#include "halbb_path_div.h"
#include "halbb_statistics.h"
#include "halbb_statistics_ex.h"
#include "halbb_dbg_cnsl_out_ex.h"
#include "halbb_ch_info_ex.h"
#include "halbb_ch_info.h"
#include "halbb_auto_dbg.h"
#include "halbb_dbcc_ex.h"
#include "halbb_dbcc.h"
#include "halbb_snif_ex.h"
#include "halbb_snif.h"
#include "halbb_rua_tbl.h"
#include "halbb_spur_suppress.h"
#include "halbb_fwofld.h"
#include "halbb_fwofld_ex.h"
#include "halbb_dyn_1r_cca.h"
#include "halbb_dyn_dtr.h"
#include "halbb_ex.h"
#include "halbb.h"

#endif
