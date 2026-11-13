// SPDX-License-Identifier: GPL-2.0-only
/*
 *  MikroTik RouterBOARD SPI shift register GPIO driver
 *
 *  Some RouterBOARDs have a 74HC595 sharing the SPI bus with the boot
 *  flash. The register only shifts in data while the flash is selected,
 *  so RouterOS sends a harmless READ STATUS command followed by the
 *  register contents, then pulses a strobe GPIO to latch the outputs.
 *
 *  Copyright (C) 2026 Santiago Hormazabal <santiagohssl@gmail.com>
 *
 *  Based on the 74x164 GPIO driver (drivers/gpio/gpio-74x164.c):
 *    Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *    Copyright (C) 2010 Miguel Gaio <miguel.gaio@efixo.com>
 *
 *  The update sequence follows rb400_change_cfg() from the MikroTik
 *  RouterOS GPL kernel sources (arch/mips/rb/rb400/spi.c).
 */

#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/spi/spi.h>

#define GPIO_SSR_DRIVER_NAME	"gpio-ssr-mikrotik"
#define GPIO_SSR_MAX_REGISTERS	3
#define GPIO_SSR_CMD_RD_STATUS	0x05

struct gpio_ssr_chip {
	struct gpio_chip gc;
	struct mutex lock;
	struct spi_device *spi;
	struct gpio_desc *strobe;
	u32 registers;
	/* Register contents, in the order they are shifted out */
	u8 buffer[GPIO_SSR_MAX_REGISTERS];
	u8 tx[1 + GPIO_SSR_MAX_REGISTERS] ____cacheline_aligned;
};

static int gpio_ssr_write(struct gpio_ssr_chip *chip)
{
	int ret;

	chip->tx[0] = GPIO_SSR_CMD_RD_STATUS;
	memcpy(&chip->tx[1], chip->buffer, chip->registers);

	ret = spi_write(chip->spi, chip->tx, 1 + chip->registers);
	if (ret)
		return ret;

	gpiod_set_value_cansleep(chip->strobe, 1);
	gpiod_set_value_cansleep(chip->strobe, 0);

	return 0;
}

static int gpio_ssr_get(struct gpio_chip *gc, unsigned int offset)
{
	struct gpio_ssr_chip *chip = gpiochip_get_data(gc);
	unsigned int bank = chip->registers - 1 - offset / 8;
	int ret;

	mutex_lock(&chip->lock);
	ret = !!(chip->buffer[bank] & BIT(offset % 8));
	mutex_unlock(&chip->lock);

	return ret;
}

static int gpio_ssr_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct gpio_ssr_chip *chip = gpiochip_get_data(gc);
	unsigned int bank = chip->registers - 1 - offset / 8;
	int ret;

	mutex_lock(&chip->lock);
	if (value)
		chip->buffer[bank] |= BIT(offset % 8);
	else
		chip->buffer[bank] &= ~BIT(offset % 8);
	ret = gpio_ssr_write(chip);
	mutex_unlock(&chip->lock);

	return ret;
}

static int gpio_ssr_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	return GPIO_LINE_DIRECTION_OUT;
}

static int gpio_ssr_direction_output(struct gpio_chip *gc,
				     unsigned int offset, int value)
{
	return gpio_ssr_set(gc, offset, value);
}

static void gpio_ssr_put_spi(void *data)
{
	put_device(data);
}

static int gpio_ssr_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gpio_ssr_chip *chip;
	struct device_node *np;
	struct device *spi_dev;
	int ret;

	chip = devm_kzalloc(dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	ret = device_property_read_u32(dev, "registers-number",
				       &chip->registers);
	if (ret || !chip->registers ||
	    chip->registers > GPIO_SSR_MAX_REGISTERS)
		return dev_err_probe(dev, -EINVAL,
				     "invalid 'registers-number'\n");

	device_property_read_u8_array(dev, "registers-default", chip->buffer,
				      chip->registers);

	np = of_parse_phandle(dev->of_node, "mikrotik,spi-flash", 0);
	if (!np)
		return dev_err_probe(dev, -EINVAL,
				     "missing 'mikrotik,spi-flash'\n");

	spi_dev = bus_find_device_by_of_node(&spi_bus_type, np);
	of_node_put(np);
	if (!spi_dev)
		return -EPROBE_DEFER;

	ret = devm_add_action_or_reset(dev, gpio_ssr_put_spi, spi_dev);
	if (ret)
		return ret;
	chip->spi = to_spi_device(spi_dev);

	/* Idle with the strobe inactive; the latch fires on its release */
	chip->strobe = devm_gpiod_get(dev, "strobe", GPIOD_OUT_LOW);
	if (IS_ERR(chip->strobe))
		return dev_err_probe(dev, PTR_ERR(chip->strobe),
				     "failed to get strobe GPIO\n");

	ret = devm_mutex_init(dev, &chip->lock);
	if (ret)
		return ret;

	ret = gpio_ssr_write(chip);
	if (ret)
		return dev_err_probe(dev, ret, "initial write failed\n");

	chip->gc.label = GPIO_SSR_DRIVER_NAME;
	chip->gc.parent = dev;
	chip->gc.owner = THIS_MODULE;
	chip->gc.base = -1;
	chip->gc.ngpio = chip->registers * 8;
	chip->gc.can_sleep = true;
	chip->gc.get = gpio_ssr_get;
	chip->gc.set = gpio_ssr_set;
	chip->gc.get_direction = gpio_ssr_get_direction;
	chip->gc.direction_output = gpio_ssr_direction_output;

	return devm_gpiochip_add_data(dev, &chip->gc, chip);
}

static const struct of_device_id gpio_ssr_match[] = {
	{ .compatible = "mikrotik,gpio-ssr" },
	{}
};
MODULE_DEVICE_TABLE(of, gpio_ssr_match);

static struct platform_driver gpio_ssr_driver = {
	.probe = gpio_ssr_probe,
	.driver = {
		.name = GPIO_SSR_DRIVER_NAME,
		.of_match_table = gpio_ssr_match,
	},
};

module_platform_driver(gpio_ssr_driver);

MODULE_DESCRIPTION("MikroTik RouterBOARD SPI shift register GPIO driver");
MODULE_LICENSE("GPL v2");
