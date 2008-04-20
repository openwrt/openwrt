/*
 * Marvell 88E6060 switch driver
 * Copyright (c) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation
 */
#ifndef __MVSWITCH_H
#define __MVSWITCH_H

#define MV_PORTS	5
#define MV_WANPORT	4
#define MV_CPUPORT	5

#define MV_BASE		0x10

#define MV_PHYPORT_BASE		(MV_BASE + 0x0)
#define MV_PHYPORT(_n)		(MV_PHYPORT_BASE + (_n))
#define MV_SWITCHPORT_BASE	(MV_BASE + 0x8)
#define MV_SWITCHPORT(_n)	(MV_SWITCHPORT_BASE + (_n))
#define MV_SWITCHREGS		(MV_BASE + 0xf)

enum {
	MV_PHY_CONTROL      = 0x00,
	MV_PHY_STATUS       = 0x01,
	MV_PHY_IDENT0       = 0x02,
	MV_PHY_IDENT1       = 0x03,
	MV_PHY_ANEG         = 0x04,
	MV_PHY_LINK_ABILITY = 0x05,
	MV_PHY_ANEG_EXPAND  = 0x06,
	MV_PHY_XMIT_NEXTP   = 0x07,
	MV_PHY_LINK_NEXTP   = 0x08,
	MV_PHY_CONTROL1     = 0x10,
	MV_PHY_STATUS1      = 0x11,
	MV_PHY_INTR_EN      = 0x12,
	MV_PHY_INTR_STATUS  = 0x13,
	MV_PHY_INTR_PORT    = 0x14,
	MV_PHY_RECV_COUNTER = 0x15,
	MV_PHY_LED_PARALLEL = 0x16,
	MV_PHY_LED_STREAM   = 0x17,
	MV_PHY_LED_CTRL     = 0x18,
	MV_PHY_LED_OVERRIDE = 0x19,
	MV_PHY_VCT_CTRL     = 0x1a,
	MV_PHY_VCT_STATUS   = 0x1b,
	MV_PHY_CONTROL2     = 0x1e
};
#define MV_PHYREG(_type, _port) MV_PHYPORT(_port), MV_PHY_##_type

enum {
	MV_PORT_STATUS      = 0x00,
	MV_PORT_IDENT       = 0x03,
	MV_PORT_CONTROL     = 0x04,
	MV_PORT_VLANMAP     = 0x06,
	MV_PORT_ASSOC       = 0x0b,
	MV_PORT_RXCOUNT     = 0x10,
	MV_PORT_TXCOUNT     = 0x11,
};
#define MV_PORTREG(_type, _port) MV_SWITCHPORT(_port), MV_PORT_##_type

enum {
	MV_PORTCTRL_BLOCK   =  (1 << 0),
	MV_PORTCTRL_LEARN   =  (2 << 0),
	MV_PORTCTRL_ENABLED =  (3 << 0),
	MV_PORTCTRL_VLANTUN =  (1 << 7),	/* Enforce VLANs on packets */
	MV_PORTCTRL_RXTR    =  (1 << 8),	/* Enable Marvell packet trailer for ingress */
	MV_PORTCTRL_TXTR    = (1 << 14),	/* Enable Marvell packet trailer for egress */
	MV_PORTCTRL_FORCEFL = (1 << 15),	/* force flow control */
};

#define MV_PORTVLAN_ID(_n) (((_n) & 0xf) << 12)
#define MV_PORTVLAN_PORTS(_n) ((_n) & 0x3f)

#define MV_PORTASSOC_PORTS(_n) ((_n) & 0x1f)
#define MV_PORTASSOC_MONITOR	(1 << 15)

enum {
	MV_SWITCH_MAC0      = 0x01,
	MV_SWITCH_MAC1      = 0x02,
	MV_SWITCH_MAC2      = 0x03,
	MV_SWITCH_CTRL      = 0x04,
	MV_SWITCH_ATU_CTRL  = 0x0a,
	MV_SWITCH_ATU_OP    = 0x0b,
	MV_SWITCH_ATU_DATA  = 0x0c,
	MV_SWITCH_ATU_MAC0  = 0x0d,
	MV_SWITCH_ATU_MAC1  = 0x0e,
	MV_SWITCH_ATU_MAC2  = 0x0f,
};
#define MV_SWITCHREG(_type) MV_SWITCHREGS, MV_SWITCH_##_type

enum {
#define MV_ATUCTL_AGETIME(_n)	((((_n) / 16) & 0xff) << 4)
	MV_ATUCTL_ATU_256   = (0 << 12),
	MV_ATUCTL_ATU_512   = (1 << 12),
	MV_ATUCTL_ATU_1K	= (2 << 12),
	MV_ATUCTL_ATUMASK   = (3 << 12),
	MV_ATUCTL_NO_LEARN  = (1 << 14),
	MV_ATUCTL_RESET     = (1 << 15),
}

#define MV_IDENT_MASK		0xfff0
#define MV_IDENT_VALUE		0x0600

#endif
