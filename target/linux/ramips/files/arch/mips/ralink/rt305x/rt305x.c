/*
 * Ralink RT305x SoC specific setup
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
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

void __iomem * rt305x_sysc_base;
void __iomem * rt305x_memc_base;

void __init rt305x_detect_sys_type(void)
{
	u32 n0;
	u32 n1;
	u32 id;

	n0 = rt305x_sysc_rr(SYSC_REG_CHIP_NAME0);
	n1 = rt305x_sysc_rr(SYSC_REG_CHIP_NAME1);
	id = rt305x_sysc_rr(SYSC_REG_CHIP_ID);

	snprintf(ramips_sys_type, RAMIPS_SYS_TYPE_LEN,
		"Ralink %c%c%c%c%c%c%c%c id:%u rev:%u",
		(char) (n0 & 0xff), (char) ((n0 >> 8) & 0xff),
		(char) ((n0 >> 16) & 0xff), (char) ((n0 >> 24) & 0xff),
		(char) (n1 & 0xff), (char) ((n1 >> 8) & 0xff),
		(char) ((n1 >> 16) & 0xff), (char) ((n1 >> 24) & 0xff),
		(id >> CHIP_ID_ID_SHIFT) & CHIP_ID_ID_MASK,
		(id & CHIP_ID_REV_MASK));
}

static void rt305x_gpio_reserve(int first, int last)
{
	for (; first <= last; first++)
		gpio_request(first, "reserved");
}

void __init rt305x_gpio_init(u32 mode)
{
	u32 t;

	rt305x_sysc_wr(mode, SYSC_REG_GPIO_MODE);

	ramips_gpio_init();
	if ((mode & RT305X_GPIO_MODE_I2C) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_I2C_SD, RT305X_GPIO_I2C_SCLK);

	if ((mode & RT305X_GPIO_MODE_SPI) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_SPI_EN, RT305X_GPIO_SPI_CLK);

	t = mode >> RT305X_GPIO_MODE_UART0_SHIFT;
	t &= RT305X_GPIO_MODE_UART0_MASK;
	switch (t) {
	case RT305X_GPIO_MODE_UARTF:
	case RT305X_GPIO_MODE_PCM_UARTF:
	case RT305X_GPIO_MODE_PCM_I2S:
	case RT305X_GPIO_MODE_I2S_UARTF:
		rt305x_gpio_reserve(RT305X_GPIO_7, RT305X_GPIO_14);
		break;
	case RT305X_GPIO_MODE_PCM_GPIO:
		rt305x_gpio_reserve(RT305X_GPIO_10, RT305X_GPIO_14);
		break;
	case RT305X_GPIO_MODE_GPIO_UARTF:
	case RT305X_GPIO_MODE_GPIO_I2S:
		rt305x_gpio_reserve(RT305X_GPIO_7, RT305X_GPIO_10);
		break;
	}

	if ((mode & RT305X_GPIO_MODE_UART1) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_UART1_TXD,
				    RT305X_GPIO_UART1_RXD);

	if ((mode & RT305X_GPIO_MODE_JTAG) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_JTAG_TDO, RT305X_GPIO_JTAG_TDI);

	if ((mode & RT305X_GPIO_MODE_MDIO) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_MDIO_MDC,
				    RT305X_GPIO_MDIO_MDIO);

	if ((mode & RT305X_GPIO_MODE_SDRAM) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_SDRAM_MD16,
				    RT305X_GPIO_SDRAM_MD31);

	if ((mode & RT305X_GPIO_MODE_RGMII) == 0)
		rt305x_gpio_reserve(RT305X_GPIO_GE0_TXD0,
				    RT305X_GPIO_GE0_RXCLK);
}
