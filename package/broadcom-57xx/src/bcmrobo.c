/*
 * Broadcom BCM5325E/536x switch configuration module
 *
 * Copyright (C) 2007 Felix Fietkau <nbd@openwrt.org>
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
 *
 * Based on:
 *   Broadcom 53xx RoboSwitch device driver.
 *
 *   Copyright 2007, Broadcom Corporation
 *   All Rights Reserved.
 * 
 *   THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 *   KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 *   SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 *   FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */


#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>

#include <typedefs.h>
#include <osl.h>
#include <sbutils.h>
#include <sbconfig.h>
#include <bcmendian.h>
#include "bcmparams.h"
#include <bcmnvram.h>
#include <bcmdevs.h>
#include "bcmrobo.h"
#include "proto/ethernet.h"
#include <switch-core.h>

#define DRIVER_NAME		"bcm57xx"
#define DRIVER_VERSION	"0.1"

#ifndef GPIO_PIN_NOTDEFINED
#define GPIO_PIN_NOTDEFINED	0x20
#endif

#ifdef	BCMDBG
#define	ET_ERROR(args)	printk args
#else	/* BCMDBG */
#define	ET_ERROR(args)
#endif	/* BCMDBG */
#define	ET_MSG(args)

/*
 * Switch can be programmed through SPI interface, which
 * has a rreg and a wreg functions to read from and write to
 * registers.
 */

/* MII access registers */
#define PSEUDO_PHYAD	0x1E	/* MII Pseudo PHY address */
#define REG_MII_PAGE	0x10	/* MII Page register */
#define REG_MII_ADDR	0x11	/* MII Address register */
#define REG_MII_DATA0	0x18	/* MII Data register 0 */
#define REG_MII_DATA1	0x19	/* MII Data register 1 */
#define REG_MII_DATA2	0x1a	/* MII Data register 2 */
#define REG_MII_DATA3	0x1b	/* MII Data register 3 */

/* Page numbers */
#define PAGE_CTRL	0x00	/* Control page */
#define PAGE_MMR	0x02	/* 5397 Management/Mirroring page */
#define PAGE_VTBL	0x05	/* ARL/VLAN Table access page */
#define PAGE_VLAN	0x34	/* VLAN page */

/* Control page registers */
#define REG_CTRL_PORT0	0x00	/* Port 0 traffic control register */
#define REG_CTRL_PORT1	0x01	/* Port 1 traffic control register */
#define REG_CTRL_PORT2	0x02	/* Port 2 traffic control register */
#define REG_CTRL_PORT3	0x03	/* Port 3 traffic control register */
#define REG_CTRL_PORT4	0x04	/* Port 4 traffic control register */
#define REG_CTRL_PORT5	0x05	/* Port 5 traffic control register */
#define REG_CTRL_PORT6	0x06	/* Port 6 traffic control register */
#define REG_CTRL_PORT7	0x07	/* Port 7 traffic control register */
#define REG_CTRL_MODE	0x0B	/* Switch Mode register */
#define REG_CTRL_MIIPO	0x0E	/* 5325: MII Port Override register */
#define REG_CTRL_SRST	0x79	/* Software reset control register */

#define REG_DEVICE_ID	0x30	/* 539x Device id: */
#define	DEVID5325	0x25	/*  5325 (Not really be we fake it) */
#define	DEVID5395	0x95	/*  5395 */
#define	DEVID5397	0x97	/*  5397 */
#define	DEVID5398	0x98	/*  5398 */

/* VLAN page registers */
#define REG_VLAN_CTRL0	0x00	/* VLAN Control 0 register */
#define REG_VLAN_CTRL1	0x01	/* VLAN Control 1 register */
#define REG_VLAN_CTRL4	0x04	/* VLAN Control 4 register */
#define REG_VLAN_CTRL5	0x05	/* VLAN Control 5 register */
#define REG_VLAN_ACCESS	0x06	/* VLAN Table Access register */
#define REG_VLAN_WRITE	0x08	/* VLAN Write register */
#define REG_VLAN_READ	0x0C	/* VLAN Read register */
#define REG_VLAN_PTAG0	0x10	/* VLAN Default Port Tag register - port 0 */
#define REG_VLAN_PTAG1	0x12	/* VLAN Default Port Tag register - port 1 */
#define REG_VLAN_PTAG2	0x14	/* VLAN Default Port Tag register - port 2 */
#define REG_VLAN_PTAG3	0x16	/* VLAN Default Port Tag register - port 3 */
#define REG_VLAN_PTAG4	0x18	/* VLAN Default Port Tag register - port 4 */
#define REG_VLAN_PTAG5	0x1a	/* VLAN Default Port Tag register - port 5 */
#define REG_VLAN_PTAG6	0x1c	/* VLAN Default Port Tag register - port 6 */
#define REG_VLAN_PTAG7	0x1e	/* VLAN Default Port Tag register - port 7 */
#define REG_VLAN_PTAG8	0x20	/* 539x: VLAN Default Port Tag register - IMP port */
#define REG_VLAN_PMAP	0x20	/* 5325: VLAN Priority Re-map register */

#define VLAN_NUMVLANS	16	/* # of VLANs */


/* ARL/VLAN Table Access page registers */
#define REG_VTBL_CTRL		0x00	/* ARL Read/Write Control */
#define REG_VTBL_MINDX		0x02	/* MAC Address Index */
#define REG_VTBL_VINDX		0x08	/* VID Table Index */
#define REG_VTBL_ARL_E0		0x10	/* ARL Entry 0 */
#define REG_VTBL_ARL_E1		0x18	/* ARL Entry 1 */
#define REG_VTBL_DAT_E0		0x18	/* ARL Table Data Entry 0 */
#define REG_VTBL_SCTRL		0x20	/* ARL Search Control */
#define REG_VTBL_SADDR		0x22	/* ARL Search Address */
#define REG_VTBL_SRES		0x24	/* ARL Search Result */
#define REG_VTBL_SREXT		0x2c	/* ARL Search Result */
#define REG_VTBL_VID_E0		0x30	/* VID Entry 0 */
#define REG_VTBL_VID_E1		0x32	/* VID Entry 1 */
#define REG_VTBL_PREG		0xFF	/* Page Register */
#define REG_VTBL_ACCESS		0x60	/* VLAN table access register */
#define REG_VTBL_INDX		0x61	/* VLAN table address index register */
#define REG_VTBL_ENTRY		0x63	/* VLAN table entry register */
#define REG_VTBL_ACCESS_5395	0x80	/* VLAN table access register */
#define REG_VTBL_INDX_5395	0x81	/* VLAN table address index register */
#define REG_VTBL_ENTRY_5395	0x83	/* VLAN table entry register */

