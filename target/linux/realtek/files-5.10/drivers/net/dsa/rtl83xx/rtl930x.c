// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl83xx.h"

extern struct mutex smi_lock;
extern struct rtl83xx_soc_info soc_info;

/* Definition of the RTL930X-specific template field IDs as used in the PIE */
enum template_field_id {
	TEMPLATE_FIELD_SPM0 = 0,		// Source portmask ports 0-15
	TEMPLATE_FIELD_SPM1 = 1,		// Source portmask ports 16-31
	TEMPLATE_FIELD_DMAC0 = 2,		// Destination MAC [15:0]
	TEMPLATE_FIELD_DMAC1 = 3,		// Destination MAC [31:16]
	TEMPLATE_FIELD_DMAC2 = 4,		// Destination MAC [47:32]
	TEMPLATE_FIELD_SMAC0 = 5,		// Source MAC [15:0]
	TEMPLATE_FIELD_SMAC1 = 6,		// Source MAC [31:16]
	TEMPLATE_FIELD_SMAC2 = 7,		// Source MAC [47:32]
	TEMPLATE_FIELD_ETHERTYPE = 8,		// Ethernet frame type field
	TEMPLATE_FIELD_OTAG = 9,
	TEMPLATE_FIELD_ITAG = 10,
	TEMPLATE_FIELD_SIP0 = 11,
	TEMPLATE_FIELD_SIP1 = 12,
	TEMPLATE_FIELD_DIP0 = 13,
	TEMPLATE_FIELD_DIP1 = 14,
	TEMPLATE_FIELD_IP_TOS_PROTO = 15,
	TEMPLATE_FIELD_L4_SPORT = 16,
	TEMPLATE_FIELD_L4_DPORT = 17,
	TEMPLATE_FIELD_L34_HEADER = 18,
	TEMPLATE_FIELD_TCP_INFO = 19,
	TEMPLATE_FIELD_FIELD_SELECTOR_VALID = 20,
	TEMPLATE_FIELD_FIELD_SELECTOR_0 = 21,
	TEMPLATE_FIELD_FIELD_SELECTOR_1 = 22,
	TEMPLATE_FIELD_FIELD_SELECTOR_2 = 23,
	TEMPLATE_FIELD_FIELD_SELECTOR_3 = 24,
	TEMPLATE_FIELD_FIELD_SELECTOR_4 = 25,
	TEMPLATE_FIELD_FIELD_SELECTOR_5 = 26,
	TEMPLATE_FIELD_SIP2 = 27,
	TEMPLATE_FIELD_SIP3 = 28,
	TEMPLATE_FIELD_SIP4 = 29,
	TEMPLATE_FIELD_SIP5 = 30,
	TEMPLATE_FIELD_SIP6 = 31,
	TEMPLATE_FIELD_SIP7 = 32,
	TEMPLATE_FIELD_DIP2 = 33,
	TEMPLATE_FIELD_DIP3 = 34,
	TEMPLATE_FIELD_DIP4 = 35,
	TEMPLATE_FIELD_DIP5 = 36,
	TEMPLATE_FIELD_DIP6 = 37,
	TEMPLATE_FIELD_DIP7 = 38,
	TEMPLATE_FIELD_PKT_INFO = 39,
	TEMPLATE_FIELD_FLOW_LABEL = 40,
	TEMPLATE_FIELD_DSAP_SSAP = 41,
	TEMPLATE_FIELD_SNAP_OUI = 42,
	TEMPLATE_FIELD_FWD_VID = 43,
	TEMPLATE_FIELD_RANGE_CHK = 44,
	TEMPLATE_FIELD_VLAN_GMSK = 45,		// VLAN Group Mask/IP range check
	TEMPLATE_FIELD_DLP = 46,
	TEMPLATE_FIELD_META_DATA = 47,
	TEMPLATE_FIELD_SRC_FWD_VID = 48,
	TEMPLATE_FIELD_SLP = 49,
};

/* The meaning of TEMPLATE_FIELD_VLAN depends on phase and the configuration in
 * RTL930X_PIE_CTRL. We use always the same definition and map to the inner VLAN tag:
 */
#define TEMPLATE_FIELD_VLAN TEMPLATE_FIELD_ITAG

// Number of fixed templates predefined in the RTL9300 SoC
#define N_FIXED_TEMPLATES 5
// RTL9300 specific predefined templates
static enum template_field_id fixed_templates[N_FIXED_TEMPLATES][N_FIXED_FIELDS] =
{
	{
	  TEMPLATE_FIELD_DMAC0, TEMPLATE_FIELD_DMAC1, TEMPLATE_FIELD_DMAC2,
	  TEMPLATE_FIELD_SMAC0, TEMPLATE_FIELD_SMAC1, TEMPLATE_FIELD_SMAC2,
	  TEMPLATE_FIELD_VLAN, TEMPLATE_FIELD_IP_TOS_PROTO, TEMPLATE_FIELD_DSAP_SSAP,
	  TEMPLATE_FIELD_ETHERTYPE, TEMPLATE_FIELD_SPM0, TEMPLATE_FIELD_SPM1
	}, {
	  TEMPLATE_FIELD_SIP0, TEMPLATE_FIELD_SIP1, TEMPLATE_FIELD_DIP0,
	  TEMPLATE_FIELD_DIP1, TEMPLATE_FIELD_IP_TOS_PROTO, TEMPLATE_FIELD_TCP_INFO,
	  TEMPLATE_FIELD_L4_SPORT, TEMPLATE_FIELD_L4_DPORT, TEMPLATE_FIELD_VLAN,
	  TEMPLATE_FIELD_RANGE_CHK, TEMPLATE_FIELD_SPM0, TEMPLATE_FIELD_SPM1
	}, {
	  TEMPLATE_FIELD_DMAC0, TEMPLATE_FIELD_DMAC1, TEMPLATE_FIELD_DMAC2,
	  TEMPLATE_FIELD_VLAN, TEMPLATE_FIELD_ETHERTYPE, TEMPLATE_FIELD_IP_TOS_PROTO,
	  TEMPLATE_FIELD_SIP0, TEMPLATE_FIELD_SIP1, TEMPLATE_FIELD_DIP0,
	  TEMPLATE_FIELD_DIP1, TEMPLATE_FIELD_L4_SPORT, TEMPLATE_FIELD_L4_DPORT
	}, {
	  TEMPLATE_FIELD_DIP0, TEMPLATE_FIELD_DIP1, TEMPLATE_FIELD_DIP2,
	  TEMPLATE_FIELD_DIP3, TEMPLATE_FIELD_DIP4, TEMPLATE_FIELD_DIP5,
	  TEMPLATE_FIELD_DIP6, TEMPLATE_FIELD_DIP7, TEMPLATE_FIELD_IP_TOS_PROTO,
	  TEMPLATE_FIELD_TCP_INFO, TEMPLATE_FIELD_L4_SPORT, TEMPLATE_FIELD_L4_DPORT
	}, {
	  TEMPLATE_FIELD_SIP0, TEMPLATE_FIELD_SIP1, TEMPLATE_FIELD_SIP2,
	  TEMPLATE_FIELD_SIP3, TEMPLATE_FIELD_SIP4, TEMPLATE_FIELD_SIP5,
	  TEMPLATE_FIELD_SIP6, TEMPLATE_FIELD_SIP7, TEMPLATE_FIELD_VLAN,
	  TEMPLATE_FIELD_RANGE_CHK, TEMPLATE_FIELD_SPM1, TEMPLATE_FIELD_SPM1
	},
};

void rtl930x_print_matrix(void)
{
	int i;
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 6);

	for (i = 0; i < 29; i++) {
		rtl_table_read(r, i);
		pr_debug("> %08x\n", sw_r32(rtl_table_data(r, 0)));
	}
	rtl_table_release(r);
}

inline void rtl930x_exec_tbl0_cmd(u32 cmd)
{
	sw_w32(cmd, RTL930X_TBL_ACCESS_CTRL_0);
	do { } while (sw_r32(RTL930X_TBL_ACCESS_CTRL_0) & (1 << 17));
}

inline void rtl930x_exec_tbl1_cmd(u32 cmd)
{
	sw_w32(cmd, RTL930X_TBL_ACCESS_CTRL_1);
	do { } while (sw_r32(RTL930X_TBL_ACCESS_CTRL_1) & (1 << 17));
}

inline int rtl930x_tbl_access_data_0(int i)
{
	return RTL930X_TBL_ACCESS_DATA_0(i);
}

static inline int rtl930x_l2_port_new_salrn(int p)
{
	return RTL930X_L2_PORT_SALRN(p);
}

