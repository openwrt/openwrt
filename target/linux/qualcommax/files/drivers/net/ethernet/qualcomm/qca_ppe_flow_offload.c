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
#include <linux/dsa/oob.h>
#include <linux/hash.h>
#include <linux/if_pppox.h>
#include <linux/if_vlan.h>
#include <linux/netdevice.h>
#include <linux/ppp_defs.h>
#include <linux/rhashtable.h>
#include <linux/unaligned.h>
#include <net/dsa.h>
#include <net/flow_offload.h>
#include <net/dsfield.h>
#include <linux/netfilter_netdev.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_flow_table.h>
#include <net/pkt_cls.h>
#include <net/tcp.h>

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
	u16 ivid;
	u16 pppoe_sid;
	bool pppoe_valid;

	struct net_device *odev;
	/* The profile the entry is given; filled only where the flowtable
	 * reports a priority, and zero means DSCP still decides.
	 */
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
	u32 hwords[PPE_HOST_ENTRY_WORDS_V6];
	u8 nhwords;
	u8 profile;
	u8 quiet;
	bool sparse;
	u8 src_if;
	u32 host_index;
	int nexthop;
	int my_mac;
	int l3_if;
	int eg_l3_if;
	int pub_ip;
	int uplink;
	int in_uplink;
	int session;
	int xdev;
	unsigned long ct;
	u16 vid;
	u8 xport;
	u8 dscp;
	u8 iport;
	u8 oport;
	u16 ivid;
	u16 ovid;
	u64 packets;
	u64 bytes;
	u64 unread_packets;
	u64 unread_bytes;
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
 * sourced from the same MAC, shares one slot; without that, the nexthops would
 * run out long before the flow entries do.
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

static void ppe_entry_set_addr6(u32 *words, u32 offset,
				const struct in6_addr *addr)
{
	int i;

	/* The hardware stores the address least significant word first. */
	for (i = 3; i >= 0; i--)
		ppe_entry_set(words, offset + (3 - i) * 32, 32,
			      ntohl(addr->s6_addr32[i]));
}

/* GRE is the one protocol the flowtable offers whose tuple has no ports: the
 * entry keys on the IP protocol instead, which the hardware selects by naming
 * no L4 protocol at all.
 */
