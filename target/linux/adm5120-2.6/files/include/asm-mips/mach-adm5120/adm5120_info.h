/*
 * $Id$
 *
 * Copyright (C) 2007 OpenWrt.org
 * Copyright (C) Gabor Juhos <juhosg@freemail.hu>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef _ADM5120_INFO_H
#define _ADM5120_INFO_H

#include <linux/types.h>

#define ADM5120_BOARD_NAMELEN	64

struct adm5120_board {
	char		name[ADM5120_BOARD_NAMELEN];
	unsigned long	mach_type;
	unsigned int	iface_num;	/* Number of Ethernet interfaces */
	unsigned int	has_usb;	/* USB controller presence flag */
	u32		flash0_size;	/* Flash 0 size */
};

extern struct adm5120_board adm5120_board;

extern unsigned int adm5120_boot_loader;
#define BOOT_LOADER_UNKNOWN	0
#define BOOT_LOADER_CFE		1
#define BOOT_LOADER_UBOOT	2
#define BOOT_LOADER_MYLOADER	3
#define BOOT_LOADER_ROUTERBOOT	4
#define BOOT_LOADER_BOOTBASE	5
#define BOOT_LOADER_LAST	5

extern unsigned int adm5120_product_code;
extern unsigned int adm5120_revision;
extern unsigned int adm5120_nand_boot;

extern unsigned long adm5120_speed;
#define ADM5120_SPEED_175	175000000
#define ADM5120_SPEED_200	200000000
#define ADM5120_SPEED_225	225000000
#define ADM5120_SPEED_250	250000000

extern unsigned int adm5120_package;
#define ADM5120_PACKAGE_PQFP	0
#define ADM5120_PACKAGE_BGA	1

extern unsigned long adm5120_memsize;

extern void adm5120_info_init(void);

static inline int adm5120_package_pqfp(void)
{
	return (adm5120_package == ADM5120_PACKAGE_PQFP);
}

static inline int adm5120_package_bga(void)
{
	return (adm5120_package == ADM5120_PACKAGE_BGA);
}

static inline int adm5120_has_pci(void)
{
	return (adm5120_package == ADM5120_PACKAGE_BGA);
}

static inline int adm5120_has_gmii(void)
{
	return (adm5120_package == ADM5120_PACKAGE_BGA);
}

static inline char *adm5120_board_name(void)
{
	return adm5120_board.name;
}

#endif /* _ADM5120_INFO_H */
