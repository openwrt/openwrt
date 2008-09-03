/*
 * EHCI HCD (Host Controller Driver) for USB.
 *
 * Copyright (C) 2007 Atheros Communications, Inc.
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * Bus Glue for Atheros AR71xx built-in EHCI controller
 *
 */

#include <linux/platform_device.h>
#include <linux/delay.h>

extern int usb_disabled(void);

static void ar71xx_start_ehci(struct platform_device *pdev)
{
}

static void ar71xx_stop_ehci(struct platform_device *pdev)
{
	/*
	 * TODO: implement
	 */
}

int usb_ehci_ar71xx_probe(const struct hc_driver *driver,
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

	hcd = usb_create_hcd(driver, &pdev->dev, pdev->dev.bus_id);
	if (!hcd)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(&pdev->dev, "no base address specified for %s\n",
			pdev->dev.bus_id);
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

	ar71xx_start_ehci(pdev);

	ehci		= hcd_to_ehci(hcd);
	ehci->caps	= hcd->regs;
	ehci->regs	= hcd->regs + HC_LENGTH(readl(&ehci->caps->hc_capbase));
	ehci->hcs_params = readl(&ehci->caps->hcs_params);

	ret = usb_add_hcd(hcd, irq, IRQF_DISABLED | IRQF_SHARED);
	if (ret)
		goto err_stop_ehc;

	return 0;

err_stop_ehc:
	ar71xx_stop_ehci(pdev);
	iounmap(hcd->regs);

err_release_region:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
err_put_hcd:
	usb_put_hcd(hcd);
	return ret;
}

/* may be called without controller electrically present */
/* may be called with controller, bus, and devices active */

/**
 * usb_ehci_ar71xx_remove - shutdown processing for AR71xx-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_ar71xx_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
static void usb_ehci_ar71xx_remove(struct usb_hcd *hcd,
				   struct platform_device *pdev)
{
	usb_remove_hcd(hcd);
	ar71xx_stop_ehci(pdev);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

static const struct hc_driver ehci_ar71xx_hc_driver = {
	.description	= hcd_name,
	.product_desc	= "Atheros AR71xx built-in EHCI controller",
	.hcd_priv_size	= sizeof(struct ehci_hcd),
	/*
	 * generic hardware linkage
	 */
	.irq		= ehci_irq,
	.flags		= HCD_MEMORY | HCD_USB2,

	/*
	 * basic lifecycle operations
	 */
	.reset		= ehci_init,
	.start		= ehci_run,
	.stop		= ehci_stop,
	.shutdown	= ehci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue		= ehci_urb_enqueue,
	.urb_dequeue		= ehci_urb_dequeue,
	.endpoint_disable	= ehci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number = ehci_get_frame,

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

static int ehci_hcd_ar71xx_drv_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd = NULL;
	int ret;

	ret = -ENODEV;
	if (!usb_disabled())
		ret = usb_ehci_ar71xx_probe(&ehci_ar71xx_hc_driver, &hcd, pdev);

	return ret;
}

static int ehci_hcd_ar71xx_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_ehci_ar71xx_remove(hcd, pdev);
	return 0;
}

static struct platform_driver ehci_hcd_ar71xx_driver = {
	.probe		= ehci_hcd_ar71xx_drv_probe,
	.remove		= ehci_hcd_ar71xx_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver = {
		.name	= "ar71xx-ehci",
		.bus	= &platform_bus_type
	}
};

MODULE_ALIAS("ar71xx-ehci");
