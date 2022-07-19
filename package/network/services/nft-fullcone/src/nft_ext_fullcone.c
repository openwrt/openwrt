// SPDX-License-Identifier: GPL-2.0-only
/*
 * Nftables NAT extension: fullcone expression support
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
#define NF_FULLCONE_WORKQUEUE_NAME "fullcone " KBUILD_MODNAME ": wq"

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables.h>
#include <net/netfilter/nf_nat.h>

#include <linux/workqueue.h>

#include "nf_nat_fullcone.h"

static void nft_fullcone_set_regs(const struct nft_expr *expr, const struct nft_regs *regs,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				  struct nf_nat_range2 *range);
#else
				  struct nf_nat_range *range);
#endif

#ifdef CONFIG_NF_CONNTRACK_CHAIN_EVENTS
struct notifier_block ct_event_notifier;
#else
struct nf_ct_event_notifier ct_event_notifier;
#endif
static DEFINE_MUTEX(nf_ct_net_event_lock);
int ct_event_notifier_registered = 0;

int module_refer_count = 0;

static void gc_worker(struct work_struct *work);
static struct workqueue_struct *wq __read_mostly = NULL;
static DECLARE_DELAYED_WORK(gc_worker_wk, gc_worker);

static void gc_worker(struct work_struct *work)
{
	nf_nat_fullcone_handle_dying_tuples();
}

struct nft_fullcone {
	u32 flags;
	u8 sreg_proto_min;
	u8 sreg_proto_max;
};

static const struct nla_policy nft_fullcone_policy[NFTA_FULLCONE_MAX + 1] = {
	[NFTA_FULLCONE_FLAGS] = {.type = NLA_U32 },
	[NFTA_FULLCONE_REG_PROTO_MIN] = {.type = NLA_U32 },
	[NFTA_FULLCONE_REG_PROTO_MAX] = {.type = NLA_U32 },
};

/* conntrack destroy event callback function */
#ifdef CONFIG_NF_CONNTRACK_CHAIN_EVENTS
static int ct_event_cb(struct notifier_block *this, unsigned long events, void *ptr)
{
	struct nf_ct_event *item = ptr;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
static int ct_event_cb(unsigned int events, const struct nf_ct_event *item)
{
#else
static int ct_event_cb(unsigned int events, struct nf_ct_event *item)
{
#endif
	struct nf_conn *ct;
	struct nf_conntrack_tuple *ct_tuple_reply, *ct_tuple_original;
	uint8_t protonum;
	struct tuple_list *dying_tuple_item;

	ct = item->ct;
	/* we handle only conntrack destroy events */
	if (ct == NULL || !(events & (1 << IPCT_DESTROY))) {
		return 0;
	}

	ct_tuple_original = &(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);

	ct_tuple_reply = &(ct->tuplehash[IP_CT_DIR_REPLY].tuple);

	protonum = (ct_tuple_original->dst).protonum;
	if (protonum != IPPROTO_UDP) {
		return 0;
	}

	dying_tuple_item = kmalloc(sizeof(struct tuple_list), GFP_ATOMIC);

	if (dying_tuple_item == NULL) {
		pr_debug("warning: ct_event_cb(): kmalloc failed.\n");
		return 0;
	}

	memcpy(&(dying_tuple_item->tuple_original), ct_tuple_original, sizeof(struct nf_conntrack_tuple));
	memcpy(&(dying_tuple_item->tuple_reply), ct_tuple_reply, sizeof(struct nf_conntrack_tuple));

	nf_nat_fullcone_dying_tuple_list_add(&(dying_tuple_item->list));

	if (wq != NULL)
		queue_delayed_work(wq, &gc_worker_wk, msecs_to_jiffies(100));

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0) && !defined(CONFIG_NF_CONNTRACK_CHAIN_EVENTS)
static int exp_event_cb(unsigned int events, const struct nf_exp_event *item)
{
	return 0;
}
#endif

static int nft_fullcone_validate(const struct nft_ctx *ctx, const struct nft_expr *expr, const struct nft_data **data)
{
	int err;

	err = nft_chain_validate_dependency(ctx->chain, NFT_CHAIN_T_NAT);
	if (err < 0)
		return err;

	// TODO: check hooks
	return nft_chain_validate_hooks(ctx->chain, (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_POST_ROUTING));
}

static int nft_fullcone_init(const struct nft_ctx *ctx, const struct nft_expr *expr, const struct nlattr *const tb[])
{
	int err;
	int register_ct_notifier_ret = 0;

	err = nf_ct_netns_get(ctx->net, ctx->family);

	mutex_lock(&nf_ct_net_event_lock);

	module_refer_count++;

	pr_debug("nft_fullcone_init(): module_refer_count is now %d\n", module_refer_count);

	if (module_refer_count == 1) {
#ifdef CONFIG_NF_CONNTRACK_CHAIN_EVENTS
		ct_event_notifier.notifier_call = ct_event_cb;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
		ct_event_notifier.ct_event = ct_event_cb;
		ct_event_notifier.exp_event = exp_event_cb;
#else
		ct_event_notifier.fcn = ct_event_cb;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0) && !defined(CONFIG_NF_CONNTRACK_CHAIN_EVENTS)
		if (!READ_ONCE(ctx->net->ct.nf_conntrack_event_cb)) {
			nf_conntrack_register_notifier(ctx->net, &ct_event_notifier);
		}
#else
		register_ct_notifier_ret = nf_conntrack_register_notifier(ctx->net, &ct_event_notifier);
#endif

		if (register_ct_notifier_ret) {
			/* non-zero means failure */
			pr_warn("failed to register a conntrack notifier. Disable active GC for mappings.\n");
		} else {
			ct_event_notifier_registered = 1;
			pr_debug("nft_fullcone_init(): ct_event_notifier registered\n");
		}

	}

	mutex_unlock(&nf_ct_net_event_lock);

	return err;
}

static int nft_fullcone_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_fullcone *priv = nft_expr_priv(expr);

	if (priv->flags != 0 && nla_put_be32(skb, NFTA_FULLCONE_FLAGS, htonl(priv->flags)))
		goto nla_put_failure;

	if (priv->sreg_proto_min) {
		if (nft_dump_register(skb, NFTA_FULLCONE_REG_PROTO_MIN,
				      priv->sreg_proto_min) ||
		    nft_dump_register(skb, NFTA_FULLCONE_REG_PROTO_MAX, priv->sreg_proto_max))
			goto nla_put_failure;
	}

	return 0;

nla_put_failure:
	return -1;
}

