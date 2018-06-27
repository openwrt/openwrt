// SPDX-License-Identifier: GPL-2.0+
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/leds.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>

/*
 * AW9523 registers
 */
#define AW9523_REG_INPUT_PORT0		0x00
#define AW9523_REG_INPUT_PORT1		0x01
#define AW9523_REG_OUTPUT_PORT0		0x02
#define AW9523_REG_OUTPUT_PORT1		0x03
#define AW9523_REG_CFG_PORT0		0x04	/* P0 0-7 config: 0 - OUT, 1 - IN */
#define AW9523_REG_CFG_PORT1		0x05
#define AW9523_REG_INTN_PORT0		0x06	/* P0 0-7 inten: - disabled, 1 - enabled */
#define AW9523_REG_INTN_PORT1		0x07
#define AW9523_REG_DEV_ID		0x10
#define AW9523_REG_GPOMD		0x11	/* BIT(4) - P0 mode: 0 - Open-Drain, 1 - Push-Pull, BITS(1:0) - ISEL */
#define AW9523_REG_MODE_PORT0		0x12	/* P0 mode switch: 0 - GPIO, 1 - LED */
#define AW9523_REG_MODE_PORT1		0x13
#define AW9523_REG_LED_BASE		0x20	/* PORT1-4 0x20-0x24, PORT0 0x24-0x2b, PORT1-5 0x2c-0x2f WR only */
#define AW9523_REG_SWRESET		0x7F

#define AW9523_P0_PUSHPULL		BIT(4)	/* 0 - open-drain, 1 - push-pull */

#define AW9523_MAXGPIO			16
#define AW9523_BANK(offs)		((offs) >> 3)
#define AW9523_BIT(offs)		BIT((offs) & 0x7)
#define MAX_BRIGHTNESS			255

struct aw9523_led {
	struct aw9523_priv_data *pdata;
	struct led_classdev cdev;
	uint8_t brighness;
	uint8_t port;
};

struct aw9523_priv_data {
	struct i2c_client *client;
	struct mutex lock;	/* protect cached dir, dat_out */
	uint8_t mode[2];	/* LED/GPIO bits */
	uint8_t p0xmod;		/* AW9523 Port 0 mode */
	struct aw9523_led leds[AW9523_MAXGPIO];
};

static int aw9523_i2c_read(struct i2c_client *client, u8 reg)
{
	int ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		dev_err(&client->dev, "Read Error\n");

	return ret;
}

static int aw9523_i2c_write(struct i2c_client *client, u8 reg, u8 val)
{
	int ret = i2c_smbus_write_byte_data(client, reg, val);

	if (ret < 0)
		dev_err(&client->dev, "Write Error\n");

	return ret;
}

static enum led_brightness aw9523_led_get_value(struct led_classdev *cdev)
{
	struct aw9523_led *led = container_of(cdev, struct aw9523_led, cdev);

	return led->brighness;
}

static void aw9523_led_set_value(struct led_classdev *cdev, enum led_brightness value)
{
	struct aw9523_led *led = container_of(cdev, struct aw9523_led, cdev);
	unsigned int port;

	mutex_lock(&led->pdata->lock);

	aw9523_i2c_write(led->pdata->client, AW9523_REG_LED_BASE + led->port, value);
	led->brighness = value;
	mutex_unlock(&led->pdata->lock);
}

static int aw9523_led_to_port(uint8_t led)
{
	if (led < 8)
	    return 4 + led;
	if (led > 12)
	    return led;
	return (led & 0x07);
}

static int aw9523_leds_register(struct aw9523_priv_data *dev, struct device_node *np)
{
	struct aw9523_led *led;
	struct device_node *child;
	int ret = 0, i;
	unsigned int bit, bank;

	for_each_available_child_of_node(np, child) {
		ret = of_property_read_u32(child, "reg", &i);
		if (!ret) {
			led = &dev->leds[i & 0x0F];
			led->cdev.name = of_get_property(child, "label", NULL);
		}
	}

	for (i = 0; i < AW9523_MAXGPIO; i++){
		bank = AW9523_BANK(i);
		bit = AW9523_BIT(i);
		if (!(dev->mode[bank] & bit)) {
			led = &dev->leds[i];
			led->pdata = dev;
			led->port = aw9523_led_to_port(i);
			led->cdev.brightness_get = aw9523_led_get_value;
			led->cdev.brightness_set = aw9523_led_set_value;
			led->cdev.max_brightness = MAX_BRIGHTNESS;
			led->cdev.brightness = LED_OFF;
			if (led->cdev.name == NULL)
				led->cdev.name = devm_kasprintf(&dev->client->dev,
						GFP_KERNEL, "aw9523:led%02d",i);
			if (!led->cdev.name)
				return -ENOMEM;
			ret = led_classdev_register(&dev->client->dev, &led->cdev);
			if (ret)
				return ret;
		}
	}
	return ret;
}

