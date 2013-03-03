/*
 *  Allnet ALL5002
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
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

// #define ALL5002_GPIO_BUTTON_RESET 0
// #define ALL5002_GPIO_LED_RSSI_LOW 14
// #define ALL5002_GPIO_LED_RSSI_MED 12
// #define ALL5002_GPIO_LED_RSSI_HIGH 13
// #define ALL5002_BUTTONS_POLL_INTERVAL 20

const struct flash_platform_data all5002_flash = {
	.type		= "mx25l25635e",
};

struct spi_board_info all5002_spi_slave_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &all5002_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

static void __init all5002_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);
	rt305x_register_spi(all5002_spi_slave_info,
			    ARRAY_SIZE(all5002_spi_slave_info));
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_NONE;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_ALL5002, "ALL5002", "Allnet ALL5002/ALL5003",
	     all5002_init);
