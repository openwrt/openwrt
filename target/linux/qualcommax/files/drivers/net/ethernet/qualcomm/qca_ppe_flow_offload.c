// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/* Copyright (c) 2026 Julius Bairaktaris <julius@bairaktaris.de> */
/* netfilter flowtable offload for the Qualcomm PPE.
 *
 * The kernel hands us one rule per direction of a connection; each becomes one
 * PPE flow entry, and the two are unrelated as far as this driver is concerned.
 * A rule that rewrites the source address is an SNAT entry, one that rewrites
 * the destination is a DNAT entry, and one that rewrites neither is a plain
 * route. The hardware overlays the SNAT and DNAT fields on the same bits, so a
 * rule that rewrites both is declined and stays in software.
 *
 * Three side tables carry what does not fit in the flow entry: the egress L3
 * interface holds the source MAC and the PPPoE session, the nexthop holds the
 * destination MAC, the egress port, the VLAN tag and the DNAT address, and the
 * public-address table holds the SNAT address. All three are shared between
 * flows that need the same contents and are reference counted.
 */

#include <linux/bitfield.h>
#include <linux/if_pppox.h>
#include <linux/if_vlan.h>
#include <linux/netdevice.h>
#include <linux/rhashtable.h>
#include <net/dsa.h>
#include <net/flow_offload.h>
#include <net/netfilter/nf_flow_table.h>
#include <net/pkt_cls.h>

#include "qca_ppe.h"

struct ppe_flow_data {
	struct ethhdr eth;
	u16 addr_type;
	u8 l4proto;

	__be32 v4_src, v4_dst, v4_src_new, v4_dst_new;
	struct in6_addr v6_src, v6_dst;
	__be16 sport, dport, sport_new, dport_new;

	u16 vlan_id;
	bool vlan_valid;
	u16 pppoe_sid;
	bool pppoe_valid;

	struct net_device *odev;
	u8 priority;
};

struct ppe_flow_entry {
	struct rhash_head node;
	struct list_head list;
	struct flow_block *block;
	unsigned long cookie;
	u32 index;
	u32 words[PPE_FLOW_ENTRY_WORDS_V6];
	u8 nwords;
	u8 src_if;
	u32 host_index;
	int nexthop;
	int my_mac;
	int l3_if;
	int eg_l3_if;
	int pub_ip;
	int wan_port;
	int wan_iport;
	u8 iport;
	u8 oport;
	u64 packets;
	u64 bytes;
	unsigned long last_used;
};

/* One bound flowtable block. Entries remember which block installed them,
 * because a dying flowtable unbinds from every port before the core flushes
 * its flows - the FLOW_CLS_DESTROY commands never arrive, and the block
 * callback's release, which the core invokes after the last unbind, is the
 * one point where that block's flows are known dead.
 */
struct ppe_flow_block {
	struct qca_ppe_priv *priv;
	struct flow_block *block;
};

static const struct rhashtable_params ppe_flow_ht_params = {
	.head_offset = offsetof(struct ppe_flow_entry, node),
	.key_offset = offsetof(struct ppe_flow_entry, cookie),
	.key_len = sizeof(unsigned long),
	.automatic_shrinking = true,
};

/* Reference-counted allocation over one hardware side table. Entries are
 * compared by content so that every flow leaving by the same nexthop, or
 * sourced from the same MAC, shares one slot — without that, the 2560 nexthops
 * would run out long before the 4096 flow entries do.
 */
static int ppe_res_get(struct ppe_res *tbl, u32 count, const u32 *words,
		       int nwords)
{
	int free_idx = -1;
	u32 i;

	for (i = 0; i < count; i++) {
		if (!tbl[i].refcount) {
			if (free_idx < 0)
				free_idx = i;
			continue;
		}
		if (!memcmp(tbl[i].words, words, nwords * sizeof(*words))) {
			tbl[i].refcount++;
			return i;
		}
	}

	if (free_idx < 0)
		return -ENOSPC;

	memcpy(tbl[free_idx].words, words, nwords * sizeof(*words));
	tbl[free_idx].refcount = 1;

	return free_idx;
}

static bool ppe_res_put(struct ppe_res *tbl, int idx)
{
	if (idx < 0)
		return false;

	return --tbl[idx].refcount == 0;
}

/* The hardware find-or-creates a host entry from the source address, so every
 * flow from one host lands on the same slot and the slot outlives any single
 * flow that referenced it.
 */
static void ppe_host_ref_get(struct qca_ppe_priv *priv, u32 index)
{
	priv->host_ref[index]++;
}

static void ppe_host_ref_put(struct qca_ppe_priv *priv, u32 index)
{
	if (!--priv->host_ref[index])
		ppe_host_del(priv, index);
}

/* The age of the entry in @index, or -ENOENT if the slot no longer holds this
 * flow. Word 0 on its own is not an identity - its host index, protocol and
 * forwarding type are shared by every flow of one client - so a slot the
 * hardware aged out and handed to a sibling would read back as still ours.
 */
static int ppe_flow_entry_age(struct qca_ppe_priv *priv,
			      struct ppe_flow_entry *entry)
{
	u32 w[PPE_FLOW_ENTRY_WORDS_V6];
	u32 age;
	int ret;

	/* Only a read that says the slot is empty means the flow is gone; any
	 * other failure leaves it unknown, and reporting "gone" would strand a
	 * live entry pointing at side-table slots about to be handed on.
	 */
	ret = ppe_flow_entry_read(priv, entry->index, w, entry->nwords);
	if (ret)
		return ret;

	age = FIELD_GET(PPE_FLOW_E_AGE_MASK, w[0]);
	w[0] &= ~PPE_FLOW_E_AGE_MASK;

	if (memcmp(w, entry->words, entry->nwords * sizeof(*w)))
		return -ENOENT;

	return age;
}

static void ppe_tbl_write(struct qca_ppe_priv *priv, u32 reg, const u32 *words,
			  int nwords)
{
	int i;

	for (i = 0; i < nwords; i++)
		regmap_write(priv->regmap, reg + i * 4, words[i]);
}

static void ppe_tbl_clear(struct qca_ppe_priv *priv, u32 reg, int nwords)
{
	int i;

	for (i = 0; i < nwords; i++)
		regmap_write(priv->regmap, reg + i * 4, 0);
}

/* An entry of a multi-word table takes the write to its last word as the
 * commit: a word 0 written on its own is staged and reads back unchanged.
 *
 * MRU is also how an interface is retired: teardown clears the entry, leaving
 * MRU 0, so a frame still matching a flow whose interface is gone fails the
 * MRU check and L3_ROUTE_CTRL decides what happens to it. The driver never
 * writes that register and depends on its reset, which redirects to the CPU;
 * writing it is how teardown would become a black hole.
 */
static void ppe_l3_if_mtu_set(struct qca_ppe_priv *priv, u32 vsi, u32 mtu)
{
	u32 words[PPE_L3_IF_WORDS];
	int i;

	/* A bridge takes an mtu wider than the field holds, and the value that
	 * survives the truncation reads back as a retired interface.
	 */
	mtu = min_t(u32, mtu, FIELD_MAX(PPE_L3_IF_MRU));

	for (i = 0; i < PPE_L3_IF_WORDS; i++)
		regmap_read(priv->regmap, PPE_IN_L3_IF_TBL(vsi) + i * 4,
			    &words[i]);

	words[0] &= ~(PPE_L3_IF_MRU | PPE_L3_IF_MTU);
	words[0] |= FIELD_PREP(PPE_L3_IF_MRU, mtu) |
		    FIELD_PREP(PPE_L3_IF_MTU, mtu);

	ppe_tbl_write(priv, PPE_IN_L3_IF_TBL(vsi), words, PPE_L3_IF_WORDS);
}

/* The egress half of an L3 interface answers the mtu check and nothing else:
 * no route enables and no my-mac bitmap, because no VSI resolves to it.
 */
static void ppe_eg_l3_if_mtu_set(struct qca_ppe_priv *priv, u32 idx, u32 mtu)
{
	u32 words[PPE_L3_IF_WORDS] = {};

	words[0] = FIELD_PREP(PPE_L3_IF_MRU, mtu) |
		   FIELD_PREP(PPE_L3_IF_MTU, mtu);

	ppe_tbl_write(priv, PPE_IN_L3_IF_TBL(idx), words, PPE_L3_IF_WORDS);
}

