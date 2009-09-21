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
 * Copyright (c) 2008,2009
 *       Andy Green <andy@openmoko.com>
 *       Nelson Castillo <arhuaco@freaks-unidos.net>
 *
 * Simple mean filter.
 *
 */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include <linux/touchscreen/ts_filter_mean.h>

struct ts_filter_mean {
	/* Copy of the private filter configuration. */
	struct ts_filter_mean_configuration *config;
	/* Filter API. */
	struct ts_filter tsf;

	/* Index on a circular buffer. */
	int curr;
	/* Useful to tell if the circular buffer is full(read:ready). */
	int count;
	/* Sumation used to compute the mean. */
	int sum[MAX_TS_FILTER_COORDS];
	/* Keep point values and decrement them from the sum on time. */
	int *fifo[MAX_TS_FILTER_COORDS];
	/* Store the output of this filter. */
	int ready;
};

#define ts_filter_to_filter_mean(f) container_of(f, struct ts_filter_mean, tsf)


static void ts_filter_mean_clear(struct ts_filter *tsf);

static struct ts_filter *ts_filter_mean_create(
	struct platform_device *pdev,
	const struct ts_filter_configuration *conf,
	int count_coords)
{
	struct ts_filter_mean *priv;
	int *v;
	int n;

	priv = kzalloc(sizeof(struct ts_filter_mean), GFP_KERNEL);
	if (!priv)
		return NULL;

	priv->tsf.count_coords = count_coords;
	priv->config = container_of(conf,
				    struct ts_filter_mean_configuration,
				    config);

	BUG_ON(priv->config->length <= 0);

	v = kmalloc(priv->config->length * sizeof(int) * count_coords,
		    GFP_KERNEL);
	if (!v)
		return NULL;

	for (n = 0; n < count_coords; n++) {
		priv->fifo[n] = v;
		v += priv->config->length;
	}

	ts_filter_mean_clear(&priv->tsf);

	dev_info(&pdev->dev, "Created Mean filter len:%d coords:%d\n",
		 priv->config->length, count_coords);

	return &priv->tsf;
}

static void ts_filter_mean_destroy(struct ts_filter *tsf)
{
	struct ts_filter_mean *priv = ts_filter_to_filter_mean(tsf);

	kfree(priv->fifo[0]); /* first guy has pointer from kmalloc */
	kfree(tsf);
}

static void ts_filter_mean_clear(struct ts_filter *tsf)
{
	struct ts_filter_mean *priv = ts_filter_to_filter_mean(tsf);

	priv->count = 0;
	priv->curr = 0;
	priv->ready = 0;
	memset(priv->sum, 0, tsf->count_coords * sizeof(int));
}

static int ts_filter_mean_process(struct ts_filter *tsf, int *coords)
{
	struct ts_filter_mean *priv = ts_filter_to_filter_mean(tsf);
	int n;

	BUG_ON(priv->ready);

	for (n = 0; n < tsf->count_coords; n++) {
		priv->sum[n] += coords[n];
		priv->fifo[n][priv->curr] = coords[n];
	}

	if (priv->count + 1 == priv->config->length)
		priv->ready = 1;
	else
		priv->count++;

	priv->curr = (priv->curr + 1) % priv->config->length;

	return 0; /* No error. */
}

static int ts_filter_mean_haspoint(struct ts_filter *tsf)
{
	struct ts_filter_mean *priv = ts_filter_to_filter_mean(tsf);

	return priv->ready;
}

static void ts_filter_mean_getpoint(struct ts_filter *tsf, int *point)
{
	struct ts_filter_mean *priv = ts_filter_to_filter_mean(tsf);
	int n;

	BUG_ON(!priv->ready);

	for (n = 0; n < tsf->count_coords; n++) {
		point[n] = priv->sum[n];
		priv->sum[n] -= priv->fifo[n][priv->curr];
	}

	priv->ready = 0;
}

static void ts_filter_mean_scale(struct ts_filter *tsf, int *coords)
{
	int n;
	struct ts_filter_mean *priv = ts_filter_to_filter_mean(tsf);

	for (n = 0; n < tsf->count_coords; n++) {
		coords[n] += priv->config->length >> 1; /* Rounding. */
		coords[n] /= priv->config->length;
	}
}

const struct ts_filter_api ts_filter_mean_api = {
	.create =	ts_filter_mean_create,
	.destroy =	ts_filter_mean_destroy,
	.clear =	ts_filter_mean_clear,
	.process =	ts_filter_mean_process,
	.scale =	ts_filter_mean_scale,
	.haspoint =	ts_filter_mean_haspoint,
	.getpoint =	ts_filter_mean_getpoint,
};
EXPORT_SYMBOL_GPL(ts_filter_mean_api);

