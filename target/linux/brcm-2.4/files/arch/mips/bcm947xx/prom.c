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
	unsigned long max;

        mips_machgroup = MACH_GROUP_BRCM;
        mips_machtype = MACH_BCM947XX;

	/* Figure out memory size by finding aliases
	 * 
	 * BCM47XX uses 128MB for addressing the ram, if the system contains
	 * less that that amount of ram it remaps the ram more often into the
	 * available space.
	 * Accessing memory after 128MB will cause an exception.
	 * max contains the biggest possible address supported by the platform.
	 * If the method wants to try something above we assume 128MB ram.
 	 */
	max = ((unsigned long)(prom_init) | ((128 << 20) - 1));
	for (mem = (1 << 20); mem < (128 << 20); mem += (1 << 20)) {
		if (((unsigned long)(prom_init) + mem) > max) {
			mem = (128 << 20);
			printk("assume 128MB RAM\n");
			break;
		}
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
