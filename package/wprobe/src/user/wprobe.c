/*
 * wprobe.c: Wireless probe user space library
 * Copyright (C) 2008-2009 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <linux/wprobe.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include "wprobe.h"

#define DEBUG 1
#ifdef DEBUG
#define DPRINTF(fmt, ...) fprintf(stderr, "%s(%d): " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define DPRINTF(fmt, ...) do {} while (0)
#endif

static struct nl_sock *handle = NULL;
static struct nl_cache *cache = NULL;
static struct genl_family *family = NULL;
static struct nlattr *tb[WPROBE_ATTR_LAST+1];
static struct nla_policy attribute_policy[WPROBE_ATTR_LAST+1] = {
	[WPROBE_ATTR_ID] = { .type = NLA_U32 },
	[WPROBE_ATTR_MAC] = { .type = NLA_UNSPEC, .minlen = 6, .maxlen = 6 },
	[WPROBE_ATTR_NAME] = { .type = NLA_STRING },
	[WPROBE_ATTR_FLAGS] = { .type = NLA_U32 },
	[WPROBE_ATTR_TYPE] = { .type = NLA_U8 },
	[WPROBE_VAL_S8] = { .type = NLA_U8 },
	[WPROBE_VAL_S16] = { .type = NLA_U16 },
	[WPROBE_VAL_S32] = { .type = NLA_U32 },
	[WPROBE_VAL_S64] = { .type = NLA_U64 },
	[WPROBE_VAL_U8] = { .type = NLA_U8 },
	[WPROBE_VAL_U16] = { .type = NLA_U16 },
	[WPROBE_VAL_U32] = { .type = NLA_U32 },
	[WPROBE_VAL_U64] = { .type = NLA_U64 },
	[WPROBE_VAL_SUM] = { .type = NLA_U64 },
	[WPROBE_VAL_SUM_SQ] = { .type = NLA_U64 },
	[WPROBE_VAL_SAMPLES] = { .type = NLA_U32 },
};

static int
error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
	int *ret = arg;
	*ret = err->error;
	return NL_STOP;
}

static int
finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_SKIP;
}

static int
ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_STOP;
}


void
wprobe_free(void)
{
	if (cache)
		nl_cache_free(cache);
	if (handle)
		nl_socket_free(handle);
	handle = NULL;
	cache = NULL;
}

int
wprobe_init(void)
{
	int ret;

	handle = nl_socket_alloc();
	if (!handle) {
		DPRINTF("Failed to create handle\n");
		goto err;
	}

	if (genl_connect(handle)) {
		DPRINTF("Failed to connect to generic netlink\n");
		goto err;
	}

	ret = genl_ctrl_alloc_cache(handle, &cache);
	if (ret < 0) {
		DPRINTF("Failed to allocate netlink cache\n");
		goto err;
	}

	family = genl_ctrl_search_by_name(cache, "wprobe");
	if (!family) {
		DPRINTF("wprobe API not present\n");
		goto err;
	}
	return 0;

err:
	wprobe_free();
	return -EINVAL;
}


static struct nl_msg *
wprobe_new_msg(const char *ifname, int cmd, bool dump)
{
	struct nl_msg *msg;
	uint32_t flags = 0;

	msg = nlmsg_alloc();
	if (!msg)
		return NULL;

	if (dump)
		flags |= NLM_F_DUMP;

	genlmsg_put(msg, 0, 0, genl_family_get_id(family),
			0, flags, cmd, 0);

	NLA_PUT_STRING(msg, WPROBE_ATTR_INTERFACE, ifname);
nla_put_failure:
	return msg;
}

static int
wprobe_send_msg(struct nl_msg *msg, void *callback, void *arg)
{
	struct nl_cb *cb;
	int err = 0;

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb)
		goto out_no_cb;

	if (callback)
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, callback, arg);

	err = nl_send_auto_complete(handle, msg);
	if (err < 0)
		goto out;

	err = 1;

	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

	while (err > 0)
		nl_recvmsgs(handle, cb);

out:
	nl_cb_put(cb);
out_no_cb:
	nlmsg_free(msg);
	return err;
}

struct wprobe_attr_cb {
	struct list_head *list;
	char *addr;
};

static int
save_attribute_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	const char *name = "N/A";
	struct wprobe_attribute *attr;
	int type = 0;
	struct wprobe_attr_cb *cb = arg;

	nla_parse(tb, WPROBE_ATTR_LAST, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), attribute_policy);

	if (tb[WPROBE_ATTR_NAME])
		name = nla_data(tb[WPROBE_ATTR_NAME]);

	attr = malloc(sizeof(struct wprobe_attribute) + strlen(name) + 1);
	if (!attr)
		return -1;

	memset(attr, 0, sizeof(struct wprobe_attribute));

	if (tb[WPROBE_ATTR_ID])
		attr->id = nla_get_u32(tb[WPROBE_ATTR_ID]);

	if (tb[WPROBE_ATTR_MAC] && cb->addr)
		memcpy(cb->addr, nla_data(tb[WPROBE_ATTR_MAC]), 6);

	if (tb[WPROBE_ATTR_FLAGS])
		attr->flags = nla_get_u32(tb[WPROBE_ATTR_FLAGS]);

	if (tb[WPROBE_ATTR_TYPE])
		type = nla_get_u8(tb[WPROBE_ATTR_TYPE]);

	if ((type < WPROBE_VAL_STRING) ||
		(type > WPROBE_VAL_U64))
		type = 0;

	attr->type = type;
	strcpy(attr->name, name);
	INIT_LIST_HEAD(&attr->list);
	list_add(&attr->list, cb->list);
	return 0;
}


int
wprobe_dump_attributes(const char *ifname, bool link, struct list_head *list, char *addr)
{
	struct nl_msg *msg;
	struct wprobe_attr_cb cb;

	cb.list = list;
	cb.addr = addr;
	msg = wprobe_new_msg(ifname, WPROBE_CMD_GET_LIST, true);
	if (!msg)
		return -ENOMEM;

	if (link)
		NLA_PUT(msg, WPROBE_ATTR_MAC, 6, "\x00\x00\x00\x00\x00\x00");

	return wprobe_send_msg(msg, save_attribute_handler, &cb);

nla_put_failure:
	nlmsg_free(msg);
	return -EINVAL;
}

static struct wprobe_link *
get_link(struct list_head *list, const char *addr)
{
	struct wprobe_link *l;

	list_for_each_entry(l, list, list) {
		if (!memcmp(l->addr, addr, 6)) {
			list_del_init(&l->list);
			goto out;
		}
	}

	/* no previous link found, allocate a new one */
	l = malloc(sizeof(struct wprobe_link));
	if (!l)
		goto out;

	memset(l, 0, sizeof(struct wprobe_link));
	memcpy(l->addr, addr, sizeof(l->addr));
	INIT_LIST_HEAD(&l->list);

