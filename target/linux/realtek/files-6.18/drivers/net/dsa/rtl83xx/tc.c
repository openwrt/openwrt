// SPDX-License-Identifier: GPL-2.0-only

#include <net/dsa.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/math64.h>
#include <linux/netdevice.h>
#include <linux/regmap.h>
#include <net/flow_offload.h>
#include <linux/rhashtable.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "pie.h"
#include "qos.h"
#include "rtl-otto.h"
#include "stats.h"
#include "tc.h"

struct rtl83xx_flow {
	unsigned long cookie;
	struct rhash_head node;
	struct rcu_head rcu_head;
	struct otto_dsa_priv *priv;
	struct pie_rule rule;
	u32 flags;
};

#define RTL930X_BANDWIDTH_CTRL_EGRESS(port)	(0x7660 + (port * 16))
#define RTL930X_BANDWIDTH_CTRL_INGRESS(port)	(0x8068 + (port * 4))
#define RTL930X_BANDWIDTH_CTRL_QUEUE_SET0(port, queue) \
						(0x3C60 + ((port) * 384) + ((queue) * 8))
#define RTL930X_BANDWIDTH_CTRL_QUEUE_SET1(port, queue) \
						(0xE300 + (((port) - 24) * 96) + ((queue) * 8))
#define RTL930X_EGBW_LB_CTRL			0x78EC
#define RTL930X_EGBW_LB_CTRL_TOKEN		GENMASK(31, 16)
#define RTL930X_BANDWIDTH_CTRL_INGRESS_BURST_HIGH_ON(port) \
						(0x80DC + (port * 8))
#define RTL930X_BANDWIDTH_CTRL_INGRESS_BURST_HIGH_OFF(port) \
						(0x80E0 + (port * 8))
#define RTL930X_BANDWIDTH_CTRL_INGRESS_BURST_MAX \
						GENMASK(30, 0)

#define RTL931X_BANDWIDTH_CTRL_EGRESS(port)	(0x2164 + (port * 8))
#define RTL931X_BANDWIDTH_CTRL_INGRESS(port)	(0xe008 + (port * 8))

#define RTL93XX_BANDWIDTH_CTRL_RATE_MAX		GENMASK(19, 0)
#define RTL93XX_BANDWIDTH_CTRL_ENABLE		BIT(20)
#define RTL931X_BANDWIDTH_CTRL_MAX_BURST	GENMASK(15, 0)

#define RTL930X_INGRESS_FC_CTRL(port)		(0x81CC + ((port / 29) * 4))
#define RTL930X_INGRESS_FC_CTRL_EN(port)	BIT(port % 29)

/* Parse the flow rule for the matching conditions */
static int otto_tc_parse_flow_rule(struct otto_dsa_priv *priv,
				   struct flow_rule *rule, struct rtl83xx_flow *flow)
{
	struct flow_dissector *dissector = rule->match.dissector;
	struct flow_match_control control;
	u64 supported_keys;

	dev_dbg(priv->dev, "parsing flower match keys\n");
	/* KEY_CONTROL and KEY_BASIC are needed for forming a meaningful key */
	if ((dissector->used_keys & BIT(FLOW_DISSECTOR_KEY_CONTROL)) == 0 ||
	    (dissector->used_keys & BIT(FLOW_DISSECTOR_KEY_BASIC)) == 0) {
		dev_err(priv->dev, "cannot form a TC key: used_keys = 0x%llx\n", dissector->used_keys);
		return -EOPNOTSUPP;
	}

