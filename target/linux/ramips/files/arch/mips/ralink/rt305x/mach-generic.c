/*
 *  Generic RT305x machine setup
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>

#include <asm/mips_machine.h>

#include "machine.h"

static void __init rt305x_generic_init(void)
{
}

MIPS_MACHINE(RT305X_MACH_GENERIC, "Generic RT305x board", rt305x_generic_init);
