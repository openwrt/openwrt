/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2004 peng.liu@infineon.com 
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */

#include <linux/init.h>

#include <asm/time.h>
#include <asm/traps.h>
#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/bootinfo.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/ifxmips/ifxmips_pmu.h>
#include <asm/ifxmips/ifxmips_prom.h>

static unsigned int r4k_offset;
static unsigned int r4k_cur;

extern void ifxmips_reboot_setup(void);

unsigned int
ifxmips_get_ddr_hz(void)
{
	switch(ifxmips_r32(IFXMIPS_CGU_SYS) & 0x3)
	{
	case 0:
		return CLOCK_167M;
	case 1:
		return CLOCK_133M;
	case 2:
		return CLOCK_111M;
	}
	return CLOCK_83M;
}
EXPORT_SYMBOL(ifxmips_get_ddr_hz);

unsigned int
ifxmips_get_cpu_hz(void)
{
	unsigned int ddr_clock = ifxmips_get_ddr_hz();
	switch(ifxmips_r32(IFXMIPS_CGU_SYS) & 0xc)
	{
	case 0:
		return CLOCK_333M;
	case 4:
		return ddr_clock;
	}
	return ddr_clock << 1;
}
EXPORT_SYMBOL(ifxmips_get_cpu_hz);

unsigned int
ifxmips_get_fpi_hz(void)
{
	unsigned int ddr_clock = ifxmips_get_ddr_hz();
	if(ifxmips_r32(IFXMIPS_CGU_SYS) & 0x40)
		return ddr_clock >> 1;
	return ddr_clock;
}
EXPORT_SYMBOL(ifxmips_get_fpi_hz);

unsigned int
ifxmips_get_cpu_ver(void)
{
	return ifxmips_r32(IFXMIPS_MCD_CHIPID) & 0xFFFFF000;
}
EXPORT_SYMBOL(ifxmips_get_cpu_ver);

static __inline__ u32
ifxmips_get_counter_resolution(void)
{
	u32 res;
	__asm__ __volatile__(
		".set   push\n"
		".set   mips32r2\n"
		".set   noreorder\n"
		"rdhwr  %0, $3\n"
		"ehb\n"
		".set pop\n"
		: "=&r" (res)
		: /* no input */
		: "memory");
		instruction_hazard();
		return res;
}

int
ifxmips_be_handler(struct pt_regs *regs, int is_fixup)
{
	/*TODO*/
	printk(KERN_ERR "TODO: BUS error\n");

	return MIPS_BE_FATAL;
}

void __init
plat_time_init(void)
{
	mips_hpt_frequency = ifxmips_get_cpu_hz() / ifxmips_get_counter_resolution();
	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_GPT | IFXMIPS_PMU_PWDCR_FPI);

	ifxmips_w32(0x100, IFXMIPS_GPTU_GPT_CLC);

	ifxmips_w32(0xffff, IFXMIPS_GPTU_GPT_CAPREL);
	ifxmips_w32(0x80C0, IFXMIPS_GPTU_GPT_T6CON);
}

void __init
plat_mem_setup(void)
{
	u32 status;
	prom_printf("This %s has a cpu rev of 0x%X\n", get_system_type(), ifxmips_get_cpu_ver());

	status = read_c0_status();
	status &= (~(1<<25));
	write_c0_status(status);

	ifxmips_reboot_setup();
	board_be_handler = &ifxmips_be_handler;

	ioport_resource.start = IOPORT_RESOURCE_START;
	ioport_resource.end = IOPORT_RESOURCE_END;
	iomem_resource.start = IOMEM_RESOURCE_START;
	iomem_resource.end = IOMEM_RESOURCE_END;
}
