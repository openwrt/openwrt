/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __CS752X_NE_H__
#define __CS752X_NE_H__

#include <linux/types.h>

struct device;

int cs752x_fe_init(struct device *dev, void __iomem *fe);
int cs752x_sch_init(struct device *dev, void __iomem *sch);
int cs752x_tm_init(struct device *dev, void __iomem *tm, bool internal_buf);

#endif
