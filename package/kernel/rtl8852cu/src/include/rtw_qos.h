/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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


#ifndef _RTW_QOS_H_
#define _RTW_QOS_H_

#define DRV_CFG_UAPSD_VO 	BIT0
#define DRV_CFG_UAPSD_VI 	BIT1
#define DRV_CFG_UAPSD_BK 	BIT2
#define DRV_CFG_UAPSD_BE 	BIT3

#define WMM_IE_UAPSD_VO 	BIT0
#define WMM_IE_UAPSD_VI 	BIT1
#define WMM_IE_UAPSD_BK 	BIT2
#define WMM_IE_UAPSD_BE 	BIT3

#define WMM_TID0 	BIT0
#define WMM_TID1 	BIT1
#define WMM_TID2 	BIT2
#define WMM_TID3 	BIT3
#define WMM_TID4 	BIT4
#define WMM_TID5 	BIT5
#define WMM_TID6 	BIT6
#define WMM_TID7 	BIT7

#define AP_SUPPORTED_UAPSD BIT7
/* TC = Traffic Category,  TID0~7 represents TC */
#define BIT_MASK_TID_TC 0xff
/* TS = Traffic Stream,  TID8~15 represents TS */
#define BIT_MASK_TID_TS 0xff00
#define ALL_TID_TC_SUPPORTED_UAPSD 0xff

struct	qos_priv	{

	unsigned int	  qos_option;	/* bit mask option: u-apsd, s-apsd, ts, block ack...		 */
};


#endif /* _RTL871X_QOS_H_ */
