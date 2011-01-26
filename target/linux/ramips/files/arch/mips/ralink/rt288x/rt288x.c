/*
 * Ralink RT288x SoC specific setup
 *
 * Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

void __iomem * rt288x_sysc_base;
void __iomem * rt288x_memc_base;

void __init rt288x_detect_sys_type(void)
{
	u32 n0;
	u32 n1;
	u32 id;

	n0 = rt288x_sysc_rr(SYSC_REG_CHIP_NAME0);
	n1 = rt288x_sysc_rr(SYSC_REG_CHIP_NAME1);
	id = rt288x_sysc_rr(SYSC_REG_CHIP_ID);

	snprintf(ramips_sys_type, RAMIPS_SYS_TYPE_LEN,
		"Ralink %c%c%c%c%c%c%c%c id:%u rev:%u",
		(char) (n0 & 0xff), (char) ((n0 >> 8) & 0xff),
		(char) ((n0 >> 16) & 0xff), (char) ((n0 >> 24) & 0xff),
		(char) (n1 & 0xff), (char) ((n1 >> 8) & 0xff),
		(char) ((n1 >> 16) & 0xff), (char) ((n1 >> 24) & 0xff),
		(id >> CHIP_ID_ID_SHIFT) & CHIP_ID_ID_MASK,
		(id & CHIP_ID_REV_MASK));
}

static void rt288x_gpio_reserve(int first, int last)
{
	for (; first <= last; first++)
		gpio_request(first, "reserved");
}

void __init rt288x_gpio_init(u32 mode)
{
	rt288x_sysc_wr(mode, SYSC_REG_GPIO_MODE);

	ramips_gpio_init();
	if ((mode & RT2880_GPIO_MODE_I2C) == 0)
		rt288x_gpio_reserve(1, 2);

	if ((mode & RT2880_GPIO_MODE_SPI) == 0)
		rt288x_gpio_reserve(3, 6);

	if ((mode & RT2880_GPIO_MODE_UART0) == 0)
		rt288x_gpio_reserve(7, 14);

	if ((mode & RT2880_GPIO_MODE_JTAG) == 0)
		rt288x_gpio_reserve(17, 21);

	if ((mode & RT2880_GPIO_MODE_MDIO) == 0)
		rt288x_gpio_reserve(22, 23);

	if ((mode & RT2880_GPIO_MODE_SDRAM) == 0)
		rt288x_gpio_reserve(24, 39);

	if ((mode & RT2880_GPIO_MODE_PCI) == 0)
		rt288x_gpio_reserve(40, 71);
}
