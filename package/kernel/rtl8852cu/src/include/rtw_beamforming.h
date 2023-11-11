/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
#ifndef __RTW_BEAMFORMING_H_
#define __RTW_BEAMFORMING_H_

#ifdef CONFIG_BEAMFORMING

#define	BEAMFORMING_HT_BEAMFORMER_ENABLE	BIT(0)	/*Declare sta support beamformer*/
#define	BEAMFORMING_HT_BEAMFORMEE_ENABLE	BIT(1)	/*Declare sta support beamformee*/
#define	BEAMFORMING_HT_BEAMFORMER_TEST		BIT(2)	/*Transmiting Beamforming no matter the target supports it or not*/
#define	BEAMFORMING_HT_BEAMFORMER_STEER_NUM		(BIT(4)|BIT(5))		/*Sta Bfer's capability*/
#define	BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP	(BIT(6)|BIT(7))		/*Sta BFee's capability*/

#define	BEAMFORMING_VHT_BEAMFORMER_ENABLE	BIT(0)	/*Declare sta support beamformer*/
#define	BEAMFORMING_VHT_BEAMFORMEE_ENABLE	BIT(1)	/*Declare sta support beamformee*/
#define	BEAMFORMING_VHT_MU_MIMO_AP_ENABLE	BIT(2)	/*Declare sta support MU beamformer*/
#define	BEAMFORMING_VHT_MU_MIMO_STA_ENABLE	BIT(3)	/*Declare sta support MU beamformer*/
#define	BEAMFORMING_VHT_BEAMFORMER_TEST		BIT(4)	/*Transmiting Beamforming no matter the target supports it or not*/
#define	BEAMFORMING_VHT_BEAMFORMER_STS_CAP		(BIT(8)|BIT(9)|BIT(10))		/*Sta BFee's capability*/
#define	BEAMFORMING_VHT_BEAMFORMEE_SOUND_DIM	(BIT(12)|BIT(13)|BIT(14))	/*Sta Bfer's capability*/

#define BEAMFORMING_HE_BEAMFORMER_ENABLE        BIT(0)  /*Declare sta support beamformer*/
#define BEAMFORMING_HE_BEAMFORMEE_ENABLE        BIT(1)  /*Declare sta support beamformee*/
#define BEAMFORMING_HE_MU_MIMO_AP_ENABLE        BIT(2)  /*Declare sta support MU beamformer*/
#define BEAMFORMING_HE_MU_MIMO_STA_ENABLE       BIT(3)  /*Declare sta support MU beamformer*/
#define BEAMFORMING_HE_BEAMFORMER_TEST          BIT(4)  /*Transmiting Beamforming no matter the target supports it or not*/
#define BEAMFORMING_HE_BEAMFORMER_STS_CAP               (BIT(8)|BIT(9)|BIT(10))         /*Sta BFee's capability*/
#define BEAMFORMING_HE_BEAMFORMEE_SOUND_DIM     (BIT(12)|BIT(13)|BIT(14))       /*Sta Bfer's capability*/

void rtw_core_bf_watchdog(_adapter *padapter);

int rtw_bf_get_vht_gid_mgnt_packet(struct _ADAPTER *a, union recv_frame *rframe);
#define rtw_beamforming_get_vht_gid_mgnt_frame		rtw_bf_get_vht_gid_mgnt_packet

#endif
#endif /*__RTW_BEAMFORMING_H_*/
