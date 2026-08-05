#ifndef _XPON_GLOBAL_PUBLIC_XMCS_GPON_H_
#define _XPON_GLOBAL_PUBLIC_XMCS_GPON_H_

#include "xmcs_const.h"

typedef enum {
	GPON_SW = 0,
	GPON_HW,
	GPON_SW_HW,
} GPON_SW_HW_SELECT_T, GPON_SW_HW_SELECT_t ;

typedef struct XMCS_GponOnuInfo_S {
    __u8  onuId ;
    __u8  state ;
    __u8  sn[GPON_SN_LENS] ;
    __u8  PasswdLength;
    __u8  hexFlag;
    __u8  passwd[GPON_PASSWD_LENS] ;
    __u8  keyIdx ;
    __u8  key[GPON_ENCRYPT_KEY_LENS] ;
    __u32    actTo1Timer ;
    __u32    actTo2Timer ;
    __u16  omcc ;
    __u8  EmergencyState ;
} GPON_ONU_INFO_t;

typedef struct XMCS_GponGemCounter_S {
    __u16 gemPortId ;
    __u32 rxGemFrameH ;
    __u32 rxGemFrameL ;
    __u32 rxGemPayloadH ;
    __u32 rxGemPayloadL ;
    __u32 txGemFrameH ;
    __u32 txGemFrameL ;  
    __u32 txGemPayloadH ;    
    __u32 txGemPayloadL ;
} GEM_STATISTIC_t;

#endif /* _XPON_GLOBAL_PUBLIC_XMCS_GPON_H_ */

