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

#ifndef _MAC_AX_HCIFC_H_
#define _MAC_AX_HCIFC_H_

#include "../type.h"
#include "trxcfg.h"
#include "../mac_ax.h"

#define HFC_PAGE_UNIT 64

/**
 * @enum mac_ax_hfc_full_cond
 *
 * @brief mac_ax_hfc_full_cond
 *
 * @var mac_ax_hfc_full_cond::MAC_AX_HFC_FULL_COND_X1
 * Please Place Description here.
 * @var mac_ax_hfc_full_cond::MAC_AX_HFC_FULL_COND_X2
 * Please Place Description here.
 * @var mac_ax_hfc_full_cond::MAC_AX_HFC_FULL_COND_X3
 * Please Place Description here.
 * @var mac_ax_hfc_full_cond::MAC_AX_HFC_FULL_COND_X4
 * Please Place Description here.
 */
enum mac_ax_hfc_full_cond {
	MAC_AX_HFC_FULL_COND_X1 = 0,
	MAC_AX_HFC_FULL_COND_X2,
	MAC_AX_HFC_FULL_COND_X3,
	MAC_AX_HFC_FULL_COND_X4
};

/**
 * @addtogroup HCI
 * @{
 * @addtogroup HCI_FlowControl
 * @{
 */
/**
 * @brief hfc_ch_ctrl
 *
 * @param *adapter
 * @param ch
 * @return Please Place Description here.
 * @retval u32
 */
u32 hfc_ch_ctrl(struct mac_ax_adapter *adapter, u8 ch);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup HCI_FlowControl
 * @{
 */

/**
 * @brief hfc_upd_ch_info
 *
 * @param *adapter
 * @param ch
 * @return Please Place Description here.
 * @retval u32
 */
u32 hfc_upd_ch_info(struct mac_ax_adapter *adapter, u8 ch);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup HCI_FlowControl
 * @{
 */

/**
 * @brief hfc_pub_ctrl
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 hfc_pub_ctrl(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup HCI_FlowControl
 * @{
 */

/**
 * @brief hfc_upd_mix_info
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 hfc_upd_mix_info(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup HCI_FlowControl
 * @{
 */

/**
 * @brief hfc_init
 *
 * @param *adapter
 * @param rst
 * @param en
 * @param h2c_en
 * @return Please Place Description here.
 * @retval u32
 */
u32 hfc_init(struct mac_ax_adapter *adapter, u8 rst, u8 en, u8 h2c_en);
/**
 * @}
 * @}
 */

u32 hfc_info_init(struct mac_ax_adapter *adapter);

u32 hfc_info_exit(struct mac_ax_adapter *adapter);

#endif
