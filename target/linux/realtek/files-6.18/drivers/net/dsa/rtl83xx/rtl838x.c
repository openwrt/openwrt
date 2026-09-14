// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>
#include <linux/iopoll.h>
#include <net/nexthop.h>

#include "l3.h"
#include "pie.h"
#include "qos.h"
#include "rtl-otto.h"
#include "vlan.h"

const struct rtldsa_mib_list_item rtldsa_838x_mib_list[] = {
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_REG_STD, 0xec, 1)),
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_REG_STD, 0xd0, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_REG_STD, 0xa8, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_REG_STD, 0xa4, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_REG_STD, 0xa0, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_REG_STD, 0x98, 1)),
	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_REG_STD, 0x90, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_REG_STD, 0x8c, 1)),
	MIB_LIST_ITEM("Collisions", MIB_ITEM(MIB_REG_STD, 0x7c, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_REG_STD, 0x40, 1))
};

const struct rtldsa_mib_desc rtldsa_838x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_REG_STD, 0xb8, 1),

	.if_in_octets = MIB_ITEM(MIB_REG_STD, 0xf8, 2),
	.if_out_octets = MIB_ITEM(MIB_REG_STD, 0xf0, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xe8, 1),
	.if_in_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe4, 1),
	.if_in_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe0, 1),
	.if_out_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xdc, 1),
	.if_out_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd8, 1),
	.if_out_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd4, 1),
	.if_out_discards = MIB_ITEM(MIB_REG_STD, 0xd0, 1),
	.single_collisions = MIB_ITEM(MIB_REG_STD, 0xcc, 1),
	.multiple_collisions = MIB_ITEM(MIB_REG_STD, 0xc8, 1),
	.deferred_transmissions = MIB_ITEM(MIB_REG_STD, 0xc4, 1),
	.late_collisions = MIB_ITEM(MIB_REG_STD, 0xc0, 1),
	.excessive_collisions = MIB_ITEM(MIB_REG_STD, 0xbc, 1),
	.crc_align_errors = MIB_ITEM(MIB_REG_STD, 0x9c, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_REG_STD, 0xb4, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_REG_STD, 0x94, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_REG_STD, 0x88, 1),
	.rx_fragments = MIB_ITEM(MIB_REG_STD, 0x84, 1),
	.rx_jabbers = MIB_ITEM(MIB_REG_STD, 0x80, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x78, 1),
		MIB_ITEM(MIB_REG_STD, 0x70, 1),
		MIB_ITEM(MIB_REG_STD, 0x68, 1),
		MIB_ITEM(MIB_REG_STD, 0x60, 1),
		MIB_ITEM(MIB_REG_STD, 0x58, 1),
		MIB_ITEM(MIB_REG_STD, 0x50, 1),
		MIB_ITEM(MIB_REG_STD, 0x48, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x74, 1),
		MIB_ITEM(MIB_REG_STD, 0x6c, 1),
		MIB_ITEM(MIB_REG_STD, 0x64, 1),
		MIB_ITEM(MIB_REG_STD, 0x5c, 1),
		MIB_ITEM(MIB_REG_STD, 0x54, 1),
		MIB_ITEM(MIB_REG_STD, 0x4c, 1),
		MIB_ITEM(MIB_REG_STD, 0x44, 1)
	},
	.rmon_ranges = {
		{ 0, 64 },
		{ 65, 127 },
		{ 128, 255 },
		{ 256, 511 },
		{ 512, 1023 },
		{ 1024, 1518 },
		{ 1519, 10000 }
	},

	.drop_events = MIB_ITEM(MIB_REG_STD, 0xa8, 1),
	.collisions = MIB_ITEM(MIB_REG_STD, 0x7c, 1),

	.rx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xb0, 1),
	.tx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xac, 1),

	.list_count = ARRAY_SIZE(rtldsa_838x_mib_list),
	.list = rtldsa_838x_mib_list
};

void rtldsa_838x_print_matrix(void)
{
	unsigned volatile int *ptr8;

	ptr8 = RTL838X_SW_BASE + RTL838X_PORT_ISO_CTRL(0);
	for (int i = 0; i < 28; i += 8)
		pr_debug("> %8x %8x %8x %8x %8x %8x %8x %8x\n",
			 ptr8[i + 0], ptr8[i + 1], ptr8[i + 2], ptr8[i + 3],
			 ptr8[i + 4], ptr8[i + 5], ptr8[i + 6], ptr8[i + 7]);
	pr_debug("CPU_PORT> %8x\n", ptr8[28]);
}

