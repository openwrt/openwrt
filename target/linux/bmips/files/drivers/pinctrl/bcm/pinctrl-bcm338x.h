/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Common BCM338x pin controller support
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#ifndef __PINCTRL_BCM338X_H__
#define __PINCTRL_BCM338X_H__

#include <linux/kernel.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/platform_device.h>

struct regmap;
struct bcm338x_pinctrl;

#define BCM338X_GPIO_BANK_GPIOS		32
#define BCM338X_GPIO_BANK_STRIDE	0x04

struct bcm338x_mux_field {
	unsigned int reg;
	unsigned int mask;
	unsigned int value;
};

struct bcm338x_function {
	const char *name;
	const char * const *groups;
	unsigned int num_groups;

	const struct bcm338x_mux_field *fields;
	unsigned int num_fields;
};

struct bcm338x_pinctrl_variant {
	const struct pinctrl_pin_desc *pins;
	unsigned int npins;

	const struct pingroup *groups;
	unsigned int ngroups;

	const struct bcm338x_function *functions;
	unsigned int nfunctions;

	const char *gpio_compatible;
	unsigned int gpio_bank_gpios;
	unsigned int gpio_bank_stride;

	int (*set_gpio)(struct bcm338x_pinctrl *pc, unsigned int pin);
};

struct bcm338x_pinctrl {
	struct device *dev;
	struct regmap *regs;
	const struct bcm338x_pinctrl_variant *variant;

	struct pinctrl_desc pctl_desc;
	struct pinctrl_dev *pctl_dev;

	void *driver_data;
};

#define BCM338X_PIN_GROUP(n)	PINCTRL_PINGROUP(#n, n##_pins, ARRAY_SIZE(n##_pins))

int bcm338x_pinctrl_probe(struct platform_device *pdev,
			  const struct bcm338x_pinctrl_variant *variant,
			  void *driver_data);

#endif /* __PINCTRL_BCM338X_H__ */
