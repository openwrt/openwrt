/*
 * Ralink RT305x SoC specific definitions
 *
 * Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RT305X_H_
#define _RT305X_H_

#include <linux/init.h>
#include <linux/io.h>

enum rt305x_soc_type {
	RT305X_SOC_UNKNOWN = 0,
	RT305X_SOC_RT3050,
	RT305X_SOC_RT3052,
	RT305X_SOC_RT3352,
};

extern enum rt305x_soc_type rt305x_soc;

static inline int soc_is_rt3050(void)
{
	return rt305x_soc == RT305X_SOC_RT3050;
}

static inline int soc_is_rt3052(void)
{
	return rt305x_soc == RT305X_SOC_RT3052;
}

static inline int soc_is_rt305x(void)
{
	return soc_is_rt3050() || soc_is_rt3052();
}

static inline int soc_is_rt3352(void)
{
	return rt305x_soc == RT305X_SOC_RT3352;
}

#define RT305X_MEM_SIZE_MIN (2 * 1024 * 1024)
#define RT305X_MEM_SIZE_MAX (64 * 1024 * 1024)

#define RT305X_CPU_IRQ_BASE	0
#define RT305X_INTC_IRQ_BASE	8
#define RT305X_INTC_IRQ_COUNT	32
#define RT305X_GPIO_IRQ_BASE	40

#define RT305X_CPU_IRQ_INTC	(RT305X_CPU_IRQ_BASE + 2)
#define RT305X_CPU_IRQ_FE	(RT305X_CPU_IRQ_BASE + 5)
#define RT305X_CPU_IRQ_WNIC	(RT305X_CPU_IRQ_BASE + 6)
#define RT305X_CPU_IRQ_COUNTER	(RT305X_CPU_IRQ_BASE + 7)

#define RT305X_INTC_IRQ_SYSCTL	(RT305X_INTC_IRQ_BASE + 0)
#define RT305X_INTC_IRQ_TIMER0	(RT305X_INTC_IRQ_BASE + 1)
#define RT305X_INTC_IRQ_TIMER1	(RT305X_INTC_IRQ_BASE + 2)
#define RT305X_INTC_IRQ_IA	(RT305X_INTC_IRQ_BASE + 3)
#define RT305X_INTC_IRQ_PCM	(RT305X_INTC_IRQ_BASE + 4)
#define RT305X_INTC_IRQ_UART0	(RT305X_INTC_IRQ_BASE + 5)
#define RT305X_INTC_IRQ_PIO	(RT305X_INTC_IRQ_BASE + 6)
#define RT305X_INTC_IRQ_DMA	(RT305X_INTC_IRQ_BASE + 7)
#define RT305X_INTC_IRQ_NAND	(RT305X_INTC_IRQ_BASE + 8)
#define RT305X_INTC_IRQ_PERFC	(RT305X_INTC_IRQ_BASE + 9)
#define RT305X_INTC_IRQ_I2S	(RT305X_INTC_IRQ_BASE + 10)
#define RT305X_INTC_IRQ_UART1	(RT305X_INTC_IRQ_BASE + 12)
#define RT305X_INTC_IRQ_ESW	(RT305X_INTC_IRQ_BASE + 17)
#define RT305X_INTC_IRQ_OTG	(RT305X_INTC_IRQ_BASE + 18)

extern void __iomem *rt305x_sysc_base;
extern void __iomem *rt305x_memc_base;

static inline void rt305x_sysc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt305x_sysc_base + reg);
}

static inline u32 rt305x_sysc_rr(unsigned reg)
{
	return __raw_readl(rt305x_sysc_base + reg);
}

static inline void rt305x_memc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt305x_memc_base + reg);
}

static inline u32 rt305x_memc_rr(unsigned reg)
{
	return __raw_readl(rt305x_memc_base + reg);
}

#define RT305X_GPIO_I2C_SD	1
#define RT305X_GPIO_I2C_SCLK	2
#define RT305X_GPIO_SPI_EN	3
#define RT305X_GPIO_SPI_CLK	4
#define RT305X_GPIO_SPI_DOUT	5
#define RT305X_GPIO_SPI_DIN	6
/* GPIO 7-14 is shared between UART0, PCM  and I2S interfaces */
#define RT305X_GPIO_7		7
#define RT305X_GPIO_8		8
#define RT305X_GPIO_9		9
#define RT305X_GPIO_10		10
#define RT305X_GPIO_11		11
#define RT305X_GPIO_12		12
#define RT305X_GPIO_13		13
#define RT305X_GPIO_14		14
#define RT305X_GPIO_UART1_TXD	15
#define RT305X_GPIO_UART1_RXD	16
#define RT305X_GPIO_JTAG_TDO	17
#define RT305X_GPIO_JTAG_TDI	18
#define RT305X_GPIO_JTAG_TMS	19
#define RT305X_GPIO_JTAG_TCLK	20
#define RT305X_GPIO_JTAG_TRST_N	21
#define RT305X_GPIO_MDIO_MDC	22
#define RT305X_GPIO_MDIO_MDIO	23
#define RT305X_GPIO_SDRAM_MD16	24
#define RT305X_GPIO_SDRAM_MD17	25
#define RT305X_GPIO_SDRAM_MD18	26
#define RT305X_GPIO_SDRAM_MD19	27
#define RT305X_GPIO_SDRAM_MD20	28
#define RT305X_GPIO_SDRAM_MD21	29
#define RT305X_GPIO_SDRAM_MD22	30
#define RT305X_GPIO_SDRAM_MD23	31
#define RT305X_GPIO_SDRAM_MD24	32
#define RT305X_GPIO_SDRAM_MD25	33
#define RT305X_GPIO_SDRAM_MD26	34
#define RT305X_GPIO_SDRAM_MD27	35
#define RT305X_GPIO_SDRAM_MD28	36
#define RT305X_GPIO_SDRAM_MD29	37
#define RT305X_GPIO_SDRAM_MD30	38
#define RT305X_GPIO_SDRAM_MD31	39
#define RT305X_GPIO_GE0_TXD0	40
#define RT305X_GPIO_GE0_TXD1	41
#define RT305X_GPIO_GE0_TXD2	42
#define RT305X_GPIO_GE0_TXD3	43
#define RT305X_GPIO_GE0_TXEN	44
#define RT305X_GPIO_GE0_TXCLK	45
#define RT305X_GPIO_GE0_RXD0	46
#define RT305X_GPIO_GE0_RXD1	47
#define RT305X_GPIO_GE0_RXD2	48
#define RT305X_GPIO_GE0_RXD3	49
#define RT305X_GPIO_GE0_RXDV	50
#define RT305X_GPIO_GE0_RXCLK	51

void rt305x_gpio_init(u32 mode);

#endif /* _RT305X_H_ */
