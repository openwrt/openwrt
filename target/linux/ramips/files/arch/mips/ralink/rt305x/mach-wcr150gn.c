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

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wcr150gn_partitions[] = {
	{
		.name	= "Bootloader",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "Config",
		.offset	= 0x030000,
		.size	= 0x040000,
	}, {
		.name	= "Factory",
		.offset	= 0x040000,
		.size	= 0x050000,
	}, {
		.name	= "Kernel",
		.offset	= 0x050000,
		.size	= 0x120000,
	}, {
		.name	= "rootfs",
		.offset	= 0x120000,
		.size	= 0x400000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data wcr150gn_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(wcr150gn_partitions),
	.parts		= wcr150gn_partitions,
#endif
};

static void __init wcr150gn_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_flash(0, &wcr150gn_flash_data);
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_WCR150GN, "WCR150GN", "Sparklan WCR-150GN",
	     wcr150gn_init);