static int aw9523_leds_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	struct device_node *np = client->dev.of_node;
	struct aw9523_priv_data *dev;
	struct gpio_desc *reset_gpio;
	u8 val;
	int ret, revid;

	if (!i2c_check_functionality(client->adapter,
					I2C_FUNC_SMBUS_BYTE_DATA)) {
		dev_err(&client->dev, "SMBUS Byte Data not Supported\n");
		return -EIO;
	}

	/* bring the chip out of reset if reset pin is provided*/
	reset_gpio = devm_gpiod_get_optional(&client->dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(reset_gpio)) {
		dev_warn(&client->dev, "Could not get reset-gpios\n");
	} else {
		gpiod_set_value_cansleep(reset_gpio, 0);
		usleep_range(1000, 1100);
		gpiod_set_value_cansleep(reset_gpio, 1);
	}

	dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->client = client;

	mutex_init(&dev->lock);

	ret = aw9523_i2c_write(client, AW9523_REG_SWRESET, 0);
	if (ret < 0)
		goto err;

	ret = aw9523_i2c_read(client, AW9523_REG_DEV_ID);
	if (ret < 0)
		goto err;

	revid = ret;	/* 0x23 by default */

	if (!device_property_read_u8(&client->dev, "led-current", &val))
	    dev->p0xmod |= (val & 7);

	ret = device_property_read_u8(&client->dev, "ch0-config", &val);
	if (!ret) {
		dev->mode[0] = val;
	} else
		dev->mode[0] = 0x0;

	ret = device_property_read_u8(&client->dev, "ch1-config", &val);
	if (!ret) {
		dev->mode[1] = val;
	} else
		dev->mode[1] = 0x0;

	/* Configure registers: port to OUT, interrupts DISABLE */
	ret = aw9523_i2c_write(client, AW9523_REG_CFG_PORT0, 0);
	ret |= aw9523_i2c_write(client, AW9523_REG_CFG_PORT1, 0);
	ret |= aw9523_i2c_write(client, AW9523_REG_INTN_PORT0, 0xFF);
	ret |= aw9523_i2c_write(client, AW9523_REG_INTN_PORT1, 0xFF);
	ret |= aw9523_i2c_write(client, AW9523_REG_MODE_PORT0, dev->mode[0]);
	ret |= aw9523_i2c_write(client, AW9523_REG_MODE_PORT1, dev->mode[1]);

	ret |= aw9523_i2c_write(client, AW9523_REG_GPOMD, dev->p0xmod);

	if (ret) {
		dev_err(&client->dev, "Initial programming error\n");
		goto err;
	}

	ret = aw9523_leds_register(dev, np);
	if (ret) {
		dev_err(&client->dev, "Register led class failed: %d\n", ret);
		goto err;
	}

	dev_info(&client->dev, "I2C AW9523 Rev.: %d\n", revid);

	i2c_set_clientdata(client, dev);

	return 0;

err:
	return ret;
}

static int aw9523_leds_remove(struct i2c_client *client)
{
	struct aw9523_priv_data *dev = i2c_get_clientdata(client);
	struct aw9523_led *led;
	int i;

	for (i = 0; i < AW9523_MAXGPIO; i++){
		led = &dev->leds[i];
		if (led->pdata)
		    led_classdev_unregister(&led->cdev);
	}

	return 0;
}

static const struct of_device_id aw9523_of_match_table[] = {
	{ .compatible = "allwinc,aw9523" },
	{ }
};
MODULE_DEVICE_TABLE(of, aw9523_of_match_table);

static const struct i2c_device_id aw9523_leds_id[] = {
	{"aw9523-leds", 0},
	{ }
};

MODULE_DEVICE_TABLE(i2c, aw9523_leds_id);

static struct i2c_driver aw9523_leds_driver = {
	.driver = {
		    .name = "aw9523-leds",
		    .of_match_table = of_match_ptr(aw9523_of_match_table),
		   },
	.probe = aw9523_leds_probe,
	.remove = aw9523_leds_remove,
};

module_i2c_driver(aw9523_leds_driver);

MODULE_DESCRIPTION("Allwinc AW9523 GPIO LED expander driver");
MODULE_LICENSE("GPL");
