// SPDX-License-Identifier: GPL-2.0-or-later OR MIT
/* Copyright (c) 2026 Julius Bairaktaris <julius@bairaktaris.de> */
/* Hardware classifier (ACL/IPO) for the Qualcomm PPE.
 *
 * The engine holds 512 entries as 64 lists of eight. One entry carries a
 * 53-bit key, a mask and an action, and a rule needing more key than that
 * takes several entries of one list which the list's RULE_EXT bits merge into
 * a single match. Only the pairs (0,1) (2,3) (4,5) (6,7), (0,2) (4,6) and
 * (0,4) can be merged, so a rule of n entries has to land on one of the shapes
 * those pairings build - which is what the allocator here is for.
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/module.h>
#include <net/flow_offload.h>
#include <net/ipv6.h>

#include "qca_ppe.h"

/* One entry of a rule: the key, its mask, and the rule type that tells the
 * engine which bits of the packet the key is compared against. A range
 * compare takes the mask slot of the field it names for the range maximum,
 * leaving the entry's other masks alone, and the entry it lands on has to be
 * an even one.
 */
struct ppe_acl_slice {
	u8 type;
	bool range;
	u32 key[2];
	u32 mask[2];
};

/* The entries of one list a rule may occupy, and the RULE_EXT bits that chain
 * them. Taken from the vendor's table in its order: the single-entry shapes
 * name the odd entries first, which leaves the even ones - the only place a
 * range compare may sit - free for longer.
 */
static const struct {
	u8 num;
	u8 ext1;
	u8 ext2;
	u8 ext4;
	u8 entries;
} ppe_acl_shape[] = {
	{ 1, 0x0, 0x0, 0x0, 0x02 },
	{ 1, 0x0, 0x0, 0x0, 0x08 },
	{ 1, 0x0, 0x0, 0x0, 0x20 },
	{ 1, 0x0, 0x0, 0x0, 0x80 },
	{ 1, 0x0, 0x0, 0x0, 0x01 },
	{ 1, 0x0, 0x0, 0x0, 0x04 },
	{ 1, 0x0, 0x0, 0x0, 0x10 },
	{ 1, 0x0, 0x0, 0x0, 0x40 },
	{ 2, 0x1, 0x0, 0x0, 0x03 },
	{ 2, 0x2, 0x0, 0x0, 0x0c },
	{ 2, 0x4, 0x0, 0x0, 0x30 },
	{ 2, 0x8, 0x0, 0x0, 0xc0 },
	{ 2, 0x0, 0x1, 0x0, 0x05 },
	{ 2, 0x0, 0x2, 0x0, 0x50 },
	{ 2, 0x0, 0x0, 0x1, 0x11 },
	{ 3, 0x1, 0x1, 0x0, 0x07 },
	{ 3, 0x1, 0x0, 0x1, 0x13 },
	{ 3, 0x2, 0x1, 0x0, 0x0d },
	{ 3, 0x4, 0x2, 0x0, 0x70 },
	{ 3, 0x4, 0x0, 0x1, 0x31 },
	{ 3, 0x8, 0x2, 0x0, 0xd0 },
	{ 3, 0x0, 0x1, 0x1, 0x15 },
	{ 3, 0x0, 0x2, 0x1, 0x51 },
	{ 4, 0x3, 0x1, 0x0, 0x0f },
	{ 4, 0x5, 0x0, 0x1, 0x33 },
	{ 4, 0x2, 0x1, 0x1, 0x1d },
	{ 4, 0xc, 0x2, 0x0, 0xf0 },
	{ 4, 0x4, 0x1, 0x1, 0x35 },
	{ 4, 0x8, 0x2, 0x1, 0xd1 },
	{ 4, 0x0, 0x3, 0x1, 0x55 },
	{ 5, 0x3, 0x1, 0x1, 0x1f },
	{ 5, 0x6, 0x1, 0x1, 0x3d },
	{ 5, 0xc, 0x2, 0x1, 0xf1 },
	{ 5, 0x8, 0x3, 0x1, 0xd5 },
	{ 6, 0x7, 0x1, 0x1, 0x3f },
	{ 6, 0x6, 0x3, 0x1, 0x7d },
	{ 6, 0xc, 0x3, 0x1, 0xf5 },
	{ 7, 0x7, 0x3, 0x1, 0x7f },
	{ 7, 0xe, 0x3, 0x1, 0xfd },
	{ 8, 0xf, 0x3, 0x1, 0xff },
};

/* Entries of a list a range compare may sit on. */
#define PPE_ACL_EVEN_ENTRIES	0x55

/* One rule's placement: which list, which of its entries, and the RULE_EXT
 * bits this rule set in that list.
 */
struct ppe_acl_group {
	u8 list;
	u8 entries;
	u8 ext1;
	u8 ext2;
	u8 ext4;
	u8 index[PPE_ACL_LIST_ENTRIES];
	u8 nslices;
};

static void ppe_acl_ext_write(struct qca_ppe_priv *priv,
			      const struct ppe_acl_group *g, bool set)
{
	regmap_update_bits(priv->regmap, PPE_ACL_RULE_EXT1(g->list), g->ext1,
			   set ? g->ext1 : 0);
	regmap_update_bits(priv->regmap, PPE_ACL_RULE_EXT2(g->list), g->ext2,
			   set ? g->ext2 : 0);
	regmap_update_bits(priv->regmap, PPE_ACL_RULE_EXT4(g->list), g->ext4,
			   set ? g->ext4 : 0);
}

/* Hand one entry of the shape to a slice. A range compare has to have an even
 * entry; everything else takes an odd one first, which leaves the even ones to
 * the ranges of the same rule - the shape was picked with enough of them, and
 * with exactly one entry per slice, so there is always something to hand out.
 */
static u8 ppe_acl_entry_take(u8 *avail, bool range)
{
	u8 want = range ? PPE_ACL_EVEN_ENTRIES : ~PPE_ACL_EVEN_ENTRIES;
	u8 pick = *avail & want ? *avail & want : *avail;

	pick &= -pick;
	*avail &= ~pick;

	return __ffs(pick);
}

/* Place a rule on the first list carrying a free shape of the right width with
 * an even entry left for each of the rule's range compares.
 */
static int ppe_acl_alloc(struct qca_ppe_priv *priv,
			 const struct ppe_acl_slice *slice, int nslices,
			 struct ppe_acl_group *g)
{
	int i, l, n, nranges = 0;
	u8 avail;

