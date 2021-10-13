/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "tsens-sensor.h"
#include "bcl-sensor.h"
#include "thermal.h"

char *sensor_names[] =
{
	"tsens_tz_sensor0",
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor3",
	"tsens_tz_sensor4",
	"tsens_tz_sensor5",
	"tsens_tz_sensor6",
	"tsens_tz_sensor7",
	"tsens_tz_sensor8",
	"tsens_tz_sensor9",
	"tsens_tz_sensor10",
	"bcl",
};

static sensor_t g_sensors[] = {
	{
		.name = "tsens_tz_sensor0",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor1",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor2",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor3",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor4",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor5",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor6",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor7",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor8",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor9",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor10",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "bcl",
		.setup = bcl_setup,
		.shutdown = bcl_shutdown,
		.get_temperature = bcl_get_diff_imax_ibat,
		.interrupt_wait = bcl_interrupt_wait,
		.update_thresholds = bcl_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
        },
};

int sensors_setup(thermal_setting_t *settings)
{
	int i = 0;
	int j = 0;
	int ret = 0;
	int sensor_count = 0;

	if (!settings)
		return sensor_count;

	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		for (j = 0; j < SENSOR_IDX_MAX; j++) {
			if (!settings->sensors[j].desc ||
			    strncmp(settings->sensors[j].desc, g_sensors[i].name,
				    SENSOR_NAME_MAX))
				continue;

			info("Sensor setup:[%s]\n", g_sensors[i].name);
			g_sensors[i].setting = &settings->sensors[j];
			settings->sensors[j].sensor = &g_sensors[i];

			ret = g_sensors[i].setup(&settings->sensors[j],
						 (sensor_t *)&g_sensors[i]);
			sensor_count += ret;

			if (!ret)
				break;

			if (!g_sensors[i].interrupt_enable)
				break;

			/* TSENS sensor threshold setup */
			if (strncmp(settings->sensors[j].desc,
				g_sensors[i].name, strlen("tsens_tz_sensor")) == 0) {
				tsens_sensor_update_thresholds(g_sensors[i].setting,
							       THRESHOLD_NOCHANGE, 0);
			}

			/* BCL sensor threshold setup */
			if (strncmp(settings->sensors[j].desc,
				g_sensors[i].name, strlen("bcl")) == 0) {
				bcl_enable(g_sensors[i].setting, 1);
				bcl_update_thresholds(g_sensors[i].setting, 0, 0);
			}

			break;
		}
	}

	return sensor_count;
}

void sensors_shutdown()
{
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		info("Sensor shutdown:[%s] \n", g_sensors[i].name);
		g_sensors[i].shutdown(g_sensors[i].setting);
	}
}

int sensor_threshold_trigger(int value, sensor_setting_t *sensor, int level)
{
	if (sensor->id == BCL) {
		if (value <= sensor->t[level].lvl_trig)
			return 1;
		else
			return 0;
	} else {
		if (value >= sensor->t[level].lvl_trig)
			return 1;
		else
			return 0;
	}
}

int sensor_threshold_clear(int value, sensor_setting_t *sensor, int level)
{
	if (sensor->id == BCL) {
		if (value >= sensor->t[level].lvl_clr)
			return 1;
		else
			return 0;
	} else {
		if (value <= sensor->t[level].lvl_clr)
			return 1;
		else
			return 0;
	}
}

int sensor_get_temperature(sensor_setting_t *setting)
{
	int temp = 0;

	if (setting == NULL ||
	    setting->sensor == NULL ||
	    setting->sensor->get_temperature == NULL) {
		return -EFAULT;
	}

	temp = setting->sensor->get_temperature(setting);
	dbgmsg("Sensor[%s] Temperature : %2.1f\n", setting->desc, RCONV(temp));

	return temp;
}

void sensor_wait(sensor_setting_t *setting)
{
	static int is_first_poll = 1;

	if (setting == NULL ||
		setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}
	if (setting->sensor->interrupt_enable &&
	    setting->sensor->interrupt_wait) {
		setting->sensor->interrupt_wait(setting);
	} else if (!is_first_poll) {
		usleep(setting->sampling_period_us);
	} else {
		is_first_poll = 0;
	}
}

void sensor_update_thresholds(sensor_setting_t *setting,
				int threshold_type, int level)
{
	if (setting == NULL ||
	    setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}
	if (setting->sensor->interrupt_enable == 0 ||
	    setting->sensor->update_thresholds == NULL)
		return;

	setting->sensor->update_thresholds(setting, threshold_type, level);
}
