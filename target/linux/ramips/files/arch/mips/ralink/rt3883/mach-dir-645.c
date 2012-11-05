/*
 *  D-Link DIR-645 board support
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/rtl8367.h>
#include <linux/ethtool.h>
#include <linux/rt2x00_platform.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define DIR_645_GPIO_LED_INET		0
#define DIR_645_GPIO_LED_WPS		26

#define DIR_645_GPIO_BUTTON_RESET	9
#define DIR_645_GPIO_BUTTON_WPS		14

#define DIR_645_GPIO_USB_POWER		30

#define DIR_645_GPIO_RTL8367_SCK	2
#define DIR_645_GPIO_RTL8367_SDA	1

#define DIR_645_KEYS_POLL_INTERVAL	20
#define DIR_645_KEYS_DEBOUNCE_INTERVAL	(3 * DIR_645_KEYS_POLL_INTERVAL)

static struct gpio_led dir_645_leds_gpio[] __initdata = {
	{
		.name		= "d-link:green:inet",
		.gpio		= DIR_645_GPIO_LED_INET,
		.active_low	= 1,
	},
	{
		.name		= "d-link:green:wps",
		.gpio		= DIR_645_GPIO_LED_WPS,
		.active_low	= 1,
	},
};

static struct gpio_keys_button dir_645_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DIR_645_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_645_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DIR_645_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_645_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static struct rtl8367_extif_config dir_645_rtl8367_extif1_cfg = {
	.txdelay = 1,
	.rxdelay = 0,
	.mode = RTL8367_EXTIF_MODE_RGMII,
	.ability = {
		.force_mode = 1,
		.txpause = 1,
		.rxpause = 1,
		.link = 1,
		.duplex = 1,
		.speed = RTL8367_PORT_SPEED_1000,
	}
};

static struct rtl8367_platform_data dir_645_rtl8367_data = {
	.gpio_sda	= DIR_645_GPIO_RTL8367_SDA,
	.gpio_sck	= DIR_645_GPIO_RTL8367_SCK,
	.extif1_cfg	= &dir_645_rtl8367_extif1_cfg,
};

static struct platform_device dir_645_rtl8367_device = {
	.name		= RTL8367B_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &dir_645_rtl8367_data,
	}
};

static struct spi_board_info dir_645_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};

static void __init dir_645_gpio_init(void)
{
	rt3883_gpio_init(RT3883_GPIO_MODE_I2C |
			 RT3883_GPIO_MODE_UART0(RT3883_GPIO_MODE_GPIO) |
			 RT3883_GPIO_MODE_JTAG |
			 RT3883_GPIO_MODE_PCI(RT3883_GPIO_MODE_PCI_FNC));

	gpio_request_one(DIR_645_GPIO_USB_POWER,
			 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			 "USB power");
}

static void __init dir_645_init(void)
{
	dir_645_gpio_init();

	rt3883_register_spi(dir_645_spi_info,
			    ARRAY_SIZE(dir_645_spi_info));

	ramips_register_gpio_leds(-1, ARRAY_SIZE(dir_645_leds_gpio),
				  dir_645_leds_gpio);

	ramips_register_gpio_buttons(-1, DIR_645_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(dir_645_gpio_buttons),
				     dir_645_gpio_buttons);

	platform_device_register(&dir_645_rtl8367_device);

	rt3883_wlan_data.disable_5ghz = 1;
	rt3883_register_wlan();

	rt3883_eth_data.speed = SPEED_1000;
	rt3883_eth_data.duplex = DUPLEX_FULL;
	rt3883_eth_data.tx_fc = 1;
	rt3883_eth_data.rx_fc = 1;
	rt3883_register_ethernet();

	rt3883_register_wdt(false);
	rt3883_register_usbhost();
}

MIPS_MACHINE(RAMIPS_MACH_DIR_645, "DIR-645", "D-Link DIR-645", dir_645_init);
