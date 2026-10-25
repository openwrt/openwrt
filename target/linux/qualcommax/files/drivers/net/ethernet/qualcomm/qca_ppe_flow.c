// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/* IPv4 flowtable offload into the PPE's L3 flow table.
 *
 * nf_flowtable hands each direction of an offloaded connection over as a
 * flower rule on the flow block DSA binds to the conduit; the EDMA conduit
 * passes TC_SETUP_FT back to the switch on its CPU port. A direction becomes
 * one 5-tuple flow entry, keyed on its source address and both ports, and a
 * host entry for its destination address, which the flows to one remote
 * address share. The flow entry names a next hop carrying the egress port,
 * the destination MAC and, for NAT, the new address; the new port sits in the
 * flow entry itself. A frame that hits a flow leaves the switch without ever
 * reaching EDMA.
 *
 * The layouts are qca-ssdk's (hppe_flow_reg.h, hppe_ip_reg.h). What it does
 * not say was measured on IPQ8074 v1 (rev 1.2):
 *  - FLOW_CTRL1's key_sel names the address the host table is searched
 *    with: 1 matches entries that store the source address.
 *  - a next hop of type 0 is a VSI, type 1 a port.
 *  - multi-word table entries are staged and take effect on the last word,
 *    so every entry is written whole.
 *  - a frame enters the L3 stage only if its destination MAC is in MY_MAC
 *    and its port names an L3 interface with routing on.
 *
 * Not offloaded: IPv6, VLAN and PPPoE encapsulation, flows that change both
 * addresses. The router MAC set and MTUs are taken when the first flowtable
 * binds; a later change needs the flowtable rebound.
 */

#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/rhashtable.h>
#include <net/dsa.h>
#include <net/flow_offload.h>
#include <net/pkt_cls.h>

#include "qca_ppe.h"

/* Egress L3 interfaces carrying a router MAC sit above the per-port ones the
 * ingress side uses, one per MY_MAC entry.
 */
#define PPE_FT_EG_IF_BASE	16
#define PPE_FT_OP_TIMEOUT_US	1000
#define PPE_FT_FWD_RDT_CPU	3

struct ppe_ft_mac {
	u8 addr[ETH_ALEN];
	u32 refs;
};

struct ppe_ft_pub_ip {
	__be32 addr;
	u32 refs;
};

struct ppe_ft_nexthop {
	u32 w[PPE_IN_NEXTHOP_WORDS];
	u32 refs;
};

struct ppe_ft_flow {
	struct rhash_head node;
	unsigned long cookie;
	u16 flow_idx;
	u16 host_idx;
	u16 nexthop;
	s8 mac;
	s8 pub_ip;
	/* The counters as last reported, which the next report subtracts. */
	u32 pkts;
	u64 bytes;
};

struct ppe_ft {
	/* Guards everything here and both operation engines. */
	struct mutex lock;
	struct rhashtable flows;
	unsigned int binds;
	u8 cmd_id;
	u8 host_cmd_id;
	u32 flow_ctrl1;
	u32 l3_if[QCA_PPE_MAX_PORTS][PPE_IN_L3_IF_WORDS];
	struct ppe_ft_mac mac[PPE_MY_MAC_ENTRIES];
	struct ppe_ft_pub_ip pub_ip[PPE_IN_PUB_IP_ENTRIES];
	struct ppe_ft_nexthop nexthop[PPE_IN_NEXTHOP_ENTRIES];
	u16 host_refs[PPE_HOST_ENTRIES];
	/* A key added twice lands on its old index, so the hardware entry
	 * belongs to whichever flow wrote it last.
	 */
	struct ppe_ft_flow *owner[PPE_IN_FLOW_ENTRIES];
};

struct ppe_ft_rule {
	__be32 saddr, daddr, new_saddr, new_daddr;
	__be16 sport, dport, new_sport, new_dport;
	u8 proto;
	struct ethhdr eth;
	int port;
};

static const struct rhashtable_params ppe_ft_ht_params = {
	.head_offset = offsetof(struct ppe_ft_flow, node),
	.key_offset = offsetof(struct ppe_ft_flow, cookie),
	.key_len = sizeof(unsigned long),
	.automatic_shrinking = true,
};

static LIST_HEAD(ppe_ft_block_cb_list);

