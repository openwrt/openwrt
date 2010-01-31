/*
 *	 Copyright (C) 2004 Peng Liu <peng.liu@infineon.com>
 *	 Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 *	 Copyright (C) 2007 Felix Fietkau <nbd@openwrt.org>
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
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <asm/reboot.h>
#include <asm/system.h>
#include <asm/time.h>
#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>
#include <asm/amazon/model.h>

static unsigned int r4k_offset;
static unsigned int r4k_cur;

/* required in arch/mips/kernel/kspd.c */
unsigned long cpu_khz;

static void amazon_reboot_setup(void);

/* the CPU clock rate - lifted from u-boot */
unsigned int amazon_get_cpu_hz(void)
{
	/*-----------------------------------*/
	/**CGU CPU Clock Reduction Register***/ 
	/*-----------------------------------*/
	switch(amazon_readl(AMAZON_CGU_CPUCRD) & 0x3){
		case 0:
			/*divider ration 1/1, 235 MHz clock */
			return 235000000;
		case 1:
			/*divider ration 2/3, 235 MHz clock, clock not accurate, here */
			return 150000000;
		case 2:
			/*divider ration 1/2, 235 MHz clock */
			return 117500000;
		default:
			/*divider ration 1/4, 235 MHz clock */
			return 58750000;
	}
}

/* the FPI clock rate - lifted from u-boot */
unsigned int amazon_get_fpi_hz(void)
{
	unsigned int  clkCPU;
	clkCPU = amazon_get_cpu_hz();

	/*-------------------------------------*/
	/***CGU Clock Divider Select Register***/
	/*-------------------------------------*/
	switch (amazon_readl(AMAZON_CGU_DIV) & 0x3)
	{
		case 1:
			return clkCPU >> 1;
		case 2:
			return clkCPU >> 2;
		default:
			return clkCPU;
		/* '11' is reserved */
	}
}
EXPORT_SYMBOL(amazon_get_fpi_hz);

/* this doesn't really belong here, but it's a convenient location */
unsigned int amazon_get_cpu_ver(void)
{
	static unsigned int cpu_ver = 0;
	if (cpu_ver == 0)
		cpu_ver = amazon_readl(AMAZON_MCD_CHIPID) & 0xFFFFF000;
	return cpu_ver;
}

static inline u32 amazon_get_counter_resolution(void)
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
	mips_hpt_frequency = amazon_get_cpu_hz() / amazon_get_counter_resolution();
	r4k_offset = mips_hpt_frequency / HZ;
	printk("mips_hpt_frequency:%d\n", mips_hpt_frequency);
	printk("r4k_offset: %08x(%d)\n", r4k_offset, r4k_offset);

	r4k_cur = (read_c0_count() + r4k_offset);
	write_c0_compare(r4k_cur);

	/* enable the timer in the PMU */
	amazon_writel(amazon_readl(AMAZON_PMU_PWDCR)| AMAZON_PMU_PWDCR_GPT|AMAZON_PMU_PWDCR_FPI, AMAZON_PMU_PWDCR);
	
	/* setup the GPTU for timer tick  f_fpi == f_gptu*/
	amazon_writel(0x0100, AMAZON_GPTU_CLC);
	amazon_writel(0xffff, AMAZON_GPTU_CAPREL);
	amazon_writel(0x80C0, AMAZON_GPTU_T6CON);
}

void __init plat_mem_setup(void)
{	
	u32 chipid = 0;
	u32 part_no = 0;
	
	chipid = amazon_readl(AMAZON_MCD_CHIPID);
	part_no = AMAZON_MCD_CHIPID_PART_NUMBER_GET(chipid);
	
	if(part_no == AMAZON_CHIPID_YANGTSE){
		printk("Yangtse Version\n");	
	} else if (part_no == AMAZON_CHIPID_STANDARD) {
		printk(SYSTEM_MODEL_NAME "\n");
	} else {
		printk("unknown version %8x\n",part_no);
	}
	
	amazon_reboot_setup();

	//stop reset TPE and DFE
	amazon_writel(0, AMAZON_RST_REQ);
	//clock
	amazon_writel(0x3fff, AMAZON_PMU_PWDCR);
	//reenable trace capability
	part_no = readl(AMAZON_BCU_ECON);

	ioport_resource.start = IOPORT_RESOURCE_START;
	ioport_resource.end = IOPORT_RESOURCE_END;
	iomem_resource.start = IOMEM_RESOURCE_START;
	iomem_resource.end = IOMEM_RESOURCE_END;
}

static void amazon_machine_restart(char *command)
{
    local_irq_disable();
    amazon_writel(AMAZON_RST_ALL, AMAZON_RST_REQ);
    for (;;) ;
}

static void amazon_machine_halt(void)
{
    printk(KERN_NOTICE "System halted.\n");
    local_irq_disable();
    for (;;) ;
}

static void amazon_machine_power_off(void)
{
	printk(KERN_NOTICE "Please turn off the power now.\n");
    local_irq_disable();
    for (;;) ;
}

static void amazon_reboot_setup(void)
{
	_machine_restart = amazon_machine_restart;
	_machine_halt = amazon_machine_halt;
	pm_power_off = amazon_machine_power_off;
}
