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

#endif /* _OTTO_MAC_H */
