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

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-ralink/common.h>
#include <ralink_soc.h>

static void __init detect_mem_size(void)
{
	unsigned long size;

	for (size = RALINK_SOC_MEM_SIZE_MIN; size < RALINK_SOC_MEM_SIZE_MAX;
	     size <<= 1 ) {
		if (!memcmp(detect_mem_size,
			    detect_mem_size + size, 1024))
			break;
	}

	add_memory_region(RALINK_SOC_SDRAM_BASE, size, BOOT_MEM_RAM);
}

void __init plat_mem_setup(void)
{
	set_io_port_base(KSEG1);

	detect_mem_size();
	ramips_soc_setup();
}
