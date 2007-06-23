/*
 * $Id$
 *
 * Copyright (C) 2007 OpenWrt.org
 * Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/types.h>
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/byteorder.h>

#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_switch.h>
#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/myloader.h>
#include <asm/mach-adm5120/routerboot.h>
#include <asm/mach-adm5120/zynos.h>

/*
 * Globals
 */
struct adm5120_board adm5120_board;
unsigned int adm5120_boot_loader;

unsigned int adm5120_product_code;
unsigned int adm5120_revision;
unsigned int adm5120_package;
unsigned int adm5120_nand_boot;
unsigned long adm5120_speed;
unsigned long adm5120_memsize;

/*
 * Locals
 */
static char *boot_loader_names[BOOT_LOADER_LAST+1] = {
	[BOOT_LOADER_UNKNOWN]	= "Unknown",
	[BOOT_LOADER_CFE]	= "CFE",
	[BOOT_LOADER_UBOOT]	= "U-Boot",
	[BOOT_LOADER_MYLOADER]	= "MyLoader",
	[BOOT_LOADER_ROUTERBOOT]= "RouterBOOT",
	[BOOT_LOADER_BOOTBASE]	= "Bootbase"
};

static struct adm5120_board __initdata adm5120_boards[] = {
	{
		.name		= "Compex NetPassage 27G",
		.mach_type	= MACH_ADM5120_NP27G,
		.has_usb	= 1,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex NetPassage 28G",
		.mach_type	= MACH_ADM5120_NP28G,
		.has_usb	= 0,
		.iface_num	= 4,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex NP28G (HotSpot)",
		.mach_type	= MACH_ADM5120_NP28GHS,
		.has_usb	= 0,
		.iface_num	= 4,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex WP54AG",
		.mach_type	= MACH_ADM5120_WP54AG,
		.has_usb	= 0,
		.iface_num	= 2,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex WP54G",
		.mach_type	= MACH_ADM5120_WP54G,
		.has_usb	= 0,
		.iface_num	= 2,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex WP54G-WRT",
		.mach_type	= MACH_ADM5120_WP54G_WRT,
		.has_usb	= 0,
		.iface_num	= 2,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex WP54G v1C",
		.mach_type	= MACH_ADM5120_WP54Gv1C,
		.has_usb	= 0,
		.iface_num	= 2,
		.flash0_size	= 2*1024*1024,
	},
	{
		.name		= "Compex WPP54AG",
		.mach_type	= MACH_ADM5120_WPP54AG,
		.has_usb	= 0,
		.iface_num	= 2,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Compex WPP54G",
		.mach_type	= MACH_ADM5120_WPP54G,
		.has_usb	= 0,
		.iface_num	= 2,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "RouterBOARD 111",
		.mach_type	= MACH_ADM5120_RB_111,
		.has_usb	= 0,
		.iface_num	= 1,
		.flash0_size	= 128*1024,
	},
	{
		.name		= "RouterBOARD 112",
		.mach_type	= MACH_ADM5120_RB_112,
		.has_usb	= 0,
		.iface_num	= 1,
		.flash0_size	= 128*1024,
	},
	{
		.name		= "RouterBOARD 133",
		.mach_type	= MACH_ADM5120_RB_133,
		.has_usb	= 0,
		.iface_num	= 3,
		.flash0_size	= 128*1024,
	},
	{
		.name		= "RouterBOARD 133C",
		.mach_type	= MACH_ADM5120_RB_133C,
		.has_usb	= 0,
		.iface_num	= 1,
		.flash0_size	= 128*1024,
	},
	{
		.name		= "RouterBOARD 150",
		.mach_type	= MACH_ADM5120_RB_150,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 128*1024,
	},
	{
		.name		= "RouterBOARD 153",
		.mach_type	= MACH_ADM5120_RB_153,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 128*1024,
	},
	{
		.name		= "ZyXEL ES-2108",
		.mach_type	= MACH_ADM5120_ES2108,
		.has_usb	= 0,
		.iface_num	= 0,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL ES-2108-F",
		.mach_type	= MACH_ADM5120_ES2108F,
		.has_usb	= 0,
		.iface_num	= 0,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL ES-2108-G",
		.mach_type	= MACH_ADM5120_ES2108G,
		.has_usb	= 0,
		.iface_num	= 0,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL ES-2108-LC",
		.mach_type	= MACH_ADM5120_ES2108LC,
		.has_usb	= 0,
		.iface_num	= 0,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL ES-2108PWR",
		.mach_type	= MACH_ADM5120_ES2108PWR,
		.has_usb	= 0,
		.iface_num	= 0,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL HomeSafe 100/100W",
		.mach_type	= MACH_ADM5120_HS100,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 334",
		.mach_type	= MACH_ADM5120_P334,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 2*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 334U",
		.mach_type	= MACH_ADM5120_P334U,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 334W",
		.mach_type	= MACH_ADM5120_P334W,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 2*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 334WH",
		.mach_type	= MACH_ADM5120_P334WH,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 334WHD",
		.mach_type	= MACH_ADM5120_P334WHD,
		.has_usb	= 0,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 334WT",
		.mach_type	= MACH_ADM5120_P334WT,
		.has_usb	= 1,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 335/335WT",
		.mach_type	= MACH_ADM5120_P335,
		.has_usb	= 1,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 335Plus",
		.mach_type	= MACH_ADM5120_P335PLUS,
		.has_usb	= 1,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "ZyXEL Prestige 335U",
		.mach_type	= MACH_ADM5120_P335U,
		.has_usb	= 1,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	},
	{
		.name		= "Unknown ADM5120 board",
		.mach_type	= MACH_ADM5120_UNKNOWN,
		.has_usb	= 1,
		.iface_num	= 5,
		.flash0_size	= 4*1024*1024,
	}
};

