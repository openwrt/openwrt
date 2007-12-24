/******************************************************************************
**
** FILE NAME    : ifxmips_mei_app_ioctl.h
** PROJECT      : Danube
** MODULES      : MEI
**
** DATE         : 1 Jan 2006
** AUTHOR       : TC Chen
** DESCRIPTION  : MEI Driver
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Version $Date      $Author     $Comment
*******************************************************************************/
#ifndef __IFXMIPS_MEI_APP_IOCTL_H
#define __IFXMIPS_MEI_APP_IOCTL_H

#ifdef __KERNEL__
#include "ifxmips_mei_ioctl.h"
#endif

/* Interface Name */
//#define INTERFACE_NAME <define the interface>

/* adslLineTable constants */
#define GET_ADSL_LINE_CODE		1

/* adslAtucPhysTable constants */
#define GET_ADSL_ATUC_PHY		4

/* adslAturPhysTable constants */
#define GET_ADSL_ATUR_PHY		10

/* adslAtucChanTable constants */
#define GET_ADSL_ATUC_CHAN_INFO 	15

/* adslAturChanTable constants */
#define GET_ADSL_ATUR_CHAN_INFO		18

/* adslAtucPerfDataTable constants */
#define GET_ADSL_ATUC_PERF_DATA		21

/* adslAturPerfDataTable constants */
#define GET_ADSL_ATUR_PERF_DATA		40

/* adslAtucIntervalTable constants */
#define GET_ADSL_ATUC_INTVL_INFO	60

/* adslAturIntervalTable constants */
#define GET_ADSL_ATUR_INTVL_INFO	65

/* adslAtucChanPerfDataTable constants */
#define GET_ADSL_ATUC_CHAN_PERF_DATA	70

/* adslAturChanPerfDataTable constants */
#define GET_ADSL_ATUR_CHAN_PERF_DATA	90

/* adslAtucChanIntervalTable constants */
#define GET_ADSL_ATUC_CHAN_INTVL_INFO	110

/* adslAturChanIntervalTable constants */
#define GET_ADSL_ATUR_CHAN_INTVL_INFO	115

/* adslLineAlarmConfProfileTable constants */
#define GET_ADSL_ALRM_CONF_PROF		120
#define SET_ADSL_ALRM_CONF_PROF		121

/* adslAturTrap constants */
#define ADSL_ATUR_TRAPS			135

//////////////////  RFC-3440 //////////////

#ifdef IFXMIPS_MEI_MIB_RFC3440
/* adslLineExtTable */
#define GET_ADSL_ATUC_LINE_EXT		201
#define SET_ADSL_ATUC_LINE_EXT		203

/* adslAtucPerfDateExtTable */
#define GET_ADSL_ATUC_PERF_DATA_EXT	205

/* adslAtucIntervalExtTable */
#define GET_ADSL_ATUC_INTVL_EXT_INFO	221

/* adslAturPerfDataExtTable */
#define GET_ADSL_ATUR_PERF_DATA_EXT	225

/* adslAturIntervalExtTable */
#define GET_ADSL_ATUR_INTVL_EXT_INFO	233

/* adslAlarmConfProfileExtTable */
#define GET_ADSL_ALRM_CONF_PROF_EXT	235
#define SET_ADSL_ALRM_CONF_PROF_EXT	236

/* adslAturExtTrap */
#define ADSL_ATUR_EXT_TRAPS		240

#endif

/* The following constants are added to support the WEB related ADSL Statistics */

/* adslLineStatus constants */
#define GET_ADSL_LINE_STATUS		245

/* adslLineRate constants */
#define GET_ADSL_LINE_RATE		250

/* adslLineInformation constants */
#define GET_ADSL_LINE_INFO		255

/* adslNearEndPerformanceStats constants */
#define GET_ADSL_NEAREND_STATS	270

/* adslFarEndPerformanceStats constants */
#define GET_ADSL_FAREND_STATS	290

/* Sub-carrier related parameters */
#define GET_ADSL_LINE_INIT_STATS	150
#define GET_ADSL_POWER_SPECTRAL_DENSITY	151

#define IFXMIPS_MIB_LO_ATUC		295
#define IFXMIPS_MIB_LO_ATUR		296

#define GET_ADSL_ATUC_SUBCARRIER_STATS	297
#define GET_ADSL_ATUR_SUBCARRIER_STATS	298



///////////////////////////////////////////////////////////
// makeCMV(Opcode, Group, Address, Index, Size, Data)

/* adslLineCode Flags */
#define LINE_CODE_FLAG			0x1	/* BIT 0th position */

/* adslAtucPhysTable Flags */
#define ATUC_PHY_SER_NUM_FLAG		0x1	/* BIT 0th position */
#define ATUC_PHY_SER_NUM_FLAG_MAKECMV1	makeCMV(H2D_CMV_READ, INFO, 57, 0, 12, data,TxMessage) 
#define ATUC_PHY_SER_NUM_FLAG_MAKECMV2	makeCMV(H2D_CMV_READ, INFO, 57, 12, 4, data,TxMessage) 

#define ATUC_PHY_VENDOR_ID_FLAG		0x2	/* BIT 1 */
#define ATUC_PHY_VENDOR_ID_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 64, 0, 4, data,TxMessage)

#define ATUC_PHY_VER_NUM_FLAG		0x4	/* BIT 2 */
#define ATUC_PHY_VER_NUM_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 58, 0, 8, data,TxMessage)

#define ATUC_CURR_STAT_FLAG		0x8	/* BIT 3 */

#define ATUC_CURR_OUT_PWR_FLAG		0x10	/* BIT 4 */
#define ATUC_CURR_OUT_PWR_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 5, 1, data,TxMessage)

#define ATUC_CURR_ATTR_FLAG		0x20	/* BIT 5 */
#define ATUC_CURR_ATTR_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 69, 0, 2, data,TxMessage)


/* adslAturPhysTable	Flags */
#define ATUR_PHY_SER_NUM_FLAG		0x1	/* BIT 0th position */
#define ATUR_PHY_SER_NUM_FLAG_MAKECMV1	makeCMV(H2D_CMV_READ, INFO, 62, 0, 12, data,TxMessage)
#define ATUR_PHY_SER_NUM_FLAG_MAKECMV2	makeCMV(H2D_CMV_READ, INFO, 62, 12, 4, data,TxMessage)

#define ATUR_PHY_VENDOR_ID_FLAG		0x2	/* BIT 1 */
#define ATUR_PHY_VENDOR_ID_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 65, 0, 4, data,TxMessage)

#define ATUR_PHY_VER_NUM_FLAG		0x4	/* BIT 2 */
#define ATUR_PHY_VER_NUM_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 61, 0, 8, data,TxMessage)

#define ATUR_SNRMGN_FLAG		0x8
#if 0 /* [ Ritesh. Use PLAM 45 0 for 0.1dB resolution rather than INFO 68 3 */
#define ATUR_SNRMGN_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 4, 1, data,TxMessage)
#else
#define ATUR_SNRMGN_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, PLAM_SNRMargin_0_1db, 0, 1, data, TxMessage)
#endif

#define ATUR_ATTN_FLAG			0x10
#define ATUR_ATTN_FLAG_MAKECMV		makeCMV(H2D_CMV_READ, INFO, 68, 2, 1, data,TxMessage)

#define ATUR_CURR_STAT_FLAG		0x20	/* BIT 3 */

#define ATUR_CURR_OUT_PWR_FLAG		0x40	/* BIT 4 */
#define ATUR_CURR_OUT_PWR_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 69, 5, 1, data,TxMessage)

#define ATUR_CURR_ATTR_FLAG		0x80	/* BIT 5 */
#define ATUR_CURR_ATTR_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 0, 2, data,TxMessage)

/* adslAtucChanTable Flags */
#define ATUC_CHAN_INTLV_DELAY_FLAG	0x1	/* BIT 0th position */
//KD #define ATUC_CHAN_INTLV_DELAY_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, RATE, 3, 1, 1, data,TxMessage)
#define ATUC_CHAN_INTLV_DELAY_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 92, 1, 1, data,TxMessage)

#define ATUC_CHAN_CURR_TX_RATE_FLAG	0x2	/* BIT 1 */
#define ATUC_CHAN_CURR_TX_RATE_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, RATE, 1, 0, 2, data,TxMessage)

#define ATUC_CHAN_PREV_TX_RATE_FLAG	0x4	/* BIT 2 */

/* adslAturChanTable Flags */
#define ATUR_CHAN_INTLV_DELAY_FLAG	0x1	/* BIT 0th position */
//KD #define ATUR_CHAN_INTLV_DELAY_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, RATE, 2, 1, 1, data,TxMessage)
#define ATUR_CHAN_INTLV_DELAY_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 93, 1, 1, data,TxMessage)

#define ATUR_CHAN_CURR_TX_RATE_FLAG	0x2	/* BIT 1 */
#define ATUR_CHAN_CURR_TX_RATE_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, RATE, 0, 0, 2, data,TxMessage)

#define ATUR_CHAN_PREV_TX_RATE_FLAG	0x4	/* BIT 2 */

