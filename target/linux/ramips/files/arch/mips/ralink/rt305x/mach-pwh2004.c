/*
 *  Prolink PWH2004 support (or Abocom WR5205)
 *
 *  Copyright (C) 2010 Esa Hyytia <esa@netlab.tkk.fi>
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

#define PWH2004_GPIO_BUTTON_WPS		12
#define PWH2004_GPIO_LED_POWER		9
#define PWH2004_GPIO_LED_WIFI		14
#define PWH2004_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition pwh2004_partitions[] = {
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
		.name	= "factory",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size	= 0x7b0000,
	}, {
		.name	= "openwrt",
		.offset	= 0x050000,
		.size	= 0x7b0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data pwh2004_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(pwh2004_partitions),
	.parts		= pwh2004_partitions,
#endif
};

static struct gpio_led pwh2004_leds_gpio[] __initdata = {
	{
		.name		= "pwh2004:red:wifi",
		.gpio		= PWH2004_GPIO_LED_WIFI,
		.active_low	= 1,
	}, {
		.name		= "pwh2004:green:power",
		.gpio		= PWH2004_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static struct gpio_button pwh2004_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= PWH2004_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init pwh2004_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_flash(0, &pwh2004_flash_data);
	ramips_register_gpio_leds(-1, ARRAY_SIZE(pwh2004_leds_gpio),
				  pwh2004_leds_gpio);
	ramips_register_gpio_buttons(-1, PWH2004_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(pwh2004_gpio_buttons),
				     pwh2004_gpio_buttons);
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_PWH2004, "PWH2004", "Prolink PWH2004",
	     pwh2004_init);
