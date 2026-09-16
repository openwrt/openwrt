// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>
#include <linux/inetdevice.h>

#include "l3.h"
#include "pie.h"
#include "qos.h"
#include "rtl-otto.h"
#include "tc.h"
#include "vlan.h"

#define RTL930X_LED_GLB_ACTIVE_LOW				BIT(22)
#define RTL930X_LED_CLK_SEL_MASK				GENMASK(17, 16)
#define RTL930X_LED_CLK_SEL_800NS				0
#define RTL930X_LED_CLK_SEL_400NS				1
#define RTL930X_LED_CLK_SEL_200NS				2
#define RTL930X_LED_CLK_SEL_100NS				3

#define RTL930X_LED_SETX_0_CTRL(x) (RTL930X_LED_SET0_0_CTRL - (x * 8))
#define RTL930X_LED_SETX_1_CTRL(x) (RTL930X_LED_SETX_0_CTRL(x) - 4)

/* get register for given set and led in the set */
#define RTL930X_LED_SETX_LEDY(x, y) (RTL930X_LED_SETX_0_CTRL(x) - 4 * (y / 2))

/* get shift for given led in any set */
#define RTL930X_LED_SET_LEDX_SHIFT(x) (16 * (x % 2))

const struct rtldsa_mib_list_item rtldsa_930x_mib_list[] = {
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_REG_STD, 0xbc, 1)),
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_REG_STD, 0xb8, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_REG_STD, 0x90, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_REG_STD, 0x8c, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_REG_STD, 0x88, 1)),
	MIB_LIST_ITEM("tx_CRCAlignErrors", MIB_ITEM(MIB_REG_STD, 0x84, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_REG_STD, 0x7c, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_REG_STD, 0x74, 1)),
	MIB_LIST_ITEM("tx_Fragments", MIB_ITEM(MIB_REG_STD, 0x6c, 1)),
	MIB_LIST_ITEM("tx_Jabbers", MIB_ITEM(MIB_REG_STD, 0x64, 1)),
	MIB_LIST_ITEM("tx_Collisions", MIB_ITEM(MIB_REG_STD, 0x5c, 1)),
	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_REG_PRV, 0x7c, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_REG_PRV, 0x68, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet1", MIB_ITEM(MIB_REG_PRV, 0x64, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_REG_PRV, 0x60, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet1", MIB_ITEM(MIB_REG_PRV, 0x5c, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_REG_PRV, 0x58, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsSet0", MIB_ITEM(MIB_REG_PRV, 0x54, 1)),
	MIB_LIST_ITEM("tx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_REG_PRV, 0x50, 1)),
	MIB_LIST_ITEM("rx_PktsFlexibleOctetsCRCSet0", MIB_ITEM(MIB_REG_PRV, 0x4c, 1)),
	MIB_LIST_ITEM("LengthFieldError", MIB_ITEM(MIB_REG_PRV, 0x48, 1)),
	MIB_LIST_ITEM("FalseCarrierTimes", MIB_ITEM(MIB_REG_PRV, 0x44, 1)),
	MIB_LIST_ITEM("UndersizeOctets", MIB_ITEM(MIB_REG_PRV, 0x40, 1)),
	MIB_LIST_ITEM("FramingErrors", MIB_ITEM(MIB_REG_PRV, 0x3c, 1)),
	MIB_LIST_ITEM("ParserErrors", MIB_ITEM(MIB_REG_PRV, 0x38, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_REG_PRV, 0x34, 1)),
	MIB_LIST_ITEM("rx_MacIPGShortDrop", MIB_ITEM(MIB_REG_PRV, 0x30, 1))
};

const struct rtldsa_mib_desc rtldsa_930x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_REG_STD, 0xa0, 1),

	.if_in_octets = MIB_ITEM(MIB_REG_STD, 0xf8, 2),
	.if_out_octets = MIB_ITEM(MIB_REG_STD, 0xf0, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xe8, 2),
	.if_in_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe0, 2),
	.if_in_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd8, 2),
	.if_out_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xd0, 2),
	.if_out_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xc8, 2),
	.if_out_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xc0, 2),
	.if_out_discards = MIB_ITEM(MIB_REG_STD, 0xbc, 1),
	.single_collisions = MIB_ITEM(MIB_REG_STD, 0xb4, 1),
	.multiple_collisions = MIB_ITEM(MIB_REG_STD, 0xb0, 1),
	.deferred_transmissions = MIB_ITEM(MIB_REG_STD, 0xac, 1),
	.late_collisions = MIB_ITEM(MIB_REG_STD, 0xa8, 1),
	.excessive_collisions = MIB_ITEM(MIB_REG_STD, 0xa4, 1),
	.crc_align_errors = MIB_ITEM(MIB_REG_STD, 0x80, 1),
	.rx_pkts_over_max_octets = MIB_ITEM(MIB_REG_PRV, 0x6c, 1),

	.unsupported_opcodes = MIB_ITEM(MIB_REG_STD, 0x9c, 1),

	.rx_undersize_pkts = MIB_ITEM(MIB_REG_STD, 0x78, 1),
	.rx_oversize_pkts = MIB_ITEM(MIB_REG_STD, 0x70, 1),
	.rx_fragments = MIB_ITEM(MIB_REG_STD, 0x68, 1),
	.rx_jabbers = MIB_ITEM(MIB_REG_STD, 0x60, 1),

	.tx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x58, 1),
		MIB_ITEM(MIB_REG_STD, 0x50, 1),
		MIB_ITEM(MIB_REG_STD, 0x48, 1),
		MIB_ITEM(MIB_REG_STD, 0x40, 1),
		MIB_ITEM(MIB_REG_STD, 0x38, 1),
		MIB_ITEM(MIB_REG_STD, 0x30, 1),
		MIB_ITEM(MIB_REG_PRV, 0x78, 1),
		MIB_ITEM(MIB_REG_PRV, 0x70, 1)
	},
	.rx_pkts = {
		MIB_ITEM(MIB_REG_STD, 0x54, 1),
		MIB_ITEM(MIB_REG_STD, 0x4c, 1),
		MIB_ITEM(MIB_REG_STD, 0x44, 1),
		MIB_ITEM(MIB_REG_STD, 0x3c, 1),
		MIB_ITEM(MIB_REG_STD, 0x34, 1),
		MIB_ITEM(MIB_REG_STD, 0x2c, 1),
		MIB_ITEM(MIB_REG_PRV, 0x74, 1),
		MIB_ITEM(MIB_REG_PRV, 0x6c, 1),
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

	.drop_events = MIB_ITEM(MIB_REG_STD, 0x90, 1),
	.collisions = MIB_ITEM(MIB_REG_STD, 0x5c, 1),

	.rx_pause_frames = MIB_ITEM(MIB_REG_STD, 0x98, 1),
	.tx_pause_frames = MIB_ITEM(MIB_REG_STD, 0x94, 1),

	.list_count = ARRAY_SIZE(rtldsa_930x_mib_list),
	.list = rtldsa_930x_mib_list
};

