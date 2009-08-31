/*
 *  Ralink RT288x SoC register definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _RT288X_REGS_H_
#define _RT288X_REGS_H_

#include <linux/bitops.h>

#define RT2880_SYSC_BASE 	0x00300000
#define RT2880_TIMER_BASE	0x00300100
#define RT2880_MEMC_BASE	0x00300300
#define RT2880_UART0_BASE	0x00300500
#define RT2880_PIO_BASE		0x00300600
#define RT2880_I2C_BASE		0x00300900
#define RT2880_SPI_BASE		0x00300b00
#define RT2880_UART1_BASE	0x00300c00
#define RT2880_FE_BASE		0x00310000
#define RT2880_ROM_BASE		0x00400000
#define RT2880_PCI_BASE		0x00500000
#define RT2880_WMAC_BASE	0x00600000
#define RT2880_FLASH1_BASE	0x01000000
#define RT2880_FLASH0_BASE	0x1fc00000
#define RT2880_SDRAM_BASE	0x08000000

#define RT2880_SYSC_SIZE	0x100
#define RT2880_INTC_SIZE	0x100
#define RT2880_MEMC_SIZE	0x100
#define RT2880_UART0_SIZE	0x100
#define RT2880_UART1_SIZE	0x100
#define RT2880_FLASH1_SIZE	(16 * 1024 * 1024)
#define RT2880_FLASH0_SIZE	(4 * 1024 * 1024)

/* SYSC registers */
#define SYSC_REG_CHIP_NAME0	0x000	/* Chip Name 0 */
#define SYSC_REG_CHIP_NAME1	0x004	/* Chip Name 1 */
#define SYSC_REG_CHIP_ID	0x00c	/* Chip Identification */
#define SYSC_REG_SYSTEM_CONFIG	0x010	/* System Configuration */
#define SYSC_REG_RESET_CTRL	0x034	/* Reset Control*/
#define SYSC_REG_RESET_STATUS	0x038	/* Reset Status*/
#define SYSC_REG_IA_ADDRESS	0x310	/* Illegal Access Address */
#define SYSC_REG_IA_TYPE	0x314	/* Illegal Access Type */

#define CHIP_ID_ID_MASK		0xff
#define CHIP_ID_ID_SHIFT	8
#define CHIP_ID_REV_MASK	0xff

#define SYSTEM_CONFIG_CPUCLK_SHIFT	20
#define SYSTEM_CONFIG_CPUCLK_MASK	0x3
#define SYSTEM_CONFIG_CPUCLK_250	0x0
#define SYSTEM_CONFIG_CPUCLK_266	0x1
#define SYSTEM_CONFIG_CPUCLK_280	0x2
#define SYSTEM_CONFIG_CPUCLK_300	0x3

#define RT2880_RESET_SYSTEM	BIT(0)
#define RT2880_RESET_TIMER	BIT(1)
#define RT2880_RESET_INTC	BIT(2)
#define RT2880_RESET_MEMC	BIT(3)
#define RT2880_RESET_CPU	BIT(4)
#define RT2880_RESET_UART0	BIT(5)
#define RT2880_RESET_PIO	BIT(6)
#define RT2880_RESET_I2C	BIT(9)
#define RT2880_RESET_SPI	BIT(11)
#define RT2880_RESET_UART1	BIT(12)
#define RT2880_RESET_PCI	BIT(16)
#define RT2880_RESET_WMAC	BIT(17)
#define RT2880_RESET_FE		BIT(18)
#define RT2880_RESET_PCM	BIT(19)

#define RT2880_INTC_INT_TIMER0	BIT(0)
#define RT2880_INTC_INT_TIMER1	BIT(1)
#define RT2880_INTC_INT_UART0	BIT(2)
#define RT2880_INTC_INT_PIO	BIT(3)
#define RT2880_INTC_INT_PCM	BIT(4)
#define RT2880_INTC_INT_UART1	BIT(8)
#define RT2880_INTC_INT_IA	BIT(23)
#define RT2880_INTC_INT_GLOBAL	BIT(31)

/* MEMC registers */
#define MEMC_REG_SDRAM_CFG0	0x00
#define MEMC_REG_SDRAM_CFG1	0x04
#define MEMC_REG_FLASH_CFG0	0x08
#define MEMC_REG_FLASH_CFG1	0x0c
#define MEMC_REG_IA_ADDR	0x10
#define MEMC_REG_IA_TYPE	0x14

#define FLASH_CFG_WIDTH_SHIFT	26
#define FLASH_CFG_WIDTH_MASK	0x3
#define FLASH_CFG_WIDTH_8BIT	0x0
#define FLASH_CFG_WIDTH_16BIT	0x1
#define FLASH_CFG_WIDTH_32BIT	0x2

/* UART registers */
#define UART_REG_RX	0
#define UART_REG_TX	1
#define UART_REG_IER	2
#define UART_REG_IIR	3
#define UART_REG_FCR	4
#define UART_REG_LCR	5
#define UART_REG_MCR	6
#define UART_REG_LSR	7

#endif /* _RT288X_REGS_H_ */
