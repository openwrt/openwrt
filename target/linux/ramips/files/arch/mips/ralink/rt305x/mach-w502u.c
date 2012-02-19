/*
 *  ALFA Networks W502U board support
 *
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
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

#define W502U_GPIO_LED_USB		13
#define W502U_GPIO_LED_WPS		14

#define W502U_GPIO_BUTTON_WPS		0
#define W502U_GPIO_BUTTON_RESET		10

#define W502U_KEYS_POLL_INTERVAL	20
#define W502U_KEYS_DEBOUNCE_INTERVAL	(3 * W502U_KEYS_POLL_INTERVAL)

static struct gpio_led w502u_leds_gpio[] __initdata = {
	{
		.name		= "alfa:blue:usb",
		.gpio		= W502U_GPIO_LED_USB,
		.active_low	= 1,
	},
	{
		.name		= "alfa:blue:wps",
		.gpio		= W502U_GPIO_LED_WPS,
		.active_low	= 1,
	},
};

static struct gpio_keys_button w502u_gpio_buttons[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = W502U_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= W502U_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = W502U_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= W502U_GPIO_BUTTON_WPS,
		.active_low	= 1,
	},
};

static void __init w502u_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
			  RT305X_GPIO_MODE_UART0_SHIFT));

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(w502u_leds_gpio),
				  w502u_leds_gpio);
	ramips_register_gpio_buttons(-1, W502U_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(w502u_gpio_buttons),
				     w502u_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_W502U, "W502U", "ALFA Networks W502U",
	      w502u_init);
