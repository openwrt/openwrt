// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>
#include <linux/iopoll.h>
#include <net/nexthop.h>

#include "lag.h"
#include "l2.h"
#include "l3.h"
#include "pie.h"
#include "qos.h"
#include "mirror.h"
#include "mac.h"
#include "rtl-otto.h"
#include "stats.h"
#include "vlan.h"
#include "stp.h"

/* Register definition */
#define RTL838X_MAC_PORT_CTRL(port)		(0xd560 + (((port) << 7)))

/* MAC maximum packet length (jumbo frame) control.
 *
 * The switch MAC drops frames whose L2 length exceeds the configured maximum.
 * A family holds either one register per user port or a single one for the
 * whole switch. The length is a direct byte value held in two 14-bit fields
 * (high-speed links in [13:0], 10/100M links in [27:14]); bit 28 selects
 * whether VLAN tag bytes count towards the limit.
 */

/* RTL838x and RTL839x hold one limit for the whole switch instead, bounding
 * the CPU port with it. RTL838x mirrors it in a second register, and the
 * vendor SDK writes both (dal_maple_switch_maxPktLenLinkSpeed_set()).
 */
#define RTL838X_MAC_MAX_LEN_CTRL		(0xa9e0)
#define RTL838X_MAC_MAX_LEN_CTRL_DUP		(0x6b00)

/* RTL838x stops at what its datasheet gives, below the vendor SDK value */
#define RTL838X_MAX_FRAME			10000

/* MAC handling */
#define RTL838X_MAC_LINK_STS			(0xa188)

#define RTL838X_EEE_PORT_TX_EN			(0x014c)
#define RTL838X_EEE_PORT_RX_EN			(0x0150)
#define RTL838X_EEE_TX_TIMER_GIGA_CTRL		(0xaa04)
#define RTL838X_EEE_TX_TIMER_GELITE_CTRL	(0xaa08)

/* L2 functionality */
#define RTL838X_L2_CTRL_0			(0x3200)

#define RTL838X_L2_TBL_FLUSH_CTRL		(0x3370)

/* 802.1X */
#define RTL838X_RMA_BPDU_FLD_PMSK		(0x4348)

#define RTL838X_SPCL_TRAP_EAPOL_CTRL		(0x6988)
#define RTL838X_SPCL_TRAP_SWITCH_MAC_CTRL	(0x6998)

/* Switch interrupts */
#define RTL838X_IMR_GLB				(0x1100)
#define RTL838X_IMR_PORT_LINK_STS_CHG		(0x1104)
#define RTL838X_ISR_GLB_SRC			(0x1148)
#define RTL838X_ISR_PORT_LINK_STS_CHG		(0x114C)

#define RTL838X_SMI_GLB_CTRL			(0xa100) /* used by RTL838x EEE setup */

#define RTL838X_RMA_BPDU_CTRL			(0x4330)

#define RTL838X_RMA_PTP_CTRL			(0x4338)

#define RTL838X_RMA_LLDP_CTRL			(0x4340)

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

static inline int rtl838x_mac_force_mode_ctrl(int p)
{
	return RTL838X_MAC_FORCE_MODE_CTRL + (p << 2);
}

