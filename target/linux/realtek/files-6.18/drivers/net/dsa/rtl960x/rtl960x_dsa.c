// SPDX-License-Identifier: GPL-2.0-only
/*
 * DSA switch driver for the Realtek RTL9607C family switch core.
 *
 * The RTL9607C family of SoCs has an embedded 11 port switch controller. It
 * exposes 4 internal GPHYs for the user facing ports. It also has 2 HSGMII
 * interfaces that can be connected to external PHYs, 2 interfaces for CPU
 * ports and one SerDes interface to operate with external PON transceiver.
 */

#include <linux/bitfield.h>
#include <linux/if_bridge.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <net/dsa.h>

#include "rtl960x_dsa.h"
#include "rtl960x_l2.h"
#include "rtl960x_vlan.h"

/* Forced MAC ability of a port, used to force the CPU ports' link up */
#define RTL960X_FORCE_P_ABLTY_REG(_port)	(0x1cc + (_port) * 4)
#define   RTL960X_FORCE_P_ABLTY_NWAY		BIT(7)
#define   RTL960X_FORCE_P_ABLTY_LINK		BIT(4)
#define   RTL960X_FORCE_P_ABLTY_DUPLEX		BIT(2)
#define   RTL960X_FORCE_P_ABLTY_SPEED_MASK	GENMASK(1, 0)
#define   RTL960X_FORCE_P_ABLTY_SPEED_1000	2
#define RTL960X_ABLTY_FORCE_MODE_REG(_port)	(0x238 + (_port) * 4)

/*
 * Port isolation registers - one egress portmask per port. Bits 28:11 are
 * the extension ports of MAC7/9/10, which the driver leaves clear.
 */
#define RTL960X_PORT_ISOLATION_REG(_port)	(0x27000 + (_port) * 4)
#define   RTL960X_PORT_ISOLATION_MASK		GENMASK(10, 0)

/* MSTP port state registers - one per port, a 2-bit state per MSTI */
#define RTL960X_MSTI_CTRL_REG(_port)		(0x1704c + (_port) * 4)
#define   RTL960X_MSTI_CTRL_PORT_STATE_OFFSET(_msti)	((_msti) << 1)
#define   RTL960X_MSTI_CTRL_PORT_STATE_MASK(_msti) \
		(0x3 << RTL960X_MSTI_CTRL_PORT_STATE_OFFSET(_msti))

enum rtl960x_stp_state {
	RTL960X_STP_STATE_DISABLED = 0,
	RTL960X_STP_STATE_BLOCKING = 1,
	RTL960X_STP_STATE_LEARNING = 2,
	RTL960X_STP_STATE_FORWARDING = 3,
};

static int rtl960x_cpu_port_setup(struct rtl960x_dsa *priv, int port)
{
	int ret;

	/* Force the link to the GMAC up at 1000/full, as the SDK does */
	ret = regmap_write(priv->map, RTL960X_FORCE_P_ABLTY_REG(port),
			   RTL960X_FORCE_P_ABLTY_NWAY |
			   RTL960X_FORCE_P_ABLTY_LINK |
			   RTL960X_FORCE_P_ABLTY_DUPLEX |
			   FIELD_PREP(RTL960X_FORCE_P_ABLTY_SPEED_MASK,
				      RTL960X_FORCE_P_ABLTY_SPEED_1000));
	if (ret)
		return ret;

	return regmap_write(priv->map, RTL960X_ABLTY_FORCE_MODE_REG(port),
			    0xffff);
}

static int rtl960x_port_set_isolation(struct rtl960x_dsa *priv, int port,
				      u32 mask)
{
	return regmap_write(priv->map, RTL960X_PORT_ISOLATION_REG(port), mask);
}

static int rtl960x_port_add_isolation(struct rtl960x_dsa *priv, int port,
				      u32 mask)
{
	return regmap_update_bits(priv->map, RTL960X_PORT_ISOLATION_REG(port),
				  mask, mask);
}

static int rtl960x_port_remove_isolation(struct rtl960x_dsa *priv, int port,
					 u32 mask)
{
	return regmap_update_bits(priv->map, RTL960X_PORT_ISOLATION_REG(port),
				  mask, 0);
}

static void rtl960x_port_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;

	ret = rtl960x_l2_flush(priv, port);
	if (ret)
		dev_err(priv->dev, "failed to fast age on port %d: %pe\n",
			port, ERR_PTR(ret));
}