/* Ports outside a bridge sit on VSI 0 since setup. */
static u32 ppe_port_l3_vsi(struct qca_ppe_priv *priv, int port)
{
	return priv->port_vsi[port] == PPE_VSI_INVALID ? 0 :
	       priv->port_vsi[port];
}

static void ppe_entry_set_addr6(u32 *words, u32 offset,
				const struct in6_addr *addr)
{
	int i;

	/* The hardware stores the address least significant word first. */
	for (i = 3; i >= 0; i--)
		ppe_entry_set(words, offset + (3 - i) * 32, 32,
			      ntohl(addr->s6_addr32[i]));
}

static int ppe_flow_proto(u8 l4proto)
{
	switch (l4proto) {
	case IPPROTO_TCP:
		return PPE_FLOW_PROTO_TCP;
	case IPPROTO_UDP:
		return PPE_FLOW_PROTO_UDP;
	default:
		return -EOPNOTSUPP;
	}
}

static int ppe_flow_mangle_eth(const struct flow_action_entry *act,
			       struct ethhdr *eth)
{
	void *dest = (void *)eth + act->mangle.offset;
	const void *src = &act->mangle.val;

	/* The core writes the egress header as mangles at byte offsets into
	 * struct ethhdr, so the destination follows from the offset alone.
	 */
	if (act->mangle.offset > 8)
		return -EOPNOTSUPP;

	if (act->mangle.mask == 0xffff) {
		src += 2;
		dest += 2;
	}

	memcpy(dest, src, act->mangle.mask ? 2 : 4);

	return 0;
}

static int ppe_flow_mangle_ports(const struct flow_action_entry *act,
				 struct ppe_flow_data *data)
{
	u32 val = ntohl(act->mangle.val);