void rtldsa_930x_print_matrix(void)
{
	int tbl = otto_table_acquire(RTL9300_TBL_PORT_ISO_CTRL);
	u32 v;

	for (int i = 0; i < 29; i++) {
		__otto_table_read(tbl, i, &v);
		pr_debug("> %08x\n", v);
	}
	otto_table_release(tbl);
}

static inline int rtl930x_l2_port_new_salrn(int p)
{
	return RTL930X_L2_PORT_SALRN(p);
}

static inline int rtl930x_l2_port_new_sa_fwd(int p)
{
	/* TODO: The definition of the fields changed, because of the master-cpu in a stack */
	return RTL930X_L2_PORT_NEW_SA_FWD(p);
}

static int rtldsa_930x_get_mirror_config(struct rtldsa_mirror_config *config,
					 int group, int port)
{
	config->ctrl = RTL930X_MIR_CTRL + group * 4;
	config->spm = RTL930X_MIR_SPM_CTRL + group * 4;
	config->dpm = RTL930X_MIR_DPM_CTRL + group * 4;

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

static inline int rtl930x_trk_mbr_ctr(int group)
{
	return RTL930X_TRK_MBR_CTRL + (group << 2);
}

static void rtl930x_l2_learning_setup(void)
{
	/* Portmask for flooding broadcast traffic */
	sw_w32(RTL930X_MC_PMASK_ALL_PORTS, RTL930X_L2_BC_FLD_PMSK);

	/* Portmask for flooding unicast traffic with unknown destination */
	sw_w32(RTL930X_MC_PMASK_ALL_PORTS, RTL930X_L2_UNKN_UC_FLD_PMSK);

	/* Limit learning to maximum: 32k entries, after that just flood (bits 0-1) */
	sw_w32((0x7fff << 2) | 0, RTL930X_L2_LRN_CONSTRT_CTRL);
}

static void rtldsa_930x_enable_learning(int port, bool enable)
{
	/* Limit learning to maximum: 32k entries */
	sw_w32_mask(GENMASK(17, 3), enable ? (0x7ffe << 3) : 0,
		    RTL930X_L2_LRN_PORT_CONSTRT_CTRL + port * 4);
}

static void rtldsa_930x_l2_port_new_sa_fwd(int port, enum rtldsa_flood_type mode)
{
	u32 new_sa_fwd_shift = (port % 10) * 3;

	sw_w32_mask(GENMASK(new_sa_fwd_shift + 2, new_sa_fwd_shift),
		    mode << new_sa_fwd_shift,
		    rtl930x_l2_port_new_sa_fwd(port));
}

static void rtldsa_930x_enable_flood(int port, enum rtldsa_flood_type mode)
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

static void rtldsa_930x_enable_bcast_flood(int port, bool enable)
{
	u32 port_mask = BIT(port);

	sw_w32_mask(port_mask,
		    enable ? port_mask : 0,
		    RTL930X_L2_BC_FLD_PMSK);
}

static void rtldsa_930x_lag_set_port2group(int group, int port, bool valid)
{
	int tbl = otto_table_acquire(RTL9300_TBL_SRC_TRK_MAP);
	u32 mask = valid ? RTL930X_SRC_TRK_MAP_TRK_VALID : 0;

	mask |= FIELD_PREP(RTL930X_SRC_TRK_MAP_TRK_ID, group); // Update TRK Field.
	__otto_table_write(tbl, port, &mask);
	otto_table_release(tbl);
}

/* Write data from the data buffer into the lag-entry strucure */
static void rtldsa_930x_lag_fill_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	e->num_tx_candi = FIELD_GET(RTL930X_LAG_NUM_TX_CANDI, data[0]);
	e->l2_hash_mask_idx = FIELD_GET(RTL930X_LAG_L2_HASH_MSK_IDX, data[0]);
	e->ip4_hash_mask_idx = FIELD_GET(RTL930X_LAG_IP4_HASH_MSK_IDX, data[0]);
	e->ip6_hash_mask_idx = FIELD_GET(RTL930X_LAG_IP6_HASH_MSK_IDX, data[0]);
	e->flood_dlf_bcast.sep_dlf_bcast_en = FIELD_GET(RTL930X_LAG_SEP_DLF_BCAST_EN, data[0]);
	e->sep_kwn_mc_en = FIELD_GET(RTL930X_LAG_SEP_KWN_MC_EN, data[0]);
	e->trk_dev7 = FIELD_GET(RTL930X_LAG_TRK_DEV7, data[0]);
	e->trk_port7 = FIELD_GET(RTL930X_LAG_TRK_PORT7, data[0]);
	e->trk_dev6 = FIELD_GET(RTL930X_LAG_TRK_DEV6, data[0]);
	e->trk_port6 = FIELD_GET(RTL930X_LAG_TRK_PORT6, data[0]);

	/* 63-32 */
	e->trk_dev5 = FIELD_GET(RTL930X_LAG_TRK_DEV5, data[1]);
	e->trk_port5 = FIELD_GET(RTL930X_LAG_TRK_PORT5, data[1]);
	e->trk_dev4 = FIELD_GET(RTL930X_LAG_TRK_DEV4, data[1]);
	e->trk_port4 = FIELD_GET(RTL930X_LAG_TRK_PORT4, data[1]);
	e->trk_dev3 = FIELD_GET(RTL930X_LAG_TRK_DEV3, data[1]);
	e->trk_port3 = FIELD_GET(RTL930X_LAG_TRK_PORT3, data[1]);

	/* 31-0 */
	e->trk_dev2 = FIELD_GET(RTL930X_LAG_TRK_DEV2, data[2]);
	e->trk_port2 = FIELD_GET(RTL930X_LAG_TRK_PORT2, data[2]);
	e->trk_dev1 = FIELD_GET(RTL930X_LAG_TRK_DEV1, data[2]);
	e->trk_port1 = FIELD_GET(RTL930X_LAG_TRK_PORT1, data[2]);
	e->trk_dev0 = FIELD_GET(RTL930X_LAG_TRK_DEV0, data[2]);
	e->trk_port0 = FIELD_GET(RTL930X_LAG_TRK_PORT0, data[2]);
}

