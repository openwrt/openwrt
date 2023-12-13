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
#ifndef __HALRF_PRECOMP_H__
#define __HALRF_PRECOMP_H__

/*@--------------------------[Define] ---------------------------------------*/

#ifdef CONFIG_RTL8852A
    #define RF_8852A_SUPPORT
#endif

#ifdef CONFIG_RTL8852B
    #define RF_8852B_SUPPORT
#endif

#ifdef CONFIG_RTL8852C
    #define RF_8852C_SUPPORT
#endif

#ifdef CONFIG_RTL8832BR
    #define RF_8832BR_SUPPORT
#endif

#ifdef CONFIG_RTL8192XB
    #define RF_8192XB_SUPPORT
#endif

#ifdef CONFIG_RTL8852BP
    #define RF_8852BP_SUPPORT
#endif

#ifdef CONFIG_RTL8851B
    #define RF_8851B_SUPPORT
#endif

#ifdef CONFIG_RTL8832CRVU
    #define RF_8832CRVU_SUPPORT
#endif

#ifdef CONFIG_RTL8832BRVT
    #define RF_8832BRVT_SUPPORT
#endif

#define	MASKBYTE0		0xff
#define	MASKBYTE1		0xff00
#define	MASKBYTE2		0xff0000
#define	MASKBYTE3		0xff000000
#define	MASKHWORD		0xffff0000
#define	MASKLWORD		0x0000ffff
#define	MASKDWORD		0xffffffff
#define	MASKRF		0xfffff
#define	MASKRFMODE		0xf0000
#define	MASKRFRXBB		0x003e0
#define	MASKTXPWR		0x0003f
#define	INVALID_RF_DATA 0xffffffff


/*---[Define Only] ----------------------------------------------------------*/
#include "../../hal_headers_le.h"
#include "halrf_ic_hw_info.h"
#include "halrf_ic_sw_info.h"

#ifdef CONFIG_RTL8730A
    #define RF_8730A_SUPPORT
#endif

/*---[Include structure & prototype] ----------------------------------------*/

#include "halrf_hw_cfg.h"
#include "halrf_hw_cfg_ex.h"
#include "halrf_interface.h"
#include "halrf_dbg_cmd.h"
#include "halrf_dbg.h"
#include "halrf_txgapk.h"
#include "halrf_pwr_track.h"
#include "halrf_iqk.h"
#include "halrf_dpk.h"
#include "halrf_dack.h"
#include "halrf_pmac.h"
//#include "halrf_dbg_cmd.h"
#include "halrf_dbg_cmd_ex.h"
#include "halrf_init.h"
#include "halrf_init_ex.h"
#include "halrf_pwr_table.h"
#include "halrf_api.h"
#include "halrf_psd.h"
#include "halrf_kfree.h"
#include "halrf_hwimg.h"
#ifdef RF_8852A_SUPPORT
#include "halrf_8852a/halrf_dack_8852a.h"
#include "halrf_8852a/halrf_hwimg_8852a.h"
#include "halrf_8852a/halrf_kfree_8852a.h"
#endif

#ifdef RF_8852B_SUPPORT
#include "halrf_8852b/halrf_dack_8852b.h"
#include "halrf_8852b/halrf_hwimg_8852b.h"
#include "halrf_8852b/halrf_kfree_8852b.h"
#endif

#ifdef RF_8852C_SUPPORT
#include "halrf_8852c/halrf_dack_8852c.h"
#include "halrf_8852c/halrf_hwimg_8852c.h"
#include "halrf_8852c/halrf_kfree_8852c.h"
#endif

#ifdef RF_8832BR_SUPPORT
#include "halrf_8832br/halrf_dack_8832br.h"
#include "halrf_8832br/halrf_hwimg_8832br.h"
#include "halrf_8832br/halrf_kfree_8832br.h"
#endif

#ifdef RF_8192XB_SUPPORT
#include "halrf_8192xb/halrf_dack_8192xb.h"
#include "halrf_8192xb/halrf_hwimg_8192xb.h"
#include "halrf_8192xb/halrf_kfree_8192xb.h"
#endif

