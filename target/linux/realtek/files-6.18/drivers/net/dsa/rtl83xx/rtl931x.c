// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>

#include "pie.h"
#include "qos.h"
#include "rtl-otto.h"
#include "stats.h"
#include "tc.h"
#include "vlan.h"

#define RTL931X_LED_CLK_SEL_MASK				GENMASK(16, 15)
#define RTL931X_LED_CLK_SEL_800NS				0
#define RTL931X_LED_CLK_SEL_400NS				1
#define RTL931X_LED_CLK_SEL_200NS				2
#define RTL931X_LED_CLK_SEL_100NS				3

const struct rtldsa_mib_list_item rtldsa_931x_mib_list[] = {
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_TBL_STD, 36, 1)),
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_TBL_STD, 35, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_TBL_STD, 25, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_TBL_STD, 24, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_TBL_STD, 23, 1)),
	MIB_LIST_ITEM("tx_CRCAlignErrors", MIB_ITEM(MIB_TBL_STD, 22, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_TBL_STD, 20, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_TBL_STD, 18, 1)),
	MIB_LIST_ITEM("tx_Fragments", MIB_ITEM(MIB_TBL_STD, 16, 1)),
	MIB_LIST_ITEM("tx_Jabbers", MIB_ITEM(MIB_TBL_STD, 14, 1)),
	MIB_LIST_ITEM("tx_Collisions", MIB_ITEM(MIB_TBL_STD, 12, 1)),

	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_TBL_PRV, 27, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_TBL_PRV, 22, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_TBL_PRV, 21, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_TBL_PRV, 20, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_TBL_PRV, 19, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_TBL_PRV, 18, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_TBL_PRV, 17, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_TBL_PRV, 16, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_TBL_PRV, 15, 1)),
	MIB_LIST_ITEM("LengthFieldError", MIB_ITEM(MIB_TBL_PRV, 14, 1)),
	MIB_LIST_ITEM("FalseCarrierTimes", MIB_ITEM(MIB_TBL_PRV, 13, 1)),
	MIB_LIST_ITEM("UndersizeOctets", MIB_ITEM(MIB_TBL_PRV, 12, 1)),
	MIB_LIST_ITEM("FramingErrors", MIB_ITEM(MIB_TBL_PRV, 11, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_TBL_PRV, 9, 1)),
	MIB_LIST_ITEM("rx_MacIPGShortDrop", MIB_ITEM(MIB_TBL_PRV, 8, 1))
};

const struct rtldsa_mib_desc rtldsa_931x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_TBL_STD, 29, 1),

	.if_in_octets = MIB_ITEM(MIB_TBL_STD, 51, 2),
	.if_out_octets = MIB_ITEM(MIB_TBL_STD, 49, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_TBL_STD, 47, 2),
	.if_in_mcast_pkts = MIB_ITEM(MIB_TBL_STD, 45, 2),
	.if_in_bcast_pkts = MIB_ITEM(MIB_TBL_STD, 43, 2),
	.if_out_ucast_pkts = MIB_ITEM(MIB_TBL_STD, 41, 2),
	.if_out_mcast_pkts = MIB_ITEM(MIB_TBL_STD, 39, 2),
	.if_out_bcast_pkts = MIB_ITEM(MIB_TBL_STD, 37, 2),
	.if_out_discards = MIB_ITEM(MIB_TBL_STD, 36, 1),
	.single_collisions = MIB_ITEM(MIB_TBL_STD, 34, 1),
	.multiple_collisions = MIB_ITEM(MIB_TBL_STD, 33, 1),
	.deferred_transmissions = MIB_ITEM(MIB_TBL_STD, 32, 1),
	.late_collisions = MIB_ITEM(MIB_TBL_STD, 31, 1),
	.excessive_collisions = MIB_ITEM(MIB_TBL_STD, 30, 1),
	.crc_align_errors = MIB_ITEM(MIB_TBL_STD, 21, 1),
	.rx_pkts_over_max_octets = MIB_ITEM(MIB_TBL_PRV, 23, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_TBL_STD, 28, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_TBL_STD, 19, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_TBL_STD, 17, 1),
	.rx_fragments = MIB_ITEM(MIB_TBL_STD, 15, 1),
	.rx_jabbers = MIB_ITEM(MIB_TBL_STD, 13, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_TBL_STD, 11, 1),
		MIB_ITEM(MIB_TBL_STD, 9, 1),
		MIB_ITEM(MIB_TBL_STD, 7, 1),
		MIB_ITEM(MIB_TBL_STD, 5, 1),
		MIB_ITEM(MIB_TBL_STD, 3, 1),
		MIB_ITEM(MIB_TBL_STD, 1, 1),
		MIB_ITEM(MIB_TBL_PRV, 26, 1),
		MIB_ITEM(MIB_TBL_PRV, 24, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_TBL_STD, 10, 1),
		MIB_ITEM(MIB_TBL_STD, 8, 1),
		MIB_ITEM(MIB_TBL_STD, 6, 1),
		MIB_ITEM(MIB_TBL_STD, 4, 1),
		MIB_ITEM(MIB_TBL_STD, 2, 1),
		MIB_ITEM(MIB_TBL_STD, 0, 1),
		MIB_ITEM(MIB_TBL_PRV, 25, 1),
		MIB_ITEM(MIB_TBL_PRV, 23, 1),
	},
	.rmon_ranges = {
		{ 0, 64 },
		{ 65, 127 },
		{ 128, 255 },
		{ 256, 511 },
		{ 512, 1023 },
		{ 1024, 1518 },
		{ 1519, 12288 },
		{ 12289, 65535 }
	},

	.drop_events = MIB_ITEM(MIB_TBL_STD, 25, 1),
	.collisions = MIB_ITEM(MIB_TBL_STD, 12, 1),

	.rx_pause_frames = MIB_ITEM(MIB_TBL_STD, 27, 1),
	.tx_pause_frames = MIB_ITEM(MIB_TBL_STD, 26, 1),

	.list_count = ARRAY_SIZE(rtldsa_931x_mib_list),
	.list = rtldsa_931x_mib_list
};

