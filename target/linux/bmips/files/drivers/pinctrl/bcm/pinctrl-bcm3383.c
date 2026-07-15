// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for BCM3383 pin controller
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include "../pinctrl-utils.h"

#include "pinctrl-bcm63xx.h"

#define BCM3383_NUM_PINS		1

// GpioGpioRgmiiPadCtrl
#define BCM3383_PAD_CTRL_REG		0x18
// Core0Rgmii1Sel0 = 1， Core0Rgmii0Sel0 = 1
#define BCM3383_PAD_CTRL_RGMII0_EXTERNAL	0x00000210

// GpioPinMuxSelMid
#define BCM3383_PINMUX_MID_REG		0xc4
// PmSelectRgmii1Tx0 = 1b1, PmSelectRgmii1Rx = 3b101, PmSelectRgmii1Rxd = 3b101
// PmSelectRgmii0Txd = 3b0, PmSelectRgmii0Tx = 3b0, PmSelectRgmii0Rx = 3b0, PmSelectRgmii0Rxd = 3b0
#define BCM3383_PINMUX_MID_RGMII0_EXTERNAL	0xda000000

// GpioPinMuxSelHi1
#define BCM3383_PINMUX_HI1_REG		0xc8
// PmSelectUsOe = 3b0, PmSelectTpSc = 3b0, PmSelectTpSpim = 3b0, PmSelectTpSpi = 3b0
// PmSelectSpisUart1 = 3b0, PmSelectPass = 3b0, PmSelectHvg = 3b0, PmSelectBmu = 3b0,
// PmSelectMdio = 3b0, PmSelectRgmii1Txd = 3b101, PmSelectRgmii1Tx21 = 2b10
#define BCM3383_PINMUX_HI1_RGMII0_EXTERNAL	0x00000016

struct bcm3383_mux_field {
	unsigned int reg;
	unsigned int mask;
	unsigned int value;
};

struct bcm3383_function {
	const char *name;
	const char * const *groups;
	const unsigned int num_groups;

	const struct bcm3383_mux_field *fields;
	const unsigned int num_fields;
};

#define BCM3383_MUX_FIELD(_reg, _value)		\
	{					\
		.reg = (_reg),			\
		.mask = (_value),		\
		.value = (_value),		\
	}

static const struct pinctrl_pin_desc bcm3383_pins[] = {
	PINCTRL_PIN(0, "rgmii0"),
};

static unsigned int rgmii0_pins[] = { 0 };

static struct pingroup bcm3383_groups[] = {
	BCM_PIN_GROUP(rgmii0),
};

static const char * const rgmii0_groups[] = {
	"rgmii0",
};

/*
 * The bootloader enables the external GMAC0 RGMII path by ORing these raw
 * Reg32 values into the GPIO pad and pinmux registers.  Keep the same
 * set-only behavior until the individual field values are fully decoded.
 */
static const struct bcm3383_mux_field rgmii0_fields[] = {
	BCM3383_MUX_FIELD(BCM3383_PAD_CTRL_REG,
			  BCM3383_PAD_CTRL_RGMII0_EXTERNAL),
	BCM3383_MUX_FIELD(BCM3383_PINMUX_MID_REG,
			  BCM3383_PINMUX_MID_RGMII0_EXTERNAL),
	BCM3383_MUX_FIELD(BCM3383_PINMUX_HI1_REG,
			  BCM3383_PINMUX_HI1_RGMII0_EXTERNAL),
};

#define BCM3383_PINMUX_FUN(n)				\
	{						\
		.name = #n,				\
		.groups = n##_groups,			\
		.num_groups = ARRAY_SIZE(n##_groups),	\
		.fields = n##_fields,			\
		.num_fields = ARRAY_SIZE(n##_fields),	\
	}

static const struct bcm3383_function bcm3383_funcs[] = {
	BCM3383_PINMUX_FUN(rgmii0),
};

static int bcm3383_pinctrl_get_group_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(bcm3383_groups);
}

static const char *bcm3383_pinctrl_get_group_name(struct pinctrl_dev *pctldev,
						  unsigned int group)
{
	return bcm3383_groups[group].name;
}

static int bcm3383_pinctrl_get_group_pins(struct pinctrl_dev *pctldev,
					  unsigned int group,
					  const unsigned int **pins,
					  unsigned int *npins)
{
	*pins = bcm3383_groups[group].pins;
	*npins = bcm3383_groups[group].npins;

	return 0;
}

static int bcm3383_pinctrl_get_func_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(bcm3383_funcs);
}

static const char *bcm3383_pinctrl_get_func_name(struct pinctrl_dev *pctldev,
						 unsigned int selector)
{
	return bcm3383_funcs[selector].name;
}

static int bcm3383_pinctrl_get_groups(struct pinctrl_dev *pctldev,
				      unsigned int selector,
				      const char * const **groups,
				      unsigned int * const num_groups)
{
	*groups = bcm3383_funcs[selector].groups;
	*num_groups = bcm3383_funcs[selector].num_groups;

	return 0;
}

static int bcm3383_pinctrl_set_mux(struct pinctrl_dev *pctldev,
				   unsigned int selector, unsigned int group)
{
	struct bcm63xx_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);
	const struct bcm3383_function *f = &bcm3383_funcs[selector];
	unsigned int i;
	int ret;

	for (i = 0; i < f->num_fields; i++) {
		ret = regmap_update_bits(pc->regs, f->fields[i].reg,
					 f->fields[i].mask,
					 f->fields[i].value);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct pinctrl_ops bcm3383_pctl_ops = {
	.dt_free_map = pinctrl_utils_free_map,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_pin,
	.get_group_name = bcm3383_pinctrl_get_group_name,
	.get_group_pins = bcm3383_pinctrl_get_group_pins,
	.get_groups_count = bcm3383_pinctrl_get_group_count,
};

static const struct pinmux_ops bcm3383_pmx_ops = {
	.get_function_groups = bcm3383_pinctrl_get_groups,
	.get_function_name = bcm3383_pinctrl_get_func_name,
	.get_functions_count = bcm3383_pinctrl_get_func_count,
	.set_mux = bcm3383_pinctrl_set_mux,
	.strict = true,
};

static const struct bcm63xx_pinctrl_soc bcm3383_soc = {
	.ngpios = 0,
	.npins = ARRAY_SIZE(bcm3383_pins),
	.pctl_ops = &bcm3383_pctl_ops,
	.pins = bcm3383_pins,
	.pmx_ops = &bcm3383_pmx_ops,
};

static int bcm3383_pinctrl_probe(struct platform_device *pdev)
{
	return bcm63xx_pinctrl_probe(pdev, &bcm3383_soc, NULL);
}

static const struct of_device_id bcm3383_pinctrl_match[] = {
	{ .compatible = "brcm,bcm3383-pinctrl", },
	{ /* sentinel */ }
};

static struct platform_driver bcm3383_pinctrl_driver = {
	.probe = bcm3383_pinctrl_probe,
	.driver = {
		.name = "bcm3383-pinctrl",
		.of_match_table = bcm3383_pinctrl_match,
	},
};

builtin_platform_driver(bcm3383_pinctrl_driver);