#define DUMMY_BOARD()	{.mach_type = MACH_ADM5120_UNKNOWN}

struct mylo_board {
	u16	vid;
	u16	did;
	u16	svid;
	u16	sdid;
	unsigned long	mach_type;
};


#define MYLO_BOARD(v,d,sv,sd,mt) { .vid = (v), .did = (d), .svid = (sv), \
	.sdid = (sd), .mach_type = (mt) }

#define COMPEX_BOARD(d,mt) MYLO_BOARD(VENID_COMPEX,(d),VENID_COMPEX,(d),(mt))

static struct mylo_board  __initdata mylo_boards[] = {
	COMPEX_BOARD(DEVID_COMPEX_NP27G, MACH_ADM5120_NP27G),
	COMPEX_BOARD(DEVID_COMPEX_NP28G, MACH_ADM5120_NP28G),
	COMPEX_BOARD(DEVID_COMPEX_NP28GHS, MACH_ADM5120_NP28GHS),
	COMPEX_BOARD(DEVID_COMPEX_WP54G, MACH_ADM5120_WP54G),
	COMPEX_BOARD(DEVID_COMPEX_WP54Gv1C, MACH_ADM5120_WP54Gv1C),
	COMPEX_BOARD(DEVID_COMPEX_WP54AG, MACH_ADM5120_WP54AG),
	COMPEX_BOARD(DEVID_COMPEX_WPP54G, MACH_ADM5120_WPP54G),
	COMPEX_BOARD(DEVID_COMPEX_WPP54AG, MACH_ADM5120_WPP54AG),
	DUMMY_BOARD()
};

#define ROUTERBOARD_NAME_LEN	16

struct routerboard {
	unsigned long	mach_type;
	char	name[ROUTERBOARD_NAME_LEN];
};

#define ROUTERBOARD(n, mt) { .name = (n), .mach_type = (mt) }
static struct routerboard  __initdata routerboards[] = {
	ROUTERBOARD("111", MACH_ADM5120_RB_111),
	ROUTERBOARD("112", MACH_ADM5120_RB_112),
	ROUTERBOARD("133", MACH_ADM5120_RB_133),
	ROUTERBOARD("133C", MACH_ADM5120_RB_133C),
	ROUTERBOARD("miniROUTER", MACH_ADM5120_RB_150),
	ROUTERBOARD("153", MACH_ADM5120_RB_150),
	DUMMY_BOARD()
};

