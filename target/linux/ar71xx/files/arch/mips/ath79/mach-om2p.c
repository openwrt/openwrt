/*
 *  OpenMesh OM2P support
 *
 *  Copyright (C) 2011 Marek Lindner <marek@open-mesh.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "machtypes.h"

#define OM2P_GPIO_LED_POWER	0
#define OM2P_GPIO_LED_GREEN	13
#define OM2P_GPIO_LED_RED	14
#define OM2P_GPIO_LED_YELLOW	15
#define OM2P_GPIO_LED_LAN	16
#define OM2P_GPIO_LED_WAN	17
#define OM2P_GPIO_BTN_RESET	11

#define OM2P_KEYS_POLL_INTERVAL		20	/* msecs */
#define OM2P_KEYS_DEBOUNCE_INTERVAL	(3 * OM2P_KEYS_POLL_INTERVAL)

#define OM2P_WAN_PHYMASK	BIT(4)

static struct flash_platform_data om2p_flash_data = {
	.type = "s25sl12800",
	.name = "ar7240-nor0",
};

static struct gpio_led om2p_leds_gpio[] __initdata = {
	{
		.name		= "om2p:blue:power",
		.gpio		= OM2P_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "om2p:red:wifi",
		.gpio		= OM2P_GPIO_LED_RED,
		.active_low	= 1,
	}, {
		.name		= "om2p:yellow:wifi",
		.gpio		= OM2P_GPIO_LED_YELLOW,
		.active_low	= 1,
	}, {
		.name		= "om2p:green:wifi",
		.gpio		= OM2P_GPIO_LED_GREEN,
		.active_low	= 1,
	}, {
		.name		= "om2p:blue:lan",
		.gpio		= OM2P_GPIO_LED_LAN,
		.active_low	= 1,
	}, {
		.name		= "om2p:blue:wan",
		.gpio		= OM2P_GPIO_LED_WAN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button om2p_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = OM2P_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= OM2P_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init om2p_setup(void)
{
	u8 *mac1 = (u8 *)KSEG1ADDR(0x1ffc0000);
	u8 *mac2 = (u8 *)KSEG1ADDR(0x1ffc0000 + ETH_ALEN);
	u8 *ee = (u8 *)KSEG1ADDR(0x1ffc1000);

	ath79_gpio_function_disable(AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				    AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ath79_register_m25p80(&om2p_flash_data);

	ath79_register_mdio(0, ~OM2P_WAN_PHYMASK);

	ath79_init_mac(ath79_eth0_data.mac_addr, mac1, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac2, 0);

	ath79_register_eth(0);
	ath79_register_eth(1);

	ap91_pci_init(ee, NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(om2p_leds_gpio),
				 om2p_leds_gpio);

	ath79_register_gpio_keys_polled(-1, OM2P_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(om2p_gpio_keys),
					om2p_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_OM2P, "OM2P", "OpenMesh OM2P", om2p_setup);
