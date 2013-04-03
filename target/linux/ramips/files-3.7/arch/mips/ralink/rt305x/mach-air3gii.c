/*
 *  AirLive Air3GII board support
 *
 *  Copyright (C) 2012 Cezary Jackiewicz <cezary.jackiewicz@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */


#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define AIR3GII_GPIO_BUTTON_WPS 7

#define AIR3GII_GPIO_LED_WLAN   8
#define AIR3GII_GPIO_LED_MOBILE 9

#define AIR3GII_KEYS_POLL_INTERVAL   20
#define AIR3GII_KEYS_DEBOUNCE_INTERVAL (3 * AIR3GII_KEYS_POLL_INTERVAL)

const struct flash_platform_data air3gii_flash = {
	.type		= "en25q32b",
};

struct spi_board_info air3gii_spi_slave_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &air3gii_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

static struct gpio_led air3gii_leds_gpio[] __initdata = {
	{
		.name		= "airlive:green:wlan",
		.gpio		= AIR3GII_GPIO_LED_WLAN,
		.active_low	= 0,
	}, {
		.name		= "airlive:green:mobile",
		.gpio		= AIR3GII_GPIO_LED_MOBILE,
		.active_low	= 1,
	}
};

static struct gpio_keys_button air3gii_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AIR3GII_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AIR3GII_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init air3gii_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_spi(air3gii_spi_slave_info,
			    ARRAY_SIZE(air3gii_spi_slave_info));

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();

	ramips_register_gpio_leds(-1, ARRAY_SIZE(air3gii_leds_gpio),
				  air3gii_leds_gpio);

	ramips_register_gpio_buttons(-1, AIR3GII_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(air3gii_gpio_buttons),
				     air3gii_gpio_buttons);

	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_AIR3GII, "AIR3GII", "AirLive Air3GII",
		air3gii_init);