/* nft_fullcone_set_regs sets nft_regs from nft_expr fullcone specific private data */
static void nft_fullcone_set_regs(const struct nft_expr *expr, const struct nft_regs *regs,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
				  struct nf_nat_range2 *range
#else
				  struct nf_nat_range *range
#endif
    )
{
	// private data connected via nft_expr_type.ops <==> nft_expr_ops.type
	// private data type from nft_expr_type.{policy,maxattr,ops}
	// private data size from nft_expr_ops.size
	struct nft_fullcone *priv = nft_expr_priv(expr);
	range->flags = priv->flags;
	if (priv->sreg_proto_min) {
		range->min_proto.all = (__force __be16)
		    nft_reg_load16(&regs->data[priv->sreg_proto_min]);
		range->max_proto.all = (__force __be16)
		    nft_reg_load16(&regs->data[priv->sreg_proto_max]);
	}
}

static void nft_fullcone_ipv4_eval(const struct nft_expr *expr, struct nft_regs *regs, const struct nft_pktinfo *pkt)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	struct nf_nat_range2 range;
#else
	struct nf_nat_range range;
#endif

	memset(&range, 0, sizeof(range));
	nft_fullcone_set_regs(expr, regs, &range);
	regs->verdict.code = nf_nat_fullcone_ipv4(pkt->skb, nft_hook(pkt), &range, nft_out(pkt));
}

