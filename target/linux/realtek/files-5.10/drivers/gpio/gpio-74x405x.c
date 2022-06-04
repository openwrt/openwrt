// SPDX-License-Identifier: GPL-2.0-only

#include <linux/gpio/driver.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#define GEN_74x405x_MAX_S_PINS	3

struct gen_74x405x_data {
	int channels;
	int ios_per_channel;
	int s_pins;
};

struct gen_74x405x_gpio_priv {
	struct gpio_chip gc;
	struct device *dev;
	struct gen_74x405x_data *cd;

	u8 current_state;

	struct gpio_descs *selects;
	struct gpio_desc *a_n;
	struct gpio_desc *b_n;
	struct gpio_desc *c_n;
};

static struct gpio_desc *h405x_get_combined_pin(struct gen_74x405x_gpio_priv *priv, int offset)
{
	int channel = offset/priv->cd->ios_per_channel;

	switch (channel) {
	case 0:
		return priv->a_n;
	case 1:
		return priv->b_n;
	case 2:
		return priv->c_n;
	}
	return NULL;
}

static void gen_74x405x_switch_io(struct gen_74x405x_gpio_priv *priv, int offset)
{
	int i;
	u8 state = offset & GENMASK(priv->cd->s_pins, 0);

	for (i = 0; i < priv->cd->s_pins; i++) {
		if ((state ^ priv->current_state) & BIT(i))
			gpiod_set_value_cansleep(priv->selects->desc[i], state & BIT(i) ? 1 : 0);
	}

	priv->current_state = state;
}

static void gen_74x405x_gpio_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct gen_74x405x_gpio_priv *priv = gpiochip_get_data(gc);

	gen_74x405x_switch_io(priv, offset);
	gpiod_set_value_cansleep(h405x_get_combined_pin(priv, offset), value);
}

static int gen_74x405x_direction_input(struct gpio_chip *gc, unsigned int offset)
{
	struct gen_74x405x_gpio_priv *priv = gpiochip_get_data(gc);

	gen_74x405x_switch_io(priv, offset);
	return gpiod_direction_input(h405x_get_combined_pin(priv, offset));
}

static int gen_74x405x_direction_output(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct gen_74x405x_gpio_priv *priv = gpiochip_get_data(gc);

	gen_74x405x_switch_io(priv, offset);
	return gpiod_direction_output(h405x_get_combined_pin(priv, offset), value);
}

static int gen_74x405x_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	struct gen_74x405x_gpio_priv *priv = gpiochip_get_data(gc);

	return gpiod_get_direction(h405x_get_combined_pin(priv, offset));
}

static int gen_74x405x_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	struct gen_74x405x_gpio_priv *priv = gpiochip_get_data(gc);

	gen_74x405x_switch_io(priv, offset);
	return gpiod_get_value_cansleep(h405x_get_combined_pin(priv, offset));
}

struct gen_74x405x_data gen_74x4051_data = {
	.channels = 1,
	.ios_per_channel = 8,
	.s_pins = 3,
};

struct gen_74x405x_data gen_74x4052_data = {
	.channels = 2,
	.ios_per_channel = 4,
	.s_pins = 2,
};

struct gen_74x405x_data gen_74x4053_data = {
	.channels = 3,
	.ios_per_channel = 2,
	.s_pins = 3,
};

static const struct of_device_id gen_74x405x_gpio_of_match[] = {
	{ .compatible = "ti,74x4051", .data = (void *) &gen_74x4051_data},
	{ .compatible = "ti,74x4052", .data = (void *) &gen_74x4052_data},
	{ .compatible = "ti,74x4053", .data = (void *) &gen_74x4053_data},
	{},
};

MODULE_DEVICE_TABLE(of, gen_74x405x_gpio_of_match);

static int gen_74x405x_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct gen_74x405x_gpio_priv *gpios;
	int err;

	pr_info("Probing 74x405x GPIOs\n");

	if (!np) {
		dev_err(dev, "No DTS bindings for 74x405x GPIOs found\n");
		return -EINVAL;
	}

	gpios = devm_kzalloc(dev, sizeof(*gpios), GFP_KERNEL);
	if (!gpios)
		return -ENOMEM;

	gpios->cd = (struct gen_74x405x_data *) device_get_match_data(dev);
	gpios->dev = dev;

	gpios->selects = devm_gpiod_get_array(dev, "select", GPIOD_OUT_LOW);
	if (IS_ERR(gpios->selects)) {
		err = PTR_ERR(gpios->selects);
		if (err != -EPROBE_DEFER)
			dev_err(dev, "Failed to get 'select' GPIOs: %d", err);
		else
			dev_info(dev, "Probing deferred\n");
		return err;
	}
	gpios->current_state = 0;

	if (gpios->selects->ndescs != gpios->cd->s_pins) {
		dev_err(dev, "Incorrect number of 'select' pins provided: %d, should be %d\n",
			gpios->selects->ndescs, gpios->cd->s_pins);
		return -EINVAL;
	}

	gpios->a_n = devm_gpiod_get(dev, "an", GPIOD_ASIS);
	if (IS_ERR(gpios->a_n)) {
		err = PTR_ERR(gpios->a_n);
		dev_err(dev, "Failed to get 'an' GPIO: %d\n", err);
		return err;
	}

	if (gpios->cd->channels > 1) {
		gpios->b_n = devm_gpiod_get(dev, "bn", GPIOD_ASIS);
		if (IS_ERR(gpios->b_n)) {
			err = PTR_ERR(gpios->b_n);
			dev_err(dev, "Failed to get 'bn' GPIO: %d\n", err);
			return err;
		}
	}

	if (gpios->cd->channels > 2) {
		gpios->c_n = devm_gpiod_get(dev, "cn", GPIOD_ASIS);
		if (IS_ERR(gpios->c_n)) {
			err = PTR_ERR(gpios->c_n);
			dev_err(dev, "Failed to get 'cn' GPIO: %d\n", err);
			return err;
		}
	}

	gpios->gc.base = -1;
	gpios->gc.ngpio = gpios->cd->ios_per_channel * gpios->cd->channels;
	gpios->gc.label = "gen_74x405x";
	gpios->gc.parent = dev;
	gpios->gc.owner = THIS_MODULE;
	gpios->gc.can_sleep = true;

	gpios->gc.direction_input = gen_74x405x_direction_input;
	gpios->gc.direction_output = gen_74x405x_direction_output;
	gpios->gc.set = gen_74x405x_gpio_set;
	gpios->gc.get = gen_74x405x_gpio_get;
	gpios->gc.get_direction = gen_74x405x_get_direction;

	err = devm_gpiochip_add_data(dev, &gpios->gc, gpios);

	return err;
}

static struct platform_driver gen_74x405x_gpio_driver = {
	.driver = {
		.name = "74x405x-gpio",
		.of_match_table	= gen_74x405x_gpio_of_match,
	},
	.probe = gen_74x405x_gpio_probe,
};

module_platform_driver(gen_74x405x_gpio_driver);

MODULE_DESCRIPTION("Generic 74x405x GPIO expansion chip support");
MODULE_LICENSE("GPL v2");
