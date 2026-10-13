// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s Ethernet switch: hardware flow offload
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/bitfield.h>
#include <linux/etherdevice.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/ip.h>
#include <linux/rhashtable.h>
#include <linux/slab.h>
#include <net/flow_offload.h>

#include "zx279128s-eth.h"

/* ---- Hardware flow offload ------------------------------------------ */

/*
 * The packet processor forwards established IPv4 TCP/UDP flows without the
 * CPU: the classifier looks the 5-tuple up in two hash banks, the packet
 * modifier rewrites MACs, addresses, ports and TTL, and the entry sends
 * the frame straight to the egress port.  A miss goes to the CPU as usual.
 * Formats reverse engineered from the stock firmware's NPU tables.
 *
 * The WAN is a plain switch port here, so the hardware treats both
 * directions of a connection as upstream: both use the upstream key
 * header, and each direction has its own flow (rewrite) index.
 */

struct zx_ppe_entry {
	struct rhash_head	node;
	unsigned long		cookie;
	u16			hash;	/* 0-255 bank 0, 256-383 bank 1 */
	u16			flow;
	u16			nh;
	u8			subnet;
};

const struct rhashtable_params zx_ppe_ht_params = {
	.head_offset		= offsetof(struct zx_ppe_entry, node),
	.key_offset		= offsetof(struct zx_ppe_entry, cookie),
	.key_len		= sizeof(unsigned long),
	.automatic_shrinking	= true,
};

static u32 zx_pp_data_reg(u32 blk, int i)
{
	if (blk == ZX_PM_BASE && i >= 4)
		return ZX_PM_IND_DATA_HI + (i - 4) * 4;
	return ZX_PP_IND_DATA + i * 4;
}

static int zx_pp_wait(struct zx_eth_adapter *adapter, u32 blk)
{
	u32 val;

	return readl_poll_timeout(adapter->base + blk + ZX_PP_IND_STATUS, val,
				  val & 1, 0, 1000);
}

/* Write @n data words of an entry; the hardware takes them when word 0 is
 * written, so it goes last (as in the stock driver).
 */
static int zx_pp_write(struct zx_eth_adapter *adapter, u32 blk, u32 table,
		       u32 index, const u32 *data, int n)
{
	int words = blk == ZX_CLA_BASE ? 17 : 8;
	int i, ret;

	ret = zx_pp_wait(adapter, blk);
	if (ret)
		return ret;
	writel(index | table << 22, adapter->base + blk + ZX_PP_IND_CMD);
	for (i = words - 1; i >= 0; i--)
		writel(i < n ? data[i] : 0, adapter->base + blk + zx_pp_data_reg(blk, i));
	/* the status can still read idle right after the command */
	for (i = 0; i < 64 && (readl(adapter->base + blk + ZX_PP_IND_STATUS) & 1); i++)
		;
	return zx_pp_wait(adapter, blk);
}

static int zx_pp_read_word0(struct zx_eth_adapter *adapter, u32 blk, u32 table,
			    u32 index, u32 *val)
{
	int i, ret;

	ret = zx_pp_wait(adapter, blk);
	if (ret)
		return ret;
	writel(index | table << 22 | ZX_PP_IND_READ, adapter->base + blk + ZX_PP_IND_CMD);
	for (i = 0; i < 64 && (readl(adapter->base + blk + ZX_PP_IND_STATUS) & 1); i++)
		;
	ret = zx_pp_wait(adapter, blk);
	if (!ret)
		*val = readl(adapter->base + blk + ZX_PP_IND_DATA);
	return ret;
}

/* MAC in the layout of the switch's MAC registers: bytes 2-5, then 0-1 */
static void zx_mac_words(const u8 *mac, u32 *w)
{
	w[0] = mac[2] << 24 | mac[3] << 16 | mac[4] << 8 | mac[5];
	w[1] = mac[0] << 8 | mac[1];
}

