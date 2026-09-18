// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>

#include "l2.h"
#include "rtl-otto.h"

u64 rtl838x_l2_hash_seed(u64 mac, u32 vid)
{
	return mac << 12 | vid;
}

/* Applies the same hash algorithm as the one used currently by the ASIC to the seed
 * and returns a key into the L2 hash table
 */
u32 rtl838x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 h1, h2, h3, h;

	if (sw_r32(priv->r->l2_ctrl_0) & 1) {
		h1 = (seed >> 11) & 0x7ff;
		h1 = ((h1 & 0x1f) << 6) | ((h1 >> 5) & 0x3f);

		h2 = (seed >> 33) & 0x7ff;
		h2 = ((h2 & 0x3f) << 5) | ((h2 >> 6) & 0x1f);

		h3 = (seed >> 44) & 0x7ff;
		h3 = ((h3 & 0x7f) << 4) | ((h3 >> 7) & 0xf);

		h = h1 ^ h2 ^ h3 ^ ((seed >> 55) & 0x1ff);
		h ^= ((seed >> 22) & 0x7ff) ^ (seed & 0x7ff);
	} else {
		h = ((seed >> 55) & 0x1ff) ^ ((seed >> 44) & 0x7ff) ^
		    ((seed >> 33) & 0x7ff) ^ ((seed >> 22) & 0x7ff) ^
		    ((seed >> 11) & 0x7ff) ^ (seed & 0x7ff);
	}

	return h;
}

int rtl838x_l2_port_new_salrn(int p)
{
	return RTL838X_L2_PORT_NEW_SALRN(p);
}

int rtl838x_l2_port_new_sa_fwd(int p)
{
	return RTL838X_L2_PORT_NEW_SA_FWD(p);
}

/* Fills an L2 entry structure from the SoC registers */
static void rtl838x_fill_l2_entry(u32 r[], struct rtl838x_l2_entry *e)
{
	/* Table contains different entry types, we need to identify the right one:
	 * Check for MC entries, first
	 * In contrast to the RTL93xx SoCs, there is no valid bit, use heuristics to
	 * identify valid entries
	 */
	e->is_ip_mc = !!(r[0] & BIT(22));
	e->is_ipv6_mc = !!(r[0] & BIT(21));
	e->type = L2_INVALID;

	if (!e->is_ip_mc && !e->is_ipv6_mc) {
		e->mac[0] = (r[1] >> 20);
		e->mac[1] = (r[1] >> 12);
		e->mac[2] = (r[1] >> 4);
		e->mac[3] = (r[1] & 0xf) << 4 | (r[2] >> 28);
		e->mac[4] = (r[2] >> 20);
		e->mac[5] = (r[2] >> 12);

		e->rvid = r[2] & 0xfff;
		e->vid = r[0] & 0xfff;

		/* Is it a unicast entry? check multicast bit */
		if (!(e->mac[0] & 1)) {
			e->is_static = !!((r[0] >> 19) & 1);
			e->port = (r[0] >> 12) & 0x1f;
			e->block_da = !!(r[1] & BIT(30));
			e->block_sa = !!(r[1] & BIT(31));
			e->suspended = !!(r[1] & BIT(29));
			e->next_hop = !!(r[1] & BIT(28));
			if (e->next_hop) {
				pr_debug("Found next hop entry, need to read extra data\n");
				e->nh_vlan_target = !!(r[0] & BIT(9));
				e->nh_route_id = r[0] & 0x1ff;
				e->vid = e->rvid;
			}
			e->age = (r[0] >> 17) & 0x3;
			e->valid = true;

			/* A valid entry has one of multi-cast, aging, sa/da-blocking,
			 * next-hop or static entry bit set
			 */
			if (!(r[0] & 0x007c0000) && !(r[1] & 0xd0000000))
				e->valid = false;
			else
				e->type = L2_UNICAST;
		} else { /* L2 multicast */
			pr_debug("Got L2 MC entry: %08x %08x %08x\n", r[0], r[1], r[2]);
			e->valid = true;
			e->type = L2_MULTICAST;
			e->mc_portmask_index = (r[0] >> 12) & 0x1ff;
		}
	} else { /* IPv4 and IPv6 multicast */
		e->valid = true;
		e->mc_portmask_index = (r[0] >> 12) & 0x1ff;
		e->mc_gip = (r[1] << 20) | (r[2] >> 12);
		e->rvid = r[2] & 0xfff;
	}
	if (e->is_ip_mc)
		e->type = IP4_MULTICAST;
	if (e->is_ipv6_mc)
		e->type = IP6_MULTICAST;
}

/* Fills the 3 SoC table registers r[] with the information of in the rtl838x_l2_entry */
static void rtl838x_fill_l2_row(u32 r[], struct rtl838x_l2_entry *e)
{
	u64 mac = ether_addr_to_u64(e->mac);

	if (!e->valid) {
		r[0] = r[1] = r[2] = 0;
		return;
	}

	r[0] = e->is_ip_mc ? BIT(22) : 0;
	r[0] |= e->is_ipv6_mc ? BIT(21) : 0;

	if (!e->is_ip_mc && !e->is_ipv6_mc) {
		r[1] = mac >> 20;
		r[2] = (mac & 0xfffff) << 12;

		/* Is it a unicast entry? check multicast bit */
		if (!(e->mac[0] & 1)) {
			r[0] |= e->is_static ? BIT(19) : 0;
			r[0] |= (e->port & 0x3f) << 12;
			r[0] |= e->vid;
			r[1] |= e->block_da ? BIT(30) : 0;
			r[1] |= e->block_sa ? BIT(31) : 0;
			r[1] |= e->suspended ? BIT(29) : 0;
			r[2] |= e->rvid & 0xfff;
			if (e->next_hop) {
				r[1] |= BIT(28);
				r[0] |= e->nh_vlan_target ? BIT(9) : 0;
				r[0] |= e->nh_route_id & 0x1ff;
			}
			r[0] |= (e->age & 0x3) << 17;
		} else { /* L2 Multicast */
			r[0] |= (e->mc_portmask_index & 0x1ff) << 12;
			r[2] |= e->rvid & 0xfff;
			r[0] |= e->vid & 0xfff;
			pr_debug("FILL MC: %08x %08x %08x\n", r[0], r[1], r[2]);
		}
	} else { /* IPv4 and IPv6 multicast */
		r[0] |= (e->mc_portmask_index & 0x1ff) << 12;
		r[1] = e->mc_gip >> 20;
		r[2] = e->mc_gip << 12;
		r[2] |= e->rvid;
	}
}