static int ppe_flow_proto(u8 l4proto)
{
	switch (l4proto) {
	case IPPROTO_TCP:
		return PPE_FLOW_PROTO_TCP;
	case IPPROTO_UDP:
		return PPE_FLOW_PROTO_UDP;
	case IPPROTO_GRE:
		return PPE_FLOW_PROTO_OTHER;
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

static int ppe_uplink_find(struct qca_ppe_priv *priv, int port, u16 vid)
{
	int i;

	for (i = 0; i < PPE_PPPOE_SESSIONS; i++)
		if (priv->uplink[i].ref && priv->uplink[i].port == port &&
		    priv->uplink[i].vid == vid)
			return i;

	return -ENOENT;
}

/* Make an uplink route its ingress traffic in hardware, so the download
 * direction of an offloaded connection reaches the flow lookup on its inner
 * tuple.
 *
 * A dedicated VSI keeps the uplink out of any L2 domain: the ingress VLAN is
 * classified into it with the 802.1Q tag stripped, and the VSI carries a routing
 * L3 interface holding the router's MAC. A frame that misses the flow table is
 * still forwarded to the CPU, with the tag the hardware stripped re-added on
 * that path, so the software stack sees on-wire frames throughout. The MRU
 * leaves room for the tag and a PPPoE header whether or not either is there.
 */
static int ppe_uplink_get(struct qca_ppe_priv *priv, int port, u16 vid,
			  const u8 *mac, u32 mtu)
{
	u32 words[PPE_MY_MAC_WORDS] = {};
	struct ppe_uplink *up;
	int i, vsi, xlt = -1, ret;

	i = ppe_uplink_find(priv, port, vid);
	if (i >= 0) {
		priv->uplink[i].ref++;
		return i;
	}

	for (i = 0; i < PPE_PPPOE_SESSIONS && priv->uplink[i].ref; i++)
		;
	if (i == PPE_PPPOE_SESSIONS)
		return -ENOSPC;
	up = &priv->uplink[i];

	/* The translation rule shares one table with the bridge VLANs, so its
	 * index comes from the allocator they share.
	 */
	if (vid) {
		xlt = ppe_xlt_idx_alloc(priv);
		if (xlt < 0)
			return xlt;
	}

	vsi = ppe_vsi_alloc(priv);
	if (vsi < 0) {
		ret = vsi;
		goto err_xlt;
	}
	ppe_vsi_member_set(priv, vsi, BIT(port) | BIT(QCA_PPE_CPU_PORT));

	ppe_entry_set(words, PPE_MY_MAC_ADDR_OFF, PPE_MY_MAC_ADDR_LEN,
		      ether_addr_to_u64(mac));
	ppe_entry_set(words, PPE_MY_MAC_VALID_OFF, PPE_MY_MAC_VALID_LEN, 1);
	ret = ppe_res_get(priv->my_mac, PPE_MY_MAC_ENTRIES, words,
			  PPE_MY_MAC_WORDS);
	if (ret < 0)
		goto err_vsi;
	up->mymac = ret;
	if (priv->my_mac[ret].refcount == 1)
		ppe_tbl_write(priv, PPE_MY_MAC_TBL(ret), words, PPE_MY_MAC_WORDS);

	regmap_write(priv->regmap, PPE_IN_L3_IF_TBL(vsi),
		     PPE_L3_IF_IPV4_ROUTE_EN | PPE_L3_IF_IPV6_ROUTE_EN);
	regmap_write(priv->regmap, PPE_IN_L3_IF_TBL(vsi) + 4,
		     FIELD_PREP(PPE_L3_IF_TTL_EXCEED_CMD,
				PPE_L3_IF_TTL_EXCEED_TO_CPU) |
		     FIELD_PREP(PPE_L3_IF_MAC_BITMAP, GENMASK(7, 0)));
	ppe_l3_if_mtu_set(priv, vsi, mtu + VLAN_ETH_HLEN + PPPOE_SES_HLEN);
	regmap_write(priv->regmap, PPE_L3_VSI_TBL(vsi),
		     PPE_L3_VSI_IF_VALID | FIELD_PREP(PPE_L3_VSI_IF_INDEX, vsi));

	if (vid) {
		/* The L3 stage does not parse through a residual 802.1Q tag:
		 * a PPPoE session is only recognised, and the inner tuple
		 * only reaches the flow lookup, once the rule also strips the
		 * tag. The hardware re-adds it toward the CPU (below), so a
		 * frame that misses the flow table still reaches the software
		 * stack in its on-wire form. Action and re-tag are in place
		 * before the rule goes live.
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
			     FIELD_PREP(PPE_EG_XLT_CVID, vid));
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
			     FIELD_PREP(PPE_XLT_CKEY_VID, vid));
		regmap_write(priv->regmap, PPE_XLT_RULE_TBL(xlt) + 8, 0);
	}

	up->ref = 1;
	up->port = port;
	up->vid = vid;
	up->vsi = vsi;
	up->xlt = xlt;

	return i;

err_vsi:
	ppe_vsi_free(priv, vsi);
err_xlt:
	if (xlt >= 0)
		ppe_xlt_idx_free(priv, &xlt);
	return ret;
}

static void ppe_uplink_put(struct qca_ppe_priv *priv, int i)
{
	struct ppe_uplink *up = &priv->uplink[i];
	int xlt = up->xlt;

	if (--up->ref)
		return;

	if (xlt >= 0) {
		/* Ingress first: the egress rule is what puts the tag back on
		 * a frame the ingress rule stripped, so taking it down first
		 * would surface stripped frames on the port device. Within the
		 * ingress rule the allocator clears the three key words before
		 * the action's, since a key left live over a zeroed action
		 * blackholes every frame it matches.
		 */
		ppe_xlt_idx_free(priv, &up->xlt);
		regmap_write(priv->regmap, PPE_EG_XLT_RULE(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_RULE_W1(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_ACTION(xlt), 0);
		regmap_write(priv->regmap, PPE_EG_XLT_ACTION_W1(xlt), 0);
	}
	regmap_write(priv->regmap, PPE_L3_VSI_TBL(up->vsi), 0);
	ppe_tbl_clear(priv, PPE_IN_L3_IF_TBL(up->vsi), PPE_L3_IF_WORDS);
	if (ppe_res_put(priv->my_mac, up->mymac))
		ppe_tbl_clear(priv, PPE_MY_MAC_TBL(up->mymac),
			      PPE_MY_MAC_WORDS);
	ppe_vsi_free(priv, up->vsi);
}

/* Recognise a PPPoE session on its uplink, so the header is parsed through to
 * the inner IP. A re-dialled session has a new id and gets an entry of its own;
 * the old one goes with the last flow that used it. The uplink terminates PPPoE
 * from its first session on. Like the kernel's session lookup, the entry
 * matches only frames from the server's address.
 */
static int ppe_pppoe_session_get(struct qca_ppe_priv *priv, int up, u16 sid,
				 const u8 *smac)
{
	int port = priv->uplink[up].port;
	u32 vsi = priv->uplink[up].vsi;
	u32 w[PPE_L3_IF_WORDS];
	int i, free = -1;

	for (i = 0; i < PPE_PPPOE_SESSIONS; i++) {
		struct ppe_pppoe_session *s = &priv->pppoe[i];

		if (!s->ref) {
			if (free < 0)
				free = i;
			continue;
		}
		if (s->sid != sid || priv->uplink[s->uplink].port != port ||
		    !ether_addr_equal(s->smac, smac))
			continue;
		/* The same id on another VLAN of the port would match the
		 * same entry.
		 */
		if (s->uplink != up)
			return -ENOSPC;
		s->ref++;
		return i;
	}

	if (free < 0)
		return -ENOSPC;

	for (i = 0; i < PPE_L3_IF_WORDS; i++)
		regmap_read(priv->regmap, PPE_IN_L3_IF_TBL(vsi) + i * 4, &w[i]);
	w[1] |= PPE_L3_IF_PPPOE_EN;
	ppe_tbl_write(priv, PPE_IN_L3_IF_TBL(vsi), w, PPE_L3_IF_WORDS);

	priv->pppoe[free].ref = 1;
	priv->pppoe[free].sid = sid;
	priv->pppoe[free].uplink = up;
	ether_addr_copy(priv->pppoe[free].smac, smac);
	regmap_write(priv->regmap, PPE_PPPOE_SESSION(free),
		     FIELD_PREP(PPE_PPPOE_SESSION_ID, sid) |
		     FIELD_PREP(PPE_PPPOE_SESSION_PORT_BMP, BIT(port)) |
		     FIELD_PREP(PPE_PPPOE_SESSION_L3_IF, vsi));
	regmap_write(priv->regmap, PPE_PPPOE_SESSION_EXT1(free),
		     get_unaligned_be32(smac));
	regmap_write(priv->regmap, PPE_PPPOE_SESSION_EXT(free),
		     PPE_PPPOE_EXT_L3_IF_VALID | PPE_PPPOE_EXT_UC_VALID |
		     PPE_PPPOE_EXT_SMAC_VALID |
		     FIELD_PREP(PPE_PPPOE_EXT_SMAC_LO,
				get_unaligned_be16(smac + 4)));

	return free;
}

static void ppe_pppoe_session_put(struct qca_ppe_priv *priv, int i)
{
	if (--priv->pppoe[i].ref)
		return;

	regmap_write(priv->regmap, PPE_PPPOE_SESSION(i), 0);
	regmap_write(priv->regmap, PPE_PPPOE_SESSION_EXT(i), 0);
	regmap_write(priv->regmap, PPE_PPPOE_SESSION_EXT1(i), 0);
}

/* A port's PVID only classifies while the bridge enforces its VLANs; until
 * then the bridge VLANs are recorded and name no port.
 */
static u16 ppe_port_pvid(struct qca_ppe_priv *priv, int port)
{
	return priv->vlan_filtering & BIT(port) ? priv->port_pvid[port] : 0;
}

/* The VSI the ingress classification puts this rule's packets in - the routing
 * domain the flow belongs to, and the one ingress identifier its hardware entry
 * can carry. A domain that cannot be named is declined rather than encoded as
 * the bare tuple, which would let the flow forward traffic from another VLAN.
 */
static int ppe_flow_ingress_vsi(struct qca_ppe_priv *priv, int iport, u16 vid)
{
	struct qca_ppe_vlan_entry *vlan;

	lockdep_assert_held(&priv->vlan_lock);

	/* A VLAN-filtering bridge classifies a tagged frame into its VLAN's
	 * VSI and an untagged one into the PVID's, so the port's own VSI
	 * answers only without either.
	 */
	if (!vid)
		vid = ppe_port_pvid(priv, iport);
	if (!vid)
		return priv->port_vsi[iport];
	if (!priv->port_br_dev[iport])
		return -EOPNOTSUPP;

	vlan = ppe_vlan_find(priv, priv->port_br_dev[iport], vid);

	return vlan && (vlan->ports & priv->vlan_filtering & BIT(iport)) ?
	       (int)vlan->vsi : -EOPNOTSUPP;
}

/* The VSI a flow from this port and tag is keyed on: the uplink's, or the one
 * the port classifies into.
 */
static int ppe_flow_src_if(struct qca_ppe_priv *priv, int iport, u16 vid)
{
	int up = ppe_uplink_find(priv, iport, vid);

	return up >= 0 ? priv->uplink[up].vsi :
			 ppe_flow_ingress_vsi(priv, iport, vid);
}

/* A VLAN device on a port that no bridge VLAN classifies, such as lan1.20 on a
 * port of its own or on a port of a bridge that does not filter VLANs, routes
 * on an uplink of its own, as the WAN's VLAN does. Its frames are addressed to
 * the device, or to the bridge it is a port of.
 */
static int ppe_flow_vlan_uplink_get(struct qca_ppe_priv *priv, int iport,
				    u16 vid)
{
	struct dsa_port *dp = dsa_to_port(&priv->ds, iport);
	struct net_device *vdev, *br;
	u8 mac[ETH_ALEN];

	rcu_read_lock();
	vdev = __vlan_find_dev_deep_rcu(dp->user, htons(ETH_P_8021Q), vid);
	if (!vdev) {
		rcu_read_unlock();
		return -EOPNOTSUPP;
	}
	br = netif_is_bridge_port(vdev) ?
	     netdev_master_upper_dev_get_rcu(vdev) : NULL;
	ether_addr_copy(mac, (br ?: vdev)->dev_addr);
	rcu_read_unlock();

	return ppe_uplink_get(priv, iport, vid, mac, dp->user->mtu);
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
				  u16 vid, struct ppe_flow_entry *entry)
{
	struct dsa_port *dp = dsa_to_port(&priv->ds, iport);
	u32 words[PPE_MY_MAC_WORDS] = {};
	struct net_device *l3dev;
	u32 mtu;
	int vsi, ret;

	lockdep_assert_held(&priv->vlan_lock);

	/* An uplink's ingress interface belongs to the uplink, so take a
	 * reference rather than programming anything: a sibling flow going away
	 * must not pull the classification out from under this one. A tag no
	 * bridge VLAN classifies gets an uplink here, before any flow egresses
	 * it.
	 */
	ret = ppe_uplink_find(priv, iport, vid);
	if (ret >= 0) {
		priv->uplink[ret].ref++;
	} else if (vid && ppe_flow_ingress_vsi(priv, iport, vid) < 0) {
		ret = ppe_flow_vlan_uplink_get(priv, iport, vid);
		if (ret < 0)
			return ret;
	}
	if (ret >= 0) {
		entry->in_uplink = ret;
		entry->src_if = priv->uplink[ret].vsi;
		entry->ivid = vid;
		return 0;
	}

	vsi = ppe_flow_ingress_vsi(priv, iport, vid);
	if (vsi < 0)
		return vsi;

	entry->src_if = vsi;
	entry->ivid = vid ? vid : ppe_port_pvid(priv, iport);

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
		     FIELD_PREP(PPE_L3_IF_MAC_BITMAP, GENMASK(7, 0)));
	ppe_l3_if_mtu_set(priv, vsi, mtu);
	regmap_write(priv->regmap, PPE_L3_VSI_TBL(vsi),
		     PPE_L3_VSI_IF_VALID | FIELD_PREP(PPE_L3_VSI_IF_INDEX, vsi));

	return 0;
}

static void ppe_flow_entry_destroy(struct qca_ppe_priv *priv,
				   struct ppe_flow_entry *entry);

static void ppe_flow_free(struct qca_ppe_priv *priv,
			  struct ppe_flow_entry *entry)
{
	rhashtable_remove_fast(&priv->flow_table, &entry->node,
			       ppe_flow_ht_params);
	list_del(&entry->list);
	ppe_flow_entry_destroy(priv, entry);
	kfree(entry);
}

static void ppe_flow_drop(struct qca_ppe_priv *priv,
			  struct ppe_flow_entry *entry)
{
	priv->flow_stale++;
	ppe_flow_free(priv, entry);
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

	for (i = 0; i < PPE_PPPOE_SESSIONS; i++)
		if (priv->uplink[i].ref && priv->uplink[i].port == port)
			ppe_l3_if_mtu_set(priv, priv->uplink[i].vsi,
					  mtu + VLAN_ETH_HLEN + PPPOE_SES_HLEN);

	vsi = priv->port_vsi[port];
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

/* Only the flows of one direction change path when its shaper does: those
 * already handed to a shaper of it, and those it would now take, by the rules
 * of ppe_flow_shape().
 */
static void ppe_flow_shaper_drop(struct qca_ppe_priv *priv, bool upload)
{
	struct ppe_flow_entry *entry, *tmp;

	lockdep_assert_held(&priv->flow_lock);

	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		bool hit;

		if (upload)
			hit = entry->xdev & PPE_XDEV_UL ||
			      (entry->oport != QCA_PPE_CPU_PORT &&
			       !priv->port_br_dev[entry->oport]);
		else
			hit = entry->xdev & PPE_XDEV_DL ||
			      entry->in_uplink >= 0 ||
			      !priv->port_br_dev[entry->iport];
		if (hit)
			ppe_flow_drop(priv, entry);
	}
}

/* Find where the uplink device - or, unnamed, the upload shaper's - meets the
 * switch, the way the flowtable resolves a device: down through PPPoE and
 * VLAN to a port of this switch. A path that does not reach one leaves
 * ul_port at -1. Returns whether it changed.
 */
static bool ppe_flow_shaper_path(struct qca_ppe_priv *priv)
{
	int port = -1, vid = 0, sid = 0, i;
	u64 h_dest = 0, h_source = 0;
	struct net_device_path_ctx ctx = {};
	struct net_device_path_stack stack;
	struct net_device *dev;
	bool changed;

	lockdep_assert_held(&priv->flow_lock);

	rcu_read_lock();
	i = priv->uplink_dev ?: priv->ul_shaper;
	dev = i ? dev_get_by_index_rcu(&init_net, i) : NULL;
	ctx.dev = dev;
	if (dev && !dev_fill_forward_path(&ctx, &stack)) {
		for (i = 0; i < stack.num_paths; i++) {
			const struct net_device_path *path = &stack.path[i];
			struct dsa_port *dp;

			switch (path->type) {
			case DEV_PATH_PPPOE:
				sid = path->encap.id;
				h_dest = ether_addr_to_u64(path->encap.h_dest);
				break;
			case DEV_PATH_VLAN:
				if (!vid) {
					vid = path->encap.id;
					h_source = ether_addr_to_u64(path->dev->dev_addr);
				}
				break;
			case DEV_PATH_DSA:
				dp = dsa_port_from_netdev((struct net_device *)path->dev);
				if (!IS_ERR(dp) && dp->ds == &priv->ds)
					port = path->dsa.port;
				if (!h_source)
					h_source = ether_addr_to_u64(path->dev->dev_addr);
				break;
			default:
				break;
			}
		}
	}
	rcu_read_unlock();

	if (port < 0)
		vid = sid = 0;
	changed = port != priv->ul_port || vid != priv->ul_vid ||
		  sid != priv->ul_sid;
	WRITE_ONCE(priv->ul_port, port);
	WRITE_ONCE(priv->ul_vid, vid);
	WRITE_ONCE(priv->ul_sid, sid);
	WRITE_ONCE(priv->ul_h_dest, h_dest);
	WRITE_ONCE(priv->ul_h_source, h_source);

	return changed;
}

/* A shaper is named, and the device of that name can go and come back with
 * another ifindex: a PPPoE session that reconnects brings up a new ppp
 * device. While it is gone that direction is not shaped rather than sent
 * to an ifindex that no longer exists, and a device that takes the name
 * becomes the shaper again.
 */
static void ppe_flow_shaper_track(struct qca_ppe_priv *priv,
				  struct net_device *dev, unsigned long event)
{
	int i;

	lockdep_assert_held(&priv->flow_lock);

	if (!net_eq(dev_net(dev), &init_net))
		return;

	for (i = 0; i < 3; i++) {
		bool upload = i == 1, uplink = i == 2;
		int *shaper = uplink ? &priv->uplink_dev :
			      upload ? &priv->ul_shaper : &priv->dl_shaper;
		const char *name = uplink ? priv->uplink_name :
				   upload ? priv->ul_shaper_name :
				   priv->dl_shaper_name;
		int now = *shaper;

		if (!*name)
			continue;

		if (now == dev->ifindex &&
		    (event == NETDEV_UNREGISTER || strcmp(dev->name, name)))
			now = 0;
		else if (event != NETDEV_UNREGISTER && !strcmp(dev->name, name) &&
			 dev->ifindex <= PPE_XDEV_IFINDEX)
			now = dev->ifindex;

		if (now == *shaper) {
			/* A PPPoE device is up before its session is. */
			if ((upload || uplink) && now && now == dev->ifindex &&
			    event == NETDEV_UP && ppe_flow_shaper_path(priv)) {
				ppe_flow_shaper_drop(priv, true);
				ppe_flow_shaper_drop(priv, false);
			}
			continue;
		}

		WRITE_ONCE(*shaper, now);
		if (upload || uplink) {
			ppe_flow_shaper_path(priv);
			ppe_flow_shaper_drop(priv, false);
		}
		ppe_flow_shaper_drop(priv, upload || uplink);
	}
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

	if (event == NETDEV_REGISTER || event == NETDEV_UNREGISTER ||
	    event == NETDEV_CHANGENAME || event == NETDEV_UP) {
		guard(mutex)(&priv->flow_lock);
		guard(mutex)(&priv->vlan_lock);
		ppe_flow_shaper_track(priv, dev, event);
		return NOTIFY_DONE;
	}

	if (event != NETDEV_CHANGEMTU && event != NETDEV_CHANGEADDR)
		return NOTIFY_DONE;

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	/* Reached through the netdev itself rather than by walking the switch:
	 * this notifier is live before the switch is registered, and every
	 * netdev in the system passes through it. An uplink's MY_MAC is the
	 * address of the VLAN device on the port, so that one counts too.
	 */
	if (event == NETDEV_CHANGEADDR && is_vlan_dev(dev))
		dp = dsa_port_from_netdev(vlan_dev_real_dev(dev));
	else
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

	if (entry->in_uplink >= 0) {
		ppe_uplink_put(priv, entry->in_uplink);
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

/* A flow that ingresses on the uplink's port and VLAN and was installed before
 * the uplink had a classification named the port's plain VSI, and can never
 * match now that one exists. Drop those entries: the flowtable reinstalls
 * whatever is still live.
 */
static void ppe_flow_purge_ingress(struct qca_ppe_priv *priv,
				   const struct ppe_uplink *up)
{
	struct ppe_flow_entry *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		if (entry->iport != up->port || entry->ivid != up->vid ||
		    entry->src_if == up->vsi)
			continue;

		ppe_flow_drop(priv, entry);
	}
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

	if (entry->session >= 0)
		ppe_pppoe_session_put(priv, entry->session);
	if (entry->uplink >= 0)
		ppe_uplink_put(priv, entry->uplink);
}

static bool ppe_cpu_xmit = true;
module_param_named(cpu_xmit, ppe_cpu_xmit, bool, 0644);
MODULE_PARM_DESC(cpu_xmit,
		 "Route flows to devices outside the switch in hardware and pass them out through the CPU port");

/* A flow out of a device the switch cannot reach, such as a Wi-Fi interface,
 * is still routed by the PPE. Its frames come to the CPU finished, and the
 * driver only queues them on the device. The device has to take Ethernet
 * frames as the nexthop builds them, with no tag or session to push.
 */
static bool ppe_cpu_xmit_ok(const struct ppe_flow_data *data)
{
	return ppe_cpu_xmit && data->odev->type == ARPHRD_ETHER &&
	       !data->vlan_valid && !data->pppoe_valid;
}

/* Whether a flow's side of the switch is the shaped uplink: the port, VLAN
 * and, upstream, the PPPoE session the upload shaper's device sits on. A
 * second routed port, a DMZ or another VLAN on the WAN port is not, and its
 * flows stay in hardware. Until the path is known, any routed port is.
 */
static bool ppe_flow_on_uplink(struct qca_ppe_priv *priv, int port, u16 vid,
			       u16 sid, bool upload)
{
	if (priv->ul_port < 0)
		return !priv->port_br_dev[port];

	return port == priv->ul_port && vid == priv->ul_vid &&
	       (!upload || sid == priv->ul_sid);
}

/* The PPE has no queue that can run CAKE or an L4S AQM: it drops on a
 * buffer count and cannot mark. Where the shaping is done by a qdisc
 * instead, the flows that cross the shaped link still have their lookup,
 * NAT and nexthop done in hardware, and only their frames come to the CPU,
 * finished, to pass the qdisc on the way out: a download flow through the
 * ifb that carries the download qdisc, an upload flow through the uplink
 * device, whose own qdisc, PPPoE and VLAN stack then do the rest. The
 * uplink is where the upload shaper's device meets the switch; a download
 * enters by it. Returns the port the frames leave the switch by.
 */
static int ppe_flow_shape(struct qca_ppe_priv *priv, struct ppe_flow_data *data,
			  struct ppe_flow_entry *entry, int port)
{
	int ul = READ_ONCE(priv->ul_shaper), dl = READ_ONCE(priv->dl_shaper);

	if (ul && port != QCA_PPE_CPU_PORT &&
	    ppe_flow_on_uplink(priv, port,
			       data->vlan_valid ? data->vlan_id : 0,
			       data->pppoe_valid ? data->pppoe_sid : 0, true)) {
		entry->xdev = ul | PPE_XDEV_UL;
		entry->xport = port + 1;
		return QCA_PPE_CPU_PORT;
	}

	/* A download to a tagged LAN VLAN comes to the CPU untagged: the tag
	 * is added after the shaper, as the VLAN stack would.
	 */
	if (dl && !data->pppoe_valid &&
	    ppe_flow_on_uplink(priv, entry->iport, data->ivid, 0, false)) {
		if (port != QCA_PPE_CPU_PORT) {
			entry->xdev = data->odev->ifindex;
			entry->xport = port + 1;
		}
		entry->xdev |= PPE_XDEV_DL;
		entry->vid = data->vlan_valid ? data->vlan_id : 0;
		return QCA_PPE_CPU_PORT;
	}

	return port;
}

/* An upload frame arrives NATed, without the conntrack the stack would have
 * attached, so the qdisc could not tell the LAN host behind it: CAKE's nat
 * mode finds the connection by the frame's own tuple, which after NAT is
 * neither of the connection's. The conntrack goes with the frame as it would
 * on the forward path. The entry holds a reference until it is destroyed; a
 * conntrack is freed by RCU and reused only as one, so taking a reference
 * that is not zero is safe here.
 */
static void ppe_flow_ct_attach(struct qca_ppe_priv *priv, struct sk_buff *skb,
			       u32 flow)
{
	unsigned long v = READ_ONCE(priv->flow_ct[flow]);
	struct nf_conn *ct = (struct nf_conn *)(v & ~1UL);

	if (ct && refcount_inc_not_zero(&ct->ct_general.use))
		nf_ct_set(skb, ct, v & 1 ? IP_CT_ESTABLISHED_REPLY :
				   IP_CT_ESTABLISHED);
}

/* The flowtable's cookie is the tuple of the offloaded direction. */
static unsigned long ppe_flow_ct_get(unsigned long cookie)
{
	const struct flow_offload_tuple *tuple = (void *)cookie;
	struct flow_offload_tuple_rhash *th;
	struct flow_offload *flow;

	th = container_of(tuple, struct flow_offload_tuple_rhash, tuple);
	flow = container_of(th, struct flow_offload, tuplehash[tuple->dir]);
	nf_conntrack_get(&flow->ct->ct_general);

	return (unsigned long)flow->ct | (tuple->dir == FLOW_OFFLOAD_DIR_REPLY);
}

/* The DSCP a firewall rule stamped into the conntrack mark on the flow's
 * first packets, as act_ctinfo reads it: bit 7 says it is set, bits 0-5 are
 * the value. An offloaded flow's later packets pass no such rule, so its
 * frames are given it when they are handed over.
 */
static u8 ppe_flow_ct_dscp(unsigned long cookie)
{
#if IS_ENABLED(CONFIG_NF_CONNTRACK_MARK)
	const struct flow_offload_tuple *tuple = (void *)cookie;
	struct flow_offload_tuple_rhash *th;
	struct flow_offload *flow;
	u32 mark;

	th = container_of(tuple, struct flow_offload_tuple_rhash, tuple);
	flow = container_of(th, struct flow_offload, tuplehash[tuple->dir]);
	mark = READ_ONCE(flow->ct->mark);
	if (mark & 0x80)
		return 0x80 | (mark & 0x3f);
#endif
	return 0;
}

/* Only the DSCP bits: the ECN field stays as the sender and the path set it. */
static void ppe_flow_dscp_apply(struct sk_buff *skb, u8 dscp)
{
	if (skb->protocol == htons(ETH_P_IP))
		ipv4_change_dsfield(ip_hdr(skb), INET_ECN_MASK, dscp << 2);
	else if (skb->protocol == htons(ETH_P_IPV6))
		ipv6_change_dsfield(ipv6_hdr(skb), INET_ECN_MASK, dscp << 2);
}

static void ppe_flow_slot_set(struct qca_ppe_priv *priv,
			      struct ppe_flow_entry *entry, u32 index)
{
	WRITE_ONCE(priv->flow_dscp[index], entry->dscp);
	WRITE_ONCE(priv->flow_vid[index], entry->vid);
	WRITE_ONCE(priv->flow_port[index], entry->xport);
	WRITE_ONCE(priv->flow_ct[index], entry->ct);
	WRITE_ONCE(priv->flow_xdev[index], entry->xdev);
}

static void ppe_flow_slot_clear(struct qca_ppe_priv *priv, u32 index)
{
	WRITE_ONCE(priv->flow_xdev[index], 0);
	WRITE_ONCE(priv->flow_ct[index], 0);
	WRITE_ONCE(priv->flow_dscp[index], 0);
	WRITE_ONCE(priv->flow_vid[index], 0);
	WRITE_ONCE(priv->flow_port[index], 0);
}

/* The shaper device. It stands where an ifb would, in front of the devices
 * a direction's flows leave by, and carries that direction's qdisc. What the
 * host forwards or sends itself is redirected to it, as to an ifb, and it
 * hands that on to the device the redirect came from, or back to the receive
 * path. A frame the PPE built for a flow is handed to it by ppe_flow_xmit()
 * instead: one for a switch port goes straight into the conduit's ring once
 * the qdisc lets it go, with no other device or qdisc on its way; one for a
 * device outside the switch, such as Wi-Fi, to that device.
 */
static bool ppe_shaper_direct = true;
module_param_named(shaper_direct, ppe_shaper_direct, bool, 0644);
MODULE_PARM_DESC(shaper_direct,
		 "Send frames the PPE built for a switch port from the shaper device straight to the conduit");

static bool ppe_shaper_push = true;
module_param_named(shaper_push, ppe_shaper_push, bool, 0644);
MODULE_PARM_DESC(shaper_push,
		 "Have the PPE build an upload frame's PPPoE and VLAN headers when it is handed to the shaper device (flows rebuilt on the next shaper change)");

struct ppe_shaper {
	struct qca_ppe_priv *priv;
	struct net_device *conduit;
};

static const struct net_device_ops ppe_shaper_ops;

static bool ppe_shaper_is(const struct net_device *dev)
{
	return dev->netdev_ops == &ppe_shaper_ops;
}

/* Whether upload flows are built whole by the PPE: their shaper is this
 * driver's device, which sends them to the conduit.
 */
static bool ppe_flow_ul_push(struct qca_ppe_priv *priv)
{
	struct net_device *sh = priv->shaper_dev[1];

	lockdep_assert_held(&priv->flow_lock);

	return ppe_shaper_push && sh && priv->ul_shaper == sh->ifindex;
}

/* Into the conduit's ring as edma_ndo_xmit() would be called from its qdisc,
 * on the queue of the ring the frame arrived on. A full ring leaves the frame
 * in the shaper's qdisc, which offers it again.
 */
static netdev_tx_t ppe_shaper_conduit_xmit(struct sk_buff *skb,
					   struct net_device *dev,
					   struct net_device *conduit)
{
	struct ppe_shaper *sh = netdev_priv(dev);
	struct netdev_queue *txq;
	unsigned int len = skb->len;
	netdev_tx_t rc;
	u16 qi;

	qi = skb_get_queue_mapping(skb);
	if (unlikely(qi >= conduit->real_num_tx_queues))
		qi %= conduit->real_num_tx_queues;
	txq = netdev_get_tx_queue(conduit, qi);

	skb->dev = conduit;
	__netif_tx_lock(txq, smp_processor_id());
	if (unlikely(netif_xmit_frozen_or_stopped(txq)))
		rc = NETDEV_TX_BUSY;
	else
		rc = netdev_start_xmit(skb, conduit, txq, false);
	__netif_tx_unlock(txq);

	if (unlikely(rc == NETDEV_TX_BUSY)) {
		skb->dev = dev;
		atomic_inc(&sh->priv->shaper_busy);
		return NETDEV_TX_BUSY;
	}
	dev_sw_netstats_tx_add(dev, 1, len);

	return NETDEV_TX_OK;
}

/* For the conduit: the switch port the frame leaves by, as the tagger
 * would set it.
 */
static bool ppe_flow_tag(struct sk_buff *skb, u8 xport)
{
	struct dsa_oob_tag_info *ti = skb_ext_add(skb, SKB_EXT_DSA_OOB);

	if (unlikely(!ti))
		return false;
	ti->port = xport - 1;

	return true;
}

static bool ppe_shaper_ul_build = true;
module_param_named(shaper_ul_build, ppe_shaper_ul_build, bool, 0644);
MODULE_PARM_DESC(shaper_ul_build,
		 "Have the upload shaper device build the PPPoE and VLAN headers of what the host routes to the uplink and send it to the conduit");

/* What the host routed to a PPPoE uplink, redirected here from its egress:
 * built the way the switch builds an offloaded upload frame, with the
 * session, the VLAN tag and the port the uplink's own path resolves to, so
 * that it too goes straight to the conduit rather than through the PPP,
 * PPPoE, VLAN and switch port devices. Anything else goes the ifb way.
 */
static struct net_device *ppe_shaper_ul_frame(struct qca_ppe_priv *priv,
					      struct sk_buff *skb,
					      struct net_device *to)
{
	int port = READ_ONCE(priv->ul_port);
	u16 sid = READ_ONCE(priv->ul_sid), vid = READ_ONCE(priv->ul_vid);
	struct net_device *conduit;
	struct pppoe_hdr *ph;
	struct ethhdr *eth;
	__be16 proto;
	int len;

	if (!ppe_shaper_ul_build || to->ifindex != READ_ONCE(priv->uplink_dev) ||
	    to->type != ARPHRD_PPP || port < 0 || !sid || skb_is_gso(skb) ||
	    skb->from_ingress || skb_network_offset(skb))
		return NULL;

	if (skb->protocol == htons(ETH_P_IP))
		proto = htons(PPP_IP);
	else if (skb->protocol == htons(ETH_P_IPV6))
		proto = htons(PPP_IPV6);
	else
		return NULL;

	conduit = ((struct ppe_shaper *)netdev_priv(priv->shaper_dev[1]))->conduit;
	if (skb->ip_summed == CHECKSUM_PARTIAL && skb_checksum_help(skb))
		return NULL;
	if (skb_cow_head(skb, ETH_HLEN + PPPOE_SES_HLEN +
			 conduit->needed_headroom))
		return NULL;

	len = skb->len;
	*(__be16 *)__skb_push(skb, sizeof(proto)) = proto;
	ph = __skb_push(skb, sizeof(*ph));
	ph->ver = 1;
	ph->type = 1;
	ph->code = 0;
	ph->sid = htons(sid);
	ph->length = htons(len + sizeof(proto));
	eth = __skb_push(skb, ETH_HLEN);
	u64_to_ether_addr(READ_ONCE(priv->ul_h_dest), eth->h_dest);
	u64_to_ether_addr(READ_ONCE(priv->ul_h_source), eth->h_source);
	eth->h_proto = htons(ETH_P_PPP_SES);
	skb_reset_mac_header(skb);
	skb->protocol = htons(ETH_P_PPP_SES);
	if (vid)
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vid);
	if (!ppe_flow_tag(skb, port + 1))
		return NULL;

	return conduit;
}

static netdev_tx_t ppe_shaper_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ppe_shaper *sh = netdev_priv(dev);
	struct net_device *to, *conduit;

	to = dev_get_by_index_rcu(dev_net(dev), skb->skb_iif);
	if (unlikely(!to)) {
		dev_core_stats_tx_dropped_inc(dev);
		kfree_skb(skb);
		return NETDEV_TX_OK;
	}

	if (!skb->redirected && netdev_uses_dsa(to))
		return ppe_shaper_conduit_xmit(skb, dev, to);

	if (skb->redirected && dev == READ_ONCE(sh->priv->shaper_dev[1])) {
		conduit = ppe_shaper_ul_frame(sh->priv, skb, to);
		if (conduit) {
			skb->redirected = 0;
			return ppe_shaper_conduit_xmit(skb, dev, conduit);
		}
	}

	/* As an ifb hands a frame on: tc and netfilter are skipped on the
	 * way out, so that the redirect that brought it is not taken again.
	 */
	dev_sw_netstats_tx_add(dev, 1, skb->len);
	skb->redirected = 0;
#ifdef CONFIG_NET_CLS_ACT
	skb->tc_skip_classify = 1;
#endif
	nf_skip_egress(skb, true);
	skb->dev = to;
	skb->skb_iif = dev->ifindex;

	if (skb->from_ingress) {
		skb_pull_rcsum(skb, skb->mac_len);
		netif_rx(skb);
	} else {
		dev_queue_xmit(skb);
	}

	return NETDEV_TX_OK;
}

static const struct net_device_ops ppe_shaper_ops = {
	.ndo_start_xmit = ppe_shaper_xmit,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_get_stats64 = dev_get_tstats64,
};

#define PPE_SHAPER_FEATURES	(NETIF_F_HW_CSUM | NETIF_F_SG | \
				 NETIF_F_FRAGLIST | NETIF_F_TSO_ECN | \
				 NETIF_F_TSO | NETIF_F_TSO6 | \
				 NETIF_F_GSO_ENCAP_ALL | NETIF_F_HIGHDMA | \
				 NETIF_F_HW_VLAN_CTAG_TX | \
				 NETIF_F_HW_VLAN_STAG_TX)

static void ppe_shaper_setup(struct net_device *dev)
{
	dev->netdev_ops = &ppe_shaper_ops;
	ether_setup(dev);
	dev->tx_queue_len = 32;
	dev->features |= PPE_SHAPER_FEATURES;
	dev->hw_features |= dev->features;
	dev->hw_enc_features |= dev->features;
	dev->vlan_features |= PPE_SHAPER_FEATURES & ~(NETIF_F_HW_VLAN_CTAG_TX |
						      NETIF_F_HW_VLAN_STAG_TX);
	dev->flags |= IFF_NOARP;
	dev->flags &= ~IFF_MULTICAST;
	dev->priv_flags &= ~IFF_TX_SKB_SHARING;
	dev->lltx = true;
	dev->pcpu_stat_type = NETDEV_PCPU_STAT_TSTATS;
	netif_keep_dst(dev);
	eth_hw_addr_random(dev);
	dev->needs_free_netdev = true;
	dev->min_mtu = 0;
	dev->max_mtu = 0;
	netif_set_tso_max_size(dev, GSO_MAX_SIZE);
}

static struct net_device *ppe_shaper_create(struct qca_ppe_priv *priv,
					    const char *name)
{
	struct net_device *dev;
	struct ppe_shaper *sh;
	int ret;

	dev = alloc_netdev_mqs(sizeof(*sh), name, NET_NAME_USER,
			       ppe_shaper_setup, num_possible_cpus(), 1);
	if (!dev)
		return ERR_PTR(-ENOMEM);
	sh = netdev_priv(dev);
	sh->priv = priv;
	sh->conduit = dsa_to_port(&priv->ds, QCA_PPE_CPU_PORT)->conduit;

	ret = register_netdev(dev);
	if (ret) {
		free_netdev(dev);
		return ERR_PTR(ret);
	}

	return dev;
}

/* An upload frame the PPE built for the shaper device carries its PPPoE
 * header already; the qdisc classifies and marks by the IP header behind it,
 * which the network header points at. The uplink's VLAN tag, @vid, is in the
 * frame or, when the engine took it off on the way to the CPU, added on the
 * way out as the host would.
 */
static bool ppe_flow_push_parse(struct sk_buff *skb, u16 vid)
{
	unsigned int off = ETH_HLEN;
	__be16 type = eth_hdr(skb)->h_proto;

	if (eth_type_vlan(type)) {
		if (skb_headlen(skb) < off + VLAN_HLEN)
			return false;
		type = ((struct vlan_hdr *)(skb->data + off))->h_vlan_encapsulated_proto;
		off += VLAN_HLEN;
	} else if (vid) {
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vid);
	}

	if (type == htons(ETH_P_PPP_SES)) {
		__be16 ppp;

		if (skb_headlen(skb) < off + PPPOE_SES_HLEN)
			return false;
		ppp = *(__be16 *)(skb->data + off + sizeof(struct pppoe_hdr));
		type = ppp == htons(PPP_IP) ? htons(ETH_P_IP) :
		       ppp == htons(PPP_IPV6) ? htons(ETH_P_IPV6) : 0;
		off += PPPOE_SES_HLEN;
	}

	if (type != htons(ETH_P_IP) && type != htons(ETH_P_IPV6))
		return false;

	skb->protocol = type;
	skb_set_network_header(skb, off);

	return true;
}


