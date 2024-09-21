// SPDX-License-Identifier: GPL-2.0-or-later
/* a mtdsplit driver for Fortinet FortiGate/FortiWiFi devices */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/nvmem-consumer.h>

#include "mtdsplit.h"

#define NR_PARTS		2

#define FWINFO_MAGIC		0x434f5346 /* "COSF" (BE: "FSOC") */
#define BLOCKSIZE		0x200

static int fwinfo_read_nvmem(struct mtd_info *mtd,
			     size_t *offset, size_t *length)
{
	u32 magic;
	u8 active;
	int ret;

	ret = nvmem_cell_read_u32(&mtd->dev, "magic", &magic);
	if (ret)
		return ret;
	else if (magic != FWINFO_MAGIC)
		return -EINVAL;

	ret = nvmem_cell_read_u8(&mtd->dev, "active-image", &active);
	if (!ret)
		ret = nvmem_cell_read_u32(&mtd->dev, "kernel-offset", offset);
	if (!ret)
		ret = nvmem_cell_read_u32(&mtd->dev, "kernel-length", length);

	return ret ? ret : active;
}

static int mtdsplit_parse_fortigate_fw(struct mtd_info *master,
				       const struct mtd_partition **pparts,
				       struct mtd_part_parser_data *data)
{
	struct device_node *np = mtd_get_of_node(master);
	struct mtd_partition *parts;
	size_t kernel_offset, kernel_length, rootfs_offset;
	int ret, index, active;

	active = fwinfo_read_nvmem(master, &kernel_offset, &kernel_length);
	if (active < 0)
		return active;

	ret = of_property_read_u32(np, "fortinet,image-index", &index);
	if (ret)
		return ret;

	if (index != active)
		return -ENODEV;

	/* convert to bytes from blocks */
	kernel_offset *= BLOCKSIZE;
	kernel_length *= BLOCKSIZE;
	if (kernel_offset != master->part.offset) {
		/*
		 * indexes in the names of kernel/rootfs partitions
		 * on the stock firmware are 1 and 2 (kn1/rfs1, kn2/rfs2)
		 */
		pr_err("firmware-info has wrong offset for kernel%d (0x%08zx)\n",
		       index + 1, kernel_offset);
		return -EINVAL;
	}

	if (kernel_length > master->size) {
		pr_err("kernel image exceeds MTD device \"%s\"\n", master->name);
		return -EINVAL;
	}

	parts = kcalloc(NR_PARTS, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	/* find the roots after the kernel image */
	ret = mtd_find_rootfs_from(master, kernel_length,
				   master->size, &rootfs_offset, NULL);
	if (ret || (master->size - rootfs_offset) == 0) {
		pr_debug("no rootfs after kernel image in \"%s\"\n",
			 master->name);
		ret = -ENODEV;
		goto err_free_parts;
	}

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = rootfs_offset;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return NR_PARTS;

err_free_parts:
	kfree(parts);
	return ret;
}

static const struct of_device_id mtdsplit_fortigate_fw_of_match_table[] = {
	{ .compatible = "fortinet,fortigate-firmware" },
	{},
};
MODULE_DEVICE_TABLE(of, mtdsplit_fortigate_fw_of_match_table);

static struct mtd_part_parser mtdsplit_fortigate_fw_parser = {
	.owner = THIS_MODULE,
	.name = "fortigate-fw",
	.of_match_table = mtdsplit_fortigate_fw_of_match_table,
	.parse_fn = mtdsplit_parse_fortigate_fw,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

module_mtd_part_parser(mtdsplit_fortigate_fw_parser);
