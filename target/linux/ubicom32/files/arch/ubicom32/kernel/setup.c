/*
 * arch/ubicom32/kernel/setup.c
 *   Ubicom32 architecture-dependent parts of system setup.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1999-2007  Greg Ungerer (gerg@snapgear.com)
 * Copyright (C) 1998,1999  D. Jeff Dionne <jeff@uClinux.org>
 * Copyleft  ()) 2000       James D. Schettine {james@telos-systems.com}
 * Copyright (C) 1998       Kenneth Albanowski <kjahds@kjahds.com>
 * Copyright (C) 1995       Hamish Macdonald
 * Copyright (C) 2000       Lineo Inc. (www.lineo.com)
 * Copyright (C) 2001	    Lineo, Inc. <www.lineo.com>
 * 68VZ328 Fixes/support    Evan Stawnyczy <e@lineo.ca>
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <linux/seq_file.h>
#include <linux/init.h>

#include <asm/devtree.h>
#include <asm/setup.h>
#include <asm/irq.h>
#include <asm/machdep.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/ubicom32-common.h>
#include <asm/processor.h>
#include <asm/bootargs.h>
#include <asm/thread.h>

unsigned long memory_start;
EXPORT_SYMBOL(memory_start);

unsigned long memory_end;
EXPORT_SYMBOL(memory_end);

static char __initdata command_line[COMMAND_LINE_SIZE];
#ifdef CONFIG_CMDLINE_BOOL
static char __initdata builtin_cmdline[COMMAND_LINE_SIZE] = CONFIG_CMDLINE;
#endif

extern int _stext, _etext, _sdata, _edata, _sbss, _ebss, _end;

/*
 * setup_arch()
 *	Setup the architecture dependent portions of the system.
 */
void __init setup_arch(char **cmdline_p)
{
	int bootmap_size;
	unsigned long ram_start;

	processor_init();
	bootargs_init();

	/*
	 * Use the link for memory_start from the link and the processor
	 * node for memory_end.
	 */
	memory_start = PAGE_ALIGN(((unsigned long)&_end));
	processor_dram(&ram_start, &memory_end);

	init_mm.start_code = (unsigned long) &_stext;
	init_mm.end_code = (unsigned long) &_etext;
	init_mm.end_data = (unsigned long) &_edata;
	init_mm.brk = (unsigned long) 0;

	/*
	 * bootexec copies the original default command line to end of memory.
	 * u-boot can modify it there (i.e. to enable network boot) and the
	 * kernel picks up the modified version.
	 *
	 * mainexec creates a `new default' command_line which is in the
	 * bootargs devnode. It is updated on every firmware update but
	 * not used at the moment.
	 */
	strlcpy(boot_command_line, (char *)(memory_end - COMMAND_LINE_SIZE), COMMAND_LINE_SIZE);

#ifdef CONFIG_CMDLINE_BOOL
#ifdef CONFIG_CMDLINE_OVERRIDE
	strlcpy(boot_command_line, builtin_cmdline, COMMAND_LINE_SIZE);
#else
	if (builtin_cmdline[0]) {
		/* append boot loader cmdline to builtin */
		strlcat(builtin_cmdline, " ", COMMAND_LINE_SIZE);
		strlcat(builtin_cmdline, boot_command_line, COMMAND_LINE_SIZE);
		strlcpy(boot_command_line, builtin_cmdline, COMMAND_LINE_SIZE);
	}
#endif
#endif

	strlcpy(command_line, boot_command_line, COMMAND_LINE_SIZE);
	*cmdline_p = command_line;

	parse_early_param();

	printk(KERN_INFO "%s Processor, Ubicom, Inc. <www.ubicom.com>\n", CPU);

#if defined(DEBUG)
	printk(KERN_DEBUG "KERNEL -> TEXT=0x%06x-0x%06x DATA=0x%06x-0x%06x "
		"BSS=0x%06x-0x%06x\n", (int) &_stext, (int) &_etext,
		(int) &_sdata, (int) &_edata,
		(int) &_sbss, (int) &_ebss);
	printk(KERN_DEBUG "MEMORY -> ROMFS=0x%06x-0x%06x MEM=0x%06x-0x%06x\n ",
		(int) &_ebss, (int) memory_start,
		(int) memory_start, (int) memory_end);
#endif

#ifdef DEBUG
	if (strlen(*cmdline_p))
		printk(KERN_DEBUG "Command line: '%s'\n", *cmdline_p);
#endif

#if defined(CONFIG_FRAMEBUFFER_CONSOLE) && defined(CONFIG_DUMMY_CONSOLE)
	conswitchp = &dummy_con;
#endif

	/*
	 * If we have a device tree, see if we have the nodes we need.
	 */
	if (devtree) {
		devtree_print();
	}

	/*
	 * From the arm initialization comment:
	 *
	 * This doesn't seem to be used by the Linux memory manager any
	 * more, but is used by ll_rw_block.  If we can get rid of it, we
	 * also get rid of some of the stuff above as well.
	 *
	 * Note: max_low_pfn and max_pfn reflect the number of _pages_ in
	 * the system, not the maximum PFN.
	 */
	max_pfn = max_low_pfn = (memory_end - PAGE_OFFSET) >> PAGE_SHIFT;

	/*
	 * Give all the memory to the bootmap allocator, tell it to put the
	 * boot mem_map at the start of memory.
	 */
	bootmap_size = init_bootmem_node(
			NODE_DATA(0),
			memory_start >> PAGE_SHIFT,	/* map goes here */
			PAGE_OFFSET >> PAGE_SHIFT,	/* 0 on coldfire */
			memory_end >> PAGE_SHIFT);
	/*
	 * Free the usable memory, we have to make sure we do not free
	 * the bootmem bitmap so we then reserve it after freeing it :-)
	 */
	free_bootmem(memory_start, memory_end - memory_start);
	reserve_bootmem(memory_start, bootmap_size, BOOTMEM_DEFAULT);

	/*
	 * Get kmalloc into gear.
	 */
	paging_init();

	/*
	 * Fix up the thread_info structure, indicate this is a mainline Linux
	 * thread and setup the sw_ksp().
	 */
	sw_ksp[thread_get_self()] = (unsigned int) current_thread_info();
	thread_set_mainline(thread_get_self());
}
