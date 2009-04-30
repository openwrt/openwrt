/*
 * lib/cache.c		Caching Module
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation version 2.1
 *	of the License.
 *
 * Copyright (c) 2003-2008 Thomas Graf <tgraf@suug.ch>
 */

/**
 * @ingroup cache_mngt
 * @defgroup cache Cache
 *
 * @code
 *   Cache Management             |    | Type Specific Cache Operations
 *                                      
 *                                |    | +----------------+ +------------+
 *                                       | request update | | msg_parser |
 *                                |    | +----------------+ +------------+
 *                                     +- - - - -^- - - - - - - -^- -|- - - -
 *    nl_cache_update:            |              |               |   |
 *          1) --------- co_request_update ------+               |   |
 *                                |                              |   |
 *          2) destroy old cache     +----------- pp_cb ---------|---+
 *                                |  |                           |
 *          3) ---------- nl_recvmsgs ----------+   +- cb_valid -+
 *             +--------------+   |  |          |   |
 *             | nl_cache_add |<-----+   + - - -v- -|- - - - - - - - - - -
 *             +--------------+   |      | +-------------+
 *                                         | nl_recvmsgs |
 *                                |      | +-----|-^-----+
 *                                           +---v-|---+
 *                                |      |   | nl_recv |
 *                                           +---------+
 *                                |      |                 Core Netlink
 * @endcode
 * 
 * @{
 */

#include <netlink-local.h>
#include <netlink/netlink.h>
#include <netlink/cache.h>
#include <netlink/object.h>
#include <netlink/utils.h>

/**
 * @name Access Functions
 * @{
 */

#ifdef disabled
/**
 * Return the number of items in the cache
 * @arg cache		cache handle
 */
int nl_cache_nitems(struct nl_cache *cache)
{
	return cache->c_nitems;
}

/**
 * Return the number of items matching a filter in the cache
 * @arg cache		Cache object.
 * @arg filter		Filter object.
 */
int nl_cache_nitems_filter(struct nl_cache *cache, struct nl_object *filter)
{
	struct nl_object_ops *ops;
	struct nl_object *obj;
	int nitems = 0;

	if (cache->c_ops == NULL)
		BUG();

	ops = cache->c_ops->co_obj_ops;
	
	nl_list_for_each_entry(obj, &cache->c_items, ce_list) {
		if (filter && !nl_object_match_filter(obj, filter))
			continue;

		nitems++;
	}

	return nitems;
}

/**
 * Returns \b true if the cache is empty.
 * @arg cache		Cache to check
 * @return \a true if the cache is empty, otherwise \b false is returned.
 */
int nl_cache_is_empty(struct nl_cache *cache)
{
	return nl_list_empty(&cache->c_items);
}

/**
 * Return the operations set of the cache
 * @arg cache		cache handle
 */
struct nl_cache_ops *nl_cache_get_ops(struct nl_cache *cache)
{
	return cache->c_ops;
}

/**
 * Return the first element in the cache
 * @arg cache		cache handle
 */
struct nl_object *nl_cache_get_first(struct nl_cache *cache)
{
	if (nl_list_empty(&cache->c_items))
		return NULL;

	return nl_list_entry(cache->c_items.next,
			     struct nl_object, ce_list);
}

/**
 * Return the last element in the cache
 * @arg cache		cache handle
 */
struct nl_object *nl_cache_get_last(struct nl_cache *cache)
{
	if (nl_list_empty(&cache->c_items))
		return NULL;

	return nl_list_entry(cache->c_items.prev,
			     struct nl_object, ce_list);
}

/**
 * Return the next element in the cache
 * @arg obj		current object
 */
struct nl_object *nl_cache_get_next(struct nl_object *obj)
{
	if (nl_list_at_tail(obj, &obj->ce_cache->c_items, ce_list))
		return NULL;
	else
		return nl_list_entry(obj->ce_list.next,
				     struct nl_object, ce_list);
}

/**
 * Return the previous element in the cache
 * @arg obj		current object
 */
struct nl_object *nl_cache_get_prev(struct nl_object *obj)
{
	if (nl_list_at_head(obj, &obj->ce_cache->c_items, ce_list))
		return NULL;
	else
		return nl_list_entry(obj->ce_list.prev,
				     struct nl_object, ce_list);
}
#endif

/** @} */

/**
 * @name Cache Creation/Deletion
 * @{
 */

/**
 * Allocate an empty cache
 * @arg ops		cache operations to base the cache on
 * 
 * @return A newly allocated and initialized cache.
 */
