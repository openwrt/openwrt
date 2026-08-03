// SPDX-License-Identifier: GPL-2.0-only
/*
 * VLAN offload for the RTL960x switch core.
 *
 * VLAN 0 is the standalone VLAN: every user port outside a bridge is an
 * untagged member together with the CPU port and uses VID 0 as PVID, so
 * untagged standalone traffic is exchanged with the CPU (the isolation masks
 * keep standalone ports apart from each other). The CPU port also uses VID 0
 * as its PVID. A port leaves VLAN 0 when it joins a bridge and rejoins it on
 * leave.
 *
 * Each offloaded bridge owns one reserved VID at the top of the table (DSA
 * hands out the 1-based bridge number, bounded by ds->max_num_bridges). While
 * the bridge is VLAN-unaware its ports use that VID as PVID with SVL/FID 1
 * learning, so the VLAN-unaware FDB (DSA VID 0) shares one hardware
 * database, the same model as mv88e6xxx's FID_BRIDGED and ocelot's per-bridge
 * unaware PVID. The chip has no per-port "ignore the tag" mode, so the VLANs
 * a bridge owns are switched between SVL/FID 1 and IVL when vlan_filtering
 * toggles.
 */

#include <linux/bitfield.h>
#include <linux/if_bridge.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#include "rtl960x_l2.h"
#include "rtl960x_table.h"
#include "rtl960x_vlan.h"

#define RTL960X_VLAN_ENTRY_SIZE			1	/* 32-bit words */

/* VLAN table entry, one 32-bit word */
#define RTL960X_VLAN_ENTRY_MBR_MSK		GENMASK(10, 0)
#define RTL960X_VLAN_ENTRY_UNTAG_MSK		GENMASK(21, 11)
#define RTL960X_VLAN_ENTRY_FID_MSTI_MSK		GENMASK(23, 22)
/* Force the SVLAN decision to honour the IVL/SVL setting */
#define RTL960X_VLAN_ENTRY_SVLAN_CHK_MSK	BIT(24)
#define RTL960X_VLAN_ENTRY_IVL_MSK		BIT(25)
/* Index into the 32-entry extension port member table (MAC7/9/10) */
#define RTL960X_VLAN_ENTRY_EXT_MASKIDX_MSK	GENMASK(30, 26)

/* Per-port ingress VLAN filtering, one bit per port */
#define RTL960X_VLAN_INGRESS_REG		0x13004
/* Global VLAN engine enable */
#define RTL960X_VLAN_CTRL_REG			0x13008
#define   RTL960X_VLAN_CTRL_FILTER_EN_MSK	BIT(0)
/* Port-based VID, two 12-bit fields per register */
#define RTL960X_VLAN_PB_VID_REG(_p)		(0x1300c + ((_p) / 2) * 4)
#define   RTL960X_VLAN_PB_VID_SHIFT(_p)		(((_p) % 2) * 12)
#define   RTL960X_VLAN_PB_VID_MSK(_p)		(GENMASK(11, 0) << \
						 RTL960X_VLAN_PB_VID_SHIFT(_p))
/* Per-port egress tag handling */
#define RTL960X_VLAN_EGR_TAG_REG(_p)		(0x2a000 + (_p) * 4)
#define   RTL960X_VLAN_EGR_TAG_MODE_ORI		0
#define   RTL960X_VLAN_EGR_TAG_MODE_KEEP	1	/* keep the ingress format */

/*
 * struct rtl960x_vlan4k - VLAN table entry
 * @vid: VLAN ID, the table index
 * @member: port mask of ports in this VLAN
 * @untag: port mask of ports that strip the tag on egress
 * @fid: filtering database for SVL; the same field selects the MSTI
 * @ivl: independent VLAN learning (key by CVID) instead of shared (key by FID)
 * @svlan_chk: force the SVLAN decision to honour @ivl (unused)
 * @ext_maskidx: extension port member table index (unused)
 */
struct rtl960x_vlan4k {
	u16 vid;
	u16 member;
	u16 untag;
	u8 fid;
	bool ivl;
	bool svlan_chk;
	u8 ext_maskidx;
};

