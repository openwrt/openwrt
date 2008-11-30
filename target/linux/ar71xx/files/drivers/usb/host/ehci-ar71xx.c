/*
 *  Bus Glue for Atheros AR71xx built-in EHCI controller.
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
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

extern int usb_disabled(void);

static void ehci_ar71xx_setup(void)
{
	/*
	 * TODO: implement
	 */
}

static int ehci_ar71xx_probe(const struct hc_driver *driver,
			     struct usb_hcd **hcd_out,
			     struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	struct resource *res;
	int irq;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no IRQ specified for %s\n",
			pdev->dev.bus_id);
		return -ENODEV;
	}
	irq = res->start;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no base address specified for %s\n",
			pdev->dev.bus_id);
		return -ENODEV;
	}

	hcd = usb_create_hcd(driver, &pdev->dev, pdev->dev.bus_id);
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

	ehci = hcd_to_ehci(hcd);
	ehci->caps = hcd->regs;
	ehci->regs = hcd->regs +
			HC_LENGTH(ehci_readl(ehci, &ehci->caps->hc_capbase));
	ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

	ehci_ar71xx_setup();

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

	/*
	 * generic hardware linkage
	 */
	.irq			= ehci_irq,
	.flags			= HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 */
	.reset			= ehci_init,
	.start			= ehci_run,
	.stop			= ehci_stop,
	.shutdown		= ehci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number	= ehci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data	= ehci_hub_status_data,
	.hub_control		= ehci_hub_control,
#ifdef CONFIG_PM
	.hub_suspend		= ehci_hub_suspend,
	.hub_resume		= ehci_hub_resume,
#endif
};

static int ehci_ar71xx_driver_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd = NULL;

	if (usb_disabled())
		return -ENODEV;

	return ehci_ar71xx_probe(&ehci_ar71xx_hc_driver, &hcd, pdev);
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
