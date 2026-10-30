/*
 *  linux/arch/arm/mach-alpine/platsmp.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/smp.h>

#include <asm/cacheflush.h>
#include <asm/unified.h>

#include <mach/alpine_machine.h>

/* XXX alpine_pen_release is cargo culted code - DO NOT COPY XXX */
volatile int alpine_pen_release = -1;

extern void secondary_startup(void);

static void ca15x4_init_cpu_map(void) {
  unsigned int i, ncores;

  asm volatile("mrc p15, 1, %0, c9, c0, 2\n" : "=r"(ncores));
  ncores = ((ncores >> 24) & 3) + 1;

  for (i = 0; i < ncores; i++)
    set_cpu_possible(i, true);
}

static void ca15x4_smp_enable(unsigned int max_cpus) {
  int i;

  for (i = 0; i < max_cpus; i++)
    set_cpu_present(i, true);
}

/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not.  This is necessary for the hotplug code to work reliably.
 */
static void write_pen_release(int val) {
  alpine_pen_release = val;
  smp_wmb();
  __cpuc_flush_dcache_area((void *)&alpine_pen_release,
                           sizeof(alpine_pen_release));
  outer_clean_range(__pa(&alpine_pen_release), __pa(&alpine_pen_release + 1));
}

static DEFINE_SPINLOCK(boot_lock);

static void platform_secondary_init(unsigned int cpu) {
  /*
   * let the primary processor know we're out of the
   * pen, then head off into the C entry point
   */
  write_pen_release(-1);

  /*
   * Synchronise with the boot thread.
   */
  spin_lock(&boot_lock);
  spin_unlock(&boot_lock);
}

static int al_boot_secondary(unsigned int cpu, struct task_struct *idle) {
  unsigned long timeout;

  /* Check CPU resume regs validity */
  if (!alpine_cpu_suspend_wakeup_supported()) {
    WARN(1, "%s: wakeup not supported!\n", __func__);
    return -ENOSYS;
  }

  /*
   * Set synchronisation state between this boot processor
   * and the secondary one
   */
  spin_lock(&boot_lock);

  /*
   * This is really belt and braces; we hold unintended secondary
   * CPUs in the holding pen until we're ready for them.  However,
   * since we haven't sent them a soft interrupt, they shouldn't
   * be there.
   */
  write_pen_release(cpu);

  /* Wake-up secondary CPU */
  alpine_cpu_wakeup(cpu, virt_to_phys(secondary_startup));

  /*
   * Send the secondary CPU a soft interrupt, thereby causing
   * the boot monitor to read the system wide flags register,
   * and branch to the address found there.
   */
  arch_send_wakeup_ipi_mask(cpumask_of(cpu));

  timeout = jiffies + (1 * HZ);
  while (time_before(jiffies, timeout)) {
    smp_rmb();
    if (alpine_pen_release == -1)
      break;

    udelay(10);
  }

  /*
   * now the secondary core is starting up let it run its
   * calibrations, then wait for it to finish
   */
  spin_unlock(&boot_lock);

  return alpine_pen_release != -1 ? -ENOSYS : 0;
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
static void __init al_smp_init_cpus(void) { ca15x4_init_cpu_map(); }

static void __init platform_smp_prepare_cpus(unsigned int max_cpus) {
  alpine_cpu_pm_init();

  /*
   * Initialise the present map, which describes the set of CPUs
   * actually populated at the present time.
   */
  ca15x4_smp_enable(max_cpus);
}

extern int alpine_suspend_finish(unsigned long);

struct smp_operations __initdata al_smp_ops = {
    .smp_init_cpus = al_smp_init_cpus,
    .smp_prepare_cpus = platform_smp_prepare_cpus,
    .smp_secondary_init = platform_secondary_init,
    .smp_boot_secondary = al_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
    .cpu_die = alpine_cpu_die,
    .cpu_kill = alpine_cpu_kill,
#endif
};
