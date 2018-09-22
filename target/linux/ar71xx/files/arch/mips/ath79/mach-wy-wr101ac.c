/*
 * GainStrong Oolite V5.2 module and development board support
 *
 * Copyright (C) 2018 Piotr Dymacz <pepe2k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"


#define WY_WR101AC_GPIO_LED_WAN		4
#define WY_WR101AC_GPIO_LED_LAN1	11
#define WY_WR101AC_GPIO_LED_LAN2	14
#define WY_WR101AC_GPIO_LED_LAN3	15
#define WY_WR101AC_GPIO_LED_LAN4	16
#define WY_WR101AC_GPIO_LED_STATUS	13
#define WY_WR101AC_GPIO_LED_WLAN_2G	12
#define WY_WR101AC_GPIO_BTN_RESET_WPS	17

#define WY_WR101AC_KEYS_POLL_INTERVAL		20 /* msec */
#define WY_WR101AC_KEYS_DEBOUNCE_INTERVAL	(3 * WY_WR101AC_KEYS_POLL_INTERVAL)

#define WY_WR101AC_WMAC_CALDATA_OFFSET	0x1000

static const char *wy_wr101ac_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data wy_wr101ac_flash_data = {
	.part_probes = wy_wr101ac_part_probes,
};

static struct gpio_led wy_wr101ac_dev_gpio_leds[] __initdata = {
	{
		.name		= "wr101ac:green:status",
		.gpio		= WY_WR101AC_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "wr101ac:green:wlan-2g",
		.gpio		= WY_WR101AC_GPIO_LED_WLAN_2G,
		.active_low	= 1,
	}, {
		.name		= "wr101ac:green:lan1",
		.gpio		= WY_WR101AC_GPIO_LED_LAN1,
		.active_low	= 1,
	}, {
		.name		= "wr101ac:green:lan2",
		.gpio		= WY_WR101AC_GPIO_LED_LAN2,
		.active_low	= 1,
	}, {
		.name		= "wr101ac:green:lan3",
		.gpio		= WY_WR101AC_GPIO_LED_LAN3,
		.active_low	= 1,
	}, {
		.name		= "wr101ac:green:lan4",
		.gpio		= WY_WR101AC_GPIO_LED_LAN4,
		.active_low	= 1,
	}, {
		.name		= "wr101ac:green:wan",
		.gpio		= WY_WR101AC_GPIO_LED_WAN,
		.active_low	= 1,
	},

};

static struct gpio_keys_button wy_wr101ac_dev_gpio_keys[] __initdata = {
	{
		.desc		= "wps button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WY_WR101AC_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WY_WR101AC_GPIO_BTN_RESET_WPS,
		.active_low	= 1,
	}
};

static void __init wy_wr101ac_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(&wy_wr101ac_flash_data);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(wy_wr101ac_dev_gpio_leds),
				 wy_wr101ac_dev_gpio_leds);
	ath79_register_gpio_keys_polled(-1, WY_WR101AC_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(wy_wr101ac_dev_gpio_keys),
					wy_wr101ac_dev_gpio_keys);

	ath79_register_usb();
	ath79_register_pci();
	ath79_register_wmac(art + WY_WR101AC_WMAC_CALDATA_OFFSET, NULL);
	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask |= BIT(4);

	/* LAN */
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_init_mac(ath79_eth1_data.mac_addr, art, 0);
	ath79_register_eth(1);

	/* WAN */
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.speed = SPEED_100;
	ath79_init_mac(ath79_eth0_data.mac_addr, art, 1);
	ath79_register_eth(0);


	
}


MIPS_MACHINE(ATH79_MACH_WY_WR101AC, "WY-WR101AC",
	     "wy-wr101ac", wy_wr101ac_setup);

