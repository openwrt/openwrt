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

struct adm5120_info {
	unsigned int product_code;
	unsigned int revision;
	unsigned int cpu_package;
	unsigned int nand_boot;
	unsigned long cpu_speed;
	unsigned int boot_loader;
	unsigned int board_type;
	unsigned int iface_num;
	unsigned int has_usb;
};

#define CPU_SPEED_175		175000000
#define CPU_SPEED_200		200000000
#define CPU_SPEED_225		225000000
#define CPU_SPEED_250		250000000

#define CPU_PACKAGE_PQFP	0
#define CPU_PACKAGE_BGA		1

#define BOOT_LOADER_UNKNOWN	0
#define BOOT_LOADER_CFE		1
#define BOOT_LOADER_UBOOT	2
#define BOOT_LOADER_MYLOADER	3
#define BOOT_LOADER_ROUTERBOOT	4
#define BOOT_LOADER_LAST	4	

#define BOARD_TYPE_UNKNOWN	0
#define BOARD_TYPE_WP54G_WRT	1
#define BOARD_TYPE_WP54G	2
#define BOARD_TYPE_WP54AG	3
#define BOARD_TYPE_WPP54G	4
#define BOARD_TYPE_WPP54AG	5
#define BOARD_TYPE_NP28G	6
#define BOARD_TYPE_NP28GHS	7
#define BOARD_TYPE_NP27G	8
#define BOARD_TYPE_WP54Gv1C	9
#define BOARD_TYPE_RB_111	10
#define BOARD_TYPE_RB_112	11
#define BOARD_TYPE_RB_133	12
#define BOARD_TYPE_RB_133C	13
#define BOARD_TYPE_RB_150	14
#define BOARD_TYPE_RB_153	15
#define BOART_TYPE_LAST		15

extern struct adm5120_info adm5120_info;
extern void adm5120_info_init(void);

static inline int adm5120_has_pci(void)
{
	return (adm5120_info.cpu_package == CPU_PACKAGE_BGA);
}

#endif /* _ADM5120_INFO_H */
