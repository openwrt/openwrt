#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/inet.h>

#include "dpns_common.h"
#include "dpns_acl_msg.h"

extern void acl_add(struct acl_genl_msg_add *msg);
extern int acl_del(struct acl_genl_msg *msg);
extern void acl_clear(bool is_eacl);
extern void acl_dump_table(bool is_eacl);
extern void acl_dump_list(bool is_eacl);
extern void acl_set_mode(struct acl_genl_msg_add *msg);

enum acl_blob_id {
	BLOB_ACL_INDEX,
	BLOB_ACL_POLICY,
	BLOB_ACL_DIR,   /* 0: IACL , 1: EACL */
	BLOB_ACL_V4_MODE,
	BLOB_ACL_V6_MODE,
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
	NUM_ACL_BLOB_IDS,
};

static const char* acl_args[] = {"index", "policy", "dir", "v4_mode", "v6_mode", "priority", "priority_en", "dscp_replace_en", "new_id", "smac", "smac_mask", "dmac", "dmac_mask", "sip", "sip6", "sip_mask", "dip", "dip6", "dip_mask", "iport_id", "oport_id", "msport_id", "sport", "sport_mask", "dport", "dport_mask", "ovid", "tos", "ip_proto",  "cast", "vlan", "etype", "iptype", "frag", "l4_type", "mf_action", "spl", "spl_index", "spec_info_l1", "spec_info_l2", "spec_info_h1", "spec_info_h2", "l1_mask", "l2_mask", "h1_mask", "h2_mask", "offset0", "offset1", "offset2", "offset3", "offset4", "offset5", "offset6", "offset7", "upsend_spl_only", "spl_id"};

static u32 blobmsg_get_u32(char* value)
{
	return simple_strtoul(value, &value, 10);
}

static u32 blobmsg_get_bool(char* value)
{
	return blobmsg_get_u32(value) ? 1 : 0;
}

static int parse_ip(void *buf, const char *str, bool is_ipv6)
{
	if (is_ipv6) {
		if (!in6_pton(str, strlen(str), buf, -1, NULL))
			goto invalid_ip;
	} else {
		if (!in4_pton(str, strlen(str), buf, -1, NULL))
			goto invalid_ip;
	}
	return 0;

invalid_ip:
	printk("invalid IP address: %s\n", str);
	return -1;
}

static int parse_mac(u8* pMac, char* szMac)
{
	if(sscanf(szMac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
		&pMac[0], &pMac[1], &pMac[2], &pMac[3], &pMac[4], &pMac[5]))
		goto parse_mac_false;

	return 0;

parse_mac_false:
	printk("parse_mac_error");
	return -1;
}