static int rtl960x_vlan_4k_read(struct rtl960x_dsa *priv, u16 vid,
				struct rtl960x_vlan4k *vlan4k)
{
	u32 data[RTL960X_VLAN_ENTRY_SIZE];
	int ret;

	ret = rtl960x_table_query(priv, RTL960X_TABLE_VLAN,
				  RTL960X_TABLE_OP_READ, &vid, 0, 0,
				  data, RTL960X_VLAN_ENTRY_SIZE);
	if (ret)
		return ret;

	/* Unpack table entry */
	memset(vlan4k, 0, sizeof(*vlan4k));
	vlan4k->vid = vid;
	vlan4k->member = FIELD_GET(RTL960X_VLAN_ENTRY_MBR_MSK, data[0]);
	vlan4k->untag = FIELD_GET(RTL960X_VLAN_ENTRY_UNTAG_MSK, data[0]);
	vlan4k->fid = FIELD_GET(RTL960X_VLAN_ENTRY_FID_MSTI_MSK, data[0]);
	vlan4k->svlan_chk = FIELD_GET(RTL960X_VLAN_ENTRY_SVLAN_CHK_MSK, data[0]);
	vlan4k->ivl = FIELD_GET(RTL960X_VLAN_ENTRY_IVL_MSK, data[0]);
	vlan4k->ext_maskidx = FIELD_GET(RTL960X_VLAN_ENTRY_EXT_MASKIDX_MSK,
					data[0]);

	return 0;
}

static int rtl960x_vlan_4k_write(struct rtl960x_dsa *priv,
				 const struct rtl960x_vlan4k *vlan4k)
{
	u32 data[RTL960X_VLAN_ENTRY_SIZE];
	u16 vid = vlan4k->vid;

	/* Pack table entry */
	data[0] = FIELD_PREP(RTL960X_VLAN_ENTRY_MBR_MSK, vlan4k->member) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_UNTAG_MSK, vlan4k->untag) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_FID_MSTI_MSK, vlan4k->fid) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_SVLAN_CHK_MSK, vlan4k->svlan_chk) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_IVL_MSK, vlan4k->ivl) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_EXT_MASKIDX_MSK,
			     vlan4k->ext_maskidx);

	return rtl960x_table_query(priv, RTL960X_TABLE_VLAN,
				   RTL960X_TABLE_OP_WRITE, &vid, 0, 0,
				   data, RTL960X_VLAN_ENTRY_SIZE);
}

static int rtl960x_vlan_set_pvid(struct rtl960x_dsa *priv, int port, u16 vid)
{
	int ret;

	ret = regmap_update_bits(priv->map, RTL960X_VLAN_PB_VID_REG(port),
				 RTL960X_VLAN_PB_VID_MSK(port),
				 vid << RTL960X_VLAN_PB_VID_SHIFT(port));
	if (ret)
		return ret;

	priv->pvid[port] = vid;

	return 0;
}

/*
 * Add a user port to, or remove it from, the standalone VLAN 0. A standalone
 * port is an untagged member and uses VID 0 as PVID, so untagged ingress is
 * classified to VID 0 and egresses the CPU untagged; an L3 netdev on the
 * port sees plain frames.
 */
static int rtl960x_vlan_set_standalone(struct rtl960x_dsa *priv, int port,
				       bool standalone)
{
	struct rtl960x_vlan4k vlan;
	int ret;

	ret = rtl960x_vlan_4k_read(priv, RTL960X_STANDALONE_VID, &vlan);
	if (ret)
		return ret;

	if (standalone) {
		vlan.member |= BIT(port);
		vlan.untag |= BIT(port);
	} else {
		vlan.member &= ~BIT(port);
		vlan.untag &= ~BIT(port);
	}

	ret = rtl960x_vlan_4k_write(priv, &vlan);
	if (ret || !standalone)
		return ret;

	return rtl960x_vlan_set_pvid(priv, port, RTL960X_STANDALONE_VID);
}

