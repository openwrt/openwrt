#ifndef _XPON_GLOBAL_PUBLIC_XMCS_PHY_H_
#define _XPON_GLOBAL_PUBLIC_XMCS_PHY_H_

#include "xmcs_const.h"

typedef struct XMCS_PhyTransParams_S {
	__u16			temperature ;
	__u16			voltage ;
	__u16			txCurrent ;
	__u16			txPower ;
	__u16			rxPower ;
} PHY_PARAMS_t;

typedef struct XMCS_PhyTxRxFecStatus_S{
	__u32 rx_status;
	__u32 tx_status;
} PHY_FECSTATUS_t;

#endif /* _XPON_GLOBAL_PUBLIC_XMCS_PHY_H_  */
