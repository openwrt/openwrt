/*
 *  8devices Carambola dev board support
 *
 *  Copyright (C) 2012 Tobias Diedrich <ranma+openwrt@tdiedrich.de>
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

static void __init carambola_init(void)
{
	/* Ugh, inverted logic...
	 * This actually puts the pins into GPIO mode rather I2C, SPI, ... */
	rt305x_gpio_init(RT305X_GPIO_MODE_UART0(RT305X_GPIO_MODE_GPIO) |
			 RT305X_GPIO_MODE_I2C |
			 RT305X_GPIO_MODE_SPI |
			 RT305X_GPIO_MODE_JTAG |
			 RT305X_GPIO_MODE_MDIO);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_NONE;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_CARAMBOLA, "CARAMBOLA", "8devices Carambola",
	     carambola_init);
