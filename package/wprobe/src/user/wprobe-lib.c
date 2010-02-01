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

#define _ISOC99_SOURCE
#define _BSD_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <linux/wprobe.h>
#include <netlink/netlink.h>
#include <netlink/attr.h>
#include <netlink/genl/genl.h>
#ifndef NO_LOCAL_ACCESS 
#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <endian.h>
#endif
#include "wprobe.h"

#define DEBUG 1
#ifdef DEBUG
#define DPRINTF(fmt, ...) fprintf(stderr, "%s(%d): " fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define DPRINTF(fmt, ...) do {} while (0)
#endif

#if defined(BYTE_ORDER) && !defined(__BYTE_ORDER)
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __BIG_ENDIAN BIG_ENDIAN
#define __BYTE_ORDER BYTE_ORDER
#endif

#ifndef __BYTE_ORDER
#error Unknown endian type
#endif

#define WPROBE_MAX_MSGLEN	65536

static inline __u16 __swab16(__u16 x)
{
	return x<<8 | x>>8;
}

static inline __u32 __swab32(__u32 x)
{
	return x<<24 | x>>24 |
		(x & (__u32)0x0000ff00UL)<<8 |
		(x & (__u32)0x00ff0000UL)>>8;
}

static inline __u64 __swab64(__u64 x)
{
	return x<<56 | x>>56 |
		(x & (__u64)0x000000000000ff00ULL)<<40 |
		(x & (__u64)0x0000000000ff0000ULL)<<24 |
		(x & (__u64)0x00000000ff000000ULL)<< 8 |
		(x & (__u64)0x000000ff00000000ULL)>> 8 |
		(x & (__u64)0x0000ff0000000000ULL)>>24 |
		(x & (__u64)0x00ff000000000000ULL)>>40;
}


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define SWAP16(var) var = __swab16(var)
#define SWAP32(var) var = __swab32(var)
#define SWAP64(var) var = __swab64(var)
#else
#define SWAP16(var) do {} while(0)
#define SWAP32(var) do {} while(0)
#define SWAP64(var) do {} while(0)
#endif

int wprobe_port = 17990;
static struct nlattr *tb[WPROBE_ATTR_LAST+1];
static struct nla_policy attribute_policy[WPROBE_ATTR_LAST+1] = {
	[WPROBE_ATTR_ID] = { .type = NLA_U32 },
	[WPROBE_ATTR_MAC] = { .type = NLA_UNSPEC, .minlen = 6, .maxlen = 6 },
	[WPROBE_ATTR_NAME] = { .type = NLA_STRING },
	[WPROBE_ATTR_FLAGS] = { .type = NLA_U32 },
	[WPROBE_ATTR_TYPE] = { .type = NLA_U8 },
	[WPROBE_ATTR_FLAGS] = { .type = NLA_U32 },
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
	[WPROBE_VAL_SCALE_TIME] = { .type = NLA_U64 },
	[WPROBE_ATTR_INTERVAL] = { .type = NLA_U64 },
	[WPROBE_ATTR_SAMPLES_MIN] = { .type = NLA_U32 },
	[WPROBE_ATTR_SAMPLES_MAX] = { .type = NLA_U32 },
	[WPROBE_ATTR_SAMPLES_SCALE_M] = { .type = NLA_U32 },
	[WPROBE_ATTR_SAMPLES_SCALE_D] = { .type = NLA_U32 },
	[WPROBE_ATTR_FILTER_GROUP] = { .type = NLA_NESTED },
	[WPROBE_ATTR_RXCOUNT] = { .type = NLA_U64 },
	[WPROBE_ATTR_TXCOUNT] = { .type = NLA_U64 },
};

typedef int (*wprobe_cb_t)(struct nl_msg *, void *);

struct wprobe_iface_ops {
	int (*send_msg)(struct wprobe_iface *dev, struct nl_msg *msg, wprobe_cb_t cb, void *arg);
	void (*free)(struct wprobe_iface *dev);
};

struct wprobe_attr_cb {
	struct list_head *list;
	char *addr;
};

#define WPROBE_MAGIC_STR "WPROBE"
struct wprobe_init_hdr {
	struct {
		char magic[sizeof(WPROBE_MAGIC_STR)];

		/* protocol version */
		uint8_t version;