static void ppe_ft_entry_write(struct qca_ppe_priv *priv, u32 reg,
			       const u32 *w, int words)
{
	int i;

	for (i = 0; i < words; i++)
		regmap_write(priv->regmap, reg + i * 4, w[i]);
}

/* The result register holds the id of the command it last finished. Every
 * operation waits for its own, so the previous id is never ours.
 */
static int ppe_ft_flow_op(struct qca_ppe_priv *priv, u32 type, bool by_index,
			  bool host, u16 *idx)
{
	struct ppe_ft *ft = priv->ft;
	u32 id, val;
	int ret;

	ft->cmd_id = ft->cmd_id % 15 + 1;
	id = ft->cmd_id;

	regmap_write(priv->regmap, PPE_IN_FLOW_OP,
		     FIELD_PREP(PPE_IN_FLOW_OP_CMD_ID, id) |
		     FIELD_PREP(PPE_IN_FLOW_OP_TYPE, type) |
		     FIELD_PREP(PPE_IN_FLOW_OP_HASH_BLOCK, 3) |
		     FIELD_PREP(PPE_IN_FLOW_OP_MODE, by_index) |
		     FIELD_PREP(PPE_IN_FLOW_OP_HOST_EN, host) |
		     FIELD_PREP(PPE_IN_FLOW_OP_INDEX, *idx));

	ret = regmap_read_poll_timeout(priv->regmap, PPE_IN_FLOW_OP_RSLT, val,
				       FIELD_GET(PPE_IN_FLOW_RSLT_CMD_ID, val) ==
				       id, 1, PPE_FT_OP_TIMEOUT_US);
	if (ret)
		return ret;
	if (val & PPE_IN_FLOW_RSLT_FAIL)
		return -ENOSPC;

	*idx = FIELD_GET(PPE_IN_FLOW_RSLT_INDEX, val);
	return 0;
}

static int ppe_ft_host_del(struct qca_ppe_priv *priv, u16 idx)
{
	struct ppe_ft *ft = priv->ft;
	u32 id, val;
	int ret;

	ft->host_cmd_id = ft->host_cmd_id % 15 + 1;
	id = ft->host_cmd_id;

	regmap_write(priv->regmap, PPE_HOST_TBL_OP,
		     FIELD_PREP(PPE_HOST_TBL_OP_CMD_ID, id) |
		     FIELD_PREP(PPE_HOST_TBL_OP_TYPE, 1) |
		     FIELD_PREP(PPE_HOST_TBL_OP_HASH_BLOCK, 3) |
		     PPE_HOST_TBL_OP_MODE |
		     FIELD_PREP(PPE_HOST_TBL_OP_INDEX, idx));

	ret = regmap_read_poll_timeout(priv->regmap, PPE_HOST_TBL_OP_RSLT, val,
				       FIELD_GET(PPE_HOST_TBL_RSLT_CMD_ID, val) ==
				       id, 1, PPE_FT_OP_TIMEOUT_US);
	if (ret)
		return ret;

	return val & PPE_HOST_TBL_RSLT_FAIL ? -ENOENT : 0;
}

static void ppe_ft_host_put(struct qca_ppe_priv *priv, u16 idx)
{
	struct ppe_ft *ft = priv->ft;
	u32 zero[PPE_HOST_V4_WORDS] = { };

	if (WARN_ON(!ft->host_refs[idx]) || --ft->host_refs[idx])
		return;

	if (ppe_ft_host_del(priv, idx))
		ppe_ft_entry_write(priv, PPE_HOST_TBL(idx), zero,
				   ARRAY_SIZE(zero));
}

static void ppe_ft_flow_hw_del(struct qca_ppe_priv *priv, u16 idx)
{
	u32 zero[PPE_IN_FLOW_V4_WORDS] = { };
	u16 i = idx;

	if (ppe_ft_flow_op(priv, 1, true, false, &i))
		ppe_ft_entry_write(priv, PPE_IN_FLOW_TBL(idx), zero,
				   ARRAY_SIZE(zero));
}

/* A router MAC is both the MY_MAC entry frames to it are matched against and
 * the egress L3 interface frames from it are rewritten with.
 */
