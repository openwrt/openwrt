/*
 *  Sparklan WCR-150GN board support
 *
 *  Copyright (C) 2010 Roman Yeryomin <roman@advem.lv>
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

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define WCR150GN_GPIO_LED_USER                 12
#define WCR150GN_GPIO_LED_POWER                        8
#define WCR150GN_GPIO_BUTTON_WPS               10
#define WCR150GN_GPIO_BUTTON_RESET             0
#define WCR150GN_KEYS_POLL_INTERVAL	20
#define WCR150GN_KEYS_DEBOUNCE_INTERVAL	(3 * WCR150GN_KEYS_POLL_INTERVAL)

static struct gpio_led wcr150gn_leds_gpio[] __initdata = {
	{
		.name       = "wcr150gn:amber:user",
		.gpio       = WCR150GN_GPIO_LED_USER,
		.active_low = 1,
	},
	{
		.name       = "wcr150gn:amber:power",
		.gpio       = WCR150GN_GPIO_LED_POWER,
		.active_low = 1,
	}
};

static struct gpio_keys_button wcr150gn_gpio_buttons[] __initdata = {
	{
		.desc       = "wps",
		.type       = EV_KEY,
		.code       = KEY_WPS_BUTTON,
		.debounce_interval = WCR150GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio       = WCR150GN_GPIO_BUTTON_WPS,
		.active_low = 1,
	},
	{
		.desc       = "reset",
		.type       = EV_KEY,
		.code       = KEY_RESTART,
		.debounce_interval = WCR150GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio       = WCR150GN_GPIO_BUTTON_RESET,
		.active_low = 1,
	}
};

static struct mtd_partition wcr150gn_partitions[] = {
	{
		.name	= "bootloader",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "config",
		.offset	= 0x030000,
		.size	= 0x040000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "factory",
		.offset	= 0x040000,
		.size	= 0x050000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size   = 0x0d0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x120000,
		.size   = 0x2e0000,
	}, {
		.name   = "firmware",
		.offset = 0x050000,
		.size   = 0x3b0000,
	}
};

static struct physmap_flash_data wcr150gn_flash_data = {
	.nr_parts	= ARRAY_SIZE(wcr150gn_partitions),
	.parts		= wcr150gn_partitions,
};

static void __init wcr150gn_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_flash(0, &wcr150gn_flash_data);
	ramips_register_gpio_leds(-1, ARRAY_SIZE(wcr150gn_leds_gpio),
				  wcr150gn_leds_gpio);
	ramips_register_gpio_buttons(-1, WCR150GN_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wcr150gn_gpio_buttons),
				     wcr150gn_gpio_buttons);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_WCR150GN, "WCR150GN", "Sparklan WCR-150GN",
	     wcr150gn_init);