/* Set @width bits at bit @pos of a little-endian bit string */
static void zx_ppe_put_bits(u8 *buf, int pos, int width, u32 val)
{
	int i;

	for (i = 0; i < width; i++, pos++)
		if (val & BIT(i))
			buf[pos / 8] |= BIT(pos % 8);
}

/* MSB-first CRC over the key, last byte first, no init or final xor */
static u32 zx_ppe_crc(u32 poly, const u8 *key, int len)
{
	u32 crc = 0;
	int i, b;

	for (i = len - 1; i >= 0; i--) {
		crc ^= key[i] << 24;
		for (b = 0; b < 8; b++)
			crc = crc & BIT(31) ? crc << 1 ^ poly : crc << 1;
	}
	return crc;
}

struct zx_ppe_tuple {
	u8	proto;
	__be32	saddr, daddr;
	__be16	sport, dport;
};

static void zx_ppe_halfwords(const struct zx_ppe_tuple *t, u16 hw[7])
{
	u32 s = be32_to_cpu(t->saddr), d = be32_to_cpu(t->daddr);

	hw[0] = t->proto;
	hw[1] = s >> 16;
	hw[2] = s & 0xffff;
	hw[3] = d >> 16;
	hw[4] = d & 0xffff;
	hw[5] = be16_to_cpu(t->sport);
	hw[6] = be16_to_cpu(t->dport);
}

/* Hash slot of a tuple in bank 0 (0-255) and bank 1 (256-383).  The key is
 * 45 bytes: header (extract rule id at bit 23, direction bit 32 = 0) and
 * the extracted halfwords from bit 33.
 */
static void zx_ppe_slots(const struct zx_ppe_tuple *t, u16 slot[2])
{
	u8 key[45] = {};
	u16 hw[7];
	int i;

	zx_ppe_halfwords(t, hw);
	zx_ppe_put_bits(key, 23, 8, ZX_PPE_V4_RULE);
	for (i = 0; i < 7; i++)
		zx_ppe_put_bits(key, 33 + 16 * i, 16, hw[i]);
	slot[0] = zx_ppe_crc(0x04c11db7, key, sizeof(key)) & 0xff;
	slot[1] = ZX_PPE_HASH0_SIZE + (zx_ppe_crc(0x1edc6f41, key, sizeof(key)) & 0x7f);
}

/*
 * Classifier entry: rewrite index, egress port and forward flag, then the
 * key. Words 1-3 are the same constants in every entry of the stock
 * firmware, and so is the 0x44 in word 0.
 */
static void zx_ppe_hash_entry(const struct zx_ppe_tuple *t, u16 flow, int port,
			      u32 w[9])
{
	u8 key[20] = {};
	u16 hw[7];
	int i;

	zx_ppe_halfwords(t, hw);
	/* valid, extract index 9, upstream */
	zx_ppe_put_bits(key, 0, 8, 0x40 | ZX_PPE_V4_INDEX);
	for (i = 0; i < 7; i++)
		zx_ppe_put_bits(key, 24 + 16 * i, 16, hw[i]);
	w[0] = FIELD_PREP(ZX_CLA_W0_FLOW, flow) | ZX_CLA_W0_FWD |
	       FIELD_PREP(ZX_CLA_W0_EGRESS, ZX_BRPORT(port)) | ZX_CLA_W0_BITS;
	w[1] = 0xfa11c000;
	w[2] = 0x00000608;
	w[3] = 0x80000000;
	for (i = 0; i < 5; i++)
		w[4 + i] = get_unaligned_le32(key + 4 * i);
}

struct zx_ppe_rewrite {
	bool	sip, dip, sport, dport;
	__be16	new_sport, new_dport;
	u8	subnet;
	u16	nh;
};

