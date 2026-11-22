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
#include "dpns_vlan_genl.h"

static void ubus_err_code_reply(struct ubus_context *ctx, struct ubus_request_data *req, int err)
{
	struct blob_buf b = { };
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "error", err);
	if (err < 0)
		blobmsg_add_string(&b, "message", strerror(-err));
	ubus_send_reply(ctx, req, b.head);
}

static int vlan_send_recv(void *sbuf, size_t slen)
{
	struct vlan_genl_resp *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_VLAN, sbuf, slen, (void **)&rbuf, &rlen, 1000);
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

static int table_name_to_index(char *table_name)
{
	int table_name_index;

	if (!strncmp(table_name, "iport", strlen("iport")))
		table_name_index = IPORT;
	else if (!strncmp(table_name, "ivlan_pbv", strlen("ivlan_pbv")))
		table_name_index = IVLAN_PBV;
	else if (!strncmp(table_name, "ivlan_lkp", strlen("ivlan_lkp")))
		table_name_index = IVLAN_LKP;
	else if (!strncmp(table_name, "ivlan_xlt", strlen("ivlan_xlt")))
		table_name_index = IVALN_XLT;
	else if (!strncmp(table_name, "ivlan_spl", strlen("ivlan_spl")))
		table_name_index = IVLAN_SPL;
	else if (!strncmp(table_name, "evlan_lkp", strlen("evlan_lkp")))
		table_name_index = EVLAN_LKP;
	else if (!strncmp(table_name, "evlan_act", strlen("evlan_act")))
		table_name_index = EVLAN_ACT;
	else if (!strncmp(table_name, "evlan_xlt", strlen("evlan_xlt")))
		table_name_index = EVLAN_XLT;
	else if (!strncmp(table_name, "evlan_ptpid", strlen("evlan_ptpid")))
		table_name_index = EVLAN_PTPID;
	else if (!strncmp(table_name, "evlan_otpid", strlen("evlan_otpid")))
		table_name_index = EVLAN_OTPID;
	else
		table_name_index = -1;

	return table_name_index;
}

/*vlan_table_dump*/
enum dump_vlan_table_blob_id {
	BLOB_VLAN_TABLE_NAME,
	BLOB_VLAN_TABLE_INDEX,
	NUM_DUMP_VLAN_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy table_dump_blob_policy[NUM_DUMP_VLAN_TABLE_BLOB_IDS] = {
	[BLOB_VLAN_TABLE_NAME]		= { .name = "table_name",  	.type = BLOBMSG_TYPE_STRING},
	[BLOB_VLAN_TABLE_INDEX]		= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
};

static int vlan_table_dump(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DUMP_VLAN_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_dump_vlan = {
		.method = VLAN_TABLE_DUMP,
	};
	int ret;
	int table_name_index;

	blobmsg_parse(table_dump_blob_policy, NUM_DUMP_VLAN_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_VLAN_TABLE_NAME])
		return UBUS_STATUS_INVALID_ARGUMENT;

	table_name_index = table_name_to_index(blobmsg_get_string(tb[BLOB_VLAN_TABLE_NAME]));

	if (table_name_index == -1)
		return UBUS_STATUS_INVALID_ARGUMENT;

	msg_dump_vlan.table_name_index = table_name_index;

	if (tb[BLOB_VLAN_TABLE_INDEX])
		msg_dump_vlan.table_index = blobmsg_get_u32(tb[BLOB_VLAN_TABLE_INDEX]);


