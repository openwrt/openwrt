/*
 * D-Link DAP-1533 rev. A1 support
 *
 * Copyright (c) 2017 Taras Gavrylenko <gavrylenko.taras@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define DAP1533A1_GPIO_LED_GREEN_POWER	22
#define DAP1533A1_GPIO_LED_RED_POWER	14
#define DAP1533A1_GPIO_LED_WLAN			12
#define DAP1533A1_GPIO_LED_WLAN_CLIENT	19

#define DAP1533A1_GPIO_BTN_RESET		17
#define DAP1533A1_GPIO_BTN_WPS			16
#define DAP1533A1_GPIO_SW_RF_5GHz		18
#define DAP1533A1_GPIO_SW_BRIDGE		11

#define DAP1533A1_KEYS_POLL_INTERVAL	20	/* msecs */
#define DAP1533A1_KEYS_DEBOUNCE_INTERVAL	(3 * DAP1533A1_KEYS_POLL_INTERVAL)

#define DAP1533A1_MAC0_OFFSET			0x00FFA0
#define DAP1533A1_MAC1_OFFSET			0x00FFB4
#define DAP1533A1_PCIE_CALDATA_OFFSET	0x1000

static struct gpio_led dap1533a1_leds_gpio[] __initdata = {
	{
		.name		= "d-link:green:power",
		.gpio		= DAP1533A1_GPIO_LED_GREEN_POWER,
		.active_low	= 1,
	},
	{
		.name		= "d-link:red:power",
		.gpio		= DAP1533A1_GPIO_LED_RED_POWER,
		.active_low	= 1,
	},
	{
		.name		= "d-link:green:wlan",
		.gpio		= DAP1533A1_GPIO_LED_WLAN,
		.active_low	= 1,
	},
	{
		.name		= "d-link:green:wlan_client",
		.gpio		= DAP1533A1_GPIO_LED_WLAN_CLIENT,
		.active_low	= 1,
	},
};

static struct gpio_keys_button dap1533a1_gpio_keys[] __initdata = {
	{
		.desc			= "Soft reset",
		.type			= EV_KEY,
		.code			= KEY_RESTART,
		.debounce_interval	= DAP1533A1_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= DAP1533A1_GPIO_BTN_RESET,
		.active_low		= 1,
	},
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DAP1533A1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DAP1533A1_GPIO_BTN_WPS,
		.active_low	= 1,
	},
	{
		.desc           = "RF 5GHz band switch",
		.type           = EV_SW,
		.code           = BTN_1,
		.debounce_interval = DAP1533A1_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = DAP1533A1_GPIO_SW_RF_5GHz,
		.active_low	= 1,
	},
	{
		.desc           = "Bridge mode switch",
		.type           = EV_SW,
		.code           = BTN_2,
		.debounce_interval = DAP1533A1_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = DAP1533A1_GPIO_SW_BRIDGE,
		.active_low	= 1,
	},
};

static struct ar8327_pad_cfg dap1533a1_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.txclk_delay_en = true,
	.rxclk_delay_en = true,
	.txclk_delay_sel = AR8327_CLK_DELAY_SEL1,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2,
	.mac06_exchange_dis = true,
};

static struct ar8327_platform_data dap1533a1_ar8327_data = {
	.pad0_cfg = &dap1533a1_ar8327_pad0_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
};

static struct mdio_board_info dap1533a1_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &dap1533a1_ar8327_data,
	},
};

static struct flash_platform_data dap1533a1_flash_data = {
	.type = "mx25l6406e",
};

static void __init dap1533a1_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 mac0[ETH_ALEN], mac1[ETH_ALEN];
	u8 wmac0[ETH_ALEN];

	ath79_parse_ascii_mac(art + DAP1533A1_MAC0_OFFSET, mac0);
	ath79_parse_ascii_mac(art + DAP1533A1_MAC1_OFFSET, mac1);

	ath79_register_m25p80(&dap1533a1_flash_data);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(dap1533a1_leds_gpio),
				 dap1533a1_leds_gpio);

	ath79_register_gpio_keys_polled(-1, DAP1533A1_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(dap1533a1_gpio_keys),
					dap1533a1_gpio_keys);

	ath79_init_mac(wmac0, mac0, 0);
	ap91_pci_init(art + DAP1533A1_PCIE_CALDATA_OFFSET, wmac0);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0);

	mdiobus_register_board_info(dap1533a1_mdio0_info,
				    ARRAY_SIZE(dap1533a1_mdio0_info));

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, mac0, 0);

	/* GMAC0 is connected to an AR8327N switch */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_pll_data.pll_1000 = 0x06000000;
	ath79_register_eth(0);

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_DAP_1533_A1, "DAP-1533-A1",
		"D-Link DAP-1533 rev. A1",
		dap1533a1_setup);