#define ATUR_CHAN_CRC_BLK_LEN_FLAG	0x8	/* BIT 3 */

/* adslAtucPerfDataTable Flags */
#define ATUC_PERF_LOFS_FLAG		0x1	/* BIT 0th position */
#define ATUC_PERF_LOSS_FLAG		0x2	/* BIT 1 */
#define ATUC_PERF_LO_FLAG_MAKECMV		makeCMV(H2D_CMV_READ, PLAM, 0, 0, 1, data,TxMessage)
#define ATUC_PERF_ESS_FLAG		0x4	/* BIT 2 */
#define ATUC_PERF_ESS_FLAG_MAKECMV		makeCMV(H2D_CMV_READ, PLAM, 7, 0, 1, data,TxMessage) 
#define ATUC_PERF_INITS_FLAG	0x8	/* BIT 3 */
#define ATUC_PERF_VALID_INTVLS_FLAG	0x10 /* BIT 4 */
#define ATUC_PERF_INVALID_INTVLS_FLAG	0x20 /* BIT 5 */
#define ATUC_PERF_CURR_15MIN_TIME_ELAPSED_FLAG	0x40 /* BIT 6 */
#define ATUC_PERF_CURR_15MIN_LOFS_FLAG	 	0x80 	 /* BIT 7 */
#define ATUC_PERF_CURR_15MIN_LOSS_FLAG		0x100 /* BIT 8 */
#define ATUC_PERF_CURR_15MIN_ESS_FLAG		0x200	/* BIT 9 */
#define ATUC_PERF_CURR_15MIN_INIT_FLAG		0x400 /* BIT 10 */
#define ATUC_PERF_CURR_1DAY_TIME_ELAPSED_FLAG 0x800 /* BIT 11 */
#define ATUC_PERF_CURR_1DAY_LOFS_FLAG		0x1000 /* BIT 12 */
#define ATUC_PERF_CURR_1DAY_LOSS_FLAG		0x2000 /* BIT 13 */
#define ATUC_PERF_CURR_1DAY_ESS_FLAG		0x4000 /* BIT 14 */
#define ATUC_PERF_CURR_1DAY_INIT_FLAG		0x8000 /* BIT 15 */
#define ATUC_PERF_PREV_1DAY_MON_SEC_FLAG	0x10000 /* BIT 16 */
#define ATUC_PERF_PREV_1DAY_LOFS_FLAG		0x20000 /* BIT 17 */
#define ATUC_PERF_PREV_1DAY_LOSS_FLAG		0x40000 /* BIT 18 */
#define ATUC_PERF_PREV_1DAY_ESS_FLAG		0x80000 /* BIT 19 */
#define ATUC_PERF_PREV_1DAY_INITS_FLAG		0x100000 /* BIT 20 */

/* adslAturPerfDataTable Flags */
#define ATUR_PERF_LOFS_FLAG		0x1	/* BIT 0th position */
#define ATUR_PERF_LOSS_FLAG		0x2	/* BIT 1 */
#define ATUR_PERF_LPR_FLAG		0x4	/* BIT 2 */
#define ATUR_PERF_LO_FLAG_MAKECMV		makeCMV(H2D_CMV_READ, PLAM, 1, 0, 1, data,TxMessage)
#define ATUR_PERF_ESS_FLAG		0x8	/* BIT 3 */
#define ATUR_PERF_ESS_FLAG_MAKECMV		makeCMV(H2D_CMV_READ, PLAM, 33, 0, 1, data,TxMessage)
#define ATUR_PERF_VALID_INTVLS_FLAG	0x10 /* BIT 4 */
#define ATUR_PERF_INVALID_INTVLS_FLAG	0x20 /* BIT 5 */
#define ATUR_PERF_CURR_15MIN_TIME_ELAPSED_FLAG	0x40 /* BIT 6 */
#define ATUR_PERF_CURR_15MIN_LOFS_FLAG	 	0x80 	 /* BIT 7 */
#define ATUR_PERF_CURR_15MIN_LOSS_FLAG		0x100 /* BIT 8 */
#define ATUR_PERF_CURR_15MIN_LPR_FLAG		0x200 /* BIT 9 */
#define ATUR_PERF_CURR_15MIN_ESS_FLAG		0x400	/* BIT 10 */
#define ATUR_PERF_CURR_1DAY_TIME_ELAPSED_FLAG 	0x800 /* BIT 11 */
#define ATUR_PERF_CURR_1DAY_LOFS_FLAG		0x1000 /* BIT 12 */
#define ATUR_PERF_CURR_1DAY_LOSS_FLAG		0x2000 /* BIT 13 */
#define ATUR_PERF_CURR_1DAY_LPR_FLAG		0x4000 /* BIT 14 */
#define ATUR_PERF_CURR_1DAY_ESS_FLAG		0x8000 /* BIT 15 */
#define ATUR_PERF_PREV_1DAY_MON_SEC_FLAG	0x10000 /* BIT 16 */
#define ATUR_PERF_PREV_1DAY_LOFS_FLAG		0x20000 /* BIT 17 */
#define ATUR_PERF_PREV_1DAY_LOSS_FLAG		0x40000 /* BIT 18 */
#define ATUR_PERF_PREV_1DAY_LPR_FLAG		0x80000 /* BIT 19 */
#define ATUR_PERF_PREV_1DAY_ESS_FLAG		0x100000 /* BIT 20 */

/* adslAtucIntervalTable Flags */
#define ATUC_INTVL_LOF_FLAG		0x1	/* BIT 0th position */
#define ATUC_INTVL_LOS_FLAG		0x2 	/* BIT 1 */
#define ATUC_INTVL_ESS_FLAG		0x4	/* BIT 2 */
#define ATUC_INTVL_INIT_FLAG		0x8   /* BIT 3 */
#define ATUC_INTVL_VALID_DATA_FLAG 	0x10 /* BIT 4 */

/* adslAturIntervalTable Flags */
#define ATUR_INTVL_LOF_FLAG		0x1	/* BIT 0th position */
#define ATUR_INTVL_LOS_FLAG		0x2 	/* BIT 1 */
#define ATUR_INTVL_LPR_FLAG		0x4 	/* BIT 2 */
#define ATUR_INTVL_ESS_FLAG		0x8	/* BIT 3 */
#define ATUR_INTVL_VALID_DATA_FLAG 	0x10 /* BIT 4 */

/* adslAtucChanPerfDataTable Flags */
#define ATUC_CHAN_RECV_BLK_FLAG	0x01	/* BIT 0th position */
#define ATUC_CHAN_TX_BLK_FLAG	0x02	/* BIT 1 */
#define ATUC_CHAN_CORR_BLK_FLAG	0x04	/* BIT 2 */
#define ATUC_CHAN_UNCORR_BLK_FLAG 0x08	/* BIT 3 */
#define ATUC_CHAN_PERF_VALID_INTVL_FLAG 0x10 /* BIT 4 */
#define ATUC_CHAN_PERF_INVALID_INTVL_FLAG 0x20 /* BIT 5 */
#define ATUC_CHAN_PERF_CURR_15MIN_TIME_ELAPSED_FLAG 0x40 /* BIT 6 */
#define ATUC_CHAN_PERF_CURR_15MIN_RECV_BLK_FLAG	0x80 /* BIT 7 */
#define ATUC_CHAN_PERF_CURR_15MIN_TX_BLK_FLAG 0x100 /* BIT 8 */
#define ATUC_CHAN_PERF_CURR_15MIN_CORR_BLK_FLAG 0x200 /* BIT 9 */
#define ATUC_CHAN_PERF_CURR_15MIN_UNCORR_BLK_FLAG 0x400 /* BIT 10 */
#define ATUC_CHAN_PERF_CURR_1DAY_TIME_ELAPSED_FLAG 0x800 /* BIT 11*/
#define ATUC_CHAN_PERF_CURR_1DAY_RECV_BLK_FLAG 0x1000 /* BIT 12 */
#define ATUC_CHAN_PERF_CURR_1DAY_TX_BLK_FLAG 0x2000 /* BIT 13 */
#define ATUC_CHAN_PERF_CURR_1DAY_CORR_BLK_FLAG 0x4000 /* BIT 14 */
#define ATUC_CHAN_PERF_CURR_1DAY_UNCORR_BLK_FLAG 0x8000 /* BIT 15 */
#define ATUC_CHAN_PERF_PREV_1DAY_MONI_SEC_FLAG 0x10000 /* BIT 16 */
#define ATUC_CHAN_PERF_PREV_1DAY_RECV_BLK_FLAG 0x20000 /* BIT 17 */
#define ATUC_CHAN_PERF_PREV_1DAY_TX_BLK_FLAG 0x40000 /* BIT 18 */
#define ATUC_CHAN_PERF_PREV_1DAY_CORR_BLK_FLAG 0x80000 /* BIT 19 */
#define ATUC_CHAN_PERF_PREV_1DAY_UNCORR_BLK_FLAG 0x100000 /* BIT 20 */


