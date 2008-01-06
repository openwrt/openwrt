/*
 * Low-Level PCI and SB support for BCM47xx
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: hndpci.c,v 1.1.1.3 2006/04/08 06:13:39 honor Exp $
 */

#include <typedefs.h>
#include <osl.h>
#include <pcicfg.h>
#include <bcmdevs.h>
#include <sbconfig.h>
#include <sbutils.h>
#include <sbpci.h>
#include <bcmendian.h>
#include <bcmnvram.h>
#include <hndcpu.h>
#include <hndmips.h>
#include <hndpci.h>

/* debug/trace */
#ifdef BCMDBG_PCI
#define	PCI_MSG(args)	printf args
#else
#define	PCI_MSG(args)
#endif /* BCMDBG_PCI */

/* Can free sbpci_init() memory after boot */
#ifndef linux
#define __init
#endif /* linux */

/* Emulated configuration space */
typedef struct {
	int n;
	uint size0;
	uint size1;
	uint size2;
	uint size3;
} sb_bar_cfg_t;
static pci_config_regs sb_config_regs[SB_MAXCORES];
static sb_bar_cfg_t sb_bar_cfg[SB_MAXCORES];

/* Links to emulated and real PCI configuration spaces */
#define MAXFUNCS  2
typedef struct {
	pci_config_regs *emu;	/* emulated PCI config */
	pci_config_regs *pci;	/* real PCI config */
	sb_bar_cfg_t *bar;	/* region sizes */
} sb_pci_cfg_t;
static sb_pci_cfg_t sb_pci_cfg[SB_MAXCORES][MAXFUNCS];

/* Special emulated config space for non-existing device */
static pci_config_regs sb_pci_null = { 0xffff, 0xffff };

/* Banned cores */
static uint16 pci_ban[SB_MAXCORES] = { 0 };
static uint pci_banned = 0;

/* CardBus mode */
static bool cardbus = FALSE;

/* Disable PCI host core */
static bool pci_disabled = FALSE;

/* Host bridge slot #, default to 0 */
static uint8 pci_hbslot = 0;

/* Internal macros */
#define PCI_SLOTAD_MAP	16	/* SLOT<n> mapps to AD<n+16> */
#define PCI_HBSBCFG_REV	8	/* MIN. core rev. required to
				 * access host bridge PCI cfg space
				 * from SB
				 */

/*
 * Functions for accessing external PCI configuration space
 */

/* Assume one-hot slot wiring */
#define PCI_SLOT_MAX 16		/* Max. PCI Slots */

static uint32 config_cmd(sb_t * sbh, uint bus, uint dev, uint func, uint off)
{
	uint coreidx;
	sbpciregs_t *regs;
	uint32 addr = 0;
	osl_t *osh;

	/* CardBusMode supports only one device */
	if (cardbus && dev > 1)
		return 0;

	osh = sb_osh(sbh);

	coreidx = sb_coreidx(sbh);
	regs = (sbpciregs_t *) sb_setcore(sbh, SB_PCI, 0);

	/* Type 0 transaction */
	if (bus == 1) {
		/* Skip unwired slots */
		if (dev < PCI_SLOT_MAX) {
			uint32 win;

			/* Slide the PCI window to the appropriate slot */
			win =
			    (SBTOPCI_CFG0 |
			     ((1 << (dev + PCI_SLOTAD_MAP)) & SBTOPCI1_MASK));
			W_REG(osh, &regs->sbtopci1, win);
			addr = SB_PCI_CFG |
			    ((1 << (dev + PCI_SLOTAD_MAP)) & ~SBTOPCI1_MASK) |
			    (func << PCICFG_FUN_SHIFT) | (off & ~3);
		}
	} else {
		/* Type 1 transaction */
		W_REG(osh, &regs->sbtopci1, SBTOPCI_CFG1);
		addr = SB_PCI_CFG |
		    (bus << PCICFG_BUS_SHIFT) |
		    (dev << PCICFG_SLOT_SHIFT) |
		    (func << PCICFG_FUN_SHIFT) | (off & ~3);
	}

	sb_setcoreidx(sbh, coreidx);

	return addr;
}

