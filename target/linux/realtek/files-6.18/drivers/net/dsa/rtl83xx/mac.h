/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_MAC_H
#define _OTTO_MAC_H

#include <linux/bits.h>
#include <linux/phy.h>
#include <linux/types.h>

/* MAC definitions shared with family and DSA code. */
#define RTL838X_MAC_FORCE_MODE_CTRL		(0xa104)

#define RTL83XX_FORCE_EN			BIT(0)
#define RTL83XX_FORCE_LINK_EN			BIT(1)

#define RTL838X_DUPLEX_MODE			BIT(3)
#define RTL838X_SPEED_SHIFT			(4)
#define RTL838X_SPEED_MASK			(3 << RTL838X_SPEED_SHIFT)
#define RTL838X_TX_PAUSE_EN			BIT(6)
#define RTL838X_RX_PAUSE_EN			BIT(7)

#define RTL839X_DUPLEX_MODE			BIT(2)
#define RTL839X_SPEED_SHIFT			(3)
#define RTL839X_SPEED_MASK			(3 << RTL839X_SPEED_SHIFT)
#define RTL839X_TX_PAUSE_EN			BIT(5)
#define RTL839X_RX_PAUSE_EN			BIT(6)

#define RTL930X_FORCE_EN			BIT(0)
#define RTL930X_FORCE_LINK_EN			BIT(1)
#define RTL930X_DUPLEX_MODE			BIT(2)
#define RTL930X_SPEED_SHIFT			(3)
#define RTL930X_SPEED_MASK			(15 << RTL930X_SPEED_SHIFT)
#define RTL930X_TX_PAUSE_EN			BIT(7)
#define RTL930X_RX_PAUSE_EN			BIT(8)
#define RTL930X_MAC_FORCE_FC_EN			BIT(9)
#define RTL930X_MEDIA_SEL			BIT(16)

#define RTL931X_FORCE_LINK_EN			BIT(0)
#define RTL931X_FORCE_DUPLEX_EN			BIT(2)
#define RTL931X_FORCE_SPEED_EN			BIT(3)
#define RTL931X_MAC_FORCE_FC_EN			BIT(4)
#define RTL931X_FORCE_LINK			BIT(9)
#define RTL931X_DUPLEX_MODE			BIT(11)
#define RTL931X_SPEED_SHIFT			(12)
#define RTL931X_SPEED_MASK			(15 << RTL931X_SPEED_SHIFT)
#define RTL931X_TX_PAUSE_EN			BIT(16)
#define RTL931X_RX_PAUSE_EN			BIT(17)

/* MAC maximum packet length (jumbo frame) control.
 *
 * The switch MAC drops frames whose L2 length exceeds the configured maximum.
 * A family holds either one register per user port or a single one for the
 * whole switch. The length is a direct byte value held in two 14-bit fields
 * (high-speed links in [13:0], 10/100M links in [27:14]); bit 28 selects
 * whether VLAN tag bytes count towards the limit.
 */

/* RTL838x and RTL839x hold one limit for the whole switch instead, bounding
 * the CPU port with it. RTL838x mirrors it in a second register, and the
 * vendor SDK writes both (dal_maple_switch_maxPktLenLinkSpeed_set()).
 */
#define RTL838X_MAC_MAX_LEN_CTRL		(0xa9e0)
#define RTL838X_MAC_MAX_LEN_CTRL_DUP		(0x6b00)

/* RTL838x stops at what its datasheet gives, below the vendor SDK value */
#define RTL838X_MAX_FRAME			10000

/* MAC handling */
#define RTL838X_MAC_LINK_STS			(0xa188)

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

#define RTL839X_MAC_LINK_STS			(0x0390)

/* Largest frame each family switches; the length field would allow 16383 */
#define RTL930X_MAX_FRAME			12288

#define RTL930X_MAC_LINK_STS			(0xCB10)

#define RTL931X_MAX_FRAME			12288

#define RTL931X_MAC_LINK_STS			(0x0EC0)

struct dsa_switch;
struct ethtool_keee;
struct otto_dsa_priv;
struct phylink_config;
struct phylink_mac_ops;

enum rtldsa_mac_link_state_source {
	RTLDSA_MAC_LINK_STATE_SOURCE_PCS,
	RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
};

struct rtldsa_mac_force_mode_cfg {
	u32 force_en_mask;
	u32 fc_force_en_mask;
	u32 link_up_mask;
	u32 duplex_mask;
	u32 speed_mask;
	u32 tx_pause_mask;
	u32 rx_pause_mask;
	u32 media_mask;
};

int rtl838x_mac_force_mode_ctrl(int p);
int rtl838x_mac_port_ctrl(int p);
void rtldsa_838x_set_mac_eee(struct otto_dsa_priv *priv, int port, bool enable);
void rtl838x_init_eee(struct otto_dsa_priv *priv, bool enable);

int rtl839x_mac_force_mode_ctrl(int p);
int rtl839x_mac_port_ctrl(int p);
void rtldsa_839x_set_mac_eee(struct otto_dsa_priv *priv, int port, bool enable);
void rtl839x_init_eee(struct otto_dsa_priv *priv, bool enable);

int rtl930x_mac_force_mode_ctrl(int p);
int rtl930x_mac_port_ctrl(int p);
int rtl930x_mac_max_len_reg(int p);
void rtldsa_930x_set_mac_eee(struct otto_dsa_priv *priv, int port, bool enable);
void rtl930x_init_eee(struct otto_dsa_priv *priv, bool enable);
int rtldsa_930x_mac_link_state_source_set(struct otto_dsa_priv *priv, int port,
					 enum rtldsa_mac_link_state_source source,
					 phy_interface_t interface);

int rtl931x_mac_force_mode_ctrl(int p);
int rtl931x_mac_port_ctrl(int p);
int rtl931x_mac_max_len_reg(int p);
int rtldsa_931x_mac_link_state_source_set(struct otto_dsa_priv *priv, int port,
					 enum rtldsa_mac_link_state_source source,
					 phy_interface_t interface);

void rtldsa_phylink_get_caps(struct dsa_switch *ds, int port,
			    struct phylink_config *config);
int rtldsa_port_max_mtu(struct dsa_switch *ds, int port);
int rtldsa_port_change_mtu(struct dsa_switch *ds, int port, int new_mtu);
bool rtldsa_support_eee(struct dsa_switch *ds, int port);
int rtldsa_set_mac_eee(struct dsa_switch *ds, int port, struct ethtool_keee *e);

extern const struct phylink_mac_ops rtldsa_83xx_phylink_mac_ops;
extern const struct phylink_mac_ops rtldsa_93xx_phylink_mac_ops;

#endif /* _OTTO_MAC_H */
