/*
 * Misc utility routines for accessing chip-specific features
 * of the SiliconBackplane-based Broadcom chips.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: sbutils.c,v 1.10 2006/04/08 07:12:42 honor Exp $
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <sbutils.h>
#include <bcmdevs.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <sbpci.h>
#include <sbpcie.h>
#include <pcicfg.h>
#include <sbpcmcia.h>
#include <sbextif.h>
#include <sbsocram.h>
#include <bcmsrom.h>
#ifdef __mips__
#include <mipsinc.h>
#endif	/* __mips__ */

/* debug/trace */
#define	SB_ERROR(args)

typedef uint32 (*sb_intrsoff_t)(void *intr_arg);
typedef void (*sb_intrsrestore_t)(void *intr_arg, uint32 arg);
typedef bool (*sb_intrsenabled_t)(void *intr_arg);

/* misc sb info needed by some of the routines */
typedef struct sb_info {

	struct sb_pub  	sb;		/* back plane public state (must be first field) */

	void	*osh;			/* osl os handle */
	void	*sdh;			/* bcmsdh handle */

	void	*curmap;		/* current regs va */
	void	*regs[SB_MAXCORES];	/* other regs va */

	uint	curidx;			/* current core index */
	uint	dev_coreid;		/* the core provides driver functions */

	bool	memseg;			/* flag to toggle MEM_SEG register */

	uint	gpioidx;		/* gpio control core index */
	uint	gpioid;			/* gpio control coretype */

	uint	numcores;		/* # discovered cores */
	uint	coreid[SB_MAXCORES];	/* id of each core */

	void	*intr_arg;		/* interrupt callback function arg */
	sb_intrsoff_t		intrsoff_fn;		/* turns chip interrupts off */
	sb_intrsrestore_t	intrsrestore_fn;	/* restore chip interrupts */
	sb_intrsenabled_t	intrsenabled_fn;	/* check if interrupts are enabled */

} sb_info_t;

/* local prototypes */
static sb_info_t * sb_doattach(sb_info_t *si, uint devid, osl_t *osh, void *regs,
	uint bustype, void *sdh, char **vars, uint *varsz);
static void sb_scan(sb_info_t *si);
static uint sb_corereg(sb_info_t *si, uint coreidx, uint regoff, uint mask, uint val);
static uint _sb_coreidx(sb_info_t *si);
static uint sb_findcoreidx(sb_info_t *si, uint coreid, uint coreunit);
static uint sb_pcidev2chip(uint pcidev);
static uint sb_chip2numcores(uint chip);
static bool sb_ispcie(sb_info_t *si);
static bool sb_find_pci_capability(sb_info_t *si, uint8 req_cap_id, uchar *buf, uint32 *buflen);
static int sb_pci_fixcfg(sb_info_t *si);

/* routines to access mdio slave device registers */
static int sb_pcie_mdiowrite(sb_info_t *si,  uint physmedia, uint readdr, uint val);
static void sb_war30841(sb_info_t *si);

/* delay needed between the mdio control/ mdiodata register data access */
#define PR28829_DELAY() OSL_DELAY(10)

/* size that can take bitfielddump */
#define BITFIELD_DUMP_SIZE  32

/* global variable to indicate reservation/release of gpio's */
static uint32 sb_gpioreservation = 0;

#define	SB_INFO(sbh)	(sb_info_t*)sbh
#define	SET_SBREG(si, r, mask, val)	\
		W_SBREG((si), (r), ((R_SBREG((si), (r)) & ~(mask)) | (val)))
#define	GOODCOREADDR(x)	(((x) >= SB_ENUM_BASE) && ((x) <= SB_ENUM_LIM) && \
		ISALIGNED((x), SB_CORE_SIZE))
#define	GOODREGS(regs)	((regs) && ISALIGNED((uintptr)(regs), SB_CORE_SIZE))
#define	REGS2SB(va)	(sbconfig_t*) ((int8*)(va) + SBCONFIGOFF)
#define	GOODIDX(idx)	(((uint)idx) < SB_MAXCORES)
#define	BADIDX		(SB_MAXCORES+1)
#define	NOREV		-1		/* Invalid rev */

#define PCI(si)		((BUSTYPE(si->sb.bustype) == PCI_BUS) && (si->sb.buscoretype == SB_PCI))
#define PCIE(si)	((BUSTYPE(si->sb.bustype) == PCI_BUS) && (si->sb.buscoretype == SB_PCIE))

/* sonicsrev */
#define	SONICS_2_2	(SBIDL_RV_2_2 >> SBIDL_RV_SHIFT)
#define	SONICS_2_3	(SBIDL_RV_2_3 >> SBIDL_RV_SHIFT)

#define	R_SBREG(si, sbr)	sb_read_sbreg((si), (sbr))
#define	W_SBREG(si, sbr, v)	sb_write_sbreg((si), (sbr), (v))
#define	AND_SBREG(si, sbr, v)	W_SBREG((si), (sbr), (R_SBREG((si), (sbr)) & (v)))
#define	OR_SBREG(si, sbr, v)	W_SBREG((si), (sbr), (R_SBREG((si), (sbr)) | (v)))

/*
 * Macros to disable/restore function core(D11, ENET, ILINE20, etc) interrupts before/
 * after core switching to avoid invalid register accesss inside ISR.
 */
#define INTR_OFF(si, intr_val) \
	if ((si)->intrsoff_fn && (si)->coreid[(si)->curidx] == (si)->dev_coreid) {	\
		intr_val = (*(si)->intrsoff_fn)((si)->intr_arg); }
#define INTR_RESTORE(si, intr_val) \
	if ((si)->intrsrestore_fn && (si)->coreid[(si)->curidx] == (si)->dev_coreid) {	\
		(*(si)->intrsrestore_fn)((si)->intr_arg, intr_val); }

/* dynamic clock control defines */
#define	LPOMINFREQ		25000		/* low power oscillator min */
#define	LPOMAXFREQ		43000		/* low power oscillator max */
#define	XTALMINFREQ		19800000	/* 20 MHz - 1% */
#define	XTALMAXFREQ		20200000	/* 20 MHz + 1% */
#define	PCIMINFREQ		25000000	/* 25 MHz */
#define	PCIMAXFREQ		34000000	/* 33 MHz + fudge */

#define	ILP_DIV_5MHZ		0		/* ILP = 5 MHz */
#define	ILP_DIV_1MHZ		4		/* ILP = 1 MHz */

/* different register spaces to access thr'u pcie indirect access */
#define PCIE_CONFIGREGS 	1		/* Access to config space */
#define PCIE_PCIEREGS 		2		/* Access to pcie registers */

/* force HT war check */
#define FORCEHT_WAR32414(si)   \
   ((PCIE(si)) && (((si->sb.chip == BCM4311_CHIP_ID) && (si->sb.chiprev == 1)) ||  \
   ((si->sb.chip == BCM4321_CHIP_ID) && (si->sb.chiprev <= 3))))

/* GPIO Based LED powersave defines */
#define DEFAULT_GPIO_ONTIME	10		/* Default: 10% on */
#define DEFAULT_GPIO_OFFTIME	90		/* Default: 10% on */

#define DEFAULT_GPIOTIMERVAL  ((DEFAULT_GPIO_ONTIME << GPIO_ONTIME_SHIFT) | DEFAULT_GPIO_OFFTIME)

static uint32
sb_read_sbreg(sb_info_t *si, volatile uint32 *sbr)
{
	uint8 tmp;
	uint32 val, intr_val = 0;


	/*
	 * compact flash only has 11 bits address, while we needs 12 bits address.
	 * MEM_SEG will be OR'd with other 11 bits address in hardware,
	 * so we program MEM_SEG with 12th bit when necessary(access sb regsiters).
	 * For normal PCMCIA bus(CFTable_regwinsz > 2k), do nothing special
	 */
	if (si->memseg) {
		INTR_OFF(si, intr_val);
		tmp = 1;
		OSL_PCMCIA_WRITE_ATTR(si->osh, MEM_SEG, &tmp, 1);
		sbr = (volatile uint32 *)((uintptr)sbr & ~(1 << 11)); /* mask out bit 11 */
	}

	val = R_REG(si->osh, sbr);

	if (si->memseg) {
		tmp = 0;
		OSL_PCMCIA_WRITE_ATTR(si->osh, MEM_SEG, &tmp, 1);
		INTR_RESTORE(si, intr_val);
	}

	return (val);
}

static void
sb_write_sbreg(sb_info_t *si, volatile uint32 *sbr, uint32 v)
{
	uint8 tmp;
	volatile uint32 dummy;
	uint32 intr_val = 0;


	/*
	 * compact flash only has 11 bits address, while we needs 12 bits address.
	 * MEM_SEG will be OR'd with other 11 bits address in hardware,
	 * so we program MEM_SEG with 12th bit when necessary(access sb regsiters).
	 * For normal PCMCIA bus(CFTable_regwinsz > 2k), do nothing special
	 */
	if (si->memseg) {
		INTR_OFF(si, intr_val);
		tmp = 1;
		OSL_PCMCIA_WRITE_ATTR(si->osh, MEM_SEG, &tmp, 1);
		sbr = (volatile uint32 *)((uintptr)sbr & ~(1 << 11)); /* mask out bit 11 */
	}

	if (BUSTYPE(si->sb.bustype) == PCMCIA_BUS) {
#ifdef IL_BIGENDIAN
		dummy = R_REG(si->osh, sbr);
		W_REG(si->osh, ((volatile uint16 *)sbr + 1), (uint16)((v >> 16) & 0xffff));
		dummy = R_REG(si->osh, sbr);
		W_REG(si->osh, (volatile uint16 *)sbr, (uint16)(v & 0xffff));
#else
		dummy = R_REG(si->osh, sbr);
		W_REG(si->osh, (volatile uint16 *)sbr, (uint16)(v & 0xffff));
		dummy = R_REG(si->osh, sbr);
		W_REG(si->osh, ((volatile uint16 *)sbr + 1), (uint16)((v >> 16) & 0xffff));
#endif	/* IL_BIGENDIAN */
	} else
		W_REG(si->osh, sbr, v);

	if (si->memseg) {
		tmp = 0;
		OSL_PCMCIA_WRITE_ATTR(si->osh, MEM_SEG, &tmp, 1);
		INTR_RESTORE(si, intr_val);
	}
}

/*
 * Allocate a sb handle.
 * devid - pci device id (used to determine chip#)
 * osh - opaque OS handle
 * regs - virtual address of initial core registers
 * bustype - pci/pcmcia/sb/sdio/etc
 * vars - pointer to a pointer area for "environment" variables
 * varsz - pointer to int to return the size of the vars
 */
sb_t *
BCMINITFN(sb_attach)(uint devid, osl_t *osh, void *regs,
                     uint bustype, void *sdh, char **vars, uint *varsz)
{
	sb_info_t *si;

	/* alloc sb_info_t */
	if ((si = MALLOC(osh, sizeof (sb_info_t))) == NULL) {
		SB_ERROR(("sb_attach: malloc failed! malloced %d bytes\n", MALLOCED(osh)));
		return (NULL);
	}

	if (sb_doattach(si, devid, osh, regs, bustype, sdh, vars, (uint*)varsz) == NULL) {
		MFREE(osh, si, sizeof(sb_info_t));
		return (NULL);
	}

	return (sb_t *)si;
}

/* Using sb_kattach depends on SB_BUS support, either implicit  */
/* no limiting BCMBUSTYPE value) or explicit (value is SB_BUS). */
#if !defined(BCMBUSTYPE) || (BCMBUSTYPE == SB_BUS)

/* global kernel resource */
static sb_info_t ksi;
static bool ksi_attached = FALSE;

/* generic kernel variant of sb_attach() */
sb_t *
BCMINITFN(sb_kattach)(void)
{
	osl_t *osh = NULL;
	uint32 *regs;

	if (!ksi_attached) {
		uint32 cid;

		regs = (uint32 *)REG_MAP(SB_ENUM_BASE, SB_CORE_SIZE);
		cid = R_REG(osh, (uint32 *)regs);
		if (((cid & CID_ID_MASK) == BCM4712_CHIP_ID) &&
		    ((cid & CID_PKG_MASK) != BCM4712LARGE_PKG_ID) &&
		    ((cid & CID_REV_MASK) <= (3 << CID_REV_SHIFT))) {
			uint32 *scc, val;

			scc = (uint32 *)((uchar*)regs + OFFSETOF(chipcregs_t, slow_clk_ctl));
			val = R_REG(osh, scc);
			SB_ERROR(("    initial scc = 0x%x\n", val));
			val |= SCC_SS_XTAL;
			W_REG(osh, scc, val);
		}

		if (sb_doattach(&ksi, BCM4710_DEVICE_ID, osh, (void*)regs,
			SB_BUS, NULL, NULL, NULL) == NULL) {
			return NULL;
		}
		else
			ksi_attached = TRUE;
	}

	return (sb_t *)&ksi;
}
#endif	/* !BCMBUSTYPE || (BCMBUSTYPE == SB_BUS) */

