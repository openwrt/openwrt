// SPDX-License-Identifier: GPL-2.0-only
/*
 * DSA switch driver for the Realtek RTL9607C family switch core.
 *
 * The RTL9607C family of SoCs has an embedded 11 port switch controller. It
 * exposes 4 internal GPHYs for the user facing ports. It also has 2 HSGMII
 * interfaces that can be connected to external PHYs, 2 interfaces for CPU
 * ports and one SerDes interface to operate with external PON transceiver.
 */

#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/iopoll.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <net/dsa.h>
#include <net/switchdev.h>

#include "rtl960x_dsa.h"

/* Driver configuration */
#define SWCORE_L2_FLUSH_TIMEOUT_US	100000
#define SWCORE_TBL_ACCESS_TIMEOUT_US	10000

/*
 * Give each standalone user port a private reserved VID, shared only with the
 * CPU, and use it as the port's PVID. This keeps untagged standalone traffic
 * out of bridge-owned VIDs and preserves per-port isolation. Bridge join/leave
 * switches the port between this VID and the bridge-managed VLANs.
 */
#define RTL960X_STANDALONE_VID(p)	(RTL960X_NUM_VLANS - RTL960X_NUM_PORTS + (p))
#define RTL960X_NUM_BRIDGE_VIDS	RTL960X_NUM_PORTS
#define RTL960X_BRIDGE_VID_START	(RTL960X_STANDALONE_VID(0) - \
					 RTL960X_NUM_BRIDGE_VIDS)
/*
 * Use the CPU port's reserved VID for the transparent default and PVID
 * fallback, keeping it outside both bridge-usable VIDs and special VID 0.
 */
#define RTL960X_DEFAULT_VID	RTL960X_STANDALONE_VID(RTL960X_CPU_PORT)

struct rtl960x_dsa {
	struct dsa_switch *ds;
	struct device *dev;
	struct regmap *map;
	struct mutex l2_lock;		/* serialize shared L2 table/flush engines */
	u16 pvid[RTL960X_NUM_PORTS];	/* shadow of each port's PVID */
	u16 bridge_pvid[RTL960X_NUM_PORTS]; /* PVID set by the bridge, 0 if none */
	/* Bridges whose ports are programmed VLAN-unaware, by bridge.num - 1. */
	DECLARE_BITMAP(unaware, RTL960X_NUM_BRIDGE_VIDS);
	u8 vlan_owner[RTL960X_BRIDGE_VID_START]; /* bridge.num using the VID */
};

static void rtl960x_cpu_port_setup(struct rtl960x_dsa *priv)
{
	regmap_write(priv->map, SWCORE_FORCE_P_ABLTY(RTL960X_CPU_PORT),
		     FORCE_P_ABLTY_NWAY | FORCE_P_ABLTY_LINK |
		     FORCE_P_ABLTY_DUPLEX | FORCE_P_ABLTY_SPEED_1000);
	regmap_write(priv->map, SWCORE_ABLTY_FORCE_MODE(RTL960X_CPU_PORT), 0xffff);
}

/*
 * Program every port's isolation portmask from the current Linux bridge
 * membership (HW bridge offload):
 *   - every user port may always reach the CPU port,
 *   - user ports in the same bridge may reach each other (forwarded in HW by
 *     the switch's own L2 engine, learning enabled),
 *   - the CPU port may reach every user port.
 * Standalone user ports, and ports in a different bridge (e.g. WAN), stay
 * CPU-only and are therefore isolated in hardware. Called at setup() (no
 * bridges yet -> every user port isolated) and on every bridge join/leave.
 */
static void rtl960x_recalc_isolation(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	int p, q;

	for (p = 0; p < ds->num_ports; p++) {
		u32 mask;

		if (dsa_is_cpu_port(ds, p)) {
			mask = 0;
			for (q = 0; q < ds->num_ports; q++)
				if (dsa_is_user_port(ds, q))
					mask |= BIT(q);
		} else if (dsa_is_user_port(ds, p)) {
			mask = BIT(RTL960X_CPU_PORT);
			for (q = 0; q < ds->num_ports; q++) {
				if (q == p || !dsa_is_user_port(ds, q))
					continue;
				if (dsa_port_bridge_same(dsa_to_port(ds, p),
							 dsa_to_port(ds, q)))
					mask |= BIT(q);
			}
		} else {
			continue;
		}

		regmap_write(priv->map, SWCORE_PORT_ISO_CTRL(p), mask & SWCORE_PORT_ISO_MASK);
	}
}

/* Flush dynamically-learned L2 entries on a port (STP topology change). */
static int rtl960x_port_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;
	u32 v;

	mutex_lock(&priv->l2_lock);

	ret = regmap_write(priv->map, SWCORE_L2_FLUSH_CTRL,
			   SWCORE_L2_FLUSH_MODE_PORT | SWCORE_L2_FLUSH_DYNAMIC);
	if (ret)
		goto out;
	ret = regmap_write(priv->map, SWCORE_L2_FLUSH_EN, BIT(port));
	if (ret)
		goto out;

	ret = regmap_read_poll_timeout(priv->map, SWCORE_L2_FLUSH_CTRL, v,
				       !(v & SWCORE_L2_FLUSH_BUSY), 10,
				       SWCORE_L2_FLUSH_TIMEOUT_US);

out:
	mutex_unlock(&priv->l2_lock);

	if (ret)
		dev_warn(priv->dev, "L2 flush failed on port %d: %pe\n",
			 port, ERR_PTR(ret));

	return ret;
}

static void rtl960x_dsa_port_fast_age(struct dsa_switch *ds, int port)
{
	rtl960x_port_fast_age(ds, port);
}