static inline int rtl838x_mac_port_ctrl(int p)
{
	return RTL838X_MAC_PORT_CTRL(p);
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

static void rtldsa_838x_stat_init(struct rtl838x_switch_priv *priv)
{
	/* Enable statistics module: all counters plus debug */
	sw_w32_mask(0, 3, RTL838X_STAT_CTRL);
}

const struct rtldsa_config rtldsa_838x_cfg = {
	.switch_ops = &rtldsa_83xx_switch_ops,
	.phylink_mac_ops = &rtldsa_83xx_phylink_mac_ops,
	.stp_init = rtldsa_838x_stp_init,
	.l2_bucket_size = 4,
	.n_mst = 64,
	.num_lag_ids = 8,
	.cpu_port = RTL838X_CPU_PORT,
	.fib_entries = 8192,
	.l2_uc_tbl = RTL8380_TBL_L2_UC,
	.l2_cam_tbl = RTL8380_TBL_L2_CAM_UC,
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
	.set_ageing_time = otto_l2_838x_set_ageing_time,
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
	.isr_glb_src = RTL838X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL838X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL838X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL838X_IMR_GLB,
	.n_counters = 128,
	.n_pie_blocks = 12,
	.port_ignore = 0x1f,
	.vlan_tables_read = otto_vlan_838x_tables_read,
	.vlan_set_tagged = otto_vlan_838x_set_tagged,
	.vlan_set_untagged = otto_vlan_838x_set_untagged,
	.mac_force_mode_mask = RTL83XX_FORCE_EN | RTL83XX_FORCE_LINK_EN,
	.mac_force_mode_ctrl = rtl838x_mac_force_mode_ctrl,
	.mac_link_sts = RTL838X_MAC_LINK_STS,
	.vlan_profile_get = otto_vlan_838x_profile_get,
	.vlan_profile_dump = otto_vlan_838x_profile_dump,
	.vlan_profile_setup = otto_vlan_838x_profile_setup,
	.vlan_fwd_on_inner = otto_vlan_838x_port_forward_on_inner,
	.set_vlan_igr_filter = rtl838x_set_igr_filter,
	.set_vlan_egr_filter = rtl838x_set_egr_filter,
	.enable_learning = otto_l2_838x_enable_learning,
	.enable_flood = otto_l2_838x_enable_flood,
	.enable_mcast_flood = otto_l2_838x_enable_mcast_flood,
	.enable_bcast_flood = otto_l2_838x_enable_bcast_flood,
	.set_static_move_action = otto_l2_838x_set_static_move_action,
	.stp_get = rtldsa_838x_stp_get,
	.stp_set = rtl838x_stp_set,
	.mac_port_ctrl = rtl838x_mac_port_ctrl,
	.mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 | MAC_100 | MAC_1000FD,
	.mac_max_len_ctrl = RTL838X_MAC_MAX_LEN_CTRL,
	.mac_max_len_ctrl_dup = RTL838X_MAC_MAX_LEN_CTRL_DUP,
	.max_frame = RTL838X_MAX_FRAME,
	.l2_port_new_salrn = otto_l2_838x_port_new_salrn,
	.l2_port_new_sa_fwd = otto_l2_838x_port_new_sa_fwd,
	.get_mirror_config = rtldsa_838x_get_mirror_config,
	.print_matrix = rtldsa_838x_print_matrix,
	.read_l2_entry_using_hash = otto_l2_838x_read_entry_using_hash,
	.write_l2_entry_using_hash = otto_l2_838x_write_entry_using_hash,
	.read_cam = otto_l2_838x_read_cam,
	.write_cam = otto_l2_838x_write_cam,
	.vlan_port_keep_tag_set = otto_vlan_838x_port_keep_tag_set,
	.vlan_port_pvidmode_set = otto_vlan_838x_port_pvid_mode_set,
	.vlan_port_pvid_set = otto_vlan_838x_port_pvid_set,
	.fast_age = otto_l2_838x_fast_age,
	.trk_mbr_ctr = otto_lag_838x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL838X_RMA_BPDU_FLD_PMSK,
	.spcl_trap_eapol_ctrl = RTL838X_SPCL_TRAP_EAPOL_CTRL,
	.init_eee = rtl838x_init_eee,
	.set_mac_eee = rtldsa_838x_set_mac_eee,
	.l2_hash_seed = otto_l2_838x_hash_seed,
	.l2_hash_key = otto_l2_838x_hash_key,
	.read_mcast_pmask = otto_l2_838x_read_mcast_pmask,
	.write_mcast_pmask = otto_l2_838x_write_mcast_pmask,
	.pie_init = rtl838x_pie_init,
	.pie_rule_read = rtl838x_pie_rule_read,
	.pie_rule_write = rtl838x_pie_rule_write,
	.pie_rule_add = rtl838x_pie_rule_add,
	.pie_rule_rm = rtl838x_pie_rule_rm,
	.l2_learning_setup = otto_l2_838x_learning_setup,
	.packet_cntr_read = rtl838x_packet_cntr_read,
	.packet_cntr_clear = rtl838x_packet_cntr_clear,
	.set_receive_management_action = rtl838x_set_receive_management_action,
	.get_egress_rate = rtldsa_838x_get_egress_rate,
	.set_egress_rate = rtldsa_838x_set_egress_rate,
	.qos_init = rtldsa_838x_qos_init,
	.lag_set_distribution_algorithm = otto_lag_838x_set_distribution_algorithm,
	.lag_set_port_members = otto_lag_838x_set_port_members,
	.lag_setup_algomask = otto_lag_83xx_setup_algomask,
};