/* Write lag-entry data into buffer */
static void rtldsa_930x_lag_write_data(u32 data[], struct rtldsa_93xx_lag_entry *e)
{
	/* 95-64 */
	data[0] = FIELD_PREP(RTL930X_LAG_NUM_TX_CANDI, e->num_tx_candi);
	data[0] |= FIELD_PREP(RTL930X_LAG_L2_HASH_MSK_IDX, e->l2_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL930X_LAG_IP4_HASH_MSK_IDX, e->ip4_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL930X_LAG_IP6_HASH_MSK_IDX, e->ip6_hash_mask_idx);
	data[0] |= FIELD_PREP(RTL930X_LAG_SEP_DLF_BCAST_EN, e->flood_dlf_bcast.sep_dlf_bcast_en);
	data[0] |= FIELD_PREP(RTL930X_LAG_SEP_KWN_MC_EN, e->sep_kwn_mc_en);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_DEV7, e->trk_dev7);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_PORT7, e->trk_port7);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_DEV6, e->trk_dev6);
	data[0] |= FIELD_PREP(RTL930X_LAG_TRK_PORT6, e->trk_port6);

	/* 63-32 */
	data[1] = FIELD_PREP(RTL930X_LAG_TRK_DEV5, e->trk_dev5);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_PORT5, e->trk_port5);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_DEV4, e->trk_dev4);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_PORT4, e->trk_port4);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_DEV3, e->trk_dev3);
	data[1] |= FIELD_PREP(RTL930X_LAG_TRK_PORT3, e->trk_port3);

	/* 31-0 */
	data[2] = FIELD_PREP(RTL930X_LAG_TRK_DEV2, e->trk_dev2);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_PORT2, e->trk_port2);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_DEV1, e->trk_dev1);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_PORT1, e->trk_port1);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_DEV0, e->trk_dev0);
	data[2] |= FIELD_PREP(RTL930X_LAG_TRK_PORT0, e->trk_port0);
}

static void rtldsa_930x_lag_set_local_group_id(int local_group, int global_group, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL930X_TRK_ID_CTRL_TRK_VALID : 0;
	mask |= FIELD_PREP(RTL930X_TRK_ID_CTRL_TRK_ID, global_group);
	sw_w32(mask, RTL930X_TRK_ID_CTRL + (4 * local_group));
}

static void rtldsa_930x_lag_set_local_port2group(int group, int port, bool valid)
{
	u32 mask = 0;

	mask |= valid ? RTL930X_LOCAL_PORT_TRK_MAP_IS_TRK_MBR : 0;
	mask |= FIELD_PREP(RTL930X_LOCAL_PORT_TRK_MAP_TRK_ID, group);
	sw_w32(mask, RTL930X_LOCAL_PORT_TRK_MAP + (4 * port));
}

static void rtldsa_930x_lag_sync_tables(void)
{
	u32 val;
	int ret;

	sw_w32(BIT(0), RTL930X_TRK_LOCAL_TBL_REFRESH);

	ret = readx_poll_timeout(sw_r32, RTL930X_TRK_LOCAL_TBL_REFRESH, val,
				 !(val & BIT(0)), 20, 10000);
	if (ret)
		pr_err("%s: timeout\n", __func__);
}

static int rtldsa_930x_lag_table(void)
{
	return otto_table_acquire(RTL9300_TBL_LAG);
}

static int rtldsa_930x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 1 - ((port + 3) / 16);
	int bit = 2 * ((port + 3) % 16);
	/* port ranges 0..RTL930X_CPU_PORT (28), so idx is always 0 or 1 */
	u32 buf[2];
	int state;

	otto_table_read(RTL9300_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

static void rtl930x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL9300_TBL_MSTI);
	int idx = 1 - ((port + 3) / 16);
	int bit = 2 * ((port + 3) % 16);
	/* port ranges 0..RTL930X_CPU_PORT (28), so idx is always 0 or 1 */
	u32 buf[2];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

static inline int rtl930x_mac_force_mode_ctrl(int p)
{
	return RTL930X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl930x_mac_port_ctrl(int p)
{
	return RTL930X_MAC_L2_PORT_CTRL(p);
}

static inline int rtl930x_mac_max_len_reg(int p)
{
	return RTL930X_MAC_L2_PORT_MAX_LEN_CTRL(p);
}

static u64 rtl930x_l2_hash_seed(u64 mac, u32 vid)
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
static u32 rtl930x_l2_hash_key(struct rtl838x_switch_priv *priv, u64 seed)
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
static u64 rtl930x_read_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
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

static void rtl930x_write_l2_entry_using_hash(u32 hash, u32 pos, struct rtl838x_l2_entry *e)
{
	u32 r[3];
	u32 idx = (0 << 14) | (hash << 2) | pos; /* Access SRAM, with hash and at pos in bucket */

	pr_debug("%s: hash %d, pos %d\n", __func__, hash, pos);
	pr_debug("%s: index %d -> mac %02x:%02x:%02x:%02x:%02x:%02x\n", __func__, idx,
		 e->mac[0], e->mac[1], e->mac[2], e->mac[3], e->mac[4], e->mac[5]);

	rtl930x_fill_l2_row(r, e);

	otto_table_write(RTL9300_TBL_L2_UC, idx, &r);
}

static u64 rtl930x_read_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	otto_table_read(RTL9300_TBL_L2_CAM_UC, idx, &r);

	rtl930x_fill_l2_entry(r, e);
	if (!e->valid)
		return 0;

	/* return mac with concatenated vid as unique id */
	return ((u64)r[0] << 28) | ((r[1] & 0xffff0000) >> 4) | e->vid;
}

