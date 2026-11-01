// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/ethtool.h>
#include <linux/if_vlan.h>
#include <linux/of.h>
#include <linux/pcs/pcs.h>
#include <linux/phylink.h>
#include <linux/regmap.h>
#include <net/dsa.h>

#include "mac.h"
#include "rtl-otto.h"

/* Both length fields are 14 bits wide (hardware maximum 16383 bytes). */
#define RTLDSA_MAC_MAX_LEN_FIELD		GENMASK(13, 0)
/* Mask of both length fields, leaving the tag-inclusion bit (28) untouched. */
#define RTLDSA_MAC_MAX_LEN_MASK \
	(RTLDSA_MAC_MAX_LEN_FIELD | (RTLDSA_MAC_MAX_LEN_FIELD << 14))
/* Encode @len into both the high-speed and the 10/100M length field. */
#define RTLDSA_MAC_MAX_LEN_VAL(len) \
	(((len) & RTLDSA_MAC_MAX_LEN_FIELD) | (((len) & RTLDSA_MAC_MAX_LEN_FIELD) << 14))

/* MAC link state bits */
#define RTL_SPEED_10				0
#define RTL_SPEED_100				1
#define RTL_SPEED_1000				2
#define RTL_SPEED_2500				5
#define RTL_SPEED_5000				6
#define RTL_SPEED_10000				4

#define RTL838X_NWAY_EN				BIT(2)

/* Ethernet header, two stacked VLAN tags (802.1ad QinQ) and FCS */
#define RTLDSA_FRAME_OVERHEAD		(ETH_HLEN + 2 * VLAN_HLEN + ETH_FCS_LEN)
/* Tail tag the DSA core adds to a frame on its way to the conduit */
#define RTLDSA_TAG_OVERHEAD		4

/* Register definition */
#define RTL838X_MAC_PORT_CTRL(port)		(0xd560 + (((port) << 7)))

#define RTL838X_EEE_PORT_TX_EN			(0x014c)
#define RTL838X_EEE_PORT_RX_EN			(0x0150)
#define RTL838X_EEE_TX_TIMER_GIGA_CTRL		(0xaa04)
#define RTL838X_EEE_TX_TIMER_GELITE_CTRL	(0xaa08)

#define RTL838X_SMI_GLB_CTRL			(0xa100) /* used by RTL838x EEE setup */

#define RTL839X_MAC_PORT_CTRL(port)		(0x8004 + (((port) << 7)))

#define RTL839X_MAC_FORCE_MODE_CTRL		(0x02bc)

#define RTL839X_EEE_TX_TIMER_GELITE_CTRL	(0x042C)
#define RTL839X_EEE_TX_TIMER_GIGA_CTRL		(0x0430)
#define RTL839X_EEE_TX_TIMER_10G_CTRL		(0x0434)
#define RTL839X_EEE_CTRL(p)			(0x8008 + ((p) << 7))

#define RTL930X_MAC_L2_PORT_CTRL(port)		(0x3268 + (((port) << 6)))

/* MAC maximum packet length (jumbo frame) control.
 *
 * The switch MAC drops frames whose L2 length exceeds the configured maximum.
 * A family holds either one register per user port or a single one for the
 * whole switch. The length is a direct byte value held in two 14-bit fields
 * (high-speed links in [13:0], 10/100M links in [27:14]); bit 28 selects
 * whether VLAN tag bytes count towards the limit.
 */

/* RTL930x holds one register per user port, inside the 64-byte MAC block of
 * the port. The CPU port has a row of its own, which the ethernet driver owns
 * and programs from the conduit MTU. Register offsets taken from the
 * reverse-engineered Realtek register maps at https://svanheule.net/realtek/
 */
#define RTL930X_MAC_L2_PORT_MAX_LEN_CTRL(port)	(0x326C + (((port) << 6)))

#define RTL930X_MAC_FORCE_MODE_CTRL		(0xCA1C)

#define RTL930X_EEE_CTRL(p)			(0x3274 + ((p) << 6))

