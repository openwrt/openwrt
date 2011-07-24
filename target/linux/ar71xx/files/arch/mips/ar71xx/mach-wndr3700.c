/*
 *  Netgear WNDR3700 board support
 *
 *  Copyright (C) 2009 Marco Porsch
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
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

#define WNDR3700_GPIO_LED_WPS_ORANGE	0
#define WNDR3700_GPIO_LED_POWER_ORANGE	1
#define WNDR3700_GPIO_LED_POWER_GREEN	2
#define WNDR3700_GPIO_LED_WPS_GREEN	4
#define WNDR3700_GPIO_LED_WAN_GREEN	6

#define WNDR3700_GPIO_BTN_WPS		3
#define WNDR3700_GPIO_BTN_RESET		8
#define WNDR3700_GPIO_BTN_WIFI		11

#define WNDR3700_GPIO_RTL8366_SDA	5
#define WNDR3700_GPIO_RTL8366_SCK	7

#define WNDR3700_KEYS_POLL_INTERVAL	20	/* msecs */
#define WNDR3700_KEYS_DEBOUNCE_INTERVAL (3 * WNDR3700_KEYS_POLL_INTERVAL)

#define WNDR3700_ETH0_MAC_OFFSET	0
#define WNDR3700_ETH1_MAC_OFFSET	0x6

#define WNDR3700_WMAC0_MAC_OFFSET	0
#define WNDR3700_WMAC1_MAC_OFFSET	0xc
#define WNDR3700_CALDATA0_OFFSET	0x1000
#define WNDR3700_CALDATA1_OFFSET	0x5000

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wndr3700_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x050000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "env",
		.offset		= 0x050000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "rootfs",
		.offset		= 0x070000,
		.size		= 0x720000,
	}, {
		.name		= "config",
		.offset		= 0x790000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "config_bak",
		.offset		= 0x7a0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "pot",
		.offset		= 0x7b0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "traffic_meter",
		.offset		= 0x7c0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "language",
		.offset		= 0x7d0000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "caldata",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};

static struct mtd_partition wndr3700v2_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x050000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "env",
		.offset		= 0x050000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "rootfs",
		.offset		= 0x070000,
		.size		= 0xe40000,
	}, {
		.name		= "config",
		.offset		= 0xeb0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "config_bak",
		.offset		= 0xec0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "pot",
		.offset		= 0xed0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "traffic_meter",
		.offset		= 0xee0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "language",
		.offset		= 0xef0000,
		.size		= 0x100000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "caldata",
		.offset		= 0xff0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#define wndr3700_num_partitions		ARRAY_SIZE(wndr3700_partitions)
#define wndr3700v2_num_partitions	ARRAY_SIZE(wndr3700v2_partitions)
#else
#define wndr3700_partitions		NULL
#define wndr3700_num_partitions		0
#define wndr3700v2_partitions		NULL
#define wndr3700v2_num_partitions	0
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wndr3700_flash_data;

static struct gpio_led wndr3700_leds_gpio[] __initdata = {
	{
		.name		= "wndr3700:green:power",
		.gpio		= WNDR3700_GPIO_LED_POWER_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:orange:power",
		.gpio		= WNDR3700_GPIO_LED_POWER_ORANGE,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:green:wps",
		.gpio		= WNDR3700_GPIO_LED_WPS_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:orange:wps",
		.gpio		= WNDR3700_GPIO_LED_WPS_ORANGE,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:green:wan",
		.gpio		= WNDR3700_GPIO_LED_WAN_GREEN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button wndr3700_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WNDR3700_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNDR3700_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WNDR3700_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNDR3700_GPIO_BTN_WPS,
		.active_low	= 1,
	}, {
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = WNDR3700_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNDR3700_GPIO_BTN_WIFI,
		.active_low	= 1,
	}
};

static struct rtl8366_platform_data wndr3700_rtl8366s_data = {
	.gpio_sda	= WNDR3700_GPIO_RTL8366_SDA,
	.gpio_sck	= WNDR3700_GPIO_RTL8366_SCK,
};

static struct platform_device wndr3700_rtl8366s_device = {
	.name		= RTL8366S_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &wndr3700_rtl8366s_data,
	}
};

static void __init wndr3700_common_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr,
			art + WNDR3700_ETH0_MAC_OFFSET, 0);
	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;
	ar71xx_eth0_data.mii_bus_dev = &wndr3700_rtl8366s_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr,
			art + WNDR3700_ETH1_MAC_OFFSET, 0);
	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;
	ar71xx_eth1_data.mii_bus_dev = &wndr3700_rtl8366s_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&wndr3700_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wndr3700_leds_gpio),
					wndr3700_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, WNDR3700_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(wndr3700_gpio_keys),
					 wndr3700_gpio_keys);

	platform_device_register(&wndr3700_rtl8366s_device);
	platform_device_register_simple("wndr3700-led-usb", -1, NULL, 0);

	ap94_pci_setup_wmac_led_pin(0, 5);
	ap94_pci_setup_wmac_led_pin(1, 5);

	/* 2.4 GHz uses the first fixed antenna group (1, 0, 1, 0) */
	ap94_pci_setup_wmac_gpio(0, (0xf << 6), (0xa << 6));

	/* 5 GHz uses the second fixed antenna group (0, 1, 1, 0) */
	ap94_pci_setup_wmac_gpio(1, (0xf << 6), (0x6 << 6));

	ap94_pci_init(art + WNDR3700_CALDATA0_OFFSET,
		      art + WNDR3700_WMAC0_MAC_OFFSET,
		      art + WNDR3700_CALDATA1_OFFSET,
		      art + WNDR3700_WMAC1_MAC_OFFSET);
}

static void __init wndr3700_setup(void)
{
	wndr3700_flash_data.parts = wndr3700_partitions,
	wndr3700_flash_data.nr_parts = wndr3700_num_partitions,
	wndr3700_common_setup();
}

MIPS_MACHINE(AR71XX_MACH_WNDR3700, "WNDR3700", "NETGEAR WNDR3700",
	     wndr3700_setup);

static void __init wndr3700v2_setup(void)
{
	wndr3700_flash_data.parts = wndr3700v2_partitions,
	wndr3700_flash_data.nr_parts = wndr3700v2_num_partitions,
	wndr3700_common_setup();
}

MIPS_MACHINE(AR71XX_MACH_WNDR3700V2, "WNDR3700v2", "NETGEAR WNDR3700v2",
	     wndr3700v2_setup);