static void rtl930x_write_cam(int idx, struct rtl838x_l2_entry *e)
{
	u32 r[3];

	rtl930x_fill_l2_row(r, e);

	/* Access L2 Table 1 */
	otto_table_write(RTL9300_TBL_L2_CAM_UC, idx, &r);
}

static u64 rtl930x_read_mcast_pmask(int idx)
{
	u32 portmask;

	otto_table_read(RTL9300_TBL_MC_PORTMASK, idx, &portmask);
	portmask >>= 3;

	pr_debug("%s: Index idx %d has portmask %08x\n", __func__, idx, portmask);

	return portmask;
}

static void rtl930x_write_mcast_pmask(int idx, u64 portmask)
{
	u32 pm = portmask;

	pr_debug("%s: Index idx %d has portmask %08x\n", __func__, idx, pm);
	pm <<= 3;

	otto_table_write(RTL9300_TBL_MC_PORTMASK, idx, &pm);
}

static void rtldsa_930x_set_receive_management_action(int port, rma_ctrl_t type,
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
		reg = RTL930X_RMA_BPDU_CTRL + (port / 10) * 4;
		shift = (port % 10) * 3;
		sw_w32_mask(GENMASK(shift + 2, shift), value << shift, reg);
		break;
	case PTP:
		reg = RTL930X_RMA_PTP_CTRL + port * 4;

		/* udp */
		sw_w32_mask(GENMASK(3, 2), value << 2, reg);

		/* eth2 */
		sw_w32_mask(GENMASK(1, 0), value, reg);
		break;
	case PTP_UDP:
		reg = RTL930X_RMA_PTP_CTRL + port * 4;
		sw_w32_mask(GENMASK(3, 2), value << 2, reg);
		break;
	case PTP_ETH2:
		reg = RTL930X_RMA_PTP_CTRL + port * 4;
		sw_w32_mask(GENMASK(1, 0), value, reg);
		break;
	case LLDP:
		reg = RTL930X_RMA_LLDP_CTRL + (port / 10) * 4;
		shift = (port % 10) * 3;
		sw_w32_mask(GENMASK(shift + 2, shift), value << shift, reg);
		break;
	case EAPOL:
		reg = RTL930X_RMA_EAPOL_CTRL + (port / 10) * 4;
		shift = (port % 10) * 3;
		sw_w32_mask(GENMASK(shift + 2, shift), value << shift, reg);
		break;
	case GRATARP:
		reg = RTL930X_SPCL_TRAP_PORT_CTRL + (port / 16) * 4;
		shift = (port % 16) * 2;
		sw_w32_mask(GENMASK(shift + 1, shift), value << shift, reg);
		break;
	}
}

/* Enable traffic between a source port and a destination port matrix */
static void rtl930x_traffic_set(int source, u64 dest_matrix)
{
	u32 v = dest_matrix << 3;

	otto_table_write(RTL9300_TBL_PORT_ISO_CTRL, source, &v);
}

static void rtl930x_traffic_enable(int source, int dest)
{
	int tbl = otto_table_acquire(RTL9300_TBL_PORT_ISO_CTRL);
	u32 v;

	__otto_table_read(tbl, source, &v);
	v |= BIT(dest + 3);
	__otto_table_write(tbl, source, &v);
	otto_table_release(tbl);
}

static void rtl930x_traffic_disable(int source, int dest)
{
	int tbl = otto_table_acquire(RTL9300_TBL_PORT_ISO_CTRL);
	u32 v;

	__otto_table_read(tbl, source, &v);
	v &= ~BIT(dest + 3);
	__otto_table_write(tbl, source, &v);
	otto_table_release(tbl);
}

void rtl9300_dump_debug(void)
{
	u16 r = RTL930X_STAT_PRVTE_DROP_COUNTER0;

	for (int i = 0; i < 10; i++) {
		pr_debug("# %d %08x %08x %08x %08x %08x %08x %08x %08x\n", i * 8,
			 sw_r32(r), sw_r32(r + 4), sw_r32(r + 8), sw_r32(r + 12),
			 sw_r32(r + 16), sw_r32(r + 20), sw_r32(r + 24), sw_r32(r + 28));
		r += 32;
	}
	pr_debug("# %08x %08x %08x %08x %08x\n",
		 sw_r32(r), sw_r32(r + 4), sw_r32(r + 8), sw_r32(r + 12), sw_r32(r + 16));
	rtldsa_930x_print_matrix();
	pr_debug("RTL930X_L2_PORT_SABLK_CTRL: %08x, RTL930X_L2_PORT_DABLK_CTRL %08x\n",
		 sw_r32(RTL930X_L2_PORT_SABLK_CTRL), sw_r32(RTL930X_L2_PORT_DABLK_CTRL)

	);
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

/* Enables or disables the EEE/EEEP capability of a port */
static void rtldsa_930x_set_mac_eee(struct rtl838x_switch_priv *priv, int port, bool enable)
{
	u32 v;

	/* This works only for Ethernet ports, and on the RTL930X, ports from 26 are SFP */
	if (port >= 26)
		return;

	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0x3f : 0x0;

	/* Set EEE/EEEP state for 100, 500, 1000MBit and 2.5, 5 and 10GBit */
	sw_w32_mask(0, v << 10, rtl930x_mac_force_mode_ctrl(port));

	/* Set TX/RX EEE state */
	v = enable ? 0x3 : 0x0;
	sw_w32(v, RTL930X_EEE_CTRL(port));

	priv->ports[port].eee_enabled = enable;
}

static void rtl930x_init_eee(struct rtl838x_switch_priv *priv, bool enable)
{
	pr_debug("Setting up EEE, state: %d\n", enable);

	/* Setup EEE on all ports */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy)
			priv->r->set_mac_eee(priv, i, enable);
	}

	priv->eee_enabled = enable;
}

