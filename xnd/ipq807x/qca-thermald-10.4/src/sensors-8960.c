/*===========================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "qmi-ts-sensor.h"
#include "tsens-sensor.h"
#include "bcl-sensor.h"
#include "adc-sensor.h"
#include "gen-sensor.h"
#include "pm8821-sensor.h"
#include "thermal.h"

#define NUM_TSENS_SENSORS (11)

#define HOTPLUG_HIGH_THRESHOLD_8960 (100)
#define HOTPLUG_LOW_THRESHOLD_8960  (HOTPLUG_HIGH_THRESHOLD_8960 - 20)
#define HOTPLUG_HIGH_THRESHOLD_8064 (115)
#define HOTPLUG_LOW_THRESHOLD_8064  (HOTPLUG_HIGH_THRESHOLD_8064 - 20)
#define HOTPLUG_HIGH_THRESHOLD_8930 (110)
#define HOTPLUG_LOW_THRESHOLD_8930  (HOTPLUG_HIGH_THRESHOLD_8930 - 20)
#define MPDECISION_SOCKET      "/dev/socket/mpdecision"

char *sensor_names[] =
{
	"pm8921_tz",
	"pa_therm0",
	"pa_therm1",
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
	"bcl",
	"pm8821_tz",
};

enum hybrid_state_t {
	HYBRID_STATE_MAIN_INTERRUPT,
	HYBRID_STATE_POLLING
};

enum sensor_type_8960_t {
	SENSOR_TYPE_NON_HYBRID,
	SENSOR_TYPE_HYBRID_MAIN,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_BCL,
};

/* variables controlling hybrid polling */
pthread_cond_t hybrid_condition;
pthread_mutex_t hybrid_mutex;
enum hybrid_state_t hybrid_state;
pthread_cond_t finish_poll_condition;
pthread_mutex_t poll_count_mutex;
int poll_count = 0;

static void hybrid_main_interrupt_wait(sensor_setting_t *setting);
static void hybrid_main_update_thresholds(sensor_setting_t *setting, int threshold_type, int level);
static void hybrid_aux_interrupt_wait(sensor_setting_t *setting);
static void hybrid_aux_update_thresholds(sensor_setting_t *setting, int threshold_type, int level);
static int  pa_sensor_setup(sensor_setting_t *settings, sensor_t *sensor);

