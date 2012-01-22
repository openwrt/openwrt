/*
 *  Buffalo WZR-HP-AG300H board support
 *
 *  Copyright (C) 2011 Felix Fietkau <nbd@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ath79/ath79.h>

#include "dev-eth.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "machtypes.h"

#define WZRHPAG300H_MAC_OFFSET		0x20c
#define WZRHPAG300H_KEYS_POLL_INTERVAL	20      /* msecs */
#define WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL (3 * WZRHPAG300H_KEYS_POLL_INTERVAL)

static struct mtd_partition wzrhpag300h_flash_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x0040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x0040000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "art",
		.offset		= 0x0050000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x0060000,
		.size		= 0x0100000,
	}, {
		.name		= "rootfs",
		.offset		= 0x0160000,
		.size		= 0x1e90000,
	}, {
		.name		= "user_property",
		.offset		= 0x1ff0000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x0060000,
		.size		= 0x1f90000,
	}
};

static struct flash_platform_data wzrhpag300h_flash_data = {
	.parts      = wzrhpag300h_flash_partitions,
	.nr_parts   = ARRAY_SIZE(wzrhpag300h_flash_partitions),
};

static struct gpio_led wzrhpag300h_leds_gpio[] __initdata = {
	{
		.name		= "buffalo:red:diag",
		.gpio		= 1,
		.active_low	= 1,
	},
};

static struct gpio_keys_button wzrhpag300h_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 11,
		.active_low	= 1,
	}, {
		.desc		= "usb",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 3,
		.active_low	= 1,
	}, {
		.desc		= "aoss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 5,
		.active_low	= 1,
	}, {
		.desc		= "router_auto",
		.type		= EV_KEY,
		.code		= BTN_6,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 6,
		.active_low	= 1,
	}, {
		.desc		= "router_off",
		.type		= EV_KEY,
		.code		= BTN_5,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 7,
		.active_low	= 1,
	}
};

static void __init wzrhpag300h_setup(void)
{
	u8 *eeprom1 = (u8 *) KSEG1ADDR(0x1f051000);
	u8 *eeprom2 = (u8 *) KSEG1ADDR(0x1f055000);
	u8 *mac1 = eeprom1 + WZRHPAG300H_MAC_OFFSET;
	u8 *mac2 = eeprom2 + WZRHPAG300H_MAC_OFFSET;

	ath79_init_mac(ath79_eth0_data.mac_addr, mac1, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac2, 1);

	ath79_register_mdio(0, ~(BIT(0) | BIT(4)));

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(0);

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth1_data.phy_mask = BIT(4);

	ath79_register_eth(0);
	ath79_register_eth(1);

	ath79_register_usb();
	gpio_request(2, "usb");
	gpio_direction_output(2, 1);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wzrhpag300h_leds_gpio),
					wzrhpag300h_leds_gpio);

	ath79_register_gpio_keys_polled(-1, WZRHPAG300H_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(wzrhpag300h_gpio_keys),
					 wzrhpag300h_gpio_keys);

	ath79_register_m25p80_multi(&wzrhpag300h_flash_data);

	ap94_pci_init(eeprom1, mac1, eeprom2, mac2);
}

MIPS_MACHINE(ATH79_MACH_WZR_HP_AG300H, "WZR-HP-AG300H",
	     "Buffalo WZR-HP-AG300H", wzrhpag300h_setup);