void
BCMINITFN(sb_war32414_forceHT)(sb_t *sbh, bool forceHT)
{
   sb_info_t *si;

   si = SB_INFO(sbh);

   
   if (FORCEHT_WAR32414(si)) {
       uint32 val = 0;
       if (forceHT)
           val = SYCC_HR;
       sb_corereg((void*)si, SB_CC_IDX, OFFSETOF(chipcregs_t, system_clk_ctl),
           SYCC_HR, val);
   }
}

static sb_info_t  *
BCMINITFN(sb_doattach)(sb_info_t *si, uint devid, osl_t *osh, void *regs,
                       uint bustype, void *sdh, char **vars, uint *varsz)
{
	uint origidx;
	chipcregs_t *cc;
	sbconfig_t *sb;
	uint32 w;

	ASSERT(GOODREGS(regs));

	bzero((uchar*)si, sizeof(sb_info_t));

	si->sb.buscoreidx = si->gpioidx = BADIDX;

	si->curmap = regs;
	si->sdh = sdh;
	si->osh = osh;

	/* check to see if we are a sb core mimic'ing a pci core */
	if (bustype == PCI_BUS) {
		if (OSL_PCI_READ_CONFIG(si->osh, PCI_SPROM_CONTROL, sizeof(uint32)) == 0xffffffff) {
			SB_ERROR(("%s: incoming bus is PCI but it's a lie, switching to SB "
			          "devid:0x%x\n", __FUNCTION__, devid));
			bustype = SB_BUS;
		}
	}

	si->sb.bustype = bustype;
	if (si->sb.bustype != BUSTYPE(si->sb.bustype)) {
		SB_ERROR(("sb_doattach: bus type %d does not match configured bus type %d\n",
		          si->sb.bustype, BUSTYPE(si->sb.bustype)));
		return NULL;
	}

	/* need to set memseg flag for CF card first before any sb registers access */
	if (BUSTYPE(si->sb.bustype) == PCMCIA_BUS)
		si->memseg = TRUE;

	/* kludge to enable the clock on the 4306 which lacks a slowclock */
	if (BUSTYPE(si->sb.bustype) == PCI_BUS)
		sb_clkctl_xtal(&si->sb, XTAL|PLL, ON);

	if (BUSTYPE(si->sb.bustype) == PCI_BUS) {
		w = OSL_PCI_READ_CONFIG(si->osh, PCI_BAR0_WIN, sizeof(uint32));
		if (!GOODCOREADDR(w))
			OSL_PCI_WRITE_CONFIG(si->osh, PCI_BAR0_WIN, sizeof(uint32), SB_ENUM_BASE);
	}

	/* initialize current core index value */
	si->curidx = _sb_coreidx(si);

	if (si->curidx == BADIDX) {
		SB_ERROR(("sb_doattach: bad core index\n"));
		return NULL;
	}

	/* get sonics backplane revision */
	sb = REGS2SB(si->curmap);
	si->sb.sonicsrev = (R_SBREG(si, &sb->sbidlow) & SBIDL_RV_MASK) >> SBIDL_RV_SHIFT;

	/* keep and reuse the initial register mapping */
	origidx = si->curidx;
	if (BUSTYPE(si->sb.bustype) == SB_BUS)
		si->regs[origidx] = regs;

	/* is core-0 a chipcommon core? */
	si->numcores = 1;
	cc = (chipcregs_t*) sb_setcoreidx(&si->sb, 0);
	if (sb_coreid(&si->sb) != SB_CC)
		cc = NULL;

	/* determine chip id and rev */
	if (cc) {
		/* chip common core found! */
		si->sb.chip = R_REG(si->osh, &cc->chipid) & CID_ID_MASK;
		si->sb.chiprev = (R_REG(si->osh, &cc->chipid) & CID_REV_MASK) >> CID_REV_SHIFT;
		si->sb.chippkg = (R_REG(si->osh, &cc->chipid) & CID_PKG_MASK) >> CID_PKG_SHIFT;
	} else {
		/* no chip common core -- must convert device id to chip id */
		if ((si->sb.chip = sb_pcidev2chip(devid)) == 0) {
			SB_ERROR(("sb_doattach: unrecognized device id 0x%04x\n", devid));
			sb_setcoreidx(&si->sb, origidx);
			return NULL;
		}
	}

	/* get chipcommon rev */
	si->sb.ccrev = cc ? (int)sb_corerev(&si->sb) : NOREV;

	/* determine numcores */
	if (cc && ((si->sb.ccrev == 4) || (si->sb.ccrev >= 6)))
		si->numcores = (R_REG(si->osh, &cc->chipid) & CID_CC_MASK) >> CID_CC_SHIFT;
	else
		si->numcores = sb_chip2numcores(si->sb.chip);

	/* return to original core */
	sb_setcoreidx(&si->sb, origidx);

	/* sanity checks */
	ASSERT(si->sb.chip);

	/* scan for cores */
	sb_scan(si);

	/* fixup necessary chip/core configurations */
	if (BUSTYPE(si->sb.bustype) == PCI_BUS) {
		if (sb_pci_fixcfg(si)) {
			SB_ERROR(("sb_doattach: sb_pci_fixcfg failed\n"));
			return NULL;
		}
	}

	/* srom_var_init() depends on sb_scan() info */
	if (srom_var_init(si, si->sb.bustype, si->curmap, si->osh, vars, varsz)) {
		SB_ERROR(("sb_doattach: srom_var_init failed: bad srom\n"));
		return (NULL);
	}

	if (cc == NULL) {
		/*
		 * The chip revision number is hardwired into all
		 * of the pci function config rev fields and is
		 * independent from the individual core revision numbers.
		 * For example, the "A0" silicon of each chip is chip rev 0.
		 * For PCMCIA we get it from the CIS instead.
		 */
		if (BUSTYPE(si->sb.bustype) == PCMCIA_BUS) {
			ASSERT(vars);
			si->sb.chiprev = getintvar(*vars, "chiprev");
		} else if (BUSTYPE(si->sb.bustype) == PCI_BUS) {
			w = OSL_PCI_READ_CONFIG(si->osh, PCI_CFG_REV, sizeof(uint32));
			si->sb.chiprev = w & 0xff;
		} else
			si->sb.chiprev = 0;
	}

	if (BUSTYPE(si->sb.bustype) == PCMCIA_BUS) {
		w = getintvar(*vars, "regwindowsz");
		si->memseg = (w <= CFTABLE_REGWIN_2K) ? TRUE : FALSE;
	}

	/* gpio control core is required */
	if (!GOODIDX(si->gpioidx)) {
		SB_ERROR(("sb_doattach: gpio control core not found\n"));
		return NULL;
	}

	/* get boardtype and boardrev */
	switch (BUSTYPE(si->sb.bustype)) {
	case PCI_BUS:
		/* do a pci config read to get subsystem id and subvendor id */
		w = OSL_PCI_READ_CONFIG(si->osh, PCI_CFG_SVID, sizeof(uint32));
		si->sb.boardvendor = w & 0xffff;
		si->sb.boardtype = (w >> 16) & 0xffff;
		break;

	case PCMCIA_BUS:
	case SDIO_BUS:
		si->sb.boardvendor = getintvar(*vars, "manfid");
		si->sb.boardtype = getintvar(*vars, "prodid");
		break;

	case SB_BUS:
	case JTAG_BUS:
		si->sb.boardvendor = VENDOR_BROADCOM;
		if ((si->sb.boardtype = getintvar(NULL, "boardtype")) == 0)
			si->sb.boardtype = 0xffff;
		break;
	}

	if (si->sb.boardtype == 0) {
		SB_ERROR(("sb_doattach: unknown board type\n"));
		ASSERT(si->sb.boardtype);
	}

	/* setup the GPIO based LED powersave register */
	if (si->sb.ccrev >= 16) {
		if ((vars == NULL) || ((w = getintvar(*vars, "leddc")) == 0))
			w = DEFAULT_GPIOTIMERVAL;
		sb_corereg(si, 0, OFFSETOF(chipcregs_t, gpiotimerval), ~0, w);
	}
	if (FORCEHT_WAR32414(si)) {
		/* set proper clk setup delays before forcing HT */
		sb_clkctl_init((void *)si);
		sb_war32414_forceHT((void *)si, 1);
	}


	return (si);
}


uint
sb_coreid(sb_t *sbh)
{
	sb_info_t *si;
	sbconfig_t *sb;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);

	return ((R_SBREG(si, &sb->sbidhigh) & SBIDH_CC_MASK) >> SBIDH_CC_SHIFT);
}

uint
sb_coreidx(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->curidx);
}

/* return current index of core */
static uint
_sb_coreidx(sb_info_t *si)
{
	sbconfig_t *sb;
	uint32 sbaddr = 0;

	ASSERT(si);

	switch (BUSTYPE(si->sb.bustype)) {
	case SB_BUS:
		sb = REGS2SB(si->curmap);
		sbaddr = sb_base(R_SBREG(si, &sb->sbadmatch0));
		break;

	case PCI_BUS:
		sbaddr = OSL_PCI_READ_CONFIG(si->osh, PCI_BAR0_WIN, sizeof(uint32));
		break;

	case PCMCIA_BUS: {
		uint8 tmp = 0;

		OSL_PCMCIA_READ_ATTR(si->osh, PCMCIA_ADDR0, &tmp, 1);
		sbaddr  = (uint)tmp << 12;
		OSL_PCMCIA_READ_ATTR(si->osh, PCMCIA_ADDR1, &tmp, 1);
		sbaddr |= (uint)tmp << 16;
		OSL_PCMCIA_READ_ATTR(si->osh, PCMCIA_ADDR2, &tmp, 1);
		sbaddr |= (uint)tmp << 24;
		break;
	}

#ifdef BCMJTAG
	case JTAG_BUS:
		sbaddr = (uint32)si->curmap;
		break;
#endif	/* BCMJTAG */

	default:
		ASSERT(0);
	}

	if (!GOODCOREADDR(sbaddr))
		return BADIDX;

	return ((sbaddr - SB_ENUM_BASE) / SB_CORE_SIZE);
}

uint
sb_corevendor(sb_t *sbh)
{
	sb_info_t *si;
	sbconfig_t *sb;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);

	return ((R_SBREG(si, &sb->sbidhigh) & SBIDH_VC_MASK) >> SBIDH_VC_SHIFT);
}

uint
sb_corerev(sb_t *sbh)
{
	sb_info_t *si;
	sbconfig_t *sb;
	uint sbidh;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);
	sbidh = R_SBREG(si, &sb->sbidhigh);

	return (SBCOREREV(sbidh));
}

void *
sb_osh(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return si->osh;
}

void
sb_setosh(sb_t *sbh, osl_t *osh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	if (si->osh != NULL) {
		SB_ERROR(("osh is already set....\n"));
		ASSERT(!si->osh);
	}
	si->osh = osh;
}

/* set/clear sbtmstatelow core-specific flags */
uint32
sb_coreflags(sb_t *sbh, uint32 mask, uint32 val)
{
	sb_info_t *si;
	sbconfig_t *sb;
	uint32 w;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);

	ASSERT((val & ~mask) == 0);

	/* mask and set */
	if (mask || val) {
		w = (R_SBREG(si, &sb->sbtmstatelow) & ~mask) | val;
		W_SBREG(si, &sb->sbtmstatelow, w);
	}

	/* return the new value */
	return (R_SBREG(si, &sb->sbtmstatelow));
}

/* set/clear sbtmstatehigh core-specific flags */
uint32
sb_coreflagshi(sb_t *sbh, uint32 mask, uint32 val)
{
	sb_info_t *si;
	sbconfig_t *sb;
	uint32 w;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);

	ASSERT((val & ~mask) == 0);
	ASSERT((mask & ~SBTMH_FL_MASK) == 0);

	/* mask and set */
	if (mask || val) {
		w = (R_SBREG(si, &sb->sbtmstatehigh) & ~mask) | val;
		W_SBREG(si, &sb->sbtmstatehigh, w);
	}

	/* return the new value */
	return (R_SBREG(si, &sb->sbtmstatehigh) & SBTMH_FL_MASK);
}

/* Run bist on current core. Caller needs to take care of core-specific bist hazards */
int
sb_corebist(sb_t *sbh)
{
	uint32 sblo;
	sb_info_t *si;
	sbconfig_t *sb;
	int result = 0;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);

	sblo = R_SBREG(si, &sb->sbtmstatelow);
	W_SBREG(si, &sb->sbtmstatelow, (sblo | SBTML_FGC | SBTML_BE));

	SPINWAIT(((R_SBREG(si, &sb->sbtmstatehigh) & SBTMH_BISTD) == 0), 100000);

	if (R_SBREG(si, &sb->sbtmstatehigh) & SBTMH_BISTF)
		result = BCME_ERROR;

	W_SBREG(si, &sb->sbtmstatelow, sblo);

	return result;
}

