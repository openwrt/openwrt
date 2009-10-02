/******************************************************************************
**
** FILE NAME    : ifxmips_mei_ioctl.h
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
#ifndef         _IFXMIPS_MEI_IOCTL_H
#define        	_IFXMIPS_MEI_IOCTL_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
#define PCM_BUFF_SIZE		1024	//bytes
//  interrupt numbers

#if !(defined(_IFXMIPS_ADSL_APP) || defined (_AMAZON_ADSL_APP))

// Number of intervals
#define INTERVAL_NUM					192	//two days
typedef struct ifxmips_mei_mib {
	struct list_head list;
	struct timeval start_time;	//start of current interval

	int AtucPerfLof;
	int AtucPerfLos;
	int AtucPerfEs;
	int AtucPerfInit;

	int AturPerfLof;
	int AturPerfLos;
	int AturPerfLpr;
	int AturPerfEs;

	int AturChanPerfRxBlk;
	int AturChanPerfTxBlk;
	int AturChanPerfCorrBlk;
	int AturChanPerfUncorrBlk;

	//RFC-3440
	int AtucPerfStatFastR;
	int AtucPerfStatFailedFastR;
	int AtucPerfStatSesL;
	int AtucPerfStatUasL;
	int AturPerfStatSesL;
	int AturPerfStatUasL;
} ifxmips_mei_mib;

typedef struct adslChanPrevTxRate {
	u32 adslAtucChanPrevTxRate;
	u32 adslAturChanPrevTxRate;
} adslChanPrevTxRate;

typedef struct adslPhysCurrStatus {
	u32 adslAtucCurrStatus;
	u32 adslAturCurrStatus;
} adslPhysCurrStatus;

typedef struct ChanType {
	int interleave;
	int fast;
	int bearchannel0;
	int bearchannel1;
} ChanType;

typedef struct mib_previous_read {
	u16 ATUC_PERF_ESS;
	u16 ATUR_PERF_ESS;
	u32 ATUR_CHAN_RECV_BLK;
	u16 ATUR_CHAN_CORR_BLK_INTL;
	u16 ATUR_CHAN_CORR_BLK_FAST;
	u16 ATUR_CHAN_UNCORR_BLK_INTL;
	u16 ATUR_CHAN_UNCORR_BLK_FAST;
	u16 ATUC_PERF_STAT_FASTR;
	u16 ATUC_PERF_STAT_FAILED_FASTR;
	u16 ATUC_PERF_STAT_SESL;
	u16 ATUC_PERF_STAT_UASL;
	u16 ATUR_PERF_STAT_SESL;
} mib_previous_read;

typedef struct mib_flags_pretime {
	struct timeval ATUC_PERF_LOSS_PTIME;
	struct timeval ATUC_PERF_LOFS_PTIME;
	struct timeval ATUR_PERF_LOSS_PTIME;
	struct timeval ATUR_PERF_LOFS_PTIME;
	struct timeval ATUR_PERF_LPR_PTIME;
} mib_flags_pretime;

		//  cmv message structures
#define	MP_PAYLOAD_SIZE         			12
typedef struct mpmessage {
	u16 iFunction;
	u16 iGroup;
	u16 iAddress;
	u16 iIndex;
	u16 iPayload[MP_PAYLOAD_SIZE];
} MPMessage;
#endif

typedef struct meireg {
	u32 iAddress;
	u32 iData;
} meireg;

#define MEIDEBUG_BUFFER_SIZES 50
typedef struct meidebug {
	u32 iAddress;
	u32 iCount;
	u32 buffer[MEIDEBUG_BUFFER_SIZES];
} meidebug;

//==============================================================================
// Group definitions                                                              
//==============================================================================
#define OPTN                    5
#define CNFG                    8
#define CNTL                    1
#define STAT                    2
#define RATE                    6
#define PLAM                    7
#define INFO                    3
#define TEST			4
//==============================================================================
// Opcode definitions
//==============================================================================
#define H2D_CMV_READ                            0x00
#define H2D_CMV_WRITE                           0x04
#define H2D_CMV_INDICATE_REPLY                  0x10
#define H2D_ERROR_OPCODE_UNKNOWN          	0x20
#define H2D_ERROR_CMV_UNKNOWN                	0x30

#define D2H_CMV_READ_REPLY                     	0x01
#define D2H_CMV_WRITE_REPLY                     0x05
#define D2H_CMV_INDICATE                        0x11
#define D2H_ERROR_OPCODE_UNKNOWN                0x21
#define D2H_ERROR_CMV_UNKNOWN                   0x31
#define D2H_ERROR_CMV_READ_NOT_AVAILABLE        0x41
#define D2H_ERROR_CMV_WRITE_ONLY                0x51
#define D2H_ERROR_CMV_READ_ONLY                 0x61

#define H2D_DEBUG_READ_DM                       0x02
#define H2D_DEBUG_READ_PM                       0x06
#define H2D_DEBUG_WRITE_DM                      0x0a
#define H2D_DEBUG_WRITE_PM                      0x0e

#define D2H_DEBUG_READ_DM_REPLY         	0x03
#define D2H_DEBUG_READ_FM_REPLY         	0x07
#define D2H_DEBUG_WRITE_DM_REPLY        	0x0b
#define D2H_DEBUG_WRITE_FM_REPLY        	0x0f
#define D2H_ERROR_ADDR_UNKNOWN          	0x33

#define D2H_AUTONOMOUS_MODEM_READY_MSG		0xf1
//==============================================================================
// INFO register address field definitions
//==============================================================================

#define INFO_TxState					0
#define INFO_RxState					1
#define INFO_TxNextState				2
#define INFO_RxNextState				3
#define INFO_TxStateJumpFrom			        4
#define INFO_RxStateJumpFrom			        5

#define INFO_ReverbSnrBuf				8
#define INFO_ReverbEchoSnrBuf			        9
#define INFO_MedleySnrBuf				10
#define INFO_RxShowtimeSnrBuf		              	11
#define INFO_DECdelay					12
#define INFO_DECExponent				13
#define INFO_DECTaps					14
#define INFO_AECdelay					15
#define INFO_AECExponent				16
#define INFO_AECTaps					17
#define INFO_TDQExponent				18
#define INFO_TDQTaps					19
#define INFO_FDQExponent				20
#define INFO_FDQTaps					21
#define INFO_USBat					22
#define INFO_DSBat					23
#define INFO_USFineGains				24
#define INFO_DSFineGains				25
#define INFO_BitloadFirstChannel		        26
#define INFO_BitloadLastChannel		                27
#define INFO_PollEOCData				28	// CO specific
#define INFO_CSNRMargin					29	// CO specific
#define INFO_RCMsgs1					30
#define INFO_RMsgs1					31
#define INFO_RMsgRA					32
#define INFO_RCMsgRA					33
#define INFO_RMsg2					34
#define INFO_RCMsg2					35
#define INFO_BitLoadOK				        36
#define INFO_RCRates1					37
#define INFO_RRates1Tab					38
#define INFO_RMsgs1Tab					39
#define INFO_RMsgRATab					40
#define INFO_RRatesRA					41
#define INFO_RCRatesRA					42
#define INFO_RRates2					43
#define INFO_RCRates2					44
#define INFO_PackedRMsg2				45
#define INFO_RxBitSwapFlag			        46
#define INFO_TxBitSwapFlag			        47
#define INFO_ShowtimeSNRUpdateCount	                48
#define INFO_ShowtimeFDQUpdateCount	                49
#define INFO_ShowtimeDECUpdateCount	                50
#define INFO_CopyRxBuffer				51
#define INFO_RxToneBuf				        52
#define INFO_TxToneBuf                                  53
#define INFO_Version					54
#define INFO_TimeStamp                                  55
#define INFO_feVendorID					56
#define INFO_feSerialNum				57
#define INFO_feVersionNum				58
#define INFO_BulkMemory					59	//Points to start of bulk memory
#define INFO_neVendorID                                 60
#define INFO_neVersionNum				61
#define INFO_neSerialNum				62

//==============================================================================
// RATE register address field definitions
//==============================================================================

#define RATE_UsRate					0
#define RATE_DsRate					1

//==============================================================================
// PLAM (Physical Layer Management) register address field definitions
//      (See G997.1 for reference)
//==============================================================================

	//                                      ///
	// Failure Flags        ///
	//                                      ///

#define PLAM_NearEndFailureFlags		0
#define PLAM_FarEndFailureFlags			1

	//                                                                      ///
	// Near End Failure Flags Bit Definitions       ///
	//                                                                      ///

// ADSL Failures ///
#define PLAM_LOS_FailureBit				0x0001
#define PLAM_LOF_FailureBit				0x0002
#define PLAM_LPR_FailureBit				0x0004
#define PLAM_RFI_FailureBit				0x0008

// ATM Failures ///
#define PLAM_NCD_LP0_FailureBit				0x0010
#define PLAM_NCD_LP1_FailureBit				0x0020
#define PLAM_LCD_LP0_FailureBit				0x0040
#define PLAM_LCD_LP1_FailureBit				0x0080

#define PLAM_NCD_BC0_FailureBit				0x0100
#define PLAM_NCD_BC1_FailureBit				0x0200
#define PLAM_LCD_BC0_FailureBit				0x0400
#define PLAM_LCD_BC1_FailureBit				0x0800
	//                                              ///
	// Performance Counts   ///
	//                                              ///

#define PLAM_NearEndCrcCnt				2
#define PLAM_CorrectedRSErrors			        3

#define PLAM_NearEndECSCnt				6
#define PLAM_NearEndESCnt		                7
#define PLAM_NearEndSESCnt				8
#define PLAM_NearEndLOSSCnt				9
#define PLAM_NearEndUASLCnt				10

#define PLAM_NearEndHECErrCnt				11

#define PLAM_NearEndHECTotCnt				16
#define PLAM_NearEndCellTotCnt			        18
#define PLAM_NearEndSfCntLSW				20
#define PLAM_NearEndSfCntMSW				21

#define PLAM_FarEndFebeCnt				24

#define PLAM_FarEndFecCnt				28

#define PLAM_FarEndFECSCnt				32
#define PLAM_FarEndESCnt				33
#define PLAM_FarEndSESCnt				34
#define PLAM_FarEndLOSSCnt				35
#define PLAM_FarEndUASLCnt				36

#define PLAM_FarEndHECErrCnt				37

#define PLAM_FarEndHECTotCnt				41

#define PLAM_FarEndCellTotCnt				43

#define PLAM_SNRMargin_0_1db				45

#define PLAM_SNRMargin					46

//==============================================================================
// CNTL register address and bit field definitions
//==============================================================================

#define CNTL_ModemControl				0

#define CNTL_ModemReset					0x0
#define CNTL_ModemStart					0x2

//==============================================================================
// STAT register address and bit field definitions
//==============================================================================

#define STAT_MacroState					0
#define STAT_Mode					1
#define STAT_DMTFramingMode				2
#define STAT_SleepState					3
#define STAT_Misc					4
#define STAT_FailureState				5

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // STAT_OLRStatus provides status of OLR
 //16-bit STAT_OLRStatus_DS
 //  [1:0]      :       OLR status 00=IDLE,  01=OLR_IN_PROGRESS, 10=OLR_Completed, 11=OLR_Aborted
 //  [3:2]:             Reserved
 //  [5:4]:             OLR_Type (1:bitswap; 2: DRR; 3: SRA)
 //  [7:6]:             Reserved
 //  [10:8]:            >0=Request. 0=not.   For DS, # of request transmissions/retransmissions (3 bits).
 //  [11]:              1=Receive Response, 0=not
 //  [15:12]:   Reserved
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///
#define STAT_OLRStatus_DS				6

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // STAT_OLRStatus provides status of OLR
 // 16-bit STAT_OLRStatus_US CMV
 //  [1:0]      :       OLR status 00=IDLE,  01=OLR_IN_PROGRESS, 10=OLR_Completed, 11=OLR_Aborted
 //  [3:2]:             Reserved
 //  [5:4]:             OLR_Type (1:bitswap; 2: DRR; 3: SRA)
 //  [7:6]:             Reserved
 //  [8]:               1=Request Received. 0=not.
 //  [10:9]:     Reserved
 //  [11]:              1=Response Sent, 0=not
 //  [15:12]:   Reserved
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
#define STAT_OLRStatus_US				7

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // STAT_PMStatus provides status of PM
 // 16-bit STAT_PMStatus CMV
 //  [1:0]      :       PM Status 00=IDLE,  01=PM_IN_PROGRESS, 10=PM_Completed, 11=PM_Aborted
 //  [2] :              0=ATU_R initiated PM; 1 = ATU_C initiated PM
 //  [3]:               Reserved
 //  [5:4]:             PM_Type (1:Simple Request; 2: L2 request; 3: L2 trim)
 //  [7:6]:             Reserved
 //  [10:8]:            >0=Request. 0=not.   # of request transmissions/retransmissions (3 bits).
 //  [11]:              1=Response, 0=not
 //  [15:12]:   Reserved
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///
#define STAT_PMStatus					8

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // 16-bit STAT_OLRError_DS, STAT_OLRError_US, STAT_PMError
 // [3:0]:          OLR/PM response reason code
 // [7:4]:             OLR/PM Internal error code
 // [15:8]:         OLR/PM Reserved for future
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 ///
#define STAT_OLRError_DS				9
#define STAT_OLRError_US				10
#define STAT_PMError					11

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STAT_MacroState
// MacroState reflects the high level state of the modem

#define STAT_InitState				0x0000
#define STAT_ReadyState		            	0x0001
#define STAT_FailState				0x0002
#define STAT_IdleState				0x0003
#define STAT_QuietState				0x0004
#define STAT_GhsState				0x0005
#define STAT_FullInitState			0x0006
#define STAT_ShowTimeState			0x0007
#define STAT_FastRetrainState			0x0008
#define STAT_LoopDiagMode   			0x0009
#define STAT_ShortInit          		0x000A	// Bis short initialization ///

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STAT_Mode
// ConfigurationMode indicates the mode of the current ADSL Link. In general, a modem may use
// G.Hs or some other mechanism to negotiate the specific mode of operation.
// The OPTN_modeControl CMV is used to select a set of desired modes.
// The STAT_Mode CMV indicates which mode was actually selected.

#define STAT_ConfigMode_T1413			0x0001
#define STAT_ConfigMode_G992_2_AB		0x0002
#define STAT_ConfigMode_G992_1_A		0x0004
#define STAT_ConfigMode_G992_1_B		0x0008
#define STAT_ConfigMode_G992_1_C		0x0010
#define STAT_ConfigMode_G992_2_C		0x0020

#define STAT_ConfigMode_G992_3_A		0x0100
#define STAT_ConfigMode_G992_3_B		0x0200
#define STAT_ConfigMode_G992_3_I		0x0400
#define STAT_ConfigMode_G992_3_J		0x0800
#define STAT_ConfigMode_G992_3_L		0x1000

#define STAT_ConfigMode_G992_4_A		0x2000
#define STAT_ConfigMode_G992_4_I		0x4000

#define STAT_ConfigMode_G992_5			0x8000

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STAT_DMTFramingMode
// FramingMode indicates the DMT framing mde negotiated during initialization. The framing mode
// status is not applicable in BIS mode and its value is undefined
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STAT_FramingModeMask			0x0003

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STAT_Misc
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STAT_OverlappedSpectrum		0x0008
#define STAT_TCM			0x0010
#define STAT_TDQ_at_1104	        0x0020
#define STAT_T1413_Signal_Detected	0x0040
#define STAT_AnnexL_US_Mask1_PSD    	0x1000	//indicate we actually selected G992.3 AnnexL US PSD mask1
#define STAT_AnnexL_US_Mask2_PSD   	0x2000	//indicate we actually selected G992.3 AnnexL US PSD mask2

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STAT_FailureState
// when the MacroSTate indicates the fail state, FailureState provides a failure code
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define E_CODE_NO_ERROR								0
#define E_CODE_BAT_TX								1	// TX BAT table is incorrect */
#define E_CODE_BAT_RX								2	//  RX BAT table is incorrect */
#define E_CODE_PROFILE								3	//  profile is not selected in fast retrain */
#define E_CODE_TX_AOC_FIFO_OVERFLOW						4
#define E_CODE_TRUNCATE_FR							5	//Fast Retrain truncated due to no stored profiles*/
#define E_CODE_BITLOAD								6	//  bit loading fails */
#define E_CODE_ST_ERROR								7	//  showtime CRC error */
#define E_CODE_RESERVED								8	//  using parameters reserved by the ITU-T */
#define E_CODE_C_TONES								9	//  detected C_TONES */
#define E_CODE_CODESWAP_ERR							10	//  codeswap not finished in time */
#define E_CODE_FIFO_OVERFLOW							11	// we have run out of fifo space */
#define E_CODE_C_BG_DECODE_ERR							12	// error in decoding C-BG message */
#define E_CODE_C_RATES2_DECODE_ERR						13	// error in decoding C-MSGS2 and C-RATES2 */
#define E_CODE_RCMedleyRx_C_SEGUE2_Failure					14	//  Timeout after RCMedleyRx waiting for C_SEGUE2 */
#define E_CODE_RReverbRATx_C_SEGUE2_Failure					15	//  Timeout after RReverbRATx waiting for C_SEGUE2 */
#define E_CODE_RReverb3Tx_C_SEGUE1_Failure					16	//  Timeout after RReverb3Tx waiting for C_SEGUE1 */
#define E_CODE_RCCRC2Rx_C_RATES1_DECOD_ERR					17	//  Received CRC not equal to computed CRC */
#define E_CODE_RCCRC1Rx_C_RATES1_DECOD_ERR					18	//  Received CRC not equal to computed CRC */
#define E_CODE_RReverb5Tx_C_SEGUE2_Failure					19	//  Timeout after RReverb5Tx waiting for C_SEGUE2 */
#define E_CODE_RReverb6Tx_C_SEGUE3_Failure					20	//  Timeout after RReverb6Tx waiting for C_SEGUE3 */
#define E_CODE_RSegue5Tx_C_SEGUE3_Failure					21	//  Timeout after RSegue5Tx waiting for C_SEGUE3 */
#define E_CODE_RCReverb5Rx_C_SEGUE_Failure					22	//  Timeout after RCReverb5Rx waiting for C_SEGUE */
#define E_CODE_RCReverbRARx_C_SEGUE2_Failure					23	//  Timeout after RCReverbRARx waiting for C_SEGUE2 */
#define E_CODE_RCCRC4Rx_CMSGS2_DECOD_ERR					24	//  Received CRC not equal to computed CRC */
#define E_CODE_RCCRC5Rx_C_BG_DECOD_ERR						25	//  Received CRC not equal to computed CRC */
#define E_CODE_RCCRC3Rx_DECOD_ERR						26	//  Received CRC not equal to computed CRC */
#define E_CODE_RCPilot3_DEC_PATH_DEL_TIMEOUT					27	//  DEC Path Delay timeout */
#define E_CODE_RCPilot3_DEC_TRAINING_TIMEOUT					28	//  DEC Training timeout */
#define E_CODE_RCReverb3Rx_C_SEGUE1_Failure					29	//  Timeout after RCReverb3Rx waiting for C_SEGUE1 */
#define E_CODE_RCReverb2Rx_SignalEnd_Failure					30	//  Timeout waiting for the end of RCReverb2Rx signal */
#define E_CODE_RQuiet2_SignalEnd_Failure					31	//  Timeout waiting for the end of RQuiet2 signal */
#define E_CODE_RCReverbFR1Rx_Failure						32	//  Timeout waiting for the end of RCReverbFR1Rx signal */
#define E_CODE_RCPilotFR1Rx_SignalEnd_Failure					33	//  Timeout waiting for the end of RCPilotFR1Rx signal */
#define E_CODE_RCReverbFR2Rx_C_Segue_Failure					34	//  Timeout after RCReverbFR2Rx waiting for C_SEGUE */
#define E_CODE_RCReverbFR5Rx_SignalEnd_TIMEOUT					35	//  Timeout waiting for the end of RCReverbFR5Rx signal */
#define E_CODE_RCReverbFR6Rx_C_SEGUE_Failure					36	//  Timeout after RCReverbFR6Rx waiting for C_SEGUE */
#define E_CODE_RCReverbFR8Rx_C_SEGUE_FR4_Failure				37	//  Timeout after RCReverbFR8Rx waiting for C_SEGUE_FR4 */
#define E_CODE_RCReverbFR8Rx_No_PROFILE						38	//  Timeout since no profile was selected */
#define E_CODE_RCReverbFR8Rx_SignalEnd_TIMEOUT					39	//  Timeout waiting for the end of RCReverbFR8Rx signal */
#define E_CODE_RCCRCFR1_DECOD_ERR						40	//  Received CRC not equal to computed CRC */
#define E_CODE_RCRecovRx_SingnalEnd_TIMEOUT					41	//  Timeout waiting for the end of RCRecovRx signal */
#define E_CODE_RSegueFR5Tx_TX_Not_Ready_TIMEOUT					42	//  Timeout after RSegueFR5Tx waiting for C_SEGUE2 */
#define E_CODE_RRecovTx_SignalEnd_TIMEOUT					43	//  Timeout waiting for the end of RRecovTx signal */
#define E_CODE_RCMedleyFRRx_C_SEGUE2_Failure					44	//  Timeout after RCMedleyFRRx waiting for C_SEGUE2 */
#define E_CODE_CONFIGURATION_PARAMETERS_ERROR					45	// one of the configuration parameters do not meet the standard */
#define E_CODE_BAD_MEM_ACCESS							46
#define E_CODE_BAD_INSTRUCTION_ACCESS						47
#define E_CODE_TX_EOC_FIFO_OVERFLOW						48
#define E_CODE_RX_EOC_FIFO_OVERFLOW						49
#define E_CODE_GHS_CD_FLAG_TIME_OUT						50	// Timeout when transmitting Flag in handshake cleardown */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//STAT_OLRStatus:
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define STAT_OLRPM_IDLE                                 0x0000
#define STAT_OLRPM_IN_PROGRESS                          0x0001
#define STAT_OLRPM_COMPLETE                             0x0002
#define STAT_OLRPM_ABORTED                              0x0003
#define STAT_OLRPM_RESPONSE				0x0800

