/*
 * Ralink RT288x SoC specific setup
 *
 * Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
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

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/ramips_gpio.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

void __iomem * rt288x_sysc_base;
void __iomem * rt288x_memc_base;

void __init ramips_soc_prom_init(void)
{
	void __iomem *sysc = (void __iomem *) KSEG1ADDR(RT2880_SYSC_BASE);
	u32 n0;
	u32 n1;
	u32 id;

	n0 = __raw_readl(sysc + SYSC_REG_CHIP_NAME0);
	n1 = __raw_readl(sysc + SYSC_REG_CHIP_NAME1);
	id = __raw_readl(sysc + SYSC_REG_CHIP_ID);

	snprintf(ramips_sys_type, RAMIPS_SYS_TYPE_LEN,
		"Ralink %c%c%c%c%c%c%c%c id:%u rev:%u",
		(char) (n0 & 0xff), (char) ((n0 >> 8) & 0xff),
		(char) ((n0 >> 16) & 0xff), (char) ((n0 >> 24) & 0xff),
		(char) (n1 & 0xff), (char) ((n1 >> 8) & 0xff),
		(char) ((n1 >> 16) & 0xff), (char) ((n1 >> 24) & 0xff),
		(id >> CHIP_ID_ID_SHIFT) & CHIP_ID_ID_MASK,
		(id & CHIP_ID_REV_MASK));

	ramips_mem_base = RT2880_SDRAM_BASE;
	ramips_mem_size_min = RT288X_MEM_SIZE_MIN;
	ramips_mem_size_max = RT288X_MEM_SIZE_MAX;
}

static struct ramips_gpio_chip rt288x_gpio_chips[] = {
	{
		.chip = {
			.label	= "RT288X-GPIO0",
			.base	= 0,
			.ngpio	= 24,
		},
		.regs = {
			[RAMIPS_GPIO_REG_INT]	= 0x00,
			[RAMIPS_GPIO_REG_EDGE]	= 0x04,
			[RAMIPS_GPIO_REG_RENA]	= 0x08,
			[RAMIPS_GPIO_REG_FENA]	= 0x0c,
			[RAMIPS_GPIO_REG_DATA]	= 0x20,
			[RAMIPS_GPIO_REG_DIR]	= 0x24,
			[RAMIPS_GPIO_REG_POL]	= 0x28,
			[RAMIPS_GPIO_REG_SET]	= 0x2c,
			[RAMIPS_GPIO_REG_RESET]	= 0x30,
			[RAMIPS_GPIO_REG_TOGGLE] = 0x34,
		},
		.map_base = RT2880_PIO_BASE,
		.map_size = RT2880_PIO_SIZE,
	},
	{
		.chip = {
			.label	= "RT288X-GPIO1",
			.base	= 24,
			.ngpio	= 16,
		},
		.regs = {
			[RAMIPS_GPIO_REG_INT]	= 0x38,
			[RAMIPS_GPIO_REG_EDGE]	= 0x3c,
			[RAMIPS_GPIO_REG_RENA]	= 0x40,
			[RAMIPS_GPIO_REG_FENA]	= 0x44,
			[RAMIPS_GPIO_REG_DATA]	= 0x48,
			[RAMIPS_GPIO_REG_DIR]	= 0x4c,
			[RAMIPS_GPIO_REG_POL]	= 0x50,
			[RAMIPS_GPIO_REG_SET]	= 0x54,
			[RAMIPS_GPIO_REG_RESET]	= 0x58,
			[RAMIPS_GPIO_REG_TOGGLE] = 0x5c,
		},
		.map_base = RT2880_PIO_BASE,
		.map_size = RT2880_PIO_SIZE,
	},
	{
		.chip = {
			.label	= "RT288X-GPIO2",
			.base	= 40,
			.ngpio	= 32,
		},
		.regs = {
			[RAMIPS_GPIO_REG_INT]	= 0x60,
			[RAMIPS_GPIO_REG_EDGE]	= 0x64,
			[RAMIPS_GPIO_REG_RENA]	= 0x68,
			[RAMIPS_GPIO_REG_FENA]	= 0x6c,
			[RAMIPS_GPIO_REG_DATA]	= 0x70,
			[RAMIPS_GPIO_REG_DIR]	= 0x74,
			[RAMIPS_GPIO_REG_POL]	= 0x78,
			[RAMIPS_GPIO_REG_SET]	= 0x7c,
			[RAMIPS_GPIO_REG_RESET]	= 0x80,
			[RAMIPS_GPIO_REG_TOGGLE] = 0x84,
		},
		.map_base = RT2880_PIO_BASE,
		.map_size = RT2880_PIO_SIZE,
	},
};

static struct ramips_gpio_data rt288x_gpio_data = {
	.chips = rt288x_gpio_chips,
	.num_chips = ARRAY_SIZE(rt288x_gpio_chips),
};

static void rt288x_gpio_reserve(int first, int last)
{
	for (; first <= last; first++)
		gpio_request(first, "reserved");
}

void __init rt288x_gpio_init(u32 mode)
{
	rt288x_sysc_wr(mode, SYSC_REG_GPIO_MODE);

	ramips_gpio_init(&rt288x_gpio_data);
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