/* adslAturChanPerfDataTable Flags */
#define ATUR_CHAN_RECV_BLK_FLAG   0x01 	/* BIT 0th position */ 
#define ATUR_CHAN_RECV_BLK_FLAG_MAKECMV_LSW		makeCMV(H2D_CMV_READ, PLAM, 20, 0, 1, data,TxMessage)
#define ATUR_CHAN_RECV_BLK_FLAG_MAKECMV_MSW		makeCMV(H2D_CMV_READ, PLAM, 21, 0, 1, data,TxMessage)
#define ATUR_CHAN_TX_BLK_FLAG     0x02 	/* BIT 1 */
#define ATUR_CHAN_TX_BLK_FLAG_MAKECMV_LSW		makeCMV(H2D_CMV_READ, PLAM, 20, 0, 1, data,TxMessage)
#define ATUR_CHAN_TX_BLK_FLAG_MAKECMV_MSW		makeCMV(H2D_CMV_READ, PLAM, 21, 0, 1, data,TxMessage)
#define ATUR_CHAN_CORR_BLK_FLAG   0x04 	/* BIT 2 */
#define ATUR_CHAN_CORR_BLK_FLAG_MAKECMV_INTL		makeCMV(H2D_CMV_READ, PLAM, 3, 0, 1, data,TxMessage)
#define ATUR_CHAN_CORR_BLK_FLAG_MAKECMV_FAST		makeCMV(H2D_CMV_READ, PLAM, 3, 1, 1, data,TxMessage)
#define ATUR_CHAN_UNCORR_BLK_FLAG 0x08		/* BIT 3 */
#define ATUR_CHAN_UNCORR_BLK_FLAG_MAKECMV_INTL		makeCMV(H2D_CMV_READ, PLAM, 2, 0, 1, data,TxMessage)
#define ATUR_CHAN_UNCORR_BLK_FLAG_MAKECMV_FAST		makeCMV(H2D_CMV_READ, PLAM, 2, 1, 1, data,TxMessage)
#define ATUR_CHAN_PERF_VALID_INTVL_FLAG   0x10 	/* BIT 4 */
#define ATUR_CHAN_PERF_INVALID_INTVL_FLAG 0x20 	/* BIT 5 */
#define ATUR_CHAN_PERF_CURR_15MIN_TIME_ELAPSED_FLAG 0x40 /* BIT 6 */
#define ATUR_CHAN_PERF_CURR_15MIN_RECV_BLK_FLAG    0x80   /* BIT 7 */
#define ATUR_CHAN_PERF_CURR_15MIN_TX_BLK_FLAG      0x100 /* BIT 8 */
#define ATUR_CHAN_PERF_CURR_15MIN_CORR_BLK_FLAG    0x200 /* BIT 9 */
#define ATUR_CHAN_PERF_CURR_15MIN_UNCORR_BLK_FLAG  0x400 /* BIT 10 */
#define ATUR_CHAN_PERF_CURR_1DAY_TIME_ELAPSED_FLAG 0x800 /* BIT 11 */
#define ATUR_CHAN_PERF_CURR_1DAY_RECV_BLK_FLAG     0x1000 /* BIT 12 */
#define ATUR_CHAN_PERF_CURR_1DAY_TX_BLK_FLAG       0x2000 /* BIT 13 */
#define ATUR_CHAN_PERF_CURR_1DAY_CORR_BLK_FLAG     0x4000 /* BIT 14 */
#define ATUR_CHAN_PERF_CURR_1DAY_UNCORR_BLK_FLAG   0x8000 /* BIT 15 */
#define ATUR_CHAN_PERF_PREV_1DAY_MONI_SEC_FLAG     0x10000 /* BIT 16 */
#define ATUR_CHAN_PERF_PREV_1DAY_RECV_BLK_FLAG     0x20000 /* BIT 17 */
#define ATUR_CHAN_PERF_PREV_1DAY_TRANS_BLK_FLAG    0x40000 /* BIT 18 */
#define ATUR_CHAN_PERF_PREV_1DAY_CORR_BLK_FLAG     0x80000 /* BIT 19 */
#define ATUR_CHAN_PERF_PREV_1DAY_UNCORR_BLK_FLAG   0x100000 /* BIT 20 */

/* adslAtucChanIntervalTable Flags */
#define ATUC_CHAN_INTVL_NUM_FLAG   	     	0x1 	/* BIT 0th position */
#define ATUC_CHAN_INTVL_RECV_BLK_FLAG  		0x2 	/* BIT 1 */
#define ATUC_CHAN_INTVL_TX_BLK_FLAG  		0x4	/* BIT 2 */
#define ATUC_CHAN_INTVL_CORR_BLK_FLAG   	0x8 	/* BIT 3 */
#define ATUC_CHAN_INTVL_UNCORR_BLK_FLAG   	0x10 	/* BIT 4 */
#define ATUC_CHAN_INTVL_VALID_DATA_FLAG 	0x20 	/* BIT 5 */

/* adslAturChanIntervalTable Flags */
#define ATUR_CHAN_INTVL_NUM_FLAG   	     	0x1 	/* BIT 0th Position */
#define ATUR_CHAN_INTVL_RECV_BLK_FLAG  		0x2 	/* BIT 1 */
#define ATUR_CHAN_INTVL_TX_BLK_FLAG  		0x4	/* BIT 2 */
#define ATUR_CHAN_INTVL_CORR_BLK_FLAG   	0x8 	/* BIT 3 */
#define ATUR_CHAN_INTVL_UNCORR_BLK_FLAG   	0x10 	/* BIT 4 */
#define ATUR_CHAN_INTVL_VALID_DATA_FLAG 	0x20 	/* BIT 5 */

/* adslLineAlarmConfProfileTable Flags */
#define ATUC_THRESH_15MIN_LOFS_FLAG   		0x01   /* BIT 0th position */
#define ATUC_THRESH_15MIN_LOSS_FLAG   		0x02   /* BIT 1 */
#define ATUC_THRESH_15MIN_ESS_FLAG      	0x04   /* BIT 2 */
#define ATUC_THRESH_FAST_RATEUP_FLAG     	0x08   /* BIT 3 */
#define ATUC_THRESH_INTERLEAVE_RATEUP_FLAG	0x10   /* BIT 4 */
#define ATUC_THRESH_FAST_RATEDOWN_FLAG		0x20	 /* BIT 5 */
#define ATUC_THRESH_INTERLEAVE_RATEDOWN_FLAG   	0x40	/* BIT 6 */
#define ATUC_INIT_FAILURE_TRAP_ENABLE_FLAG	0x80 	/* BIT 7 */
#define ATUR_THRESH_15MIN_LOFS_FLAG   		0x100  	/* BIT 8 */
#define ATUR_THRESH_15MIN_LOSS_FLAG   		0x200  	/* BIT 9 */
#define ATUR_THRESH_15MIN_LPRS_FLAG    		0x400  	/* BIT 10 */
#define ATUR_THRESH_15MIN_ESS_FLAG      	0x800   	/* BIT 11 */
#define ATUR_THRESH_FAST_RATEUP_FLAG     	0x1000  	/* BIT 12 */
#define ATUR_THRESH_INTERLEAVE_RATEUP_FLAG	0x2000  	/* BIT 13 */
#define ATUR_THRESH_FAST_RATEDOWN_FLAG     	0x4000 	/* BIT 14 */
#define ATUR_THRESH_INTERLEAVE_RATEDOWN_FLAG	0x8000   	/* BIT 15 */
#define LINE_ALARM_CONF_PROFILE_ROWSTATUS_FLAG  0x10000   	/* BIT 16 */


/* adslAturTraps Flags */
#define ATUC_PERF_LOFS_THRESH_FLAG   	     	0x1 	/* BIT 0th position */
#define ATUC_PERF_LOSS_THRESH_FLAG  	 	0x2 	/* BIT 1 */
#define ATUC_PERF_ESS_THRESH_FLAG  	 	0x4 	/* BIT 2 */
#define ATUC_RATE_CHANGE_FLAG	  	 	0x8 	/* BIT 3 */
#define ATUR_PERF_LOFS_THRESH_FLAG   	     	0x10 	/* BIT 4 */
#define ATUR_PERF_LOSS_THRESH_FLAG  	 	0x20 	/* BIT 5 */
#define ATUR_PERF_LPRS_THRESH_FLAG  	 	0x40 	/* BIT 6 */
#define ATUR_PERF_ESS_THRESH_FLAG  	 	0x80 	/* BIT 7 */
#define ATUR_RATE_CHANGE_FLAG	  	 	0x100	/* BIT 8 */

//RFC- 3440 FLAG DEFINITIONS

