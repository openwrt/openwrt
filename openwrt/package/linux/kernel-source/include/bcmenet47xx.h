/*
 * Hardware-specific definitions for
 * Broadcom BCM47XX 10/100 Mbps Ethernet cores.
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id$
 */

#ifndef	_bcmenet_47xx_h_
#define	_bcmenet_47xx_h_

#include <bcmdevs.h>
#include <hnddma.h>

#define	BCMENET_NFILTERS	64		/* # ethernet address filter entries */
#define	BCMENET_MCHASHBASE	0x200		/* multicast hash filter base address */
#define	BCMENET_MCHASHSIZE	256		/* multicast hash filter size in bytes */
#define	BCMENET_MAX_DMA		4096		/* chip has 12 bits of DMA addressing */

/* power management event wakeup pattern constants */
#define	BCMENET_NPMP		4		/* chip supports 4 wakeup patterns */
#define	BCMENET_PMPBASE		0x400		/* wakeup pattern base address */
#define	BCMENET_PMPSIZE		0x80		/* 128bytes each pattern */
#define	BCMENET_PMMBASE		0x600		/* wakeup mask base address */
#define	BCMENET_PMMSIZE		0x10		/* 128bits each mask */

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/* sometimes you just need the enet mib definitions */
#include <bcmenetmib.h>

/*
 * Host Interface Registers
 */
typedef volatile struct _bcmenettregs {
	/* Device and Power Control */
	uint32	devcontrol;
	uint32	PAD[2];
	uint32	biststatus;
	uint32	wakeuplength;
	uint32	PAD[3];
	
	/* Interrupt Control */
	uint32	intstatus;
	uint32	intmask;
	uint32	gptimer;
	uint32	PAD[23];

	/* Ethernet MAC Address Filtering Control */
	uint32	PAD[2];
	uint32	enetftaddr;
	uint32	enetftdata;
	uint32	PAD[2];

	/* Ethernet MAC Control */
	uint32	emactxmaxburstlen;
	uint32	emacrxmaxburstlen;
	uint32	emaccontrol;
	uint32	emacflowcontrol;

	uint32	PAD[20];

	/* DMA Lazy Interrupt Control */
	uint32	intrecvlazy;
	uint32	PAD[63];

	/* DMA engine */
	dmaregs_t	dmaregs;
	dmafifo_t	dmafifo;
	uint32	PAD[116];

	/* EMAC Registers */
	uint32 rxconfig;
	uint32 rxmaxlength;
	uint32 txmaxlength;
	uint32 PAD;
	uint32 mdiocontrol;
	uint32 mdiodata;
	uint32 emacintmask;
	uint32 emacintstatus;
	uint32 camdatalo;
	uint32 camdatahi;
	uint32 camcontrol;
	uint32 enetcontrol;
	uint32 txcontrol;
	uint32 txwatermark;
	uint32 mibcontrol;
	uint32 PAD[49];

	/* EMAC MIB counters */
	bcmenetmib_t	mib;

	uint32	PAD[585];

	/* Sonics SiliconBackplane config registers */
	sbconfig_t	sbconfig;
} bcmenetregs_t;

/* device control */
#define	DC_PM		((uint32)1 << 7)	/* pattern filtering enable */
#define	DC_IP		((uint32)1 << 10)	/* internal ephy present (rev >= 1) */
#define	DC_ER		((uint32)1 << 15)	/* ephy reset */
#define	DC_MP		((uint32)1 << 16)	/* mii phy mode enable */
#define	DC_CO		((uint32)1 << 17)	/* mii phy mode: enable clocks */
#define	DC_PA_MASK	0x7c0000		/* mii phy mode: mdc/mdio phy address */
#define	DC_PA_SHIFT	18

/* wakeup length */
#define	WL_P0_MASK	0x7f			/* pattern 0 */
#define	WL_D0		((uint32)1 << 7)
#define	WL_P1_MASK	0x7f00			/* pattern 1 */
#define	WL_P1_SHIFT	8
#define	WL_D1		((uint32)1 << 15)
#define	WL_P2_MASK	0x7f0000		/* pattern 2 */
#define	WL_P2_SHIFT	16
#define	WL_D2		((uint32)1 << 23)
#define	WL_P3_MASK	0x7f000000		/* pattern 3 */
#define	WL_P3_SHIFT	24
#define	WL_D3		((uint32)1 << 31)

/* intstatus and intmask */
#define	I_PME		((uint32)1 << 6)	/* power management event */
#define	I_TO		((uint32)1 << 7)	/* general purpose timeout */
#define	I_PC		((uint32)1 << 10)	/* descriptor error */
#define	I_PD		((uint32)1 << 11)	/* data error */
#define	I_DE		((uint32)1 << 12)	/* descriptor protocol error */
#define	I_RU		((uint32)1 << 13)	/* receive descriptor underflow */
#define	I_RO		((uint32)1 << 14)	/* receive fifo overflow */
#define	I_XU		((uint32)1 << 15)	/* transmit fifo underflow */
#define	I_RI		((uint32)1 << 16)	/* receive interrupt */
#define	I_XI		((uint32)1 << 24)	/* transmit interrupt */
#define	I_EM		((uint32)1 << 26)	/* emac interrupt */
#define	I_MW		((uint32)1 << 27)	/* mii write */
#define	I_MR		((uint32)1 << 28)	/* mii read */

