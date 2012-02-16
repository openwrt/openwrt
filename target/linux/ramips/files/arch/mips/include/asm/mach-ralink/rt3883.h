/*
 * Ralink RT3662/RT3883 SoC specific definitions
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Parts of this file are based on Ralink's 2.6.21 BSP
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _RT3883_H_
#define _RT3883_H_

#include <linux/init.h>
#include <linux/io.h>

void rt3883_detect_sys_type(void);

#define RT3883_MEM_SIZE_MIN (2 * 1024 * 1024)
#define RT3883_MEM_SIZE_MAX (256 * 1024 * 1024)

#define RT3883_CPU_IRQ_BASE	0
#define RT3883_CPU_IRQ_COUNT	8
#define RT3883_INTC_IRQ_BASE	(RT3883_CPU_IRQ_BASE + RT3883_CPU_IRQ_COUNT)
#define RT3883_INTC_IRQ_COUNT	32
#define RT3883_GPIO_IRQ_BASE	(RT3883_INTC_IRQ_BASE + RT3883_INTC_IRQ_COUNT)
#define RT3883_GPIO_IRQ_COUNT	96
#define RT3883_PCI_IRQ_BASE	(RT3883_GPIO_IRQ_BASE + RT3883_GPIO_IRQ_COUNT)
#define RT3883_PCI_IRQ_COUNT	3

#define RT3883_CPU_IRQ_INTC	(RT3883_CPU_IRQ_BASE + 2)
#define RT3883_CPU_IRQ_PCI	(RT3883_CPU_IRQ_BASE + 4)
#define RT3883_CPU_IRQ_FE	(RT3883_CPU_IRQ_BASE + 5)
#define RT3883_CPU_IRQ_WLAN	(RT3883_CPU_IRQ_BASE + 6)
#define RT3883_CPU_IRQ_COUNTER	(RT3883_CPU_IRQ_BASE + 7)

#define RT3883_INTC_IRQ_SYSCTL	(RT3883_INTC_IRQ_BASE + 0)
#define RT3883_INTC_IRQ_TIMER0	(RT3883_INTC_IRQ_BASE + 1)
#define RT3883_INTC_IRQ_TIMER1	(RT3883_INTC_IRQ_BASE + 2)
#define RT3883_INTC_IRQ_IA	(RT3883_INTC_IRQ_BASE + 3)
#define RT3883_INTC_IRQ_PCM	(RT3883_INTC_IRQ_BASE + 4)
#define RT3883_INTC_IRQ_UART0	(RT3883_INTC_IRQ_BASE + 5)
#define RT3883_INTC_IRQ_PIO	(RT3883_INTC_IRQ_BASE + 6)
#define RT3883_INTC_IRQ_DMA	(RT3883_INTC_IRQ_BASE + 7)
#define RT3883_INTC_IRQ_NAND	(RT3883_INTC_IRQ_BASE + 8)
#define RT3883_INTC_IRQ_PERFC	(RT3883_INTC_IRQ_BASE + 9)
#define RT3883_INTC_IRQ_I2S	(RT3883_INTC_IRQ_BASE + 10)
#define RT3883_INTC_IRQ_UART1	(RT3883_INTC_IRQ_BASE + 12)
#define RT3883_INTC_IRQ_UHST	(RT3883_INTC_IRQ_BASE + 18)
#define RT3883_INTC_IRQ_UDEV	(RT3883_INTC_IRQ_BASE + 19)

#define RT3883_PCI_IRQ_PCI0	(RT3883_PCI_IRQ_BASE + 0)
#define RT3883_PCI_IRQ_PCI1	(RT3883_PCI_IRQ_BASE + 1)
#define RT3883_PCI_IRQ_PCIE	(RT3883_PCI_IRQ_BASE + 2)

extern void __iomem *rt3883_sysc_base;
extern void __iomem *rt3883_memc_base;

static inline void rt3883_sysc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt3883_sysc_base + reg);
}

static inline u32 rt3883_sysc_rr(unsigned reg)
{
	return __raw_readl(rt3883_sysc_base + reg);
}

static inline void rt3883_memc_wr(u32 val, unsigned reg)
{
	__raw_writel(val, rt3883_memc_base + reg);
}

static inline u32 rt3883_memc_rr(unsigned reg)
{
	return __raw_readl(rt3883_memc_base + reg);
}

#define RT3883_GPIO_I2C_SD	1
#define RT3883_GPIO_I2C_SCLK	2
#define RT3883_GPIO_SPI_CS0	3
#define RT3883_GPIO_SPI_CLK	4
#define RT3883_GPIO_SPI_MOSI	5
#define RT3883_GPIO_SPI_MISO	6
/* GPIO 7-14 is shared between UART0, PCM  and I2S interfaces */
#define RT3883_GPIO_7		7
#define RT3883_GPIO_8		8
#define RT3883_GPIO_9		9
#define RT3883_GPIO_10		10
#define RT3883_GPIO_11		11
#define RT3883_GPIO_12		12
#define RT3883_GPIO_13		13
#define RT3883_GPIO_14		14
#define RT3883_GPIO_UART1_TXD	15
#define RT3883_GPIO_UART1_RXD	16
#define RT3883_GPIO_JTAG_TDO	17
#define RT3883_GPIO_JTAG_TDI	18
#define RT3883_GPIO_JTAG_TMS	19
#define RT3883_GPIO_JTAG_TCLK	20
#define RT3883_GPIO_JTAG_TRST_N	21
#define RT3883_GPIO_MDIO_MDC	22
#define RT3883_GPIO_MDIO_MDIO	23
#define RT3883_GPIO_LNA_PE_A0	32
#define RT3883_GPIO_LNA_PE_A1	33
#define RT3883_GPIO_LNA_PE_A2	34
#define RT3883_GPIO_LNA_PE_G0	35
#define RT3883_GPIO_LNA_PE_G1	36
#define RT3883_GPIO_LNA_PE_G2	37
#define RT3883_GPIO_PCI_AD0	40
#define RT3883_GPIO_PCI_AD31	71
#define RT3883_GPIO_GE2_TXD0	72
#define RT3883_GPIO_GE2_TXD1	73
#define RT3883_GPIO_GE2_TXD2	74
#define RT3883_GPIO_GE2_TXD3	75
#define RT3883_GPIO_GE2_TXEN	76
#define RT3883_GPIO_GE2_TXCLK	77
#define RT3883_GPIO_GE2_RXD0	78
#define RT3883_GPIO_GE2_RXD1	79
#define RT3883_GPIO_GE2_RXD2	80
#define RT3883_GPIO_GE2_RXD3	81
#define RT3883_GPIO_GE2_RXDV	82
#define RT3883_GPIO_GE2_RXCLK	83
#define RT3883_GPIO_GE1_TXD0	84
#define RT3883_GPIO_GE1_TXD1	85
#define RT3883_GPIO_GE1_TXD2	86
#define RT3883_GPIO_GE1_TXD3	87
#define RT3883_GPIO_GE1_TXEN	88
#define RT3883_GPIO_GE1_TXCLK	89
#define RT3883_GPIO_GE1_RXD0	90
#define RT3883_GPIO_GE1_RXD1	91
#define RT3883_GPIO_GE1_RXD2	92
#define RT3883_GPIO_GE1_RXD3	93
#define RT3883_GPIO_GE1_RXDV	94
#define RT3883_GPIO_GE1_RXCLK	95

void rt3883_gpio_init(u32 mode);

#define RT3883_PCI_MODE_PCI	0x01
#define RT3883_PCI_MODE_PCIE	0x02
#define RT3883_PCI_MODE_BOTH	(RT3883_PCI_MODE_PCI | RT3883_PCI_MODE_PCIE)

struct pci_dev;

#ifdef CONFIG_PCI
void rt3883_pci_init(unsigned mode);
void rt3883_pci_set_plat_dev_init(int (*f)(struct pci_dev *));
#else
static inline void rt3883_pci_init(unsigned mode) {}
static inline void rt3883_pci_set_plat_dev_init(int (*f)(struct pci_dev *)) {}
#endif /* CONFIG_PCI */

#endif /* _RT3883_H_ */
