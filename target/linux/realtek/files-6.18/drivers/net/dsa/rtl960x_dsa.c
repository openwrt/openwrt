// SPDX-License-Identifier: GPL-2.0-only
/*
 * DSA switch driver for the Realtek RTL9607C family switch core.
 *
 * The RTL9607C family of SoCs has an embedded 11 port switch controller. It
 * exposes 4 internal GPHYs for the user facing ports. It also has 2 HSGMII
 * interfaces that can be connected to external PHYs, 2 interfaces for CPU
 * ports and one SerDes interface to operate with external PON transceiver.
 */

#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/iopoll.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <net/dsa.h>
#include <net/switchdev.h>

#include "rtl960x_dsa.h"

/* Driver configuration */
#define RTL960X_DEFAULT_VID	1
#define MDIO_TIMEOUT_US		10000
#define SWCORE_L2_FLUSH_TIMEOUT_US	100000
#define SWCORE_TBL_ACCESS_TIMEOUT_US	10000

/*
 * Per-port standalone VLAN. A standalone (non-bridged) user port must exchange
 * untagged frames with the CPU: the rtl_otto tagger carries the source port in
 * the descriptor trailer, not in an 802.1Q tag, and an L3 netdev on the port
 * cannot handle a VLAN tag. But the CPU port has to be a *tagged* member of any
 * VLAN-aware bridge's VLANs (so the per-port netdev can demux them), and a
 * single CPU untag bit per VID cannot satisfy both. Give each standalone port
 * its own reserved VID with the CPU as an untagged member, used as the port's
 * PVID, so its untagged traffic reaches the CPU untagged without colliding with
 * the bridge VLANs (which keep the CPU tagged). Bridge join/leave switches the
 * port between this VID and the bridge-managed VLANs.
 */
#define RTL960X_STANDALONE_VID(p)	(RTL960X_NUM_VLANS - RTL960X_NUM_PORTS + (p))

struct rtl960x_dsa {
	struct dsa_switch *ds;
	struct device *dev;
	struct regmap *map;
	struct mii_bus *mbus;
	struct mutex l2_lock;		/* serialize shared L2 table/flush engines */
	u16 pvid[RTL960X_NUM_PORTS];	/* shadow of each port's PVID */
	/* VIDs whose table entry DSA has programmed (vs the setup defaults) */
	DECLARE_BITMAP(vlan_managed, RTL960X_NUM_VLANS);
};

/* Map phy register to OCP address */
static int rtl960x_mdio_ocp(int reg)
{
	if (reg < 16 || (reg >= 24 && reg < 30))
		return PHY_OCP_ADDR_PHYREG_BASE + reg * 2;
	return -EINVAL;		/* 16..23 are paged; this bus has no page state */
}

static int rtl960x_mdio_wait(struct rtl960x_dsa *priv)
{
	u32 v;

	return regmap_read_poll_timeout(priv->map, SWCORE_GPHY_IND_RD, v, !(v & MDIO_STS_BUSY),
				 1, MDIO_TIMEOUT_US);
}

static int rtl960x_mdio_read(struct mii_bus *bus, int addr, int reg)
{
	struct rtl960x_dsa *priv = bus->priv;
	int ocp = rtl960x_mdio_ocp(reg);
	int ret;
	u32 v;

	if (ocp < 0 || addr >= SWCORE_MAX_PHY_PORT)
		return 0xffff;		/* unsupported reg: read as all-ones */

	regmap_write(priv->map, SWCORE_GPHY_IND_CMD,
		     MDIO_CMD_EN | MDIO_CMD_READ |
		     FIELD_PREP(MDIO_CMD_PHYID, addr) | ocp);
	ret = rtl960x_mdio_wait(priv);
	if (ret)
		return ret;

	ret = regmap_read(priv->map, SWCORE_GPHY_IND_RD, &v);
	if (ret)
		return ret;

	return MDIO_STS_DATA(v);
}

static int rtl960x_mdio_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct rtl960x_dsa *priv = bus->priv;
	int ocp = rtl960x_mdio_ocp(reg);

	if (ocp < 0 || addr >= SWCORE_MAX_PHY_PORT)
		return 0;		/* unsupported reg: ignore the write */

	regmap_update_bits(priv->map, SWCORE_GPHY_IND_WD, 0xffff, val);

	regmap_write(priv->map, SWCORE_GPHY_IND_CMD,
		     MDIO_CMD_EN | MDIO_CMD_WRITE |
		     FIELD_PREP(MDIO_CMD_PHYID, addr) | ocp);

	return rtl960x_mdio_wait(priv);
}