struct zynos_board {
	unsigned long	mach_type;
	unsigned int	vendor_id;
	u16		board_id;
};

#define ZYNOS_BOARD(vi, bi, mt) { .vendor_id = (vi), .board_id = (bi), \
		.mach_type = (mt) }

#define ZYXEL_BOARD(bi, mt) ZYNOS_BOARD(ZYNOS_VENDOR_ID_ZYXEL, bi, mt)
#define DLINK_BOARD(bi, mt) ZYNOS_BOARD(ZYNOS_VENDOR_ID_DLINK, bi, mt)
#define LUCENT_BOARD(bi, mt) ZYNOS_BOARD(ZYNOS_VENDOR_ID_LUCENT, bi, mt)

static struct zynos_board __initdata zynos_boards[] = {
	ZYXEL_BOARD(ZYNOS_BOARD_HS100, MACH_ADM5120_HS100),
	ZYXEL_BOARD(ZYNOS_BOARD_P334, MACH_ADM5120_P334),
	ZYXEL_BOARD(ZYNOS_BOARD_P334U, MACH_ADM5120_P334U),
	ZYXEL_BOARD(ZYNOS_BOARD_P334W, MACH_ADM5120_P334W),
	ZYXEL_BOARD(ZYNOS_BOARD_P334WH, MACH_ADM5120_P334WH),
	ZYXEL_BOARD(ZYNOS_BOARD_P334WHD, MACH_ADM5120_P334WHD),
	ZYXEL_BOARD(ZYNOS_BOARD_P334WT, MACH_ADM5120_P334WT),
	ZYXEL_BOARD(ZYNOS_BOARD_P335, MACH_ADM5120_P335),
	ZYXEL_BOARD(ZYNOS_BOARD_P335PLUS, MACH_ADM5120_P335PLUS),
	ZYXEL_BOARD(ZYNOS_BOARD_P335U, MACH_ADM5120_P335U),
	DUMMY_BOARD()
};

/*
 * Helper routines
 */
static inline u16 read_le16(void *buf)
{
	u8 *p;

	p = buf;
	return ((u16)p[0] + ((u16)p[1] << 8));
}

static inline u32 read_le32(void *buf)
{
	u8 *p;

	p = buf;
	return ((u32)p[0] + ((u32)p[1] << 8) + ((u32)p[2] << 16) +
		((u32)p[3] << 24));
}

static inline u16 read_be16(void *buf)
{
	u8 *p;

	p = buf;
	return (((u16)p[0] << 8) + (u16)p[1]);
}

static inline u32 read_be32(void *buf)
{
	u8 *p;

	p = buf;
	return (((u32)p[0] << 24) + ((u32)p[1] << 16) + ((u32)p[2] << 8) +
		((u32)p[3]));
}

/*
 * CFE based boards
 */
#define CFE_EPTSEAL	0x43464531 /* CFE1 is the magic number to recognize CFE
from other bootloaders */

static int __init cfe_present(void)
{
	/*
	 * This method only works, when we are booted directly from the CFE.
	 */
	u32 cfe_handle = (u32) fw_arg0;
	u32 cfe_a1_val = (u32) fw_arg1;
	u32 cfe_entry = (u32) fw_arg2;
	u32 cfe_seal = (u32) fw_arg3;

	/* Check for CFE by finding the CFE magic number */
	if (cfe_seal != CFE_EPTSEAL) {
		/* We are not booted from CFE */
		return 0;
	}

	/* cfe_a1_val must be 0, because only one CPU present in the ADM5120 */
	if (cfe_a1_val != 0) {
		return 0;
	}

	/* The cfe_handle, and the cfe_entry must be kernel mode addresses */
	if ((cfe_handle < KSEG0) || (cfe_entry < KSEG0)) {
		return 0;
	}

	return 1;
}

static unsigned long __init cfe_detect_board(void)
{
	return MACH_ADM5120_WP54G_WRT;
}

/*
 * MyLoader based boards
 */