	ret = vlan_send_recv(&msg_dump_vlan, sizeof(msg_dump_vlan));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set iport table*/
enum set_iport_table_blob_id {
	BLOB_IPORT_TABLE_INDEX,
	BLOB_DEFAULT_PORT,
	BLOB_ACTION,
	BLOB_VALID,
	NUM_SET_IPORT_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy iport_set_blob_policy[NUM_SET_IPORT_TABLE_BLOB_IDS] = {
	[BLOB_IPORT_TABLE_INDEX]	= { .name = "index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_DEFAULT_PORT]		= { .name = "default_port",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_ACTION]			= { .name = "action",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_VALID]			= { .name = "valid",		.type = BLOBMSG_TYPE_INT32},
};

static int iport_table_set(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_IPORT_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_iport = {
		.method = SET_IPORT,
	};
	int ret;

	blobmsg_parse(iport_set_blob_policy, NUM_SET_IPORT_TABLE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));
	if (tb[BLOB_IPORT_TABLE_INDEX])
		msg_set_iport.iport_num = blobmsg_get_u32(tb[BLOB_IPORT_TABLE_INDEX]);

	if (tb[BLOB_DEFAULT_PORT])
		msg_set_iport.default_port = blobmsg_get_u32(tb[BLOB_DEFAULT_PORT]);

	if (tb[BLOB_ACTION])
		msg_set_iport.action = blobmsg_get_u32(tb[BLOB_ACTION]);

	if (tb[BLOB_VALID])
		msg_set_iport.valid = blobmsg_get_u32(tb[BLOB_VALID]);

	ret = vlan_send_recv(&msg_set_iport, sizeof(msg_set_iport));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}


/*set ivlan_pbv table*/
enum set_ivlan_pbv_table_blob_id {
	BLOB_IVLAN_PBV_TABLE_INDEX,
	BLOB_IVID,
	BLOB_OVID,
	BLOB_IVLAN_PBV_VALID,
	BLOB_DT_OTAG,
	BLOB_DT_POTAG,
	BLOB_SOT_OTAG,
	BLOB_SOT_POTAG,
	BLOB_SIT_OTAG,
	BLOB_SIT_POTAG,
	BLOB_UN_OTAG,
	BLOB_UN_POTAG,
	BLOB_DEF_ACTION,
	BLOB_PRI,
	NUM_SET_IVLAN_PBV_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy ivlan_pbv_set_blob_policy[NUM_SET_IVLAN_PBV_TABLE_BLOB_IDS] = {
	[BLOB_IVLAN_PBV_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVID]			= { .name = "ivid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_OVID]			= { .name = "ovid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_VALID]			= { .name = "valid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_DT_OTAG]			= { .name = "dt_otag",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_DT_POTAG]			= { .name = "dt_potag",	 	.type = BLOBMSG_TYPE_INT32},
	[BLOB_SOT_OTAG]			= { .name = "sot_otag",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_SOT_POTAG]		= { .name = "sot_potag",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_SIT_OTAG]			= { .name = "sit_otag",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_SIT_POTAG]		= { .name = "sit_potag",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UN_OTAG]			= { .name = "un_otag",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_UN_POTAG]			= { .name = "un_potag",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_DEF_ACTION]		= { .name = "def_action",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_PRI]			= { .name = "pri",		.type = BLOBMSG_TYPE_INT32},
};

static int ivlan_pbv_table_set(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_IVLAN_PBV_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_ivlan_pbv = {
		.method = SET_IVLAN_PBV,
	};
	int ret;

	blobmsg_parse(ivlan_pbv_set_blob_policy, NUM_SET_IVLAN_PBV_TABLE_BLOB_IDS,
			tb, blob_data(msg), blob_len(msg));
	if (tb[BLOB_IVLAN_PBV_TABLE_INDEX])
		msg_set_ivlan_pbv.iport_num = blobmsg_get_u32(tb[BLOB_IVLAN_PBV_TABLE_INDEX]);

	if (tb[BLOB_IVID])
		msg_set_ivlan_pbv.ivid = blobmsg_get_u32(tb[BLOB_IVID]);

	if (tb[BLOB_OVID])
		msg_set_ivlan_pbv.ovid = blobmsg_get_u32(tb[BLOB_OVID]);

	if (tb[BLOB_IVLAN_PBV_VALID])
		msg_set_ivlan_pbv.valid = blobmsg_get_u32(tb[BLOB_IVLAN_PBV_VALID]);

	if (tb[BLOB_DT_OTAG])
		msg_set_ivlan_pbv.dt_otag = blobmsg_get_u32(tb[BLOB_DT_OTAG]);

	if (tb[BLOB_DT_POTAG])
		msg_set_ivlan_pbv.dt_potag = blobmsg_get_u32(tb[BLOB_DT_POTAG]);

	if (tb[BLOB_SOT_OTAG])
		msg_set_ivlan_pbv.sot_otag = blobmsg_get_u32(tb[BLOB_SOT_OTAG]);

	if (tb[BLOB_SOT_POTAG])
		msg_set_ivlan_pbv.sot_potag = blobmsg_get_u32(tb[BLOB_SOT_POTAG]);

	if (tb[BLOB_SIT_OTAG])
		msg_set_ivlan_pbv.sit_otag = blobmsg_get_u32(tb[BLOB_SIT_OTAG]);

	if (tb[BLOB_SIT_POTAG])
		msg_set_ivlan_pbv.sit_potag = blobmsg_get_u32(tb[BLOB_SIT_POTAG]);

	if (tb[BLOB_UN_OTAG])
		msg_set_ivlan_pbv.un_otag = blobmsg_get_u32(tb[BLOB_UN_OTAG]);

	if (tb[BLOB_UN_POTAG])
		msg_set_ivlan_pbv.un_potag = blobmsg_get_u32(tb[BLOB_UN_POTAG]);

	if (tb[BLOB_DEF_ACTION])
		msg_set_ivlan_pbv.def_action = blobmsg_get_u32(tb[BLOB_DEF_ACTION]);

	if (tb[BLOB_PRI])
		msg_set_ivlan_pbv.pri = blobmsg_get_u32(tb[BLOB_PRI]);

	ret = vlan_send_recv(&msg_set_ivlan_pbv, sizeof(msg_set_ivlan_pbv));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set ivlan_lkp table*/
enum set_ivlan_lkp_table_blob_id {
	BLOB_IVLAN_LKP_TABLE_INDEX,
	BLOB_IVLAN_LKP_VID,
	BLOB_L2_MISS_TOCPU,
	BLOB_L2_NON_UVAST_TOCPU,
	BLOB_IVLAN_LKP_VALID,
	BLOB_IVLAN_LKP_PORT_BITMAP,
	NUM_SET_IVLAN_LKP_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy ivlan_lkp_set_blob_policy[NUM_SET_IVLAN_LKP_TABLE_BLOB_IDS] = {
	[BLOB_IVLAN_LKP_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVLAN_LKP_VID]		= { .name = "vid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_L2_MISS_TOCPU]		= { .name = "l2_miss",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_L2_NON_UVAST_TOCPU]	= { .name = "l2_non_ucast",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVLAN_LKP_VALID]		= { .name = "valid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVLAN_LKP_PORT_BITMAP]	= { .name = "port_map",		.type = BLOBMSG_TYPE_INT32},
};

static int ivlan_lkp_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_IVLAN_LKP_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_ivlan_lkp = {
		.method = SET_IVLAN_LKP,
	};
	int ret;

	blobmsg_parse(ivlan_lkp_set_blob_policy, NUM_SET_IVLAN_LKP_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_IVLAN_LKP_TABLE_INDEX])
		msg_set_ivlan_lkp.iport_num = blobmsg_get_u32(tb[BLOB_IVLAN_LKP_TABLE_INDEX]);

	if (tb[BLOB_IVLAN_LKP_VID])
		msg_set_ivlan_lkp.vid = blobmsg_get_u32(tb[BLOB_IVLAN_LKP_VID]);

	if (tb[BLOB_L2_MISS_TOCPU])
		msg_set_ivlan_lkp.l2_miss_tocpu = blobmsg_get_u32(tb[BLOB_L2_MISS_TOCPU]);

	if (tb[BLOB_L2_NON_UVAST_TOCPU])
		msg_set_ivlan_lkp.l2_non_ucast_tocpu = blobmsg_get_u32(tb[BLOB_L2_NON_UVAST_TOCPU]);

	if (tb[BLOB_IVLAN_LKP_VALID])
		msg_set_ivlan_lkp.valid = blobmsg_get_u32(tb[BLOB_IVLAN_LKP_VALID]);

	if (tb[BLOB_IVLAN_LKP_PORT_BITMAP])
		msg_set_ivlan_lkp.port_bitmap = blobmsg_get_u32(tb[BLOB_IVLAN_LKP_PORT_BITMAP]);

	ret = vlan_send_recv(&msg_set_ivlan_lkp, sizeof(msg_set_ivlan_lkp));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set ivlan_xlt table*/
enum set_ivlan_xlt_table_blob_id {
	BLOB_IVLAN_XLT_TABLE_INDEX,
	BLOB_IVLAN_XLT_VID,
	BLOB_IVLAN_XLT_VALID,
	NUM_SET_IVLAN_XLT_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy ivlan_xlt_set_blob_policy[NUM_SET_IVLAN_XLT_TABLE_BLOB_IDS] = {
	[BLOB_IVLAN_XLT_TABLE_INDEX]		= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVLAN_XLT_VID]			= { .name = "vid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVLAN_XLT_VALID]			= { .name = "valid",		.type = BLOBMSG_TYPE_INT32},
};

static int ivlan_xlt_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_IVLAN_XLT_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_ivlan_xlt = {
		.method = SET_IVLAN_XLT,
	};
	int ret;

	blobmsg_parse(ivlan_xlt_set_blob_policy, NUM_SET_IVLAN_XLT_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_IVLAN_XLT_TABLE_INDEX])
		msg_set_ivlan_xlt.iport_num = blobmsg_get_u32(tb[BLOB_IVLAN_XLT_TABLE_INDEX]);

	if (tb[BLOB_IVLAN_XLT_VID])
		msg_set_ivlan_xlt.vid = blobmsg_get_u32(tb[BLOB_IVLAN_XLT_VID]);

	if (tb[BLOB_IVLAN_XLT_VALID])
		msg_set_ivlan_xlt.valid = blobmsg_get_u32(tb[BLOB_IVLAN_XLT_VALID]);

	ret = vlan_send_recv(&msg_set_ivlan_xlt, sizeof(msg_set_ivlan_xlt));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set ivlan_spl table*/
enum set_ivlan_spl_table_blob_id {
	BLOB_IVLAN_SPL_TABLE_INDEX,
	BLOB_IVLAN_SPL_CREDIT,
	NUM_SET_IVLAN_SPL_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy ivlan_spl_set_blob_policy[NUM_SET_IVLAN_SPL_TABLE_BLOB_IDS] = {
	[BLOB_IVLAN_SPL_TABLE_INDEX]		= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IVLAN_SPL_CREDIT]			= { .name = "credit",		.type = BLOBMSG_TYPE_INT32},
};

static int ivlan_spl_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_IVLAN_SPL_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_ivlan_spl = {
		.method = SET_IVLAN_SPL,
	};
	int ret;

	blobmsg_parse(ivlan_spl_set_blob_policy, NUM_SET_IVLAN_SPL_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_IVLAN_SPL_TABLE_INDEX])
		msg_set_ivlan_spl.iport_num = blobmsg_get_u32(tb[BLOB_IVLAN_SPL_TABLE_INDEX]);

	if (tb[BLOB_IVLAN_SPL_CREDIT])
		msg_set_ivlan_spl.credit = blobmsg_get_u32(tb[BLOB_IVLAN_SPL_CREDIT]);

	ret = vlan_send_recv(&msg_set_ivlan_spl, sizeof(msg_set_ivlan_spl));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set evlan_lkp table*/
enum set_evlan_lkp_table_blob_id {
	BLOB_EVLAN_LKP_TABLE_INDEX,
	BLOB_EVLAN_LKP_VID,
	BLOB_EVLAN_LKP_VALID,
	BLOB_EVLAN_UN_BITMAP,
	BLOB_EVLAN_LKP_PORT_BITMAP,
	NUM_SET_EVLAN_LKP_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy evlan_lkp_set_blob_policy[NUM_SET_EVLAN_LKP_TABLE_BLOB_IDS] = {
	[BLOB_EVLAN_LKP_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_LKP_VID]		= { .name = "vid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_LKP_VALID]		= { .name = "valid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_UN_BITMAP]		= { .name = "un_bitmap",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_LKP_PORT_BITMAP]	= { .name = "port_map",		.type = BLOBMSG_TYPE_INT32},
};

static int evlan_lkp_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_EVLAN_LKP_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_evlan_lkp = {
		.method = SET_EVLAN_LKP,
	};
	int ret;

	blobmsg_parse(evlan_lkp_set_blob_policy, NUM_SET_EVLAN_LKP_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_EVLAN_LKP_TABLE_INDEX])
		msg_set_evlan_lkp.iport_num = blobmsg_get_u32(tb[BLOB_EVLAN_LKP_TABLE_INDEX]);

	if (tb[BLOB_EVLAN_LKP_VID])
		msg_set_evlan_lkp.vid = blobmsg_get_u32(tb[BLOB_EVLAN_LKP_VID]);

	if (tb[BLOB_EVLAN_LKP_VALID])
		msg_set_evlan_lkp.valid = blobmsg_get_u32(tb[BLOB_EVLAN_LKP_VALID]);

	if (tb[BLOB_EVLAN_UN_BITMAP])
		msg_set_evlan_lkp.un_bitmap = blobmsg_get_u32(tb[BLOB_EVLAN_UN_BITMAP]);

	if (tb[BLOB_EVLAN_LKP_PORT_BITMAP])
		msg_set_evlan_lkp.port_bitmap = blobmsg_get_u32(tb[BLOB_EVLAN_LKP_PORT_BITMAP]);

	ret = vlan_send_recv(&msg_set_evlan_lkp, sizeof(msg_set_evlan_lkp));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set evlan_act table*/
enum set_evlan_act_table_blob_id {
	BLOB_EVLAN_ACT_TABLE_INDEX,
	BLOB_EVLAN_ACT_SOT_ACTION,
	BLOB_EVLAN_ACT_PSOT_ACTION,
	BLOB_EVLAN_ACT_DT_ACTION,
	BLOB_EVLAN_ACT_PDT_ACTION,
	BLOB_EVLAN_ACT_DEF_ACTION,
	NUM_SET_EVLAN_ACT_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy evlan_act_set_blob_policy[NUM_SET_EVLAN_ACT_TABLE_BLOB_IDS] = {
	[BLOB_EVLAN_ACT_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_ACT_SOT_ACTION]	= { .name = "sot_action",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_ACT_PSOT_ACTION]	= { .name = "psot_action",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_ACT_DT_ACTION]	= { .name = "dt_action",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_ACT_PDT_ACTION]	= { .name = "pdt_action",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_ACT_DEF_ACTION]	= { .name = "def_action",	.type = BLOBMSG_TYPE_INT32},
};

static int evlan_act_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_EVLAN_ACT_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_evlan_act = {
		.method = SET_EVLAN_ACT,
	};
	int ret;

	blobmsg_parse(evlan_act_set_blob_policy, NUM_SET_EVLAN_ACT_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_EVLAN_ACT_TABLE_INDEX])
		msg_set_evlan_act.iport_num = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_TABLE_INDEX]);

