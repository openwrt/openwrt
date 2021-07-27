/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef __FAB_SCALING_H
#define __FAB_SCALING_H

struct fab_scaling_info {
	struct clk *clk;
	unsigned long idle_freq;
};

/**
 * fab_scaling_register - Register for APPS and DDR FABRICS scaling
 * @data: FABRIC scaling info
 *
 * This registers the clock source which needs to be monitored.
 *
 */
int fab_scaling_register(struct fab_scaling_info *data);

/**
 * scale_fabrics - Scale DDR and APPS FABRICS
 *
 * This function monitors all the registered clocks and does APPS
 * and DDR FABRIC scaling based on the idle frequencies with which
 * it was registered.
 *
 */
int scale_fabrics(void);

/**
 * fab_scaling_unregister - Unregister for APPS and DDR FABRICS scaling
 * @clk: Clock pointer which needs to be unregistered.
 *
 * This unregisters the clock source and is no longer monitored.
 *
 */
int fab_scaling_unregister(struct clk *clk);

#endif
