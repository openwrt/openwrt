#ifndef _SE_COMMON_H_
#define _SE_COMMON_H_

#define CONFIG_BCAST_DA_SRCH			BIT(9)		// bcast dmac search enable
#define CONFIG_X_UNAUTH_FROCE_UP		BIT(20)		// enable 802X unauth force up

#define SE_MAC_AGE_REG_OFFSET_RAM		0x4		//total 64reg; end addr is 0x1980ff

typedef struct se_l2_conf {
	uint8_t hnat_mode;
} se_l2_conf_t;

typedef struct tbl_mac {
	uint64_t mib_id		:9;
	uint64_t mib_en		:3;
	uint64_t spl_id		:8;
	uint64_t spl_en		:1;
	uint64_t age_en		:1;
	uint64_t da_cml		:2;
	uint64_t sa_cml		:2;
	uint64_t l3_en		:1;
	uint64_t repeater_id	:4;
	uint64_t sta_id		:10;
	uint64_t port_bitmap	:27;
	uint64_t mac		:48;
	uint64_t vid		:12;
	uint64_t vlan_offload_en:1;
	uint64_t valid		:1;
	uint64_t rsv		:30;
} __packed tbl_mac_t;

union mac_table_cfg {
	tbl_mac_t table;
	uint32_t data[5];
};

struct l2_mac_genl_msg_add{
	uint64_t	mac;
	uint32_t 	public_ip[4];
	uint32_t 	private_ip[4];
	uint32_t 	router_ip[4];
	uint32_t	method;
	uint32_t	scredit;
	uint32_t	dcredit;
	uint32_t	value;
	uint16_t 	public_port;
	uint16_t 	private_port;
	uint16_t 	router_port;
	uint16_t	vid;
	uint16_t	start;
	uint16_t	end;
	uint16_t	sta_id;
	uint16_t	repeater_id;
	uint16_t	mib_index;
	uint8_t		mib_mode;
	uint8_t		spl_index;
	uint8_t		da_cml;
	uint8_t		sa_cml;
	uint8_t		port;
	uint8_t		mib_op;
	bool		l2_spl_mode;
	bool		mage_en;
	bool		l3_en;
	bool		vlan_en;
	bool		enable;
	bool		mib_en;
	bool		is_v6;
	bool 		is_udp;
	bool 		is_dnat;
} __packed;

enum l2_mac_genl_method{
	L2_MAC_ADD,
	L2_MAC_MIB_EN,
	L2_MAC_SPL_EN,
	L2_MAC_AGE_EN,
	L2_MAC_DEL,
	L2_MAC_SET_AGEING_EN,
	L2_MAC_SET_LEARNING_EN,
	L2_MAC_SET_AGE_TIME,
	L2_MAC_DUMP_MAC_TB,
	L2_MAC_DUMP_SPL_TB,
	L2_MAC_CLEAR,
	L2_MAC_NUM_DUMP,
	NAT_MIB_EN,
	DPNS_MIB_EN,
	L2_MAC_HIT,
};

struct l2_mac_genl_resp{
	int32_t err;
	char msg[];
} __packed;

#endif // _SE_COMMON_H_