static void acl_proc_add(char *add_values[])
{
	struct acl_genl_msg_add msg_add = {
		.method = ACL_ADD,
	};
	int ret, i;
	struct acl_data_t *key = &msg_add.key, *mask = &msg_add.mask;

	if (!add_values[BLOB_ACL_DIR]) {
		printk("dir must be specified: 0 for iacl, others for eacl\n");
		goto error;
	}
	if (!add_values[BLOB_ACL_POLICY]) {
		printk("%s must be specified\n", add_values[BLOB_ACL_POLICY]);
		goto error;
	}

	memset(mask, 0xff, sizeof(*mask));

	if (!add_values[BLOB_ACL_INDEX]) {
		msg_add.index = -1;
	} else {
		msg_add.index = simple_strtoul(add_values[BLOB_ACL_INDEX], &add_values[BLOB_ACL_INDEX], 10);
		if (msg_add.index > INDEX_MAX) {
			printk("index should be in the range of 16K\n");
			goto error;
		}
	}

	msg_add.is_eacl = !!(simple_strtoul(add_values[BLOB_ACL_DIR], &add_values[BLOB_ACL_DIR], 10));

	key->policy = simple_strtoul(add_values[BLOB_ACL_POLICY], &add_values[BLOB_ACL_POLICY], 10);

	if (msg_add.is_eacl) {
		if (key->policy == ACT_MIRROR2CPU || key->policy == ACT_MIRROR_INGRESS || key->policy == ACT_MIRROR_EGRESS) {
			printk("EACL not support policy%d\n", key->policy);
		}
	}

	if (key->policy == ACT_REDIRECT || key->policy == ACT_MIRROR_INGRESS || key->policy == ACT_MIRROR_EGRESS) {
		if (add_values[BLOB_ACL_NEW_ID]) {
			key->pkt_ctrl = simple_strtoul(add_values[BLOB_ACL_NEW_ID], &add_values[BLOB_ACL_NEW_ID], 10);
			if (key->policy == ACT_MIRROR_EGRESS && add_values[BLOB_ACL_MSPORT_ID]) {
				key->pkt_ctrl |= simple_strtoul(add_values[BLOB_ACL_MSPORT_ID], &add_values[BLOB_ACL_MSPORT_ID], 10) << 5;
			}
		} else {
			printk("%s must be specified when %s is %u\n",
				add_values[BLOB_ACL_NEW_ID],
				add_values[BLOB_ACL_POLICY], ACT_REDIRECT);
			goto error;
		}
	}

	if (key->policy == ACT_SPL) {
		if(add_values[BLOB_ACL_SPEED_SET]) {
			if (!add_values[BLOB_ACL_SPEED_INDEX]) {
				printk("when set speed, must choose a index\n");
				goto error;
			} else {
				msg_add.spl_index = blobmsg_get_u32(add_values[BLOB_ACL_SPEED_INDEX]);
				if (msg_add.spl_index > ACL_SPL_TB_SZ) {
					printk("spl_index should in range 0~31\n");
					goto error;
				}
			}
			msg_add.spl = blobmsg_get_u32(add_values[BLOB_ACL_SPEED_SET]);
			if (msg_add.spl > SPL_MAX) {
				printk("SPL max is 2^24-1, out of range!\n");
				goto error;
			}
		} else {
			msg_add.spl = -1;
		}

		if (add_values[BLOB_ACL_UPSEND_SPL_ONLY])
			key->pkt_ctrl |= blobmsg_get_bool(add_values[BLOB_ACL_UPSEND_SPL_ONLY]) << 5;

		if (add_values[BLOB_ACL_SPL_ID])
			key->pkt_ctrl |= blobmsg_get_u32(add_values[BLOB_ACL_SPL_ID]) & 0x1f;
	}

	if (add_values[BLOB_ACL_SRC_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, add_values[BLOB_ACL_SRC_MAC]);
		if (ret) {
			printk("%d goto error", __LINE__);
			goto error;
		}

		key->smac = be64_to_cpu(mac_buf);

		if (add_values[BLOB_ACL_SRC_MAC_MASK]) {
			ret = parse_mac((uint8_t *)&mac_buf + 2, add_values[BLOB_ACL_SRC_MAC_MASK]);
			if (ret) {
				printk("%d goto error", __LINE__);
				goto error;
			}

			mask->smac = be64_to_cpu(mac_buf);
		} else {
			mask->smac = 0;
		}
	}

	if (add_values[BLOB_ACL_DST_MAC]) {
		uint64_t mac_buf = 0;
		ret = parse_mac((uint8_t *)&mac_buf + 2, add_values[BLOB_ACL_DST_MAC]);
		if (ret) {
			printk("%d goto error", __LINE__);
			goto error;
		}

		key->dmac = be64_to_cpu(mac_buf);

		if (add_values[BLOB_ACL_DST_MAC_MASK]) {
			ret = parse_mac((uint8_t *)&mac_buf + 2, add_values[BLOB_ACL_DST_MAC_MASK]);
			if (ret) {
				printk("%d goto error", __LINE__);
				goto error;
			}

			mask->dmac = be64_to_cpu(mac_buf);
		} else {
			mask->dmac = 0;
		}
	}

	if (add_values[BLOB_ACL_SRC_IP]) {
		uint32_t ip_buf = 0;
		ret = parse_ip(&ip_buf, add_values[BLOB_ACL_SRC_IP], 0);
		if (ret) {
			printk("%d goto error", __LINE__);
			goto error;
		}

		key->sip = be32_to_cpu(ip_buf);

		if (add_values[BLOB_ACL_SRC_IP_MASK]) {
			ip_buf = 0;
			ret = parse_ip(&ip_buf, add_values[BLOB_ACL_SRC_IP_MASK], 0);
			if (ret) {
				printk("%d goto error", __LINE__);
				goto error;
			}

			mask->sip = be32_to_cpu(ip_buf);
		} else {
			mask->sip = 0;
		}
		msg_add.is_ipv4 = true;
	}

	if (add_values[BLOB_ACL_DST_IP]) {
		uint32_t ip_buf = 0;
		ret = parse_ip(&ip_buf, add_values[BLOB_ACL_DST_IP], 0);
		if (ret) {
			printk("%d goto error", __LINE__);
			goto error;
		}

		key->dip = be32_to_cpu(ip_buf);

		if (add_values[BLOB_ACL_DST_IP_MASK]) {
			ip_buf = 0;
			ret = parse_ip(&ip_buf, add_values[BLOB_ACL_DST_IP_MASK], 0);
			if (ret) {
				printk("%d goto error", __LINE__);
				goto error;
			}

			mask->dip = be32_to_cpu(ip_buf);
		} else {
			mask->dip = 0;
		}
		msg_add.is_ipv4 = true;
	}

	if (add_values[BLOB_ACL_SRC_IP6]) {
		uint64_t ip_buf[2] = {0};
		ret = parse_ip(ip_buf, add_values[BLOB_ACL_SRC_IP6], 1);
		if (ret) {
			printk("%d goto error", __LINE__);
			goto error;
		}

		key->sip_l = be64_to_cpu(ip_buf[1]);
		key->sip_h = be64_to_cpu(ip_buf[0]);

		if (add_values[BLOB_ACL_SRC_IP_MASK]) {
			memset(ip_buf, 0, sizeof(ip_buf));
			ret = parse_ip(ip_buf, add_values[BLOB_ACL_SRC_IP_MASK], 1);
			if (ret) {
				printk("%d goto error", __LINE__);
				goto error;
			}
			mask->sip_l = be64_to_cpu(ip_buf[1]);
			mask->sip_h = be64_to_cpu(ip_buf[0]);
		} else {
			mask->sip_l = 0;
			mask->sip_h = 0;
		}
		msg_add.is_ipv6 = true;
	}

	if (add_values[BLOB_ACL_DST_IP6]) {
		uint64_t ip_buf[2] = {0};
		ret = parse_ip(ip_buf, add_values[BLOB_ACL_DST_IP6], 1);
		if (ret) {
			printk("%d goto error", __LINE__);
			goto error;
		}

		key->dip_l = be64_to_cpu(ip_buf[1]);
		key->dip_h = be64_to_cpu(ip_buf[0]);

		if (add_values[BLOB_ACL_DST_IP_MASK]) {
			memset(ip_buf, 0, sizeof(ip_buf));
			ret = parse_ip(ip_buf, add_values[BLOB_ACL_DST_IP_MASK], 1);

			if (ret) {
				printk("%d goto error", __LINE__);
				goto error;
			}

			mask->dip_l = be64_to_cpu(ip_buf[1]);
			mask->dip_h = be64_to_cpu(ip_buf[0]);
		} else {
			mask->dip_l = 0;
			mask->dip_h = 0;
		}
		msg_add.is_ipv6 = true;
	}

	if (add_values[BLOB_ACL_SRC_PORT]) {
		key->sport = blobmsg_get_u32(add_values[BLOB_ACL_SRC_PORT]);
		if (add_values[BLOB_ACL_SRC_PORT_MASK])
			mask->sport = blobmsg_get_u32(add_values[BLOB_ACL_SRC_PORT_MASK]);
		else
			mask->sport = 0;
	}

	if (add_values[BLOB_ACL_DST_PORT]) {
		key->dport = blobmsg_get_u32(add_values[BLOB_ACL_DST_PORT]);
		if (add_values[BLOB_ACL_DST_PORT_MASK])
			mask->dport = blobmsg_get_u32(add_values[BLOB_ACL_DST_PORT_MASK]);
		else
			mask->dport = 0;
	}

	if (add_values[BLOB_ACL_TOS]) {
		key->tos_pri = blobmsg_get_u32(add_values[BLOB_ACL_TOS]);
		mask->tos_pri = 0;
	}

	if (add_values[BLOB_ACL_IPPROTO]) {
		key->protocol = blobmsg_get_u32(add_values[BLOB_ACL_IPPROTO]);
		mask->protocol = 0;
	}

	if (add_values[BLOB_ACL_OVID]) {
		key->ovid = blobmsg_get_u32(add_values[BLOB_ACL_OVID]);
		mask->ovid = 0;
	}

	if (add_values[BLOB_ACL_IPORT_ID]) {
		key->ivport_id = blobmsg_get_u32(add_values[BLOB_ACL_IPORT_ID]);
		mask->ivport_id = 0;
	}

	if (add_values[BLOB_ACL_OPORT_ID]) {
		key->ovport_id = blobmsg_get_u32(add_values[BLOB_ACL_OPORT_ID]);
		mask->ovport_id = 0;
	}

	if (add_values[BLOB_ACL_FTYPE_01]) {
		key->frame_type_1_0 = blobmsg_get_u32(add_values[BLOB_ACL_FTYPE_01]);
		mask->frame_type_1_0 = 0;
	}

	for (i = 0; i < 8; i++) {
		if (add_values[BLOB_ACL_PKT_OFFSET_0 + i]) {
			msg_add.offset[i] = blobmsg_get_u32(add_values[BLOB_ACL_PKT_OFFSET_0 + i]);
		} else {
			msg_add.offset[i] = -1;
			break;
		}
	}

	if (add_values[BLOB_ACL_FTYPE_23]) {
		key->frame_type_3_2 = blobmsg_get_u32(add_values[BLOB_ACL_FTYPE_23]);
		mask->frame_type_3_2 = 0;
	}

	if (add_values[BLOB_ACL_FTYPE_45]) {
		key->frame_type_5_4 = blobmsg_get_u32(add_values[BLOB_ACL_FTYPE_45]);
		mask->frame_type_5_4 = 0;
	}

	if (add_values[BLOB_ACL_FTYPE_67]) {
		key->frame_type_7_6 = blobmsg_get_u32(add_values[BLOB_ACL_FTYPE_67]);
		mask->frame_type_7_6 = 0;
	}

	if (add_values[BLOB_ACL_FTYPE_89]) {
		key->frame_type_9_8 = blobmsg_get_u32(add_values[BLOB_ACL_FTYPE_89]);
		mask->frame_type_9_8 = 0;
	}

	if (add_values[BLOB_ACL_FTYPE_1011]) {
		key->frame_type_11_10 = blobmsg_get_u32(add_values[BLOB_ACL_FTYPE_1011]);
		mask->frame_type_11_10 = 0;
	}

	if (add_values[BLOB_ACL_ACTION]) {
		key->mf_action = blobmsg_get_u32(add_values[BLOB_ACL_ACTION]);
		mask->mf_action = 0;
	}

	if (add_values[BLOB_ACL_SPEC_INFO_L1]) {
		uint32_t info_buf;
		info_buf = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_L1]);
		key->spec_info_l1 = be32_to_cpu(info_buf);
		if (add_values[BLOB_ACL_SPEC_INFO_L1_MASK]) {
			info_buf = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_L1_MASK]);
			mask->spec_info_l1 = be32_to_cpu(info_buf);
		} else {
			mask->spec_info_l1 = 0;
		}

		if (add_values[BLOB_ACL_SPEC_INFO_L2]) {
			uint64_t info_buf;
			info_buf = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_L2]);
			key->spec_info_l2 = be32_to_cpu(info_buf);
			if (add_values[BLOB_ACL_SPEC_INFO_L2_MASK]) {
				info_buf = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_L2_MASK]);
				mask->spec_info_l2 = be32_to_cpu(info_buf);
			} else {
				mask->spec_info_l2 = 0;
			}
		}
	}

	if (add_values[BLOB_ACL_SPEC_INFO_L1] && add_values[BLOB_ACL_SPEC_INFO_L2]) {
		if (add_values[BLOB_ACL_SPEC_INFO_H1] && add_values[BLOB_ACL_SPEC_INFO_H2]) {
			uint32_t info_h1, info_h2;

			info_h1 = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_H1]);
			info_h2 = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_H2]);

			key->spec_info_h1 = be32_to_cpu(info_h1);
			key->spec_info_h2 = be32_to_cpu(info_h2);

			if (add_values[BLOB_ACL_SPEC_INFO_H1_MASK]) {
				info_h1 = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_H1_MASK]);
				mask->spec_info_h1 = be32_to_cpu(info_h1);
			} else {
				mask->spec_info_h1 = 0;
			}

			if (add_values[BLOB_ACL_SPEC_INFO_H2_MASK]) {
				info_h2 = blobmsg_get_u32(add_values[BLOB_ACL_SPEC_INFO_H2_MASK]);
				mask->spec_info_h2 = be32_to_cpu(info_h2);
			} else {
				mask->spec_info_h2 = 0;
			}
		}
	}

	if (add_values[BLOB_ACL_PRIORITY]) {
		key->pkt_ctrl |= (blobmsg_get_u32(add_values[BLOB_ACL_PRIORITY]) & 7) << 6;

		if (add_values[BLOB_ACL_PRIORITY_EN])
			key->pkt_ctrl |= blobmsg_get_bool(add_values[BLOB_ACL_PRIORITY_EN]) << 9;

		if (add_values[BLOB_ACL_DSCP_REPLACE_EN])
			key->pkt_ctrl |= blobmsg_get_bool(add_values[BLOB_ACL_DSCP_REPLACE_EN]) << 5;

		if (!add_values[BLOB_ACL_PRIORITY_EN] && !add_values[BLOB_ACL_DSCP_REPLACE_EN]) {
			printk("%s or %s must be true if %s is present\n",
				add_values[BLOB_ACL_PRIORITY_EN],
				add_values[BLOB_ACL_DSCP_REPLACE_EN],
				add_values[BLOB_ACL_PRIORITY]);

				goto error;
		}
	}

	acl_add(&msg_add);

	printk("add complete! \n");

	return;

