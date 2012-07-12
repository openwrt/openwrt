/*
 *  Edimax 3g-6200n board support
 *
 *  Copyright (C) 2011 Andrzej Hajda <andrzej.hajda@wp.pl>
 *  Copyright (C) 2012 Lukasz Golebiowski <lgolebio@gmail.com>
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

#define EDIMAX_GPIO_BUTTON_WPS 12
#define EDIMAX_GPIO_BTN_0 13 /* Wifi on/off switch button */

#define EDIMAX_GPIO_LED_POWER  9
#define EDIMAX_GPIO_LED_WLAN   14
#define EDIMAX_GPIO_LED_3G     7

#define EDIMAX_KEYS_POLL_INTERVAL   20
#define EDIMAX_KEYS_DEBOUNCE_INTERVAL (3 * EDIMAX_KEYS_POLL_INTERVAL)

static struct gpio_led edimax_leds_gpio[] __initdata = {
	{
		.name		= "edimax:green:power",
		.gpio		= EDIMAX_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "edimax:amber:wlan",
		.gpio		= EDIMAX_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "edimax:blue:3g",
		.gpio		= EDIMAX_GPIO_LED_3G,
		.active_low	= 1,
	}
};

static struct gpio_keys_button edimax_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = EDIMAX_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= EDIMAX_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}, {
		.desc		= "wlanswitch",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = EDIMAX_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= EDIMAX_GPIO_BTN_0,
		.active_low	= 1,
	}
};

static void __init edimax_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(edimax_leds_gpio),
				  edimax_leds_gpio);
	ramips_register_gpio_buttons(-1, EDIMAX_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(edimax_gpio_buttons),
				     edimax_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_3G_6200N, "3G-6200N", "Edimax 3g-6200n",
	      edimax_init);