/**
 * rtl960x_vlan_setup() - bring up the VLAN engine with every port standalone
 * @ds: DSA switch instance
 *
 * All user ports and the CPU port become untagged members of VLAN 0 with
 * VID 0 as PVID and ingress filtering off, so until DSA programs real VLANs
 * the switch forwards exactly as in the VLAN-unaware case (forwarding is
 * still gated by the bridge/PISO isolation masks). DSA then drives per-port
 * filtering and membership -- the same model as the rtl83xx DSA driver.
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k vlan = {};
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
	for (v = 1; v < RTL960X_NUM_VLANS; v++) {
		vlan.vid = v;
		ret = rtl960x_vlan_4k_write(priv, &vlan);
		if (ret)
			return ret;
	}

	vlan.vid = RTL960X_STANDALONE_VID;
	vlan.member = all;
	vlan.untag = all;
	ret = rtl960x_vlan_4k_write(priv, &vlan);
	if (ret)
		return ret;

	/*
	 * No ingress filtering yet. Egress on user ports follows the per-VID
	 * untag mask. Reset VID_0_TYPE=0 treats priority-tagged ingress as
	 * untagged and classifies it with the port PVID.
	 */
	ret = regmap_write(priv->map, RTL960X_VLAN_INGRESS_REG, 0);
	if (ret)
		return ret;
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
		ret = regmap_write(priv->map, RTL960X_VLAN_EGR_TAG_REG(p),
				   dsa_is_cpu_port(ds, p) ?
				   RTL960X_VLAN_EGR_TAG_MODE_KEEP :
				   RTL960X_VLAN_EGR_TAG_MODE_ORI);
		if (ret)
			return ret;
		ret = rtl960x_vlan_set_pvid(priv, p, RTL960X_STANDALONE_VID);
		if (ret)
			return ret;
	}

	return regmap_set_bits(priv->map, RTL960X_VLAN_CTRL_REG,
			       RTL960X_VLAN_CTRL_FILTER_EN_MSK);
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
	struct rtl960x_vlan4k vlan = { .vid = RTL960X_BRIDGE_VID(num) };

	if (ports && !vlan_filtering) {
		vlan.member = ports | dsa_cpu_ports(priv->ds);
		/* CPU egress uses KEEP mode; only user ports need untag bits. */
		vlan.untag = ports;
		vlan.fid = 1;
	}

	return rtl960x_vlan_4k_write(priv, &vlan);
}

/**
 * rtl960x_vlan_port_bridge_join() - VLAN side of a bridge join
 * @ds: DSA switch instance
 * @port: port index
 * @bridge: bridge being joined
 *
 * Takes the port out of the standalone VLAN 0 and, for a VLAN-unaware
 * bridge, puts it on the bridge's reserved VID as PVID.
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_port_bridge_join(struct dsa_switch *ds, int port,
				  struct dsa_bridge bridge)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k old_vlan;
	u16 ports = rtl960x_bridge_ports(ds, bridge.dev); /* includes port */
	u16 vid = RTL960X_BRIDGE_VID(bridge.num);
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

	/* Leave VLAN 0; the PVID follows the bridge from here on. */
	ret = rtl960x_vlan_set_standalone(priv, port, false);
	if (ret)
		return ret;

	if (unaware) {
		ret = rtl960x_vlan_4k_read(priv, vid, &old_vlan);
		if (ret)
			goto restore_standalone;
		ret = rtl960x_bridge_vlan_write(priv, bridge.num, ports, false);
		if (ret)
			goto restore_standalone;
		ret = rtl960x_vlan_set_pvid(priv, port, vid);
		if (ret) {
			if (rtl960x_vlan_4k_write(priv, &old_vlan))
				dev_warn(priv->dev,
					 "failed to restore bridge VLAN %u\n",
					 vid);
			priv->pvid[port] = old_pvid;
			goto restore_standalone;
		}
	}

	assign_bit(bridge.num - 1, priv->unaware, unaware);

	return 0;

restore_standalone:
	if (rtl960x_vlan_set_standalone(priv, port, true))
		dev_warn(priv->dev, "failed to restore port %d standalone VLAN\n",
			 port);
	return ret;
}

/**
 * rtl960x_vlan_port_bridge_leave() - VLAN side of a bridge leave
 * @ds: DSA switch instance
 * @port: port index
 * @bridge: bridge being left
 *
 * Returns the port to the standalone VLAN 0 and shrinks the bridge's
 * reserved VLAN accordingly.
 *
 * Context: Can sleep. Called under rtnl.
 */
