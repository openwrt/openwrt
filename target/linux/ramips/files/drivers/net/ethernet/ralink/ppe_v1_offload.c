// SPDX-License-Identifier: GPL-2.0-only
/* Flowtable hardware offload for the first-generation Ralink PPE.
 *
 * Action parsing follows drivers/net/ethernet/mediatek/mtk_ppe_offload.c,
 * Copyright (C) 2020 Felix Fietkau <nbd@nbd.name>.
 */

#include <linux/bitfield.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/rhashtable.h>
#include <linux/slab.h>
#include <linux/unaligned.h>
#include <net/flow_offload.h>
#include <net/pkt_cls.h>

#include "mtk_eth_soc.h"
#include "ppe_v1_offload.h"

/* Rules only come from the flowtable, which matches the exact 5-tuple.
 * Checking the keys is enough to turn down IPv6 and GRE.
 *
 * The PPE hands TCP segments with SYN, FIN or RST to the CPU, which covers
 * the FIN/RST match of the flowtable. It cannot match the ingress VLAN, so
 * a frame with the same 5-tuple received on another VLAN of the GMAC takes
 * the same path.
 */
#define PPE_V1_KEYS_REQUIRED	(BIT_ULL(FLOW_DISSECTOR_KEY_META) | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_CONTROL) | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_BASIC) | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_IPV4_ADDRS) | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_PORTS))
#define PPE_V1_KEYS_SUPPORTED	(PPE_V1_KEYS_REQUIRED | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_TCP) | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_VLAN) | \
				 BIT_ULL(FLOW_DISSECTOR_KEY_CVLAN))

static const struct rhashtable_params ppe_v1_flow_ht_params = {
	.head_offset = offsetof(struct ppe_v1_flow, node),
	.key_offset = offsetof(struct ppe_v1_flow, cookie),
	.key_len = sizeof(unsigned long),
	.automatic_shrinking = true,
};

struct ppe_v1_flow_data {
	struct ethhdr eth;
	u16 vlan[2];
	u8 vlan_num;
	u16 pppoe_sid;
	u8 pppoe_num;
};

static void ppe_v1_mangle_eth(const struct flow_action_entry *act, void *eth)
{
	void *dest = eth + act->mangle.offset;
	const void *src = &act->mangle.val;

	if (act->mangle.offset > 8)
		return;

	if (act->mangle.mask == 0xffff) {
		src += 2;
		dest += 2;
	}

	memcpy(dest, src, act->mangle.mask ? 2 : 4);
}

static int ppe_v1_mangle_ports(const struct flow_action_entry *act,
			       struct ppe_v1_foe_ipv4 *foe)
{
	u32 val = be32_to_cpu((__force __be32)act->mangle.val);