/* Read an L2 UC or MC entry out of a hash bucket of the L2 forwarding table
 * hash is the id of the bucket and pos is the position of the entry in that bucket
 * The data read from the SoC is filled into rtl838x_l2_entry
 */
u64 rtl838x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Search SRAM, with hash and at pos in bucket */

	/* Access L2 Table 0 */
	otto_table_read(RTL8380_TBL_L2_UC, idx, &r);

	rtl838x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	return (((u64)r[1]) << 32) | (r[2]);  /* mac and vid concatenated as hash seed */
}

void rtl838x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Access SRAM, with hash and at pos in bucket */

	rtl838x_fill_l2_row(r, e);

	/* Access L2 Table 0 */
	otto_table_write(RTL8380_TBL_L2_UC, idx, &r);
}

u64 rtl838x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	/* Access L2 Table 1 */
	otto_table_read(RTL8380_TBL_L2_CAM_UC, idx, &r);

	rtl838x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	pr_debug("Found in CAM: R1 %x R2 %x R3 %x\n", r[0], r[1], r[2]);

	/* Return MAC with concatenated VID ac concatenated ID */
	return (((u64)r[1]) << 32) | r[2];
}

void rtl838x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	rtl838x_fill_l2_row(r, e);

	/* Access L2 Table 1 */
	otto_table_write(RTL8380_TBL_L2_CAM_UC, idx, &r);
}

u64 rtl838x_read_mcast_pmask(int idx)
{
	u32 portmask;

	otto_table_read(RTL8380_TBL_MC_PMSK, idx, &portmask);

	return portmask;
}

void rtl838x_write_mcast_pmask(int idx, u64 portmask)
{
	u32 buf[1] = { ((u32)portmask) & RTL838X_MC_PMASK_ALL_PORTS };

	otto_table_write(RTL8380_TBL_MC_PMSK, idx, &buf);
}

void rtl838x_l2_learning_setup(void)
{
	/* Set portmask for broadcast traffic and unknown unicast address flooding
	 * to the reserved entry in the portmask table used also for
	 * multicast flooding
	 */
	sw_w32(RTL838X_L2_BC_FLD(MC_PMASK_ALL_PORTS_IDX) |
	       RTL838X_L2_UNKN_UC_FLD(MC_PMASK_ALL_PORTS_IDX),
	       RTL838X_L2_FLD_PMSK);

	/* Enable learning constraint system-wide (bit 0), per-port (bit 1)
	 * and per vlan (bit 2)
	 */
	sw_w32(0x7, RTL838X_L2_LRN_CONSTRT_EN);

	/* Limit learning to maximum: 16k entries, after that just flood (bits 0-1) */
	sw_w32((0x3fff << 2) | 0, RTL838X_L2_LRN_CONSTRT);

	/* Do not trap ARP packets to CPU_PORT */
	sw_w32(0, RTL838X_SPCL_TRAP_ARP_CTRL);
}

void rtl838x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 16k entries */

	sw_w32_mask(0x3fff << 2, enable ? (0x3fff << 2) : 0,
		    RTL838X_L2_PORT_LRN_CONSTRT + (port << 2));
}

void rtl838x_enable_flood(int port, enum rtldsa_flood_type mode)
{
	/* 0: Forward
	 * 1: Disable
	 * 2: to CPU
	 * 3: Copy to CPU
	 */
	sw_w32_mask(0x3, mode,
		    RTL838X_L2_PORT_LRN_CONSTRT + (port << 2));
}

void rtl838x_enable_mcast_flood(int port, bool enable)
{
}

void rtl838x_enable_bcast_flood(int port, bool enable)
{
}

void rtl838x_set_static_move_action(int port, bool forward)
{
	int shift = MV_ACT_PORT_SHIFT(port);
	u32 val = forward ? MV_ACT_FORWARD : MV_ACT_DROP;

	sw_w32_mask(MV_ACT_MASK << shift, val << shift,
		    RTL838X_L2_PORT_STATIC_MV_ACT(port));
}

int rtldsa_838x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
{
	u32 val;

	val = BIT(26) | BIT(23) | (port << 5);
	if (vid >= 0)
		val |= BIT(24) | (vid << 10);

	sw_w32(val, priv->r->l2_tbl_flush_ctrl);
	do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & BIT(26));

	return 0;
}

int rtl838x_set_ageing_time(unsigned long msec)
{
	int t = sw_r32(RTL838X_L2_CTRL_1);

	t &= 0x7FFFFF;
	t = t * 128 / 625; /* Aging time in seconds. 0: L2 aging disabled */
	pr_debug("L2 AGING time: %d sec\n", t);

	t = (msec * 625 + 127000) / 128000;
	t = t > 0x7FFFFF ? 0x7FFFFF : t;
	sw_w32_mask(0x7FFFFF, t, RTL838X_L2_CTRL_1);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(RTL838X_L2_PORT_AGING_OUT));

	return 0;
}

/* Hash seed is vid (actually rvid) concatenated with the MAC address */
u64 rtl839x_l2_hash_seed(u64 mac, u32 vid)
{
	u64 v = vid;

	v <<= 48;
	v |= mac;

	return v;
}

/* Applies the same hash algorithm as the one used currently by the ASIC to the seed
 * and returns a key into the L2 hash table
 */
u32 rtl839x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 h1, h2, h;

	if (sw_r32(priv->r->l2_ctrl_0) & 1) {
		h1 = (u32)(((seed >> 60) & 0x3f) ^ ((seed >> 54) & 0x3f) ^
			   ((seed >> 36) & 0x3f) ^ ((seed >> 30) & 0x3f) ^
			   ((seed >> 12) & 0x3f) ^ ((seed >> 6) & 0x3f));
		h2 = (u32)(((seed >> 48) & 0x3f) ^ ((seed >> 42) & 0x3f) ^
			   ((seed >> 24) & 0x3f) ^ ((seed >> 18) & 0x3f) ^
			   (seed & 0x3f));
		h = (h1 << 6) | h2;
	} else {
		h = (seed >> 60) ^
		    ((((seed >> 48) & 0x3f) << 6) | ((seed >> 54) & 0x3f)) ^
		    ((seed >> 36) & 0xfff) ^ ((seed >> 24) & 0xfff) ^
		    ((seed >> 12) & 0xfff) ^ (seed & 0xfff);
	}

	return h;
}

int rtl839x_l2_port_new_salrn(int p)
{
	return RTL839X_L2_PORT_NEW_SALRN(p);
}