/*
 * One-time switch-core PHY/port bring-up, recovered from the U-Boot "apro"
 * recipe: trigger the GPHY analog patch and force the CPU-port MAC link. The
 * analog patch must run before the MDIO bus is registered; without it PHY ID
 * reads return garbage. The user PHYs are left for phylib to configure. The
 * soc_init_rdy line on the switch node is deasserted by the syscon core when
 * the regmap is created, so it is already released before any switch-core
 * access.
 */
static void rtl960x_switch_bringup(struct rtl960x_dsa *priv)
{
	regmap_write(priv->map, SWCORE_WRAP_GPHY_MISC, 0x1);
	msleep(800);

	regmap_write(priv->map, SWCORE_FORCE_P_ABLTY(RTL960X_CPU_PORT),
		     FORCE_P_ABLTY_NWAY | FORCE_P_ABLTY_LINK |
		     FORCE_P_ABLTY_DUPLEX | FORCE_P_ABLTY_SPEED_1000);
	regmap_write(priv->map, SWCORE_ABLTY_FORCE_MODE(RTL960X_CPU_PORT), 0xffff);
}

static int rtl960x_mdio_register(struct rtl960x_dsa *priv)
{
	struct device_node *mnp;
	int ret;

	mnp = of_get_child_by_name(priv->dev->of_node, "mdio");
	if (!mnp)
		return 0;		/* no MDIO: ports stay fixed-link */

	priv->mbus = devm_mdiobus_alloc(priv->dev);
	if (!priv->mbus) {
		of_node_put(mnp);
		return -ENOMEM;
	}

	priv->mbus->name = "rtl960x-mdio";
	priv->mbus->read = rtl960x_mdio_read;
	priv->mbus->write = rtl960x_mdio_write;
	priv->mbus->priv = priv;
	priv->mbus->parent = priv->dev;
	snprintf(priv->mbus->id, MII_BUS_ID_SIZE, "rtl9607c-phys");

	ret = devm_of_mdiobus_register(priv->dev, priv->mbus, mnp);
	of_node_put(mnp);
	if (ret)
		return ret;

	priv->ds->user_mii_bus = priv->mbus;

	return 0;
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
static void rtl960x_dsa_port_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;
	u32 v;

	mutex_lock(&priv->l2_lock);

	regmap_write(priv->map, SWCORE_L2_FLUSH_CTRL,
		     SWCORE_L2_FLUSH_MODE_PORT | SWCORE_L2_FLUSH_DYNAMIC);
	regmap_write(priv->map, SWCORE_L2_FLUSH_EN, BIT(port));

	ret = regmap_read_poll_timeout(priv->map, SWCORE_L2_FLUSH_CTRL, v,
				       !(v & SWCORE_L2_FLUSH_BUSY), 10,
				       SWCORE_L2_FLUSH_TIMEOUT_US);

	mutex_unlock(&priv->l2_lock);

	if (ret)
		dev_warn(priv->dev, "L2 flush timed out on port %d\n", port);
}

static void rtl960x_dsa_port_stp_state_set(struct dsa_switch *ds, int port,
					   u8 state)
{
	struct rtl960x_dsa *priv = ds->priv;
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

	regmap_update_bits(priv->map, SWCORE_MSTI_CTRL(port),
			   SWCORE_MSTI_STATE_MASK(0), hw);
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

	regmap_write(priv->map, SWCORE_TBL_ACCESS_WR_DATA(0), word);
	regmap_write(priv->map, SWCORE_TBL_ACCESS_CTRL,
		     SWCORE_TBL_ACCESS_ADDR(vid) |
		     SWCORE_TBL_ACCESS_CMD(SWCORE_TBL_ACCESS_CMD_WRITE) |
		     SWCORE_TBL_ACCESS_TYPE_VLAN);
	ret = rtl960x_tbl_busy_wait(priv);

	mutex_unlock(&priv->l2_lock);

	return ret;
}

static int rtl960x_vlan_tbl_read(struct rtl960x_dsa *priv, u16 vid, u32 *word)
{
	int ret;

	mutex_lock(&priv->l2_lock);

	regmap_write(priv->map, SWCORE_TBL_ACCESS_CTRL,
		     SWCORE_TBL_ACCESS_ADDR(vid) |
		     SWCORE_TBL_ACCESS_CMD(SWCORE_TBL_ACCESS_CMD_READ) |
		     SWCORE_TBL_ACCESS_TYPE_VLAN);
	ret = rtl960x_tbl_busy_wait(priv);
	if (ret)
		goto out;

	ret = regmap_read(priv->map, SWCORE_TBL_ACCESS_RD_DATA(0), word);

out:
	mutex_unlock(&priv->l2_lock);

	return ret;
}