/* A frame the PPE routed to the CPU port for a flow whose egress is a
 * device outside the switch, or behind a software shaper. It arrives
 * finished, so it goes straight out.
 */
bool ppe_flow_xmit(struct dsa_switch *ds, struct sk_buff *skb, u32 flow)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct net_device *conduit = skb->dev;
	struct net_device *dev, *shaper, *uplink;
	int xdev, up;
	u8 dscp, xport;

	if (flow >= priv->data->num_flow_entries)
		return false;
	xdev = READ_ONCE(priv->flow_xdev[flow]);
	if (!xdev)
		return false;

	dev = dev_get_by_index_rcu(dev_net(skb->dev), xdev & PPE_XDEV_IFINDEX);
	if (unlikely(!dev || !netif_running(dev)))
		goto drop;

	/* An upload shaper may sit on an ifb in front of the uplink, as the
	 * download one does: the frame is then built for the uplink and the
	 * ifb hands it on there once its qdisc lets it go. A frame the PPE
	 * built whole needs no uplink.
	 */
	uplink = dev;
	up = READ_ONCE(priv->uplink_dev);
	if ((xdev & (PPE_XDEV_UL | PPE_XDEV_PUSH)) == PPE_XDEV_UL && up &&
	    up != dev->ifindex) {
		uplink = dev_get_by_index_rcu(dev_net(dev), up);
		if (unlikely(!uplink || !netif_running(uplink)))
			goto drop;
	}

	/* What the egress classifies on - tc, the Wi-Fi access category -
	 * reads the IP header and the flow hash, so both are set as the
	 * receive path would have. The flow index is unique per flow.
	 */
	skb->dev = dev;
	skb_reset_mac_header(skb);
	skb_set_network_header(skb, ETH_HLEN);
	skb->protocol = eth_hdr(skb)->h_proto;
	skb_set_hash(skb, hash_32(flow, 32), PKT_HASH_TYPE_L4);

	if (xdev & PPE_XDEV_PUSH) {
		if (unlikely(!ppe_flow_push_parse(skb, READ_ONCE(priv->ul_vid))))
			goto drop;
	} else if ((xdev & PPE_XDEV_UL) && uplink->type != ARPHRD_ETHER) {
		/* An uplink that is not Ethernet, PPPoE's ppp device, takes
		 * the IP packet and builds its own link header.
		 */
		__skb_pull(skb, ETH_HLEN);
		skb_reset_network_header(skb);
		skb_reset_mac_header(skb);
	}

	dscp = READ_ONCE(priv->flow_dscp[flow]);
	if (dscp)
		ppe_flow_dscp_apply(skb, dscp & 0x3f);

	xport = READ_ONCE(priv->flow_port[flow]);

	/* Into the download shaper the way act_mirred sends an egress frame
	 * to an ifb: the ifb sends it on to skb_iif once its qdisc lets it go.
	 * The driver's own shaper device sends a frame for a switch port to
	 * the conduit instead. A shaper that has gone away leaves the frame
	 * unshaped, not lost.
	 */
	if (xdev & PPE_XDEV_DL) {
		shaper = dev_get_by_index_rcu(dev_net(dev),
					      READ_ONCE(priv->dl_shaper));
		if (likely(shaper && netif_running(shaper))) {
			u16 vid = READ_ONCE(priv->flow_vid[flow]);

			if (vid)
				__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q),
						       vid);
			skb->dev = shaper;
			if (!ppe_shaper_is(shaper)) {
				skb->skb_iif = dev->ifindex;
				skb_set_redirected(skb, false);
			} else if (xport && ppe_shaper_direct) {
				if (unlikely(!ppe_flow_tag(skb, xport)))
					goto drop;
				skb->skb_iif = conduit->ifindex;
			} else {
				skb->skb_iif = dev->ifindex;
			}
		} else {
			atomic_inc(&priv->flow_shaper_miss);
		}
	} else if (xdev & PPE_XDEV_UL) {
		ppe_flow_ct_attach(priv, skb, flow);
		if (xdev & PPE_XDEV_PUSH) {
			if (unlikely(!xport || !ppe_flow_tag(skb, xport)))
				goto drop;
			skb->skb_iif = conduit->ifindex;
		} else if (uplink != dev) {
			skb->skb_iif = uplink->ifindex;
			skb_set_redirected(skb, false);
		}
	}

	dev_queue_xmit(skb);
	return true;

