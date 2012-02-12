/*
 *  Sitecom WL-351 v1 002 board support
 *
 *  Copyright (C) 2011 Tobias Diedrich <ranma+openwrt@tdiedrich.de>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

#include <linux/rtl8366.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define WL351_GPIO_LED_POWER_AMBER		8
#define WL351_GPIO_LED_UNPOPULATED_AMBER	12
#define WL351_GPIO_LED_UNPOPULATED_BLUE		13

#define WL351_GPIO_BUTTON_RESET	10
#define WL351_GPIO_BUTTON_WPS	0

#define WL351_KEYS_POLL_INTERVAL	20
#define WL351_KEYS_DEBOUNCE_INTERVAL	(3 * WL351_KEYS_POLL_INTERVAL)

static struct gpio_led wl351_leds_gpio[] __initdata = {
	{
		.name		= "wl-351:amber:power",
		.gpio		= WL351_GPIO_LED_POWER_AMBER,
		.active_low	= 1,
	}, {
		.name		= "wl-351:amber:unpopulated",
		.gpio		= WL351_GPIO_LED_UNPOPULATED_AMBER,
		.active_low	= 1,
	}, {
		.name		= "wl-351:blue:unpopulated",
		.gpio		= WL351_GPIO_LED_UNPOPULATED_BLUE,
		.active_low	= 1,
	}
};


static struct gpio_keys_button wl351_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WL351_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WL351_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WL351_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WL351_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static struct mtd_partition wl351_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "u-boot-env",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "factory",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size	= 0x110000,
	}, {
		.name	= "rootfs",
		.offset	= 0x160000,
		.size	= 0x230000,
	}, {
		.name	= "user (jffs2)",
		.offset	= 0x3F0000,
		.size	= 0x010000,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x3a0000,
	}
};

static struct rtl8366_platform_data wl351_switch_data = {
	.gpio_sda	= RT305X_GPIO_I2C_SD,
	.gpio_sck	= RT305X_GPIO_I2C_SCLK,
};

static struct platform_device wl351_switch = {
	.name		= RTL8366RB_DRIVER_NAME,
	.id		= -1,
	.dev		= {
		.platform_data = &wl351_switch_data,
	}
};

static void __init wl351_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
				RT305X_GPIO_MODE_UART0_SHIFT) |
				RT305X_GPIO_MODE_I2C |
				RT305X_GPIO_MODE_SPI |
				RT305X_GPIO_MODE_MDIO);

	rt305x_flash0_data.nr_parts = ARRAY_SIZE(wl351_partitions);
	rt305x_flash0_data.parts = wl351_partitions;
	rt305x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(wl351_leds_gpio),
						wl351_leds_gpio);
	ramips_register_gpio_buttons(-1, WL351_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wl351_gpio_buttons),
				     wl351_gpio_buttons);
	// external rtl8366rb
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_BYPASS;
	rt305x_esw_data.reg_initval_fct2 = 0x0002500c;
	rt305x_esw_data.reg_initval_fpa2 = 0x1f003fff;
	rt305x_register_ethernet();
	platform_device_register(&wl351_switch);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_WL351, "WL-351", "Sitecom WL-351 v1 002",
	     wl351_init);
