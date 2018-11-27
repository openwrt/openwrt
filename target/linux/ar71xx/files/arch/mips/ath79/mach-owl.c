/*
 *  Hak5 Owl board support
 *
 *  Copyright (C) 2018 Sebastian Kinne <seb@hak5.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

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

#define OWL_GPIO_LED_SYSTEM 27

#define OWL_GPIO_BTN_RESET	12

#define OWL_KEYS_POLL_INTERVAL		20	/* msecs */
#define OWL_KEYS_DEBOUNCE_INTERVAL	(3 * OWL_KEYS_POLL_INTERVAL)

#define OWL_CALDATA_OFFSET	0x1000

static struct gpio_led owl_leds_gpio[] __initdata = {
	{
		.name		= "owl:red:system",
		.gpio		= OWL_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button owl_gpio_keys[] __initdata = {
	{
		.desc		= "Reset Button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = OWL_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= OWL_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init owl_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1f040000);

	ath79_register_m25p80(NULL);

	ath79_register_mdio(0, 0x0);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(owl_leds_gpio),
					owl_leds_gpio);
	ath79_register_gpio_keys_polled(-1, OWL_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(owl_gpio_keys),
					 owl_gpio_keys);

	ath79_register_wmac(art + OWL_CALDATA_OFFSET, NULL);

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_OWL, "OWL", "Owl",
	    owl_setup);