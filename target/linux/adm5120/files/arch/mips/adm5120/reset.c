/*
 *  $Id$
 *
 *  ADM5120 specific reset routines
 *
 *  Copyright (C) ADMtek Incorporated.
 *  Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *  Copyright (C) 2007 OpenWrt.org
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/irq.h>
#include <linux/io.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <adm5120_info.h>
#include <adm5120_defs.h>
#include <adm5120_switch.h>

void (*adm5120_board_reset)(void);

void adm5120_restart(char *command)
{
	/* TODO: stop switch before reset */

	if (adm5120_board_reset)
		adm5120_board_reset();

	SW_WRITE_REG(SWITCH_REG_SOFT_RESET, 1);
}

void adm5120_halt(void)
{
	local_irq_disable();

	printk(KERN_NOTICE "\n** You can safely turn off the power\n");
	while (1) {
		if (cpu_wait)
			cpu_wait();
	};
}
