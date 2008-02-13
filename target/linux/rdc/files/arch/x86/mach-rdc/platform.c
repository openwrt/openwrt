/*
 *  $Id: platform.c 8331 2007-08-03 15:59:23Z florian $
 *
 *  Generic RDC321x platform devices
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/leds.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>

#include <asm/gpio.h>

/* Flash */
static struct resource rdc_flash_resource[] = {
	[0] = {
		.start = (u32)-CONFIG_MTD_RDC3210_SIZE,
		.end = (u32)-1,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device rdc_flash_device = {
	.name = "rdc321x-flash",
	.id = -1,
	.num_resources = ARRAY_SIZE(rdc_flash_resource),
	.resource = rdc_flash_resource,
};

/* LEDS */
static struct gpio_led default_leds[] = {
        { .name = "rdc321x:dmz", .gpio = 1, },
};

static struct gpio_led_platform_data rdc321x_led_data = {
	.num_leds = ARRAY_SIZE(default_leds),
	.leds = default_leds,
};

static struct platform_device rdc321x_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &rdc321x_led_data,
	}
};

/* Watchdog */
static struct platform_device rdc321x_wdt = {
	.name = "rdc321x-wdt",
	.id = -1,
	.num_resources = 0,
};

/* Button */
static struct gpio_keys_button rdc321x_gpio_btn[] = {
	{
		.gpio = 0,
		.code = BTN_0,
		.desc = "Reset",
		.active_low = 1,
	}
};

static struct gpio_keys_platform_data rdc321x_gpio_btn_data = {
	.buttons = rdc321x_gpio_btn,
	.nbuttons = ARRAY_SIZE(rdc321x_gpio_btn),
};

static struct platform_device rdc321x_button = {
	.name = "gpio-keys",
	.id = -1,
	.dev = {
		.platform_data = &rdc321x_gpio_btn_data,
	}
};

static struct platform_device *rdc321x_devs[] = {
	&rdc_flash_device,
	&rdc321x_leds,
	&rdc321x_wdt,
	&rdc321x_button
};

static int __init rdc_board_setup(void)
{
	return platform_add_devices(rdc321x_devs, ARRAY_SIZE(rdc321x_devs));
}

arch_initcall(rdc_board_setup);