#ifdef CONFIG_NET_DSA_RTL83XX_RTL930X_L3_OFFLOAD

// Currently not used
// static u32 rtl930x_l3_hash6(struct in6_addr *ip6, int algorithm, bool move_dip)
// {
// 	u32 rows[16];
// 	u32 hash;
// 	u32 s0, s1, pH;

// 	rows[0] = (HASH_PICK(ip6->s6_addr[0], 6, 2) << 0);
// 	rows[1] = (HASH_PICK(ip6->s6_addr[0], 0, 6) << 3) | HASH_PICK(ip6->s6_addr[1], 5, 3);
// 	rows[2] = (HASH_PICK(ip6->s6_addr[1], 0, 5) << 4) | HASH_PICK(ip6->s6_addr[2], 4, 4);
// 	rows[3] = (HASH_PICK(ip6->s6_addr[2], 0, 4) << 5) | HASH_PICK(ip6->s6_addr[3], 3, 5);
// 	rows[4] = (HASH_PICK(ip6->s6_addr[3], 0, 3) << 6) | HASH_PICK(ip6->s6_addr[4], 2, 6);
// 	rows[5] = (HASH_PICK(ip6->s6_addr[4], 0, 2) << 7) | HASH_PICK(ip6->s6_addr[5], 1, 7);
// 	rows[6] = (HASH_PICK(ip6->s6_addr[5], 0, 1) << 8) | HASH_PICK(ip6->s6_addr[6], 0, 8);
// 	rows[7] = (HASH_PICK(ip6->s6_addr[7], 0, 8) << 1) | HASH_PICK(ip6->s6_addr[8], 7, 1);
// 	rows[8] = (HASH_PICK(ip6->s6_addr[8], 0, 7) << 2) | HASH_PICK(ip6->s6_addr[9], 6, 2);
// 	rows[9] = (HASH_PICK(ip6->s6_addr[9], 0, 6) << 3) | HASH_PICK(ip6->s6_addr[10], 5, 3);
// 	rows[10] = (HASH_PICK(ip6->s6_addr[10], 0, 5) << 4) | HASH_PICK(ip6->s6_addr[11], 4, 4);
// 	if (!algorithm) {
// 		rows[11] = (HASH_PICK(ip6->s6_addr[11], 0, 4) << 5) |
// 			   (HASH_PICK(ip6->s6_addr[12], 3, 5) << 0);
// 		rows[12] = (HASH_PICK(ip6->s6_addr[12], 0, 3) << 6) |
// 			   (HASH_PICK(ip6->s6_addr[13], 2, 6) << 0);
// 		rows[13] = (HASH_PICK(ip6->s6_addr[13], 0, 2) << 7) |
// 			   (HASH_PICK(ip6->s6_addr[14], 1, 7) << 0);
// 		if (!move_dip) {
// 			rows[14] = (HASH_PICK(ip6->s6_addr[14], 0, 1) << 8) |
// 				   (HASH_PICK(ip6->s6_addr[15], 0, 8) << 0);
// 		}
// 		hash = rows[0] ^ rows[1] ^ rows[2] ^ rows[3] ^ rows[4] ^
// 		       rows[5] ^ rows[6] ^ rows[7] ^ rows[8] ^ rows[9] ^
// 		       rows[10] ^ rows[11] ^ rows[12] ^ rows[13] ^ rows[14];
// 	} else {
// 		rows[11] = (HASH_PICK(ip6->s6_addr[11], 0, 4) << 5);
// 		rows[12] = (HASH_PICK(ip6->s6_addr[12], 3, 5) << 0);
// 		rows[13] = (HASH_PICK(ip6->s6_addr[12], 0, 3) << 6) |
// 			   HASH_PICK(ip6->s6_addr[13], 2, 6);
// 		rows[14] = (HASH_PICK(ip6->s6_addr[13], 0, 2) << 7) |
// 			   HASH_PICK(ip6->s6_addr[14], 1, 7);
// 		if (!move_dip) {
// 			rows[15] = (HASH_PICK(ip6->s6_addr[14], 0, 1) << 8) |
// 				   (HASH_PICK(ip6->s6_addr[15], 0, 8) << 0);
// 		}
// 		s0 = rows[12] + rows[13] + rows[14];
// 		s1 = (s0 & 0x1ff) + ((s0 & (0x1ff << 9)) >> 9);
// 		pH = (s1 & 0x1ff) + ((s1 & (0x1ff << 9)) >> 9);
// 		hash = rows[0] ^ rows[1] ^ rows[2] ^ rows[3] ^ rows[4] ^
// 		       rows[5] ^ rows[6] ^ rows[7] ^ rows[8] ^ rows[9] ^
// 		       rows[10] ^ rows[11] ^ pH ^ rows[15];
// 	}
// 	return hash;
// }

// Currently not used
// static int rtl930x_l3_mtu_del(struct rtl838x_switch_priv *priv, int mtu)
// {
// 	int i;

// 	for (i = 0; i < MAX_INTF_MTUS; i++) {
// 		if (mtu == priv->intf_mtus[i])
// 			break;
// 	}
// 	if (i >= MAX_INTF_MTUS || !priv->intf_mtu_count[i]) {
// 		pr_err("%s: No MTU slot found for MTU: %d\n", __func__, mtu);
// 		return -EINVAL;
// 	}

// 	priv->intf_mtu_count[i]--;
// }

// Currently not used
// static int rtl930x_l3_mtu_add(struct rtl838x_switch_priv *priv, int mtu)
// {
// 	int i, free_mtu;
// 	int mtu_id;

