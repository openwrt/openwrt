// SPDX-License-Identifier: GPL-2.0-only

#include <net/dsa.h>
#include <linux/bitfield.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/pcs/pcs.h>
#include <linux/regmap.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "lag.h"
#include "mirror.h"
#include "mac.h"
#include "l2.h"
#include "rtl-otto.h"
#include "stats.h"
#include "tc.h"
#include "vlan.h"
#include "stp.h"

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

#define RTL930X_PORT_IGNORE 0x3f

/* Ethernet header, two stacked VLAN tags (802.1ad QinQ) and FCS */
#define RTLDSA_FRAME_OVERHEAD		(ETH_HLEN + 2 * VLAN_HLEN + ETH_FCS_LEN)
/* Tail tag the DSA core adds to a frame on its way to the conduit */
#define RTLDSA_TAG_OVERHEAD		4

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

/* DSA callbacks */

static enum dsa_tag_protocol rtldsa_get_tag_protocol(struct dsa_switch *ds,
						     int port,
						     enum dsa_tag_protocol mprot)
{
	/* The switch does not tag the frames, instead internally the header
	 * structure for each packet is tagged accordingly.
	 */
	return DSA_TAG_PROTO_RTL_OTTO;
}

static void rtldsa_83xx_mc_pmasks_setup(struct otto_dsa_priv *priv)
{
	/* RTL8380 and RTL8390 use an index into the portmask table to set the
	 * unknown multicast portmask, setup a default at a safe location
	 * On RTL93XX, the portmask is directly set in the profile,
	 * see e.g. rtl9300_vlan_profile_setup
	 */
	priv->r->write_mcast_pmask(MC_PMASK_ALL_PORTS_IDX, ~0);
}

static void rtldsa_setup_bpdu_traps(struct otto_dsa_priv *priv)
{
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->set_receive_management_action(i, BPDU, TRAP2CPU);
}

static void rtldsa_setup_lldp_traps(struct otto_dsa_priv *priv)
{
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->set_receive_management_action(i, LLDP, TRAP2CPU);
}

static void rtldsa_setup_eapol_traps(struct otto_dsa_priv *priv)
{
	for (int i = 0; i < priv->r->cpu_port; i++)
		priv->r->set_receive_management_action(i, EAPOL, TRAP2CPU);
}

static void rtldsa_port_set_salrn(struct otto_dsa_priv *priv,
				  int port, bool enable)
{
	int shift = SALRN_PORT_SHIFT(port);
	int val = enable ? SALRN_MODE_HARDWARE : SALRN_MODE_DISABLED;

	sw_w32_mask(SALRN_MODE_MASK << shift, val << shift,
		    priv->r->l2_port_new_salrn(port));
}

static int rtldsa_83xx_setup(struct dsa_switch *ds)
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
			priv->r->traffic_set(i, BIT_ULL(i));
		}
	}
	priv->r->traffic_set(priv->r->cpu_port, BIT_ULL(priv->r->cpu_port));

	/* For standalone ports, forward packets even if a static fdb
	 * entry for the source address exists on another port.
	 */
	if (priv->r->set_static_move_action) {
		for (int i = 0; i <= priv->r->cpu_port; i++)
			priv->r->set_static_move_action(i, true);
	}

	priv->r->print_matrix();
	otto_stats_init(priv);
	otto_stats_init_counters(priv);

	rtldsa_83xx_mc_pmasks_setup(priv);
	otto_vlan_setup(priv);

	rtldsa_setup_bpdu_traps(priv);
	rtldsa_setup_lldp_traps(priv);

	ds->configure_vlan_while_not_filtering = true;

	priv->r->l2_learning_setup();

	rtldsa_port_set_salrn(priv, priv->r->cpu_port, false);
	ds->assisted_learning_on_cpu_port = true;

	/* Make sure all frames sent to the switch's MAC are trapped to the CPU-port
	 *  0: FWD, 1: DROP, 2: TRAP2CPU
	 */
	sw_w32(0x2, priv->r->self_mac_trap_ctrl);

	priv->r->pie_init(priv);

	if (priv->r->pie_cls_flower) {
		err = otto_tc_init(priv);
		if (err)
			return err;
	}

	return 0;
}

