/*
 *  R8610 RDC321x platform devices
 *
 *  Copyright (C) 2009, Florian Fainelli 
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/init.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>

#include <asm/rdc_boards.h>

static int __init parse_r8610_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long plat_data)
{
	struct mtd_partition *rdc_flash_parts;
	
	rdc_flash_parts = kzalloc(sizeof(struct mtd_partition) * 4, GFP_KERNEL);
	
	*pparts = rdc_flash_parts;
	
	rdc_flash_parts[0].name = "kernel";
	rdc_flash_parts[0].size = 0x001f0000;
	rdc_flash_parts[0].offset = 0;
	rdc_flash_parts[1].name = "config";
	rdc_flash_parts[1].size = 0x10000;
	rdc_flash_parts[1].offset = MTDPART_OFS_APPEND;
	rdc_flash_parts[2].name = "rootfs";
	rdc_flash_parts[2].size = 0x1E0000; 
	rdc_flash_parts[2].offset = MTDPART_OFS_APPEND;
	rdc_flash_parts[3].name = "redboot";
	rdc_flash_parts[3].size = 0x20000;
	rdc_flash_parts[3].offset = MTDPART_OFS_APPEND;
	rdc_flash_parts[3].mask_flags = MTD_WRITEABLE;

	return 4;
}

struct mtd_part_parser __initdata r8610_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_r8610_partitions,
	.name = "R8610",
};

static int __init r8610_setup(void)
{
	return register_mtd_parser(&r8610_parser);
}

arch_initcall(r8610_setup);