drop:
	atomic_inc(&priv->flow_xmit_drop);
	kfree_skb(skb);
	return true;
}

/* Flows already handed over keep the DSCP they were built with, so a change
 * rebuilds them.
 */
void ppe_flow_ct_dscp_set(struct qca_ppe_priv *priv, bool on)
{
	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	if (priv->ct_dscp == on)
		return;
	priv->ct_dscp = on;
	ppe_flow_shaper_drop(priv, false);
	ppe_flow_shaper_drop(priv, true);
}

void ppe_flow_uplink_get(struct qca_ppe_priv *priv, char *name)
{
	guard(mutex)(&priv->flow_lock);

	strscpy(name, priv->uplink_name, IFNAMSIZ);
}

/* Name the WAN's L3 device, or clear it with an empty name. It decides which
 * flows are the shaped ones in both directions, so the flows of both are
 * rebuilt.
 */
int ppe_flow_uplink_set(struct qca_ppe_priv *priv, const char *name)
{
	struct net_device *dev;
	int ifindex = 0;

	if (*name) {
		dev = dev_get_by_name(&init_net, name);
		if (!dev)
			return -ENODEV;
		ifindex = dev->ifindex;
		dev_put(dev);
	}

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	priv->uplink_dev = ifindex;
	strscpy(priv->uplink_name, name, IFNAMSIZ);
	ppe_flow_shaper_path(priv);
	ppe_flow_shaper_drop(priv, false);
	ppe_flow_shaper_drop(priv, true);

	return 0;
}

