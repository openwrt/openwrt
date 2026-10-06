// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 Gennaro Cimmino <gcimmino@rayonra.net>
 * Assisted-by: Claude:claude-opus-5, Claude:claude-opus-5-5
 */

/*
 * Layer 3 unicast routing offload for the Realtek Otto switches
 *
 * The driver follows the kernel's routes through the FIB notifier and its
 * neighbours through the netevent notifier, and mirrors the routes whose
 * gateway sits behind a switch port into the switch, which then routes their
 * packets without the CPU. Only the local and main tables are taken.
 *
 * Hardware logic. The families route in one of two ways (use_l3_tables in
 * struct otto_l3_config):
 * - RTL930x matches the destination in its own tables: a host table of single
 *   addresses and a prefix table, which answers with the lowest matching row
 *   rather than the longest prefix. A packet whose destination MAC hits a
 *   router MAC entry is routed. The entry it matches either traps it to the
 *   CPU or points at a next hop entry, which names the gateway's entry in the
 *   L2 table, for its MAC and port, and an egress interface with the VLAN,
 *   source MAC and MTU to route with.
 * - RTL838x and RTL839x hold no destination in their route table, only the
 *   gateway's MAC. A PIE rule matches the destination prefix and points at
 *   the gateway's L2 entry.
 *
 * Driver logic on RTL930x. IPv4 /32 routes go to the host table, all others,
 * IPv6 /128 included, to the prefix table. A route takes its entry as soon as
 * it is added, trapping to the CPU, and forwards once the gateway's neighbour
 * is resolved and its MAC is learnt on a port. It traps again when the
 * neighbour goes away. A destination the switch must not forward itself - an
 * address of the box, an on-link prefix, a route the hardware cannot carry -
 * gets a trapping entry too, so that no shorter prefix in hardware forwards
 * it. The prefix rows of each family form one block, longest prefix first,
 * and a catch-all row below each block hands lookup misses to the CPU, which
 * the hardware would otherwise drop. Policy rules under which the kernel's
 * lookup for a forwarded packet is not local, then main, turn the forwarding
 * entries of that family into traps until those rules are gone. An event the
 * driver could not take, for want of memory, has the routes flushed and the
 * FIB replayed.
 *
 * State per family:
 * - RTL930x: IPv4 and IPv6 unicast routes through a gateway are forwarded.
 *   The prefix table has 512 rows shared by both families; an IPv6 entry
 *   takes three, and at most two start in every eight rows. The driver takes
 *   up to 1536 host routes. Trapped to the CPU: addresses of the box, on-link
 *   prefixes, blackhole, unreachable and prohibit routes, routes through a
 *   device outside the switch, a nexthop object or a lightweight tunnel,
 *   multipath routes, IPv4 routes for one DSCP value and, for IPv6,
 *   source-specific and RA-learnt routes.
 *   Default routes have no entry, so their packets reach the CPU through the
 *   catch-all rows. Multicast routing is not offloaded, and all egress
 *   interfaces share one 1536 byte MTU. The programmed state is shown in
 *   debugfs, under realtek_otto_l3.
 * - RTL839x: IPv4 unicast routes through a gateway are forwarded through PIE
 *   rules, each with a packet counter while one is free. No IPv6, and no
 *   trapping entries.
 * - RTL838x: route table accessors only. FIB and neighbour events are ignored.
 * - RTL931x: not supported. FIB and neighbour events are ignored.
 */

#include <linux/debugfs.h>
#include <linux/if_vlan.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/iopoll.h>
#include <linux/notifier.h>
#include <linux/of.h>
#include <linux/seq_file.h>
#include <net/arp.h>
#include <net/fib_notifier.h>
#include <net/ip6_fib.h>
#include <net/ndisc.h>
#include <net/netevent.h>
#include <net/nexthop.h>
#include <uapi/linux/rtnetlink.h>

#include "l3.h"
#include "pie.h"
#include "rtl-otto.h"

/* Route entry types */
#define ROUTE_TYPE_IP4UC	0
#define ROUTE_TYPE_IP4MC	1
#define ROUTE_TYPE_IP6UC	2
#define ROUTE_TYPE_IP6MC	3

/* Route actions */
#define ROUTE_ACT_FORWARD	0
#define ROUTE_ACT_TRAP2CPU	1
#define ROUTE_ACT_COPY2CPU	2
#define ROUTE_ACT_DROP		3

/* Route kinds */
#define ROUTE_PREFIX		0
#define ROUTE_HOST		1

/* L3 Routing */
#define RTL839X_ROUTING_SA_CTRL			0x6afc
#define RTL930X_L3_HOST_TBL_CTRL		(0xAB48)
#define RTL930X_L3_IPUC_ROUTE_CTRL		(0xAB4C)
#define RTL930X_L3_IP6UC_ROUTE_CTRL		(0xAB50)
#define RTL930X_L3_IPMC_ROUTE_CTRL		(0xAB54)
#define RTL930X_L3_IP6MC_ROUTE_CTRL		(0xAB58)
#define RTL930X_L3_IP_MTU_CTRL(i)		(0xAB5C + ((i >> 1) << 2))
#define RTL930X_L3_IP6_MTU_CTRL(i)		(0xAB6C + ((i >> 1) << 2))
#define RTL930X_L3_HW_LU_KEY_CTRL		(0xAC9C)
#define RTL930X_L3_HW_LU_KEY_IP_CTRL		(0xACA0)
#define RTL930X_L3_HW_LU_CTRL			(0xACC0)
#define RTL930X_L3_IP_ROUTE_CTRL		0xab44

#define DEFAULT_MTU 1536
#define MAX_ROUTER_MACS 64
#define L3_EGRESS_DMACS 2048

struct otto_l3_net_event_work {
	struct work_struct work;
	struct otto_l3_ctrl *ctrl;
	u64 mac;
	struct in6_addr gw_addr;
	int ifindex;
	bool valid;
	u8 type;
};

struct otto_l3_fib_event_work {
	struct work_struct work;
	struct otto_l3_ctrl *ctrl;
	union {
		struct fib_entry_notifier_info fen_info;
		struct fib6_entry_notifier_info fen6_info;
		struct fib_rule_notifier_info fr_info;
	};
	unsigned int members;
	unsigned long event;
};

struct otto_l3_walk_data {
	struct otto_l3_ctrl *ctrl;
	int port;
};

static void otto_l3_838x_route_read(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	u32 data[2];

	otto_table_read(RTL8380_TBL_ROUTING, idx, &data);

	rt->nh.gw = data[0];
	rt->nh.gw <<= 32;
	rt->nh.gw |= data[1];
}

static void otto_l3_838x_route_write(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	u32 data[2] = { rt->nh.gw >> 32, rt->nh.gw };

	otto_table_write(RTL8380_TBL_ROUTING, idx, &data);
}

static void otto_l3_839x_route_read(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	u32 data[2];
	u64 v;

	otto_table_read(RTL8390_TBL_ROUTING, idx, &data);

	v = data[0];
	v <<= 32;
	v |= data[1];
	rt->switch_mac_id = (v >> 12) & 0xf;
	rt->nh.gw = v >> 16;
}

static void otto_l3_839x_route_write(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	u32 data[2];

	data[0] = rt->nh.gw >> 16;
	data[1] = (rt->nh.gw << 16) | (rt->switch_mac_id << 12);

	otto_table_write(RTL8390_TBL_ROUTING, idx, &data);
}

static void otto_l3_839x_setup_port_macs(struct otto_l3_ctrl *ctrl)
{
	struct rtl838x_switch_priv *priv = ctrl->priv;
	struct net_device *dev;
	u64 mac;

	/* Configure the switch's own MAC addresses used when routing packets */
	dev_dbg(ctrl->dev, "got port %08x\n", (u32)priv->ports[priv->r->cpu_port].dp);
	dev = priv->ports[priv->r->cpu_port].dp->user;
	mac = ether_addr_to_u64(dev->dev_addr);

	for (int i = 0; i < 15; i++) {
		mac++;  /* BUG: VRRP for testing */
		sw_w32(mac >> 32, RTL839X_ROUTING_SA_CTRL + i * 8);
		sw_w32(mac, RTL839X_ROUTING_SA_CTRL + i * 8 + 4);
	}
}

static int otto_l3_839x_setup(struct otto_l3_ctrl *ctrl)
{
	otto_l3_839x_setup_port_macs(ctrl);

	return 0;
}

static u32 otto_l3_930x_hash4(u32 ip, int algorithm, bool move_dip)
{
	u32 s0, s1, pH;
	u32 rows[4];
	u32 hash;

	memset(rows, 0, sizeof(rows));

	rows[0] = HASH_PICK(ip, 27, 5);
	rows[1] = HASH_PICK(ip, 18, 9);
	rows[2] = HASH_PICK(ip, 9, 9);

	if (!move_dip)
		rows[3] = HASH_PICK(ip, 0, 9);

	if (!algorithm) {
		hash = rows[0] ^ rows[1] ^ rows[2] ^ rows[3];
	} else {
		s0 = rows[0] + rows[1] + rows[2];
		s1 = (s0 & 0x1ff) + ((s0 & (0x1ff << 9)) >> 9);
		pH = (s1 & 0x1ff) + ((s1 & (0x1ff << 9)) >> 9);
		hash = pH ^ rows[3];
	}
	return hash;
}

/*
 * Get the Destination-MAC of an L3 egress interface or the Source MAC for routed packets
 * from the SoC's L3_EGR_INTF_MAC table. Indexes 0-2047 are DMACs, 2048+ are SMACs
 */
static u64 otto_l3_930x_get_egress_mac(struct otto_l3_ctrl *ctrl, u32 idx)
{
	u32 data[2];
	u64 mac;

	otto_table_read(RTL9300_TBL_L3_EGR_INTF_MAC, idx, &data);
	mac = data[0];
	mac <<= 32;
	mac |= data[1];

	return mac;
}

/* Set the Destination-MAC of a route or the Source MAC of an L3 egress interface
 * in the SoC's L3_EGR_INTF_MAC table. Indexes 0-2047 are DMACs, 2048+ are SMACs
 */
static void otto_l3_930x_set_egress_mac(struct otto_l3_ctrl *ctrl, u32 idx, u64 mac)
{
	u32 data[2] = { mac >> 32, mac };

	dev_dbg(ctrl->dev, "setting index %d to %016llx\n", idx, mac);
	otto_table_write(RTL9300_TBL_L3_EGR_INTF_MAC, idx, &data);
}

/* Decode a host route entry fetched into @data. Returns false unless it is a
 * valid unicast entry; valid is set for any entry, type for any valid one.
 */
static bool otto_l3_930x_host_route_decode(const u32 *data, struct otto_l3_route *rt)
{
	u32 v = data[0];

	rt->attr.valid = !!(v & BIT(31));
	if (!rt->attr.valid)
		return false;
	rt->attr.type = (v >> 29) & 0x3;
	switch (rt->attr.type) {
	case ROUTE_TYPE_IP4UC:
		rt->dst_ip = data[4];
		break;
	case ROUTE_TYPE_IP6UC:
		ipv6_addr_set(&rt->dst_ip6,
			      data[1], data[2],
			      data[3], data[4]);
		break;
	default:
		return false;
	}

	rt->attr.hit = !!(v & BIT(20));
	rt->attr.dst_null = !!(v & BIT(19));
	rt->attr.action = (v >> 17) & 3;
	rt->nh.id = (v >> 6) & 0x7ff;
	rt->attr.ttl_dec = !!(v & BIT(5));
	rt->attr.ttl_check = !!(v & BIT(4));
	rt->attr.qos_as = !!(v & BIT(3));
	rt->attr.qos_prio =  v & 0x7;

	return true;
}

/* Read a host route entry from the table using its index. Only IPv4 and IPv6 unicast supported */
static void otto_l3_930x_host_route_read(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	/* Only the unicast layout is handled below */
	u32 data[5];

	idx = ((idx / 6) * 8) + (idx % 6);

	otto_table_read(RTL9300_TBL_L3_HOST_ROUTE_IPUC, idx, &data);
	if (!otto_l3_930x_host_route_decode(data, rt)) {
		if (rt->attr.valid)
			dev_warn(ctrl->dev, "route type not supported\n");
		return;
	}

	dev_dbg(ctrl->dev, "index %d is valid: %d\n", idx, rt->attr.valid);
	dev_dbg(ctrl->dev, "next_hop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		rt->nh.id, rt->attr.hit, rt->attr.action, rt->attr.ttl_dec, rt->attr.ttl_check,
		rt->attr.dst_null);
	dev_dbg(ctrl->dev, "Destination: %pI4\n", &rt->dst_ip);
}

/* Write a host route entry from the table using its index. Only unicast routes supported */
static void otto_l3_930x_host_route_write(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	/* Only the unicast layout is handled below */
	u32 data[5];
	u32 v;

	idx = ((idx / 6) * 8) + (idx % 6);

	dev_dbg(ctrl->dev, "index %d is valid: %d\n", idx, rt->attr.valid);
	dev_dbg(ctrl->dev, "next_hop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		rt->nh.id, rt->attr.hit, rt->attr.action, rt->attr.ttl_dec, rt->attr.ttl_check,
		rt->attr.dst_null);
	dev_dbg(ctrl->dev, "GW: %pI4, prefix_len: %d\n", &rt->dst_ip, rt->prefix_len);

	v = rt->attr.valid ? BIT(31) : 0;
	v |= (rt->attr.type & 0x3) << 29;
	v |= rt->attr.hit ? BIT(20) : 0;
	v |= rt->attr.dst_null ? BIT(19) : 0;
	v |= (rt->attr.action & 0x3) << 17;
	v |= (rt->nh.id & 0x7ff) << 6;
	v |= rt->attr.ttl_dec ? BIT(5) : 0;
	v |= rt->attr.ttl_check ? BIT(4) : 0;
	v |= rt->attr.qos_as ? BIT(3) : 0;
	v |= rt->attr.qos_prio & 0x7;

	data[0] = v;
	switch (rt->attr.type) {
	case ROUTE_TYPE_IP4UC:
		data[1] = 0;
		data[2] = 0;
		data[3] = 0;
		data[4] = rt->dst_ip;
		break;
	case ROUTE_TYPE_IP6UC:
		data[1] = rt->dst_ip6.s6_addr32[0];
		data[2] = rt->dst_ip6.s6_addr32[1];
		data[3] = rt->dst_ip6.s6_addr32[2];
		data[4] = rt->dst_ip6.s6_addr32[3];
		break;
	case ROUTE_TYPE_IP4MC:
	case ROUTE_TYPE_IP6MC:
		dev_warn(ctrl->dev, "route type not supported\n");
		return;
	}

	otto_table_write(RTL9300_TBL_L3_HOST_ROUTE_IPUC, idx, &data);
}

/* Slots one entry occupies, by type: IPv4 unicast, IPv4 multicast, IPv6
 * unicast, IPv6 multicast. They come from the SDK allocator rather than from
 * the entry size, which does not imply them.
 */
static const u8 otto_l3_930x_slot_widths[] = { 1, 2, 3, 6 };

static int otto_l3_930x_find_slot(struct otto_l3_ctrl *ctrl, struct otto_l3_route *rt, bool must_exist)
{
	int slot_width, algorithm, addr, idx;
	struct otto_l3_route route_entry;
	u32 hash;

	slot_width = otto_l3_930x_slot_widths[rt->attr.type & 0x3];

	for (int t = 0; t < 2; t++) {
		algorithm = (sw_r32(RTL930X_L3_HOST_TBL_CTRL) >> (2 + t)) & 0x1;
		hash = otto_l3_930x_hash4(rt->dst_ip, algorithm, false);

		dev_dbg(ctrl->dev, "table %d, algorithm %d, hash %04x\n", t, algorithm, hash);

		for (int s = 0; s < 6; s += slot_width) {
			addr = (t << 12) | ((hash & 0x1ff) << 3) | s;
			dev_dbg(ctrl->dev, "physical address %d\n", addr);
			idx = ((addr / 8) * 6) + (addr % 8);
			dev_dbg(ctrl->dev, "logical address %d\n", idx);

			otto_l3_930x_host_route_read(ctrl, idx, &route_entry);
			dev_dbg(ctrl->dev, "route valid %d, route dest: %pI4, hit %d\n",
				rt->attr.valid, &rt->dst_ip, rt->attr.hit);
			if (!must_exist && !route_entry.attr.valid)
				return idx;
			if (must_exist &&
			    route_entry.attr.valid &&
			    route_entry.attr.type == rt->attr.type) {
				switch (rt->attr.type) {
				case ROUTE_TYPE_IP4UC:
					if (route_entry.dst_ip == rt->dst_ip)
						return idx;
					break;
				case ROUTE_TYPE_IP6UC:
					if (ipv6_addr_equal(&route_entry.dst_ip6, &rt->dst_ip6))
						return idx;
					break;
				}
			}
		}
	}

	return -1;
}

/*
 * Reads a MAC entry for L3 termination as entry point for routing from the hardware table.
 * idx is the index into the L3_ROUTER_MAC table
 */
static void otto_l3_930x_get_router_mac(struct otto_l3_ctrl *ctrl,
					u32 idx, struct otto_l3_router_mac *m)
{
	u32 data[7];
	u32 v, w;

	otto_table_read(RTL9300_TBL_L3_ROUTER_MAC, idx, &data);
	v = data[0];
	w = data[3];
	m->valid = !!(v & BIT(20));
	if (!m->valid)
		return;

	m->p_type = !!(v & BIT(19));
	m->p_id = (v >> 13) & 0x3f;  /* trunk id of port */
	m->vid = v & 0xfff;
	m->vid_mask = w & 0xfff;
	m->action = data[6] & 0x7;
	m->mac_mask = ((((u64)data[5]) << 32) & 0xffffffffffffULL) | data[4];
	m->mac = ((((u64)data[1]) << 32) & 0xffffffffffffULL) | data[2];
	/* Bits L3_INTF and BMSK_L3_INTF are 0 */
}

/*
 * Writes a MAC entry for L3 termination as entry point for routing into the hardware table
 * idx is the index into the L3_ROUTER_MAC table
 */