static int rtldsa_93xx_setup(struct dsa_switch *ds)
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
			priv->r->traffic_set(i, BIT_ULL(i));
		}
	}
	priv->r->traffic_set(priv->r->cpu_port, BIT_ULL(priv->r->cpu_port));
	priv->r->print_matrix();

	otto_stats_init(priv);
	otto_stats_init_counters(priv);

	otto_vlan_setup(priv);

	rtldsa_setup_bpdu_traps(priv);
	rtldsa_setup_lldp_traps(priv);
	rtldsa_setup_eapol_traps(priv);

	ds->configure_vlan_while_not_filtering = true;

	priv->r->l2_learning_setup();

	rtldsa_port_set_salrn(priv, priv->r->cpu_port, false);
	ds->assisted_learning_on_cpu_port = true;

	priv->r->pie_init(priv);

	if (priv->r->pie_cls_flower) {
		err = otto_tc_init(priv);
		if (err)
			return err;
	}

	priv->r->led_init(priv);

	return 0;
}

static int rtldsa_phylink_fill_available_pcs(struct phylink_config *config,
					     struct phylink_pcs **available_pcs,
					     unsigned int num_possible_pcs)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);

	return fwnode_phylink_pcs_parse(of_fwnode_handle(dp->dn),
					available_pcs, num_possible_pcs);
}

static void rtldsa_phylink_get_caps(struct dsa_switch *ds, int port,
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
	u32 eee = 0;

	/* Nothing to be done for the CPU-port */
	if (port == priv->r->cpu_port)
		return;

	cfg = &priv->r->mac_force_mode;

	/*
	 * Keep independently configured per-port EEE capability while the
	 * forced link state is dropped. Families that hold those controls in
	 * the same register describe them in mac_force_mode_eee_mask, so that
	 * a phylink reconfiguration does not silently undo an ethtool EEE
	 * request.
	 */
	if (READ_ONCE(priv->ports[port].eee_enabled))
		eee = priv->r->mac_force_mode_eee_mask;

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
			     cfg->force_en_mask | cfg->fc_force_en_mask | eee);
		if (force_dynamic_phy)
			rtldsa_mac_link_state_source_set(priv, port,
						 RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
						 state->interface);
	} else {
		/* Disable MAC completely, retaining EEE enables */
		regmap_write(priv->map, priv->r->mac_force_mode_ctrl(port), eee);
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

static int rtldsa_mc_group_alloc(struct otto_dsa_priv *priv, int port)
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

static u64 rtldsa_mc_group_add_port(struct otto_dsa_priv *priv, int mc_group, int port)
{
	u64 portmask = priv->r->read_mcast_pmask(mc_group);

	pr_debug("%s: %d\n", __func__, port);

	portmask |= BIT_ULL(port);
	priv->r->write_mcast_pmask(mc_group, portmask);

	return portmask;
}

static u64 rtldsa_mc_group_del_port(struct otto_dsa_priv *priv, int mc_group, int port)
{
	u64 portmask = priv->r->read_mcast_pmask(mc_group);

	pr_debug("%s: %d\n", __func__, port);

	portmask &= ~BIT_ULL(port);
	priv->r->write_mcast_pmask(mc_group, portmask);
	if (!portmask)
		clear_bit(mc_group, priv->mc_group_bm);

	return portmask;
}

static int rtldsa_port_enable(struct dsa_switch *ds, int port, struct phy_device *phydev)
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
	priv->r->traffic_enable(priv->r->cpu_port, port);

	/* add all other ports in the same bridge to switch mask of port */
	priv->r->traffic_set(port, priv->ports[port].pm);

	/* TODO: Figure out if this is necessary */
	if (priv->family_id == RTL9300_FAMILY_ID) {
		sw_w32_mask(0, BIT(port), RTL930X_L2_PORT_SABLK_CTRL);
		sw_w32_mask(0, BIT(port), RTL930X_L2_PORT_DABLK_CTRL);
	}

	return 0;
}

static void rtldsa_port_disable(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);
	/* you can only disable user ports */
	if (!dsa_is_user_port(ds, port))
		return;

	/* BUG: This does not work on RTL931X */
	/* remove port from switch mask of CPU_PORT */
	priv->r->traffic_disable(priv->r->cpu_port, port);

	/* remove all other ports from switch mask of port */
	priv->r->traffic_set(port, 0);

	priv->ports[port].enable = false;
}

static int rtldsa_port_max_mtu(struct dsa_switch *ds, int port)
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

static int rtldsa_port_change_mtu(struct dsa_switch *ds, int port, int new_mtu)
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

