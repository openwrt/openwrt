/*===========================================================================

  adc-sensor.c

  DESCRIPTION
  ADC sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "thermal.h"

#define MSM_ADC_NODE_PM8921	"/sys/devices/platform/msm_ssbi.0/pm8921-core/pm8xxx-adc/%s"
#define MSM_ADC_NODE_PM8038	"/sys/devices/platform/msm_ssbi.0/pm8038-core/pm8xxx-adc/%s"
#define MSM_PM8921		"/sys/devices/platform/msm_ssbi.0/pm8921-core"
#define MSM_PM8038		"/sys/devices/platform/msm_ssbi.0/pm8038-core"
#define LVL_BUF_MAX (12)

static char *get_adc_sysfs()
{
	struct stat filestat;

	int msm_id = therm_get_msm_id();

	switch (msm_id) {
	case THERM_MSM_8960AB:
	case THERM_MSM_8960:
	case THERM_MSM_8064AB:
	case THERM_MSM_8064:
		return MSM_ADC_NODE_PM8921;
	case THERM_MSM_8930AB:
	case THERM_MSM_8930AA:
	case THERM_MSM_8930:
		if (stat(MSM_PM8038, &filestat) > -1)
			return MSM_ADC_NODE_PM8038;
		else if(stat(MSM_PM8921, &filestat) > -1)
			return MSM_ADC_NODE_PM8921;
		else
			return NULL;
	default:
		dbgmsg("%s: ADC sensors unsupported on target %d",
		       __func__, msm_id);
		return NULL;
	}
}

int adc_sensors_setup(sensor_setting_t *setting, sensor_t *sensor)
{
	int fd = -1;
	int sensor_count = 0;
	char name[MAX_PATH] = {0};
	char *adc_sysfs = NULL;

	/* We have nothing to do if there are no thresholds */
	if (!setting->num_thresholds) {
		dbgmsg("No thresholds for sensor %s\n", sensor->name);
		return sensor_count;
	}
	adc_sysfs = get_adc_sysfs();
	if (adc_sysfs == NULL) {
		return sensor_count;
	}

	snprintf(name, MAX_PATH, adc_sysfs, sensor->name);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		return sensor_count;
	}

	sensor_count++;
	setting->disabled = 0;
	setting->chan_idx = fd;

	return sensor_count;
}

void adc_sensors_shutdown(sensor_setting_t *setting)
{
	if (NULL == setting ||
	    NULL == setting->sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	if (setting->chan_idx > 0)
		close(setting->chan_idx);
}

int adc_sensor_get_temperature(sensor_setting_t *setting)
{
	char buf[3*LVL_BUF_MAX] = {0};
	int temp = 0;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	if (read(setting->chan_idx, buf, sizeof(buf) - 1) != -1) {
		sscanf(buf, "Result:%d Raw:%*d\n", &temp);
	}

	lseek(setting->chan_idx, 0, SEEK_SET);
	return CONV(temp);
}
