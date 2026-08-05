/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _ECNT_HOOK_QDMA_TYPE_H_
#define _ECNT_HOOK_QDMA_TYPE_H_


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
	
#ifdef QDMA_LAN
#define ECNT_QDMA ECNT_QDMA_LAN
#define storm_ctrl_shrehold storm_ctrl_shrehold_lan
#define qdma_fwd_timer qdma_lan_fwd_timer
#else
#define ECNT_QDMA ECNT_QDMA_WAN
#define storm_ctrl_shrehold storm_ctrl_shrehold_wan
#define qdma_fwd_timer qdma_wan_fwd_timer
#endif


/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define CONFIG_QDMA_QUEUE	8
#define ECNT_DRIVER_API  0

#define TX_RING_NUM		(2)
#define RX_RING_NUM		(2)
#define RING_IDX_0		(0)
#define RING_IDX_1		(1)

#define QDMA_BUSY_TIMER_MAX 60 /* 60s */
#define QDMA_REG_POLLING_MAX 60 /* 60s */

#define QDMA_MAGIC					(0xAB)
#define IO_QDMA_CMD_OPT				_IOW(QDMA_MAGIC, 0, struct ECNT_QDMA_Data *)
#if 0
/* Interface IO Command */
#define IO_QDMA_SET_QOS_CONFIG				_IOW(QDMA_MAGIC, 0x01, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_QOS_VALUE				_IOW(QDMA_MAGIC, 0x02, struct ECNT_QDMA_Data *)
#define IO_QDMA_GET_QOS						_IOR(QDMA_MAGIC, 0x02, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_TX_RATELIMIT_VALUE		_IOW(QDMA_MAGIC, 0x10, struct ECNT_QDMA_Data *)
#define IO_QDMA_GET_TX_RATELIMIT_VALUE		_IOR(QDMA_MAGIC, 0x10, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_RX_RATELIMIT_VALUE		_IOW(QDMA_MAGIC, 0x18, struct ECNT_QDMA_Data *)
#define IO_QDMA_GET_RX_RATELIMIT_VALUE		_IOR(QDMA_MAGIC, 0x18, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_RX_RATELIMIT_EN			_IOW(QDMA_MAGIC, 0x19, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_RX_RATELIMIT_PKT_MODE	_IOW(QDMA_MAGIC, 0x1A, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_VIRTUAL_CHANNEL_CONFIG	_IOW(QDMA_MAGIC, 0x20, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_VIRTUAL_CHANNEL_VALUE	_IOW(QDMA_MAGIC, 0x21, struct ECNT_QDMA_Data *)
#define IO_QDMA_GET_VIRTUAL_CHANNEL			_IOR(QDMA_MAGIC, 0x21, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_RX_LOW_THRESHOLD		_IOW(QDMA_MAGIC, 0x24, struct ECNT_QDMA_Data *)
#define IO_QDMA_GET_RX_LOW_THRESHOLD		_IOR(QDMA_MAGIC, 0x24, struct ECNT_QDMA_Data *)
#define IO_QDMA_SET_DEI_DROP_EN				_IOW(QDMA_MAGIC, 0x25, struct ECNT_QDMA_Data *)
#define IO_QDMA_GET_DEI_DROP_EN				_IOR(QDMA_MAGIC, 0x25, struct ECNT_QDMA_Data *)

#define IO_QDMA_CLEAR_CPU_COUNTER			_IOW(QDMA_MAGIC, 0xC1, struct ECNT_QDMA_Data *)
#define IO_QDMA_DUMP_CPU_COUNTER			_IOW(QDMA_MAGIC, 0xC2, struct ECNT_QDMA_Data *)
#define IO_QDMA_DUMP_REGISTER				_IOW(QDMA_MAGIC, 0xC3, struct ECNT_QDMA_Data *)
#define IO_QDMA_DUMP_DESCRIPTOR				_IOW(QDMA_MAGIC, 0xC4, struct ECNT_QDMA_Data *)
#define IO_QDMA_DUMP_IRQ					_IOW(QDMA_MAGIC, 0xC5, struct ECNT_QDMA_Data *)
#define IO_QDMA_DUMP_HWFWD					_IOW(QDMA_MAGIC, 0xC6, struct ECNT_QDMA_Data *)
#endif


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
	
struct port_info;

/***************************************
 enum definition
***************************************/
typedef enum {
	DBG_ERR ,
	DBG_ST ,
	DBG_WARN ,
	DBG_MSG ,
	DBG_LEVEL_MAX
} QDMA_DebugLevel_t ;

