// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * econet trx firmware partition parser with dual-boot support.
 *
 * econet devices use a dual-boot firmware layout with two tclinux (TRX)
 * partitions. Only the active tclinux partition gets split into
 * kernel + rootfs.
 *
 * Each tclinux partition may carry an econet,boot-flag-value property.
 * A boot flag byte is read from one of the following sources (in order):
 *
 * econet,boot-flag phandle + offset on the partitions node. The target
 * node type is detected automatically:
 *
 * - Reserved memory (no-map) - read via memremap. The bootloader stores
 *   the booted kernel index at a known RAM address.
 *
 * - Flash partition - read via mtd_read. For devices whose bootloader
 *   does not set the RAM flag.
 *
 * The boot flag byte is compared against each partition's
 * econet,boot-flag-value. A match means that partition is active.
 *
 * If no boot flag source is defined, partitions without
 * econet,boot-flag-value are always split (default active).
 */

#include <linux/io.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>
#include <linux/unaligned.h>

#include "mtdsplit.h"

#define ECONET_TRX_HDRLEN	256
#define ECONET_TRX_KLEN_OFF	80		/* kernel_len offset in header */

#define NR_PARTS		2

static int econet_trx_read_boot_flag(struct device_node *pp,
				     struct mtd_info *mtd)
{
	struct of_phandle_args args;
	struct mtd_info *flag_mtd;
	struct resource res;
	const char *label;
	u32 offset, part_base;
	size_t retlen;
	void *addr;
	u8 byte;
	int ret;

	ret = of_parse_phandle_with_fixed_args(pp, "econet,boot-flag", 1, 0,
					       &args);
	if (ret)
		return ret;

	offset = args.args[0];

	if (of_property_read_bool(args.np, "no-map")) {
		/* Reserved memory: read via memremap */
		if (of_address_to_resource(args.np, 0, &res)) {
			of_node_put(args.np);
			return -EINVAL;
		}
		of_node_put(args.np);

		if (offset >= resource_size(&res))
			return -EINVAL;

		addr = memremap(res.start, resource_size(&res), MEMREMAP_WB);
		if (!addr)
			return -ENOMEM;

		byte = ((u8 *)addr)[offset];
		memunmap(addr);
		return byte;
	}

	/* Flash partition: try MTD by name, fall back to parent + reg */
	if (!of_property_read_string(args.np, "label", &label)) {
		flag_mtd = get_mtd_device_nm(label);
		if (!IS_ERR(flag_mtd)) {
			of_node_put(args.np);
			ret = mtd_read(flag_mtd, offset, 1, &retlen, &byte);
			put_mtd_device(flag_mtd);
			if (ret || retlen != 1)
				return -EIO;
			return byte;
		}
	}

	if (of_property_read_u32_index(args.np, "reg", 0, &part_base)) {
		of_node_put(args.np);
		return -EINVAL;
	}
	of_node_put(args.np);

	if (!mtd->parent)
		return -ENODEV;

	if (mtd_read(mtd->parent, part_base + offset, 1, &retlen, &byte) ||
	    retlen != 1)
		return -EIO;

	return byte;
}

static int mtdsplit_parse_econet_trx(struct mtd_info *mtd,
				     const struct mtd_partition **pparts,
				     struct mtd_part_parser_data *data)
{
	struct device_node *np = mtd_get_of_node(mtd);
	struct device_node *parent_np;
	enum mtdsplit_part_type type;
	struct mtd_partition *parts;
	u8 hdr[ECONET_TRX_HDRLEN];
	u32 flag_value;
	u32 kernel_len;
	size_t rootfs_off;
	size_t retlen;
	int boot_flag;
	int ret;

	if (!np)
		return 0;

	parent_np = of_get_parent(np);
	if (!parent_np)
		return 0;

	boot_flag = econet_trx_read_boot_flag(parent_np, mtd);
	of_node_put(parent_np);

	if (!of_property_read_u32(np, "econet,boot-flag-value", &flag_value)) {
		if (boot_flag < 0) {
			pr_info("econet-trx: %s skipped (no boot flag source)\n",
				mtd->name);
			return 0;
		}
		if ((u8)boot_flag != (u8)flag_value) {
			pr_info("econet-trx: %s is inactive (flag 0x%02x)\n",
				mtd->name, (u8)boot_flag);
			return 0;
		}
		pr_info("econet-trx: %s is active (flag 0x%02x)\n",
			mtd->name, (u8)boot_flag);
	}

	ret = mtd_read(mtd, 0, sizeof(hdr), &retlen, hdr);
	if (ret || retlen != sizeof(hdr))
		return -EIO;

	if (!memcmp(hdr, "HDR2", 4))
		kernel_len = get_unaligned_le32(hdr + ECONET_TRX_KLEN_OFF);
	else if (!memcmp(hdr, "2RDH", 4))
		kernel_len = get_unaligned_be32(hdr + ECONET_TRX_KLEN_OFF);
	else
		return -EINVAL;

	ret = mtd_find_rootfs_from(mtd, ECONET_TRX_HDRLEN + kernel_len,
				  mtd->size, &rootfs_off, &type);
	if (ret)
		return ret;

	parts = kcalloc(NR_PARTS, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = rootfs_off;

	if (type == MTDSPLIT_PART_TYPE_UBI)
		parts[1].name = UBI_PART_NAME;
	else
		parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_off;
	parts[1].size = mtd->size - rootfs_off;

	*pparts = parts;
	return NR_PARTS;
}

static const struct of_device_id mtdsplit_econet_trx_of_match[] = {
	{ .compatible = "econet,trx" },
	{},
};
MODULE_DEVICE_TABLE(of, mtdsplit_econet_trx_of_match);

static struct mtd_part_parser mtdsplit_econet_trx_parser = {
	.owner = THIS_MODULE,
	.name = "econet-trx",
	.of_match_table = mtdsplit_econet_trx_of_match,
	.parse_fn = mtdsplit_parse_econet_trx,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};
module_mtd_part_parser(mtdsplit_econet_trx_parser);
