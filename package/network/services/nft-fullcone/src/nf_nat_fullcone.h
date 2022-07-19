// SPDX-License-Identifier: GPL-2.0-only

/*
 * Nftables NAT extension: fullcone expression support library header
 *
 * Copyright (c) 2018 Chion Tang <tech@chionlab.moe>
 *   Original xt_FULLCONENAT and related iptables extension author
 * Copyright (c) 2019-2022 GitHub/llccd Twitter/@gNodeB
 *   Added IPv6 support for xt_FULLCONENAT and ip6tables extension
 *   Ported to recent kernel versions
 * Copyright (c) 2022 Syrone Wong <wong.syrone@gmail.com>
 *   Massively rewrite the whole module, split the original code into library and nftables 'fullcone' expression module
 */
#ifndef _NF_NAT_FULLCONE_H_
#define _NF_NAT_FULLCONE_H_

#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/list.h>
#include <linux/inetdevice.h>
#include <linux/netfilter/nf_tables.h>

#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_zones.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_ecache.h>

#ifndef NF_NAT_RANGE_PROTO_RANDOM_FULLY
#define NF_NAT_RANGE_PROTO_RANDOM_FULLY (1 << 4)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0)
static inline int nf_ct_netns_get(struct net *net, u8 nfproto)
{
	return 0;
}

static inline void nf_ct_netns_put(struct net *net, u8 nfproto)
{
}
#endif

/**
 * enum nft_fullcone_attributes - nf_tables fullcone expression netlink attributes
 *
 * @NFTA_FULLCONE_REG_PROTO_MIN: source register of proto range start (NLA_U32: nft_registers)
 * @NFTA_FULLCONE_REG_PROTO_MAX: source register of proto range end (NLA_U32: nft_registers)
 * @NFTA_FULLCONE_FLAGS: NAT flags (see NF_NAT_RANGE_* in linux/netfilter/nf_nat.h) (NLA_U32)
 */
enum nft_fullcone_attributes {
	NFTA_FULLCONE_UNSPEC,
	NFTA_FULLCONE_REG_PROTO_MIN,
	NFTA_FULLCONE_REG_PROTO_MAX,
	NFTA_FULLCONE_FLAGS,
	__NFTA_FULLCONE_MAX
};
#define NFTA_FULLCONE_MAX		(__NFTA_FULLCONE_MAX - 1)

/* fullcone specific data structures */

struct nat_mapping_original_tuple {
	struct nf_conntrack_tuple tuple;

	struct list_head node;
};

struct nat_mapping {
	uint16_t port;		/* external source port */
	__be32 addr;		/* external source ip address */

	__be32 int_addr;	/* internal source ip address */
	uint16_t int_port;	/* internal source port */

	int refer_count;	/* how many references linked to this mapping
				 * aka. length of original_tuple_list */

	struct list_head original_tuple_list;

	struct hlist_node node_by_ext_port;
	struct hlist_node node_by_int_src;

};

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
struct nat_mapping6 {
	uint16_t port;		/* external source port */
	union nf_inet_addr addr;	/* external source ip address */

	union nf_inet_addr int_addr;	/* internal source ip address */
	uint16_t int_port;	/* internal source port */

	int refer_count;	/* how many references linked to this mapping
				 * aka. length of original_tuple_list */

	struct list_head original_tuple_list;

	struct hlist_node node_by_ext_port;
	struct hlist_node node_by_int_src;

};
#endif

struct tuple_list {
	struct nf_conntrack_tuple tuple_original;
	struct nf_conntrack_tuple tuple_reply;
	struct list_head list;
};

/* fullcone specific data structures end */

// NOTE: declaration listed here must use EXPORT_SYMBOL_*

unsigned int nf_nat_fullcone_ipv4(struct sk_buff *skb, unsigned int hooknum,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				  struct nf_nat_range2 *range,
#else
				  struct nf_nat_range *range,
#endif
				  const struct net_device *out);

unsigned int nf_nat_fullcone_ipv6(struct sk_buff *skb, unsigned int hooknum,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				  struct nf_nat_range2 *range,
#else
				  struct nf_nat_range *range,
#endif
				  const struct net_device *out);

void nf_nat_fullcone_handle_dying_tuples(void);
void nf_nat_fullcone_destroy_mappings(void);
void nf_nat_fullcone_dying_tuple_list_add(struct list_head *new_dying);

/*
 * For [FUTURE] usage
 *
 * from https://elixir.bootlin.com/linux/v5.15.32/source/net/netfilter/xt_nat.c#L37
static void xt_nat_convert_range(struct nf_nat_range2 *dst,
				 const struct nf_nat_ipv4_range *src)
{
	memset(&dst->min_addr, 0, sizeof(dst->min_addr));
	memset(&dst->max_addr, 0, sizeof(dst->max_addr));
	// base_proto is nf_nat_range2 specific
	memset(&dst->base_proto, 0, sizeof(dst->base_proto));

	dst->flags	 = src->flags;
	dst->min_addr.ip = src->min_ip;
	dst->max_addr.ip = src->max_ip;
	dst->min_proto	 = src->min;
	dst->max_proto	 = src->max;
}
 *
 */

#endif /*_NF_NAT_FULLCONE_H_ */