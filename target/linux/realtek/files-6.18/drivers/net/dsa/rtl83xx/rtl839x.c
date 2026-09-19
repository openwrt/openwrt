// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>

#include "lag.h"
#include "l2.h"
#include "l3.h"
#include "pie.h"
#include "qos.h"
#include "rtl-otto.h"
#include "stats.h"
#include "vlan.h"

const struct rtldsa_mib_list_item rtldsa_839x_mib_list[] = {
	MIB_LIST_ITEM("ifOutDiscards", MIB_ITEM(MIB_REG_STD, 0xd4, 1)),
	MIB_LIST_ITEM("dot1dTpPortInDiscards", MIB_ITEM(MIB_REG_STD, 0xd0, 1)),
	MIB_LIST_ITEM("DropEvents", MIB_ITEM(MIB_REG_STD, 0xa8, 1)),
	MIB_LIST_ITEM("tx_BroadcastPkts", MIB_ITEM(MIB_REG_STD, 0xa4, 1)),
	MIB_LIST_ITEM("tx_MulticastPkts", MIB_ITEM(MIB_REG_STD, 0xa0, 1)),
	MIB_LIST_ITEM("tx_UndersizePkts", MIB_ITEM(MIB_REG_STD, 0x98, 1)),
	MIB_LIST_ITEM("rx_UndersizeDropPkts", MIB_ITEM(MIB_REG_STD, 0x90, 1)),
	MIB_LIST_ITEM("tx_OversizePkts", MIB_ITEM(MIB_REG_STD, 0x8c, 1)),
	MIB_LIST_ITEM("Collisions", MIB_ITEM(MIB_REG_STD, 0x7c, 1)),
	MIB_LIST_ITEM("rx_LengthFieldError", MIB_ITEM(MIB_REG_STD, 0x40, 1)),
	MIB_LIST_ITEM("rx_FalseCarrierTimes", MIB_ITEM(MIB_REG_STD, 0x3c, 1)),
	MIB_LIST_ITEM("rx_UnderSizeOctets", MIB_ITEM(MIB_REG_STD, 0x38, 1)),
	MIB_LIST_ITEM("tx_Fragments", MIB_ITEM(MIB_REG_STD, 0x34, 1)),
	MIB_LIST_ITEM("tx_Jabbers", MIB_ITEM(MIB_REG_STD, 0x30, 1)),
	MIB_LIST_ITEM("tx_CRCAlignErrors", MIB_ITEM(MIB_REG_STD, 0x2c, 1)),
	MIB_LIST_ITEM("rx_FramingErrors", MIB_ITEM(MIB_REG_STD, 0x28, 1)),
	MIB_LIST_ITEM("rx_MacDiscards", MIB_ITEM(MIB_REG_STD, 0x24, 1))
};

const struct rtldsa_mib_desc rtldsa_839x_mib_desc = {
	.symbol_errors = MIB_ITEM(MIB_REG_STD, 0xb8, 1),

	.if_in_octets = MIB_ITEM(MIB_REG_STD, 0xf8, 2),
	.if_out_octets = MIB_ITEM(MIB_REG_STD, 0xf0, 2),
	.if_in_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xec, 1),
	.if_in_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe8, 1),
	.if_in_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xe4, 1),
	.if_out_ucast_pkts = MIB_ITEM(MIB_REG_STD, 0xe0, 1),
	.if_out_mcast_pkts = MIB_ITEM(MIB_REG_STD, 0xdc, 1),
	.if_out_bcast_pkts = MIB_ITEM(MIB_REG_STD, 0xd8, 1),
	.if_out_discards = MIB_ITEM(MIB_REG_STD, 0xd4, 1),
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
		{ 1519, 12288 }
	},

	.drop_events = MIB_ITEM(MIB_REG_STD, 0xa8, 1),
	.collisions = MIB_ITEM(MIB_REG_STD, 0x7c, 1),

	.rx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xb0, 1),
	.tx_pause_frames = MIB_ITEM(MIB_REG_STD, 0xac, 1),

	.list_count = ARRAY_SIZE(rtldsa_839x_mib_list),
	.list = rtldsa_839x_mib_list
};

void rtldsa_839x_print_matrix(void)
{
	volatile u64 *ptr9;

	ptr9 = RTL838X_SW_BASE + RTL839X_PORT_ISO_CTRL(0);
	for (int i = 0; i < 52; i += 4)
		pr_debug("> %16llx %16llx %16llx %16llx\n",
			 ptr9[i + 0], ptr9[i + 1], ptr9[i + 2], ptr9[i + 3]);
	pr_debug("CPU_PORT> %16llx\n", ptr9[52]);
}

