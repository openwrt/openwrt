/*
 * arch/ubicom32/kernel/devtree.c
 *   Ubicom32 architecture device tree implementation.
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
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <asm/devtree.h>

/*
 * The device tree.
 */
struct devtree_node *devtree;

/*
 * devtree_print()
 *	Print the device tree.
 */
void devtree_print(void)
{
	struct devtree_node *p = devtree;
	printk(KERN_INFO "Device Tree:\n");
	while (p) {
		if (p->magic != DEVTREE_NODE_MAGIC) {
			printk(KERN_EMERG
			       "device tree has improper node: %p\n", p);
			return;
		}
		printk(KERN_INFO "\t%p: sendirq=%03d, recvirq=%03d, "
		       " name=%s\n", p, p->sendirq, p->recvirq, p->name);
		p = p->next;
	}
}
EXPORT_SYMBOL(devtree_print);

/*
 * devtree_irq()
 *	Return the IRQ(s) associated with devtree node.
 */
int devtree_irq(struct devtree_node *dn,
		unsigned char *sendirq,
		unsigned char *recvirq)
{
	if (dn->magic != DEVTREE_NODE_MAGIC) {
		printk(KERN_EMERG "improper node: %p\n", dn);
		if (sendirq) {
			*sendirq = DEVTREE_IRQ_NONE;
		}
		if (recvirq) {
			*recvirq = DEVTREE_IRQ_NONE;
		}
		return -EFAULT;
	}

	/*
	 * Copy the devtree irq(s) to the output parameters.
	 */
	if (sendirq) {
		*sendirq = dn->sendirq;
	}
	if (recvirq) {
		*recvirq = dn->recvirq;
	}
	return 0;
}
EXPORT_SYMBOL(devtree_irq);

/*
 * devtree_find_next()
 *	Provide an iterator for walking the device tree.
 */
struct devtree_node *devtree_find_next(struct devtree_node **cur)
{
	struct devtree_node *p = *cur;
	if (!p) {
		*cur = devtree;
		return devtree;
	}
	p = p->next;
	*cur = p;
	return p;
}

/*
 * devtree_find_by_irq()
 *	Return the node associated with a given irq.
 */
struct devtree_node *devtree_find_by_irq(uint8_t sendirq, uint8_t recvirq)
{
	struct devtree_node *p = devtree;

	if (sendirq == recvirq) {
		printk(KERN_EMERG "identical request makes no sense sendirq = "
		       "%d, recvirq= %d\n", sendirq, recvirq);
		return NULL;
	}

	while (p) {
		if (p->magic != DEVTREE_NODE_MAGIC) {
			printk(KERN_EMERG
			       "device tree has improper node: %p\n", p);
			return NULL;
		}

		/*
		 * See if we can find a match on the IRQ(s) specified.
		 */
		if ((sendirq == p->sendirq) && (recvirq == p->recvirq)) {
			return p;
		}

		if ((sendirq == DEVTREE_IRQ_DONTCARE) &&
		    (p->recvirq == recvirq)) {
			return p;
		}

		if ((recvirq == DEVTREE_IRQ_DONTCARE) &&
		    (p->sendirq == sendirq)) {
			return p;
		}

		p = p->next;
	}
	return NULL;
}
EXPORT_SYMBOL(devtree_find_by_irq);

/*
 * devtree_find_node()
 *	Find a node in the device tree by name.
 */
struct devtree_node *devtree_find_node(const char *str)
{
	struct devtree_node *p = devtree;
	while (p) {
		if (p->magic != DEVTREE_NODE_MAGIC) {
			printk(KERN_EMERG
			       "device tree has improper node: %p\n", p);
			return NULL;
		}
		if (strcmp(p->name, str) == 0) {
			return p;
		}
		p = p->next;
	}
	return NULL;
}
EXPORT_SYMBOL(devtree_find_node);
