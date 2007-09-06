/*
 *  $Id$
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
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
 */

#ifndef _ADM5120_INFO_H
#define _ADM5120_INFO_H

#include <linux/types.h>

extern unsigned int adm5120_prom_type;
#define ADM5120_PROM_GENERIC	0
#define ADM5120_PROM_CFE	1
#define ADM5120_PROM_MYLOADER	2
#define ADM5120_PROM_ROUTERBOOT	3
#define ADM5120_PROM_BOOTBASE	4
#define ADM5120_PROM_UBOOT	5
#define ADM5120_PROM_LAST	5

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

/*
 * TODO:remove adm5120_eth* variables when the switch driver will be
 * 	converted into a real platform driver
 */
extern unsigned int adm5120_eth_num_ports;
extern unsigned char adm5120_eth_macs[6][6];
extern unsigned char adm5120_eth_vlans[6];

extern void adm5120_soc_init(void) __init;
extern void adm5120_mem_init(void) __init;
extern void adm5120_time_init(void) __init;
extern void adm5120_ndelay(u32 ns);

extern void adm5120_restart(char *command);
extern void adm5120_halt(void);
extern void adm5120_power_off(void);

extern void (*adm5120_board_reset)(void);

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

#endif /* _ADM5120_INFO_H */