static inline int rtl839x_port_iso_ctrl(int p)
{
	return RTL839X_PORT_ISO_CTRL(p);
}

inline void rtl839x_exec_tbl2_cmd(u32 cmd)
{
	sw_w32(cmd, RTL839X_TBL_ACCESS_CTRL_2);
	do { } while (sw_r32(RTL839X_TBL_ACCESS_CTRL_2) & (1 << 9));
}

static inline int rtl839x_mac_force_mode_ctrl(int p)
{
	return RTL839X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl839x_mac_port_ctrl(int p)
{
	return RTL839X_MAC_PORT_CTRL(p);
}

static int rtldsa_839x_get_mirror_config(struct rtldsa_mirror_config *config,
					 int group, int port)
{
	config->ctrl = RTL839X_MIR_CTRL + group * 4;
	config->spm = RTL839X_MIR_SPM_CTRL + group * 8;
	config->dpm = RTL839X_MIR_DPM_CTRL + group * 8;

	/* Enable mirroring to destination port */
	config->val = BIT(0);
	config->val |= port << 4;

	return 0;
}

static void rtl839x_traffic_set(int source, u64 dest_matrix)
{
	rtl839x_set_port_reg_be(dest_matrix, rtl839x_port_iso_ctrl(source));
}

static void rtl839x_traffic_enable(int source, int dest)
{
	rtl839x_mask_port_reg_be(0, BIT_ULL(dest), rtl839x_port_iso_ctrl(source));
}

static void rtl839x_traffic_disable(int source, int dest)
{
	rtl839x_mask_port_reg_be(BIT_ULL(dest), 0, rtl839x_port_iso_ctrl(source));
}

static int rtldsa_839x_stp_get(struct rtl838x_switch_priv *priv, u16 msti, int port)
{
	int idx = 3 - ((port + 12) / 16);
	int bit = 2 * ((port + 12) % 16);
	/* port < priv->r->cpu_port (RTL839X_CPU_PORT == 52), so idx is 0..3 */
	u32 buf[4];
	int state;

	otto_table_read(RTL8390_TBL_MSTI, msti, &buf);
	state = (buf[idx] >> bit) & 0x3;

	return state;
}

static void rtl839x_stp_set(struct rtl838x_switch_priv *priv, u16 msti, int port, int state)
{
	int tbl = otto_table_acquire(RTL8390_TBL_MSTI);
	int idx = 3 - ((port + 12) / 16);
	int bit = 2 * ((port + 12) % 16);
	/* port < priv->r->cpu_port (RTL839X_CPU_PORT == 52), so idx is 0..3 */
	u32 buf[4];

	__otto_table_read(tbl, msti, &buf);
	buf[idx] = (buf[idx] & ~(0x3 << bit)) | (state << bit);
	__otto_table_write(tbl, msti, &buf);
	otto_table_release(tbl);
}

/* Enables or disables the EEE/EEEP capability of a port */
static void rtldsa_839x_set_mac_eee(struct rtl838x_switch_priv *priv, int port, bool enable)
{
	u32 v;

	/* This works only for Ethernet ports, and on the RTL839X, ports above 47 are SFP */
	if (port >= 48)
		return;

	enable = true;
	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0xf : 0x0;

	/* Set EEE for 100, 500, 1000MBit and 10GBit */
	sw_w32_mask(0xf << 8, v << 8, rtl839x_mac_force_mode_ctrl(port));

	/* Set TX/RX EEE state */
	v = enable ? 0x3 : 0x0;
	sw_w32(v, RTL839X_EEE_CTRL(port));

	priv->ports[port].eee_enabled = enable;
}

static void rtl839x_init_eee(struct rtl838x_switch_priv *priv, bool enable)
{
	pr_debug("Setting up EEE, state: %d\n", enable);

	/* Set wake timer for TX and pause timer both to 0x21 */
	sw_w32_mask(0xff << 20 | 0xff, 0x21 << 20 | 0x21, RTL839X_EEE_TX_TIMER_GELITE_CTRL);
	/* Set pause wake timer for GIGA-EEE to 0x11 */
	sw_w32_mask(0xff << 20, 0x11 << 20, RTL839X_EEE_TX_TIMER_GIGA_CTRL);
	/* Set pause wake timer for 10GBit ports to 0x11 */
	sw_w32_mask(0xff << 20, 0x11 << 20, RTL839X_EEE_TX_TIMER_10G_CTRL);

	/* Setup EEE on all ports */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy)
			priv->r->set_mac_eee(priv, i, enable);
	}
	priv->eee_enabled = enable;
}