/* Packet modifier flow entry (12 bytes, little-endian bit fields) */
static void zx_ppe_flow_entry(const struct zx_ppe_rewrite *r, u32 w[3])
{
	u8 b[12] = {};

	zx_ppe_put_bits(b, 0, 1, 1);			/* replace destination MAC */
	zx_ppe_put_bits(b, 1, 1, 1);			/* replace source MAC */
	if (r->dport)
		zx_ppe_put_bits(b, 2, 16, be16_to_cpu(r->new_dport));
	if (r->sport)
		zx_ppe_put_bits(b, 18, 16, be16_to_cpu(r->new_sport));
	zx_ppe_put_bits(b, 34, 1, 1);			/* decrement TTL */
	zx_ppe_put_bits(b, 35, 1, 1);			/* update L4 checksum */
	zx_ppe_put_bits(b, 36, 1, 1);			/* update IP checksum */
	zx_ppe_put_bits(b, 37, 1, r->dport);
	zx_ppe_put_bits(b, 38, 1, r->sport);
	zx_ppe_put_bits(b, 39, 1, r->dip);
	zx_ppe_put_bits(b, 40, 1, r->sip);
	zx_ppe_put_bits(b, 41, 4, r->subnet);
	zx_ppe_put_bits(b, 50, 9, r->nh);
	w[0] = get_unaligned_le32(b);
	w[1] = get_unaligned_le32(b + 4);
	w[2] = get_unaligned_le32(b + 8);
}

/* Subnet: source MAC and (for SNAT) source IP of rewritten frames */
static int zx_ppe_subnet_get(struct zx_eth_adapter *adapter, const u8 *mac,
			     __be32 ip)
{
	int i, free = -1;
	u32 w[2];

	for (i = 0; i < ZX_PPE_SUBNETS; i++) {
		if (adapter->ppe_subnet[i].refs &&
		    ether_addr_equal(adapter->ppe_subnet[i].mac, mac) &&
		    adapter->ppe_subnet[i].ip == ip) {
			adapter->ppe_subnet[i].refs++;
			return i;
		}
		if (!adapter->ppe_subnet[i].refs && free < 0)
			free = i;
	}
	if (free < 0)
		return -ENOSPC;

	zx_mac_words(mac, w);
	if (zx_pp_write(adapter, ZX_PM_BASE, ZX_PM_SRC_MAC, free, w, 2))
		return -ETIMEDOUT;
	writel(be32_to_cpu(ip), adapter->base + ZX_PP_SNAT_IP(free));
	ether_addr_copy(adapter->ppe_subnet[free].mac, mac);
	adapter->ppe_subnet[free].ip = ip;
	adapter->ppe_subnet[free].refs = 1;
	return free;
}

static void zx_ppe_subnet_put(struct zx_eth_adapter *adapter, int i)
{
	adapter->ppe_subnet[i].refs--;
}

static int zx_ppe_alloc(unsigned long *map, int size, int first)
{
	int i = find_next_zero_bit(map, size, first);

	if (i >= size)
		return -ENOSPC;
	set_bit(i, map);
	return i;
}

/*
 * Apply one of the MAC rewrites that nf_flow_table_offload.c builds: the
 * destination MAC as 4 bytes at offset 0 and 2 bytes at offset 4 (mask
 * 0xffff0000), the source MAC as 2 bytes at offset 4 (mask 0x0000ffff, value
 * in the upper half) and 4 bytes at offset 8. The masks and values are in
 * CPU order, so the byte positions below are those of a little-endian CPU,
 * which the zx279128s is.
 */
static void zx_ppe_mangle_eth(const struct flow_action_entry *act, void *eth)
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

static int zx_ppe_flow_add(struct zx_eth_adapter *adapter,
			   struct flow_cls_offload *f)
{
	struct flow_rule *rule = flow_cls_offload_flow_rule(f);
	struct flow_action_entry *act;
	struct zx_ppe_rewrite rw = {};
	struct zx_ppe_tuple t = {};
	struct net_device *odev = NULL;
	struct zx_ppe_entry *e;
	struct ethhdr eth = {};
	__be32 new_saddr, new_daddr;
	u16 slot[2];
	u32 w[17];
	int i, ret, port, subnet, flow, nh;

