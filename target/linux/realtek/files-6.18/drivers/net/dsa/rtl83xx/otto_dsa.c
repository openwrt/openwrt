// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bitops.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/inetdevice.h>
#include <linux/mfd/syscon.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/rhashtable.h>

#include <net/arp.h>
#include <net/dsa.h>
#include <net/neighbour.h>
#include <net/netevent.h>
#include <net/nexthop.h>

#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "fdb.h"
#include "l2.h"
#include "l3.h"
#include "lag.h"
#include "mac.h"
#include "mirror.h"
#include "otto_dsa.h"
#include "pie.h"
#include "qos.h"
#include "stats.h"
#include "stp.h"
#include "tc.h"
#include "vlan.h"

static const u8 ipv4_ll_mcast_addr_base[ETH_ALEN] = {
	0x01, 0x00, 0x5e, 0x00, 0x00, 0x00
};

static const u8 ipv4_ll_mcast_addr_mask[ETH_ALEN] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

static const u8 ipv6_all_hosts_mcast_addr_base[ETH_ALEN] = {
	0x33, 0x33, 0x00, 0x00, 0x00, 0x01
};

static const u8 ipv6_all_hosts_mcast_addr_mask[ETH_ALEN] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

int rtl83xx_port_is_under(const struct net_device *dev, struct otto_dsa_priv *priv)
{
	/* Is the lower network device a DSA user network device of our RTL930X-switch?
	 * Unfortunately we cannot just follow dev->dsa_prt as this is only set for the
	 * DSA conduit device. TODO: since 6.12:
	 *
	 * if(!dsa_user_dev_check(dev)) {
	 *   netdev_info(dev, "%s: not a DSA device.\n", __func__);
	 *   return -EINVAL;
	 * }
	 */

	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (!priv->ports[i].dp)
			continue;
		if (priv->ports[i].dp->user == dev)
			return i;
	}

	return -EINVAL;
}

/* DSA callbacks */

static enum dsa_tag_protocol otto_dsa_get_tag_protocol(struct dsa_switch *ds,
						       int port,
						       enum dsa_tag_protocol mprot)
{
	/* The switch does not tag the frames, instead internally the header
	 * structure for each packet is tagged accordingly.
	 */
	return DSA_TAG_PROTO_RTL_OTTO;
}

static void otto_dsa_83xx_mc_pmasks_setup(struct otto_dsa_priv *priv)
{
	/* RTL8380 and RTL8390 use an index into the portmask table to set the
	 * unknown multicast portmask, setup a default at a safe location
	 * On RTL93XX, the portmask is directly set in the profile,
	 * see e.g. rtl9300_vlan_profile_setup
	 */
	priv->r->write_mcast_pmask(MC_PMASK_ALL_PORTS_IDX, ~0);
}

static void otto_dsa_setup_bpdu_traps(struct otto_dsa_priv *priv)
{
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->set_receive_management_action(priv, i, BPDU, TRAP2CPU);
}

static void otto_dsa_setup_lldp_traps(struct otto_dsa_priv *priv)
{
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->set_receive_management_action(priv, i, LLDP, TRAP2CPU);
}

static void otto_dsa_93xx_setup_eapol_traps(struct otto_dsa_priv *priv)
{
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->set_receive_management_action(priv, i, EAPOL, TRAP2CPU);
}

static void otto_dsa_port_set_salrn(struct otto_dsa_priv *priv,
				    int port, bool enable)
{
	int shift = SALRN_PORT_SHIFT(port);
	int val = enable ? SALRN_MODE_HARDWARE : SALRN_MODE_DISABLED;

	regmap_write_bits(priv->map, priv->r->l2_port_new_salrn(port),
			  SALRN_MODE_MASK << shift, val << shift);
}

static int otto_dsa_83xx_setup(struct dsa_switch *ds)
{
	struct otto_dsa_priv *priv = ds->priv;
	int err;

	pr_debug("%s called\n", __func__);

	for (int i = 0; i < ds->num_ports; i++)
		priv->ports[i].enable = false;
	priv->ports[priv->r->cpu_port].enable = true;

	/* Configure ports so they are disabled by default, but once enabled
	 * they will work in isolated mode (only traffic between port and CPU).
	 */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy || priv->ports[i].has_pcs) {
			priv->ports[i].pm = BIT_ULL(priv->r->cpu_port);
			priv->r->traffic_set(priv, i, BIT_ULL(i));
		}
	}
	priv->r->traffic_set(priv, priv->r->cpu_port, BIT_ULL(priv->r->cpu_port));

	/* For standalone ports, forward packets even if a static fdb
	 * entry for the source address exists on another port.
	 */
	if (priv->r->set_static_move_action) {
		for (int i = 0; i <= priv->r->cpu_port; i++)
			priv->r->set_static_move_action(priv, i, true);
	}

	priv->r->print_matrix(priv);
	otto_stats_init(priv);
	otto_stats_init_counters(priv);

	otto_dsa_83xx_mc_pmasks_setup(priv);
	otto_vlan_setup(priv);

	otto_dsa_setup_bpdu_traps(priv);
	otto_dsa_setup_lldp_traps(priv);

	ds->configure_vlan_while_not_filtering = true;

	priv->r->l2_learning_setup(priv);

	otto_dsa_port_set_salrn(priv, priv->r->cpu_port, false);
	ds->assisted_learning_on_cpu_port = true;

	/* Make sure all frames sent to the switch's MAC are trapped to the CPU-port
	 *  0: FWD, 1: DROP, 2: TRAP2CPU
	 */
	regmap_write(priv->map, priv->r->self_mac_trap_ctrl, 0x2);

	priv->r->pie_init(priv);

	if (priv->r->pie_cls_flower) {
		err = otto_tc_init(priv);
		if (err)
			return err;
	}

	return 0;
}

