/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_BCM3383_MBDMA_H
#define __SOC_BCM3383_MBDMA_H

#include <linux/device.h>
#include <linux/types.h>

struct bcm3380_fpm_pool;

struct unimac_mbdma;

struct unimac_mbdma {
	bool (*is_dev)(struct unimac_mbdma *mbdma, struct device *dev);
	struct bcm3380_fpm_pool *(*get_fpm_pool)(struct unimac_mbdma *mbdma);
	u32 (*prepare)(struct unimac_mbdma *mbdma,
		       u32 in_msg_data_bus_addr, u32 mac_id);
};

#endif /* __SOC_BCM3383_MBDMA_H */