	switch (act->mangle.offset) {
	case 0:
		if (act->mangle.mask == ~htonl(0xffff))
			data->dport_new = cpu_to_be16(val);
		else
			data->sport_new = cpu_to_be16(val >> 16);
		break;
	case 2:
		data->dport_new = cpu_to_be16(val);
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int ppe_flow_mangle_ipv4(const struct flow_action_entry *act,
				struct ppe_flow_data *data)
{
	__be32 *dest;

	switch (act->mangle.offset) {
	case offsetof(struct iphdr, saddr):
		dest = &data->v4_src_new;
		break;
	case offsetof(struct iphdr, daddr):
		dest = &data->v4_dst_new;
		break;
	default:
		return -EOPNOTSUPP;
	}

	memcpy(dest, &act->mangle.val, sizeof(u32));

	return 0;
}

static int ppe_flow_port_by_ifindex(struct qca_ppe_priv *priv, int ifindex)
{
	struct dsa_port *dp;

	dsa_switch_for_each_user_port(dp, &priv->ds)
		if (dp->user && dp->user->ifindex == ifindex)
			return dp->index;

	return -EOPNOTSUPP;
}

/* Make a tagged PPPoE WAN port route its ingress traffic in hardware, so the
 * download direction of an offloaded connection reaches the flow lookup on its
 * inner tuple.
 *
 * A dedicated VSI keeps the uplink out of any L2 domain: the ingress VLAN is
 * classified into it with the 802.1Q tag stripped, the VSI carries a route-
 * and PPPoE-terminating L3 interface holding the router's MAC, and the PPPoE
 * session id is recognised so the header is parsed through to the inner IP.
 * Everything is shared by every flow on the port and torn down with the last
 * of them. A frame that misses the flow table is still forwarded to the CPU,
 * with the tag the hardware stripped re-added on that path, so the software
 * PPPoE stack sees on-wire frames throughout.
 */
static int ppe_wan_ingress_get(struct qca_ppe_priv *priv, int port, u16 sid,
			       bool vlan_valid, u16 vlan_id, const u8 *mac,
			       u32 mtu)
{
	u32 words[PPE_MY_MAC_WORDS] = {};
	int vsi, xlt = -1, ret;

	if (priv->wan_ref[port]++) {
		/* A re-dialled session has a new id, and an ingress still
		 * keyed to the dead one silently un-offloads every download on
		 * this uplink: the entries stay valid and correct, the frame
		 * is never parsed through to the tuple they key on, and the
		 * flow lookup is never reached. It is cheaper to write the
		 * session on every install than to keep a copy of it that
		 * could be wrong.
		 */
		regmap_write(priv->regmap, PPE_PPPOE_SESSION(port),
			     FIELD_PREP(PPE_PPPOE_SESSION_ID, sid) |
			     FIELD_PREP(PPE_PPPOE_SESSION_PORT_BMP, BIT(port)) |
			     FIELD_PREP(PPE_PPPOE_SESSION_L3_IF,
					priv->wan_vsi[port]));
		return 0;
	}

	/* The translation rule shares one table with the bridge VLANs, so its
	 * index comes from the allocator they share.
	 */
	if (vlan_valid) {
		xlt = ppe_xlt_idx_alloc(priv);
		if (xlt < 0) {
			priv->wan_ref[port]--;
			return xlt;
		}
	}

	vsi = ppe_vsi_alloc(priv);
	if (vsi < 0) {
		ret = vsi;
		goto err_xlt;
	}
	priv->wan_vsi[port] = vsi;
	ppe_vsi_member_set(priv, vsi, BIT(port) | BIT(QCA_PPE_CPU_PORT));

	ppe_entry_set(words, PPE_MY_MAC_ADDR_OFF, PPE_MY_MAC_ADDR_LEN,
		      ether_addr_to_u64(mac));
	ppe_entry_set(words, PPE_MY_MAC_VALID_OFF, PPE_MY_MAC_VALID_LEN, 1);
	ret = ppe_res_get(priv->my_mac, PPE_MY_MAC_ENTRIES, words,
			  PPE_MY_MAC_WORDS);
	if (ret < 0)
		goto err_vsi;
	priv->wan_mymac[port] = ret;
	if (priv->my_mac[ret].refcount == 1)
		ppe_tbl_write(priv, PPE_MY_MAC_TBL(ret), words, PPE_MY_MAC_WORDS);

	regmap_write(priv->regmap, PPE_IN_L3_IF_TBL(vsi),
		     PPE_L3_IF_IPV4_ROUTE_EN | PPE_L3_IF_IPV6_ROUTE_EN);
	regmap_write(priv->regmap, PPE_IN_L3_IF_TBL(vsi) + 4,
		     FIELD_PREP(PPE_L3_IF_TTL_EXCEED_CMD,
				PPE_L3_IF_TTL_EXCEED_TO_CPU) |
		     PPE_L3_IF_TTL_EXCEED_DEACCEL |
		     FIELD_PREP(PPE_L3_IF_MAC_BITMAP, GENMASK(7, 0)) |
		     PPE_L3_IF_PPPOE_EN);
	ppe_l3_if_mtu_set(priv, vsi, mtu);
	regmap_write(priv->regmap, PPE_L3_VSI_TBL(vsi),
		     PPE_L3_VSI_IF_VALID | FIELD_PREP(PPE_L3_VSI_IF_INDEX, vsi));

	regmap_write(priv->regmap, PPE_PPPOE_SESSION(port),
		     FIELD_PREP(PPE_PPPOE_SESSION_ID, sid) |
		     FIELD_PREP(PPE_PPPOE_SESSION_PORT_BMP, BIT(port)) |
		     FIELD_PREP(PPE_PPPOE_SESSION_L3_IF, vsi));
	regmap_write(priv->regmap, PPE_PPPOE_SESSION_EXT(port),
		     PPE_PPPOE_EXT_L3_IF_VALID | PPE_PPPOE_EXT_UC_VALID);

	if (vlan_valid) {
		priv->wan_xlt[port] = xlt;

		/* The L3 stage does not parse through a residual 802.1Q tag:
		 * the PPPoE session is only recognised, and the inner tuple
		 * only reaches the flow lookup, once the rule also strips the
		 * tag. The hardware re-adds it toward the CPU (below), so a
		 * frame that misses the flow table still reaches the software
		 * PPPoE stack in its on-wire form. Action and re-tag are in
		 * place before the rule goes live.
		 */
		regmap_write(priv->regmap, PPE_XLT_ACTION_TBL(xlt),
			     FIELD_PREP(PPE_XLT_CVID_CMD, PPE_XLT_CVID_DEL));
		regmap_write(priv->regmap, PPE_XLT_ACTION_W1(xlt),
			     PPE_XLT_VSI_CMD | FIELD_PREP(PPE_XLT_VSI, vsi));

		/* An egress translation rule, keyed on the VSI, puts the tag
		 * back on everything leaving toward the CPU port. Nothing else
		 * uses the egress table, so the ingress index names its entry
		 * too rather than needing a second allocator.
		 */
		regmap_write(priv->regmap, PPE_EG_XLT_ACTION(xlt),
			     FIELD_PREP(PPE_EG_XLT_CVID_CMD,
					PPE_EG_XLT_CVID_ADD) |
			     FIELD_PREP(PPE_EG_XLT_CVID, vlan_id));
		regmap_write(priv->regmap, PPE_EG_XLT_ACTION_W1(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_RULE(xlt),
			     PPE_EG_XLT_VALID |
			     FIELD_PREP(PPE_EG_XLT_PORT_BMP,
					BIT(QCA_PPE_CPU_PORT)) |
			     PPE_EG_XLT_VSI_INCL |
			     FIELD_PREP(PPE_EG_XLT_VSI, vsi) |
			     PPE_EG_XLT_VSI_VALID |
			     FIELD_PREP(PPE_EG_XLT_SKEY_FMT,
					PPE_XLT_SKEY_UNTAGGED));
		regmap_write(priv->regmap, PPE_EG_XLT_RULE_W1(xlt),
			     FIELD_PREP(PPE_EG_XLT_CKEY_FMT,
					PPE_XLT_SKEY_UNTAGGED));

		/* The frame-format fields are match bitmaps: a zero matches no
		 * frame at all. The uplink is single (C-)tagged, so the S-tag
		 * side must accept the untagged format for the rule to hit.
		 */
		regmap_write(priv->regmap, PPE_XLT_RULE_TBL(xlt),
			     PPE_XLT_VALID |
			     FIELD_PREP(PPE_XLT_PORT_BMP, BIT(port)) |
			     FIELD_PREP(PPE_XLT_SKEY_FMT,
					PPE_XLT_SKEY_UNTAGGED));
		regmap_write(priv->regmap, PPE_XLT_RULE_W1(xlt),
			     FIELD_PREP(PPE_XLT_CKEY_FMT_1,
					PPE_XLT_CKEY_TAGGED >> 1) |
			     PPE_XLT_CKEY_VID_INCL |
			     FIELD_PREP(PPE_XLT_CKEY_VID, vlan_id));
		regmap_write(priv->regmap, PPE_XLT_RULE_TBL(xlt) + 8, 0);
	}

	return 1;

err_vsi:
	ppe_vsi_free(priv, vsi);
	priv->wan_vsi[port] = -1;
err_xlt:
	if (xlt >= 0)
		ppe_xlt_idx_free(priv, &xlt);
	priv->wan_ref[port]--;
	return ret;
}

static void ppe_wan_ingress_put(struct qca_ppe_priv *priv, int port)
{
	int xlt = priv->wan_xlt[port];
	u32 vsi = priv->wan_vsi[port];

	if (--priv->wan_ref[port])
		return;

	if (xlt >= 0) {
		/* Ingress first: the egress rule is what puts the tag back on
		 * a frame the ingress rule stripped, so taking it down first
		 * would surface stripped frames on the port device. Within the
		 * ingress rule the allocator clears the three key words before
		 * the action's, since a key left live over a zeroed action
		 * blackholes every frame it matches.
		 */
		ppe_xlt_idx_free(priv, &priv->wan_xlt[port]);
		regmap_write(priv->regmap, PPE_EG_XLT_RULE(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_RULE_W1(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_ACTION(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_ACTION_W1(xlt), 0);
	}
	regmap_write(priv->regmap, PPE_PPPOE_SESSION(port), 0);
	regmap_write(priv->regmap, PPE_PPPOE_SESSION_EXT(port), 0);
	regmap_write(priv->regmap, PPE_L3_VSI_TBL(vsi), 0);
	ppe_tbl_clear(priv, PPE_IN_L3_IF_TBL(vsi), PPE_L3_IF_WORDS);
	if (ppe_res_put(priv->my_mac, priv->wan_mymac[port]))
		ppe_tbl_clear(priv, PPE_MY_MAC_TBL(priv->wan_mymac[port]),
			      PPE_MY_MAC_WORDS);
	ppe_vsi_free(priv, vsi);
	priv->wan_vsi[port] = -1;
}

/* The VSI the ingress classification puts this rule's packets in - the routing
 * domain the flow belongs to, and the one ingress identifier its hardware entry
 * can carry. A domain that cannot be named is declined rather than encoded as
 * the bare tuple, which would let the flow forward traffic from another VLAN.
 */
static int ppe_flow_ingress_vsi(struct qca_ppe_priv *priv, int iport)
{
	struct qca_ppe_vlan_entry *vlan;

	lockdep_assert_held(&priv->vlan_lock);

	/* A PPPoE uplink is classified into a VSI of its own, and only the
	 * VLAN and session id that classification names reach it.
	 */
	if (priv->wan_ref[iport])
		return priv->wan_vsi[iport];

	/* A VLAN-filtering bridge reclassifies an untagged frame into the VSI
	 * of the port's PVID, so the port's own VSI answers only without one.
	 */
	if (!priv->port_pvid[iport])
		return ppe_port_l3_vsi(priv, iport);

	vlan = ppe_vlan_find(priv, priv->port_br_dev[iport],
			     priv->port_pvid[iport]);

	return vlan ? (int)vlan->vsi : -EOPNOTSUPP;
}

/* The flow lookup only runs on packets the L3 stage accepted, and nothing in
 * the L2 half of this driver sets that up: the ingress interface has to route,
 * and the frame's destination address has to match a MY_MAC entry. Both are
 * built here from the ports the flow actually uses, and torn down with the last
 * flow that needed them.
 *
 * One ingress L3 interface per VSI is enough, and using the VSI number as its
 * index keeps the two in step without a second allocator.
 */
static int ppe_flow_alloc_ingress(struct qca_ppe_priv *priv, int iport,
				  struct ppe_flow_entry *entry)
{
	struct dsa_port *dp = dsa_to_port(&priv->ds, iport);
	u32 words[PPE_NEXTHOP_WORDS] = {};
	struct net_device *l3dev;
	u32 mtu;
	int vsi, ret;

	lockdep_assert_held(&priv->vlan_lock);

	vsi = ppe_flow_ingress_vsi(priv, iport);
	if (vsi < 0)
		return vsi;

	entry->src_if = vsi;

	/* A PPPoE uplink's ingress interface belongs to the uplink, so take a
	 * reference rather than programming anything: a sibling flow going away
	 * must not pull the classification out from under this one.
	 */
	if (priv->wan_ref[iport]) {
		priv->wan_ref[iport]++;
		entry->wan_iport = iport;
		return 0;
	}

	/* The address the packet is sent to is the address of the device that
	 * routes for this port, which is the bridge when there is one. That
	 * pointer is this driver's own, kept in step under this lock; DSA's is
	 * only safe to follow under rtnl, which the flowtable does not hold.
	 */
	l3dev = priv->port_br_dev[iport];
	if (!l3dev)
		l3dev = dp->user;

	ppe_entry_set(words, PPE_MY_MAC_ADDR_OFF, PPE_MY_MAC_ADDR_LEN,
		      ether_addr_to_u64(l3dev->dev_addr));
	ppe_entry_set(words, PPE_MY_MAC_VALID_OFF, PPE_MY_MAC_VALID_LEN, 1);

	ret = ppe_res_get(priv->my_mac, PPE_MY_MAC_ENTRIES, words,
			  PPE_MY_MAC_WORDS);
	if (ret < 0)
		return ret;
	entry->my_mac = ret;
	if (priv->my_mac[ret].refcount == 1)
		ppe_tbl_write(priv, PPE_MY_MAC_TBL(ret), words,
			      PPE_MY_MAC_WORDS);

	entry->l3_if = vsi;
	if (priv->l3_if_ref[vsi]++)
		return 0;

	mtu = l3dev->mtu + ETH_HLEN;

	/* Accept any of our addresses rather than tracking which MY_MAC entry
	 * belongs to which interface: the table only holds our own addresses.
	 */
	regmap_write(priv->regmap, PPE_IN_L3_IF_TBL(vsi),
		     PPE_L3_IF_IPV4_ROUTE_EN | PPE_L3_IF_IPV6_ROUTE_EN);
	regmap_write(priv->regmap, PPE_IN_L3_IF_TBL(vsi) + 4,
		     FIELD_PREP(PPE_L3_IF_TTL_EXCEED_CMD,
				PPE_L3_IF_TTL_EXCEED_TO_CPU) |
		     PPE_L3_IF_TTL_EXCEED_DEACCEL |
		     FIELD_PREP(PPE_L3_IF_MAC_BITMAP, GENMASK(7, 0)));
	ppe_l3_if_mtu_set(priv, vsi, mtu);
	regmap_write(priv->regmap, PPE_L3_VSI_TBL(vsi),
		     PPE_L3_VSI_IF_VALID | FIELD_PREP(PPE_L3_VSI_IF_INDEX, vsi));

	return 0;
}

static void ppe_flow_entry_destroy(struct qca_ppe_priv *priv,
				   struct ppe_flow_entry *entry);

static void ppe_flow_drop(struct qca_ppe_priv *priv,
			  struct ppe_flow_entry *entry)
{
	priv->flow_stale++;
	rhashtable_remove_fast(&priv->flow_table, &entry->node,
			       ppe_flow_ht_params);
	list_del(&entry->list);
	ppe_flow_entry_destroy(priv, entry);
	kfree(entry);
}

/* The ingress L3 interface holds its own MTU and is programmed with the first
 * flow that needs it, so a routing domain whose MTU changes while flows are
 * live keeps being forwarded to the old limit unless the change reaches it.
 */
static void ppe_flow_l3_mtu_set(struct qca_ppe_priv *priv, int port, int mtu)
{
	u32 vsi;
	int i;

	lockdep_assert_held(&priv->vlan_lock);

	if (priv->wan_ref[port])
		ppe_l3_if_mtu_set(priv, priv->wan_vsi[port],
				  mtu + VLAN_ETH_HLEN + PPPOE_SES_HLEN);

	vsi = ppe_port_l3_vsi(priv, port);
	if (priv->l3_if_ref[vsi])
		ppe_l3_if_mtu_set(priv, vsi, mtu + ETH_HLEN);

	for (i = 0; i < PPE_VSI_MAX; i++) {
		struct qca_ppe_vlan_entry *vlan = &priv->vlans[i];

		if (vlan->br_dev && vlan->ports & BIT(port) &&
		    priv->l3_if_ref[vlan->vsi])
			ppe_l3_if_mtu_set(priv, vlan->vsi, mtu + ETH_HLEN);
	}
}

/* A MAC the entries were built against is changing: it is the address the
 * ingress accepts and the source the egress writes, so neither half can match
 * any more. Drop them and let the flowtable rebuild against the new one.
 */
static void ppe_flow_drop_port(struct qca_ppe_priv *priv, int port)
{
	struct ppe_flow_entry *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list)
		if (entry->iport == port || entry->oport == port)
			ppe_flow_drop(priv, entry);
}

/* Each half of the size check follows the device that owns it: the routing
 * domain's MRU follows the device that routes for the port, and the egress size
 * a flow was built with follows the port itself.
 *
 * Neither can be taken from DSA's port_change_mtu. That runs before the bridge
 * recomputes its own MTU, so a bridged port reads the previous one there, and
 * `ip link set br-lan mtu N` never reaches it at all. By the NETDEV_CHANGEMTU
 * of the device the value belongs to, both have settled - whichever of the two
 * the user set, and however the bridge chose to answer it.
 *
 * The egress size is part of the key that picks the interface entry, and that
 * slot is shared by every flow leaving the same way at the old size, so a live
 * flow cannot be moved to the new one. Drop those and let the flowtable rebuild
 * them against the size it has now.
 */
static int ppe_flow_netdev_event(struct notifier_block *nb, unsigned long event,
				 void *ptr)
{
	struct qca_ppe_priv *priv = container_of(nb, struct qca_ppe_priv,
						 netdev_nb);
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct ppe_flow_entry *entry, *tmp;
	struct dsa_port *dp;
	int i;

	if (event != NETDEV_CHANGEMTU && event != NETDEV_CHANGEADDR)
		return NOTIFY_DONE;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	/* Reached through the netdev itself rather than by walking the switch:
	 * this notifier is live before the switch is registered, and every
	 * netdev in the system passes through it.
	 */
	dp = dsa_port_from_netdev(dev);
	if (!IS_ERR(dp) && dp->ds == &priv->ds) {
		if (event == NETDEV_CHANGEADDR) {
			ppe_flow_drop_port(priv, dp->index);
		} else {
			list_for_each_entry_safe(entry, tmp, &priv->flow_list,
						 list)
				if (entry->oport == dp->index)
					ppe_flow_drop(priv, entry);

			if (!priv->port_br_dev[dp->index])
				ppe_flow_l3_mtu_set(priv, dp->index, dev->mtu);
		}
	}

	for (i = 0; i < QCA_PPE_MAX_PORTS; i++) {
		if (priv->port_br_dev[i] != dev)
			continue;

		if (event == NETDEV_CHANGEADDR)
			ppe_flow_drop_port(priv, i);
		else
			ppe_flow_l3_mtu_set(priv, i, dev->mtu);
	}

	return NOTIFY_DONE;
}

static void ppe_flow_free_ingress(struct qca_ppe_priv *priv,
				  struct ppe_flow_entry *entry)
{
	lockdep_assert_held(&priv->vlan_lock);

	if (entry->wan_iport >= 0) {
		ppe_wan_ingress_put(priv, entry->wan_iport);
		return;
	}

	if (entry->l3_if >= 0 && !--priv->l3_if_ref[entry->l3_if]) {
		regmap_write(priv->regmap, PPE_L3_VSI_TBL(entry->l3_if), 0);
		ppe_tbl_clear(priv, PPE_IN_L3_IF_TBL(entry->l3_if),
			      PPE_L3_IF_WORDS);
	}

	if (ppe_res_put(priv->my_mac, entry->my_mac))
		ppe_tbl_clear(priv, PPE_MY_MAC_TBL(entry->my_mac),
			      PPE_MY_MAC_WORDS);
}

/* The routing domain a set of flows was built for is going away. Their entries
 * name its VSI as the ingress they match on, and that number is about to be
 * handed to another domain, so they cannot be left behind: they would match the
 * new domain's traffic, and their own teardown would later clear the new
 * owner's L3 interface out from under it. The flowtable reinstalls whatever is
 * still live, against whatever domain it belongs to then.
 */
void ppe_flow_purge_vsi(struct qca_ppe_priv *priv, u32 vsi)
{
	struct ppe_flow_entry *entry, *tmp;

	lockdep_assert_held(&priv->flow_lock);

	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		if (entry->src_if != vsi)
			continue;

		ppe_flow_drop(priv, entry);
	}
}

/* A flow that ingresses on this port and was installed before the uplink had a
 * classification named the port's plain VSI, and can never match now that one
 * exists. Drop those entries: the flowtable reinstalls whatever is still live.
 */
static void ppe_flow_purge_ingress(struct qca_ppe_priv *priv, int iport,
				   u8 wan_vsi)
{
	struct ppe_flow_entry *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		if (entry->iport != iport || entry->src_if == wan_vsi)
			continue;

		ppe_flow_drop(priv, entry);
	}
}

/* Build the egress L3 interface, nexthop and public-address entries this rule
 * needs, and program any that were not already in use by another flow.
 */
static int ppe_flow_alloc_egress(struct qca_ppe_priv *priv,
				 struct ppe_flow_data *data, bool snat,
				 bool dnat, int iport,
				 struct ppe_flow_entry *entry)
{
	u32 words[PPE_NEXTHOP_WORDS] = {};
	struct dsa_port *odp;
	u32 eg_mtu;
	u64 mac;
	int port, ret;

	port = ppe_flow_port_by_ifindex(priv, data->odev->ifindex);
	if (port < 0)
		return port;

	/* A frame sent back out the port it arrived on is discarded by source
	 * port filtering, so offloading it would black-hole what the CPU would
	 * otherwise have forwarded.
	 */
	if (port == iport)
		return -EBUSY;

	entry->oport = port;

	mac = ether_addr_to_u64(data->eth.h_source);
	ppe_entry_set(words, PPE_EG_L3_IF_MAC_OFF, PPE_EG_L3_IF_MAC_LEN, mac);
	if (data->pppoe_valid) {
		ppe_entry_set(words, PPE_EG_L3_IF_SESSION_OFF,
			      PPE_EG_L3_IF_SESSION_LEN, data->pppoe_sid);
		ppe_entry_set(words, PPE_EG_L3_IF_PPPOE_EN_OFF,
			      PPE_EG_L3_IF_PPPOE_EN_LEN, 1);
	}
	/* The size a routed frame leaves at, which the hardware compares before
	 * it egresses and sends the frame to the CPU when it does not fit. The
	 * port's mtu carries one mac header and the vlan tag the nexthop pushes;
	 * a pppoe session header rides inside that mtu rather than on top of it.
	 * It joins the key because two interfaces sharing a source address need
	 * separate entries when they do not share a size.
	 */
	odp = dsa_to_port(&priv->ds, port);
	eg_mtu = odp->user->mtu + ETH_HLEN + (data->vlan_valid ? VLAN_HLEN : 0);
	words[PPE_EG_L3_IF_WORDS] = eg_mtu;

	/* An L3 interface is one index with an ingress half and an egress half.
	 * ppe_flow_alloc_ingress() keys the ingress half by VSI, so an egress
	 * interface allocated below PPE_VSI_MAX would answer the size check out
	 * of some VSI's ingress mtu, or overwrite it. Allocating above that
	 * range is what keeps the two halves from sharing an entry.
	 */
	ret = ppe_res_get(priv->eg_l3_if + PPE_VSI_MAX,
			  PPE_EG_L3_IF_ENTRIES - PPE_VSI_MAX, words,
			  PPE_EG_L3_IF_WORDS + 1);
	if (ret < 0)
		return ret;
	entry->eg_l3_if = ret + PPE_VSI_MAX;
	if (priv->eg_l3_if[entry->eg_l3_if].refcount == 1) {
		ppe_tbl_write(priv, PPE_EG_L3_IF_TBL(entry->eg_l3_if), words,
			      PPE_EG_L3_IF_WORDS);
		ppe_eg_l3_if_mtu_set(priv, entry->eg_l3_if, eg_mtu);
	}

	if (snat) {
		u32 pub = ntohl(data->v4_src_new);

		ret = ppe_res_get(priv->pub_ip, PPE_PUB_IP_ENTRIES, &pub, 1);
		if (ret < 0)
			goto err_eg_l3_if;
		entry->pub_ip = ret;
		if (priv->pub_ip[ret].refcount == 1)
			regmap_write(priv->regmap, PPE_PUB_IP_TBL(ret), pub);
	}

	memset(words, 0, sizeof(words));
	/* Type selects how the port field is read: as a physical port (1) or,
	 * left at 0, as a VSI whose FDB resolves the port. The egress port is
	 * known exactly here, so name it directly; left at 0 the port number
	 * would be read as a VSI.
	 */
	ppe_entry_set(words, PPE_NEXTHOP_TYPE_OFF, PPE_NEXTHOP_TYPE_LEN,
		      PPE_NEXTHOP_TYPE_PORT);
	ppe_entry_set(words, PPE_NEXTHOP_PORT_OFF, PPE_NEXTHOP_PORT_LEN, port);
	ppe_entry_set(words, PPE_NEXTHOP_POST_L3_IF_OFF,
		      PPE_NEXTHOP_POST_L3_IF_LEN, entry->eg_l3_if);
	if (data->vlan_valid) {
		ppe_entry_set(words, PPE_NEXTHOP_CTAG_FMT_OFF,
			      PPE_NEXTHOP_CTAG_FMT_LEN, 1);
		ppe_entry_set(words, PPE_NEXTHOP_CVID_OFF,
			      PPE_NEXTHOP_CVID_LEN, data->vlan_id);
	}
	if (snat)
		ppe_entry_set(words, PPE_NEXTHOP_PUB_IP_IDX_OFF,
			      PPE_NEXTHOP_PUB_IP_IDX_LEN, entry->pub_ip);
	ppe_entry_set(words, PPE_NEXTHOP_MAC_OFF, PPE_NEXTHOP_MAC_LEN,
		      ether_addr_to_u64(data->eth.h_dest));
	if (dnat)
		ppe_entry_set(words, PPE_NEXTHOP_DNAT_IP_OFF,
			      PPE_NEXTHOP_DNAT_IP_LEN, ntohl(data->v4_dst_new));

	ret = ppe_res_get(priv->nexthop, priv->data->num_nexthop_entries, words,
			  PPE_NEXTHOP_WORDS);
	if (ret < 0)
		goto err_pub_ip;
	entry->nexthop = ret;
	if (priv->nexthop[ret].refcount == 1)
		ppe_tbl_write(priv, PPE_IN_NEXTHOP_TBL(ret), words,
			      PPE_NEXTHOP_WORDS);

	/* The reverse of a flow that egresses PPPoE arrives PPPoE-encapsulated
	 * on this same port; set the port up to route it so that direction
	 * offloads too.
	 */
	if (data->pppoe_valid) {
		struct dsa_port *odp = dsa_to_port(&priv->ds, port);
		u8 wan_vsi;

		ret = ppe_wan_ingress_get(priv, port, data->pppoe_sid,
					  data->vlan_valid, data->vlan_id,
					  odp->user->dev_addr,
					  odp->user->mtu + VLAN_ETH_HLEN +
					  PPPOE_SES_HLEN);
		wan_vsi = priv->wan_vsi[port];
		if (ret < 0)
			goto err_nexthop;
		entry->wan_port = port;

		if (ret == 1)
			ppe_flow_purge_ingress(priv, port, wan_vsi);
	}

	return 0;

err_nexthop:
	if (ppe_res_put(priv->nexthop, entry->nexthop))
		ppe_tbl_clear(priv, PPE_IN_NEXTHOP_TBL(entry->nexthop),
			      PPE_NEXTHOP_WORDS);
err_pub_ip:
	if (ppe_res_put(priv->pub_ip, entry->pub_ip))
		regmap_write(priv->regmap, PPE_PUB_IP_TBL(entry->pub_ip), 0);
err_eg_l3_if:
	if (ppe_res_put(priv->eg_l3_if, entry->eg_l3_if)) {
		ppe_tbl_clear(priv, PPE_EG_L3_IF_TBL(entry->eg_l3_if),
			      PPE_EG_L3_IF_WORDS);
		ppe_tbl_clear(priv, PPE_IN_L3_IF_TBL(entry->eg_l3_if),
			      PPE_L3_IF_WORDS);
	}

	return ret;
}

static void ppe_flow_free_egress(struct qca_ppe_priv *priv,
				 struct ppe_flow_entry *entry)
{
	if (ppe_res_put(priv->nexthop, entry->nexthop))
		ppe_tbl_clear(priv, PPE_IN_NEXTHOP_TBL(entry->nexthop),
			      PPE_NEXTHOP_WORDS);
	if (ppe_res_put(priv->pub_ip, entry->pub_ip))
		regmap_write(priv->regmap, PPE_PUB_IP_TBL(entry->pub_ip), 0);
	if (ppe_res_put(priv->eg_l3_if, entry->eg_l3_if)) {
		ppe_tbl_clear(priv, PPE_EG_L3_IF_TBL(entry->eg_l3_if),
			      PPE_EG_L3_IF_WORDS);
		ppe_tbl_clear(priv, PPE_IN_L3_IF_TBL(entry->eg_l3_if),
			      PPE_L3_IF_WORDS);
	}

	if (entry->wan_port >= 0)
		ppe_wan_ingress_put(priv, entry->wan_port);
}

/* Both locks: the release below reaches the VLAN side, and a routing domain
 * going away destroys flows with that lock already held, so taking it here
 * would be the same task asking for it twice.
 */
static void ppe_flow_entry_destroy(struct qca_ppe_priv *priv,
				   struct ppe_flow_entry *entry)
{
	lockdep_assert_held(&priv->flow_lock);
	lockdep_assert_held(&priv->vlan_lock);

	/* Delete by index - the key does not have to be restaged. Only a read
	 * that says the slot belongs to another flow is a reason not to: the
	 * hardware removes an idle entry on its own and may have handed the
	 * slot on, but a read that failed leaves it unknown, and leaving a live
	 * entry pointing at side tables about to be reused is the worse half of
	 * that trade.
	 */
	if (ppe_flow_entry_age(priv, entry) != -ENOENT)
		ppe_flow_entry_delete(priv, entry->index);
	ppe_host_ref_put(priv, entry->host_index);
	ppe_flow_free_egress(priv, entry);
	ppe_flow_free_ingress(priv, entry);
}

/* Encode the flow entry and its host half. The key is always the tuple as the
 * packet arrives: the destination address in the flow entry, the source address
 * in the host entry.
 */
static void ppe_flow_encode(struct ppe_flow_data *data, bool v6, bool snat,
			    bool dnat, u32 nexthop, u32 src_if, u32 *fw,
			    u32 *hw)
{
	u32 fwd;

	ppe_entry_set(fw, PPE_FLOW_E_VALID_OFF, PPE_FLOW_E_VALID_LEN, 1);

	/* The key is a bare 5-tuple otherwise, so the same addresses on two
	 * VLANs - or on two ports that route separately - would alias. The
	 * ingress L3 interface is the one ingress identifier the entry can
	 * hold; a packet that resolves another one misses to the CPU.
	 */
	ppe_entry_set(fw, PPE_FLOW_E_SRC_IF_VALID_OFF,
		      PPE_FLOW_E_SRC_IF_VALID_LEN, 1);
	ppe_entry_set(fw, PPE_FLOW_E_SRC_IF_OFF, PPE_FLOW_E_SRC_IF_LEN, src_if);
	ppe_entry_set(fw, PPE_FLOW_E_TYPE_OFF, PPE_FLOW_E_TYPE_LEN, v6);
	ppe_entry_set(fw, PPE_FLOW_E_PROTO_OFF, PPE_FLOW_E_PROTO_LEN,
		      ppe_flow_proto(data->l4proto));
	ppe_entry_set(fw, PPE_FLOW_E_AGE_OFF, PPE_FLOW_E_AGE_LEN,
		      PPE_FLOW_AGE_MAX);
	ppe_entry_set(fw, PPE_FLOW_E_PRI_PROFILE_OFF,
		      PPE_FLOW_E_PRI_PROFILE_LEN, data->priority);

	fwd = snat ? PPE_FLOW_FWD_SNAT : dnat ? PPE_FLOW_FWD_DNAT :
						PPE_FLOW_FWD_ROUTE;
	ppe_entry_set(fw, PPE_FLOW_E_FWD_TYPE_OFF, PPE_FLOW_E_FWD_TYPE_LEN, fwd);
	ppe_entry_set(fw, PPE_FLOW_E_NEXTHOP_OFF, PPE_FLOW_E_NEXTHOP_LEN,
		      nexthop);
	if (snat)
		ppe_entry_set(fw, PPE_FLOW_E_NEW_PORT_OFF,
			      PPE_FLOW_E_NEW_PORT_LEN, ntohs(data->sport_new));
	else if (dnat)
		ppe_entry_set(fw, PPE_FLOW_E_NEW_PORT_OFF,
			      PPE_FLOW_E_NEW_PORT_LEN, ntohs(data->dport_new));

	ppe_entry_set(fw, PPE_FLOW_E_SPORT_OFF, PPE_FLOW_E_SPORT_LEN,
		      ntohs(data->sport));
	ppe_entry_set(fw, PPE_FLOW_E_DPORT_OFF, PPE_FLOW_E_DPORT_LEN,
		      ntohs(data->dport));

	ppe_entry_set(hw, PPE_HOST_E_VALID_OFF, PPE_HOST_E_VALID_LEN, 1);

	if (v6) {
		ppe_entry_set_addr6(fw, PPE_FLOW_E_IPV6_OFF, &data->v6_dst);
		ppe_entry_set(hw, PPE_HOST_E_KEY_TYPE_OFF,
			      PPE_HOST_E_KEY_TYPE_LEN, PPE_HOST_KEY_IPV6);
		ppe_entry_set_addr6(hw, PPE_HOST_E_IPV6_OFF, &data->v6_src);
	} else {
		ppe_entry_set(fw, PPE_FLOW_E_IPV4_OFF, PPE_FLOW_E_IPV4_LEN,
			      ntohl(data->v4_dst));
		ppe_entry_set(hw, PPE_HOST_E_KEY_TYPE_OFF,
			      PPE_HOST_E_KEY_TYPE_LEN, PPE_HOST_KEY_IPV4);
		ppe_entry_set(hw, PPE_HOST_E_IPV4_OFF, PPE_HOST_E_IPV4_LEN,
			      ntohl(data->v4_src));
	}
}

static int ppe_flow_reject(struct qca_ppe_priv *priv, enum ppe_flow_reject why)
{
	priv->flow_reject[why]++;

	return -EOPNOTSUPP;
}

static int ppe_flow_offload_replace(struct ppe_flow_block *fb,
				    struct flow_cls_offload *f)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct qca_ppe_priv *priv = fb->priv;
	u32 fw[PPE_FLOW_ENTRY_WORDS_V6] = {};
	u32 hw[PPE_HOST_ENTRY_WORDS_V6] = {};
	struct ppe_flow_entry *entry;
	struct ppe_flow_data data = {};
	struct flow_action_entry *act;
	bool snat, dnat, v6;
	int i, ret, nfw, nhw, iport;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	/* A replace for a cookie already held is the kernel refreshing a flow
	 * whose packets it saw in the software path. If the hardware entry is
	 * intact the refresh is a no-op; if the hardware has lost it - however
	 * that happened - this is the moment to reinstall, or the flow would
	 * stay in software for the rest of its life.
	 */
	entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie,
				       ppe_flow_ht_params);
	if (entry) {
		if (ppe_flow_entry_age(priv, entry) >= 0)
			return 0;

		priv->flow_reinstalled++;
		rhashtable_remove_fast(&priv->flow_table, &entry->node,
				       ppe_flow_ht_params);
		list_del(&entry->list);
		ppe_flow_entry_destroy(priv, entry);
		kfree(entry);
	}

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_META) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC))
		return ppe_flow_reject(priv, PPE_REJECT_KEY);

	{
		struct flow_match_meta match;

		flow_rule_match_meta(rule, &match);
		/* Compare against our own ports rather than looking the index
		 * up, which keeps this correct without knowing the namespace
		 * the flowtable belongs to. A flow that did not arrive on a
		 * switch port cannot be matched anyway: Wi-Fi reaches the CPU
		 * port, and this silicon has no virtual ports to give it a
		 * flow-table identity.
		 */
		iport = ppe_flow_port_by_ifindex(priv,
						 match.key->ingress_ifindex);
		if (iport < 0)
			return ppe_flow_reject(priv, PPE_REJECT_INGRESS_PORT);
	}

	/* An ingress VLAN cannot be part of the hardware key. Where a bridge
	 * classifies the tag in hardware the kernel marks it as such and does
	 * not offer it as a match at all, so a tag that does arrive here is one
	 * this driver has no classification for and no L3 interface to stand in
	 * for it; the flow stays in software. A second tag has no expression
	 * either.
	 */
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN) ||
	    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CVLAN))
		return ppe_flow_reject(priv, PPE_REJECT_INGRESS_VLAN);

	{
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		if (flow_rule_has_control_flags(match.mask->flags,
						f->common.extack))
			return ppe_flow_reject(priv, PPE_REJECT_KEY);
		data.addr_type = match.key->addr_type;
	}

	{
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		data.l4proto = match.key->ip_proto;
	}

	if (ppe_flow_proto(data.l4proto) < 0)
		return ppe_flow_reject(priv, PPE_REJECT_PROTO);

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			if (act->mangle.htype == FLOW_ACT_MANGLE_HDR_TYPE_ETH)
				ret = ppe_flow_mangle_eth(act, &data.eth);
			else
				ret = 0;
			if (ret)
				return ppe_flow_reject(priv, PPE_REJECT_ACTION);
			break;
		case FLOW_ACTION_REDIRECT:
			data.odev = act->dev;
			break;
		case FLOW_ACTION_CSUM:
			break;
		case FLOW_ACTION_VLAN_PUSH:
			if (data.vlan_valid ||
			    act->vlan.proto != htons(ETH_P_8021Q))
				return ppe_flow_reject(priv, PPE_REJECT_ACTION);
			data.vlan_id = act->vlan.vid;
			data.vlan_valid = true;
			break;
		case FLOW_ACTION_VLAN_POP:
			/* Routed egress rebuilds the L2 header from the
			 * nexthop, which sheds the ingress encapsulation on
			 * its own.
			 */
			break;
		case FLOW_ACTION_PRIORITY:
			if (act->priority > PPE_QOS_MAX_PRI)
				return ppe_flow_reject(priv, PPE_REJECT_ACTION);
			data.priority = act->priority;
			break;
		case FLOW_ACTION_PPPOE_PUSH:
			if (data.pppoe_valid)
				return ppe_flow_reject(priv, PPE_REJECT_ACTION);
			data.pppoe_sid = act->pppoe.sid;
			data.pppoe_valid = true;
			break;
		default:
			return ppe_flow_reject(priv, PPE_REJECT_ACTION);
		}
	}

	if (!data.odev || !is_valid_ether_addr(data.eth.h_source) ||
	    !is_valid_ether_addr(data.eth.h_dest))
		return ppe_flow_reject(priv, PPE_REJECT_L2);

	switch (data.addr_type) {
	case FLOW_DISSECTOR_KEY_IPV4_ADDRS: {
		struct flow_match_ipv4_addrs match;

		flow_rule_match_ipv4_addrs(rule, &match);
		data.v4_src = match.key->src;
		data.v4_dst = match.key->dst;
		v6 = false;
		break;
	}
	case FLOW_DISSECTOR_KEY_IPV6_ADDRS: {
		struct flow_match_ipv6_addrs match;

		flow_rule_match_ipv6_addrs(rule, &match);
		data.v6_src = match.key->src;
		data.v6_dst = match.key->dst;
		v6 = true;
		break;
	}
	default:
		return ppe_flow_reject(priv, PPE_REJECT_KEY);
	}

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS))
		return ppe_flow_reject(priv, PPE_REJECT_KEY);
	{
		struct flow_match_ports match;

		flow_rule_match_ports(rule, &match);
		data.sport = match.key->src;
		data.dport = match.key->dst;
	}

	data.v4_src_new = data.v4_src;
	data.v4_dst_new = data.v4_dst;
	data.sport_new = data.sport;
	data.dport_new = data.dport;

	flow_action_for_each(i, act, &rule->action) {
		if (act->id != FLOW_ACTION_MANGLE)
			continue;

		switch (act->mangle.htype) {
		case FLOW_ACT_MANGLE_HDR_TYPE_IP4:
			ret = ppe_flow_mangle_ipv4(act, &data);
			break;
		case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
		case FLOW_ACT_MANGLE_HDR_TYPE_UDP:
			ret = ppe_flow_mangle_ports(act, &data);
			break;
		case FLOW_ACT_MANGLE_HDR_TYPE_IP6:
			/* The IPv6 flow entry has no L4 port rewrite fields and
			 * the nexthop's NAT address is 32 bits wide, so IPv6
			 * address translation cannot be expressed at all.
			 */
			return ppe_flow_reject(priv, PPE_REJECT_NAT_IPV6);
		default:
			ret = 0;
			break;
		}
		if (ret)
			return ppe_flow_reject(priv, PPE_REJECT_ACTION);
	}

	snat = data.v4_src_new != data.v4_src || data.sport_new != data.sport;
	dnat = data.v4_dst_new != data.v4_dst || data.dport_new != data.dport;

	/* SNAT and DNAT share the same bits of the flow entry, so a rule that
	 * needs both - a hairpinned connection - has no hardware expression.
	 */
	if (snat && dnat)
		return ppe_flow_reject(priv, PPE_REJECT_NAT_BOTH);

	if (v6 && (snat || dnat))
		return ppe_flow_reject(priv, PPE_REJECT_NAT_IPV6);

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->cookie = f->cookie;
	entry->block = fb->block;
	entry->nexthop = -1;
	entry->eg_l3_if = -1;
	entry->pub_ip = -1;
	entry->my_mac = -1;
	entry->l3_if = -1;
	entry->wan_port = -1;
	entry->wan_iport = -1;
	entry->iport = iport;

	ret = ppe_flow_alloc_ingress(priv, iport, entry);
	if (ret) {
		priv->flow_reject[ret == -EOPNOTSUPP ? PPE_REJECT_INGRESS_VLAN :
				  PPE_REJECT_RESOURCE]++;
		goto err_free;
	}

	ret = ppe_flow_alloc_egress(priv, &data, snat, dnat, iport, entry);
	if (ret) {
		priv->flow_reject[ret == -ENOSPC ? PPE_REJECT_RESOURCE :
				  ret == -EBUSY ? PPE_REJECT_HAIRPIN :
				  PPE_REJECT_EGRESS_PORT]++;
		goto err_ingress;
	}

	ppe_flow_encode(&data, v6, snat, dnat, entry->nexthop, entry->src_if,
			fw, hw);

	nfw = v6 ? PPE_FLOW_ENTRY_WORDS_V6 : PPE_FLOW_ENTRY_WORDS_V4;
	nhw = v6 ? PPE_HOST_ENTRY_WORDS_V6 : PPE_HOST_ENTRY_WORDS_V4;

	ret = ppe_flow_op(priv, PPE_TBL_OP_ADD, fw, nfw, hw, nhw, &entry->index,
			  &entry->host_index);
	if (ret) {
		priv->flow_reject[PPE_REJECT_HW_OP]++;
		goto err_egress;
	}

	ppe_host_ref_get(priv, entry->host_index);

	/* The hardware writes the host index it resolved into the entry and
	 * counts the age down from there, so the stored image carries the one
	 * and drops the other for readbacks to compare equal.
	 */
	memcpy(entry->words, fw, nfw * sizeof(*fw));
	entry->nwords = nfw;
	entry->words[0] = (fw[0] | entry->host_index << PPE_FLOW_E_HOST_IDX_OFF) &
			  ~PPE_FLOW_E_AGE_MASK;

	/* The counter survives the entry that filled it, so a recycled slot has
	 * to start from zero rather than from the previous flow's total.
	 */
	ppe_flow_counter_clear(priv, entry->index);
	entry->last_used = jiffies;

	ret = rhashtable_insert_fast(&priv->flow_table, &entry->node,
				     ppe_flow_ht_params);
	if (ret)
		goto err_hw;

	list_add_tail(&entry->list, &priv->flow_list);
	priv->flow_offloaded++;

	return 0;

