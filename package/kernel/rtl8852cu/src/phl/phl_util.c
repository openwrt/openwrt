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
#include "phl_headers.h"
#define _PHL_UTIL_C_


/* phl queue general API */
void pq_init(void *d, struct phl_queue *q)
{
	INIT_LIST_HEAD(&q->queue);
	_os_spinlock_init(d, &q->lock);
	q->cnt = 0;
}

void pq_deinit(void *d, struct phl_queue *q)
{
	_os_spinlock_free(d, &q->lock);
}

void pq_reset(void *d, struct phl_queue *q, enum lock_type type)
{
	_os_spinlockfg sp_flags;

	_os_spinlock(d, &q->lock, type, &sp_flags);
	INIT_LIST_HEAD(&q->queue);
	q->cnt = 0;
	_os_spinunlock(d, &q->lock, type, &sp_flags);
}

u8 pq_push(void *d, struct phl_queue *q, _os_list *obj, u8 pos, enum lock_type type)
{
	_os_spinlockfg sp_flags;

	_os_spinlock(d, &q->lock, type, &sp_flags);
	if(pos == _first)
		list_add(obj, &q->queue);
	else
		list_add_tail(obj, &q->queue);
	q->cnt++;
	_os_spinunlock(d, &q->lock, type, &sp_flags);
	return true;
}

u8 pq_pop(void *d, struct phl_queue *q, _os_list **obj, u8 pos, enum lock_type type)
{
	_os_spinlockfg sp_flags = 0;

	(*obj) = NULL;
	_os_spinlock(d, &q->lock, type, &sp_flags);
	if(!list_empty(&q->queue) && (q->cnt > 0)) {
		if(pos == _first)
			(*obj) = _get_next(&q->queue);
		else
			(*obj) = _get_prev(&q->queue);
		list_del(*obj);
		q->cnt--;
	}
	_os_spinunlock(d, &q->lock, type, &sp_flags);

	return ((*obj) == NULL || (*obj) == &q->queue) ? (false) : (true);
}

u8 pq_get_front(void *d, struct phl_queue *q, _os_list **obj, enum lock_type type)
{
	_os_spinlockfg sp_flags = 0;

	(*obj) = NULL;

	_os_spinlock(d, &q->lock, type, &sp_flags);
	if(!list_empty(&q->queue) && (q->cnt > 0))
		(*obj) = q->queue.next;
	_os_spinunlock(d, &q->lock, type, &sp_flags);
	return ((*obj) == NULL || (*obj) == &q->queue) ? (false) : (true);
}

u8 pq_get_next(void *d, struct phl_queue *queue, _os_list *cur_obj,
	       _os_list **obj, enum lock_type type)
{
	_os_spinlockfg sp_flags;

	(*obj) = NULL;
	if(cur_obj == NULL)
		return false;
	_os_spinlock(d, &queue->lock, type, &sp_flags);
	(*obj) = cur_obj->next;
	_os_spinunlock(d, &queue->lock, type, &sp_flags);
	return ((*obj) == NULL || (*obj) == &(queue->queue)) ? (false) : (true);
}

u8 pq_get_tail(void *d, struct phl_queue *q, _os_list **obj, enum lock_type type)
{
	_os_spinlockfg sp_flags = 0;

	(*obj) = NULL;

	_os_spinlock(d, &q->lock, type, &sp_flags);
	if(!list_empty(&q->queue) && (q->cnt > 0))
		(*obj) = q->queue.prev;
	_os_spinunlock(d, &q->lock, type, &sp_flags);
	return ((*obj) == NULL || (*obj) == &q->queue) ? (false) : (true);
}

u8 pq_get_prev(void *d, struct phl_queue *queue, _os_list *cur_obj,
	       _os_list **obj, enum lock_type type)
{
	_os_spinlockfg sp_flags;

	(*obj) = NULL;
	if(cur_obj == NULL)
		return false;
	_os_spinlock(d, &queue->lock, type, &sp_flags);
	(*obj) = cur_obj->prev;
	_os_spinunlock(d, &queue->lock, type, &sp_flags);
	return ((*obj) == NULL || (*obj) == &(queue->queue)) ? (false) : (true);
}

u8 pq_search_node(void *d, struct phl_queue *q, _os_list **obj,
		  enum lock_type type, bool bdel, void *priv,
		  u8 (*search_fun)(void *d, void *obj, void *priv))
{
	_os_spinlockfg sp_flags = 0;
	_os_list *newobj = NULL;
	bool bhit = false;

	(*obj) = NULL;
	_os_spinlock(d, &q->lock, type, &sp_flags);

	if(!list_empty(&q->queue) && (q->cnt > 0))
		newobj = _get_next(&q->queue);

	while(newobj && (newobj != &(q->queue)))  {

		if(search_fun)
			bhit = search_fun(d, newobj, priv);

		if(bhit && bdel) {
			list_del(newobj);
			q->cnt--;
		}

		if(bhit) {
			(*obj) = newobj;
			break;
		}

		newobj = newobj->next;
	};
	_os_spinunlock(d, &q->lock, type, &sp_flags);

	return ((*obj) == NULL || (*obj) == &(q->queue)) ? (false) : (true);
}

void pq_del_node(void *d, struct phl_queue *q, _os_list *obj, enum lock_type type)
{
	_os_spinlockfg sp_flags;

	if(obj == NULL)
		return;
	_os_spinlock(d, &q->lock, type, &sp_flags);
	list_del(obj);
	q->cnt--;
	_os_spinunlock(d, &q->lock, type, &sp_flags);
}

u8 pq_insert(void *d, struct phl_queue *q, enum lock_type type, void *priv, _os_list *input,
		  u8 (*pq_predicate)(void *d, void *priv,_os_list *input, _os_list *obj))
{
	_os_spinlockfg sp_flags;
	_os_list *obj = NULL;

	_os_spinlock(d, &q->lock, type, &sp_flags);
	obj = q->queue.next;
	while (obj != &(q->queue)) {
		if (pq_predicate && (pq_predicate(d, priv, input, obj) == true))
			break;
		obj = obj->next;
	}
	list_add_tail(input, obj);
	q->cnt++;
	_os_spinunlock(d, &q->lock, type, &sp_flags);
	return true;
}
u32 phl_get_passing_time_us(u32 start)
{
	u32 now = _os_get_cur_time_us();
	u32 pass = 0;

	if (now == start)
		pass = 0;
	else if (now > start)
		/* -- start -- now -- */
		pass = now - start;
	else
		/* -- now -- start -- */
		pass = 0xffffffff - start + now;

	return pass;
}

u32 phl_get_passing_time_ms(u32 start)
{
	u32 now = _os_get_cur_time_ms();
	u32 pass = 0;

	if (now == start)
		pass = 0;
	else if (now > start)
		/* -- start -- now -- */
		pass = now - start;
	else
		/* -- now -- start -- */
		pass = 0xffffffff - start + now;

	return pass;
}

#ifdef DBG_MONITOR_TIME
void phl_fun_monitor_start(u32 *start_t, bool show_caller, const char *caller)
{
	*start_t = _os_get_cur_time_us();
	if (show_caller)
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">> %s:\n", caller);
}

void phl_fun_monitor_end(u32 *start_t, const char *caller)
{
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<< %s: Process time(us): %d\n",
		caller, phl_get_passing_time_us(*start_t));
}
#endif /* DBG_MONITOR_TIME */