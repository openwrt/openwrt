// SPDX-License-Identifier: GPL-2.0-only

#include <net/dsa.h>
#include <linux/if_bridge.h>

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl83xx.h"


extern struct rtl83xx_soc_info soc_info;


static void rtl83xx_print_matrix(void)
{
	unsigned volatile int *ptr8;
	volatile u64 *ptr9;
	int i;

	if (soc_info.family == RTL8380_FAMILY_ID) {
		ptr8 = RTL838X_SW_BASE + RTL838X_PORT_ISO_CTRL(0);
		for (i = 0; i < 28; i += 8)
			pr_debug("> %8x %8x %8x %8x %8x %8x %8x %8x\n",
				ptr8[i + 0], ptr8[i + 1], ptr8[i + 2], ptr8[i + 3],
				ptr8[i + 4], ptr8[i + 5], ptr8[i + 6], ptr8[i + 7]);
		pr_debug("CPU_PORT> %8x\n", ptr8[28]);
	} else {
		ptr9 = RTL838X_SW_BASE + RTL839X_PORT_ISO_CTRL(0);
		for (i = 0; i < 52; i += 4)
			pr_debug("> %16llx %16llx %16llx %16llx\n",
				ptr9[i + 0], ptr9[i + 1], ptr9[i + 2], ptr9[i + 3]);
		pr_debug("CPU_PORT> %16llx\n", ptr9[52]);
	}

}

static void rtl83xx_init_stats(struct rtl838x_switch_priv *priv)
{
	mutex_lock(&priv->reg_mutex);

	/* Enable statistics module: all counters plus debug.
	 * On RTL839x all counters are enabled by default
	 */
	if (priv->family_id == RTL8380_FAMILY_ID)
		sw_w32_mask(0, 3, RTL838X_STAT_CTRL);

	/* Reset statistics counters */
	sw_w32_mask(0, 1, priv->r->stat_rst);

	mutex_unlock(&priv->reg_mutex);
}

static void rtl83xx_write_cam(int idx, u32 *r)
{
	u32 cmd = BIT(16) /* Execute cmd */
		| BIT(15) /* Read */
		| BIT(13) /* Table type 0b01 */
		| (idx & 0x3f);

	sw_w32(r[0], RTL838X_TBL_ACCESS_L2_DATA(0));
	sw_w32(r[1], RTL838X_TBL_ACCESS_L2_DATA(1));
	sw_w32(r[2], RTL838X_TBL_ACCESS_L2_DATA(2));

	sw_w32(cmd, RTL838X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL838X_TBL_ACCESS_L2_CTRL) & BIT(16));
}

static u64 rtl83xx_hash_key(struct rtl838x_switch_priv *priv, u64 mac, u32 vid)
{
	if (priv->family_id == RTL8380_FAMILY_ID)
		return rtl838x_hash(priv, mac << 12 | vid);
	else
		return rtl839x_hash(priv, mac << 12 | vid);
}

static void rtl83xx_write_hash(int idx, u32 *r)
{
	u32 cmd = BIT(16) /* Execute cmd */
		| 0 << 15 /* Write */
		| 0 << 13 /* Table type 0b00 */
		| (idx & 0x1fff);

	sw_w32(0, RTL838X_TBL_ACCESS_L2_DATA(0));
	sw_w32(0, RTL838X_TBL_ACCESS_L2_DATA(1));
	sw_w32(0, RTL838X_TBL_ACCESS_L2_DATA(2));
	sw_w32(cmd, RTL838X_TBL_ACCESS_L2_CTRL);
	do { }  while (sw_r32(RTL838X_TBL_ACCESS_L2_CTRL) & BIT(16));
}

static void rtl83xx_enable_phy_polling(struct rtl838x_switch_priv *priv)
{
	int i;
	u64 v = 0;

	msleep(1000);
	/* Enable all ports with a PHY, including the SFP-ports */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy)
			v |= BIT(i);
	}

	pr_debug("%s: %16llx\n", __func__, v);
	priv->r->set_port_reg_le(v, priv->r->smi_poll_ctrl);

	/* PHY update complete */
	if (priv->family_id == RTL8390_FAMILY_ID)
		sw_w32_mask(0, BIT(7), RTL839X_SMI_GLB_CTRL);
	else
		sw_w32_mask(0, 0x8000, RTL838X_SMI_GLB_CTRL);
}