#ifdef IFXMIPS_MEI_MIB_RFC3440
/* adslLineExtTable flags */
#define ATUC_LINE_TRANS_CAP_FLAG	 	0x1		/* BIT 0th position */
#define ATUC_LINE_TRANS_CAP_FLAG_MAKECMV	makeCMV(H2D_CMV_READ,INFO, 67, 0, 1, data,TxMessage)
#define ATUC_LINE_TRANS_CONFIG_FLAG	 	0x2		/* BIT 1 */
#define ATUC_LINE_TRANS_CONFIG_FLAG_MAKECMV	makeCMV(H2D_CMV_READ,INFO, 67, 0, 1, data,TxMessage)
#define ATUC_LINE_TRANS_CONFIG_FLAG_MAKECMV_WR	makeCMV(H2D_CMV_WRITE,INFO, 67, 0, 1, data,TxMessage)
#define ATUC_LINE_TRANS_ACTUAL_FLAG	 	0x4		/* BIT 2 */
#define ATUC_LINE_TRANS_ACTUAL_FLAG_MAKECMV	makeCMV(H2D_CMV_READ,STAT, 1, 0, 1, data,TxMessage)
#define LINE_GLITE_POWER_STATE_FLAG 	 	0x8		/* BIT 3 */
#define LINE_GLITE_POWER_STATE_FLAG_MAKECMV	makeCMV(H2D_CMV_READ,STAT, 0, 0, 1, data,TxMessage) 

/* adslAtucPerfDataExtTable flags */
#define ATUC_PERF_STAT_FASTR_FLAG	   0x1 /* BIT 0th position */
#define ATUC_PERF_STAT_FASTR_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, STAT, 0, 0, 1, data, TxMessage)
#define ATUC_PERF_STAT_FAILED_FASTR_FLAG 0x2 /* BIT 1 */
#define ATUC_PERF_STAT_FAILED_FASTR_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, STAT, 0, 0, 1, data, TxMessage)
#define ATUC_PERF_STAT_SESL_FLAG 	   0X4	/* BIT 2 */
#define ATUC_PERF_STAT_SESL_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 8, 0, 1, data, TxMessage)
#define ATUC_PERF_STAT_UASL_FLAG		   0X8	/* BIT 3 */
#define ATUC_PERF_STAT_UASL_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 10, 0, 1, data, TxMessage)
#define ATUC_PERF_CURR_15MIN_FASTR_FLAG	   0X10	/* BIT 4 */
#define ATUC_PERF_CURR_15MIN_FAILED_FASTR_FLAG 0X20	/* BIT 5 */
#define ATUC_PERF_CURR_15MIN_SESL_FLAG	         0X40	/* BIT 6 */
#define ATUC_PERF_CURR_15MIN_UASL_FLAG		    0X80	/* BIT 7 */
#define ATUC_PERF_CURR_1DAY_FASTR_FLAG		    0X100	/* BIT 8 */
#define ATUC_PERF_CURR_1DAY_FAILED_FASTR_FLAG	0X200	/* BIT 9 */
#define ATUC_PERF_CURR_1DAY_SESL_FLAG			0X400	/* BIT 10 */
#define ATUC_PERF_CURR_1DAY_UASL_FLAG			0X800	/* BIT 11 */
#define ATUC_PERF_PREV_1DAY_FASTR_FLAG		     0X1000 /* BIT 12 */
#define ATUC_PERF_PREV_1DAY_FAILED_FASTR_FLAG	0X2000 /* BIT 13 */
#define ATUC_PERF_PREV_1DAY_SESL_FLAG			0X4000 /* BIT 14 */
#define ATUC_PERF_PREV_1DAY_UASL_FLAG			0X8000 /* BIT 15 */

/* adslAturPerfDataExtTable */
#define ATUR_PERF_STAT_SESL_FLAG		0X1 /* BIT 0th position */
#define ATUR_PERF_STAT_SESL_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 34, 0, 1, data, TxMessage)	
#define ATUR_PERF_STAT_UASL_FLAG		0X2 /* BIT 1 */
#define ATUR_PERF_STAT_UASL_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 36, 0, 1, data, TxMessage)
#define ATUR_PERF_CURR_15MIN_SESL_FLAG		0X4 /* BIT 2 */
#define ATUR_PERF_CURR_15MIN_UASL_FLAG		0X8 /* BIT 3 */
#define ATUR_PERF_CURR_1DAY_SESL_FLAG		0X10 /* BIT 4 */
#define ATUR_PERF_CURR_1DAY_UASL_FLAG		0X20 /* BIT 5 */
#define ATUR_PERF_PREV_1DAY_SESL_FLAG		0X40 /* BIT 6 */
#define ATUR_PERF_PREV_1DAY_UASL_FLAG		0X80 /* BIT 7 */

/* adslAutcIntervalExtTable flags */
#define ATUC_INTERVAL_FASTR_FLAG		0x1 /* Bit 0 */ 	
#define ATUC_INTERVAL_FAILED_FASTR_FLAG		0x2 /* Bit 1 */ 	
#define ATUC_INTERVAL_SESL_FLAG			0x4 /* Bit 2 */ 	
#define ATUC_INTERVAL_UASL_FLAG			0x8 /* Bit 3 */ 	

/* adslAturIntervalExtTable */
#define ATUR_INTERVAL_SESL_FLAG		0X1 /* BIT 0th position */
#define ATUR_INTERVAL_UASL_FLAG		0X2 /* BIT 1 */

/* adslAlarmConfProfileExtTable */
#define ATUC_THRESH_15MIN_FAILED_FASTR_FLAG 0X1/* BIT 0th position */
#define ATUC_THRESH_15MIN_SESL_FLAG		 0X2 /* BIT 1 */
#define ATUC_THRESH_15MIN_UASL_FLAG		 0X4 /* BIT 2 */
#define ATUR_THRESH_15MIN_SESL_FLAG		 0X8 /* BIT 3 */
#define ATUR_THRESH_15MIN_UASL_FLAG		 0X10 /* BIT 4 */

/* adslAturExtTraps */
#define ATUC_15MIN_FAILED_FASTR_TRAP_FLAG 	0X1 /* BIT 0th position */
#define ATUC_15MIN_SESL_TRAP_FLAG		 0X2 /* BIT 1 */
#define ATUC_15MIN_UASL_TRAP_FLAG		 0X4 /* BIT 2 */
#define ATUR_15MIN_SESL_TRAP_FLAG		 0X8 /* BIT 3 */
#define ATUR_15MIN_UASL_TRAP_FLAG		 0X10 /* BIT 4 */

#endif

/* adslLineStatus Flags */
#define LINE_STAT_MODEM_STATUS_FLAG   	 0x1 /* BIT 0th position */
#define LINE_STAT_MODEM_STATUS_FLAG_MAKECMV makeCMV(H2D_CMV_READ, STAT, 0, 0, 1, data, TxMessage)
#define LINE_STAT_MODE_SEL_FLAG  	 0x2 /* BIT 1 */
#define LINE_STAT_MODE_SEL_FLAG_MAKECMV makeCMV(H2D_CMV_READ, STAT, 1, 0, 1, data, TxMessage)
#define LINE_STAT_TRELLCOD_ENABLE_FLAG 0x4 /* BIT 2 */
#define LINE_STAT_TRELLCOD_ENABLE_FLAG_MAKECMV makeCMV(H2D_CMV_READ, OPTN, 2, 0, 1, data, TxMessage)
#define LINE_STAT_LATENCY_FLAG	  	 0x8 /* BIT 3 */
#define LINE_STAT_LATENCY_FLAG_MAKECMV makeCMV(H2D_CMV_READ, STAT, 12, 0, 1, data, TxMessage)

