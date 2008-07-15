/*
 ***************************************************************************
 * Definitions for IDT RC323434 CPU.
 *
 ****************************************************************************
 * Kiran Rao
 * 
 * Original form
 ****************************************************************************
 * P. Sadik   Oct 08, 2003
 *
 * Started revision history
 * Made IDT_BUS_FREQ a kernel configuration parameter
 ****************************************************************************
 * P. Sadik   Oct 10, 2003
 *
 * Removed IDT_BUS_FREQ, since this parameter is no longer required. Instead
 * idt_cpu_freq is used everywhere
 ****************************************************************************
 * P. Sadik   Oct 20, 2003
 *
 * Removed RC32434_BASE_BAUD
 ****************************************************************************
*/
#ifndef _RC32434_H_
#define _RC32434_H_

#include <linux/autoconf.h>
#include <linux/delay.h>
#include <asm/io.h>

#define RC32434_REG_BASE   0x18000000

#define interrupt ((volatile INT_t ) INT0_VirtualAddress)


#define IDT_CLOCK_MULT 2
#define MIPS_CPU_TIMER_IRQ 7
/* Interrupt Controller */
#define IC_GROUP0_PEND     (RC32434_REG_BASE + 0x38000)
#define IC_GROUP0_MASK     (RC32434_REG_BASE + 0x38008)
#define IC_GROUP_OFFSET    0x0C

#define NUM_INTR_GROUPS    5
/* 16550 UARTs */

#define GROUP0_IRQ_BASE 8		/* GRP2 IRQ numbers start here */
#define GROUP1_IRQ_BASE (GROUP0_IRQ_BASE + 32) /* GRP3 IRQ numbers start here */
#define GROUP2_IRQ_BASE (GROUP1_IRQ_BASE + 32) /* GRP4 IRQ numbers start here */
#define GROUP3_IRQ_BASE (GROUP2_IRQ_BASE + 32)	/* GRP5 IRQ numbers start here */
#define GROUP4_IRQ_BASE (GROUP3_IRQ_BASE + 32)


#ifdef __MIPSEB__
#define RC32434_UART0_BASE (RC32434_REG_BASE + 0x58003)
#else
#define RC32434_UART0_BASE (RC32434_REG_BASE + 0x58000)
#endif

#define RC32434_UART0_IRQ  GROUP3_IRQ_BASE + 0
// #define EB434_UART1_IRQ    GROUP4_IRQ_BASE + 11

#define local_readl(addr) __raw_readl(addr)
#define local_writel(l,addr) __raw_writel(l,addr)

/* cpu pipeline flush */
static inline void rc32434_sync(void)
{
        __asm__ volatile ("sync");
}

static inline void rc32434_sync_udelay(int us)
{
        __asm__ volatile ("sync");
        udelay(us);
}

static inline void rc32434_sync_delay(int ms)
{
        __asm__ volatile ("sync");
        mdelay(ms);
}

/*
 * C access to CLZ and CLO instructions
 * (count leading zeroes/ones).
 */
static inline int rc32434_clz(unsigned long val)
{
	int ret;
        __asm__ volatile (
		".set\tnoreorder\n\t"
		".set\tnoat\n\t"
		".set\tmips32\n\t"
		"clz\t%0,%1\n\t"
                ".set\tmips0\n\t"
                ".set\tat\n\t"
                ".set\treorder"
                : "=r" (ret)
		: "r" (val));

	return ret;
}
static inline int rc32434_clo(unsigned long val)
{
	int ret;
        __asm__ volatile (
		".set\tnoreorder\n\t"
		".set\tnoat\n\t"
		".set\tmips32\n\t"
		"clo\t%0,%1\n\t"
                ".set\tmips0\n\t"
                ".set\tat\n\t"
                ".set\treorder"
                : "=r" (ret)
		: "r" (val));

	return ret;
}

#endif /* _RC32434_H_ */