static inline int rtl930x_l2_port_new_sa_fwd(int p)
{
	// TODO: The definition of the fields changed, because of the master-cpu in a stack
	return RTL930X_L2_PORT_NEW_SA_FWD(p);
}

inline static int rtl930x_trk_mbr_ctr(int group)
{
	return RTL930X_TRK_MBR_CTRL + (group << 2);
}

static void rtl930x_vlan_tables_read(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 v, w;
	// Read VLAN table (1) via register 0
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 1);

	rtl_table_read(r, vlan);
	v = sw_r32(rtl_table_data(r, 0));
	w = sw_r32(rtl_table_data(r, 1));
	pr_debug("VLAN_READ %d: %08x %08x\n", vlan, v, w);
	rtl_table_release(r);

	info->tagged_ports = v >> 3;
	info->profile_id = (w >> 24) & 7;
	info->hash_mc_fid = !!(w & BIT(27));
	info->hash_uc_fid = !!(w & BIT(28));
	info->fid = ((v & 0x7) << 3) | ((w >> 29) & 0x7);

	// Read UNTAG table via table register 2
	r = rtl_table_get(RTL9300_TBL_2, 0);
	rtl_table_read(r, vlan);
	v = sw_r32(rtl_table_data(r, 0));
	rtl_table_release(r);

	info->untagged_ports = v >> 3;
}

static void rtl930x_vlan_set_tagged(u32 vlan, struct rtl838x_vlan_info *info)
{
	u32 v, w;
	// Access VLAN table (1) via register 0
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 1);

	v = info->tagged_ports << 3;
	v |= ((u32)info->fid) >> 3;

	w = ((u32)info->fid) << 29;
	w |= info->hash_mc_fid ? BIT(27) : 0;
	w |= info->hash_uc_fid ? BIT(28) : 0;
	w |= info->profile_id << 24;

	sw_w32(v, rtl_table_data(r, 0));
	sw_w32(w, rtl_table_data(r, 1));

	rtl_table_write(r, vlan);
	rtl_table_release(r);
}

void rtl930x_vlan_profile_dump(int profile)
{
	u32 p[5];

	if (profile < 0 || profile > 7)
		return;

	p[0] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile));
	p[1] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile) + 4);
	p[2] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile) + 8) & 0x1FFFFFFF;
	p[3] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile) + 12) & 0x1FFFFFFF;
	p[4] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile) + 16) & 0x1FFFFFFF;

	pr_info("VLAN %d: L2 learn: %d; Unknown MC PMasks: L2 %0x, IPv4 %0x, IPv6: %0x",
		profile, p[0] & (3 << 21), p[2], p[3], p[4]);
	pr_info("  Routing enabled: IPv4 UC %c, IPv6 UC %c, IPv4 MC %c, IPv6 MC %c\n",
		p[0] & BIT(17) ? 'y' : 'n', p[0] & BIT(16) ? 'y' : 'n',
		p[0] & BIT(13) ? 'y' : 'n', p[0] & BIT(12) ? 'y' : 'n');
	pr_info("  Bridge enabled: IPv4 MC %c, IPv6 MC %c,\n",
		p[0] & BIT(15) ? 'y' : 'n', p[0] & BIT(14) ? 'y' : 'n');
	pr_info("VLAN profile %d: raw %08x %08x %08x %08x %08x\n",
		profile, p[0], p[1], p[2], p[3], p[4]);
}

static void rtl930x_vlan_set_untagged(u32 vlan, u64 portmask)
{
	struct table_reg *r = rtl_table_get(RTL9300_TBL_2, 0);

	sw_w32(portmask << 3, rtl_table_data(r, 0));
	rtl_table_write(r, vlan);
	rtl_table_release(r);
}

/* Sets the L2 forwarding to be based on either the inner VLAN tag or the outer
 */
static void rtl930x_vlan_fwd_on_inner(int port, bool is_set)
{
	// Always set all tag modes to fwd based on either inner or outer tag
	if (is_set)
		sw_w32_mask(0, 0xf, RTL930X_VLAN_PORT_FWD + (port << 2));
	else
		sw_w32_mask(0xf, 0, RTL930X_VLAN_PORT_FWD + (port << 2));
}

static void rtl930x_vlan_profile_setup(int profile)
{
	u32 p[5];

	pr_info("In %s\n", __func__);
	p[0] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile));
	p[1] = sw_r32(RTL930X_VLAN_PROFILE_SET(profile) + 4);

	// Enable routing of Ipv4/6 Unicast and IPv4/6 Multicast traffic
	p[0] |= BIT(17) | BIT(16) | BIT(13) | BIT(12);
	p[2] = 0x1fffffff; // L2 unknown MC flooding portmask all ports, including the CPU-port
	p[3] = 0x1fffffff; // IPv4 unknown MC flooding portmask
	p[4] = 0x1fffffff; // IPv6 unknown MC flooding portmask

	sw_w32(p[0], RTL930X_VLAN_PROFILE_SET(profile));
	sw_w32(p[1], RTL930X_VLAN_PROFILE_SET(profile) + 4);
	sw_w32(p[2], RTL930X_VLAN_PROFILE_SET(profile) + 8);
	sw_w32(p[3], RTL930X_VLAN_PROFILE_SET(profile) + 12);
	sw_w32(p[4], RTL930X_VLAN_PROFILE_SET(profile) + 16);
	pr_info("Leaving %s\n", __func__);
}

static void rtl930x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, u32 port_state[])
{
	int i;
	u32 cmd = 1 << 17 /* Execute cmd */
		| 0 << 16 /* Read */
		| 4 << 12 /* Table type 0b10 */
		| (msti & 0xfff);
	priv->r->exec_tbl0_cmd(cmd);

	for (i = 0; i < 2; i++)
		port_state[i] = sw_r32(RTL930X_TBL_ACCESS_DATA_0(i));
	pr_debug("MSTI: %d STATE: %08x, %08x\n", msti, port_state[0], port_state[1]);
}

static void rtl930x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, u32 port_state[])
{
	int i;
	u32 cmd = 1 << 17 /* Execute cmd */
		| 1 << 16 /* Write */
		| 4 << 12 /* Table type 4 */
		| (msti & 0xfff);

	for (i = 0; i < 2; i++)
		sw_w32(port_state[i], RTL930X_TBL_ACCESS_DATA_0(i));
	priv->r->exec_tbl0_cmd(cmd);
}