/* SPI registers */
#define REG_SPI_PAGE	0xff	/* SPI Page register */

/* Access switch registers through GPIO/SPI */

/* Minimum timing constants */
#define SCK_EDGE_TIME	2	/* clock edge duration - 2us */
#define MOSI_SETUP_TIME	1	/* input setup duration - 1us */
#define SS_SETUP_TIME	1 	/* select setup duration - 1us */

/* misc. constants */
#define SPI_MAX_RETRY	100

static int config_attach(robo_info_t *robo);
static void config_detach(robo_info_t *robo);

/* Enable GPIO access to the chip */
static void
gpio_enable(robo_info_t *robo)
{
	/* Enable GPIO outputs with SCK and MOSI low, SS high */
	sb_gpioout(robo->sbh, robo->ss | robo->sck | robo->mosi, robo->ss, GPIO_DRV_PRIORITY);
	sb_gpioouten(robo->sbh, robo->ss | robo->sck | robo->mosi,
	             robo->ss | robo->sck | robo->mosi, GPIO_DRV_PRIORITY);
}

/* Disable GPIO access to the chip */
static void
gpio_disable(robo_info_t *robo)
{
	/* Disable GPIO outputs with all their current values */
	sb_gpioouten(robo->sbh, robo->ss | robo->sck | robo->mosi, 0, GPIO_DRV_PRIORITY);
}

/* Write a byte stream to the chip thru SPI */
static int
spi_write(robo_info_t *robo, uint8 *buf, uint len)
{
	uint i;
	uint8 mask;

	/* Byte bang from LSB to MSB */
	for (i = 0; i < len; i++) {
		/* Bit bang from MSB to LSB */
		for (mask = 0x80; mask; mask >>= 1) {
			/* Clock low */
			sb_gpioout(robo->sbh, robo->sck, 0, GPIO_DRV_PRIORITY);
			OSL_DELAY(SCK_EDGE_TIME);

			/* Sample on rising edge */
			if (mask & buf[i])
				sb_gpioout(robo->sbh, robo->mosi, robo->mosi, GPIO_DRV_PRIORITY);
			else
				sb_gpioout(robo->sbh, robo->mosi, 0, GPIO_DRV_PRIORITY);
			OSL_DELAY(MOSI_SETUP_TIME);

			/* Clock high */
			sb_gpioout(robo->sbh, robo->sck, robo->sck, GPIO_DRV_PRIORITY);
			OSL_DELAY(SCK_EDGE_TIME);
		}
	}

	return 0;
}

/* Read a byte stream from the chip thru SPI */
static int
spi_read(robo_info_t *robo, uint8 *buf, uint len)
{
	uint i, timeout;
	uint8 rack, mask, byte;

	/* Timeout after 100 tries without RACK */
	for (i = 0, rack = 0, timeout = SPI_MAX_RETRY; i < len && timeout;) {
		/* Bit bang from MSB to LSB */
		for (mask = 0x80, byte = 0; mask; mask >>= 1) {
			/* Clock low */
			sb_gpioout(robo->sbh, robo->sck, 0, GPIO_DRV_PRIORITY);
			OSL_DELAY(SCK_EDGE_TIME);

			/* Sample on falling edge */
			if (sb_gpioin(robo->sbh) & robo->miso)
				byte |= mask;

			/* Clock high */
			sb_gpioout(robo->sbh, robo->sck, robo->sck, GPIO_DRV_PRIORITY);
			OSL_DELAY(SCK_EDGE_TIME);
		}
		/* RACK when bit 0 is high */
		if (!rack) {
			rack = (byte & 1);
			timeout--;
			continue;
		}
		/* Byte bang from LSB to MSB */
		buf[i] = byte;
		i++;
	}

	if (timeout == 0) {
		ET_ERROR(("spi_read: timeout"));
		return -1;
	}

	return 0;
}

/* Enable/disable SPI access */
static void
spi_select(robo_info_t *robo, uint8 spi)
{
	if (spi) {
		/* Enable SPI access */
		sb_gpioout(robo->sbh, robo->ss, 0, GPIO_DRV_PRIORITY);
	} else {
		/* Disable SPI access */
		sb_gpioout(robo->sbh, robo->ss, robo->ss, GPIO_DRV_PRIORITY);
	}
	OSL_DELAY(SS_SETUP_TIME);
}


/* Select chip and page */
static void
spi_goto(robo_info_t *robo, uint8 page)
{
	uint8 reg8 = REG_SPI_PAGE;	/* page select register */
	uint8 cmd8;

	/* Issue the command only when we are on a different page */
	if (robo->page == page)
		return;

	robo->page = page;

	/* Enable SPI access */
	spi_select(robo, 1);

	/* Select new page with CID 0 */
	cmd8 = ((6 << 4) |		/* normal SPI */
	        1);			/* write */
	spi_write(robo, &cmd8, 1);
	spi_write(robo, &reg8, 1);
	spi_write(robo, &page, 1);

	/* Disable SPI access */
	spi_select(robo, 0);
}