err_hw:
	ppe_flow_entry_delete(priv, entry->index);
	ppe_host_ref_put(priv, entry->host_index);
err_egress:
	ppe_flow_free_egress(priv, entry);
err_ingress:
	ppe_flow_free_ingress(priv, entry);
err_free:
	kfree(entry);

	return ret;
}

static int ppe_flow_offload_destroy(struct qca_ppe_priv *priv,
				    struct flow_cls_offload *f)
{
	struct ppe_flow_entry *entry;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie,
				       ppe_flow_ht_params);
	if (!entry) {
		priv->flow_destroy_miss++;
		return -ENOENT;
	}

	rhashtable_remove_fast(&priv->flow_table, &entry->node,
			       ppe_flow_ht_params);
	list_del(&entry->list);
	ppe_flow_entry_destroy(priv, entry);
	kfree(entry);

	return 0;
}

static int ppe_flow_offload_stats(struct qca_ppe_priv *priv,
				  struct flow_cls_offload *f)
{
	struct ppe_flow_entry *entry;
	u64 packets, bytes, pkts;
	int age;

	guard(mutex)(&priv->flow_lock);

	entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie,
				       ppe_flow_ht_params);
	if (!entry)
		return -ENOENT;

	/* The hit counter does not advance for every forwarding type, but any
	 * hit rewinds the entry's age to its maximum while the hardware
	 * decrements it once per aging period - so an entry still at maximum
	 * age was used within the last period, which is all the flowtable's
	 * idle detection needs. The comparison excludes an entry whose slot
	 * the hardware has aged out and handed to another flow.
	 */
	age = ppe_flow_entry_age(priv, entry);
	if (age < 0) {
		flow_stats_update(&f->stats, 0, 0, 0, entry->last_used,
				  FLOW_ACTION_HW_STATS_DELAYED);
		return 0;
	}

	if (age == PPE_FLOW_AGE_MAX)
		entry->last_used = jiffies;

	ppe_flow_counter_read(priv, entry->index, &packets, &bytes);

	/* The counters are cumulative and narrower than u64 - 32-bit packets,
	 * 40-bit bytes - so the deltas are computed in the counters' own
	 * widths to survive wraparound.
	 */
	pkts = (u32)(packets - entry->packets);
	if (pkts) {
		entry->last_used = jiffies;
		flow_stats_update(&f->stats,
				  (bytes - entry->bytes) & PPE_FLOW_CNT_BYTES,
				  pkts, 0, entry->last_used,
				  FLOW_ACTION_HW_STATS_DELAYED);
		entry->packets = packets;
		entry->bytes = bytes;
	} else {
		flow_stats_update(&f->stats, 0, 0, 0, entry->last_used,
				  FLOW_ACTION_HW_STATS_DELAYED);
	}

	return 0;
}

