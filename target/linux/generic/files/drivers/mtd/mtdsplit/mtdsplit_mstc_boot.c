// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * a mtdsplit parser using "bootnum" value in the "persist" partition
 * for the devices manufactured by MSTC (MitraStar Technology Corp.)
 */
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/types.h>
#include <linux/byteorder/generic.h>
#include <linux/slab.h>
#include <linux/libfdt.h>
#include <linux/of_fdt.h>
#include <dt-bindings/mtd/partitions/uimage.h>

#include "mtdsplit.h"

#define PERSIST_BOOTNUM_OFFSET	0x4
#define NR_PARTS_MAX		2

/*
 * Legacy format image header,
 * all data in network byte order (aka natural aka bigendian).
 */
 struct uimage_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
};

/* check whether the current mtd device is active or not */
static int
mstcboot_is_active(struct mtd_info *mtd, u32 *bootnum_dt)
{
	struct device_node *np = mtd_get_of_node(mtd);
	struct device_node *persist_np;
	size_t retlen;
	u32 persist_offset;
	u_char bootnum;
	int ret;

	ret = of_property_read_u32(np, "mstc,bootnum", bootnum_dt);
	if (ret)
		return ret;

	persist_np = of_parse_phandle(np, "mstc,persist", 0);
	if (!persist_np)
		return -ENODATA;
	/* is "persist" under the same node? */
	if (persist_np->parent != np->parent)
		return -EINVAL;
	ret = of_property_read_u32(persist_np, "reg", &persist_offset);
	of_node_put(persist_np);
	if (ret)
		return ret;
	ret = mtd_read(mtd->parent, persist_offset + PERSIST_BOOTNUM_OFFSET,
		       1, &retlen, &bootnum);
	if (ret)
		return ret;
	if (retlen != 1)
		return -EIO;

	switch (bootnum) {
	case 0 ... 2:
		return (bootnum == *bootnum_dt) ? 1 : 0;
	default:
		pr_err("invalid bootnum detected within persist! (0x%x)\n",
		       bootnum);
		return -EINVAL;
	}
}

/*
 * mainly for NOR devices that uses raw kernel and squashfs
 *
 * example:
 *
 * partition@5a0000 {
 * 	compatible = "mstc,boot";
 * 	label = "firmware1";
 * 	reg = <0x5a0000 0x3200000>;
 * 	mstc,bootnum = <1>;
 * 	mstc,persist = <&mtd_persist>;
 * };
 */
