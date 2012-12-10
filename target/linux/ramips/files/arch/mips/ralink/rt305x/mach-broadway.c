/*
 *  Hauppauge/PCTV Broadway Support
 *
 *  Copyright (C) 2012 Devin Heitmueller <dheitmueller@kernellabs.com>
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

#define BROADWAY_GPIO_LED_DISKMOUNT	9
#define BROADWAY_GPIO_LED_WPS		14

#define BROADWAY_GPIO_BUTTON_WPS		0
#define BROADWAY_GPIO_BUTTON_FACTORYRESET	13

#define BROADWAY_KEYS_POLL_INTERVAL	20
#define BROADWAY_KEYS_DEBOUNCE_INTERVAL	(3 * BROADWAY_KEYS_POLL_INTERVAL)

static struct gpio_led broadway_leds_gpio[] __initdata = {
	{
		.name		= "red:diskmounted",
		.gpio		= BROADWAY_GPIO_LED_DISKMOUNT,
		.active_low	= 1,
	},
	{
		.name		= "red:wps_active",
		.gpio		= BROADWAY_GPIO_LED_WPS,
		.active_low	= 1,
	},
};

static struct gpio_keys_button broadway_gpio_buttons[] __initdata = {
	{
		.desc		= "Factory Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = BROADWAY_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= BROADWAY_GPIO_BUTTON_FACTORYRESET,
		.active_low	= 1,
	},
#ifdef DJH_WPS_BUTTON_NOT_WIRED_TO_GPIO
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = BROADWAY_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= BROADWAY_GPIO_BUTTON_WPS,
		.active_low	= 1,
	},
#endif
};

static void __init broadway_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
			  RT305X_GPIO_MODE_UART0_SHIFT));

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();

	ramips_register_gpio_leds(-1, ARRAY_SIZE(broadway_leds_gpio),
				  broadway_leds_gpio);
	ramips_register_gpio_buttons(-1, BROADWAY_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(broadway_gpio_buttons),
				     broadway_gpio_buttons);

	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_BROADWAY, "BROADWAY", "Hauppauge Broadway",
	      broadway_init);
