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
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

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

#define DIR_300B_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition dir_300b_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "devdata",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "devconf",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size	= 0x090000,
	}, {
		.name	= "rootfs",
		.offset	= 0x140000,
		.size	= 0x2B0000,
	}, {
		.name	= "openwrt",
		.offset	= 0x050000,
		.size	= 0x3a0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data dir_300b_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(dir_300b_partitions),
	.parts		= dir_300b_partitions,
#endif
};

static struct gpio_led dir_300b_leds_gpio[] __initdata = {
	{
		.name		= "dir-300b:amber:status",
		.gpio		= DIR_300B_GPIO_LED_STATUS_AMBER,
		.active_low	= 1,
	}, {
		.name		= "dir-300b:green:status",
		.gpio		= DIR_300B_GPIO_LED_STATUS_GREEN,
		.active_low	= 1,
	}, {
		.name		= "dir-300b:blue:wps",
		.gpio		= DIR_300B_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_button dir_300b_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= DIR_300B_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= DIR_300B_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init dir_300b_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0, &dir_300b_flash_data);
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(dir_300b_leds_gpio),
				  dir_300b_leds_gpio);
	ramips_register_gpio_buttons(-1, DIR_300B_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(dir_300b_gpio_buttons),
				     dir_300b_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_DIR_300_REVB, "DIR-300-revB", "D-Link DIR-300 revB",
	     dir_300b_init);