static void rtl960x_dsa_port_stp_state_set(struct dsa_switch *ds, int port,
					   u8 state)
{
	struct rtl960x_dsa *priv = ds->priv;
	u32 mask = 0, value = 0;
	int msti;
	u32 hw;

	switch (state) {
	case BR_STATE_DISABLED:
		hw = SWCORE_STP_DISABLED;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		hw = SWCORE_STP_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		hw = SWCORE_STP_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		hw = SWCORE_STP_FORWARDING;
		break;
	default:
		dev_warn(priv->dev, "Unknown STP state (%d) on port %d\n", state, port);
		return;
	}

	for (msti = 0; msti < RTL960X_NUM_MSTI; msti++) {
		mask |= SWCORE_MSTI_STATE_MASK(msti);
		value |= hw << (2 * msti);
	}

	regmap_update_bits(priv->map, SWCORE_MSTI_CTRL(port), mask, value);
}

/* Indirect VLAN-table access through the shared TBL_ACCESS engine. */
static int rtl960x_tbl_busy_wait(struct rtl960x_dsa *priv)
{
	u32 v;

	return regmap_read_poll_timeout(priv->map, SWCORE_TBL_ACCESS_STS, v,
					!(v & SWCORE_TBL_ACCESS_BUSY), 1,
					SWCORE_TBL_ACCESS_TIMEOUT_US);
}

static int rtl960x_vlan_tbl_write(struct rtl960x_dsa *priv, u16 vid, u32 word)
{
	int ret;

	mutex_lock(&priv->l2_lock);

	ret = regmap_write(priv->map, SWCORE_TBL_ACCESS_WR_DATA(0), word);
	if (ret)
		goto out;
	ret = regmap_write(priv->map, SWCORE_TBL_ACCESS_CTRL,
			   SWCORE_TBL_ACCESS_ADDR(vid) |
			   SWCORE_TBL_ACCESS_CMD(SWCORE_TBL_ACCESS_CMD_WRITE) |
			   SWCORE_TBL_ACCESS_TYPE_VLAN);
	if (ret)
		goto out;
	ret = rtl960x_tbl_busy_wait(priv);

out:
	mutex_unlock(&priv->l2_lock);

	return ret;
}

static int rtl960x_vlan_tbl_read(struct rtl960x_dsa *priv, u16 vid, u32 *word)
{
	int ret;

	mutex_lock(&priv->l2_lock);

	ret = regmap_write(priv->map, SWCORE_TBL_ACCESS_CTRL,
			   SWCORE_TBL_ACCESS_ADDR(vid) |
			   SWCORE_TBL_ACCESS_CMD(SWCORE_TBL_ACCESS_CMD_READ) |
			   SWCORE_TBL_ACCESS_TYPE_VLAN);
	if (ret)
		goto out;
	ret = rtl960x_tbl_busy_wait(priv);
	if (ret)
		goto out;

	ret = regmap_read(priv->map, SWCORE_TBL_ACCESS_RD_DATA(0), word);

out:
	mutex_unlock(&priv->l2_lock);

	return ret;
}

struct rtl960x_vlan_entry {
	u32 member;
	u32 untag;
	u8 fid;
	bool ivl;
};

/* Replace a VLAN entry's port masks and L2 learning domain. */
static int rtl960x_vlan_write(struct rtl960x_dsa *priv, u16 vid,
			      const struct rtl960x_vlan_entry *vlan)
{
	u32 word = (vlan->member & VLAN_MBR_MASK) |
		   ((vlan->untag & VLAN_MBR_MASK) << VLAN_UNTAG_SHIFT) |
		   FIELD_PREP(VLAN_FID_MSTI_MASK, vlan->fid);

	if (vlan->ivl)
		word |= VLAN_IVL_SVL;

	return rtl960x_vlan_tbl_write(priv, vid, word);
}

static int rtl960x_vlan_read(struct rtl960x_dsa *priv, u16 vid,
			     struct rtl960x_vlan_entry *vlan)
{
	u32 word;
	int ret;

	ret = rtl960x_vlan_tbl_read(priv, vid, &word);
	if (ret)
		return ret;

	vlan->member = word & VLAN_MBR_MASK;
	vlan->untag = (word >> VLAN_UNTAG_SHIFT) & VLAN_MBR_MASK;
	vlan->fid = FIELD_GET(VLAN_FID_MSTI_MASK, word);
	vlan->ivl = word & VLAN_IVL_SVL;
	return 0;
}

static int rtl960x_set_pvid(struct rtl960x_dsa *priv, int port, u16 vid)
{
	u32 shift = (port % 2) * SWCORE_VLAN_PVID_BITS;
	u32 reg = SWCORE_VLAN_PB_VID + (port / 2) * 4;
	int ret;

	ret = regmap_update_bits(priv->map, reg,
				 SWCORE_VLAN_PVID_MASK << shift, vid << shift);
	if (ret)
		return ret;

	priv->pvid[port] = vid;

	return 0;
}

/*
 * Bring the VLAN engine up with a transparent default: every port is an
 * untagged member of the default VLAN with that VID as its PVID and ingress
 * filtering off, so until DSA programs real VLANs the switch forwards exactly
 * as in the VLAN-unaware case (forwarding is still gated by the bridge/PISO
 * isolation masks). DSA then drives per-port filtering and membership --
 * the same model as the rtl83xx DSA driver.
 */
