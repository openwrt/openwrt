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

extern void prom_printf(const char * fmt, ...);
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

/* this doesn't really belong here, but it's a convenient location */
unsigned int amazon_get_cpu_ver(void)
{
	static unsigned int cpu_ver = 0;
	if (cpu_ver == 0)
		cpu_ver = amazon_readl(AMAZON_MCD_CHIPID) & 0xFFFFF000;
	return cpu_ver;
}

void amazon_time_init(void)
{
	mips_hpt_frequency = amazon_get_cpu_hz()/2;
	printk("mips_hpt_frequency:%d\n", mips_hpt_frequency);
}

extern int hr_time_resolution;

/* ISR GPTU Timer 6 for high resolution timer */
static void amazon_timer6_interrupt(int irq, void *dev_id)
{
	timer_interrupt(AMAZON_TIMER6_INT, NULL);
}

static struct irqaction hrt_irqaction = {
	.handler = amazon_timer6_interrupt,
	.flags = SA_INTERRUPT,
	.name = "hrt",
};

/*
 * THe CPU counter for System timer, set to HZ
 * GPTU Timer 6 for high resolution timer, set to hr_time_resolution
 * Also misuse this routine to print out the CPU type and clock.
 */
void __init plat_timer_setup(struct irqaction *irq)
{
	/* cpu counter for timer interrupts */
	setup_irq(MIPS_CPU_TIMER_IRQ, irq);

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
		prom_printf("Yangtse Version\n");	
	} else if (part_no == AMAZON_CHIPID_STANDARD) {
		prom_printf(SYSTEM_MODEL_NAME "\n");
	} else {
		prom_printf("unknown version %8x\n",part_no);
	}
	
	amazon_reboot_setup();
	board_time_init = amazon_time_init;

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
