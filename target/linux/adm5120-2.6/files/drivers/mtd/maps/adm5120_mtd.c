/*
 *	Flash device on the ADM5120 board
 *
 *	Copyright Florian Fainelli <florian@openwrt.org>
 *   
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
#include <linux/mtd/partitions.h>
#include <linux/vmalloc.h>


#define FLASH_PHYS_ADDR		0x1FC00000
#define FLASH_SIZE 		0x400000

#define IMAGE_LEN 		10                   	/* Length of Length Field */
#define ADDRESS_LEN 		12                 	/* Length of Address field */
#define EXTENDED_SIZE 		0xBFC00000       	/* Extended flash address */
#define ROUNDUP(x, y) ((((x)+((y)-1))/(y))*(y))
 

static struct mtd_info *adm5120_mtd;
static struct mtd_partition *parsed_parts;

static struct map_info adm5120_mtd_map = {
	.name = "adm5120",
	.size = FLASH_SIZE,
	.bankwidth = 2,
	.phys = FLASH_PHYS_ADDR,
};

static int parse_cfe_partitions( struct mtd_info *master, struct mtd_partition **pparts)
{
	int nrparts = 2, curpart = 0; // CFE and NVRAM always present.
	struct adm5120_cfe_map {
		unsigned char tagVersion[4];                            // Version of the image tag
		unsigned char sig_1[20];                                // Company Line 1
		unsigned char sig_2[14];                                // Company Line 2
		unsigned char chipid[6];                                        // Chip this image is for
		unsigned char boardid[16];                              // Board name
		unsigned char bigEndian[2];                             // Map endianness -- 1 BE 0 LE
		unsigned char totalLength[IMAGE_LEN];           //Total length of image
		unsigned char cfeAddress[ADDRESS_LEN];  // Address in memory of CFE
		unsigned char cfeLength[IMAGE_LEN];             // Size of CFE
		unsigned char rootAddress[ADDRESS_LEN];         // Address in memory of rootfs
		unsigned char rootLength[IMAGE_LEN];            // Size of rootfs
		unsigned char kernelAddress[ADDRESS_LEN];       // Address in memory of kernel
		unsigned char kernelLength[IMAGE_LEN];  // Size of kernel
		unsigned char dualImage[2];                             // Unused at present
		unsigned char inactiveFlag[2];                  // Unused at present
		unsigned char reserved1[74];                            // Reserved area not in use
		unsigned char imageCRC[4];                              // CRC32 of images
		unsigned char reserved2[16];                            // Unused at present
		unsigned char headerCRC[4];                             // CRC32 of header excluding tagVersion
		unsigned char reserved3[16];                            // Unused at present
	} *buf;
	struct mtd_partition *parts;
	int ret;
	size_t retlen;
	unsigned int rootfsaddr, kerneladdr, spareaddr;
	unsigned int rootfslen, kernellen, sparelen, totallen;
	int namelen = 0;
	int i;
	// Allocate memory for buffer
	buf = vmalloc(sizeof(struct adm5120_cfe_map));

	if (!buf)
		return -ENOMEM;

	// Get the tag
	ret = master->read(master,master->erasesize,sizeof(struct adm5120_cfe_map), &retlen, (void *)buf);

	if (retlen != sizeof(struct adm5120_cfe_map)){
		vfree(buf);
		return -EIO;
	};

	printk("adm5120: CFE boot tag found with version %s and board type %s.\n",buf->tagVersion,buf->boardid);
	// Get the values and calculate
	sscanf(buf->rootAddress,"%u", &rootfsaddr);
	rootfsaddr = rootfsaddr - EXTENDED_SIZE;
	sscanf(buf->rootLength, "%u", &rootfslen);
	sscanf(buf->kernelAddress, "%u", &kerneladdr);
	kerneladdr = kerneladdr - EXTENDED_SIZE;
	sscanf(buf->kernelLength, "%u", &kernellen);
	sscanf(buf->totalLength, "%u", &totallen);
	spareaddr = ROUNDUP(totallen,master->erasesize) + master->erasesize;
	sparelen = master->size - spareaddr - master->erasesize;
	// Determine number of partitions
	namelen = 8;
	if (rootfslen > 0){
		nrparts++;
		namelen =+ 6;
	};

	if (kernellen > 0){
		nrparts++;
		namelen =+ 6;
	};
	if (sparelen > 0){
		nrparts++;
		namelen =+ 6;
	};
	// Ask kernel for more memory.
	parts = kmalloc(sizeof(*parts)*nrparts+10*nrparts, GFP_KERNEL);
	if (!parts){
		vfree(buf);
		return -ENOMEM;
	};
	memset(parts,0,sizeof(*parts)*nrparts+10*nrparts);
	// Start building partition list
	parts[curpart].name = "CFE";
	parts[curpart].offset = 0;
	parts[curpart].size = master->erasesize;
	curpart++;
	if (kernellen > 0) {
		parts[curpart].name = "Kernel";
		parts[curpart].offset = kerneladdr;
		parts[curpart].size = kernellen;
		curpart++;
	};
	if (rootfslen > 0) {
		parts[curpart].name = "Rootfs";
		parts[curpart].offset = rootfsaddr;
		parts[curpart].size = rootfslen;
		curpart++;
	};
	if (sparelen > 0){
		parts[curpart].name = "OpenWrt";
		parts[curpart].offset = spareaddr;
		parts[curpart].size = sparelen;
		curpart++;
	};
	parts[curpart].name = "NVRAM";
	parts[curpart].offset = master->size - master->erasesize;
	parts[curpart].size = master->erasesize;
	for (i = 0; i < nrparts; i++) {
		printk("adm5120: Partition %d is %s offset %x and length %x\n", i, parts[i].name, parts[i].offset, parts[i].size);
	}
	*pparts = parts;
	vfree(buf);
	return nrparts;
};

