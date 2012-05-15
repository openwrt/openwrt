/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/etherdevice.h>
#include <linux/mdio-gpio.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include <lantiq_soc.h>
#include <lantiq_platform.h>

#include "../machtypes.h"
#include "devices.h"
#include "../dev-gpio-leds.h"
#include "dev-dwc_otg.h"


static struct mtd_partition p2601hnfx_partitions[] __initdata =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
/*	{
		.name	= "uboot_env",
		.offset	= 0x20000,
		.size	= 0x20000,
	},
*/	{
		.name	= "linux",
		.offset	= 0x020000,
		.size	= 0xfc0000,
	},
	{
		.name	= "board_config",
		.offset	= 0xfe0000,
		.size	= 0x20000,
	},
};

static struct physmap_flash_data p2601hnfx_flash_data __initdata = {
	.nr_parts	= ARRAY_SIZE(p2601hnfx_partitions),
	.parts		= p2601hnfx_partitions,
};

static struct gpio_led p2601hnfx_leds_gpio[] __initdata = {
	{ .name = "soc:red:power", .gpio = 29, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:yellow:phone", .gpio = 64, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:green:phone", .gpio = 65, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:yellow:wlan", .gpio = 66, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:green:power", .gpio = 67, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:internet", .gpio = 68, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:green:internet", .gpio = 69, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:green:dsl", .gpio = 70, .active_low = 1, .default_trigger = "default-off" },
	{ .name = "soc:green:wlan", .gpio = 71, .active_low = 1, .default_trigger = "default-off" },
};

static struct gpio_button
p2601hnfx_gpio_buttons[] /* __initdata */ = {
	{ .desc = "reset", .type = EV_KEY, .code = BTN_0, .threshold = 3, .gpio = 53, .active_low = 1, },
	{ .desc = "wlan", .type = EV_KEY, .code = BTN_1, .threshold = 1, .gpio = 54, .active_low = 1, },
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_RMII,
};

static void __init
p2601hnfx_init(void)
{
#define P2601HNFX_USB			9

	ltq_register_gpio_stp();
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(p2601hnfx_leds_gpio), p2601hnfx_leds_gpio);
	ltq_register_gpio_buttons(p2601hnfx_gpio_buttons, ARRAY_SIZE(p2601hnfx_gpio_buttons));
	ltq_register_nor(&p2601hnfx_flash_data);
	ltq_register_etop(&ltq_eth_data);
	xway_register_dwc(P2601HNFX_USB);

	// enable the ethernet ports on the SoC
//	ltq_w32((ltq_r32(LTQ_GPORT_P0_CTL) & ~(1 << 17)) | (1 << 18), LTQ_GPORT_P0_CTL);
//	ltq_w32((ltq_r32(LTQ_GPORT_P1_CTL) & ~(1 << 17)) | (1 << 18), LTQ_GPORT_P1_CTL);
//	ltq_w32((ltq_r32(LTQ_GPORT_P2_CTL) & ~(1 << 17)) | (1 << 18), LTQ_GPORT_P2_CTL);
}

MIPS_MACHINE(LANTIQ_MACH_P2601HNFX,
			"P2601HNFX",
			"ZyXEL P-2601HN-Fx",
			p2601hnfx_init);
