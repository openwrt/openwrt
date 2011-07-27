/*
 * swlib.c: Switch configuration API (user space part)
 *
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/switch.h>
#include "swlib.h"
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>

//#define DEBUG 1
#ifdef DEBUG
#define DPRINTF(fmt, ...) fprintf(stderr, "%s(%d): " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define DPRINTF(fmt, ...) do {} while (0)
#endif

static struct nl_sock *handle;
static struct nl_cache *cache;
static struct genl_family *family;
static struct nlattr *tb[SWITCH_ATTR_MAX];
static int refcount = 0;

static struct nla_policy port_policy[] = {
	[SWITCH_PORT_ID] = { .type = NLA_U32 },
	[SWITCH_PORT_FLAG_TAGGED] = { .type = NLA_FLAG },
};

static inline void *
swlib_alloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (!ptr)
		goto done;
	memset(ptr, 0, size);

done:
	return ptr;
}

static int
wait_handler(struct nl_msg *msg, void *arg)
{
	int *finished = arg;

	*finished = 1;
	return NL_STOP;
}

/* helper function for performing netlink requests */
static int
swlib_call(int cmd, int (*call)(struct nl_msg *, void *),
		int (*data)(struct nl_msg *, void *), void *arg)
{
	struct nl_msg *msg;
	struct nl_cb *cb = NULL;
	int finished;
	int flags = 0;
	int err;

	msg = nlmsg_alloc();
	if (!msg) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}

	if (!data)
		flags |= NLM_F_DUMP;

	genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, genl_family_get_id(family), 0, flags, cmd, 0);
	if (data) {
		if (data(msg, arg) < 0)
			goto nla_put_failure;
	}

	cb = nl_cb_alloc(NL_CB_CUSTOM);
	if (!cb) {
		fprintf(stderr, "nl_cb_alloc failed.\n");
		exit(1);
	}

	err = nl_send_auto_complete(handle, msg);
	if (err < 0) {
		fprintf(stderr, "nl_send_auto_complete failed: %d\n", err);
		goto out;
	}

	finished = 0;

	if (call)
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, call, arg);

	if (data)
		nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, wait_handler, &finished);
	else
		nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, wait_handler, &finished);

	err = nl_recvmsgs(handle, cb);
	if (err < 0) {
		goto out;
	}

	if (!finished)
		err = nl_wait_for_ack(handle);

out:
	if (cb)
		nl_cb_put(cb);
nla_put_failure:
	nlmsg_free(msg);
	return err;
}

static int
send_attr(struct nl_msg *msg, void *arg)
{
	struct switch_val *val = arg;
	struct switch_attr *attr = val->attr;

	NLA_PUT_U32(msg, SWITCH_ATTR_ID, attr->dev->id);
	NLA_PUT_U32(msg, SWITCH_ATTR_OP_ID, attr->id);
	switch(attr->atype) {
	case SWLIB_ATTR_GROUP_PORT:
		NLA_PUT_U32(msg, SWITCH_ATTR_OP_PORT, val->port_vlan);
		break;
	case SWLIB_ATTR_GROUP_VLAN:
		NLA_PUT_U32(msg, SWITCH_ATTR_OP_VLAN, val->port_vlan);
		break;
	default:
		break;
	}

	return 0;

nla_put_failure:
	return -1;
}

static int
store_port_val(struct nl_msg *msg, struct nlattr *nla, struct switch_val *val)
{
	struct nlattr *p;
	int ports = val->attr->dev->ports;
	int err = 0;
	int remaining;

	if (!val->value.ports)
		val->value.ports = malloc(sizeof(struct switch_port) * ports);

	nla_for_each_nested(p, nla, remaining) {
		struct nlattr *tb[SWITCH_PORT_ATTR_MAX+1];
		struct switch_port *port;

		if (val->len >= ports)
			break;

		err = nla_parse_nested(tb, SWITCH_PORT_ATTR_MAX, p, port_policy);
		if (err < 0)
			goto out;

		if (!tb[SWITCH_PORT_ID])
			continue;

		port = &val->value.ports[val->len];
		port->id = nla_get_u32(tb[SWITCH_PORT_ID]);
		port->flags = 0;
		if (tb[SWITCH_PORT_FLAG_TAGGED])
			port->flags |= SWLIB_PORT_FLAG_TAGGED;

		val->len++;
	}

out:
	return err;
}

