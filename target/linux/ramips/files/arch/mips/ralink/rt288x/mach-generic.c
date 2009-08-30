/*
 *  Generic RT288x machine setup
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>

#include <asm/mips_machine.h>

static void __init rt288x_generic_init(void)
{
}

MIPS_MACHINE(RT288X_MACH_GENERIC, "Generic RT288x board", rt288x_generic_init);
