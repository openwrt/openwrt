/*
 *  Generic AR71xx machine support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include "machtype.h"

static void __init ar71xx_generic_init(void)
{
	/* Nothing to do */
}

MIPS_MACHINE(AR71XX_MACH_GENERIC, "Generic AR71xx board", ar71xx_generic_init);
