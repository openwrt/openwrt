/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2023 Peter Körner
 *
 * RTL83XX leds
 */
#ifndef __DT_BINDINGS_LEDS_RTL83XX_H
#define __DT_BINDINGS_LEDS_RTL83XX_H

// https://svanheule.net/realtek/maple/register/led_glb_ctrl
#define RTL838x_NUM_LEDS_NO_LEDS (0x0)
#define RTL838x_NUM_LEDS_ONE_LED (0x1)
#define RTL838x_NUM_LEDS_TWO_LEDS (0x3)
#define RTL838x_NUM_LEDS_THREE_LEDS (0x7)

// https://svanheule.net/realtek/maple/register/led_mode_sel
#define RTL838x_LEDS_CONTROL_SERIAL (0x0)
#define RTL838x_LEDS_CONTROL_SINGLE_COLOR_SCAN (0x1)
#define RTL838x_LEDS_CONTROL_BI_COLOR_SCAN (0x2)
#define RTL838x_LEDS_CONTROL_LEDS_DISABLED (0x3)

// https://svanheule.net/realtek/maple/register/led_mode_ctrl
#define RTL838x_LEDS_MODE_LINK_ACT (0)
#define RTL838x_LEDS_MODE_LINK (1)
#define RTL838x_LEDS_MODE_ACT (2)
#define RTL838x_LEDS_MODE_ACT_RX (3)
#define RTL838x_LEDS_MODE_ACT_TX (4)
#define RTL838x_LEDS_MODE_LINK_1G (7)
#define RTL838x_LEDS_MODE_LINK_100M (8)
#define RTL838x_LEDS_MODE_LINK_10M (9)
#define RTL838x_LEDS_MODE_LINK_ACT_1G (10)
#define RTL838x_LEDS_MODE_LINK_ACT_100M (11)
#define RTL838x_LEDS_MODE_LINK_ACT_10M (12)
#define RTL838x_LEDS_MODE_LINK_ACT_1G_100M (13)
#define RTL838x_LEDS_MODE_LINK_ACT_1G_10M (14)
#define RTL838x_LEDS_MODE_LINK_ACT_100M_10M (15)
#define RTL838x_LEDS_MODE_DISABLED (31)


#endif /* __DT_BINDINGS_LEDS_RTL83XX_H */
