/*
 * Copyright (C) 2007 OpenWrt.org
 * Copyright (C) Gabor Juhos
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

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <adm5120_info.h>
#include <myloader.h>

/* boot loaders specific definitions */
#define CFE_EPTSEAL	0x43464531 /* CFE1 is the magic number to recognize CFE from other bootloaders */


struct adm5120_info adm5120_info = {
	.cpu_speed		= CPU_SPEED_175,
	.cpu_package		= CPU_PACKAGE_PQFP,
	.boot_loader		= BOOT_LOADER_UNKNOWN,
	.board_type		= BOARD_TYPE_UNKNOWN
};

static char *boot_loader_names[BOOT_LOADER_LAST+1] = {
	[BOOT_LOADER_UNKNOWN]	= "Unknown",
	[BOOT_LOADER_CFE]	= "CFE",
	[BOOT_LOADER_UBOOT]	= "U-Boot",
	[BOOT_LOADER_MYLOADER]	= "MyLoader"
};

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
	if (cfe_seal != CFE_EPTSEAL) {
		/* We are not booted from CFE */
		return 0;
	}
	
	/* cfe_a1_val must be 0, because only one CPU present in the ADM5120 SoC */
	if (cfe_a1_val != 0) {
		return 0;
	}
	
	/* The cfe_handle, and the cfe_entry must be kernel mode addresses */
	if ((cfe_handle < KSEG0) || (cfe_entry < KSEG0)) {
		return 0;
	}

	return 1;
}

static int __init detect_uboot(void)
{
	/* FIXME: not yet implemented */
	return 0;
}

static int __init detect_myloader(void)
{
	struct mylo_system_params *sysp;
	struct mylo_board_params *boardp;
	struct mylo_partition_table *parts;

	sysp = (struct mylo_system_params *)(MYLO_MIPS_SYS_PARAMS);
	boardp = (struct mylo_board_params *)(MYLO_MIPS_BOARD_PARAMS);
	parts = (struct mylo_partition_table *)(MYLO_MIPS_PARTITIONS);

	/* Check for some magic numbers */
	if ((sysp->magic != MYLO_MAGIC_SYS_PARAMS) || 
	   (boardp->magic != MYLO_MAGIC_BOARD_PARAMS) ||
	   (parts->magic != MYLO_MAGIC_PARTITIONS))
		return 0;

	return 1;
}

static int __init detect_bootloader(void)
{
	if (detect_cfe())
		return BOOT_LOADER_CFE;
	
	if (detect_uboot()) 
		return BOOT_LOADER_UBOOT;
	
	if (detect_myloader())
		return BOOT_LOADER_MYLOADER;
	
	return BOOT_LOADER_UNKNOWN;
}

void __init adm5120_info_show(void)
{
	printk("adm5120: boot loader is %s\n", boot_loader_names[adm5120_info.boot_loader]);
}

void __init adm5120_info_init(void)
{
	adm5120_info.boot_loader = detect_bootloader();
	
	adm5120_info_show();
}