int rtl839x_l2_port_new_sa_fwd(int p)
{
	return RTL839X_L2_PORT_NEW_SA_FWD(p);
}

static void rtl839x_fill_l2_entry(u32 r[], struct rtl838x_l2_entry *e)
{
	/* Table contains different entry types, we need to identify the right one:
	 * Check for MC entries, first
	 */
	e->is_ip_mc = !!(r[2] & BIT(31));
	e->is_ipv6_mc = !!(r[2] & BIT(30));
	e->type = L2_INVALID;
	if (!e->is_ip_mc && !e->is_ipv6_mc) {
		e->mac[0] = (r[0] >> 12);
		e->mac[1] = (r[0] >> 4);
		e->mac[2] = ((r[1] >> 28) | (r[0] << 4));
		e->mac[3] = (r[1] >> 20);
		e->mac[4] = (r[1] >> 12);
		e->mac[5] = (r[1] >> 4);

		e->vid = (r[2] >> 4) & 0xfff;
		e->rvid = (r[0] >> 20) & 0xfff;

		/* Is it a unicast entry? check multicast bit */
		if (!(e->mac[0] & 1)) {
			e->is_static = !!((r[2] >> 18) & 1);
			e->port = (r[2] >> 24) & 0x3f;
			e->block_da = !!(r[2] & (1 << 19));
			e->block_sa = !!(r[2] & (1 << 20));
			e->suspended = !!(r[2] & (1 << 17));
			e->next_hop = !!(r[2] & (1 << 16));
			if (e->next_hop) {
				pr_debug("Found next hop entry, need to read data\n");
				e->nh_vlan_target = !!(r[2] & BIT(15));
				e->nh_route_id = (r[2] >> 4) & 0x1ff;
				e->vid = e->rvid;
			}
			e->age = (r[2] >> 21) & 0x7;
			e->valid = true;
			if (!(r[2] & 0xc0fd0000)) /* Check for valid entry */
				e->valid = false;
			else
				e->type = L2_UNICAST;
		} else {
			e->valid = true;
			e->type = L2_MULTICAST;
			e->mc_portmask_index = (r[2] >> 6) & 0xfff;
			e->vid = e->rvid;
		}
	} else { /* IPv4 and IPv6 multicast */
		e->vid = e->rvid = (r[0] << 20) & 0xfff;
		e->mc_gip = r[1];
		e->mc_portmask_index = (r[2] >> 6) & 0xfff;
	}
	if (e->is_ip_mc) {
		e->valid = true;
		e->type = IP4_MULTICAST;
	}
	if (e->is_ipv6_mc) {
		e->valid = true;
		e->type = IP6_MULTICAST;
	}
	/* pr_debug("%s: vid %d, rvid: %d\n", __func__, e->vid, e->rvid); */
}

/* Fills the 3 SoC table registers r[] with the information in the rtl838x_l2_entry */
static void rtl839x_fill_l2_row(u32 r[], struct rtl838x_l2_entry *e)
{
	if (!e->valid) {
		r[0] = r[1] = r[2] = 0;
		return;
	}

	r[2] = e->is_ip_mc ? BIT(31) : 0;
	r[2] |= e->is_ipv6_mc ? BIT(30) : 0;

	if (!e->is_ip_mc  && !e->is_ipv6_mc) {
		r[0] = ((u32)e->mac[0]) << 12;
		r[0] |= ((u32)e->mac[1]) << 4;
		r[0] |= ((u32)e->mac[2]) >> 4;
		r[1] = ((u32)e->mac[2]) << 28;
		r[1] |= ((u32)e->mac[3]) << 20;
		r[1] |= ((u32)e->mac[4]) << 12;
		r[1] |= ((u32)e->mac[5]) << 4;

		if (!(e->mac[0] & 1)) { /* Not multicast */
			r[2] |= e->is_static ? BIT(18) : 0;
			r[0] |= ((u32)e->rvid) << 20;
			r[2] |= e->port << 24;
			r[2] |= e->block_da ? BIT(19) : 0;
			r[2] |= e->block_sa ? BIT(20) : 0;
			r[2] |= e->suspended ? BIT(17) : 0;
			r[2] |= (e->age & 0x7) << 21;
			if (e->next_hop) {
				r[2] |= BIT(16);
				r[2] |= e->nh_vlan_target ? BIT(15) : 0;
				r[2] |= (e->nh_route_id & 0x7ff) << 4;
			} else {
				r[2] |= e->vid << 4;
			}
			pr_debug("Write L2 NH: %08x %08x %08x\n", r[0], r[1], r[2]);
		} else {  /* L2 Multicast */
			r[0] |= ((u32)e->rvid) << 20;
			r[2] |= ((u32)e->mc_portmask_index) << 6;
		}
	} else { /* IPv4 or IPv6 MC entry */
		r[0] = ((u32)e->rvid) << 20;
		r[1] = e->mc_gip;
		r[2] |= ((u32)e->mc_portmask_index) << 6;
	}
}

/* Read an L2 UC or MC entry out of a hash bucket of the L2 forwarding table
 * hash is the id of the bucket and pos is the position of the entry in that bucket
 * The data read from the SoC is filled into rtl838x_l2_entry
 */
u64 rtl839x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Search SRAM, with hash and at pos in bucket */

	otto_table_read(RTL8390_TBL_L2_UC, idx, &r);

	rtl839x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	return rtl839x_l2_hash_seed(ether_addr_to_u64(&e->mac[0]), e->rvid);
}

void rtl839x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Access SRAM, with hash and at pos in bucket */

	rtl839x_fill_l2_row(r, e);

	otto_table_write(RTL8390_TBL_L2_UC, idx, &r);
}

u64 rtl839x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	/* Access L2 Table 1 */
	otto_table_read(RTL8390_TBL_L2_CAM_UC, idx, &r);

	rtl839x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	pr_debug("Found in CAM: R1 %x R2 %x R3 %x\n", r[0], r[1], r[2]);

	/* Return MAC with concatenated VID ac concatenated ID */
	return rtl839x_l2_hash_seed(ether_addr_to_u64(&e->mac[0]), e->rvid);
}

void rtl839x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	rtl839x_fill_l2_row(r, e);

	/* Access L2 Table 1 */
	otto_table_write(RTL8390_TBL_L2_CAM_UC, idx, &r);
}

u64 rtl839x_read_mcast_pmask(int idx)
{
	u32 buf[2];
	u64 portmask;

	otto_table_read(RTL8390_TBL_MC_PMSK, idx, &buf);
	portmask = buf[0];
	portmask <<= 32;
	portmask |= buf[1];
	portmask >>= 11;  /* LSB is bit 11 in data registers */

	return portmask;
}

