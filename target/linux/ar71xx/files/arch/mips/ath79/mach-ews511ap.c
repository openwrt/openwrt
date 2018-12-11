/*
 *  EnGenius EWS511AP board support
 *
 * Copyright (C) 2017 Guan-Hong Lin <GH.Lin@senao.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "nvram.h"
#include "pci.h"

#define EWS511AP_GPIO_LED_POWER				0
#define EWS511AP_GPIO_LED_LAN1				4
#define EWS511AP_GPIO_LED_LAN2				13
#define EWS511AP_GPIO_LED_WLAN2G			15
#define EWS511AP_GPIO_LED_WLAN5G			3

#define EWS511AP_GPIO_BTN_RESET				1

#define EWS511AP_GPIO_WDT_ENABLE			11
#define EWS511AP_GPIO_WDT_INPUT				12

#define EWS511AP_KEYS_POLL_INTERVAL			20	/* msecs */
#define EWS511AP_KEYS_DEBOUNCE_INTERVAL		(3 * EWS511AP_KEYS_POLL_INTERVAL)

#define EWS511AP_EXT_WDT_TIMEOUT_MS			500

#define EWS511AP_WMAC_ART_OFFSET			0x1000

#define EWS511AP_NVRAM_ADDR					0x1f040000
#define EWS511AP_NVRAM_SIZE					0x10000

static struct gpio_led ews511ap_leds_gpio[] __initdata = {
	{
		.name		= "ews511ap:amber:power",
		.gpio		= EWS511AP_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "ews511ap:blue:lan1",
		.gpio		= EWS511AP_GPIO_LED_LAN1,
		.active_low	= 1,
	}, {
		.name		= "ews511ap:blue:lan2",
		.gpio		= EWS511AP_GPIO_LED_LAN2,
		.active_low	= 1,
	}, {
		.name		= "ews511ap:green:wlan2g",
		.gpio		= EWS511AP_GPIO_LED_WLAN2G,
		.active_low	= 1,
	}, {
		.name		= "ews511ap:green:wlan5g",
		.gpio		= EWS511AP_GPIO_LED_WLAN5G,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ews511ap_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = EWS511AP_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= EWS511AP_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};

static struct timer_list gpio_wdt_timer;

static void gpio_wdt_toggle(unsigned long gpio)
{
	static int state;

	state = !state;
	gpio_set_value(gpio, state);

	mod_timer(&gpio_wdt_timer,
			jiffies + msecs_to_jiffies(EWS511AP_EXT_WDT_TIMEOUT_MS));
}

static void __init ews511ap_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1f050000);
	u8 *nvram = (u8 *) KSEG1ADDR(EWS511AP_NVRAM_ADDR);
	u8 mac_buff[6];

	/* Disable JTAG, enabling GPIOs 0-3 */
	ath79_gpio_function_setup(AR934X_GPIO_FUNC_JTAG_DISABLE, 0);

	// Register and feed WATCHDOG's GPIO
	gpio_request_one(EWS511AP_GPIO_WDT_ENABLE,
			GPIOF_OUT_INIT_LOW | GPIOF_EXPORT_DIR_FIXED,
			"WDT enable");


	gpio_request_one(EWS511AP_GPIO_WDT_INPUT,
			GPIOF_OUT_INIT_LOW | GPIOF_EXPORT_DIR_FIXED,
			"WDT input");
	setup_timer(&gpio_wdt_timer, gpio_wdt_toggle, EWS511AP_GPIO_WDT_INPUT);
	gpio_wdt_toggle(EWS511AP_GPIO_WDT_INPUT);

	ath79_register_m25p80(NULL);

	ath79_register_pci();

	ath79_register_wmac(art + EWS511AP_WMAC_ART_OFFSET, NULL);

	ath79_register_mdio(0, 0x0);

	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask |= BIT(4);

	if (ath79_nvram_parse_mac_addr(nvram, 0x10000,
				"ethaddr=", mac_buff) == 0) {
		ath79_init_mac(ath79_eth0_data.mac_addr, mac_buff, 0);
		ath79_init_mac(ath79_eth1_data.mac_addr, mac_buff, 1);
	}

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;

	ath79_register_eth(0);
	ath79_register_eth(1);

	ath79_gpio_output_select(EWS511AP_GPIO_LED_POWER, 0);
	ath79_gpio_output_select(EWS511AP_GPIO_LED_LAN1, 0);
	ath79_gpio_output_select(EWS511AP_GPIO_LED_LAN2, 0);
	ath79_gpio_output_select(EWS511AP_GPIO_LED_WLAN2G, 0);
	ath79_gpio_output_select(EWS511AP_GPIO_LED_WLAN5G, 0);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(ews511ap_leds_gpio),
				 ews511ap_leds_gpio);

	ath79_register_gpio_keys_polled(-1, EWS511AP_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(ews511ap_gpio_keys),
					ews511ap_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_EWS511AP, "EWS511AP", "EnGenius EWS511AP",
	     ews511ap_setup);
