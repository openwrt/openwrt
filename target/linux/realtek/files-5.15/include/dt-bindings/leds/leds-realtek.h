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

/* RTL93XX 16bit LED SET configuration bitmap */
#define RTL93XX_LED_SET_NONE            (0x0000)
#define RTL93XX_LED_SET_10G_LINK        (1 << 0)
#define RTL93XX_LED_SET_5G_LINK         (1 << 1)
#define RTL93XX_LED_SET_2P_2G5_LINK     (1 << 2)
#define RTL93XX_LED_SET_2G5_LINK        (1 << 3)
#define RTL93XX_LED_SET_2P_1G_LINK      (1 << 4)
#define RTL93XX_LED_SET_1000M_LINK      (1 << 5)
#define RTL93XX_LED_SET_500M_LINK       (1 << 6)
#define RTL93XX_LED_SET_100M_LINK       (1 << 7)
#define RTL93XX_LED_SET_10M_LINK        (1 << 8)
#define RTL93XX_LED_SET_LINK_UP_SOLID   (1 << 9)
#define RTL93XX_LED_SET_LINK_UP_BLINK   (1 << 10)
#define RTL93XX_LED_SET_LINK_ACTIVITY   (1 << 11)
#define RTL93XX_LED_SET_RX_ACTIVITY     (1 << 12)
#define RTL93XX_LED_SET_TX_ACTIVITY     (1 << 13)
#define RTL93XX_LED_SET_COLLISION       (1 << 14)
#define RTL93XX_LED_SET_FULL_DUPLEX     (1 << 15)

#endif /* _DT_BINDINGS_LEDS_REALTEK_H */
