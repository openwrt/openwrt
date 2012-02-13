/*
 *  Bus Glue for the built-in OHCI controller of the Ralink RT3662/RT3883 SoCs
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
#include <asm/mach-ralink/rt3883_ohci_platform.h>

static int __devinit ohci_rt3883_start(struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci(hcd);
	int ret;

	ret = ohci_init(ohci);
	if (ret < 0)
		return ret;

	ret = ohci_run(ohci);
	if (ret < 0)
		goto err;

	return 0;

err:
	ohci_stop(hcd);
	return ret;
}

static const struct hc_driver ohci_rt3883_hc_driver = {
	.description		= hcd_name,
	.product_desc		= "Ralink RT3883 built-in OHCI controller",
	.hcd_priv_size		= sizeof(struct ohci_hcd),

	.irq			= ohci_irq,
	.flags			= HCD_USB11 | HCD_MEMORY,

	.start			= ohci_rt3883_start,
	.stop			= ohci_stop,
	.shutdown		= ohci_shutdown,

	.urb_enqueue		= ohci_urb_enqueue,
	.urb_dequeue		= ohci_urb_dequeue,
	.endpoint_disable	= ohci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number	= ohci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data	= ohci_hub_status_data,
	.hub_control		= ohci_hub_control,
	.start_port_reset	= ohci_start_port_reset,
};

static int ohci_rt3883_probe(struct platform_device *pdev)
{
	struct rt3883_ohci_platform_data *pdata;
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

	hcd = usb_create_hcd(&ohci_rt3883_hc_driver,
			     &pdev->dev, dev_name(&pdev->dev));
	if (!hcd)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no base address specified for %s\n",
			dev_name(&pdev->dev));
		ret = -ENODEV;
		goto err_put_hcd;
	}
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

	ohci_hcd_init(hcd_to_ohci(hcd));

	ret = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (ret)
		goto err_stop_hcd;

	return 0;

err_stop_hcd:
	iounmap(hcd->regs);
err_release_region:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err_put_hcd:
	usb_put_hcd(hcd);
	return ret;
}

static int ohci_rt3883_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct rt3883_ohci_platform_data *pdata;

	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);

	pdata = pdev->dev.platform_data;
	if (pdata && pdata->stop_hw)
		pdata->stop_hw();

	return 0;
}

static struct platform_driver ohci_rt3883_driver = {
	.probe		= ohci_rt3883_probe,
	.remove		= ohci_rt3883_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver		= {
		.name	= "rt3883-ohci",
		.owner	= THIS_MODULE,
	},
};

MODULE_ALIAS("platform:rt3883-ohci");
