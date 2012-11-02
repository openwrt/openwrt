/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/time.h>
#include <asm/bootinfo.h>

#include <lantiq_soc.h>

#include "../prom.h"
#include "../clk.h"
#include "../machtypes.h"

#include <base_reg.h>
#include <ebu_reg.h>

#define SOC_SVIP		"SVIP"

#define PART_SHIFT	12
#define PART_MASK	0x0FFFF000
#define REV_SHIFT	28
#define REV_MASK	0xF0000000

static struct svip_reg_ebu *const ebu = (struct svip_reg_ebu *)LTQ_EBU_BASE;

void __init ltq_soc_init(void)
{
	clkdev_add_static(ltq_svip_cpu_hz(), ltq_svip_fpi_hz(),
		ltq_svip_io_region_clock());
}

void __init
ltq_soc_setup(void)
{
	if (mips_machtype == LANTIQ_MACH_EASY33016 ||
	    mips_machtype == LANTIQ_MACH_EASY336) {
		ebu_w32(0x120000f1, addr_sel_2);
		ebu_w32(LTQ_EBU_CON_0_ADSWP |
			LTQ_EBU_CON_0_SETUP |
			LTQ_EBU_CON_0_BCGEN_VAL(0x02) |
			LTQ_EBU_CON_0_WAITWRC_VAL(7) |
			LTQ_EBU_CON_0_WAITRDC_VAL(3) |
			LTQ_EBU_CON_0_HOLDC_VAL(3) |
			LTQ_EBU_CON_0_RECOVC_VAL(3) |
			LTQ_EBU_CON_0_CMULT_VAL(3), con_2);
	}
}

void __init
ltq_soc_detect(struct ltq_soc_info *i)
{
	i->partnum = (ltq_r32(LTQ_STATUS_CHIPID) & PART_MASK) >> PART_SHIFT;
	i->rev = (ltq_r32(LTQ_STATUS_CHIPID) & REV_MASK) >> REV_SHIFT;
	sprintf(i->rev_type, "1.%d", i->rev);
	switch (i->partnum) {
	case SOC_ID_SVIP:
		i->name = SOC_SVIP;
		i->type = SOC_TYPE_SVIP;
		break;

	default:
		printk(KERN_ERR "unknown partnum : 0x%08X\n", i->partnum);
		while (1);
		break;
	}
}
