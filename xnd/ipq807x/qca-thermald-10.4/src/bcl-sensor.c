/*===========================================================================

  bcl-sensor.c

  DESCRIPTION
  BCL sensor access functions.

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

#include "bcl-sensor.h"
#include "thermal.h"

#define LVL_BUF_MAX (12)

#ifndef BCL_DUMP_FILE_DEFAULT
#define BCL_DUMP_FILE_DEFAULT "/data/bcl_dump.log"
#endif

/* BCL sysfs defines */
#define BCL_MODE "/sys/devices/platform/battery_current_limit/mode"
#define BCL_TYPE "/sys/devices/platform/battery_current_limit/type"
#define BCL_IBAT "/sys/devices/platform/battery_current_limit/ibat"
#define BCL_IMAX "/sys/devices/platform/battery_current_limit/imax"
#define BCL_VBAT "/sys/devices/platform/battery_current_limit/vbat"
#define BCL_RBAT "/sys/devices/platform/battery_current_limit/rbat"
#define BCL_OCV "/sys/devices/platform/battery_current_limit/ocv"
#define BCL_SOC "/sys/class/power_supply/battery/capacity"
#define BCL_POLL_INTERVAL "/sys/devices/platform/battery_current_limit/poll_interval"
#define BCL_IBAT_IMAX_LOW_THRESHOLD_MODE "/sys/devices/platform/battery_current_limit/ibat_imax_low_threshold_mode"
#define BCL_IBAT_IMAX_HIGH_THRESHOLD_MODE "/sys/devices/platform/battery_current_limit/ibat_imax_high_threshold_mode"
#define BCL_IBAT_IMAX_LOW_THRESHOLD_VALUE "/sys/devices/platform/battery_current_limit/ibat_imax_low_threshold_value"
#define BCL_IBAT_IMAX_HIGH_THRESHOLD_VALUE "/sys/devices/platform/battery_current_limit/ibat_imax_high_threshold_value"

typedef struct bcl_data {
	pthread_t bcl_thread;
	pthread_mutex_t bcl_mutex;
	pthread_cond_t bcl_condition;
	int threshold_reached;
	int hi_threshold, lo_threshold;
	int hi_idx, lo_idx;
	int sensor_shutdown;
	sensor_t *sensor;

	FILE *dump_bcl_fd;
	unsigned int ibat_idx;
	unsigned int imax_idx;
	unsigned int vbat_idx;
	unsigned int rbat_idx;
	unsigned int soc_idx;
	unsigned int ocv_idx;
	int ibat;
	int imax;

} bcl_data;

static void *bcl_uevent(void *data)
{
	int err = 0;
	sensor_t *sensor = (sensor_t *)data;
	struct pollfd fds;
	int fd;
	char uevent[MAX_PATH] = {0};
	char buf[MAX_PATH] = {0};
	bcl_data *bcl = NULL;

	if (NULL == sensor ||
	    NULL == sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return NULL;
	}
	bcl = (bcl_data *) sensor->data;

	/* Looking for bcl uevent */
	strlcpy(uevent, BCL_TYPE, MAX_PATH);

	fd = open(uevent, O_RDONLY);
	if (fd < 0) {
		msg("Unable to open %s to receive notifications.\n", uevent);
		return NULL;
	};

	while (!bcl->sensor_shutdown) {
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

		dbgmsg("BCL uevent :%s", buf);

		/* notify the waiting threads */
		pthread_mutex_lock(&(bcl->bcl_mutex));
		bcl->threshold_reached = 1;
		pthread_cond_broadcast(&(bcl->bcl_condition));
		pthread_mutex_unlock(&(bcl->bcl_mutex));
	}

        if (fd > -1)
               close(fd);

	return NULL;
}