static inline int rtl930x_mac_force_mode_ctrl(int p)
{
	return RTL930X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl930x_mac_port_ctrl(int p)
{
	return RTL930X_MAC_L2_PORT_CTRL(p);
}

static inline int rtl930x_mac_link_spd_sts(int p)
{
	return RTL930X_MAC_LINK_SPD_STS(p);
}

static u64 rtl930x_l2_hash_seed(u64 mac, u32 vid)
{
	u64 v = vid;

	v <<= 48;
	v |= mac;

	return v;
}

/*
 * Calculate both the block 0 and the block 1 hash by applyingthe same hash
 * algorithm as the one used currently by the ASIC to the seed, and return
 * both hashes in the lower and higher word of the return value since only 12 bit of
 * the hash are significant
 */
static u32 rtl930x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 k0, k1, h1, h2, h;

	k0 = (u32) (((seed >> 55) & 0x1f) ^ ((seed >> 44) & 0x7ff)
		^ ((seed >> 33) & 0x7ff) ^ ((seed >> 22) & 0x7ff)
		^ ((seed >> 11) & 0x7ff) ^ (seed & 0x7ff));

	h1 = (seed >> 11) & 0x7ff;
	h1 = ((h1 & 0x1f) << 6) | ((h1 >> 5) & 0x3f);

	h2 = (seed >> 33) & 0x7ff;
	h2 = ((h2 & 0x3f) << 5)| ((h2 >> 6) & 0x3f);

	k1 = (u32) (((seed << 55) & 0x1f) ^ ((seed >> 44) & 0x7ff) ^ h2
		    ^ ((seed >> 22) & 0x7ff) ^ h1
		    ^ (seed & 0x7ff));

	// Algorithm choice for block 0
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

/*
 * Fills an L2 entry structure from the SoC registers
 */
static void rtl930x_fill_l2_entry(u32 r[], struct rtl838x_l2_entry *e)
{
	pr_debug("In %s valid?\n", __func__);
	e->valid = !!(r[2] & BIT(31));
	if (!e->valid)
		return;

	pr_debug("In %s is valid\n", __func__);
	e->is_ip_mc = false;
	e->is_ipv6_mc = false;

	// TODO: Is there not a function to copy directly MAC memory?
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
		// Check for trunk port
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
		e->age = (r[2] >> 17) & 3;
		e->valid = true;
		// the UC_VID field in hardware is used for the VID or for the route id
		if (e->next_hop) {
			e->nh_route_id = r[2] & 0xfff;
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

/*
 * Fills the 3 SoC table registers r[] with the information of in the rtl838x_l2_entry
 */
static void rtl930x_fill_l2_row(u32 r[], struct rtl838x_l2_entry *e)
{
	u32 port;

	if (!e->valid) {
		r[0] = r[1] = r[2] = 0;
		return;
	}

	r[2] = BIT(31);	// Set valid bit

	r[0] = ((u32)e->mac[0]) << 24 | ((u32)e->mac[1]) << 16 
		| ((u32)e->mac[2]) << 8 | ((u32)e->mac[3]);
	r[1] = ((u32)e->mac[4]) << 24 | ((u32)e->mac[5]) << 16;

	r[2] |= e->next_hop ? BIT(12) : 0;

	if (e->type == L2_UNICAST) {
		r[2] |= e->is_static ? BIT(14) : 0;
		r[1] |= e->rvid & 0xfff;
		r[2] |= (e->port & 0x3ff) << 20;
		if (e->is_trunk) {
			r[2] |= BIT(30);
			port = e->stack_dev << 9 | (e->port & 0x3f);
		} else {
			port = (e->stack_dev & 0xf) << 6;
			port |= e->port & 0x3f;
		}
		r[2] |= port << 20;
		r[2] |= e->block_da ? BIT(15) : 0;
		r[2] |= e->block_sa ? BIT(17) : 0;
		r[2] |= e->suspended ? BIT(13) : 0;
		r[2] |= (e->age & 0x3) << 17;
		// the UC_VID field in hardware is used for the VID or for the route id
		if (e->next_hop)
			r[2] |= e->nh_route_id & 0xfff;
		else
			r[2] |= e->vid & 0xfff;
	} else { // L2_MULTICAST
		r[2] |= (e->mc_portmask_index & 0x3ff) << 16;
		r[2] |= e->mc_mac_index & 0x7ff;
	}
}

/*
 * Read an L2 UC or MC entry out of a hash bucket of the L2 forwarding table
 * hash is the id of the bucket and pos is the position of the entry in that bucket
 * The data read from the SoC is filled into rtl838x_l2_entry
 */
static u64 rtl930x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	struct table_reg *q = rtl_table_get(RTL9300_TBL_L2, 0);
	u32 idx;
	int i;
	u64 mac;
	u64 seed;

	pr_debug("%s: hash %08x, pos: %d\n", __func__, hash, pos);

	/* On the RTL93xx, 2 different hash algorithms are used making it a total of
	 * 8 buckets that need to be searched, 4 for each hash-half
	 * Use second hash space when bucket is between 4 and 8 */
	if (pos >= 4) {
		pos -= 4;
		hash >>= 16;
	} else {
		hash &= 0xffff;
	}

	idx = (0 << 14) | (hash << 2) | pos; // Search SRAM, with hash and at pos in bucket
	pr_debug("%s: NOW hash %08x, pos: %d\n", __func__, hash, pos);

	rtl_table_read(q, idx);
	for (i = 0; i < 3; i++)
		r[i] = sw_r32(rtl_table_data(q, i));

	rtl_table_release(q);

	rtl930x_fill_l2_entry(r, e);

	pr_debug("%s: valid: %d, nh: %d\n", __func__, e->valid, e->next_hop);
	if (!e->valid)
		return 0;

	mac = ((u64)e->mac[0]) << 40 | ((u64)e->mac[1]) << 32 | ((u64)e->mac[2]) << 24
		| ((u64)e->mac[3]) << 16 | ((u64)e->mac[4]) << 8 | ((u64)e->mac[5]);

	seed = rtl930x_l2_hash_seed(mac, e->rvid);
	pr_debug("%s: mac %016llx, seed %016llx\n", __func__, mac, seed);
	// return vid with concatenated mac as unique id
	return seed;
}

static void rtl930x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	struct table_reg *q = rtl_table_get(RTL9300_TBL_L2, 0);
	u32 idx = (0 << 14) | (hash << 2) | pos; // Access SRAM, with hash and at pos in bucket
	int i;

	pr_info("%s: hash %d, pos %d\n", __func__, hash, pos);
	pr_info("%s: index %d -> mac %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, idx,
		e->mac[0], e->mac[1], e->mac[2], e->mac[3],e->mac[4],e->mac[5]);

	rtl930x_fill_l2_row(r, e);

	for (i= 0; i < 3; i++)
		sw_w32(r[i], rtl_table_data(q, i));

	rtl_table_write(q, idx);
	rtl_table_release(q);
}

static u64 rtl930x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	struct table_reg *q = rtl_table_get(RTL9300_TBL_L2, 1);
	int i;

	rtl_table_read(q, idx);
	for (i= 0; i < 3; i++)
		r[i] = sw_r32(rtl_table_data(q, i));

	rtl_table_release(q);

	rtl930x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	// return mac with concatenated vid as unique id
	return ((u64)r[0] << 28) | ((r[1] & 0xffff0000) >> 4) | e->vid;
}

static void rtl930x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	struct table_reg *q = rtl_table_get(RTL9300_TBL_L2, 1); // Access L2 Table 1
	int i;

	rtl930x_fill_l2_row(r, e);

	for (i= 0; i < 3; i++)
		sw_w32(r[i], rtl_table_data(q, i));

	rtl_table_write(q, idx);
	rtl_table_release(q);
}

static void dump_l2_entry(struct rtl838x_l2_entry *e)
{
	pr_info("MAC: %02x:%02x:%02x:%02x:%02x:%02x vid: %d, rvid: %d, port: %d, valid: %d\n",
		e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5],
		e->vid, e->rvid, e->port, e->valid);
	pr_info("Type: %d, is_static: %d, is_ip_mc: %d, is_ipv6_mc: %d, block_da: %d\n",
		e->type, e->is_static, e->is_ip_mc, e->is_ipv6_mc, e->block_da);
	pr_info("  block_sa: %d, suspended: %d, next_hop: %d, age: %d, is_trunk: %d, trunk: %d\n",
		e->block_sa, e->suspended, e->next_hop, e->age, e->is_trunk, e->trunk);
	if (e->is_ip_mc || e->is_ipv6_mc)
		pr_info("  mc_portmask_index: %d, mc_gip: %d, mc_sip: %d\n",
			e->mc_portmask_index, e->mc_gip, e->mc_sip);
	pr_info("  stac_dev: %d, nh_route_id: %d, port: %d, dev_id\n",
		e->stack_dev, e->nh_route_id, e->port);
}

/*
 * Add an L2 nexthop entry for the L3 routing system in the SoC
 * Use VID and MAC in rtl838x_l2_entry to identify either a free slot in the L2 hash table
 * or mark an existing entry as a nexthop by setting it's nexthop bit
 * Called from the L3 layer
 * The index in the L2 hash table is filled into nh->l2_id;
 */
static int rtl930x_l2_nexthop_add(struct rtl838x_switch_priv *priv, struct rtl838x_nexthop *nh)
{
	struct rtl838x_l2_entry e;
	u64 seed = rtl930x_l2_hash_seed(nh->mac, nh->vid);
	u32 key = rtl930x_l2_hash_key(priv, seed);
	int i, idx = -1;
	u64 entry;

	pr_info("%s searching for %08llx vid %d with key %d, seed: %016llx\n",
		__func__, nh->mac, nh->vid, key, seed);
	
	e.type = L2_UNICAST;
	e.rvid = nh->fid; // Verify its the forwarding ID!!! l2_entry.un.unicast.fid
	u64_to_ether_addr(nh->mac, &e.mac[0]);
	e.port = RTL930X_PORT_IGNORE;

	// Loop over all entries in the hash-bucket and over the second block on 93xx SoCs
	for (i = 0; i < priv->l2_bucket_size; i++) {
		entry = rtl930x_read_l2_entry_using_hash(key, i, &e);
		pr_info("%s i: %d, entry %016llx, seed %016llx\n", __func__, i, entry, seed);
		if (e.valid && e.next_hop)
			continue;
		if (!e.valid || ((entry & 0x0fffffffffffffffULL) == seed)) {
			idx = i > 3 ? ((key >> 14) & 0xffff) | i >> 1
					: ((key << 2) | i) & 0xffff;
			break;
		}
	}

	pr_info("%s: found idx %d and i %d\n", __func__, idx, i);

	if (idx < 0) {
		pr_err("%s: No more L2 forwarding entries available\n", __func__);
		return -1;
	}

	// Found an existing or empty entry, make it a nexthop entry
	pr_info("%s BEFORE -> key %d, pos: %d, index: %d\n", __func__, key, i, idx);
	dump_l2_entry(&e);
	nh->l2_id = idx;

	// Found an existing (e->valid is true) or empty entry, make it a nexthop entry
	if (e.valid) {
		nh->port = e.port;
		nh->fid = e.rvid;
		nh->vid = e.vid;
		nh->dev_id = e.stack_dev;
	} else {
		e.valid = true;
		e.is_static = false;
		e.vid = nh->vid;
		e.rvid = nh->fid;
		e.port = RTL930X_PORT_IGNORE;
		u64_to_ether_addr(nh->mac, &e.mac[0]);
	}
	e.next_hop = true;
	// For nexthop entries, the vid field in the table is used to denote the dest mac_id
	e.nh_route_id = nh->mac_id;
	pr_info("%s AFTER\n", __func__);
	dump_l2_entry(&e);

	rtl930x_write_l2_entry_using_hash(idx >> 2, idx & 0x3, &e);

	// _dal_longan_l2_nexthop_add
	return 0;
}