static int
store_val(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct switch_val *val = arg;
	struct switch_attr *attr = val->attr;

	if (!val)
		goto error;

	if (nla_parse(tb, SWITCH_ATTR_MAX - 1, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL) < 0) {
		goto error;
	}

	if (tb[SWITCH_ATTR_OP_VALUE_INT])
		val->value.i = nla_get_u32(tb[SWITCH_ATTR_OP_VALUE_INT]);
	else if (tb[SWITCH_ATTR_OP_VALUE_STR])
		val->value.s = strdup(nla_get_string(tb[SWITCH_ATTR_OP_VALUE_STR]));
	else if (tb[SWITCH_ATTR_OP_VALUE_PORTS])
		val->err = store_port_val(msg, tb[SWITCH_ATTR_OP_VALUE_PORTS], val);

	val->err = 0;
	return 0;

error:
	return NL_SKIP;
}

int
swlib_get_attr(struct switch_dev *dev, struct switch_attr *attr, struct switch_val *val)
{
	int cmd;
	int err;

	switch(attr->atype) {
	case SWLIB_ATTR_GROUP_GLOBAL:
		cmd = SWITCH_CMD_GET_GLOBAL;
		break;
	case SWLIB_ATTR_GROUP_PORT:
		cmd = SWITCH_CMD_GET_PORT;
		break;
	case SWLIB_ATTR_GROUP_VLAN:
		cmd = SWITCH_CMD_GET_VLAN;
		break;
	default:
		return -EINVAL;
	}

	memset(&val->value, 0, sizeof(val->value));
	val->len = 0;
	val->attr = attr;
	val->err = -EINVAL;
	err = swlib_call(cmd, store_val, send_attr, val);
	if (!err)
		err = val->err;

	return err;
}

static int
send_attr_ports(struct nl_msg *msg, struct switch_val *val)
{
	struct nlattr *n;
	int i;

	/* TODO implement multipart? */
	if (val->len == 0)
		goto done;
	n = nla_nest_start(msg, SWITCH_ATTR_OP_VALUE_PORTS);
	if (!n)
		goto nla_put_failure;
	for (i = 0; i < val->len; i++) {
		struct switch_port *port = &val->value.ports[i];
		struct nlattr *np;

		np = nla_nest_start(msg, SWITCH_ATTR_PORT);
		if (!np)
			goto nla_put_failure;

		NLA_PUT_U32(msg, SWITCH_PORT_ID, port->id);
		if (port->flags & SWLIB_PORT_FLAG_TAGGED)
			NLA_PUT_FLAG(msg, SWITCH_PORT_FLAG_TAGGED);

		nla_nest_end(msg, np);
	}
	nla_nest_end(msg, n);
done:
	return 0;

nla_put_failure:
	return -1;
}

static int
send_attr_val(struct nl_msg *msg, void *arg)
{
	struct switch_val *val = arg;
	struct switch_attr *attr = val->attr;

	if (send_attr(msg, arg))
		goto nla_put_failure;

	switch(attr->type) {
	case SWITCH_TYPE_NOVAL:
		break;
	case SWITCH_TYPE_INT:
		NLA_PUT_U32(msg, SWITCH_ATTR_OP_VALUE_INT, val->value.i);
		break;
	case SWITCH_TYPE_STRING:
		if (!val->value.s)
			goto nla_put_failure;
		NLA_PUT_STRING(msg, SWITCH_ATTR_OP_VALUE_STR, val->value.s);
		break;
	case SWITCH_TYPE_PORTS:
		if (send_attr_ports(msg, val) < 0)
			goto nla_put_failure;
		break;
	default:
		goto nla_put_failure;
	}
	return 0;

nla_put_failure:
	return -1;
}

int
swlib_set_attr(struct switch_dev *dev, struct switch_attr *attr, struct switch_val *val)
{
	int cmd;

	switch(attr->atype) {
	case SWLIB_ATTR_GROUP_GLOBAL:
		cmd = SWITCH_CMD_SET_GLOBAL;
		break;
	case SWLIB_ATTR_GROUP_PORT:
		cmd = SWITCH_CMD_SET_PORT;
		break;
	case SWLIB_ATTR_GROUP_VLAN:
		cmd = SWITCH_CMD_SET_VLAN;
		break;
	default:
		return -EINVAL;
	}

	val->attr = attr;
	return swlib_call(cmd, NULL, send_attr_val, val);
}

