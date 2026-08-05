#ifndef _XPON_GLOBAL_PRIVATE_XMCS_FDET_H_
#define _XPON_GLOBAL_PRIVATE_XMCS_FDET_H_

#include "../public/xmcs_fdet.h"
#include "xmcs_const.h"

#define XMCS_EVENT_MAX_ITEMS			32

/* struct definition for fault detection command */
struct XMCS_PonEventStatus_S {
	XPON_EVENT_t event[XMCS_EVENT_MAX_ITEMS] ;
	unsigned int				items ;
};

typedef struct{
    __u32 report_init_O1:1;
    __u32 reserve:31;
}Event_ctrlFlag_t;

#define FDET_MAGIC							0xDB

/* Interface IO Command */
#define FDET_IOG_WAITING_EVENT				_IOR(FDET_MAGIC, 0x01, struct XMCS_PonEventStatus_S *)
#define FDET_IOG_RMMOD_EVENT				_IOR(FDET_MAGIC, 0x02, struct XMCS_PonEventStatus_S *)
#define FDET_IOS_EVENT_CTRL                 _IOW(FDET_MAGIC, 0X03, Event_ctrlFlag_t *)
#define FDET_IOG_EVENT_CTRL                 _IOR(FDET_MAGIC, 0X03, Event_ctrlFlag_t *)

#endif /* _XPON_GLOBAL_PRIVATE_XMCS_FDET_H_ */

