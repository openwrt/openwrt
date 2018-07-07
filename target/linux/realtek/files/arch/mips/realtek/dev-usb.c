/*
 *  Realtek RLX based SoC USB Host Controller support
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  Parts of this file are based on Linux kernel of Realtek RSDK 1.3.6
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/usb/ehci_pdriver.h>
#include <linux/usb/ohci_pdriver.h>

#include <asm/addrspace.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>
#include "common.h"

static u64 realtek_ohci_dmamask = DMA_BIT_MASK(32);

static struct resource realtek_ohci_resources[] = {
	{
		.start	= REALTEK_USB_OHCI_BASE,
		.end	= REALTEK_USB_OHCI_BASE + REALTEK_USB_OHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.flags	= IORESOURCE_IRQ,
	}
};

static struct platform_device realtek_ohci_device = {
	.name			= "ohci-platform",
	.id			= -1,
	.resource		= realtek_ohci_resources,
	.num_resources		= ARRAY_SIZE(realtek_ohci_resources),
	.dev = {
		.dma_mask		= &realtek_ohci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	}
};

static u64 realtek_ehci_dmamask = DMA_BIT_MASK(32);

static struct resource realtek_ehci_resources[] = {
	{
		.start	= REALTEK_USB_EHCI_BASE,
		.end	= REALTEK_USB_EHCI_BASE + REALTEK_USB_EHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.flags	= IORESOURCE_IRQ,
	}
};

static struct usb_ehci_pdata realtek_ehci_pdata = {
	.has_tt			= 1,
};

static struct platform_device realtek_ehci_device = {
	.name			= "ehci-platform",
	.id			= -1,
	.resource		= realtek_ehci_resources,
	.num_resources		= ARRAY_SIZE(realtek_ehci_resources),
	.dev = {
		.dma_mask		= &realtek_ehci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &realtek_ehci_pdata,
	}
};

static void __init realtek_usb_register(struct platform_device *usb_dev, int irq)
{
	usb_dev->resource[1].start = usb_dev->resource[1].end = irq;

	platform_device_register(usb_dev);
}

static void __init rtl8196c_ehci_wr(u32 reg, u32 val)
{
	u32 reg_h = ((reg & 0xf0) >> 4) << 16;
	u32 reg_l = (reg & 0x0f) << 16;

	realtek_sys_write(REALTEK_SYS_REG_USB_SIE, (0x1f00 | val));

	__raw_writel((0x00300000 | reg_l), (void __iomem *) (KSEG1ADDR(REALTEK_USB_EHCI_BASE) + 0xA4));
	__raw_writel((0x00200000 | reg_l), (void __iomem *) (KSEG1ADDR(REALTEK_USB_EHCI_BASE) + 0xA4));
	__raw_writel((0x00300000 | reg_l), (void __iomem *) (KSEG1ADDR(REALTEK_USB_EHCI_BASE) + 0xA4));
	__raw_writel((0x00300000 | reg_h), (void __iomem *) (KSEG1ADDR(REALTEK_USB_EHCI_BASE) + 0xA4));
	__raw_writel((0x00200000 | reg_h), (void __iomem *) (KSEG1ADDR(REALTEK_USB_EHCI_BASE) + 0xA4));
	__raw_writel((0x00300000 | reg_h), (void __iomem *) (KSEG1ADDR(REALTEK_USB_EHCI_BASE) + 0xA4));
}

static void __init rtl8196c_usb_setup(void)
{
	u32 val;

	val = realtek_sys_read(REALTEK_SYS_REG_CLK_MANAGE);
	val |= RTL8196C_SYS_CLK_USB_HOST_EN;
	realtek_sys_write(REALTEK_SYS_REG_CLK_MANAGE, val);

	/* disable Host chirp J-K */
	rtl8196c_ehci_wr(0xf4, 0xe3);
	/* 8196C demo board: 0xE0:99, 0xE1:A8, 0xE2:98, 0xE3:C1,  0xE5:91 */
	rtl8196c_ehci_wr(0xe0, 0x99);
	rtl8196c_ehci_wr(0xe1, 0xa8);
	rtl8196c_ehci_wr(0xe2, 0x98);
	rtl8196c_ehci_wr(0xe3, 0xc1);
	rtl8196c_ehci_wr(0xe5, 0x91);

	realtek_usb_register(&realtek_ohci_device, REALTEK_CPU_IRQ(4));
	realtek_usb_register(&realtek_ehci_device, REALTEK_CPU_IRQ(4));
}

void __init realtek_register_usb(void)
{
	if (soc_is_rtl8196c())
		rtl8196c_usb_setup();
	else
		BUG();
}