static void nft_fullcone_common_destory(const struct nft_ctx *ctx)
{
	mutex_lock(&nf_ct_net_event_lock);

	module_refer_count--;

	pr_debug("nft_fullcone_common_destory(): module_refer_count is now %d\n", module_refer_count);

	if (module_refer_count == 0) {
		if (ct_event_notifier_registered) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0) && !defined(CONFIG_NF_CONNTRACK_CHAIN_EVENTS)
			nf_conntrack_unregister_notifier(ctx->net);
#else
			nf_conntrack_unregister_notifier(ctx->net, &ct_event_notifier);
#endif
			ct_event_notifier_registered = 0;

			pr_debug("nft_fullcone_common_destory(): ct_event_notifier unregistered\n");

		}
	}

	mutex_unlock(&nf_ct_net_event_lock);
}

static void nft_fullcone_ipv4_destroy(const struct nft_ctx *ctx, const struct nft_expr *expr)
{
	nft_fullcone_common_destory(ctx);
	nf_ct_netns_put(ctx->net, NFPROTO_IPV4);
}

static struct nft_expr_type nft_fullcone_ipv4_type;
static const struct nft_expr_ops nft_fullcone_ipv4_ops = {
	.type = &nft_fullcone_ipv4_type,
	.size = NFT_EXPR_SIZE(sizeof(struct nft_fullcone)),
	.eval = nft_fullcone_ipv4_eval,
	.init = nft_fullcone_init,
	.destroy = nft_fullcone_ipv4_destroy,
	.dump = nft_fullcone_dump,
	.validate = nft_fullcone_validate,
};

static struct nft_expr_type nft_fullcone_ipv4_type __read_mostly = {
	.family = NFPROTO_IPV4,
	.name = "fullcone",
	.ops = &nft_fullcone_ipv4_ops,
	.policy = nft_fullcone_policy,
	.maxattr = NFTA_FULLCONE_MAX,
	.owner = THIS_MODULE,
};

#ifdef CONFIG_NF_TABLES_IPV6
static void nft_fullcone_ipv6_eval(const struct nft_expr *expr, struct nft_regs *regs, const struct nft_pktinfo *pkt)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
	struct nf_nat_range2 range;
#else
	struct nf_nat_range range;
#endif

	memset(&range, 0, sizeof(range));
	nft_fullcone_set_regs(expr, regs, &range);
	regs->verdict.code = nf_nat_fullcone_ipv6(pkt->skb, nft_hook(pkt), &range, nft_out(pkt));
}

static void nft_fullcone_ipv6_destroy(const struct nft_ctx *ctx, const struct nft_expr *expr)
{
	nft_fullcone_common_destory(ctx);
	nf_ct_netns_put(ctx->net, NFPROTO_IPV6);
}

static struct nft_expr_type nft_fullcone_ipv6_type;
static const struct nft_expr_ops nft_fullcone_ipv6_ops = {
	.type = &nft_fullcone_ipv6_type,
	.size = NFT_EXPR_SIZE(sizeof(struct nft_fullcone)),
	.eval = nft_fullcone_ipv6_eval,
	.init = nft_fullcone_init,
	.destroy = nft_fullcone_ipv6_destroy,
	.dump = nft_fullcone_dump,
	.validate = nft_fullcone_validate,
};

static struct nft_expr_type nft_fullcone_ipv6_type __read_mostly = {
	.family = NFPROTO_IPV6,
	.name = "fullcone",
	.ops = &nft_fullcone_ipv6_ops,
	.policy = nft_fullcone_policy,
	.maxattr = NFTA_FULLCONE_MAX,
	.owner = THIS_MODULE,
};

static int __init nft_fullcone_module_init_ipv6(void)
{
	return nft_register_expr(&nft_fullcone_ipv6_type);
}

