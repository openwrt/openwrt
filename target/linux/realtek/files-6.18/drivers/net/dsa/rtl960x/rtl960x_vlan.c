// SPDX-License-Identifier: GPL-2.0-only
/*
 * VLAN offload for the RTL960x switch core.
 *
 * The chip has no per-port "ignore the tag" mode: the VLAN engine is global,
 * every frame is classified by its VID and egress is gated by that VID's
 * member mask. A user port that is not VLAN-filtering (standalone, or in a
 * VLAN-unaware bridge) is therefore made a member of every user VID (1-4083;
 * the top of the table holds the driver's per-bridge VIDs) and keeps the
 * ingress tag format on egress ("transparent"), so tagged frames of those
 * VIDs pass through it unmodified; the address lookup and the isolation
 * masks decide where they go. A port of a VLAN-aware bridge is a member of
 * the VLANs DSA adds and strips tags per the untag mask.
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
 * learning, so untagged traffic of the VLAN-unaware bridges (DSA VID 0)
 * shares one hardware database, the same model as mv88e6xxx's FID_BRIDGED
 * and mt7530's FID_BRIDGED. All other VIDs learn per VID (IVL).
 */

#include <linux/bitfield.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/regmap.h>
#include <linux/soc/realtek/otto_table.h>

#include "rtl960x_l2.h"
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
#define   RTL960X_VLAN_EGR_TAG_MODE_ORI		0	/* follow the untag mask */
#define   RTL960X_VLAN_EGR_TAG_MODE_KEEP	1	/* keep the ingress format */

/**
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

static int rtl960x_vlan_4k_read(u16 vid, struct rtl960x_vlan4k *vlan4k)
{
	u32 data[RTL960X_VLAN_ENTRY_SIZE];
	int ret;

	ret = otto_table_read(RTL9607C_TBL_VLAN, vid, &data);
	if (ret)
		return ret;

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

static int rtl960x_vlan_4k_write(const struct rtl960x_vlan4k *vlan4k)
{
	u32 data[RTL960X_VLAN_ENTRY_SIZE];

	data[0] = FIELD_PREP(RTL960X_VLAN_ENTRY_MBR_MSK, vlan4k->member) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_UNTAG_MSK, vlan4k->untag) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_FID_MSTI_MSK, vlan4k->fid) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_SVLAN_CHK_MSK, vlan4k->svlan_chk) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_IVL_MSK, vlan4k->ivl) |
		  FIELD_PREP(RTL960X_VLAN_ENTRY_EXT_MASKIDX_MSK,
			     vlan4k->ext_maskidx);

	return otto_table_write(RTL9607C_TBL_VLAN, vlan4k->vid, &data);
}

static int rtl960x_vlan_set_pvid(struct rtl960x_dsa *priv, int port, u16 vid)
{
	int ret;

	ret = regmap_update_bits(priv->map, RTL960X_VLAN_PB_VID_REG(port),
				 RTL960X_VLAN_PB_VID_MSK(port),
				 field_prep(RTL960X_VLAN_PB_VID_MSK(port), vid));
	if (ret)
		return ret;

	priv->pvid[port] = vid;

	return 0;
}

/*
 * Write the table entry of a regular VID (1 .. RTL960X_BRIDGE_VID_MIN - 1)
 * from the membership DSA configured plus the transparent ports. The CPU
 * ports are members whenever any user port is; their KEEP egress mode makes
 * the untag bits irrelevant for them, as it does for the transparent ports.
 */
static int rtl960x_vlan_4k_sync(struct rtl960x_dsa *priv, u16 vid)
{
	struct rtl960x_vlan4k vlan = { .vid = vid };
	u16 user = priv->vlan_member[vid] | priv->transparent;

	if (user) {
		vlan.member = user | dsa_cpu_ports(priv->ds);
		vlan.untag = priv->vlan_untag[vid];
		vlan.ivl = true;
	}

	return rtl960x_vlan_4k_write(&vlan);
}

