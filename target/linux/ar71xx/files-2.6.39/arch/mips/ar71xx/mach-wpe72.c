/*
 *  Compex WPE72 board support
 *
 *  Copyright (C) 2012 Johnathan Boyce<jon.boyce@globalreach.eu.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include<asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-pb42-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define WPE72_GPIO_RESET	12
#define WPE72_GPIO_LED_DIAG	13
#define WPE72_GPIO_LED_1	14
#define WPE72_GPIO_LED_2	15
#define WPE72_GPIO_LED_3	16
#define WPE72_GPIO_LED_4	17

#define WPE72_KEYS_POLL_INTERVAL	20	/* msecs */
#define WPE72_KEYS_DEBOUNCE_INTERVAL	(3 * WPE72_KEYS_POLL_INTERVAL)

static struct gpio_led wpe72_leds_gpio[] __initdata = {
	{
		.name		= "wpe72:green:led1",
		.gpio		= WPE72_GPIO_LED_1,
		.active_low	= 1,
	}, {
		.name		= "wpe72:green:led2",
		.gpio		= WPE72_GPIO_LED_2,
		.active_low	= 1,
	}, {
		.name		= "wpe72:green:led3",
		.gpio		= WPE72_GPIO_LED_3,
		.active_low	= 1,
	}, {
		.name		= "wpe72:green:led4",
		.gpio		= WPE72_GPIO_LED_4,
		.active_low	= 1,
	}, {
		.name		= "wpe72:green:diag",
		.gpio		= WPE72_GPIO_LED_DIAG,
		.active_low	= 1,
	}
};

static struct gpio_keys_button wpe72_gpio_keys[] __initdata = {
	{
		.desc           = "reset",
		.type           = EV_KEY,
		.code           = KEY_RESTART,
		.debounce_interval = WPE72_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = WPE72_GPIO_RESET,
	}
};

static const char *wpe72_part_probes[] = {
	"MyLoader",
	NULL,
};

static struct flash_platform_data wpe72_flash_data = {
	.part_probes	= wpe72_part_probes,
};

static void __init wpe72_setup(void)
{
	ar71xx_add_device_m25p80(&wpe72_flash_data);
	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, ar71xx_mac_base, 1);

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	pb42_pci_init();

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wpe72_leds_gpio),
					wpe72_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, WPE72_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(wpe72_gpio_keys),
					wpe72_gpio_keys);
}

MIPS_MACHINE(AR71XX_MACH_WPE72, "WPE72", "Compex WPE72", wpe72_setup);
