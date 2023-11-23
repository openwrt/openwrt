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

#ifndef _MAC_AX_H_
#define _MAC_AX_H_

#include "mac_def.h"
#include "mac_ax/fwcmd.h"
#include "mac_ax/security_cam.h"
#include "mac_ax/secure_boot.h"
#include "mac_ax/efuse.h"
#include "mac_ax/p2p.h"
#include "mac_ax/twt.h"
#include "mac_ax/dbcc.h"
#if MAC_AX_SDIO_SUPPORT
#include "mac_ax/_sdio.h"
#endif
#if MAC_AX_FEATURE_HV
#include "hv_ax/init_hv.h"
#include "hv_ax/fwcmd_hv.h"
#endif

#if MAC_AX_FEATURE_HV
#include "hv_type.h"
#endif

#define MAC_AX_MAJOR_VER	0	/*Software Architcture Modify*/
#define MAC_AX_PROTOTYPE_VER	29	/*New Feature;Regular Release*/
#define MAC_AX_SUB_VER		29	/*for bug fix*/
#define MAC_AX_SUB_INDEX	0	/*for special used*/

#define MAC_AX_SRC_VER(a, b, c, d)                                             \
				(((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

#define acv_mask		0x0F

#ifdef CONFIG_NEW_HALMAC_INTERFACE

/**
 * @brief mac_ax_ops_init_v1
 *
 * @param *phl_adapter
 * @param *drv_adapter
 * @param chip_id
 * @param hci
 * @param **mac_adapter
 * @param **mac_ops
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_ax_ops_init_v1(void *phl_adapter, void *drv_adapter,
		       enum rtw_chip_id chip_id,
		       enum rtw_hci_type hci,
		       struct mac_ax_adapter **mac_adapter,
		       struct mac_ax_ops **mac_ops);

/**
 * @brief mac_ax_ops_init
 *
 * @param *drv_adapter
 * @param *pltfm_cb
 * @param intf
 * @param **mac_adapter
 * @param **mac_ops
 * @return Please Place Description here.
 * @retval u32
 */
#else
u32 mac_ax_ops_init(void *drv_adapter, struct mac_ax_pltfm_cb *pltfm_cb,
		    enum mac_ax_intf intf,
		    struct mac_ax_adapter **mac_adapter,
		    struct mac_ax_ops **mac_ops);

/**
 * @brief mac_ax_phl_init
 *
 * @param *phl_adapter
 * @param *mac_adapter
 * @return Please Place Description here.
 * @retval u32
 */

#endif
#if MAC_AX_PHL_H2C
u32 mac_ax_phl_init(void *phl_adapter, struct mac_ax_adapter *mac_adapter);

/**
 * @brief mac_ax_ops_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
#endif

u32 mac_ax_ops_exit(struct mac_ax_adapter *adapter);

/**
 * @brief is_chip_id
 *
 * @param *adapter
 * @param id
 * @return Please Place Description here.
 * @retval u32
 */

u32 is_chip_id(struct mac_ax_adapter *adapter, enum mac_ax_chip_id id);

/**
 * @brief is_cv
 *
 * @param *adapter
 * @param rtw_cv cv
 * @return Please Place Description here.
 * @retval u32
 */

u32 is_cv(struct mac_ax_adapter *adapter, enum rtw_cv cv);

/**
 * @brief xlat_chip_id
 *
 * @param hw_id
 * @param *chip_id
 * @return Please Place Description here.
 * @retval u32
 */
u32 xlat_chip_id(u8 hw_id, u8 *chip_id);

#endif
