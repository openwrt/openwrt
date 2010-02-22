/*
 * arch/ubicom32/kernel/processor.c
 *   Ubicom32 architecture processor info implementation.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/profile.h>
#include <linux/clocksource.h>
#include <linux/types.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/cpu.h>
#include <asm/devtree.h>
#include <asm/processor.h>
#include <asm/cpu.h>
#include <asm/ocm_size.h>

struct procnode {
	struct devtree_node dn;
	unsigned int threads;
	unsigned int timers;
	unsigned int frequency;
	unsigned int ddr_frequency;
	unsigned int interrupt0;
	unsigned int interrupt1;
	void *socm;
	void *eocm;
	void *sdram;
	void *edram;
	unsigned int arch_version;
	void *os_syscall_begin;
	void *os_syscall_end;
};

struct procnode *pn;

/*
 * show_processorinfo()
 *	Print the actual processor information.
 */
static void show_processorinfo(struct seq_file *m)
{
	char *cpu, *mmu, *fpu;
	unsigned int clockfreq;
	unsigned int chipid;

	cpu = CPU;
	mmu = "none";
	fpu = "none";

	asm volatile (
	"move.4		%0, CHIP_ID	\n\t"
	: "=r" (chipid)
	);

	/*
	 * General Processor Information.
	 */
	seq_printf(m, "Vendor:\t\t%s\n", "Ubicom");
	seq_printf(m, "CPU:\t\t%s\n", cpu);
	seq_printf(m, "MMU:\t\t%s\n", mmu);
	seq_printf(m, "FPU:\t\t%s\n", fpu);
	seq_printf(m, "Arch:\t\t%hx\n", chipid >> 16);
	seq_printf(m, "Rev:\t\t%hx\n", (chipid & 0xffff));

	/*
	 * Now compute the clock frequency in Mhz.
	 */
	clockfreq = processor_frequency();
	seq_printf(m, "Clock Freq:\t%u.0 MHz\n",
		   clockfreq / 1000000);
	seq_printf(m, "DDR Freq:\t%u.0 MHz\n",
		   pn ? pn->ddr_frequency / 1000000 : 0);
	seq_printf(m, "BogoMips:\t%lu.%02lu\n",
		   (loops_per_jiffy * HZ) / 500000,
		   ((loops_per_jiffy * HZ) / 5000) % 100);
	seq_printf(m, "Calibration:\t%lu loops\n", (loops_per_jiffy * HZ));
}

/*
 * show_cpuinfo()
 *	Get CPU information for use by the procfs.
 */
static int show_cpuinfo(struct seq_file *m, void *v)
{
	unsigned long n = (unsigned long)v - 1;

#if defined(CONFIG_SMP)
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, n);
#endif

	/*
	 * Print the general processor information on the first
	 * call.
	 */
	if (n == 0) {
		show_processorinfo(m);
	}

#if defined(CONFIG_SMP)
	/*
	 * For each hwthread, print if this hwthread is running Linux
	 * or is an I/O thread.
	 */
	if (cpu_isset(n, cpu_online_map)) {
		seq_printf(m, "cpu[%02lu]:\tthread id - %lu\n", n, p->tid);
	} else {
		seq_printf(m, "cpu[%02lu]:\toff-line\n", n);
	}
#endif
	return 0;

}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	unsigned long i = *pos;

	return i < NR_CPUS ? (void *)(i + 1) : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}

static void c_stop(struct seq_file *m, void *v)
{
}

const struct seq_operations cpuinfo_op = {
	.start	= c_start,
	.next	= c_next,
	.stop	= c_stop,
	.show	= show_cpuinfo,
};

/*
 * processor_timers()
 *	Returns the timers available to Linux.
 */
unsigned int processor_timers(void)
{
	if (!pn) {
		return 0;
	}
	return pn->timers;
}

/*
 * processor_threads()
 *	Returns the threads available to Linux.
 */
unsigned int processor_threads(void)
{
	if (!pn) {
		return 0;
	}
	return pn->threads;
}

/*
 * processor_frequency()
 *	Returns the frequency of the system clock.
 */
unsigned int processor_frequency(void)
{
	if (!pn) {
		return 0;
	}
	return pn->frequency;
}
EXPORT_SYMBOL(processor_frequency);

/*
 * processor_interrupts()
 *	Return the interrupts that are setup at boot time.
 */
int processor_interrupts(unsigned int *int0, unsigned int *int1)
{
	if (!pn) {
		return -EFAULT;
	}

	if (int0) {
		*int0 = pn->interrupt0;
	}

	if (int1) {
		*int1 = pn->interrupt1;
	}
	return 0;
}

