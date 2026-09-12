// SPDX-License-Identifier: GPL-2.0-only
/*
 * Parser for MikroTik RouterBoot partitions.
 *
 * Copyright (C) 2020 Thibaut VARÈNE <hacks+kernel@slashdirt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This parser builds from the "fixed-partitions" one (see ofpart.c), but it can
 * handle dynamic partitions as found on routerboot devices.
 *
 * DTS nodes are defined as follows:
 * For fixed partitions:
 *	node-name@unit-address {
 *		reg = <prop-encoded-array>;
 *		label = <string>;
 *		read-only;
 *		lock;
 *	};
 *
 * reg property is mandatory; other properties are optional.
 * reg format is <address length>. length can be 0 if the next partition is
 * another fixed partition or a "well-known" partition as defined below: in that
 * case the partition will extend up to the next one.
 *
 * For dynamic partitions:
 *	node-name {
 *		size = <prop-encoded-array>;
 *		label = <string>;
 *		read-only;
 *		lock;
 *	};
 *
 * size property is normally mandatory. It can only be omitted (or set to 0) if:
 *	- the partition is a "well-known" one (as defined below), in which case
 *	  the partition size will be automatically adjusted; or
 *	- the next partition is a fixed one or a "well-known" one, in which case
 *	  the current partition will extend up to the next one.
 * Other properties are optional.
 * size format is <length>.
 * By default dynamic partitions are appended after the preceding one, except
 * for "well-known" ones which are automatically located on flash.
 *
 * Well-known partitions (matched via label or node-name):
 * - "hard_config"
 * - "soft_config"
 * - "dtb_config"
 *
 * Note: this parser will happily register 0-sized partitions if misused.
 *
 * This parser requires the DTS to list partitions in ascending order as
 * expected on the MTD device.
 *
 * Since only the "hard_config" and "soft_config" partitions are used in OpenWRT,
 * a minimal working DTS could define only these two partitions dynamically (in
 * the right order, usually hard_config then soft_config).
 *
 * Note: some mips RB devices encode the hard_config offset and length in two
 * consecutive u32 located at offset 0x14 (for ramips) or 0x24 (for ath79) on
 * the SPI NOR flash. Unfortunately this seems inconsistent across machines and
 * does not apply to e.g. ipq-based ones, so we ignore that information.
 *
 * Note: To find well-known partitions, this parser will go through the entire
 * top mtd partition parsed, _before_ the DTS nodes are processed. This works
 * well in the current state of affairs, and is a simpler implementation than
 * searching for known partitions in the "holes" left between fixed-partition,
 * _after_ processing DTS nodes.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/libfdt_env.h>
#include <linux/string.h>

#define RB_MAGIC_HARD	(('H') | ('a' << 8) | ('r' << 16) | ('d' << 24))
#define RB_MAGIC_SOFT	(('S') | ('o' << 8) | ('f' << 16) | ('t' << 24))
#define RB_BLOCK_SIZE	0x1000

struct routerboot_dynpart {
	const char * const name;
	const u32 magic;
	int (* const size_fixup)(struct mtd_info *, struct routerboot_dynpart *);
	size_t offset;
	size_t size;
	bool found;
};

static int routerboot_dtbsfixup(struct mtd_info *, struct routerboot_dynpart *);
static int routerboot_hardfixup(struct mtd_info *, struct routerboot_dynpart *);

static struct routerboot_dynpart rb_dynparts[] = {
	{
		.name = "hard_config",
		.magic = RB_MAGIC_HARD,	// stored in CPU-endianness on flash
		.size_fixup = routerboot_hardfixup,
		.offset = 0x0,
		.size = RB_BLOCK_SIZE,
		.found = false,
	}, {
		.name = "soft_config",
		.magic = RB_MAGIC_SOFT,	// stored in CPU-endianness on flash
		.size_fixup = NULL,
		.offset = 0x0,
		.size = RB_BLOCK_SIZE,
		.found = false,
	}, {
		.name = "dtb_config",
		.magic = fdt32_to_cpu(OF_DT_HEADER),	// stored BE on flash
		.size_fixup = routerboot_dtbsfixup,
		.offset = 0x0,
		.size = 0x0,
		.found = false,
	}
};

/*
 * hard_config is not guaranteed to fit within RB_BLOCK_SIZE. On the MikroTik
 * Chateau 5G R17 ax the tag list spans 5576 bytes, because the WLAN calibration
 * tag alone carries 5100 bytes of LZ77-compressed data. Truncating the partition
 * to RB_BLOCK_SIZE makes that tag unreadable ("Out of bounds field").
 *
 * Walk the tag list to its terminating null node and size the partition to match.
 * Tag nodes are u32: low 16 bits are the id, high 16 bits the payload length;
 * payloads are 32bit-aligned. The result is rounded up to RB_BLOCK_SIZE and never
 * shrinks below it, so devices whose tag list already fits are unaffected.
 */
