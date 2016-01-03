/*
 *  NETGEAR WNR2200 board support
 *
 *  Copyright (C) 2013 Aidan Kissane <aidankissane at googlemail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ath79/ath79.h>

#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "machtypes.h"

#define WNR2200_GPIO_LED_LAN2_AMBER	0
#define WNR2200_GPIO_LED_LAN4_AMBER	1
#define WNR2200_GPIO_LED_WPS		5
#define WNR2200_GPIO_LED_WAN_GREEN	7
#define WNR2200_GPIO_LED_USB		8
#define WNR2200_GPIO_LED_LAN3_AMBER	11
#define WNR2200_GPIO_LED_WAN_AMBER	12
#define WNR2200_GPIO_LED_LAN1_GREEN	13
#define WNR2200_GPIO_LED_LAN2_GREEN	14
#define WNR2200_GPIO_LED_LAN3_GREEN	15
#define WNR2200_GPIO_LED_LAN4_GREEN	16
#define WNR2200_GPIO_LED_PWR_AMBER	21
#define WNR2200_GPIO_LED_PWR_GREEN	22

#define WNR2200_GPIO_USB_POWER		24

#define WNR2200_KEYS_POLL_INTERVAL	20 /* msecs */
#define WNR2200_KEYS_DEBOUNCE_INTERVAL	(3 * WNR2200_KEYS_POLL_INTERVAL)

#define WNR2200_MAC0_OFFSET		0
#define WNR2200_MAC1_OFFSET		6
#define WNR2200_PCIE_CALDATA_OFFSET	0x1000

static struct gpio_led wnr2200_leds_gpio[] __initdata = {
	{
		.name		= "netgear:amber:lan2",
		.gpio		= WNR2200_GPIO_LED_LAN2_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:lan4",
		.gpio		= WNR2200_GPIO_LED_LAN4_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:wps",
		.gpio		= WNR2200_GPIO_LED_WPS,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:wan",
		.gpio		= WNR2200_GPIO_LED_WAN_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:usb",
		.gpio		= WNR2200_GPIO_LED_USB,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:lan3",
		.gpio		= WNR2200_GPIO_LED_LAN3_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:wan",
		.gpio		= WNR2200_GPIO_LED_WAN_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan1",
		.gpio		= WNR2200_GPIO_LED_LAN1_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan2",
		.gpio		= WNR2200_GPIO_LED_LAN2_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan3",
		.gpio		= WNR2200_GPIO_LED_LAN3_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:lan4",
		.gpio		= WNR2200_GPIO_LED_LAN4_GREEN,
		.active_low	= 1,
	}, {
		.name		= "netgear:amber:power",
		.gpio		= WNR2200_GPIO_LED_PWR_AMBER,
		.active_low	= 1,
	}, {
		.name		= "netgear:green:power",
		.gpio		= WNR2200_GPIO_LED_PWR_GREEN,
		.active_low	= 1,
	}
};

static void __init wnr2200_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art+WNR2200_MAC0_OFFSET, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;

	ath79_init_mac(ath79_eth1_data.mac_addr, art+WNR2200_MAC1_OFFSET, 0);
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ath79_eth1_data.phy_mask = 0x10;

	ath79_register_eth(0);
	ath79_register_eth(1);

	ath79_register_m25p80(NULL);
	ap91_pci_init(art + WNR2200_PCIE_CALDATA_OFFSET,
		      art + WNR2200_MAC1_OFFSET);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wnr2200_leds_gpio),
					wnr2200_leds_gpio);

	/* enable power for the USB port */
	gpio_request_one(WNR2200_GPIO_USB_POWER,
			GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			"USB power");

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_WNR2200, "WNR2200", "NETGEAR WNR2200", wnr2200_setup);
