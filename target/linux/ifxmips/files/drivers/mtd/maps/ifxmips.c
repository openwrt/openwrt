/*
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
 * Copyright (C) 2008 John Crispin <blogic@openwrt.org>
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
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_prom.h>
#include <asm/ifxmips/ifxmips_ebu.h>
#include <linux/magic.h>
#include <linux/platform_device.h>

static struct map_info
ifxmips_map = {
	.name = "ifxmips_mtd",
	.bankwidth = 2,
	.size = 0x400000,
};

static map_word
ifxmips_read16(struct map_info * map, unsigned long adr)
{
	unsigned long flags;
	map_word temp;
	spin_lock_irqsave(&ebu_lock, flags);
	adr ^= 2;
	temp.x[0] = *((__u16*)(map->virt + adr));
	spin_unlock_irqrestore(&ebu_lock, flags);
	return temp;
}

static void
ifxmips_write16(struct map_info *map, map_word d, unsigned long adr)
{
	unsigned long flags;
	spin_lock_irqsave(&ebu_lock, flags);
	adr ^= 2;
	*((__u16*)(map->virt + adr)) = d.x[0];
	spin_unlock_irqrestore(&ebu_lock, flags);
}

void
ifxmips_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	unsigned char *p;
	unsigned char *to_8;
	unsigned long flags;
	spin_lock_irqsave(&ebu_lock, flags);
	from = (unsigned long)(from + map->virt);
	p = (unsigned char*) from;
	to_8 = (unsigned char*) to;
	while(len--)
		*to_8++ = *p++;
	spin_unlock_irqrestore(&ebu_lock, flags);
}

void
ifxmips_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	unsigned char *p =  (unsigned char*)from;
	unsigned char *to_8;
	unsigned long flags;
	spin_lock_irqsave(&ebu_lock, flags);
	to += (unsigned long) map->virt;
	to_8 = (unsigned char*)to;
	while(len--)
		*p++ = *to_8++;
	spin_unlock_irqrestore(&ebu_lock, flags);
}

static struct mtd_partition
ifxmips_partitions[] = {
	{
		name:"uboot",
		offset:0x00000000,
		size:0x00020000,
	},
	{
		name:"uboot_env",
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
	{
		name:"board_config",
		offset:0x0,
		size:0x0,
	},
};

int
find_uImage_size(unsigned long start_offset){
	unsigned long temp;
	ifxmips_copy_from(&ifxmips_map, &temp, start_offset + 12, 4);
	printk(KERN_INFO "ifxmips_mtd: kernel size is %ld \n", temp + 0x40);
	return temp + 0x40;
}

int
find_brn_block(unsigned long start_offset){
	unsigned char temp[9];
	ifxmips_copy_from(&ifxmips_map, &temp, start_offset, 8);
	temp[8] = '\0';
	printk(KERN_INFO "data in brn block %s\n", temp);
	if(memcmp(temp, "BRN-BOOT", 8) == 0)
		return 1;
	else
		return 0;
}

int
detect_squashfs_partition(unsigned long start_offset){
	unsigned long temp;
	ifxmips_copy_from(&ifxmips_map, &temp, start_offset, 4);
	return (temp == SQUASHFS_MAGIC);
}

static int
ifxmips_mtd_probe(struct platform_device *dev)
{
	struct mtd_info *ifxmips_mtd = NULL;
	struct mtd_partition *parts = NULL;
	unsigned long uimage_size;

	ifxmips_w32(0x1d7ff, IFXMIPS_EBU_BUSCON0);

	ifxmips_map.read = ifxmips_read16;
	ifxmips_map.write = ifxmips_write16;
	ifxmips_map.copy_from = ifxmips_copy_from;
	ifxmips_map.copy_to = ifxmips_copy_to;
	ifxmips_map.phys = dev->resource->start;
	ifxmips_map.size = dev->resource->end - ifxmips_map.phys + 1;
	ifxmips_map.virt = ioremap_nocache(ifxmips_map.phys, ifxmips_map.size);

	if(!ifxmips_map.virt)
	{
		printk(KERN_WARNING "ifxmips_mtd: failed to ioremap!\n");
		return -EIO;
	}

	ifxmips_mtd = (struct mtd_info *) do_map_probe("cfi_probe", &ifxmips_map);
	if(!ifxmips_mtd)
	{
		iounmap(ifxmips_map.virt);
		printk(KERN_WARNING "ifxmips_mtd: probing failed\n");
		return -ENXIO;
	}

	ifxmips_mtd->owner = THIS_MODULE;
	uimage_size = find_uImage_size(ifxmips_partitions[2].offset);

	if(detect_squashfs_partition(ifxmips_partitions[2].offset + uimage_size))
	{
		printk(KERN_INFO "ifxmips_mtd: found a squashfs following the uImage\n");
	} else {
		uimage_size &= ~0xffff;
		uimage_size += 0x10000;
	}

	parts = &ifxmips_partitions[0];
	ifxmips_partitions[2].size = uimage_size;
	ifxmips_partitions[3].offset = ifxmips_partitions[2].offset + ifxmips_partitions[2].size;
	ifxmips_partitions[3].size = ((ifxmips_mtd->size >> 20) * 1024 * 1024) - ifxmips_partitions[3].offset;
	if(ifxmips_has_brn_block())
	{
		ifxmips_partitions[3].size -= ifxmips_mtd->erasesize;
		ifxmips_partitions[4].offset = ifxmips_mtd->size - ifxmips_mtd->erasesize;
		ifxmips_partitions[4].size = ifxmips_mtd->erasesize;
		add_mtd_partitions(ifxmips_mtd, parts, 5);
	} else {
		add_mtd_partitions(ifxmips_mtd, parts, 4);
	}

	printk(KERN_INFO "ifxmips_mtd: added ifxmips flash with %dMB\n", ifxmips_mtd->size >> 20);
	return 0;
}

static struct
platform_driver ifxmips_mtd_driver = {
	.probe = ifxmips_mtd_probe,
	.driver = {
		.name = "ifxmips_mtd",
		.owner = THIS_MODULE,
	},
};

int __init
init_ifxmips_mtd(void)
{
	int ret = platform_driver_register(&ifxmips_mtd_driver);
	if(ret)
		printk(KERN_INFO "ifxmips_mtd: error registering platfom driver!");
	return ret;
}

static void __exit
cleanup_ifxmips_mtd(void)
{
	platform_driver_unregister(&ifxmips_mtd_driver);
}

module_init(init_ifxmips_mtd);
module_exit(cleanup_ifxmips_mtd);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("John Crispin <blogic@openwrt.org>");
MODULE_DESCRIPTION("MTD map driver for IFXMIPS boards");