void ppe_flow_shaper_get(struct qca_ppe_priv *priv, bool upload, char *name)
{
	guard(mutex)(&priv->flow_lock);

	strscpy(name, upload ? priv->ul_shaper_name : priv->dl_shaper_name,
		IFNAMSIZ);
}

/* Name the device whose qdisc shapes one direction, or clear it with an
 * empty name. A name no device has yet gets the driver's own shaper device,
 * which goes again once the direction is cleared or named another device.
 * The flows of that direction already in the table were built for the old
 * path, so they are dropped and the flowtable offloads them again on the new
 * one.
 */
int ppe_flow_shaper_set(struct qca_ppe_priv *priv, bool upload,
			const char *name)
{
	struct net_device *dev, *own = NULL, *old;
	int ifindex = 0;

	if (*name) {
		dev = dev_get_by_name(&init_net, name);
		if (!dev) {
			own = ppe_shaper_create(priv, name);
			if (IS_ERR(own))
				return PTR_ERR(own);
			dev = own;
			dev_hold(dev);
		}
		ifindex = dev->ifindex;
		dev_put(dev);
		if (ifindex > PPE_XDEV_IFINDEX) {
			if (own)
				unregister_netdev(own);
			return -ERANGE;
		}
	}

	scoped_guard(mutex, &priv->flow_lock) {
		guard(mutex)(&priv->vlan_lock);

		old = priv->shaper_dev[upload];
		if (old && old->ifindex == ifindex)
			old = NULL;
		else
			priv->shaper_dev[upload] = own;

		if (upload) {
			WRITE_ONCE(priv->ul_shaper, ifindex);
			strscpy(priv->ul_shaper_name, name, IFNAMSIZ);
			/* The uplink decides which downloads are shaped as well. */
			ppe_flow_shaper_path(priv);
			ppe_flow_shaper_drop(priv, false);
		} else {
			WRITE_ONCE(priv->dl_shaper, ifindex);
			strscpy(priv->dl_shaper_name, name, IFNAMSIZ);
		}

		ppe_flow_shaper_drop(priv, upload);
	}

	/* Frames on their way to it found it by ifindex under RCU, which the
	 * unregistration waits out.
	 */
	if (old)
		unregister_netdev(old);

	return 0;
}

