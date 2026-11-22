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
#include "dpns_acl_msg.h"

enum acl_add_blob_id {
	BLOB_ACL_INDEX,
	BLOB_ACL_POLICY,
	BLOB_ACL_DIR,	/* 0: IACL , 1: EACL */
	BLOB_ACL_PRIORITY,
	BLOB_ACL_PRIORITY_EN,
	BLOB_ACL_DSCP_REPLACE_EN,
	BLOB_ACL_NEW_ID,
	BLOB_ACL_SRC_MAC,
	BLOB_ACL_SRC_MAC_MASK,
	BLOB_ACL_DST_MAC,
	BLOB_ACL_DST_MAC_MASK,
	BLOB_ACL_SRC_IP,
	BLOB_ACL_SRC_IP6,
	BLOB_ACL_SRC_IP_MASK,
	BLOB_ACL_DST_IP,
	BLOB_ACL_DST_IP6,
	BLOB_ACL_DST_IP_MASK,
	BLOB_ACL_IPORT_ID,
	BLOB_ACL_OPORT_ID,
	BLOB_ACL_MSPORT_ID,
	BLOB_ACL_SRC_PORT,
	BLOB_ACL_SRC_PORT_MASK,
	BLOB_ACL_DST_PORT,
	BLOB_ACL_DST_PORT_MASK,
	BLOB_ACL_OVID,
	BLOB_ACL_TOS,
	BLOB_ACL_IPPROTO,
	BLOB_ACL_FTYPE_01,
	BLOB_ACL_FTYPE_23,
	BLOB_ACL_FTYPE_45,
	BLOB_ACL_FTYPE_67,
	BLOB_ACL_FTYPE_89,
	BLOB_ACL_FTYPE_1011,
	BLOB_ACL_ACTION,
	BLOB_ACL_SPEED_SET,
	BLOB_ACL_SPEED_INDEX,
	BLOB_ACL_SPEC_INFO_L1,
	BLOB_ACL_SPEC_INFO_L2,
	BLOB_ACL_SPEC_INFO_H1,
	BLOB_ACL_SPEC_INFO_H2,
	BLOB_ACL_SPEC_INFO_L1_MASK,
	BLOB_ACL_SPEC_INFO_L2_MASK,
	BLOB_ACL_SPEC_INFO_H1_MASK,
	BLOB_ACL_SPEC_INFO_H2_MASK,
	BLOB_ACL_PKT_OFFSET_0,
	BLOB_ACL_PKT_OFFSET_1,
	BLOB_ACL_PKT_OFFSET_2,
	BLOB_ACL_PKT_OFFSET_3,
	BLOB_ACL_PKT_OFFSET_4,
	BLOB_ACL_PKT_OFFSET_5,
	BLOB_ACL_PKT_OFFSET_6,
	BLOB_ACL_PKT_OFFSET_7,
	BLOB_ACL_UPSEND_SPL_ONLY,
	BLOB_ACL_SPL_ID,
	BLOB_ACL_IS_IPV4,
	BLOB_ACL_IS_IPV6,
	NUM_ACL_BLOB_IDS,
};

