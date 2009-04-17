/*
 * Misc utility routines for accessing chip-specific features
 * of Broadcom HNBU SiliconBackplane-based chips.
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

#ifndef	_sbutils_h_
#define	_sbutils_h_

/* 
 * Data structure to export all chip specific common variables
 * public (read-only) portion of sbutils handle returned by
 * sb_attach()/sb_kattach()
*/

struct sb_pub {

	uint	bustype;		/* SB_BUS, PCI_BUS  */
	uint	buscoretype;		/* SB_PCI, SB_PCMCIA, SB_PCIE */
	uint	buscorerev;		/* buscore rev */
	uint	buscoreidx;		/* buscore index */
	int	ccrev;			/* chip common core rev */
	uint32	cccaps;			/* chip common capabilities */
	int	pmurev;			/* pmu core rev */
	uint32	pmucaps;		/* pmu capabilities */
	uint	boardtype;		/* board type */
	uint	boardvendor;		/* board vendor */
	uint	boardflags;		/* board flags */
	uint	chip;			/* chip number */
	uint	chiprev;		/* chip revision */
	uint	chippkg;		/* chip package option */
	uint32	chipst;			/* chip status */
	uint    sonicsrev;		/* sonics backplane rev */
	bool	pr42780;		/* whether PCIE 42780 WAR applies to this chip */
	bool	pr32414;		/* whether 432414 WAR applis to the chip */
};

typedef const struct sb_pub sb_t;

/*
 * Many of the routines below take an 'sbh' handle as their first arg.
 * Allocate this by calling sb_attach().  Free it by calling sb_detach().
 * At any one time, the sbh is logically focused on one particular sb core
 * (the "current core").
 * Use sb_setcore() or sb_setcoreidx() to change the association to another core.
 */

#define	SB_OSH		NULL	/* Use for sb_kattach when no osh is available */

/* exported externs */
extern sb_t *sb_attach(uint pcidev, osl_t *osh, void *regs, uint bustype,
                       void *sdh, char **vars, uint *varsz);
extern sb_t *sb_kattach(osl_t *osh);
extern void sb_detach(sb_t *sbh);
extern uint sb_chip(sb_t *sbh);
extern uint sb_chiprev(sb_t *sbh);
extern uint sb_chipcrev(sb_t *sbh);
extern uint sb_chippkg(sb_t *sbh);
extern uint sb_pcirev(sb_t *sbh);
extern bool sb_war16165(sb_t *sbh);
extern uint sb_pcmciarev(sb_t *sbh);
extern uint sb_boardvendor(sb_t *sbh);
extern uint sb_boardtype(sb_t *sbh);
extern uint sb_bus(sb_t *sbh);
extern uint sb_buscoretype(sb_t *sbh);
extern uint sb_buscorerev(sb_t *sbh);
extern uint sb_corelist(sb_t *sbh, uint coreid[]);
extern uint sb_coreid(sb_t *sbh);
extern uint sb_flag(sb_t *sbh);
extern uint sb_coreidx(sb_t *sbh);
extern uint sb_coreunit(sb_t *sbh);
extern uint sb_corevendor(sb_t *sbh);
extern uint sb_corerev(sb_t *sbh);
extern void *sb_osh(sb_t *sbh);
extern void sb_setosh(sb_t *sbh, osl_t *osh);
extern uint sb_corereg(sb_t *sbh, uint coreidx, uint regoff, uint mask, uint val);
extern void *sb_coreregs(sb_t *sbh);
extern uint32 sb_coreflags(sb_t *sbh, uint32 mask, uint32 val);
extern void sb_coreflags_wo(sb_t *sbh, uint32 mask, uint32 val);
extern uint32 sb_coreflagshi(sb_t *sbh, uint32 mask, uint32 val);
extern bool sb_iscoreup(sb_t *sbh);
extern uint sb_findcoreidx(sb_t *sbh, uint coreid, uint coreunit);
extern void *sb_setcoreidx(sb_t *sbh, uint coreidx);
extern void *sb_setcore(sb_t *sbh, uint coreid, uint coreunit);
extern int sb_corebist(sb_t *sbh);
extern void sb_commit(sb_t *sbh);
extern uint32 sb_base(uint32 admatch);
extern uint32 sb_size(uint32 admatch);
extern void sb_core_reset(sb_t *sbh, uint32 bits, uint32 resetbits);
extern void sb_core_tofixup(sb_t *sbh);
extern void sb_core_disable(sb_t *sbh, uint32 bits);
extern uint32 sb_clock_rate(uint32 pll_type, uint32 n, uint32 m);
extern uint32 sb_clock(sb_t *sbh);
extern uint32 sb_alp_clock(sb_t *sbh);
extern void sb_pci_setup(sb_t *sbh, uint coremask);
extern void sb_pcmcia_init(sb_t *sbh);
extern void sb_watchdog(sb_t *sbh, uint ticks);
extern void *sb_gpiosetcore(sb_t *sbh);
extern uint32 sb_gpiocontrol(sb_t *sbh, uint32 mask, uint32 val, uint8 priority);
extern uint32 sb_gpioouten(sb_t *sbh, uint32 mask, uint32 val, uint8 priority);
extern uint32 sb_gpioout(sb_t *sbh, uint32 mask, uint32 val, uint8 priority);
extern uint32 sb_gpioin(sb_t *sbh);
extern uint32 sb_gpiointpolarity(sb_t *sbh, uint32 mask, uint32 val, uint8 priority);
extern uint32 sb_gpiointmask(sb_t *sbh, uint32 mask, uint32 val, uint8 priority);
extern uint32 sb_gpioled(sb_t *sbh, uint32 mask, uint32 val);
extern uint32 sb_gpioreserve(sb_t *sbh, uint32 gpio_num, uint8 priority);
extern uint32 sb_gpiorelease(sb_t *sbh, uint32 gpio_num, uint8 priority);
extern uint32 sb_gpiopull(sb_t *sbh, bool updown, uint32 mask, uint32 val);
extern uint32 sb_gpioevent(sb_t *sbh, uint regtype, uint32 mask, uint32 val);
extern uint32 sb_gpio_int_enable(sb_t *sbh, bool enable);

