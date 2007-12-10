/*
 *  Driver for DANUBE flashmap 
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright (C) 2004 Liu Peng Infineon IFAP DC COM CPE
 * Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>

#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/cfi.h>
#include <asm/danube/danube.h>
#include <linux/magic.h>

static struct map_info
danube_map = {
	.name = "DANUBE_FLASH",
	.bankwidth = 2,
	.size = 0x400000,
};

static map_word
danube_read16 (struct map_info * map, unsigned long adr)
{
	map_word temp;

	adr ^= 2;
	temp.x[0] = *((__u16 *) (map->virt + adr));

	return temp;
}

static void
danube_write16 (struct map_info *map, map_word d, unsigned long adr)
{
	adr ^= 2;
	*((__u16 *) (map->virt + adr)) = d.x[0];
}

void
danube_copy_from (struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	u8 *p;
	u8 *to_8;

	from = (unsigned long) (from + map->virt);
	p = (u8 *) from;
	to_8 = (u8 *) to;
	while(len--){
		*to_8++ = *p++;
	}
}

void
danube_copy_to (struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	u8 *p =  (u8*) from;
	u8 *to_8;

	to += (unsigned long) map->virt;
	to_8 = (u8*)to;
	while(len--){
		*p++ = *to_8++;
	}
}

static struct mtd_partition
danube_partitions[4] = {
	{
		name:"U-Boot",
		offset:0x00000000,
		size:0x00020000,
	},
	{
		name:"U-Boot-Env",
		offset:0x00020000,
		size:0x00010000,
	},
	{
		name:"kernel",
		offset:0x00030000,
		size:0x0,
	},
	{
		name:"rootfs",
		offset:0x0,
		size:0x0,
	},
};

#define DANUBE_FLASH_START		0x10000000
#define DANUBE_FLASH_MAX		0x2000000

int
find_uImage_size (unsigned long start_offset){
	unsigned long temp;

	danube_copy_from(&danube_map, &temp, start_offset + 12, 4);
	printk("kernel size is %ld \n", temp + 0x40);
	return temp + 0x40;
}

int
detect_squashfs_partition (unsigned long start_offset){
	unsigned long temp;

	danube_copy_from(&danube_map, &temp, start_offset, 4);

	return (temp == SQUASHFS_MAGIC);
}

int __init
init_danube_mtd (void)
{
	struct mtd_info *danube_mtd = NULL;
	struct mtd_partition *parts = NULL;
	unsigned long uimage_size;

	writel(0x1d7ff, DANUBE_EBU_BUSCON0);

	danube_map.read = danube_read16;
	danube_map.write = danube_write16;
	danube_map.copy_from = danube_copy_from;
	danube_map.copy_to = danube_copy_to;

	danube_map.phys = DANUBE_FLASH_START;
	danube_map.virt = ioremap_nocache(DANUBE_FLASH_START, DANUBE_FLASH_MAX);
	danube_map.size = DANUBE_FLASH_MAX;
	if (!danube_map.virt) {
		printk(KERN_WARNING "Failed to ioremap!\n");
		return -EIO;
	}

	danube_mtd = (struct mtd_info *) do_map_probe("cfi_probe", &danube_map);
	if (!danube_mtd) {
		iounmap(danube_map.virt);
		printk("probing failed\n");
		return -ENXIO;
	}

	danube_mtd->owner = THIS_MODULE;

	uimage_size = find_uImage_size(danube_partitions[2].offset);

	if(detect_squashfs_partition(danube_partitions[2].offset + uimage_size)){
		printk("Found a squashfs following the uImage\n");
	} else {
		uimage_size &= ~0xffff;
		uimage_size += 0x10000;
	}

	danube_partitions[2].size = uimage_size;
	danube_partitions[3].offset = danube_partitions[2].offset + danube_partitions[2].size;
	danube_partitions[3].size = ((danube_mtd->size >> 20) * 1024 * 1024) - danube_partitions[3].offset;

	parts = &danube_partitions[0];
	add_mtd_partitions(danube_mtd, parts, 4);

	printk("Added danube flash with %dMB\n", danube_mtd->size >> 20);
	return 0;
}

static void
__exit
cleanup_danube_mtd (void)
{
}

module_init (init_danube_mtd);
module_exit (cleanup_danube_mtd);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION ("MTD map driver for DANUBE boards");