static int adm5120_detect_cfe(struct mtd_info *master)
{
	int idoffset = 0x4e0;
#ifdef CONFIG_CPU_BIG_ENDIAN
	static char idstring[8] = "CFE1CFE1";
#else
	static char idstring[8] = "1EFC1EFC";
#endif
	char buf[8];
	int ret;
	size_t retlen;

	ret = master->read(master, idoffset, 8, &retlen, (void *)buf);
	printk("adm5120: Read Signature value of %s\n", buf);
	return strcmp(idstring,buf);
}

static int __init adm5120_mtd_init(void)
{
	printk(KERN_INFO "ADM5120 board flash (0x%08x at 0x%08x)\n", FLASH_SIZE,
	    FLASH_PHYS_ADDR);

	adm5120_mtd_map.virt = ioremap(FLASH_PHYS_ADDR, FLASH_SIZE);

	if (!adm5120_mtd_map.virt) {
		printk("ADM5120: failed to ioremap\n");
		return -EIO;
	}
		
	simple_map_init(&adm5120_mtd_map);

	adm5120_mtd = do_map_probe("cfi_probe", &adm5120_mtd_map);

	if (adm5120_mtd) {
		adm5120_mtd->owner = THIS_MODULE;
		
		if (adm5120_detect_cfe(adm5120_mtd) == 0)
		{
			int parsed_nr_parts = 0;
			char * part_type;

			printk("adm5120: CFE bootloader detected\n");
			
			if (parsed_nr_parts == 0) {
				int ret = parse_cfe_partitions(adm5120_mtd, &parsed_parts);
				
				if (ret > 0) {
					part_type = "CFE";
					parsed_nr_parts = ret;
				}
			}
			add_mtd_partitions(adm5120_mtd, parsed_parts, parsed_nr_parts);
			return 0;
		} else {
			add_mtd_device(adm5120_mtd);
		}
		return 0;
	}
	iounmap(adm5120_mtd);
	return -ENXIO;
}

static void __exit adm5120_mtd_exit(void)
{
	if (adm5120_mtd) {
		del_mtd_device(adm5120_mtd);
		map_destroy(adm5120_mtd);
	}

	if (adm5120_mtd_map.virt) {
		iounmap(adm5120_mtd_map.virt);
		adm5120_mtd_map.virt = 0;
	}
	
}

module_init(adm5120_mtd_init);
module_exit(adm5120_mtd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("MTD map driver for ADM5120 boards");