static int ppe_ft_mac_get(struct qca_ppe_priv *priv, const u8 *addr)
{
	struct ppe_ft *ft = priv->ft;
	u32 w[PPE_EG_L3_IF_WORDS] = { };
	int i, free = -1;

	for (i = 0; i < PPE_MY_MAC_ENTRIES; i++) {
		if (ft->mac[i].refs && ether_addr_equal(ft->mac[i].addr, addr)) {
			ft->mac[i].refs++;
			return i;
		}
		if (!ft->mac[i].refs && free < 0)
			free = i;
	}
	if (free < 0)
		return -ENOSPC;

	ppe_entry_set(w, 0, 48, ether_addr_to_u64(addr));
	ppe_ft_entry_write(priv, PPE_EG_L3_IF_TBL(PPE_FT_EG_IF_BASE + free), w,
			   PPE_EG_L3_IF_WORDS);
	ppe_entry_set(w, PPE_MY_MAC_VALID_OFF, 1, 1);
	ppe_ft_entry_write(priv, PPE_MY_MAC_TBL(free), w, PPE_MY_MAC_WORDS);

	ether_addr_copy(ft->mac[free].addr, addr);
	ft->mac[free].refs = 1;
	return free;
}

static void ppe_ft_mac_put(struct qca_ppe_priv *priv, int i)
{
	struct ppe_ft *ft = priv->ft;
	u32 zero[PPE_EG_L3_IF_WORDS] = { };

	if (WARN_ON(!ft->mac[i].refs) || --ft->mac[i].refs)
		return;

	ppe_ft_entry_write(priv, PPE_MY_MAC_TBL(i), zero, PPE_MY_MAC_WORDS);
	ppe_ft_entry_write(priv, PPE_EG_L3_IF_TBL(PPE_FT_EG_IF_BASE + i), zero,
			   PPE_EG_L3_IF_WORDS);
}

static int ppe_ft_pub_ip_get(struct qca_ppe_priv *priv, __be32 addr)
{
	struct ppe_ft *ft = priv->ft;
	int i, free = -1;

	for (i = 0; i < PPE_IN_PUB_IP_ENTRIES; i++) {
		if (ft->pub_ip[i].refs && ft->pub_ip[i].addr == addr) {
			ft->pub_ip[i].refs++;
			return i;
		}
		if (!ft->pub_ip[i].refs && free < 0)
			free = i;
	}
	if (free < 0)
		return -ENOSPC;

	regmap_write(priv->regmap, PPE_IN_PUB_IP_TBL(free), ntohl(addr));
	ft->pub_ip[free].addr = addr;
	ft->pub_ip[free].refs = 1;
	return free;
}

static void ppe_ft_pub_ip_put(struct qca_ppe_priv *priv, int i)
{
	struct ppe_ft *ft = priv->ft;

	if (WARN_ON(!ft->pub_ip[i].refs) || --ft->pub_ip[i].refs)
		return;

	regmap_write(priv->regmap, PPE_IN_PUB_IP_TBL(i), 0);
}

/* Next hops are shared by content; entry 0 is left alone. */
static int ppe_ft_nexthop_get(struct qca_ppe_priv *priv, const u32 *w)
{
	struct ppe_ft *ft = priv->ft;
	int i, free = -1;

	for (i = 1; i < PPE_IN_NEXTHOP_ENTRIES; i++) {
		struct ppe_ft_nexthop *nh = &ft->nexthop[i];

		if (nh->refs && !memcmp(nh->w, w, sizeof(nh->w))) {
			nh->refs++;
			return i;
		}
		if (!nh->refs && free < 0)
			free = i;
	}
	if (free < 0)
		return -ENOSPC;

	memcpy(ft->nexthop[free].w, w, sizeof(ft->nexthop[free].w));
	ppe_ft_entry_write(priv, PPE_IN_NEXTHOP_TBL(free), w,
			   PPE_IN_NEXTHOP_WORDS);
	ft->nexthop[free].refs = 1;
	return free;
}

static void ppe_ft_nexthop_put(struct qca_ppe_priv *priv, int i)
{
	struct ppe_ft *ft = priv->ft;
	u32 zero[PPE_IN_NEXTHOP_WORDS] = { };

	if (WARN_ON(!ft->nexthop[i].refs) || --ft->nexthop[i].refs)
		return;

	ppe_ft_entry_write(priv, PPE_IN_NEXTHOP_TBL(i), zero,
			   PPE_IN_NEXTHOP_WORDS);
}

