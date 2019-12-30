// SPDX-License-Identifier: GPL-2.0+

/*
 * Edgecore ecw7210-l Board Setup
 *
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>

#include <asm/time.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <mm/mmu_decl.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <asm/mpic.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>
#include "smp.h"

#include "mpc85xx.h"

void __init ecw7210_l_pic_init(void)
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
static void __init ecw7210_l_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("ecw7210_l_setup_arch()", 0);

	mpc85xx_smp_init();

	fsl_pci_assign_primary();

	printk(KERN_INFO "ECW7210-L board from Edgecore\n");
}

machine_arch_initcall(ecw7210_l, mpc85xx_common_publish_devices);

/*
 * Called very early, device-tree isn't unflattened
 */
static int __init ecw7210_l_probe(void)
{
	if (of_machine_is_compatible("edgecore,ecw7210-l"))
		return 1;
	return 0;
}

define_machine(ecw7210_l) {
	.name			= "Edgecore ECW7210-L",
	.probe			= ecw7210_l_probe,
	.setup_arch		= ecw7210_l_setup_arch,
	.init_IRQ		= ecw7210_l_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
	.pcibios_fixup_phb      = fsl_pcibios_fixup_phb,
#endif
	.get_irq		= mpic_get_irq,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};