static u64 rtl930x_read_mcast_pmask(int idx)
{
	u32 portmask;
	// Read MC_PORTMASK (2) via register RTL9300_TBL_L2
	struct table_reg *q = rtl_table_get(RTL9300_TBL_L2, 2);

	rtl_table_read(q, idx);
	portmask = sw_r32(rtl_table_data(q, 0));
	portmask >>= 3;
	rtl_table_release(q);

	pr_debug("%s: Index idx %d has portmask %08x\n", __func__, idx, portmask);
	return portmask;
}

static void rtl930x_write_mcast_pmask(int idx, u64 portmask)
{
	u32 pm = portmask;

	// Access MC_PORTMASK (2) via register RTL9300_TBL_L2
	struct table_reg *q = rtl_table_get(RTL9300_TBL_L2, 2);

	pr_debug("%s: Index idx %d has portmask %08x\n", __func__, idx, pm);
	pm <<= 3;
	sw_w32(pm, rtl_table_data(q, 0));
	rtl_table_write(q, idx);
	rtl_table_release(q);
}

u64 rtl930x_traffic_get(int source)
{
	u32 v;
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 6);

	rtl_table_read(r, source);
	v = sw_r32(rtl_table_data(r, 0));
	rtl_table_release(r);
	return v >> 3;
}

/*
 * Enable traffic between a source port and a destination port matrix
 */
void rtl930x_traffic_set(int source, u64 dest_matrix)
{
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 6);

	sw_w32((dest_matrix << 3), rtl_table_data(r, 0));
	rtl_table_write(r, source);
	rtl_table_release(r);
}

void rtl930x_traffic_enable(int source, int dest)
{
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 6);
	rtl_table_read(r, source);
	sw_w32_mask(0, BIT(dest + 3), rtl_table_data(r, 0));
	rtl_table_write(r, source);
	rtl_table_release(r);
}

void rtl930x_traffic_disable(int source, int dest)
{
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 6);
	rtl_table_read(r, source);
	sw_w32_mask(BIT(dest + 3), 0, rtl_table_data(r, 0));
	rtl_table_write(r, source);
	rtl_table_release(r);
}

void rtl9300_dump_debug(void)
{
	int i;
	u16 r = RTL930X_STAT_PRVTE_DROP_COUNTER0;

	for (i = 0; i < 10; i ++) {
		pr_info("# %d %08x %08x %08x %08x %08x %08x %08x %08x\n", i * 8,
			sw_r32(r), sw_r32(r + 4), sw_r32(r + 8), sw_r32(r + 12),
			sw_r32(r + 16), sw_r32(r + 20), sw_r32(r + 24), sw_r32(r + 28));
		r += 32;
	}
	pr_info("# %08x %08x %08x %08x %08x\n",
		sw_r32(r), sw_r32(r + 4), sw_r32(r + 8), sw_r32(r + 12), sw_r32(r + 16));
	rtl930x_print_matrix();
	pr_info("RTL930X_L2_PORT_SABLK_CTRL: %08x, RTL930X_L2_PORT_DABLK_CTRL %08x\n",
		sw_r32(RTL930X_L2_PORT_SABLK_CTRL), sw_r32(RTL930X_L2_PORT_DABLK_CTRL)

	);
}

irqreturn_t rtl930x_switch_irq(int irq, void *dev_id)
{
	struct dsa_switch *ds = dev_id;
	u32 status = sw_r32(RTL930X_ISR_GLB);
	u32 ports = sw_r32(RTL930X_ISR_PORT_LINK_STS_CHG);
	u32 link;
	int i;

	/* Clear status */
	sw_w32(ports, RTL930X_ISR_PORT_LINK_STS_CHG);
	pr_info("RTL9300 Link change: status: %x, ports %x\n", status, ports);

	rtl9300_dump_debug();

	for (i = 0; i < 28; i++) {
		if (ports & BIT(i)) {
			/* Read the register twice because of issues with latency at least
			 * with the external RTL8226 PHY on the XGS1210 */
			link = sw_r32(RTL930X_MAC_LINK_STS);
			link = sw_r32(RTL930X_MAC_LINK_STS);
			if (link & BIT(i))
				dsa_port_phylink_mac_change(ds, i, true);
			else
				dsa_port_phylink_mac_change(ds, i, false);
		}
	}

	return IRQ_HANDLED;
}

int rtl9300_sds_power(int mac, int val)
{
	int sds_num;
	u32 mode;

	// TODO: these numbers are hard-coded for the Zyxel XGS1210 12 Switch
	pr_info("SerDes: %s %d\n", __func__, mac);
	switch (mac) {
	case 24:
		sds_num = 6;
		mode = 0x12; // HISGMII
		break;
	case 25:
		sds_num = 7;
		mode = 0x12; // HISGMII
		break;
	case 26:
		sds_num = 8;
		mode = 0x1b; // 10GR/1000BX auto
		break;
	case 27:
		sds_num = 9;
		mode = 0x1b; // 10GR/1000BX auto
		break;
	default:
		return -1;
	}
	if (!val)
		mode = 0x1f; // OFF

	rtl9300_sds_rst(sds_num, mode);

	return 0;
}

int rtl930x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;
	int err = 0;

	pr_debug("%s: port %d, page: %d, reg: %x, val: %x\n", __func__, port, page, reg, val);

	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	val &= 0xffff;
	mutex_lock(&smi_lock);

	sw_w32(BIT(port), RTL930X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff << 16, val << 16, RTL930X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3 | 0x1f << 15 | BIT(2) | BIT(0);
	sw_w32(v, RTL930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTL930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & 0x1);

	if (v & 0x2)
		err = -EIO;

	mutex_unlock(&smi_lock);

	return err;
}

int rtl930x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 v;
	int err = 0;

	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&smi_lock);

	sw_w32_mask(0xffff << 16, port << 16, RTL930X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3 | 0x1f << 15 | 1;
	sw_w32(v, RTL930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTL930X_SMI_ACCESS_PHY_CTRL_1);
	} while ( v & 0x1);

	if (v & BIT(25)) {
		pr_debug("Error reading phy %d, register %d\n", port, reg);
		err = -EIO;
	}
	*val = (sw_r32(RTL930X_SMI_ACCESS_PHY_CTRL_2) & 0xffff);

	pr_debug("%s: port %d, page: %d, reg: %x, val: %x\n", __func__, port, page, reg, *val);

	mutex_unlock(&smi_lock);

	return err;
}

/*
 * Write to an mmd register of the PHY
 */
int rtl930x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	int err = 0;
	u32 v;

	mutex_lock(&smi_lock);

	// Set PHY to access
	sw_w32(BIT(port), RTL930X_SMI_ACCESS_PHY_CTRL_0);

	// Set data to write
	sw_w32_mask(0xffff << 16, val << 16, RTL930X_SMI_ACCESS_PHY_CTRL_2);

	// Set MMD device number and register to write to
	sw_w32(devnum << 16 | (regnum & 0xffff), RTL930X_SMI_ACCESS_PHY_CTRL_3);

	v = BIT(2) | BIT(1) | BIT(0); // WRITE | MMD-access | EXEC
	sw_w32(v, RTL930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTL930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & BIT(0));

	pr_debug("%s: port %d, regnum: %x, val: %x (err %d)\n", __func__, port, regnum, val, err);
	mutex_unlock(&smi_lock);
	return err;
}

/*
 * Read an mmd register of the PHY
 */
