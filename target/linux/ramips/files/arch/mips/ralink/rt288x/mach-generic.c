/*
 *  Generic RT288x machine setup
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
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

#include <asm/mips_machine.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/platform.h>

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition generic_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "config",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "defconfig",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size	= 0x100000,
	}, {
		.name	= "rootfs",
//		.offset = MTDPART_OFS_NXTBLK,
//		.size   = MTDPART_SIZ_FULL,
		.offset	= 0x150000,
		.size	= 0x2B0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data generic_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(generic_partitions),
	.parts		= generic_partitions,
#endif
};

static void __init rt288x_generic_init(void)
{
	rt288x_register_flash(0, &generic_flash_data);
}

MIPS_MACHINE(RT288X_MACH_GENERIC, "Generic RT288x board", rt288x_generic_init);
