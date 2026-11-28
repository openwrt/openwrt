#ifndef __DPNS_VLAN_GENL_H_
#define __DPNS_VLAN_GENL_H_

struct vlan_genl_msg {
	uint32_t method;
	uint32_t ivid;
	uint32_t ovid;
	uint32_t dt_otag;
	uint32_t dt_potag;
	uint32_t sot_otag;
	uint32_t sot_potag;
	uint32_t sit_otag;
	uint32_t sit_potag;
	uint32_t un_otag;
	uint32_t un_potag;
	uint32_t def_action;
	uint32_t port_bitmap;
	uint32_t credit;
	uint32_t un_bitmap;
	uint32_t old_ivid;
	uint32_t old_ovid;
	uint32_t new_ivid;
	uint32_t new_ovid;
	uint32_t old_ivid_mask;
	uint32_t old_ovid_mask;
	uint32_t evlan_act_idx;
	uint32_t itpid;
	uint32_t otpid;
	uint32_t tpid;
	uint16_t vid;
	uint16_t sot_action;
	uint16_t psot_action;
	uint16_t dt_action;
	uint16_t pdt_action;
	uint16_t evlan_def_action;
	uint8_t table_name_index;
	uint8_t oport_num;
	uint8_t l2_non_ucast_tocpu;
	uint8_t l2_miss_tocpu;
	uint8_t pri;
	uint8_t table_index;
	uint8_t iport_num;
	uint8_t default_port;
	uint8_t action;
	uint8_t valid;
	uint8_t is_en;
	uint8_t port;
	uint8_t vport;
	uint8_t is_add;
	uint8_t new_ipri;
	uint8_t new_opri;
} __packed;

enum vlan_genl_method {
        VLAN_TABLE_DUMP,
	SET_IPORT,
	SET_IVLAN_PBV,
	SET_IVLAN_LKP,
	SET_IVLAN_XLT,
	SET_IVLAN_SPL,
	SET_EVLAN_LKP,
	SET_EVLAN_ACT,
	SET_EVLAN_XLT,
	SET_EVLAN_PTPID,
	SET_EVLAN_OTPID,
	EN_VLAN_VPORT,
	SET_VLAN_VPORT,
};

enum table_name_index {
	IPORT,
	IVLAN_PBV,
	IVLAN_LKP,
	IVALN_XLT,
	IVLAN_SPL,
	EVLAN_LKP,
	EVLAN_ACT,
	EVLAN_XLT,
	EVLAN_PTPID,
	EVLAN_OTPID,
	VLAN_VPORT_MAP,
	MODIFY_VPORT_MAP,
	TMU_IVPORT_MAP,
	NUM_TABLE_NAME_INDEX,
};
struct vlan_genl_resp{
	int32_t err;
	char msg[];
} __packed;

#endif