/*
 * Read host bridge PCI config registers from Silicon Backplane (>=rev8).
 *
 * It returns TRUE to indicate that access to the host bridge's pci config
 * from SB is ok, and values in 'addr' and 'val' are valid.
 *
 * It can only read registers at multiple of 4-bytes. Callers must pick up
 * needed bytes from 'val' based on 'off' value. Value in 'addr' reflects
 * the register address where value in 'val' is read.
 */
static bool
sb_pcihb_read_config(sb_t * sbh, uint bus, uint dev, uint func, uint off,
		     uint32 ** addr, uint32 * val)
{
	sbpciregs_t *regs;
	osl_t *osh;
	uint coreidx;
	bool ret = FALSE;

	/* sanity check */
	ASSERT(bus == 1);
	ASSERT(dev == pci_hbslot);
	ASSERT(func == 0);

	osh = sb_osh(sbh);

	/* read pci config when core rev >= 8 */
	coreidx = sb_coreidx(sbh);
	regs = (sbpciregs_t *) sb_setcore(sbh, SB_PCI, 0);
	if (regs && sb_corerev(sbh) >= PCI_HBSBCFG_REV) {
		*addr = (uint32 *) & regs->pcicfg[func][off >> 2];
		*val = R_REG(osh, *addr);
		ret = TRUE;
	}
	sb_setcoreidx(sbh, coreidx);

	return ret;
}

int
extpci_read_config(sb_t * sbh, uint bus, uint dev, uint func, uint off,
		   void *buf, int len)
{
	uint32 addr = 0, *reg = NULL, val;
	int ret = 0;

	/*
	 * Set value to -1 when:
	 *      flag 'pci_disabled' is true;
	 *      value of 'addr' is zero;
	 *      REG_MAP() fails;
	 *      BUSPROBE() fails;
	 */
	if (pci_disabled)
		val = 0xffffffff;
	else if (bus == 1 && dev == pci_hbslot && func == 0 &&
		 sb_pcihb_read_config(sbh, bus, dev, func, off, &reg, &val)) ;
	else if (((addr = config_cmd(sbh, bus, dev, func, off)) == 0) ||
		 ((reg = (uint32 *) REG_MAP(addr, len)) == 0) ||
		 (BUSPROBE(val, reg) != 0))
		val = 0xffffffff;

	PCI_MSG(("%s: 0x%x <= 0x%p(0x%x), len %d, off 0x%x, buf 0x%p\n",
		 __FUNCTION__, val, reg, addr, len, off, buf));

	val >>= 8 * (off & 3);
	if (len == 4)
		*((uint32 *) buf) = val;
	else if (len == 2)
		*((uint16 *) buf) = (uint16) val;
	else if (len == 1)
		*((uint8 *) buf) = (uint8) val;
	else
		ret = -1;

	if (reg && addr)
		REG_UNMAP(reg);

	return ret;
}

int
extpci_write_config(sb_t * sbh, uint bus, uint dev, uint func, uint off,
		    void *buf, int len)
{
	osl_t *osh;
	uint32 addr = 0, *reg = NULL, val;
	int ret = 0;

	osh = sb_osh(sbh);

	/*
	 * Ignore write attempt when:
	 *      flag 'pci_disabled' is true;
	 *      value of 'addr' is zero;
	 *      REG_MAP() fails;
	 *      BUSPROBE() fails;
	 */
	if (pci_disabled)
		return 0;
	else if (bus == 1 && dev == pci_hbslot && func == 0 &&
		 sb_pcihb_read_config(sbh, bus, dev, func, off, &reg, &val)) ;
	else if (((addr = config_cmd(sbh, bus, dev, func, off)) == 0) ||
		 ((reg = (uint32 *) REG_MAP(addr, len)) == 0) ||
		 (BUSPROBE(val, reg) != 0))
		goto done;

	if (len == 4)
		val = *((uint32 *) buf);
	else if (len == 2) {
		val &= ~(0xffff << (8 * (off & 3)));
		val |= *((uint16 *) buf) << (8 * (off & 3));
	} else if (len == 1) {
		val &= ~(0xff << (8 * (off & 3)));
		val |= *((uint8 *) buf) << (8 * (off & 3));
	} else {
		ret = -1;
		goto done;
	}

	PCI_MSG(("%s: 0x%x => 0x%p\n", __FUNCTION__, val, reg));

	W_REG(osh, reg, val);

      done:
	if (reg && addr)
		REG_UNMAP(reg);

	return ret;
}

