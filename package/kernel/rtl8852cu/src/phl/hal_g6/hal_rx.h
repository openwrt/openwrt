
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _HAL_RX_H_
#define _HAL_RX_H_

void
hal_rx_ppdu_sts_normal_data(struct rtw_phl_com_t *phl_com,
			    void *hdr,
			    struct rtw_r_meta_data *meta);

void
hal_rx_ppdu_sts(struct rtw_phl_com_t *phl_com,
		struct rtw_phl_rx_pkt *phl_rx,
		struct hal_ppdu_sts *ppdu_sts);

#ifdef CONFIG_PCI_HCI
/**
 * rtw_hal_query_rxch_num - query total hw rx dma channels number
 *
 * returns the number of  hw rx dma channel
 */
u8 rtw_hal_query_rxch_num(void *hal);
#endif

enum rtw_hal_status
hal_usb_rx_agg_cfg(struct hal_info_t *hal, u8 mode, u8 agg_mode, u8 drv_define,
		   u8 timeout, u8 size, u8 pkt_num);

enum rtw_hal_status rtw_hal_set_rxfltr_opt_by_mode(void *hal, u8 band,
					       enum rtw_rx_fltr_opt_mode mode);

enum rtw_hal_status rtw_hal_set_rxfltr_type_by_mode(void *hal, u8 band,
						    enum rtw_rxfltr_type_mode mode);

#endif /*_HAL_RX_H_*/