static void otto_l3_930x_set_router_mac(struct otto_l3_ctrl *ctrl,
					u32 idx, struct otto_l3_router_mac *m)
{
	u32 data[7];
	u32 v, w;

	v = BIT(20); /* mac entry valid, port type is 0: individual */
	v |= (m->p_id & 0x3f) << 13;
	v |= (m->vid & 0xfff); /* Set the interface_id to the vlan id */

	w = m->vid_mask;
	w |= (m->p_id_mask & 0x3f) << 13;

	data[0] = v;
	data[3] = w;

	/* Set MAC address, L3_INTF (bit 12 in register 1) needs to be 0 */
	data[2] = (u32)(m->mac);
	data[1] = m->mac >> 32;

	/* Set MAC address mask, BMSK_L3_INTF (bit 12 in register 5) needs to be 0 */
	data[4] = (u32)(m->mac_mask >> 32);
	data[5] = (u32)m->mac_mask;

	data[6] = m->action & 0x7;

	dev_dbg(ctrl->dev, "writing index %d: %08x %08x %08x %08x %08x %08x %08x\n", idx,
		 data[0], data[1], data[2], data[3], data[4], data[5], data[6]);
	otto_table_write(RTL9300_TBL_L3_ROUTER_MAC, idx, &data);
}

/* Destination MAC and L3 egress interface ID of a nexthop entry from the SoC's L3_NEXTHOP table */
static void otto_l3_930x_get_nexthop(struct otto_l3_ctrl *ctrl,
				     int idx, u16 *dmac_id, u16 *interface)
{
	u32 v;

	otto_table_read(RTL9300_TBL_L3_NEXTHOP, idx, &v);

	*dmac_id = (v >> 7) & 0x7fff;
	*interface = v & 0x7f;
}

/*
 * Set the destination MAC and L3 egress interface ID for a nexthop entry in the SoC's L3_NEXTHOP
 * table. The nexthop entry is identified by idx. dmac_id is the reference to the L2 entry in the
 * L2 forwarding table, special values are
 * 0x7ffe: TRAP2CPU
 * 0x7ffd: TRAP2MASTERCPU
 * 0x7fff: DMAC_ID_DROP
 */
static void otto_l3_930x_set_nexthop(struct otto_l3_ctrl *ctrl,
				     int idx, u16 dmac_id, u16 interface)
{
	u32 v = ((dmac_id & 0x7fff) << 7) | (interface & 0x7f);

	dev_dbg(ctrl->dev, "Writing to L3_NEXTHOP table, index %d, dmac_id %d, interface %d\n",
		idx, dmac_id, interface);

	dev_dbg(ctrl->dev, "value at index 0: %08x\n", v);
	otto_table_write(RTL9300_TBL_L3_NEXTHOP, idx, &v);
}

/* Prefix length of an IPv6 mask, i.e. how many leading bits are set */
static int otto_l3_930x_mask6_len(const struct in6_addr *mask)
{
	int len = 0;

	for (int i = 0; i < 4; i++) {
		u32 word = ntohl(mask->s6_addr32[i]);

		if (word == 0xffffffff) {
			len += 32;
			continue;
		}

		return len + 32 - fls(~word);
	}

	return len;
}

/* Decode a prefix route entry fetched into @data. Returns false unless it is
 * a valid unicast entry; valid is set for any entry, type for any valid one.
 */
static bool otto_l3_930x_route_decode(const u32 *data, struct otto_l3_route *rt)
{
	bool host_route, default_route;
	struct in6_addr ip6_m;
	u32 v;

	rt->attr.valid = !!(data[0] & BIT(31));
	if (!rt->attr.valid)
		return false;

	rt->attr.type = (data[0] >> 29) & 0x3;

	v = data[10];
	host_route = !!(v & BIT(21));
	default_route = !!(v & BIT(20));
	rt->prefix_len = -1;

	switch (rt->attr.type) {
	case ROUTE_TYPE_IP4UC:
		rt->dst_ip = data[4];
		if (host_route)
			rt->prefix_len = 32;
		else if (default_route)
			rt->prefix_len = 0;
		else
			rt->prefix_len = inet_mask_len(data[9]);
		break;
	case ROUTE_TYPE_IP6UC:
		ipv6_addr_set(&rt->dst_ip6,
			      data[1], data[2],
			      data[3], data[4]);
		ipv6_addr_set(&ip6_m,
			      data[6], data[7],
			      data[8], data[9]);
		if (host_route)
			rt->prefix_len = 128;
		else if (default_route)
			rt->prefix_len = 0;
		else
			rt->prefix_len = otto_l3_930x_mask6_len(&ip6_m);
		break;
	default:
		return false;
	}

	rt->attr.hit = !!(v & BIT(22));
	rt->attr.action = (v >> 18) & 3;
	rt->nh.id = (v >> 7) & 0x7ff;
	rt->attr.ttl_dec = !!(v & BIT(6));
	rt->attr.ttl_check = !!(v & BIT(5));
	rt->attr.dst_null = !!(v & BIT(4));
	rt->attr.qos_as = !!(v & BIT(3));
	rt->attr.qos_prio =  v & 0x7;

	return true;
}

/* Read a prefix route entry from the L3_PREFIX_ROUTE_IPUC table
 * We currently only support IPv4 and IPv6 unicast route
 */
static void otto_l3_930x_route_read(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	u32 data[11];

	dev_dbg(ctrl->dev, "%s\n", __func__);

	otto_table_read(RTL9300_TBL_L3_PREFIX_ROUTE_IPUC, idx, &data);
	if (!otto_l3_930x_route_decode(data, rt)) {
		if (rt->attr.valid)
			dev_warn(ctrl->dev, "route type not supported\n");
		return;
	}

	dev_dbg(ctrl->dev, "index %d is valid: %d\n", idx, rt->attr.valid);
	dev_dbg(ctrl->dev, "next_hop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		rt->nh.id, rt->attr.hit, rt->attr.action,
		rt->attr.ttl_dec, rt->attr.ttl_check, rt->attr.dst_null);
	dev_dbg(ctrl->dev, "GW: %pI4, prefix_len: %d\n", &rt->dst_ip, rt->prefix_len);
}

static void otto_l3_930x_net6_mask(int prefix_len, struct in6_addr *ip6_m)
{
	int o = prefix_len >> 3;
	int b = prefix_len & 0x7;

	memset(ip6_m, 0, sizeof(*ip6_m));
	memset(ip6_m->s6_addr, 0xff, o);
	if (b)
		ip6_m->s6_addr[o] = 0xff00 >> b;
}

/*
 * Look up the index of a prefix route in the routing table CAM for unicast IPv4/6 routes
 * using hardware offload.
 */
static int otto_l3_930x_route_lookup_hw(struct otto_l3_ctrl *ctrl, struct otto_l3_route *rt)
{
	u32 ip4_m, v;

	if (rt->attr.type == ROUTE_TYPE_IP4MC || rt->attr.type == ROUTE_TYPE_IP6MC)
		return -1;

	sw_w32_mask(0x3 << 19, rt->attr.type << 19, RTL930X_L3_HW_LU_KEY_CTRL);
	if (rt->attr.type) { /* IPv6 */
		struct in6_addr key;

		ipv6_addr_prefix(&key, &rt->dst_ip6, rt->prefix_len);
		for (int i = 0; i < 4; i++)
			sw_w32(key.s6_addr32[i],
			       RTL930X_L3_HW_LU_KEY_IP_CTRL + (i << 2));
	} else { /* IPv4 */
		ip4_m = inet_make_mask(rt->prefix_len);
		sw_w32(0, RTL930X_L3_HW_LU_KEY_IP_CTRL);
		sw_w32(0, RTL930X_L3_HW_LU_KEY_IP_CTRL + 4);
		sw_w32(0, RTL930X_L3_HW_LU_KEY_IP_CTRL + 8);
		v = rt->dst_ip & ip4_m;
		dev_dbg(ctrl->dev, "searching for %pI4\n", &v);
		sw_w32(v, RTL930X_L3_HW_LU_KEY_IP_CTRL + 12);
	}

	/* Execute CAM lookup in SoC */
	sw_w32(BIT(15), RTL930X_L3_HW_LU_CTRL);

	/* Wait until execute bit clears and result is ready */
	if (readx_poll_timeout(sw_r32, RTL930X_L3_HW_LU_CTRL, v, !(v & BIT(15)),
			       20, 10000))
		return -ETIMEDOUT;

	dev_dbg(ctrl->dev, "found: %d, index: %d\n", !!(v & BIT(14)), v & 0x1ff);

	/* Test if search successful (BIT 14 set) */
	if (v & BIT(14))
		return v & 0x1ff;

	return -1;
}

/* Move count prefix route rows from src to dst. The rows are copied as they
 * are: re-encoding them would rebuild the entries from driver state, which
 * does not carry the hit bit, and would lose the multicast rows the driver
 * cannot decode. Returns -EAGAIN when no row was written and the table is as
 * it was, and -EIO when the block may be half shifted.
 */
static int otto_l3_930x_route_rows_move(struct otto_l3_ctrl *ctrl, int dst, int src, int count)
{
	u32 data[11];
	int handle, err = 0;

	handle = otto_table_acquire(RTL9300_TBL_L3_PREFIX_ROUTE_IPUC);
	if (handle < 0) {
		dev_err(ctrl->dev, "cannot move prefix route rows: %d\n", handle);
		return -EAGAIN;
	}

	/* The ranges overlap by one row per insertion or removal, so the copy
	 * runs away from the direction of travel. A read that fails hands back
	 * a cleared buffer, so a row is only written once its source is in.
	 */
	for (int n = 0; n < count; n++) {
		int i = dst > src ? count - 1 - n : n;
		bool unread;

		err = __otto_table_read(handle, src + i, &data);
		unread = err;
		if (!err)
			err = __otto_table_write(handle, dst + i, &data);
		if (err) {
			dev_err(ctrl->dev, "prefix route row %d not moved to %d: %d\n",
				src + i, dst + i, err);
			err = unread && !n ? -EAGAIN : -EIO;
			break;
		}
	}

	otto_table_release(handle);

	return err;
}

/* Write a prefix route into the routing table CAM at position idx
 * Currently only IPv4 and IPv6 unicast routes are supported
 */
static void otto_l3_930x_route_write(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_route *rt)
{
	struct in6_addr ip6_m;
	u32 data[11];
	u32 v, ip4_m;

	dev_dbg(ctrl->dev, "%s\n", __func__);
	dev_dbg(ctrl->dev, "index %d is valid: %d\n", idx, rt->attr.valid);
	dev_dbg(ctrl->dev, "nexthop: %d, hit: %d, action :%d, ttl_dec %d, ttl_check %d, dst_null %d\n",
		rt->nh.id, rt->attr.hit, rt->attr.action,
		rt->attr.ttl_dec, rt->attr.ttl_check, rt->attr.dst_null);
	dev_dbg(ctrl->dev, "GW: %pI4, prefix_len: %d\n", &rt->dst_ip, rt->prefix_len);

	v = rt->attr.valid ? BIT(31) : 0;
	v |= (rt->attr.type & 0x3) << 29;
	data[0] = v;

	v = rt->attr.hit ? BIT(22) : 0;
	v |= (rt->attr.action & 0x3) << 18;
	v |= (rt->nh.id & 0x7ff) << 7;
	v |= rt->attr.ttl_dec ? BIT(6) : 0;
	v |= rt->attr.ttl_check ? BIT(5) : 0;
	v |= rt->attr.dst_null ? BIT(4) : 0;
	v |= rt->attr.qos_as ? BIT(3) : 0;
	v |= rt->attr.qos_prio & 0x7;
	v |= rt->prefix_len == 0 ? BIT(20) : 0; /* set default route bit */

	/* set bit mask for entry type always to 0x3 */
	data[5] = 0x3 << 29;

	switch (rt->attr.type) {
	case ROUTE_TYPE_IP4UC:
		data[1] = 0;
		data[2] = 0;
		data[3] = 0;
		data[4] = rt->dst_ip;

		v |= rt->prefix_len == 32 ? BIT(21) : 0; /* set host-route bit */
		ip4_m = inet_make_mask(rt->prefix_len);
		data[6] = 0;
		data[7] = 0;
		data[8] = 0;
		data[9] = ip4_m;
		break;
	case ROUTE_TYPE_IP6UC:
		data[1] = rt->dst_ip6.s6_addr32[0];
		data[2] = rt->dst_ip6.s6_addr32[1];
		data[3] = rt->dst_ip6.s6_addr32[2];
		data[4] = rt->dst_ip6.s6_addr32[3];

		v |= rt->prefix_len == 128 ? BIT(21) : 0; /* set host-route bit */

		otto_l3_930x_net6_mask(rt->prefix_len, &ip6_m);

		data[6] = ip6_m.s6_addr32[0];
		data[7] = ip6_m.s6_addr32[1];
		data[8] = ip6_m.s6_addr32[2];
		data[9] = ip6_m.s6_addr32[3];
		break;
	case ROUTE_TYPE_IP4MC:
	case ROUTE_TYPE_IP6MC:
		dev_warn(ctrl->dev, "route type not supported\n");
		return;
	}
	data[10] = v;

	dev_dbg(ctrl->dev, "%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n",
		 data[0], data[1], data[2], data[3], data[4], data[5],
		 data[6], data[7], data[8], data[9], data[10]);

	otto_table_write(RTL9300_TBL_L3_PREFIX_ROUTE_IPUC, idx, &data);
}

static int otto_l3_port_lower_walk(struct net_device *lower, struct netdev_nested_priv *_priv)
{
	struct otto_l3_walk_data *data = (struct otto_l3_walk_data *)_priv->data;
	struct rtl838x_switch_priv *priv = data->ctrl->priv;
	struct otto_l3_ctrl *ctrl = data->ctrl;
	int ret = 0;
	int index;

	index = rtl83xx_port_is_under(lower, priv);
	data->port = index;
	if (index >= 0) {
		dev_dbg(ctrl->dev, "Found DSA-port, index %d\n", index);
		ret = 1;
	}

	return ret;
}

static int otto_l3_port_dev_lower_find(struct net_device *dev, struct otto_l3_ctrl *ctrl)
{
	struct otto_l3_walk_data data;
	struct netdev_nested_priv _priv;

	data.ctrl = ctrl;
	data.port = -EINVAL;
	_priv.data = (void *)&data;

	netdev_walk_all_lower_dev(dev, otto_l3_port_lower_walk, &_priv);

	return data.port;
}

/* On the RTL93xx, an L3 termination endpoint MAC address on which the router waits
 * for packets to be routed needs to be allocated.
 */
static int otto_l3_alloc_router_mac(struct otto_l3_ctrl *ctrl, u64 mac)
{
	struct otto_l3_router_mac m;
	int free_mac = -1;

	mutex_lock(ctrl->lock);
	for (int i = 0; i < MAX_ROUTER_MACS; i++) {
		ctrl->cfg->get_router_mac(ctrl, i, &m);
		if (free_mac < 0 && !m.valid) {
			free_mac = i;
			continue;
		}
		if (m.valid && m.mac == mac) {
			free_mac = i;
			break;
		}
	}

	if (free_mac < 0) {
		dev_err(ctrl->dev, "No free router MACs, cannot offload\n");
		mutex_unlock(ctrl->lock);
		return -1;
	}

	m.valid = true;
	m.mac = mac;
	m.p_type = 0; /* An individual port, not a trunk port */
	m.p_id = 0x3f;			/* Listen on any port */
	m.p_id_mask = 0;
	m.vid = 0;			/* Listen on any VLAN... */
	m.vid_mask = 0;			/* ... so mask needs to be 0 */
	m.mac_mask = 0xffffffffffffULL;	/* We want an exact match of the interface MAC */
	m.action = ROUTE_ACT_FORWARD;	/* Route the packet */
	ctrl->cfg->set_router_mac(ctrl, free_mac, &m);

	mutex_unlock(ctrl->lock);

	return 0;
}

/* Read back an egress interface descriptor, the layout written below. */
static void otto_l3_930x_get_egress_intf(struct otto_l3_ctrl *ctrl, int idx,
					 struct otto_l3_intf *intf)
{
	u32 data[2];

	otto_table_read(RTL9300_TBL_L3_EGR_INTF, idx & 0x7f, &data);

	intf->vid = (data[0] >> 9) & 0xfff;
	intf->smac_idx = (data[0] >> 3) & 0x3f;
	intf->ip4_mtu_id = data[0] & 0x7;

	intf->ip6_mtu_id = (data[1] >> 28) & 0x7;
	intf->ttl_scope = (data[1] >> 20) & 0xff;
	intf->hl_scope = (data[1] >> 12) & 0xff;
	intf->ip4_icmp_redirect = (data[1] >> 9) & 0x7;
	intf->ip6_icmp_redirect = (data[1] >> 6) & 0x7;
	intf->ip4_pbr_icmp_redirect = (data[1] >> 3) & 0x7;
	intf->ip6_pbr_icmp_redirect = data[1] & 0x7;
}

/*
 * Sets up an egress interface for L3 actions Actions for ip4/6_icmp_redirect, ip4/6_pbr_icmp_redirect are:
 * 0: FORWARD, 1: DROP, 2: TRAP2CPU, 3: COPY2CPU, 4: TRAP2MASTERCPU, 5: COPY2MASTERCPU,  6: HARDDROP
 * idx is the index in the HW interface table: idx < 0x80
 */
static void otto_l3_930x_set_egress_intf(struct otto_l3_ctrl *ctrl, int idx, struct otto_l3_intf *intf)
{
	u32 data[2];

	data[0] = (intf->vid & 0xfff) << 9;
	data[0] |= (intf->smac_idx & 0x3f) << 3;
	data[0] |= (intf->ip4_mtu_id & 0x7);

	data[1] = (intf->ip6_mtu_id & 0x7) << 28;
	data[1] |= (intf->ttl_scope & 0xff) << 20;
	data[1] |= (intf->hl_scope & 0xff) << 12;
	data[1] |= (intf->ip4_icmp_redirect & 0x7) << 9;
	data[1] |= (intf->ip6_icmp_redirect & 0x7) << 6;
	data[1] |= (intf->ip4_pbr_icmp_redirect & 0x7) << 3;
	data[1] |= (intf->ip6_pbr_icmp_redirect & 0x7);

	dev_dbg(ctrl->dev, "writing to index %d: %08x %08x\n", idx, data[0], data[1]);
	otto_table_write(RTL9300_TBL_L3_EGR_INTF, idx & 0x7f, &data);
}