bool
sb_iscoreup(sb_t *sbh)
{
	sb_info_t *si;
	sbconfig_t *sb;

	si = SB_INFO(sbh);
	sb = REGS2SB(si->curmap);

	return ((R_SBREG(si, &sb->sbtmstatelow) &
	         (SBTML_RESET | SBTML_REJ_MASK | SBTML_CLK)) == SBTML_CLK);
}

/*
 * Switch to 'coreidx', issue a single arbitrary 32bit register mask&set operation,
 * switch back to the original core, and return the new value.
 *
 * When using the silicon backplane, no fidleing with interrupts or core switches are needed.
 *
 * Also, when using pci/pcie, we can optimize away the core switching for pci registers
 * and (on newer pci cores) chipcommon registers.
 */
static uint
sb_corereg(sb_info_t *si, uint coreidx, uint regoff, uint mask, uint val)
{
	uint origidx = 0;
	uint32 *r = NULL;
	uint w;
	uint intr_val = 0;
	bool fast = FALSE;

	ASSERT(GOODIDX(coreidx));
	ASSERT(regoff < SB_CORE_SIZE);
	ASSERT((val & ~mask) == 0);

#ifdef	notyet
	if (si->sb.bustype == SB_BUS) {
		/* If internal bus, we can always get at everything */
		fast = TRUE;
		r = (uint32 *)((uchar *)si->regs[coreidx] + regoff);
	} else if (si->sb.bustype == PCI_BUS) {
		/* If pci/pcie, we can get at pci/pcie regs and on newer cores to chipc */

		if ((si->coreid[coreidx] == SB_CC) &&
		    ((si->sb.buscoretype == SB_PCIE) ||
		     (si->sb.buscorerev >= 13))) {
			/* Chipc registers are mapped at 12KB */

			fast = TRUE;
			r = (uint32 *)((char *)si->curmap + PCI_16KB0_CCREGS_OFFSET + regoff);
		} else if (si->sb.buscoreidx == coreidx) {
			/* pci registers are at either in the last 2KB of an 8KB window
			 * or, in pcie and pci rev 13 at 8KB
			 */
			fast = TRUE;
			if ((si->sb.buscoretype == SB_PCIE) ||
			    (si->sb.buscorerev >= 13))
				r = (uint32 *)((char *)si->curmap +
				               PCI_16KB0_PCIREGS_OFFSET + regoff);
			else
				r = (uint32 *)((char *)si->curmap +
				               ((regoff >= SBCONFIGOFF) ?
				                PCI_BAR0_PCISBR_OFFSET : PCI_BAR0_PCIREGS_OFFSET) +
				               regoff);
		}
	}
#endif	/* notyet */

	if (!fast) {
		INTR_OFF(si, intr_val);

		/* save current core index */
		origidx = sb_coreidx(&si->sb);

		/* switch core */
		r = (uint32*) ((uchar*) sb_setcoreidx(&si->sb, coreidx) + regoff);
	}
	ASSERT(r);

	/* mask and set */
	if (mask || val) {
		if (regoff >= SBCONFIGOFF) {
			w = (R_SBREG(si, r) & ~mask) | val;
			W_SBREG(si, r, w);
		} else {
			w = (R_REG(si->osh, r) & ~mask) | val;
			W_REG(si->osh, r, w);
		}
	}

	/* readback */
	if (regoff >= SBCONFIGOFF)
		w = R_SBREG(si, r);
	else
		w = R_REG(si->osh, r);

	if (!fast) {
		/* restore core index */
		if (origidx != coreidx)
			sb_setcoreidx(&si->sb, origidx);

		INTR_RESTORE(si, intr_val);
	}

	return (w);
}

#define DWORD_ALIGN(x)  (x & ~(0x03))
#define BYTE_POS(x) (x & 0x3)
#define WORD_POS(x) (x & 0x1)

#define BYTE_SHIFT(x)  (8 * BYTE_POS(x))
#define WORD_SHIFT(x)  (16 * WORD_POS(x))

#define BYTE_VAL(a, x) ((a >> BYTE_SHIFT(x)) & 0xFF)
#define WORD_VAL(a, x) ((a >> WORD_SHIFT(x)) & 0xFFFF)

#define read_pci_cfg_byte(a) \
	(BYTE_VAL(OSL_PCI_READ_CONFIG(si->osh, DWORD_ALIGN(a), 4), a) & 0xff)

#define read_pci_cfg_word(a) \
	(WORD_VAL(OSL_PCI_READ_CONFIG(si->osh, DWORD_ALIGN(a), 4), a) & 0xffff)


/* return TRUE if requested capability exists in the PCI config space */
static bool
sb_find_pci_capability(sb_info_t *si, uint8 req_cap_id, uchar *buf, uint32 *buflen)
{
	uint8 cap_id;
	uint8 cap_ptr;
	uint32 	bufsize;
	uint8 byte_val;

	if (BUSTYPE(si->sb.bustype) != PCI_BUS)
		return FALSE;

	/* check for Header type 0 */
	byte_val = read_pci_cfg_byte(PCI_CFG_HDR);
	if ((byte_val & 0x7f) != PCI_HEADER_NORMAL)
		return FALSE;

	/* check if the capability pointer field exists */
	byte_val = read_pci_cfg_byte(PCI_CFG_STAT);
	if (!(byte_val & PCI_CAPPTR_PRESENT))
		return FALSE;

	cap_ptr = read_pci_cfg_byte(PCI_CFG_CAPPTR);
	/* check if the capability pointer is 0x00 */
	if (cap_ptr == 0x00)
		return FALSE;


	/* loop thr'u the capability list and see if the pcie capabilty exists */

	cap_id = read_pci_cfg_byte(cap_ptr);

	while (cap_id != req_cap_id) {
		cap_ptr = read_pci_cfg_byte((cap_ptr+1));
		if (cap_ptr == 0x00) break;
		cap_id = read_pci_cfg_byte(cap_ptr);
	}
	if (cap_id != req_cap_id) {
		return FALSE;
	}
	/* found the caller requested capability */
	if ((buf != NULL) && (buflen != NULL)) {
		bufsize = *buflen;
		if (!bufsize) goto end;
		*buflen = 0;
		/* copy the cpability data excluding cap ID and next ptr */
		cap_ptr += 2;
		if ((bufsize + cap_ptr)  > SZPCR)
			bufsize = SZPCR - cap_ptr;
		*buflen = bufsize;
		while (bufsize--) {
			*buf = read_pci_cfg_byte(cap_ptr);
			cap_ptr++;
			buf++;
		}
	}
end:
	return TRUE;
}

/* return TRUE if PCIE capability exists the pci config space */
static inline bool
sb_ispcie(sb_info_t *si)
{
	return (sb_find_pci_capability(si, PCI_CAP_PCIECAP_ID, NULL, NULL));
}

/* scan the sb enumerated space to identify all cores */
static void
BCMINITFN(sb_scan)(sb_info_t *si)
{
	uint origidx;
	uint i;
	bool pci;
	bool pcie;
	uint pciidx;
	uint pcieidx;
	uint pcirev;
	uint pcierev;


	/* numcores should already be set */
	ASSERT((si->numcores > 0) && (si->numcores <= SB_MAXCORES));

	/* save current core index */
	origidx = sb_coreidx(&si->sb);

	si->sb.buscorerev = NOREV;
	si->sb.buscoreidx = BADIDX;

	si->gpioidx = BADIDX;

	pci = pcie = FALSE;
	pcirev = pcierev = NOREV;
	pciidx = pcieidx = BADIDX;

	for (i = 0; i < si->numcores; i++) {
		sb_setcoreidx(&si->sb, i);
		si->coreid[i] = sb_coreid(&si->sb);

		if (si->coreid[i] == SB_PCI) {
			pciidx = i;
			pcirev = sb_corerev(&si->sb);
			pci = TRUE;
		} else if (si->coreid[i] == SB_PCIE) {
			pcieidx = i;
			pcierev = sb_corerev(&si->sb);
			pcie = TRUE;
		} else if (si->coreid[i] == SB_PCMCIA) {
			si->sb.buscorerev = sb_corerev(&si->sb);
			si->sb.buscoretype = si->coreid[i];
			si->sb.buscoreidx = i;
		}
	}
	if (pci && pcie) {
		if (sb_ispcie(si))
			pci = FALSE;
		else
			pcie = FALSE;
	}
	if (pci) {
		si->sb.buscoretype = SB_PCI;
		si->sb.buscorerev = pcirev;
		si->sb.buscoreidx = pciidx;
	} else if (pcie) {
		si->sb.buscoretype = SB_PCIE;
		si->sb.buscorerev = pcierev;
		si->sb.buscoreidx = pcieidx;
	}

	/*
	 * Find the gpio "controlling core" type and index.
	 * Precedence:
	 * - if there's a chip common core - use that
	 * - else if there's a pci core (rev >= 2) - use that
	 * - else there had better be an extif core (4710 only)
	 */
	if (GOODIDX(sb_findcoreidx(si, SB_CC, 0))) {
		si->gpioidx = sb_findcoreidx(si, SB_CC, 0);
		si->gpioid = SB_CC;
	} else if (PCI(si) && (si->sb.buscorerev >= 2)) {
		si->gpioidx = si->sb.buscoreidx;
		si->gpioid = SB_PCI;
	} else if (sb_findcoreidx(si, SB_EXTIF, 0)) {
		si->gpioidx = sb_findcoreidx(si, SB_EXTIF, 0);
		si->gpioid = SB_EXTIF;
	} else
		ASSERT(si->gpioidx != BADIDX);

	/* return to original core index */
	sb_setcoreidx(&si->sb, origidx);
}

/* may be called with core in reset */
void
sb_detach(sb_t *sbh)
{
	sb_info_t *si;
	uint idx;

	si = SB_INFO(sbh);

	if (si == NULL)
		return;

	if (BUSTYPE(si->sb.bustype) == SB_BUS)
		for (idx = 0; idx < SB_MAXCORES; idx++)
			if (si->regs[idx]) {
				REG_UNMAP(si->regs[idx]);
				si->regs[idx] = NULL;
			}
#if !defined(BCMBUSTYPE) || (BCMBUSTYPE == SB_BUS)
	if (si != &ksi)
#endif	/* !BCMBUSTYPE || (BCMBUSTYPE == SB_BUS) */
		MFREE(si->osh, si, sizeof(sb_info_t));

}

/* use pci dev id to determine chip id for chips not having a chipcommon core */
static uint
BCMINITFN(sb_pcidev2chip)(uint pcidev)
{
	if ((pcidev >= BCM4710_DEVICE_ID) && (pcidev <= BCM47XX_USB_ID))
		return (BCM4710_CHIP_ID);
	if ((pcidev >= BCM4402_ENET_ID) && (pcidev <= BCM4402_V90_ID))
		return (BCM4402_CHIP_ID);
	if (pcidev == BCM4401_ENET_ID)
		return (BCM4402_CHIP_ID);

	return (0);
}

/* convert chip number to number of i/o cores */
static uint
BCMINITFN(sb_chip2numcores)(uint chip)
{
	if (chip == BCM4710_CHIP_ID)
		return (9);
	if (chip == BCM4402_CHIP_ID)
		return (3);
	if (chip == BCM4306_CHIP_ID)	/* < 4306c0 */
		return (6);
	if (chip == BCM4704_CHIP_ID)
		return (9);
	if (chip == BCM5365_CHIP_ID)
		return (7);

	SB_ERROR(("sb_chip2numcores: unsupported chip 0x%x\n", chip));
	ASSERT(0);
	return (1);
}

/* return index of coreid or BADIDX if not found */
static uint
sb_findcoreidx(sb_info_t *si, uint coreid, uint coreunit)
{
	uint found;
	uint i;

	found = 0;

	for (i = 0; i < si->numcores; i++)
		if (si->coreid[i] == coreid) {
			if (found == coreunit)
				return (i);
			found++;
		}

	return (BADIDX);
}

/* 
 * this function changes logical "focus" to the indiciated core,
 * must be called with interrupt off.
 * Moreover, callers should keep interrupts off during switching out of and back to d11 core
 */
