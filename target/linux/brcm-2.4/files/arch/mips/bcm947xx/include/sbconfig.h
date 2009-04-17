/*
 * Broadcom SiliconBackplane hardware register definitions.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef	_SBCONFIG_H
#define	_SBCONFIG_H
#include "linuxver.h"

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif

/*
 * SiliconBackplane Address Map.
 * All regions may not exist on all chips.
 */
#define SB_SDRAM_BASE		0x00000000	/* Physical SDRAM */
#define SB_PCI_MEM		0x08000000	/* Host Mode sb2pcitranslation0 (64 MB) */
#define SB_PCI_MEM_SZ		(64 * 1024 * 1024)
#define SB_PCI_CFG		0x0c000000	/* Host Mode sb2pcitranslation1 (64 MB) */
#define	SB_SDRAM_SWAPPED	0x10000000	/* Byteswapped Physical SDRAM */
#define SB_ENUM_BASE    	0x18000000	/* Enumeration space base */
#define	SB_ENUM_LIM		0x18010000	/* Enumeration space limit */

#define	SB_FLASH2		0x1c000000	/* Flash Region 2 (region 1 shadowed here) */
#define	SB_FLASH2_SZ		0x02000000	/* Size of Flash Region 2 */
#define	SB_EXTIF_BASE		0x1f000000	/* External Interface region base address */
#define	SB_ARMCM3_ROM		0x1e000000	/* ARM Cortex-M3 ROM */
#define	SB_FLASH1		0x1fc00000	/* MIPS Flash Region 1 */
#define	SB_FLASH1_SZ		0x00400000	/* MIPS Size of Flash Region 1 */
#define	SB_ARM7S_ROM		0x20000000	/* ARM7TDMI-S ROM */
#define	SB_ARMCM3_SRAM2		0x60000000	/* ARM Cortex-M3 SRAM Region 2 */
#define	SB_ARM7S_SRAM2		0x80000000	/* ARM7TDMI-S SRAM Region 2 */
#define	SB_ARM_FLASH1		0xffff0000	/* ARM Flash Region 1 */
#define	SB_ARM_FLASH1_SZ	0x00010000	/* ARM Size of Flash Region 1 */

#define SB_PCI_DMA		0x40000000	/* Client Mode sb2pcitranslation2 (1 GB) */
#define SB_PCI_DMA_SZ		0x40000000	/* Client Mode sb2pcitranslation2 size in bytes */
#define SB_PCIE_DMA_L32		0x00000000	/* PCIE Client Mode sb2pcitranslation2
						 * (2 ZettaBytes), low 32 bits
						 */
#define SB_PCIE_DMA_H32		0x80000000	/* PCIE Client Mode sb2pcitranslation2
						 * (2 ZettaBytes), high 32 bits
						 */
#define	SB_EUART		(SB_EXTIF_BASE + 0x00800000)
#define	SB_LED			(SB_EXTIF_BASE + 0x00900000)


/* enumeration space related defs */
#define SB_CORE_SIZE    	0x1000		/* each core gets 4Kbytes for registers */
#define	SB_MAXCORES		((SB_ENUM_LIM - SB_ENUM_BASE)/SB_CORE_SIZE)
#define SB_MAXFUNCS		4		/* max. # functions per core */
#define	SBCONFIGOFF		0xf00		/* core sbconfig regs are top 256bytes of regs */
#define	SBCONFIGSIZE		256		/* sizeof (sbconfig_t) */

/* mips address */
#define	SB_EJTAG		0xff200000	/* MIPS EJTAG space (2M) */

/*
 * Sonics Configuration Space Registers.
 */
#define SBIPSFLAG		0x08
#define SBTPSFLAG		0x18
#define	SBTMERRLOGA		0x48		/* sonics >= 2.3 */
#define	SBTMERRLOG		0x50		/* sonics >= 2.3 */
#define SBADMATCH3		0x60
#define SBADMATCH2		0x68
#define SBADMATCH1		0x70
#define SBIMSTATE		0x90
#define SBINTVEC		0x94
#define SBTMSTATELOW		0x98
#define SBTMSTATEHIGH		0x9c
#define SBBWA0			0xa0
#define SBIMCONFIGLOW		0xa8
#define SBIMCONFIGHIGH		0xac
#define SBADMATCH0		0xb0
#define SBTMCONFIGLOW		0xb8
#define SBTMCONFIGHIGH		0xbc
#define SBBCONFIG		0xc0
#define SBBSTATE		0xc8
#define SBACTCNFG		0xd8
#define	SBFLAGST		0xe8
#define SBIDLOW			0xf8
#define SBIDHIGH		0xfc