static int rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan_entry vlan = {};
	u32 all = 0;
	int p, ret, v;

	for (p = 0; p < ds->num_ports; p++)
		if (dsa_is_user_port(ds, p) || dsa_is_cpu_port(ds, p))
			all |= BIT(p);

	/*
	 * Unconfigured VLAN entries power on with every port as a member (flat
	 * forwarding). Clear them so a VID only reaches ports DSA explicitly
	 * adds; otherwise port_vlan_add's read-modify-write would keep the
	 * all-ones default and never isolate VLANs.
	 */
	for (v = 0; v < RTL960X_NUM_VLANS; v++) {
		ret = rtl960x_vlan_write(priv, v, &vlan);
		if (ret)
			return ret;
	}

	vlan.member = all;
	vlan.untag = all;
	ret = rtl960x_vlan_write(priv, RTL960X_DEFAULT_VID, &vlan);
	if (ret)
		return ret;

	/*
	 * No ingress filtering yet. Egress on user ports follows the per-VID
	 * untag mask. VID 0 stays empty, while reset VID_0_TYPE=0 treats
	 * priority-tagged ingress as untagged and classifies it with the port
	 * PVID.
	 */
	regmap_write(priv->map, SWCORE_VLAN_INGRESS, 0);
	/*
	 * The CPU port keeps each frame's ingress tag format, so CPU-bound
	 * tagging ignores the VLAN untag mask. VLAN-unaware frames stay
	 * untagged and VLAN-aware frames stay tagged, including when a VID has
	 * mixed tagging, so the bridge does not misclassify them using its PVID.
	 * This per-egress-port mode does not affect CPU-to-user traffic.
	 */
	for (p = 0; p < ds->num_ports; p++) {
		if (!dsa_is_user_port(ds, p) && !dsa_is_cpu_port(ds, p))
			continue;
		ret = regmap_write(priv->map, SWCORE_VLAN_EGR_TAG(p),
				   dsa_is_cpu_port(ds, p) ?
				   SWCORE_VLAN_EGR_MODE_KEEP :
				   SWCORE_VLAN_EGR_MODE_ORI);
		if (ret)
			return ret;
		ret = rtl960x_set_pvid(priv, p, RTL960X_DEFAULT_VID);
		if (ret)
			return ret;
	}

	return regmap_set_bits(priv->map, SWCORE_VLAN_CTRL,
			       SWCORE_VLAN_CTRL_FILTER_EN);
}

/*
 * Put a user port into standalone mode: program its reserved per-port VID with
 * the port and the CPU port as untagged members, and make it the port's PVID.
 * Untagged ingress is then tagged internally with this VID and egresses the CPU
 * untagged, so an L3 netdev on the port sees plain (untagged) frames.
 */
static int rtl960x_port_setup_standalone(struct dsa_switch *ds, int port)
{
	u32 mask = BIT(port) | BIT(RTL960X_CPU_PORT);
	u16 vid = RTL960X_STANDALONE_VID(port);
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan_entry vlan = {
		.member = mask,
		.untag = mask,
	};
	int ret;

	ret = rtl960x_vlan_write(priv, vid, &vlan);
	if (ret)
		return ret;

	return rtl960x_set_pvid(priv, port, vid);
}

/*
 * Each offloaded bridge owns one reserved VID (DSA hands out the 1-based
 * bridge number, bounded by ds->max_num_bridges). While the bridge is
 * VLAN-unaware its ports use that VID as PVID with SVL/FID 1 learning, so the
 * VLAN-unaware FDB (DSA VID 0) shares one hardware database, the same model
 * as mv88e6xxx's FID_BRIDGED and ocelot's per-bridge unaware PVID.
 */
static u16 rtl960x_bridge_vid(unsigned int num)
{
	return RTL960X_BRIDGE_VID_START + num - 1;
}

/* User ports DSA currently has under this bridge. */
static u16 rtl960x_bridge_ports(struct dsa_switch *ds,
				const struct net_device *bridge_dev)
{
	struct dsa_port *dp;
	u16 ports = 0;

	dsa_switch_for_each_user_port(dp, ds)
		if (dsa_port_bridge_dev_get(dp) == bridge_dev)
			ports |= BIT(dp->index);

	return ports;
}

static bool rtl960x_bridge_unaware(struct rtl960x_dsa *priv, unsigned int num)
{
	return test_bit(num - 1, priv->unaware);
}

static int rtl960x_bridge_vlan_write(struct rtl960x_dsa *priv,
				     unsigned int num, u16 ports,
				     bool vlan_filtering)
{
	struct rtl960x_vlan_entry vlan = {};

	if (ports && !vlan_filtering) {
		vlan.member = ports | BIT(RTL960X_CPU_PORT);
		/* CPU egress uses KEEP mode; only user ports need untag bits. */
		vlan.untag = ports;
		vlan.fid = 1;
	}

	return rtl960x_vlan_write(priv, rtl960x_bridge_vid(num), &vlan);
}

static void rtl960x_restore_vlan_modes(struct rtl960x_dsa *priv,
				       const u16 *vids,
				       const struct rtl960x_vlan_entry *vlans,
				       int count)
{
	int i;

	for (i = count - 1; i >= 0; i--)
		if (rtl960x_vlan_write(priv, vids[i], &vlans[i]))
			dev_warn(priv->dev, "failed to restore VLAN %u\n", vids[i]);
}

