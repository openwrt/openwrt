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

/* Per-port EEE speed enables in MAC_FORCE_MODE_CTRL (bit 19 is reserved):
 * 100M@18, 1000M@20, 2.5G@21, 5G@22, 10G@23. Note the RTL931x (Mango) layout
 * differs from the RTL930x sibling (which packs its EEE enables at [15:10]).
 */
#define RTL931X_MAC_FORCE_EEE_MASK		(BIT(18) | GENMASK(23, 20))

#endif /* _OTTO_MAC_H */
