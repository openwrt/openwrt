/*
 * Driver for batteries with bq27000 chips inside via HDQ
 *
 * Copyright 2008 Openmoko, Inc
 * Andy Green <andy@openmoko.com>
 *
 * based on ds2760 driver, original copyright notice for that --->
 *
 * Copyright © 2007 Anton Vorontsov
 *	       2004-2007 Matt Reimer
 *	       2004 Szabolcs Gyurko
 *
 * Use consistent with the GNU GPL is permitted,
 * provided that this copyright notice is
 * preserved in its entirety in all copies and derived works.
 *
 * Author:  Anton Vorontsov <cbou@mail.ru>
 *	    February 2007
 *
 *	    Matt Reimer <mreimer@vpop.net>
 *	    April 2004, 2005, 2007
 *
 *	    Szabolcs Gyurko <szabolcs.gyurko@tlt.hu>
 *	    September 2004
 */

#include <linux/module.h>
#include <linux/param.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/bq27000_battery.h>

enum bq27000_regs {
	/* RAM regs */
		/* read-write after this */
	BQ27000_CTRL = 0, /* Device Control Register */
	BQ27000_MODE, /* Device Mode Register */
	BQ27000_AR_L, /* At-Rate H L */
	BQ27000_AR_H,
		/* read-only after this */
	BQ27000_ARTTE_L, /* At-Rate Time To Empty H L */
	BQ27000_ARTTE_H,
	BQ27000_TEMP_L, /* Reported Temperature H L */
	BQ27000_TEMP_H,
	BQ27000_VOLT_L, /* Reported Voltage H L */
	BQ27000_VOLT_H,
	BQ27000_FLAGS, /* Status Flags */
	BQ27000_RSOC, /* Relative State of Charge */
	BQ27000_NAC_L, /* Nominal Available Capacity H L */
	BQ27000_NAC_H,
	BQ27000_CACD_L, /* Discharge Compensated H L */
	BQ27000_CACD_H,
	BQ27000_CACT_L, /* Temperature Compensated H L */
	BQ27000_CACT_H,
	BQ27000_LMD_L, /* Last measured discharge H L */
	BQ27000_LMD_H,
	BQ27000_AI_L, /* Average Current H L */
	BQ27000_AI_H,
	BQ27000_TTE_L, /* Time to Empty H L */
	BQ27000_TTE_H,
	BQ27000_TTF_L, /* Time to Full H L */
	BQ27000_TTF_H,
	BQ27000_SI_L, /* Standby Current H L */
	BQ27000_SI_H,
	BQ27000_STTE_L, /* Standby Time To Empty H L */
	BQ27000_STTE_H,
	BQ27000_MLI_L, /* Max Load Current H L */
	BQ27000_MLI_H,
	BQ27000_MLTTE_L, /* Max Load Time To Empty H L */
	BQ27000_MLTTE_H,
	BQ27000_SAE_L, /* Available Energy H L */
	BQ27000_SAE_H,
	BQ27000_AP_L, /* Available Power H L */
	BQ27000_AP_H,
	BQ27000_TTECP_L, /* Time to Empty at Constant Power H L */
	BQ27000_TTECP_H,
	BQ27000_CYCL_L, /* Cycle count since learning cycle H L */
	BQ27000_CYCL_H,
	BQ27000_CYCT_L, /* Cycle Count Total H L */
	BQ27000_CYCT_H,
	BQ27000_CSOC, /* Compensated State Of Charge */
	/* EEPROM regs */
		/* read-write after this */
	BQ27000_EE_EE_EN = 0x6e, /* EEPROM Program Enable */
	BQ27000_EE_ILMD = 0x76, /* Initial Last Measured Discharge High Byte */
	BQ27000_EE_SEDVF, /* Scaled EDVF Threshold */
	BQ27000_EE_SEDV1, /* Scaled EDV1 Threshold */
	BQ27000_EE_ISLC, /* Initial Standby Load Current */
	BQ27000_EE_DMFSD, /* Digital Magnitude Filter and Self Discharge */
	BQ27000_EE_TAPER, /* Aging Estimate Enable, Charge Termination Taper */
	BQ27000_EE_PKCFG, /* Pack Configuration Values */
	BQ27000_EE_IMLC, /* Initial Max Load Current or ID #3 */
	BQ27000_EE_DCOMP, /* Discharge rate compensation constants or ID #2 */
	BQ27000_EE_TCOMP, /* Temperature Compensation constants or ID #1 */
};

enum bq27000_status_flags {
	BQ27000_STATUS_CHGS = 0x80, /* 1 = being charged */
	BQ27000_STATUS_NOACT = 0x40, /* 1 = no activity */
	BQ27000_STATUS_IMIN = 0x20, /* 1 = Lion taper current mode */
	BQ27000_STATUS_CI = 0x10, /* 1 = capacity likely  innacurate */
	BQ27000_STATUS_CALIP = 0x08, /* 1 = calibration in progress */
	BQ27000_STATUS_VDQ = 0x04, /* 1 = capacity should be accurate */
	BQ27000_STATUS_EDV1 = 0x02, /* 1 = end of discharge.. <6% left */
	BQ27000_STATUS_EDVF = 0x01, /* 1 = no, it's really empty now */
};

