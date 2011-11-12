/*
 *  Atheros AP81 board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2009 Imre Kaloz <kaloz@openwrt.org>
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
#include "dev-ar9xxx-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define AP81_GPIO_LED_STATUS	1
#define AP81_GPIO_LED_AOSS	3
#define AP81_GPIO_LED_WLAN	6
#define AP81_GPIO_LED_POWER	14

#define AP81_GPIO_BTN_SW4	12
#define AP81_GPIO_BTN_SW1	21

#define AP81_KEYS_POLL_INTERVAL		20 /* msecs */
#define AP81_KEYS_DEBOUNCE_INTERVAL	(3 * AP81_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition ap81_partitions[] = {
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
		.size		= 0x500000,
	}, {
		.name		= "uImage",
		.offset		= 0x550000,
		.size		= 0x100000,
	}, {
		.name		= "ART",
		.offset		= 0x650000,
		.size		= 0x1b0000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data ap81_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= ap81_partitions,
	.nr_parts	= ARRAY_SIZE(ap81_partitions),
#endif
};

static struct gpio_led ap81_leds_gpio[] __initdata = {
	{
		.name		= "ap81:green:status",
		.gpio		= AP81_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "ap81:amber:aoss",
		.gpio		= AP81_GPIO_LED_AOSS,
		.active_low	= 1,
	}, {
		.name		= "ap81:green:wlan",
		.gpio		= AP81_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "ap81:green:power",
		.gpio		= AP81_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ap81_gpio_keys[] __initdata = {
	{
		.desc		= "sw1",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = AP81_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP81_GPIO_BTN_SW1,
		.active_low	= 1,
	}, {
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = AP81_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AP81_GPIO_BTN_SW4,
		.active_low	= 1,
	}
};

static void __init ap81_setup(void)
{
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, eeprom, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.has_ar8216 = 1;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, eeprom, 1);
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&ap81_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ap81_leds_gpio),
					ap81_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, AP81_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ap81_gpio_keys),
					 ap81_gpio_keys);

	ar9xxx_add_device_wmac(eeprom, NULL);
}

MIPS_MACHINE(AR71XX_MACH_AP81, "AP81", "Atheros AP81", ap81_setup);