static bool rtldsa_support_eee(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	/* TODO: do this better */
	return (priv->ports[port].phy != 0);
}

static int rtldsa_set_mac_eee(struct dsa_switch *ds, int port, struct ethtool_keee *e)
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

static int rtldsa_set_ageing_time(struct dsa_switch *ds, unsigned int msec)
{
	struct otto_dsa_priv *priv = ds->priv;

	priv->r->set_ageing_time(msec);

	return 0;
}

static void rtldsa_update_port_member(struct otto_dsa_priv *priv, int port,
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
			priv->r->traffic_set(other_port, other_p->pm);
	}

	p->pm = port_mask;

	if (p->enable)
		priv->r->traffic_set(port, port_mask);
}

static int rtldsa_port_bridge_join(struct dsa_switch *ds, int port, struct dsa_bridge bridge,
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

	rtldsa_update_port_member(priv, port, bridge.dev, true);

	if (priv->r->set_static_move_action)
		priv->r->set_static_move_action(port, false);

	/* Set to disabled in all MSTs, common code will take care of CIST */
	rtldsa_port_non_cist_states_set(priv, port, BR_STATE_DISABLED);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void rtldsa_port_bridge_leave(struct dsa_switch *ds, int port, struct dsa_bridge bridge)
{
	struct otto_dsa_priv *priv = ds->priv;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);

	mutex_lock(&priv->reg_mutex);

	rtldsa_update_port_member(priv, port, bridge.dev, false);

	if (priv->ports[port].qinq) {
		priv->r->vlan_port_qinq_set(priv, port, false);
		priv->ports[port].qinq = false;
	}

	if (priv->r->set_static_move_action)
		priv->r->set_static_move_action(port, true);

	/* Set to forwarding in all MSTs, common code will take care of CIST */
	rtldsa_port_non_cist_states_set(priv, port, BR_STATE_FORWARDING);

	mutex_unlock(&priv->reg_mutex);
}

static int rtldsa_port_vlan_protocol(struct dsa_switch *ds, int port, u16 proto,
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

static void rtldsa_setup_l2_uc_entry(struct rtl838x_l2_entry *e, int port,
				     int vid, u64 mac)
{
	bool next_hop = e->valid && e->next_hop;
	u16 nh_route_id = next_hop ? e->nh_route_id : 0;

	memset(e, 0, sizeof(*e));

	e->type = L2_UNICAST;
	e->valid = true;

	/* A route may be forwarding through this address already, and the id
	 * it is known by lives nowhere else.
	 */
	e->next_hop = next_hop;
	e->nh_route_id = nh_route_id;

	e->age = 3;
	e->is_static = true;

	e->port = port;

	e->rvid = e->vid = vid;
	e->is_ip_mc = e->is_ipv6_mc = false;
	u64_to_ether_addr(mac, e->mac);
}

static void rtldsa_setup_l2_mc_entry(struct rtl838x_l2_entry *e, int vid, u64 mac, int mc_group)
{
	memset(e, 0, sizeof(*e));

	e->type = L2_MULTICAST;
	e->valid = true;

	e->mc_portmask_index = mc_group;

	e->rvid = e->vid = vid;
	e->is_ip_mc = e->is_ipv6_mc = false;
	u64_to_ether_addr(mac, e->mac);
}

static int rtldsa_port_fdb_add(struct dsa_switch *ds, int port,
			       const unsigned char *addr, u16 vid,
			       const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	struct rtl838x_l2_entry e;
	int err = 0, idx;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	int lag_group = otto_lag_find_group_from_port(priv, port);

	if (lag_group >= 0 && priv->r->prepare_lag_fdb) {
		priv->r->prepare_lag_fdb(&e, lag_group);
	} else {
		if (priv->lag_non_primary & BIT_ULL(port)) {
			pr_debug("%s: %d is lag slave but prepare_lag_fdb is not supported. ignore\n",
				 __func__, port);
			return 0;
		}
	}

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, false, &e);

	/* Found an existing or empty entry */
	if (idx >= 0) {
		struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, idx);

		if (m) {
			/* A slot nobody had claimed carries whatever its last
			 * owner left behind.
			 */
			if (!e.valid)
				*m = (struct rtldsa_l2_uc){};
			m->fdb_ref = true;
		}

		rtldsa_setup_l2_uc_entry(&e, port, vid, mac);
		priv->r->write_l2_entry_using_hash(idx >> 2, idx & 0x3, &e);
		goto out;
	}

	/* Hash buckets full, try CAM */
	idx = otto_l2_find_cam_entry(priv, seed, false, &e);

	if (idx >= 0) {
		rtldsa_setup_l2_uc_entry(&e, port, vid, mac);
		priv->r->write_cam(idx, &e);
		goto out;
	}

	err = -ENOTSUPP;