	supported_keys = BIT_ULL(FLOW_DISSECTOR_KEY_CONTROL) |
			 BIT_ULL(FLOW_DISSECTOR_KEY_BASIC) |
			 BIT_ULL(FLOW_DISSECTOR_KEY_ETH_ADDRS) |
			 BIT_ULL(FLOW_DISSECTOR_KEY_VLAN) |
			 BIT_ULL(FLOW_DISSECTOR_KEY_IPV4_ADDRS) |
			 BIT_ULL(FLOW_DISSECTOR_KEY_IPV6_ADDRS) |
			 BIT_ULL(FLOW_DISSECTOR_KEY_PORTS);
	if (dissector->used_keys & ~supported_keys) {
		dev_err(priv->dev, "unsupported TC keys: used_keys = 0x%llx\n",
			dissector->used_keys & ~supported_keys);
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		dev_dbg(priv->dev, "match BASIC\n");
		flow_rule_match_basic(rule, &match);
		if (match.mask->n_proto) {
			if (match.mask->n_proto != htons(0xffff))
				return -EOPNOTSUPP;

			if (match.key->n_proto == htons(ETH_P_ARP)) {
				flow->rule.frame_type = 0;
			} else if (match.key->n_proto == htons(ETH_P_IP)) {
				flow->rule.frame_type = 2;
			} else if (match.key->n_proto == htons(ETH_P_IPV6)) {
				flow->rule.frame_type = 3;
			} else {
				flow->rule.frame_type = 1;
				flow->rule.ethertype = ntohs(match.key->n_proto);
				flow->rule.ethertype_m = ntohs(match.mask->n_proto);
			}
			flow->rule.frame_type_m = 3;
		}
		if (match.mask->ip_proto) {
			if (flow->rule.frame_type < 2 || match.mask->ip_proto != 0xff)
				return -EOPNOTSUPP;

			switch (match.key->ip_proto) {
			case IPPROTO_UDP:
				flow->rule.frame_type_l4 = 0;
				break;
			case IPPROTO_TCP:
				flow->rule.frame_type_l4 = 1;
				break;
			case IPPROTO_ICMP:
			case IPPROTO_ICMPV6:
				flow->rule.frame_type_l4 = 2;
				break;
			case IPPROTO_IGMP:
				flow->rule.frame_type_l4 = 3;
				break;
			default:
				return -EOPNOTSUPP;
			}
			flow->rule.frame_type_l4_m = 7;
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct flow_match_eth_addrs match;

		dev_dbg(priv->dev, "match ETH_ADDR\n");
		flow_rule_match_eth_addrs(rule, &match);
		ether_addr_copy(flow->rule.dmac, match.key->dst);
		ether_addr_copy(flow->rule.dmac_m, match.mask->dst);
		ether_addr_copy(flow->rule.smac, match.key->src);
		ether_addr_copy(flow->rule.smac_m, match.mask->src);
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_match_vlan match;

		dev_dbg(priv->dev, "match VLAN\n");
		flow_rule_match_vlan(rule, &match);
		if (match.mask->vlan_priority || match.mask->vlan_dei ||
		    match.mask->vlan_eth_type)
			return -EOPNOTSUPP;
		/* cls_flower sets a full vlan_tpid mask for every VLAN-ethertype
		 * rule; only reject a TPID the PIE cannot match on.
		 */
		if (match.mask->vlan_tpid &&
		    match.key->vlan_tpid != htons(ETH_P_8021Q))
			return -EOPNOTSUPP;
		flow->rule.itag = match.key->vlan_id;
		flow->rule.itag_m = match.mask->vlan_id;
	}

	/* cls_flower keeps IPv4 and IPv6 addresses in one union, so an IPv4
	 * rule shows the IPv6 key in use too: the address type tells which
	 * one the rule holds, the key whether it matches on it at all.
	 */
	flow_rule_match_control(rule, &control);
	if (control.key->addr_type == FLOW_DISSECTOR_KEY_IPV4_ADDRS &&
	    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS)) {
		struct flow_match_ipv4_addrs match;

		dev_dbg(priv->dev, "match IPV4\n");
		flow_rule_match_ipv4_addrs(rule, &match);
		flow->rule.is_ipv6 = false;
		flow->rule.dip = match.key->dst;
		flow->rule.dip_m = match.mask->dst;
		flow->rule.sip = match.key->src;
		flow->rule.sip_m = match.mask->src;
	} else if (control.key->addr_type == FLOW_DISSECTOR_KEY_IPV6_ADDRS &&
		   flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS)) {
		struct flow_match_ipv6_addrs match;

		dev_dbg(priv->dev, "match IPV6\n");
		flow->rule.is_ipv6 = true;
		flow_rule_match_ipv6_addrs(rule, &match);
		flow->rule.dip6 = match.key->dst;
		flow->rule.dip6_m = match.mask->dst;
		flow->rule.sip6 = match.key->src;
		flow->rule.sip6_m = match.mask->src;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS)) {
		struct flow_match_ports match;

		dev_dbg(priv->dev, "match PORTS\n");
		flow_rule_match_ports(rule, &match);
		flow->rule.dport = match.key->dst;
		flow->rule.dport_m = match.mask->dst;
		flow->rule.sport = match.key->src;
		flow->rule.sport_m = match.mask->src;
	}

	/* TODO: ICMP */
	return 0;
}

static void otto_tc_flow_bypass_all(struct rtl83xx_flow *flow)
{
	flow->rule.bypass_sel = true;
	flow->rule.bypass_all = true;
	flow->rule.bypass_igr_stp = true;
	flow->rule.bypass_ibc_sc = true;
}

static int otto_tc_validate_flow_actions(struct flow_rule *rule)
{
	const struct flow_action_entry *act;
	bool drop = false, fwd = false;
	int i, count = 0;

	flow_action_for_each(i, act, &rule->action) {
		count++;
		switch (act->id) {
		case FLOW_ACTION_DROP:
			drop = true;
			break;
		case FLOW_ACTION_TRAP:
		case FLOW_ACTION_REDIRECT:
		case FLOW_ACTION_MIRRED:
			if (fwd)
				return -EOPNOTSUPP;
			fwd = true;
			break;
		default:
			/* FLOW_ACTION_VLAN_PUSH / _POP map to the ivid/ovid PIE
			 * action fields via the translation in otto_tc_add_flow(),
			 * which predates this offload and has never been exercised
			 * through it. Keep them - and every other action - rejected
			 * until that path is reviewed.
			 */
			return -EOPNOTSUPP;
		}
	}

	if (!count || (drop && count != 1))
		return -EOPNOTSUPP;

	return 0;
}

static int otto_tc_parse_fwd(struct otto_dsa_priv *priv,
			     const struct flow_action_entry *act, struct rtl83xx_flow *flow)
{
	struct net_device *dev = act->dev;
	int port;

	port = rtl83xx_port_is_under(dev, priv);
	if (port < 0) {
		netdev_info(dev, "not a DSA port on this switch\n");
		return -EINVAL;
	}

	flow->rule.fwd_sel = true;
	flow->rule.fwd_data = port;
	dev_dbg(priv->dev, "redirect/mirror to port %d\n", port);
	otto_tc_flow_bypass_all(flow);

	return 0;
}

