/*
 * arch/ubicom32/include/asm/smp.h
 *   SMP definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_SMP_H
#define _ASM_UBICOM32_SMP_H

#ifndef CONFIG_SMP
#error you should not include smp.h if smp is off
#endif

#ifndef ASSEMBLY
#include <linux/bitops.h>
#include <linux/threads.h>
#include <linux/cpumask.h>
#include <asm/ip5000.h>

typedef unsigned long address_t;
extern unsigned int smp_ipi_irq;

/*
 * This magic constant controls our willingness to transfer
 * a process across CPUs.
 *
 * Such a transfer incurs cache and tlb
 * misses. The current value is inherited from i386. Still needs
 * to be tuned for parisc.
 */
#define PROC_CHANGE_PENALTY	15		/* Schedule penalty */
#define NO_PROC_ID		0xFF		/* No processor magic marker */
#define ANY_PROC_ID		0xFF		/* Any processor magic marker */

#ifdef CONFIG_SMP
#define raw_smp_processor_id()	(current_thread_info()->cpu)
#endif /* CONFIG_SMP */

static inline int __cpu_disable (void)
{
  return 0;
}

static inline void __cpu_die (unsigned int cpu)
{
	while(1) {
	};
}

extern int __cpu_up(unsigned int cpu);
extern void smp_send_timer_all(void);
extern void smp_timer_broadcast(const struct cpumask *mask);
extern void smp_set_affinity(unsigned int irq, const struct cpumask *dest);
extern void arch_send_call_function_single_ipi(int cpu);
#define arch_send_call_function_ipi_mask arch_send_call_function_ipi_mask
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);

/*
 * TODO: Once these are fully tested, we should turn them into
 * inline macros for performance.
 */
extern unsigned long smp_get_affinity(unsigned int irq, int *all);
extern void smp_reset_ipi(unsigned long mask);

#endif /* !ASSEMBLY */
#endif /*  _ASM_UBICOM32_SMP_H */
