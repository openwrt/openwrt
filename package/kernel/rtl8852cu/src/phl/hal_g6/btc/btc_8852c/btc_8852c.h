/******************************************************************************
 *
 * Copyright(c) 2016 - 2019 Realtek Corporation.
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
#ifndef __RTL8852C_BTC_H__
#define __RTL8852C_BTC_H__
/* rtl8852c_btc.c */

#define R_BTC_BT_CNT_CFG 0xDA10
#define R_BTC_COEX_WL_REQ 0xDA24
#define R_BTC_BREAK_TABLE 0xDA44
#define R_BTC_BT_CNT_HIGH 0xDA14
#define R_BTC_BT_CNT_LOW 0xDA18

#define B_BTC_TX_BCN_HI BIT(22)
#define B_BTC_TX_TRI_HI BIT(17)
#define B_BTC_RSP_ACK_HI BIT(10)
#define B_BTC_PRI_MASK_TX_TIME (BIT(4) | BIT(3))
#define B_BTC_PRI_MASK_RX_TIME_V1 (BIT(2) | BIT(1))
#define B_BTC_BT_CNT_RST_V1 BIT(1)
#define B_BTC_BT_CNT_EN BIT(0)

extern const struct btc_chip chip_8852c;
void _8852c_rfe_type(struct btc_t *btc);
void _8852c_init_cfg(struct btc_t *btc);
void _8852c_wl_pri (struct btc_t *btc, u8 map, bool state);
void _8852c_wl_tx_power(struct btc_t *btc, u32 level);
void _8852c_wl_rx_gain(struct btc_t *btc, u32 level);
void _8852c_wl_s1_standby(struct btc_t *btc, u32 state);
void _8852c_wl_req_mac(struct btc_t *btc, u8 mac_id);
void _8852c_update_bt_cnt(struct btc_t *btc);
u8 _8852c_bt_rssi(struct btc_t *btc, u8 val);
#endif /*__RTL8852C_PHY_H__*/