#define STAT_OLR_BITSWAP                                0x0010
#define STAT_OLR_DRR					0x0020
#define STAT_OLR_SRA					0x0030

//STAT_PMStatus_US:
#define STAT_PM_CO_REQ                                  0x0004
#define STAT_PM_SIMPLE_REQ                              0x0010
#define STAT_PM_L2_REQ					0x0020
#define STAT_PM_L2_TRIM_REQ				0x0030

// STAT_OLRError_DS, STAT_OLRError_US
//4 bit response reason code:
#define RESP_BUSY					0x01
#define RESP_INVALID_PARAMETERS		            	0x02
#define RESP_NOT_ENABLED				0x03
#define RESP_NOT_SUPPORTED			        0x04

//4 bit internal error code (common for OLR and PM)
#define REQ_INVALID_BiGi				0x10
#define REQ_INVALID_Lp					0x20
#define REQ_INVALID_Bpn					0x30
#define REQ_INVALID_FRAMING_CONSTRAINT			0x40
#define REQ_NOT_IN_L0_STATE				0x50
#define REQ_NOT_IN_L2_STATE				0x60
#define REQ_INVALID_PCB					0x70
#define REQ_VIOLATES_MARGIN				0x80

//STAT_PMError
//4 bit response reason code:
#define RESP_STATE_NOT_DESIRED                          0x03
#define RESP_INFEASIBLE_PARAMETERS                      0x04

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPTN register address and bit field definitions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OPTN_ModeControl				0
#define OPTN_DMTLnkCtl                          	1
// Reserved                                             2
#define OPTN_GhsControl					3
// Reserved                                             4
#define OPTN_PwrManControl			        5
#define OPTN_AnnexControl				6
#define OPTN_ModeControl1				7
// Reserved                                             8
#define OPTN_StateMachineCtrl			        9
// Reserved                                             10
// Reserved                                             11
#define OPTN_BisLinkControl				12
#define OPTN_ATMAddrConfig			        13
#define OPTN_ATMNumCellConfig				14

