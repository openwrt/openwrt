/*
 *  Netgear WNDR3700 board support
 *
 *  Copyright (C) 2009 Marco Porsch
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
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

#define WNDR3700_GPIO_BTN_WPS		3
#define WNDR3700_GPIO_BTN_RESET		8
#define WNDR3700_GPIO_BTN_WIFI		11

#define WNDR3700_GPIO_RTL8366_SDA	5
#define WNDR3700_GPIO_RTL8366_SCK	7

#define WNDR3700_BUTTONS_POLL_INTERVAL    20

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
	} , {
		.name		= "env",
		.offset		= 0x050000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "rootfs",
		.offset		= 0x070000,
		.size		= 0x720000,
	} , {
		.name		= "config",
		.offset		= 0x790000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config_bak",
		.offset		= 0x7a0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "pot",
		.offset		= 0x7b0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "traffic_meter",
		.offset		= 0x7c0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "language",
		.offset		= 0x7d0000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "caldata",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wndr3700_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wndr3700_partitions,
        .nr_parts       = ARRAY_SIZE(wndr3700_partitions),
#endif
};

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
	}
};

static struct gpio_button wndr3700_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_RESET,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_WPS,
	} , {
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_2,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_WIFI,
	}
};

static struct rtl8366_smi_platform_data wndr3700_rtl8366_smi_data = {
	.gpio_sda        = WNDR3700_GPIO_RTL8366_SDA,
	.gpio_sck        = WNDR3700_GPIO_RTL8366_SCK,
};

static struct platform_device wndr3700_rtl8366_smi_device = {
	.name		= "rtl8366-smi",
	.id		= -1,
	.dev = {
		.platform_data	= &wndr3700_rtl8366_smi_data,
	}
};

static void __init wndr3700_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_set_mac_base(art);

	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;
	ar71xx_eth0_data.mii_bus_dev = &wndr3700_rtl8366_smi_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0xf;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;
	ar71xx_eth1_data.mii_bus_dev = &wndr3700_rtl8366_smi_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&wndr3700_flash_data);

        ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wndr3700_leds_gpio),
				    wndr3700_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WNDR3700_BUTTONS_POLL_INTERVAL,
				      ARRAY_SIZE(wndr3700_gpio_buttons),
				      wndr3700_gpio_buttons);

	platform_device_register(&wndr3700_rtl8366_smi_device);
	platform_device_register_simple("wndr3700-led-usb", -1, NULL, 0);

	ap94_pci_init(art + WNDR3700_CALDATA0_OFFSET,
		      art + WNDR3700_WMAC0_MAC_OFFSET,
		      art + WNDR3700_CALDATA1_OFFSET,
		      art + WNDR3700_WMAC1_MAC_OFFSET);
}

MIPS_MACHINE(AR71XX_MACH_WNDR3700, "NETGEAR WNDR3700", wndr3700_setup);
