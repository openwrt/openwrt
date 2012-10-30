/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

/*
 * This file is just holds extra declarations of macros that would normally
 * be providesd in the Linux kernel. These macros have been written from
 * scratch but are functionally equivalent to the Linux ones.
 *
 */

#ifndef __YAFFS_LIST_H__
#define __YAFFS_LIST_H__


#include "yportenv.h"

/*
 * This is a simple doubly linked list implementation that matches the
 * way the Linux kernel doubly linked list implementation works.
 */

struct ylist_head {
	struct ylist_head *next; /* next in chain */
	struct ylist_head *prev; /* previous in chain */
};


/* Initialise a static list */
#define YLIST_HEAD(name) \
struct ylist_head name = { &(name), &(name)}



/* Initialise a list head to an empty list */
#define YINIT_LIST_HEAD(p) \
do { \
	(p)->next = (p);\
	(p)->prev = (p); \
} while (0)


/* Add an element to a list */
static Y_INLINE void ylist_add(struct ylist_head *newEntry,
				struct ylist_head *list)
{
	struct ylist_head *listNext = list->next;

	list->next = newEntry;
	newEntry->prev = list;
	newEntry->next = listNext;
	listNext->prev = newEntry;

}

static Y_INLINE void ylist_add_tail(struct ylist_head *newEntry,
				 struct ylist_head *list)
{
	struct ylist_head *listPrev = list->prev;

	list->prev = newEntry;
	newEntry->next = list;
	newEntry->prev = listPrev;
	listPrev->next = newEntry;

}


/* Take an element out of its current list, with or without
 * reinitialising the links.of the entry*/
static Y_INLINE void ylist_del(struct ylist_head *entry)
{
	struct ylist_head *listNext = entry->next;
	struct ylist_head *listPrev = entry->prev;

	listNext->prev = listPrev;
	listPrev->next = listNext;

}

static Y_INLINE void ylist_del_init(struct ylist_head *entry)
{
	ylist_del(entry);
	entry->next = entry->prev = entry;
}


/* Test if the list is empty */
static Y_INLINE int ylist_empty(struct ylist_head *entry)
{
	return (entry->next == entry);
}


/* ylist_entry takes a pointer to a list entry and offsets it to that
 * we can find a pointer to the object it is embedded in.
 */


#define ylist_entry(entry, type, member) \
	((type *)((char *)(entry)-(unsigned long)(&((type *)NULL)->member)))


/* ylist_for_each and list_for_each_safe  iterate over lists.
 * ylist_for_each_safe uses temporary storage to make the list delete safe
 */

#define ylist_for_each(itervar, list) \
	for (itervar = (list)->next; itervar != (list); itervar = itervar->next)

#define ylist_for_each_safe(itervar, saveVar, list) \
	for (itervar = (list)->next, saveVar = (list)->next->next; \
		itervar != (list); itervar = saveVar, saveVar = saveVar->next)


#endif
