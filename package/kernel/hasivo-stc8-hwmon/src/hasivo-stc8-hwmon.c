// SPDX-License-Identifier: GPL-2.0-only
/*
 * Hasivo STC8 microcontroller hwmon driver
 *
 * Exposes CPU/system temperatures and fan presence + control as hwmon
 * attributes via the parent MFD's syscon regmap. The STC8 controls fan
 * speed internally based on temperature; the host can only request one
 * of three fan modes (auto / force-on / force-off) and read whether the
 * fan is currently running. There is no tach / PWM duty register on the
 * chip - PWM control is exposed as a thresholded on/off via pwm_input,
 * with pwm_enable following the hwmon convention:
 *   0 = no host control (fan at full speed)
 *   1 = manual (state determined by pwm_input)
 *   2 = automatic (MCU regulates based on internal temperature)
 *
 * Register protocol reverse-engineered from the stock 'imi' binary on a
 * Hasivo S1300WP-8XGT-4S+ (port_type 0x1b). The chip at I2C 0x4D reports
 * identity 0xDB at reg 0xFD on that board (verified empirically).
 *
 * Copyright (C) 2026 Carlo Szelinsky <github@szelinsky.de>
 */

#include <linux/bitops.h>
#include <linux/hwmon.h>
#include <linux/minmax.h>
#include <linux/mfd/syscon.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define STC8_REG_CPU_TEMP_SIGN		0x01
#define STC8_REG_CPU_TEMP_MAG		0x02
#define STC8_REG_SYS_TEMP_SIGN		0x03
#define STC8_REG_SYS_TEMP_MAG		0x04
#define STC8_REG_FAN_CONTROL		0xfa
#define STC8_REG_FAN_STATUS		0xfb
#define STC8_REG_ID			0xfd
#define STC8_REG_CAPABILITIES		0xfe

#define STC8_ID_EXPECTED		0xdb

#define STC8_CAP_FAN_PRESENT		BIT(6)
#define STC8_CAP_TEMP_MASK		GENMASK(5, 0)
#define STC8_TEMP_FLAG_NONE		1
#define STC8_TEMP_FLAG_SYSTEM_ONLY	2

#define STC8_FAN_VAL_AUTO		'O'	/* 0x4f */
#define STC8_FAN_VAL_FORCE_ON		'_'	/* 0x5f */
#define STC8_FAN_VAL_FORCE_OFF		'@'	/* 0x40 */

#define STC8_PWM_FORCE_THRESHOLD	128

enum stc8_temp_channel {
	STC8_TEMP_CPU = 0,
	STC8_TEMP_SYS,
};

struct stc8_hwmon {
	struct regmap *regmap;
	bool fan_present;
	bool cpu_temp_avail;
	bool sys_temp_avail;
};

static int stc8_read_temp(struct regmap *regmap, unsigned int sign_reg,
			  unsigned int mag_reg, long *temp)
{
	unsigned int sign, mag;
	int ret;

	ret = regmap_read(regmap, sign_reg, &sign);
	if (ret)
		return ret;
	ret = regmap_read(regmap, mag_reg, &mag);
	if (ret)
		return ret;

	*temp = (long)(sign == 1 ? -(int)mag : (int)mag) * 1000;
	return 0;
}

static umode_t stc8_is_visible(const void *drvdata,
			       enum hwmon_sensor_types type,
			       u32 attr, int channel)
{
	const struct stc8_hwmon *priv = drvdata;

	switch (type) {
	case hwmon_temp:
		/*
		 * Expose both temperature channels unconditionally so
		 * sysfs numbering stays contiguous (some userspace tools
		 * assume so). Per-channel availability is reported via
		 * tempN_fault.
		 */
		switch (attr) {
		case hwmon_temp_input:
		case hwmon_temp_label:
		case hwmon_temp_fault:
			return 0444;
		}
		break;
	case hwmon_fan:
		if (!priv->fan_present)
			return 0;
		if (attr == hwmon_fan_alarm)
			return 0444;
		break;
	case hwmon_pwm:
		if (!priv->fan_present)
			return 0;
		if (attr == hwmon_pwm_enable || attr == hwmon_pwm_input)
			return 0644;
		break;
	default:
		break;
	}
	return 0;
}

