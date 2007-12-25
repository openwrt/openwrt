/*
 *   arch/mips/ifxmips/setup.c
 *
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
 *
 *   Rewrite of Infineon IFXMips code, thanks to infineon for the support,
 *   software and hardware
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/init.h>

#include <asm/time.h>
#include <asm/traps.h>
#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/ifxmips/ifxmips_pmu.h>

static unsigned int r4k_offset; /* Amount to increment compare reg each time */
static unsigned int r4k_cur;    /* What counter should be at next timer irq */

extern void ifxmips_reboot_setup (void);
void prom_printf (const char * fmt, ...);

void
__init bus_error_init (void)
{
		/* nothing yet */
}

unsigned int
ifxmips_get_ddr_hz (void)
{
	switch (readl(IFXMIPS_CGU_SYS) & 0x3)
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
ifxmips_get_cpu_hz (void)
{
	unsigned int ddr_clock = ifxmips_get_ddr_hz();
	switch (readl(IFXMIPS_CGU_SYS) & 0xc)
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
ifxmips_get_fpi_hz (void)
{
	unsigned int ddr_clock = ifxmips_get_ddr_hz();
	if (readl(IFXMIPS_CGU_SYS) & 0x40)
	{
		return ddr_clock >> 1;
	}
	return ddr_clock;
}
EXPORT_SYMBOL(ifxmips_get_fpi_hz);

unsigned int
ifxmips_get_cpu_ver (void)
{
	return readl(IFXMIPS_MCD_CHIPID) & 0xFFFFF000;
}
EXPORT_SYMBOL(ifxmips_get_cpu_ver);

void
ifxmips_time_init (void)
{
	mips_hpt_frequency = ifxmips_get_cpu_hz() / 2;
	r4k_offset = mips_hpt_frequency / HZ;
	printk("mips_hpt_frequency:%d\n", mips_hpt_frequency);
	printk("r4k_offset: %08x(%d)\n", r4k_offset, r4k_offset);
}

int
ifxmips_be_handler(struct pt_regs *regs, int is_fixup)
{
	/*TODO*/
	printk(KERN_ERR "TODO: BUS error\n");

	return MIPS_BE_FATAL;
}

/* ISR GPTU Timer 6 for high resolution timer */
static irqreturn_t
ifxmips_timer6_interrupt(int irq, void *dev_id)
{
	timer_interrupt(IFXMIPS_TIMER6_INT, NULL);

	return IRQ_HANDLED;
}

static struct irqaction hrt_irqaction = {
	.handler = ifxmips_timer6_interrupt,
	.flags = IRQF_DISABLED,
	.name = "hrt",
};

void __init
plat_timer_setup (struct irqaction *irq)
{
	unsigned int retval;

	setup_irq(MIPS_CPU_TIMER_IRQ, irq);

	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);

	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_GPT | IFXMIPS_PMU_PWDCR_FPI);

	writel(0x100, IFXMIPS_GPTU_GPT_CLC);

	writel(0xffff, IFXMIPS_GPTU_GPT_CAPREL);
	writel(0x80C0, IFXMIPS_GPTU_GPT_T6CON);

	//retval = setup_irq(IFXMIPS_TIMER6_INT, &hrt_irqaction);

	if (retval)
	{
		prom_printf("reqeust_irq failed %d. HIGH_RES_TIMER is diabled\n", IFXMIPS_TIMER6_INT);
	}
}

extern const char* get_system_type (void);

void __init
plat_mem_setup (void)
{
	u32 status;
	prom_printf("This %s has a cpu rev of 0x%X\n", get_system_type(), ifxmips_get_cpu_ver());

	//TODO WHY ???
	/* clear RE bit*/
	status = read_c0_status();
	status &= (~(1<<25));
	write_c0_status(status);

	ifxmips_reboot_setup();
	board_time_init = ifxmips_time_init;
	board_be_handler = &ifxmips_be_handler;

	ioport_resource.start = IOPORT_RESOURCE_START;
	ioport_resource.end = IOPORT_RESOURCE_END;
	iomem_resource.start = IOMEM_RESOURCE_START;
	iomem_resource.end = IOMEM_RESOURCE_END;
}
