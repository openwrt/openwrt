/*
 * OCEDO Raccoon board support
 *
 * The OCEDO Raccoon uses similar hardware like the Openesh MR600.
 * This file is based on the one for the MR600.
 *
 * Copyright (C) 2012 Marek Lindner <marek@open-mesh.com>
 * Copyright (C) 2018 David Bauer <mail@david-bauer.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-wmac.h"
#include "machtypes.h"


#define RACCOON_LAN_MAC_OFFSET		0
#define RACCOON_WIFI2G_MAC_OFFSET	0x6
#define RACCOON_WIFI5G_MAC_OFFSET	0xc

#define RACCOON_WMAC_CALDATA_OFFSET	0x1000
#define RACCOON_PCIE_CALDATA_OFFSET	0x5000

#define RACCOON_GPIO_LED_WLAN24		12
#define RACCOON_GPIO_LED_SYS		13
#define RACCOON_GPIO_LED_POWER		14

#define RACCOON_GPIO_BTN_RESET		17

#define RACCOON_KEYS_POLL_INTERVAL	20	/* msecs */
#define RACCOON_KEYS_DEBOUNCE_INTERVAL (3 * RACCOON_KEYS_POLL_INTERVAL)


static struct gpio_led raccoon_leds_gpio[] __initdata = {
	{
		.name		= "raccoon:green:power",
		.gpio		= RACCOON_GPIO_LED_POWER,
		.active_low	= 1,
	},
	{
		.name		= "raccoon:blue:sys",
		.gpio		= RACCOON_GPIO_LED_SYS,
		.active_low	= 1,
	},
	{
		.name		= "raccoon:yellow:wlan24",
		.gpio		= RACCOON_GPIO_LED_WLAN24,
		.active_low	= 1,
	},
};

static struct gpio_keys_button raccoon_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = RACCOON_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RACCOON_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};

static void __init raccoon_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);
	u8 wifi24_mac[6];
	u8 wifi5_mac[6];

	ath79_register_m25p80(NULL);

	ath79_init_mac(wifi24_mac, art + RACCOON_WIFI2G_MAC_OFFSET, 0);
	ath79_register_wmac(art + RACCOON_WMAC_CALDATA_OFFSET, wifi24_mac);

	ath79_init_mac(wifi5_mac, art + RACCOON_WIFI5G_MAC_OFFSET, 0);
	ap91_pci_init(art + RACCOON_PCIE_CALDATA_OFFSET, wifi5_mac);

	ap9x_pci_setup_wmac_led_pin(0, 0);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 |
				   AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_register_mdio(0, 0);
	ath79_init_mac(ath79_eth0_data.mac_addr, art + RACCOON_LAN_MAC_OFFSET, 0);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_pll_data.pll_1000 = 0x06000000;
	ath79_register_eth(0);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(raccoon_leds_gpio), raccoon_leds_gpio);
	ath79_register_gpio_keys_polled(-1, RACCOON_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(raccoon_gpio_keys),
					raccoon_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_RACCOON, "RACCOON", "OCEDO Raccoon",
	     raccoon_setup);