void rtl839x_write_mcast_pmask(int idx, u64 portmask)
{
	u32 buf[2];

	portmask <<= 11; /* LSB is bit 11 in data registers */
	buf[0] = (u32)(portmask >> 32);
	buf[1] = (u32)(portmask & 0xfffff800);

	otto_table_write(RTL8390_TBL_MC_PMSK, idx, &buf);
}

void rtl839x_l2_learning_setup(void)
{
	/* Set portmask for broadcast (offset bit 12) and unknown unicast (offset 0)
	 * address flooding to the reserved entry in the portmask table used
	 * also for multicast flooding
	 */
	sw_w32(RTL839X_L2_BC_FLD(MC_PMASK_ALL_PORTS_IDX) |
	       RTL839X_L2_UNKN_UC_FLD(MC_PMASK_ALL_PORTS_IDX),
	       RTL839X_L2_FLD_PMSK);

	/* Limit learning to maximum: 32k entries, after that just flood (bits 0-1) */
	sw_w32((0x7fff << 2) | 0, RTL839X_L2_LRN_CONSTRT);

	/* Do not trap ARP packets to CPU_PORT */
	sw_w32(0, RTL839X_SPCL_TRAP_ARP_CTRL);
}

void rtl839x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 32k entries */

	sw_w32_mask(0x7fff << 2, enable ? (0x7fff << 2) : 0,
		    RTL839X_L2_PORT_LRN_CONSTRT + (port << 2));
}

void rtl839x_enable_flood(int port, enum rtldsa_flood_type mode)
{
	/* 0: Forward
	 * 1: Disable
	 * 2: to CPU
	 * 3: Copy to CPU
	 */
	sw_w32_mask(0x3, mode,
		    RTL839X_L2_PORT_LRN_CONSTRT + (port << 2));
}

void rtl839x_enable_mcast_flood(int port, bool enable)
{
}

void rtl839x_enable_bcast_flood(int port, bool enable)
{
}

void rtl839x_set_static_move_action(int port, bool forward)
{
	int shift = MV_ACT_PORT_SHIFT(port);
	u32 val = forward ? MV_ACT_FORWARD : MV_ACT_DROP;

	sw_w32_mask(MV_ACT_MASK << shift, val << shift,
		    RTL839X_L2_PORT_STATIC_MV_ACT(port));
}

int rtldsa_839x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
{
	u32 val;

	val = BIT(28) | BIT(25) | (port << 6);
	if (vid >= 0)
		val |= BIT(26) | (vid << 12);

	sw_w32(val, priv->r->l2_tbl_flush_ctrl);
	do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & BIT(28));

	return 0;
}

int rtl839x_set_ageing_time(unsigned long msec)
{
	int t = sw_r32(RTL839X_L2_CTRL_1);

	t &= 0x1FFFFF;
	t = t * 3 / 5; /* Aging time in seconds. 0: L2 aging disabled */
	pr_debug("L2 AGING time: %d sec\n", t);

	t = (msec * 5 + 2000) / 3000;
	t = t > 0x1FFFFF ? 0x1FFFFF : t;
	sw_w32_mask(0x1FFFFF, t, RTL839X_L2_CTRL_1);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(RTL839X_L2_PORT_AGING_OUT));

	return 0;
}

int rtl930x_l2_port_new_salrn(int p)
{
	return RTL930X_L2_PORT_SALRN(p);
}

int rtl930x_l2_port_new_sa_fwd(int p)
{
	/* TODO: The definition of the fields changed, because of the master-cpu in a stack */
	return RTL930X_L2_PORT_NEW_SA_FWD(p);
}

void rtl930x_l2_learning_setup(void)
{
	/* Portmask for flooding broadcast traffic */
	sw_w32(RTL930X_MC_PMASK_ALL_PORTS, RTL930X_L2_BC_FLD_PMSK);

	/* Portmask for flooding unicast traffic with unknown destination */
	sw_w32(RTL930X_MC_PMASK_ALL_PORTS, RTL930X_L2_UNKN_UC_FLD_PMSK);

	/* Limit learning to maximum: 32k entries, after that just flood (bits 0-1) */
	sw_w32((0x7fff << 2) | 0, RTL930X_L2_LRN_CONSTRT_CTRL);
}

void rtldsa_930x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 32k entries */
	sw_w32_mask(GENMASK(17, 3), enable ? (0x7ffe << 3) : 0,
		    RTL930X_L2_LRN_PORT_CONSTRT_CTRL + port * 4);
}

void rtldsa_930x_l2_port_new_sa_fwd(int port, enum rtldsa_flood_type mode)
{
	u32 new_sa_fwd_shift = (port % 10) * 3;

	sw_w32_mask(GENMASK(new_sa_fwd_shift + 2, new_sa_fwd_shift),
		    mode << new_sa_fwd_shift,
		    rtl930x_l2_port_new_sa_fwd(port));
}

void rtldsa_930x_enable_flood(int port, enum rtldsa_flood_type mode)
{
	u32 port_mask = BIT(port);
	u32 val;

	val = (mode == RTLDSA_FLOOD_TYPE_FORWARD) ? port_mask : 0;

	sw_w32_mask(GENMASK(2, 0), mode,
		    RTL930X_L2_LRN_PORT_CONSTRT_CTRL + port * 4);

	sw_w32_mask(port_mask,
		    val,
		    RTL930X_L2_UNKN_UC_FLD_PMSK);
}

void rtldsa_930x_enable_bcast_flood(int port, bool enable)
{
	u32 port_mask = BIT(port);

	sw_w32_mask(port_mask,
		    enable ? port_mask : 0,
		    RTL930X_L2_BC_FLD_PMSK);
}

u64 rtl930x_l2_hash_seed(u64 mac, u32 vid)
{
	u64 v = vid;

	v <<= 48;
	v |= mac;

	return v;
}

/* Calculate both the block 0 and the block 1 hash by applyingthe same hash
 * algorithm as the one used currently by the ASIC to the seed, and return
 * both hashes in the lower and higher word of the return value since only 12 bit of
 * the hash are significant
 */
