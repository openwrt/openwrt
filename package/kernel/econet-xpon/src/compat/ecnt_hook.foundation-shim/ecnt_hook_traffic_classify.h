/*
 * compat shim for <ecnt_hook/ecnt_hook_traffic_classify.h>
 * Traffic-classify hook -> no-op (forward everything).
 */
#ifndef _COMPAT_ECNT_HOOK_TRAFFIC_CLASSIFY_H_
#define _COMPAT_ECNT_HOOK_TRAFFIC_CLASSIFY_H_
#include "ecnt_hook/ecnt_common.h"

enum {
	E_ECNT_FORWARD = 0,
	E_ECNT_DROP,
};

/* no-op: never drops.  flag arg points at an unsigned char. */
#define ECNT_TRAFFIC_CLASSIFY_HOOK(skb, pflag) \
	do { (void)(skb); *(pflag) = E_ECNT_FORWARD; } while (0)
#endif