static inline int rtl838x_port_iso_ctrl(int p)
{
	return RTL838X_PORT_ISO_CTRL(p);
}

static u64 rtl838x_l2_hash_seed(u64 mac, u32 vid)
{
	return mac << 12 | vid;
}

/* Applies the same hash algorithm as the one used currently by the ASIC to the seed
 * and returns a key into the L2 hash table
 */
static u32 rtl838x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
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

static inline int rtl838x_mac_force_mode_ctrl(int p)
{
	return RTL838X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl838x_mac_port_ctrl(int p)
{
	return RTL838X_MAC_PORT_CTRL(p);
}

static inline int rtl838x_l2_port_new_salrn(int p)
{
	return RTL838X_L2_PORT_NEW_SALRN(p);
}

static inline int rtl838x_l2_port_new_sa_fwd(int p)
{
	return RTL838X_L2_PORT_NEW_SA_FWD(p);
}

static int rtldsa_838x_get_mirror_config(struct rtldsa_mirror_config *config,
					 int group, int port)
{
	config->ctrl = RTL838X_MIR_CTRL + group * 4;
	config->spm = RTL838X_MIR_SPM_CTRL + group * 4;
	config->dpm = RTL838X_MIR_DPM_CTRL + group * 4;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 4;

	/* Enable mirroring to port across VLANs */
	config->val |= BIT(11);

	return 0;
}

static inline int rtl838x_trk_mbr_ctr(int group)
{
	return RTL838X_TRK_MBR_CTR + (group << 2);
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
static u64 rtl838x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
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

static void rtl838x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Access SRAM, with hash and at pos in bucket */

	rtl838x_fill_l2_row(r, e);

	/* Access L2 Table 0 */
	otto_table_write(RTL8380_TBL_L2_UC, idx, &r);
}

static u64 rtl838x_read_cam(int idx, struct rtl838x_l2_entry *e)
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

static void rtl838x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	rtl838x_fill_l2_row(r, e);

	/* Access L2 Table 1 */
	otto_table_write(RTL8380_TBL_L2_CAM_UC, idx, &r);
}

static u64 rtl838x_read_mcast_pmask(int idx)
{
	u32 portmask;

	otto_table_read(RTL8380_TBL_MC_PMSK, idx, &portmask);

	return portmask;
}

static void rtl838x_write_mcast_pmask(int idx, u64 portmask)
{
	u32 buf[1] = { ((u32)portmask) & RTL838X_MC_PMASK_ALL_PORTS };

	otto_table_write(RTL8380_TBL_MC_PMSK, idx, &buf);
}

static void rtl838x_l2_learning_setup(void)
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

static void rtl838x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 16k entries */

	sw_w32_mask(0x3fff << 2, enable ? (0x3fff << 2) : 0,
		    RTL838X_L2_PORT_LRN_CONSTRT + (port << 2));
}

static void rtl838x_enable_flood(int port, enum rtldsa_flood_type mode)
{
	/* 0: Forward
	 * 1: Disable
	 * 2: to CPU
	 * 3: Copy to CPU
	 */
	sw_w32_mask(0x3, mode,
		    RTL838X_L2_PORT_LRN_CONSTRT + (port << 2));
}

static void rtl838x_enable_mcast_flood(int port, bool enable)
{
}

static void rtl838x_enable_bcast_flood(int port, bool enable)
{
}

static void rtl838x_set_static_move_action(int port, bool forward)
{
	int shift = MV_ACT_PORT_SHIFT(port);
	u32 val = forward ? MV_ACT_FORWARD : MV_ACT_DROP;

	sw_w32_mask(MV_ACT_MASK << shift, val << shift,
		    RTL838X_L2_PORT_STATIC_MV_ACT(port));
}

static int rtldsa_838x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 1 - (port / 16);
	int bit = 2 * (port % 16);
	/* port < priv->r->cpu_port (RTL838X_CPU_PORT == 28), so idx is 0 or 1 */
	u32 buf[2];
	int state;

	otto_table_read(RTL8380_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

static void rtl838x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL8380_TBL_MSTI);
	int idx = 1 - (port / 16);
	int bit = 2 * (port % 16);
	/* port < priv->r->cpu_port (RTL838X_CPU_PORT == 28), so idx is 0 or 1 */
	u32 buf[2];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

