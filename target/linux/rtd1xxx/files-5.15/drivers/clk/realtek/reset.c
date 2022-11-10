// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2019 Realtek Semiconductor Corporation
 */

#include <linux/of.h>
#include <linux/device.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>
#include <linux/slab.h>
#include "reset.h"

#define RTK_RESET_MAGIC (0x20)

#define RTK_RESET_HWLOCK_TIMEOUT_MS (2)

static int rtk_reset_should_locked(struct rtk_reset_data *data)
{
	return data->shared && data->lock;
}

static int rtk_reset_reg_lock(struct rtk_reset_data *data)
{
	int ret;

	if (!rtk_reset_should_locked(data))
		return 0;

	ret = hwspin_lock_timeout_raw(data->lock, RTK_RESET_HWLOCK_TIMEOUT_MS);
	if (ret)
		dev_warn(data->dev, "%s: lock hwlock failed with %pe\n", __func__, ERR_PTR(ret));
	return ret == 0;
}

static void rtk_reset_reg_unlock(struct rtk_reset_data *data, int locked)
{
	if (!rtk_reset_should_locked(data) || !locked)
		return;
	hwspin_unlock_raw(data->lock);
}

static int rtk_reset_update_bits(struct rtk_reset_data *data,
		uint32_t offset, uint32_t mask, uint32_t val)
{
	int locked;
	int ret;

	locked = rtk_reset_reg_lock(data);

	ret = regmap_update_bits(data->regmap, offset, mask, val);

	rtk_reset_reg_unlock(data, locked);

	return ret;
}

static int rtk_reset_read(struct rtk_reset_data *data,
		uint32_t offset, uint32_t *val)
{
	int locked;
	int ret;

	locked = rtk_reset_reg_lock(data);

	ret = regmap_read(data->regmap, offset, val);

	rtk_reset_reg_unlock(data, locked);

	return ret;
}

static int rtk_reset_assert(struct reset_controller_dev *rcdev,
			    unsigned long idx)
{
	struct rtk_reset_data *data = to_rtk_reset_controller(rcdev);
	struct rtk_reset_bank *bank = rtk_reset_get_bank(data, idx);
	uint32_t id   = rtk_reset_get_id(data, idx);
	uint32_t mask = bank->write_en ? (0x3 << id) : BIT(id);
	uint32_t val  = bank->write_en ? (0x2 << id) : 0;

	dev_dbg(data->dev, "%s: idx=%lx\n", __func__, idx);

	return rtk_reset_update_bits(data, bank->ofs, mask, val);
}

static int rtk_reset_deassert(struct reset_controller_dev *rcdev,
			      unsigned long idx)
{
	struct rtk_reset_data *data = to_rtk_reset_controller(rcdev);
	struct rtk_reset_bank *bank = rtk_reset_get_bank(data, idx);
	uint32_t id   = rtk_reset_get_id(data, idx);
	uint32_t mask = bank->write_en ? (0x3 << id) : BIT(id);
	uint32_t val  = mask;

	dev_dbg(data->dev, "%s: idx=%lx\n", __func__, idx);

	return rtk_reset_update_bits(data, bank->ofs, mask, val);
}

static int rtk_reset_reset(struct reset_controller_dev *rcdev,
			   unsigned long idx)
{
	struct rtk_reset_data *data = to_rtk_reset_controller(rcdev);
	int ret;

	dev_dbg(data->dev, "%s: idx=%lx\n", __func__, idx);

	ret = rtk_reset_assert(rcdev, idx);
	if (ret)
		return ret;

	return rtk_reset_deassert(rcdev, idx);
}

static int rtk_reset_status(struct reset_controller_dev *rcdev,
			    unsigned long idx)
{
	struct rtk_reset_data *data = to_rtk_reset_controller(rcdev);
	struct rtk_reset_bank *bank = &data->banks[idx >> 8];
	uint32_t id = idx & 0xff;
	uint32_t val;

	rtk_reset_read(data, bank->ofs, &val);
	return !((val >> id) & 1);
}

static struct reset_control_ops rtk_reset_ops = {
	.assert   = rtk_reset_assert,
	.deassert = rtk_reset_deassert,
	.reset    = rtk_reset_reset,
	.status   = rtk_reset_status,
};

static int rtk_of_reset_xlate(struct reset_controller_dev *rcdev,
			      const struct of_phandle_args *reset_spec)
{
	int val;

	val = reset_spec->args[0];
	if (val >= rcdev->nr_resets)
		return -EINVAL;

	return val;
}

int rtk_reset_controller_add(struct device *dev,
			     struct rtk_reset_initdata *initdata)
{
	struct rtk_reset_data *data;
	struct device_node *np = dev->of_node;
	int ret;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->dev       = dev;
	data->num_banks = initdata->num_banks;
	data->banks     = initdata->banks;
	data->regmap    = initdata->regmap;
	data->lock      = initdata->lock;
	data->shared    = initdata->shared;

	WARN_ON_ONCE(data->shared && !data->lock);

	data->rcdev.owner     = THIS_MODULE;
	data->rcdev.ops       = &rtk_reset_ops;
	data->rcdev.of_node   = np;
	data->rcdev.nr_resets = initdata->num_banks * 0x100;
	data->rcdev.of_xlate  = rtk_of_reset_xlate;
	data->rcdev.of_reset_n_cells = 1;

	ret = reset_controller_register(&data->rcdev);
	if (ret)
		kfree(data);
	return ret;
}
EXPORT_SYMBOL_GPL(rtk_reset_controller_add);