static int rtldsa_931x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 3 - ((port + 8) / 16);
	int bit = 2 * ((port + 8) % 16);
	/* port ranges 0..55 (RTL931x covers ports 0 to 55 only), so idx is 0..3 */
	u32 buf[4];
	int state;

	otto_table_read(RTL9310_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

static void rtl931x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL9310_TBL_MSTI);
	int idx = 3 - ((port + 8) / 16);
	int bit = 2 * ((port + 8) % 16);
	/* port ranges 0..55 (RTL931x covers ports 0 to 55 only), so idx is 0..3 */
	u32 buf[4];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

static inline int rtldsa_931x_trk_mbr_ctr(int group)
{
	return RTL931X_TRK_MBR_CTRL + (group << 3);
}

static inline int rtl931x_mac_force_mode_ctrl(int p)
{
	return RTL931X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl931x_mac_port_ctrl(int p)
{
	return RTL931X_MAC_L2_PORT_CTRL + (p << 7);
}

static inline int rtl931x_mac_max_len_reg(int p)
{
	return RTL931X_MAC_L2_PORT_MAX_LEN_CTRL + (p << 2);
}

static inline int rtl931x_l2_port_new_salrn(int p)
{
	return RTL931X_L2_PORT_NEW_SALRN(p);
}

static inline int rtl931x_l2_port_new_sa_fwd(int p)
{
	return RTL931X_L2_PORT_NEW_SA_FWD(p);
}

static int rtldsa_931x_get_mirror_config(struct rtldsa_mirror_config *config,
					 int group, int port)
{
	config->ctrl = RTL931X_MIR_CTRL + group * 4;
	config->spm = RTL931X_MIR_SPM_CTRL + group * 8;
	config->dpm = RTL931X_MIR_DPM_CTRL + group * 8;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 9;

	/* mirror mode: let mirrored packets follow TX settings of
	 * mirroring port
	 */
	config->val |= BIT(5);

	/* direction of traffic to be mirrored when a packet
	 * hits both SPM and DPM ports: prefer egress
	 */
	config->val |= BIT(4);

	return 0;
}

void rtldsa_931x_print_matrix(void)
{
	int tbl = otto_table_acquire(RTL9310_TBL_PORT_ISO_CTRL);
	u32 v[2];

	for (int i = 0; i < 64; i++) {
		__otto_table_read(tbl, i, &v);
		pr_debug("> %08x %08x\n", v[0], v[1]);
	}
	otto_table_release(tbl);
}

static void rtldsa_931x_set_receive_management_action(int port, rma_ctrl_t type,
						      action_type_t action)
{
	u32 shift;
	u32 value;
	u32 reg;

	/* hack for value mapping */
	if (type == GRATARP && action == COPY2CPU)
		action = TRAP2MASTERCPU;

	/* PTP doesn't allow to flood to all ports */
	if (action == FLOODALL &&
	    (type == PTP || type == PTP_UDP || type == PTP_ETH2)) {
		pr_warn("%s: Port flooding not supported for PTP\n", __func__);
		return;
	}

	switch (action) {
	case FORWARD:
		value = 0;
		break;
	case DROP:
		value = 1;
		break;
	case TRAP2CPU:
		value = 2;
		break;
	case TRAP2MASTERCPU:
		value = 3;
		break;
	case FLOODALL:
		value = 4;
		break;
	default:
		return;
	}

	switch (type) {
	case BPDU:
		reg = RTL931X_RMA_BPDU_CTRL + (port / 10) * 4;
		shift = (port % 10) * 3;
		sw_w32_mask(GENMASK(shift + 2, shift), value << shift, reg);
		break;
	case PTP:
		reg = RTL931X_RMA_PTP_CTRL + port * 4;

		/* udp */
		sw_w32_mask(GENMASK(3, 2), value << 2, reg);

		/* eth2 */
		sw_w32_mask(GENMASK(1, 0), value, reg);
		break;
	case PTP_UDP:
		reg = RTL931X_RMA_PTP_CTRL + port * 4;
		sw_w32_mask(GENMASK(3, 2), value << 2, reg);
		break;
	case PTP_ETH2:
		reg = RTL931X_RMA_PTP_CTRL + port * 4;
		sw_w32_mask(GENMASK(1, 0), value, reg);
		break;
	case LLDP:
		reg = RTL931X_RMA_LLDP_CTRL + (port / 10) * 4;
		shift = (port % 10) * 3;
		sw_w32_mask(GENMASK(shift + 2, shift), value << shift, reg);
		break;
	case EAPOL:
		reg = RTL931X_RMA_EAPOL_CTRL + (port / 10) * 4;
		shift = (port % 10) * 3;
		sw_w32_mask(GENMASK(shift + 2, shift), value << shift, reg);
		break;
	case GRATARP:
		reg = RTL931X_TRAP_ARP_GRAT_PORT_ACT + (port / 16) * 4;
		shift = (port % 16) * 2;
		sw_w32_mask(GENMASK(shift + 1, shift), value << shift, reg);
		break;
	}
}

/* Enable traffic between a source port and a destination port matrix */
static void rtl931x_traffic_set(int source, u64 dest_matrix)
{
	u32 buf[2] = { dest_matrix >> (32 - 7), dest_matrix << 7 };

	otto_table_write(RTL9310_TBL_PORT_ISO_CTRL, source, &buf);
}

/* The ternary below only ever selects word 0 or 1, whatever dest is */
static void rtl931x_traffic_enable(int source, int dest)
{
	int tbl = otto_table_acquire(RTL9310_TBL_PORT_ISO_CTRL);
	int idx = (dest + 7) / 32 ? 0 : 1;
	u32 buf[2];

	__otto_table_read(tbl, source, &buf);
	buf[idx] |= BIT((dest + 7) % 32);
	__otto_table_write(tbl, source, &buf);
	otto_table_release(tbl);
}

static void rtl931x_traffic_disable(int source, int dest)
{
	int tbl = otto_table_acquire(RTL9310_TBL_PORT_ISO_CTRL);
	int idx = (dest + 7) / 32 ? 0 : 1;
	u32 buf[2];

	__otto_table_read(tbl, source, &buf);
	buf[idx] &= ~BIT((dest + 7) % 32);
	__otto_table_write(tbl, source, &buf);
	otto_table_release(tbl);
}

static u64 rtldsa_931x_l2_hash_seed(u64 mac, u32 vid)
{
	return (u64)vid << 48 | mac;
}

/* Calculate both the block 0 and the block 1 hash by applyingthe same hash
 * algorithm as the one used currently by the ASIC to the seed, and return
 * both hashes in the lower and higher word of the return value since only 12 bit of
 * the hash are significant.
 */
static u32 rtl931x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
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
static u64 rtl931x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
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

static u64 rtl931x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[4];

	otto_table_read(RTL9310_TBL_L2_CAM_UC, idx, &r);
	rtl931x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	/* return mac with concatenated fid as unique id */
	return ((((u64)(r[0] & 0xffff) << 32) | (u64)r[1]) << 12) | e->vid;
}

