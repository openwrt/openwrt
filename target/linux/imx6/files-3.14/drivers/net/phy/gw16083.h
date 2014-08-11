/*
 * drivers/net/phy/mv88e6176.h
 *
 * Driver for Marvell Switch
 *
 * Author: Tim Harvey
 *
 * Copyright (c) 2014 Tim Harvey <tharvey@gateworks.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _GW16083_H_
#define _GW16083_H_

#define MII_MARVELL_PHY_PAGE		22

/*
 * I2C Addresses
 */
#define GW16083_I2C_ADDR_SFP1		0x50
#define GW16083_I2C_ADDR_SFP2		0x51
#define GW16083_I2C_ADDR_EEPROM		0x52
#define GW16083_I2C_ADDR_PCA9543	0x70

/*
 * MV88E1111 PHY Registers
 */
enum {
	MII_M1111_PHY_CONTROL		= 0,
	MII_M1111_PHY_STATUS 		= 1,
	MII_M1111_PHY_IDENT0		= 2,
	MII_M1111_PHY_IDENT1		= 3,
	MII_M1111_PHY_EXT_CR		= 20,
	MII_M1111_PHY_LED_CONTROL	= 24,
	MII_M1111_PHY_EXT_SR		= 27,
};

#define MII_M1111_PHY_ID_MASK		0xfffffff0
#define MII_M1111_PHY_ID		0x01410cc0

#define MII_M1111_PHY_CONTROL_RESET	(1 << 15)
#define MII_M1111_PHY_LED_DIRECT	0x4100
#define MII_M1111_PHY_LED_PULSE_STR	0x4111
#define MII_M1111_PHY_LED_COMBINE	0x411c
#define MII_M1111_RX_DELAY		0x80
#define MII_M1111_TX_DELAY		0x2

/*
 * MV88E6176 Switch Registers
 */

/* PHY Addrs */
#define MV_BASE		0x10
#define MV_GLOBAL1	0x1b
#define MV_GLOBAL2	0x1c
#define MV_GLOBAL3	0x1d

/* Global2 Registers */
enum {
	MV_SMI_PHY_COMMAND	= 0x18,
	MV_SMI_PHY_DATA		= 0x19,
	MV_SCRATCH_MISC		= 0x1A,
};

/* Scratch And Misc Reg offsets */
enum {
	MV_GPIO_MODE		= 0x60,
	MV_GPIO_DIR		= 0x62,
	MV_GPIO_DATA		= 0x64,
	MV_GPIO76_CNTL		= 0x6B,
	MV_GPIO54_CNTL		= 0x6A,
	MV_GPIO32_CNTL		= 0x69,
	MV_GPIO10_CNTL		= 0x68,
	MV_CONFIG0		= 0x70,
	MV_CONFIG1		= 0x71,
	MV_CONFIG2		= 0x72,
	MV_CONFIG3		= 0x73,
};

/* PHY Registers */
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
};

/* Port Registers */
enum {
	MV_PORT_STATUS      	= 0x00,
	MV_PORT_PHYS_CONTROL	= 0x01,
	MV_PORT_IDENT       	= 0x03,
	MV_PORT_CONTROL     	= 0x04,
	MV_PORT_VLANMAP     	= 0x06,
	MV_PORT_ASSOC       	= 0x0b,
	MV_PORT_RXCOUNT     	= 0x10,
	MV_PORT_TXCOUNT     	= 0x11,
};

#define SMIBUSY		(1<<15)
#define SMIMODE22	(1<<12)
#define SMIOP_READ	(2<<10)
#define SMIOP_WRITE	(1<<10)
#define DEVADDR		5
#define REGADDR		0

#define MV_IDENT_MASK		0x0000fff0
#define MV_IDENT_VALUE		0x00001760

#endif /* _GW16083_H_ */
