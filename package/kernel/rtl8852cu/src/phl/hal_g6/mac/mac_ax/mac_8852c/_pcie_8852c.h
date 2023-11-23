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

#ifndef _MAC_AX_PCIE_8852C_H_
#define _MAC_AX_PCIE_8852C_H_

#include "../_pcie.h"
#include "../mac_priv.h"
#if MAC_AX_8852C_SUPPORT

#define LTR_EN_BITS (B_AX_LTR_HW_DEC_EN | B_AX_LTR_FW_DEC_EN | B_AX_LTR_DRV_DEC_EN)

enum pcie_ltr_idx {
	PCIE_LTR_IDX_ACT = 0,
	PCIE_LTR_IDX_1,
	PCIE_LTR_IDX_2,
	PCIE_LTR_IDX_IDLE,
	PCIE_LTR_IDX_MAX
};

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_pcie_info_def_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval struct mac_ax_intf_info *
 */
struct mac_ax_intf_info *
get_pcie_info_def_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_bdram_tbl_pcie_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval struct mac_ax_intf_info *
 */
struct txbd_ram *
get_bdram_tbl_pcie_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief mio_w32_pcie_8852c
 *
 * @param *adapter
 * @param addr
 * @param value
 * @return Please Place Description here.
 * @retval u32
 */
u32 mio_w32_pcie_8852c(struct mac_ax_adapter *adapter, u16 addr, u32 value);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief mio_r32_pcie_8852a
 *
 * @param *adapter
 * @param addr
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mio_r32_pcie_8852c(struct mac_ax_adapter *adapter, u16 addr, u32 *val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_txbd_reg_pcie_8852c
 *
 * @param *adapter
 * @param dma_ch
 * @param *reg
 * @param type
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_txbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch, u32 *reg,
			    enum pcie_bd_ctrl_type type);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief set_txbd_reg_pcie_8852c
 *
 * @param *adapter
 * @param dma_ch
 * @param type
 * @param val0
 * @param val1
 * @param val2
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_txbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch,
			    enum pcie_bd_ctrl_type type, u32 val0, u32 val1, u32 val2);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_rxbd_reg_pcie_8852c
 *
 * @param *adapter
 * @param dma_ch
 * @param *reg
 * @param type
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_rxbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch, u32 *reg,
			    enum pcie_bd_ctrl_type type);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief set_rxbd_reg_pcie_8852c
 *
 * @param *adapter
 * @param dma_ch
 * @param type
 * @param val0
 * @param val1
 * @param val2
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_rxbd_reg_pcie_8852c(struct mac_ax_adapter *adapter, u8 dma_ch,
			    enum pcie_bd_ctrl_type type, u32 val0, u32 val1, u32 val2);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief pcie_cfgspc_write_8852c
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 pcie_cfgspc_write_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_pcie_cfgspc_param *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief pcie_cfgspc_read_8852c
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 pcie_cfgspc_read_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_pcie_cfgspc_param *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief pcie_ltr_write_8852c
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 pcie_ltr_write_8852c(struct mac_ax_adapter *adapter,
			 struct mac_ax_pcie_ltr_param *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief pcie_ltr_read_8852c
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 pcie_ltr_read_8852c(struct mac_ax_adapter *adapter,
			struct mac_ax_pcie_ltr_param *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief ltr_sw_trigger_8852c
 *
 * @param *adapter
 * @param ctrl
 * @return Please Place Description here.
 * @retval u32
 */
u32 ltr_sw_trigger_8852c(struct mac_ax_adapter *adapter,
			 enum mac_ax_pcie_ltr_sw_ctrl ctrl);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_avail_txbd_8852c
 *
 * @param *adapter
 * @param ch_idx
 * @param *host_idx
 * @param *hw_idx
 * @param *avail_txbd
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_avail_txbd_8852c(struct mac_ax_adapter *adapter, u8 ch_idx,
			 u16 *host_idx, u16 *hw_idx, u16 *avail_txbd);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_avail_rxbd_8852c
 *
 * @param *adapter
 * @param ch_idx
 * @param *host_idx
 * @param *hw_idx
 * @param *avail_rxbd
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_avail_rxbd_8852c(struct mac_ax_adapter *adapter, u8 ch_idx,
			 u16 *host_idx, u16 *hw_idx, u16 *avail_rxbd);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_io_stat_pcie_8852c
 *
 * @param *adapter
 * @param out_st
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_io_stat_pcie_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_io_stat *out_st);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief ctrl_hci_dma_en_pcie_8852c
 *
 * @param *adapter
 * @param txen
 * @param rxen
 * @return Please Place Description here.
 * @retval u32
 */
u32 ctrl_hci_dma_en_pcie_8852c(struct mac_ax_adapter *adapter,
			       enum mac_ax_pcie_func_ctrl txen,
			       enum mac_ax_pcie_func_ctrl rxen);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief ctrl_trxdma_pcie_8852c
 *
 * @param *adapter
 * @param txen
 * @param rxen
 * @param ioen
 * @return Please Place Description here.
 * @retval u32
 */
u32 ctrl_trxdma_pcie_8852c(struct mac_ax_adapter *adapter,
			   enum mac_ax_pcie_func_ctrl txen,
			   enum mac_ax_pcie_func_ctrl rxen,
			   enum mac_ax_pcie_func_ctrl ioen);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief ctrl_txdma_ch_pcie_8852c
 *
 * @param *adapter
 * @param *ch_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 ctrl_txdma_ch_pcie_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_txdma_ch_map *ch_map);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief ctrl_wpdma_pcie_8852c
 *
 * @param *adapter
 * @param wpen
 * @return Please Place Description here.
 * @retval u32
 */
