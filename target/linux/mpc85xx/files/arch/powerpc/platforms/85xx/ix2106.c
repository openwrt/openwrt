// SPDX-License-Identifier: GPL-2.0-or-later
/* Minimal machine support for the NEC IX2106 (Freescale P1010). */

#include <linux/init.h>
#include <linux/panic.h>
#include <asm/machdep.h>
#include <asm/mpic.h>
#include <asm/prom.h>
#include <asm/udbg.h>

#include "mpc85xx.h"

static void __init ix2106_init_irq(void)
{
	struct mpic *mpic;

	mpic = mpic_alloc(NULL, 0, MPIC_BIG_ENDIAN | MPIC_SINGLE_DEST_CPU,
			  0, 256, " OpenPIC  ");
	if (!mpic)
		panic("Unable to allocate MPIC for NEC IX2106");
	mpic_init(mpic);
}

static int __init ix2106_probe(void)
{
	return of_machine_is_compatible("nec,ix2106");
}

machine_arch_initcall(ix2106, mpc85xx_common_publish_devices);

define_machine(ix2106) {
	.name		= "NEC IX2106",
	.probe		= ix2106_probe,
	.init_IRQ	= ix2106_init_irq,
	.get_irq	= mpic_get_irq,
	.progress	= udbg_progress,
};