typedef enum {
	QDMA_CALLBACK_RX_PACKET,
#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
    QDMA_CALLBACK_RX_PACKET_PTM_LAN,
#endif
	QDMA_CALLBACK_EVENT_HANDLER,
	QDMA_CALLBACK_RECYCLE_PACKET,
	QDMA_CALLBACK_GPON_MAC_HANDLER,
	QDMA_CALLBACK_EPON_MAC_HANDLER,
	QDMA_CALLBACK_XPON_PHY_HANDLER,
} QDMA_CbType_t ;

typedef enum {
	QDMA_EVENT_RECV_PKTS = 0 ,
	QDMA_EVENT_NO_RX_BUFFER ,
	QDMA_EVENT_TX_CROWDED
} QDMA_EventType_t ;

typedef enum {
	QDMA_LOOPBACK_DISABLE = 0 ,
	QDMA_LOOPBACK_QDMA ,
	QDMA_LOOPBACK_UMAC
} QDMA_LoopbackMode_t ;

typedef enum {
	QDMA_TX_POLLING = 0 ,
	QDMA_TX_INTERRUPT ,
} QDMA_RecycleMode_t ;

typedef enum {
	QDMA_RX_POLLING = 0 ,
	QDMA_RX_INTERRUPT ,
	QDMA_RX_NAPI
} QDMA_RecvMode_t ;

typedef enum {
	QDMA_DISABLE = 0 ,
	QDMA_ENABLE
} QDMA_Mode_t ;

typedef enum {
	QDMA_WAN_TYPE_GPON = 0,
	QDMA_WAN_TYPE_EPON,
	QDMA_WAN_TYPE_PTM,
	QDMA_WAN_TYPE_SAR
} QDMA_WanType_t ;

typedef enum {
	QDMA_TXQOS_WEIGHT_BY_PACKET = 0,
	QDMA_TXQOS_WEIGHT_BY_BYTE,
	QDMA_TXQOS_WEIGHT_MAX,
} QDMA_TxQosWeightType_t ;

typedef enum {
	QDMA_TXQOS_WEIGHT_SCALE_64B = 0,
	QDMA_TXQOS_WEIGHT_SCALE_16B,
	QDMA_TXQOS_WEIGHT_SCALE_MAX,
} QDMA_TxQosWeightScale_t ;

typedef enum {
	QDMA_TXQOS_TYPE_WRR = 0,
	QDMA_TXQOS_TYPE_SP,
	QDMA_TXQOS_TYPE_SPWRR7, 
	QDMA_TXQOS_TYPE_SPWRR6, 
	QDMA_TXQOS_TYPE_SPWRR5, 
	QDMA_TXQOS_TYPE_SPWRR4, 
	QDMA_TXQOS_TYPE_SPWRR3, 
	QDMA_TXQOS_TYPE_SPWRR2, 
	QDMA_TXQOS_TYPE_NUMS
} QDMA_TxQosType_t ;

typedef enum {
	QDMA_VCHNL_TXQOS_TYPE_WRR = 0,
	QDMA_VCHNL_TXQOS_TYPE_SP,
	QDMA_VCHNL_TXQOS_TYPE_SPWRR3, 
	QDMA_VCHNL_TXQOS_TYPE_SPWRR2, 
	QDMA_VCHNL_TXQOS_TYPE_NUMS
} QDMA_VirChnlQosType_t ;

typedef enum {
	QDMA_TRTCM_SCALE_1BYTE = 0,
	QDMA_TRTCM_SCALE_2BYTE,
	QDMA_TRTCM_SCALE_4BYTE, 
	QDMA_TRTCM_SCALE_8BYTE,
	QDMA_TRTCM_SCALE_16BYTE,
	QDMA_TRTCM_SCALE_32BYTE,
	QDMA_TRTCM_SCALE_64BYTE,
	QDMA_TRTCM_SCALE_128BYTE,
	QDMA_TRTCM_SCALE_256BYTE,
	QDMA_TRTCM_SCALE_512BYTE,
	QDMA_TRTCM_SCALE_1KBYTE,
	QDMA_TRTCM_SCALE_2KBYTE,
	QDMA_TRTCM_SCALE_4KBYTE,
	QDMA_TRTCM_SCALE_8KBYTE,
	QDMA_TRTCM_SCALE_16KBYTE,
	QDMA_TRTCM_SCALE_32KBYTE,
	QDMA_TRTCM_SCALE_MAX_ITEMS
} QDMA_TrtcmScale_t ;

typedef enum {
	QDMA_TRTCM_PARAM_CIR = 0,
	QDMA_TRTCM_PARAM_CBS,
	QDMA_TRTCM_PARAM_PIR,
	QDMA_TRTCM_PARAM_PBS
} QDMA_TrtcmParamType_t ;

