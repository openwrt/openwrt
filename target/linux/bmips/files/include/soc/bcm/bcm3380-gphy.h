/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_BCM3380_GPHY_H
#define __SOC_BCM3380_GPHY_H

#include <linux/device.h>

struct bcm3380_gphy;

int gphy_get(struct device *consumer, struct bcm3380_gphy **gphy);
void gphy_put(void *data);

#endif /* __SOC_BCM3380_GPHY_H */
