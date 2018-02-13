/*
 *  Hak5 WiFi Pineapple NANO board support
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

#define PINEAPPLE_NANO_GPIO_LED_SYSTEM		18

#define PINEAPPLE_NANO_GPIO_BTN_RESET	12
#define PINEAPPLE_NANO_GPIO_BTN_SD		19
#define PINEAPPLE_NANO_GPIO_USB_ALARM	20
#define PINEAPPLE_NANO_GPIO_USB_POWER	23

#define PINEAPPLE_NANO_KEYS_POLL_INTERVAL		20	/* msecs */
#define PINEAPPLE_NANO_KEYS_DEBOUNCE_INTERVAL	(3 * PINEAPPLE_NANO_KEYS_POLL_INTERVAL)

#define PINEAPPLE_NANO_MAC1_OFFSET		0x0006
#define PINEAPPLE_NANO_CALDATA_OFFSET	0x1000

static const char *pineapple_nano_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data pineapple_nano_flash_data = {
	.part_probes	= pineapple_nano_part_probes,
};

static struct gpio_led pineapple_nano_leds_gpio[] __initdata = {
	{
		.name		= "hak5:blue:system",
		.gpio		= PINEAPPLE_NANO_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button pineapple_nano_gpio_keys[] __initdata = {
	{
		.desc		= "SD Card Present",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = PINEAPPLE_NANO_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PINEAPPLE_NANO_GPIO_BTN_SD,
		.active_low	= 1,
	},
	{
		.desc		= "USB Alarm",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = PINEAPPLE_NANO_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PINEAPPLE_NANO_GPIO_USB_ALARM,
		.active_low	= 1,
	},
	{
		.desc		= "Reset Button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = PINEAPPLE_NANO_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PINEAPPLE_NANO_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init pineapple_nano_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_gpio_function_disable(AR933X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	// This enables the USB Host port on boot. Set to high to disable.
	gpio_request_one(PINEAPPLE_NANO_GPIO_USB_POWER,
			 GPIOF_OUT_INIT_LOW | GPIOF_EXPORT_DIR_FIXED,
			 "USB power");

	ath79_register_leds_gpio(-1, ARRAY_SIZE(pineapple_nano_leds_gpio),
					pineapple_nano_leds_gpio);
	ath79_register_gpio_keys_polled(-1, PINEAPPLE_NANO_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(pineapple_nano_gpio_keys),
					 pineapple_nano_gpio_keys);

	ath79_init_mac(ath79_eth0_data.mac_addr,
			art + PINEAPPLE_NANO_MAC1_OFFSET, 0);

	ath79_register_m25p80(&pineapple_nano_flash_data);
	ath79_register_mdio(0, 0x0);
	ath79_register_eth(0);
	ath79_register_wmac(art + PINEAPPLE_NANO_CALDATA_OFFSET, NULL);
	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_HAK5_PINEAPPLE_NANO, "HAK5-PINEAPPLE-NANO", "Hak5 WiFi Pineapple NANO",
	     pineapple_nano_setup);