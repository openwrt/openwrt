/*
 * Procedures for creating, accessing and interpreting the device tree.
 *
 * Paul Mackerras	August 1996.
 * Copyright (C) 1996-2005 Paul Mackerras.
 * 
 *  Adapted for 64bit PowerPC by Dave Engebretsen and Peter Bergner.
 *    {engebret|bergner}@us.ibm.com 
 *
 *  Adapted for sparc64 by David S. Miller davem@davemloft.net
 *
 *  Adapter for i386/OLPC by Andres Salomon <dilinger@debian.org>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/module.h>
#include <asm/prom.h>
#include <asm/ofw.h>

/*
 * XXX: This is very much a stub; right now we're keeping 2 device trees
 * in memory (one for promfs, and one here).  That will not remain
 * for long!
 */

static struct device_node *allnodes;

/* use when traversing tree through the allnext, child, sibling,
 * or parent members of struct device_node.
 */
static DEFINE_RWLOCK(devtree_lock);

int of_device_is_compatible(const struct device_node *device,
			    const char *compat)
{
	const char* cp;
	int cplen, l;

	cp = of_get_property(device, "compatible", &cplen);
	if (cp == NULL)
		return 0;
	while (cplen > 0) {
		if (strncmp(cp, compat, strlen(compat)) == 0)
			return 1;
		l = strlen(cp) + 1;
		cp += l;
		cplen -= l;
	}

	return 0;
}
EXPORT_SYMBOL(of_device_is_compatible);

struct device_node *of_get_parent(const struct device_node *node)
{
	struct device_node *np;

	if (!node)
		return NULL;

	np = node->parent;

	return np;
}
EXPORT_SYMBOL(of_get_parent);

struct device_node *of_get_next_child(const struct device_node *node,
	struct device_node *prev)
{
	struct device_node *next;

	next = prev ? prev->sibling : node->child;
	for (; next != 0; next = next->sibling) {
		break;
	}

	return next;
}
EXPORT_SYMBOL(of_get_next_child);

struct device_node *of_find_node_by_path(const char *path)
{
	struct device_node *np = allnodes;

	for (; np != 0; np = np->allnext) {
		if (np->full_name != 0 && strcmp(np->full_name, path) == 0)
			break;
	}

	return np;
}
EXPORT_SYMBOL(of_find_node_by_path);

struct device_node *of_find_node_by_phandle(phandle handle)
{
	struct device_node *np;

	for (np = allnodes; np != 0; np = np->allnext)
		if (np->node == handle)
			break;

	return np;
}
EXPORT_SYMBOL(of_find_node_by_phandle);

struct device_node *of_find_node_by_name(struct device_node *from,
	const char *name)
{
	struct device_node *np;

	np = from ? from->allnext : allnodes;
	for (; np != NULL; np = np->allnext)
		if (np->name != NULL && strcmp(np->name, name) == 0)
			break;

	return np;
}
EXPORT_SYMBOL(of_find_node_by_name);

struct device_node *of_find_node_by_type(struct device_node *from,
	const char *type)
{
	struct device_node *np;

	np = from ? from->allnext : allnodes;
	for (; np != 0; np = np->allnext)
		if (np->type != 0 && strcmp(np->type, type) == 0)
			break;

	return np;
}
EXPORT_SYMBOL(of_find_node_by_type);

struct device_node *of_find_compatible_node(struct device_node *from,
	const char *type, const char *compatible)
{
	struct device_node *np;

	np = from ? from->allnext : allnodes;
	for (; np != 0; np = np->allnext) {
		if (type != NULL
		    && !(np->type != 0 && strcmp(np->type, type) == 0))
			continue;
		if (of_device_is_compatible(np, compatible))
			break;
	}

	return np;
}
EXPORT_SYMBOL(of_find_compatible_node);

struct property *of_find_property(const struct device_node *np,
				  const char *name,
				  int *lenp)
{
	struct property *pp;

	for (pp = np->properties; pp != 0; pp = pp->next) {
		if (strcasecmp(pp->name, name) == 0) {
			if (lenp != 0)
				*lenp = pp->length;
			break;
		}
	}
	return pp;
}
EXPORT_SYMBOL(of_find_property);