#define RTL930X_SMI_MAC_TYPE_CTRL		0xca04
#define RTL930X_SMI_MAC_TYPE_P0_23(pn)		(GENMASK(1, 0) << (((pn) / 4) * 2))
#define RTL930X_SMI_MAC_TYPE_P24_27(pn)		(GENMASK(2, 0) << (((pn) - 24) * 3 + 12))
#define RTL930X_SMI_MAC_TYPE_PCS		0x0
#define RTL930X_SMI_MAC_TYPE_MULTIGIG_PHY	0x1
#define RTL930X_SMI_MAC_TYPE_1G_PHY		0x3

#define RTL931X_MAC_L2_PORT_CTRL		(0x6000)

/* MAC maximum packet length (jumbo frame) control.
 *
 * The switch MAC drops frames whose L2 length exceeds the configured maximum.
 * A family holds either one register per user port or a single one for the
 * whole switch. The length is a direct byte value held in two 14-bit fields
 * (high-speed links in [13:0], 10/100M links in [27:14]); bit 28 selects
 * whether VLAN tag bytes count towards the limit.
 */

/* RTL931x covers ports 0 to 55 only, one word each. The CPU port has no row:
 * the word that would follow the array is MAC_DBG_SEL_CTRL.
 */
#define RTL931X_MAC_L2_PORT_MAX_LEN_CTRL	(0x5554)

#define RTL931X_MAC_FORCE_MODE_CTRL		(0x0DCC)

#define RTL931X_SMI_PHY_ABLTY_GET_SEL(port)	(0x0cac + ((port) / 16) * 4)
#define RTL931X_SMI_PHY_ABLTY_MDIO		0x0
#define RTL931X_SMI_PHY_ABLTY_SDS		0x2
#define RTL931X_SMI_PHY_ABLTY_MASK(port)	(GENMASK(1, 0) << (((port) % 16) * 2))

int rtl838x_mac_force_mode_ctrl(int p)
{
	return RTL838X_MAC_FORCE_MODE_CTRL + (p << 2);
}

int rtl838x_mac_port_ctrl(int p)
{
	return RTL838X_MAC_PORT_CTRL(p);
}

/* Enables or disables the EEE/EEEP capability of a port */
void rtldsa_838x_set_mac_eee(struct otto_dsa_priv *priv, int port, bool enable)
{
	u32 v;

	/* This works only for Ethernet ports, and on the RTL838X, ports from 24 are SFP */
	if (port >= 24)
		return;

	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0x3 : 0x0;

	/* Set EEE state for 100 (bit 9) & 1000MBit (bit 10) */
	regmap_write_bits(priv->map, priv->r->mac_force_mode_ctrl(port), GENMASK(10, 9), v << 9);

	/* Set TX/RX EEE state */
	if (enable) {
		regmap_write_bits(priv->map, RTL838X_EEE_PORT_TX_EN, BIT(port), BIT(port));
		regmap_write_bits(priv->map, RTL838X_EEE_PORT_RX_EN, BIT(port), BIT(port));
	} else {
		regmap_write_bits(priv->map, RTL838X_EEE_PORT_TX_EN, BIT(port), 0);
		regmap_write_bits(priv->map, RTL838X_EEE_PORT_RX_EN, BIT(port), 0);
	}
	priv->ports[port].eee_enabled = enable;
}

void rtl838x_init_eee(struct otto_dsa_priv *priv, bool enable)
{
	pr_debug("Setting up EEE, state: %d\n", enable);
	regmap_write_bits(priv->map, RTL838X_SMI_GLB_CTRL, BIT(2), 0);

	/* Set timers for EEE */
	regmap_write(priv->map, RTL838X_EEE_TX_TIMER_GIGA_CTRL, 0x5001411);
	regmap_write(priv->map, RTL838X_EEE_TX_TIMER_GELITE_CTRL, 0x5001417);

	/* Enable EEE MAC support on ports */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy)
			priv->r->set_mac_eee(priv, i, enable);
	}
	priv->eee_enabled = enable;
}

