/*
 * SiliconBackplane Chipcommon core hardware definitions.
 *
 * The chipcommon core provides chip identification, SB control,
 * jtag, 0/1/2 uarts, clock frequency control, a watchdog interrupt timer,
 * gpio interface, extbus, and support for serial and parallel flashes.
 *
 * $Id$
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef	_SBCHIPC_H
#define	_SBCHIPC_H


#ifndef _LANGUAGE_ASSEMBLY

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	/* PAD */

typedef volatile struct {
	uint32	chipid;			/* 0x0 */
	uint32	capabilities;
	uint32	corecontrol;		/* corerev >= 1 */
	uint32	bist;

	/* OTP */
	uint32	otpstatus;		/* 0x10, corerev >= 10 */
	uint32	otpcontrol;
	uint32	otpprog;
	uint32	PAD;

	/* Interrupt control */
	uint32	intstatus;		/* 0x20 */
	uint32	intmask;
	uint32	PAD[2];

	/* Jtag Master */
	uint32	jtagcmd;		/* 0x30, rev >= 10 */
	uint32	jtagir;
	uint32	jtagdr;
	uint32	jtagctrl;

	/* serial flash interface registers */
	uint32	flashcontrol;		/* 0x40 */
	uint32	flashaddress;
	uint32	flashdata;
	uint32	PAD[1];

	/* Silicon backplane configuration broadcast control */
	uint32	broadcastaddress;	/* 0x50 */
	uint32	broadcastdata;
	uint32	PAD[2];

	/* gpio - cleared only by power-on-reset */
	uint32	gpioin;			/* 0x60 */
	uint32	gpioout;
	uint32	gpioouten;
	uint32	gpiocontrol;
	uint32	gpiointpolarity;
	uint32	gpiointmask;
	uint32	PAD[2];

	/* Watchdog timer */
	uint32	watchdog;		/* 0x80 */
	uint32	PAD[3];

	/* clock control */
	uint32	clockcontrol_n;		/* 0x90 */
	uint32	clockcontrol_sb;	/* aka m0 */
	uint32	clockcontrol_pci;	/* aka m1 */
	uint32	clockcontrol_m2;	/* mii/uart/mipsref */
	uint32	clockcontrol_mips;	/* aka m3 */
	uint32	clkdiv;			/* corerev >= 3 */
	uint32	PAD[2];

	/* pll delay registers (corerev >= 4) */
	uint32	pll_on_delay;		/* 0xb0 */
	uint32	fref_sel_delay;
	uint32	slow_clk_ctl;		/* 5 < corerev < 10 */
	uint32	PAD[1];

	/* Instaclock registers (corerev >= 10) */
	uint32	system_clk_ctl;		/* 0xc0 */
	uint32	clkstatestretch;
	uint32	PAD[14];

	/* ExtBus control registers (corerev >= 3) */
	uint32	pcmcia_config;		/* 0x100 */
	uint32	pcmcia_memwait;
	uint32	pcmcia_attrwait;
	uint32	pcmcia_iowait;
	uint32	ide_config;
	uint32	ide_memwait;
	uint32	ide_attrwait;
	uint32	ide_iowait;
	uint32	prog_config;
	uint32	prog_waitcount;
	uint32	flash_config;
	uint32	flash_waitcount;
	uint32	PAD[116];

	/* uarts */
	uint8	uart0data;		/* 0x300 */
	uint8	uart0imr;
	uint8	uart0fcr;
	uint8	uart0lcr;
	uint8	uart0mcr;
	uint8	uart0lsr;
	uint8	uart0msr;
	uint8	uart0scratch;
	uint8	PAD[248];		/* corerev >= 1 */

	uint8	uart1data;		/* 0x400 */
	uint8	uart1imr;
	uint8	uart1fcr;
	uint8	uart1lcr;
	uint8	uart1mcr;
	uint8	uart1lsr;
	uint8	uart1msr;
	uint8	uart1scratch;
} chipcregs_t;

#endif /* _LANGUAGE_ASSEMBLY */

#define	CC_CHIPID		0
#define	CC_CAPABILITIES		4
#define	CC_JTAGCMD		0x30
#define	CC_JTAGIR		0x34
#define	CC_JTAGDR		0x38
#define	CC_JTAGCTRL		0x3c
#define	CC_CLKDIV		0xa4