static int rtl960x_vlan_4k_sync_all(struct rtl960x_dsa *priv)
{
	int ret, vid;

	for (vid = 1; vid < RTL960X_BRIDGE_VID_MIN; vid++) {
		ret = rtl960x_vlan_4k_sync(priv, vid);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtl960x_vlan_set_egress_mode(struct rtl960x_dsa *priv, int port,
					bool keep)
{
	return regmap_write(priv->map, RTL960X_VLAN_EGR_TAG_REG(port),
			    keep ? RTL960X_VLAN_EGR_TAG_MODE_KEEP :
				   RTL960X_VLAN_EGR_TAG_MODE_ORI);
}

/*
 * Make the user ports in @ports transparent, or VLAN-filtering again. A
 * transparent port is a member of every regular VID and keeps the ingress
 * tag format on egress; a filtering port is a member of the VLANs DSA added
 * and follows the untag mask. Only ports whose state actually changes are
 * touched, and the table is rewritten once for all of them.
 */
static int rtl960x_vlan_set_transparent(struct rtl960x_dsa *priv, u16 ports,
					bool transparent)
{
	u16 old = priv->transparent;
	int p, ret;

	if (transparent)
		priv->transparent |= ports;
	else
		priv->transparent &= ~ports;
	ports = priv->transparent ^ old;
	if (!ports)
		return 0;

	for (p = 0; p < RTL960X_NUM_PORTS; p++) {
		if (!(ports & BIT(p)))
			continue;
		ret = rtl960x_vlan_set_egress_mode(priv, p, transparent);
		if (ret)
			goto restore;
	}

	ret = rtl960x_vlan_4k_sync_all(priv);
	if (ret)
		goto restore;

	return 0;

restore:
	priv->transparent = old;
	for (p = 0; p < RTL960X_NUM_PORTS; p++)
		if ((ports & BIT(p)) &&
		    rtl960x_vlan_set_egress_mode(priv, p, !transparent))
			dev_warn(priv->dev,
				 "failed to restore port %d egress mode\n", p);
	if (rtl960x_vlan_4k_sync_all(priv))
		dev_warn(priv->dev, "failed to restore VLAN memberships\n");
	return ret;
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

	ret = rtl960x_vlan_4k_read(RTL960X_STANDALONE_VID, &vlan);
	if (ret)
		return ret;

	if (standalone) {
		vlan.member |= BIT(port);
		vlan.untag |= BIT(port);
	} else {
		vlan.member &= ~BIT(port);
		vlan.untag &= ~BIT(port);
	}

	ret = rtl960x_vlan_4k_write(&vlan);
	if (ret || !standalone)
		return ret;

	return rtl960x_vlan_set_pvid(priv, port, RTL960X_STANDALONE_VID);
}

/**
 * rtl960x_vlan_setup() - bring up the VLAN engine with every port standalone
 * @ds: DSA switch instance
 *
 * All user ports and the CPU port become untagged members of VLAN 0 with
 * VID 0 as PVID and ingress filtering off. Every user port starts
 * transparent: a member of every user VID (1-4083), keeping the ingress tag
 * format on egress, so frames forwarded to the CPU retain their ingress tag
 * format and forwarding is decided by the address lookup and the isolation
 * masks. The reserved VIDs at the top of the table are cleared.
 *
 * Context: Can sleep.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k vlan = {};
	struct dsa_port *dp;
	u16 user, all;
	int ret, vid;

	user = dsa_user_ports(ds);
	all = user | dsa_cpu_ports(ds);

	vlan.vid = RTL960X_STANDALONE_VID;
	vlan.member = all;
	vlan.untag = all;
	ret = rtl960x_vlan_4k_write(&vlan);
	if (ret)
		return ret;

	/*
	 * Unconfigured VLAN entries power on with every port as a member, FID
	 * 0 and shared learning. Rewrite them as IVL with the user ports as
	 * transparent members, so tagged traffic learns per VID instead of in
	 * the standalone database.
	 */
	priv->transparent = user;
	ret = rtl960x_vlan_4k_sync_all(priv);
	if (ret)
		return ret;

	/* The per-bridge VIDs and VID 4095 start empty. */
	vlan = (struct rtl960x_vlan4k){};
	for (vid = RTL960X_BRIDGE_VID_MIN; vid < RTL960X_NUM_VLANS; vid++) {
		vlan.vid = vid;
		ret = rtl960x_vlan_4k_write(&vlan);
		if (ret)
			return ret;
	}

	/*
	 * No ingress filtering yet. Reset VID_0_TYPE=0 treats priority-tagged
	 * ingress as untagged and classifies it with the port PVID.
	 */
	ret = regmap_write(priv->map, RTL960X_VLAN_INGRESS_REG, 0);
	if (ret)
		return ret;
	/*
	 * Every port keeps each frame's ingress tag format on egress. For the
	 * CPU port this means CPU-bound tagging ignores the VLAN untag mask:
	 * VLAN-unaware frames stay untagged and VLAN-aware frames stay tagged,
	 * including when a VID has mixed tagging, so the bridge does not
	 * misclassify them using its PVID. User ports switch to the untag
	 * mask only while they are in a VLAN-aware bridge.
	 */
	dsa_switch_for_each_available_port(dp, ds) {
		ret = rtl960x_vlan_set_egress_mode(priv, dp->index, true);
		if (ret)
			return ret;
		ret = rtl960x_vlan_set_pvid(priv, dp->index,
					    RTL960X_STANDALONE_VID);
		if (ret)
			return ret;
	}

	return regmap_set_bits(priv->map, RTL960X_VLAN_CTRL_REG,
			       RTL960X_VLAN_CTRL_FILTER_EN_MSK);
}

/*
 * Whether a bridged port is programmed VLAN-unaware. The state is per port,
 * as DSA applies vlan_filtering per port and may roll a single port back:
 * a transparent bridge port uses the bridge's reserved VID as PVID and is a
 * member of it, a filtering one follows the VLANs DSA added.
 */
static bool rtl960x_port_unaware(struct rtl960x_dsa *priv, int port)
{
	return priv->transparent & BIT(port);
}

/* Fallback PVID when the configured PVID is cleared or its VLAN is deleted. */
static u16 rtl960x_vlan_pvid_fallback(struct rtl960x_dsa *priv, int port,
				      unsigned int num)
{
	if (num && rtl960x_port_unaware(priv, port))
		return RTL960X_BRIDGE_VID(num);

	return RTL960X_STANDALONE_VID;
}

/*
 * The reserved VID of a bridge: its VLAN-unaware ports plus the CPU ports,
 * learning in the shared FID 1. The ports are transparent, so no untag bits
 * are needed. Empty once every port of the bridge is VLAN-filtering.
 */
static int rtl960x_bridge_vlan_write(struct rtl960x_dsa *priv,
				     unsigned int num, u16 ports)
{
	struct rtl960x_vlan4k vlan = { .vid = RTL960X_BRIDGE_VID(num) };

	ports &= priv->transparent;
	if (ports) {
		vlan.member = ports | dsa_cpu_ports(priv->ds);
		vlan.fid = 1;
	}

	return rtl960x_vlan_4k_write(&vlan);
}

/* Transparency and ingress filter of @ports; restores both on failure. */
static int rtl960x_vlan_set_port_filtering(struct rtl960x_dsa *priv, u16 ports,
					   bool vlan_filtering)
{
	u16 old = priv->transparent;
	int ret;

	ret = rtl960x_vlan_set_transparent(priv, ports, !vlan_filtering);
	if (ret)
		return ret;

	ret = regmap_assign_bits(priv->map, RTL960X_VLAN_INGRESS_REG, ports,
				 vlan_filtering);
	if (ret) {
		ports = priv->transparent ^ old;
		if (ports &&
		    rtl960x_vlan_set_transparent(priv, ports, vlan_filtering))
			dev_warn(priv->dev,
				 "failed to restore port transparency\n");
	}

	return ret;
}

struct rtl960x_vlan_upper_check {
	struct rtl960x_dsa *priv;
	struct net_device *br;
	unsigned int num;
	struct netlink_ext_ack *extack;
};

static int rtl960x_vlan_check_upper(struct net_device *vdev, int vid, void *arg)
{
	struct rtl960x_vlan_upper_check *c = arg;
	struct bridge_vlan_info info;

	if (vid >= RTL960X_BRIDGE_VID_MIN) {
		NL_SET_ERR_MSG_MOD(c->extack,
				   "Must first remove VLAN uppers with VIDs reserved by the driver");
		return -EINVAL;
	}
	if (c->priv->vlan_owner[vid] && c->priv->vlan_owner[vid] != c->num) {
		NL_SET_ERR_MSG_MOD(c->extack,
				   "Must first remove VLAN uppers with VIDs used by another bridge");
		return -EINVAL;
	}
	if (!br_vlan_get_info(c->br, vid, &info)) {
		NL_SET_ERR_MSG_MOD(c->extack,
				   "Must first remove VLAN uppers having VIDs also present in bridge");
		return -EINVAL;
	}

	return 0;
}

/*
 * Before any port of a bridge turns VLAN-filtering, whether on its
 * vlan_filtering callback or on a join to a VLAN-aware bridge, make sure
 * the 802.1Q uppers of every port of the bridge can be offloaded. DSA
 * replays a port's uppers as VLANs once it is filtering and, if that
 * fails, restores just that port to non-filtering, which a VLAN-aware
 * bridge must never contain (see rtl960x_port_vlan_filtering()). DSA also
 * applies the bridge's setting port by port without rolling the earlier
 * ports back, so the conditions it checks for the port at hand are checked
 * for all of them.
 */
static int rtl960x_vlan_check_bridge_uppers(struct dsa_switch *ds,
					    struct net_device *br,
					    unsigned int num,
					    struct netlink_ext_ack *extack)
{
	struct rtl960x_vlan_upper_check c = {
		.priv = ds->priv, .br = br, .num = num, .extack = extack,
	};
	struct dsa_port *dp;
	int ret;

	dsa_switch_for_each_bridge_member(dp, ds, br) {
		if (!dp->user)
			continue;
		ret = vlan_for_each(dp->user, rtl960x_vlan_check_upper, &c);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * vlan_filtering change of a bridged port. Its untagged traffic moves
 * between the bridge's reserved VID (SVL/FID 1) and the PVID VLAN the
 * bridge configured, so what was learned under the old key is stale and
 * the port is flushed first. Only this port changes: DSA applies the
 * bridge's setting port by port and restores a port on its own when its
 * 802.1Q uppers cannot be offloaded, so the rest of the bridge must stay
 * as it is.
 */
static int rtl960x_bridge_set_vlan_filtering(struct dsa_switch *ds, int port,
					     unsigned int num,
					     const struct net_device *bridge_dev,
					     bool vlan_filtering)
{
	struct rtl960x_dsa *priv = ds->priv;
	bool was_filtering = !rtl960x_port_unaware(priv, port);
	u16 ports = dsa_bridge_ports(ds, bridge_dev);
	u16 old_pvid = priv->pvid[port];
	int ret;

	ret = rtl960x_l2_flush(priv, port);
	if (ret) {
		dev_err(priv->dev,
			"failed to flush port %d for VLAN filtering: %pe\n",
			port, ERR_PTR(ret));
		return ret;
	}

	ret = rtl960x_vlan_set_pvid(priv, port, vlan_filtering ?
				    priv->bridge_pvid[port] ?:
				    RTL960X_STANDALONE_VID :
				    RTL960X_BRIDGE_VID(num));
	if (ret)
		return ret;

	ret = rtl960x_vlan_set_port_filtering(priv, BIT(port), vlan_filtering);
	if (ret)
		goto restore_pvid;

	ret = rtl960x_bridge_vlan_write(priv, num, ports);
	if (ret)
		goto restore_filtering;

	return 0;

restore_filtering:
	if (rtl960x_vlan_set_port_filtering(priv, BIT(port), was_filtering))
		dev_warn(priv->dev, "failed to restore port %d transparency\n",
			 port);
restore_pvid:
	if (rtl960x_vlan_set_pvid(priv, port, old_pvid))
		dev_warn(priv->dev, "failed to restore port %d PVID\n", port);
	return ret;
}

/**
 * rtl960x_vlan_port_bridge_join() - VLAN side of a bridge join
 * @ds: DSA switch instance
 * @port: port index
 * @bridge: bridge being joined
 * @extack: netlink extended ACK for error reporting
 *
 * Takes the port out of the standalone VLAN 0 and puts it on the bridge's
 * reserved VID as PVID. A standalone port is transparent, so it joins as a
 * VLAN-unaware port; if the bridge is VLAN-aware, DSA's vlan_filtering
 * callback follows and makes it filtering, unless DSA already records the
 * port as filtering, in which case the join does so itself.
 *
 * Context: Can sleep. Called under rtnl.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_vlan_port_bridge_join(struct dsa_switch *ds, int port,
				  struct dsa_bridge bridge,
				  struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_vlan4k old_vlan;
	u16 ports = dsa_bridge_ports(ds, bridge.dev); /* includes port */
	u16 vid = RTL960X_BRIDGE_VID(bridge.num);
	int ret;

	ret = rtl960x_vlan_set_standalone(priv, port, false);
	if (ret)
		return ret;

	ret = rtl960x_vlan_4k_read(vid, &old_vlan);
	if (ret)
		goto restore_standalone;
	ret = rtl960x_bridge_vlan_write(priv, bridge.num, ports);
	if (ret)
		goto restore_standalone;
	ret = rtl960x_vlan_set_pvid(priv, port, vid);
	if (ret)
		goto restore_vlan;

	/*
	 * After a failed bridge-wide toggle and a leave, DSA may still record
	 * the port as filtering although it is transparent. A VLAN-aware join
	 * then gets neither DSA's filtering callback nor the upper replay, so
	 * check the uppers and enable filtering here. On a VLAN-unaware join,
	 * DSA clears its record and removes the uppers' offload itself.
	 */
	if (br_vlan_enabled(bridge.dev) &&
	    dsa_port_is_vlan_filtering(dsa_to_port(ds, port)) &&
	    rtl960x_port_unaware(priv, port)) {
		ret = rtl960x_vlan_check_bridge_uppers(ds, bridge.dev,
						       bridge.num, extack);
		if (!ret)
			ret = rtl960x_bridge_set_vlan_filtering(ds, port,
								bridge.num,
								bridge.dev,
								true);
		if (ret)
			goto restore_vlan;
	}

	return 0;

restore_vlan:
	if (rtl960x_vlan_4k_write(&old_vlan))
		dev_warn(priv->dev, "failed to restore bridge VLAN %u\n", vid);
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
 * Returns the port to the standalone VLAN 0, makes it transparent again and
 * shrinks the bridge's reserved VLAN accordingly.
 *
 * Context: Can sleep. Called under rtnl.
 */
void rtl960x_vlan_port_bridge_leave(struct dsa_switch *ds, int port,
				    struct dsa_bridge bridge)
{
	struct rtl960x_dsa *priv = ds->priv;
	u16 ports;
	int ret;

	/*
	 * DSA rolls back a failed join by broadcasting the leave before it
	 * clears dp->bridge, so the port is still counted; exclude it.
	 */
	ports = dsa_bridge_ports(ds, bridge.dev) & ~BIT(port);

	ret = rtl960x_vlan_set_standalone(priv, port, true);
	if (ret)
		dev_warn(priv->dev, "failed to restore port %d standalone VLAN\n",
			 port);
	/*
	 * DSA resets a leaving port's vlan_filtering only if its bridge is
	 * VLAN-aware; a port left filtering by a bridge toggle that failed on
	 * a later port would otherwise stay filtering as a standalone port.
	 */
	ret = rtl960x_vlan_set_port_filtering(priv, BIT(port), false);
	if (ret)
		dev_warn(priv->dev, "failed to make port %d transparent\n",
			 port);

	priv->bridge_pvid[port] = 0;
	ret = rtl960x_bridge_vlan_write(priv, bridge.num, ports);
	if (ret)
		dev_warn(priv->dev, "failed to update bridge VLAN %u\n",
			 RTL960X_BRIDGE_VID(bridge.num));
}

/**
 * rtl960x_port_vlan_filtering() - toggle VLAN filtering on a port
 * @ds: DSA switch instance
 * @port: port index
 * @vlan_filtering: new state
 * @extack: netlink extended ACK for error reporting
 *
 * A filtering port is a member of the VLANs DSA added, follows the untag
 * mask and drops ingress VIDs it is not a member of; a non-filtering port
 * is transparent; a bridged one also moves between the bridge's reserved
 * VID and its configured PVID. Ingress VLAN filtering remains disabled on
 * the CPU ports.
 *
 * Turning filtering on is refused while any port of the bridge has an
 * 802.1Q upper that cannot be offloaded (see
 * rtl960x_vlan_check_bridge_uppers()); the fallback DSA makes when the
 * replay fails for another reason is honoured.
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
	struct net_device *br;
	unsigned int num;
	int ret;

	dev_dbg(priv->dev, "port %d: %s VLAN filtering\n", port,
		vlan_filtering ? "enable" : "disable");

	if (dsa_port_is_cpu(dp))
		return 0;

	num = dsa_port_bridge_num_get(dp);
	if (!num)
		return rtl960x_vlan_set_port_filtering(priv, BIT(port),
						       vlan_filtering);

	br = dsa_port_bridge_dev_get(dp);
	if (vlan_filtering) {
		ret = rtl960x_vlan_check_bridge_uppers(ds, br, num, extack);
		if (ret)
			return ret;
	}

	return rtl960x_bridge_set_vlan_filtering(ds, port, num, br,
						 vlan_filtering);
}

/*
 * Add or remove @port in the configured membership of @vlan->vid and
 * rewrite the table entry. On return @member holds the resulting configured
 * user-port membership, so the caller knows when the VID was released.
 */
static int rtl960x_vlan_4k_port_set(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_vlan *vlan,
				    bool include, u16 *member)
{
	struct rtl960x_dsa *priv = ds->priv;
	u16 old_member, old_untag;
	int ret;

	dev_dbg(priv->dev, "%s VLAN %d 4K on port %d\n",
		include ? "add" : "del", vlan->vid, port);

	/*
	 * The CPU ports are members of every VLAN that has user ports, and
	 * the host-VLAN notifier only tracks the CPU port's own reference, so
	 * there is nothing to record for them.
	 */
	if (dsa_is_cpu_port(ds, port)) {
		if (member)
			*member = priv->vlan_member[vlan->vid];
		return 0;
	}

	old_member = priv->vlan_member[vlan->vid];
	old_untag = priv->vlan_untag[vlan->vid];

	if (include) {
		priv->vlan_member[vlan->vid] |= BIT(port);
		if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
			priv->vlan_untag[vlan->vid] |= BIT(port);
		else
			priv->vlan_untag[vlan->vid] &= ~BIT(port);
	} else {
		priv->vlan_member[vlan->vid] &= ~BIT(port);
		priv->vlan_untag[vlan->vid] &= ~BIT(port);
	}

	ret = rtl960x_vlan_4k_sync(priv, vlan->vid);
	if (ret) {
		dev_err(priv->dev, "Failed to write VLAN 4k table\n");
		priv->vlan_member[vlan->vid] = old_member;
		priv->vlan_untag[vlan->vid] = old_untag;
		return ret;
	}

	if (member)
		*member = priv->vlan_member[vlan->vid];

	return 0;
}

/**
 * rtl960x_port_vlan_add() - add a port to a VLAN
 * @ds: DSA switch instance
 * @port: port index
 * @vlan: switchdev VLAN object with the target VID and flags
 * @extack: netlink extended ACK for error reporting
 *
 * Programs the port's PVID with %BRIDGE_VLAN_INFO_PVID and the VLAN table
 * entry. Re-adding the port's PVID VLAN without the flag moves the PVID back
 * to the fallback, as mt7530 does. A VID reserved for the driver, or already
 * offloaded for another bridge, is refused with %-EOPNOTSUPP so the bridge
 * keeps it in software, as mv88e6xxx does. So is any other VID on a user
 * port outside a bridge: with this driver DSA keeps the 802.1Q uppers of a
 * standalone port in software, and the VLAN named may belong to a bridge.
 *
 * DSA also adds the VLANs of a VLAN-unaware bridge. They are recorded but
 * inactive: the port keeps the bridge's reserved VID as PVID and is
 * transparent, so the membership and untag bits only take effect once the
 * port turns VLAN-filtering.
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
	struct rtl960x_dsa *priv = ds->priv;
	bool user_port = !dsa_is_cpu_port(ds, port);
	bool unaware = false;
	unsigned int num = 0;
	u16 old_pvid = 0, new_pvid = 0;
	bool change_pvid = false;
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

	/*
	 * The top of the table holds the per-bridge VIDs. Like mv88e6xxx,
	 * refuse with -EOPNOTSUPP so the bridge keeps such a VLAN in software.
	 */
	if (vlan->vid >= RTL960X_BRIDGE_VID_MIN) {
		NL_SET_ERR_MSG_MOD(extack, "VID is reserved for driver use");
		return -EOPNOTSUPP;
	}

	if (user_port) {
		num = dsa_port_bridge_num_get(dsa_to_port(ds, port));
		if (!num) {
			NL_SET_ERR_MSG_MOD(extack,
					   "VLAN offload needs a bridge");
			return -EOPNOTSUPP;
		}
	}

	/* The VLAN's membership belongs to the bridge that owns the VID. */
	if (num && priv->vlan_owner[vlan->vid] &&
	    priv->vlan_owner[vlan->vid] != num) {
		NL_SET_ERR_MSG_MOD(extack, "VID is already used by another bridge");
		return -EOPNOTSUPP;
	}

	if (num)
		unaware = rtl960x_port_unaware(priv, port);

	/* A VLAN-unaware port keeps the bridge's reserved VID as PVID. */
	if (user_port && pvid && !unaware) {
		change_pvid = true;
		new_pvid = vlan->vid;
	} else if (user_port && !pvid && priv->pvid[port] == vlan->vid) {
		/* The PVID flag was cleared on a VLAN already offloaded as PVID. */
		change_pvid = true;
		new_pvid = rtl960x_vlan_pvid_fallback(priv, port, num);
	}
	if (change_pvid) {
		old_pvid = priv->pvid[port];
		ret = rtl960x_vlan_set_pvid(priv, port, new_pvid);
		if (ret)
			return ret;
	}

	ret = rtl960x_vlan_4k_port_set(ds, port, vlan, true, NULL);
	if (ret)
		goto undo_change_pvid;

	if (num) {
		if (pvid)
			priv->bridge_pvid[port] = vlan->vid;
		else if (priv->bridge_pvid[port] == vlan->vid)
			priv->bridge_pvid[port] = 0;
		priv->vlan_owner[vlan->vid] = num;
	}

	return 0;

undo_change_pvid:
	if (change_pvid && rtl960x_vlan_set_pvid(priv, port, old_pvid))
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
		old_pvid = priv->pvid[port];
		ret = rtl960x_vlan_set_pvid(priv, port,
					    rtl960x_vlan_pvid_fallback(priv, port,
								       num));
		if (ret)
			return ret;
	}

	ret = rtl960x_vlan_4k_port_set(ds, port, vlan, false, &member);
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