int rtl930x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err = 0;
	u32 v;

	mutex_lock(&smi_lock);

	// Set PHY to access
	sw_w32_mask(0xffff << 16, port << 16, RTL930X_SMI_ACCESS_PHY_CTRL_2);

	// Set MMD device number and register to write to
	sw_w32(devnum << 16 | (regnum & 0xffff), RTL930X_SMI_ACCESS_PHY_CTRL_3);

	v = BIT(1) | BIT(0); // MMD-access | EXEC
	sw_w32(v, RTL930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTL930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & BIT(0));
	// There is no error-checking via BIT 25 of v, as it does not seem to be set correctly
	*val = (sw_r32(RTL930X_SMI_ACCESS_PHY_CTRL_2) & 0xffff);
	pr_debug("%s: port %d, regnum: %x, val: %x (err %d)\n", __func__, port, regnum, *val, err);

	mutex_unlock(&smi_lock);

	return err;
}

/*
 * Calculate both the block 0 and the block 1 hash, and return in
 * lower and higher word of the return value since only 12 bit of
 * the hash are significant
 */
u32 rtl930x_hash(struct rtl838x_switch_priv *priv, u64 seed)
{
	u32 k0, k1, h1, h2, h;

	k0 = (u32) (((seed >> 55) & 0x1f) ^ ((seed >> 44) & 0x7ff)
		^ ((seed >> 33) & 0x7ff) ^ ((seed >> 22) & 0x7ff)
		^ ((seed >> 11) & 0x7ff) ^ (seed & 0x7ff));

	h1 = (seed >> 11) & 0x7ff;
	h1 = ((h1 & 0x1f) << 6) | ((h1 >> 5) & 0x3f);

	h2 = (seed >> 33) & 0x7ff;
	h2 = ((h2 & 0x3f) << 5)| ((h2 >> 6) & 0x3f);

	k1 = (u32) (((seed << 55) & 0x1f) ^ ((seed >> 44) & 0x7ff) ^ h2
		    ^ ((seed >> 22) & 0x7ff) ^ h1
		    ^ (seed & 0x7ff));

	// Algorithm choice for block 0
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

/*
 * Enables or disables the EEE/EEEP capability of a port
 */
void rtl930x_port_eee_set(struct rtl838x_switch_priv *priv, int port, bool enable)
{
	u32 v;

	// This works only for Ethernet ports, and on the RTL930X, ports from 26 are SFP
	if (port >= 26)
		return;

	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0x3f : 0x0;

	// Set EEE/EEEP state for 100, 500, 1000MBit and 2.5, 5 and 10GBit
	sw_w32_mask(0, v << 10, rtl930x_mac_force_mode_ctrl(port));

	// Set TX/RX EEE state
	v = enable ? 0x3 : 0x0;
	sw_w32(v, RTL930X_EEE_CTRL(port));

	priv->ports[port].eee_enabled = enable;
}

/*
 * Get EEE own capabilities and negotiation result
 */
int rtl930x_eee_port_ability(struct rtl838x_switch_priv *priv, struct ethtool_eee *e, int port)
{
	u32 link, a;

	if (port >= 26)
		return -ENOTSUPP;

	pr_info("In %s, port %d\n", __func__, port);
	link = sw_r32(RTL930X_MAC_LINK_STS);
	link = sw_r32(RTL930X_MAC_LINK_STS);
	if (!(link & BIT(port)))
		return 0;

	pr_info("Setting advertised\n");
	if (sw_r32(rtl930x_mac_force_mode_ctrl(port)) & BIT(10))
		e->advertised |= ADVERTISED_100baseT_Full;

	if (sw_r32(rtl930x_mac_force_mode_ctrl(port)) & BIT(12))
		e->advertised |= ADVERTISED_1000baseT_Full;

	if (priv->ports[port].is2G5 && sw_r32(rtl930x_mac_force_mode_ctrl(port)) & BIT(13)) {
		pr_info("ADVERTISING 2.5G EEE\n");
		e->advertised |= ADVERTISED_2500baseX_Full;
	}

	if (priv->ports[port].is10G && sw_r32(rtl930x_mac_force_mode_ctrl(port)) & BIT(15))
		e->advertised |= ADVERTISED_10000baseT_Full;

	a = sw_r32(RTL930X_MAC_EEE_ABLTY);
	a = sw_r32(RTL930X_MAC_EEE_ABLTY);
	pr_info("Link partner: %08x\n", a);
	if (a & BIT(port)) {
		e->lp_advertised = ADVERTISED_100baseT_Full;
		e->lp_advertised |= ADVERTISED_1000baseT_Full;
		if (priv->ports[port].is2G5)
			e->lp_advertised |= ADVERTISED_2500baseX_Full;
		if (priv->ports[port].is10G)
			e->lp_advertised |= ADVERTISED_10000baseT_Full;
	}

	// Read 2x to clear latched state
	a = sw_r32(RTL930X_EEEP_PORT_CTRL(port));
	a = sw_r32(RTL930X_EEEP_PORT_CTRL(port));
	pr_info("%s RTL930X_EEEP_PORT_CTRL: %08x\n", __func__, a);

	return 0;
}

static void rtl930x_init_eee(struct rtl838x_switch_priv *priv, bool enable)
{
	int i;

	pr_info("Setting up EEE, state: %d\n", enable);

	// Setup EEE on all ports
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy)
			rtl930x_port_eee_set(priv, i, enable);
	}

	priv->eee_enabled = enable;
}

static void rtl930x_pie_lookup_enable(struct rtl838x_switch_priv *priv, int index)
{
	int block = index / PIE_BLOCK_SIZE;

	sw_w32_mask(0, BIT(block), RTL930X_PIE_BLK_LOOKUP_CTRL);
}

/*
 * Reads the intermediate representation of the templated match-fields of the
 * PIE rule in the pie_rule structure and fills in the raw data fields in the
 * raw register space r[].
 * The register space configuration size is identical for the RTL8380/90 and RTL9300,
 * however the RTL9310 has 2 more registers / fields and the physical field-ids are different
 * on all SoCs
 * On the RTL9300 the mask fields are not word-aligend!
 */
