/*
 * Hardware-specific External Interface I/O core definitions
 * for the BCM47xx family of SiliconBackplane-based chips.
 *
 * The External Interface core supports a total of three external chip selects
 * supporting external interfaces. One of the external chip selects is
 * used for Flash, one is used for PCMCIA, and the other may be
 * programmed to support either a synchronous interface or an
 * asynchronous interface. The asynchronous interface can be used to
 * support external devices such as UARTs and the BCM2019 Bluetooth
 * baseband processor.
 * The external interface core also contains 2 on-chip 16550 UARTs, clock
 * frequency control, a watchdog interrupt timer, and a GPIO interface.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef	_SBEXTIF_H
#define	_SBEXTIF_H

/* external interface address space */
#define	EXTIF_PCMCIA_MEMBASE(x)	(x)
#define	EXTIF_PCMCIA_IOBASE(x)	((x) + 0x100000)
#define	EXTIF_PCMCIA_CFGBASE(x)	((x) + 0x200000)
#define	EXTIF_CFGIF_BASE(x)	((x) + 0x800000)
#define	EXTIF_FLASH_BASE(x)	((x) + 0xc00000)

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

/*
 * The multiple instances of output and output enable registers
 * are present to allow driver software for multiple cores to control
 * gpio outputs without needing to share a single register pair.
 */
struct gpiouser {
	uint32	out;
	uint32	outen;
};
#define	NGPIOUSER	5

typedef volatile struct {
	uint32	corecontrol;
	uint32	extstatus;
	uint32	PAD[2];

	/* pcmcia control registers */
	uint32	pcmcia_config;
	uint32	pcmcia_memwait;
	uint32	pcmcia_attrwait;
	uint32	pcmcia_iowait;

	/* programmable interface control registers */
	uint32	prog_config;
	uint32	prog_waitcount;

	/* flash control registers */
	uint32	flash_config;
	uint32	flash_waitcount;
	uint32	PAD[4];

	uint32	watchdog;

	/* clock control */
	uint32	clockcontrol_n;
	uint32	clockcontrol_sb;
	uint32	clockcontrol_pci;
	uint32	clockcontrol_mii;
	uint32	PAD[3];

	/* gpio */
	uint32	gpioin;
	struct gpiouser	gpio[NGPIOUSER];
	uint32	PAD;
	uint32	ejtagouten;
	uint32	gpiointpolarity;
	uint32	gpiointmask;
	uint32	PAD[153];

	uint8	uartdata;
	uint8	PAD[3];
	uint8	uartimer;
	uint8	PAD[3];
	uint8	uartfcr;
	uint8	PAD[3];
	uint8	uartlcr;
	uint8	PAD[3];
	uint8	uartmcr;
	uint8	PAD[3];
	uint8	uartlsr;
	uint8	PAD[3];
	uint8	uartmsr;
	uint8	PAD[3];
	uint8	uartscratch;
	uint8	PAD[3];
} extifregs_t;

/* corecontrol */
#define	CC_UE		(1 << 0)		/* uart enable */

/* extstatus */
#define	ES_EM		(1 << 0)		/* endian mode (ro) */
#define	ES_EI		(1 << 1)		/* external interrupt pin (ro) */
#define	ES_GI		(1 << 2)		/* gpio interrupt pin (ro) */

/* gpio bit mask */
#define GPIO_BIT0	(1 << 0)
#define GPIO_BIT1	(1 << 1)
#define GPIO_BIT2	(1 << 2)
#define GPIO_BIT3	(1 << 3)
#define GPIO_BIT4	(1 << 4)
#define GPIO_BIT5	(1 << 5)
#define GPIO_BIT6	(1 << 6)
#define GPIO_BIT7	(1 << 7)


/* pcmcia/prog/flash_config */
#define	CF_EN		(1 << 0)		/* enable */
#define	CF_EM_MASK	0xe			/* mode */
#define	CF_EM_SHIFT	1
#define	CF_EM_FLASH	0x0			/* flash/asynchronous mode */
#define	CF_EM_SYNC	0x2			/* synchronous mode */
#define	CF_EM_PCMCIA	0x4			/* pcmcia mode */
#define	CF_DS		(1 << 4)		/* destsize:  0=8bit, 1=16bit */
#define	CF_BS		(1 << 5)		/* byteswap */
#define	CF_CD_MASK	0xc0			/* clock divider */
#define	CF_CD_SHIFT	6
#define	CF_CD_DIV2	0x0			/* backplane/2 */
#define	CF_CD_DIV3	0x40			/* backplane/3 */
#define	CF_CD_DIV4	0x80			/* backplane/4 */
#define	CF_CE		(1 << 8)		/* clock enable */
#define	CF_SB		(1 << 9)		/* size/bytestrobe (synch only) */

