/*
 *  Atheros AR71xx machine type definitions
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_MACHTYPE_H
#define _AR71XX_MACHTYPE_H

#include <asm/mips_machine.h>

enum ar71xx_mach_type {
	AR71XX_MACH_GENERIC = 0,
	AR71XX_MACH_AP81,	/* Atheros AP81 */
	AR71XX_MACH_AP83,	/* Atheros AP83 */
	AR71XX_MACH_AW_NR580,	/* AzureWave AW-NR580 */
	AR71XX_MACH_DIR_615_C1,	/* D-Link DIR-615 rev. C1 */
	AR71XX_MACH_DIR_825_B1,	/* D-Link DIR-825 rev. B1 */
	AR71XX_MACH_RB_411,	/* MikroTik RouterBOARD 411/411A/411AH */
	AR71XX_MACH_RB_411U,	/* MikroTik RouterBOARD 411U */
	AR71XX_MACH_RB_433,	/* MikroTik RouterBOARD 433/433AH */
	AR71XX_MACH_RB_433U,	/* MikroTik RouterBOARD 433UAH */
	AR71XX_MACH_RB_450,	/* MikroTik RouterBOARD 450 */
	AR71XX_MACH_RB_450G,	/* MikroTik RouterBOARD 450G */
	AR71XX_MACH_RB_493,	/* Mikrotik RouterBOARD 493/493AH */
	AR71XX_MACH_PB42,	/* Atheros PB42 */
	AR71XX_MACH_PB44,	/* Atheros PB44 */
	AR71XX_MACH_MZK_W04NU,	/* Planex MZK-W04NU */
	AR71XX_MACH_MZK_W300NH,	/* Planex MZK-W300NH */
	AR71XX_MACH_TEW_632BRP,	/* TRENDnet TEW-632BRP */
	AR71XX_MACH_TL_WR741ND,	/* TP-LINK TL-WR741ND */
	AR71XX_MACH_TL_WR841N_V1, /* TP-LINK TL-WR841N v1 */
	AR71XX_MACH_TL_WR941ND,	/* TP-LINK TL-WR941ND */
	AR71XX_MACH_TL_WR1043ND, /* TP-LINK TL-WR1041ND */
	AR71XX_MACH_UBNT_LSSR71, /* Ubiquiti LS-SR71 */
	AR71XX_MACH_UBNT_LSX,	/* Ubiquiti LSX */
	AR71XX_MACH_UBNT_RS,	/* Ubiquiti RouterStation */
	AR71XX_MACH_UBNT_AR71XX, /* Ubiquiti AR71xx-based board */
	AR71XX_MACH_UBNT_RSPRO,	/* Ubiquiti RouterStation Pro */
	AR71XX_MACH_UBNT_BULLET_M, /* Ubiquiti Bullet M */
	AR71XX_MACH_UBNT_ROCKET_M, /* Ubiquiti Rocket M */
	AR71XX_MACH_UBNT_NANO_M, /* Ubiquiti NanoStation M */
	AR71XX_MACH_WNDR3700,	/* NETGEAR WNDR3700 */
	AR71XX_MACH_WNR2000,	/* NETGEAR WNR2000 */
	AR71XX_MACH_WP543,	/* Compex WP543 */
	AR71XX_MACH_WRT160NL,	/* Linksys WRT160NL */
	AR71XX_MACH_WRT400N,	/* Linksys WRT400N */
};

#endif /* _AR71XX_MACHTYPE_H */