static void enable_bcl(bcl_data *bcl, int enabled)
{
	int ret = 0;
	char name[MAX_PATH] = {0};

	if (NULL == bcl ||
	    NULL == bcl->sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	strlcpy(name, BCL_MODE, MAX_PATH);
	if (enabled)
		ret = write_to_file(name, "enabled", strlen("enabled"));
	else
		ret = write_to_file(name, "disabled", strlen("disabled"));

	if (ret <= 0) {
		msg("BCL failed to set mode %d\n", enabled);
	} else {
		dbgmsg("BCL mode set to %d\n", enabled);
	}

}

static void enable_bcl_threshold_high(bcl_data *bcl, int enabled)
{
	int ret = 0;
	char name[MAX_PATH] = {0};

	if (NULL == bcl ||
	    NULL == bcl->sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	strlcpy(name, BCL_IBAT_IMAX_HIGH_THRESHOLD_MODE, MAX_PATH);
	if (enabled)
		ret = write_to_file(name, "enabled", strlen("enabled"));
	else
		ret = write_to_file(name, "disabled", strlen("disabled"));

	if (ret <= 0) {
		msg("BCL high threshold failed to %d\n", enabled);
	} else {
		dbgmsg("BCL high threshold enabled: %d\n", enabled);
	}
}

static void enable_bcl_threshold_low(bcl_data *bcl, int enabled)
{
	int ret = 0;
	char name[MAX_PATH] = {0};

	if (NULL == bcl ||
	    NULL == bcl->sensor) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	strlcpy(name, BCL_IBAT_IMAX_LOW_THRESHOLD_MODE, MAX_PATH);
	if (enabled)
		ret = write_to_file(name, "enabled", strlen("enabled"));
	else
		ret = write_to_file(name, "disabled", strlen("disabled"));

	if (ret <= 0) {
		msg("BCL low threshold failed to %d\n", enabled);
	} else {
		dbgmsg("BCL low threshold enabled: %d\n", enabled);
	}
}


static void set_bcl_thresholds(bcl_data *bcl, int lvl, int lvl_clr,
			   int hi_enable, int lo_enable)
{
	char highname[MAX_PATH]= {0};
	char lowname[MAX_PATH]= {0};
	char buf[LVL_BUF_MAX] = {0};
	int ret = 0;
	int lowbcl = 0;

	if (NULL == bcl) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	strlcpy(highname, BCL_IBAT_IMAX_HIGH_THRESHOLD_VALUE, MAX_PATH);
	strlcpy(lowname, BCL_IBAT_IMAX_LOW_THRESHOLD_VALUE, MAX_PATH);

	/* Set thresholds in legal order */
	if (read_line_from_file(lowname, buf, sizeof(buf)) > 0) {
		lowbcl = atoi(buf);
	}

	if (lvl <= lowbcl) {
		/* set high threshold first */
		dbgmsg("Setting up BCL thresholds high: %d\n", lvl);
		enable_bcl_threshold_high(bcl, hi_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl);
		ret = write_to_file(highname, buf, strlen(buf));
		if (ret <= 0)
			msg("BCL threshold high failed to set %d\n", lvl);

		dbgmsg("Setting up BCL thresholds low: %d\n", lvl_clr);
		enable_bcl_threshold_low(bcl, lo_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl_clr);
		ret = write_to_file(lowname, buf, strlen(buf));
		if (ret <= 0)
			msg("BCL threshold low failed to set %d\n", lvl_clr);

	} else {
		dbgmsg("Setting up BCL thresholds low: %d\n", lvl_clr);
		enable_bcl_threshold_low(bcl, lo_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl_clr);
		ret = write_to_file(lowname, buf, strlen(buf));
		if (ret <= 0)
			msg("BCL threshold low failed to set %d\n", lvl_clr);

		dbgmsg("Setting up BCL thresholds high: %d\n", lvl);
		enable_bcl_threshold_high(bcl, hi_enable);
		snprintf(buf, LVL_BUF_MAX, "%d", lvl);
		ret = write_to_file(highname, buf, strlen(buf));
		if (ret <= 0)
			msg("BCL threshold high failed to set %d\n", lvl);
	}

	bcl->hi_threshold = lvl;
	bcl->lo_threshold = lvl_clr;
}

void bcl_enable_thresholds(sensor_setting_t *setting, int enabled)
{
	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("bcl: unexpected NULL");
		return;
	}

	enable_bcl_threshold_high(setting->sensor->data, enabled);
	enable_bcl_threshold_low(setting->sensor->data, enabled);
}

void bcl_enable(sensor_setting_t *setting, int enabled)
{
	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("bcl: unexpected NULL");
		return;
	}

	enable_bcl(setting->sensor->data, enabled);
}

