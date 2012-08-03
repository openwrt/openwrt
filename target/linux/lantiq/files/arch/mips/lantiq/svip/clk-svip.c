/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>

#include <asm/irq.h>
#include <asm/div64.h>

#include <lantiq_soc.h>
#include <base_reg.h>
#include <sys0_reg.h>
#include <sys1_reg.h>
#include <status_reg.h>

static struct svip_reg_status *const status =
(struct svip_reg_status *)LTQ_STATUS_BASE;
static struct svip_reg_sys0 *const sys0 = (struct svip_reg_sys0 *)LTQ_SYS0_BASE;
static struct svip_reg_sys1 *const sys1 = (struct svip_reg_sys1 *)LTQ_SYS1_BASE;

unsigned int ltq_svip_io_region_clock(void)
{
	return 200000000; /* 200 MHz */
}
EXPORT_SYMBOL(ltq_svip_io_region_clock);

unsigned int ltq_svip_cpu_hz(void)
{
	/* Magic BootROM speed location... */
	if ((*(u32 *)0x9fc07ff0) == 1)
		return *(u32 *)0x9fc07ff4;

	if (STATUS_CONFIG_CLK_MODE_GET(status_r32(config)) == 1) {
		/* xT16 */
		return 393216000;
	} else {
		switch (SYS0_PLL1CR_PLLDIV_GET(sys0_r32(pll1cr))) {
		case 3:
			return 475000000;
		case 2:
			return 450000000;
		case 1:
			return 425000000;
		default:
			return 400000000;
		}
	}
}
EXPORT_SYMBOL(ltq_svip_cpu_hz);

unsigned int ltq_svip_fpi_hz(void)
{
	u32 fbs0_div[2] = {4, 8};
	u32 div;

	div = SYS1_FPICR_FPIDIV_GET(sys1_r32(fpicr));
	return ltq_svip_cpu_hz()/fbs0_div[div];
}
EXPORT_SYMBOL(ltq_svip_fpi_hz);

unsigned int ltq_get_ppl_hz(void)
{
	/* Magic BootROM speed location... */
	if ((*(u32 *)0x9fc07ff0) == 1)
		return *(u32 *)0x9fc07ff4;

	if (STATUS_CONFIG_CLK_MODE_GET(status_r32(config)) == 1) {
		/* xT16 */
		return 393216000;
	} else {
		switch (SYS0_PLL1CR_PLLDIV_GET(sys0_r32(pll1cr))) {
		case 3:
			return 475000000;
		case 2:
			return 450000000;
		case 1:
			return 425000000;
		default:
			return 400000000;
		}
	}
}

unsigned int ltq_get_fbs0_hz(void)
{
	u32 fbs0_div[2] = {4, 8};
	u32 div;

	div = SYS1_FPICR_FPIDIV_GET(sys1_r32(fpicr));
	return ltq_get_ppl_hz()/fbs0_div[div];
}
EXPORT_SYMBOL(ltq_get_fbs0_hz);