/* Write register thru SPI */
static int
spi_wreg(robo_info_t *robo, uint8 page, uint8 addr, void *val, int len)
{
	int status = 0;
	uint8 cmd8;
	union {
		uint8 val8;
		uint16 val16;
		uint32 val32;
	} bytes;

	/* validate value length and buffer address */
	ASSERT(len == 1 || (len == 2 && !((int)val & 1)) ||
	       (len == 4 && !((int)val & 3)));

	/* Select chip and page */
	spi_goto(robo, page);

	/* Enable SPI access */
	spi_select(robo, 1);

	/* Write with CID 0 */
	cmd8 = ((6 << 4) |		/* normal SPI */
	        1);			/* write */
	spi_write(robo, &cmd8, 1);
	spi_write(robo, &addr, 1);
	switch (len) {
	case 1:
		bytes.val8 = *(uint8 *)val;
		break;
	case 2:
		bytes.val16 = htol16(*(uint16 *)val);
		break;
	case 4:
		bytes.val32 = htol32(*(uint32 *)val);
		break;
	}
	spi_write(robo, (uint8 *)val, len);

	ET_MSG(("%s: [0x%x-0x%x] := 0x%x (len %d)\n", __FUNCTION__, page, addr,
	        *(uint16 *)val, len));
	/* Disable SPI access */
	spi_select(robo, 0);
	return status;
}

/* Read register thru SPI in fast SPI mode */
static int
spi_rreg(robo_info_t *robo, uint8 page, uint8 addr, void *val, int len)
{
	int status = 0;
	uint8 cmd8;
	union {
		uint8 val8;
		uint16 val16;
		uint32 val32;
	} bytes;

	/* validate value length and buffer address */
	ASSERT(len == 1 || (len == 2 && !((int)val & 1)) ||
	       (len == 4 && !((int)val & 3)));

	/* Select chip and page */
	spi_goto(robo, page);

	/* Enable SPI access */
	spi_select(robo, 1);

	/* Fast SPI read with CID 0 and byte offset 0 */
	cmd8 = (1 << 4);		/* fast SPI */
	spi_write(robo, &cmd8, 1);
	spi_write(robo, &addr, 1);
	status = spi_read(robo, (uint8 *)&bytes, len);
	switch (len) {
	case 1:
		*(uint8 *)val = bytes.val8;
		break;
	case 2:
		*(uint16 *)val = ltoh16(bytes.val16);
		break;
	case 4:
		*(uint32 *)val = ltoh32(bytes.val32);
		break;
	}

	ET_MSG(("%s: [0x%x-0x%x] => 0x%x (len %d)\n", __FUNCTION__, page, addr,
	        *(uint16 *)val, len));

	/* Disable SPI access */
	spi_select(robo, 0);
	return status;
}

/* SPI/gpio interface functions */
static dev_ops_t spigpio = {
	gpio_enable,
	gpio_disable,
	spi_wreg,
	spi_rreg,
	"SPI (GPIO)"
};


/* Access switch registers through MII (MDC/MDIO) */

#define MII_MAX_RETRY	100

/* Write register thru MDC/MDIO */
static  int
mii_wreg(robo_info_t *robo, uint8 page, uint8 reg, void *val, int len)
{
	uint16 cmd16, val16,val48[3];
	void *h = robo->h;
    uint32 val64[2];
	memset(val48,0,6);
	memset(val64,0,8);
	int i;
	uint8 *ptr = (uint8 *)val;

	/* validate value length and buffer address */
	ASSERT(len == 1 || len == 6 || len == 8 ||
	       ((len == 2) && !((int)val & 1)) || ((len == 4) && !((int)val & 3)));

	ET_MSG(("%s: [0x%x-0x%x] := 0x%x (len %d)\n", __FUNCTION__, page, reg,
	          *(uint16 *)val, len));

	/* set page number - MII register 0x10 */
	if (robo->page != page) {
		cmd16 = ((page << 8) |		/* page number */
		         1);			/* mdc/mdio access enable */
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_PAGE, cmd16);
		robo->page = page;
	}

	switch (len) {
	case 8:
		val16 = ptr[7];
		val16 = ((val16 << 8) | ptr[6]);
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA3, val16);
		/* FALLTHRU */

	case 6:
		val16 = ptr[5];
		val16 = ((val16 << 8) | ptr[4]);
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA2, val16);
		val16 = ptr[3];
		val16 = ((val16 << 8) | ptr[2]);
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA1, val16);
		val16 = ptr[1];
		val16 = ((val16 << 8) | ptr[0]);
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA0, val16);
		break;

	case 4:
		val16 = (uint16)((*(uint32 *)val) >> 16);
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA1, val16);
		val16 = (uint16)(*(uint32 *)val);
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA0, val16);
		break;

	case 2:
		val16 = *(uint16 *)val;
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA0, val16);
		break;

	case 1:
		val16 = *(uint8 *)val;
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_DATA0, val16);
		break;
	}

	/* set register address - MII register 0x11 */
	cmd16 = ((reg << 8) |		/* register address */
	         1);		/* opcode write */
	robo->miiwr(h, PSEUDO_PHYAD, REG_MII_ADDR, cmd16);

	/* is operation finished? */
	for (i = MII_MAX_RETRY; i > 0; i --) {
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_ADDR);
		if ((val16 & 3) == 0)
			break;
	}

	/* timed out */
	if (!i) {
		ET_ERROR(("mii_wreg: timeout"));
		return -1;
	}
	return 0;
}

