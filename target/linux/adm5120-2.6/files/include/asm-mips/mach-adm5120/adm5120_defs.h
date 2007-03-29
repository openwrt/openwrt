/*
 *  $Id$
 *
 *  ADM5120 SoC definitions
 *
 *  This file defines some constants specific to the ADM5120 SoC
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */
#ifndef _ADM5120_DEFS_H
#define _ADM5120_DEFS_H

#define ADM5120_SDRAM0_BASE	0x00000000
#define ADM5120_SDRAM1_BASE	0x01000000
#define ADM5120_SRAM1_BASE	0x10000000
#define ADM5120_MPMC_BASE	0x11000000
#define ADM5120_USBC_BASE	0x11200000
#define ADM5120_PCIMEM_BASE	0x11400000
#define ADM5120_PCIIO_BASE	0x11500000
#define ADM5120_PCICFG_ADDR	0x115FFFF0
#define ADM5120_PCICFG_DATA	0x115FFFF8
#define ADM5120_SWITCH_BASE	0x12000000
#define ADM5120_INTC_BASE	0x12200000
#define ADM5120_UART0_BASE	0x12600000
#define ADM5120_UART1_BASE	0x12800000
#define ADM5120_SRAM0_BASE	0x1FC00000

#define ADM5120_MPMC_SIZE	0x1000
#define ADM5120_USBC_SIZE	0x84
#define ADM5120_PCIMEM_SIZE	(ADM5120_PCIIO_BASE - ADM5120_PCIMEM_BASE)
#define ADM5120_PCIIO_SIZE	(ADM5120_PCICFG_ADDR - ADM5120_PCIIO_BASE)
#define ADM5120_PCICFG_SIZE	0x10
#define ADM5120_SWITCH_SIZE	0x114
#define ADM5120_INTC_SIZE	0x28
#define ADM5120_UART_SIZE	0x20

#define ADM5120_CLK_175		175000000
#define ADM5120_CLK_200		200000000
#define ADM5120_CLK_225		225000000
#define ADM5120_CLK_250		250000000

#define ADM5120_UART_CLOCK	62500000

#endif /* _ADM5120_DEFS_H */
