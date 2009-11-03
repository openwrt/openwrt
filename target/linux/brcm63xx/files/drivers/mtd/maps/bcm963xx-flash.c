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

static struct tagiddesc_t tagidtab[NUM_TAGID] = TAGID_DEFINITIONS;

static uint32_t tagcrc32tab[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

static uint32_t tagcrc32(uint32_t crc, uint8_t *data, size_t len)
{
	while (len--)
		crc = (crc >> 8) ^ tagcrc32tab[(crc ^ *data++) & 0xFF];

	return crc;
}

static int parse_cfe_partitions( struct mtd_info *master, struct mtd_partition **pparts)
{
	int nrparts = 3, curpart = 0; /* CFE,NVRAM and global LINUX are always present. */
	union bcm_tag *buf;
	struct mtd_partition *parts;
	int ret;
	size_t retlen;
	unsigned int rootfsaddr, kerneladdr, spareaddr;
	unsigned int rootfslen, kernellen, sparelen, totallen;
	unsigned char *tagid;
	int namelen = 0;
	int i;
	uint32_t tagidcrc;
	uint32_t calctagidcrc;
        bool tagid_match = false;
	char *boardid;
        char *tagversion;
	char *matchtagid;

	/* Allocate memory for buffer */
	buf = vmalloc(sizeof(union bcm_tag));
	if (!buf)
		return -ENOMEM;

	/* Get the tag */
	ret = master->read(master,master->erasesize,sizeof(union bcm_tag), &retlen, (void *)buf);
	if (retlen != sizeof(union bcm_tag)){
		vfree(buf);
		return -EIO;
	}

	/* tagId isn't in the same location, so we check each tagid against the
         * tagid CRC.  If the CRC is valid we have found the right tag and so
         * use that tag
         */

	for (i = 0; i < NUM_TAGID; i++) {
	  switch(i) {
	  case 0:
	    matchtagid = "bccfe";
	    tagid = &(buf->bccfe.tagId[0]);
	    sscanf(buf->bccfe.rootAddress,"%u", &rootfsaddr);
	    sscanf(buf->bccfe.rootLength, "%u", &rootfslen);
	    sscanf(buf->bccfe.kernelAddress, "%u", &kerneladdr);
	    sscanf(buf->bccfe.kernelLength, "%u", &kernellen);
	    sscanf(buf->bccfe.totalLength, "%u", &totallen);
	    tagidcrc = *(uint32_t *)&(buf->bccfe.tagIdCRC[0]);
	    tagversion = &(buf->bccfe.tagVersion[0]);
	    boardid = &(buf->bccfe.boardid[0]);
	    break;
	  case 1:
	    matchtagid = "bc300";
	    tagid = &(buf->bc300.tagId[0]);
	    sscanf(buf->bc300.rootAddress,"%u", &rootfsaddr);
	    sscanf(buf->bc300.rootLength, "%u", &rootfslen);
	    sscanf(buf->bc300.kernelAddress, "%u", &kerneladdr);
	    sscanf(buf->bc300.kernelLength, "%u", &kernellen);
	    sscanf(buf->bc300.totalLength, "%u", &totallen);
	    tagidcrc = *(uint32_t *)&(buf->bc300.tagIdCRC[0]);
	    tagversion = &(buf->bc300.tagVersion[0]);
	    boardid = &(buf->bc300.boardid[0]);
	    break;
	  case 2:
	    matchtagid = "ag306";
	    tagid = &(buf->ag306.tagId[0]);
	    sscanf(buf->ag306.rootAddress,"%u", &rootfsaddr);
	    sscanf(buf->ag306.rootLength, "%u", &rootfslen);
	    sscanf(buf->ag306.kernelAddress, "%u", &kerneladdr);
	    sscanf(buf->ag306.kernelLength, "%u", &kernellen);
	    sscanf(buf->ag306.totalLength, "%u", &totallen);
	    tagidcrc = *(uint32_t *)&(buf->ag306.tagIdCRC[0]);
	    tagversion = &(buf->ag306.tagVersion[0]);
	    boardid = &(buf->ag306.boardid[0]);
	    break;
	  case 3:
	    matchtagid = "bc221";
	    tagid = &(buf->bc221.tagId[0]);
	    sscanf(buf->bc221.rootAddress,"%u", &rootfsaddr);
	    sscanf(buf->bc221.rootLength, "%u", &rootfslen);
	    sscanf(buf->bc221.kernelAddress, "%u", &kerneladdr);
	    sscanf(buf->bc221.kernelLength, "%u", &kernellen);
	    sscanf(buf->bc221.totalLength, "%u", &totallen);
	    tagidcrc = *(uint32_t *)&(buf->bc221.tagIdCRC[0]);
	    tagversion = &(buf->bc221.tagVersion[0]);
	    boardid = &(buf->bc221.boardid[0]);
	    break;
	  case 4:
	    matchtagid = "bc310";
	    tagid = &(buf->bc310.tagId[0]);
	    sscanf(buf->bc310.rootAddress,"%u", &rootfsaddr);
	    sscanf(buf->bc310.rootLength, "%u", &rootfslen);
	    sscanf(buf->bc310.kernelAddress, "%u", &kerneladdr);
	    sscanf(buf->bc310.kernelLength, "%u", &kernellen);
	    sscanf(buf->bc310.totalLength, "%u", &totallen);
	    tagidcrc = *(uint32_t *)&(buf->bc310.tagIdCRC[0]);
	    tagversion = &(buf->bc310.tagVersion[0]);
	    boardid = &(buf->bc310.boardid[0]);
	    break;
	  }
	  if (strncmp(tagid, matchtagid, TAGID_LEN) != 0) {
	    continue;
	  }

	  calctagidcrc = htonl(tagcrc32(IMAGETAG_CRC_START, tagid, TAGID_LEN));
	  if (tagidcrc == calctagidcrc) {
	    tagid_match = true;
	    break;
	  }
	}

	if (!tagid_match) {
	    tagid = "bcram";
	    sscanf(buf->bccfe.rootAddress,"%u", &rootfsaddr);
	    sscanf(buf->bccfe.rootLength, "%u", &rootfslen);
	    sscanf(buf->bccfe.kernelAddress, "%u", &kerneladdr);
	    sscanf(buf->bccfe.kernelLength, "%u", &kernellen);
	    sscanf(buf->bccfe.totalLength, "%u", &totallen);
	    tagidcrc = *(uint32_t *)&(buf->bccfe.tagIdCRC[0]);
	    tagversion = &(buf->bccfe.tagVersion[0]);
	    boardid = &(buf->bccfe.boardid[0]);
	}

	printk(KERN_INFO PFX "CFE boot tag found with version %s, board type %s, and tagid %s.\n",tagversion,boardid,tagid);
	
	rootfsaddr = rootfsaddr - EXTENDED_SIZE;
	kerneladdr = kerneladdr - EXTENDED_SIZE;
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
		printk(KERN_INFO PFX "Partition %d is %s offset %llx and length %llx\n", i, parts[i].name, parts[i].offset, parts[i].size);

 	printk(KERN_INFO PFX "Spare partition is %x offset and length %x\n", spareaddr, sparelen);
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
			printk(KERN_INFO PFX "Support for extended flash memory size : 0x%llx ; ONLY 64MBIT SUPPORT\n", bcm963xx_mtd_info->size);
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
