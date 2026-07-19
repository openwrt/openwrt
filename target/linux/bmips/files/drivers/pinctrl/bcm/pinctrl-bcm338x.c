// SPDX-License-Identifier: GPL-2.0+
/*
 * Common BCM338x pin controller support
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/err.h>
#include <linux/gpio/regmap.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/regmap.h>

#include "../pinctrl-utils.h"

#include "pinctrl-bcm338x.h"

static int bcm338x_pinctrl_get_group_count(struct pinctrl_dev *pctldev)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);

	return pc->variant->ngroups;
}

static const char *bcm338x_pinctrl_get_group_name(struct pinctrl_dev *pctldev,
						  unsigned int group)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);

	return pc->variant->groups[group].name;
}

static int bcm338x_pinctrl_get_group_pins(struct pinctrl_dev *pctldev,
					  unsigned int group,
					  const unsigned int **pins,
					  unsigned int *npins)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);

	*pins = pc->variant->groups[group].pins;
	*npins = pc->variant->groups[group].npins;

	return 0;
}

static int bcm338x_pinctrl_get_func_count(struct pinctrl_dev *pctldev)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);

	return pc->variant->nfunctions;
}

static const char *bcm338x_pinctrl_get_func_name(struct pinctrl_dev *pctldev,
						 unsigned int selector)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);

	return pc->variant->functions[selector].name;
}

static int bcm338x_pinctrl_get_groups(struct pinctrl_dev *pctldev,
				      unsigned int selector,
				      const char * const **groups,
				      unsigned int * const num_groups)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);
	const struct bcm338x_function *f = &pc->variant->functions[selector];

	*groups = f->groups;
	*num_groups = f->num_groups;

	return 0;
}

static int bcm338x_pinctrl_set_mux(struct pinctrl_dev *pctldev,
				   unsigned int selector, unsigned int group)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);
	const struct bcm338x_function *f = &pc->variant->functions[selector];

	if (pc->variant->set_gpio) {
		const struct pingroup *pg = &pc->variant->groups[group];

		for (unsigned int i = 0; i < pg->npins; i++) {
			int ret = pc->variant->set_gpio(pc, pg->pins[i]);

			if (ret)
				return ret;
		}
	}

	for (unsigned int i = 0; i < f->num_fields; i++) {
		int ret = regmap_update_bits(pc->regs, f->fields[i].reg,
					     f->fields[i].mask,
					     f->fields[i].value);

		if (ret)
			return ret;
	}

	return 0;
}

static int bcm338x_gpio_request_enable(struct pinctrl_dev *pctldev,
				       struct pinctrl_gpio_range *range,
				       unsigned int offset)
{
	struct bcm338x_pinctrl *pc = pinctrl_dev_get_drvdata(pctldev);

	if (!pc->variant->set_gpio)
		return 0;

	return pc->variant->set_gpio(pc, offset);
}

static const struct pinctrl_ops bcm338x_pctl_ops = {
	.dt_free_map = pinctrl_utils_free_map,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_pin,
	.get_group_name = bcm338x_pinctrl_get_group_name,
	.get_group_pins = bcm338x_pinctrl_get_group_pins,
	.get_groups_count = bcm338x_pinctrl_get_group_count,
};

static const struct pinmux_ops bcm338x_pmx_ops = {
	.get_function_groups = bcm338x_pinctrl_get_groups,
	.get_function_name = bcm338x_pinctrl_get_func_name,
	.get_functions_count = bcm338x_pinctrl_get_func_count,
	.gpio_request_enable = bcm338x_gpio_request_enable,
	.set_mux = bcm338x_pinctrl_set_mux,
	.strict = true,
};

static int bcm338x_gpio_named_reg(struct device *dev, struct device_node *np,
				  const char *name, unsigned int *reg,
				  unsigned int *size)
{
	int index = of_property_match_string(np, "reg-names", name);

	if (index < 0)
		return dev_err_probe(dev, index,
				     "%pOF: missing %s reg range\n", np, name);

	u64 addr;
	u64 range_size;
	int ret = of_property_read_reg(np, index, &addr, &range_size);

	if (ret)
		return dev_err_probe(dev, ret,
				     "%pOF: failed to read %s reg range\n",
				     np, name);

	if (addr > UINT_MAX)
		return dev_err_probe(dev, -EINVAL,
				     "%pOF: %s reg offset is too large\n",
				     np, name);

	if (range_size > UINT_MAX)
		return dev_err_probe(dev, -EINVAL,
				     "%pOF: %s reg range is too large\n",
				     np, name);

	*reg = addr;
	*size = range_size;

	return 0;
}

static int bcm338x_gpio_register_one(struct platform_device *pdev,
				     struct regmap *regs,
				     const struct bcm338x_pinctrl_variant *variant,
				     struct device_node *gpio_np)
{
	struct device *dev = &pdev->dev;
	unsigned int dirout_reg;
	unsigned int dirout_size;
	int ret = bcm338x_gpio_named_reg(dev, gpio_np, "dirout", &dirout_reg,
					 &dirout_size);

	if (ret)
		return ret;

	unsigned int data_reg;
	unsigned int data_size;

	ret = bcm338x_gpio_named_reg(dev, gpio_np, "dat", &data_reg,
				     &data_size);
	if (ret)
		return ret;

	if (dirout_size != data_size)
		return dev_err_probe(dev, -EINVAL,
				     "%pOF: dirout/dat ranges have different sizes\n",
				     gpio_np);

	unsigned int gpio_bank_stride = variant->gpio_bank_stride ?:
					BCM338X_GPIO_BANK_STRIDE;

	if (!dirout_size || dirout_size % gpio_bank_stride)
		return dev_err_probe(dev, -EINVAL,
				     "%pOF: invalid GPIO register range size\n",
				     gpio_np);

	u32 ngpios;

	ret = of_property_read_u32(gpio_np, "ngpios", &ngpios);
	if (ret)
		return dev_err_probe(dev, ret, "%pOF: missing ngpios\n",
				     gpio_np);

	unsigned int gpio_bank_gpios = variant->gpio_bank_gpios ?:
				       BCM338X_GPIO_BANK_GPIOS;
	unsigned int gpio_banks = data_size / gpio_bank_stride;

	if (gpio_banks > UINT_MAX / gpio_bank_gpios)
		return dev_err_probe(dev, -EINVAL,
				     "%pOF: GPIO register range is too large\n",
				     gpio_np);

	unsigned int covered_gpios = gpio_banks * gpio_bank_gpios;

	if (!ngpios || ngpios > covered_gpios)
		return dev_err_probe(dev, -EINVAL,
				     "%pOF: ngpios exceeds GPIO register range\n",
				     gpio_np);

	struct gpio_regmap_config grc = {};

	grc.parent = dev;
	grc.fwnode = of_fwnode_handle(gpio_np);
	grc.ngpio = ngpios;
	grc.ngpio_per_reg = gpio_bank_gpios;
	grc.reg_stride = gpio_bank_stride;
	grc.regmap = regs;
	grc.reg_dat_base = GPIO_REGMAP_ADDR(data_reg);
	grc.reg_set_base = GPIO_REGMAP_ADDR(data_reg);
	grc.reg_dir_out_base = GPIO_REGMAP_ADDR(dirout_reg);

	return PTR_ERR_OR_ZERO(devm_gpio_regmap_register(dev, &grc));
}

static int bcm338x_gpio_register(struct platform_device *pdev,
				 struct regmap *regs,
				 const struct bcm338x_pinctrl_variant *variant)
{
	struct device *dev = &pdev->dev;

	if (!variant->gpio_compatible)
		return 0;

	for_each_child_of_node_scoped(dev->parent->of_node, gpio_np) {
		if (!of_device_is_compatible(gpio_np, variant->gpio_compatible))
			continue;

		if (!of_device_is_available(gpio_np))
			continue;

		int ret = bcm338x_gpio_register_one(pdev, regs, variant,
						    gpio_np);

		if (ret)
			return ret;
	}

	return 0;
}

int bcm338x_pinctrl_probe(struct platform_device *pdev,
			  const struct bcm338x_pinctrl_variant *variant,
			  void *driver_data)
{
	struct device *dev = &pdev->dev;

	if (!variant || !variant->pins || !variant->npins || !variant->groups ||
	    !variant->ngroups || !variant->functions || !variant->nfunctions)
		return dev_err_probe(dev, -EINVAL,
				     "invalid BCM338x pinctrl variant data\n");

	struct bcm338x_pinctrl *pc = devm_kzalloc(dev, sizeof(*pc),
						 GFP_KERNEL);

	if (!pc)
		return -ENOMEM;

	platform_set_drvdata(pdev, pc);

	pc->dev = dev;
	pc->variant = variant;
	pc->driver_data = driver_data;

	pc->regs = syscon_node_to_regmap(dev->parent->of_node);
	if (IS_ERR(pc->regs))
		return PTR_ERR(pc->regs);

	pc->pctl_desc.name = dev_name(dev);
	pc->pctl_desc.pins = variant->pins;
	pc->pctl_desc.npins = variant->npins;
	pc->pctl_desc.pctlops = &bcm338x_pctl_ops;
	pc->pctl_desc.pmxops = &bcm338x_pmx_ops;
	pc->pctl_desc.owner = THIS_MODULE;

	pc->pctl_dev = devm_pinctrl_register(dev, &pc->pctl_desc, pc);
	if (IS_ERR(pc->pctl_dev))
		return PTR_ERR(pc->pctl_dev);

	return bcm338x_gpio_register(pdev, pc->regs, variant);
}
