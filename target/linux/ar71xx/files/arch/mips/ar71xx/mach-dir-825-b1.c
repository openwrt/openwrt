/*
 *  D-Link DIR-825 rev. B1 board support
 *
 *  Copyright (C) 2009 Lukas Kuna, Evkanet, s.r.o.
 *
 *  based on mach-wndr3700.c
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/rtl8366_smi.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap94-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define DIR825B1_GPIO_LED_BLUE_USB		0
#define DIR825B1_GPIO_LED_ORANGE_POWER		1
#define DIR825B1_GPIO_LED_BLUE_POWER		2
#define DIR825B1_GPIO_LED_BLUE_POWERSAVE	4
#define DIR825B1_GPIO_LED_ORANGE_PLANET		6
#define DIR825B1_GPIO_LED_BLUE_PLANET		11

#define DIR825B1_GPIO_BTN_RESET			3
#define DIR825B1_GPIO_BTN_POWERSAVE		8

#define DIR825B1_GPIO_RTL8366_SDA		5
#define DIR825B1_GPIO_RTL8366_SCK		7

#define DIR825B1_BUTTONS_POLL_INTERVAL		20

#define DIR825B1_CAL_LOCATION_0			0x1f661000
#define DIR825B1_CAL_LOCATION_1			0x1f665000

#define DIR825B1_MAC_LOCATION_0			0x2ffa81b8
#define DIR825B1_MAC_LOCATION_1			0x2ffa8370

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition dir825b1_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config",
		.offset		= 0x040000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x050000,
		.size		= 0x610000,
	} , {
		.name		= "caldata",
		.offset		= 0x660000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data dir825b1_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = dir825b1_partitions,
        .nr_parts       = ARRAY_SIZE(dir825b1_partitions),
#endif
};

static struct gpio_led dir825b1_leds_gpio[] __initdata = {
	{
		.name		= "dir825b1:blue:usb",
		.gpio		= DIR825B1_GPIO_LED_BLUE_USB,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:orange:power",
		.gpio		= DIR825B1_GPIO_LED_ORANGE_POWER,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:blue:power",
		.gpio		= DIR825B1_GPIO_LED_BLUE_POWER,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:blue:powersave",
		.gpio		= DIR825B1_GPIO_LED_BLUE_POWERSAVE,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:orange:planet",
		.gpio		= DIR825B1_GPIO_LED_ORANGE_PLANET,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:blue:planet",
		.gpio		= DIR825B1_GPIO_LED_BLUE_PLANET,
		.active_low	= 1,
	}
};

static struct gpio_button dir825b1_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= DIR825B1_GPIO_BTN_RESET,
		.active_low	= 1,
	} , {
		.desc		= "powersave",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= DIR825B1_GPIO_BTN_POWERSAVE,
		.active_low	= 1,
	}
};

static struct rtl8366_smi_platform_data dir825b1_rtl8366_smi_data = {
	.gpio_sda        = DIR825B1_GPIO_RTL8366_SDA,
	.gpio_sck        = DIR825B1_GPIO_RTL8366_SCK,
};

static struct platform_device dir825b1_rtl8366_smi_device = {
	.name		= "rtl8366-smi",
	.id		= -1,
	.dev = {
		.platform_data	= &dir825b1_rtl8366_smi_data,
	}
};

static void __init dir825b1_setup(void)
{
        u8 mac[6], i;

	memcpy(mac, (u8*)KSEG1ADDR(DIR825B1_MAC_LOCATION_1), 6);
	for(i = 5; i >= 3; i--)
		if(++mac[i] != 0x00) break;

	ar71xx_set_mac_base(mac);

	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.mii_bus_dev = &dir825b1_rtl8366_smi_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;

	ar71xx_eth1_data.mii_bus_dev = &dir825b1_rtl8366_smi_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;
	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&dir825b1_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(dir825b1_leds_gpio),
					dir825b1_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, DIR825B1_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(dir825b1_gpio_buttons),
					dir825b1_gpio_buttons);

	ar71xx_add_device_usb();

	platform_device_register(&dir825b1_rtl8366_smi_device);

	ap94_pci_init((u8 *) KSEG1ADDR(DIR825B1_CAL_LOCATION_0),
		      (u8 *) KSEG1ADDR(DIR825B1_MAC_LOCATION_0),
		      (u8 *) KSEG1ADDR(DIR825B1_CAL_LOCATION_1),
		      (u8 *) KSEG1ADDR(DIR825B1_MAC_LOCATION_1));
}

MIPS_MACHINE(AR71XX_MACH_DIR_825_B1, "D-Link DIR-825 rev. B1", dir825b1_setup);