	if (rhashtable_lookup_fast(&adapter->ppe_flows, &f->cookie, zx_ppe_ht_params))
		return -EEXIST;

	/* the hardware only sees frames from our switch ports */
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_META)) {
		struct flow_match_meta match;

		flow_rule_match_meta(rule, &match);
		for (i = 0; i < ZX_NUM_PORTS; i++)
			if (adapter->ports[i] &&
			    adapter->ports[i]->ifindex == match.key->ingress_ifindex)
				break;
		if (i == ZX_NUM_PORTS)
			return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_CONTROL)) {
		struct flow_match_control match;

		flow_rule_match_control(rule, &match);
		if (match.key->addr_type != FLOW_DISSECTOR_KEY_IPV4_ADDRS)
			return -EOPNOTSUPP;
	} else {
		return -EOPNOTSUPP;
	}

	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_match_basic match;

		flow_rule_match_basic(rule, &match);
		if (match.key->n_proto != htons(ETH_P_IP))
			return -EOPNOTSUPP;
		t.proto = match.key->ip_proto;
	}
	if (t.proto != IPPROTO_TCP && t.proto != IPPROTO_UDP)
		return -EOPNOTSUPP;

	if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS) ||
	    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_PORTS))
		return -EOPNOTSUPP;
	{
		struct flow_match_ipv4_addrs addrs;
		struct flow_match_ports ports;

		flow_rule_match_ipv4_addrs(rule, &addrs);
		flow_rule_match_ports(rule, &ports);
		t.saddr = addrs.key->src;
		t.daddr = addrs.key->dst;
		t.sport = ports.key->src;
		t.dport = ports.key->dst;
	}
	new_saddr = t.saddr;
	new_daddr = t.daddr;
	rw.new_sport = t.sport;
	rw.new_dport = t.dport;

	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			switch (act->mangle.htype) {
			case FLOW_ACT_MANGLE_HDR_TYPE_ETH:
				zx_ppe_mangle_eth(act, &eth);
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_IP4:
				/* the value holds the address in network order */
				if (act->mangle.offset == offsetof(struct iphdr, saddr))
					new_saddr = (__force __be32)act->mangle.val;
				else if (act->mangle.offset == offsetof(struct iphdr, daddr))
					new_daddr = (__force __be32)act->mangle.val;
				else
					return -EOPNOTSUPP;
				break;
			case FLOW_ACT_MANGLE_HDR_TYPE_TCP:
			case FLOW_ACT_MANGLE_HDR_TYPE_UDP: {
				u32 val = be32_to_cpu((__force __be32)act->mangle.val);

				if (act->mangle.offset == 0) {
					if (act->mangle.mask == ~(__force u32)cpu_to_be32(0xffff))
						rw.new_dport = cpu_to_be16(val);
					else
						rw.new_sport = cpu_to_be16(val >> 16);
				} else if (act->mangle.offset == 2) {
					rw.new_dport = cpu_to_be16(val);
				} else {
					return -EOPNOTSUPP;
				}
				break;
			}
			default:
				return -EOPNOTSUPP;
			}
			break;
		case FLOW_ACTION_CSUM:
			break;
		case FLOW_ACTION_REDIRECT:
			odev = act->dev;
			break;
		default:
			/* VLAN, PPPoE and tunnel encapsulation are not supported */
			return -EOPNOTSUPP;
		}
	}

	if (!odev || !zx_eth_is_port(odev))
		return -EOPNOTSUPP;
	port = ((struct zx_eth_priv *)netdev_priv(odev))->port;
	if (!is_valid_ether_addr(eth.h_source) || !is_valid_ether_addr(eth.h_dest))
		return -EOPNOTSUPP;

	rw.sip = new_saddr != t.saddr;
	rw.dip = new_daddr != t.daddr;
	rw.sport = rw.new_sport != t.sport;
	rw.dport = rw.new_dport != t.dport;

	zx_ppe_slots(&t, slot);
	if (!test_bit(slot[0], adapter->ppe_hash_used))
		slot[1] = slot[0];
	else if (test_bit(slot[1], adapter->ppe_hash_used))
		return -EBUSY;

	e = kzalloc_obj(*e);
	if (!e)
		return -ENOMEM;

	subnet = zx_ppe_subnet_get(adapter, eth.h_source, rw.sip ? new_saddr : 0);
	if (subnet < 0) {
		ret = subnet;
		goto err_free;
	}
	flow = zx_ppe_alloc(adapter->ppe_flow_used, ZX_PPE_FLOWS, 1);
	if (flow < 0) {
		ret = flow;
		goto err_subnet;
	}
	nh = zx_ppe_alloc(adapter->ppe_nh_used, ZX_PPE_NEXT_HOPS, 1);
	if (nh < 0) {
		ret = nh;
		goto err_flow;
	}
	rw.subnet = subnet;
	rw.nh = nh;

	/* next hop: new destination IP (used for DNAT) and MAC */
	w[0] = rw.dip ? be32_to_cpu(new_daddr) : 0;
	zx_mac_words(eth.h_dest, w + 1);
	ret = zx_pp_write(adapter, ZX_PM_BASE, ZX_PM_NEXT_HOP, nh, w, 3);
	if (ret)
		goto err_nh;
	zx_ppe_flow_entry(&rw, w);
	ret = zx_pp_write(adapter, ZX_PM_BASE, ZX_PM_FLOW, flow, w, 3);
	if (ret)
		goto err_nh;
	ret = zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_AGING, slot[1], w, 0);
	if (ret)
		goto err_nh;
	zx_ppe_hash_entry(&t, flow, port, w);
	ret = zx_pp_write(adapter, ZX_CLA_BASE,
			  slot[1] < ZX_PPE_HASH0_SIZE ? ZX_CLA_HASH0 : ZX_CLA_HASH1,
			  slot[1] % ZX_PPE_HASH0_SIZE, w, 9);
	if (ret)
		goto err_nh;

	e->cookie = f->cookie;
	e->hash = slot[1];
	e->flow = flow;
	e->nh = nh;
	e->subnet = subnet;
	ret = rhashtable_insert_fast(&adapter->ppe_flows, &e->node, zx_ppe_ht_params);
	if (ret)
		goto err_entry;
	set_bit(e->hash, adapter->ppe_hash_used);
	adapter->ppe_count++;
	return 0;