	switch (act->mangle.offset) {
	case 0:
		if (act->mangle.mask == ~htonl(0xffff))
			foe->new_dport = lower_16_bits(val);
		else
			foe->new_sport = upper_16_bits(val);
		break;
	case 2:
		foe->new_dport = lower_16_bits(val);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int ppe_v1_mangle_ipv4(const struct flow_action_entry *act,
			      struct ppe_v1_foe_ipv4 *foe)
{
	u32 val = be32_to_cpu((__force __be32)act->mangle.val);

	switch (act->mangle.offset) {
	case offsetof(struct iphdr, saddr):
		foe->new_sip = val;
		break;
	case offsetof(struct iphdr, daddr):
		foe->new_dip = val;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int ppe_v1_mangle(const struct flow_action_entry *act,
			 struct ppe_v1_flow_data *data,
			 struct ppe_v1_foe_ipv4 *foe)
{
	switch (act->mangle.htype) {
	case FLOW_ACT_MANGLE_HDR_TYPE_ETH:
		ppe_v1_mangle_eth(act, &data->eth);
		return 0;
	case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
	case FLOW_ACT_MANGLE_HDR_TYPE_UDP:
		return ppe_v1_mangle_ports(act, foe);
	case FLOW_ACT_MANGLE_HDR_TYPE_IP4:
		return ppe_v1_mangle_ipv4(act, foe);
	default:
		return -EOPNOTSUPP;
	}
}

static bool ppe_v1_udp_port_excluded(u16 port)
{
	return port == 500 || port == 4500 || port == 1701;
}

static int ppe_v1_parse_match(struct ppe_v1 *ppe, struct flow_rule *rule,
			      struct ppe_v1_tuple *key,
			      struct netlink_ext_ack *extack)
{
	u64 keys = rule->match.dissector->used_keys;
	struct flow_match_ipv4_addrs addrs;
	struct flow_match_control control;
	struct flow_match_ports ports;
	struct flow_match_basic basic;
	struct flow_match_meta meta;

	if ((keys & PPE_V1_KEYS_REQUIRED) != PPE_V1_KEYS_REQUIRED ||
	    (keys & ~PPE_V1_KEYS_SUPPORTED))
		return -EOPNOTSUPP;

	/* only frames received by the GMAC pass through the PPE */
	flow_rule_match_meta(rule, &meta);
	if (meta.key->ingress_ifindex != READ_ONCE(ppe->netdev->ifindex))
		return -EOPNOTSUPP;

	flow_rule_match_control(rule, &control);
	if (flow_rule_has_control_flags(control.mask->flags, extack))
		return -EOPNOTSUPP;

	flow_rule_match_basic(rule, &basic);
	flow_rule_match_ipv4_addrs(rule, &addrs);
	flow_rule_match_ports(rule, &ports);
	key->sip = be32_to_cpu(addrs.key->src);
	key->dip = be32_to_cpu(addrs.key->dst);
	key->sport = be16_to_cpu(ports.key->src);
	key->dport = be16_to_cpu(ports.key->dst);
	key->proto = basic.key->ip_proto;

	/* IKE/NAT-T and L2TP mix checksummed and zero-checksum datagrams in
	 * one flow, and the PPE corrupts the latter.
	 */
	if (key->proto == IPPROTO_UDP &&
	    (ppe_v1_udp_port_excluded(key->sport) ||
	     ppe_v1_udp_port_excluded(key->dport)))
		return -EOPNOTSUPP;

	return 0;
}

/* A bound entry only punts frames that do not fit the GMAC
 * (PPE_CPU_REASON_EXCEED_MTU). Leave flows out of a VLAN device with a
 * smaller MTU to the flowtable, which fragments or sends ICMP. The
 * flowtable pushes the tag of the upper device first, so walk down from
 * the GMAC with the last one.
 */
static bool ppe_v1_egress_mtu_ok(struct ppe_v1 *ppe,
				 const struct ppe_v1_flow_data *data)
{
	struct net_device *dev = ppe->netdev;
	bool ok;
	int i;

	rcu_read_lock();
	for (i = data->vlan_num - 1; i >= 0 && dev; i--)
		dev = __vlan_find_dev_deep_rcu(dev, htons(ETH_P_8021Q),
					       data->vlan[i]);
	/* no VLAN device (bridge VLAN filtering): nothing smaller to check */
	ok = !dev || READ_ONCE(dev->mtu) >= READ_ONCE(ppe->netdev->mtu);
	rcu_read_unlock();

	return ok;
}

static int ppe_v1_flow_replace(struct ppe_v1_block *block,
			       struct flow_cls_offload *f)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct ppe_v1_foe_entry template = {};
	struct ppe_v1_foe_ipv4 *foe = &template.ipv4;
	struct ppe_v1 *ppe = block->ppe;
	struct ppe_v1_flow_data data = {};
	struct net_device *odev = NULL;
	struct flow_action_entry *act;
	struct ppe_v1_flow *flow;
	struct ppe_v1_tuple key = {};
	int i, err;

	lockdep_assert_held(&ppe->mutex);

	if (rhashtable_lookup_fast(&ppe->flow_ht, &f->cookie,
				   ppe_v1_flow_ht_params))
		return -EEXIST;

	err = ppe_v1_parse_match(ppe, rule, &key, f->common.extack);
	if (err)
		return err;

	foe->new_sip = key.sip;
	foe->new_dip = key.dip;
	foe->new_sport = key.sport;
	foe->new_dport = key.dport;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			err = ppe_v1_mangle(act, &data, foe);
			if (err)
				return err;
			break;
		case FLOW_ACTION_REDIRECT:
			odev = act->dev;
			break;
		case FLOW_ACTION_CSUM:
		case FLOW_ACTION_VLAN_POP:
			break;
		case FLOW_ACTION_VLAN_PUSH:
			if (data.vlan_num + data.pppoe_num == 2 ||
			    act->vlan.proto != htons(ETH_P_8021Q))
				return -EOPNOTSUPP;
			data.vlan[data.vlan_num++] = act->vlan.vid;
			break;
		case FLOW_ACTION_PPPOE_PUSH:
			if (data.pppoe_num == 1 || data.vlan_num == 2)
				return -EOPNOTSUPP;
			data.pppoe_sid = act->pppoe.sid;
			data.pppoe_num++;
			break;
		default:
			return -EOPNOTSUPP;
		}
	}

	if (odev != ppe->netdev || !ppe_v1_egress_mtu_ok(ppe, &data))
		return -EOPNOTSUPP;

	if (!is_valid_ether_addr(data.eth.h_source) ||
	    !is_valid_ether_addr(data.eth.h_dest))
		return -EINVAL;

	foe->dmac_hi = get_unaligned_be16(data.eth.h_dest);
	foe->dmac_lo = get_unaligned_be32(data.eth.h_dest + 2);
	foe->smac_hi = get_unaligned_be16(data.eth.h_source);
	foe->smac_lo = get_unaligned_be32(data.eth.h_source + 2);

	/* INSERT replaces a header that is already there and DELETE does
	 * nothing on a frame without one, so the actions do not depend on the
	 * encapsulation the frame arrives with.
	 */
	foe->ib1 = PPE_IB1_BIND_TTL |
		   FIELD_PREP(PPE_IB1_BIND_VLAN1, data.vlan_num > 0 ?
			      PPE_L2_ACT_INSERT : PPE_L2_ACT_DELETE) |
		   FIELD_PREP(PPE_IB1_BIND_VLAN2, data.vlan_num > 1 ?
			      PPE_L2_ACT_INSERT : PPE_L2_ACT_DELETE) |
		   FIELD_PREP(PPE_IB1_BIND_PPPOE, data.pppoe_num ?
			      PPE_L2_ACT_INSERT : PPE_L2_ACT_DELETE);
	foe->vlan1 = data.vlan[0];
	foe->vlan2 = data.vlan[1];
	foe->pppoe_id = data.pppoe_sid;
	foe->ib2 = PPE_IB2_FORCE_DP | FIELD_PREP(PPE_IB2_DP, PPE_PSE_PORT_GDMA1);

	flow = kzalloc_obj(*flow);
	if (!flow)
		return -ENOMEM;

	flow->cookie = f->cookie;
	flow->block = block;
	flow->key = key;
	flow->foe = template;
	flow->hash = -1;

	err = rhashtable_insert_fast(&ppe->flow_ht, &flow->node,
				     ppe_v1_flow_ht_params);
	if (err)
		goto free;

	/* Last, since the RX path may bind the flow right away. A flow with
	 * the same tuple may still be waiting for its DESTROY; the flowtable
	 * retries the REPLACE later.
	 */
	err = ppe_v1_flow_commit(ppe, flow);
	if (err) {
		rhashtable_remove_fast(&ppe->flow_ht, &flow->node,
				       ppe_v1_flow_ht_params);
		goto free;
	}

	list_add_tail(&flow->list, &ppe->flows);
	return 0;

free:
	kfree(flow);
	return err;
}

static void ppe_v1_flow_free(struct ppe_v1 *ppe, struct ppe_v1_flow *flow)
{
	lockdep_assert_held(&ppe->mutex);

	ppe_v1_flow_clear(ppe, flow);
	rhashtable_remove_fast(&ppe->flow_ht, &flow->node,
			       ppe_v1_flow_ht_params);
	list_del(&flow->list);
	kfree(flow);
}

static struct ppe_v1_flow *ppe_v1_flow_find(struct ppe_v1_block *block,
					    unsigned long cookie)
{
	struct ppe_v1_flow *flow;

	flow = rhashtable_lookup_fast(&block->ppe->flow_ht, &cookie,
				      ppe_v1_flow_ht_params);
	if (!flow || flow->block != block)
		return NULL;

	return flow;
}

static int ppe_v1_flow_cmd(struct ppe_v1_block *block,
			   struct flow_cls_offload *cls)
{
	struct ppe_v1 *ppe = block->ppe;
	struct ppe_v1_flow *flow;
	int err = 0, idle;

