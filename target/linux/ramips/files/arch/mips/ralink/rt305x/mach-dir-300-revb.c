/*
 *  D-Link DIR-300 rev B board support
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

#define DIR_300B_GPIO_LED_STATUS_AMBER	8
#define DIR_300B_GPIO_LED_STATUS_GREEN	9
#define DIR_300B_GPIO_LED_WPS		13

#define DIR_300B_GPIO_BUTTON_WPS	0	/* active low */
#define DIR_300B_GPIO_BUTTON_RESET	10	/* active low */

#define DIR_300B_KEYS_POLL_INTERVAL	20
#define DIR_300B_KEYS_DEBOUNCE_INTERVAL	(3 * DIR_300B_KEYS_POLL_INTERVAL)

static struct gpio_led dir_300b_leds_gpio[] __initdata = {
	{
		.name		= "d-link:amber:status",
		.gpio		= DIR_300B_GPIO_LED_STATUS_AMBER,
		.active_low	= 1,
	}, {
		.name		= "d-link:green:status",
		.gpio		= DIR_300B_GPIO_LED_STATUS_GREEN,
		.active_low	= 1,
	}, {
		.name		= "d-link:blue:wps",
		.gpio		= DIR_300B_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button dir_300b_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DIR_300B_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_300B_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DIR_300B_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_300B_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init dir_300b_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(dir_300b_leds_gpio),
				  dir_300b_leds_gpio);
	ramips_register_gpio_buttons(-1, DIR_300B_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(dir_300b_gpio_buttons),
				     dir_300b_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_DIR_300_B1, "DIR-300-B1", "D-Link DIR-300 B1",
	     dir_300b_init);

MIPS_MACHINE(RAMIPS_MACH_DIR_600_B1, "DIR-600-B1", "D-Link DIR-600 B1",
	     dir_300b_init);

MIPS_MACHINE(RAMIPS_MACH_DIR_600_B2, "DIR-600-B2", "D-Link DIR-600 B2",
	     dir_300b_init);