static int otto_dsa_93xx_setup(struct dsa_switch *ds)
{
	struct otto_dsa_priv *priv = ds->priv;
	int err;

	pr_info("%s called\n", __func__);

	/* Disable all ports except CPU port */
	for (int i = 0; i < ds->num_ports; i++)
		priv->ports[i].enable = false;
	priv->ports[priv->r->cpu_port].enable = true;

	/* Configure ports so they are disabled by default, but once enabled
	 * they will work in isolated mode (only traffic between port and CPU).
	 */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy || priv->ports[i].has_pcs) {
			priv->ports[i].pm = BIT_ULL(priv->r->cpu_port);
			priv->r->traffic_set(priv, i, BIT_ULL(i));
		}
	}
	priv->r->traffic_set(priv, priv->r->cpu_port, BIT_ULL(priv->r->cpu_port));
	priv->r->print_matrix(priv);

	otto_stats_init(priv);
	otto_stats_init_counters(priv);

	otto_vlan_setup(priv);

	otto_dsa_setup_bpdu_traps(priv);
	otto_dsa_setup_lldp_traps(priv);
	otto_dsa_93xx_setup_eapol_traps(priv);

	ds->configure_vlan_while_not_filtering = true;

	priv->r->l2_learning_setup(priv);

	otto_dsa_port_set_salrn(priv, priv->r->cpu_port, false);
	ds->assisted_learning_on_cpu_port = true;

	priv->r->pie_init(priv);

	err = otto_tc_init(priv);
	if (err)
		return err;

	priv->r->led_init(priv);

	otto_qos_setup(ds);

	return 0;
}

static int otto_dsa_port_enable(struct dsa_switch *ds, int port, struct phy_device *phydev)
{
	struct otto_dsa_priv *priv = ds->priv;

	pr_debug("%s: %x %d", __func__, (u32)priv, port);
	priv->ports[port].enable = true;

	if (priv->ports[port].qinq)
		priv->r->vlan_port_qinq_set(priv, port, true);
	else
		priv->r->vlan_port_keep_tag_set(priv, port, 0, 1);

	if (dsa_is_cpu_port(ds, port))
		return 0;

	/* add port to switch mask of CPU_PORT */
	priv->r->traffic_enable(priv, priv->r->cpu_port, port);

	/* add all other ports in the same bridge to switch mask of port */
	priv->r->traffic_set(priv, port, priv->ports[port].pm);

	return 0;
}

static void otto_dsa_port_disable(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);
	/* you can only disable user ports */
	if (!dsa_is_user_port(ds, port))
		return;

	/* BUG: This does not work on RTL931X */
	/* remove port from switch mask of CPU_PORT */
	priv->r->traffic_disable(priv, priv->r->cpu_port, port);

	/* remove all other ports from switch mask of port */
	priv->r->traffic_set(priv, port, 0);

	priv->ports[port].enable = false;
}

static int otto_dsa_set_ageing_time(struct dsa_switch *ds, unsigned int msec)
{
	struct otto_dsa_priv *priv = ds->priv;

	return priv->r->set_ageing_time(priv, msec);
}

static void otto_dsa_update_port_member(struct otto_dsa_priv *priv, int port,
					const struct net_device *bridge_dev, bool join)
					__must_hold(&priv->reg_mutex)
{
	struct dsa_port *dp = dsa_to_port(priv->ds, port);
	struct rtldsa_port *p = &priv->ports[port];
	struct dsa_port *cpu_dp = dp->cpu_dp;
	u64 port_mask = BIT_ULL(cpu_dp->index);
	struct rtldsa_port *other_p;
	struct dsa_port *other_dp;
	int other_port;
	bool isolated;

	dsa_switch_for_each_user_port(other_dp, priv->ds) {
		other_port = other_dp->index;
		other_p = &priv->ports[other_port];

		if (dp == other_dp)
			continue;

		if (!dsa_port_offloads_bridge_dev(other_dp, bridge_dev))
			continue;

		isolated = p->isolated && other_p->isolated;

		if (join && !isolated) {
			port_mask |= BIT_ULL(other_port);
			other_p->pm |= BIT_ULL(port);
		} else {
			other_p->pm &= ~BIT_ULL(port);
		}

		if (other_p->enable)
			priv->r->traffic_set(priv, other_port, other_p->pm);
	}

	p->pm = port_mask;

	if (p->enable)
		priv->r->traffic_set(priv, port, port_mask);
}

static int otto_dsa_port_bridge_join(struct dsa_switch *ds, int port, struct dsa_bridge bridge,
				     bool *tx_fwd_offload, struct netlink_ext_ack *extack)
{
	struct otto_dsa_priv *priv = ds->priv;
	u16 proto = ETH_P_8021Q;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);

	br_vlan_get_proto(bridge.dev, &proto);
	if (proto != ETH_P_8021Q && (proto != ETH_P_8021AD || !priv->r->vlan_port_qinq_set)) {
		NL_SET_ERR_MSG_MOD(extack, "802.1ad bridges are not supported");
		return -EINVAL;
	}

	/* reset to default flags for new net_bridge_port */
	priv->ports[port].isolated = false;
	priv->ports[port].cached_flags = 0;

	mutex_lock(&priv->reg_mutex);

	if (proto == ETH_P_8021AD) {
		priv->r->vlan_port_qinq_set(priv, port, true);
		priv->ports[port].qinq = true;
	}

	priv->ports[port].bridged = true;
	otto_vlan_port_untagged_set(priv, port);

	otto_dsa_update_port_member(priv, port, bridge.dev, true);

	if (priv->r->set_static_move_action)
		priv->r->set_static_move_action(priv, port, false);

	/* Set to disabled in all MSTs, common code will take care of CIST */
	otto_stp_port_non_cist_states_set(priv, port, BR_STATE_DISABLED);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void otto_dsa_port_bridge_leave(struct dsa_switch *ds, int port, struct dsa_bridge bridge)
{
	struct otto_dsa_priv *priv = ds->priv;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);

	mutex_lock(&priv->reg_mutex);

	otto_dsa_update_port_member(priv, port, bridge.dev, false);

	if (priv->ports[port].qinq) {
		priv->r->vlan_port_qinq_set(priv, port, false);
		priv->ports[port].qinq = false;
	}

	priv->ports[port].bridged = false;
	otto_vlan_port_untagged_set(priv, port);

	if (priv->r->set_static_move_action)
		priv->r->set_static_move_action(priv, port, true);

	/* Set to forwarding in all MSTs, common code will take care of CIST */
	otto_stp_port_non_cist_states_set(priv, port, BR_STATE_FORWARDING);

	mutex_unlock(&priv->reg_mutex);
}

