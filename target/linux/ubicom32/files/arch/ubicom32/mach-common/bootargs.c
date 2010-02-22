/*
 * arch/ubicom32/mach-common/bootargs.c
 *   Board init and support code.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/cpu.h>
#include <asm/devtree.h>

struct bootargsnode {
	struct devtree_node dn;
	const char cmdline[512];
};

static const struct bootargsnode *ban;

/*
 * bootargs_get_cmdline()
 *	Returns kernel boot arguments set by the bootloader.
 */
const char *bootargs_get_cmdline(void)
{
	if (!ban) {
		return "";
	}

	return ban->cmdline;
}

/*
 * bootargs_init
 */
void __init bootargs_init(void)
{
	ban = (struct bootargsnode *)devtree_find_node("bootargs");
	if (!ban) {
		printk(KERN_WARNING "bootargs node not found\n");
		return;
	}
}