/* Read register thru MDC/MDIO */
static  int
mii_rreg(robo_info_t *robo, uint8 page, uint8 reg, void *val, int len)
{
	uint16 cmd16, val16;
	void *h = robo->h;
	int i;
	uint8 *ptr = (uint8 *)val;

	/* validate value length and buffer address */
	ASSERT(len == 1 || len == 6 || len == 8 ||
	       ((len == 2) && !((int)val & 1)) || ((len == 4) && !((int)val & 3)));

	/* set page number - MII register 0x10 */
	if (robo->page != page) {
		cmd16 = ((page << 8) |		/* page number */
		         1);			/* mdc/mdio access enable */
		robo->miiwr(h, PSEUDO_PHYAD, REG_MII_PAGE, cmd16);
		robo->page = page;
	}

	/* set register address - MII register 0x11 */
	cmd16 = ((reg << 8) |		/* register address */
	         2);			/* opcode read */
	robo->miiwr(h, PSEUDO_PHYAD, REG_MII_ADDR, cmd16);

	/* is operation finished? */
	for (i = MII_MAX_RETRY; i > 0; i --) {
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_ADDR);
		if ((val16 & 3) == 0)
			break;
	}
	/* timed out */
	if (!i) {
		ET_ERROR(("mii_rreg: timeout"));
		return -1;
	}

	ET_MSG(("%s: [0x%x-0x%x] => 0x%x (len %d)\n", __FUNCTION__, page, reg, val16, len));

	switch (len) {
	case 8:
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA3);
		ptr[7] = (val16 >> 8);
		ptr[6] = (val16 & 0xff);
		/* FALLTHRU */

	case 6:
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA2);
		ptr[5] = (val16 >> 8);
		ptr[4] = (val16 & 0xff);
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA1);
		ptr[3] = (val16 >> 8);
		ptr[2] = (val16 & 0xff);
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA0);
		ptr[1] = (val16 >> 8);
		ptr[0] = (val16 & 0xff);
		break;

	case 4:
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA1);
		*(uint32 *)val = (((uint32)val16) << 16);
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA0);
		*(uint32 *)val |= val16;
		break;

	case 2:
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA0);
		*(uint16 *)val = val16;
		break;
	case 1:
		val16 = robo->miird(h, PSEUDO_PHYAD, REG_MII_DATA0);
		*(uint8 *)val = (uint8)(val16 & 0xff);
		break;
	}

	return 0;
}

/* MII interface functions */
static dev_ops_t mdcmdio = {
	NULL,
	NULL,
	mii_wreg,
	mii_rreg,
	"MII (MDC/MDIO)"
};

/* High level switch configuration functions. */

static int
findmatch(char *string, char *name)
{
	uint len;
	char *c;

	len = strlen(name);
	/* CSTYLED */
	while ((c = strchr(string, ',')) != NULL) {
		if (len == (uint)(c - string) && !strncmp(string, name, len))
			return 1;
		string = c + 1;
	}

	return (!strcmp(string, name));
}

static uint
getgpiopin(char *vars, char *pin_name, uint def_pin)
{
	char name[] = "gpioXXXX";
	char *val;
	uint pin;

	/* Go thru all possibilities till a match in pin name */
	for (pin = 0; pin < GPIO_NUMPINS; pin ++) {
		snprintf(name, sizeof(name), "gpio%d", pin);
		val = getvar(vars, name);
		if (val && findmatch(val, pin_name))
			return pin;
	}

	if (def_pin != GPIO_PIN_NOTDEFINED) {
		/* make sure the default pin is not used by someone else */
		snprintf(name, sizeof(name), "gpio%d", def_pin);
		if (getvar(vars, name)) {
			def_pin =  GPIO_PIN_NOTDEFINED;
		}
	}

	return def_pin;
}


/* Port flags */
#define FLAG_TAGGED	't'	/* output tagged (external ports only) */
#define FLAG_UNTAG	'u'	/* input & output untagged (CPU port only, for OS (linux, ...) */
#define FLAG_LAN	'*'	/* input & output untagged (CPU port only, for CFE */

/* port descriptor */
typedef	struct {
	uint32 untag;	/* untag enable bit (Page 0x05 Address 0x63-0x66 Bit[17:9]) */
	uint32 member;	/* vlan member bit (Page 0x05 Address 0x63-0x66 Bit[7:0]) */
	uint8 ptagr;	/* port tag register address (Page 0x34 Address 0x10-0x1F) */
	uint8 cpu;	/* is this cpu port? */
} pdesc_t;

pdesc_t pdesc97[] = {
	/* 5395/5397/5398 is 0 ~ 7.  port 8 is IMP port. */
	/* port 0 */ {1 << 9, 1 << 0, REG_VLAN_PTAG0, 0},
	/* port 1 */ {1 << 10, 1 << 1, REG_VLAN_PTAG1, 0},
	/* port 2 */ {1 << 11, 1 << 2, REG_VLAN_PTAG2, 0},
	/* port 3 */ {1 << 12, 1 << 3, REG_VLAN_PTAG3, 0},
	/* port 4 */ {1 << 13, 1 << 4, REG_VLAN_PTAG4, 0},
	/* port 5 */ {1 << 14, 1 << 5, REG_VLAN_PTAG5, 0},
	/* port 6 */ {1 << 15, 1 << 6, REG_VLAN_PTAG6, 0},
	/* port 7 */ {1 << 16, 1 << 7, REG_VLAN_PTAG7, 0},
	/* mii port */ {1 << 17, 1 << 8, REG_VLAN_PTAG8, 1},
};

pdesc_t pdesc25[] = {
	/* port 0 */ {1 << 6, 1 << 0, REG_VLAN_PTAG0, 0},
	/* port 1 */ {1 << 7, 1 << 1, REG_VLAN_PTAG1, 0},
	/* port 2 */ {1 << 8, 1 << 2, REG_VLAN_PTAG2, 0},
	/* port 3 */ {1 << 9, 1 << 3, REG_VLAN_PTAG3, 0},
	/* port 4 */ {1 << 10, 1 << 4, REG_VLAN_PTAG4, 0},
	/* mii port */ {1 << 11, 1 << 5, REG_VLAN_PTAG5, 1},
};


#define to_robo(driver) ((robo_info_t *) ((switch_driver *) driver)->priv)
#define ROBO_START(driver) \
	do { \
		robo_info_t *robo = to_robo(driver); \
		if (robo->ops->enable_mgmtif) \
			robo->ops->enable_mgmtif(robo)

#define ROBO_END(driver) \
		if (robo->ops->disable_mgmtif) \
			robo->ops->disable_mgmtif(robo); \
	} while (0)