struct nl_cache *nl_cache_alloc(struct nl_cache_ops *ops)
{
	struct nl_cache *cache;

	cache = calloc(1, sizeof(*cache));
	if (!cache)
		return NULL;

	nl_init_list_head(&cache->c_items);
	cache->c_ops = ops;

	NL_DBG(2, "Allocated cache %p <%s>.\n", cache, nl_cache_name(cache));

	return cache;
}

int nl_cache_alloc_and_fill(struct nl_cache_ops *ops, struct nl_sock *sock,
			    struct nl_cache **result)
{
	struct nl_cache *cache;
	int err;
	
	if (!(cache = nl_cache_alloc(ops)))
		return -NLE_NOMEM;

	if (sock && (err = nl_cache_refill(sock, cache)) < 0) {
		nl_cache_free(cache);
		return err;
	}

	*result = cache;
	return 0;
}

#ifdef disabled
/**
 * Allocate an empty cache based on type name
 * @arg kind		Name of cache type
 * @return A newly allocated and initialized cache.
 */
int nl_cache_alloc_name(const char *kind, struct nl_cache **result)
{
	struct nl_cache_ops *ops;
	struct nl_cache *cache;

	ops = nl_cache_ops_lookup(kind);
	if (!ops)
		return -NLE_NOCACHE;

	if (!(cache = nl_cache_alloc(ops)))
		return -NLE_NOMEM;

	*result = cache;
	return 0;
}

/**
 * Allocate a new cache containing a subset of a cache
 * @arg orig		Original cache to be based on
 * @arg filter		Filter defining the subset to be filled into new cache
 * @return A newly allocated cache or NULL.
 */
struct nl_cache *nl_cache_subset(struct nl_cache *orig,
				 struct nl_object *filter)
{
	struct nl_cache *cache;
	struct nl_object_ops *ops;
	struct nl_object *obj;

	if (!filter)
		BUG();

	cache = nl_cache_alloc(orig->c_ops);
	if (!cache)
		return NULL;

	ops = orig->c_ops->co_obj_ops;

	nl_list_for_each_entry(obj, &orig->c_items, ce_list) {
		if (!nl_object_match_filter(obj, filter))
			continue;

		nl_cache_add(cache, obj);
	}

	return cache;
}
#endif

/**
 * Clear a cache.
 * @arg cache		cache to clear
 *
 * Removes all elements of a cache.
 */
void nl_cache_clear(struct nl_cache *cache)
{
	struct nl_object *obj, *tmp;

	NL_DBG(1, "Clearing cache %p <%s>...\n", cache, nl_cache_name(cache));

	nl_list_for_each_entry_safe(obj, tmp, &cache->c_items, ce_list)
		nl_cache_remove(obj);
}

/**
 * Free a cache.
 * @arg cache		Cache to free.
 *
 * Removes all elements of a cache and frees all memory.
 *
 * @note Use this function if you are working with allocated caches.
 */
void nl_cache_free(struct nl_cache *cache)
{
	if (!cache)
		return;

	nl_cache_clear(cache);
	NL_DBG(1, "Freeing cache %p <%s>...\n", cache, nl_cache_name(cache));
	free(cache);
}

/** @} */

/**
 * @name Cache Modifications
 * @{
 */

static int __cache_add(struct nl_cache *cache, struct nl_object *obj)
{
	obj->ce_cache = cache;

	nl_list_add_tail(&obj->ce_list, &cache->c_items);
	cache->c_nitems++;

	NL_DBG(1, "Added %p to cache %p <%s>.\n",
	       obj, cache, nl_cache_name(cache));

	return 0;
}

/**
 * Add object to a cache.
 * @arg cache		Cache to add object to
 * @arg obj		Object to be added to the cache
 *
 * Adds the given object to the specified cache. The object is cloned
 * if it has been added to another cache already.
 *
 * @return 0 or a negative error code.
 */
int nl_cache_add(struct nl_cache *cache, struct nl_object *obj)
{
	struct nl_object *new;

	if (cache->c_ops->co_obj_ops != obj->ce_ops)
		return -NLE_OBJ_MISMATCH;

	if (!nl_list_empty(&obj->ce_list)) {
		new = nl_object_clone(obj);
		if (!new)
			return -NLE_NOMEM;
	} else {
		nl_object_get(obj);
		new = obj;
	}

	return __cache_add(cache, new);
}

#ifdef disabled
/**
 * Move object from one cache to another
 * @arg cache		Cache to move object to.
 * @arg obj		Object subject to be moved
 *
 * Removes the given object from its associated cache if needed
 * and adds it to the new cache.
 *
 * @return 0 on success or a negative error code.
 */
