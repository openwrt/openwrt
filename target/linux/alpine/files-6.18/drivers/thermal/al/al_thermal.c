/*
 * Annapurna Labs thermal driver.
 *
 * Copyright (C) 2013 Annapurna Labs
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/thermal.h>

#include "al_hal_thermal_sensor.h"

#define MCELSIUS(temp) ((temp) * 1000)

#define TIMEOUT_MS 1000

struct al_thermal_dev {
  struct al_thermal_sensor_handle handle;
  void __iomem *pbs_base;
  struct thermal_zone_device *tzd;
  bool tripless_tzd;
};

static inline int thermal_enable(struct al_thermal_sensor_handle *handle) {
  int timeout;

  al_thermal_sensor_enable_set(handle, 1);

  for (timeout = 0; timeout < TIMEOUT_MS; timeout++) {
    if (al_thermal_sensor_is_ready(handle))
      break;
    udelay(1000);
  }
  if (timeout == TIMEOUT_MS) {
    pr_err("%s: al_thermal_sensor_is_ready timed out!\n", __func__);
    return -ETIME;
  }

  al_thermal_sensor_trigger_continuous(handle);

  return 0;
}

static inline int thermal_get_temp(struct thermal_zone_device *thermal,
                                   int *temp) {
  struct al_thermal_dev *al_dev = thermal_zone_device_priv(thermal);
  int timeout;

  for (timeout = 0; timeout < TIMEOUT_MS; timeout++) {
    if (al_thermal_sensor_readout_is_valid(&al_dev->handle))
      break;
    udelay(1000);
  }
  if (timeout == TIMEOUT_MS) {
    pr_err("%s: al_thermal_sensor_readout_is_valid timed out!\n", __func__);
    return -ETIME;
  }

  *temp = MCELSIUS(al_thermal_sensor_readout_get(&al_dev->handle));

  return 0;
}

static struct thermal_zone_device_ops ops = {
    .get_temp = thermal_get_temp,
};

static int al_thermal_register_zone(struct platform_device *pdev,
                                    struct al_thermal_dev *al_dev) {
  struct thermal_zone_device *al_thermal;
  int err;

  al_thermal = devm_thermal_of_zone_register(&pdev->dev, 0, al_dev, &ops);
  if (IS_ERR(al_thermal)) {
    err = PTR_ERR(al_thermal);
    if (err != -ENODEV) {
      pr_err("%s: thermal OF zone registration failed: %d\n", __func__, err);
      return err;
    }

    al_thermal =
        thermal_tripless_zone_device_register("al_thermal", al_dev, &ops, NULL);
    if (IS_ERR(al_thermal)) {
      pr_err("%s: thermal zone device registration failed\n", __func__);
      return PTR_ERR(al_thermal);
    }

    al_dev->tripless_tzd = true;
  }

  al_dev->tzd = al_thermal;

  err = thermal_zone_device_enable(al_dev->tzd);
  if (err && al_dev->tripless_tzd)
    thermal_zone_device_unregister(al_dev->tzd);

  return err;
}

static void __iomem *al_thermal_pbs_iomap(struct platform_device *pdev) {
  struct device_node *np = pdev->dev.of_node;
  struct device_node *pbs_np;
  void __iomem *pbs_base;

  pbs_np = of_parse_phandle(np, "annapurna-labs,pbs", 0);
  if (!pbs_np)
    pbs_np = of_parse_phandle(np, "pbs", 0);
  if (!pbs_np)
    pbs_np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-pbs");
  if (!pbs_np) {
    pr_err("%s: PBS DT node missing\n", __func__);
    return ERR_PTR(-ENODEV);
  }

  pbs_base = of_iomap(pbs_np, 0);
  of_node_put(pbs_np);
  if (!pbs_base) {
    pr_err("%s: pbs_base map failed\n", __func__);
    return ERR_PTR(-ENOMEM);
  }

  return pbs_base;
}

#ifdef CONFIG_PM
static int al_thermal_suspend(struct device *dev) {
  struct platform_device *pdev = to_platform_device(dev);
  struct al_thermal_dev *al_dev = platform_get_drvdata(pdev);

  /* Disable Annapurna Labs Thermal Sensor */
  al_thermal_sensor_enable_set(&al_dev->handle, 0);

  pr_info("%s: Suspended.\n", __func__);

  return 0;
}

