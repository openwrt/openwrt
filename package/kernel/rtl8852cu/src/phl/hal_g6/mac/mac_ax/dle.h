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

#ifndef _MAC_AX_DLE_H_
#define _MAC_AX_DLE_H_

#include "../type.h"
#include "../mac_ax.h"
#include "cpuio.h"

/*--------------------Define ----------------------------------------*/
#define DLE_DFI_WAIT_CNT 1000
#define DLE_DFI_WAIT_US 1

#define DLE_WAIT_CNT 2000
#define DLE_WAIT_US 1

// DLE_DFI_TYPE_FREEPG
#define B_AX_DLE_FREE_TAILPG_SH 16
#define B_AX_DLE_FREE_TAILPG_MSK 0xfff
#define B_AX_DLE_FREE_HEADPG_SH 0
#define B_AX_DLE_FREE_HEADPG_MSK 0xfff

#define B_AX_DLE_PUB_PGNUM_SH 0
#define B_AX_DLE_PUB_PGNUM_MSK 0x1fff

// DLE_DFI_TYPE_QUOTA
#define B_AX_DLE_USE_PGNUM_SH 16
#define B_AX_DLE_USE_PGNUM_MSK 0xfff
#define B_AX_DLE_RSV_PGNUM_SH 0
#define B_AX_DLE_RSV_PGNUM_MSK 0xfff

// DLE_DFI_TYPE_QEMPTY
#define B_AX_DLE_QEMPTY_GRP_SH 0
#define B_AX_DLE_QEMPTY_GRP_MSK 0xffffffff

#define QUEUE_EMPTY_CHK_CNT 2
#define WDE_QEMPTY_NUM_8852A 18
#define WDE_QEMPTY_NUM_8852B 5
#define WDE_QEMPTY_NUM_8852C 19
#define WDE_QEMPTY_NUM_8192XB 18
#define WDE_QEMPTY_NUM_8851B 5
#define WDE_QEMPTY_NUM_8851E 19
#define WDE_QEMPTY_NUM_8852D 19
#define WDE_QEMPTY_NUM_1115E 36
#define PLE_QEMPTY_NUM 2
#define WDE_QEMPTY_ACQ_NUM_8852A 16 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_8852B 4 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_8852C 16 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_8192XB 16 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_8851B 4 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_8851E 16 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_8852D 16 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_ACQ_NUM_1115E 16 /* cannot over WDE_QEMPTY_ACQ_NUM_MAX */
#define WDE_QEMPTY_MGQ_SEL_8852A 16
#define WDE_QEMPTY_MGQ_SEL_8852B 4
#define WDE_QEMPTY_MGQ_SEL_8852C 16
#define WDE_QEMPTY_MGQ_SEL_8192XB 16
#define WDE_QEMPTY_MGQ_SEL_8851B 4
#define WDE_QEMPTY_MGQ_SEL_8851E 16
#define WDE_QEMPTY_MGQ_SEL_8852D 16
#define WDE_QEMPTY_MGQ_SEL_1115E 16
#define QEMP_ACQ_GRP_MACID_NUM 8
#define QEMP_ACQ_GRP_QSEL_SH 4
#define QEMP_ACQ_GRP_QSEL_MASK 0xF
#define S_AX_WDE_PAGE_SEL_64	0
#define S_AX_WDE_PAGE_SEL_128	1
/* #define S_AX_WDE_PAGE_SEL_256	2 // HDP not support */

/* #define S_AX_PLE_PAGE_SEL_64	0 // HDP not support */
#define S_AX_PLE_PAGE_SEL_128	1
#define S_AX_PLE_PAGE_SEL_256	2

#define DLE_BOUND_UNIT (8 * 1024)

#define DLE_QUEUE_NONEMPTY	0
#define DLE_QUEUE_EMPTY		1

#define B_CMAC0_MGQ_NORMAL	BIT2
#define B_CMAC0_MGQ_NO_PWRSAV	BIT3
#define B_CMAC0_CPUMGQ		BIT4
#define B_CMAC1_MGQ_NORMAL	BIT10
#define B_CMAC1_MGQ_NO_PWRSAV	BIT11
#define B_CMAC1_CPUMGQ		BIT12

