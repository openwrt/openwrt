/*===========================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "thermal.h"

#define CONFIG_FILE_SIZE_MAX  5120


static char *field_names[] = {
	"debug",
	"sampling",
	"thresholds",
	"thresholds_clr",
	"actions",
	"action_info",
};

enum {
	DEBUG,
	SAMPLING,
	THRESHOLDS,
	THRESHOLDS_CLR,
	ACTIONS,
	ACTION_INFO,
	FIELD_IDX_MAX
} field_type;

static char *action_names[] = {
	"none",
	"cpu",
	"shutdown",
#ifdef IPQ_806x
        "powersave",
	"nss",
#else
	"report",
	"lcd",
	"modem",
	"fusion",
	"battery",
	"gpu",
	"wlan",
	"camera",
	"camcorder"
#endif
};

/* HK */
sensor_setting_t def_sensor_hk[] = {
	{
	.desc = "tsens_tz_sensor6",
	.id = 6,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 2, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 110, /* Level trigger temp */
			.lvl_clr = 100, /* Level clear temp */
			.num_actions = 1, /* No. of actions */
			.actions = {
				{
					.action = CPU_FREQ, /* Action type */
					.info = 1382400 /* Action info/value */
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 2,
	._n_to_clear = 2,
	._n_actions = 2,
	._n_action_info = 2
	},

	{
	.desc = "tsens_tz_sensor13",
	.id = 13,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 2, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 110, /* Level trigger temp */
			.lvl_clr = 100, /* Level clear temp */
			.num_actions = 1, /* No. of actions */
			.actions = {
				{
					.action = CPU_FREQ, /* Action type */
					.info = 1382400 /* Action info/value */
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 2,
	._n_to_clear = 2,
	._n_actions = 2,
	._n_action_info = 2
	},

	{
	.desc = "tsens_tz_sensor15",
	.id = 15,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 2, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 110, /* Level trigger temp */
			.lvl_clr = 100, /* Level clear temp */
			.num_actions = 1, /* No. of actions */
			.actions = {
				{
					.action = CPU_FREQ, /* Action type */
					.info = 1382400 /* Action info/value */
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 2,
	._n_to_clear = 2,
	._n_actions = 2,
	._n_action_info = 2
	}
};

def_sensor_setting_t def_hk_setting = {
	.sensor_count = 3,
	.sensors = def_sensor_hk
};

/* OAK & AC */
sensor_setting_t def_sensor_ac[] = {
	{
	.desc = "tsens_tz_sensor6",
	.id = 6,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 1, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 1,
	._n_to_clear = 1,
	._n_actions = 1,
	._n_action_info = 1
	},

	{
	.desc = "tsens_tz_sensor13",
	.id = 13,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 1, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 1,
	._n_to_clear = 1,
	._n_actions = 1,
	._n_action_info = 1
	},

	{
	.desc = "tsens_tz_sensor15",
	.id = 15,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 1, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 1,
	._n_to_clear = 1,
	._n_actions = 1,
	._n_action_info = 1
	}
};

def_sensor_setting_t def_ac_setting = {
	.sensor_count = 3,
	.sensors = def_sensor_ac
};

sensor_setting_t def_sensor_cp[] = {
	{
	.desc = "tsens_tz_sensor13",
	.id = 13,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 2, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 110, /* Level trigger temp */
			.lvl_clr = 100, /* Level clear temp */
			.num_actions = 1, /* No. of actions */
			.actions = {
				{
					.action = CPU_FREQ, /* Action type */
					.info = 864000 /* Action info/value */
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 2,
	._n_to_clear = 2,
	._n_actions = 2,
	._n_action_info = 2
	},
};

def_sensor_setting_t def_cp_setting = {
	.sensor_count = 1,
	.sensors = def_sensor_cp
};

/* IPQ806x */
sensor_setting_t def_ipq8064[] = {
	{
	.desc = "tsens_tz_sensor0",
	.id = 0,
	.disabled = 0, /* Sensor enabled */
	.sampling_period_us = 1000,
	.num_thresholds = 5, /* No. of threshold levels */

	.t = {
		{
			.lvl_trig = 105, /* Level trigger temp */
			.lvl_clr = 95, /* Level clear temp */
			.num_actions = 1, /* No. of actions */
			.actions = {
				{
					.action = CPU_FREQ, /* Action type */
					.info = 1200000 /* Action info/value */
				}
			}
		},

		{
			.lvl_trig = 110,
			.lvl_clr = 100,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 1000000
				}
			}
		},

		{
			.lvl_trig = 115,
			.lvl_clr = 105,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 800000
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 384000
				}
			}
		},

		{
			.lvl_trig = 125,
			.lvl_clr = 115,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	/*
	Internal variables initialized with threshold count
	*/
	._n_thresholds = 5,
	._n_to_clear = 5,
	._n_actions = 5,
	._n_action_info = 5
	}
};

def_sensor_setting_t def_ipq8064_setting = {
	.sensor_count = 1,
	.sensors = def_ipq8064
};

sensor_setting_t def_ipq8066[] = {
	{
	.desc = "tsens_tz_sensor0",
	.id = 0,
	.disabled = 0,
	.sampling_period_us = 1000,
	.num_thresholds = 3,

	.t = {
		{
			.lvl_trig = 115,
			.lvl_clr = 105,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 800000
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 384000
				}
			}
		},

		{
			.lvl_trig = 125,
			.lvl_clr = 115,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	._n_thresholds = 3,
	._n_to_clear = 3,
	._n_actions = 3,
	._n_action_info = 3
	}

};

def_sensor_setting_t def_ipq8066_setting = {
	.sensor_count = 1,
	.sensors = def_ipq8066
};

sensor_setting_t def_ipq8069[] = {
	{
	.desc = "tsens_tz_sensor0",
	.id = 0,
	.disabled = 0,
	.sampling_period_us = 1000,
	.num_thresholds = 3,

	.t = {
		{
			.lvl_trig = 115,
			.lvl_clr = 105,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 1400000
				}
			}
		},

		{
			.lvl_trig = 120,
			.lvl_clr = 110,
			.num_actions = 1,
			.actions = {
				{
					.action = CPU_FREQ,
					.info = 800000
				}
			}
		},

		{
			.lvl_trig = 125,
			.lvl_clr = 115,
			.num_actions = 1,
			.actions = {
				{
					.action = SHUTDOWN,
					.info = 1000
				}
			}
		}
	},

	._n_thresholds = 3,
	._n_to_clear = 3,
	._n_actions = 3,
	._n_action_info = 3
	}
};

def_sensor_setting_t def_ipq8069_setting = {
	.sensor_count = 1,
	.sensors = def_ipq8069
};

void init_settings(thermal_setting_t *settings)
{
	int i;

	if (!settings)
		return;

	settings->soc_id = therm_get_msm_id();
	dbgmsg("Identidied SOC ID: %d \n", settings->soc_id);

	switch (settings->soc_id) {
		case THERM_IPQ_8062:
		case THERM_IPQ_8064:
		case THERM_IPQ_8065:
		case THERM_IPQ_8066:
		case THERM_IPQ_8068:
		case THERM_IPQ_8069:
			settings->disable_unused_tsens = 1;
			break;
		default:
			settings->disable_unused_tsens = 0;
	}

	settings->sample_period_ms = -1;
	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		settings->sensors[i].sampling_period_us = -1;
		settings->sensors[i].num_thresholds     = 0;
		settings->sensors[i].desc               = NULL;
		settings->sensors[i].id                 = i;
		settings->sensors[i].disabled           = 1;
		settings->sensors[i].action_mask        = 0;
		settings->sensors[i]._n_thresholds      = 0;
		settings->sensors[i]._n_to_clear        = 0;
		settings->sensors[i]._n_actions         = 0;
		settings->sensors[i]._n_action_info     = 0;
		settings->sensors[i].last_lvl = 0;

		msg("init_settings: %d\nnum_thresholds: %d\n", i, settings->sensors[i].num_thresholds);
	}
}

void update_def_sensor_settings(thermal_setting_t *settings)
{
	def_sensor_setting_t *def_sensor_setting;
	int i,j, s_id, idx;

	if (!settings)
                return;

	msg("Using default sensor setting...\n");

	settings->sample_period_ms = 5000;

	switch (settings->soc_id) {
		case THERM_IPQ_8062:
		case THERM_IPQ_8066:
			msg("==== IPQ8062/8066 ===\n");
			def_sensor_setting = &def_ipq8066_setting;
			break;

		case THERM_IPQ_8064:
		case THERM_IPQ_8068:
			msg("==== IPQ8064/8068 ===\n");
			def_sensor_setting = &def_ipq8064_setting;
			break;

		case THERM_IPQ_8065:
		case THERM_IPQ_8069:
			msg("==== IPQ8065/8069 ===\n");
			def_sensor_setting = &def_ipq8069_setting;
			break;

		case THERM_IPQ_8070:
		case THERM_IPQ_8070A:
		case THERM_IPQ_8071:
		case THERM_IPQ_8071A:
		case THERM_IPQ_8172:
		case THERM_IPQ_8173:
		case THERM_IPQ_8174:
			msg("==== IPQ8070X / IPQ8071X / IPQ817X ===\n");
			def_sensor_setting = &def_ac_setting;
			break;

		case THERM_IPQ_8072:
		case THERM_IPQ_8072A:
		case THERM_IPQ_8074:
		case THERM_IPQ_8074A:
		case THERM_IPQ_8076:
		case THERM_IPQ_8076A:
		case THERM_IPQ_8078:
		case THERM_IPQ_8078A:
			msg("==== IPQ807X ===\n");
			def_sensor_setting = &def_hk_setting;
			break;
		case THERM_IPQ_6018:
		case THERM_IPQ_6028:
		case THERM_IPQ_6000:
		case THERM_IPQ_6010:
			msg("==== IPQ60xx ====\n");
			def_sensor_setting = &def_cp_setting;
			break;
		default:
			msg("==== DEFAULT ===\n");
			def_sensor_setting = &def_hk_setting;
	}


	for (idx = 0; idx < def_sensor_setting->sensor_count; idx++)
	{
		s_id = def_sensor_setting->sensors[idx].id;
		if (s_id >= SENSOR_IDX_MAX)
		{
			msg("==== Invalid sensor ID ===\n");
			continue;
		}

		settings->sensors[s_id].id = s_id;
		settings->sensors[s_id].desc = def_sensor_setting->sensors[idx].desc;
		settings->sensors[s_id].disabled = def_sensor_setting->sensors[idx].disabled;
		settings->sensors[s_id].sampling_period_us = def_sensor_setting->sensors[idx].sampling_period_us;
		settings->sensors[s_id].num_thresholds = def_sensor_setting->sensors[idx].num_thresholds;
		settings->sensors[s_id]._n_thresholds = def_sensor_setting->sensors[idx]._n_thresholds;
		settings->sensors[s_id]._n_to_clear = def_sensor_setting->sensors[idx]._n_to_clear;
		settings->sensors[s_id]._n_actions = def_sensor_setting->sensors[idx]._n_actions;
		settings->sensors[s_id]._n_action_info = def_sensor_setting->sensors[idx]._n_action_info;

		for (i = 0; i < def_sensor_setting->sensors[idx].num_thresholds; i++)
		{
			settings->sensors[s_id].t[i].lvl_trig = CONV(def_sensor_setting->sensors[idx].t[i].lvl_trig);
			settings->sensors[s_id].t[i].lvl_clr = CONV(def_sensor_setting->sensors[idx].t[i].lvl_clr);
			settings->sensors[s_id].t[i].num_actions = def_sensor_setting->sensors[idx].t[i].num_actions;

			for (j = 0; j < def_sensor_setting->sensors[idx].t[i].num_actions; j++)
			{
				settings->sensors[s_id].t[i].actions[j].action = def_sensor_setting->sensors[idx].t[i].actions[j].action;
				settings->sensors[s_id].t[i].actions[j].info = def_sensor_setting->sensors[idx].t[i].actions[j].info;
			}
		}
	}
}

void skip_space(char **ppos)
{
	char *pos = *ppos;

	while(*pos != '\0') {
		if ((*pos == ' ') || (*pos == '\t') || (*pos == '\n')) {
			pos++;
		}
		else
			break;
	}
	*ppos = pos;
}

void skip_line(char **ppos)
{
	char *pos = *ppos;
	char *ptmp;

	ptmp = index(pos, '\n');
	if (!ptmp) {
		*pos = '\0';
		return;
	}

	*ppos = ptmp + 1;
}

int parse_action(char *pvalue)
{
	int i;

	for (i = 0; i < ACTION_IDX_MAX; i++) {
		if (strcasecmp(action_names[i], pvalue) == 0)
			return i;
	}

	/* Unknown action; assuming none */
	msg("Unknown action '%s'\n", pvalue);
	return NONE;
}

int parse_action_values(char **ppos, double result[THRESHOLDS_MAX][ACTIONS_MAX], int *num_actions, int is_action)
{
	char *pos = *ppos;
	char *ptmp;
	char *pvalue, *psave = NULL;
	int  nresult = 0;

	ptmp = index(pos, '\n');
	if (ptmp)
		*ptmp = '\0';

	pvalue = strtok_r(pos, "\t \r\n", &psave);

	while (pvalue != NULL) {
		char *pstr = NULL;
		char *psave2 = NULL;
		int count = 0;

		pstr = strtok_r(pvalue , "+", &psave2);
		while (pstr != NULL) {
			dbgmsg("Found action '%s'\n", pstr);

			if (!is_action)
				result[nresult][count] = strtod(pstr, NULL);
			else
				result[nresult][count] = parse_action(pstr);
			count++;
			if (count >= ACTIONS_MAX)
				break;

			pstr = strtok_r(NULL, "+", &psave2);
		}

		num_actions[nresult++] = count;
		if (nresult >= THRESHOLDS_MAX)
			break;
		pvalue = strtok_r(NULL, "\t \r\n", &psave);
	}

	dbgmsg("No. of items found : %d\n", nresult);

	*ppos = ptmp + 1;
	return nresult;
}

int parse_values(char **ppos, double *result)
{
	char *pos = *ppos;
	char *ptmp;
	char *pvalue, *psave = NULL;
	int  nresult = 0;

	ptmp = index(pos, '\n');
	if (ptmp)
		*ptmp = '\0';

	pvalue = strtok_r(pos, "\t \r\n", &psave);

	while (pvalue != NULL) {
		result[nresult] = strtod(pvalue, NULL);
		nresult++;
		if (nresult >= THRESHOLDS_MAX)
			break;
		pvalue = strtok_r(NULL, "\t \r\n", &psave);
	}

	*ppos = ptmp + 1;
	return nresult;
}

int parse_config(thermal_setting_t *settings, int fd)
{
	char buf[CONFIG_FILE_SIZE_MAX];
	int sz, i;
	char *pos = buf;
	char *maxpos;
	int error_found = 0;
	char *idx;
	sensor_setting_t *in_section = NULL;
	int in_field = FIELD_IDX_MAX;

	if (fd == -1)
		return 0;

	dbgmsg("Loading configuration file...\n");

	sz = read(fd, buf, CONFIG_FILE_SIZE_MAX);
	if (sz < 0) {
		msg("Failed to read config file\n");
		return 0;
	}
	maxpos = pos + sz;
	dbgmsg("Config file read (%d bytes)\n", sz);

	buf[CONFIG_FILE_SIZE_MAX-1] = '\0';

	while (pos && (*pos != '\0') && (pos < maxpos)) {
		switch (*pos) {
		case '[':
			idx = index(++pos, ']');
			if (!idx) {
				error_found = 1;
				break;
			}
			in_field = FIELD_IDX_MAX;
			*idx = '\0';

			/* Init sensor index based on SOC ID */
			if ( settings->soc_id == THERM_IPQ_8070
				|| settings->soc_id == THERM_IPQ_8070A
				|| settings->soc_id == THERM_IPQ_8071
				|| settings->soc_id == THERM_IPQ_8071A
				|| settings->soc_id == THERM_IPQ_8072
				|| settings->soc_id == THERM_IPQ_8072A
				|| settings->soc_id == THERM_IPQ_8074
				|| settings->soc_id == THERM_IPQ_8074A
				|| settings->soc_id == THERM_IPQ_8076
				|| settings->soc_id == THERM_IPQ_8076A
				|| settings->soc_id == THERM_IPQ_8078
				|| settings->soc_id == THERM_IPQ_8078A
				|| settings->soc_id == THERM_IPQ_8172
				|| settings->soc_id == THERM_IPQ_8173
				|| settings->soc_id == THERM_IPQ_8174
				|| settings->soc_id == THERM_IPQ_6018
				|| settings->soc_id == THERM_IPQ_6028
				|| settings->soc_id == THERM_IPQ_6000
				|| settings->soc_id == THERM_IPQ_6010)
				i = TSENS_TZ_SENSOR4;
			else
				i = TSENS_TZ_SENSOR0;

			for (; i < SENSOR_IDX_MAX; i++) {
				if (strncasecmp(pos, sensor_names[i],
						SENSOR_NAME_MAX) != 0)
					continue;
				in_section = &settings->sensors[i];
				in_section->desc = sensor_names[i];
				pos = idx + 1;
				dbgmsg("Found section '%s'\n", in_section->desc);
				break;
			}
			if (i == SENSOR_IDX_MAX) {
				msg("Ignoring unknown section '%s'\n", pos);
				in_section = NULL;
				pos = index(++idx, '[');
				if (!pos)
					pos = &buf[CONFIG_FILE_SIZE_MAX-1];
			}
			break;

		case '\t':
		case '\r':
		case '\n':
		case ' ':
			pos++;
			break;

		default:
			if (in_field != FIELD_IDX_MAX) {
				int    j;
				int    num;
				double values[THRESHOLDS_MAX];
				int    num_actions[THRESHOLDS_MAX];
				double actions[THRESHOLDS_MAX][ACTIONS_MAX];

				switch (in_field) {
				case DEBUG:
					info("Debug output enabled from config");
					debug_output = 1;
					break;
				case SAMPLING:
					num = parse_values(&pos, values);
					if (num < 1)
						break;
					if (values[0] < SAMPLING_MS_MINIMUM) {
						values[0] = SAMPLING_MS_MINIMUM;
						info("Sampling time specified too low, using %d ms",
						    SAMPLING_MS_MINIMUM);
					}
					if (!in_section)
						settings->sample_period_ms = (int)values[0];
					else
						in_section->sampling_period_us = (int)values[0]*1000;
					break;
				case THRESHOLDS:
					if (!in_section)
						break;
					num = parse_values(&pos, values);
					if (num < 1)
						break;
					in_section->_n_thresholds = num;
					for (i = 0; i < num; i++) {
						if (strncmp(in_section->desc, "bcl", strlen("bcl")) != 0)
							in_section->t[i].lvl_trig = CONV(values[i]);
						else
							in_section->t[i].lvl_trig = values[i];
					}
					break;
				case THRESHOLDS_CLR:
					if (!in_section)
						break;
					num = parse_values(&pos, values);
					if (num < 1)
						break;
					in_section->_n_to_clear = num;
					for (i = 0; i < num; i++) {
						if (strncmp(in_section->desc, "bcl", strlen("bcl")) != 0)
							in_section->t[i].lvl_clr = CONV(values[i]);
						else
							in_section->t[i].lvl_clr = values[i];
					}
					break;
				case ACTIONS:
					if (!in_section)
						break;
					num = parse_action_values(&pos, actions, num_actions, 1);
					if (num < 1)
						break;
					in_section->_n_actions = num;
					for (i = 0; i < num; i++) {
						in_section->t[i].num_actions = num_actions[i];
						for (j = 0; j < num_actions[i]; j++) {
							in_section->t[i].actions[j].action = (int)actions[i][j];
							info("Threshold %d Action[%d] : %s\n", i, j, action_names[(int)actions[i][j]]);
						}
					}
					break;
				case ACTION_INFO:
					if (!in_section)
						break;
					num = parse_action_values(&pos, actions, num_actions, 0);
					if (num < 1)
						break;
					in_section->_n_action_info  = num;
					for (i = 0; i < num; i++) {
						for (j = 0; j < num_actions[i]; j++) {
							in_section->t[i].actions[j].info = (int)actions[i][j];
							info("Threshold %d Action Info[%d] : %lf\n", i, j, actions[i][j]);
						}
					}
					break;
				}
				in_field = FIELD_IDX_MAX;
				break;
			}

			idx = strpbrk(pos, "\t\r\n ");
			if (!idx) {
				msg("Error in file\n");
				error_found = 1;
				break;
			}
			*idx = '\0';
			for (i = 0; i < FIELD_IDX_MAX; i++) {
				if (strcasecmp(pos, field_names[i]) != 0)
					continue;
				pos = idx + 1;
				skip_space(&pos);
				dbgmsg("Found field '%s'\n", field_names[i]);
				in_field = i;
				break;
			}
			if (i == FIELD_IDX_MAX) {
				msg("Ignoring unknown field '%s'\n", pos);
				pos = idx + 1;
				skip_line(&pos);
			}
			break;
		}

		if (error_found) {
			return 0;
		}
	}
	return 1;
}

int threshold_array_compare(const void *x, const void *y)
{
	threshold_t *i = (threshold_t *)x;
	threshold_t *j = (threshold_t *)y;

	if (i->lvl_trig < j->lvl_trig)
		return -1;
	else if (i->lvl_trig > j->lvl_trig)
		return 1;
	return 0;
}

int bcl_threshold_array_compare(const void *x, const void *y)
{
	threshold_t *i = (threshold_t *)x;
	threshold_t *j = (threshold_t *)y;

	if (i->lvl_trig < j->lvl_trig)
		return 1;
	else if (i->lvl_trig > j->lvl_trig)
		return -1;
	return 0;
}

static void fallback_sampling_values(thermal_setting_t *settings,
				     sensor_setting_t * section)
{
	/* Fallback to default sampling times if sampling period
	   was not configured */
	if (settings == NULL ||
	    section == NULL) {
		msg("%s: unexpected NULL", __func__);
		return;
	}

	if (settings->sample_period_ms > 0 &&
	    section->sampling_period_us == -1) {
		section->sampling_period_us =
			settings->sample_period_ms * 1000;
		dbgmsg("Using configured default sampling period "
		       "%dms for sensor[%s]\n", settings->sample_period_ms,
		       section->desc);
	} else if (settings->sample_period_ms == -1 &&
		   section->sampling_period_us == -1) {
		section->sampling_period_us =
			SAMPLING_MS_DEFAULT * 1000;
		dbgmsg("Using default sampling period %dms "
		       "for sensor[%s]\n", SAMPLING_MS_DEFAULT,
		       section->desc);
	}
}

void validate_config(thermal_setting_t *settings)
{
	sensor_setting_t *s;
	int i, j, k;

	for (i = 0; i < SENSOR_IDX_MAX; i++) {
		s = &settings->sensors[i];

		/* Skip validation for unconfigured sensors */
		if (s->desc == NULL)
			continue;

		/* Fill out the number of thresholds based on min entry */
		s->num_thresholds = s->_n_thresholds;
		if (s->num_thresholds > s->_n_to_clear)
			s->num_thresholds = s->_n_to_clear;
		if (s->num_thresholds > s->_n_actions)
			s->num_thresholds = s->_n_actions;
		if (s->num_thresholds > s->_n_action_info)
			s->num_thresholds = s->_n_action_info;

		/* Ensure thresolds are in ascending order */
                /* For BCL, thresholds are in descending order */
		if (s->num_thresholds > 1) {
			if (strncmp(s->desc, "bcl", strlen("bcl")) != 0)
				qsort(s->t, s->num_thresholds, sizeof(threshold_t),
					threshold_array_compare);
			else
				qsort(s->t, s->num_thresholds, sizeof(threshold_t),
					bcl_threshold_array_compare);
		}

		/* Sort so as to move shutdown to last action */
		for (j = 0; j < s->num_thresholds; j++) {
			for (k = 0; k < s->t[j].num_actions; k++) {
				if (s->t[j].actions[k].action == SHUTDOWN &&
						k < s->t[j].num_actions - 1) {
					int value = 0;

					value = s->t[j].actions[k].info;
					s->t[j].actions[k].action = s->t[j].actions[k+1].action;
					s->t[j].actions[k].info = s->t[j].actions[k+1].info;
					s->t[j].actions[k+1].action= SHUTDOWN;
					s->t[j].actions[k+1].info = value;
				}
			}
		}

		/* Fallback on default sampling times, if not configured */
		fallback_sampling_values(settings, s);
		for (j = 0; j < s->num_thresholds; j++) {
			for (k = 0; k < s->t[j].num_actions; k++)
				dbgmsg(" %d %d %s %d\n",
				       s->t[j].lvl_trig,
				       s->t[j].lvl_clr,
				       action_names[s->t[j].actions[k].action],
				       s->t[j].actions[k].info);
		}
	}
}

int load_config(thermal_setting_t *settings, const char *pFName)
{
	int config_fd;

	/* reset to default before loading new configurationn */
	init_settings(settings);

	if (pFName) {
		config_fd = open(pFName, O_RDONLY);
		if (config_fd >= 0) {
			if (!parse_config(settings, config_fd)) {
				update_def_sensor_settings(settings);
			}
		}
		else {
			update_def_sensor_settings(settings);
		}
		close(config_fd);
	}
	else {
		update_def_sensor_settings(settings);
	}
	validate_config(settings);
	return 1;
}

