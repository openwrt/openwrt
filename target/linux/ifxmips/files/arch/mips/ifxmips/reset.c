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
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */

#include <linux/kernel.h>
#include <linux/pm.h>
#include <asm/reboot.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/ifxmips/ifxmips.h>

static void
ifxmips_machine_restart(char *command)
{
	printk(KERN_NOTICE "System restart\n");
	local_irq_disable();

	ifxmips_w32(ifxmips_r32(IFXMIPS_RCU_RST) | IFXMIPS_RCU_RST_ALL, IFXMIPS_RCU_RST);
	for(;;);
}

static void
ifxmips_machine_halt(void)
{
	printk(KERN_NOTICE "System halted.\n");
	local_irq_disable();
	for(;;);
}

static void
ifxmips_machine_power_off(void)
{
	printk (KERN_NOTICE "Please turn off the power now.\n");
	local_irq_disable();
	for(;;);
}

void
ifxmips_reboot_setup(void)
{
	_machine_restart = ifxmips_machine_restart;
	_machine_halt = ifxmips_machine_halt;
	pm_power_off = ifxmips_machine_power_off;
}
