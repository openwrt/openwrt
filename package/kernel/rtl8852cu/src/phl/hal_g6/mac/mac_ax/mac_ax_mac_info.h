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

#ifndef _MAC_AX_MAC_INFO_H_
#define _MAC_AX_MAC_INFO_H_

/* dword0 */
#define AX_MAC_INFO_USR_NUM_SH		0
#define AX_MAC_INFO_USR_NUM_MSK		0xf
#define AX_MAC_INFO_FW_DEFINE_SH		8
#define AX_MAC_INFO_FW_DEFINE_MSK		0xff
#define AX_MAC_INFO_LSIG_LEN_SH		16
#define AX_MAC_INFO_LSIG_LEN_MSK		0xfff
#define AX_MAC_INFO_IS_TO_SELF		BIT(28)
#define AX_MAC_INFO_RX_CNT_VLD		BIT(29)
#define AX_MAC_INFO_LONG_RXD_SH		30
#define AX_MAC_INFO_LONG_RXD_MSK		0x3

/* dword1 */
#define AX_MAC_INFO_SERVICE_SH		0
#define AX_MAC_INFO_SERVICE_MSK		0xffff
#define AX_MAC_INFO_PLCP_LEN_SH		16
#define AX_MAC_INFO_PLCP_LEN_MSK		0xff

/* dword2 */
#define AX_MAC_INFO_MAC_ID_VALID		BIT(0)
#define AX_MAC_INFO_HAS_DATA		BIT(1)
#define AX_MAC_INFO_HAS_CTRL		BIT(2)
#define AX_MAC_INFO_HAS_MGNT		BIT(3)
#define AX_MAC_INFO_HAS_BCN		BIT(4)
#define AX_MAC_INFO_MACID_SH		8
#define AX_MAC_INFO_MACID_MSK		0xff

#endif