	for (i = 0; i < nslices; i++)
		nranges += slice[i].range;

	for (l = 0; l < PPE_ACL_LISTS; l++) {
		for (i = 0; i < ARRAY_SIZE(ppe_acl_shape); i++) {
			u8 e = ppe_acl_shape[i].entries;

			if (ppe_acl_shape[i].num != nslices ||
			    (priv->acl_free[l] & e) != e ||
			    hweight8(e & PPE_ACL_EVEN_ENTRIES) < nranges)
				continue;

			priv->acl_free[l] &= ~e;
			g->list = l;
			g->entries = e;
			g->ext1 = ppe_acl_shape[i].ext1;
			g->ext2 = ppe_acl_shape[i].ext2;
			g->ext4 = ppe_acl_shape[i].ext4;
			g->nslices = nslices;

			avail = e;
			for (n = 0; n < nslices; n++)
				g->index[n] = l * PPE_ACL_LIST_ENTRIES +
					      ppe_acl_entry_take(&avail,
							slice[n].range);

			/* The chain has to exist before any of its entries
			 * goes live: an armed entry whose RULE_EXT bit is
			 * still clear is a rule of its own, matching far more
			 * than what was asked for and carrying the action.
			 */
			ppe_acl_ext_write(priv, g, true);

			return 0;
		}
	}

	return -ENOSPC;
}

/* Zeroing the rule words clears the source bitmap, which is the only thing
 * that makes an entry live; the chain goes last for the same reason it was
 * built first.
 */
static void ppe_acl_free(struct qca_ppe_priv *priv, struct ppe_acl_group *g)
{
	int i, j;

	for (i = 0; i < g->nslices; i++)
		for (j = 0; j < PPE_ACL_RULE_WORDS; j++)
			regmap_write(priv->regmap,
				     PPE_ACL_RULE(g->index[i]) + j * 4, 0);

	ppe_acl_ext_write(priv, g, false);
	priv->acl_free[g->list] |= g->entries;
	g->nslices = 0;
}

/* Arm one entry. The engine commits an entry on the write to its last word, so
 * the action and the mask are in place before the rule word that carries the
 * source bitmap lands, and the rule is disarmed first in case it was live.
 */
static void ppe_acl_slice_write(struct qca_ppe_priv *priv, u32 index,
				const struct ppe_acl_slice *s, const u32 *act,
				u32 ports, u16 pri)
{
	u32 w[PPE_ACL_ACTION_WORDS];
	int i;

	for (i = 0; i < PPE_ACL_RULE_WORDS; i++)
		regmap_write(priv->regmap, PPE_ACL_RULE(index) + i * 4, 0);

	for (i = 0; i < PPE_ACL_ACTION_WORDS; i++)
		regmap_write(priv->regmap, PPE_ACL_ACTION(index) + i * 4,
			     act[i]);

	memset(w, 0, sizeof(w));
	w[0] = s->mask[0];
	w[1] = s->mask[1];
	for (i = 0; i < PPE_ACL_MASK_WORDS; i++)
		regmap_write(priv->regmap, PPE_ACL_MASK(index) + i * 4, w[i]);

	memset(w, 0, sizeof(w));
	w[0] = s->key[0];
	w[1] = s->key[1] |
	       FIELD_PREP(PPE_ACL_RULE_TYPE, s->type) |
	       (s->range ? PPE_ACL_RANGE_EN : 0) |
	       FIELD_PREP(PPE_ACL_SRC_TYPE, PPE_ACL_SRC_PORT_BMP) |
	       FIELD_PREP(PPE_ACL_SRC_LO, ports & 0x7);
	w[2] = FIELD_PREP(PPE_ACL_SRC_HI, ports >> 3) |
	       FIELD_PREP(PPE_ACL_RULE_PRI, pri);
	for (i = 0; i < PPE_ACL_RULE_WORDS; i++)
		regmap_write(priv->regmap, PPE_ACL_RULE(index) + i * 4, w[i]);
}

/* The keys the rule types below cover. A filter carrying anything else is
 * declined rather than offloaded without it: a key the driver drops makes the
 * rule match a superset of what was asked for, and the action goes with it.
 */
#define PPE_ACL_MATCH_KEYS					\
	(BIT_ULL(FLOW_DISSECTOR_KEY_CONTROL) |			\
	 BIT_ULL(FLOW_DISSECTOR_KEY_BASIC) |			\
	 BIT_ULL(FLOW_DISSECTOR_KEY_ETH_ADDRS) |		\
	 BIT_ULL(FLOW_DISSECTOR_KEY_IPV4_ADDRS) |		\
	 BIT_ULL(FLOW_DISSECTOR_KEY_IPV6_ADDRS) |		\
	 BIT_ULL(FLOW_DISSECTOR_KEY_PORTS) |			\
	 BIT_ULL(FLOW_DISSECTOR_KEY_PORTS_RANGE) |		\
	 BIT_ULL(FLOW_DISSECTOR_KEY_ICMP) |			\
	 BIT_ULL(FLOW_DISSECTOR_KEY_IP) |			\
	 BIT_ULL(FLOW_DISSECTOR_KEY_TCP))

/* TCP flags as the header carries them, FIN first. */
#define PPE_ACL_TCP_FLAGS_MAX	GENMASK(5, 0)

/* A filter needs at most one entry per rule type, which is more than one
 * hardware list holds; the width is checked once the whole filter is parsed.
 */
#define PPE_ACL_MAX_SLICES	(PPE_ACL_TYPE_IPMISC + 1)

struct ppe_acl_rule {
	struct list_head list;
	unsigned long cookie;
	/* The ethtool location this rule was inserted at, or -1 for one that
	 * came from tc. ethtool has to hand the whole spec back on a get, so
	 * the rules it owns keep theirs.
	 */
	int loc;
	struct ethtool_rx_flow_spec *fs;
	int port;
	int meter;
	bool mirror;
	struct ppe_acl_group group;
	u32 act[PPE_ACL_ACTION_WORDS];
};

/* One entry per rule type, reused as more of the filter is parsed into it. */
static struct ppe_acl_slice *ppe_acl_slice_get(struct ppe_acl_slice *slice,
					       int *n, u8 type)
{
	int i;

	for (i = 0; i < *n; i++)
		if (slice[i].type == type)
			return &slice[i];

	slice[*n].type = type;

	return &slice[(*n)++];
}