/*
 * Find a property with a given name for a given node
 * and return the value.
 */
const void *of_get_property(const struct device_node *np, const char *name,
		      int *lenp)
{
	struct property *pp = of_find_property(np,name,lenp);
	return pp ? pp->value : NULL;
}
EXPORT_SYMBOL(of_get_property);

int of_getintprop_default(struct device_node *np, const char *name, int def)
{
	struct property *prop;
	int len;

	prop = of_find_property(np, name, &len);
	if (!prop || len != 4)
		return def;

	return *(int *) prop->value;
}
EXPORT_SYMBOL(of_getintprop_default);

int of_n_addr_cells(struct device_node *np)
{
	const int* ip;
	do {
		if (np->parent)
			np = np->parent;
		ip = of_get_property(np, "#address-cells", NULL);
		if (ip != NULL)
			return *ip;
	} while (np->parent);
	/* No #address-cells property for the root node, default to 2 */
	return 2;
}
EXPORT_SYMBOL(of_n_addr_cells);

int of_n_size_cells(struct device_node *np)
{
	const int* ip;
	do {
		if (np->parent)
			np = np->parent;
		ip = of_get_property(np, "#size-cells", NULL);
		if (ip != NULL)
			return *ip;
	} while (np->parent);
	/* No #size-cells property for the root node, default to 1 */
	return 1;
}
EXPORT_SYMBOL(of_n_size_cells);

int of_set_property(struct device_node *dp, const char *name, void *val, int len)
{
	return -EIO;
}
EXPORT_SYMBOL(of_set_property);

static unsigned int prom_early_allocated;

static void * __init prom_early_alloc(unsigned long size)
{
	void *ret;

	ret = kmalloc(size, GFP_KERNEL);
	if (ret != NULL)
		memset(ret, 0, size);
	else
		printk(KERN_ERR "ACK!  couldn't allocate prom memory!\n");

	prom_early_allocated += size;

	return ret;
}

static int is_root_node(const struct device_node *dp)
{
	if (!dp)
		return 0;

	return (dp->parent == NULL);
}

static char * __init build_path_component(struct device_node *dp)
{
	int pathlen;
	char *n, *i;

	if (ofw("package-to-path", 3, 1, dp->node, NULL, 0, &pathlen)) {
		printk(KERN_ERR "PROM: unable to get path name from OFW!\n");
		return "ERROR";
	}
	n = prom_early_alloc(pathlen + 1);
	if (ofw("package-to-path", 3, 1, dp->node, n, pathlen+1, &pathlen))
		printk(KERN_ERR "PROM: unable to get path name from OFW\n");

	if ((i = strrchr(n, '/')))
		n = ++i;	/* we only want the file name */
	return n;
}

static char * __init build_full_name(struct device_node *dp)
{
	int len, ourlen, plen;
	char *n;

	plen = strlen(dp->parent->full_name);
	ourlen = strlen(dp->path_component_name);
	len = ourlen + plen + 2;

	n = prom_early_alloc(len);
	strcpy(n, dp->parent->full_name);
	if (!is_root_node(dp->parent)) {
		strcpy(n + plen, "/");
		plen++;
	}
	strcpy(n + plen, dp->path_component_name);

	return n;
}

static struct property * __init build_one_prop(phandle node, char *prev, char *special_name, void *special_val, int special_len)
{
	static struct property *tmp = NULL;
	struct property *p;

	if (tmp) {
		p = tmp;
		memset(p, 0, sizeof(*p) + 32);
		tmp = NULL;
	} else {
		p = prom_early_alloc(sizeof(struct property) + 32);
	}