		/* extra header length (unused for now) */
		uint16_t extra;
	} pre __attribute__((packed));
	union {
		struct {
			uint16_t genl_family;
		} v0 __attribute__((packed));
	};
} __attribute__((packed));

struct wprobe_msg_hdr {
	__u16 status;
	__u16 error;
	__u32 len;
};

enum wprobe_resp_status {
	WPROBE_MSG_DONE = 0,
	WPROBE_MSG_DATA = 1,
};

static inline void
wprobe_swap_msg_hdr(struct wprobe_msg_hdr *mhdr)
{
	SWAP16(mhdr->status);
	SWAP16(mhdr->error);
	SWAP32(mhdr->len);
}

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

static struct nl_msg *
wprobe_new_msg(struct wprobe_iface *dev, int cmd, bool dump)
{
	struct nl_msg *msg;
	uint32_t flags = 0;

	msg = nlmsg_alloc_size(65536);
	if (!msg)
		return NULL;

	if (dump)
		flags |= NLM_F_DUMP;

	genlmsg_put(msg, 0, 0, dev->genl_family,
			0, flags, cmd, 0);

	NLA_PUT_STRING(msg, WPROBE_ATTR_INTERFACE, dev->ifname);
nla_put_failure:
	return msg;
}


static int
dump_attributes(struct wprobe_iface *dev, bool link, struct list_head *list, char *addr)
{
	struct nl_msg *msg;
	struct wprobe_attr_cb cb;

	cb.list = list;
	cb.addr = addr;
	msg = wprobe_new_msg(dev, WPROBE_CMD_GET_LIST, true);
	if (!msg)
		return -ENOMEM;

	if (link)
		NLA_PUT(msg, WPROBE_ATTR_MAC, 6, "\x00\x00\x00\x00\x00\x00");

	return dev->ops->send_msg(dev, msg, save_attribute_handler, &cb);

nla_put_failure:
	nlmsg_free(msg);
	return -EINVAL;
}

static struct wprobe_iface *
wprobe_alloc_dev(void)
{
	struct wprobe_iface *dev;

	dev = malloc(sizeof(struct wprobe_iface));
	if (!dev)
		return NULL;

	memset(dev, 0, sizeof(struct wprobe_iface));

	dev->interval = -1;
	dev->scale_min = -1;
	dev->scale_max = -1;
	dev->scale_m = -1;
	dev->scale_d = -1;
	dev->sockfd = -1;

	INIT_LIST_HEAD(&dev->global_attr);
	INIT_LIST_HEAD(&dev->link_attr);
	INIT_LIST_HEAD(&dev->links);
	return dev;
}

static int
wprobe_init_dev(struct wprobe_iface *dev)
{
	dump_attributes(dev, false, &dev->global_attr, NULL);
	dump_attributes(dev, true, &dev->link_attr, NULL);
	return 0;
}

#ifndef NO_LOCAL_ACCESS 
static int n_devs = 0;
static struct nl_sock *handle = NULL;
static struct nl_cache *cache = NULL;
static struct genl_family *family = NULL;

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

static void
wprobe_local_free(struct wprobe_iface *dev)
{
	/* should not happen */
	if (n_devs == 0)
		return;

	if (--n_devs != 0)
		return;

	if (cache)
		nl_cache_free(cache);
	if (handle)
		nl_socket_free(handle);
	handle = NULL;
	cache = NULL;
}

static int
wprobe_local_init(void)
{
	int ret;

	if (n_devs++ > 0)
		return 0;

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
	wprobe_local_free(NULL);
	return -EINVAL;
}


static int
wprobe_local_send_msg(struct wprobe_iface *dev, struct nl_msg *msg, wprobe_cb_t callback, void *arg)
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

static const struct wprobe_iface_ops wprobe_local_ops = {
	.send_msg = wprobe_local_send_msg,
	.free = wprobe_local_free,
};

struct wprobe_iface *
wprobe_get_dev(const char *ifname)
{
	struct wprobe_iface *dev;

	if (wprobe_local_init() != 0)
		return NULL;

	dev = wprobe_alloc_dev();
	if (!dev)
		goto error_alloc;

	dev->ifname = strdup(ifname);
	dev->ops = &wprobe_local_ops;
	dev->genl_family = genl_family_get_id(family);

	if (wprobe_init_dev(dev) < 0)
		goto error;

	return dev;

error:
	free(dev);
error_alloc:
	wprobe_local_free(NULL);
	return NULL;
}

