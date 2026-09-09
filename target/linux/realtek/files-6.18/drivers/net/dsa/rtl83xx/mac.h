/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_MAC_H
#define _OTTO_MAC_H

#include <linux/bits.h>

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
#define RTL931X_FORCE_LINK			BIT(9)

enum rtldsa_mac_link_state_source {
	RTLDSA_MAC_LINK_STATE_SOURCE_PCS,
	RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
};

struct rtldsa_mac_force_mode_cfg {
	u32 force_en_mask;
	u32 link_up_mask;
	u32 duplex_mask;
	u32 speed_mask;
	u32 tx_pause_mask;
	u32 rx_pause_mask;
	u32 media_mask;
};

#endif /* _OTTO_MAC_H */