int
bcm_robo_reset(robo_info_t *robo)
{
	int i, max_port_ind;
	uint8 val8;
	uint16 val16;
	uint32 val32;
	pdesc_t *pdesc;
	int pdescsz;

/* printk(KERN_WARNING "bcmrobo.c: bcm_robo_reset\n"); */

	if (robo->ops->enable_mgmtif)
		robo->ops->enable_mgmtif(robo);

	/* setup global vlan configuration, FIXME: necessary? */
	/* VLAN Control 0 Register (Page 0x34, Address 0) */
	val8 = ((1 << 7) |		/* enable 802.1Q VLAN */
	        (3 << 5));		/* individual VLAN learning mode */
	robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));

	/* VLAN Control 1 Register (Page 0x34, Address 1) */
	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));
	val8 |= ((1 << 2) |		/* enable RSV multicast V Fwdmap */
		(1 << 3));		/* enable RSV multicast V Untagmap */
	if (robo->devid == DEVID5325)
		val8 |= (1 << 1);	/* enable RSV multicast V Tagging */
	robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL1, &val8, sizeof(val8));

	bcm_robo_set_macaddr(robo, NULL);

	if (robo->devid == DEVID5325) {
		/* VLAN Control 4 Register (Page 0x34, Address 4) */
		val8 = (1 << 6);		/* drop frame with VID violation */
		robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL4, &val8, sizeof(val8));

		/* VLAN Control 5 Register (Page 0x34, Address 5) */
		val8 = (1 << 3);		/* drop frame when miss V table */
		robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL5, &val8, sizeof(val8));

		pdesc = pdesc25;
		pdescsz = sizeof(pdesc25) / sizeof(pdesc_t);
	} else {
		pdesc = pdesc97;
		pdescsz = sizeof(pdesc97) / sizeof(pdesc_t);
	}

	if (robo->devid == DEVID5325) {
		/* setup priority mapping - applies to tagged ingress frames */
		/* Priority Re-map Register (Page 0x34, Address 0x20-0x23) */
		/* FIXME: un-hardcode */
		val32 = ((0 << 0) |	/* 0 -> 0 */
		         (1 << 3) |	/* 1 -> 1 */
		         (2 << 6) |	/* 2 -> 2 */
		         (3 << 9) |	/* 3 -> 3 */
		         (4 << 12) |	/* 4 -> 4 */
		         (5 << 15) |	/* 5 -> 5 */
		         (6 << 18) |	/* 6 -> 6 */
		         (7 << 21));	/* 7 -> 7 */
		robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_PMAP, &val32, sizeof(val32));
	}

	/* Set unmanaged mode */
	robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));
	val8 &= (~(1 << 0));
	robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));

	/* No spanning tree for unmanaged mode */
	val8 = 0;
	max_port_ind = ((robo->devid == DEVID5398) ? REG_CTRL_PORT7 : REG_CTRL_PORT4);
	for (i = REG_CTRL_PORT0; i <= max_port_ind; i++) {
		robo->ops->write_reg(robo, PAGE_CTRL, i, &val8, sizeof(val8));
	}

	/* WAN port LED */
	val16 = 0x1f;
	robo->ops->write_reg(robo, PAGE_CTRL, 0x16, &val16, 2);

	if (robo->ops->enable_mgmtif)
		robo->ops->disable_mgmtif(robo);

	return 0;
}