int rtl839x_mac_force_mode_ctrl(int p)
{
	return RTL839X_MAC_FORCE_MODE_CTRL + (p << 2);
}

int rtl839x_mac_port_ctrl(int p)
{
	return RTL839X_MAC_PORT_CTRL(p);
}

/* Enables or disables the EEE/EEEP capability of a port */
void rtldsa_839x_set_mac_eee(struct otto_dsa_priv *priv, int port, bool enable)
{
	u32 v;

	/* This works only for Ethernet ports, and on the RTL839X, ports above 47 are SFP */
	if (port >= 48)
		return;

	enable = true;
	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0xf : 0x0;

	/* Set EEE for 100, 500, 1000MBit and 10GBit */
	regmap_write_bits(priv->map, rtl839x_mac_force_mode_ctrl(port), GENMASK(11, 8), v << 8);

	/* Set TX/RX EEE state */
	v = enable ? 0x3 : 0x0;
	regmap_write(priv->map, RTL839X_EEE_CTRL(port), v);

	priv->ports[port].eee_enabled = enable;
}

void rtl839x_init_eee(struct otto_dsa_priv *priv, bool enable)
{
	pr_debug("Setting up EEE, state: %d\n", enable);

	/* Set wake timer for TX and pause timer both to 0x21 */
	regmap_write_bits(priv->map, RTL839X_EEE_TX_TIMER_GELITE_CTRL,
			  GENMASK(27, 20) | GENMASK(7, 0), 0x21 << 20 | 0x21);
	/* Set pause wake timer for GIGA-EEE to 0x11 */
	regmap_write_bits(priv->map, RTL839X_EEE_TX_TIMER_GIGA_CTRL, GENMASK(27, 20), 0x11 << 20);
	/* Set pause wake timer for 10GBit ports to 0x11 */
	regmap_write_bits(priv->map, RTL839X_EEE_TX_TIMER_10G_CTRL, GENMASK(27, 20), 0x11 << 20);

	/* Setup EEE on all ports */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy)
			priv->r->set_mac_eee(priv, i, enable);
	}
	priv->eee_enabled = enable;
}

int rtl930x_mac_force_mode_ctrl(int p)
{
	return RTL930X_MAC_FORCE_MODE_CTRL + (p << 2);
}

int rtl930x_mac_port_ctrl(int p)
{
	return RTL930X_MAC_L2_PORT_CTRL(p);
}

int rtl930x_mac_max_len_reg(int p)
{
	return RTL930X_MAC_L2_PORT_MAX_LEN_CTRL(p);
}

/* Enables or disables the EEE/EEEP capability of a port */
void rtldsa_930x_set_mac_eee(struct otto_dsa_priv *priv, int port, bool enable)
{
	u32 v;

	/* This works only for Ethernet ports, and on the RTL930X, ports from 26 are SFP */
	if (port >= 26)
		return;

	pr_debug("In %s: setting port %d to %d\n", __func__, port, enable);
	v = enable ? 0x3f : 0x0;

	/* Set EEE/EEEP state for 100, 500, 1000MBit and 2.5, 5 and 10GBit */
	regmap_write_bits(priv->map, rtl930x_mac_force_mode_ctrl(port), v << 10, v << 10);

	/* Set TX/RX EEE state */
	v = enable ? 0x3 : 0x0;
	regmap_write(priv->map, RTL930X_EEE_CTRL(port), v);

	priv->ports[port].eee_enabled = enable;
}

void rtl930x_init_eee(struct otto_dsa_priv *priv, bool enable)
{
	pr_debug("Setting up EEE, state: %d\n", enable);

	/* Setup EEE on all ports */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy)
			priv->r->set_mac_eee(priv, i, enable);
	}

	priv->eee_enabled = enable;
}