static int otto_tc_add_flow(struct otto_dsa_priv *priv, struct flow_cls_offload *f,
			    struct rtl83xx_flow *flow)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	const struct flow_action_entry *act;
	int i, err, port;

	dev_dbg(priv->dev, "adding flower rule\n");

	if (flow_rule_match_has_control_flags(rule, f->common.extack))
		return -EOPNOTSUPP;

	if (!flow_action_hw_stats_check(&rule->action, f->common.extack,
					FLOW_ACTION_HW_STATS_IMMEDIATE_BIT))
		return -EOPNOTSUPP;

	err = otto_tc_validate_flow_actions(rule);
	if (err)
		return err;

	err = otto_tc_parse_flow_rule(priv, rule, flow);
	if (err)
		return err;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_DROP:
			dev_dbg(priv->dev, "action DROP\n");
			flow->rule.drop = true;
			otto_tc_flow_bypass_all(flow);
			return 0;

		case FLOW_ACTION_TRAP:
			dev_dbg(priv->dev, "action TRAP\n");
			flow->rule.fwd_sel = true;
			flow->rule.fwd_data = priv->r->cpu_port;
			flow->rule.fwd_act = PIE_ACT_REDIRECT_TO_PORT;
			otto_tc_flow_bypass_all(flow);
			break;

		case FLOW_ACTION_MANGLE:
			dev_err(priv->dev, "unsupported action: MANGLE\n");
			return -EOPNOTSUPP;

		case FLOW_ACTION_ADD:
			dev_err(priv->dev, "unsupported action: ADD\n");
			return -EOPNOTSUPP;

		case FLOW_ACTION_VLAN_PUSH:
			dev_dbg(priv->dev, "action VLAN_PUSH\n");
/*			TODO: act->vlan.proto */
			flow->rule.ivid_act = PIE_ACT_VID_ASSIGN;
			flow->rule.ivid_sel = true;
			flow->rule.ivid_data = htons(act->vlan.vid);
			flow->rule.ovid_act = PIE_ACT_VID_ASSIGN;
			flow->rule.ovid_sel = true;
			flow->rule.ovid_data = htons(act->vlan.vid);
			flow->rule.fwd_mod_to_cpu = true;
			break;

		case FLOW_ACTION_VLAN_POP:
			dev_dbg(priv->dev, "action VLAN_POP\n");
			flow->rule.ivid_act = PIE_ACT_VID_ASSIGN;
			flow->rule.ivid_data = 0;
			flow->rule.ivid_sel = true;
			flow->rule.ovid_act = PIE_ACT_VID_ASSIGN;
			flow->rule.ovid_data = 0;
			flow->rule.ovid_sel = true;
			flow->rule.fwd_mod_to_cpu = true;
			break;

		case FLOW_ACTION_CSUM:
			dev_err(priv->dev, "unsupported action: CSUM\n");
			return -EOPNOTSUPP;

		case FLOW_ACTION_REDIRECT:
			dev_dbg(priv->dev, "action REDIRECT\n");
			err = otto_tc_parse_fwd(priv, act, flow);
			if (err)
				return err;
			flow->rule.fwd_act = PIE_ACT_REDIRECT_TO_PORT;
			break;

		case FLOW_ACTION_MIRRED:
			dev_dbg(priv->dev, "action MIRRED\n");
			if (!priv->r->pie_mirror_group) {
				err = otto_tc_parse_fwd(priv, act, flow);
				if (err)
					return err;
				flow->rule.fwd_act = PIE_ACT_COPY_TO_PORT;
				break;
			}
			port = rtl83xx_port_is_under(act->dev, priv);
			if (port < 0) {
				netdev_info(act->dev, "not a DSA port on this switch\n");
				return -EINVAL;
			}
			/* A copy through the forwarding action would take over the
			 * forwarding decision of the L3 rules in later blocks.
			 */
			err = rtldsa_mirror_group_get(priv, port);
			if (err < 0) {
				if (err == -ENOSPC)
					NL_SET_ERR_MSG_MOD(f->common.extack,
							   "all four mirror groups are in use");
				return err;
			}
			flow->rule.mir_sel = true;
			flow->rule.mir_data = err;
			break;

		default:
			dev_err(priv->dev, "unsupported action: %d\n", act->id);
			return -EOPNOTSUPP;
		}
	}

	return 0;
}

static const struct rhashtable_params tc_ht_params = {
	.head_offset = offsetof(struct rtl83xx_flow, node),
	.key_offset = offsetof(struct rtl83xx_flow, cookie),
	.key_len = sizeof(((struct rtl83xx_flow *)0)->cookie),
	.automatic_shrinking = true,
};

int otto_tc_init(struct otto_dsa_priv *priv)
{
	int err;

	if (priv->tc_initialized)
		return 0;

	err = rhashtable_init(&priv->tc_ht, &tc_ht_params);
	if (err)
		return err;

	mutex_init(&priv->tc_flow_lock);
	priv->tc_initialized = true;

	return 0;
}

/* Zero a PIE rule's LOG-table packet counter */
static void otto_tc_packet_cntr_clear(struct otto_dsa_priv *priv, int counter)
{
	if (counter < 0 || !priv->r->packet_cntr_clear)
		return;

	mutex_lock(&priv->reg_mutex);
	priv->r->packet_cntr_clear(priv, counter);
	mutex_unlock(&priv->reg_mutex);
}

/* A flow's counter on its way out: zeroed, and handed back to the allocator
 * where it came from there. On RTL930x the counter id is the PIE rule id
 * itself: the rule owns it and pie_rule_rm() releases it.
 */
static void otto_tc_packet_cntr_put(struct otto_dsa_priv *priv, int counter)
{
	otto_tc_packet_cntr_clear(priv, counter);
	if (!priv->r->pie_rule_id_is_log_counter)
		otto_stats_packet_cntr_free(priv, counter);
}

