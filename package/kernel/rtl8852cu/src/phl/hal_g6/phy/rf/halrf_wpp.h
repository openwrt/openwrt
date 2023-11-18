/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef __HALRF_WPP_H__
#define __HALRF_WPP_H__

#define HALRF_WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID( \
		HalrfGuid, (C01A40AB,B69E,4CF6,BB09,4E0672EA2FC6), \
		WPP_DEFINE_BIT(DBG_RF_TX_PWR_TRACK) \
		WPP_DEFINE_BIT(DBG_RF_IQK) \
		WPP_DEFINE_BIT(DBG_RF_LCK) \
		WPP_DEFINE_BIT(DBG_RF_DPK) \
		WPP_DEFINE_BIT(DBG_RF_TXGAPK) \
		WPP_DEFINE_BIT(DBG_RF_DACK) \
		WPP_DEFINE_BIT(DBG_RF_DPK_TRACK) \
		WPP_DEFINE_BIT(DBG_RF_RXDCK) \
		WPP_DEFINE_BIT(DBG_RF_RFK) \
		WPP_DEFINE_BIT(DBG_RF_INIT) \
		WPP_DEFINE_BIT(DBG_RF_POWER) \
		WPP_DEFINE_BIT(DBG_RF_RXGAINK) \
		WPP_DEFINE_BIT(DBG_RF_THER_TRIM) \
		WPP_DEFINE_BIT(DBG_RF_PABIAS_TRIM) \
		WPP_DEFINE_BIT(DBG_RF_TSSI_TRIM) \
		WPP_DEFINE_BIT(DBG_RF_FW) \
		WPP_DEFINE_BIT(DBG_RF_MP) \
		WPP_DEFINE_BIT(DBG_RF_TMP) \
		WPP_DEFINE_BIT(DBG_RF_PSD) \
		)

#endif /* __HALRF_WPP_H__ */