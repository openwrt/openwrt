/*
 *  EnGenius EMR3000 board support
 *
 *  Copyright (c) 2017-2018 Christian Beier <cb@shoutrlabs.com>
 *
 *  based on mach-epg5000.c
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "pci.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "nvram.h"

#define EMR3000_GPIO_LED_RED		11
#define EMR3000_GPIO_LED_ORANGE 	16
#define EMR3000_GPIO_LED_WHITE	        18
#define EMR3000_GPIO_LED_BLUE	        19

#define EMR3000_GPIO_BTN_RESET		17

#define EMR3000_KEYS_POLL_INTERVAL	20	/* msecs */
#define EMR3000_KEYS_DEBOUNCE_INTERVAL	(3 * EMR3000_KEYS_POLL_INTERVAL)

#define EMR3000_ART_ADDR                0x1f050000
#define EMR3000_WMAC_CALDATA_OFFSET     0x1000

#define EMR3000_NVRAM_ADDR	0x1f040000
#define EMR3000_NVRAM_SIZE	0x10000

static struct gpio_led emr3000_leds_gpio[] __initdata = {
	{
		.name		= "emr3000:red:front",
		.gpio		= EMR3000_GPIO_LED_RED,
		.active_low	= 0,
	},
	{
		.name		= "emr3000:orange:front",
		.gpio		= EMR3000_GPIO_LED_ORANGE,
		.active_low	= 0,
	},
	{
		.name		= "emr3000:white:front",
		.gpio		= EMR3000_GPIO_LED_WHITE,
		.active_low	= 0,
	},
	{
		.name		= "emr3000:blue:front",
		.gpio		= EMR3000_GPIO_LED_BLUE,
		.active_low	= 0,
	}
};

static struct gpio_keys_button emr3000_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = EMR3000_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= EMR3000_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};


static void __init emr3000_setup_qca955x_eth_cfg(u32 mask,
						unsigned int rxd,
						unsigned int rxdv,
						unsigned int txd,
						unsigned int txe)
{
	void __iomem *base;
	u32 t;

	base = ioremap(QCA955X_GMAC_BASE, QCA955X_GMAC_SIZE);

	t = mask;
	t |= rxd << QCA955X_ETH_CFG_RXD_DELAY_SHIFT;
	t |= rxdv << QCA955X_ETH_CFG_RDV_DELAY_SHIFT;
	t |= txd << QCA955X_ETH_CFG_TXD_DELAY_SHIFT;
	t |= txe << QCA955X_ETH_CFG_TXE_DELAY_SHIFT;

	__raw_writel(t, base + QCA955X_GMAC_REG_ETH_CFG);

	iounmap(base);
}


static int emr3000_get_mac(const char *name, char *mac)
{
	u8 *nvram = (u8 *) KSEG1ADDR(EMR3000_NVRAM_ADDR);
	int err;

	err = ath79_nvram_parse_mac_addr(nvram, EMR3000_NVRAM_SIZE,
					 name, mac);
	if (err) {
		pr_err("no MAC address found for %s\n", name);
		return false;
	}

	return true;
}


static void __init emr3000_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(EMR3000_ART_ADDR);
	u8 mac0[ETH_ALEN], mac1[ETH_ALEN];

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(emr3000_leds_gpio),
					emr3000_leds_gpio);
	ath79_register_gpio_keys_polled(-1, EMR3000_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(emr3000_gpio_keys),
					emr3000_gpio_keys);

	ath79_register_usb();

	emr3000_setup_qca955x_eth_cfg(QCA955X_ETH_CFG_RGMII_EN, 3, 3, 0, 0);

	ath79_register_mdio(0, 0x0);

	/* GMAC0 is connected to RGMII interface */
	if (emr3000_get_mac("wanaddr=", mac0))
		ath79_init_mac(ath79_eth0_data.mac_addr, mac0, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(1);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_pll_data.pll_1000 = 0x9a000000;
	ath79_eth0_pll_data.pll_100 = 0x80000101;
	ath79_eth0_pll_data.pll_10 = 0x80001313;

	ath79_register_eth(0);

	/* GMAC1 is connected to SGMII interface */
	if (emr3000_get_mac("ethaddr=", mac1))
		ath79_init_mac(ath79_eth1_data.mac_addr, mac1, 0);
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth1_data.phy_mask = BIT(2);
	ath79_eth1_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth1_pll_data.pll_1000 = 0x9b000000;
	ath79_eth1_pll_data.pll_100 = 0x80000101;
	ath79_eth1_pll_data.pll_10 = 0x80001313;

	ath79_register_eth(1);

	ath79_register_wmac(art + EMR3000_WMAC_CALDATA_OFFSET, mac1);

	ath79_register_pci();
}

MIPS_MACHINE(ATH79_MACH_EMR3000, "EMR3000",
	     "EnGenius EMR3000",
	     emr3000_setup);
