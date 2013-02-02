/*
 *  Edimax BR-6425 board support
 *
 *  Copyright (C) 2012 OpenWrt.org
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define BR6425_GPIO_LED_POWER		9
#define BR6425_GPIO_BUTTON_RESET	12
#define BR6425_GPIO_SWITCH_RFKILL	13
#define BR6425_GPIO_LED_WLAN		14
#define BR6425_KEYS_POLL_INTERVAL	20
#define BR6425_KEYS_DEBOUNCE_INTERVAL	(3 * BR6425_KEYS_POLL_INTERVAL)

static struct gpio_keys_button br6425_gpio_buttons[] __initdata = {
	{
		.desc		= "reset_wps",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = BR6425_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= BR6425_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc           = "RFKILL switch",
		.type           = EV_SW,
		.code           = KEY_RFKILL,
		.debounce_interval = BR6425_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = BR6425_GPIO_SWITCH_RFKILL,
		.active_low     = 1,
	},
};

static struct gpio_led br6425_leds_gpio[] __initdata = {
	{
		.name		= "edimax:green:power",
		.gpio		= BR6425_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "edimax:orange:wlan",
		.gpio		= BR6425_GPIO_LED_WLAN,
		.active_low	= 1,
	},
};

static void __init br6425_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_flash(0);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(br6425_leds_gpio),
				  br6425_leds_gpio);
	ramips_register_gpio_buttons(-1, BR6425_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(br6425_gpio_buttons),
				     br6425_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_BR6425, "BR-6425", "Edimax BR-6425",
	     br6425_init);
