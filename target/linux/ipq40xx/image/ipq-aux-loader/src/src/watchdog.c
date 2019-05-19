/*
 * Auxiliary kernel loader for Qualcom IPQ-4XXX/806X based boards
 *
 * Copyright (C) 2019 Sergey Sergeev <adron@mstnt.com>
 *
 * Some structures and code has been taken from the U-Boot project.
 *	(C) Copyright 2008 Semihalf
 *	(C) Copyright 2000-2005
 *	Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <io.h>

#define WDT_RESETUP_PERIOD 10

#ifdef CONFIG_IPQ4XXX
#define WDT_BASE 		0xB017000
#define WDT_RST    (WDT_BASE + 0x4)
#define WDT_EN    (WDT_BASE + 0x8)
#define WDT_STS (WDT_BASE + 0xc)
#define WDT_BARK_TIME    (WDT_BASE + 0x10)
#define WDT_BITE_TIME    (WDT_BASE + 0x14)

#define GCNT_PSHOLD		0x004AB000

void watchdog_setup(int period){
	writel(0, WDT_EN);
	writel(1, WDT_RST);
	writel(period * 65536, WDT_BARK_TIME);
	writel(period * 65536, WDT_BITE_TIME);
	writel(1,  WDT_EN);
}

void reset_cpu(unsigned long addr)
{
	/* clear ps-hold bit to reset the soc */
	writel(0, GCNT_PSHOLD);
	while (1);
}

#endif /* CONFIG_IPQ4XXX */

#ifdef CONFIG_IPQ806X
#define MSM_CLK_CTL_BASE    0x00900000

#define MSM_TMR_BASE        0x0200A000
#define MSM_GPT_BASE        (MSM_TMR_BASE + 0x04)
#define MSM_DGT_BASE        (MSM_TMR_BASE + 0x24)

#define GPT_REG(off)        (MSM_GPT_BASE + (off))
#define DGT_REG(off)        (MSM_DGT_BASE + (off))

#define APCS_WDT0_EN        (MSM_TMR_BASE + 0x0040)
#define APCS_WDT0_RST       (MSM_TMR_BASE + 0x0038)
#define APCS_WDT0_BARK_TIME (MSM_TMR_BASE + 0x004C)
#define APCS_WDT0_BITE_TIME (MSM_TMR_BASE + 0x005C)

#define APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE (MSM_CLK_CTL_BASE + 0x3820)

/* Watchdog bite time set to default reset value */
#define RESET_WDT_BITE_TIME 0x31F3

/* Watchdog bark time value is ketp larger than the watchdog timeout
 * of 0x31F3, effectively disabling the watchdog bark interrupt
 */
#define RESET_WDT_BARK_TIME (5 * RESET_WDT_BITE_TIME)

void watchdog_setup(int period){
	writel(0, APCS_WDT0_EN);
	writel(1, APCS_WDT0_RST);
	writel(period * 32768, APCS_WDT0_BARK_TIME);
	writel(period * 32768, APCS_WDT0_BITE_TIME);
	writel(1, APCS_WDT0_EN);
	writel(1, APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE);
}

void reset_cpu(unsigned long addr)
{
	writel(0, APCS_WDT0_EN);
	writel(1, APCS_WDT0_RST);
	writel(RESET_WDT_BARK_TIME, APCS_WDT0_BARK_TIME);
	writel(RESET_WDT_BITE_TIME, APCS_WDT0_BITE_TIME);
	writel(1, APCS_WDT0_EN);
	writel(1, APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE);

	for(;;);
}
#endif /* CONFIG_IPQ806X */

void watchdog_resetup(void){
	watchdog_setup(WDT_RESETUP_PERIOD);
}