static void otto_tc_flow_free(void *ptr, void *arg)
{
	struct rtl83xx_flow *flow = ptr;
	struct otto_dsa_priv *priv = arg;

	priv->r->pie_rule_rm(priv, &flow->rule);
	otto_tc_packet_cntr_put(priv, flow->rule.packet_cntr);
	if (flow->rule.mir_sel)
		rtldsa_mirror_group_put(priv, flow->rule.mir_data);

	/* Readers may still hold an RCU-protected reference after the
	 * object has been removed from the hash table.
	 */
	kfree_rcu(flow, rcu_head);
}

void otto_tc_cleanup(struct otto_dsa_priv *priv)
{
	if (!priv->tc_initialized)
		return;

	/* Hold tc_flow_lock like the add/del/stats callbacks do: any
	 * callback that slipped in before teardown must finish before the
	 * table and rules are torn down, otherwise it can walk a half-freed
	 * flow or run pie_rule_rm() against a rule this path already removed.
	 */
	mutex_lock(&priv->tc_flow_lock);
	rhashtable_free_and_destroy(&priv->tc_ht, otto_tc_flow_free, priv);
	priv->tc_initialized = false;
	mutex_unlock(&priv->tc_flow_lock);

	rcu_barrier();
	mutex_destroy(&priv->tc_flow_lock);
}

static int otto_tc_configure_flower(struct otto_dsa_priv *priv,
				    struct flow_cls_offload *f, int ingress_port)
{
	struct rtl83xx_flow *flow;
	int err = 0;

	dev_dbg(priv->dev, "configuring flower rule\n");

	if (!priv->r->packet_cntr_read || !priv->r->packet_cntr_clear)
		return -EOPNOTSUPP;

	dev_dbg(priv->dev, "cookie %08lx\n", f->cookie);

	mutex_lock(&priv->tc_flow_lock);

	/* otto_tc_cleanup() clears this under the lock before it destroys
	 * tc_ht; a callback that was parked on the lock must bail rather than
	 * walk the freed table.
	 */
	if (!priv->tc_initialized) {
		err = -ENODEV;
		goto out_unlock;
	}

	flow = rhashtable_lookup_fast(&priv->tc_ht, &f->cookie, tc_ht_params);
	if (flow) {
		dev_dbg(priv->dev, "cookie already offloaded\n");
		err = -EEXIST;
		goto out_unlock;
	}
	dev_dbg(priv->dev, "new flow\n");

	flow = kzalloc(sizeof(*flow), GFP_KERNEL);
	if (!flow) {
		err = -ENOMEM;
		goto out_unlock;
	}

	flow->cookie = f->cookie;
	flow->priv = priv;
	/* kzalloc() leaves this at 0, a valid counter id; -1 means "none". */
	flow->rule.packet_cntr = -1;

	err = rhashtable_insert_fast(&priv->tc_ht, &flow->node, tc_ht_params);
	if (err) {
		dev_err(priv->dev, "could not insert new rule\n");
		goto out_free;
	}

	err = otto_tc_add_flow(priv, f, flow);
	if (err)
		goto out_remove;

	flow->rule.spn = ingress_port;
	flow->rule.spn_m = 0x7f;

	/* Where the PIE rule ID is also the LOG table counter ID (RTL930x), the
	 * counter is implied by the rule and only known once pie_rule_add() has
	 * assigned the ID. That range is kept out of otto_stats_packet_cntr_alloc(),
	 * so the implied LOG entry cannot alias an L3 route PIE rule's counter.
	 * Elsewhere the rule logs into a counter of its own from the allocator.
	 */
	flow->rule.log_sel = true;
	if (!priv->r->pie_rule_id_is_log_counter) {
		flow->rule.packet_cntr = otto_stats_packet_cntr_alloc(priv);
		if (flow->rule.packet_cntr < 0) {
			err = -ENOSPC;
			goto out_remove;
		}
		flow->rule.log_data = flow->rule.packet_cntr;
	}

	err = priv->r->pie_rule_add(priv, &flow->rule);
	if (err)
		goto out_remove;

	if (priv->r->pie_rule_id_is_log_counter)
		flow->rule.packet_cntr = flow->rule.id;
	dev_dbg(priv->dev, "using PIE rule counter %d\n", flow->rule.packet_cntr);
	otto_tc_packet_cntr_clear(priv, flow->rule.packet_cntr);

	mutex_unlock(&priv->tc_flow_lock);
	return 0;

out_remove:
	rhashtable_remove_fast(&priv->tc_ht, &flow->node, tc_ht_params);
	otto_tc_packet_cntr_put(priv, flow->rule.packet_cntr);
	if (flow->rule.mir_sel)
		rtldsa_mirror_group_put(priv, flow->rule.mir_data);
	/* published in tc_ht above; a concurrent reader may still hold a ref */
	kfree_rcu(flow, rcu_head);
	goto out_err;
out_free:
	kfree(flow);
out_err:
	dev_err(priv->dev, "flower rule setup failed: %d\n", err);
out_unlock:
	mutex_unlock(&priv->tc_flow_lock);

	return err;
}

static int otto_tc_delete_flower(struct otto_dsa_priv *priv,
				 struct flow_cls_offload *cls_flower)
{
	struct rtl83xx_flow *flow;
	int err;

	dev_dbg(priv->dev, "deleting flower rule\n");

	mutex_lock(&priv->tc_flow_lock);

	/* see otto_tc_configure_flower(): bail if teardown already ran */
	if (!priv->tc_initialized) {
		err = -ENODEV;
		goto out_unlock;
	}

