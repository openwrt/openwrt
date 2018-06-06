/*
 * OCEDO Ursus board support
 * Based on the OCEDO Koala machine file
 *
 * Copyright (c) 2012 Qualcomm Atheros
 * Copyright (c) 2012-2013 Marek Lindner <marek@open-mesh.com>
 * Copyright (c) 2018 David Bauer <mail@david-bauer.net>
 * Copyright (c) 2018 Simon Terzenbach <simon@terzenba.ch>
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
 *
 */

#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <linux/platform_data/phy-at803x.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define URSUS_GPIO_LED_WLAN_5G		15
#define URSUS_GPIO_LED_WLAN_2G		14

#define URSUS_LAN0_MAC_OFFSET		0
#define URSUS_LAN1_MAC_OFFSET		0x06
#define URSUS_WIFI2G_MAC_OFFSET		0x0c
#define URSUS_WMAC_CALDATA_OFFSET	0x1000

static struct gpio_led ursus_leds_gpio[] __initdata = {
	{
		.name		= "ursus:green:wlan58",
		.gpio		= URSUS_GPIO_LED_WLAN_5G,
		.active_low	= 1,
	},
	{
		.name		= "ursus:green:wlan2",
		.gpio		= URSUS_GPIO_LED_WLAN_2G,
		.active_low	= 1,
	}
};


static struct at803x_platform_data ursus_at803x_data = {
	.disable_smarteee = 1,
	.enable_rgmii_rx_delay = 1,
	.enable_rgmii_tx_delay = 0,
	.fixup_rgmii_tx_delay = 1,
};

static struct mdio_board_info ursus_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 1,
		.platform_data = &ursus_at803x_data,
	},
};

static void __init ursus_setup_qca955x_eth_cfg(u32 mask,
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

static void __init ursus_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);
	u8 mac[6];

	ath79_eth0_pll_data.pll_1000 = 0xae000000;
	ath79_eth0_pll_data.pll_100 = 0xa0000101;
	ath79_eth0_pll_data.pll_10 = 0xa0001313;
	ath79_eth1_pll_data.pll_1000 = 0x03000000;
	ath79_eth1_pll_data.pll_100 = 0x00000101;
	ath79_eth1_pll_data.pll_10 = 0x00001313;

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(ursus_leds_gpio),
				 ursus_leds_gpio);

	ath79_init_mac(mac, art + URSUS_WIFI2G_MAC_OFFSET, 0);
	ath79_register_wmac(art + URSUS_WMAC_CALDATA_OFFSET, mac);
	ath79_register_pci();

	ursus_setup_qca955x_eth_cfg(QCA955X_ETH_CFG_RGMII_EN, 3, 3, 0, 0);
	ath79_register_mdio(0, 0x0);

	mdiobus_register_board_info(ursus_mdio0_info,
				    ARRAY_SIZE(ursus_mdio0_info));

	ath79_init_mac(ath79_eth0_data.mac_addr, art + URSUS_LAN0_MAC_OFFSET, 0);

	/* GMAC0 is connected to the RMGII interface */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(1);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;

	ath79_register_eth(0);

	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth1_data.mac_addr, art + URSUS_LAN1_MAC_OFFSET, 0);

	/* GMAC0 is connected to the RMGII interface */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth1_data.phy_mask = BIT(2);
	ath79_eth1_data.mii_bus_dev = &ath79_mdio0_device.dev;

	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_URSUS, "URSUS", "OCEDO Ursus", ursus_setup);
