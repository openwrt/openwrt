/*
 * BCM947xx nvram variable access
 *
 * Copyright (C) 2005 Broadcom Corporation
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2010-2014 Hauke Mehrtens <hauke@hauke-m.de>
 *
 * This program is free software; you can redistribute	it and/or modify it
 * under  the terms of	the GNU General	 Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/of_address.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/bcm47xx_nvram.h>

struct bcm47xx_nvram {
	size_t nvram_len;
	char *nvram_buf;
};

static const u32 nvram_sizes[] = {0x8000, 0xF000, 0x10000};

static u32 find_nvram_size(void __iomem *end)
{
	struct nvram_header __iomem *header;
	int i;

	for (i = 0; i < ARRAY_SIZE(nvram_sizes); i++) {
		header = (struct nvram_header __iomem *)(end - nvram_sizes[i]);
		if (__raw_readl(&header->magic) == NVRAM_HEADER)
			return nvram_sizes[i];
	}

	return 0;
}

/* Probe for NVRAM header */
static int nvram_find_and_copy(struct device *dev, void __iomem *base,
			       size_t len, char **nvram_buf,
			       size_t *nvram_len)
{
	struct nvram_header __iomem *header;
	int i;
	u32 off;
	u32 *dst;
	__le32 __iomem *src;
	u32 size;

	/* TODO: when nvram is on nand flash check for bad blocks first. */
	off = FLASH_MIN;
	while (off <= len) {
		/* Windowed flash access */
		size = find_nvram_size(base + off);
		if (size) {
			header = (struct nvram_header __iomem *)
					(base + off - size);
			goto found;
		}
		off += 0x10000;
	}

	/* Try embedded NVRAM at 4 KB and 1 KB as last resorts */
	header = (struct nvram_header __iomem *)(base + 4096);
	if (__raw_readl(&header->magic) == NVRAM_HEADER) {
		size = NVRAM_SPACE;
		goto found;
	}

	header = (struct nvram_header __iomem *)(base + 1024);
	if (__raw_readl(&header->magic) == NVRAM_HEADER) {
		size = NVRAM_SPACE;
		goto found;
	}

	*nvram_buf = NULL;
	*nvram_len = 0;
	pr_err("no nvram found\n");
	return -ENXIO;

found:
	if (readl(&header->len) > size)
		pr_err("The nvram size accoridng to the header seems to be bigger than the partition on flash\n");
	*nvram_len = min_t(u32, readl(&header->len), size);

	*nvram_buf = devm_kzalloc(dev, *nvram_len, GFP_KERNEL);
	if (!*nvram_buf)
		return -ENOMEM;

	src = (__le32 __iomem *) header;
	dst = (u32 *) *nvram_buf;
	for (i = 0; i < sizeof(struct nvram_header); i += 4)
		*dst++ = __raw_readl(src++);
	for (; i < *nvram_len; i += 4)
		*dst++ = readl(src++);

	return 0;
}

int bcm47xx_nvram_getenv(const struct device *dev, const char *name, char *val,
			 size_t val_len)
{
	char *var, *value, *end, *eq;
	struct bcm47xx_nvram *nvram;

	if (!dev)
		return -ENODEV;

	nvram = dev_get_drvdata(dev);

	if (!name || !nvram || !nvram->nvram_len)
		return -EINVAL;

	/* Look for name=value and return value */
	var = nvram->nvram_buf + sizeof(struct nvram_header);
	end = nvram->nvram_buf + nvram->nvram_len - 2;
	end[0] = end[1] = '\0';
	for (; *var; var = value + strlen(value) + 1) {
		eq = strchr(var, '=');
		if (!eq)
			break;
		value = eq + 1;
		if ((eq - var) == strlen(name) &&
			strncmp(var, name, (eq - var)) == 0) {
			return snprintf(val, val_len, "%s", value);
		}
	}
	return -ENOENT;
}
EXPORT_SYMBOL(bcm47xx_nvram_getenv);

int bcm47xx_nvram_gpio_pin(const struct device *dev, const char *name)
{
	int i, err;
	char nvram_var[10];
	char buf[30];

	if (!dev)
		return -ENODEV;

	for (i = 0; i < 32; i++) {
		err = snprintf(nvram_var, sizeof(nvram_var), "gpio%i", i);
		if (err <= 0)
			continue;
		err = bcm47xx_nvram_getenv(dev, nvram_var, buf, sizeof(buf));
		if (err <= 0)
			continue;
		if (!strcmp(name, buf))
			return i;
	}
	return -ENOENT;
}
EXPORT_SYMBOL(bcm47xx_nvram_gpio_pin);

static int bcm47xx_nvram_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct bcm47xx_nvram *nvram;
	int err;
	struct resource flash_mem;
	void __iomem *mmio;

	/* Alloc */
	nvram = devm_kzalloc(dev, sizeof(*nvram), GFP_KERNEL);
	if (!nvram)
		return -ENOMEM;

	err = of_address_to_resource(np, 0, &flash_mem);
	if (err)
		return err;

	mmio = ioremap_nocache(flash_mem.start, resource_size(&flash_mem));
	if (!mmio)
		return -ENOMEM;

	err = nvram_find_and_copy(dev, mmio, resource_size(&flash_mem),
				  &nvram->nvram_buf, &nvram->nvram_len);
	if (err)
		goto err_unmap_mmio;

	platform_set_drvdata(pdev, nvram);

err_unmap_mmio:
	iounmap(mmio);
	return err;
}

static const struct of_device_id bcm47xx_nvram_of_match_table[] = {
	{ .compatible = "brcm,bcm47xx-nvram", },
	{},
};
MODULE_DEVICE_TABLE(of, mvebu_pcie_of_match_table);

static struct platform_driver bcm47xx_nvram_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "bcm47xx-nvram",
		.of_match_table = bcm47xx_nvram_of_match_table,
		/* driver unloading/unbinding currently not supported */
		.suppress_bind_attrs = true,
	},
	.probe = bcm47xx_nvram_probe,
};
module_platform_driver(bcm47xx_nvram_driver);

MODULE_AUTHOR("Hauke Mehrtens <hauke@hauke-m.de>");
MODULE_LICENSE("GPLv2");