typedef enum {
	QDMA_EPON_REPORT_WO_THRESHOLD = 0,
	QDMA_EPON_REPORT_ONE_THRESHOLD,
	QDMA_EPON_REPORT_TWO_THRESHOLD,
	QDMA_EPON_REPORT_THREE_THRESHOLD
} QDMA_EponReportMode_t ;

typedef enum {
	QDMA_TXQUEUE_SCALE_2_DSCP = 0,
	QDMA_TXQUEUE_SCALE_4_DSCP,
	QDMA_TXQUEUE_SCALE_8_DSCP,
	QDMA_TXQUEUE_SCALE_16_DSCP,
	QDMA_TXQUEUE_SCALE_ITEMS
} QDMA_TxQueueThresholdScale_t ;

typedef enum {
	PSE_PCP_TYPE_CDM_TX = 0,
	PSE_PCP_TYPE_CDM_RX, 
	PSE_PCP_TYPE_GDM_RX
} PSE_PcpType_t ;

typedef enum {
	PSE_PCP_MODE_DISABLE = 0,
	PSE_PCP_MODE_8B0D = 1,
	PSE_PCP_MODE_7B1D = 2, 
	PSE_PCP_MODE_6B2D = 4,
	PSE_PCP_MODE_5B3D = 8
} PSE_PcpMode_t ;

typedef enum {
	QDMA_RX_RATE_LIMIT_BY_BYTE = 0,
	QDMA_RX_RATE_LIMIT_BY_PACKET,
} QDMA_RxPktMode_t ;

typedef enum {
	QDMA_Rx_Rate_Limit_PIR = 0,
	QDMA_Rx_Rate_Limit_PBS
} QDMA_RxRateLimitType_t ;

typedef enum {
	QDMA_Tx_Rate_Limit_CIR = 0,
	QDMA_Tx_Rate_Limit_CBS,
	QDMA_Tx_Rate_Limit_PIR,
	QDMA_Tx_Rate_Limit_PBS
} QDMA_TxRateLimitType_t ;

typedef enum {
	QDMA_TX_RATE_METER_TIME_DIVISOR_1 = 0,
	QDMA_TX_RATE_METER_TIME_DIVISOR_2,
	QDMA_TX_RATE_METER_TIME_DIVISOR_4,
	QDMA_TX_RATE_METER_TIME_DIVISOR_8,
	QDMA_TX_RATE_METER_TIME_DIVISOR_ITEMS
} QDMA_TxRateMeterTimeDivisor_t ;

typedef enum {
	QDMA_DYNCNGST_DEI_THRH_SCALE_1_2 = 0,
	QDMA_DYNCNGST_DEI_THRH_SCALE_1_4,
	QDMA_DYNCNGST_DEI_THRH_SCALE_1_8,
	QDMA_DYNCNGST_DEI_THRH_SCALE_1_16,
	QDMA_DYNCNGST_DEI_THRH_SCALE_ITEMS
} QDMA_TxDynCngstDeiThrhScale_t ;

typedef enum {
	QDMA_QUEUE_NONBLOCKING = 0 ,
	QDMA_QUEUE_NORMAL
} QDMA_TxQCngstQueueMode_t ;

typedef enum {
	QDMA_CHANNEL_NONBLOCKING = 0 ,
	QDMA_CHANNEL_NORMAL
} QDMA_TxQCngstChannelMode_t ;

typedef enum {
	QDMA_DBG_CNTR_SRC_CPU_TX = 0,
	QDMA_DBG_CNTR_SRC_FWD_TX,
	QDMA_DBG_CNTR_SRC_CPU_RX,
	QDMA_DBG_CNTR_SRC_FWD_RX,
	QDMA_DBG_CNTR_SRC_ITEMS
} QDMA_DbgCntrSrc_t ;

typedef enum {
	QDMA_TXCNGST_DYNAMIC_NORMAL = 0,
	QDMA_TXCNGST_DYNAMIC_PEAKRATE_MARGIN,
	QDMA_TXCNGST_STATIC,
} QDMA_TxCngstMode_t ;

typedef enum {
	QDMA_TXCNGST_PEEKRATE_MARGIN_0 = 0,
	QDMA_TXCNGST_PEEKRATE_MARGIN_25,
	QDMA_TXCNGST_PEEKRATE_MARGIN_50,
	QDMA_TXCNGST_PEEKRATE_MARGIN_100,
} QDMA_PeekRateMargin_t ;


