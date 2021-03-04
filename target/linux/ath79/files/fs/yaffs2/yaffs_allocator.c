/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "yaffs_allocator.h"
#include "yaffs_guts.h"
#include "yaffs_trace.h"
#include "yportenv.h"

/*
 * Each entry in yaffs_tnode_list and yaffs_obj_list hold blocks
 * of approx 100 objects that are themn allocated singly.
 * This is basically a simplified slab allocator.
 *
 * We don't use the Linux slab allocator because slab does not allow
 * us to dump all the objects in one hit when we do a umount and tear
 * down  all the tnodes and objects. slab requires that we first free
 * the individual objects.
 *
 * Once yaffs has been mainlined I shall try to motivate for a change
 * to slab to provide the extra features we need here.
 */

struct yaffs_tnode_list {
	struct yaffs_tnode_list *next;
	struct yaffs_tnode *tnodes;
};

struct yaffs_obj_list {
	struct yaffs_obj_list *next;
	struct yaffs_obj *objects;
};

struct yaffs_allocator {
	int n_tnodes_created;
	struct yaffs_tnode *free_tnodes;
	int n_free_tnodes;
	struct yaffs_tnode_list *alloc_tnode_list;

	int n_obj_created;
	struct list_head free_objs;
	int n_free_objects;

	struct yaffs_obj_list *allocated_obj_list;
};

static void yaffs_deinit_raw_tnodes(struct yaffs_dev *dev)
{
	struct yaffs_allocator *allocator =
	    (struct yaffs_allocator *)dev->allocator;
	struct yaffs_tnode_list *tmp;

	if (!allocator) {
		BUG();
		return;
	}

	while (allocator->alloc_tnode_list) {
		tmp = allocator->alloc_tnode_list->next;

		kfree(allocator->alloc_tnode_list->tnodes);
		kfree(allocator->alloc_tnode_list);
		allocator->alloc_tnode_list = tmp;
	}

	allocator->free_tnodes = NULL;
	allocator->n_free_tnodes = 0;
	allocator->n_tnodes_created = 0;
}

static void yaffs_init_raw_tnodes(struct yaffs_dev *dev)
{
	struct yaffs_allocator *allocator = dev->allocator;

	if (!allocator) {
		BUG();
		return;
	}

	allocator->alloc_tnode_list = NULL;
	allocator->free_tnodes = NULL;
	allocator->n_free_tnodes = 0;
	allocator->n_tnodes_created = 0;
}

static int yaffs_create_tnodes(struct yaffs_dev *dev, int n_tnodes)
{
	struct yaffs_allocator *allocator =
	    (struct yaffs_allocator *)dev->allocator;
	int i;
	struct yaffs_tnode *new_tnodes;
	u8 *mem;
	struct yaffs_tnode *curr;
	struct yaffs_tnode *next;
	struct yaffs_tnode_list *tnl;

	if (!allocator) {
		BUG();
		return YAFFS_FAIL;
	}

	if (n_tnodes < 1)
		return YAFFS_OK;

	/* make these things */
	new_tnodes = kmalloc(n_tnodes * dev->tnode_size, GFP_NOFS);
	mem = (u8 *) new_tnodes;

	if (!new_tnodes) {
		yaffs_trace(YAFFS_TRACE_ERROR,
			"yaffs: Could not allocate Tnodes");
		return YAFFS_FAIL;
	}

	/* New hookup for wide tnodes */
	for (i = 0; i < n_tnodes - 1; i++) {
		curr = (struct yaffs_tnode *)&mem[i * dev->tnode_size];
		next = (struct yaffs_tnode *)&mem[(i + 1) * dev->tnode_size];
		curr->internal[0] = next;
	}

	curr = (struct yaffs_tnode *)&mem[(n_tnodes - 1) * dev->tnode_size];
	curr->internal[0] = allocator->free_tnodes;
	allocator->free_tnodes = (struct yaffs_tnode *)mem;

	allocator->n_free_tnodes += n_tnodes;
	allocator->n_tnodes_created += n_tnodes;

	/* Now add this bunch of tnodes to a list for freeing up.
	 * NB If we can't add this to the management list it isn't fatal
	 * but it just means we can't free this bunch of tnodes later.
	 */
	tnl = kmalloc(sizeof(struct yaffs_tnode_list), GFP_NOFS);
	if (!tnl) {
		yaffs_trace(YAFFS_TRACE_ERROR,
			"Could not add tnodes to management list");
		return YAFFS_FAIL;
	} else {
		tnl->tnodes = new_tnodes;
		tnl->next = allocator->alloc_tnode_list;
		allocator->alloc_tnode_list = tnl;
	}

	yaffs_trace(YAFFS_TRACE_ALLOCATE, "Tnodes added");

	return YAFFS_OK;
}

struct yaffs_tnode *yaffs_alloc_raw_tnode(struct yaffs_dev *dev)
{
	struct yaffs_allocator *allocator =
	    (struct yaffs_allocator *)dev->allocator;
	struct yaffs_tnode *tn = NULL;

	if (!allocator) {
		BUG();
		return NULL;
	}

	/* If there are none left make more */
	if (!allocator->free_tnodes)
		yaffs_create_tnodes(dev, YAFFS_ALLOCATION_NTNODES);