static void rtl838x_traffic_set(int source, u64 dest_matrix)
{
	rtl838x_set_port_reg(dest_matrix, rtl838x_port_iso_ctrl(source));
}

static void rtl838x_traffic_enable(int source, int dest)
{
	rtl838x_mask_port_reg(0, BIT(dest), rtl838x_port_iso_ctrl(source));
}

static void rtl838x_traffic_disable(int source, int dest)
{
	rtl838x_mask_port_reg(BIT(dest), 0, rtl838x_port_iso_ctrl(source));
}

/* Enables or disables the EEE/EEEP capability of a port */
static void rtldsa_838x_set_mac_eee(struct rtl838x_switch_priv *priv, int port, bool enable)
{
	u32 v;

	/* This works only for Ethernet ports, and on the RTL838X, ports from 24 are SFP */
	if (port >= 24)
		return;

	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0x3 : 0x0;

	/* Set EEE state for 100 (bit 9) & 1000MBit (bit 10) */
	sw_w32_mask(0x3 << 9, v << 9, priv->r->mac_force_mode_ctrl(port));

	/* Set TX/RX EEE state */
	if (enable) {
		sw_w32_mask(0, BIT(port), RTL838X_EEE_PORT_TX_EN);
		sw_w32_mask(0, BIT(port), RTL838X_EEE_PORT_RX_EN);
	} else {
		sw_w32_mask(BIT(port), 0, RTL838X_EEE_PORT_TX_EN);
		sw_w32_mask(BIT(port), 0, RTL838X_EEE_PORT_RX_EN);
	}
	priv->ports[port].eee_enabled = enable;
}

static void rtl838x_init_eee(struct rtl838x_switch_priv *priv, bool enable)
{
	pr_debug("Setting up EEE, state: %d\n", enable);
	sw_w32_mask(0x4, 0, RTL838X_SMI_GLB_CTRL);

	/* Set timers for EEE */
	sw_w32(0x5001411, RTL838X_EEE_TX_TIMER_GIGA_CTRL);
	sw_w32(0x5001417, RTL838X_EEE_TX_TIMER_GELITE_CTRL);

	/* Enable EEE MAC support on ports */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy)
			priv->r->set_mac_eee(priv, i, enable);
	}
	priv->eee_enabled = enable;
}

static u32 rtl838x_packet_cntr_read(struct rtl838x_switch_priv *priv, int counter)
{
	u32 buf[2];
	u32 v;

	dev_dbg(priv->dev, "reading LOG packet counter %d\n", counter);
	otto_table_read(RTL8380_TBL_LOG, counter / 2, &buf);

	dev_dbg(priv->dev, "LOG entry: %08x %08x\n", buf[0], buf[1]);
	if (counter % 2)
		v = buf[0];
	else
		v = buf[1];

	return v;
}

static void rtl838x_packet_cntr_clear(struct rtl838x_switch_priv *priv, int counter)
{
	int tbl = otto_table_acquire(RTL8380_TBL_LOG);
	u32 buf[2];

	dev_dbg(priv->dev, "clearing LOG packet counter %d\n", counter);

	/*
	 * Two counters share one LOG table entry. Read the current entry
	 * first so clearing one half preserves the adjacent counter.
	 */
	__otto_table_read(tbl, counter / 2, &buf);

	if (counter % 2)
		buf[0] = 0;
	else
		buf[1] = 0;

	__otto_table_write(tbl, counter / 2, &buf);

	otto_table_release(tbl);
}

static int rtldsa_838x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
{
	u32 val;

	val = BIT(26) | BIT(23) | (port << 5);
	if (vid >= 0)
		val |= BIT(24) | (vid << 10);

	sw_w32(val, priv->r->l2_tbl_flush_ctrl);
	do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & BIT(26));

	return 0;
}

static int rtl838x_set_ageing_time(unsigned long msec)
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

static void rtl838x_set_igr_filter(int port, enum igr_filter state)
{
	sw_w32_mask(0x3 << ((port & 0xf) << 1), state << ((port & 0xf) << 1),
		    RTL838X_VLAN_PORT_IGR_FLTR + (((port >> 4) << 2)));
}

static void rtl838x_set_egr_filter(int port, enum egr_filter state)
{
	sw_w32_mask(0x1 << (port % 0x1d), state << (port % 0x1d),
		    RTL838X_VLAN_PORT_EGR_FLTR + (((port / 29) << 2)));
}

