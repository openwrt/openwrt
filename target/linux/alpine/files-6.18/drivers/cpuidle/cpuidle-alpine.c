/*
 * Annapurna labs cpu-idle handler.
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

#include "cpuidle.h"

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

static DEFINE_PER_CPU(struct cpuidle_device, alpine_cpuidle_device);

static int alpine_enter_lowpower(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
				int index)
{
	alpine_cpu_suspend();
	return index;
}

static int __init alpine_init_cpuidle(void)
{
	int cpu_id;
	struct cpuidle_device *device;
	struct cpuidle_driver *drv = &alpine_idle_driver;

	if (cpuidle_disabled())
		return -ENOENT;

	if (!(alpine_cpu_suspend_wakeup_supported())) {
		pr_err("Annapurna Labs CPUidle components not found\n");
		return -ENOENT;
	}

	/* Setup cpuidle driver */
	drv->safe_state_index = 0;
	cpuidle_register_driver(&alpine_idle_driver);

	for_each_cpu(cpu_id, cpu_online_mask) {
		device = &per_cpu(alpine_cpuidle_device, cpu_id);
		device->cpu = cpu_id;
//		device->state_count = alpine_idle_driver.state_count;
		if (cpuidle_register_device(device)) {
			pr_err("CPUidle device registration failed\n,");
			return -EIO;
		}
	}

	return 0;
}
device_initcall(alpine_init_cpuidle);