	if (tb[BLOB_EVLAN_ACT_SOT_ACTION])
		msg_set_evlan_act.sot_action = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_SOT_ACTION]);

	if (tb[BLOB_EVLAN_ACT_PSOT_ACTION])
		msg_set_evlan_act.psot_action = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_PSOT_ACTION]);

	if (tb[BLOB_EVLAN_ACT_DT_ACTION])
		msg_set_evlan_act.dt_action = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_DT_ACTION]);

	if (tb[BLOB_EVLAN_ACT_PDT_ACTION])
		msg_set_evlan_act.pdt_action = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_PDT_ACTION]);

	if (tb[BLOB_EVLAN_ACT_DEF_ACTION])
		msg_set_evlan_act.evlan_def_action = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_DEF_ACTION]);

	ret = vlan_send_recv(&msg_set_evlan_act, sizeof(msg_set_evlan_act));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set evlan_xlt table*/
enum set_evlan_xlt_table_blob_id {
	BLOB_EVLAN_XLT_TABLE_INDEX,
	BLOB_EVLAN_XLT_OPORT_NUM,
	BLOB_EVLAN_XLT_VALID,
	BLOB_EVLAN_XLT_OLD_IVID,
	BLOB_EVLAN_XLT_OLD_OVID,
	BLOB_EVLAN_XLT_NEW_IVID,
	BLOB_EVLAN_XLT_NEW_OVID,
	BLOB_EVLAN_XLT_OLD_IVID_MASK,
	BLOB_EVLAN_XLT_OLD_OVID_MASK,
	BLOB_EVLAN_ACT_IDX,
	BLOB_EVLAN_XLT_NEW_IPRI,
	BLOB_EVLAN_XLT_NEW_OPRI,
	NUM_SET_EVLAN_XLT_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy evlan_xlt_set_blob_policy[NUM_SET_EVLAN_XLT_TABLE_BLOB_IDS] = {
	[BLOB_EVLAN_XLT_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_OPORT_NUM]	= { .name = "oport_num",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_VALID]		= { .name = "valid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_OLD_IVID]	= { .name = "old_ivid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_OLD_OVID]	= { .name = "old_ovid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_NEW_IVID]	= { .name = "new_ivid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_NEW_OVID]	= { .name = "new_ovid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_OLD_IVID_MASK]	= { .name = "old_ivid_mask",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_OLD_OVID_MASK]	= { .name = "old_ovid_mask",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_ACT_IDX]		= { .name = "evlan_act_idx",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_NEW_IPRI]	= { .name = "new_ipri",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_XLT_NEW_OPRI]	= { .name = "new_opri",		.type = BLOBMSG_TYPE_INT32},
};

static int evlan_xlt_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_EVLAN_XLT_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_evlan_xlt = {
		.method = SET_EVLAN_XLT,
	};
	int ret;

	blobmsg_parse(evlan_xlt_set_blob_policy, NUM_SET_EVLAN_XLT_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_EVLAN_XLT_OPORT_NUM])
		msg_set_evlan_xlt.iport_num = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_OPORT_NUM]);

	if (tb[BLOB_EVLAN_XLT_OPORT_NUM])
		msg_set_evlan_xlt.oport_num = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_OPORT_NUM]);

	if (tb[BLOB_EVLAN_XLT_VALID])
		msg_set_evlan_xlt.valid = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_VALID]);

	if (tb[BLOB_EVLAN_XLT_OLD_IVID])
		msg_set_evlan_xlt.old_ivid = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_OLD_IVID]);

	if (tb[BLOB_EVLAN_XLT_OLD_OVID])
		msg_set_evlan_xlt.old_ovid = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_OLD_OVID]);

	if (tb[BLOB_EVLAN_XLT_NEW_IVID])
		msg_set_evlan_xlt.new_ivid = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_NEW_IVID]);

	if (tb[BLOB_EVLAN_XLT_NEW_OVID])
		msg_set_evlan_xlt.new_ovid = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_NEW_OVID]);

	if (tb[BLOB_EVLAN_XLT_OLD_IVID_MASK])
		msg_set_evlan_xlt.old_ivid_mask = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_OLD_IVID_MASK]);

	if (tb[BLOB_EVLAN_XLT_OLD_OVID_MASK])
		msg_set_evlan_xlt.old_ovid_mask = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_OLD_OVID_MASK]);

	if (tb[BLOB_EVLAN_ACT_IDX])
		msg_set_evlan_xlt.evlan_act_idx = blobmsg_get_u32(tb[BLOB_EVLAN_ACT_IDX]);

	if (tb[BLOB_EVLAN_XLT_NEW_IPRI])
		msg_set_evlan_xlt.new_ipri = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_NEW_IPRI]);

	if (tb[BLOB_EVLAN_XLT_NEW_OPRI])
		msg_set_evlan_xlt.new_opri = blobmsg_get_u32(tb[BLOB_EVLAN_XLT_NEW_OPRI]);

	ret = vlan_send_recv(&msg_set_evlan_xlt, sizeof(msg_set_evlan_xlt));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set evlan_otpid table*/
