// SPDX-License-Identifier: GPL-2.0-only
/* WD WT6703F LED driver
 *
 * Implements LED control using the reverse-engineered command set used by
 * mcm-daemon on WD My Cloud platforms.
 */

#include <dt-bindings/leds/common.h>
#include <linux/leds.h>
#include <linux/list.h>
#include <linux/mfd/wd-wt6703f.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/slab.h>

#define WD_WT6703F_CMD_LEN 7

struct wd_wt6703f_led;

struct wd_wt6703f_led_group {
	struct mutex lock;
	struct list_head leds;
};

static const u8 wd_wt6703f_led_off[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x01, 0x00, 0x01, 0xfb };
static const u8 wd_wt6703f_led_blue_on[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x11, 0x00, 0x01, 0xfb };
static const u8 wd_wt6703f_led_blue_blink[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x21, 0x00, 0x01, 0xfb };
static const u8 wd_wt6703f_led_red_on[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x14, 0x00, 0x01, 0xfb };
static const u8 wd_wt6703f_led_red_blink[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x24, 0x00, 0x01, 0xfb };
static const u8 wd_wt6703f_led_orange_on[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x12, 0x00, 0x01, 0xfb };
static const u8 wd_wt6703f_led_orange_blink[WD_WT6703F_CMD_LEN] =
	{ 0xfa, 0x26, 0x00, 0x22, 0x00, 0x01, 0xfb };

struct wd_wt6703f_led {
	struct led_classdev cdev;
	struct wd_wt6703f *mcu;
	struct wd_wt6703f_led_group *group;
	struct list_head node;
	enum led_brightness brightness;
	u8 cmd_on[WD_WT6703F_CMD_LEN];
	u8 cmd_blink[WD_WT6703F_CMD_LEN];
	bool blinking;
};

static inline struct wd_wt6703f_led *cdev_to_wd_wt6703f_led(struct led_classdev *cdev)
{
	return container_of(cdev, struct wd_wt6703f_led, cdev);
}

static int wd_wt6703f_led_exec(struct wd_wt6703f_led *priv, const u8 *cmd)
{
	return wd_wt6703f_exec(priv->mcu, cmd, WD_WT6703F_CMD_LEN,
			      NULL, NULL);
}

static int wd_wt6703f_led_brightness_set_blocking(struct led_classdev *cdev,
						   enum led_brightness brightness)
{
	struct wd_wt6703f_led *priv = cdev_to_wd_wt6703f_led(cdev);
	struct wd_wt6703f_led *other;
	const u8 *cmd;
	int ret;

	cmd = (brightness == LED_OFF) ? wd_wt6703f_led_off : priv->cmd_on;
	mutex_lock(&priv->group->lock);
	ret = wd_wt6703f_led_exec(priv, cmd);
	if (ret) {
		mutex_unlock(&priv->group->lock);
		return ret;
	}

	priv->brightness = brightness ? LED_ON : LED_OFF;
	priv->blinking = false;
	if (brightness != LED_OFF) {
		list_for_each_entry(other, &priv->group->leds, node) {
			if (other == priv)
				continue;
			other->brightness = LED_OFF;
			other->cdev.brightness = LED_OFF;
			other->blinking = false;
		}
	}
	mutex_unlock(&priv->group->lock);

	return 0;
}

static enum led_brightness wd_wt6703f_led_brightness_get(struct led_classdev *cdev)
{
	struct wd_wt6703f_led *priv = cdev_to_wd_wt6703f_led(cdev);
	enum led_brightness brightness;

	mutex_lock(&priv->group->lock);
	brightness = priv->brightness;
	mutex_unlock(&priv->group->lock);

	return brightness;
}

static int wd_wt6703f_led_set_blink(struct led_classdev *cdev,
				    unsigned long *delay_on,
				    unsigned long *delay_off)
{
	struct wd_wt6703f_led *priv = cdev_to_wd_wt6703f_led(cdev);
	int ret;

	/* MCU supports a single hardware blink mode per color command. */
	*delay_on = 500;
	*delay_off = 500;

	mutex_lock(&priv->group->lock);
	ret = wd_wt6703f_led_exec(priv, priv->cmd_blink);
	if (ret) {
		mutex_unlock(&priv->group->lock);
		return ret;
	}

	priv->brightness = LED_ON;
	priv->blinking = true;
	{
		struct wd_wt6703f_led *other;

		list_for_each_entry(other, &priv->group->leds, node) {
			if (other == priv)
				continue;
			other->brightness = LED_OFF;
			other->cdev.brightness = LED_OFF;
			other->blinking = false;
		}
	}
	mutex_unlock(&priv->group->lock);

	return 0;
}

