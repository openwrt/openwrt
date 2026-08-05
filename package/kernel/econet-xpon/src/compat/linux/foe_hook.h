/*
 * compat shim for <linux/foe_hook.h>  (MTK hwNAT / PPE flow-offload hooks)
 *
 * hwNAT is OFF for the GPON compile milestone.  The driver references the
 * MTK sw-NAT hook function pointers + the per-packet "port_info" descriptor.
 * We declare the pointers (defined NULL in sdk_stubs.c) so the `!= NULL`
 * guards short-circuit, and define port_info with the fields the GPON TX path
 * touches.  TODO EN7528: wire to econet_eth QDMA offload when enabled
 */
#ifndef _COMPAT_LINUX_FOE_HOOK_H_
#define _COMPAT_LINUX_FOE_HOOK_H_

#include <linux/types.h>
#include <linux/skbuff.h>

/* flow magics */
enum {
	FOE_MAGIC_FROM_CPU = 0,
	FOE_MAGIC_PPE,
	FOE_MAGIC_GPON,
	FOE_MAGIC_EPON,
};

/* per-packet offload descriptor carried alongside the skb */
struct port_info {
	struct net_device *dev;
	u16 stag;
	u16 channel;
	u16 gem;
	u8  tse;
	u8  tsid;
	u8  txq;
	u8  magic;
};

/* MTK sw-NAT hook function pointers (NULL when hwNAT disabled) */
extern int  (*ra_sw_nat_hook_rx)(struct sk_buff *skb);
extern int  (*ra_sw_nat_hook_tx)(struct sk_buff *skb, struct port_info *info, int magic);
extern int  (*ra_sw_nat_hook_magic)(struct sk_buff *skb, int magic);
extern void (*ra_sw_nat_hook_clean_table)(void);
extern void (*ra_sw_nat_hook_drop_packet)(struct sk_buff *skb);
extern void (*ra_sw_nat_hook_rxinfo)(struct sk_buff *skb, struct port_info *info);
extern void (*ra_sw_nat_hook_rx_set_l2lu)(struct sk_buff *skb);
extern void (*ra_sw_nat_hook_xfer)(struct sk_buff *skb);
extern void (*restore_offload_info_hook)(struct sk_buff *skb, struct port_info *info, int magic);

#endif /* _COMPAT_LINUX_FOE_HOOK_H_ */
