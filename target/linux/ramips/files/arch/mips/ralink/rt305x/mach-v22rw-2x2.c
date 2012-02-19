/*
 *  Ralink AP-RT3052-V22RW-2X2 board support
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
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

#define V22RW_2X2_GPIO_BUTTON_WPS	0
#define V22RW_2X2_GPIO_BUTTON_SWRST	10
#define V22RW_2X2_GPIO_LED_SECURITY	13
#define V22RW_2X2_GPIO_LED_WPS		14

#define V22RW_2X2_KEYS_POLL_INTERVAL	20
#define V22RW_2X2_KEYS_DEBOUNCE_INTERVAL (3 * V22RW_2X2_KEYS_POLL_INTERVAL)

static struct gpio_led v22rw_2x2_leds_gpio[] __initdata = {
	{
		.name		= "v22rw-2x2:green:security",
		.gpio		= V22RW_2X2_GPIO_LED_SECURITY,
		.active_low	= 1,
	}, {
		.name		= "v22rw-2x2:red:wps",
		.gpio		= V22RW_2X2_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button v22rw_2x2_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = V22RW_2X2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= V22RW_2X2_GPIO_BUTTON_SWRST,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = V22RW_2X2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= V22RW_2X2_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init v22rw_2x2_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(v22rw_2x2_leds_gpio),
				  v22rw_2x2_leds_gpio);
	ramips_register_gpio_buttons(-1, V22RW_2X2_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(v22rw_2x2_gpio_buttons),
				     v22rw_2x2_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_V22RW_2X2, "V22RW-2X2", "Ralink AP-RT3052-V22RW-2X2",
	     v22rw_2x2_init);