/* pcmcia_memwait */
#define	PM_W0_MASK	0x3f			/* waitcount0 */
#define	PM_W1_MASK	0x1f00			/* waitcount1 */
#define	PM_W1_SHIFT	8
#define	PM_W2_MASK	0x1f0000		/* waitcount2 */
#define	PM_W2_SHIFT	16
#define	PM_W3_MASK	0x1f000000		/* waitcount3 */
#define	PM_W3_SHIFT	24

/* pcmcia_attrwait */
#define	PA_W0_MASK	0x3f			/* waitcount0 */
#define	PA_W1_MASK	0x1f00			/* waitcount1 */
#define	PA_W1_SHIFT	8
#define	PA_W2_MASK	0x1f0000		/* waitcount2 */
#define	PA_W2_SHIFT	16
#define	PA_W3_MASK	0x1f000000		/* waitcount3 */
#define	PA_W3_SHIFT	24

/* prog_waitcount */
#define	PW_W0_MASK	0x0000001f			/* waitcount0 */
#define	PW_W1_MASK	0x00001f00			/* waitcount1 */
#define	PW_W1_SHIFT	8
#define	PW_W2_MASK	0x001f0000		/* waitcount2 */
#define	PW_W2_SHIFT	16
#define	PW_W3_MASK	0x1f000000		/* waitcount3 */
#define	PW_W3_SHIFT	24

#define PW_W0       0x0000000c
#define PW_W1       0x00000a00
#define PW_W2       0x00020000
#define PW_W3       0x01000000

/* flash_waitcount */
#define	FW_W0_MASK	0x1f			/* waitcount0 */
#define	FW_W1_MASK	0x1f00			/* waitcount1 */
#define	FW_W1_SHIFT	8
#define	FW_W2_MASK	0x1f0000		/* waitcount2 */
#define	FW_W2_SHIFT	16
#define	FW_W3_MASK	0x1f000000		/* waitcount3 */
#define	FW_W3_SHIFT	24

/* watchdog */
#define WATCHDOG_CLOCK	48000000		/* Hz */

/* clockcontrol_n */
#define	CN_N1_MASK	0x3f			/* n1 control */
#define	CN_N2_MASK	0x3f00			/* n2 control */
#define	CN_N2_SHIFT	8

/* clockcontrol_sb/pci/mii */
#define	CC_M1_MASK	0x3f			/* m1 control */
#define	CC_M2_MASK	0x3f00			/* m2 control */
#define	CC_M2_SHIFT	8
#define	CC_M3_MASK	0x3f0000		/* m3 control */
#define	CC_M3_SHIFT	16
#define	CC_MC_MASK	0x1f000000		/* mux control */
#define	CC_MC_SHIFT	24

/* Clock control default values */
#define CC_DEF_N	0x0009			/* Default values for bcm4710 */
#define CC_DEF_100	0x04020011
#define CC_DEF_33	0x11030011
#define CC_DEF_25	0x11050011

/* Clock control values for 125Mhz */
#define	CC_125_N	0x0802
#define	CC_125_M	0x04020009
#define	CC_125_M25	0x11090009
#define	CC_125_M33	0x11090005

/* Clock control magic field values */
#define	CC_F6_2		0x02			/* A factor of 2 in */
#define	CC_F6_3		0x03			/*  6-bit fields like */
#define	CC_F6_4		0x05			/*  N1, M1 or M3 */
#define	CC_F6_5		0x09
#define	CC_F6_6		0x11
#define	CC_F6_7		0x21

#define	CC_F5_BIAS	5			/* 5-bit fields get this added */

#define	CC_MC_BYPASS	0x08
#define	CC_MC_M1	0x04
#define	CC_MC_M1M2	0x02
#define	CC_MC_M1M2M3	0x01
#define	CC_MC_M1M3	0x11

#define	CC_CLOCK_BASE	24000000	/* Half the clock freq. in the 4710 */

#endif	/* _SBEXTIF_H */
