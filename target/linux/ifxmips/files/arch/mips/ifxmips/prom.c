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

static char buf[1024]; /* for prom_printf() */

/* for voice cpu (MIPS24K) */
unsigned int *prom_cp1_base;
unsigned int prom_cp1_size;

/* for Multithreading (APRP) on MIPS34K */
unsigned long physical_memsize;

/* early printk on asc0 or asc1 ? */
#ifdef CONFIG_IFXMIPS_PROM_ASC0
#define IFXMIPS_ASC_DIFF	0
#else
#define IFXMIPS_ASC_DIFF	IFXMIPS_ASC_BASE_DIFF
#endif

static inline u32 asc_r32(unsigned long r)
{
	return ifxmips_r32((u32 *)(IFXMIPS_ASC_BASE_ADDR + IFXMIPS_ASC_DIFF + r));
}

static inline void asc_w32(u32 v, unsigned long r)
{
	ifxmips_w32(v, (u32 *)(IFXMIPS_ASC_BASE_ADDR + IFXMIPS_ASC_DIFF + r));
}

void prom_free_prom_memory(void)
{
}

void prom_putchar(char c)
{
	unsigned long flags;

	local_irq_save(flags);
	while ((asc_r32(IFXMIPS_ASC_FSTAT) & ASCFSTAT_TXFFLMASK) >> ASCFSTAT_TXFFLOFF)
		;

	if (c == '\n')
		asc_w32('\r', IFXMIPS_ASC_TBUF);
	asc_w32(c, IFXMIPS_ASC_TBUF);
	local_irq_restore(flags);
}

void prom_printf(const char *fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;

	va_start(args, fmt);
	l = vsprintf(buf, fmt, args);
	va_end(args);
	buf_end = buf + l;

	for (p = buf; p < buf_end; p++)
		prom_putchar(*p);
}

unsigned int *prom_get_cp1_base(void)
{
	return prom_cp1_base;
}
EXPORT_SYMBOL(prom_get_cp1_base);

unsigned int prom_get_cp1_size(void)
{
	/* return size im MB */
	return prom_cp1_size>>20;
}
EXPORT_SYMBOL(prom_get_cp1_size);

void __init prom_init(void)
{
	int argc = fw_arg0;
	char **argv = (char **) fw_arg1;
	char **envp = (char **) fw_arg2;

	int memsize = 16; /* assume 16M as default */
	int i;

	mips_machtype = MACH_INFINEON_IFXMIPS;

	if (argc) {
		argv = (char **)KSEG1ADDR((unsigned long)argv);
		arcs_cmdline[0] = '\0';
		for (i = 1; i < argc; i++) {
			char *a = (char *)KSEG1ADDR(argv[i]);
			if (!argv[i])
				continue;
			/* for voice cpu on Twinpass/Danube */
			if (cpu_data[0].cputype == CPU_24K)
				if (!strncmp(a, "cp1_size=", 9)) {
					prom_cp1_size = memparse(a + 9, &a);
					continue;
				}
			if (strlen(arcs_cmdline) + strlen(a + 1) >= sizeof(arcs_cmdline)) {
				prom_printf("cmdline overflow, skipping: %s\n", a);
				break;
			}
			strcat(arcs_cmdline, a);
			strcat(arcs_cmdline, " ");
		}
		if (!*arcs_cmdline)
			strcpy(&(arcs_cmdline[0]),
				"console=ttyS0,115200 rootfstype=squashfs,jffs2");
	}
	envp = (char **)KSEG1ADDR((unsigned long)envp);
	while (*envp) {
		char *e = (char *)KSEG1ADDR(*envp);

		if (!strncmp(e, "memsize=", 8)) {
			e += 8;
			memsize = simple_strtoul(e, NULL, 10);
		}
		envp++;
	}
	memsize *= 1024 * 1024;

	/* only on Twinpass/Danube a second CPU is used for Voice */
	if ((cpu_data[0].cputype == CPU_24K) && (prom_cp1_size)) {
		memsize -= prom_cp1_size;
		prom_cp1_base = (unsigned int *)KSEG1ADDR(memsize);

		prom_printf("Using %dMB Ram and reserving %dMB for cp1\n",
			memsize>>20, prom_cp1_size>>20);
	}

	add_memory_region(0x00000000, memsize, BOOT_MEM_RAM);
}
