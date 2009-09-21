#ifndef __TS_FILTER_H__
#define __TS_FILTER_H__

/*
 * Touchscreen filter.
 *
 * (c) 2008,2009 Andy Green <andy@openmoko.com>
 */

#include <linux/platform_device.h>

#define MAX_TS_FILTER_COORDS		3  /* X, Y and Z (pressure). */

struct ts_filter;
struct ts_filter_configuration;

/* Operations that a filter can perform. */

struct ts_filter_api {
	/* Create the filter - mandatory. */
	struct ts_filter * (*create)(
		struct platform_device *pdev,
		const struct ts_filter_configuration *config,
		int count_coords);
	/* Destroy the filter - mandatory. */
	void (*destroy)(struct ts_filter *filter);
	/* Clear the filter - optional. */
	void (*clear)(struct ts_filter *filter);


	/*
	 * The next three API functions only make sense if all of them are
	 * set for a filter. If a filter has the next three methods then
	 * it can propagate coordinates in the chain.
	 */

	/*
	 * Process the filter.
	 * It returns non-zero if the filter reaches an error.
	 */
	int (*process)(struct ts_filter *filter, int *coords);
	/*
	 * Is the filter ready to return a point?
	 * Please do not code side effects in this function.
	 */
	int (*haspoint)(struct ts_filter *filter);
	/*
	 * Get a point.
	 * Do not call unless the filter actually has a point to deliver.
	 */
	void (*getpoint)(struct ts_filter *filter, int *coords);

	/*
	 * Scale the points - optional.
	 * A filter could only scale coordinates.
	 */
	void (*scale)(struct ts_filter *filter, int *coords);
};

/*
 * Generic filter configuration. Actual configurations have this structure
 * as a member.
 */
struct ts_filter_configuration {
};

struct ts_filter {
	/* Operations for this filter. */
	const struct ts_filter_api *api;
	/* Number of coordinates to process. */
	int count_coords;
};

#endif
