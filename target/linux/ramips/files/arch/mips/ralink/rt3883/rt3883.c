/*
 * Ralink RT3662/RT3883 SoC specific setup
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
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
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>

void __iomem * rt3883_sysc_base;
void __iomem * rt3883_memc_base;

void __init ramips_soc_prom_init(void)
{
	void __iomem *sysc = (void __iomem *) KSEG1ADDR(RT3883_SYSC_BASE);
	u32 n0;
	u32 n1;
	u32 id;

	n0 = __raw_readl(sysc + RT3883_SYSC_REG_CHIPID0_3);
	n1 = __raw_readl(sysc + RT3883_SYSC_REG_CHIPID4_7);
	id = __raw_readl(sysc + RT3883_SYSC_REG_REVID);

	snprintf(ramips_sys_type, RAMIPS_SYS_TYPE_LEN,
		"Ralink %c%c%c%c%c%c%c%c ver:%u eco:%u",
		(char) (n0 & 0xff), (char) ((n0 >> 8) & 0xff),
		(char) ((n0 >> 16) & 0xff), (char) ((n0 >> 24) & 0xff),
		(char) (n1 & 0xff), (char) ((n1 >> 8) & 0xff),
		(char) ((n1 >> 16) & 0xff), (char) ((n1 >> 24) & 0xff),
		(id >> RT3883_REVID_VER_ID_SHIFT) & RT3883_REVID_VER_ID_MASK,
		(id & RT3883_REVID_ECO_ID_MASK));

	ramips_mem_base = RT3883_SDRAM_BASE;
	ramips_mem_size_min = RT3883_MEM_SIZE_MIN;
	ramips_mem_size_max = RT3883_MEM_SIZE_MAX;
}

static struct ramips_gpio_chip rt3883_gpio_chips[] = {
	{
		.chip = {
			.label			= "RT3883-GPIO0",
			.base			= 0,
			.ngpio			= 24,
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
		.map_base	= RT3883_PIO_BASE,
		.map_size	= RT3883_PIO_SIZE,
	},
	{
		.chip = {
			.label			= "RT3883-GPIO1",
			.base			= 24,
			.ngpio			= 16,
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
		.map_base	= RT3883_PIO_BASE,
		.map_size	= RT3883_PIO_SIZE,
	},
	{
		.chip = {
			.label			= "RT3883-GPIO2",
			.base			= 40,
			.ngpio			= 32,
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
		.map_base	= RT3883_PIO_BASE,
		.map_size	= RT3883_PIO_SIZE,
	},
	{
		.chip = {
			.label			= "RT3883-GPIO3",
			.base			= 72,
			.ngpio			= 24,
		},
		.regs = {
			[RAMIPS_GPIO_REG_INT]	= 0x88,
			[RAMIPS_GPIO_REG_EDGE]	= 0x8c,
			[RAMIPS_GPIO_REG_RENA]	= 0x90,
			[RAMIPS_GPIO_REG_FENA]	= 0x94,
			[RAMIPS_GPIO_REG_DATA]	= 0x98,
			[RAMIPS_GPIO_REG_DIR]	= 0x9c,
			[RAMIPS_GPIO_REG_POL]	= 0xa0,
			[RAMIPS_GPIO_REG_SET]	= 0xa4,
			[RAMIPS_GPIO_REG_RESET]	= 0xa8,
			[RAMIPS_GPIO_REG_TOGGLE] = 0xac,
		},
		.map_base	= RT3883_PIO_BASE,
		.map_size	= RT3883_PIO_SIZE,
	},
};

static struct ramips_gpio_data rt3883_gpio_data = {
	.chips = rt3883_gpio_chips,
	.num_chips = ARRAY_SIZE(rt3883_gpio_chips),
};

static void rt3883_gpio_reserve(int first, int last)
{
	for (; first <= last; first++)
		gpio_request(first, "reserved");
}

void __init rt3883_gpio_init(u32 mode)
{
	u32 t;

	rt3883_sysc_wr(mode, RT3883_SYSC_REG_GPIO_MODE);

	ramips_gpio_init(&rt3883_gpio_data);
	if ((mode & RT3883_GPIO_MODE_I2C) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_I2C_SD, RT3883_GPIO_I2C_SCLK);

	if ((mode & RT3883_GPIO_MODE_SPI) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_SPI_CS0, RT3883_GPIO_SPI_CLK);

	t = mode >> RT3883_GPIO_MODE_UART0_SHIFT;
	t &= RT3883_GPIO_MODE_UART0_MASK;
	switch (t) {
	case RT3883_GPIO_MODE_UARTF:
	case RT3883_GPIO_MODE_PCM_UARTF:
	case RT3883_GPIO_MODE_PCM_I2S:
	case RT3883_GPIO_MODE_I2S_UARTF:
		rt3883_gpio_reserve(RT3883_GPIO_7, RT3883_GPIO_14);
		break;
	case RT3883_GPIO_MODE_PCM_GPIO:
		rt3883_gpio_reserve(RT3883_GPIO_11, RT3883_GPIO_14);
		break;
	case RT3883_GPIO_MODE_GPIO_UARTF:
	case RT3883_GPIO_MODE_GPIO_I2S:
		rt3883_gpio_reserve(RT3883_GPIO_7, RT3883_GPIO_10);
		break;
	}

	if ((mode & RT3883_GPIO_MODE_UART1) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_UART1_TXD,
				    RT3883_GPIO_UART1_RXD);

	if ((mode & RT3883_GPIO_MODE_JTAG) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_JTAG_TDO,
				    RT3883_GPIO_JTAG_TCLK);

	if ((mode & RT3883_GPIO_MODE_MDIO) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_MDIO_MDC,
				    RT3883_GPIO_MDIO_MDIO);

	if ((mode & RT3883_GPIO_MODE_GE1) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_GE1_TXD0,
				    RT3883_GPIO_GE1_RXCLK);

	if ((mode & RT3883_GPIO_MODE_GE2) == 0)
		rt3883_gpio_reserve(RT3883_GPIO_GE2_TXD0,
				    RT3883_GPIO_GE2_RXCLK);

	t = mode >> RT3883_GPIO_MODE_PCI_SHIFT;
	t &= RT3883_GPIO_MODE_PCI_MASK;
	if (t != RT3883_GPIO_MODE_PCI_GPIO)
		rt3883_gpio_reserve(RT3883_GPIO_PCI_AD0,
				    RT3883_GPIO_PCI_AD31);

	t = mode >> RT3883_GPIO_MODE_LNA_A_SHIFT;
	t &= RT3883_GPIO_MODE_LNA_A_MASK;
	if (t != RT3883_GPIO_MODE_LNA_A_GPIO)
		rt3883_gpio_reserve(RT3883_GPIO_LNA_PE_A0,
				    RT3883_GPIO_LNA_PE_A2);

	t = mode >> RT3883_GPIO_MODE_LNA_G_SHIFT;
	t &= RT3883_GPIO_MODE_LNA_G_MASK;
	if (t != RT3883_GPIO_MODE_LNA_G_GPIO)
		rt3883_gpio_reserve(RT3883_GPIO_LNA_PE_G0,
				    RT3883_GPIO_LNA_PE_G2);
}