#define DLE_LAMODE_SIZE_8852A 262144 // (256 * 1024)
#define DLE_LAMODE_SIZE_8852B 131072 // (128 * 1024)
#define DLE_LAMODE_SIZE_8852C 262144 // (256 * 1024)
#define DLE_LAMODE_SIZE_8192XB 262144 // (256 * 1024)
#define DLE_LAMODE_SIZE_8851B 131072 // (128 * 1024)
#define DLE_LAMODE_SIZE_8851E 262144 // (256 * 1024)
#define DLE_LAMODE_SIZE_8852D 262144 // (256 * 1024)

#define DLE_SCC_RSVD_SIZE_8852B 98304 // (96 * 1024)
#define DLE_SCC_RSVD_SIZE_8851B 98304 // (96 * 1024)

#define WDE_QTA_NUM 5
#define PLE_QTA_NUM_8852AB 11
#define PLE_QTA_NUM_8852C 12
#define PLE_QTA_NUM_8192XB 12
#define PLE_QTA_NUM_8851E 12
#define PLE_QTA_NUM_8852D 12

#define PLE_QTA_PG128B_12KB 96

#define QLNKTBL_ADDR_INFO_SEL BIT(0)
#define QLNKTBL_ADDR_INFO_SEL_0 0
#define QLNKTBL_ADDR_INFO_SEL_1 1
#define QLNKTBL_ADDR_TBL_IDX_MSK 0x3FF
#define QLNKTBL_ADDR_TBL_IDX_SH 1
#define QLNKTBL_DATA_SEL0_HEAD_PKT_ID_11_8_MSK 0xF
#define QLNKTBL_DATA_SEL0_HEAD_PKT_ID_11_8_SH 0
#define QLNKTBL_DATA_SEL1_PKT_CNT_MSK 0xFFF
#define QLNKTBL_DATA_SEL1_PKT_CNT_SH 0
#define QLNKTBL_DATA_SEL1_TAIL_PKTID_MSK 0xFFF
#define QLNKTBL_DATA_SEL1_TAIL_PKT_ID_SH 12
#define QLNKTBL_DATA_SEL1_HEAD_PKTID_7_0_MSK 0xFF
#define QLNKTBL_DATA_SEL1_HEAD_PKTID_7_0_SH 24

#define PRELD_B0_ENT_NUM 10
#define PRELD_B1_ENT_NUM 4
#define PRELD_AMSDU_SIZE 52 // (1536 + 128) * 2 / 64
#define PRELD_NEXT_WND 1

/*--------------------Define Enum------------------------------------*/

/**
 * @enum WDE_QTAID
 *
 * @brief WDE_QTAID
 *
 * @var WDE_QTAID::WDE_QTAID_HOST_IF
 * Please Place Description here.
 * @var WDE_QTAID::WDE_QTAID_WLAN_CPU
 * Please Place Description here.
 * @var WDE_QTAID::WDE_QTAID_DATA_CPU
 * Please Place Description here.
 * @var WDE_QTAID::WDE_QTAID_PKTIN
 * Please Place Description here.
 * @var WDE_QTAID::WDE_QTAID_CPUIO
 * Please Place Description here.
 */
enum WDE_QTAID {
	WDE_QTAID_HOST_IF = 0,
	WDE_QTAID_WLAN_CPU = 1,
	WDE_QTAID_DATA_CPU = 2,
	WDE_QTAID_PKTIN = 3,
	WDE_QTAID_CPUIO = 4
};

/**
 * @enum PLE_QTAID
 *
 * @brief PLE_QTAID
 *
 * @var PLE_QTAID::PLE_QTAID_B0_TXPL
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_B1_TXPL
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_C2H
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_H2C
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_WLAN_CPU
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_MPDU
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_CMAC0_RX
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_CMAC1_RX
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_CMAC1_BBRPT
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_WDRLS
 * Please Place Description here.
 * @var PLE_QTAID::PLE_QTAID_CPUIO
 * Please Place Description here.
 */
