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
#include "dpns_nat_genl.h"

#define DEFAULT_NUM_NAT_BLOB_IDS	1

enum search_nat_blob_id {
	BLOB_NAT_DIR,
	BLOB_NAT_PROTO,
	BLOB_NAT_IS_V6,
	BLOB_NAT_PUB_PORT,
	BLOB_NAT_PRI_PORT,
	BLOB_NAT_RT_PORT,
	BLOB_NAT_PUB_IP,
	BLOB_NAT_PRI_IP,
	BLOB_NAT_RT_IP,
	BLOB_NAT_IS_OFFLOAD,
	NUM_SEARCH_NAT_BLOB_IDS,
};

static const struct blobmsg_policy hw_search_blob_policy[NUM_SEARCH_NAT_BLOB_IDS] = {
	[BLOB_NAT_DIR]		= { .name = "is_dnat",	.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_PROTO]	= { .name = "is_udp",	.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_IS_V6]	= { .name = "is_v6",	.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_PUB_PORT]	= { .name = "pub_port",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_PRI_PORT]	= { .name = "pri_port",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_RT_PORT]	= { .name = "rt_port",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_PUB_IP]	= { .name = "pub_ip",	.type = BLOBMSG_TYPE_STRING},
	[BLOB_NAT_PRI_IP]	= { .name = "pri_ip",	.type = BLOBMSG_TYPE_STRING},
	[BLOB_NAT_RT_IP]	= { .name = "rt_ip",	.type = BLOBMSG_TYPE_STRING},
	[BLOB_NAT_IS_OFFLOAD]	= { .name = "is_offload",.type = BLOBMSG_TYPE_BOOL},
};

enum addnapt_nat_blob_id {
	BLOB_NAT_ADDNAPT_DIR,
	BLOB_NAT_ADDNAPT_PROTO,
	BLOB_NAT_ADDNAPT_IS_V6,
	BLOB_NAT_ADDNAPT_IS_LF,
	BLOB_NAT_ADDNAPT_PUB_PORT,
	BLOB_NAT_ADDNAPT_PRI_PORT,
	BLOB_NAT_ADDNAPT_RT_PORT,
	BLOB_NAT_ADDNAPT_PUB_IP,
	BLOB_NAT_ADDNAPT_PRI_IP,
	BLOB_NAT_ADDNAPT_RT_IP,
	BLOB_NAT_ADDNAPT_SOPORT,
	BLOB_NAT_ADDNAPT_DOPORT,
	BLOB_NAT_ADDNAPT_PUBMAC_INDEX,
	BLOB_NAT_ADDNAPT_PRIMAC_INDEX,
	BLOB_NAT_ADDNAPT_DRTMAC_INDEX,
	BLOB_NAT_ADDNAPT_SRTMAC_INDEX,
	BLOB_NAT_ADDNAPT_HNAT_MODE,
	NUM_ADDNAPT_NAT_BLOB_IDS,
};

static const struct blobmsg_policy addnapt_blob_policy[NUM_ADDNAPT_NAT_BLOB_IDS] = {
	[BLOB_NAT_ADDNAPT_DIR]		= { .name = "is_dnat",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_ADDNAPT_PROTO]	= { .name = "is_udp",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_ADDNAPT_IS_V6]	= { .name = "is_v6",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_ADDNAPT_IS_LF]	= { .name = "is_lf",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_ADDNAPT_PUB_PORT]	= { .name = "pub_port",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_PRI_PORT]	= { .name = "pri_port",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_RT_PORT]	= { .name = "rt_port",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_PUB_IP]	= { .name = "pub_ip",		.type = BLOBMSG_TYPE_STRING},
	[BLOB_NAT_ADDNAPT_PRI_IP]	= { .name = "pri_ip",		.type = BLOBMSG_TYPE_STRING},
	[BLOB_NAT_ADDNAPT_RT_IP]	= { .name = "rt_ip",		.type = BLOBMSG_TYPE_STRING},
	[BLOB_NAT_ADDNAPT_PUBMAC_INDEX]	= { .name = "pubmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_PRIMAC_INDEX]	= { .name = "primac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_DRTMAC_INDEX]	= { .name = "drtmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_SRTMAC_INDEX]	= { .name = "srtmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_SOPORT]	= { .name = "soport_id",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_DOPORT]	= { .name = "doport_id",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_ADDNAPT_HNAT_MODE]	= { .name = "hnat_mode",	.type = BLOBMSG_TYPE_INT32},
};

