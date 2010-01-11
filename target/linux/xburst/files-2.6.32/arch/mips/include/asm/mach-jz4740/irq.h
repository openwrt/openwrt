/*
 *  Copyright (C) 2009-2010, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ7420/JZ4740 IRQ definitions
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __ASM_MACH_JZ4740_IRQ_H__
#define __ASM_MACH_JZ4740_IRQ_H__

#define MIPS_CPU_IRQ_BASE 0
#define JZ_IRQ_BASE 8

/* 1st-level interrupts */
#define JZ_IRQ(x)	(JZ_IRQ_BASE + (x))
#define JZ_IRQ_I2C	JZ_IRQ(1)
#define JZ_IRQ_UHC	JZ_IRQ(3)
#define JZ_IRQ_UART1	JZ_IRQ(8)
#define JZ_IRQ_UART0	JZ_IRQ(9)
#define JZ_IRQ_SADC	JZ_IRQ(12)
#define JZ_IRQ_MSC	JZ_IRQ(14)
#define JZ_IRQ_RTC	JZ_IRQ(15)
#define JZ_IRQ_SSI	JZ_IRQ(16)
#define JZ_IRQ_CIM	JZ_IRQ(17)
#define JZ_IRQ_AIC	JZ_IRQ(18)
#define JZ_IRQ_ETH	JZ_IRQ(19)
#define JZ_IRQ_DMAC	JZ_IRQ(20)
#define JZ_IRQ_TCU2	JZ_IRQ(21)
#define JZ_IRQ_TCU1	JZ_IRQ(22)
#define JZ_IRQ_TCU0	JZ_IRQ(23)
#define JZ_IRQ_UDC 	JZ_IRQ(24)
#define JZ_IRQ_GPIO3	JZ_IRQ(25)
#define JZ_IRQ_GPIO2	JZ_IRQ(26)
#define JZ_IRQ_GPIO1	JZ_IRQ(27)
#define JZ_IRQ_GPIO0	JZ_IRQ(28)
#define JZ_IRQ_IPU	JZ_IRQ(29)
#define JZ_IRQ_LCD	JZ_IRQ(30)

/* 2nd-level interrupts */
#define JZ_IRQ_DMA(x)	((x) + JZ_IRQ(32))  /* 32 to 37 for DMAC channel 0 to 5 */

#define JZ_IRQ_INTC_GPIO(x)	(JZ_IRQ_GPIO0 - (x))
#define JZ_IRQ_GPIO(x)		(JZ_IRQ(48) + (x))

#define NR_IRQS (JZ_IRQ_GPIO(127) + 1)

#endif
