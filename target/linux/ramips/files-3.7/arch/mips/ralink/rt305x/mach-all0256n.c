/*
 *  Allnet ALL0256N board support
 *
 *  Copyright (C) 2012 Daniel Golle <dgolle@allnet.de>
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

#define ALL0256N_GPIO_BUTTON_RESET 0
#define ALL0256N_GPIO_LED_RSSI_LOW 14
#define ALL0256N_GPIO_LED_RSSI_MED 12
#define ALL0256N_GPIO_LED_RSSI_HIGH 13
#define ALL0256N_KEYS_POLL_INTERVAL 20
#define ALL0256N_KEYS_DEBOUNCE_INTERVAL	(3 * ALL0256N_KEYS_POLL_INTERVAL)

const struct flash_platform_data all0256n_flash = {
	.type		= "mx25l3205d",
};

struct spi_board_info all0256n_spi_slave_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &all0256n_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

static struct gpio_keys_button all0256n_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = ALL0256N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= ALL0256N_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static struct gpio_led all0256n_leds_gpio[] __initdata = {
	{
		.name		= "all0256n:green:rssilow",
		.gpio		= ALL0256N_GPIO_LED_RSSI_LOW,
		.active_low	= 1,
	}, {
		.name		= "all0256n:green:rssimed",
		.gpio		= ALL0256N_GPIO_LED_RSSI_MED,
		.active_low	= 1,
	}, {
		.name		= "all0256n:green:rssihigh",
		.gpio		= ALL0256N_GPIO_LED_RSSI_HIGH,
		.active_low	= 1,
	}
};

static void __init all0256n_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_spi(all0256n_spi_slave_info,
			    ARRAY_SIZE(all0256n_spi_slave_info));
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_NONE;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(all0256n_leds_gpio),
				  all0256n_leds_gpio);
	ramips_register_gpio_buttons(-1, ALL0256N_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(all0256n_gpio_buttons),
				     all0256n_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_ALL0256N, "ALL0256N", "Allnet ALL0256N",
	     all0256n_init);