/* chipid */
#define	CID_ID_MASK		0x0000ffff		/* Chip Id mask */
#define	CID_REV_MASK		0x000f0000		/* Chip Revision mask */
#define	CID_REV_SHIFT		16			/* Chip Revision shift */
#define	CID_PKG_MASK		0x00f00000		/* Package Option mask */
#define	CID_PKG_SHIFT		20			/* Package Option shift */
#define	CID_CC_MASK		0x0f000000		/* CoreCount (corerev >= 4) */
#define CID_CC_SHIFT		24

/* capabilities */
#define	CAP_UARTS_MASK		0x00000003		/* Number of uarts */
#define CAP_MIPSEB		0x00000004		/* MIPS is in big-endian mode */
#define CAP_UCLKSEL		0x00000018		/* UARTs clock select */
#define CAP_UINTCLK		0x00000008		/* UARTs are driven by internal divided clock */
#define CAP_UARTGPIO		0x00000020		/* UARTs own Gpio's 15:12 */
#define CAP_EXTBUS		0x00000040		/* External bus present */
#define	CAP_FLASH_MASK		0x00000700		/* Type of flash */
#define	CAP_PLL_MASK		0x00038000		/* Type of PLL */
#define CAP_PWR_CTL		0x00040000		/* Power control */
#define CAP_OTPSIZE		0x00380000		/* OTP Size (0 = none) */
#define CAP_JTAGP		0x00400000		/* JTAG Master Present */
#define CAP_ROM			0x00800000		/* Internal boot rom active */

/* PLL type */
#define PLL_NONE		0x00000000
#define PLL_TYPE1		0x00010000		/* 48Mhz base, 3 dividers */
#define PLL_TYPE2		0x00020000		/* 48Mhz, 4 dividers */
#define PLL_TYPE3		0x00030000		/* 25Mhz, 2 dividers */
#define PLL_TYPE4		0x00008000		/* 48Mhz, 4 dividers */

/* corecontrol */
#define CC_UARTCLKO		0x00000001		/* Drive UART with internal clock */
#define	CC_SE			0x00000002		/* sync clk out enable (corerev >= 3) */

/* jtagcmd */
#define JCMD_START		0x80000000
#define JCMD_BUSY		0x80000000
#define JCMD_PAUSE		0x40000000
#define JCMD0_ACC_MASK		0x0000f000
#define JCMD0_ACC_IRDR		0x00000000
#define JCMD0_ACC_DR		0x00001000
#define JCMD0_ACC_IR		0x00002000
#define JCMD0_ACC_RESET		0x00003000
#define JCMD0_ACC_IRPDR		0x00004000
#define JCMD0_ACC_PDR		0x00005000
#define JCMD0_IRW_MASK		0x00000f00
#define JCMD_ACC_MASK		0x000f0000		/* Changes for corerev 11 */
#define JCMD_ACC_IRDR		0x00000000
#define JCMD_ACC_DR		0x00010000
#define JCMD_ACC_IR		0x00020000
#define JCMD_ACC_RESET		0x00030000
#define JCMD_ACC_IRPDR		0x00040000
#define JCMD_ACC_PDR		0x00050000
#define JCMD_IRW_MASK		0x00001f00
#define JCMD_IRW_SHIFT		8
#define JCMD_DRW_MASK		0x0000003f

/* jtagctrl */
#define JCTRL_FORCE_CLK		4			/* Force clock */
#define JCTRL_EXT_EN		2			/* Enable external targets */
#define JCTRL_EN		1			/* Enable Jtag master */

/* Fields in clkdiv */
#define	CLKD_SFLASH		0x0f000000
#define	CLKD_SFLASH_SHIFT		24
#define	CLKD_OTP		0x000f0000
#define	CLKD_OTP_SHIFT		16
#define	CLKD_JTAG		0x00000f00
#define	CLKD_JTAG_SHIFT		8		
#define	CLKD_UART		0x000000ff

/* intstatus/intmask */
#define	CI_GPIO			0x00000001		/* gpio intr */
#define	CI_EI			0x00000002		/* ro: ext intr pin (corerev >= 3) */
#define	CI_WDREST		0x80000000		/* watchdog reset occurred */

