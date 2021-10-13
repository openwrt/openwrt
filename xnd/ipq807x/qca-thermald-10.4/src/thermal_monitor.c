/*===========================================================================

Copyright (c) 2010-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "thermal.h"

static void clear_all_alarms(sensor_setting_t *sensor, int sensor_temp)
{
	int i, cpu;
#ifndef IPQ_806x
	int gpu;
#endif

	for (i = 0; i < ACTION_IDX_MAX; i++) {
		/* check if action may have been set */
		if ((sensor->action_mask & (1 << i)) == 0)
			continue;

		switch (i) {
		case CPU_FREQ:
			/* set CPU speed to highest */
			for (cpu = 0; cpu < num_cpus; cpu++) {
				cpufreq_request(cpu, sensor->id,
						sensor_temp, -1);
			}
			break;

#ifndef IPQ_806x
		case GPU_FREQ:
			/* set GPU speed to highest */
			for (gpu = 0; gpu < num_gpus; gpu++) {
				gpufreq_request(gpu, sensor->id,
						sensor_temp, -1);
			}
			break;
		case LCD:
			lcd_brightness_request(sensor->id, sensor_temp, -1);
			break;
		case MODEM:
			modem_request(sensor->id, sensor_temp, 0);
			break;
		case FUSION:
			fusion_modem_request(sensor->id, sensor_temp, 0);
			break;
		case BATTERY:
			battery_request(sensor->id, sensor_temp, 0);
			break;
		case WLAN:
			wlan_request(sensor->id, sensor_temp, 0);
			break;
		case CAMERA:
			camera_request(sensor->id, sensor_temp, 0);
			break;
		case CAMCORDER:
			camcorder_request(sensor->id, sensor_temp, 0);
			break;
#endif

#ifdef IPQ_806x
		case POWERSAVE:
			powersave_request(0);
			break;
		case NSS_FREQ:
                        nssfreq_request(-1);
                        break;
#endif
		}
	}
	sensor->action_mask = 0;
}

