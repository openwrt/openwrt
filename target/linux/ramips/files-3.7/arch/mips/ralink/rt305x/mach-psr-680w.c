/*
 *  Petatel PSR-680W Wireless 3G Router support
 *
 *  Copyright (C) 2012 Dmitry Shmygov <shmygov@rambler.ru>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define PSR_680W_GPIO_BUTTON_RESET	0	/* active low */

#define PSR_680W_GPIO_LED_STATUS	19

#define PSR_680W_KEYS_POLL_INTERVAL	20
#define PSR_680W_KEYS_DEBOUNCE_INTERVAL	(3 * PSR_680W_KEYS_POLL_INTERVAL)


static struct gpio_led psr_680w_leds_gpio[] __initdata = {
	{
		.name		= "psr-680w:red:wan",
		.gpio		= PSR_680W_GPIO_LED_STATUS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button psr_680w_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = PSR_680W_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PSR_680W_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static void __init psr_680w_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_I2S_UARTF << RT305X_GPIO_MODE_UART0_SHIFT) |
				RT305X_GPIO_MODE_SPI |
				RT305X_GPIO_MODE_JTAG |
				RT305X_GPIO_MODE_MDIO |
				RT305X_GPIO_MODE_RGMII);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(psr_680w_leds_gpio),
				  psr_680w_leds_gpio);
	ramips_register_gpio_buttons(-1, PSR_680W_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(psr_680w_gpio_buttons),
				     psr_680w_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_PSR_680W, "PSR-680W",
	     "Petatel PSR-680W Wireless 3G Router",
	     psr_680w_init);
