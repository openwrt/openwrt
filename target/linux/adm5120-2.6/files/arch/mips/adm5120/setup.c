/*
 *	Copyright (C) ADMtek Incorporated.
 *		Creator : daniell@admtek.com.tw
 *	Copyright 1999, 2000 MIPS Technologies, Inc.
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 *	Copyright (C) 2007 OpenWrt.org
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <asm/reboot.h>
#include <asm/io.h>
#include <asm/time.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_irq.h>

extern void adm5120_time_init(void) __init;

#define ADM5120_SOFTRESET	0x12000004

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

void __init plat_mem_setup(void)
{
	printk(KERN_INFO "ADM5120 board setup\n");

	board_time_init = adm5120_time_init;

	_machine_restart = adm5120_restart;
	_machine_halt = adm5120_halt;
	pm_power_off = adm5120_power_off;

	set_io_port_base(KSEG1);
}

const char *get_system_type(void)
{
	return adm5120_board_name();
}

static struct resource adm5120_hcd_resources[] = {
	[0] = {
		.start	= ADM5120_USBC_BASE,
		.end	= ADM5120_USBC_BASE+ADM5120_USBC_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= ADM5120_IRQ_USBC,
		.end	= ADM5120_IRQ_USBC,
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