int swlib_set_attr_string(struct switch_dev *dev, struct switch_attr *a, int port_vlan, const char *str)
{
	struct switch_port *ports;
	struct switch_val val;
	char *ptr;

	memset(&val, 0, sizeof(val));
	val.port_vlan = port_vlan;
	switch(a->type) {
	case SWITCH_TYPE_INT:
		val.value.i = atoi(str);
		break;
	case SWITCH_TYPE_STRING:
		val.value.s = str;
		break;
	case SWITCH_TYPE_PORTS:
		ports = alloca(sizeof(struct switch_port) * dev->ports);
		memset(ports, 0, sizeof(struct switch_port) * dev->ports);
		val.len = 0;
		ptr = (char *)str;
		while(ptr && *ptr)
		{
			while(*ptr && isspace(*ptr))
				ptr++;

			if (!*ptr)
				break;

			if (!isdigit(*ptr))
				return -1;

			if (val.len >= dev->ports)
				return -1;

			ports[val.len].flags = 0;
			ports[val.len].id = strtoul(ptr, &ptr, 10);
			while(*ptr && !isspace(*ptr)) {
				if (*ptr == 't')
					ports[val.len].flags |= SWLIB_PORT_FLAG_TAGGED;
				else
					return -1;

				ptr++;
			}
			if (*ptr)
				ptr++;
			val.len++;
		}
		val.value.ports = ports;
		break;
	case SWITCH_TYPE_NOVAL:
		if (str && !strcmp(str, "0"))
			return 0;

		break;
	default:
		return -1;
	}
	return swlib_set_attr(dev, a, &val);
}


struct attrlist_arg {
	int id;
	int atype;
	struct switch_dev *dev;
	struct switch_attr *prev;
	struct switch_attr **head;
};

static int
add_id(struct nl_msg *msg, void *arg)
{
	struct attrlist_arg *l = arg;

	NLA_PUT_U32(msg, SWITCH_ATTR_ID, l->id);

	return 0;
nla_put_failure:
	return -1;
}

static int
add_attr(struct nl_msg *msg, void *ptr)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct attrlist_arg *arg = ptr;
	struct switch_attr *new;

	if (nla_parse(tb, SWITCH_ATTR_MAX - 1, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL) < 0)
		goto done;

	new = swlib_alloc(sizeof(struct switch_attr));
	if (!new)
		goto done;

	new->dev = arg->dev;
	new->atype = arg->atype;
	if (arg->prev) {
		arg->prev->next = new;
	} else {
		arg->prev = *arg->head;
	}
	*arg->head = new;
	arg->head = &new->next;

	if (tb[SWITCH_ATTR_OP_ID])
		new->id = nla_get_u32(tb[SWITCH_ATTR_OP_ID]);
	if (tb[SWITCH_ATTR_OP_TYPE])
		new->type = nla_get_u32(tb[SWITCH_ATTR_OP_TYPE]);
	if (tb[SWITCH_ATTR_OP_NAME])
		new->name = strdup(nla_get_string(tb[SWITCH_ATTR_OP_NAME]));
	if (tb[SWITCH_ATTR_OP_DESCRIPTION])
		new->description = strdup(nla_get_string(tb[SWITCH_ATTR_OP_DESCRIPTION]));

done:
	return NL_SKIP;
}

int
swlib_scan(struct switch_dev *dev)
{
	struct attrlist_arg arg;

	if (dev->ops || dev->port_ops || dev->vlan_ops)
		return 0;

	arg.atype = SWLIB_ATTR_GROUP_GLOBAL;
	arg.dev = dev;
	arg.id = dev->id;
	arg.prev = NULL;
	arg.head = &dev->ops;
	swlib_call(SWITCH_CMD_LIST_GLOBAL, add_attr, add_id, &arg);

	arg.atype = SWLIB_ATTR_GROUP_PORT;
	arg.prev = NULL;
	arg.head = &dev->port_ops;
	swlib_call(SWITCH_CMD_LIST_PORT, add_attr, add_id, &arg);

	arg.atype = SWLIB_ATTR_GROUP_VLAN;
	arg.prev = NULL;
	arg.head = &dev->vlan_ops;
	swlib_call(SWITCH_CMD_LIST_VLAN, add_attr, add_id, &arg);

	return 0;
}

