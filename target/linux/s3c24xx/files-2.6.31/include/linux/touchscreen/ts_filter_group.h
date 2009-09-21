#ifndef __TS_FILTER_GROUP_H__
#define __TS_FILTER_GROUP_H__

#include "ts_filter.h"

/*
 * Touchscreen group filter.
 *
 * Copyright (C) 2008,2009 by Openmoko, Inc.
 * Author: Nelson Castillo <arhuaco@freaks-unidos.net>
 *
 */

struct ts_filter_group_configuration {
	/* Size of the filter. */
	int length;
	/*
	 * If two points are separated by this distance or less they
	 * are considered to be members of the same group.
	 */
	int close_enough;
	/* Minimum allowed size for the biggest group in the sample set. */
	int threshold;
	/*
	 * Number of times we try to get a group of points with at least
	 * threshold points.
	 */
	int attempts;

	/* Generic filter configuration. */
	struct ts_filter_configuration config;
};

extern const struct ts_filter_api ts_filter_group_api;

#endif