static int otto_dsa_port_vlan_protocol(struct dsa_switch *ds, int port, u16 proto,
				       struct netlink_ext_ack *extack)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (proto == (priv->ports[port].qinq ? ETH_P_8021AD : ETH_P_8021Q))
		return 0;

	NL_SET_ERR_MSG_MOD(extack, "VLAN protocol can't be changed on a bridge with switch ports");
	return -EINVAL;
}

void rtldsa_port_fast_age(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (!priv->r->fast_age)
		return;

	mutex_lock(&priv->reg_mutex);
	priv->r->fast_age(priv, port, -1);
	mutex_unlock(&priv->reg_mutex);
}

static int otto_dsa_83xx_mc_group_alloc(struct otto_dsa_priv *priv, int port)
{
	int mc_group = find_first_zero_bit(priv->mc_group_bm, MAX_MC_GROUPS - 1);
	u64 portmask;

	if (mc_group >= MAX_MC_GROUPS - 1)
		return -1;

	set_bit(mc_group, priv->mc_group_bm);
	portmask = BIT_ULL(port);
	priv->r->write_mcast_pmask(mc_group, portmask);

	return mc_group;
}

static u64 otto_dsa_83xx_mc_group_add_port(struct otto_dsa_priv *priv, int mc_group, int port)
{
	u64 portmask = priv->r->read_mcast_pmask(mc_group);

	pr_debug("%s: %d\n", __func__, port);

	portmask |= BIT_ULL(port);
	priv->r->write_mcast_pmask(mc_group, portmask);

	return portmask;
}

static u64 otto_dsa_mc_group_del_port(struct otto_dsa_priv *priv, int mc_group, int port)
{
	u64 portmask = priv->r->read_mcast_pmask(mc_group);

	pr_debug("%s: %d\n", __func__, port);

	portmask &= ~BIT_ULL(port);
	priv->r->write_mcast_pmask(mc_group, portmask);
	if (!portmask)
		clear_bit(mc_group, priv->mc_group_bm);

	return portmask;
}

static void otto_dsa_83xx_setup_l2_mc_entry(struct otto_l2_entry *e, int vid, u64 mac, int mc_group)
{
	memset(e, 0, sizeof(*e));

	e->type = L2_MULTICAST;
	e->valid = true;

	e->mc_portmask_index = mc_group;

	e->rvid = e->vid = vid;
	e->is_ip_mc = e->is_ipv6_mc = false;
	u64_to_ether_addr(mac, e->mac);
}

static bool otto_dsa_mac_is_unsnoop(const unsigned char *addr)
{
	/*
	 * RFC4541, section 2.1.2.2 + section 3:
	 * Unsnoopable address ranges must always be flooded.
	 *
	 * mapped MAC for 224.0.0.x -> 01:00:5e:00:00:xx
	 * mapped MAC for ff02::1 -> 33:33:00:00:00:01
	 */
	if (ether_addr_equal_masked(addr, ipv4_ll_mcast_addr_base,
				    ipv4_ll_mcast_addr_mask) ||
	    ether_addr_equal_masked(addr, ipv6_all_hosts_mcast_addr_base,
				    ipv6_all_hosts_mcast_addr_mask))
		return true;

	return false;
}

static int otto_dsa_83xx_port_mdb_add(struct dsa_switch *ds, int port,
				      const struct switchdev_obj_port_mdb *mdb,
				      const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(mdb->addr);
	struct otto_l2_entry e;
	int err = 0, idx;
	int vid = mdb->vid;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	int mc_group;

	pr_debug("In %s port %d, mac %llx, vid: %d\n", __func__, port, mac, vid);

	if (priv->lag_non_primary & BIT_ULL(port)) {
		pr_debug("%s: %d is lag slave. ignore\n", __func__, port);
		return -EINVAL;
	}

	if (otto_dsa_mac_is_unsnoop(mdb->addr)) {
		dev_dbg(priv->dev,
			"%s: %pM might belong to an unsnoopable IP. ignore\n",
			__func__, mdb->addr);
		return -EADDRNOTAVAIL;
	}

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, false, &e);

	/* Found an existing or empty entry */
	if (idx >= 0) {
		if (e.valid) {
			pr_debug("Found an existing entry %016llx, mc_group %d\n",
				 ether_addr_to_u64(e.mac), e.mc_portmask_index);
			otto_dsa_83xx_mc_group_add_port(priv, e.mc_portmask_index, port);
		} else {
			pr_debug("New entry for seed %016llx\n", seed);
			mc_group = otto_dsa_83xx_mc_group_alloc(priv, port);
			if (mc_group < 0) {
				err = -ENOTSUPP;
				goto out;
			}
			otto_dsa_83xx_setup_l2_mc_entry(&e, vid, mac, mc_group);
			priv->r->write_l2_entry_using_hash(priv, idx >> 2, idx & 0x3, &e);
		}
		goto out;
	}

	/* Hash buckets full, try CAM */
	idx = otto_l2_find_cam_entry(priv, seed, false, &e);

	if (idx >= 0) {
		if (e.valid) {
			pr_debug("Found existing CAM entry %016llx, mc_group %d\n",
				 ether_addr_to_u64(e.mac), e.mc_portmask_index);
			otto_dsa_83xx_mc_group_add_port(priv, e.mc_portmask_index, port);
		} else {
			pr_debug("New entry\n");
			mc_group = otto_dsa_83xx_mc_group_alloc(priv, port);
			if (mc_group < 0) {
				err = -ENOTSUPP;
				goto out;
			}
			otto_dsa_83xx_setup_l2_mc_entry(&e, vid, mac, mc_group);
			priv->r->write_cam(idx, &e);
		}
		goto out;
	}

	err = -ENOTSUPP;

