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

#ifndef _MAC_AX_TRX_DESC_8852C_H_
#define _MAC_AX_TRX_DESC_8852C_H_

#include "../../type.h"
#include "../role.h"
#if MAC_AX_8852C_SUPPORT

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_Config
 * @{
 */
/**
 * @brief mac_txdesc_len
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_txdesc_len_8852c(struct mac_ax_adapter *adapter,
			 struct rtw_t_meta_data *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_Config
 * @{
 */

/**
 * @brief mac_build_txdesc
 *
 * @param *adapter
 * @param *info
 * @param *buf
 * @param len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_build_txdesc_8852c(struct mac_ax_adapter *adapter,
			   struct rtw_t_meta_data *info, u8 *buf, u32 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_Config
 * @{
 */

/**
 * @brief mac_refill_txdesc
 *
 * @param *adapter
 * @param *txpkt_info
 * @param *mask
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_refill_txdesc_8852c(struct mac_ax_adapter *adapter,
			    struct rtw_t_meta_data *txpkt_info,
			    struct mac_ax_refill_info *mask,
			    struct mac_ax_refill_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_Config
 * @{
 */

/**
 * @brief mac_parse_rxdesc
 *
 * @param *adapter
 * @param *info
 * @param *buf
 * @param len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_parse_rxdesc_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_Config
 * @{
 */

/**
 * @brief mac_wd_checksum
 *
 * @param *adapter
 * @param *info
 * @param *wddesc
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_wd_checksum_8852c(struct mac_ax_adapter *adapter,
			  struct rtw_t_meta_data *info, u8 *wddesc);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup TX_Config
 * @{
 */

/**
 * @brief mac_patch_rx_rate_8852c
 *
 * @param *adapter
 * @param *rtw_r_meta_data
 * @return For RMAC rate patch func
 * @retval u32
 */
u32 mac_patch_rx_rate_8852c(struct mac_ax_adapter *adapter,
			    struct rtw_r_meta_data *info);
/**
 * @}
 * @}
 */

/**
 * @brief mac_get_wp_offset_8852c
 *
 * @param *adapter
 * @param *ofld_conf
 * @param *val
 * @return error code
 * @retval u32
 */
u32 mac_get_wp_offset_8852c(struct mac_ax_adapter *adapter,
			    struct mac_txd_ofld_wp_offset *ofld_conf, u16 *val);
/**
 * @}
 * @}
 */

#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
