#ifndef _XPON_GLOBAL_PUBLIC_XMCS_FDET_H_
#define _XPON_GLOBAL_PUBLIC_XMCS_FDET_H_

#include "xmcs_const.h"

/* enum definition for fault detection command */
typedef enum {
	XMCS_EVENT_TYPE_GPON = 1,
	XMCS_EVENT_TYPE_EPON
} XMCS_EventType_t ;

typedef struct {
	XMCS_EventType_t	type ;
	unsigned int		id ;
	unsigned int		value ;
} XPON_EVENT_t;

typedef enum {
	XMCS_EVENT_GPON_LOS = 0,
	XMCS_EVENT_GPON_PHY_READY,
	XMCS_EVNET_GPON_DEACTIVATE,
	XMCS_EVENT_GPON_ACTIVATE,
	XMCS_EVENT_GPON_TCONT_ALLOCED,
	XMCS_EVENT_GPON_MODE_CHANGE,
	XMCS_EVENT_GPON_PHY_TRANS_ALARM,
	XMCS_EVENT_GPON_MAX_ID, /* no use, just for backward compatiblity*/
	XMCS_EVENT_GPON_DISABLE,
	XMCS_EVENT_GPON_ENABLE,
	XMCS_EVENT_GPON_STATE_CHANGE,

    /* all new events must be added above this line! */
    XMCS_EVENT_GPON_MAX
} XMCS_GponEventId_t ;

typedef enum {
	XMCS_EVENT_EPON_LOS = 0,
	XMCS_EVENT_EPON_PHY_READY,
	XMCS_EVENT_EPON_REGISTER,
	XMCS_EVENT_EPON_DEREGISTER,
	XMCS_EVENT_EPON_DENIED,
	XMCS_EVENT_EPON_MODE_CHANGE,
	XMCS_EVENT_EPON_PHY_TRANS_ALARM,
	XMCS_EVENT_EPON_TX_POWER_OFF,	

    /* all new events must be added above this line! */
	XMCS_EVENT_EPON_MAX_ID
} XMCS_EponEventId_t ;

/*****************************************************************
*****************************************************************/
/* It's used for XMCS_EVENT_GPON_PHY_TRANS_ALARM and XMCS_EVENT_EPON_PHY_TRANS_ALARM */
typedef enum {
	XMCS_PHY_ALARM_TX_POWER_HIGH	= (1<<0),
	XMCS_PHY_ALARM_TX_POWER_LOW		= (1<<1),
	XMCS_PHY_ALARM_TX_CURRENT_HIGH	= (1<<2),
	XMCS_PHY_ALARM_TX_CURRENT_LOW	= (1<<3),
	XMCS_PHY_ALARM_RX_POWER_HIGH	= (1<<4),
	XMCS_PHY_ALARM_RX_POWER_LOW		= (1<<5),
} XMCSPHY_PhyTransAlarm_t ;

#endif /* _XPON_GLOBAL_PUBLIC_XMCS_FDET_H_ */


