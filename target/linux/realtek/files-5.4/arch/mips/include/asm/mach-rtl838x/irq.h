// SPDX-License-Identifier: GPL-2.0-only

#ifndef _RTL83XX_IRQ_H_
#define _RTL83XX_IRQ_H_

#define NR_IRQS 32
#include_next <irq.h>

/* Global Interrupt Mask Register */
#define RTL83XX_ICTL_GIMR	0x00
/* Global Interrupt Status Register */
#define RTL83XX_ICTL_GISR	0x04

#define RTL83XX_IRQ_CPU_BASE	0
#define RTL83XX_IRQ_CPU_NUM	8
#define RTL83XX_IRQ_ICTL_BASE	(RTL83XX_IRQ_CPU_BASE + RTL83XX_IRQ_CPU_NUM)
#define RTL83XX_IRQ_ICTL_NUM	32

/* Cascaded interrupts */
#define RTL83XX_ICTL1_IRQ	(RTL83XX_IRQ_CPU_BASE + 2)
#define RTL83XX_ICTL2_IRQ	(RTL83XX_IRQ_CPU_BASE + 3)
#define RTL83XX_ICTL3_IRQ	(RTL83XX_IRQ_CPU_BASE + 4)
#define RTL83XX_ICTL4_IRQ	(RTL83XX_IRQ_CPU_BASE + 5)
#define RTL83XX_ICTL5_IRQ	(RTL83XX_IRQ_CPU_BASE + 6)

/* Interrupt routing register */
#define RTL83XX_IRR0		0x08
#define RTL83XX_IRR1		0x0c
#define RTL83XX_IRR2		0x10
#define RTL83XX_IRR3		0x14

/* Cascade map */
#define UART0_CASCADE		2
#define UART1_CASCADE		1
#define TC0_CASCADE		5
#define TC1_CASCADE		1
#define OCPTO_CASCADE		1
#define HLXTO_CASCADE		1
#define SLXTO_CASCADE		1
#define NIC_CASCADE		4
#define GPIO_ABCD_CASCADE	4
#define GPIO_EFGH_CASCADE	4
#define RTC_CASCADE		4
#define	SWCORE_CASCADE		3
#define WDT_IP1_CASCADE		4
#define WDT_IP2_CASCADE		5

/* Pack cascade map into interrupt routing registers */
#define RTL83XX_IRR0_SETTING (\
	(UART0_CASCADE		<< 28) | \
	(UART1_CASCADE		<< 24) | \
	(TC0_CASCADE		<< 20) | \
	(TC1_CASCADE		<< 16) | \
	(OCPTO_CASCADE		<< 12) | \
	(HLXTO_CASCADE		<< 8)  | \
	(SLXTO_CASCADE		<< 4)  | \
	(NIC_CASCADE		<< 0))
#define RTL83XX_IRR1_SETTING (\
	(GPIO_ABCD_CASCADE	<< 28) | \
	(GPIO_EFGH_CASCADE	<< 24) | \
	(RTC_CASCADE		<< 20) | \
	(SWCORE_CASCADE		<< 16))
#define RTL83XX_IRR2_SETTING	0
#define RTL83XX_IRR3_SETTING	0

#endif /* _RTL83XX_IRQ_H_ */
