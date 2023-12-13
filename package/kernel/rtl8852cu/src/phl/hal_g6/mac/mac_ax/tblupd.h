/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _TABLEUPD_H2C_H_
#define _TABLEUPD_H2C_H_

#include "../type.h"
#include "fwcmd.h"
#include "trx_desc.h"
#include "addr_cam.h"

/*--------------------Define MACRO--------------------------------------*/
#define DLRU_CLASS_GRP_TBL	0
#define ULRU_CLASS_GRP_TBL	1
#define CLASS_RUSTA_INFO	2
#define DLRU_CLASS_FIXED_TBL	3
#define ULRU_CLASS_FIXED_TBL	4
#define CLASS_BA_INFOTBL	5
#define CLASS_MUDECISION_PARA	6
#define CLASS_UL_FIXINFO	7
#define CLASS_F2P_FIXMODE_PARA	8

#define BACAM_RST_DLY_CNT	1000
#define BACAM_RST_DLY_US	1

#define NTX_PATH_EN_MASK 0xf
#define PATH_MAP_MASK 0x3

#define BACAM_MAX_ENTRY_IDX_8852C 15
#define BACAM_INIT_TMP_ENTRY_NUM_AP_8852C 8
#define BACAM_INIT_TMP_ENTRY_NUM_STA_8852C 2
#define BACAM_MAX_ENTRY_IDX_8192XB 15
#define BACAM_INIT_TMP_ENTRY_NUM_AP_8192XB 8
#define BACAM_INIT_TMP_ENTRY_NUM_STA_8192XB 2
#define BACAM_MAX_RU_SUPPORT_B0_AP_8852C 8
#define BACAM_MAX_RU_SUPPORT_B1_AP_8852C 0
#define BACAM_MAX_RU_SUPPORT_B0_AP_8192XB 8
#define BACAM_MAX_RU_SUPPORT_B1_AP_8192XB 8
#define BACAM_MIN_ENTRY_IDX_DEF_1115E 0
#define BACAM_MAX_ENTRY_IDX_DEF_1115E 127
#define BACAM_MAX_RU_SUPPORT_B0_STA 1
#define BACAM_MAX_RU_SUPPORT_B1_STA 1
/*--------------------Define Enum---------------------------------------*/

/**
 * @enum H2C_WLANINFO_SEL
 *
 * @brief H2C_WLANINFO_SEL
 *
 * @var H2C_WLANINFO_SEL::DUMPWLANC
 * Please Place Description here.
 * @var H2C_WLANINFO_SEL::DUMPWLANS
 * Please Place Description here.
 * @var H2C_WLANINFO_SEL::DUMPWLAND
 * Please Place Description here.
 */
enum H2C_WLANINFO_SEL {
	DUMPWLANC = BIT0,
	DUMPWLANS = BIT1,
	DUMPWLAND = BIT2
};

/**
 * @enum DLDECISION_RESULT_TYPE
 *
 * @brief DLDECISION_RESULT_TYPE
 *
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_FORCESU
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_MU_FORCEMU
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_FORCEMU_FAIL
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_FORCERU_FAIL
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_FORCERU_RUARST_RU2SU
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_RU_FORCERU_RUSRST_FIXTBL
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_RU_FORCERU
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_WDINFO_USERATE
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_PRINULLWD
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_MU_BYPASS_MUTPCOMPARE
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_MUTXTIME_PASS_MU_NOTSUPPORT
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_MUTXTIME_FAIL_RU_NOTSUPPORT
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_RUARST_RU2SU
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_RU_RUARST_FIXTBL
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_MU_TPCOMPARE_RST
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_RU_TPCOMPARE_RST
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_SU_TPCOMPARE_RST
 * Please Place Description here.
 * @var DLDECISION_RESULT_TYPE::DLDECISION_MAX
 * Please Place Description here.
 */
