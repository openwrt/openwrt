// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380 USB OTG Wrapper Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>

// UsbOtgReg.IntrStatus
#define USB_OTG_INTR_STATUS			0x0600
#define USB_OTG_INTR_STATUS_ALL			0x00001fff
// UsbOtgReg.IntrMask
#define USB_OTG_INTR_MASK			0x0604
#define USB_OTG_INTR_MASK_OTG			BIT(0)
// UsbOtgReg.ChIntStMask
#define USB_OTG_CH_INTST_MASK_BASE		0x0650
#define USB_OTG_CH_INTST_MASK_STRIDE		0x0004
#define USB_OTG_CH_INTST_MASK_ALL		0x000003ff
#define USB_OTG_NUM_CHANNELS			16

// UsbOtgReg.UTMIControl1
#define USB_OTG_UTMI_CTL_1			0x06ac
#define USB_OTG_UTMI_CTL_1_DISABLE_SRP		BIT(29)
#define USB_OTG_UTMI_CTL_1_HOSTB_DEV		BIT(26)
#define USB_OTG_UTMI_CTL_1_PHY_PWDNB		BIT(25)
#define USB_OTG_UTMI_CTL_1_UTMI_PWDNB		BIT(24)
#define USB_OTG_UTMI_CTL_1_SOFT_RESETB		BIT(23)
#define USB_OTG_UTMI_CTL_1_DISCON_PHY		BIT(21)
#define USB_OTG_UTMI_CTL_1_HOST_MODE		(USB_OTG_UTMI_CTL_1_DISABLE_SRP | USB_OTG_UTMI_CTL_1_PHY_PWDNB | USB_OTG_UTMI_CTL_1_UTMI_PWDNB | USB_OTG_UTMI_CTL_1_SOFT_RESETB)

// UsbOtgReg.PllControl1
#define USB_OTG_PLL_CTL_1			0x06b4
#define USB_OTG_PLL_CTL_1_PLL_RESET		BIT(9)
#define USB_OTG_PLL_CTL_1_PLL_PWRDNB		BIT(3)
#define USB_OTG_PLL_CTL_1_XTAL_PWRDNB		BIT(2)

// UsbOtgReg.USBControl
#define USB_OTG_USB_CTL			0x06b8
#define USB_OTG_USB_CTL_USB_PWR_ON		BIT(0)

struct bcm3380_usb {
	struct device *dev;
	void __iomem *base;
	struct clk_bulk_data *clks;
	int num_clks;
	struct reset_control *reset;
};

static void usb_disable_clks(void *data)
{
	struct bcm3380_usb *usb = data;

	clk_bulk_disable_unprepare(usb->num_clks, usb->clks);
}

static int usb_init_host(struct bcm3380_usb *usb)
{
	struct device *dev = usb->dev;
	unsigned int i;
	u32 val;

	val = readl_be(usb->base + USB_OTG_PLL_CTL_1);
	val &= ~USB_OTG_PLL_CTL_1_PLL_RESET;
	val |= USB_OTG_PLL_CTL_1_PLL_PWRDNB | USB_OTG_PLL_CTL_1_XTAL_PWRDNB;
	writel_be(val, usb->base + USB_OTG_PLL_CTL_1);
	usleep_range(1000, 2000);

	val = USB_OTG_UTMI_CTL_1_HOST_MODE & ~USB_OTG_UTMI_CTL_1_SOFT_RESETB;
	writel_be(val, usb->base + USB_OTG_UTMI_CTL_1);
	udelay(10);

	val |= USB_OTG_UTMI_CTL_1_SOFT_RESETB;
	writel_be(val, usb->base + USB_OTG_UTMI_CTL_1);
	usleep_range(1000, 2000);

	val = readl_be(usb->base + USB_OTG_USB_CTL);
	val |= USB_OTG_USB_CTL_USB_PWR_ON;
	writel_be(val, usb->base + USB_OTG_USB_CTL);

	for (i = 0; i < USB_OTG_NUM_CHANNELS; i++) {
		void __iomem *ch_intst_mask = usb->base + USB_OTG_CH_INTST_MASK_BASE + i * USB_OTG_CH_INTST_MASK_STRIDE;

		writel_be(readl_be(ch_intst_mask) | USB_OTG_CH_INTST_MASK_ALL,
			  ch_intst_mask);
	}

	writel_be(USB_OTG_INTR_STATUS_ALL, usb->base + USB_OTG_INTR_STATUS);
	writel_be(readl_be(usb->base + USB_OTG_INTR_MASK) | USB_OTG_INTR_MASK_OTG,
		  usb->base + USB_OTG_INTR_MASK);

	dev_info(dev, "configured USB OTG wrapper for host mode: utmi=0x%08x pll=0x%08x ctl=0x%08x\n",
		 readl_be(usb->base + USB_OTG_UTMI_CTL_1),
		 readl_be(usb->base + USB_OTG_PLL_CTL_1),
		 readl_be(usb->base + USB_OTG_USB_CTL));

	return 0;
}

static int usb_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3380_usb *usb;
	int ret;

	usb = devm_kzalloc(dev, sizeof(*usb), GFP_KERNEL);
	if (!usb)
		return -ENOMEM;

	usb->dev = dev;
	platform_set_drvdata(pdev, usb);

	usb->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(usb->base))
		return PTR_ERR(usb->base);

	usb->num_clks = devm_clk_bulk_get_all(dev, &usb->clks);
	if (usb->num_clks < 0)
		return dev_err_probe(dev, usb->num_clks, "failed to get clocks\n");

	ret = clk_bulk_prepare_enable(usb->num_clks, usb->clks);
	if (ret)
		return dev_err_probe(dev, ret, "failed to enable clocks\n");

	ret = devm_add_action_or_reset(dev, usb_disable_clks, usb);
	if (ret)
		return ret;

	usb->reset = devm_reset_control_get_optional_shared(dev, NULL);
	if (IS_ERR(usb->reset))
		return dev_err_probe(dev, PTR_ERR(usb->reset), "failed to get reset\n");

	ret = reset_control_deassert(usb->reset);
	if (ret)
		return dev_err_probe(dev, ret, "failed to deassert reset\n");

	ret = usb_init_host(usb);
	if (ret)
		return ret;

	ret = devm_of_platform_populate(dev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to populate DWC2 child\n");

	return 0;
}

static const struct of_device_id usb_of_match[] = {
	{ .compatible = "brcm,bcm3380-usb", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, usb_of_match);

static struct platform_driver usb_driver = {
	.probe = usb_probe,
	.driver = {
		.name = "bcm3380-usb",
		.of_match_table = usb_of_match,
	},
};
module_platform_driver(usb_driver);

MODULE_DESCRIPTION("BCM3380 USB OTG wrapper driver");
MODULE_LICENSE("GPL");