u32 rtl930x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 k0, k1, h1, h2, h;

	k0 = (u32)(((seed >> 55) & 0x1f) ^
		   ((seed >> 44) & 0x7ff) ^
		   ((seed >> 33) & 0x7ff) ^
		   ((seed >> 22) & 0x7ff) ^
		   ((seed >> 11) & 0x7ff) ^
		   (seed & 0x7ff));

	h1 = (seed >> 11) & 0x7ff;
	h1 = ((h1 & 0x1f) << 6) | ((h1 >> 5) & 0x3f);

	h2 = (seed >> 33) & 0x7ff;
	h2 = ((h2 & 0x3f) << 5) | ((h2 >> 6) & 0x3f);

	k1 = (u32)(((seed << 55) & 0x1f) ^
		   ((seed >> 44) & 0x7ff) ^
		   h2 ^
		   ((seed >> 22) & 0x7ff) ^
		   h1 ^
		   (seed & 0x7ff));

	/* Algorithm choice for block 0 */
	if (sw_r32(RTL930X_L2_CTRL) & BIT(0))
		h = k1;
	else
		h = k0;

	/* Algorithm choice for block 1
	 * Since k0 and k1 are < 2048, adding 2048 will offset the hash into the second
	 * half of hash-space
	 * 2048 is in fact the hash-table size 16384 divided by 4 hashes per bucket
	 * divided by 2 to divide the hash space in 2
	 */
	if (sw_r32(RTL930X_L2_CTRL) & BIT(1))
		h |= (k1 + 2048) << 16;
	else
		h |= (k0 + 2048) << 16;

	return h;
}

/* Fills an L2 entry structure from the SoC registers */
static void rtl930x_fill_l2_entry(u32 r[], struct rtl838x_l2_entry *e)
{
	pr_debug("In %s valid?\n", __func__);
	e->valid = !!(r[2] & BIT(31));
	if (!e->valid)
		return;

	pr_debug("In %s is valid\n", __func__);
	e->is_ip_mc = false;
	e->is_ipv6_mc = false;

	/* TODO: Is there not a function to copy directly MAC memory? */
	e->mac[0] = (r[0] >> 24);
	e->mac[1] = (r[0] >> 16);
	e->mac[2] = (r[0] >> 8);
	e->mac[3] = r[0];
	e->mac[4] = (r[1] >> 24);
	e->mac[5] = (r[1] >> 16);

	e->next_hop = !!(r[2] & BIT(12));
	e->rvid = r[1] & 0xfff;

	/* Is it a unicast entry? check multicast bit */
	if (!(e->mac[0] & 1)) {
		e->type = L2_UNICAST;
		e->is_static = !!(r[2] & BIT(14));
		e->port = (r[2] >> 20) & 0x3ff;
		/* Check for trunk port */
		if (r[2] & BIT(30)) {
			e->is_trunk = true;
			e->stack_dev = (e->port >> 9) & 1;
			e->trunk = e->port & 0x3f;
		} else {
			e->is_trunk = false;
			e->stack_dev = (e->port >> 6) & 0xf;
			e->port = e->port & 0x3f;
		}

		e->block_da = !!(r[2] & BIT(15));
		e->block_sa = !!(r[2] & BIT(16));
		e->suspended = !!(r[2] & BIT(13));
		e->age = (r[2] >> 17) & 0x7;
		e->valid = true;
		/* the UC_VID field in hardware is used for the VID or for the route id */
		if (e->next_hop) {
			e->nh_route_id = r[2] & 0x7ff;
			e->vid = 0;
		} else {
			e->vid = r[2] & 0xfff;
			e->nh_route_id = 0;
		}
	} else {
		e->valid = true;
		e->type = L2_MULTICAST;
		e->mc_portmask_index = (r[2] >> 16) & 0x3ff;
	}
}

/* Fills the 3 SoC table registers r[] with the information of in the rtl838x_l2_entry */
static void rtl930x_fill_l2_row(u32 r[], struct rtl838x_l2_entry *e)
{
	u32 port;

	if (!e->valid) {
		r[0] = r[1] = r[2] = 0;
		return;
	}

	r[2] = BIT(31);	/* Set valid bit */

	r[0] = ((u32)e->mac[0]) << 24 |
	       ((u32)e->mac[1]) << 16 |
	       ((u32)e->mac[2]) << 8 |
	       ((u32)e->mac[3]);
	r[1] = ((u32)e->mac[4]) << 24 |
	       ((u32)e->mac[5]) << 16;

	r[2] |= e->next_hop ? BIT(12) : 0;

	if (e->type == L2_UNICAST) {
		r[2] |= e->is_static ? BIT(14) : 0;
		r[1] |= e->rvid & 0xfff;
		if (e->is_trunk) {
			r[2] |= BIT(30);
			port = e->stack_dev << 9 | (e->port & 0x3f);
		} else {
			port = (e->stack_dev & 0xf) << 6;
			port |= e->port & 0x3f;
		}
		r[2] |= port << 20;
		r[2] |= e->block_da ? BIT(15) : 0;
		r[2] |= e->block_sa ? BIT(16) : 0;
		r[2] |= e->suspended ? BIT(13) : 0;
		r[2] |= (e->age & 0x7) << 17;
		/* the UC_VID field in hardware is used for the VID or for the route id */
		if (e->next_hop)
			r[2] |= e->nh_route_id & 0x7ff;
		else
			r[2] |= e->vid & 0xfff;
	} else { /* L2_MULTICAST */
		r[2] |= (e->mc_portmask_index & 0x3ff) << 16;
		r[2] |= e->mc_mac_index & 0x7ff;
	}
}

/* Read an L2 UC or MC entry out of a hash bucket of the L2 forwarding table
 * hash is the id of the bucket and pos is the position of the entry in that bucket
 * The data read from the SoC is filled into rtl838x_l2_entry
 */
u64 rtl930x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx;
	u64 mac;
	u64 seed;

	pr_debug("%s: hash %08x, pos: %d\n", __func__, hash, pos);

	/* On the RTL93xx, 2 different hash algorithms are used making it a
	 * total of 8 buckets that need to be searched, 4 for each hash-half
	 * Use second hash space when bucket is between 4 and 8
	 */
	if (pos >= 4) {
		pos -= 4;
		hash >>= 16;
	} else {
		hash &= 0xffff;
	}

	idx = (0 << 14) | (hash << 2) | pos; /* Search SRAM, with hash and at pos in bucket */
	pr_debug("%s: NOW hash %08x, pos: %d\n", __func__, hash, pos);

	otto_table_read(RTL9300_TBL_L2_UC, idx, &r);

	rtl930x_fill_l2_entry(r, e);

	pr_debug("%s: valid: %d, nh: %d\n", __func__, e->valid, e->next_hop);
	if (!e->valid)
		return 0;

	mac = ((u64)e->mac[0]) << 40 |
	      ((u64)e->mac[1]) << 32 |
	      ((u64)e->mac[2]) << 24 |
	      ((u64)e->mac[3]) << 16 |
	      ((u64)e->mac[4]) << 8 |
	      ((u64)e->mac[5]);

	seed = rtl930x_l2_hash_seed(mac, e->rvid);
	pr_debug("%s: mac %016llx, seed %016llx\n", __func__, mac, seed);

	/* return vid with concatenated mac as unique id */
	return seed;
}