void*
sb_setcoreidx(sb_t *sbh, uint coreidx)
{
	sb_info_t *si;
	uint32 sbaddr;
	uint8 tmp;

	si = SB_INFO(sbh);

	if (coreidx >= si->numcores)
		return (NULL);

	/*
	 * If the user has provided an interrupt mask enabled function,
	 * then assert interrupts are disabled before switching the core.
	 */
	ASSERT((si->intrsenabled_fn == NULL) || !(*(si)->intrsenabled_fn)((si)->intr_arg));

	sbaddr = SB_ENUM_BASE + (coreidx * SB_CORE_SIZE);

	switch (BUSTYPE(si->sb.bustype)) {
	case SB_BUS:
		/* map new one */
		if (!si->regs[coreidx]) {
			si->regs[coreidx] = (void*)REG_MAP(sbaddr, SB_CORE_SIZE);
			ASSERT(GOODREGS(si->regs[coreidx]));
		}
		si->curmap = si->regs[coreidx];
		break;

	case PCI_BUS:
		/* point bar0 window */
		OSL_PCI_WRITE_CONFIG(si->osh, PCI_BAR0_WIN, 4, sbaddr);
		break;

	case PCMCIA_BUS:
		tmp = (sbaddr >> 12) & 0x0f;
		OSL_PCMCIA_WRITE_ATTR(si->osh, PCMCIA_ADDR0, &tmp, 1);
		tmp = (sbaddr >> 16) & 0xff;
		OSL_PCMCIA_WRITE_ATTR(si->osh, PCMCIA_ADDR1, &tmp, 1);
		tmp = (sbaddr >> 24) & 0xff;
		OSL_PCMCIA_WRITE_ATTR(si->osh, PCMCIA_ADDR2, &tmp, 1);
		break;
#ifdef BCMJTAG
	case JTAG_BUS:
		/* map new one */
		if (!si->regs[coreidx]) {
			si->regs[coreidx] = (void *)sbaddr;
			ASSERT(GOODREGS(si->regs[coreidx]));
		}
		si->curmap = si->regs[coreidx];
		break;
#endif	/* BCMJTAG */
	}

	si->curidx = coreidx;

	return (si->curmap);
}

/* 
 * this function changes logical "focus" to the indiciated core,
 * must be called with interrupt off.
 * Moreover, callers should keep interrupts off during switching out of and back to d11 core
 */
void*
sb_setcore(sb_t *sbh, uint coreid, uint coreunit)
{
	sb_info_t *si;
	uint idx;

	si = SB_INFO(sbh);
	idx = sb_findcoreidx(si, coreid, coreunit);
	if (!GOODIDX(idx))
		return (NULL);

	return (sb_setcoreidx(sbh, idx));
}

/* return chip number */
uint
sb_chip(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.chip);
}

/* return chip revision number */
uint
sb_chiprev(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.chiprev);
}

/* return chip common revision number */
uint
sb_chipcrev(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.ccrev);
}

/* return chip package option */
uint
sb_chippkg(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.chippkg);
}

/* return PCI core rev. */
uint
sb_pcirev(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.buscorerev);
}

bool
BCMINITFN(sb_war16165)(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	return (PCI(si) && (si->sb.buscorerev <= 10));
}

static void
BCMINITFN(sb_war30841)(sb_info_t *si)
{
	sb_pcie_mdiowrite(si, MDIODATA_DEV_RX, SERDES_RX_TIMER1, 0x8128);
	sb_pcie_mdiowrite(si, MDIODATA_DEV_RX, SERDES_RX_CDR, 0x0100);
	sb_pcie_mdiowrite(si, MDIODATA_DEV_RX, SERDES_RX_CDRBW, 0x1466);
}

/* return PCMCIA core rev. */
uint
BCMINITFN(sb_pcmciarev)(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.buscorerev);
}

/* return board vendor id */
uint
sb_boardvendor(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.boardvendor);
}

/* return boardtype */
uint
sb_boardtype(sb_t *sbh)
{
	sb_info_t *si;
	char *var;

	si = SB_INFO(sbh);

	if (BUSTYPE(si->sb.bustype) == SB_BUS && si->sb.boardtype == 0xffff) {
		/* boardtype format is a hex string */
		si->sb.boardtype = getintvar(NULL, "boardtype");

		/* backward compatibility for older boardtype string format */
		if ((si->sb.boardtype == 0) && (var = getvar(NULL, "boardtype"))) {
			if (!strcmp(var, "bcm94710dev"))
				si->sb.boardtype = BCM94710D_BOARD;
			else if (!strcmp(var, "bcm94710ap"))
				si->sb.boardtype = BCM94710AP_BOARD;
			else if (!strcmp(var, "bu4710"))
				si->sb.boardtype = BU4710_BOARD;
			else if (!strcmp(var, "bcm94702mn"))
				si->sb.boardtype = BCM94702MN_BOARD;
			else if (!strcmp(var, "bcm94710r1"))
				si->sb.boardtype = BCM94710R1_BOARD;
			else if (!strcmp(var, "bcm94710r4"))
				si->sb.boardtype = BCM94710R4_BOARD;
			else if (!strcmp(var, "bcm94702cpci"))
				si->sb.boardtype = BCM94702CPCI_BOARD;
			else if (!strcmp(var, "bcm95380_rr"))
				si->sb.boardtype = BCM95380RR_BOARD;
		}
	}

	return (si->sb.boardtype);
}

/* return bus type of sbh device */
uint
sb_bus(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	return (si->sb.bustype);
}

/* return bus core type */
uint
sb_buscoretype(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	return (si->sb.buscoretype);
}

/* return bus core revision */
uint
sb_buscorerev(sb_t *sbh)
{
	sb_info_t *si;
	si = SB_INFO(sbh);

	return (si->sb.buscorerev);
}

/* return list of found cores */
uint
sb_corelist(sb_t *sbh, uint coreid[])
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	bcopy((uchar*)si->coreid, (uchar*)coreid, (si->numcores * sizeof(uint)));
	return (si->numcores);
}

/* return current register mapping */
void *
sb_coreregs(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	ASSERT(GOODREGS(si->curmap));

	return (si->curmap);
}


/* do buffered registers update */
void
sb_commit(sb_t *sbh)
{
	sb_info_t *si;
	uint origidx;
	uint intr_val = 0;

	si = SB_INFO(sbh);

	origidx = si->curidx;
	ASSERT(GOODIDX(origidx));

	INTR_OFF(si, intr_val);

	/* switch over to chipcommon core if there is one, else use pci */
	if (si->sb.ccrev != NOREV) {
		chipcregs_t *ccregs = (chipcregs_t *)sb_setcore(sbh, SB_CC, 0);

		/* do the buffer registers update */
		W_REG(si->osh, &ccregs->broadcastaddress, SB_COMMIT);
		W_REG(si->osh, &ccregs->broadcastdata, 0x0);
	} else if (PCI(si)) {
		sbpciregs_t *pciregs = (sbpciregs_t *)sb_setcore(sbh, SB_PCI, 0);

		/* do the buffer registers update */
		W_REG(si->osh, &pciregs->bcastaddr, SB_COMMIT);
		W_REG(si->osh, &pciregs->bcastdata, 0x0);
	} else
		ASSERT(0);

	/* restore core index */
	sb_setcoreidx(sbh, origidx);
	INTR_RESTORE(si, intr_val);
}

/* reset and re-enable a core
 * inputs:
 * bits - core specific bits that are set during and after reset sequence
 * resetbits - core specific bits that are set only during reset sequence
 */
void
sb_core_reset(sb_t *sbh, uint32 bits, uint32 resetbits)
{
	sb_info_t *si;
	sbconfig_t *sb;
	volatile uint32 dummy;

	si = SB_INFO(sbh);
	ASSERT(GOODREGS(si->curmap));
	sb = REGS2SB(si->curmap);

	/*
	 * Must do the disable sequence first to work for arbitrary current core state.
	 */
	sb_core_disable(sbh, (bits | resetbits));

	/*
	 * Now do the initialization sequence.
	 */

	/* set reset while enabling the clock and forcing them on throughout the core */
	W_SBREG(si, &sb->sbtmstatelow, (SBTML_FGC | SBTML_CLK | SBTML_RESET | bits | resetbits));
	dummy = R_SBREG(si, &sb->sbtmstatelow);
	OSL_DELAY(1);

	if (R_SBREG(si, &sb->sbtmstatehigh) & SBTMH_SERR) {
		W_SBREG(si, &sb->sbtmstatehigh, 0);
	}
	if ((dummy = R_SBREG(si, &sb->sbimstate)) & (SBIM_IBE | SBIM_TO)) {
		AND_SBREG(si, &sb->sbimstate, ~(SBIM_IBE | SBIM_TO));
	}

	/* clear reset and allow it to propagate throughout the core */
	W_SBREG(si, &sb->sbtmstatelow, (SBTML_FGC | SBTML_CLK | bits));
	dummy = R_SBREG(si, &sb->sbtmstatelow);
	OSL_DELAY(1);

	/* leave clock enabled */
	W_SBREG(si, &sb->sbtmstatelow, (SBTML_CLK | bits));
	dummy = R_SBREG(si, &sb->sbtmstatelow);
	OSL_DELAY(1);
}

void
sb_core_tofixup(sb_t *sbh)
{
	sb_info_t *si;
	sbconfig_t *sb;

	si = SB_INFO(sbh);

	if ((BUSTYPE(si->sb.bustype) != PCI_BUS) || PCIE(si) ||
	    (PCI(si) && (si->sb.buscorerev >= 5)))
		return;

	ASSERT(GOODREGS(si->curmap));
	sb = REGS2SB(si->curmap);

	if (BUSTYPE(si->sb.bustype) == SB_BUS) {
		SET_SBREG(si, &sb->sbimconfiglow,
		          SBIMCL_RTO_MASK | SBIMCL_STO_MASK,
		          (0x5 << SBIMCL_RTO_SHIFT) | 0x3);
	} else {
		if (sb_coreid(sbh) == SB_PCI) {
			SET_SBREG(si, &sb->sbimconfiglow,
			          SBIMCL_RTO_MASK | SBIMCL_STO_MASK,
			          (0x3 << SBIMCL_RTO_SHIFT) | 0x2);
		} else {
			SET_SBREG(si, &sb->sbimconfiglow, (SBIMCL_RTO_MASK | SBIMCL_STO_MASK), 0);
		}
	}

	sb_commit(sbh);
}

/*
 * Set the initiator timeout for the "master core".
 * The master core is defined to be the core in control
 * of the chip and so it issues accesses to non-memory
 * locations (Because of dma *any* core can access memeory).
 *
 * The routine uses the bus to decide who is the master:
 *	SB_BUS => mips
 *	JTAG_BUS => chipc
 *	PCI_BUS => pci or pcie
 *	PCMCIA_BUS => pcmcia
 *	SDIO_BUS => pcmcia
 *
 * This routine exists so callers can disable initiator
 * timeouts so accesses to very slow devices like otp
 * won't cause an abort. The routine allows arbitrary
 * settings of the service and request timeouts, though.
 *
 * Returns the timeout state before changing it or -1
 * on error.
 */

#define	TO_MASK	(SBIMCL_RTO_MASK | SBIMCL_STO_MASK)

uint32
sb_set_initiator_to(sb_t *sbh, uint32 to)
{
	sb_info_t *si;
	uint origidx, idx;
	uint intr_val = 0;
	uint32 tmp, ret = 0xffffffff;
	sbconfig_t *sb;

	si = SB_INFO(sbh);

	if ((to & ~TO_MASK) != 0)
		return ret;

	/* Figure out the master core */
	idx = BADIDX;
	switch (BUSTYPE(si->sb.bustype)) {
	case PCI_BUS:
		idx = si->sb.buscoreidx;
		break;
	case JTAG_BUS:
		idx = SB_CC_IDX;
		break;
	case PCMCIA_BUS:
	case SDIO_BUS:
		idx = sb_findcoreidx(si, SB_PCMCIA, 0);
		break;
	case SB_BUS:
		if ((idx = sb_findcoreidx(si, SB_MIPS33, 0)) == BADIDX)
			idx = sb_findcoreidx(si, SB_MIPS, 0);
		break;
	default:
		ASSERT(0);
	}
	if (idx == BADIDX)
		return ret;

	INTR_OFF(si, intr_val);
	origidx = sb_coreidx(sbh);

	sb = REGS2SB(sb_setcoreidx(sbh, idx));

	tmp = R_SBREG(si, &sb->sbimconfiglow);
	ret = tmp & TO_MASK;
	W_SBREG(si, &sb->sbimconfiglow, (tmp & ~TO_MASK) | to);

	sb_commit(sbh);
	sb_setcoreidx(sbh, origidx);
	INTR_RESTORE(si, intr_val);
	return ret;
}

void
sb_core_disable(sb_t *sbh, uint32 bits)
{
	sb_info_t *si;
	volatile uint32 dummy;
	uint32 rej;
	sbconfig_t *sb;

	si = SB_INFO(sbh);

	ASSERT(GOODREGS(si->curmap));
	sb = REGS2SB(si->curmap);

	/* if core is already in reset, just return */
	if (R_SBREG(si, &sb->sbtmstatelow) & SBTML_RESET)
		return;

	/* reject value changed between sonics 2.2 and 2.3 */
	if (si->sb.sonicsrev == SONICS_2_2)
		rej = (1 << SBTML_REJ_SHIFT);
	else
		rej = (2 << SBTML_REJ_SHIFT);

	/* if clocks are not enabled, put into reset and return */
	if ((R_SBREG(si, &sb->sbtmstatelow) & SBTML_CLK) == 0)
		goto disable;

	/* set target reject and spin until busy is clear (preserve core-specific bits) */
	OR_SBREG(si, &sb->sbtmstatelow, rej);
	dummy = R_SBREG(si, &sb->sbtmstatelow);
	OSL_DELAY(1);
	SPINWAIT((R_SBREG(si, &sb->sbtmstatehigh) & SBTMH_BUSY), 100000);
	if (R_SBREG(si, &sb->sbtmstatehigh) & SBTMH_BUSY)
		SB_ERROR(("%s: target state still busy\n", __FUNCTION__));

	if (R_SBREG(si, &sb->sbidlow) & SBIDL_INIT) {
		OR_SBREG(si, &sb->sbimstate, SBIM_RJ);
		dummy = R_SBREG(si, &sb->sbimstate);
		OSL_DELAY(1);
		SPINWAIT((R_SBREG(si, &sb->sbimstate) & SBIM_BY), 100000);
	}

	/* set reset and reject while enabling the clocks */
	W_SBREG(si, &sb->sbtmstatelow, (bits | SBTML_FGC | SBTML_CLK | rej | SBTML_RESET));
	dummy = R_SBREG(si, &sb->sbtmstatelow);
	OSL_DELAY(10);

	/* don't forget to clear the initiator reject bit */
	if (R_SBREG(si, &sb->sbidlow) & SBIDL_INIT)
		AND_SBREG(si, &sb->sbimstate, ~SBIM_RJ);

disable:
	/* leave reset and reject asserted */
	W_SBREG(si, &sb->sbtmstatelow, (bits | rej | SBTML_RESET));
	OSL_DELAY(1);
}

