/*
 * arch/ubicom32/include/asm/devtree.h
 *   Device Tree Header File (Shared between ultra and the Host OS)
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

#ifndef _ASM_UBICOM32_DEVTREE_H
#define _ASM_UBICOM32_DEVTREE_H

#define DEVTREE_MAX_NAME 32
#define DEVTREE_IRQ_NONE 0xff
#define DEVTREE_IRQ_DONTCARE 0xff
#define DEVTREE_NODE_MAGIC 0x10203040

struct devtree_node {
	struct devtree_node *next;
	unsigned char sendirq;
	unsigned char recvirq;
	char name[DEVTREE_MAX_NAME];
	unsigned int magic;
};

extern struct devtree_node *devtree;
extern struct devtree_node *devtree_find_by_irq(uint8_t sendirq, uint8_t recvirq);
extern struct devtree_node *devtree_find_node(const char *str);
extern struct devtree_node *devtree_find_next(struct devtree_node **cur);
extern int devtree_irq(struct devtree_node *dn, unsigned char *sendirq, unsigned char *recvirq);
extern void devtree_print(void);

#endif /* _ASM_UBICOM32_DEVTREE_H */
