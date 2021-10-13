/*===========================================================================

  Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "tsens-sensor.h"
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
	"tsens_tz_sensor11",
	"tsens_tz_sensor12",
	"tsens_tz_sensor13",
	"tsens_tz_sensor14",
	"tsens_tz_sensor15",
};

static sensor_t g_sensors[] = {
	{
		.name = "tsens_tz_sensor0",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
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
		.enable_sensor = tsens_sensor_enable_sensor,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor11",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.enable_sensor = tsens_sensor_enable_sensor,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor12",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.enable_sensor = tsens_sensor_enable_sensor,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor13",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.enable_sensor = tsens_sensor_enable_sensor,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor14",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.enable_sensor = tsens_sensor_enable_sensor,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "tsens_tz_sensor15",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = tsens_sensor_interrupt_wait,
		.update_thresholds = tsens_sensor_update_thresholds,
		.enable_sensor = tsens_sensor_enable_sensor,
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
#ifdef IPQ_806x
	int act_cnt;
	int thrs_cnt;
	int valid_act_cnt;
	int max_thr;
#endif
	if (!settings)
		return sensor_count;

	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		for (j = 0; j < SENSOR_IDX_MAX; j++) {
#ifdef IPQ_806x
			/*
			Some threshold actions are set to "none", so it
			is therefore required to track only the valid
			actions to avoid setting up that particular sensor.
			*/
			max_thr = settings->sensors[j].num_thresholds;
			valid_act_cnt = 0;
			for (thrs_cnt = max_thr - 1; thrs_cnt >= 0; thrs_cnt--) {
				for( act_cnt = 0 ; act_cnt < settings->sensors[j].t[thrs_cnt].num_actions;act_cnt++ ){
					if( NONE != settings->sensors[j].t[thrs_cnt].actions[act_cnt].action )
						valid_act_cnt++;
				}
			}
#endif
			/*
			Do not setup sensors which are not being used.
			This is required to reduce memory foot print as
			each sensor requires about 1.5MB - 2 MB for setup.
			*/
			if (!settings->sensors[j].desc ||
				valid_act_cnt == 0 ||
				settings->sensors[j].num_thresholds < 1 ||
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
	dbgmsg("Sensor_wait step in.\n");
	if (setting == NULL ||
		setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}
	if (setting->sensor->interrupt_enable &&
	    setting->sensor->interrupt_wait) {
		setting->sensor->interrupt_wait(setting);
	} else if (!is_first_poll) {
		dbgmsg("Sleeping for smapling period of %d us.\n" , setting->sampling_period_us );
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

	dbgmsg("Interrupt Enable: %d \n" , setting->sensor->interrupt_enable );

	if (setting->sensor->interrupt_enable == 0 ||
	    setting->sensor->update_thresholds == NULL)
	{
		dbgmsg("%s: return. interrupt disabled or updatethreshold is NULL \n",  __func__ );
		return;
	}

	setting->sensor->update_thresholds(setting, threshold_type, level);
}

void sensor_enable( sensor_setting_t *setting , int tzn , int enabled )
{
	if ( setting && setting->sensor && setting->sensor->enable_sensor) {
                dbgmsg("%s: sensor setting found for tzn%d\n" , __func__ , tzn );
		setting->sensor->enable_sensor(tzn , enabled);
        }
	else{
		dbgmsg("%s: sensor setting not found for tzn%d. Using defaults...\n" , __func__ , tzn );
		tsens_sensor_enable_sensor(tzn , enabled);
	}
}