/* One path's egress L3 interface and nexthop words: out of @port with the
 * headers pushed in hardware (@push), or handed over through the CPU port for
 * the host to build them. Returns the egress interface index.
 */
static int ppe_flow_egress_path(struct qca_ppe_priv *priv,
				struct ppe_flow_data *data,
				struct ppe_flow_entry *entry, int port,
				int nh_port, bool push, bool snat, bool dnat,
				u32 *nh)
{
	u32 words[PPE_NEXTHOP_WORDS] = {};
	struct dsa_port *odp;
	int eg_l3_if, ret;
	u32 eg_mtu;
	u64 mac;

	mac = ether_addr_to_u64(data->eth.h_source);
	ppe_entry_set(words, PPE_EG_L3_IF_MAC_OFF, PPE_EG_L3_IF_MAC_LEN, mac);
	if (data->pppoe_valid && push) {
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
	eg_mtu = odp->user->mtu + ETH_HLEN +
		 (data->vlan_valid && push ? VLAN_HLEN : 0);
	/* A flow handed to the uplink device leaves at that device's size,
	 * which for PPPoE is under the port's; a larger frame then takes the
	 * host's path, which fragments it or answers with ICMP.
	 */
	if (!push) {
		int ifindex = entry->xdev & PPE_XDEV_IFINDEX;
		struct net_device *ul;

		/* An upload shaper on an ifb hands the frame on to the uplink,
		 * whose size is the one that counts.
		 */
		if ((entry->xdev & PPE_XDEV_UL) && READ_ONCE(priv->uplink_dev))
			ifindex = READ_ONCE(priv->uplink_dev);

		rcu_read_lock();
		ul = dev_get_by_index_rcu(&init_net, ifindex);
		if (ul)
			eg_mtu = min_t(u32, eg_mtu, ul->mtu + ETH_HLEN);
		rcu_read_unlock();
	}
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
	eg_l3_if = ret + PPE_VSI_MAX;
	if (priv->eg_l3_if[eg_l3_if].refcount == 1) {
		ppe_tbl_write(priv, PPE_EG_L3_IF_TBL(eg_l3_if), words,
			      PPE_EG_L3_IF_WORDS);
		ppe_eg_l3_if_mtu_set(priv, eg_l3_if, eg_mtu);
	}

	memset(nh, 0, PPE_NEXTHOP_WORDS * sizeof(*nh));
	/* Type selects how the port field is read: as a physical port (1) or,
	 * left at 0, as a VSI whose FDB resolves the port. The egress port is
	 * known exactly here, so name it directly; left at 0 the port number
	 * would be read as a VSI.
	 */
	ppe_entry_set(nh, PPE_NEXTHOP_TYPE_OFF, PPE_NEXTHOP_TYPE_LEN,
		      PPE_NEXTHOP_TYPE_PORT);
	ppe_entry_set(nh, PPE_NEXTHOP_PORT_OFF, PPE_NEXTHOP_PORT_LEN, nh_port);
	ppe_entry_set(nh, PPE_NEXTHOP_POST_L3_IF_OFF,
		      PPE_NEXTHOP_POST_L3_IF_LEN, eg_l3_if);
	if (data->vlan_valid && push) {
		ppe_entry_set(nh, PPE_NEXTHOP_CTAG_FMT_OFF,
			      PPE_NEXTHOP_CTAG_FMT_LEN, 1);
		ppe_entry_set(nh, PPE_NEXTHOP_CVID_OFF,
			      PPE_NEXTHOP_CVID_LEN, data->vlan_id);
	}
	if (snat)
		ppe_entry_set(nh, PPE_NEXTHOP_PUB_IP_IDX_OFF,
			      PPE_NEXTHOP_PUB_IP_IDX_LEN, entry->pub_ip);
	ppe_entry_set(nh, PPE_NEXTHOP_MAC_OFF, PPE_NEXTHOP_MAC_LEN,
		      ether_addr_to_u64(data->eth.h_dest));
	if (dnat)
		ppe_entry_set(nh, PPE_NEXTHOP_DNAT_IP_OFF,
			      PPE_NEXTHOP_DNAT_IP_LEN, ntohl(data->v4_dst_new));

	return eg_l3_if;
}

/* Build the egress L3 interface, nexthop and public-address entries this rule
 * needs, and program any that were not already in use by another flow.
 */
static int ppe_flow_alloc_egress(struct qca_ppe_priv *priv,
				 struct ppe_flow_data *data, bool snat,
				 bool dnat, struct ppe_flow_entry *entry)
{
	u32 words[PPE_NEXTHOP_WORDS] = {};
	struct dsa_port *odp;
	int port, nh_port, ret;
	bool push;

	port = ppe_flow_port_by_ifindex(priv, data->odev->ifindex);
	if (port < 0 && ppe_cpu_xmit_ok(data)) {
		port = QCA_PPE_CPU_PORT;
		entry->xdev = data->odev->ifindex;
	}
	if (port < 0)
		return port;
	odp = dsa_to_port(&priv->ds, port);

	/* A flow handed to a shaper leaves the switch by the CPU port with its
	 * link headers still to be built by the host: an upload's PPPoE and
	 * VLAN by the uplink device, a download's LAN VLAN tag by
	 * ppe_flow_xmit(). An upload keeps the uplink below, which is what
	 * lets its reverse offload.
	 */
	nh_port = ppe_flow_shape(priv, data, entry, port);
	push = !(entry->xdev & (PPE_XDEV_UL | PPE_XDEV_DL));
	if ((entry->xdev & PPE_XDEV_UL) && ppe_flow_ul_push(priv)) {
		push = true;
		entry->xdev |= PPE_XDEV_PUSH;
	}

	entry->oport = nh_port;
	/* An untagged egress is credited to the port's PVID VLAN, the one an
	 * untagged frame on that port belongs to.
	 */
	entry->ovid = data->vlan_valid && !(entry->xdev & PPE_XDEV_UL) ?
		      data->vlan_id :
		      ppe_port_pvid(priv, nh_port);

	if (snat) {
		u32 pub = ntohl(data->v4_src_new);

		ret = ppe_res_get(priv->pub_ip, PPE_PUB_IP_ENTRIES, &pub, 1);
		if (ret < 0)
			goto err;
		entry->pub_ip = ret;
		if (priv->pub_ip[ret].refcount == 1)
			regmap_write(priv->regmap, PPE_PUB_IP_TBL(ret), pub);
	}

	ret = ppe_flow_egress_path(priv, data, entry, port, nh_port, push,
				   snat, dnat, words);
	if (ret < 0)
		goto err;
	entry->eg_l3_if = ret;

	ret = ppe_res_get(priv->nexthop, priv->data->num_nexthop_entries, words,
			  PPE_NEXTHOP_WORDS);
	if (ret < 0)
		goto err;
	entry->nexthop = ret;
	if (priv->nexthop[ret].refcount == 1)
		ppe_tbl_write(priv, PPE_IN_NEXTHOP_TBL(ret), words,
			      PPE_NEXTHOP_WORDS);

	/* The reverse of a flow that egresses PPPoE, or a VLAN of a port no
	 * bridge classifies, arrives the same way on this port; set the port up
	 * to route it so that direction offloads too.
	 */
	if (data->pppoe_valid ||
	    (data->vlan_valid && !priv->port_br_dev[port])) {
		ret = ppe_uplink_get(priv, port,
				     data->vlan_valid ? data->vlan_id : 0,
				     data->eth.h_source, odp->user->mtu);
		if (ret < 0)
			goto err;
		entry->uplink = ret;
	}

	if (data->pppoe_valid) {
		ret = ppe_pppoe_session_get(priv, entry->uplink,
					    data->pppoe_sid, data->eth.h_dest);
		if (ret < 0)
			goto err;
		entry->session = ret;
	}

	if (entry->uplink >= 0 && priv->uplink[entry->uplink].ref == 1)
		ppe_flow_purge_ingress(priv, &priv->uplink[entry->uplink]);

	return 0;

err:
	ppe_flow_free_egress(priv, entry);
	return ret;
}

/* The counters are cumulative and narrower than u64 - 32-bit packets, 40-bit
 * bytes - so the deltas are computed in the counters' own widths to survive
 * wraparound.
 */
static u32 ppe_flow_counter_delta(struct qca_ppe_priv *priv,
				  struct ppe_flow_entry *entry, u64 *bytes)
{
	u64 packets, total;
	u32 pkts;

	ppe_flow_counter_read(priv, entry->index, &packets, &total);
	pkts = (u32)(packets - entry->packets);
	*bytes = (total - entry->bytes) & PPE_FLOW_CNT_BYTES;
	entry->packets = packets;
	entry->bytes = total;

	return pkts;
}

static void ppe_flow_dev_add(struct net_device *dev, bool rx, u32 pkts,
			     u64 bytes)
{
	local_bh_disable();
	if (is_vlan_dev(dev)) {
		struct vlan_pcpu_stats *s;

		s = this_cpu_ptr(vlan_dev_priv(dev)->vlan_pcpu_stats);
		u64_stats_update_begin(&s->syncp);
		u64_stats_add(rx ? &s->rx_packets : &s->tx_packets, pkts);
		u64_stats_add(rx ? &s->rx_bytes : &s->tx_bytes, bytes);
		u64_stats_update_end(&s->syncp);
	} else if (netif_is_bridge_master(dev)) {
		struct pcpu_sw_netstats *s = this_cpu_ptr(dev->tstats);

		u64_stats_update_begin(&s->syncp);
		u64_stats_add(rx ? &s->rx_packets : &s->tx_packets, pkts);
		u64_stats_add(rx ? &s->rx_bytes : &s->tx_bytes, bytes);
		u64_stats_update_end(&s->syncp);
	}
	local_bh_enable();
}

static void ppe_flow_account_side(struct qca_ppe_priv *priv, int port,
				  u16 vid, bool rx, u32 pkts, u64 bytes)
{
	struct net_device *dev = priv->port_br_dev[port];

	if (dev)
		ppe_flow_dev_add(dev, rx, pkts, bytes);
	else
		dev = dsa_to_port(&priv->ds, port)->user;

	if (dev && vid) {
		dev = __vlan_find_dev_deep_rcu(dev, htons(ETH_P_8021Q), vid);
		if (dev)
			ppe_flow_dev_add(dev, rx, pkts, bytes);
	}
}

/* What the hardware forwarded never crossed the software interfaces on the
 * way: the bridge a port is in and the VLAN interface of the VLAN the frame
 * carries, received on the ingress side and sent on the egress side. Their
 * counters get the flow's deltas so they read as without offload. The port
 * itself is left alone; its MIB already counts the frames.
 *
 * A delta can be read only once, because the read moves the baseline, so the
 * flowtable is answered from what this banks rather than from the counter.
 *
 * The bridge pointer is written under the flow lock by the bridge join and
 * leave ops, and a VLAN interface is found under RCU rather than held.
 */
static void ppe_flow_account(struct qca_ppe_priv *priv,
			     struct ppe_flow_entry *entry, u32 pkts, u64 bytes)
{
	lockdep_assert_held(&priv->flow_lock);

	if (!pkts)
		return;

	entry->unread_packets += pkts;
	entry->unread_bytes += bytes;

	rcu_read_lock();
	ppe_flow_account_side(priv, entry->iport, entry->ivid, true, pkts,
			      bytes);
	ppe_flow_account_side(priv, entry->oport, entry->ovid, false, pkts,
			      bytes);
	rcu_read_unlock();
}

/* Both locks: the release below reaches the VLAN side, and a routing domain
 * going away destroys flows with that lock already held, so taking it here
 * would be the same task asking for it twice.
 */
static void ppe_flow_entry_destroy(struct qca_ppe_priv *priv,
				   struct ppe_flow_entry *entry)
{
	u64 bytes;
	int age;

	lockdep_assert_held(&priv->flow_lock);
	lockdep_assert_held(&priv->vlan_lock);

	/* Delete by index - the key does not have to be restaged. Only a read
	 * that says the slot belongs to another flow is a reason not to: the
	 * hardware removes an idle entry on its own and may have handed the
	 * slot on, but a read that failed leaves it unknown, and leaving a live
	 * entry pointing at side tables about to be reused is the worse half of
	 * that trade.
	 */
	age = ppe_flow_entry_age(priv, entry);
	if (age >= 0) {
		u32 pkts = ppe_flow_counter_delta(priv, entry, &bytes);

		ppe_flow_account(priv, entry, pkts, bytes);
	}
	/* A slot handed on may carry another flow's egress by now. */
	if (age != -ENOENT) {
		if (entry->xdev)
			ppe_flow_slot_clear(priv, entry->index);
		ppe_flow_entry_delete(priv, entry->index);
	}
	/* The conntrack core may be a module: put it through its hook. */
	if (entry->ct)
		nf_conntrack_put(&((struct nf_conn *)(entry->ct & ~1UL))->ct_general);
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

	if (ppe_flow_proto(data->l4proto) == PPE_FLOW_PROTO_OTHER) {
		ppe_entry_set(fw, PPE_FLOW_E_IP_PROTO_OFF,
			      PPE_FLOW_E_IP_PROTO_LEN, data->l4proto);
	} else {
		ppe_entry_set(fw, PPE_FLOW_E_SPORT_OFF, PPE_FLOW_E_SPORT_LEN,
			      ntohs(data->sport));
		ppe_entry_set(fw, PPE_FLOW_E_DPORT_OFF, PPE_FLOW_E_DPORT_LEN,
			      ntohs(data->dport));
	}

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
		 * switch port, Wi-Fi included, has no source port to match.
		 */
		iport = ppe_flow_port_by_ifindex(priv,
						 match.key->ingress_ifindex);
		if (iport < 0)
			return ppe_flow_reject(priv, PPE_REJECT_INGRESS_PORT);
	}

	/* An ingress tag is matched by the VSI it is classified into - the
	 * uplink's tag on a PPPoE port, a bridge VLAN the port is a member
	 * of - never by the tag itself, which the hardware key cannot hold.
	 * A tag this driver has no classification for, and a second tag,
	 * stay in software.
	 */
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CVLAN))
		return ppe_flow_reject(priv, PPE_REJECT_INGRESS_VLAN);
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_match_vlan match;

		flow_rule_match_vlan(rule, &match);
		if (match.key->vlan_tpid != htons(ETH_P_8021Q))
			return ppe_flow_reject(priv, PPE_REJECT_INGRESS_VLAN);
		data.ivid = match.key->vlan_id;
	}

	/* A replace for a cookie already held is the kernel refreshing a flow
	 * whose packets it saw in the software path. If the hardware entry is
	 * intact and its port still classifies into the VSI it is keyed on, the
	 * refresh is a no-op. Otherwise the hardware has lost it or no longer
	 * matches it - a bridge join or a PVID change moves the port - and this
	 * is the moment to reinstall, or the flow would stay in software until
	 * the entry ages out.
	 */
	entry = rhashtable_lookup_fast(&priv->flow_table, &f->cookie,
				       ppe_flow_ht_params);
	if (entry) {
		if (entry->src_if == ppe_flow_src_if(priv, iport, data.ivid) &&
		    ppe_flow_entry_age(priv, entry) >= 0)
			return 0;

		priv->flow_reinstalled++;
		ppe_flow_free(priv, entry);
	}

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

	/* The entry matches any flags; a flag the rule excludes is only
	 * honoured if its exception takes it to the CPU.
	 */
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_TCP)) {
		struct flow_match_tcp match;

		flow_rule_match_tcp(rule, &match);
		if (match.key->flags & match.mask->flags ||
		    match.mask->flags & ~htons(PPE_FLOW_TCP_TRAP))
			return ppe_flow_reject(priv, PPE_REJECT_KEY);
	}

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
			/* A priority the profiles cannot hold, such as the
			 * 802.1d tags of 256 and up that Wi-Fi stamps take,
			 * leaves the flow to DSCP: refusing it would send the
			 * whole flow to the CPU for the sake of its queue.
			 */
			if (act->priority <= PPE_QOS_MAX_PRI)
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

	if (ppe_flow_proto(data.l4proto) != PPE_FLOW_PROTO_OTHER) {
		struct flow_match_ports match;

		if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS))
			return ppe_flow_reject(priv, PPE_REJECT_KEY);

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
	entry->uplink = -1;
	entry->in_uplink = -1;
	entry->session = -1;
	entry->iport = iport;

	ret = ppe_flow_alloc_ingress(priv, iport, data.ivid, entry);
	if (ret) {
		priv->flow_reject[ret == -EOPNOTSUPP ? PPE_REJECT_INGRESS_VLAN :
				  PPE_REJECT_RESOURCE]++;
		goto err_free;
	}

	ret = ppe_flow_alloc_egress(priv, &data, snat, dnat, entry);
	if (ret) {
		priv->flow_reject[ret == -ENOSPC ? PPE_REJECT_RESOURCE :
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
	if (entry->xdev & PPE_XDEV_UL)
		entry->ct = ppe_flow_ct_get(f->cookie);
	if (entry->xdev && priv->ct_dscp)
		entry->dscp = ppe_flow_ct_dscp(f->cookie);
	if (entry->xdev)
		ppe_flow_slot_set(priv, entry, entry->index);

	/* The hardware writes the host index it resolved into the entry and
	 * counts the age down from there, so the stored image carries the one
	 * and drops the other for readbacks to compare equal.
	 */
	memcpy(entry->words, fw, nfw * sizeof(*fw));
	entry->nwords = nfw;
	memcpy(entry->hwords, hw, nhw * sizeof(*hw));
	entry->nhwords = nhw;
	entry->profile = data.priority;
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

	ppe_flow_free(priv, entry);

	return 0;
}

static int ppe_flow_offload_stats(struct qca_ppe_priv *priv,
				  struct flow_cls_offload *f)
{
	struct ppe_flow_entry *entry;
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
	if (age >= 0 && (entry->unread_packets || age == PPE_FLOW_AGE_MAX))
		entry->last_used = jiffies;

	flow_stats_update(&f->stats, entry->unread_bytes, entry->unread_packets,
			  0, entry->last_used, FLOW_ACTION_HW_STATS_DELAYED);
	entry->unread_packets = 0;
	entry->unread_bytes = 0;

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

		ppe_flow_free(priv, entry);
	}
	mutex_unlock(&priv->vlan_lock);
	mutex_unlock(&priv->flow_lock);

	kfree(fb);
}

