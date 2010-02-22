/*
 * arch/ubicom32/mach-common/board.c
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

struct boardnode {
	struct devtree_node dn;
	const char *revision;
};

static const struct boardnode *bn;

/*
 * board_get_revision()
 *	Returns revision string of the board.
 */
const char *board_get_revision(void)
{
	if (!bn) {
		return "NULL";
	}

	return bn->revision;
}

/*
 * board_init
 */
void __init board_init(void)
{
	bn = (struct boardnode *)devtree_find_node("board");
	if (!bn) {
		printk(KERN_WARNING "board node not found\n");
		return;
	}
}
