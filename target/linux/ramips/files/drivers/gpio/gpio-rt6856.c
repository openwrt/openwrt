// SPDX-License-Identifier: GPL-2.0-only
/*
 * GPIO controller for the Ralink RT6855/RT6856 family.
 *
 * Unlike the older Ralink blocks, RT6856 uses two 2-bit-per-pin mode
 * registers and a separate output-enable register.  The data register is
 * shared by all 32 pins.
 */

#include <linux/bitops.h>
#include <linux/gpio/driver.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#define RT6856_GPIO_DIR0	0x00
#define RT6856_GPIO_DATA	0x04
#define RT6856_GPIO_OE		0x14
#define RT6856_GPIO_DIR1	0x20

#define RT6856_GPIO_MODE_MASK	0x3
#define RT6856_GPIO_MODE_INPUT	0x0
#define RT6856_GPIO_MODE_OUTPUT	0x1

struct rt6856_gpio {
	struct gpio_chip chip;
	void __iomem *base;
	/* Protects read-modify-write sequences on shared GPIO registers. */
	spinlock_t lock;
};

static inline struct rt6856_gpio *to_rt6856_gpio(struct gpio_chip *chip)
{
	return gpiochip_get_data(chip);
}

static int rt6856_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct rt6856_gpio *gpio = to_rt6856_gpio(chip);

	return !!(readl(gpio->base + RT6856_GPIO_DATA) & BIT(offset));
}

static int rt6856_gpio_set(struct gpio_chip *chip, unsigned int offset,
			   int value)
{
	struct rt6856_gpio *gpio = to_rt6856_gpio(chip);
	unsigned long flags;
	u32 data;

	spin_lock_irqsave(&gpio->lock, flags);
	data = readl(gpio->base + RT6856_GPIO_DATA);
	if (value)
		data |= BIT(offset);
	else
		data &= ~BIT(offset);
	writel(data, gpio->base + RT6856_GPIO_DATA);
	spin_unlock_irqrestore(&gpio->lock, flags);

	return 0;
}

static void rt6856_gpio_set_mode(struct rt6856_gpio *gpio,
				 unsigned int offset, u32 mode)
{
	unsigned int shift = (offset & 0xf) * 2;
	unsigned int reg = offset < 16 ? RT6856_GPIO_DIR0 : RT6856_GPIO_DIR1;
	u32 value;

	value = readl(gpio->base + reg);
	value &= ~(RT6856_GPIO_MODE_MASK << shift);
	value |= mode << shift;
	writel(value, gpio->base + reg);
}

static int rt6856_gpio_direction_input(struct gpio_chip *chip,
				       unsigned int offset)
{
	struct rt6856_gpio *gpio = to_rt6856_gpio(chip);
	unsigned long flags;
	u32 oe;

	spin_lock_irqsave(&gpio->lock, flags);
	oe = readl(gpio->base + RT6856_GPIO_OE);
	oe &= ~BIT(offset);
	writel(oe, gpio->base + RT6856_GPIO_OE);
	rt6856_gpio_set_mode(gpio, offset, RT6856_GPIO_MODE_INPUT);
	spin_unlock_irqrestore(&gpio->lock, flags);

	return 0;
}

static int rt6856_gpio_direction_output(struct gpio_chip *chip,
					unsigned int offset, int value)
{
	struct rt6856_gpio *gpio = to_rt6856_gpio(chip);
	unsigned long flags;
	u32 data, oe;

	spin_lock_irqsave(&gpio->lock, flags);
	data = readl(gpio->base + RT6856_GPIO_DATA);
	if (value)
		data |= BIT(offset);
	else
		data &= ~BIT(offset);
	writel(data, gpio->base + RT6856_GPIO_DATA);
	rt6856_gpio_set_mode(gpio, offset, RT6856_GPIO_MODE_OUTPUT);
	oe = readl(gpio->base + RT6856_GPIO_OE);
	oe |= BIT(offset);
	writel(oe, gpio->base + RT6856_GPIO_OE);
	spin_unlock_irqrestore(&gpio->lock, flags);

	return 0;
}

static int rt6856_gpio_get_direction(struct gpio_chip *chip,
				     unsigned int offset)
{
	struct rt6856_gpio *gpio = to_rt6856_gpio(chip);
	u32 oe = readl(gpio->base + RT6856_GPIO_OE);

	/* gpio_chip ABI: 0 is output, 1 is input. */
	return (oe & BIT(offset)) ? 0 : 1;
}

static int rt6856_gpio_probe(struct platform_device *pdev)
{
	struct rt6856_gpio *gpio;
	struct resource *res;

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpio->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(gpio->base))
		return PTR_ERR(gpio->base);

	spin_lock_init(&gpio->lock);
	gpio->chip.label = dev_name(&pdev->dev);
	gpio->chip.parent = &pdev->dev;
	gpio->chip.owner = THIS_MODULE;
	gpio->chip.fwnode = dev_fwnode(&pdev->dev);
	gpio->chip.base = -1;
	gpio->chip.ngpio = 32;
	gpio->chip.get = rt6856_gpio_get;
	gpio->chip.set = rt6856_gpio_set;
	gpio->chip.direction_input = rt6856_gpio_direction_input;
	gpio->chip.direction_output = rt6856_gpio_direction_output;
	gpio->chip.get_direction = rt6856_gpio_get_direction;
	gpio->chip.can_sleep = false;

	return devm_gpiochip_add_data(&pdev->dev, &gpio->chip, gpio);
}

static const struct of_device_id rt6856_gpio_match[] = {
	{ .compatible = "ralink,rt6856-gpio" },
	{ }
};
MODULE_DEVICE_TABLE(of, rt6856_gpio_match);

static struct platform_driver rt6856_gpio_driver = {
	.probe = rt6856_gpio_probe,
	.driver = {
		.name = "rt6856-gpio",
		.of_match_table = rt6856_gpio_match,
	},
};
module_platform_driver(rt6856_gpio_driver);

MODULE_DESCRIPTION("Ralink RT6856 GPIO driver");
MODULE_LICENSE("GPL");