/* Replace a VID's member and untagged port masks. */
static int rtl960x_vlan_set_masks(struct rtl960x_dsa *priv, u16 vid,
				  u32 member, u32 untag)
{
	u32 word = (member & VLAN_MBR_MASK) |
		   ((untag & VLAN_MBR_MASK) << VLAN_UNTAG_SHIFT);

	return rtl960x_vlan_tbl_write(priv, vid, word);
}

static int rtl960x_vlan_get_masks(struct rtl960x_dsa *priv, u16 vid,
				  u32 *member, u32 *untag)
{
	u32 word;
	int ret;

	ret = rtl960x_vlan_tbl_read(priv, vid, &word);
	if (ret)
		return ret;

	*member = word & VLAN_MBR_MASK;
	*untag = (word >> VLAN_UNTAG_SHIFT) & VLAN_MBR_MASK;
	return 0;
}

static void rtl960x_set_pvid(struct rtl960x_dsa *priv, int port, u16 vid)
{
	u32 shift = (port % 2) * SWCORE_VLAN_PVID_BITS;
	u32 reg = SWCORE_VLAN_PB_VID + (port / 2) * 4;

	regmap_update_bits(priv->map, reg, SWCORE_VLAN_PVID_MASK << shift,
			   vid << shift);

	priv->pvid[port] = vid;
}

/*
 * Bring the VLAN engine up with a transparent default: every port is an
 * untagged member of the default VLAN with that VID as its PVID and ingress
 * filtering off, so until DSA programs real VLANs the switch forwards exactly
 * as in the VLAN-unaware case (forwarding is still gated by the bridge/PISO
 * isolation masks). DSA then drives per-port filtering and membership --
 * the same model as the rtl83xx DSA driver.
 */
static void rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	u32 all = 0;
	int p, v;

	for (p = 0; p < ds->num_ports; p++)
		if (dsa_is_user_port(ds, p) || dsa_is_cpu_port(ds, p))
			all |= BIT(p);

	/*
	 * Unconfigured VLAN entries power on with every port as a member (flat
	 * forwarding). Clear them so a VID only reaches ports DSA explicitly
	 * adds; otherwise port_vlan_add's read-modify-write would keep the
	 * all-ones default and never isolate VLANs.
	 */
	for (v = 1; v < RTL960X_NUM_VLANS; v++)
		rtl960x_vlan_set_masks(priv, v, 0, 0);

	rtl960x_vlan_set_masks(priv, RTL960X_DEFAULT_VID, all, all);

	/* No ingress filtering yet; egress follows the per-VID untag mask. */
	regmap_write(priv->map, SWCORE_VLAN_INGRESS, 0);
	for (p = 0; p < ds->num_ports; p++) {
		if (!dsa_is_user_port(ds, p) && !dsa_is_cpu_port(ds, p))
			continue;
		regmap_write(priv->map, SWCORE_VLAN_EGR_TAG(p), SWCORE_VLAN_EGR_MODE_ORI);
		rtl960x_set_pvid(priv, p, RTL960X_DEFAULT_VID);
	}

	regmap_set_bits(priv->map, SWCORE_VLAN_CTRL, SWCORE_VLAN_CTRL_FILTER_EN);
}

/*
 * Put a user port into standalone mode: program its reserved per-port VID with
 * the port and the CPU port as untagged members, and make it the port's PVID.
 * Untagged ingress is then tagged internally with this VID and egresses the CPU
 * untagged, so an L3 netdev on the port sees plain (untagged) frames.
 */
static void rtl960x_port_setup_standalone(struct dsa_switch *ds, int port)
{
	u32 mask = BIT(port) | BIT(RTL960X_CPU_PORT);
	u16 vid = RTL960X_STANDALONE_VID(port);
	struct rtl960x_dsa *priv = ds->priv;

	rtl960x_vlan_set_masks(priv, vid, mask, mask);
	rtl960x_set_pvid(priv, port, vid);
}

static int rtl960x_dsa_port_vlan_filtering(struct dsa_switch *ds, int port,
					   bool vlan_filtering,
					   struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;

	/*
	 * Toggle per-port ingress VLAN filtering. The CPU port is left
	 * unfiltered so CPU-bound and CPU-injected frames are never dropped.
	 */
	if (dsa_is_cpu_port(ds, port))
		return 0;

	regmap_assign_bits(priv->map, SWCORE_VLAN_INGRESS, BIT(port), vlan_filtering);

	return 0;
}

static int rtl960x_dsa_port_vlan_add(struct dsa_switch *ds, int port,
				     const struct switchdev_obj_port_vlan *vlan,
				     struct netlink_ext_ack *extack)
{
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	struct rtl960x_dsa *priv = ds->priv;
	u32 member, untag;
	int ret;

	if (!vlan->vid)
		return 0;

