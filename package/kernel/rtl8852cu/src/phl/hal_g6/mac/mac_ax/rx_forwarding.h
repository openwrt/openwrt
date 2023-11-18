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

#ifndef _MAC_AX_RX_FORWARDING_H_
#define _MAC_AX_RX_FORWARDING_H_

#include "../type.h"

/*--------------------Define ----------------------------------------*/
// Payload Match CAM
#define PM_CAM_OFFSET_DW0		0
#define PM_CAM_OFFSET_DW1		1
#define PM_CAM_OFFSET_DW2		2
#define PM_CAM_OFFSET_DW3		3
#define PM_CAM_OFFSET_DW4		4

/* dword0 */
#define PM_CAM_VALID			BIT(0)
#define PM_CAM_TYPE_SH			1
#define PM_CAM_TYPE_MSK			0x3
#define PM_CAM_SUBTYPE_SH		3
#define PM_CAM_SUBTYPE_MSK		0xf
#define PM_CAM_SKIP_MAC_IV_HDR	BIT(7)
#define PM_CAM_TARGET_IND_SH	8
#define PM_CAM_TARGET_IND_MSK	0x7
#define PM_CAM_CRC16_SH			16
#define PM_CAM_CRC16_MSK		0xffff

/* dword1 */
#define PM_CAM_PLD_MASK0_SH		0
#define PM_CAM_PLD_MASK0_MSK	0xffffffff

/* dword2 */
#define PM_CAM_PLD_MASK1_SH		0
#define PM_CAM_PLD_MASK1_MSK	0xffffffff

/* dword3 */
#define PM_CAM_PLD_MASK2_SH		0
#define PM_CAM_PLD_MASK2_MSK	0xffffffff

/* dword4 */
#define PM_CAM_PLD_MASK3_SH		0
#define PM_CAM_PLD_MASK3_MSK	0xffffffff

#define MAC_AX_PM_CAM_ENTRY_CONTENT_SIZE	20
#define MAC_AX_PM_CAM_ENTRY_NUM_MAX			16

#define PM_CAM_WAIT_CNT			2000
#define PM_CAM_WAIT_US			1

/*--------------------Define Enum------------------------------------*/

/*--------------------Define MACRO----------------------------------*/

/*--------------------Define Struct-----------------------------------*/

/*--------------------Export global variable----------------------------*/

/*--------------------Function declaration-----------------------------*/

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup RX_Forwarding
 * @{
 */

/**
 * @brief mac_set_rx_forwarding:
 *    Set rx forwarding feature:
 *	1. MAC_AX_FT_ACTION: for action frame
 *	2. MAC_AX_FT_ACTION_UD: for action frame with user define
 *	3. MAC_AX_FT_TRIGGER: for trigger frame
 *	4. MAC_AX_FT_PM_CAM: for Payload match CAM
 *
 * @param *adapter
 * @param *rf_ctrl_p
 * @return Please Place Description here.
 * @retval u32
 * #if MAC_AX_FW_REG_OFLD
 *	1. MACSUCCESS		0
 *	2. MACNPTR		5
 *	3. MACNOBUF		9
 *	4. MACFWNONRDY	80
 * #else
 *	1. MACSUCCESS		0
 *	2. MACNPTR		5
 *	3. MACNOITEM		11
 *	4. MACPOLLTO		12
 *	5. MACRFPMCAM		42
 * #endif
 */
u32 mac_set_rx_forwarding(struct mac_ax_adapter *adapter,
			  struct mac_ax_rx_fwd_ctrl_t *rf_ctrl_p);
/**
 * @}
 * @}
 */

#endif