#ifdef RF_8852BP_SUPPORT
#include "halrf_8852bp/halrf_dack_8852bp.h"
#include "halrf_8852bp/halrf_hwimg_8852bp.h"
#include "halrf_8852bp/halrf_kfree_8852bp.h"
#endif

#ifdef RF_8730A_SUPPORT
#include "halrf_8730a/halrf_dack_8730a.h"
#include "halrf_8730a/halrf_hwimg_8730a.h"
#include "halrf_8730a/halrf_kfree_8730a.h"
#endif

#ifdef RF_8851B_SUPPORT
#include "halrf_8851b/halrf_dack_8851b.h"
#include "halrf_8851b/halrf_hwimg_8851b.h"
#include "halrf_8851b/halrf_kfree_8851b.h"
#endif

#include "halrf.h"
#include "halrf_ex.h"

#ifdef RF_8852A_SUPPORT
	#include "halrf_8852a/halrf_efuse_8852a.h"
	#include "halrf_8852a/halrf_reg_cfg_8852a.h"
	#include "halrf_8852a/halrf_8852a.h"
	#include "halrf_8852a/halrf_8852a_api.h"
	#include "halrf_8852a/halrf_8852a_api_ex.h"
	#include "halrf_8852a/halrf_iqk_8852a.h"
	#include "halrf_8852a/halrf_dpk_8852a.h"
	#include "halrf_8852a/halrf_txgapk_8852a.h"
	#include "halrf_8852a/halrf_version_rtl8852a.h"
	#include "halrf_8852a/halrf_set_pwr_table_8852a.h"
	#include "halrf_8852a/halrf_tssi_8852a.h"
	#include "halrf_8852a/halrf_psd_8852a.h"
#endif

#ifdef RF_8852B_SUPPORT
	#include "halrf_8852b/halrf_efuse_8852b.h"
	#include "halrf_8852b/halrf_reg_cfg_8852b.h"
	#include "halrf_8852b/halrf_8852b.h"
	#include "halrf_8852b/halrf_8852b_api.h"
	#include "halrf_8852b/halrf_8852b_api_ex.h"
	#include "halrf_8852b/halrf_iqk_8852b.h"
	#include "halrf_8852b/halrf_dpk_8852b.h"
	#include "halrf_8852b/halrf_txgapk_8852b.h"
	#include "halrf_8852b/halrf_version_rtl8852b.h"
	#include "halrf_8852b/halrf_set_pwr_table_8852b.h"
	#include "halrf_8852b/halrf_tssi_8852b.h"
	#include "halrf_8852b/halrf_psd_8852b.h"
#endif

#ifdef RF_8852C_SUPPORT
	#include "halrf_8852c/halrf_efuse_8852c.h"
	#include "halrf_8852c/halrf_reg_cfg_8852c.h"
	#include "halrf_8852c/halrf_8852c.h"
	#include "halrf_8852c/halrf_8852c_api.h"
//	#include "halrf_8852c/halrf_8852c_api_ex.h"
	#include "halrf_8852c/halrf_iqk_8852c.h"
	#include "halrf_8852c/halrf_dpk_8852c.h"
	#include "halrf_8852c/halrf_txgapk_8852c.h"
	#include "halrf_8852c/halrf_version_rtl8852c.h"
	#include "halrf_8852c/halrf_set_pwr_table_8852c.h"
	#include "halrf_8852c/halrf_tssi_8852c.h"
	#include "halrf_8852c/halrf_psd_8852c.h"
#endif

#ifdef RF_8832BR_SUPPORT
	#include "halrf_8832br/halrf_efuse_8832br.h"
	#include "halrf_8832br/halrf_reg_cfg_8832br.h"
	#include "halrf_8832br/halrf_8832br.h"
	#include "halrf_8832br/halrf_8832br_api.h"