static void rtl931x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[4];

	rtl931x_fill_l2_row(r, e);

	otto_table_write(RTL9310_TBL_L2_CAM_UC, idx, &r);
}

static void rtl931x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
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

static void rtl931x_l2_learning_setup(void)
{
	/* Portmask for flooding broadcast traffic */
	rtl839x_set_port_reg_be(RTL931X_MC_PMASK_ALL_PORTS, RTL931X_L2_BC_FLD_PMSK);

	/* Portmask for flooding unicast traffic with unknown destination */
	rtl839x_set_port_reg_be(RTL931X_MC_PMASK_ALL_PORTS, RTL931X_L2_UNKN_UC_FLD_PMSK);

	/* Limit learning to maximum: 64k entries, after that just flood (bits 0-2) */
	sw_w32((0xffff << 3) | FORWARD, RTL931X_L2_LRN_CONSTRT_CTRL);
}

static void rtldsa_931x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 64k entries */
	sw_w32_mask(GENMASK(18, 3), enable ? (0xfffe << 3) : 0,
		    RTL931X_L2_LRN_PORT_CONSTRT_CTRL + port * 4);
}

static void rtldsa_931x_l2_port_new_sa_fwd(int port, enum rtldsa_flood_type mode)
{
	u32 new_sa_fwd_shift = (port % 10) * 3;

	sw_w32_mask(GENMASK(new_sa_fwd_shift + 2, new_sa_fwd_shift),
		    mode << new_sa_fwd_shift,
		    rtl931x_l2_port_new_sa_fwd(port));
}

