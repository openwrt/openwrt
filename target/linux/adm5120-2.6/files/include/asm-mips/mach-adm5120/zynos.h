/*
 *  $Id$
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef _ZYNOS_H
#define _ZYNOS_H

#define ZYNOS_NAME_LEN		32
#define ZYNOS_FEAT_BYTES	22
#define ZYNOS_MAC_LEN		6

struct zynos_board_info {
	unsigned char	vendor[ZYNOS_NAME_LEN];
	unsigned char	product[ZYNOS_NAME_LEN];
	u32		bootext_addr;
	u32		res0;
	u16		board_id;
	u8		res1[6];
	u8		feat_other[ZYNOS_FEAT_BYTES];
	u8		feat_main;
	u8		res2;
	u8		mac[ZYNOS_MAC_LEN];
	u8		country;
	u8		dbgflag;
} __attribute__ ((packed));

/*
 * Vendor names
 */
#define ZYNOS_VENDOR_ZYXEL	"ZyXEL"
#define ZYNOS_VENDOR_LUCENT	"LUCENT"
#define ZYNOS_VENDOR_NETGEAR	"D-Link"

/*
 * Board IDs (big-endian)
 */
#define ZYNOS_BOARD_HS100	0x9FF1	/* HomeSafe 100/100W */
#define ZYNOS_BOARD_P334	0x9FF5	/* Prestige 334 */
#define ZYNOS_BOARD_P334U	0x9FDD	/* Prestige 334U */
#define ZYNOS_BOARD_P334W	0x9FF3	/* Prestige 334W */
#define ZYNOS_BOARD_P334WH	0x00E0	/* Prestige 334WH */
#define ZYNOS_BOARD_P334WHD	0x00E1	/* Prestige 334WHD */
#define ZYNOS_BOARD_P334WT	0x9FEF	/* Prestige 334WT */
#define ZYNOS_BOARD_P335	0x9FED	/* Prestige 335/335WT */
#define ZYNOS_BOARD_P335PLUS	0x0025	/* Prestige 335Plus */
#define ZYNOS_BOARD_P335U	0x9FDC	/* Prestige 335U */

#endif /* _ZYNOS_H */
