/*===========================================================================

  qmi-ts-sensor.c

  DESCRIPTION
  QMI TS sensor access functions.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_ts_setup() function should be called before qmi_ts_get_temperature().
  qmi_ts_shutdown() function should be called to clean up resources.

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "thermal.h"

#include "thermal_sensor_service_v01.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"

/* Specific qmi_ts sensor data */
typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t  condition;
	int init_thresh_set;
	int threshold_reached;
	int hi_idx, lo_idx;
	sensor_t *sensor;
} qmi_ts_data;

/* Help track individual sensors for setup, shutdown, and updates. */
typedef struct {
	const char  *thermald_name;
	const char  *ts_qmi_name;
	int          last_read;
	qmi_ts_data *data;
} qmi_ts_thermald_data;

/* Used to protect access of thermald_info */
static pthread_mutex_t qmi_ts_info_mtx = PTHREAD_MUTEX_INITIALIZER;

/* Add newly supported sensors here. */
static qmi_ts_thermald_data thermald_info[] = {
	{"pa_therm0", "pa", 0, NULL},
	{"pa_therm1", "pa_1", 0, NULL}
};

#define QMI_TS_MAX_STRING 16

void qmi_ts_update_thresholds(sensor_setting_t *setting,
			       int threshold_type, int level);

/*===========================================================================
LOCAL FUNCTION qmi_ts_get_thermald_info_idx

Helper function for finding thermald_info index of information based off of
thermald sensor name..

ARGUMENTS
	sensor - Thermald version of sensor name.

RETURN VALUE
	-1 on Failure, thermald_info array idx on Success.
===========================================================================*/
static int qmi_ts_get_thermald_info_idx(const char *sensor)
{
	int idx = -1;

	/* Find corresponding qmi_ts_thermald_data */
	for (idx = 0; idx < ARRAY_SIZE(thermald_info); idx++) {
		if (strncmp(thermald_info[idx].thermald_name, sensor,
			    QMI_TS_MAX_STRING) == 0) {
			break;
		}
	}

	if (idx == ARRAY_SIZE(thermald_info))
		idx = -1;

	return idx;
}

/*===========================================================================
FUNCTION qmi_ts_update_temperature

Updates the temperature for the thermald_info array.

ARGUMENTS
	sensor - QMI TS version of sensor name.

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_update_temperature(const char *sensor, int temperature)
{
	int idx = 0;

	if (NULL == sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	/* Find corresponding qmi_ts_data */
	for (idx = 0; idx < ARRAY_SIZE(thermald_info); idx++) {
		if (strncmp(thermald_info[idx].ts_qmi_name, sensor,
			    QMI_TS_MAX_STRING) == 0) {
			break;
		}
	}

	if (idx >= ARRAY_SIZE(thermald_info)) {
		msg("%s: unknown sensor %s\n", __func__, sensor);
		return;
	}

	thermald_info[idx].last_read = temperature;

	pthread_mutex_lock(&qmi_ts_info_mtx);
	if (thermald_info[idx].data != NULL) {
		qmi_ts_data *qmi_ts = thermald_info[idx].data;

		dbgmsg("Sensor update recieved :%s %d", sensor, temperature);
		/* notify the waiting thread */
		pthread_mutex_lock(&(qmi_ts->mutex));
		qmi_ts->threshold_reached = 1;
		pthread_cond_broadcast(&(qmi_ts->condition));
		pthread_mutex_unlock(&(qmi_ts->mutex));
	}
	pthread_mutex_unlock(&qmi_ts_info_mtx);
}

/*===========================================================================
FUNCTION qmi_ts_setup

QMI TS setup sensor.

ARGUMENTS
	setting - thermald sensor setting
	sensor - thermald sensor data

RETURN VALUE
	0 on Failure, 1 on Success
===========================================================================*/
int qmi_ts_setup(sensor_setting_t *setting, sensor_t *sensor)
{
	qmi_ts_data *qmi_ts = NULL;
	int idx;

	/* We have nothing to do if there are no thresholds */
	if (!setting->num_thresholds) {
		dbgmsg("No thresholds for sensor %s\n", sensor->name);
		return 0;
	}

	idx = qmi_ts_get_thermald_info_idx(sensor->name);
	if (idx < 0) {
		msg("%s: invalid sensor name %s", __func__, sensor->name);
		return 0;
	}

	/* Allocate QMI TS data */
	qmi_ts = (qmi_ts_data *) malloc(sizeof(qmi_ts_data));
	if (NULL == qmi_ts) {
		msg("%s: malloc failed", __func__);
		return 0;
	}
	memset(qmi_ts, 0, sizeof(qmi_ts_data));
	sensor->data = (void *) qmi_ts;

	pthread_mutex_init(&(qmi_ts->mutex), NULL);
	pthread_cond_init(&(qmi_ts->condition), NULL);
	qmi_ts->threshold_reached = 0;
	qmi_ts->init_thresh_set = 0;
	qmi_ts->sensor = sensor;
	setting->disabled = 0;
	thermald_info[idx].data = qmi_ts;

	return 1;
}