/* Get access to the RoboSwitch */
robo_info_t *
bcm_robo_attach(sb_t *sbh, void *h, char *name, char *vars, miird_f miird, miiwr_f miiwr)
{
	robo_info_t *robo;
	uint32 reset, idx;
	uint8 val8;
	uint16 val16;

	/* Allocate and init private state */
	if (!(robo = MALLOC(sb_osh(sbh), sizeof(robo_info_t)))) {
		ET_ERROR(("robo_attach: out of memory, malloced %d bytes", MALLOCED(sb_osh(sbh))));
		return NULL;
	}
	bzero(robo, sizeof(robo_info_t));

	robo->h = h;
	robo->sbh = sbh;
	robo->vars = vars;
	robo->miird = miird;
	robo->miiwr = miiwr;
	robo->page = -1;
	robo->name = name;

	/* Trigger external reset by nvram variable existance */
	if ((reset = getgpiopin(robo->vars, "robo_reset", GPIO_PIN_NOTDEFINED)) !=
	    GPIO_PIN_NOTDEFINED) {
		/*
		 * Reset sequence: RESET low(50ms)->high(20ms)
		 *
		 * We have to perform a full sequence for we don't know how long
		 * it has been from power on till now.
		 */
		ET_MSG(("%s: Using external reset in gpio pin %d\n", __FUNCTION__, reset));
		reset = 1 << reset;

		/* Keep RESET low for 50 ms */
		sb_gpioout(robo->sbh, reset, 0, GPIO_DRV_PRIORITY);
		sb_gpioouten(robo->sbh, reset, reset, GPIO_DRV_PRIORITY);
		bcm_mdelay(50);

		if (robo->devid == DEVID5395)
			nvram_set("switch_type", "BCM5395");
		else if(robo->devid == DEVID5397)
			nvram_set("switch_type", "BCM5397");
		else
			nvram_set("switch_type", "unknown");

		/* Keep RESET high for at least 20 ms */
		sb_gpioout(robo->sbh, reset, reset, GPIO_DRV_PRIORITY);
		bcm_mdelay(20);
	} else {
		/* In case we need it */
		idx = sb_coreidx(robo->sbh);

		if (sb_setcore(robo->sbh, SB_ROBO, 0)) {
			/* If we have an internal robo core, reset it using sb_core_reset */
			ET_MSG(("%s: Resetting internal robo core\n", __FUNCTION__));
			sb_core_reset(robo->sbh, 0, 0);
		}

		sb_setcoreidx(robo->sbh, idx);
	}

	if (miird && miiwr) {
		uint16 tmp;
		int rc, retry_count = 0;

		/* Read the PHY ID */
		tmp = miird(h, PSEUDO_PHYAD, 2);
		if (tmp != 0xffff) {
			do {
				rc = mii_rreg(robo, PAGE_MMR, REG_DEVICE_ID, \
							  &robo->devid, sizeof(uint16));
				if (rc != 0)
					break;
				retry_count++;
			} while ((robo->devid == 0) && (retry_count < 10));

			ET_MSG(("%s: devid read %ssuccesfully via mii: 0x%x\n", __FUNCTION__, \
			        rc ? "un" : "", robo->devid));
			ET_MSG(("%s: mii access to switch works\n", __FUNCTION__));
			robo->ops = &mdcmdio;
			if ((rc != 0) || (robo->devid == 0)) {
				ET_MSG(("%s: error reading devid, assuming 5325e\n", __FUNCTION__));
				robo->devid = DEVID5325;
			}
			ET_MSG(("%s: devid: 0x%x\n", __FUNCTION__, robo->devid));
		}
	}

	if ((robo->devid == DEVID5395) ||
	    (robo->devid == DEVID5397) ||
	    (robo->devid == DEVID5398)) {
		uint8 srst_ctrl;

		/* If it is a 539x switch, use the soft reset register */
		ET_MSG(("%s: Resetting 539x robo switch\n", __FUNCTION__));

		/* Reset the 539x switch core and register file */
		srst_ctrl = 0x83;
		mii_wreg(robo, PAGE_CTRL, REG_CTRL_SRST, &srst_ctrl, sizeof(uint8));

		bcm_mdelay(500);

		srst_ctrl = 0x00;
		mii_wreg(robo, PAGE_CTRL, REG_CTRL_SRST, &srst_ctrl, sizeof(uint8));
	}

	if (!robo->ops) {
		int mosi, miso, ss, sck;

		robo->ops = &spigpio;
		robo->devid = DEVID5325;

		/* Init GPIO mapping. Default 2, 3, 4, 5 */
		ss = getgpiopin(vars, "robo_ss", 2);
		if (ss == GPIO_PIN_NOTDEFINED) {
			ET_ERROR(("robo_attach: robo_ss gpio fail: GPIO 2 in use"));
			goto error;
		}
		robo->ss = 1 << ss;
		sck = getgpiopin(vars, "robo_sck", 3);
		if (sck == GPIO_PIN_NOTDEFINED) {
			ET_ERROR(("robo_attach: robo_sck gpio fail: GPIO 3 in use"));
			goto error;
		}
		robo->sck = 1 << sck;
		mosi = getgpiopin(vars, "robo_mosi", 4);
		if (mosi == GPIO_PIN_NOTDEFINED) {
			ET_ERROR(("robo_attach: robo_mosi gpio fail: GPIO 4 in use"));
			goto error;
		}
		robo->mosi = 1 << mosi;
		miso = getgpiopin(vars, "robo_miso", 5);
		if (miso == GPIO_PIN_NOTDEFINED) {
			ET_ERROR(("robo_attach: robo_miso gpio fail: GPIO 5 in use"));
			goto error;
		}
		robo->miso = 1 << miso;
		ET_MSG(("%s: ss %d sck %d mosi %d miso %d\n", __FUNCTION__,
		        ss, sck, mosi, miso));
	}

	/* sanity check */
	ASSERT(robo->ops);
	ASSERT(robo->ops->write_reg);
	ASSERT(robo->ops->read_reg);
	ASSERT((robo->devid == DEVID5325) ||
	       (robo->devid == DEVID5395) ||
	       (robo->devid == DEVID5397) ||
	       (robo->devid == DEVID5398));

	bcm_robo_reset(robo);
	config_attach(robo);

	return robo;

error:
	MFREE(sb_osh(robo->sbh), robo, sizeof(robo_info_t));
	return NULL;
}

/* Release access to the RoboSwitch */
void
bcm_robo_detach(robo_info_t *robo)
{
	config_detach(robo);
	MFREE(sb_osh(robo->sbh), robo, sizeof(robo_info_t));
}

/* Enable the device and set it to a known good state */
int
bcm_robo_enable_device(robo_info_t *robo)
{
	uint8 reg_offset, reg_val;
	int ret = 0;

	/* Enable management interface access */
	if (robo->ops->enable_mgmtif)
		robo->ops->enable_mgmtif(robo);

	if (robo->devid == DEVID5398) {
		/* Disable unused ports: port 6 and 7 */
		for (reg_offset = REG_CTRL_PORT6; reg_offset <= REG_CTRL_PORT7; reg_offset ++) {
			/* Set bits [1:0] to disable RX and TX */
			reg_val = 0x03;
			robo->ops->write_reg(robo, PAGE_CTRL, reg_offset, &reg_val,
			                     sizeof(reg_val));
		}
	}

	if (robo->devid == DEVID5325) {
		/* Must put the switch into Reverse MII mode! */

		/* MII port state override (page 0 register 14) */
		robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MIIPO, &reg_val, sizeof(reg_val));

		/* Bit 4 enables reverse MII mode */
		if (!(reg_val & (1 << 4))) {
			/* Enable RvMII */
			reg_val |= (1 << 4);
			robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_MIIPO, &reg_val,
			                     sizeof(reg_val));

			/* Read back */
			robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MIIPO, &reg_val,
			                    sizeof(reg_val));
			if (!(reg_val & (1 << 4))) {
				ET_ERROR(("robo_enable_device: enabling RvMII mode failed\n"));
				ret = -1;
			}
		}
	}

	/* Disable management interface access */
	if (robo->ops->disable_mgmtif)
		robo->ops->disable_mgmtif(robo);

	return ret;
}


