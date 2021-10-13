/*===========================================================================

  tsens-sensor.c

  DESCRIPTION
  TSENS sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  setup() function should be called before get_temperature().
  shutdown() function should be called to clean up resources.

  Copyright (c) 2011 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>

#include "tsens-sensor.h"
#include "thermal.h"

#define LVL_BUF_MAX (12)

#define TSENS_TZ_TRIP_TYPE "/sys/devices/virtual/thermal/thermal_zone%d/trip_point_%d_type"
#define TSENS_TZ_TRIP_TEMP "/sys/devices/virtual/thermal/thermal_zone%d/trip_point_%d_temp"

#ifdef SENSORS_8974
#define TRIP_MAX 0 /* Trip point 0 is the high temperature */
#define TRIP_MIN 1 /* Trip point 1 is the low temperature */
#else
#define TRIP_MAX 1 /* Trip point 1 is the high temperature */
#define TRIP_MIN 2 /* Trip point 2 is the low temperature */
#endif

typedef struct tsens_data {
	pthread_t tsens_thread;
	pthread_mutex_t tsens_mutex;
	pthread_cond_t tsens_condition;
	int threshold_reached;
	int hi_threshold, lo_threshold;
	int hi_idx, lo_idx;
	int sensor_shutdown;
	sensor_t *sensor;
} tsens_data;

static void *tsens_uevent(void *data)
{
	int err = 0;
	sensor_t *sensor = (sensor_t *)data;
	struct pollfd fds;
	int fd;
	char uevent[MAX_PATH] = {0};
	char buf[MAX_PATH] = {0};
	tsens_data *tsens = NULL;

	if (NULL == sensor ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return NULL;
	}
	tsens = (tsens_data *) sensor->data;

	/* Looking for tsens uevent */
	snprintf(uevent, MAX_PATH, TZ_TYPE, sensor->tzn);

	fd = open(uevent, O_RDONLY);
	if (fd < 0) {
		msg("Unable to open %s to receive notifications.\n", uevent);
		return NULL;
	};

	while (!tsens->sensor_shutdown) {
		fds.fd = fd;
		fds.events = POLLERR|POLLPRI;
		fds.revents = 0;

		err = poll(&fds, 1, -1);
		if (err == -1) {
			msg("Error in uevent poll.\n");
			break;
		}

		if (read(fd, buf, sizeof(buf)) < 0) {
			dbgmsg("TSENS uevent read failed\n");
			close(fd);
			return NULL;
		}

		lseek(fd, 0, SEEK_SET);

		dbgmsg("TSENS uevent :%s", buf);

		/* notify the waiting threads */
		pthread_mutex_lock(&(tsens->tsens_mutex));
		tsens->threshold_reached = 1;
		pthread_cond_broadcast(&(tsens->tsens_condition));
		pthread_mutex_unlock(&(tsens->tsens_mutex));
	}
	close(fd);

	return NULL;
}

static void enable_sensor(int tzn , int enabled)
{
	int ret = 0;
	char name[MAX_PATH] = {0};

	snprintf( name, MAX_PATH, TZ_MODE, tzn );
	if (enabled)
		ret = write_to_file(name, "enabled", strlen("enabled"));
	else
		ret = write_to_file(name, "disabled", strlen("disabled"));

	if (ret <= 0) {
		msg("TSENS tzn %d failed to set mode %d\n", tzn, enabled);
	} else {
		dbgmsg("TSENS tzn %d mode set to %d\n", tzn, enabled);
	}
}