int nl_cache_move(struct nl_cache *cache, struct nl_object *obj)
{
	if (cache->c_ops->co_obj_ops != obj->ce_ops)
		return -NLE_OBJ_MISMATCH;

	NL_DBG(3, "Moving object %p to cache %p\n", obj, cache);
	
	/* Acquire reference, if already in a cache this will be
	 * reverted during removal */
	nl_object_get(obj);

	if (!nl_list_empty(&obj->ce_list))
		nl_cache_remove(obj);

	return __cache_add(cache, obj);
}
#endif

/**
 * Removes an object from a cache.
 * @arg obj		Object to remove from its cache
 *
 * Removes the object \c obj from the cache it is assigned to, since
 * an object can only be assigned to one cache at a time, the cache
 * must ne be passed along with it.
 */
void nl_cache_remove(struct nl_object *obj)
{
	struct nl_cache *cache = obj->ce_cache;

	if (cache == NULL)
		return;

	nl_list_del(&obj->ce_list);
	obj->ce_cache = NULL;
	nl_object_put(obj);
	cache->c_nitems--;

	NL_DBG(1, "Deleted %p from cache %p <%s>.\n",
	       obj, cache, nl_cache_name(cache));
}

#ifdef disabled
/**
 * Search for an object in a cache
 * @arg cache		Cache to search in.
 * @arg needle		Object to look for.
 *
 * Iterates over the cache and looks for an object with identical
 * identifiers as the needle.
 *
 * @return Reference to object or NULL if not found.
 * @note The returned object must be returned via nl_object_put().
 */
struct nl_object *nl_cache_search(struct nl_cache *cache,
				  struct nl_object *needle)
{
	struct nl_object *obj;

	nl_list_for_each_entry(obj, &cache->c_items, ce_list) {
		if (nl_object_identical(obj, needle)) {
			nl_object_get(obj);
			return obj;
		}
	}

	return NULL;
}
#endif

/** @} */

/**
 * @name Synchronization
 * @{
 */

/**
 * Request a full dump from the kernel to fill a cache
 * @arg sk		Netlink socket.
 * @arg cache		Cache subjected to be filled.
 *
 * Send a dumping request to the kernel causing it to dump all objects
 * related to the specified cache to the netlink socket.
 *
 * Use nl_cache_pickup() to read the objects from the socket and fill them
 * into a cache.
 */
int nl_cache_request_full_dump(struct nl_sock *sk, struct nl_cache *cache)
{
	NL_DBG(2, "Requesting dump from kernel for cache %p <%s>...\n",
	          cache, nl_cache_name(cache));

	if (cache->c_ops->co_request_update == NULL)
		return -NLE_OPNOTSUPP;

	return cache->c_ops->co_request_update(cache, sk);
}

/** @cond SKIP */
struct update_xdata {
	struct nl_cache_ops *ops;
	struct nl_parser_param *params;
};

static int update_msg_parser(struct nl_msg *msg, void *arg)
{
	struct update_xdata *x = arg;
	
	return nl_cache_parse(x->ops, &msg->nm_src, msg->nm_nlh, x->params);
}
/** @endcond */

int __cache_pickup(struct nl_sock *sk, struct nl_cache *cache,
		   struct nl_parser_param *param)
{
	int err;
	struct nl_cb *cb;
	struct update_xdata x = {
		.ops = cache->c_ops,
		.params = param,
	};

	NL_DBG(1, "Picking up answer for cache %p <%s>...\n",
		  cache, nl_cache_name(cache));

	cb = nl_cb_clone(sk->s_cb);
	if (cb == NULL)
		return -NLE_NOMEM;

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, update_msg_parser, &x);

	err = nl_recvmsgs(sk, cb);
	if (err < 0)
		NL_DBG(2, "While picking up for %p <%s>, recvmsgs() returned " \
		       "%d: %s", cache, nl_cache_name(cache),
		       err, nl_geterror(err));

	nl_cb_put(cb);

	return err;
}

static int pickup_cb(struct nl_object *c, struct nl_parser_param *p)
{
	return nl_cache_add((struct nl_cache *) p->pp_arg, c);
}

/**
 * Pickup a netlink dump response and put it into a cache.
 * @arg sk		Netlink socket.
 * @arg cache		Cache to put items into.
 *
 * Waits for netlink messages to arrive, parses them and puts them into
 * the specified cache.
 *
 * @return 0 on success or a negative error code.
 */
