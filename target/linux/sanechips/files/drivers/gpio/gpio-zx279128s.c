// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s GPIO controller
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/gpio/driver.h>
#include <linux/gpio/generic.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>

/*
 * Each bank has 16 lines, one bit per line in each register. The registers
 * at 0x04-0x10 and 0x28-0x34 control the interrupts, which are not supported
 * yet.
 */
#define ZX279128S_GPIO_DIR	0x00	/* 1: output */
#define ZX279128S_GPIO_DATA	0x14	/* line level */
#define ZX279128S_GPIO_SET	0x18	/* write 1: drive high */
#define ZX279128S_GPIO_CLEAR	0x1c	/* write 1: drive low */

static int zx279128s_gpio_probe(struct platform_device *pdev)
{
	struct gpio_generic_chip_config config = { };
	struct device *dev = &pdev->dev;
	struct gpio_generic_chip *chip;
	void __iomem *base;
	struct clk *clk;
	int ret;

	chip = devm_kzalloc(dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return PTR_ERR(base);

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "failed to get the clock\n");

	config.dev = dev;
	config.sz = 2;
	config.dat = base + ZX279128S_GPIO_DATA;
	config.set = base + ZX279128S_GPIO_SET;
	config.clr = base + ZX279128S_GPIO_CLEAR;
	config.dirout = base + ZX279128S_GPIO_DIR;

	ret = gpio_generic_chip_init(chip, &config);
	if (ret)
		return dev_err_probe(dev, ret, "failed to set up the GPIO chip\n");

	return devm_gpiochip_add_data(dev, &chip->gc, NULL);
}

static const struct of_device_id zx279128s_gpio_of_match[] = {
	{ .compatible = "zte,zx279128s-gpio" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx279128s_gpio_of_match);

static struct platform_driver zx279128s_gpio_driver = {
	.probe = zx279128s_gpio_probe,
	.driver = {
		.name = "zx279128s-gpio",
		.of_match_table = zx279128s_gpio_of_match,
	},
};
module_platform_driver(zx279128s_gpio_driver);

MODULE_AUTHOR("Navid Ghahremani <ghahramani.navid@gmail.com>");
MODULE_DESCRIPTION("ZTE zx279128s GPIO controller driver");
MODULE_LICENSE("GPL");