/* Configure L3 routing settings of the device:
 * - MTUs
 * - Egress interface
 * - The router's MAC address on which routed packets are expected
 * - MAC addresses used as source macs of routed packets
 */
static int otto_l3_930x_setup(struct otto_l3_ctrl *ctrl)
{
	struct rtl838x_switch_priv *priv = ctrl->priv;

	/* Setup MTU with id 0 for default interface */
	for (int i = 0; i < MAX_INTF_MTUS; i++)
		priv->intf_mtu_count[i] = priv->intf_mtus[i] = 0;

	priv->intf_mtu_count[0] = 0; /* Needs to stay forever */
	priv->intf_mtus[0] = DEFAULT_MTU;
	sw_w32_mask(0xffff, DEFAULT_MTU, RTL930X_L3_IP_MTU_CTRL(0));
	sw_w32_mask(0xffff, DEFAULT_MTU, RTL930X_L3_IP6_MTU_CTRL(0));
	priv->intf_mtus[1] = DEFAULT_MTU;
	sw_w32_mask(0xffff0000, DEFAULT_MTU << 16, RTL930X_L3_IP_MTU_CTRL(0));
	sw_w32_mask(0xffff0000, DEFAULT_MTU << 16, RTL930X_L3_IP6_MTU_CTRL(0));

	sw_w32_mask(0xffff, DEFAULT_MTU, RTL930X_L3_IP_MTU_CTRL(1));
	sw_w32_mask(0xffff, DEFAULT_MTU, RTL930X_L3_IP6_MTU_CTRL(1));
	sw_w32_mask(0xffff0000, DEFAULT_MTU << 16, RTL930X_L3_IP_MTU_CTRL(1));
	sw_w32_mask(0xffff0000, DEFAULT_MTU << 16, RTL930X_L3_IP6_MTU_CTRL(1));

	/* Clear all source port MACs */
	for (int i = 0; i < MAX_SMACS; i++)
		otto_l3_930x_set_egress_mac(ctrl, L3_EGRESS_DMACS + i, 0ULL);

	/* Configure the default L3 hash algorithm */
	sw_w32_mask(BIT(2), 0, RTL930X_L3_HOST_TBL_CTRL);  /* Algorithm selection 0 = 0 */
	sw_w32_mask(0, BIT(3), RTL930X_L3_HOST_TBL_CTRL);  /* Algorithm selection 1 = 1 */

	pr_debug("L3_IPUC_ROUTE_CTRL %08x, IPMC_ROUTE %08x, IP6UC_ROUTE %08x, IP6MC_ROUTE %08x\n",
		 sw_r32(RTL930X_L3_IPUC_ROUTE_CTRL), sw_r32(RTL930X_L3_IPMC_ROUTE_CTRL),
		 sw_r32(RTL930X_L3_IP6UC_ROUTE_CTRL), sw_r32(RTL930X_L3_IP6MC_ROUTE_CTRL));
	/* A packet whose hop count runs out is answered, not dropped: the
	 * action for it is TTL_FAIL_ACT at bit 17 and HL_FAIL_ACT at bit
	 * 21, two bits each, and the Realtek GPL SDK lists the values as
	 * drop, trap to CPU, trap to master CPU in that order
	 * (dal_longan_l3.c, _actIpucRouteCtrlTtlFail and
	 * _actIp6ucRouteCtrlHlFail).
	 */
	sw_w32(0x00022001, RTL930X_L3_IPUC_ROUTE_CTRL);
	sw_w32(0x00214581, RTL930X_L3_IP6UC_ROUTE_CTRL);
	sw_w32(0x00000501, RTL930X_L3_IPMC_ROUTE_CTRL);
	sw_w32(0x00012881, RTL930X_L3_IP6MC_ROUTE_CTRL);

	pr_debug("L3_IPUC_ROUTE_CTRL %08x, IPMC_ROUTE %08x, IP6UC_ROUTE %08x, IP6MC_ROUTE %08x\n",
		 sw_r32(RTL930X_L3_IPUC_ROUTE_CTRL), sw_r32(RTL930X_L3_IPMC_ROUTE_CTRL),
		 sw_r32(RTL930X_L3_IP6UC_ROUTE_CTRL), sw_r32(RTL930X_L3_IP6MC_ROUTE_CTRL));

	/* Trap non-ip traffic to the CPU-port (e.g. ARP so we stay reachable) */
	sw_w32_mask(0x3 << 8, 0x1 << 8, RTL930X_L3_IP_ROUTE_CTRL);
	pr_debug("L3_IP_ROUTE_CTRL %08x\n", sw_r32(RTL930X_L3_IP_ROUTE_CTRL));

	/* PORT_ISO_RESTRICT_ROUTE_CTRL? */

	/* Do not use prefix route 0 because of HW limitations */
	set_bit(0, ctrl->route_use_bm);

	return 0;
}

static int otto_l3_alloc_egress_intf(struct otto_l3_ctrl *ctrl, u64 mac, int vlan)
{
	struct otto_l3_intf intf = {};
	int free_mac = -1;
	u64 m;

	mutex_lock(ctrl->lock);
	for (int i = 0; i < MAX_SMACS; i++) {
		m = ctrl->cfg->get_egress_mac(ctrl, L3_EGRESS_DMACS + i);
		if (free_mac < 0 && !m) {
			free_mac = i;
			continue;
		}
		if (m == mac && ctrl->interfaces[i].vid == vlan) {
			dev_dbg(ctrl->dev, "reusing egress interface %d for VLAN %d\n",
				i, vlan);
			mutex_unlock(ctrl->lock);
			return i;
		}
	}

	if (free_mac < 0) {
		dev_err(ctrl->dev, "No free egress interface, cannot offload\n");
		mutex_unlock(ctrl->lock);
		return -1;
	}

	/* Set up default egress interface 1 */
	intf.vid = vlan;
	intf.smac_idx = free_mac;
	intf.ip4_mtu_id = 1;
	intf.ip6_mtu_id = 1;
	intf.ttl_scope = 1; /* TTL */
	intf.hl_scope = 1;  /* Hop Limit */
	intf.ip4_icmp_redirect = intf.ip6_icmp_redirect = 2;  /* FORWARD */
	intf.ip4_pbr_icmp_redirect = intf.ip6_pbr_icmp_redirect = 2; /* FORWARD; */
	dev_dbg(ctrl->dev, "new egress interface %d for VLAN %d\n", free_mac, vlan);
	ctrl->cfg->set_egress_intf(ctrl, free_mac, &intf);
	ctrl->interfaces[free_mac] = intf;

	ctrl->cfg->set_egress_mac(ctrl, L3_EGRESS_DMACS + free_mac, mac);

	mutex_unlock(ctrl->lock);

	return free_mac;
}

/* Row 0 is not used, see otto_l3_930x_setup(). */
#define FIRST_PREFIX_ROW	1

/* An IPv6 prefix route is matched over three consecutive rows, and only rows
 * 0 and 3 of every eight can hold the first of the three, so two rows in every
 * eight start no route at all. The last row one may start at is four below the
 * last row of the table. The IPv6 rows therefore run downwards from there
 * against the IPv4 rows, which run upwards from FIRST_PREFIX_ROW (Realtek GPL
 * SDK, dal_longan_l3.c: L3_ROUTE_TBL_USED, IS_L3_ROUTE_IPV6_IDX_VALID and
 * L3_ROUTE_IPV6_IDX_MAX).
 */
#define V6_PREFIX_ROWS		3
/* One access at an entry's first row reads or writes the whole entry, so a
 * move copies that row alone
 */
#define V6_MOVE_ROWS		1
#define FIRST_V6_ROW		(MAX_ROUTES - 1 - 4)

static int otto_l3_v6_row(int slot)
{
	return FIRST_V6_ROW - (slot / 2) * 8 - (slot % 2) * V6_PREFIX_ROWS;
}

static int otto_l3_v6_slot(int row)
{
	return 2 * (FIRST_V6_ROW / 8 - row / 8) + (row % 8 ? 0 : 1);
}

/* The programmed prefix routes of one address family sit in one dense block,
 * longest prefix first, because the hardware answers a lookup with the lowest
 * matching row rather than the most specific one. A lookup carries the entry
 * type, so a block is only ever matched against its own family.
 */
static int otto_l3_prefix_rows(struct otto_l3_ctrl *ctrl, u8 type, int at_least)
{
	struct otto_l3_route *q;
	int n = 0;

	list_for_each_entry(q, &ctrl->routes_list, list) {
		if (!q->is_host_route && q->row >= 0 &&
		    q->attr.type == type && q->prefix_len >= at_least)
			n++;
	}

	return n;
}

/* A row move that fails partway leaves the block half shifted: some rows have
 * moved, and the list still names the rows they were at. Every placement and
 * every compaction after that is computed from those names, so the next route
 * placed lands on a row that still holds a live one, and the copy an
 * interrupted move left behind goes on forwarding after the route that made it
 * is gone. The mover reports no progress to renumber from, and the engine that
 * failed is the only way to undo it, so the table is declared unusable and left
 * as it is: what is in it keeps working and can still be removed, and nothing
 * new is placed until the driver is loaded again.
 */
static void otto_l3_rows_stale(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r,
			       int row)
{
	if (ctrl->prefix_rows_stale)
		return;

	ctrl->prefix_rows_stale = true;
	dev_err(ctrl->dev,
		"prefix route %d: row %d not moved, no route will be placed again\n",
		r->id, row);
}

/* Open the row this route belongs at, pushing everything below it down. */
static int otto_l3_route_place(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	int below, rows, row, last, v6_rows, err;
	struct otto_l3_route *q;

	if (!ctrl->cfg->route_rows_move)
		return r->id;

	if (ctrl->prefix_rows_stale)
		return -1;

	/* The local table is looked up before main, so its entries go ahead of
	 * main's of the same length: the hardware answers with the first row
	 * that matches.
	 */
	below = otto_l3_prefix_rows(ctrl, r->attr.type,
				    r->prefix_len + (r->tb_id == RT_TABLE_LOCAL));
	rows = otto_l3_prefix_rows(ctrl, r->attr.type, 0);

	if (r->attr.type == ROUTE_TYPE_IP6UC) {
		int v4_rows = otto_l3_prefix_rows(ctrl, ROUTE_TYPE_IP4UC, 0);

		if (otto_l3_v6_row(rows) < FIRST_PREFIX_ROW + v4_rows) {
			dev_err(ctrl->dev, "prefix route table full, %d IPv4 and %d IPv6 routes\n",
				v4_rows, rows);
			return -1;
		}

		/* Deepest first, so a route is only written over a slot the
		 * route that held it has already left. Once one slot has
		 * moved, a failure leaves the block half shifted whatever the
		 * failing move wrote itself.
		 */
		for (int s = rows - 1; s >= rows - below; s--) {
			err = ctrl->cfg->route_rows_move(ctrl, otto_l3_v6_row(s + 1),
							 otto_l3_v6_row(s), V6_MOVE_ROWS);
			if (err) {
				if (err == -EIO || s < rows - 1)
					otto_l3_rows_stale(ctrl, r, otto_l3_v6_row(s));
				return -1;
			}
		}

		row = otto_l3_v6_row(rows - below);

		list_for_each_entry(q, &ctrl->routes_list, list)
			if (!q->is_host_route && q->attr.type == r->attr.type &&
			    q->row >= FIRST_PREFIX_ROW && q->row <= row)
				q->row = otto_l3_v6_row(otto_l3_v6_slot(q->row) + 1);

		return row;
	}

	v6_rows = otto_l3_prefix_rows(ctrl, ROUTE_TYPE_IP6UC, 0);
	row = FIRST_PREFIX_ROW + below;
	last = FIRST_PREFIX_ROW + rows;

	if (v6_rows && otto_l3_v6_row(v6_rows - 1) <= last) {
		dev_err(ctrl->dev, "prefix route table full, %d IPv4 and %d IPv6 routes\n",
			rows, v6_rows);
		return -1;
	}

	if (row < last) {
		/* A move that fails partway leaves the block half shifted, and
		 * the rows the list names would no longer be the rows that hold
		 * them. One that wrote nothing leaves the table as it was.
		 */
		err = ctrl->cfg->route_rows_move(ctrl, row + 1, row, last - row);
		if (err) {
			if (err == -EIO)
				otto_l3_rows_stale(ctrl, r, row);
			return -1;
		}

		list_for_each_entry(q, &ctrl->routes_list, list)
			if (!q->is_host_route && q->attr.type == r->attr.type &&
			    q->row >= row)
				q->row++;
	}

	return row;
}

/* Close the row this route leaves behind, pulling everything below it up. */
static void otto_l3_route_compact(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	struct otto_l3_route *q;
	int rows, last;

	if (!ctrl->cfg->route_rows_move || r->row < FIRST_PREFIX_ROW ||
	    ctrl->prefix_rows_stale)
		return;

	rows = otto_l3_prefix_rows(ctrl, r->attr.type, 0);

	/* The leaving row is already invalid, so even a move that wrote nothing
	 * leaves a hole in the block that the row count cannot see.
	 */
	if (r->attr.type == ROUTE_TYPE_IP6UC) {
		int slot = otto_l3_v6_slot(r->row);

		last = otto_l3_v6_row(rows - 1);
		if (slot >= rows - 1)
			return;

		for (int s = slot + 1; s < rows; s++)
			if (ctrl->cfg->route_rows_move(ctrl, otto_l3_v6_row(s - 1),
						       otto_l3_v6_row(s), V6_MOVE_ROWS)) {
				otto_l3_rows_stale(ctrl, r, otto_l3_v6_row(s));
				return;
			}

		list_for_each_entry(q, &ctrl->routes_list, list)
			if (!q->is_host_route && q->attr.type == r->attr.type &&
			    q->row >= FIRST_PREFIX_ROW && q->row < r->row)
				q->row = otto_l3_v6_row(otto_l3_v6_slot(q->row) - 1);
	} else {
		last = FIRST_PREFIX_ROW + rows - 1;
		if (r->row >= last)
			return;

		if (ctrl->cfg->route_rows_move(ctrl, r->row, r->row + 1, last - r->row)) {
			otto_l3_rows_stale(ctrl, r, r->row);
			return;
		}

		list_for_each_entry(q, &ctrl->routes_list, list)
			if (!q->is_host_route && q->attr.type == r->attr.type &&
			    q->row > r->row)
				q->row--;
	}

	/* The tail now holds a copy of the row the block has pulled up. */
	r->attr.valid = false;
	ctrl->cfg->route_write(ctrl, last, r);
}

/* %pI4 and %pI6c take arguments of different types, so a message that can
 * name either builds its destination first.
 */
static const char *otto_l3_route_dst(struct otto_l3_route *r, char *buf, size_t len)
{
	if (r->attr.type == ROUTE_TYPE_IP6UC)
		snprintf(buf, len, "%pI6c/%d", &r->dst_ip6, r->prefix_len);
	else
		snprintf(buf, len, "%pI4/%d", &r->dst_ip, r->prefix_len);

	return buf;
}

/* The host table holds one entry per address, and the local table is looked up
 * before main: while an address is the switch's own, the entry is the local
 * route's, and a host route from main for that address keeps out of it.
 */
static bool otto_l3_host_shadowed(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	struct otto_l3_route *q;

	if (!r->is_host_route || r->tb_id == RT_TABLE_LOCAL)
		return false;

	list_for_each_entry(q, &ctrl->routes_list, list)
		if (q != r && q->is_host_route && q->tb_id == RT_TABLE_LOCAL &&
		    q->attr.type == r->attr.type && q->dst_ip == r->dst_ip)
			return true;

	return false;
}

/* The address is the switch's no longer: a host route from main for it gets
 * the entry back, as it last wrote it
 */
static void otto_l3_host_unshadow(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	struct otto_l3_route *q;
	int slot;

	list_for_each_entry(q, &ctrl->routes_list, list) {
		if (q == r || !q->is_host_route || q->tb_id == RT_TABLE_LOCAL ||
		    q->attr.type != r->attr.type || q->dst_ip != r->dst_ip ||
		    !q->attr.valid)
			continue;

		slot = ctrl->cfg->find_slot(ctrl, q, false);
		if (slot >= 0)
			ctrl->cfg->host_route_write(ctrl, slot, q);
		return;
	}
}

static bool otto_l3_fwd_off(struct otto_l3_ctrl *ctrl, u8 type)
{
	return type == ROUTE_TYPE_IP6UC ? ctrl->v6_fwd_off : ctrl->v4_fwd_off;
}

/* Writes a route to its host slot or prefix row, taking a free slot or placing
 * a row when it has none yet. A host route that a local route shadows keeps out
 * of the entry, and nothing is written for it.
 */
static int otto_l3_route_install(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	char dst[INET6_ADDRSTRLEN + sizeof("/128")];
	int slot;

	if (otto_l3_host_shadowed(ctrl, r)) {
		dev_dbg(ctrl->dev, "%pI4 is an address of the switch, its entry stays\n",
			&r->dst_ip);
		return 0;
	}

	if (r->is_host_route) {
		slot = ctrl->cfg->find_slot(ctrl, r, true);
		if (slot < 0)
			slot = ctrl->cfg->find_slot(ctrl, r, false);
		if (slot < 0) {
			dev_err(ctrl->dev, "no slot for host route %pI4\n", &r->dst_ip);
			return -ENOSPC;
		}

		dev_dbg(ctrl->dev, "Got slot for route: %d\n", slot);
		ctrl->cfg->host_route_write(ctrl, slot, r);
		return 0;
	}

	if (r->row < 0)
		r->row = otto_l3_route_place(ctrl, r);
	if (r->row < 0) {
		dev_err(ctrl->dev, "no row for prefix route %s\n",
			otto_l3_route_dst(r, dst, sizeof(dst)));
		return -ENOSPC;
	}

	ctrl->cfg->route_write(ctrl, r->row, r);
	return 0;
}