int
rtldsa_930x_mac_link_state_source_set(struct otto_dsa_priv *priv,
				      int port,
				      enum rtldsa_mac_link_state_source source,
				      phy_interface_t interface)
{
	u32 mask, src_val;

	if (port < 24)
		mask = RTL930X_SMI_MAC_TYPE_P0_23(port);
	else if (port < 28)
		mask = RTL930X_SMI_MAC_TYPE_P24_27(port);
	else
		return 0;

	if (source == RTLDSA_MAC_LINK_STATE_SOURCE_PCS)
		src_val = RTL930X_SMI_MAC_TYPE_PCS;
	else if (interface == PHY_INTERFACE_MODE_2500BASEX ||
		 interface == PHY_INTERFACE_MODE_10GBASER ||
		 interface == PHY_INTERFACE_MODE_USXGMII ||
		 interface == PHY_INTERFACE_MODE_10G_QXGMII)
		src_val = RTL930X_SMI_MAC_TYPE_MULTIGIG_PHY;
	else
		src_val = RTL930X_SMI_MAC_TYPE_1G_PHY;

	return regmap_update_bits(priv->map, RTL930X_SMI_MAC_TYPE_CTRL,
				  mask, field_prep(mask, src_val));
}

int rtl931x_mac_force_mode_ctrl(int p)
{
	return RTL931X_MAC_FORCE_MODE_CTRL + (p << 2);
}

int rtl931x_mac_port_ctrl(int p)
{
	return RTL931X_MAC_L2_PORT_CTRL + (p << 7);
}

int rtl931x_mac_max_len_reg(int p)
{
	return RTL931X_MAC_L2_PORT_MAX_LEN_CTRL + (p << 2);
}

int
rtldsa_931x_mac_link_state_source_set(struct otto_dsa_priv *priv,
				      int port,
				      enum rtldsa_mac_link_state_source source,
				      phy_interface_t interface)
{
	u32 mask, src_val;

	if (port >= RTL931X_CPU_PORT)
		return 0;

	mask = RTL931X_SMI_PHY_ABLTY_MASK(port);
	if (source == RTLDSA_MAC_LINK_STATE_SOURCE_PCS)
		src_val = RTL931X_SMI_PHY_ABLTY_SDS;
	else
		src_val = RTL931X_SMI_PHY_ABLTY_MDIO;

	return regmap_update_bits(priv->map,
				  RTL931X_SMI_PHY_ABLTY_GET_SEL(port),
				  mask, field_prep(mask, src_val));
}

static int rtldsa_phylink_fill_available_pcs(struct phylink_config *config,
					     struct phylink_pcs **available_pcs,
					     unsigned int num_possible_pcs)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);

	return fwnode_phylink_pcs_parse(of_fwnode_handle(dp->dn),
					available_pcs, num_possible_pcs);
}

void rtldsa_phylink_get_caps(struct dsa_switch *ds, int port,
				    struct phylink_config *config)
{
	struct dsa_port *dp = dsa_to_port(ds, port);
	struct otto_dsa_priv *priv = ds->priv;
	unsigned long caps = priv->r->mac_capabilities;

	/* TODO: This needs to take into account the MAC to SERDES mapping */
	config->mac_capabilities = caps;
	if (caps & MAC_100)
		__set_bit(PHY_INTERFACE_MODE_100BASEX, config->supported_interfaces);
	if (caps & MAC_1000FD) {
		__set_bit(PHY_INTERFACE_MODE_1000BASEX, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_SGMII, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_QSGMII, config->supported_interfaces);
	}
	if (caps & MAC_2500FD)
		__set_bit(PHY_INTERFACE_MODE_2500BASEX, config->supported_interfaces);
	if (caps & MAC_10000FD) {
		__set_bit(PHY_INTERFACE_MODE_10GBASER, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_USXGMII, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_10G_QXGMII, config->supported_interfaces);
	}

	config->num_possible_pcs = fwnode_phylink_pcs_count(of_fwnode_handle(dp->dn));
	if (config->num_possible_pcs) {
		config->fill_available_pcs = rtldsa_phylink_fill_available_pcs;
		bitmap_copy(config->pcs_interfaces, config->supported_interfaces,
			    PHY_INTERFACE_MODE_MAX);
	}

	__set_bit(PHY_INTERFACE_MODE_INTERNAL, config->supported_interfaces);
}

