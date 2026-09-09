/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_MAC_H
#define _OTTO_MAC_H

#include <linux/bits.h>

/* MAC definitions shared with family and DSA code. */
#define RTL838X_MAC_FORCE_MODE_CTRL		(0xa104)

#define RTL83XX_FORCE_EN			BIT(0)
#define RTL83XX_FORCE_LINK_EN			BIT(1)

#define RTL930X_FORCE_EN			BIT(0)
#define RTL930X_FORCE_LINK_EN			BIT(1)
#define RTL930X_MAC_FORCE_FC_EN			BIT(9)
#define RTL930X_MEDIA_SEL			BIT(16)

enum rtldsa_mac_link_state_source {
	RTLDSA_MAC_LINK_STATE_SOURCE_PCS,
	RTLDSA_MAC_LINK_STATE_SOURCE_PHY,
};

#endif /* _OTTO_MAC_H */