	flow = rhashtable_lookup_fast(&priv->tc_ht, &cls_flower->cookie, tc_ht_params);
	if (!flow) {
		err = -ENOENT;
		goto out_unlock;
	}

	err = rhashtable_remove_fast(&priv->tc_ht, &flow->node, tc_ht_params);
	if (err)
		goto out_unlock;

	priv->r->pie_rule_rm(priv, &flow->rule);
	otto_tc_packet_cntr_put(priv, flow->rule.packet_cntr);
	if (flow->rule.mir_sel)
		rtldsa_mirror_group_put(priv, flow->rule.mir_data);

	kfree_rcu(flow, rcu_head);

out_unlock:
	mutex_unlock(&priv->tc_flow_lock);

	return err;
}

static int otto_tc_stats_flower(struct otto_dsa_priv *priv,
				struct flow_cls_offload *cls_flower)
{
	struct rtl83xx_flow *flow;
	unsigned long lastused = 0;
	u32 total_packets, new_packets = 0;
	int err = 0;

	dev_dbg(priv->dev, "reading flower rule stats\n");

	mutex_lock(&priv->tc_flow_lock);

	/* see otto_tc_configure_flower(): bail if teardown already ran */
	if (!priv->tc_initialized) {
		err = -ENODEV;
		goto out_unlock;
	}

	flow = rhashtable_lookup_fast(&priv->tc_ht, &cls_flower->cookie, tc_ht_params);
	if (!flow) {
		err = -ENOENT;
		goto out_unlock;
	}

	/* tc_flow_lock keeps the flow alive for the duration of the sleeping
	 * counter read, so it is safe to dereference it here.
	 */
	if (flow->rule.packet_cntr >= 0) {
		mutex_lock(&priv->reg_mutex);
		total_packets = priv->r->packet_cntr_read(priv, flow->rule.packet_cntr);
		mutex_unlock(&priv->reg_mutex);
		dev_dbg(priv->dev, "total packets: %u\n", total_packets);

		new_packets = total_packets - flow->rule.last_packet_cnt;
		flow->rule.last_packet_cnt = total_packets;
	}

	/* We have no byte counter, report packets only */
	flow_stats_update(&cls_flower->stats, 0, new_packets, 0, lastused,
			  FLOW_ACTION_HW_STATS_IMMEDIATE);

out_unlock:
	mutex_unlock(&priv->tc_flow_lock);

	return err;
}

static int otto_tc_pie_cls_flower_add(struct otto_dsa_priv *priv, int port,
				      struct flow_cls_offload *cls, bool ingress)
{
	if (!ingress || !priv->r->pie_cls_flower)
		return -EOPNOTSUPP;

	/* No goto chain is offloaded, so in hardware a rule of another chain
	 * would see frames that never reach it in software.
	 */
	if (cls->common.chain_index) {
		NL_SET_ERR_MSG_MOD(cls->common.extack, "only chain 0 is offloaded");
		return -EOPNOTSUPP;
	}

	return otto_tc_configure_flower(priv, cls, port);
}

static int otto_tc_pie_cls_flower_del(struct otto_dsa_priv *priv,
				      struct flow_cls_offload *cls, bool ingress)
{
	if (!ingress || !priv->r->pie_cls_flower)
		return -ENOENT;

	return otto_tc_delete_flower(priv, cls);
}

static int otto_tc_pie_cls_flower_stats(struct otto_dsa_priv *priv,
					struct flow_cls_offload *cls, bool ingress)
{
	if (!ingress || !priv->r->pie_cls_flower)
		return -ENOENT;

	return otto_tc_stats_flower(priv, cls);
}

static const struct flow_action_entry *
otto_tc_rate_policy_extract(struct flow_cls_offload *cls)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(cls);

	/* only simple rules with a single action are supported */
	if (!flow_offload_has_one_action(&rule->action))
		return NULL;

	/* Anything that is not a policer is offloaded to PIE. Bail out before
	 * flow_action_basic_hw_stats_check() so it does not stamp a "HW stats
	 * type unsupported" extack onto a rule the PIE path accepts.
	 */
	if (rule->action.entries[0].id != FLOW_ACTION_POLICE)
		return NULL;

	if (!flow_action_basic_hw_stats_check(&rule->action, cls->common.extack))
		return NULL;

	return &rule->action.entries[0];
}

static bool otto_tc_flower_matches_all(struct flow_cls_offload *cls)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(cls);
	struct flow_match_control control;
	struct flow_match_basic basic;

	if (cls->common.chain_index || cls->common.protocol != htons(ETH_P_ALL))
		return false;

	if (rule->match.dissector->used_keys &
	    ~(BIT_ULL(FLOW_DISSECTOR_KEY_CONTROL) | BIT_ULL(FLOW_DISSECTOR_KEY_BASIC)))
		return false;

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		flow_rule_match_basic(rule, &basic);
		if (basic.mask->n_proto || basic.mask->ip_proto)
			return false;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL)) {
		flow_rule_match_control(rule, &control);
		if (control.mask->addr_type || control.mask->flags)
			return false;
	}

	return true;
}

static bool otto_tc_port_rate_police_validate(const struct flow_action_entry *act)
{
	if (!act)
		return false;

	/* only allow action which just limit rate with by dropping packets */
	if (act->id != FLOW_ACTION_POLICE)
		return false;

	if (act->police.rate_pkt_ps > 0)
		return false;

	if (act->police.exceed.act_id != FLOW_ACTION_DROP)
		return false;

	if (act->police.notexceed.act_id != FLOW_ACTION_ACCEPT)
		return false;

	return true;
}