/* A MAC address sits low byte first: bytes 5 to 2 in word 0, bytes 1 and 0 in
 * word 1.
 */
static void ppe_acl_mac_words(u32 *w, const u8 *mac)
{
	w[0] = mac[5] | mac[4] << 8 | mac[3] << 16 | mac[2] << 24;
	w[1] = FIELD_PREP(PPE_ACL_MAC_HI, mac[1] | mac[0] << 8);
}

/* An IPv6 address needs three entries: its low 32 bits and the 16 above them
 * in the first, the next 48 in the second, the top 32 in the third - which
 * leaves the third entry's port field free for the L4 port.
 */
static void ppe_acl_ip6_words(u32 w[3][2], const __be32 *addr)
{
	u32 v[4];
	int i;

	for (i = 0; i < 4; i++)
		v[i] = ntohl(addr[i]);

	w[0][0] = FIELD_PREP(PPE_ACL_IP_PORT, v[3]) |
		  FIELD_PREP(PPE_ACL_IP_LO, v[3] >> 16);
	w[0][1] = FIELD_PREP(PPE_ACL_IP_HI, v[2]);
	w[1][0] = FIELD_PREP(PPE_ACL_IP_PORT, v[2] >> 16) |
		  FIELD_PREP(PPE_ACL_IP_LO, v[1]);
	w[1][1] = FIELD_PREP(PPE_ACL_IP_HI, v[1] >> 16);
	w[2][0] = FIELD_PREP(PPE_ACL_IP_LO, v[0]);
	w[2][1] = FIELD_PREP(PPE_ACL_IP_HI, v[0] >> 16);
}

static void ppe_acl_key_ip6(struct ppe_acl_slice *slice, int *n, u8 type,
			    const struct in6_addr *key,
			    const struct in6_addr *mask)
{
	u32 kw[3][2], mw[3][2];
	int i;

	ppe_acl_ip6_words(kw, key->s6_addr32);
	ppe_acl_ip6_words(mw, mask->s6_addr32);

	for (i = 0; i < 3; i++) {
		struct ppe_acl_slice *s;

		/* A prefix shorter than the address leaves whole entries with
		 * nothing to compare, and an entry that compares nothing still
		 * costs one of the list's eight. The vendor gates each rule
		 * type on its own half of the mask; so does this.
		 */
		if (!mw[i][0] && !mw[i][1])
			continue;

		s = ppe_acl_slice_get(slice, n, type + i);
		s->key[0] |= kw[i][0];
		s->key[1] |= kw[i][1];
		s->mask[0] |= mw[i][0];
		s->mask[1] |= mw[i][1];
	}
}

/* Turn the filter into entries: one per rule type it needs, each carrying the
 * part of the key that rule type compares. Returns how many.
 */
static int ppe_acl_parse_key(struct flow_rule *rule,
			     struct netlink_ext_ack *extack,
			     struct ppe_acl_slice *slice, __be16 *family)
{
	struct ppe_acl_slice *s;
	u8 sip_type, dip_type;
	u16 addr_type = 0;
	__be16 proto = 0;
	int n = 0;

