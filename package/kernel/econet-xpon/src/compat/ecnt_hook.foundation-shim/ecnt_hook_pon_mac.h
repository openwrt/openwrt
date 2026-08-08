/*
 * compat shim for <ecnt_hook/ecnt_hook_pon_mac.h>
 * Provides the PHY->MAC event payload struct.
 */
#ifndef _COMPAT_ECNT_HOOK_PON_MAC_H_
#define _COMPAT_ECNT_HOOK_PON_MAC_H_
#include "ecnt_hook/ecnt_common.h"
#include "ecnt_hook/ecnt_hook_pon_phy.h"

struct xpon_mac_hook_data_s {
	int src_module;
	PON_PHY_Event_data_t *pEvent;
	unsigned char sn[8];
};
typedef struct xpon_mac_hook_data_s xpon_mac_hook_data_t;
#endif