static int ppe_flow_block_cb(enum tc_setup_type type, void *type_data,
			     void *cb_priv)
{
	struct flow_cls_offload *cls = type_data;
	struct ppe_flow_block *fb = cb_priv;
	struct qca_ppe_priv *priv = fb->priv;

	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;

	switch (cls->command) {
	case FLOW_CLS_REPLACE:
		return ppe_flow_offload_replace(fb, cls);
	case FLOW_CLS_DESTROY:
		return ppe_flow_offload_destroy(priv, cls);
	case FLOW_CLS_STATS:
		return ppe_flow_offload_stats(priv, cls);
	default:
		return -EOPNOTSUPP;
	}
}

static LIST_HEAD(ppe_block_cb_list);

static void ppe_flow_block_release(void *cb_priv)
{
	struct ppe_flow_block *fb = cb_priv;
	struct qca_ppe_priv *priv = fb->priv;
	struct ppe_flow_entry *entry, *tmp;

	mutex_lock(&priv->flow_lock);
	mutex_lock(&priv->vlan_lock);
	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		if (entry->block != fb->block)
			continue;

		rhashtable_remove_fast(&priv->flow_table, &entry->node,
				       ppe_flow_ht_params);
		list_del(&entry->list);
		ppe_flow_entry_destroy(priv, entry);
		kfree(entry);
	}
	mutex_unlock(&priv->vlan_lock);
	mutex_unlock(&priv->flow_lock);

	kfree(fb);
}

