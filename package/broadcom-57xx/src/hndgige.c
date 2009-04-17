/*
 * HND SiliconBackplane Gigabit Ethernet core software interface
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

#include <typedefs.h>
#include <osl.h>
#include <pcicfg.h>
#include <sbconfig.h>
#include <sbutils.h>
#include "sbgige.h"
#include <hndpci.h>
#include "hndgige.h"

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

/*
 * Setup the gige core.
 * Resetting the core will lose all settings.
 */
void
sb_gige_init(sb_t *sbh, uint32 unit, bool *rgmii)
{
	volatile pci_config_regs *pci;
	sbgige_pcishim_t *ocp;
	sbconfig_t *sb;
	osl_t *osh;
	uint32 statelow;
	uint32 statehigh;
	uint32 base;
	uint32 idx;
	void *regs;

	/* Sanity checks */
	ASSERT(sbh);
	ASSERT(rgmii);

	idx = sb_coreidx(sbh);

	/* point to the gige core registers */
	regs = sb_setcore(sbh, SB_GIGETH, unit);
	ASSERT(regs);

	osh = sb_osh(sbh);

	pci = &((sbgige_t *)regs)->pcicfg;
	ocp = &((sbgige_t *)regs)->pcishim;
	sb = &((sbgige_t *)regs)->sbconfig;

	/* Enable the core clock and memory access */
	if (!sb_iscoreup(sbh))
		sb_core_reset(sbh, 0, 0);

	/*
	 * Setup the 64K memory-mapped region base address through BAR0.
	 * Leave the other BAR values alone.
	 */
	base = sb_base(R_REG(osh, &sb->sbadmatch1));
	W_REG(osh, &pci->base[0], base);
	W_REG(osh, &pci->base[1], 0);

	/*
	 * Enable the PCI memory access anyway. Any PCI config commands
	 * issued before the core is enabled will go to the emulation
	 * only and will not go to the real PCI config registers.
	 */
	OR_REG(osh, &pci->command, 2);

	/*
	 * Enable the posted write flush scheme as follows:
	 *
	 * - Enable flush on any core register read
	 * - Enable timeout on the flush
	 * - Disable the interrupt mask when flushing
	 *
	 * This differs from the default setting only in that interrupts are
	 * not masked.  Since posted writes are not flushed on interrupt, the
	 * driver must explicitly request a flush in its interrupt handling
	 * by reading a core register.
	 */
	W_REG(osh, &ocp->FlushStatusControl, 0x68);

	/*
	 * Determine whether the GbE is in GMII or RGMII mode.  This is
	 * indicated in bit 16 of the SBTMStateHigh register, which is
	 * part of the core-specific flags field.
	 *
	 * For GMII, bypass the Rx/Tx DLLs, i.e. add no delay to RXC/GTXC
	 * within the core.  For RGMII, do not bypass the DLLs, resulting
	 * in added delay for RXC/GTXC.  The SBTMStateLow register contains
	 * the controls for doing this in the core-specific flags field:
	 *
	 *   bit 24 - Enable DLL controls
	 *   bit 20 - Bypass Rx DLL
	 *   bit 19 - Bypass Tx DLL
	 */
	statelow = R_REG(osh, &sb->sbtmstatelow);	/* DLL controls */
	statehigh = R_REG(osh, &sb->sbtmstatehigh);	/* GMII/RGMII mode */
	if ((statehigh & (1 << 16)) != 0)	/* RGMII */
	{
		statelow &= ~(1 << 20);		/* no Rx bypass (delay) */
		statelow &= ~(1 << 19);		/* no Tx bypass (delay) */
		*rgmii = TRUE;
	}
	else					/* GMII */
	{
		statelow |= (1 << 20);		/* Rx bypass (no delay) */
		statelow |= (1 << 19);		/* Tx bypass (no delay) */
		*rgmii = FALSE;
	}
	statelow |= (1 << 24);			/* enable DLL controls */
	W_REG(osh, &sb->sbtmstatelow, statelow);

	sb_setcoreidx(sbh, idx);
}