#define NANOVOLTS_UNIT 3750

struct bq27000_bat_regs {
	int		ai;
	int		flags;
	int		lmd;
	int		rsoc;
	int		temp;
	int		tte;
	int		ttf;
	int		volt;
};

struct bq27000_device_info {
	struct device *dev;
	struct power_supply bat;
	struct power_supply ac;
	struct power_supply usb;
	struct delayed_work work;
	struct bq27000_platform_data *pdata;

	struct bq27000_bat_regs regs;
};

static unsigned int cache_time = 5000;
module_param(cache_time, uint, 0644);
MODULE_PARM_DESC(cache_time, "cache time in milliseconds");

/*
 * reading 16 bit values over HDQ has a special hazard where the
 * hdq device firmware can update the 16-bit register during the time we
 * read the two halves.  TI document SLUS556D recommends the algorithm here
 * to avoid trouble
 */

static int hdq_read16(struct bq27000_device_info *di, int address)
{
	int acc;
	int high;
	int retries = 3;

	while (retries--) {

		high = (di->pdata->hdq_read)(address + 1); /* high part */

		if (high < 0)
			return high;
		acc = (di->pdata->hdq_read)(address);
		if (acc < 0)
			return acc;

		/* confirm high didn't change between reading it and low */
		if (high == (di->pdata->hdq_read)(address + 1))
			return (high << 8) | acc;
	}

	return -ETIME;
}

static void bq27000_battery_external_power_changed(struct power_supply *psy)
{
	struct bq27000_device_info *di = container_of(psy, struct bq27000_device_info, bat);

	dev_dbg(di->dev, "%s\n", __FUNCTION__);
	schedule_delayed_work(&di->work, 0);
}

static int bq27000_battery_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	int n;
	struct bq27000_device_info *di = container_of(psy, struct bq27000_device_info, bat);

	if (di->regs.rsoc < 0 && psp != POWER_SUPPLY_PROP_PRESENT)
		return -ENODEV;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = POWER_SUPPLY_STATUS_UNKNOWN;

		if (!di->pdata->get_charger_online_status)
			goto use_bat;
		if ((di->pdata->get_charger_online_status)()) {
			/*
			 * charger is definitively present
			 * we report our state in terms of what it says it
			 * is doing
			 */
			if (!di->pdata->get_charger_active_status)
				goto use_bat;

			if ((di->pdata->get_charger_active_status)()) {
				val->intval = POWER_SUPPLY_STATUS_CHARGING;
				break;
			}
			val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
			break;
		}

		/*
		 * platform provided definite indication of charger presence,
		 * and it is telling us it isn't there... but we are on so we
		 * must be running from battery --->
		 */

		val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		break;

use_bat:
		/*
		 * either the charger is not connected, or the
		 * platform doesn't give info about charger, use battery state
		 * but... battery state can be out of date by 4 seconds or
		 * so... use the platform callbacks if possible.
		 */

		/* no real activity on the battery */
		if (di->regs.ai < 2) {
			if (!di->regs.ttf)
				val->intval = POWER_SUPPLY_STATUS_FULL;
			else
				val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
			break;
		}
		/* power is actually going in or out... */
		if (di->regs.flags < 0)
			return di->regs.flags;
		if (di->regs.flags & BQ27000_STATUS_CHGS)
			val->intval = POWER_SUPPLY_STATUS_CHARGING;
		else
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = POWER_SUPPLY_HEALTH_UNKNOWN;
		/* Do we have accurate readings... */
		if (di->regs.flags < 0)
			return di->regs.flags;
		if (di->regs.flags & BQ27000_STATUS_VDQ)
			val->intval = POWER_SUPPLY_HEALTH_GOOD;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		if (di->regs.volt < 0)
			return di->regs.volt;
		/* mV -> uV */
		val->intval = di->regs.volt * 1000;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		if (di->regs.flags < 0)
			return di->regs.flags;
		if (di->regs.flags & BQ27000_STATUS_CHGS)
			n = -NANOVOLTS_UNIT;
		else
			n = NANOVOLTS_UNIT;
		if (di->regs.ai < 0)
			return di->regs.ai;
		val->intval = (di->regs.ai * n) / di->pdata->rsense_mohms;
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL:
		if (di->regs.lmd < 0)
			return di->regs.lmd;
		val->intval = (di->regs.lmd * 3570) / di->pdata->rsense_mohms;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		if (di->regs.temp < 0)
			return di->regs.temp;
		/* K (in 0.25K units) is 273.15 up from C (in 0.1C)*/
		/* 10926 = 27315 * 4 / 10 */
		val->intval = (((long)di->regs.temp * 10l) - 10926) / 4;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = di->regs.rsoc;
		if (val->intval < 0)
			return val->intval;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = !(di->regs.rsoc < 0);
		break;
	case POWER_SUPPLY_PROP_TIME_TO_EMPTY_NOW:
		if (di->regs.tte < 0)
			return di->regs.tte;
		val->intval = 60 * di->regs.tte;
		break;
	case POWER_SUPPLY_PROP_TIME_TO_FULL_NOW:
		if (di->regs.ttf < 0)
			return di->regs.ttf;
		val->intval = 60 * di->regs.ttf;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		if (di->pdata->get_charger_online_status)
			val->intval = (di->pdata->get_charger_online_status)();
		else
			return -EINVAL;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static void bq27000_battery_work(struct work_struct *work)
{
	struct bq27000_device_info *di =
		container_of(work, struct bq27000_device_info, work.work);

	if ((di->pdata->hdq_initialized)()) {
		struct bq27000_bat_regs regs;

		regs.ai    = hdq_read16(di, BQ27000_AI_L);
		regs.flags = (di->pdata->hdq_read)(BQ27000_FLAGS);
		regs.lmd   = hdq_read16(di, BQ27000_LMD_L);
		regs.rsoc  = (di->pdata->hdq_read)(BQ27000_RSOC);
		regs.temp  = hdq_read16(di, BQ27000_TEMP_L);
		regs.tte   = hdq_read16(di, BQ27000_TTE_L);
		regs.ttf   = hdq_read16(di, BQ27000_TTF_L);
		regs.volt  = hdq_read16(di, BQ27000_VOLT_L);

		if (memcmp (&regs, &di->regs, sizeof(regs)) != 0) {
			di->regs = regs;
			power_supply_changed(&di->bat);
		}
	}

	if (!schedule_delayed_work(&di->work, cache_time))
		dev_err(di->dev, "battery service reschedule failed\n");
}