error:
	printk("args input false\n");
}

static void acl_proc_clear(char *values[])
{
	int dir;

	if (!values[BLOB_ACL_DIR])
		goto error;

	dir = !!blobmsg_get_u32(values[BLOB_ACL_DIR]);

	acl_clear(dir);

	printk("clear %d \n", dir);
	return;
error:
	printk("args false\n");
}

static void acl_proc_delete(char *values[])
{
	struct acl_genl_msg msg_del = {
		.method = ACL_DEL,
	};
	int dir, index;

	if (!values[BLOB_ACL_DIR]) {
		printk("dir must be specified: 0 for iacl, others for eacl");
		return;
	}

	if (!values[BLOB_ACL_INDEX]) {
		printk("%s must be specified\n", acl_args[BLOB_ACL_INDEX]);
		return;
	}

	dir = blobmsg_get_u32(values[BLOB_ACL_DIR]);
	index = blobmsg_get_u32(values[BLOB_ACL_INDEX]);

	msg_del.is_eacl = dir;
	msg_del.index = index;

	acl_del(&msg_del);
}

static void acl_proc_set_mode(char *values[])
{
	struct acl_genl_msg_add msg_mode = {
		.method = ACL_SET_MODE,
	};

	if (!values[BLOB_ACL_DIR]) {
		printk("dir must be specified: 0 for iacl, others for eacl");
		return;
	}

	if( !values[BLOB_ACL_V4_MODE] && !values[BLOB_ACL_V6_MODE]) {
		printk("please choose v4 or v6\n");
		return;
	}

	msg_mode.is_eacl = !!(blobmsg_get_u32(values[BLOB_ACL_DIR]));

	if (values[BLOB_ACL_V4_MODE]) {
		msg_mode.v4_mode = blobmsg_get_u32(values[BLOB_ACL_V4_MODE]);
	} else {
		msg_mode.v4_mode = -1;
	}

	if (values[BLOB_ACL_V6_MODE]) {
		msg_mode.v6_mode = blobmsg_get_u32(values[BLOB_ACL_V6_MODE]);
	} else {
		msg_mode.v6_mode = -1;
	}

	acl_set_mode(&msg_mode);
}