/* GPIO event handlers */
typedef void (*gpio_handler_t)(uint32 stat, void *arg);

extern void *sb_gpio_handler_register(sb_t *sbh, uint32 event,
	bool level, gpio_handler_t cb, void *arg);
extern void sb_gpio_handler_unregister(sb_t *sbh, void* gpioh);
extern void sb_gpio_handler_process(sb_t *sbh);

extern void sb_clkctl_init(sb_t *sbh);
extern uint16 sb_clkctl_fast_pwrup_delay(sb_t *sbh);
extern bool sb_clkctl_clk(sb_t *sbh, uint mode);
extern int sb_clkctl_xtal(sb_t *sbh, uint what, bool on);
extern void sb_register_intr_callback(sb_t *sbh, void *intrsoff_fn, void *intrsrestore_fn,
                                      void *intrsenabled_fn, void *intr_arg);
extern void sb_deregister_intr_callback(sb_t *sbh);
extern uint32 sb_set_initiator_to(sb_t *sbh, uint32 to, uint idx);
extern uint16 sb_d11_devid(sb_t *sbh);
extern int sb_corepciid(sb_t *sbh, uint func, uint16 *pcivendor, uint16 *pcidevice,
                        uint8 *pciclass, uint8 *pcisubclass, uint8 *pciprogif,
                        uint8 *pciheader);
extern uint sb_pcie_readreg(void *sbh, void* arg1, uint offset);
extern uint sb_pcie_writereg(sb_t *sbh, void *arg1,  uint offset, uint val);
extern uint32 sb_gpiotimerval(sb_t *sbh, uint32 mask, uint32 val);
extern bool sb_backplane64(sb_t *sbh);
extern void sb_btcgpiowar(sb_t *sbh);


#if defined(BCMDBG_ASSERT)
extern bool sb_taclear(sb_t *sbh);
#endif 

#ifdef BCMDBG
extern void sb_dump(sb_t *sbh, struct bcmstrbuf *b);
extern void sb_dumpregs(sb_t *sbh, struct bcmstrbuf *b);
extern void sb_view(sb_t *sbh);
extern void sb_viewall(sb_t *sbh);
extern void sb_clkctl_dump(sb_t *sbh, struct bcmstrbuf *b);
extern uint8 sb_pcieL1plldown(sb_t *sbh);
extern uint32 sb_pcielcreg(sb_t *sbh, uint32 mask, uint32 val);
#endif

extern bool sb_deviceremoved(sb_t *sbh);
extern uint32 sb_socram_size(sb_t *sbh);

/*
* Build device path. Path size must be >= SB_DEVPATH_BUFSZ.
* The returned path is NULL terminated and has trailing '/'.
* Return 0 on success, nonzero otherwise.
*/
extern int sb_devpath(sb_t *sbh, char *path, int size);
/* Read variable with prepending the devpath to the name */
extern char *sb_getdevpathvar(sb_t *sbh, const char *name);
extern int sb_getdevpathintvar(sb_t *sbh, const char *name);

extern uint8 sb_pcieclkreq(sb_t *sbh, uint32 mask, uint32 val);
extern void sb_war42780_clkreq(sb_t *sbh, bool clkreq);
extern void sb_pci_sleep(sb_t *sbh);
extern void sb_pci_down(sb_t *sbh);
extern void sb_pci_up(sb_t *sbh);

/* Wake-on-wireless-LAN (WOWL) */
extern bool sb_pci_pmecap(sb_t *sbh);
extern bool sb_pci_pmeclr(sb_t *sbh);
extern void sb_pci_pmeen(sb_t *sbh);

/* clkctl xtal what flags */
#define	XTAL			0x1		/* primary crystal oscillator (2050) */
#define	PLL			0x2		/* main chip pll */

/* clkctl clk mode */
#define	CLK_FAST		0		/* force fast (pll) clock */
#define	CLK_DYNAMIC		2		/* enable dynamic clock control */


/* GPIO usage priorities */
#define GPIO_DRV_PRIORITY	0		/* Driver */
#define GPIO_APP_PRIORITY	1		/* Application */
#define GPIO_HI_PRIORITY	2		/* Highest priority. Ignore GPIO reservation */

/* GPIO pull up/down */
#define GPIO_PULLUP		0
#define GPIO_PULLDN		1

/* GPIO event regtype */
#define GPIO_REGEVT			0	/* GPIO register event */
#define GPIO_REGEVT_INTMSK		1	/* GPIO register event int mask */
#define GPIO_REGEVT_INTPOL		2	/* GPIO register event int polarity */

/* device path */
#define SB_DEVPATH_BUFSZ	16		/* min buffer size in bytes */

#endif	/* _sbutils_h_ */