	if (rule->match.dissector->used_keys & ~PPE_ACL_MATCH_KEYS) {
		NL_SET_ERR_MSG_MOD(extack, "match key the classifier has no field for");
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL)) {
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		addr_type = match.key->addr_type;
		if (!flow_rule_is_supp_control_flags(FLOW_DIS_IS_FRAGMENT,
						     match.mask->flags, extack))
			return -EOPNOTSUPP;

		if (match.mask->flags & FLOW_DIS_IS_FRAGMENT) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPMISC);
			if (match.key->flags & FLOW_DIS_IS_FRAGMENT)
				s->key[1] |= PPE_ACL_L3_FRAG;
			s->mask[1] |= PPE_ACL_L3_FRAG;
		}
	}

	/* The engine tells the families apart by one bit rather than by the
	 * ethertype, which is also what picks the IPv4 or IPv6 rule types for
	 * everything below, so an L3 or L4 match needs the family named.
	 */
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		if (match.mask->n_proto) {
			if (match.mask->n_proto != htons(0xffff)) {
				NL_SET_ERR_MSG_MOD(extack, "the protocol is matched whole or not at all");
				return -EOPNOTSUPP;
			}
			proto = match.key->n_proto;
			if (proto != htons(ETH_P_IP) &&
			    proto != htons(ETH_P_IPV6)) {
				NL_SET_ERR_MSG_MOD(extack, "only IPv4 and IPv6 are matched by protocol");
				return -EOPNOTSUPP;
			}
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPMISC);
			if (proto == htons(ETH_P_IPV6))
				s->key[1] |= PPE_ACL_IS_IPV6;
			s->mask[1] |= PPE_ACL_IS_IPV6;
		}
		if (match.mask->ip_proto) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPMISC);
			s->key[0] |= FIELD_PREP(PPE_ACL_L3_PROT,
						match.key->ip_proto);
			s->mask[0] |= FIELD_PREP(PPE_ACL_L3_PROT,
						 match.mask->ip_proto);
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IP)) {
		struct flow_match_ip match;

		flow_rule_match_ip(rule, &match);
		if (match.mask->ttl) {
			NL_SET_ERR_MSG_MOD(extack, "the TTL field is two encoded bits, not a value");
			return -EOPNOTSUPP;
		}
		if (match.mask->tos) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPMISC);
			s->key[0] |= FIELD_PREP(PPE_ACL_L3_DSCP,
						match.key->tos);
			s->mask[0] |= FIELD_PREP(PPE_ACL_L3_DSCP,
						 match.mask->tos);
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_TCP)) {
		struct flow_match_tcp match;
		u16 flags, mask;

		flow_rule_match_tcp(rule, &match);
		flags = ntohs(match.key->flags);
		mask = ntohs(match.mask->flags);
		if (mask & ~PPE_ACL_TCP_FLAGS_MAX) {
			NL_SET_ERR_MSG_MOD(extack, "the classifier stops at the URG flag");
			return -EOPNOTSUPP;
		}
		if (mask) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPMISC);
			s->key[1] |= FIELD_PREP(PPE_ACL_TCP_FLAGS, flags);
			s->mask[1] |= FIELD_PREP(PPE_ACL_TCP_FLAGS, mask);
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct flow_match_eth_addrs match;
		u32 kw[2], mw[2];

		flow_rule_match_eth_addrs(rule, &match);
		if (!is_zero_ether_addr(match.mask->dst)) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_MAC_DA);
			ppe_acl_mac_words(kw, match.key->dst);
			ppe_acl_mac_words(mw, match.mask->dst);
			s->key[0] |= kw[0];
			s->key[1] |= kw[1];
			s->mask[0] |= mw[0];
			s->mask[1] |= mw[1];
		}
		if (!is_zero_ether_addr(match.mask->src)) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_MAC_SA);
			ppe_acl_mac_words(kw, match.key->src);
			ppe_acl_mac_words(mw, match.mask->src);
			s->key[0] |= kw[0];
			s->key[1] |= kw[1];
			s->mask[0] |= mw[0];
			s->mask[1] |= mw[1];
		}
	}

	if (proto == htons(ETH_P_IPV6)) {
		sip_type = PPE_ACL_TYPE_IPV6_SIP0 + 2;
		dip_type = PPE_ACL_TYPE_IPV6_DIP0 + 2;
	} else {
		sip_type = PPE_ACL_TYPE_IPV4_SIP;
		dip_type = PPE_ACL_TYPE_IPV4_DIP;
	}

	/* Which of the two address keys carries the filter's addresses is the
	 * control key's to say. cls_flower keeps both families in one union
	 * and registers a key for either whenever the bytes it covers are
	 * masked, so a family chosen by the dissector alone reads the other
	 * one's addresses out of the same memory, and the entries that come
	 * back match no frame of either family.
	 */
	if (addr_type == FLOW_DISSECTOR_KEY_IPV4_ADDRS &&
	    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS)) {
		struct flow_match_ipv4_addrs match;

		flow_rule_match_ipv4_addrs(rule, &match);
		if (match.mask->src) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPV4_SIP);
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_LO,
						ntohl(match.key->src));
			s->key[1] |= FIELD_PREP(PPE_ACL_IP_HI,
						ntohl(match.key->src) >> 16);
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_LO,
						 ntohl(match.mask->src));
			s->mask[1] |= FIELD_PREP(PPE_ACL_IP_HI,
						 ntohl(match.mask->src) >> 16);
		}
		if (match.mask->dst) {
			s = ppe_acl_slice_get(slice, &n, PPE_ACL_TYPE_IPV4_DIP);
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_LO,
						ntohl(match.key->dst));
			s->key[1] |= FIELD_PREP(PPE_ACL_IP_HI,
						ntohl(match.key->dst) >> 16);
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_LO,
						 ntohl(match.mask->dst));
			s->mask[1] |= FIELD_PREP(PPE_ACL_IP_HI,
						 ntohl(match.mask->dst) >> 16);
		}
	}

	if (addr_type == FLOW_DISSECTOR_KEY_IPV6_ADDRS &&
	    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS)) {
		struct flow_match_ipv6_addrs match;

		flow_rule_match_ipv6_addrs(rule, &match);
		if (!ipv6_addr_any(&match.mask->src))
			ppe_acl_key_ip6(slice, &n, PPE_ACL_TYPE_IPV6_SIP0,
					&match.key->src, &match.mask->src);
		if (!ipv6_addr_any(&match.mask->dst))
			ppe_acl_key_ip6(slice, &n, PPE_ACL_TYPE_IPV6_DIP0,
					&match.key->dst, &match.mask->dst);
	}

	/* The port and the ICMP type/code share one field of the address
	 * entry, which is why a filter may not ask for both.
	 */
	if ((flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS) ||
	     flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS_RANGE)) &&
	    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ICMP)) {
		NL_SET_ERR_MSG_MOD(extack, "ports and ICMP share one field");
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS)) {
		struct flow_match_ports match;

		if (!proto) {
			NL_SET_ERR_MSG_MOD(extack, "a port match needs the IP version named");
			return -EOPNOTSUPP;
		}
		flow_rule_match_ports(rule, &match);
		if (match.mask->src) {
			s = ppe_acl_slice_get(slice, &n, sip_type);
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						ntohs(match.key->src));
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						 ntohs(match.mask->src));
		}
		if (match.mask->dst) {
			s = ppe_acl_slice_get(slice, &n, dip_type);
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						ntohs(match.key->dst));
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						 ntohs(match.mask->dst));
		}
	}

	/* A range compare turns the same field's mask slot into the maximum,
	 * so the entry carrying it has no mask left for the port and cannot
	 * also match one exactly. The address the entry carries keeps its own
	 * mask - only the field the range names is repurposed - and the
	 * allocator gives a ranged entry an even slot.
	 */
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS_RANGE)) {
		struct flow_match_ports_range match;

		if (!proto) {
			NL_SET_ERR_MSG_MOD(extack, "a port match needs the IP version named");
			return -EOPNOTSUPP;
		}
		flow_rule_match_ports_range(rule, &match);
		if (match.mask->tp_min.src) {
			s = ppe_acl_slice_get(slice, &n, sip_type);
			if (s->mask[0] & PPE_ACL_IP_PORT)
				goto both;
			s->range = true;
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						ntohs(match.key->tp_min.src));
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						 ntohs(match.key->tp_max.src));
		}
		if (match.mask->tp_min.dst) {
			s = ppe_acl_slice_get(slice, &n, dip_type);
			if (s->mask[0] & PPE_ACL_IP_PORT)
				goto both;
			s->range = true;
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						ntohs(match.key->tp_min.dst));
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						 ntohs(match.key->tp_max.dst));
		}
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_ICMP)) {
		struct flow_match_icmp match;

		if (!proto) {
			NL_SET_ERR_MSG_MOD(extack, "an ICMP match needs the IP version named");
			return -EOPNOTSUPP;
		}
		flow_rule_match_icmp(rule, &match);
		if (match.mask->type || match.mask->code) {
			s = ppe_acl_slice_get(slice, &n, dip_type);
			s->key[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						match.key->type << 8 |
						match.key->code);
			s->mask[0] |= FIELD_PREP(PPE_ACL_IP_PORT,
						 match.mask->type << 8 |
						 match.mask->code);
		}
	}

	*family = proto;

	return n;