static int rtl960x_bridge_set_vlan_filtering(struct dsa_switch *ds,
					     unsigned int num,
					     const struct net_device *bridge_dev,
					     bool vlan_filtering)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan_entry internal, *old_vlans;
	u16 ports = rtl960x_bridge_ports(ds, bridge_dev);
	u16 old_pvid[RTL960X_NUM_PORTS];
	u16 *vids;
	int changed = 0, count = 0;
	int p, ret, vid;

	/* The whole bridge moves on its first port callback; later ones are no-ops. */
	if (rtl960x_bridge_unaware(priv, num) == !vlan_filtering)
		return regmap_assign_bits(priv->map, SWCORE_VLAN_INGRESS,
					  ports, vlan_filtering);

	for (p = 0; p < ds->num_ports; p++) {
		if (!(ports & BIT(p)))
			continue;
		ret = rtl960x_port_fast_age(ds, p);
		if (ret)
			return ret;
	}

	for (vid = 1; vid < RTL960X_BRIDGE_VID_START; vid++)
		if (priv->vlan_owner[vid] == num)
			count++;

	vids = kcalloc(max(count, 1), sizeof(*vids), GFP_KERNEL);
	old_vlans = kcalloc(max(count, 1), sizeof(*old_vlans), GFP_KERNEL);
	if (!vids || !old_vlans) {
		ret = -ENOMEM;
		goto out_free;
	}

	for (vid = 1; vid < RTL960X_BRIDGE_VID_START; vid++) {
		struct rtl960x_vlan_entry vlan;

		if (priv->vlan_owner[vid] != num)
			continue;

		ret = rtl960x_vlan_read(priv, vid, &old_vlans[changed]);
		if (ret)
			goto restore_modes;
		vlan = old_vlans[changed];
		vlan.ivl = vlan_filtering;
		vlan.fid = vlan_filtering ? 0 : 1;
		ret = rtl960x_vlan_write(priv, vid, &vlan);
		if (ret)
			goto restore_modes;
		vids[changed++] = vid;
	}

	ret = rtl960x_vlan_read(priv, rtl960x_bridge_vid(num), &internal);
	if (ret)
		goto restore_modes;
	ret = rtl960x_bridge_vlan_write(priv, num, ports, vlan_filtering);
	if (ret)
		goto restore_modes;

	for (p = 0; p < ds->num_ports; p++)
		if (ports & BIT(p))
			old_pvid[p] = priv->pvid[p];

	for (p = 0; p < ds->num_ports; p++) {
		if (!(ports & BIT(p)))
			continue;
		ret = rtl960x_set_pvid(priv, p, vlan_filtering ?
					priv->bridge_pvid[p] ?:
					RTL960X_STANDALONE_VID(p) :
					rtl960x_bridge_vid(num));
		if (ret)
			goto restore_pvids;
	}

	ret = regmap_assign_bits(priv->map, SWCORE_VLAN_INGRESS,
				 ports, vlan_filtering);
	if (ret)
		goto restore_pvids;

	assign_bit(num - 1, priv->unaware, !vlan_filtering);
	goto out_free;

restore_pvids:
	for (p = 0; p < ds->num_ports; p++)
		if ((ports & BIT(p)) && priv->pvid[p] != old_pvid[p] &&
		    rtl960x_set_pvid(priv, p, old_pvid[p]))
			dev_warn(priv->dev, "failed to restore port %d PVID\n", p);
	if (rtl960x_vlan_write(priv, rtl960x_bridge_vid(num), &internal))
		dev_warn(priv->dev, "failed to restore bridge VLAN %u\n",
			 rtl960x_bridge_vid(num));
restore_modes:
	rtl960x_restore_vlan_modes(priv, vids, old_vlans, changed);
out_free:
	kfree(old_vlans);
	kfree(vids);
	return ret;
}

static int rtl960x_dsa_port_vlan_filtering(struct dsa_switch *ds, int port,
					   bool vlan_filtering,
					   struct netlink_ext_ack *extack)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	struct rtl960x_dsa *priv = ds->priv;
	unsigned int num;

	/*
	 * Toggle per-port ingress VLAN filtering. The CPU port is left
	 * unfiltered so CPU-bound and CPU-injected frames are never dropped.
	 */
	if (dsa_is_cpu_port(ds, port))
		return 0;

	num = dsa_port_bridge_num_get(dp);
	if (num)
		return rtl960x_bridge_set_vlan_filtering(ds, num,
						dsa_port_bridge_dev_get(dp),
						vlan_filtering);

	return regmap_assign_bits(priv->map, SWCORE_VLAN_INGRESS, BIT(port),
				  vlan_filtering);
}

static int rtl960x_dsa_port_vlan_add(struct dsa_switch *ds, int port,
				     const struct switchdev_obj_port_vlan *vlan,
				     struct netlink_ext_ack *extack)
{
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan_entry entry, old_entry;
	bool unaware = false;
	unsigned int num = 0;
	int ret;

	if (!vlan->vid)
		return 0;

	/*
	 * The top of the table is reserved for driver-internal VIDs, and one
	 * hardware VLAN entry cannot serve two bridges. Like mv88e6xxx, refuse
	 * with -EOPNOTSUPP so the bridge keeps the VLAN in software.
	 */
	if (vlan->vid >= RTL960X_BRIDGE_VID_START) {
		NL_SET_ERR_MSG_MOD(extack, "VID is reserved for driver use");
		return -EOPNOTSUPP;
	}

	if (!dsa_is_cpu_port(ds, port))
		num = dsa_port_bridge_num_get(dsa_to_port(ds, port));
	if (num && priv->vlan_owner[vlan->vid] &&
	    priv->vlan_owner[vlan->vid] != num) {
		NL_SET_ERR_MSG_MOD(extack, "VID is already used by another bridge");
		return -EOPNOTSUPP;
	}
	if (num)
		unaware = rtl960x_bridge_unaware(priv, num);

	ret = rtl960x_vlan_read(priv, vlan->vid, &entry);
	if (ret)
		return ret;
	old_entry = entry;

	entry.member |= BIT(port);
	if (untagged)
		entry.untag |= BIT(port);
	else
		entry.untag &= ~BIT(port);

