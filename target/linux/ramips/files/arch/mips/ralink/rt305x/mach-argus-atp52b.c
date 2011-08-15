/*
 *  Argus ATP-52B router support
 *  http://www.argus-co.com/english/productsview.php?id=70&cid=81
 *
 *  Copyright (C) 2011 Roman Yeryomin <roman@advem.lv>
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

#define ARGUS_ATP52B_GPIO_LED_RUN		9
#define ARGUS_ATP52B_GPIO_LED_NET		13
#define ARGUS_ATP52B_GPIO_BUTTON_WPS		0
#define ARGUS_ATP52B_GPIO_BUTTON_RESET		10
#define ARGUS_ATP52B_BUTTONS_POLL_INTERVAL	20

static struct gpio_led argus_atp52b_leds_gpio[] __initdata = {
	{
		.name       = "argus_atp52b:green:run",
		.gpio       = ARGUS_ATP52B_GPIO_LED_RUN,
		.active_low = 1,
	},
	{
		.name       = "argus_atp52b:amber:net",
		.gpio       = ARGUS_ATP52B_GPIO_LED_NET,
		.active_low = 1,
	}
};

static struct gpio_button argus_atp52b_gpio_buttons[] __initdata = {
	{
		.desc       = "wps",
		.type       = EV_KEY,
		.code       = KEY_WPS_BUTTON,
		.threshold  = 3,
		.gpio       = ARGUS_ATP52B_GPIO_BUTTON_WPS,
		.active_low = 1,
	},
	{
		.desc       = "reset",
		.type       = EV_KEY,
		.code       = KEY_RESTART,
		.threshold  = 10,
		.gpio       = ARGUS_ATP52B_GPIO_BUTTON_RESET,
		.active_low = 1,
	}
};

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition argus_atp52b_partitions[] = {
	{
		.name	= "bootloader",
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
		.size	= 0x120000,
	}, {
		.name	= "rootfs",
		.offset	= 0x170000,
		.size	= 0x680000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data argus_atp52b_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(argus_atp52b_partitions),
	.parts		= argus_atp52b_partitions,
#endif
};

static void __init argus_atp52b_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_flash(0, &argus_atp52b_flash_data);
	ramips_register_gpio_leds(-1, ARRAY_SIZE(argus_atp52b_leds_gpio),
					argus_atp52b_leds_gpio);
	ramips_register_gpio_buttons(-1, ARGUS_ATP52B_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(argus_atp52b_gpio_buttons),
					argus_atp52b_gpio_buttons);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_ARGUS_ATP52B, "ARGUS_ATP52B", "Argus ATP-52B",
					argus_atp52b_init);