static int rtldsa_838x_set_distribution_algorithm(struct rtl838x_switch_priv *priv,
						  int group, int algoidx, u32 algomsk)
{
	algoidx &= 1; /* RTL838X only supports 2 concurrent algorithms */
	sw_w32_mask(1 << (group % 8), algoidx << (group % 8),
		    RTL838X_TRK_HASH_IDX_CTRL + ((group >> 3) << 2));
	sw_w32(algomsk, RTL838X_TRK_HASH_CTRL + (algoidx << 2));
	return 0;
}

static void rtl838x_set_receive_management_action(int port, rma_ctrl_t type, action_type_t action)
{
	switch (type) {
	case BPDU:
		sw_w32_mask(3 << ((port & 0xf) << 1), (action & 0x3) << ((port & 0xf) << 1),
			    RTL838X_RMA_BPDU_CTRL + ((port >> 4) << 2));
		break;
	case PTP:
		sw_w32_mask(3 << ((port & 0xf) << 1), (action & 0x3) << ((port & 0xf) << 1),
			    RTL838X_RMA_PTP_CTRL + ((port >> 4) << 2));
		break;
	case LLDP:
		sw_w32_mask(3 << ((port & 0xf) << 1), (action & 0x3) << ((port & 0xf) << 1),
			    RTL838X_RMA_LLDP_CTRL + ((port >> 4) << 2));
		break;
	default:
		break;
	}
}

static int rtldsa_838x_lag_set_port_members(struct rtl838x_switch_priv *priv, int group,
					    u64 members, struct netdev_lag_upper_info *info)
{
	priv->lags_port_members[group] = members;

	priv->r->set_port_reg_be(priv->lags_port_members[group],
				 priv->r->trk_mbr_ctr(group));

	return 0;
}

int rtldsa_83xx_lag_setup_algomask(struct rtl838x_switch_priv *priv, int group,
				   struct netdev_lag_upper_info *info);

static void rtldsa_838x_stat_init(struct rtl838x_switch_priv *priv)
{
	/* Enable statistics module: all counters plus debug */
	sw_w32_mask(0, 3, RTL838X_STAT_CTRL);
}

