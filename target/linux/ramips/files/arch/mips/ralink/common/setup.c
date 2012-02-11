/*
 * Ralink SoC common setup
 *
 * Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/serial_8250.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-ralink/common.h>
#include <asm/mach-ralink/machine.h>
#include <ralink_soc.h>

unsigned char ramips_sys_type[RAMIPS_SYS_TYPE_LEN];

const char *get_system_type(void)
{
	return ramips_sys_type;
}

static void __init detect_mem_size(void)
{
	unsigned long size;
	void *base;

	base = (void *) KSEG1ADDR(detect_mem_size);
	for (size = RALINK_SOC_MEM_SIZE_MIN; size < RALINK_SOC_MEM_SIZE_MAX;
	     size <<= 1 ) {
		if (!memcmp(base, base + size, 1024))
			break;
	}

	add_memory_region(RALINK_SOC_SDRAM_BASE, size, BOOT_MEM_RAM);
}

void __init ramips_early_serial_setup(int line, unsigned base, unsigned freq,
				      unsigned irq)
{
	struct uart_port p;
	int err;

	memset(&p, 0, sizeof(p));
	p.flags		= UPF_SKIP_TEST | UPF_FIXED_TYPE;
	p.iotype	= UPIO_AU;
	p.uartclk	= freq;
	p.regshift	= 2;
	p.type		= PORT_16550A;

	p.mapbase	= base;
	p.membase	= ioremap_nocache(p.mapbase, PAGE_SIZE);
	p.line		= line;
	p.irq		= irq;

	err = early_serial_setup(&p);
	if (err)
		printk(KERN_ERR "early serial%d registration failed %d\n",
		       line, err);
}

void __init plat_mem_setup(void)
{
	set_io_port_base(KSEG1);

	detect_mem_size();
	ramips_soc_setup();
}

__setup("board=", mips_machtype_setup);

static int __init ramips_machine_setup(void)
{
	mips_machine_setup();
	return 0;
}

arch_initcall(ramips_machine_setup);

static void __init ramips_generic_init(void)
{
}

MIPS_MACHINE(RAMIPS_MACH_GENERIC, "Generic", "Generic Ralink board",
	     ramips_generic_init);
