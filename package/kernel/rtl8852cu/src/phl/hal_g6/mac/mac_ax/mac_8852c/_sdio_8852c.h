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

#ifndef _MAC_AX_SDIO_8852C_H_
#define _MAC_AX_SDIO_8852C_H_

#include "../_sdio.h"

#if MAC_AX_8852C_SUPPORT
#if MAC_AX_SDIO_SUPPORT
/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief get_sdio_rx_req_len
 *
 * @param *adapter
 * @param *rx_req_len
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_sdio_rx_req_len_8852c(struct mac_ax_adapter *adapter, u32 *rx_req_len);

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief r_indir_cmd52_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @return Please Place Description here.
 * @retval u8
 */
u8 r_indir_cmd52_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr);

/**
 * @brief _r_indir_cmd52_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @return Please Place Description here.
 * @retval u8
 */
u8 _r_indir_cmd52_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr);

/**
 * @brief _r_indir_cmd53_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @return Please Place Description here.
 * @retval u32
 */
u32 _r_indir_cmd53_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr);

/**
 * @brief r16_indir_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @return Please Place Description here.
 * @retval u32
 */
u32 r16_indir_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr);

/**
 * @brief r32_indir_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @return Please Place Description here.
 * @retval u32
 */
u32 r32_indir_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr);

/**
 * @brief w_indir_cmd52_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @param val
 * @param size
 * @return Please Place Description here.
 * @retval void
 */
void w_indir_cmd52_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			      enum sdio_io_size size);

/**
 * @brief w_indir_cmd53_sdio_8852c
 *
 * @param *adapter
 * @param adr
 * @param val
 * @param size
 * @return Please Place Description here.
 * @retval void
 */
void w_indir_cmd53_sdio_8852c(struct mac_ax_adapter *adapter, u32 adr, u32 val,
			      enum sdio_io_size size);

/**
 * @brief ud_fs_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval void
 */
void ud_fs_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief sdio_pre_init_8852c
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 sdio_pre_init_8852c(struct mac_ax_adapter *adapter, void *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief tx_mode_cfg_sdio_8852c
 *
 * @param *adapter
 * @param mode
 * @return Please Place Description here.
 * @retval u32
 */
u32 tx_mode_cfg_sdio_8852c(struct mac_ax_adapter *adapter,
			   enum mac_ax_sdio_tx_mode mode);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief leave_suspend_sdio_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 leave_suspend_sdio_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief get_int_latency_sdio_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_int_latency_sdio_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief get_clk_cnt_sdio_8852c
 *
 * @param *adapter
 * @param *cnt
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_clk_cnt_sdio_8852c(struct mac_ax_adapter *adapter, u32 *cnt);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief set_wt_cfg_sdio_8852c
 *
 * @param *adapter
 * @param en
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_wt_cfg_sdio_8852c(struct mac_ax_adapter *adapter, u8 en);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief set_clk_mon_sdio_8852c
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_clk_mon_sdio_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_sdio_clk_mon_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief sdio_pwr_switch_8852c
 *
 * @param *vadapter
 * @param pre_switch
 * @param on
 * @return Please Place Description here.
 * @retval u32
 */
u32 sdio_pwr_switch_8852c(void *vadapter, u8 pre_switch, u8 on);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief rx_agg_cfg_sdio_8852c
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval void
 */
void rx_agg_cfg_sdio_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rx_agg_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup SDIO
 * @{
 */

/**
 * @brief aval_page_cfg_sdio_8852c
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval void
 */
void aval_page_cfg_sdio_8852c(struct mac_ax_adapter *adapter,
			      struct mac_ax_aval_page_cfg *cfg);
/**
 * @}
 * @}
 */

#endif /*MAC_AX_SDIO_SUPPORT*/
#endif /* MAC_AX_8852C_SUPPORT */
#endif
