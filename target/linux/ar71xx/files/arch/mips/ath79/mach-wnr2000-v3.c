/*
 *  NETGEAR WNR2000v3/WNR612v2/WNR1000v2 board support
 *
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

#include <asm/mach-ath79/ath79.h>

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