static void rtl930x_write_pie_templated(u32 r[], struct pie_rule *pr, enum template_field_id t[])
{
	int i;
	enum template_field_id field_type;
	u16 data, data_m;

	for (i = 0; i < N_FIXED_FIELDS; i++) {
		field_type = t[i];
		data = data_m = 0;

		switch (field_type) {
		case TEMPLATE_FIELD_SPM0:
			data = pr->spm;
			data_m = pr->spm_m;
			break;
		case TEMPLATE_FIELD_SPM1:
			data = pr->spm >> 16;
			data_m = pr->spm_m >> 16;
			break;
		case TEMPLATE_FIELD_OTAG:
			data = pr->otag;
			data_m = pr->otag_m;
			break;
		case TEMPLATE_FIELD_SMAC0:
			data = pr->smac[4];
			data = (data << 8) | pr->smac[5];
			data_m = pr->smac_m[4];
			data_m = (data_m << 8) | pr->smac_m[5];
			break;
		case TEMPLATE_FIELD_SMAC1:
			data = pr->smac[2];
			data = (data << 8) | pr->smac[3];
			data_m = pr->smac_m[2];
			data_m = (data_m << 8) | pr->smac_m[3];
			break;
		case TEMPLATE_FIELD_SMAC2:
			data = pr->smac[0];
			data = (data << 8) | pr->smac[1];
			data_m = pr->smac_m[0];
			data_m = (data_m << 8) | pr->smac_m[1];
			break;
		case TEMPLATE_FIELD_DMAC0:
			data = pr->dmac[4];
			data = (data << 8) | pr->dmac[5];
			data_m = pr->dmac_m[4];
			data_m = (data_m << 8) | pr->dmac_m[5];
			break;
		case TEMPLATE_FIELD_DMAC1:
			data = pr->dmac[2];
			data = (data << 8) | pr->dmac[3];
			data_m = pr->dmac_m[2];
			data_m = (data_m << 8) | pr->dmac_m[3];
			break;
		case TEMPLATE_FIELD_DMAC2:
			data = pr->dmac[0];
			data = (data << 8) | pr->dmac[1];
			data_m = pr->dmac_m[0];
			data_m = (data_m << 8) | pr->dmac_m[1];
			break;
		case TEMPLATE_FIELD_ETHERTYPE:
			data = pr->ethertype;
			data_m = pr->ethertype_m;
			break;
		case TEMPLATE_FIELD_ITAG:
			data = pr->itag;
			data_m = pr->itag_m;
			break;
		case TEMPLATE_FIELD_SIP0:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[7];
				data_m = pr->sip6_m.s6_addr16[7];
			} else {
				data = pr->sip;
				data_m = pr->sip_m;
			}
			break;
		case TEMPLATE_FIELD_SIP1:
			if (pr->is_ipv6) {
				data = pr->sip6.s6_addr16[6];
				data_m = pr->sip6_m.s6_addr16[6];
			} else {
				data = pr->sip >> 16;
				data_m = pr->sip_m >> 16;
			}
			break;

		case TEMPLATE_FIELD_SIP2:
		case TEMPLATE_FIELD_SIP3:
		case TEMPLATE_FIELD_SIP4:
		case TEMPLATE_FIELD_SIP5:
		case TEMPLATE_FIELD_SIP6:
		case TEMPLATE_FIELD_SIP7:
			data = pr->sip6.s6_addr16[5 - (field_type - TEMPLATE_FIELD_SIP2)];
			data_m = pr->sip6_m.s6_addr16[5 - (field_type - TEMPLATE_FIELD_SIP2)];
			break;

		case TEMPLATE_FIELD_DIP0:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[7];
				data_m = pr->dip6_m.s6_addr16[7];
			} else {
				data = pr->dip;
				data_m = pr->dip_m;
			}
			break;

		case TEMPLATE_FIELD_DIP1:
			if (pr->is_ipv6) {
				data = pr->dip6.s6_addr16[6];
				data_m = pr->dip6_m.s6_addr16[6];
			} else {
				data = pr->dip >> 16;
				data_m = pr->dip_m >> 16;
			}
			break;

		case TEMPLATE_FIELD_DIP2:
		case TEMPLATE_FIELD_DIP3:
		case TEMPLATE_FIELD_DIP4:
		case TEMPLATE_FIELD_DIP5:
		case TEMPLATE_FIELD_DIP6:
		case TEMPLATE_FIELD_DIP7:
			data = pr->dip6.s6_addr16[5 - (field_type - TEMPLATE_FIELD_DIP2)];
			data_m = pr->dip6_m.s6_addr16[5 - (field_type - TEMPLATE_FIELD_DIP2)];
			break;

		case TEMPLATE_FIELD_IP_TOS_PROTO:
			data = pr->tos_proto;
			data_m = pr->tos_proto_m;
			break;
		case TEMPLATE_FIELD_L4_SPORT:
			data = pr->sport;
			data_m = pr->sport_m;
			break;
		case TEMPLATE_FIELD_L4_DPORT:
			data = pr->dport;
			data_m = pr->dport_m;
			break;
		case TEMPLATE_FIELD_DSAP_SSAP:
			data = pr->dsap_ssap;
			data_m = pr->dsap_ssap_m;
			break;
		case TEMPLATE_FIELD_TCP_INFO:
			data = pr->tcp_info;
			data_m = pr->tcp_info_m;
			break;
		case TEMPLATE_FIELD_RANGE_CHK:
			pr_warn("Warning: TEMPLATE_FIELD_RANGE_CHK: not configured\n");
			break;
		default:
			pr_info("%s: unknown field %d\n", __func__, field_type);
		}

		// On the RTL9300, the mask fields are not word aligned!
		if (!(i % 2)) {
			r[5 - i / 2] = data;
			r[12 - i / 2] |= ((u32)data_m << 8);
		} else {
			r[5 - i / 2] |= ((u32)data) << 16;
			r[12 - i / 2] |= ((u32)data_m) << 24;
			r[11 - i / 2] |= ((u32)data_m) >> 8;
		}
	}
}

static void rtl930x_read_pie_fixed_fields(u32 r[], struct pie_rule *pr)
{
	pr->stacking_port = r[6] & BIT(31);
	pr->spn = (r[6] >> 24) & 0x7f;
	pr->mgnt_vlan = r[6] & BIT(23);
	if (pr->phase == PHASE_IACL)
		pr->dmac_hit_sw = r[6] & BIT(22);
	else
		pr->content_too_deep = r[6] & BIT(22);
	pr->not_first_frag = r[6]  & BIT(21);
	pr->frame_type_l4 = (r[6] >> 18) & 7;
	pr->frame_type = (r[6] >> 16) & 3;
	pr->otag_fmt = (r[6] >> 15) & 1;
	pr->itag_fmt = (r[6] >> 14) & 1;
	pr->otag_exist = (r[6] >> 13) & 1;
	pr->itag_exist = (r[6] >> 12) & 1;
	pr->frame_type_l2 = (r[6] >> 10) & 3;
	pr->igr_normal_port = (r[6] >> 9) & 1;
	pr->tid = (r[6] >> 8) & 1;

	pr->stacking_port_m = r[12] & BIT(7);
	pr->spn_m = r[12]  & 0x7f;
	pr->mgnt_vlan_m = r[13] & BIT(31);
	if (pr->phase == PHASE_IACL)
		pr->dmac_hit_sw_m = r[13] & BIT(30);
	else
		pr->content_too_deep_m = r[13] & BIT(30);
	pr->not_first_frag_m = r[13] & BIT(29);
	pr->frame_type_l4_m = (r[13] >> 26) & 7;
	pr->frame_type_m = (r[13] >> 24) & 3;
	pr->otag_fmt_m = r[13] & BIT(23);
	pr->itag_fmt_m = r[13] & BIT(22);
	pr->otag_exist_m = r[13] & BIT(21);
	pr->itag_exist_m = r[13] & BIT (20);
	pr->frame_type_l2_m = (r[13] >> 18) & 3;
	pr->igr_normal_port_m = r[13] & BIT(17);
	pr->tid_m = (r[13] >> 16) & 1;

	pr->valid = r[13] & BIT(15);
	pr->cond_not = r[13] & BIT(14);
	pr->cond_and1 = r[13] & BIT(13);
	pr->cond_and2 = r[13] & BIT(12);
}

static void rtl930x_write_pie_fixed_fields(u32 r[],  struct pie_rule *pr)
{
	r[6] = pr->stacking_port ? BIT(31) : 0;
	r[6] |= ((u32) (pr->spn & 0x7f)) << 24;
	r[6] |= pr->mgnt_vlan ? BIT(23) : 0;
	if (pr->phase == PHASE_IACL)
		r[6] |= pr->dmac_hit_sw ? BIT(22) : 0;
	else
		r[6] |= pr->content_too_deep ? BIT(22) : 0;
	r[6] |= pr->not_first_frag ? BIT(21) : 0;
	r[6] |= ((u32) (pr->frame_type_l4 & 0x7)) << 18;
	r[6] |= ((u32) (pr->frame_type & 0x3)) << 16;
	r[6] |= pr->otag_fmt ? BIT(15) : 0;
	r[6] |= pr->itag_fmt ? BIT(14) : 0;
	r[6] |= pr->otag_exist ? BIT(13) : 0;
	r[6] |= pr->itag_exist ? BIT(12) : 0;
	r[6] |= ((u32) (pr->frame_type_l2 & 0x3)) << 10;
	r[6] |= pr->igr_normal_port ? BIT(9) : 0;
	r[6] |= ((u32) (pr->tid & 0x1)) << 8;

	r[12] |= pr->stacking_port_m ? BIT(7) : 0;
	r[12] |= (u32) (pr->spn_m & 0x7f);
	r[13] |= pr->mgnt_vlan_m ? BIT(31) : 0;
	if (pr->phase == PHASE_IACL)
		r[13] |= pr->dmac_hit_sw_m ? BIT(30) : 0;
	else
		r[13] |= pr->content_too_deep_m ? BIT(30) : 0;
	r[13] |= pr->not_first_frag_m ? BIT(29) : 0;
	r[13] |= ((u32) (pr->frame_type_l4_m & 0x7)) << 26;
	r[13] |= ((u32) (pr->frame_type_m & 0x3)) << 24;
	r[13] |= pr->otag_fmt_m ? BIT(23) : 0;
	r[13] |= pr->itag_fmt_m ? BIT(22) : 0;
	r[13] |= pr->otag_exist_m ? BIT(21) : 0;
	r[13] |= pr->itag_exist_m ? BIT(20) : 0;
	r[13] |= ((u32) (pr->frame_type_l2_m & 0x3)) << 18;
	r[13] |= pr->igr_normal_port_m ? BIT(17) : 0;
	r[13] |= ((u32) (pr->tid_m & 0x1)) << 16;

	r[13] |= pr->valid ? BIT(15) : 0;
	r[13] |= pr->cond_not ? BIT(14) : 0;
	r[13] |= pr->cond_and1 ? BIT(13) : 0;
	r[13] |= pr->cond_and2 ? BIT(12) : 0;
}