//	#include "halrf_8832br/halrf_8832br_api_ex.h"
	#include "halrf_8832br/halrf_iqk_8832br.h"
	#include "halrf_8832br/halrf_dpk_8832br.h"
	#include "halrf_8832br/halrf_txgapk_8832br.h"
	#include "halrf_8832br/halrf_version_rtl8832br.h"
	#include "halrf_8832br/halrf_set_pwr_table_8832br.h"
	#include "halrf_8832br/halrf_tssi_8832br.h"
	#include "halrf_8832br/halrf_psd_8832br.h"
#endif

#ifdef RF_8192XB_SUPPORT
	#include "halrf_8192xb/halrf_efuse_8192xb.h"
	#include "halrf_8192xb/halrf_reg_cfg_8192xb.h"
	#include "halrf_8192xb/halrf_8192xb.h"
	#include "halrf_8192xb/halrf_8192xb_api.h"
//	#include "halrf_8192xb/halrf_8192xb_api_ex.h"
	#include "halrf_8192xb/halrf_iqk_8192xb.h"
	#include "halrf_8192xb/halrf_dpk_8192xb.h"
	#include "halrf_8192xb/halrf_txgapk_8192xb.h"
	#include "halrf_8192xb/halrf_version_rtl8192xb.h"
	#include "halrf_8192xb/halrf_set_pwr_table_8192xb.h"
	#include "halrf_8192xb/halrf_tssi_8192xb.h"
	#include "halrf_8192xb/halrf_psd_8192xb.h"
#endif

#ifdef RF_8852BP_SUPPORT
	#include "halrf_8852bp/halrf_efuse_8852bp.h"
	#include "halrf_8852bp/halrf_reg_cfg_8852bp.h"
	#include "halrf_8852bp/halrf_8852bp.h"
	#include "halrf_8852bp/halrf_8852bp_api.h"
//	#include "halrf_8852bp/halrf_8852bp_api_ex.h"
	#include "halrf_8852bp/halrf_iqk_8852bp.h"
	#include "halrf_8852bp/halrf_dpk_8852bp.h"
	#include "halrf_8852bp/halrf_txgapk_8852bp.h"
	#include "halrf_8852bp/halrf_version_rtl8852bp.h"
	#include "halrf_8852bp/halrf_set_pwr_table_8852bp.h"
	#include "halrf_8852bp/halrf_tssi_8852bp.h"
	#include "halrf_8852bp/halrf_psd_8852bp.h"
#endif
#ifdef RF_8730A_SUPPORT
	#include "halrf_8730a/halrf_efuse_8730a.h"
	#include "halrf_8730a/halrf_reg_cfg_8730a.h"
	#include "halrf_8730a/halrf_8730a.h"
	#include "halrf_8730a/halrf_8730a_api.h"
//	#include "halrf_8730a/halrf_8730a_api_ex.h"
	#include "halrf_8730a/halrf_iqk_8730a.h"
	#include "halrf_8730a/halrf_dpk_8730a.h"
//	#include "halrf_8730a/halrf_txgapk_8730a.h"
	#include "halrf_8730a/halrf_version_rtl8730a.h"
	#include "halrf_8730a/halrf_set_pwr_table_8730a.h"
	#include "halrf_8730a/halrf_tssi_8730a.h"
	#include "halrf_8730a/halrf_psd_8730a.h"
#endif

#ifdef RF_8851B_SUPPORT
	#include "halrf_8851b/halrf_efuse_8851b.h"
	#include "halrf_8851b/halrf_reg_cfg_8851b.h"
	#include "halrf_8851b/halrf_8851b.h"
	#include "halrf_8851b/halrf_8851b_api.h"
	#include "halrf_8851b/halrf_8851b_api_ex.h"
	#include "halrf_8851b/halrf_iqk_8851b.h"
	#include "halrf_8851b/halrf_dpk_8851b.h"
	#include "halrf_8851b/halrf_txgapk_8851b.h"
	#include "halrf_8851b/halrf_version_rtl8851b.h"
	#include "halrf_8851b/halrf_set_pwr_table_8851b.h"
	#include "halrf_8851b/halrf_tssi_8851b.h"
	#include "halrf_8851b/halrf_psd_8851b.h"
#endif

#endif
