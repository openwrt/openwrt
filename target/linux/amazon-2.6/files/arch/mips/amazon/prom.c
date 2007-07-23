/*
 * copyright 2007 john crispin <blogic@openwrt.org> 
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
	while (((*AMAZON_ASC_FSTAT) >> 8) != 0x00) ;
	/* Crude cr/nl handling is better than none */
	if(c == '\n')
		*AMAZON_ASC_TBUF=('\r');
	*AMAZON_ASC_TBUF=(c);
}

void prom_printf(const char * fmt, ...)
{
	va_list args;
	int l;
	char *p, *buf_end;
	char buf[1024];	

	va_start(args, fmt);
	l = vsprintf(buf, fmt, args); /* hopefully i < sizeof(buf) */
	va_end(args);
	buf_end = buf + l;
	
	for (p = buf; p < buf_end; p++)
		prom_putchar(*p);
}


void __init prom_init(void)
{
	mips_machgroup = MACH_GROUP_INFINEON;
	mips_machtype = MACH_INFINEON_AMAZON;

	strcpy(&(arcs_cmdline[0]), "console=ttyS0,115200 rootfstype=squashfs,jffs2 init=/etc/preinit");
	
	add_memory_region(0x00000000, 0x1000000, BOOT_MEM_RAM);
}

void prom_free_prom_memory(void)
{
}

const char *get_system_type(void)
{
	return BOARD_SYSTEM_TYPE;
}