/* All the previous registers are above SBCONFIGOFF, but with Sonics 2.3, we have
 * a few registers *below* that line. I think it would be very confusing to try
 * and change the value of SBCONFIGOFF, so I'm definig them as absolute offsets here,
 */

#define SBIMERRLOGA		0xea8
#define SBIMERRLOG		0xeb0
#define SBTMPORTCONNID0		0xed8
#define SBTMPORTLOCK0		0xef8

#ifndef _LANGUAGE_ASSEMBLY

typedef volatile struct _sbconfig {
	uint32	PAD[2];
	uint32	sbipsflag;		/* initiator port ocp slave flag */
	uint32	PAD[3];
	uint32	sbtpsflag;		/* target port ocp slave flag */
	uint32	PAD[11];
	uint32	sbtmerrloga;		/* (sonics >= 2.3) */
	uint32	PAD;
	uint32	sbtmerrlog;		/* (sonics >= 2.3) */
	uint32	PAD[3];
	uint32	sbadmatch3;		/* address match3 */
	uint32	PAD;
	uint32	sbadmatch2;		/* address match2 */
	uint32	PAD;
	uint32	sbadmatch1;		/* address match1 */
	uint32	PAD[7];
	uint32	sbimstate;		/* initiator agent state */
	uint32	sbintvec;		/* interrupt mask */
	uint32	sbtmstatelow;		/* target state */
	uint32	sbtmstatehigh;		/* target state */
	uint32	sbbwa0;			/* bandwidth allocation table0 */
	uint32	PAD;
	uint32	sbimconfiglow;		/* initiator configuration */
	uint32	sbimconfighigh;		/* initiator configuration */
	uint32	sbadmatch0;		/* address match0 */
	uint32	PAD;
	uint32	sbtmconfiglow;		/* target configuration */
	uint32	sbtmconfighigh;		/* target configuration */
	uint32	sbbconfig;		/* broadcast configuration */
	uint32	PAD;
	uint32	sbbstate;		/* broadcast state */
	uint32	PAD[3];
	uint32	sbactcnfg;		/* activate configuration */
	uint32	PAD[3];
	uint32	sbflagst;		/* current sbflags */
	uint32	PAD[3];
	uint32	sbidlow;		/* identification */
	uint32	sbidhigh;		/* identification */
} sbconfig_t;

#endif /* _LANGUAGE_ASSEMBLY */

/* sbipsflag */
#define	SBIPS_INT1_MASK		0x3f		/* which sbflags get routed to mips interrupt 1 */
#define	SBIPS_INT1_SHIFT	0
#define	SBIPS_INT2_MASK		0x3f00		/* which sbflags get routed to mips interrupt 2 */
#define	SBIPS_INT2_SHIFT	8
#define	SBIPS_INT3_MASK		0x3f0000	/* which sbflags get routed to mips interrupt 3 */
#define	SBIPS_INT3_SHIFT	16
#define	SBIPS_INT4_MASK		0x3f000000	/* which sbflags get routed to mips interrupt 4 */
#define	SBIPS_INT4_SHIFT	24

/* sbtpsflag */
#define	SBTPS_NUM0_MASK		0x3f		/* interrupt sbFlag # generated by this core */
#define	SBTPS_F0EN0		0x40		/* interrupt is always sent on the backplane */

/* sbtmerrlog */
#define	SBTMEL_CM		0x00000007	/* command */
#define	SBTMEL_CI		0x0000ff00	/* connection id */
#define	SBTMEL_EC		0x0f000000	/* error code */
#define	SBTMEL_ME		0x80000000	/* multiple error */

