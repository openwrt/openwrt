#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <libubus.h>

#include "genl.h"
#include "private.h"
#include "dpns_common_genl.h"

static void ubus_err_code_reply(struct ubus_context *ctx, struct ubus_request_data *req, int err)
{
	struct blob_buf b = { };
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "error", err);
	if (err < 0)
		blobmsg_add_string(&b, "message", strerror(-err));
	ubus_send_reply(ctx, req, b.head);
}

static int common_send_recv(void *sbuf, size_t slen)
{
	struct common_genl_resp *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_COMMON, sbuf, slen, (void **)&rbuf, &rlen, 1000);
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

/*common_debug_dump*/
static int common_debug_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct common_genl_msg msg_dump_common = {
		.method = DEBUG_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_COMMON, &msg_dump_common, sizeof(msg_dump_common));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

/*intf_table_add*/
enum add_intf_table_blob_id {
	BLOB_INTF_TABLE_VID,
	BLOB_INTF_TMP_PPPOE_EN,
	BLOB_INTF_TMP_TUNNEL_EN,
	BLOB_INTF_TMP_WAN_FLAG,
	BLOB_INTF_TABLE_SMAC,
	NUM_ADD_INTF_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy intf_add_blob_policy[NUM_ADD_INTF_TABLE_BLOB_IDS] = {
    [BLOB_INTF_TABLE_VID]       = { .name = "vid",          .type = BLOBMSG_TYPE_INT32},
    [BLOB_INTF_TMP_PPPOE_EN]    = { .name = "pppoe_en",     .type = BLOBMSG_TYPE_BOOL},
    [BLOB_INTF_TMP_TUNNEL_EN]   = { .name = "tunnel_en",    .type = BLOBMSG_TYPE_BOOL},
    [BLOB_INTF_TMP_WAN_FLAG]    = { .name = "wan_flag",     .type = BLOBMSG_TYPE_BOOL},
    [BLOB_INTF_TABLE_SMAC]      = { .name = "smac",         .type = BLOBMSG_TYPE_STRING},
};

static int intf_table_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ADD_INTF_TABLE_BLOB_IDS] = { };
	struct common_genl_msg msg_add_intf = {
		.method = INTF_ADD,
	};
	int ret;

	blobmsg_parse(intf_add_blob_policy, NUM_ADD_INTF_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_INTF_TABLE_VID])
		msg_add_intf.vid = blobmsg_get_u32(tb[BLOB_INTF_TABLE_VID]);

	if (tb[BLOB_INTF_TMP_PPPOE_EN])
		msg_add_intf.pppoe_en = blobmsg_get_bool(tb[BLOB_INTF_TMP_PPPOE_EN]);

	if (tb[BLOB_INTF_TMP_TUNNEL_EN])
		msg_add_intf.tunnel_en = blobmsg_get_bool(tb[BLOB_INTF_TMP_TUNNEL_EN]);

	if (tb[BLOB_INTF_TMP_WAN_FLAG])
		msg_add_intf.wan_flag = blobmsg_get_bool(tb[BLOB_INTF_TMP_WAN_FLAG]);

    if (tb[BLOB_INTF_TABLE_SMAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_INTF_TABLE_SMAC]));
		if (ret)
			return ret;
		msg_add_intf.smac = be64_to_cpu(mac_buf);
	}

	ret = common_send_recv(&msg_add_intf, sizeof(msg_add_intf));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*intf_table_del*/
enum del_intf_table_blob_id {
	BLOB_INTF_TABLE_INDEX,
	NUM_DEL_INTF_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy intf_del_blob_policy[NUM_DEL_INTF_TABLE_BLOB_IDS] = {
	[BLOB_INTF_TABLE_INDEX]		= { .name = "index",  	        .type = BLOBMSG_TYPE_INT32},
};

static int intf_table_del(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DEL_INTF_TABLE_BLOB_IDS] = { };
	struct common_genl_msg msg_del_intf = {
		.method = INTF_DEL,
	};
	int ret;

	blobmsg_parse(intf_del_blob_policy, NUM_DEL_INTF_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_INTF_TABLE_INDEX])
		msg_del_intf.index = blobmsg_get_u32(tb[BLOB_INTF_TABLE_INDEX]);

	ret = common_send_recv(&msg_del_intf, sizeof(msg_del_intf));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*iso_table_set*/
enum set_iso_table_blob_id {
	BLOB_ISO_IPORT_NUM,
	BLOB_ISO_PORT_BITMAP,
	BLOB_ISO_OFFLOAD_BITMAP,
	NUM_SET_ISO_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy iso_set_blob_policy[NUM_SET_ISO_TABLE_BLOB_IDS] = {
    [BLOB_ISO_IPORT_NUM]        = { .name = "iport_num",        .type = BLOBMSG_TYPE_INT32},
    [BLOB_ISO_PORT_BITMAP]      = { .name = "port_bitmap",      .type = BLOBMSG_TYPE_INT32},
    [BLOB_ISO_OFFLOAD_BITMAP]   = { .name = "offload_bitmap",   .type = BLOBMSG_TYPE_INT32},
};

static int iso_table_set(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_ISO_TABLE_BLOB_IDS] = { };
	struct common_genl_msg msg_set_iso = {
		.method = ISO_SET,
	};
	int ret;

