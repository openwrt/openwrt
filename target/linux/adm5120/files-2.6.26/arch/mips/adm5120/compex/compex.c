/*
 *  Compex boards
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/bootinfo.h>
#include <linux/gpio.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_board.h>
#include <asm/mach-adm5120/adm5120_platform.h>
#include <asm/mach-adm5120/adm5120_irq.h>

#define COMPEX_GPIO_DEV_MASK	(1 << ADM5120_GPIO_PIN5)

static void switch_bank_gpio5(unsigned bank)
{
	switch (bank) {
	case 0:
		gpio_set_value(ADM5120_GPIO_PIN5, 0);
		break;
	case 1:
		gpio_set_value(ADM5120_GPIO_PIN5, 1);
		break;
	}
}

void __init compex_generic_setup(void)
{
	gpio_request(ADM5120_GPIO_PIN5, NULL); /* for flash A20 line */
	gpio_direction_output(ADM5120_GPIO_PIN5, 0);

	adm5120_flash0_data.switch_bank = switch_bank_gpio5;
	adm5120_add_device_flash(0);

	adm5120_add_device_gpio(COMPEX_GPIO_DEV_MASK);
}