#define SYS_PARAMS_ADDR		KSEG1ADDR(ADM5120_SRAM0_BASE+0x0F000)
#define BOARD_PARAMS_ADDR	KSEG1ADDR(ADM5120_SRAM0_BASE+0x0F800)
#define PART_TABLE_ADDR		KSEG1ADDR(ADM5120_SRAM0_BASE+0x10000)

static unsigned long __init myloader_detect_board(void)
{
	struct mylo_system_params *sysp;
	struct mylo_board_params *boardp;
	struct mylo_partition_table *parts;
	struct mylo_board *board;
	unsigned long ret;

	ret = MACH_ADM5120_UNKNOWN;

	sysp = (struct mylo_system_params *)(SYS_PARAMS_ADDR);
	boardp = (struct mylo_board_params *)(BOARD_PARAMS_ADDR);
	parts = (struct mylo_partition_table *)(PART_TABLE_ADDR);

	/* Check for some magic numbers */
	if ((le32_to_cpu(sysp->magic) != MYLO_MAGIC_SYS_PARAMS) ||
	   (le32_to_cpu(boardp->magic) != MYLO_MAGIC_BOARD_PARAMS) ||
	   (le32_to_cpu(parts->magic) != MYLO_MAGIC_PARTITIONS))
		goto out;

	for (board = mylo_boards; board->mach_type != MACH_ADM5120_UNKNOWN;
		board++) {
		if ((le16_to_cpu(sysp->vid) == board->vid) &&
			(le16_to_cpu(sysp->did) == board->did) &&
			(le16_to_cpu(sysp->svid) == board->svid) &&
			(le16_to_cpu(sysp->sdid) == board->sdid)) {
			ret = board->mach_type;
			break;
		}
	}

	/* assume MyLoader as the boot-loader */
	adm5120_boot_loader = BOOT_LOADER_MYLOADER;

out:
	return ret;
}

/*
 * RouterBOOT based boards
 */
static int __init routerboot_load_hs(u8 *buf, u16 buflen,
	struct rb_hard_settings *hs)
{
	u16 id,len;
	u8 *mac;
	int i,j;

	if (buflen < 4)
		return -1;

	if (read_le32(buf) != RB_MAGIC_HARD)
		return -1;

	/* skip magic value */
	buf += 4;
	buflen -= 4;

	while (buflen > 2) {
		id = read_le16(buf);
		buf += 2;
		buflen -= 2;
		if (id == RB_ID_TERMINATOR || buflen < 2)
			break;

		len = read_le16(buf);
		buf += 2;
		buflen -= 2;

		if (buflen < len)
			break;

		switch (id) {
		case RB_ID_BIOS_VERSION:
			hs->bios_ver = (char *)buf;
			break;
		case RB_ID_BOARD_NAME:
			hs->name = (char *)buf;
			break;
		case RB_ID_MEMORY_SIZE:
			hs->mem_size = read_le32(buf);
			break;
		case RB_ID_MAC_ADDRESS_COUNT:
			hs->mac_count = read_le32(buf);
			break;
		case RB_ID_MAC_ADDRESS_PACK:
			hs->mac_count = len/RB_MAC_SIZE;
			if (hs->mac_count > RB_MAX_MAC_COUNT)
				hs->mac_count = RB_MAX_MAC_COUNT;
			mac = buf;
			for (i=0; i < hs->mac_count; i++) {
				for (j=0; j < RB_MAC_SIZE; j++)
					hs->macs[i][j] = mac[j];
				mac += RB_MAC_SIZE;
			}
			break;
		}

		buf += len;
		buflen -= len;

	}

	return 0;
}

#define RB_BS_OFFS	0x14
#define RB_OFFS_MAX	(128*1024)

