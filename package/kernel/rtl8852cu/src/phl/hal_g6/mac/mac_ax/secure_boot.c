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

#include "secure_boot.h"

//#if WIFI_HAL_G6

// OTP zone1 map
u8 otp_sec_dis_zone_map_v01[] = {0xFC, 0xFF, 0xFF, 0x3F};

// OTP zone3 map
u8 otp_secure_zone_map_v01[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x99, 0xB6, 0x34, 0xA0, 0xCD, 0xA1, 0x6B,
	0xA1, 0xC5, 0x06, 0x8F, 0xCA, 0xDC, 0xE2, 0x95,
	0xA5, 0xCC, 0x8B, 0x33, 0x13, 0x6E, 0x4F, 0x28,
	0xFC, 0x1A, 0xE7, 0x91, 0x84, 0x4F, 0x62, 0x43,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// OTP zone4 values
u8 otp_key_info_cell_01_val = 0xFF; // OTP 0x5EC
u8 otp_key_info_cell_02_val = 0x6E; // OTP 0x5ED

u32 mac_chk_sec_rec(struct mac_ax_adapter *adapter, u8 *sec_mode)
{
	u32 ret = 0;
	u8 byte_val = 0;
	u8 sec_rec;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_key_info_cell_02_addr = OTP_KEY_INFO_CELL_02_ADDR;

	// read efuse OTP_KEY_INFO_CELL_02 to byte_val
	ret = mac_ops->read_efuse(adapter, otp_key_info_cell_02_addr,
				  1, &byte_val, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] chk_sec_rec read_efuse fail!\n");
		return ret;
	}

	sec_rec = _security_rec(byte_val);

	PLTFM_MSG_TRACE("[TRACE] sec_rec = %x\n", sec_rec);
	if (sec_rec == 0x0)
		*sec_mode = MAC_SEC;
	else
		*sec_mode = MAC_NON_SEC;

	if (*sec_mode == MAC_SEC)
		adapter->hw_info->is_sec_ic = 1;
	else
		adapter->hw_info->is_sec_ic = 0;

	return MACSUCCESS;
}

u32 mac_pg_sec_phy_wifi(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;
	u32 i;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_secure_zone_base = OTP_SECURE_ZONE_BASE;

	for (i = 0; i < OTP_SECURE_ZONE_SIZE; i++) {
		// write OTP_SECURE_ZONE_BASE + i = otp_secure_zone_map_v01[i]
		if (otp_secure_zone_map_v01[i] == 0xFF) {
			continue;
		} else {
			ret = mac_ops->write_efuse(adapter, otp_secure_zone_base + i,
						   otp_secure_zone_map_v01[i],
						   MAC_AX_EFUSE_BANK_WIFI);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR] pg_sec_phy_wifi write_efuse fail!\n");
				return ret;
			}
		}
	}

	PLTFM_MSG_TRACE("[TRACE] pg_sec_phy_wifi success!\n");
	return MACSUCCESS;
}

u32 mac_cmp_sec_phy_wifi(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;
	u32 i;
	u8 byte_val[OTP_SECURE_ZONE_SIZE];
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_secure_zone_base = OTP_SECURE_ZONE_BASE;
	u32 otp_secure_zone_size = OTP_SECURE_ZONE_SIZE;

	ret = mac_ops->read_efuse(adapter, otp_secure_zone_base,
				  otp_secure_zone_size, byte_val,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] cmp_sec_phy_wifi read_efuse fail!\n");
		return ret;
	}

	for (i = 0; i < OTP_SECURE_ZONE_SIZE; i++) {
		if (byte_val[i] != otp_secure_zone_map_v01[i]) {
			PLTFM_MSG_ERR("[ERR] cmp_sec_phy_wifi fail!\n");
			return MACEFUSECMP;
		}
	}

	PLTFM_MSG_TRACE("[TRACE] cmp_sec_phy_wifi success!\n");
	return MACSUCCESS;
}

u32 mac_pg_sec_hid_wifi(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_key_info_cell_01_addr = OTP_KEY_INFO_CELL_01_ADDR;
	u32 otp_key_info_cell_02_addr = OTP_KEY_INFO_CELL_02_ADDR;

	// write OTP_KEY_INFO_CELL_01_ADDR = otp_key_info_cell_01_val
	if (otp_key_info_cell_01_val == 0xFF) {
		// do nothing
	} else {
		ret = mac_ops->write_efuse(adapter, otp_key_info_cell_01_addr,
					   otp_key_info_cell_01_val,
					   MAC_AX_EFUSE_BANK_WIFI);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] pg_sec_hid_wifi write_efuse fail!\n");
			return ret;
		}
	}

	// write OTP_KEY_INFO_CELL_02_ADDR = otp_key_info_cell_02_val
	ret = mac_ops->write_efuse(adapter, otp_key_info_cell_02_addr,
				   otp_key_info_cell_02_val,
				   MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] pg_sec_hid_wifi write_efuse fail!\n");
		return ret;
	}

	PLTFM_MSG_TRACE("[TRACE] pg_sec_hid_wifi success!\n");
	return MACSUCCESS;
}

