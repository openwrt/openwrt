/*
 * TRENDnet TEW-691GR board support
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

#define TEW_691GR_GPIO_LED_WPS_GREEN	9

#define TEW_691GR_GPIO_BUTTON_RESET	10
#define TEW_691GR_GPIO_BUTTON_WPS	26

#define TEW_691GR_GPIO_SWITCH_RFKILL	0

#define TEW_691GR_KEYS_POLL_INTERVAL	20
#define TEW_691GR_KEYS_DEBOUNCE_INTERVAL (3 * TEW_691GR_KEYS_POLL_INTERVAL)

static struct gpio_led tew_691gr_leds_gpio[] __initdata = {
	{
		.name		= "trendnet:green:wps",
		.gpio		= TEW_691GR_GPIO_LED_WPS_GREEN,
		.active_low	= 1,
	},
};

static struct gpio_keys_button tew_691gr_gpio_buttons[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TEW_691GR_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW_691GR_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TEW_691GR_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW_691GR_GPIO_BUTTON_WPS,
		.active_low	= 1,
	},
	{
		.desc		= "RFKILL switch",
		.type		= EV_SW,
		.code		= KEY_RFKILL,
		.debounce_interval = TEW_691GR_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW_691GR_GPIO_SWITCH_RFKILL,
		.active_low	= 1,
	},
};

static void __init tew_691gr_init(void)
{
	rt3883_gpio_init(RT3883_GPIO_MODE_I2C |
			 RT3883_GPIO_MODE_SPI |
			 RT3883_GPIO_MODE_UART0(RT3883_GPIO_MODE_GPIO) |
			 RT3883_GPIO_MODE_JTAG |
			 RT3883_GPIO_MODE_PCI(RT3883_GPIO_MODE_PCI_FNC));

	rt3883_register_pflash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(tew_691gr_leds_gpio),
				  tew_691gr_leds_gpio);

	ramips_register_gpio_buttons(-1, TEW_691GR_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(tew_691gr_gpio_buttons),
				     tew_691gr_gpio_buttons);

	rt3883_wlan_data.disable_5ghz = 1;
	rt3883_register_wlan();

	rt3883_eth_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	rt3883_eth_data.phy_mask = BIT(0);
	rt3883_eth_data.tx_fc = 1;
	rt3883_eth_data.rx_fc = 1;
	rt3883_register_ethernet();

	rt3883_register_wdt(false);
}

MIPS_MACHINE(RAMIPS_MACH_TEW_691GR, "TEW-691GR", "TRENDnet TEW-691GR",
	     tew_691gr_init);
