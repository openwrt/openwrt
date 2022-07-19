// SPDX-License-Identifier: GPL-2.0-only

/*
 * Nftables NAT extension: fullcone expression support library
 *
 * Copyright (c) 2018 Chion Tang <tech@chionlab.moe>
 *   Original xt_FULLCONENAT and related iptables extension author
 * Copyright (c) 2019-2022 GitHub/llccd Twitter/@gNodeB
 *   Added IPv6 support for xt_FULLCONENAT and ip6tables extension
 *   Ported to recent kernel versions
 * Copyright (c) 2022 Syrone Wong <wong.syrone@gmail.com>
 *   Massively rewrite the whole module, split the original code into library and nftables 'fullcone' expression module
 */

#define pr_fmt(fmt) "fullcone " KBUILD_MODNAME ": " fmt

#include <linux/version.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/hashtable.h>
#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/jhash.h>

#ifdef CONFIG_NF_CONNTRACK_CHAIN_EVENTS
#include <linux/notifier.h>
#endif

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
#include <linux/netfilter_ipv6.h>
#include <linux/ipv6.h>
#include <net/addrconf.h>
#endif

#include "nf_nat_fullcone.h"

/*
 * FULLCONE_HKEY generates u32 hash value
 * Modified from net/netfilter/ipset/ip_set_hash_gen.h
 * dataptr: a pointer
 * datatypelen: sizeof(struct blah) or sizeof(u32)
 * initval: initial value
 * htable_bits: hashtable bits
 */
#define FULLCONE_HKEY(dataptr, datatypelen, initval, htable_bits)			\
({								\
	const u32 *__k = (const u32 *)(dataptr);			\
	u32 __l = (datatypelen) / sizeof(u32);			\
								\
	BUILD_BUG_ON((datatypelen) % sizeof(u32) != 0);		\
								\
	jhash2(__k, __l, (initval)) & jhash_mask((htable_bits));	\
})

#define HASHTABLE_BUCKET_BITS 10

/* static variables */

static DEFINE_HASHTABLE(mapping_table_by_ext_port, HASHTABLE_BUCKET_BITS);
static DEFINE_HASHTABLE(mapping_table_by_int_src, HASHTABLE_BUCKET_BITS);

static DEFINE_SPINLOCK(fullconenat_lock);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static DEFINE_HASHTABLE(mapping6_table_by_ext_port, HASHTABLE_BUCKET_BITS);
static DEFINE_HASHTABLE(mapping6_table_by_int_src, HASHTABLE_BUCKET_BITS);

static DEFINE_SPINLOCK(fullconenat6_lock);
#endif

static LIST_HEAD(dying_tuple_list);
static DEFINE_SPINLOCK(dying_tuple_list_lock);

/* static variables end */

/* forward declaration */

#if IS_ENABLED(CONFIG_IPV6)
static int nat_ipv6_dev_get_saddr(struct net *net, const struct net_device *dev,
				  const struct in6_addr *daddr, unsigned int srcprefs, struct in6_addr *saddr);
#endif

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
/* non-atomic: can only be called serially within lock zones. */
static char *fullcone_nf_ct_stringify_tuple6(const struct nf_conntrack_tuple
					     *t);
#endif
/* non-atomic: can only be called serially within lock zones. */
static char *nf_ct_stringify_tuple(const struct nf_conntrack_tuple *t);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static struct nat_mapping6 *allocate_mapping6(const union nf_inet_addr
					      *int_addr,
					      const uint16_t int_port,
					      const uint16_t port, const union nf_inet_addr *addr);
#endif
static struct nat_mapping *allocate_mapping(const __be32 int_addr,
					    const uint16_t int_port, const uint16_t port, const __be32 addr);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static void add_original_tuple_to_mapping6(struct nat_mapping6 *mapping, const struct nf_conntrack_tuple
					   *original_tuple);
#endif
static void add_original_tuple_to_mapping(struct nat_mapping *mapping, const struct nf_conntrack_tuple
					  *original_tuple);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static struct nat_mapping6 *get_mapping6_by_int_src(const union nf_inet_addr
						    *src_ip, const uint16_t src_port, const union nf_inet_addr
						    *ext_ip);
#endif

static struct nat_mapping *get_mapping_by_int_src(const __be32 src_ip, const uint16_t src_port, const __be32 ext_ip);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static struct nat_mapping6 *get_mapping6_by_int_src_inrange(const union
							    nf_inet_addr
							    *src_ip, const uint16_t src_port, const union
							    nf_inet_addr
							    *min_ip, const union
							    nf_inet_addr
							    *max_ip);
#endif
static struct nat_mapping *get_mapping_by_int_src_inrange(const __be32 src_ip,
							  const uint16_t
							  src_port, const __be32 min_ip, const __be32 max_ip);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static void kill_mapping6(struct nat_mapping6 *mapping);
#endif
static void kill_mapping(struct nat_mapping *mapping);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))

/* check if a mapping is valid.
 * possibly delete and free an invalid mapping.
 * the mapping should not be used anymore after check_mapping6() returns 0. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static int check_mapping6(struct nat_mapping6 *mapping, struct net *net, const struct nf_conntrack_zone *zone);
#else
static int check_mapping6(struct nat_mapping6 *mapping, struct net *net, const u16 zone);
#endif

#endif

/* check if a mapping is valid.
 * possibly delete and free an invalid mapping.
 * the mapping should not be used anymore after check_mapping() returns 0. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static int check_mapping(struct nat_mapping *mapping, struct net *net, const struct nf_conntrack_zone *zone);
#else
static int check_mapping(struct nat_mapping *mapping, struct net *net, const u16 zone);
#endif

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static struct nat_mapping6 *get_mapping6_by_ext_port(const uint16_t port, const union nf_inet_addr
						     *ext_ip, struct net *net, const struct
						     nf_conntrack_zone *zone);
#else
static struct nat_mapping6 *get_mapping6_by_ext_port(const uint16_t port, const union nf_inet_addr
						     *ext_ip, struct net *net, const u16 zone);
#endif
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static struct nat_mapping *get_mapping_by_ext_port(const uint16_t port, const __be32 ext_ip, struct net *net, const struct
						   nf_conntrack_zone *zone);
#else
static struct nat_mapping *get_mapping_by_ext_port(const uint16_t port,
						   const __be32 ext_ip, struct net *net, const u16 zone);
#endif

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static uint16_t find_appropriate_port6(struct net *net,
				       const struct nf_conntrack_zone *zone,
				       const uint16_t original_port, const union nf_inet_addr *ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				       struct nf_nat_range2 *range);

#else
				       struct nf_nat_range *range);

#endif

#else
static uint16_t find_appropriate_port6(struct net *net, const u16 zone,
				       const uint16_t original_port, const union nf_inet_addr *ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				       struct nf_nat_range2 *range);

#else
				       struct nf_nat_range *range);

#endif
#endif

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static uint16_t find_appropriate_port(struct net *net,
				      const struct nf_conntrack_zone *zone,
				      const uint16_t original_port, const __be32 ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				      struct nf_nat_range2 *range);
#else
				      struct nf_nat_range *range);
#endif

#else
static uint16_t find_appropriate_port(struct net *net, const u16 zone,
				      const uint16_t original_port, const __be32 ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				      struct nf_nat_range2 *range);
#else
				      struct nf_nat_range *range);
#endif
#endif

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static void find_leastused_ip6(const struct nf_conntrack_zone *zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
			       struct nf_nat_range2 *range,
#else
			       struct nf_nat_range *range,
#endif
			       const union nf_inet_addr *src,
			       const union nf_inet_addr *dst, union nf_inet_addr *var_ipp);
#else
static void find_leastused_ip6(const u16 zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
			       struct nf_nat_range2 *range,
#else
			       struct nf_nat_range *range,
#endif
			       const union nf_inet_addr *src,
			       const union nf_inet_addr *dst, union nf_inet_addr *var_ipp);
#endif
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static __be32 find_leastused_ip(const struct nf_conntrack_zone *zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				struct nf_nat_range2 *range,
#else
				struct nf_nat_range *range,
#endif
				const __be32 src, const __be32 dst);
#else
static __be32 find_leastused_ip(const u16 zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				struct nf_nat_range2 *range,
#else
				struct nf_nat_range *range,
#endif
				const __be32 src, const __be32 dst);
#endif

/* forward declaration end */