/*
 * Must access emulated PCI configuration at these locations even when
 * the real PCI config space exists and is accessible.
 *
 * PCI_CFG_VID (0x00)
 * PCI_CFG_DID (0x02)
 * PCI_CFG_PROGIF (0x09)
 * PCI_CFG_SUBCL  (0x0a)
 * PCI_CFG_BASECL (0x0b)
 * PCI_CFG_HDR (0x0e)
 * PCI_CFG_INT (0x3c)
 * PCI_CFG_PIN (0x3d)
 */
#define FORCE_EMUCFG(off, len) \
	((off == PCI_CFG_VID) || (off == PCI_CFG_DID) || \
	 (off == PCI_CFG_PROGIF) || \
	 (off == PCI_CFG_SUBCL) || (off == PCI_CFG_BASECL) || \
	 (off == PCI_CFG_HDR) || \
	 (off == PCI_CFG_INT) || (off == PCI_CFG_PIN))

/* Sync the emulation registers and the real PCI config registers. */
static void
sb_pcid_read_config(sb_t * sbh, uint coreidx, sb_pci_cfg_t * cfg,
		    uint off, uint len)
{
	osl_t *osh;
	uint oldidx;

	ASSERT(cfg);
	ASSERT(cfg->emu);
	ASSERT(cfg->pci);

	/* decide if real PCI config register access is necessary */
	if (FORCE_EMUCFG(off, len))
		return;

	osh = sb_osh(sbh);

	/* access to the real pci config space only when the core is up */
	oldidx = sb_coreidx(sbh);
	sb_setcoreidx(sbh, coreidx);
	if (sb_iscoreup(sbh)) {
		if (len == 4)
			*(uint32 *) ((ulong) cfg->emu + off) =
			    htol32(R_REG
				   (osh, (uint32 *) ((ulong) cfg->pci + off)));
		else if (len == 2)
			*(uint16 *) ((ulong) cfg->emu + off) =
			    htol16(R_REG
				   (osh, (uint16 *) ((ulong) cfg->pci + off)));
		else if (len == 1)
			*(uint8 *) ((ulong) cfg->emu + off) =
			    R_REG(osh, (uint8 *) ((ulong) cfg->pci + off));
	}
	sb_setcoreidx(sbh, oldidx);
}

static void
sb_pcid_write_config(sb_t * sbh, uint coreidx, sb_pci_cfg_t * cfg,
		     uint off, uint len)
{
	osl_t *osh;
	uint oldidx;

	ASSERT(cfg);
	ASSERT(cfg->emu);
	ASSERT(cfg->pci);

	osh = sb_osh(sbh);

	/* decide if real PCI config register access is necessary */
	if (FORCE_EMUCFG(off, len))
		return;

	/* access to the real pci config space only when the core is up */
	oldidx = sb_coreidx(sbh);
	sb_setcoreidx(sbh, coreidx);
	if (sb_iscoreup(sbh)) {
		if (len == 4)
			W_REG(osh, (uint32 *) ((ulong) cfg->pci + off),
			      ltoh32(*(uint32 *) ((ulong) cfg->emu + off)));
		else if (len == 2)
			W_REG(osh, (uint16 *) ((ulong) cfg->pci + off),
			      ltoh16(*(uint16 *) ((ulong) cfg->emu + off)));
		else if (len == 1)
			W_REG(osh, (uint8 *) ((ulong) cfg->pci + off),
			      *(uint8 *) ((ulong) cfg->emu + off));
	}
	sb_setcoreidx(sbh, oldidx);
}

