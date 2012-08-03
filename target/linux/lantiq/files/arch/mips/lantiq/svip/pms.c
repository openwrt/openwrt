/************************************************************************
 *
 * Copyright (c) 2007
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <asm/addrspace.h>

#include <base_reg.h>
#include <sys1_reg.h>
#include <sys2_reg.h>
#include <lantiq_soc.h>

static struct svip_reg_sys1 *const sys1 = (struct svip_reg_sys1 *)LTQ_SYS1_BASE;
static struct svip_reg_sys2 *const sys2 = (struct svip_reg_sys2 *)LTQ_SYS2_BASE;

void svip_sys1_clk_enable(u32 mask)
{
	sys1_w32(sys1_r32(clksr) | mask, clkenr);
	asm("sync;");
}
EXPORT_SYMBOL(svip_sys1_clk_enable);

int svip_sys1_clk_is_enabled(u32 mask)
{
	return (sys1_r32(clksr) & mask) != 0;
}
EXPORT_SYMBOL(svip_sys1_clk_is_enabled);

void svip_sys2_clk_enable(u32 mask)
{
	sys2_w32(sys2_r32(clksr) | mask, clkenr);
	asm("sync;");
}
EXPORT_SYMBOL(svip_sys2_clk_enable);

int svip_sys2_clk_is_enabled(u32 mask)
{
	return (sys2_r32(clksr) & mask) != 0;
}
EXPORT_SYMBOL(svip_sys2_clk_is_enabled);

int ltq_pms_read_procmem(char *buf, char **start, off_t offset,
			 int count, int *eof, void *data)
{
	long len = 0;
	int t = 0;
	u32 bit = 0;
	u32 reg_tmp, bits_tmp;

	len = sprintf(buf, "\nSVIP PMS Settings\n");
	len = len + sprintf(buf + len,
			    "              3         2         1         0\n");
	len = len + sprintf(buf + len,
			    "            210987654321098765432109876543210\n");
	len = len + sprintf(buf + len,
			    "---------------------------------------------\n");
	len = len + sprintf(buf + len,
			    "SYS1_CLKSR: ");
	reg_tmp = sys1_r32(clksr);
	bit = 0x80000000;
	for (t = 31; t != -1; t--) {
		bits_tmp = (reg_tmp & bit) >> t;
		len = len + sprintf(buf + len, "%d", bits_tmp);
		bit = bit >> 1;
	}
	len = len + sprintf(buf + len, "\n\n");
	len = len + sprintf(buf + len, "SYS2_CLKSR: ");
	reg_tmp = sys2_r32(clksr);
	bit = 0x80000000;
	for (t = 31; t != -1; t--) {
		bits_tmp = (reg_tmp & bit) >> t;
		len = len + sprintf(buf + len, "%d", bits_tmp);
		bit = bit >> 1;
	}
	len = len + sprintf(buf + len, "\n\n");

	*eof = 1;

	return len;
}

int __init ltq_pms_init_proc(void)
{
	return create_proc_read_entry("driver/ltq_pms", 0, NULL,
				      ltq_pms_read_procmem, NULL) == NULL;
}

module_init(ltq_pms_init_proc);
