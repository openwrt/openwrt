/*
 * Ralink machine types
 *
 * Copyright (C) 2010 Joonas Lahtinen <joonas.lahtinen@gmail.com>
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <asm/mips_machine.h>

enum ramips_mach_type {
	RAMIPS_MACH_GENERIC,
	/* RT2880 based machines */
	RAMIPS_MACH_F5D8235_V1,		/* Belkin F5D8235 v1 */
	RAMIPS_MACH_RT_N15,		/* Asus RT-N15 */
	RAMIPS_MACH_V11ST_FE,		/* Ralink V11ST-FE */
	RAMIPS_MACH_WLI_TX4_AG300N,	/* Buffalo WLI-TX4-AG300N */
	RAMIPS_MACH_WZR_AGL300NH,	/* Buffalo WZR-AGL300NH */

	/* RT3050 based machines */
	RAMIPS_MACH_3G_6200N,		/* Edimax 3G-6200N */
	RAMIPS_MACH_ALL0256N,		/* Allnet ALL0256N */
	RAMIPS_MACH_CARAMBOLA,		/* 8devices Carambola */
	RAMIPS_MACH_DIR_300_B1,		/* D-Link DIR-300 B1 */
	RAMIPS_MACH_DIR_600_B1,		/* D-Link DIR-600 B1 */
	RAMIPS_MACH_DIR_600_B2,		/* D-Link DIR-600 B2 */
	RAMIPS_MACH_DIR_615_D,		/* D-Link DIR-615 D */
	RAMIPS_MACH_DIR_620_A1,		/* D-Link DIR-620 A1 */
	RAMIPS_MACH_RT_G32_B1,		/* Asus RT-G32 B1 */
	RAMIPS_MACH_RT_N10_PLUS,	/* Asus RT-N10+ */
	RAMIPS_MACH_NW718,		/* Netcore NW718 */
	RAMIPS_MACH_WL_330N,		/* Asus WL-330N */
	RAMIPS_MACH_WL_330N3G,		/* Asus WL-330N3G */

	/* RT3052 based machines */
	RAMIPS_MACH_ALL0239_3G,		/* ALL0239-3G */
	RAMIPS_MACH_ARGUS_ATP52B,	/* Argus ATP-52B */
	RAMIPS_MACH_BC2,		/* NexAira BC2 */
	RAMIPS_MACH_BROADWAY,		/* Hauppauge Broadway */
	RAMIPS_MACH_DAP_1350,		/* D-Link DAP-1350 */
	RAMIPS_MACH_ESR_9753,		/* Senao / EnGenius ESR-9753*/
	RAMIPS_MACH_F5D8235_V2,         /* Belkin F5D8235 v2 */
	RAMIPS_MACH_FONERA20N,		/* La Fonera 2.0N */
	RAMIPS_MACH_RT_N13U,		/* ASUS RT-N13U */
	RAMIPS_MACH_FREESTATION5,	/* ARC Freestation5 */
	RAMIPS_MACH_HW550_3G,		/* Aztech HW550-3G */
	RAMIPS_MACH_MOFI3500_3GN,	/* MoFi Network MOFI3500-3GN */
	RAMIPS_MACH_NBG_419N,		/* ZyXEL NBG-419N */
	RAMIPS_MACH_OMNI_EMB,         	/* Omnima MiniEMBWiFi */
	RAMIPS_MACH_PSR_680W,		/* Petatel PSR-680W Wireless 3G Router*/
	RAMIPS_MACH_PWH2004,		/* Prolink 2004H / Abocom 5205 */
	RAMIPS_MACH_SL_R7205,		/* Skylink SL-R7205 Wireless 3G Router*/
	RAMIPS_MACH_V22RW_2X2,		/* Ralink AP-RT3052-V22RW-2X2 */
	RAMIPS_MACH_W306R_V20,	        /* Tenda W306R_V20 */
	RAMIPS_MACH_W502U,		/* ALFA Networks W502U */
	RAMIPS_MACH_WCR150GN,		/* Sparklan WCR-150GN */
	RAMIPS_MACH_WHR_G300N,		/* Buffalo WHR-G300N */
	RAMIPS_MACH_WL341V3,		/* Sitecom WL-341 v3 */
	RAMIPS_MACH_WL351,		/* Sitecom WL-351 v1 002 */
	RAMIPS_MACH_WR512_3GN,		/* SH-WR512NU/WS-WR512N1-like 3GN*/
	RAMIPS_MACH_WR6202,		/* Accton WR6202 */
	RAMIPS_MACH_XDXRN502J,          /* unknown XDX-RN502J */
	RAMIPS_MACH_UR_336UN,		/* UPVEL ROUTER */

	/* RT3352 based machines */
	RAMIPS_MACH_ALL5002,		/* Allnet ALL5002 */
	RAMIPS_MACH_DIR_615_H1,

	/* RT3662 based machines */
	RAMIPS_MACH_DIR_645,		/* D-Link DIR-645 */
	RAMIPS_MACH_RT_N56U,		/* Asus RT-N56U */

	/* RT3883 based machines */
	RAMIPS_MACH_TEW_691GR,		/* TRENDnet TEW-691GR */
	RAMIPS_MACH_TEW_692GR		/* TRENDnet TEW-692GR */
};