/*
 * Functions for accessing translated SB configuration space
 */
static int
sb_read_config(sb_t * sbh, uint bus, uint dev, uint func, uint off, void *buf,
	       int len)
{
	pci_config_regs *cfg;

	if (dev >= SB_MAXCORES || func >= MAXFUNCS
	    || (off + len) > sizeof(pci_config_regs))
		return -1;
	cfg = sb_pci_cfg[dev][func].emu;

	ASSERT(ISALIGNED(off, len));
	ASSERT(ISALIGNED((uintptr) buf, len));

	/* use special config space if the device does not exist */
	if (!cfg)
		cfg = &sb_pci_null;
	/* sync emulation with real PCI config if necessary */
	else if (sb_pci_cfg[dev][func].pci)
		sb_pcid_read_config(sbh, dev, &sb_pci_cfg[dev][func], off, len);

	if (len == 4)
		*((uint32 *) buf) = ltoh32(*((uint32 *) ((ulong) cfg + off)));
	else if (len == 2)
		*((uint16 *) buf) = ltoh16(*((uint16 *) ((ulong) cfg + off)));
	else if (len == 1)
		*((uint8 *) buf) = *((uint8 *) ((ulong) cfg + off));
	else
		return -1;

	return 0;
}

static int
sb_write_config(sb_t * sbh, uint bus, uint dev, uint func, uint off, void *buf,
		int len)
{
	uint coreidx;
	void *regs;
	pci_config_regs *cfg;
	osl_t *osh;
	sb_bar_cfg_t *bar;

	if (dev >= SB_MAXCORES || func >= MAXFUNCS
	    || (off + len) > sizeof(pci_config_regs))
		return -1;
	cfg = sb_pci_cfg[dev][func].emu;
	if (!cfg)
		return -1;

	ASSERT(ISALIGNED(off, len));
	ASSERT(ISALIGNED((uintptr) buf, len));

	osh = sb_osh(sbh);

	/* Emulate BAR sizing */
	if (off >= OFFSETOF(pci_config_regs, base[0]) &&
	    off <= OFFSETOF(pci_config_regs, base[3]) &&
	    len == 4 && *((uint32 *) buf) == ~0) {
		coreidx = sb_coreidx(sbh);
		if ((regs = sb_setcoreidx(sbh, dev))) {
			bar = sb_pci_cfg[dev][func].bar;
			/* Highest numbered address match register */
			if (off == OFFSETOF(pci_config_regs, base[0]))
				cfg->base[0] = ~(bar->size0 - 1);
			else if (off == OFFSETOF(pci_config_regs, base[1])
				 && bar->n >= 1)
				cfg->base[1] = ~(bar->size1 - 1);
			else if (off == OFFSETOF(pci_config_regs, base[2])
				 && bar->n >= 2)
				cfg->base[2] = ~(bar->size2 - 1);
			else if (off == OFFSETOF(pci_config_regs, base[3])
				 && bar->n >= 3)
				cfg->base[3] = ~(bar->size3 - 1);
		}
		sb_setcoreidx(sbh, coreidx);
	} else if (len == 4)
		*((uint32 *) ((ulong) cfg + off)) = htol32(*((uint32 *) buf));
	else if (len == 2)
		*((uint16 *) ((ulong) cfg + off)) = htol16(*((uint16 *) buf));
	else if (len == 1)
		*((uint8 *) ((ulong) cfg + off)) = *((uint8 *) buf);
	else
		return -1;

	/* sync emulation with real PCI config if necessary */
	if (sb_pci_cfg[dev][func].pci)
		sb_pcid_write_config(sbh, dev, &sb_pci_cfg[dev][func], off,
				     len);

	return 0;
}