/* NOTE: bcl_setup() function does not enable the sensor
 * or set thresholds. This should be done in the target-specific setup */
int bcl_setup(sensor_setting_t *setting, sensor_t *sensor)
{
	int fd = -1;
	int sensor_count = 0;
	char name[MAX_PATH] = {0};
	bcl_data *bcl = NULL;
	char buf[LVL_BUF_MAX] = {0};
	int ret = 0;
	const char *dump_bcl_file = (dump_bcl_ibat_imax_file) ? dump_bcl_ibat_imax_file : BCL_DUMP_FILE_DEFAULT;

	/* We have nothing to do if there are no thresholds */
	if (!setting->num_thresholds) {
		dbgmsg("No thresholds for sensor %s\n", sensor->name);
		return sensor_count;
	}

	/* Allocate BCL data */
	bcl = (bcl_data *) malloc(sizeof(bcl_data));
	if (NULL == bcl) {
		msg("%s: malloc failed", __func__);
		return sensor_count;
	}
	memset(bcl, 0, sizeof(bcl_data));
	sensor->data = (void *) bcl;

	if (dump_bcl_ibat_imax) {
		bcl->dump_bcl_fd = fopen(dump_bcl_file, "w");
		if (bcl->dump_bcl_fd == NULL) {
			msg("Failed to open BCL dump file %s\n", dump_bcl_file);
			free(bcl);
			return sensor_count;
		}
		fprintf(bcl->dump_bcl_fd, "ibat(mA) imax(mA) vbat(mV) ocv(uV) rbat(mOhms) soc\n");
		fflush(bcl->dump_bcl_fd);
	}

	sensor->tzn = 0;

	strlcpy(name, BCL_IBAT, MAX_PATH);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		if (bcl->dump_bcl_fd != NULL)
			fclose(bcl->dump_bcl_fd);
		return sensor_count;
	}
	bcl->ibat_idx = fd;

	strlcpy(name, BCL_IMAX, MAX_PATH);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		close(bcl->ibat_idx);
		if (bcl->dump_bcl_fd != NULL)
			fclose(bcl->dump_bcl_fd);
		return sensor_count;
	}
	bcl->imax_idx = fd;

	strlcpy(name, BCL_VBAT, MAX_PATH);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		close(bcl->ibat_idx);
		close(bcl->imax_idx);
		if (bcl->dump_bcl_fd != NULL)
			fclose(bcl->dump_bcl_fd);
		return sensor_count;
	}
	bcl->vbat_idx = fd;

	strlcpy(name, BCL_RBAT, MAX_PATH);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		close(bcl->ibat_idx);
		close(bcl->imax_idx);
		close(bcl->vbat_idx);
		if (bcl->dump_bcl_fd != NULL)
			fclose(bcl->dump_bcl_fd);
		return sensor_count;
	}
	bcl->rbat_idx = fd;

	strlcpy(name, BCL_SOC, MAX_PATH);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		close(bcl->ibat_idx);
		close(bcl->imax_idx);
		close(bcl->vbat_idx);
		close(bcl->rbat_idx);
		if (bcl->dump_bcl_fd != NULL)
			fclose(bcl->dump_bcl_fd);
		return sensor_count;
	}
	bcl->soc_idx = fd;

	strlcpy(name, BCL_OCV, MAX_PATH);
	fd = open(name, O_RDONLY);
	if (fd < 0) {
		msg("%s: Error opening %s\n", __func__, name);
		close(bcl->ibat_idx);
		close(bcl->imax_idx);
		close(bcl->vbat_idx);
		close(bcl->rbat_idx);
		close(bcl->soc_idx);
		if (bcl->dump_bcl_fd != NULL)
			fclose(bcl->dump_bcl_fd);
		return sensor_count;
	}
	bcl->ocv_idx = fd;

	sensor_count++;
	pthread_mutex_init(&(bcl->bcl_mutex), NULL);
	pthread_cond_init(&(bcl->bcl_condition), NULL);
	bcl->sensor_shutdown = 0;
	bcl->threshold_reached = 0;
	bcl->sensor = sensor;
	setting->disabled = 0;

	if (sensor->interrupt_enable) {
		pthread_create(&(bcl->bcl_thread), NULL,
			       bcl_uevent, sensor);
	}

	strlcpy(name, BCL_POLL_INTERVAL, MAX_PATH);
	snprintf(buf, LVL_BUF_MAX, "%d", setting->sampling_period_us/1000);
	ret = write_to_file(name, buf, strlen(buf));
	if (ret <= 0)
		msg("BCL poll interval failed to set %d\n", setting->sampling_period_us);

	return sensor_count;
}