out:
	mutex_unlock(&priv->reg_mutex);

	return err;
}

static int rtldsa_port_fdb_del(struct dsa_switch *ds, int port,
			       const unsigned char *addr, u16 vid,
			       const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	struct rtl838x_l2_entry e;
	int err = 0, idx;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	int lag_group = otto_lag_find_group_from_port(priv, port);

	if (lag_group >= 0 && priv->r->prepare_lag_fdb)
		priv->r->prepare_lag_fdb(&e, lag_group);

	pr_debug("In %s, mac %llx, vid: %d\n", __func__, mac, vid);

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, true, &e);

	if (idx >= 0) {
		struct rtldsa_l2_uc *m = otto_l2_uc_lookup(priv, idx);

		pr_debug("Found entry index %d, key %d and bucket %d\n", idx, idx >> 2, idx & 3);

		if (m)
			m->fdb_ref = false;

		/* A route still forwarding through this address keeps it, as
		 * the next hop it already is, reachable through no port of its
		 * own.
		 */
		if (m && m->l3_refcount && e.next_hop) {
			e.port = priv->r->port_ignore;
			e.age = 0;
		} else {
			e.valid = false;
		}

		priv->r->write_l2_entry_using_hash(idx >> 2, idx & 0x3, &e);
		goto out;
	}

	/* Check CAM for spillover from hash buckets */
	idx = otto_l2_find_cam_entry(priv, seed, true, &e);

	if (idx >= 0) {
		e.valid = false;
		priv->r->write_cam(idx, &e);
		goto out;
	}
	err = -ENOENT;

out:
	mutex_unlock(&priv->reg_mutex);

	return err;
}

