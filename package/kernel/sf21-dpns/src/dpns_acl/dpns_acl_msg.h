#ifndef __DPNS_ACL_MSG_H
#define __DPNS_ACL_MSG_H

#define SPL_MAX		16777216	/* 2^24 */
#define INDEX_MAX	0x4000		/* 16K */
#define PKT_OFFSET_MAX	1024
#define ACL_SPL_TB_SZ	32

struct acl_key_mode0 {
 	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 1;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_5_4		: 2;
	uint64_t frame_type_7_6		: 2;
	uint64_t frame_type_9_8		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t spec_info		: 32;
} __packed;//<=72

struct acl_key_v4_mode1 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv0			: 1;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t dip			: 32;
	uint64_t spec_info		: 8;
} __packed;//<=72

struct acl_key_v4_mode2 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 1;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip			: 32;
	uint64_t spec_info		: 8;
} __packed;//<=72

struct acl_key_v6_mode2 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 1;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip_l			: 64;
	uint64_t sip_h			: 64;
	uint64_t dip_l			: 64;
	uint64_t dip_h			: 64;
} __packed;//<=288

struct acl_key_v4_mode3 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip			: 32;
	uint64_t dip			: 32;
	uint64_t sport			: 16;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t ovid			: 12;
} __packed;//<=144

struct acl_key_v6_mode3 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 2;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_5_4		: 2;
	uint64_t frame_type_7_6		: 2;
	uint64_t frame_type_9_8		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t dip_l			: 64;
	uint64_t dip_h			: 64;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t tos_pri		: 3;
	uint64_t ovid			: 12;
	uint64_t spec_info		: 32;
} __packed;//<=288

struct acl_key_v4_mode4_v6_mode1 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 13;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_5_4		: 2;
	uint64_t frame_type_7_6		: 2;
	uint64_t frame_type_9_8		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t smac			: 48;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t ovid			: 12;
	uint64_t spec_info_l		: 64;
	uint64_t spec_info_h		: 64;
} __packed;//<=288

struct acl_key_v4_mode5 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 2;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_5_4		: 2;
	uint64_t frame_type_7_6		: 2;
	uint64_t frame_type_9_8		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t smac			: 48;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip			: 32;
	uint64_t dip			: 32;
	uint64_t sport			: 16;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t tos_pri		: 3;
	uint64_t ovid			: 12;
	uint64_t spec_info		: 32;
} __packed;//<=288

struct acl_key_v4_mode6 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv0			: 64;
	uint64_t rsv1			: 64;
	uint64_t rsv2			: 64;
	uint64_t rsv3			: 2;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_5_4		: 2;
	uint64_t frame_type_7_6		: 2;
	uint64_t frame_type_9_8		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t smac			: 48;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip			: 32;
	uint64_t dip			: 32;
	uint64_t sport			: 16;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t tos_pri		: 3;
	uint64_t ovid			: 12;
	uint64_t spec_info_l		: 64;
	uint64_t spec_info_h		: 64;
} __packed;//<=576

struct acl_key_v4_mode7 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv0			: 8;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t smac			: 48;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip			: 32;
	uint64_t rsv1			: 32;
	uint64_t rsv2			: 64;
	uint64_t dip			: 32;
	uint64_t rsv3			: 32;
	uint64_t rsv4			: 64;
	uint64_t sport			: 16;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t tos_pri		: 3;
	uint64_t ovid			: 12;
	uint64_t spec_info_l		: 64;
	uint64_t spec_info_h		: 64;
} __packed;//<=576

struct acl_key_v6_mode7 {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t rsv			: 8;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t smac			: 48;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip_l			: 64;
	uint64_t sip_h			: 64;
	uint64_t dip_l			: 64;
	uint64_t dip_h			: 64;
	uint64_t sport			: 16;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t tos_pri		: 3;
	uint64_t ovid			: 12;
	uint64_t spec_info_l		: 64;
	uint64_t spec_info_h		: 64;
} __packed;//<=576

struct acl_data_t {
	uint64_t policy			: 3;
	uint64_t pkt_ctrl		: 10;
	uint64_t l3_hit			: 1;
	uint64_t mf_action		: 3;
	uint64_t frame_type_1_0		: 2;
	uint64_t frame_type_3_2		: 2;
	uint64_t frame_type_5_4		: 2;
	uint64_t frame_type_7_6		: 2;
	uint64_t frame_type_9_8		: 2;
	uint64_t frame_type_11_10	: 2;
	uint64_t smac			: 48;
	uint64_t dmac			: 48;
	uint64_t ivport_id		: 5;
	uint64_t ovport_id		: 5;
	uint64_t sip			: 32;
	uint64_t dip			: 32;
	uint64_t sip_l			: 64;
	uint64_t sip_h			: 64;
	uint64_t dip_l			: 64;
	uint64_t dip_h			: 64;
	uint64_t sport			: 16;
	uint64_t dport			: 16;
	uint64_t protocol		: 8;
	uint64_t tos_pri		: 3;
	uint64_t ovid			: 12;
	uint64_t rsv0			: 2;
	uint64_t spec_info_l1		: 32;
	uint64_t spec_info_l2		: 32;
	uint64_t spec_info_h1		: 32;
	uint64_t spec_info_h2		: 32;
} __packed;

enum acl_action {
	ACT_PASS,
	ACT_DROP,
	ACT_TRAP,
	ACT_REDIRECT,
	ACT_MIRROR2CPU,
	ACT_MIRROR_INGRESS,
	ACT_MIRROR_EGRESS,
	ACT_SPL,
};

struct acl_genl_msg {
	uint32_t method;
	uint32_t index;
	bool is_eacl;
} __packed;

struct acl_genl_msg_add {
	uint32_t method;
	uint32_t index;
	bool is_eacl;	/* 0: IACL, 1: EACL */
	bool is_ipv4;
	bool is_ipv6;
	uint32_t v4_mode;
	uint32_t v6_mode;
	uint32_t spl;
	uint32_t spl_index;
	uint32_t offset[8];
	struct acl_data_t key;
	struct acl_data_t mask;
} __packed;

enum acl_genl_method {
	ACL_ADD,
	ACL_DEL,
	ACL_CLEAR,
	ACL_SET_MODE,
	ACL_DUMP,
	ACL_DUMP_LIST,
};

enum acl_w_addr {
	  V4_V6_MODE0 = 0,
	  V4_MODE1,
	  V4_MODE2,
	  V4_MODE3 = 4,
	  V4_MODE4_V6_MODE1 = 8,
	  V4_MODE5,
	  V6_MODE2,
	  V6_MODE3,
	  V4_MODE7,
	  V6_MODE7,
};

enum struct_size {
	SZ_9B = 9,
	SZ_18B = 18,
	SZ_36B = 36,
	SZ_72B = 72,
};

enum acl_dir {
	DIR_IACL,
	DIR_EACL,
};

struct acl_genl_resp {
	int32_t err;
	char msg[];
} __packed;

#endif