/* set chip watchdog reset timer to fire in 'ticks' backplane cycles */
void
sb_watchdog(sb_t *sbh, uint ticks)
{
	sb_info_t *si = SB_INFO(sbh);

	/* make sure we come up in fast clock mode */
	sb_clkctl_clk(sbh, CLK_FAST);

	/* instant NMI */
	switch (si->gpioid) {
	case SB_CC:
#ifdef __mips__
		if (sb_chip(sbh) == BCM4785_CHIP_ID && ticks <= 1)
			MTC0(C0_BROADCOM, 4, (1 << 22));
#endif	/* __mips__ */
		sb_corereg(si, 0, OFFSETOF(chipcregs_t, watchdog), ~0, ticks);
#ifdef __mips__
		if (sb_chip(sbh) == BCM4785_CHIP_ID && ticks <= 1) {
			__asm__ __volatile__ (
				".set\tmips3\n\t"
				"sync\n\t"
				"wait\n\t"
				".set\tmips0"
			);
			while (1);
		}
#endif	/* __mips__ */
		break;
	case SB_EXTIF:
		sb_corereg(si, si->gpioidx, OFFSETOF(extifregs_t, watchdog), ~0, ticks);
		break;
	}
}

/* initialize the pcmcia core */
void
sb_pcmcia_init(sb_t *sbh)
{
	sb_info_t *si;
	uint8 cor = 0;

	si = SB_INFO(sbh);

	/* enable d11 mac interrupts */
	OSL_PCMCIA_READ_ATTR(si->osh, PCMCIA_FCR0 + PCMCIA_COR, &cor, 1);
	cor |= COR_IRQEN | COR_FUNEN;
	OSL_PCMCIA_WRITE_ATTR(si->osh, PCMCIA_FCR0 + PCMCIA_COR, &cor, 1);

}


/*
 * Configure the pci core for pci client (NIC) action
 * coremask is the bitvec of cores by index to be enabled.
 */
void
BCMINITFN(sb_pci_setup)(sb_t *sbh, uint coremask)
{
	sb_info_t *si;
	sbconfig_t *sb;
	sbpciregs_t *pciregs;
	uint32 sbflag;
	uint32 w;
	uint idx;
	int reg_val;

	si = SB_INFO(sbh);

	/* if not pci bus, we're done */
	if (BUSTYPE(si->sb.bustype) != PCI_BUS)
		return;

	ASSERT(PCI(si) || PCIE(si));
	ASSERT(si->sb.buscoreidx != BADIDX);

	/* get current core index */
	idx = si->curidx;

	/* we interrupt on this backplane flag number */
	ASSERT(GOODREGS(si->curmap));
	sb = REGS2SB(si->curmap);
	sbflag = R_SBREG(si, &sb->sbtpsflag) & SBTPS_NUM0_MASK;

	/* switch over to pci core */
	pciregs = (sbpciregs_t*) sb_setcoreidx(sbh, si->sb.buscoreidx);
	sb = REGS2SB(pciregs);

	/*
	 * Enable sb->pci interrupts.  Assume
	 * PCI rev 2.3 support was added in pci core rev 6 and things changed..
	 */
	if (PCIE(si) || (PCI(si) && ((si->sb.buscorerev) >= 6))) {
		/* pci config write to set this core bit in PCIIntMask */
		w = OSL_PCI_READ_CONFIG(si->osh, PCI_INT_MASK, sizeof(uint32));
		w |= (coremask << PCI_SBIM_SHIFT);
		OSL_PCI_WRITE_CONFIG(si->osh, PCI_INT_MASK, sizeof(uint32), w);
	} else {
		/* set sbintvec bit for our flag number */
		OR_SBREG(si, &sb->sbintvec, (1 << sbflag));
	}

	if (PCI(si)) {
		OR_REG(si->osh, &pciregs->sbtopci2, (SBTOPCI_PREF|SBTOPCI_BURST));
		if (si->sb.buscorerev >= 11)
			OR_REG(si->osh, &pciregs->sbtopci2, SBTOPCI_RC_READMULTI);
		if (si->sb.buscorerev < 5) {
			SET_SBREG(si, &sb->sbimconfiglow, SBIMCL_RTO_MASK | SBIMCL_STO_MASK,
				(0x3 << SBIMCL_RTO_SHIFT) | 0x2);
			sb_commit(sbh);
		}
	}

#ifdef PCIE_SUPPOER
	/* PCIE workarounds */
	if (PCIE(si)) {
		if ((si->sb.buscorerev == 0) || (si->sb.buscorerev == 1)) {
			reg_val = sb_pcie_readreg((void *)sbh, (void *)PCIE_PCIEREGS,
				PCIE_TLP_WORKAROUNDSREG);
			reg_val |= 0x8;
			sb_pcie_writereg((void *)sbh, (void *)PCIE_PCIEREGS,
				PCIE_TLP_WORKAROUNDSREG, reg_val);
		}

		if (si->sb.buscorerev == 1) {
			reg_val = sb_pcie_readreg((void *)sbh, (void *)PCIE_PCIEREGS,
				PCIE_DLLP_LCREG);
			reg_val |= (0x40);
			sb_pcie_writereg(sbh, (void *)PCIE_PCIEREGS, PCIE_DLLP_LCREG, reg_val);
		}

		if (si->sb.buscorerev == 0)
			sb_war30841(si);
	}
#endif

	/* switch back to previous core */
	sb_setcoreidx(sbh, idx);
}

uint32
sb_base(uint32 admatch)
{
	uint32 base;
	uint type;

	type = admatch & SBAM_TYPE_MASK;
	ASSERT(type < 3);

	base = 0;

	if (type == 0) {
		base = admatch & SBAM_BASE0_MASK;
	} else if (type == 1) {
		ASSERT(!(admatch & SBAM_ADNEG));	/* neg not supported */
		base = admatch & SBAM_BASE1_MASK;
	} else if (type == 2) {
		ASSERT(!(admatch & SBAM_ADNEG));	/* neg not supported */
		base = admatch & SBAM_BASE2_MASK;
	}

	return (base);
}

uint32
sb_size(uint32 admatch)
{
	uint32 size;
	uint type;

	type = admatch & SBAM_TYPE_MASK;
	ASSERT(type < 3);

	size = 0;

	if (type == 0) {
		size = 1 << (((admatch & SBAM_ADINT0_MASK) >> SBAM_ADINT0_SHIFT) + 1);
	} else if (type == 1) {
		ASSERT(!(admatch & SBAM_ADNEG));	/* neg not supported */
		size = 1 << (((admatch & SBAM_ADINT1_MASK) >> SBAM_ADINT1_SHIFT) + 1);
	} else if (type == 2) {
		ASSERT(!(admatch & SBAM_ADNEG));	/* neg not supported */
		size = 1 << (((admatch & SBAM_ADINT2_MASK) >> SBAM_ADINT2_SHIFT) + 1);
	}

	return (size);
}

/* return the core-type instantiation # of the current core */
uint
sb_coreunit(sb_t *sbh)
{
	sb_info_t *si;
	uint idx;
	uint coreid;
	uint coreunit;
	uint i;

	si = SB_INFO(sbh);
	coreunit = 0;

	idx = si->curidx;

	ASSERT(GOODREGS(si->curmap));
	coreid = sb_coreid(sbh);

	/* count the cores of our type */
	for (i = 0; i < idx; i++)
		if (si->coreid[i] == coreid)
			coreunit++;

	return (coreunit);
}

static INLINE uint32
factor6(uint32 x)
{
	switch (x) {
	case CC_F6_2:	return 2;
	case CC_F6_3:	return 3;
	case CC_F6_4:	return 4;
	case CC_F6_5:	return 5;
	case CC_F6_6:	return 6;
	case CC_F6_7:	return 7;
	default:	return 0;
	}
}

/* calculate the speed the SB would run at given a set of clockcontrol values */
uint32
sb_clock_rate(uint32 pll_type, uint32 n, uint32 m)
{
	uint32 n1, n2, clock, m1, m2, m3, mc;

	n1 = n & CN_N1_MASK;
	n2 = (n & CN_N2_MASK) >> CN_N2_SHIFT;

	if (pll_type == PLL_TYPE6) {
		if (m & CC_T6_MMASK)
			return CC_T6_M1;
		else
			return CC_T6_M0;
	} else if ((pll_type == PLL_TYPE1) ||
	           (pll_type == PLL_TYPE3) ||
	           (pll_type == PLL_TYPE4) ||
	           (pll_type == PLL_TYPE7)) {
		n1 = factor6(n1);
		n2 += CC_F5_BIAS;
	} else if (pll_type == PLL_TYPE2) {
		n1 += CC_T2_BIAS;
		n2 += CC_T2_BIAS;
		ASSERT((n1 >= 2) && (n1 <= 7));
		ASSERT((n2 >= 5) && (n2 <= 23));
	} else if (pll_type == PLL_TYPE5) {
		return (100000000);
	} else
		ASSERT(0);
	/* PLL types 3 and 7 use BASE2 (25Mhz) */
	if ((pll_type == PLL_TYPE3) ||
	    (pll_type == PLL_TYPE7)) {
		clock =  CC_CLOCK_BASE2 * n1 * n2;
	} else
		clock = CC_CLOCK_BASE1 * n1 * n2;

	if (clock == 0)
		return 0;

	m1 = m & CC_M1_MASK;
	m2 = (m & CC_M2_MASK) >> CC_M2_SHIFT;
	m3 = (m & CC_M3_MASK) >> CC_M3_SHIFT;
	mc = (m & CC_MC_MASK) >> CC_MC_SHIFT;

	if ((pll_type == PLL_TYPE1) ||
	    (pll_type == PLL_TYPE3) ||
	    (pll_type == PLL_TYPE4) ||
	    (pll_type == PLL_TYPE7)) {
		m1 = factor6(m1);
		if ((pll_type == PLL_TYPE1) || (pll_type == PLL_TYPE3))
			m2 += CC_F5_BIAS;
		else
			m2 = factor6(m2);
		m3 = factor6(m3);

		switch (mc) {
		case CC_MC_BYPASS:	return (clock);
		case CC_MC_M1:		return (clock / m1);
		case CC_MC_M1M2:	return (clock / (m1 * m2));
		case CC_MC_M1M2M3:	return (clock / (m1 * m2 * m3));
		case CC_MC_M1M3:	return (clock / (m1 * m3));
		default:		return (0);
		}
	} else {
		ASSERT(pll_type == PLL_TYPE2);

		m1 += CC_T2_BIAS;
		m2 += CC_T2M2_BIAS;
		m3 += CC_T2_BIAS;
		ASSERT((m1 >= 2) && (m1 <= 7));
		ASSERT((m2 >= 3) && (m2 <= 10));
		ASSERT((m3 >= 2) && (m3 <= 7));

		if ((mc & CC_T2MC_M1BYP) == 0)
			clock /= m1;
		if ((mc & CC_T2MC_M2BYP) == 0)
			clock /= m2;
		if ((mc & CC_T2MC_M3BYP) == 0)
			clock /= m3;

		return (clock);
	}
}