static void rtldsa_931x_enable_flood(int port, enum rtldsa_flood_type mode)
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

static void rtldsa_931x_enable_bcast_flood(int port, bool enable)
{
	/* RTL931X_L2_BC_FLD_PMSK is big-endian */
	u32 port_offset = ((63 - port) / 32) * 4;
	u32 port_mask = BIT(port % 32);

	sw_w32_mask(port_mask,
		    enable ? port_mask : 0,
		    RTL931X_L2_BC_FLD_PMSK + port_offset);
}

static u64 rtl931x_read_mcast_pmask(int idx)
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

static void rtl931x_write_mcast_pmask(int idx, u64 portmask)
{
	u64 pm = portmask;
	u32 buf[2];

	pr_debug("%s: Index idx %d has portmask %016llx\n", __func__, idx, pm);
	pm <<= 7;
	buf[0] = pm >> 32;
	buf[1] = pm;

	otto_table_write(RTL9310_TBL_MC_PMSK, idx, &buf);
}

static int rtl931x_set_ageing_time(unsigned long msec)
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

static int rtldsa_931x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
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

static void rtl931x_set_igr_filter(int port, enum igr_filter state)
{
	sw_w32_mask(0x3 << ((port & 0xf) << 1), state << ((port & 0xf) << 1),
		    RTL931X_VLAN_PORT_IGR_FLTR + (((port >> 4) << 2)));
}

static void rtl931x_set_egr_filter(int port,  enum egr_filter state)
{
	sw_w32_mask(0x1 << (port % 0x20), state << (port % 0x20),
		    RTL931X_VLAN_PORT_EGR_FLTR + (((port >> 5) << 2)));
}

static void rtldsa_931x_led_get_forced(const struct device_node *node,
				       const u8 leds_in_set[4],
				       u8 forced_leds_per_port[RTL931X_CPU_PORT])
{
	DECLARE_BITMAP(mask, RTL931X_CPU_PORT);
	unsigned int port;
	char set_str[36];
	u64 pm;

	for (u8 set = 0; set < 4; set++) {
		snprintf(set_str, sizeof(set_str), "realtek,led-set%d-force-port-mask", set);
		if (of_property_read_u64(node, set_str, &pm))
			continue;

		bitmap_from_arr64(mask, &pm, RTL931X_CPU_PORT);

		for_each_set_bit(port, mask, RTL931X_CPU_PORT)
			forced_leds_per_port[port] = leds_in_set[set];
	}
}