	if (allocator->free_tnodes) {
		tn = allocator->free_tnodes;
		allocator->free_tnodes = allocator->free_tnodes->internal[0];
		allocator->n_free_tnodes--;
	}

	return tn;
}

/* FreeTnode frees up a tnode and puts it back on the free list */
void yaffs_free_raw_tnode(struct yaffs_dev *dev, struct yaffs_tnode *tn)
{
	struct yaffs_allocator *allocator = dev->allocator;

	if (!allocator) {
		BUG();
		return;
	}

	if (tn) {
		tn->internal[0] = allocator->free_tnodes;
		allocator->free_tnodes = tn;
		allocator->n_free_tnodes++;
	}
	dev->checkpoint_blocks_required = 0;	/* force recalculation */
}

/*--------------- yaffs_obj alloaction ------------------------
 *
 * Free yaffs_objs are stored in a list using obj->siblings.
 * The blocks of allocated objects are stored in a linked list.
 */

static void yaffs_init_raw_objs(struct yaffs_dev *dev)
{
	struct yaffs_allocator *allocator = dev->allocator;

	if (!allocator) {
		BUG();
		return;
	}

	allocator->allocated_obj_list = NULL;
	INIT_LIST_HEAD(&allocator->free_objs);
	allocator->n_free_objects = 0;
}

static void yaffs_deinit_raw_objs(struct yaffs_dev *dev)
{
	struct yaffs_allocator *allocator = dev->allocator;
	struct yaffs_obj_list *tmp;

	if (!allocator) {
		BUG();
		return;
	}

	while (allocator->allocated_obj_list) {
		tmp = allocator->allocated_obj_list->next;
		kfree(allocator->allocated_obj_list->objects);
		kfree(allocator->allocated_obj_list);
		allocator->allocated_obj_list = tmp;
	}

	INIT_LIST_HEAD(&allocator->free_objs);
	allocator->n_free_objects = 0;
	allocator->n_obj_created = 0;
}

static int yaffs_create_free_objs(struct yaffs_dev *dev, int n_obj)
{
	struct yaffs_allocator *allocator = dev->allocator;
	int i;
	struct yaffs_obj *new_objs;
	struct yaffs_obj_list *list;

	if (!allocator) {
		BUG();
		return YAFFS_FAIL;
	}

	if (n_obj < 1)
		return YAFFS_OK;

	/* make these things */
	new_objs = kmalloc(n_obj * sizeof(struct yaffs_obj), GFP_NOFS);
	list = kmalloc(sizeof(struct yaffs_obj_list), GFP_NOFS);

	if (!new_objs || !list) {
		kfree(new_objs);
		new_objs = NULL;
		kfree(list);
		list = NULL;
		yaffs_trace(YAFFS_TRACE_ALLOCATE,
			"Could not allocate more objects");
		return YAFFS_FAIL;
	}

	/* Hook them into the free list */
	for (i = 0; i < n_obj; i++)
		list_add(&new_objs[i].siblings, &allocator->free_objs);

	allocator->n_free_objects += n_obj;
	allocator->n_obj_created += n_obj;

	/* Now add this bunch of Objects to a list for freeing up. */

	list->objects = new_objs;
	list->next = allocator->allocated_obj_list;
	allocator->allocated_obj_list = list;

	return YAFFS_OK;
}

struct yaffs_obj *yaffs_alloc_raw_obj(struct yaffs_dev *dev)
{
	struct yaffs_obj *obj = NULL;
	struct list_head *lh;
	struct yaffs_allocator *allocator = dev->allocator;

	if (!allocator) {
		BUG();
		return obj;
	}

	/* If there are none left make more */
	if (list_empty(&allocator->free_objs))
		yaffs_create_free_objs(dev, YAFFS_ALLOCATION_NOBJECTS);

	if (!list_empty(&allocator->free_objs)) {
		lh = allocator->free_objs.next;
		obj = list_entry(lh, struct yaffs_obj, siblings);
		list_del_init(lh);
		allocator->n_free_objects--;
	}

	return obj;
}

void yaffs_free_raw_obj(struct yaffs_dev *dev, struct yaffs_obj *obj)
{

	struct yaffs_allocator *allocator = dev->allocator;

	if (!allocator) {
		BUG();
		return;
	}

	/* Link into the free list. */
	list_add(&obj->siblings, &allocator->free_objs);
	allocator->n_free_objects++;
}

void yaffs_deinit_raw_tnodes_and_objs(struct yaffs_dev *dev)
{

	if (!dev->allocator) {
		BUG();
		return;
	}

	yaffs_deinit_raw_tnodes(dev);
	yaffs_deinit_raw_objs(dev);
	kfree(dev->allocator);
	dev->allocator = NULL;
}

void yaffs_init_raw_tnodes_and_objs(struct yaffs_dev *dev)
{
	struct yaffs_allocator *allocator;

	if (dev->allocator) {
		BUG();
		return;
	}

	allocator = kmalloc(sizeof(struct yaffs_allocator), GFP_NOFS);
	if (allocator) {
		dev->allocator = allocator;
		yaffs_init_raw_tnodes(dev);
		yaffs_init_raw_objs(dev);
	}
}

