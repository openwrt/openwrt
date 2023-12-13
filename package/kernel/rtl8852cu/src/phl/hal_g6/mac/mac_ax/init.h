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

#ifndef _MAC_AX_INIT_H_
#define _MAC_AX_INIT_H_

#include "../type.h"
#if MAC_AX_8852A_SUPPORT
#include "mac_8852a/init_8852a.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "mac_8852b/init_8852b.h"
#endif
#if MAC_AX_8852C_SUPPORT
#include "mac_8852c/init_8852c.h"
#endif
#if MAC_AX_8192XB_SUPPORT
#include "mac_8192xb/init_8192xb.h"
#endif
#if MAC_AX_8851B_SUPPORT
#include "mac_8851b/init_8851b.h"
#endif
#if MAC_AX_8851E_SUPPORT
#include "mac_8851e/init_8851e.h"
#endif
#if MAC_AX_8852D_SUPPORT
#include "mac_8852d/init_8852d.h"
#endif
#if MAC_AX_1115E_SUPPORT
#include "mac_1115e/init_1115e.h"
#endif

#include "role.h"
#include "fwdl.h"
#include "mport.h"
#include "phy_rpt.h"
#if MAC_AX_PCIE_SUPPORT
#include "_pcie.h"
#endif
/*--------------------Define -------------------------------------------*/
/*--------------------Define Enum---------------------------------------*/
/*--------------------Define Struct-------------------------------------*/

#ifdef CONFIG_NEW_HALMAC_INTERFACE

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief get_mac_ax_adapter
 *
 * @param intf
 * @param chip_id
 * @param cv
 * @param *phl_adapter
 * @param *drv_adapter
 * @param *pltfm_cb
 * @return Please Place Description here.
 * @retval  mac_ax_adapter
 */
struct mac_ax_adapter *get_mac_ax_adapter(enum mac_ax_intf intf,
					  u8 chip_id, u8 cv,
					  void *phl_adapter, void *drv_adapter,
					  struct mac_ax_pltfm_cb *pltfm_cb);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief get_mac_ax_adapter
 *
 * @param intf
 * @param chip_id
 * @param cv
 * @param *drv_adapter
 * @param *pltfm_cb
 * @return Please Place Description here.
 * @retval  mac_ax_adapter
 */
#else
struct mac_ax_adapter *get_mac_ax_adapter(enum mac_ax_intf intf,
					  u8 chip_id, u8 cv,
					  void *drv_adapter,
					  struct mac_ax_pltfm_cb *pltfm_cb);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief cmac_func_en
 *
 * @param *adapter
 * @param band
 * @param en
 * @return Please Place Description here.
 * @retval u32
 */
#endif
u32 cmac_func_en(struct mac_ax_adapter *adapter, u8 band, u8 en);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief mac_sys_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_sys_init(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief mac_hal_init
 *
 * @param *adapter
 * @param *trx_info
 * @param *fwdl_info
 * @param *intf_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hal_init(struct mac_ax_adapter *adapter,
		 struct mac_ax_trx_info *trx_info,
		 struct mac_ax_fwdl_info *fwdl_info,
		 struct mac_ax_intf_info *intf_info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief mac_hal_fast_init
 *
 * @param *adapter
 * @param *trx_info
 * @param *fwdl_info
 * @param *intf_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hal_fast_init(struct mac_ax_adapter *adapter,
		      struct mac_ax_trx_info *trx_info,
		      struct mac_ax_fwdl_info *fwdl_info,
		      struct mac_ax_intf_info *intf_info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief mac_hal_fast_deinit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hal_fast_deinit(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief mac_hal_deinit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hal_deinit(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup System
 * @{
 */

/**
 * @brief mac_ax_init_state
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_ax_init_state(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

u32 mix_info_init(struct mac_ax_adapter *adapter);
u32 mix_info_exit(struct mac_ax_adapter *adapter);

#endif
