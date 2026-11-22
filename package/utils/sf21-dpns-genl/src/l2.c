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
#include "se_common.h"

#define DEFAULT_NUM_L2_MAC_BLOB_IDS	1

enum l2_mac_blob_id {
	BLOB_L2_MAC = 0,
	BLOB_L2_VLAN_ID,
	BLOB_L2_PORT,
	BLOB_L2_L3_ENABLE,
	BLOB_L2_DA_CML,
	BLOB_L2_SA_CML,
	BLOB_L2_MAGE_ENABLE,
	BLOB_L2_VLAN_ENABLE,
	BLOB_L2_STA_ID,
	BLOB_L2_REPEATER_ID,
	NUM_L2_MAC_BLOB_IDS,
};

enum l2_mac_mib_blob_id {
	BLOB_L2_MIB_MAC = 0,
	BLOB_L2_MIB_VLAN_ID,
	BLOB_L2_MIB_INDEX,
	BLOB_L2_MIB_MODE,
	BLOB_L2_MIB_OP,
	NUM_L2_MIB_BLOB_IDS,
};

enum nat_mib_blob_id {
	BLOB_NAT_DIR = 0,
	BLOB_NAT_PROTO,
	BLOB_NAT_PUB_PORT,
	BLOB_NAT_PRI_PORT,
	BLOB_NAT_RT_PORT,
	BLOB_NAT_PUB_IP,
	BLOB_NAT_PRI_IP,
	BLOB_NAT_RT_IP,
	BLOB_NAT_MIB_INDEX,
	BLOB_NAT_MIB_MODE,
	BLOB_NAT_MIB_OP,
	BLOB_NAT_IS_V6,
	NUM_NAT_MIB_BLOB_IDS,
};

enum l2_mac_spl_blob_id {
	BLOB_L2_SPL_MAC = 0,
	BLOB_L2_SPL_VLAN_ID,
	BLOB_L2_SPL_INDEX,
	BLOB_L2_SPL_MODE,
	BLOB_L2_SPL_SCREDIT,
	BLOB_L2_SPL_DCREDIT,
	NUM_L2_SPL_BLOB_IDS,
};

enum l2_mac_del_blob_id {
	BLOB_L2_DEL_MAC = 0,
	BLOB_L2_DEL_VLAN_ID,
	NUM_L2_DEL_BLOB_IDS,
};

enum dpns_mib_blob_id {
	BLOB_DPNSMIB_EN = 0,
	NUM_DPNS_MIB_BLOB_IDS,
};

enum l2_mac_hit_blob_id {
	BLOB_L2_HIT_MAC = 0,
	BLOB_L2_HIT_VLAN_ID,
	NUM_L2_HIT_BLOB_IDS,
};

static const struct blobmsg_policy l2_mac_blob_policy[] = {
	[BLOB_L2_PORT]			= { .name = "port",			.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_MAC]			= { .name = "mac",			.type = BLOBMSG_TYPE_STRING },
	[BLOB_L2_VLAN_ID]		= { .name = "vlan_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_L3_ENABLE]		= { .name = "l3_en",		.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_L2_DA_CML]		= { .name = "da_cml",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_SA_CML]		= { .name = "sa_cml",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_MAGE_ENABLE]	= { .name = "mac_age_en",	.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_L2_VLAN_ENABLE]	= { .name = "vlan_en",		.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_L2_STA_ID]		= { .name = "sta_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_REPEATER_ID]	= { .name = "repeater_id",	.type = BLOBMSG_TYPE_INT32	},
};