enum offload_en_blob_id {
	BLOB_OFFLOAD_MODE,
	NUM_OFFLOAD_EN_BLOB_IDS,
};

static const struct blobmsg_policy offload_mode_blob_policy[NUM_OFFLOAD_EN_BLOB_IDS] = {
	[BLOB_OFFLOAD_MODE]	= { .name = "offload_mode",	.type = BLOBMSG_TYPE_INT32},
};

enum mode_set_blob_id {
	BLOB_NAT_IS_LF,
	BLOB_NAT_IS_UDP,
	BLOB_NAT_IS_V6_MODE,
	BLOB_NAT_LF_MODE,
	BLOB_NAT_HNAT_MODE,
	NUM_MODE_SET_BLOB_IDS,
};

static const struct blobmsg_policy mode_set_blob_policy[NUM_MODE_SET_BLOB_IDS] = {
	[BLOB_NAT_IS_LF]	= { .name = "is_lf",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_IS_UDP]	= { .name = "is_udp",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_IS_V6_MODE]	= { .name = "is_v6_mode",	.type = BLOBMSG_TYPE_BOOL},
	[BLOB_NAT_HNAT_MODE]	= { .name = "hnat_mode",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_LF_MODE]	= { .name = "lf_mode",		.type = BLOBMSG_TYPE_INT32},
};

enum subnet_op_blob_id {
	BLOB_SUBNET_IS_GET,
	BLOB_SUBNET_IS_LAN,
	BLOB_SUBNET_INDEX,
	BLOB_SUBNET_IFNAME,
	NUM_SUBNET_OP_BLOB_IDS,
};