static void rtldsa_931x_led_init(struct rtl838x_switch_priv *priv)
{
	u8 forced_leds_per_port[RTL931X_CPU_PORT] = {};
	u64 pm_copper = 0, pm_fiber = 0;
	struct device *dev = priv->dev;
	struct device_node *node;
	u8 leds_in_set[4] = {};
	u32 clk_freq;
	int ret;

	node = of_find_compatible_node(NULL, NULL, "realtek,rtl9300-leds");
	if (!node) {
		dev_dbg(dev, "No compatible LED node found\n");
		return;
	}

	ret = of_property_read_u32(node, "clock-frequency", &clk_freq);
	if (!ret) {
		u8 clk_sel;

		switch (clk_freq) {
		case 10000000:
			clk_sel = RTL931X_LED_CLK_SEL_100NS;
			break;
		case 5000000:
			clk_sel = RTL931X_LED_CLK_SEL_200NS;
			break;
		case 1250000:
			clk_sel = RTL931X_LED_CLK_SEL_800NS;
			break;
		default:
			dev_warn(dev, "invalid LED clock frequency, falling back to default\n");
			fallthrough;
		case 2500000:
			clk_sel = RTL931X_LED_CLK_SEL_400NS;
			break;
		}

		sw_w32_mask(RTL931X_LED_CLK_SEL_MASK,
			    FIELD_PREP(RTL931X_LED_CLK_SEL_MASK, clk_sel), RTL931X_LED_GLB_CTRL);
	}

	for (int set = 0; set < 4; set++) {
		char set_name[16] = {0};
		u32 set_config[4];
		int leds_in_this_set = 0;

		/* Reset LED set configuration */
		sw_w32(0, RTL931X_LED_SETX_0_CTRL(set));
		sw_w32(0, RTL931X_LED_SETX_1_CTRL(set));

		/* Each LED set has (up to) 4 LEDs, and each LED is configured
		 * with 16 bits. So each 32 bit register holds configuration for
		 * 2 LEDs. Therefore, each set requires 2 registers for
		 * configuring all 4 LEDs.
		 */
		snprintf(set_name, sizeof(set_name), "led_set%d", set);
		leds_in_this_set = of_property_count_u32_elems(node, set_name);

		if (leds_in_this_set <= 0 || leds_in_this_set > ARRAY_SIZE(set_config)) {
			if (leds_in_this_set != -EINVAL) {
				dev_err(dev, "%s invalid, skipping this set, leds_in_this_set=%d, should be (0, %d]\n",
					set_name, leds_in_this_set, ARRAY_SIZE(set_config));
			}

			continue;
		}

		dev_info(dev, "%s has %d LEDs configured\n", set_name, leds_in_this_set);
		leds_in_set[set] = leds_in_this_set;

		if (of_property_read_u32_array(node, set_name, set_config, leds_in_this_set))
			break;

		/* Write configuration for selected LEDs */
		for (int i = 0, led = leds_in_this_set - 1; led >= 0; led--, i++) {
			sw_w32_mask(0xffff << RTL931X_LED_SET_LEDX_SHIFT(led),
				    (0xffff & set_config[i]) << RTL931X_LED_SET_LEDX_SHIFT(led),
				    RTL931X_LED_SETX_LEDY(set, led));
		}
	}

	rtldsa_931x_led_get_forced(node, leds_in_set, forced_leds_per_port);

	for (int i = 0; i < priv->r->cpu_port; i++) {
		int pos = (i << 1) % 32;
		u32 set;

		sw_w32_mask(0x3 << pos, 0, RTL931X_LED_PORT_FIB_SET_SEL_CTRL(i));
		sw_w32_mask(0x3 << pos, 0, RTL931X_LED_PORT_COPR_SET_SEL_CTRL(i));

		/* Skip port if not present (auto-detect) or not in forced mask */
		if (!priv->ports[i].phy && !priv->ports[i].has_pcs && !(forced_leds_per_port[i]))
			continue;

		if (forced_leds_per_port[i] > 0)
			priv->ports[i].leds_on_this_port = forced_leds_per_port[i];

		/* 0x0 = 1 led, 0x1 = 2 leds, 0x2 = 3 leds, 0x3 = 4 leds per port */
		sw_w32_mask(0x3 << pos, (priv->ports[i].leds_on_this_port - 1) << pos,
			    RTL931X_LED_PORT_NUM_CTRL(i));

		if (priv->ports[i].phy)
			pm_copper |= BIT_ULL(i);
		else
			pm_fiber |= BIT_ULL(i);

		set = priv->ports[i].led_set;
		sw_w32_mask(0, set << pos, RTL931X_LED_PORT_COPR_SET_SEL_CTRL(i));
		sw_w32_mask(0, set << pos, RTL931X_LED_PORT_FIB_SET_SEL_CTRL(i));
	}

	/* Set LED mode to serial (0x1) */
	sw_w32_mask(0x3, 0x1, RTL931X_LED_GLB_CTRL);

	if (of_property_read_bool(node, "active-low"))
		sw_w32_mask(RTL931X_LED_GLB_ACTIVE_LOW, 0, RTL931X_LED_GLB_CTRL);
	else
		sw_w32_mask(0, RTL931X_LED_GLB_ACTIVE_LOW, RTL931X_LED_GLB_CTRL);

	rtl839x_set_port_reg_le(pm_copper, RTL931X_LED_PORT_COPR_MASK_CTRL);
	rtl839x_set_port_reg_le(pm_fiber, RTL931X_LED_PORT_FIB_MASK_CTRL);
	rtl839x_set_port_reg_le(pm_copper | pm_fiber, RTL931X_LED_PORT_COMBO_MASK_CTRL);

	for (int i = 0; i < 32; i++)
		dev_dbg(dev, "%08x: %08x\n", 0xbb000600 + i * 4, sw_r32(0x0600 + i * 4));
}