/* Writes what a route now says where it already is */
static void otto_l3_route_rewrite(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	int slot;

	if (otto_l3_host_shadowed(ctrl, r))
		return;

	if (r->is_host_route) {
		slot = ctrl->cfg->find_slot(ctrl, r, true);
		if (slot >= 0)
			ctrl->cfg->host_route_write(ctrl, slot, r);
	} else if (r->row >= FIRST_PREFIX_ROW) {
		ctrl->cfg->route_write(ctrl, r->row, r);
	}
}

static void otto_l3_route_update_hw(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r,
				    u64 mac)
{
	struct rtl838x_switch_priv *priv = ctrl->priv;
	bool require_existing = ctrl->cfg->use_l3_tables;
	char dst[INET6_ADDRSTRLEN + sizeof("/128")];
	bool trapped = r->attr.action == ROUTE_ACT_TRAP2CPU;
	bool first = !r->nh.mac;
	bool no_port, trap;

	dev_dbg(ctrl->dev, "setting up fwding: gw %pI6c, mac %016llx\n",
		&r->gw_ip, mac);

	dev_dbg(ctrl->dev, "route %d to %s\n",
		r->id, otto_l3_route_dst(r, dst, sizeof(dst)));

	/* A route that forwards and gets a new gateway MAC traps while its
	 * next hop moves over, or it would forward through the L2 entry
	 * rtldsa_l2_nexthop_add() releases. A family that routes through a PIE
	 * rule has no entry that traps.
	 */
	if (ctrl->cfg->use_l3_tables && !first && mac != r->nh.mac &&
	    r->attr.action == ROUTE_ACT_FORWARD) {
		r->attr.action = ROUTE_ACT_TRAP2CPU;
		r->attr.ttl_dec = false;
		r->attr.ttl_check = false;
		otto_l3_route_rewrite(ctrl, r);
	}

	r->nh.mac = r->nh.gw = mac;
	r->nh.port = priv->r->port_ignore;
	r->nh.id = r->id;

	/* Do we need to explicitly add a DMAC entry with the route's nh index? */
	if (ctrl->cfg->set_egress_mac)
		ctrl->cfg->set_egress_mac(ctrl, r->id, mac);

	/* Update ROUTING table: map gateway-mac and switch-mac id to route id */
	if (!rtldsa_l2_nexthop_add(priv, &r->nh, require_existing))
		r->nh.l2_installed = true;

	/* A next hop with no port delivers the frame twice, so where the
	 * route entry can trap, let the CPU route it alone until an update
	 * brings one, the way otto_l3_fib_add_v4() treats a host route
	 * with no gateway. A family that routes through a PIE rule keeps
	 * the rule it had.
	 */
	no_port = ctrl->cfg->use_l3_tables &&
		  r->nh.port == priv->r->port_ignore;
	if (no_port && (first || !trapped)) {
		if (r->attr.type == ROUTE_TYPE_IP4UC)
			dev_info(ctrl->dev, "no port for %pI4, routing %s in software\n",
				 &r->gw_ip.s6_addr32[3], otto_l3_route_dst(r, dst, sizeof(dst)));
		else
			dev_info(ctrl->dev, "no port for %pI6c, routing %s in software\n",
				 &r->gw_ip, otto_l3_route_dst(r, dst, sizeof(dst)));
	}

	trap = no_port || otto_l3_fwd_off(ctrl, r->attr.type);

	r->attr.valid = true;
	r->attr.action = trap ? ROUTE_ACT_TRAP2CPU : ROUTE_ACT_FORWARD;
	r->attr.hit = false; /* Reset route-used indicator */

	/* Forwarding a packet is what makes this a hop, and a hop
	 * spends one of the packet's. The two bits are what the SDK
	 * asks for on an entry it creates with no flags of its own.
	 * A route that traps does not forward, so it keeps them clear.
	 */
	r->attr.ttl_dec = !trap;
	r->attr.ttl_check = !trap;

	if (r->attr.type == ROUTE_TYPE_IP4UC) {
		/* Add PIE entry with dst_ip and prefix_len */
		r->pr.dip = r->dst_ip;
		r->pr.dip_m = inet_make_mask(r->prefix_len);
	}

	/* The next hop is in place before the entry that points at it */
	if (ctrl->cfg->set_nexthop)
		ctrl->cfg->set_nexthop(ctrl, r->nh.id, r->nh.l2_id, r->nh.if_id);

	if (otto_l3_route_install(ctrl, r))
		return;

	if (!r->is_host_route) {
		r->pr.fwd_sel = true;
		r->pr.fwd_data = r->nh.l2_id;
		r->pr.fwd_act = PIE_ACT_ROUTE_UC;
	}

	if (ctrl->cfg->use_l3_tables)
		return;

	if (r->pr.id < 0) {
		r->pr.packet_cntr = rtldsa_packet_cntr_alloc(priv);
		if (r->pr.packet_cntr >= 0) {
			dev_info(ctrl->dev, "Using packet counter %d\n",
				 r->pr.packet_cntr);
			r->pr.log_sel = true;
			r->pr.log_data = r->pr.packet_cntr;
		}
		priv->r->pie_rule_add(priv, &r->pr);
	} else {
		int pkts = priv->r->packet_cntr_read(priv, r->pr.packet_cntr);

		dev_dbg(ctrl->dev, "total packets: %d\n", pkts);

		priv->r->pie_rule_write(priv, r->pr.id, &r->pr);
	}
}

/* The hardware would go on forwarding to the address the gateway last
 * answered from. The CPU resolves it again or reports it unreachable, and the
 * next update with a valid neighbour gives the route back. A family that
 * routes through a PIE rule keeps the rule it had.
 */
static void otto_l3_route_trap_hw(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	char dst[INET6_ADDRSTRLEN + sizeof("/128")];
	int slot = r->row;

	if (!ctrl->cfg->use_l3_tables || r->attr.action == ROUTE_ACT_TRAP2CPU)
		return;

	/* What the route says is what the next write of it puts in hardware */
	r->attr.action = ROUTE_ACT_TRAP2CPU;
	r->attr.ttl_dec = false;
	r->attr.ttl_check = false;

	if (r->is_host_route)
		slot = ctrl->cfg->find_slot(ctrl, r, true);

	/* Not in the hardware, so nothing forwards to the old address */
	if (slot < 0)
		return;

	if (r->attr.type == ROUTE_TYPE_IP4UC)
		dev_info(ctrl->dev, "no valid neighbour for %pI4, routing %s in software\n",
			 &r->gw_ip.s6_addr32[3], otto_l3_route_dst(r, dst, sizeof(dst)));
	else
		dev_info(ctrl->dev, "no valid neighbour for %pI6c, routing %s in software\n",
			 &r->gw_ip, otto_l3_route_dst(r, dst, sizeof(dst)));

	otto_l3_route_rewrite(ctrl, r);
}

/* Updates an L3 next hop entry in the ROUTING table */
static int otto_l3_nexthop_update(struct otto_l3_ctrl *ctrl, u8 type, int ifindex,
				  const struct in6_addr *gw, u64 mac, bool valid)
{
	struct otto_l3_route *r;
	bool known = false;

	list_for_each_entry(r, &ctrl->routes_list, list) {
		if (!ipv6_addr_equal(&r->gw_ip, gw))
			continue;

		known = true;

		/* An IPv4 gateway written v4-mapped is a valid IPv6 one, so
		 * the address alone does not say whose route this is.
		 */
		if (r->attr.type != type || r->gw_ifindex != ifindex)
			continue;

		if (valid)
			otto_l3_route_update_hw(ctrl, r, mac);
		else
			otto_l3_route_trap_hw(ctrl, r);
	}

	if (!known)
		return -ENOENT;

	/* An address of one family can be written as an address of the other,
	 * so a neighbour of the wrong family reaches this far without having
	 * placed anything, and has nothing to report.
	 */
	if (type != ROUTE_TYPE_IP6UC)
		return 0;

	/* Reporting the offload allocates and can send a netlink message. It
	 * takes no lock of its own, so unlike the FIB work this path needs no
	 * rtnl. The FIB notifier is registered on init_net, which is where
	 * every route here comes from.
	 */
	list_for_each_entry(r, &ctrl->routes_list, list) {
		bool trap;

		if (!IS_REACHABLE(CONFIG_IPV6) || !r->f6i || r->gw_ifindex != ifindex ||
		    !ipv6_addr_equal(&r->gw_ip, gw))
			continue;

		/* A route takes its row when it is added, so one still without a
		 * row is one that could not be placed.
		 */
		trap = r->attr.action == ROUTE_ACT_TRAP2CPU;
		if (r->row >= FIRST_PREFIX_ROW)
			fib6_info_hw_flags_set(&init_net, r->f6i, !trap, trap, false);
		else
			fib6_info_hw_flags_set(&init_net, r->f6i, false, false, true);
	}

	return 0;
}

static int otto_l3_port_gw_resolve(struct otto_l3_ctrl *ctrl, struct net_device *dev,
				   struct neigh_table *tbl, const struct in6_addr *gw)
{
	/* An ARP neighbour is keyed on four bytes, which in a v4-mapped
	 * address are the last word of it.
	 */
	const void *key = tbl == &arp_tbl ? (const void *)&gw->s6_addr32[3] : gw;
	u8 type = tbl == &arp_tbl ? ROUTE_TYPE_IP4UC : ROUTE_TYPE_IP6UC;
	struct neighbour *n = neigh_lookup(tbl, key, dev);
	int err = 0;
	u64 mac;

	if (!n) {
		n = neigh_create(tbl, key, dev);
		if (IS_ERR(n))
			return PTR_ERR(n);
	}

	/* If the neigh is already resolved, then go ahead and
	 * install the entry, otherwise start the resolution.
	 */
	if (n->nud_state & NUD_VALID) {
		mac = ether_addr_to_u64(n->ha);
		dev_info(ctrl->dev, "resolved mac: %016llx\n", mac);
		otto_l3_nexthop_update(ctrl, type, dev->ifindex, gw, mac, true);
	} else {
		dev_info(ctrl->dev, "need to wait\n");
		neigh_event_send(n, NULL);
	}

	neigh_release(n);

	return err;
}

/* The hardware search keys on the masked destination, so a shorter prefix that
 * covers the same address answers as well. The SDK reads the entry back and
 * compares it with the route it asked for before touching it.
 */
static bool otto_l3_route_is_at(struct otto_l3_ctrl *ctrl, int id, struct otto_l3_route *r)
{
	struct otto_l3_route entry;

	if (id < FIRST_PREFIX_ROW)
		return false;

	ctrl->cfg->route_read(ctrl, id, &entry);
	/* The next hop index a row carries is the one the route wrote: its
	 * own id once its gateway answered, and zero on a row that only
	 * traps, which the SDK keeps for exactly that. Comparing it with the
	 * route's own next hop tells two routes for one destination apart,
	 * whatever their action.
	 * Both are read after the type: the reader leaves them untouched on a
	 * multicast row, and the type comparison is what stops it being read
	 * there.
	 */
	if (!entry.attr.valid || entry.attr.type != r->attr.type ||
	    entry.prefix_len != r->prefix_len ||
	    entry.attr.action != r->attr.action ||
	    entry.nh.id != r->nh.id)
		return false;

	switch (r->attr.type) {
	case ROUTE_TYPE_IP4UC:
		return entry.dst_ip == r->dst_ip;
	case ROUTE_TYPE_IP6UC:
		return ipv6_addr_equal(&entry.dst_ip6, &r->dst_ip6);
	}

	return false;
}

static struct otto_l3_route *otto_l3_route_find(struct otto_l3_ctrl *ctrl, u32 tb_id, u8 type,
						u32 dst_ip, const struct in6_addr *dst_ip6,
						int prefix_len)
{
	struct otto_l3_route *r;

	list_for_each_entry(r, &ctrl->routes_list, list) {
		if (r->tb_id != tb_id || r->attr.type != type ||
		    r->prefix_len != prefix_len)
			continue;

		switch (type) {
		case ROUTE_TYPE_IP4UC:
			if (r->dst_ip == dst_ip)
				return r;
			break;
		case ROUTE_TYPE_IP6UC:
			if (ipv6_addr_equal(&r->dst_ip6, dst_ip6))
				return r;
			break;
		}
	}

	return NULL;
}

/* One source of the source-specific routes a trap row stands for */
struct otto_l3_route_src {
	struct list_head list;
	struct in6_addr addr;
	int plen;
	unsigned int members;
};

static void otto_l3_route_free(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	struct otto_l3_route_src *s, *tmp;

	list_for_each_entry_safe(s, tmp, &r->srcs, list)
		kfree(s);

	if (r->is_host_route)
		clear_bit(r->id - MAX_ROUTES, ctrl->host_route_use_bm);
	else
		clear_bit(r->id, ctrl->route_use_bm);

	if (IS_REACHABLE(CONFIG_IPV6) && r->f6i) {
		fib6_info_hw_flags_set(&init_net, r->f6i, false, false, false);
		fib6_info_release(r->f6i);
	}

	list_del(&r->list);
	kfree(r);
}

static void otto_l3_route_remove(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	char dst[INET6_ADDRSTRLEN + sizeof("/128")];
	int id;

	if (otto_l3_host_shadowed(ctrl, r)) {
		/* The entry for the address is the local route's */
	} else if (r->is_host_route) {
		id = ctrl->cfg->find_slot(ctrl, r, true);
		if (id >= 0) {
			dev_dbg(ctrl->dev, "Got id for host route: %d\n", id);
			r->attr.valid = false;
			ctrl->cfg->host_route_write(ctrl, id, r);
		} else {
			dev_err(ctrl->dev, "Host route %pI4 was not in hardware\n",
				&r->dst_ip);
		}

		if (r->tb_id == RT_TABLE_LOCAL && !r->replaced)
			otto_l3_host_unshadow(ctrl, r);
	} else {
		/* If there is a HW representation of the route, delete it */
		if (ctrl->cfg->route_lookup_hw && r->row >= FIRST_PREFIX_ROW) {
			/* The route was written at the row we recorded, and a
			 * route whose gateway never resolved has none. Ask the
			 * hardware when it is not where we put it.
			 */
			id = r->row;
			if (!otto_l3_route_is_at(ctrl, id, r)) {
				id = ctrl->cfg->route_lookup_hw(ctrl, r);
				if (id != -ETIMEDOUT && !otto_l3_route_is_at(ctrl, id, r)) {
					if (id >= FIRST_PREFIX_ROW)
						dev_err(ctrl->dev,
							"prefix route %s: row %d holds another route\n",
							otto_l3_route_dst(r, dst, sizeof(dst)), id);
					id = -1;
				}
			}

			if (id >= FIRST_PREFIX_ROW) {
				dev_dbg(ctrl->dev, "Got id for prefix route: %d\n", id);
				r->attr.valid = false;
				ctrl->cfg->route_write(ctrl, id, r);
			} else if (id == -ETIMEDOUT) {
				/* The route may still hold a row that the block,
				 * counted from the list, will no longer include
				 */
				ctrl->prefix_rows_stale = true;
				dev_err(ctrl->dev,
					"prefix route %s: hardware lookup timed out, no route will be placed again\n",
					otto_l3_route_dst(r, dst, sizeof(dst)));
			} else {
				dev_err(ctrl->dev, "prefix route %s was not in hardware\n",
					otto_l3_route_dst(r, dst, sizeof(dst)));
			}

			/* The block closes up over the row the route was
			 * really at, which is not always the one recorded.
			 */
			r->row = id;
			otto_l3_route_compact(ctrl, r);
		}
	}
}

static void otto_l3_route_teardown(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	struct rtl838x_switch_priv *priv = ctrl->priv;

	/* What forwards through the route goes before the next hop it forwards
	 * to. A route whose gateway never resolved holds neither the next hop
	 * nor the PIE rule: otto_l3_nexthop_update() is what allocates them,
	 * and it may have programmed the next hop without reaching the rule.
	 */
	if (r->pr.id >= 0)
		priv->r->pie_rule_rm(priv, &r->pr);
	otto_l3_route_remove(ctrl, r);

	if (r->nh.l2_installed)
		rtldsa_l2_nexthop_del(priv, &r->nh);

	dev_dbg(ctrl->dev, "releasing packet counter %d\n", r->pr.packet_cntr);
	rtldsa_packet_cntr_free(priv, r->pr.packet_cntr);

	otto_l3_route_free(ctrl, r);
}

static struct otto_l3_route *otto_l3_route_alloc(struct otto_l3_ctrl *ctrl,
						 const struct in6_addr *gw, int kind)
{
	bool host = kind == ROUTE_HOST;
	unsigned long *use_bm = host ? ctrl->host_route_use_bm : ctrl->route_use_bm;
	int size = host ? MAX_HOST_ROUTES : MAX_ROUTES;
	struct otto_l3_route *r;
	int idx;

	mutex_lock(ctrl->lock);

	idx = find_first_zero_bit(use_bm, size);
	if (idx >= size) {
		dev_err(ctrl->dev, "%s route table full, %d entries in use\n",
			host ? "host" : "prefix", size);
		mutex_unlock(ctrl->lock);
		return NULL;
	}
	dev_dbg(ctrl->dev, "id: %d, gw %pI6c\n", idx, gw);

	r = kzalloc(sizeof(*r), GFP_KERNEL);
	if (!r) {
		mutex_unlock(ctrl->lock);
		return NULL;
	}

	/* We require a unique route ID irrespective of whether it is a prefix or host
	 * route (on RTL93xx) as we use this ID to associate a DMAC and next-hop entry
	 */
	r->id = host ? idx + MAX_ROUTES : idx;
	r->row = -1;	/* no row until placed; a host route never has one */
	r->gw_ip = *gw;
	r->pr.id = -1; /* We still need to allocate a rule in HW */
	r->pr.packet_cntr = -1;
	r->is_host_route = host;
	INIT_LIST_HEAD(&r->srcs);

	list_add_tail(&r->list, &ctrl->routes_list);
	set_bit(idx, use_bm);

	mutex_unlock(ctrl->lock);

	return r;
}

