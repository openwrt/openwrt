/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_LEDS_H
#define _OTTO_LEDS_H

#include <linux/kconfig.h>

struct device_node;
struct rtl838x_switch_priv;

#if IS_ENABLED(CONFIG_NET_DSA_RTL83XX_LEDS)
void rtldsa_930x_sw_leds_init(struct rtl838x_switch_priv *priv,
			      struct device_node *node);
#else
static inline void rtldsa_930x_sw_leds_init(struct rtl838x_switch_priv *priv,
					    struct device_node *node)
{
}
#endif

#endif /* _OTTO_LEDS_H */
