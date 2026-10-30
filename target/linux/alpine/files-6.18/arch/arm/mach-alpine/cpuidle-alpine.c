// SPDX-License-Identifier: GPL-2.0-or-later
/* Annapurna labs cpu-idle handler. */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/io.h>
#include <linux/export.h>

#include <asm/proc-fns.h>
#include <asm/suspend.h>
#include <asm/cpuidle.h>

int alpine_cpu_suspend_wakeup_supported(void);
void alpine_cpu_suspend(void);

static int alpine_enter_lowpower(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
				int index);

static struct cpuidle_driver alpine_idle_driver = {
	.name			= "alpine_idle",
	.owner			= THIS_MODULE,
	.states[0] = ARM_CPUIDLE_WFI_STATE_PWR(250),
	.states[1] = {
		.enter			= alpine_enter_lowpower,
		.exit_latency		= 10,
		.power_usage		= 125,
		.target_residency	= 1000,
		.flags			= 0,
		.name			= "C1",
		.desc			= "ARM power down",
	},
	.state_count = 2,
};

static int alpine_enter_lowpower(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
				int index)
{
	alpine_cpu_suspend();
	return index;
}

static int __init alpine_init_cpuidle(void)
{
	if (!alpine_cpu_suspend_wakeup_supported()) {
		pr_err("Annapurna Labs CPUidle components not found\n");
		return -ENOENT;
	}

	alpine_idle_driver.safe_state_index = 0;

	/* Registers the driver and a device for every possible CPU */
	return cpuidle_register(&alpine_idle_driver, NULL);
}

device_initcall(alpine_init_cpuidle);
