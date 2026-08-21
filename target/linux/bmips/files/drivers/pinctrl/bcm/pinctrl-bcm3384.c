// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for BCM3384 pin controller
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include "pinctrl-bcm338x.h"

/*
 * GPL source:
 * C6300BD_1TLAUS/C6300BD_LxG1.0.10_src/kernel/linux/
 * arch/mips/include/asm/brcmstb/3384a0/bchp_gpio_per.h
 *
 * The header declares these as GPIO_PER.TestPort* registers at absolute
 * addresses 0x14e0029c..0x14e002ac.  The offsets below are relative to
 * GPIO_PER_REG_START / BCHP_GPIO_PER_REG_START, 0x14e00100.
 */
// GPIO_PER.TestPortBlkEn1
#define BCM3384_TESTPORT_BLK_EN1		0x019c
// GPIO_PER.TestPortBlkEn2
#define BCM3384_TESTPORT_BLK_EN2		0x01a0
// GPIO_PER.TestPortBlkData1
#define BCM3384_TESTPORT_BLK_DATA1		0x01a4
// GPIO_PER.TestPortBlkData2
#define BCM3384_TESTPORT_BLK_DATA2		0x01a8
// GPIO_PER.TestPortBlkData2.TP_BLK_DATA_LSB payload fields
#define BCM3384_TESTPORT_BLK_DATA2_SELECTOR	GENMASK(11, 0)
#define BCM3384_TESTPORT_BLK_DATA2_MUX		GENMASK(15, 12)
// GPIO_PER.TestPortCommand
#define BCM3384_TESTPORT_COMMAND		0x01ac
#define BCM3384_TESTPORT_COMMAND_WRITE		0x00000021

/*
 * bchp_gpio_per.h exposes the TestPort data register as a raw 32-bit payload
 * and does not name these per-function selector IDs or mux values.  The
 * selector/mux values below were recovered from the stock bootloader/eCos
 * pinmux programming.
 */
// TestPort selector 36..37, mux value 5: USB VBUS controls
#define BCM3384_USB_VBUS_SELECTOR_FIRST		36
#define BCM3384_USB_VBUS_SELECTOR_COUNT		2
#define BCM3384_USB_VBUS_MUX_VALUE		5

// TestPort selector 0x59, mux value 1: HSSPI switch control path
#define BCM3384_HSSPI_SWITCH_SELECTOR		0x59
#define BCM3384_HSSPI_SWITCH_MUX_VALUE		1

// TestPort selectors 0x1e/0x1f, mux value 5: PCIe0/PCIe1 controls
#define BCM3384_PCIE0_SELECTOR			0x1e
#define BCM3384_PCIE1_SELECTOR			0x1f
#define BCM3384_PCIE_MUX_VALUE			5

// BCHP_GPIO_PER_PAD_CTRL
#define BCM3384_GPIO_PER_PAD_CTRL		0x0058
#define BCM3384_GPIO_PER_PAD_CTRL_RGMII_CFG	0x00000210

static const struct pinctrl_pin_desc bcm3384_pins[] = {
	PINCTRL_PIN(0, "usb_vbus"),
	PINCTRL_PIN(1, "hsspi_switch"),
	PINCTRL_PIN(2, "pcie0"),
	PINCTRL_PIN(3, "pcie1"),
	PINCTRL_PIN(4, "unimac1_internal_phy"),
};

static unsigned int usb_vbus_pins[] = { 0 };
static unsigned int hsspi_switch_pins[] = { 1 };
static unsigned int pcie0_pins[] = { 2 };
static unsigned int pcie1_pins[] = { 3 };
static unsigned int unimac1_internal_phy_pins[] = { 4 };

static const struct pingroup bcm3384_groups[] = {
	BCM338X_PIN_GROUP(usb_vbus),
	BCM338X_PIN_GROUP(hsspi_switch),
	BCM338X_PIN_GROUP(pcie0),
	BCM338X_PIN_GROUP(pcie1),
	BCM338X_PIN_GROUP(unimac1_internal_phy),
};

static const char * const usb_vbus_groups[] = {
	"usb_vbus",
};

static const char * const hsspi_switch_groups[] = {
	"hsspi_switch",
};

static const char * const pcie0_groups[] = {
	"pcie0",
};

static const char * const pcie1_groups[] = {
	"pcie1",
};

static const char * const unimac1_internal_phy_groups[] = {
	"unimac1_internal_phy",
};

enum bcm3384_function {
	BCM3384_FUNC_USB_VBUS,
	BCM3384_FUNC_HSSPI_SWITCH,
	BCM3384_FUNC_PCIE0,
	BCM3384_FUNC_PCIE1,
	BCM3384_FUNC_UNIMAC1_INTERNAL_PHY,
};