static const struct blobmsg_policy subnet_op_blob_policy[NUM_SUBNET_OP_BLOB_IDS] = {
	[BLOB_SUBNET_IS_GET]	= { .name = "is_get",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_SUBNET_IS_LAN]	= { .name = "is_lan",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_SUBNET_INDEX]	= { .name = "index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_SUBNET_IFNAME]	= { .name = "ifname",		.type = BLOBMSG_TYPE_STRING},
};

enum subnet_ovport_blob_id {
	BLOB_OVPORT_IFNAME,
	NUM_OVPORT_SET_BLOB_IDS,
};

static const struct blobmsg_policy subnet_ovport_blob_policy[NUM_OVPORT_SET_BLOB_IDS] = {
	[BLOB_OVPORT_IFNAME]	= { .name = "ifname",		.type = BLOBMSG_TYPE_STRING},
};

enum add_mode_set_blob_id {
	BLOB_NAT_NAPT_ADD_MODE,
	NUM_ADD_MODE_SET_BLOB_IDS,
};

static const struct blobmsg_policy add_mode_set_blob_policy[NUM_ADD_MODE_SET_BLOB_IDS] = {
	[BLOB_NAT_NAPT_ADD_MODE]	= { .name = "add_mode",		.type = BLOBMSG_TYPE_INT32},
};

enum nat_update_blob_id {
	BLOB_UPDATE_NAT_ID,
	BLOB_UPDATE_IS_V6,
	BLOB_UPDATE_SPL_IDX,
	BLOB_UPDATE_STAT_IDX,
	BLOB_UPDATE_REPL_PRI,
	BLOB_UPDATE_REPL_PRI_EN,
	BLOB_UPDATE_SPL_EN,
	BLOB_UPDATE_STAT_EN,
	BLOB_UPDATE_SRTMAC,
	BLOB_UPDATE_DRTMAC,
	BLOB_UPDATE_PRIMAC,
	BLOB_UPDATE_PUBMAC,
	BLOB_UPDATE_SOPORT,
	BLOB_UPDATE_DOPORT,
	NUM_UPDATE_BLOB_IDS,
};

static const struct blobmsg_policy nat_update_blob_policy[NUM_UPDATE_BLOB_IDS] = {
	[BLOB_UPDATE_NAT_ID]		= { .name = "nat_id",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_IS_V6]		= { .name = "is_v6",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_UPDATE_SPL_EN]		= { .name = "spl_en",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_UPDATE_SPL_IDX]		= { .name = "spl_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_STAT_EN]		= { .name = "stat_en",		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_UPDATE_STAT_IDX]		= { .name = "stat_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_REPL_PRI_EN]	= { .name = "repl_pri_en",	.type = BLOBMSG_TYPE_BOOL},
	[BLOB_UPDATE_REPL_PRI]		= { .name = "repl_pri",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_SRTMAC]		= { .name = "srtmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_DRTMAC]		= { .name = "drtmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_PRIMAC]		= { .name = "primac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_PUBMAC]		= { .name = "pubmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_SOPORT]		= { .name = "soport_id",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_UPDATE_DOPORT]		= { .name = "doport_id",	.type = BLOBMSG_TYPE_INT32},
};

enum spl_set_blob_id {
	BLOB_SPL_IS_DNAT,
	BLOB_SPL_INDEX,
	BLOB_SPL_VALUE,
	BLOB_PKT_LENGTH,
	BLOB_NAT_MIB_MODE,
	BLOB_IS_ZERO_LMT,
	BLOB_SPL_CNT_MODE,
	BLOB_SPL_MODE,
	BLOB_SPL_SOURCE,
	NUM_SPL_SET_BLOB_IDS,
};

static const struct blobmsg_policy spl_set_blob_policy[NUM_SPL_SET_BLOB_IDS] = {
	[BLOB_SPL_IS_DNAT]	= { .name = "is_dnat", 			.type = BLOBMSG_TYPE_BOOL},
	[BLOB_SPL_INDEX]	= { .name = "spl_index",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_SPL_VALUE]	= { .name = "speed_value",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_PKT_LENGTH]	= { .name = "pkt_length",		.type = BLOBMSG_TYPE_INT32},
	[BLOB_NAT_MIB_MODE]	= { .name = "nat_mib_mode", 		.type = BLOBMSG_TYPE_INT32},
	[BLOB_IS_ZERO_LMT]	= { .name = "is_zero_limit", 		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_SPL_CNT_MODE]	= { .name = "spl_cnt_mode", 		.type = BLOBMSG_TYPE_INT32},
	[BLOB_SPL_MODE]		= { .name = "spl_mode", 		.type = BLOBMSG_TYPE_BOOL},
	[BLOB_SPL_SOURCE]	= { .name = "spl_source", 		.type = BLOBMSG_TYPE_INT32},
};

enum dump_byid_blob_id {
	BLOB_NAT_ID,
	BLOB_IS_IPV6,
	NUM_DUMP_BYID_BLOB_IDS,
};

static const struct blobmsg_policy dump_byid_blob_policy[NUM_DUMP_BYID_BLOB_IDS] = {
	[BLOB_NAT_ID]		= { .name = "nat_id",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_IS_IPV6]		= { .name = "is_ipv6", 	.type = BLOBMSG_TYPE_BOOL},
};

enum del_byid_blob_id {
	BLOB_DEL_ID,
	NUM_DEL_BYID_BLOB_IDS,
};

static const struct blobmsg_policy del_byid_blob_policy[NUM_DEL_BYID_BLOB_IDS] = {
	[BLOB_DEL_ID]		= { .name = "nat_id",	.type = BLOBMSG_TYPE_INT32},
};

enum add_ppphdr_blob_id {
	BLOB_SRTMAC_INDEX,
	BLOB_PPPOE_SID,
	BLOB_PPPOE_IS_IPV6,
	NUM_ADD_PPPHDR_BLOB_IDS,
};

static const struct blobmsg_policy add_ppphdr_blob_policy[NUM_ADD_PPPHDR_BLOB_IDS] = {
	[BLOB_SRTMAC_INDEX]	= { .name = "srtmac_index",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_PPPOE_SID]	= { .name = "pppoe_sid",	.type = BLOBMSG_TYPE_INT32},
	[BLOB_PPPOE_IS_IPV6]	= { .name = "is_ipv6",		.type = BLOBMSG_TYPE_BOOL},
};

static void ubus_err_code_reply(struct ubus_context *ctx, struct ubus_request_data *req, int err)
{
	struct blob_buf b = { };
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "error", err);
	if (err < 0)
		blobmsg_add_string(&b, "message", strerror(-err));
	ubus_send_reply(ctx, req, b.head);
}

