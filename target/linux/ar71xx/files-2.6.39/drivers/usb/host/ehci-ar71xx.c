/*
 *  Bus Glue for Atheros AR71xx built-in EHCI controller.
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *	Copyright (C) 2007 Atheros Communications, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/mach-ar71xx/platform.h>

extern int usb_disabled(void);

static int ehci_ar71xx_init(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int ret;

	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs +
			HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	ehci->sbrn = 0x20;
	ehci->has_synopsys_hc_bug = 1;

	ehci_reset(ehci);

	ret = ehci_init(hcd);
	if (ret)
		return ret;

	ehci_port_power(ehci, 0);

	return 0;
}

static int ehci_ar91xx_init(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int ret;

	ehci->caps = hcd->regs + 0x100;
	ehci->regs = hcd->regs + 0x100 +
			HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	hcd->has_tt = 1;
	ehci->sbrn = 0x20;

	ehci_reset(ehci);

	ret = ehci_init(hcd);
	if (ret)
		return ret;

	ehci_port_power(ehci, 0);

	return 0;
}

static int ehci_ar71xx_probe(const struct hc_driver *driver,
			     struct usb_hcd **hcd_out,
			     struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct resource *res;
	int irq;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no IRQ specified for %s\n",
			dev_name(&pdev->dev));
		return -ENODEV;
	}
	irq = res->start;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no base address specified for %s\n",
			dev_name(&pdev->dev));
		return -ENODEV;
	}

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd)
		return -ENOMEM;

	hcd->rsrc_start	= res->start;
	hcd->rsrc_len	= res->end - res->start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		dev_dbg(&pdev->dev, "controller already in use\n");
		ret = -EBUSY;
		goto err_put_hcd;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		dev_dbg(&pdev->dev, "error mapping memory\n");
		ret = -EFAULT;
		goto err_release_region;
	}

	ret = usb_add_hcd(hcd, irq, IRQF_DISABLED | IRQF_SHARED);
	if (ret)
		goto err_iounmap;

	return 0;

err_iounmap:
	iounmap(hcd->regs);

err_release_region:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err_put_hcd:
	usb_put_hcd(hcd);
	return ret;
}

static void ehci_ar71xx_remove(struct usb_hcd *hcd,
			       struct platform_device *pdev)
{
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

static const struct hc_driver ehci_ar71xx_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Atheros AR71xx built-in EHCI controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),

	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	.reset			= ehci_ar71xx_init,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,

	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.endpoint_reset		= ehci_endpoint_reset,

	.get_frame_number	= ehci_get_frame,

	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
#ifdef CONFIG_PM
	.hub_suspend		= ehci_hub_suspend,
	.hub_resume		= ehci_hub_resume,
#endif
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete = ehci_clear_tt_buffer_complete,
};

static const struct hc_driver ehci_ar91xx_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Atheros AR91xx built-in EHCI controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	.reset			= ehci_ar91xx_init,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,

	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,
	.endpoint_reset		= ehci_endpoint_reset,

	.get_frame_number	= ehci_get_frame,

	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
#ifdef CONFIG_PM
	.hub_suspend		= ehci_hub_suspend,
	.hub_resume		= ehci_hub_resume,
#endif
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete = ehci_clear_tt_buffer_complete,
};

static int ehci_ar71xx_driver_probe(struct platform_device *pdev)
{
	struct ar71xx_ehci_platform_data *pdata;
	struct usb_hcd *hcd = NULL;
	int ret;

	if (usb_disabled())
		return -ENODEV;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified for %s\n",
			dev_name(&pdev->dev));
		return -ENODEV;
	}

	if (pdata->is_ar91xx)
		ret = ehci_ar71xx_probe(&ehci_ar91xx_hc_driver, &hcd, pdev);
	else
		ret = ehci_ar71xx_probe(&ehci_ar71xx_hc_driver, &hcd, pdev);

	return ret;
}

static int ehci_ar71xx_driver_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	ehci_ar71xx_remove(hcd, pdev);
	return 0;
}

MODULE_ALIAS("platform:ar71xx-ehci");

static struct platform_driver ehci_ar71xx_driver = {
	.probe		= ehci_ar71xx_driver_probe,
	.remove		= ehci_ar71xx_driver_remove,
	.driver = {
		.name	= "ar71xx-ehci",
	}
};
