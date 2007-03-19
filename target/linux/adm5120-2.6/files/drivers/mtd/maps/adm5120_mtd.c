/*
 *	Flash device on the ADM5120 board
 *
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License version
 *	2 as published by the Free Software Foundation.
 */

#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>

#define FLASH_PHYS_ADDR		0x1FC00000
#define FLASH_SIZE 		0x200000  

static struct mtd_info *adm5120_mtd;

static struct map_info adm5120_mtd_map = {
	.name = "ADM5120",
	.size = FLASH_SIZE,
	.bankwidth = 2,
	.phys = FLASH_PHYS_ADDR,
};

static int __init adm5120_mtd_init(void)
{
	printk(KERN_INFO "ADM5120 board flash (0x%x at 0x%x)\n", FLASH_SIZE,
	    FLASH_PHYS_ADDR);
	adm5120_mtd_map.virt = ioremap_nocache(FLASH_PHYS_ADDR, FLASH_SIZE);
	simple_map_init(&adm5120_mtd_map);
	adm5120_mtd = do_map_probe("cfi_probe", &adm5120_mtd_map);
	if (adm5120_mtd) {
		adm5120_mtd->owner = THIS_MODULE;
		add_mtd_device(adm5120_mtd);
		return 0;
	}
	return -ENXIO;
}

static void __exit adm5120_mtd_exit(void)
{
	del_mtd_device(adm5120_mtd);
	map_destroy(adm5120_mtd);
}

module_init(adm5120_mtd_init);
module_exit(adm5120_mtd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeroen Vreeken (pe1rxq@amsat.org)");
MODULE_DESCRIPTION("MTD map driver for ADM5120 boards");
