/*
 *  $Id$
 *
 *  ADM5120 specific reset routines
 *
 *  Copyright (C) ADMtek Incorporated.
 *  Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
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

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_switch.h>

#define ADM5120_SOFTRESET	0x12000004

void (*adm5120_board_reset)(void);

void adm5120_restart(char *command)
{
	/* TODO: stop switch before reset */

	if (adm5120_board_reset)
		adm5120_board_reset();

	*(u32*)KSEG1ADDR(ADM5120_SOFTRESET)=1;
}

void adm5120_halt(void)
{
        printk(KERN_NOTICE "\n** You can safely turn off the power\n");
        while (1);
}

void adm5120_power_off(void)
{
        adm5120_halt();
}
