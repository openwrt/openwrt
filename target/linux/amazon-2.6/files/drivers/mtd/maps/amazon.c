/*
 * Handle mapping of the flash memory access routines
 * on Amazon  based devices.
 *
 * Copyright(C) 2004 peng.liu@infineon.com
 *
 * This code is GPLed
 *
 */
// 000005:fchang 2005/6/2 Modified by Bingtao to double check if the EBU is enabled/disabled
// 506231:tc.chen 2005/06/23 increase firmware partition size form 192KB to 256KB
// 050701:linmars 2005/07/01 fix flash size wrong alignment after increase firmware partition
// 165001:henryhsu 2005/8/18 Remove the support for Intel flash because of 2.1 not enough rootfs partition size
// 165001:henryhsu 2005/9/7 Rolback to support INtel flash
// 509071:tc.chen 2005/09/07 Reduced flash writing time
// 511046:linmars 2005/11/04 change bootloader size from 128 into 64
// 511241:linmars 2005/11/24 merge TaiChen's IRM patch 

// copyright 2005 infineon

// copyright 2007 john crispin <blogic@openwrt.org>
// copyright 2007 felix fietkau <nbd@openwrt.org>

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>

#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/cfi.h>
#include <linux/mutex.h>
#include <asm/amazon/amazon.h>

#define AMAZON_PCI_ARB_CTL_ALT 0xb100205c
#define AMAZON_MTD_REG32( addr )          (*(volatile u32 *)(addr))


static struct map_info amazon_map = {
	.name = "AMAZON_FLASH",
	.bankwidth = 2,
	.size = 0x400000,
};

static map_word amazon_read16(struct map_info * map, unsigned long ofs)
{
	map_word temp;
	ofs ^= 2;
	temp.x[0] = *((__u16 *) (map->virt + ofs));
	return temp;
}

static void amazon_write16(struct map_info *map, map_word d, unsigned long adr)
{
	adr ^= 2;
	*((__u16 *) (map->virt + adr)) = d.x[0];
}

void amazon_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	u8 *p;
	u8 *to_8;
	ssize_t l = len;
	from = (unsigned long) (from + map->virt);
	p = (u8 *) from;
	to_8 = (u8 *) to;
	while(len--){
		*to_8++ = *p++;
	}
}

void amazon_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	u8 *p =  (u8*) from;
	u8 *to_8;
	to += (unsigned long) map->virt;
	to_8 = (u8*)to;
	while(len--){
		*p++ = *to_8++;
	}
}

#define UBOOT_SIZE		0x40000

static struct mtd_partition	amazon_partitions[3] = {
	 {
		  name:"U-Boot",		/* U-Boot firmware */
		  offset:0x00000000,
		  size:UBOOT_SIZE ,		/* 128k */
	  },
	 {
		  name:"kernel",		/* firmware */
		  offset:UBOOT_SIZE,
		  size:0x00100000,		/* 192K */
	  },
	 {
		  name:"rootfs",		/* default partition */
		  offset:0x00200000,
		  size:0x00200000,
	  },
};


unsigned long flash_start = 0x13000000;
unsigned long flash_size = 0x800000;
unsigned long uImage_size = 0x10000d;

int find_uImage_size(unsigned long start_offset){
	unsigned long temp;

	printk("trying to find uImage and its size\n");
	amazon_copy_from(&amazon_map, &temp, start_offset + 12, 4);
	printk("kernel size is %d \n", temp + 0x40);
	return temp + 0x40;
}

int __init init_amazon_mtd(void)
{
	int ret = 0;
	struct mtd_info *mymtd = NULL;
	struct mtd_partition *parts = NULL;

	*AMAZON_EBU_BUSCON0 = 0x1d7ff;
	
	amazon_map.read = amazon_read16;
	amazon_map.write = amazon_write16;
	amazon_map.copy_from = amazon_copy_from;
	amazon_map.copy_to = amazon_copy_to;

	amazon_map.phys = flash_start;
	amazon_map.virt = ioremap_nocache(flash_start, flash_size);
	
	if (!amazon_map.virt) {
		printk(KERN_WARNING "Failed to ioremap!\n");
		return -EIO;
	}
	
	mymtd = (struct mtd_info *) do_map_probe("cfi_probe", &amazon_map);
	if (!mymtd) {
		iounmap(amazon_map.virt);
		printk("probing failed\n");
		return -ENXIO;
	}

	mymtd->owner = THIS_MODULE;
	parts = &amazon_partitions[0];
	amazon_partitions[2].offset = UBOOT_SIZE + find_uImage_size(amazon_partitions[1].offset);
	amazon_partitions[1].size = mymtd->size - amazon_partitions[1].offset - (2 * mymtd->erasesize);
	amazon_partitions[2].size = mymtd->size - amazon_partitions[2].offset - (2 * mymtd->erasesize);
	add_mtd_partitions(mymtd, parts, 3);
	return 0;
}

static void __exit cleanup_amazon_mtd(void)
{
	/* FIXME! */
}

module_init(init_amazon_mtd);
module_exit(cleanup_amazon_mtd);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("john crispin blogic@openwrt.org");
MODULE_DESCRIPTION("MTD map driver for AMAZON boards");
