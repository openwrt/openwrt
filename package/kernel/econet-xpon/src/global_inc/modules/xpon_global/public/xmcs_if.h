#ifndef _XPON_GLOBAL_PUBLIC_XMCS_IF_H_
#define _XPON_GLOBAL_PUBLIC_XMCS_IF_H_

#include "xmcs_const.h"

/**************************************************/
/* struct definition for EPON/GPON common command */
/**************************************************/
typedef struct XMCS_WanCntStats_S {
	__u32							mask;
	__u32							txFrameCnt;
	__u32							txFrameLen;
	__u32							txDropCnt;
	__u32							txBroadcastCnt;
	__u32							txMulticastCnt;
	__u32							txLess64Cnt;
	__u32							txMore1518Cnt;
	__u32							tx64Cnt;
	__u32							tx65To127Cnt;
	__u32							tx128To255Cnt;
	__u32							tx256To511Cnt;
	__u32							tx512To1023Cnt;
	__u32							tx1024To1518Cnt;
	
	__u32							rxFrameCnt;
	__u32							rxFrameLen;
	__u32							rxDropCnt;
	__u32							rxBroadcastCnt;
	__u32							rxMulticastCnt;
	__u32							rxCrcCnt;
	__u32							rxFragFameCnt;
	__u32							rxJabberFameCnt;
	__u32							rxLess64Cnt;
	__u32							rxMore1518Cnt;
	__u32							rx64Cnt;
	__u32							rx65To127Cnt;
	__u32							rx128To255Cnt;
	__u32							rx256To511Cnt;
	__u32							rx512To1023Cnt;
	__u32							rx1024To1518Cnt;

	__u32							rxHecErrorCnt;
	__u32							rxFecErrorCnt;
}WAN_STATISTIC_t;


typedef enum {
	GPON_UNICAST_GEM = 0,
	GPON_MULTICAST_GEM,
} GPON_GemType_t ;

#ifdef TCSUPPORT_WAN_EPON
/* enum definition for EPON */
typedef enum {
	EPON_RX_FORWARDING = 0,
	EPON_RX_DISCARD,
	EPON_RX_LOOPBACK
} EPON_RxMode_t ;

typedef enum {
	EPON_TX_FORWARDING = 0,
	EPON_TX_DISCARD,
} EPON_TxMode_t ;

/* struct definition for EPON */
struct XMCS_EponLlidCreate_S {
	__u8 		idx ;
	__u16		llid ;
} ;

struct XMCS_EponRxConfig_S {
	__u8 			idx ;
	EPON_RxMode_t	rxMode ;
} ;

struct XMCS_EponTxConfig_S {
	__u8			idx ;
	EPON_TxMode_t	txMode ;
} ;

struct XMCS_EponLlidInfo_S {
	struct {
		__u8 				idx ;
		__u16 				llid ;
		__u8				channel ;
		EPON_RxMode_t		rxMode ;
		EPON_TxMode_t		txMode ;
	} info[CONFIG_EPON_MAX_LLID] ;
	__u16 entryNum ; 
} ;
#endif /* TCSUPPORT_WAN_EPON */

#endif /* _XPON_GLOBAL_PUBLIC_XMCS_IF_H_ */