static sensor_t g_sensors[] = {
	{
		.name = "pm8921_tz",
		.setup = gen_sensors_setup,
		.shutdown = gen_sensors_shutdown,
		.get_temperature = gen_sensor_get_temperature,
		.interrupt_wait = NULL,
		.update_thresholds = NULL,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "pa_therm0",
		.setup = pa_sensor_setup,
		.shutdown = NULL,
		.get_temperature = NULL,
		.interrupt_wait = NULL,
		.update_thresholds = NULL,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "pa_therm1",
		.setup = pa_sensor_setup,
		.shutdown = NULL,
		.get_temperature = NULL,
		.interrupt_wait = NULL,
		.update_thresholds = NULL,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor0",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_main_interrupt_wait,
		.update_thresholds = hybrid_main_update_thresholds,
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
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 1,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor2",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 2,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor3",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 3,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor4",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 4,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor5",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 5,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor6",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 6,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor7",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 7,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor8",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 8,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor9",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 9,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "tsens_tz_sensor10",
		.setup = tsens_sensors_setup,
		.shutdown = tsens_sensors_shutdown,
		.get_temperature = tsens_sensor_get_temperature,
		.interrupt_wait = hybrid_aux_interrupt_wait,
		.update_thresholds = hybrid_aux_update_thresholds,
		.setting = NULL,
		.tzn = 10,
		.data = NULL,
		.interrupt_enable = 0,
	},
	{
		.name = "bcl",
		.setup = bcl_setup,
		.shutdown = bcl_shutdown,
		.get_temperature = bcl_get_diff_imax_ibat,
		.interrupt_wait = bcl_interrupt_wait,
		.update_thresholds = bcl_update_thresholds,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
	{
		.name = "pm8821_tz",
		.setup = pm8821_setup,
		.shutdown = pm8821_shutdown,
		.get_temperature = pm8821_get_temperature,
		.interrupt_wait = pm8821_interrupt_wait,
		.update_thresholds = NULL,
		.setting = NULL,
		.tzn = 0,
		.data = NULL,
		.interrupt_enable = 1,
	},
};

/* NOTE: number of indexes in sensor_type should match g_sensors */
static enum sensor_type_8960_t sensor_type[] = {
	SENSOR_TYPE_NON_HYBRID,
	SENSOR_TYPE_NON_HYBRID,
	SENSOR_TYPE_NON_HYBRID,
	SENSOR_TYPE_HYBRID_MAIN,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_HYBRID_AUX,
	SENSOR_TYPE_BCL,
	SENSOR_TYPE_NON_HYBRID,
};

/* status of main sensor's threshold interrupt, toggle 0 and 1 */
static int main_sensor_hi_threshold_enabled;
/* are all sensor thresholds cleared? true/false */
static int threshold_cleared[NUM_TSENS_SENSORS] = {1,1,1,1,1,1,1,1,1,1,1};

static hotplug_map_t hotplug_map_8960[] = {
	{"tsens_tz_sensor2",  0x2, TSENS_TZ_SENSOR2},
	{NULL, 0x0, 0},
};

static hotplug_map_t hotplug_map_8930[] = {
	{"tsens_tz_sensor6",  0x2, TSENS_TZ_SENSOR6},
	{NULL, 0x0, 0},
};

static hotplug_map_t hotplug_map_8064[] = {
	{"tsens_tz_sensor8",  0x2, TSENS_TZ_SENSOR8},
	{"tsens_tz_sensor9",  0x4, TSENS_TZ_SENSOR9},
	{"tsens_tz_sensor10", 0x8, TSENS_TZ_SENSOR10},
	{"pm8821_tz",         0xC, PM8821_TZ},
	{NULL, 0x0, 0},
};
hotplug_map_t *hotplug_map;
static int hotplug_high_threshold;
static int hotplug_low_threshold;

/* returns gsensor idx matching the sensor name, or -1 if error */
static int get_gsensor_idx(char *name)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		if (0 == strncmp(name, g_sensors[i].name,
				 SENSOR_NAME_MAX))
			break;
	}
	return (i >= ARRAY_SIZE(g_sensors)) ? -1 : i;
}

static int all_hybrid_thresholds_cleared(void)
{
	int ret = 0;
	int i;

	for (i = 0; i < NUM_TSENS_SENSORS; i++) {
		if (threshold_cleared[i] == 0)
			break;
	}
	if (i == NUM_TSENS_SENSORS) {
		dbgmsg("All hybrid_thresholds_cleared");
		ret = 1;
	}

	return ret;
}

static int hybrid_sensor_id(sensor_setting_t *setting)
{
	int sensor_id = 0;

	if (NULL == setting ||
	    NULL == setting->desc) {
		msg("%s: Unexpected NULL", __func__);
		return -1;
	}
	if (!sscanf(setting->desc, "tsens_tz_sensor%d", &sensor_id)) {
		return -1;
	} else {
		return sensor_id;
	}
}

/* Manually disable sensors for unconfigured sensors */
static void disable_sensor_manual(char *sensor_name)
{
	int tzn = 0;
	char name[MAX_PATH] = {0};
	tzn = get_tzn(sensor_name);

	if (tzn != -1) {
		snprintf(name, MAX_PATH, TZ_MODE, tzn);
		write_to_file(name, "disabled", strlen("disabled"));
		dbgmsg("Sensor '%s' not configured - set disabled\n",
		       sensor_name);
	}
}

static int sensor_is_configured(sensor_t *sensor)
{
	return (sensor->setting != NULL);
}

