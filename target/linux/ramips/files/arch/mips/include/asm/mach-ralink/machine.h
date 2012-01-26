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
	RAMIPS_MACH_WLI_TX4_AG300N,	/* Buffalo WLI-TX4-AG300N */
	RAMIPS_MACH_WZR_AGL300NH,	/* Buffalo WZR-AGL300NH */

	/* RT3050 based machines */
	RAMIPS_MACH_ALL0256N,		/* Allnet ALL0256N */
	RAMIPS_MACH_DIR_300_B1,		/* D-Link DIR-300 B1 */
	RAMIPS_MACH_DIR_600_B1,		/* D-Link DIR-600 B1 */
	RAMIPS_MACH_DIR_600_B2,		/* D-Link DIR-600 B2 */
	RAMIPS_MACH_RT_G32_B1,		/* Asus RT-G32 B1 */
	RAMIPS_MACH_NW718,		/* Netcore NW718 */

	/* RT3052 based machines */
	RAMIPS_MACH_ARGUS_ATP52B,	/* Argus ATP-52B */
	RAMIPS_MACH_BC2,		/* NexAira BC2 */
	RAMIPS_MACH_ESR_9753,		/* Senao / EnGenius ESR-9753*/
	RAMIPS_MACH_F5D8235_V2,         /* Belkin F5D8235 v2 */
	RAMIPS_MACH_FONERA20N,		/* La Fonera 2.0N */
	RAMIPS_MACH_HW550_3G,		/* Aztech HW550-3G */
	RAMIPS_MACH_MOFI3500_3GN,	/* MoFi Network MOFI3500-3GN */
	RAMIPS_MACH_NBG_419N,		/* ZyXEL NBG-419N */
	RAMIPS_MACH_OMNI_EMB,         	/* Omnima MiniEMBWiFi */
	RAMIPS_MACH_PWH2004,		/* Prolink 2004H / Abocom 5205 */
	RAMIPS_MACH_V22RW_2X2,		/* Ralink AP-RT3052-V22RW-2X2 */
	RAMIPS_MACH_W502U,		/* ALFA Networks W502U */
	RAMIPS_MACH_WCR150GN,		/* Sparklan WCR-150GN */
	RAMIPS_MACH_WHR_G300N,		/* Buffalo WHR-G300N */
	RAMIPS_MACH_WL341V3,		/* Sitecom WL-341 v3 */
	RAMIPS_MACH_WL351,		/* Sitecom WL-351 v1 002 */
	RAMIPS_MACH_WR512_3GN,		/* SH-WR512NU/WS-WR512N1-like 3GN*/
};
