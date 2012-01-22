/*
 * Parallel flash driver for the Atheros AR91xx SoC
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/io.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/ar91xx_flash.h>

#define DRV_NAME	"ar91xx-flash"

struct ar91xx_flash_info {
	struct mtd_info		*mtd;
	struct map_info		map;
#ifdef CONFIG_MTD_PARTITIONS
	int			nr_parts;
	struct mtd_partition	*parts;
#endif
};

static map_word ar91xx_flash_read(struct map_info *map, unsigned long ofs)
{
	map_word val;

	if (map_bankwidth_is_1(map))
		val.x[0] = __raw_readb(map->virt + (ofs ^ 3));
	else if (map_bankwidth_is_2(map))
		val.x[0] = __raw_readw(map->virt + (ofs ^ 2));
	else
		val = map_word_ff(map);

	return val;
}

static void ar91xx_flash_write(struct map_info *map, map_word d,
			       unsigned long ofs)
{
	if (map_bankwidth_is_1(map))
		__raw_writeb(d.x[0], map->virt + (ofs ^ 3));
	else if (map_bankwidth_is_2(map))
		__raw_writew(d.x[0], map->virt + (ofs ^ 2));

	mb();
}

static map_word ar91xx_flash_read_lock(struct map_info *map, unsigned long ofs)
{
	map_word ret;

	ar71xx_flash_acquire();
	ret = ar91xx_flash_read(map, ofs);
	ar71xx_flash_release();

	return ret;
}

static void ar91xx_flash_write_lock(struct map_info *map, map_word d,
			       unsigned long ofs)
{
	ar71xx_flash_acquire();
	ar91xx_flash_write(map, d, ofs);
	ar71xx_flash_release();
}

static void ar91xx_flash_copy_from_lock(struct map_info *map, void *to,
					unsigned long from, ssize_t len)
{
	ar71xx_flash_acquire();
	inline_map_copy_from(map, to, from, len);
	ar71xx_flash_release();
}

static void ar91xx_flash_copy_to_lock(struct map_info *map, unsigned long to,
				      const void *from, ssize_t len)
{
	ar71xx_flash_acquire();
	inline_map_copy_to(map, to, from, len);
	ar71xx_flash_release();
}

static int ar91xx_flash_remove(struct platform_device *pdev)
{
	struct ar91xx_flash_platform_data *pdata;
	struct ar91xx_flash_info *info;

	info = platform_get_drvdata(pdev);
	if (info == NULL)
		return 0;

	platform_set_drvdata(pdev, NULL);

	if (info->mtd == NULL)
		return 0;

	pdata = pdev->dev.platform_data;
#ifdef CONFIG_MTD_PARTITIONS
	if (info->nr_parts) {
		del_mtd_partitions(info->mtd);
		kfree(info->parts);
	} else if (pdata->nr_parts) {
		del_mtd_partitions(info->mtd);
	} else {
		del_mtd_device(info->mtd);
	}
#else
	del_mtd_device(info->mtd);
#endif
	map_destroy(info->mtd);

	return 0;
}

static const char *rom_probe_types[] = { "cfi_probe", "jedec_probe", NULL };
#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probe_types[] = { "cmdlinepart", "RedBoot", NULL };
#endif

static int ar91xx_flash_probe(struct platform_device *pdev)
{
	struct ar91xx_flash_platform_data *pdata;
	struct ar91xx_flash_info *info;
	struct resource *res;
	struct resource *region;
	const char **probe_type;
	int err = 0;

	pdata = pdev->dev.platform_data;
	if (pdata == NULL)
		return -EINVAL;

	info = devm_kzalloc(&pdev->dev, sizeof(struct ar91xx_flash_info),
			    GFP_KERNEL);
	if (info == NULL) {
		err = -ENOMEM;
		goto err_out;
	}

	platform_set_drvdata(pdev, info);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		err = -ENOENT;
		goto err_out;
	}

	dev_info(&pdev->dev, "%.8llx at %.8llx\n",
		 (unsigned long long)(res->end - res->start + 1),
		 (unsigned long long)res->start);

	region = devm_request_mem_region(&pdev->dev,
					 res->start, res->end - res->start + 1,
					 dev_name(&pdev->dev));
	if (region == NULL) {
		dev_err(&pdev->dev, "could not reserve memory region\n");
		err = -ENOMEM;
		goto err_out;
	}

	info->map.name = dev_name(&pdev->dev);
	info->map.phys = res->start;
	info->map.size = res->end - res->start + 1;
	info->map.bankwidth = pdata->width;

	info->map.virt = devm_ioremap(&pdev->dev, info->map.phys,
				      info->map.size);
	if (info->map.virt == NULL) {
		dev_err(&pdev->dev, "failed to ioremap flash region\n");
		err = -EIO;
		goto err_out;
	}

	simple_map_init(&info->map);
	if (pdata->is_shared) {
		info->map.read = ar91xx_flash_read_lock;
		info->map.write = ar91xx_flash_write_lock;
		info->map.copy_from = ar91xx_flash_copy_from_lock;
		info->map.copy_to = ar91xx_flash_copy_to_lock;
	} else {
		info->map.read = ar91xx_flash_read;
		info->map.write = ar91xx_flash_write;
	}

	probe_type = rom_probe_types;
	for (; info->mtd == NULL && *probe_type != NULL; probe_type++)
		info->mtd = do_map_probe(*probe_type, &info->map);

	if (info->mtd == NULL) {
		dev_err(&pdev->dev, "map_probe failed\n");
		err = -ENXIO;
		goto err_out;
	}

	info->mtd->owner = THIS_MODULE;

#ifdef CONFIG_MTD_PARTITIONS
	if (pdata->nr_parts) {
		dev_info(&pdev->dev, "using static partition mapping\n");
		add_mtd_partitions(info->mtd, pdata->parts, pdata->nr_parts);
		return 0;
	}

	err = parse_mtd_partitions(info->mtd, part_probe_types,
				   &info->parts, 0);
	if (err > 0) {
		add_mtd_partitions(info->mtd, info->parts, err);
		return 0;
	}
#endif

	add_mtd_device(info->mtd);
	return 0;

err_out:
	ar91xx_flash_remove(pdev);
	return err;
}

#ifdef CONFIG_PM
static int ar91xx_flash_suspend(struct platform_device *dev, pm_message_t state)
{
	struct ar91xx_flash_info *info = platform_get_drvdata(dev);
	int ret = 0;

	if (info->mtd->suspend)
		ret = info->mtd->suspend(info->mtd);

	if (ret)
		goto fail;

	return 0;

fail:
	if (info->mtd->suspend) {
		BUG_ON(!info->mtd->resume);
		info->mtd->resume(info->mtd);
	}

	return ret;
}

static int ar91xx_flash_resume(struct platform_device *pdev)
{
	struct ar91xx_flash_info *info = platform_get_drvdata(pdev);

	if (info->mtd->resume)
		info->mtd->resume(info->mtd);

	return 0;
}

static void ar91xx_flash_shutdown(struct platform_device *pdev)
{
	struct ar91xx_flash_info *info = platform_get_drvdata(pdev);

	if (info->mtd->suspend && info->mtd->resume)
		if (info->mtd->suspend(info->mtd) == 0)
			info->mtd->resume(info->mtd);
}
#else
#define ar91xx_flash_suspend	NULL
#define ar91xx_flash_resume	NULL
#define ar91xx_flash_shutdown	NULL
#endif

static struct platform_driver ar91xx_flash_driver = {
	.probe		= ar91xx_flash_probe,
	.remove		= ar91xx_flash_remove,
	.suspend	= ar91xx_flash_suspend,
	.resume		= ar91xx_flash_resume,
	.shutdown	= ar91xx_flash_shutdown,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init ar91xx_flash_init(void)
{
	return platform_driver_register(&ar91xx_flash_driver);
}

static void __exit ar91xx_flash_exit(void)
{
	platform_driver_unregister(&ar91xx_flash_driver);
}

module_init(ar91xx_flash_init);
module_exit(ar91xx_flash_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_DESCRIPTION("Parallel flash driver for the Atheros AR91xx SoC");
MODULE_ALIAS("platform:" DRV_NAME);