static void enable_threshold(tsens_data *tsens, int trip, int enabled)
{
	int ret = 0;
	char name[MAX_PATH] = {0};

	if (NULL == tsens ||
	    NULL == tsens->sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	snprintf(name, MAX_PATH, TSENS_TZ_TRIP_TYPE, tsens->sensor->tzn, trip);
	if (enabled)
		ret = write_to_file(name, "enabled", strlen("enabled"));
	else
		ret = write_to_file(name, "disabled", strlen("disabled"));

	if (ret <= 0) {
		msg("TSENS threshold at %d failed to %d\n", trip, enabled);
	} else {
		dbgmsg("TSENS threshold at %d enabled: %d\n", trip, enabled);
	}
}

static void set_thresholds(tsens_data *tsens, int lvl, int lvl_clr,
			   int hi_enable, int lo_enable)
{
	char minname[MAX_PATH]= {0};
	char maxname[MAX_PATH]= {0};
	char buf[LVL_BUF_MAX] = {0};
	int ret = 0;
	int mintemp = 0;

	if (NULL == tsens) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	snprintf(minname, MAX_PATH, TSENS_TZ_TRIP_TEMP, tsens->sensor->tzn, TRIP_MIN);
	snprintf(maxname, MAX_PATH, TSENS_TZ_TRIP_TEMP, tsens->sensor->tzn, TRIP_MAX);

	/* Set thresholds in legal order */
	if (read_line_from_file(minname, buf, sizeof(buf)) > 0) {
		mintemp = atoi(buf);
	}

	if (lvl >= mintemp) {
		/* set high threshold first */
		dbgmsg("Setting up TSENS thresholds high: %d\n", lvl);
		enable_threshold(tsens, TRIP_MAX, hi_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl);
		ret = write_to_file(maxname, buf, strlen(buf));
		if (ret <= 0)
			msg("TSENS threshold high failed to set %d\n", lvl);

		dbgmsg("Setting up TSENS thresholds low: %d\n", lvl_clr);
		enable_threshold(tsens, TRIP_MIN, lo_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl_clr);
		ret = write_to_file(minname, buf, strlen(buf));
		if (ret <= 0)
			msg("TSENS threshold low failed to set %d\n", lvl_clr);

	} else {
		dbgmsg("Setting up TSENS thresholds low: %d\n", lvl_clr);
		enable_threshold(tsens, TRIP_MIN, lo_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl_clr);
		ret = write_to_file(minname, buf, strlen(buf));
		if (ret <= 0)
			msg("TSENS threshold low failed to set %d\n", lvl_clr);

		dbgmsg("Setting up TSENS thresholds high: %d\n", lvl);
		enable_threshold(tsens, TRIP_MAX, hi_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl);
		ret = write_to_file(maxname, buf, strlen(buf));
		if (ret <= 0)
			msg("TSENS threshold high failed to set %d\n", lvl);
	}

	tsens->hi_threshold = lvl;
	tsens->lo_threshold = lvl_clr;
}

void tsens_sensor_enable_thresholds(sensor_setting_t *setting, int hi_enabled,
				    int lo_enabled)
{
#ifdef ENABLE_TSENS_INTERRUPT
	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("tsens: unexpected NULL");
		return;
	}

	enable_threshold(setting->sensor->data, TRIP_MAX, hi_enabled);
	enable_threshold(setting->sensor->data, TRIP_MIN, lo_enabled);
#endif
}

#ifdef IPQ_806x
void tsens_sensor_enable_sensor(int tzn, int enabled)
{
	if ( (0 >= tzn) && (SENSOR_IDX_MAX <= tzn) ){
		msg("%s: Invalid sensor id = %d\n" , __func__ , tzn );
		return;
	}

	enable_sensor(tzn, enabled);
}
#else
void tsens_sensor_enable_sensor(sensor_setting_t *setting, int enabled)
{
	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("tsens: unexpected NULL");
		return;
	}
	tsens_data *tsens = setting->sensor->data;
	enable_sensor( tsens->sensor->tzn, enabled);
}
#endif


/* NOTE: tsens_sensors_setup() function does not enable the sensor
 * or set thresholds. This should be done in the target-specific setup */