enum DLDECISION_RESULT_TYPE {
	DLDECISION_SU_FORCESU = 0,
	DLDECISION_MU_FORCEMU = 1,
	DLDECISION_SU_FORCEMU_FAIL = 2,
	DLDECISION_SU_FORCERU_FAIL = 3,
	DLDECISION_SU_FORCERU_RUARST_RU2SU = 4,
	DLDECISION_RU_FORCERU_RUSRST_FIXTBL = 5,
	DLDECISION_RU_FORCERU = 6,
	DLDECISION_SU_WDINFO_USERATE = 7,
	DLDECISION_SU_PRINULLWD = 8,
	DLDECISION_MU_BYPASS_MUTPCOMPARE = 9,
	DLDECISION_SU_MUTXTIME_PASS_MU_NOTSUPPORT = 10,
	DLDECISION_SU_MUTXTIME_FAIL_RU_NOTSUPPORT = 11,
	DLDECISION_SU_RUARST_RU2SU = 12,
	DLDECISION_RU_RUARST_FIXTBL = 13,
	DLDECISION_MU_TPCOMPARE_RST = 14,
	DLDECISION_RU_TPCOMPARE_RST = 15,
	DLDECISION_SU_TPCOMPARE_RST = 16,
	DLDECISION_MAX = 17
};

enum S_AX_BACAM_RST {
	S_AX_BACAM_RST_DONE = 0,
	S_AX_BACAM_RST_ENT,
	S_AX_BACAM_RST_ALL,
	S_AX_BACAM_RST_RSVD
};

enum BACAM_RST_TYPE {
	BACAM_RST_ALL = 0,
	BACAM_RST_ENT,
};

/*--------------------Define Struct-------------------------------------*/

struct rst_bacam_info {
	enum BACAM_RST_TYPE type;
	u8 ent;
};

/**
 * @brief mac_bacam_avl_std_entry_idx
 *
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_bacam_avl_std_entry_idx(struct mac_ax_adapter *adapter,
				struct mac_ax_avl_std_bacam_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup BA_Info
 * @{
 */

/**
 * @brief mac_bacam_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_bacam_init(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup BA_Info
 * @{
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup BA_Info
 * @{
 */
/**
 * @brief mac_upd_ba_infotbl
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_upd_ba_infotbl(struct mac_ax_adapter *adapter,
		       struct mac_ax_ba_infotbl *info);
/**
 * @}
 * @}
 */