static int otto_l3_fib_check_v4(struct otto_l3_ctrl *ctrl,
				struct fib_entry_notifier_info *info,
				enum fib_event_type event)
{
	struct net_device *ndev = fib_info_nh(info->fi, 0)->fib_nh_dev;
	int vlan = is_vlan_dev(ndev) ? vlan_dev_vlan_id(ndev) : 0;
	struct fib_nh *nh = fib_info_nh(info->fi, 0);
	char gw_message[32] = "";

	if (nh->fib_nh_gw4)
		snprintf(gw_message, sizeof(gw_message), "via %pI4 ", &nh->fib_nh_gw4);

	dev_info(ctrl->dev, "%s IPv4 route %pI4/%d %s(VLAN %d, MAC %pM)\n",
		 event == FIB_EVENT_ENTRY_ADD ? "add" : "delete",
		 &info->dst, info->dst_len, gw_message, vlan, ndev->dev_addr);

	if ((info->type == RTN_BROADCAST) || ipv4_is_loopback(info->dst) || !info->dst) {
		dev_warn(ctrl->dev, "skip loopback/broadcast addresses and default routes\n");
		return -EINVAL;
	}

	return 0;
}

static void otto_l3_host_route_trap(struct otto_l3_ctrl *ctrl,
				    struct fib_entry_notifier_info *info)
{
	struct otto_l3_route *route;
	struct in6_addr gw;

	ipv6_addr_set_v4mapped(0, &gw);
	route = otto_l3_route_alloc(ctrl, &gw, ROUTE_HOST);
	if (!route)
		return;

	route->dst_ip = info->dst;
	route->prefix_len = info->dst_len;
	route->tb_id = info->tb_id;
	route->attr.valid = true;
	route->attr.action = ROUTE_ACT_TRAP2CPU;
	route->attr.type = ROUTE_TYPE_IP4UC;

	if (otto_l3_route_install(ctrl, route))
		otto_l3_route_free(ctrl, route);
}

/* A route this driver leaves out still has to keep a shorter prefix in
 * hardware from forwarding its traffic, so it gets an entry that traps: a /32
 * in the host table, anything shorter a row of its own. The default route gets
 * none, since its row would match every destination; the catch-all row traps
 * what nothing else matches.
 */
static void otto_l3_fib_trap_v4(struct otto_l3_ctrl *ctrl, struct fib_entry_notifier_info *info)
{
	struct otto_l3_route *route;
	struct in6_addr gw;

	if (!ctrl->cfg->use_l3_tables || !info->dst_len)
		return;

	if (info->dst_len == 32) {
		otto_l3_host_route_trap(ctrl, info);
		return;
	}

	ipv6_addr_set_v4mapped(0, &gw);
	route = otto_l3_route_alloc(ctrl, &gw, ROUTE_PREFIX);
	if (!route) {
		dev_err(ctrl->dev, "no row to trap %pI4/%d\n", &info->dst, info->dst_len);
		return;
	}

	route->dst_ip = info->dst;
	route->prefix_len = info->dst_len;
	route->tb_id = info->tb_id;
	route->attr.type = ROUTE_TYPE_IP4UC;
	route->attr.action = ROUTE_ACT_TRAP2CPU;
	route->attr.valid = true;

	route->row = otto_l3_route_place(ctrl, route);
	if (route->row < FIRST_PREFIX_ROW) {
		dev_err(ctrl->dev, "no row to trap %pI4/%d\n", &info->dst, info->dst_len);
		otto_l3_route_teardown(ctrl, route);
		return;
	}

	ctrl->cfg->route_write(ctrl, route->row, route);
}

static void otto_l3_route_trap_new(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	r->attr.valid = true;
	r->attr.action = ROUTE_ACT_TRAP2CPU;

	otto_l3_route_install(ctrl, r);
}

static int otto_l3_fib_add_v4(struct otto_l3_ctrl *ctrl, struct fib_entry_notifier_info *info)
{
	struct rtl838x_switch_priv *priv = ctrl->priv;
	struct otto_l3_route *route;
	struct net_device *ndev;
	struct in6_addr gw;
	struct fib_nh *nh;
	int port, vlan;

	/* A route through a nexthop object has no nexthop array to read, a
	 * blackhole, unreachable or prohibit route has no device behind its
	 * nexthop, a route with a lightweight tunnel (seg6, MPLS) needs the
	 * CPU to encapsulate what the hardware would forward bare, a multipath
	 * route would be forwarded through its first next hop alone, and a route
	 * for one DSCP value would carry every other value too, since the entry
	 * matches on the destination only. None is offloaded, but any can
	 * replace a route that is.
	 */
	if (info->fi->nh || !fib_info_nh(info->fi, 0)->fib_nh_dev ||
	    fib_info_nh(info->fi, 0)->fib_nh_lws || fib_info_num_path(info->fi) > 1 ||
	    info->dscp) {
		dev_dbg(ctrl->dev,
			"route not offloaded: no device, nexthop object, tunnel, ECMP or DSCP\n");
		route = otto_l3_route_find(ctrl, info->tb_id, ROUTE_TYPE_IP4UC, info->dst,
					   NULL, info->dst_len);
		if (route) {
			route->replaced = true;
			otto_l3_route_teardown(ctrl, route);
		}
		otto_l3_fib_trap_v4(ctrl, info);
		return 0;
	}

	nh = fib_info_nh(info->fi, 0);
	ndev = nh->fib_nh_dev;
	vlan = is_vlan_dev(ndev) ? vlan_dev_vlan_id(ndev) : 0;

	if (otto_l3_fib_check_v4(ctrl, info, FIB_EVENT_ENTRY_ADD))
		return 0;

	/* Every add that reaches the driver arrives as a replace, so a route
	 * for this destination may already be programmed. Take it out first,
	 * before anything can turn the new one down: deleting the programmed
	 * route arrives as a replace too, carrying its successor.
	 */
	route = otto_l3_route_find(ctrl, info->tb_id, ROUTE_TYPE_IP4UC, info->dst, NULL,
				   info->dst_len);
	if (route) {
		dev_dbg(ctrl->dev, "replacing route %pI4/%d, id %d\n",
			&info->dst, info->dst_len, route->id);
		/* A route to the same destination takes the entry over: a
		 * main /32 kept out of it stays out while it changes hands
		 */
		route->replaced = true;
		otto_l3_route_teardown(ctrl, route);
	}

	/* A route through a device outside the switch is the CPU's, and so is
	 * an address of the box on such a device
	 */
	port = otto_l3_port_dev_lower_find(ndev, ctrl);
	if (port < 0) {
		dev_err(ctrl->dev, "lower interface %s not found\n", ndev->name);
		otto_l3_fib_trap_v4(ctrl, info);
		return -ENODEV;
	}

	/* Allocate route or host-route entry (if hardware supports this) */
	ipv6_addr_set_v4mapped(nh->fib_nh_gw4, &gw);
	route = otto_l3_route_alloc(ctrl, &gw,
				    info->dst_len == 32 && ctrl->cfg->host_route_write ?
				    ROUTE_HOST : ROUTE_PREFIX);

	if (route)
		dev_info(ctrl->dev, "route allocated for gw %pI4\n", &nh->fib_nh_gw4);
	else {
		dev_err(ctrl->dev, "could not allocate a route for gw %pI4\n",
			&nh->fib_nh_gw4);
		return -ENOSPC;
	}

	route->dst_ip = info->dst;
	route->prefix_len = info->dst_len;
	route->tb_id = info->tb_id;
	route->attr.type = ROUTE_TYPE_IP4UC;
	route->nh.rvid = vlan;
	route->gw_ifindex = ndev->ifindex;

	if (ctrl->cfg->set_router_mac) {
		u64 mac = ether_addr_to_u64(ndev->dev_addr);

		dev_dbg(ctrl->dev, "Local route and router MAC %pM\n", ndev->dev_addr);
		if (otto_l3_alloc_router_mac(ctrl, mac))
			goto out_free_rt;

		/* vid = 0: Do not care about VID */
		route->nh.if_id = otto_l3_alloc_egress_intf(ctrl, mac, vlan);
		if (route->nh.if_id < 0)
			goto out_free_rmac;

		if (!nh->fib_nh_gw4 && route->is_host_route) {
			route->nh.mac = mac;
			route->nh.port = priv->r->port_ignore;
			route->attr.valid = true;
			route->attr.action = ROUTE_ACT_TRAP2CPU;
			route->attr.type = ROUTE_TYPE_IP4UC;

			if (otto_l3_route_install(ctrl, route))
				goto out_free_rt;
		}
	}

	/* Until the gateway answers, or for good where there is none, a shorter
	 * prefix in hardware would forward this destination. The route takes
	 * its entry now, trapping to the CPU, and otto_l3_route_update_hw()
	 * turns it into a forwarding one once the gateway resolves. The next
	 * hop index stays zero until then, as on any entry that only traps.
	 */
	if (ctrl->cfg->use_l3_tables && (nh->fib_nh_gw4 || !route->is_host_route))
		otto_l3_route_trap_new(ctrl, route);

	/* We need to resolve the mac address of the GW */
	if (nh->fib_nh_gw4)
		otto_l3_port_gw_resolve(ctrl, ndev, &arp_tbl, &gw);

	nh->fib_nh_flags |= RTNH_F_OFFLOAD;

	return 0;

out_free_rmac:
out_free_rt:
	otto_l3_route_free(ctrl, route);
	return 0;
}

static int otto_l3_fib_del_v4(struct otto_l3_ctrl *ctrl, struct fib_entry_notifier_info *info)
{
	struct otto_l3_route *route;
	struct in6_addr gw;
	struct fib_nh *nh;

	/* A route through a nexthop object, without a device, with a
	 * lightweight tunnel, with several paths or for one DSCP value holds at
	 * most a trap entry
	 */
	if (info->fi->nh || !fib_info_nh(info->fi, 0)->fib_nh_dev ||
	    fib_info_nh(info->fi, 0)->fib_nh_lws || fib_info_num_path(info->fi) > 1 ||
	    info->dscp) {
		route = otto_l3_route_find(ctrl, info->tb_id, ROUTE_TYPE_IP4UC, info->dst,
					   NULL, info->dst_len);
		if (route)
			otto_l3_route_teardown(ctrl, route);
		return 0;
	}

	nh = fib_info_nh(info->fi, 0);

	if (otto_l3_fib_check_v4(ctrl, info, FIB_EVENT_ENTRY_DEL))
		return 0;

	/* The entry that traps for a route left out carries no gateway,
	 * whatever gateway the route itself names
	 */
	route = otto_l3_route_find(ctrl, info->tb_id, ROUTE_TYPE_IP4UC, info->dst, NULL,
				   info->dst_len);
	if (ctrl->cfg->use_l3_tables && route && !route->gw_ip.s6_addr32[3]) {
		otto_l3_route_teardown(ctrl, route);
		nh->fib_nh_flags &= ~RTNH_F_OFFLOAD;
		return 0;
	}

	ipv6_addr_set_v4mapped(nh->fib_nh_gw4, &gw);

	if (!route || !ipv6_addr_equal(&route->gw_ip, &gw)) {
		dev_err(ctrl->dev, "no route %pI4/%d via %pI4\n",
			&info->dst, info->dst_len, &nh->fib_nh_gw4);
		return -ENOENT;
	}
	dev_info(ctrl->dev, "found a route with id %d, nh-id %d\n",
		 route->id, route->nh.id);

	otto_l3_route_teardown(ctrl, route);

	nh->fib_nh_flags &= ~RTNH_F_OFFLOAD;

	return 0;
}

/* A route that carries a nexthop object keeps its next hop elsewhere, and its
 * next hop array is not allocated behind it at all, so nothing that reads
 * one may run before the object itself has been ruled out.
 *
 * The IPv4 twin of this narrates every event at info level. IPv6 routes
 * arrive in numbers IPv4 ones do not - every router advertisement brings
 * some - so this one speaks at debug level instead.
 *
 * The members are the ones the notifier counted. The route's own sibling
 * count is read later, and a member deleted in between has had it cleared,
 * so it would pass for a route of its own.
 */
static int otto_l3_fib_check_v6(struct otto_l3_ctrl *ctrl, struct fib6_info *rt,
				unsigned int members)
{
	dev_dbg(ctrl->dev, "IPv6 route %pI6c/%d, type %d, flags %x, members %u\n",
		&rt->fib6_dst.addr, rt->fib6_dst.plen, rt->fib6_type,
		rt->fib6_flags, members);

	if (rt->nh)
		return -EOPNOTSUPP;

	if (rt->fib6_src.plen || members > 1)
		return -EOPNOTSUPP;

	if (rt->fib6_type != RTN_UNICAST || rt->fib6_flags & RTF_REJECT)
		return -EOPNOTSUPP;

	/* Routers advertise these, and the kernel picks among them at lookup
	 * time, with no event when its pick changes
	 */
	if (rt->fib6_flags & RTF_ADDRCONF)
		return -EOPNOTSUPP;

	/* A row for the default route matches every destination no more
	 * specific row holds, and the destinations this driver leaves out are
	 * exactly the ones it does not know what to do with - a prefix that is
	 * on-link somewhere else among them. Sending those to the gateway is
	 * worse than trapping them, which is what the catch-all row does with
	 * them.
	 */
	if (ipv6_addr_any(&rt->fib6_dst.addr) || ipv6_addr_loopback(&rt->fib6_dst.addr))
		return -EOPNOTSUPP;

	if (rt->fib6_nh->fib_nh_gw_family != AF_INET6)
		return -EOPNOTSUPP;

	/* The hardware would forward bare what the tunnel has to encapsulate */
	if (rt->fib6_nh->fib_nh_lws)
		return -EOPNOTSUPP;

	return 0;
}

/* An address of the switch itself has no gateway to resolve, and the hardware
 * keeps no notion of its own addresses: the router MAC table it terminates on
 * carries no IP at all. Without a row that traps them, packets to one of those
 * addresses are looked up as routes, miss, and are dropped - which is what the
 * IPUC_ROUTING_LOOKUP_MISS drop counter counts. The IPv4 path writes the same
 * kind of entry, into the host route table.
 *
 * An address the box holds on several devices at once - the link-local of a
 * bridge is also the link-local of its VLAN interfaces and of the conduit -
 * reaches the driver once, for whichever of them the kernel made the leaf of
 * that destination. That device is not necessarily one of ours, so which
 * device holds an address says nothing about whether it has to be trapped:
 * every address of the box does, and each costs an entry, which is three
 * rows.
 */
static int otto_l3_fib_trap_v6(struct otto_l3_ctrl *ctrl, struct fib6_info *rt,
			       unsigned int members)
{
	struct otto_l3_route *route;

	/* While a copy of the address remains the kernel replaces the entry
	 * rather than deleting it, so a row already trapping here is the row
	 * this event asks for. Any other row for the destination belongs to
	 * the route this one replaces: an address of the switch arrives before
	 * the replace path in otto_l3_fib_add_v6() has taken that out.
	 */
	route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
				   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);
	if (route && ipv6_addr_any(&route->gw_ip)) {
		route->members = members;
		return 0;
	}
	if (route)
		otto_l3_route_teardown(ctrl, route);

	route = otto_l3_route_alloc(ctrl, &in6addr_any, ROUTE_PREFIX);
	if (!route) {
		dev_err(ctrl->dev, "no row to trap %pI6c/%d\n",
			&rt->fib6_dst.addr, rt->fib6_dst.plen);
		return -ENOSPC;
	}

	route->dst_ip6 = rt->fib6_dst.addr;
	route->prefix_len = rt->fib6_dst.plen;
	route->tb_id = rt->fib6_table->tb6_id;
	route->attr.type = ROUTE_TYPE_IP6UC;
	route->attr.action = ROUTE_ACT_TRAP2CPU;
	route->attr.valid = true;
	route->members = members;

	route->row = otto_l3_route_place(ctrl, route);
	if (route->row < FIRST_PREFIX_ROW) {
		otto_l3_route_teardown(ctrl, route);
		return -ENOSPC;
	}

	/* The next hop index stays zero, which is what the hardware reads as
	 * "no next hop" on a route that only traps.
	 */
	ctrl->cfg->route_write(ctrl, route->row, route);
	dev_dbg(ctrl->dev, "%pI6c/%d trapped at row %d\n",
		&rt->fib6_dst.addr, rt->fib6_dst.plen, route->row);

	return 0;
}

static int otto_l3_fib_add_v6_local(struct otto_l3_ctrl *ctrl, struct fib6_info *rt)
{
	if (rt->nh || ipv6_addr_loopback(&rt->fib6_dst.addr))
		return 0;

	return otto_l3_fib_trap_v6(ctrl, rt, 1);
}

static int otto_l3_fib_del_v6_local(struct otto_l3_ctrl *ctrl, struct fib6_info *rt)
{
	struct otto_l3_route *route;

	route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
				   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);
	if (!route)
		return 0;

	otto_l3_route_teardown(ctrl, route);

	return 0;
}

static struct otto_l3_route_src *otto_l3_route_src_find(struct otto_l3_route *r,
							 const struct rt6key *src)
{
	struct otto_l3_route_src *s;

	list_for_each_entry(s, &r->srcs, list)
		if (s->plen == src->plen && ipv6_addr_equal(&s->addr, &src->addr))
			return s;

	return NULL;
}

/* A source that could not be tracked is still a route the row stands for */
static bool otto_l3_route_has_srcs(struct otto_l3_route *r)
{
	return !list_empty(&r->srcs) || r->srcs_incomplete;
}

static void otto_l3_resync_request(struct otto_l3_ctrl *ctrl);

/* A source-specific route matches only the packets from its source, and the
 * hardware matches on the destination alone, so the destination traps for as
 * long as one of them remains. The trap row counts the members of each source
 * apart, and those of the route without a source in its own members: each
 * delete names one source, and only empties its own count.
 */