static void rtldsa_931x_lag_set_port2group(int group, int port, bool valid)
{
	u32 trk_id_valid = valid ? RTL931X_SRC_TRK_MAP_TRK_ID_VALID : 0;
	int tbl = otto_table_acquire(RTL9310_TBL_SRC_TRK_MAP);
	u32 mask = 0;

	mask |= trk_id_valid;
	/* Update TRK Field */
	mask |= FIELD_PREP(RTL931X_SRC_TRK_MAP_TRK_ID, group);

	__otto_table_write(tbl, port, &mask);
	otto_table_release(tbl);
}

/* Write data from the data buffer into the lag-entry strucure */
static void rtldsa_931x_lag_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	e->num_tx_candi = FIELD_GET(RTL931X_LAG_NUM_TX_CANDI, data[0]);
	e->l2_hash_mask_idx = FIELD_GET(RTL931X_LAG_L2_HASH_MSK_IDX, data[0]);
	e->ip4_hash_mask_idx = FIELD_GET(RTL931X_LAG_IP4_HASH_MSK_IDX, data[0]);
	e->ip6_hash_mask_idx = FIELD_GET(RTL931X_LAG_IP6_HASH_MSK_IDX, data[0]);
	e->flood_dlf_bcast.sep_flood_en = FIELD_GET(RTL931X_LAG_SEP_FLOOD_EN, data[0]);
	e->sep_kwn_mc_en = FIELD_GET(RTL931X_LAG_SEP_KWN_MC_EN, data[0]);
	e->trk_dev7 = FIELD_GET(RTL931X_LAG_TRK_DEV7, data[0]);
	e->trk_port7 = FIELD_GET(RTL931X_LAG_TRK_PORT7, data[0]);
	e->trk_dev6 = FIELD_GET(RTL931X_LAG_TRK_DEV6, data[0]);
	e->trk_port6 = FIELD_GET(RTL931X_LAG_TRK_PORT6, data[0]);

	/* 63-32 */
	e->trk_dev5 = FIELD_GET(RTL931X_LAG_TRK_DEV5, data[1]);
	e->trk_port5 = FIELD_GET(RTL931X_LAG_TRK_PORT5, data[1]);
	e->trk_dev4 = FIELD_GET(RTL931X_LAG_TRK_DEV4, data[1]);
	e->trk_port4 = FIELD_GET(RTL931X_LAG_TRK_PORT4, data[1]);
	e->trk_dev3 = FIELD_GET(RTL931X_LAG_TRK_DEV3, data[1]);
	e->trk_port3 = FIELD_GET(RTL931X_LAG_TRK_PORT3, data[1]);

	/* 31-0 */
	e->trk_dev2 = FIELD_GET(RTL931X_LAG_TRK_DEV2, data[2]);
	e->trk_port2 = FIELD_GET(RTL931X_LAG_TRK_PORT2, data[2]);
	e->trk_dev1 = FIELD_GET(RTL931X_LAG_TRK_DEV1, data[2]);
	e->trk_port1 = FIELD_GET(RTL931X_LAG_TRK_PORT1, data[2]);
	e->trk_dev0 = FIELD_GET(RTL931X_LAG_TRK_DEV0, data[2]);
	e->trk_port0 = FIELD_GET(RTL931X_LAG_TRK_PORT0, data[2]);
}

/* Write lag-entry data into buffer */
static void rtldsa_931x_lag_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	data[0] = FIELD_PREP(RTL931X_LAG_NUM_TX_CANDI, e->num_tx_candi);
	data[0] |= FIELD_PREP(RTL931X_LAG_L2_HASH_MSK_IDX, e->l2_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL931X_LAG_IP4_HASH_MSK_IDX, e->ip4_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL931X_LAG_IP6_HASH_MSK_IDX, e->ip6_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL931X_LAG_SEP_FLOOD_EN, e->flood_dlf_bcast.sep_flood_en);
	data[0] |= FIELD_PREP(RTL931X_LAG_SEP_KWN_MC_EN, e->sep_kwn_mc_en);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_DEV7, e->trk_dev7);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_PORT7, e->trk_port7);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_DEV6, e->trk_dev6);
	data[0] |= FIELD_PREP(RTL931X_LAG_TRK_PORT6, e->trk_port6);

	/* 63-32 */
	data[1] = FIELD_PREP(RTL931X_LAG_TRK_DEV5, e->trk_dev5);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_PORT5, e->trk_port5);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_DEV4, e->trk_dev4);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_PORT4, e->trk_port4);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_DEV3, e->trk_dev3);
	data[1] |= FIELD_PREP(RTL931X_LAG_TRK_PORT3, e->trk_port3);

	/* 31-0 */
	data[2] = FIELD_PREP(RTL931X_LAG_TRK_DEV2, e->trk_dev2);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_PORT2, e->trk_port2);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_DEV1, e->trk_dev1);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_PORT1, e->trk_port1);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_DEV0, e->trk_dev0);
	data[2] |= FIELD_PREP(RTL931X_LAG_TRK_PORT0, e->trk_port0);
}

