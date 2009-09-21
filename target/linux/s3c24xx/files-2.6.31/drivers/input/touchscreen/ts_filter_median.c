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
 * Copyright (c) 2008 Andy Green <andy@openmoko.com>
 *
 *
 * Median averaging stuff.  We sort incoming raw samples into an array of
 * MEDIAN_SIZE length, discarding the oldest sample each time once we are full.
 * We then return the sum of the middle three samples for X and Y.  It means
 * the final result must be divided by (3 * scaling factor) to correct for
 * avoiding the repeated /3.
 *
 * This strongly rejects brief excursions away from a central point that is
 * sticky in time compared to the excursion duration.
 *
 * Thanks to Dale Schumacher (who wrote some example code) and Carl-Daniel
 * Halifinger who pointed out this would be a good method.
 */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/touchscreen/ts_filter_median.h>

struct ts_filter_median {
	/* Private configuration. */
	struct ts_filter_median_configuration *config;
	/* Generic Filter API. */
	struct ts_filter tsf;

	/* Count raw samples we get. */
	int samples_count;
	/*
	 * Remember the last coordinates we got in order to know if
	 * we are moving slow or fast.
	 */
	int last_issued[MAX_TS_FILTER_COORDS];
	/* How many samples in the sort buffer are valid. */
	int valid;
	/* Samples taken for median in sorted form. */
	int *sort[MAX_TS_FILTER_COORDS];
	/* Samples taken for median. */
	int *fifo[MAX_TS_FILTER_COORDS];
	/* Where we are in the fifo sample memory. */
	int pos;
	/* Do we have a sample to deliver? */
	int ready;
};

#define ts_filter_to_filter_median(f) \
	container_of(f, struct ts_filter_median, tsf)


static void ts_filter_median_insert(int *p, int sample, int count)
{
	int n;

	/* Search through what we got so far to find where to put sample. */
	for (n = 0; n < count; n++)
		if (sample < p[n]) {	/* We met somebody bigger than us? */
			/* Starting from the end, push bigger guys down one. */
			for (count--; count >= n; count--)
				p[count + 1] = p[count];
			p[n] = sample; /* Put us in place of first bigger. */
			return;
		}

	p[count] = sample; /* Nobody was bigger than us, add us on the end. */
}

static void ts_filter_median_del(int *p, int value, int count)
{
	int index;

	for (index = 0; index < count; index++)
		if (p[index] == value) {
			for (; index < count; index++)
				p[index] = p[index + 1];
			return;
		}
}


static void ts_filter_median_clear(struct ts_filter *tsf)
{
	struct ts_filter_median *tsfm = ts_filter_to_filter_median(tsf);

	tsfm->pos = 0;
	tsfm->valid = 0;
	tsfm->ready = 0;
	memset(&tsfm->last_issued[0], 1, tsf->count_coords * sizeof(int));
}

static struct ts_filter *ts_filter_median_create(
	struct platform_device *pdev,
	const struct ts_filter_configuration *conf,
	int count_coords)
{
	int *p;
	int n;
	struct ts_filter_median *tsfm = kzalloc(sizeof(struct ts_filter_median),
								    GFP_KERNEL);

	if (!tsfm)
		return NULL;

	tsfm->config = container_of(conf,
				    struct ts_filter_median_configuration,
				    config);

	tsfm->tsf.count_coords = count_coords;

	tsfm->config->midpoint = (tsfm->config->extent >> 1) + 1;

	p = kmalloc(2 * count_coords * sizeof(int) * (tsfm->config->extent + 1),
		    GFP_KERNEL);
	if (!p) {
		kfree(tsfm);
		return NULL;
	}

	for (n = 0; n < count_coords; n++) {
		tsfm->sort[n] = p;
		p += tsfm->config->extent + 1;
		tsfm->fifo[n] = p;
		p += tsfm->config->extent + 1;
	}

	ts_filter_median_clear(&tsfm->tsf);

	dev_info(&pdev->dev,
		 "Created Median filter len:%d coords:%d dec_threshold:%d\n",
		 tsfm->config->extent, count_coords,
		 tsfm->config->decimation_threshold);

	return &tsfm->tsf;
}

