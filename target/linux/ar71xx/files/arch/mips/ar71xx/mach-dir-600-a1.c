/*
 *  D-Link DIR-600 rev. A1 board support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
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
#include "dev-ap91-eth.h"
#include "dev-ap91-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "nvram.h"

#define DIR_600_A1_GPIO_LED_WPS			0
#define DIR_600_A1_GPIO_LED_POWER_AMBER		1
#define DIR_600_A1_GPIO_LED_POWER_GREEN		6

#define DIR_600_A1_GPIO_BTN_RESET		8
#define DIR_600_A1_GPIO_BTN_WPS			12

#define DIR_600_A1_BUTTONS_POLL_INTERVAL	20

#define DIR_600_A1_NVRAM_ADDR	0x1f030000
#define DIR_600_A1_NVRAM_SIZE	0x10000

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition dir_600_a1_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x030000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "nvram",
		.offset		= 0x030000,
		.size		= 0x010000,
	}, {
		.name		= "kernel",
		.offset		= 0x040000,
		.size		= 0x0e0000,
	}, {
		.name		= "rootfs",
		.offset		= 0x120000,
		.size		= 0x2c0000,
	}, {
		.name		= "mac",
		.offset		= 0x3e0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x040000,
		.size		= 0x3a0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data dir_600_a1_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = dir_600_a1_partitions,
        .nr_parts       = ARRAY_SIZE(dir_600_a1_partitions),
#endif
};

static struct gpio_led dir_600_a1_leds_gpio[] __initdata = {
	{
		.name		= "dir-600-a1:green:power",
		.gpio		= DIR_600_A1_GPIO_LED_POWER_GREEN,
	}, {
		.name		= "dir-600-a1:amber:power",
		.gpio		= DIR_600_A1_GPIO_LED_POWER_AMBER,
	}, {
		.name		= "dir-600-a1:blue:wps",
		.gpio		= DIR_600_A1_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_button dir_600_a1_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= DIR_600_A1_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 3,
		.gpio		= DIR_600_A1_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static void __init dir_600_a1_setup(void)
{
	const char *nvram = (char *) KSEG1ADDR(DIR_600_A1_NVRAM_ADDR);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 mac_buff[6];
	u8 *mac = NULL;

	if (nvram_parse_mac_addr(nvram, DIR_600_A1_NVRAM_SIZE,
			         "lan_mac=", mac_buff) == 0)
		mac = mac_buff;

	ar71xx_add_device_m25p80(&dir_600_a1_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(dir_600_a1_leds_gpio),
					dir_600_a1_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, DIR_600_A1_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(dir_600_a1_gpio_buttons),
					dir_600_a1_gpio_buttons);

	ap91_eth_init(mac);
	ap91_pci_init(ee, mac);
}

MIPS_MACHINE(AR71XX_MACH_DIR_600_A1, "DIR-600-A1", "D-Link DIR-600 rev. A1",
	     dir_600_a1_setup);