static const struct blobmsg_policy acl_add_blob_policy[NUM_ACL_BLOB_IDS] = {
	[BLOB_ACL_INDEX]		= { .name = "index",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_POLICY]		= { .name = "policy",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_DIR]			= { .name = "dir",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PRIORITY]		= { .name = "priority",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PRIORITY_EN]		= { .name = "priority_en",	.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_ACL_DSCP_REPLACE_EN]	= { .name = "dscp_replace_en",	.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_ACL_NEW_ID]		= { .name = "new_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SRC_MAC]		= { .name = "smac",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_SRC_MAC_MASK]		= { .name = "smac_mask",	.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_DST_MAC]		= { .name = "dmac",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_DST_MAC_MASK]		= { .name = "dmac_mask",	.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_SRC_IP]		= { .name = "sip",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_SRC_IP6]		= { .name = "sip6",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_SRC_IP_MASK]		= { .name = "sip_mask",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_DST_IP]		= { .name = "dip",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_DST_IP6]		= { .name = "dip6",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_DST_IP_MASK]		= { .name = "dip_mask",		.type = BLOBMSG_TYPE_STRING	},
	[BLOB_ACL_IPORT_ID]		= { .name = "iport_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_OPORT_ID]		= { .name = "oport_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_MSPORT_ID]		= { .name = "msport_id",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SRC_PORT]		= { .name = "sport",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SRC_PORT_MASK]	= { .name = "sport_mask",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_DST_PORT]		= { .name = "dport",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_DST_PORT_MASK]	= { .name = "dport_mask",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_OVID]			= { .name = "ovid",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_TOS]			= { .name = "tos",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_IPPROTO]		= { .name = "ip_proto",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_FTYPE_01]		= { .name = "cast",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_FTYPE_23]		= { .name = "vlan",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_FTYPE_45]		= { .name = "etype",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_FTYPE_67]		= { .name = "iptype",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_FTYPE_89]		= { .name = "frag",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_FTYPE_1011]		= { .name = "l4_type",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_ACTION]		= { .name = "mf_action",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEED_SET]		= { .name = "spl",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEED_INDEX]		= { .name = "spl_index",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_L1]		= { .name = "spec_info_l1",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_L2]		= { .name = "spec_info_l2",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_H1]		= { .name = "spec_info_h1",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_H2]		= { .name = "spec_info_h2",	.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_L1_MASK]	= { .name = "l1_mask",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_L2_MASK]	= { .name = "l2_mask",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_H1_MASK]	= { .name = "h1_mask",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_SPEC_INFO_H2_MASK]	= { .name = "h2_mask",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_0]		= { .name = "offset0",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_1]		= { .name = "offset1",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_2]		= { .name = "offset2",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_3]		= { .name = "offset3",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_4]		= { .name = "offset4",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_5]		= { .name = "offset5",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_6]		= { .name = "offset6",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_PKT_OFFSET_7]		= { .name = "offset7",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_UPSEND_SPL_ONLY]	= { .name = "upsend_spl_only",	.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_ACL_SPL_ID]		= { .name = "spl_id",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_IS_IPV4]              = { .name = "is_v4",		.type = BLOBMSG_TYPE_BOOL	},
	[BLOB_ACL_IS_IPV6]              = { .name = "is_v6",		.type = BLOBMSG_TYPE_BOOL	},
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

static int acl_send_recv(void *sbuf, size_t slen)
{
	struct acl_genl_resp *rbuf = NULL;
	size_t rlen = 0;
	int ret;

	ret = sfgenl_msg_send_recv(SF_GENL_COMP_ACL, sbuf, slen, (void **)&rbuf, &rlen, 1000);
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

static int acl_add(struct ubus_context *ctx, struct ubus_object *obj,
		   struct ubus_request_data *req, const char *method,
		   struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ACL_BLOB_IDS] = { };
	struct acl_genl_msg_add msg_add = {
		.method = ACL_ADD,
	};
	int ret, i;
	struct acl_data_t *key = &msg_add.key, *mask = &msg_add.mask;

	blobmsg_parse(acl_add_blob_policy, NUM_ACL_BLOB_IDS, tb, blob_data(msg), blob_len(msg));
	memset(mask, 0xff, sizeof(*mask));

	if (!tb[BLOB_ACL_INDEX]) {
		msg_add.index = -1;
	} else {
		msg_add.index = blobmsg_get_u32(tb[BLOB_ACL_INDEX]);
		if (msg_add.index > INDEX_MAX) {
			pr_err("index should be in the range of 16K\n");
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	if (!tb[BLOB_ACL_DIR]) {
		pr_err("dir must be specified: 0 for iacl, others for eacl");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	msg_add.is_eacl = !!(blobmsg_get_u32(tb[BLOB_ACL_DIR]));

	if (!tb[BLOB_ACL_POLICY]) {
		pr_err("%s must be specified\n", acl_add_blob_policy[BLOB_ACL_POLICY].name);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	key->policy = blobmsg_get_u32(tb[BLOB_ACL_POLICY]);
	if (msg_add.is_eacl) {
		if (key->policy == ACT_MIRROR2CPU || key->policy == ACT_MIRROR_INGRESS || key->policy == ACT_MIRROR_EGRESS) {
			pr_err("EACL not support policy%d\n", key->policy);
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	if (key->policy == ACT_REDIRECT || key->policy == ACT_MIRROR_INGRESS || key->policy == ACT_MIRROR_EGRESS) {
		if (tb[BLOB_ACL_NEW_ID]) {
			key->pkt_ctrl = blobmsg_get_u32(tb[BLOB_ACL_NEW_ID]);
			if (key->policy == ACT_MIRROR_EGRESS && tb[BLOB_ACL_MSPORT_ID]) {
				key->pkt_ctrl |= blobmsg_get_u32(tb[BLOB_ACL_MSPORT_ID]) << 5;
			}
		} else {
			pr_err("%s must be specified when %s is %u\n",
				acl_add_blob_policy[BLOB_ACL_NEW_ID].name,
				acl_add_blob_policy[BLOB_ACL_POLICY].name, ACT_REDIRECT);
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	if (key->policy == ACT_SPL) {
		if(tb[BLOB_ACL_SPEED_SET]) {
			if (!tb[BLOB_ACL_SPEED_INDEX]) {
				pr_err("when set speed, must choose a index\n");
				return UBUS_STATUS_INVALID_ARGUMENT;
			} else {
				msg_add.spl_index = blobmsg_get_u32(tb[BLOB_ACL_SPEED_INDEX]);
				if (msg_add.spl_index > ACL_SPL_TB_SZ) {
					pr_err("spl_index should in range 0~31\n");
					return UBUS_STATUS_INVALID_ARGUMENT;
				}
			}
			msg_add.spl = blobmsg_get_u32(tb[BLOB_ACL_SPEED_SET]);
			if (msg_add.spl > SPL_MAX) {
				pr_err("SPL max is 2^24-1, out of range!\n");
				return UBUS_STATUS_INVALID_ARGUMENT;
			}
		} else {
			msg_add.spl = -1;
		}

		if (tb[BLOB_ACL_UPSEND_SPL_ONLY])
			key->pkt_ctrl |= blobmsg_get_bool(tb[BLOB_ACL_UPSEND_SPL_ONLY]) << 5;

		if (tb[BLOB_ACL_SPL_ID])
			key->pkt_ctrl |= blobmsg_get_u32(tb[BLOB_ACL_SPL_ID]) & 0x1f;
	}

	if (tb[BLOB_ACL_SRC_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_ACL_SRC_MAC]));
		if (ret)
			return ret;

		key->smac = be64_to_cpu(mac_buf);

		if (tb[BLOB_ACL_SRC_MAC_MASK]) {
			ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_ACL_SRC_MAC_MASK]));
			if (ret)
				return ret;

			mask->smac = be64_to_cpu(mac_buf);
		} else {
			mask->smac = 0;
		}
	}

	if (tb[BLOB_ACL_DST_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_ACL_DST_MAC]));
		if (ret)
			return ret;

		key->dmac = be64_to_cpu(mac_buf);

		if (tb[BLOB_ACL_DST_MAC_MASK]) {
			ret = parse_mac((uint8_t *)&mac_buf + 2, blobmsg_get_string(tb[BLOB_ACL_DST_MAC_MASK]));
			if (ret)
				return ret;

			mask->dmac = be64_to_cpu(mac_buf);
		} else {
			mask->dmac = 0;
		}
	}

	if (tb[BLOB_ACL_SRC_IP]) {
		uint32_t ip_buf = 0;
		ret = parse_ip(&ip_buf, blobmsg_get_string(tb[BLOB_ACL_SRC_IP]), 0);
		if (ret)
			return ret;

		key->sip = be32_to_cpu(ip_buf);

		if (tb[BLOB_ACL_SRC_IP_MASK]) {
			ip_buf = 0;
			ret = parse_ip(&ip_buf, blobmsg_get_string(tb[BLOB_ACL_SRC_IP_MASK]), 0);
			if (ret)
				return ret;

			mask->sip = be32_to_cpu(ip_buf);
		} else {
			mask->sip = 0;
		}
		msg_add.is_ipv4 = true;
	}

	if (tb[BLOB_ACL_DST_IP]) {
		uint32_t ip_buf = 0;
		ret = parse_ip(&ip_buf, blobmsg_get_string(tb[BLOB_ACL_DST_IP]), 0);
		if (ret)
			return ret;

		key->dip = be32_to_cpu(ip_buf);

		if (tb[BLOB_ACL_DST_IP_MASK]) {
			ip_buf = 0;
			ret = parse_ip(&ip_buf, blobmsg_get_string(tb[BLOB_ACL_DST_IP_MASK]), 0);
			if (ret)
				return ret;

			mask->dip = be32_to_cpu(ip_buf);
		} else {
			mask->dip = 0;
		}
		msg_add.is_ipv4 = true;
	}

	if (tb[BLOB_ACL_SRC_IP6]) {
		uint64_t ip_buf[2] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_ACL_SRC_IP6]), 1);
		if (ret)
			return ret;

		key->sip_l = be64_to_cpu(ip_buf[1]);
		key->sip_h = be64_to_cpu(ip_buf[0]);

		if (tb[BLOB_ACL_SRC_IP_MASK]) {
			memset(ip_buf, 0, sizeof(ip_buf));
			ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_ACL_SRC_IP_MASK]), 1);
			if (ret)
				return ret;
			mask->sip_l = be64_to_cpu(ip_buf[1]);
			mask->sip_h = be64_to_cpu(ip_buf[0]);
		} else {
			mask->sip_l = 0;
			mask->sip_h = 0;
		}
		msg_add.is_ipv6 = true;
	}

	if (tb[BLOB_ACL_DST_IP6]) {
		uint64_t ip_buf[2] = {0};
		ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_ACL_DST_IP6]), 1);
		if (ret)
			return ret;

		key->dip_l = be64_to_cpu(ip_buf[1]);
		key->dip_h = be64_to_cpu(ip_buf[0]);

		if (tb[BLOB_ACL_DST_IP_MASK]) {
			memset(ip_buf, 0, sizeof(ip_buf));
			ret = parse_ip(ip_buf, blobmsg_get_string(tb[BLOB_ACL_DST_IP_MASK]), 1);

			if (ret)
				return ret;

			mask->dip_l = be64_to_cpu(ip_buf[1]);
			mask->dip_h = be64_to_cpu(ip_buf[0]);
		} else {
			mask->dip_l = 0;
			mask->dip_h = 0;
		}
		msg_add.is_ipv6 = true;
	}

	if (tb[BLOB_ACL_SRC_PORT]) {
		key->sport = blobmsg_get_u32(tb[BLOB_ACL_SRC_PORT]);
		if (tb[BLOB_ACL_SRC_PORT_MASK])
			mask->sport = blobmsg_get_u32(tb[BLOB_ACL_SRC_PORT_MASK]);
		else
			mask->sport = 0;
	}

	if (tb[BLOB_ACL_DST_PORT]) {
		key->dport = blobmsg_get_u32(tb[BLOB_ACL_DST_PORT]);
		if (tb[BLOB_ACL_DST_PORT_MASK])
			mask->dport = blobmsg_get_u32(tb[BLOB_ACL_DST_PORT_MASK]);
		else
			mask->dport = 0;
	}

	if (tb[BLOB_ACL_TOS]) {
		key->tos_pri = blobmsg_get_u32(tb[BLOB_ACL_TOS]);
		mask->tos_pri = 0;
	}

	if (tb[BLOB_ACL_IPPROTO]) {
		key->protocol = blobmsg_get_u32(tb[BLOB_ACL_IPPROTO]);
		mask->protocol = 0;
	}

	if (tb[BLOB_ACL_OVID]) {
		key->ovid = blobmsg_get_u32(tb[BLOB_ACL_OVID]);
		mask->ovid = 0;
	}

	if (tb[BLOB_ACL_IPORT_ID]) {
		key->ivport_id = blobmsg_get_u32(tb[BLOB_ACL_IPORT_ID]);
		mask->ivport_id = 0;
	}

	if (tb[BLOB_ACL_OPORT_ID]) {
		key->ovport_id = blobmsg_get_u32(tb[BLOB_ACL_OPORT_ID]);
		mask->ovport_id = 0;
	}

	if (tb[BLOB_ACL_FTYPE_01]) {
		key->frame_type_1_0 = blobmsg_get_u32(tb[BLOB_ACL_FTYPE_01]);
		mask->frame_type_1_0 = 0;
	}

	for (i = 0; i < 8; i++) {
		if (tb[BLOB_ACL_PKT_OFFSET_0 + i]) {
			msg_add.offset[i] = blobmsg_get_u32(tb[BLOB_ACL_PKT_OFFSET_0 + i]);
		} else {
			msg_add.offset[i] = -1;
			break;
		}
	}

	if (tb[BLOB_ACL_FTYPE_23]) {
		key->frame_type_3_2 = blobmsg_get_u32(tb[BLOB_ACL_FTYPE_23]);
		mask->frame_type_3_2 = 0;
	}

	if (tb[BLOB_ACL_FTYPE_45]) {
		key->frame_type_5_4 = blobmsg_get_u32(tb[BLOB_ACL_FTYPE_45]);
		mask->frame_type_5_4 = 0;
	}

	if (tb[BLOB_ACL_FTYPE_67]) {
		key->frame_type_7_6 = blobmsg_get_u32(tb[BLOB_ACL_FTYPE_67]);
		mask->frame_type_7_6 = 0;
	}

	if (tb[BLOB_ACL_FTYPE_89]) {
		key->frame_type_9_8 = blobmsg_get_u32(tb[BLOB_ACL_FTYPE_89]);
		mask->frame_type_9_8 = 0;
	}

	if (tb[BLOB_ACL_FTYPE_1011]) {
		key->frame_type_11_10 = blobmsg_get_u32(tb[BLOB_ACL_FTYPE_1011]);
		mask->frame_type_11_10 = 0;
	}

	if (tb[BLOB_ACL_ACTION]) {
		key->mf_action = blobmsg_get_u32(tb[BLOB_ACL_ACTION]);
		mask->mf_action = 0;
	}

	if (tb[BLOB_ACL_SPEC_INFO_L1]) {
		uint32_t info_buf;
		info_buf = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_L1]);
		key->spec_info_l1 = be32_to_cpu(info_buf);
		if (tb[BLOB_ACL_SPEC_INFO_L1_MASK]) {
			info_buf = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_L1_MASK]);
			mask->spec_info_l1 = be32_to_cpu(info_buf);
		} else {
			mask->spec_info_l1 = 0;
		}

		if (tb[BLOB_ACL_SPEC_INFO_L2]) {
			uint64_t info_buf;
			info_buf = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_L2]);
			key->spec_info_l2 = be32_to_cpu(info_buf);
			if (tb[BLOB_ACL_SPEC_INFO_L2_MASK]) {
				info_buf = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_L2_MASK]);
				mask->spec_info_l2 = be32_to_cpu(info_buf);
			} else {
				mask->spec_info_l2 = 0;
			}
		}
	}

	if (tb[BLOB_ACL_SPEC_INFO_L1] && tb[BLOB_ACL_SPEC_INFO_L2]) {
		if (tb[BLOB_ACL_SPEC_INFO_H1] && tb[BLOB_ACL_SPEC_INFO_H2]) {
			uint32_t info_h1, info_h2;

			info_h1 = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_H1]);
			info_h2 = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_H2]);

			key->spec_info_h1 = be32_to_cpu(info_h1);
			key->spec_info_h2 = be32_to_cpu(info_h2);

			if (tb[BLOB_ACL_SPEC_INFO_H1_MASK]) {
				info_h1 = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_H1_MASK]);
				mask->spec_info_h1 = be32_to_cpu(info_h1);
			} else {
				mask->spec_info_h1 = 0;
			}

			if (tb[BLOB_ACL_SPEC_INFO_H2_MASK]) {
				info_h2 = blobmsg_get_u32(tb[BLOB_ACL_SPEC_INFO_H2_MASK]);
				mask->spec_info_h2 = be32_to_cpu(info_h2);
			} else {
				mask->spec_info_h2 = 0;
			}
		}
	}

	if (tb[BLOB_ACL_PRIORITY]) {
		key->pkt_ctrl |= (blobmsg_get_u32(tb[BLOB_ACL_PRIORITY]) & 7) << 6;

		if (tb[BLOB_ACL_PRIORITY_EN])
			key->pkt_ctrl |= blobmsg_get_bool(tb[BLOB_ACL_PRIORITY_EN]) << 9;

		if (tb[BLOB_ACL_DSCP_REPLACE_EN])
			key->pkt_ctrl |= blobmsg_get_bool(tb[BLOB_ACL_DSCP_REPLACE_EN]) << 5;

		if (!tb[BLOB_ACL_PRIORITY_EN] && !tb[BLOB_ACL_DSCP_REPLACE_EN]) {
			pr_err("%s or %s must be true if %s is present\n",
				acl_add_blob_policy[BLOB_ACL_PRIORITY_EN].name,
				acl_add_blob_policy[BLOB_ACL_DSCP_REPLACE_EN].name,
				acl_add_blob_policy[BLOB_ACL_PRIORITY].name);
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	if (tb[BLOB_ACL_IS_IPV4])
		msg_add.is_ipv4 = blobmsg_get_bool(tb[BLOB_ACL_IS_IPV4]);

	if (tb[BLOB_ACL_IS_IPV6])
		msg_add.is_ipv6 = blobmsg_get_bool(tb[BLOB_ACL_IS_IPV6]);

	ret = acl_send_recv(&msg_add, sizeof(msg_add));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

enum acl_del_blob_id {
	BLOB_ACL_DEL_INDEX,
	BLOB_ACL_DEL_DIR,
	NUM_ACL_DEL_BLOB_IDS,
};

static const struct blobmsg_policy acl_del_blob_policy[NUM_ACL_DEL_BLOB_IDS] = {
	[BLOB_ACL_DEL_INDEX]		= { .name = "index",		.type = BLOBMSG_TYPE_INT32	},
	[BLOB_ACL_DEL_DIR]		= { .name = "dir",		.type = BLOBMSG_TYPE_INT32	},
};

static int acl_del(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ACL_DEL_BLOB_IDS] = { };
	struct acl_genl_msg msg_del = {
		.method = ACL_DEL,
	};
	int ret;

	blobmsg_parse(acl_del_blob_policy, NUM_ACL_DEL_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_ACL_DEL_DIR]) {
		pr_err("dir must be specified: 0 for iacl, others for eacl");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	msg_del.is_eacl = !!(blobmsg_get_u32(tb[BLOB_ACL_DEL_DIR]));

	if (!tb[BLOB_ACL_DEL_INDEX]) {
		pr_err("%s must be specified\n", acl_del_blob_policy[BLOB_ACL_DEL_INDEX].name);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	msg_del.index = blobmsg_get_u32(tb[BLOB_ACL_DEL_INDEX]);
	ret = acl_send_recv(&msg_del, sizeof(msg_del));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

enum acl_clear_blob_id {
	BLOB_ACL_CLEAR_DIR,
	NUM_ACL_CLEAR_BLOB_IDS,
};

static const struct blobmsg_policy acl_clear_blob_policy[NUM_ACL_CLEAR_BLOB_IDS] = {
	[BLOB_ACL_CLEAR_DIR]		= { .name = "dir",		.type = BLOBMSG_TYPE_INT32	},
};

static int acl_clear(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct blob_attr *tb[NUM_ACL_CLEAR_BLOB_IDS] = { };
	struct acl_genl_msg msg_clear = {
		.method = ACL_CLEAR,
	};
	int ret;

	blobmsg_parse(acl_clear_blob_policy, NUM_ACL_CLEAR_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_ACL_CLEAR_DIR]) {
		pr_err("dir must be specified: 0 for iacl, others for eacl");
		return UBUS_STATUS_INVALID_ARGUMENT;
	}
	msg_clear.is_eacl = !!(blobmsg_get_u32(tb[BLOB_ACL_CLEAR_DIR]));

	ret = acl_send_recv(&msg_clear, sizeof(msg_clear));
	ubus_err_code_reply(ctx, req, ret);
	return 0;
}

enum acl_set_mode_blob_id {
        BLOB_ACL_MODE_DIR,
        BLOB_ACL_MODE_V4_MODE,
        BLOB_ACL_MODE_V6_MODE,
        NUM_ACL_MODE_BLOB_IDS,
};

static const struct blobmsg_policy acl_set_mode_blob_policy[NUM_ACL_MODE_BLOB_IDS] = {
        [BLOB_ACL_MODE_DIR]             = { .name = "dir",              .type = BLOBMSG_TYPE_INT32      },
        [BLOB_ACL_MODE_V4_MODE]         = { .name = "v4_mode",          .type = BLOBMSG_TYPE_INT32      },
        [BLOB_ACL_MODE_V6_MODE]         = { .name = "v6_mode",          .type = BLOBMSG_TYPE_INT32      },
};

static int acl_set_mode(struct ubus_context *ctx, struct ubus_object *obj,
                     struct ubus_request_data *req, const char *method,
                     struct blob_attr *msg)
{
        struct blob_attr *tb[NUM_ACL_MODE_BLOB_IDS] = { };
        struct acl_genl_msg_add msg_mode = {
                .method = ACL_SET_MODE,
        };
        int ret;
        bool changed = false;

        blobmsg_parse(acl_set_mode_blob_policy, NUM_ACL_MODE_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

        if (!tb[BLOB_ACL_MODE_DIR]) {
                pr_err("dir must be specified: 0 for iacl, others for eacl");
                return UBUS_STATUS_INVALID_ARGUMENT;
        }
        msg_mode.is_eacl = !!(blobmsg_get_u32(tb[BLOB_ACL_MODE_DIR]));

        if (tb[BLOB_ACL_MODE_V4_MODE]) {
                changed = true;
                msg_mode.v4_mode = blobmsg_get_u32(tb[BLOB_ACL_MODE_V4_MODE]);
        } else {
                msg_mode.v4_mode = -1;
        }

        if (tb[BLOB_ACL_MODE_V6_MODE]) {
                changed = true;
                msg_mode.v6_mode = blobmsg_get_u32(tb[BLOB_ACL_MODE_V6_MODE]);
        } else {
                msg_mode.v6_mode = -1;
        }

        if (!changed)
                return UBUS_STATUS_INVALID_ARGUMENT;

        ret = acl_send_recv(&msg_mode, sizeof(msg_mode));
        ubus_err_code_reply(ctx, req, ret);
        return 0;
}

enum acl_dump_blob_id {
        BLOB_ACL_DUMP_DIR,
        NUM_ACL_DUMP_BLOB_IDS,
};

static const struct blobmsg_policy acl_dump_blob_policy[NUM_ACL_DUMP_BLOB_IDS] = {
        [BLOB_ACL_DUMP_DIR]             = { .name = "dir",              .type = BLOBMSG_TYPE_INT32      },
};

static int acl_table_dump(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
        struct blob_attr *tb[NUM_ACL_DUMP_BLOB_IDS] = { };
        struct acl_genl_msg msg_dump_table = {
                .method = ACL_DUMP,
        };
        int ret;

        blobmsg_parse(acl_dump_blob_policy, NUM_ACL_DUMP_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

        if (!tb[NUM_ACL_DUMP_BLOB_IDS])
                return UBUS_STATUS_INVALID_ARGUMENT;

        msg_dump_table.is_eacl = !!(blobmsg_get_u32(tb[BLOB_ACL_DUMP_DIR]));

        ret = acl_send_recv(&msg_dump_table, sizeof(msg_dump_table));
        ubus_err_code_reply(ctx, req, ret);

        return UBUS_STATUS_OK;
}

static int acl_table_dump_list(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
        struct blob_attr *tb[NUM_ACL_DUMP_BLOB_IDS] = { };
        struct acl_genl_msg msg_dump_table = {
                .method = ACL_DUMP_LIST,
        };
        int ret;

        blobmsg_parse(acl_dump_blob_policy, NUM_ACL_DUMP_BLOB_IDS, tb, blob_data(msg), blob_len(msg));

        if (!tb[NUM_ACL_DUMP_BLOB_IDS])
                return UBUS_STATUS_INVALID_ARGUMENT;

        msg_dump_table.is_eacl = !!(blobmsg_get_u32(tb[BLOB_ACL_DUMP_DIR]));

        ret = acl_send_recv(&msg_dump_table, sizeof(msg_dump_table));
        ubus_err_code_reply(ctx, req, ret);

        return UBUS_STATUS_OK;
}

static const struct ubus_method acl_methods[] = {
	UBUS_METHOD("add", acl_add, acl_add_blob_policy),
	UBUS_METHOD("del", acl_del, acl_del_blob_policy),
	UBUS_METHOD("clear", acl_clear, acl_clear_blob_policy),
	UBUS_METHOD("set_mode", acl_set_mode, acl_set_mode_blob_policy),
	UBUS_METHOD("dump", acl_table_dump, acl_dump_blob_policy),
	UBUS_METHOD("dump_list", acl_table_dump_list, acl_dump_blob_policy),
};

static struct ubus_object_type acl_object_type =
	UBUS_OBJECT_TYPE("acl", acl_methods);

static struct ubus_object acl_object = {
	.name = "dpns.acl",
	.type = &acl_object_type,
	.methods = acl_methods,
	.n_methods = ARRAY_SIZE(acl_methods),
};

int acl_init(void)
{
	return ubus_add_object(ubus_ctx_get(), &acl_object);
}

int acl_exit(void)
{
	return ubus_remove_object(ubus_ctx_get(), &acl_object);
}