static void rtldsa_83xx_phylink_mac_config(struct phylink_config *config,
					   unsigned int mode,
					   const struct phylink_link_state *state)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct otto_dsa_priv *priv = dp->ds->priv;
	int port = dp->index;
	u32 mcr;

	pr_debug("%s port %d, mode %x\n", __func__, port, mode);

	/* currently only needed for RTL8380 */
	if (priv->family_id != RTL8380_FAMILY_ID)
		return;

	if (dsa_port_is_cpu(dp)) {
		/* allow CRC errors on CPU-port */
		regmap_update_bits(priv->map, priv->r->mac_port_ctrl(port),
				   0x8, 0x8);
		return;
	}

	regmap_read(priv->map, priv->r->mac_force_mode_ctrl(port), &mcr);
	if (mode == MLO_AN_PHY || phylink_autoneg_inband(mode)) {
		pr_debug("port %d PHY autonegotiates\n", port);

		mcr |= RTL838X_NWAY_EN;
	} else {
		mcr &= ~RTL838X_NWAY_EN;
	}
	regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), mcr);
}

static void
rtldsa_mac_link_state_source_set(struct otto_dsa_priv *priv, int port,
				 enum rtldsa_mac_link_state_source source,
				 phy_interface_t interface)
{
	int err;

	if (!priv->r->mac_link_state_source_set)
		return;

	err = priv->r->mac_link_state_source_set(priv, port, source, interface);
	if (err)
		dev_err(priv->dev,
			"failed to set MAC link-state source for port %d: %d\n",
			port, err);
}

static void rtldsa_93xx_phylink_mac_config(struct phylink_config *config,
					   unsigned int mode,
					   const struct phylink_link_state *state)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct otto_dsa_priv *priv = dp->ds->priv;
	const struct rtldsa_mac_force_mode_cfg *cfg;
	bool force_dynamic_phy;
	int port = dp->index;

	/* Nothing to be done for the CPU-port */
	if (port == priv->r->cpu_port)
		return;

	cfg = &priv->r->mac_force_mode;
	force_dynamic_phy = !priv->ports[port].phy &&
		(mode == MLO_AN_PHY || state->interface == PHY_INTERFACE_MODE_SGMII);

	/* Select PCS before dropping force mode to avoid a transient link down. */
	if (!priv->ports[port].phy && phylink_autoneg_inband(mode) &&
	    !force_dynamic_phy)
		rtldsa_mac_link_state_source_set(priv, port,
					 RTLDSA_MAC_LINK_STATE_SOURCE_PCS,
					 state->interface);

	if (mode == MLO_AN_FIXED || force_dynamic_phy) {
		/* Keep links that require force mode forced down. */
		regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port),
			     cfg->force_en_mask | cfg->fc_force_en_mask);
		if (force_dynamic_phy)
			rtldsa_mac_link_state_source_set(priv, port,
						 RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
						 state->interface);
	} else {
		/* Disable MAC completely */
		regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), 0);
	}
}

static void rtldsa_83xx_phylink_mac_link_down(struct phylink_config *config,
					      unsigned int mode,
					      phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct otto_dsa_priv *priv = dp->ds->priv;
	const struct rtldsa_mac_force_mode_cfg *cfg;
	int port = dp->index;

	cfg = &priv->r->mac_force_mode;

	/* Stop TX/RX to port */
	regmap_update_bits(priv->map, priv->r->mac_port_ctrl(port), 0x3, 0);

	/* No longer force link */
	regmap_update_bits(priv->map, priv->r->mac_force_mode_ctrl(port),
			   cfg->force_en_mask | cfg->link_up_mask, 0);
}