static enum power_supply_property bq27000_battery_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CHARGE_FULL,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_TIME_TO_EMPTY_NOW,
	POWER_SUPPLY_PROP_TIME_TO_FULL_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_ONLINE
};

static int bq27000_battery_probe(struct platform_device *pdev)
{
	int retval = 0;
	struct bq27000_device_info *di;
	struct bq27000_platform_data *pdata;

	dev_info(&pdev->dev, "BQ27000 Battery Driver (C) 2008 Openmoko, Inc\n");

	di = kzalloc(sizeof(*di), GFP_KERNEL);
	if (!di) {
		retval = -ENOMEM;
		goto di_alloc_failed;
	}

	platform_set_drvdata(pdev, di);

	pdata = pdev->dev.platform_data;
	di->dev		= &pdev->dev;
	/* di->w1_dev	     = pdev->dev.parent; */
	di->bat.name	   = pdata->name;
	di->bat.type	   = POWER_SUPPLY_TYPE_BATTERY;
	di->bat.properties     = bq27000_battery_props;
	di->bat.num_properties = ARRAY_SIZE(bq27000_battery_props);
	di->bat.get_property   = bq27000_battery_get_property;
	di->bat.external_power_changed =
				  bq27000_battery_external_power_changed;
	di->bat.use_for_apm = 1;
	di->pdata = pdata;

	retval = power_supply_register(&pdev->dev, &di->bat);
	if (retval) {
		dev_err(di->dev, "failed to register battery\n");
		goto batt_failed;
	}

	INIT_DELAYED_WORK(&di->work, bq27000_battery_work);

	if (!schedule_delayed_work(&di->work, 0))
		dev_err(di->dev, "failed to schedule bq27000_battery_work\n");

	return 0;

batt_failed:
	kfree(di);
di_alloc_failed:
	return retval;
}

static int bq27000_battery_remove(struct platform_device *pdev)
{
	struct bq27000_device_info *di = platform_get_drvdata(pdev);

	cancel_delayed_work(&di->work);

	power_supply_unregister(&di->bat);

	return 0;
}

void bq27000_charging_state_change(struct platform_device *pdev)
{
	struct bq27000_device_info *di = platform_get_drvdata(pdev);

	if (!di)
	    return;
}
EXPORT_SYMBOL_GPL(bq27000_charging_state_change);

#ifdef CONFIG_PM

static int bq27000_battery_suspend(struct platform_device *pdev,
				  pm_message_t state)
{
	struct bq27000_device_info *di = platform_get_drvdata(pdev);

	cancel_delayed_work(&di->work);
	return 0;
}

static int bq27000_battery_resume(struct platform_device *pdev)
{
	struct bq27000_device_info *di = platform_get_drvdata(pdev);

	schedule_delayed_work(&di->work, 0);
	return 0;
}

#else

#define bq27000_battery_suspend NULL
#define bq27000_battery_resume NULL

#endif /* CONFIG_PM */

static struct platform_driver bq27000_battery_driver = {
	.driver = {
		.name = "bq27000-battery",
	},
	.probe	  = bq27000_battery_probe,
	.remove   = bq27000_battery_remove,
	.suspend  = bq27000_battery_suspend,
	.resume	  = bq27000_battery_resume,
};

static int __init bq27000_battery_init(void)
{
	return platform_driver_register(&bq27000_battery_driver);
}

static void __exit bq27000_battery_exit(void)
{
	platform_driver_unregister(&bq27000_battery_driver);
}

module_init(bq27000_battery_init);
module_exit(bq27000_battery_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy Green <andy@openmoko.com>");
MODULE_DESCRIPTION("bq27000 battery driver");