struct switch_attr *swlib_lookup_attr(struct switch_dev *dev,
		enum swlib_attr_group atype, const char *name)
{
	struct switch_attr *head;

	if (!name || !dev)
		return NULL;

	switch(atype) {
	case SWLIB_ATTR_GROUP_GLOBAL:
		head = dev->ops;
		break;
	case SWLIB_ATTR_GROUP_PORT:
		head = dev->port_ops;
		break;
	case SWLIB_ATTR_GROUP_VLAN:
		head = dev->vlan_ops;
		break;
	}
	while(head) {
		if (!strcmp(name, head->name))
			return head;
		head = head->next;
	}

	return NULL;
}

static void
swlib_priv_free(void)
{
	if (cache)
		nl_cache_free(cache);
	if (handle)
		nl_socket_free(handle);
	handle = NULL;
	cache = NULL;
}

static int
swlib_priv_init(void)
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

	family = genl_ctrl_search_by_name(cache, "switch");
	if (!family) {
		DPRINTF("Switch API not present\n");
		goto err;
	}
	return 0;

err:
	swlib_priv_free();
	return -EINVAL;
}

struct swlib_scan_arg {
	const char *name;
	struct switch_dev *head;
	struct switch_dev *ptr;
};

static int
add_switch(struct nl_msg *msg, void *arg)
{
	struct swlib_scan_arg *sa = arg;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct switch_dev *dev;
	const char *name;
	const char *alias;

	if (nla_parse(tb, SWITCH_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL) < 0)
		goto done;

	if (!tb[SWITCH_ATTR_DEV_NAME])
		goto done;

	name = nla_get_string(tb[SWITCH_ATTR_DEV_NAME]);
	alias = nla_get_string(tb[SWITCH_ATTR_ALIAS]);

	if (sa->name && (strcmp(name, sa->name) != 0) && (strcmp(alias, sa->name) != 0))
		goto done;

	dev = swlib_alloc(sizeof(struct switch_dev));
	if (!dev)
		goto done;

	strncpy(dev->dev_name, name, IFNAMSIZ - 1);
	dev->alias = strdup(alias);
	if (tb[SWITCH_ATTR_ID])
		dev->id = nla_get_u32(tb[SWITCH_ATTR_ID]);
	if (tb[SWITCH_ATTR_NAME])
		dev->name = strdup(nla_get_string(tb[SWITCH_ATTR_NAME]));
	if (tb[SWITCH_ATTR_PORTS])
		dev->ports = nla_get_u32(tb[SWITCH_ATTR_PORTS]);
	if (tb[SWITCH_ATTR_VLANS])
		dev->vlans = nla_get_u32(tb[SWITCH_ATTR_VLANS]);
	if (tb[SWITCH_ATTR_CPU_PORT])
		dev->cpu_port = nla_get_u32(tb[SWITCH_ATTR_CPU_PORT]);

	if (!sa->head) {
		sa->head = dev;
		sa->ptr = dev;
	} else {
		sa->ptr->next = dev;
		sa->ptr = dev;
	}

	refcount++;
done:
	return NL_SKIP;
}


struct switch_dev *
swlib_connect(const char *name)
{
	struct swlib_scan_arg arg;
	int err;

	if (!refcount) {
		if (swlib_priv_init() < 0)
			return NULL;
	};

	arg.head = NULL;
	arg.ptr = NULL;
	arg.name = name;
	swlib_call(SWITCH_CMD_GET_SWITCH, add_switch, NULL, &arg);

	if (!refcount)
		swlib_priv_free();

	return arg.head;
}

static void
swlib_free_attributes(struct switch_attr **head)
{
	struct switch_attr *a = *head;
	struct switch_attr *next;

	while (a) {
		next = a->next;
		free(a);
		a = next;
	}
	*head = NULL;
}

void
swlib_free(struct switch_dev *dev)
{
	swlib_free_attributes(&dev->ops);
	swlib_free_attributes(&dev->port_ops);
	swlib_free_attributes(&dev->vlan_ops);
	free(dev);

	if (--refcount == 0)
		swlib_priv_free();
}

void
swlib_free_all(struct switch_dev *dev)
{
	struct switch_dev *p;

	while (dev) {
		p = dev->next;
		swlib_free(dev);
		dev = p;
	}
}
