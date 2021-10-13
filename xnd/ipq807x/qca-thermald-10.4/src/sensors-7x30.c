/*===========================================================================

  Copyright (c) 2010-2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <linux/msm_adc.h>
#include "thermal.h"
#include "sensors-hw.h"

#define MSM_ADC_NODE  "/dev/msm_adc"
#define POP_MEM_NODE  "/sys/devices/virtual/thermal/thermal_zone0/temp"

char *sensor_names[] =
{
	"PMIC_THERM",
	"XO_THERM",
	"XO_THERM_GPS",
	"POP_MEM_TZ",
};

static int adc_fd = -1;
static int pop_mem_fd = 0;

int sensors_setup(thermal_setting_t *settings)
{
	struct msm_adc_lookup lookup;
	int i = 0;
	int rc, numEnabled = 0;

	adc_fd = open(MSM_ADC_NODE, O_RDWR);
	if (adc_fd < 0) {
		msg("Error opening %s\n", MSM_ADC_NODE);
		i = POP_MEM_TZ;
	}

	for (; i < POP_MEM_TZ; i++) {
		memset(&lookup, 0, sizeof(lookup));
		strlcpy(lookup.name, sensor_names[i], MSM_ADC_MAX_CHAN_STR);
		lookup.name[MSM_ADC_MAX_CHAN_STR-1] = '\0';

		rc = ioctl(adc_fd, MSM_ADC_LOOKUP, &lookup);
		if (rc < 0) {
			msg("Error looking up '%s', skipping\n", sensor_names[i]);
			settings->sensors[i].disabled = 1;
			continue;
		}

		settings->sensors[i].chan_idx = lookup.chan_idx;
		settings->sensors[i].disabled = 0;
		numEnabled++;

		dbgmsg("Sensor '%s' -> channel '%u'\n",
			sensor_names[i], lookup.chan_idx);
	}

	pop_mem_fd = open(POP_MEM_NODE, O_RDONLY);
	if (pop_mem_fd > 0) {
		settings->sensors[i].chan_idx = pop_mem_fd;
		settings->sensors[i].disabled = 0;
		numEnabled++;
	} else {
		msg("Error(%d) opening %s\n", pop_mem_fd, POP_MEM_NODE);
		settings->sensors[i].chan_idx = -1;
		settings->sensors[i].disabled = 1;
	}

	return (numEnabled > 0) ? 1 : 0;
}

void sensors_shutdown()
{
	if (adc_fd != -1)
		close(adc_fd);
	adc_fd = -1;

	if (pop_mem_fd != -1)
		close(pop_mem_fd);
	pop_mem_fd = -1;
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

int sensor_get_temperature(sensor_setting_t *sensor)
{
	int rc = 0;
	struct msm_adc_conversion  conv;

	if (sensor->id == POP_MEM_TZ) {
		char buf[30] = {0};
		int temp = 0;

		lseek(sensor->chan_idx, 0, SEEK_SET);
		read(sensor->chan_idx, buf, sizeof(buf) - 1);
		temp = strtol(buf, NULL, 10);

		return temp;
	}

	memset(&conv, 0, sizeof(conv));
	conv.chan = sensor->chan_idx;

	if (adc_fd <= 0) {
		msg("Error opening sensor '%s'\n", sensor->desc);
		return -1;
	}

	rc = ioctl(adc_fd, MSM_ADC_REQUEST, &conv);
	if (rc) {
		msg("Error getting sensor info '%s'\n", sensor->desc);
	} else {
		dbgmsg("Sensor '%s' - %2.1f*C (%u)\n",
			sensor->desc, RCONV(conv.result), conv.result);
	}

	return conv.result;
}


void sensor_wait(sensor_setting_t *setting)
{
	static int is_first_poll = 1;
	if (setting == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	if (!is_first_poll) {
		usleep(setting->sampling_period_us);
	} else {
		is_first_poll = 0;
	}
}

void sensor_update_thresholds(sensor_setting_t *setting,
			      int threshold_type, int level)
{
	return;
}