void bcl_shutdown(sensor_setting_t *setting)
{
	bcl_data *bcl;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	bcl = (bcl_data *) setting->sensor->data;
	bcl->sensor_shutdown = 1;

	if (bcl->imax_idx > 0)
		close(bcl->imax_idx);

	if (bcl->ibat_idx > 0)
		close(bcl->ibat_idx);

	if (bcl->vbat_idx > 0)
		close(bcl->vbat_idx);

	if (bcl->rbat_idx > 0)
		close(bcl->rbat_idx);

	if (bcl->soc_idx > 0)
		close(bcl->soc_idx);

	if (bcl->ocv_idx > 0)
		close(bcl->ocv_idx);

	if (bcl->dump_bcl_fd)
		fclose(bcl->dump_bcl_fd);

	if (setting->sensor->interrupt_enable)
		pthread_join(bcl->bcl_thread, NULL);
	free(bcl);
}

int bcl_get_imax(sensor_setting_t *setting)
{
	char buf[64];
	int temp = 0;
	bcl_data *bcl;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	memset(buf, 0, sizeof(buf));

	bcl = (bcl_data *) setting->sensor->data;
	if (read(bcl->imax_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(bcl->imax_idx, 0, SEEK_SET);

	return (temp);
}

int bcl_get_vbat(sensor_setting_t *setting)
{
	char buf[64];
	int temp = 0;
	bcl_data *bcl;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	memset(buf, 0, sizeof(buf));

	bcl = (bcl_data *) setting->sensor->data;
	if (read(bcl->vbat_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(bcl->vbat_idx, 0, SEEK_SET);

	return (temp);
}

int bcl_get_rbat(sensor_setting_t *setting)
{
	char buf[64];
	int temp = 0;
	bcl_data *bcl;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	memset(buf, 0, sizeof(buf));

	bcl = (bcl_data *) setting->sensor->data;
	if (read(bcl->rbat_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(bcl->rbat_idx, 0, SEEK_SET);

	return (temp);
}

int bcl_get_soc(sensor_setting_t *setting)
{
	char buf[64];
	int temp = 0;
	bcl_data *bcl;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	memset(buf, 0, sizeof(buf));

	bcl = (bcl_data *) setting->sensor->data;
	if (read(bcl->soc_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(bcl->soc_idx, 0, SEEK_SET);

	return (temp);
}

int bcl_get_ocv(sensor_setting_t *setting)
{
	char buf[64];
	int temp = 0;
	bcl_data *bcl;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	memset(buf, 0, sizeof(buf));

	bcl = (bcl_data *) setting->sensor->data;
	if (read(bcl->ocv_idx, buf, sizeof(buf) -1) != -1)
		temp = atoi(buf);
	lseek(bcl->ocv_idx, 0, SEEK_SET);

	return (temp);
}

int bcl_get_ibat(sensor_setting_t *setting)
{
	char buf[64];
	int temp = 0;
	bcl_data *bcl;

	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	memset(buf, 0, sizeof(buf));

	bcl = (bcl_data *) setting->sensor->data;
	if (read(bcl->ibat_idx, buf, sizeof(buf) - 1) != -1)
		temp = atoi(buf);
	lseek(bcl->ibat_idx, 0, SEEK_SET);

	return (temp);
}

int bcl_get_diff_imax_ibat(sensor_setting_t *setting)
{
	if (NULL == setting) {
		msg("%s: unexpected NULL", __func__);
		return 0;
	}

	return (bcl_get_imax(setting) - bcl_get_ibat(setting));
}

void bcl_interrupt_wait(sensor_setting_t *setting)
{
	bcl_data *bcl;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	bcl = (bcl_data *) setting->sensor->data;

	if (dump_bcl_ibat_imax == 1) {
		usleep(setting->sampling_period_us);
		/* Read all BCL related params for dumping to a file */
		bcl->ibat = bcl_get_ibat(setting);
		bcl->imax = bcl_get_imax(setting);

		if (bcl->dump_bcl_fd) {
			fprintf(bcl->dump_bcl_fd, "%d %d %d %d %d %d\n", bcl->ibat, bcl->imax, bcl_get_vbat(setting),
					bcl_get_ocv(setting), bcl_get_rbat(setting), bcl_get_soc(setting));
			fflush(bcl->dump_bcl_fd);
		}
		return;
	}

	if (setting->sensor->interrupt_enable) {
		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&(bcl->bcl_mutex));
		while (!bcl->threshold_reached) {
			pthread_cond_wait(&(bcl->bcl_condition),
					&(bcl->bcl_mutex));
		}
		bcl->threshold_reached = 0;
		pthread_mutex_unlock(&(bcl->bcl_mutex));
		/* Read all BCL related params after event is fired */
		bcl->ibat = bcl_get_ibat(setting);
		bcl->imax = bcl_get_imax(setting);
	}
}

void bcl_update_thresholds(sensor_setting_t *setting,
				int threshold_type, int level)
{
	int hi, lo;
	int hi_enable = 1;
	int lo_enable = 1;
	bcl_data *bcl;

	if (NULL == setting ||
	    NULL == setting->sensor ||
	    NULL == setting->sensor->data) {
		msg("%s: unexpected NULL", __func__);
		return;
	}
	bcl = (bcl_data *) setting->sensor->data;

	dbgmsg("%s: threshold_type %d, level %d", __func__,
	       threshold_type, level);
	hi = setting->t[bcl->hi_idx].lvl_trig;
	lo = setting->t[bcl->lo_idx].lvl_clr;
	if (level >= setting->num_thresholds) {
		/* handle corner high case */
		hi = setting->t[setting->num_thresholds - 1].lvl_trig;
		bcl->hi_idx = setting->num_thresholds - 1;
		hi_enable = 0;
	} else {
		hi = setting->t[level].lvl_trig;
		bcl->hi_idx = level;
	}
	if (level <= 0) {
		/* handle corner low case */
		lo = setting->t[0].lvl_clr;
		bcl->lo_idx = 0;
		lo_enable = 0;
	} else {
		lo = setting->t[level - 1].lvl_clr;
		bcl->lo_idx = level - 1;
	}

	set_bcl_thresholds(bcl, hi, lo, hi_enable, lo_enable);
}