void *sensor_monitor(void *vsensor)
{
	sensor_setting_t *sensor = (sensor_setting_t *)vsensor;
	int i, j, cpu;
#ifndef IPQ_806x
	int gpu;
#endif
	int max_thr;
	int lvl_alarm[THRESHOLDS_MAX];
	int alarm_raised = 0, alarm_cleared = 0;
	int sensor_temp = 0;
	int lvl_max, lvl_min;
	int threshold_type = THRESHOLD_NOCHANGE;
	int threshold_level = 0;

	if (!sensor || (sensor->num_thresholds < 1))
		return NULL;

	max_thr = sensor->num_thresholds;
	for (i = 0; i < max_thr; i++) {
		lvl_alarm[i] = 0;
	}

	while (exit_daemon != 1) {
		sensor_wait(sensor);

		sensor_temp = sensor_get_temperature(sensor);

		lvl_max = -1;
		lvl_min = INT_MAX;
		for (i = max_thr - 1; i >= 0; i--) {
			/* Scan for new alarm conditions */
			if (sensor_threshold_trigger(sensor_temp, sensor, i)) {
				if (lvl_alarm[i] == 0) {
					info("Sensor '%s' - alarm raised %d at %2.1f degC\n",
					       sensor->desc, i + 1, RCONV(sensor_temp));
					lvl_alarm[i] = 1;
					alarm_raised = 1;
				}
				if (i > lvl_max)
					lvl_max = i;
			}
			/* Scan for alarm clearing conditions */
			if (sensor_threshold_clear(sensor_temp, sensor, i)) {
				if (lvl_alarm[i] == 1) {
					info("Sensor '%s' - alarm cleared %d at %2.1f degC\n",
					       sensor->desc, i + 1, RCONV(sensor_temp));
					lvl_alarm[i] = 0;
					alarm_cleared = 1;
				}
				if (i < lvl_min)
					lvl_min = i;
			}
		}

		/* Update temperature thresholds */
		if (alarm_raised) {
			threshold_type = THRESHOLD_CROSS;
			threshold_level = lvl_max + 1;
		} else if (alarm_cleared) {
			threshold_type = THRESHOLD_CLEAR;
			threshold_level = lvl_min;
		} else {
			threshold_type = THRESHOLD_NOCHANGE;
			threshold_level = sensor->last_lvl;
		}
		sensor->last_lvl = threshold_level;

		sensor_update_thresholds(sensor, threshold_type, threshold_level);

		if (!alarm_raised && !alarm_cleared) {
			continue;
		}

		/* Perform actions on highest level alarm */
		for (i = max_thr - 1; i >= 0; i--) {
			if (lvl_alarm[i] == 0)
				continue;

			for (j = 0; j < sensor->t[i].num_actions; j++) {
				dbgmsg ("j=%d i=%d Sensor %s: action %d value %d\n",
					j, i, sensor->desc,
					sensor->t[i].actions[j].action,
					sensor->t[i].actions[j].info);

				sensor->action_mask |= (1 << sensor->t[i].actions[j].action);
				switch(sensor->t[i].actions[j].action) {
				case NONE:
					break;
				case CPU_FREQ:
					for (cpu = 0; cpu < num_cpus; cpu++) {
						cpufreq_request(cpu, sensor->id,
								sensor_temp,
								sensor->t[i].actions[j].info);
					}
					break;
				case SHUTDOWN:
                                        shutdown_action(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
                                        break;

#ifdef IPQ_806x
                                case POWERSAVE:
                                        powersave_request(sensor->t[i].actions[j].info);
                                        break;
				case NSS_FREQ:
                                        nssfreq_request(sensor->t[i].actions[j].info);
                                        break;
#else
				case REPORT:
                                        if (alarm_raised)
                                                report_action(sensor->id, sensor_temp, i, 1);
                                        break;

				case GPU_FREQ:
					for (gpu = 0; gpu < num_gpus; gpu++) {
						gpufreq_request(gpu, sensor->id,
								sensor_temp,
								sensor->t[i].actions[j].info);
					}
					break;
				case LCD:
					lcd_brightness_request(sensor->id,
							       sensor_temp,
							       sensor->t[i].actions[j].info);
					break;
				case MODEM:
					modem_request(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
					break;
				case FUSION:
					fusion_modem_request(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
					break;
				case BATTERY:
					battery_request(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
					break;
				case WLAN:
					wlan_request(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
					break;
				case CAMERA:
					camera_request(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
					break;
				case CAMCORDER:
					camcorder_request(sensor->id, sensor_temp, sensor->t[i].actions[j].info);
					break;
#endif

				default:
					msg("Unknown action %d\n", sensor->t[i].actions[j].action);
				}
			}

			break;
		}

		if (alarm_cleared) {
			/* Handle alarm clearing cases */
			if (lvl_min == 0) {
				dbgmsg("Clearing all alarms on sensor %s\n", sensor->desc);
				clear_all_alarms(sensor, sensor_temp);
			}
#ifndef IPQ_806x
			/* report threshold clearing */
			for (j = 0; j < sensor->t[lvl_min].num_actions; j++) {
				if (REPORT == sensor->t[lvl_min].actions[j].action) {
					report_action(sensor->id, sensor_temp, lvl_min, 0);
					break;
				}
			}
#endif
		}

		alarm_raised = 0;
		alarm_cleared = 0;
	}

	return NULL;
}

void thermal_monitor(thermal_setting_t *settings)
{
	int i;
	int act_cnt;
	int thrs_cnt;
	int valid_act_cnt;
	int max_thr;

	if (!sensors_setup(settings)) {
		msg("Failed to setup at least one sensor for monitoring\n");
		return;
	}

	if (settings->disable_unused_tsens) {
		for (i = 0; i < SENSOR_IDX_MAX; i++){
			dbgmsg("Disabling sensor %d\n", i);
			sensor_enable(&settings->sensors[i], i, 0 );
		}
	}


	for (i = 0; i < SENSOR_IDX_MAX; i++){
		max_thr = settings->sensors[i].num_thresholds;
		valid_act_cnt = 0;
		for (thrs_cnt = max_thr - 1; thrs_cnt >= 0; thrs_cnt--) {
			for( act_cnt = 0 ; act_cnt < settings->sensors[i].t[thrs_cnt].num_actions;act_cnt++ ){
				if( NONE != settings->sensors[i].t[thrs_cnt].actions[act_cnt].action )
					valid_act_cnt++;
			}
		}
		dbgmsg("Sensor %d: No.of valid actions: %d\n" , i , valid_act_cnt );

		if (valid_act_cnt) {
			dbgmsg("%s - Enabling sensor: %d tzn: %d\n" , __func__ , i, settings->sensors[i].sensor->tzn);
                        sensor_enable(&settings->sensors[i], settings->sensors[i].sensor->tzn, 1 );
                } else
			continue;

		dbgmsg("Spawn Monitor thread for sensor: %d\n" , i );

		if (pthread_create(&settings->sensors[i].monitor,
				NULL,
				(void *)&sensor_monitor,
				(void *)&settings->sensors[i]) != 0) {
			msg("Error initializing monitor for sensor '%s'\n",
				sensor_names[i]);
			settings->sensors[i].disabled = 1;
		}
	}

	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		if (!settings->sensors[i].disabled &&
			(settings->sensors[i].num_thresholds > 0)) {
			pthread_join(settings->sensors[i].monitor, NULL);
		}
	}

	sensors_shutdown();
}

