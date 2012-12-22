/*
 * TRENDnet TEW-692GR board support
 *
 * Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/rt2x00_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define TEW_692GR_GPIO_LED_WPS_ORANGE	9
#define TEW_692GR_GPIO_LED_WPS_GREEN	28

#define TEW_692GR_GPIO_BUTTON_RESET	10
#define TEW_692GR_GPIO_BUTTON_WPS	26

#define TEW_692GR_KEYS_POLL_INTERVAL	20
#define TEW_692GR_KEYS_DEBOUNCE_INTERVAL (3 * TEW_692GR_KEYS_POLL_INTERVAL)

static struct gpio_led tew_692gr_leds_gpio[] __initdata = {
	{
		.name		= "trendnet:orange:wps",
		.gpio		= TEW_692GR_GPIO_LED_WPS_ORANGE,
		.active_low	= 1,
	},
	{
		.name		= "trendnet:green:wps",
		.gpio		= TEW_692GR_GPIO_LED_WPS_GREEN,
		.active_low	= 1,
	},
};

static struct gpio_keys_button tew_692gr_gpio_buttons[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TEW_692GR_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW_692GR_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TEW_692GR_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW_692GR_GPIO_BUTTON_WPS,
		.active_low	= 1,
	},
};

static struct ar8327_pad_cfg tew_692gr_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.txclk_delay_en = true,
	.rxclk_delay_en = true,
	.txclk_delay_sel = AR8327_CLK_DELAY_SEL1,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2,
};

static struct ar8327_pad_cfg tew_692gr_ar8327_pad6_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.rxclk_delay_en = true,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL0,
};

static struct ar8327_led_cfg tew_692gr_ar8327_led_cfg = {
	.led_ctrl0 = 0xc437c437,
	.led_ctrl1 = 0xc337c337,
	.led_ctrl2 = 0x00000000,
	.led_ctrl3 = 0x03ffff00,
	.open_drain = false,
};

static struct ar8327_platform_data tew_692gr_ar8327_data = {
	.pad0_cfg = &tew_692gr_ar8327_pad0_cfg,
	.pad6_cfg = &tew_692gr_ar8327_pad6_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
	.led_cfg = &tew_692gr_ar8327_led_cfg,
};

static struct mdio_board_info tew_692gr_mdio0_info[] = {
	{
		.bus_id = "ramips_mdio",
		.phy_addr = 0,
		.platform_data = &tew_692gr_ar8327_data,
	},
};

static void __init tew_692gr_init(void)
{
	rt3883_gpio_init(RT3883_GPIO_MODE_I2C |
			 RT3883_GPIO_MODE_SPI |
			 RT3883_GPIO_MODE_UART0(RT3883_GPIO_MODE_GPIO) |
			 RT3883_GPIO_MODE_JTAG |
			 RT3883_GPIO_MODE_PCI(RT3883_GPIO_MODE_PCI_FNC));

	rt3883_register_pflash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(tew_692gr_leds_gpio),
				  tew_692gr_leds_gpio);

	ramips_register_gpio_buttons(-1, TEW_692GR_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(tew_692gr_gpio_buttons),
				     tew_692gr_gpio_buttons);

	rt3883_wlan_data.disable_5ghz = 1;
	rt3883_register_wlan();

	mdiobus_register_board_info(tew_692gr_mdio0_info,
				    ARRAY_SIZE(tew_692gr_mdio0_info));

	rt3883_eth_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	rt3883_eth_data.phy_mask = BIT(0);
	rt3883_eth_data.tx_fc = 1;
	rt3883_eth_data.rx_fc = 1;
	rt3883_register_ethernet();

	rt3883_register_wdt(false);

	rt3883_pci_init(RT3883_PCI_MODE_PCIE);
}

MIPS_MACHINE(RAMIPS_MACH_TEW_692GR, "TEW-692GR", "TRENDnet TEW-692GR",
	     tew_692gr_init);