u32 mac_cmp_sec_hid_wifi(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;
	u8 byte_val;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_key_info_cell_01_addr = OTP_KEY_INFO_CELL_01_ADDR;
	u32 otp_key_info_cell_02_addr = OTP_KEY_INFO_CELL_02_ADDR;

	ret = mac_ops->read_efuse(adapter, otp_key_info_cell_01_addr,
				  1, &byte_val, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] cmp_sec_hid_wifi read_efuse fail!\n");
		return ret;
	}

	if (byte_val != otp_key_info_cell_01_val) {
		PLTFM_MSG_ERR("[ERR] cmp_sec_hid_wifi cell_01 fail!\n");
		return MACEFUSECMP;
	}

	ret = mac_ops->read_efuse(adapter, otp_key_info_cell_02_addr,
				  1, &byte_val, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] cmp_sec_hid_wifi read_efuse fail!\n");
		return ret;
	}

	if (byte_val != otp_key_info_cell_02_val) {
		PLTFM_MSG_ERR("[ERR] cmp_sec_hid_wifi cell_02 fail!\n");
		return MACEFUSECMP;
	}

	PLTFM_MSG_TRACE("[TRACE] cmp_sec_hid_wifi success!\n");
	return MACSUCCESS;
}

u32 mac_pg_sec_dis(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;
	u32 i;
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_sec_dis_zone_base = OTP_SEC_DIS_ZONE_BASE;

	// write efuse OTP_SEC_CTRL_ZONE_BASE
	for (i = 0; i < OTP_SEC_DIS_ZONE_SIZE; i++) {
		// write OTP_SEC_DIS_ZONE_BASE + i = otp_sec_dis_zone_map_v01[i]
		if (otp_sec_dis_zone_map_v01[i] == 0xFF) {
			continue;
		} else {
			ret = mac_ops->write_efuse(adapter, otp_sec_dis_zone_base + i,
						   otp_sec_dis_zone_map_v01[i],
						   MAC_AX_EFUSE_BANK_WIFI);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR] pg_sec_dis write_efuse fail!\n");
				return ret;
			}
		}
	}

	PLTFM_MSG_TRACE("[TRACE] pg_sec_dis success!\n");
	return MACSUCCESS;
}

u32 mac_cmp_sec_dis(struct mac_ax_adapter *adapter)
{
	u32 ret = 0;
	u32 i;
	u8 byte_val[OTP_SEC_DIS_ZONE_SIZE];
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 otp_sec_dis_zone_base = OTP_SEC_DIS_ZONE_BASE;
	u32 otp_sec_dis_zone_size = OTP_SEC_DIS_ZONE_SIZE;

	ret = mac_ops->read_efuse(adapter, otp_sec_dis_zone_base,
				  otp_sec_dis_zone_size, byte_val,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] cmp_sec_dis read_efuse fail!\n");
		return ret;
	}

	for (i = 0; i < OTP_SEC_DIS_ZONE_SIZE; i++) {
		if (byte_val[i] != otp_sec_dis_zone_map_v01[i]) {
			PLTFM_MSG_ERR("[ERR] cmp_sec_dis fail!\n");
			return MACEFUSECMP;
		}
	}

	PLTFM_MSG_TRACE("[TRACE] cmp_sec_dis success!\n");
	return MACSUCCESS;
}

u32 mac_sic_dis(struct mac_ax_adapter *adapter)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_1ST_B_OFFSET;
	u8 byte = SIC_B_VAL;
	u8 byte_tmp[1];

	ret = mac_ops->write_efuse(adapter, addr, byte,
				   MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] sic_dis write_efuse fail!\n");
		return ret;
	}

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] sic_dis read_efuse fail!\n");
		return ret;
	}

	if (_2nd_double_bits(byte_tmp[0]) != _2nd_double_bits(byte)) {
		PLTFM_MSG_ERR("[ERR] sic_dis cmp fail!\n");
		return MACEFUSECMP;
	}

	PLTFM_MSG_TRACE("[TRACE] sic_dis success!\n");
	return MACSUCCESS;
}

