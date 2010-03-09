/*
 * ar8216.h: AR8216 switch driver
 *
 * Copyright (C) 2009 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __AR8216_H
#define __AR8216_H

#define BITS(_s, _n)	(((1UL << (_n)) - 1) << _s)

#define AR8216_PORT_CPU	0
#define AR8216_NUM_PORTS	6
#define AR8216_NUM_VLANS	16

#define AR8216_REG_CTRL			0x0000
#define   AR8216_CTRL_REVISION		BITS(0, 8)
#define   AR8216_CTRL_REVISION_S	0
#define   AR8216_CTRL_VERSION		BITS(8, 8)
#define   AR8216_CTRL_VERSION_S		8
#define   AR8216_CTRL_RESET		BIT(31)

#define AR8216_REG_GLOBAL_CTRL		0x0030
#define   AR8216_GCTRL_MTU		BITS(0, 11)

#define AR8216_REG_VTU			0x0040
#define   AR8216_VTU_OP			BITS(0, 3)
#define   AR8216_VTU_OP_NOOP		0x0
#define   AR8216_VTU_OP_FLUSH		0x1
#define   AR8216_VTU_OP_LOAD		0x2
#define   AR8216_VTU_OP_PURGE		0x3
#define   AR8216_VTU_OP_REMOVE_PORT	0x4
#define   AR8216_VTU_ACTIVE		BIT(3)
#define   AR8216_VTU_FULL		BIT(4)
#define   AR8216_VTU_PORT		BITS(8, 4)
#define   AR8216_VTU_PORT_S		8
#define   AR8216_VTU_VID		BITS(16, 12)
#define   AR8216_VTU_VID_S		16
#define   AR8216_VTU_PRIO		BITS(28, 3)
#define   AR8216_VTU_PRIO_S		28
#define   AR8216_VTU_PRIO_EN		BIT(31)

#define AR8216_REG_VTU_DATA		0x0044
#define   AR8216_VTUDATA_MEMBER		BITS(0, 10)
#define   AR8216_VTUDATA_VALID		BIT(11)

#define AR8216_REG_ATU			0x0050
#define   AR8216_ATU_OP			BITS(0, 3)
#define   AR8216_ATU_OP_NOOP		0x0
#define   AR8216_ATU_OP_FLUSH		0x1
#define   AR8216_ATU_OP_LOAD		0x2
#define   AR8216_ATU_OP_PURGE		0x3
#define   AR8216_ATU_OP_FLUSH_LOCKED	0x4
#define   AR8216_ATU_OP_FLUSH_UNICAST	0x5
#define   AR8216_ATU_OP_GET_NEXT	0x6
#define   AR8216_ATU_ACTIVE		BIT(3)
#define   AR8216_ATU_PORT_NUM		BITS(8, 4)
#define   AR8216_ATU_FULL_VIO		BIT(12)
#define   AR8216_ATU_ADDR4		BITS(16, 8)
#define   AR8216_ATU_ADDR5		BITS(24, 8)

#define AR8216_REG_ATU_DATA		0x0054
#define   AR8216_ATU_ADDR3		BITS(0, 8)
#define   AR8216_ATU_ADDR2		BITS(8, 8)
#define   AR8216_ATU_ADDR1		BITS(16, 8)
#define   AR8216_ATU_ADDR0		BITS(24, 8)

#define AR8216_PORT_OFFSET(_i)		(0x0100 * (_i + 1))
#define AR8216_REG_PORT_STATUS(_i)	(AR8216_PORT_OFFSET(_i) + 0x0000)
#define   AR8216_PORT_STATUS_SPEED	BITS(0,2)
#define   AR8216_PORT_STATUS_SPEED_S	0
#define   AR8216_PORT_STATUS_TXMAC	BIT(2)
#define   AR8216_PORT_STATUS_RXMAC	BIT(3)
#define   AR8216_PORT_STATUS_TXFLOW	BIT(4)
#define   AR8216_PORT_STATUS_RXFLOW	BIT(5)
#define   AR8216_PORT_STATUS_DUPLEX	BIT(6)
#define   AR8216_PORT_STATUS_LINK_UP	BIT(8)
#define   AR8216_PORT_STATUS_LINK_AUTO	BIT(9)
#define   AR8216_PORT_STATUS_LINK_PAUSE	BIT(10)

#define AR8216_REG_PORT_CTRL(_i)	(AR8216_PORT_OFFSET(_i) + 0x0004)

/* port forwarding state */
#define   AR8216_PORT_CTRL_STATE	BITS(0, 3)
#define   AR8216_PORT_CTRL_STATE_S	0

