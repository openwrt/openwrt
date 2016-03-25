/*
 *  NETGEAR WNR2000v3/WNR612v2/WNR1000v2/WPN824N board support
 *
 *  Copyright (C) 2015 Hartmut Knaack <knaack.h@gmx.de>
 *  Copyright (C) 2013 Mathieu Olivari <mathieu.olivari@gmail.com>
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

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h> /* needed to disable switch LEDs */
#include "common.h" /* needed to disable switch LEDs */

#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "machtypes.h"

#define WNR2000V3_GPIO_LED_WAN_GREEN	0
#define WNR2000V3_GPIO_LED_LAN1_AMBER	1
#define WNR2000V3_GPIO_LED_LAN4_AMBER	12
#define WNR2000V3_GPIO_LED_PWR_GREEN	14
#define WNR2000V3_GPIO_BTN_WPS		11

#define WNR612V2_GPIO_LED_PWR_GREEN	11

#define WNR1000V2_GPIO_LED_PWR_AMBER	1
#define WNR1000V2_GPIO_LED_PWR_GREEN	11

/* Connected through AR7240 */
#define WPN824N_GPIO_LED_WAN_AMBER	0
#define WPN824N_GPIO_LED_STATUS_AMBER	1
#define WPN824N_GPIO_LED_LAN1_AMBER	6 /* AR724X_GPIO_FUNC_JTAG_DISABLE */
#define WPN824N_GPIO_LED_LAN2_AMBER	7 /* AR724X_GPIO_FUNC_JTAG_DISABLE */
#define WPN824N_GPIO_LED_LAN3_AMBER	8 /* AR724X_GPIO_FUNC_JTAG_DISABLE */
#define WPN824N_GPIO_LED_LAN4_AMBER	12
#define WPN824N_GPIO_LED_LAN1_GREEN	13
#define WPN824N_GPIO_LED_LAN2_GREEN	14
#define WPN824N_GPIO_LED_LAN3_GREEN	15 /* AR724X_GPIO_FUNC_CLK_OBS3_EN */
#define WPN824N_GPIO_LED_LAN4_GREEN	16
#define WPN824N_GPIO_LED_WAN_GREEN	17

/* Connected through AR9285 */
#define WPN824N_WGPIO_LED_PWR_GREEN	0
#define WPN824N_WGPIO_LED_WLAN_BLUE	1
#define WPN824N_WGPIO_LED_WPS1_BLUE	5
#define WPN824N_WGPIO_LED_WPS2_BLUE	9
#define WPN824N_WGPIO_LED_TEST_AMBER	10
#define WPN824N_WGPIO_BTN_PUSH		6	/* currently unused */
#define WPN824N_WGPIO_BTN_RESET		7	/* currently unused */
#define WPN824N_WGPIO_BTN_WLAN		8	/* currently unused */

#define WNR2000V3_KEYS_POLL_INTERVAL	20	/* msecs */
#define WNR2000V3_KEYS_DEBOUNCE_INTERVAL	(3 * WNR2000V3_KEYS_POLL_INTERVAL)

#define WNR2000V3_MAC0_OFFSET		0
#define WNR2000V3_MAC1_OFFSET		6
#define WNR2000V3_PCIE_CALDATA_OFFSET	0x1000

static struct gpio_led wnr2000v3_leds_gpio[] __initdata = {
	{
		.name		= "wnr2000v3:green:power",
		.gpio		= WNR2000V3_GPIO_LED_PWR_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wnr2000v3:green:wan",
		.gpio		= WNR2000V3_GPIO_LED_WAN_GREEN,
		.active_low	= 1,
	}
};

static struct gpio_led wnr612v2_leds_gpio[] __initdata = {
	{
		.name		= "netgear:green:power",
		.gpio		= WNR612V2_GPIO_LED_PWR_GREEN,
		.active_low	= 1,
	}
};

static struct gpio_led wnr1000v2_leds_gpio[] __initdata = {
	{
		.name		= "netgear:green:power",
		.gpio		= WNR1000V2_GPIO_LED_PWR_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:power",
		.gpio		= WNR1000V2_GPIO_LED_PWR_AMBER,
		.active_low	= 1,
	}
};