out:
	mutex_unlock(&priv->reg_mutex);
	if (err)
		dev_err(ds->dev, "failed to add MDB entry\n");

	return err;
}

static int otto_dsa_93xx_port_mdb_add(struct dsa_switch *ds, int port,
				      const struct switchdev_obj_port_mdb *mdb,
				      const struct dsa_db db)
{
	return -EOPNOTSUPP;
}

static int otto_dsa_port_mdb_del(struct dsa_switch *ds, int port,
				 const struct switchdev_obj_port_mdb *mdb,
				 const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(mdb->addr);
	struct otto_l2_entry e;
	int err = 0, idx;
	int vid = mdb->vid;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	u64 portmask;

	pr_debug("In %s, port %d, mac %llx, vid: %d\n", __func__, port, mac, vid);

	if (priv->lag_non_primary & BIT_ULL(port)) {
		pr_info("%s: %d is lag slave. ignore\n", __func__, port);
		return 0;
	}

	if (otto_dsa_mac_is_unsnoop(mdb->addr)) {
		dev_dbg(priv->dev,
			"%s: %pM might belong to an unsnoopable IP. ignore\n",
			__func__, mdb->addr);
		return 0;
	}

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, true, &e);

	if (idx >= 0) {
		pr_debug("Found entry index %d, key %d and bucket %d\n", idx, idx >> 2, idx & 3);
		portmask = otto_dsa_mc_group_del_port(priv, e.mc_portmask_index, port);
		if (!portmask) {
			e.valid = false;
			priv->r->write_l2_entry_using_hash(priv, idx >> 2, idx & 0x3, &e);
		}
		goto out;
	}

	/* Check CAM for spillover from hash buckets */
	idx = otto_l2_find_cam_entry(priv, seed, true, &e);

	if (idx >= 0) {
		portmask = otto_dsa_mc_group_del_port(priv, e.mc_portmask_index, port);
		if (!portmask) {
			e.valid = false;
			priv->r->write_cam(idx, &e);
		}
		goto out;
	}
	/* TODO: Re-enable with a newer kernel: err = -ENOENT; */

out:
	mutex_unlock(&priv->reg_mutex);

	return err;
}

static int otto_dsa_port_pre_bridge_flags(struct dsa_switch *ds, int port,
					  struct switchdev_brport_flags flags,
					  struct netlink_ext_ack *extack)
{
	struct otto_dsa_priv *priv = ds->priv;
	unsigned long features = BR_ISOLATED | BR_PORT_LOCKED;

	pr_debug("%s: %d %lX\n", __func__, port, flags.val);
	if (priv->r->enable_learning)
		features |= BR_LEARNING;

	if (priv->r->enable_flood)
		features |= BR_FLOOD;

	if (priv->r->enable_mcast_flood)
		features |= BR_MCAST_FLOOD;
	if (priv->r->enable_bcast_flood)
		features |= BR_BCAST_FLOOD;
	if (flags.mask & ~(features))
		return -EINVAL;

	return 0;
}

/* dsa_port_fast_age() is DSA internal, repeat its bridge notification here. */
static void otto_dsa_port_fast_age_notify(struct dsa_port *dp)
{
	struct net_device *brport_dev = dsa_port_to_bridge_port(dp);
	struct switchdev_notifier_fdb_info info = {
		.vid = 0, /* all VLANs */
	};

	rtldsa_port_fast_age(dp->ds, dp->index);

	if (!brport_dev)
		return;

	call_switchdev_notifiers(SWITCHDEV_FDB_FLUSH_TO_BRIDGE, brport_dev,
				 &info.info, NULL);
}

static int otto_dsa_port_bridge_flags(struct dsa_switch *ds, int port,
				      struct switchdev_brport_flags flags,
				      struct netlink_ext_ack *extack)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct dsa_port *dp = dsa_to_port(ds, port);
	unsigned long cached_flags;

	pr_debug("%s: %d %lX\n", __func__, port, flags.val);

	priv->ports[port].cached_flags &= ~flags.mask;
	priv->ports[port].cached_flags |= flags.val & flags.mask;

	cached_flags = priv->ports[port].cached_flags;

	if (cached_flags & BR_PORT_LOCKED) {
		/* A locked port must not learn addresses on its own, and the
		 * entries it learned before are no longer authorized.
		 */
		otto_dsa_port_set_salrn(priv, port, false);

		if (flags.mask & BR_PORT_LOCKED)
			otto_dsa_port_fast_age_notify(dp);
	} else {
		otto_dsa_port_set_salrn(priv, port, !!(cached_flags & BR_LEARNING));
	}

	if (priv->r->enable_flood)
		priv->r->enable_flood(priv, port, !!(cached_flags & BR_FLOOD));

	/* Trap frames with an unknown source address on a locked port to the
	 * CPU, so that an authenticator can inspect them and add an FDB entry.
	 */
	priv->r->set_l2_new_sa_trap(priv, port, !!(cached_flags & BR_PORT_LOCKED));

	if (priv->r->enable_learning)
		priv->r->enable_learning(priv, port, !!(cached_flags & BR_LEARNING));

	if (priv->r->enable_mcast_flood)
		priv->r->enable_mcast_flood(priv, port, !!(cached_flags & BR_MCAST_FLOOD));

	if (priv->r->enable_bcast_flood)
		priv->r->enable_bcast_flood(priv, port, !!(cached_flags & BR_BCAST_FLOOD));

	if (flags.mask & BR_ISOLATED) {
		struct net_device *bridge_dev = dsa_port_bridge_dev_get(dp);

		priv->ports[port].isolated = !!(cached_flags & BR_ISOLATED);

		mutex_lock(&priv->reg_mutex);
		otto_dsa_update_port_member(priv, port, bridge_dev, true);
		mutex_unlock(&priv->reg_mutex);
	}

	return 0;
}