const struct rtl83xx_mib_desc rtl83xx_mib[] = {
	MIB_DESC(2, 0xf8, "ifInOctets"),
	MIB_DESC(2, 0xf0, "ifOutOctets"),
	MIB_DESC(1, 0xec, "dot1dTpPortInDiscards"),
	MIB_DESC(1, 0xe8, "ifInUcastPkts"),
	MIB_DESC(1, 0xe4, "ifInMulticastPkts"),
	MIB_DESC(1, 0xe0, "ifInBroadcastPkts"),
	MIB_DESC(1, 0xdc, "ifOutUcastPkts"),
	MIB_DESC(1, 0xd8, "ifOutMulticastPkts"),
	MIB_DESC(1, 0xd4, "ifOutBroadcastPkts"),
	MIB_DESC(1, 0xd0, "ifOutDiscards"),
	MIB_DESC(1, 0xcc, ".3SingleCollisionFrames"),
	MIB_DESC(1, 0xc8, ".3MultipleCollisionFrames"),
	MIB_DESC(1, 0xc4, ".3DeferredTransmissions"),
	MIB_DESC(1, 0xc0, ".3LateCollisions"),
	MIB_DESC(1, 0xbc, ".3ExcessiveCollisions"),
	MIB_DESC(1, 0xb8, ".3SymbolErrors"),
	MIB_DESC(1, 0xb4, ".3ControlInUnknownOpcodes"),
	MIB_DESC(1, 0xb0, ".3InPauseFrames"),
	MIB_DESC(1, 0xac, ".3OutPauseFrames"),
	MIB_DESC(1, 0xa8, "DropEvents"),
	MIB_DESC(1, 0xa4, "tx_BroadcastPkts"),
	MIB_DESC(1, 0xa0, "tx_MulticastPkts"),
	MIB_DESC(1, 0x9c, "CRCAlignErrors"),
	MIB_DESC(1, 0x98, "tx_UndersizePkts"),
	MIB_DESC(1, 0x94, "rx_UndersizePkts"),
	MIB_DESC(1, 0x90, "rx_UndersizedropPkts"),
	MIB_DESC(1, 0x8c, "tx_OversizePkts"),
	MIB_DESC(1, 0x88, "rx_OversizePkts"),
	MIB_DESC(1, 0x84, "Fragments"),
	MIB_DESC(1, 0x80, "Jabbers"),
	MIB_DESC(1, 0x7c, "Collisions"),
	MIB_DESC(1, 0x78, "tx_Pkts64Octets"),
	MIB_DESC(1, 0x74, "rx_Pkts64Octets"),
	MIB_DESC(1, 0x70, "tx_Pkts65to127Octets"),
	MIB_DESC(1, 0x6c, "rx_Pkts65to127Octets"),
	MIB_DESC(1, 0x68, "tx_Pkts128to255Octets"),
	MIB_DESC(1, 0x64, "rx_Pkts128to255Octets"),
	MIB_DESC(1, 0x60, "tx_Pkts256to511Octets"),
	MIB_DESC(1, 0x5c, "rx_Pkts256to511Octets"),
	MIB_DESC(1, 0x58, "tx_Pkts512to1023Octets"),
	MIB_DESC(1, 0x54, "rx_Pkts512to1023Octets"),
	MIB_DESC(1, 0x50, "tx_Pkts1024to1518Octets"),
	MIB_DESC(1, 0x4c, "rx_StatsPkts1024to1518Octets"),
	MIB_DESC(1, 0x48, "tx_Pkts1519toMaxOctets"),
	MIB_DESC(1, 0x44, "rx_Pkts1519toMaxOctets"),
	MIB_DESC(1, 0x40, "rxMacDiscards")
};


/* DSA callbacks */


static enum dsa_tag_protocol rtl83xx_get_tag_protocol(struct dsa_switch *ds, int port)
{
	/* The switch does not tag the frames, instead internally the header
	 * structure for each packet is tagged accordingly.
	 */
	return DSA_TAG_PROTO_TRAILER;
}

static int rtl83xx_setup(struct dsa_switch *ds)
{
	int i;
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 port_bitmap = BIT_ULL(priv->cpu_port);

	pr_debug("%s called\n", __func__);

	/* Disable MAC polling the PHY so that we can start configuration */
	priv->r->set_port_reg_le(0ULL, priv->r->smi_poll_ctrl);

	for (i = 0; i < ds->num_ports; i++)
		priv->ports[i].enable = false;
	priv->ports[priv->cpu_port].enable = true;

	/* Isolate ports from each other: traffic only CPU <-> port */
	/* Setting bit j in register RTL838X_PORT_ISO_CTRL(i) allows
	 * traffic from source port i to destination port j
	 */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy) {
			priv->r->set_port_reg_be(BIT_ULL(priv->cpu_port) | BIT(i),
					      priv->r->port_iso_ctrl(i));
			port_bitmap |= BIT_ULL(i);
		}
	}
	priv->r->set_port_reg_be(port_bitmap, priv->r->port_iso_ctrl(priv->cpu_port));

	rtl83xx_print_matrix();

	rtl83xx_init_stats(priv);

	ds->configure_vlan_while_not_filtering = true;

	/* Enable MAC Polling PHY again */
	rtl83xx_enable_phy_polling(priv);
	pr_debug("Please wait until PHY is settled\n");
	msleep(1000);
	return 0;
}

