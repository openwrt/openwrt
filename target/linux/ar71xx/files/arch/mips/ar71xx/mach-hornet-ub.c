/*
 *  ALFA NETWORKS Hornet-UB board support
 *
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

#define HORNET_UB_GPIO_LED_WLAN		0
#define HORNET_UB_GPIO_LED_USB		1
#define HORNET_UB_GPIO_LED_LAN		13
#define HORNET_UB_GPIO_LED_WAN		17
#define HORNET_UB_GPIO_LED_WPS		27

#define HORNET_UB_GPIO_BTN_RESET	11
#define HORNET_UB_GPIO_BTN_WPS		12

#define HORNET_UB_GPIO_USB_POWER	26

#define HORNET_UB_KEYS_POLL_INTERVAL	20	/* msecs */
#define HORNET_UB_KEYS_DEBOUNCE_INTERVAL	(3 * HORNET_UB_KEYS_POLL_INTERVAL)

#define HORNET_UB_MAC0_OFFSET		0x0000
#define HORNET_UB_MAC1_OFFSET		0x0006
#define HORNET_UB_CALDATA_OFFSET	0x1000

static struct gpio_led hornet_ub_leds_gpio[] __initdata = {
	{
		.name		= "alfa:blue:lan",
		.gpio		= HORNET_UB_GPIO_LED_LAN,
		.active_low	= 0,
	},
	{
		.name		= "alfa:blue:usb",
		.gpio		= HORNET_UB_GPIO_LED_USB,
		.active_low	= 0,
	},
	{
		.name		= "alfa:blue:wan",
		.gpio		= HORNET_UB_GPIO_LED_WAN,
		.active_low	= 1,
	},
	{
		.name		= "alfa:blue:wlan",
		.gpio		= HORNET_UB_GPIO_LED_WLAN,
		.active_low	= 0,
	},
	{
		.name		= "alfa:blue:wps",
		.gpio		= HORNET_UB_GPIO_LED_WPS,
		.active_low	= 1,
	},
};

static struct gpio_keys_button hornet_ub_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = HORNET_UB_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= HORNET_UB_GPIO_BTN_WPS,
		.active_low	= 1,
	},
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = HORNET_UB_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= HORNET_UB_GPIO_BTN_RESET,
		.active_low	= 0,
	}
};

static void __init hornet_ub_gpio_setup(void)
{
	u32 t;

	ar71xx_gpio_function_disable(AR933X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				     AR933X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	t = ar71xx_reset_rr(AR933X_RESET_REG_BOOTSTRAP);
	t |= AR933X_BOOTSTRAP_MDIO_GPIO_EN;
	ar71xx_reset_wr(AR933X_RESET_REG_BOOTSTRAP, t);

	gpio_request(HORNET_UB_GPIO_USB_POWER, "USB power");
	gpio_direction_output(HORNET_UB_GPIO_USB_POWER, 1);
}

static void __init hornet_ub_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	hornet_ub_gpio_setup();

	ar71xx_add_device_m25p80(NULL);
	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(hornet_ub_leds_gpio),
					hornet_ub_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, HORNET_UB_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(hornet_ub_gpio_keys),
					 hornet_ub_gpio_keys);

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr,
			art + HORNET_UB_MAC0_OFFSET, 0);
	ar71xx_init_mac(ar71xx_eth0_data.mac_addr,
			art + HORNET_UB_MAC1_OFFSET, 0);

	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_add_device_eth(1);
	ar71xx_add_device_eth(0);

	ar9xxx_add_device_wmac(art + HORNET_UB_CALDATA_OFFSET, NULL);
	ar71xx_add_device_usb();
}

MIPS_MACHINE(AR71XX_MACH_HORNET_UB, "HORNET-UB", "ALFA NETWORKS Hornet-UB",
	     hornet_ub_setup);
