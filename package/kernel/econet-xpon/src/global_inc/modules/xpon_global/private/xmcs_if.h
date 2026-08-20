#ifndef _XPON_GLOBAL_PRIVATE_XMCS_IF_H_
#define _XPON_GLOBAL_PRIVATE_XMCS_IF_H_


#include "../public/xmcs_if.h"
#include "xmcs_const.h"

/* enum definition for EPON/GPON common command */
typedef enum {
	XMCS_IF_WEIGHT_TYPE_PACKET = 0,
	XMCS_IF_WEIGHT_TYPE_BYTE
} XMCSIF_QoSWeightType_t ;

typedef enum {
	XMCS_IF_WEIGHT_SCALE_64B = 0,
	XMCS_IF_WEIGHT_SCALE_16B
} XMCSIF_QoSWeightScale_t ;

typedef enum {
	XMCS_IF_QOS_TYPE_WRR = 0,
	XMCS_IF_QOS_TYPE_SP,
	XMCS_IF_QOS_TYPE_SPWRR7, 
	XMCS_IF_QOS_TYPE_SPWRR6, 
	XMCS_IF_QOS_TYPE_SPWRR5, 
	XMCS_IF_QOS_TYPE_SPWRR4, 
	XMCS_IF_QOS_TYPE_SPWRR3, 
	XMCS_IF_QOS_TYPE_SPWRR2, 
} XMCSIF_QosType_t ;

typedef enum {
	XMCS_IF_CONGESTIOM_SCALE_2 = 0,
	XMCS_IF_CONGESTIOM_SCALE_4,
	XMCS_IF_CONGESTIOM_SCALE_8,
	XMCS_IF_CONGESTIOM_SCALE_16,
} XMCSIF_CongestionScale_t ;

typedef enum {
	XMCS_IF_TRTCM_SCALE_1B = 0,
	XMCS_IF_TRTCM_SCALE_2B,
	XMCS_IF_TRTCM_SCALE_4B, 
	XMCS_IF_TRTCM_SCALE_8B,
	XMCS_IF_TRTCM_SCALE_16B,
	XMCS_IF_TRTCM_SCALE_32B,
	XMCS_IF_TRTCM_SCALE_64B,
	XMCS_IF_TRTCM_SCALE_128B,
	XMCS_IF_TRTCM_SCALE_256B,
	XMCS_IF_TRTCM_SCALE_512B,
	XMCS_IF_TRTCM_SCALE_1K,
	XMCS_IF_TRTCM_SCALE_2K,
	XMCS_IF_TRTCM_SCALE_4K,
	XMCS_IF_TRTCM_SCALE_8K,
	XMCS_IF_TRTCM_SCALE_16K,
	XMCS_IF_TRTCM_SCALE_32K,
	XMCS_IF_TRTCM_SCALE_ITEMS
} XMCSIF_TrtcmScale_t ;

typedef enum {
	XMCS_IF_PCP_TYPE_CDM_TX = 0,
	XMCS_IF_PCP_TYPE_CDM_RX, 
	XMCS_IF_PCP_TYPE_GDM_RX
} XMCSIF_PcpType_t ;

typedef enum {
	XMCS_IF_PCP_MODE_DISABLE = 0,
	XMCS_IF_PCP_MODE_8B0D,
	XMCS_IF_PCP_MODE_7B1D, 
	XMCS_IF_PCP_MODE_6B2D,
	XMCS_IF_PCP_MODE_5B3D
} XMCSIF_PcpMode_t ;

typedef enum {
	XMCS_IF_WAN_DETECT_MODE_AUTO = 0,
	XMCS_IF_WAN_DETECT_MODE_GPON,
	XMCS_IF_WAN_DETECT_MODE_EPON
} XMCSIF_WanDetectionMode_t ;

typedef enum {
	XMCS_IF_WAN_LINK_OFF = 0,
	XMCS_IF_WAN_LINK_GPON,
	XMCS_IF_WAN_LINK_EPON
} XMCSIF_WanLinkStatus_t ;

typedef enum {
	XMCS_IF_ONU_TYPE_UNKNOWN = 0,
	XMCS_IF_ONU_TYPE_SFU,
	XMCS_IF_ONU_TYPE_HGU
} XMCSIF_OnuType_t;

typedef enum {
	XMCS_IF_CLEAR_TYPE_XPON = 0,
} XMCSIF_ClearType_t;