const struct dsa_switch_ops rtldsa_83xx_switch_ops = {
	.get_tag_protocol	= otto_dsa_get_tag_protocol,
	.setup			= otto_dsa_83xx_setup,

	.phylink_get_caps	= otto_mac_phylink_get_caps,

	.get_strings		= otto_stats_get_strings,
	.get_ethtool_stats	= otto_stats_get_ethtool,
	.get_sset_count		= otto_stats_get_sset_count,
	.get_eth_phy_stats	= otto_stats_get_eth_phy,
	.get_eth_mac_stats	= otto_stats_get_eth_mac,
	.get_eth_ctrl_stats	= otto_stats_get_eth_ctrl,
	.get_rmon_stats		= otto_stats_get_rmon,
	.get_stats64		= otto_stats_get64,
	.get_pause_stats	= otto_stats_get_pause,

	.port_enable		= otto_dsa_port_enable,
	.port_disable		= otto_dsa_port_disable,

	.port_change_mtu	= otto_mac_port_change_mtu,
	.port_max_mtu		= otto_mac_port_max_mtu,

	.support_eee		= otto_mac_support_eee,
	.set_mac_eee		= otto_mac_set_eee,

	.set_ageing_time	= otto_dsa_set_ageing_time,
	.port_bridge_join	= otto_dsa_port_bridge_join,
	.port_bridge_leave	= otto_dsa_port_bridge_leave,
	.port_stp_state_set	= otto_stp_port_state_set,
	.port_fast_age		= rtldsa_port_fast_age,
	.port_mst_state_set	= otto_stp_port_mst_state_set,

	.port_vlan_filtering	= otto_vlan_port_filtering,
	.port_vlan_protocol	= otto_dsa_port_vlan_protocol,
	.port_vlan_add		= otto_vlan_port_add,
	.port_vlan_del		= otto_vlan_port_del,
	.port_vlan_fast_age	= otto_vlan_port_fast_age,
	.vlan_msti_set		= otto_vlan_msti_set,

	.port_fdb_add		= otto_fdb_port_add,
	.port_fdb_del		= otto_fdb_port_del,
	.port_fdb_dump		= otto_fdb_port_dump,

	.port_mdb_add		= otto_dsa_83xx_port_mdb_add,
	.port_mdb_del		= otto_dsa_port_mdb_del,

	.port_mirror_add	= otto_mirror_port_add,
	.port_mirror_del	= otto_mirror_port_del,

	.port_lag_change	= otto_lag_port_change,
	.port_lag_join		= otto_lag_port_join,
	.port_lag_leave		= otto_lag_port_leave,

	.port_pre_bridge_flags	= otto_dsa_port_pre_bridge_flags,
	.port_bridge_flags	= otto_dsa_port_bridge_flags,

	.cls_flower_add		= otto_tc_cls_flower_add,
	.cls_flower_del		= otto_tc_cls_flower_del,
	.cls_flower_stats	= otto_tc_cls_flower_stats,
};

