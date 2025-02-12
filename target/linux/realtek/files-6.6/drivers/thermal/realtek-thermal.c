// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek thermal sensor driver
 *
 * Copyright (C) 2025 Bjørn Mork <bjorn@mork.no>>
 */

#include <linux/bitfield.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include "thermal_hwmon.h"

#define RTL8380_THERMAL_METER_CTRL0	0x98
#define RTL8380_THERMAL_METER_CTRL1	0x9c
#define RTL8380_THERMAL_METER_CTRL2	0xa0
#define RTL8380_THERMAL_METER_RESULT	0xa4
#define RTL8380_TM_ENABLE		BIT(0)
#define RTL8380_TEMP_VALID		BIT(8)
#define RTL8380_TEMP_OUT_MASK		GENMASK(6, 0)

#define RTL9300_THERMAL_METER_CTRL0	0x60
#define RTL9300_THERMAL_METER_CTRL1	0x64
#define RTL9300_THERMAL_METER_CTRL2	0x68
#define RTL9300_THERMAL_METER_RESULT0	0x6c
#define RTL9300_THERMAL_METER_RESULT1	0x70
#define RTL9300_TM_ENABLE		BIT(16)
#define RTL9300_TEMP_VALID		BIT(24)
#define RTL9300_TEMP_OUT_MASK		GENMASK(23, 16)

#define REALTEK_THERMAL_FLAG_INIT	BIT(0)

struct realtek_thermal_priv {
	struct regmap *regmap;
	u32 flags;
};

static void rtl8380_thermal_init(struct realtek_thermal_priv *priv)
{
	if (!regmap_update_bits(priv->regmap, RTL8380_THERMAL_METER_CTRL0, RTL8380_TM_ENABLE, RTL8380_TM_ENABLE))
		priv->flags |= REALTEK_THERMAL_FLAG_INIT;
}

static int rtl8380_get_temp(struct thermal_zone_device *tz, int *res)
{
	struct realtek_thermal_priv *priv = thermal_zone_device_priv(tz);
        int offset = thermal_zone_get_offset(tz);
        int slope = thermal_zone_get_slope(tz);
	u32 val;
	int ret;

	if (!(priv->flags & REALTEK_THERMAL_FLAG_INIT))
		rtl8380_thermal_init(priv);

        ret = regmap_read(priv->regmap, RTL8380_THERMAL_METER_RESULT, &val);
        if (ret)
                return ret;

	if (!(val & RTL8380_TEMP_VALID))
		return -EAGAIN;

	*res = FIELD_GET(RTL8380_TEMP_OUT_MASK, val) * slope + offset;
	return 0;
}

static const struct thermal_zone_device_ops rtl8380_ops = {
	.get_temp = rtl8380_get_temp,
};

static void rtl9300_thermal_init(struct realtek_thermal_priv *priv)
{
	if (!regmap_update_bits(priv->regmap, RTL9300_THERMAL_METER_CTRL2, RTL9300_TM_ENABLE, RTL9300_TM_ENABLE))
		priv->flags |= REALTEK_THERMAL_FLAG_INIT;
}

static int rtl9300_get_temp(struct thermal_zone_device *tz, int *res)
{
	struct realtek_thermal_priv *priv = thermal_zone_device_priv(tz);
        int offset = thermal_zone_get_offset(tz);
        int slope = thermal_zone_get_slope(tz);
	u32 val;
	int ret;

	if (!(priv->flags & REALTEK_THERMAL_FLAG_INIT))
		rtl9300_thermal_init(priv);

        ret = regmap_read(priv->regmap, RTL9300_THERMAL_METER_RESULT0, &val);
        if (ret)
                return ret;

	if (!(val & RTL9300_TEMP_VALID))
		return -EAGAIN;

	*res = FIELD_GET(RTL9300_TEMP_OUT_MASK, val) * slope + offset;
	return 0;
}

static const struct thermal_zone_device_ops rtl9300_ops = {
	.get_temp = rtl9300_get_temp,
};

static int realtek_thermal_probe(struct platform_device *pdev)
{
	struct realtek_thermal_priv *priv;
	struct thermal_zone_device *tzdev;
        struct device *dev = &pdev->dev;
        struct device_node *parent;
        struct regmap *regmap;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

        parent = of_get_parent(dev->of_node);
        regmap = syscon_node_to_regmap(parent);
        of_node_put(parent);
        if (IS_ERR(regmap)) {
                ret = PTR_ERR(regmap);
                return ret;
        }
        priv->regmap = regmap;

        tzdev = devm_thermal_of_zone_register(dev, 0, priv, device_get_match_data(dev));
        if (IS_ERR(tzdev)) {
                ret = PTR_ERR(tzdev);
                return ret;
        }

        return thermal_add_hwmon_sysfs(tzdev);
}

static const struct of_device_id realtek_sensor_ids[] = {
	{ .compatible = "realtek,rtl8380-thermal", .data = &rtl8380_ops, },
	{ .compatible = "realtek,rtl9300-thermal", .data = &rtl9300_ops, },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, realtek_sensor_ids);

static struct platform_driver realtek_thermal_driver = {
	.probe	= realtek_thermal_probe,
	.driver	= {
		.name		= "realtek-thermal",
		.of_match_table	= realtek_sensor_ids,
	},
};

module_platform_driver(realtek_thermal_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bjørn Mork <bjorn@mork.no>");
MODULE_DESCRIPTION("Realtek temperature sensor");