static void acl_proc_dump_table(char *values[])
{
	int dir;

	if (!values[BLOB_ACL_DIR])
		goto error;

	dir = !!blobmsg_get_u32(values[BLOB_ACL_DIR]);

	acl_dump_table(dir);

	return;
error:
	printk("args false\n");
}

static void acl_proc_dump_list(char *values[])
{
	int dir;

	if (!values[BLOB_ACL_DIR])
		goto error;

	dir = !!blobmsg_get_u32(values[BLOB_ACL_DIR]);

	acl_dump_list(dir);

	return;
error:
	printk("args false\n");
}

static void acl_proc_help(void)
{
	printk("you can use as these: \n"
		"add : you can use these args : index, policy, dir, priority, priority_en, dscp_replace_en, new_id, smac, smac_mask, dmac, dmac_mask, sip, sip6, sip_mask, dip, dip6, dip_mask, iport_id, oport_id, msport_id, sport, sport_mask, dport, dport_mask, ovid, tos, ip_proto, cast, vlan, etype, iptype, frag, l4_type, mf_action, spl, spl_index, spec_info_l1, spec_info_l2, spec_info_h1, spec_info_h2, l1_mask, l2_mask, h1_mask, h2_mask, offset0, offset1, offset2, offset3, offset4, offset5, offset6, offset7, upsend_spl_only, spl_id \n");
	printk(	"you can use add as these: \n"
		"echo add dir 0 policy 1 sip 192.168.1.0 sip_mask 0.0.0.255 > proc/dpns_acl (to drop frames from 192.168.1.0/24 )\n"
		"echo add dir 0 policy 5 iport_id 0 new_id 1 > proc/dpns_acl (send frames which receive by port 0 to port 1)\n"
		"echo add dir 0 policy 6 oport_id 1 new_id 0 > proc/dpns_acl (send frames which send by port 1 to port 0)\n");
	printk(	"clear: you can use these to clear iacl or eacl\n"
		"echo clear dir [dir] > proc/dpns_acl \n"
		"delete: you can use these to delete one entry\n"
		"echo delete dir [dir] index [index] > proc/dpns_acl\n"
		"dump: echo dump_table dir [dir] > proc/dpns_acl\n"
		"dump_list: echo dump_list dir [dir] > proc/dpns_acl\n"
		"set_mode: echo set_mode dir [dir] v4_mode [v4_mode] v6_mode [v6_mode]\n");
}

