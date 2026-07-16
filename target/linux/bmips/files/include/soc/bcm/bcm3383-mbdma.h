/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_BCM3383_MBDMA_H
#define __SOC_BCM3383_MBDMA_H

#include <linux/device.h>
#include <linux/types.h>

struct bcm3380_fpm_pool;

struct unimac_mbdma;

struct unimac_mbdma {
	bool (*is_dev)(struct unimac_mbdma *mbdma, struct device *dev);
	int (*prepare)(struct unimac_mbdma *mbdma,
		       struct bcm3380_fpm_pool *fpm_pool,
		       u32 in_msg_data_bus_addr);
	u32 (*tx_fifo_bus_addr)(struct unimac_mbdma *mbdma, u32 mac_id);
};

#endif /* __SOC_BCM3383_MBDMA_H */