static int stc8_read(struct device *dev, enum hwmon_sensor_types type,
		     u32 attr, int channel, long *val)
{
	struct stc8_hwmon *priv = dev_get_drvdata(dev);
	unsigned int v;
	int ret;

	switch (type) {
	case hwmon_temp:
		switch (attr) {
		case hwmon_temp_input:
			if (channel == STC8_TEMP_CPU)
				return stc8_read_temp(priv->regmap,
						      STC8_REG_CPU_TEMP_SIGN,
						      STC8_REG_CPU_TEMP_MAG,
						      val);
			return stc8_read_temp(priv->regmap,
					      STC8_REG_SYS_TEMP_SIGN,
					      STC8_REG_SYS_TEMP_MAG,
					      val);
		case hwmon_temp_fault:
			if (channel == STC8_TEMP_CPU)
				*val = priv->cpu_temp_avail ? 0 : 1;
			else
				*val = priv->sys_temp_avail ? 0 : 1;
			return 0;
		}
		break;
	case hwmon_fan:
		if (attr == hwmon_fan_alarm) {
			ret = regmap_read(priv->regmap,
					  STC8_REG_FAN_STATUS, &v);
			if (ret)
				return ret;
			*val = (v != STC8_FAN_VAL_AUTO &&
				v != STC8_FAN_VAL_FORCE_ON) ? 1 : 0;
			return 0;
		}
		break;
	case hwmon_pwm:
		switch (attr) {
		case hwmon_pwm_enable:
		case hwmon_pwm_input:
			ret = regmap_read(priv->regmap,
					  STC8_REG_FAN_CONTROL, &v);
			if (ret)
				return ret;
			break;
		default:
			return -EOPNOTSUPP;
		}
		switch (attr) {
		case hwmon_pwm_enable:
			/*
			 * hwmon sysfs convention:
			 *   0 = no host control (fan at full speed)
			 *   1 = manual (driven by pwm_input)
			 *   2 = automatic
			 */
			switch (v) {
			case STC8_FAN_VAL_AUTO:
				*val = 2;
				break;
			case STC8_FAN_VAL_FORCE_ON:
				*val = 0;
				break;
			default:
				*val = 1;
				break;
			}
			return 0;
		case hwmon_pwm_input:
			*val = (v == STC8_FAN_VAL_FORCE_ON) ? 255 : 0;
			return 0;
		}
		break;
	default:
		break;
	}
	return -EOPNOTSUPP;
}

static int stc8_read_string(struct device *dev, enum hwmon_sensor_types type,
			    u32 attr, int channel, const char **str)
{
	if (type == hwmon_temp && attr == hwmon_temp_label) {
		*str = (channel == STC8_TEMP_CPU) ? "CPU" : "System";
		return 0;
	}
	return -EOPNOTSUPP;
}