static const struct blobmsg_policy l2_mac_mib_blob_policy[] = {
	[BLOB_L2_MIB_MAC]			= { .name = "mac",		.type = BLOBMSG_TYPE_STRING 	},
	[BLOB_L2_MIB_VLAN_ID]			= { .name = "vlan_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_MIB_MODE]			= { .name = "mib_mode",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_MIB_INDEX]			= { .name = "mib_index",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_MIB_OP]			= { .name = "mib_op",		.type = BLOBMSG_TYPE_INT32	},
};

static const struct blobmsg_policy dpns_mib_blob_policy[] = {
	[BLOB_DPNSMIB_EN]			= { .name = "dpnsmib_en",		.type = BLOBMSG_TYPE_BOOL 	},
};

static const struct blobmsg_policy nat_mib_blob_policy[] = {
	[BLOB_NAT_DIR]				= { .name = "is_dnat",		.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_NAT_PROTO]			= { .name = "is_udp",		.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_NAT_PUB_PORT]			= { .name = "pub_port",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_NAT_PRI_PORT]			= { .name = "pri_port",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_NAT_RT_PORT]			= { .name = "rt_port",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_NAT_PUB_IP]			= { .name = "pub_ip",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_NAT_PRI_IP]			= { .name = "pri_ip",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_NAT_RT_IP]			= { .name = "rt_ip",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_NAT_MIB_MODE]			= { .name = "mib_mode",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_NAT_MIB_INDEX]			= { .name = "mib_index",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_NAT_MIB_OP]			= { .name = "mib_op",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_NAT_IS_V6]			= { .name = "is_v6",		.type = BLOBMSG_TYPE_BOOL 	},
};

static const struct blobmsg_policy l2_mac_hit_blob_policy[] = {
	[BLOB_L2_HIT_MAC]			= { .name = "mac",		.type = BLOBMSG_TYPE_STRING 	},
	[BLOB_L2_HIT_VLAN_ID]			= { .name = "vlan_id",		.type = BLOBMSG_TYPE_INT32	},
};

static const struct blobmsg_policy l2_mac_spl_blob_policy[] = {
	[BLOB_L2_SPL_MAC]			= { .name = "mac",			.type = BLOBMSG_TYPE_STRING },
	[BLOB_L2_SPL_VLAN_ID]		= { .name = "vlan_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_SPL_MODE]			= { .name = "spl_mode",		.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_L2_SPL_INDEX]			= { .name = "spl_index",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_SPL_SCREDIT]		= { .name = "spl_scredit",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_L2_SPL_DCREDIT]		= { .name = "spl_dcredit",	.type = BLOBMSG_TYPE_INT32	},
};

static const struct blobmsg_policy l2_mac_del_blob_policy[] = {
	[BLOB_L2_DEL_MAC]			= { .name = "mac",			.type = BLOBMSG_TYPE_STRING },
	[BLOB_L2_DEL_VLAN_ID]		= { .name = "vlan_id",		.type = BLOBMSG_TYPE_INT32	},
};

static const struct blobmsg_policy l2_mac_age_en_blob_policy[] = {
	[0]		= { .name = "age_en",	.type = BLOBMSG_TYPE_BOOL	},
};

static const struct blobmsg_policy l2_mac_learn_en_blob_policy[] = {
	[0]		= { .name = "learn_en",	.type = BLOBMSG_TYPE_BOOL	},
};

static const struct blobmsg_policy l2_mac_age_time_blob_policy[] = {
	[0]		= { .name = "age_time",	.type = BLOBMSG_TYPE_INT32	},
};

static const struct blobmsg_policy l2_mac_dump_spl_blob_policy[] = {
	[0]		= { .name = "index",	.type = BLOBMSG_TYPE_INT32	},
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

static int l2_mac_send_recv(void *sbuf, size_t slen, int timeout)
{
	struct l2_mac_genl_resp *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_L2_MAC, sbuf, slen, (void **)&rbuf, &rlen, timeout);
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

static int l2_mac_add(struct ubus_context *ctx, struct ubus_object *obj,
						struct ubus_request_data *req, const char *method,
						struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_L2_MAC_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = L2_MAC_ADD,
	};
	int ret;

	blobmsg_parse(l2_mac_blob_policy, NUM_L2_MAC_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_L2_PORT]) {
		msg_add.port = blobmsg_cast_u64(tb[BLOB_L2_PORT]);
	}

	if (tb[BLOB_L2_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_L2_MAC]));
		if (ret)
			return ret;
		msg_add.mac = be64_to_cpu(mac_buf);
	}

	if (tb[BLOB_L2_VLAN_ID]) {
		msg_add.vid = blobmsg_get_u32(tb[BLOB_L2_VLAN_ID]);
	}

	if (tb[BLOB_L2_L3_ENABLE]) {
		msg_add.l3_en = blobmsg_get_bool(tb[BLOB_L2_L3_ENABLE]);
	}

	if (tb[BLOB_L2_DA_CML]) {
		msg_add.da_cml = blobmsg_get_u32(tb[BLOB_L2_DA_CML]);
	}

	if (tb[BLOB_L2_SA_CML]) {
		msg_add.sa_cml = blobmsg_get_u32(tb[BLOB_L2_SA_CML]);
	}

	if (tb[BLOB_L2_MAGE_ENABLE]) {
		msg_add.mage_en = blobmsg_get_bool(tb[BLOB_L2_MAGE_ENABLE]);
	}

	if (tb[BLOB_L2_VLAN_ENABLE]) {
		msg_add.vlan_en = blobmsg_get_bool(tb[BLOB_L2_VLAN_ENABLE]);
	}

	if (tb[BLOB_L2_STA_ID]) {
		msg_add.sta_id = blobmsg_get_u32(tb[BLOB_L2_STA_ID]);
	}

	if (tb[BLOB_L2_REPEATER_ID]) {
		msg_add.repeater_id = blobmsg_get_u32(tb[BLOB_L2_REPEATER_ID]);
	}

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_mib(struct ubus_context *ctx, struct ubus_object *obj,
						struct ubus_request_data *req, const char *method,
						struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_L2_MIB_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = L2_MAC_MIB_EN,
	};
	int ret;

	blobmsg_parse(l2_mac_mib_blob_policy, NUM_L2_MIB_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_L2_MIB_OP])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.mib_op = blobmsg_get_u32(tb[BLOB_L2_MIB_OP]);

	if (tb[BLOB_L2_MIB_INDEX]) {
		msg_add.mib_index = blobmsg_cast_u64(tb[BLOB_L2_MIB_INDEX]);
	}

	if (tb[BLOB_L2_MIB_MODE]) {
		msg_add.mib_mode = blobmsg_cast_u64(tb[BLOB_L2_MIB_MODE]);
	}

	if (tb[BLOB_L2_MIB_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_L2_MIB_MAC]));
		if (ret)
			return ret;
		msg_add.mac = be64_to_cpu(mac_buf);
	}

	if (tb[BLOB_L2_MIB_VLAN_ID]) {
		msg_add.vid = blobmsg_get_u32(tb[BLOB_L2_MIB_VLAN_ID]);
	}

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
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

