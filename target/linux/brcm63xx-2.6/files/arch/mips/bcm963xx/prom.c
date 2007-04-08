/*
<:copyright-gpl
 Copyright 2004 Broadcom Corp. All Rights Reserved.

 This program is free software; you can distribute it and/or modify it
 under the terms of the GNU General Public License (Version 2) as
 published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
:>
*/
/*
 * prom.c: PROM library initialization code.
 *
 */
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/bootmem.h>
#include <linux/blkdev.h>

#include <asm/addrspace.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>
#include <asm/time.h>
#include <asm/mach-bcm963xx/bootloaders.h>
#include <asm/mach-bcm963xx/6348_map_part.h>

#include "../cfe/cfe_private.h"

extern void __init detect_bootloader(void); 
extern void serial_init(void);
extern int boot_loader_type;

#define MACH_BCM                    MACH_BCM96348

const char *get_system_type(void)
{
	return "Broadcom BCM963xx";
}

void __init prom_init(void)
{
    	serial_init();

    	printk( "%s prom init\n", get_system_type() );

    	PERF->IrqMask = 0;

	detect_bootloader();

	if (boot_loader_type == BOOT_LOADER_CFE) {
		cfe_setup(fw_arg0, fw_arg1, fw_arg2, fw_arg3);
		add_memory_region(0, (boot_mem_map.map[0].size - ADSL_SDRAM_IMAGE_SIZE), BOOT_MEM_RAM);
	}
	else
		add_memory_region(0, (0x01000000 - ADSL_SDRAM_IMAGE_SIZE), BOOT_MEM_RAM);
	
	mips_machgroup = MACH_GROUP_BRCM;
	mips_machtype = MACH_BCM;
}

void __init prom_free_prom_memory(void)
{
	/* We do not have any memory to free */
}
