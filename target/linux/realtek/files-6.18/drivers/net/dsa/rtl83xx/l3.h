/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_L3_H
#define _OTTO_L3_H

#include "rtl-otto.h"

#define MAX_HOST_ROUTES		1536

/* An entry in the RTL93XX SoC's ROUTER_MAC tables setting up a termination point
 * for the L3 routing system. Packets arriving and matching an entry in this table
 * will be considered for routing.
 * Mask fields state whether the corresponding data fields matter for matching
 */
struct otto_l3_router_mac {
	bool valid;	/* Valid or not */
	bool p_type;	/* Individual (0) or trunk (1) port */
	bool p_mask;	/* Whether the port type is used */
	u8 p_id;
	u8 p_id_mask;	/* Mask for the port */
	u8 action;	/* Routing action performed: 0: FORWARD, 1: DROP, 2: TRAP2CPU */
			/*   3: COPY2CPU, 4: TRAP2MASTERCPU, 5: COPY2MASTERCPU, 6: HARDDROP */
	u16 vid;
	u16 vid_mask;
	u64 mac;	/* MAC address used as source MAC in the routed packet */
	u64 mac_mask;
};


struct otto_l3_config {
	u64 (*get_egress_mac)(struct otto_l3_ctrl *ctrl, u32 idx);
	void (*host_route_write)(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt);
	void (*get_router_mac)(struct otto_l3_ctrl *ctrl, u32 idx, struct otto_l3_router_mac *m);
	void (*set_router_mac)(struct otto_l3_ctrl *ctrl, u32 idx, struct otto_l3_router_mac *m);
	void (*get_nexthop)(struct otto_l3_ctrl *ctrl, int idx, u16 *dmac_id, u16 *interface);
	void (*set_nexthop)(struct otto_l3_ctrl *ctrl, int idx, u16 dmac_id, u16 interface);
	int (*route_lookup_hw)(struct otto_l3_ctrl *ctrl, struct otto_l3_route *rt);
	void (*route_read)(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt);
	void (*route_write)(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt);
};

struct otto_l3_ctrl {
	const struct otto_l3_config *cfg;
	struct device *dev;
	struct rtl838x_switch_priv *priv;
	struct notifier_block fib_nb;
	struct notifier_block ne_nb;
	struct rhltable routes;
	unsigned long host_route_use_bm[MAX_HOST_ROUTES / 32];
};

struct otto_l3_route_attr {
	bool valid;
	bool hit;
	bool ttl_dec;
	bool ttl_check;
	bool dst_null;
	bool qos_as;
	u8 qos_prio;
	u8 type;
	u8 action;
};

struct otto_l3_nexthop {
	u16 id;		/* ID: L3_NEXT_HOP table-index or route-index set in L2_NEXT_HOP */
	u32 dev_id;
	u16 port;
	u16 vid;	/* VLAN-ID for L2 table entry (saved from L2-UC entry) */
	u16 rvid;	/* Relay VID/FID for the L2 table entry */
	u64 mac;	/* The MAC address of the entry in the L2_NEXT_HOP table */
	u16 mac_id;
	u16 l2_id;	/* Index of this next hop forwarding entry in L2 FIB table */
	u64 gw;		/* The gateway MAC address packets are forwarded to */
	int if_id;	/* Interface (into L3_EGR_INTF_IDX) */
};

struct otto_l3_route {
	u32 gw_ip;			/* IP of the route's gateway */
	u32 dst_ip;			/* IP of the destination net */
	struct in6_addr dst_ip6;
	int prefix_len;			/* Network prefix len of the destination net */
	bool is_host_route;
	int id;				/* ID number of this route */
	struct rhlist_head linkage;
	u16 switch_mac_id;		/* Index into switch's own MACs, RTL839X only */
	struct otto_l3_nexthop nh;
	struct pie_rule pr;
	struct otto_l3_route_attr attr;
};

static const struct rhashtable_params otto_l3_route_ht_params = {
	.key_len     = sizeof(u32),
	.key_offset  = offsetof(struct otto_l3_route, gw_ip),
	.head_offset = offsetof(struct otto_l3_route, linkage),
};

int otto_l3_probe(struct device *dev, struct rtl838x_switch_priv *priv);
void otto_l3_remove(struct rtl838x_switch_priv *priv);

#endif /* _OTTO_L3_H */
