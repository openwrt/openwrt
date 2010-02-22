/*
 * arch/ubicom32/oprofile/profile.c
 *	Oprofile support for arch Ubicom32
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option)
 * any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

/**
 * @file profile.c
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Hunyue Yau <hy@hy-research.com>
 */

#include <linux/oprofile.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/devtree.h>
#include <asm/thread.h>

/* For identifying userland vs kernel address */
#include <asm/stacktrace.h>
#include "ipProf.h"

/* For communications with the backend */
static struct profilenode *profile_node;

/* Bitmask containing all Linux threads - as seen by the ROSR reg */
static unsigned long th_all_mask;

/* Lookup table to translate a hardware thread into a CPU identifier
 * Table is indexed by the ROSR value which is assumed to be
 * relatively small (0...15).
 */
unsigned int cpu_map[THREAD_ARCHITECTURAL_MAX];

static struct pt_regs regs;

/*
 * For each sample returned, checked to see if they are relevant to
 * us. This is necessary as the ubicom32 architecture has other software
 * running outside of Linux. Only then, put the sample into the relevant
 * cpu bins.
 *
 * To minimize overhead, a global mask with all possible threads of in
 * interest to us is used as a first check. Then a second mask identifying
 * the thread is used to obtain an identifier for that "CPU".
 */

/*
 * ubicom32_build_cpu_th_mask()
 *
 * Build a lookup table for translation between hardware thread
 * "ROSR" values and Linux CPU ids
 *
 * *** This gets executed on all CPUs at once! ***
 */
static void ubicom32_build_cpu_th_mask(void *mask)
{
	thread_t self = thread_get_self();
	unsigned long *th_m = mask;

	BUG_ON(self <= 0 || self >= THREAD_ARCHITECTURAL_MAX);
	cpu_map[self] = smp_processor_id();

	set_bit(self, th_m);
}

/*
 * profile_interrupt()
 *
 * Process samples returned from the profiler backend. The backend
 * may return samples that are irrelevant to us or may even return
 * multiple samples for the same CPU. Note that the sames may be
 * for ANY cpu. At this time, this is unique and to support this requires
 * Oprofile to expose an interface to accept the CPU that the same came
 * frome.
 */
static irqreturn_t profile_interrupt(int irq, void *arg)
{
	int i, buf_entry;
	int is_kernel;
	unsigned int bit_th;
	unsigned int th;

	if (!(profile_node->enabled) || profile_node->count < 0) {
		printk(KERN_WARNING
			"Unexpected interrupt, no samples or not enabled!\n");
		return IRQ_HANDLED;
	}

	profile_node->busy = 1;		/* Keep backend out */

	for (i = 0; i < profile_node->count; i++) {
		buf_entry = profile_node->tail;
		profile_node->tail++;
		profile_node->tail %= IPPROFILETIO_MAX_SAMPLES;

		/* Note - the "thread" ID is only the lower 4 bits */
		th = (0x0f & profile_node->samples[buf_entry].thread);
		bit_th = (1 << th);

		if ((bit_th & th_all_mask) == 0)
			continue;

		regs.pc = profile_node->samples[buf_entry].pc;

		is_kernel = ubicom32_is_kernel(regs.pc);

		oprofile_add_ext_sample_cpu(regs.pc, &regs, 0, is_kernel,
					    cpu_map[th]);
	}
	profile_node->count = 0;
	profile_node->busy = 0;

	return IRQ_HANDLED;
}

/*
 * profile_start()
 *
 * Notification from oprofile to start the profiler
 */
static int profile_start(void)
{
	if (!profile_node)
		return -1;

	profile_node->enabled = 1;

	return 0;
}

/*
 * profile_stop()
 *
 * Notification from oprofile to stop the profiler
 */
static void profile_stop(void)
{
	if (profile_node)
		profile_node->enabled = 0;
}

/*
 * oprofile_arch_init()
 *
 * Attach to Oprofile after qualify the availability of the backend
 * profiler support.
 */
int __init oprofile_arch_init(struct oprofile_operations *ops)
{
	int r = -ENODEV;

	profile_node = (struct profilenode *)devtree_find_node("profiler");

	if (profile_node == NULL) {
		printk(KERN_WARNING "Cannot find profiler node\n");
		return r;
	}

	r = request_irq(profile_node->dn.recvirq, profile_interrupt,
			IRQF_DISABLED, "profiler", NULL);

	if (r < 0) {
		profile_node = NULL;
		printk(KERN_WARNING "Cannot get profiler IRQ\n");
		return r;
	}

	ops->start = profile_start;
	ops->stop = profile_stop;
	ops->cpu_type = "timer";

	memset(cpu_map, 0, sizeof(cpu_map));

	on_each_cpu(ubicom32_build_cpu_th_mask, &th_all_mask, 1);

	memset(&regs, 0, sizeof(regs));

	return r;
}

/*
 * oprofile_arch_exit()
 *
 * External call to take outselves out.
 * Make sure backend is not running.
 */
void oprofile_arch_exit(void)
{
	BUG_ON(profile_node->enabled);
}