static unsigned long __init routerboot_detect_board(void)
{
	struct routerboard *board;
	struct rb_hard_settings	hs;
	struct rb_bios_settings	*bs;
	u8 *base;
	u32 off,len;
	unsigned long ret;

	ret = MACH_ADM5120_UNKNOWN;

	base = (u8 *)KSEG1ADDR(ADM5120_SRAM0_BASE);
	bs = (struct rb_bios_settings *)(base + RB_BS_OFFS);

	off = read_le32(&bs->hs_offs);
	len = read_le32(&bs->hs_size);
	if (off > RB_OFFS_MAX)
		return ret;

	memset(&hs, 0, sizeof(hs));
	if (routerboot_load_hs(base+off, len, &hs) != 0)
		return ret;

	/* assume RouterBOOT as the boot-loader */
	adm5120_boot_loader = BOOT_LOADER_ROUTERBOOT;

	if (hs.name == NULL)
		return ret;

	for (board = routerboards; board->mach_type != MACH_ADM5120_UNKNOWN;
		board++) {
		if (strncmp(board->name, hs.name, strlen(board->name)) == 0) {
			ret = board->mach_type;
			break;
		}
	}

	return ret;
}

/*
 * ZyNOS based boards
 */
static inline u32 zynos_dbgarea_present(u8 *data)
{
	u32 t;

	t = read_be32(data+5);
	if (t != ZYNOS_MAGIC_DBGAREA1)
		return 0;

	t = read_be32(data+9);
	if (t != ZYNOS_MAGIC_DBGAREA2)
		return 0;

	return 1;
}

#define CHECK_VENDOR(n) (strnicmp(vendor,(n),strlen(n)) == 0)

static inline unsigned int zynos_get_vendor_id(struct zynos_board_info *info)
{
	unsigned char vendor[ZYNOS_NAME_LEN];
	int i;

	for (i=0; i<ZYNOS_NAME_LEN; i++)
		vendor[i]=info->vendor[i];

	if CHECK_VENDOR(ZYNOS_VENDOR_ZYXEL)
		return ZYNOS_VENDOR_ID_ZYXEL;
#if 0
	/* TODO: there are no known ADM5120 based boards from other vendors */
	if CHECK_VENDOR(ZYNOS_VENDOR_DLINK)
		return ZYNOS_VENDOR_ID_DLINK;

	if CHECK_VENDOR(ZYNOS_VENDOR_LUCENT)
		return ZYNOS_VENDOR_ID_LUCENT;

	if CHECK_VENDOR(ZYNOS_VENDOR_NETGEAR)
		return ZYNOS_VENDOR_ID_NETGEAR;
#endif

	return ZYNOS_VENDOR_ID_OTHER;
}

static inline u16 zynos_get_board_id(struct zynos_board_info *info)
{
	return read_be16(&info->board_id);
}

static inline u32 zynos_get_bootext_addr(struct zynos_board_info *info)
{
	return read_be32(&info->bootext_addr);
}


#define ZYNOS_INFO_ADDR		KSEG1ADDR(ADM5120_SRAM0_BASE+0x3F90)
#define ZYNOS_HDBG_ADDR		KSEG1ADDR(ADM5120_SRAM0_BASE+0x4000)
#define BOOTEXT_ADDR_MIN	KSEG1ADDR(ADM5120_SRAM0_BASE)
#define BOOTEXT_ADDR_MAX	(BOOTEXT_ADDR_MIN + (2*1024*1024))

static unsigned long __init zynos_detect_board(void)
{
	struct zynos_board_info *info;
	struct zynos_board *board;
	unsigned int vendor_id;
	u16	board_id;
	u32	t;
	unsigned long ret;

	ret = MACH_ADM5120_UNKNOWN;
	/* check presence of the dbgarea */
	if (zynos_dbgarea_present((u8 *)ZYNOS_HDBG_ADDR) == 0)
		goto out;

	info = (struct zynos_board_info *)(ZYNOS_INFO_ADDR);

	/* check for a valid BootExt address */
	t = zynos_get_bootext_addr(info);
	if ((t < BOOTEXT_ADDR_MIN) || (t > BOOTEXT_ADDR_MAX))
		goto out;

	vendor_id = zynos_get_vendor_id(info);
	board_id = zynos_get_board_id(info);

	for (board = zynos_boards; board->mach_type != MACH_ADM5120_UNKNOWN;
		board++) {
		if ((board->vendor_id == vendor_id) &&
			(board->board_id == board_id)) {
			ret = board->mach_type;
			break;
		}
	}

	/* assume Bootbase as the boot-loader */
	adm5120_boot_loader = BOOT_LOADER_BOOTBASE;

out:
	return ret;
}