int
sbpci_read_config(sb_t * sbh, uint bus, uint dev, uint func, uint off,
		  void *buf, int len)
{
	if (bus == 0)
		return sb_read_config(sbh, bus, dev, func, off, buf, len);
	else
		return extpci_read_config(sbh, bus, dev, func, off, buf, len);
}

int
sbpci_write_config(sb_t * sbh, uint bus, uint dev, uint func, uint off,
		   void *buf, int len)
{
	if (bus == 0)
		return sb_write_config(sbh, bus, dev, func, off, buf, len);
	else
		return extpci_write_config(sbh, bus, dev, func, off, buf, len);
}

void sbpci_ban(uint16 core)
{
	if (pci_banned < ARRAYSIZE(pci_ban))
		pci_ban[pci_banned++] = core;
}

/*
 * Initiliaze PCI core. Return 0 after a successful initialization.
 * Otherwise return -1 to indicate there is no PCI core and return 1
 * to indicate PCI core is disabled.
 */
int __init sbpci_init_pci(sb_t * sbh)
{
	uint chip, chiprev, chippkg, host;
	uint32 boardflags;
	sbpciregs_t *pci;
	sbconfig_t *sb;
	uint32 val;
	int ret = 0;
	char *hbslot;
	osl_t *osh;

	chip = sb_chip(sbh);
	chiprev = sb_chiprev(sbh);
	chippkg = sb_chippkg(sbh);

	osh = sb_osh(sbh);

	if (!(pci = (sbpciregs_t *) sb_setcore(sbh, SB_PCI, 0))) {
		printk("PCI: no core\n");
		pci_disabled = TRUE;
		return -1;
	}

	if ((chip == 0x4310) && (chiprev == 0))
		pci_disabled = TRUE;

	sb = (sbconfig_t *) ((ulong) pci + SBCONFIGOFF);

	boardflags = (uint32) getintvar(NULL, "boardflags");

	/*
	 * The 200-pin BCM4712 package does not bond out PCI. Even when
	 * PCI is bonded out, some boards may leave the pins
	 * floating.
	 */
	if (((chip == BCM4712_CHIP_ID) &&
	     ((chippkg == BCM4712SMALL_PKG_ID) ||
	      (chippkg == BCM4712MID_PKG_ID))) || (boardflags & BFL_NOPCI))
		pci_disabled = TRUE;

	/* Enable the core */
	sb_core_reset(sbh, 0, 0);

	/*
	 * If the PCI core should not be touched (disabled, not bonded
	 * out, or pins floating), do not even attempt to access core
	 * registers. Otherwise, try to determine if it is in host
	 * mode.
	 */
	if (pci_disabled)
		host = 0;
	else
		host = !BUSPROBE(val, &pci->control);

	if (!host) {
		ret = 1;

		/* Disable PCI interrupts in client mode */
		W_REG(osh, &sb->sbintvec, 0);

		/* Disable the PCI bridge in client mode */
		sbpci_ban(SB_PCI);
		sb_core_disable(sbh, 0);

		printk("PCI: Disabled\n");
	} else {
		printk("PCI: Initializing host\n");

		/* Disable PCI SBReqeustTimeout for BCM4785 */
		if (chip == BCM4785_CHIP_ID) {
			AND_REG(osh, &sb->sbimconfiglow, ~0x00000070);
			sb_commit(sbh);
		}

		/* Reset the external PCI bus and enable the clock */
		W_REG(osh, &pci->control, 0x5);	/* enable the tristate drivers */
		W_REG(osh, &pci->control, 0xd);	/* enable the PCI clock */
		OSL_DELAY(150);	/* delay > 100 us */
		W_REG(osh, &pci->control, 0xf);	/* deassert PCI reset */
		/* Use internal arbiter and park REQ/GRNT at external master 0 */
		W_REG(osh, &pci->arbcontrol, PCI_INT_ARB);
		OSL_DELAY(1);	/* delay 1 us */
		if (sb_corerev(sbh) >= 8) {
			val = getintvar(NULL, "parkid");
			ASSERT(val <= PCI_PARKID_LAST);
			OR_REG(osh, &pci->arbcontrol, val << PCI_PARKID_SHIFT);
			OSL_DELAY(1);
		}

		/* Enable CardBusMode */
		cardbus = getintvar(NULL, "cardbus") == 1;
		if (cardbus) {
			printk("PCI: Enabling CardBus\n");
			/* GPIO 1 resets the CardBus device on bcm94710ap */
			sb_gpioout(sbh, 1, 1, GPIO_DRV_PRIORITY);
			sb_gpioouten(sbh, 1, 1, GPIO_DRV_PRIORITY);
			W_REG(osh, &pci->sprom[0],
			      R_REG(osh, &pci->sprom[0]) | 0x400);
		}

		/* 64 MB I/O access window */
		W_REG(osh, &pci->sbtopci0, SBTOPCI_IO);
		/* 64 MB configuration access window */
		W_REG(osh, &pci->sbtopci1, SBTOPCI_CFG0);
		/* 1 GB memory access window */
		W_REG(osh, &pci->sbtopci2, SBTOPCI_MEM | SB_PCI_DMA);

		/* Host bridge slot # nvram overwrite */
		if ((hbslot = nvram_get("pcihbslot"))) {
			pci_hbslot = simple_strtoul(hbslot, NULL, 0);
			ASSERT(pci_hbslot < PCI_MAX_DEVICES);
		}

		/* Enable PCI bridge BAR0 prefetch and burst */
		val = 6;
		sbpci_write_config(sbh, 1, pci_hbslot, 0, PCI_CFG_CMD, &val,
				   sizeof(val));

		/* Enable PCI interrupts */
		W_REG(osh, &pci->intmask, PCI_INTA);
	}

	return ret;
}