static void ppe_ft_counters(struct qca_ppe_priv *priv, u16 idx, u32 *pkts,
			    u64 *bytes)
{
	u32 w[PPE_IN_FLOW_CNT_WORDS];

	if (regmap_bulk_read(priv->regmap, PPE_IN_FLOW_CNT_TBL(idx), w,
			     ARRAY_SIZE(w))) {
		*pkts = 0;
		*bytes = 0;
		return;
	}

	*pkts = w[0];
	*bytes = ppe_entry_get(w, 32, 40);
}

static int ppe_ft_mangle_eth(const struct flow_action_entry *act, void *eth)
{
	void *dest = eth + act->mangle.offset;
	const void *src = &act->mangle.val;

	if (act->mangle.offset > 8)
		return -EOPNOTSUPP;

	if (act->mangle.mask == 0xffff) {
		src += 2;
		dest += 2;
	}

	memcpy(dest, src, act->mangle.mask ? 2 : 4);
	return 0;
}

static int ppe_ft_mangle_ports(const struct flow_action_entry *act,
			       struct ppe_ft_rule *r)
{
	u32 val = ntohl(act->mangle.val);

	switch (act->mangle.offset) {
	case 0:
		if (act->mangle.mask == ~htonl(0xffff))
			r->new_dport = cpu_to_be16(val);
		else
			r->new_sport = cpu_to_be16(val >> 16);
		break;
	case 2:
		r->new_dport = cpu_to_be16(val);
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int ppe_ft_mangle_ipv4(const struct flow_action_entry *act,
			      struct ppe_ft_rule *r)
{
	switch (act->mangle.offset) {
	case offsetof(struct iphdr, saddr):
		r->new_saddr = act->mangle.val;
		break;
	case offsetof(struct iphdr, daddr):
		r->new_daddr = act->mangle.val;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int ppe_ft_parse(struct qca_ppe_priv *priv, struct flow_rule *rule,
			struct ppe_ft_rule *r)
{
	struct flow_match_control control;
	struct flow_match_basic basic;
	struct flow_match_ipv4_addrs addrs;
	struct flow_match_ports ports;
	struct net_device *odev = NULL;
	struct flow_action_entry *act;
	struct dsa_port *dp;
	int i, ret;

	if (rule->match.dissector->used_keys &
	    (BIT_ULL(FLOW_DISSECTOR_KEY_VLAN) |
	     BIT_ULL(FLOW_DISSECTOR_KEY_CVLAN) |
	     BIT_ULL(FLOW_DISSECTOR_KEY_PPPOE)))
		return -EOPNOTSUPP;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS))
		return -EOPNOTSUPP;

	flow_rule_match_control(rule, &control);
	if (control.key->addr_type != FLOW_DISSECTOR_KEY_IPV4_ADDRS)
		return -EOPNOTSUPP;

	flow_rule_match_basic(rule, &basic);
	if (basic.key->n_proto != htons(ETH_P_IP))
		return -EOPNOTSUPP;
	switch (basic.key->ip_proto) {
	case IPPROTO_TCP:
		r->proto = 1;
		break;
	case IPPROTO_UDP:
		r->proto = 2;
		break;
	default:
		return -EOPNOTSUPP;
	}

	flow_rule_match_ipv4_addrs(rule, &addrs);
	r->saddr = r->new_saddr = addrs.key->src;
	r->daddr = r->new_daddr = addrs.key->dst;

	flow_rule_match_ports(rule, &ports);
	r->sport = r->new_sport = ports.key->src;
	r->dport = r->new_dport = ports.key->dst;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			switch (act->mangle.htype) {
			case FLOW_ACT_MANGLE_HDR_TYPE_ETH:
				ret = ppe_ft_mangle_eth(act, &r->eth);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_IP4:
				ret = ppe_ft_mangle_ipv4(act, r);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
			case FLOW_ACT_MANGLE_HDR_TYPE_UDP:
				ret = ppe_ft_mangle_ports(act, r);
				break;
			default:
				ret = -EOPNOTSUPP;
			}
			if (ret)
				return ret;
			break;
		case FLOW_ACTION_REDIRECT:
			odev = act->dev;
			break;
		case FLOW_ACTION_CSUM:
			break;
		default:
			return -EOPNOTSUPP;
		}
	}

