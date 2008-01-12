/*
 *   arch/mips/ifxmips/prom.c
 *
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
 *
 *   Rewrite of Infineon IFXMips code, thanks to infineon for the support,
 *   software and hardware
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/init.h>
#include <linux/bootmem.h>
#include <asm/bootinfo.h>
#include <asm/ifxmips/ifxmips.h>

static char buf[1024];

void
prom_free_prom_memory (void)
{
}

void
prom_putchar (char c)
{
	while ((readl(IFXMIPS_ASC1_FSTAT) & ASCFSTAT_TXFFLMASK) >> ASCFSTAT_TXFFLOFF);

	if (c == '\n')
		writel('\r', IFXMIPS_ASC1_TBUF);
	writel(c, IFXMIPS_ASC1_TBUF);
}

void
prom_printf (const char * fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;

	va_start(args, fmt);
	l = vsprintf(buf, fmt, args);
	va_end(args);
	buf_end = buf + l;

	for (p = buf; p < buf_end; p++)
	{
		prom_putchar(*p);
	}
}

void __init
prom_init(void)
{
	mips_machgroup = MACH_GROUP_IFXMIPS;
	mips_machtype = MACH_INFINEON_IFXMIPS;

	strcpy(&(arcs_cmdline[0]), "console=ttyS0,115200 rootfstype=squashfs,jffs2 init=/etc/preinit");
	add_memory_region (0x00000000, 0x2000000, BOOT_MEM_RAM);
}