static void rtldsa_93xx_phylink_mac_link_down(struct phylink_config *config,
					      unsigned int mode,
					      phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct otto_dsa_priv *priv = dp->ds->priv;
	const struct rtldsa_mac_force_mode_cfg *cfg;
	bool force_dynamic_phy;
	int port = dp->index;

	cfg = &priv->r->mac_force_mode;
	force_dynamic_phy = !priv->ports[port].phy &&
		(mode == MLO_AN_PHY || interface == PHY_INTERFACE_MODE_SGMII);

	/* Stop TX/RX to port */
	regmap_update_bits(priv->map, priv->r->mac_port_ctrl(port), 0x3, 0);

	if (!priv->ports[port].phy && phylink_autoneg_inband(mode) &&
	    !force_dynamic_phy)
		rtldsa_mac_link_state_source_set(priv, port,
					 RTLDSA_MAC_LINK_STATE_SOURCE_PCS,
					 interface);

	if (dsa_port_is_cpu(dp) || mode == MLO_AN_FIXED || force_dynamic_phy) {
		/* Preserve force mode while forcing the link down. */
		regmap_update_bits(priv->map, priv->r->mac_force_mode_ctrl(port),
				   cfg->link_up_mask, 0);
		if (force_dynamic_phy)
			rtldsa_mac_link_state_source_set(priv, port,
						 RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
						 interface);
	} else {
		/* No longer force link */
		regmap_update_bits(priv->map, priv->r->mac_force_mode_ctrl(port),
				   cfg->force_en_mask | cfg->fc_force_en_mask |
				   cfg->link_up_mask, 0);
	}
}

static void rtldsa_83xx_phylink_mac_link_up(struct phylink_config *config,
					    struct phy_device *phydev,
					    unsigned int mode,
					    phy_interface_t interface,
					    int speed, int duplex,
					    bool tx_pause, bool rx_pause)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct otto_dsa_priv *priv = dp->ds->priv;
	const struct rtldsa_mac_force_mode_cfg *cfg;
	int port = dp->index;
	u32 mcr, spdsel;

	if (speed == SPEED_1000)
		spdsel = RTL_SPEED_1000;
	else if (speed == SPEED_100)
		spdsel = RTL_SPEED_100;
	else
		spdsel = RTL_SPEED_10;

	cfg = &priv->r->mac_force_mode;
	regmap_read(priv->map, priv->r->mac_force_mode_ctrl(port), &mcr);
	mcr &= ~(cfg->rx_pause_mask | cfg->tx_pause_mask |
		 cfg->duplex_mask | cfg->speed_mask);
	mcr |= cfg->link_up_mask;
	mcr |= field_prep(cfg->speed_mask, spdsel);

	if (tx_pause)
		mcr |= cfg->tx_pause_mask;
	if (rx_pause)
		mcr |= cfg->rx_pause_mask;
	if (duplex == DUPLEX_FULL || priv->lagmembers & BIT_ULL(port))
		mcr |= cfg->duplex_mask;
	if (dsa_port_is_cpu(dp))
		mcr |= cfg->force_en_mask;

	pr_debug("%s port %d, mode %x, speed %d, duplex %d, txpause %d, rxpause %d: set mcr=%08x\n",
		 __func__, port, mode, speed, duplex, tx_pause, rx_pause, mcr);
	regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), mcr);

	/* Restart TX/RX to port */
	regmap_update_bits(priv->map, priv->r->mac_port_ctrl(port), 0x3, 0x3);
}

