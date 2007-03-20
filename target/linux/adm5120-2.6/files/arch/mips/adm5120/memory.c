/*****************************************************************************
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 * Copyright (C) 2003 ADMtek Incorporated.
 *	daniell@admtek.com.tw
 * Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 *****************************************************************************/

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pfn.h>
#include <linux/string.h>

#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/sections.h>

#include <asm-mips/mips-boards/prom.h>

extern char *prom_getenv(char *envname);

#define PFN_ALIGN(x)    (((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)

#define ADM5120_MEMCTRL			0x1200001c
#define ADM5120_MEMCTRL_SDRAM_MASK	0x7

static const unsigned long adm_sdramsize[] __initdata = {
	0x0,		/* Reserved */
	0x0400000,	/* 4Mb */
	0x0800000,	/* 8Mb */
	0x1000000,	/* 16Mb */
	0x4000000,	/* 64Mb */
	0x8000000,	/* 128Mb */
};

/* determined physical memory size, not overridden by command line args  */
unsigned long physical_memsize = 0L;

struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];

struct prom_pmemblock * __init prom_getmdesc(void)
{
	char *memsize_str;
	unsigned int memsize;
	char cmdline[CL_SIZE], *ptr;

	memsize_str = prom_getenv("memsize");

	if (!memsize_str)
	{
		prom_printf("memsize not set in boot prom, set to default (8Mb)\n");
		physical_memsize = 0x00800000;
	}
	else
#ifdef DEBUG
		prom_printf("prom_memsize = %s\n", memsize_str);
#endif
		physical_memsize = simple_strtol(memsize_str, NULL, 0);

	/* Check the command line for a memsize directive that overrides
	 * the physical/default amount */
	strcpy(cmdline, arcs_cmdline);
	ptr = strstr(cmdline, "memsize=");
	if (ptr && (ptr != cmdline) && (*(ptr - 1) != ' '))
	ptr = strstr(ptr, " memsize=");
	
	if (ptr)
		memsize = memparse(ptr + 8, &ptr);
	else
		memsize = physical_memsize;

       memset(mdesc, 0, sizeof(mdesc));

       mdesc[0].type = BOOT_MEM_RAM;
       mdesc[0].base = CPHYSADDR(PFN_ALIGN(&_end));
       mdesc[0].size = memsize - mdesc[0].base;

       return &mdesc[0];
}

void __init prom_meminit(void)
{
	struct prom_pmemblock *p;

	p = prom_getmdesc();

	while (p->size)
	{
		long type;
		unsigned long base, size;
		base = p->base;
		type = p->type,
		size = p->size;
		add_memory_region(base, size, type);
		p++;
	}
}

#if 0
void __init prom_meminit(void)
{
	unsigned long base = CPHYSADDR(PFN_ALIGN(&_end));
	unsigned long size;

	u32 memctrl = *(u32*)KSEG1ADDR(ADM5120_MEMCTRL);
	size = adm_sdramsize[memctrl & ADM5120_MEMCTRL_SDRAM_MASK];
	add_memory_region(base, size-base, BOOT_MEM_RAM);
}
#endif

unsigned long __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
        return;
}
