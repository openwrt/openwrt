/*
 *  Asus WL_330N3G board support
 *
 *  Copyright (C) 2012 Frederic Leroy <fredo@starox.org>
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

#define WL_330N3G_GPIO_BUTTON_RESET	10
#define WL_330N3G_GPIO_BUTTON_WPS	0
#define WL_330N3G_GPIO_LED_3G_BLUE	9
#define WL_330N3G_GPIO_LED_3G_RED	13
#define WL_330N3G_GPIO_LED_POWER	11
#define WL_330N3G_KEYS_POLL_INTERVAL	20
#define WL_330N3G_KEYS_DEBOUNCE_INTERVAL (3 * WL_330N3G_KEYS_POLL_INTERVAL)

const struct flash_platform_data wl_330n3g_flash = {
	.type		= "mx25l3205d",
};

struct spi_board_info wl_330n3g_spi_slave_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &wl_330n3g_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

static struct gpio_keys_button wl_330n3g_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WL_330N3G_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WL_330N3G_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WL_330N3G_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WL_330N3G_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static struct gpio_led wl_330n3g_leds_gpio[] __initdata = {
	{
		.name		= "asus:blue:3g",
		.gpio		= WL_330N3G_GPIO_LED_3G_BLUE,
		.active_low	= 1,
	}, {
		.name		= "asus:red:3g",
		.gpio		= WL_330N3G_GPIO_LED_3G_RED,
		.active_low	= 1,
	}, {
		.name		= "asus:blue:power",
		.gpio		= WL_330N3G_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static void __init wl_330n3g_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_spi(wl_330n3g_spi_slave_info,
			    ARRAY_SIZE(wl_330n3g_spi_slave_info));
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(wl_330n3g_leds_gpio),
				  wl_330n3g_leds_gpio);
	ramips_register_gpio_buttons(-1, WL_330N3G_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wl_330n3g_gpio_buttons),
				     wl_330n3g_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_usb();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_WL_330N3G, "WL_330N3G", "Asus WL-330N3G",
	     wl_330n3g_init);
