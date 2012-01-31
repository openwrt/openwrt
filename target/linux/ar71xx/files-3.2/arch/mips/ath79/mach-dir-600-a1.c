/*
 *  D-Link DIR-600 rev. A1 board support
 *
 *  Copyright (C) 2010-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "machtypes.h"
#include "nvram.h"

#define DIR_600_A1_GPIO_LED_WPS			0
#define DIR_600_A1_GPIO_LED_POWER_AMBER		1
#define DIR_600_A1_GPIO_LED_POWER_GREEN		6
#define DIR_600_A1_GPIO_LED_LAN1		13
#define DIR_600_A1_GPIO_LED_LAN2		14
#define DIR_600_A1_GPIO_LED_LAN3		15
#define DIR_600_A1_GPIO_LED_LAN4		16
#define DIR_600_A1_GPIO_LED_WAN_AMBER		7
#define DIR_600_A1_GPIO_LED_WAN_GREEN		17

#define DIR_600_A1_GPIO_BTN_RESET		8
#define DIR_600_A1_GPIO_BTN_WPS			12

#define DIR_600_A1_KEYS_POLL_INTERVAL		20	/* msecs */
#define DIR_600_A1_KEYS_DEBOUNCE_INTERVAL (3 * DIR_600_A1_KEYS_POLL_INTERVAL)

#define DIR_600_A1_NVRAM_ADDR	0x1f030000
#define DIR_600_A1_NVRAM_SIZE	0x10000

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

static struct flash_platform_data dir_600_a1_flash_data = {
	.parts		= dir_600_a1_partitions,
	.nr_parts	= ARRAY_SIZE(dir_600_a1_partitions),
};

static struct gpio_led dir_600_a1_leds_gpio[] __initdata = {
	{
		.name		= "d-link:green:power",
		.gpio		= DIR_600_A1_GPIO_LED_POWER_GREEN,
	}, {
		.name		= "d-link:amber:power",
		.gpio		= DIR_600_A1_GPIO_LED_POWER_AMBER,
	}, {
		.name		= "d-link:amber:wan",
		.gpio		= DIR_600_A1_GPIO_LED_WAN_AMBER,
	}, {
		.name		= "d-link:green:wan",
		.gpio		= DIR_600_A1_GPIO_LED_WAN_GREEN,
		.active_low	= 1,
	}, {
		.name		= "d-link:green:lan1",
		.gpio		= DIR_600_A1_GPIO_LED_LAN1,
		.active_low	= 1,
	}, {
		.name		= "d-link:green:lan2",
		.gpio		= DIR_600_A1_GPIO_LED_LAN2,
		.active_low	= 1,
	}, {
		.name		= "d-link:green:lan3",
		.gpio		= DIR_600_A1_GPIO_LED_LAN3,
		.active_low	= 1,
	}, {
		.name		= "d-link:green:lan4",
		.gpio		= DIR_600_A1_GPIO_LED_LAN4,
		.active_low	= 1,
	}, {
		.name		= "d-link:blue:wps",
		.gpio		= DIR_600_A1_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button dir_600_a1_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DIR_600_A1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_600_A1_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DIR_600_A1_KEYS_DEBOUNCE_INTERVAL,
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

	if (ath79_nvram_parse_mac_addr(nvram, DIR_600_A1_NVRAM_SIZE,
				       "lan_mac=", mac_buff) == 0) {
		ath79_init_mac(ath79_eth0_data.mac_addr, mac_buff, 0);
		ath79_init_mac(ath79_eth1_data.mac_addr, mac_buff, 1);
		mac = mac_buff;
	}

	ath79_register_m25p80(&dir_600_a1_flash_data);

	ath79_gpio_function_disable(AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(dir_600_a1_leds_gpio),
				 dir_600_a1_leds_gpio);

	ath79_register_gpio_keys_polled(-1, DIR_600_A1_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(dir_600_a1_gpio_keys),
					dir_600_a1_gpio_keys);

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 1);

	ath79_register_mdio(0, 0x0);

	/* LAN ports */
	ath79_register_eth(1);

	/* WAN port */
	ath79_register_eth(0);

	ap91_pci_init(ee, mac);
}

MIPS_MACHINE(ATH79_MACH_DIR_600_A1, "DIR-600-A1", "D-Link DIR-600 rev. A1",
	     dir_600_a1_setup);