u32 mac_chk_sic_dis(struct mac_ax_adapter *adapter, u8 *sic_mode)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_1ST_B_OFFSET;
	u8 byte_tmp[1];

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] chk_sic_dis read_efuse fail!\n");
		return ret;
	}

	if (_2nd_double_bits(byte_tmp[0]) == 0x0)
		*sic_mode = MAC_SIC_EN;
	else
		*sic_mode = MAC_SIC_DIS;

	PLTFM_MSG_TRACE("[TRACE] chk_sic_dis success!\n");
	return MACSUCCESS;
}

u32 mac_jtag_dis(struct mac_ax_adapter *adapter)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_1ST_B_OFFSET;
	u8 byte = JTAG_B_VAL;
	u8 byte_tmp[1];

	ret = mac_ops->write_efuse(adapter, addr, byte,
				   MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] jtag_dis write_efuse fail!\n");
		return ret;
	}

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] jtag_dis read_efuse fail!\n");
		return ret;
	}

	if (_3rd_double_bits(byte_tmp[0]) != _3rd_double_bits(byte)) {
		PLTFM_MSG_ERR("[ERR] jtag_dis cmp fail!\n");
		return MACEFUSECMP;
	}

	PLTFM_MSG_TRACE("[TRACE] jtag_dis success!\n");
	return MACSUCCESS;
}

u32 mac_chk_jtag_dis(struct mac_ax_adapter *adapter, u8 *jtag_mode)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_1ST_B_OFFSET;
	u8 byte_tmp[1];

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] chk_jtag_dis read_efuse fail!\n");
		return ret;
	}

	if (_3rd_double_bits(byte_tmp[0]) == 0x0)
		*jtag_mode = MAC_JTAG_EN;
	else
		*jtag_mode = MAC_JTAG_DIS;

	PLTFM_MSG_TRACE("[TRACE] chk_jtag_dis success!\n");
	return MACSUCCESS;
}

u32 mac_uart_tx_dis(struct mac_ax_adapter *adapter)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_1ST_B_OFFSET;
	u8 byte = UART_TX_B_VAL;
	u8 byte_tmp[1];

	ret = mac_ops->write_efuse(adapter, addr, byte,
				   MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] uart_tx_dis write_efuse fail!\n");
		return ret;
	}

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] uart_tx_dis read_efuse fail!\n");
		return ret;
	}

	if (_4th_double_bits(byte_tmp[0]) != _4th_double_bits(byte)) {
		PLTFM_MSG_ERR("[ERR] uart_tx_dis cmp fail!\n");
		return MACEFUSECMP;
	}

	PLTFM_MSG_TRACE("[TRACE] uart_tx_dis success!\n");
	return MACSUCCESS;
}

u32 mac_chk_uart_tx_dis(struct mac_ax_adapter *adapter, u8 *uart_tx_mode)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_1ST_B_OFFSET;
	u8 byte_tmp[1];

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] chk_uart_tx_dis read_efuse fail!\n");
		return ret;
	}

	if (_4th_double_bits(byte_tmp[0]) == 0x0)
		*uart_tx_mode = MAC_UART_TX_EN;
	else
		*uart_tx_mode = MAC_UART_TX_DIS;

	PLTFM_MSG_TRACE("[TRACE] chk_uart_tx_dis success!\n");
	return MACSUCCESS;
}

u32 mac_uart_rx_dis(struct mac_ax_adapter *adapter)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_2ND_B_OFFSET;
	u8 byte = UART_RX_B_VAL;
	u8 byte_tmp[1];

	ret = mac_ops->write_efuse(adapter, addr, byte,
				   MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] uart_rx_dis write_efuse fail!\n");
		return ret;
	}

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] uart_rx_dis read_efuse fail!\n");
		return ret;
	}

	if (_1st_double_bits(byte_tmp[0]) != _1st_double_bits(byte)) {
		PLTFM_MSG_ERR("[ERR] uart_rx_dis cmp fail!\n");
		return MACEFUSECMP;
	}

	PLTFM_MSG_TRACE("[TRACE] uart_rx_dis success!\n");
	return MACSUCCESS;
}

u32 mac_chk_uart_rx_dis(struct mac_ax_adapter *adapter, u8 *uart_rx_mode)
{
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	u32 ret;
	u32 addr = OTP_PHY_BASE + OTP_PHY_2ND_B_OFFSET;
	u8 byte_tmp[1];

	ret = mac_ops->read_efuse(adapter, addr, 1, byte_tmp,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] chk_uart_rx_dis read_efuse fail!\n");
		return ret;
	}

	if (_1st_double_bits(byte_tmp[0]) == 0x0)
		*uart_rx_mode = MAC_UART_RX_EN;
	else
		*uart_rx_mode = MAC_UART_RX_DIS;

	PLTFM_MSG_TRACE("[TRACE] chk_uart_rx_dis success!\n");
	return MACSUCCESS;
}

//#endif