	entry.member |= BIT(RTL960X_CPU_PORT);
	/*
	 * CPU-bound tag format follows the CPU port's KEEP egress mode, not
	 * this bit. Keep it clear for a deterministic VLAN table state.
	 */
	entry.untag &= ~BIT(RTL960X_CPU_PORT);

	/* A CPU-first callback has no bridge context, so it preserves the mode. */
	if (num) {
		entry.ivl = !unaware;
		entry.fid = unaware ? 1 : 0;
	} else if (!dsa_is_cpu_port(ds, port)) {
		entry.ivl = true;
		entry.fid = 0;
	}

	ret = rtl960x_vlan_write(priv, vlan->vid, &entry);
	if (ret)
		return ret;

	if (!dsa_is_cpu_port(ds, port) && pvid) {
		/* A VLAN-unaware bridge keeps its internal VID as PVID. */
		if (!unaware) {
			ret = rtl960x_set_pvid(priv, port, vlan->vid);
			if (ret) {
				if (rtl960x_vlan_write(priv, vlan->vid, &old_entry))
					dev_warn(priv->dev,
						 "failed to restore VLAN %u\n",
						 vlan->vid);
				return ret;
			}
		}
		if (num)
			priv->bridge_pvid[port] = vlan->vid;
	}

	if (num)
		priv->vlan_owner[vlan->vid] = num;

	return 0;
}

static int rtl960x_dsa_port_vlan_del(struct dsa_switch *ds, int port,
				     const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan_entry entry, old_entry;
	u16 fallback = RTL960X_STANDALONE_VID(port);
	unsigned int num = 0;
	int ret;

	if (!vlan->vid)
		return 0;

	/* The top of the table is reserved for driver-internal VIDs. */
	if (vlan->vid >= RTL960X_BRIDGE_VID_START)
		return 0;

	if (!dsa_is_cpu_port(ds, port))
		num = dsa_port_bridge_num_get(dsa_to_port(ds, port));
	ret = rtl960x_vlan_read(priv, vlan->vid, &entry);
	if (ret)
		return ret;
	old_entry = entry;

	/*
	 * The host-VLAN notifier only drops the CPU port's own reference; the
	 * CPU stays a member for as long as any user port is, so leave the
	 * mask to the guard below.
	 */
	if (!dsa_is_cpu_port(ds, port)) {
		entry.member &= ~BIT(port);
		entry.untag &= ~BIT(port);
	}

	/* Drop the CPU port once the VLAN has no user members left. */
	if (!(entry.member & ~BIT(RTL960X_CPU_PORT)))
		entry = (struct rtl960x_vlan_entry){};

	ret = rtl960x_vlan_write(priv, vlan->vid, &entry);
	if (ret)
		return ret;

	if (!dsa_is_cpu_port(ds, port) && priv->pvid[port] == vlan->vid) {
		if (num && rtl960x_bridge_unaware(priv, num))
			fallback = rtl960x_bridge_vid(num);
		ret = rtl960x_set_pvid(priv, port, fallback);
		if (ret) {
			if (rtl960x_vlan_write(priv, vlan->vid, &old_entry))
				dev_warn(priv->dev, "failed to restore VLAN %u\n",
					 vlan->vid);
			return ret;
		}
	}

	if (!entry.member)
		priv->vlan_owner[vlan->vid] = 0;

	if (!dsa_is_cpu_port(ds, port) && priv->bridge_pvid[port] == vlan->vid)
		priv->bridge_pvid[port] = 0;

	return 0;
}

/*
 * L2 unicast forwarding database (FDB) offload. The hardware hashes the
 * {MAC, VLAN} key itself, so adding and deleting an entry is a single
 * MAC-method write (delete writes the same key back invalid); a dump walks the
 * valid unicast entries with the NEXT_L2UC access method. Add/delete run from
 * DSA's deferred workqueue and dump under rtnl, so they serialise against each
 * other and against the VLAN table accesses above only through l2_lock.
 */
struct rtl960x_l2_entry {
	u8 mac[ETH_ALEN];
	u16 vid;		/* CVID for IVL; 0 selects/reports SVL */
	u8 fid;			/* forwarding database for SVL entries */
	u8 port;		/* source/forwarding port (SPA) */
	bool valid;
	bool is_static;
};

/* Set/get a bit field (up to 48 bits) within the three-word entry. */
static void rtl960x_l2_field_set(u32 *w, int lsp, int len, u64 val)
{
	int i;

	for (i = 0; i < len; i++)
		if (val & BIT_ULL(i))
			w[(lsp + i) / 32] |= BIT((lsp + i) % 32);
}

static u64 rtl960x_l2_field_get(const u32 *w, int lsp, int len)
{
	u64 val = 0;
	int i;

	for (i = 0; i < len; i++)
		if (w[(lsp + i) / 32] & BIT((lsp + i) % 32))
			val |= BIT_ULL(i);

	return val;
}

static void rtl960x_l2_pack(const struct rtl960x_l2_entry *e, u32 *w)
{
	memset(w, 0, 3 * sizeof(*w));

	/*
	 * MAC byte order in the 48-bit field follows ether_addr_to_u64()
	 * (addr[0] in the most-significant byte); confirmed against the
	 * hardware by a write/read-back, like the VLAN table bring-up.
	 */
	rtl960x_l2_field_set(w, L2UC_MAC_LSP, L2UC_MAC_LEN,
			     ether_addr_to_u64(e->mac));
	if (e->vid) {
		rtl960x_l2_field_set(w, L2UC_CVID_LSP, L2UC_CVID_LEN, e->vid);
		rtl960x_l2_field_set(w, L2UC_IVL_SVL_LSP, 1, 1);
	} else {
		rtl960x_l2_field_set(w, L2UC_FID_LSP, L2UC_FID_LEN, e->fid);
	}
	rtl960x_l2_field_set(w, L2UC_SPA_LSP, L2UC_SPA_LEN, e->port);
	if (e->is_static) {
		rtl960x_l2_field_set(w, L2UC_NOSALEARN_LSP, 1, 1);
		rtl960x_l2_field_set(w, L2UC_AGE_LSP, L2UC_AGE_LEN,
				     SWCORE_L2_AGE_MAX);
	}
	if (e->valid)
		rtl960x_l2_field_set(w, L2UC_VALID_LSP, 1, 1);
}