	/* The top of the table is reserved for the standalone-port VIDs. */
	if (vlan->vid >= RTL960X_STANDALONE_VID(0)) {
		NL_SET_ERR_MSG_MOD(extack, "VID is reserved for standalone ports");
		return -EBUSY;
	}

	/*
	 * Entries DSA has not programmed yet still hold the setup defaults
	 * (the transparent default VLAN); start those from an empty entry
	 * instead of inheriting their member/untag masks.
	 */
	if (test_bit(vlan->vid, priv->vlan_managed)) {
		ret = rtl960x_vlan_get_masks(priv, vlan->vid, &member, &untag);
		if (ret)
			return ret;
	} else {
		member = 0;
		untag = 0;
	}

	member |= BIT(port);
	if (untagged)
		untag |= BIT(port);
	else
		untag &= ~BIT(port);

	/*
	 * This kernel has no separate host-VLAN callback, so keep the CPU port
	 * a (tagged) member of every VLAN that has user members, so VLAN traffic
	 * destined to the CPU (the bridge / routing) is forwarded there.
	 */
	member |= BIT(RTL960X_CPU_PORT);

	ret = rtl960x_vlan_set_masks(priv, vlan->vid, member, untag);
	if (ret)
		return ret;

	set_bit(vlan->vid, priv->vlan_managed);

	/* The CPU port keeps the default PVID (tagging is driven by DSA). */
	if (!dsa_is_cpu_port(ds, port) && pvid)
		rtl960x_set_pvid(priv, port, vlan->vid);

	return 0;
}

static int rtl960x_dsa_port_vlan_del(struct dsa_switch *ds, int port,
				     const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl960x_dsa *priv = ds->priv;
	u32 member, untag;
	int ret;

	if (!vlan->vid)
		return 0;

	/* Only entries DSA programmed can be deleted; leave the defaults be. */
	if (!test_bit(vlan->vid, priv->vlan_managed))
		return 0;

	ret = rtl960x_vlan_get_masks(priv, vlan->vid, &member, &untag);
	if (ret)
		return ret;

	member &= ~BIT(port);
	untag &= ~BIT(port);

	/* Drop the CPU port once the VLAN has no user members left. */
	if (!(member & ~BIT(RTL960X_CPU_PORT)))
		member &= ~BIT(RTL960X_CPU_PORT);

	ret = rtl960x_vlan_set_masks(priv, vlan->vid, member, untag);
	if (ret)
		return ret;

	/* Removing the port's current PVID falls back to the default VLAN. */
	if (!dsa_is_cpu_port(ds, port) && priv->pvid[port] == vlan->vid)
		rtl960x_set_pvid(priv, port, RTL960X_DEFAULT_VID);

	return 0;
}

/*
 * L2 unicast forwarding database (FDB) offload. The hardware hashes the
 * {MAC, VLAN} key itself, so adding and deleting an entry is a single
 * MAC-method write (delete writes the same key back invalid); a dump walks the
 * valid unicast entries with the NEXT_L2UC access method. All three callbacks
 * run under rtnl, which serialises them against the VLAN table accesses above.
 */
struct rtl960x_l2_entry {
	u8 mac[ETH_ALEN];
	u16 vid;		/* CVID; 0 = shared-VLAN learning (FID 0) */
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
	int p;

	/*
	 * Bring up the switch-core PHYs/ports before registering the MDIO bus:
	 * PHY ID reads only succeed after the analog patch has run.
	 */
	rtl960x_switch_bringup(priv);

	/* No bridges yet: every user port starts isolated (CPU-only). */
	rtl960x_recalc_isolation(ds);

	/* VLAN-aware with a transparent default VLAN (see rtl960x_vlan_setup). */
	rtl960x_vlan_setup(ds);

	/* Every user port starts standalone: untagged traffic to/from the CPU. */
	for (p = 0; p < ds->num_ports; p++)
		if (dsa_is_user_port(ds, p))
			rtl960x_port_setup_standalone(ds, p);

	/* Program VLANs even before a bridge turns on VLAN filtering. */
	ds->configure_vlan_while_not_filtering = true;

	return rtl960x_mdio_register(priv);
}

static int rtl960x_dsa_port_bridge_join(struct dsa_switch *ds, int port,
					struct dsa_bridge bridge,
					bool *tx_fwd_offload,
					struct netlink_ext_ack *extack)
{
	/* Open HW forwarding among the ports of this bridge. */
	rtl960x_recalc_isolation(ds);

	return 0;
}

static void rtl960x_dsa_port_bridge_leave(struct dsa_switch *ds, int port,
					  struct dsa_bridge bridge)
{
	/* The port is already unbridged here; reflect the new membership. */
	rtl960x_recalc_isolation(ds);

	/* Back to standalone: untagged exchange with the CPU on its own VID. */
	rtl960x_port_setup_standalone(ds, port);
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
