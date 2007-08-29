/*
 *  $Id$
 *
 *  ADM5120 specific IRQ numbers
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
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
 *
 */
#ifndef _ADM5120_IRQ_H_
#define _ADM5120_IRQ_H_

#include <asm/mach-adm5120/adm5120_intc.h>

#define MIPS_IRQ_HW0		2
#define MIPS_IRQ_COUNTER	7
#define MIPS_IRQ_COUNT		8

#define ADM5120_CPU_IRQ_BASE	0
#define ADM5120_INTC_IRQ_BASE	(ADM5120_CPU_IRQ_BASE+MIPS_IRQ_COUNT)
#define ADM5120_SWITCH_IRQ_BASE (ADM5120_INTC_IRQ_BASE+INTC_IRQ_COUNT)

#define ADM5120_CPU_IRQ(x)	(ADM5120_CPU_IRQ_BASE + (x))
#define ADM5120_INTC_IRQ(x)	(ADM5120_INTC_IRQ_BASE + (x))

#define ADM5120_IRQ_INTC	ADM5120_CPU_IRQ(MIPS_IRQ_HW0)
#define ADM5120_IRQ_COUNTER	ADM5120_CPU_IRQ(MIPS_IRQ_COUNTER)

#define ADM5120_IRQ_TIMER	ADM5120_INTC_IRQ(INTC_IRQ_TIMER)
#define ADM5120_IRQ_UART0	ADM5120_INTC_IRQ(INTC_IRQ_UART0)
#define ADM5120_IRQ_UART1	ADM5120_INTC_IRQ(INTC_IRQ_UART1)
#define ADM5120_IRQ_USBC	ADM5120_INTC_IRQ(INTC_IRQ_USBC)
#define ADM5120_IRQ_GPIO2	ADM5120_INTC_IRQ(INTC_IRQ_GPIO2)
#define ADM5120_IRQ_GPIO4	ADM5120_INTC_IRQ(INTC_IRQ_GPIO4)
#define ADM5120_IRQ_PCI0	ADM5120_INTC_IRQ(INTC_IRQ_PCI0)
#define ADM5120_IRQ_PCI1	ADM5120_INTC_IRQ(INTC_IRQ_PCI1)
#define ADM5120_IRQ_PCI2	ADM5120_INTC_IRQ(INTC_IRQ_PCI2)
#define ADM5120_IRQ_SWITCH	ADM5120_INTC_IRQ(INTC_IRQ_SWITCH)

#endif