// Mode control defines the allowable operating modes of an ADSL link. In general, a modem may ///
// use G.Hs or some other mechanism to negotiate the specific mode of operation. ///
// The OPTN_ModeControl CMV is used to select a set of desired modes ///
// The STAT_ModeControl CMV indicates which mode was actually selected ///

// OPTN_ModeControl
#define OPTN_ConfigMode_T1413			0x0001
#define OPTN_ConfigMode_G992_2_AB		0x0002
#define OPTN_ConfigMode_G992_1_A		0x0004
#define OPTN_ConfigMode_G992_1_B		0x0008
#define OPTN_ConfigMode_G992_1_C		0x0010
#define OPTN_ConfigMode_G992_2_C		0x0020

#define OPTN_ConfigMode_G992_3_A		0x0100
#define OPTN_ConfigMode_G992_3_B		0x0200
#define OPTN_ConfigMode_G992_3_I		0x0400
#define OPTN_ConfigMode_G992_3_J		0x0800
#define OPTN_ConfigMode_G992_3_L		0x1000

#define OPTN_ConfigMode_G992_4_A		0x2000
#define OPTN_ConfigMode_G992_4_I		0x4000

#define OPTN_ConfigMode_G992_5			0x8000

// OPTN_PwrManControl
#define OPTN_PwrManWakeUpGhs			0x1
#define OPTN_PwrManWakeUpFR			0x2

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPTN_DMT Link Control
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OPTN_DMT_DualLatency_Dis                0x200
#define OPTN_DMT_S_Dis                          0x100
#define OPTN_DMT_FRAMINGMODE                 	0x1
#define OPTN_DMT_FRAMINGMODE_MASK     		0x7

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPTN_BIS Link Control
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define OPTN_BisLinkContrl_LineProbeDis         0x1
#define OPTN_BisLinkContrl_DSBlackBitsEn        0x2
#define OPTN_BisLinkContrl_DiagnosticModeEn     0x4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPTN_GhsControl
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// for OPTN_GhsControl, we will assign 16bit word as follows
// bit 0~3: set the control over which start(initial) message CPE will send:
//
//              BIT: 2  1  0
//                       0  0  1  CLR
//                       0  1  0  MR
//                       0  1  1  MS
//                       1  0  0  MP
//
//  // bit 4~6: set the control over which message will be sent when we get at lease one CL/CLR exchange
//        BIT: 5  4
//                   0  1  MS
//                       1  0  MR
//                       1  1  MP
//
//  // bit 15: RT initiated G.hs sample sessions one through eight.  Session one is default.
//        BIT: 15
//                        1  means session one
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OPTN_GHS_ST_GHS					0x8000
#define OPTN_GHS_INIT_MASK		            	0x000F
#define OPTN_GHS_RESP_MASK				0x00F0

