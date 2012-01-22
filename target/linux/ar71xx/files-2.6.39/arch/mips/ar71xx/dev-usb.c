/*
 *  Atheros AR71xx USB host device support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/platform.h>

#include "dev-usb.h"

/*
 * OHCI (USB full speed host controller)
 */
static struct resource ar71xx_ohci_resources[] = {
	[0] = {
		.start	= AR71XX_OHCI_BASE,
		.end	= AR71XX_OHCI_BASE + AR71XX_OHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AR71XX_MISC_IRQ_OHCI,
		.end	= AR71XX_MISC_IRQ_OHCI,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource ar7240_ohci_resources[] = {
	[0] = {
		.start	= AR7240_OHCI_BASE,
		.end	= AR7240_OHCI_BASE + AR7240_OHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AR71XX_CPU_IRQ_USB,
		.end	= AR71XX_CPU_IRQ_USB,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 ar71xx_ohci_dmamask = DMA_BIT_MASK(32);
static struct platform_device ar71xx_ohci_device = {
	.name		= "ar71xx-ohci",
	.id		= -1,
	.resource	= ar71xx_ohci_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_ohci_resources),
	.dev = {
		.dma_mask		= &ar71xx_ohci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
};

/*
 * EHCI (USB high/full speed host controller)
 */
static struct resource ar71xx_ehci_resources[] = {
	[0] = {
		.start	= AR71XX_EHCI_BASE,
		.end	= AR71XX_EHCI_BASE + AR71XX_EHCI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AR71XX_CPU_IRQ_USB,
		.end	= AR71XX_CPU_IRQ_USB,
		.flags	= IORESOURCE_IRQ,
	},
};

static u64 ar71xx_ehci_dmamask = DMA_BIT_MASK(32);
static struct ar71xx_ehci_platform_data ar71xx_ehci_data;

static struct platform_device ar71xx_ehci_device = {
	.name		= "ar71xx-ehci",
	.id		= -1,
	.resource	= ar71xx_ehci_resources,
	.num_resources	= ARRAY_SIZE(ar71xx_ehci_resources),
	.dev = {
		.dma_mask		= &ar71xx_ehci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data		= &ar71xx_ehci_data,
	},
};

#define AR71XX_USB_RESET_MASK \
	(RESET_MODULE_USB_HOST | RESET_MODULE_USB_PHY \
	| RESET_MODULE_USB_OHCI_DLL)

#define AR7240_USB_RESET_MASK \
	(RESET_MODULE_USB_HOST | RESET_MODULE_USB_OHCI_DLL_7240)

static void __init ar71xx_usb_setup(void)
{
	ar71xx_device_stop(AR71XX_USB_RESET_MASK);
	mdelay(1000);
	ar71xx_device_start(AR71XX_USB_RESET_MASK);

	/* Turning on the Buff and Desc swap bits */
	ar71xx_usb_ctrl_wr(USB_CTRL_REG_CONFIG, 0xf0000);

	/* WAR for HW bug. Here it adjusts the duration between two SOFS */
	ar71xx_usb_ctrl_wr(USB_CTRL_REG_FLADJ, 0x20c00);

	mdelay(900);

	platform_device_register(&ar71xx_ohci_device);
	platform_device_register(&ar71xx_ehci_device);
}

static void __init ar7240_usb_setup(void)
{
	ar71xx_device_stop(AR7240_USB_RESET_MASK);
	mdelay(1000);
	ar71xx_device_start(AR7240_USB_RESET_MASK);

	/* WAR for HW bug. Here it adjusts the duration between two SOFS */
	ar71xx_usb_ctrl_wr(USB_CTRL_REG_FLADJ, 0x3);

	ar71xx_ohci_device.resource = ar7240_ohci_resources;
	ar71xx_ohci_device.num_resources = ARRAY_SIZE(ar7240_ohci_resources);
	platform_device_register(&ar71xx_ohci_device);
}

static void __init ar7241_usb_setup(void)
{
	ar71xx_device_start(AR724X_RESET_USBSUS_OVERRIDE);
	mdelay(10);

	ar71xx_device_start(AR724X_RESET_USB_HOST);
	mdelay(10);

	ar71xx_device_start(AR724X_RESET_USB_PHY);
	mdelay(10);

	ar71xx_ehci_data.is_ar91xx = 1;
	ar71xx_ehci_device.resource = ar7240_ohci_resources;
	ar71xx_ehci_device.num_resources = ARRAY_SIZE(ar7240_ohci_resources);
	platform_device_register(&ar71xx_ehci_device);
}

static void __init ar91xx_usb_setup(void)
{
	ar71xx_device_stop(RESET_MODULE_USBSUS_OVERRIDE);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_USB_HOST);
	mdelay(10);

	ar71xx_device_start(RESET_MODULE_USB_PHY);
	mdelay(10);

	ar71xx_ehci_data.is_ar91xx = 1;
	platform_device_register(&ar71xx_ehci_device);
}

static void __init ar933x_usb_setup(void)
{
	ar71xx_device_reset_rmw(0, AR933X_RESET_USBSUS_OVERRIDE);
	mdelay(10);

	ar71xx_device_reset_rmw(AR933X_RESET_USB_HOST,
				AR933X_RESET_USBSUS_OVERRIDE);
	mdelay(10);

	ar71xx_device_reset_rmw(AR933X_RESET_USB_PHY,
				AR933X_RESET_USBSUS_OVERRIDE);
	mdelay(10);

	ar71xx_ehci_data.is_ar91xx = 1;
	platform_device_register(&ar71xx_ehci_device);
}

static void __init ar934x_usb_setup(void)
{
	u32 bootstrap;

	bootstrap = ar71xx_reset_rr(AR934X_RESET_REG_BOOTSTRAP);
	if (bootstrap & AR934X_BOOTSTRAP_USB_MODE_DEVICE)
		return;

	ar71xx_device_stop(AR934X_RESET_USBSUS_OVERRIDE);
	udelay(1000);

	ar71xx_device_start(AR934X_RESET_USB_PHY);
	udelay(1000);

	ar71xx_device_start(AR934X_RESET_USB_PHY_ANALOG);
	udelay(1000);

	ar71xx_device_start(AR934X_RESET_USB_HOST);
	udelay(1000);

	ar71xx_ehci_data.is_ar91xx = 1;
	platform_device_register(&ar71xx_ehci_device);
}

void __init ar71xx_add_device_usb(void)
{
	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7240:
		ar7240_usb_setup();
		break;

	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		ar7241_usb_setup();
		break;

	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
		ar71xx_usb_setup();
		break;

	case AR71XX_SOC_AR9130:
	case AR71XX_SOC_AR9132:
		ar91xx_usb_setup();
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
		ar933x_usb_setup();
		break;

	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		ar934x_usb_setup();
		break;

	default:
		BUG();
	}
}
