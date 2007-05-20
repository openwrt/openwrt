 /**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *   Definitions for IDT RC32434 CPU
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb.
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#ifndef _RC32434_H_
#define _RC32434_H_

#include <linux/autoconf.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/idt-boards/rc32434/rc32434_timer.h>

#define RC32434_REG_BASE   0x18000000


#define interrupt ((volatile INT_t ) INT0_VirtualAddress)
#define idt_timer     ((volatile TIM_t)  TIM0_VirtualAddress)
#define idt_gpio	  ((volatile GPIO_t) GPIO0_VirtualAddress)

#define IDT_CLOCK_MULT 2
#define MIPS_CPU_TIMER_IRQ 7
/* Interrupt Controller */
#define IC_GROUP0_PEND     (RC32434_REG_BASE + 0x38000)
#define IC_GROUP0_MASK     (RC32434_REG_BASE + 0x38008)
#define IC_GROUP_OFFSET    0x0C
#define RTC_BASE           0xBA001FF0

#define NUM_INTR_GROUPS    5
/* 16550 UARTs */

#define GROUP0_IRQ_BASE 8		/* GRP2 IRQ numbers start here */
#define GROUP1_IRQ_BASE (GROUP0_IRQ_BASE + 32) /* GRP3 IRQ numbers start here */
#define GROUP2_IRQ_BASE (GROUP1_IRQ_BASE + 32) /* GRP4 IRQ numbers start here */
#define GROUP3_IRQ_BASE (GROUP2_IRQ_BASE + 32)	/* GRP5 IRQ numbers start here */
#define GROUP4_IRQ_BASE (GROUP3_IRQ_BASE + 32)

#ifdef __MIPSEB__

#define RC32434_UART0_BASE (RC32434_REG_BASE + 0x58003)
#define EB434_UART1_BASE   (0x19800003)

#else

#define RC32434_UART0_BASE (RC32434_REG_BASE + 0x58000)
#define EB434_UART1_BASE   (0x19800000)

#endif

#define RC32434_UART0_IRQ  GROUP3_IRQ_BASE + 0
#define EB434_UART1_IRQ    GROUP4_IRQ_BASE + 11

#define RC32434_NR_IRQS  (GROUP4_IRQ_BASE + 32)

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
 * Macros to access internal RC32434 registers. No byte
 * swapping should be done when accessing the internal
 * registers.
 */

#define rc32434_readb __raw_readb
#define rc32434_readw __raw_readw
#define rc32434_readl __raw_readl

#define rc32434_writeb __raw_writeb
#define rc32434_writew __raw_writew
#define rc32434_writel __raw_writel

#if 0
static inline u8 rc32434_readb(unsigned long pa)
{
	return *((volatile u8 *)KSEG1ADDR(pa));
}
static inline u16 rc32434_readw(unsigned long pa)
{
	return *((volatile u16 *)KSEG1ADDR(pa));
}
static inline u32 rc32434_readl(unsigned long pa)
{
	return *((volatile u32 *)KSEG1ADDR(pa));
}
static inline void rc32434_writeb(u8 val, unsigned long pa)
{
	*((volatile u8 *)KSEG1ADDR(pa)) = val;
}
static inline void rc32434_writew(u16 val, unsigned long pa)
{
	*((volatile u16 *)KSEG1ADDR(pa)) = val;
}
static inline void rc32434_writel(u32 val, unsigned long pa)
{
	*((volatile u32 *)KSEG1ADDR(pa)) = val;
}

#endif


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












