/*
 * Driver model for batteries
 *
 *	© 2006 David Woodhouse <dwmw2@infradead.org>
 *
 * Based on LED Class support, by John Lenz and Richard Purdie:
 *
 *	© 2005 John Lenz <lenz@cs.wisc.edu>
 *	© 2005-2006 Richard Purdie <rpurdie@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __LINUX_BATTERY_H__
#define __LINUX_BATTERY_H__

struct device;
struct class_device;

/*
 * Battery Core
 */
#define PWRDEV_TYPE_BATTERY	0
#define PWRDEV_TYPE_AC		1

#define BAT_STAT_PRESENT	(1<<0)
#define BAT_STAT_LOW		(1<<1)
#define BAT_STAT_FULL		(1<<2)
#define BAT_STAT_CHARGING	(1<<3)
#define BAT_STAT_DISCHARGING	(1<<4)
#define BAT_STAT_OVERTEMP	(1<<5)
#define BAT_STAT_CRITICAL	(1<<6)
#define BAT_STAT_FIRE		(1<<7)
#define BAT_STAT_CHARGE_DONE	(1<<8)

/* Thou shalt not export any attributes in sysfs except these, and
   with these units: */
#define BAT_INFO_STATUS		"status"		/* Not free-form. Use
							   provided function */
#define BAT_INFO_TEMP1		"temp1"			/* °C/1000 */
#define BAT_INFO_TEMP1_NAME	"temp1_name"		/* string */

#define BAT_INFO_TEMP2		"temp2"			/* °C/1000 */
#define BAT_INFO_TEMP2_NAME	"temp2_name"		/* string */

#define BAT_INFO_VOLTAGE	"voltage"		/* mV */
#define BAT_INFO_VOLTAGE_DESIGN	"voltage_design"	/* mV */

#define BAT_INFO_CURRENT	"current"		/* mA */
#define BAT_INFO_CURRENT_NOW	"current_now"		/* mA */

#define BAT_INFO_POWER		"power"			/* mW */
#define BAT_INFO_POWER_NOW	"power_now"		/* mW */

/* The following capacity/charge properties are represented in either
   mA or mW. The CAP_UNITS property MUST be provided if any of these are. */
#define BAT_INFO_RATE		"rate"			/* CAP_UNITS */
#define BAT_INFO_CAP_LEFT	"capacity_left"		/* CAP_UNITS*h */
#define BAT_INFO_CAP_DESIGN	"capacity_design"	/* CAP_UNITS*h */
#define BAT_INFO_CAP_LAST_FULL	"capacity_last_full"	/* CAP_UNITS*h */
#define BAT_INFO_CAP_LOW	"capacity_low_thresh"	/* CAP_UNITS*h */
#define BAT_INFO_CAP_WARN	"capacity_warn_thresh"	/* CAP_UNITS*h */
#define BAT_INFO_CAP_UNITS	"capacity_units"	/* string: must be
							   either mA or mW */
	
#define BAT_INFO_CAP_PCT	"capacity_percentage"	/* integer */

#define BAT_INFO_TIME_EMPTY	"time_to_empty"		/* seconds */
#define BAT_INFO_TIME_EMPTY_NOW	"time_to_empty_now"	/* seconds */
#define BAT_INFO_TIME_FULL	"time_to_full"		/* seconds */
#define BAT_INFO_TIME_FULL_NOW	"time_to_full_now"	/* seconds */

#define BAT_INFO_MANUFACTURER	"manufacturer"		/* string */
#define BAT_INFO_TECHNOLOGY	"technology"		/* string */
#define BAT_INFO_MODEL		"model"			/* string */
#define BAT_INFO_SERIAL		"serial"		/* string */
#define BAT_INFO_OEM_INFO	"oem_info"		/* string */

#define BAT_INFO_CYCLE_COUNT	"cycle_count"		/* integer */
#define BAT_INFO_DATE_MFR	"date_manufactured"	/* YYYY[-MM[-DD]] */
#define BAT_INFO_DATE_FIRST_USE	"date_first_use"	/* YYYY[-MM[-DD]] */

struct battery_dev {
	int			status_cap;
	int			id;
	int			type;
	const char		*name;

	struct device		*dev;
};

int battery_device_register(struct device *parent,
			    struct battery_dev *battery_cdev);
void battery_device_unregister(struct battery_dev *battery_cdev);


ssize_t battery_attribute_show_status(char *buf, unsigned long status);
ssize_t battery_attribute_show_ac_status(char *buf, unsigned long status);
#endif /* __LINUX_BATTERY_H__ */