out:
	return l;
}

struct wprobe_save_cb {
	struct list_head *list;
	struct list_head old_list;
};

static int
save_link_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct wprobe_link *link;
	struct wprobe_save_cb *cb = arg;
	const char *addr;

	nla_parse(tb, WPROBE_ATTR_LAST, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), attribute_policy);

	if (!tb[WPROBE_ATTR_MAC] || (nla_len(tb[WPROBE_ATTR_MAC]) != 6))
		return -1;

	addr = nla_data(tb[WPROBE_ATTR_MAC]);
	link = get_link(&cb->old_list, addr);
	if (!link)
		return -1;

	if (tb[WPROBE_ATTR_FLAGS])
		link->flags = nla_get_u32(tb[WPROBE_ATTR_FLAGS]);

	list_add_tail(&link->list, cb->list);
	return 0;
}


int
wprobe_update_links(const char *ifname, struct list_head *list)
{
	struct wprobe_link *l, *tmp;
	struct nl_msg *msg;
	struct wprobe_save_cb cb;
	int err;

	INIT_LIST_HEAD(&cb.old_list);
	list_splice_init(list, &cb.old_list);
	cb.list = list;

	msg = wprobe_new_msg(ifname, WPROBE_CMD_GET_LINKS, true);
	if (!msg)
		return -ENOMEM;

	err = wprobe_send_msg(msg, save_link_handler, &cb);
	if (err < 0)
		return err;

	list_for_each_entry_safe(l, tmp, &cb.old_list, list) {
		list_del(&l->list);
		free(l);
	}

	return 0;
}

