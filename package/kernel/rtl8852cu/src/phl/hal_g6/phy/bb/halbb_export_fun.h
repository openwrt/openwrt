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
#ifndef __HALBB_EXPORT_FUN_H__
#define __HALBB_EXPORT_FUN_H__
#include "halbb_cfg_ic.h"
#include "halbb_hw_cfg_ex.h"
#include "halbb_init_ex.h"
#include "halbb_ex.h"
#include "halbb_dbg_cmd_ex.h"
#include "halbb_physts_ex.h"
#include "halbb_api_ex.h"
#include "halbb_interface_ex.h"
#include "halbb_dfs_ex.h"
#include "halbb_dig_ex.h"
#include "halbb_mp_ex.h"
#include "halbb_outsrc_def.h"
#include "halbb_pmac_setting_ex.h"
#include "halbb_la_mode_ex.h"
#include "halbb_ra_ex.h"
#include "halbb_cmn_rpt_ex.h"
#include "halbb_ch_info_ex.h"
#include "halbb_math_lib_ex.h"
#include "halbb_edcca_ex.h"
#include "halbb_dbcc_ex.h"
#include "halbb_rua_tbl_ex.h"
#include "halbb_env_mntr_ex.h"
#include "halbb_pwr_ctrl_ex.h"
#include "halbb_dyn_csi_rsp_ex.h"
#include "halbb_dbcc_ex.h"
#include "halbb_snif_ex.h"
#include "halbb_fwofld_ex.h"
#include "halbb_statistics_ex.h"
#include "halbb_dbg_cnsl_out_ex.h"
#ifdef BB_8852A_2_SUPPORT
	#include "halbb_8852a_2/halbb_8852a_2_api_ex.h"
#endif
#ifdef BB_8852B_SUPPORT
	#include "halbb_8852b/halbb_8852b_api_ex.h"
#endif
#ifdef BB_8852C_SUPPORT
	#include "halbb_8852c/halbb_8852c_api_ex.h"
	#include "halbb_8852c/halbb_8852c_dcra_ex.h"
#endif
#ifdef BB_8192XB_SUPPORT
	#include "halbb_8192xb/halbb_8192xb_api_ex.h"
#endif
#ifdef BB_8922A_SUPPORT
#include "bbmcu/bbmcu_export_fun.h"
#endif
#endif