static int stc8_write(struct device *dev, enum hwmon_sensor_types type,
		      u32 attr, int channel, long val)
{
	struct stc8_hwmon *priv = dev_get_drvdata(dev);
	unsigned int byte;

	if (type != hwmon_pwm)
		return -EOPNOTSUPP;

	switch (attr) {
	case hwmon_pwm_enable:
		switch (val) {
		case 0:
			byte = STC8_FAN_VAL_FORCE_ON;
			break;
		case 1:
			/*
			 * Manual: leave the MCU's automatic regulation
			 * by switching to an explicit FORCE_OFF state so
			 * a subsequent pwm_enable read reports 1 (manual)
			 * even before any pwm_input write. Userspace is
			 * expected to follow up by writing pwm_input to
			 * pick the actual fan state.
			 */
			byte = STC8_FAN_VAL_FORCE_OFF;
			break;
		case 2:
			byte = STC8_FAN_VAL_AUTO;
			break;
		default:
			return -EINVAL;
		}
		break;
	case hwmon_pwm_input:
		if (val < 0)
			return -EINVAL;
		val = clamp_val(val, 0, 255);
		byte = (val >= STC8_PWM_FORCE_THRESHOLD) ?
			STC8_FAN_VAL_FORCE_ON : STC8_FAN_VAL_FORCE_OFF;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return regmap_write(priv->regmap, STC8_REG_FAN_CONTROL, byte);
}

static const struct hwmon_channel_info *const stc8_hwmon_info[] = {
	HWMON_CHANNEL_INFO(temp,
			   HWMON_T_INPUT | HWMON_T_LABEL | HWMON_T_FAULT,
			   HWMON_T_INPUT | HWMON_T_LABEL | HWMON_T_FAULT),
	HWMON_CHANNEL_INFO(fan, HWMON_F_ALARM),
	HWMON_CHANNEL_INFO(pwm, HWMON_PWM_ENABLE | HWMON_PWM_INPUT),
	NULL
};

static const struct hwmon_ops stc8_hwmon_ops = {
	.is_visible = stc8_is_visible,
	.read = stc8_read,
	.read_string = stc8_read_string,
	.write = stc8_write,
};

static const struct hwmon_chip_info stc8_hwmon_chip_info = {
	.ops = &stc8_hwmon_ops,
	.info = stc8_hwmon_info,
};

static int stc8_hwmon_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct stc8_hwmon *priv;
	struct device *hwmon_dev;
	unsigned int id, caps, flag;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->regmap = syscon_node_to_regmap(dev_of_node(dev->parent));
	if (IS_ERR(priv->regmap))
		return dev_err_probe(dev, PTR_ERR(priv->regmap),
				     "failed to get parent regmap\n");

	ret = regmap_read(priv->regmap, STC8_REG_ID, &id);
	if (ret)
		return dev_err_probe(dev, ret, "failed to read identity\n");
	if (id != STC8_ID_EXPECTED)
		return dev_err_probe(dev, -ENODEV,
				     "unexpected identity 0x%02x (want 0x%02x)\n",
				     id, STC8_ID_EXPECTED);

	ret = regmap_read(priv->regmap, STC8_REG_CAPABILITIES, &caps);
	if (ret)
		return dev_err_probe(dev, ret, "failed to read capabilities\n");

	priv->fan_present = !!(caps & STC8_CAP_FAN_PRESENT);
	flag = caps & STC8_CAP_TEMP_MASK;
	priv->cpu_temp_avail = flag != STC8_TEMP_FLAG_NONE &&
			       flag != STC8_TEMP_FLAG_SYSTEM_ONLY;
	priv->sys_temp_avail = flag != STC8_TEMP_FLAG_NONE;

	dev_info(dev, "fan=%s cpu_temp=%s sys_temp=%s\n",
		 priv->fan_present ? "present" : "absent",
		 priv->cpu_temp_avail ? "yes" : "no",
		 priv->sys_temp_avail ? "yes" : "no");

	platform_set_drvdata(pdev, priv);

	hwmon_dev = devm_hwmon_device_register_with_info(dev, "hasivo_stc8",
							 priv,
							 &stc8_hwmon_chip_info,
							 NULL);
	return PTR_ERR_OR_ZERO(hwmon_dev);
}

static void stc8_hwmon_remove(struct platform_device *pdev)
{
	struct stc8_hwmon *priv = platform_get_drvdata(pdev);

	/*
	 * Restore AUTO mode so the MCU resumes thermal regulation after
	 * unload. Skipping this would leave a user-forced state in
	 * effect indefinitely (e.g. fan off after a force-off
	 * experiment + reboot), which is a real thermal-safety hazard.
	 */
	if (priv->fan_present)
		regmap_write(priv->regmap, STC8_REG_FAN_CONTROL,
			     STC8_FAN_VAL_AUTO);
}

static const struct of_device_id stc8_hwmon_of_match[] = {
	{ .compatible = "hasivo,stc8-hwmon" },
	{ }
};
MODULE_DEVICE_TABLE(of, stc8_hwmon_of_match);

static struct platform_driver stc8_hwmon_driver = {
	.driver = {
		.name = "hasivo-stc8-hwmon",
		.of_match_table = stc8_hwmon_of_match,
	},
	.probe = stc8_hwmon_probe,
	.remove = stc8_hwmon_remove,
};
module_platform_driver(stc8_hwmon_driver);

MODULE_AUTHOR("Carlo Szelinsky <github@szelinsky.de>");
MODULE_DESCRIPTION("Hasivo STC8 microcontroller hwmon driver");
MODULE_LICENSE("GPL");