static u32 rtl839x_packet_cntr_read(struct rtl838x_switch_priv *priv, int counter)
{
	u32 buf[2];
	u32 v;

	dev_dbg(priv->dev, "reading LOG packet counter %d\n", counter);
	otto_table_read(RTL8390_TBL_LOG, counter / 2, &buf);

	if (counter % 2)
		v = buf[0];
	else
		v = buf[1];

	return v;
}

static void rtl839x_packet_cntr_clear(struct rtl838x_switch_priv *priv, int counter)
{
	int tbl = otto_table_acquire(RTL8390_TBL_LOG);
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

static void rtl839x_set_igr_filter(int port,  enum igr_filter state)
{
	sw_w32_mask(0x3 << ((port & 0xf) << 1), state << ((port & 0xf) << 1),
		    RTL839X_VLAN_PORT_IGR_FLTR + (((port >> 4) << 2)));
}

static void rtl839x_set_egr_filter(int port,  enum egr_filter state)
{
	sw_w32_mask(0x1 << (port % 0x20), state << (port % 0x20),
		    RTL839X_VLAN_PORT_EGR_FLTR + (((port >> 5) << 2)));
}

static void rtl839x_set_receive_management_action(int port, rma_ctrl_t type, action_type_t action)
{
	switch (type) {
	case BPDU:
		sw_w32_mask(3 << ((port & 0xf) << 1), (action & 0x3) << ((port & 0xf) << 1),
			    RTL839X_RMA_BPDU_CTRL + ((port >> 4) << 2));
		break;
	case PTP:
		sw_w32_mask(3 << ((port & 0xf) << 1), (action & 0x3) << ((port & 0xf) << 1),
			    RTL839X_RMA_PTP_CTRL + ((port >> 4) << 2));
		break;
	case LLDP:
		sw_w32_mask(3 << ((port & 0xf) << 1), (action & 0x3) << ((port & 0xf) << 1),
			    RTL839X_RMA_LLDP_CTRL + ((port >> 4) << 2));
		break;
	default:
		break;
	}
}

const struct rtldsa_config rtldsa_839x_cfg = {
	.switch_ops = &rtldsa_83xx_switch_ops,
	.phylink_mac_ops = &rtldsa_83xx_phylink_mac_ops,
	.spanning_tree_ctrl = RTL839X_ST_CTRL,
	.l2_bucket_size = 4,
	.n_mst = 256,
	.num_lag_ids = 16,
	.cpu_port = RTL839X_CPU_PORT,
	.fib_entries = 16384,
	.mask_port_reg_be = rtl839x_mask_port_reg_be,
	.set_port_reg_be = rtl839x_set_port_reg_be,
	.get_port_reg_be = rtl839x_get_port_reg_be,
	.mask_port_reg_le = rtl839x_mask_port_reg_le,
	.set_port_reg_le = rtl839x_set_port_reg_le,
	.get_port_reg_le = rtl839x_get_port_reg_le,
	.stat_port_rst = RTL839X_STAT_PORT_RST,
	.stat_rst = RTL839X_STAT_RST,
	.stat_port_std_mib = RTL839X_STAT_PORT_STD_MIB,
	.mib_desc = &rtldsa_839x_mib_desc,
	.stat_counters_lock = rtldsa_counters_lock_register,
	.stat_counters_unlock = rtldsa_counters_unlock_register,
	.stat_update_counters_atomically = rtldsa_update_counters_atomically,
	.stat_counter_poll_interval = RTLDSA_COUNTERS_POLL_INTERVAL,
	.traffic_enable = rtl839x_traffic_enable,
	.traffic_disable = rtl839x_traffic_disable,
	.traffic_set = rtl839x_traffic_set,
	.port_iso_ctrl = rtl839x_port_iso_ctrl,
	.l2_ctrl_0 = RTL839X_L2_CTRL_0,
	.l2_ctrl_1 = RTL839X_L2_CTRL_1,
	.self_mac_trap_ctrl = RTL839X_SPCL_TRAP_SWITCH_MAC_CTRL,
	.l2_port_aging_out = RTL839X_L2_PORT_AGING_OUT,
	.set_ageing_time = rtl839x_set_ageing_time,
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
	.isr_glb_src = RTL839X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL839X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL839X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL839X_IMR_GLB,
	.n_counters = 1024,
	.n_pie_blocks = 18,
	.port_ignore = 0x3f,
	.vlan_tables_read = rtl839x_vlan_tables_read,
	.vlan_set_tagged = rtl839x_vlan_set_tagged,
	.vlan_set_untagged = rtl839x_vlan_set_untagged,
	.vlan_profile_get = rtldsa_839x_vlan_profile_get,
	.vlan_profile_dump = rtldsa_839x_vlan_profile_dump,
	.vlan_profile_setup = rtl839x_vlan_profile_setup,
	.vlan_fwd_on_inner = rtl839x_vlan_fwd_on_inner,
	.vlan_port_keep_tag_set = rtl839x_vlan_port_keep_tag_set,
	.vlan_port_pvidmode_set = rtl839x_vlan_port_pvidmode_set,
	.vlan_port_pvid_set = rtl839x_vlan_port_pvid_set,
	.set_vlan_igr_filter = rtl839x_set_igr_filter,
	.set_vlan_egr_filter = rtl839x_set_egr_filter,
	.enable_learning = rtl839x_enable_learning,
	.enable_flood = rtl839x_enable_flood,
	.enable_mcast_flood = rtl839x_enable_mcast_flood,
	.enable_bcast_flood = rtl839x_enable_bcast_flood,
	.set_static_move_action = rtl839x_set_static_move_action,
	.stp_get = rtldsa_839x_stp_get,
	.stp_set = rtl839x_stp_set,
	.mac_force_mode_mask = RTL83XX_FORCE_EN | RTL83XX_FORCE_LINK_EN,
	.mac_force_mode_ctrl = rtl839x_mac_force_mode_ctrl,
	.mac_link_sts = RTL839X_MAC_LINK_STS,
	.mac_port_ctrl = rtl839x_mac_port_ctrl,
	.mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 | MAC_100 | MAC_1000FD,
	.mac_max_len_ctrl = RTL839X_MAC_MAX_LEN_CTRL,
	.max_frame = RTL839X_MAX_FRAME,
	.l2_port_new_salrn = rtl839x_l2_port_new_salrn,
	.l2_port_new_sa_fwd = rtl839x_l2_port_new_sa_fwd,
	.get_mirror_config = rtldsa_839x_get_mirror_config,
	.print_matrix = rtldsa_839x_print_matrix,
	.read_l2_entry_using_hash = rtl839x_read_l2_entry_using_hash,
	.write_l2_entry_using_hash = rtl839x_write_l2_entry_using_hash,
	.read_cam = rtl839x_read_cam,
	.write_cam = rtl839x_write_cam,
	.fast_age = rtldsa_839x_fast_age,
	.trk_mbr_ctr = rtl839x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL839X_RMA_BPDU_FLD_PMSK,
	.spcl_trap_eapol_ctrl = RTL839X_SPCL_TRAP_EAPOL_CTRL,
	.init_eee = rtl839x_init_eee,
	.set_mac_eee = rtldsa_839x_set_mac_eee,
	.l2_hash_seed = rtl839x_l2_hash_seed,
	.l2_hash_key = rtl839x_l2_hash_key,
	.read_mcast_pmask = rtl839x_read_mcast_pmask,
	.write_mcast_pmask = rtl839x_write_mcast_pmask,
	.pie_init = rtl839x_pie_init,
	.pie_rule_read = rtl839x_pie_rule_read,
	.pie_rule_write = rtl839x_pie_rule_write,
	.pie_rule_add = rtl839x_pie_rule_add,
	.pie_rule_rm = rtl839x_pie_rule_rm,
	.l2_learning_setup = rtl839x_l2_learning_setup,
	.packet_cntr_read = rtl839x_packet_cntr_read,
	.packet_cntr_clear = rtl839x_packet_cntr_clear,
	.set_receive_management_action = rtl839x_set_receive_management_action,
	.get_egress_rate = rtldsa_839x_get_egress_rate,
	.set_egress_rate = rtldsa_839x_set_egress_rate,
	.qos_init = rtldsa_839x_qos_init,
	.lag_set_distribution_algorithm = rtldsa_839x_set_distribution_algorithm,
	.lag_set_port_members = rtldsa_839x_lag_set_port_members,
	.lag_setup_algomask = rtldsa_83xx_lag_setup_algomask,
};
