/*
 * Copyright (C) 2006-2008  Florian Fainelli <florian@openwrt.org>
 * 			    Mike Albon <malbon@openwrt.org>
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

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>

#include <bcm_tag.h>
#include <asm/io.h>

#define BUSWIDTH 2                     /* Buswidth */
#define EXTENDED_SIZE 0xBFC00000       /* Extended flash address */

#define PFX KBUILD_MODNAME ": "

extern int parse_redboot_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long fis_origin);
static struct mtd_partition *parsed_parts;

static struct mtd_info *bcm963xx_mtd_info;

static struct map_info bcm963xx_map = {
       .name		= "bcm963xx",
       .bankwidth	= BUSWIDTH,
};

static int parse_cfe_partitions( struct mtd_info *master, struct mtd_partition **pparts)
{
	int nrparts = 3, curpart = 0; /* CFE,NVRAM and global LINUX are always present. */
	struct bcm_tag *buf;
	struct mtd_partition *parts;
	int ret;
	size_t retlen;
	unsigned int rootfsaddr, kerneladdr, spareaddr;
	unsigned int rootfslen, kernellen, sparelen, totallen;
	int namelen = 0;
	int i;

	/* Allocate memory for buffer */
	buf = vmalloc(sizeof(struct bcm_tag));
	if (!buf)
		return -ENOMEM;

	/* Get the tag */
	ret = master->read(master,master->erasesize,sizeof(struct bcm_tag), &retlen, (void *)buf);
	if (retlen != sizeof(struct bcm_tag)){
		vfree(buf);
		return -EIO;
	}
	printk(KERN_INFO PFX "CFE boot tag found with version %s and board type %s.\n",buf->tagVersion,buf->boardid);
	
	/* Get the values and calculate */
	sscanf(buf->rootAddress,"%u", &rootfsaddr);
	rootfsaddr = rootfsaddr - EXTENDED_SIZE;
	sscanf(buf->rootLength, "%u", &rootfslen);
	sscanf(buf->kernelAddress, "%u", &kerneladdr);
	kerneladdr = kerneladdr - EXTENDED_SIZE;
	sscanf(buf->kernelLength, "%u", &kernellen);
	sscanf(buf->totalLength, "%u", &totallen);
	spareaddr = roundup(totallen,master->erasesize) + master->erasesize;
	sparelen = master->size - spareaddr - master->erasesize;

	/* Determine number of partitions */
	namelen = 8;
	if (rootfslen > 0){
		nrparts++;
		namelen =+ 6;
	};
	if (kernellen > 0) {
		nrparts++;
		namelen =+ 6;
	};

	/* Ask kernel for more memory */
	parts = kzalloc(sizeof(*parts) * nrparts + 10 * nrparts, GFP_KERNEL);
	if (!parts) {
		vfree(buf);
		return -ENOMEM;
	};
	
	/* Start building partition list */
	parts[curpart].name = "CFE";
	parts[curpart].offset = 0;
	parts[curpart].size = master->erasesize;
	curpart++;

	if (kernellen > 0) {
		parts[curpart].name = "kernel";
		parts[curpart].offset = kerneladdr;
		parts[curpart].size = kernellen;
		curpart++;
	};

	if (rootfslen > 0) {
		parts[curpart].name = "rootfs";
		parts[curpart].offset = rootfsaddr;
		parts[curpart].size = rootfslen;
		if (sparelen > 0)
			parts[curpart].size += sparelen;
		curpart++;
	};
	
	parts[curpart].name = "nvram";
	parts[curpart].offset = master->size - master->erasesize;
	parts[curpart].size = master->erasesize;

	/* Global partition "linux" to make easy firmware upgrade */
	curpart++;
	parts[curpart].name = "linux";
	parts[curpart].offset = parts[0].size;
	parts[curpart].size = master->size - parts[0].size - parts[3].size;
       
	for (i = 0; i < nrparts; i++)
		printk(KERN_INFO PFX "Partition %d is %s offset %x and length %x\n", i, parts[i].name, parts[i].offset, parts[i].size);

	*pparts = parts;
	vfree(buf);

	return nrparts;
};

