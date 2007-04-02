/*
 * $Id$
 *
 * Copyright (C) 2007 OpenWrt.org
 * Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
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
#include <adm5120_defs.h>
#include <adm5120_switch.h>
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
	[BOOT_LOADER_MYLOADER]	= "MyLoader",
	[BOOT_LOADER_ROUTERBOOT]= "RouterBOOT"
};

/*
 * CPU settings detection
 */
#define CODE_GET_PC(c)		((c) & CODE_PC_MASK)
#define CODE_GET_REV(c)		(((c) >> CODE_REV_SHIFT) & CODE_REV_MASK)
#define CODE_GET_PK(c)		(((c) >> CODE_PK_SHIFT) & CODE_PK_MASK)
#define CODE_GET_CLKS(c)	(((c) >> CODE_CLKS_SHIFT) & CODE_CLKS_MASK)
#define CODE_GET_NAB(c)		(((c) & CODE_NAB) != 0)

static void __init detect_cpu_info(void)
{
	uint32_t *reg;
	uint32_t code;
	uint32_t clks;

	reg = (uint32_t *)KSEG1ADDR(ADM5120_SWITCH_BASE+SWITCH_REG_CODE);
	code = *reg;

	clks = CODE_GET_CLKS(code);

	adm5120_info.product_code = CODE_GET_PC(code);
	adm5120_info.revision = CODE_GET_REV(code);

	adm5120_info.cpu_speed = CPU_SPEED_175;
	if (clks & 1)
		adm5120_info.cpu_speed += 25000000;
	if (clks & 2)
		adm5120_info.cpu_speed += 50000000;

	adm5120_info.cpu_package = (CODE_GET_PK(code) == CODE_PK_BGA) ?
		CPU_PACKAGE_BGA : CPU_PACKAGE_PQFP;

	adm5120_info.nand_boot = CODE_GET_NAB(code);

}

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

static int __init detect_routerboot(void)
{
	/* FIXME: not yet implemented */
	return 0;
}

static int __init detect_bootloader(void)
{
	if (detect_cfe())
		return BOOT_LOADER_CFE;

	if (detect_uboot())
		return BOOT_LOADER_UBOOT;

	if (detect_myloader())
		return BOOT_LOADER_MYLOADER;

	if (detect_routerboot())
		return BOOT_LOADER_ROUTERBOOT;

	return BOOT_LOADER_UNKNOWN;
}

/*
 * Board detection
 */
static void __init detect_board_type(void)
{
	/* FIXME: not yet implemented */
}

void __init adm5120_info_show(void)
{
	printk("ADM%04X%s revision %d, running at %ldMHz\n",
		adm5120_info.product_code,
		(adm5120_info.cpu_package == CPU_PACKAGE_BGA) ? "" : "P",
		adm5120_info.revision,
		(adm5120_info.cpu_speed / 1000000)
		);
	printk("Boot loader is: %s\n", boot_loader_names[adm5120_info.boot_loader]);
	printk("Booted from   : %s flash\n", adm5120_info.nand_boot ? "NAND" : "NOR");
}

void __init adm5120_info_init(void)
{
	detect_cpu_info();
	adm5120_info.boot_loader = detect_bootloader();
	detect_board_type();

	adm5120_info_show();
}
