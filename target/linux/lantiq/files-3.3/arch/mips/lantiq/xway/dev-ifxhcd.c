/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2012 John Crispin <blogic@openwrt.org>
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

static u64 dmamask = (u32)0x1fffffff;

static struct platform_device platform_dev = {
	.name = "ifxusb_hcd",
	.dev.dma_mask = &dmamask,
};

int __init
xway_register_hcd(int *pins)
{
	platform_dev.dev.platform_data = pins;
	return platform_device_register(&platform_dev);
}
