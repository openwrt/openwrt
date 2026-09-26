// SPDX-License-Identifier: GPL-2.0-only
/*
 * Minimal Broadcom eCos Permanent NonVol NVMEM layout.
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 *
 * This only exposes MAC cells described by DT.  It is not a general CMAp
 * settings parser.
 */

#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/nvmem-provider.h>
#include <linux/of.h>
#include <linux/overflow.h>
#include <linux/slab.h>
#include <linux/unaligned.h>

static u32 nonvol_checksum(const u8 *data, size_t len)
{
	u32 sum = 0;

	while (len >= sizeof(u32)) {
		sum += get_unaligned_be32(data);
		data += sizeof(u32);
		len -= sizeof(u32);
	}

	if (len) {
		u32 tail = 0;

		for (int i = 0; i < len; i++)
			tail |= (u32)data[i] << (24 - i * 8);

		sum += tail;
	}

	return ~sum;
}

static int nonvol_find_latest_section(struct device *dev,
				      struct nvmem_device *nvmem,
				      size_t slot_size,
				      u32 section_offset,
				      size_t *section_start,
				      size_t *section_len)
{
	bool found = false;
	size_t dev_size = nvmem_dev_size(nvmem);

	for (size_t slot = 0; slot + section_offset + 8 <= dev_size;
	     slot += slot_size) {
		u8 header[8];
		size_t start = slot + section_offset;
		u32 len;
		u8 *buf;
		int ret;

		ret = nvmem_device_read(nvmem, start, sizeof(header), header);
		if (ret < 0)
			return ret;
		if (ret != (int)sizeof(header))
			return -EIO;

		len = get_unaligned_be32(header);
		if (!len || len == U32_MAX || len > dev_size - start)
			continue;
		if (len > slot_size - section_offset)
			continue;

		buf = kmalloc(len, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		ret = nvmem_device_read(nvmem, start, len, buf);
		if (ret < 0) {
			kfree(buf);
			return ret;
		}
		if (ret != (int)len) {
			kfree(buf);
			return -EIO;
		}

		if (nonvol_checksum(buf, len)) {
			kfree(buf);
			continue;
		}

		kfree(buf);
		*section_start = start;
		*section_len = len;
		found = true;
	}

	if (!found)
		return -ENOENT;

	dev_dbg(dev, "using NonVol section at 0x%zx len=0x%zx\n",
		*section_start, *section_len);

	return 0;
}

static int nonvol_add_one_cell(struct device *dev,
			       struct nvmem_device *nvmem,
			       struct device_node *cell_np,
			       size_t section_start,
			       size_t section_len)
{
	u32 path[8];
	struct nvmem_cell_info info = {};
	u32 offset;
	u8 mac[ETH_ALEN];
	int path_len = of_property_count_u32_elems(cell_np,
						   "brcm,record-path");

	if (path_len <= 0)
		return -EINVAL;
	if (path_len > (int)ARRAY_SIZE(path))
		return -EINVAL;

	int ret = of_property_read_u32_array(cell_np, "brcm,record-path",
					     path, path_len);
	if (ret)
		return ret;

	offset = 0;
	for (int i = 0; i < path_len; i++) {
		if (check_add_overflow(offset, path[i], &offset))
			return -EINVAL;
	}

	if (offset > section_len || ETH_ALEN > section_len - offset)
		return -EINVAL;

	ret = nvmem_device_read(nvmem, section_start + offset, ETH_ALEN, mac);
	if (ret < 0)
		return ret;
	if (ret != ETH_ALEN)
		return -EIO;
	if (!is_valid_ether_addr(mac)) {
		dev_err(dev, "%pOF has invalid MAC %pM\n", cell_np, mac);
		return -EINVAL;
	}

	info.name = cell_np->name;
	info.offset = section_start + offset;
	info.bytes = ETH_ALEN;
	info.np = cell_np;

	ret = nvmem_add_one_cell(nvmem, &info);
	if (ret)
		return ret;

	dev_info(dev, "added %pOF MAC cell at nvmem offset 0x%x: %pM\n",
		 cell_np, info.offset, mac);

	return 0;
}

static int nonvol_add_cells(struct nvmem_layout *layout)
{
	struct nvmem_device *nvmem = layout->nvmem;
	struct device *dev = &layout->dev;
	size_t section_start;
	size_t section_len;
	struct device_node *layout_np = of_nvmem_layout_get_container(nvmem);
	if (!layout_np)
		return -ENOENT;

	u32 slot_size;
	int ret = of_property_read_u32(layout_np, "brcm,slot-size",
				       &slot_size);
	if (ret) {
		dev_err(dev, "%pOF is missing brcm,slot-size\n", layout_np);
		of_node_put(layout_np);
		return ret;
	}

	u32 section_offset;

	ret = of_property_read_u32(layout_np, "brcm,section-offset",
				   &section_offset);
	if (ret) {
		dev_err(dev, "%pOF is missing brcm,section-offset\n",
			layout_np);
		of_node_put(layout_np);
		return ret;
	}

	if (!slot_size || section_offset >= slot_size) {
		of_node_put(layout_np);
		return -EINVAL;
	}

	ret = nonvol_find_latest_section(dev, nvmem, slot_size,
					 section_offset,
					 &section_start,
					 &section_len);
	if (ret) {
		of_node_put(layout_np);
		return ret;
	}

	struct device_node *cell_np;

	for_each_available_child_of_node(layout_np, cell_np) {
		ret = nonvol_add_one_cell(dev, nvmem, cell_np,
					  section_start,
					  section_len);
		if (ret) {
			of_node_put(cell_np);
			of_node_put(layout_np);
			return ret;
		}
	}

	of_node_put(layout_np);

	return 0;
}

static int nonvol_probe(struct nvmem_layout *layout)
{
	layout->add_cells = nonvol_add_cells;

	return nvmem_layout_register(layout);
}

static void nonvol_remove(struct nvmem_layout *layout)
{
	nvmem_layout_unregister(layout);
}

static const struct of_device_id nonvol_of_match_table[] = {
	{ .compatible = "brcm,ecos-nonvol-perm" },
	{}
};
MODULE_DEVICE_TABLE(of, nonvol_of_match_table);

static struct nvmem_layout_driver nonvol_layout = {
	.driver = {
		.name = "ecos-nonvol-perm-layout",
		.of_match_table = nonvol_of_match_table,
	},
	.probe = nonvol_probe,
	.remove = nonvol_remove,
};
module_nvmem_layout_driver(nonvol_layout);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NVMEM layout for Broadcom eCos Permanent NonVol MAC cells");