static void rtldsa_931x_lag_set_local_group_id(int local_group, int global_group, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL931X_TRK_ID_CTRL_TRK_VALID : 0;
	mask |= FIELD_PREP(RLT931X_TRK_ID_CTRL_TRK_ID, global_group);
	sw_w32(mask, RTL931X_TRK_ID_CTRL + (4 * local_group));
}

static void rtldsa_931x_lag_set_local_port2group(int group, int port, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL931X_LOCAL_PORT_TRK_MAP_IS_TRK_MBR : 0;
	mask |= FIELD_PREP(RTL931X_LOCAL_PORT_TRK_MAP_TRK_ID, group);
	sw_w32(mask, RTL931X_LOCAL_PORT_TRK_MAP + (4 * port));
}

static void rtldsa_931x_lag_sync_tables(void)
{
	u32 val;
	int ret;

	sw_w32(BIT(0), RTL931X_TRK_LOCAL_TBL_REFRESH);

	ret = readx_poll_timeout(sw_r32, RTL931X_TRK_LOCAL_TBL_REFRESH, val,
				 !(val & BIT(0)), 20, 10000);
	if (ret)
		pr_err("%s: timeout\n", __func__);
}

static int rtldsa_931x_lag_table(void)
{
	return otto_table_acquire(RTL9310_TBL_LAG);
}

static u64 rtldsa_931x_stat_port_table_read(int port, unsigned int mib_size,
					    unsigned int mib_offset, bool is_pvt)
{
	enum otto_table_id id;
	int field_offset;
	u32 val[2];

	if (is_pvt) {
		id = RTL9310_TBL_STAT_PORT_PRVTE_CNTR;
		field_offset = 27;
	} else {
		id = RTL9310_TBL_STAT_PORT_MIB_CNTR;
		field_offset = 52;
	}

	/* Counter fields are numbered down from the last data word of the
	 * entry, so the high half of a 64 bit counter sits at the lower word
	 * index.
	 */
	if (mib_size == 2) {
		otto_table_read_bytes(id, port, val,
				      field_offset - (mib_offset + 1),
				      sizeof(val));

		return (u64)val[0] << 32 | val[1];
	}

	otto_table_read_bytes(id, port, val, field_offset - mib_offset,
			      sizeof(val[0]));

	return val[0];
}