static struct gpio_led wpn824n_leds_gpio[] __initdata = {
	{
		.name		= "netgear:amber:wan",
		.gpio		= WPN824N_GPIO_LED_WAN_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:status",
		.gpio		= WPN824N_GPIO_LED_STATUS_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:lan1",
		.gpio		= WPN824N_GPIO_LED_LAN1_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:lan2",
		.gpio		= WPN824N_GPIO_LED_LAN2_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:lan3",
		.gpio		= WPN824N_GPIO_LED_LAN3_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:lan4",
		.gpio		= WPN824N_GPIO_LED_LAN4_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan1",
		.gpio		= WPN824N_GPIO_LED_LAN1_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan2",
		.gpio		= WPN824N_GPIO_LED_LAN2_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan3",
		.gpio		= WPN824N_GPIO_LED_LAN3_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan4",
		.gpio		= WPN824N_GPIO_LED_LAN4_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:wan",
		.gpio		= WPN824N_GPIO_LED_WAN_GREEN,
		.active_low	= 1,
	}
};

static struct gpio_led wpn824n_wmac_leds_gpio[] = {
	{
		.name		= "netgear:green:power",
		.gpio		= WPN824N_WGPIO_LED_PWR_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:blue:wlan",
		.gpio		= WPN824N_WGPIO_LED_WLAN_BLUE,
		.active_low	= 1,
	}, {
		.name		= "netgear:blue:wps1",
		.gpio		= WPN824N_WGPIO_LED_WPS1_BLUE,
		.active_low	= 1,
	}, {
		.name		= "netgear:blue:wps2",
		.gpio		= WPN824N_WGPIO_LED_WPS2_BLUE,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:test",
		.gpio		= WPN824N_WGPIO_LED_TEST_AMBER,
		.active_low	= 1,
	}
};

static struct gpio_keys_button wnr2000v3_gpio_keys[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WNR2000V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V3_GPIO_BTN_WPS,
	}
};

static void __init wnr_common_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art+WNR2000V3_MAC0_OFFSET, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;

	ath79_init_mac(ath79_eth1_data.mac_addr, art+WNR2000V3_MAC1_OFFSET, 0);
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ath79_eth1_data.phy_mask = 0x10;

	ath79_register_eth(0);
	ath79_register_eth(1);

	ath79_register_m25p80(NULL);
	ap91_pci_init(art + WNR2000V3_PCIE_CALDATA_OFFSET, NULL);
}

static void __init wnr2000v3_setup(void)
{
	wnr_common_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wnr2000v3_leds_gpio),
				 wnr2000v3_leds_gpio);

	ath79_register_gpio_keys_polled(-1, WNR2000V3_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(wnr2000v3_gpio_keys),
					wnr2000v3_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_WNR2000_V3, "WNR2000V3", "NETGEAR WNR2000 V3", wnr2000v3_setup);

static void __init wnr612v2_setup(void)
{
	wnr_common_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wnr612v2_leds_gpio),
				 wnr612v2_leds_gpio);
}

MIPS_MACHINE(ATH79_MACH_WNR612_V2, "WNR612V2", "NETGEAR WNR612 V2", wnr612v2_setup);

static void __init wnr1000v2_setup(void)
{
	wnr_common_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wnr1000v2_leds_gpio),
				 wnr1000v2_leds_gpio);
}

MIPS_MACHINE(ATH79_MACH_WNR1000_V2, "WNR1000V2", "NETGEAR WNR1000 V2", wnr1000v2_setup);

static void __init wpn824n_setup(void)
{
	ath79_gpio_function_setup(AR724X_GPIO_FUNC_JTAG_DISABLE,
				  AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				  AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				  AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				  AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				  AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN |
				  AR724X_GPIO_FUNC_CLK_OBS3_EN);

	wnr_common_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wpn824n_leds_gpio),
				 wpn824n_leds_gpio);

	ap9x_pci_setup_wmac_led_pin(0, WPN824N_WGPIO_LED_WLAN_BLUE);
	ap9x_pci_setup_wmac_leds(0, wpn824n_wmac_leds_gpio,
				 ARRAY_SIZE(wpn824n_wmac_leds_gpio));
}

MIPS_MACHINE(ATH79_MACH_WPN824N, "WPN824N", "NETGEAR WPN824N", wpn824n_setup);