typedef enum {
	QDMA_DBG_MEM_XS_MEMSEL_LOCAL_DSCP_INFO = 0,
	QDMA_DBG_MEM_XS_MEMSEL_LOCAL_QUEUE_INFO,
	QDMA_DBG_MEM_XS_MEMSEL_QOS_WEIGHT_COUNTER,
	QDMA_DBG_MEM_XS_MEMSEL_LOCAL_DMA_INDEX,
	QDMA_DBG_MEM_XS_MEMSEL_BUFFER_MONITOR,
	QDMA_DBG_MEM_XS_MEMSEL_RATELIMIT_PARAM,
	QDMA_DBG_MEM_XS_MEMSEL_VCHNL_QOS_WEIGHT,
	QDMA_DBG_MEM_XS_MEMSEL_MAX,
} QDMA_DbgMemXsMemSel_t ;

typedef enum {
	MAC_TYPE_ETHER = 0,
	MAC_TYPE_XPON,
	MAC_TYPE_PTM,
	MAC_TYPE_ATM,
} MAC_Type_t ;

typedef enum {
	QDMA_VIRTUAL_CHANNEL_16Queue = 0,
	QDMA_VIRTUAL_CHANNEL_32Queue,
} QDMA_VirChnlMapMode_t ;

typedef enum {
	QDMA_DBG_CNTR_CHNL_TXCPU = 0,
	QDMA_DBG_CNTR_CHNL_TXFWD1,
	QDMA_DBG_CNTR_CHNL_TXFWD2,
	QDMA_DBG_CNTR_QUEUE,
	QDMA_DBG_CNTR_MAX,
} QDMA_DbgCntrChnlGroup_t ;
/* Warning: same sequence with function array 'qdma_operation' */
typedef enum {
	/* init */
	QDMA_FUNCTION_INIT = 0,
	QDMA_FUNCTION_DEINIT ,
	QDMA_FUNCTION_TX_DMA_MODE ,
	QDMA_FUNCTION_RX_DMA_MODE ,
	QDMA_FUNCTION_LOOPBACK_MODE ,
	QDMA_FUNCTION_REGISTER ,
	QDMA_FUNCTION_UNREGISTER ,
	QDMA_FUNCTION_ENABLE_RXPKT_INT ,
	QDMA_FUNCTION_DISABLE_RXPKT_INT ,
	QDMA_FUNCTION_RECEIVE_PACKETS ,
	QDMA_FUNCTION_TRANSMIT_PACKETS ,
	QDMA_FUNCTION_SET_TX_QOS_WEIGHT ,
	QDMA_FUNCTION_GET_TX_QOS_WEIGHT ,
	QDMA_FUNCTION_SET_TX_QOS ,
	QDMA_FUNCTION_GET_TX_QOS ,
	QDMA_FUNCTION_SET_MAC_LIMIT_THRESHOLD ,
	QDMA_FUNCTION_GET_MAC_LIMIT_THRESHOLD ,

	/* other */
	QDMA_FUNCTION_SET_TXBUF_THRESHOLD ,
	QDMA_FUNCTION_GET_TXBUF_THRESHOLD ,
	QDMA_FUNCTION_SET_PREFETCH_MODE ,
	QDMA_FUNCTION_SET_PKTSIZE_OVERHEAD_EN ,
	QDMA_FUNCTION_GET_PKTSIZE_OVERHEAD_EN ,
	QDMA_FUNCTION_SET_PKTSIZE_OVERHEAD_VALUE ,
	QDMA_FUNCTION_GET_PKTSIZE_OVERHEAD_VALUE ,
	QDMA_FUNCTION_SET_LMGR_LOW_THRESHOLD ,
	QDMA_FUNCTION_GET_LMGR_LOW_THRESHOLD ,
	QDMA_FUNCTION_GET_LMGR_STATUS ,

	/* test */
	QDMA_FUNCTION_SET_DEBUG_LEVEL,
	QDMA_FUNCTION_DUMP_DMA_BUSY,
	QDMA_FUNCTION_DUMP_REG_POLLING,
	QDMA_FUNCTION_SET_FORCE_RX_RING1,

	/* tx rate limit */
	QDMA_FUNCTION_SET_TX_DROP_EN,	//default enable
	QDMA_FUNCTION_GET_TX_DROP_EN,	//default enable
	QDMA_FUNCTION_SET_TX_RATEMETER,
	QDMA_FUNCTION_GET_TX_RATEMETER,
	QDMA_FUNCTION_ENABLE_TX_RATELIMIT,
	QDMA_FUNCTION_SET_TX_RATELIMIT_CFG,
	QDMA_FUNCTION_GET_TX_RATELIMIT_CFG,
	QDMA_FUNCTION_SET_TX_RATELIMIT,
	QDMA_FUNCTION_GET_TX_RATELIMIT,
	QDMA_FUNCTION_SET_TX_DBAREPORT,
	QDMA_FUNCTION_GET_TX_DBAREPORT,

	/* rx rate limit */
	QDMA_FUNCTION_SET_RX_PROTECT_EN,	//default enable
	QDMA_FUNCTION_GET_RX_PROTECT_EN,	//default enable
	QDMA_FUNCTION_SET_RX_LOW_THRESHOLD,
	QDMA_FUNCTION_GET_RX_LOW_THRESHOLD,
	QDMA_FUNCTION_SET_RX_RATELIMIT_EN,
	QDMA_FUNCTION_SET_RX_RATELIMIT_PKT_MODE,
	QDMA_FUNCTION_GET_RX_RATELIMIT_CFG,
	QDMA_FUNCTION_SET_RX_RATELIMIT,
	QDMA_FUNCTION_GET_RX_RATELIMIT,

	/* txq cngst */
	QDMA_FUNCTION_SET_TXQ_DEI_DROP_MODE,
	QDMA_FUNCTION_GET_TXQ_DEI_DROP_MODE,
	QDMA_FUNCTION_SET_TXQ_CNGST_MODE,
	QDMA_FUNCTION_GET_TXQ_CNGST_MODE,
	QDMA_FUNCTION_SET_TXQ_DEI_THRH_SCALE,
	QDMA_FUNCTION_GET_TXQ_DEI_THRH_SCALE,
	QDMA_FUNCTION_SET_TXQ_CNGST_AUTO_CONFIG,
	QDMA_FUNCTION_GET_TXQ_CNGST_AUTO_CONFIG,
	QDMA_FUNCTION_SET_TXQ_CNGST_DYNAMIC_THRESHOLD,
	QDMA_FUNCTION_GET_TXQ_CNGST_DYNAMIC_THRESHOLD,
	QDMA_FUNCTION_SET_TXQ_CNGST_TOTAL_THRESHOLD,
	QDMA_FUNCTION_GET_TXQ_CNGST_TOTAL_THRESHOLD,
	QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_THRESHOLD,
	QDMA_FUNCTION_GET_TXQ_CNGST_CHANNEL_THRESHOLD,
	QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_THRESHOLD,
	QDMA_FUNCTION_GET_TXQ_CNGST_QUEUE_THRESHOLD,
	QDMA_FUNCTION_SET_PEEKRATE_PARAMS,
	QDMA_FUNCTION_GET_PEEKRATE_PARAMS,
	QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_NORMAL_THRESHOLD,
	QDMA_FUNCTION_SET_TXQ_STATIC_QUEUE_DEI_THRESHOLD,
	QDMA_FUNCTION_GET_TXQ_CNGST_DYNAMIC_INFO,
	QDMA_FUNCTION_GET_TXQ_CNGST_STATIC_INFO,
	QDMA_FUNCTION_SET_TXQ_CNGST_QUEUE_NONBLOCKING,
	QDMA_FUNCTION_GET_TXQ_CNGST_QUEUE_NONBLOCKING,
	QDMA_FUNCTION_SET_TXQ_CNGST_CHANNEL_NONBLOCKING,
	QDMA_FUNCTION_GET_TXQ_CNGST_CHANNEL_NONBLOCKING,

	/* virtual channel */
	QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_MODE,
	QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_MODE,
	QDMA_FUNCTION_SET_VIRTUAL_CHANNEL_QOS,
	QDMA_FUNCTION_GET_VIRTUAL_CHANNEL_QOS,

	/* dbg cntr */
	QDMA_FUNCTION_SET_DBGCNTR_CHANNEL,
	QDMA_FUNCTION_SET_DBGCNTR_QUEUE,
	QDMA_FUNCTION_CLEAR_DBGCNTR,
	QDMA_FUNCTION_DUMP_DBGCNTR,

	/* dump */
	QDMA_FUNCTION_DUMP_TX_QOS,
	QDMA_FUNCTION_DUMP_VIRTUAL_CHANNEL_QOS,
	QDMA_FUNCTION_DUMP_TX_RATELIMIT,
	QDMA_FUNCTION_DUMP_RX_RATELIMIT,
	QDMA_FUNCTION_DUMP_TX_DBA_REPORT,
	QDMA_FUNCTION_DUMP_TXQ_CNGST,

	QDMA_FUNCTION_CLEAR_CPU_COUNTER,
	QDMA_FUNCTION_DUMP_CPU_COUNTER,
	QDMA_FUNCTION_DUMP_REGISTER_INFO,
	QDMA_FUNCTION_DUMP_DESCRIPTOR_INFO,
	QDMA_FUNCTION_DUMP_IRQ_INFO,
	QDMA_FUNCTION_DUMP_HWFWD_INFO,
	QDMA_FUNCTION_DUMP_INFO_ALL,
	
	/*other function*/
	QDMA_FUNCTION_READ_VIP_INFO,

	QDMA_FUNCTION_MAX_NUM ,
} QDMA_HookFunction_t ;

