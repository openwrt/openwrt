/*
 *  D-Link DIR-615 H1
 *
 *  Copyright (C) 2012 Mikko Hissa <mikko.hissa@uta.fi>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define DIR_615_H1_GPIO_LED_WAN_AMBER	12 /* active low */
#define DIR_615_H1_GPIO_LED_WAN_GREEN	13 /* active low */
#define DIR_615_H1_GPIO_LED_WPS_BLUE	14 /* active low */

#define DIR_615_H1_GPIO_LED_STATUS_AMBER 7
#define DIR_615_H1_GPIO_LED_STATUS_GREEN 9

#define DIR_615_H1_GPIO_BUTTON_RESET	10 /* active low */
#define DIR_615_H1_GPIO_BUTTON_WPS	0  /* active low */

#define DIR_615_H1_KEYS_POLL_INTERVAL	20
#define DIR_615_H1_KEYS_DEBOUNCE_INTERVAL (3 * DIR_615_H1_KEYS_POLL_INTERVAL)

static struct gpio_led dir_615_h1_leds_gpio[] __initdata = {
	{
		.name		= "d-link:amber:status",
		.gpio		= DIR_615_H1_GPIO_LED_STATUS_AMBER,
	}, {
		.name		= "d-link:green:status",
		.gpio		= DIR_615_H1_GPIO_LED_STATUS_GREEN,
	}, {
		.name		= "d-link:amber:wan",
		.gpio		= DIR_615_H1_GPIO_LED_WAN_AMBER,
		.active_low	= 1,
	}, {
		.name		= "d-link:green:wan",
		.gpio		= DIR_615_H1_GPIO_LED_WAN_GREEN,
		.active_low	= 1,
	}, {
		.name		= "d-link:blue:wps",
		.gpio		= DIR_615_H1_GPIO_LED_WPS_BLUE,
		.active_low	= 1,
	}
};

static struct gpio_keys_button dir_615_h1_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DIR_615_H1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_615_H1_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DIR_615_H1_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DIR_615_H1_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

const struct flash_platform_data dir615h1_flash = {
	.type		= "mx25l3205d",
};

struct spi_board_info dir615h1_spi_slave_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &dir615h1_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

static void __init dir615h1_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_spi(dir615h1_spi_slave_info,
			    ARRAY_SIZE(dir615h1_spi_slave_info));
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(dir_615_h1_leds_gpio),
				  dir_615_h1_leds_gpio);
	ramips_register_gpio_buttons(-1, DIR_615_H1_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(dir_615_h1_gpio_buttons),
				     dir_615_h1_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_DIR_615_H1, "DIR-615-H1", "D-Link DIR-615 H1",
	     dir615h1_init);