static void rtl83xx_phylink_validate(struct dsa_switch *ds, int port,
				     unsigned long *supported,
				     struct phylink_link_state *state)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };

	pr_debug("In %s port %d", __func__, port);

	if (!phy_interface_mode_is_rgmii(state->interface) &&
	    state->interface != PHY_INTERFACE_MODE_1000BASEX &&
	    state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII &&
	    state->interface != PHY_INTERFACE_MODE_GMII &&
	    state->interface != PHY_INTERFACE_MODE_QSGMII &&
	    state->interface != PHY_INTERFACE_MODE_INTERNAL &&
	    state->interface != PHY_INTERFACE_MODE_SGMII) {
		bitmap_zero(supported, __ETHTOOL_LINK_MODE_MASK_NBITS);
		dev_err(ds->dev,
			"Unsupported interface: %d for port %d\n",
			state->interface, port);
		return;
	}

	/* Allow all the expected bits */
	phylink_set(mask, Autoneg);
	phylink_set_port_modes(mask);
	phylink_set(mask, Pause);
	phylink_set(mask, Asym_Pause);

	/* With the exclusion of MII and Reverse MII, we support Gigabit,
	 * including Half duplex
	 */
	if (state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII) {
		phylink_set(mask, 1000baseT_Full);
		phylink_set(mask, 1000baseT_Half);
	}

	/* On both the 8380 and 8382, ports 24-27 are SFP ports */
	if (port >= 24 && port <= 27 && priv->family_id == RTL8380_FAMILY_ID)
		phylink_set(mask, 1000baseX_Full);

	phylink_set(mask, 10baseT_Half);
	phylink_set(mask, 10baseT_Full);
	phylink_set(mask, 100baseT_Half);
	phylink_set(mask, 100baseT_Full);

	bitmap_and(supported, supported, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
	bitmap_and(state->advertising, state->advertising, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
}

static int rtl83xx_phylink_mac_link_state(struct dsa_switch *ds, int port,
					  struct phylink_link_state *state)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 speed;

	if (port < 0 || port > priv->cpu_port)
		return -EINVAL;

	state->link = 0;
	if (priv->r->get_port_reg_le(priv->r->mac_link_sts) & BIT_ULL(port))
		state->link = 1;
	state->duplex = 0;
	if (priv->r->get_port_reg_le(priv->r->mac_link_dup_sts) & BIT_ULL(port))
		state->duplex = 1;

	speed = priv->r->get_port_reg_le(priv->r->mac_link_spd_sts(port));
	speed >>= (port % 16) << 1;
	switch (speed & 0x3) {
	case 0:
		state->speed = SPEED_10;
		break;
	case 1:
		state->speed = SPEED_100;
		break;
	case 2:
		state->speed = SPEED_1000;
		break;
	case 3:
		if (port == 24 || port == 26) /* Internal serdes */
			state->speed = SPEED_2500;
		else
			state->speed = SPEED_100; /* Is in fact 500Mbit */
	}

	state->pause &= (MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (priv->r->get_port_reg_le(priv->r->mac_rx_pause_sts) & BIT_ULL(port))
		state->pause |= MLO_PAUSE_RX;
	if (priv->r->get_port_reg_le(priv->r->mac_tx_pause_sts) & BIT_ULL(port))
		state->pause |= MLO_PAUSE_TX;
	return 1;
}

static void rtl83xx_phylink_mac_config(struct dsa_switch *ds, int port,
					unsigned int mode,
					const struct phylink_link_state *state)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u32 reg;
	int speed_bit = priv->family_id == RTL8380_FAMILY_ID ? 4 : 3;

	pr_debug("%s port %d, mode %x\n", __func__, port, mode);

	if (port == priv->cpu_port) {
		/* Set Speed, duplex, flow control
		 * FORCE_EN | LINK_EN | NWAY_EN | DUP_SEL
		 * | SPD_SEL = 0b10 | FORCE_FC_EN | PHY_MASTER_SLV_MANUAL_EN
		 * | MEDIA_SEL
		 */
		if (priv->family_id == RTL8380_FAMILY_ID) {
			sw_w32(0x6192F, priv->r->mac_force_mode_ctrl(priv->cpu_port));
			/* allow CRC errors on CPU-port */
			sw_w32_mask(0, 0x8, RTL838X_MAC_PORT_CTRL(priv->cpu_port));
		} else {
			sw_w32_mask(0, 3, priv->r->mac_force_mode_ctrl(priv->cpu_port));
		}
		return;
	}

	reg = sw_r32(priv->r->mac_force_mode_ctrl(port));
	/* Auto-Negotiation does not work for MAC in RTL8390 */
	if (priv->family_id == RTL8380_FAMILY_ID) {
		if (mode == MLO_AN_PHY) {
			pr_debug("PHY autonegotiates\n");
			reg |= BIT(2);
			sw_w32(reg, priv->r->mac_force_mode_ctrl(port));
			return;
		}
	}

	if (mode != MLO_AN_FIXED)
		pr_debug("Fixed state.\n");

	if (priv->family_id == RTL8380_FAMILY_ID) {
		/* Clear id_mode_dis bit, and the existing port mode, let
		 * RGMII_MODE_EN bet set by mac_link_{up,down}
		 */
		reg &= ~(RX_PAUSE_EN | TX_PAUSE_EN);

		if (state->pause & MLO_PAUSE_TXRX_MASK) {
			if (state->pause & MLO_PAUSE_TX)
				reg |= TX_PAUSE_EN;
			reg |= RX_PAUSE_EN;
		}
	}

	reg &= ~(3 << speed_bit);
	switch (state->speed) {
	case SPEED_1000:
		reg |= 2 << speed_bit;
		break;
	case SPEED_100:
		reg |= 1 << speed_bit;
		break;
	}

	if (priv->family_id == RTL8380_FAMILY_ID) {
		reg &= ~(DUPLEX_FULL | FORCE_LINK_EN);
		if (state->link)
			reg |= FORCE_LINK_EN;
		if (state->duplex == DUPLEX_FULL)
			reg |= DUPLX_MODE;
	}

	// Disable AN
	if (priv->family_id == RTL8380_FAMILY_ID)
		reg &= ~BIT(2);
	sw_w32(reg, priv->r->mac_force_mode_ctrl(port));
}

static void rtl83xx_phylink_mac_link_down(struct dsa_switch *ds, int port,
				     unsigned int mode,
				     phy_interface_t interface)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	/* Stop TX/RX to port */
	sw_w32_mask(0x3, 0, priv->r->mac_port_ctrl(port));
}