const struct rtldsa_config rtldsa_838x_cfg = {
	.switch_ops = &rtldsa_83xx_switch_ops,
	.phylink_mac_ops = &rtldsa_83xx_phylink_mac_ops,
	.spanning_tree_ctrl = RTL838X_VLAN_STP_CTRL,
	.l2_bucket_size = 4,
	.n_mst = 64,
	.num_lag_ids = 8,
	.cpu_port = RTL838X_CPU_PORT,
	.fib_entries = 8192,
	.mask_port_reg_be = rtl838x_mask_port_reg,
	.set_port_reg_be = rtl838x_set_port_reg,
	.get_port_reg_be = rtl838x_get_port_reg,
	.mask_port_reg_le = rtl838x_mask_port_reg,
	.set_port_reg_le = rtl838x_set_port_reg,
	.get_port_reg_le = rtl838x_get_port_reg,
	.stat_port_rst = RTL838X_STAT_PORT_RST,
	.stat_rst = RTL838X_STAT_RST,
	.stat_init = rtldsa_838x_stat_init,
	.stat_port_std_mib = RTL838X_STAT_PORT_STD_MIB,
	.mib_desc = &rtldsa_838x_mib_desc,
	.stat_counters_lock = rtldsa_counters_lock_register,
	.stat_counters_unlock = rtldsa_counters_unlock_register,
	.stat_update_counters_atomically = rtldsa_update_counters_atomically,
	.stat_counter_poll_interval = RTLDSA_COUNTERS_POLL_INTERVAL,
	.port_iso_ctrl = rtl838x_port_iso_ctrl,
	.traffic_enable = rtl838x_traffic_enable,
	.traffic_disable = rtl838x_traffic_disable,
	.traffic_set = rtl838x_traffic_set,
	.l2_ctrl_0 = RTL838X_L2_CTRL_0,
	.l2_ctrl_1 = RTL838X_L2_CTRL_1,
	.high_res_l2_age = true,
	.self_mac_trap_ctrl = RTL838X_SPCL_TRAP_SWITCH_MAC_CTRL,
	.l2_port_aging_out = RTL838X_L2_PORT_AGING_OUT,
	.set_ageing_time = rtl838x_set_ageing_time,
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
	.isr_glb_src = RTL838X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL838X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL838X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL838X_IMR_GLB,
	.n_counters = 128,
	.n_pie_blocks = 12,
	.port_ignore = 0x1f,
	.vlan_tables_read = rtl838x_vlan_tables_read,
	.vlan_set_tagged = rtl838x_vlan_set_tagged,
	.vlan_set_untagged = rtl838x_vlan_set_untagged,
	.mac_force_mode_mask = RTL83XX_FORCE_EN | RTL83XX_FORCE_LINK_EN,
	.mac_force_mode_ctrl = rtl838x_mac_force_mode_ctrl,
	.mac_link_sts = RTL838X_MAC_LINK_STS,
	.vlan_profile_get = rtldsa_838x_vlan_profile_get,
	.vlan_profile_dump = rtldsa_838x_vlan_profile_dump,
	.vlan_profile_setup = rtl838x_vlan_profile_setup,
	.vlan_fwd_on_inner = rtl838x_vlan_fwd_on_inner,
	.set_vlan_igr_filter = rtl838x_set_igr_filter,
	.set_vlan_egr_filter = rtl838x_set_egr_filter,
	.enable_learning = rtl838x_enable_learning,
	.enable_flood = rtl838x_enable_flood,
	.enable_mcast_flood = rtl838x_enable_mcast_flood,
	.enable_bcast_flood = rtl838x_enable_bcast_flood,
	.set_static_move_action = rtl838x_set_static_move_action,
	.stp_get = rtldsa_838x_stp_get,
	.stp_set = rtl838x_stp_set,
	.mac_port_ctrl = rtl838x_mac_port_ctrl,
	.mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 | MAC_100 | MAC_1000FD,
	.mac_max_len_ctrl = RTL838X_MAC_MAX_LEN_CTRL,
	.mac_max_len_ctrl_dup = RTL838X_MAC_MAX_LEN_CTRL_DUP,
	.max_frame = RTL838X_MAX_FRAME,
	.l2_port_new_salrn = rtl838x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl838x_l2_port_new_sa_fwd,
	.get_mirror_config = rtldsa_838x_get_mirror_config,
	.print_matrix = rtldsa_838x_print_matrix,
	.read_l2_entry_using_hash = rtl838x_read_l2_entry_using_hash,
	.write_l2_entry_using_hash = rtl838x_write_l2_entry_using_hash,
	.read_cam = rtl838x_read_cam,
	.write_cam = rtl838x_write_cam,
	.vlan_port_keep_tag_set = rtl838x_vlan_port_keep_tag_set,
	.vlan_port_pvidmode_set = rtl838x_vlan_port_pvidmode_set,
	.vlan_port_pvid_set = rtl838x_vlan_port_pvid_set,
	.fast_age = rtldsa_838x_fast_age,
	.trk_mbr_ctr = rtl838x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL838X_RMA_BPDU_FLD_PMSK,
	.spcl_trap_eapol_ctrl = RTL838X_SPCL_TRAP_EAPOL_CTRL,
	.init_eee = rtl838x_init_eee,
	.set_mac_eee = rtldsa_838x_set_mac_eee,
	.l2_hash_seed = rtl838x_l2_hash_seed,
	.l2_hash_key = rtl838x_l2_hash_key,
	.read_mcast_pmask = rtl838x_read_mcast_pmask,
	.write_mcast_pmask = rtl838x_write_mcast_pmask,
	.pie_init = rtl838x_pie_init,
	.pie_rule_read = rtl838x_pie_rule_read,
	.pie_rule_write = rtl838x_pie_rule_write,
	.pie_rule_add = rtl838x_pie_rule_add,
	.pie_rule_rm = rtl838x_pie_rule_rm,
	.l2_learning_setup = rtl838x_l2_learning_setup,
	.packet_cntr_read = rtl838x_packet_cntr_read,
	.packet_cntr_clear = rtl838x_packet_cntr_clear,
	.set_receive_management_action = rtl838x_set_receive_management_action,
	.get_egress_rate = rtldsa_838x_get_egress_rate,
	.set_egress_rate = rtldsa_838x_set_egress_rate,
	.qos_init = rtldsa_838x_qos_init,
	.lag_set_distribution_algorithm = rtldsa_838x_set_distribution_algorithm,
	.lag_set_port_members = rtldsa_838x_lag_set_port_members,
	.lag_setup_algomask = rtldsa_83xx_lag_setup_algomask,
};