int otto_tc_930x_port_rate_police_add(struct dsa_switch *ds, int port,
				      const struct flow_action_entry *act,
				      bool ingress)
{
	struct otto_dsa_priv *priv = ds->priv;
	u32 burst;
	u64 rate;
	u32 addr;

	/* rate has unit 16000 bit */
	rate = div_u64(act->police.rate_bytes_ps, 2000);
	rate = min_t(u64, rate, RTL93XX_BANDWIDTH_CTRL_RATE_MAX);
	rate |= RTL93XX_BANDWIDTH_CTRL_ENABLE;

	if (ingress)
		addr = RTL930X_BANDWIDTH_CTRL_INGRESS(port);
	else
		addr = RTL930X_BANDWIDTH_CTRL_EGRESS(port);

	if (ingress) {
		burst = min_t(u32, act->police.burst, RTL930X_BANDWIDTH_CTRL_INGRESS_BURST_MAX);

		/* the linux kernel only provides a single burst value. But the
		 * realtek HW needs two. And to get flow control correctly
		 * working, the realtek default ratio of 1:2 seems to work
		 * reasonable well
		 */
		sw_w32(burst, RTL930X_BANDWIDTH_CTRL_INGRESS_BURST_HIGH_ON(port));
		sw_w32(burst / 2, RTL930X_BANDWIDTH_CTRL_INGRESS_BURST_HIGH_OFF(port));

		/* Enable ingress bandwidth flow control to improve TCP throughput and avoid
		 * the drops behavior of the RTL930x ingress rate limiter which seem to not
		 * play well with any congestion control algorithm
		 */
		sw_w32_mask(0, RTL930X_INGRESS_FC_CTRL_EN(port),
			    RTL930X_INGRESS_FC_CTRL(port));
	} else {
		/* the bucket is 16 bit wide and has to hold the burst and one frame */
		burst = min_t(u32, act->police.burst, U16_MAX - priv->r->max_frame);

		sw_w32(burst, addr + 4);
	}

	sw_w32(rate, addr);

	return 0;
}

int otto_tc_930x_port_rate_police_del(struct dsa_switch *ds, int port,
				      struct flow_cls_offload *cls,
				      bool ingress)
{
	u32 addr;

	if (ingress)
		addr = RTL930X_BANDWIDTH_CTRL_INGRESS(port);
	else
		addr = RTL930X_BANDWIDTH_CTRL_EGRESS(port);

	sw_w32_mask(RTL93XX_BANDWIDTH_CTRL_ENABLE, 0, addr);

	if (ingress)
		sw_w32_mask(RTL930X_INGRESS_FC_CTRL_EN(port), 0,
			    RTL930X_INGRESS_FC_CTRL(port));

	return 0;
}

/* A queue below 0 is the shaper of the whole port and a rate of 0 switches the shaper off */
int otto_tc_930x_egress_shaper_set(struct otto_dsa_priv *priv, int port, int queue,
				   u64 rate_bytes_ps, u32 burst)
{
	u64 rate = div_u64(rate_bytes_ps, 2000);
	u32 addr, lb_ctrl;

	if (queue < 0)
		addr = RTL930X_BANDWIDTH_CTRL_EGRESS(port);
	else if (port < 24)
		addr = RTL930X_BANDWIDTH_CTRL_QUEUE_SET0(port, queue);
	else
		addr = RTL930X_BANDWIDTH_CTRL_QUEUE_SET1(port, queue);

	if (!rate_bytes_ps)
		return regmap_clear_bits(priv->map, addr, RTL93XX_BANDWIDTH_CTRL_ENABLE);

	/* the bucket is 16 bit wide and has to hold the burst and one frame, and the burst
	 * at least three of the tokens the bucket is refilled with
	 */
	regmap_read(priv->map, RTL930X_EGBW_LB_CTRL, &lb_ctrl);
	burst = min_t(u32, burst, U16_MAX - priv->r->max_frame);
	if (!rate || burst < 3 * FIELD_GET(RTL930X_EGBW_LB_CTRL_TOKEN, lb_ctrl))
		return -EINVAL;

	rate = min_t(u64, rate, RTL93XX_BANDWIDTH_CTRL_RATE_MAX);

	regmap_write(priv->map, addr + 4, burst);
	regmap_write(priv->map, addr, rate | RTL93XX_BANDWIDTH_CTRL_ENABLE);

	return 0;
}

int otto_tc_931x_port_rate_police_add(struct dsa_switch *ds, int port,
				      const struct flow_action_entry *act,
				      bool ingress)
{
	u32 burst;
	u64 rate;
	u32 addr;

	/* rate has unit 16000 bit */
	rate = div_u64(act->police.rate_bytes_ps, 2000);
	rate = min_t(u64, rate, RTL93XX_BANDWIDTH_CTRL_RATE_MAX);
	rate |= RTL93XX_BANDWIDTH_CTRL_ENABLE;

	burst = min_t(u32, act->police.burst, RTL931X_BANDWIDTH_CTRL_MAX_BURST);

	if (ingress)
		addr = RTL931X_BANDWIDTH_CTRL_INGRESS(port);
	else
		addr = RTL931X_BANDWIDTH_CTRL_EGRESS(port);

	sw_w32(burst, addr + 4);
	sw_w32(rate, addr);

	return 0;
}

int otto_tc_931x_port_rate_police_del(struct dsa_switch *ds, int port,
				      struct flow_cls_offload *cls,
				      bool ingress)
{
	u32 addr;

	if (ingress)
		addr = RTL931X_BANDWIDTH_CTRL_INGRESS(port);
	else
		addr = RTL931X_BANDWIDTH_CTRL_EGRESS(port);

	sw_w32_mask(RTL93XX_BANDWIDTH_CTRL_ENABLE, 0, addr);

	return 0;
}