/**************************************************/
/* struct definition for EPON/GPON common command */
/**************************************************/
struct XMCS_DebugLevel_S {
	xPON_DebugMsg_t mask;
	XPON_Mode_t enable;
};

struct XMCS_QoSWeightConfig_S {
	XMCSIF_QoSWeightType_t		weightType ;
	XMCSIF_QoSWeightScale_t		weightScale ;
};

struct XMCS_ChannelQoS_S {
	__u8						channel ;
	XMCSIF_QosType_t			qosType ;
	struct {
		__u8					weight ;
	} queue[XPON_QUEUE_NUMBER];
};

struct XMCS_TxQueueCongestion_S {
	struct {
		XPON_Mode_t				trtcm ;
		XPON_Mode_t				dei ;
		XPON_Mode_t				threshold ;
	} congestMode ;
	struct {
		XMCSIF_CongestionScale_t	max ;
		XMCSIF_CongestionScale_t	min ;
	} scale ;
	struct {
		__u8					green ;
		__u8					yellow ;
	} dropProbability ;
	struct {
		__u8					queueIdx ;
		__u8					greenMax ;
		__u8					greenMin ;
		__u8					yellowMax ;
		__u8					yellowMin ;
	} queueThreshold[XPON_QUEUE_NUMBER] ;
};

struct XMCS_TxTrtcmScale_S {
	XMCSIF_TrtcmScale_t			trtcmScale ;
};

struct XMCS_TxQueueTrtcm_S {
	__u8						tsIdx ;
	__u16						cirValue ;
	__u16						cbsUnit ;
	__u16						pirValue ;
	__u16						pbsUnit ;
};

struct XMCS_TxPCPConfig_S {
	XMCSIF_PcpType_t			pcpType ;
	XMCSIF_PcpMode_t			pcpMode ;
};

struct XMCS_ClearConfig_S  {
	XMCSIF_ClearType_t			clearType ;
};
struct XMCS_WanLinkConfig_S {
	XPON_Mode_t					linkStart ;
	XMCSIF_WanDetectionMode_t	detectMode ;
	XMCSIF_WanLinkStatus_t		linkStatus ;
};

struct XMCS_StormCtrlConfig_S {
	__u32				mask;
	__u32 				threld ;
	__u32 				timer ;
};

#if defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
struct XMCS_QosPolicerCreat_S {
	__u32                policer_id;
	__u32                cir;
};

struct XMCS_QosPolicerDelete_S {
    __u32                policer_id;
};

struct XMCS_OverallRatelimitConfig_S {
    __u32                bandwidth;
};
#endif

#ifdef TCSUPPORT_WAN_GPON

/* struct definition for GPON */
struct XMCS_GemPortAssign_S {
	struct {
		__u16 id ;
		__u16 ani ;
	} gemPort[CONFIG_GPON_MAX_GEMPORT] ;
	__u16 entryNum ;
};

struct XMCS_GemPortCreate_S {
	GPON_GemType_t		gemType ;
	__u16 				gemPortId ;
	__u16 				allocId ;
};

struct XMCS_GemPortLoopback_S {
	__u16 		gemPortId ;
	XPON_Mode_t	loopback ;
};

struct XMCS_GemPortInfo_S {
	struct {
		GPON_GemType_t		gemType ;
		__u16 				gemPortId ;	//gem id
		__u16 				allocId ;	//alloc id
		__u16 				aniIdx ;	//interface index
		XPON_Mode_t 		lbMode ;	//downstream loopback mode
		XPON_Mode_t			enMode ;	//downstream encryption mode
	} info[CONFIG_GPON_MAX_GEMPORT] ;
	__u16 entryNum ; 
} ;

struct XMCS_TcontInfo_S {
	struct {
		__u16 			allocId ;
		__u8 			channel ;
	} info[CONFIG_GPON_MAX_TCONT] ;
	__u16 entryNum ; 
} ;

struct XMCS_TcontCfg_S {
	__u16 			allocId ;	
	__u8 			tcontId ;
	__u8 			tcontPolicy ; 
} ;

struct XMCS_OMCC_Info_S {
    __u16  allocId;
    __u32    gemportid;
} ;

struct XMCS_TcontTrtcmScale {
	XMCSIF_TrtcmScale_t			trtcmScale ;
} ;

struct FH_VLAN_INFO_S {
    __u16  srcVlan;
    __u16  dstVlan;
};

#define XMCS_FH_MAX_VLAN_NUMBER 8

struct XMCS_VLAN_CFG_INFO_S {
	__u8 vlanCount;  
	struct FH_VLAN_INFO_S vlan[XMCS_FH_MAX_VLAN_NUMBER];
} ;

