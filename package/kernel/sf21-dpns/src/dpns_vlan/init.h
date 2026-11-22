#ifndef _DPNS_VLAN_H
#define _DPNS_VLAN_H

#include <linux/regmap.h>
#include <linux/debugfs.h>
#include <linux/bitfield.h>
#include "dpns_common.h"

#define IVLAN_LKP_CLEAR		0x0010		//ivlan_lkp clear control
#define EVLAN_LKP_CLEAR		0x2000		//evlan_lkp clear control

#define REAL_PORT_NUM		5
#define MAX_PORT_NUM		27


struct sf_vlan_tbl_entry {
	struct list_head node;
	u16 vlan_id;
	u8 ivlan_lkp_index;
	u8 evlan_lkp_index;
	u32 member_ports;
	u32 untagged_ports;
	u32 vlan_ports;
};

enum Port_Vlan_Options {
	PORT_WITH_NO_VLAN = 0,
	PORT_WITH_ONE_VLAN = 1,
};

/* sf_vlan_id_entry is set to store only VLAN ID information
 * used for the deletion of MAC table entries
 * */
struct sf_vlan_id_entry {
	struct list_head node;
	u16 vlan_id;
};

struct dpns_vlan_netdev_event_work {
	struct work_struct work;
	struct netdev_notifier_changeupper_info info;
	struct net_device *dev;
	VLAN_t	*priv;
	dpns_port_t *dp_port;
	unsigned long event;
};

void default_vlan_init(VLAN_t * priv);

int dpns_vlan_genl_init(struct dpns_vlan_priv *priv);
int dpns_vlan_genl_exit(void);


int dpns_vlan_proc_init(struct dpns_vlan_priv *priv);
int dpns_vlan_proc_exit(void);
#endif //_DPNS_VLAN_H