err_entry:
	zx_pp_write(adapter, ZX_CLA_BASE,
		    slot[1] < ZX_PPE_HASH0_SIZE ? ZX_CLA_HASH0 : ZX_CLA_HASH1,
		    slot[1] % ZX_PPE_HASH0_SIZE, w, 0);
err_nh:
	clear_bit(nh, adapter->ppe_nh_used);
err_flow:
	clear_bit(flow, adapter->ppe_flow_used);
err_subnet:
	zx_ppe_subnet_put(adapter, subnet);
err_free:
	kfree(e);
	return ret;
}

static void zx_ppe_entry_remove(struct zx_eth_adapter *adapter,
				struct zx_ppe_entry *e)
{
	zx_pp_write(adapter, ZX_CLA_BASE,
		    e->hash < ZX_PPE_HASH0_SIZE ? ZX_CLA_HASH0 : ZX_CLA_HASH1,
		    e->hash % ZX_PPE_HASH0_SIZE, NULL, 0);
	zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_AGING, e->hash, NULL, 0);
	clear_bit(e->hash, adapter->ppe_hash_used);
	clear_bit(e->flow, adapter->ppe_flow_used);
	clear_bit(e->nh, adapter->ppe_nh_used);
	zx_ppe_subnet_put(adapter, e->subnet);
	adapter->ppe_count--;
}

static int zx_ppe_flow_del(struct zx_eth_adapter *adapter,
			   struct flow_cls_offload *f)
{
	struct zx_ppe_entry *e;