static void ubus_ifname_reply(struct ubus_context *ctx, struct ubus_request_data *req, struct nat_genl_msg *rbuf)
{
	struct blob_buf b = { };
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "ifname", rbuf->ifname);
	ubus_send_reply(ctx, req, b.head);
}


static int nat_send_recv(void *sbuf, size_t slen)
{
	struct nat_genl_resp *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_NAT, sbuf, slen, (void **)&rbuf, &rlen, 1000);
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

static int parse_ip(void *buf, const char *str, bool is_ipv6)
{
	if (is_ipv6) {
		if (!inet_pton(AF_INET6, str, buf))
			goto invalid_ip;
	} else {
		if (!inet_pton(AF_INET, str, buf))
			goto invalid_ip;
	}
	return 0;

invalid_ip:
	pr_err("invalid IP address: %s\n", str);
	return UBUS_STATUS_INVALID_ARGUMENT;
}

static int nat_dump_count(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct nat_genl_msg msg_dump_nat = {
		.method = NAT_DUMP_NAPT_COUNT,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_NAT, &msg_dump_nat, sizeof(msg_dump_nat));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int nat_dump_delay(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct nat_genl_msg msg_dump_nat = {
		.method = NAT_ELKP_DELAY,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_NAT, &msg_dump_nat, sizeof(msg_dump_nat));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int nat_dump_napt_tb(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct nat_genl_msg msg_dump_nat = {
		.method = NAT_DUMP_NAPT_TB,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_NAT, &msg_dump_nat, sizeof(msg_dump_nat));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int nat_mode_reset(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct nat_genl_msg msg_dump_nat = {
		.method = NAT_MODE_RESET,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_NAT, &msg_dump_nat, sizeof(msg_dump_nat));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int nat_hw_search(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SEARCH_NAT_BLOB_IDS] = { };
	struct nat_genl_msg msg_search = {
		.method = NAT_HW_SEARCH,
	};
	int ret, i;

	blobmsg_parse(hw_search_blob_policy, NUM_SEARCH_NAT_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_NAT_IS_V6])
		msg_search.is_v6 = blobmsg_get_bool(tb[BLOB_NAT_IS_V6]);

	if (tb[BLOB_NAT_DIR])
		msg_search.is_dnat = blobmsg_get_bool(tb[BLOB_NAT_DIR]);

	if (tb[BLOB_NAT_PROTO])
		msg_search.is_udp = blobmsg_get_bool(tb[BLOB_NAT_PROTO]);

	if (tb[BLOB_NAT_IS_OFFLOAD])
		msg_search.offload_en = blobmsg_get_bool(tb[BLOB_NAT_IS_OFFLOAD]);

	if (tb[BLOB_NAT_PUB_PORT])
		msg_search.public_port = blobmsg_get_u32(tb[BLOB_NAT_PUB_PORT]);

	if (tb[BLOB_NAT_PRI_PORT])
		msg_search.private_port = blobmsg_get_u32(tb[BLOB_NAT_PRI_PORT]);

	if (tb[BLOB_NAT_RT_PORT])
		msg_search.router_port = blobmsg_get_u32(tb[BLOB_NAT_RT_PORT]);

	if (tb[BLOB_NAT_PUB_IP]) {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_PUB_IP]),
					msg_search.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_search.public_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	if (tb[BLOB_NAT_PRI_IP]) {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_PRI_IP]),
					msg_search.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_search.private_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}

	}

	if (tb[BLOB_NAT_RT_IP]) {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_RT_IP]),
					msg_search.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_search.router_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	ret = nat_send_recv(&msg_search, sizeof(msg_search));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_clean_tb(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct nat_genl_msg msg_clean_nat = {
		.method = NAT_CLEAN,
	};
	int ret;

	ret = sfgenl_msg_send(SF_GENL_COMP_NAT, &msg_clean_nat, sizeof(msg_clean_nat));
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int nat_del_byid(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DEL_BYID_BLOB_IDS] = { };
	struct nat_genl_msg msg_del = {
		.method = NAT_DEL,
	};
	int ret;

	blobmsg_parse(del_byid_blob_policy, NUM_DEL_BYID_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_DEL_ID])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_del.nat_id = blobmsg_get_u32(tb[BLOB_DEL_ID]);

	ret = nat_send_recv(&msg_del, sizeof(msg_del));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_napt_add(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ADDNAPT_NAT_BLOB_IDS] = { };
	struct nat_genl_msg msg_add = {
		.method = NAT_NAPT_ADD,
	};
	int ret, i;

	blobmsg_parse(addnapt_blob_policy, NUM_ADDNAPT_NAT_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_NAT_ADDNAPT_IS_V6])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.is_v6 = blobmsg_get_bool(tb[BLOB_NAT_ADDNAPT_IS_V6]);

	if (!tb[BLOB_NAT_ADDNAPT_IS_LF])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.is_lf = blobmsg_get_bool(tb[BLOB_NAT_ADDNAPT_IS_LF]);

	if (!tb[BLOB_NAT_ADDNAPT_DIR])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.is_dnat = blobmsg_get_bool(tb[BLOB_NAT_ADDNAPT_DIR]);

	if (!tb[BLOB_NAT_ADDNAPT_PROTO])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.is_udp = blobmsg_get_bool(tb[BLOB_NAT_ADDNAPT_PROTO]);

	if (!tb[BLOB_NAT_ADDNAPT_PUB_PORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.public_port = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_PUB_PORT]);

	if (!tb[BLOB_NAT_ADDNAPT_PRI_PORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.private_port = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_PRI_PORT]);

	if (!tb[BLOB_NAT_ADDNAPT_RT_PORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.router_port = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_RT_PORT]);

	if (!tb[BLOB_NAT_ADDNAPT_PUB_IP]) {
		return UBUS_STATUS_INVALID_ARGUMENT;
	} else {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_ADDNAPT_PUB_IP]),
					msg_add.is_v6);
		if (ret)
			return ret;
		for (i = 0; i < 4; i++) {
			msg_add.public_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	if (!tb[BLOB_NAT_ADDNAPT_PRI_IP]) {
		return UBUS_STATUS_INVALID_ARGUMENT;
	} else {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_ADDNAPT_PRI_IP]),
					msg_add.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_add.private_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}


	if (!tb[BLOB_NAT_ADDNAPT_RT_IP]) {
		return UBUS_STATUS_INVALID_ARGUMENT;
	} else {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_ADDNAPT_RT_IP]),
					msg_add.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_add.router_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	if (!tb[BLOB_NAT_ADDNAPT_PUBMAC_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.pubmac_index = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_PUBMAC_INDEX]);

	if (!tb[BLOB_NAT_ADDNAPT_PRIMAC_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.primac_index = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_PRIMAC_INDEX]);

	if (!tb[BLOB_NAT_ADDNAPT_DRTMAC_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.drtmac_index = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_DRTMAC_INDEX]);

	if (!tb[BLOB_NAT_ADDNAPT_SRTMAC_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.srtmac_index = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_SRTMAC_INDEX]);

	if (!tb[BLOB_NAT_ADDNAPT_SOPORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.soport_id = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_SOPORT]);

	if (!tb[BLOB_NAT_ADDNAPT_DOPORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.doport_id = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_DOPORT]);

	if (!tb[BLOB_NAT_ADDNAPT_HNAT_MODE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.hnat_mode = blobmsg_get_u32(tb[BLOB_NAT_ADDNAPT_HNAT_MODE]);

	ret = nat_send_recv(&msg_add, sizeof(msg_add));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_offload_mode(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_OFFLOAD_EN_BLOB_IDS] = { };
	struct nat_genl_msg msg_offload = {
		.method = NAT_OFFLOAD_MODE,
	};
	int ret;

	blobmsg_parse(offload_mode_blob_policy, NUM_OFFLOAD_EN_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));


	if (!tb[BLOB_OFFLOAD_MODE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	msg_offload.nat_offload_mode = blobmsg_get_u32(tb[BLOB_OFFLOAD_MODE]);
	if (msg_offload.nat_offload_mode < 0 || msg_offload.nat_offload_mode > 2)
		return UBUS_STATUS_INVALID_ARGUMENT;

	ret = nat_send_recv(&msg_offload, sizeof(msg_offload));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_mode_set(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_MODE_SET_BLOB_IDS] = { };
	struct nat_genl_msg msg_mode = {
		.method = NAT_MODE_SET,
	};
	int ret;

	blobmsg_parse(mode_set_blob_policy, NUM_MODE_SET_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_NAT_IS_V6_MODE] || !tb[BLOB_NAT_IS_LF])
		return UBUS_STATUS_INVALID_ARGUMENT;

	msg_mode.is_lf = blobmsg_get_bool(tb[BLOB_NAT_IS_LF]);
	msg_mode.is_v6_mode = blobmsg_get_bool(tb[BLOB_NAT_IS_V6_MODE]);

	if (msg_mode.is_lf) {
		if (!tb[BLOB_NAT_LF_MODE])
			return UBUS_STATUS_INVALID_ARGUMENT;

		msg_mode.lf_mode = blobmsg_get_u32(tb[BLOB_NAT_LF_MODE]);
	} else {
		if (!tb[BLOB_NAT_IS_UDP] || !tb[BLOB_NAT_HNAT_MODE])
			return UBUS_STATUS_INVALID_ARGUMENT;

		msg_mode.is_udp = blobmsg_get_bool(tb[BLOB_NAT_IS_UDP]);
		msg_mode.hnat_mode = blobmsg_get_u32(tb[BLOB_NAT_HNAT_MODE]);
	}

	ret = nat_send_recv(&msg_mode, sizeof(msg_mode));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_subnet_op(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SUBNET_OP_BLOB_IDS] = { };
	struct nat_genl_msg msg_mode = {
		.is_get = true,
		.method = NAT_SUBNET,
	};
	int ret;

	blobmsg_parse(subnet_op_blob_policy, NUM_SUBNET_OP_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_SUBNET_IS_GET])
		msg_mode.is_get = blobmsg_get_bool(tb[BLOB_SUBNET_IS_GET]);

	if (tb[BLOB_SUBNET_IS_LAN])
		msg_mode.is_lan = blobmsg_get_bool(tb[BLOB_SUBNET_IS_LAN]);

	if (tb[BLOB_SUBNET_INDEX])
		msg_mode.index  = blobmsg_get_u32(tb[BLOB_SUBNET_INDEX]);

	if (tb[BLOB_SUBNET_IFNAME])
		memcpy(msg_mode.ifname, blobmsg_get_string(tb[BLOB_SUBNET_IFNAME]), IFNAMSIZ);

	ret = nat_send_recv(&msg_mode, sizeof(msg_mode));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_set_ovport(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_OVPORT_SET_BLOB_IDS] = { };
	struct nat_genl_msg msg_mode = {
		.method = NAT_OVPORT_SET,
	};
	int ret;

	blobmsg_parse(subnet_ovport_blob_policy, NUM_OVPORT_SET_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_OVPORT_IFNAME])
		memcpy(msg_mode.ifname, blobmsg_get_string(tb[BLOB_OVPORT_IFNAME]), IFNAMSIZ);

	ret = nat_send_recv(&msg_mode, sizeof(msg_mode));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_update_byid(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_UPDATE_BLOB_IDS] = { };
	struct nat_genl_msg msg_search = {
		.method = NAT_UPDATE_BYID,
	};
	int ret;

	blobmsg_parse(nat_update_blob_policy, NUM_UPDATE_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_UPDATE_NAT_ID])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.nat_id = blobmsg_get_u32(tb[BLOB_UPDATE_NAT_ID]);

	if (!tb[BLOB_UPDATE_IS_V6])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.is_v6 = blobmsg_get_bool(tb[BLOB_UPDATE_IS_V6]);

	if (!tb[BLOB_UPDATE_SPL_EN])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.spl_en = blobmsg_get_u32(tb[BLOB_UPDATE_SPL_EN]);

	if (!tb[BLOB_UPDATE_STAT_EN])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.stat_en = blobmsg_get_u32(tb[BLOB_UPDATE_STAT_EN]);

	if (!tb[BLOB_UPDATE_REPL_PRI_EN])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.repl_pri_en = blobmsg_get_u32(tb[BLOB_UPDATE_REPL_PRI_EN]);

	if (!tb[BLOB_UPDATE_REPL_PRI])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.repl_pri = blobmsg_get_u32(tb[BLOB_UPDATE_REPL_PRI]);

	if (!tb[BLOB_UPDATE_SPL_IDX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.spl_index = blobmsg_get_u32(tb[BLOB_UPDATE_SPL_IDX]);

	if (!tb[BLOB_UPDATE_STAT_IDX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.stat_index = blobmsg_get_u32(tb[BLOB_UPDATE_STAT_IDX]);

	if (!tb[BLOB_UPDATE_SRTMAC])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.srtmac_index = blobmsg_get_u32(tb[BLOB_UPDATE_SRTMAC]);

	if (!tb[BLOB_UPDATE_DRTMAC])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.drtmac_index = blobmsg_get_u32(tb[BLOB_UPDATE_DRTMAC]);

	if (!tb[BLOB_UPDATE_PRIMAC])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.primac_index = blobmsg_get_u32(tb[BLOB_UPDATE_PRIMAC]);

	if (!tb[BLOB_UPDATE_PUBMAC])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.pubmac_index = blobmsg_get_u32(tb[BLOB_UPDATE_PUBMAC]);

	if (!tb[BLOB_UPDATE_SOPORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.soport_id = blobmsg_get_u32(tb[BLOB_UPDATE_SOPORT]);

	if (!tb[BLOB_UPDATE_DOPORT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_search.doport_id = blobmsg_get_u32(tb[BLOB_UPDATE_DOPORT]);

	ret = nat_send_recv(&msg_search, sizeof(msg_search));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_dump_byid(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DUMP_BYID_BLOB_IDS] = { };
	struct nat_genl_msg msg_dump = {
		.method = NAT_DUMP_BYID,
	};
	int ret;

	blobmsg_parse(dump_byid_blob_policy, NUM_DUMP_BYID_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_NAT_ID])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_dump.nat_id = blobmsg_get_u32(tb[BLOB_NAT_ID]);

	if (!tb[BLOB_IS_IPV6])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_dump.is_v6 = blobmsg_get_bool(tb[BLOB_IS_IPV6]);

	ret = nat_send_recv(&msg_dump, sizeof(msg_dump));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_napt_add_mode_set(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ADD_MODE_SET_BLOB_IDS] = { };
	struct nat_genl_msg msg_mode = {
		.method = NAT_NAPT_ADD_MODE_SET,
	};
	int ret;

	blobmsg_parse(add_mode_set_blob_policy, NUM_ADD_MODE_SET_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_NAT_NAPT_ADD_MODE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_mode.napt_add_mode = blobmsg_get_u32(tb[BLOB_NAT_NAPT_ADD_MODE]);

	ret = nat_send_recv(&msg_mode, sizeof(msg_mode));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_get_ovport(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct nat_genl_msg msg_mode = {
		.method = NAT_OVPORT_GET,
	};
	struct nat_genl_msg *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_NAT, &msg_mode, sizeof(msg_mode), (void **)&rbuf, &rlen, 1000);
	if (ret)
		return ret;

	if (rlen < sizeof(*rbuf)) {
		free(rbuf);
		return -EINVAL;
	}

	ubus_ifname_reply(ctx, req, rbuf);
	free(rbuf);
	return 0;
}

static int nat_spl_set(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_SPL_SET_BLOB_IDS] = { };
	struct nat_genl_msg msg_spl = {
		.method = NAT_SPL_SET,
	};
	int ret;

	blobmsg_parse(spl_set_blob_policy, NUM_SPL_SET_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_SPL_IS_DNAT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_spl.is_dnat = blobmsg_get_bool(tb[BLOB_SPL_IS_DNAT]);

	if (!tb[BLOB_SPL_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_spl.spl_index = blobmsg_get_u32(tb[BLOB_SPL_INDEX]);

	if (tb[BLOB_PKT_LENGTH])
		msg_spl.pkt_length = blobmsg_get_u32(tb[BLOB_PKT_LENGTH]);

	if (tb[BLOB_NAT_MIB_MODE]) {
		msg_spl.nat_mib_mode = blobmsg_get_u32(tb[BLOB_NAT_MIB_MODE]);
		if ((msg_spl.nat_mib_mode > 15) || (msg_spl.nat_mib_mode < 0))
			return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[BLOB_IS_ZERO_LMT])
		msg_spl.is_zero_lmt = blobmsg_get_bool(tb[BLOB_IS_ZERO_LMT]);

	if (!tb[BLOB_SPL_CNT_MODE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_spl.spl_cnt_mode = blobmsg_get_u32(tb[BLOB_SPL_CNT_MODE]);

	if ((msg_spl.spl_cnt_mode > 3) || (msg_spl.spl_cnt_mode < 0))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[BLOB_SPL_MODE])
		msg_spl.spl_mode = blobmsg_get_bool(tb[BLOB_SPL_MODE]);

	if (!tb[BLOB_SPL_SOURCE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_spl.spl_source = blobmsg_get_u32(tb[BLOB_SPL_SOURCE]);
	if ((msg_spl.spl_source > 3) || (msg_spl.spl_source < 0))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[BLOB_SPL_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_spl.spl_value = blobmsg_get_u32(tb[BLOB_SPL_VALUE]);

	ret = nat_send_recv(&msg_spl, sizeof(msg_spl));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_add_ppphdr(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ADD_PPPHDR_BLOB_IDS] = { };
	struct nat_genl_msg msg_del = {
		.method = NAT_ADD_PPPHDR,
	};
	int ret;

	blobmsg_parse(add_ppphdr_blob_policy, NUM_ADD_PPPHDR_BLOB_IDS,
				tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_SRTMAC_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_del.index = blobmsg_get_u32(tb[BLOB_SRTMAC_INDEX]);

	if (!tb[BLOB_PPPOE_SID])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_del.pppoe_sid = blobmsg_get_u32(tb[BLOB_PPPOE_SID]);

	if (!tb[BLOB_PPPOE_IS_IPV6])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_del.is_v6 = blobmsg_get_bool(tb[BLOB_PPPOE_IS_IPV6]);

	ret = nat_send_recv(&msg_del, sizeof(msg_del));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static const struct ubus_method nat_methods[] = {
	UBUS_METHOD_NOARG("count",	nat_dump_count),
	UBUS_METHOD_NOARG("delay",	nat_dump_delay),
	UBUS_METHOD_NOARG("dump_nat", 	nat_dump_napt_tb),
	UBUS_METHOD_NOARG("ovport_get",	nat_get_ovport),
	UBUS_METHOD_NOARG("mode_reset",	nat_mode_reset),
	UBUS_METHOD_NOARG("clean", 	nat_clean_tb),
	UBUS_METHOD("set_offload_mode", nat_offload_mode,	offload_mode_blob_policy),
	UBUS_METHOD("search", 		nat_hw_search,		hw_search_blob_policy),
	UBUS_METHOD("mode", 		nat_mode_set,		mode_set_blob_policy),
	UBUS_METHOD("subnet", 		nat_subnet_op,		subnet_op_blob_policy),
	UBUS_METHOD("ovport_set",	nat_set_ovport,		subnet_ovport_blob_policy),
	UBUS_METHOD("spl_set", 		nat_spl_set,		spl_set_blob_policy),
	UBUS_METHOD("add_mode", 	nat_napt_add_mode_set,	add_mode_set_blob_policy),
	UBUS_METHOD("add_napt", 	nat_napt_add,		addnapt_blob_policy),
	UBUS_METHOD("dump_nat_byid",	nat_dump_byid,		dump_byid_blob_policy),
	UBUS_METHOD("update_napt_byid", nat_update_byid,	nat_update_blob_policy),
	UBUS_METHOD("del_nat_byid",	nat_del_byid,		del_byid_blob_policy),
	UBUS_METHOD("add_pppoe_hdr",	nat_add_ppphdr,		add_ppphdr_blob_policy),
};

static struct ubus_object_type nat_object_type =
	UBUS_OBJECT_TYPE("nat", nat_methods);

static struct ubus_object nat_object = {
	.name = "dpns.nat",
	.type = &nat_object_type,
	.methods = nat_methods,
	.n_methods = ARRAY_SIZE(nat_methods),
};

int nat_init(void)
{
	return ubus_add_object(ubus_ctx_get(), &nat_object);
}

int nat_exit(void)
{
	return ubus_remove_object(ubus_ctx_get(), &nat_object);
}