static void rtl930x_write_pie_action(u32 r[],  struct pie_rule *pr)
{
	// Either drop or forward
	if (pr->drop) {
		r[14] |= BIT(24) | BIT(25) | BIT(26); // Do Green, Yellow and Red drops
		// Actually DROP, not PERMIT in Green / Yellow / Red
		r[14] |= BIT(23) | BIT(22) | BIT(20);
	} else {
		r[14] |= pr->fwd_sel ? BIT(27) : 0;
		r[14] |= pr->fwd_act << 18;
		r[14] |= BIT(14); // We overwrite any drop
	}
	if (pr->phase == PHASE_VACL)
		r[14] |= pr->fwd_sa_lrn ? BIT(15) : 0;
	r[13] |= pr->bypass_sel ? BIT(5) : 0;
	r[13] |= pr->nopri_sel ? BIT(4) : 0;
	r[13] |= pr->tagst_sel ? BIT(3) : 0;
	r[13] |= pr->ovid_sel ? BIT(1) : 0;
	r[14] |= pr->ivid_sel ? BIT(31) : 0;
	r[14] |= pr->meter_sel ? BIT(30) : 0;
	r[14] |= pr->mir_sel ? BIT(29) : 0;
	r[14] |= pr->log_sel ? BIT(28) : 0;

	r[14] |= ((u32)(pr->fwd_data & 0x3fff)) << 3;
	r[15] |= pr->log_octets ? BIT(31) : 0;
	r[15] |= (u32)(pr->meter_data) << 23;

	r[15] |= ((u32)(pr->ivid_act) << 21) & 0x3;
	r[15] |= ((u32)(pr->ivid_data) << 9) & 0xfff;
	r[16] |= ((u32)(pr->ovid_act) << 30) & 0x3;
	r[16] |= ((u32)(pr->ovid_data) & 0xfff) << 16;
	r[16] |= (pr->mir_data & 0x3) << 6;
	r[17] |= ((u32)(pr->tagst_data) & 0xf) << 28;
	r[17] |= ((u32)(pr->nopri_data) & 0x7) << 25;
	r[17] |= pr->bypass_ibc_sc ? BIT(16) : 0;
}

