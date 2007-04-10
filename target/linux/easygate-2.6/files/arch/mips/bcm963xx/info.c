/*
 * $Id$
 *
 * Copyright (C) 2007 OpenWrt.org
 * Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 * Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/types.h>
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/string.h>
#include <asm/mach-bcm963xx/bootloaders.h>

static char *boot_loader_names[BOOT_LOADER_LAST+1] = {
        [BOOT_LOADER_UNKNOWN]   = "Unknown",
        [BOOT_LOADER_CFE]       = "CFE",
        [BOOT_LOADER_REDBOOT]	= "RedBoot",
	[BOOT_LOADER_CFE2]	= "CFEv2"
};

/* boot loaders specific definitions */
#define CFE_EPTSEAL	0x43464531 /* CFE1 is the magic number to recognize CFE from other bootloaders */

int boot_loader_type;
/*
 * Boot loader detection routines
 */
static int __init detect_cfe(void)
{
	/*
	 * This method only works, when we are booted directly from the CFE.
	 */
	uint32_t cfe_handle = (uint32_t) fw_arg0;
	uint32_t cfe_a1_val = (uint32_t) fw_arg1;
	uint32_t cfe_entry = (uint32_t) fw_arg2;
	uint32_t cfe_seal = (uint32_t) fw_arg3;

	/* Check for CFE by finding the CFE magic number */
	if (cfe_seal != CFE_EPTSEAL)
		/* We are not booted from CFE */
		return 0;

	/* cfe_a1_val must be 0, because only one CPU present in the ADM5120 SoC */
	if (cfe_a1_val != 0)
		return 0;

	/* The cfe_handle, and the cfe_entry must be kernel mode addresses */
	if ((cfe_handle < KSEG0) || (cfe_entry < KSEG0))
		return 0;

	return 1;
}

static int __init detect_redboot(void)
{
	/* On Inventel Livebox, the boot loader is passed as a command line argument, check for it */
	if (!strncmp(arcs_cmdline, "boot_loader=RedBoot", 19))
		return 1;
	return 0;
}

void __init detect_bootloader(void)
{
	if (detect_cfe()) {
		boot_loader_type = BOOT_LOADER_CFE;
	}

	if (detect_redboot()) {
		boot_loader_type = BOOT_LOADER_REDBOOT;
	}
	else {
		/* Some devices are using CFE, but it is not detected as is */
		boot_loader_type = BOOT_LOADER_CFE2;
	}
	printk("Boot loader is : %s\n", boot_loader_names[boot_loader_type]);
}

void __init detect_board(void)
{
	switch (boot_loader_type)
	{
		case BOOT_LOADER_CFE:
			break;
		case BOOT_LOADER_REDBOOT:
			break;
		default:
			break;
	}
}

EXPORT_SYMBOL(boot_loader_type);
