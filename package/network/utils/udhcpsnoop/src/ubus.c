// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */
#include <libubus.h>

#include "dhcpsnoop.h"

enum {
	DS_CONFIG_DEVICES,
	__DS_CONFIG_MAX
};

static const struct blobmsg_policy dhcpsnoop_config_policy[__DS_CONFIG_MAX] = {
	[DS_CONFIG_DEVICES] = { "devices", BLOBMSG_TYPE_TABLE },
};

static struct blob_buf b;

static int
dhcpsnoop_ubus_config(struct ubus_context *ctx, struct ubus_object *obj,
		   struct ubus_request_data *req, const char *method,
		   struct blob_attr *msg)
{
	struct blob_attr *tb[__DS_CONFIG_MAX];

	blobmsg_parse(dhcpsnoop_config_policy, __DS_CONFIG_MAX, tb,
		      blobmsg_data(msg), blobmsg_len(msg));

	dhcpsnoop_dev_config_update(tb[DS_CONFIG_DEVICES], false);

	dhcpsnoop_dev_check();

	return 0;
}


static int
dhcpsnoop_ubus_add_devices(struct ubus_context *ctx, struct ubus_object *obj,
		           struct ubus_request_data *req, const char *method,
		           struct blob_attr *msg)
{
	struct blob_attr *tb[__DS_CONFIG_MAX];

	blobmsg_parse(dhcpsnoop_config_policy, __DS_CONFIG_MAX, tb,
		      blobmsg_data(msg), blobmsg_len(msg));

	dhcpsnoop_dev_config_update(tb[DS_CONFIG_DEVICES], true);

	dhcpsnoop_dev_check();

	return 0;
}

static int
dhcpsnoop_ubus_check_devices(struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg)
{
	dhcpsnoop_dev_check();

	return 0;
}

static int
dhcpsnoop_ubus_dump(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	blob_buf_init(&b, 0);

	cache_dump(&b);

	ubus_send_reply(ctx, req, b.head);

	return 0;
}

static const struct ubus_method dhcpsnoop_methods[] = {
	UBUS_METHOD("config", dhcpsnoop_ubus_config, dhcpsnoop_config_policy),
	UBUS_METHOD("add_devices", dhcpsnoop_ubus_add_devices, dhcpsnoop_config_policy),
	UBUS_METHOD_NOARG("check_devices", dhcpsnoop_ubus_check_devices),
	UBUS_METHOD_NOARG("dump", dhcpsnoop_ubus_dump),
};

static struct ubus_object_type dhcpsnoop_object_type =
	UBUS_OBJECT_TYPE("dhcpsnoop", dhcpsnoop_methods);

static struct ubus_object dhcpsnoop_object = {
	.name = "dhcpsnoop",
	.type = &dhcpsnoop_object_type,
	.methods = dhcpsnoop_methods,
	.n_methods = ARRAY_SIZE(dhcpsnoop_methods),
};

static void
ubus_connect_handler(struct ubus_context *ctx)
{
	ubus_add_object(ctx, &dhcpsnoop_object);
}

static struct ubus_auto_conn conn;

void dhcpsnoop_ubus_init(void)
{
	conn.cb = ubus_connect_handler;
	ubus_auto_connect(&conn);
}

void dhcpsnoop_ubus_done(void)
{
	ubus_auto_shutdown(&conn);
	blob_buf_free(&b);
}

void dhcpsnoop_ubus_notify(const char *type, const uint8_t *msg, size_t len)
{
	char *buf;

	fprintf(stderr, "dhcp message type=%s\n", type);

	if (!dhcpsnoop_object.has_subscribers)
		return;

	blob_buf_init(&b, 0);
	buf = blobmsg_alloc_string_buffer(&b, "packet", 2 * len + 1);
	while (len > 0) {
		buf += sprintf(buf, "%02x", *msg);
		msg++;
		len--;
	}
	blobmsg_add_string_buffer(&b);

	ubus_notify(&conn.ctx, &dhcpsnoop_object, type, b.head, -1);
}