#define OPTN_RTInitTxMsg_CLR		           	0x0001
#define OPTN_RTInitTxMsg_MR		           	0x0002
#define OPTN_RTInitTxMsg_MS		            	0x0003
#define OPTN_RTInitTxMsg_MP		            	0x0004

#define OPTN_RTRespTxMsg_MS				0x0010
#define OPTN_RTRespTxMsg_MR				0x0020
#define OPTN_RTRespTxMsg_MP				0x0030

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//      OPTN_AnnexControl
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// G.992.3 Annex A/L1/L2 US PSD Mask preferred

#define OPTN_G992_3_AnnexA_PreferredModeMask		0x3000
#define OPTN_G992_3_AnnexA_PreferredModeA		0x0000	// default AnnexA PSD mask ///
#define OPTN_G992_3_AnnexA_PreferredModeL1	        0x1000	// AnnexL wide spectrum upstream PSD mask ///
#define OPTN_G992_3_AnnexA_PreferredModeL2	        0x2000	// AnnexL narrow spectrum upstream PSD mask ///

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OPTN_ATMAddrConfig
// Bits 4:0             are Utopia address for BC1
// Bits 9:5             are Utopia address for BC0
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OPTN_UTPADDR_BC1				0x001F
#define OPTN_UTPADDR_BC0				0x03E0

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OPTN_ATMNumCellConfig
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define OPTN_BC1_NUM_CELL_PAGES				0x000F	// Bits 0:3 ///
#define OPTN_BC0_NUM_CELL_PAGES				0x00F0	// Bits 4:7 ///

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CNFG register address field ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////
// these cmvs are used by bis handshake ///
///////////////////////////////////////////