both:
	NL_SET_ERR_MSG_MOD(extack, "a port is matched exactly or as a range, not both");

	return -EOPNOTSUPP;
}

/* One of the 512 ingress meters. The rate maths is the port policer's - same
 * block, same refresh period - and metering mode 1 is RFC 2697: one rate, one
 * bucket, everything the committed burst cannot hold is red. The two words
 * past the rate leave the excess bucket empty and the violate command at zero,
 * which is drop, so red means "over the rate" and is dropped.
 */
static int ppe_acl_meter_set(struct qca_ppe_priv *priv, u32 index,
			     u64 rate_bps, u32 burst)
{
	u32 cir = 0, cbs = 0;
	unsigned long clk;
	int sel = 0;

	if (rate_bps) {
		clk = ppe_clk_rate(priv);
		if (!clk)
			return -ENODEV;

		sel = ppe_token_bucket(clk, PPE_POLICER_SLOT, rate_bps, burst,
				       FIELD_MAX(PPE_ACL_METER_CIR_HI) << 8 |
				       FIELD_MAX(PPE_ACL_METER_CIR_LO),
				       FIELD_MAX(PPE_ACL_METER_CBS),
				       &cir, &cbs);
		if (sel < 0)
			return sel;
	}

	/* Four words, latching on the last, like every other PPE table. */
	regmap_write(priv->regmap, PPE_ACL_METER(index),
		     (rate_bps ? PPE_ACL_METER_EN : 0) |
		     PPE_ACL_METER_MODE |
		     FIELD_PREP(PPE_ACL_METER_TOKEN_UNIT, sel) |
		     FIELD_PREP(PPE_ACL_METER_CBS, cbs) |
		     FIELD_PREP(PPE_ACL_METER_CIR_LO, cir));
	regmap_write(priv->regmap, PPE_ACL_METER(index) + 0x4,
		     FIELD_PREP(PPE_ACL_METER_CIR_HI, cir >> 8));
	regmap_write(priv->regmap, PPE_ACL_METER(index) + 0x8, 0);
	regmap_write(priv->regmap, PPE_ACL_METER(index) + 0xc, 0);

	/* A meter index outlives the rule that held it, so the bucket goes back
	 * with it: tokens the old rule bought at its own rate would otherwise
	 * be the first thing the next rule to take this index spends.
	 */
	if (!rate_bps) {
		regmap_write(priv->regmap, PPE_ACL_METER_CRDT(index), 0);
		regmap_write(priv->regmap, PPE_ACL_METER_CRDT(index) + 0x4, 0);
	}

	return 0;
}

/* The resources a rule holds outside its own entries. */
static void ppe_acl_rule_free(struct qca_ppe_priv *priv,
			      struct ppe_acl_rule *r)
{
	if (r->mirror)
		ppe_mirror_analyzer_put(priv);
	if (r->meter >= 0) {
		ppe_acl_meter_set(priv, r->meter, 0, 0);
		clear_bit(r->meter, priv->acl_meter_used);
	}
	kfree(r->fs);
	kfree(r);
}

static int ppe_acl_parse_action(struct qca_ppe_priv *priv,
				struct flow_rule *rule,
				struct netlink_ext_ack *extack, __be16 family,
				struct ppe_acl_rule *r)
{
	const struct flow_action_entry *a;
	u32 *act = r->act;
	bool fwd = false;
	int i, ret;

