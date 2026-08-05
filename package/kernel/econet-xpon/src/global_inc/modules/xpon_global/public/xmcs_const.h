#ifndef _XPON_GLOBAL_PUBLIC_XMCS_CONST_H_
#define _XPON_GLOBAL_PUBLIC_XMCS_CONST_H_

#include <linux/types.h>
    
#ifdef __KERNEL__
    #include <linux/ioctl.h>
#else
    #include <sys/ioctl.h>
#endif

#define GPON_SN_LENS            (8)
#define GPON_PASSWD_LENS        (10)
#define GPON_ENCRYPT_KEY_LENS   (24)
#define GPON_MAX_GEM_ID         (4096)
#define GPON_UNASSIGN_ALLOC_ID  (0xFF)
#define GPON_MAX_ALLOC_ID       (4096)


/* add or delate this enum, must change file 
apps\private\xpon\ponmgr\inc\core\cmgr\cmgr_enum.h at the same time!!!! */
typedef enum {
    XPON_DISABLE = 0,
    XPON_ENABLE,
    XPON_POWER_DOWN,
    XPON_OTHER
} XPON_Mode_t;

typedef enum {
    GPON_STATE_O1 = 1, 
    GPON_STATE_O2, 
    GPON_STATE_O3, 
    GPON_STATE_O4, 
    GPON_STATE_O5, 
    GPON_STATE_O6, 
    GPON_STATE_O7 
} ENUM_GponState_t;


#endif /*_XPON_GLOBAL_PUBLIC_XMCS_CONST_H_ */