// Each of the CNFG_TPS entries points to a structure of type (TPS_TC_BearerChannel_t)
#define CNFG_TPS_TC_DS0					0
#define CNFG_TPS_TC_DS1					1
#define CNFG_TPS_TC_US0					2
#define CNFG_TPS_TC_US1					3

#define CNFG_HDLC_Overhead_Requirements	                4

// Each of the CNFG_PMS entries points to a structure of type (PMS_TC_LatencyPath_t)
#define CNFG_PMS_TC_DS0					5
#define CNFG_PMS_TC_DS1					6
#define CNFG_PMS_TC_US0					7
#define CNFG_PMS_TC_US1					8

// CNFG_PMD_PARAMETERS points to a structure of type (PMD_params_t)
#define CNFG_PMD_PARAMETERS				9

////////////////////////////////////////////////////////////
// these cmvs are used by bis training and showtime code ///
////////////////////////////////////////////////////////////

////////////////
// Tx Config ///
////////////////
#define CNFG_tx_Cnfg_Nbc				10
#define CNFG_tx_Cnfg_Nlp				11
#define CNFG_tx_Cnfg_Rp					12
#define CNFG_tx_Cnfg_Mp					13
#define CNFG_tx_Cnfg_Lp					14
#define CNFG_tx_Cnfg_Tp					15
#define CNFG_tx_Cnfg_Dp					16
#define CNFG_tx_Cnfg_Bpn				17
#define CNFG_tx_Cnfg_FramingMode		        18
#define CNFG_tx_Cnfg_MSGLp			        19
#define CNFG_tx_Cnfg_MSGc				20

////////////////
// Rx Config ///
////////////////
#define CNFG_rx_Cnfg_Nbc				21
#define CNFG_rx_Cnfg_Nlp				22
#define CNFG_rx_Cnfg_Rp					23
#define CNFG_rx_Cnfg_Mp					24
#define CNFG_rx_Cnfg_Lp					25
#define CNFG_rx_Cnfg_Tp					26
#define CNFG_rx_Cnfg_Dp					27
#define CNFG_rx_Cnfg_Bpn				28
#define CNFG_rx_Cnfg_FramingMode	                29
#define CNFG_rx_Cnfg_MSGLp		                30
#define CNFG_rx_Cnfg_MSGc				31

#define CNFG_tx_Cnfg_BCnToLPp				32
#define CNFG_rx_Cnfg_BCnToLPp				33

#endif