static void rtl960x_l2_unpack(struct rtl960x_l2_entry *e, const u32 *w)
{
	u64_to_ether_addr(rtl960x_l2_field_get(w, L2UC_MAC_LSP, L2UC_MAC_LEN),
			  e->mac);
	e->valid = rtl960x_l2_field_get(w, L2UC_VALID_LSP, 1);
	e->is_static = rtl960x_l2_field_get(w, L2UC_NOSALEARN_LSP, 1);
	e->port = rtl960x_l2_field_get(w, L2UC_SPA_LSP, L2UC_SPA_LEN);
	e->fid = rtl960x_l2_field_get(w, L2UC_FID_LSP, L2UC_FID_LEN);
	/* CVID is not part of an SVL entry's key; DSA represents it as VID 0. */
	e->vid = rtl960x_l2_field_get(w, L2UC_IVL_SVL_LSP, 1) ?
		 rtl960x_l2_field_get(w, L2UC_CVID_LSP, L2UC_CVID_LEN) : 0;
}

/*
 * The 80-bit entry maps to the data registers in natural word order:
 * DATA(0) = bits[31:0], DATA(1) = bits[63:32], DATA(2) = bits[79:64]. (The
 * RTL9607C SDK helper reverses the words, but the tested hardware uses natural
 * order -- confirmed by read-back and by decoding hardware-learned entries.)
 */
static void rtl960x_l2_put_data(struct rtl960x_dsa *priv, const u32 *w)
{
	int i;

	for (i = 0; i < 3; i++)
		regmap_write(priv->map, SWCORE_TBL_ACCESS_WR_DATA(i), w[i]);
}

static int rtl960x_l2_get_data(struct rtl960x_dsa *priv, u32 *w)
{
	int i, ret;

	for (i = 0; i < 3; i++) {
		ret = regmap_read(priv->map, SWCORE_TBL_ACCESS_RD_DATA(i),
				  &w[i]);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * Add (valid) or delete (invalid) an L2 unicast entry by MAC: the hardware
 * hashes the key and places or clears the entry. On add, *hit reports whether a
 * slot was found (a miss means the hash bucket and CAM overflowed).
 */
static int rtl960x_l2_write_by_mac(struct rtl960x_dsa *priv,
				   const struct rtl960x_l2_entry *e, bool *hit)
{
	u32 w[3];
	int ret;

	rtl960x_l2_pack(e, w);

	mutex_lock(&priv->l2_lock);

	ret = rtl960x_tbl_busy_wait(priv);
	if (ret)
		goto out;

	rtl960x_l2_put_data(priv, w);
	regmap_write(priv->map, SWCORE_TBL_ACCESS_CTRL,
		     SWCORE_TBL_ACCESS_METHOD(SWCORE_TBL_ACCESS_METHOD_MAC) |
		     SWCORE_TBL_ACCESS_CMD(SWCORE_TBL_ACCESS_CMD_WRITE) |
		     SWCORE_TBL_ACCESS_TYPE_L2);

	ret = rtl960x_tbl_busy_wait(priv);
	if (ret)
		goto out;

	if (hit) {
		u32 v;

		ret = regmap_read(priv->map, SWCORE_TBL_ACCESS_STS, &v);
		if (ret)
			goto out;

		*hit = !!(v & SWCORE_TBL_ACCESS_HIT);
	}

out:
	mutex_unlock(&priv->l2_lock);

	return ret;
}

/*
 * Read the next valid unicast entry at or after index *idx (NEXT_L2UC skips
 * multicast). On a hit, fills *e and sets *idx to the found index; otherwise
 * clears *hit (no more entries).
 */
static int rtl960x_l2_read_next_locked(struct rtl960x_dsa *priv, u32 *idx,
				       struct rtl960x_l2_entry *e, bool *hit)
{
	u32 w[3], sts;
	int ret;

	ret = rtl960x_tbl_busy_wait(priv);
	if (ret)
		return ret;

	regmap_write(priv->map, SWCORE_TBL_ACCESS_CTRL,
		     SWCORE_TBL_ACCESS_ADDR(*idx) |
		     SWCORE_TBL_ACCESS_METHOD(SWCORE_TBL_ACCESS_METHOD_NEXT_L2UC) |
		     SWCORE_TBL_ACCESS_CMD(SWCORE_TBL_ACCESS_CMD_READ) |
		     SWCORE_TBL_ACCESS_TYPE_L2);

	ret = rtl960x_tbl_busy_wait(priv);
	if (ret)
		return ret;

	ret = regmap_read(priv->map, SWCORE_TBL_ACCESS_STS, &sts);
	if (ret)
		return ret;

	*hit = sts & SWCORE_TBL_ACCESS_HIT;
	if (!*hit)
		return 0;

	*idx = sts & SWCORE_TBL_ACCESS_ADDR_RET;
	ret = rtl960x_l2_get_data(priv, w);
	if (ret)
		return ret;

	rtl960x_l2_unpack(e, w);

	return 0;
}

static int rtl960x_dsa_port_fdb_add(struct dsa_switch *ds, int port,
				    const unsigned char *addr, u16 vid,
				    const struct dsa_db db)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_l2_entry e = {
		.vid = vid,
		.fid = vid ? 0 : 1,
		.port = port,
		.valid = true,
		.is_static = true,
	};
	bool hit;
	int ret;

	ether_addr_copy(e.mac, addr);

	ret = rtl960x_l2_write_by_mac(priv, &e, &hit);
	if (ret)
		return ret;

	return hit ? 0 : -ENOSPC;
}

static int rtl960x_dsa_port_fdb_del(struct dsa_switch *ds, int port,
				    const unsigned char *addr, u16 vid,
				    const struct dsa_db db)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_l2_entry e = {
		.vid = vid,
		.fid = vid ? 0 : 1,
		.valid = false,
	};

	ether_addr_copy(e.mac, addr);

	return rtl960x_l2_write_by_mac(priv, &e, NULL);
}

