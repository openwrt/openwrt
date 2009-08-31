/*
 * Ralink SoC common setup
 *
 * Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include <asm/addrspace.h>
#include <asm/mach-ralink/common.h>

void __init plat_mem_setup(void)
{
	set_io_port_base(KSEG1);

	ramips_soc_setup();
}
