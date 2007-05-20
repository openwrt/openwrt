/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     timer routines for IDT EB434 boards
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
 * May 2004 rkt, neb
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/mc146818rtc.h>
#include <linux/irq.h>
#include <linux/timex.h>

#include <linux/param.h>
#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/time.h>
#include <asm/hardirq.h>

#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/debug.h>
#include <asm/time.h>

#include <asm/idt-boards/rc32434/rc32434.h>

static unsigned long r4k_offset;	/* Amount to incr compare reg each time */
static unsigned long r4k_cur;	/* What counter should be at next timer irq */

extern unsigned int idt_cpu_freq;

static unsigned long __init cal_r4koff(void)
{
	mips_hpt_frequency = idt_cpu_freq * IDT_CLOCK_MULT / 2;
	return (mips_hpt_frequency / HZ);
}

void __init aruba_time_init(void)
{
	unsigned int est_freq, flags;
	local_irq_save(flags);

	printk("calculating r4koff... ");
	r4k_offset = cal_r4koff();
	printk("%08lx(%d)\n", r4k_offset, (int)r4k_offset);

	est_freq = 2 * r4k_offset * HZ;
	est_freq += 5000;	/* round */
	est_freq -= est_freq % 10000;
	printk("CPU frequency %d.%02d MHz\n", est_freq / 1000000,
	       (est_freq % 1000000) * 100 / 1000000);
	local_irq_restore(flags);

}

void __init plat_timer_setup(struct irqaction *irq)
{
	/* we are using the cpu counter for timer interrupts */
	setup_irq(MIPS_CPU_TIMER_IRQ, irq);

	/* to generate the first timer interrupt */
	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);

}

asmlinkage void aruba_timer_interrupt(struct pt_regs *regs)
{
	int irq = MIPS_CPU_TIMER_IRQ;

	irq_enter();
	kstat_this_cpu.irqs[irq]++;

	timer_interrupt(irq, NULL);
	irq_exit();
}
