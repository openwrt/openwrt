/*
 *  $Id$
 *
 *  Broadcom's CFE specific prom routines
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
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
 */

#include <linux/types.h>
#include <linux/init.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include <prom/cfe.h>
#include "prom_read.h"

/*
 * CFE based boards
 */
#define CFE_EPTSEAL	0x43464531 /* CFE1 is the magic number to recognize CFE
from other bootloaders */

static int cfe_found;

static u32 cfe_handle;
static u32 cfe_entry;
static u32 cfe_seal;

int __init cfe_present(void)
{
	/*
	 * This method only works, when we are booted directly from the CFE.
	 */
	u32 a1 = (u32) fw_arg1;

	if (cfe_found)
		return 1;

	cfe_handle = (u32) fw_arg0;
	cfe_entry = (u32) fw_arg2;
	cfe_seal = (u32) fw_arg3;

	/* Check for CFE by finding the CFE magic number */
	if (cfe_seal != CFE_EPTSEAL)
		return 0;

	/* cfe_a1_val must be 0, because only one CPU present in the ADM5120 */
	if (a1 != 0)
		return 0;

	/* The cfe_handle, and the cfe_entry must be kernel mode addresses */
	if ((cfe_handle < KSEG0) || (cfe_entry < KSEG0))
		return 0;

	cfe_found = 1;
	return 1;
}

char *cfe_getenv(char *envname)
{
	if (cfe_found == 0)
		return NULL;

	return NULL;
}