	flow_action_for_each(i, a, &rule->action) {
		switch (a->id) {
		case FLOW_ACTION_ACCEPT:
		case FLOW_ACTION_DROP:
		case FLOW_ACTION_TRAP:
			if (fwd) {
				NL_SET_ERR_MSG_MOD(extack, "one forward command per rule");
				return -EOPNOTSUPP;
			}
			fwd = true;
			act[0] |= PPE_ACL_DEST_CHANGE_EN |
				  FIELD_PREP(PPE_ACL_FWD_CMD,
					     a->id == FLOW_ACTION_DROP ?
					     PPE_ACL_FWD_DROP :
					     a->id == FLOW_ACTION_TRAP ?
					     PPE_ACL_FWD_RDT_CPU :
					     PPE_ACL_FWD_FORWARD);
			break;
		case FLOW_ACTION_REDIRECT: {
			struct dsa_port *to = dsa_port_from_netdev(a->dev);

			if (IS_ERR(to) || to->ds != &priv->ds) {
				NL_SET_ERR_MSG_MOD(extack, "the destination has to name a port of this switch");
				return -EOPNOTSUPP;
			}
			if (fwd) {
				NL_SET_ERR_MSG_MOD(extack, "one forward command per rule");
				return -EOPNOTSUPP;
			}
			fwd = true;
			/* The forward command stays at pass: what changes is
			 * the destination it passes the frame to.
			 */
			act[0] |= PPE_ACL_DEST_CHANGE_EN |
				  FIELD_PREP(PPE_ACL_DEST_TYPE,
					     PPE_ACL_DEST_PORT_BMP) |
				  FIELD_PREP(PPE_ACL_DEST_VALUE,
					     BIT(to->index));
			break;
		}
		case FLOW_ACTION_PRIORITY:
			if (a->priority > FIELD_MAX(PPE_ACL_PRI)) {
				NL_SET_ERR_MSG_MOD(extack, "internal priority is four bits");
				return -EOPNOTSUPP;
			}
			act[3] |= PPE_ACL_PRI_CHANGE_EN |
				  FIELD_PREP(PPE_ACL_PRI, a->priority);
			break;
		case FLOW_ACTION_QUEUE:
			/* The index is a hardware queue of the whole switch:
			 * which port it drains is the queue's business, not
			 * the classifier's.
			 */
			if (a->queue.index > FIELD_MAX(PPE_ACL_QID)) {
				NL_SET_ERR_MSG_MOD(extack, "no such hardware queue");
				return -EOPNOTSUPP;
			}
			act[3] |= PPE_ACL_QID_EN |
				  FIELD_PREP(PPE_ACL_QID, a->queue.index);
			break;
		case FLOW_ACTION_MANGLE: {
			enum flow_action_mangle_base htype = a->mangle.htype;
			u32 mask = ntohl(a->mangle.mask);
			u32 val = ntohl(a->mangle.val);
			int shift;

			/* Where the byte sits in the header's first word,
			 * and which family that header is: pedit resolves
			 * IPv4, IPv6 and unspecified alike to the network
			 * header, so only the filter's own protocol tells the
			 * two apart, and the engine rewrites per family.
			 */
			if (family == htons(ETH_P_IP) &&
			    htype == FLOW_ACT_MANGLE_HDR_TYPE_IP4) {
				shift = 16;
			} else if (family == htons(ETH_P_IPV6) &&
				   htype == FLOW_ACT_MANGLE_HDR_TYPE_IP6) {
				shift = 20;
			} else {
				NL_SET_ERR_MSG_MOD(extack, "only the traffic class of the IP version the filter names is rewritten");
				return -EOPNOTSUPP;
			}
			/* The field has no mask on this generation, so the
			 * rewrite has to be the whole byte and nothing else -
			 * and what the kernel keeps of the word is what the
			 * value is xored into, so it may not reach outside.
			 */
			if (a->mangle.offset || ~mask != (u32)0xff << shift ||
			    val & mask) {
				NL_SET_ERR_MSG_MOD(extack, "the traffic class is rewritten whole or not at all");
				return -EOPNOTSUPP;
			}
			act[2] |= PPE_ACL_DSCP_TC_CHANGE_EN |
				  FIELD_PREP(PPE_ACL_DSCP_TC, val >> shift);
			break;
		}
		case FLOW_ACTION_POLICE: {
			unsigned long index;

			/* One byte rate whose excess is dropped: the meter
			 * has no second bucket to hand a peak rate to, counts
			 * bytes rather than packets, compensates frame length
			 * by the block's own constant rather than by a per
			 * frame overhead, and the colour a lesser exceed
			 * action would set is read by nothing here.
			 */
			if (!a->police.rate_bytes_ps ||
			    a->police.peakrate_bytes_ps || a->police.avrate ||
			    a->police.rate_pkt_ps || a->police.overhead ||
			    a->police.exceed.act_id != FLOW_ACTION_DROP ||
			    (a->police.notexceed.act_id != FLOW_ACTION_ACCEPT &&
			     a->police.notexceed.act_id != FLOW_ACTION_PIPE)) {
				NL_SET_ERR_MSG_MOD(extack, "the meter is one byte rate and drops what exceeds it");
				return -EOPNOTSUPP;
			}
			/* Accepting what conforms ends the filter, so a later
			 * action would run in the hardware and nowhere else.
			 */
			if (a->police.notexceed.act_id == FLOW_ACTION_ACCEPT &&
			    !flow_action_is_last_entry(&rule->action, a)) {
				NL_SET_ERR_MSG_MOD(extack, "nothing may follow an action that accepts what conforms");
				return -EOPNOTSUPP;
			}
			if (r->meter >= 0) {
				NL_SET_ERR_MSG_MOD(extack, "one meter per rule");
				return -EOPNOTSUPP;
			}
			index = find_first_zero_bit(priv->acl_meter_used,
						    PPE_ACL_METER_ENTRIES);
			if (index >= PPE_ACL_METER_ENTRIES) {
				NL_SET_ERR_MSG_MOD(extack, "every meter is taken");
				return -ENOSPC;
			}
			ret = ppe_acl_meter_set(priv, index,
						a->police.rate_bytes_ps *
						BITS_PER_BYTE,
						a->police.burst);
			if (ret) {
				NL_SET_ERR_MSG_MOD(extack, "the rate and burst are outside the meter's range");
				return ret;
			}
			set_bit(index, priv->acl_meter_used);
			r->meter = index;
			act[3] |= PPE_ACL_POLICER_EN |
				  FIELD_PREP(PPE_ACL_POLICER_INDEX, index);
			break;
		}
		case FLOW_ACTION_MIRRED: {
			struct dsa_port *to = dsa_port_from_netdev(a->dev);

			if (IS_ERR(to) || to->ds != &priv->ds) {
				NL_SET_ERR_MSG_MOD(extack, "the mirror has to name a port of this switch");
				return -EOPNOTSUPP;
			}
			if (r->mirror) {
				NL_SET_ERR_MSG_MOD(extack, "one mirror per rule");
				return -EOPNOTSUPP;
			}
			ret = ppe_mirror_analyzer_get(priv, to->index);
			if (ret) {
				NL_SET_ERR_MSG_MOD(extack, "another port is already mirrored elsewhere");
				return ret;
			}
			r->mirror = true;
			act[0] |= PPE_ACL_MIRROR_EN;
			break;
		}
		default:
			NL_SET_ERR_MSG_MOD(extack, "action the classifier cannot take");
			return -EOPNOTSUPP;
		}
	}

	return 0;
}

static struct ppe_acl_rule *ppe_acl_rule_find(struct qca_ppe_priv *priv,
					      unsigned long cookie, int port)
{
	struct ppe_acl_rule *r;

	list_for_each_entry(r, &priv->acl_rules, list)
		if (r->cookie == cookie && r->port == port)
			return r;

	return NULL;
}

/* Place one parsed rule in the engine. Both uAPIs land here: the preference
 * is tc's for a filter and the location for an ethtool entry, and in each the
 * lower number is the stronger rule, which the engine expresses as the higher
 * priority.
 */
static int ppe_acl_rule_add(struct qca_ppe_priv *priv, int port,
			    struct flow_rule *rule, unsigned long cookie,
			    int loc, const struct ethtool_rx_flow_spec *fs,
			    u16 prio, struct netlink_ext_ack *extack)
{
	struct ppe_acl_slice slice[PPE_ACL_MAX_SLICES] = {};
	struct ppe_acl_rule *r;
	int nslices, ret, i;
	__be16 family;
	u16 pri;

	nslices = ppe_acl_parse_key(rule, extack, slice, &family);
	if (nslices < 0)
		return nslices;
	if (!nslices) {
		NL_SET_ERR_MSG_MOD(extack, "a rule with no key would match every frame");
		return -EOPNOTSUPP;
	}
	if (nslices > PPE_ACL_LIST_ENTRIES) {
		NL_SET_ERR_MSG_MOD(extack, "the key needs more entries than one list holds");
		return -EOPNOTSUPP;
	}

	r = kzalloc(sizeof(*r), GFP_KERNEL);
	if (!r)
		return -ENOMEM;
	r->meter = -1;
	r->loc = loc;
	if (fs) {
		r->fs = kmemdup(fs, sizeof(*fs), GFP_KERNEL);
		if (!r->fs) {
			kfree(r);
			return -ENOMEM;
		}
	}

	/* The meter index comes out of the same lock the entries do: the
	 * small-packet parameters reach the table without rtnl, and so does
	 * ethtool's n-tuple insert.
	 */
	mutex_lock(&priv->acl_lock);
	ret = ppe_acl_parse_action(priv, rule, extack, family, r);
	if (ret)
		goto err;

	ret = ppe_acl_alloc(priv, slice, nslices, &r->group);
	if (ret) {
		NL_SET_ERR_MSG_MOD(extack, "no room left in the classifier");
		goto err;
	}

	/* Every entry of one rule carries the rule's priority. */
	pri = FIELD_MAX(PPE_ACL_RULE_PRI) - prio;
	for (i = 0; i < nslices; i++)
		ppe_acl_slice_write(priv, r->group.index[i], &slice[i], r->act,
				    BIT(port), pri);

	r->cookie = cookie;
	r->port = port;
	list_add_tail(&r->list, &priv->acl_rules);
	mutex_unlock(&priv->acl_lock);

	return 0;

err:
	ppe_acl_rule_free(priv, r);
	mutex_unlock(&priv->acl_lock);

	return ret;
}