typedef int (*qdma_callback_recv_packet_t)(void *, uint, struct sk_buff *, uint) ;
typedef int (*qdma_callback_event_handler_t)(QDMA_EventType_t) ;
typedef void (*qdma_callback_int_handler_t)(void) ;
typedef int (*qdma_callback_recycle_packet_t)(struct sk_buff *, uint) ;

typedef struct {
    MAC_Type_t macType ;
    unchar txRecycleThrh ;
	qdma_callback_recv_packet_t		cbRecvPkts ;
#if defined(TCSUPPORT_FTTDP_V2) && defined(QDMA_LAN)
    qdma_callback_recv_packet_t		cbRecvPktsPtmLan ;
#endif
	qdma_callback_event_handler_t	cbEventHandler ;
	qdma_callback_recycle_packet_t  cbRecyclePkts ;
	qdma_callback_int_handler_t		cbGponMacHandler ;
	qdma_callback_int_handler_t		cbEponMacHandler ;
	qdma_callback_int_handler_t		cbXponPhyHandler ;
} QDMA_InitCfg_t ;

typedef struct {
	QDMA_Mode_t	mode ;
	unchar		chnThreshold ;
	unchar		totalThreshold ;
} QDMA_TxBufCtrl_T ;

typedef struct {
	unchar					channel ;
	QDMA_TxQosType_t		qosType ;
	struct {
		unchar				weight ;		//0 for don't care
	} queue[CONFIG_QDMA_QUEUE] ;
} QDMA_TxQosScheduler_T ;