/* Every user port of the switch binds the same flowtable block, so it is shared
 * and reference counted rather than refused as busy.
 */
int ppe_setup_ft_block(struct qca_ppe_priv *priv,
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

/* A shared queue gives a sparse flow the loss rate the bulk imposes on it,
 * and a flow that sends a few packets a second recovers a lost one by a
 * retransmission timer, not by the next packet: a speed test's latency
 * probe stalls for hundreds of milliseconds while the download it measures
 * is unharmed. The hardware has no queue per flow, but a flow entry names a
 * priority profile the classifier ranks above every other source, so a flow
 * the counters show to be sparse is moved to the list the scheduler serves
 * ahead of the band, inside the band's shaper, and back the moment it is
 * not. A flow is sparse after ten periods under the packet count, one
 * second, which a TCP flow that is ramping never spends; a promoted flow
 * that bursts is demoted at the end of the period it burst in.
 */
#define PPE_SPARSE_PERIOD_MS	100
#define PPE_SPARSE_PKTS		8
#define PPE_SPARSE_QUIET	10

static bool ppe_sparse_flows = true;
module_param_named(sparse_flows, ppe_sparse_flows, bool, 0644);
MODULE_PARM_DESC(sparse_flows,
		 "Serve flows of under 80 packets a second ahead of a shaped port's bulk queue");

/* The entry is replaced rather than rewritten in place: an IPv6 entry's
 * words are spread over two slots in a layout only the op engine holds. The
 * host entry is the same one, found again by its key; the counter starts
 * over with the new slot, and so does everything derived from it.
 */
static int ppe_flow_profile_set(struct qca_ppe_priv *priv,
				struct ppe_flow_entry *entry, u8 profile)
{
	u32 w[PPE_FLOW_ENTRY_WORDS_V6];
	u32 index, host_index, pkts;
	u64 bytes;
	int ret;

	lockdep_assert_held(&priv->flow_lock);

	/* The hardware ages an idle entry out and hands its slot on, and a
	 * flow under the count is the likeliest to have been; a delete by
	 * index would then take the sibling's entry.
	 */
	ret = ppe_flow_entry_age(priv, entry);
	if (ret < 0)
		return ret;

	pkts = ppe_flow_counter_delta(priv, entry, &bytes);
	ppe_flow_account(priv, entry, pkts, bytes);

	/* The stored image carries the host index and no age; an add is
	 * staged the way the encoder stages it, at full age and with no host
	 * index, for the hardware to write the one it resolves.
	 */
	memcpy(w, entry->words, entry->nwords * sizeof(*w));
	w[0] = (w[0] & ~PPE_FLOW_E_HOST_IDX_MASK) |
	       FIELD_PREP(PPE_FLOW_E_AGE_MASK, PPE_FLOW_AGE_MAX);
	ppe_entry_clear(w, PPE_FLOW_E_PRI_PROFILE_OFF,
			PPE_FLOW_E_PRI_PROFILE_LEN);
	ppe_entry_set(w, PPE_FLOW_E_PRI_PROFILE_OFF, PPE_FLOW_E_PRI_PROFILE_LEN,
		      profile);

	ret = ppe_flow_entry_delete(priv, entry->index);
	if (ret)
		return ret;
	if (entry->xdev)
		ppe_flow_slot_clear(priv, entry->index);

	ret = ppe_flow_op(priv, PPE_TBL_OP_ADD, w, entry->nwords,
			  entry->hwords, entry->nhwords, &index, &host_index);
	if (ret)
		return ret;
	/* The re-added entry may sit in another slot, and its frames name it. */
	if (entry->xdev)
		ppe_flow_slot_set(priv, entry, index);

	if (host_index != entry->host_index) {
		ppe_host_ref_get(priv, host_index);
		ppe_host_ref_put(priv, entry->host_index);
		entry->host_index = host_index;
	}
	memcpy(entry->words, w, entry->nwords * sizeof(*w));
	entry->words[0] = (w[0] | host_index << PPE_FLOW_E_HOST_IDX_OFF) &
			  ~PPE_FLOW_E_AGE_MASK;
	entry->index = index;
	ppe_flow_counter_clear(priv, index);
	entry->packets = 0;
	entry->bytes = 0;

	return 0;
}

static void ppe_sparse_work(struct work_struct *work)
{
	struct qca_ppe_priv *priv = container_of(work, struct qca_ppe_priv,
						 sparse_work.work);
	struct ppe_flow_entry *entry;
	u32 pkts, delta;

	mutex_lock(&priv->flow_lock);
	list_for_each_entry(entry, &priv->flow_list, list) {
		u64 bytes;

		regmap_read(priv->regmap, PPE_IN_FLOW_CNT_TBL(entry->index),
			    &pkts);
		delta = pkts - (u32)entry->packets;

		/* The age read costs the entry's words and only a flow that
		 * moved has anything to attribute, so it is taken on the
		 * delta rather than every period.
		 */
		if (delta && ppe_flow_entry_age(priv, entry) >= 0) {
			u32 acct = ppe_flow_counter_delta(priv, entry, &bytes);

			ppe_flow_account(priv, entry, acct, bytes);
		}

		if (delta > PPE_SPARSE_PKTS || !ppe_sparse_flows) {
			entry->quiet = 0;
			if (entry->sparse &&
			    !ppe_flow_profile_set(priv, entry, entry->profile)) {
				entry->sparse = false;
				priv->flow_sparse_demoted++;
			}
			continue;
		}

		if (entry->sparse)
			continue;
		if (entry->quiet < PPE_SPARSE_QUIET) {
			entry->quiet++;
			continue;
		}
		if (!ppe_flow_profile_set(priv, entry, PPE_QOS_SPARSE_PRI)) {
			entry->sparse = true;
			priv->flow_sparse_promoted++;
		}
	}
	mutex_unlock(&priv->flow_lock);

	schedule_delayed_work(&priv->sparse_work,
			      msecs_to_jiffies(PPE_SPARSE_PERIOD_MS));
}

int ppe_flow_offload_init(struct qca_ppe_priv *priv)
{
	struct device *dev = priv->ds.dev;
	int ret;

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
	priv->flow_xdev = devm_kcalloc(dev, priv->data->num_flow_entries,
				       sizeof(*priv->flow_xdev), GFP_KERNEL);
	priv->ul_port = -1;
	priv->flow_ct = devm_kcalloc(dev, priv->data->num_flow_entries,
				     sizeof(*priv->flow_ct), GFP_KERNEL);
	priv->flow_dscp = devm_kcalloc(dev, priv->data->num_flow_entries,
				       sizeof(*priv->flow_dscp), GFP_KERNEL);
	priv->flow_vid = devm_kcalloc(dev, priv->data->num_flow_entries,
				      sizeof(*priv->flow_vid), GFP_KERNEL);
	priv->flow_port = devm_kcalloc(dev, priv->data->num_flow_entries,
				       sizeof(*priv->flow_port), GFP_KERNEL);
	if (!priv->eg_l3_if || !priv->pub_ip || !priv->nexthop ||
	    !priv->host_ref || !priv->my_mac || !priv->flow_xdev ||
	    !priv->flow_ct || !priv->flow_dscp || !priv->flow_vid ||
	    !priv->flow_port)
		return -ENOMEM;

	INIT_LIST_HEAD(&priv->flow_list);

	ret = rhashtable_init(&priv->flow_table, &ppe_flow_ht_params);
	if (ret)
		return ret;

	priv->netdev_nb.notifier_call = ppe_flow_netdev_event;
	ret = register_netdevice_notifier(&priv->netdev_nb);
	if (ret) {
		rhashtable_destroy(&priv->flow_table);
		return ret;
	}

	INIT_DELAYED_WORK(&priv->sparse_work, ppe_sparse_work);
	schedule_delayed_work(&priv->sparse_work,
			      msecs_to_jiffies(PPE_SPARSE_PERIOD_MS));

	return 0;
}

void ppe_flow_offload_exit(struct qca_ppe_priv *priv)
{
	struct ppe_flow_entry *entry, *tmp;
	int i;

	unregister_netdevice_notifier(&priv->netdev_nb);
	cancel_delayed_work_sync(&priv->sparse_work);

	mutex_lock(&priv->flow_lock);
	mutex_lock(&priv->vlan_lock);
	list_for_each_entry_safe(entry, tmp, &priv->flow_list, list) {
		ppe_flow_free(priv, entry);
	}
	mutex_unlock(&priv->vlan_lock);
	mutex_unlock(&priv->flow_lock);

	rhashtable_destroy(&priv->flow_table);

	for (i = 0; i < ARRAY_SIZE(priv->shaper_dev); i++)
		if (priv->shaper_dev[i])
			unregister_netdev(priv->shaper_dev[i]);
}