// 	/* Try to find an existing mtu-value or a free slot */
// 	free_mtu = MAX_INTF_MTUS;
// 	for (i = 0; i < MAX_INTF_MTUS && priv->intf_mtus[i] != mtu; i++) {
// 		if ((!priv->intf_mtu_count[i]) && (free_mtu == MAX_INTF_MTUS))
// 			free_mtu = i;
// 	}
// 	i = (i < MAX_INTF_MTUS) ? i : free_mtu;
// 	if (i < MAX_INTF_MTUS) {
// 		mtu_id = i;
// 	} else {
// 		pr_err("%s: No free MTU slot available!\n", __func__);
// 		return -EINVAL;
// 	}

// 	priv->intf_mtus[i] = mtu;
// 	pr_debug("Writing MTU %d to slot %d\n", priv->intf_mtus[i], i);
// 	/* Set MTU-value of the slot TODO: distinguish between IPv4/IPv6 routes / slots */
// 	sw_w32_mask(0xffff << ((i % 2) * 16), priv->intf_mtus[i] << ((i % 2) * 16),
// 		    RTL930X_L3_IP_MTU_CTRL(i));
// 	sw_w32_mask(0xffff << ((i % 2) * 16), priv->intf_mtus[i] << ((i % 2) * 16),
// 		    RTL930X_L3_IP6_MTU_CTRL(i));

// 	priv->intf_mtu_count[i]++;

// 	return mtu_id;
// }

// Currently not used
// /* Creates an interface for a route by setting up the HW tables in the SoC */
// static int rtl930x_l3_intf_add(struct rtl838x_switch_priv *priv, struct rtl838x_l3_intf *intf)
// {
// 	int i, intf_id, mtu_id;
// 	/* number of MTU-values < 16384 */

// 	/* Use the same IPv6 mtu as the ip4 mtu for this route if unset */
// 	intf->ip6_mtu = intf->ip6_mtu ? intf->ip6_mtu : intf->ip4_mtu;

// 	mtu_id = rtl930x_l3_mtu_add(priv, intf->ip4_mtu);
// 	pr_debug("%s: added mtu %d with mtu-id %d\n", __func__, intf->ip4_mtu, mtu_id);
// 	if (mtu_id < 0)
// 		return -ENOSPC;
// 	intf->ip4_mtu_id = mtu_id;
// 	intf->ip6_mtu_id = mtu_id;

// 	for (i = 0; i < MAX_INTERFACES; i++) {
// 		if (!priv->interfaces[i])
// 			break;
// 	}
// 	if (i >= MAX_INTERFACES) {
// 		pr_err("%s: cannot find free interface entry\n", __func__);
// 		return -EINVAL;
// 	}
// 	intf_id = i;
// 	priv->interfaces[i] = kzalloc(sizeof(struct rtl838x_l3_intf), GFP_KERNEL);
// 	if (!priv->interfaces[i]) {
// 		pr_err("%s: no memory to allocate new interface\n", __func__);
// 		return -ENOMEM;
// 	}
// }

#endif /* CONFIG_NET_DSA_RTL83XX_RTL930X_L3_OFFLOAD */

/* A PIE rule logs its matched packets into the LOG table entry that carries
 * its own rule ID, in data word 1 - one entry per rule. Counters handed out
 * by rtldsa_packet_cntr_alloc() for L3 route statistics start above the PIE
 * rule ID range and pack two 32-bit counters into one 64-bit LOG entry.
 */
#define RTL930X_PIE_RULE_IDS	(rtldsa_930x_cfg.n_pie_blocks * PIE_BLOCK_SIZE)

static u32 rtl930x_packet_cntr_read(struct rtl838x_switch_priv *priv, int counter)
{
	u32 buf[2];
	u32 v;

	dev_dbg(priv->dev, "reading LOG packet counter %d\n", counter);

	if (counter < RTL930X_PIE_RULE_IDS) {
		otto_table_read(RTL9300_TBL_LOG, counter, &buf);
		v = buf[1];
	} else {
		/* Two counters share one entry, so the parity of counter picks
		 * which half of it to return.
		 */
		otto_table_read(RTL9300_TBL_LOG, counter / 2, &buf);
		v = counter % 2 ? buf[0] : buf[1];
	}

	dev_dbg(priv->dev, "LOG entry: %08x %08x\n", buf[0], buf[1]);

	return v;
}

static void rtl930x_packet_cntr_clear(struct rtl838x_switch_priv *priv, int counter)
{
	int tbl = otto_table_acquire(RTL9300_TBL_LOG);

	u32 v[2];

	dev_dbg(priv->dev, "clearing LOG packet counter %d\n", counter);

	/* Read-modify-write: for a route counter the other 32-bit counter in
	 * the same 64-bit entry must be preserved; for a PIE rule ID the entry
	 * is ours alone but the RMW is harmless.
	 */
	if (counter < RTL930X_PIE_RULE_IDS) {
		__otto_table_read(tbl, counter, &v);
		v[1] = 0;
		__otto_table_write(tbl, counter, &v);
	} else {
		__otto_table_read(tbl, counter / 2, &v);
		v[counter % 2 ? 0 : 1] = 0;
		__otto_table_write(tbl, counter / 2, &v);
	}

	otto_table_release(tbl);
}

static int rtldsa_930x_fast_age(struct rtl838x_switch_priv *priv, int port, int vid)
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

static int rtl930x_set_ageing_time(unsigned long msec)
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

static void rtl930x_set_igr_filter(int port,  enum igr_filter state)
{
	sw_w32_mask(0x3 << ((port & 0xf) << 1), state << ((port & 0xf) << 1),
		    RTL930X_VLAN_PORT_IGR_FLTR + (((port >> 4) << 2)));
}

static void rtl930x_set_egr_filter(int port,  enum egr_filter state)
{
	sw_w32_mask(0x1 << (port % 0x1D), state << (port % 0x1D),
		    RTL930X_VLAN_PORT_EGR_FLTR + (((port / 29) << 2)));
}

