// SPDX-License-Identifier: GPL-2.0-only
/* Reuses the preceding GPL RTX830 port's hardware-tested reset sequence.
 * The board reboot notifier prepares the FPGA reset state first.
 * No CPLD/GPIO/SPI writes here. Module init is read-only.
 */
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/reboot.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <asm/irqflags.h>
#include <asm/barrier.h>

#include "rtx830_board.h"

#define WD_LOAD 0x00 /* Cortex-A9 TWD watchdog +0x20 */
#define WD_COUNTER 0x04
#define WD_CONTROL 0x08 /* TWD +0x28 */
#define WD_INTSTAT 0x0c
#define WD_RESETSTAT 0x10
#define OEM_CONTROL 0xff09
#define OEM_ROUTE 0x00030103

static bool enable = true;
module_param(enable, bool, 0400);
MODULE_PARM_DESC(enable, "1=register OEM private-watchdog restart; 0=read-only snapshot (no init writes)");
static void __iomem *wd, *mask, *route;
static bool registered;
struct snapshot {
	u32 load;
	u32 control;
	u32 intstat;
	u32 resetstat;
};

static struct snapshot initial;

static void cpu0_snapshot(void *arg)
{
	struct snapshot *s = arg;

	s->load = readl(wd + WD_LOAD);
	s->control = readl(wd + WD_CONTROL);
	s->intstat = readl(wd + WD_INTSTAT);
	s->resetstat = readl(wd + WD_RESETSTAT);
}

static int restart_oem(struct notifier_block *nb, unsigned long action, void *data)
{
	u32 saved_mask, control;
	unsigned int i;

	if (!rtx830_restart_is_prepared()) {
		pr_emerg("rtx830_restart: board preparation missing; refusing private watchdog writes\n");
		return NOTIFY_DONE;
	}
	/* ARM machine_restart has IRQs off and stopped other CPUs. Reboot path
	 * normally migrates to reboot_cpu=0. Refuse to arm a different banked WD.
	 */
	if (raw_smp_processor_id() != 0) {
		pr_emerg("rtx830_restart: CPU0 required; falling back without OEM writes\n");
		return NOTIFY_DONE;
	}
	control = readl(wd + WD_CONTROL);
	if (control & BIT(0)) {
		pr_emerg("rtx830_restart: unexpected active watchdog; refusing OEM writes\n");
		return NOTIFY_DONE;
	}
	local_irq_disable();
	local_fiq_disable();
	saved_mask = readl(mask);
	pr_emerg("rtx830_restart: OEM private-WD reset on CPU0, mask=%08x route=%08x control=%08x\n",
		 saved_mask, readl(route), control);
	/* Preserve unrelated reset mask bits; use the verified reset route. */
	writel(saved_mask & ~BIT(10), mask);
	if (readl(mask) != (saved_mask & ~BIT(10))) {
		pr_emerg("rtx830_restart: reset-mask readback failed; fallback\n");
		return NOTIFY_DONE;
	}
	writel(OEM_ROUTE, route);
	if (readl(route) != OEM_ROUTE) {
		pr_emerg("rtx830_restart: reset-route readback failed; fallback\n");
		return NOTIFY_DONE;
	}
	dsb(sy);
	writel(OEM_CONTROL, wd + WD_CONTROL);
	/* Device writes are ordered; barrier ensures watchdog enabled before load. */
	dsb(sy);
	writel(0, wd + WD_LOAD);
	dsb(sy);
	/* Atomic bounded grace period. Successful reset never returns. */
	/* Atomic restart context cannot sleep. */
	for (i = 0; i < 1000; i++)
		mdelay(1);
	pr_emerg("rtx830_restart: OEM watchdog did not reset within 1s; load=%08x counter=%08x control=%08x intstat=%08x resetstat=%08x mask=%08x route=%08x; fallback\n",
		 readl(wd + WD_LOAD), readl(wd + WD_COUNTER), readl(wd + WD_CONTROL),
		 readl(wd + WD_INTSTAT), readl(wd + WD_RESETSTAT), readl(mask), readl(route));
	return NOTIFY_DONE;
}

static struct notifier_block restart_nb = {
	.notifier_call = restart_oem,
	.priority = 255, /* ARM machine/mvebu handler is priority 128. */
};

static int rtx830_restart_probe(struct platform_device *pdev)
{
	int ret;

	if (!cpu_online(0))
		return -ENODEV;
	/* DT describes exact ranges. Do not reserve the Linux private timer,
	 * or the complete system controller shared with the mvebu fallback.
	 */
	wd = devm_platform_ioremap_resource_byname(pdev, "private-watchdog");
	if (IS_ERR(wd))
		return PTR_ERR(wd);
	mask = devm_platform_ioremap_resource_byname(pdev, "reset-mask");
	if (IS_ERR(mask))
		return PTR_ERR(mask);
	route = devm_platform_ioremap_resource_byname(pdev, "reset-route");
	if (IS_ERR(route))
		return PTR_ERR(route);
	ret = smp_call_function_single(0, cpu0_snapshot, &initial, 1);
	if (ret)
		return ret;
	pr_info("rtx830_restart: read-only CPU0 snapshot load=%08x control=%08x intstat=%08x resetstat=%08x mask=%08x route=%08x enable=%u\n",
		initial.load, initial.control, initial.intstat, initial.resetstat,
		readl(mask), readl(route), enable);
	if (!enable)
		return 0;
	if (initial.control & BIT(0))
		return -EBUSY;
	ret = register_restart_handler(&restart_nb);
	if (ret)
		return ret;
	registered = true;
	pr_info("rtx830_restart: handler priority255 registered; no registers changed at init\n");
	return 0;
}

static void rtx830_restart_remove(struct platform_device *pdev)
{
	if (registered) {
		unregister_restart_handler(&restart_nb);
		registered = false;
	}
}

static const struct of_device_id restart_of_match[] = {
	{ .compatible = "yamaha,rtx830-restart" },
	{ }
};
MODULE_DEVICE_TABLE(of, restart_of_match);
static struct platform_driver rtx830_restart_driver = {
	.probe = rtx830_restart_probe,
	.remove = rtx830_restart_remove,
	.driver = {
		.name = "rtx830-restart",
		.of_match_table = restart_of_match,
	},
};
module_platform_driver(rtx830_restart_driver);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RTX830 guarded Cortex-A9 private-watchdog restart, init read-only");
