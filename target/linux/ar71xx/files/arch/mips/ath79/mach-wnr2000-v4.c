/*
 *  NETGEAR WNR2000v4 board support
 *
 *  Copytight (C) 2014 Michaël Burtin <mburtin@gmail.com>
 *  Copytight (C) 2013 Mathieu Olivari <mathieu.olivari@gmail.com>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *  Copyright (C) 2008-2009 Andy Boyett <agb@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

/* AR9341 GPIOs */
#define WNR2000V4_GPIO_LED_WAN_AMBER   22
#define WNR2000V4_GPIO_LED_WAN_GREEN   17
#define WNR2000V4_GPIO_LED_WPS 2
#define WNR2000V4_GPIO_LED_WLAN 12
#define WNR2000V4_GPIO_LED_LAN1 18
#define WNR2000V4_GPIO_LED_LAN2 19
#define WNR2000V4_GPIO_LED_LAN3 20
#define WNR2000V4_GPIO_LED_LAN4 21
#define STATUS_LED_GPIO         1
#define WNR2000V4_GPIO_LED_PWR_GREEN    0

#define WNR2000V4_GPIO_BTN_WPS      3
#define WNR2000V4_GPIO_BTN_RESET    4
#define WNR2000V4_GPIO_BTN_WLAN     11

#define WNR2000V4_KEYS_POLL_INTERVAL	20	/* msecs */
#define WNR2000V4_KEYS_DEBOUNCE_INTERVAL	(3 * WNR2000V4_KEYS_POLL_INTERVAL)

#define WNR2000V4_MAC0_OFFSET		0
#define WNR2000V4_MAC1_OFFSET		6

static struct gpio_led wnr2000v4_leds_gpio[] __initdata = {
	{
		.name		= "wnr2000-v4:green:power",
		.gpio		= WNR2000V4_GPIO_LED_PWR_GREEN,
		.active_low	= 1,
        .default_trigger = "default-on",
	}, {
		.name		= "wnr2000-v4:green:wan",
		.gpio		= WNR2000V4_GPIO_LED_WAN_AMBER,
		.active_low	= 1,
	},
	{
		.name		= "wnr2000-v4:green:wps",
		.gpio		= WNR2000V4_GPIO_LED_WPS,
		.active_low	= 1,
	},
	{
		.name		= "wnr2000-v4:amber:status",
		.gpio		= STATUS_LED_GPIO,
		.active_low	= 1,
	},
	{
		.name		= "wnr2000-v4:blue:wlan",
		.gpio		= WNR2000V4_GPIO_LED_WLAN,
		.active_low	= 1,
	},
	{
		.name		= "wnr2000-v4:amber:lan1",
		.gpio		= WNR2000V4_GPIO_LED_LAN1,
		.active_low	= 1,
	}, {
		.name		= "wnr2000-v4:amber:lan2",
		.gpio		= WNR2000V4_GPIO_LED_LAN2,
		.active_low	= 1,
	}, {
		.name		= "wnr2000-v4:amber:lan3",
		.gpio		= WNR2000V4_GPIO_LED_LAN3,
		.active_low	= 1,
	}, {
		.name		= "wnr2000-v4:amber:lan4",
		.gpio		= WNR2000V4_GPIO_LED_LAN4,
		.active_low	= 1,
	}
};

static struct gpio_keys_button wnr2000v4_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WNR2000V4_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V4_GPIO_BTN_WPS,
	},
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WNR2000V4_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V4_GPIO_BTN_RESET,
	},
	{
		.desc		= "WLAN button",
		.type		= EV_KEY,
		.code		= KEY_WLAN,
		.debounce_interval = WNR2000V4_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V4_GPIO_BTN_WLAN,
	},
};

static void __init wnr_common_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ath79_register_m25p80(NULL);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_ONLY_MODE |
				   AR934X_ETH_CFG_SW_PHY_SWAP);

	ath79_register_mdio(1, 0x0);

	/* LAN */
	ath79_init_mac(ath79_eth1_data.mac_addr, art+WNR2000V4_MAC0_OFFSET, 0);

	/* GMAC1 is connected to the internal switch */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_register_eth(1);

	/* WAN */
	ath79_init_mac(ath79_eth0_data.mac_addr, art+WNR2000V4_MAC1_OFFSET, 0);

	/* GMAC0 is connected to the PHY0 of the internal switch */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(4);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;

	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;

	ath79_register_eth(0);

	/* WLAN */
	ath79_register_wmac(ee, art+WNR2000V4_MAC0_OFFSET);

	/* USB */
	ath79_register_usb();
}

static void __init wnr2000v4_setup(void)
{
	wnr_common_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wnr2000v4_leds_gpio),
				 wnr2000v4_leds_gpio);

	ath79_register_gpio_keys_polled(-1, WNR2000V4_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(wnr2000v4_gpio_keys),
					wnr2000v4_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_WNR2000_V4, "WNR2000V4", "NETGEAR WNR2000 V4", wnr2000v4_setup);
