#ifndef _OPS_H_
#define _OPS_H_

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include "dpns_common.h"

#include <linux/netdevice.h>
#include <linux/notifier.h>
#include <net/neighbour.h>
#include <net/switchdev.h>
#include <linux/random.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/ethtool.h>
#include <linux/if_vlan.h>
#include <linux/if_bridge.h>
#include <linux/bitops.h>
#include <linux/of.h>
#include <linux/ctype.h>
#include <linux/workqueue.h>
#include <net/switchdev.h>
#include <net/rtnetlink.h>
#include <net/netevent.h>

#define INTF_TABLE_MAX		64

enum error_pkt_act {
	ERR_PKT_UP,
	ERR_PKT_FWD,
	ERR_PKT_DROP,
};

struct arp_intf_table {
	u64 ovid           : 12;
	u64 smac           : 48;
	u64 tunnel_en	   : 1;
	u64 pppoe_en       : 1;
	u64 wan_flag	   : 1;
	u64 valid          : 1;
} __packed; // <=64

union arp_intf_table_cfg {
	struct arp_intf_table table;
	u32 data[4];
};

typedef struct _intf_entry {
	struct arp_intf_table data;
	u8  valid;
	u32 ref_count;
} intf_entry;

typedef struct _tcam_block {
	u8 key_mode;
	u8 item_index;
	u8 slice_index;
} tcam_block;

int dpns_table_read(COMMON_t *priv,
	 u8 ram_id, u16 table_addr, u32* data, u32 size);
int dpns_table_write(COMMON_t *priv,
	 u8 ram_id, u16 table_addr, u32* data, u32 size);
int dpns_intf_table_add(COMMON_t *priv, int vid, bool pppoe_en, bool tunnel_en,
	 bool wan_flag, u8 *smac);
void dpns_intf_table_del(COMMON_t *priv, u32 index);
void dpns_tcam_update(COMMON_t *priv, u8 block_id, u8 req_id, u8 req_addr,
		void *data, void *mask, u32 size, u8 tbid_and_kmd);
void dump_intf_table(COMMON_t *priv);
int dpns_tcam_access(COMMON_t *priv, int opcode, u8 req_id,
		u8 req_addr, void *data, u32 size);
void dpns_tcam_clean(COMMON_t *priv, u8 block_id);
void dpns_read_npu_mib(COMMON_t *priv);
int dpns_mem_alloc_init(u8 module);
void dpns_mem_alloc_deinit(u8 module);
void dump_dpns_mem_info(void);
void se_reg_set_wait(COMMON_t *priv, u32 reg, u32 val, u32 waitfor, u32 timeout);
void se_wait_busy(COMMON_t *priv, u32 reg, u32 mask);

bool dpns_port_dev_check(COMMON_t *priv, struct net_device *dev);
int dpns_common_netdevice_event(struct notifier_block *unused,
				unsigned long event, void *ptr);
dpns_port_t *dpns_port_by_netdev(COMMON_t *priv, const struct net_device* dev);
int dpns_port_id_by_netdev(COMMON_t *priv, const struct net_device *dev, u8 *port_id);

int dpns_common_genl_init(struct dpns_common_priv *priv);
int dpns_common_genl_exit(void);

void dpns_destroy_portsarray(COMMON_t *priv);
#endif // _OPS_H_