int nl_cache_pickup(struct nl_sock *sk, struct nl_cache *cache)
{
	struct nl_parser_param p = {
		.pp_cb = pickup_cb,
		.pp_arg = cache,
	};

	return __cache_pickup(sk, cache, &p);
}

#ifdef disabled
static int cache_include(struct nl_cache *cache, struct nl_object *obj,
			 struct nl_msgtype *type, change_func_t cb)
{
	struct nl_object *old;

	switch (type->mt_act) {
	case NL_ACT_NEW:
	case NL_ACT_DEL:
		old = nl_cache_search(cache, obj);
		if (old) {
			nl_cache_remove(old);
			if (type->mt_act == NL_ACT_DEL) {
				if (cb)
					cb(cache, old, NL_ACT_DEL);
				nl_object_put(old);
			}
		}

		if (type->mt_act == NL_ACT_NEW) {
			nl_cache_move(cache, obj);
			if (old == NULL && cb)
				cb(cache, obj, NL_ACT_NEW);
			else if (old) {
				if (nl_object_diff(old, obj) && cb)
					cb(cache, obj, NL_ACT_CHANGE);

				nl_object_put(old);
			}
		}
		break;
	default:
		NL_DBG(2, "Unknown action associated to object %p\n", obj);
		return 0;
	}

	return 0;
}

int nl_cache_include(struct nl_cache *cache, struct nl_object *obj,
		     change_func_t change_cb)
{
	struct nl_cache_ops *ops = cache->c_ops;
	int i;

	if (ops->co_obj_ops != obj->ce_ops)
		return -NLE_OBJ_MISMATCH;

	for (i = 0; ops->co_msgtypes[i].mt_id >= 0; i++)
		if (ops->co_msgtypes[i].mt_id == obj->ce_msgtype)
			return cache_include(cache, obj, &ops->co_msgtypes[i],
					     change_cb);

	return -NLE_MSGTYPE_NOSUPPORT;
}

static int resync_cb(struct nl_object *c, struct nl_parser_param *p)
{
	struct nl_cache_assoc *ca = p->pp_arg;

	return nl_cache_include(ca->ca_cache, c, ca->ca_change);
}

int nl_cache_resync(struct nl_sock *sk, struct nl_cache *cache,
		    change_func_t change_cb)
{
	struct nl_object *obj, *next;
	struct nl_cache_assoc ca = {
		.ca_cache = cache,
		.ca_change = change_cb,
	};
	struct nl_parser_param p = {
		.pp_cb = resync_cb,
		.pp_arg = &ca,
	};
	int err;

	NL_DBG(1, "Resyncing cache %p <%s>...\n", cache, nl_cache_name(cache));

	/* Mark all objects so we can see if some of them are obsolete */
	nl_cache_mark_all(cache);

	err = nl_cache_request_full_dump(sk, cache);
	if (err < 0)
		goto errout;

	err = __cache_pickup(sk, cache, &p);
	if (err < 0)
		goto errout;

	nl_list_for_each_entry_safe(obj, next, &cache->c_items, ce_list)
		if (nl_object_is_marked(obj))
			nl_cache_remove(obj);

	NL_DBG(1, "Finished resyncing %p <%s>\n", cache, nl_cache_name(cache));

	err = 0;
errout:
	return err;
}
#endif

/** @} */

/**
 * @name Parsing
 * @{
 */

/** @cond SKIP */
int nl_cache_parse(struct nl_cache_ops *ops, struct sockaddr_nl *who,
		   struct nlmsghdr *nlh, struct nl_parser_param *params)
{
	int i, err;

	if (!nlmsg_valid_hdr(nlh, ops->co_hdrsize))
		return -NLE_MSG_TOOSHORT;

	for (i = 0; ops->co_msgtypes[i].mt_id >= 0; i++) {
		if (ops->co_msgtypes[i].mt_id == nlh->nlmsg_type) {
			err = ops->co_msg_parser(ops, who, nlh, params);
			if (err != -NLE_OPNOTSUPP)
				goto errout;
		}
	}


	err = -NLE_MSGTYPE_NOSUPPORT;
errout:
	return err;
}
/** @endcond */

/**
 * Parse a netlink message and add it to the cache.
 * @arg cache		cache to add element to
 * @arg msg		netlink message
 *
 * Parses a netlink message by calling the cache specific message parser
 * and adds the new element to the cache.
 *
 * @return 0 or a negative error code.
 */
