/*
 *  Belkin F5D8235 v2 board support
 *
 *  Copyright (C) 2010 Roman Yeryomin <roman@advem.lv>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <linux/rtl8366.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define F5D8235_V2_GPIO_LED_INTERNET_BLUE	5
#define F5D8235_V2_GPIO_LED_INTERNET_AMBER	6
#define F5D8235_V2_GPIO_LED_MODEM_BLUE		11
#define F5D8235_V2_GPIO_LED_MODEM_AMBER		8
#define F5D8235_V2_GPIO_LED_ROUTER		9
#define F5D8235_V2_GPIO_LED_STORAGE_BLUE	23
#define F5D8235_V2_GPIO_LED_STORAGE_AMBER	22
#define F5D8235_V2_GPIO_LED_SECURITY_BLUE	13
#define F5D8235_V2_GPIO_LED_SECURITY_AMBER	12

static struct gpio_led f5d8235v2_leds_gpio[] __initdata = {
	{
		.name		= "f5d8235v2:blue:internet",
		.gpio		= F5D8235_V2_GPIO_LED_INTERNET_BLUE,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:amber:internet",
		.gpio		= F5D8235_V2_GPIO_LED_INTERNET_AMBER,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:blue:modem",
		.gpio		= F5D8235_V2_GPIO_LED_MODEM_BLUE,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:amber:modem",
		.gpio		= F5D8235_V2_GPIO_LED_MODEM_AMBER,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:blue:router",
		.gpio		= F5D8235_V2_GPIO_LED_ROUTER,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:blue:storage",
		.gpio		= F5D8235_V2_GPIO_LED_STORAGE_BLUE,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:amber:storage",
		.gpio		= F5D8235_V2_GPIO_LED_STORAGE_AMBER,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:blue:security",
		.gpio		= F5D8235_V2_GPIO_LED_SECURITY_BLUE,
		.active_low	= 1,
	}, {
		.name		= "f5d8235v2:amber:security",
		.gpio		= F5D8235_V2_GPIO_LED_SECURITY_AMBER,
		.active_low	= 1,
	}
};

static struct rtl8366_platform_data f5d8235v2_switch_data = {
	.gpio_sda	= RT305X_GPIO_I2C_SD,
	.gpio_sck	= RT305X_GPIO_I2C_SCLK,
};

static struct platform_device f5d8235v2_switch = {
	.name		= RTL8366RB_DRIVER_NAME,
	.id		= -1,
	.dev		= {
		.platform_data = &f5d8235v2_switch_data,
	}
};

static void __init f5d8235v2_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
					RT305X_GPIO_MODE_UART0_SHIFT) |
					RT305X_GPIO_MODE_I2C |
					RT305X_GPIO_MODE_SPI |
					RT305X_GPIO_MODE_MDIO);

	rt305x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(f5d8235v2_leds_gpio),
						f5d8235v2_leds_gpio);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_NONE;
	rt305x_register_ethernet();
	platform_device_register(&f5d8235v2_switch);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_F5D8235_V2, "F5D8235_V2", "Belkin F5D8235 v2",
	     f5d8235v2_init);
