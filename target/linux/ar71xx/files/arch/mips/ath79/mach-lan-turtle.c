/*
 *  Hak5 LAN Turtle board support
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
#include "dev-wmac.h"
#include "machtypes.h"
#include "dev-usb.h"

#define HAK5_TURTLE_GPIO_BTN_RESET	11

#define HAK5_TURTLE_LED_SYS 13

#define HAK5_TURTLE_KEYS_POLL_INTERVAL	20	/* msecs */
#define HAK5_TURTLE_KEYS_DEBOUNCE_INTERVAL (3 * HAK5_TURTLE_KEYS_POLL_INTERVAL)

static const char *hak5_turtle_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data hak5_turtle_flash_data = {
	.part_probes	= hak5_turtle_part_probes,
};

static struct gpio_led hak5_turtle_leds_gpio[] __initdata = {
	{

		.name		= "turtle:yellow",
		.gpio		= HAK5_TURTLE_LED_SYS,
		.active_low	= 1,
	},

};

static struct gpio_keys_button hak5_turtle_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = HAK5_TURTLE_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= HAK5_TURTLE_GPIO_BTN_RESET,
		.active_low	= 0,
	},
};

static void __init hak5_turtle_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(hak5_turtle_leds_gpio),
				 hak5_turtle_leds_gpio);

	ath79_register_gpio_keys_polled(-1, HAK5_TURTLE_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(hak5_turtle_gpio_keys),
					hak5_turtle_gpio_keys);


	ath79_gpio_function_disable( AR933X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED3_EN);

	ath79_register_usb();

	ath79_register_m25p80(&hak5_turtle_flash_data);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, -1);

	ath79_register_mdio(0, 0x0);

	ath79_register_eth(0);
	ath79_register_eth(1);

}

MIPS_MACHINE(ATH79_MACH_LAN_TURTLE, "LAN-TURTLE",
	     "LAN Turtle", hak5_turtle_setup);