/*
 * processor_ocm()
 *	Returns the start and end of OCM available to Linux.
 */
void processor_ocm(unsigned long *socm, unsigned long *eocm)
{
	*socm = (unsigned long)pn->socm;
	*eocm = (unsigned long)pn->eocm;
}

/*
 * processor_dram()
 *	Returns the start and end of dram available to Linux.
 */
void processor_dram(unsigned long *sdram, unsigned long *edram)
{
	*sdram = (unsigned long)pn->sdram;
	*edram = (unsigned long)pn->edram;
}

/*
 * processor_validate_failed()
 *	Returns the dram available to Linux.
 */
static noinline void processor_validate_failed(void)
{
	while (1)
		THREAD_STALL;
}

/*
 * processor_validate()
 *	Validates the procnode against limitations of this link/built.
 */
static void processor_validate(void)
{
	void *dram_start = (void *)(KERNELSTART);
	void *dram_end   = (void *)(SDRAMSTART + CONFIG_MIN_RAMSIZE);
#if APP_OCM_CODE_SIZE || APP_OCM_DATA_SIZE
	void *ocm_code_start = (void *)(OCMSTART + APP_OCM_CODE_SIZE);
	void *ocm_data_end   = (void *)(OCMEND   - APP_OCM_DATA_SIZE);
#endif
	extern void __os_syscall_begin;
	extern void __os_syscall_end;
	int proc_node_valid = 1;

	if (!pn) {
		printk(KERN_ERR "ERROR: processor node not found\n");
		goto error;
	}


	if (dram_start < pn->sdram || dram_end > pn->edram) {
		printk(KERN_ERR "ERROR: processor dram mismatch %p-%p "
		       "available but we are expecting %p-%p\n",
		       pn->sdram, pn->edram, dram_start, dram_end);
		proc_node_valid = 0;
	} else {
		printk(KERN_ERR "processor dram %p-%p, expecting %p-%p\n",
		       pn->sdram, pn->edram, dram_start, dram_end);
	}
	if (&__os_syscall_begin < pn->os_syscall_begin ||
	    &__os_syscall_end > pn->os_syscall_end) {
		printk(KERN_ERR "ERROR: processor syscall area mismatch "
		       "%p-%p available but we are expecting %p-%p\n",
		       pn->os_syscall_begin, pn->os_syscall_end,
		       &__os_syscall_begin, &__os_syscall_end);
		proc_node_valid = 0;
	} else {
		printk(KERN_ERR "processor dram %p-%p, expecting %p-%p\n",
		       pn->sdram, pn->edram, dram_start, dram_end);
	}
#if APP_OCM_CODE_SIZE || APP_OCM_DATA_SIZE
	if (ocm_code_start < pn->socm ||  ocm_data_end > pn->eocm) {
		printk(KERN_ERR "ERROR: processor ocm mismatch %p-%p "
		       "available but we are expecting %p-%p\n",
		       pn->socm, pn->eocm, ocm_code_start, ocm_data_end);
		proc_node_valid = 0;
	} else {
		printk(KERN_INFO "processor ocm %p-%p, expecting %p-%p\n",
		       pn->socm, pn->eocm, ocm_code_start, ocm_data_end);

	}
#endif

	if (UBICOM32_ARCH_VERSION != pn->arch_version) {
		printk(KERN_ERR "ERROR: processor arch mismatch, kernel"
		       "compiled for %d found %d\n",
		       UBICOM32_ARCH_VERSION, pn->arch_version);
		proc_node_valid = 0;
	}

	if (proc_node_valid)
		return;
error:
	processor_validate_failed();
}

void __init processor_init(void)
{
	/*
	 * If we do not have a trap node in the device tree, we leave the fault
	 * handling to the underlying hardware.
	 */
	pn = (struct procnode *)devtree_find_node("processor");

	processor_validate();

	/*
	 * If necessary correct the initial range registers to cover the
	 * complete physical space
	 */
	if (pn->edram > (void *)(SDRAMSTART + CONFIG_MIN_RAMSIZE)) {
		printk(KERN_INFO "updating range registers for expanded dram\n");
		asm volatile (
			"	move.4 D_RANGE1_HI, %0		\t\n"
			"	move.4 I_RANGE0_HI, %0		\t\n"
#ifdef CONFIG_PROTECT_KERNEL
			"	move.4 D_RANGE2_HI, %0		\t\n"
			"	move.4 I_RANGE2_HI, %0		\t\n"
#endif
		: : "a"((unsigned long)pn->edram - 4)
			);
	}

}
