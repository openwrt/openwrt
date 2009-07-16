/*
 *  linux/drivers/mmc/host/glamo-mmc.h - GLAMO MCI driver
 *
 *  Copyright (C) 2007-2008 Openmoko, Inc, Andy Green <andy@openmoko.com>
 *   based on S3C MMC driver -->
 *  Copyright (C) 2004-2006 Thomas Kleffel, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/regulator/consumer.h>

struct glamo_mci_host {
	struct platform_device *pdev;
	struct glamo_mmc_platform_data *pdata;
	struct mmc_host        *mmc;
	struct resource        *mmio_mem;
	struct resource        *data_mem;
	void __iomem           *mmio_base;
	u16 __iomem            *data_base;

	int suspending;

	int          power_mode_current;
	unsigned int vdd_current;

	unsigned long clk_rate;
	unsigned long clk_div;
	unsigned long real_rate;

	int force_slow_during_powerup;

	struct mmc_request *mrq;
	struct work_struct  irq_work;

	spinlock_t lock;

	unsigned int request_counter;

	struct regulator *regulator;
};
