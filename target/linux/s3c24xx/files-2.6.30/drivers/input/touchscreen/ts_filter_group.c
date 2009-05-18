/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Copyright (C) 2008,2009 by Openmoko, Inc.
 * Author: Nelson Castillo <arhuaco@freaks-unidos.net>
 * All rights reserved.
 *
 *
 * This filter is useful to reject samples that are not reliable. We consider
 * that a sample is not reliable if it deviates form the Majority.
 *
 * 1) We collect S samples.
 *
 * 2) For each dimension:
 *
 *  - We sort the points.
 *  - Points that are "close enough" are considered to be in the same set.
 *  - We choose the set with more elements. If more than "threshold"
 *    points are in this set we use the first and the last point of the set
 *    to define the valid range for this dimension [min, max], otherwise we
 *    discard all the points and go to step 1.
 *
 * 3) We consider the unsorted S samples and try to feed them to the next
 *    filter in the chain. If one of the points of each sample
 *    is not in the allowed range for its dimension, we discard the sample.
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/touchscreen/ts_filter_group.h>

struct ts_filter_group {
	/* Private filter configuration. */
	struct ts_filter_group_configuration *config;
	/* Filter API. */
	struct ts_filter tsf;

	int N;			/* How many samples we have. */
	int *samples[MAX_TS_FILTER_COORDS];	/* The samples: our input. */

	int *group_size;	/* Used for temporal computations. */
	int *sorted_samples;	/* Used for temporal computations. */

	int range_max[MAX_TS_FILTER_COORDS];	/* Max. computed ranges. */
	int range_min[MAX_TS_FILTER_COORDS];	/* Min. computed ranges. */

	int tries_left;		/* We finish if we don't get enough samples. */
	int ready;		/* If we are ready to deliver samples. */
	int result;		/* Index of the point being returned. */
};

#define ts_filter_to_filter_group(f) \
	container_of(f, struct ts_filter_group, tsf)


static void ts_filter_group_clear_internal(struct ts_filter_group *tsfg,
					   int attempts)
{
	tsfg->N = 0;
	tsfg->tries_left = attempts;
	tsfg->ready = 0;
	tsfg->result = 0;
}

static void ts_filter_group_clear(struct ts_filter *tsf)
{
	struct ts_filter_group *tsfg = ts_filter_to_filter_group(tsf);

	ts_filter_group_clear_internal(tsfg, tsfg->config->attempts);
}

static struct ts_filter *ts_filter_group_create(
	struct platform_device *pdev,
	const struct ts_filter_configuration *conf,
	int count_coords)
{
	struct ts_filter_group *tsfg;
	int i;

	tsfg = kzalloc(sizeof(struct ts_filter_group), GFP_KERNEL);
	if (!tsfg)
		return NULL;

	tsfg->config = container_of(conf,
				    struct ts_filter_group_configuration,
				    config);
	tsfg->tsf.count_coords = count_coords;

	BUG_ON(tsfg->config->attempts <= 0);

	tsfg->samples[0] = kmalloc((2 + count_coords) * sizeof(int) *
				   tsfg->config->length, GFP_KERNEL);
	if (!tsfg->samples[0]) {
		kfree(tsfg);
		return NULL;
	}
	for (i = 1; i < count_coords; ++i)
		tsfg->samples[i] = tsfg->samples[0] + i * tsfg->config->length;
	tsfg->sorted_samples = tsfg->samples[0] + count_coords *
			       tsfg->config->length;
	tsfg->group_size = tsfg->samples[0] + (1 + count_coords) *
			       tsfg->config->length;

	ts_filter_group_clear_internal(tsfg, tsfg->config->attempts);

	dev_info(&pdev->dev, "Created Group filter len:%d coords:%d close:%d "
		 "thresh:%d\n", tsfg->config->length, count_coords,
		 tsfg->config->close_enough, tsfg->config->threshold);

	return &tsfg->tsf;
}

static void ts_filter_group_destroy(struct ts_filter *tsf)
{
	struct ts_filter_group *tsfg = ts_filter_to_filter_group(tsf);

	kfree(tsfg->samples[0]); /* first guy has pointer from kmalloc */
	kfree(tsf);
}

static int int_cmp(const void *_a, const void *_b)
{
	const int *a = _a;
	const int *b = _b;

	if (*a > *b)
		return 1;
	if (*a < *b)
		return -1;
	return 0;
}

static void ts_filter_group_prepare_next(struct ts_filter *tsf);