#endif

static void swap_nlmsghdr(struct nlmsghdr *nlh)
{
	SWAP32(nlh->nlmsg_len);
	SWAP16(nlh->nlmsg_type);
	SWAP16(nlh->nlmsg_flags);
	SWAP32(nlh->nlmsg_seq);
	SWAP32(nlh->nlmsg_pid);
}

static void swap_genlmsghdr(struct genlmsghdr *gnlh)
{
#if 0 /* probably unnecessary */
	SWAP16(gnlh->reserved);
#endif
}

static void
wprobe_swap_nested(void *data, int len, bool outgoing)
{
	void *end = data + len;

	while (data < end) {
		struct nlattr *nla = data;
		unsigned int type, len;

		if (!outgoing) {
			SWAP16(nla->nla_len);
			SWAP16(nla->nla_type);

			/* required for further sanity checks */
			if (data + nla->nla_len > end)
				nla->nla_len = end - data;
		}

		len = NLA_ALIGN(nla->nla_len);
		type = nla->nla_type & NLA_TYPE_MASK;

		if (type <= WPROBE_ATTR_LAST) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
			switch(attribute_policy[type].type) {
			case NLA_U16:
				SWAP16(*(__u16 *)nla_data(nla));
				break;
			case NLA_U32:
				SWAP32(*(__u32 *)nla_data(nla));
				break;
			case NLA_U64:
				SWAP64(*(__u64 *)nla_data(nla));
				break;
			case NLA_NESTED:
				wprobe_swap_nested(nla_data(nla), nla_len(nla), outgoing);
				break;
			}
#endif
		}
		data += len;

		if (outgoing) {
			SWAP16(nla->nla_len);
			SWAP16(nla->nla_type);
		}
		if (!nla->nla_len)
			break;
	}
}

static struct nl_msg *
wprobe_msg_from_network(int socket, int len)
{
	struct genlmsghdr *gnlh;
	struct nlmsghdr *nlh;
	struct nl_msg *msg;
	void *data;

	msg = nlmsg_alloc_size(len + 32);
	if (!msg)
		return NULL;

	nlh = nlmsg_hdr(msg);
	if (read(socket, nlh, len) != len)
		goto free;

	swap_nlmsghdr(nlh);
	if (nlh->nlmsg_len > len)
		goto free;

	gnlh = nlmsg_data(nlh);
	swap_genlmsghdr(gnlh);

	data = genlmsg_data(gnlh);
	wprobe_swap_nested(data, genlmsg_len(gnlh), false);

	return msg;
free:
	nlmsg_free(msg);
	return NULL;
}

static int
wprobe_msg_to_network(int socket, struct nl_msg *msg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct wprobe_msg_hdr mhdr;
	struct genlmsghdr *gnlh;
	void *buf, *data;
	int buflen, datalen;
	int ret;

	buflen = nlh->nlmsg_len;
	buf = malloc(buflen);
	if (!buf)
		return -ENOMEM;

	memset(&mhdr, 0, sizeof(mhdr));
	mhdr.status = WPROBE_MSG_DATA;
	mhdr.len = buflen;
	wprobe_swap_msg_hdr(&mhdr);
	ret = write(socket, &mhdr, sizeof(mhdr));
	if (ret < 0)
		goto out;

	memcpy(buf, nlh, buflen);
	nlh = buf;
	gnlh = nlmsg_data(nlh);
	data = genlmsg_data(gnlh);
	datalen = genlmsg_len(gnlh);

	wprobe_swap_nested(data, datalen, true);
	swap_genlmsghdr(gnlh);
	swap_nlmsghdr(nlh);
	ret = write(socket, buf, buflen);

out:
	free(buf);

	return ret;
}