void rtl930x_pie_rule_dump_raw(u32 r[])
{
	pr_info("Raw IACL table entry:\n");
	pr_info("r 0 - 7: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
	pr_info("r 8 - 15: %08x %08x %08x %08x %08x %08x %08x %08x\n",
		r[8], r[9], r[10], r[11], r[12], r[13], r[14], r[15]);
	pr_info("r 16 - 18: %08x %08x %08x\n", r[16], r[17], r[18]);
	pr_info("Match  : %08x %08x %08x %08x %08x %08x\n", r[0], r[1], r[2], r[3], r[4], r[5]);
	pr_info("Fixed  : %06x\n", r[6] >> 8);
	pr_info("Match M: %08x %08x %08x %08x %08x %08x\n",
		(r[6] << 24) | (r[7] >> 8), (r[7] << 24) | (r[8] >> 8), (r[8] << 24) | (r[9] >> 8),
		(r[9] << 24) | (r[10] >> 8), (r[10] << 24) | (r[11] >> 8),
		(r[11] << 24) | (r[12] >> 8));
	pr_info("R[13]:   %08x\n", r[13]);
	pr_info("Fixed M: %06x\n", ((r[12] << 16) | (r[13] >> 16)) & 0xffffff);
	pr_info("Valid / not / and1 / and2 : %1x\n", (r[13] >> 12) & 0xf);
	pr_info("r 13-16: %08x %08x %08x %08x\n", r[13], r[14], r[15], r[16]);
}

static int rtl930x_pie_rule_write(struct rtl838x_switch_priv *priv, int idx, struct pie_rule *pr)
{
	// Access IACL table (2) via register 0
	struct table_reg *q = rtl_table_get(RTL9300_TBL_0, 2);
	u32 r[19];
	int i;
	int block = idx / PIE_BLOCK_SIZE;
	u32 t_select = sw_r32(RTL930X_PIE_BLK_TMPLTE_CTRL(block));

	pr_debug("%s: %d, t_select: %08x\n", __func__, idx, t_select);

	for (i = 0; i < 19; i++)
		r[i] = 0;

	if (!pr->valid) {
		rtl_table_write(q, idx);
		rtl_table_release(q);
		return 0;
	}
	rtl930x_write_pie_fixed_fields(r, pr);

	pr_debug("%s: template %d\n", __func__, (t_select >> (pr->tid * 4)) & 0xf);
	rtl930x_write_pie_templated(r, pr, fixed_templates[(t_select >> (pr->tid * 4)) & 0xf]);

	rtl930x_write_pie_action(r, pr);

//	rtl930x_pie_rule_dump_raw(r);

	for (i = 0; i < 19; i++)
		sw_w32(r[i], rtl_table_data(q, i));

	rtl_table_write(q, idx);
	rtl_table_release(q);

	return 0;
}

static bool rtl930x_pie_templ_has(int t, enum template_field_id field_type)
{
	int i;
	enum template_field_id ft;

	for (i = 0; i < N_FIXED_FIELDS; i++) {
		ft = fixed_templates[t][i];
		if (field_type == ft)
			return true;
	}

	return false;
}

/*
 * Verify that the rule pr is compatible with a given template t in block block
 * Note that this function is SoC specific since the values of e.g. TEMPLATE_FIELD_SIP0
 * depend on the SoC
 */
static int rtl930x_pie_verify_template(struct rtl838x_switch_priv *priv,
				       struct pie_rule *pr, int t, int block)
{
	int i;

	if (!pr->is_ipv6 && pr->sip_m && !rtl930x_pie_templ_has(t, TEMPLATE_FIELD_SIP0))
		return -1;

	if (!pr->is_ipv6 && pr->dip_m && !rtl930x_pie_templ_has(t, TEMPLATE_FIELD_DIP0))
		return -1;

	if (pr->is_ipv6) {
		if ((pr->sip6_m.s6_addr32[0] || pr->sip6_m.s6_addr32[1]
			|| pr->sip6_m.s6_addr32[2] || pr->sip6_m.s6_addr32[3])
			&& !rtl930x_pie_templ_has(t, TEMPLATE_FIELD_SIP2))
			return -1;
		if ((pr->dip6_m.s6_addr32[0] || pr->dip6_m.s6_addr32[1]
			|| pr->dip6_m.s6_addr32[2] || pr->dip6_m.s6_addr32[3])
			&& !rtl930x_pie_templ_has(t, TEMPLATE_FIELD_DIP2))
			return -1;
	}

	if (ether_addr_to_u64(pr->smac) && !rtl930x_pie_templ_has(t, TEMPLATE_FIELD_SMAC0))
		return -1;

	if (ether_addr_to_u64(pr->dmac) && !rtl930x_pie_templ_has(t, TEMPLATE_FIELD_DMAC0))
		return -1;

	// TODO: Check more

	i = find_first_zero_bit(&priv->pie_use_bm[block * 4], PIE_BLOCK_SIZE);

	if (i >= PIE_BLOCK_SIZE)
		return -1;

	return i + PIE_BLOCK_SIZE * block;
}

static int rtl930x_pie_rule_add(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx, block, j, t;
	int min_block = 0;
	int max_block = priv->n_pie_blocks / 2;

	if (pr->is_egress) {
		min_block = max_block;
		max_block = priv->n_pie_blocks;
	}
	pr_debug("In %s\n", __func__);

	mutex_lock(&priv->pie_mutex);

	for (block = min_block; block < max_block; block++) {
		for (j = 0; j < 2; j++) {
			t = (sw_r32(RTL930X_PIE_BLK_TMPLTE_CTRL(block)) >> (j * 4)) & 0xf;
			pr_debug("Testing block %d, template %d, template id %d\n", block, j, t);
			pr_debug("%s: %08x\n",
				__func__, sw_r32(RTL930X_PIE_BLK_TMPLTE_CTRL(block)));
			idx = rtl930x_pie_verify_template(priv, pr, t, block);
			if (idx >= 0)
				break;
		}
		if (j < 2)
			break;
	}

	if (block >= priv->n_pie_blocks) {
		mutex_unlock(&priv->pie_mutex);
		return -EOPNOTSUPP;
	}

	pr_debug("Using block: %d, index %d, template-id %d\n", block, idx, j);
	set_bit(idx, priv->pie_use_bm);

	pr->valid = true;
	pr->tid = j;  // Mapped to template number
	pr->tid_m = 0x1;
	pr->id = idx;

	rtl930x_pie_lookup_enable(priv, idx);
	rtl930x_pie_rule_write(priv, idx, pr);

	mutex_unlock(&priv->pie_mutex);
	return 0;
}

/*
 * Delete a range of Packet Inspection Engine rules
 */
static int rtl930x_pie_rule_del(struct rtl838x_switch_priv *priv, int index_from, int index_to)
{
	u32 v = (index_from << 1)| (index_to << 12 ) | BIT(0);

	pr_debug("%s: from %d to %d\n", __func__, index_from, index_to);
	mutex_lock(&priv->reg_mutex);

	// Write from-to and execute bit into control register
	sw_w32(v, RTL930X_PIE_CLR_CTRL);

	// Wait until command has completed
	do {
	} while (sw_r32(RTL930X_PIE_CLR_CTRL) & BIT(0));

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static void rtl930x_pie_rule_rm(struct rtl838x_switch_priv *priv, struct pie_rule *pr)
{
	int idx = pr->id;

	rtl930x_pie_rule_del(priv, idx, idx);
	clear_bit(idx, priv->pie_use_bm);
}

static void rtl930x_pie_init(struct rtl838x_switch_priv *priv)
{
	int i;
	u32 template_selectors;

	mutex_init(&priv->pie_mutex);

	pr_info("%s\n", __func__);
	// Enable ACL lookup on all ports, including CPU_PORT
	for (i = 0; i <= priv->cpu_port; i++)
		sw_w32(1, RTL930X_ACL_PORT_LOOKUP_CTRL(i));

	// Include IPG in metering
	sw_w32_mask(0, 1, RTL930X_METER_GLB_CTRL);

	// Delete all present rules, block size is 128 on all SoC families
	rtl930x_pie_rule_del(priv, 0, priv->n_pie_blocks * 128 - 1);

	// Assign blocks 0-7 to VACL phase (bit = 0), blocks 8-15 to IACL (bit = 1)
	sw_w32(0xff00, RTL930X_PIE_BLK_PHASE_CTRL);
	
	// Enable predefined templates 0, 1 for first quarter of all blocks
	template_selectors = 0 | (1 << 4);
	for (i = 0; i < priv->n_pie_blocks / 4; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

	// Enable predefined templates 2, 3 for second quarter of all blocks
	template_selectors = 2 | (3 << 4);
	for (i = priv->n_pie_blocks / 4; i < priv->n_pie_blocks / 2; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

	// Enable predefined templates 0, 1 for third half of all blocks
	template_selectors = 0 | (1 << 4);
	for (i = priv->n_pie_blocks / 2; i < priv->n_pie_blocks * 3 / 4; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

	// Enable predefined templates 2, 3 for fourth quater of all blocks
	template_selectors = 2 | (3 << 4);
	for (i = priv->n_pie_blocks * 3 / 4; i < priv->n_pie_blocks; i++)
		sw_w32(template_selectors, RTL930X_PIE_BLK_TMPLTE_CTRL(i));

}

static u32 rtl930x_packet_cntr_read(int counter)
{
	u32 v;

	// Read LOG table (3) via register RTL9300_TBL_0
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 3);

	pr_debug("In %s, id %d\n", __func__, counter);
	rtl_table_read(r, counter / 2);

	pr_debug("Registers: %08x %08x\n",
		sw_r32(rtl_table_data(r, 0)), sw_r32(rtl_table_data(r, 1)));
	// The table has a size of 2 registers
	if (counter % 2)
		v = sw_r32(rtl_table_data(r, 0));
	else
		v = sw_r32(rtl_table_data(r, 1));

	rtl_table_release(r);

	return v;
}

static void rtl930x_packet_cntr_clear(int counter)
{
	// Access LOG table (3) via register RTL9300_TBL_0
	struct table_reg *r = rtl_table_get(RTL9300_TBL_0, 3);

	pr_info("In %s, id %d\n", __func__, counter);
	// The table has a size of 2 registers
	if (counter % 2)
		sw_w32(0, rtl_table_data(r, 0));
	else
		sw_w32(0, rtl_table_data(r, 1));

	rtl_table_write(r, counter / 2);

	rtl_table_release(r);
}

const struct rtl838x_reg rtl930x_reg = {
	.mask_port_reg_be = rtl838x_mask_port_reg,
	.set_port_reg_be = rtl838x_set_port_reg,
	.get_port_reg_be = rtl838x_get_port_reg,
	.mask_port_reg_le = rtl838x_mask_port_reg,
	.set_port_reg_le = rtl838x_set_port_reg,
	.get_port_reg_le = rtl838x_get_port_reg,
	.stat_port_rst = RTL930X_STAT_PORT_RST,
	.stat_rst = RTL930X_STAT_RST,
	.stat_port_std_mib = RTL930X_STAT_PORT_MIB_CNTR,
	.traffic_enable = rtl930x_traffic_enable,
	.traffic_disable = rtl930x_traffic_disable,
	.traffic_get = rtl930x_traffic_get,
	.traffic_set = rtl930x_traffic_set,
	.l2_ctrl_0 = RTL930X_L2_CTRL,
	.l2_ctrl_1 = RTL930X_L2_AGE_CTRL,
	.l2_port_aging_out = RTL930X_L2_PORT_AGE_CTRL,
	.smi_poll_ctrl = RTL930X_SMI_POLL_CTRL, // TODO: Difference to RTL9300_SMI_PRVTE_POLLING_CTRL
	.l2_tbl_flush_ctrl = RTL930X_L2_TBL_FLUSH_CTRL,
	.exec_tbl0_cmd = rtl930x_exec_tbl0_cmd,
	.exec_tbl1_cmd = rtl930x_exec_tbl1_cmd,
	.tbl_access_data_0 = rtl930x_tbl_access_data_0,
	.isr_glb_src = RTL930X_ISR_GLB,
	.isr_port_link_sts_chg = RTL930X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL930X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL930X_IMR_GLB,
	.vlan_tables_read = rtl930x_vlan_tables_read,
	.vlan_set_tagged = rtl930x_vlan_set_tagged,
	.vlan_set_untagged = rtl930x_vlan_set_untagged,
	.vlan_profile_dump = rtl930x_vlan_profile_dump,
	.vlan_profile_setup = rtl930x_vlan_profile_setup,
	.vlan_fwd_on_inner = rtl930x_vlan_fwd_on_inner,
	.stp_get = rtl930x_stp_get,
	.stp_set = rtl930x_stp_set,
	.mac_force_mode_ctrl = rtl930x_mac_force_mode_ctrl,
	.mac_port_ctrl = rtl930x_mac_port_ctrl,
	.l2_port_new_salrn = rtl930x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl930x_l2_port_new_sa_fwd,
	.mir_ctrl = RTL930X_MIR_CTRL,
	.mir_dpm = RTL930X_MIR_DPM_CTRL,
	.mir_spm = RTL930X_MIR_SPM_CTRL,
	.mac_link_sts = RTL930X_MAC_LINK_STS,
	.mac_link_dup_sts = RTL930X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts = rtl930x_mac_link_spd_sts,
	.mac_rx_pause_sts = RTL930X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts = RTL930X_MAC_TX_PAUSE_STS,
	.read_l2_entry_using_hash = rtl930x_read_l2_entry_using_hash,
	.write_l2_entry_using_hash = rtl930x_write_l2_entry_using_hash,
	.read_cam = rtl930x_read_cam,
	.write_cam = rtl930x_write_cam,
	.vlan_port_egr_filter = RTL930X_VLAN_PORT_EGR_FLTR,
	.vlan_port_igr_filter = RTL930X_VLAN_PORT_IGR_FLTR(0),
	.vlan_port_pb = RTL930X_VLAN_PORT_PB_VLAN,
	.vlan_port_tag_sts_ctrl = RTL930X_VLAN_PORT_TAG_STS_CTRL,
	.trk_mbr_ctr = rtl930x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL930X_RMA_BPDU_FLD_PMSK,
	.init_eee = rtl930x_init_eee,
	.port_eee_set = rtl930x_port_eee_set,
	.eee_port_ability = rtl930x_eee_port_ability,
	.read_mcast_pmask = rtl930x_read_mcast_pmask,
	.write_mcast_pmask = rtl930x_write_mcast_pmask,
	.pie_init = rtl930x_pie_init,
	.pie_rule_write = rtl930x_pie_rule_write,
	.pie_rule_add = rtl930x_pie_rule_add,
	.pie_rule_rm = rtl930x_pie_rule_rm,
	.packet_cntr_read = rtl930x_packet_cntr_read,
	.packet_cntr_clear = rtl930x_packet_cntr_clear,
};