struct XMCS_TcontTrtcm_S {
	struct {
		__u8					channel ;
		__u32					cirValue ;
		__u32					cbsUnit ;
		__u32					pirValue ;
		__u32					pbsUnit ;
	} trtcmParam[CONFIG_GPON_MAX_TCONT] ;
	__u8 entryNum ;
} ;
#endif /* TCSUPPORT_WAN_GPON */

/**********************************************************************************
***********************************************************************************/
#define INTERFACE_MAGIC						(0xDA)

/* Interface IO Command */
#define IF_IOS_QOS_WEIGHT_CONFIG			_IOW(INTERFACE_MAGIC, 0x01, struct XMCS_QoSWeightConfig_S *)
#define IF_IOG_QOS_WEIGHT_CONFIG			_IOR(INTERFACE_MAGIC, 0x01, struct XMCS_QoSWeightConfig_S *)
#define IF_IOS_QOS_SCHEDULER				_IOW(INTERFACE_MAGIC, 0x02, struct XMCS_ChannelQoS_S *)
#define IF_IOG_QOS_SCHEDULER				_IOR(INTERFACE_MAGIC, 0x02, struct XMCS_ChannelQoS_S *)
#define IF_IOS_TRTCM_CONGEST_MODE			_IOW(INTERFACE_MAGIC, 0x03, struct XMCS_TxQueueCongestion_S *)
#define IF_IOS_DEI_CONGEST_MODE				_IOW(INTERFACE_MAGIC, 0x04, struct XMCS_TxQueueCongestion_S *)
#define IF_IOS_THRESHOLD_CONGEST_MODE		_IOW(INTERFACE_MAGIC, 0x05, struct XMCS_TxQueueCongestion_S *)
#define IF_IOS_CONGEST_SCALE				_IOW(INTERFACE_MAGIC, 0x06, struct XMCS_TxQueueCongestion_S *)
#define IF_IOS_CONGEST_DROP_PROBABILITY		_IOW(INTERFACE_MAGIC, 0x07, struct XMCS_TxQueueCongestion_S *)
#define IF_IOS_CONGEST_THRESHOLD			_IOW(INTERFACE_MAGIC, 0x08, struct XMCS_TxQueueCongestion_S *)
#define IO_IOG_CONGEST_INFO					_IOR(INTERFACE_MAGIC, 0x09, struct XMCS_TxQueueCongestion_S *)
#define IO_IOS_TX_TRTCM_SCALE				_IOW(INTERFACE_MAGIC, 0x0A, struct XMCS_TxTrtcmScale_S *)
#define IO_IOG_TX_TRTCM_SCALE				_IOR(INTERFACE_MAGIC, 0x0A, struct XMCS_TxTrtcmScale_S *)
#define IO_IOS_TX_TRTCM_PARAMS				_IOW(INTERFACE_MAGIC, 0x0B, struct XMCS_TxQueueTrtcm_S *)
#define IO_IOG_TX_TRTCM_PARAMS				_IOR(INTERFACE_MAGIC, 0x0B, struct XMCS_TxQueueTrtcm_S *)
#define IO_IOS_PCP_CONFIG					_IOW(INTERFACE_MAGIC, 0x0C, struct XMCS_TxPCPConfig_S *)
#define IO_IOG_PCP_CONFIG					_IOR(INTERFACE_MAGIC, 0x0C, struct XMCS_TxPCPConfig_S *)
#define IO_IOS_WAN_LINK_START				_IOW(INTERFACE_MAGIC, 0x20, XPON_Mode_t)
#define IO_IOS_WAN_DETECTION_MODE			_IOW(INTERFACE_MAGIC, 0x21, XMCSIF_WanDetectionMode_t)
#define IO_IOG_WAN_LINK_CONFIG				_IOR(INTERFACE_MAGIC, 0x22, struct XMCS_WanLinkConfig_S *)
#define IO_IOG_ONU_TYPE						_IOR(INTERFACE_MAGIC, 0x23, XMCSIF_OnuType_t *) 
#define IO_IOS_WAN_CNT_STATS				_IOW(INTERFACE_MAGIC, 0x24, struct XMCS_WanCntStats_S *)
#define IO_IOG_WAN_CNT_STATS				_IOR(INTERFACE_MAGIC, 0x24, struct XMCS_WanCntStats_S *)
#define IO_IOS_STORM_CTL_CONFIG				_IOW(INTERFACE_MAGIC, 0x25, struct XMCS_StormCtrlConfig_S *)
#define IO_IOG_STORM_CTL_CONFIG				_IOR(INTERFACE_MAGIC, 0x25, struct XMCS_StormCtrlConfig_S *)