enum PLE_QTAID {
	PLE_QTAID_B0_TXPL = 0,
	PLE_QTAID_B1_TXPL = 1,
	PLE_QTAID_C2H = 2,
	PLE_QTAID_H2C = 3,
	PLE_QTAID_WLAN_CPU = 4,
	PLE_QTAID_MPDU = 5,
	PLE_QTAID_CMAC0_RX = 6,
	PLE_QTAID_CMAC1_RX = 7,
	PLE_QTAID_BBRPT = 8,
	PLE_QTAID_WDRLS = 9,
	PLE_QTAID_CPUIO = 10,
	PLE_QTAID_TXRPT = 11
};

/**
 * @enum DLE_CTRL_TYPE
 *
 * @brief DLE_CTRL_TYPE
 *
 * @var DLE_CTRL_TYPE::DLE_CTRL_TYPE_WDE
 * Please Place Description here.
 * @var DLE_CTRL_TYPE::DLE_CTRL_TYPE_PLE
 * Please Place Description here.
 * @var DLE_CTRL_TYPE::DLE_CTRL_TYPE_NUM
 * Please Place Description here.
 */
enum DLE_CTRL_TYPE {
	DLE_CTRL_TYPE_WDE = 0,
	DLE_CTRL_TYPE_PLE = 1,
	DLE_CTRL_TYPE_NUM = 2
};

/**
 * @enum DLE_DFI_TYPE
 *
 * @brief DLE_DFI_TYPE
 *
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_FREEPG
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_QUOTA
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_PAGELLT
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_PKTINFO
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_PREPKTLLT
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_NXTPKTLLT
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_QLNKTBL
 * Please Place Description here.
 * @var DLE_DFI_TYPE::DLE_DFI_TYPE_QEMPTY
 * Please Place Description here.
 */
enum DLE_DFI_TYPE {
	DLE_DFI_TYPE_FREEPG = 0,
	DLE_DFI_TYPE_QUOTA = 1,
	DLE_DFI_TYPE_PAGELLT = 2,
	DLE_DFI_TYPE_PKTINFO = 3,
	DLE_DFI_TYPE_PREPKTLLT = 4,
	DLE_DFI_TYPE_NXTPKTLLT = 5,
	DLE_DFI_TYPE_QLNKTBL = 6,
	DLE_DFI_TYPE_QEMPTY = 7
};

enum DFI_TYPE_FREEPG_SEL {
	DFI_TYPE_FREEPG_IDX = 0,
	DFI_TYPE_FREEPG_PUBNUM
};

/**
 * @enum DLE_RSVD_INFO
 *
 * @brief DLE_RSVD_INFO
 *
 * @var DLE_RSVD_INFO::DLE_RSVD_INFO_NONE
 * do not rsvd dle quota.
 * @var DLE_RSVD_INFO::DLE_RSVD_INFO_FW
 * rsvd dle quota for fw.
 * @var DLE_RSVD_INFO::DLE_RSVD_INFO_LAMODE
 * rsvd dle quota for BB LA mode.
 */
enum DLE_RSVD_INFO {
	DLE_RSVD_INFO_NONE,
	DLE_RSVD_INFO_FW,
	DLE_RSVD_INFO_LAMODE,
};

/*--------------------Define MACRO----------------------------------*/

/*--------------------Define Struct-----------------------------------*/

/**
 * @struct dle_dfi_freepg_t
 * @brief dle_dfi_freepg_t
 *
 * @var dle_dfi_freepg_t::dle_type
 * Please Place Description here.
 * @var dle_dfi_freepg_t::free_headpg
 * Please Place Description here.
 * @var dle_dfi_freepg_t::free_tailpg
 * Please Place Description here.
 * @var dle_dfi_freepg_t::pub_pgnum
 * Please Place Description here.
 */
struct dle_dfi_freepg_t {
	// input parameter
	enum DLE_CTRL_TYPE dle_type;
	// output parameter
	u16 free_headpg;
	u16 free_tailpg;
	u16 pub_pgnum;
};