static void hybrid_enable_auxiliary_sensors(int enabled)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		if (sensor_type[i] != SENSOR_TYPE_HYBRID_AUX)
			continue;

		if (sensor_is_configured(&g_sensors[i]) &&
		    g_sensors[i].setting->disabled == 0) {
			tsens_sensor_enable_sensor(g_sensors[i].setting,
						   enabled);
		} else if (enabled == 0 &&
			   !sensor_is_configured(&g_sensors[i])) {
			/* disable sensor manually if unconfigured */
			disable_sensor_manual(g_sensors[i].name);
		}

	}
}

/* request hotplug manually via sysfs.
   Fallback function for hotplugging CPUs
   This function should only be called in one thread per hotplug core */
static int hotplug_request_manual(int cpu, int online)
{
	char node_buf[MAX_PATH] = {0};
	int i = 0;
	int ret = 0;
	static int prev[MAX_CPUS] = {1, 1, 1, 1};

	if (online != prev[cpu]) {
		/* Write to offline node if present */
		snprintf(node_buf, MAX_PATH, CPU_SYSFS(HOTPLUG_NODE), cpu);
		if (write_to_file(node_buf, online ? "1": "0", 2) > 0) {
			info("ACTION: Hotplugged %s CPU[%d]\n",
			    online ? "ON" : "OFF", cpu);
			prev[cpu] = online;
		} else {
			msg("Failed to hotplug CPU[%d] at path %s\n",
			    cpu, node_buf);
			ret = -1;
		}
	} else {
		dbgmsg("Hotplug request already at %d\n", online);
	}
	return ret;
}

