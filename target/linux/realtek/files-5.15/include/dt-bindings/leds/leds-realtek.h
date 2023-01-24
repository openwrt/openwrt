/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * This header provides constants for the Realtek RTL83xx/RTL93xx led controller
 */

#ifndef _DT_BINDINGS_LEDS_REALTEK_H
#define _DT_BINDINGS_LEDS_REALTEK_H __FILE__

/* RTL93XX number of leds configuration bitmap */
#define RTL93XX_LED_NONE                (0x0)
#define RTL93XX_LED_NUM0                (1 << 0)
#define RTL93XX_LED_NUM1                (1 << 1)
#define RTL93XX_LED_NUM2                (1 << 2)
#define RTL93XX_LED_NUM3                (1 << 3)

#endif /* _DT_BINDINGS_LEDS_REALTEK_H */
