/*
 * compat shim: shared base for the ECNT hook framework headers.
 *
 * The vendor tclinux_phoenix SDK implements a generic "ecnt_hook" dispatch
 * table (maintype/subtype -> registered hookfn).  Per the port plan we do NOT
 * reimplement the dispatch table: __ECNT_HOOK() becomes a no-op that returns
 * success, and ecnt_register_hook/unregister_hook are no-ops.  This keeps the
 * driver's call sites + struct layouts intact so the rest of the code compiles
 * while the real wiring (to econet_eth / a thin local dispatcher) is a later
 * step.  TODO: wire selected hooks (PHY->MAC event, FhNet DS vlan) live
 *
 * All ecnt_hook_*.h shims include this single file so any one of them pulls in
 * the full set of types/enums the driver references.
 */
#ifndef _COMPAT_ECNT_HOOK_COMMON_H_
#define _COMPAT_ECNT_HOOK_COMMON_H_

#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>

/* ---- hook return codes ---- */
#define ECNT_HOOK_ERROR			(-1)
#define ECNT_HOOK_PROCESS_SUCESS	(0)	/* sic: vendor spelling */
#define ECNT_REGISTER_SUCCESS		(0)
#define ECNT_RETURN			(0)
#define ECNT_CONTINUE			(0)
#define ECNT_RETURN_DROP		(1)

/* ---- maintype / subtype identifiers (values are nominal) ---- */
enum ecnt_hook_maintype {
	ECNT_SMUX = 1,
	ECNT_XPON_MAC,
	ECNT_XPON_PHY,
	ECNT_NET_CORE_DEV,
	ECNT_QDMA,
};

enum ecnt_hook_subtype {
	ECNT_SMUX_API = 1,
	ECNT_XPON_MAC_HOOK,
	ECNT_XPON_PHY_API,
	ECNT_DEV_QUE_XMIT,
	ECNT_QDMA_SET_QOS_FLAG,
	ECNT_QDMA_GREEN_DROP_CTRL_HOOK,
};

/* ---- source-module ids carried in xpon_mac_hook_data_s.src_module ---- */
enum xpon_src_module {
	XPON_MAC_MODULE = 0,
	XPON_PHY_MODULE,
	XPON_SN_SET,
};

/* ---- generic hook payload base ---- */
struct ecnt_data {
	int dummy;
};

/* a registered hook entry */
struct ecnt_hook_ops {
	const char *name;
	int  is_execute;
	int  (*hookfn)(struct ecnt_data *data);
	int  maintype;
	int  subtype;
	int  priority;
};

static inline int ecnt_register_hook(struct ecnt_hook_ops *ops)
{
	(void)ops;
	return ECNT_REGISTER_SUCCESS;
}
static inline int ecnt_unregister_hook(struct ecnt_hook_ops *ops)
{
	(void)ops;
	return ECNT_REGISTER_SUCCESS;
}

/*
 * Dispatch macro.  No-op for the compile milestone: returns success so the
 * `if (ECNT_HOOK_ERROR == __ECNT_HOOK(...))` panic guards are never taken.
 * The data argument is still type-checked at the cast site by the caller.
 */
#define __ECNT_HOOK(maintype, subtype, data) \
	({ (void)(data); ECNT_HOOK_PROCESS_SUCESS; })

#endif /* _COMPAT_ECNT_HOOK_COMMON_H_ */
