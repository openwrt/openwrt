#ifndef _L2_H_
#define _L2_H_

#include <linux/if_ether.h>
#include <linux/list.h>
#include <linux/regmap.h>
#include <linux/etherdevice.h>
#include <net/genetlink.h>

#include "dpns_common.h"
#include "se_common.h"
#include "sf_genl_msg.h"

#define L2_MAC_NUM_MAX		2048
#define	L2_MIB_NUM_MAX		1024
#define	L2_SPL_NUM_MAX		512
#define	L2_ISO_NUM_MAX		64
#define	L2_HASH_TABLE_MAX	10
#define	L2_AGE_REG_DEPTH	64
#define	L2_AGE_REG_WIDTH	32

#define L2_DEFAULT_AGEING_TIME	(150 * HZ)
#define	L2_MIB_DEFAULT_TIME	(1*HZ)
/**
 * @brief se register access define...
 *
 */
#define se_write32(dpns, reg, val)	\
	writel((val), (dpns)->iobase + (reg))

#define se_read32(dpns, reg)	\
	readl((dpns)->iobase + (reg))

#define se_write64(dpns, reg, val)	\
	writeq((val), (dpns)->iobase + (reg))

#define se_read64(dpns, reg)	\
	readq((dpns)->iobase + (reg))

#define se_clean_set(dpns, reg, clear, set) \
	do { \
		void __iomem *addr = (dpns)->iobase + (reg);	\
		u32 val = readl(addr);				\
								\
		val &= ~(clear);				\
		val |= (set);					\
		writel((val), (addr));				\
	} while (0)


typedef struct l2_hash_key {
	uint8_t	mac[ETH_ALEN];
} __packed l2_hash_key_t;


struct mac_spl_table {
	uint32_t	credit		:24;
	uint32_t	count 		:30;
	uint32_t 	rsv0		:10;
} __packed;

union mac_spl_table_cfg {
    struct mac_spl_table table;
    u32 data[2];
};

struct l2_iso_table {
	u32 port_isolation_bitmap: 27;
	u32 isolation_offload_bitmap: 27;
    u32 rsv0            : 10;
} __packed; //<=64

union l2_iso_table_cfg {
    struct l2_iso_table table;
    u32 data[2];
};

/**
 * @brief Bind vport to linux bridge, sync hw offload flow in vports;
 * use vport-id as default group-id, both ivlan and evlan surpported.
 * @param dp
 * @param mac 		target mac
 * @param valid
 * @param vlan_id 	output vlan-id
 * @param port_map	output vport-id in BIT()
 * @param l3_en		layer 3 enable
 * @param sa_cml	drop, to host, forward, fwd & to host;
 * @param da_cml
 * @return		next mac table index to write;
 */
int se_l2_mac_table_update(MAC_t* priv,
		 const u8 *dsmac,
		 u8 valid,
		 u16 vlan_id,
		 u64 port_map,
		 u8 age_en,
		 u8 l3_en,
		 u8 sa_cml,
		 u8 da_cml,
		 u8 vlan_en,
		 u16 sta_id,
		 u16 repeater_id);

struct sf_ts_info {
	u8 mac[ETH_ALEN];
	int nat_id;
	u64 tx_bytes;
	u64 rx_bytes;
	u64 total_bytes;
	u64 tx_pkts;
	u64 rx_pkts;
	u64 total_pkts;
	u32 rx_rate;
	u32 tx_rate;
	u32 total_rate;
	u16 mib_index;
	u16 vid;
	u8 mode;
};

struct sf_traffic_statics_info{
	struct hlist_node snode;
	struct sf_ts_info ts_info;
};

struct sf_vlan_tbl_entry {
	struct list_head node;
	u16 vlan_id;
	u8 ivlan_lkp_index;
	u8 evlan_lkp_index;
	u32 member_ports;
	u32 untagged_ports;
	u32 vlan_ports;
};

struct mac_table_entry{
	struct list_head node;
	u8 mac[ETH_ALEN];
	u32 index;
};

/* sf_vlan_id_entry is set to store only VLAN ID information
 * used for the deletion of MAC table entries
 * */
struct sf_vlan_id_entry {
	struct list_head node;
	u16 vlan_id;
};

void sf_mac_clear(MAC_t *priv);

enum mib_op {
	MIB_ON,
	MIB_OFF,
	MIB_CLEAR,
};

struct l2_ubus_work {
	struct work_struct work;
	struct l2_mac_genl_msg_add *msg;
	MAC_t *priv;
};
#endif