int qca_ppe_cls_flower_add(struct dsa_switch *ds, int port,
			   struct flow_cls_offload *cls, bool ingress)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(cls);
	struct netlink_ext_ack *extack = cls->common.extack;
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	if (!ingress) {
		NL_SET_ERR_MSG_MOD(extack, "the classifier only sees ingress");
		return -EOPNOTSUPP;
	}
	if (cls->common.chain_index) {
		NL_SET_ERR_MSG_MOD(extack, "only chain 0 reaches the classifier");
		return -EOPNOTSUPP;
	}
	/* The engine matches the highest priority it holds, where tc gives
	 * precedence to the lowest preference, so the rule's standing is the
	 * field's span less the preference. Nine bits carry it, and a
	 * preference past that is refused rather than tied to another rule's:
	 * the filter tc reports as offloaded has to be the one that wins.
	 */
	if (cls->common.prio > FIELD_MAX(PPE_ACL_RULE_PRI)) {
		NL_SET_ERR_MSG_MOD(extack, "filter preference is above what the classifier can order; use one below 512");
		return -EOPNOTSUPP;
	}

	return ppe_acl_rule_add(priv, port, rule, cls->cookie, -1, NULL,
				cls->common.prio, extack);
}

int qca_ppe_cls_flower_del(struct dsa_switch *ds, int port,
			   struct flow_cls_offload *cls, bool ingress)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct ppe_acl_rule *r;

	mutex_lock(&priv->acl_lock);
	r = ppe_acl_rule_find(priv, cls->cookie, port);
	if (r) {
		ppe_acl_free(priv, &r->group);
		list_del(&r->list);
		ppe_acl_rule_free(priv, r);
	}
	mutex_unlock(&priv->acl_lock);

	return 0;
}

/* Small packets jump the bulk queue. One rule per IP family classifies every
 * frame whose L3 length is at most small_pkt_len into internal priority
 * small_pkt_prio, whose queue the scheduler serves past a standing bulk queue
 * on a shaped port. Length is the one property the silicon can read that
 * separates acks, handshakes, DNS, VoIP and game traffic from full-size bulk
 * without any marking on the packet; the deep queue a shaper wants for
 * throughput then costs its latency only to the bulk traffic inside it.
 * The default takes the 200-byte G.711 voice frame, the largest of the
 * common voice codecs on the wire, with room for its tunnelled forms;
 * every bulk protocol's data rides at the MTU.
 *
 * The priority is capped at 7: the slots above sit on the port's second SP
 * beside the multicast queues.
 */
static ushort ppe_small_pkt_len = 256;
static ushort ppe_small_pkt_prio = 5;

/* One PPE per SoC; the parameter store needs the instance back. */
static struct qca_ppe_priv *ppe_acl_priv;

static struct ppe_acl_group ppe_small_pkt_group[2];

/* ethtool's n-tuple table is the second way into the same engine, and the only
 * one that reaches the queue action: its ring cookie names a queue of the whole
 * switch, where tc can ask for a priority class and leave the hash to pick
 * which of that class's queues the flow lands in.
 */
static struct ppe_acl_rule *ppe_acl_rule_at(struct qca_ppe_priv *priv,
					    int port, u32 loc)
{
	struct ppe_acl_rule *r;

	lockdep_assert_held(&priv->acl_lock);

	/* Only a rule ethtool inserted has a location. Without the sign test
	 * the -1 of a tc rule promotes to 0xffffffff and a delete at that
	 * location takes the tc rule's hardware entry away under it.
	 */
	list_for_each_entry(r, &priv->acl_rules, list)
		if (r->port == port && r->loc >= 0 && r->loc == loc)
			return r;

	return NULL;
}

static int ppe_acl_rxnfc_ins(struct qca_ppe_priv *priv, int port,
			     struct ethtool_rx_flow_spec *fs)
{
	struct ethtool_rx_flow_spec_input input = { .fs = fs };
	struct ethtool_rx_flow_rule *flow;
	bool taken;
	int ret;

	/* The location names the rule and orders it, so it shares the span the
	 * priority field has.
	 */
	if (fs->location > FIELD_MAX(PPE_ACL_RULE_PRI))
		return -EINVAL;

	mutex_lock(&priv->acl_lock);
	taken = ppe_acl_rule_at(priv, port, fs->location);
	mutex_unlock(&priv->acl_lock);
	if (taken)
		return -EEXIST;

	flow = ethtool_rx_flow_rule_create(&input);
	if (IS_ERR(flow))
		return PTR_ERR(flow);

	ret = ppe_acl_rule_add(priv, port, flow->rule, 0, fs->location, fs,
			       fs->location, NULL);
	ethtool_rx_flow_rule_destroy(flow);

	return ret;
}

int qca_ppe_set_rxnfc(struct dsa_switch *ds, int port,
		      struct ethtool_rxnfc *nfc)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct ppe_acl_rule *r;

	if (nfc->cmd == ETHTOOL_SRXCLSRLINS)
		return ppe_acl_rxnfc_ins(priv, port, &nfc->fs);
	if (nfc->cmd != ETHTOOL_SRXCLSRLDEL)
		return -EOPNOTSUPP;

	guard(mutex)(&priv->acl_lock);

	r = ppe_acl_rule_at(priv, port, nfc->fs.location);
	if (!r)
		return -ENOENT;

	ppe_acl_free(priv, &r->group);
	list_del(&r->list);
	ppe_acl_rule_free(priv, r);

	return 0;
}

