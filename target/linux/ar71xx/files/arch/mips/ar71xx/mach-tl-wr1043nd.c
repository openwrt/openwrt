/*
 *  TP-LINK TL-WR1043ND board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ar913x-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define TL_WR1043ND_GPIO_LED_USB        1
#define TL_WR1043ND_GPIO_LED_SYSTEM     2
#define TL_WR1043ND_GPIO_LED_QSS        5
#define TL_WR1043ND_GPIO_LED_WLAN       9

#define TL_WR1043ND_GPIO_BTN_RESET      3
#define TL_WR1043ND_GPIO_BTN_QSS        7

#define TL_WR1043ND_BUTTONS_POLL_INTERVAL     20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tl_wr1043nd_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "kernel",
		.offset		= 0x020000,
		.size		= 0x140000,
	} , {
		.name		= "rootfs",
		.offset		= 0x160000,
		.size		= 0x690000,
	} , {
		.name		= "art",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x020000,
		.size		= 0x7d0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tl_wr1043nd_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= tl_wr1043nd_partitions,
	.nr_parts	= ARRAY_SIZE(tl_wr1043nd_partitions),
#endif
};

static struct gpio_led tl_wr1043nd_leds_gpio[] __initdata = {
	{
		.name		= "tl-wr1043nd:green:usb",
		.gpio		= TL_WR1043ND_GPIO_LED_USB,
		.active_low	= 1,
	}, {
		.name		= "tl-wr1043nd:green:system",
		.gpio		= TL_WR1043ND_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tl-wr1043nd:green:qss",
		.gpio		= TL_WR1043ND_GPIO_LED_QSS,
		.active_low	= 0,
	}, {
		.name		= "tl-wr1043nd:green:wlan",
		.gpio		= TL_WR1043ND_GPIO_LED_WLAN,
		.active_low	= 1,
	}
};

static struct gpio_button tl_wr1043nd_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= TL_WR1043ND_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "qss",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= TL_WR1043ND_GPIO_BTN_QSS,
		.active_low	= 1,
	}
};

static void __init tl_wr1043nd_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);

	ar71xx_set_mac_base(mac);

	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0x0;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&tl_wr1043nd_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_wr1043nd_leds_gpio),
					tl_wr1043nd_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, TL_WR1043ND_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(tl_wr1043nd_gpio_buttons),
					tl_wr1043nd_gpio_buttons);
	ar913x_add_device_wmac();
}

MIPS_MACHINE(AR71XX_MACH_TL_WR1043ND, "TP-LINK TL-WR1043ND", tl_wr1043nd_setup);