const struct rtldsa_config rtldsa_931x_cfg = {
	.switch_ops = &rtldsa_93xx_switch_ops,
	.phylink_mac_ops = &rtldsa_93xx_phylink_mac_ops,
	.spanning_tree_ctrl = RTL931X_ST_CTRL,
	.l2_bucket_size = 8,
	.n_mst = 128,
	.num_lag_ids = 16,
	.cpu_port = RTL931X_CPU_PORT,
	.fib_entries = 16384, /* TODO: has 32K but code cannot handle that */
	.mask_port_reg_be = rtl839x_mask_port_reg_be,
	.set_port_reg_be = rtl839x_set_port_reg_be,
	.get_port_reg_be = rtl839x_get_port_reg_be,
	.mask_port_reg_le = rtl839x_mask_port_reg_le,
	.set_port_reg_le = rtl839x_set_port_reg_le,
	.get_port_reg_le = rtl839x_get_port_reg_le,
	.stat_port_rst = RTL931X_STAT_PORT_RST,
	.stat_rst = RTL931X_STAT_RST,
	.stat_port_std_mib = 0,  /* Not defined */
	.mib_desc = &rtldsa_931x_mib_desc,
	.stat_port_table_read = rtldsa_931x_stat_port_table_read,
	.stat_counters_lock = rtldsa_counters_lock_table,
	.stat_counters_unlock = rtldsa_counters_unlock_table,
	.stat_counter_poll_interval = RTLDSA_COUNTERS_FAST_POLL_INTERVAL,
	.traffic_enable = rtl931x_traffic_enable,
	.traffic_disable = rtl931x_traffic_disable,
	.traffic_set = rtl931x_traffic_set,
	.l2_ctrl_0 = RTL931X_L2_CTRL,
	.l2_ctrl_1 = RTL931X_L2_AGE_CTRL,
	.l2_port_aging_out = RTL931X_L2_PORT_AGE_CTRL,
	.set_ageing_time = rtl931x_set_ageing_time,
	.l2_tbl_flush_ctrl = RTL931X_L2_TBL_FLUSH_CTRL,
	.isr_glb_src = RTL931X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL931X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL931X_IMR_PORT_LINK_STS_CHG,
	/* imr_glb does not exist on RTL931X */
	.n_counters = 2048,
	.n_pie_blocks = 16,
	.port_ignore = 0x3f,
	.vlan_tables_read = rtl931x_vlan_tables_read,
	.vlan_set_tagged = rtl931x_vlan_set_tagged,
	.vlan_set_untagged = rtl931x_vlan_set_untagged,
	.vlan_profile_get = rtldsa_931x_vlan_profile_get,
	.vlan_profile_dump = rtldsa_931x_vlan_profile_dump,
	.vlan_profile_setup = rtl931x_vlan_profile_setup,
	.vlan_fwd_on_inner = rtl931x_vlan_fwd_on_inner,
	.stp_get = rtldsa_931x_stp_get,
	.stp_set = rtl931x_stp_set,
	.mac_force_mode_mask = RTL931X_FORCE_EN | RTL931X_FORCE_LINK_EN,
	.mac_force_mode_ctrl = rtl931x_mac_force_mode_ctrl,
	.mac_link_sts = RTL931X_MAC_LINK_STS,
	.mac_port_ctrl = rtl931x_mac_port_ctrl,
	.mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 | MAC_100 |
			    MAC_1000FD | MAC_2500FD | MAC_5000FD | MAC_10000FD,
	.mac_max_len_reg = rtl931x_mac_max_len_reg,
	.max_frame = RTL931X_MAX_FRAME,
	.l2_port_new_salrn = rtl931x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl931x_l2_port_new_sa_fwd,
	.get_mirror_config = rtldsa_931x_get_mirror_config,
	.port_rate_police_add = rtldsa_931x_port_rate_police_add,
	.port_rate_police_del = rtldsa_931x_port_rate_police_del,
	.print_matrix = rtldsa_931x_print_matrix,
	.read_l2_entry_using_hash = rtl931x_read_l2_entry_using_hash,
	.write_l2_entry_using_hash = rtl931x_write_l2_entry_using_hash,
	.read_cam = rtl931x_read_cam,
	.write_cam = rtl931x_write_cam,
	.vlan_port_keep_tag_set = rtl931x_vlan_port_keep_tag_set,
	.vlan_port_pvidmode_set = rtl931x_vlan_port_pvidmode_set,
	.vlan_port_pvid_set = rtl931x_vlan_port_pvid_set,
	.fast_age = rtldsa_931x_fast_age,
	.trk_mbr_ctr = rtldsa_931x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL931X_RMA_BPDU_FLD_PMSK,
	.set_vlan_igr_filter = rtl931x_set_igr_filter,
	.set_vlan_egr_filter = rtl931x_set_egr_filter,
	.l2_hash_key = rtl931x_l2_hash_key,
	.l2_hash_seed = rtldsa_931x_l2_hash_seed,
	.read_mcast_pmask = rtl931x_read_mcast_pmask,
	.write_mcast_pmask = rtl931x_write_mcast_pmask,
	.pie_init = rtl931x_pie_init,
	.pie_rule_write = rtl931x_pie_rule_write,
	.pie_rule_add = rtl931x_pie_rule_add,
	.pie_rule_rm = rtl931x_pie_rule_rm,
	.l2_learning_setup = rtl931x_l2_learning_setup,
	.led_init = rtldsa_931x_led_init,
	.enable_learning = rtldsa_931x_enable_learning,
	.enable_l2_new_sa_fwd = rtldsa_931x_l2_port_new_sa_fwd,
	.enable_flood = rtldsa_931x_enable_flood,
	.enable_bcast_flood = rtldsa_931x_enable_bcast_flood,
	.set_receive_management_action = rtldsa_931x_set_receive_management_action,
	.qos_init = rtldsa_931x_qos_init,
	.trk_ctrl = RTL931X_TRK_CTRL,
	.trk_hash_ctrl = RTL931X_TRK_HASH_CTRL,
	.prepare_lag_fdb = rtldsa_93xx_prepare_lag_fdb,
	.lag_switch_init = rtldsa_93xx_lag_switch_init,
	.lag_set_port_members = rtldsa_93xx_lag_set_port_members,
	.lag_set_distribution_algorithm = rtldsa_93xx_lag_set_distribution_algorithm,
	.lag_set_local_group_id = rtldsa_931x_lag_set_local_group_id,
	.lag_write_data = rtldsa_931x_lag_write_data,
	.lag_fill_data = rtldsa_931x_lag_fill_data,
	.lag_set_local_port2group = rtldsa_931x_lag_set_local_port2group,
	.lag_set_port2group = rtldsa_931x_lag_set_port2group,
	.lag_sync_tables = rtldsa_931x_lag_sync_tables,
	.lag_table = rtldsa_931x_lag_table,
};
