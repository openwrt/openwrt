/*
 *	Copyright (C) ADMtek Incorporated.
 *		Creator : daniell@admtek.com.tw
 *	Copyright 1999, 2000 MIPS Technologies, Inc.
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <asm/reboot.h>
#include <asm/io.h>
#include <asm/time.h>

#include <adm5120_info.h>

#define ADM5120_SOFTRESET	0x12000004
#define STATUS_IE		0x00000001
#define ALLINTS (IE_IRQ0 | IE_IRQ5 | STATUS_IE)

void  mips_time_init(void);

extern unsigned int mips_counter_frequency;

void adm5120_restart(char *command)
{
	*(u32*)KSEG1ADDR(ADM5120_SOFTRESET)=1;
}


void adm5120_halt(void)
{
        printk(KERN_NOTICE "\n** You can safely turn off the power\n");
        while (1);
}


void adm5120_power_off(void)
{
        adm5120_halt();
}

void __init adm5120_time_init(void)
{
	mips_counter_frequency = adm5120_info.cpu_speed >> 1;
}

void __init plat_timer_setup(struct irqaction *irq)
{
	/* to generate the first timer interrupt */
	write_c0_compare(read_c0_count()+ mips_counter_frequency/HZ);
	clear_c0_status(ST0_BEV);
	set_c0_status(ALLINTS);
}

void __init plat_mem_setup(void)
{
	printk(KERN_INFO "ADM5120 board setup\n");

	board_time_init = adm5120_time_init;
	//board_timer_setup = mips_timer_setup;

	_machine_restart = adm5120_restart;
	_machine_halt = adm5120_halt;
	pm_power_off = adm5120_power_off;

	set_io_port_base(KSEG1);
}

const char *get_system_type(void)
{
	return "ADM5120 Board";
}

static struct resource adm5120_hcd_resources[] = {
	[0] = {
		.start	= 0x11200000,
		.end	= 0x11200084,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= 0x3,
		.end	= 0x3,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device adm5120hcd_device = {
	.name		= "adm5120-hcd",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(adm5120_hcd_resources),
	.resource	= adm5120_hcd_resources,
};

static struct platform_device *devices[] __initdata = {
	&adm5120hcd_device,
};

static int __init adm5120_init(void)
{
	return platform_add_devices(devices, ARRAY_SIZE(devices));
}

subsys_initcall(adm5120_init);
