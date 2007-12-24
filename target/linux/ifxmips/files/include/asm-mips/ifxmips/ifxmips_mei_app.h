/******************************************************************************
**
** FILE NAME    : ifxmips_mei_app.h
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
#ifndef       	_IFXMIPS_MEI_APP_H
#define        	_IFXMIPS_MEI_APP_H
		//  ioctl control
#define IFXMIPS_MEI_START 	                       	300
#define IFXMIPS_MEI_REPLY                           	301
#define IFXMIPS_MEI_NOREPLY                      	302

#define IFXMIPS_MEI_RESET				303
#define IFXMIPS_MEI_REBOOT				304
#define IFXMIPS_MEI_HALT					305
#define IFXMIPS_MEI_CMV_WINHOST				306
#define IFXMIPS_MEI_CMV_READ				307
#define IFXMIPS_MEI_CMV_WRITE				308
#define IFXMIPS_MEI_MIB_DAEMON				309
#define IFXMIPS_MEI_SHOWTIME				310
#define IFXMIPS_MEI_REMOTE				311
#define IFXMIPS_MEI_READDEBUG				312
#define IFXMIPS_MEI_WRITEDEBUG				313
#define IFXMIPS_MEI_LOP					314

#define IFXMIPS_MEI_PCM_SETUP				315
#define IFXMIPS_MEI_PCM_START_TIMER			316
#define IFXMIPS_MEI_PCM_STOP_TIMER			317
#define IFXMIPS_MEI_PCM_CHECK				318
#define IFXMIPS_MEI_GET_EOC_LEN				319
#define IFXMIPS_MEI_GET_EOC_DATA				320
#define IFXMIPS_MEI_PCM_GETDATA				321
#define IFXMIPS_MEI_PCM_GPIO				322
#define IFXMIPS_MEI_EOC_SEND				323
#define IFXMIPS_MEI_DOWNLOAD				326
#define IFXMIPS_MEI_JTAG_ENABLE				327
#define IFXMIPS_MEI_RUN					328
#define IFXMIPS_MEI_DEBUG_MODE				329

/* Loop diagnostics mode of the ADSL line related constants */
#define SET_ADSL_LOOP_DIAGNOSTICS_MODE 			330
#define GET_ADSL_LOOP_DIAGNOSTICS_MODE 			331
#define LOOP_DIAGNOSTIC_MODE_COMPLETE                   332
#define IS_ADSL_LOOP_DIAGNOSTICS_MODE_COMPLETE		333

/* L3 Power Mode */
/* Get current Power Moaagement Mode Status*/
#define GET_POWER_MANAGEMENT_MODE			334
/* Set L3 Power Mode /disable L3 power mode */
#define SET_L3_POWER_MODE				335

/* get current dual latency configuration */
#define GET_ADSL_DUAL_LATENCY				336
/* enable/disable dual latency path */
#define SET_ADSL_DUAL_LATENCY				337

/* Enable/Disable autoboot mode. */
/* When the autoboot mode is disabled, the driver will excute some cmv 
   commands for led control and dual latency when DSL startup.*/
#define AUTOBOOT_ENABLE_SET				338

/* Enable/Disable Quiet Mode*/
/* Quiet mode is used for firmware debug. if the quiet mode enable, the autoboot daemon will not reset arc when the arc need to reboot */
#define QUIET_MODE_GET					339
#define QUIET_MODE_SET					340

/* Enable/Disable showtime lock*/
/* showtime lock is used for firmware debug. if the showtime lock enable, the autoboot daemon will not reset arc when the arc reach showtime and need to reboot */
#define SHOWTIME_LOCK_GET				341
#define SHOWTIME_LOCK_SET				342

#define L0_POWER_MODE 0
#define L2_POWER_MODE 2
#define L3_POWER_MODE 3

#define DUAL_LATENCY_US_DS_DISABLE			0
#define DUAL_LATENCY_US_ENABLE				(1<<0)
#define DUAL_LATENCY_DS_ENABLE				(1<<1)
#define DUAL_LATENCY_US_DS_ENABLE			(DUAL_LATENCY_US_ENABLE|DUAL_LATENCY_DS_ENABLE)

#define ME_HDLC_IDLE 0
#define ME_HDLC_INVALID_MSG 1
#define ME_HDLC_MSG_QUEUED 2
#define ME_HDLC_MSG_SENT 3
#define ME_HDLC_RESP_RCVD 4
#define ME_HDLC_RESP_TIMEOUT 5
#define ME_HDLC_RX_BUF_OVERFLOW 6
#define ME_HDLC_UNRESOLVED 1
#define ME_HDLC_RESOLVED 2

/***	Enums    ***/
typedef enum mei_error {
	MEI_SUCCESS = 0,
	MEI_FAILURE = -1,
	MEI_MAILBOX_FULL = -2,
	MEI_MAILBOX_EMPTY = -3,
	MEI_MAILBOX_TIMEOUT = -4,
} MEI_ERROR;

#endif