static void rtl83xx_phylink_mac_link_up(struct dsa_switch *ds, int port,
				   unsigned int mode,
				   phy_interface_t interface,
				   struct phy_device *phydev)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	/* Restart TX/RX to port */
	sw_w32_mask(0, 0x3, priv->r->mac_port_ctrl(port));
}

static void rtl83xx_get_strings(struct dsa_switch *ds,
				int port, u32 stringset, u8 *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(rtl83xx_mib); i++)
		strncpy(data + i * ETH_GSTRING_LEN, rtl83xx_mib[i].name,
			ETH_GSTRING_LEN);
}

static void rtl83xx_get_ethtool_stats(struct dsa_switch *ds, int port,
				      uint64_t *data)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	const struct rtl83xx_mib_desc *mib;
	int i;
	u64 high;

	for (i = 0; i < ARRAY_SIZE(rtl83xx_mib); i++) {
		mib = &rtl83xx_mib[i];

		data[i] = sw_r32(priv->r->stat_port_std_mib(port) + 252 - mib->offset);
		if (mib->size == 2) {
			high = sw_r32(priv->r->stat_port_std_mib(port) + 252 - mib->offset - 4);
			data[i] |= high << 32;
		}
	}
}

static int rtl83xx_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(rtl83xx_mib);
}

static int rtl83xx_port_enable(struct dsa_switch *ds, int port,
				struct phy_device *phydev)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s: %x %d", __func__, (u32) priv, port);
	priv->ports[port].enable = true;

	/* enable inner tagging on egress, do not keep any tags */
	sw_w32(1, priv->r->vlan_port_tag_sts_ctrl(port));

	if (dsa_is_cpu_port(ds, port))
		return 0;

	/* add port to switch mask of CPU_PORT */
	priv->r->mask_port_reg_be(0ULL, BIT_ULL(port), priv->r->port_iso_ctrl(priv->cpu_port));

	/* add all other ports in the same bridge to switch mask of port */
	priv->r->mask_port_reg_be(0ULL, priv->ports[port].pm, priv->r->port_iso_ctrl(port));

	return 0;
}

static void rtl83xx_port_disable(struct dsa_switch *ds, int port)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);
	/* you can only disable user ports */
	if (!dsa_is_user_port(ds, port))
		return;

	/* remove port from switch mask of CPU_PORT */
	priv->r->mask_port_reg_be(BIT_ULL(port), 0, priv->r->port_iso_ctrl(priv->cpu_port));

	/* remove all other ports in the same bridge from switch mask of port */
	priv->r->mask_port_reg_be(priv->ports[port].pm, 0LL, priv->r->port_iso_ctrl(port));

	priv->ports[port].enable = false;
}

static int rtl83xx_get_mac_eee(struct dsa_switch *ds, int port,
			       struct ethtool_eee *e)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s: port %d", __func__, port);
	e->supported = SUPPORTED_100baseT_Full | SUPPORTED_1000baseT_Full;
	if (sw_r32(priv->r->mac_force_mode_ctrl(port)) & BIT(9))
		e->advertised |= ADVERTISED_100baseT_Full;

	if (sw_r32(priv->r->mac_force_mode_ctrl(port)) & BIT(10))
		e->advertised |= ADVERTISED_1000baseT_Full;

	e->eee_enabled = priv->ports[port].eee_enabled;
	pr_debug("enabled: %d, active %x\n", e->eee_enabled, e->advertised);

	if (sw_r32(RTL838X_MAC_EEE_ABLTY) & BIT(port)) {
		e->lp_advertised = ADVERTISED_100baseT_Full;
		e->lp_advertised |= ADVERTISED_1000baseT_Full;
	}

	e->eee_active = !!(e->advertised & e->lp_advertised);
	pr_debug("active: %d, lp %x\n", e->eee_active, e->lp_advertised);

	return 0;
}

static int rtl83xx_set_mac_eee(struct dsa_switch *ds, int port,
			       struct ethtool_eee *e)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s: port %d", __func__, port);
	if (e->eee_enabled) {
		pr_debug("Globally enabling EEE\n");
		sw_w32_mask(0x4, 0, RTL838X_SMI_GLB_CTRL);
	}
	if (e->eee_enabled) {
		pr_debug("Enabling EEE for MAC %d\n", port);
		sw_w32_mask(0, 3 << 9, priv->r->mac_force_mode_ctrl(port));
		sw_w32_mask(0, BIT(port), RTL838X_EEE_PORT_TX_EN);
		sw_w32_mask(0, BIT(port), RTL838X_EEE_PORT_RX_EN);
		priv->ports[port].eee_enabled = true;
		e->eee_enabled = true;
	} else {
		pr_debug("Disabling EEE for MAC %d\n", port);
		sw_w32_mask(3 << 9, 0, priv->r->mac_force_mode_ctrl(port));
		sw_w32_mask(BIT(port), 0, RTL838X_EEE_PORT_TX_EN);
		sw_w32_mask(BIT(port), 0, RTL838X_EEE_PORT_RX_EN);
		priv->ports[port].eee_enabled = false;
		e->eee_enabled = false;
	}
	return 0;
}

/*
 * Set Switch L2 Aging time, t is time in milliseconds
 * t = 0: aging is disabled
 */
