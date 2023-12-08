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
#ifndef __HALBB_WPP_H__
#define __HALBB_WPP_H__

#define HALBB_WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID( \
		HalbbGuid, (111AA0F8,4BB9,4C05,BE3E,03383B8CD916), \
		WPP_DEFINE_BIT(DBG_RA) \
		WPP_DEFINE_BIT(DBG_FA_CNT) \
		WPP_DEFINE_BIT(HALBB_FUN_RSVD_2) \
		WPP_DEFINE_BIT(DBG_DFS) \
		WPP_DEFINE_BIT(DBG_EDCCA)\
		WPP_DEFINE_BIT(DBG_ENV_MNTR)\
		WPP_DEFINE_BIT(DBG_CFO_TRK)\
		WPP_DEFINE_BIT(DBG_PWR_CTRL)\
		WPP_DEFINE_BIT(DBG_RUA_TBL)\
		WPP_DEFINE_BIT(DBG_AUTO_DBG)\
		WPP_DEFINE_BIT(DBG_ANT_DIV)\
		WPP_DEFINE_BIT(DBG_DIG)\
		WPP_DEFINE_BIT(DBG_PATH_DIV)\
		WPP_DEFINE_BIT(DBG_UL_TB_CTRL)\
		WPP_DEFINE_BIT(DBG_BIT14)\
		WPP_DEFINE_BIT(DBG_BIT15)\
		WPP_DEFINE_BIT(DBG_BIT16)\
		WPP_DEFINE_BIT(DBG_BIT17)\
		WPP_DEFINE_BIT(DBG_SNIFFER)\
		WPP_DEFINE_BIT(DBG_CH_INFO)\
		WPP_DEFINE_BIT(DBG_PHY_STS)\
		WPP_DEFINE_BIT(DBG_CONNECT)\
		WPP_DEFINE_BIT(DBG_FW_INFO)\
		WPP_DEFINE_BIT(DBG_COMMON_FLOW)\
		WPP_DEFINE_BIT(DBG_IC_API)\
		WPP_DEFINE_BIT(DBG_DBG_API)\
		WPP_DEFINE_BIT(DBG_DBCC)\
		WPP_DEFINE_BIT(DBG_DM_SUMMARY)\
		WPP_DEFINE_BIT(DBG_PHY_CONFIG)\
		WPP_DEFINE_BIT(DBG_INIT)\
		WPP_DEFINE_BIT(DBG_CMN)\
		WPP_DEFINE_BIT(DBG_DCR)\
		)
#endif /* __HALBB_TYPES_H__ */