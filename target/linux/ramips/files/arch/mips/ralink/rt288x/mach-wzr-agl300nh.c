/*
 *  Buffalo WZR-AGL300NH board support
 *
 *  Copyright (C) 2010 Joonas Lahtinen <joonas.lahtinen@gmail.com>
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
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

#include "devices.h"

/*
 * MTD layout from stock firmware:
 * mtd0: 00030000 00010000 "uboot"
 * mtd1: 00010000 00010000 "uboot_environ"
 * mtd2: 00010000 00010000 "factory_default"
 * mtd3: 000b0000 00010000 "linux"
 * mtd4: 002f0000 00010000 "rootfs"
 * mtd5: 00010000 00010000 "user_property"
 */

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wzr_agl300nh_partitions[] = {
	{
		.name	= "uboot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "uboot_environ",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "factory_default",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "linux",
		.offset	= 0x050000,
		.size	= 0x0b0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x100000,
		.size	= 0x2f0000,
	}, {
		.name	= "user_property",
		.offset	= 0x3f0000,
		.size	= 0x010000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data wzr_agl300nh_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(wzr_agl300nh_partitions),
	.parts		= wzr_agl300nh_partitions,
#endif
};

static void __init wzr_agl300nh_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0);

	rt288x_register_flash(0, &wzr_agl300nh_flash_data);
	rt288x_register_wifi();
	rt288x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_WZR_AGL300NH, "WZR-AGL300NH",
	     "Buffalo WZR-AGL300NH", wzr_agl300nh_init);