/* sbimstate */
#define	SBIM_PC			0xf		/* pipecount */
#define	SBIM_AP_MASK		0x30		/* arbitration policy */
#define	SBIM_AP_BOTH		0x00		/* use both timeslaces and token */
#define	SBIM_AP_TS		0x10		/* use timesliaces only */
#define	SBIM_AP_TK		0x20		/* use token only */
#define	SBIM_AP_RSV		0x30		/* reserved */
#define	SBIM_IBE		0x20000		/* inbanderror */
#define	SBIM_TO			0x40000		/* timeout */
#define	SBIM_BY			0x01800000	/* busy (sonics >= 2.3) */
#define	SBIM_RJ			0x02000000	/* reject (sonics >= 2.3) */

/* sbtmstatelow */
#define	SBTML_RESET		0x1		/* reset */
#define	SBTML_REJ_MASK		0x6		/* reject */
#define	SBTML_REJ_SHIFT		1
#define	SBTML_CLK		0x10000		/* clock enable */
#define	SBTML_FGC		0x20000		/* force gated clocks on */
#define	SBTML_FL_MASK		0x3ffc0000	/* core-specific flags */
#define	SBTML_PE		0x40000000	/* pme enable */
#define	SBTML_BE		0x80000000	/* bist enable */

/* sbtmstatehigh */
#define	SBTMH_SERR		0x1		/* serror */
#define	SBTMH_INT		0x2		/* interrupt */
#define	SBTMH_BUSY		0x4		/* busy */
#define	SBTMH_TO		0x00000020	/* timeout (sonics >= 2.3) */
#define	SBTMH_FL_MASK		0x0fff0000	/* core-specific flags */
#define SBTMH_DMA64		0x10000000      /* supports DMA with 64-bit addresses */
#define	SBTMH_GCR		0x20000000	/* gated clock request */
#define	SBTMH_BISTF		0x40000000	/* bist failed */
#define	SBTMH_BISTD		0x80000000	/* bist done */


/* sbbwa0 */
#define	SBBWA_TAB0_MASK		0xffff		/* lookup table 0 */
#define	SBBWA_TAB1_MASK		0xffff		/* lookup table 1 */
#define	SBBWA_TAB1_SHIFT	16

/* sbimconfiglow */
#define	SBIMCL_STO_MASK		0x7		/* service timeout */
#define	SBIMCL_RTO_MASK		0x70		/* request timeout */
#define	SBIMCL_RTO_SHIFT	4
#define	SBIMCL_CID_MASK		0xff0000	/* connection id */
#define	SBIMCL_CID_SHIFT	16

/* sbimconfighigh */
#define	SBIMCH_IEM_MASK		0xc		/* inband error mode */
#define	SBIMCH_TEM_MASK		0x30		/* timeout error mode */
#define	SBIMCH_TEM_SHIFT	4
#define	SBIMCH_BEM_MASK		0xc0		/* bus error mode */
#define	SBIMCH_BEM_SHIFT	6

/* sbadmatch0 */
#define	SBAM_TYPE_MASK		0x3		/* address type */
#define	SBAM_AD64		0x4		/* reserved */
#define	SBAM_ADINT0_MASK	0xf8		/* type0 size */
#define	SBAM_ADINT0_SHIFT	3
#define	SBAM_ADINT1_MASK	0x1f8		/* type1 size */
#define	SBAM_ADINT1_SHIFT	3
#define	SBAM_ADINT2_MASK	0x1f8		/* type2 size */
#define	SBAM_ADINT2_SHIFT	3
#define	SBAM_ADEN		0x400		/* enable */
#define	SBAM_ADNEG		0x800		/* negative decode */
#define	SBAM_BASE0_MASK		0xffffff00	/* type0 base address */
#define	SBAM_BASE0_SHIFT	8
#define	SBAM_BASE1_MASK		0xfffff000	/* type1 base address for the core */
#define	SBAM_BASE1_SHIFT	12
#define	SBAM_BASE2_MASK		0xffff0000	/* type2 base address for the core */
#define	SBAM_BASE2_SHIFT	16

/* sbtmconfiglow */
#define	SBTMCL_CD_MASK		0xff		/* clock divide */
#define	SBTMCL_CO_MASK		0xf800		/* clock offset */
#define	SBTMCL_CO_SHIFT		11
#define	SBTMCL_IF_MASK		0xfc0000	/* interrupt flags */
#define	SBTMCL_IF_SHIFT		18
#define	SBTMCL_IM_MASK		0x3000000	/* interrupt mode */
#define	SBTMCL_IM_SHIFT		24

