/*
 *  Buffalo WHR-G300N board support
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
#include <asm/mach-ralink/dev_gpio_leds.h>

#include "machine.h"
#include "devices.h"

#define WHR_G300N_GPIO_LED_DIAG		7
#define WHR_G300N_GPIO_LED_ROUTER	9
#define WHR_G300N_GPIO_LED_SECURITY	14

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition whr_g300n_partitions[] = {
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
		.size	= 0x090000,
	}, {
		.name	= "rootfs",
		.offset	= 0x140000,
		.size	= 0x2B0000,
	}, {
		.name	= "user",
		.offset	= 0x3f0000,
		.size	= 0x010000,
	}, {
		.name	= "openwrt",
		.offset	= 0x050000,
		.size	= 0x3a0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data whr_g300n_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(whr_g300n_partitions),
	.parts		= whr_g300n_partitions,
#endif
};

static struct gpio_led whr_g300n_leds_gpio[] __initdata = {
	{
		.name		= "whr-g300n:red:diag",
		.gpio		= WHR_G300N_GPIO_LED_DIAG,
		.active_low	= 1,
	}, {
		.name		= "whr-g300n:green:router",
		.gpio		= WHR_G300N_GPIO_LED_ROUTER,
		.active_low	= 1,
	}, {
		.name		= "whr-g300n:amber:security",
		.gpio		= WHR_G300N_GPIO_LED_SECURITY,
		.active_low	= 1,
	}
};

static void __init whr_g300n_init(void)
{
	rt305x_register_flash(0, &whr_g300n_flash_data);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(whr_g300n_leds_gpio),
				  whr_g300n_leds_gpio);
}

MIPS_MACHINE(RT305X_MACH_WHR_G300N, "Buffalo WHR-G300N", whr_g300n_init);