static int
wprobe_remote_send_msg(struct wprobe_iface *dev, struct nl_msg *msg, wprobe_cb_t callback, void *arg)
{
	struct wprobe_msg_hdr mhdr;
	int msgs = 0;

	wprobe_msg_to_network(dev->sockfd, msg);
	nlmsg_free(msg);
	do {
		if (read(dev->sockfd, &mhdr, sizeof(mhdr)) != sizeof(mhdr)) {
			DPRINTF("Failed to read response header\n");
			return -1;
		}
		wprobe_swap_msg_hdr(&mhdr);

		switch(mhdr.status) {
		case WPROBE_MSG_DATA:
			if (mhdr.len > WPROBE_MAX_MSGLEN) {
				fprintf(stderr, "Invalid length in received response message.\n");
				exit(1);
			}

			msg = wprobe_msg_from_network(dev->sockfd, mhdr.len);
			if (!msg)
				return -EINVAL;

			msgs++;
			callback(msg, arg);
			nlmsg_free(msg);
			break;
		}
	} while (mhdr.status != WPROBE_MSG_DONE);

	if (mhdr.error)
		return -mhdr.error;
	else
		return msgs;
}


static void
wprobe_socket_dev_free(struct wprobe_iface *dev)
{
	if (dev->sockfd >= 0)
		close(dev->sockfd);
}

static const struct wprobe_iface_ops wprobe_remote_ops = {
	.send_msg = wprobe_remote_send_msg,
	.free = wprobe_socket_dev_free,
};


#ifndef NO_LOCAL_ACCESS 
int
wprobe_server_init(int socket)
{
	struct wprobe_init_hdr hdr;
	int ret;

	ret = wprobe_local_init();
	if (ret != 0)
		return ret;

	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.pre.magic, WPROBE_MAGIC_STR, sizeof(WPROBE_MAGIC_STR));
	hdr.pre.version = 0;
	hdr.v0.genl_family = genl_family_get_id(family);
	SWAP16(hdr.v0.genl_family);
	write(socket, (unsigned char *)&hdr, sizeof(hdr));

	return 0;
}

static int
wprobe_server_cb(struct nl_msg *msg, void *arg)
{
	int *socket = arg;
	int ret;

	ret = wprobe_msg_to_network(*socket, msg);
	if (ret > 0)
		ret = 0;

	return ret;
}


int
wprobe_server_handle(int socket)
{
	struct wprobe_msg_hdr mhdr;
	struct nl_msg *msg;
	int ret;

	ret = read(socket, &mhdr, sizeof(mhdr));
	if (ret != sizeof(mhdr)) {
		if (ret <= 0)
			return -1;

		DPRINTF("Failed to read request header\n");
		return -EINVAL;
	}
	wprobe_swap_msg_hdr(&mhdr);

	switch(mhdr.status) {
	case WPROBE_MSG_DATA:
		if (mhdr.len > WPROBE_MAX_MSGLEN) {
			DPRINTF("Invalid length in received response message.\n");
			return -EINVAL;
		}
		msg = wprobe_msg_from_network(socket, mhdr.len);
		break;
	default:
		DPRINTF("Invalid request header type\n");
		return -ENOENT;
	}

	if (!msg) {
		DPRINTF("Failed to get message\n");
		return -EINVAL;
	}

	ret = wprobe_local_send_msg(NULL, msg, wprobe_server_cb, &socket);

	memset(&mhdr, 0, sizeof(mhdr));
	mhdr.status = WPROBE_MSG_DONE;
	if (ret < 0)
		mhdr.error = (uint16_t) -ret;

	ret = write(socket, (unsigned char *)&mhdr, sizeof(mhdr));
	if (ret > 0)
		ret = 0;

	return ret;
}

void
wprobe_server_done(void)
{
	wprobe_local_free(NULL);
}
#endif

struct wprobe_iface *
wprobe_get_from_socket(int socket, const char *name)
{
	struct wprobe_iface *dev;
	struct wprobe_init_hdr hdr;

	dev = wprobe_alloc_dev();
	if (!dev)
		goto out;

	dev->ops = &wprobe_remote_ops;
	dev->sockfd = socket;
	dev->ifname = strdup(name);

	/* read version and header length */
	if (read(socket, &hdr.pre, sizeof(hdr.pre)) != sizeof(hdr.pre)) {
		DPRINTF("Could not read header\n");
		goto error;
	}

	/* magic not found */
	if (memcmp(hdr.pre.magic, WPROBE_MAGIC_STR, sizeof(hdr.pre.magic)) != 0) {
		DPRINTF("Magic does not match\n");
		goto error;
	}

	/* unsupported version */
	if (hdr.pre.version != 0) {
		DPRINTF("Protocol version does not match\n");
		goto error;
	}

	if (read(socket, &hdr.v0, sizeof(hdr.v0)) != sizeof(hdr.v0)) {
		DPRINTF("Could not read header data\n");
		goto error;
	}

	SWAP16(hdr.pre.extra);
	SWAP16(hdr.v0.genl_family);
	dev->genl_family = hdr.v0.genl_family;

	if (wprobe_init_dev(dev) < 0) {
		DPRINTF("Could not initialize device\n");
		goto error;
	}

out:
	return dev;

error:
	wprobe_free_dev(dev);
	return NULL;
}