const struct dsa_switch_ops rtldsa_93xx_switch_ops = {
	.get_tag_protocol	= otto_dsa_get_tag_protocol,
	.setup			= otto_dsa_93xx_setup,

	.phylink_get_caps	= otto_mac_phylink_get_caps,

	.get_strings		= otto_stats_get_strings,
	.get_ethtool_stats	= otto_stats_get_ethtool,
	.get_sset_count		= otto_stats_get_sset_count,
	.get_eth_phy_stats	= otto_stats_get_eth_phy,
	.get_eth_mac_stats	= otto_stats_get_eth_mac,
	.get_eth_ctrl_stats	= otto_stats_get_eth_ctrl,
	.get_rmon_stats		= otto_stats_get_rmon,
	.get_stats64		= otto_stats_get64,
	.get_pause_stats	= otto_stats_get_pause,

	.port_enable		= otto_dsa_port_enable,
	.port_disable		= otto_dsa_port_disable,

	.port_change_mtu	= otto_mac_port_change_mtu,
	.port_max_mtu		= otto_mac_port_max_mtu,

	.support_eee		= otto_mac_support_eee,
	.set_mac_eee		= otto_mac_set_eee,

	.set_ageing_time	= otto_dsa_set_ageing_time,
	.port_bridge_join	= otto_dsa_port_bridge_join,
	.port_bridge_leave	= otto_dsa_port_bridge_leave,
	.port_stp_state_set	= otto_stp_port_state_set,
	.port_fast_age		= rtldsa_port_fast_age,
	.port_mst_state_set	= otto_stp_port_mst_state_set,

	.port_vlan_filtering	= otto_vlan_port_filtering,
	.port_vlan_protocol	= otto_dsa_port_vlan_protocol,
	.port_vlan_add		= otto_vlan_port_add,
	.port_vlan_del		= otto_vlan_port_del,
	.port_vlan_fast_age	= otto_vlan_port_fast_age,
	.vlan_msti_set		= otto_vlan_msti_set,

	.port_fdb_add		= otto_fdb_port_add,
	.port_fdb_del		= otto_fdb_port_del,
	.port_fdb_dump		= otto_fdb_port_dump,

	.port_mdb_add		= otto_dsa_93xx_port_mdb_add,
	.port_mdb_del		= otto_dsa_port_mdb_del,

	.port_mirror_add	= otto_mirror_port_add,
	.port_mirror_del	= otto_mirror_port_del,

	.port_lag_change	= otto_lag_port_change,
	.port_lag_join		= otto_lag_port_join,
	.port_lag_leave		= otto_lag_port_leave,

	.port_pre_bridge_flags	= otto_dsa_port_pre_bridge_flags,
	.port_bridge_flags	= otto_dsa_port_bridge_flags,

	.port_setup_tc		= otto_tc_port_setup_tc,
	.cls_flower_add		= otto_tc_cls_flower_add,
	.cls_flower_del		= otto_tc_cls_flower_del,
	.cls_flower_stats	= otto_tc_cls_flower_stats,

	.port_get_default_prio	= otto_qos_port_get_default_prio,
	.port_set_default_prio	= otto_qos_port_set_default_prio,
	.port_get_dscp_prio	= otto_qos_port_get_dscp_prio,
	.port_add_dscp_prio	= otto_qos_port_add_dscp_prio,
	.port_del_dscp_prio	= otto_qos_port_del_dscp_prio,
	.port_get_apptrust	= otto_qos_port_get_apptrust,
	.port_set_apptrust	= otto_qos_port_set_apptrust,
};

/*
 * A port with a PHY and without in-band status takes its link state from
 * phylib, which polls the PHY once a second. Reporting a MAC link loss to
 * phylink would make it take the link down and then straight back up from the
 * PHY state of the last poll, until the next poll reads the link as down. Let
 * phylib read the PHY now instead, which also reports a new link without
 * waiting for the poll.
 *
 * With in-band status, phylink takes the link state also from the PCS, and
 * needs the MAC link change to resolve the link again when the PCS comes up
 * after the PHY. The SFP ports have no PHY in the device tree, so the PHY of
 * an SFP module, which goes away with the module, is not used here.
 */
static void otto_dsa_port_link_change(struct dsa_switch *ds, int port,
				      const struct rtldsa_port *p, bool up)
{
	struct dsa_port *dp = dsa_to_port(ds, port);

	if (p->phy && !p->inband && dp->user && dp->user->phydev)
		phy_mac_interrupt(dp->user->phydev);
	else
		dsa_port_phylink_mac_change(ds, port, up);
}

static irqreturn_t otto_dsa_switch_irq(int irq, void *dev_id)
{
	struct dsa_switch *ds = dev_id;
	struct otto_dsa_priv *priv;
	u32 link, ports;

	priv = ds->priv;

	for (int i = 0; i < DIV_ROUND_UP(priv->r->cpu_port + 1, 32); i++) {
		if (regmap_read(priv->map, priv->r->isr_port_link_sts_chg + i * 4,
				&ports))
			break;

		/* Acknowledge only the status bits read above. */
		if (regmap_write(priv->map, priv->r->isr_port_link_sts_chg + i * 4,
				 ports))
			break;

		/* read latched */
		if (regmap_read(priv->map, priv->r->mac_link_sts + i * 4, &link))
			break;
		if (regmap_read(priv->map, priv->r->mac_link_sts + i * 4, &link))
			break;

		for (int port = i * 32; port < (i + 1) * 32 && port < priv->r->cpu_port; port++)
			if (ports & BIT(port % 32))
				otto_dsa_port_link_change(ds, port, &priv->ports[port],
							  link & BIT(port % 32));
	}

	return IRQ_HANDLED;
}

static void otto_dsa_switch_irq_disable(struct otto_dsa_priv *priv)
{
	const u32 masks[2] = { 0 };

	/* Stop the device from asserting the shared IRQ before releasing it. */
	regmap_bulk_write(priv->map, priv->r->imr_port_link_sts_chg, masks,
			  DIV_ROUND_UP(priv->r->cpu_port + 1, 32));

	if (priv->r->imr_glb)
		regmap_write(priv->map, priv->r->imr_glb, 0);
}

static bool otto_dsa_phy_load_deferred(struct phy_device *phydev)
{
	struct device *d = &phydev->mdio.dev;

	if (d->driver)
		return false;

	return driver_deferred_probe_check_state(d) == -EPROBE_DEFER;
}

static bool otto_dsa_phys_load_deferred(void)
{
	struct device_node *phy_node;
	struct phy_device *phydev;
	struct device_node *dn;
	bool deferred;

	for_each_node_by_name(dn, "port") {
		if (!of_device_is_available(dn))
			continue;

		phy_node = of_parse_phandle(dn, "phy-handle", 0);
		if (!phy_node)
			continue;

		phydev = of_phy_find_device(phy_node);
		of_node_put(phy_node);
		if (!phydev)
			continue;

		deferred = otto_dsa_phy_load_deferred(phydev);
		put_device(&phydev->mdio.dev);

		if (deferred) {
			of_node_put(dn);
			return true;
		}
	}

	return false;
}

/*
 * TODO: This check is usually built into the DSA initialization functions. After carving
 * out the mdio driver from the ethernet driver, there are two drivers that must be loaded
 * before the DSA setup can start. This driver has severe issues with handling of deferred
 * probing. For now provide this function for early dependency checks.
 */