/* adslLineRate Flags */
#define LINE_RATE_DATA_RATEDS_FLAG   	0x1 /* BIT 0th position */
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL1_LP0_MAKECMV makeCMV(H2D_CMV_READ, RATE, 1, 0, 2, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL1_LP1_MAKECMV makeCMV(H2D_CMV_READ, RATE, 1, 2, 2, data, TxMessage)


#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_RP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 12, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_MP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 13, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_LP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 14, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_TP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 15, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_KP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 17, 0, 2, data, TxMessage)

#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_RP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 12, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_MP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 13, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_LP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 14, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_TP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 15, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEDS_FLAG_ADSL2_KP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 17, 2, 2, data, TxMessage)

#define LINE_RATE_DATA_RATEUS_FLAG  	0x2 /* BIT 1 */
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL1_LP0_MAKECMV makeCMV(H2D_CMV_READ, RATE, 0, 0, 2, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL1_LP1_MAKECMV makeCMV(H2D_CMV_READ, RATE, 0, 2, 2, data, TxMessage)


#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_RP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 23, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_MP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 24, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_LP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 25, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_TP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 26, 0, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_KP_LP0_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 28, 0, 2, data, TxMessage)

#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_RP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 23, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_MP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 24, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_LP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 25, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_TP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 26, 1, 1, data, TxMessage)
#define LINE_RATE_DATA_RATEUS_FLAG_ADSL2_KP_LP1_MAKECMV makeCMV(H2D_CMV_READ, CNFG, 28, 2, 2, data, TxMessage)

#define LINE_RATE_ATTNDRDS_FLAG  	0x4 /* BIT 2 */
#define LINE_RATE_ATTNDRDS_FLAG_MAKECMV makeCMV(H2D_CMV_READ, INFO, 68, 4, 2, data, TxMessage)

#define LINE_RATE_ATTNDRUS_FLAG	  	0x8 /* BIT 3 */
#define LINE_RATE_ATTNDRUS_FLAG_MAKECMV makeCMV(H2D_CMV_READ, INFO, 69, 4, 2, data, TxMessage)

/* adslLineInformation Flags */
#define LINE_INFO_INTLV_DEPTHDS_FLAG	0x1 /* BIT 0th position */
#define LINE_INFO_INTLV_DEPTHDS_FLAG_LP0_MAKECMV	makeCMV(H2D_CMV_READ, CNFG, 27, 0, 1, data, TxMessage)
#define LINE_INFO_INTLV_DEPTHDS_FLAG_LP1_MAKECMV	makeCMV(H2D_CMV_READ, CNFG, 27, 1, 1, data, TxMessage)
#define LINE_INFO_INTLV_DEPTHUS_FLAG	0x2 /* BIT 1 */
#define LINE_INFO_INTLV_DEPTHUS_FLAG_LP0_MAKECMV	makeCMV(H2D_CMV_READ, CNFG, 16, 0, 1, data, TxMessage)
#define LINE_INFO_INTLV_DEPTHUS_FLAG_LP1_MAKECMV	makeCMV(H2D_CMV_READ, CNFG, 16, 1, 1, data, TxMessage)
#define LINE_INFO_LATNDS_FLAG		0x4 /* BIT 2 */
#define LINE_INFO_LATNDS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 1, 1, data, TxMessage)
#define LINE_INFO_LATNUS_FLAG	  	0x8 /* BIT 3 */
#define LINE_INFO_LATNUS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 69, 1, 1, data, TxMessage)
#define LINE_INFO_SATNDS_FLAG   	 	0x10 /* BIT 4 */
#define LINE_INFO_SATNDS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 2, 1, data, TxMessage)
#define LINE_INFO_SATNUS_FLAG  	 	0x20 /* BIT 5 */
#define LINE_INFO_SATNUS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 69, 2, 1, data, TxMessage)
#define LINE_INFO_SNRMNDS_FLAG  	 	0x40 /* BIT 6 */
#define LINE_INFO_SNRMNDS_FLAG_ADSL1_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 3, 1, data, TxMessage)
#define LINE_INFO_SNRMNDS_FLAG_ADSL2_MAKECMV	makeCMV(H2D_CMV_READ, RATE, 3, 0, 1, data, TxMessage)
#define LINE_INFO_SNRMNDS_FLAG_ADSL2PLUS_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 46, 0, 1, data, TxMessage)
#define LINE_INFO_SNRMNUS_FLAG  	 	0x80 /* BIT 7 */
#define LINE_INFO_SNRMNUS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 69, 3, 1, data, TxMessage)
#define LINE_INFO_ACATPDS_FLAG	  	0x100 /* BIT 8 */
#define LINE_INFO_ACATPDS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 68, 6, 1, data, TxMessage)
#define LINE_INFO_ACATPUS_FLAG	  	0x200 /* BIT 9 */
#define LINE_INFO_ACATPUS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 69, 6, 1, data, TxMessage)

/* adslNearEndPerformanceStats Flags */
#define NEAREND_PERF_SUPERFRAME_FLAG_LSW_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 20, 0, 1, data, TxMessage)
#define NEAREND_PERF_SUPERFRAME_FLAG_MSW_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 21, 0, 1, data, TxMessage)
#define NEAREND_PERF_SUPERFRAME_FLAG	0x1 /* BIT 0th position */
#define NEAREND_PERF_LOS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 0, 0, 1, data, TxMessage)
#define NEAREND_PERF_LOS_FLAG		0x2 /* BIT 1 */
#define NEAREND_PERF_LOF_FLAG		0x4 /* BIT 2 */
#define NEAREND_PERF_LPR_FLAG		0x8 /* BIT 3 */
#define NEAREND_PERF_NCD_FLAG		0x10 /* BIT 4 */
#define NEAREND_PERF_LCD_FLAG		0x20 /* BIT 5 */
#define NEAREND_PERF_CRC_FLAG		0x40 /* BIT 6 */
#define NEAREND_PERF_CRC_FLAG_LP0_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 2, 0, 1, data, TxMessage)
#define NEAREND_PERF_CRC_FLAG_LP1_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 2, 1, 1, data, TxMessage)
#define NEAREND_PERF_RSCORR_FLAG_LP0_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 3, 0, 1, data, TxMessage)
#define NEAREND_PERF_RSCORR_FLAG_LP1_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 3, 1, 1, data, TxMessage)
#define NEAREND_PERF_RSCORR_FLAG	0x80 /* BIT 7 */
#define NEAREND_PERF_FECS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 6, 0, 1, data, TxMessage)
#define NEAREND_PERF_FECS_FLAG		0x100 /* BIT 8 */
#define NEAREND_PERF_ES_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 7, 0, 1, data, TxMessage)
#define NEAREND_PERF_ES_FLAG		0x200 /* BIT 9 */
#define NEAREND_PERF_SES_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 8, 0, 1, data, TxMessage)
#define NEAREND_PERF_SES_FLAG		0x400 /* BIT 10 */
#define NEAREND_PERF_LOSS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 9, 0, 1, data, TxMessage)
#define NEAREND_PERF_LOSS_FLAG		0x800 /* BIT 11 */
#define NEAREND_PERF_UAS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 10, 0, 1, data, TxMessage)
#define NEAREND_PERF_UAS_FLAG		0x1000 /* BIT 12 */
#define NEAREND_PERF_HECERR_FLAG_BC0_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 11, 0, 2, data, TxMessage)
#define NEAREND_PERF_HECERR_FLAG_BC1_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 11, 2, 2, data, TxMessage)
#define NEAREND_PERF_HECERR_FLAG		0x2000 /* BIT 13 */

/* adslFarEndPerformanceStats Flags */
#define FAREND_PERF_LOS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 1, 0, 1, data, TxMessage)
#define FAREND_PERF_LOS_FLAG	0x1 /* BIT 0th position */
#define FAREND_PERF_LOF_FLAG	0x2 /* BIT 1 */
#define FAREND_PERF_LPR_FLAG	0x4 /* BIT 2 */
#define FAREND_PERF_NCD_FLAG	0x8 /* BIT 3 */
#define FAREND_PERF_LCD_FLAG	0x10 /* BIT 4 */
#define FAREND_PERF_CRC_FLAG_LP0_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 24, 0, 1, data, TxMessage)
#define FAREND_PERF_CRC_FLAG_LP1_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 24, 1, 1, data, TxMessage)
#define FAREND_PERF_CRC_FLAG	0x20 /* BIT 5 */
#define FAREND_PERF_RSCORR_FLAG_LP0_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 28, 0, 1, data, TxMessage)
#define FAREND_PERF_RSCORR_FLAG_LP1_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 28, 1, 1, data, TxMessage)
#define FAREND_PERF_RSCORR_FLAG	0x40 /* BIT 6 */
#define FAREND_PERF_FECS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 32, 0, 1, data, TxMessage)
#define FAREND_PERF_FECS_FLAG	0x80 /* BIT 7 */
#define FAREND_PERF_ES_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 33, 0, 1, data, TxMessage)
#define FAREND_PERF_ES_FLAG	0x100 /* BIT 8 */
#define FAREND_PERF_SES_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 34, 0, 1, data, TxMessage)
#define FAREND_PERF_SES_FLAG	0x200 /* BIT 9 */
#define FAREND_PERF_LOSS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 35, 0, 1, data, TxMessage)
#define FAREND_PERF_LOSS_FLAG	0x400 /* BIT 10 */
#define FAREND_PERF_UAS_FLAG_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 36, 0, 1, data, TxMessage)
#define FAREND_PERF_UAS_FLAG	0x800 /* BIT 11 */
#define FAREND_PERF_HECERR_FLAG_BC0_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 37, 0, 2, data, TxMessage)
#define FAREND_PERF_HECERR_FLAG_BC1_MAKECMV	makeCMV(H2D_CMV_READ, PLAM, 37, 2, 2, data, TxMessage)
#define FAREND_PERF_HECERR_FLAG	0x1000 /* BIT 12 */
// 603221:tc.chen end
/* TR-69 related additional parameters - defines */
/* Defines for  struct adslATURSubcarrierInfo */
#define	NEAREND_HLINSC	0x1
#define NEAREND_HLINSC_MAKECMV(mode)		makeCMV(mode, INFO, 71, 2, 1, data, TxMessage)
#define	NEAREND_HLINPS	0x2
#define NEAREND_HLINPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 73, idx, size, data, TxMessage)
#define	NEAREND_HLOGMT	0x4
#define NEAREND_HLOGMT_MAKECMV(mode)		makeCMV(mode, INFO, 80, 0, 1, data, TxMessage)
#define NEAREND_HLOGPS	0x8
#define NEAREND_HLOGPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 75, idx, size, data, TxMessage)
#define NEAREND_QLNMT	0x10
#define NEAREND_QLNMT_MAKECMV(mode)		makeCMV(mode, INFO, 80, 1, 1, data, TxMessage)
#define	NEAREND_QLNPS	0x20
#define NEAREND_QLNPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 77, idx, size, data, TxMessage)
#define	NEAREND_SNRMT	0x40
#define NEAREND_SNRMT_MAKECMV(mode)		makeCMV(mode, INFO, 80, 2, 1, data, TxMessage)
#define	NEAREND_SNRPS	0x80
#define NEAREND_SNRPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 78, idx, size, data, TxMessage)
#define	NEAREND_BITPS	0x100
#define NEAREND_BITPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 22, idx, size, data, TxMessage)
#define	NEAREND_GAINPS	0x200
#define NEAREND_GAINPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 24, idx, size, data, TxMessage)

/* Defines for  struct adslATUCSubcarrierInfo */
#define	 FAREND_HLINSC	0x1

/* As per the feedback from Knut on 21/08/2006, the cmv command of HLINSC should be INFO 70 2 */
#define  FAREND_HLINSC_MAKECMV(mode)		makeCMV(mode, INFO, 70, 2, 1, data, TxMessage)
#define	 FAREND_HLINPS	0x2
#define  FAREND_HLINPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 72, idx, size, data, TxMessage)
#define	 FAREND_HLOGMT	0x4
#define  FAREND_HLOGMT_MAKECMV(mode)		makeCMV(mode, INFO, 79, 0, 1, data, TxMessage)
#define  FAREND_HLOGPS	0x8
#define  FAREND_HLOGPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 74, idx, size, data, TxMessage)
#define  FAREND_QLNMT	0x10
#define  FAREND_QLNMT_MAKECMV(mode)		makeCMV(mode, INFO, 79, 1, 1, data, TxMessage)
#define	 FAREND_QLNPS	0x20
#define  FAREND_QLNPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 76, idx, size, data, TxMessage)
#define	 FAREND_SNRMT	0x40
#define  FAREND_SNRMT_MAKECMV(mode)		makeCMV(mode, INFO, 79, 2, 1, data, TxMessage)
#define	 FAREND_SNRPS	0x80
#define  FAREND_SNRPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 11, idx, size, data, TxMessage)
#define  FAREND_SNRPS_DIAG_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 10, idx, size, data, TxMessage)
#define	 FAREND_BITPS	0x100
#define  FAREND_BITPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 23, idx, size, data, TxMessage)
#define	 FAREND_GAINPS	0x200
#define  FAREND_GAINPS_MAKECMV(mode,idx,size)	makeCMV(mode, INFO, 25, idx, size, data, TxMessage)


// GET_ADSL_POWER_SPECTRAL_DENSITY
#define NOMPSD_US_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 102, 0, 1, data, TxMessage)
#define NOMPSD_DS_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 102, 1, 1, data, TxMessage)
#define PCB_US_MAKECMV		makeCMV(H2D_CMV_READ, INFO, 102, 6, 1, data, TxMessage)
#define PCB_DS_MAKECMV		makeCMV(H2D_CMV_READ, INFO, 102, 7, 1, data, TxMessage)
#define	RMSGI_US_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 102, 10, 1, data, TxMessage)
#define	RMSGI_DS_MAKECMV	makeCMV(H2D_CMV_READ, INFO, 102, 11, 1, data, TxMessage)

/////////////////////////////////////////////////Macro Definitions ? FLAG Setting & Testing

#define SET_FLAG(flags, flag_val)   ((*flags) = ((*flags) | flag_val))
//	-- This macro sets the flags with the flag_val. Here flags is passed as a pointer

#define IS_FLAG_SET(flags, test_flag)	(((*flags) & (test_flag)) == (test_flag)? test_flag:0)
// 	-- This macro verifies whether test_flag has been set in flags. Here flags is passed as a pointer


#define CLR_FLAG(flags, flag_bit)	((*flags) = (*flags) & (~flag_bit))
//	-- This macro resets the specified flag_bit in the flags. Here flags is passed as a pointer


////////////////////////////////////////////////DATA STRUCTURES ORGANIZATION
	
//Here are the data structures used for accessing mib parameters. The ioctl call includes the third parameter as a void pointer. This parameter has to be type-casted in the driver code to the corresponding structure depending upon the command type. For Ex: consider the ioctl used to get the adslLineCode type, ioctl(fd,GET_ADSL_LINE_CODE,void *struct_adslLineTableEntry). In the driver code we check on the type of the command, i.e GET_ADSL_LINE_CODE and type-cast the void pointer to struct adslLineTableEntry type.
	//
#define u32 unsigned int
#define u16 unsigned short
#define s16 short
#define u8 unsigned char


typedef u32 AdslPerfTimeElapsed;
typedef u32 AdslPerfPrevDayCount;
typedef u32 PerfCurrentCount;
typedef u32 PerfIntervalCount;
typedef u32 AdslPerfCurrDayCount;


//ioctl(int fd, GET_ADSL_LINE_CODE, void *struct_adslLineTableEntry)

typedef struct adslLineTableEntry {
	int ifIndex;
	int adslLineCode;
	u8 flags;
} adslLineTableEntry;

#ifdef IFXMIPS_MEI_MIB_RFC3440
typedef struct adslLineExtTableEntry {
	int ifIndex;
	u16 adslLineTransAtucCap;
	u16 adslLineTransAtucConfig;
	u16 adslLineTransAtucActual;
	int adslLineGlitePowerState;
	u32 flags;
}adslLineExtTableEntry;
#endif
//ioctl(int fd, GET_ADSL_ATUC_PHY, void  *struct_adslAtucPhysEntry)
#ifndef u_char 
#define u_char u8
#endif

typedef struct adslVendorId {
	u16	country_code;
	u_char	provider_id[4];  /* Ascii characters */
	u_char	revision_info[2];
}adslVendorId;

typedef struct adslAtucPhysEntry {
	int ifIndex;
	char serial_no[32];
	union {
	char vendor_id[16];
		adslVendorId vendor_info;
	} vendor_id;
	char version_no[16];
	u32 status;
	int outputPwr;
	u32 attainableRate;
	u8 flags;
} adslAtucPhysEntry;


//ioctl(int fd, GET_ADSL_ATUR_PHY, void  *struct_adslAturPhysEntry)

typedef struct adslAturPhysEntry {
	int ifIndex;
	char serial_no[32];
	union {
	char vendor_id[16];
		adslVendorId vendor_info;
	} vendor_id;
	char version_no[16];
	int SnrMgn;
	u32 Attn;
	u32 status;
	int outputPwr;
	u32 attainableRate;
	u8 flags;
} adslAturPhysEntry;


//ioctl(int fd, GET_ADSL_ATUC_CHAN_INFO, void *struct_adslAtucChanInfo)

typedef struct adslAtucChanInfo {
	int ifIndex;
 	u32 interleaveDelay;
	u32 currTxRate;
	u32 prevTxRate;
	u8 flags;
} adslAtucChanInfo;


//ioctl(int fd, GET_ADSL_ATUR_CHAN_INFO, void *struct_adslAturChanInfo)

typedef struct adslAturChanInfo {
	int ifIndex;
 	u32 interleaveDelay;
 	u32 currTxRate;
 	u32 prevTxRate;
 	u32 crcBlkLen;
 	u8 flags;
} adslAturChanInfo;


//ioctl(int fd, GET_ADSL_ATUC_PERF_DATA,  void *struct_atucPerfDataEntry)

typedef struct atucPerfDataEntry
{
   int			ifIndex;
   u32 			adslAtucPerfLofs;             
   u32 			adslAtucPerfLoss;             
   u32 			adslAtucPerfESs;                 
   u32 			adslAtucPerfInits;
   int         		adslAtucPerfValidIntervals;
   int         		adslAtucPerfInvalidIntervals;
   AdslPerfTimeElapsed 	adslAtucPerfCurr15MinTimeElapsed;
   PerfCurrentCount 	adslAtucPerfCurr15MinLofs;
   PerfCurrentCount 	adslAtucPerfCurr15MinLoss;
   PerfCurrentCount 	adslAtucPerfCurr15MinESs;
   PerfCurrentCount 	adslAtucPerfCurr15MinInits;
   AdslPerfTimeElapsed 	adslAtucPerfCurr1DayTimeElapsed;
   AdslPerfCurrDayCount adslAtucPerfCurr1DayLofs;
   AdslPerfCurrDayCount adslAtucPerfCurr1DayLoss;
   AdslPerfCurrDayCount adslAtucPerfCurr1DayESs;
   AdslPerfCurrDayCount adslAtucPerfCurr1DayInits;
   int         		adslAtucPerfPrev1DayMoniSecs;
   AdslPerfPrevDayCount adslAtucPerfPrev1DayLofs;
   AdslPerfPrevDayCount adslAtucPerfPrev1DayLoss;
   AdslPerfPrevDayCount adslAtucPerfPrev1DayESs;
   AdslPerfPrevDayCount adslAtucPerfPrev1DayInits;
   u32			flags;
} atucPerfDataEntry;

#ifdef IFXMIPS_MEI_MIB_RFC3440
typedef struct atucPerfDataExtEntry
 {
  int ifIndex;
  u32 adslAtucPerfStatFastR;
  u32 adslAtucPerfStatFailedFastR;
  u32 adslAtucPerfStatSesL;
  u32 adslAtucPerfStatUasL;
  u32 adslAtucPerfCurr15MinFastR;
  u32 adslAtucPerfCurr15MinFailedFastR;
  u32 adslAtucPerfCurr15MinSesL;
  u32 adslAtucPerfCurr15MinUasL;
  u32 adslAtucPerfCurr1DayFastR;
  u32 adslAtucPerfCurr1DayFailedFastR;
  u32 adslAtucPerfCurr1DaySesL;
  u32 adslAtucPerfCurr1DayUasL;
  u32 adslAtucPerfPrev1DayFastR;
  u32 adslAtucPerfPrev1DayFailedFastR;
  u32 adslAtucPerfPrev1DaySesL;
  u32 adslAtucPerfPrev1DayUasL;
  u32	flags;
} atucPerfDataExtEntry; 

#endif
//ioctl(int fd, GET_ADSL_ATUR_PERF_DATA, void *struct_aturPerfDataEntry)

typedef struct aturPerfDataEntry
{
   int			ifIndex;
   u32 			adslAturPerfLofs;             
   u32 			adslAturPerfLoss;             
   u32 			adslAturPerfLprs;                 
   u32 			adslAturPerfESs;
   int         		adslAturPerfValidIntervals;
   int         		adslAturPerfInvalidIntervals;
   AdslPerfTimeElapsed 	adslAturPerfCurr15MinTimeElapsed;
   PerfCurrentCount 	adslAturPerfCurr15MinLofs;
   PerfCurrentCount 	adslAturPerfCurr15MinLoss;
   PerfCurrentCount 	adslAturPerfCurr15MinLprs;
   PerfCurrentCount 	adslAturPerfCurr15MinESs;
   AdslPerfTimeElapsed 	adslAturPerfCurr1DayTimeElapsed;
   AdslPerfCurrDayCount adslAturPerfCurr1DayLofs;
   AdslPerfCurrDayCount adslAturPerfCurr1DayLoss;
   AdslPerfCurrDayCount adslAturPerfCurr1DayLprs;
   AdslPerfCurrDayCount adslAturPerfCurr1DayESs;
   int         		adslAturPerfPrev1DayMoniSecs;
   AdslPerfPrevDayCount adslAturPerfPrev1DayLofs;
   AdslPerfPrevDayCount adslAturPerfPrev1DayLoss;
   AdslPerfPrevDayCount adslAturPerfPrev1DayLprs;
   AdslPerfPrevDayCount adslAturPerfPrev1DayESs;
   u32			flags;
} aturPerfDataEntry;

#ifdef IFXMIPS_MEI_MIB_RFC3440
typedef struct aturPerfDataExtEntry
 {
  int ifIndex;
  u32 adslAturPerfStatSesL;
  u32 adslAturPerfStatUasL;
  u32 adslAturPerfCurr15MinSesL;
  u32 adslAturPerfCurr15MinUasL;
  u32 adslAturPerfCurr1DaySesL;
  u32 adslAturPerfCurr1DayUasL;
  u32 adslAturPerfPrev1DaySesL;
  u32 adslAturPerfPrev1DayUasL;
  u32	flags;
} aturPerfDataExtEntry;
#endif
//ioctl(int fd, GET_ADSL_ATUC_INTVL_INFO, void *struct_adslAtucInvtInfo)

typedef struct adslAtucIntvlInfo {
	int ifIndex;
        int IntervalNumber;
 	PerfIntervalCount intervalLOF;
 	PerfIntervalCount intervalLOS;
  	PerfIntervalCount intervalES;
 	PerfIntervalCount intervalInits; 
	int intervalValidData;
 	u8 flags;
} adslAtucIntvlInfo;

#ifdef IFXMIPS_MEI_MIB_RFC3440
typedef struct adslAtucInvtlExtInfo
 {
  int ifIndex;
  int IntervalNumber;
  u32 adslAtucIntervalFastR;
  u32 adslAtucIntervalFailedFastR;
  u32 adslAtucIntervalSesL;
  u32 adslAtucIntervalUasL;
  u32	flags;
} adslAtucInvtlExtInfo;
#endif
//ioctl(int fd, GET_ADSL_ATUR_INTVL_INFO, void *struct_adslAturInvtlInfo)

typedef struct adslAturIntvlInfo {
	int ifIndex;
        int IntervalNumber;
 	PerfIntervalCount intervalLOF;
 	PerfIntervalCount intervalLOS;
 	PerfIntervalCount intervalLPR;
  	PerfIntervalCount intervalES;
 	int intervalValidData;
 	u8 flags;
} adslAturIntvlInfo;

#ifdef IFXMIPS_MEI_MIB_RFC3440
typedef struct adslAturInvtlExtInfo
 {
  int ifIndex;
  int IntervalNumber;
  u32 adslAturIntervalSesL;
  u32 adslAturIntervalUasL;
  u32	flags;
} adslAturInvtlExtInfo;
#endif
//ioctl(int fd, GET_ADSL_ATUC_CHAN_PERF_DATA,  void *struct_atucChannelPerfDataEntry)

typedef struct atucChannelPerfDataEntry
{
   int			ifIndex;
   u32 			adslAtucChanReceivedBlks;             
   u32 			adslAtucChanTransmittedBlks;             
   u32 			adslAtucChanCorrectedBlks;                 
   u32 			adslAtucChanUncorrectBlks;
   int         		adslAtucChanPerfValidIntervals;
   int         		adslAtucChanPerfInvalidIntervals;
   AdslPerfTimeElapsed 	adslAtucChanPerfCurr15MinTimeElapsed;
   PerfCurrentCount 	adslAtucChanPerfCurr15MinReceivedBlks;
   PerfCurrentCount 	adslAtucChanPerfCurr15MinTransmittedBlks;
   PerfCurrentCount 	adslAtucChanPerfCurr15MinCorrectedBlks;
   PerfCurrentCount 	adslAtucChanPerfCurr15MinUncorrectBlks;
   AdslPerfTimeElapsed  adslAtucChanPerfCurr1DayTimeElapsed;
   AdslPerfCurrDayCount adslAtucChanPerfCurr1DayReceivedBlks;
   AdslPerfCurrDayCount adslAtucChanPerfCurr1DayTransmittedBlks;
   AdslPerfCurrDayCount adslAtucChanPerfCurr1DayCorrectedBlks;
   AdslPerfCurrDayCount adslAtucChanPerfCurr1DayUncorrectBlks;
   int                  adslAtucChanPerfPrev1DayMoniSecs;
   AdslPerfPrevDayCount adslAtucChanPerfPrev1DayReceivedBlks;
   AdslPerfPrevDayCount adslAtucChanPerfPrev1DayTransmittedBlks;
   AdslPerfPrevDayCount adslAtucChanPerfPrev1DayCorrectedBlks;
   AdslPerfPrevDayCount adslAtucChanPerfPrev1DayUncorrectBlks;
   u32			flags;
}atucChannelPerfDataEntry;


//ioctl(int fd, GET_ADSL_ATUR_CHAN_PERF_DATA,  void *struct_aturChannelPerfDataEntry)

typedef struct aturChannelPerfDataEntry
{
   int			ifIndex;
   u32 			adslAturChanReceivedBlks;             
   u32 			adslAturChanTransmittedBlks;             
   u32 			adslAturChanCorrectedBlks;                 
   u32 			adslAturChanUncorrectBlks;
   int         		adslAturChanPerfValidIntervals;
   int         		adslAturChanPerfInvalidIntervals;
   AdslPerfTimeElapsed 	adslAturChanPerfCurr15MinTimeElapsed;
   PerfCurrentCount 	adslAturChanPerfCurr15MinReceivedBlks;
   PerfCurrentCount 	adslAturChanPerfCurr15MinTransmittedBlks;
   PerfCurrentCount 	adslAturChanPerfCurr15MinCorrectedBlks;
   PerfCurrentCount 	adslAturChanPerfCurr15MinUncorrectBlks;
   AdslPerfTimeElapsed  adslAturChanPerfCurr1DayTimeElapsed;
   AdslPerfCurrDayCount adslAturChanPerfCurr1DayReceivedBlks;
   AdslPerfCurrDayCount adslAturChanPerfCurr1DayTransmittedBlks;
   AdslPerfCurrDayCount adslAturChanPerfCurr1DayCorrectedBlks;
   AdslPerfCurrDayCount adslAturChanPerfCurr1DayUncorrectBlks;
   int                  adslAturChanPerfPrev1DayMoniSecs;
   AdslPerfPrevDayCount adslAturChanPerfPrev1DayReceivedBlks;
   AdslPerfPrevDayCount adslAturChanPerfPrev1DayTransmittedBlks;
   AdslPerfPrevDayCount adslAturChanPerfPrev1DayCorrectedBlks;
   AdslPerfPrevDayCount adslAturChanPerfPrev1DayUncorrectBlks;
   u32			flags;
} aturChannelPerfDataEntry;


//ioctl(int fd, GET_ADSL_ATUC_CHAN_INTVL_INFO, void *struct_adslAtucChanIntvlInfo)

typedef struct adslAtucChanIntvlInfo {
	int ifIndex;
        int IntervalNumber;
 	PerfIntervalCount chanIntervalRecvdBlks;
 	PerfIntervalCount chanIntervalXmitBlks;
  	PerfIntervalCount chanIntervalCorrectedBlks;
 	PerfIntervalCount chanIntervalUncorrectBlks;
 	int intervalValidData;
 	u8 flags;
} adslAtucChanIntvlInfo;


//ioctl(int fd, GET_ADSL_ATUR_CHAN_INTVL_INFO, void *struct_adslAturChanIntvlInfo)

typedef struct adslAturChanIntvlInfo {
	int ifIndex;
        int IntervalNumber;
 	PerfIntervalCount chanIntervalRecvdBlks;
 	PerfIntervalCount chanIntervalXmitBlks;
  	PerfIntervalCount chanIntervalCorrectedBlks;
 	PerfIntervalCount chanIntervalUncorrectBlks;
 	int intervalValidData;
	u8 flags;
} adslAturChanIntvlInfo;


//ioctl(int fd, GET_ADSL_ALRM_CONF_PROF,  void *struct_adslLineAlarmConfProfileEntry)
//ioctl(int fd, SET_ADSL_ALRM_CONF_PROF,  void *struct_adslLineAlarmConfProfileEntry)

typedef struct  adslLineAlarmConfProfileEntry
 {
  unsigned char adslLineAlarmConfProfileName[32];
    int 	adslAtucThresh15MinLofs;
    int 	adslAtucThresh15MinLoss;
    int 	adslAtucThresh15MinESs;
    u32 	adslAtucThreshFastRateUp;
    u32 	adslAtucThreshInterleaveRateUp;
    u32 	adslAtucThreshFastRateDown;
    u32 	adslAtucThreshInterleaveRateDown;
    int 	adslAtucInitFailureTrapEnable;
    int 	adslAturThresh15MinLofs;
    int 	adslAturThresh15MinLoss;
    int 	adslAturThresh15MinLprs;
    int 	adslAturThresh15MinESs;
    u32 	adslAturThreshFastRateUp;
    u32 	adslAturThreshInterleaveRateUp;
    u32 	adslAturThreshFastRateDown;
    u32 	adslAturThreshInterleaveRateDown;
    int 	adslLineAlarmConfProfileRowStatus;
    u32	flags;
} adslLineAlarmConfProfileEntry;

#ifdef IFXMIPS_MEI_MIB_RFC3440
typedef struct adslLineAlarmConfProfileExtEntry
 {
  u8  adslLineAlarmConfProfileExtName[32];
  u32 adslAtucThreshold15MinFailedFastR;
  u32 adslAtucThreshold15MinSesL;
  u32 adslAtucThreshold15MinUasL;
  u32 adslAturThreshold15MinSesL;
  u32 adslAturThreshold15MinUasL;
  u32	flags;
} adslLineAlarmConfProfileExtEntry;
#endif
//TRAPS

/* The following Data Sturctures are added to support the WEB related parameters for ADSL Statistics */
typedef struct  adslLineStatus
 {
    int 	adslModemStatus;
    u32 	adslModeSelected;
    int 	adslAtucThresh15MinESs;
    int 	adslTrellisCodeEnable;
    int 	adslLatency;
    u8 flags;
 } adslLineStatusInfo;

typedef struct  adslLineRate
 {
    u32 	adslDataRateds;
    u32 	adslDataRateus;
    u32 	adslATTNDRds;	
    u32 	adslATTNDRus;	
    u8		flags;
 } adslLineRateInfo;

typedef struct  adslLineInfo
 {
    u32 	adslInterleaveDepthds;
    u32 	adslInterleaveDepthus;
    u32 	adslLATNds;
    u32 	adslLATNus;
    u32 	adslSATNds;
    u32 	adslSATNus;
    int	 	adslSNRMds;
    int	 	adslSNRMus;
    int	 	adslACATPds;
    int		adslACATPus;
    u32	flags;
 } adslLineInfo;

typedef struct  adslNearEndPerfStats
 {
    u32 	adslSuperFrames; 
    u32 	adslneLOS;
    u32 	adslneLOF;
    u32 	adslneLPR;
    u32 	adslneNCD;
    u32 	adslneLCD;
    u32 	adslneCRC;
    u32		adslneRSCorr;
    u32		adslneFECS;
    u32		adslneES;
    u32		adslneSES;
    u32		adslneLOSS;
    u32		adslneUAS;
    u32		adslneHECErrors;
    u32		flags;
 } adslNearEndPerfStats;

typedef struct  adslFarEndPerfStats
 {
    u32 	adslfeLOS;
    u32 	adslfeLOF;
    u32 	adslfeLPR;
    u32 	adslfeNCD;
    u32 	adslfeLCD;
    u32 	adslfeCRC;
    u32		adslfeRSCorr;
    u32		adslfeFECS;
    u32		adslfeES;
    u32		adslfeSES;
    u32		adslfeLOSS;
    u32		adslfeUAS;
    u32		adslfeHECErrors;
    u32		flags;
 } adslFarEndPerfStats;

/* The number of tones (and hence indexes) is dependent on the ADSL mode - G.992.1, G.992.2, G.992.3, * G.992.4 and G.992.5 */
typedef struct adslATURSubcarrierInfo {
	int 	ifindex;
	u16	HLINSCds;
	u16	HLINpsds[1024];/* Even index = real part; Odd Index
				    = imaginary part for each tone */
	u16	HLOGMTds;
	u16	HLOGpsds[512];
	u16	QLNMTds;
	u16	QLNpsds[512];
	u16	SNRMTds;
	u16	SNRpsds[512];  
	u16	BITpsds[512];
	s16	GAINpsds[512]; /* Signed value in 0.1dB units. i.e dB * 10.
				Needs to be converted into linear scale*/
	u16	flags;
}adslATURSubcarrierInfo;

typedef struct adslATUCSubcarrierInfo {
	int 	ifindex;
	u16	HLINSCus;
	u16	HLINpsus[128];/* Even index = real part; Odd Index
				    = imaginary part for each tone */
	u16	HLOGMTus;
	u16	HLOGpsus[64];
	u16	QLNMTus;
	u16	QLNpsus[64]; 
	u16	SNRMTus;
	u16 	SNRpsus[64];  
	u16	BITpsus[64];
	s16	GAINpsus[64]; /* Signed value in 0.1dB units. i.e dB * 10.
				Needs to be converted into linear scale*/
	u16	flags;
}adslATUCSubcarrierInfo;

#ifndef u_int16
#define u_int16 u16
#endif

typedef struct adslInitStats {
	u_int16	FullInitializationCount;
	u_int16 FailedFullInitializationCount;
	u_int16 LINIT_Errors;
	u_int16	Init_Timeouts;
}adslInitStats;

typedef struct adslPowerSpectralDensity {
	int	ACTPSDds;
	int 	ACTPSDus;
}adslPowerSpectralDensity;

//ioctl(int fd, ADSL_ATUR_TRAPS, void  *uint16_flags)
typedef union structpts {
	adslLineTableEntry * adslLineTableEntry_pt;
	adslAtucPhysEntry * adslAtucPhysEntry_pt;
	adslAturPhysEntry * adslAturPhysEntry_pt;
	adslAtucChanInfo * adslAtucChanInfo_pt;
	adslAturChanInfo * adslAturChanInfo_pt;
	atucPerfDataEntry * atucPerfDataEntry_pt;
	aturPerfDataEntry * aturPerfDataEntry_pt;
	adslAtucIntvlInfo * adslAtucIntvlInfo_pt;
	adslAturIntvlInfo * adslAturIntvlInfo_pt;
	atucChannelPerfDataEntry * atucChannelPerfDataEntry_pt;
	aturChannelPerfDataEntry * aturChannelPerfDataEntry_pt;
	adslAtucChanIntvlInfo * adslAtucChanIntvlInfo_pt;
	adslAturChanIntvlInfo * adslAturChanIntvlInfo_pt;
	adslLineAlarmConfProfileEntry * adslLineAlarmConfProfileEntry_pt;
	// RFC 3440
	
    #ifdef IFXMIPS_MEI_MIB_RFC3440
	adslLineExtTableEntry * adslLineExtTableEntry_pt;
	atucPerfDataExtEntry * atucPerfDataExtEntry_pt;
	adslAtucInvtlExtInfo * adslAtucInvtlExtInfo_pt;
	aturPerfDataExtEntry * aturPerfDataExtEntry_pt;
	adslAturInvtlExtInfo * adslAturInvtlExtInfo_pt;
	adslLineAlarmConfProfileExtEntry * adslLineAlarmConfProfileExtEntry_pt;
    #endif 
    	adslLineStatusInfo	* adslLineStatusInfo_pt;
    	adslLineRateInfo	* adslLineRateInfo_pt;
    	adslLineInfo		* adslLineInfo_pt;
    	adslNearEndPerfStats	* adslNearEndPerfStats_pt;
    	adslFarEndPerfStats	* adslFarEndPerfStats_pt;
	adslATUCSubcarrierInfo  * adslATUCSubcarrierInfo_pt;
	adslATURSubcarrierInfo  * adslATURSubcarrierInfo_pt;
	adslPowerSpectralDensity * adslPowerSpectralDensity_pt;
}structpts;

#endif /* ] __IFXMIPS_MEI_APP_IOCTL_H */