/* returns the current speed the SB is running at */
uint32
sb_clock(sb_t *sbh)
{
	sb_info_t *si;
	extifregs_t *eir;
	chipcregs_t *cc;
	uint32 n, m;
	uint idx;
	uint32 pll_type, rate;
	uint intr_val = 0;

	si = SB_INFO(sbh);
	idx = si->curidx;
	pll_type = PLL_TYPE1;

	INTR_OFF(si, intr_val);

	/* switch to extif or chipc core */
	if ((eir = (extifregs_t *) sb_setcore(sbh, SB_EXTIF, 0))) {
		n = R_REG(si->osh, &eir->clockcontrol_n);
		m = R_REG(si->osh, &eir->clockcontrol_sb);
	} else if ((cc = (chipcregs_t *) sb_setcore(sbh, SB_CC, 0))) {
		pll_type = R_REG(si->osh, &cc->capabilities) & CAP_PLL_MASK;
		if (pll_type == PLL_NONE) {
			INTR_RESTORE(si, intr_val);
			return 80000000;
		}
		n = R_REG(si->osh, &cc->clockcontrol_n);
		if (pll_type == PLL_TYPE6)
			m = R_REG(si->osh, &cc->clockcontrol_m3);
		else if ((pll_type == PLL_TYPE3) && !(BCMINIT(sb_chip)(sbh) == 0x5365))
			m = R_REG(si->osh, &cc->clockcontrol_m2);
		else
			m = R_REG(si->osh, &cc->clockcontrol_sb);
	} else {
		INTR_RESTORE(si, intr_val);
		return 0;
	}

	/* calculate rate */
	if (BCMINIT(sb_chip)(sbh) == 0x5365)
		rate = 100000000;
	else {
		rate = sb_clock_rate(pll_type, n, m);

		if (pll_type == PLL_TYPE3)
			rate = rate / 2;
	}

	/* switch back to previous core */
	sb_setcoreidx(sbh, idx);

	INTR_RESTORE(si, intr_val);

	return rate;
}

/* change logical "focus" to the gpio core for optimized access */
void*
sb_gpiosetcore(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	return (sb_setcoreidx(sbh, si->gpioidx));
}

/* mask&set gpiocontrol bits */
uint32
sb_gpiocontrol(sb_t *sbh, uint32 mask, uint32 val, uint8 priority)
{
	sb_info_t *si;
	uint regoff;

	si = SB_INFO(sbh);
	regoff = 0;

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* gpios could be shared on router platforms */
	if ((BUSTYPE(si->sb.bustype) == SB_BUS) && (val || mask)) {
		mask = priority ? (sb_gpioreservation & mask) :
			((sb_gpioreservation | mask) & ~(sb_gpioreservation));
		val &= mask;
	}

	switch (si->gpioid) {
	case SB_CC:
		regoff = OFFSETOF(chipcregs_t, gpiocontrol);
		break;

	case SB_PCI:
		regoff = OFFSETOF(sbpciregs_t, gpiocontrol);
		break;

	case SB_EXTIF:
		return (0);
	}

	return (sb_corereg(si, si->gpioidx, regoff, mask, val));
}

/* mask&set gpio output enable bits */
uint32
sb_gpioouten(sb_t *sbh, uint32 mask, uint32 val, uint8 priority)
{
	sb_info_t *si;
	uint regoff;

	si = SB_INFO(sbh);
	regoff = 0;

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* gpios could be shared on router platforms */
	if ((BUSTYPE(si->sb.bustype) == SB_BUS) && (val || mask)) {
		mask = priority ? (sb_gpioreservation & mask) :
			((sb_gpioreservation | mask) & ~(sb_gpioreservation));
		val &= mask;
	}

	switch (si->gpioid) {
	case SB_CC:
		regoff = OFFSETOF(chipcregs_t, gpioouten);
		break;

	case SB_PCI:
		regoff = OFFSETOF(sbpciregs_t, gpioouten);
		break;

	case SB_EXTIF:
		regoff = OFFSETOF(extifregs_t, gpio[0].outen);
		break;
	}

	return (sb_corereg(si, si->gpioidx, regoff, mask, val));
}

/* mask&set gpio output bits */
uint32
sb_gpioout(sb_t *sbh, uint32 mask, uint32 val, uint8 priority)
{
	sb_info_t *si;
	uint regoff;

	si = SB_INFO(sbh);
	regoff = 0;

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* gpios could be shared on router platforms */
	if ((BUSTYPE(si->sb.bustype) == SB_BUS) && (val || mask)) {
		mask = priority ? (sb_gpioreservation & mask) :
			((sb_gpioreservation | mask) & ~(sb_gpioreservation));
		val &= mask;
	}

	switch (si->gpioid) {
	case SB_CC:
		regoff = OFFSETOF(chipcregs_t, gpioout);
		break;

	case SB_PCI:
		regoff = OFFSETOF(sbpciregs_t, gpioout);
		break;

	case SB_EXTIF:
		regoff = OFFSETOF(extifregs_t, gpio[0].out);
		break;
	}

	return (sb_corereg(si, si->gpioidx, regoff, mask, val));
}

/* reserve one gpio */
uint32
sb_gpioreserve(sb_t *sbh, uint32 gpio_bitmask, uint8 priority)
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* only cores on SB_BUS share GPIO's and only applcation users need to
	 * reserve/release GPIO
	 */
	if ((BUSTYPE(si->sb.bustype) != SB_BUS) || (!priority))  {
		ASSERT((BUSTYPE(si->sb.bustype) == SB_BUS) && (priority));
		return -1;
	}
	/* make sure only one bit is set */
	if ((!gpio_bitmask) || ((gpio_bitmask) & (gpio_bitmask - 1))) {
		ASSERT((gpio_bitmask) && !((gpio_bitmask) & (gpio_bitmask - 1)));
		return -1;
	}

	/* already reserved */
	if (sb_gpioreservation & gpio_bitmask)
		return -1;
	/* set reservation */
	sb_gpioreservation |= gpio_bitmask;

	return sb_gpioreservation;
}

/* release one gpio */
/* 
 * releasing the gpio doesn't change the current value on the GPIO last write value
 * persists till some one overwrites it
*/

uint32
sb_gpiorelease(sb_t *sbh, uint32 gpio_bitmask, uint8 priority)
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* only cores on SB_BUS share GPIO's and only applcation users need to
	 * reserve/release GPIO
	 */
	if ((BUSTYPE(si->sb.bustype) != SB_BUS) || (!priority))  {
		ASSERT((BUSTYPE(si->sb.bustype) == SB_BUS) && (priority));
		return -1;
	}
	/* make sure only one bit is set */
	if ((!gpio_bitmask) || ((gpio_bitmask) & (gpio_bitmask - 1))) {
		ASSERT((gpio_bitmask) && !((gpio_bitmask) & (gpio_bitmask - 1)));
		return -1;
	}

	/* already released */
	if (!(sb_gpioreservation & gpio_bitmask))
		return -1;

	/* clear reservation */
	sb_gpioreservation &= ~gpio_bitmask;

	return sb_gpioreservation;
}

/* return the current gpioin register value */
uint32
sb_gpioin(sb_t *sbh)
{
	sb_info_t *si;
	uint regoff;

	si = SB_INFO(sbh);
	regoff = 0;

	switch (si->gpioid) {
	case SB_CC:
		regoff = OFFSETOF(chipcregs_t, gpioin);
		break;

	case SB_PCI:
		regoff = OFFSETOF(sbpciregs_t, gpioin);
		break;

	case SB_EXTIF:
		regoff = OFFSETOF(extifregs_t, gpioin);
		break;
	}

	return (sb_corereg(si, si->gpioidx, regoff, 0, 0));
}

/* mask&set gpio interrupt polarity bits */
uint32
sb_gpiointpolarity(sb_t *sbh, uint32 mask, uint32 val, uint8 priority)
{
	sb_info_t *si;
	uint regoff;

	si = SB_INFO(sbh);
	regoff = 0;

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* gpios could be shared on router platforms */
	if ((BUSTYPE(si->sb.bustype) == SB_BUS) && (val || mask)) {
		mask = priority ? (sb_gpioreservation & mask) :
			((sb_gpioreservation | mask) & ~(sb_gpioreservation));
		val &= mask;
	}

	switch (si->gpioid) {
	case SB_CC:
		regoff = OFFSETOF(chipcregs_t, gpiointpolarity);
		break;

	case SB_PCI:
		/* pci gpio implementation does not support interrupt polarity */
		ASSERT(0);
		break;

	case SB_EXTIF:
		regoff = OFFSETOF(extifregs_t, gpiointpolarity);
		break;
	}

	return (sb_corereg(si, si->gpioidx, regoff, mask, val));
}

/* mask&set gpio interrupt mask bits */
uint32
sb_gpiointmask(sb_t *sbh, uint32 mask, uint32 val, uint8 priority)
{
	sb_info_t *si;
	uint regoff;

	si = SB_INFO(sbh);
	regoff = 0;

	priority = GPIO_DRV_PRIORITY; /* compatibility hack */

	/* gpios could be shared on router platforms */
	if ((BUSTYPE(si->sb.bustype) == SB_BUS) && (val || mask)) {
		mask = priority ? (sb_gpioreservation & mask) :
			((sb_gpioreservation | mask) & ~(sb_gpioreservation));
		val &= mask;
	}

	switch (si->gpioid) {
	case SB_CC:
		regoff = OFFSETOF(chipcregs_t, gpiointmask);
		break;

	case SB_PCI:
		/* pci gpio implementation does not support interrupt mask */
		ASSERT(0);
		break;

	case SB_EXTIF:
		regoff = OFFSETOF(extifregs_t, gpiointmask);
		break;
	}

	return (sb_corereg(si, si->gpioidx, regoff, mask, val));
}

/* assign the gpio to an led */
uint32
sb_gpioled(sb_t *sbh, uint32 mask, uint32 val)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	if (si->sb.ccrev < 16)
		return -1;

	/* gpio led powersave reg */
	return (sb_corereg(si, 0, OFFSETOF(chipcregs_t, gpiotimeroutmask), mask, val));
}

/* mask & set gpio timer val */
uint32
sb_gpiotimerval(sb_t *sbh, uint32 mask, uint32 gpiotimerval)
{
	sb_info_t *si;
	si = SB_INFO(sbh);

	if (si->sb.ccrev < 16)
		return -1;

	return (sb_corereg(si, 0, OFFSETOF(chipcregs_t, gpiotimerval), mask, gpiotimerval));
}


/* return the slow clock source - LPO, XTAL, or PCI */
static uint
sb_slowclk_src(sb_info_t *si)
{
	chipcregs_t *cc;


	ASSERT(sb_coreid(&si->sb) == SB_CC);

	if (si->sb.ccrev < 6) {
		if ((BUSTYPE(si->sb.bustype) == PCI_BUS) &&
		    (OSL_PCI_READ_CONFIG(si->osh, PCI_GPIO_OUT, sizeof(uint32)) &
		     PCI_CFG_GPIO_SCS))
			return (SCC_SS_PCI);
		else
			return (SCC_SS_XTAL);
	} else if (si->sb.ccrev < 10) {
		cc = (chipcregs_t*) sb_setcoreidx(&si->sb, si->curidx);
		return (R_REG(si->osh, &cc->slow_clk_ctl) & SCC_SS_MASK);
	} else	/* Insta-clock */
		return (SCC_SS_XTAL);
}

/* return the ILP (slowclock) min or max frequency */
static uint
sb_slowclk_freq(sb_info_t *si, bool max)
{
	chipcregs_t *cc;
	uint32 slowclk;
	uint div;


	ASSERT(sb_coreid(&si->sb) == SB_CC);

	cc = (chipcregs_t*) sb_setcoreidx(&si->sb, si->curidx);

	/* shouldn't be here unless we've established the chip has dynamic clk control */
	ASSERT(R_REG(si->osh, &cc->capabilities) & CAP_PWR_CTL);

	slowclk = sb_slowclk_src(si);
	if (si->sb.ccrev < 6) {
		if (slowclk == SCC_SS_PCI)
			return (max? (PCIMAXFREQ/64) : (PCIMINFREQ/64));
		else
			return (max? (XTALMAXFREQ/32) : (XTALMINFREQ/32));
	} else if (si->sb.ccrev < 10) {
		div = 4 * (((R_REG(si->osh, &cc->slow_clk_ctl) & SCC_CD_MASK) >> SCC_CD_SHIFT) + 1);
		if (slowclk == SCC_SS_LPO)
			return (max? LPOMAXFREQ : LPOMINFREQ);
		else if (slowclk == SCC_SS_XTAL)
			return (max? (XTALMAXFREQ/div) : (XTALMINFREQ/div));
		else if (slowclk == SCC_SS_PCI)
			return (max? (PCIMAXFREQ/div) : (PCIMINFREQ/div));
		else
			ASSERT(0);
	} else {
		/* Chipc rev 10 is InstaClock */
		div = R_REG(si->osh, &cc->system_clk_ctl) >> SYCC_CD_SHIFT;
		div = 4 * (div + 1);
		return (max ? XTALMAXFREQ : (XTALMINFREQ/div));
	}
	return (0);
}

static void
BCMINITFN(sb_clkctl_setdelay)(sb_info_t *si, void *chipcregs)
{
	chipcregs_t * cc;
	uint slowmaxfreq, pll_delay, slowclk;
	uint pll_on_delay, fref_sel_delay;

	pll_delay = PLL_DELAY;

	/* If the slow clock is not sourced by the xtal then add the xtal_on_delay
	 * since the xtal will also be powered down by dynamic clk control logic.
	 */

	slowclk = sb_slowclk_src(si);
	if (slowclk != SCC_SS_XTAL)
		pll_delay += XTAL_ON_DELAY;

	/* Starting with 4318 it is ILP that is used for the delays */
	slowmaxfreq = sb_slowclk_freq(si, (si->sb.ccrev >= 10) ? FALSE : TRUE);

	pll_on_delay = ((slowmaxfreq * pll_delay) + 999999) / 1000000;
	fref_sel_delay = ((slowmaxfreq * FREF_DELAY) + 999999) / 1000000;

	cc = (chipcregs_t *)chipcregs;
	W_REG(si->osh, &cc->pll_on_delay, pll_on_delay);
	W_REG(si->osh, &cc->fref_sel_delay, fref_sel_delay);
}