/**
 * @struct dle_dfi_quota_t
 * @brief dle_dfi_quota_t
 *
 * @var dle_dfi_quota_t::dle_type
 * Please Place Description here.
 * @var dle_dfi_quota_t::qtaid
 * Please Place Description here.
 * @var dle_dfi_quota_t::rsv_pgnum
 * Please Place Description here.
 * @var dle_dfi_quota_t::use_pgnum
 * Please Place Description here.
 */
struct dle_dfi_quota_t {
	// input parameter
	enum DLE_CTRL_TYPE dle_type;
	u32 qtaid;
	// output parameter
	u16 rsv_pgnum;
	u16 use_pgnum;
};

/**
 * @struct dle_dfi_qempty_t
 * @brief dle_dfi_qempty_t
 *
 * @var dle_dfi_qempty_t::dle_type
 * Please Place Description here.
 * @var dle_dfi_qempty_t::grpsel
 * Please Place Description here.
 * @var dle_dfi_qempty_t::qempty
 * Please Place Description here.
 */
struct dle_dfi_qempty_t {
	// input parameter
	enum DLE_CTRL_TYPE dle_type;
	u32 grpsel;
	// output parameter
	u32 qempty;
};

/**
 * @struct dle_dfi_ctrl_t
 * @brief dle_dfi_ctrl_t
 *
 * @var dle_dfi_ctrl_t::ctrl_type
 * Please Place Description here.
 * @var dle_dfi_ctrl_t::dfi_ctrl
 * Please Place Description here.
 * @var dle_dfi_ctrl_t::dfi_data
 * Please Place Description here.
 */
struct dle_dfi_ctrl_t {
	enum DLE_CTRL_TYPE type;
	u32 target;
	u32 addr;
	u32 out_data;
	union {
		struct dle_dfi_freepg_t freepg;
		struct dle_dfi_quota_t quota;
		struct dle_dfi_qempty_t qempty;
	} u;
};

/**
 * @struct dle_size_t
 * @brief dle_size_t
 *
 * @var dle_size_t::pge_size
 * Please Place Description here.
 * @var dle_size_t::lnk_pge_num
 * Please Place Description here.
 * @var dle_size_t::unlnk_pge_num
 * Please Place Description here.
 */
struct dle_size_t {
	u16 pge_size;
	u16 lnk_pge_num;
	u16 unlnk_pge_num;
};

/**
 * @struct wde_quota_t
 * @brief wde_quota_t
 *
 * @var wde_quota_t::hif
 * Please Place Description here.
 * @var wde_quota_t::wcpu
 * Please Place Description here.
 * @var wde_quota_t::dcpu
 * Please Place Description here.
 * @var wde_quota_t::pkt_in
 * Please Place Description here.
 * @var wde_quota_t::cpu_io
 * Please Place Description here.
 */
struct wde_quota_t {
	u16 hif;
	u16 wcpu;
	u16 dcpu;
	u16 pkt_in;
	u16 cpu_io;
};

/**
 * @struct ple_quota_t
 * @brief ple_quota_t
 *
 * @var ple_quota_t::cma0_tx
 * Please Place Description here.
 * @var ple_quota_t::cma1_tx
 * Please Place Description here.
 * @var ple_quota_t::c2h
 * Please Place Description here.
 * @var ple_quota_t::h2c
 * Please Place Description here.
 * @var ple_quota_t::wcpu
 * Please Place Description here.
 * @var ple_quota_t::mpdu_proc
 * Please Place Description here.
 * @var ple_quota_t::cma0_dma
 * Please Place Description here.
 * @var ple_quota_t::cma1_dma
 * Please Place Description here.
 * @var ple_quota_t::bb_rpt
 * Please Place Description here.
 * @var ple_quota_t::wd_rel
 * Please Place Description here.
 * @var ple_quota_t::cpu_io
 * Please Place Description here.
 * @var ple_quota_t::tx_rpt
 * Please Place Description here.
 * @var ple_quota_t::h2d
 * Please Place Description here.
 */
struct ple_quota_t {
	u16 cma0_tx;
	u16 cma1_tx;
	u16 c2h;
	u16 h2c;
	u16 wcpu;
	u16 mpdu_proc;
	u16 cma0_dma;
	u16 cma1_dma;
	u16 bb_rpt;
	u16 wd_rel;
	u16 cpu_io;
	u16 tx_rpt;
	u16 h2d;
};