static int dpnsmib_en(struct ubus_context *ctx, struct ubus_object *obj,
						struct ubus_request_data *req, const char *method,
						struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_DPNS_MIB_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = DPNS_MIB_EN,
	};
	int ret, i;

	blobmsg_parse(dpns_mib_blob_policy, NUM_DPNS_MIB_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_DPNSMIB_EN])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.mib_en = blobmsg_get_bool(tb[BLOB_DPNSMIB_EN]);

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int nat_mib(struct ubus_context *ctx, struct ubus_object *obj,
						struct ubus_request_data *req, const char *method,
						struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_NAT_MIB_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = NAT_MIB_EN,
	};
	int ret, i;

	blobmsg_parse(nat_mib_blob_policy, NUM_NAT_MIB_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_NAT_DIR])
		msg_add.is_dnat = blobmsg_get_bool(tb[BLOB_NAT_DIR]);

	if (tb[BLOB_NAT_PROTO])
		msg_add.is_udp = blobmsg_get_bool(tb[BLOB_NAT_PROTO]);

	if (tb[BLOB_NAT_PUB_PORT])
		msg_add.public_port = blobmsg_get_u32(tb[BLOB_NAT_PUB_PORT]);

	if (tb[BLOB_NAT_PRI_PORT])
		msg_add.private_port = blobmsg_get_u32(tb[BLOB_NAT_PRI_PORT]);

	if (tb[BLOB_NAT_RT_PORT])
		msg_add.router_port = blobmsg_get_u32(tb[BLOB_NAT_RT_PORT]);

	if (tb[BLOB_NAT_PUB_IP]) {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_PUB_IP]),
					msg_add.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_add.public_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	if (tb[BLOB_NAT_PRI_IP]) {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_PRI_IP]),
					msg_add.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_add.private_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}

	}

	if (tb[BLOB_NAT_RT_IP]) {
		uint32_t ip_buf[4] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_NAT_RT_IP]),
					msg_add.is_v6);
		if (ret)
			return ret;

		for (i = 0; i < 4; i++) {
			msg_add.router_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	if (!tb[BLOB_NAT_MIB_OP])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.mib_op = blobmsg_get_u32(tb[BLOB_NAT_MIB_OP]);

	if (!tb[BLOB_NAT_MIB_INDEX])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.mib_index = blobmsg_get_u32(tb[BLOB_NAT_MIB_INDEX]);

	if (!tb[BLOB_NAT_MIB_MODE])
		msg_add.mib_mode = 15;
	msg_add.mib_mode = blobmsg_get_u32(tb[BLOB_NAT_MIB_MODE]);
	if (msg_add.mib_mode < 0 || msg_add.mib_mode > 15)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[BLOB_NAT_IS_V6])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_add.is_v6 = blobmsg_get_bool(tb[BLOB_NAT_IS_V6]);

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_spl(struct ubus_context *ctx, struct ubus_object *obj,
						struct ubus_request_data *req, const char *method,
						struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_L2_SPL_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = L2_MAC_SPL_EN,
	};
	int ret;

	blobmsg_parse(l2_mac_spl_blob_policy, NUM_L2_SPL_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_L2_SPL_INDEX]) {
		msg_add.spl_index = blobmsg_cast_u64(tb[BLOB_L2_SPL_INDEX]);
	}

	if (tb[BLOB_L2_SPL_MODE]) {
		msg_add.l2_spl_mode = blobmsg_get_bool(tb[BLOB_L2_SPL_MODE]);
	}

	if (tb[BLOB_L2_SPL_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_L2_SPL_MAC]));
		if (ret)
			return ret;
		msg_add.mac = be64_to_cpu(mac_buf);
	}

	if (tb[BLOB_L2_SPL_VLAN_ID]) {
		msg_add.vid = blobmsg_cast_u64(tb[BLOB_L2_SPL_VLAN_ID]);
	}

	if (tb[BLOB_L2_SPL_SCREDIT]) {
		msg_add.scredit = blobmsg_cast_u64(tb[BLOB_L2_SPL_SCREDIT]);
	}
	if (tb[BLOB_L2_SPL_DCREDIT]) {
		msg_add.dcredit = blobmsg_cast_u64(tb[BLOB_L2_SPL_DCREDIT]);
	}

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_del(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_L2_DEL_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = L2_MAC_DEL,
	};
	int ret;

	blobmsg_parse(l2_mac_del_blob_policy, NUM_L2_DEL_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_L2_DEL_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_L2_DEL_MAC]));
		if (ret)
			return ret;
		msg_add.mac = be64_to_cpu(mac_buf);
	}

	if (tb[BLOB_L2_DEL_VLAN_ID]) {
		msg_add.vid = blobmsg_get_u32(tb[BLOB_L2_DEL_VLAN_ID]);
	}

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_age(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[DEFAULT_NUM_L2_MAC_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_en = {
		.method = L2_MAC_SET_AGEING_EN,
	};
	int ret;

	blobmsg_parse(l2_mac_age_en_blob_policy, 1, tb, blob_data(msg), blob_len(msg));

	if (!tb[0])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_en.enable = blobmsg_get_bool(tb[0]);

	ret = l2_mac_send_recv(&msg_en, sizeof(msg_en), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_learn(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[DEFAULT_NUM_L2_MAC_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_en = {
		.method = L2_MAC_SET_LEARNING_EN,
	};
	int ret;

	blobmsg_parse(l2_mac_learn_en_blob_policy, 1, tb, blob_data(msg), blob_len(msg));

	if (!tb[0])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_en.enable = blobmsg_get_bool(tb[0]);

	ret = l2_mac_send_recv(&msg_en, sizeof(msg_en), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_age_time(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[DEFAULT_NUM_L2_MAC_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_time = {
		.method = L2_MAC_SET_AGE_TIME,
	};
	int ret;

	blobmsg_parse(l2_mac_age_time_blob_policy, 1, tb, blob_data(msg), blob_len(msg));

	if (!tb[0])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_time.value = blobmsg_get_u32(tb[0]);

	ret = l2_mac_send_recv(&msg_time, sizeof(msg_time), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_dump_mac(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct l2_mac_genl_msg_add msg_dump_mac = {
		.method = L2_MAC_DUMP_MAC_TB,
	};
	int ret;

	ret = l2_mac_send_recv(&msg_dump_mac, sizeof(msg_dump_mac), 1000);
	return ret;
}

static int l2_mac_dump_spl(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[DEFAULT_NUM_L2_MAC_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_dump_spl = {
		.method = L2_MAC_DUMP_SPL_TB,
	};
	int ret;

	blobmsg_parse(l2_mac_dump_spl_blob_policy, 1, tb, blob_data(msg), blob_len(msg));

	if (!tb[0])
		return UBUS_STATUS_INVALID_ARGUMENT;
	msg_dump_spl.value = blobmsg_get_u32(tb[0]);

	ret = l2_mac_send_recv(&msg_dump_spl, sizeof(msg_dump_spl), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static int l2_mac_clear(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct l2_mac_genl_msg_add msg_clear_mac = {
		.method = L2_MAC_CLEAR,
	};
	int ret;

	ret = l2_mac_send_recv(&msg_clear_mac, sizeof(msg_clear_mac), 1000);
	return ret;
}

static int l2_mac_dump_num(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct l2_mac_genl_msg_add msg_dump_num = {
		.method = L2_MAC_NUM_DUMP,
	};
	int ret;

	ret = l2_mac_send_recv(&msg_dump_num, sizeof(msg_dump_num), 1000);
	return ret;
}

static int l2_mac_hit(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_L2_HIT_BLOB_IDS] = { };
	struct l2_mac_genl_msg_add msg_add = {
		.method = L2_MAC_HIT,
	};
	int ret;

	blobmsg_parse(l2_mac_hit_blob_policy, NUM_L2_HIT_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (tb[BLOB_L2_HIT_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_L2_HIT_MAC]));
		if (ret)
			return ret;
		msg_add.mac = be64_to_cpu(mac_buf);
	}

	if (tb[BLOB_L2_HIT_VLAN_ID]) {
		msg_add.vid = blobmsg_get_u32(tb[BLOB_L2_HIT_VLAN_ID]);
	}

	ret = l2_mac_send_recv(&msg_add, sizeof(msg_add), 1000);
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

static const struct ubus_method l2_mac_methods[] = {
	UBUS_METHOD("add",		l2_mac_add,		l2_mac_blob_policy),
	UBUS_METHOD("macmib",		l2_mac_mib,		l2_mac_mib_blob_policy),
	UBUS_METHOD("spl",		l2_mac_spl,		l2_mac_spl_blob_policy),
 	UBUS_METHOD("del",		l2_mac_del,		l2_mac_del_blob_policy),
 	UBUS_METHOD("age_en",	l2_mac_age,		l2_mac_age_en_blob_policy),
 	UBUS_METHOD("learn_en", l2_mac_learn,	l2_mac_learn_en_blob_policy),
	UBUS_METHOD("age_time", l2_mac_age_time,l2_mac_age_time_blob_policy),
	UBUS_METHOD_NOARG("dump", l2_mac_dump_mac),
	UBUS_METHOD("dump_spl", l2_mac_dump_spl,l2_mac_dump_spl_blob_policy),
	UBUS_METHOD_NOARG("clear", l2_mac_clear),
	UBUS_METHOD_NOARG("dump_num", l2_mac_dump_num),
	UBUS_METHOD("natmib",		nat_mib,		nat_mib_blob_policy),
	UBUS_METHOD("dpnsmib_en",	dpnsmib_en,		dpns_mib_blob_policy),
	UBUS_METHOD("hit",		l2_mac_hit,		l2_mac_hit_blob_policy),
 };

 static struct ubus_object_type l2_mac_object_type =
		UBUS_OBJECT_TYPE("l2", l2_mac_methods);

 static struct ubus_object l2_mac_object = {
	.name = "dpns.l2",
	.type = &l2_mac_object_type,
 	.methods = l2_mac_methods,
	.n_methods = ARRAY_SIZE(l2_mac_methods),
};

int l2_mac_init(void)
{
	return ubus_add_object(ubus_ctx_get(), &l2_mac_object);
}

int l2_mac_exit(void)
{
	return ubus_remove_object(ubus_ctx_get(), &l2_mac_object);
}