static int otto_dsa_ethernet_loaded(struct platform_device *pdev)
{
	struct platform_device *eth_pdev;
	struct device_node *port_np;
	struct device_node *eth_np;
	int ret = -EPROBE_DEFER;

	for_each_node_with_property(port_np, "ethernet") {
		eth_np = of_parse_phandle(port_np, "ethernet", 0);
		if (!eth_np)
			continue;

		eth_pdev = of_find_device_by_node(eth_np);
		of_node_put(eth_np);

		if (!eth_pdev)
			continue;

		if (eth_pdev->dev.driver)
			ret = 0;

		put_device(&eth_pdev->dev);
	}

	return ret;
}

static int otto_dsa_mdio_loaded(void)
{
	struct device_node *dn;

	/* Check if all buses of the Realtek MDIO controller are registered. */
	dn = of_find_compatible_node(NULL, NULL, "realtek,otto-mdio");
	if (!of_device_is_available(dn)) {
		of_node_put(dn);
		return -ENODEV;
	}

	for_each_child_of_node_scoped(dn, bn) {
		struct mii_bus *bus = of_mdio_find_bus(bn);

		if (!bus) {
			of_node_put(dn);
			return -EPROBE_DEFER;
		}
		put_device(&bus->dev);
	}
	of_node_put(dn);

	return 0;
}

static int otto_dsa_ports_probe(struct otto_dsa_priv *priv)
{
	struct device_node *dn, *phy_node, *led_node;
	u32 pn;

	dn = of_find_compatible_node(NULL, NULL, "realtek,otto-switch");
	if (!dn) {
		dev_err(priv->dev, "No RTL switch node in DTS\n");
		return -ENODEV;
	}

	led_node = of_find_compatible_node(NULL, NULL, "realtek,rtl9300-leds");

	for_each_node_by_name(dn, "port") {
		const char *managed;
		u32 led_set;
		char led_set_str[16] = {0};

		if (!of_device_is_available(dn))
			continue;

		if (of_property_read_u32(dn, "reg", &pn))
			continue;

		phy_node = of_parse_phandle(dn, "phy-handle", 0);
		priv->ports[pn].has_pcs = fwnode_property_present(of_fwnode_handle(dn),
								  "pcs-handle");
		if (pn != priv->r->cpu_port && !phy_node && !priv->ports[pn].has_pcs) {
			dev_err(priv->dev, "Port node %d has neither pcs-handle nor phy-handle\n", pn);
			continue;
		}

		priv->ports[pn].leds_on_this_port = 0;
		if (led_node) {
			if (of_property_read_u32(dn, "led-set", &led_set))
				led_set = 0;
			priv->ports[pn].led_set = led_set;
			sprintf(led_set_str, "led_set%d", led_set);
			priv->ports[pn].leds_on_this_port = of_property_count_u32_elems(led_node, led_set_str);
			if (priv->ports[pn].leds_on_this_port > 4) {
				of_node_put(dn);
				dev_err(priv->dev, "led_set %d for port %d configuration is invalid\n", led_set, pn);
				return -ENODEV;
			}
		}

		priv->ports[pn].phy = !!phy_node;
		/* In-band status as phylink reads it */
		if (!of_property_read_string(dn, "managed", &managed) &&
		    !strcmp(managed, "in-band-status"))
			priv->ports[pn].inband = true;
	}

	return 0;
}