static int rtl960x_dsa_port_fdb_dump(struct dsa_switch *ds, int port,
				     dsa_fdb_dump_cb_t *cb, void *data)
{
	struct rtl960x_dsa *priv = ds->priv;
	u32 idx = 0;
	int ret = 0;

	mutex_lock(&priv->l2_lock);
	while (idx < SWCORE_L2_NUM_ENTRIES) {
		struct rtl960x_l2_entry e;
		u32 req = idx;
		bool hit;

		ret = rtl960x_l2_read_next_locked(priv, &idx, &e, &hit);
		if (ret)
			break;
		if (!hit)
			break;
		/*
		 * NEXT_L2UC returns the next valid entry at or after the
		 * requested index, then wraps back to the start of the table
		 * once the end is reached. Stop as soon as the returned index
		 * stops advancing so the walk always terminates.
		 */
		if (idx < req)
			break;

		if (e.valid && e.port == port) {
			ret = cb(e.mac, e.vid, e.is_static, data);
			if (ret)
				break;
		}

		idx++;
		if (!(idx % 64))
			cond_resched();
	}

	mutex_unlock(&priv->l2_lock);

	return ret;
}

static enum dsa_tag_protocol rtl960x_dsa_get_tag_protocol(struct dsa_switch *ds,
							  int port,
							  enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_RTL_OTTO;
}

