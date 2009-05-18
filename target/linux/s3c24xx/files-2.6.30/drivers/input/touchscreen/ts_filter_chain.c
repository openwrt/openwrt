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
 * Copyright (c) 2008,2009 Andy Green <andy@openmoko.com>
 */

#include <linux/kernel.h>
#include <linux/device.h>

#include <linux/touchscreen/ts_filter_chain.h>
#include <linux/touchscreen/ts_filter.h>

/*
 * Tux, would you like the following function in /lib?
 * It helps us avoid silly code.
 */

/**
 * sptrlen - Count how many non-null pointers are in a pointer array
 * @arr: The array of pointers
 */
static int sptrlen(const void *arr)
{
	/* All pointers have the same size. */
	const int **p = (const int **)arr;
	int len = 0;

	while (*(p++))
		len++;

	return len;
}


struct ts_filter_chain {
	/* All of the filters. */
	struct ts_filter **arr;
	/* Filters that can propagate values in the chain. */
	struct ts_filter **pchain;
	/* Length of the pchain array. */
	int pchain_len;
	/* FIXME: Add a spinlock and use it. */
};

struct ts_filter_chain *ts_filter_chain_create(
	struct platform_device *pdev,
	const struct ts_filter_chain_configuration conf[],
	int count_coords)
{
	struct ts_filter_chain *c;
	int count = 0;
	int len;

	BUG_ON((count_coords < 1));
	BUG_ON(count_coords > MAX_TS_FILTER_COORDS);

	c = kzalloc(sizeof(struct ts_filter_chain), GFP_KERNEL);
	if (!c)
		goto create_err_1;

	len = (sptrlen(conf) + 1);
	/* Memory for two null-terminated arrays of filters. */
	c->arr = kzalloc(2 * sizeof(struct ts_filter *) * len, GFP_KERNEL);
	if (!c->arr)
		goto create_err_1;
	c->pchain = c->arr + len;

	while (conf->api) {
		/* TODO: Can we get away with only sending pdev->dev? */
		struct ts_filter *f =
			(conf->api->create)(pdev, conf->config, count_coords);
		if (!f) {
			dev_info(&pdev->dev, "Filter %d creation failed\n",
				 count);
			goto create_err_2;
		}

		f->api = conf->api;
		c->arr[count++] = f;

		if (f->api->haspoint && f->api->getpoint && f->api->process)
			c->pchain[c->pchain_len++] = f;

		conf++;
	}

	dev_info(&pdev->dev, "%d filter(s) initialized\n", count);

	return c;

create_err_2:
	ts_filter_chain_destroy(c); /* Also frees c. */
create_err_1:
	dev_info(&pdev->dev, "Error in filter chain initialization\n");
	/*
	 * FIXME: Individual filters have to return errors this way.
	 * We only have to forward the errors we find.
	 */
	return ERR_PTR(-ENOMEM);
}
EXPORT_SYMBOL_GPL(ts_filter_chain_create);

void ts_filter_chain_destroy(struct ts_filter_chain *c)
{
	if (c->arr) {
		struct ts_filter **a = c->arr;
		while (*a) {
			((*a)->api->destroy)(*a);
			a++;
		}
		kfree(c->arr);
	}
	kfree(c);
}
EXPORT_SYMBOL_GPL(ts_filter_chain_destroy);

void ts_filter_chain_clear(struct ts_filter_chain *c)
{
	struct ts_filter **a = c->arr;

	while (*a) {
		if ((*a)->api->clear)
			((*a)->api->clear)(*a);
		a++;
	}
}
EXPORT_SYMBOL_GPL(ts_filter_chain_clear);

static void ts_filter_chain_scale(struct ts_filter_chain *c, int *coords)
{
	struct ts_filter **a = c->arr;
	while (*a) {
		if ((*a)->api->scale)
			((*a)->api->scale)(*a, coords);
		a++;
	}
}

int ts_filter_chain_feed(struct ts_filter_chain *c, int *coords)
{
	int len = c->pchain_len;
	int i = len - 1;

	if (!c->pchain[0])
		return 1; /* Nothing to do. */

	BUG_ON(c->pchain[0]->api->haspoint(c->pchain[0]));

	if (c->pchain[0]->api->process(c->pchain[0], coords))
		return -1;

	while (i >= 0 && i < len) {
		if (c->pchain[i]->api->haspoint(c->pchain[i])) {
			c->pchain[i]->api->getpoint(c->pchain[i], coords);
			if (++i < len &&
			    c->pchain[i]->api->process(c->pchain[i], coords))
				return -1; /* Error. */
		} else {
			i--;
		}
	}

	if (i >= 0) {	/* Same as i == len. */
		ts_filter_chain_scale(c, coords);
		return 1;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(ts_filter_chain_feed);

