/*
 *  Atheros DB120 board (WASP SoC) support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"
#include "dev-ar934x-wmac.h"

#define DB120_GPIO_LED_USB	11
#define DB120_GPIO_LED_WLAN_5G	12
#define DB120_GPIO_LED_WLAN_2G	13
#define DB120_GPIO_LED_STATUS	14
#define DB120_GPIO_LED_WPS	15

#define DB120_GPIO_BTN_SW1	16

#define DB120_CALDATA_OFFSET	0x1000
#define DB120_WMAC_MAC_OFFSET	0x1003

#define DB120_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition db120_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x040000,
		.size		= 0x010000,
	}, {
		.name		= "rootfs",
		.offset		= 0x050000,
		.size		= 0x630000,
	}, {
		.name		= "uImage",
		.offset		= 0x680000,
		.size		= 0x160000,
	}, {
		.name		= "NVRAM",
		.offset		= 0x7E0000,
		.size		= 0x010000,
	}, {
		.name		= "ART",
		.offset		= 0x7F0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data db120_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= db120_partitions,
	.nr_parts	= ARRAY_SIZE(db120_partitions),
#endif
};

static struct gpio_led db120_leds_gpio[] __initdata = {
	{
		.name		= "db120:green:status",
		.gpio		= DB120_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "db120:green:wps",
		.gpio		= DB120_GPIO_LED_WPS,
		.active_low	= 1,
	}, {
		.name		= "db120:green:wlan-5g",
		.gpio		= DB120_GPIO_LED_WLAN_5G,
		.active_low	= 1,
	}, {
		.name		= "db120:green:wlan-2g",
		.gpio		= DB120_GPIO_LED_WLAN_2G,
		.active_low	= 1,
	}, {
		.name		= "db120:green:usb",
		.gpio		= DB120_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static struct gpio_button db120_gpio_buttons[] __initdata = {
	{
		.desc		= "sw1",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= DB120_GPIO_BTN_SW1,
		.active_low	= 1,
	}
};

static void __init db120_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&db120_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(db120_leds_gpio),
					db120_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, DB120_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(db120_gpio_buttons),
					db120_gpio_buttons);

	ar934x_add_device_wmac(art + DB120_CALDATA_OFFSET,
				art + DB120_WMAC_MAC_OFFSET);
}

MIPS_MACHINE(AR71XX_MACH_DB120, "DB120", "Atheros DB120", db120_setup);
