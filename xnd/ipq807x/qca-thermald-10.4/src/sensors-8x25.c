/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "oncrpc.h"
#include "adc.h"
#include "adc_rpc.h"

#define MSM_ADC_NODE  "/dev/msm_adc"
#define EXTN_MSM_ONCRPC_CHANNEL	 (23)
#define EXTN_MSM_INVALID_TEMP  (-555)
#define EXTN_MSM_MAX_TEMP       (255)

char *sensor_names[] =
{
	"PMIC_THERM",
	"MSM_THERM",
};

static int adc_fd = -1;

int sensors_setup(thermal_setting_t *settings)
{
	struct msm_adc_lookup lookup;
	int rc, adc_value;
	uint32_t convert_value;

	oncrpc_init();
	oncrpc_task_start();

	/* 8x25 has two thermal mitigation solutions
	   a) pmic thermistor: uses dalrpc.
	   b) external thermistor: uses oncrpc.
	   both solutions are mutually exclusive.
	   Hence check oncrpc availability by issuing
	   adc_null. If the oncrpc is available, then check whether
	   msm thermistor rework is installed or not on the HW?
	   */
	if (adc_null()) {
		dbgmsg("oncrpc is available\n");
		/* look for msm thermistor rework if oncrpc is available
		   and check whether value is negative or not */
		convert_value = adc_read(EXTN_MSM_ONCRPC_CHANNEL);
		if (convert_value > EXTN_MSM_MAX_TEMP) {
			convert_value |= 0xFFFF0000;
		}
		adc_value = convert_value;
		if (adc_value == EXTN_MSM_INVALID_TEMP) {
			settings->sensors[MSM_THERM].disabled = 1;
			dbgmsg("Sensor '%s' is not present\n", sensor_names[MSM_THERM]);
			oncrpc_task_stop();
			oncrpc_deinit();
		}
		else {
			dbgmsg("Sensor '%s' is present\n", sensor_names[MSM_THERM]);
			settings->sensors[MSM_THERM].disabled = 0;
			return 1;
		}
	} else {
		dbgmsg("oncrpc is not available\n");
	}

	adc_fd = open(MSM_ADC_NODE, O_RDWR);
	if (adc_fd < 0) {
		msg("Error opening %s\n", MSM_ADC_NODE);
		return 0;
	}

	memset(&lookup, 0, sizeof(lookup));
	strlcpy(lookup.name, sensor_names[PMIC_THERM], MSM_ADC_MAX_CHAN_STR);
	lookup.name[MSM_ADC_MAX_CHAN_STR-1] = '\0';

	rc = ioctl(adc_fd, MSM_ADC_LOOKUP, &lookup);
	if (rc < 0) {
		msg("Error looking up '%s', skipping\n", sensor_names[PMIC_THERM]);
			settings->sensors[PMIC_THERM].disabled = 1;
		return 0;
	}

	settings->sensors[PMIC_THERM].chan_idx = lookup.chan_idx;
	settings->sensors[PMIC_THERM].disabled = 0;

	dbgmsg("Sensor '%s' -> channel '%u'\n",
		sensor_names[PMIC_THERM], lookup.chan_idx);

	return 1;
}

void sensors_shutdown()
{
	if (adc_fd != -1)
		close(adc_fd);
	adc_fd = -1;

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
	int rc = 0, adc_value;
	uint32_t convert_value;
	struct adc_chan_result conv;

	if (sensor->id == MSM_THERM) {
		convert_value = adc_read(EXTN_MSM_ONCRPC_CHANNEL);
		if (convert_value > EXTN_MSM_MAX_TEMP) {
			convert_value |= 0xFFFF0000;
		}
		adc_value = convert_value;
		/* external msm thermistor returns the temparature in degress,
		   but thermal daemon expects the temparature in millie degrees.
		   Hence convert from degress to miiliedegrees.*/
		dbgmsg("Sensor '%s' -  %d*C (%d)\n",sensor->desc, adc_value, CONV(adc_value));
		return CONV(adc_value);
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
		dbgmsg("Sensor '%s' - %2.1f*C (%lld)\n",
			sensor->desc, RCONV(conv.physical), conv.physical);
	}

	return conv.physical;
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
				int threshold_triggered, int level)
{
	return;
}