	e = rhashtable_lookup_fast(&adapter->ppe_flows, &f->cookie, zx_ppe_ht_params);
	if (!e)
		return -ENOENT;
	rhashtable_remove_fast(&adapter->ppe_flows, &e->node, zx_ppe_ht_params);
	zx_ppe_entry_remove(adapter, e);
	kfree(e);
	return 0;
}

/* No packet counters: report use from the entry's hit flag */
static int zx_ppe_flow_stats(struct zx_eth_adapter *adapter,
			     struct flow_cls_offload *f)
{
	struct zx_ppe_entry *e;
	u32 hit;

	e = rhashtable_lookup_fast(&adapter->ppe_flows, &f->cookie, zx_ppe_ht_params);
	if (!e)
		return -ENOENT;
	if (zx_pp_read_word0(adapter, ZX_CLA_BASE, ZX_CLA_AGING, e->hash, &hit))
		return -ETIMEDOUT;
	if (hit & 1) {
		f->stats.lastused = jiffies;
		zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_AGING, e->hash, NULL, 0);
	}
	return 0;
}

/* Values from the stock firmware */
static const u32 zx_ppe_v4_index[5] = {
	0x93929190, 0x97969594, 0x9b9a9998, 0x9f9e9d9c,
	0x00150151,	/* bit 8: look up the hash banks */
};

static const u32 zx_ppe_v4_rule[16] = {
	0x22038608, 0x000058a1, 0, 0, 0xf00ff000, 0xffffffff, 0xffffffff,
	0x0fffffff, 0, 0, 0, 0, 0, 0, 0x00700000, 0x00092492,
};

static const u32 zx_ppe_sub[2] = { 0xfc000000, 0x00001fff };

/* classifier L3 MTU and actions, packet modifier padding of short frames.
 * The "other L3 packet" action (0x38c0cc) keeps the value 1 of the base
 * driver: with the stock value 0, IPv6 addressed to our MACs was dropped
 * once its trap was lifted (hardware LAN switching).
 */
static const u32 zx_ppe_regs[4] = { 0x38c088, 0x38c094, 0x38c098, 0x39c034 };
static const u32 zx_ppe_reg_vals[4] = { 0x7fff, 4, 0x7fff7fff, 0x3d };

/*
 * Traps that send frames to the CPU before the flow lookup: the IPv4
 * ethertype trap, and the registered packet class for TCP segments without
 * payload (trap reason 0x2c), which would send every pure ACK of an
 * offloaded connection through the software path.  Both are off while
 * offload is active.  SYN, FIN and RST still reach the CPU, so conntrack
 * sees connections open and close.  Hardware LAN switching also needs the
 * IPv6 trap off.  Frames for the router, broadcast, multicast and unknown
 * unicast still reach the CPU through the other traps.
 */
void zx_update_traps(struct zx_eth_adapter *adapter)
{
	void __iomem *base = adapter->base;
	bool sw, fwd;
	u32 val;

	spin_lock(&adapter->trap_lock);
	sw = READ_ONCE(adapter->sw_members);
	fwd = READ_ONCE(adapter->ppe_active) || sw;
	writel((fwd ? 0 : ETH_P_IP << 16) | (sw ? 0 : ETH_P_IPV6),
	       base + ZX_SPA_TRAP_ETH_TYPE);
	val = readl(base + ZX_SPA_UP_REG_PKT_EN + 8);
	if (fwd)
		val &= ~ZX_SPA_REG_PKT_TCP_ACK;
	else
		val |= ZX_SPA_REG_PKT_TCP_ACK;
	writel(val, base + ZX_SPA_UP_REG_PKT_EN + 8);
	spin_unlock(&adapter->trap_lock);
}

/*
 * Frames to the ports' own addresses reach the CPU through the trap table,
 * or with flow offload active, through the ONU MAC table, which marks them
 * as routed so that the classifier sees them first.  Addresses beyond the
 * trap table still reach the CPU as unknown unicast.  Called with ppe_lock
 * held, after an address or the offload state changed.
 */
