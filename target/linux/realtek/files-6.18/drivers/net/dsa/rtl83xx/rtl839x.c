// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/etherdevice.h>

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

#define RTL839X_MAC_PORT_CTRL(port)		(0x8004 + (((port) << 7)))

/* MAC maximum packet length (jumbo frame) control.
 *
 * The switch MAC drops frames whose L2 length exceeds the configured maximum.
 * A family holds either one register per user port or a single one for the
 * whole switch. The length is a direct byte value held in two 14-bit fields
 * (high-speed links in [13:0], 10/100M links in [27:14]); bit 28 selects
 * whether VLAN tag bytes count towards the limit.
 */

#define RTL839X_MAC_MAX_LEN_CTRL		(0x02b0)

#define RTL839X_MAX_FRAME			12288

#define RTL839X_MAC_FORCE_MODE_CTRL		(0x02bc)

#define RTL839X_PORT_ISO_CTRL(port)		(0x1400 + ((port) << 3))

#define RTL839X_TBL_ACCESS_CTRL_2		(0x611C)

#define RTL839X_MAC_LINK_STS			(0x0390)

#define RTL839X_EEE_TX_TIMER_GELITE_CTRL	(0x042C)
#define RTL839X_EEE_TX_TIMER_GIGA_CTRL		(0x0430)
#define RTL839X_EEE_TX_TIMER_10G_CTRL		(0x0434)
#define RTL839X_EEE_CTRL(p)			(0x8008 + ((p) << 7))

#define RTL839X_L2_CTRL_0			(0x3800)

#define RTL839X_L2_TBL_FLUSH_CTRL		(0x3ba0)

#define RTL839X_RMA_BPDU_FLD_PMSK		(0x125C)

#define RTL839X_SPCL_TRAP_EAPOL_CTRL		(0x105C)
#define RTL839X_SPCL_TRAP_SWITCH_MAC_CTRL	(0x1068)

#define RTL839X_IMR_GLB				(0x0064)
#define RTL839X_IMR_PORT_LINK_STS_CHG		(0x0068)
#define RTL839X_ISR_GLB_SRC			(0x009c)
#define RTL839X_ISR_PORT_LINK_STS_CHG		(0x00a0)

#define RTL839X_RMA_BPDU_CTRL			(0x122C)

#define RTL839X_RMA_PTP_CTRL			(0x123C)

#define RTL839X_RMA_LLDP_CTRL			(0x124C)

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
	.stp_init = rtldsa_839x_stp_init,
	.l2_bucket_size = 4,
	.n_mst = 256,
	.num_lag_ids = 16,
	.cpu_port = RTL839X_CPU_PORT,
	.fib_entries = 16384,
	.l2_uc_tbl = RTL8390_TBL_L2_UC,
	.l2_cam_tbl = RTL8390_TBL_L2_CAM_UC,
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
	.set_ageing_time = otto_l2_839x_set_ageing_time,
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
	.isr_glb_src = RTL839X_ISR_GLB_SRC,
	.isr_port_link_sts_chg = RTL839X_ISR_PORT_LINK_STS_CHG,
	.imr_port_link_sts_chg = RTL839X_IMR_PORT_LINK_STS_CHG,
	.imr_glb = RTL839X_IMR_GLB,
	.n_counters = 1024,
	.n_pie_blocks = 18,
	.port_ignore = 0x3f,
	.vlan_tables_read = otto_vlan_839x_tables_read,
	.vlan_set_tagged = otto_vlan_839x_set_tagged,
	.vlan_set_untagged = otto_vlan_839x_set_untagged,
	.vlan_profile_get = otto_vlan_839x_profile_get,
	.vlan_profile_dump = otto_vlan_839x_profile_dump,
	.vlan_profile_setup = otto_vlan_839x_profile_setup,
	.vlan_fwd_on_inner = otto_vlan_839x_port_forward_on_inner,
	.vlan_port_keep_tag_set = otto_vlan_839x_port_keep_tag_set,
	.vlan_port_pvidmode_set = otto_vlan_839x_port_pvid_mode_set,
	.vlan_port_pvid_set = otto_vlan_839x_port_pvid_set,
	.set_vlan_igr_filter = rtl839x_set_igr_filter,
	.set_vlan_egr_filter = rtl839x_set_egr_filter,
	.enable_learning = otto_l2_839x_enable_learning,
	.enable_flood = otto_l2_839x_enable_flood,
	.enable_mcast_flood = otto_l2_839x_enable_mcast_flood,
	.enable_bcast_flood = otto_l2_839x_enable_bcast_flood,
	.set_static_move_action = otto_l2_839x_set_static_move_action,
	.stp_get = rtldsa_839x_stp_get,
	.stp_set = rtl839x_stp_set,
	.mac_force_mode_mask = RTL83XX_FORCE_EN | RTL83XX_FORCE_LINK_EN,
	.mac_force_mode_ctrl = rtl839x_mac_force_mode_ctrl,
	.mac_link_sts = RTL839X_MAC_LINK_STS,
	.mac_port_ctrl = rtl839x_mac_port_ctrl,
	.mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE | MAC_10 | MAC_100 | MAC_1000FD,
	.mac_max_len_ctrl = RTL839X_MAC_MAX_LEN_CTRL,
	.max_frame = RTL839X_MAX_FRAME,
	.l2_port_new_salrn = otto_l2_839x_port_new_salrn,
	.l2_port_new_sa_fwd = otto_l2_839x_port_new_sa_fwd,
	.get_mirror_config = rtldsa_839x_get_mirror_config,
	.print_matrix = rtldsa_839x_print_matrix,
	.read_l2_entry_using_hash = otto_l2_839x_read_entry_using_hash,
	.write_l2_entry_using_hash = otto_l2_839x_write_entry_using_hash,
	.read_cam = otto_l2_839x_read_cam,
	.write_cam = otto_l2_839x_write_cam,
	.fast_age = otto_l2_839x_fast_age,
	.trk_mbr_ctr = otto_lag_839x_trk_mbr_ctr,
	.rma_bpdu_fld_pmask = RTL839X_RMA_BPDU_FLD_PMSK,
	.spcl_trap_eapol_ctrl = RTL839X_SPCL_TRAP_EAPOL_CTRL,
	.init_eee = rtl839x_init_eee,
	.set_mac_eee = rtldsa_839x_set_mac_eee,
	.l2_hash_seed = otto_l2_839x_hash_seed,
	.l2_hash_key = otto_l2_839x_hash_key,
	.read_mcast_pmask = otto_l2_839x_read_mcast_pmask,
	.write_mcast_pmask = otto_l2_839x_write_mcast_pmask,
	.pie_init = rtl839x_pie_init,
	.pie_rule_read = rtl839x_pie_rule_read,
	.pie_rule_write = rtl839x_pie_rule_write,
	.pie_rule_add = rtl839x_pie_rule_add,
	.pie_rule_rm = rtl839x_pie_rule_rm,
	.l2_learning_setup = otto_l2_839x_learning_setup,
	.packet_cntr_read = rtl839x_packet_cntr_read,
	.packet_cntr_clear = rtl839x_packet_cntr_clear,
	.set_receive_management_action = rtl839x_set_receive_management_action,
	.get_egress_rate = rtldsa_839x_get_egress_rate,
	.set_egress_rate = rtldsa_839x_set_egress_rate,
	.qos_init = rtldsa_839x_qos_init,
	.lag_set_distribution_algorithm = otto_lag_839x_set_distribution_algorithm,
	.lag_set_port_members = otto_lag_839x_set_port_members,
	.lag_setup_algomask = otto_lag_83xx_setup_algomask,
};