typedef struct {
	unchar	channel ;
	ushort	cir ;
	ushort	cbs ;
	ushort	pir ;
	ushort	pbs ;
} QDMA_TrtcmParam_T ;

typedef struct {
	QDMA_TxQueueThresholdScale_t	maxScale ;
	QDMA_TxQueueThresholdScale_t	minScale ;
} QDMA_TxQueueCongestScale_T ;

typedef struct {
	unchar deiScale;
	struct {
		unchar			staticDeiThreshold ;
		unchar			staticNormalThreshold ;
	} queue[CONFIG_QDMA_QUEUE] ;
} QDMA_TxQueueCongestCfg_T ;

typedef struct {
	uint	normalThrh[CONFIG_QDMA_QUEUE] ;	// unit is kByte.
} QDMA_TxQStaticNormalCfg_T ;

typedef struct {
	uint	deiThrh[CONFIG_QDMA_QUEUE] ;	// unit is kByte.
} QDMA_TxQStaticDeiCfg_T ;

typedef struct {
	unchar				cntrIdx ;
	unchar				cntrEn ;
	QDMA_DbgCntrSrc_t	cntrSrc ;
	unchar				isChnlAll ;
	unchar				isQueueAll ;
	unchar				isDscpRingAll ;
	unchar				chnlIdx ;
	unchar				queueIdx ;
	unchar				dscpRingIdx ;
	uint				cntrVal ;
} QDMA_DBG_CNTR_T ;

typedef struct {
	ushort				txRateMeterTimeSlice ;
	QDMA_TxRateMeterTimeDivisor_t	txRateMeterTimeDivisor ;
} QDMA_TxRateMeter_T ;

typedef struct {
	ushort				txRateLimitUnit ; //unit is kbps, scope is (1~65)
	QDMA_TrtcmScale_t	txRateLimitBucketScale ;
} QDMA_TxRateLimitCfg_T ;

typedef struct {
	unchar				chnlIdx ;
	unchar				chnlRateLimitEn ;
	uint				rateLimitValue ; //unit is kbps
} QDMA_TxRateLimitSet_T ;

typedef struct {
	unchar				chnlIdx ;
	unchar				chnlRateLimitEn ;
	uint				rateLimitValue ; //unit is kbps
	uint				pbsParamValue ;	
} QDMA_TxRateLimitGet_T ;

typedef struct {
	unchar				tsid ;
	ushort				cirParamValue ; //The unit of CIR, PIR is 64Kbps
	ushort				cbsParamValue ; //The default trtcm scale of CBS,PBS is 128 Byte
	ushort				pirParamValue ; 
	ushort				pbsParamValue ;
} QDMA_TxQueueTrtcm_T ;

