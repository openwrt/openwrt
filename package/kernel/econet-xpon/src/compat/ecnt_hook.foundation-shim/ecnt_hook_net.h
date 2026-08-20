/* compat shim for <ecnt_hook/ecnt_hook_net.h> */
#ifndef _COMPAT_ECNT_HOOK_NET_H_
#define _COMPAT_ECNT_HOOK_NET_H_
#include "ecnt_hook/ecnt_common.h"

/* payload for ECNT_NET_CORE_DEV / ECNT_DEV_QUE_XMIT */
struct net_data_s {
	struct sk_buff **pskb;
};
#endif