static int routerboot_hardfixup(struct mtd_info *master, struct routerboot_dynpart *rbdpart)
{
	size_t bytes_read, pos;
	u32 node;
	int err;

	pos = rbdpart->offset + sizeof(node);	/* skip magic */

	while (pos < master->size) {
		err = mtd_read(master, pos, sizeof(node), &bytes_read, (u8 *)&node);
		if (err)
			return err;

		if (bytes_read != sizeof(node))
			return -EIO;

		if (!node)	/* tag list ends with a null node */
			break;

		pos += sizeof(node) + ALIGN((size_t)(node >> 16), sizeof(node));
	}

	if (pos >= master->size)
		return -EINVAL;

	pos += sizeof(node);	/* account for the null node itself */

	rbdpart->size = max_t(size_t, ALIGN(pos - rbdpart->offset, RB_BLOCK_SIZE),
			      RB_BLOCK_SIZE);
	return 0;
}

static int routerboot_dtbsfixup(struct mtd_info *master, struct routerboot_dynpart *rbdpart)
{
	int err;
	size_t bytes_read, psize;
	struct {
		fdt32_t magic;
		fdt32_t totalsize;
		fdt32_t off_dt_struct;
		fdt32_t off_dt_strings;
		fdt32_t off_mem_rsvmap;
		fdt32_t version;
		fdt32_t last_comp_version;
		fdt32_t boot_cpuid_phys;
		fdt32_t size_dt_strings;
		fdt32_t size_dt_struct;
	} fdt_header;

	err = mtd_read(master, rbdpart->offset, sizeof(fdt_header),
		       &bytes_read, (u8 *)&fdt_header);
	if (err)
		return err;

	if (bytes_read != sizeof(fdt_header))
		return -EIO;

	psize = fdt32_to_cpu(fdt_header.totalsize);
	if (!psize)
		return -EINVAL;

	rbdpart->size = psize;
	return 0;
}

static void routerboot_find_dynparts(struct mtd_info *master)
{
	size_t bytes_read, offset;
	bool allfound;
	int err, i;
	u32 buf;

	/*
	 * Dynamic RouterBoot partitions offsets are aligned to RB_BLOCK_SIZE:
	 * read the whole partition at RB_BLOCK_SIZE intervals to find sigs.
	 * Skip partition content when possible.
	 */
	offset = 0;
	while (offset < master->size) {
		err = mtd_read(master, offset, sizeof(buf), &bytes_read, (u8 *)&buf);
		if (err) {
			pr_err("%s: mtd_read error while parsing (offset: 0x%zX): %d\n",
			       master->name, offset, err);
			continue;
		}

		allfound = true;

		for (i = 0; i < ARRAY_SIZE(rb_dynparts); i++) {
			if (rb_dynparts[i].found)
				continue;

			allfound = false;

			if (rb_dynparts[i].magic == buf) {
				rb_dynparts[i].offset = offset;

				if (rb_dynparts[i].size_fixup) {
					err = rb_dynparts[i].size_fixup(master, &rb_dynparts[i]);
					if (err) {
						pr_err("%s: size fixup error while parsing \"%s\": %d\n",
						       master->name, rb_dynparts[i].name, err);
						continue;
					}
				}

				rb_dynparts[i].found = true;

				/*
				 * move offset to skip the whole partition on
				 * next iteration if size > RB_BLOCK_SIZE.
				 */
				if (rb_dynparts[i].size > RB_BLOCK_SIZE)
					offset += ALIGN_DOWN((rb_dynparts[i].size - RB_BLOCK_SIZE), RB_BLOCK_SIZE);

				break;
			}
		}

		offset += RB_BLOCK_SIZE;

		if (allfound)
			break;
	}
}

static int routerboot_partitions_parse(struct mtd_info *master,
				       const struct mtd_partition **pparts,
				       struct mtd_part_parser_data *data)
{
	struct device_node *rbpart_node, *pp;
	struct mtd_partition *parts;
	const char *partname;
	size_t master_ofs;
	int np;

	/* Pull of_node from the master device node */
	rbpart_node = mtd_get_of_node(master);
	if (!rbpart_node)
		return 0;

	/* First count the subnodes */
	np = 0;
	for_each_child_of_node_scoped(rbpart_node, pp)
		np++;

	if (!np)
		return 0;

