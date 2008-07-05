/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2005 Wu Qi Ming infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/bootmem.h>
#include <asm/bootinfo.h>
#include <asm/ifxmips/ifxmips.h>

static char buf[1024];
unsigned int *prom_cp1_base = NULL;
unsigned int prom_cp1_size = 0;

#ifdef IFXMIPS_PROM_ASC0
#define IFXMIPS_ASC_DIFF	0
#else
#define IFXMIPS_ASC_DIFF	IFXMIPS_ASC_BASE_DIFF
#endif

static inline u32
asc_r32(unsigned long r)
{
	return ifxmips_r32((u32*)(IFXMIPS_ASC_BASE_ADDR + IFXMIPS_ASC_DIFF + r));
}

static inline void
asc_w32(u32 v, unsigned long r)
{
	ifxmips_w32(v, (u32*)(IFXMIPS_ASC_BASE_ADDR + IFXMIPS_ASC_DIFF + r));
}

void
prom_free_prom_memory(void)
{
}

void
prom_putchar(char c)
{
	unsigned long flags;

	local_irq_save(flags);
	while((asc_r32(IFXMIPS_ASC_FSTAT) & ASCFSTAT_TXFFLMASK) >> ASCFSTAT_TXFFLOFF);

	if(c == '\n')
		asc_w32('\r', IFXMIPS_ASC_TBUF);
	asc_w32(c, IFXMIPS_ASC_TBUF);
	local_irq_restore(flags);
}

void
prom_printf(const char * fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;

	va_start(args, fmt);
	l = vsprintf(buf, fmt, args);
	va_end(args);
	buf_end = buf + l;

	for(p = buf; p < buf_end; p++)
		prom_putchar(*p);
}

unsigned int *prom_get_cp1_base(void)
{
	return prom_cp1_base;
}
EXPORT_SYMBOL(prom_get_cp1_base);

unsigned int prom_get_cp1_size(void)
{
	return prom_cp1_size;
}
EXPORT_SYMBOL(prom_get_cp1_size);

void __init
prom_init(void)
{
	int argc = fw_arg0;
	char **argv = (char **) fw_arg1;
	char **envp = (char **) fw_arg2;

	int memsize = 16;
	int i;

	mips_machtype = MACH_INFINEON_IFXMIPS;

	argv = (char**)KSEG1ADDR((unsigned long)argv);
	arcs_cmdline[0] = '\0';
	for(i = 1; i < argc; i++)
	{
		char *a = (char*)KSEG1ADDR(argv[i]);
		if(!a)
			continue;
		if(strlen(arcs_cmdline) + strlen(a + 1) >= sizeof(arcs_cmdline))
			break;
		strcat(arcs_cmdline, a);
		strcat(arcs_cmdline, " ");
	}

	envp = (char**)KSEG1ADDR((unsigned long)envp);
	while(*envp)
	{
		char *e = (char*)KSEG1ADDR(*envp);

		if(!strncmp(e, "memsize=", 8))
		{
			e += 8;
			memsize = simple_strtoul(e, NULL, 10);
		}
		envp++;
	}

	prom_cp1_size = 2;
	memsize -= prom_cp1_size;
	prom_cp1_base = (unsigned int*)(0xA0000000 + (memsize * 1024 * 1024));

	prom_printf("Using %dMB Ram and reserving %dMB for cp1\n", memsize, prom_cp1_size);
	memsize *= 1024 * 1024;

	if(!*arcs_cmdline)
		strcpy(&(arcs_cmdline[0]),
			"console=ttyS0,115200 rootfstype=squashfs,jffs2 init=/etc/preinit");

	add_memory_region(0x00000000, memsize, BOOT_MEM_RAM);
}