static int rtl83xx_set_l2aging(struct dsa_switch *ds, u32 t)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int t_max = priv->family_id == RTL8380_FAMILY_ID ? 0x7fffff : 0x1FFFFF;

	/* Convert time in mseconds to internal value */
	if (t > 0x10000000) { /* Set to maximum */
		t = t_max;
	} else {
		if (priv->family_id == RTL8380_FAMILY_ID)
			t = ((t * 625) / 1000 + 127) / 128;
		else
			t = (t * 5 + 2) / 3;
	}
	sw_w32(t, priv->r->l2_ctrl_1);
	return 0;
}

static int rtl83xx_port_bridge_join(struct dsa_switch *ds, int port,
					struct net_device *bridge)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 port_bitmap = BIT_ULL(priv->cpu_port);
	int i;

	pr_debug("%s %x: %d %llx", __func__, (u32)priv, port, port_bitmap);
	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < ds->num_ports; i++) {
		/* Add this port to the port matrix of the other ports in the
		 * same bridge. If the port is disabled, port matrix is kept
		 * and not being setup until the port becomes enabled.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (dsa_to_port(ds, i)->bridge_dev != bridge)
				continue;
			if (priv->ports[i].enable)
				priv->r->mask_port_reg_be(0, BIT_ULL(port),
						       priv->r->port_iso_ctrl(i));
			priv->ports[i].pm |= BIT_ULL(port);

			port_bitmap |= BIT_ULL(i);
		}
	}

	/* Add all other ports to this port matrix. */
	if (priv->ports[port].enable) {
		priv->r->mask_port_reg_be(0, BIT_ULL(port),
					priv->r->port_iso_ctrl(priv->cpu_port));
		priv->r->mask_port_reg_be(0, port_bitmap,
					priv->r->port_iso_ctrl(port));
	}
	priv->ports[port].pm |= port_bitmap;
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void rtl83xx_port_bridge_leave(struct dsa_switch *ds, int port,
					struct net_device *bridge)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 port_bitmap = BIT_ULL(priv->cpu_port);
	int i;

	pr_debug("%s %x: %d", __func__, (u32)priv, port);
	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < ds->num_ports; i++) {
		/* Remove this port from the port matrix of the other ports
		 * in the same bridge. If the port is disabled, port matrix
		 * is kept and not being setup until the port becomes enabled.
		 * And the other port's port matrix cannot be broken when the
		 * other port is still a VLAN-aware port.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (dsa_to_port(ds, i)->bridge_dev != bridge)
				continue;
			if (priv->ports[i].enable)
				priv->r->mask_port_reg_be(BIT_ULL(port), 0,
						       priv->r->port_iso_ctrl(i));
			priv->ports[i].pm &= ~BIT_ULL(port);

			port_bitmap &= ~BIT_ULL(i);
		}
	}

	/* Add all other ports to this port matrix. */
	if (priv->ports[port].enable)
		priv->r->mask_port_reg_be(0, port_bitmap, priv->r->port_iso_ctrl(port));
	priv->ports[port].pm &= ~port_bitmap;

	mutex_unlock(&priv->reg_mutex);
}

static void rtl83xx_port_stp_state_set(struct dsa_switch *ds, int port,
				       u8 state)
{
	u32 cmd, msti = 0;
	u32 port_state[4];
	int index, bit, i;
	int pos = port;
	struct rtl838x_switch_priv *priv = ds->priv;
	int n = priv->family_id == RTL8380_FAMILY_ID ? 2 : 4;

	pr_debug("%s: port %d state %2x\n", __func__, port, state);

	/* CPU PORT can only be configured on RTL838x */
	if (port >= priv->cpu_port || port > 51)
		return;

	mutex_lock(&priv->reg_mutex);

	/* For the RTL839x, the bits are left-aligned in the 128 bit field */
	if (priv->family_id == RTL8390_FAMILY_ID)
		pos += 12;

	index = n - (pos >> 4) - 1;
	bit = (pos << 1) % 32;

	if (priv->family_id == RTL8380_FAMILY_ID) {
		cmd = BIT(15) /* Execute cmd */
			| BIT(14) /* Read */
			| 2 << 12 /* Table type 0b10 */
			| (msti & 0xfff);
	} else {
		cmd = BIT(16) /* Execute cmd */
			| 0 << 15 /* Read */
			| 5 << 12 /* Table type 0b101 */
			| (msti & 0xfff);
	}
	priv->r->exec_tbl0_cmd(cmd);

	for (i = 0; i < n; i++)
		port_state[i] = sw_r32(priv->r->tbl_access_data_0(i));

	pr_debug("Current state, port %d: %d\n", port, (port_state[index] >> bit) & 3);
	port_state[index] &= ~(3 << bit);

	switch (state) {
	case BR_STATE_DISABLED: /* 0 */
		port_state[index] |= (0 << bit);
		break;
	case BR_STATE_BLOCKING:  /* 4 */
	case BR_STATE_LISTENING: /* 1 */
		port_state[index] |= (1 << bit);
		break;
	case BR_STATE_LEARNING: /* 2 */
		port_state[index] |= (2 << bit);
		break;
	case BR_STATE_FORWARDING: /* 3*/
		port_state[index] |= (3 << bit);
	default:
		break;
	}

	if (priv->family_id == RTL8380_FAMILY_ID) {
		cmd = BIT(15) /* Execute cmd */
			| 0 << 14 /* Write */
			| 2 << 12 /* Table type 0b10 */
			| (msti & 0xfff);
	} else {
		cmd = 1 << 16 /* Execute cmd */
			| BIT(15) /* Write */
			| 5 << 12 /* Table type 0b101 */
			| (msti & 0xfff);
	}
	for (i = 0; i < n; i++)
		sw_w32(port_state[i], priv->r->tbl_access_data_0(i));
	priv->r->exec_tbl0_cmd(cmd);

	mutex_unlock(&priv->reg_mutex);
}

