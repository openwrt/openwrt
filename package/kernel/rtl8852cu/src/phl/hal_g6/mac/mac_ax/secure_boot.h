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

#ifndef _MAC_AX_SECURE_BOOT_H_
#define _MAC_AX_SECURE_BOOT_H_

#include "../type.h"
#include "efuse.h"
//#include "fwcmd.h"

//#if WIFI_HAL_G6

#define OTP_SEC_DIS_ZONE_BASE 0x0
#define OTP_SEC_DIS_ZONE_SIZE 4
#define OTP_SECURE_ZONE_BASE 0x4C0
#define OTP_SECURE_ZONE_SIZE 192

#define OTP_KEY_INFO_CELL_01_ADDR 0x5EC
#define OTP_KEY_INFO_CELL_02_ADDR 0x5ED

#define OTP_PHY_BASE 0x0
#define OTP_PHY_1ST_B_OFFSET 0
#define OTP_PHY_2ND_B_OFFSET 1
#define OTP_PHY_3RD_B_OFFSET 2
#define OTP_PHY_4TH_B_OFFSET 3

#define SEC_B_VAL 0xFC
#define SIC_B_VAL 0xF3
#define JTAG_B_VAL 0xCF
#define UART_TX_B_VAL 0x3F
#define UART_RX_B_VAL 0xFC

enum mac_sec_mode {
	MAC_NON_SEC,
	MAC_SEC,
};

enum mac_sic_mode {
	MAC_SIC_EN,
	MAC_SIC_DIS,
};

enum mac_jtag_mode {
	MAC_JTAG_EN,
	MAC_JTAG_DIS,
};

enum mac_uart_tx_mode {
	MAC_UART_TX_EN,
	MAC_UART_TX_DIS,
};

enum mac_uart_rx_mode {
	MAC_UART_RX_EN,
	MAC_UART_RX_DIS,
};

// externalPN  = 0x5EC[7:0]
// customer    = 0x5ED[3:0]
// serialNum   = 0x5ED[6:4]
// securityRec = 0x5ED[7:7]
#define _external_pn(byte)   ((byte & 0xFF) >> 0)
#define _customer(byte)     ((byte & 0x0F) >> 0)
#define _serial_num(byte)    ((byte & 0x70) >> 4)
#define _security_rec(byte)  ((byte & 0x80) >> 7)

#define _1st_double_bits(byte) ((byte & 0x03) >> 0)
#define _2nd_double_bits(byte) ((byte & 0x0C) >> 2)
#define _3rd_double_bits(byte) ((byte & 0x30) >> 4)
#define _4th_double_bits(byte) ((byte & 0xC0) >> 6)

u32 mac_chk_sec_rec(struct mac_ax_adapter *adapter, u8 *sec_mode);
u32 mac_pg_sec_phy_wifi(struct mac_ax_adapter *adapter);
u32 mac_cmp_sec_phy_wifi(struct mac_ax_adapter *adapter);
u32 mac_pg_sec_hid_wifi(struct mac_ax_adapter *adapter);
u32 mac_cmp_sec_hid_wifi(struct mac_ax_adapter *adapter);
u32 mac_pg_sec_dis(struct mac_ax_adapter *adapter);
u32 mac_cmp_sec_dis(struct mac_ax_adapter *adapter);
u32 mac_sic_dis(struct mac_ax_adapter *adapter);
u32 mac_chk_sic_dis(struct mac_ax_adapter *adapter, u8 *sic_mode);
u32 mac_jtag_dis(struct mac_ax_adapter *adapter);
u32 mac_chk_jtag_dis(struct mac_ax_adapter *adapter, u8 *jtag_mode);
u32 mac_uart_tx_dis(struct mac_ax_adapter *adapter);
u32 mac_chk_uart_tx_dis(struct mac_ax_adapter *adapter, u8 *uart_tx_mode);
u32 mac_uart_rx_dis(struct mac_ax_adapter *adapter);
u32 mac_chk_uart_rx_dis(struct mac_ax_adapter *adapter, u8 *uart_rx_mode);

#endif
//#endif