static int ts_filter_group_process(struct ts_filter *tsf, int *coords)
{
	struct ts_filter_group *tsfg = ts_filter_to_filter_group(tsf);
	int n;
	int i;

	BUG_ON(tsfg->N >= tsfg->config->length);
	BUG_ON(tsfg->ready);

	for (n = 0; n < tsf->count_coords; n++)
		tsfg->samples[n][tsfg->N] = coords[n];

	if (++tsfg->N < tsfg->config->length)
		return 0;	/* We need more samples. */

	for (n = 0; n < tsfg->tsf.count_coords; n++) {
		int *v = tsfg->sorted_samples;
		int ngroups = 0;
		int best_size;
		int best_idx = 0;
		int idx = 0;

		memcpy(v, tsfg->samples[n], tsfg->N * sizeof(int));
		/*
		 * FIXME: Remove this sort call. We already have the
		 * algorithm for this modification. The filter will
		 * need less points (about half) if there is not a
		 * lot of noise. Right now we are doing a constant
		 * amount of work no matter how much noise we are
		 * dealing with.
		 */
		sort(v, tsfg->N, sizeof(int), int_cmp, NULL);

		tsfg->group_size[0] = 1;
		for (i = 1; i < tsfg->N; ++i) {
			if (v[i] - v[i - 1] <= tsfg->config->close_enough)
				tsfg->group_size[ngroups]++;
			else
				tsfg->group_size[++ngroups] = 1;
		}
		ngroups++;

		best_size = tsfg->group_size[0];
		for (i = 1; i < ngroups; i++) {
			idx += tsfg->group_size[i - 1];
			if (best_size < tsfg->group_size[i]) {
				best_size = tsfg->group_size[i];
				best_idx = idx;
			}
		}

		if (best_size < tsfg->config->threshold) {
			/* This set is not good enough for us. */
			if (--tsfg->tries_left) {
				ts_filter_group_clear_internal
					(tsfg, tsfg->tries_left);
				/* No errors but we need more samples. */
				return 0;
			}
			return 1; /* We give up: error. */
		}

		tsfg->range_min[n] = v[best_idx];
		tsfg->range_max[n] = v[best_idx + best_size - 1];
	}

	ts_filter_group_prepare_next(tsf);

	return 0;
}

/*
 * This private function prepares a point that will be returned
 * in ts_filter_group_getpoint if it is available. It updates
 * the priv->ready state also.
 */
static void ts_filter_group_prepare_next(struct ts_filter *tsf)
{
	struct ts_filter_group *priv = ts_filter_to_filter_group(tsf);
	int n;

	while (priv->result < priv->N) {
		for (n = 0; n < priv->tsf.count_coords; ++n) {
			if (priv->samples[n][priv->result] <
			    priv->range_min[n] ||
			    priv->samples[n][priv->result] > priv->range_max[n])
				break;
		}

		if (n == priv->tsf.count_coords) /* Sample is OK. */
			break;

		priv->result++;
	}

	if (unlikely(priv->result >= priv->N)) { /* No sample to deliver. */
		ts_filter_group_clear_internal(priv, priv->config->attempts);
		priv->ready = 0;
	} else {
		priv->ready = 1;
	}
}

static int ts_filter_group_haspoint(struct ts_filter *tsf)
{
	struct ts_filter_group *priv = ts_filter_to_filter_group(tsf);

	return priv->ready;
}

static void ts_filter_group_getpoint(struct ts_filter *tsf, int *point)
{
	struct ts_filter_group *priv = ts_filter_to_filter_group(tsf);
	int n;

	BUG_ON(!priv->ready);

	for (n = 0; n < priv->tsf.count_coords; n++)
		point[n] = priv->samples[n][priv->result];

	priv->result++;

	/* This call will update priv->ready. */
	ts_filter_group_prepare_next(tsf);
}

/*
 * Get ready to process the next batch of points, forget
 * points we could have delivered.
 */
static void ts_filter_group_scale(struct ts_filter *tsf, int *coords)
{
	struct ts_filter_group *priv = ts_filter_to_filter_group(tsf);

	ts_filter_group_clear_internal(priv, priv->config->attempts);
}

const struct ts_filter_api ts_filter_group_api = {
	.create =	ts_filter_group_create,
	.destroy =	ts_filter_group_destroy,
	.clear =	ts_filter_group_clear,
	.process =	ts_filter_group_process,
	.haspoint =	ts_filter_group_haspoint,
	.getpoint =	ts_filter_group_getpoint,
	.scale =	ts_filter_group_scale,
};
EXPORT_SYMBOL_GPL(ts_filter_group_api);