static int al_thermal_resume(struct device *dev) {
  struct platform_device *pdev = to_platform_device(dev);
  struct al_thermal_dev *al_dev = platform_get_drvdata(pdev);
  int err = 0;

  /* Enable Annapurna Labs Thermal Sensor */
  err = thermal_enable(&al_dev->handle);
  if (err) {
    pr_err("%s: thermal_enable failed!\n", __func__);
    return err;
  }

  pr_info("%s: Resumed.\n", __func__);

  return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(al_thermal_pm_ops, al_thermal_suspend,
                         al_thermal_resume);

static int al_thermal_probe(struct platform_device *pdev) {
  struct al_thermal_dev *al_dev;
  struct device_node *np = pdev->dev.of_node;
  struct resource *al_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
  void __iomem *thermal_base;
  int err;

  if (!np) {
    pr_err("%s: Failed: DT data not available\n", __func__);
    return -EINVAL;
  }

  if (!al_res) {
    pr_err("%s: memory resource missing\n", __func__);
    return -ENODEV;
  }

  al_dev = devm_kzalloc(&pdev->dev, sizeof(*al_dev), GFP_KERNEL);
  if (!al_dev) {
    pr_err("%s: kzalloc fail\n", __func__);
    return -ENOMEM;
  }

  thermal_base = devm_ioremap(&pdev->dev, al_res->start, resource_size(al_res));
  if (!thermal_base) {
    pr_err("%s: ioremap failed\n", __func__);
    return -ENOMEM;
  }

  al_dev->pbs_base = al_thermal_pbs_iomap(pdev);
  if (IS_ERR(al_dev->pbs_base))
    return PTR_ERR(al_dev->pbs_base);

  err = al_thermal_sensor_handle_init(&al_dev->handle, thermal_base,
                                      al_dev->pbs_base);
  if (err) {
    pr_err("%s: al_thermal_sensor_init failed!\n", __func__);
    goto err_unmap_pbs;
  }

  err = thermal_enable(&al_dev->handle);
  if (err) {
    pr_err("%s: thermal_enable failed!\n", __func__);
    goto err_disable_sensor;
  }

  err = al_thermal_register_zone(pdev, al_dev);
  if (err) {
    pr_err("%s: thermal zone registration failed\n", __func__);
    goto err_disable_sensor;
  }

  platform_set_drvdata(pdev, al_dev);

  pr_info("%s: Thermal Sensor Loaded at: 0x%p (dev_id=%u, offset=%u, multiplier=%u).\n",
          __func__, thermal_base, al_dev->handle.dev_id,
          al_dev->handle.offset, al_dev->handle.multiplier);

  return 0;

err_disable_sensor:
  al_thermal_sensor_enable_set(&al_dev->handle, 0);
err_unmap_pbs:
  iounmap(al_dev->pbs_base);
  return err;
}

static void al_thermal_exit(struct platform_device *pdev) {
  struct al_thermal_dev *al_dev = platform_get_drvdata(pdev);

  al_thermal_sensor_enable_set(&al_dev->handle, 0);
  if (al_dev->tripless_tzd)
    thermal_zone_device_unregister(al_dev->tzd);
  platform_set_drvdata(pdev, NULL);

  iounmap(al_dev->pbs_base);
}

static const struct of_device_id al_thermal_id_table[] = {
    {.compatible = "annapurna-labs,al-thermal"}, {}};
MODULE_DEVICE_TABLE(of, al_thermal_id_table);

static struct platform_driver al_thermal_driver = {
    .probe = al_thermal_probe,
    .remove = al_thermal_exit,
    .driver =
        {
            .name = "al_thermal",
            .owner = THIS_MODULE,
            .pm = &al_thermal_pm_ops,
            .of_match_table = of_match_ptr(al_thermal_id_table),
        },
};

module_platform_driver(al_thermal_driver);

MODULE_DESCRIPTION("Annapurna Labs thermal driver");
MODULE_LICENSE("GPL");