void rtl960x_vlan_port_bridge_leave(struct dsa_switch *ds, int port,
				    struct dsa_bridge bridge)
{
	struct rtl960x_dsa *priv = ds->priv;
	/*
	 * DSA rolls back a failed join by broadcasting the leave before it
	 * clears dp->bridge, so the port is still counted; exclude it.
	 */
	u16 ports = rtl960x_bridge_ports(ds, bridge.dev) & ~BIT(port);
	int ret;

	/* Back to standalone: untagged exchange with the CPU on VLAN 0. */
	ret = rtl960x_vlan_set_standalone(priv, port, true);
	if (ret)
		dev_warn(priv->dev, "failed to restore port %d standalone VLAN\n",
			 port);

	priv->bridge_pvid[port] = 0;
	ret = rtl960x_bridge_vlan_write(priv, bridge.num, ports,
					!rtl960x_bridge_unaware(priv, bridge.num));
	if (ret)
		dev_warn(priv->dev, "failed to update bridge VLAN %u\n",
			 RTL960X_BRIDGE_VID(bridge.num));
	if (!ports)
		clear_bit(bridge.num - 1, priv->unaware);
}

static void rtl960x_restore_vlan_modes(struct rtl960x_dsa *priv,
				       const struct rtl960x_vlan4k *vlans,
				       int count)
{
	int i;

	for (i = count - 1; i >= 0; i--)
		if (rtl960x_vlan_4k_write(priv, &vlans[i]))
			dev_warn(priv->dev, "failed to restore VLAN %u\n",
				 vlans[i].vid);
}

static int rtl960x_bridge_set_vlan_filtering(struct dsa_switch *ds,
					     unsigned int num,
					     const struct net_device *bridge_dev,
					     bool vlan_filtering)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k internal, *old_vlans;
	u16 ports = rtl960x_bridge_ports(ds, bridge_dev);
	u16 old_pvid[RTL960X_NUM_PORTS];
	int changed = 0, count = 0;
	int p, ret, vid;

	/* The whole bridge moves on its first port callback; later ones are no-ops. */
	if (rtl960x_bridge_unaware(priv, num) == !vlan_filtering)
		return regmap_assign_bits(priv->map, RTL960X_VLAN_INGRESS_REG,
					  ports, vlan_filtering);

	for (p = 0; p < ds->num_ports; p++) {
		if (!(ports & BIT(p)))
			continue;
		ret = rtl960x_l2_flush(priv, p);
		if (ret) {
			dev_err(priv->dev,
				"failed to flush port %d for VLAN filtering: %pe\n",
				p, ERR_PTR(ret));
			return ret;
		}
	}

	for (vid = 1; vid < RTL960X_BRIDGE_VID_MIN; vid++)
		if (priv->vlan_owner[vid] == num)
			count++;

	old_vlans = kcalloc(max(count, 1), sizeof(*old_vlans), GFP_KERNEL);
	if (!old_vlans)
		return -ENOMEM;

	for (vid = 1; vid < RTL960X_BRIDGE_VID_MIN; vid++) {
		struct rtl960x_vlan4k vlan;

		if (priv->vlan_owner[vid] != num)
			continue;

		ret = rtl960x_vlan_4k_read(priv, vid, &old_vlans[changed]);
		if (ret)
			goto restore_modes;
		vlan = old_vlans[changed];
		vlan.ivl = vlan_filtering;
		vlan.fid = vlan_filtering ? 0 : 1;
		ret = rtl960x_vlan_4k_write(priv, &vlan);
		if (ret)
			goto restore_modes;
		changed++;
	}

	ret = rtl960x_vlan_4k_read(priv, RTL960X_BRIDGE_VID(num), &internal);
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
		ret = rtl960x_vlan_set_pvid(priv, p, vlan_filtering ?
					    priv->bridge_pvid[p] ?:
					    RTL960X_STANDALONE_VID :
					    RTL960X_BRIDGE_VID(num));
		if (ret)
			goto restore_pvids;
	}

	ret = regmap_assign_bits(priv->map, RTL960X_VLAN_INGRESS_REG,
				 ports, vlan_filtering);
	if (ret)
		goto restore_pvids;

	assign_bit(num - 1, priv->unaware, !vlan_filtering);
	goto out_free;