static void otto_tc_tbf_apply(struct otto_dsa_priv *priv, int port, int queue, bool allowed)
{
	struct rtldsa_port *pp = &priv->ports[port];
	struct rtldsa_tbf *tbf = queue < 0 ? &pp->tbf : &pp->queue_tbf[queue];
	bool offload = allowed && tbf->handle && tbf->rate_bytes_ps && !tbf->child;

	if (offload)
		offload = !priv->r->egress_shaper_set(priv, port, queue, tbf->rate_bytes_ps,
						      tbf->burst);
	if (!offload && tbf->offloaded)
		priv->r->egress_shaper_set(priv, port, queue, 0, 0);

	tbf->offloaded = offload;
}

int otto_tc_cls_flower_add(struct dsa_switch *ds, int port,
			   struct flow_cls_offload *cls, bool ingress)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct rtldsa_port *p = &priv->ports[port];
	const struct flow_action_entry *act;
	int ret;

	/* a single rate/bandwidth limiter action on all frames is handled as port policing */
	act = otto_tc_rate_policy_extract(cls);

	/* everything else is offloaded to the PIE engine */
	if (!otto_tc_port_rate_police_validate(act) || !otto_tc_flower_matches_all(cls))
		return otto_tc_pie_cls_flower_add(priv, port, cls, ingress);

	if (!priv->r->port_rate_police_add)
		return -EOPNOTSUPP;

	mutex_lock(&priv->reg_mutex);

	/* only allow one offloaded police for ingress/egress */
	if (ingress && p->rate_police_ingress) {
		ret = -EOPNOTSUPP;
		goto unlock;
	}

	if (!ingress && (p->rate_police_egress || p->tbf.handle)) {
		ret = -EOPNOTSUPP;
		goto unlock;
	}

	ret = priv->r->port_rate_police_add(ds, port, act, ingress);
	if (ret < 0)
		goto unlock;

	if (ingress)
		p->rate_police_ingress = cls->cookie;
	else
		p->rate_police_egress = cls->cookie;

unlock:
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

int otto_tc_cls_flower_del(struct dsa_switch *ds, int port,
			   struct flow_cls_offload *cls, bool ingress)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct rtldsa_port *p = &priv->ports[port];
	int ret;

	/* PIE flower rules are ingress only. Try to remove a PIE rule first;
	 * if none exists for this cookie, fall back to port rate policing.
	 */
	if (ingress) {
		ret = otto_tc_pie_cls_flower_del(priv, cls, ingress);
		if (ret != -ENOENT)
			return ret;
	}

	if (!priv->r->port_rate_police_del)
		return -EOPNOTSUPP;

	mutex_lock(&priv->reg_mutex);

	if ((ingress ? p->rate_police_ingress : p->rate_police_egress) != cls->cookie) {
		ret = -ENOENT;
		goto unlock;
	}

	ret = priv->r->port_rate_police_del(ds, port, cls, ingress);
	if (ret < 0)
		goto unlock;

	if (ingress) {
		p->rate_police_ingress = 0;
	} else {
		p->rate_police_egress = 0;
		otto_tc_tbf_apply(priv, port, -1, true);
	}

unlock:
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

int otto_tc_cls_flower_stats(struct dsa_switch *ds, int port,
			     struct flow_cls_offload *cls, bool ingress)
{
	struct otto_dsa_priv *priv = ds->priv;
	int ret;

	/* only PIE flower rules provide per-rule statistics, and only ingress */
	if (!ingress)
		return 0;

	ret = otto_tc_pie_cls_flower_stats(priv, cls, ingress);
	if (ret == -ENOENT)
		return 0;

	return ret;
}

static bool otto_tc_ets_offloaded(struct rtldsa_port *pp)
{
	if (!pp->ets_offloadable || pp->ets_children)
		return false;

	for (int queue = 0; queue < MAX_PRIOS; queue++)
		if (pp->queue_tbf[queue].handle && !pp->queue_tbf[queue].offloaded)
			return false;

	return true;
}

static void otto_tc_ets_apply(struct otto_dsa_priv *priv, int port)
{
	struct rtldsa_port *pp = &priv->ports[port];
	bool offload = pp->ets_offloadable && !pp->ets_children;

	for (int queue = 0; queue < MAX_PRIOS; queue++)
		otto_tc_tbf_apply(priv, port, queue, offload);

	offload = otto_tc_ets_offloaded(pp);

	for (int queue = 0; queue < MAX_PRIOS; queue++) {
		if (!offload)
			otto_tc_tbf_apply(priv, port, queue, false);
		else if (pp->ets_weights[queue])
			priv->r->queue_sched_set(priv, port, queue, pp->ets_weights[queue], false);
		else
			priv->r->queue_sched_set(priv, port, queue, 1, true);
	}

	if (!offload)
		otto_qos_queue_sched_reset(priv, port);
}

/* Band 0 is the band ETS serves first and queue 7 the queue the hardware serves first.
 * The hardware maps priority n to queue n for the whole switch, so only the priomap
 * that says the same can be offloaded.
 */
static int otto_tc_setup_qdisc_ets(struct otto_dsa_priv *priv, int port,
				   struct tc_ets_qopt_offload *qopt)
{
	struct tc_ets_qopt_offload_replace_params *p = &qopt->replace_params;
	struct rtldsa_port *pp = &priv->ports[port];
	u32 old_handle = pp->ets_handle, child;
	unsigned int band;

