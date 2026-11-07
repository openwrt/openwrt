// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 *
 * The boot loader runs from on-chip SRAM.  Its reset code sends every
 * core but CPU0 into a loop: wait for an event (WFE), then jump to the
 * start of the SRAM, where the boot loader's reset vector leads back to
 * the WFE.  To start the second core, replace that vector with a jump to
 * secondary_startup and send an event.
 *
 * To take the second core offline again (CPU hotplug, and kexec, which needs
 * it), the core puts the boot loader's vector back and returns to the boot
 * loader's loop with the MMU and caches off. That loop is outside the
 * kernel's memory, so the core survives the kernel being replaced, and the
 * next start works the same way as the first.
 */

#include <linux/cacheflush.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/smp.h>

#include <asm/cp15.h>
#include <asm/idmap.h>
#include <asm/proc-fns.h>
#include <asm/smp_scu.h>
#include <asm/tlbflush.h>

/* ldr pc, [pc, #-4]: jump to the address stored in the next word */
#define ZX279128S_JUMP_INSN	0xe51ff004

static void __iomem *zx279128s_smp_sram;
static phys_addr_t zx279128s_smp_sram_phys;
/* the boot loader's vector, as found at boot */
static u32 zx279128s_boot_vector[2];

static int zx279128s_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	if (!zx279128s_smp_sram)
		return -ENODEV;

	/*
	 * Store the address before the instruction: a core that wakes up
	 * in between still runs the boot loader's vector and goes back to
	 * sleep instead of jumping to a stale address.
	 */
	writel(__pa_symbol(secondary_startup), zx279128s_smp_sram + 4);
	writel(ZX279128S_JUMP_INSN, zx279128s_smp_sram);

	/* Complete both writes, then wake the core */
	dsb_sev();

	return 0;
}

static void __init zx279128s_smp_prepare_cpus(unsigned int max_cpus)
{
	struct device_node *np;
	struct resource res;
	void __iomem *scu;

	np = of_find_compatible_node(NULL, NULL, "arm,cortex-a9-scu");
	if (!np) {
		pr_err("zx279128s: no SCU node\n");
		return;
	}
	scu = of_iomap(np, 0);
	of_node_put(np);
	if (!scu) {
		pr_err("zx279128s: cannot map the SCU\n");
		return;
	}
	scu_enable(scu);
	iounmap(scu);

	np = of_find_compatible_node(NULL, NULL, "zte,zx279128s-smp-sram");
	if (!np) {
		pr_err("zx279128s: no SMP SRAM node\n");
		return;
	}
	if (of_address_to_resource(np, 0, &res)) {
		of_node_put(np);
		pr_err("zx279128s: no SMP SRAM address\n");
		return;
	}
	zx279128s_smp_sram = of_iomap(np, 0);
	of_node_put(np);
	if (!zx279128s_smp_sram) {
		pr_err("zx279128s: cannot map the SMP SRAM\n");
		return;
	}
	zx279128s_smp_sram_phys = res.start;
	zx279128s_boot_vector[0] = readl(zx279128s_smp_sram);
	zx279128s_boot_vector[1] = readl(zx279128s_smp_sram + 4);

	/* zx279128s_cpu_die() reads these with its data cache off */
	sync_cache_w(&zx279128s_smp_sram);
	sync_cache_w(&zx279128s_smp_sram_phys);
	sync_cache_w(&zx279128s_boot_vector);
}

#ifdef CONFIG_HOTPLUG_CPU
typedef void (*phys_reset_t)(unsigned long addr, bool hvc);

/*
 * If the vector already holds a jump, an earlier kernel started the core
 * and did not put the boot loader's vector back: the original is unknown,
 * so the core cannot be parked safely.
 */
static bool zx279128s_cpu_can_disable(unsigned int cpu)
{
	return zx279128s_smp_sram &&
	       zx279128s_boot_vector[0] != ZX279128S_JUMP_INSN;
}

static void zx279128s_cpu_die(unsigned int cpu)
{
	phys_reset_t phys_reset;

	/* Take out a flat mapping, which keeps the kernel mappings too */
	setup_mm_for_reboot();

	/*
	 * The boot loader's loop executes the vector, which
	 * zx279128s_boot_secondary() changes with data writes. Stop caching
	 * and predicting instructions, as at power-on, so that the parked
	 * core sees the change.
	 */
	set_cr(get_cr() & ~(CR_I | CR_Z));
	__flush_icache_all();
	local_flush_bp_all();

	/* Clean the caches and leave coherency with the other core */
	v7_exit_coherency_flush(louis);

	/*
	 * Put the boot loader's vector back, instruction word last: the core
	 * then wakes up into the boot loader's loop, and zx279128s_cpu_kill()
	 * sees that it is about to leave the kernel.
	 */
	writel_relaxed(zx279128s_boot_vector[1], zx279128s_smp_sram + 4);
	writel_relaxed(zx279128s_boot_vector[0], zx279128s_smp_sram);
	dsb();

	/* Turn the MMU off and continue at the boot loader's vector */
	phys_reset = (phys_reset_t)virt_to_idmap(cpu_reset);
	phys_reset(zx279128s_smp_sram_phys, false);
}

static int zx279128s_cpu_kill(unsigned int cpu)
{
	u32 val;

	/* Wait until the dying core has put the vector back */
	return !readl_poll_timeout(zx279128s_smp_sram, val,
				   val == zx279128s_boot_vector[0], 1000,
				   100 * USEC_PER_MSEC);
}
#endif

static const struct smp_operations zx279128s_smp_ops __initconst = {
	.smp_prepare_cpus	= zx279128s_smp_prepare_cpus,
	.smp_boot_secondary	= zx279128s_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_can_disable	= zx279128s_cpu_can_disable,
	.cpu_die		= zx279128s_cpu_die,
	.cpu_kill		= zx279128s_cpu_kill,
#endif
};

CPU_METHOD_OF_DECLARE(zx279128s_smp, "zte,zx279128s-smp", &zx279128s_smp_ops);
