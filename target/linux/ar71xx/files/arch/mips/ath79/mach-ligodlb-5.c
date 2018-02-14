/*
 *  LigoWawe(Deliberant) LigoDLB Propeller 5 support
 *
 *  Copyright (C) 2010-2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2011-2012 Anan Huang <axishero@foxmail.com>
 *  Copyright (C) 2017 Robert Marko <robimarko@otvorenamreza.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */
 
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define LIGODLB_GPIO_BTN_RESET		17
#define LIGODLB_GPIO_LED_LAN0		1
#define LIGODLB_GPIO_LED_L1		2
#define LIGODLB_GPIO_LED_L2		3
#define LIGODLB_GPIO_LED_L3		4
#define LIGODLB_GPIO_LED_L4		13

#define LIGODLB_GPIO_LED_PWR		0

#define LIGODLB_KEYS_POLL_INTERVAL	20	/* msecs */
#define LIGODLB_KEYS_DEBOUNCE_INTERVAL	(3 * LIGODLB_KEYS_POLL_INTERVAL)

static struct gpio_led ligdolb_leds_gpio[] __initdata = {
	{
		.name		= "dlb-5:green:power",
		.gpio		= LIGODLB_GPIO_LED_PWR,
		.default_state  = LEDS_GPIO_DEFSTATE_ON,
		.active_low	= 1,
	}, {
		.name		= "dlb-5:green:lan0",
		.gpio		= LIGODLB_GPIO_LED_LAN0,
		.active_low	= 1,
	}, {
		.name		= "dlb-5:yellow:link1",
		.gpio		= LIGODLB_GPIO_LED_L1,
		.active_low	= 1,
	}, {
		.name		= "dlb-5:yellow:link2",
		.gpio		= LIGODLB_GPIO_LED_L2,
		.active_low	= 1,
	}, {
		.name		= "dlb-5:yellow:link3",
		.gpio		= LIGODLB_GPIO_LED_L3,
		.active_low	= 1,
	}, {
		.name		= "dlb-5:yellow:link4",
		.gpio		= LIGODLB_GPIO_LED_L4,
		.active_low	= 1,
	},
};

static struct gpio_keys_button ligdolb_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = LIGODLB_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= LIGODLB_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init ligdolb_5_setup(void)
{
	u8 *eth_mac = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ath79_register_m25p80(NULL);
	ath79_register_wmac(ee, NULL);

	//Disable JTAG to enable use of GPIO 0-4
	ath79_gpio_function_setup(AR934X_GPIO_FUNC_JTAG_DISABLE,
				  AR934X_GPIO_FUNC_CLK_OBS4_EN);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_MII_GMAC0 |
				   AR934X_ETH_CFG_MII_GMAC0_SLAVE);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(ligdolb_leds_gpio),
				 ligdolb_leds_gpio);
	ath79_register_gpio_keys_polled(-1, LIGODLB_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ligdolb_gpio_keys),
					 ligdolb_gpio_keys);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, eth_mac, 0);

	/* GMAC0 is connected to an AR8032 PHY */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_LIGODLB_5, "DLB-5",
	     "LigoDLB Propeller 5", ligdolb_5_setup);
