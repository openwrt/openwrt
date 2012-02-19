/*
 *  Bus Glue for the built-in EHCI controller of the Ralink RT3662/RT3883 SoCs
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Parts of this file are based on Ralink's 2.6.21 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_ehci_platform.h>

static int ehci_rt3883_init(struct usb_hcd *hcd)
{
	struct ehci_hcd *ehci = hcd_to_ehci(hcd);
	int ret;

	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs +
		     HC_LENGTH(ehci, ehci_readl(ehci, &ehci->caps->hc_capbase));
	dbg_hcs_params(ehci, "reset");
	dbg_hcc_params(ehci, "reset");

	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);
	ehci->sbrn = 0x20;

	ehci_reset(ehci);

	ret = ehci_init(hcd);
	if (ret)
		return ret;

	ehci_port_power(ehci, 0);

	return 0;
}

static const struct hc_driver ehci_rt3883_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Ralink RT3883 built-in EHCI controller",
	.hcd_priv_size		= sizeof(struct ehci_hcd),
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	.reset			= ehci_rt3883_init,
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
	.relinquish_port	= ehci_relinquish_port,
	.port_handed_over	= ehci_port_handed_over,

	.clear_tt_buffer_complete = ehci_clear_tt_buffer_complete,
};

static int ehci_rt3883_probe(struct platform_device *pdev)
{
	struct rt3883_ehci_platform_data *pdata;
	struct usb_hcd *hcd;
	struct resource *res;
	int irq;
	int ret;

	if (usb_disabled())
		return -ENODEV;

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

	hcd = usb_create_hcd(&ehci_rt3883_hc_driver, &pdev->dev,
			     dev_name(&pdev->dev));
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

	pdata = pdev->dev.platform_data;
	if (pdata && pdata->start_hw)
		pdata->start_hw();

	ret = usb_add_hcd(hcd, irq, IRQF_SHARED);
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

static int ehci_rt3883_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct rt3883_ehci_platform_data *pdata;

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);

	pdata = pdev->dev.platform_data;
	if (pdata && pdata->stop_hw)
		pdata->stop_hw();

	return 0;
}

static struct platform_driver ehci_rt3883_driver = {
	.probe		= ehci_rt3883_probe,
	.remove		= ehci_rt3883_remove,
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "rt3883-ehci",
	}
};

MODULE_ALIAS("platform:rt3883-ehci");
