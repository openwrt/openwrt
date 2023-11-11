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

#ifndef _MAC_AX_LA_MODE_H_
#define _MAC_AX_LA_MODE_H_

#include "../type.h"
#include "../mac_ax.h"

/*--------------------Define ----------------------------------------*/
#define BITS_AX_LA_BUF_CFG 0xFFF00
#define BITS_AX_LA_CFG 0xFFF000FF
#define LA_SIZE_192K_BUF_BNDY 0x20
#define LA_SIZE_256K_BUF_BNDY 0x18

#define LA_BUF_SEL_256K 0x3
#define LA_BUF_SEL_192K 0x2
#define LA_BUF_SEL_128K 0x1
#define LA_BUF_SEL_64K 0x0

#define DLE_BLOCK_SIZE 0x2000
#define DLE_BUF_BNDY_8852A 0x70000
#define DLE_BUF_BNDY_8852B 0x20000
#define DLE_BUF_BNDY_8851B 0x20000
#define LA_BUF_END_8852C_256K 0x70000
#define LA_BUF_END_8852C_128K 0x50000
#define LA_BUF_END_8192XB_256K 0x70000
#define LA_BUF_END_8192XB_128K 0x50000
#define LA_BUF_END_8851E_256K 0x70000
#define LA_BUF_END_8851E_128K 0x50000
#define LA_BUF_END_8852D_256K 0x70000
#define LA_BUF_END_8852D_128K 0x50000
#define LA_SIZE_128K_BUF_BNDY_8852B 0x8
#define LA_SIZE_128K_BUF_BNDY_8851B 0x8

/*--------------------Define Enum------------------------------------*/
/*--------------------Define MACRO----------------------------------*/
/*--------------------Define Struct-----------------------------------*/
/*--------------------Export global variable----------------------------*/
/*--------------------Function declaration-----------------------------*/

/**
 * @addtogroup Common
 * @{
 * @addtogroup BB_Related
 * @{
 */
/**
 * @brief mac_lamode_cfg
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_lamode_cfg(struct mac_ax_adapter *adapter,
		   struct mac_ax_la_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BB_Related
 * @{
 */

/**
 * @brief mac_lamode_trigger
 *
 * @param *adapter
 * @param tgr
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_lamode_trigger(struct mac_ax_adapter *adapter, u8 tgr);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BB_Related
 * @{
 */

/**
 * @brief mac_lamode_buf_cfg
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_lamode_buf_cfg(struct mac_ax_adapter *adapter,
		       struct mac_ax_la_buf_param *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup BB_Related
 * @{
 */

/**
 * @brief mac_get_lamode_st
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval  mac_ax_la_status
 */
struct mac_ax_la_status mac_get_lamode_st
			(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

#endif
