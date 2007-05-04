/*
 * Early initialization code for BCM94710 boards
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: prom.c,v 1.1 2005/03/16 13:49:59 wbx Exp $
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <asm/bootinfo.h>

void __init
prom_init(int argc, const char **argv)
{
	unsigned long mem;

        mips_machgroup = MACH_GROUP_BRCM;
        mips_machtype = MACH_BCM947XX;

	/* Figure out memory size by finding aliases */
	for (mem = (1 << 20); mem < (128 << 20); mem += (1 << 20)) {
		if (*(unsigned long *)((unsigned long)(prom_init) + mem) == 
		    *(unsigned long *)(prom_init))
			break;
	}
	add_memory_region(0, mem, BOOT_MEM_RAM);
}

void __init
prom_free_prom_memory(void)
{
}