static void nft_fullcone_module_exit_ipv6(void)
{
	nft_unregister_expr(&nft_fullcone_ipv6_type);
}
#else
static inline int nft_fullcone_module_init_ipv6(void)
{
	return 0;
}

static inline void nft_fullcone_module_exit_ipv6(void)
{
}
#endif

#ifdef CONFIG_NF_TABLES_INET
static void nft_fullcone_inet_eval(const struct nft_expr *expr, struct nft_regs *regs, const struct nft_pktinfo *pkt)
{
	switch (nft_pf(pkt)) {
	case NFPROTO_IPV4:
		return nft_fullcone_ipv4_eval(expr, regs, pkt);
	case NFPROTO_IPV6:
		return nft_fullcone_ipv6_eval(expr, regs, pkt);
	}

	WARN_ON_ONCE(1);
}

static void nft_fullcone_inet_destroy(const struct nft_ctx *ctx, const struct nft_expr *expr)
{
	nft_fullcone_common_destory(ctx);
	nf_ct_netns_put(ctx->net, NFPROTO_INET);
}

static struct nft_expr_type nft_fullcone_inet_type;
static const struct nft_expr_ops nft_fullcone_inet_ops = {
	.type = &nft_fullcone_inet_type,
	.size = NFT_EXPR_SIZE(sizeof(struct nft_fullcone)),
	.eval = nft_fullcone_inet_eval,
	.init = nft_fullcone_init,
	.destroy = nft_fullcone_inet_destroy,
	.dump = nft_fullcone_dump,
	.validate = nft_fullcone_validate,
};

static struct nft_expr_type nft_fullcone_inet_type __read_mostly = {
	.family = NFPROTO_INET,
	.name = "fullcone",
	.ops = &nft_fullcone_inet_ops,
	.policy = nft_fullcone_policy,
	.maxattr = NFTA_FULLCONE_MAX,
	.owner = THIS_MODULE,
};

static int __init nft_fullcone_module_init_inet(void)
{
	return nft_register_expr(&nft_fullcone_inet_type);
}

static void nft_fullcone_module_exit_inet(void)
{
	nft_unregister_expr(&nft_fullcone_inet_type);
}
#else
static inline int nft_fullcone_module_init_inet(void)
{
	return 0;
}

static inline void nft_fullcone_module_exit_inet(void)
{
}
#endif

static int __init nft_fullcone_module_init(void)
{
	int ret;

	ret = nft_fullcone_module_init_ipv6();
	if (ret < 0)
		return ret;

	ret = nft_fullcone_module_init_inet();
	if (ret < 0) {
		nft_fullcone_module_exit_ipv6();
		return ret;
	}

	ret = nft_register_expr(&nft_fullcone_ipv4_type);
	if (ret < 0) {
		nft_fullcone_module_exit_inet();
		nft_fullcone_module_exit_ipv6();
		return ret;
	}

	wq = create_singlethread_workqueue(NF_FULLCONE_WORKQUEUE_NAME);
	if (wq == NULL) {
		pr_err("failed to create workqueue %s\n", NF_FULLCONE_WORKQUEUE_NAME);
	}

	return ret;
}

static void __exit nft_fullcone_module_exit(void)
{
	nft_fullcone_module_exit_ipv6();
	nft_fullcone_module_exit_inet();
	nft_unregister_expr(&nft_fullcone_ipv4_type);

	if (wq) {
		cancel_delayed_work_sync(&gc_worker_wk);
		flush_workqueue(wq);
		destroy_workqueue(wq);
	}

	nf_nat_fullcone_handle_dying_tuples();
	nf_nat_fullcone_destroy_mappings();
}

module_init(nft_fullcone_module_init);
module_exit(nft_fullcone_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Syrone Wong <wong.syrone@gmail.com>");
MODULE_ALIAS_NFT_EXPR("fullcone");
MODULE_DESCRIPTION("Netfilter nftables fullcone expression support of RFC3489 full cone NAT");