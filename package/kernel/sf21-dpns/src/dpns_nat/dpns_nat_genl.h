#ifndef __DPNS_NAT_GENL_H_
#define __DPNS_NAT_GENL_H_


struct nat_genl_msg {
	int nat_id;
	uint32_t method;
	uint32_t public_ip[4];
	uint32_t private_ip[4];
	uint32_t router_ip[4];
	uint32_t spl_value;
	uint16_t pkt_length;
	uint16_t spl_index;
	uint16_t stat_index;
	uint16_t pubmac_index;
	uint16_t primac_index;
	uint16_t public_port;
	uint16_t private_port;
	uint16_t router_port;
	uint16_t lf_mode;
	uint16_t hnat_mode;
	uint16_t index;
	uint16_t napt_add_mode;
	uint16_t pppoe_sid;
	uint8_t nat_mib_mode;
	uint8_t spl_cnt_mode;
	uint8_t spl_source;
	uint8_t soport_id;
	uint8_t doport_id;
	uint8_t drtmac_index;
	uint8_t srtmac_index;
	uint8_t repl_pri;
	uint8_t nat_offload_mode;
	char ifname[IFNAMSIZ];
	bool is_zero_lmt;
	bool spl_mode;
	bool offload_en;
	bool is_v6_mode;
	bool is_lf;
	bool is_udp;
	bool is_dnat;
	bool is_v6;
	bool is_get;
	bool is_lan;
	bool spl_en;
	bool stat_en;
	bool repl_pri_en;
} __packed;

enum nat_genl_method {
	NAT_DUMP_NAPT_COUNT,
	NAT_DUMP_NAPT_TB,
	NAT_HW_SEARCH,
	NAT_OFFLOAD_EN,
	NAT_MODE_SET,
	NAT_MODE_RESET,
	NAT_SUBNET,
	NAT_OVPORT_SET,
	NAT_OVPORT_GET,
	NAT_SPL_SET,
	NAT_NAPT_ADD_MODE_SET,
	NAT_NAPT_ADD,
	NAT_DUMP_BYID,
	NAT_UPDATE_BYID,
	NAT_ELKP_DELAY,
	NAT_CLEAN,
	NAT_DEL,
	NAT_ADD_PPPHDR,
	NAT_OFFLOAD_MODE,
};

struct nat_genl_resp {
	int32_t err;
	char msg[];
} __packed;

int dpns_nat_hw_search6(struct dpns_nat_priv *priv, struct nat_genl_msg *msg);
int dpns_nat_hw_search4(struct dpns_nat_priv *priv, struct nat_genl_msg *msg);
int dpns_nat_mode_set(struct dpns_nat_priv *priv, struct nat_genl_msg *msg);
int dpns_nat_subnet_op(struct dpns_nat_priv *priv, struct nat_genl_msg *msg);
int dpns_nat_spl_set(struct dpns_nat_priv *priv, struct nat_genl_msg *msg);
void dpns_nat_offload_tuple_set(uint16_t l2offload_mode, struct nat_hash_tuple *tuple);
void dpns_nat_hnat_tuple_set(uint16_t hnat_mode, struct nat_hash_tuple *tuple, bool is_dnat);
#endif /* __NAT_MSG_H */