int nl_cache_parse_and_add(struct nl_cache *cache, struct nl_msg *msg)
{
	struct nl_parser_param p = {
		.pp_cb = pickup_cb,
		.pp_arg = cache,
	};

	return nl_cache_parse(cache->c_ops, NULL, nlmsg_hdr(msg), &p);
}

/**
 * (Re)fill a cache with the contents in the kernel.
 * @arg sk		Netlink socket.
 * @arg cache		cache to update
 *
 * Clears the specified cache and fills it with the current state in
 * the kernel.
 *
 * @return 0 or a negative error code.
 */
int nl_cache_refill(struct nl_sock *sk, struct nl_cache *cache)
{
	int err;

	err = nl_cache_request_full_dump(sk, cache);
	if (err < 0)
		return err;

	NL_DBG(2, "Upading cache %p <%s>, request sent, waiting for dump...\n",
	       cache, nl_cache_name(cache));
	nl_cache_clear(cache);

	return nl_cache_pickup(sk, cache);
}

/** @} */
#ifdef disabled

/**
 * @name Utillities
 * @{
 */

/**
 * Mark all objects in a cache
 * @arg cache		Cache to mark all objects in
 */
void nl_cache_mark_all(struct nl_cache *cache)
{
	struct nl_object *obj;

	NL_DBG(2, "Marking all objects in cache %p <%s>...\n",
	          cache, nl_cache_name(cache));

	nl_list_for_each_entry(obj, &cache->c_items, ce_list)
		nl_object_mark(obj);
}

/** @} */

/**
 * @name Dumping
 * @{
 */
#ifdef disabled
/**
 * Dump all elements of a cache.
 * @arg cache		cache to dump
 * @arg params		dumping parameters
 *
 * Dumps all elements of the \a cache to the file descriptor \a fd.
 */
void nl_cache_dump(struct nl_cache *cache, struct nl_dump_params *params)
{
	nl_cache_dump_filter(cache, params, NULL);
}

/**
 * Dump all elements of a cache (filtered).
 * @arg cache		cache to dump
 * @arg params		dumping parameters (optional)
 * @arg filter		filter object
 *
 * Dumps all elements of the \a cache to the file descriptor \a fd
 * given they match the given filter \a filter.
 */
void nl_cache_dump_filter(struct nl_cache *cache,
			  struct nl_dump_params *params,
			  struct nl_object *filter)
{
	int type = params ? params->dp_type : NL_DUMP_DETAILS;
	struct nl_object_ops *ops;
	struct nl_object *obj;

	NL_DBG(2, "Dumping cache %p <%s> filter %p\n",
	       cache, nl_cache_name(cache), filter);

	if (type > NL_DUMP_MAX || type < 0)
		BUG();

	if (cache->c_ops == NULL)
		BUG();

	ops = cache->c_ops->co_obj_ops;
	if (!ops->oo_dump[type])
		return;

	nl_list_for_each_entry(obj, &cache->c_items, ce_list) {
		if (filter && !nl_object_match_filter(obj, filter))
			continue;

		NL_DBG(4, "Dumping object %p...\n", obj);
		dump_from_ops(obj, params);
	}
}
#endif

/** @} */

/**
 * @name Iterators
 * @{
 */

/**
 * Call a callback on each element of the cache.
 * @arg cache		cache to iterate on
 * @arg cb		callback function
 * @arg arg		argument passed to callback function
 *
 * Calls a callback function \a cb on each element of the \a cache.
 * The argument \a arg is passed on the callback function.
 */
void nl_cache_foreach(struct nl_cache *cache,
		      void (*cb)(struct nl_object *, void *), void *arg)
{
	nl_cache_foreach_filter(cache, NULL, cb, arg);
}

/**
 * Call a callback on each element of the cache (filtered).
 * @arg cache		cache to iterate on
 * @arg filter		filter object
 * @arg cb		callback function
 * @arg arg		argument passed to callback function
 *
 * Calls a callback function \a cb on each element of the \a cache
 * that matches the \a filter. The argument \a arg is passed on
 * to the callback function.
 */
void nl_cache_foreach_filter(struct nl_cache *cache, struct nl_object *filter,
			     void (*cb)(struct nl_object *, void *), void *arg)
{
	struct nl_object *obj, *tmp;
	struct nl_object_ops *ops;

	if (cache->c_ops == NULL)
		BUG();

	ops = cache->c_ops->co_obj_ops;

	nl_list_for_each_entry_safe(obj, tmp, &cache->c_items, ce_list) {
		if (filter && !nl_object_match_filter(obj, filter))
			continue;

		cb(obj, arg);
	}
}

/** @} */
#endif

/** @} */
