/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright MontaVista Software Inc
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

/*
 * Prom setup file for ar531x
 */

#include <linux/init.h>
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include "ar531x.h"

void __init prom_init(void)
{
	u32 memsize, memcfg;

	mips_machgroup = MACH_GROUP_ATHEROS;
	mips_machtype = -1;

	/*                                                                             
	 * Atheros CPUs before the AR2315 are using MIPS 4Kc core, later designs are
	 * using MIPS 4KEc R2 core. This makes it easy to determine the board at runtime.
	 */
                                                                            
	if (current_cpu_data.cputype == CPU_4KEC) {
		mips_machtype = MACH_ATHEROS_AR5315;
		
		memcfg = sysRegRead(AR5315_MEM_CFG);
		memsize   = 1 + ((memcfg & SDRAM_DATA_WIDTH_M) >> SDRAM_DATA_WIDTH_S);
		memsize <<= 1 + ((memcfg & SDRAM_COL_WIDTH_M) >> SDRAM_COL_WIDTH_S);
		memsize <<= 1 + ((memcfg & SDRAM_ROW_WIDTH_M) >> SDRAM_ROW_WIDTH_S);
		memsize <<= 3;
	} else {
		int bank0AC, bank1AC;

		mips_machtype = MACH_ATHEROS_AR5312;

		memcfg = sysRegRead(AR531X_MEM_CFG1);
		bank0AC = (memcfg & MEM_CFG1_AC0) >> MEM_CFG1_AC0_S;
		bank1AC = (memcfg & MEM_CFG1_AC1) >> MEM_CFG1_AC1_S;
		memsize = (bank0AC ? (1 << (bank0AC+1)) : 0)
		        + (bank1AC ? (1 << (bank1AC+1)) : 0);
		memsize <<= 20;
	}

	add_memory_region(0, memsize, BOOT_MEM_RAM);
	strcpy(arcs_cmdline, "console=ttyS0,9600 rootfstype=squashfs,jffs2");
}

void __init prom_free_prom_memory(void)
{
}
