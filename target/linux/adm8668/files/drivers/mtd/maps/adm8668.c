/*
 * Copyright (C) 2010 Scott Nicholas <neutronscott@scottn.us>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/magic.h>
#include <asm/io.h>

#define	FLASH_SIZE	0x800000
#define	FLASH_PHYS_ADDR	0x10000000

/* just interested in part of the full struct */
struct squashfs_super_block {
	__le32	s_magic;
	__le32	pad0[9];	/* it's not really padding */
	__le64	bytes_used;
};

static struct mtd_info *mymtd;

static struct map_info nor_map = {
	name:		"adm8668-nor",
	size:		FLASH_SIZE,
	phys:		FLASH_PHYS_ADDR,
	bankwidth:	2,
};

static struct mtd_partition nor_parts[] = {
	{ name: "linux",  offset: 0x40000,  size: FLASH_SIZE - 0x40000, },
	{ name: "rootfs", offset: 0x200000, size: 0x400000, },
	{ name: NULL, },
};

int __init init_mtd_partitions(struct mtd_info *mtd, size_t size)
{
	int blocksize, off;
	size_t len;
	struct squashfs_super_block hdr;

	blocksize = mtd->erasesize;
	if (blocksize < 0x10000)
		blocksize = 0x10000;

	memset(&hdr, 0xe5, sizeof(hdr));
	/* find and size squashfs */
	for (off = (128*1024); off < size; off += blocksize) {
		if (mtd->read(mtd, off, sizeof(hdr), &len, (char *)&hdr) ||
		    len != sizeof(hdr))
			continue;

		if (hdr.s_magic == SQUASHFS_MAGIC) {
			printk(KERN_INFO "%s: Filesystem type: squashfs, off=%#x size=%#x\n",
				mtd->name, off, (unsigned int)hdr.bytes_used);
			/* Update the squashfs start address only! */
			nor_parts[1].offset = off;
			nor_parts[1].size = FLASH_SIZE - off;
			return 2;
		}
	}
	return 2;
}

int __init init_nor(void)
{
	int nr_parts;

	printk(KERN_NOTICE "ADM8668 NOR flash device: %#x at %#x\n",
			FLASH_SIZE, FLASH_PHYS_ADDR);

	nor_map.virt = ioremap(FLASH_PHYS_ADDR,	FLASH_SIZE);
	if (!nor_map.virt) {
		printk("Failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&nor_map);
	mymtd = do_map_probe("cfi_probe", &nor_map);
	if (!mymtd) {
		iounmap((void *)nor_map.virt);
		return -ENXIO;
	}
	mymtd->owner = THIS_MODULE;
	nr_parts = init_mtd_partitions(mymtd, mymtd->size);
	add_mtd_partitions(mymtd, nor_parts, nr_parts);

	return 0;
}

static void __exit cleanup_nor(void)
{
	if (mymtd) {
		del_mtd_partitions(mymtd);
		map_destroy(mymtd);
	}
	if (nor_map.virt) {
		iounmap((void *)nor_map.virt);
		nor_map.virt = 0;
	}
}

module_init(init_nor);
module_exit(cleanup_nor);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Scott Nicholas <neutronscott@scottn.us>");
MODULE_DESCRIPTION("MTD map driver for ADM8668 NOR Flash");