static int rtldsa_port_fdb_dump(struct dsa_switch *ds, int port,
				dsa_fdb_dump_cb_t *cb, void *data)
{
	struct otto_dsa_priv *priv = ds->priv;
	int uc_rows, cam_rows;

	uc_rows = otto_table_rows(priv->r->l2_uc_tbl);
	if (uc_rows < 0)
		return uc_rows;

	cam_rows = otto_table_rows(priv->r->l2_cam_tbl);
	if (cam_rows < 0)
		return cam_rows;

	mutex_lock(&priv->reg_mutex);

	for (int i = 0; i < uc_rows; i++) {
		struct rtl838x_l2_entry e = {};

		if (!((i + 1) % 64))
			cond_resched();

		priv->r->read_l2_entry_using_hash(i >> 2, i & 0x3, &e);

		if (!e.valid || e.type != L2_UNICAST || e.is_trunk)
			continue;

		if (e.port == port || e.port == RTL930X_PORT_IGNORE)
			cb(e.mac, e.rvid, e.is_static, data);
	}

	for (int i = 0; i < cam_rows; i++) {
		struct rtl838x_l2_entry e = {};

		priv->r->read_cam(i, &e);

		if (!e.valid || e.type != L2_UNICAST || e.is_trunk)
			continue;

		if (e.port == port)
			cb(e.mac, e.rvid, e.is_static, data);
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static bool rtldsa_mac_is_unsnoop(const unsigned char *addr)
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

static int rtldsa_83xx_port_mdb_add(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_mdb *mdb,
				    const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(mdb->addr);
	struct rtl838x_l2_entry e;
	int err = 0, idx;
	int vid = mdb->vid;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	int mc_group;

	pr_debug("In %s port %d, mac %llx, vid: %d\n", __func__, port, mac, vid);

	if (priv->lag_non_primary & BIT_ULL(port)) {
		pr_debug("%s: %d is lag slave. ignore\n", __func__, port);
		return -EINVAL;
	}

	if (rtldsa_mac_is_unsnoop(mdb->addr)) {
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
			rtldsa_mc_group_add_port(priv, e.mc_portmask_index, port);
		} else {
			pr_debug("New entry for seed %016llx\n", seed);
			mc_group = rtldsa_mc_group_alloc(priv, port);
			if (mc_group < 0) {
				err = -ENOTSUPP;
				goto out;
			}
			rtldsa_setup_l2_mc_entry(&e, vid, mac, mc_group);
			priv->r->write_l2_entry_using_hash(idx >> 2, idx & 0x3, &e);
		}
		goto out;
	}

	/* Hash buckets full, try CAM */
	idx = otto_l2_find_cam_entry(priv, seed, false, &e);

	if (idx >= 0) {
		if (e.valid) {
			pr_debug("Found existing CAM entry %016llx, mc_group %d\n",
				 ether_addr_to_u64(e.mac), e.mc_portmask_index);
			rtldsa_mc_group_add_port(priv, e.mc_portmask_index, port);
		} else {
			pr_debug("New entry\n");
			mc_group = rtldsa_mc_group_alloc(priv, port);
			if (mc_group < 0) {
				err = -ENOTSUPP;
				goto out;
			}
			rtldsa_setup_l2_mc_entry(&e, vid, mac, mc_group);
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
static int rtldsa_93xx_port_mdb_add(struct dsa_switch *ds, int port,
				    const struct switchdev_obj_port_mdb *mdb,
				    const struct dsa_db db)
{
	return -EOPNOTSUPP;
}

static int rtldsa_port_mdb_del(struct dsa_switch *ds, int port,
			       const struct switchdev_obj_port_mdb *mdb,
			       const struct dsa_db db)
{
	struct otto_dsa_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(mdb->addr);
	struct rtl838x_l2_entry e;
	int err = 0, idx;
	int vid = mdb->vid;
	u64 seed = priv->r->l2_hash_seed(mac, vid);
	u64 portmask;

	pr_debug("In %s, port %d, mac %llx, vid: %d\n", __func__, port, mac, vid);

	if (priv->lag_non_primary & BIT_ULL(port)) {
		pr_info("%s: %d is lag slave. ignore\n", __func__, port);
		return 0;
	}

	if (rtldsa_mac_is_unsnoop(mdb->addr)) {
		dev_dbg(priv->dev,
			"%s: %pM might belong to an unsnoopable IP. ignore\n",
			__func__, mdb->addr);
		return 0;
	}

	mutex_lock(&priv->reg_mutex);

	idx = otto_l2_find_hash_entry(priv, seed, true, &e);

	if (idx >= 0) {
		pr_debug("Found entry index %d, key %d and bucket %d\n", idx, idx >> 2, idx & 3);
		portmask = rtldsa_mc_group_del_port(priv, e.mc_portmask_index, port);
		if (!portmask) {
			e.valid = false;
			priv->r->write_l2_entry_using_hash(idx >> 2, idx & 0x3, &e);
		}
		goto out;
	}

	/* Check CAM for spillover from hash buckets */
	idx = otto_l2_find_cam_entry(priv, seed, true, &e);

	if (idx >= 0) {
		portmask = rtldsa_mc_group_del_port(priv, e.mc_portmask_index, port);
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

static int rtldsa_port_pre_bridge_flags(struct dsa_switch *ds, int port,
					struct switchdev_brport_flags flags,
					struct netlink_ext_ack *extack)
{
	struct otto_dsa_priv *priv = ds->priv;
	unsigned long features = BR_ISOLATED;

	pr_debug("%s: %d %lX\n", __func__, port, flags.val);
	if (priv->r->enable_learning)
		features |= BR_LEARNING;

	if (priv->r->enable_flood)
		features |= BR_FLOOD;

	if (priv->r->enable_l2_new_sa_fwd)
		features |= BR_PORT_LOCKED;

	if (priv->r->enable_mcast_flood)
		features |= BR_MCAST_FLOOD;
	if (priv->r->enable_bcast_flood)
		features |= BR_BCAST_FLOOD;
	if (flags.mask & ~(features))
		return -EINVAL;

	return 0;
}

/* dsa_port_fast_age() is DSA internal, repeat its bridge notification here. */
static void rtldsa_port_fast_age_notify(struct dsa_port *dp)
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

static int rtldsa_port_bridge_flags(struct dsa_switch *ds, int port,
				    struct switchdev_brport_flags flags,
				    struct netlink_ext_ack *extack)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct dsa_port *dp = dsa_to_port(ds, port);
	enum rtldsa_flood_type new_sa_fwd;
	unsigned long cached_flags;

	pr_debug("%s: %d %lX\n", __func__, port, flags.val);

	priv->ports[port].cached_flags &= ~flags.mask;
	priv->ports[port].cached_flags |= flags.val & flags.mask;

	cached_flags = priv->ports[port].cached_flags;

	if (cached_flags & BR_PORT_LOCKED) {
		/* A locked port must not learn addresses on its own, and the
		 * entries it learned before are no longer authorized.
		 */
		rtldsa_port_set_salrn(priv, port, false);

		if (flags.mask & BR_PORT_LOCKED)
			rtldsa_port_fast_age_notify(dp);
	} else {
		rtldsa_port_set_salrn(priv, port, !!(cached_flags & BR_LEARNING));
	}

	priv->ports[port].flood_type = (cached_flags & BR_FLOOD) ?
				       RTLDSA_FLOOD_TYPE_FORWARD :
				       RTLDSA_FLOOD_TYPE_DROP;

	if (priv->r->enable_flood)
		priv->r->enable_flood(port, priv->ports[port].flood_type);

	/* Trap frames with an unknown source address on a locked port to the
	 * CPU, so that an authenticator can inspect them and add an FDB entry.
	 */
	new_sa_fwd = (cached_flags & BR_PORT_LOCKED) ? RTLDSA_FLOOD_TYPE_TRAP2CPU :
						       RTLDSA_FLOOD_TYPE_FORWARD;

	if (priv->r->enable_l2_new_sa_fwd)
		priv->r->enable_l2_new_sa_fwd(port, new_sa_fwd);

	if (priv->r->enable_learning)
		priv->r->enable_learning(port, !!(cached_flags & BR_LEARNING));

	if (priv->r->enable_mcast_flood)
		priv->r->enable_mcast_flood(port, !!(cached_flags & BR_MCAST_FLOOD));

	if (priv->r->enable_bcast_flood)
		priv->r->enable_bcast_flood(port, !!(cached_flags & BR_BCAST_FLOOD));

	if (flags.mask & BR_ISOLATED) {
		struct net_device *bridge_dev = dsa_port_bridge_dev_get(dp);

		priv->ports[port].isolated = !!(cached_flags & BR_ISOLATED);

		mutex_lock(&priv->reg_mutex);
		rtldsa_update_port_member(priv, port, bridge_dev, true);
		mutex_unlock(&priv->reg_mutex);
	}

	return 0;
}

const struct phylink_mac_ops rtldsa_83xx_phylink_mac_ops = {
	.mac_config		= rtldsa_83xx_phylink_mac_config,
	.mac_link_down		= rtldsa_83xx_phylink_mac_link_down,
	.mac_link_up		= rtldsa_83xx_phylink_mac_link_up,
};

const struct dsa_switch_ops rtldsa_83xx_switch_ops = {
	.get_tag_protocol	= rtldsa_get_tag_protocol,
	.setup			= rtldsa_83xx_setup,

	.phylink_get_caps	= rtldsa_phylink_get_caps,

	.get_strings		= otto_stats_get_strings,
	.get_ethtool_stats	= otto_stats_get_ethtool,
	.get_sset_count		= otto_stats_get_sset_count,
	.get_eth_phy_stats	= otto_stats_get_eth_phy,
	.get_eth_mac_stats	= otto_stats_get_eth_mac,
	.get_eth_ctrl_stats	= otto_stats_get_eth_ctrl,
	.get_rmon_stats		= otto_stats_get_rmon,
	.get_stats64		= otto_stats_get64,
	.get_pause_stats	= otto_stats_get_pause,

	.port_enable		= rtldsa_port_enable,
	.port_disable		= rtldsa_port_disable,

	.port_change_mtu	= rtldsa_port_change_mtu,
	.port_max_mtu		= rtldsa_port_max_mtu,

	.support_eee		= rtldsa_support_eee,
	.set_mac_eee		= rtldsa_set_mac_eee,

	.set_ageing_time	= rtldsa_set_ageing_time,
	.port_bridge_join	= rtldsa_port_bridge_join,
	.port_bridge_leave	= rtldsa_port_bridge_leave,
	.port_stp_state_set	= rtldsa_port_stp_state_set,
	.port_fast_age		= rtldsa_port_fast_age,
	.port_mst_state_set	= rtldsa_port_mst_state_set,

	.port_vlan_filtering	= otto_vlan_port_filtering,
	.port_vlan_protocol	= rtldsa_port_vlan_protocol,
	.port_vlan_add		= otto_vlan_port_add,
	.port_vlan_del		= otto_vlan_port_del,
	.port_vlan_fast_age	= otto_vlan_port_fast_age,
	.vlan_msti_set		= otto_vlan_msti_set,

	.port_fdb_add		= rtldsa_port_fdb_add,
	.port_fdb_del		= rtldsa_port_fdb_del,
	.port_fdb_dump		= rtldsa_port_fdb_dump,

	.port_mdb_add		= rtldsa_83xx_port_mdb_add,
	.port_mdb_del		= rtldsa_port_mdb_del,

	.port_mirror_add	= rtldsa_port_mirror_add,
	.port_mirror_del	= rtldsa_port_mirror_del,

	.port_lag_change	= otto_lag_port_change,
	.port_lag_join		= otto_lag_port_join,
	.port_lag_leave		= otto_lag_port_leave,

	.port_pre_bridge_flags	= rtldsa_port_pre_bridge_flags,
	.port_bridge_flags	= rtldsa_port_bridge_flags,

	.cls_flower_add		= otto_tc_cls_flower_add,
	.cls_flower_del		= otto_tc_cls_flower_del,
	.cls_flower_stats	= otto_tc_cls_flower_stats,
};

const struct phylink_mac_ops rtldsa_93xx_phylink_mac_ops = {
	.mac_config		= rtldsa_93xx_phylink_mac_config,
	.mac_link_down		= rtldsa_93xx_phylink_mac_link_down,
	.mac_link_up		= rtldsa_93xx_phylink_mac_link_up,
};

const struct dsa_switch_ops rtldsa_93xx_switch_ops = {
	.get_tag_protocol	= rtldsa_get_tag_protocol,
	.setup			= rtldsa_93xx_setup,

	.phylink_get_caps	= rtldsa_phylink_get_caps,

	.get_strings		= otto_stats_get_strings,
	.get_ethtool_stats	= otto_stats_get_ethtool,
	.get_sset_count		= otto_stats_get_sset_count,
	.get_eth_phy_stats	= otto_stats_get_eth_phy,
	.get_eth_mac_stats	= otto_stats_get_eth_mac,
	.get_eth_ctrl_stats	= otto_stats_get_eth_ctrl,
	.get_rmon_stats		= otto_stats_get_rmon,
	.get_stats64		= otto_stats_get64,
	.get_pause_stats	= otto_stats_get_pause,

	.port_enable		= rtldsa_port_enable,
	.port_disable		= rtldsa_port_disable,

	.port_change_mtu	= rtldsa_port_change_mtu,
	.port_max_mtu		= rtldsa_port_max_mtu,

	.support_eee		= rtldsa_support_eee,
	.set_mac_eee		= rtldsa_set_mac_eee,

	.set_ageing_time	= rtldsa_set_ageing_time,
	.port_bridge_join	= rtldsa_port_bridge_join,
	.port_bridge_leave	= rtldsa_port_bridge_leave,
	.port_stp_state_set	= rtldsa_port_stp_state_set,
	.port_fast_age		= rtldsa_port_fast_age,
	.port_mst_state_set	= rtldsa_port_mst_state_set,

	.port_vlan_filtering	= otto_vlan_port_filtering,
	.port_vlan_protocol	= rtldsa_port_vlan_protocol,
	.port_vlan_add		= otto_vlan_port_add,
	.port_vlan_del		= otto_vlan_port_del,
	.port_vlan_fast_age	= otto_vlan_port_fast_age,
	.vlan_msti_set		= otto_vlan_msti_set,

	.port_fdb_add		= rtldsa_port_fdb_add,
	.port_fdb_del		= rtldsa_port_fdb_del,
	.port_fdb_dump		= rtldsa_port_fdb_dump,

	.port_mdb_add		= rtldsa_93xx_port_mdb_add,
	.port_mdb_del		= rtldsa_port_mdb_del,

	.port_mirror_add	= rtldsa_port_mirror_add,
	.port_mirror_del	= rtldsa_port_mirror_del,

	.port_lag_change	= otto_lag_port_change,
	.port_lag_join		= otto_lag_port_join,
	.port_lag_leave		= otto_lag_port_leave,

	.port_pre_bridge_flags	= rtldsa_port_pre_bridge_flags,
	.port_bridge_flags	= rtldsa_port_bridge_flags,

	.cls_flower_add		= otto_tc_cls_flower_add,
	.cls_flower_del		= otto_tc_cls_flower_del,
	.cls_flower_stats	= otto_tc_cls_flower_stats,
};