	blobmsg_parse(iso_set_blob_policy, NUM_SET_ISO_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_ISO_IPORT_NUM])
		msg_set_iso.iport_num = blobmsg_get_u32(tb[BLOB_ISO_IPORT_NUM]);

	if (tb[BLOB_ISO_PORT_BITMAP])
		msg_set_iso.port_bitmap = blobmsg_get_u32(tb[BLOB_ISO_PORT_BITMAP]);

	if (tb[BLOB_ISO_OFFLOAD_BITMAP])
		msg_set_iso.offload_bitmap = blobmsg_get_u32(tb[BLOB_ISO_OFFLOAD_BITMAP]);

	ret = common_send_recv(&msg_set_iso, sizeof(msg_set_iso));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*log_table_set*/
enum set_log_table_blob_id {
	BLOB_LOG_MODULE_NUM,
	BLOB_LOG_LEVEL,
	NUM_SET_LOG_BLOB_IDS,
};

static const struct blobmsg_policy log_set_blob_policy[NUM_SET_LOG_BLOB_IDS] = {
	[BLOB_LOG_MODULE_NUM]		= { .name = "module_num",  	.type = BLOBMSG_TYPE_INT32},
	[BLOB_LOG_LEVEL]		= { .name = "log_level",  	.type = BLOBMSG_TYPE_INT32},
};

static int log_table_set(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_LOG_BLOB_IDS] = { };
	struct common_genl_msg msg_set_log = {
		.method = LOG_SET,
	};
	int ret;

	blobmsg_parse(log_set_blob_policy, NUM_SET_LOG_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_LOG_MODULE_NUM])
		msg_set_log.module_num = blobmsg_get_u32(tb[BLOB_LOG_MODULE_NUM]);

	if (tb[BLOB_LOG_LEVEL])
		msg_set_log.log_level = blobmsg_get_u32(tb[BLOB_LOG_LEVEL]);

	ret = common_send_recv(&msg_set_log, sizeof(msg_set_log));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*common_intf_dump*/
static int common_intf_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct common_genl_msg msg_dump_intf = {
		.method = INTF_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_COMMON, &msg_dump_intf, sizeof(msg_dump_intf));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}


/*common_iso_dump*/
enum dump_iso_table_blob_id {
	BLOB_ISO_DUMP_IPORT_NUM,
	NUM_DUMP_ISO_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy iso_dump_blob_policy[NUM_DUMP_ISO_TABLE_BLOB_IDS] = {
	[BLOB_ISO_DUMP_IPORT_NUM]		= { .name = "iport_num",  	.type = BLOBMSG_TYPE_INT32},
};

static int common_iso_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DUMP_ISO_TABLE_BLOB_IDS] = { };
	struct common_genl_msg msg_dump_iso = {
		.method = ISO_DUMP,
	};
	int ret;

	blobmsg_parse(iso_dump_blob_policy, NUM_DUMP_ISO_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_ISO_DUMP_IPORT_NUM])
		msg_dump_iso.iport_num = blobmsg_get_u32(tb[BLOB_ISO_DUMP_IPORT_NUM]);


	ret = common_send_recv(&msg_dump_iso, sizeof(msg_dump_iso));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*common_log_dump*/
static int common_log_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct common_genl_msg msg_dump_log = {
		.method = LOG_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_COMMON, &msg_dump_log, sizeof(msg_dump_log));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

/*common_mib_dump*/
static int common_mib_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct common_genl_msg msg_dump_mib = {
		.method = MIB_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_COMMON, &msg_dump_mib, sizeof(msg_dump_mib));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

/*common_dev_dump*/
static int common_dev_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct common_genl_msg msg_dump_dev = {
		.method = DEV_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_COMMON, &msg_dump_dev, sizeof(msg_dump_dev));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

/*common_mem_dump*/
static int common_mem_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct common_genl_msg msg_dump_mem = {
		.method = MEM_DUMP,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_COMMON, &msg_dump_mem, sizeof(msg_dump_mem));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static const struct ubus_method common_methods[] = {
    UBUS_METHOD_NOARG("debug_dump", common_debug_dump),
    UBUS_METHOD("intf_add",         intf_table_add,         intf_add_blob_policy),
    UBUS_METHOD("intf_del",         intf_table_del,         intf_del_blob_policy),
    UBUS_METHOD("iso_set",          iso_table_set,          iso_set_blob_policy),
    UBUS_METHOD("log_set",          log_table_set,          log_set_blob_policy),
    UBUS_METHOD("iso_dump",         common_iso_dump,        iso_dump_blob_policy),
    UBUS_METHOD_NOARG("intf_dump",  common_intf_dump),
    UBUS_METHOD_NOARG("log_dump",   common_log_dump),
    UBUS_METHOD_NOARG("mib_dump",   common_mib_dump),
    UBUS_METHOD_NOARG("dev_dump",   common_dev_dump),
    UBUS_METHOD_NOARG("mem_dump",   common_mem_dump),
};

static struct ubus_object_type common_object_type =
	UBUS_OBJECT_TYPE("common", common_methods);

static struct ubus_object common_object = {
	.name = "dpns.common",
	.type = &common_object_type,
	.methods = common_methods,
	.n_methods = ARRAY_SIZE(common_methods),
};

int common_init(void)
{
	return ubus_add_object(ubus_ctx_get(), &common_object);
}

int common_exit(void)
{
	return ubus_remove_object(ubus_ctx_get(), &common_object);
}
