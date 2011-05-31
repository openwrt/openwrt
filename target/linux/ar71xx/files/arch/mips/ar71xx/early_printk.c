/*
 *  Atheros AR7xxx/AR9xxx SoC early printk support
 *
 *  Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/errno.h>
#include <linux/io.h>
#include <linux/serial_reg.h>
#include <asm/addrspace.h>

#include <asm/mach-ar71xx/ar71xx.h>

static void __iomem *prom_uart_base;
static void (*_putchar)(unsigned char);

#define UART_READ(r) \
	__raw_readl(prom_uart_base + 4 * (r))

#define UART_WRITE(r, v) \
	__raw_writel((v), prom_uart_base + 4 * (r))

static void prom_putchar_ar71xx(unsigned char ch)
{
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0)
		;
	UART_WRITE(UART_TX, ch);
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0)
		;
}

static void prom_putchar_ar933x(unsigned char ch)
{
	while (((UART_READ(0)) & 0x200) == 0)
		;
	UART_WRITE(0, 0x200 | ch);
	while (((UART_READ(0)) & 0x200) == 0)
		;
}

static int prom_putchar_init(void)
{
	if (_putchar)
		return 0;

	switch(ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		prom_uart_base = (void __iomem *) KSEG1ADDR(AR71XX_UART_BASE);
		_putchar = prom_putchar_ar71xx;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		prom_uart_base = (void __iomem *) KSEG1ADDR(AR933X_UART_BASE);
		_putchar = prom_putchar_ar933x;
		break;

	default:
		return -ENODEV;
	}

	return 0;
}

void prom_putchar(unsigned char ch)
{
	if (prom_putchar_init())
		return;

	_putchar(ch);
}
