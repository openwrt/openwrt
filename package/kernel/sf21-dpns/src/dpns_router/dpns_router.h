#ifndef _DPNS_L3_H_
#define _DPNS_L3_H_

#include <linux/types.h>
#include <linux/etherdevice.h>
#include <linux/mroute_base.h>
#include <linux/unaligned.h>
#include <net/fib_rules.h>
#include <net/ip6_fib.h>
#include "dpns_common.h"


#define TCAM_BLK_RAM_ID(BLK_ID, RAM_INDEX) ((BLK_ID)*9+(RAM_INDEX))

#define DPNS_NEIGH_UNRESOLVED          0x55
#define DPNS_RESERVED_MAC_INDEX        0
#define DPNS_ROUTER_TBL4_MAX           32 //half of the tcam for v4, and another for v6
#define DPNS_ROUTER_TBL6_MAX           8
#define V4_ADDR_LEN		       4
#define V6_ADDR_LEN		       16
#define PPPOE_WAN_MAX		       4

#define sf_readl(p, reg)	\
	readl((p)->iobase + (reg))

#define sf_writel(p, reg, val)	\
	writel((val), (p)->iobase + (reg))

#define sf_readq(p, reg)	\
	readq((p)->iobase + (reg))

#define sf_writeq(p, reg, val)	\
	writeq((val), (p)->iobase + (reg))

#define sf_update(p, reg, mask, val)		\
	do {	\
		u32 tmp = readl((p)->iobase + (reg));		\
		tmp &= ~(mask);						\
		tmp |= (val);						\
		writel(tmp, (p)->iobase + (reg));			\
	}while(0)


typedef __uint128_t	u128;

struct l3_uc_ipv4_table {
	u32  next_hop_ptr      : 11;
	u32  oport_id          : 5;
	u32  intf_id           : 6;
	u32  rsv1              : 2;
	u32  rsv2              : 4;
	u32  dip               : 32;
	u32  ovid              : 12;
} __packed; //<=72

union l3_uc_ipv4_table_cfg {
	struct l3_uc_ipv4_table table;
	u8 data[9];
};

struct l3_uc_ipv6_table {
	u32  next_hop_ptr      : 11;
	u32  oport_id          : 5;
	u32  intf_id           : 6;
	u128 rsv               : 126;
	u128 dip               : 128;
	u32  ovid              : 12;
} __packed; //<=288

union l3_uc_ipv6_table_cfg {
	struct l3_uc_ipv6_table table;
	u8 data[36];
};

struct pppoe_info {
    struct net_device       *rel_dev;
    u16                      ppp_sid;
    u16			     ovid;
    u8              gw_mac[ETH_ALEN];
    u8                         valid;
    char		     ifname[IFNAMSIZ];
};

struct router_tbl_entry {
	struct list_head node;
	struct net_device *fib_ndev;
	u8 mac[ETH_ALEN];
	u8 addr[sizeof(struct in6_addr)];
	u8 gw_addr[sizeof(struct in6_addr)];
	u8 addr_len;
	u8 prefix_len;
	u8 prio;
	u8 type;
	/* below is hardware needed info */
	u32 next_hop_ptr; // dmac index
	u32 intf_index; // pppoe info index
	u32 ovport;
	u16 ovid;
	/* below is software needed info */
	u8 req_id;
	u8 req_addr;
	u8 flags;
};

struct dpns_fib_event_work {
	struct work_struct work;
	union {
		struct fib6_entry_notifier_info fen6_info;
		struct fib_entry_notifier_info  fen_info;
		struct fib_rule_notifier_info   fr_info;
		struct fib_nh_notifier_info     fnh_info;
	};
	ROUTER_t *priv;
	unsigned long event;
};

struct dpns_netevent_work {
	struct work_struct work;
	struct neighbour   *n;
	ROUTER_t           *priv;
	int                family;
};

struct dpns_l2_event_work {
	struct work_struct work;
	u32                l2_index;
	u8                 port_id;
	ROUTER_t           *priv;
};

void dpns_router_table_add(COMMON_t* priv, struct router_tbl_entry *entry);
void dpns_router_table_del(COMMON_t* priv, struct router_tbl_entry *entry);
void dump_dpns_router_tbl(COMMON_t* priv);

int dpns_router_genl_init(struct dpns_router_priv *priv);
int dpns_router_genl_exit(void);

#endif
