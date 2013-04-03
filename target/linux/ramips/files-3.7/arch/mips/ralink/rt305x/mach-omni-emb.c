/*
 *  Omnima MiniEMBWiFi board support
 *
 *  Copyright (C) 2011 Johnathan Boyce <jon.boyce@globalreach.eu.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define OMNI_EMB_GPIO_BUTTON_RESET	12 /* active low */

#define OMNI_EMB_KEYS_POLL_INTERVAL	20
#define OMNI_EMB_KEYS_DEBOUNCE_INTERVAL	(3 * OMNI_EMB_KEYS_POLL_INTERVAL)

#define OMNI_EMB_GPIO_LED_STATUS	9
#define OMNI_EMB_GPIO_LED_WLAN		14

static struct gpio_led omni_emb_leds_gpio[] __initdata = {
	{
		.name           = "emb:green:status",
		.gpio           = OMNI_EMB_GPIO_LED_STATUS,
		.active_low     = 1,
	}, {
		.name           = "emb:green:wlan",
		.gpio           = OMNI_EMB_GPIO_LED_WLAN,
		.active_low     = 1,
	}
};

static struct gpio_keys_button omni_emb_gpio_buttons[] __initdata = {
	{
		.desc           = "reset",
		.type           = EV_KEY,
		.code           = KEY_RESTART,
		.debounce_interval = OMNI_EMB_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = OMNI_EMB_GPIO_BUTTON_RESET,
		.active_low     = 1,
	}
};

static void __init omni_emb_init(void)
{
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;

	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(omni_emb_leds_gpio),
				omni_emb_leds_gpio);
	ramips_register_gpio_buttons(-1, OMNI_EMB_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(omni_emb_gpio_buttons),
				omni_emb_gpio_buttons);

	rt305x_register_flash(0);
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_OMNI_EMB, "OMNI-EMB", "Omnima MiniEMBWiFi",
	     omni_emb_init);