	if (!odev || !is_valid_ether_addr(r->eth.h_dest) ||
	    !is_valid_ether_addr(r->eth.h_source))
		return -EOPNOTSUPP;

	/* The flowtable resolves its output through the DSA user port, so a
	 * flow leaving anywhere else is one this switch cannot forward.
	 */
	dp = dsa_port_from_netdev(odev);
	if (IS_ERR(dp) || dp->ds != &priv->ds)
		return -EOPNOTSUPP;
	r->port = dp->index;

	return 0;
}

static int ppe_ft_replace(struct qca_ppe_priv *priv,
			  struct flow_cls_offload *cls)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(cls);
	u32 host[PPE_HOST_V4_WORDS] = { };
	u32 flow[PPE_IN_FLOW_V4_WORDS] = { };
	u32 nh[PPE_IN_NEXTHOP_WORDS] = { };
	struct ppe_ft *ft = priv->ft;
	struct ppe_ft_rule r = { };
	struct ppe_ft_flow *f;
	bool snat, dnat;
	u16 idx = 0, nat_port;
	u32 val;
	int fwd, ret;

	ret = ppe_ft_parse(priv, rule, &r);
	if (ret)
		return ret;

	snat = r.new_saddr != r.saddr || r.new_sport != r.sport;
	dnat = r.new_daddr != r.daddr || r.new_dport != r.dport;
	if (snat && dnat)
		return -EOPNOTSUPP;

	if (rhashtable_lookup_fast(&ft->flows, &cls->cookie, ppe_ft_ht_params))
		return -EEXIST;

	f = kzalloc(sizeof(*f), GFP_KERNEL);
	if (!f)
		return -ENOMEM;
	f->cookie = cls->cookie;
	f->pub_ip = -1;

	ret = ppe_ft_mac_get(priv, r.eth.h_source);
	if (ret < 0)
		goto err_free;
	f->mac = ret;

	ppe_entry_set(nh, PPE_NEXTHOP_TYPE_OFF, 1, 1);
	ppe_entry_set(nh, PPE_NEXTHOP_PORT_OFF, 8, r.port);
	ppe_entry_set(nh, PPE_NEXTHOP_L3_IF_OFF, 8, PPE_FT_EG_IF_BASE + f->mac);
	ppe_entry_set(nh, PPE_NEXTHOP_MAC_OFF, 48,
		      ether_addr_to_u64(r.eth.h_dest));

	if (snat) {
		ret = ppe_ft_pub_ip_get(priv, r.new_saddr);
		if (ret < 0)
			goto err_mac;
		f->pub_ip = ret;
		ppe_entry_set(nh, PPE_NEXTHOP_PUB_IP_OFF, 4, f->pub_ip);
		fwd = PPE_FLOW_FWD_SNAT;
		nat_port = ntohs(r.new_sport);
	} else if (dnat) {
		ppe_entry_set(nh, PPE_NEXTHOP_DNAT_IP_OFF, 32,
			      ntohl(r.new_daddr));
		fwd = PPE_FLOW_FWD_DNAT;
		nat_port = ntohs(r.new_dport);
	} else {
		fwd = PPE_FLOW_FWD_ROUTE;
		nat_port = 0;
	}

	ret = ppe_ft_nexthop_get(priv, nh);
	if (ret < 0)
		goto err_pub_ip;
	f->nexthop = ret;

	/* The host entry sends what is routed to the address without a flow
	 * to the CPU, as everything was before.
	 */
	ppe_entry_set(host, PPE_HOST_VALID_OFF, 1, 1);
	ppe_entry_set(host, PPE_HOST_FWD_CMD_OFF, 2, PPE_FT_FWD_RDT_CPU);
	ppe_entry_set(host, PPE_HOST_LAN_WAN_OFF, 1, 1);
	ppe_entry_set(host, PPE_HOST_IP_OFF, 32, ntohl(r.daddr));

	ppe_entry_set(flow, PPE_IN_FLOW_VALID_OFF, 1, 1);
	ppe_entry_set(flow, PPE_IN_FLOW_HOST_TYPE_OFF, 1, 1);
	ppe_entry_set(flow, PPE_IN_FLOW_PROTO_OFF, 2, r.proto);
	ppe_entry_set(flow, PPE_IN_FLOW_AGE_OFF, 2, 3);
	ppe_entry_set(flow, PPE_IN_FLOW_FWD_TYPE_OFF, 3, fwd);
	ppe_entry_set(flow, PPE_IN_FLOW_NEXTHOP_OFF, 12, f->nexthop);
	ppe_entry_set(flow, PPE_IN_FLOW_NAT_PORT_OFF, 16, nat_port);
	ppe_entry_set(flow, PPE_IN_FLOW_IP_OFF, 32, ntohl(r.saddr));
	ppe_entry_set(flow, PPE_IN_FLOW_SPORT_OFF, 16, ntohs(r.sport));
	ppe_entry_set(flow, PPE_IN_FLOW_DPORT_OFF, 16, ntohs(r.dport));

	ppe_ft_entry_write(priv, PPE_FLOW_HOST_OP_DATA(0), host,
			   ARRAY_SIZE(host));
	regmap_write(priv->regmap, PPE_IN_FLOW_HOST_OP,
		     FIELD_PREP(PPE_IN_FLOW_HOST_OP_HASH_BLOCK, 3));
	ppe_ft_entry_write(priv, PPE_IN_FLOW_OP_DATA(0), flow,
			   ARRAY_SIZE(flow));

	ret = ppe_ft_flow_op(priv, 0, false, true, &idx);
	if (ret)
		goto err_nexthop;

	regmap_read(priv->regmap, PPE_FLOW_HOST_OP_RSLT, &val);
	f->flow_idx = idx;
	f->host_idx = FIELD_GET(PPE_FLOW_HOST_RSLT_INDEX, val);
	if (f->host_idx >= PPE_HOST_ENTRIES) {
		ret = -EIO;
		goto err_flow;
	}
	ft->host_refs[f->host_idx]++;

	ret = rhashtable_insert_fast(&ft->flows, &f->node, ppe_ft_ht_params);
	if (ret)
		goto err_host;

	if (ft->owner[idx])
		ft->owner[idx]->flow_idx = PPE_IN_FLOW_ENTRIES;
	ft->owner[idx] = f;
	ppe_ft_counters(priv, idx, &f->pkts, &f->bytes);

	return 0;

