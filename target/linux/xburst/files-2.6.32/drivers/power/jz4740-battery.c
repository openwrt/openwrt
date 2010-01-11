/*
 * Battery measurement code for Ingenic JZ SOC.
 *
 * based on tosa_battery.c
 *
 * Copyright (C) 2008 Marek Vasut <marek.vasut@gmail.com>
 * Copyright (C) 2009 Jiejing Zhang <kzjeef@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>

#include <linux/power/jz4740-battery.h>
#include <linux/jz4740-adc.h>

struct jz_battery_info {
	struct power_supply usb;
	struct power_supply bat;
	struct power_supply ac;
	int bat_status;
	struct jz_batt_info *pdata;
	struct mutex work_lock;
	struct workqueue_struct *monitor_wqueue;
	struct delayed_work bat_work;
};

#define ps_to_jz_battery(x) container_of((x), struct jz_battery_info, bat);

/*********************************************************************
 *		Power
 *********************************************************************/


static int jz_get_power_prop(struct jz_battery_info *bat_info,
			     struct power_supply *psy,
			     enum power_supply_property psp,
			     union power_supply_propval *val)
{
	int gpio;
	
	if (bat_info == 0 || bat_info->pdata == 0)
		return -EINVAL;
	gpio = (psy->type == POWER_SUPPLY_TYPE_MAINS) ?
		bat_info->pdata->dc_dect_gpio :
		bat_info->pdata->usb_dect_gpio;
	if (!gpio_is_valid(gpio))
		return -EINVAL;
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = !gpio_get_value(gpio);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int jz_usb_get_power_prop(struct power_supply *psy,
				 enum power_supply_property psp,
				 union power_supply_propval *val)
{
	struct jz_battery_info *bat_info = container_of(psy, struct jz_battery_info, usb);
	return jz_get_power_prop(bat_info, psy, psp, val);
}

static int jz_ac_get_power_prop(struct power_supply *psy,
				 enum power_supply_property psp,
				 union power_supply_propval *val)
{
	struct jz_battery_info *bat_info = container_of(psy, struct jz_battery_info, ac);
	return jz_get_power_prop(bat_info, psy, psp, val);
}


static enum power_supply_property jz_power_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static struct power_supply jz_ac = {
	.name = "ac",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.properties = jz_power_props,
	.num_properties = ARRAY_SIZE(jz_power_props),
	.get_property = jz_ac_get_power_prop,
};

static struct power_supply jz_usb = {
	.name = "usb",
	.type = POWER_SUPPLY_TYPE_USB,
	.properties = jz_power_props,
	.num_properties = ARRAY_SIZE(jz_power_props),
	.get_property = jz_usb_get_power_prop,
};


/*********************************************************************
 *		Battery properties
 *********************************************************************/

static long jz_read_bat(struct power_supply *psy)
{
	struct jz_battery_info *bat_info = ps_to_jz_battery(psy);
	enum jz_adc_battery_scale scale;

	if (bat_info->pdata->max_voltag > 2500000)
		scale = JZ_ADC_BATTERY_SCALE_7V5;
	else
		scale = JZ_ADC_BATTERY_SCALE_2V5;

	return jz4740_adc_read_battery_voltage(psy->dev->parent->parent, scale);
}

static int jz_bat_get_capacity(struct power_supply *psy)
{
	int ret;
	struct jz_battery_info *bat_info = ps_to_jz_battery(psy);

	ret = jz_read_bat(psy);

	if (ret < 0)
		return ret;

	ret = (ret - bat_info->pdata->min_voltag) * 100
		/ (bat_info->pdata->max_voltag - bat_info->pdata->min_voltag);

	if (ret > 100)
		ret = 100;
	else if (ret < 0)
		ret = 0;

	return ret;
}

static int jz_bat_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{
	struct jz_battery_info *bat_info = ps_to_jz_battery(psy)
	
	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = bat_info->bat_status;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = bat_info->pdata->batt_tech;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		if(jz_read_bat(psy) < bat_info->pdata->min_voltag) {
			dev_dbg(psy->dev, "%s: battery is dead,"
				"voltage too low!\n", __func__);
			val->intval = POWER_SUPPLY_HEALTH_DEAD;
		} else {
			dev_dbg(psy->dev, "%s: battery is good,"
				"voltage normal.\n", __func__);
			val->intval = POWER_SUPPLY_HEALTH_GOOD;
		}
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = jz_bat_get_capacity(psy);
		dev_dbg(psy->dev, "%s: battery_capacity = %d\n",
			__func__, val->intval);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = jz_read_bat(psy);
		if (val->intval < 0)
			return val->intval;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
		val->intval = bat_info->pdata->max_voltag;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
		val->intval = bat_info->pdata->min_voltag;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = 1;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static void jz_bat_external_power_changed(struct power_supply *psy)
{
	struct jz_battery_info *bat_info = ps_to_jz_battery(psy);
	
	cancel_delayed_work(&bat_info->bat_work);
	queue_delayed_work(bat_info->monitor_wqueue, &bat_info->bat_work, HZ / 8);
}

static char *status_text[] = {
	[POWER_SUPPLY_STATUS_UNKNOWN] =		"Unknown",
	[POWER_SUPPLY_STATUS_CHARGING] =	"Charging",
	[POWER_SUPPLY_STATUS_DISCHARGING] =	"Discharging",
	[POWER_SUPPLY_STATUS_NOT_CHARGING] =    "Not charging",
};

static void jz_bat_update(struct power_supply *psy)
{
	struct jz_battery_info *bat_info = ps_to_jz_battery(psy);
		
	int old_status = bat_info->bat_status;
	static unsigned long old_batt_vol = 0;
	unsigned long batt_vol = jz_read_bat(psy);
	
	mutex_lock(&bat_info->work_lock);

	if (gpio_is_valid(bat_info->pdata->charg_stat_gpio)) {
		if(!gpio_get_value(bat_info->pdata->charg_stat_gpio))
			bat_info->bat_status = POWER_SUPPLY_STATUS_CHARGING;
		else
			bat_info->bat_status = POWER_SUPPLY_STATUS_NOT_CHARGING;
		dev_dbg(psy->dev, "%s: battery status=%s\n",
			__func__, status_text[bat_info->bat_status]);
		
		if (old_status != bat_info->bat_status) {
			dev_dbg(psy->dev, "%s %s -> %s\n",
				psy->name,
				status_text[old_status],
				status_text[bat_info->bat_status]);

			power_supply_changed(psy);
		}
	}

	if (old_batt_vol - batt_vol > 50000) {
		dev_dbg(psy->dev, "voltage change : %ld -> %ld\n",
			old_batt_vol, batt_vol);
		power_supply_changed(psy);
		old_batt_vol = batt_vol;
	}

	mutex_unlock(&bat_info->work_lock);
}

static enum power_supply_property jz_bat_main_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_CAPACITY, /* in percents! */
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_PRESENT,
};

struct power_supply bat_ps = {
	.name			= "battery",
	.type			= POWER_SUPPLY_TYPE_BATTERY,
	.properties		= jz_bat_main_props,
	.num_properties		= ARRAY_SIZE(jz_bat_main_props),
	.get_property		= jz_bat_get_property,
	.external_power_changed = jz_bat_external_power_changed,
	.use_for_apm		= 1,
};

static void jz_bat_work(struct work_struct *work)
{
	/* query interval too small will increase system workload*/
	const int interval = HZ * 30;
	struct jz_battery_info *bat_info = container_of(work,struct jz_battery_info, bat_work.work);

	jz_bat_update(&bat_info->bat);
	queue_delayed_work(bat_info->monitor_wqueue,
			   &bat_info->bat_work, interval);
}

#ifdef CONFIG_PM
static int jz_bat_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct jz_battery_info *bat_info = platform_get_drvdata(pdev);
	
