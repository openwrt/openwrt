/*
 * Misc utility routines for accessing chip-specific features
 * of Broadcom HNBU SiliconBackplane-based chips.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: sbutils.h,v 1.4 2006/04/08 07:12:42 honor Exp $
 */

#ifndef	_sbutils_h_
#define	_sbutils_h_

/* 
 * Datastructure to export all chip specific common variables
 * public (read-only) portion of sbutils handle returned by
 * sb_attach()/sb_kattach()
*/

struct sb_pub {

	uint	bustype;		/* SB_BUS, PCI_BUS  */
	uint	buscoretype;		/* SB_PCI, SB_PCMCIA, SB_PCIE */
	uint	buscorerev;		/* buscore rev */
	uint	buscoreidx;		/* buscore index */
	int	ccrev;			/* chip common core rev */
	uint	boardtype;		/* board type */
	uint	boardvendor;		/* board vendor */
	uint	chip;			/* chip number */
	uint	chiprev;		/* chip revision */
	uint	chippkg;		/* chip package option */
	uint    sonicsrev;		/* sonics backplane rev */
};

typedef const struct sb_pub  sb_t;

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
extern sb_t *sb_kattach(void);
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
extern uint sb_coreidx(sb_t *sbh);
extern uint sb_coreunit(sb_t *sbh);
extern uint sb_corevendor(sb_t *sbh);
extern uint sb_corerev(sb_t *sbh);
extern void *sb_osh(sb_t *sbh);
extern void sb_setosh(sb_t *sbh, osl_t *osh);
extern void *sb_coreregs(sb_t *sbh);
extern uint32 sb_coreflags(sb_t *sbh, uint32 mask, uint32 val);
extern uint32 sb_coreflagshi(sb_t *sbh, uint32 mask, uint32 val);
extern bool sb_iscoreup(sb_t *sbh);
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

extern void sb_clkctl_init(sb_t *sbh);
extern uint16 sb_clkctl_fast_pwrup_delay(sb_t *sbh);
extern bool sb_clkctl_clk(sb_t *sbh, uint mode);
extern int sb_clkctl_xtal(sb_t *sbh, uint what, bool on);
extern void sb_register_intr_callback(sb_t *sbh, void *intrsoff_fn, void *intrsrestore_fn,
                                      void *intrsenabled_fn, void *intr_arg);
extern uint32 sb_set_initiator_to(sb_t *sbh, uint32 to);
extern int sb_corepciid(sb_t *sbh, uint func, uint16 *pcivendor, uint16 *pcidevice,
                        uint8 *pciclass, uint8 *pcisubclass, uint8 *pciprogif,
                        uint8 *pciheader);
extern uint sb_pcie_readreg(void *sbh, void* arg1, uint offset);
extern uint sb_pcie_writereg(sb_t *sbh, void *arg1,  uint offset, uint val);
extern uint32 sb_gpiotimerval(sb_t *sbh, uint32 mask, uint32 val);
extern bool sb_backplane64(sb_t *sbh);
extern void sb_btcgpiowar(sb_t *sbh);




extern bool sb_deviceremoved(sb_t *sbh);
extern uint32 sb_socram_size(sb_t *sbh);

/*
* Build device path. Path size must be >= SB_DEVPATH_BUFSZ.
* The returned path is NULL terminated and has trailing '/'.
* Return 0 on success, nonzero otherwise.
*/
extern int sb_devpath(sb_t *sbh, char *path, int size);

/* clkctl xtal what flags */
#define	XTAL			0x1		/* primary crystal oscillator (2050) */
#define	PLL			0x2		/* main chip pll */

/* clkctl clk mode */
#define	CLK_FAST		0		/* force fast (pll) clock */
#define	CLK_DYNAMIC		2		/* enable dynamic clock control */


/* GPIO usage priorities */
#define GPIO_DRV_PRIORITY	0		/* Driver */
#define GPIO_APP_PRIORITY	1		/* Application */
#define GPIO_HI_PRIORITY   2		/* Highest priority. Ignore GPIO reservation */

/* device path */
#define SB_DEVPATH_BUFSZ	16		/* min buffer size in bytes */

#endif	/* _sbutils_h_ */