enum set_evlan_otpid_table_blob_id {
	BLOB_EVLAN_OTPID_TABLE_INDEX,
	BLOB_EVLAN_OTPID_TPID,
	NUM_SET_EVLAN_OTPID_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy evlan_otpid_set_blob_policy[NUM_SET_EVLAN_OTPID_TABLE_BLOB_IDS] = {
	[BLOB_EVLAN_OTPID_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_OTPID_TPID]		= { .name = "tpid",		.type = BLOBMSG_TYPE_INT32},
};

static int evlan_otpid_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_EVLAN_OTPID_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_evlan_otpid = {
		.method = SET_EVLAN_OTPID,
	};
	int ret;

	blobmsg_parse(evlan_otpid_set_blob_policy, NUM_SET_EVLAN_OTPID_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_EVLAN_OTPID_TABLE_INDEX])
		msg_set_evlan_otpid.iport_num = blobmsg_get_u32(tb[BLOB_EVLAN_OTPID_TABLE_INDEX]);

	if (tb[BLOB_EVLAN_OTPID_TPID])
		msg_set_evlan_otpid.tpid = blobmsg_get_u32(tb[BLOB_EVLAN_OTPID_TPID]);

	ret = vlan_send_recv(&msg_set_evlan_otpid, sizeof(msg_set_evlan_otpid));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