typedef struct {
	unchar 			channel ;
	unchar			queue ;
	unchar			thrIdx ;
	ushort			value ;
} QDMA_EponQueueThreshold_T ;

typedef struct {
	unchar				channel ;
	uint				cirParamValue ; // unit  is Kbps
	uint				cbsParamValue ; // unit is Byte
	uint				pirParamValue ; // unit is Kbps
	uint				pbsParamValue ; // unit is Byte
} QDMA_TxDbaReport_T ;

typedef struct {
	uint				rxRing0LowThrh ;
	uint				rxRing1LowThrh ;
} QDMA_RxLowThreshold_T ;

typedef struct {
	unchar				rxRateLimitEn ;
	QDMA_RxPktMode_t	rxRateLimitPktMode ;
	ushort				rxRateLimitUnit ; //unit is kbps, scope is (1~65), or pps, scope is (16~1000)
	QDMA_TrtcmScale_t	rxRateLimitBucketScale ;
} QDMA_RxRateLimitCfg_T ;

typedef struct {
	unchar				ringIdx ;
	uint				rateLimitValue ; //unit is kbps or pps
} QDMA_RxRateLimitSet_T ;

typedef struct {
	unchar				ringIdx ;
	uint				rateLimitValue ; //unit is kbps or pps
	uint				pbsParamValue ;	
} QDMA_RxRateLimitGet_T ;

typedef struct {
	unchar				cngstDropEn ;
	unchar				cngstDeiDropEn ;
	unchar				dynCngstEn ;
	unchar				dynCngstMaxThrhTx1En ;
	unchar				dynCngstMinThrhTx1En ;
	unchar				dynCngstMaxThrhTx0En ;
	unchar				dynCngstMinThrhTx0En ;
	unchar				dynCngstModeConfigTrigEn ;
	unchar				dynCngstModePacketTrigEn ;
	unchar				dynCngstModeTimeTrigEn ;
	ushort				dynCngstTicksel ;
	QDMA_TxDynCngstDeiThrhScale_t	dynCngstDeiThrhScale ;
} QDMA_TxQCngstCfg_T ;

typedef struct {
	uint				dynCngstTotalMaxThrh ;	// unit is kByte.
	uint				dynCngstTotalMinThrh ;	// unit is kByte.
} QDMA_TxQDynCngstTotalThrh_T ;

typedef struct {
	uint				dynCngstChnlMaxThrh ;	// unit is kByte.
	uint				dynCngstChnlMinThrh ;	// unit is kByte.
} QDMA_TxQDynCngstChnlThrh_T ;

typedef struct {
	uint				dynCngstQueueMaxThrh ;	// unit is kByte.
	uint				dynCngstQueueMinThrh ;	// unit is kByte.
} QDMA_TxQDynCngstQueueThrh_T ;

typedef struct {
	ushort				dynCngstTotalMaxThrh ;
	ushort				dynCngstTotalMinThrh ;
	ushort				dynCngstChnlMaxThrh ;
	ushort				dynCngstChnlMinThrh ;
	ushort				dynCngstQueueMaxThrh ;
	ushort				dynCngstQueueMinThrh ;
} QDMA_TxQDynCngstThrh_T ;


typedef struct {
	unchar				queueIdx ;
	ushort				staticDeiThrh ;
	ushort				staticNormalThrh ;
} QDMA_TxQStaticCngstThrh_T ;

typedef struct {
	QDMA_TxCngstMode_t				txCngstMode ;
	QDMA_PeekRateMargin_t			peekRateMargin ;	// only used in peek rate mode
	uint							peekRateDuration ;	// only used in peek rate mode, unit is ms
} QDMA_txCngstCfg_t ;

typedef struct {
	QDMA_Mode_t						peekRateEn ;
	QDMA_PeekRateMargin_t			peekRateMargin ;	// only used in peek rate mode
	uint							peekRateDuration ;	//only used in peek rate mode, unit is ms
} QDMA_PeekRateCfg_t ;

typedef struct {	
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr;
	QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr;
	QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr;
	QDMA_PeekRateCfg_t *peekrateCfgPtr;
} QDMA_TxQDynamicCngstInfo_T ;

typedef struct {	
	QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr;
	QDMA_TxQStaticNormalCfg_T *normThrhPtr;
	QDMA_TxQStaticDeiCfg_T *deiThrhPtr;
} QDMA_TxQStaticCngstInfo_T ;

typedef struct {	
	QDMA_TxQCngstQueueMode_t queueMode;
	uint queue;
} QDMA_TxQCngstQueueCfg_T ;

typedef struct {	
	QDMA_TxQCngstChannelMode_t channelMode;
	uint channel;
} QDMA_TxQCngstChannelCfg_T ;

