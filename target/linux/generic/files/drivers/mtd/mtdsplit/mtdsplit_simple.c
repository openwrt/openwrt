// SPDX-License-Identifier: GPL-2.0-or-later

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/nvmem-consumer.h>
#include <linux/byteorder/generic.h>
#include <linux/of.h>

#include "mtdsplit.h"

#define NR_PARTS	2
#define MAGIC_MAX	4

static int get_cell_value_be32(struct device_node *np, const char *name,
				u32 *value, size_t *len)
{
	struct nvmem_cell *cell;
	void *cell_raw;
	size_t _len;

	cell = of_nvmem_cell_get(np, name);
	if (IS_ERR(cell))
		return (int)PTR_ERR(cell);

	cell_raw = nvmem_cell_read(cell, &_len);
	nvmem_cell_put(cell);
	if (IS_ERR(cell_raw))
		return (int)PTR_ERR(cell_raw);

	if (len)
		*len = _len;

	if (!value)
		return 0;

	else if (_len > sizeof(u32))
		return -EINVAL;

	memcpy(value, cell_raw, _len);
	*value = be32_to_cpu(*value)
			>> ((sizeof(u32) - _len) * BITS_PER_BYTE);

	return 0;
}

static int simple_fw_nvmem_verify(struct device_node *np,
				  size_t *header_len, size_t *data_len)
{
	struct device_node *np_cell;
	u32 cell_val, match_val;
	int ret, i, cell_idx;
	char cell_name[8];

	ret = get_cell_value_be32(np, "header", NULL, header_len);
	if (ret)
		return ret;
	ret = get_cell_value_be32(np, "data-length", data_len, NULL);
	if (ret)
		return ret;

	for (i = 0; i < MAGIC_MAX; i++) {
		scnprintf(cell_name, 8, "magic%d", i + 1);

		ret = get_cell_value_be32(np, cell_name, &cell_val, NULL);
		if (ret)
			return (i > 0 && ret == -ENOENT) ? 0 : ret;

		cell_idx = of_property_match_string(np, "nvmem-cell-names",
						    cell_name);
		pr_debug("got value from cell-> 0x%08x (index: %d)\n",
			 cell_val, cell_idx);
		np_cell = of_parse_phandle(np, "nvmem-cells", cell_idx);
		ret = of_property_read_u32(np_cell, "openwrt,match", &match_val);
		if (ret)
			return ret;

		if (cell_val != match_val)
			return -ENODEV;
	}

	return 0;
}

static int mtdsplit_parse_simple_fw(struct mtd_info *master,
				    const struct mtd_partition **pparts,
				    struct mtd_part_parser_data *data)
{
	struct device_node *np = mtd_get_of_node(master);
	struct mtd_partition *parts;
	size_t header_len, data_len;
	size_t image_size, rootfs_offset;
	enum mtdsplit_part_type type;
	int ret;

	ret = simple_fw_nvmem_verify(np, &header_len, &data_len);
	if (ret) {
		pr_debug("failed to verify (%d)\n", ret);
		return ret;
	}

	parts = kzalloc(NR_PARTS * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	image_size = header_len + data_len;
	if (image_size > master->size) {
		pr_debug("kernel image exceeds MTD device \"%s\"\n", master->name);
		ret = -ENODEV;
		goto err_free_parts;
	}

	ret = mtd_find_rootfs_from(master, image_size,
				   master->size, &rootfs_offset, &type);
	if (ret) {
		pr_debug("no rootfs found after kernel image in \"%s\"\n",
			 master->name);
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

static const struct of_device_id mtdsplit_simple_fw_of_match_table[] = {
	{ .compatible = "openwrt,simple-fw" },
	{},
};
MODULE_DEVICE_TABLE(of, mtdsplit_simple_fw_of_match_table);

static struct mtd_part_parser mtdsplit_simple_fw_parser = {
	.owner = THIS_MODULE,
	.name = "simple-fw",
	.of_match_table = mtdsplit_simple_fw_of_match_table,
	.parse_fn = mtdsplit_parse_simple_fw,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

module_mtd_part_parser(mtdsplit_simple_fw_parser)
