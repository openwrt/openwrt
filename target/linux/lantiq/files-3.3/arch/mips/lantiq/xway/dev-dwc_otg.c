/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>

#include <lantiq_soc.h>
#include <lantiq_irq.h>
#include <lantiq_platform.h>

#define LTQ_USB_IOMEM_BASE 0x1e101000
#define LTQ_USB_IOMEM_SIZE 0x00001000

static struct resource resources[] =
{
	[0] = {
		.name	= "dwc_otg_membase",
		.start	= LTQ_USB_IOMEM_BASE,
		.end	= LTQ_USB_IOMEM_BASE + LTQ_USB_IOMEM_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.name	= "dwc_otg_irq",
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 dwc_dmamask = (u32)0x1fffffff;

static struct platform_device platform_dev = {
	.name = "dwc_otg",
	.dev = {
		.dma_mask	= &dwc_dmamask,
	},
	.resource		= resources,
	.num_resources		= ARRAY_SIZE(resources),
};

int __init
xway_register_dwc(int pin)
{
	struct irq_data d;
	d.irq = resources[1].start;
	ltq_enable_irq(&d);
	resources[1].start = ltq_is_ase() ? LTQ_USB_ASE_INT : LTQ_USB_INT;
	platform_dev.dev.platform_data = (void*) pin;
	return platform_device_register(&platform_dev);
}
