/*
 *  Copyright (C) 2007-2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#ifndef _MACH_ADM5120_INFO_H
#define _MACH_ADM5120_INFO_H

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

#define MACH_ADM5120_GENERIC	0	/* Generic board */
#define MACH_ADM5120_WP54G_WRT	1	/* Compex WP54G-WRT */
#define MACH_ADM5120_WP54	2	/* Compex WP54G/WP54AG/WPP54G/WPP54AG */
#define MACH_ADM5120_NP28G	3	/* Compex NP28G */
#define MACH_ADM5120_NP28GHS	4	/* Compex NP28G HotSpot */
#define MACH_ADM5120_NP27G	5	/* Compex NP27G */
#define MACH_ADM5120_WP54Gv1C	6	/* Compex WP54G version 1C */
#define MACH_ADM5120_RB_11X	7	/* Mikrotik RouterBOARD 111/112 */
#define MACH_ADM5120_RB_133	8	/* Mikrotik RouterBOARD 133 */
#define MACH_ADM5120_RB_133C	9	/* Mikrotik RouterBOARD 133c */
#define MACH_ADM5120_RB_150	10	/* Mikrotik RouterBOARD 150 */
#define MACH_ADM5120_RB_153	11	/* Mikrotik RouterBOARD 153 */
#define MACH_ADM5120_RB_192	12	/* Mikrotik RouterBOARD 192 */
#define MACH_ADM5120_HS100	13	/* ZyXEL HomeSafe 100/100W */
#define MACH_ADM5120_P334U	14	/* ZyXEL Prestige 334U */
#define MACH_ADM5120_P334W	15	/* ZyXEL Prestige 334W */
#define MACH_ADM5120_P334WH	16	/* ZyXEL Prestige 334WH */
#define MACH_ADM5120_P334WHD	17	/* ZyXEL Prestige 334WHD */
#define MACH_ADM5120_P334WT	18	/* ZyXEL Prestige 334WT */
#define MACH_ADM5120_P335	19	/* ZyXEL Prestige 335/335WT */
#define MACH_ADM5120_P335PLUS	20	/* ZyXEL Prestige 335Plus */
#define MACH_ADM5120_P335U	21	/* ZyXEL Prestige 335U */
#define MACH_ADM5120_ES2108	22	/* ZyXEL Ethernet Switch 2108 */
#define MACH_ADM5120_ES2108F	23	/* ZyXEL Ethernet Switch 2108-F */
#define MACH_ADM5120_ES2108G	24	/* ZyXEL Ethernet Switch 2108-G */
#define MACH_ADM5120_ES2108LC	25	/* ZyXEL Ethernet Switch 2108-LC */
#define MACH_ADM5120_ES2108PWR	26	/* ZyXEL Ethernet Switch 2108-PWR */
#define MACH_ADM5120_ES2024A	27	/* ZyXEL Ethernet Switch 2024A */
#define MACH_ADM5120_ES2024PWR	28	/* ZyXEL Ethernet Switch 2024PWR */
#define MACH_ADM5120_CAS630	29	/* Cellvision CAS-630/630W */
#define MACH_ADM5120_CAS670	30	/* Cellvision CAS-670/670W */
#define MACH_ADM5120_CAS700	31	/* Cellvision CAS-700/700W */
#define MACH_ADM5120_CAS771	32	/* Cellvision CAS-771/771W */
#define MACH_ADM5120_CAS790	33	/* Cellvision CAS-790 */
#define MACH_ADM5120_CAS861	34	/* Cellvision CAS-861/861W */
#define MACH_ADM5120_NFS101U	35	/* Cellvision NFS-101U/101WU */
#define MACH_ADM5120_NFS202U	36	/* Cellvision NFS-202U/202WU */
#define MACH_ADM5120_EASY5120PATA 37	/* Infineon EASY 5120P-ATA */
#define MACH_ADM5120_EASY5120RT 38	/* Infineon EASY 5120-RT */
#define MACH_ADM5120_EASY5120WVOIP 39	/* Infineon EASY 5120-WVoIP */
#define MACH_ADM5120_EASY83000	40	/* Infineon EASY-83000 */
#define MACH_ADM5120_BR6104K	41	/* Edimax BR-6104K */
#define MACH_ADM5120_BR6104KP	42	/* Edimax BR-6104KP */
#define MACH_ADM5120_BR61X4WG	43	/* Edimax BR-6104Wg/BR-6114WG */
#define MACH_ADM5120_PMUGW	44	/* Motorola Powerline MU Gateway */
#define MACH_ADM5120_5GXI	45	/* OSBRiDGE 5GXi/5XLi */

/*
 * TODO:remove adm5120_eth* variables when the switch driver will be
 * 	converted into a real platform driver
 */
extern unsigned int adm5120_eth_num_ports;
extern unsigned char adm5120_eth_macs[6][6];
extern unsigned char adm5120_eth_vlans[6];

extern void adm5120_soc_init(void) __init;
extern void adm5120_mem_init(void) __init;
extern void adm5120_ndelay(u32 ns);

extern void (*adm5120_board_reset)(void);

extern void adm5120_gpio_init(void) __init;
extern void adm5120_gpio_csx0_enable(void) __init;
extern void adm5120_gpio_csx1_enable(void) __init;
extern void adm5120_gpio_ew_enable(void) __init;

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

#endif /* _MACH_ADM5120_INFO_H */
