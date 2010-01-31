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
 *   Copyright 2007 John Crispin <blogic@openwrt.org> 
 */

#include <linux/init.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <asm/bootinfo.h>
#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>
#include <asm/amazon/model.h>
#include <asm/cpu.h>

void prom_putchar(char c)
{
	/* Wait for FIFO to empty */
	while ((amazon_readl(AMAZON_ASC_FSTAT) >> 8) != 0x00) ;
	/* Crude cr/nl handling is better than none */
	if(c == '\n')
		amazon_writel('\r', AMAZON_ASC_TBUF);
	amazon_writel(c, AMAZON_ASC_TBUF);
}

void prom_printf(const char * fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;
	char buf[1024];	

	va_start(args, fmt);
	/* FIXME - hopefully i < sizeof(buf) */
	l = vsprintf(buf, fmt, args); 
	va_end(args);
	buf_end = buf + l;
	
	for (p = buf; p < buf_end; p++)
		prom_putchar(*p);
}


void __init prom_init(void)
{
	char **envp = (char **) fw_arg2;

	int memsize = 16; /* assume 16M as default */

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

	strcpy(&(arcs_cmdline[0]), "console=ttyS0,115200 rootfstype=squashfs,jffs2");
	
	add_memory_region(0x00000000, memsize, BOOT_MEM_RAM);
}

void prom_free_prom_memory(void)
{
}

const char *get_system_type(void)
{
	return BOARD_SYSTEM_TYPE;
}
