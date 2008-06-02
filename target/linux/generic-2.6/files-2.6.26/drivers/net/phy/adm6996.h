/*
 * ADM6996 switch driver
 *
 * Copyright (c) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation
 */
#ifndef __ADM6996_H
#define __ADM6996_H

#define ADM_PHY_PORTS	5
#define ADM_CPU_PORT	5
#define ADM_WAN_PORT	0 /* FIXME: dynamic ? */

enum admreg {
	ADM_EEPROM_BASE		= 0x0,
		ADM_P0_CFG		= ADM_EEPROM_BASE + 1,
		ADM_P1_CFG		= ADM_EEPROM_BASE + 3,
		ADM_P2_CFG		= ADM_EEPROM_BASE + 5,
		ADM_P3_CFG		= ADM_EEPROM_BASE + 7,
		ADM_P4_CFG		= ADM_EEPROM_BASE + 8,
		ADM_P5_CFG		= ADM_EEPROM_BASE + 9,
	ADM_EEPROM_EXT_BASE	= 0x40,
	ADM_COUNTER_BASE	= 0xa0,
		ADM_SIG0		= ADM_COUNTER_BASE + 0,
		ADM_SIG1		= ADM_COUNTER_BASE + 1,
	ADM_PHY_BASE		= 0x200,
#define ADM_PHY_PORT(n) (ADM_PHY_BASE + (0x20 * n))
};

/* Chip identification patterns */
#define	ADM_SIG0_MASK	0xfff0
#define ADM_SIG0_VAL	0x1020
#define ADM_SIG1_MASK	0xffff
#define ADM_SIG1_VAL	0x0007

enum {
	ADM_PHYCFG_COLTST     = (1 << 7),	/* Enable collision test */
	ADM_PHYCFG_DPLX       = (1 << 8),	/* Enable full duplex */
	ADM_PHYCFG_ANEN_RST   = (1 << 9),	/* Restart auto negotiation (self clear) */
	ADM_PHYCFG_ISO        = (1 << 10),	/* Isolate PHY */
	ADM_PHYCFG_PDN        = (1 << 11),	/* Power down PHY */
	ADM_PHYCFG_ANEN       = (1 << 12),	/* Enable auto negotiation */
	ADM_PHYCFG_SPEED_100  = (1 << 13),	/* Enable 100 Mbit/s */
	ADM_PHYCFG_LPBK       = (1 << 14),	/* Enable loopback operation */
	ADM_PHYCFG_RST        = (1 << 15),	/* Reset the port (self clear) */
	ADM_PHYCFG_INIT = (
		ADM_PHYCFG_RST |
		ADM_PHYCFG_SPEED_100 |
		ADM_PHYCFG_ANEN |
		ADM_PHYCFG_ANEN_RST
	)
};

enum {
	ADM_PORTCFG_FC        = (1 << 0),	/* Enable 802.x flow control */
	ADM_PORTCFG_AN        = (1 << 1),	/* Enable auto-negotiation */
	ADM_PORTCFG_SPEED_100 = (1 << 2),	/* Enable 100 Mbit/s */
	ADM_PORTCFG_DPLX      = (1 << 3),	/* Enable full duplex */
	ADM_PORTCFG_OT        = (1 << 4),	/* Output tagged packets */
	ADM_PORTCFG_PD        = (1 << 5),	/* Port disable */
	ADM_PORTCFG_TV_PRIO   = (1 << 6),	/* 0 = VLAN based priority
	                                 	 * 1 = TOS based priority */
	ADM_PORTCFG_PPE       = (1 << 7),	/* Port based priority enable */
	ADM_PORTCFG_PP_S      = (1 << 8),	/* Port based priority, 2 bits */
	ADM_PORTCFG_PVID_BASE = (1 << 10),	/* Primary VLAN id, 4 bits */
	ADM_PORTCFG_FSE	      = (1 << 14),	/* Fx select enable */
	ADM_PORTCFG_CAM       = (1 << 15),	/* Crossover Auto MDIX */

	ADM_PORTCFG_INIT = (
		ADM_PORTCFG_FC |
		ADM_PORTCFG_AN |
		ADM_PORTCFG_SPEED_100 |
		ADM_PORTCFG_DPLX |
		ADM_PORTCFG_CAM
	),
	ADM_PORTCFG_CPU = (
		ADM_PORTCFG_FC |
		ADM_PORTCFG_SPEED_100 |
		ADM_PORTCFG_OT |
		ADM_PORTCFG_DPLX
	),
};

#define ADM_PORTCFG_PPID(N) ((n & 0x3) << 8)
#define ADM_PORTCFG_PVID(n) ((n & 0xf) << 10)

static const u8 adm_portcfg[] = {
	[0] = ADM_P0_CFG,
	[1] = ADM_P1_CFG,
	[2] = ADM_P2_CFG,
	[3] = ADM_P3_CFG,
	[4] = ADM_P4_CFG,
	[5] = ADM_P5_CFG,
};

/*
 * Split the register address in phy id and register
 * it will get combined again by the mdio bus op
 */
#define PHYADDR(_reg)	((_reg >> 5) & 0xff), (_reg & 0x1f)

#endif
