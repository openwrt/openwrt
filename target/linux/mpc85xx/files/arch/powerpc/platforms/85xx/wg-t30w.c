// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * WatchGuard T30-W board Board Setup
 *
 * Copyright (C) 2021 Damien Mascord <tusker@tusker.org>
 *
 * Based on:
 *   mpc85xx_rdb.c:
 *      MPC85xx RDB Board Setup
 *      Copyright 2013 Freescale Semiconductor Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/stddef.h>

#include <asm/time.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <mm/mmu_decl.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <asm/mpic.h>
#include <linux/of.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>
#include "smp.h"

#include "mpc85xx.h"

static void __init wg_t30w_pic_init(void)
{
	struct mpic *mpic;

	mpic = mpic_alloc(NULL, 0,
	  MPIC_BIG_ENDIAN |
	  MPIC_SINGLE_DEST_CPU,
	  0, 256, " OpenPIC  ");

	BUG_ON(mpic == NULL);
	mpic_init(mpic);
}

/*
 * Setup the architecture
 */
static void __init wg_t30w_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("wg_t30w_setup_arch()", 0);

	mpc85xx_smp_init();

	fsl_pci_assign_primary();

	pr_info("T30-W board from WatchGuard\n");
}

machine_arch_initcall(wg_t30w, mpc85xx_common_publish_devices);

define_machine(wg_t30w) {
	.name			= "P1020 RDB",
	.compatible		= "watchguard,t30-w",
	.setup_arch		= wg_t30w_setup_arch,
	.init_IRQ		= wg_t30w_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
	.pcibios_fixup_phb      = fsl_pcibios_fixup_phb,
#endif
	.get_irq		= mpic_get_irq,
	.calibrate_decr		= generic_calibrate_decr,
#ifdef DEBUG
	.progress	        = udbg_progress,
#endif
};
