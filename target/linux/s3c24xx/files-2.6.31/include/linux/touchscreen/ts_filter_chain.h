#ifndef __TS_FILTER_CHAIN_H__
#define __TS_FILTER_CHAIN_H__

/*
 * Touchscreen filter chains.
 *
 * (c) 2008,2009 Andy Green <andy@openmoko.com>
 */

#include "ts_filter.h"

#include <linux/err.h>

struct ts_filter_chain_configuration {
	/* API to use. */
	const struct ts_filter_api *api;
	/* Generic filter configuration. Different for each filter. */
	const struct ts_filter_configuration *config;
};

struct ts_filter_chain;

#ifdef CONFIG_TOUCHSCREEN_FILTER

/*
 * Create a filter chain. It will allocate an array of
 * null-terminated pointers to filters. On error it will return
 * an error you can check with IS_ERR.
 */
extern struct ts_filter_chain *ts_filter_chain_create(
	struct platform_device *pdev,
	const struct ts_filter_chain_configuration conf[],
	int count_coords);

/* Destroy the chain. */
extern void ts_filter_chain_destroy(struct ts_filter_chain *c);

/* Clear the filter chain. */
extern void ts_filter_chain_clear(struct ts_filter_chain *c);

/*
 * Try to get one point. Returns 0 if no points are available.
 * coords will be used as temporal space, thus you supply a point
 * using coords but you shouldn't rely on its value on return unless
 * it returns a nonzero value that is not -1.
 * If one of the filters find an error then this function will
 * return -1.
 */
int ts_filter_chain_feed(struct ts_filter_chain *c, int *coords);

#else /* !CONFIG_TOUCHSCREEN_FILTER */
#define ts_filter_chain_create(pdev, config, count_coords) (NULL)
#define ts_filter_chain_destroy(c) do { } while (0)
#define ts_filter_chain_clear(c) do { } while (0)
#define ts_filter_chain_feed(c, coords) (1)
#endif

#endif
