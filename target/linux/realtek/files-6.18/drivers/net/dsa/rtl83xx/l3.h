/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_L3_H
#define _OTTO_L3_H

#include "rtl-otto.h"

struct otto_l3_ctrl {
	struct rtl838x_switch_priv *priv;
	struct notifier_block fib_nb;
	struct notifier_block ne_nb;
	struct rhltable routes;
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