static int otto_l3_fib_add_v6_src(struct otto_l3_ctrl *ctrl, struct fib6_info *rt,
				  unsigned int members)
{
	struct otto_l3_route_src *s;
	struct otto_l3_route *route;
	unsigned int plain = 0;
	int err;

	if (!rt->fib6_dst.plen || rt->fib6_type == RTN_MULTICAST)
		return 0;

	/* A row that forwards here is the route without a source, which the
	 * trap row stands for from now on
	 */
	route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
				   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);
	if (route && !ipv6_addr_any(&route->gw_ip)) {
		dev_info(ctrl->dev, "route %pI6c/%d is no longer offloaded\n",
			 &rt->fib6_dst.addr, rt->fib6_dst.plen);
		otto_l3_route_teardown(ctrl, route);
		route = NULL;
		plain = 1;
	}

	if (!route) {
		err = otto_l3_fib_trap_v6(ctrl, rt, plain);
		if (err)
			return err;
		route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
					   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);
		if (!route)
			return -ENOENT;
	}

	s = otto_l3_route_src_find(route, &rt->fib6_src);
	if (!s) {
		s = kzalloc(sizeof(*s), GFP_KERNEL);
		if (!s) {
			/* The row keeps trapping whatever the list says; a replay
			 * builds the list again
			 */
			route->srcs_incomplete = true;
			dev_err(ctrl->dev, "%pI6c/%d from %pI6c/%d: no memory to track the source\n",
				&rt->fib6_dst.addr, rt->fib6_dst.plen,
				&rt->fib6_src.addr, rt->fib6_src.plen);
			otto_l3_resync_request(ctrl);
			return -ENOMEM;
		}
		s->addr = rt->fib6_src.addr;
		s->plen = rt->fib6_src.plen;
		list_add_tail(&s->list, &route->srcs);
	}
	s->members = members;

	return 0;
}

static int otto_l3_fib_del_v6_src(struct otto_l3_ctrl *ctrl, struct fib6_info *rt,
				  unsigned int members)
{
	struct otto_l3_route_src *s;
	struct otto_l3_route *route;

	route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
				   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);
	if (!route || !ipv6_addr_any(&route->gw_ip))
		return 0;

	s = otto_l3_route_src_find(route, &rt->fib6_src);
	if (!s)
		return 0;

	if (s->members > members) {
		s->members -= members;
		return 0;
	}

	list_del(&s->list);
	kfree(s);

	if (!otto_l3_route_has_srcs(route) && !route->members)
		otto_l3_route_teardown(ctrl, route);

	return 0;
}

/* A destination with no row of its own misses the route lookup, and the miss
 * is a drop the hardware gives no way to change: the Realtek GPL SDK carries a
 * lookup miss action in the multicast route controls and in neither of the
 * unicast ones (rtk_longan_regField_list.c, L3_IP6MC_ROUTE_CTRL against
 * L3_IP6UC_ROUTE_CTRL). The router MAC matches on any port and any VLAN, so
 * that drop takes every IP frame addressed to the switch whose destination
 * this driver did not program - an on-link prefix among them, since a route
 * with no gateway to resolve is never offloaded.
 *
 * One row below the whole block catches them and hands them to the CPU, which
 * is what the box does with the offload off. It forwards nothing, so it needs
 * no next hop: index zero is what the hardware reads as "no next hop" on a
 * route that only traps. The block is ordered longest prefix first and the
 * hardware answers with the lowest matching row, so a destination that has a
 * row of its own is answered by that row and never reaches this one.
 *
 * It is not a route the kernel knows about: table id zero is not a table the
 * kernel hands out, so no FIB event finds it and takes it away.
 */
static int otto_l3_add_catch_all(struct otto_l3_ctrl *ctrl, u8 type)
{
	struct otto_l3_route *route;

	route = otto_l3_route_alloc(ctrl, &in6addr_any, ROUTE_PREFIX);
	if (!route)
		return -ENOSPC;

	route->attr.type = type;
	route->attr.action = ROUTE_ACT_TRAP2CPU;
	route->attr.valid = true;

	route->row = otto_l3_route_place(ctrl, route);
	if (route->row < FIRST_PREFIX_ROW) {
		otto_l3_route_teardown(ctrl, route);
		return -ENOSPC;
	}

	ctrl->cfg->route_write(ctrl, route->row, route);

	return 0;
}

static int otto_l3_fib_add_v6(struct otto_l3_ctrl *ctrl, struct fib6_entry_notifier_info *info,
			      unsigned int members)
{
	struct fib6_info *rt = info->rt;
	struct otto_l3_route *route;
	struct net_device *ndev;
	const struct in6_addr *gw;
	bool dropped = false;
	int vlan, port;

	/* The kernel joins the subnet-router anycast address of every prefix
	 * shorter than a /127 and names its route RTN_ANYCAST, so that one is
	 * an address of the switch as much as the RTN_LOCAL ones are.
	 */
	if (rt->fib6_type == RTN_LOCAL || rt->fib6_type == RTN_ANYCAST)
		return otto_l3_fib_add_v6_local(ctrl, rt);

	if (rt->fib6_src.plen)
		return otto_l3_fib_add_v6_src(ctrl, rt, members);

	/* Every add that reaches the driver arrives as a replace, and a replace
	 * is the only word the kernel sends when the route that was programmed
	 * for this destination goes away. Take that one out before deciding
	 * anything about the one replacing it, or a route this driver will not
	 * take keeps the old row forwarding. A row that also stands for
	 * source-specific routes stays, and traps for this one too.
	 */
	route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
				   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);
	if (route && otto_l3_route_has_srcs(route)) {
		route->members = members;
		return 0;
	}
	if (route) {
		dropped = !ipv6_addr_any(&route->gw_ip);
		otto_l3_route_teardown(ctrl, route);
	}

	if (otto_l3_fib_check_v6(ctrl, rt, members))
		goto not_offloaded;

	gw = &rt->fib6_nh->fib_nh_gw6;
	ndev = rt->fib6_nh->fib_nh_dev;

	port = otto_l3_port_dev_lower_find(ndev, ctrl);
	if (port < 0)
		goto not_offloaded;

	vlan = is_vlan_dev(ndev) ? vlan_dev_vlan_id(ndev) : 0;

	route = otto_l3_route_alloc(ctrl, gw, ROUTE_PREFIX);
	if (!route) {
		dev_err(ctrl->dev, "could not allocate a route for gw %pI6c\n", gw);
		goto out_failed;
	}

	route->dst_ip6 = rt->fib6_dst.addr;
	route->prefix_len = rt->fib6_dst.plen;
	route->tb_id = rt->fib6_table->tb6_id;
	route->attr.type = ROUTE_TYPE_IP6UC;
	route->nh.rvid = vlan;
	route->gw_ifindex = ndev->ifindex;

	if (ctrl->cfg->set_router_mac) {
		u64 mac = ether_addr_to_u64(ndev->dev_addr);

		if (otto_l3_alloc_router_mac(ctrl, mac))
			goto out_failed;

		route->nh.if_id = otto_l3_alloc_egress_intf(ctrl, mac, vlan);
		if (route->nh.if_id < 0)
			goto out_failed;
	}

	route->f6i = rt;
	fib6_info_hold(rt);

	/* Until the gateway answers, a shorter prefix in hardware would still
	 * forward this destination. The route takes its row now, trapping to
	 * the CPU, which resolves the gateway; otto_l3_nexthop_update() then
	 * turns the row into a forwarding one and reports the offload. The
	 * next hop index stays zero until then, as on any row that only traps.
	 */
	route->attr.action = ROUTE_ACT_TRAP2CPU;
	route->attr.valid = true;
	route->row = otto_l3_route_place(ctrl, route);
	if (route->row < 0) {
		fib6_info_hw_flags_set(&init_net, rt, false, false, true);
	} else {
		ctrl->cfg->route_write(ctrl, route->row, route);
		fib6_info_hw_flags_set(&init_net, rt, false, true, false);
	}

	otto_l3_port_gw_resolve(ctrl, ndev, &nd_tbl, gw);

	return 0;

not_offloaded:
	/* A replace is also how the kernel says a programmed route is gone, so
	 * this is where a destination stops being forwarded by hardware.
	 */
	if (dropped)
		dev_info(ctrl->dev, "route %pI6c/%d is no longer offloaded\n",
			 &rt->fib6_dst.addr, rt->fib6_dst.plen);

	/* A shorter prefix in hardware would still match this destination and
	 * forward it, whatever this route says - a blackhole or a gateway
	 * the switch cannot reach. A row of its own traps it to the CPU, which
	 * routes it as the kernel does. The default route gets none: that row
	 * would match every destination.
	 */
	if (rt->fib6_dst.plen && rt->fib6_type != RTN_MULTICAST)
		return otto_l3_fib_trap_v6(ctrl, rt, members);

	return 0;

out_failed:
	if (route)
		otto_l3_route_teardown(ctrl, route);
	fib6_info_hw_flags_set(&init_net, rt, false, false, true);

	return -ENOSPC;
}

static int otto_l3_fib_del_v6(struct otto_l3_ctrl *ctrl, struct fib6_entry_notifier_info *info,
			      unsigned int members)
{
	struct fib6_info *rt = info->rt;
	struct otto_l3_route *route;

	if (rt->fib6_type == RTN_LOCAL || rt->fib6_type == RTN_ANYCAST)
		return otto_l3_fib_del_v6_local(ctrl, rt);

	if (rt->fib6_src.plen)
		return otto_l3_fib_del_v6_src(ctrl, rt, members);

	route = otto_l3_route_find(ctrl, rt->fib6_table->tb6_id, ROUTE_TYPE_IP6UC,
				   0, &rt->fib6_dst.addr, rt->fib6_dst.plen);

	/* A route left out of the offload holds at most the row trapping it.
	 * A multipath route keeps that row until its last member is gone: a
	 * delete names only the members that leave. The row stays while it
	 * also stands for source-specific routes.
	 */
	if (route && ipv6_addr_any(&route->gw_ip)) {
		if (route->members > members)
			route->members -= members;
		else if (otto_l3_route_has_srcs(route))
			route->members = 0;
		else
			otto_l3_route_teardown(ctrl, route);
		return 0;
	}

	if (otto_l3_fib_check_v6(ctrl, rt, members))
		return 0;

	/* Several FIB entries can share a destination and differ only in their
	 * gateway, and a delete arrives for one of them with its sibling count
	 * already cleared, so the gateway is what says whether this is the
	 * route that was programmed.
	 */
	if (!route || !ipv6_addr_equal(&route->gw_ip, &rt->fib6_nh->fib_nh_gw6)) {
		/* Most IPv6 routes are never offloaded, so this is the ordinary
		 * case rather than a failure.
		 */
		dev_dbg(ctrl->dev, "no route %pI6c/%d via %pI6c\n",
			&rt->fib6_dst.addr, rt->fib6_dst.plen,
			&rt->fib6_nh->fib_nh_gw6);
		return 0;
	}

	otto_l3_route_teardown(ctrl, route);

	return 0;
}

/* A dscp selector the rule was given explicitly, 0 included, is kept in the
 * family's own part of the rule, which only the family's callbacks see; its
 * fill callback reports one as FRA_DSCP. A rule that cannot be read out counts
 * as having one.
 */
static bool otto_l3_rule_dscp(struct fib_rules_ops *ops, struct fib_rule *rule)
{
	struct fib_rule_hdr *frh;
	struct sk_buff *skb;
	bool dscp = true;

	skb = alloc_skb(sizeof(*frh) + ops->nlmsg_payload(rule), GFP_KERNEL);
	if (!skb)
		return true;

	frh = skb_put_zero(skb, sizeof(*frh));
	if (!ops->fill(rule, skb, frh))
		dscp = nla_find((struct nlattr *)(frh + 1), skb->len - sizeof(*frh), FRA_DSCP);

	consume_skb(skb);

	return dscp;
}

static bool otto_l3_rule_default(struct fib_rules_ops *ops, struct fib_rule *rule,
				 int family)
{
	bool def = false;

	/* The kernel's own test leaves the protocol and the mark mask out */
	if (rule->ip_proto || rule->mark_mask)
		return false;
#if IS_ENABLED(CONFIG_IP_MULTIPLE_TABLES)
	if (family == AF_INET)
		def = fib4_rule_default(rule);
#endif
#if IS_REACHABLE(CONFIG_IPV6) && IS_ENABLED(CONFIG_IPV6_MULTIPLE_TABLES)
	if (family == AF_INET6)
		def = fib6_rule_default(rule);
#endif
	return def && !otto_l3_rule_dscp(ops, rule);
}

/* The switch looks a destination up in one table, so it forwards the way the
 * kernel does only while the kernel's lookup for a forwarded packet is the
 * local table, then main. A rule ahead of main may send the packet elsewhere,
 * and so does a main rule that is missing or takes only some packets. A rule
 * after main only sees what main has no route for, and no row forwards that
 * either. A rule that only matches what the switch sends itself never sees a
 * forwarded packet.
 */
static bool otto_l3_rules_allow(int family)
{
	struct fib_rules_ops *ops = NULL;
	struct fib_rule *rule;
	bool main_seen = false;
	u32 main_pref = 0;

#if IS_ENABLED(CONFIG_IP_MULTIPLE_TABLES)
	if (family == AF_INET)
		ops = init_net.ipv4.rules_ops;
#endif
#if IS_REACHABLE(CONFIG_IPV6) && IS_ENABLED(CONFIG_IPV6_MULTIPLE_TABLES)
	if (family == AF_INET6)
		ops = init_net.ipv6.fib6_rules_ops;
#endif
	/* Without policy routing the lookup is local, then main */
	if (!ops)
		return true;

	/* Sorted by priority, and changed only under rtnl, which is held */
	list_for_each_entry(rule, &ops->rules_list, list) {
		if (main_seen) {
			if (rule->pref == main_pref)
				return false;
			break;
		}

		if (otto_l3_rule_default(ops, rule, family)) {
			if (rule->table == RT_TABLE_MAIN) {
				main_seen = true;
				main_pref = rule->pref;
			} else if (rule->table != RT_TABLE_LOCAL) {
				return false;
			}
			continue;
		}

		if (rule->iifindex == LOOPBACK_IFINDEX && !(rule->flags & FIB_RULE_INVERT))
			continue;

		return false;
	}

	return main_seen;
}

/* One resolve brings back every route through a gateway */
static bool otto_l3_gw_seen(struct otto_l3_ctrl *ctrl, struct otto_l3_route *r)
{
	struct otto_l3_route *q;

	list_for_each_entry(q, &ctrl->routes_list, list) {
		if (q == r)
			return false;
		if (q->attr.type == r->attr.type && q->gw_ifindex == r->gw_ifindex &&
		    ipv6_addr_equal(&q->gw_ip, &r->gw_ip))
			return true;
	}

	return false;
}

static void otto_l3_rules_check(struct otto_l3_ctrl *ctrl, int family)
{
	u8 type = family == AF_INET6 ? ROUTE_TYPE_IP6UC : ROUTE_TYPE_IP4UC;
	bool *off = family == AF_INET6 ? &ctrl->v6_fwd_off : &ctrl->v4_fwd_off;
	struct neigh_table *tbl = &arp_tbl;
	struct otto_l3_route *r;
	struct net_device *dev;

	if (otto_l3_rules_allow(family) != *off)
		return;

	*off = !*off;
	dev_info(ctrl->dev, "%s forwarding offload %s by the policy rules\n",
		 family == AF_INET6 ? "IPv6" : "IPv4", *off ? "suspended" : "resumed");

	if (family == AF_INET6) {
		if (!IS_REACHABLE(CONFIG_IPV6))
			return;
		tbl = &nd_tbl;
	}

	list_for_each_entry(r, &ctrl->routes_list, list) {
		if (r->attr.type != type || !r->tb_id)
			continue;

		if (*off) {
			if (r->attr.action != ROUTE_ACT_FORWARD)
				continue;
			r->attr.action = ROUTE_ACT_TRAP2CPU;
			r->attr.ttl_dec = false;
			r->attr.ttl_check = false;
			otto_l3_route_rewrite(ctrl, r);
			if (IS_REACHABLE(CONFIG_IPV6) && r->f6i && r->row >= FIRST_PREFIX_ROW)
				fib6_info_hw_flags_set(&init_net, r->f6i, false, true, false);
			continue;
		}

		/* Back to forwarding the way a neighbour update brings a route
		 * back: through the gateway, if it still answers
		 */
		if (ipv6_addr_any(&r->gw_ip) ||
		    (type == ROUTE_TYPE_IP4UC && !r->gw_ip.s6_addr32[3]) ||
		    otto_l3_gw_seen(ctrl, r))
			continue;
		dev = __dev_get_by_index(&init_net, r->gw_ifindex);
		if (dev)
			otto_l3_port_gw_resolve(ctrl, dev, tbl, &r->gw_ip);
	}
}

static void otto_l3_fib_event_work_do(struct work_struct *work)
{
	struct otto_l3_fib_event_work *fib_work =
		container_of(work, struct otto_l3_fib_event_work, work);
	struct otto_l3_ctrl *ctrl = fib_work->ctrl;
	struct fib_rule *rule;
	int err;

	/* Protect internal structures from changes */
	rtnl_lock();
	dev_dbg(ctrl->dev, "doing work, event %ld\n", fib_work->event);
	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_ADD:
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
		err = otto_l3_fib_add_v4(ctrl, &fib_work->fen_info);
		if (err)
			dev_err(ctrl->dev, "fib_add() failed\n");

		fib_info_put(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_ENTRY_DEL:
		err = otto_l3_fib_del_v4(ctrl, &fib_work->fen_info);
		if (err)
			dev_err(ctrl->dev, "fib_del() failed\n");

		fib_info_put(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_RULE_ADD:
	case FIB_EVENT_RULE_DEL:
		rule = fib_work->fr_info.rule;
		if (ctrl->cfg->use_l3_tables)
			otto_l3_rules_check(ctrl, fib_work->fr_info.info.family);
		else if (fib_work->fr_info.info.family == AF_INET && !fib4_rule_default(rule))
			dev_err(ctrl->dev, "FIB4 default rule failed\n");
		fib_rule_put(rule);
		break;
	}
	rtnl_unlock();
	kfree(fib_work);
}

static void otto_l3_fib6_event_work_do(struct work_struct *work)
{
	struct otto_l3_fib_event_work *fib_work =
		container_of(work, struct otto_l3_fib_event_work, work);
	struct otto_l3_ctrl *ctrl = fib_work->ctrl;
	int err = 0;

	/* Protect internal structures from changes */
	rtnl_lock();
	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
		err = otto_l3_fib_add_v6(ctrl, &fib_work->fen6_info, fib_work->members);
		break;
	case FIB_EVENT_ENTRY_DEL:
		err = otto_l3_fib_del_v6(ctrl, &fib_work->fen6_info, fib_work->members);
		break;
	}
	if (err)
		dev_err(ctrl->dev, "FIB6 event %ld failed: %d\n", fib_work->event, err);
	rtnl_unlock();

	fib6_info_release(fib_work->fen6_info.rt);
	kfree(fib_work);
}


