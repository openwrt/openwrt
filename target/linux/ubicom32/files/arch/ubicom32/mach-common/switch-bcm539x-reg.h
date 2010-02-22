/*
 * arch/ubicom32/mach-common/switch-bcm539x-reg.h
 *   Broadcom switch definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

/*
 * Broadcom 53xx RoboSwitch device driver.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _SWITCH_BCM539X_REG_H_
#define _SWITCH_BCM539X_REG_H_

#define	BCM539X_CMD_READ		0x60
#define	BCM539X_CMD_WRITE		0x61

#define	BCM539X_GLOBAL_SPI_DATA0	0xf0

#define	BCM539X_GLOBAL_SPI_STATUS	0xfe
#define	BCM539X_GLOBAL_SPI_ST_SPIF	(1<<7)
#define	BCM539X_GLOBAL_SPI_ST_RACK	(1<<5)

#define	BCM539X_GLOBAL_PAGE		0xff

#define PAGE_PORT_TC			0x00		// Port Traffic Control Register

#define PAGE_QOS_CTL			0x30		// QoS Global Control Register
#define PAGE_QOS_TAG			0x34		// Default IEEE 802.1Q TAG Register

#define PAGE_MII_CTL_PORT0		0x10		// Internal PHY MII Register
#define PAGE_MII_CTL_PORT1		0x11
#define PAGE_MII_CTL_PORT2		0x12
#define PAGE_MII_CTL_PORT3		0x13
#define PAGE_MII_CTL_PORT4		0x14

#define PAGE_STATUS			0x01		// Status Register Page
#define PAGE_RATE_CONTROL		0x41		// Broadcast Storm Suppression Register

#define REG_GRATE_CONTROL		0x00

#define REG_LED_POWER			0x12

// Ingress Rate Control
#define REG_IRATE_CONTROLP0		0x10
#define REG_IRATE_CONTROLP1		0x14
#define REG_IRATE_CONTROLP2		0x18
#define REG_IRATE_CONTROLP3		0x1C
#define REG_IRATE_CONTROLP4		0x20
#define REG_IRATE_CONTROLP7		0x2C
#define REG_IRATE_CONTROLPI		0x30

// Egress Rate Control
#define REG_ERATE_CONTROLP0		0x80
#define REG_ERATE_CONTROLP1		0x82
#define REG_ERATE_CONTROLP2		0x84
#define REG_ERATE_CONTROLP3		0x86
#define REG_ERATE_CONTROLP4		0x88
#define REG_ERATE_CONTROLP5		0x8A
#define REG_ERATE_CONTROLP6		0x8C
#define REG_ERATE_CONTROLP7		0x8E
#define REG_ERATE_CONTROLPI		0x90

#define REG_LINK_STATUS			0x00

#define REG_TC_PORT0			0x00
#define REG_TC_PORT1			0x01
#define REG_TC_PORT2			0x02
#define REG_TC_PORT3			0x03
#define REG_TC_PORT4			0x04
#define REG_TC_PORT5			0x05

#define REG_SPEED_CTL			0x00
#define REG_SPEED_ADV100		0x08
#define REG_SPEED_ADV1000		0x12

#define REG_QOS_EN			0x00
#define REG_QOS_TAG_PORT1		0x12		// Default IEEE 802.1Q TAG, PORT 1
#define REG_QOS_TAG_PORT2		0x14		// Default IEEE 802.1Q TAG, PORT 2
#define REG_QOS_TAG_PORT3		0x16		// Default IEEE 802.1Q TAG, PORT 3
#define REG_QOS_TAG_PORT4		0x18		// Default IEEE 802.1Q TAG, PORT 4
#define REG_QOS_PID_PORT1		0x52		// Ingress Port Priority ID MAP, PORT 1
#define REG_QOS_PID_PORT2		0x54		// Ingress Port Priority ID MAP, PORT 2
#define REG_QOS_PID_PORT3		0x56		// Ingress Port Priority ID MAP, PORT 3
#define REG_QOS_PID_PORT4		0x58		// Ingress Port Priority ID MAP, PORT 4
#define REG_QOS_TXQ_CTL			0x80		// Tx Queue Control Register
#define REG_QOS_TXQ_WHTQ0		0x81		// Tx Queue Weight Register Queue 0
#define REG_QOS_TXQ_WHTQ1		0x82		// Tx Queue Weight Register Queue 1
#define REG_QOS_TXQ_WHTQ2		0x83		// Tx Queue Weight Register Queue 2
#define REG_QOS_TXQ_WHTQ3		0x84		// Tx Queue Weight Register Queue 3

#define REG_CTRL_PPSEL  		0x24		/* 5397: Protected port select register */

#define RATE_CONTROL_ENABLED		(1 << 22)
#define RATE_CONTROL_BSIZE		((1 << 10) | (1 << 9) | (1 << 8))

#define RATE_CONTROL_HIGH		((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4))
#define RATE_CONTROL_HIGH_N		~((1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))

#define RATE_CONTROL_MEDIUM		((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
#define RATE_CONTROL_MEDIUM_N		~((1 << 7))

#define RATE_CONTROL_NORMAL		((1 << 5) | (1 << 2) | (1 << 0))
#define RATE_CONTROL_NORMAL_N		~((1 << 7) | (1 << 6) | (1 << 4) | (1 << 3) | (1 << 1))

#define RATE_CONTROL_LOW		((1 << 4) | (1 << 3) | (1 << 0))
#define RATE_CONTROL_LOW_N		~((1 << 7) | (1 << 6) | (1 << 5) | (1 << 2) | (1 << 1))