restore_pvids:
	for (p = 0; p < ds->num_ports; p++)
		if ((ports & BIT(p)) && priv->pvid[p] != old_pvid[p] &&
		    rtl960x_vlan_set_pvid(priv, p, old_pvid[p]))
			dev_warn(priv->dev, "failed to restore port %d PVID\n", p);
	if (rtl960x_vlan_4k_write(priv, &internal))
		dev_warn(priv->dev, "failed to restore bridge VLAN %u\n",
			 RTL960X_BRIDGE_VID(num));
restore_modes:
	rtl960x_restore_vlan_modes(priv, old_vlans, changed);
out_free:
	kfree(old_vlans);
	return ret;
}

/**
 * rtl960x_port_vlan_filtering() - toggle VLAN filtering on a port
 * @ds: DSA switch instance
 * @port: port index
 * @vlan_filtering: new state
 * @extack: netlink extended ACK for error reporting
 *
 * Toggles per-port ingress VLAN filtering. For a bridged port the whole
 * bridge moves between the VLAN-unaware and VLAN-aware models on the first
 * callback. The CPU port is left unfiltered so CPU-bound and CPU-injected
 * frames are never dropped.
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_port_vlan_filtering(struct dsa_switch *ds, int port,
				bool vlan_filtering,
				struct netlink_ext_ack *extack)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	struct rtl960x_dsa *priv = ds->priv;
	unsigned int num;

	dev_dbg(priv->dev, "port %d: %s VLAN filtering\n", port,
		vlan_filtering ? "enable" : "disable");

	if (dsa_is_cpu_port(ds, port))
		return 0;

	num = dsa_port_bridge_num_get(dp);
	if (num)
		return rtl960x_bridge_set_vlan_filtering(ds, num,
						dsa_port_bridge_dev_get(dp),
						vlan_filtering);

	return regmap_assign_bits(priv->map, RTL960X_VLAN_INGRESS_REG, BIT(port),
				  vlan_filtering);
}

/*
 * enum rtl960x_vlan_learning - L2 learning domain of a VLAN entry
 * @RTL960X_VLAN_LEARNING_KEEP: leave the entry's IVL/FID bits as they are
 * @RTL960X_VLAN_LEARNING_IVL: independent learning keyed by the CVID
 * @RTL960X_VLAN_LEARNING_SVL_BRIDGED: shared learning in FID 1, the
 *   database of the VLAN-unaware bridges
 */
enum rtl960x_vlan_learning {
	RTL960X_VLAN_LEARNING_KEEP,
	RTL960X_VLAN_LEARNING_IVL,
	RTL960X_VLAN_LEARNING_SVL_BRIDGED,
};

/*
 * Add or remove @port in the table entry of @vlan->vid. On return @member
 * holds the resulting user-port membership, so the caller knows when the VID
 * was released without reading the table again.
 */
static int rtl960x_vlan_4k_port_set(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_vlan *vlan,
				    struct netlink_ext_ack *extack, bool include,
				    enum rtl960x_vlan_learning learning,
				    u16 *member)
{
	struct rtl960x_dsa *priv = ds->priv;
	u16 cpu_ports = dsa_cpu_ports(ds);
	struct rtl960x_vlan4k vlan4k;
	int ret;

	dev_dbg(priv->dev, "%s VLAN %d 4K on port %d\n",
		include ? "add" : "del", vlan->vid, port);

	/*
	 * The top of the table holds the per-bridge VIDs. Like mv88e6xxx,
	 * refuse with -EOPNOTSUPP so the bridge keeps such a VLAN in software.
	 */
	if (vlan->vid >= RTL960X_BRIDGE_VID_MIN) {
		NL_SET_ERR_MSG_MOD(extack, "VID is reserved for driver use");
		return -EOPNOTSUPP;
	}

	ret = rtl960x_vlan_4k_read(priv, vlan->vid, &vlan4k);
	if (ret) {
		dev_err(priv->dev, "Failed to read VLAN 4k table\n");
		return ret;
	}

	if (include) {
		vlan4k.member |= BIT(port);
		if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
			vlan4k.untag |= BIT(port);
		else
			vlan4k.untag &= ~BIT(port);
	} else if (!dsa_is_cpu_port(ds, port)) {
		/*
		 * The host-VLAN notifier only drops the CPU port's own
		 * reference; the CPU stays a member for as long as any user
		 * port is, so only user ports are removed here.
		 */
		vlan4k.member &= ~BIT(port);
		vlan4k.untag &= ~BIT(port);
	}

	/*
	 * CPU-bound tag format follows the CPU ports' KEEP egress mode, not
	 * the untag bit. Keep it clear for a deterministic table state.
	 */
	vlan4k.member |= cpu_ports;
	vlan4k.untag &= ~cpu_ports;

	switch (learning) {
	case RTL960X_VLAN_LEARNING_IVL:
		vlan4k.ivl = true;
		vlan4k.fid = 0;
		break;
	case RTL960X_VLAN_LEARNING_SVL_BRIDGED:
		vlan4k.ivl = false;
		vlan4k.fid = 1;
		break;
	case RTL960X_VLAN_LEARNING_KEEP:
		break;
	}

	/* Drop the CPU ports once the VLAN has no user members left. */
	if (!(vlan4k.member & ~cpu_ports))
		vlan4k = (struct rtl960x_vlan4k){ .vid = vlan->vid };

	ret = rtl960x_vlan_4k_write(priv, &vlan4k);
	if (ret) {
		dev_err(priv->dev, "Failed to write VLAN 4k table\n");
		return ret;
	}

	if (member)
		*member = vlan4k.member & ~cpu_ports;

	return 0;
}

