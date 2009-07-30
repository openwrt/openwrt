/*
 * GTA02 WLAN power management
 *
 * (C) 2008, 2009 by Openmoko Inc.
 * Author: Andy Green <andy@openmoko.com>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>

#include <mach/hardware.h>
#include <asm/mach-types.h>

#include <mach/gta02.h>
#include <mach/gta02-pm-wlan.h>
#include <mach/regs-gpio.h>
#include <mach/regs-gpioj.h>

#include <linux/delay.h>
#include <linux/rfkill.h>


/* ----- Module hardware reset ("power") ----------------------------------- */


void gta02_wlan_reset(int assert_reset)
{
	if (assert_reset) {
		s3c2410_gpio_setpin(GTA02_GPIO_nWLAN_RESET, 0);
		msleep(200); /* probably excessive but we don't have specs */
	} else {
		s3c2410_gpio_setpin(GTA02_GPIO_nWLAN_RESET, 1);
	}
}

/* ----- rfkill ------------------------------------------------------------ */

/*
 * S3C MCI handles suspend/resume through device removal/insertion. In order to
 * preserve rfkill state, as required in clause 7 of section 3.1 in rfkill.txt,
 * we therefore need to maintain rfkill state outside the driver.
 *
 * This platform driver is as good a place as any other.
 */

static int (*gta02_wlan_rfkill_cb)(void *user, int on);
static void *gta02_wlan_rfkill_user;
static DEFINE_MUTEX(gta02_wlan_rfkill_lock);
static int gta02_wlan_rfkill_on;

/*
 * gta02_wlan_query_rfkill_lock is used to obtain the rfkill state before the
 * driver is ready to process rfkill callbacks. To prevent the state from
 * changing until the driver has completed its initialization, we grab and hold
 * the rfkill lock.
 *
 * A call to gta02_wlan_query_rfkill_lock must be followed by either
 * - a call to gta02_wlan_set_rfkill_cb, to complete the setup, or
 * - a call to gta02_wlan_query_rfkill_unlock to abort the setup process.
 */

int gta02_wlan_query_rfkill_lock(void)
{
	mutex_lock(&gta02_wlan_rfkill_lock);
	return gta02_wlan_rfkill_on;
}
EXPORT_SYMBOL_GPL(gta02_wlan_query_rfkill_lock);

void gta02_wlan_query_rfkill_unlock(void)
{
	mutex_unlock(&gta02_wlan_rfkill_lock);
}
EXPORT_SYMBOL_GPL(gta02_wlan_query_rfkill_unlock);

void gta02_wlan_set_rfkill_cb(int (*cb)(void *user, int on), void *user)
{
	BUG_ON(!mutex_is_locked(&gta02_wlan_rfkill_lock));
	BUG_ON(gta02_wlan_rfkill_cb);
	gta02_wlan_rfkill_cb = cb;
	gta02_wlan_rfkill_user = user;
	mutex_unlock(&gta02_wlan_rfkill_lock);
}
EXPORT_SYMBOL_GPL(gta02_wlan_set_rfkill_cb);

void gta02_wlan_clear_rfkill_cb(void)
{
	mutex_lock(&gta02_wlan_rfkill_lock);
	BUG_ON(!gta02_wlan_rfkill_cb);
	gta02_wlan_rfkill_cb = NULL;
	mutex_unlock(&gta02_wlan_rfkill_lock);
}
EXPORT_SYMBOL_GPL(gta02_wlan_clear_rfkill_cb);

static int gta02_wlan_toggle_radio(void *data, enum rfkill_state state)
{
	struct device *dev = data;
	int on = state == RFKILL_STATE_UNBLOCKED;
	int res = 0;

	dev_dbg(dev, "gta02_wlan_toggle_radio: state %d (%p)\n",
	    state, gta02_wlan_rfkill_cb);
	mutex_lock(&gta02_wlan_rfkill_lock);
	if (gta02_wlan_rfkill_cb)
		res = gta02_wlan_rfkill_cb(gta02_wlan_rfkill_user, on);
	if (!res)
		gta02_wlan_rfkill_on = on;
	mutex_unlock(&gta02_wlan_rfkill_lock);
	return res;
}


/* ----- Initialization/removal -------------------------------------------- */


static int __init gta02_wlan_probe(struct platform_device *pdev)
{
	/* default-on for now */
	const int default_state = 1;
	struct rfkill *rfkill;
	int error;

	dev_info(&pdev->dev, "starting\n");

	s3c2410_gpio_cfgpin(GTA02_GPIO_nWLAN_RESET, S3C2410_GPIO_OUTPUT);
	gta02_wlan_reset(1);
	gta02_wlan_reset(0);

	rfkill = rfkill_allocate(&pdev->dev, RFKILL_TYPE_WLAN);
	rfkill->name = "ar6000";
	rfkill->data = &pdev->dev;
	rfkill->state = default_state ? RFKILL_STATE_ON : RFKILL_STATE_OFF;
	/*
	 * If the WLAN driver somehow managed to get activated before we're
	 * ready, the driver is now in an unknown state, which isn't something
	 * we're prepared to handle. This can't happen, so just fail hard.
	 */
	BUG_ON(gta02_wlan_rfkill_cb);
	gta02_wlan_rfkill_on = default_state;
	rfkill->toggle_radio = gta02_wlan_toggle_radio;

	error = rfkill_register(rfkill);
	if (error) {
		rfkill_free(rfkill);
		return error;
	}

	dev_set_drvdata(&pdev->dev, rfkill);

	return 0;
}

static int gta02_wlan_remove(struct platform_device *pdev)
{
	struct rfkill *rfkill = dev_get_drvdata(&pdev->dev);

	rfkill_unregister(rfkill);
	rfkill_free(rfkill);

	return 0;
}

static struct platform_driver gta02_wlan_driver = {
	.probe		= gta02_wlan_probe,
	.remove		= gta02_wlan_remove,
	.driver		= {
		.name		= "gta02-pm-wlan",
	},
};

static int __devinit gta02_wlan_init(void)
{
	return platform_driver_register(&gta02_wlan_driver);
}

static void gta02_wlan_exit(void)
{
	platform_driver_unregister(&gta02_wlan_driver);
}

module_init(gta02_wlan_init);
module_exit(gta02_wlan_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy Green <andy@openmoko.com>");
MODULE_DESCRIPTION("Openmoko GTA02 WLAN power management");
