/*
 * ar525w RDC321x platform devices
 *
 *  Copyright (C) 2007-2009 OpenWrt.org
 *  Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *  Copyright (C) 2008-2009 Daniel Gimpelevich <daniel@gimpelevich.san-francisco.ca.us>
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
#include <linux/vmalloc.h>
#include <linux/mtd/mtd.h>

#include <asm/rdc_boards.h>

struct image_header {
	char magic[4];              /* ASICII: GMTK */
	u32 checksum;               /* CRC32 */
	u32 version;                /* x.x.x.x */
	u32 kernelsz;               /* The size of the kernel image */
	u32 imagesz;                /* The length of this image file ( kernel + romfs + this header) */
	u32 pid;                    /* Product ID */
	u32 fastcksum;              /* Partial CRC32 on (First(256), medium(256), last(512)) */
	u32 reserved;
};

static struct gpio_led ar525w_leds[] = {
	{ .name = "rdc321x:dmz", .gpio = 1, .active_low = 1},
};
static struct gpio_button ar525w_btns[] = {
	{
		.gpio = 6,
		.code = BTN_0,
		.desc = "Reset",
		.active_low = 1,
	}
};

static u32 __initdata crctab[257] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
	0
};

static u32 __init crc32(u8 * buf, u32 len)
{
	register int i;
	u32 sum;
	register u32 s0;
	s0 = ~0;
	for (i = 0; i < len; i++) {
		s0 = (s0 >> 8) ^ crctab[(u8) (s0 & 0xFF) ^ buf[i]];
	}
	sum = ~s0;
	return sum;
}

static int __init fixup_ar525w_header(struct mtd_info *master, struct image_header *header)
{
	char *buffer;
	int res;
	u32 bufferlength = header->kernelsz + sizeof(struct image_header);
	u32 len;
	char crcbuf[0x400];

	printk(KERN_INFO "Fixing up AR525W header, old image size: %u, new image size: %u\n",
		   header->imagesz, bufferlength);

	buffer = vmalloc(bufferlength);
	if (!buffer) {
		printk(KERN_ERR "Can't allocate %u bytes\n", bufferlength);
		return -ENOMEM;
	}

	res =  master->read(master, 0x0, bufferlength, &len, buffer);
	if (res || len != bufferlength)
		goto out;

	header = (struct image_header *) buffer;
	header->imagesz = bufferlength;
	header->checksum = 0;
	header->fastcksum = 0;

	memcpy(crcbuf, buffer, 0x100);
	memcpy(crcbuf + 0x100, buffer + (bufferlength >> 1) - ((bufferlength & 0x6) >> 1), 0x100);
	memcpy(crcbuf + 0x200, buffer + bufferlength - 0x200, 0x200);

	header->fastcksum = crc32(crcbuf, sizeof(crcbuf));
	header->checksum = crc32(buffer, bufferlength);

	if (master->unlock)
		master->unlock(master, 0, master->erasesize);
	res = erase_write (master, 0, master->erasesize, buffer);
	if (res)
		printk(KERN_ERR "Can't rewrite image header\n");

out:
	vfree(buffer);
	return res;
}

static int __init parse_ar525w_partitions(struct mtd_info *master, struct mtd_partition **pparts, unsigned long plat_data)
{
	struct image_header header;
	int res;
	size_t len;
	struct mtd_partition *rdc_flash_parts;
	struct rdc_platform_data *pdata = (struct rdc_platform_data *) plat_data;

	if (master->size != 0x400000) //4MB
		return -ENOSYS;

	res =  master->read(master, 0x0, sizeof(header), &len, (char *)&header);
	if (res)
		return res;

	if (strncmp(header.magic, "GMTK", 4))
		return -ENOSYS;

	if (header.kernelsz > 0x400000 || header.kernelsz < master->erasesize) {
		printk(KERN_ERR "AR525W image header found, but seems corrupt, kernel size %u\n", header.kernelsz);
		return -EINVAL;
	}

	if (header.kernelsz + sizeof(header) != header.imagesz) {
		res = fixup_ar525w_header(master, &header);
		if (res)
			return res;
	}

	rdc_flash_parts = kzalloc(sizeof(struct mtd_partition) * 3, GFP_KERNEL);

	rdc_flash_parts[0].name = "firmware";
	rdc_flash_parts[0].offset = 0x0;
	rdc_flash_parts[0].size = 0x3E0000;
	rdc_flash_parts[1].name = "rootfs";
	rdc_flash_parts[1].offset = header.kernelsz + sizeof(header);
	rdc_flash_parts[1].size = rdc_flash_parts[0].size - rdc_flash_parts[1].offset;
	rdc_flash_parts[2].name = "bootloader";
	rdc_flash_parts[2].offset = 0x3E0000;
	rdc_flash_parts[2].size = 0x20000;

	*pparts = rdc_flash_parts;

	pdata->led_data.num_leds = ARRAY_SIZE(ar525w_leds);
	pdata->led_data.leds = ar525w_leds;
	pdata->button_data.nbuttons = ARRAY_SIZE(ar525w_btns);
	pdata->button_data.buttons = ar525w_btns;

	return 3;
}

static struct mtd_part_parser __initdata ar525w_parser = {
	.owner = THIS_MODULE,
	.parse_fn = parse_ar525w_partitions,
	.name = "AR525W",
};

static int __init ar525w_setup(void)
{
	return register_mtd_parser(&ar525w_parser);
}

arch_initcall(ar525w_setup);