struct wprobe_iface *
wprobe_get_auto(const char *arg, char **err)
{
	static struct sockaddr_in sa;
	static char errbuf[512];

	struct wprobe_iface *dev = NULL;
	struct hostent *h;
	char *devstr = strdup(arg);
	char *sep = NULL;
	int sock = -1;
	int len;

	if (err)
		*err = NULL;

	sep = strchr(devstr, ':');
	if (!sep) {
#ifndef NO_LOCAL_ACCESS 
		free(devstr);
		return wprobe_get_dev(arg);
#else
		if (err)
			*err = "Invalid argument";
		goto out;
#endif
	}

	*sep = 0;
	sep++;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		goto syserr;

	h = gethostbyname(devstr);
	if (!h) {
		sprintf(errbuf, "Host not found");
		goto out_err;
	}

	memcpy(&sa.sin_addr, h->h_addr, h->h_length);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(wprobe_port);
	if (connect(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0)
		goto syserr;

	dev = wprobe_get_from_socket(sock, sep);
	if (!dev) {
		sprintf(errbuf, "wprobe connection initialization failed");
		goto out_err;
	}
	goto out;

syserr:
	if (err) {
		strcpy(errbuf, "Connection failed: ");
		len = strlen(errbuf);
		strerror_r(errno, errbuf + len, sizeof(errbuf) - len - 1);
	}
out_err:
	if (err)
		*err = errbuf;
	if (sock >= 0)
		close(sock);
out:
	if (devstr)
		free(devstr);
	return dev;
}

static void
free_attr_list(struct list_head *list)
{
	struct wprobe_attribute *attr, *tmp;

	list_for_each_entry_safe(attr, tmp, list, list) {
		list_del(&attr->list);
		free(attr);
	}
}

void
wprobe_free_dev(struct wprobe_iface *dev)
{
	if (dev->ops->free)
		dev->ops->free(dev);
	free_attr_list(&dev->global_attr);
	free_attr_list(&dev->link_attr);
	free((void *)dev->ifname);
	free(dev);
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
wprobe_update_links(struct wprobe_iface *dev)
{
	struct wprobe_link *l, *tmp;
	struct nl_msg *msg;
	struct wprobe_save_cb cb;
	int err;

	INIT_LIST_HEAD(&cb.old_list);
	list_splice_init(&dev->links, &cb.old_list);
	cb.list = &dev->links;

	msg = wprobe_new_msg(dev, WPROBE_CMD_GET_LINKS, true);
	if (!msg)
		return -ENOMEM;

	err = dev->ops->send_msg(dev, msg, save_link_handler, &cb);
	if (err < 0)
		return err;

	list_for_each_entry_safe(l, tmp, &cb.old_list, list) {
		list_del(&l->list);
		free(l);
	}

	return 0;
}


struct wprobe_filter_data
{
	wprobe_filter_cb cb;
	void *arg;
	struct wprobe_filter_item *buf;
	int buflen;
};

static int
dump_filter_handler(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct wprobe_filter_data *data = arg;
	struct nlattr *p;
	const char *name;
	int count = 0;
	int len;

	nla_parse(tb, WPROBE_ATTR_LAST, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), attribute_policy);

	if (!tb[WPROBE_ATTR_NAME] || !tb[WPROBE_ATTR_FILTER_GROUP])
		return -1;

	name = nla_data(tb[WPROBE_ATTR_NAME]);
	nla_for_each_nested(p, tb[WPROBE_ATTR_FILTER_GROUP], len) {
		count++;
	}

	if (data->buflen < count) {
		if (data->buf)
			free(data->buf);
		data->buflen = count;
		data->buf = malloc(sizeof(struct wprobe_filter_item) * count);
		memset(data->buf, 0, sizeof(struct wprobe_filter_item) * count);
	}

	count = 0;
	nla_for_each_nested(p, tb[WPROBE_ATTR_FILTER_GROUP], len) {
		struct wprobe_filter_item *fi;

		nla_parse(tb, WPROBE_ATTR_LAST, nla_data(p),
				nla_len(p), attribute_policy);

		if (!tb[WPROBE_ATTR_NAME] || !tb[WPROBE_ATTR_RXCOUNT]
				|| !tb[WPROBE_ATTR_TXCOUNT])
			continue;

		fi = &data->buf[count++];
		strncpy(fi->name, nla_data(tb[WPROBE_ATTR_NAME]), sizeof(fi->name) - 1);
		fi->name[sizeof(fi->name) - 1] = 0;
		fi->rx = nla_get_u64(tb[WPROBE_ATTR_RXCOUNT]);
		fi->tx = nla_get_u64(tb[WPROBE_ATTR_TXCOUNT]);
	}
	data->cb(data->arg, name, data->buf, count);

	return 0;
}

int
wprobe_dump_filters(struct wprobe_iface *dev, wprobe_filter_cb cb, void *arg)
{
	struct wprobe_filter_data data;
	struct nl_msg *msg;
	int err;

	data.buf = 0;
	data.buflen = 0;
	data.cb = cb;
	data.arg = arg;

	msg = wprobe_new_msg(dev, WPROBE_CMD_GET_FILTER, true);
	if (!msg)
		return -ENOMEM;

	err = dev->ops->send_msg(dev, msg, dump_filter_handler, &data);
	if (err < 0)
		return err;

	return 0;
}

int
wprobe_apply_config(struct wprobe_iface *dev)
{
	struct nl_msg *msg;

	msg = wprobe_new_msg(dev, WPROBE_CMD_CONFIG, false);
	if (!msg)
		return -ENOMEM;

	if (dev->interval >= 0)
		NLA_PUT_MSECS(msg, WPROBE_ATTR_INTERVAL, dev->interval);

	if (dev->filter_len < 0) {
		NLA_PUT(msg, WPROBE_ATTR_FILTER, 0, NULL);
		dev->filter_len = 0;
	} else if (dev->filter && dev->filter_len > 0) {
		NLA_PUT(msg, WPROBE_ATTR_FILTER, dev->filter_len, dev->filter);
	}
	dev->filter = NULL;

	dev->ops->send_msg(dev, msg, NULL, NULL);
	return 0;

nla_put_failure:
	nlmsg_free(msg);
	return -ENOMEM;
}

int
wprobe_measure(struct wprobe_iface *dev)
{
	struct nl_msg *msg;

	msg = wprobe_new_msg(dev, WPROBE_CMD_MEASURE, false);
	if (!msg)
		return -ENOMEM;

	dev->ops->send_msg(dev, msg, NULL, NULL);
	return 0;
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
			if (isnan(stdev))
				stdev = 0.0f;
			if (isnan(avg))
				avg = 0.0f;
			attr->val.avg = avg;
			attr->val.stdev = stdev;
		}
	}

out:
	list_add_tail(&attr->list, cb->list);
	return 0;
}


int
wprobe_request_data(struct wprobe_iface *dev, const unsigned char *addr)
{
	struct wprobe_request_cb cb;
	struct list_head *attrs;
	struct nl_msg *msg;
	int err;

	msg = wprobe_new_msg(dev, WPROBE_CMD_GET_INFO, true);
	if (!msg)
		return -ENOMEM;

	if (addr) {
		attrs = &dev->link_attr;
		NLA_PUT(msg, WPROBE_ATTR_MAC, 6, addr);
	} else {
		attrs = &dev->global_attr;
	}

	INIT_LIST_HEAD(&cb.old_list);
	list_splice_init(attrs, &cb.old_list);
	cb.list = attrs;

	err = dev->ops->send_msg(dev, msg, save_attrdata_handler, &cb);
	list_splice(&cb.old_list, attrs->prev);
	return err;

nla_put_failure:
	nlmsg_free(msg);
	return -ENOMEM;
}