/*set evlan_ptpid table*/
enum set_evlan_ptpid_table_blob_id {
	BLOB_EVLAN_PTPID_TABLE_INDEX,
	BLOB_EVLAN_PTPID_ITPID,
	BLOB_EVLAN_PTPID_OTPID,
	NUM_SET_EVLAN_PTPID_TABLE_BLOB_IDS,
};

static const struct blobmsg_policy evlan_ptpid_set_blob_policy[NUM_SET_EVLAN_PTPID_TABLE_BLOB_IDS] = {
	[BLOB_EVLAN_PTPID_TABLE_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_PTPID_ITPID]	= { .name = "itpid",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_EVLAN_PTPID_OTPID]	= { .name = "otpid",		.type = BLOBMSG_TYPE_INT32},
};

static int evlan_ptpid_table_set(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SET_EVLAN_PTPID_TABLE_BLOB_IDS] = { };
	struct vlan_genl_msg msg_set_evlan_ptpid = {
		.method = SET_EVLAN_PTPID,
	};
	int ret;

	blobmsg_parse(evlan_ptpid_set_blob_policy, NUM_SET_EVLAN_PTPID_TABLE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_EVLAN_PTPID_TABLE_INDEX])
		msg_set_evlan_ptpid.iport_num = blobmsg_get_u32(tb[BLOB_EVLAN_PTPID_TABLE_INDEX]);

	if (tb[BLOB_EVLAN_PTPID_ITPID])
		msg_set_evlan_ptpid.itpid = blobmsg_get_u32(tb[BLOB_EVLAN_PTPID_ITPID]);

	if (tb[BLOB_EVLAN_PTPID_OTPID])
		msg_set_evlan_ptpid.otpid = blobmsg_get_u32(tb[BLOB_EVLAN_PTPID_OTPID]);

	ret = vlan_send_recv(&msg_set_evlan_ptpid, sizeof(msg_set_evlan_ptpid));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static const struct ubus_method vlan_methods[] = {
	UBUS_METHOD("dump", 		vlan_table_dump,	table_dump_blob_policy),
	UBUS_METHOD("set_iport", 	iport_table_set,	iport_set_blob_policy),
	UBUS_METHOD("set_ivlan_pbv",	ivlan_pbv_table_set,	ivlan_pbv_set_blob_policy),
	UBUS_METHOD("set_ivlan_lkp",	ivlan_lkp_table_set,	ivlan_lkp_set_blob_policy),
	UBUS_METHOD("set_ivlan_xlt",	ivlan_xlt_table_set,	ivlan_xlt_set_blob_policy),
	UBUS_METHOD("set_ivlan_spl",	ivlan_spl_table_set,	ivlan_spl_set_blob_policy),
	UBUS_METHOD("set_evlan_lkp",	evlan_lkp_table_set,	evlan_lkp_set_blob_policy),
	UBUS_METHOD("set_evlan_act",	evlan_act_table_set,	evlan_act_set_blob_policy),
	UBUS_METHOD("set_evlan_xlt",	evlan_xlt_table_set,	evlan_xlt_set_blob_policy),
	UBUS_METHOD("set_evlan_otpid",	evlan_otpid_table_set,	evlan_otpid_set_blob_policy),
	UBUS_METHOD("set_evlan_ptpid",	evlan_ptpid_table_set,	evlan_ptpid_set_blob_policy),
};

static struct ubus_object_type vlan_object_type =
	UBUS_OBJECT_TYPE("vlan", vlan_methods);

static struct ubus_object vlan_object = {
	.name = "dpns.vlan",
	.type = &vlan_object_type,
	.methods = vlan_methods,
	.n_methods = ARRAY_SIZE(vlan_methods),
};

int vlan_init(void)
{
	return ubus_add_object(ubus_ctx_get(), &vlan_object);
}

int vlan_exit(void)
{
	return ubus_remove_object(ubus_ctx_get(), &vlan_object);
}