/* slow_clk_ctl */
#define SCC_SS_MASK		0x00000007		/* slow clock source mask */
#define	SCC_SS_LPO		0x00000000		/* source of slow clock is LPO */
#define	SCC_SS_XTAL		0x00000001		/* source of slow clock is crystal */
#define	SCC_SS_PCI		0x00000002		/* source of slow clock is PCI */
#define SCC_LF			0x00000200		/* LPOFreqSel, 1: 160Khz, 0: 32KHz */
#define SCC_LP			0x00000400		/* LPOPowerDown, 1: LPO is disabled, 0: LPO is enabled */
#define SCC_FS			0x00000800		/* ForceSlowClk, 1: sb/cores running on slow clock, 0: power logic control */
#define SCC_IP			0x00001000		/* IgnorePllOffReq, 1/0: power logic ignores/honors PLL clock disable requests from core */
#define SCC_XC			0x00002000		/* XtalControlEn, 1/0: power logic does/doesn't disable crystal when appropriate */
#define SCC_XP			0x00004000		/* XtalPU (RO), 1/0: crystal running/disabled */
#define SCC_CD_MASK		0xffff0000		/* ClockDivider mask, SlowClk = 1/(4+divisor) * crystal/PCI clock */
#define SCC_CD_SHF		16			/* CLockDivider shift */

/* sys_clk_ctl */
#define	SYCC_IE			0x00000001		/* ILPen: Enable Idle Low Power */
#define	SYCC_AE			0x00000002		/* ALPen: Enable Active Low Power */
#define	SYCC_FP			0x00000004		/* ForcePLLOn */
#define	SYCC_AR			0x00000008		/* Force ALP (or HT if ALPen is not set */
#define	SYCC_HR			0x00000010		/* Force HT */
#define SYCC_CD_MASK		0xffff0000		/* ClockDivider mask, SlowClk = 1/(4+divisor) * crystal/PCI clock */
#define SYCC_CD_SHF		16			/* CLockDivider shift */

/* clockcontrol_n */
#define	CN_N1_MASK		0x3f			/* n1 control */
#define	CN_N2_MASK		0x3f00			/* n2 control */
#define	CN_N2_SHIFT		8

/* clockcontrol_sb/pci/uart */
#define	CC_M1_MASK		0x3f			/* m1 control */
#define	CC_M2_MASK		0x3f00			/* m2 control */
#define	CC_M2_SHIFT		8
#define	CC_M3_MASK		0x3f0000		/* m3 control */
#define	CC_M3_SHIFT		16
#define	CC_MC_MASK		0x1f000000		/* mux control */
#define	CC_MC_SHIFT		24

/* N3M Clock control values for 125Mhz */
#define	CC_125_N		0x0802			/* Default values for bcm4310 */
#define	CC_125_M		0x04020009
#define	CC_125_M25		0x11090009
#define	CC_125_M33		0x11090005

/* N3M Clock control magic field values */
#define	CC_F6_2			0x02			/* A factor of 2 in */
#define	CC_F6_3			0x03			/* 6-bit fields like */
#define	CC_F6_4			0x05			/* N1, M1 or M3 */
#define	CC_F6_5			0x09
#define	CC_F6_6			0x11
#define	CC_F6_7			0x21

#define	CC_F5_BIAS		5			/* 5-bit fields get this added */

#define	CC_MC_BYPASS		0x08
#define	CC_MC_M1		0x04
#define	CC_MC_M1M2		0x02
#define	CC_MC_M1M2M3		0x01
#define	CC_MC_M1M3		0x11

/* Type 2 Clock control magic field values */
#define	CC_T2_BIAS		2			/* n1, n2, m1 & m3 bias */
#define	CC_T2M2_BIAS		3			/* m2 bias */

#define	CC_T2MC_M1BYP		1
#define	CC_T2MC_M2BYP		2
#define	CC_T2MC_M3BYP		4

/* Common clock base */
#define	CC_CLOCK_BASE		24000000		/* Half the clock freq */


/* Flash types in the chipcommon capabilities register */
#define FLASH_NONE		0x000		/* No flash */
#define SFLASH_ST		0x100		/* ST serial flash */
#define SFLASH_AT		0x200		/* Atmel serial flash */
#define	PFLASH			0x700		/* Parallel flash */