static void rtldsa_930x_led_get_forced(const struct device_node *node,
				       const u8 leds_in_set[4],
				       u8 forced_leds_per_port[RTL930X_CPU_PORT])
{
	DECLARE_BITMAP(mask, RTL930X_CPU_PORT);
	unsigned int port;
	char set_str[36];
	u32 pm;

	for (u8 set = 0; set < 4; set++) {
		snprintf(set_str, sizeof(set_str), "realtek,led-set%d-force-port-mask", set);
		if (of_property_read_u32(node, set_str, &pm))
			continue;

		bitmap_from_arr32(mask, &pm, RTL930X_CPU_PORT);

		for_each_set_bit(port, mask, RTL930X_CPU_PORT)
			forced_leds_per_port[port] = leds_in_set[set];
	}
}

static void rtl930x_led_init(struct rtl838x_switch_priv *priv)
{
	u8 forced_leds_per_port[RTL930X_CPU_PORT] = {};
	struct device_node *node;
	struct device *dev = priv->dev;
	u8 leds_in_set[4] = {};
	u32 led_mode = 1;
	u32 clk_freq;
	u32 pm = 0;
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
			clk_sel = RTL930X_LED_CLK_SEL_100NS;
			break;
		case 5000000:
			clk_sel = RTL930X_LED_CLK_SEL_200NS;
			break;
		case 1250000:
			clk_sel = RTL930X_LED_CLK_SEL_800NS;
			break;
		default:
			dev_warn(dev, "invalid LED clock frequency, falling back to default\n");
			fallthrough;
		case 2500000:
			clk_sel = RTL930X_LED_CLK_SEL_400NS;
			break;
		}

		sw_w32_mask(RTL930X_LED_CLK_SEL_MASK,
			    FIELD_PREP(RTL930X_LED_CLK_SEL_MASK, clk_sel), RTL930X_LED_GLB_CTRL);
	}

	for (int set = 0; set < 4; set++) {
		char set_name[16] = {0};
		u32 set_config[4];
		int leds_in_this_set = 0;

		/* Reset LED set configuration */
		sw_w32(0, RTL930X_LED_SETX_0_CTRL(set));
		sw_w32(0, RTL930X_LED_SETX_1_CTRL(set));

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
			sw_w32_mask(0xffff << RTL930X_LED_SET_LEDX_SHIFT(led),
				    (0xffff & set_config[i]) << RTL930X_LED_SET_LEDX_SHIFT(led),
				    RTL930X_LED_SETX_LEDY(set, led));
		}
	}

	rtldsa_930x_led_get_forced(node, leds_in_set, forced_leds_per_port);

	for (int i = 0; i < priv->r->cpu_port; i++) {
		int pos = (i << 1) % 32;
		u32 set;

		sw_w32_mask(0x3 << pos, 0, RTL930X_LED_PORT_FIB_SET_SEL_CTRL(i));
		sw_w32_mask(0x3 << pos, 0, RTL930X_LED_PORT_COPR_SET_SEL_CTRL(i));

		if (!priv->ports[i].phy && !priv->ports[i].has_pcs && !(forced_leds_per_port[i]))
			continue;

		if (forced_leds_per_port[i] > 0)
			priv->ports[i].leds_on_this_port = forced_leds_per_port[i];

		/* 0x0 = 1 led, 0x1 = 2 leds, 0x2 = 3 leds, 0x3 = 4 leds per port */
		sw_w32_mask(0x3 << pos, (priv->ports[i].leds_on_this_port - 1) << pos, RTL930X_LED_PORT_NUM_CTRL(i));

		pm |= BIT(i);

		set = priv->ports[i].led_set;
		sw_w32_mask(0, set << pos, RTL930X_LED_PORT_COPR_SET_SEL_CTRL(i));
		sw_w32_mask(0, set << pos, RTL930X_LED_PORT_FIB_SET_SEL_CTRL(i));
	}

	/* Set LED mode */
	of_property_read_u32(node, "realtek,led-mode", &led_mode);
	sw_w32_mask(0x3, led_mode & 0x3, RTL930X_LED_GLB_CTRL);

	/* Set LED active state */
	if (of_property_read_bool(node, "active-low"))
		sw_w32_mask(RTL930X_LED_GLB_ACTIVE_LOW, 0, RTL930X_LED_GLB_CTRL);
	else
		sw_w32_mask(0, RTL930X_LED_GLB_ACTIVE_LOW, RTL930X_LED_GLB_CTRL);

	/* Set port type masks */
	sw_w32(pm, RTL930X_LED_PORT_COPR_MASK_CTRL);
	sw_w32(pm, RTL930X_LED_PORT_FIB_MASK_CTRL);
	sw_w32(pm, RTL930X_LED_PORT_COMBO_MASK_CTRL);

	for (int i = 0; i < 24; i++)
		dev_dbg(dev, "%08x: %08x\n", 0xbb00cc00 + i * 4, sw_r32(0xcc00 + i * 4));
}