/*===========================================================================
FUNCTION qmi_ts_shutdown

QMI TS shutdown sensor.

ARGUMENTS
	setting - thermald sensor setting

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_shutdown(sensor_setting_t *setting)
{
	qmi_ts_data *qmi_ts;
	int idx;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->name ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	idx = qmi_ts_get_thermald_info_idx(setting->sensor->name);
	if (idx < 0) {
		msg("%s: invalid sensor name %s", __func__,
		    setting->sensor->name);
		return;
	}

	pthread_mutex_lock(&qmi_ts_info_mtx);
	/* Make sure an indication cannot be issued to a sensor being shutdown */
	qmi_ts = thermald_info[idx].data;
	thermald_info[idx].data = NULL;
	setting->sensor->data = NULL;
	pthread_mutex_unlock(&qmi_ts_info_mtx);

	pthread_mutex_destroy(&qmi_ts->mutex);
	pthread_cond_destroy(&qmi_ts->condition);

	free(qmi_ts);
}

/*===========================================================================
FUNCTION qmi_ts_get_temperature

QMI TS get sensor temperature.

ARGUMENTS
	setting - thermald sensor setting

RETURN VALUE
	Current temperature, CONV(-273) on failure
===========================================================================*/
int qmi_ts_get_temperature(sensor_setting_t *setting)
{
	int temp = -273;
	int idx;

	qmi_ts_data *qmi_ts;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->name ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return CONV(temp);
	}

	idx = qmi_ts_get_thermald_info_idx(setting->sensor->name);
	if (idx < 0) {
		msg("%s: invalid sensor name %s", __func__,
		    setting->sensor->name);
		return CONV(temp);
	}

	temp = thermald_info[idx].last_read;
	return CONV(temp);
}

/*===========================================================================
FUNCTION qmi_ts_interrupt_wait

QMI TS sensor wait for interrupt.

ARGUMENTS
	setting - thermald sensor setting

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_interrupt_wait(sensor_setting_t *setting)
{
	qmi_ts_data *qmi_ts;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	if (setting->sensor->interrupt_enable) {
		qmi_ts = (qmi_ts_data *) setting->sensor->data;

		if (qmi_ts->init_thresh_set == 0) {
			/* Set first threshold to trigger. */
			qmi_ts_update_thresholds(setting, THRESHOLD_NOCHANGE, 0);
			qmi_ts->init_thresh_set = 1;
		}

		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&(qmi_ts->mutex));
		while (!qmi_ts->threshold_reached) {
			pthread_cond_wait(&(qmi_ts->condition),
					&(qmi_ts->mutex));
		}
		qmi_ts->threshold_reached = 0;
		pthread_mutex_unlock(&(qmi_ts->mutex));
	}
}

/*===========================================================================
FUNCTION qmi_ts_update_thresholds

QMI TS sensor update interrupt thresholds.

ARGUMENTS
	setting - thermald sensor setting
	threshold_type - type of threshold change that triggered update
	level - current settings level

RETURN VALUE
	None
===========================================================================*/
void qmi_ts_update_thresholds(sensor_setting_t *setting,
			       int threshold_type, int level)
{
	int hi, lo;
	int hi_enable = 1;
	int lo_enable = 1;
	int idx;
	qmi_ts_data *qmi_ts;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data ||
	    NULL == setting->sensor->name) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	qmi_ts = (qmi_ts_data *) setting->sensor->data;

	idx = qmi_ts_get_thermald_info_idx(setting->sensor->name);
	if (idx < 0) {
		msg("%s: invalid sensor name %s", __func__, setting->sensor->name);
		return;
	}

	dbgmsg("%s: threshold_type %d, level %d", __func__,
	       threshold_type, level);
	hi = RCONV(setting->t[qmi_ts->hi_idx].lvl_trig);
	lo = RCONV(setting->t[qmi_ts->lo_idx].lvl_clr);
	if (level >= setting->num_thresholds) {
		/* handle corner high case */
		hi = RCONV(setting->t[setting->num_thresholds - 1].lvl_trig);
		qmi_ts->hi_idx = setting->num_thresholds - 1;
		hi_enable = 0;
	} else {
		hi = RCONV(setting->t[level].lvl_trig);
		qmi_ts->hi_idx = level;
	}
	if (level <= 0) {
		/* handle corner low case */
		lo = RCONV(setting->t[0].lvl_clr);
		qmi_ts->lo_idx = 0;
		lo_enable = 0;
	} else {
		lo = RCONV(setting->t[level - 1].lvl_clr);
		qmi_ts->lo_idx = level - 1;
	}

	modem_ts_temp_request(thermald_info[idx].ts_qmi_name, 0,
			      hi_enable, hi, lo_enable, lo);
}