/* initialize power control delay registers */
void
BCMINITFN(sb_clkctl_init)(sb_t *sbh)
{
	sb_info_t *si;
	uint origidx;
	chipcregs_t *cc;

	si = SB_INFO(sbh);

	origidx = si->curidx;

	if ((cc = (chipcregs_t*) sb_setcore(sbh, SB_CC, 0)) == NULL)
		return;

	if ((si->sb.chip == BCM4321_CHIP_ID) && (si->sb.chiprev < 2))
		W_REG(si->osh, &cc->chipcontrol,
		      (si->sb.chiprev == 0) ? CHIPCTRL_4321A0_DEFAULT : CHIPCTRL_4321A1_DEFAULT);

	if (!(R_REG(si->osh, &cc->capabilities) & CAP_PWR_CTL))
		goto done;

	/* set all Instaclk chip ILP to 1 MHz */
	else if (si->sb.ccrev >= 10)
		SET_REG(si->osh, &cc->system_clk_ctl, SYCC_CD_MASK,
		        (ILP_DIV_1MHZ << SYCC_CD_SHIFT));

	sb_clkctl_setdelay(si, (void *)cc);

done:
	sb_setcoreidx(sbh, origidx);
}

/* return the value suitable for writing to the dot11 core FAST_PWRUP_DELAY register */
uint16
sb_clkctl_fast_pwrup_delay(sb_t *sbh)
{
	sb_info_t *si;
	uint origidx;
	chipcregs_t *cc;
	uint slowminfreq;
	uint16 fpdelay;
	uint intr_val = 0;

	si = SB_INFO(sbh);
	fpdelay = 0;
	origidx = si->curidx;

	INTR_OFF(si, intr_val);

	if ((cc = (chipcregs_t*) sb_setcore(sbh, SB_CC, 0)) == NULL)
		goto done;

	if (!(R_REG(si->osh, &cc->capabilities) & CAP_PWR_CTL))
		goto done;

	slowminfreq = sb_slowclk_freq(si, FALSE);
	fpdelay = (((R_REG(si->osh, &cc->pll_on_delay) + 2) * 1000000) +
	           (slowminfreq - 1)) / slowminfreq;

done:
	sb_setcoreidx(sbh, origidx);
	INTR_RESTORE(si, intr_val);
	return (fpdelay);
}

/* turn primary xtal and/or pll off/on */
int
sb_clkctl_xtal(sb_t *sbh, uint what, bool on)
{
	sb_info_t *si;
	uint32 in, out, outen;

	si = SB_INFO(sbh);

	switch (BUSTYPE(si->sb.bustype)) {


		case PCMCIA_BUS:
			return (0);


		case PCI_BUS:

			/* pcie core doesn't have any mapping to control the xtal pu */
			if (PCIE(si))
				return -1;

			in = OSL_PCI_READ_CONFIG(si->osh, PCI_GPIO_IN, sizeof(uint32));
			out = OSL_PCI_READ_CONFIG(si->osh, PCI_GPIO_OUT, sizeof(uint32));
			outen = OSL_PCI_READ_CONFIG(si->osh, PCI_GPIO_OUTEN, sizeof(uint32));

			/*
			 * Avoid glitching the clock if GPRS is already using it.
			 * We can't actually read the state of the PLLPD so we infer it
			 * by the value of XTAL_PU which *is* readable via gpioin.
			 */
			if (on && (in & PCI_CFG_GPIO_XTAL))
				return (0);

			if (what & XTAL)
				outen |= PCI_CFG_GPIO_XTAL;
			if (what & PLL)
				outen |= PCI_CFG_GPIO_PLL;

			if (on) {
				/* turn primary xtal on */
				if (what & XTAL) {
					out |= PCI_CFG_GPIO_XTAL;
					if (what & PLL)
						out |= PCI_CFG_GPIO_PLL;
					OSL_PCI_WRITE_CONFIG(si->osh, PCI_GPIO_OUT,
					                     sizeof(uint32), out);
					OSL_PCI_WRITE_CONFIG(si->osh, PCI_GPIO_OUTEN,
					                     sizeof(uint32), outen);
					OSL_DELAY(XTAL_ON_DELAY);
				}

				/* turn pll on */
				if (what & PLL) {
					out &= ~PCI_CFG_GPIO_PLL;
					OSL_PCI_WRITE_CONFIG(si->osh, PCI_GPIO_OUT,
					                     sizeof(uint32), out);
					OSL_DELAY(2000);
				}
			} else {
				if (what & XTAL)
					out &= ~PCI_CFG_GPIO_XTAL;
				if (what & PLL)
					out |= PCI_CFG_GPIO_PLL;
				OSL_PCI_WRITE_CONFIG(si->osh, PCI_GPIO_OUT, sizeof(uint32), out);
				OSL_PCI_WRITE_CONFIG(si->osh, PCI_GPIO_OUTEN, sizeof(uint32),
				                     outen);
			}

		default:
			return (-1);
	}

	return (0);
}

/* set dynamic clk control mode (forceslow, forcefast, dynamic) */
/*   returns true if we are forcing fast clock */
bool
sb_clkctl_clk(sb_t *sbh, uint mode)
{
	sb_info_t *si;
	uint origidx;
	chipcregs_t *cc;
	uint32 scc;
	uint intr_val = 0;

	si = SB_INFO(sbh);

	/* chipcommon cores prior to rev6 don't support dynamic clock control */
	if (si->sb.ccrev < 6)
		return (FALSE);


	/* Chips with ccrev 10 are EOL and they don't have SYCC_HR which we use below */
	ASSERT(si->sb.ccrev != 10);

	INTR_OFF(si, intr_val);

	origidx = si->curidx;

	if (sb_setcore(sbh, SB_MIPS33, 0) && (sb_corerev(&si->sb) <= 7) &&
	    (BUSTYPE(si->sb.bustype) == SB_BUS) && (si->sb.ccrev >= 10))
		goto done;

	/* PR32414WAR  "Force HT clock on" all the time, no dynamic clk ctl */
	if ((si->sb.chip == BCM4311_CHIP_ID) && (si->sb.chiprev <= 1))
		goto done;

	cc = (chipcregs_t*) sb_setcore(sbh, SB_CC, 0);
	ASSERT(cc != NULL);

	if (!(R_REG(si->osh, &cc->capabilities) & CAP_PWR_CTL))
		goto done;

	switch (mode) {
	case CLK_FAST:	/* force fast (pll) clock */
		if (si->sb.ccrev < 10) {
			/* don't forget to force xtal back on before we clear SCC_DYN_XTAL.. */
			sb_clkctl_xtal(&si->sb, XTAL, ON);

			SET_REG(si->osh, &cc->slow_clk_ctl, (SCC_XC | SCC_FS | SCC_IP), SCC_IP);
		} else
			OR_REG(si->osh, &cc->system_clk_ctl, SYCC_HR);
		/* wait for the PLL */
		OSL_DELAY(PLL_DELAY);
		break;

	case CLK_DYNAMIC:	/* enable dynamic clock control */

		if (si->sb.ccrev < 10) {
			scc = R_REG(si->osh, &cc->slow_clk_ctl);
			scc &= ~(SCC_FS | SCC_IP | SCC_XC);
			if ((scc & SCC_SS_MASK) != SCC_SS_XTAL)
				scc |= SCC_XC;
			W_REG(si->osh, &cc->slow_clk_ctl, scc);

			/* for dynamic control, we have to release our xtal_pu "force on" */
			if (scc & SCC_XC)
				sb_clkctl_xtal(&si->sb, XTAL, OFF);
		} else {
			/* Instaclock */
			AND_REG(si->osh, &cc->system_clk_ctl, ~SYCC_HR);
		}
		break;

	default:
		ASSERT(0);
	}

done:
	sb_setcoreidx(sbh, origidx);
	INTR_RESTORE(si, intr_val);
	return (mode == CLK_FAST);
}

/* register driver interrupt disabling and restoring callback functions */
void
sb_register_intr_callback(sb_t *sbh, void *intrsoff_fn, void *intrsrestore_fn,
                          void *intrsenabled_fn, void *intr_arg)
{
	sb_info_t *si;

	si = SB_INFO(sbh);
	si->intr_arg = intr_arg;
	si->intrsoff_fn = (sb_intrsoff_t)intrsoff_fn;
	si->intrsrestore_fn = (sb_intrsrestore_t)intrsrestore_fn;
	si->intrsenabled_fn = (sb_intrsenabled_t)intrsenabled_fn;
	/* save current core id.  when this function called, the current core
	 * must be the core which provides driver functions(il, et, wl, etc.)
	 */
	si->dev_coreid = si->coreid[si->curidx];
}


int
sb_corepciid(sb_t *sbh, uint func, uint16 *pcivendor, uint16 *pcidevice,
             uint8 *pciclass, uint8 *pcisubclass, uint8 *pciprogif,
             uint8 *pciheader)
{
	uint16 vendor = 0xffff, device = 0xffff;
	uint core, unit;
	uint chip, chippkg;
	uint nfunc;
	char varname[SB_DEVPATH_BUFSZ + 8];
	uint8 class, subclass, progif;
	char devpath[SB_DEVPATH_BUFSZ];
	uint8 header;

	core = sb_coreid(sbh);
	unit = sb_coreunit(sbh);

	chip = sb_chip(sbh);
	chippkg = sb_chippkg(sbh);

	progif = 0;
	header = PCI_HEADER_NORMAL;

	/* Verify whether the function exists for the core */
	nfunc = (core == SB_USB20H) ? 2 : 1;
	if (func >= nfunc)
		return BCME_ERROR;

	/* Known vendor translations */
	switch (sb_corevendor(sbh)) {
	case SB_VEND_BCM:
		vendor = VENDOR_BROADCOM;
		break;
	default:
		return BCME_ERROR;
	}

	/* Determine class based on known core codes */
	switch (core) {
	case SB_ILINE20:
		class = PCI_CLASS_NET;
		subclass = PCI_NET_ETHER;
		device = BCM47XX_ILINE_ID;
		break;
	case SB_ENET:
		class = PCI_CLASS_NET;
		subclass = PCI_NET_ETHER;
		device = BCM47XX_ENET_ID;
		break;
	case SB_GIGETH:
		class = PCI_CLASS_NET;
		subclass = PCI_NET_ETHER;
		device = BCM47XX_GIGETH_ID;
		break;
	case SB_SDRAM:
	case SB_MEMC:
		class = PCI_CLASS_MEMORY;
		subclass = PCI_MEMORY_RAM;
		device = (uint16)core;
		break;
	case SB_PCI:
	case SB_PCIE:
		class = PCI_CLASS_BRIDGE;
		subclass = PCI_BRIDGE_PCI;
		device = (uint16)core;
		header = PCI_HEADER_BRIDGE;
		break;
	case SB_MIPS:
	case SB_MIPS33:
		class = PCI_CLASS_CPU;
		subclass = PCI_CPU_MIPS;
		device = (uint16)core;
		break;
	case SB_CODEC:
		class = PCI_CLASS_COMM;
		subclass = PCI_COMM_MODEM;
		device = BCM47XX_V90_ID;
		break;
	case SB_USB:
		class = PCI_CLASS_SERIAL;
		subclass = PCI_SERIAL_USB;
		progif = 0x10; /* OHCI */
		device = BCM47XX_USB_ID;
		break;
	case SB_USB11H:
		class = PCI_CLASS_SERIAL;
		subclass = PCI_SERIAL_USB;
		progif = 0x10; /* OHCI */
		device = BCM47XX_USBH_ID;
		break;
	case SB_USB20H:
		class = PCI_CLASS_SERIAL;
		subclass = PCI_SERIAL_USB;
		progif = func == 0 ? 0x10 : 0x20; /* OHCI/EHCI */
		device = BCM47XX_USB20H_ID;
		header = 0x80; /* multifunction */
		break;
	case SB_USB11D:
		class = PCI_CLASS_SERIAL;
		subclass = PCI_SERIAL_USB;
		device = BCM47XX_USBD_ID;
		break;
	case SB_USB20D:
		class = PCI_CLASS_SERIAL;
		subclass = PCI_SERIAL_USB;
		device = BCM47XX_USB20D_ID;
		break;
	case SB_IPSEC:
		class = PCI_CLASS_CRYPT;
		subclass = PCI_CRYPT_NETWORK;
		device = BCM47XX_IPSEC_ID;
		break;
	case SB_ROBO:
		class = PCI_CLASS_NET;
		subclass = PCI_NET_OTHER;
		device = BCM47XX_ROBO_ID;
		break;
	case SB_EXTIF:
	case SB_CC:
		class = PCI_CLASS_MEMORY;
		subclass = PCI_MEMORY_FLASH;
		device = (uint16)core;
		break;
	case SB_D11:
		class = PCI_CLASS_NET;
		subclass = PCI_NET_OTHER;
		/* Let nvram variable override core ID */
		sb_devpath(sbh, devpath, sizeof(devpath));
		sprintf(varname, "%sdevid", devpath);
		if ((device = (uint16)getintvar(NULL, varname)))
			break;
		/*
		* no longer support wl%did, but keep the code
		* here for backward compatibility.
		*/
		sprintf(varname, "wl%did", unit);
		if ((device = (uint16)getintvar(NULL, varname)))
			break;
		/* Chip specific conversion */
		if (chip == BCM4712_CHIP_ID) {
			if (chippkg == BCM4712SMALL_PKG_ID)
				device = BCM4306_D11G_ID;
			else
				device = BCM4306_D11DUAL_ID;
			break;
		}
		/* ignore it */
		device = 0xffff;
		break;
	case SB_SATAXOR:
		class = PCI_CLASS_XOR;
		subclass = PCI_XOR_QDMA;
		device = BCM47XX_SATAXOR_ID;
		break;
	case SB_ATA100:
		class = PCI_CLASS_DASDI;
		subclass = PCI_DASDI_IDE;
		device = BCM47XX_ATA100_ID;
		break;

	default:
		class = subclass = progif = 0xff;
		device = (uint16)core;
		break;
	}

	*pcivendor = vendor;
	*pcidevice = device;
	*pciclass = class;
	*pcisubclass = subclass;
	*pciprogif = progif;
	*pciheader = header;

	return 0;
}