static int bcm963xx_detect_cfe(struct mtd_info *master)
{
	int idoffset = 0x4e0;
	static char idstring[8] = "CFE1CFE1";
	char buf[9];
	int ret;
	size_t retlen;

	ret = master->read(master, idoffset, 8, &retlen, (void *)buf);
	buf[retlen] = 0;
	printk(KERN_INFO PFX "Read Signature value of %s\n", buf);
	
	return strncmp(idstring, buf, 8);
}

static int bcm963xx_probe(struct platform_device *pdev)
{
	int err = 0;
	int parsed_nr_parts = 0;
	char *part_type;
	struct resource *r;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0); 
	bcm963xx_map.phys = r->start;
	bcm963xx_map.size = (r->end - r->start) + 1;
	bcm963xx_map.virt = ioremap(r->start, r->end - r->start + 1);

	if (!bcm963xx_map.virt) {
		printk(KERN_ERR PFX "Failed to ioremap\n");
		return -EIO;
	}
	printk(KERN_INFO PFX "0x%08lx at 0x%08x\n", bcm963xx_map.size, bcm963xx_map.phys);

	simple_map_init(&bcm963xx_map);

	bcm963xx_mtd_info = do_map_probe("cfi_probe", &bcm963xx_map);
	if (!bcm963xx_mtd_info) {
		printk(KERN_ERR PFX "Failed to probe using CFI\n");
		err = -EIO;
		goto err_probe;
	}

	bcm963xx_mtd_info->owner = THIS_MODULE;

	/* This is mutually exclusive */
	if (bcm963xx_detect_cfe(bcm963xx_mtd_info) == 0) {
		printk(KERN_INFO PFX "CFE bootloader detected\n");
		if (parsed_nr_parts == 0) {
			int ret = parse_cfe_partitions(bcm963xx_mtd_info, &parsed_parts);
			if (ret > 0) {
				part_type = "CFE";
				parsed_nr_parts = ret;
			}
		}
	} else {
		printk(KERN_INFO PFX "assuming RedBoot bootloader\n");
		if (bcm963xx_mtd_info->size > 0x00400000) {
			printk(KERN_INFO PFX "Support for extended flash memory size : 0x%08X ; ONLY 64MBIT SUPPORT\n", bcm963xx_mtd_info->size);
			bcm963xx_map.virt = (u32)(EXTENDED_SIZE);
		}

#ifdef CONFIG_MTD_REDBOOT_PARTS
		if (parsed_nr_parts == 0) {
			int ret = parse_redboot_partitions(bcm963xx_mtd_info, &parsed_parts, 0);
			if (ret > 0) {
				part_type = "RedBoot";
				parsed_nr_parts = ret;
			}
		}
#endif
	}
	
	return add_mtd_partitions(bcm963xx_mtd_info, parsed_parts, parsed_nr_parts);

err_probe:
	iounmap(bcm963xx_map.virt);
	return err;
}

static int bcm963xx_remove(struct platform_device *pdev)
{
	if (bcm963xx_mtd_info) {
		del_mtd_partitions(bcm963xx_mtd_info);
		map_destroy(bcm963xx_mtd_info);
	}

	if (bcm963xx_map.virt) {
		iounmap(bcm963xx_map.virt);
		bcm963xx_map.virt = 0;
	}

	return 0;
}

static struct platform_driver bcm63xx_mtd_dev = {
	.probe	= bcm963xx_probe,
	.remove = bcm963xx_remove,
	.driver = {
		.name	= "bcm963xx-flash",
		.owner	= THIS_MODULE,
	},
};

static int __init bcm963xx_mtd_init(void)
{
	return platform_driver_register(&bcm63xx_mtd_dev);
}

static void __exit bcm963xx_mtd_exit(void)
{
	platform_driver_unregister(&bcm63xx_mtd_dev);
}

module_init(bcm963xx_mtd_init);
module_exit(bcm963xx_mtd_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Broadcom BCM63xx MTD partition parser/mapping for CFE and RedBoot");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_AUTHOR("Mike Albon <malbon@openwrt.org>");