err_host:
	ppe_ft_host_put(priv, f->host_idx);
err_flow:
	ppe_ft_flow_hw_del(priv, idx);
err_nexthop:
	ppe_ft_nexthop_put(priv, f->nexthop);
err_pub_ip:
	if (f->pub_ip >= 0)
		ppe_ft_pub_ip_put(priv, f->pub_ip);
err_mac:
	ppe_ft_mac_put(priv, f->mac);
err_free:
	kfree(f);
	return ret;
}

static void ppe_ft_flow_free(struct qca_ppe_priv *priv, struct ppe_ft_flow *f)
{
	struct ppe_ft *ft = priv->ft;

	/* A flow whose key another one rewrote no longer owns the entry. */
	if (f->flow_idx < PPE_IN_FLOW_ENTRIES) {
		ppe_ft_flow_hw_del(priv, f->flow_idx);
		ft->owner[f->flow_idx] = NULL;
	}
	ppe_ft_host_put(priv, f->host_idx);
	ppe_ft_nexthop_put(priv, f->nexthop);
	if (f->pub_ip >= 0)
		ppe_ft_pub_ip_put(priv, f->pub_ip);
	ppe_ft_mac_put(priv, f->mac);
	kfree(f);
}

static int ppe_ft_destroy(struct qca_ppe_priv *priv,
			  struct flow_cls_offload *cls)
{
	struct ppe_ft *ft = priv->ft;
	struct ppe_ft_flow *f;

	f = rhashtable_lookup_fast(&ft->flows, &cls->cookie, ppe_ft_ht_params);
	if (!f)
		return -ENOENT;

	rhashtable_remove_fast(&ft->flows, &f->node, ppe_ft_ht_params);
	ppe_ft_flow_free(priv, f);
	return 0;
}

static int ppe_ft_stats(struct qca_ppe_priv *priv, struct flow_cls_offload *cls)
{
	struct ppe_ft *ft = priv->ft;
	struct ppe_ft_flow *f;
	u32 pkts;
	u64 bytes;

	f = rhashtable_lookup_fast(&ft->flows, &cls->cookie, ppe_ft_ht_params);
	if (!f)
		return -ENOENT;
	if (f->flow_idx >= PPE_IN_FLOW_ENTRIES)
		return 0;

	ppe_ft_counters(priv, f->flow_idx, &pkts, &bytes);
	if (pkts == f->pkts)
		return 0;

	flow_stats_update(&cls->stats, (bytes - f->bytes) & GENMASK_ULL(39, 0),
			  pkts - f->pkts, 0, jiffies,
			  FLOW_ACTION_HW_STATS_DELAYED);
	f->pkts = pkts;
	f->bytes = bytes;
	return 0;
}