static void rtl960x_port_stp_state_set(struct dsa_switch *ds, int port,
				       u8 state)
{
	struct rtl960x_dsa *priv = ds->priv;
	enum rtl960x_stp_state val;
	int msti;

	switch (state) {
	case BR_STATE_DISABLED:
		val = RTL960X_STP_STATE_DISABLED;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		val = RTL960X_STP_STATE_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		val = RTL960X_STP_STATE_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		val = RTL960X_STP_STATE_FORWARDING;
		break;
	default:
		dev_err(priv->dev, "invalid STP state: %u\n", state);
		return;
	}

	/*
	 * A VLAN entry's FID field doubles as its MSTI: standalone ports and
	 * VLAN-aware bridges use FID 0, VLAN-unaware bridges FID 1. MST is not
	 * offloaded, so mirror the port's single STP state into every MSTI.
	 */
	for (msti = 0; msti < RTL960X_NUM_MSTI; msti++)
		regmap_update_bits(priv->map, RTL960X_MSTI_CTRL_REG(port),
				   RTL960X_MSTI_CTRL_PORT_STATE_MASK(msti),
				   val << RTL960X_MSTI_CTRL_PORT_STATE_OFFSET(msti));
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
	u32 upports_mask = 0, downports_mask = 0;
	struct dsa_port *dp;
	int ret;

	/* Start with all ports completely isolated, including unused ports */
	dsa_switch_for_each_port(dp, ds) {
		ret = rtl960x_port_set_isolation(priv, dp->index, 0);
		if (ret)
			return ret;

		if (dsa_port_is_cpu(dp))
			upports_mask |= BIT(dp->index);
	}

	/* User ports forward only to the CPU ports */
	dsa_switch_for_each_user_port(dp, ds) {
		ret = rtl960x_port_set_isolation(priv, dp->index, upports_mask);
		if (ret)
			return ret;

		downports_mask |= BIT(dp->index);
	}

	/* CPU ports forward to every user port */
	dsa_switch_for_each_cpu_port(dp, ds) {
		ret = rtl960x_cpu_port_setup(priv, dp->index);
		if (ret)
			return ret;

		ret = rtl960x_port_set_isolation(priv, dp->index,
						 downports_mask);
		if (ret)
			return ret;
	}

	/* One reserved VID per offloaded bridge; DSA numbers them for us. */
	ds->max_num_bridges = RTL960X_NUM_BRIDGE_VIDS;

	/* Every user port starts standalone (see rtl960x_vlan_setup). */
	return rtl960x_vlan_setup(ds);
}

static int rtl960x_port_bridge_join(struct dsa_switch *ds, int port,
				    struct dsa_bridge bridge,
				    bool *tx_fwd_offload,
				    struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct dsa_port *dp;
	u32 mask = 0;
	int ret;

	dev_dbg(priv->dev, "bridge %d join port %d\n", bridge.num, port);

	/*
	 * Add this port to the isolation group of every other port
	 * offloading this bridge.
	 */
	dsa_switch_for_each_user_port(dp, ds) {
		/* Handle this port after */
		if (dp->index == port)
			continue;

		/* Skip ports that are not in this bridge */
		if (!dsa_port_offloads_bridge(dp, &bridge))
			continue;

		ret = rtl960x_port_add_isolation(priv, dp->index, BIT(port));
		if (ret)
			goto undo_isolation;

		mask |= BIT(dp->index);
	}

	/* Add those ports to the isolation group of this port */
	ret = rtl960x_port_add_isolation(priv, port, mask);
	if (ret)
		goto undo_isolation;

	/* Leave VLAN 0 and follow the bridge's VLAN configuration */
	ret = rtl960x_vlan_port_bridge_join(ds, port, bridge);
	if (ret)
		goto undo_self_isolation;

	return 0;

undo_self_isolation:
	rtl960x_port_remove_isolation(priv, port, mask);

undo_isolation:
	dsa_switch_for_each_user_port(dp, ds) {
		if (mask & BIT(dp->index))
			rtl960x_port_remove_isolation(priv, dp->index,
						      BIT(port));
	}

	return ret;
}

static void rtl960x_port_bridge_leave(struct dsa_switch *ds, int port,
				      struct dsa_bridge bridge)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct dsa_port *dp;
	u32 mask = 0;
	int ret;

	dev_dbg(priv->dev, "bridge %d leave port %d\n", bridge.num, port);

	/*
	 * Remove this port from the isolation group of every other port
	 * offloading this bridge.
	 */
	dsa_switch_for_each_user_port(dp, ds) {
		/* Handle this port after */
		if (dp->index == port)
			continue;

		/* Skip ports that are not in this bridge */
		if (!dsa_port_offloads_bridge(dp, &bridge))
			continue;

		ret = rtl960x_port_remove_isolation(priv, dp->index, BIT(port));
		if (ret)
			dev_err(priv->dev,
				"failed to remove port %d from isolation group of port %d: %pe\n",
				port, dp->index, ERR_PTR(ret));

		mask |= BIT(dp->index);
	}

	/* Remove those ports from the isolation group of this port */
	ret = rtl960x_port_remove_isolation(priv, port, mask);
	if (ret)
		dev_err(priv->dev,
			"failed to remove isolation group of port %d: %pe\n",
			port, ERR_PTR(ret));

	/*
	 * The bridge disables the port before it leaves, which fast-ages it,
	 * but a join that DSA rolls back leaves without an STP transition.
	 * Flush here so nothing learned in the bridge's database stays behind.
	 */
	rtl960x_port_fast_age(ds, port);

	/* Back to standalone on VLAN 0 */
	rtl960x_vlan_port_bridge_leave(ds, port, bridge);
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
	.port_bridge_join	= rtl960x_port_bridge_join,
	.port_bridge_leave	= rtl960x_port_bridge_leave,
	.port_stp_state_set	= rtl960x_port_stp_state_set,
	.port_fast_age		= rtl960x_port_fast_age,
	.port_vlan_filtering	= rtl960x_port_vlan_filtering,
	.port_vlan_add		= rtl960x_port_vlan_add,
	.port_vlan_del		= rtl960x_port_vlan_del,
	.port_fdb_add		= rtl960x_port_fdb_add,
	.port_fdb_del		= rtl960x_port_fdb_del,
	.port_fdb_dump		= rtl960x_port_fdb_dump,
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

	ret = devm_mutex_init(dev, &priv->table_lock);
	if (ret)
		return ret;

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
