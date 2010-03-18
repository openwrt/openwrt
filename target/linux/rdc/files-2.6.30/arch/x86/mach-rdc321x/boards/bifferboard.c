/*
 *  Bifferboard RDC321x platform devices
 *
 *  Copyright (C) 2010 bifferos@yahoo.co.uk 
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

static int __init parse_bifferboard_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long plat_data)
{
	int res;
	size_t len;
	struct mtd_partition *rdc_flash_parts;
	u32 kernel_len;
	u16 tmp;

	if (master->size == 0x100000)
		kernel_len = master->size - 0x10000;
	else {
		res =  master->read(master, 0x4000 + 1036, 2, &len, (char *) &tmp);
		if (res)
			return res;
		kernel_len = tmp * master->erasesize;
	}
	
	rdc_flash_parts = kzalloc(sizeof(struct mtd_partition) * 4, GFP_KERNEL);
	
	*pparts = rdc_flash_parts;

	rdc_flash_parts[0].name = "biffboot";
	rdc_flash_parts[0].offset = master->size - 0x10000;
	rdc_flash_parts[0].size = 0x10000;
	rdc_flash_parts[0].mask_flags = MTD_WRITEABLE;
	rdc_flash_parts[1].name = "firmware";
	rdc_flash_parts[1].offset = 0;
	rdc_flash_parts[1].size = rdc_flash_parts[0].offset;
	rdc_flash_parts[2].name = "kernel";
	rdc_flash_parts[2].offset = 0x00000000;
	rdc_flash_parts[2].size = kernel_len;
	
	if (master->size == 0x100000)
		return 2;

	rdc_flash_parts[3].name = "rootfs";
	rdc_flash_parts[3].offset = MTDPART_OFS_APPEND;
	rdc_flash_parts[3].size = rdc_flash_parts[1].size - rdc_flash_parts[2].size;

	return 4;
}

struct mtd_part_parser __initdata bifferboard_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_bifferboard_partitions,
	.name = "Bifferboard",
};

static int __init bifferboard_setup(void)
{
	return register_mtd_parser(&bifferboard_parser);
}

arch_initcall(bifferboard_setup);