static int rtl960x_dsa_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	int p, ret;

	/*
	 * The port masks below are built around GMAC0. GMAC1 (port 10) is a CPU
	 * port in hardware but is not supported as a conduit, so reject a DT
	 * that assigns one rather than silently programming port 9.
	 */
	for (p = 0; p < ds->num_ports; p++)
		if (dsa_is_cpu_port(ds, p) && p != RTL960X_CPU_PORT)
			return dev_err_probe(ds->dev, -EINVAL,
					     "port %d cannot be used as the CPU port, only port %d (GMAC0) is supported\n",
					     p, RTL960X_CPU_PORT);

	rtl960x_cpu_port_setup(priv);

	/* No bridges yet: every user port starts isolated (CPU-only). */
	rtl960x_recalc_isolation(ds);

	/* One reserved VID per offloaded bridge; DSA numbers them for us. */
	ds->max_num_bridges = RTL960X_NUM_BRIDGE_VIDS;

	/* VLAN-aware with a transparent default VLAN (see rtl960x_vlan_setup). */
	ret = rtl960x_vlan_setup(ds);
	if (ret)
		return ret;

	/* Every user port starts standalone: untagged traffic to/from the CPU. */
	for (p = 0; p < ds->num_ports; p++) {
		if (!dsa_is_user_port(ds, p))
			continue;
		ret = rtl960x_port_setup_standalone(ds, p);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtl960x_dsa_port_bridge_join(struct dsa_switch *ds, int port,
					struct dsa_bridge bridge,
					bool *tx_fwd_offload,
					struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan_entry old_vlan;
	u16 ports = rtl960x_bridge_ports(ds, bridge.dev); /* includes port */
	u16 old_pvid = priv->pvid[port];
	bool unaware;
	int ret;

	/*
	 * The first port to join decides from the bridge's own state; later
	 * ports follow what the hardware was programmed to, since the bridge
	 * may be mid-transition (vlan_filtering callbacks come per port).
	 */
	if (ports == BIT(port))
		unaware = !br_vlan_enabled(bridge.dev);
	else
		unaware = rtl960x_bridge_unaware(priv, bridge.num);

	if (unaware) {
		u16 vid = rtl960x_bridge_vid(bridge.num);

		ret = rtl960x_vlan_read(priv, vid, &old_vlan);
		if (ret)
			return ret;
		ret = rtl960x_bridge_vlan_write(priv, bridge.num, ports, false);
		if (ret)
			return ret;
		ret = rtl960x_set_pvid(priv, port, vid);
		if (ret) {
			if (rtl960x_vlan_write(priv, vid, &old_vlan))
				dev_warn(priv->dev,
					 "failed to restore bridge VLAN %u\n",
					 vid);
			priv->pvid[port] = old_pvid;
			return ret;
		}
	}

	assign_bit(bridge.num - 1, priv->unaware, unaware);

	/* Open HW forwarding among the ports of this bridge. */
	rtl960x_recalc_isolation(ds);

	return 0;
}

static void rtl960x_dsa_port_bridge_leave(struct dsa_switch *ds, int port,
					  struct dsa_bridge bridge)
{
	struct rtl960x_dsa *priv = ds->priv;
	/* The port is already unbridged here, so this excludes it. */
	u16 ports = rtl960x_bridge_ports(ds, bridge.dev);
	int ret;

	rtl960x_port_fast_age(ds, port);

	/* Back to standalone: untagged exchange with the CPU on its own VID. */
	ret = rtl960x_port_setup_standalone(ds, port);
	if (ret)
		dev_warn(priv->dev, "failed to restore port %d standalone VLAN\n",
			 port);

	priv->bridge_pvid[port] = 0;
	ret = rtl960x_bridge_vlan_write(priv, bridge.num, ports,
					!rtl960x_bridge_unaware(priv, bridge.num));
	if (ret)
		dev_warn(priv->dev, "failed to update bridge VLAN %u\n",
			 rtl960x_bridge_vid(bridge.num));
	if (!ports)
		clear_bit(bridge.num - 1, priv->unaware);

	/* Reflect the new membership. */
	rtl960x_recalc_isolation(ds);
}

static void rtl960x_dsa_phylink_get_caps(struct dsa_switch *ds, int port,
					 struct phylink_config *config)
{
	config->mac_capabilities = MAC_SYM_PAUSE | MAC_ASYM_PAUSE |
				   MAC_10 | MAC_100 | MAC_1000FD;

	__set_bit(PHY_INTERFACE_MODE_INTERNAL, config->supported_interfaces);
	__set_bit(PHY_INTERFACE_MODE_GMII, config->supported_interfaces);
}

static void rtl960x_dsa_mac_config(struct phylink_config *config,
				   unsigned int mode,
				   const struct phylink_link_state *state)
{
}

static void rtl960x_dsa_mac_link_down(struct phylink_config *config,
				      unsigned int mode,
				      phy_interface_t interface)
{
}

static void rtl960x_dsa_mac_link_up(struct phylink_config *config,
				    struct phy_device *phydev,
				    unsigned int mode,
				    phy_interface_t interface,
				    int speed, int duplex,
				    bool tx_pause, bool rx_pause)
{
	/*
	 * The internal GPHYs and the switch MAC sync speed/duplex in hardware,
	 * so nothing to program here; phylink uses the PHY link state to drive
	 * the per-port netdev carrier.
	 */
}

static const struct phylink_mac_ops rtl960x_dsa_phylink_mac_ops = {
	.mac_config	= rtl960x_dsa_mac_config,
	.mac_link_down	= rtl960x_dsa_mac_link_down,
	.mac_link_up	= rtl960x_dsa_mac_link_up,
};

static const struct dsa_switch_ops rtl960x_dsa_ops = {
	.get_tag_protocol	= rtl960x_dsa_get_tag_protocol,
	.setup			= rtl960x_dsa_setup,
	.phylink_get_caps	= rtl960x_dsa_phylink_get_caps,
	.port_bridge_join	= rtl960x_dsa_port_bridge_join,
	.port_bridge_leave	= rtl960x_dsa_port_bridge_leave,
	.port_stp_state_set	= rtl960x_dsa_port_stp_state_set,
	.port_fast_age		= rtl960x_dsa_port_fast_age,
	.port_vlan_filtering	= rtl960x_dsa_port_vlan_filtering,
	.port_vlan_add		= rtl960x_dsa_port_vlan_add,
	.port_vlan_del		= rtl960x_dsa_port_vlan_del,
	.port_fdb_add		= rtl960x_dsa_port_fdb_add,
	.port_fdb_del		= rtl960x_dsa_port_fdb_del,
	.port_fdb_dump		= rtl960x_dsa_port_fdb_dump,
};

static int rtl960x_dsa_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtl960x_dsa *priv;
	struct dsa_switch *ds;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	ds = devm_kzalloc(dev, sizeof(*ds), GFP_KERNEL);
	if (!ds)
		return -ENOMEM;

	priv->dev = dev;
	priv->ds = ds;

	ret = devm_mutex_init(dev, &priv->l2_lock);
	if (ret)
		return ret;

	priv->map = syscon_node_to_regmap(dev->of_node);
	if (IS_ERR(priv->map))
		return dev_err_probe(dev, PTR_ERR(priv->map),
				     "failed to get switch-core regmap\n");

	ds->dev = dev;
	ds->num_ports = RTL960X_NUM_PORTS;
	ds->ops = &rtl960x_dsa_ops;
	ds->phylink_mac_ops = &rtl960x_dsa_phylink_mac_ops;
	ds->priv = priv;

	platform_set_drvdata(pdev, priv);

	return dsa_register_switch(ds);
}

static void rtl960x_dsa_remove(struct platform_device *pdev)
{
	struct rtl960x_dsa *priv = platform_get_drvdata(pdev);

	if (priv)
		dsa_unregister_switch(priv->ds);
}

static void rtl960x_dsa_shutdown(struct platform_device *pdev)
{
	struct rtl960x_dsa *priv = platform_get_drvdata(pdev);

	if (priv)
		dsa_switch_shutdown(priv->ds);

	platform_set_drvdata(pdev, NULL);
}

static const struct of_device_id rtl960x_dsa_of_match[] = {
	{ .compatible = "realtek,rtl9607c-switch" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtl960x_dsa_of_match);

static struct platform_driver rtl960x_dsa_driver = {
	.probe = rtl960x_dsa_probe,
	.remove = rtl960x_dsa_remove,
	.shutdown = rtl960x_dsa_shutdown,
	.driver = {
		.name = "rtl960x-switch",
		.of_match_table = rtl960x_dsa_of_match,
	},
};
module_platform_driver(rtl960x_dsa_driver);

MODULE_DESCRIPTION("Realtek RTL9607C family DSA switch driver");
MODULE_AUTHOR("Taiga Ogawa");
MODULE_LICENSE("GPL");
