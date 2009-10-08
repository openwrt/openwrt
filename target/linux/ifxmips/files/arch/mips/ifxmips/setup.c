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
#include <linux/cpu.h>

#include <asm/time.h>
#include <asm/traps.h>
#include <asm/irq.h>
#include <asm/bootinfo.h>

#include <ifxmips.h>
#include <ifxmips_irq.h>
#include <ifxmips_pmu.h>
#include <ifxmips_cgu.h>
#include <ifxmips_prom.h>

static unsigned int r4k_offset;
static unsigned int r4k_cur;

/* required in arch/mips/kernel/kspd.c */
unsigned long cpu_khz;

extern void ifxmips_reboot_setup(void);

unsigned int ifxmips_get_cpu_ver(void)
{
	return (ifxmips_r32(IFXMIPS_MPS_CHIPID) & 0xF0000000) >> 28;
}
EXPORT_SYMBOL(ifxmips_get_cpu_ver);

static inline u32 ifxmips_get_counter_resolution(void)
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

void __init plat_time_init(void)
{
	mips_hpt_frequency = ifxmips_get_cpu_hz() / ifxmips_get_counter_resolution();
	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);

	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_GPT | IFXMIPS_PMU_PWDCR_FPI);
	ifxmips_w32(0x100, IFXMIPS_GPTU_GPT_CLC); /* set clock divider to 1 */
	cpu_khz = ifxmips_get_cpu_hz();
}

void __init plat_mem_setup(void)
{
	u32 status;
	prom_printf("This %s system has a cpu rev of %d\n", get_system_type(), ifxmips_get_cpu_ver());

	/* make sure to have no "reverse endian" for user mode! */
	status = read_c0_status();
	status &= (~(1<<25));
	write_c0_status(status);

	ifxmips_reboot_setup();

	ioport_resource.start = IOPORT_RESOURCE_START;
	ioport_resource.end = IOPORT_RESOURCE_END;
	iomem_resource.start = IOMEM_RESOURCE_START;
	iomem_resource.end = IOMEM_RESOURCE_END;
}
