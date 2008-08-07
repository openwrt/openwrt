/*
 *  Edimax BR-61xx support
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include "br-61xx.h"

#define BR61XX_GPIO_DEV_MASK	0

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition br61xx_partitions[] = {
	{
		.name	= "admboot",
		.offset	= 0,
		.size	= 32*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "config",
		.offset	= MTDPART_OFS_APPEND,
		.size	= 32*1024,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct gpio_button br61xx_gpio_buttons[] __initdata = {
	{
		.desc		= "reset_button",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= ADM5120_GPIO_PIN2,
	}
};

static u8 br61xx_vlans[6] __initdata = {
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

void __init br61xx_generic_setup(void)
{

#ifdef CONFIG_MTD_PARTITIONS
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(br61xx_partitions);
	adm5120_flash0_data.parts = br61xx_partitions;
#endif /* CONFIG_MTD_PARTITIONS */
	adm5120_add_device_flash(0);

	adm5120_add_device_gpio(BR61XX_GPIO_DEV_MASK);
	adm5120_add_device_switch(5, br61xx_vlans);
	adm5120_add_device_gpio_buttons(ARRAY_SIZE(br61xx_gpio_buttons),
					br61xx_gpio_buttons);

	/* TODO: setup mac addresses */
}