/*
 * rtl960x_vlan_4k_port_add() - add a port to a VLAN table entry
 * @ds: DSA switch instance
 * @port: port index
 * @vlan: switchdev VLAN object with the target VID and flags
 * @extack: netlink extended ACK for error reporting
 * @learning: L2 learning domain to program, or KEEP to preserve it
 *
 * Adds the port to the hardware VLAN membership; the PVID is handled by the
 * caller. The entry takes @learning because the chip has no per-port
 * "ignore the tag" mode: a VLAN-unaware bridge's VLANs learn in the shared
 * FID 1 and a VLAN-aware bridge's VLANs learn per CVID.
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
static int rtl960x_vlan_4k_port_add(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_vlan *vlan,
				    struct netlink_ext_ack *extack,
				    enum rtl960x_vlan_learning learning)
{
	return rtl960x_vlan_4k_port_set(ds, port, vlan, extack, true, learning,
					NULL);
}

/*
 * rtl960x_vlan_4k_port_del() - remove a port from a VLAN table entry
 * @ds: DSA switch instance
 * @port: port index
 * @vlan: switchdev VLAN object with the target VID
 * @member: receives the user ports still in the VLAN afterwards
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
static int rtl960x_vlan_4k_port_del(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_vlan *vlan,
				    u16 *member)
{
	return rtl960x_vlan_4k_port_set(ds, port, vlan, NULL, false,
					RTL960X_VLAN_LEARNING_KEEP, member);
}

/**
 * rtl960x_port_vlan_add() - add a port to a VLAN
 * @ds: DSA switch instance
 * @port: port index
 * @vlan: switchdev VLAN object with the target VID and flags
 * @extack: netlink extended ACK for error reporting
 *
 * Programs the port's PVID with %BRIDGE_VLAN_INFO_PVID and the VLAN table
 * entry. A VID reserved for the driver, or already offloaded for another
 * bridge, is refused with %-EOPNOTSUPP so the bridge keeps it in software,
 * as mv88e6xxx does.
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_port_vlan_add(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan,
			  struct netlink_ext_ack *extack)
{
	bool untagged = !!(vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED);
	bool pvid = !!(vlan->flags & BRIDGE_VLAN_INFO_PVID);
	enum rtl960x_vlan_learning learning = RTL960X_VLAN_LEARNING_KEEP;
	struct rtl960x_dsa *priv = ds->priv;
	bool user_port = !dsa_is_cpu_port(ds, port);
	bool unaware = false;
	unsigned int num = 0;
	u16 old_pvid = 0;
	bool set_pvid;
	int ret;

	dev_dbg(priv->dev, "add VLAN %d on port %d, %s, %s\n", vlan->vid,
		port, untagged ? "untagged" : "tagged", pvid ? "PVID" : "no PVID");

	/*
	 * VID 0 is the standalone VLAN. The 8021q core registers it on every
	 * VLAN-filtering netdev that comes up; nothing to program for it, so
	 * succeed as mv88e6xxx does.
	 */
	if (vlan->vid == 0)
		return 0;

	/* Checked again in rtl960x_vlan_4k_port_set(), but before any write. */
	if (vlan->vid >= RTL960X_BRIDGE_VID_MIN) {
		NL_SET_ERR_MSG_MOD(extack, "VID is reserved for driver use");
		return -EOPNOTSUPP;
	}

	if (user_port)
		num = dsa_port_bridge_num_get(dsa_to_port(ds, port));
	if (num && priv->vlan_owner[vlan->vid] &&
	    priv->vlan_owner[vlan->vid] != num) {
		NL_SET_ERR_MSG_MOD(extack, "VID is already used by another bridge");
		return -EOPNOTSUPP;
	}

	/* A CPU-first callback has no bridge context, so it preserves the mode. */
	if (num) {
		unaware = rtl960x_bridge_unaware(priv, num);
		learning = unaware ? RTL960X_VLAN_LEARNING_SVL_BRIDGED :
				     RTL960X_VLAN_LEARNING_IVL;
	} else if (user_port) {
		learning = RTL960X_VLAN_LEARNING_IVL;
	}

	/* A VLAN-unaware bridge keeps its internal VID as PVID. */
	set_pvid = user_port && pvid && !unaware;
	if (set_pvid) {
		old_pvid = priv->pvid[port];
		ret = rtl960x_vlan_set_pvid(priv, port, vlan->vid);
		if (ret)
			return ret;
	}

	ret = rtl960x_vlan_4k_port_add(ds, port, vlan, extack, learning);
	if (ret)
		goto undo_set_pvid;

	if (user_port && pvid && num)
		priv->bridge_pvid[port] = vlan->vid;
	if (num)
		priv->vlan_owner[vlan->vid] = num;

	return 0;