	if (!priv->r->queue_sched_set || qopt->parent != TC_H_ROOT)
		return -EOPNOTSUPP;

	switch (qopt->command) {
	case TC_ETS_REPLACE:
		if (pp->ets_handle != qopt->handle)
			pp->ets_children = 0;
		pp->ets_children &= GENMASK(p->bands - 1, 0);
		pp->ets_handle = qopt->handle;
		pp->ets_offloadable = p->bands == MAX_PRIOS;
		for (int band = 0; pp->ets_offloadable && band < MAX_PRIOS; band++) {
			int queue = MAX_PRIOS - 1 - band;

			pp->ets_offloadable = p->priomap[queue] == band &&
					      (!p->quanta[band] || p->weights[band]);
			pp->ets_weights[queue] = p->quanta[band] ? p->weights[band] : 0;
		}
		break;
	case TC_ETS_DESTROY:
		if (pp->ets_handle != qopt->handle)
			return 0;
		pp->ets_handle = 0;
		pp->ets_offloadable = false;
		break;
	case TC_ETS_STATS:
		if (pp->ets_handle != qopt->handle)
			return -EOPNOTSUPP;
		return otto_tc_ets_offloaded(pp) ? 0 : -EOPNOTSUPP;
	case TC_ETS_GRAFT:
		if (pp->ets_handle != qopt->handle)
			return -EOPNOTSUPP;
		/* the kernel keeps a refused child, so the qdisc stays out of the hardware
		 * while any band has one, other than a TBF on the queue of the band
		 */
		band = qopt->graft_params.band;
		child = qopt->graft_params.child_handle;
		if (child && (band >= MAX_PRIOS ||
			      child != pp->queue_tbf[MAX_PRIOS - 1 - band].handle))
			pp->ets_children |= BIT(band);
		else
			pp->ets_children &= ~BIT(band);
		break;
	default:
		return -EOPNOTSUPP;
	}

	/* the TBFs on the bands of another ETS cannot be matched to a queue */
	for (int queue = 0; pp->ets_handle != old_handle && queue < MAX_PRIOS; queue++)
		pp->queue_tbf[queue].handle = 0;

	scoped_guard(mutex, &priv->reg_mutex)
		otto_tc_ets_apply(priv, port);

	return (otto_tc_ets_offloaded(pp) || qopt->command == TC_ETS_DESTROY) ? 0 : -EOPNOTSUPP;
}

static int otto_tc_setup_qdisc_tbf(struct otto_dsa_priv *priv, int port,
				   struct tc_tbf_qopt_offload *qopt)
{
	const struct psched_ratecfg *rate = &qopt->replace_params.rate;
	unsigned int band = TC_H_MIN(qopt->parent);
	struct rtldsa_port *pp = &priv->ports[port];
	struct rtldsa_tbf *tbf;
	int queue = -1;

	if (!priv->r->egress_shaper_set)
		return -EOPNOTSUPP;

	if (qopt->parent == TC_H_ROOT) {
		tbf = &pp->tbf;
	} else if (pp->ets_handle && TC_H_MAJ(qopt->parent) == pp->ets_handle &&
		   band && band <= MAX_PRIOS) {
		queue = MAX_PRIOS - band;
		tbf = &pp->queue_tbf[queue];
	} else {
		return -EOPNOTSUPP;
	}

	switch (qopt->command) {
	case TC_TBF_REPLACE:
		if (tbf->handle != qopt->handle)
			tbf->child = false;
		tbf->handle = qopt->handle;
		/* the limiters know no overhead, minimum size or ATM cells, so a TBF that
		 * uses them gets no rate and stays in software
		 */
		if (rate->overhead || rate->mpu || rate->linklayer == TC_LINKLAYER_ATM)
			tbf->rate_bytes_ps = 0;
		else
			tbf->rate_bytes_ps = rate->rate_bytes_ps;
		tbf->burst = qopt->replace_params.max_size;
		break;
	case TC_TBF_DESTROY:
		if (tbf->handle != qopt->handle)
			return 0;
		tbf->handle = 0;
		break;
	case TC_TBF_STATS:
		return tbf->handle == qopt->handle && tbf->offloaded ? 0 : -EOPNOTSUPP;
	case TC_TBF_GRAFT:
		if (tbf->handle != qopt->handle)
			return -EOPNOTSUPP;
		/* the kernel keeps a refused child and puts the noop qdisc, which drops
		 * everything, in place of a deleted one, so the qdisc stays out of the
		 * hardware from its first graft on
		 */
		tbf->child = true;
		break;
	default:
		return -EOPNOTSUPP;
	}

	scoped_guard(mutex, &priv->reg_mutex) {
		/* the egress police uses the shaper of the port */
		if (queue < 0)
			otto_tc_tbf_apply(priv, port, queue, !pp->rate_police_egress);
		else
			otto_tc_ets_apply(priv, port);
	}

	return (tbf->offloaded || qopt->command == TC_TBF_DESTROY) ? 0 : -EOPNOTSUPP;
}

int otto_tc_port_setup_tc(struct dsa_switch *ds, int port, enum tc_setup_type type,
			  void *type_data)
{
	struct otto_dsa_priv *priv = ds->priv;

	switch (type) {
	case TC_SETUP_QDISC_TBF:
		return otto_tc_setup_qdisc_tbf(priv, port, type_data);
	case TC_SETUP_QDISC_ETS:
		return otto_tc_setup_qdisc_ets(priv, port, type_data);
	default:
		return -EOPNOTSUPP;
	}
}