#if defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
    #define IO_IOS_QOS_CREAT_POLICER			_IOW(INTERFACE_MAGIC, 0x26, struct XMCS_QosPolicerCreat_S *)
    #define IO_IOS_QOS_DELETE_POLICER			_IOW(INTERFACE_MAGIC, 0x27, struct XMCS_QosPolicerDelete_S *)
    #define IO_IOS_QOS_OVERALL_RATELIMIT_CONFIG	_IOW(INTERFACE_MAGIC, 0x28, struct XMCS_OverallRatelimitConfig_S *)
#endif

#define IO_IOS_DBG_LEVEL					_IOW(INTERFACE_MAGIC, 0x29, struct XMCS_DebugLevel_S *)
#define IO_IOG_DBG_LEVEL                    _IOR(INTERFACE_MAGIC, 0x29, __u16 *)

#ifdef TCSUPPORT_WAN_GPON
	#define IF_IOS_INTERFACE_CREATE			_IOW(INTERFACE_MAGIC, 0x40, __u32)
	#define IF_IOS_INTERFACE_DELETE			_IOW(INTERFACE_MAGIC, 0x41, __u32)
	#define IF_IOS_GEMPORT_ASSIGN			_IOW(INTERFACE_MAGIC, 0x42, struct XMCS_GemPortAssign_S *)
	#define IF_IOS_GEMPORT_UNASSIGN			_IOW(INTERFACE_MAGIC, 0x43, struct XMCS_GemPortAssign_S *)
	#define IF_IOS_GEMPORT_CREATE			_IOW(INTERFACE_MAGIC, 0x44, struct XMCS_GemPortCreate_S *)
	#define IF_IOS_GEMPORT_REMOVE			_IOW(INTERFACE_MAGIC, 0x45, __u32)
	#define IF_IOS_GEMPORT_REMOVE_ALL		_IOW(INTERFACE_MAGIC, 0x46, __u32)
	#define IF_IOS_GEMPORT_LOOPBACK			_IOW(INTERFACE_MAGIC, 0x47, struct XMCS_GemPortLoopback_S *)
	#define IF_IOG_GEMPORT_INFO				_IOR(INTERFACE_MAGIC, 0x48, struct XMCS_GemPortInfo_S *)
	#define IF_IOG_TCONT_INFO				_IOR(INTERFACE_MAGIC, 0x49, struct XMCS_TcontInfo_S *)
	#define IF_IOS_TCONT_INFO_CREATE		_IOW(INTERFACE_MAGIC, 0x4A, struct XMCS_TcontCfg_S *)
	#define IF_IOS_TCONT_INFO_REMOVE		_IOW(INTERFACE_MAGIC, 0x4B, struct XMCS_TcontCfg_S *)
	#define IF_IOG_OMCC_INFO				_IOR(INTERFACE_MAGIC, 0x4C, __u16 *)
    #define IF_IOS_VLAN_CONFIG_SET  		_IOW(INTERFACE_MAGIC, 0x4D, struct XMCS_VLAN_CFG_OPERATE_S *)
    #define IF_IOG_VLAN_INFO_GET  		    _IOW(INTERFACE_MAGIC, 0x4E, struct XMCS_VLAN_CFG_INFO_S *)
#endif /* TCSUPPORT_WAN_GPON */

#ifdef TCSUPPORT_WAN_EPON
	#define IF_IOS_LLID_CREATE				_IOW(INTERFACE_MAGIC, 0x81, struct XMCS_EponLlidCreate_S *)
	#define IF_IOS_LLID_REMOVE				_IOW(INTERFACE_MAGIC, 0x82, __u32)
	#define IF_IOS_LLID_RX_CFG				_IOW(INTERFACE_MAGIC, 0x83, struct XMCS_EponRxConfig_S *)
	#define IF_IOS_LLID_TX_CFG				_IOW(INTERFACE_MAGIC, 0x84, struct XMCS_EponTxConfig_S *)
	#define IF_IOG_LLID_INFO				_IOR(INTERFACE_MAGIC, 0x85, struct XMCS_EponLlidInfo_S *)
#endif /* TCSUPPORT_WAN_EPON */



#endif /* _XPON_GLOBAL_PRIVATE_XMCS_IF_H_ */