	parts = kcalloc(np, sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	/* Preemptively look for known parts in flash */
	routerboot_find_dynparts(master);

	np = 0;
	master_ofs = 0;
	for_each_child_of_node_scoped(rbpart_node, pp) {
		const __be32 *reg, *sz;
		size_t offset, size;
		int i, len, a_cells, s_cells;

		partname = of_get_property(pp, "label", &len);
		/* Allow deprecated use of "name" instead of "label" */
		if (!partname)
			partname = of_get_property(pp, "name", &len);
		/* Fallback to node name per spec if all else fails: partname is always set */
		if (!partname)
			partname = pp->name;
		parts[np].name = partname;

		reg = of_get_property(pp, "reg", &len);
		if (reg) {
			/* Fixed partition */
			a_cells = of_n_addr_cells(pp);
			s_cells = of_n_size_cells(pp);

			if ((len / 4) != (a_cells + s_cells)) {
				pr_debug("%s: routerboot partition %pOF (%pOF) error parsing reg property.\n",
					 master->name, pp, rbpart_node);
				goto rbpart_fail;
			}

			offset = of_read_number(reg, a_cells);
			size = of_read_number(reg + a_cells, s_cells);
		} else {
			/* Dynamic partition */
			/* Default: part starts at current offset, 0 size */
			offset = master_ofs;
			size = 0;

			/* Check if well-known partition */
			for (i = 0; i < ARRAY_SIZE(rb_dynparts); i++) {
				if (!strcmp(partname, rb_dynparts[i].name) && rb_dynparts[i].found) {
					offset = rb_dynparts[i].offset;
					size = rb_dynparts[i].size;
					break;
				}
			}

			/* Standalone 'size' property? Override size */
			sz = of_get_property(pp, "size", &len);
			if (sz) {
				s_cells = of_n_size_cells(pp);
				if ((len / 4) != s_cells) {
					pr_debug("%s: routerboot partition %pOF (%pOF) error parsing size property.\n",
						 master->name, pp, rbpart_node);
					goto rbpart_fail;
				}

				size = of_read_number(sz, s_cells);
			}
		}

		if (np > 0) {
			/* Minor sanity check for overlaps */
			if (offset < (parts[np-1].offset + parts[np-1].size)) {
				pr_err("%s: routerboot partition %pOF (%pOF) \"%s\" overlaps with previous partition \"%s\".\n",
				       master->name, pp, rbpart_node,
				       partname, parts[np-1].name);
				goto rbpart_fail;
			}

			/* Fixup end of previous partition if necessary */
			if (!parts[np-1].size)
				parts[np-1].size = (offset - parts[np-1].offset);
		}

		if ((offset + size) > master->size) {
			pr_err("%s: routerboot partition %pOF (%pOF) \"%s\" extends past end of segment.\n",
			       master->name, pp, rbpart_node, partname);
			goto rbpart_fail;
		}

		parts[np].offset = offset;
		parts[np].size = size;
		parts[np].of_node = pp;

		if (of_get_property(pp, "read-only", &len))
			parts[np].mask_flags |= MTD_WRITEABLE;

		if (of_get_property(pp, "lock", &len))
			parts[np].mask_flags |= MTD_POWERUP_LOCK;

		/* Keep master offset aligned to RB_BLOCK_SIZE */
		master_ofs = ALIGN(offset + size, RB_BLOCK_SIZE);
		np++;
	}

	/*
	 * MTD refuses writes to a partition that does not end on an erase block
	 * boundary. soft_config is RB_BLOCK_SIZE (4 KiB) while NOR erase blocks
	 * are typically 64 KiB, so it would always end up read-only - even
	 * though it is the one RouterBoot segment meant to be written, as it
	 * carries the boot settings.
	 *
	 * Grow it to a full erase block where the space is actually free, so
	 * that erasing it cannot destroy a neighbouring partition. The config
	 * data itself keeps its original size; see rb_softconfig.c, which
	 * erases the block but reads, checksums and writes only RB_BLOCK_SIZE.
	 */
	for (int idx = 0; idx < np; idx++) {
		size_t grown, limit;

		if (strcmp(parts[idx].name, "soft_config"))
			continue;

		if (!master->erasesize || parts[idx].size >= master->erasesize)
			break;

		if (parts[idx].offset % master->erasesize)
			break;	/* not block aligned to begin with */

		grown = ALIGN(parts[idx].size, master->erasesize);
		limit = (idx + 1 < np) ? parts[idx + 1].offset : master->size;

		if (parts[idx].offset + grown > limit)
			break;	/* would overlap the next partition */

		pr_info("%s: growing \"soft_config\" from 0x%llX to 0x%llX to make it writable\n",
			master->name, (unsigned long long)parts[idx].size,
			(unsigned long long)grown);
		parts[idx].size = grown;
		break;
	}

	*pparts = parts;
	return np;

rbpart_fail:
	pr_err("%s: error parsing routerboot partition %pOF (%pOF)\n",
	       master->name, pp, rbpart_node);
	kfree(parts);
	return -EINVAL;
}

static const struct of_device_id parse_routerbootpart_match_table[] = {
	{ .compatible = "mikrotik,routerboot-partitions" },
	{},
};
MODULE_DEVICE_TABLE(of, parse_routerbootpart_match_table);

static struct mtd_part_parser routerbootpart_parser = {
	.parse_fn = routerboot_partitions_parse,
	.name = "routerbootpart",
	.of_match_table = parse_routerbootpart_match_table,
};
module_mtd_part_parser(routerbootpart_parser);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("MTD partitioning for RouterBoot");
MODULE_AUTHOR("Thibaut VARENE");