void rtl930x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Access SRAM, with hash and at pos in bucket */

	pr_debug("%s: hash %d, pos %d\n", __func__, hash, pos);
	pr_debug("%s: index %d -> mac %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, idx,
		 e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5]);

	rtl930x_fill_l2_row(r, e);

	otto_table_write(RTL9300_TBL_L2_UC, idx, &r);
}

u64 rtl930x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	otto_table_read(RTL9300_TBL_L2_CAM_UC, idx, &r);

	rtl930x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	/* return mac with concatenated vid as unique id */
	return ((u64)r[0] << 28) | ((r[1] & 0xffff0000) >> 4) | e->vid;
}

void rtl930x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	rtl930x_fill_l2_row(r, e);

	/* Access L2 Table 1 */
	otto_table_write(RTL9300_TBL_L2_CAM_UC, idx, &r);
}

u64 rtl930x_read_mcast_pmask(int idx)
{
	u32 portmask;

	otto_table_read(RTL9300_TBL_MC_PORTMASK, idx, &portmask);
	portmask >>= 3;

	pr_debug("%s: Index idx %d has portmask %08x\n", __func__, idx, portmask);

	return portmask;
}

void rtl930x_write_mcast_pmask(int idx, u64 portmask)
{
	u32 pm = portmask;

	pr_debug("%s: Index idx %d has portmask %08x\n", __func__, idx, pm);
	pm <<= 3;

	otto_table_write(RTL9300_TBL_MC_PORTMASK, idx, &pm);
}

/* Calculate both the block 0 and the block 1 hash, and return in
 * lower and higher word of the return value since only 12 bit of
 * the hash are significant
 */
u32 rtl930x_hash(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 k0, k1, h1, h2, h;

	k0 = (u32) (((seed >> 55) & 0x1f) ^
		    ((seed >> 44) & 0x7ff) ^
		    ((seed >> 33) & 0x7ff) ^
		    ((seed >> 22) & 0x7ff) ^
		    ((seed >> 11) & 0x7ff) ^
		    (seed & 0x7ff));

	h1 = (seed >> 11) & 0x7ff;
	h1 = ((h1 & 0x1f) << 6) | ((h1 >> 5) & 0x3f);

	h2 = (seed >> 33) & 0x7ff;
	h2 = ((h2 & 0x3f) << 5) | ((h2 >> 6) & 0x3f);

	k1 = (u32) (((seed << 55) & 0x1f) ^
		    ((seed >> 44) & 0x7ff) ^
		    h2 ^
		    ((seed >> 22) & 0x7ff) ^
		    h1 ^
		    (seed & 0x7ff));

	/* Algorithm choice for block 0 */
	if (sw_r32(RTL930X_L2_CTRL) & BIT(0))
		h = k1;
	else
		h = k0;

	/* Algorithm choice for block 1
	 * Since k0 and k1 are < 2048, adding 2048 will offset the hash into the second
	 * half of hash-space
	 * 2048 is in fact the hash-table size 16384 divided by 4 hashes per bucket
	 * divided by 2 to divide the hash space in 2
	 */
	if (sw_r32(RTL930X_L2_CTRL) & BIT(1))
		h |= (k1 + 2048) << 16;
	else
		h |= (k0 + 2048) << 16;

	return h;
}

int rtldsa_930x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
{
	u32 val;

	sw_w32(port << 11, RTL930X_L2_TBL_FLUSH_CTRL + 4);

	val = 0;
	val |= BIT(26); /* compare port id */
	val |= BIT(30); /* status - trigger flush */
	if (vid >= 0) {
		val |= BIT(28); /* compare VID */
		val |= vid << 12;
	}

	sw_w32(val, RTL930X_L2_TBL_FLUSH_CTRL);

	do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & BIT(30));

	return 0;
}

int rtl930x_set_ageing_time(unsigned long msec)
{
	int t = sw_r32(RTL930X_L2_AGE_CTRL);

	t &= 0x1FFFFF;
	t = (t * 7) / 10;
	pr_debug("L2 AGING time: %d sec\n", t);

	t = (msec / 100 + 6) / 7;
	t = t > 0x1FFFFF ? 0x1FFFFF : t;
	sw_w32_mask(0x1FFFFF, t, RTL930X_L2_AGE_CTRL);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(RTL930X_L2_PORT_AGE_CTRL));

	return 0;
}

int rtl931x_l2_port_new_salrn(int p)
{
	return RTL931X_L2_PORT_NEW_SALRN(p);
}

int rtl931x_l2_port_new_sa_fwd(int p)
{
	return RTL931X_L2_PORT_NEW_SA_FWD(p);
}

u64 rtldsa_931x_l2_hash_seed(u64 mac, u32 vid)
{
	return (u64)vid << 48 | mac;
}

/* Calculate both the block 0 and the block 1 hash by applyingthe same hash
 * algorithm as the one used currently by the ASIC to the seed, and return
 * both hashes in the lower and higher word of the return value since only 12 bit of
 * the hash are significant.
 */
u32 rtl931x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 h, h0, h1, h2, h3, h4, k0, k1;

	h0 = seed & 0xfff;
	h1 = (seed >> 12) & 0xfff;
	h2 = (seed >> 24) & 0xfff;
	h3 = (seed >> 36) & 0xfff;
	h4 = (seed >> 48) & 0xfff;
	h4 = ((h4 & 0x7) << 9) | ((h4 >> 3) & 0x1ff);
	k0 = h0 ^ h1 ^ h2 ^ h3 ^ h4;

	h0 = seed & 0xfff;
	h0 = ((h0 & 0x1ff) << 3) | ((h0 >> 9) & 0x7);
	h1 = (seed >> 12) & 0xfff;
	h1 = ((h1 & 0x3f) << 6) | ((h1 >> 6) & 0x3f);
	h2 = (seed >> 24) & 0xfff;
	h3 = (seed >> 36) & 0xfff;
	h3 = ((h3 & 0x3f) << 6) | ((h3 >> 6) & 0x3f);
	h4 = (seed >> 48) & 0xfff;
	k1 = h0 ^ h1 ^ h2 ^ h3 ^ h4;

	/* Algorithm choice for block 0 */
	if (sw_r32(RTL931X_L2_CTRL) & BIT(0))
		h = k1;
	else
		h = k0;

	/* Algorithm choice for block 1
	 * Since k0 and k1 are < 4096, adding 4096 will offset the hash into the second
	 * half of hash-space
	 * 4096 is in fact the hash-table size 32768 divided by 4 hashes per bucket
	 * divided by 2 to divide the hash space in 2
	 */
	if (sw_r32(RTL931X_L2_CTRL) & BIT(1))
		h |= (k1 + 4096) << 16;
	else
		h |= (k0 + 4096) << 16;

	return h;
}

