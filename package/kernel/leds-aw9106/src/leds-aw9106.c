// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * leds-aw9106.c - Multi-channel LED driver for Awinic AW9106B
 * 6-channel I2C LED/GPIO controller.
 *
 * Unlike the original vendor driver (which gangs all 6 channels into
 * a single led_classdev), this driver registers one independent
 * led_classdev per DT child node, each bound to one output channel
 * (0-5), so every physical LED die shows up separately under
 * /sys/class/leds and in LuCI's System > LEDs page.
 *
 * Register map and reset sequence derived from the Awinic reference
 * driver (Copyright (c) 2017 AWINIC Technology CO., LTD).
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/mutex.h>

#define AW9106_I2C_NAME		"aw9106b"
#define AW9106_MAX_CHANNELS		6

#define REG_OUTPUT_P0			0x02
#define REG_OUTPUT_P1			0x03
#define REG_CONFIG_P0			0x04
#define REG_CONFIG_P1			0x05
#define REG_ID				0x10
#define REG_CTRL			0x11
#define REG_WORK_MODE_P0		0x12
#define REG_WORK_MODE_P1		0x13
#define REG_EN_BREATH			0x14
#define REG_DIM_BASE			0x20	/* REG_DIM00 .. REG_DIM05 */
#define REG_SWRST			0x7f

#define AW9106_CHIP_ID			0x23

#define AW_I2C_RETRIES			5
#define AW_I2C_RETRY_DELAY_MS		5

struct aw9106_led {
	struct led_classdev	cdev;
	struct aw9106_chip	*chip;
	u8			channel;	/* 0..5, maps to REG_DIM_BASE + channel */
};

struct aw9106_chip {
	struct i2c_client	*i2c;
	struct device		*dev;
	struct gpio_desc	*reset_gpio;
	struct mutex		lock;		/* serialize i2c access across channels */
	unsigned int		num_leds;
	struct aw9106_led	leds[AW9106_MAX_CHANNELS];
};

static int aw9106_i2c_write(struct aw9106_chip *chip, u8 reg, u8 val)
{
	int ret = -EIO;
	int cnt = 0;

	while (cnt < AW_I2C_RETRIES) {
		ret = i2c_smbus_write_byte_data(chip->i2c, reg, val);
		if (ret >= 0)
			return 0;
		dev_warn(chip->dev, "i2c write reg 0x%02x failed (%d), retry %d\n",
			 reg, ret, cnt);
		cnt++;
		msleep(AW_I2C_RETRY_DELAY_MS);
	}
	return ret;
}

static int aw9106_i2c_read(struct aw9106_chip *chip, u8 reg, u8 *val)
{
	int ret = -EIO;
	int cnt = 0;

	while (cnt < AW_I2C_RETRIES) {
		ret = i2c_smbus_read_byte_data(chip->i2c, reg);
		if (ret >= 0) {
			*val = ret;
			return 0;
		}
		dev_warn(chip->dev, "i2c read reg 0x%02x failed (%d), retry %d\n",
			 reg, ret, cnt);
		cnt++;
		msleep(AW_I2C_RETRY_DELAY_MS);
	}
	return ret;
}

static int aw9106_brightness_set(struct led_classdev *cdev,
				  enum led_brightness brightness)
{
	struct aw9106_led *led = container_of(cdev, struct aw9106_led, cdev);
	struct aw9106_chip *chip = led->chip;
	int ret;

	mutex_lock(&chip->lock);
	ret = aw9106_i2c_write(chip, REG_DIM_BASE + led->channel,
				(u8)brightness);
	mutex_unlock(&chip->lock);

	if (ret)
		dev_err(chip->dev, "failed to set channel %u brightness: %d\n",
			led->channel, ret);
	return ret;
}