/* sbtmconfighigh */
#define	SBTMCH_BM_MASK		0x3		/* busy mode */
#define	SBTMCH_RM_MASK		0x3		/* retry mode */
#define	SBTMCH_RM_SHIFT		2
#define	SBTMCH_SM_MASK		0x30		/* stop mode */
#define	SBTMCH_SM_SHIFT		4
#define	SBTMCH_EM_MASK		0x300		/* sb error mode */
#define	SBTMCH_EM_SHIFT		8
#define	SBTMCH_IM_MASK		0xc00		/* int mode */
#define	SBTMCH_IM_SHIFT		10

/* sbbconfig */
#define	SBBC_LAT_MASK		0x3		/* sb latency */
#define	SBBC_MAX0_MASK		0xf0000		/* maxccntr0 */
#define	SBBC_MAX0_SHIFT		16
#define	SBBC_MAX1_MASK		0xf00000	/* maxccntr1 */
#define	SBBC_MAX1_SHIFT		20

/* sbbstate */
#define	SBBS_SRD		0x1		/* st reg disable */
#define	SBBS_HRD		0x2		/* hold reg disable */

/* sbidlow */
#define	SBIDL_CS_MASK		0x3		/* config space */
#define	SBIDL_AR_MASK		0x38		/* # address ranges supported */
#define	SBIDL_AR_SHIFT		3
#define	SBIDL_SYNCH		0x40		/* sync */
#define	SBIDL_INIT		0x80		/* initiator */
#define	SBIDL_MINLAT_MASK	0xf00		/* minimum backplane latency */
#define	SBIDL_MINLAT_SHIFT	8
#define	SBIDL_MAXLAT		0xf000		/* maximum backplane latency */
#define	SBIDL_MAXLAT_SHIFT	12
#define	SBIDL_FIRST		0x10000		/* this initiator is first */
#define	SBIDL_CW_MASK		0xc0000		/* cycle counter width */
#define	SBIDL_CW_SHIFT		18
#define	SBIDL_TP_MASK		0xf00000	/* target ports */
#define	SBIDL_TP_SHIFT		20
#define	SBIDL_IP_MASK		0xf000000	/* initiator ports */
#define	SBIDL_IP_SHIFT		24
#define	SBIDL_RV_MASK		0xf0000000	/* sonics backplane revision code */
#define	SBIDL_RV_SHIFT		28
#define	SBIDL_RV_2_2		0x00000000	/* version 2.2 or earlier */
#define	SBIDL_RV_2_3		0x10000000	/* version 2.3 */

/* sbidhigh */
#define	SBIDH_RC_MASK		0x000f		/* revision code */
#define	SBIDH_RCE_MASK		0x7000		/* revision code extension field */
#define	SBIDH_RCE_SHIFT		8
#define	SBCOREREV(sbidh) \
	((((sbidh) & SBIDH_RCE_MASK) >> SBIDH_RCE_SHIFT) | ((sbidh) & SBIDH_RC_MASK))
#define	SBIDH_CC_MASK		0x8ff0		/* core code */
#define	SBIDH_CC_SHIFT		4
#define	SBIDH_VC_MASK		0xffff0000	/* vendor code */
#define	SBIDH_VC_SHIFT		16

#define	SB_COMMIT		0xfd8		/* update buffered registers value */

/* vendor codes */
#define	SB_VEND_BCM		0x4243		/* Broadcom's SB vendor code */

