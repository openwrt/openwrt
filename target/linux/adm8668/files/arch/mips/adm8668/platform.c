/*
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 * Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mtd/physmap.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/amba/bus.h>
#include <linux/amba/serial.h>

#include <asm/reboot.h>
#include <asm/time.h>
#include <asm/addrspace.h>
#include <asm/bootinfo.h>
#include <asm/io.h>
#include <adm8668.h>

static void adm8668_uart_set_mctrl(struct amba_device *dev,
					void __iomem *base,
					unsigned int mcrtl)
{
}

static struct amba_pl010_data adm8668_uart0_data = {
	.set_mctrl = adm8668_uart_set_mctrl,
};

static struct amba_device adm8668_uart0_device = {
	.dev = {
		.init_name	= "apb:uart0",
		.platform_data	= &adm8668_uart0_data,
	},
	.res = {
		.start		= ADM8668_UART0_BASE,
		.end		= ADM8668_UART0_BASE + 0xF,
		.flags		= IORESOURCE_MEM,
	},
	.irq = {
		INT_LVL_UART0,
		-1
	},
	.periphid = 0x0041010,
};

static struct resource eth0_resources[] = {
	{
		.start		= ADM8668_LAN_BASE,
		.end		= ADM8668_LAN_BASE + 256,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= INT_LVL_LAN,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device adm8668_eth0_device = {
	.name		= "adm8668_eth",
	.id		= 0,
	.resource	= eth0_resources,
	.num_resources	= ARRAY_SIZE(eth0_resources),
};

static struct resource eth1_resources[] = {
	{
		.start		= ADM8668_WAN_BASE,
		.end		= ADM8668_WAN_BASE + 256,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= INT_LVL_WAN,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device adm8668_eth1_device = {
	.name		= "adm8668_eth",
	.id		= 1,
	.resource	= eth1_resources,
	.num_resources	= ARRAY_SIZE(eth1_resources),
};

static struct platform_device *adm8668_devs[] = {
	&adm8668_eth0_device,
	&adm8668_eth1_device,
};

int __devinit adm8668_devs_register(void)
{
	int ret;

	ret = amba_device_register(&adm8668_uart0_device, &iomem_resource);
	if (ret)
		panic("failed to register AMBA UART");

	return platform_add_devices(adm8668_devs, ARRAY_SIZE(adm8668_devs));
}
arch_initcall(adm8668_devs_register);
