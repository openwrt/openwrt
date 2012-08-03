/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 * Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/pm.h>
#include <asm/reboot.h>
#include <linux/export.h>

#include <lantiq_soc.h>

/* CPU0 Reset Source Register */
#define LTQ_SYS1_CPU0RS		0x0040
/* reset cause mask */
#define LTQ_CPU0RS_MASK		0x0003

int
ltq_reset_cause(void)
{
	return ltq_sys1_r32(LTQ_SYS1_CPU0RS) & LTQ_CPU0RS_MASK;
}
EXPORT_SYMBOL_GPL(ltq_reset_cause);

#define BOOT_REG_BASE	(KSEG1 | 0x1F200000)
#define BOOT_PW1_REG	(BOOT_REG_BASE | 0x20)
#define BOOT_PW2_REG	(BOOT_REG_BASE | 0x24)
#define BOOT_PW1	0x4C545100
#define BOOT_PW2	0x0051544C

#define WDT_REG_BASE	(KSEG1 | 0x1F8803F0)
#define WDT_PW1		0x00BE0000
#define WDT_PW2		0x00DC0000

static void
ltq_machine_restart(char *command)
{
	pr_notice("System restart\n");
	local_irq_disable();

	/* reboot magic */
	ltq_w32(BOOT_PW1, (void *)BOOT_PW1_REG); /* 'LTQ\0' */
	ltq_w32(BOOT_PW2, (void *)BOOT_PW2_REG); /* '\0QTL' */
	ltq_w32(0, (void *)BOOT_REG_BASE); /* reset Bootreg RVEC */

	/* watchdog magic */
	ltq_w32(WDT_PW1, (void *)WDT_REG_BASE);
	ltq_w32(WDT_PW2 |
		(0x3 << 26) | /* PWL */
		(0x2 << 24) | /* CLKDIV */
		(0x1 << 31) | /* enable */
		(1), /* reload */
		(void *)WDT_REG_BASE);
	unreachable();
}

static void
ltq_machine_halt(void)
{
	pr_notice("System halted.\n");
	local_irq_disable();
	unreachable();
}

static void
ltq_machine_power_off(void)
{
	pr_notice("Please turn off the power now.\n");
	local_irq_disable();
	unreachable();
}

static int __init
mips_reboot_setup(void)
{
	_machine_restart = ltq_machine_restart;
	_machine_halt = ltq_machine_halt;
	pm_power_off = ltq_machine_power_off;
	return 0;
}

arch_initcall(mips_reboot_setup);
