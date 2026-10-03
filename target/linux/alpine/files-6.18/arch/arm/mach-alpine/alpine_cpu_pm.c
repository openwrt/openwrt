/*
 * Alpine CPU Power Management Services
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

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/cpu_pm.h>
#include <linux/suspend.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/suspend.h>

#include <mach/alpine_machine.h>
#include "al_init_cpu_resume.h"

/* NB registers */
#undef AL_NB_SERVICE_BASE
#define AL_NB_SERVICE_BASE		al_nb_service_base
#define AL_NB_INIT_CONTROL		(AL_NB_SERVICE_BASE + 0x8)
#define AL_NB_POWER_CONTROL(cpu)	(AL_NB_SERVICE_BASE +		\
						0x2000 + (cpu)*0x100 + 0x20)
#define AL_NB_POWER_STATUS(cpu)		(AL_NB_SERVICE_BASE +		\
						0x2000 + (cpu)*0x100 + 0x24)

int alpine_suspend_finish(unsigned long);

static void __iomem *al_nb_service_base;
static struct al_cpu_resume_regs __iomem *al_cpu_resume_regs;
static int suspend_wakeup_supported;

int alpine_cpu_suspend_wakeup_supported(void)
{
	return suspend_wakeup_supported;
}
EXPORT_SYMBOL(alpine_cpu_suspend_wakeup_supported);

void alpine_cpu_wakeup(unsigned int cpu, uintptr_t resume_addr)
{
	/*
	 * Cancel previous powerdown request
	 * This can happen if the CPU is "hot plugged in" after being powered
	 * off due to being "hot plugged out" - see 'alpine_cpu_die' below.
	 */
	writel(0, (void __iomem *)AL_NB_POWER_CONTROL(cpu));

	/* Set CPU resume address */
	writel(resume_addr, &al_cpu_resume_regs->per_cpu[cpu].resume_addr);

	/* Release from reset - has effect once per SoC reset */
	writel(readl(AL_NB_INIT_CONTROL) | (1 << cpu), AL_NB_INIT_CONTROL);
}
EXPORT_SYMBOL(alpine_cpu_wakeup);

void alpine_cpu_die(unsigned int cpu)
{
	if (!suspend_wakeup_supported) {
		pr_err("Annapurna Labs PM components not found\n");
		return;
	}

	/* request powerdown. cpu will be turned off when it issues WFI */
	writel(0x3 | (0x3 << 20) | (0x3 << 18), (void __iomem *)AL_NB_POWER_CONTROL(cpu));

	alpine_suspend_finish(0);

	BUG(); /*execution should never reach this point */
}
EXPORT_SYMBOL(alpine_cpu_die);

/* Needed by kexec and platform_can_cpu_hotplug() */
int alpine_cpu_kill(unsigned int cpu)
{
	int k;
	u32 val;

	/* Wait until the dying CPU enters the powerdown state. */
	for (k = 0; k < 100; k++) {
		val = readl((void __iomem *)AL_NB_POWER_STATUS(cpu));
		if ((val & 0x3) == 0x3) {
			/*
			 * We need a delay here to ensure that the dying CPU
			 * can reach the WFI state.
			 */
			msleep(5);
			return 1;
		}
		msleep(1);
	}

	return 0;
}
EXPORT_SYMBOL(alpine_cpu_kill);

void alpine_cpu_suspend(void)
{
	unsigned int cpu = smp_processor_id();

	/* Write the resume address */
	writel(virt_to_phys(cpu_resume),
			&al_cpu_resume_regs->per_cpu[cpu].resume_addr);
	writel(AL_CPU_RESUME_FLG_PERCPU_CLUSTER_SKIP,
			&al_cpu_resume_regs->per_cpu[cpu].flags);

	/* request powerdown. cpu will be turned off when it issues WFI
	 * bits 0:1 - request core powerdown
	 * bits 20:21 - do not wake-up from i/o gic
	 */
	writel(0x3 | (3<<20), (void __iomem *)AL_NB_POWER_CONTROL(cpu));
	/*verify the write got through*/
	readl((void __iomem *)AL_NB_POWER_CONTROL(cpu));

	cpu_pm_enter();
	cpu_suspend(0, alpine_suspend_finish);

	/*clear the powerdown request*/
	writel(0, (void __iomem *)AL_NB_POWER_CONTROL(cpu));
	/*verify the write got through*/
	readl((void __iomem *)AL_NB_POWER_CONTROL(cpu));

	cpu_pm_exit();
}
EXPORT_SYMBOL(alpine_cpu_suspend);

#ifdef CONFIG_PM
static int al_pm_valid(suspend_state_t state)
{
	return ((state == PM_SUSPEND_STANDBY) || (state == PM_SUSPEND_MEM));
}

static int al_pm_enter(suspend_state_t state)
{
	if (al_pm_valid(state))
		alpine_cpu_suspend();
	else
		return -EINVAL;

	return 0;
}

static const struct platform_suspend_ops al_pm_ops = {
	.enter = al_pm_enter,
	.valid = al_pm_valid,
};
#endif

void __init alpine_cpu_pm_init(void)
{
	struct device_node *np;

	np = of_find_compatible_node(
			NULL, NULL, "annapurna-labs,al-nb-service");
	al_nb_service_base = of_iomap(np, 0);

	np = of_find_compatible_node(
			NULL, NULL, "annapurna-labs,al-cpu-resume");
	al_cpu_resume_regs =
			(struct al_cpu_resume_regs __iomem *)of_iomap(np, 0);

	suspend_wakeup_supported =
		al_nb_service_base &&
		al_cpu_resume_regs &&
		((readl(&al_cpu_resume_regs->watermark) &
		 AL_CPU_RESUME_MAGIC_NUM_MASK) == AL_CPU_RESUME_MAGIC_NUM) &&
		((readl(&al_cpu_resume_regs->watermark) &
		 AL_CPU_RESUME_MIN_VER_MASK) >= AL_CPU_RESUME_MIN_VER);

#ifdef CONFIG_PM
	if (suspend_wakeup_supported) {
		suspend_set_ops(&al_pm_ops);
	} else {
		pr_err("Annapurna Labs PM components not found\n");
		return;
	}
#endif
}