static void rtldsa_93xx_phylink_mac_link_up(struct phylink_config *config,
					    struct phy_device *phydev,
					    unsigned int mode,
					    phy_interface_t interface,
					    int speed, int duplex,
					    bool tx_pause, bool rx_pause)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct otto_dsa_priv *priv = dp->ds->priv;
	const struct rtldsa_mac_force_mode_cfg *cfg;
	int port = dp->index;
	bool dynamic_phy, force_dynamic_phy, keep_forced;
	u32 mcr, spdsel;

	/*
	 * The RTL93xx polling machinery does more than issue MDIO
	 * transactions. It collects resolved link state and distributes it
	 * internally to the MAC, so polling must remain enabled for
	 * SerDes-backed SFP ports even when there is no PHY to poll over MDIO.
	 *
	 * The MDIO driver owns the polling setup. It enables polling for PHY
	 * and SerDes ports and initializes MAC_TYPE/PHY_ABLTY from the static,
	 * firmware-described topology. That topology cannot describe a PHY
	 * which appears later in a hot-plugged SFP module. Only DSA learns
	 * about such a PHY through phylink, so the source selector is
	 * deliberately shared: MDIO initializes it, while DSA updates it as
	 * the active SFP link-state source changes. DSA does not disable
	 * polling here; it only selects whether the polling machinery supplies
	 * the MAC with state from the PCS or from the dynamic PHY.
	 *
	 * Accordingly, direct links use PCS state, while statically described
	 * PHYs retain the MDIO driver's hardware-polling selection. Dynamic
	 * SGMII and out-of-band PHYs select PHY state and remain forced.
	 * Dynamic USXGMII PHYs use PCS state after a force pulse refreshes the
	 * MAC parameters; fixed links and CPU ports also remain forced.
	 */
	cfg = &priv->r->mac_force_mode;
	dynamic_phy = !priv->ports[port].phy && phydev;
	force_dynamic_phy = dynamic_phy &&
		(!phylink_autoneg_inband(mode) ||
		 interface == PHY_INTERFACE_MODE_SGMII);
	keep_forced = dsa_port_is_cpu(dp) || mode == MLO_AN_FIXED ||
		      force_dynamic_phy;
	if (dynamic_phy &&
	    (interface == PHY_INTERFACE_MODE_SGMII ||
	     interface == PHY_INTERFACE_MODE_USXGMII)) {
		if (phydev->speed != SPEED_UNKNOWN)
			speed = phydev->speed;
		if (phydev->duplex != DUPLEX_UNKNOWN)
			duplex = phydev->duplex;
	}

	regmap_read(priv->map, priv->r->mac_force_mode_ctrl(port), &mcr);
	mcr &= ~(cfg->rx_pause_mask | cfg->tx_pause_mask);
	mcr |= cfg->fc_force_en_mask;
	if (tx_pause)
		mcr |= cfg->tx_pause_mask;
	if (rx_pause)
		mcr |= cfg->rx_pause_mask;

	if (!keep_forced && !dynamic_phy) {
		if (!priv->ports[port].phy)
			rtldsa_mac_link_state_source_set(priv, port,
							 RTLDSA_MAC_LINK_STATE_SOURCE_PCS,
							 interface);
		mcr &= ~cfg->force_en_mask;
		regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), mcr);
		goto restart;
	}

	if (speed == SPEED_10000)
		spdsel = RTL_SPEED_10000;
	else if (speed == SPEED_5000)
		spdsel = RTL_SPEED_5000;
	else if (speed == SPEED_2500)
		spdsel = RTL_SPEED_2500;
	else if (speed == SPEED_1000)
		spdsel = RTL_SPEED_1000;
	else if (speed == SPEED_100)
		spdsel = RTL_SPEED_100;
	else
		spdsel = RTL_SPEED_10;

	mcr &= ~(cfg->duplex_mask | cfg->speed_mask);
	mcr |= cfg->force_en_mask | cfg->fc_force_en_mask | cfg->link_up_mask;
	mcr |= field_prep(cfg->speed_mask, spdsel);
	if (dynamic_phy)
		mcr &= ~cfg->media_mask;

	if (duplex == DUPLEX_FULL || priv->lagmembers & BIT_ULL(port))
		mcr |= cfg->duplex_mask;

	pr_debug("%s port %d, mode %x, speed %d, duplex %d, txpause %d, rxpause %d: set mcr=%08x\n",
		 __func__, port, mode, speed, duplex, tx_pause, rx_pause, mcr);
	regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), mcr);
	/* Pulse state force enables so hardware latches the new parameters. */
	regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port),
		     mcr & ~cfg->force_en_mask);
	if (keep_forced)
		regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), mcr);

	/* Keep force mode active while switching hardware link sources. */
	if (force_dynamic_phy)
		rtldsa_mac_link_state_source_set(priv, port,
					 RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
					 interface);

restart:
	/* Restart TX/RX to port */
	regmap_update_bits(priv->map, priv->r->mac_port_ctrl(port), 0x3, 0x3);
}

