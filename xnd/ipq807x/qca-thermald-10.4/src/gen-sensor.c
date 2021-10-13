/*===========================================================================

  gen-sensor.c

  DESCRIPTION
  Generic thermal zone sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "thermal.h"

int gen_sensors_setup(sensor_setting_t *setting, sensor_t* s)
{
	int fd = -1;
	int sensor_count = 0;
	char name[MAX_PATH] = {0};
	int tzn = 0;
	sensor_t *sensor = (sensor_t *)s;

	tzn = get_tzn(sensor->name);
	if (tzn < 0) {
		msg("No thermal zone device found in the kernel for sensor %s\n", sensor->name);
		return sensor_count;
	}
	/* We have nothing to do if there are no thresholds */
	if (!setting->num_thresholds) {
		dbgmsg("No thresholds for sensor %s\n", sensor->name);
		return sensor_count;
	}

	sensor->tzn = tzn;
	snprintf(name, MAX_PATH, TZ_TEMP, sensor->tzn);

	fd = open(name, O_RDONLY);
	if (fd > -1) {
		setting->disabled = 0;
		setting->chan_idx = fd;
		sensor_count++;
	} else {
		msg("%s: Error opening %s\n", __func__, name);
	}

	return sensor_count;
}

void gen_sensors_shutdown(sensor_setting_t *setting)
{
	if (NULL == setting ||
	    NULL == setting->sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	if (setting->chan_idx > 0)
		close(setting->chan_idx);
}

int gen_sensor_get_temperature(sensor_setting_t *setting)
{
	char buf[10] = {0};
	int temp = 0;

	if (read(setting->chan_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(setting->chan_idx, 0, SEEK_SET);

	return temp;
}
