/*
 *  Asus RT-N15 board support
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
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

#include "devices.h"

#define RT_N15_GPIO_LED_POWER		11
#define RT_N15_GPIO_BUTTON_WPS		0
#define RT_N15_GPIO_BUTTON_RESET	12

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition rt_n15_partitions[] = {
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
		.size	= 0x0b0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x100000,
		.size	= 0x300000,
	}, {
		.name	= "openwrt",
		.offset	= 0x050000,
		.size	= 0x3b0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data rt_n15_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(rt_n15_partitions),
	.parts		= rt_n15_partitions,
#endif
};

static struct gpio_led rt_n15_leds_gpio[] __initdata = {
	{
		.name		= "rt-n15:blue:power",
		.gpio		= RT_N15_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static void __init rt_n15_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0);

	rt288x_register_flash(0, &rt_n15_flash_data);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(rt_n15_leds_gpio),
				  rt_n15_leds_gpio);
}

MIPS_MACHINE(RAMIPS_MACH_RT_N15, "Asus RT-N15", rt_n15_init);