// --- Gemtek, Configure the switch to support Ethernet Port QoS

/* MII access registers */
#define PSEUDO_PHYAD			0x1E	/* MII Pseudo PHY address */
#define REG_MII_PAGE			0x10	/* MII Page register */
#define REG_MII_ADDR			0x11	/* MII Address register */
#define REG_MII_DATA0			0x18	/* MII Data register 0 */
#define REG_MII_DATA1			0x19	/* MII Data register 1 */
#define REG_MII_DATA2			0x1a	/* MII Data register 2 */
#define REG_MII_DATA3			0x1b	/* MII Data register 3 */

/* Page numbers */
#define PAGE_CTRL			0x00	/* Control page */
#define PAGE_MMR			0x02	/* 5397 Management/Mirroring page */
#define PAGE_VTBL			0x05	/* ARL/VLAN Table access page */
#define PAGE_VLAN			0x34	/* VLAN page */

/* Control page registers */
#define REG_CTRL_PORT0			0x00	/* Port 0 traffic control register */
#define REG_CTRL_PORT1			0x01	/* Port 1 traffic control register */
#define REG_CTRL_PORT2			0x02	/* Port 2 traffic control register */
#define REG_CTRL_PORT3			0x03	/* Port 3 traffic control register */
#define REG_CTRL_PORT4			0x04	/* Port 4 traffic control register */
#define REG_CTRL_PORT5			0x05	/* Port 5 traffic control register */
#define REG_CTRL_PORT6			0x06	/* Port 6 traffic control register */
#define REG_CTRL_PORT7			0x07	/* Port 7 traffic control register */
#define REG_CTRL_MODE			0x0B	/* Switch Mode register */
#define REG_CTRL_MIIPO			0x0E	/* 5325: MII Port Override register */
#define REG_CTRL_SRST			0x79	/* Software reset control register */

#define REG_DEVICE_ID			0x30	/* 539x Device id: */
#define	DEVID5395			0x95	/*  5395 */
#define	DEVID5397			0x97	/*  5397 */
#define	DEVID5398			0x98	/*  5398 */
#define REG_REVISION_ID			0x40	/* 539x Revision id: */

/* VLAN page registers */
#define REG_VLAN_CTRL0			0x00	/* VLAN Control 0 register */
#define REG_VLAN_CTRL1			0x01	/* VLAN Control 1 register */
#define REG_VLAN_CTRL2			0x02	/* VLAN Control 2 register */
#define REG_VLAN_CTRL3			0x03	/* VLAN Control 3 register */
#define REG_VLAN_CTRL4			0x04	/* VLAN Control 4 register */
#define REG_VLAN_CTRL5			0x05	/* VLAN Control 5 register */
#define REG_VLAN_ACCESS			0x06	/* VLAN Table Access register */
#define REG_VLAN_WRITE			0x08	/* VLAN Write register */
#define REG_VLAN_READ			0x0C	/* VLAN Read register */
#define REG_VLAN_PTAG0			0x10	/* VLAN Default Port Tag register - port 0 */
#define REG_VLAN_PTAG1			0x12	/* VLAN Default Port Tag register - port 1 */
#define REG_VLAN_PTAG2			0x14	/* VLAN Default Port Tag register - port 2 */
#define REG_VLAN_PTAG3			0x16	/* VLAN Default Port Tag register - port 3 */
#define REG_VLAN_PTAG4			0x18	/* VLAN Default Port Tag register - port 4 */
#define REG_VLAN_PTAG5			0x1a	/* VLAN Default Port Tag register - port 5 */
#define REG_VLAN_PTAG6			0x1c	/* VLAN Default Port Tag register - port 6 */
#define REG_VLAN_PTAG7			0x1e	/* VLAN Default Port Tag register - port 7 */
#define REG_VLAN_PTAG8			0x20	/* 539x: VLAN Default Port Tag register - IMP port */
#define REG_VLAN_PMAP			0x20	/* 5325: VLAN Priority Re-map register */

/* ARL/VLAN Table Access page registers */
#define REG_VTBL_CTRL			0x00	/* ARL Read/Write Control */
#define REG_VTBL_MINDX			0x02	/* MAC Address Index */
#define REG_VTBL_VINDX			0x08	/* VID Table Index */
#define REG_VTBL_ARL_E0			0x10	/* ARL Entry 0 */
#define REG_VTBL_ARL_E1			0x18	/* ARL Entry 1 */
#define REG_VTBL_DAT_E0			0x18	/* ARL Table Data Entry 0 */
#define REG_VTBL_SCTRL			0x20	/* ARL Search Control */
#define REG_VTBL_SADDR			0x22	/* ARL Search Address */
#define REG_VTBL_SRES			0x24	/* ARL Search Result */
#define REG_VTBL_SREXT			0x2c	/* ARL Search Result */
#define REG_VTBL_VID_E0			0x30	/* VID Entry 0 */
#define REG_VTBL_VID_E1			0x32	/* VID Entry 1 */
#define REG_VTBL_PREG			0xFF	/* Page Register */
#define REG_VTBL_ACCESS			0x60	/* VLAN table access register */
#define REG_VTBL_INDX			0x61	/* VLAN table address index register */
#define REG_VTBL_ENTRY			0x63	/* VLAN table entry register */
#define REG_VTBL_ACCESS_5395		0x80	/* VLAN table access register */
#define REG_VTBL_INDX_5395		0x81	/* VLAN table address index register */
#define REG_VTBL_ENTRY_5395		0x83	/* VLAN table entry register */

/* SPI registers */
#define REG_SPI_PAGE			0xff	/* SPI Page register */

#endif
