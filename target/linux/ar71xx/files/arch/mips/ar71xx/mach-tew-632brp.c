/*
 *  TrendNET TEW-632BRP board support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

static void __init tew_632brp_setup(void)
{
}

MIPS_MACHINE(AR71XX_MACH_TEW_632BRP, "TRENDnet TEW-632BRP", tew_632brp_setup);