static const struct bcm338x_function bcm3384_funcs[] = {
	[BCM3384_FUNC_USB_VBUS] = {
		.name = "usb_vbus",
		.groups = usb_vbus_groups,
		.num_groups = ARRAY_SIZE(usb_vbus_groups),
	},
	[BCM3384_FUNC_HSSPI_SWITCH] = {
		.name = "hsspi_switch",
		.groups = hsspi_switch_groups,
		.num_groups = ARRAY_SIZE(hsspi_switch_groups),
	},
	[BCM3384_FUNC_PCIE0] = {
		.name = "pcie0",
		.groups = pcie0_groups,
		.num_groups = ARRAY_SIZE(pcie0_groups),
	},
	[BCM3384_FUNC_PCIE1] = {
		.name = "pcie1",
		.groups = pcie1_groups,
		.num_groups = ARRAY_SIZE(pcie1_groups),
	},
	[BCM3384_FUNC_UNIMAC1_INTERNAL_PHY] = {
		.name = "unimac1_internal_phy",
		.groups = unimac1_internal_phy_groups,
		.num_groups = ARRAY_SIZE(unimac1_internal_phy_groups),
	},
};

static int bcm3384_program_testport_selector(struct bcm338x_pinctrl *pc,
					     unsigned int selector,
					     unsigned int mux)
{
	u32 data = FIELD_PREP(BCM3384_TESTPORT_BLK_DATA2_SELECTOR, selector) |
		   FIELD_PREP(BCM3384_TESTPORT_BLK_DATA2_MUX, mux);
	int ret = regmap_write(pc->regs, BCM3384_TESTPORT_COMMAND, 0);

	if (ret)
		return ret;

	ret = regmap_write(pc->regs, BCM3384_TESTPORT_BLK_EN1, 0);
	if (ret)
		return ret;

	ret = regmap_write(pc->regs, BCM3384_TESTPORT_BLK_EN2, 0);
	if (ret)
		return ret;

	ret = regmap_write(pc->regs, BCM3384_TESTPORT_BLK_DATA1, 0);
	if (ret)
		return ret;

	ret = regmap_write(pc->regs, BCM3384_TESTPORT_BLK_DATA2, data);
	if (ret)
		return ret;

	ret = regmap_write(pc->regs, BCM3384_TESTPORT_COMMAND,
			   BCM3384_TESTPORT_COMMAND_WRITE);
	if (ret)
		return ret;

	return regmap_write(pc->regs, BCM3384_TESTPORT_COMMAND, 0);
}

static int bcm3384_program_pcie_selectors(struct bcm338x_pinctrl *pc)
{
	int ret = bcm3384_program_testport_selector(pc, BCM3384_PCIE0_SELECTOR,
						    BCM3384_PCIE_MUX_VALUE);

	if (ret)
		return ret;

	return bcm3384_program_testport_selector(pc, BCM3384_PCIE1_SELECTOR,
						 BCM3384_PCIE_MUX_VALUE);
}

static int bcm3384_set_mux(struct bcm338x_pinctrl *pc,
			   unsigned int function_selector,
			   unsigned int group_selector)
{
	(void)group_selector;

	switch (function_selector) {
	case BCM3384_FUNC_HSSPI_SWITCH:
		return bcm3384_program_testport_selector(pc,
							 BCM3384_HSSPI_SWITCH_SELECTOR,
							 BCM3384_HSSPI_SWITCH_MUX_VALUE);
	case BCM3384_FUNC_UNIMAC1_INTERNAL_PHY:
		return regmap_update_bits(pc->regs, BCM3384_GPIO_PER_PAD_CTRL,
					  BCM3384_GPIO_PER_PAD_CTRL_RGMII_CFG,
					  BCM3384_GPIO_PER_PAD_CTRL_RGMII_CFG);
	case BCM3384_FUNC_PCIE0:
	case BCM3384_FUNC_PCIE1:
		return bcm3384_program_pcie_selectors(pc);
	case BCM3384_FUNC_USB_VBUS:
		for (unsigned int i = 0; i < BCM3384_USB_VBUS_SELECTOR_COUNT; i++) {
			int ret = bcm3384_program_testport_selector(pc,
								    BCM3384_USB_VBUS_SELECTOR_FIRST + i,
								    BCM3384_USB_VBUS_MUX_VALUE);

			if (ret)
				return ret;
		}

		return 0;
	default:
		return dev_err_probe(pc->dev, -EINVAL,
				     "unsupported function selector %u\n",
				     function_selector);
	}
}

static const struct bcm338x_pinctrl_variant bcm3384_variant = {
	.pins = bcm3384_pins,
	.npins = ARRAY_SIZE(bcm3384_pins),
	.groups = bcm3384_groups,
	.ngroups = ARRAY_SIZE(bcm3384_groups),
	.functions = bcm3384_funcs,
	.nfunctions = ARRAY_SIZE(bcm3384_funcs),
	.gpio_compatible = "brcm,bcm3384-gpio",
	.set_mux = bcm3384_set_mux,
};

static int bcm3384_pinctrl_probe(struct platform_device *pdev)
{
	return bcm338x_pinctrl_probe(pdev, &bcm3384_variant, NULL);
}

static const struct of_device_id bcm3384_pinctrl_match[] = {
	{ .compatible = "brcm,bcm3384-pinctrl", },
	{ /* sentinel */ }
};

static struct platform_driver bcm3384_pinctrl_driver = {
	.probe = bcm3384_pinctrl_probe,
	.driver = {
		.name = "bcm3384-pinctrl",
		.of_match_table = bcm3384_pinctrl_match,
	},
};

builtin_platform_driver(bcm3384_pinctrl_driver);