/*
 * Get the PCI region address and size information.
 */
static void __init
sbpci_init_regions(sb_t * sbh, uint func, pci_config_regs * cfg,
		   sb_bar_cfg_t * bar)
{
	osl_t *osh;
	uint16 coreid;
	void *regs;
	sbconfig_t *sb;
	uint32 base;

	osh = sb_osh(sbh);
	coreid = sb_coreid(sbh);
	regs = sb_coreregs(sbh);
	sb = (sbconfig_t *) ((ulong) regs + SBCONFIGOFF);

	switch (coreid) {
	case SB_USB20H:
		base = htol32(sb_base(R_REG(osh, &sb->sbadmatch0)));

		cfg->base[0] = func == 0 ? base : base + 0x800;	/* OHCI/EHCI */
		cfg->base[1] = 0;
		cfg->base[2] = 0;
		cfg->base[3] = 0;
		cfg->base[4] = 0;
		cfg->base[5] = 0;
		bar->n = 1;
		bar->size0 = func == 0 ? 0x200 : 0x100;	/* OHCI/EHCI */
		bar->size1 = 0;
		bar->size2 = 0;
		bar->size3 = 0;
		break;
	default:
		cfg->base[0] = htol32(sb_base(R_REG(osh, &sb->sbadmatch0)));
		cfg->base[1] = htol32(sb_base(R_REG(osh, &sb->sbadmatch1)));
		cfg->base[2] = htol32(sb_base(R_REG(osh, &sb->sbadmatch2)));
		cfg->base[3] = htol32(sb_base(R_REG(osh, &sb->sbadmatch3)));
		cfg->base[4] = 0;
		cfg->base[5] = 0;
		bar->n =
		    (R_REG(osh, &sb->sbidlow) & SBIDL_AR_MASK) >>
		    SBIDL_AR_SHIFT;
		bar->size0 = sb_size(R_REG(osh, &sb->sbadmatch0));
		bar->size1 = sb_size(R_REG(osh, &sb->sbadmatch1));
		bar->size2 = sb_size(R_REG(osh, &sb->sbadmatch2));
		bar->size3 = sb_size(R_REG(osh, &sb->sbadmatch3));
		break;
	}
}

