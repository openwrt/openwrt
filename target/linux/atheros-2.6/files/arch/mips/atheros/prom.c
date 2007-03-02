/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright MontaVista Software Inc
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

/*
 * Prom setup file for ar531x
 */

#include <linux/init.h>
#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/bootmem.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include "ar531x.h"

void __init prom_init(void)
{

	mips_machgroup = MACH_GROUP_ATHEROS;
	mips_machtype = -1;

	DO_AR5312(ar5312_prom_init();)
	DO_AR5315(ar5315_prom_init();)

	strcpy(arcs_cmdline, "console=ttyS0,9600 rootfstype=squashfs,jffs2 init=/etc/preinit");
}

void __init prom_free_prom_memory(void)
{
}