/* The switch has a single routing table. A route from any table but these two
 * would be written to it as if the kernel looked it up for every packet.
 */
static bool otto_l3_fib_table_ok(struct otto_l3_ctrl *ctrl, unsigned long event, u32 tb_id)
{
	if (tb_id == RT_TABLE_MAIN || tb_id == RT_TABLE_LOCAL)
		return true;

	if (event != FIB_EVENT_ENTRY_DEL)
		dev_warn_ratelimited(ctrl->dev, "routes in table %u are not offloaded\n", tb_id);

	return false;
}

/* Probe holds the work disabled until the FIB notifier is in place, and a
 * work that is disabled does not queue: the request is noted first, for probe
 * to find once it has enabled the work.
 */
static void otto_l3_resync_request(struct otto_l3_ctrl *ctrl)
{
	WRITE_ONCE(ctrl->resync_wanted, true);
	smp_mb(); /* pairs with the one after enable_delayed_work() in probe */
	queue_delayed_work(system_long_wq, &ctrl->resync_work, 0);
}

/* Called with rcu_read_lock() */
static int otto_l3_fib_notifier(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct otto_l3_ctrl *ctrl = container_of(this, struct otto_l3_ctrl, fib_nb);
	struct rtl838x_switch_priv *priv = ctrl->priv;
	struct otto_l3_fib_event_work *fib_work;
	struct fib_notifier_info *info = ptr;

	if (info->family != AF_INET && info->family != AF_INET6)
		return NOTIFY_DONE;

	/* ignore FIB events for HW with missing L3 offloading implementation */
	if (!ctrl->cfg->setup)
		return NOTIFY_DONE;

	fib_work = kzalloc(sizeof(*fib_work), GFP_ATOMIC);
	if (!fib_work) {
		otto_l3_resync_request(ctrl);
		return NOTIFY_BAD;
	}

	fib_work->ctrl = ctrl;
	fib_work->event = event;

	switch (event) {
	case FIB_EVENT_ENTRY_ADD:
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
	case FIB_EVENT_ENTRY_DEL:
		dev_dbg(ctrl->dev, "FIB_ENTRY ADD/DEL, event %ld\n", event);
		if (info->family == AF_INET) {
			struct fib_entry_notifier_info *fen_info = ptr;

			if (!otto_l3_fib_table_ok(ctrl, event, fen_info->tb_id)) {
				kfree(fib_work);
				return NOTIFY_DONE;
			}

			if (fen_info->fi->fib_nh_is_v6) {
				NL_SET_ERR_MSG_MOD(info->extack,
						   "IPv6 gateway with IPv4 route is not supported");
				kfree(fib_work);
				return notifier_from_errno(-EINVAL);
			}

			memcpy(&fib_work->fen_info, ptr, sizeof(fib_work->fen_info));
			/* Take referece on fib_info to prevent it from being
			 * freed while work is queued. Release it afterwards.
			 */
			fib_info_hold(fib_work->fen_info.fi);
			INIT_WORK(&fib_work->work, otto_l3_fib_event_work_do);
		} else if (info->family == AF_INET6 && IS_REACHABLE(CONFIG_IPV6) &&
			   ctrl->cfg->use_l3_tables) {
			struct fib6_entry_notifier_info *fen6_info = ptr;

			if (!otto_l3_fib_table_ok(ctrl, event, fen6_info->rt->fib6_table->tb6_id)) {
				kfree(fib_work);
				return NOTIFY_DONE;
			}

			memcpy(&fib_work->fen6_info, ptr, sizeof(fib_work->fen6_info));
			fib6_info_hold(fib_work->fen6_info.rt);
			INIT_WORK(&fib_work->work, otto_l3_fib6_event_work_do);

			/* An add carries the route with every sibling it has by
			 * now, which only this moment still counts: the work runs
			 * later. A delete names the members that left, already
			 * unlinked from the others.
			 */
			if (event == FIB_EVENT_ENTRY_DEL)
				fib_work->members = fib_work->fen6_info.nsiblings + 1;
			else
				fib_work->members = READ_ONCE(fib_work->fen6_info.rt->fib6_nsiblings) + 1;
		} else {
			kfree(fib_work);
			return NOTIFY_DONE;
		}
		break;

	case FIB_EVENT_RULE_ADD:
	case FIB_EVENT_RULE_DEL:
		dev_dbg(ctrl->dev, "FIB_RULE ADD/DEL, event: %ld\n", event);
		memcpy(&fib_work->fr_info, ptr, sizeof(fib_work->fr_info));
		fib_rule_get(fib_work->fr_info.rule);
		INIT_WORK(&fib_work->work, otto_l3_fib_event_work_do);
		break;
	default:
		kfree(fib_work);
		return NOTIFY_DONE;
	}

	queue_work(priv->wq, &fib_work->work);

	return NOTIFY_DONE;
}

static void otto_l3_net_event_work_do(struct work_struct *work)
{
	struct otto_l3_net_event_work *net_work =
		container_of(work, struct otto_l3_net_event_work, work);

	otto_l3_nexthop_update(net_work->ctrl, net_work->type, net_work->ifindex,
			       &net_work->gw_addr, net_work->mac, net_work->valid);

	kfree(net_work);
}

/* nd_tbl exists only where IPv6 is built in, or is a module this driver
 * can reach
 */
static bool otto_l3_is_nd_tbl(const struct neigh_table *tbl)
{
	return IS_REACHABLE(CONFIG_IPV6) && tbl == &nd_tbl;
}

static int otto_l3_netevent_notifier(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct otto_l3_ctrl *ctrl = container_of(this, struct otto_l3_ctrl, ne_nb);
	struct rtl838x_switch_priv *priv = ctrl->priv;
	struct otto_l3_net_event_work *net_work;
	struct neighbour *n = ptr;
	struct net_device *dev;
	int port;

	switch (event) {
	case NETEVENT_NEIGH_UPDATE:
		/* ignore events for HW with missing L3 offloading implementation */
		if (!ctrl->cfg->setup)
			return NOTIFY_DONE;

		if (n->tbl != &arp_tbl && !otto_l3_is_nd_tbl(n->tbl))
			return NOTIFY_DONE;

		/* Only where the L3 tables carry an IPv6 destination can a
		 * route be waiting on an ndisc neighbour, which is how the
		 * FIB side of the same question is answered.
		 */
		if (otto_l3_is_nd_tbl(n->tbl) && !ctrl->cfg->use_l3_tables)
			return NOTIFY_DONE;
		dev = n->dev;
		port = otto_l3_port_dev_lower_find(dev, ctrl);
		if (port < 0) {
			dev_dbg(ctrl->dev, "Neighbour not on a switch port, not updating\n");
			return NOTIFY_DONE;
		}

		net_work = kzalloc(sizeof(*net_work), GFP_ATOMIC);
		if (!net_work) {
			otto_l3_resync_request(ctrl);
			return NOTIFY_BAD;
		}

		INIT_WORK(&net_work->work, otto_l3_net_event_work_do);
		net_work->ctrl = ctrl;

		/* A neighbour taken out of the table is marked dead, and keeps a
		 * NUD_VALID state if it had one
		 */
		read_lock_bh(&n->lock);
		net_work->valid = (n->nud_state & NUD_VALID) && !n->dead;
		net_work->mac = ether_addr_to_u64(n->ha);
		read_unlock_bh(&n->lock);
		net_work->ifindex = dev->ifindex;
		if (n->tbl == &arp_tbl) {
			ipv6_addr_set_v4mapped(*(__be32 *)n->primary_key,
					       &net_work->gw_addr);
			net_work->type = ROUTE_TYPE_IP4UC;
		} else {
			net_work->gw_addr = *(struct in6_addr *)n->primary_key;
			net_work->type = ROUTE_TYPE_IP6UC;
		}

		dev_dbg(ctrl->dev, "updating neighbour on port %d, mac %016llx\n",
			port, net_work->mac);
		queue_work(priv->wq, &net_work->work);
		break;
	}

	return NOTIFY_DONE;
}

/* debugfs dump of the RTL930x L3 route tables (realtek_otto_l3/routes,
 * realtek_otto_l3/clear_route_hits). Requested on issue #25129; RTL930x
 * only, since the other families' L3 tables have different layouts and are
 * not decoded here.
 *
 * Both tables are reached through OTTO_REG_9300_1, the same access register
 * as every other L3 table, so only one of the two is ever held at a time
 * (otto_table_acquire() takes a mutex per register, not per table).
 *
 * Rows are decoded by otto_l3_930x_host_route_decode() and
 * otto_l3_930x_route_decode(), the decoders the route readers use.
 */

static const char * const otto_l3_930x_dump_type_name[4] = {
	"ip4uc", "ip4mc", "ip6uc", "ip6mc",
};

/* Indexed by ROUTE_ACT_*; multicast entries never reach this, so it is only
 * ever indexed with a value produced by the unicast decoders.
 */
static const char * const otto_l3_930x_dump_action_name[4] = {
	[ROUTE_ACT_FORWARD]  = "fwd",
	[ROUTE_ACT_TRAP2CPU] = "trap",
	[ROUTE_ACT_COPY2CPU] = "copy",
	[ROUTE_ACT_DROP]     = "drop",
};

/* Rows one entry of the prefix route table covers, by type: IPv4 unicast,
 * IPv4 multicast, IPv6 unicast, IPv6 multicast. The rows behind the first
 * read back as entries of their own, so anything walking the table has to
 * step over them. The multicast pair is the Realtek GPL SDK's
 * (dal_longan_l3.h: LONGAN_L3_ROUTE_IPMC_WIDTH_IPV4 and _IPV6); nothing here
 * programs a multicast entry, and the walk has to step over one it meets all
 * the same.
 */
static const u8 otto_l3_930x_prefix_widths[] = { 1, 2, V6_PREFIX_ROWS, 8 };

/* One valid entry, where it sits and the route it holds. Multicast entries
 * (type 1 and 3) only carry what word 0 says (valid, type, hence width): no
 * field position beyond that is in this driver or in the GPL SDK excerpts it
 * was written against, so decoded stays false and the dump prints '-' for
 * the rest.
 */
struct otto_l3_930x_dump_rec {
	bool is_prefix;
	bool decoded;
	u32 addr;
	u32 idx;
	u8 width;
	struct otto_l3_route rt;
};

/* Decode a host route row already fetched into @data (words 0-4 of the
 * RTL9300_TBL_L3_HOST_ROUTE_IPMC layout). Returns false if the row is not
 * valid.
 */
static bool otto_l3_930x_dump_decode_host(const u32 *data, u32 addr,
					  struct otto_l3_930x_dump_rec *rec)
{
	memset(rec, 0, sizeof(*rec));
	rec->decoded = otto_l3_930x_host_route_decode(data, &rec->rt);
	if (!rec->rt.attr.valid)
		return false;

	rec->addr = addr;
	rec->idx = (addr / 8) * 6 + (addr % 8);
	rec->width = otto_l3_930x_slot_widths[rec->rt.attr.type];
	if (rec->decoded)
		rec->rt.prefix_len = rec->rt.attr.type == ROUTE_TYPE_IP4UC ? 32 : 128;

	return true;
}

/* Decode a prefix route row already fetched into @data (words 0-10 of the
 * RTL9300_TBL_L3_PREFIX_ROUTE_IPMC layout). Returns false if not valid.
 */
static bool otto_l3_930x_dump_decode_prefix(const u32 *data, u32 addr,
					    struct otto_l3_930x_dump_rec *rec)
{
	memset(rec, 0, sizeof(*rec));
	rec->decoded = otto_l3_930x_route_decode(data, &rec->rt);
	if (!rec->rt.attr.valid)
		return false;

	rec->is_prefix = true;
	rec->addr = addr;
	rec->idx = addr;
	rec->width = otto_l3_930x_prefix_widths[rec->rt.attr.type];

	return true;
}

static const char *otto_l3_930x_dump_ttl_label(bool dec, bool chk)
{
	if (dec && chk)
		return "chk,dec";
	if (chk)
		return "chk";
	if (dec)
		return "dec";
	return "-";
}

static void otto_l3_930x_dump_print(struct seq_file *m, const struct otto_l3_930x_dump_rec *r)
{
	char nh_id[8] = "-", dst_null[8] = "-", ttl[8] = "-", qos[8] = "-";
	char hash[8] = "-", slot[8] = "-", hit[8] = "-", action[8] = "-";
	char dest[INET6_ADDRSTRLEN + sizeof("/128")] = "-";

	if (!r->is_prefix) {
		snprintf(hash, sizeof(hash), "%u", (r->addr >> 3) & 0x1ff);
		snprintf(slot, sizeof(slot), "%u", r->addr & 7);
	}

	if (r->decoded) {
		const struct otto_l3_route *rt = &r->rt;

		snprintf(hit, sizeof(hit), "%d", rt->attr.hit);
		snprintf(action, sizeof(action), "%s",
			 otto_l3_930x_dump_action_name[rt->attr.action]);
		snprintf(nh_id, sizeof(nh_id), "%u", rt->nh.id);
		snprintf(dst_null, sizeof(dst_null), "%d", rt->attr.dst_null);
		snprintf(ttl, sizeof(ttl), "%s",
			 otto_l3_930x_dump_ttl_label(rt->attr.ttl_dec, rt->attr.ttl_check));
		if (rt->attr.qos_as)
			snprintf(qos, sizeof(qos), "%u", rt->attr.qos_prio);

		if (rt->attr.type == ROUTE_TYPE_IP6UC)
			snprintf(dest, sizeof(dest), "%pI6c/%d", &rt->dst_ip6, rt->prefix_len);
		else
			snprintf(dest, sizeof(dest), "%pI4/%d", &rt->dst_ip, rt->prefix_len);
	}

	seq_printf(m, "%-7s%5u 0x%04x %4s %4s %5u 1 %-6s%4s %-48s %-7s%5s %4s %-8s %s\n",
		   r->is_prefix ? "prefix" : "host", r->idx, r->addr, hash, slot, r->width,
		   otto_l3_930x_dump_type_name[r->rt.attr.type], hit, dest, action, nh_id, dst_null,
		   ttl, qos);
}

/* Sweep both tables in one call. A per-row seq_file iterator cannot do this
 * safely: seq_read_iter() advances the position past a row and then skips
 * its show() whenever the output buffer fills, which loses that row for good
 * because the position no longer identifies it. single_open() renders the
 * whole dump in one pass, the way debugfs.c dumps the larger L2 table.
 */
static int otto_l3_930x_dump_show(struct seq_file *m, void *v)
{
	struct otto_l3_ctrl *ctrl = m->private;
	struct otto_l3_930x_dump_rec rec;
	unsigned int mc_seen = 0;
	u32 prefix_data[20];
	u32 host_data[11];
	unsigned int n;
	int handle;
	int rows;
	u32 addr;

	seq_puts(m, "TABLE    IDX   ADDR HASH SLOT WIDTH V TYPE   HIT DESTINATION                                      ACTION NH_ID NULL TTL      QOS\n");

	rows = otto_table_rows(RTL9300_TBL_L3_HOST_ROUTE_IPMC);
	if (rows < 0)
		return rows;

	handle = otto_table_acquire(RTL9300_TBL_L3_HOST_ROUTE_IPMC);
	if (handle < 0)
		return handle;

	for (addr = 0, n = 0; addr < rows; n++) {
		if (!(n % 64))
			cond_resched();

		if ((addr & 7) >= 6) { /* unused address, no logical slot maps here */
			addr++;
			continue;
		}

		__otto_table_read(handle, addr, &host_data);
		if (!otto_l3_930x_dump_decode_host(host_data, addr, &rec)) {
			addr++;
			continue;
		}

		if (!rec.decoded)
			mc_seen++;

		otto_l3_930x_dump_print(m, &rec);
		addr += rec.width;
	}

	otto_table_release(handle);

	rows = otto_table_rows(RTL9300_TBL_L3_PREFIX_ROUTE_IPMC);
	if (rows < 0)
		return rows;

	handle = otto_table_acquire(RTL9300_TBL_L3_PREFIX_ROUTE_IPMC);
	if (handle < 0)
		return handle;

	for (addr = 0, n = 0; addr < rows; n++) {
		if (!(n % 64))
			cond_resched();

		__otto_table_read(handle, addr, &prefix_data);
		if (!otto_l3_930x_dump_decode_prefix(prefix_data, addr, &rec)) {
			addr++;
			continue;
		}

		if (!rec.decoded)
			mc_seen++;

		otto_l3_930x_dump_print(m, &rec);
		addr += rec.width;
	}

	otto_table_release(handle);

	/* Report once per read, not once per attempt: seq_file re-runs show()
	 * on a larger buffer until the whole dump fits, and only the run that
	 * fits has not overflowed.
	 */
	if (mc_seen && !seq_has_overflowed(m))
		dev_notice(ctrl->dev,
			   "routes: %u multicast entries printed as type only, field layout not implemented\n",
			   mc_seen);

	return 0;
}

static int otto_l3_930x_route_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, otto_l3_930x_dump_show, inode->i_private);
}

