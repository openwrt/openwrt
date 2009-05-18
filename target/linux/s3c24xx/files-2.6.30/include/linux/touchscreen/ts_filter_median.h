#ifndef __TS_FILTER_MEDIAN_H__
#define __TS_FILTER_MEDIAN_H__

#include "ts_filter.h"

/*
 * Touchscreen filter.
 *
 * median
 *
 * (c) 2008 Andy Green <andy@openmoko.com>
 */

struct ts_filter_median_configuration {
	/* Size of the filter. */
	int extent;
	/* Precomputed midpoint. */
	int midpoint;
	/* A reference value for us to check if we are going fast or slow. */
	int decimation_threshold;
	/* How many points to replace if we're going fast. */
	int decimation_above;
	/* How many points to replace if we're going slow. */
	int decimation_below;

	/* Generic configuration. */
	struct ts_filter_configuration config;
};

extern const struct ts_filter_api ts_filter_median_api;

#endif
