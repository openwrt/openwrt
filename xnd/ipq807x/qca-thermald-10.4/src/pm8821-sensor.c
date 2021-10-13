/*===========================================================================

  pm8821-sensor.c

  DESCRIPTION
  pm8821 temperature alarm access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>

#include "pm8821-sensor.h"
#include "thermal.h"

/* Stage 2 Alarm thresholds */
#define PM8821_STAGE_2_TEMP_TRIG  127
#define PM8821_STAGE_2_TEMP_CLR  123

struct pmic_data {
	pthread_t pmic_thread;
	pthread_mutex_t pmic_mutex;
	pthread_cond_t pmic_condition;
	int threshold_reached;
	int temp_idx;
	int sensor_shutdown;
	sensor_t *sensor;
} pmic_data;

static void *pmic_uevent(void *data)
{
	int err = 0;
	sensor_t *sensor = (sensor_t *)data;
	struct pollfd fds;
	int fd;
	char uevent[MAX_PATH] = {0};
	char buf[MAX_PATH] = {0};
	struct pmic_data *pmic = NULL;

	if (NULL == sensor ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return NULL;
	}
	pmic = (struct pmic_data *) sensor->data;

	/* Looking for pmic uevent */
	snprintf(uevent, MAX_PATH, TZ_TYPE, pmic->sensor->tzn);
	fd = open(uevent, O_RDONLY);
	if (fd < 0) {
		msg("Unable to open %s to receive notifications.\n", uevent);
		return NULL;
	};

	while (!pmic->sensor_shutdown) {
		fds.fd = fd;
		fds.events = POLLERR|POLLPRI;
		fds.revents = 0;
		err = poll(&fds, 1, -1);
		if (err == -1) {
			msg("Error in uevent poll.\n");
			break;
		}
		read(fd, buf, sizeof(buf));
		lseek(fd, 0, SEEK_SET);

		dbgmsg("pmic uevent :%s", buf);

		/* notify the waiting threads */
		pthread_mutex_lock(&(pmic->pmic_mutex));
		pmic->threshold_reached = 1;
		pthread_cond_broadcast(&(pmic->pmic_condition));
		pthread_mutex_unlock(&(pmic->pmic_mutex));
	}
	close(fd);

	return NULL;
}

void pm8821_interrupt_wait(sensor_setting_t *setting)
{
	struct pmic_data *pmic;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	pmic = (struct pmic_data *) setting->sensor->data;

	if (setting->sensor->interrupt_enable) {
		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&(pmic->pmic_mutex));
		while (!pmic->threshold_reached) {
			pthread_cond_wait(&(pmic->pmic_condition),
					&(pmic->pmic_mutex));
		}
		pmic->threshold_reached = 0;
		pthread_mutex_unlock(&(pmic->pmic_mutex));
	}
}


void pm8821_hotpug_threshold_override_default(sensor_setting_t *setting)
{
	int j;

	if (setting == NULL) {
		msg("pm8821_tz:unexpected NULL");
		return;
	}

	if (setting->desc == NULL)
		setting->desc = sensor_names[setting->id];

	for (j = 0; hotplug_map[j].sensor_name != NULL; j++) {
		if (!strcmp(hotplug_map[j].sensor_name, setting->desc))
			break;
	}

	if (hotplug_map[j].sensor_name == NULL) {
		msg("Hotplug_map for pm8821_tz is not configured");
		return;
	}

	setting->hotplug_lvl_trig = PM8821_STAGE_2_TEMP_TRIG;
	setting->hotplug_lvl_clr = PM8821_STAGE_2_TEMP_CLR;
	dbgmsg("pm8821_tz hotplug thresholds Override default");

	if (!setting->num_thresholds) {
		setting->num_thresholds = 1;
		setting->t[0].lvl_trig = CONV(PM8821_STAGE_2_TEMP_TRIG);
		setting->t[0].lvl_clr = CONV(PM8821_STAGE_2_TEMP_CLR);
	}
}

/* pm8821_tz sensor specific set up */
int pm8821_setup(sensor_setting_t *setting, sensor_t *sensor)
{
	int fd = -1;
	int sensor_count = 0;
	int tzn = 0;
	char name[MAX_PATH] = {0};
	struct pmic_data *pmic = NULL;

	if (setting == NULL) {
		msg("pm8821_tz:unexpected NULL");
		return 0;
	}

	tzn = get_tzn(sensor->name);
	if (tzn < 0) {
		msg("No thermal zone device found in the kernel for sensor %s\n", sensor->name);
		return sensor_count;
	}
	sensor->tzn = tzn;

	/* Allocate pmic data */
	pmic = (struct pmic_data *) malloc(sizeof(struct pmic_data));
	if (NULL == pmic) {
		msg("%s: malloc failed", __func__);
		return sensor_count;
	}
	memset(pmic, 0, sizeof(pmic_data));
	sensor->data = (void *) pmic;
	pmic->sensor = sensor;

	snprintf(name, MAX_PATH, TZ_TEMP, sensor->tzn);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		free(pmic);
		return sensor_count;
	}
	pmic->temp_idx = fd;
	sensor_count++;
	pthread_mutex_init(&(pmic->pmic_mutex), NULL);
	pthread_cond_init(&(pmic->pmic_condition), NULL);
	pmic->sensor_shutdown = 0;
	pmic->threshold_reached = 0;
	setting->disabled = 0;

	/* Override default hotplug thresholds with pm8821 stage 2 alarm values */
	pm8821_hotpug_threshold_override_default(setting);

	if (sensor->interrupt_enable) {
		pthread_create(&(pmic->pmic_thread), NULL,
				pmic_uevent, sensor);
	}

	return sensor_count;
}

int pm8821_get_temperature(sensor_setting_t *setting)
{
	struct pmic_data *pmic;
	int temp = 0;
	char buf[MAX_PATH] = {0};

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	pmic = (struct pmic_data *) setting->sensor->data;

	if (read(pmic->temp_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(pmic->temp_idx, 0, SEEK_SET);
	temp /= 1000;

	return CONV(temp);
}

void pm8821_shutdown(sensor_setting_t *setting)
{
	struct pmic_data *pmic;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	pmic = (struct pmic_data *) setting->sensor->data;
	pmic->sensor_shutdown = 1;
	if (setting->sensor->interrupt_enable)
		pthread_join(pmic->pmic_thread, NULL);
	free(pmic);
}