/* Prefered flash window in chipcommon */
#define	CC_FLASH_BASE		0xbc000000	/* Chips with chipcommon cores */
#define CC_FLASH_MAX		0x02000000	/* Maximum flash size with chipc */


/* Bits in the config registers */
#define	CC_CFG_EN		0x0001		/* Enable */
#define	CC_CFG_EM_MASK		0x000e		/* Extif Mode */
#define	CC_CFG_EM_ASYNC		0x0002		/*   Async/Parallel flash */
#define	CC_CFG_EM_SYNC		0x0004		/*   Synchronous */
#define	CC_CFG_EM_PCMCIA	0x0008		/*   PCMCIA */
#define	CC_CFG_EM_IDE		0x000a		/*   IDE */
#define	CC_CFG_DS		0x0010		/* Data size, 0=8bit, 1=16bit */
#define	CC_CFG_CD_MASK		0x0060		/* Sync: Clock divisor */
#define	CC_CFG_CE		0x0080		/* Sync: Clock enable */
#define	CC_CFG_SB		0x0100		/* Sync: Size/Bytestrobe */

/* Start/busy bit in flashcontrol */
#define SFLASH_START		0x80000000
#define SFLASH_BUSY		SFLASH_START

/* flashcontrol opcodes for ST flashes */
#define SFLASH_ST_WREN		0x0006		/* Write Enable */
#define SFLASH_ST_WRDIS		0x0004		/* Write Disable */
#define SFLASH_ST_RDSR		0x0105		/* Read Status Register */
#define SFLASH_ST_WRSR		0x0101		/* Write Status Register */
#define SFLASH_ST_READ		0x0303		/* Read Data Bytes */
#define SFLASH_ST_PP		0x0302		/* Page Program */
#define SFLASH_ST_SE		0x02d8		/* Sector Erase */
#define SFLASH_ST_BE		0x00c7		/* Bulk Erase */
#define SFLASH_ST_DP		0x00b9		/* Deep Power-down */
#define SFLASH_ST_RES		0x03ab		/* Read Electronic Signature */

/* Status register bits for ST flashes */
#define SFLASH_ST_WIP		0x01		/* Write In Progress */
#define SFLASH_ST_WEL		0x02		/* Write Enable Latch */
#define SFLASH_ST_BP_MASK	0x1c		/* Block Protect */
#define SFLASH_ST_BP_SHIFT	2
#define SFLASH_ST_SRWD		0x80		/* Status Register Write Disable */

/* flashcontrol opcodes for Atmel flashes */
#define SFLASH_AT_READ				0x07e8
#define SFLASH_AT_PAGE_READ			0x07d2
#define SFLASH_AT_BUF1_READ
#define SFLASH_AT_BUF2_READ
#define SFLASH_AT_STATUS			0x01d7
#define SFLASH_AT_BUF1_WRITE			0x0384
#define SFLASH_AT_BUF2_WRITE			0x0387
#define SFLASH_AT_BUF1_ERASE_PROGRAM		0x0283
#define SFLASH_AT_BUF2_ERASE_PROGRAM		0x0286
#define SFLASH_AT_BUF1_PROGRAM			0x0288
#define SFLASH_AT_BUF2_PROGRAM			0x0289
#define SFLASH_AT_PAGE_ERASE			0x0281
#define SFLASH_AT_BLOCK_ERASE			0x0250
#define SFLASH_AT_BUF1_WRITE_ERASE_PROGRAM	0x0382
#define SFLASH_AT_BUF2_WRITE_ERASE_PROGRAM	0x0385
#define SFLASH_AT_BUF1_LOAD			0x0253
#define SFLASH_AT_BUF2_LOAD			0x0255
#define SFLASH_AT_BUF1_COMPARE			0x0260
#define SFLASH_AT_BUF2_COMPARE			0x0261
#define SFLASH_AT_BUF1_REPROGRAM		0x0258
#define SFLASH_AT_BUF2_REPROGRAM		0x0259

/* Status register bits for Atmel flashes */
#define SFLASH_AT_READY				0x80
#define SFLASH_AT_MISMATCH			0x40
#define SFLASH_AT_ID_MASK			0x38
#define SFLASH_AT_ID_SHIFT			3

#endif	/* _SBCHIPC_H */