/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup MU
 * @{
 */

/**
 * @brief mac_upd_mudecision_para
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_upd_mudecision_para(struct mac_ax_adapter *adapter,
			    struct mac_ax_mudecision_para *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup UL
 * @{
 */

/**
 * @brief mac_upd_ul_fixinfo
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_upd_ul_fixinfo(struct mac_ax_adapter *adapter,
		       struct rtw_phl_ax_ul_fixinfo *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup F2P_TestCmd
 * @{
 */

/**
 * @brief mac_f2p_test_cmd
 *
 * @param *adapter
 * @param *info
 * @param *f2pwd
 * @param *ptxcmd
 * @param *psigb_addr
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_f2p_test_cmd(struct mac_ax_adapter *adapter,
		     struct mac_ax_f2p_test_para *info,
		     struct mac_ax_f2p_wd *f2pwd,
		     struct mac_ax_f2p_tx_cmd *ptxcmd,
		     u8 *psigb_addr);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup DMAC_Table
 * @{
 */

/**
 * @brief mac_upd_dctl_info
 *
 * @param *adapter
 * @param *info
 * @param *mask
 * @param macid
 * @param operation
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_upd_dctl_info(struct mac_ax_adapter *adapter,
		      struct mac_ax_dctl_info *info,
		      struct mac_ax_dctl_info *mask, u8 macid, u8 operation);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_ShortCut
 * @{
 */

/**
 * @brief mac_upd_shcut_mhdr
 *
 * @param *adapter
 * @param *info
 * @param macid
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_upd_shcut_mhdr(struct mac_ax_adapter *adapter,
		       struct mac_ax_shcut_mhdr *info, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup CMAC_Table
 * @{
 */

/**
 * @brief mac_upd_cctl_info
 *
 * @param *adapter
 * @param *info
 * @param *mask
 * @param macid
 * @param operation
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_upd_cctl_info(struct mac_ax_adapter *adapter,
		      struct rtw_hal_mac_ax_cctl_info *info,
		      struct rtw_hal_mac_ax_cctl_info *mask, u8 macid, u8 operation);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup FW_CommonInfo
 * @{
 */

/**
 * @brief mac_set_fixmode_mib
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_fixmode_mib(struct mac_ax_adapter *adapter,
			struct mac_ax_fixmode_para *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup F2P_TestCmd
 * @{
 */

/**
 * @brief mac_snd_test_cmd
 *
 * @param *adapter
 * @param *cmd_buf
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_snd_test_cmd(struct mac_ax_adapter *adapter,
		     u8 *cmd_buf);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup BA_Info
 * @{
 */

/**
 * @brief rst_bacam
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 rst_bacam(struct mac_ax_adapter *adapter, struct rst_bacam_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup BA_Info
 * @{
 */

/**
 * @brief mac_bacam_info
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_bacam_info(struct mac_ax_adapter *adapter,
		   struct mac_ax_bacam_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup SU
 * @{
 */

/**
 * @brief mac_ss_dl_grp_upd
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_ss_dl_grp_upd(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_dl_grp_upd *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup UL
 * @{
 */

/**
 * @brief mac_ss_ul_grp_upd
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_ss_ul_grp_upd(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_ul_grp_upd *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup UL
 * @{
 */

/**
 * @brief mac_ss_ul_sta_upd
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_ss_ul_sta_upd(struct mac_ax_adapter *adapter,
		      struct mac_ax_ss_ul_sta_upd *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup MU
 * @{
 */

/**
 * @brief mac_mu_sta_upd
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_mu_sta_upd(struct mac_ax_adapter *adapter,
		   struct mac_ax_mu_sta_upd *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup FW_CommonInfo
 * @{
 */

/**
 * @brief mac_wlaninfo_get
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_wlaninfo_get(struct mac_ax_adapter *adapter,
		     struct mac_ax_wlaninfo_get *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup FW_CommonInfo
 * @{
 */

/**
 * @brief mac_dumpwlanc
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dumpwlanc(struct mac_ax_adapter *adapter,
		  struct mac_ax_dumpwlanc *para);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup FW_CommonInfo
 * @{
 */

/**
 * @brief mac_dumpwlans
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dumpwlans(struct mac_ax_adapter *adapter,
		  struct mac_ax_dumpwlans *para);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup FW_CommonInfo
 * @{
 */

/**
 * @brief mac_dumpwland
 *
 * @param *adapter
 * @param *para
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dumpwland(struct mac_ax_adapter *adapter,
		  struct mac_ax_dumpwland *para);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup CMAC_Table
 * @{
 */

/**
 * @brief cctl_info_debug_write
 *
 * @param *adapter
 * @param macid
 * @param *tbl
 * @return Please Place Description here.
 * @retval u32
 */

#if MAC_AX_FEATURE_DBGPKG
u32 cctl_info_debug_write(struct mac_ax_adapter *adapter, u8 macid,
			  struct fwcmd_cctlinfo_ud *tbl);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup DMAC_Table
 * @{
 */

/**
 * @brief dctl_info_debug_write
 *
 * @param *adapter
 * @param macid
 * @param *tbl
 * @return Please Place Description here.
 * @retval u32
 */
u32 dctl_info_debug_write(struct mac_ax_adapter *adapter, u8 macid,
			  struct fwcmd_dctlinfo_ud *tbl);
/**
 * @}
 * @}
 */

#endif
u32 mac_fw_status_cmd(struct mac_ax_adapter *adapter,
		      struct mac_ax_fwstatus_payload *info);

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup CMAC_TABLE
 * @{
 */

/**
 * @brief mac_tx_path_map_cfg
 *
 * @param *adapter
 * @param macid
 * @param *tbl
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_tx_path_map_cfg(struct mac_ax_adapter *adapter,
			struct hal_txmap_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup fw_ofdma_info
 * @{
 */

/**
 * @brief mac_fwc2h_ofdma_sts_parse
 *
 * @param *adapter
 * @param *fw_c2h_sts
 * @param *content
 * @return parse the ofdma info report by fw to mac_ax_fwc2h_sts structure
 * @retval u32
 */

u32 mac_fwc2h_ofdma_sts_parse(struct mac_ax_adapter *adapter,
			      struct mac_ax_fwc2h_sts *fw_c2h_sts,
			      u32 *content);
/**
 * @}
 * @}
 */

/**
 * @addtogroup FrameExchange
 * @{
 * @addtogroup fw_ofdma_info
 * @{
 */

/**
 * @brief mac_fw_ofdma_sts_en
 *
 * @param *adapter
 * @param *fwsts_para
 * @return enable/disable and set period for fw report ofdma info
 * @retval u32
 */

u32 mac_fw_ofdma_sts_en(struct mac_ax_adapter *adapter,
			struct mac_ax_fwsts_para *fwsts_para);
/**
 * @}
 * @}
 */

#endif