#define   AR8216_PORT_CTRL_LEARN_LOCK	BIT(7)

/* egress 802.1q mode */
#define   AR8216_PORT_CTRL_VLAN_MODE	BITS(8, 2)
#define   AR8216_PORT_CTRL_VLAN_MODE_S	8

#define   AR8216_PORT_CTRL_IGMP_SNOOP	BIT(10)
#define   AR8216_PORT_CTRL_HEADER	BIT(11)
#define   AR8216_PORT_CTRL_MAC_LOOP	BIT(12)
#define   AR8216_PORT_CTRL_SINGLE_VLAN	BIT(13)
#define   AR8216_PORT_CTRL_LEARN	BIT(14)
#define   AR8216_PORT_CTRL_MIRROR_TX	BIT(16)
#define   AR8216_PORT_CTRL_MIRROR_RX	BIT(17)

#define AR8216_REG_PORT_VLAN(_i)	(AR8216_PORT_OFFSET(_i) + 0x0008)

#define   AR8216_PORT_VLAN_DEFAULT_ID	BITS(0, 12)
#define   AR8216_PORT_VLAN_DEFAULT_ID_S	0

#define   AR8216_PORT_VLAN_DEST_PORTS	BITS(16, 9)
#define   AR8216_PORT_VLAN_DEST_PORTS_S	16

/* bit0 added to the priority field of egress frames */
#define   AR8216_PORT_VLAN_TX_PRIO	BIT(27)

/* port default priority */
#define   AR8216_PORT_VLAN_PRIORITY	BITS(28, 2)
#define   AR8216_PORT_VLAN_PRIORITY_S	28

/* ingress 802.1q mode */
#define   AR8216_PORT_VLAN_MODE		BITS(30, 2)
#define   AR8216_PORT_VLAN_MODE_S	30

#define AR8216_REG_PORT_RATE(_i)	(AR8216_PORT_OFFSET(_i) + 0x000c)
#define AR8216_REG_PORT_PRIO(_i)	(AR8216_PORT_OFFSET(_i) + 0x0010)

/* port speed */
enum {
        AR8216_PORT_SPEED_10M = 0,
        AR8216_PORT_SPEED_100M = 1,
        AR8216_PORT_SPEED_1000M = 2,
        AR8216_PORT_SPEED_ERR = 3,
};

/* ingress 802.1q mode */
enum {
	AR8216_IN_PORT_ONLY = 0,
	AR8216_IN_PORT_FALLBACK = 1,
	AR8216_IN_VLAN_ONLY = 2,
	AR8216_IN_SECURE = 3
};

/* egress 802.1q mode */
enum {
	AR8216_OUT_KEEP = 0,
	AR8216_OUT_STRIP_VLAN = 1,
	AR8216_OUT_ADD_VLAN = 2
};

/* port forwarding state */
enum {
	AR8216_PORT_STATE_DISABLED = 0,
	AR8216_PORT_STATE_BLOCK = 1,
	AR8216_PORT_STATE_LISTEN = 2,
	AR8216_PORT_STATE_LEARN = 3,
	AR8216_PORT_STATE_FORWARD = 4
};

#endif
