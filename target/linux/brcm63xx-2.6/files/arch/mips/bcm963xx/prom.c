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

#include <bcm_map_part.h>
#include <board.h>
#include "boardparms.h"
#include "softdsl/AdslCoreDefs.h"


//char arcs_cmdline[CL_SIZE] __initdata = {0};
/* inv_xde */
int boot_loader_type;
int prom_argc;
char **prom_argv, **prom_envp;

extern int  do_syslog(int, char *, int);
extern void serial_init(void);
extern void __init InitNvramInfo( void );
extern void kerSysFlashInit( void );
extern unsigned long get_nvram_start_addr(void);
void __init create_root_nfs_cmdline( char *cmdline );

#define MACH_BCM                    MACH_BCM96348

const char *get_system_type(void)
{
    /*PNVRAM_DATA pNvramData = (PNVRAM_DATA) get_nvram_start_addr();

    return( pNvramData->szBoardId );*/
    return "brcm63xx";
}

unsigned long getMemorySize(void)
{
    unsigned long ulSdramType = BOARD_SDRAM_TYPE;

    unsigned long ulSdramSize;

    switch( ulSdramType )
    {
    case BP_MEMORY_16MB_1_CHIP:
    case BP_MEMORY_16MB_2_CHIP:
        ulSdramSize = 16 * 1024 * 1024;
        break;
    case BP_MEMORY_32MB_1_CHIP:
    case BP_MEMORY_32MB_2_CHIP:
        ulSdramSize = 32 * 1024 * 1024;
        break;
    case BP_MEMORY_64MB_2_CHIP:
        ulSdramSize = 64 * 1024 * 1024;
        break;
    default:
        ulSdramSize = 8 * 1024 * 1024;
        break;
    }
    if (boot_loader_type == BOOT_CFE)
      return ulSdramSize;
    else
      // assume that there is one contiguous memory map
      return boot_mem_map.map[0].size;
}

/* --------------------------------------------------------------------------
    Name: prom_init
 -------------------------------------------------------------------------- */
void __init prom_init(void)
{
    extern ulong r4k_interval;

    serial_init();

    prom_argc = fw_arg0;
    prom_argv = (char **) fw_arg1;
    prom_envp = (char **) fw_arg2;

    if ((prom_argv > 0x80000000) && (prom_argv < 0x82000000)) {
      strncpy(arcs_cmdline, prom_argv[1], CL_SIZE);
    }

    if (strncmp(arcs_cmdline, "boot_loader=RedBoot", 19) != 0) {
      boot_loader_type =  BOOT_CFE;
    }
    else {
      boot_loader_type = BOOT_REDBOOT;
    }

    do_syslog(8, NULL, 8);

    printk( "%s prom init\n", get_system_type() );

    PERF->IrqMask = 0;

    arcs_cmdline[0] = '\0';

       if (boot_loader_type == BOOT_CFE)
      add_memory_region(0, (getMemorySize() - ADSL_SDRAM_IMAGE_SIZE), BOOT_MEM_RAM);
    else
       add_memory_region(0, (0x01000000 - ADSL_SDRAM_IMAGE_SIZE), BOOT_MEM_RAM);

    mips_machgroup = MACH_GROUP_BRCM;
    mips_machtype = MACH_BCM;

       BpSetBoardId("96348GW-10");
}

/* --------------------------------------------------------------------------
    Name: prom_free_prom_memory
Abstract:
 -------------------------------------------------------------------------- */
void __init prom_free_prom_memory(void)
{

}