static void aw9106_hw_reset(struct aw9106_chip *chip)
{
	if (!chip->reset_gpio)
		return;

	/* Pulse low then hold high: matches vendor reset sequence
	 * (chip requires a low->high transition on this line to come
	 * out of shutdown and respond on the I2C bus).
	 */
	gpiod_set_value_cansleep(chip->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(chip->reset_gpio, 1);
	usleep_range(1000, 2000);
}

static int aw9106_read_chipid(struct aw9106_chip *chip)
{
	u8 val = 0;
	int ret = aw9106_i2c_read(chip, REG_ID, &val);

	if (ret)
		return ret;

	if (val != AW9106_CHIP_ID) {
		dev_err(chip->dev, "unexpected chip id 0x%02x (expected 0x%02x)\n",
			val, AW9106_CHIP_ID);
		return -ENODEV;
	}
	return 0;
}

static int aw9106_hw_init(struct aw9106_chip *chip)
{
	int ret;

	/* All channels -> LED/dimming mode (not plain GPIO), no breathing,
	 * default imax setting. Channels start at brightness 0 (off);
	 * per-LED default-state from DT is applied by the LED core after
	 * classdev registration below.
	 */
	ret = aw9106_i2c_write(chip, REG_CTRL, 0x03);
	if (ret)
		return ret;
	ret = aw9106_i2c_write(chip, REG_WORK_MODE_P0, 0x00);
	if (ret)
		return ret;
	ret = aw9106_i2c_write(chip, REG_WORK_MODE_P1, 0x00);
	if (ret)
		return ret;
	ret = aw9106_i2c_write(chip, REG_EN_BREATH, 0x00);
	if (ret)
		return ret;

	return 0;
}

static int aw9106_parse_led_children(struct aw9106_chip *chip)
{
	struct device_node *np = chip->dev->of_node;
	struct device_node *child;
	unsigned int i = 0;
	int ret;

	for_each_available_child_of_node(np, child) {
		struct aw9106_led *led;
		u32 channel;
		const char *label = NULL;
		const char *default_state = NULL;

		if (i >= AW9106_MAX_CHANNELS) {
			dev_err(chip->dev,
				"too many led child nodes (max %d), ignoring %pOF\n",
				AW9106_MAX_CHANNELS, child);
			continue;
		}

		ret = of_property_read_u32(child, "reg", &channel);
		if (ret || channel >= AW9106_MAX_CHANNELS) {
			dev_err(chip->dev,
				"led node %pOF missing/invalid reg (0..%d)\n",
				child, AW9106_MAX_CHANNELS - 1);
			continue;
		}

		led = &chip->leds[i];
		led->chip = chip;
		led->channel = (u8)channel;

		of_property_read_string(child, "label", &label);
		led->cdev.name = label ? label : child->name;
		led->cdev.max_brightness = 255;
		led->cdev.brightness_set_blocking = aw9106_brightness_set;

		of_property_read_string(child, "default-state", &default_state);
		if (default_state && !strcmp(default_state, "on"))
			led->cdev.brightness = led->cdev.max_brightness;
		else
			led->cdev.brightness = LED_OFF;

		ret = devm_led_classdev_register(chip->dev, &led->cdev);
		if (ret) {
			dev_err(chip->dev, "failed to register led %s: %d\n",
				led->cdev.name, ret);
			continue;
		}

		/* Push the DT-specified default (on/off) out to the chip
		 * immediately, since brightness_set is only invoked on
		 * subsequent changes by most LED core paths.
		 */
		aw9106_brightness_set(&led->cdev, led->cdev.brightness);

		i++;
	}

	if (i == 0) {
		dev_err(chip->dev, "no valid led child nodes found\n");
		return -ENODEV;
	}

	chip->num_leds = i;
	dev_info(chip->dev, "registered %u led channel(s)\n", i);
	return 0;
}

static int aw9106_probe(struct i2c_client *i2c)
{
	struct aw9106_chip *chip;
	int ret;

	if (!i2c_check_functionality(i2c->adapter, I2C_FUNC_I2C))
		return -EIO;

	chip = devm_kzalloc(&i2c->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
		return -ENOMEM;

	chip->i2c = i2c;
	chip->dev = &i2c->dev;
	mutex_init(&chip->lock);
	i2c_set_clientdata(i2c, chip);

	chip->reset_gpio = devm_gpiod_get_optional(chip->dev, "reset",
						    GPIOD_OUT_LOW);
	if (IS_ERR(chip->reset_gpio))
		return dev_err_probe(chip->dev, PTR_ERR(chip->reset_gpio),
				      "failed to get reset gpio\n");

	aw9106_hw_reset(chip);

	ret = aw9106_read_chipid(chip);
	if (ret)
		return ret;

	ret = aw9106_hw_init(chip);
	if (ret)
		return ret;

	ret = aw9106_parse_led_children(chip);
	if (ret)
		return ret;

	return 0;
}

static const struct of_device_id aw9106_of_match[] = {
	{ .compatible = "awinic,aw9106b" },
	{ }
};
MODULE_DEVICE_TABLE(of, aw9106_of_match);

static const struct i2c_device_id aw9106_i2c_id[] = {
	{ AW9106_I2C_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, aw9106_i2c_id);

static struct i2c_driver aw9106_i2c_driver = {
	.driver = {
		.name = AW9106_I2C_NAME,
		.of_match_table = aw9106_of_match,
	},
	.probe = aw9106_probe,
	.id_table = aw9106_i2c_id,
};
module_i2c_driver(aw9106_i2c_driver);

MODULE_DESCRIPTION("Awinic AW9106B multi-channel LED driver");
MODULE_LICENSE("GPL");
