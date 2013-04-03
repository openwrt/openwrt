/*
 *  ASUS RT-N10+ board support
 *
 *  Copyright (C) 2009-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define RT_N10_PLUS_GPIO_LED_WPS	14

#define RT_N10_PLUS_GPIO_BUTTON_WPS	0	/* active low */
#define RT_N10_PLUS_GPIO_BUTTON_RESET	10	/* active low */

#define RT_N10_PLUS_KEYS_POLL_INTERVAL	20
#define RT_N10_PLUS_KEYS_DEBOUNCE_INTERVAL  (3 * RT_N10_PLUS_KEYS_POLL_INTERVAL)

static struct gpio_led rt_n10_plus_leds_gpio[] __initdata = {
	{
		.name		= "asus:green:wps",
		.gpio		= RT_N10_PLUS_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button rt_n10_plus_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = RT_N10_PLUS_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RT_N10_PLUS_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = RT_N10_PLUS_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RT_N10_PLUS_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init rt_n10_plus_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(rt_n10_plus_leds_gpio),
				  rt_n10_plus_leds_gpio);
	ramips_register_gpio_buttons(-1, RT_N10_PLUS_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(rt_n10_plus_gpio_buttons),
				     rt_n10_plus_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_RT_N10_PLUS, "RT-N10-PLUS", "Asus RT-N10+",
	rt_n10_plus_init);
