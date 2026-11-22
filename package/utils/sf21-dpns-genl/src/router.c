#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <libubus.h>
#include <arpa/inet.h>

#include "genl.h"
#include "private.h"
#include "dpns_router_genl.h"

static void ubus_err_code_reply(struct ubus_context *ctx, struct ubus_request_data *req, int err)
{
	struct blob_buf b = { };
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "error", err);
	if (err < 0)
		blobmsg_add_string(&b, "message", strerror(-err));
	ubus_send_reply(ctx, req, b.head);
}

static int router_send_recv(void *sbuf, size_t slen)
{
	struct router_genl_resp *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_ROUTER, sbuf, slen, (void **)&rbuf, &rlen, 1000);
	if (ret)
		return ret;
	if (rlen < sizeof(*rbuf)) {
		free(rbuf);
		return -EINVAL;
	}
	ret = rbuf->err;
	free(rbuf);
	return ret;
}

/*router_table_dump*/
static int router_table_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct router_genl_msg msg_dump_router = {
		.method = ROUTER_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_ROUTER, &msg_dump_router, sizeof(msg_dump_router));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

/*add router table ipv4*/
enum add_router_table_blob_id {
	BLOB_ROUTER_IP,
	BLOB_ROUTER_PREFIX_LEN,
	BLOB_NEXT_HOP_PTR,
	BLOB_INTF_INDEX,
        BLOB_ROUTER_OVPORT,
        BLOB_ROUTER_OVID,
        BLOB_ROUTER_REQ_ID,
        BLOB_ROUTER_REQ_ADDR,
	NUM_ADD_ROUTER_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy router_add_blob_policy[NUM_ADD_ROUTER_TABLE_BLOB_IDS] = {
	[BLOB_ROUTER_IP]	        = { .name = "ip",	        .type = BLOBMSG_TYPE_STRING},
	[BLOB_ROUTER_PREFIX_LEN]	= { .name = "prefix_len",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NEXT_HOP_PTR]		= { .name = "next_hop_ptr",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_INTF_INDEX]		= { .name = "intf_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_OVPORT]    	= { .name = "ovport",	        .type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_OVID]		= { .name = "ovid",	        .type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_REQ_ID]		= { .name = "req_id",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_REQ_ADDR]		= { .name = "req_addr",		.type = BLOBMSG_TYPE_INT32},
};

static int router_table_add(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ADD_ROUTER_TABLE_BLOB_IDS] = { };
	struct router_genl_msg msg_add_router = {
		.method = ROUTER_TABLE_ADD,
	};
	struct sockaddr_in ip_v4 = { };
	int ret;

	blobmsg_parse(router_add_blob_policy, NUM_ADD_ROUTER_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	ip_v4.sin_family = AF_INET;
        inet_pton(AF_INET, blobmsg_get_string(tb[BLOB_ROUTER_IP]), &(ip_v4.sin_addr.s_addr));
	msg_add_router.ipaddr = ip_v4.sin_addr.s_addr;

	if (tb[BLOB_ROUTER_PREFIX_LEN])
		msg_add_router.prefix_len = blobmsg_get_u32(tb[BLOB_ROUTER_PREFIX_LEN]);

	if (tb[BLOB_NEXT_HOP_PTR])
		msg_add_router.next_hop_ptr = blobmsg_get_u32(tb[BLOB_NEXT_HOP_PTR]);

	if (tb[BLOB_INTF_INDEX])
		msg_add_router.intf_index = blobmsg_get_u32(tb[BLOB_INTF_INDEX]);

        if (tb[BLOB_ROUTER_OVPORT])
		msg_add_router.ovport = blobmsg_get_u32(tb[BLOB_ROUTER_OVPORT]);

	if (tb[BLOB_ROUTER_OVID])
		msg_add_router.ovid = blobmsg_get_u32(tb[BLOB_ROUTER_OVID]);

	if (tb[BLOB_ROUTER_REQ_ID])
		msg_add_router.req_id = blobmsg_get_u32(tb[BLOB_ROUTER_REQ_ID]);

	if (tb[BLOB_ROUTER_REQ_ADDR])
		msg_add_router.req_addr = blobmsg_get_u32(tb[BLOB_ROUTER_REQ_ADDR]);


	ret = router_send_recv(&msg_add_router, sizeof(msg_add_router));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*add router table ipv6*/
enum add_router_table_ipv6_blob_id {
	BLOB_ROUTER_IP_V6,
	BLOB_ROUTER_PREFIX_LEN_V6,
	BLOB_NEXT_HOP_PTR_V6,
	BLOB_INTF_INDEX_V6,
        BLOB_ROUTER_OVPORT_V6,
        BLOB_ROUTER_OVID_V6,
        BLOB_ROUTER_REQ_ID_V6,
        BLOB_ROUTER_REQ_ADDR_V6,
	NUM_ADD_ROUTER_TABLE_BLOB_IDS_V6,
};

static const struct blobmsg_policy router_add_ipv6_blob_policy[NUM_ADD_ROUTER_TABLE_BLOB_IDS_V6] = {
	[BLOB_ROUTER_IP_V6]	        = { .name = "ip",	        .type = BLOBMSG_TYPE_STRING},
	[BLOB_ROUTER_PREFIX_LEN_V6]	= { .name = "prefix_len",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NEXT_HOP_PTR_V6]		= { .name = "next_hop_ptr",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_INTF_INDEX_V6]		= { .name = "intf_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_OVPORT_V6]    	= { .name = "ovport",	        .type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_OVID_V6]		= { .name = "ovid",	        .type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_REQ_ID_V6]		= { .name = "req_id",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_ROUTER_REQ_ADDR_V6]	= { .name = "req_addr",		.type = BLOBMSG_TYPE_INT32},
};

static int router_table_add_v6(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ADD_ROUTER_TABLE_BLOB_IDS_V6] = { };
	struct router_genl_msg msg_add_router_v6 = {
		.method = ROUTER_TABLE_ADD_V6,
	};
	struct sockaddr_in6 ip_v6 = { };
	int ret;

	blobmsg_parse(router_add_blob_policy, NUM_ADD_ROUTER_TABLE_BLOB_IDS_V6, tb, blob_data(msg), blob_len(msg));

	ip_v6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, blobmsg_get_string(tb[BLOB_ROUTER_IP]), &(ip_v6.sin6_addr));
	memcpy(msg_add_router_v6.ipaddr6, &ip_v6.sin6_addr, sizeof(uint8_t) * 16);

	if (tb[BLOB_ROUTER_PREFIX_LEN_V6])
		msg_add_router_v6.prefix_len = blobmsg_get_u32(tb[BLOB_ROUTER_PREFIX_LEN_V6]);

	if (tb[BLOB_NEXT_HOP_PTR_V6])
		msg_add_router_v6.next_hop_ptr = blobmsg_get_u32(tb[BLOB_NEXT_HOP_PTR_V6]);

	if (tb[BLOB_INTF_INDEX_V6])
		msg_add_router_v6.intf_index = blobmsg_get_u32(tb[BLOB_INTF_INDEX_V6]);

        if (tb[BLOB_ROUTER_OVPORT_V6])
		msg_add_router_v6.ovport = blobmsg_get_u32(tb[BLOB_ROUTER_OVPORT_V6]);

	if (tb[BLOB_ROUTER_OVID_V6])
		msg_add_router_v6.ovid = blobmsg_get_u32(tb[BLOB_ROUTER_OVID_V6]);

	if (tb[BLOB_ROUTER_REQ_ID_V6])
		msg_add_router_v6.req_id = blobmsg_get_u32(tb[BLOB_ROUTER_REQ_ID_V6]);

	if (tb[BLOB_ROUTER_REQ_ADDR_V6])
		msg_add_router_v6.req_addr = blobmsg_get_u32(tb[BLOB_ROUTER_REQ_ADDR_V6]);


	ret = router_send_recv(&msg_add_router_v6, sizeof(msg_add_router_v6));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*del router table*/
enum del_router_table_blob_id {
        BLOB_DEL_ROUTER_REQ_ID,
        BLOB_DEL_ROUTER_REQ_ADDR,
	NUM_DEL_ROUTER_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy router_del_blob_policy[NUM_DEL_ROUTER_TABLE_BLOB_IDS] = {
	[BLOB_DEL_ROUTER_REQ_ID]	= { .name = "req_id",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_DEL_ROUTER_REQ_ADDR]	= { .name = "req_addr",		.type = BLOBMSG_TYPE_INT32},
};

static int router_table_del(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DEL_ROUTER_TABLE_BLOB_IDS] = { };
	struct router_genl_msg msg_del_router = {
		.method = ROUTER_TABLE_DEL,
	};
	int ret;

	blobmsg_parse(router_del_blob_policy, NUM_DEL_ROUTER_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));
	if (tb[BLOB_DEL_ROUTER_REQ_ID])
		msg_del_router.req_id = blobmsg_get_u32(tb[BLOB_DEL_ROUTER_REQ_ID]);

	if (tb[BLOB_DEL_ROUTER_REQ_ADDR])
		msg_del_router.req_addr = blobmsg_get_u32(tb[BLOB_DEL_ROUTER_REQ_ADDR]);

	ret = router_send_recv(&msg_del_router, sizeof(msg_del_router));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*del router table ipv6*/
enum del_router_table_ipv6_blob_id {
        BLOB_DEL_ROUTER_REQ_ID_V6,
        BLOB_DEL_ROUTER_REQ_ADDR_V6,
	NUM_DEL_ROUTER_TABLE_BLOB_IDS_V6,
};

static const struct blobmsg_policy router_del_ipv6_blob_policy[NUM_DEL_ROUTER_TABLE_BLOB_IDS_V6] = {
	[BLOB_DEL_ROUTER_REQ_ID_V6]	= { .name = "req_id",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_DEL_ROUTER_REQ_ADDR_V6]	= { .name = "req_addr",		.type = BLOBMSG_TYPE_INT32},
};

static int router_table_del_v6(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DEL_ROUTER_TABLE_BLOB_IDS_V6] = { };
	struct router_genl_msg msg_del_router_v6 = {
		.method = ROUTER_TABLE_DEL_V6,
	};
	int ret;

	blobmsg_parse(router_del_blob_policy, NUM_DEL_ROUTER_TABLE_BLOB_IDS_V6, tb, blob_data(msg), blob_len(msg));
	if (tb[BLOB_DEL_ROUTER_REQ_ID_V6])
		msg_del_router_v6.req_id = blobmsg_get_u32(tb[BLOB_DEL_ROUTER_REQ_ID_V6]);

	if (tb[BLOB_DEL_ROUTER_REQ_ADDR_V6])
		msg_del_router_v6.req_addr = blobmsg_get_u32(tb[BLOB_DEL_ROUTER_REQ_ADDR_V6]);

	ret = router_send_recv(&msg_del_router_v6, sizeof(msg_del_router_v6));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static const struct ubus_method router_methods[] = {
	UBUS_METHOD_NOARG("dump", 	router_table_dump),
	UBUS_METHOD("add", 	        router_table_add,	router_add_blob_policy),
	UBUS_METHOD("add6", 		router_table_add_v6,	router_add_ipv6_blob_policy),
	UBUS_METHOD("del",	        router_table_del,	router_del_blob_policy),
	UBUS_METHOD("del6",	        router_table_del_v6,	router_del_ipv6_blob_policy),
};

static struct ubus_object_type router_object_type =
	UBUS_OBJECT_TYPE("router", router_methods);

static struct ubus_object router_object = {
	.name = "dpns.router",
	.type = &router_object_type,
	.methods = router_methods,
	.n_methods = ARRAY_SIZE(router_methods),
};

int router_init(void)
{
	return ubus_add_object(ubus_ctx_get(), &router_object);
}

int router_exit(void)
{
	return ubus_remove_object(ubus_ctx_get(), &router_object);
}
