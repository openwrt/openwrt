// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Broadcom BCM3383 USB Wrapper Driver
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

// UsbCtrlRegs.Setup.Reg32
#define USB_CTRL_SETUP				0x0000
#define USB_CTRL_SETUP_SOFT_RESET		BIT(6)
#define USB_CTRL_SETUP_IOC			BIT(4)
// UsbCtrlRegs.PllCtl1.Reg32
#define USB_CTRL_PLL_CTL_1			0x0004
#define USB_CTRL_PLL_CTL_1_BCM3383		0x512750c0
// UsbCtrlRegs.SwpCtrl.Reg32
#define USB_CTRL_SWAP				0x000c
#define USB_CTRL_SWAP_BCM3383			0x00000009
// UsbCtrlRegs.UsbSimctl.Reg32
#define USB_CTRL_SIM_CTL			0x0020
#define USB_CTRL_SIM_CTL_SCB_ARB_SEL		BIT(5)

struct bcm3383_usb {
	struct device *dev;
	void __iomem *base;
	struct clk_bulk_data *clks;
	int num_clks;
	struct reset_control *reset;
};

static void usb_disable_clks(void *data)
{
	struct bcm3383_usb *usb = data;

	clk_bulk_disable_unprepare(usb->num_clks, usb->clks);
}

static int usb_init_host(struct bcm3383_usb *usb)
{
	u32 val = readl_be(usb->base + USB_CTRL_SETUP);

	val |= USB_CTRL_SETUP_SOFT_RESET;
	writel_be(val, usb->base + USB_CTRL_SETUP);
	udelay(10);

	val &= ~USB_CTRL_SETUP_SOFT_RESET;
	writel_be(val, usb->base + USB_CTRL_SETUP);
	udelay(10);

	writel_be(readl_be(usb->base + USB_CTRL_SETUP) | USB_CTRL_SETUP_IOC,
		  usb->base + USB_CTRL_SETUP);
	writel_be(readl_be(usb->base + USB_CTRL_SWAP) | USB_CTRL_SWAP_BCM3383,
		  usb->base + USB_CTRL_SWAP);
	writel_be(USB_CTRL_PLL_CTL_1_BCM3383, usb->base + USB_CTRL_PLL_CTL_1);
	writel_be(readl_be(usb->base + USB_CTRL_SIM_CTL) | USB_CTRL_SIM_CTL_SCB_ARB_SEL,
		  usb->base + USB_CTRL_SIM_CTL);

	dev_info(usb->dev,
		 "configured BCM3383 USB wrapper: setup=0x%08x swap=0x%08x pll=0x%08x sim=0x%08x\n",
		 readl_be(usb->base + USB_CTRL_SETUP),
		 readl_be(usb->base + USB_CTRL_SWAP),
		 readl_be(usb->base + USB_CTRL_PLL_CTL_1),
		 readl_be(usb->base + USB_CTRL_SIM_CTL));

	return 0;
}

static int usb_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3383_usb *usb;
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
		return dev_err_probe(dev, ret, "failed to populate EHCI/OHCI children\n");

	return 0;
}

static const struct of_device_id usb_of_match[] = {
	{ .compatible = "brcm,bcm3383-usb" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, usb_of_match);

static struct platform_driver usb_driver = {
	.probe = usb_probe,
	.driver = {
		.name = "bcm3383-usb",
		.of_match_table = usb_of_match,
	},
};
module_platform_driver(usb_driver);

MODULE_DESCRIPTION("Broadcom BCM3383 USB wrapper driver");
MODULE_LICENSE("GPL");