/* Fills an L2 entry structure from the SoC registers */
static void rtl931x_fill_l2_entry(u32 r[], struct rtl838x_l2_entry *e)
{
	pr_debug("In %s valid?\n", __func__);
	e->valid = !!(r[0] & BIT(31));
	if (!e->valid)
		return;

	pr_debug("%s: entry valid, raw: %08x %08x %08x %08x\n", __func__, r[0], r[1], r[2], r[3]);
	e->is_ip_mc = false;
	e->is_ipv6_mc = false;

	e->mac[0] = r[0] >> 8;
	e->mac[1] = r[0];
	e->mac[2] = r[1] >> 24;
	e->mac[3] = r[1] >> 16;
	e->mac[4] = r[1] >> 8;
	e->mac[5] = r[1];

	e->is_open_flow = !!(r[0] & BIT(30));
	e->is_pe_forward = !!(r[0] & BIT(29));
	e->next_hop = !!(r[2] & BIT(30));
	e->rvid = (r[0] >> 16) & 0xfff;

	/* Is it a unicast entry? check multicast bit */
	if (!(e->mac[0] & 1)) {
		e->type = L2_UNICAST;
		e->is_l2_tunnel = !!(r[2] & BIT(31));
		e->is_static = !!(r[2] & BIT(13));
		e->port = (r[2] >> 19) & 0x3ff;
		/* Check for trunk port */
		if (r[2] & BIT(29)) {
			e->is_trunk = true;
			e->trunk = e->port & 0xff;
		} else {
			e->is_trunk = false;
			e->stack_dev = (e->port >> 6) & 0xf;
			e->port = e->port & 0x3f;
		}

		e->block_da = !!(r[2] & BIT(14));
		e->block_sa = !!(r[2] & BIT(15));
		e->suspended = !!(r[2] & BIT(12));
		e->age = (r[2] >> 16) & 7;

		/* HW doesn't use VID but FID for as key */
		e->vid = (r[0] >> 16) & 0xfff;

		if (e->is_l2_tunnel)
			e->l2_tunnel_id = ((r[2] & 0xff) << 4) | (r[3] >> 28);
		/* TODO: Implement VLAN conversion */
	} else {
		e->type = L2_MULTICAST;
		e->is_local_forward = !!(r[2] & BIT(31));
		e->is_remote_forward = !!(r[2] & BIT(17));
		e->mc_portmask_index = (r[2] >> 18) & 0xfff;
		e->l2_tunnel_list_id = (r[2] >> 4) & 0x1fff;
	}
}

/* Fills the 3 SoC table registers r[] with the information of in the rtl838x_l2_entry */
static void rtl931x_fill_l2_row(u32 r[], struct rtl838x_l2_entry *e)
{
	u32 port;

	if (!e->valid) {
		r[0] = r[1] = r[2] = r[3] = 0;
		return;
	}

	r[3] = 0;

	r[0] = BIT(31); /* Set valid bit */

	r[0] |= ((u32)e->mac[0]) << 8 |
	       ((u32)e->mac[1]);
	r[1] = ((u32)e->mac[2]) << 24 |
	       ((u32)e->mac[3]) << 16 |
		   ((u32)e->mac[4]) << 8 |
		   ((u32)e->mac[5]);

	r[0] |= e->is_open_flow ? BIT(30) : 0;
	r[0] |= e->is_pe_forward ? BIT(29) : 0;
	r[0] |= e->hash_msb ? BIT(28) : 0;
	r[2] = e->next_hop ? BIT(30) : 0;
	r[0] |= (e->rvid & 0xfff) << 16;

	if (e->type == L2_UNICAST) {
		r[2] |= e->is_l2_tunnel ? BIT(31) : 0;
		r[2] |= e->is_static ? BIT(13) : 0;

		if (e->is_trunk) {
			r[2] |= BIT(29);
			port = e->trunk & 0xff;
		} else {
			port = e->port & 0x3f;
			port |= (e->stack_dev & 0xf) << 6;
		}

		r[2] |= (port & 0x3ff) << 19;
		r[2] |= e->block_da ? BIT(14) : 0;
		r[2] |= e->block_sa ? BIT(15) : 0;
		r[2] |= e->suspended ? BIT(12) : 0;
		r[2] |= (e->age & 0x7) << 16;
		if (e->is_l2_tunnel) {
			r[2] |= (e->l2_tunnel_id >> 4) & 0xff;
			r[3] |= (e->l2_tunnel_id & 0xf) << 28;
		}
	} else { /* L2_MULTICAST */
		r[2] |= (e->mc_portmask_index & 0xfff) << 18;
	}
}

/* Read an L2 UC or MC entry out of a hash bucket of the L2 forwarding table
 * hash is the id of the bucket and pos is the position of the entry in that bucket
 * The data read from the SoC is filled into rtl838x_l2_entry
 */
u64 rtl931x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[4];
	u32 idx;
	u64 mac;
	u64 seed;

	pr_debug("%s: hash %08x, pos: %d\n", __func__, hash, pos);

	/* On the RTL93xx, 2 different hash algorithms are used making it a total of
	 * 8 buckets that need to be searched, 4 for each hash-half
	 * Use second hash space when bucket is between 4 and 8
	 */
	if (pos >= 4) {
		pos -= 4;
		hash >>= 16;
	} else {
		hash &= 0xffff;
	}

	idx = (0 << 14) | (hash << 2) | pos; /* Search SRAM, with hash and at pos in bucket */
	pr_debug("%s: NOW hash %08x, pos: %d\n", __func__, hash, pos);

	otto_table_read(RTL9310_TBL_L2_UC, idx, &r);

	rtl931x_fill_l2_entry(r, e);

	pr_debug("%s: valid: %d, nh: %d\n", __func__, e->valid, e->next_hop);
	if (!e->valid)
		return 0;

	mac = ((u64)e->mac[0]) << 40 |
	      ((u64)e->mac[1]) << 32 |
	      ((u64)e->mac[2]) << 24 |
	      ((u64)e->mac[3]) << 16 |
	      ((u64)e->mac[4]) << 8 |
	      ((u64)e->mac[5]);

	seed = rtldsa_931x_l2_hash_seed(mac, e->rvid);
	pr_debug("%s: mac %016llx, seed %016llx\n", __func__, mac, seed);

	/* return vid with concatenated mac as unique id */
	return seed;
}