void zx_update_mac_tables(struct zx_eth_adapter *adapter)
{
	static const u8 none[ETH_ALEN];
	void __iomem *base = adapter->base;
	const u8 *addrs[ZX_NUM_PORTS];
	int i, j, n = 0;
	u32 w[2];

	lockdep_assert_held(&adapter->ppe_lock);

	/* each distinct port address once */
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		struct net_device *dev = adapter->ports[i];

		if (!dev)
			continue;
		for (j = 0; j < n; j++)
			if (ether_addr_equal(addrs[j], dev->dev_addr))
				break;
		if (j == n)
			addrs[n++] = dev->dev_addr;
	}

	for (i = 0; i < ZX_SPA_ONU_MAC_NUM; i++) {
		zx_mac_words(adapter->ppe_active && i < n ? addrs[i] : none, w);
		writel(w[0], base + ZX_SPA_ONU_MAC + i * 8);
		writel(w[1], base + ZX_SPA_ONU_MAC + i * 8 + 4);
	}
	for (i = 0; i < ZX_SPA_TRAP_DMAC_NUM; i++)
		zx_set_trap_dmac(adapter, i,
				 !adapter->ppe_active && i < n ? addrs[i] : none);
}

/*
 * Routed frames must reach the classifier: mark our MACs as routed (ONU
 * MAC table) and stop trapping IPv4 and frames to our MACs straight to the
 * CPU.  IPv6, ARP and VLAN tagged frames stay trapped.  Frames the
 * classifier does not know still go to the CPU.
 */
static int zx_ppe_start(struct zx_eth_adapter *adapter)
{
	static const u32 cmd = 0x00010000;
	void __iomem *base = adapter->base;
	int i, ret;

	for (i = 0; i < ZX_PPE_SUBNETS; i++)
		adapter->ppe_subnet[i].refs = 0;
	bitmap_zero(adapter->ppe_hash_used, ZX_PPE_HASH_SIZE);
	bitmap_zero(adapter->ppe_flow_used, ZX_PPE_FLOWS);
	bitmap_zero(adapter->ppe_nh_used, ZX_PPE_NEXT_HOPS);

	for (i = 0; i < ARRAY_SIZE(zx_ppe_regs); i++) {
		adapter->ppe_saved[i] = readl(base + zx_ppe_regs[i]);
		writel(zx_ppe_reg_vals[i], base + zx_ppe_regs[i]);
	}

	ret = zx_pp_write(adapter, ZX_PM_BASE, ZX_PM_CMD, 0, &cmd, 1);
	if (!ret)
		ret = zx_pp_write(adapter, ZX_PM_BASE, ZX_PM_SUB, 0, zx_ppe_sub, 2);
	if (!ret)
		ret = zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_EXTRA_RULE,
				  ZX_PPE_V4_RULE, zx_ppe_v4_rule, 16);
	if (!ret)
		ret = zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_EXTRA_INDEX,
				  ZX_PPE_V4_INDEX, zx_ppe_v4_index, 5);
	if (ret) {
		for (i = 0; i < ARRAY_SIZE(zx_ppe_regs); i++)
			writel(adapter->ppe_saved[i], base + zx_ppe_regs[i]);
		return ret;
	}

	adapter->ppe_active = true;
	zx_update_mac_tables(adapter);
	zx_update_traps(adapter);
	dev_info(adapter->dev, "hardware flow offload enabled\n");
	return 0;
}

static void zx_ppe_flush(void *ptr, void *arg)
{
	struct zx_ppe_entry *e = ptr;

	zx_ppe_entry_remove(arg, e);
	kfree(e);
}

