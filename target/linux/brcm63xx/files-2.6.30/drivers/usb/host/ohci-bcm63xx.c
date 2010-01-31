/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2008 Maxime Bizon <mbizon@freebox.fr>
 */

#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <bcm63xx_cpu.h>
#include <bcm63xx_regs.h>
#include <bcm63xx_io.h>

static struct clk *usb_host_clock;

static int __devinit ohci_bcm63xx_start(struct usb_hcd *hcd)
{
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);
	int ret;

	ret = ohci_init(ohci);
	if (ret < 0)
		return ret;

	/* FIXME: autodetected port 2 is shared with USB slave */

	ret = ohci_run(ohci);
	if (ret < 0) {
		err("can't start %s", hcd->self.bus_name);
		ohci_stop(hcd);
		return ret;
	}
	return 0;
}

static const struct hc_driver ohci_bcm63xx_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"BCM63XX integrated OHCI controller",
	.hcd_priv_size =	sizeof(struct ohci_hcd),

	.irq =			ohci_irq,
	.flags =		HCD_USB11 | HCD_MEMORY,
	.start =		ohci_bcm63xx_start,
	.stop =			ohci_stop,
	.shutdown =		ohci_shutdown,
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,
	.get_frame_number =	ohci_get_frame,
	.hub_status_data =	ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
	.start_port_reset =	ohci_start_port_reset,
};

static int __devinit ohci_hcd_bcm63xx_drv_probe(struct platform_device *pdev)
{
	struct resource *res_mem, *res_irq;
	struct usb_hcd *hcd;
	struct ohci_hcd *ohci;
	u32 reg;
	int ret;

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_mem || !res_irq)
		return -ENODEV;

	if (BCMCPU_IS_6348()) {
		struct clk *clk;
		/* enable USB host clock */
		clk = clk_get(&pdev->dev, "usbh");
		if (IS_ERR(clk))
			return -ENODEV;

		clk_enable(clk);
		usb_host_clock = clk;
		bcm_rset_writel(RSET_OHCI_PRIV, 0, OHCI_PRIV_REG);

	} else if (BCMCPU_IS_6358()) {
		reg = bcm_rset_readl(RSET_USBH_PRIV, USBH_PRIV_SWAP_REG);
		reg &= ~USBH_PRIV_SWAP_OHCI_ENDN_MASK;
		reg |= USBH_PRIV_SWAP_OHCI_DATA_MASK;
		bcm_rset_writel(RSET_USBH_PRIV, reg, USBH_PRIV_SWAP_REG);
		/* don't ask... */
		bcm_rset_writel(RSET_USBH_PRIV, 0x1c0020, USBH_PRIV_TEST_REG);
	} else
		return 0;

	hcd = usb_create_hcd(&ohci_bcm63xx_hc_driver, &pdev->dev, "bcm63xx");
	if (!hcd)
		return -ENOMEM;
	hcd->rsrc_start = res_mem->start;
	hcd->rsrc_len = res_mem->end - res_mem->start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed\n");
		ret = -EBUSY;
		goto out;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed\n");
		ret = -EIO;
		goto out1;
	}

	ohci = hcd_to_ohci(hcd);
	ohci->flags |= OHCI_QUIRK_BE_MMIO | OHCI_QUIRK_BE_DESC |
		OHCI_QUIRK_FRAME_NO;
	ohci_hcd_init(ohci);

	ret = usb_add_hcd(hcd, res_irq->start, IRQF_DISABLED);
	if (ret)
		goto out2;

	platform_set_drvdata(pdev, hcd);
	return 0;

out2:
	iounmap(hcd->regs);
out1:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
out:
	usb_put_hcd(hcd);
	return ret;
}

static int __devexit ohci_hcd_bcm63xx_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd;

	hcd = platform_get_drvdata(pdev);
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	usb_put_hcd(hcd);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	if (usb_host_clock) {
		clk_disable(usb_host_clock);
		clk_put(usb_host_clock);
	}
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver ohci_hcd_bcm63xx_driver = {
	.probe		= ohci_hcd_bcm63xx_drv_probe,
	.remove		= __devexit_p(ohci_hcd_bcm63xx_drv_remove),
	.shutdown	= usb_hcd_platform_shutdown,
	.driver		= {
		.name	= "bcm63xx_ohci",
		.owner	= THIS_MODULE,
		.bus	= &platform_bus_type
	},
};

MODULE_ALIAS("platform:bcm63xx_ohci");