/*
 * Construct PCI config spaces for SB cores so that they
 * can be accessed as if they were PCI devices.
 */
static void __init sbpci_init_cores(sb_t * sbh)
{
	uint chiprev, coreidx, i;
	sbconfig_t *sb;
	pci_config_regs *cfg, *pci;
	sb_bar_cfg_t *bar;
	void *regs;
	osl_t *osh;
	uint16 vendor, device;
	uint16 coreid;
	uint8 class, subclass, progif;
	uint dev;
	uint8 header;
	uint func;

	chiprev = sb_chiprev(sbh);
	coreidx = sb_coreidx(sbh);

	osh = sb_osh(sbh);

	/* Scan the SB bus */
	bzero(sb_config_regs, sizeof(sb_config_regs));
	bzero(sb_bar_cfg, sizeof(sb_bar_cfg));
	bzero(sb_pci_cfg, sizeof(sb_pci_cfg));
	memset(&sb_pci_null, -1, sizeof(sb_pci_null));
	cfg = sb_config_regs;
	bar = sb_bar_cfg;
	for (dev = 0; dev < SB_MAXCORES; dev++) {
		/* Check if the core exists */
		if (!(regs = sb_setcoreidx(sbh, dev)))
			continue;
		sb = (sbconfig_t *) ((ulong) regs + SBCONFIGOFF);

		/* Check if this core is banned */
		coreid = sb_coreid(sbh);
		for (i = 0; i < pci_banned; i++)
			if (coreid == pci_ban[i])
				break;
		if (i < pci_banned)
			continue;

		for (func = 0; func < MAXFUNCS; ++func) {
			/* Make sure we won't go beyond the limit */
			if (cfg >= &sb_config_regs[SB_MAXCORES]) {
				printk("PCI: too many emulated devices\n");
				goto done;
			}

			/* Convert core id to pci id */
			if (sb_corepciid
			    (sbh, func, &vendor, &device, &class, &subclass,
			     &progif, &header))
				continue;

			/*
			 * Differentiate real PCI config from emulated.
			 * non zero 'pci' indicate there is a real PCI config space
			 * for this device.
			 */
			switch (device) {
			case BCM47XX_GIGETH_ID:
				pci =
				    (pci_config_regs *) ((uint32) regs + 0x800);
				break;
			case BCM47XX_SATAXOR_ID:
				pci =
				    (pci_config_regs *) ((uint32) regs + 0x400);
				break;
			case BCM47XX_ATA100_ID:
				pci =
				    (pci_config_regs *) ((uint32) regs + 0x800);
				break;
			default:
				pci = NULL;
				break;
			}
			/* Supported translations */
			cfg->vendor = htol16(vendor);
			cfg->device = htol16(device);
			cfg->rev_id = chiprev;
			cfg->prog_if = progif;
			cfg->sub_class = subclass;
			cfg->base_class = class;
			cfg->header_type = header;
			sbpci_init_regions(sbh, func, cfg, bar);
			/* Save core interrupt flag */
			cfg->int_pin =
			    R_REG(osh, &sb->sbtpsflag) & SBTPS_NUM0_MASK;
			/* Save core interrupt assignment */
			cfg->int_line = sb_irq(sbh);
			/* Indicate there is no SROM */
			*((uint32 *) & cfg->sprom_control) = 0xffffffff;

			/* Point to the PCI config spaces */
			sb_pci_cfg[dev][func].emu = cfg;
			sb_pci_cfg[dev][func].pci = pci;
			sb_pci_cfg[dev][func].bar = bar;
			cfg++;
			bar++;
		}
	}

      done:
	sb_setcoreidx(sbh, coreidx);
}

/*
 * Initialize PCI core and construct PCI config spaces for SB cores.
 * Must propagate sbpci_init_pci() return value to the caller to let
 * them know the PCI core initialization status.
 */
int __init sbpci_init(sb_t * sbh)
{
	int status = sbpci_init_pci(sbh);
	sbpci_init_cores(sbh);
	return status;
}
