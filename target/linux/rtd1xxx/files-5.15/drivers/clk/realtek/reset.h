/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2019 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#ifndef __CLK_REALTEK_RESET_H
#define __CLK_REALTEK_RESET_H

#include <linux/reset-controller.h>
#include <linux/hwspinlock.h>

struct rtk_reset_bank {
	uint32_t ofs;
	uint32_t write_en;
};

struct device;

struct rtk_reset_data {
	struct device *dev;
	struct reset_controller_dev rcdev;
	struct rtk_reset_bank *banks;
	uint32_t num_banks;
	struct regmap *regmap;
	struct hwspinlock *lock;
	int shared;
};

#define to_rtk_reset_controller(r) \
		container_of(r, struct rtk_reset_data, rcdev)

#define RTK_RESET_BANK_SHIFT       8
#define RTK_RESET_ID_MASK          0xff

static inline
struct rtk_reset_bank *rtk_reset_get_bank(struct rtk_reset_data *data,
					  unsigned long idx)
{
	int bank_id = idx >> RTK_RESET_BANK_SHIFT;

	return &data->banks[bank_id];
}

static inline
int rtk_reset_get_id(struct rtk_reset_data *data, unsigned long idx)
{
	return idx & RTK_RESET_ID_MASK;
}

struct rtk_reset_initdata {
	struct rtk_reset_bank *banks;
	uint32_t num_banks;
	unsigned long (*id_xlate)(unsigned long id);
	struct regmap *regmap;
	struct hwspinlock *lock;
	int shared;
};

int rtk_reset_controller_add(struct device *dev,
			     struct rtk_reset_initdata *initdata);

#endif /* __CLK_REALTEK_RESET_H */
