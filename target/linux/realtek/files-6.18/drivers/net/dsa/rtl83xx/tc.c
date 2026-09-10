// SPDX-License-Identifier: GPL-2.0-only

#include <net/dsa.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <net/flow_offload.h>
#include <linux/rhashtable.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "rtl-otto.h"

/* Parse the flow rule for the matching conditions */
static int rtl83xx_parse_flow_rule(struct rtl838x_switch_priv *priv,
				   struct flow_rule *rule, struct rtl83xx_flow *flow)
{
	struct flow_dissector *dissector = rule->match.dissector;
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

	if ((dissector->used_keys & BIT_ULL(FLOW_DISSECTOR_KEY_IPV4_ADDRS)) &&
	    (dissector->used_keys & BIT_ULL(FLOW_DISSECTOR_KEY_IPV6_ADDRS)))
		return -EOPNOTSUPP;

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

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS)) {
		struct flow_match_ipv4_addrs match;

		dev_dbg(priv->dev, "match IPV4\n");
		flow_rule_match_ipv4_addrs(rule, &match);
		flow->rule.is_ipv6 = false;
		flow->rule.dip = match.key->dst;
		flow->rule.dip_m = match.mask->dst;
		flow->rule.sip = match.key->src;
		flow->rule.sip_m = match.mask->src;
	} else if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS)) {
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

static void rtl83xx_flow_bypass_all(struct rtl83xx_flow *flow)
{
	flow->rule.bypass_sel = true;
	flow->rule.bypass_all = true;
	flow->rule.bypass_igr_stp = true;
	flow->rule.bypass_ibc_sc = true;
}

static int rtldsa_validate_flow_actions(struct flow_rule *rule)
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
			 * action fields via the translation in rtl83xx_add_flow(),
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

static int rtl83xx_parse_fwd(struct rtl838x_switch_priv *priv,
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
	rtl83xx_flow_bypass_all(flow);

	return 0;
}

static int rtl83xx_add_flow(struct rtl838x_switch_priv *priv, struct flow_cls_offload *f,
			    struct rtl83xx_flow *flow)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	const struct flow_action_entry *act;
	int i, err;

	dev_dbg(priv->dev, "adding flower rule\n");

	if (flow_rule_match_has_control_flags(rule, f->common.extack))
		return -EOPNOTSUPP;

	if (!flow_action_hw_stats_check(&rule->action, f->common.extack,
					FLOW_ACTION_HW_STATS_IMMEDIATE_BIT))
		return -EOPNOTSUPP;

	err = rtldsa_validate_flow_actions(rule);
	if (err)
		return err;

	err = rtl83xx_parse_flow_rule(priv, rule, flow);
	if (err)
		return err;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_DROP:
			dev_dbg(priv->dev, "action DROP\n");
			flow->rule.drop = true;
			rtl83xx_flow_bypass_all(flow);
			return 0;

		case FLOW_ACTION_TRAP:
			dev_dbg(priv->dev, "action TRAP\n");
			flow->rule.fwd_sel = true;
			flow->rule.fwd_data = priv->r->cpu_port;
			flow->rule.fwd_act = PIE_ACT_REDIRECT_TO_PORT;
			rtl83xx_flow_bypass_all(flow);
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
			err = rtl83xx_parse_fwd(priv, act, flow);
			if (err)
				return err;
			flow->rule.fwd_act = PIE_ACT_REDIRECT_TO_PORT;
			break;

		case FLOW_ACTION_MIRRED:
			dev_dbg(priv->dev, "action MIRRED\n");
			err = rtl83xx_parse_fwd(priv, act, flow);
			if (err)
				return err;
			flow->rule.fwd_act = PIE_ACT_COPY_TO_PORT;
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

int rtldsa_tc_init(struct rtl838x_switch_priv *priv)
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

/* Zero a PIE rule's LOG-table packet counter. On RTL930x the counter id is
 * the PIE rule id itself: the rule owns it and pie_rule_rm() releases it,
 * so there is nothing to hand back to an allocator here.
 */
static void rtldsa_packet_cntr_clear(struct rtl838x_switch_priv *priv, int counter)
{
	if (counter < 0 || !priv->r->packet_cntr_clear)
		return;

	mutex_lock(&priv->reg_mutex);
	priv->r->packet_cntr_clear(priv, counter);
	mutex_unlock(&priv->reg_mutex);
}

static void rtldsa_tc_flow_free(void *ptr, void *arg)
{
	struct rtl83xx_flow *flow = ptr;
	struct rtl838x_switch_priv *priv = arg;

	priv->r->pie_rule_rm(priv, &flow->rule);
	rtldsa_packet_cntr_clear(priv, flow->rule.packet_cntr);

	/* Readers may still hold an RCU-protected reference after the
	 * object has been removed from the hash table.
	 */
	kfree_rcu(flow, rcu_head);
}

void rtldsa_tc_cleanup(struct rtl838x_switch_priv *priv)
{
	if (!priv->tc_initialized)
		return;

	/* Hold tc_flow_lock like the add/del/stats callbacks do: any
	 * callback that slipped in before teardown must finish before the
	 * table and rules are torn down, otherwise it can walk a half-freed
	 * flow or run pie_rule_rm() against a rule this path already removed.
	 */
	mutex_lock(&priv->tc_flow_lock);
	rhashtable_free_and_destroy(&priv->tc_ht, rtldsa_tc_flow_free, priv);
	priv->tc_initialized = false;
	mutex_unlock(&priv->tc_flow_lock);

	rcu_barrier();
	mutex_destroy(&priv->tc_flow_lock);
}

static int rtldsa_configure_flower(struct rtl838x_switch_priv *priv,
				   struct flow_cls_offload *f, int ingress_port)
{
	struct rtl83xx_flow *flow;
	int err = 0;

	dev_dbg(priv->dev, "configuring flower rule\n");

	if (!priv->r->packet_cntr_read || !priv->r->packet_cntr_clear)
		return -EOPNOTSUPP;

	dev_dbg(priv->dev, "cookie %08lx\n", f->cookie);

	mutex_lock(&priv->tc_flow_lock);

	/* rtldsa_tc_cleanup() clears this under the lock before it destroys
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

	err = rtl83xx_add_flow(priv, f, flow);
	if (err)
		goto out_remove;

	flow->rule.spn = ingress_port;
	flow->rule.spn_m = 0x7f;

	/* The only caller, rtldsa_pie_cls_flower_add(), is RTL930x-only, where
	 * the PIE rule ID is also the LOG table counter ID: the counter is
	 * implied by the rule and only known once pie_rule_add() has assigned
	 * the ID. That range is kept out of rtldsa_packet_cntr_alloc(), so the
	 * implied LOG entry cannot alias an L3 route PIE rule's counter.
	 */
	flow->rule.log_sel = true;

	err = priv->r->pie_rule_add(priv, &flow->rule);
	if (err)
		goto out_remove;

	flow->rule.packet_cntr = flow->rule.id;
	dev_dbg(priv->dev, "using PIE rule counter %d\n", flow->rule.packet_cntr);
	rtldsa_packet_cntr_clear(priv, flow->rule.packet_cntr);

	mutex_unlock(&priv->tc_flow_lock);
	return 0;

out_remove:
	rhashtable_remove_fast(&priv->tc_ht, &flow->node, tc_ht_params);
	rtldsa_packet_cntr_clear(priv, flow->rule.packet_cntr);
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

static int rtldsa_delete_flower(struct rtl838x_switch_priv *priv,
				struct flow_cls_offload *cls_flower)
{
	struct rtl83xx_flow *flow;
	int err;

	dev_dbg(priv->dev, "deleting flower rule\n");

	mutex_lock(&priv->tc_flow_lock);

	/* see rtldsa_configure_flower(): bail if teardown already ran */
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
	rtldsa_packet_cntr_clear(priv, flow->rule.packet_cntr);

	kfree_rcu(flow, rcu_head);

out_unlock:
	mutex_unlock(&priv->tc_flow_lock);

	return err;
}

static int rtldsa_stats_flower(struct rtl838x_switch_priv *priv,
			       struct flow_cls_offload *cls_flower)
{
	struct rtl83xx_flow *flow;
	unsigned long lastused = 0;
	u32 total_packets, new_packets = 0;
	int err = 0;

	dev_dbg(priv->dev, "reading flower rule stats\n");

	mutex_lock(&priv->tc_flow_lock);

	/* see rtldsa_configure_flower(): bail if teardown already ran */
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

int rtldsa_pie_cls_flower_add(struct rtl838x_switch_priv *priv, int port,
			       struct flow_cls_offload *cls, bool ingress)
{
	if (!ingress || !priv->r->pie_rule_id_is_log_counter)
		return -EOPNOTSUPP;

	return rtldsa_configure_flower(priv, cls, port);
}

int rtldsa_pie_cls_flower_del(struct rtl838x_switch_priv *priv,
			       struct flow_cls_offload *cls, bool ingress)
{
	if (!ingress || !priv->r->pie_rule_id_is_log_counter)
		return -ENOENT;

	return rtldsa_delete_flower(priv, cls);
}

int rtldsa_pie_cls_flower_stats(struct rtl838x_switch_priv *priv,
				 struct flow_cls_offload *cls, bool ingress)
{
	if (!ingress || !priv->r->pie_rule_id_is_log_counter)
		return -ENOENT;

	return rtldsa_stats_flower(priv, cls);
}
