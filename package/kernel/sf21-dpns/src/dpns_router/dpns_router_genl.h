#ifndef __DPNS_ROUTER_GENL_H_
#define __DPNS_ROUTER_GENL_H_

struct router_genl_msg {
        uint32_t method;
        uint32_t ipaddr;
        uint32_t next_hop_ptr;
        uint32_t intf_index;
        uint32_t ovport;
        uint16_t ovid;
        uint8_t ipaddr6[16];
        uint8_t req_id;
        uint8_t req_addr;
        uint8_t prefix_len;
} __packed;

enum router_genl_method {
        ROUTER_DUMP,
        ROUTER_TABLE_ADD,
        ROUTER_TABLE_ADD_V6,
        ROUTER_TABLE_DEL,
        ROUTER_TABLE_DEL_V6,
};

struct 	router_genl_resp{
	int32_t err;
	char msg[];
} __packed;

#endif
