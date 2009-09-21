#ifndef __TS_FILTER_LINEAR_H__
#define __TS_FILTER_LINEAR_H__

#include "ts_filter.h"
#include <linux/kobject.h>

/*
 * Touchscreen linear filter.
 *
 * Copyright (C) 2008,2009 by Openmoko, Inc.
 * Author: Nelson Castillo <arhuaco@freaks-unidos.net>
 *
 */

#define TS_FILTER_LINEAR_NCONSTANTS 7

struct ts_filter_linear_configuration {
	/* Calibration constants. */
	int constants[TS_FILTER_LINEAR_NCONSTANTS];
	/* First coordinate. */
	int coord0;
	/* Second coordinate. */
	int coord1;

	/* Generic filter configuration. */
	struct ts_filter_configuration config;
};

extern const struct ts_filter_api ts_filter_linear_api;

#endif