int tsens_sensors_setup(sensor_setting_t *setting, sensor_t *sensor)
{
	int fd = -1;
	int sensor_count = 0;
	char name[MAX_PATH] = {0};
	int tzn = 0;
	tsens_data *tsens = NULL;

	/* We have nothing to do if there are no thresholds */
	if (!setting->num_thresholds) {
		dbgmsg("No thresholds for sensor %s\n", sensor->name);
		return sensor_count;
	}

	tzn = get_tzn(sensor->name);
	if (tzn < 0) {
		msg("No thermal zone device found in the kernel for sensor %s\n", sensor->name);
		return sensor_count;
	}
	sensor->tzn = tzn;

	snprintf(name, MAX_PATH, TZ_TEMP, sensor->tzn);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, TZ_TEMP);
		return sensor_count;
	}

	/* Allocate TSENS data */
	tsens = (tsens_data *) malloc(sizeof(tsens_data));
	if (NULL == tsens) {
		msg("%s: malloc failed", __func__);
		close(fd);
		return sensor_count;
	}
	memset(tsens, 0, sizeof(tsens_data));
	sensor->data = (void *) tsens;

	sensor_count++;
	pthread_mutex_init(&(tsens->tsens_mutex), NULL);
	pthread_cond_init(&(tsens->tsens_condition), NULL);
	tsens->sensor_shutdown = 0;
	tsens->threshold_reached = 0;
	tsens->sensor = sensor;
	setting->disabled = 0;
	setting->chan_idx = fd;

	if (sensor->interrupt_enable) {
		pthread_create(&(tsens->tsens_thread), NULL,
			       tsens_uevent, sensor);
	}

	return sensor_count;
}

void tsens_sensors_shutdown(sensor_setting_t *setting)
{
	tsens_data *tsens;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	tsens = (tsens_data *) setting->sensor->data;
	tsens->sensor_shutdown = 1;

	if (setting->chan_idx > 0)
		close(setting->chan_idx);

	if (setting->sensor->interrupt_enable)
		pthread_join(tsens->tsens_thread, NULL);
	free(tsens);
}

int tsens_sensor_get_temperature(sensor_setting_t *setting)
{
	char buf[10] = {0};
	int temp = 0;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	if (read(setting->chan_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(setting->chan_idx, 0, SEEK_SET);

	return CONV(temp);
}

void tsens_sensor_interrupt_wait(sensor_setting_t *setting)
{
	tsens_data *tsens;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	if (setting->sensor->interrupt_enable) {
		tsens = (tsens_data *) setting->sensor->data;
		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&(tsens->tsens_mutex));
		while (!tsens->threshold_reached) {
			pthread_cond_wait(&(tsens->tsens_condition),
					&(tsens->tsens_mutex));
		}
		tsens->threshold_reached = 0;
		pthread_mutex_unlock(&(tsens->tsens_mutex));
	}
}

void tsens_sensor_update_thresholds(sensor_setting_t *setting,
				    int threshold_type, int level)
{
	int hi, lo;
	int hi_enable = 1;
	int lo_enable = 1;
	tsens_data *tsens;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	tsens = (tsens_data *) setting->sensor->data;

	dbgmsg("%s: threshold_type %d, level %d", __func__,
	       threshold_type, level);
	hi = RCONV(setting->t[tsens->hi_idx].lvl_trig);
	lo = RCONV(setting->t[tsens->lo_idx].lvl_clr);
	if (level >= setting->num_thresholds) {
		/* handle corner high case */
		hi = RCONV(setting->t[setting->num_thresholds - 1].lvl_trig);
		tsens->hi_idx = setting->num_thresholds - 1;
		hi_enable = 0;
	} else {
		hi = RCONV(setting->t[level].lvl_trig);
		tsens->hi_idx = level;
	}
	if (level <= 0) {
		/* handle corner low case */
		lo = RCONV(setting->t[0].lvl_clr);
		tsens->lo_idx = 0;
		lo_enable = 0;
	} else {
		lo = RCONV(setting->t[level - 1].lvl_clr);
		tsens->lo_idx = level - 1;
	}

	set_thresholds(tsens, hi, lo, hi_enable, lo_enable);
}
