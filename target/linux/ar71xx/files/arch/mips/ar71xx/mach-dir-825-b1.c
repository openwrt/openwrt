/*
 *  D-Link DIR-825 rev. B1 board support
 *
 *  Copyright (C) 2009-2011 Lukas Kuna, Evkanet, s.r.o.
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
#include <linux/rtl8366.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap94-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define DIR825B1_GPIO_LED_BLUE_USB		0
#define DIR825B1_GPIO_LED_ORANGE_POWER		1
#define DIR825B1_GPIO_LED_BLUE_POWER		2
#define DIR825B1_GPIO_LED_BLUE_WPS		4
#define DIR825B1_GPIO_LED_ORANGE_PLANET		6
#define DIR825B1_GPIO_LED_BLUE_PLANET		11

#define DIR825B1_GPIO_BTN_RESET			3
#define DIR825B1_GPIO_BTN_WPS			8

#define DIR825B1_GPIO_RTL8366_SDA		5
#define DIR825B1_GPIO_RTL8366_SCK		7

#define DIR825B1_KEYS_POLL_INTERVAL		20	/* msecs */
#define DIR825B1_KEYS_DEBOUNCE_INTERVAL		(3 * DIR825B1_KEYS_POLL_INTERVAL)

#define DIR825B1_CAL_LOCATION_0			0x1f661000
#define DIR825B1_CAL_LOCATION_1			0x1f665000

#define DIR825B1_MAC_LOCATION_0			0x1f66ffa0
#define DIR825B1_MAC_LOCATION_1			0x1f66ffb4

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition dir825b1_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "config",
		.offset		= 0x040000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x050000,
		.size		= 0x610000,
	}, {
		.name		= "caldata",
		.offset		= 0x660000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "unknown",
		.offset		= 0x670000,
		.size		= 0x190000,
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
		.name		= "dir825b1:blue:wps",
		.gpio		= DIR825B1_GPIO_LED_BLUE_WPS,
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

static struct gpio_keys_button dir825b1_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DIR825B1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR825B1_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DIR825B1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR825B1_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static struct rtl8366_initval dir825b1_rtl8366s_initvals[] = {
	{ .reg = 0x06, .val = 0x0108 },
};

static struct rtl8366_platform_data dir825b1_rtl8366s_data = {
	.gpio_sda	= DIR825B1_GPIO_RTL8366_SDA,
	.gpio_sck	= DIR825B1_GPIO_RTL8366_SCK,
	.num_initvals	= ARRAY_SIZE(dir825b1_rtl8366s_initvals),
	.initvals	= dir825b1_rtl8366s_initvals,
};

static struct platform_device dir825b1_rtl8366s_device = {
	.name		= RTL8366S_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &dir825b1_rtl8366s_data,
	}
};

static void dir825b1_read_ascii_mac(u8 *dest, unsigned int src_addr)
{
	int ret;
	u8 *src = (u8 *)KSEG1ADDR(src_addr);

	ret = sscanf(src, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		     &dest[0], &dest[1], &dest[2],
		     &dest[3], &dest[4], &dest[5]);

	if (ret != ETH_ALEN) memset(dest, 0, ETH_ALEN);
}

static void __init dir825b1_setup(void)
{
	u8 mac1[ETH_ALEN], mac2[ETH_ALEN];

	dir825b1_read_ascii_mac(mac1, DIR825B1_MAC_LOCATION_0);
	dir825b1_read_ascii_mac(mac2, DIR825B1_MAC_LOCATION_1);

	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac1, 2);
	ar71xx_eth0_data.mii_bus_dev = &dir825b1_rtl8366s_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac1, 3);
	ar71xx_eth1_data.mii_bus_dev = &dir825b1_rtl8366s_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;
	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&dir825b1_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(dir825b1_leds_gpio),
					dir825b1_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, DIR825B1_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(dir825b1_gpio_keys),
					 dir825b1_gpio_keys);

	ar71xx_add_device_usb();

	platform_device_register(&dir825b1_rtl8366s_device);

	ap94_pci_setup_wmac_led_pin(0, 5);
	ap94_pci_setup_wmac_led_pin(1, 5);

	ap94_pci_init((u8 *) KSEG1ADDR(DIR825B1_CAL_LOCATION_0), mac1,
		      (u8 *) KSEG1ADDR(DIR825B1_CAL_LOCATION_1), mac2);
}

MIPS_MACHINE(AR71XX_MACH_DIR_825_B1, "DIR-825-B1", "D-Link DIR-825 rev. B1",
	     dir825b1_setup);