u32 ctrl_wpdma_pcie_8852c(struct mac_ax_adapter *adapter,
			  enum mac_ax_pcie_func_ctrl wpen);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief poll_io_idle_pcie_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 poll_io_idle_pcie_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief poll_txdma_ch_idle_pcie_8852c
 *
 * @param *adapter
 * @param *ch_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 poll_txdma_ch_idle_pcie_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_txdma_ch_map *ch_map);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief poll_rxdma_ch_idle_pcie_8852c
 *
 * @param *adapter
 * @param *ch_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 poll_rxdma_ch_idle_pcie_8852c(struct mac_ax_adapter *adapter,
				  struct mac_ax_rxdma_ch_map *ch_map);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief poll_dma_all_idle_pcie_8852c
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 poll_dma_all_idle_pcie_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief clr_idx_ch_pcie_8852c
 *
 * @param *adapter
 * @param *txch_map
 * @param *rxch_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 clr_idx_ch_pcie_8852c(struct mac_ax_adapter *adapter,
			  struct mac_ax_txdma_ch_map *txch_map,
			  struct mac_ax_rxdma_ch_map *rxch_map);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief rst_bdram_pcie_8852c
 *
 * @param *adapter
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 rst_bdram_pcie_8852c(struct mac_ax_adapter *adapter, u8 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief trx_mit_pcie_8852c
 *
 * @param *adapter
 * @param *mit_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 trx_mit_pcie_8852c(struct mac_ax_adapter *adapter,
		       struct mac_ax_pcie_trx_mitigation *mit_info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief mode_op_pcie_8852c
 *
 * @param *adapter
 * @param *intf_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mode_op_pcie_8852c(struct mac_ax_adapter *adapter,
		       struct mac_ax_intf_info *intf_info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_err_flag_pcie_8852c
 *
 * @param *adapter
 * @param *out_info
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_err_flag_pcie_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_pcie_err_info *out_info);
/**
 * @}
 * @}
 */

/**
 * @brief mac_auto_refclk_cal_pcie_8852c
 *
 * @param *adapter
 * @param en
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_auto_refclk_cal_pcie_8852c(struct mac_ax_adapter *adapter,
				   enum mac_ax_pcie_func_ctrl en);
/**
 * @}
 * @}
 */

/**
 * @brief set_pcie_speed_8852c
 *
 * @param *adapter
 * @param speed
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_pcie_speed_8852c(struct mac_ax_adapter *adapter,
			 enum mac_ax_pcie_phy speed);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

/**
 * @brief get_pcie_speed_8852c
 *
 * @param *adapter
 * @param *speed
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_pcie_speed_8852c(struct mac_ax_adapter *adapter,
			 u8 *speed);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup PCIE
 * @{
 */

#endif /* #if MAC_AX_8852C_SUPPORT */
#endif