static const struct file_operations otto_l3_930x_route_fops = {
	.owner   = THIS_MODULE,
	.open    = otto_l3_930x_route_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

/* The chain a routed packet follows once the route table has matched. The next
 * hop names both halves of what the packet becomes: a DMAC entry in the L2
 * table, which carries the destination MAC and the port it leaves by, and an
 * egress interface, which carries the VLAN it is sent into and the source MAC
 * it is sent with. A route that matches and still does not forward is broken at
 * one of these, and none of it can be read back anywhere else.
 */
static int otto_l3_930x_nexthop_show(struct seq_file *m, void *v)
{
	struct otto_l3_ctrl *ctrl = m->private;
	struct rtl838x_switch_priv *priv = ctrl->priv;
	int l2_rows, rows;

	seq_puts(m,
		 "NH_ID DMAC_IDX INTF  VID SMAC              V NH STATIC TRK PORT MAC               RVID NH_RID\n");

	rows = otto_table_rows(RTL9300_TBL_L3_NEXTHOP);
	l2_rows = otto_table_rows(RTL9300_TBL_L2_UC);
	if (rows < 0 || l2_rows < 0)
		return rows < 0 ? rows : l2_rows;

	for (int idx = 0; idx < rows; idx++) {
		struct rtl838x_l2_entry e = {};
		struct otto_l3_intf egr = {};
		u16 dmac_idx, intf;
		u8 smac[ETH_ALEN];

		if (!(idx % 64))
			cond_resched();

		ctrl->cfg->get_nexthop(ctrl, idx, &dmac_idx, &intf);
		if (!dmac_idx && !intf)
			continue;

		ctrl->cfg->get_egress_intf(ctrl, intf, &egr);
		u64_to_ether_addr(ctrl->cfg->get_egress_mac(ctrl, L3_EGRESS_DMACS + egr.smac_idx),
				  smac);

		seq_printf(m, "%5d %8d %4d %4d %pM", idx, dmac_idx, intf, egr.vid, smac);

		/* The field also carries the three values that name an action
		 * instead of an entry, and they are all above the table.
		 */
		if (dmac_idx >= l2_rows) {
			seq_printf(m, " %s\n",
				   dmac_idx == 0x7fff ? "drop" :
				   dmac_idx == 0x7ffe ? "trap to CPU" :
				   dmac_idx == 0x7ffd ? "trap to master CPU" :
				   "out of the L2 table");
			continue;
		}

		priv->r->read_l2_entry_using_hash(dmac_idx >> 2, dmac_idx & 0x3, &e);
		if (!e.valid) {
			seq_puts(m, " 0  -      -   -    -                 -    -      -\n");
			continue;
		}

		seq_printf(m, " %d %2d %6d %3d %4d %pM %4d %6d\n",
			   e.valid, e.next_hop, e.is_static, e.is_trunk, e.port,
			   e.mac, e.rvid, e.nh_route_id);
	}

	return 0;
}

static int otto_l3_930x_nexthop_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, otto_l3_930x_nexthop_show, inode->i_private);
}

static const struct file_operations otto_l3_930x_nexthop_fops = {
	.owner   = THIS_MODULE,
	.open    = otto_l3_930x_nexthop_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

/* Clear the HIT bit of every valid unicast entry in the host route table by
 * read-modify-write. Read through the unicast view so the write covers the
 * entry and nothing else, the way the vendor SDK rewrites it through its own
 * typed setter. Multicast entries are left untouched: their HIT bit position
 * is not verified, and they do not fit this view anyway.
 *
 * Step by the width of the entry, not by one address: the allocator reserves
 * width consecutive slots per entry and only the first is ever written, so
 * what the others read back is not a defined entry. Decoding one and writing
 * it back would put that content into a slot reserved for a real entry.
 */
static int otto_l3_930x_clear_hit_host(unsigned int *cleared, unsigned int *skipped_mc)
{
	struct otto_l3_930x_dump_rec rec;
	u32 data[5];
	int handle;
	int rows;
	u32 addr;

	rows = otto_table_rows(RTL9300_TBL_L3_HOST_ROUTE_IPUC);
	if (rows < 0)
		return rows;

	handle = otto_table_acquire(RTL9300_TBL_L3_HOST_ROUTE_IPUC);
	if (handle < 0)
		return handle;

	for (addr = 0; addr < rows;) {
		if ((addr & 7) >= 6) {
			addr++;
			continue;
		}

		__otto_table_read(handle, addr, &data);
		if (!otto_l3_930x_dump_decode_host(data, addr, &rec)) {
			addr++;
			continue;
		}

		if (!rec.decoded) {
			(*skipped_mc)++;
		} else if (rec.rt.attr.hit) {
			data[0] &= ~BIT(20);
			__otto_table_write(handle, addr, &data);
			(*cleared)++;
		}

		addr += rec.width;
	}

	otto_table_release(handle);

	return 0;
}

/* Same as otto_l3_930x_clear_hit_host(), for the prefix route table. */
static int otto_l3_930x_clear_hit_prefix(unsigned int *cleared, unsigned int *skipped_mc)
{
	struct otto_l3_930x_dump_rec rec;
	u32 data[11];
	int handle;
	int rows;
	u32 addr;

	rows = otto_table_rows(RTL9300_TBL_L3_PREFIX_ROUTE_IPUC);
	if (rows < 0)
		return rows;

	handle = otto_table_acquire(RTL9300_TBL_L3_PREFIX_ROUTE_IPUC);
	if (handle < 0)
		return handle;

	for (addr = 0; addr < rows;) {
		__otto_table_read(handle, addr, &data);
		if (!otto_l3_930x_dump_decode_prefix(data, addr, &rec)) {
			addr++;
			continue;
		}

		if (!rec.decoded) {
			(*skipped_mc)++;
		} else if (rec.rt.attr.hit) {
			data[10] &= ~BIT(22);
			__otto_table_write(handle, addr, &data);
			(*cleared)++;
		}

		addr += rec.width;
	}

	otto_table_release(handle);

	return 0;
}

static ssize_t otto_l3_930x_clear_hit_write(struct file *filp, const char __user *buf,
					    size_t count, loff_t *ppos)
{
	struct otto_l3_ctrl *ctrl = filp->private_data;
	unsigned int cleared = 0, skipped_mc = 0;
	int err;

	if (*ppos)
		return -EINVAL;

	err = otto_l3_930x_clear_hit_host(&cleared, &skipped_mc);
	if (!err)
		err = otto_l3_930x_clear_hit_prefix(&cleared, &skipped_mc);
	if (err) {
		dev_err(ctrl->dev, "clear_route_hits: could not take the table: %d\n", err);
		return err;
	}

	dev_info(ctrl->dev, "clear_route_hits: cleared %u entries\n", cleared);
	if (skipped_mc)
		dev_notice(ctrl->dev,
			   "clear_route_hits: left %u multicast entries untouched, HIT bit position not implemented\n",
			   skipped_mc);

	return count;
}

static const struct file_operations otto_l3_930x_clear_hit_fops = {
	.owner = THIS_MODULE,
	.open  = simple_open,
	.write = otto_l3_930x_clear_hit_write,
};

static void otto_l3_930x_dbgfs_remove(void *data)
{
	debugfs_remove_recursive(data);
}

#define OTTO_L3_DBG_ROOT_DIR	"realtek_otto_l3"

/* A debugfs tree of its own rather than a subtree of the "rtl838x" directory
 * debugfs.c creates: otto_l3_probe() runs before rtl930x_dbgfs_init(), so
 * priv->dbgfs_dir does not exist yet at this point. A second RTL9300 in one
 * system would find the name taken and log the warning below rather than
 * share the tree, since the directory is tied to @dev's devm lifetime.
 */
static void otto_l3_930x_dbgfs_init(struct otto_l3_ctrl *ctrl)
{
	struct device *dev = ctrl->dev;
	struct dentry *root;

	root = debugfs_create_dir(OTTO_L3_DBG_ROOT_DIR, NULL);
	if (IS_ERR(root)) {
		/* -ENODEV is a kernel built without debugfs, not a failure */
		if (PTR_ERR(root) != -ENODEV)
			dev_warn(dev, "could not create %s debugfs directory\n",
				 OTTO_L3_DBG_ROOT_DIR);
		return;
	}

	if (devm_add_action_or_reset(dev, otto_l3_930x_dbgfs_remove, root))
		return;

	debugfs_create_file("routes", 0400, root, ctrl, &otto_l3_930x_route_fops);
	debugfs_create_file("nexthops", 0400, root, ctrl, &otto_l3_930x_nexthop_fops);
	debugfs_create_file("clear_route_hits", 0200, root, ctrl, &otto_l3_930x_clear_hit_fops);
}

const struct otto_l3_config otto_l3_838x_cfg = {
	.route_read = otto_l3_838x_route_read,
	.route_write = otto_l3_838x_route_write,
};

const struct otto_l3_config otto_l3_839x_cfg = {
	.route_read = otto_l3_839x_route_read,
	.route_write = otto_l3_839x_route_write,
	.setup = otto_l3_839x_setup,
};

const struct otto_l3_config otto_l3_930x_cfg = {
	.use_l3_tables = true,
	.find_slot = otto_l3_930x_find_slot,
	.get_egress_intf = otto_l3_930x_get_egress_intf,
	.get_egress_mac = otto_l3_930x_get_egress_mac,
	.set_egress_mac = otto_l3_930x_set_egress_mac,
	.set_egress_intf = otto_l3_930x_set_egress_intf,
	.host_route_write = otto_l3_930x_host_route_write,
	.get_router_mac = otto_l3_930x_get_router_mac,
	.set_router_mac = otto_l3_930x_set_router_mac,
	.get_nexthop = otto_l3_930x_get_nexthop,
	.set_nexthop = otto_l3_930x_set_nexthop,
	.route_lookup_hw = otto_l3_930x_route_lookup_hw,
	.route_rows_move = otto_l3_930x_route_rows_move,
	.route_read = otto_l3_930x_route_read,
	.route_write = otto_l3_930x_route_write,
	.setup = otto_l3_930x_setup,
	.dbgfs_init = otto_l3_930x_dbgfs_init,
};

const struct otto_l3_config otto_l3_931x_cfg = {
};

static const struct of_device_id otto_l3_of_ids[] = {
	{ .compatible = "realtek,rtl8380-switch", .data = &otto_l3_838x_cfg, },
	{ .compatible = "realtek,rtl8392-switch", .data = &otto_l3_839x_cfg, },
	{ .compatible = "realtek,rtl9301-switch", .data = &otto_l3_930x_cfg, },
	{ .compatible = "realtek,rtl9311-switch", .data = &otto_l3_931x_cfg, },
	{ /* sentinel */ }
};

void otto_l3_remove(struct rtl838x_switch_priv *priv)
{
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;
	struct otto_l3_route *r, *tmp;

	/* A replay would register the FIB notifier again */
	disable_delayed_work_sync(&ctrl->resync_work);

	if (ctrl->ne_nb.notifier_call) {
		unregister_netevent_notifier(&ctrl->ne_nb);
		ctrl->ne_nb.notifier_call = NULL;
	}
	if (ctrl->fib_nb.notifier_call) {
		unregister_fib_notifier(&init_net, &ctrl->fib_nb);
		ctrl->fib_nb.notifier_call = NULL;
	}

	/* Unregistering stops new events, not the work already queued, and that
	 * work walks this list. Wait for it before touching the list here: the
	 * queue is single threaded, which is what lets the rest of the driver
	 * walk it with no lock at all.
	 */
	flush_workqueue(priv->wq);

	/* Nothing takes a route out now, and a FIB entry one still names would
	 * be kept alive by it.
	 */
	rtnl_lock();
	list_for_each_entry_safe(r, tmp, &ctrl->routes_list, list)
		otto_l3_route_free(ctrl, r);
	rtnl_unlock();
}

struct otto_l3_flush_work {
	struct work_struct work;
	struct otto_l3_ctrl *ctrl;
};

static void otto_l3_fib_flush_work_do(struct work_struct *work)
{
	struct otto_l3_flush_work *fw = container_of(work, struct otto_l3_flush_work, work);
	struct otto_l3_route *r, *tmp;

	rtnl_lock();
	list_for_each_entry_safe(r, tmp, &fw->ctrl->routes_list, list)
		if (r->tb_id)
			otto_l3_route_teardown(fw->ctrl, r);
	rtnl_unlock();
}

/* A dump that raced a change to the FIB leaves programmed what it saw, and the
 * events in between never arrived. The kernel unregisters the notifier, calls
 * this and dumps again, so every route the last dump brought goes - all but
 * the catch-all rows, which no table owns - once the work it queued has run.
 * The teardown runs on the same single threaded queue, so nothing else walks
 * the routes meanwhile.
 */
static void otto_l3_fib_dump_flush(struct notifier_block *nb)
{
	struct otto_l3_ctrl *ctrl = container_of(nb, struct otto_l3_ctrl, fib_nb);
	struct otto_l3_flush_work fw = { .ctrl = ctrl };

	INIT_WORK_ONSTACK(&fw.work, otto_l3_fib_flush_work_do);
	queue_work(ctrl->priv->wq, &fw.work);
	flush_work(&fw.work);
	destroy_work_on_stack(&fw.work);
}

/* An event the notifiers had no memory to queue is lost, and with it what it
 * would have changed: a deleted route or a neighbour gone can stay forwarding.
 * Take every route out and have the kernel replay the FIB, as when the notifier
 * is first registered. The stale entry forwards until the flush, which waits
 * for the work already queued; from there until the replay has brought the
 * routes back, the catch-all rows trap what comes.
 */
static void otto_l3_resync_work_do(struct work_struct *work)
{
	struct otto_l3_ctrl *ctrl = container_of(to_delayed_work(work), struct otto_l3_ctrl,
						 resync_work);
	int err;

	WRITE_ONCE(ctrl->resync_wanted, false);
	if (!ctrl->resync_delay)
		dev_warn(ctrl->dev, "an L3 event could not be queued, replaying the FIB\n");
	unregister_fib_notifier(&init_net, &ctrl->fib_nb);
	otto_l3_fib_dump_flush(&ctrl->fib_nb);

	err = register_fib_notifier(&init_net, &ctrl->fib_nb, otto_l3_fib_dump_flush, NULL);
	if (!err) {
		ctrl->resync_delay = 0;
		return;
	}

	/* A dump that failed half way is not flushed by the kernel */
	otto_l3_fib_dump_flush(&ctrl->fib_nb);

	/* Memory may still be short, or a route the notifier refuses may have come
	 * in while it was away: try again, less and less often
	 */
	ctrl->resync_delay = clamp(2 * ctrl->resync_delay, HZ, 64 * HZ);
	dev_err(ctrl->dev, "FIB replay failed: %d, routes stay in software, next try in %us\n",
		err, ctrl->resync_delay / HZ);
	mod_delayed_work(system_long_wq, &ctrl->resync_work, ctrl->resync_delay);
}

int otto_l3_probe(struct device *dev, struct rtl838x_switch_priv *priv)
{
	const struct of_device_id *match;
	struct otto_l3_ctrl *ctrl;
	int err;

	ctrl = devm_kzalloc(dev, sizeof(struct otto_l3_ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;
	priv->l3_ctrl = ctrl;
	ctrl->priv = priv;
	ctrl->dev = priv->dev;
	/* For now share the register access lock with the DSA driver */
	ctrl->lock = &priv->reg_mutex;
	/* Until the FIB notifier is in place there is nothing to replay */
	INIT_DELAYED_WORK(&ctrl->resync_work, otto_l3_resync_work_do);
	disable_delayed_work(&ctrl->resync_work);

	match = of_match_node(otto_l3_of_ids, dev->of_node);
	if (!match)
		return dev_err_probe(dev, -EINVAL, "No compatible configuration found\n");
	ctrl->cfg = match->data;

	if (ctrl->cfg->setup) {
		err = ctrl->cfg->setup(ctrl);
		if (err)
			return dev_err_probe(dev, err, "device specific L3 setup failed\n");
	}

	INIT_LIST_HEAD(&ctrl->routes_list);

	/* Before the notifiers, so no destination is dropped in the window
	 * where the tables are live and the routes have not arrived yet.
	 */
	if (ctrl->cfg->use_l3_tables && otto_l3_add_catch_all(ctrl, ROUTE_TYPE_IP6UC))
		dev_err(dev, "no row for the IPv6 catch-all, destinations without one will be dropped\n");

	if (ctrl->cfg->use_l3_tables && otto_l3_add_catch_all(ctrl, ROUTE_TYPE_IP4UC))
		dev_err(dev, "no row for the IPv4 catch-all, destinations without one will be dropped\n");

	/*
	 * Register netevent notifier callback to catch notifications about neighboring changes
	 * to update nexthop entries for L3 routing.
	 */
	ctrl->ne_nb.notifier_call = otto_l3_netevent_notifier;
	err = register_netevent_notifier(&ctrl->ne_nb);
	if (err) {
		ctrl->ne_nb.notifier_call = NULL;
		otto_l3_remove(priv);
		return dev_err_probe(dev, err, "Failed to register netevent notifier\n");
	}

	/*
	 * Register Forwarding Information Base notifier to offload routes where possible. Only
	 * FIBs pointing to our own netdevs are programmed into the device.
	 */
	ctrl->fib_nb.notifier_call = otto_l3_fib_notifier;
	err = register_fib_notifier(&init_net, &ctrl->fib_nb, otto_l3_fib_dump_flush, NULL);
	if (err) {
		ctrl->fib_nb.notifier_call = NULL;
		otto_l3_remove(priv);
		return dev_err_probe(dev, err, "Failed to register fib event notifier\n");
	}
	/* Only the L3 tables trap while the routes are away */
	if (ctrl->cfg->use_l3_tables) {
		enable_delayed_work(&ctrl->resync_work);
		smp_mb(); /* pairs with the one in otto_l3_resync_request() */
		if (READ_ONCE(ctrl->resync_wanted))
			queue_delayed_work(system_long_wq, &ctrl->resync_work, 0);
	}

	if (ctrl->cfg->dbgfs_init)
		ctrl->cfg->dbgfs_init(ctrl);

	return 0;
}