void bcm_robo_set_macaddr(robo_info_t *robo, char *mac_addr)
{
	uint8 arl_entry[8] = { 0 }, arl_entry1[8] = { 0 };

	if (mac_addr != NULL)
		memcpy(robo->macaddr, mac_addr, 6);

	mac_addr = robo->macaddr;

	/* setup mac address */
	arl_entry[0] = mac_addr[5];
	arl_entry[1] = mac_addr[4];
	arl_entry[2] = mac_addr[3];
	arl_entry[3] = mac_addr[2];
	arl_entry[4] = mac_addr[1];
	arl_entry[5] = mac_addr[0];

	if (robo->devid == DEVID5325) {
		/* Init the entry 1 of the bin */
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_ARL_E1, \
				     arl_entry1, sizeof(arl_entry1));
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_VID_E1, \
				     arl_entry1, 1);

		/* Init the entry 0 of the bin */
		arl_entry[6] = 0x8;		/* Port Id: MII */
		arl_entry[7] = 0xc0;	/* Static Entry, Valid */

		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_ARL_E0, \
				     arl_entry, sizeof(arl_entry));
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_MINDX, \
				     arl_entry, ETHER_ADDR_LEN);

	} else {
		/* Initialize the MAC Addr Index Register */
		robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_MINDX, \
				     arl_entry, ETHER_ADDR_LEN);
	}
}

static int handle_reset(void *driver, char *buf, int nr)
{
	ROBO_START(driver);
	bcm_robo_reset(robo);
	ROBO_END(driver);

	return 0;
}


static int handle_enable_read(void *driver, char *buf, int nr)
{
	int ret;
	uint8 val8;

	ROBO_START(driver);
	robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));
	ret = sprintf(buf, "%d\n", !!(val8 & (1 << 1)));
	ROBO_END(driver);

	return ret;
}

static int handle_enable_write(void *driver, char *buf, int nr)
{
	uint8 val8;

/* printk(KERN_WARNING "bcmrobo.c: handle_enable_write\n"); */

	ROBO_START(driver);
	robo->ops->read_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));
	val8 &= ~(1 << 1);
	val8 |= ((buf[0] == '1') << 1);
	robo->ops->write_reg(robo, PAGE_CTRL, REG_CTRL_MODE, &val8, sizeof(val8));
	ROBO_END(driver);

	return 0;
}

static int handle_enable_vlan_read(void *driver, char *buf, int nr)
{
	uint8 val8;

	ROBO_START(driver);
	robo->ops->read_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));
	ROBO_END(driver);

	return sprintf(buf, "%d\n", (((val8 & (1 << 7)) == (1 << 7)) ? 1 : 0));
}
static int handle_enable_vlan_write(void *driver, char *buf, int nr)
{
	int disable = ((buf[0] != '1') ? 1 : 0);

	uint8 val8;
	uint16 val16;
	pdesc_t *pdesc;
	int pdescsz;
	uint16 vid;
	uint8 arl_entry[8] = { 0 }, arl_entry1[8] = { 0 };

/* printk(KERN_WARNING "bcmrobo.c: handle_enable_vlan_write\n"); */

	ROBO_START(driver);

	/* setup global vlan configuration */
	/* VLAN Control 0 Register (Page 0x34, Address 0) */
	val8 = disable ? 0 : 
		((1 << 7) |		/* enable/disable 802.1Q VLAN */
		(3 << 5));		/* individual VLAN learning mode */
	robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL0, &val8, sizeof(val8));

	/* VLAN Control 1 Register (Page 0x34, Address 1) */
	val8 = disable ? 0 : 
		((1 << 2) |		/* enable/disable RSV multicast V Fwdmap */
		(1 << 3));		/* enable/disable RSV multicast V Untagmap */
	if (robo->devid == DEVID5325)
		val8 |= disable ? 0 : (1 << 1);		/* enable/disable RSV multicast V Tagging */
	robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL1, &val8, sizeof(val8));

	if ( disable == 0 ) {		/* FIXME: ok to stop here when disabling? */
		arl_entry[0] = robo->macaddr[5];
		arl_entry[1] = robo->macaddr[4];
		arl_entry[2] = robo->macaddr[3];
		arl_entry[3] = robo->macaddr[2];
		arl_entry[4] = robo->macaddr[1];
		arl_entry[5] = robo->macaddr[0];

		if (robo->devid == DEVID5325) {
			/* Init the entry 1 of the bin */
			robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_ARL_E1, \
					     arl_entry1, sizeof(arl_entry1));
			robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_VID_E1, \
					     arl_entry1, 1);

			/* Init the entry 0 of the bin */
			arl_entry[6] = 0x8;		/* Port Id: MII */
			arl_entry[7] = 0xc0;	/* Static Entry, Valid */

			robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_ARL_E0, \
					     arl_entry, sizeof(arl_entry));
			robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_MINDX, \
					     arl_entry, ETHER_ADDR_LEN);

			/* VLAN Control 4 Register (Page 0x34, Address 4) */
			val8 = (1 << 6);		/* drop frame with VID violation */
			robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL4, &val8, sizeof(val8));

			/* VLAN Control 5 Register (Page 0x34, Address 5) */
			val8 = (1 << 3);		/* drop frame when miss V table */
			robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_CTRL5, &val8, sizeof(val8));

			pdesc = pdesc25;
			pdescsz = sizeof(pdesc25) / sizeof(pdesc_t);
		} else {
			/* Initialize the MAC Addr Index Register */
			robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_MINDX, \
					     arl_entry, ETHER_ADDR_LEN);

			pdesc = pdesc97;
			pdescsz = sizeof(pdesc97) / sizeof(pdesc_t);
		}

		/* setup each vlan. max. 16 vlans. */
		/* force vlan id to be equal to vlan number */
		for (vid = 0; vid < VLAN_NUMVLANS; vid ++) {

			/* Add static ARL entries */
			if (robo->devid == DEVID5325) {
				val8 = vid;
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_VID_E0, \
						     &val8, sizeof(val8));
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_VINDX, \
						     &val8, sizeof(val8));

				/* Write the entry */
				val8 = 0x80;
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_CTRL, \
						     &val8, sizeof(val8));
				/* Wait for write to complete */
				SPINWAIT((robo->ops->read_reg(robo, PAGE_VTBL, REG_VTBL_CTRL, \
					 &val8, sizeof(val8)), ((val8 & 0x80) != 0)),
					 100 /* usec */);
			} else {
				/* Set the VLAN Id in VLAN ID Index Register */
				val8 = vid;
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_VINDX, \
						     &val8, sizeof(val8));

				/* Set the MAC addr and VLAN Id in ARL Table MAC/VID Entry 0
				 * Register.
				 */
				arl_entry[6] = vid;
				arl_entry[7] = 0x0;
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_ARL_E0, \
						     arl_entry, sizeof(arl_entry));

				/* Set the Static bit , Valid bit and Port ID fields in
				 * ARL Table Data Entry 0 Register
				 */
				val16 = 0xc008;
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_DAT_E0, \
						     &val16, sizeof(val16));

				/* Clear the ARL_R/W bit and set the START/DONE bit in
				 * the ARL Read/Write Control Register.
				 */
				val8 = 0x80;
				robo->ops->write_reg(robo, PAGE_VTBL, REG_VTBL_CTRL, \
						     &val8, sizeof(val8));
				/* Wait for write to complete */
				SPINWAIT((robo->ops->read_reg(robo, PAGE_VTBL, REG_VTBL_CTRL, \
					 &val8, sizeof(val8)), ((val8 & 0x80) != 0)),
					 100 /* usec */);
			}
		}
	}

	ROBO_END(driver);
	return 0;
}