typedef struct {	
	QDMA_Mode_t virChnlEn;
	QDMA_VirChnlMapMode_t virChnlMode;
} QDMA_VirtualChannelMode_T ;

typedef struct {
	unchar					phyChnl ;
	QDMA_VirChnlQosType_t	qosType ;
	struct {
		unchar				weight ;		//0 for don't care
	} queue[4] ;
} QDMA_VirtualChannelQoS_T ;

typedef struct {
	QDMA_CbType_t type;
	void *pCbFun;
} QDMA_RegisterCallBack_T ;

typedef struct {
	struct sk_buff *skb;
	uint msg0;
	uint msg1;
	struct port_info *pMacInfo;
} QDMA_Transmit_T ;

typedef struct {
	QDMA_TxQosWeightType_t weightBase;
	QDMA_TxQosWeightScale_t weightScale;
	QDMA_TxQosScheduler_T *pTxQos;
} QDMA_TxQos_T;

typedef struct {
	QDMA_Mode_t prefecthMode;
	QDMA_Mode_t overDragMode;
	uint overDragCnt;
} QDMA_PrefetchMode_T;

typedef struct {
	uint freeLmgr;
	uint usedLmgr;
	uint usedBuf;
} QDMA_LmgrStatus_T;

typedef struct {
	uint dbgLevel;
	uint busyDuration; /* unit is second */
	uint regOffset;
	uint pollingDuration; /* unit is second */
	uint forceEn;
} QDMA_OldProc_T;

typedef struct {
	uint chnlThrh;
	uint queueThrh;
} QDMA_MacLimitThrh_T;

struct ECNT_QDMA_Data {
	QDMA_HookFunction_t function_id;	/* need put at first item */
	int retValue;
	
	union {
		QDMA_InitCfg_t *pInitCfg;
		QDMA_Mode_t mode;
		QDMA_LoopbackMode_t lbMode;
		QDMA_DbgCntrChnlGroup_t cntrSrc;
		int cnt;
		int channel;
		uint threshold;
		
		QDMA_RegisterCallBack_T qdma_reg_cb;
		QDMA_Transmit_T qdma_transmit;
		QDMA_TxQos_T qdma_tx_qos;
		QDMA_TxBufCtrl_T *pQdmaTxBufCtrl;
		QDMA_PrefetchMode_T *qdma_prefetch;
		QDMA_LmgrStatus_T *qdma_lmgr_status;
		QDMA_OldProc_T qdma_old_proc;
		QDMA_MacLimitThrh_T *qdma_mac_limit_thrh;
		
		QDMA_TxRateMeter_T *txRateMeterPtr;
		QDMA_TxRateLimitCfg_T *txRateLimitCfgPtr;
		QDMA_TxRateLimitSet_T *txRateLimitSetPtr;
		QDMA_TxRateLimitGet_T *txRateLimitGetPtr;
		QDMA_TxDbaReport_T *txDbaReportPtr;
		
		QDMA_RxLowThreshold_T *rxLowThresholdPtr;
		QDMA_RxPktMode_t pktMode;
		QDMA_RxRateLimitCfg_T *rxRateLimitCfgPtr;
		QDMA_RxRateLimitSet_T *rxRateLimitSetPtr;
		QDMA_RxRateLimitGet_T *rxRateLimitGetPtr;
		
		QDMA_txCngstCfg_t *pTxCngstCfg;
		QDMA_TxQDynCngstThrh_T *dynCngstThrhPtr;
		QDMA_TxQDynCngstTotalThrh_T *totalThrhPtr;
		QDMA_TxQDynCngstChnlThrh_T *chnlThrhPtr;
		QDMA_TxQDynCngstQueueThrh_T *queueThrhPtr;
		QDMA_PeekRateCfg_t *peekrateCfgPtr;

		QDMA_TxQStaticDeiCfg_T *deiThrhPtr;
		QDMA_TxQStaticNormalCfg_T *normThrhPtr;

		QDMA_TxQDynamicCngstInfo_T *dynCfgPtr;
		QDMA_TxQStaticCngstInfo_T *staticCfgPtr;

		QDMA_VirtualChannelMode_T *virChnlModePtr;
		QDMA_VirtualChannelQoS_T *virChnlQoSPtr;
		QDMA_TxQCngstQueueCfg_T *txqCngstQueueCfgPtr;
		QDMA_TxQCngstChannelCfg_T *txqCngstChannelCfgPtr;
	} qdma_private;
};

/************************************************************************
*                  D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N    D E C L A R A T I O N S
#                  I N L I N E    F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#endif /* _ECNT_HOOK_QDMA_TYPE_H_ */