/* use the mdio interface to write to mdio slaves */
static int
sb_pcie_mdiowrite(sb_info_t *si,  uint physmedia, uint regaddr, uint val)
{
	uint mdiodata;
	uint i = 0;
	sbpcieregs_t *pcieregs;

	pcieregs = (sbpcieregs_t*) sb_setcoreidx(&si->sb, si->sb.buscoreidx);
	ASSERT(pcieregs);

	/* enable mdio access to SERDES */
	W_REG(si->osh, (&pcieregs->mdiocontrol), MDIOCTL_PREAM_EN | MDIOCTL_DIVISOR_VAL);

	mdiodata = MDIODATA_START | MDIODATA_WRITE |
		(physmedia << MDIODATA_DEVADDR_SHF) |
		(regaddr << MDIODATA_REGADDR_SHF) | MDIODATA_TA | val;

	W_REG(si->osh, (&pcieregs->mdiodata), mdiodata);

	PR28829_DELAY();

	/* retry till the transaction is complete */
	while (i < 10) {
		if (R_REG(si->osh, &(pcieregs->mdiocontrol)) & MDIOCTL_ACCESS_DONE) {
			/* Disable mdio access to SERDES */
			W_REG(si->osh, (&pcieregs->mdiocontrol), 0);
			return 0;
		}
		OSL_DELAY(1000);
		i++;
	}

	SB_ERROR(("sb_pcie_mdiowrite: timed out\n"));
	/* Disable mdio access to SERDES */
	W_REG(si->osh, (&pcieregs->mdiocontrol), 0);
	ASSERT(0);
	return 1;

}

/* indirect way to read pcie config regs */
uint
sb_pcie_readreg(void *sb, void* arg1, uint offset)
{
	sb_info_t *si;
	sb_t   *sbh;
	uint retval = 0xFFFFFFFF;
	sbpcieregs_t *pcieregs;
	uint addrtype;

	sbh = (sb_t *)sb;
	si = SB_INFO(sbh);
	ASSERT(PCIE(si));

	pcieregs = (sbpcieregs_t *)sb_setcore(sbh, SB_PCIE, 0);
	ASSERT(pcieregs);

	addrtype = (uint)((uintptr)arg1);
	switch (addrtype) {
		case PCIE_CONFIGREGS:
			W_REG(si->osh, (&pcieregs->configaddr), offset);
			retval = R_REG(si->osh, &(pcieregs->configdata));
			break;
		case PCIE_PCIEREGS:
			W_REG(si->osh, &(pcieregs->pcieaddr), offset);
			retval = R_REG(si->osh, &(pcieregs->pciedata));
			break;
		default:
			ASSERT(0);
			break;
	}
	return retval;
}

/* indirect way to write pcie config/mdio/pciecore regs */
uint
sb_pcie_writereg(sb_t *sbh, void *arg1,  uint offset, uint val)
{
	sb_info_t *si;
	sbpcieregs_t *pcieregs;
	uint addrtype;

	si = SB_INFO(sbh);
	ASSERT(PCIE(si));

	pcieregs = (sbpcieregs_t *)sb_setcore(sbh, SB_PCIE, 0);
	ASSERT(pcieregs);

	addrtype = (uint)((uintptr)arg1);

	switch (addrtype) {
		case PCIE_CONFIGREGS:
			W_REG(si->osh, (&pcieregs->configaddr), offset);
			W_REG(si->osh, (&pcieregs->configdata), val);
			break;
		case PCIE_PCIEREGS:
			W_REG(si->osh, (&pcieregs->pcieaddr), offset);
			W_REG(si->osh, (&pcieregs->pciedata), val);
			break;
		default:
			ASSERT(0);
			break;
	}
	return 0;
}

/* Build device path. Support SB, PCI, and JTAG for now. */
int
sb_devpath(sb_t *sbh, char *path, int size)
{
	ASSERT(path);
	ASSERT(size >= SB_DEVPATH_BUFSZ);

	switch (BUSTYPE((SB_INFO(sbh))->sb.bustype)) {
	case SB_BUS:
	case JTAG_BUS:
		sprintf(path, "sb/%u/", sb_coreidx(sbh));
		break;
	case PCI_BUS:
		ASSERT((SB_INFO(sbh))->osh);
		sprintf(path, "pci/%u/%u/", OSL_PCI_BUS((SB_INFO(sbh))->osh),
			OSL_PCI_SLOT((SB_INFO(sbh))->osh));
		break;
	case PCMCIA_BUS:
		SB_ERROR(("sb_devpath: OSL_PCMCIA_BUS() not implemented, bus 1 assumed\n"));
		SB_ERROR(("sb_devpath: OSL_PCMCIA_SLOT() not implemented, slot 1 assumed\n"));
		sprintf(path, "pc/%u/%u/", 1, 1);
		break;
	case SDIO_BUS:
		SB_ERROR(("sb_devpath: device 0 assumed\n"));
		sprintf(path, "sd/%u/", sb_coreidx(sbh));
		break;
	default:
		ASSERT(0);
		break;
	}

	return 0;
}

/*
 * Fixup SROMless PCI device's configuration.
 * The current core may be changed upon return.
 */
static int
sb_pci_fixcfg(sb_info_t *si)
{
	uint origidx, pciidx;
	sbpciregs_t *pciregs;
	sbpcieregs_t *pcieregs;
	uint16 val16, *reg16;
	char name[SB_DEVPATH_BUFSZ+16], *value;
	char devpath[SB_DEVPATH_BUFSZ];

	ASSERT(BUSTYPE(si->sb.bustype) == PCI_BUS);

	/* Fixup PI in SROM shadow area to enable the correct PCI core access */
	/* save the current index */
	origidx = sb_coreidx(&si->sb);

	/* check 'pi' is correct and fix it if not */
	if (si->sb.buscoretype == SB_PCIE) {
		pcieregs = (sbpcieregs_t *)sb_setcore(&si->sb, SB_PCIE, 0);
		ASSERT(pcieregs);
		reg16 = &pcieregs->sprom[SRSH_PI_OFFSET];
	} else if (si->sb.buscoretype == SB_PCI) {
		pciregs = (sbpciregs_t *)sb_setcore(&si->sb, SB_PCI, 0);
		ASSERT(pciregs);
		reg16 = &pciregs->sprom[SRSH_PI_OFFSET];
	} else {
		ASSERT(0);
		return -1;
	}
	pciidx = sb_coreidx(&si->sb);
	val16 = R_REG(si->osh, reg16);
	if (((val16 & SRSH_PI_MASK) >> SRSH_PI_SHIFT) != (uint16)pciidx) {
		val16 = (uint16)(pciidx << SRSH_PI_SHIFT) | (val16 & ~SRSH_PI_MASK);
		W_REG(si->osh, reg16, val16);
	}

	/* restore the original index */
	sb_setcoreidx(&si->sb, origidx);

	/*
	 * Fixup bar0window in PCI config space to make the core indicated
	 * by the nvram variable the current core.
	 * !Do it last, it may change the current core!
	 */
	if (sb_devpath(&si->sb, devpath, sizeof(devpath)))
		return -1;
	sprintf(name, "%sb0w", devpath);
	if ((value = getvar(NULL, name))) {
		OSL_PCI_WRITE_CONFIG(si->osh, PCI_BAR0_WIN, sizeof(uint32),
			bcm_strtoul(value, NULL, 16));
		/* update curidx since the current core is changed */
		si->curidx = _sb_coreidx(si);
		if (si->curidx == BADIDX) {
			SB_ERROR(("sb_pci_fixcfg: bad core index\n"));
			return -1;
		}
	}

	return 0;
}

static uint
sb_chipc_capability(sb_t *sbh)
{
	sb_info_t *si;

	si = SB_INFO(sbh);

	/* Make sure that there is ChipCommon core present */
	if (si->coreid[SB_CC_IDX] == SB_CC)
		return (sb_corereg(si, SB_CC_IDX, OFFSETOF(chipcregs_t, capabilities),
		                   0, 0));
	return 0;
}

/* Return ADDR64 capability of the backplane */
bool
sb_backplane64(sb_t *sbh)
{
	return ((sb_chipc_capability(sbh) & CAP_BKPLN64) != 0);
}

void
sb_btcgpiowar(sb_t *sbh)
{
	sb_info_t *si;
	uint origidx;
	uint intr_val = 0;
	chipcregs_t *cc;
	si = SB_INFO(sbh);

	/* Make sure that there is ChipCommon core present &&
	 * UART_TX is strapped to 1
	 */
	if (!(sb_chipc_capability(sbh) & CAP_UARTGPIO))
		return;

	/* sb_corereg cannot be used as we have to guarantee 8-bit read/writes */
	INTR_OFF(si, intr_val);

	origidx = sb_coreidx(sbh);

	cc = (chipcregs_t *)sb_setcore(sbh, SB_CC, 0);
	if (cc == NULL)
		goto end;

	W_REG(si->osh, &cc->uart0mcr, R_REG(si->osh, &cc->uart0mcr) | 0x04);

end:
	/* restore the original index */
	sb_setcoreidx(sbh, origidx);

	INTR_RESTORE(si, intr_val);
}

/* check if the device is removed */
bool
sb_deviceremoved(sb_t *sbh)
{
	uint32 w;
	sb_info_t *si;

	si = SB_INFO(sbh);

	switch (BUSTYPE(si->sb.bustype)) {
	case PCI_BUS:
		ASSERT(si->osh);
		w = OSL_PCI_READ_CONFIG(si->osh, PCI_CFG_VID, sizeof(uint32));
		if ((w & 0xFFFF) != VENDOR_BROADCOM)
			return TRUE;
		else
			return FALSE;
	default:
		return FALSE;
	}
	return FALSE;
}

/* Return the RAM size of the SOCRAM core */
uint32
sb_socram_size(sb_t *sbh)
{
	sb_info_t *si;
	uint origidx;
	uint intr_val = 0;

	sbsocramregs_t *regs;
	bool wasup;
	uint corerev;
	uint32 coreinfo;
	uint memsize = 0;

	si = SB_INFO(sbh);
	ASSERT(si);

	/* Block ints and save current core */
	INTR_OFF(si, intr_val);
	origidx = sb_coreidx(sbh);

	/* Switch to SOCRAM core */
	if (!(regs = sb_setcore(sbh, SB_SOCRAM, 0)))
		goto done;

	/* Get info for determining size */
	if (!(wasup = sb_iscoreup(sbh)))
		sb_core_reset(sbh, 0, 0);
	corerev = sb_corerev(sbh);
	coreinfo = R_REG(si->osh, &regs->coreinfo);

	/* Calculate size from coreinfo based on rev */
	switch (corerev) {
	case 0:
		memsize = 1 << (16 + (coreinfo & SRCI_MS0_MASK));
		break;
	default: /* rev >= 1 */
		memsize = 1 << (SR_BSZ_BASE + (coreinfo & SRCI_SRBSZ_MASK));
		memsize *= (coreinfo & SRCI_SRNB_MASK) >> SRCI_SRNB_SHIFT;
		break;
	}

	/* Return to previous state and core */
	if (!wasup)
		sb_core_disable(sbh, 0);
	sb_setcoreidx(sbh, origidx);

done:
	INTR_RESTORE(si, intr_val);
	return memsize;
}


