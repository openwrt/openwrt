/*
 *  Redwave RW2458N support
 *
 *  Copyright (C) 2011-2012 Cezary Jackiewicz <cezary@eko.one.pl>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap91-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define RW2458N_GPIO_LED_D3	1
#define RW2458N_GPIO_LED_D4	0
#define RW2458N_GPIO_LED_D5	11
#define RW2458N_GPIO_LED_D6	7
#define RW2458N_GPIO_BTN_RESET	12

#define RW2458N_KEYS_POLL_INTERVAL	20	/* msecs */
#define RW2458N_KEYS_DEBOUNCE_INTERVAL	(3 * RW2458N_KEYS_POLL_INTERVAL)

static struct gpio_keys_button rw2458n_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = RW2458N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RW2458N_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

#define RW2458N_WAN_PHYMASK	BIT(4)

static struct gpio_led rw2458n_leds_gpio[] __initdata = {
	{
		.name		= "rw2458n:green:d3",
		.gpio		= RW2458N_GPIO_LED_D3,
		.active_low	= 1,
	}, {
		.name		= "rw2458n:green:d4",
		.gpio		= RW2458N_GPIO_LED_D4,
		.active_low	= 1,
	}, {
		.name		= "rw2458n:green:d5",
		.gpio		= RW2458N_GPIO_LED_D5,
		.active_low	= 1,
	}, {
		.name		= "rw2458n:green:d6",
		.gpio		= RW2458N_GPIO_LED_D6,
		.active_low	= 1,
	}
};

static const char *rw2458n_part_probes[] = {
        "RedBoot",
        NULL,
};

static struct flash_platform_data rw2458n_flash_data = {
        .part_probes    = rw2458n_part_probes,
};

static void __init rw2458n_setup(void)
{
	u8 *mac1 = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 *mac2 = (u8 *) KSEG1ADDR(0x1fff0000 + ETH_ALEN);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_m25p80(&rw2458n_flash_data);

	ar71xx_add_device_mdio(0, ~RW2458N_WAN_PHYMASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac1, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac2, 0);

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ap91_pci_init(ee, NULL);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(rw2458n_leds_gpio),
					rw2458n_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, RW2458N_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(rw2458n_gpio_keys),
					rw2458n_gpio_keys);

	ar71xx_add_device_usb();
}

MIPS_MACHINE(AR71XX_MACH_RW2458N, "RW2458N", "Redwave RW2458N",
	    rw2458n_setup);
