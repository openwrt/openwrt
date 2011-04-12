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
#include <linux/platform_device.h>
#include <linux/rtl8366rb.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ar9xxx-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define TL_WR1043ND_GPIO_LED_USB        1
#define TL_WR1043ND_GPIO_LED_SYSTEM     2
#define TL_WR1043ND_GPIO_LED_QSS        5
#define TL_WR1043ND_GPIO_LED_WLAN       9

#define TL_WR1043ND_GPIO_BTN_RESET      3
#define TL_WR1043ND_GPIO_BTN_QSS        7

#define TL_WR1043ND_GPIO_RTL8366_SDA	18
#define TL_WR1043ND_GPIO_RTL8366_SCK	19

#define TL_WR1043ND_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WR1043ND_KEYS_DEBOUNCE_INTERVAL (3 * TL_WR1043ND_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tl_wr1043nd_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x020000,
		.size		= 0x140000,
	}, {
		.name		= "rootfs",
		.offset		= 0x160000,
		.size		= 0x690000,
	}, {
		.name		= "art",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
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

static struct gpio_keys_button tl_wr1043nd_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_WR1043ND_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR1043ND_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "qss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TL_WR1043ND_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR1043ND_GPIO_BTN_QSS,
		.active_low	= 1,
	}
};

static struct rtl8366rb_platform_data tl_wr1043nd_rtl8366rb_data = {
	.gpio_sda        = TL_WR1043ND_GPIO_RTL8366_SDA,
	.gpio_sck        = TL_WR1043ND_GPIO_RTL8366_SCK,
};

static struct platform_device tl_wr1043nd_rtl8366rb_device = {
	.name		= RTL8366RB_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &tl_wr1043nd_rtl8366rb_data,
	}
};

static void __init tl_wr1043nd_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_eth0_data.mii_bus_dev = &tl_wr1043nd_rtl8366rb_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_pll_data.pll_1000 = 0x1a000000;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&tl_wr1043nd_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_wr1043nd_leds_gpio),
					tl_wr1043nd_leds_gpio);

	platform_device_register(&tl_wr1043nd_rtl8366rb_device);

	ar71xx_register_gpio_keys_polled(-1, TL_WR1043ND_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tl_wr1043nd_gpio_keys),
					 tl_wr1043nd_gpio_keys);

	ar9xxx_add_device_wmac(eeprom, mac);
}

MIPS_MACHINE(AR71XX_MACH_TL_WR1043ND, "TL-WR1043ND", "TP-LINK TL-WR1043ND",
	     tl_wr1043nd_setup);
