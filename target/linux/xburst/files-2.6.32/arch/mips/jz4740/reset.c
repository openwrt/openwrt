/*
 * linux/arch/mips/jz4740/reset.c
 *
 * JZ4740 reset routines.
 *
 * Copyright (c) 2006-2007  Ingenic Semiconductor Inc.
 * Author: <yliu@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/reboot.h>
#include <asm/system.h>
#include <asm/mach-jz4740/regs.h>
#include <asm/mach-jz4740/timer.h>
#include <asm/mach-jz4740/jz4740.h>

void jz_restart(char *command)
{
	printk(KERN_NOTICE "Restarting after 4 ms\n");
	REG_WDT_TCSR = WDT_TCSR_PRESCALE4 | WDT_TCSR_EXT_EN;
	REG_WDT_TCNT = 0;
	REG_WDT_TDR = JZ_EXTAL/1000;   /* reset after 4ms */
	jz4740_timer_enable_watchdog();
	REG_WDT_TCER = WDT_TCER_TCEN;  /* wdt start */
	while (1);
}

void jz_halt(void)
{
	/* Put CPU to power down mode */
	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	REG_RTC_HCR = RTC_HCR_PD;

	while (1)
		__asm__(".set\tmips3\n\t"
	                "wait\n\t"
			".set\tmips0");
}

void jz_power_off(void)
{
	jz_halt();
}