static int ppe_ft_block_cb(enum tc_setup_type type, void *type_data,
			   void *cb_priv)
{
	struct flow_cls_offload *cls = type_data;
	struct qca_ppe_priv *priv = cb_priv;

	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;

	guard(mutex)(&priv->ft->lock);

	switch (cls->command) {
	case FLOW_CLS_REPLACE:
		return ppe_ft_replace(priv, cls);
	case FLOW_CLS_DESTROY:
		return ppe_ft_destroy(priv, cls);
	case FLOW_CLS_STATS:
		return ppe_ft_stats(priv, cls);
	default:
		return -EOPNOTSUPP;
	}
}

/* Each user port gets an ingress L3 interface of its own index, with IPv4
 * routing on and every MY_MAC entry accepted. A frame whose TTL runs out goes
 * to the CPU, where Linux answers it.
 */
static void ppe_ft_l3_if_enable(struct qca_ppe_priv *priv, int port,
				unsigned int mtu)
{
	u32 w[PPE_IN_L3_IF_WORDS] = { };
	u32 vp[PPE_L3_VP_WORDS];
	int i;

	for (i = 0; i < PPE_IN_L3_IF_WORDS; i++)
		regmap_read(priv->regmap, PPE_IN_L3_IF_TBL(port) + i * 4,
			    &priv->ft->l3_if[port][i]);

	ppe_entry_set(w, PPE_IN_L3_IF_MRU_OFF, 14, mtu);
	ppe_entry_set(w, PPE_IN_L3_IF_MTU_OFF, 14, mtu);
	ppe_entry_set(w, PPE_IN_L3_IF_V4_ROUTE_OFF, 1, 1);
	ppe_entry_set(w, PPE_IN_L3_IF_TTL_CMD_OFF, 2, PPE_FT_FWD_RDT_CPU);
	ppe_entry_set(w, PPE_IN_L3_IF_MAC_BITMAP_OFF, 8,
		      BIT(PPE_MY_MAC_ENTRIES) - 1);
	ppe_ft_entry_write(priv, PPE_IN_L3_IF_TBL(port), w, ARRAY_SIZE(w));

	guard(mutex)(&priv->vlan_lock);
	for (i = 0; i < PPE_L3_VP_WORDS; i++)
		regmap_read(priv->regmap, PPE_L3_VP_PORT_TBL(port) + i * 4,
			    &vp[i]);
	vp[0] &= ~(PPE_L3_VP_L3_IF_VALID | PPE_L3_VP_L3_IF);
	vp[0] |= PPE_L3_VP_L3_IF_VALID | FIELD_PREP(PPE_L3_VP_L3_IF, port);
	ppe_ft_entry_write(priv, PPE_L3_VP_PORT_TBL(port), vp, ARRAY_SIZE(vp));
}

static void ppe_ft_l3_if_disable(struct qca_ppe_priv *priv, int port)
{
	u32 vp[PPE_L3_VP_WORDS];
	int i;

	ppe_ft_entry_write(priv, PPE_IN_L3_IF_TBL(port), priv->ft->l3_if[port],
			   PPE_IN_L3_IF_WORDS);

	guard(mutex)(&priv->vlan_lock);
	for (i = 0; i < PPE_L3_VP_WORDS; i++)
		regmap_read(priv->regmap, PPE_L3_VP_PORT_TBL(port) + i * 4,
			    &vp[i]);
	vp[0] &= ~(PPE_L3_VP_L3_IF_VALID | PPE_L3_VP_L3_IF);
	ppe_ft_entry_write(priv, PPE_L3_VP_PORT_TBL(port), vp, ARRAY_SIZE(vp));
}