static void ts_filter_median_destroy(struct ts_filter *tsf)
{
	struct ts_filter_median *tsfm = ts_filter_to_filter_median(tsf);

	kfree(tsfm->sort[0]); /* First guy has pointer from kmalloc. */
	kfree(tsf);
}

static void ts_filter_median_scale(struct ts_filter *tsf, int *coords)
{
	int n;

	for (n = 0; n < tsf->count_coords; n++)
		coords[n] = (coords[n] + 2) / 3;
}

/*
 * Give us the raw sample data coords, and if we return 1 then you can
 * get a filtered coordinate from coords. If we return 0 you didn't
 * fill all the filters with samples yet.
 */

static int ts_filter_median_process(struct ts_filter *tsf, int *coords)
{
	struct ts_filter_median *tsfm = ts_filter_to_filter_median(tsf);
	int n;
	int movement = 1;

	for (n = 0; n < tsf->count_coords; n++) {
		/* Grab copy in insertion order to remove when oldest. */
		tsfm->fifo[n][tsfm->pos] = coords[n];
		/* Insert these samples in sorted order in the median arrays. */
		ts_filter_median_insert(tsfm->sort[n], coords[n], tsfm->valid);
	}
	/* Move us on in the fifo. */
	if (++tsfm->pos == (tsfm->config->extent + 1))
		tsfm->pos = 0;

	/* Have we finished a median sampling? */
	if (++tsfm->valid < tsfm->config->extent)
		goto process_exit; /* No valid sample to use. */

	BUG_ON(tsfm->valid != tsfm->config->extent);

	tsfm->valid--;

	/*
	 * Sum the middle 3 in the median sorted arrays. We don't divide back
	 * down which increases the sum resolution by a factor of 3 until the
	 * scale API function is called.
	 */
	for (n = 0; n < tsf->count_coords; n++)
		/* Perform the deletion of the oldest sample. */
		ts_filter_median_del(tsfm->sort[n], tsfm->fifo[n][tsfm->pos],
				     tsfm->valid);

	tsfm->samples_count--;
	if (tsfm->samples_count >= 0)
		goto process_exit;

	for (n = 0; n < tsf->count_coords; n++) {
		/* Give the coordinate result from summing median 3. */
		coords[n] = tsfm->sort[n][tsfm->config->midpoint - 1] +
			    tsfm->sort[n][tsfm->config->midpoint] +
			    tsfm->sort[n][tsfm->config->midpoint + 1];

		movement += abs(tsfm->last_issued[n] - coords[n]);
	}

	if (movement > tsfm->config->decimation_threshold) /* Moving fast. */
		tsfm->samples_count = tsfm->config->decimation_above;
	else
		tsfm->samples_count = tsfm->config->decimation_below;

	memcpy(&tsfm->last_issued[0], coords, tsf->count_coords * sizeof(int));

	tsfm->ready = 1;

process_exit:
	return 0;
}

static int ts_filter_median_haspoint(struct ts_filter *tsf)
{
	struct ts_filter_median *priv = ts_filter_to_filter_median(tsf);

	return priv->ready;
}

static void ts_filter_median_getpoint(struct ts_filter *tsf, int *point)
{
	struct ts_filter_median *priv = ts_filter_to_filter_median(tsf);

	BUG_ON(!priv->ready);

	memcpy(point, &priv->last_issued[0], tsf->count_coords * sizeof(int));

	priv->ready = 0;
}

const struct ts_filter_api ts_filter_median_api = {
	.create =	ts_filter_median_create,
	.destroy =	ts_filter_median_destroy,
	.clear =	ts_filter_median_clear,
	.process =	ts_filter_median_process,
	.scale =	ts_filter_median_scale,
	.haspoint =	ts_filter_median_haspoint,
	.getpoint =	ts_filter_median_getpoint,
};
EXPORT_SYMBOL_GPL(ts_filter_median_api);