/* non-atomic part */

static char tuple_tmp_string[512];

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
/* non-atomic: can only be called serially within lock zones. */
static char *fullcone_nf_ct_stringify_tuple6(const struct nf_conntrack_tuple *t)
{
	snprintf(tuple_tmp_string, sizeof(tuple_tmp_string),
		 "[%pI6c]:%hu -> [%pI6c]:%hu", &t->src.u3.ip6,
		 be16_to_cpu(t->src.u.all), &t->dst.u3.ip6, be16_to_cpu(t->dst.u.all));
	return tuple_tmp_string;
}
#endif
/* non-atomic: can only be called serially within lock zones. */
static char *nf_ct_stringify_tuple(const struct nf_conntrack_tuple *t)
{
	snprintf(tuple_tmp_string, sizeof(tuple_tmp_string),
		 "%pI4:%hu -> %pI4:%hu", &t->src.u3.ip,
		 be16_to_cpu(t->src.u.all), &t->dst.u3.ip, be16_to_cpu(t->dst.u.all));
	return tuple_tmp_string;
}

/* non-atomic part end */

void nf_nat_fullcone_dying_tuple_list_add(struct list_head *new_dying)
{
	spin_lock_bh(&dying_tuple_list_lock);
	list_add(new_dying, &dying_tuple_list);
	spin_unlock_bh(&dying_tuple_list_lock);
}

EXPORT_SYMBOL_GPL(nf_nat_fullcone_dying_tuple_list_add);

void nf_nat_fullcone_handle_dying_tuples(void)
{
	struct list_head *iter, *tmp, *iter_2, *tmp_2;
	struct tuple_list *item;
	struct nf_conntrack_tuple *ct_tuple;
	struct nat_mapping *mapping;
	__be32 ip, ext_ip;
	uint16_t port;
	struct nat_mapping_original_tuple *original_tuple_item;
#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
	struct nat_mapping6 *mapping6;
	union nf_inet_addr *ip6, *ext_ip6;
	spin_lock_bh(&fullconenat6_lock);
#endif

	spin_lock_bh(&fullconenat_lock);
	spin_lock_bh(&dying_tuple_list_lock);

	list_for_each_safe(iter, tmp, &dying_tuple_list) {
		item = list_entry(iter, struct tuple_list, list);

		/* we dont know the conntrack direction for now so we try in both ways. */
		ct_tuple = &(item->tuple_original);
#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
		if (ct_tuple->src.l3num == PF_INET6) {
			ip6 = &(ct_tuple->src).u3;
			port = be16_to_cpu((ct_tuple->src).u.udp.port);
			ext_ip6 = &item->tuple_reply.dst.u3;
			mapping6 = get_mapping6_by_int_src(ip6, port, ext_ip6);
			if (mapping6 == NULL) {
				ext_ip6 = &(ct_tuple->dst).u3;
				ct_tuple = &(item->tuple_reply);
				ip6 = &(ct_tuple->src).u3;
				port = be16_to_cpu((ct_tuple->src).u.udp.port);
				mapping6 = get_mapping6_by_int_src(ip6, port, ext_ip6);
				if (mapping6 != NULL) {
					pr_debug
					    ("nf_nat_fullcone_handle_dying_tuples(): INBOUND dying conntrack at ext port %d\n",
					     mapping6->port);
				}
			} else {
				pr_debug
				    ("nf_nat_fullcone_handle_dying_tuples(): OUTBOUND dying conntrack at ext port %d\n",
				     mapping6->port);
			}

			if (mapping6 == NULL) {
				goto next;
			}

			/* look for the corresponding out-dated tuple and free it */
			list_for_each_safe(iter_2, tmp_2, &mapping6->original_tuple_list) {
				original_tuple_item = list_entry(iter_2, struct
								 nat_mapping_original_tuple, node);

				if (nf_ct_tuple_equal(&original_tuple_item->tuple, &(item->tuple_original))) {
					pr_debug
					    ("nf_nat_fullcone_handle_dying_tuples(): tuple %s expired. free this tuple.\n",
					     fullcone_nf_ct_stringify_tuple6(&original_tuple_item->tuple));
					list_del(&original_tuple_item->node);
					kfree(original_tuple_item);
					(mapping6->refer_count)--;
				}
			}

			/* then kill the mapping if needed */
			pr_debug
			    ("nf_nat_fullcone_handle_dying_tuples(): refer_count for mapping at ext_port %d is now %d\n",
			     mapping6->port, mapping6->refer_count);
			if (mapping6->refer_count <= 0) {
				pr_debug
				    ("nf_nat_fullcone_handle_dying_tuples(): kill expired mapping at ext port %d\n",
				     mapping6->port);
				kill_mapping6(mapping6);
			}
			goto next;
		}
		if (unlikely(ct_tuple->src.l3num != PF_INET))
#else
		if (ct_tuple->src.l3num != PF_INET)
#endif
			goto next;

		ip = (ct_tuple->src).u3.ip;
		port = be16_to_cpu((ct_tuple->src).u.udp.port);
		ext_ip = item->tuple_reply.dst.u3.ip;
		mapping = get_mapping_by_int_src(ip, port, ext_ip);
		if (mapping == NULL) {
			ext_ip = (ct_tuple->dst).u3.ip;
			ct_tuple = &(item->tuple_reply);
			ip = (ct_tuple->src).u3.ip;
			port = be16_to_cpu((ct_tuple->src).u.udp.port);
			mapping = get_mapping_by_int_src(ip, port, ext_ip);
			if (mapping != NULL) {
				pr_debug
				    ("nf_nat_fullcone_handle_dying_tuples(): INBOUND dying conntrack at ext port %d\n",
				     mapping->port);
			}
		} else {
			pr_debug
			    ("nf_nat_fullcone_handle_dying_tuples(): OUTBOUND dying conntrack at ext port %d\n",
			     mapping->port);
		}

		if (mapping == NULL) {
			goto next;
		}

		/* look for the corresponding out-dated tuple and free it */
		list_for_each_safe(iter_2, tmp_2, &mapping->original_tuple_list) {
			original_tuple_item = list_entry(iter_2, struct nat_mapping_original_tuple, node);

			if (nf_ct_tuple_equal(&original_tuple_item->tuple, &(item->tuple_original))) {
				pr_debug
				    ("nf_nat_fullcone_handle_dying_tuples(): tuple %s expired. free this tuple.\n",
				     nf_ct_stringify_tuple(&original_tuple_item->tuple));
				list_del(&original_tuple_item->node);
				kfree(original_tuple_item);
				(mapping->refer_count)--;
			}
		}

		/* then kill the mapping if needed */
		pr_debug
		    ("nf_nat_fullcone_handle_dying_tuples(): refer_count for mapping at ext_port %d is now %d\n",
		     mapping->port, mapping->refer_count);
		if (mapping->refer_count <= 0) {
			pr_debug
			    ("nf_nat_fullcone_handle_dying_tuples(): kill expired mapping at ext port %d\n",
			     mapping->port);
			kill_mapping(mapping);
		}

next:
		list_del(&item->list);
		kfree(item);
	}

	spin_unlock_bh(&dying_tuple_list_lock);
	spin_unlock_bh(&fullconenat_lock);
#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
	spin_unlock_bh(&fullconenat6_lock);
#endif
}