static void ppe_ft_enable(struct qca_ppe_priv *priv)
{
	struct ppe_ft *ft = priv->ft;
	struct dsa_port *dp;
	u32 ctrl = 0;
	int d;

	dsa_switch_for_each_user_port(dp, &priv->ds)
		ppe_ft_l3_if_enable(priv, dp->index, dp->user->mtu);

	/* A miss and a fragment go to the CPU. TCP_SPECIAL is left off: on v1
	 * it bypasses the flow for most data segments of a bulk transfer, not
	 * just SYN/FIN/RST, so a closed connection is left to the flowtable's
	 * timeout as with other hardware that offloads TCP.
	 */
	for (d = 0; d < PPE_FLOW_CTRL1_DIRS; d++)
		ctrl |= (FIELD_PREP(PPE_FLOW_CTRL1_MISS, PPE_FT_FWD_RDT_CPU) |
			 PPE_FLOW_CTRL1_FRAG_BYPASS |
			 PPE_FLOW_CTRL1_KEY_SEL) << (d * PPE_FLOW_CTRL1_DIR_SHIFT);

	regmap_read(priv->regmap, PPE_FLOW_CTRL1(0), &ft->flow_ctrl1);
	regmap_write(priv->regmap, PPE_FLOW_CTRL1(0), ctrl);
	regmap_set_bits(priv->regmap, PPE_FLOW_CTRL0, PPE_FLOW_CTRL0_EN);
}

static void ppe_ft_disable(struct qca_ppe_priv *priv)
{
	struct ppe_ft *ft = priv->ft;
	struct dsa_port *dp;

	regmap_write(priv->regmap, PPE_FLOW_CTRL1(0), ft->flow_ctrl1);

	dsa_switch_for_each_user_port(dp, &priv->ds)
		ppe_ft_l3_if_disable(priv, dp->index);
}

static void ppe_ft_flow_free_cb(void *ptr, void *arg)
{
	ppe_ft_flow_free(arg, ptr);
}

int qca_ppe_setup_ft(struct qca_ppe_priv *priv, int port, void *type_data)
{
	struct flow_block_offload *f = type_data;
	struct ppe_ft *ft = priv->ft;
	struct flow_block_cb *block_cb;

	if (!dsa_is_cpu_port(&priv->ds, port) ||
	    f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	f->driver_block_list = &ppe_ft_block_cb_list;

	switch (f->command) {
	case FLOW_BLOCK_BIND:
		block_cb = flow_block_cb_lookup(f->block, ppe_ft_block_cb, priv);
		if (block_cb) {
			flow_block_cb_incref(block_cb);
			return 0;
		}

		block_cb = flow_block_cb_alloc(ppe_ft_block_cb, priv, priv,
					       NULL);
		if (IS_ERR(block_cb))
			return PTR_ERR(block_cb);

		scoped_guard(mutex, &ft->lock)
			if (!ft->binds++)
				ppe_ft_enable(priv);

		flow_block_cb_incref(block_cb);
		flow_block_cb_add(block_cb, f);
		list_add_tail(&block_cb->driver_list, &ppe_ft_block_cb_list);
		return 0;
	case FLOW_BLOCK_UNBIND:
		block_cb = flow_block_cb_lookup(f->block, ppe_ft_block_cb, priv);
		if (!block_cb)
			return -ENOENT;

		if (!flow_block_cb_decref(block_cb)) {
			flow_block_cb_remove(block_cb, f);
			list_del(&block_cb->driver_list);

			scoped_guard(mutex, &ft->lock)
				if (!--ft->binds)
					ppe_ft_disable(priv);
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

int ppe_ft_init(struct qca_ppe_priv *priv)
{
	struct ppe_ft *ft;
	int ret;

	ft = kvzalloc(sizeof(*ft), GFP_KERNEL);
	if (!ft)
		return -ENOMEM;

	ret = rhashtable_init(&ft->flows, &ppe_ft_ht_params);
	if (ret) {
		kvfree(ft);
		return ret;
	}

	mutex_init(&ft->lock);
	priv->ft = ft;
	return 0;
}

void ppe_ft_exit(struct qca_ppe_priv *priv)
{
	struct ppe_ft *ft = priv->ft;

	if (!ft)
		return;

	scoped_guard(mutex, &ft->lock) {
		rhashtable_free_and_destroy(&ft->flows, ppe_ft_flow_free_cb,
					    priv);
		if (ft->binds)
			ppe_ft_disable(priv);
	}

	mutex_destroy(&ft->lock);
	kvfree(ft);
	priv->ft = NULL;
}