/* core codes */
#define	SB_NODEV		0x700		/* Invalid coreid */
#define	SB_CC			0x800		/* chipcommon core */
#define	SB_ILINE20		0x801		/* iline20 core */
#define	SB_SDRAM		0x803		/* sdram core */
#define	SB_PCI			0x804		/* pci core */
#define	SB_MIPS			0x805		/* mips core */
#define	SB_ENET			0x806		/* enet mac core */
#define	SB_CODEC		0x807		/* v90 codec core */
#define	SB_USB			0x808		/* usb 1.1 host/device core */
#define	SB_ADSL			0x809		/* ADSL core */
#define	SB_ILINE100		0x80a		/* iline100 core */
#define	SB_IPSEC		0x80b		/* ipsec core */
#define	SB_PCMCIA		0x80d		/* pcmcia core */
#define	SB_SOCRAM		0x80e		/* internal memory core */
#define	SB_MEMC			0x80f		/* memc sdram core */
#define	SB_EXTIF		0x811		/* external interface core */
#define	SB_D11			0x812		/* 802.11 MAC core */
#define	SB_MIPS33		0x816		/* mips3302 core */
#define	SB_USB11H		0x817		/* usb 1.1 host core */
#define	SB_USB11D		0x818		/* usb 1.1 device core */
#define	SB_USB20H		0x819		/* usb 2.0 host core */
#define	SB_USB20D		0x81a		/* usb 2.0 device core */
#define	SB_SDIOH		0x81b		/* sdio host core */
#define	SB_ROBO			0x81c		/* roboswitch core */
#define	SB_ATA100		0x81d		/* parallel ATA core */
#define	SB_SATAXOR		0x81e		/* serial ATA & XOR DMA core */
#define	SB_GIGETH		0x81f		/* gigabit ethernet core */
#define	SB_PCIE			0x820		/* pci express core */
#define	SB_MIMO			0x821		/* MIMO phy core */
#define	SB_SRAMC		0x822		/* SRAM controller core */
#define	SB_MINIMAC		0x823		/* MINI MAC/phy core */
#define	SB_ARM7S		0x825		/* ARM7tdmi-s core */
#define SB_SDIOD		0x829		/* SDIO device core */
#define SB_ARMCM3		0x82a		/* ARM Cortex M3 core */
#define SB_OCP			0x830		/* OCP2OCP bridge core */
#define SB_SC			0x831		/* shared common core */
#define SB_AHB			0x832		/* OCP2AHB bridge core */

#define	SB_CC_IDX		0		/* chipc, when present, is always core 0 */

/* Not an enumeration space register, but common to all cores to
 * communicate w/PMU regarding Silicon Backplane clocking.
 */
#define SB_CLK_CTL_ST		0x1e0		/* clock control and status */

/* clk_ctl_st register */
#define	CCS_FORCEALP		0x00000001	/* force ALP request */
#define	CCS_FORCEHT		0x00000002	/* force HT request */
#define	CCS_FORCEILP		0x00000004	/* force ILP request */
#define	CCS_ALPAREQ		0x00000008	/* ALP Avail Request */
#define	CCS_HTAREQ		0x00000010	/* HT Avail Request */
#define	CCS_FORCEHWREQOFF	0x00000020	/* Force HW Clock Request Off */
#define	CCS_ALPAVAIL		0x00010000	/* ALP is available */
#define	CCS_HTAVAIL		0x00020000	/* HT is available */
#define	CCS0_HTAVAIL		0x00010000	/* HT avail in chipc and pcmcia on 4328a0 */
#define	CCS0_ALPAVAIL		0x00020000	/* ALP avail in chipc and pcmcia on 4328a0 */

/* Not really related to Silicon Backplane, but a couple of software
 * conventions for the use the flash space:
 */

/* Minumum amount of flash we support */
#define FLASH_MIN		0x00020000	/* Minimum flash size */

/* A boot/binary may have an embedded block that describes its size  */
#define	BISZ_OFFSET		0x3e0		/* At this offset into the binary */
#define	BISZ_MAGIC		0x4249535a	/* Marked with this value: 'BISZ' */
#define	BISZ_MAGIC_IDX		0		/* Word 0: magic */
#define	BISZ_TXTST_IDX		1		/*	1: text start */
#define	BISZ_TXTEND_IDX		2		/*	2: text end */
#define	BISZ_DATAST_IDX		3		/*	3: data start */
#define	BISZ_DATAEND_IDX	4		/*	4: data end */
#define	BISZ_BSSST_IDX		5		/*	5: bss start */
#define	BISZ_BSSEND_IDX		6		/*	6: bss end */
#define BISZ_SIZE		7		/* descriptor size in 32-bit intergers */

#endif	/* _SBCONFIG_H */
