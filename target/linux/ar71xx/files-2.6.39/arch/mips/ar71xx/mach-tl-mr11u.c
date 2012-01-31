/*
 *  TP-LINK TL-MR11U board support
 *
 *  Copyright (C) 2011 dongyuqi <729650915@qq.com>
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-ar9xxx-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"

#define TL_MR11U_GPIO_LED_3G		27
#define TL_MR11U_GPIO_LED_WLAN		26
#define TL_MR11U_GPIO_LED_LAN		17

#define TL_MR11U_GPIO_BTN_WPS		20
#define TL_MR11U_GPIO_BTN_RESET		11

#define TL_MR11U_GPIO_USB_POWER	8

#define TL_MR11U_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_MR11U_KEYS_DEBOUNCE_INTERVAL	(3 * TL_MR11U_KEYS_POLL_INTERVAL)

static const char *tl_mr11u_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_mr11u_flash_data = {
	.part_probes	= tl_mr11u_part_probes,
};

static struct gpio_led tl_mr11u_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:green:3g",
		.gpio		= TL_MR11U_GPIO_LED_3G,
		.active_low	= 1,
	},
	{
		.name		= "tp-link:green:wlan",
		.gpio		= TL_MR11U_GPIO_LED_WLAN,
		.active_low	= 1,
	},
	{
		.name		= "tp-link:green:lan",
		.gpio		= TL_MR11U_GPIO_LED_LAN,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tl_mr11u_gpio_keys[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TL_MR11U_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_MR11U_GPIO_BTN_WPS,
		.active_low	= 0,
	},
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_MR11U_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_MR11U_GPIO_BTN_RESET,
		.active_low	= 0,
	}
};

static void __init tl_mr11u_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_m25p80(&tl_mr11u_flash_data);
	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_mr11u_leds_gpio),
					tl_mr11u_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, TL_MR11U_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tl_mr11u_gpio_keys),
					 tl_mr11u_gpio_keys);

	gpio_request(TL_MR11U_GPIO_USB_POWER, "USB power");
	gpio_direction_output(TL_MR11U_GPIO_USB_POWER, 1);
	ar71xx_add_device_usb();

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);

	ar71xx_add_device_mdio(0, 0x0);
	ar71xx_add_device_eth(0);

	ar9xxx_add_device_wmac(ee, mac);
}

MIPS_MACHINE(AR71XX_MACH_TL_MR11U, "TL-MR11U", "TP-LINK TL-MR11U",
	     tl_mr11u_setup);