/* emaccontrol */
#define	EMC_CG		((uint32)1 << 0)	/* crc32 generation enable */
#define	EMC_EP		((uint32)1 << 2)	/* onchip ephy: powerdown (rev >= 1) */
#define	EMC_ED		((uint32)1 << 3)	/* onchip ephy: energy detected (rev >= 1) */
#define	EMC_LC_MASK	0xe0			/* onchip ephy: led control (rev >= 1) */
#define	EMC_LC_SHIFT	5

/* emacflowcontrol */
#define	EMF_RFH_MASK	0xff			/* rx fifo hi water mark */
#define	EMF_PG		((uint32)1 << 15)	/* enable pause frame generation */

/* interrupt receive lazy */
#define	IRL_TO_MASK	0x00ffffff		/* timeout */
#define	IRL_FC_MASK	0xff000000		/* frame count */
#define	IRL_FC_SHIFT	24			/* frame count */

/* emac receive config */
#define	ERC_DB		((uint32)1 << 0)	/* disable broadcast */
#define	ERC_AM		((uint32)1 << 1)	/* accept all multicast */
#define	ERC_RDT		((uint32)1 << 2)	/* receive disable while transmitting */
#define	ERC_PE		((uint32)1 << 3)	/* promiscuous enable */
#define	ERC_LE		((uint32)1 << 4)	/* loopback enable */
#define	ERC_FE		((uint32)1 << 5)	/* enable flow control */
#define	ERC_UF		((uint32)1 << 6)	/* accept unicast flow control frame */
#define	ERC_RF		((uint32)1 << 7)	/* reject filter */

/* emac mdio control */
#define	MC_MF_MASK	0x7f			/* mdc frequency */
#define	MC_PE		((uint32)1 << 7)	/* mii preamble enable */

/* emac mdio data */
#define	MD_DATA_MASK	0xffff			/* r/w data */
#define	MD_TA_MASK	0x30000			/* turnaround value */
#define	MD_TA_SHIFT	16
#define	MD_TA_VALID	(2 << MD_TA_SHIFT)	/* valid ta */
#define	MD_RA_MASK	0x7c0000		/* register address */
#define	MD_RA_SHIFT	18
#define	MD_PMD_MASK	0xf800000		/* physical media device */
#define	MD_PMD_SHIFT	23
#define	MD_OP_MASK	0x30000000		/* opcode */
#define	MD_OP_SHIFT	28
#define	MD_OP_WRITE	(1 << MD_OP_SHIFT)	/* write op */
#define	MD_OP_READ	(2 << MD_OP_SHIFT)	/* read op */
#define	MD_SB_MASK	0xc0000000		/* start bits */
#define	MD_SB_SHIFT	30
#define	MD_SB_START	(0x1 << MD_SB_SHIFT)	/* start of frame */

/* emac intstatus and intmask */
#define	EI_MII		((uint32)1 << 0)	/* mii mdio interrupt */
#define	EI_MIB		((uint32)1 << 1)	/* mib interrupt */
#define	EI_FLOW		((uint32)1 << 2)	/* flow control interrupt */

/* emac cam data high */
#define	CD_V		((uint32)1 << 16)	/* valid bit */

/* emac cam control */
#define	CC_CE		((uint32)1 << 0)	/* cam enable */
#define	CC_MS		((uint32)1 << 1)	/* mask select */
#define	CC_RD		((uint32)1 << 2)	/* read */
#define	CC_WR		((uint32)1 << 3)	/* write */
#define	CC_INDEX_MASK	0x3f0000		/* index */
#define	CC_INDEX_SHIFT	16
#define	CC_CB		((uint32)1 << 31)	/* cam busy */

/* emac ethernet control */
#define	EC_EE		((uint32)1 << 0)	/* emac enable */
#define	EC_ED		((uint32)1 << 1)	/* emac disable */
#define	EC_ES		((uint32)1 << 2)	/* emac soft reset */
#define	EC_EP		((uint32)1 << 3)	/* external phy select */

/* emac transmit control */
#define	EXC_FD		((uint32)1 << 0)	/* full duplex */
#define	EXC_FM		((uint32)1 << 1)	/* flowmode */
#define	EXC_SB		((uint32)1 << 2)	/* single backoff enable */
#define	EXC_SS		((uint32)1 << 3)	/* small slottime */

/* emac mib control */
#define	EMC_RZ		((uint32)1 << 0)	/* autoclear on read */

/* sometimes you just need the enet rxheader definitions */
#include <bcmenetrxh.h>

#endif	/* _bcmenet_47xx_h_ */