	bat_info->bat_status =  POWER_SUPPLY_STATUS_UNKNOWN;

	return 0;
}

static int jz_bat_resume(struct platform_device *pdev)
{
	struct jz_battery_info *bat_info = platform_get_drvdata(pdev);

	bat_info->bat_status =  POWER_SUPPLY_STATUS_UNKNOWN;

	cancel_delayed_work(&bat_info->bat_work);
	queue_delayed_work(bat_info->monitor_wqueue, &bat_info->bat_work, HZ/10);

	return 0;
}
#else
#define jz_bat_suspend NULL
#define jz_bat_resume NULL
#endif

static int jz_bat_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct jz_battery_info *bat_info;
	
	bat_info = kzalloc(sizeof(struct jz_battery_info), GFP_KERNEL);

	if (!bat_info) {
		return -ENOMEM;
	}

	if (!pdev->dev.platform_data) {
		dev_err(&pdev->dev, "Please set battery info\n");
		ret = -EINVAL;
		goto err_platform_data;
	}
	platform_set_drvdata(pdev, bat_info);
	bat_info->pdata = pdev->dev.platform_data;
	bat_info->bat = bat_ps;
	bat_info->usb = jz_usb;
	bat_info->ac =  jz_ac;
	mutex_init(&bat_info->work_lock);
	INIT_DELAYED_WORK(&bat_info->bat_work, jz_bat_work);

	if (gpio_is_valid(bat_info->pdata->dc_dect_gpio)) {
		ret = gpio_request(bat_info->pdata->dc_dect_gpio, "AC/DC DECT");
		if (ret) {
			dev_err(&pdev->dev, "ac/dc dect gpio request failed.\n");

			goto err_dc_gpio_request;
		}
		ret = gpio_direction_input(bat_info->pdata->dc_dect_gpio);
		if (ret) {
			dev_err(&pdev->dev, "ac/dc dect gpio direction failed.\n");

			goto err_dc_gpio_direction;
		}
	}

	if (gpio_is_valid(bat_info->pdata->usb_dect_gpio)) {
		ret = gpio_request(bat_info->pdata->usb_dect_gpio, "USB DECT");
		if (ret) {
			dev_err(&pdev->dev, "usb dect gpio request failed.\n");

			goto err_usb_gpio_request;
		}
		ret = gpio_direction_input(bat_info->pdata->usb_dect_gpio);
		if (ret) {
			dev_err(&pdev->dev, "usb dect gpio set direction failed.\n");
			goto err_usb_gpio_direction;
		}

		jz_gpio_disable_pullup(bat_info->pdata->usb_dect_gpio);
		/* TODO: Use generic gpio is better */
	}

	if (gpio_is_valid(bat_info->pdata->charg_stat_gpio)) {
		ret = gpio_request(bat_info->pdata->charg_stat_gpio, "CHARG STAT");
		if (ret) {
			dev_err(&pdev->dev, "charger state gpio request failed.\n");
			goto err_charg_gpio_request;
		}
		ret = gpio_direction_input(bat_info->pdata->charg_stat_gpio);
		if (ret) {
			dev_err(&pdev->dev, "charger state gpio set direction failed.\n");
			goto err_charg_gpio_direction;
		}
	}
	
	if (gpio_is_valid(bat_info->pdata->dc_dect_gpio)) {
		ret = power_supply_register(&pdev->dev, &bat_info->ac);
		if (ret) {
			dev_err(&pdev->dev, "power supply ac/dc register failed.\n");
			goto err_power_register_ac;
		}
	}

	if (gpio_is_valid(bat_info->pdata->usb_dect_gpio)) {
		ret = power_supply_register(&pdev->dev, &bat_info->usb);
		if (ret) {
			dev_err(&pdev->dev, "power supply usb register failed.\n");
			goto err_power_register_usb;
		}
	}

	if (gpio_is_valid(bat_info->pdata->charg_stat_gpio)) {
		ret = power_supply_register(&pdev->dev, &bat_info->bat);
		if (ret) {
			dev_err(&pdev->dev, "power supply battery register failed.\n");
			goto err_power_register_bat;
		} else {
			bat_info->monitor_wqueue = create_singlethread_workqueue("jz_battery");
			if (!bat_info->monitor_wqueue) {
				return -ESRCH;
			}
			queue_delayed_work(bat_info->monitor_wqueue, &bat_info->bat_work, HZ * 1);
		}
	}
	printk(KERN_INFO "jz_bat init success.\n");
	return ret;

