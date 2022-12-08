// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 Realtek Semiconductor Corporation
 */

#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/hwspinlock.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include "hwspinlock_internal.h"

#define SPINLOCK_NOTTAKEN		(0)	/* free */
#define SPINLOCK_TAKEN			(1)	/* locked */

static int rtk_hwspinlock_trylock(struct hwspinlock *lock)
{
	void __iomem *lock_addr = lock->priv;

	return readl(lock_addr) == SPINLOCK_TAKEN;
}

static void rtk_hwspinlock_unlock(struct hwspinlock *lock)
{
	void __iomem *lock_addr = lock->priv;

	writel(SPINLOCK_NOTTAKEN, lock_addr);
}

static void rtk_hwspinlock_relax(struct hwspinlock *lock)
{
	ndelay(50);
}

static const struct hwspinlock_ops rtk_hwspinlock_ops = {
	.trylock = rtk_hwspinlock_trylock,
	.unlock = rtk_hwspinlock_unlock,
	.relax = rtk_hwspinlock_relax,
};

static atomic_t rtk_hwspinlock_base_id = ATOMIC_INIT(0);

static int rtk_hwspinlock_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hwspinlock_device *bank;
	struct hwspinlock *hwlock;
	struct resource *res;
	void __iomem *io_base;
	int num_locks, i;
	int base_id;
	int ret;
	const __be32 *p;

	io_base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (!io_base)
		return -ENOMEM;

	p = of_get_property(dev->of_node, "realtek,lock-id-maps", &num_locks);
	if (p && num_locks > 0)
		num_locks /= 4;
	else
		num_locks = resource_size(res) / 4;

	bank = devm_kzalloc(dev, struct_size(bank, lock, num_locks), GFP_KERNEL);
	if (!bank)
		return -ENOMEM;

	platform_set_drvdata(pdev, bank);

	for (i = 0, hwlock = &bank->lock[0]; i < num_locks; i++, hwlock++) {
		int offset = i * 4;

		if (p)
			offset = be32_to_cpup(p++);

		if (offset > (int)resource_size(res)) {
			dev_err(dev, "invalid offset %d\n", offset);
			return -EINVAL;
		}

		dev_dbg(dev, "apply index=%d with offset=%d\n", i, offset);
		hwlock->priv = io_base + offset;
	}

	base_id = atomic_fetch_add(num_locks, &rtk_hwspinlock_base_id);

	ret = devm_hwspin_lock_register(dev, bank, &rtk_hwspinlock_ops,
						base_id, num_locks);
	if (!ret)
		dev_info(dev, "add %d lock(s) from base_id %d%s\n",
			num_locks, base_id, p ? " with map" : "");
	return ret;
}

static int rtk_hwspinlock_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id rtk_hwspinlock_of_match[] = {
	{ .compatible = "realtek,sb2-hwlock", },
	{ /* end */ },
};
MODULE_DEVICE_TABLE(of, rtk_hwspinlock_of_match);

static struct platform_driver rtk_hwspinlock_driver = {
	.probe		= rtk_hwspinlock_probe,
	.remove		= rtk_hwspinlock_remove,
	.driver		= {
		.name	= "rtk-hwlock",
		.of_match_table = of_match_ptr(rtk_hwspinlock_of_match),
	},
};

static int __init rtk_hwspinlock_init(void)
{
	return platform_driver_register(&rtk_hwspinlock_driver);
}
postcore_initcall(rtk_hwspinlock_init);

static void __exit rtk_hwspinlock_exit(void)
{
	platform_driver_unregister(&rtk_hwspinlock_driver);
}
module_exit(rtk_hwspinlock_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Hardware Semaphore driver for RealTek");
