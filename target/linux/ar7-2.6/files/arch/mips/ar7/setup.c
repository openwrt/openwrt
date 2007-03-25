/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
 * 
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000 MIPS Technologies, Inc.  All rights reserved.
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
 */
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/tty.h>
#include <linux/pm.h>
#include <linux/serial_8250.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/serial_reg.h>

#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>
#include <asm/dma.h>
#include <asm/time.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <asm/reboot.h>
#include <asm/gdb-stub.h>
#include <asm/ar7/ar7.h>

extern void ar7_time_init(void);
static void ar7_machine_restart(char *command);
static void ar7_machine_halt(void);
static void ar7_machine_power_off(void);

static void ar7_machine_restart(char *command)
{
        volatile u32 *softres_reg = (u32 *)ioremap(AR7_REGS_RESET +
						   AR7_RESET_SOFTWARE, 1);
	prom_printf("Reboot\n");
        *softres_reg = 1;
}

static void ar7_machine_halt(void)
{
	prom_printf("Halt\n");
	while (1);
}

static void ar7_machine_power_off(void)
{
        volatile u32 *power_reg = (u32 *)ioremap(AR7_REGS_POWER, 1);
	u32 power_state = *power_reg | (3 << 30);
	prom_printf("Power off\n");
        *power_reg = power_state;
	ar7_machine_halt();
}

const char *get_system_type(void)
{
	u16 chip_id = ar7_chip_id();
	switch (chip_id) {
	case AR7_CHIP_7300:
		return "TI AR7 (TNETD7300)";
	case AR7_CHIP_7100:
		return "TI AR7 (TNETD7100)";
	case AR7_CHIP_7200:
		return "TI AR7 (TNETD7200)";
	default:
		return "TI AR7 (Unknown)";
	}
}

static int __init ar7_init_console(void)
{
	return 0;
}

/*
 * Initializes basic routines and structures pointers, memory size (as
 * given by the bios and saves the command line.
 */

extern void ar7_init_clocks(void);

void __init plat_mem_setup(void)
{
	unsigned long io_base;

	_machine_restart = ar7_machine_restart;
	_machine_halt = ar7_machine_halt;
	pm_power_off = ar7_machine_power_off;
	board_time_init = ar7_time_init;
	panic_timeout = 3;

	io_base = (unsigned long)ioremap(AR7_REGS_BASE, 0x10000);
	if (!io_base) panic("Can't remap IO base!\n");
	set_io_port_base(io_base);

	prom_meminit();
	ar7_init_clocks();

	ioport_resource.start = 0;
	ioport_resource.end   = ~0;
	iomem_resource.start  = 0;
	iomem_resource.end    = ~0;
}

console_initcall(ar7_init_console);