static int otto_dsa_sw_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct rtldsa_config *r;
	struct otto_dsa_priv *priv;
	int bpdu_reg, err = 0;
	u64 bpdu_mask;

	pr_debug("Probing RTL838X switch device\n");
	if (!pdev->dev.of_node) {
		dev_err(dev, "No DT found\n");
		return -EINVAL;
	}

	if (otto_dsa_phys_load_deferred())
		return -EPROBE_DEFER;

	err = otto_dsa_ethernet_loaded(pdev);
	if (err)
		return err;

	err = otto_table_loaded();
	if (err)
		return dev_err_probe(dev, err, "no switch table access\n");

	r = device_get_match_data(&pdev->dev);
	priv = devm_kzalloc(dev, struct_size(priv, msts, r->n_mst - 1), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->r = r;
	priv->map = syscon_node_to_regmap(dev->of_node);
	if (IS_ERR(priv->map))
		return dev_err_probe(dev, PTR_ERR(priv->map),
				     "no switch register map\n");

	memset(priv->lag_primary, -1, sizeof(priv->lag_primary));
	priv->l2_uc_map = devm_kcalloc(dev, r->fib_entries, sizeof(*priv->l2_uc_map),
				       GFP_KERNEL);
	if (!priv->l2_uc_map)
		return -ENOMEM;

	priv->ds = devm_kzalloc(dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds)
		return -ENOMEM;

	priv->ds->dev = dev;
	priv->ds->priv = priv;
	priv->ds->needs_standalone_vlan_filtering = true;
	priv->ds->ops = priv->r->switch_ops;
	priv->ds->phylink_mac_ops = priv->r->phylink_mac_ops;
	priv->ds->num_lag_ids = priv->r->num_lag_ids;
	priv->ds->num_ports = priv->r->cpu_port + 1;

	priv->dev = dev;
	dev_set_drvdata(dev, priv);

	err = devm_mutex_init(dev, &priv->reg_mutex);
	if (err)
		return err;

	err = devm_mutex_init(dev, &priv->counters_lock);
	if (err)
		return err;

	priv->family_id = soc_info.family;
	priv->r->stp_init(priv);
	priv->irq_mask = GENMASK_ULL(priv->r->cpu_port - 1, 0);

	err = otto_dsa_mdio_loaded();
	if (err)
		return err;

	err = otto_dsa_ports_probe(priv);
	if (err)
		return err;

	priv->wq = create_singlethread_workqueue("rtl83xx");
	if (!priv->wq) {
		dev_err(dev, "Error creating workqueue: %d\n", err);
		return -ENOMEM;
	}

	err = dsa_register_switch(priv->ds);
	if (err) {
		dev_err(dev, "Error registering switch: %d\n", err);
		goto err_register_switch;
	}

	/* dsa_to_port returns dsa_port from the port list in
	 * dsa_switch_tree, the tree is built when the switch
	 * is registered by dsa_register_switch
	 */
	for (int i = 0; i <= priv->r->cpu_port; i++)
		priv->ports[i].dp = dsa_to_port(priv->ds, i);

	/* Enable link and media change interrupts. Are the SERDES masks needed? */
	regmap_write_bits(priv->map, priv->r->isr_glb_src, 3, 3);

	for (int i = 0; i < DIV_ROUND_UP(priv->r->cpu_port + 1, 32); i++) {
		u32 mask = priv->irq_mask >> (i * 32);

		regmap_write(priv->map, priv->r->isr_port_link_sts_chg + i * 4, mask);
		regmap_write(priv->map, priv->r->imr_port_link_sts_chg + i * 4, mask);
	}

	priv->link_state_irq = platform_get_irq(pdev, 0);
	pr_info("LINK state irq: %d\n", priv->link_state_irq);
	err = request_irq(priv->link_state_irq, otto_dsa_switch_irq,
			  IRQF_SHARED, "rtldsa-link-state", priv->ds);
	if (err) {
		dev_err(dev, "Error setting up switch interrupt.\n");
		/* Need to free allocated switch here */
	} else {
		priv->link_state_irq_requested = true;
	}

	/* Enable the global interrupt mask where one exists. */
	if (priv->r->imr_glb)
		regmap_write(priv->map, priv->r->imr_glb, 1);

	otto_l2_get_ageing_time(priv);

	if (priv->r->qos_init)
		priv->r->qos_init(priv);

	/* Clear all destination ports for mirror groups */
	for (int i = 0; i < 4; i++)
		priv->mirror_group_ports[i] = -1;

	err = otto_l3_probe(dev, priv);
	if (err)
		goto err_register_l3;

	/* TODO: put this into l2_setup() */
	if (priv->r->spcl_trap_eapol_ctrl) {
		/* Flood BPDUs to all ports including cpu-port */
		bpdu_mask = GENMASK_ULL(priv->r->cpu_port, 0);
		bpdu_reg = priv->r->rma_bpdu_fld_pmask;
		if (priv->r->cpu_port >= 32) {
			regmap_write(priv->map, bpdu_reg, upper_32_bits(bpdu_mask));
			bpdu_reg += 4;
		}
		regmap_write(priv->map, bpdu_reg, lower_32_bits(bpdu_mask));

		/* TRAP 802.1X frames (EAPOL) to the CPU-Port, bypass STP and VLANs */
		regmap_write(priv->map, priv->r->spcl_trap_eapol_ctrl, 7);

		rtl838x_dbgfs_init(priv);
	} else {
		rtl930x_dbgfs_init(priv);
	}

	otto_fdb_dbgfs_init(priv);

	if (priv->r->lag_switch_init)
		priv->r->lag_switch_init(priv);

	otto_stats_dbgfs_init(priv);

	return 0;

err_register_l3:
	otto_dsa_switch_irq_disable(priv);
	if (priv->link_state_irq_requested)
		free_irq(priv->link_state_irq, priv->ds);
	cancel_delayed_work_sync(&priv->counters_work);
	dsa_unregister_switch(priv->ds);
err_register_switch:
	otto_tc_cleanup(priv);
	destroy_workqueue(priv->wq);

	return err;
}

static void otto_dsa_sw_remove(struct platform_device *pdev)
{
	struct otto_dsa_priv *priv = platform_get_drvdata(pdev);

	if (!priv)
		return;

	pr_debug("Removing platform driver for rtl83xx-sw\n");

	/* Stop debugfs readers before tearing down the switch. */
	otto_stats_dbgfs_remove(priv);

	/* unregister notifiers which will create workqueue entries with
	 * references to the switch structures. Also stop self-arming delayed
	 * work items to avoid them still accessing the DSA structures
	 * when they are getting shut down.
	 */
	otto_l3_remove(priv);
	cancel_delayed_work_sync(&priv->counters_work);

	otto_dsa_switch_irq_disable(priv);
	if (priv->link_state_irq_requested)
		free_irq(priv->link_state_irq, priv->ds);
	dsa_unregister_switch(priv->ds);

	otto_tc_cleanup(priv);

	destroy_workqueue(priv->wq);

	dev_set_drvdata(&pdev->dev, NULL);
}

static const struct of_device_id rtl83xx_switch_of_ids[] = {
	{
		.compatible = "realtek,rtl8380-switch",
		.data = &rtldsa_838x_cfg,
	},
	{
		.compatible = "realtek,rtl8392-switch",
		.data = &rtldsa_839x_cfg,
	},
	{
		.compatible = "realtek,rtl9301-switch",
		.data = &rtldsa_930x_cfg,
	},
	{
		.compatible = "realtek,rtl9311-switch",
		.data = &rtldsa_931x_cfg,
	},
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, rtl83xx_switch_of_ids);

static struct platform_driver rtl83xx_switch_driver = {
	.probe  = otto_dsa_sw_probe,
	.remove = otto_dsa_sw_remove,
	.driver = {
		.name = "rtl83xx-switch",
		.pm = NULL,
		.of_match_table = rtl83xx_switch_of_ids,
	},
};

module_platform_driver(rtl83xx_switch_driver);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL83XX SoC Switch Driver");
MODULE_LICENSE("GPL");
