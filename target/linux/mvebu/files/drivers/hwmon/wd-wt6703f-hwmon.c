// SPDX-License-Identifier: GPL-2.0-only
/* WD WT6703F MCU hwmon driver */

#include <linux/hwmon.h>
#include <linux/mfd/wd-wt6703f.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#define WD_WT6703F_CMD_LEN	7
#define WD_WT6703F_FAN_MAX	255

static const u8 wd_wt6703f_thermal_get_cmd[WD_WT6703F_CMD_LEN] = {
	0xfa, 0x03, 0x08, 0x00, 0x00, 0x00, 0xfb
};

static const u8 wd_wt6703f_fan_get_cmd[WD_WT6703F_CMD_LEN] = {
	0xfa, 0x02, 0x01, 0x00, 0x00, 0x00, 0xfb
};

static const u8 wd_wt6703f_fan_set_cmd[WD_WT6703F_CMD_LEN] = {
	0xfa, 0x02, 0x00, 0x00, 0x00, 0x00, 0xfb
};

static const u8 wd_wt6703f_thermal_table[] = {
	0x74, 0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d, 0x6c, 0x6b,
	0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61,
	0x60, 0x5f, 0x5e, 0x5d, 0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57,
	0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0x50, 0x4f, 0x4e, 0x4d,
	0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43,
	0x42, 0x41, 0x41, 0x40, 0x3f, 0x3e, 0x3e, 0x3d, 0x3d, 0x3c,
	0x3b, 0x3a, 0x3a, 0x39, 0x38, 0x38, 0x37, 0x36, 0x36, 0x35,
	0x34, 0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x30, 0x2f,
	0x2f, 0x2e, 0x2e, 0x2d, 0x2c, 0x2c, 0x2b, 0x2b, 0x2a, 0x2a,
	0x29, 0x29, 0x28, 0x28, 0x27, 0x27, 0x27, 0x26, 0x26, 0x25,
	0x25, 0x24, 0x24, 0x23, 0x23, 0x22, 0x22, 0x21, 0x21, 0x21,
	0x20, 0x20, 0x1f, 0x1f, 0x1e, 0x1e, 0x1e, 0x1d, 0x1d, 0x1c,
	0x1c, 0x1b, 0x1b, 0x1b, 0x1b, 0x1a, 0x19, 0x19, 0x19, 0x18,
	0x18, 0x17, 0x17, 0x25, 0x1b, 0x1b, 0x19, 0x19, 0x19, 0x18,
	0x18, 0x17, 0x17, 0x16, 0x16, 0x16, 0x15, 0x15, 0x14, 0x14,
	0x14, 0x13, 0x13, 0x12, 0x12, 0x12, 0x11, 0x11, 0x10, 0x10,
	0x10, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c,
	0x0c, 0x0b, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x09, 0x08, 0x08,
	0x07, 0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x04,
	0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

struct wd_wt6703f_hwmon {
	struct wd_wt6703f *mcu;
	struct mutex lock;
	u8 pwm;
};

static int wd_wt6703f_read_temp(struct wd_wt6703f_hwmon *priv, long *value)
{
	u8 response[16];
	size_t response_len = sizeof(response);
	int ret;

	ret = wd_wt6703f_exec(priv->mcu, wd_wt6703f_thermal_get_cmd,
			      WD_WT6703F_CMD_LEN, response, &response_len);
	if (ret)
		return ret;
	if (response_len < 6 || response[5] >= ARRAY_SIZE(wd_wt6703f_thermal_table))
		return -EPROTO;

	*value = wd_wt6703f_thermal_table[response[5]] * 1000;
	return 0;
}

static int wd_wt6703f_read_rpm(struct wd_wt6703f_hwmon *priv, long *value)
{
	u8 response[16];
	size_t response_len = sizeof(response);
	int ret;

	ret = wd_wt6703f_exec(priv->mcu, wd_wt6703f_fan_get_cmd,
			      WD_WT6703F_CMD_LEN, response, &response_len);
	if (ret)
		return ret;
	if (response_len < 6)
		return -EPROTO;

	*value = response[5] ? 300000 / response[5] : 0;
	return 0;
}

static int wd_wt6703f_set_pwm(struct wd_wt6703f_hwmon *priv, u8 pwm)
{
	u8 command[WD_WT6703F_CMD_LEN];
	int ret;

	memcpy(command, wd_wt6703f_fan_set_cmd, sizeof(command));
	command[3] = pwm;
	ret = wd_wt6703f_exec(priv->mcu, command, sizeof(command), NULL, NULL);
	if (!ret)
		priv->pwm = pwm;

	return ret;
}

static int wd_wt6703f_hwmon_read(struct device *dev,
				 enum hwmon_sensor_types type, u32 attr,
				 int channel, long *value)
{
	struct wd_wt6703f_hwmon *priv = dev_get_drvdata(dev);
	int ret;

	if (channel != 0)
		return -EINVAL;

	mutex_lock(&priv->lock);
	if (type == hwmon_temp && attr == hwmon_temp_input)
		ret = wd_wt6703f_read_temp(priv, value);
	else if (type == hwmon_fan && attr == hwmon_fan_input)
		ret = wd_wt6703f_read_rpm(priv, value);
	else if (type == hwmon_pwm && attr == hwmon_pwm_input) {
		*value = priv->pwm;
		ret = 0;
	} else {
		ret = -EINVAL;
	}
	mutex_unlock(&priv->lock);

	return ret;
}

static int wd_wt6703f_hwmon_write(struct device *dev,
				  enum hwmon_sensor_types type, u32 attr,
				  int channel, long value)
{
	struct wd_wt6703f_hwmon *priv = dev_get_drvdata(dev);
	int ret;

	if (type != hwmon_pwm || attr != hwmon_pwm_input || channel != 0 ||
	    value < 0 || value > WD_WT6703F_FAN_MAX)
		return -EINVAL;

	mutex_lock(&priv->lock);
	ret = wd_wt6703f_set_pwm(priv, value);
	mutex_unlock(&priv->lock);

	return ret;
}

static umode_t wd_wt6703f_hwmon_is_visible(const void *data,
					   enum hwmon_sensor_types type,
					   u32 attr, int channel)
{
	if (channel != 0)
		return 0;
	if ((type == hwmon_temp && attr == hwmon_temp_input) ||
	    (type == hwmon_fan && attr == hwmon_fan_input))
		return 0444;
	if (type == hwmon_pwm && attr == hwmon_pwm_input)
		return 0644;
	return 0;
}

static const struct hwmon_ops wd_wt6703f_hwmon_ops = {
	.is_visible = wd_wt6703f_hwmon_is_visible,
	.read = wd_wt6703f_hwmon_read,
	.write = wd_wt6703f_hwmon_write,
};

static const struct hwmon_channel_info *wd_wt6703f_hwmon_info[] = {
	HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
	HWMON_CHANNEL_INFO(fan, HWMON_F_INPUT),
	HWMON_CHANNEL_INFO(pwm, HWMON_PWM_INPUT),
	NULL,
};

static const struct hwmon_chip_info wd_wt6703f_hwmon_chip_info = {
	.ops = &wd_wt6703f_hwmon_ops,
	.info = wd_wt6703f_hwmon_info,
};

static int wd_wt6703f_hwmon_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct wd_wt6703f_hwmon *priv;
	struct device *hwmon_dev;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->mcu = dev_get_drvdata(dev->parent);
	if (!priv->mcu)
		return -ENODEV;

	ret = devm_mutex_init(dev, &priv->lock);
	if (ret)
		return ret;
	platform_set_drvdata(pdev, priv);

	hwmon_dev = devm_hwmon_device_register_with_info(
		dev, "wt6703f", priv, &wd_wt6703f_hwmon_chip_info, NULL);
	if (IS_ERR(hwmon_dev))
		return PTR_ERR(hwmon_dev);

	return 0;
}

static const struct of_device_id wd_wt6703f_hwmon_of_match[] = {
	{ .compatible = "wd,wt6703f-hwmon" },
	{ }
};
MODULE_DEVICE_TABLE(of, wd_wt6703f_hwmon_of_match);

static struct platform_driver wd_wt6703f_hwmon_driver = {
	.probe = wd_wt6703f_hwmon_probe,
	.driver = {
		.name = "wd-wt6703f-hwmon",
		.of_match_table = wd_wt6703f_hwmon_of_match,
	},
};
module_platform_driver(wd_wt6703f_hwmon_driver);

MODULE_DESCRIPTION("WD WT6703F MCU hwmon driver");
MODULE_AUTHOR("Alexander Giese <blizzardwyvern@web.de>");
MODULE_LICENSE("GPL v2");