int qca_ppe_get_rxnfc(struct dsa_switch *ds, int port,
		      struct ethtool_rxnfc *nfc, u32 *rule_locs)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct ppe_acl_rule *r;
	u32 cnt = 0;

	guard(mutex)(&priv->acl_lock);

	switch (nfc->cmd) {
	case ETHTOOL_GRXRINGS:
		/* What the action may name: a queue of the whole switch, not a
		 * receive ring of this port.
		 */
		nfc->data = FIELD_MAX(PPE_ACL_QID) + 1;
		return 0;
	case ETHTOOL_GRXCLSRLCNT:
		list_for_each_entry(r, &priv->acl_rules, list)
			if (r->port == port && r->loc >= 0)
				cnt++;
		nfc->rule_cnt = cnt;
		nfc->data = FIELD_MAX(PPE_ACL_RULE_PRI) + 1;
		return 0;
	case ETHTOOL_GRXCLSRULE:
		r = ppe_acl_rule_at(priv, port, nfc->fs.location);
		if (!r || !r->fs)
			return -ENOENT;

		nfc->fs = *r->fs;
		return 0;
	case ETHTOOL_GRXCLSRLALL:
		list_for_each_entry(r, &priv->acl_rules, list) {
			if (r->port != port || r->loc < 0)
				continue;
			if (cnt >= nfc->rule_cnt)
				return -EMSGSIZE;

			rule_locs[cnt++] = r->loc;
		}
		nfc->rule_cnt = cnt;
		nfc->data = FIELD_MAX(PPE_ACL_RULE_PRI) + 1;
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static void ppe_acl_small_pkt_apply(struct qca_ppe_priv *priv)
{
	u32 ports = GENMASK(priv->data->num_ports - 1, 0);
	u32 act[PPE_ACL_ACTION_WORDS] = {};
	struct ppe_acl_slice s = {};
	int i;

	act[3] = PPE_ACL_PRI_CHANGE_EN |
		 FIELD_PREP(PPE_ACL_PRI, ppe_small_pkt_prio);

	s.type = PPE_ACL_TYPE_IPMISC;
	s.range = true;
	/* Range compare: minimum in the rule, maximum in the mask slot. */
	s.mask[0] = FIELD_PREP(PPE_ACL_L3_LEN, ppe_small_pkt_len);
	s.mask[1] = PPE_ACL_IS_IPV6;

	/* Length 0 disarms by handing the rule an empty source bitmap, which
	 * keeps its entry so the parameter can be turned back on.
	 *
	 * Priority 1, not the floor: a tc filter at the last preference
	 * lands on priority 0, which is the one place a rule that has to
	 * lose to this promotion for a small frame can stand.
	 */
	for (i = 0; i < ARRAY_SIZE(ppe_small_pkt_group); i++) {
		struct ppe_acl_group *g = &ppe_small_pkt_group[i];

		if (!g->nslices && ppe_acl_alloc(priv, &s, 1, g))
			continue;

		s.key[1] = i ? PPE_ACL_IS_IPV6 : 0;
		ppe_acl_slice_write(priv, g->index[0], &s, act,
				    ppe_small_pkt_len ? ports : 0, 1);
	}
}

static int ppe_small_pkt_param_set(const char *val,
				   const struct kernel_param *kp)
{
	struct qca_ppe_priv *priv = ppe_acl_priv;
	int ret = param_set_ushort(val, kp);

	if (ret)
		return ret;
	if (ppe_small_pkt_prio > 7)
		ppe_small_pkt_prio = 7;
	if (!priv)
		return 0;

	mutex_lock(&priv->acl_lock);
	ppe_acl_small_pkt_apply(priv);
	mutex_unlock(&priv->acl_lock);

	return 0;
}

static const struct kernel_param_ops ppe_small_pkt_param_ops = {
	.set = ppe_small_pkt_param_set,
	.get = param_get_ushort,
};

module_param_cb(small_pkt_len, &ppe_small_pkt_param_ops,
		&ppe_small_pkt_len, 0644);
MODULE_PARM_DESC(small_pkt_len,
		 "Classify frames up to this L3 length into small_pkt_prio (0 disables)");
module_param_cb(small_pkt_prio, &ppe_small_pkt_param_ops,
		&ppe_small_pkt_prio, 0644);
MODULE_PARM_DESC(small_pkt_prio,
		 "Internal priority for small frames (0-7)");

void ppe_acl_init(struct qca_ppe_priv *priv)
{
	int i;

	mutex_init(&priv->acl_lock);
	INIT_LIST_HEAD(&priv->acl_rules);
	for (i = 0; i < PPE_ACL_LISTS; i++)
		priv->acl_free[i] = GENMASK(PPE_ACL_LIST_ENTRIES - 1, 0);

	mutex_lock(&priv->acl_lock);
	ppe_acl_priv = priv;
	ppe_acl_small_pkt_apply(priv);
	mutex_unlock(&priv->acl_lock);
}

void ppe_acl_exit(struct qca_ppe_priv *priv)
{
	struct ppe_acl_rule *r, *tmp;
	int i;

	/* The parameter writer finds this driver through the global and then
	 * takes the lock that lives inside it, so the lock cannot be what
	 * guards the pointer. param_attr_store() holds the parameter lock
	 * across the whole set, so clearing the global under it lets any
	 * writer already inside finish before this teardown frees what it is
	 * writing through.
	 */
	kernel_param_lock(THIS_MODULE);
	ppe_acl_priv = NULL;
	kernel_param_unlock(THIS_MODULE);

	mutex_lock(&priv->acl_lock);
	list_for_each_entry_safe(r, tmp, &priv->acl_rules, list) {
		ppe_acl_free(priv, &r->group);
		list_del(&r->list);
		ppe_acl_rule_free(priv, r);
	}
	for (i = 0; i < ARRAY_SIZE(ppe_small_pkt_group); i++)
		if (ppe_small_pkt_group[i].nslices)
			ppe_acl_free(priv, &ppe_small_pkt_group[i]);
	mutex_unlock(&priv->acl_lock);
}
