/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _PHL_LIST_H_
#define _PHL_LIST_H_

/*
 * Copied from include/linux/...
 */
#ifdef PHL_PLATFORM_WINDOWS

#define phl_container_of(_ptr, _type, _member) container_of(_ptr, _type, _member)
#define phl_list_for_loop(_pos, _type, _head, _member) \
				list_for_each_entry(_pos, _type, _head, _member)
#define phl_list_for_loop_safe(_pos, _n, _type, _head, _member) \
				list_for_each_entry_safe(_pos, _n, _type, _head, _member)


#elif defined(PHL_PLATFORM_LINUX)

#define phl_container_of(_ptr, _type, _member) container_of(_ptr, _type, _member)
#define phl_list_for_loop(_pos, _type, _head, _member) \
				list_for_each_entry(_pos, _head, _member)
#define phl_list_for_loop_safe(_pos, _n, _type, _head, _member) \
				list_for_each_entry_safe(_pos, _n, _head, _member)
#ifndef list_last_entry
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)
#endif

#elif defined(PHL_PLATFORM_MACOS)

#define phl_container_of(_ptr, _type, _member) container_of(_ptr, _type, _member)
#define phl_list_for_loop(_pos, _type, _head, _member) \
				list_for_each_entry(_pos, _type, _head, _member)
#define phl_list_for_loop_safe(_pos, _n, _type, _head, _member) \
				list_for_each_entry_safe(_pos, _n, _type, _head, _member)

#else /* os free */
#undef offsetof
#define offsetof(_type, _member) ((size_t) &((_type *)0)->_member)
#define phl_container_of(_ptr, _type, _member) container_of(_ptr, _type, _member)
#define phl_list_for_loop(_pos, _type, _head, _member) \
				list_for_each_entry(_pos, _type, _head, _member)
#define phl_list_for_loop_safe(_pos, _n, _type, _head, _member) \
				list_for_each_entry_safe(_pos, _n, _type, _head, _member)

#endif

enum list_pos {
	_first,
	_tail
};

#ifndef PHL_PLATFORM_LINUX
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(_ptr, _type, _member)	\
		((_type*)((char*)(_ptr) - (char*)&(((_type*)0)->_member)))


struct list_head {
	struct list_head *next, *prev;
};


#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static __inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}


/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @type:	the type of the struct this is embedded in.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, type, head, member)				\
	for (pos = list_entry((head)->next, type, member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, type, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @type:	the type of the struct this is embedded in
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, type, head, member)			\
	for (pos = list_entry((head)->next, type, member),	\
		n = list_entry(pos->member.next, type, member);	\
	     &pos->member != (head);					\
	     pos = n, n = list_entry(n->member.next, type, member))

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static __inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline void __list_add(struct list_head *_new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static __inline void list_add(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head, head->next);
}


/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static __inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)
/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static __inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = (struct list_head*)LIST_POISON1;
	entry->prev = (struct list_head*)LIST_POISON2;
}

#endif
#endif /*_PHL_LIST_H_*/