/*
 * U-Boot based boards
 */
static int __init uboot_present(void)
{
	/* FIXME: not yet implemented */
	return 0;
}

static unsigned long __init uboot_detect_board(void)
{
	/* FIXME: not yet implemented */
	return MACH_ADM5120_UNKNOWN;
}

static void __init adm5120_detect_board(void)
{
	struct adm5120_board *board;
	unsigned long t;

	adm5120_boot_loader = BOOT_LOADER_UNKNOWN;

	/* Try to detect board type without bootloader */
	t = routerboot_detect_board();

	if (t == MACH_ADM5120_UNKNOWN)
		t = zynos_detect_board();

	if (t == MACH_ADM5120_UNKNOWN)
		t = myloader_detect_board();

	/* Try to detect bootloader type */
	if (cfe_present()) {
		adm5120_boot_loader = BOOT_LOADER_CFE;
		if (t == MACH_ADM5120_UNKNOWN)
			t = cfe_detect_board();
	} else if (uboot_present()) {
		adm5120_boot_loader = BOOT_LOADER_UBOOT;
		if (t == MACH_ADM5120_UNKNOWN)
			t = uboot_detect_board();
	}

	for (board = adm5120_boards; board->mach_type != MACH_ADM5120_UNKNOWN;
		board++) {
		if (board->mach_type == t)
			break;
	}

	memcpy(&adm5120_board, board, sizeof(adm5120_board));
}

#define SWITCH_READ(r) *(u32 *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+(r))
#define SWITCH_WRITE(r,v) *(u32 *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+(r))=(v)

/*
 * CPU settings detection
 */
#define CODE_GET_PC(c)		((c) & CODE_PC_MASK)
#define CODE_GET_REV(c)		(((c) >> CODE_REV_SHIFT) & CODE_REV_MASK)
#define CODE_GET_PK(c)		(((c) >> CODE_PK_SHIFT) & CODE_PK_MASK)
#define CODE_GET_CLKS(c)	(((c) >> CODE_CLKS_SHIFT) & CODE_CLKS_MASK)
#define CODE_GET_NAB(c)		(((c) & CODE_NAB) != 0)

static void __init adm5120_detect_cpuinfo(void)
{
	u32 code;
	u32 clks;

	code = SWITCH_READ(SWITCH_REG_CODE);

	adm5120_product_code = CODE_GET_PC(code);
	adm5120_revision = CODE_GET_REV(code);
	adm5120_package = (CODE_GET_PK(code) == CODE_PK_BGA) ?
		ADM5120_PACKAGE_BGA : ADM5120_PACKAGE_PQFP;
	adm5120_nand_boot = CODE_GET_NAB(code);

	clks = CODE_GET_CLKS(code);
	adm5120_speed = ADM5120_SPEED_175;
	if (clks & 1)
		adm5120_speed += 25000000;
	if (clks & 2)
		adm5120_speed += 50000000;
}

#if 1
#  define mem_dbg(f, ...)	prom_printf("mem_detect: " f, ## __VA_ARGS__)
extern void prom_printf(char *, ...);
#else
#  define mem_dbg(f, ...)
#endif

