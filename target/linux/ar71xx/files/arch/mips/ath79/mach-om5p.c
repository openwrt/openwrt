/*
 *  OpenMesh OM5P support
 *
 *  Copyright (C) 2013 Marek Lindner <marek@open-mesh.com>
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
#include "dev-wmac.h"
#include "machtypes.h"

#define OM5P_GPIO_LED_POWER	13
#define OM5P_GPIO_LED_GREEN	16
#define OM5P_GPIO_LED_RED	19
#define OM5P_GPIO_LED_YELLOW	17
#define OM5P_GPIO_LED_LAN	14
#define OM5P_GPIO_LED_WAN	15
#define OM5P_GPIO_BTN_RESET	4

#define OM5P_KEYS_POLL_INTERVAL		20	/* msecs */
#define OM5P_KEYS_DEBOUNCE_INTERVAL	(3 * OM5P_KEYS_POLL_INTERVAL)

#define OM5P_WMAC_CALDATA_OFFSET	0x1000

static struct gpio_led om5p_leds_gpio[] __initdata = {
	{
		.name		= "om5p:blue:power",
		.gpio		= OM5P_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "om5p:red:wifi",
		.gpio		= OM5P_GPIO_LED_RED,
		.active_low	= 1,
	}, {
		.name		= "om5p:yellow:wifi",
		.gpio		= OM5P_GPIO_LED_YELLOW,
		.active_low	= 1,
	}, {
		.name		= "om5p:green:wifi",
		.gpio		= OM5P_GPIO_LED_GREEN,
		.active_low	= 1,
	}, {
		.name		= "om5p:blue:lan",
		.gpio		= OM5P_GPIO_LED_LAN,
		.active_low	= 1,
	}, {
		.name		= "om5p:blue:wan",
		.gpio		= OM5P_GPIO_LED_WAN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button om5p_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = OM5P_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= OM5P_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct flash_platform_data om5p_flash_data = {
	.type = "mx25l12805d",
};

static void __init om5p_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);
	u8 mac[6];

	/* make lan / wan leds software controllable */
	ath79_gpio_output_select(OM5P_GPIO_LED_LAN, AR934X_GPIO_OUT_GPIO);
	ath79_gpio_output_select(OM5P_GPIO_LED_WAN, AR934X_GPIO_OUT_GPIO);

	ath79_register_m25p80(&om5p_flash_data);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(om5p_leds_gpio),
				 om5p_leds_gpio);
	ath79_register_gpio_keys_polled(-1, OM5P_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(om5p_gpio_keys),
					om5p_gpio_keys);

	ath79_init_mac(mac, art, 2);
	ath79_register_wmac(art + OM5P_WMAC_CALDATA_OFFSET, mac);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_PHY_SWAP);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art, 1);

	/* GMAC0 is connected to the PHY0 of the internal switch */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_register_eth(0);

	/* GMAC1 is connected to the internal switch */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_OM5P, "OM5P", "OpenMesh OM5P", om5p_setup);