static int wd_wt6703f_led_apply_default_state(struct wd_wt6703f_led *priv,
					       struct fwnode_handle *child)
{
	const char *state;

	if (fwnode_property_read_string(child, "default-state", &state))
		return 0;

	if (!strcmp(state, "on"))
		return wd_wt6703f_led_brightness_set_blocking(&priv->cdev, LED_ON);

	if (!strcmp(state, "blink")) {
		unsigned long delay_on = 500;
		unsigned long delay_off = 500;

		return wd_wt6703f_led_set_blink(&priv->cdev, &delay_on, &delay_off);
	}

	return wd_wt6703f_led_brightness_set_blocking(&priv->cdev, LED_OFF);
}

static int wd_wt6703f_led_setup_color(struct device *dev,
				      struct wd_wt6703f_led *priv,
				      struct fwnode_handle *child)
{
	u32 color = LED_COLOR_ID_BLUE;

	fwnode_property_read_u32(child, "color", &color);

	switch (color) {
	case LED_COLOR_ID_BLUE:
		memcpy(priv->cmd_on, wd_wt6703f_led_blue_on, WD_WT6703F_CMD_LEN);
		memcpy(priv->cmd_blink, wd_wt6703f_led_blue_blink, WD_WT6703F_CMD_LEN);
		break;
	case LED_COLOR_ID_RED:
		memcpy(priv->cmd_on, wd_wt6703f_led_red_on, WD_WT6703F_CMD_LEN);
		memcpy(priv->cmd_blink, wd_wt6703f_led_red_blink, WD_WT6703F_CMD_LEN);
		break;
	case LED_COLOR_ID_ORANGE:
		memcpy(priv->cmd_on, wd_wt6703f_led_orange_on, WD_WT6703F_CMD_LEN);
		memcpy(priv->cmd_blink, wd_wt6703f_led_orange_blink, WD_WT6703F_CMD_LEN);
		break;
	default:
		dev_err(dev, "Unsupported LED color id: %u\n", color);
		return -EINVAL;
	}

	return 0;
}

static int wd_wt6703f_led_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct wd_wt6703f *mcu = dev_get_drvdata(dev->parent);
	struct wd_wt6703f_led_group *group;
	int ret = 0;

	if (!mcu)
		return -ENODEV;

	group = devm_kzalloc(dev, sizeof(*group), GFP_KERNEL);
	if (!group)
		return -ENOMEM;
	ret = devm_mutex_init(dev, &group->lock);
	if (ret)
		return ret;
	INIT_LIST_HEAD(&group->leds);

	device_for_each_child_node_scoped(dev, child) {
		struct wd_wt6703f_led *priv;
		struct led_init_data init_data = {};

		priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
		if (!priv)
			return -ENOMEM;

		priv->mcu = mcu;
		priv->group = group;
		INIT_LIST_HEAD(&priv->node);
		priv->brightness = LED_OFF;
		priv->blinking = false;

		ret = wd_wt6703f_led_setup_color(dev, priv, child);
		if (ret)
			return ret;

		priv->cdev.max_brightness = 1;
		priv->cdev.brightness_set_blocking = wd_wt6703f_led_brightness_set_blocking;
		priv->cdev.brightness_get = wd_wt6703f_led_brightness_get;
		priv->cdev.blink_set = wd_wt6703f_led_set_blink;

		init_data.fwnode = fwnode_handle_get(child);

		ret = devm_led_classdev_register_ext(dev, &priv->cdev, &init_data);
		if (ret)
			return dev_err_probe(dev, ret, "Failed to register WT6703F LED\n");

		list_add_tail(&priv->node, &group->leds);
		ret = wd_wt6703f_led_apply_default_state(priv, child);
		if (ret)
			return dev_err_probe(dev, ret, "Failed to apply default state\n");
	}

	return ret;
}

static const struct of_device_id wd_wt6703f_led_of_match[] = {
	{ .compatible = "wd,wt6703f-leds" },
	{ }
};
MODULE_DEVICE_TABLE(of, wd_wt6703f_led_of_match);

static struct platform_driver wd_wt6703f_led_driver = {
	.probe = wd_wt6703f_led_probe,
	.driver = {
		.name = "wd-wt6703f-led",
		.of_match_table = wd_wt6703f_led_of_match,
	},
};
module_platform_driver(wd_wt6703f_led_driver);

MODULE_DESCRIPTION("WD WT6703F LED driver");
MODULE_AUTHOR("Alexander Giese <blizzardwyvern@web.de>");
MODULE_LICENSE("GPL v2");