static int
mstcboot_parse_image_parts(struct mtd_info *mtd,
			   const struct mtd_partition **pparts)
{
	struct mtd_partition *parts;
	size_t retlen, kern_len = 0;
	size_t rootfs_offset;
	enum mtdsplit_part_type type;
	u_char buf[0x40];
	int ret, nr_parts = 1, index = 0;

	ret = mtd_read(mtd, 0, sizeof(struct uimage_header), &retlen, buf);
	if (ret)
		return ret;
	if (retlen != sizeof(struct uimage_header))
		return -EIO;

	if (be32_to_cpu(*(u32 *)buf) == OF_DT_HEADER) {
		/* Flattened Image Tree (FIT) */
		struct fdt_header *fdthdr = (void *)buf;
		kern_len = be32_to_cpu(fdthdr->totalsize);
	} else if (be32_to_cpu(*(u32 *)buf) == IH_MAGIC) {
		/* Legacy uImage */
		struct uimage_header *uimghdr = (void *)buf;
		kern_len = sizeof(*uimghdr) + be32_to_cpu(uimghdr->ih_size);
	}

	ret = mtd_find_rootfs_from(mtd, kern_len, mtd->size, &rootfs_offset, &type);
	if (ret) {
		pr_debug("no rootfs in \"%s\"\n", mtd->name);
		return ret;
	}

	if (kern_len > 0)
		nr_parts++;

	parts = kcalloc(nr_parts, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	if (kern_len) {
		parts[index].name = KERNEL_PART_NAME;
		parts[index].offset = 0;
		parts[index++].size = rootfs_offset;
	}

	parts[index].name = (type == MTDSPLIT_PART_TYPE_UBI)
				? UBI_PART_NAME : ROOTFS_PART_NAME;
	parts[index].offset = rootfs_offset;
	parts[index].size = mtd->size - rootfs_offset;

	*pparts = parts;
	return nr_parts;
}

/*
 * mainly for NAND devices that uses raw-kernel and UBI and needs
 * splitted kernel/ubi partitions when sysupgrade
 *
 * example:
 *
 * partition@3c0000 {
 * 	compatible = "mstc,boot";
 * 	reg = <0x3c0000 0x3240000>;
 * 	label = "firmware1";
 * 	mstc,bootnum = <1>;
 * 	mstc,persist = <&mtd_persist>;
 * 	#address-cells = <1>;
 * 	#size-cells = <1>;
 *
 * 	partition@0 {
 * 		reg = <0x0 0x800000>;
 * 		label-base = "kernel";
 * 	};
 *
 * 	partition@800000 {
 * 		reg = <0x800000 0x2a40000>;
 * 		label-base = "ubi";
 * 	};
};
 */
static int
mstcboot_parse_fixed_parts(struct mtd_info *mtd,
			   const struct mtd_partition **pparts,
			   int active, u32 bootnum_dt)
{
	struct device_node *np = mtd_get_of_node(mtd);
	struct device_node *child;
	struct mtd_partition *parts;
	int ret, nr_parts, index = 0;

	nr_parts = of_get_child_count(np);
	if (nr_parts > NR_PARTS_MAX) {
		pr_err("too many partitions found!\n");
		return -EINVAL;
	}

	parts = kcalloc(nr_parts, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	for_each_child_of_node(np, child) {
		u32 reg[2];
		if (of_n_addr_cells(child) != 1 ||
		    of_n_size_cells(child) != 1)
		{
			ret = -EINVAL;
			break;
		}

		ret = of_property_read_u32_array(child, "reg", reg, 2);
		if (ret)
			break;
		ret = of_property_read_string(child, "label-base",
					      &parts[index].name);
		if (ret)
			break;

		if (!active) {
			parts[index].name = devm_kasprintf(&mtd->dev, GFP_KERNEL,
						"%s%u",
						parts[index].name, bootnum_dt);
			if (!parts[index].name) {
				ret = -ENOMEM;
				break;
			}
		}
		parts[index].offset = reg[0];
		parts[index].size = reg[1];
		index++;
	}
	of_node_put(child);

	if (ret)
		kfree(parts);
	else
		*pparts = parts;
	return ret ? ret : nr_parts;
}

static int
mtdsplit_mstcboot_parse(struct mtd_info *mtd,
			const struct mtd_partition **pparts,
			struct mtd_part_parser_data *data)
{
	struct device_node *np = mtd_get_of_node(mtd);
	u32 bootnum_dt;
	int ret;

	ret = mstcboot_is_active(mtd, &bootnum_dt);
	if (ret < 0)
		return ret;

	if (of_get_child_count(np))
		ret = mstcboot_parse_fixed_parts(mtd, pparts, ret, bootnum_dt);
	else if (ret != 0)
		ret = mstcboot_parse_image_parts(mtd, pparts);
	else
		return -ENODEV;

	return ret;
}

static const struct of_device_id mtdsplit_mstcboot_of_match_table[] = {
	{ .compatible = "mstc,boot" },
	{},
};
MODULE_DEVICE_TABLE(of, mtdsplit_mstcboot_of_match_table);

static struct mtd_part_parser mtdsplit_mstcboot_parser = {
	.owner = THIS_MODULE,
	.name = "mstc-boot",
	.of_match_table = mtdsplit_mstcboot_of_match_table,
	.parse_fn = mtdsplit_mstcboot_parse,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};
module_mtd_part_parser(mtdsplit_mstcboot_parser)