err_power_register_bat:
	power_supply_unregister(&bat_info->usb);
err_power_register_usb:
	power_supply_unregister(&bat_info->ac);
err_power_register_ac:
err_charg_gpio_direction:
	gpio_free(bat_info->pdata->charg_stat_gpio);
err_charg_gpio_request:
err_usb_gpio_direction:
	gpio_free(bat_info->pdata->usb_dect_gpio);
err_usb_gpio_request:
err_dc_gpio_direction:
	gpio_free(bat_info->pdata->dc_dect_gpio);
err_dc_gpio_request:
err_platform_data:
	kfree(bat_info);
	return ret;
}

static int jz_bat_remove(struct platform_device *pdev)
{
	struct jz_battery_info *bat_info = platform_get_drvdata(pdev);
		
	if (bat_info->pdata) {
		if (gpio_is_valid(bat_info->pdata->dc_dect_gpio))
			gpio_free(bat_info->pdata->dc_dect_gpio);
		if (gpio_is_valid(bat_info->pdata->usb_dect_gpio))
			gpio_free(bat_info->pdata->usb_dect_gpio);
		if (gpio_is_valid(bat_info->pdata->charg_stat_gpio))
			gpio_free(bat_info->pdata->charg_stat_gpio);
	}

	power_supply_unregister(&bat_ps);
	power_supply_unregister(&jz_ac);
	power_supply_unregister(&jz_usb);

	return 0;
}

static struct platform_driver jz_bat_driver = {
	.probe		= jz_bat_probe,
	.remove		= __devexit_p(jz_bat_remove),
	.suspend	= jz_bat_suspend,
	.resume		= jz_bat_resume,
	.driver = {
		.name = "jz4740-battery",
		.owner = THIS_MODULE,
	},
};

static int __init jz_bat_init(void)
{
	return platform_driver_register(&jz_bat_driver);
}
module_init(jz_bat_init);

static void __exit jz_bat_exit(void)
{
	platform_driver_unregister(&jz_bat_driver);
}
module_exit(jz_bat_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jiejing Zhang <kzjeef@gmail.com>");
MODULE_DESCRIPTION("JZ4720/JZ4740 SoC battery driver");