static void rtl83xx_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	int s = priv->family_id == RTL8390_FAMILY_ID ? 2 : 0;

	pr_debug("FAST AGE port %d\n", port);
	mutex_lock(&priv->reg_mutex);
	/* RTL838X_L2_TBL_FLUSH_CTRL register bits, 839x has 1 bit larger
	 * port fields:
	 * 0-4: Replacing port
	 * 5-9: Flushed/replaced port
	 * 10-21: FVID
	 * 22: Entry types: 1: dynamic, 0: also static
	 * 23: Match flush port
	 * 24: Match FVID
	 * 25: Flush (0) or replace (1) L2 entries
	 * 26: Status of action (1: Start, 0: Done)
	 */
	sw_w32(1 << (26 + s) | 1 << (23 + s) | port << (5 + (s / 2)), priv->r->l2_tbl_flush_ctrl);

	do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & (1 << (26 + s)));

	mutex_unlock(&priv->reg_mutex);
}

static int rtl83xx_vlan_filtering(struct dsa_switch *ds, int port,
				  bool vlan_filtering)
{
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s: port %d\n", __func__, port);
	mutex_lock(&priv->reg_mutex);

	if (vlan_filtering) {
		/* Enable ingress and egress filtering */
		if (port != priv->cpu_port)
			sw_w32_mask(0b10 << ((port % 16) << 1), 0b01 << ((port % 16) << 1),
				    priv->r->vlan_port_igr_filter(port));
		sw_w32_mask(0, 1 << (port % 32), priv->r->vlan_port_egr_filter(port));
	} else {
		/* Disable ingress and egress filtering */
		if (port != priv->cpu_port)
			sw_w32_mask(0b11 << ((port % 16) << 1), 0,
				    priv->r->vlan_port_igr_filter(port));
		sw_w32_mask(1 << (port % 32), 0, priv->r->vlan_port_egr_filter(port));
	}

	/* Do we need to do something to the CPU-Port, too? */
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtl83xx_vlan_prepare(struct dsa_switch *ds, int port,
				const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl838x_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("%s: port %d\n", __func__, port);

	mutex_lock(&priv->reg_mutex);

	if (priv->family_id == RTL8380_FAMILY_ID)
		rtl838x_vlan_profile_dump(0);
	else
		rtl839x_vlan_profile_dump(0);

	priv->r->vlan_tables_read(0, &info);

	pr_debug("Tagged ports %llx, untag %llx, prof %x, MC# %d, UC# %d, FID %x\n",
		info.tagged_ports, info.untagged_ports, info.profile_id,
		info.hash_mc_fid, info.hash_uc_fid, info.fid);

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static void rtl83xx_vlan_add(struct dsa_switch *ds, int port,
			    const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl838x_vlan_info info = {};
	struct rtl838x_switch_priv *priv = ds->priv;
	int v;

	pr_debug("%s port %d, vid_end %d, vid_end %d, flags %x\n", __func__,
		port, vlan->vid_begin, vlan->vid_end, vlan->flags);

	if (vlan->vid_begin > 4095 || vlan->vid_end > 4095) {
		dev_err(priv->dev, "VLAN out of range: %d - %d",
			vlan->vid_begin, vlan->vid_end);
		return;
	}

	mutex_lock(&priv->reg_mutex);

	if (vlan->flags & BRIDGE_VLAN_INFO_PVID) {
		/* Set both inner and outer PVID of the port */
		sw_w32((vlan->vid_end << 16) | vlan->vid_end << 2, priv->r->vlan_port_pb(port));
		priv->ports[port].pvid = vlan->vid_end;
	}

	for (v = vlan->vid_begin; v <= vlan->vid_end; v++) {
		/* Get port memberships of this vlan */
		priv->r->vlan_tables_read(v, &info);

		/* new VLAN? */
		if (!info.tagged_ports) {
			info.fid = 0;
			info.hash_mc_fid = false;
			info.hash_uc_fid = false;
			info.profile_id = 0;
		}

		/* sanitize untagged_ports - must be a subset */
		if (info.untagged_ports & ~info.tagged_ports)
			info.untagged_ports = 0;

		info.tagged_ports |= BIT_ULL(port);
		if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
			info.untagged_ports |= BIT_ULL(port);

		priv->r->vlan_set_untagged(v, info.untagged_ports);
		pr_debug("Untagged ports, VLAN %d: %llx\n", v, info.untagged_ports);

		priv->r->vlan_set_tagged(v, &info);
		pr_debug("Tagged ports, VLAN %d: %llx\n", v, info.tagged_ports);
	}

	mutex_unlock(&priv->reg_mutex);
}

static int rtl83xx_vlan_del(struct dsa_switch *ds, int port,
			    const struct switchdev_obj_port_vlan *vlan)
{
	struct rtl838x_vlan_info info;
	struct rtl838x_switch_priv *priv = ds->priv;
	int v;
	u16 pvid;

	pr_debug("%s: port %d, vid_end %d, vid_end %d, flags %x\n", __func__,
		port, vlan->vid_begin, vlan->vid_end, vlan->flags);

	if (vlan->vid_begin > 4095 || vlan->vid_end > 4095) {
		dev_err(priv->dev, "VLAN out of range: %d - %d",
			vlan->vid_begin, vlan->vid_end);
		return -ENOTSUPP;
	}

	mutex_lock(&priv->reg_mutex);
	pvid = priv->ports[port].pvid;

	for (v = vlan->vid_begin; v <= vlan->vid_end; v++) {
		/* Reset to default if removing the current PVID */
		if (v == pvid)
			sw_w32(0, priv->r->vlan_port_pb(port));

		/* Get port memberships of this vlan */
		priv->r->vlan_tables_read(v, &info);

		/* remove port from both tables */
		info.untagged_ports &= (~BIT_ULL(port));
		/* always leave vid 1 */
		if (v != 1)
			info.tagged_ports &= (~BIT_ULL(port));

		priv->r->vlan_set_untagged(v, info.untagged_ports);
		pr_debug("Untagged ports, VLAN %d: %llx\n", v, info.untagged_ports);

		priv->r->vlan_set_tagged(v, &info);
		pr_debug("Tagged ports, VLAN %d: %llx\n", v, info.tagged_ports);
	}
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtl83xx_port_fdb_add(struct dsa_switch *ds, int port,
				const unsigned char *addr, u16 vid)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	u32 key = rtl83xx_hash_key(priv, mac, vid);
	struct rtl838x_l2_entry e;
	u32 r[3];
	u64 entry;
	int idx = -1, err = 0, i;

	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < 4; i++) {
		entry = priv->r->read_l2_entry_using_hash(key, i, &e);
		if (!e.valid) {
			idx = (key << 2) | i;
			break;
		}
		if ((entry & 0x0fffffffffffffffULL) == ((mac << 12) | vid)) {
			idx = (key << 2) | i;
			break;
		}
	}
	if (idx >= 0) {
		r[0] = 3 << 17 | port << 12; // Aging and  port
		r[0] |= vid;
		r[1] = mac >> 16;
		r[2] = (mac & 0xffff) << 12; /* rvid = 0 */
		rtl83xx_write_hash(idx, r);
		goto out;
	}

	/* Hash buckets full, try CAM */
	for (i = 0; i < 64; i++) {
		entry = priv->r->read_cam(i, &e);
		if (!e.valid) {
			if (idx < 0) /* First empty entry? */
				idx = i;
			break;
		} else if ((entry & 0x0fffffffffffffffULL) == ((mac << 12) | vid)) {
			pr_debug("Found entry in CAM\n");
			idx = i;
			break;
		}
	}
	if (idx >= 0) {
		r[0] = 3 << 17 | port << 12; // Aging
		r[0] |= vid;
		r[1] = mac >> 16;
		r[2] = (mac & 0xffff) << 12; /* rvid = 0 */
		rtl83xx_write_cam(idx, r);
		goto out;
	}
	err = -ENOTSUPP;
out:
	mutex_unlock(&priv->reg_mutex);
	return err;
}

static int rtl83xx_port_fdb_del(struct dsa_switch *ds, int port,
			   const unsigned char *addr, u16 vid)
{
	struct rtl838x_switch_priv *priv = ds->priv;
	u64 mac = ether_addr_to_u64(addr);
	u32 key = rtl83xx_hash_key(priv, mac, vid);
	struct rtl838x_l2_entry e;
	u32 r[3];
	u64 entry;
	int idx = -1, err = 0, i;

	pr_debug("In %s, mac %llx, vid: %d, key: %x\n", __func__, mac, vid, key);
	mutex_lock(&priv->reg_mutex);
	for (i = 0; i < 4; i++) {
		entry = priv->r->read_l2_entry_using_hash(key, i, &e);
		if (!e.valid)
			continue;
		if ((entry & 0x0fffffffffffffffULL) == ((mac << 12) | vid)) {
			idx = (key << 2) | i;
			break;
		}
	}

	if (idx >= 0) {
		r[0] = r[1] = r[2] = 0;
		rtl83xx_write_hash(idx, r);
		goto out;
	}

	/* Check CAM for spillover from hash buckets */
	for (i = 0; i < 64; i++) {
		entry = priv->r->read_cam(i, &e);
		if ((entry & 0x0fffffffffffffffULL) == ((mac << 12) | vid)) {
			idx = i;
			break;
		}
	}
	if (idx >= 0) {
		r[0] = r[1] = r[2] = 0;
		rtl83xx_write_cam(idx, r);
		goto out;
	}
	err = -ENOENT;
out:
	mutex_unlock(&priv->reg_mutex);
	return err;
}

static int rtl83xx_port_fdb_dump(struct dsa_switch *ds, int port,
				 dsa_fdb_dump_cb_t *cb, void *data)
{
	struct rtl838x_l2_entry e;
	struct rtl838x_switch_priv *priv = ds->priv;
	int i;
	u32 fid;
	u32 pkey;
	u64 mac;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < priv->fib_entries; i++) {
		priv->r->read_l2_entry_using_hash(i >> 2, i & 0x3, &e);

		if (!e.valid)
			continue;

		if (e.port == port) {
			fid = (i & 0x3ff) | (e.rvid & ~0x3ff);
			mac = ether_addr_to_u64(&e.mac[0]);
			pkey = rtl838x_hash(priv, mac << 12 | fid);
			fid = (pkey & 0x3ff) | (fid & ~0x3ff);
			pr_debug("-> mac %016llx, fid: %d\n", mac, fid);
			cb(e.mac, e.vid, e.is_static, data);
		}
	}

	for (i = 0; i < 64; i++) {
		priv->r->read_cam(i, &e);

		if (!e.valid)
			continue;

		if (e.port == port)
			cb(e.mac, e.vid, e.is_static, data);
	}

	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static int rtl83xx_port_mirror_add(struct dsa_switch *ds, int port,
				   struct dsa_mall_mirror_tc_entry *mirror,
				   bool ingress)
{
	/* We support 4 mirror groups, one destination port per group */
	int group;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("In %s\n", __func__);

	for (group = 0; group < 4; group++) {
		if (priv->mirror_group_ports[group] == mirror->to_local_port)
			break;
	}
	if (group >= 4) {
		for (group = 0; group < 4; group++) {
			if (priv->mirror_group_ports[group] < 0)
				break;
		}
	}

	if (group >= 4)
		return -ENOSPC;

	pr_debug("Using group %d\n", group);
	mutex_lock(&priv->reg_mutex);

	if (priv->family_id == RTL8380_FAMILY_ID) {
		/* Enable mirroring to port across VLANs (bit 11) */
		sw_w32(1 << 11 | (mirror->to_local_port << 4) | 1, RTL838X_MIR_CTRL(group));
	} else {
		/* Enable mirroring to destination port */
		sw_w32((mirror->to_local_port << 4) | 1, RTL839X_MIR_CTRL(group));
	}

	if (ingress && (priv->r->get_port_reg_be(priv->r->mir_spm(group)) & (1ULL << port))) {
		mutex_unlock(&priv->reg_mutex);
		return -EEXIST;
	}
	if ((!ingress) && (priv->r->get_port_reg_be(priv->r->mir_dpm(group)) & (1ULL << port))) {
		mutex_unlock(&priv->reg_mutex);
		return -EEXIST;
	}

	if (ingress)
		priv->r->mask_port_reg_be(0, 1ULL << port, priv->r->mir_spm(group));
	else
		priv->r->mask_port_reg_be(0, 1ULL << port, priv->r->mir_dpm(group));

	priv->mirror_group_ports[group] = mirror->to_local_port;
	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static void rtl83xx_port_mirror_del(struct dsa_switch *ds, int port,
				    struct dsa_mall_mirror_tc_entry *mirror)
{
	int group = 0;
	struct rtl838x_switch_priv *priv = ds->priv;

	pr_debug("In %s\n", __func__);
	for (group = 0; group < 4; group++) {
		if (priv->mirror_group_ports[group] == mirror->to_local_port)
			break;
	}
	if (group >= 4)
		return;

	mutex_lock(&priv->reg_mutex);
	if (mirror->ingress) {
		/* Ingress, clear source port matrix */
		priv->r->mask_port_reg_be(1ULL << port, 0, priv->r->mir_spm(group));
	} else {
		/* Egress, clear destination port matrix */
		priv->r->mask_port_reg_be(1ULL << port, 0, priv->r->mir_dpm(group));
	}

	if (!(sw_r32(priv->r->mir_spm(group)) || sw_r32(priv->r->mir_dpm(group)))) {
		priv->mirror_group_ports[group] = -1;
		sw_w32(0, priv->r->mir_ctrl(group));
	}

	mutex_unlock(&priv->reg_mutex);
}

const struct dsa_switch_ops rtl83xx_switch_ops = {
	.get_tag_protocol	= rtl83xx_get_tag_protocol,
	.setup			= rtl83xx_setup,

	.phy_read		= rtl83xx_dsa_phy_read,
	.phy_write		= rtl83xx_dsa_phy_write,

	.phylink_validate	= rtl83xx_phylink_validate,
	.phylink_mac_link_state	= rtl83xx_phylink_mac_link_state,
	.phylink_mac_config	= rtl83xx_phylink_mac_config,
	.phylink_mac_link_down	= rtl83xx_phylink_mac_link_down,
	.phylink_mac_link_up	= rtl83xx_phylink_mac_link_up,

	.get_strings		= rtl83xx_get_strings,
	.get_ethtool_stats	= rtl83xx_get_ethtool_stats,
	.get_sset_count		= rtl83xx_get_sset_count,

	.port_enable		= rtl83xx_port_enable,
	.port_disable		= rtl83xx_port_disable,

	.get_mac_eee		= rtl83xx_get_mac_eee,
	.set_mac_eee		= rtl83xx_set_mac_eee,

	.set_ageing_time	= rtl83xx_set_l2aging,
	.port_bridge_join	= rtl83xx_port_bridge_join,
	.port_bridge_leave	= rtl83xx_port_bridge_leave,
	.port_stp_state_set	= rtl83xx_port_stp_state_set,
	.port_fast_age		= rtl83xx_fast_age,

	.port_vlan_filtering	= rtl83xx_vlan_filtering,
	.port_vlan_prepare	= rtl83xx_vlan_prepare,
	.port_vlan_add		= rtl83xx_vlan_add,
	.port_vlan_del		= rtl83xx_vlan_del,

	.port_fdb_add		= rtl83xx_port_fdb_add,
	.port_fdb_del		= rtl83xx_port_fdb_del,
	.port_fdb_dump		= rtl83xx_port_fdb_dump,

	.port_mirror_add	= rtl83xx_port_mirror_add,
	.port_mirror_del	= rtl83xx_port_mirror_del,
};