	mutex_lock(&ppe->mutex);
	switch (cls->command) {
	case FLOW_CLS_REPLACE:
		err = ppe_v1_flow_replace(block, cls);
		break;
	case FLOW_CLS_DESTROY:
		flow = ppe_v1_flow_find(block, cls->cookie);
		if (flow)
			ppe_v1_flow_free(ppe, flow);
		else
			err = -ENOENT;
		break;
	case FLOW_CLS_STATS:
		/* Only lastused: the PPE has no per-entry packet or byte
		 * counters, so conntrack accounting stops once a flow is bound.
		 */
		flow = ppe_v1_flow_find(block, cls->cookie);
		if (!flow) {
			err = -ENOENT;
			break;
		}
		idle = ppe_v1_flow_idle_time(ppe, flow);
		if (idle >= 0)
			cls->stats.lastused = jiffies - idle * HZ;
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	mutex_unlock(&ppe->mutex);

	return err;
}

static int ppe_v1_setup_tc_block_cb(enum tc_setup_type type, void *type_data,
				    void *cb_priv)
{
	struct ppe_v1_block *block = cb_priv;
	struct flow_cls_offload *cls = type_data;

	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;

	/* flows that are already offloaded must still be removable */
	if (cls->command == FLOW_CLS_REPLACE &&
	    !tc_can_offload(block->ppe->netdev))
		return -EOPNOTSUPP;

	return ppe_v1_flow_cmd(block, cls);
}

/* frees the flows of @block, or all of them if @block is NULL */
static void ppe_v1_flush(struct ppe_v1 *ppe, struct ppe_v1_block *block)
{
	struct ppe_v1_flow *flow, *tmp;

	lockdep_assert_held(&ppe->mutex);

	list_for_each_entry_safe(flow, tmp, &ppe->flows, list)
		if (!block || flow->block == block)
			ppe_v1_flow_free(ppe, flow);
}

static void ppe_v1_block_release(void *cb_priv)
{
	kfree(cb_priv);
}

static int ppe_v1_setup_ft(struct ppe_v1 *ppe, struct flow_block_offload *f)
{
	flow_setup_cb_t *cb = ppe_v1_setup_tc_block_cb;
	struct flow_block_cb *block_cb;
	struct ppe_v1_block *block;
	int err = 0;

	if (f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	mutex_lock(&ppe->mutex);
	f->driver_block_list = &ppe->block_cb_list;

	switch (f->command) {
	case FLOW_BLOCK_BIND:
		block_cb = flow_block_cb_lookup(f->block, cb, ppe);
		if (block_cb) {
			flow_block_cb_incref(block_cb);
			break;
		}

		block = kzalloc_obj(*block);
		if (!block) {
			err = -ENOMEM;
			break;
		}
		block->ppe = ppe;

		block_cb = flow_block_cb_alloc(cb, ppe, block,
					       ppe_v1_block_release);
		if (IS_ERR(block_cb)) {
			kfree(block);
			err = PTR_ERR(block_cb);
			break;
		}

		flow_block_cb_incref(block_cb);
		flow_block_cb_add(block_cb, f);
		list_add_tail(&block_cb->driver_list, &ppe->block_cb_list);
		ppe_v1_set_steering(ppe);
		break;
	case FLOW_BLOCK_UNBIND:
		block_cb = flow_block_cb_lookup(f->block, cb, ppe);
		if (!block_cb) {
			err = -ENOENT;
			break;
		}

		if (flow_block_cb_decref(block_cb))
			break;

		/* No DESTROY reaches this block any more, e.g. when fw4
		 * replaces the flowtable on reload.
		 */
		ppe_v1_flush(ppe, flow_block_cb_priv(block_cb));
		flow_block_cb_remove(block_cb, f);
		list_del(&block_cb->driver_list);
		ppe_v1_set_steering(ppe);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}

	mutex_unlock(&ppe->mutex);
	return err;
}

int ppe_v1_setup_tc(struct ppe_v1 *ppe, enum tc_setup_type type,
		    void *type_data)
{
	if (!ppe || type != TC_SETUP_FT)
		return -EOPNOTSUPP;

	return ppe_v1_setup_ft(ppe, type_data);
}

int ppe_v1_offload_init(struct ppe_v1 *ppe)
{
	return rhashtable_init(&ppe->flow_ht, &ppe_v1_flow_ht_params);
}

void ppe_v1_offload_cleanup(struct ppe_v1 *ppe)
{
	mutex_lock(&ppe->mutex);
	ppe_v1_flush(ppe, NULL);
	mutex_unlock(&ppe->mutex);

	rhashtable_destroy(&ppe->flow_ht);
}
