/*
 *  Ralink AP-RT3052-V22RW-2X2 board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
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
#include <linux/leds.h>

#include <asm/mips_machine.h>
#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev_gpio_leds.h>

#include "devices.h"

#define V22RW_2X2_GPIO_LED_SECURITY	13
#define V22RW_2X2_GPIO_LED_WPS		14

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition v22rw_2x2_partitions[] = {
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
		.size	= 0x0a0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x150000,
		.size	= 0x2b0000,
	}, {
		.name	= "openwrt",
		.offset	= 0x050000,
		.size	= 0x3b0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data v22rw_2x2_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(v22rw_2x2_partitions),
	.parts		= v22rw_2x2_partitions,
#endif
};

static struct gpio_led v22rw_2x2_leds_gpio[] __initdata = {
	{
		.name		= "v22rw-2x2:green:security",
		.gpio		= V22RW_2X2_GPIO_LED_SECURITY,
		.active_low	= 1,
	}, {
		.name		= "v22rw-2x2:red:wps",
		.gpio		= V22RW_2X2_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static void __init v22rw_2x2_init(void)
{
	rt305x_register_flash(0, &v22rw_2x2_flash_data);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(v22rw_2x2_leds_gpio),
				  v22rw_2x2_leds_gpio);
}

MIPS_MACHINE(RAMIPS_MACH_V22RW_2X2, "Ralink AP-RT3052-V22RW-2X2",
	     v22rw_2x2_init);