	p->name = (char *) (p + 1);
	if (special_name) {
		strcpy(p->name, special_name);
		p->length = special_len;
		p->value = prom_early_alloc(special_len);
		memcpy(p->value, special_val, special_len);
	} else {
		int fl;
		if (prev == NULL) {
			if (ofw("nextprop", 3, 1, node, "", p->name, &fl)) {
				printk(KERN_ERR "PROM: %s: nextprop failed!\n", __func__);
				return NULL;
			}
		} else {
			if (ofw("nextprop", 3, 1, node, prev, p->name, &fl)) {
				printk(KERN_ERR "PROM: %s: nextprop failed!\n", __func__);
				return NULL;
			}
		}
		if (strlen(p->name) == 0 || fl != 1) {
			tmp = p;
			return NULL;
		}
		if (ofw("getproplen", 2, 1, node, p->name, &p->length)) {
			printk(KERN_ERR "PROM: %s: getproplen failed!\n", __func__);
			return NULL;
		}
		if (p->length <= 0) {
			p->length = 0;
		} else {
			p->value = prom_early_alloc(p->length + 1);
			if (ofw("getprop", 4, 1, node, p->name, p->value, p->length, &p->length)) {
				printk(KERN_ERR "PROM: %s: getprop failed!\n", __func__);
				return NULL;
			}
			((unsigned char *)p->value)[p->length] = '\0';
		}
	}
	return p;
}

static struct property * __init build_prop_list(phandle node)
{
	struct property *head, *tail;

	head = tail = build_one_prop(node, NULL,
				     ".node", &node, sizeof(node));

	tail->next = build_one_prop(node, NULL, NULL, NULL, 0);
	tail = tail->next;
	while(tail) {
		tail->next = build_one_prop(node, tail->name,
					    NULL, NULL, 0);
		tail = tail->next;
	}

	return head;
}

static char * __init get_one_property(phandle node, const char *name)
{
	char *buf = "<NULL>";
	int len;

	if (ofw("getproplen", 2, 1, node, name, &len)) {
		printk(KERN_ERR "PROM: %s: getproplen failed!\n", __func__);
		return NULL;
	}
	if (len > 0) {
		buf = prom_early_alloc(len);
		if (ofw("getprop", 4, 1, node, name, buf, len, &len)) {
			printk(KERN_ERR "PROM: %s: getprop failed!\n", __func__);
			return NULL;
		}
	}

	return buf;
}

static struct device_node * __init create_node(phandle node, struct device_node *parent)
{
	struct device_node *dp;

	if (!node)
		return NULL;

	dp = prom_early_alloc(sizeof(*dp));
	dp->parent = parent;

	kref_init(&dp->kref);

	dp->name = get_one_property(node, "name");
	dp->type = get_one_property(node, "device_type");
	dp->node = node;

	dp->properties = build_prop_list(node);

	return dp;
}

static struct device_node * __init build_tree(struct device_node *parent, phandle node, struct device_node ***nextp)
{
	struct device_node *ret = NULL, *prev_sibling = NULL;
	struct device_node *dp;
	u32 child;

	while (1) {
		dp = create_node(node, parent);
		if (!dp)
			break;

		if (prev_sibling)
			prev_sibling->sibling = dp;

		if (!ret)
			ret = dp;
		prev_sibling = dp;

		*(*nextp) = dp;
		*nextp = &dp->allnext;

		dp->path_component_name = build_path_component(dp);
		dp->full_name = build_full_name(dp);

		if (ofw("child", 1, 1, node, &child)) {
			printk(KERN_ERR "PROM: %s: fetching child failed!\n", __func__);
			return NULL;
		}
		dp->child = build_tree(dp, child, nextp);

		if (ofw("peer", 1, 1, node, &node)) {
			printk(KERN_ERR "PROM: %s: fetching peer failed!\n", __func__);
			return NULL;
		}
	}

	return ret;
}

static phandle root_node;

void __init prom_build_devicetree(void)
{
	struct device_node **nextp;
	u32 child;

	if (ofw("peer", 1, 1, 0, &root_node)) {
		printk(KERN_ERR "PROM: unable to get root node from OFW!\n");
		return;
	}

	allnodes = create_node(root_node, NULL);
	allnodes->path_component_name = "";
	allnodes->full_name = "/";

	nextp = &allnodes->allnext;
	if (ofw("child", 1, 1, allnodes->node, &child)) {
		printk(KERN_ERR "PROM: unable to get child node from OFW!\n");
		return;
	}
	allnodes->child = build_tree(allnodes, child, &nextp);
	printk("PROM: Built device tree with %u bytes of memory.\n",
	       prom_early_allocated);
}
