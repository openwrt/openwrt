/*
 *  La Fonera20N board support
 *
 *  Copyright (C) 2009 John Crispin <blogic@openwrt.org>
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
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define FONERA20N_GPIO_BUTTON_RESET	12
#define FONERA20N_GPIO_SWITCH		13
#define FONERA20N_GPIO_LED_WIFI		7
#define FONERA20N_GPIO_LED_POWER	9
#define FONERA20N_GPIO_LED_USB		14

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition fonera20n_partitions[] = {
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
		.size	= 0x6b0000,
	}, {
		.name	= "openwrt",
		.offset	= 0x050000,
		.size	= 0x7b0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data fonera20n_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(fonera20n_partitions),
	.parts		= fonera20n_partitions,
#endif
};

static struct gpio_led fonera20n_leds_gpio[] __initdata = {
	{
		.name		= "fonera20n:orange:wifi",
		.gpio		= FONERA20N_GPIO_LED_WIFI,
		.active_low	= 1,
	}, {
		.name		= "fonera20n:green:power",
		.gpio		= FONERA20N_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "fonera20n:orange:usb",
		.gpio		= FONERA20N_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static void __init fonera20n_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0, &fonera20n_flash_data);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(fonera20n_leds_gpio),
				  fonera20n_leds_gpio);

	rt305x_register_ethernet();
}

MIPS_MACHINE(RAMIPS_MACH_FONERA20N, "La Fonera 2.0N", fonera20n_init);