void proc_args_resolve(const char *args[], char *values[], char *arg, char *value, int size) {
	int i;

	for(i = 0; i < size; i ++){
		if (strcmp(arg, args[i]) == 0) {
			values[i] = value;
			break;
		}
	}
}

static void acl_proc_resolve_args(char *values[], char *str, char* str_end, char **cmd){
	char *arg = NULL, *value = NULL;
	int i = 0;

	*cmd = strsep(&str, "\t \n");

	while (str < str_end) {
		if(i >= NUM_ACL_BLOB_IDS)
			goto arg_num_error;
		i ++;

		arg = strsep(&str, "\t \n");
		if(!arg)
			return;
		value = strsep(&str, "\t \n");
		if(!value)
			goto arg_error;
		proc_args_resolve(acl_args, values, arg, value, NUM_ACL_BLOB_IDS);
	}
	return;
arg_num_error:
	printk("to many args\n");
	return;
arg_error:
	printk("args false\n");
}

static ssize_t acl_proc(struct file *filp, const char *buffer, size_t count, loff_t *offp)
{
	char tmpbuf[512] = {'\0'};
	char *values[NUM_ACL_BLOB_IDS] = {0};
	char *str, *cmd, *str_end;

	if (count >= 512)
		goto error;

	if(!buffer || copy_from_user(tmpbuf, buffer, count) != 0)
		return 0;

	str = tmpbuf;
	str_end = tmpbuf + count;

	acl_proc_resolve_args(values, str, str_end, &cmd);

	if(strcmp(cmd, "add") == 0) {
		acl_proc_add(values);
	}
	else if(strcmp(cmd, "clear") == 0) {
		acl_proc_clear(values);
	}
	else if(strcmp(cmd, "delete") == 0) {
		acl_proc_delete(values);
	}
	else if(strncmp(cmd, "help", 4) == 0) {
		acl_proc_help();
	}
	else if(strcmp(cmd, "dump_table") == 0) {
		acl_proc_dump_table(values);
	}
	else if(strcmp(cmd, "dump_list") == 0) {
		acl_proc_dump_list(values);
	}
	else if(strcmp(cmd, "set_mode") == 0) {
		acl_proc_set_mode(values);
	}
	else {
		goto error;
	}

	return count;
error:
	printk("don't have this cmd!\n");
	return -1;
}

const struct proc_ops acl_ctrl = {
	.proc_write = acl_proc,
};

EXPORT_SYMBOL(acl_ctrl);