void
wprobe_measure(const char *ifname)
{
	struct nl_msg *msg;

	msg = wprobe_new_msg(ifname, WPROBE_CMD_MEASURE, false);
	if (!msg)
		return;

	wprobe_send_msg(msg, NULL, NULL);
}

struct wprobe_request_cb {
	struct list_head *list;
	struct list_head old_list;
	char *addr;
};

static int
save_attrdata_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct wprobe_request_cb *cb = arg;
	struct wprobe_attribute *attr;
	int type, id;

	nla_parse(tb, WPROBE_ATTR_LAST, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), attribute_policy);

	if (!tb[WPROBE_ATTR_ID])
		return -1;

	if (!tb[WPROBE_ATTR_TYPE])
		return -1;

	id = nla_get_u32(tb[WPROBE_ATTR_ID]);
	list_for_each_entry(attr, &cb->old_list, list) {
		if (attr->id == id)
			goto found;
	}
	/* not found */
	return -1;

found:
	list_del_init(&attr->list);

	type = nla_get_u8(tb[WPROBE_ATTR_TYPE]);
	if (type != attr->type) {
		DPRINTF("WARNING: type mismatch for %s attribute '%s' (%d != %d)\n",
			(cb->addr ? "link" : "global"),
			attr->name,
			type, attr->type);
		goto out;
	}

	if ((type < WPROBE_VAL_STRING) ||
		(type > WPROBE_VAL_U64))
		goto out;

	memset(&attr->val, 0, sizeof(attr->val));

#define HANDLE_INT_TYPE(_idx, _type) \
	case WPROBE_VAL_S##_type: \
	case WPROBE_VAL_U##_type: \
		attr->val.U##_type = nla_get_u##_type(tb[_idx]); \
		break

	switch(type) {
		HANDLE_INT_TYPE(type, 8);
		HANDLE_INT_TYPE(type, 16);
		HANDLE_INT_TYPE(type, 32);
		HANDLE_INT_TYPE(type, 64);
		case WPROBE_VAL_STRING:
			/* unimplemented */
			break;
	}
#undef HANDLE_TYPE

	if (attr->flags & WPROBE_F_KEEPSTAT) {
		if (tb[WPROBE_VAL_SUM])
			attr->val.s = nla_get_u64(tb[WPROBE_VAL_SUM]);

		if (tb[WPROBE_VAL_SUM_SQ])
			attr->val.ss = nla_get_u64(tb[WPROBE_VAL_SUM_SQ]);

		if (tb[WPROBE_VAL_SAMPLES])
			attr->val.n = nla_get_u32(tb[WPROBE_VAL_SAMPLES]);

		if (attr->val.n > 0) {
			float avg = ((float) attr->val.s) / attr->val.n;
			float stdev = sqrt((((float) attr->val.ss) / attr->val.n) - (avg * avg));
			attr->val.avg = avg;
			attr->val.stdev = stdev;
		}
	}

out:
	list_add_tail(&attr->list, cb->list);
	return 0;
}


int
wprobe_request_data(const char *ifname, struct list_head *attrs, const unsigned char *addr, int scale)
{
	struct wprobe_request_cb cb;
	struct nl_msg *msg;
	int err;

	msg = wprobe_new_msg(ifname, WPROBE_CMD_GET_INFO, true);
	if (!msg)
		return -ENOMEM;

	if (scale < 0)
		NLA_PUT_U32(msg, WPROBE_ATTR_FLAGS, WPROBE_F_RESET);
	else if (scale > 0)
		NLA_PUT_U32(msg, WPROBE_ATTR_SCALE, scale);

	if (addr)
		NLA_PUT(msg, WPROBE_ATTR_MAC, 6, addr);

nla_put_failure:
	INIT_LIST_HEAD(&cb.old_list);
	list_splice_init(attrs, &cb.old_list);
	cb.list = attrs;

	err = wprobe_send_msg(msg, save_attrdata_handler, &cb);
	list_splice(&cb.old_list, attrs->prev);
	return err;
}