int rtldsa_port_max_mtu(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	/* Families that cannot limit the frame length keep advertising the
	 * ether_setup() default their ports always had.
	 */
	if (!priv->r->max_frame)
		return ETH_DATA_LEN;

	return priv->r->max_frame - RTLDSA_FRAME_OVERHEAD - RTLDSA_TAG_OVERHEAD;
}

static int rtldsa_largest_mtu(struct dsa_switch *ds, int port, int new_mtu)
{
	struct dsa_port *dp;
	int mtu = new_mtu;

	/* The MTU of the port being changed is written after this operation
	 * runs, and not every user netdevice exists yet while probing.
	 */
	dsa_switch_for_each_user_port(dp, ds)
		if (dp->index != port && dp->user)
			mtu = max_t(int, mtu, dp->user->mtu);

	return mtu;
}

int rtldsa_port_change_mtu(struct dsa_switch *ds, int port, int new_mtu)
{
	struct otto_dsa_priv *priv = ds->priv;
	int frame_size;

	if (!priv->r->max_frame)
		return -EOPNOTSUPP;

	/* The ethernet driver owns the limits of the CPU port and programs them
	 * from the conduit MTU, which carries the tagger overhead that the DSA
	 * core leaves out of the value handed here.
	 */
	if (dsa_is_cpu_port(ds, port))
		return 0;

	/* new_mtu is the L2 payload size, but the MAC limit counts the whole
	 * frame: the Ethernet header, up to two stacked VLAN tags (802.1ad
	 * QinQ) and the FCS. The MAC TAG_INC bit is left as found, so VLAN tag
	 * bytes are not counted twice. The tail tag towards the CPU port takes
	 * the place of the FCS and needs no room of its own.
	 */
	if (priv->r->mac_max_len_reg) {
		frame_size = new_mtu + RTLDSA_FRAME_OVERHEAD;

		sw_w32_mask(RTLDSA_MAC_MAX_LEN_MASK,
			    RTLDSA_MAC_MAX_LEN_VAL(frame_size),
			    priv->r->mac_max_len_reg(port));

		return 0;
	}

	/* One register for all ports, so it has to fit the largest of them */
	frame_size = rtldsa_largest_mtu(ds, port, new_mtu) + RTLDSA_FRAME_OVERHEAD;

	sw_w32_mask(RTLDSA_MAC_MAX_LEN_MASK, RTLDSA_MAC_MAX_LEN_VAL(frame_size),
		    priv->r->mac_max_len_ctrl);

	if (priv->r->mac_max_len_ctrl_dup)
		sw_w32_mask(RTLDSA_MAC_MAX_LEN_MASK,
			    RTLDSA_MAC_MAX_LEN_VAL(frame_size),
			    priv->r->mac_max_len_ctrl_dup);

	return 0;
}

bool rtldsa_support_eee(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	/* TODO: do this better */
	return (priv->ports[port].phy != 0);
}

int rtldsa_set_mac_eee(struct dsa_switch *ds, int port, struct ethtool_keee *e)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (e->eee_enabled && !priv->eee_enabled) {
		pr_info("Globally enabling EEE\n");
		priv->r->init_eee(priv, true);
	}

	priv->r->set_mac_eee(priv, port, e->eee_enabled);

	if (e->eee_enabled)
		pr_info("Enabled EEE for port %d\n", port);
	else
		pr_info("Disabled EEE for port %d\n", port);

	return 0;
}

const struct phylink_mac_ops rtldsa_83xx_phylink_mac_ops = {
	.mac_config		= rtldsa_83xx_phylink_mac_config,
	.mac_link_down		= rtldsa_83xx_phylink_mac_link_down,
	.mac_link_up		= rtldsa_83xx_phylink_mac_link_up,
};

const struct phylink_mac_ops rtldsa_93xx_phylink_mac_ops = {
	.mac_config		= rtldsa_93xx_phylink_mac_config,
	.mac_link_down		= rtldsa_93xx_phylink_mac_link_down,
	.mac_link_up		= rtldsa_93xx_phylink_mac_link_up,
};