static int handle_vlan_port_read(void *driver, char *buf, int nr)
{
	/* FIXME: yeah, some work is missing here */
	return sprintf(buf, "bcmrobo.c: handle_vlan_port_read unimplimented\n");
}

static int handle_vlan_port_write(void *driver, char *buf, int nr)
{

	switch_driver *d = (switch_driver *) driver;
	switch_vlan_config *c = switch_parse_vlan(d, buf);

	uint8 val8;
	uint16 val16;
	uint32 val32;
	int j;
	pdesc_t *pdesc;
	int pdescsz;

/* printk(KERN_WARNING "bcmrobo.c: handle_vlan_port_write, nr %d\n", nr); */

        if (c == NULL)
                return -EINVAL;

	ROBO_START(driver);

	if (robo->devid == DEVID5325) {
		pdesc = pdesc25;
		pdescsz = sizeof(pdesc25) / sizeof(pdesc_t);
	} else {
		pdesc = pdesc97;
		pdescsz = sizeof(pdesc97) / sizeof(pdesc_t);
	}


        for (j = 0; j < d->ports; j++) {
                if ((c->untag | c->pvid) & (1 << j))
			if ((j != d->cpuport) || (c->untag & (1 << j))) {

				/* change default vlan tag */

/* printk(KERN_WARNING "bcmrobo.c: set default vlan tag, port %d -> vlan %d\n", j, nr); */

				val16 = ((0 << 13) |		/* priority - always 0 */
					nr);			/* vlan id */
				robo->ops->write_reg(robo, PAGE_VLAN, pdesc[j].ptagr, &val16, sizeof(val16));
			}
        }


	if (robo->devid == DEVID5325) {
		val32 = ((c->untag << 6) |		/* untag enable */
			 c->port);			/* vlan members */
		val32 |= ((1 << 20) |           /* valid write */
			  ((nr >> 4) << 12));  /* vlan id bit[11:4] */
		/* VLAN Write Register (Page 0x34, Address 0x08-0x0B) */
		robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_WRITE, &val32,
				     sizeof(val32));
		/* VLAN Table Access Register (Page 0x34, Address 0x06-0x07) */
		val16 = ((1 << 13) |    /* start command */
			 (1 << 12) |    /* write state */
			 nr);          /* vlan id */
		robo->ops->write_reg(robo, PAGE_VLAN, REG_VLAN_ACCESS, &val16,
				     sizeof(val16));
	} else {
		uint8 vtble, vtbli, vtbla;
		val32 = ((c->untag << 9) |		/* untag enable */
			 c->port);			/* vlan members */

		if (robo->devid == DEVID5395) {
			vtble = REG_VTBL_ENTRY_5395;
			vtbli = REG_VTBL_INDX_5395;
			vtbla = REG_VTBL_ACCESS_5395;
		} else {
			vtble = REG_VTBL_ENTRY;
			vtbli = REG_VTBL_INDX;
			vtbla = REG_VTBL_ACCESS;
		}

		/* VLAN Table Entry Register (Page 0x05, Address 0x63-0x66/0x83-0x86) */
		robo->ops->write_reg(robo, PAGE_VTBL, vtble, &val32,
				     sizeof(val32));
		/* VLAN Table Address Index Reg (Page 0x05, Address 0x61-0x62/0x81-0x82) */
		val16 = nr;        /* vlan id */
		robo->ops->write_reg(robo, PAGE_VTBL, vtbli, &val16,
				     sizeof(val16));

		/* VLAN Table Access Register (Page 0x34, Address 0x60/0x80) */
		val8 = ((1 << 7) |      /* start command */
			0);             /* write */
		robo->ops->write_reg(robo, PAGE_VTBL, vtbla, &val8,
				     sizeof(val8));
	}

	ROBO_END(driver);
	return 0;
}

static int __init config_attach(robo_info_t *robo)
{
	switch_config cfg[] = {
		{"enable", handle_enable_read, handle_enable_write},
		{"reset", NULL, handle_reset},
		{"enable_vlan", handle_enable_vlan_read, handle_enable_vlan_write},
		{NULL, NULL, NULL}
	};
	switch_config vlan[] = {
		{"ports", handle_vlan_port_read, handle_vlan_port_write},
		{NULL, NULL, NULL}
	};
	switch_driver driver = {
		name: DRIVER_NAME,
		version: DRIVER_VERSION,
		interface: robo->name,
		cpuport: 8,
		ports: 9,
		vlans: 16,
		driver_handlers: cfg,
		port_handlers: NULL,
		vlan_handlers: vlan,
	};
	if (robo->devid == DEVID5325) {
		driver.ports = 6;
		driver.cpuport = 5;
	}
	driver.priv = (void *) robo;

	return switch_register_driver(&driver);
}

static void __exit config_detach(robo_info_t *robo)
{
	switch_unregister_driver(DRIVER_NAME);
}


