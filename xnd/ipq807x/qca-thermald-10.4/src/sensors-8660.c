/*===========================================================================

  Copyright (c) 2010-2011 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "tsens-sensor.h"
#include "thermal.h"

char *sensor_names[] =
{
	"pm8058_tz",
	"tsens_tz_sensor0",
};

int pmic_sensors_setup(sensor_setting_t *settings, sensor_t* sensor);
void pmic_sensors_shutdown(sensor_setting_t *setting);
int pmic_sensor_get_temperature(sensor_setting_t *setting);

static sensor_t g_sensors[] = {
	{
		.name = "pm8058_tz",
		.setup = pmic_sensors_setup,
		.shutdown = pmic_sensors_shutdown,
		.get_temperature = pmic_sensor_get_temperature,
		.interrupt_wait = NULL,
		.update_thresholds = NULL,
		.setting = NULL,
		.tzn = 1,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor0",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.setting = NULL,
		.tzn = 3,
		.data = NULL,
#ifdef ENABLE_TSENS_INTERRUPT
		.interrupt_enable = 1,
#else
		.interrupt_enable = 0,
#endif
	},
};


/* PMIC sensor */
int pmic_sensors_setup(sensor_setting_t *setting, sensor_t* s)
{
	int adc_fd = -1;
	int sensor_count = 0;
	char name[MAX_PATH] = {0};
	int tzn = 0;
	sensor_t *sensor = (sensor_t *)s;

	tzn = get_tzn(sensor->name);
	if (tzn < 0) {
		msg("No thermal zone device found in the kernel for sensor %s\n", sensor->name);
		return sensor_count;
	}
	sensor->tzn = tzn;

	snprintf(name, MAX_PATH, TZ_TEMP, sensor->tzn);

	adc_fd = open(name, O_RDONLY);
	if (adc_fd > 0) {
		setting->disabled = 0;
		setting->chan_idx = adc_fd;
		sensor_count++;
	} else {
		msg("pmic: Error opening %s\n", name);
	}

	return sensor_count;
}

void pmic_sensors_shutdown(sensor_setting_t *setting)
{
	if (setting->chan_idx > 0)
		close(setting->chan_idx);
}

int pmic_sensor_get_temperature(sensor_setting_t *setting)
{
	char buf[10] = {0};
	int temp = 0;

	if (read(setting->chan_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(setting->chan_idx, 0, SEEK_SET);

	return temp;
}

int sensors_setup(thermal_setting_t *settings)
{
	int i = 0;
	int j = 0;
	int sensor_count = 0;
	int ret = 0;

	if (!settings)
		return sensor_count;

	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		for (j = 0; j < SENSOR_IDX_MAX; j++) {
			if (settings->sensors[j].desc == NULL ||
			    0 != strcmp(settings->sensors[j].desc, g_sensors[i].name))
				continue;

			info("Sensor setup:[%s]\n", g_sensors[i].name);
			g_sensors[i].setting = &settings->sensors[j];
			settings->sensors[j].sensor = &g_sensors[i];

			ret = g_sensors[i].setup(&settings->sensors[j],
						 (sensor_t *)&g_sensors[i]);
			sensor_count += ret;

			/* TSENS sensors need separate enabling and threshold set */
			if (!ret ||
			    0 != strcmp(g_sensors[i].name, "tsens_tz_sensor0"))
				break;
			tsens_sensor_enable_sensor(g_sensors[i].setting, 1);
			if (g_sensors[i].interrupt_enable) {
				tsens_sensor_update_thresholds(g_sensors[i].setting,
							       THRESHOLD_NOCHANGE, 0);
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
	if (value >= sensor->t[level].lvl_trig)
		return 1;
	else
		return 0;
}

int sensor_threshold_clear(int value, sensor_setting_t *sensor, int level)
{
	if (value <= sensor->t[level].lvl_clr)
		return 1;
	else
		return 0;
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