static void zx_ppe_stop(struct zx_eth_adapter *adapter)
{
	void __iomem *base = adapter->base;
	int i;

	/* back to trapping everything, then drop the tables */
	adapter->ppe_active = false;
	zx_update_traps(adapter);
	zx_update_mac_tables(adapter);

	rhashtable_free_and_destroy(&adapter->ppe_flows, zx_ppe_flush, adapter);
	rhashtable_init(&adapter->ppe_flows, &zx_ppe_ht_params);
	zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_EXTRA_INDEX, ZX_PPE_V4_INDEX, NULL, 0);
	zx_pp_write(adapter, ZX_CLA_BASE, ZX_CLA_EXTRA_RULE, ZX_PPE_V4_RULE, NULL, 0);
	for (i = 0; i < ARRAY_SIZE(zx_ppe_regs); i++)
		writel(adapter->ppe_saved[i], base + zx_ppe_regs[i]);
	dev_info(adapter->dev, "hardware flow offload disabled\n");
}

static int zx_ppe_block_cb(enum tc_setup_type type, void *type_data, void *cb_priv)
{
	struct zx_eth_adapter *adapter = cb_priv;
	struct flow_cls_offload *f = type_data;
	int ret;

	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;

	mutex_lock(&adapter->ppe_lock);
	switch (f->command) {
	case FLOW_CLS_REPLACE:
		ret = zx_ppe_flow_add(adapter, f);
		if (ret == -EOPNOTSUPP)
			adapter->ppe_add_skip++;
		else if (ret && ret != -EEXIST)
			adapter->ppe_add_fail++;
		break;
	case FLOW_CLS_DESTROY:
		ret = zx_ppe_flow_del(adapter, f);
		break;
	case FLOW_CLS_STATS:
		ret = zx_ppe_flow_stats(adapter, f);
		break;
	default:
		ret = -EOPNOTSUPP;
	}
	mutex_unlock(&adapter->ppe_lock);
	return ret;
}

static LIST_HEAD(zx_ppe_block_cb_list);

/* One callback for the flowtable of all port netdevs, so each flow is
 * offered once.
 */
int zx_eth_setup_tc(struct net_device *dev, enum tc_setup_type type,
		    void *type_data)
{
	struct zx_eth_priv *priv = netdev_priv(dev);
	struct zx_eth_adapter *adapter = priv->adapter;
	struct flow_block_offload *f = type_data;
	struct flow_block_cb *block_cb;
	int ret = 0;

	if (type != TC_SETUP_FT)
		return -EOPNOTSUPP;
	if (f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		return -EOPNOTSUPP;

	f->driver_block_list = &zx_ppe_block_cb_list;
	block_cb = flow_block_cb_lookup(f->block, zx_ppe_block_cb, adapter);

	switch (f->command) {
	case FLOW_BLOCK_BIND:
		if (block_cb) {
			flow_block_cb_incref(block_cb);
			return 0;
		}
		block_cb = flow_block_cb_alloc(zx_ppe_block_cb, adapter, adapter, NULL);
		if (IS_ERR(block_cb))
			return PTR_ERR(block_cb);
		/* A ruleset reload binds the new flowtable before it unbinds the
		 * old one, so keep the hardware on while any flowtable is bound.
		 */
		mutex_lock(&adapter->ppe_lock);
		if (!adapter->ppe_users)
			ret = zx_ppe_start(adapter);
		if (!ret)
			adapter->ppe_users++;
		mutex_unlock(&adapter->ppe_lock);
		if (ret) {
			flow_block_cb_free(block_cb);
			return ret;
		}
		flow_block_cb_incref(block_cb);
		flow_block_cb_add(block_cb, f);
		list_add_tail(&block_cb->driver_list, &zx_ppe_block_cb_list);
		return 0;
	case FLOW_BLOCK_UNBIND:
		if (!block_cb)
			return -ENOENT;
		if (!flow_block_cb_decref(block_cb)) {
			flow_block_cb_remove(block_cb, f);
			list_del(&block_cb->driver_list);
			mutex_lock(&adapter->ppe_lock);
			if (!--adapter->ppe_users)
				zx_ppe_stop(adapter);
			mutex_unlock(&adapter->ppe_lock);
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}
