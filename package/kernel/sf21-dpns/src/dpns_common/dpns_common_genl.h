#ifndef __DPNS_COMMON_GENL_H_
#define __DPNS_COMMON_GENL_H_

typedef struct npu_status_info{
    uint32_t addr;
    char reg_name[32];
    uint32_t mask;
    char desc[64];
} NPU_STATUS_t;

struct common_genl_msg {
	uint64_t smac;
	uint32_t method;
	uint32_t vid;
	uint32_t index;
	uint32_t port_bitmap;
	uint32_t offload_bitmap;
	uint8_t iport_num;
	uint8_t module_num;
	uint8_t log_level;
	bool pppoe_en;
	bool tunnel_en;
	bool wan_flag;
} __packed;

enum common_genl_method {
        DEBUG_DUMP,
	INTF_ADD,
	INTF_DEL,
	ISO_SET,
	LOG_SET,
	INTF_DUMP,
	ISO_DUMP,
	LOG_DUMP,
	MIB_DUMP,
	DEV_DUMP,
	MEM_DUMP,
};

struct common_genl_resp{
	int32_t err;
	char msg[];
} __packed;
#endif