EXPORT_SYMBOL_GPL(nf_nat_fullcone_handle_dying_tuples);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static struct nat_mapping6 *allocate_mapping6(const union nf_inet_addr
					      *int_addr,
					      const uint16_t int_port,
					      const uint16_t port, const union nf_inet_addr *addr)
{
	struct nat_mapping6 *p_new;
	u32 hash_src;

	p_new = kmalloc(sizeof(struct nat_mapping6), GFP_ATOMIC);
	if (p_new == NULL) {
		pr_err("kmalloc() for allocate_mapping6 failed.\n");
		return NULL;
	}
	p_new->addr = *addr;
	p_new->port = port;
	p_new->int_addr = *int_addr;
	p_new->int_port = int_port;
	p_new->refer_count = 0;
	(p_new->original_tuple_list).next = &(p_new->original_tuple_list);
	(p_new->original_tuple_list).prev = &(p_new->original_tuple_list);

	hash_src = FULLCONE_HKEY(int_addr, sizeof(union nf_inet_addr), (u32) int_port, HASHTABLE_BUCKET_BITS);
	//hash_src = jhash2((u32 *) int_addr->all, 4, (u32) int_port);

	hash_add(mapping6_table_by_ext_port, &p_new->node_by_ext_port, port);
	hash_add(mapping6_table_by_int_src, &p_new->node_by_int_src, hash_src);

	pr_debug("new mapping allocated for [%pI6c]:%d ==> [%pI6c]:%d\n",
		 &p_new->int_addr, p_new->int_port, &p_new->addr, p_new->port);

	return p_new;
}
#endif
static struct nat_mapping *allocate_mapping(const __be32 int_addr,
					    const uint16_t int_port, const uint16_t port, const __be32 addr)
{
	struct nat_mapping *p_new;
	u32 hash_src;

	p_new = kmalloc(sizeof(struct nat_mapping), GFP_ATOMIC);
	if (p_new == NULL) {
		pr_err("kmalloc() for allocate_mapping failed.\n");
		return NULL;
	}
	p_new->addr = addr;
	p_new->port = port;
	p_new->int_addr = int_addr;
	p_new->int_port = int_port;
	p_new->refer_count = 0;
	(p_new->original_tuple_list).next = &(p_new->original_tuple_list);
	(p_new->original_tuple_list).prev = &(p_new->original_tuple_list);

	hash_src = FULLCONE_HKEY(&int_addr, sizeof(__be32), (u32) int_port, HASHTABLE_BUCKET_BITS);
	//hash_src = HASH_2(int_addr, (u32) int_port);

	hash_add(mapping_table_by_ext_port, &p_new->node_by_ext_port, port);
	hash_add(mapping_table_by_int_src, &p_new->node_by_int_src, hash_src);

	pr_debug("new mapping allocated for %pI4:%d ==> %pI4:%d\n",
		 &p_new->int_addr, p_new->int_port, &p_new->addr, p_new->port);

	return p_new;
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static void add_original_tuple_to_mapping6(struct nat_mapping6 *mapping, const struct nf_conntrack_tuple
					   *original_tuple)
{
	struct nat_mapping_original_tuple *item = kmalloc(sizeof(struct nat_mapping_original_tuple), GFP_ATOMIC);
	if (item == NULL) {
		pr_err("kmalloc() for add_original_tuple_to_mapping6 failed.\n");
		return;
	}
	memcpy(&item->tuple, original_tuple, sizeof(struct nf_conntrack_tuple));
	list_add(&item->node, &mapping->original_tuple_list);
	(mapping->refer_count)++;
}
#endif
static void add_original_tuple_to_mapping(struct nat_mapping *mapping, const struct nf_conntrack_tuple
					  *original_tuple)
{
	struct nat_mapping_original_tuple *item = kmalloc(sizeof(struct nat_mapping_original_tuple), GFP_ATOMIC);
	if (item == NULL) {
		pr_err("kmalloc() for add_original_tuple_to_mapping failed.\n");
		return;
	}
	memcpy(&item->tuple, original_tuple, sizeof(struct nf_conntrack_tuple));
	list_add(&item->node, &mapping->original_tuple_list);
	(mapping->refer_count)++;
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static struct nat_mapping6 *get_mapping6_by_int_src(const union nf_inet_addr
						    *src_ip, const uint16_t src_port, const union nf_inet_addr
						    *ext_ip)
{
	struct nat_mapping6 *p_current;
	u32 hash_src = FULLCONE_HKEY(src_ip, sizeof(union nf_inet_addr), (u32) src_port, HASHTABLE_BUCKET_BITS);
	//u32 hash_src = jhash2((u32 *) src_ip->all, 4, (u32) src_port);

	hash_for_each_possible(mapping6_table_by_int_src, p_current, node_by_int_src, hash_src) {
		if (nf_inet_addr_cmp(&p_current->int_addr, src_ip)
		    && p_current->int_port == src_port && nf_inet_addr_cmp(&p_current->addr, ext_ip)) {
			return p_current;
		}
	}

	return NULL;
}
#endif

static struct nat_mapping *get_mapping_by_int_src(const __be32 src_ip, const uint16_t src_port, const __be32 ext_ip)
{
	struct nat_mapping *p_current;
	u32 hash_src = FULLCONE_HKEY(&src_ip, sizeof(__be32), (u32) src_port, HASHTABLE_BUCKET_BITS);
	//u32 hash_src = HASH_2(src_ip, (u32) src_port);

	hash_for_each_possible(mapping_table_by_int_src, p_current, node_by_int_src, hash_src) {
		if (p_current->int_addr == src_ip && p_current->int_port == src_port && p_current->addr == ext_ip) {
			return p_current;
		}
	}

	return NULL;
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static struct nat_mapping6 *get_mapping6_by_int_src_inrange(const union
							    nf_inet_addr
							    *src_ip, const uint16_t src_port, const union
							    nf_inet_addr
							    *min_ip, const union
							    nf_inet_addr
							    *max_ip)
{
	struct nat_mapping6 *p_current;

	u32 hash_src = FULLCONE_HKEY(src_ip, sizeof(union nf_inet_addr), (u32) src_port, HASHTABLE_BUCKET_BITS);
	//u32 hash_src = jhash2((u32 *) src_ip->all, 4, (u32) src_port);

	hash_for_each_possible(mapping6_table_by_int_src, p_current, node_by_int_src, hash_src) {
		if (nf_inet_addr_cmp(&p_current->int_addr, src_ip)
		    && p_current->int_port == src_port
		    && memcmp(&p_current->addr, min_ip,
			      sizeof(union nf_inet_addr)) >= 0
		    && memcmp(&p_current->addr, max_ip, sizeof(union nf_inet_addr)) <= 0) {
			return p_current;
		}
	}

	return NULL;
}
#endif
static struct nat_mapping *get_mapping_by_int_src_inrange(const __be32 src_ip,
							  const uint16_t
							  src_port, const __be32 min_ip, const __be32 max_ip)
{
	struct nat_mapping *p_current;
	u32 hash_src = FULLCONE_HKEY(&src_ip, sizeof(__be32), (u32) src_port, HASHTABLE_BUCKET_BITS);
	//u32 hash_src = HASH_2(src_ip, (u32) src_port);

	hash_for_each_possible(mapping_table_by_int_src, p_current, node_by_int_src, hash_src) {
		if (p_current->int_addr == src_ip
		    && p_current->int_port == src_port
		    && memcmp(&p_current->addr, &min_ip, sizeof(__be32)) >= 0
		    && memcmp(&p_current->addr, &max_ip, sizeof(__be32)) <= 0) {
			return p_current;
		}
	}

	return NULL;
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
static void kill_mapping6(struct nat_mapping6 *mapping)
{
	struct list_head *iter, *tmp;
	struct nat_mapping_original_tuple *original_tuple_item;

	if (mapping == NULL) {
		return;
	}

	list_for_each_safe(iter, tmp, &mapping->original_tuple_list) {
		original_tuple_item = list_entry(iter, struct nat_mapping_original_tuple, node);
		list_del(&original_tuple_item->node);
		kfree(original_tuple_item);
	}

	hash_del(&mapping->node_by_ext_port);
	hash_del(&mapping->node_by_int_src);
	kfree(mapping);
}
#endif
static void kill_mapping(struct nat_mapping *mapping)
{
	struct list_head *iter, *tmp;
	struct nat_mapping_original_tuple *original_tuple_item;

	if (mapping == NULL) {
		return;
	}

	list_for_each_safe(iter, tmp, &mapping->original_tuple_list) {
		original_tuple_item = list_entry(iter, struct nat_mapping_original_tuple, node);
		list_del(&original_tuple_item->node);
		kfree(original_tuple_item);
	}

	hash_del(&mapping->node_by_ext_port);
	hash_del(&mapping->node_by_int_src);
	kfree(mapping);
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))

/* check if a mapping is valid.
 * possibly delete and free an invalid mapping.
 * the mapping should not be used anymore after check_mapping6() returns 0. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static int check_mapping6(struct nat_mapping6 *mapping, struct net *net, const struct nf_conntrack_zone *zone)
{
#else
static int check_mapping6(struct nat_mapping6 *mapping, struct net *net, const u16 zone)
{
#endif
	struct list_head *iter, *tmp;
	struct nat_mapping_original_tuple *original_tuple_item;
	struct nf_conntrack_tuple_hash *tuple_hash;
	struct nf_conn *ct;

	/* for dying/unconfirmed conntrack tuples, an IPCT_DESTROY event may NOT be fired.
	 * so we manually kill one of those tuples once we acquire one. */

	list_for_each_safe(iter, tmp, &mapping->original_tuple_list) {
		original_tuple_item = list_entry(iter, struct nat_mapping_original_tuple, node);

		tuple_hash = nf_conntrack_find_get(net, zone, &original_tuple_item->tuple);

		if (tuple_hash == NULL) {
			pr_debug
			    ("check_mapping6(): tuple %s dying/unconfirmed. free this tuple.\n",
			     fullcone_nf_ct_stringify_tuple6(&original_tuple_item->tuple));

			list_del(&original_tuple_item->node);
			kfree(original_tuple_item);
			(mapping->refer_count)--;
		} else {
			ct = nf_ct_tuplehash_to_ctrack(tuple_hash);
			if (likely(ct != NULL))
				nf_ct_put(ct);
		}

	}

	/* kill the mapping if need */
	pr_debug
	    ("check_mapping6() refer_count for mapping at ext_port %d is now %d\n",
	     mapping->port, mapping->refer_count);
	if (mapping->refer_count <= 0) {
		pr_debug("check_mapping6(): kill dying/unconfirmed mapping at ext port %d\n", mapping->port);
		kill_mapping6(mapping);
		return 0;
	} else {
		return 1;
	}
}

#endif

/* check if a mapping is valid.
 * possibly delete and free an invalid mapping.
 * the mapping should not be used anymore after check_mapping() returns 0. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static int check_mapping(struct nat_mapping *mapping, struct net *net, const struct nf_conntrack_zone *zone)
{
#else
static int check_mapping(struct nat_mapping *mapping, struct net *net, const u16 zone)
{
#endif
	struct list_head *iter, *tmp;
	struct nat_mapping_original_tuple *original_tuple_item;
	struct nf_conntrack_tuple_hash *tuple_hash;
	struct nf_conn *ct;

	/* for dying/unconfirmed conntrack tuples, an IPCT_DESTROY event may NOT be fired.
	 * so we manually kill one of those tuples once we acquire one. */

	list_for_each_safe(iter, tmp, &mapping->original_tuple_list) {
		original_tuple_item = list_entry(iter, struct nat_mapping_original_tuple, node);

		tuple_hash = nf_conntrack_find_get(net, zone, &original_tuple_item->tuple);

		if (tuple_hash == NULL) {
			pr_debug
			    ("check_mapping(): tuple %s dying/unconfirmed. free this tuple.\n",
			     nf_ct_stringify_tuple(&original_tuple_item->tuple));

			list_del(&original_tuple_item->node);
			kfree(original_tuple_item);
			(mapping->refer_count)--;
		} else {
			ct = nf_ct_tuplehash_to_ctrack(tuple_hash);
			if (likely(ct != NULL))
				nf_ct_put(ct);
		}

	}

	/* kill the mapping if need */
	pr_debug
	    ("check_mapping() refer_count for mapping at ext_port %d is now %d\n", mapping->port, mapping->refer_count);
	if (mapping->refer_count <= 0) {
		pr_debug("check_mapping(): kill dying/unconfirmed mapping at ext port %d\n", mapping->port);
		kill_mapping(mapping);
		return 0;
	} else {
		return 1;
	}
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static struct nat_mapping6 *get_mapping6_by_ext_port(const uint16_t port, const union nf_inet_addr
						     *ext_ip, struct net *net, const struct
						     nf_conntrack_zone *zone)
{
#else
static struct nat_mapping6 *get_mapping6_by_ext_port(const uint16_t port, const union nf_inet_addr
						     *ext_ip, struct net *net, const u16 zone)
{
#endif
	struct nat_mapping6 *p_current;
	struct hlist_node *tmp;

	hash_for_each_possible_safe(mapping6_table_by_ext_port, p_current, tmp, node_by_ext_port, port) {
		if (p_current->port == port && check_mapping6(p_current, net, zone)
		    && nf_inet_addr_cmp(&p_current->addr, ext_ip)) {
			return p_current;
		}
	}

	return NULL;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static struct nat_mapping *get_mapping_by_ext_port(const uint16_t port, const __be32 ext_ip, struct net *net, const struct
						   nf_conntrack_zone *zone)
{
#else
static struct nat_mapping *get_mapping_by_ext_port(const uint16_t port,
						   const __be32 ext_ip, struct net *net, const u16 zone)
{
#endif
	struct nat_mapping *p_current;
	struct hlist_node *tmp;

	hash_for_each_possible_safe(mapping_table_by_ext_port, p_current, tmp, node_by_ext_port, port) {
		if (p_current->port == port && check_mapping(p_current, net, zone)
		    && p_current->addr == ext_ip) {
			return p_current;
		}
	}

	return NULL;
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static uint16_t find_appropriate_port6(struct net *net,
				       const struct nf_conntrack_zone *zone,
				       const uint16_t original_port, const union nf_inet_addr *ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				       struct nf_nat_range2 *range)
#else
				       struct nf_nat_range *range)
#endif
#else
static uint16_t find_appropriate_port6(struct net *net, const u16 zone,
				       const uint16_t original_port, const union nf_inet_addr *ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				       struct nf_nat_range2 *range)
#else
				       struct nf_nat_range *range)
#endif
#endif
{
	uint16_t min, start, selected, range_size, i;
	struct nat_mapping6 *mapping = NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	// nf_nat_range2 specific
	memset(&range->base_proto, 0, sizeof(range->base_proto));
#endif
	if (range->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
		min = be16_to_cpu((range->min_proto).udp.port);
		range_size = be16_to_cpu((range->max_proto).udp.port) - min + 1;
	} else {
		/* minimum port is 1024. same behavior as default linux NAT. */
		min = 1024;
		range_size = 65535 - min + 1;
	}

	if ((range->flags & NF_NAT_RANGE_PROTO_RANDOM)
	    || (range->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY)) {
		/* for now we do the same thing for both --random and --random-fully */

		/* select a random starting point */
		start = (uint16_t) (prandom_u32() % (u32) range_size);
	} else {

		if ((original_port >= min && original_port <= min + range_size - 1)
		    || !(range->flags & NF_NAT_RANGE_PROTO_SPECIFIED)) {
			/* 1. try to preserve the port if it's available */
			mapping = get_mapping6_by_ext_port(original_port, ext_ip, net, zone);
			if (mapping == NULL) {
				return original_port;
			}
		}

		/* otherwise, we start from zero */
		start = 0;
	}

	for (i = 0; i < range_size; i++) {
		/* 2. try to find an available port */
		selected = min + ((start + i) % range_size);
		mapping = get_mapping6_by_ext_port(selected, ext_ip, net, zone);
		if (mapping == NULL) {
			return selected;
		}
	}

	/* 3. at least we tried. override a previous mapping. */
	selected = min + start;
	mapping = get_mapping6_by_ext_port(selected, ext_ip, net, zone);
	kill_mapping6(mapping);

	return selected;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static uint16_t find_appropriate_port(struct net *net,
				      const struct nf_conntrack_zone *zone,
				      const uint16_t original_port, const __be32 ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				      struct nf_nat_range2 *range)
#else
				      struct nf_nat_range *range)
#endif
#else
static uint16_t find_appropriate_port(struct net *net, const u16 zone,
				      const uint16_t original_port, const __be32 ext_ip,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				      struct nf_nat_range2 *range)
#else
				      struct nf_nat_range *range)
#endif
#endif
{
	uint16_t min, start, selected, range_size, i;
	struct nat_mapping *mapping = NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	// nf_nat_range2 specific
	memset(&range->base_proto, 0, sizeof(range->base_proto));
#endif
	if (range->flags & NF_NAT_RANGE_PROTO_SPECIFIED) {
		min = be16_to_cpu((range->min_proto).udp.port);
		range_size = be16_to_cpu((range->max_proto).udp.port) - min + 1;
	} else {
		/* minimum port is 1024. same behavior as default linux NAT. */
		min = 1024;
		range_size = 65535 - min + 1;
	}

	if ((range->flags & NF_NAT_RANGE_PROTO_RANDOM)
	    || (range->flags & NF_NAT_RANGE_PROTO_RANDOM_FULLY)) {
		/* for now we do the same thing for both --random and --random-fully */

		/* select a random starting point */
		start = (uint16_t) (prandom_u32() % (u32) range_size);
	} else {

		if ((original_port >= min && original_port <= min + range_size - 1)
		    || !(range->flags & NF_NAT_RANGE_PROTO_SPECIFIED)) {
			/* 1. try to preserve the port if it's available */
			mapping = get_mapping_by_ext_port(original_port, ext_ip, net, zone);
			if (mapping == NULL) {
				return original_port;
			}
		}

		/* otherwise, we start from zero */
		start = 0;
	}

	for (i = 0; i < range_size; i++) {
		/* 2. try to find an available port */
		selected = min + ((start + i) % range_size);
		mapping = get_mapping_by_ext_port(selected, ext_ip, net, zone);
		if (mapping == NULL) {
			return selected;
		}
	}

	/* 3. at least we tried. override a previous mapping. */
	selected = min + start;
	mapping = get_mapping_by_ext_port(selected, ext_ip, net, zone);
	kill_mapping(mapping);

	return selected;
}

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static void find_leastused_ip6(const struct nf_conntrack_zone *zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
			       struct nf_nat_range2 *range,
#else
			       struct nf_nat_range *range,
#endif
			       const union nf_inet_addr *src,
			       const union nf_inet_addr *dst, union nf_inet_addr *var_ipp)
#else
static void find_leastused_ip6(const u16 zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
			       struct nf_nat_range2 *range,
#else
			       struct nf_nat_range *range,
#endif
			       const union nf_inet_addr *src,
			       const union nf_inet_addr *dst, union nf_inet_addr *var_ipp)
#endif
{
	unsigned int i;
	/* Host order */
	u32 minip, maxip, j, dist;
	bool full_range;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	// nf_nat_range2 specific
	memset(&(range->base_proto), 0, sizeof(range->base_proto));
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
	j = FULLCONE_HKEY(src, sizeof(union nf_inet_addr),
			  range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst->all[3] ^ zone->id, HASHTABLE_BUCKET_BITS);
	//j = jhash2((u32 *) src, 4, range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst->all[3] ^ zone->id);
#else
	j = FULLCONE_HKEY(src, sizeof(union nf_inet_addr),
			  range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst->all[3] ^ zone, HASHTABLE_BUCKET_BITS);
	//j = jhash2((u32 *) src, 4, range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst->all[3] ^ zone);
#endif

	full_range = false;
	for (i = 0; i <= 3; i++) {
		/* If first bytes of the address are at the maximum, use the
		 * distance. Otherwise use the full range. */
		if (!full_range) {
			minip = ntohl(range->min_addr.all[i]);
			maxip = ntohl(range->max_addr.all[i]);
			dist = maxip - minip + 1;
		} else {
			minip = 0;
			dist = ~0;
		}

		var_ipp->all[i] = (__force __be32) htonl(minip + reciprocal_scale(j, dist));
		if (var_ipp->all[i] != range->max_addr.all[i])
			full_range = true;

		if (!(range->flags & NF_NAT_RANGE_PERSISTENT))
			j ^= (__force u32) dst->all[i];
	}
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static __be32 find_leastused_ip(const struct nf_conntrack_zone *zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				struct nf_nat_range2 *range,
#else
				struct nf_nat_range *range,
#endif
				const __be32 src, const __be32 dst)
#else
static __be32 find_leastused_ip(const u16 zone,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				struct nf_nat_range2 *range,
#else
				struct nf_nat_range *range,
#endif
				const __be32 src, const __be32 dst)
#endif
{
	/* Host order */
	u32 minip, maxip, j, dist;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	// nf_nat_range2 specific
	memset(&(range->base_proto), 0, sizeof(range->base_proto));
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
	j = FULLCONE_HKEY(&src, sizeof(__be32), range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst ^ zone->id,
			  HASHTABLE_BUCKET_BITS);
	//j = jhash_1word((u32) src, range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst ^ zone->id);
#else
	j = FULLCONE_HKEY(&src, sizeof(__be32), range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst ^ zone,
			  HASHTABLE_BUCKET_BITS);
	//j = jhash_1word((u32) src, range->flags & NF_NAT_RANGE_PERSISTENT ? 0 : dst ^ zone);
#endif

	minip = ntohl(range->min_addr.ip);
	maxip = ntohl(range->max_addr.ip);
	dist = maxip - minip + 1;

	return (__be32) htonl(minip + reciprocal_scale(j, dist));
}

void nf_nat_fullcone_destroy_mappings(void)
{
	struct nat_mapping *p_current;
#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
	struct nat_mapping6 *p6_current;
#endif
	struct hlist_node *tmp;
	int i;

	spin_lock_bh(&fullconenat_lock);

	hash_for_each_safe(mapping_table_by_ext_port, i, tmp, p_current, node_by_ext_port) {
		kill_mapping(p_current);
	}

	spin_unlock_bh(&fullconenat_lock);

#if IS_ENABLED(CONFIG_NF_NAT_IPV6) || (IS_ENABLED(CONFIG_IPV6) && LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
	spin_lock_bh(&fullconenat6_lock);

	hash_for_each_safe(mapping6_table_by_ext_port, i, tmp, p6_current, node_by_ext_port) {
		kill_mapping6(p6_current);
	}

	spin_unlock_bh(&fullconenat6_lock);
#endif
}

EXPORT_SYMBOL_GPL(nf_nat_fullcone_destroy_mappings);

/*
 * nfproto choices
 * enum {
	NFPROTO_INET   =  1,
	NFPROTO_IPV4   =  2,
	NFPROTO_IPV6   = 10,
};
 */
static unsigned int nf_nat_handle_prerouting(u8 nfproto, struct sk_buff *skb, unsigned int hooknum,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
					     struct nf_nat_range2 *newrange)
#else
					     struct nf_nat_range *newrange)
#endif
{
	unsigned int ret;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
	const struct nf_conntrack_zone *zone;
#else
	u16 zone;
#endif
	struct net *net;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	struct nf_conntrack_tuple *ct_tuple_origin;

	uint16_t port, original_port;
	uint8_t protonum;

/* NFPROTO specific def */
	struct nat_mapping *mapping;
	struct nat_mapping6 *mapping_6;

	__be32 ip;
	union nf_inet_addr *ip_6;
	/* NFPROTO specific def end */

	WARN_ON(!(nfproto == NFPROTO_IPV4 || nfproto == NFPROTO_IPV6));

	/* NFPROTO specific init */
	mapping = NULL;
	mapping_6 = NULL;

	ip = 0;
	ip_6 = NULL;
	/* NFPROTO specific init end */

	original_port = 0;
	ret = NFT_CONTINUE;	// BUG: use XT_CONTINUE for Xtables

	ct = nf_ct_get(skb, &ctinfo);
	net = nf_ct_net(ct);
	zone = nf_ct_zone(ct);

	ct_tuple_origin = &(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);

	protonum = (ct_tuple_origin->dst).protonum;
	if (protonum != IPPROTO_UDP) {
		// Currently only UDP traffic is supported for full-cone NAT.
		// For other protos FULLCONENAT is equivalent to MASQUERADE.
		return ret;
	}

	if (nfproto == NFPROTO_IPV4) {
		ip = (ct_tuple_origin->dst).u3.ip;
	} else if (nfproto == NFPROTO_IPV6) {
		ip_6 = &(ct_tuple_origin->dst).u3;
	}

	port = be16_to_cpu((ct_tuple_origin->dst).u.udp.port);

	if (nfproto == NFPROTO_IPV4) {
		spin_lock_bh(&fullconenat_lock);
	} else if (nfproto == NFPROTO_IPV6) {
		spin_lock_bh(&fullconenat6_lock);
	}

	/* find an active mapping based on the inbound port */
	if (nfproto == NFPROTO_IPV4) {
		mapping = get_mapping_by_ext_port(port, ip, net, zone);
	} else if (nfproto == NFPROTO_IPV6) {
		mapping_6 = get_mapping6_by_ext_port(port, ip_6, net, zone);
	}

	if (nfproto == NFPROTO_IPV4) {
		if (mapping == NULL) {
			goto unlock;
		}
	} else if (nfproto == NFPROTO_IPV6) {
		if (mapping_6 == NULL) {
			goto unlock;
		}
	}

	newrange->flags = NF_NAT_RANGE_MAP_IPS | NF_NAT_RANGE_PROTO_SPECIFIED;
	if (nfproto == NFPROTO_IPV4) {
		newrange->min_addr.ip = mapping->int_addr;
		newrange->max_addr.ip = mapping->int_addr;
		newrange->min_proto.udp.port = cpu_to_be16(mapping->int_port);
	} else if (nfproto == NFPROTO_IPV6) {
		newrange->min_addr = mapping_6->int_addr;
		newrange->max_addr = mapping_6->int_addr;
		newrange->min_proto.udp.port = cpu_to_be16(mapping_6->int_port);
	}
	newrange->max_proto = newrange->min_proto;

	if (nfproto == NFPROTO_IPV4) {
		pr_debug("<INBOUND DNAT> %s ==> %pI4:%d\n",
			 nf_ct_stringify_tuple(ct_tuple_origin), &mapping->int_addr, mapping->int_port);
	} else if (nfproto == NFPROTO_IPV6) {
		pr_debug("<INBOUND DNAT> %s ==> [%pI6c]:%d\n",
			 fullcone_nf_ct_stringify_tuple6(ct_tuple_origin), &mapping_6->int_addr, mapping_6->int_port);
	}

	ret = nf_nat_setup_info(ct, newrange, HOOK2MANIP(hooknum));

	if (ret == NF_ACCEPT) {
		if (nfproto == NFPROTO_IPV4) {
			add_original_tuple_to_mapping(mapping, ct_tuple_origin);
			pr_debug
			    ("INBOUND: refer_count for mapping at ext_port %d is now %d\n",
			     mapping->port, mapping->refer_count);
		} else if (nfproto == NFPROTO_IPV6) {
			add_original_tuple_to_mapping6(mapping_6, ct_tuple_origin);
			pr_debug
			    ("INBOUND: refer_count for mapping_6 at ext_port %d is now %d\n",
			     mapping_6->port, mapping_6->refer_count);
		}

	}

unlock:
	if (nfproto == NFPROTO_IPV4) {
		spin_unlock_bh(&fullconenat_lock);
	} else if (nfproto == NFPROTO_IPV6) {
		spin_unlock_bh(&fullconenat6_lock);
	}

	return ret;

}

static unsigned int nf_nat_handle_postrouting(u8 nfproto, struct sk_buff *skb, unsigned int hooknum,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
					      struct nf_nat_range2 *range, struct nf_nat_range2 *newrange,
#else
					      struct nf_nat_range *range, struct nf_nat_range *newrange,
#endif
					      const struct net_device *out)
{
	unsigned int ret;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
	const struct nf_conntrack_zone *zone;
#else
	u16 zone;
#endif
	struct net *net;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	struct nf_conn_nat *nat;
	struct nf_conntrack_tuple *ct_tuple, *ct_tuple_origin;
	uint16_t port, original_port, want_port;
	uint8_t protonum;
	bool is_src_mapping_active;

	/* NFPROTO specific def */
	struct nat_mapping *mapping, *src_mapping;
	struct nat_mapping6 *mapping_6, *src_mapping_6;

	__be32 ip;
	union nf_inet_addr *ip_6;

	const struct rtable *rt;
	__be32 newsrc, nh;

	/* NFPROTO specific def end */

	WARN_ON(!(nfproto == NFPROTO_IPV4 || nfproto == NFPROTO_IPV6));

	/* NFPROTO specific init */
	mapping = NULL;
	src_mapping = NULL;
	mapping_6 = NULL;
	src_mapping_6 = NULL;

	ip = 0;
	ip_6 = NULL;
	/* NFPROTO specific init end */

	original_port = 0;
	ret = NFT_CONTINUE;	// BUG: use XT_CONTINUE for Xtables

	ct = nf_ct_get(skb, &ctinfo);
	net = nf_ct_net(ct);
	zone = nf_ct_zone(ct);

	ct_tuple_origin = &(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
	protonum = (ct_tuple_origin->dst).protonum;

	if (range->flags & NF_NAT_RANGE_MAP_IPS) {
		if (nfproto == NFPROTO_IPV4) {
			newrange->min_addr.ip = range->min_addr.ip;
			newrange->max_addr.ip = range->max_addr.ip;
		} else if (nfproto == NFPROTO_IPV6) {
			newrange->min_addr = range->min_addr;
			newrange->max_addr = range->max_addr;
		}

	} else {
		if (nfproto == NFPROTO_IPV4) {
			rt = skb_rtable(skb);
			nh = rt_nexthop(rt, ip_hdr(skb)->daddr);
			newsrc = inet_select_addr(out, nh, RT_SCOPE_UNIVERSE);

			if (unlikely(!newsrc))
				return NF_DROP;
			newrange->min_addr.ip = newsrc;
			newrange->max_addr.ip = newsrc;
		} else if (nfproto == NFPROTO_IPV6) {
			if (unlikely
			    (nat_ipv6_dev_get_saddr
			     (nf_ct_net(ct), out, &ipv6_hdr(skb)->daddr, 0, &(newrange->min_addr.in6)) < 0))
				return NF_DROP;
			newrange->max_addr = newrange->min_addr;

		}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0)
		nat = nf_ct_nat_ext_add(ct);
#else
		nat = nfct_nat(ct);
#endif
		if (likely(nat))
			nat->masq_index = out->ifindex;

	}

	if (protonum == IPPROTO_UDP) {
		if (nfproto == NFPROTO_IPV4) {
			ip = (ct_tuple_origin->src).u3.ip;
		} else if (nfproto == NFPROTO_IPV6) {
			ip_6 = &(ct_tuple_origin->src).u3;
		}

		original_port = be16_to_cpu((ct_tuple_origin->src).u.udp.port);

		if (nfproto == NFPROTO_IPV4) {
			spin_lock_bh(&fullconenat_lock);
		} else if (nfproto == NFPROTO_IPV6) {
			spin_lock_bh(&fullconenat6_lock);
		}

		if (nfproto == NFPROTO_IPV4) {
			if (newrange->min_addr.ip != newrange->max_addr.ip)
				src_mapping =
				    get_mapping_by_int_src_inrange(ip,
								   original_port,
								   newrange->min_addr.ip, newrange->max_addr.ip);
			else
				src_mapping = get_mapping_by_int_src(ip, original_port, newrange->min_addr.ip);
		} else if (nfproto == NFPROTO_IPV6) {
			if (!nf_inet_addr_cmp(&newrange->min_addr, &newrange->max_addr))
				src_mapping_6 =
				    get_mapping6_by_int_src_inrange(ip_6,
								    original_port,
								    &newrange->min_addr, &newrange->max_addr);
			else
				src_mapping_6 = get_mapping6_by_int_src(ip_6, original_port, &newrange->min_addr);
		}

		if (nfproto == NFPROTO_IPV4) {
			is_src_mapping_active = src_mapping != NULL && check_mapping(src_mapping, net, zone);
		} else if (nfproto == NFPROTO_IPV6) {
			is_src_mapping_active = src_mapping_6 != NULL && check_mapping6(src_mapping_6, net, zone);
		}

		if (is_src_mapping_active) {

			/* outbound nat: if a previously established mapping is active,
			 * we will reuse that mapping. */

			newrange->flags = NF_NAT_RANGE_MAP_IPS | NF_NAT_RANGE_PROTO_SPECIFIED;
			if (nfproto == NFPROTO_IPV4) {
				newrange->min_proto.udp.port = cpu_to_be16(src_mapping->port);
			} else if (nfproto == NFPROTO_IPV6) {
				newrange->min_proto.udp.port = cpu_to_be16(src_mapping_6->port);
			}

			newrange->max_proto = newrange->min_proto;

			if (nfproto == NFPROTO_IPV4) {
				if (newrange->min_addr.ip != newrange->max_addr.ip) {
					newrange->min_addr.ip = src_mapping->addr;
					newrange->max_addr.ip = newrange->min_addr.ip;
				}
			} else if (nfproto == NFPROTO_IPV6) {
				if (!nf_inet_addr_cmp(&newrange->min_addr, &newrange->max_addr)) {
					newrange->min_addr = src_mapping_6->addr;
					newrange->max_addr = newrange->min_addr;
				}
			}

		} else {

			/* if not, we find a new external IP:port to map to.
			 * the SNAT may fail so we should re-check the mapped port later. */

			if (nfproto == NFPROTO_IPV4) {
				if (newrange->min_addr.ip != newrange->max_addr.ip) {
					newrange->min_addr.ip =
					    find_leastused_ip(zone, range, ip, (ct_tuple_origin->dst).u3.ip);
					newrange->max_addr.ip = newrange->min_addr.ip;
				}
				want_port =
				    find_appropriate_port(net, zone, original_port, newrange->min_addr.ip, range);
			} else if (nfproto == NFPROTO_IPV6) {

				if (!nf_inet_addr_cmp(&newrange->min_addr, &newrange->max_addr)) {
					find_leastused_ip6(zone, range, ip_6,
							   &(ct_tuple_origin->dst).u3, &newrange->min_addr);
					newrange->max_addr = newrange->min_addr;
				}

				want_port =
				    find_appropriate_port6(net, zone, original_port, &newrange->min_addr, range);
			}

			newrange->flags = NF_NAT_RANGE_MAP_IPS | NF_NAT_RANGE_PROTO_SPECIFIED;
			newrange->min_proto.udp.port = cpu_to_be16(want_port);
			newrange->max_proto = newrange->min_proto;

			if (nfproto == NFPROTO_IPV4) {
				src_mapping = NULL;
			} else if (nfproto == NFPROTO_IPV6) {
				src_mapping_6 = NULL;
			}

		}
	}

	/* do SNAT now */
	ret = nf_nat_setup_info(ct, newrange, HOOK2MANIP(hooknum));

	if (protonum != IPPROTO_UDP) {
		/* non-UDP packets, bailout */
		goto out;
	}
	if (ret != NF_ACCEPT) {
		/* failed SNAT, bailout */
		goto unlock;
	}

	/* the reply tuple contains the mapped port. */
	ct_tuple = &(ct->tuplehash[IP_CT_DIR_REPLY].tuple);
	/* this is the resulted mapped port. */
	port = be16_to_cpu((ct_tuple->dst).u.udp.port);

	if (nfproto == NFPROTO_IPV4) {
		pr_debug("<OUTBOUND SNAT> %s ==> %d\n", nf_ct_stringify_tuple(ct_tuple_origin), port);
	} else if (nfproto == NFPROTO_IPV6) {
		pr_debug("<OUTBOUND SNAT> %s ==> %d\n", fullcone_nf_ct_stringify_tuple6(ct_tuple_origin), port);
	}

	/* save the mapping information into our mapping table */

	if (nfproto == NFPROTO_IPV4) {
		mapping = src_mapping;
		if (mapping == NULL) {
			mapping = allocate_mapping(ip, original_port, port, (ct_tuple->dst).u3.ip);
		}
		if (likely(mapping != NULL)) {
			add_original_tuple_to_mapping(mapping, ct_tuple_origin);
			pr_debug
			    (" OUTBOUND: refer_count for mapping at ext_port %d is now %d\n",
			     mapping->port, mapping->refer_count);
		}
	} else if (nfproto == NFPROTO_IPV6) {
		mapping_6 = src_mapping_6;
		if (mapping_6 == NULL) {
			mapping_6 = allocate_mapping6(ip_6, original_port, port, &(ct_tuple->dst).u3);
		}
		if (likely(mapping_6 != NULL)) {
			add_original_tuple_to_mapping6(mapping_6, ct_tuple_origin);
			pr_debug
			    ("OUTBOUND: refer_count for mapping at ext_port %d is now %d\n",
			     mapping_6->port, mapping_6->refer_count);
		}
	}

unlock:
	if (nfproto == NFPROTO_IPV4) {
		spin_unlock_bh(&fullconenat_lock);
	} else if (nfproto == NFPROTO_IPV6) {
		spin_unlock_bh(&fullconenat6_lock);
	}

out:
	return ret;
}

unsigned int nf_nat_fullcone_ipv4(struct sk_buff *skb, unsigned int hooknum,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				  struct nf_nat_range2 *range,
#else
				  struct nf_nat_range *range,
#endif
				  const struct net_device *out)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	struct nf_nat_range2 newrange;
#else
	struct nf_nat_range newrange;
#endif

	WARN_ON(!(hooknum == NF_INET_POST_ROUTING || hooknum == NF_INET_PRE_ROUTING));

	memset(&newrange.min_addr, 0, sizeof(newrange.min_addr));
	memset(&newrange.max_addr, 0, sizeof(newrange.max_addr));
	newrange.flags = range->flags | NF_NAT_RANGE_MAP_IPS;
	newrange.min_proto = range->min_proto;
	newrange.max_proto = range->max_proto;

	switch (hooknum) {
	case NF_INET_PRE_ROUTING:
		/* inbound packets */
		return nf_nat_handle_prerouting(NFPROTO_IPV4, skb, hooknum, &newrange);
	case NF_INET_POST_ROUTING:
		/* outbound packets */
		return nf_nat_handle_postrouting(NFPROTO_IPV4, skb, hooknum, range, &newrange, out);
	}

	WARN_ON(1);
	// logical error
	return 5;
}

EXPORT_SYMBOL_GPL(nf_nat_fullcone_ipv4);

#if IS_ENABLED(CONFIG_IPV6)
static int
nat_ipv6_dev_get_saddr(struct net *net, const struct net_device *dev,
		       const struct in6_addr *daddr, unsigned int srcprefs, struct in6_addr *saddr)
{
#ifdef CONFIG_IPV6_MODULE
	const struct nf_ipv6_ops *v6_ops = nf_get_ipv6_ops();

	if (!v6_ops)
		return -EHOSTUNREACH;

	return v6_ops->dev_get_saddr(net, dev, daddr, srcprefs, saddr);
#else
	return ipv6_dev_get_saddr(net, dev, daddr, srcprefs, saddr);
#endif
}
#endif

unsigned int nf_nat_fullcone_ipv6(struct sk_buff *skb, unsigned int hooknum,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				  struct nf_nat_range2 *range,
#else
				  struct nf_nat_range *range,
#endif
				  const struct net_device *out)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	struct nf_nat_range2 newrange;
#else
	struct nf_nat_range newrange;
#endif

	WARN_ON(!(hooknum == NF_INET_POST_ROUTING || hooknum == NF_INET_PRE_ROUTING));

	memset(&newrange.min_addr, 0, sizeof(newrange.min_addr));
	memset(&newrange.max_addr, 0, sizeof(newrange.max_addr));
	newrange.flags = range->flags | NF_NAT_RANGE_MAP_IPS;
	newrange.min_proto = range->min_proto;
	newrange.max_proto = range->max_proto;

	switch (hooknum) {
	case NF_INET_PRE_ROUTING:
		/* inbound packets */
		return nf_nat_handle_prerouting(NFPROTO_IPV6, skb, hooknum, &newrange);
	case NF_INET_POST_ROUTING:
		/* outbound packets */
		return nf_nat_handle_postrouting(NFPROTO_IPV6, skb, hooknum, range, &newrange, out);
	}

	WARN_ON(1);
	// logical error
	return 5;
}

EXPORT_SYMBOL_GPL(nf_nat_fullcone_ipv6);