/**
 * @struct dle_mem_t
 * @brief dle_mem_t
 *
 * @var dle_mem_t::mode
 * Please Place Description here.
 * @var dle_mem_t::wde_size
 * Please Place Description here.
 * @var dle_mem_t::ple_size
 * Please Place Description here.
 * @var dle_mem_t::wde_min_qt
 * Please Place Description here.
 * @var dle_mem_t::wde_max_qt
 * Please Place Description here.
 * @var dle_mem_t::ple_min_qt
 * Please Place Description here.
 * @var dle_mem_t::ple_max_qt
 * Please Place Description here.
 */
struct dle_mem_t {
	enum mac_ax_qta_mode mode;
	struct dle_size_t *wde_size;
	struct dle_size_t *ple_size;
	struct wde_quota_t *wde_min_qt;
	struct wde_quota_t *wde_max_qt;
	struct ple_quota_t *ple_min_qt;
	struct ple_quota_t *ple_max_qt;
};

/*--------------------Export global variable----------------------------*/

/*--------------------Function declaration-----------------------------*/

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */
/**
 * @brief dle_dfi_freepg
 *
 * @param *adapter
 * @param *freepg
 * @return Please Place Description here.
 * @retval u32
 */
u32 dle_dfi_freepg(struct mac_ax_adapter *adapter,
		   struct dle_dfi_freepg_t *freepg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief dle_dfi_quota
 *
 * @param *adapter
 * @param *quota
 * @return Please Place Description here.
 * @retval u32
 */

u32 dle_dfi_quota(struct mac_ax_adapter *adapter,
		  struct dle_dfi_quota_t *quota);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief dle_dfi_qempty
 *
 * @param *adapter
 * @param *qempty
 * @return Please Place Description here.
 * @retval u32
 */

u32 dle_dfi_qempty(struct mac_ax_adapter *adapter,
		   struct dle_dfi_qempty_t *qempty);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief mac_chk_allq_empty
 *
 * @param *adapter
 * @param *empty
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_chk_allq_empty(struct mac_ax_adapter *adapter, u8 *empty);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief dle_quota_change
 *
 * @param *adapter
 * @param mode
 * @return Please Place Description here.
 * @retval u32
 */

u32 dle_quota_change(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief dle_init
 *
 * @param *adapter
 * @param mode
 * @param ext_mode
 * @return Please Place Description here.
 * @retval u32
 */

u32 dle_init(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode,
	     enum mac_ax_qta_mode ext_mode);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief is_qta_dbcc
 *
 * @param *adapter
 * @param mode
 * @param *is_dbcc
 * @return Please Place Description here.
 * @retval u32
 */

u32 is_qta_dbcc(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode,
		u8 *is_dbcc);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief is_qta_poh
 *
 * @param *adapter
 * @param mode
 * @param *is_poh
 * @return Please Place Description here.
 * @retval u32
 */

u8 is_curr_dbcc(struct mac_ax_adapter *adapter);

u32 is_qta_poh(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode,
	       u8 *is_poh);
/**
 * @}
 * @}
 */

u32 redu_wowlan_rx_qta(struct mac_ax_adapter *adapter);
u32 restr_wowlan_rx_qta(struct mac_ax_adapter *adapter);

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief mac_preload_cfg
 *
 * @param *adapter
 * @param band
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_preload_cfg(struct mac_ax_adapter *adapter, enum mac_ax_band band,
		    struct mac_ax_preld_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief preload_init
 *
 * @param *adapter
 * @param band
 * @param mode
 * @return Please Place Description here.
 * @retval u32
 */

u32 preload_init(struct mac_ax_adapter *adapter, enum mac_ax_band band,
		 enum mac_ax_qta_mode mode);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup DLE
 * @{
 */

/**
 * @brief get_dle_rsvd_info
 *
 * @param *adapter
 * @param *info
 * @return success or fail
 * @retval u32
 */

u32 get_dle_rsvd_info(struct mac_ax_adapter *adapter, enum DLE_RSVD_INFO *info);

/**
 * @}
 * @}
 */

#endif
