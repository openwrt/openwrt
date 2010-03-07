/*
 *  MikroTik RouterBOARD 750 support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "dev-ap91-eth.h"

static void __init rb750_setup(void)
{
	ap91_eth_init(NULL);
}

MIPS_MACHINE(AR71XX_MACH_RB_750, "750i", "MikroTik RouterBOARD 750",
	     rb750_setup);