u64 rtl931x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[4];

	otto_table_read(RTL9310_TBL_L2_CAM_UC, idx, &r);
	rtl931x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	/* return mac with concatenated fid as unique id */
	return ((((u64)(r[0] & 0xffff) << 32) | (u64)r[1]) << 12) | e->vid;
}

void rtl931x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[4];

	rtl931x_fill_l2_row(r, e);

	otto_table_write(RTL9310_TBL_L2_CAM_UC, idx, &r);
}

void rtl931x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[4];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Access SRAM, with hash and at pos in bucket */
	int hash_algo_id;

	pr_debug("%s: hash %d, pos %d\n", __func__, hash, pos);
	pr_debug("%s: index %d -> mac %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, idx,
		 e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5]);

	if (idx < 0x4000)
		hash_algo_id = sw_r32(RTL931X_L2_CTRL) & BIT(0);
	else
		hash_algo_id = (sw_r32(RTL931X_L2_CTRL) & BIT(1)) >> 1;

	if (hash_algo_id == 0)
		e->hash_msb = (e->rvid >> 2) & 0x1;
	else
		e->hash_msb = (e->rvid >> 11) & 0x1;

	rtl931x_fill_l2_row(r, e);
	pr_debug("%s: %d: %08x %08x %08x\n", __func__, idx, r[0], r[1], r[2]);

	otto_table_write(RTL9310_TBL_L2_UC, idx, &r);
}

void rtl931x_l2_learning_setup(void)
{
	/* Portmask for flooding broadcast traffic */
	rtl839x_set_port_reg_be(RTL931X_MC_PMASK_ALL_PORTS, RTL931X_L2_BC_FLD_PMSK);

	/* Portmask for flooding unicast traffic with unknown destination */
	rtl839x_set_port_reg_be(RTL931X_MC_PMASK_ALL_PORTS, RTL931X_L2_UNKN_UC_FLD_PMSK);

	/* Limit learning to maximum: 64k entries, after that just flood (bits 0-2) */
	sw_w32((0xffff << 3) | FORWARD, RTL931X_L2_LRN_CONSTRT_CTRL);
}

void rtldsa_931x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 64k entries */
	sw_w32_mask(GENMASK(18, 3), enable ? (0xfffe << 3) : 0,
		    RTL931X_L2_LRN_PORT_CONSTRT_CTRL + port * 4);
}

void rtldsa_931x_l2_port_new_sa_fwd(int port, enum rtldsa_flood_type mode)
{
	u32 new_sa_fwd_shift = (port % 10) * 3;

	sw_w32_mask(GENMASK(new_sa_fwd_shift + 2, new_sa_fwd_shift),
		    mode << new_sa_fwd_shift,
		    rtl931x_l2_port_new_sa_fwd(port));
}

void rtldsa_931x_enable_flood(int port, enum rtldsa_flood_type mode)
{
	/* RTL931X_L2_UNKN_UC_FLD_PMSK is big-endian */
	u32 port_offset = ((63 - port) / 32) * 4;
	u32 port_mask = BIT(port % 32);
	u32 val;

	val = (mode == RTLDSA_FLOOD_TYPE_FORWARD) ? port_mask : 0;

	sw_w32_mask(GENMASK(2, 0), mode,
		    RTL931X_L2_LRN_PORT_CONSTRT_CTRL + port * 4);

	sw_w32_mask(port_mask,
		    val,
		    RTL931X_L2_UNKN_UC_FLD_PMSK + port_offset);
}

void rtldsa_931x_enable_bcast_flood(int port, bool enable)
{
	/* RTL931X_L2_BC_FLD_PMSK is big-endian */
	u32 port_offset = ((63 - port) / 32) * 4;
	u32 port_mask = BIT(port % 32);

	sw_w32_mask(port_mask,
		    enable ? port_mask : 0,
		    RTL931X_L2_BC_FLD_PMSK + port_offset);
}

u64 rtl931x_read_mcast_pmask(int idx)
{
	u64 portmask;
	u32 buf[2];

	otto_table_read(RTL9310_TBL_MC_PMSK, idx, &buf);
	portmask = buf[0];
	portmask <<= 32;
	portmask |= buf[1];
	portmask >>= 7;

	pr_debug("%s: Index idx %d has portmask %016llx\n", __func__, idx, portmask);

	return portmask;
}

void rtl931x_write_mcast_pmask(int idx, u64 portmask)
{
	u64 pm = portmask;
	u32 buf[2];

	pr_debug("%s: Index idx %d has portmask %016llx\n", __func__, idx, pm);
	pm <<= 7;
	buf[0] = pm >> 32;
	buf[1] = pm;

	otto_table_write(RTL9310_TBL_MC_PMSK, idx, &buf);
}

int rtl931x_set_ageing_time(unsigned long msec)
{
	int t = sw_r32(RTL931X_L2_AGE_CTRL);

	t &= 0x1FFFFF;
	t = (t * 8) / 10;
	pr_debug("L2 AGING time: %d sec\n", t);

	t = (msec / 100 + 7) / 8;
	t = t > 0x1FFFFF ? 0x1FFFFF : t;
	sw_w32_mask(0x1FFFFF, t, RTL931X_L2_AGE_CTRL);
	pr_debug("Dynamic aging for ports: %x\n", sw_r32(RTL931X_L2_PORT_AGE_CTRL));

	return 0;
}

int rtldsa_931x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
{
	u32 val;

	sw_w32(0, RTL931X_L2_TBL_FLUSH_CTRL + 4);

	val = 0;
	val |= port << 11;
	val |= BIT(24); /* compare port id */
	val |= BIT(28); /* status - trigger flush */
	if (vid >= 0) {
		sw_w32(vid << 20, RTL931X_L2_TBL_FLUSH_CTRL + 4);
		val |= BIT(26); /* compare VID */
	}
	sw_w32(val, RTL931X_L2_TBL_FLUSH_CTRL);

	do { } while (sw_r32(RTL931X_L2_TBL_FLUSH_CTRL) & BIT(28));

	return 0;
}