/* Every user port of the switch binds the same flowtable block, so it is shared
 * and reference counted rather than refused as busy.
 */
static int ppe_setup_ft_block(struct qca_ppe_priv *priv,
			      struct flow_block_offload *f)
{
	struct flow_block_cb *block_cb;
	struct ppe_flow_block *fb;

	if (f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	f->driver_block_list = &ppe_block_cb_list;

	switch (f->command) {
	case FLOW_BLOCK_BIND:
		block_cb = flow_block_cb_lookup(f->block, ppe_flow_block_cb,
						priv);
		if (block_cb) {
			flow_block_cb_incref(block_cb);
			return 0;
		}

		fb = kzalloc(sizeof(*fb), GFP_KERNEL);
		if (!fb)
			return -ENOMEM;
		fb->priv = priv;
		fb->block = f->block;

		block_cb = flow_block_cb_alloc(ppe_flow_block_cb, priv, fb,
					       ppe_flow_block_release);
		if (IS_ERR(block_cb)) {
			kfree(fb);
			return PTR_ERR(block_cb);
		}

		flow_block_cb_incref(block_cb);
		flow_block_cb_add(block_cb, f);
		list_add_tail(&block_cb->driver_list, &ppe_block_cb_list);
		return 0;
	case FLOW_BLOCK_UNBIND:
		block_cb = flow_block_cb_lookup(f->block, ppe_flow_block_cb,
						priv);
		if (!block_cb)
			return -ENOENT;

		if (!flow_block_cb_decref(block_cb)) {
			flow_block_cb_remove(block_cb, f);
			list_del(&block_cb->driver_list);
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

int qca_ppe_setup_tc(struct dsa_switch *ds, int port, enum tc_setup_type type,
		     void *type_data)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	switch (type) {
	case TC_SETUP_FT:
		return ppe_setup_ft_block(priv, type_data);
	case TC_SETUP_QDISC_TBF:
		return qca_ppe_setup_tc_tbf(priv, port, type_data);
	case TC_SETUP_QDISC_ETS:
		return qca_ppe_setup_tc_ets(priv, port, type_data);
	case TC_SETUP_QDISC_MQPRIO:
		return qca_ppe_setup_tc_mqprio(priv, port, type_data);
	case TC_QUERY_CAPS:
		return qca_ppe_tc_query_caps(type_data);
	default:
		return -EOPNOTSUPP;
	}
}

int ppe_flow_offload_init(struct qca_ppe_priv *priv)
{
	struct device *dev = priv->ds.dev;
	int i, ret;

	priv->eg_l3_if = devm_kcalloc(dev, PPE_EG_L3_IF_ENTRIES,
				      sizeof(*priv->eg_l3_if), GFP_KERNEL);
	priv->pub_ip = devm_kcalloc(dev, PPE_PUB_IP_ENTRIES,
				    sizeof(*priv->pub_ip), GFP_KERNEL);
	priv->nexthop = devm_kcalloc(dev, priv->data->num_nexthop_entries,
				     sizeof(*priv->nexthop), GFP_KERNEL);
	priv->host_ref = devm_kcalloc(dev, priv->data->num_host_entries,
				      sizeof(*priv->host_ref), GFP_KERNEL);
	priv->my_mac = devm_kcalloc(dev, PPE_MY_MAC_ENTRIES,
				    sizeof(*priv->my_mac), GFP_KERNEL);
	if (!priv->eg_l3_if || !priv->pub_ip || !priv->nexthop ||
	    !priv->host_ref || !priv->my_mac)
		return -ENOMEM;

	for (i = 0; i < QCA_PPE_MAX_PORTS; i++) {
		priv->wan_vsi[i] = -1;
		priv->wan_mymac[i] = -1;
		priv->wan_xlt[i] = -1;
	}

	INIT_LIST_HEAD(&priv->flow_list);

	ret = rhashtable_init(&priv->flow_table, &ppe_flow_ht_params);
	if (ret)
		return ret;

	priv->netdev_nb.notifier_call = ppe_flow_netdev_event;
	ret = register_netdevice_notifier(&priv->netdev_nb);
	if (ret)
		rhashtable_destroy(&priv->flow_table);

	return ret;
}

void ppe_flow_offload_exit(struct qca_ppe_priv *priv)
{
	struct ppe_flow_entry *entry, *tmp;

	unregister_netdevice_notifier(&priv->netdev_nb);

	mutex_lock(&priv->flow_lock);
	mutex_lock(&priv->vlan_lock);
	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		rhashtable_remove_fast(&priv->flow_table, &entry->node,
				       ppe_flow_ht_params);
		list_del(&entry->list);
		ppe_flow_entry_destroy(priv, entry);
		kfree(entry);
	}
	mutex_unlock(&priv->vlan_lock);
	mutex_unlock(&priv->flow_lock);

	rhashtable_destroy(&priv->flow_table);
}
