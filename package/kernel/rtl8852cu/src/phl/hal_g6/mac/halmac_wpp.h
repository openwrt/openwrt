/******************************************************************************
 *
 * Copyright(c) 2007 - 2021  Realtek Corporation.
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
#ifndef __HALMAC_WPP_H__
#define __HALMAC_WPP_H__

#define HALMAC_WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID( \
		halMacGuid, (9f9d9e5b, 3854, 4b87, 8bc6, 4ce1f284d34b), \
		WPP_DEFINE_BIT(COMP_HALMAC_MSG_INIT) \
		WPP_DEFINE_BIT(COMP_HALMAC_MSG_EFUSE) \
		WPP_DEFINE_BIT(COMP_HALMAC_MSG_FW) \
		WPP_DEFINE_BIT(COMP_HALMAC_MSG_H2C) \
		WPP_DEFINE_BIT(COMP_HALMAC_MSG_PWR) \
		)
#endif /* __HALBB_TYPES_H__ */