static void __init adm5120_detect_memsize(void)
{
	u32	memctrl;
	u32	size, maxsize;
	volatile u8	*p,*r;
	u8	t;

	memctrl = SWITCH_READ(SWITCH_REG_MEMCTRL);
	switch (memctrl & MEMCTRL_SDRS_MASK) {
	case MEMCTRL_SDRS_4M:
		maxsize = 4 << 20;
		break;
	case MEMCTRL_SDRS_8M:
		maxsize = 8 << 20;
		break;
	case MEMCTRL_SDRS_16M:
		maxsize = 16 << 20;
		break;
	default:
		maxsize = 64 << 20;
		break;
	}

	/* FIXME: need to disable buffers for both SDRAM bank? */

	mem_dbg("checking for %ldMB chip\n",maxsize >> 20);

	/* detect size of the 1st SDRAM bank */
	p = (volatile u8 *)KSEG1ADDR(0);
	t = *p;
	for (size = 2<<20; size <= (maxsize >> 1); size <<= 1) {
#if 1
		r = (p+size);
		*p = 0x55;
		mem_dbg("1st pattern at 0x%lx is 0x%02x\n", size, *r);
		if (*r == 0x55) {
			*p = 0xAA;
			mem_dbg("2nd pattern at 0x%lx is 0x%02x\n", size, *r);
			if (*r == 0xAA) {
				/* mirrored address */
				mem_dbg("mirrored data found at 0x%lx\n", size);
				break;
			}
		}
#else
		p[0] = 0x55;
		mem_dbg("1st pattern at 0x%lx is 0x%02x\n", size, p[size]);
		if (p[size] != 0x55)
			continue;

		p[0] = 0xAA;
		mem_dbg("2nd pattern at 0x%lx is 0x%02x\n", size, p[size]);
		if (p[size] != 0xAA)
			continue;

		/* mirrored address */
		mem_dbg("mirrored data found at 0x%lx\n", size);
		break;
#endif
	}
	*p = t;

	mem_dbg("%ldMB chip found\n", size >> 20);

	if (size == (32 << 20))
		/* if bank size is 32MB, 2nd bank is not supported */
		goto out;

	if ((memctrl & MEMCTRL_SDR1_ENABLE) == 0)
		/* if 2nd bank is not enabled, we are done */
		goto out;

	/*
	 * some bootloaders enable 2nd bank, even if the 2nd SDRAM chip
	 * are missing.
	 */
	mem_dbg("checking second bank\n");
	p += (maxsize+size)-1;
	t = *p;
	*p = 0x55;
	if (*p != 0x55)
		goto out;

	*p = 0xAA;
	if (*p != 0xAA)
		goto out;

	*p = t;
	if (maxsize != size) {
		/* adjusting MECTRL register */
		memctrl &= ~(MEMCTRL_SDRS_MASK);
		switch (size>>20) {
		case 4:
			memctrl |= MEMCTRL_SDRS_4M;
			break;
		case 8:
			memctrl |= MEMCTRL_SDRS_8M;
			break;
		case 16:
			memctrl |= MEMCTRL_SDRS_16M;
			break;
		default:
			memctrl |= MEMCTRL_SDRS_64M;
			break;
		}
		SWITCH_WRITE(SWITCH_REG_MEMCTRL, memctrl);
	}
	size <<= 1;

out:
	adm5120_memsize = size;
	mem_dbg("%ldMB memory found\n",size>>20);
}

void __init adm5120_info_show(void)
{
	/* FIXME: move this somewhere else */
	printk("ADM%04X%s revision %d, running at %ldMHz\n",
		adm5120_product_code,
		(adm5120_package == ADM5120_PACKAGE_BGA) ? "" : "P",
		adm5120_revision, (adm5120_speed / 1000000)
		);
	printk("Boot loader is: %s\n", boot_loader_names[adm5120_boot_loader]);
	printk("Booted from   : %s flash\n", adm5120_nand_boot ? "NAND":"NOR");
	printk("Board is      : %s\n", adm5120_board_name());
	printk("Memory size   : %ldMB\n", adm5120_memsize >> 20);
}

void __init adm5120_swab_test(void)
{
#if CONFIG_ADM5120_HARDWARE_SWAB
	u32	t1,t2;

	t1 = 0x1234;
	t2 = __arch__swab16(t1);
	printk("hardware swab16 test %s, data:0x%04X, result:0x%04X\n",
		(t2 == 0x3412) ? "passed" :"failed", t1, t2);

	t1 = 0x12345678;
	t2 = __arch__swab32(t1);
	printk("hardware swab32 test %s, data:0x%08X, result:0x%08X\n",
		(t2 == 0x78563412) ? "passed" :"failed", t1, t2);

#endif /* CONFIG_ADM5120_HARDWARE_SWAB */
}

void __init adm5120_info_init(void)
{

	adm5120_detect_cpuinfo();
	adm5120_detect_memsize();
	adm5120_detect_board();

	adm5120_info_show();
	adm5120_swab_test();
}