undo_set_pvid:
	if (set_pvid && rtl960x_vlan_set_pvid(priv, port, old_pvid))
		dev_warn(priv->dev, "failed to restore port %d PVID\n", port);
	return ret;
}

/**
 * rtl960x_port_vlan_del() - remove a port from a VLAN
 * @ds: DSA switch instance
 * @port: port index
 * @vlan: switchdev VLAN object with the target VID
 *
 * If the VID was the port's PVID, falls back to the bridge's VLAN-unaware
 * VID or to VID 0 first, then removes the port from the VLAN table entry.
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_port_vlan_del(struct dsa_switch *ds, int port,
			  const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl960x_dsa *priv = ds->priv;
	bool user_port = !dsa_is_cpu_port(ds, port);
	u16 fallback = RTL960X_STANDALONE_VID;
	unsigned int num = 0;
	u16 old_pvid = 0;
	bool clear_pvid;
	u16 member;
	int ret;

	dev_dbg(priv->dev, "del VLAN %d on port %d\n", vlan->vid, port);

	/* Never programmed by port_vlan_add, so nothing to remove. */
	if (vlan->vid == 0 || vlan->vid >= RTL960X_BRIDGE_VID_MIN)
		return 0;

	if (user_port)
		num = dsa_port_bridge_num_get(dsa_to_port(ds, port));

	clear_pvid = user_port && priv->pvid[port] == vlan->vid;
	if (clear_pvid) {
		if (num && rtl960x_bridge_unaware(priv, num))
			fallback = RTL960X_BRIDGE_VID(num);
		old_pvid = priv->pvid[port];
		ret = rtl960x_vlan_set_pvid(priv, port, fallback);
		if (ret)
			return ret;
	}

	ret = rtl960x_vlan_4k_port_del(ds, port, vlan, &member);
	if (ret)
		goto undo_clear_pvid;

	/* Release the VID once no user port is left in it. */
	if (!member)
		priv->vlan_owner[vlan->vid] = 0;

	if (user_port && priv->bridge_pvid[port] == vlan->vid)
		priv->bridge_pvid[port] = 0;

	return 0;

undo_clear_pvid:
	if (clear_pvid && rtl960x_vlan_set_pvid(priv, port, old_pvid))
		dev_warn(priv->dev, "failed to restore port %d PVID\n", port);
	return ret;
}
