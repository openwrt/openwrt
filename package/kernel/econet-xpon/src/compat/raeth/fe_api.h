/*
 * compat shim for the raeth forwarding-engine API ("../../../raeth/fe_api.h").
 * The driver only uses the sw-NAT hook pointers, which live in foe_hook.h.
 */
#ifndef _COMPAT_RAETH_FE_API_H_
#define _COMPAT_RAETH_FE_API_H_
#include <linux/foe_hook.h>
#endif
