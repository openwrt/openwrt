/*
 *  D-Link DIR-615 rev C1 board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
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
#include "nvram.h"

#define DIR_615C1_GPIO_LED_ORANGE_STATUS 1	/* ORANGE:STATUS:TRICOLOR */
#define DIR_615C1_GPIO_LED_BLUE_WPS	3	/* BLUE:WPS */
#define DIR_615C1_GPIO_LED_GREEN_WAN	4       /* GREEN:WAN:TRICOLOR */
#define DIR_615C1_GPIO_LED_GREEN_WANCPU	5       /* GREEN:WAN:CPU:TRICOLOR */
#define DIR_615C1_GPIO_LED_GREEN_WLAN	6	/* GREEN:WLAN */
#define DIR_615C1_GPIO_LED_GREEN_STATUS	14	/* GREEN:STATUS:TRICOLOR */
#define DIR_615C1_GPIO_LED_ORANGE_WAN	15	/* ORANGE:WAN:TRICOLOR */

/* buttons may need refinement */

#define DIR_615C1_GPIO_BTN_WPS		12
#define DIR_615C1_GPIO_BTN_RESET	21

#define DIR_615C1_KEYS_POLL_INTERVAL	20	/* msecs */
#define DIR_615C1_KEYS_DEBOUNCE_INTERVAL (3 * DIR_615C1_KEYS_POLL_INTERVAL)

#define DIR_615C1_CONFIG_ADDR		0x1f020000
#define DIR_615C1_CONFIG_SIZE		0x10000

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition dir_615c1_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "config",
		.offset		= 0x020000,
		.size		= 0x010000,
	}, {
		.name		= "kernel",
		.offset		= 0x030000,
		.size		= 0x0e0000,
	}, {
		.name		= "rootfs",
		.offset		= 0x110000,
		.size		= 0x2e0000,
	}, {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x030000,
		.size		= 0x3c0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data dir_615c1_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= dir_615c1_partitions,
	.nr_parts	= ARRAY_SIZE(dir_615c1_partitions),
#endif
};

static struct gpio_led dir_615c1_leds_gpio[] __initdata = {
	{
		.name		= "dir-615c1:orange:status",
		.gpio		= DIR_615C1_GPIO_LED_ORANGE_STATUS,
		.active_low	= 1,
	}, {
		.name		= "dir-615c1:blue:wps",
		.gpio		= DIR_615C1_GPIO_LED_BLUE_WPS,
		.active_low	= 1,
	}, {
		.name		= "dir-615c1:green:wan",
		.gpio		= DIR_615C1_GPIO_LED_GREEN_WAN,
		.active_low	= 1,
	}, {
		.name		= "dir-615c1:green:wancpu",
		.gpio		= DIR_615C1_GPIO_LED_GREEN_WANCPU,
		.active_low	= 1,
	}, {
		.name		= "dir-615c1:green:wlan",
		.gpio		= DIR_615C1_GPIO_LED_GREEN_WLAN,
		.active_low	= 1,
	}, {
		.name		= "dir-615c1:green:status",
		.gpio		= DIR_615C1_GPIO_LED_GREEN_STATUS,
		.active_low     = 1,
	}, {
		.name		= "dir-615c1:orange:wan",
		.gpio		= DIR_615C1_GPIO_LED_ORANGE_WAN,
		.active_low	= 1,
	}

};

static struct gpio_keys_button dir_615c1_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DIR_615C1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_615C1_GPIO_BTN_RESET,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DIR_615C1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_615C1_GPIO_BTN_WPS,
	}
};

#define DIR_615C1_LAN_PHYMASK	BIT(0)
#define DIR_615C1_WAN_PHYMASK	BIT(4)
#define DIR_615C1_MDIO_MASK	(~(DIR_615C1_LAN_PHYMASK | \
				   DIR_615C1_WAN_PHYMASK))

static void __init dir_615c1_setup(void)
{
	const char *config = (char *) KSEG1ADDR(DIR_615C1_CONFIG_ADDR);
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 mac[6];
	u8 *wlan_mac = NULL;

	if (nvram_parse_mac_addr(config, DIR_615C1_CONFIG_SIZE,
					"lan_mac=", mac) == 0) {
		ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
		ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac, 1);
		wlan_mac = mac;
	}

	ar71xx_add_device_mdio(DIR_615C1_MDIO_MASK);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = DIR_615C1_LAN_PHYMASK;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = DIR_615C1_WAN_PHYMASK;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&dir_615c1_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(dir_615c1_leds_gpio),
					dir_615c1_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, DIR_615C1_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(dir_615c1_gpio_keys),
					 dir_615c1_gpio_keys);

	ar9xxx_add_device_wmac(eeprom, wlan_mac);
}

MIPS_MACHINE(AR71XX_MACH_DIR_615_C1, "DIR-615-C1", "D-Link DIR-615 rev. C1",
	     dir_615c1_setup);