/* Inform mpdecision to do hotplug, else hotplug cores manually */
void  hotplug_request_critical(sensor_setting_t *setting, int temp, int cpu,
			       int requester, int set_offline)
{
	static int  hotplug_off_req[MAX_CPUS][SENSOR_IDX_MAX];
	static int done_hotplug[MAX_CPUS];
	char tempBuf[REPORT_MSG_MAX];
	static pthread_mutex_t hotplug_cpu_mtx = PTHREAD_MUTEX_INITIALIZER;
	int rc = 0;
	int i;

	if (setting == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	pthread_mutex_lock(&hotplug_cpu_mtx);

	/* Aggregate vote for hotplug request for all sensors */
	/* While online core ensure all sensors threshold is cleared for that cpu */
	hotplug_off_req[cpu][requester] = set_offline;
	if (set_offline == 0) {
		for (i = 0; hotplug_map[i].sensor_name != NULL; i++) {
			if (hotplug_off_req[cpu][hotplug_map[i].sensor_idx] != 0) {
				dbgmsg("Waiting for other sensors to clear hotplug "
				       "threshold for cpu[%d]", cpu);
				goto handle_return;
			}
		}
	}

	if (set_offline != done_hotplug[cpu]) {
		info("ACTION: HOTPLUG CPU[%d] critical failsafe %s - "
		    "Sensor %s - temperature %d", cpu, set_offline ? "triggered" : "cleared",
		    setting->desc, temp);
		snprintf(tempBuf, REPORT_MSG_MAX, "hotplug %d %d", cpu, !set_offline);

		if (write_to_local_file_socket(MPDECISION_SOCKET, tempBuf,
			      strlen(tempBuf)) != (int) strlen(tempBuf)) {
			/* fallback to manual hotplug update */
			rc = hotplug_request_manual(cpu, !set_offline);
		}
		if (rc == 0)
			done_hotplug[cpu] = set_offline;
	}

handle_return:
	pthread_mutex_unlock(&hotplug_cpu_mtx);

}

void do_hotplug_critical(sensor_setting_t *setting, int cpu,
			 int temperature)
{
	int cpu_mask;

	if (setting == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	for (cpu_mask = 0; cpu_mask < MAX_CPUS; cpu_mask++) {
		if ((cpu & (1 << cpu_mask)) == 0)
			continue;

		if (temperature >= setting->hotplug_lvl_trig)
			hotplug_request_critical(setting, temperature,
						 cpu_mask, setting->id, 1);
		else if (temperature <= setting->hotplug_lvl_clr)
			hotplug_request_critical(setting, temperature,
						 cpu_mask, setting->id, 0);
	}

}

static void setup_hotplug_map(thermal_setting_t *settings)
{
	int j = 0;
	int msm_id = therm_get_msm_id();

	switch (msm_id) {
	case THERM_MSM_8064AB:
	case THERM_MSM_8064:
		hotplug_map = hotplug_map_8064;
		hotplug_high_threshold = HOTPLUG_HIGH_THRESHOLD_8064;
		hotplug_low_threshold = HOTPLUG_LOW_THRESHOLD_8064;
		break;
	case THERM_MSM_8930AB:
	case THERM_MSM_8930AA:
	case THERM_MSM_8930:
		hotplug_map = hotplug_map_8930;
		hotplug_high_threshold = HOTPLUG_HIGH_THRESHOLD_8930;
		hotplug_low_threshold = HOTPLUG_LOW_THRESHOLD_8930;
		break;
	case THERM_MSM_8960AB:
	case THERM_MSM_8960:
	default:
		/* default to 8960 hotplug map */
		hotplug_map = hotplug_map_8960;
		hotplug_high_threshold = HOTPLUG_HIGH_THRESHOLD_8960;
		hotplug_low_threshold = HOTPLUG_LOW_THRESHOLD_8960;
	}
        /* Assign hotplug thresholds to hotplug_lvl_trg/clr of each sensor setting */
	while (hotplug_map[j].sensor_name != NULL) {
		if (hotplug_map[j].sensor_idx >= SENSOR_IDX_MAX) {
			j++;
			continue;
		}
		settings->sensors[hotplug_map[j].sensor_idx].hotplug_lvl_trig =
			hotplug_high_threshold;
		settings->sensors[hotplug_map[j].sensor_idx].hotplug_lvl_clr =
			hotplug_low_threshold;
		j++;
	}
}

int sensors_setup(thermal_setting_t *settings)
{
	int i = 0;
	int j = 0;
	int save_main_i = -1;
	int sensor_count = 0;
	int aux_sensor_count = 0;
	int save_bcl_i = -1;

	if (!settings)
		return sensor_count;

	pthread_mutex_init(&hybrid_mutex, NULL);
	pthread_cond_init(&hybrid_condition, NULL);
	pthread_cond_init(&finish_poll_condition, NULL);
	pthread_mutex_init(&poll_count_mutex, NULL);
	hybrid_state = HYBRID_STATE_MAIN_INTERRUPT;

	setup_hotplug_map(settings);

	/* override hotplug limit if requested on cmdline */
	if (new_corelimit) {
		hotplug_high_threshold = new_corelimit;
		hotplug_low_threshold = new_corelimit - 20;
	}

	/* Set up sensors */
	for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
		for (j = 0; j < SENSOR_IDX_MAX; j++) {
			if (settings->sensors[j].desc == NULL ||
			    0 != strcmp(settings->sensors[j].desc, g_sensors[i].name))
				continue;
			info("Sensor setup:[%s]\n", g_sensors[i].name);
			g_sensors[i].setting = &settings->sensors[j];
			settings->sensors[j].sensor = &g_sensors[i];

			if (g_sensors[i].setup(&settings->sensors[j],
					       (sensor_t *)&g_sensors[i])) {
				sensor_count++;
				if (sensor_type[i] == SENSOR_TYPE_HYBRID_MAIN) {
					save_main_i = i;
				} else if (sensor_type[i] == SENSOR_TYPE_HYBRID_AUX) {
					aux_sensor_count++;
				} else if (sensor_type[i] == SENSOR_TYPE_BCL) {
					save_bcl_i = i;
				}
			}
			break;
		}
	}
	/* set up hotplug sensors if not configured */
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		int g_idx;
		if (settings->sensors[i].disabled == 0)
			continue;

		j = 0;
		while (hotplug_map[j].sensor_name != NULL) {
			if (i != hotplug_map[j].sensor_idx) {
				j++;
				continue;
			}

			settings->sensors[i].desc = sensor_names[i];
			settings->sensors[i].num_thresholds = 1;
			settings->sensors[i].disabled = 0;
			settings->sensors[i].sampling_period_us = 1000000; /* 1 sec */
			settings->sensors[i].t[0].lvl_trig = CONV(hotplug_high_threshold);
			settings->sensors[i].t[0].lvl_clr = CONV(hotplug_low_threshold);
			settings->sensors[i].t[0].actions[0].action = NONE;
			settings->sensors[i].t[0].actions[0].info = 0;
			g_idx = get_gsensor_idx(settings->sensors[i].desc);
			if (g_idx == -1) {
				msg("Invalid sensor name %s", settings->sensors[i].desc);
				break;
			}

			g_sensors[g_idx].setting = &settings->sensors[i];
			settings->sensors[i].sensor = &g_sensors[g_idx];
			if (g_sensors[g_idx].setup(&settings->sensors[i],
						   (sensor_t *)&g_sensors[g_idx])) {
				sensor_count++;
				if (sensor_type[g_idx] == SENSOR_TYPE_HYBRID_MAIN) {
					save_main_i = g_idx;
				} else if (sensor_type[g_idx] == SENSOR_TYPE_HYBRID_AUX) {
					aux_sensor_count++;
				}
			}

			info("Sensor '%s' enabled for CPU%d "
			    "core-control failsafe", sensor_names[i],
			    hotplug_map[j].cpu);
			break;
		}
	}
	poll_count = aux_sensor_count;

	/* Enable main sensors, disable auxiliary sensors, setup threshold */
	if (save_main_i > -1 && save_main_i < ARRAY_SIZE(g_sensors)) {
		tsens_sensor_enable_sensor(g_sensors[save_main_i].setting, 1);
		hybrid_enable_auxiliary_sensors(0);

		tsens_sensor_update_thresholds(g_sensors[save_main_i].setting,
					       THRESHOLD_NOCHANGE, 0);
		/* mark threshold_enabled */
		main_sensor_hi_threshold_enabled = 1;
	} else {
		info("Hybrid main sensor not configured, disabling any auxiliary sensors found");
		for (i = 0; i < ARRAY_SIZE(g_sensors); i++) {
			if (!sensor_is_configured(&g_sensors[i]) ||
			    g_sensors[i].setting->disabled != 0 ||
			    sensor_type[i] != SENSOR_TYPE_HYBRID_AUX)
				continue;

			info("Disabling auxiliary sensor:[%s]\n", g_sensors[i].name);
			g_sensors[i].setting->disabled = 1;

			j = 0;
			while (hotplug_map[j].sensor_name != NULL) {
				if (g_sensors[i].setting->id == hotplug_map[j].sensor_idx)
					info("Core-control failsafe auxiliary sensor '%s' disabled, "
					    "failsafe for CPU%d will NOT work", g_sensors[i].name,
					    hotplug_map[j].cpu);
				j++;
			}
			sensor_count--;
		}
		poll_count = 0;
	}

	/* TODO : if hotplug sensor is enabled, ensure that it will still be
	   polled at the HOTPLUG_LOW_THRESHOLD, ie. existing thresholds are not
	   t[0].lvl_clr > LOW_THRESHOLD || t[0].lvl_trig > HIGH_THRESHOLD
	   corner case where polling will stop and our hotplug failsafe won't
	   get executed since we're in interrupt mode */

	// Enable and Setup Threshold for BCL
	if (save_bcl_i > -1 && save_bcl_i < ARRAY_SIZE(g_sensors)) {
		bcl_enable(g_sensors[save_bcl_i].setting, 1);
		bcl_update_thresholds(g_sensors[save_bcl_i].setting, 0, 0);
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
	if (sensor->id == BCL) {
		if (value <= sensor->t[level].lvl_trig)
			return 1;
		else
			return 0;
	} else {
		if (value >= sensor->t[level].lvl_trig)
			return 1;
		else
			return 0;
	}
}

int sensor_threshold_clear(int value, sensor_setting_t *sensor, int level)
{
	if (sensor->id == BCL) {
		if (value >= sensor->t[level].lvl_clr)
			return 1;
		else
			return 0;
	} else {
		if (value <= sensor->t[level].lvl_clr)
			return 1;
		else
			return 0;
	}
}


int sensor_get_temperature(sensor_setting_t *setting)
{
	int temp = 0;
	int i = 0;

	if (setting == NULL ||
	    setting->sensor == NULL ||
	    setting->sensor->get_temperature == NULL) {
		return -EFAULT;
	}

	temp = setting->sensor->get_temperature(setting);
	dbgmsg("Sensor[%s] Temperature : %2.1f\n", setting->desc, RCONV(temp));

	i = 0;
	while (hotplug_map[i].sensor_name != NULL) {
		if (setting->id == hotplug_map[i].sensor_idx) {
			do_hotplug_critical(setting, hotplug_map[i].cpu,
					    (int) RCONV(temp));
		}
		i++;
	}

	return temp;
}

static void hybrid_main_interrupt_wait(sensor_setting_t *setting)
{
        int performed_wait = 0;

	if (setting == NULL ||
	    setting->desc == NULL ||
	    setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	/* If main sensor in interrupt mode */
	if (hybrid_state == HYBRID_STATE_MAIN_INTERRUPT) {
		if (main_sensor_hi_threshold_enabled == 0) {
			/* wait for auxiliary sensors to finish polling */
			pthread_mutex_lock(&poll_count_mutex);
			while (poll_count > 0) {
				pthread_cond_wait(&finish_poll_condition,
						  &poll_count_mutex);
			}
			pthread_mutex_unlock(&poll_count_mutex);

			/* disable auxiliary sensors first, then
			   reenable main sensor hi threshold */
			hybrid_enable_auxiliary_sensors(0);

			tsens_sensor_enable_thresholds(setting, 1, 0);
			main_sensor_hi_threshold_enabled = 1;
		}
		tsens_sensor_interrupt_wait(setting);
		performed_wait = 1;
	}

	/* Don't sleep on first poll */
	if (!performed_wait) {
		usleep(setting->sampling_period_us);
	}
}

static void hybrid_aux_interrupt_wait(sensor_setting_t *setting)
{
        int performed_wait = 0;

	if (setting == NULL ||
	    setting->desc == NULL ||
	    setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	/* Auxiliary sensors should block on main sensor interrupt */
	if (hybrid_state != HYBRID_STATE_POLLING) {

		/* check that all auxiliary sensors are ready to wait before
		   completing polling */
		pthread_mutex_lock(&poll_count_mutex);
		poll_count--;
		if (poll_count == 0) {
			pthread_cond_broadcast(&finish_poll_condition);
		}
		pthread_mutex_unlock(&poll_count_mutex);

		/* Wait for sensor threshold condition */
		pthread_mutex_lock(&hybrid_mutex);
		while (HYBRID_STATE_POLLING != hybrid_state) {
			pthread_cond_wait(&hybrid_condition,
					  &hybrid_mutex);
		}
		pthread_mutex_unlock(&hybrid_mutex);

		pthread_mutex_lock(&poll_count_mutex);
		poll_count++;
		pthread_mutex_unlock(&poll_count_mutex);

		performed_wait = 1;
	}

	/* Don't sleep on first poll */
	if (!performed_wait) {
		usleep(setting->sampling_period_us);
	}
}

void sensor_wait(sensor_setting_t *setting)
{
	static int is_first_poll = 1;

	if (setting == NULL ||
	    setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	if (setting->sensor->interrupt_wait) {
		setting->sensor->interrupt_wait(setting);
	} else if (!is_first_poll) {
		usleep(setting->sampling_period_us);
	} else {
		is_first_poll = 0;
	}
}

static void hybrid_main_update_thresholds(sensor_setting_t *setting,
					  int threshold_type, int level)
{

	if (setting == NULL ||
	    setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	/* TSENS thresholds don't need updating for 8960
	   sensor_setup sets threshold 0 */
	/* Update state for hybrid polling */
	if (threshold_type == THRESHOLD_CROSS) {
		threshold_cleared[0] = 0;
	} else if (threshold_type == THRESHOLD_CLEAR && level == 0) {
		threshold_cleared[0] = 1;
	}

	if (hybrid_state == HYBRID_STATE_MAIN_INTERRUPT &&
	    threshold_type == THRESHOLD_NOCHANGE) {
		/* Rearm threshold if we didn't actually cross hi threshold */
		tsens_sensor_enable_thresholds(setting, 1, 0);
	} else if (hybrid_state == HYBRID_STATE_MAIN_INTERRUPT &&
		   threshold_type == THRESHOLD_CROSS) {
		/* Enable auxiliary hybrid polling on threshold crossing */
		dbgmsg("Hybrid state -> polling enabled");
		/* transition to polling expected,
		   disable interrupts, enable aux sensors */
		tsens_sensor_enable_thresholds(setting, 0, 0);
		main_sensor_hi_threshold_enabled = 0;
		hybrid_enable_auxiliary_sensors(1);

		pthread_mutex_lock(&hybrid_mutex);
		hybrid_state = HYBRID_STATE_POLLING;
		pthread_cond_broadcast(&hybrid_condition);
		pthread_mutex_unlock(&hybrid_mutex);
	} else if (hybrid_state == HYBRID_STATE_POLLING &&
		   all_hybrid_thresholds_cleared()) {
		/* disable polling on threshold clearing */
		pthread_mutex_lock(&hybrid_mutex);
		dbgmsg("Hybrid state -> interrupt");
		hybrid_state = HYBRID_STATE_MAIN_INTERRUPT;
		pthread_mutex_unlock(&hybrid_mutex);
	}
}

static void hybrid_aux_update_thresholds(sensor_setting_t *setting,
			      int threshold_type, int level)
{
	int hybrid_id = 0;

	if (setting == NULL ||
	    setting->sensor == NULL) {
		msg("%s: Unexpected NULL", __func__);
		return;
	}

	hybrid_id = hybrid_sensor_id(setting);
	if (hybrid_id <= 0 || hybrid_id >= NUM_TSENS_SENSORS) {
		msg("%s: Bad hybrid sensor_id", __func__);
		return;
	}

	/* Update state for hybrid polling */
	if (threshold_type == THRESHOLD_CROSS) {
		threshold_cleared[hybrid_id] = 0;
	} else if (threshold_type == THRESHOLD_CLEAR && level == 0) {
		threshold_cleared[hybrid_id] = 1;
	}

	/* disable polling on threshold clearing */
	if (hybrid_state == HYBRID_STATE_POLLING &&
	    all_hybrid_thresholds_cleared()) {
		pthread_mutex_lock(&hybrid_mutex);
		dbgmsg("Hybrid state -> interrupt");
		hybrid_state = HYBRID_STATE_MAIN_INTERRUPT;
		pthread_mutex_unlock(&hybrid_mutex);
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

	if (setting->sensor->update_thresholds == NULL)
		return;

	setting->sensor->update_thresholds(setting, threshold_type, level);
}

static int  pa_sensor_setup(sensor_setting_t *settings,
				   sensor_t *sensor)
{
	int fd = -1;
	int sensor_count = 0;
	do {
		/* We have nothing to do if there are no thresholds */
		if (!settings->num_thresholds) {
			dbgmsg("No thresholds for sensor %s\n", sensor->name);
			break;
		}

		sensor_count = adc_sensors_setup(settings, sensor);
		if (sensor_count) {
			info("ADC sensor found for %s\n", sensor->name);
			sensor->shutdown        = adc_sensors_shutdown;
			sensor->get_temperature = adc_sensor_get_temperature;
			break;
		}
		sensor_count = qmi_ts_setup(settings, sensor);
		if (sensor_count) {
			info("QMI TS sensor found for %s\n", sensor->name);
			sensor->update_thresholds = qmi_ts_update_thresholds;
			sensor->get_temperature   = qmi_ts_get_temperature;
			sensor->interrupt_wait    = qmi_ts_interrupt_wait;
			sensor->shutdown          = qmi_ts_shutdown;
			sensor->interrupt_enable  = 1;
			break;
		}
	} while (0);
	return sensor_count;
}