const struct rtldsa_config rtldsa_930x_cfg = {
	.switch_ops = &rtldsa_93xx_switch_ops,
	.phylink_mac_ops = &rtldsa_93xx_phylink_mac_ops,
	.spanning_tree_ctrl = RTL930X_ST_CTRL,
	.l2_bucket_size = 8,
	.n_mst = 64,
	.num_lag_ids = 16,
	.cpu_port = RTL930X_CPU_PORT,
	.fib_entries = 16384,
	.mask_port_reg_be = rtl838x_mask_port_reg,
	.set_port_reg_be = rtl838x_set_port_reg,
	.get_port_reg_be = rtl838x_get_port_reg,
	.mask_port_reg_le = rtl838x_mask_port_reg,
	.set_port_reg_le = rtl838x_set_port_reg,
	.get_port_reg_le = rtl838x_get_port_reg,
	.stat_port_rst = RTL930X_STAT_PORT_RST,
	.stat_rst = RTL930X_STAT_RST,
	.stat_port_std_mib = RTL930X_STAT_PORT_MIB_CNTR,
	.stat_port_prv_mib = RTL930X_STAT_PORT_PRVTE_CNTR,
	.mib_desc = &rtldsa_930x_mib_desc,
	.stat_counters_lock = rtldsa_counters_lock_register,
	.stat_counters_unlock = rtldsa_counters_unlock_register,
	.stat_update_counters_atomically = rtldsa_update_counters_atomically,
	.stat_counter_poll_interval = RTLDSA_COUNTERS_POLL_INTERVAL,
	.traffic_enable = rtl930x_traffic_enable,
	.traffic_disable = rtl930x_traffic_disable,
	.traffic_set = rtl930x_traffic_set,
	.l2_ctrl_0 = RTL930X_L2_CTRL,
	.l2_ctrl_1 = RTL930X_L2_AGE_CTRL,
	.l2_port_aging_out = RTL930X_L2_PORT_AGE_CTRL,
	.set_ageing_time = rtl930x_set_ageing_time,
	.l2_tbl_flush_ctrl = RTL930X_L2_TBL_FLUSH_CTRL,
	.isr_glb_src = RTL930X_ISR_GLB,
	.isr_port_link_sts_chg = RTL930X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL930X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL930X_IMR_GLB,
	.n_counters = 2048,
	.n_pie_blocks = 16,
	.pie_rule_id_is_log_counter = true,
	.port_ignore = 0x3f,
	.vlan_tables_read = rtl930x_vlan_tables_read,
	.vlan_set_tagged = rtl930x_vlan_set_tagged,
	.vlan_set_untagged = rtl930x_vlan_set_untagged,
	.vlan_profile_get = rtldsa_930x_vlan_profile_get,
	.vlan_profile_dump = rtldsa_930x_vlan_profile_dump,
	.vlan_profile_setup = rtl930x_vlan_profile_setup,
	.vlan_fwd_on_inner = rtl930x_vlan_fwd_on_inner,
	.set_vlan_igr_filter = rtl930x_set_igr_filter,
	.set_vlan_egr_filter = rtl930x_set_egr_filter,
	.stp_get = rtldsa_930x_stp_get,
	.stp_set = rtl930x_stp_set,
	.mac_link_sts = RTL930X_MAC_LINK_STS,
	.mac_force_mode_mask = RTL930X_FORCE_EN | RTL930X_FORCE_LINK_EN,
	.mac_force_mode_ctrl = rtl930x_mac_force_mode_ctrl,
	.mac_port_ctrl = rtl930x_mac_port_ctrl,
	.mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 | MAC_100 |
			    MAC_1000FD | MAC_2500FD | MAC_5000FD | MAC_10000FD,
	.mac_max_len_reg = rtl930x_mac_max_len_reg,
	.max_frame = RTL930X_MAX_FRAME,
	.l2_port_new_salrn = rtl930x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl930x_l2_port_new_sa_fwd,
	.get_mirror_config = rtldsa_930x_get_mirror_config,
	.port_rate_police_add = rtldsa_930x_port_rate_police_add,
	.port_rate_police_del = rtldsa_930x_port_rate_police_del,
	.print_matrix = rtldsa_930x_print_matrix,
	.read_l2_entry_using_hash = rtl930x_read_l2_entry_using_hash,
	.write_l2_entry_using_hash = rtl930x_write_l2_entry_using_hash,
	.read_cam = rtl930x_read_cam,
	.write_cam = rtl930x_write_cam,
	.vlan_port_keep_tag_set = rtl930x_vlan_port_keep_tag_set,
	.vlan_port_pvidmode_set = rtl930x_vlan_port_pvidmode_set,
	.vlan_port_pvid_set = rtl930x_vlan_port_pvid_set,
	.fast_age = rtldsa_930x_fast_age,
	.trk_mbr_ctr = rtl930x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL930X_RMA_BPDU_FLD_PMSK,
	.init_eee = rtl930x_init_eee,
	.set_mac_eee = rtldsa_930x_set_mac_eee,
	.l2_hash_seed = rtl930x_l2_hash_seed,
	.l2_hash_key = rtl930x_l2_hash_key,
	.read_mcast_pmask = rtl930x_read_mcast_pmask,
	.write_mcast_pmask = rtl930x_write_mcast_pmask,
	.pie_init = rtl930x_pie_init,
	.pie_rule_write = rtl930x_pie_rule_write,
	.pie_rule_add = rtl930x_pie_rule_add,
	.pie_rule_rm = rtl930x_pie_rule_rm,
	.l2_learning_setup = rtl930x_l2_learning_setup,
	.packet_cntr_read = rtl930x_packet_cntr_read,
	.packet_cntr_clear = rtl930x_packet_cntr_clear,
	.led_init = rtl930x_led_init,
	.enable_learning = rtldsa_930x_enable_learning,
	.enable_l2_new_sa_fwd = rtldsa_930x_l2_port_new_sa_fwd,
	.enable_flood = rtldsa_930x_enable_flood,
	.enable_bcast_flood = rtldsa_930x_enable_bcast_flood,
	.set_receive_management_action = rtldsa_930x_set_receive_management_action,
	.qos_init = rtldsa_930x_qos_init,
	.trk_ctrl = RTL930X_TRK_CTRL,
	.trk_hash_ctrl = RTL930X_TRK_HASH_CTRL,
	.prepare_lag_fdb = rtldsa_93xx_prepare_lag_fdb,
	.lag_switch_init = rtldsa_93xx_lag_switch_init,
	.lag_set_port_members = rtldsa_93xx_lag_set_port_members,
	.lag_set_distribution_algorithm = rtldsa_93xx_lag_set_distribution_algorithm,
	.lag_set_local_group_id = rtldsa_930x_lag_set_local_group_id,
	.lag_write_data = rtldsa_930x_lag_write_data,
	.lag_fill_data = rtldsa_930x_lag_fill_data,
	.lag_set_local_port2group = rtldsa_930x_lag_set_local_port2group,
	.lag_set_port2group = rtldsa_930x_lag_set_port2group,
	.lag_sync_tables = rtldsa_930x_lag_sync_tables,
	.lag_table = rtldsa_930x_lag_table,
};
