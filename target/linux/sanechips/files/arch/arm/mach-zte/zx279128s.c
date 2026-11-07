// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <asm/mach/arch.h>
#include <linux/init.h>

static const char *const zx279128s_dt_compat[] __initconst = {
	"zte,zx279128s",
	NULL,
};

DT_MACHINE_START(ZX279128S, "ZTE zx279128s (Device Tree)")
	.dt_compat	= zx279128s_dt_compat,
	/* Keep the L2 cache setup of the boot loader and the device tree */
	.l2c_aux_val	= 0,
	.l2c_aux_mask	= ~0,
MACHINE_END
