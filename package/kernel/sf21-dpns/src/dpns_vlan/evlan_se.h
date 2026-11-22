#ifndef _EVLAN_SE_H
#define _EVLAN_SE_H

#include "se_common.h"


enum evlan_act_action {
	EVACT_NONE,
	EVACT_REPLACE1, /* replace with arp/intf vid */
	EVACT_REPLACE2, /* replace with xlt ovid */
	EVACT_DEL
};

struct evlan_lkp_table {
	u32 valid              : 1;
	u32 outer_tpid_index   : 3;
	u32 sp_tree_port9      : 2;
	u32 sp_tree_port8      : 2;
	u32 sp_tree_port7      : 2;
	u32 sp_tree_port6      : 2;
	u32 sp_tree_port5      : 2;
	u32 sp_tree_port4      : 2;
	u32 sp_tree_port3      : 2;
	u32 sp_tree_port2      : 2;
	u32 sp_tree_port1      : 2;
	u32 sp_tree_port0      : 2;
	u32 un_bitmap          : 27;
	u32 port_bitmap        : 27;
	u32 ovid               : 12;
	u32 rsv0               : 6;
} __packed; //<=96

union evlan_lkp_table_cfg {
	struct evlan_lkp_table table;
	u32 data[3];
};

struct evlan_tagmeb_table {
	u32 valid              : 1;
	u32 outer_tpid_index   : 3;
	u32 sp_tree_port9      : 2;
	u32 sp_tree_port8      : 2;
	u32 sp_tree_port7      : 2;
	u32 sp_tree_port6      : 2;
	u32 sp_tree_port5      : 2;
	u32 sp_tree_port4      : 2;
	u32 sp_tree_port3      : 2;
	u32 sp_tree_port2      : 2;
	u32 sp_tree_port1      : 2;
	u32 sp_tree_port0      : 2;
	u32 un_bitmap          : 27;
	u32 port_bitmap        : 27;
	u32 meb_vid            : 12;
	u32 rsv0               : 6;
} __packed; //<=96

union evlan_tagmeb_table_cfg {
	struct evlan_tagmeb_table table;
	u32 data[3];
};

struct evlan_otpid_table {
	u16 outer_tpid;
	u16 rsvd;
} __packed; //<=32

union evlan_otpid_table_cfg {
	struct evlan_otpid_table table;
	u32 data[1];
};

struct evlan_port_tpid_table {
    u16 inner_tpid;
    u16 outer_tpid;
} __packed; //<=32

union evlan_port_tpid_table_cfg {
	struct evlan_port_tpid_table table;
	u32 data[1];
};

struct evlan_xlt_table {
	u16 valid                    : 1;
	u16 old_ivid_mask            : 1;
	u16 old_ovid_mask            : 1;
	u16 oport_num_mask           : 1;
	u16 action_index             : 6;
	u16 new_ipri                 : 3;
	u16 new_opri                 : 3;
	u16 new_ivid                 : 12;
	u16 new_ovid                 : 12;
	u16 old_ivid                 : 12;
	u16 old_ovid                 : 12;
	u16 oport_num                : 5;
	u16 rsv0                     : 13;
	u16 rsv1                        ;
} __packed; //<=96

union evlan_xlt_table_cfg {
	struct evlan_xlt_table table;
	u32 data[3];
};

struct evlan_act_table {
	u8 sot_pitag_action: 2;
	u8 sot_itag_action : 2;
	u8 sot_potag_action: 2;
	u8 sot_otag_action : 2;
	u8 dt_pitag_action : 2;
	u8 dt_itag_action  : 2;
	u8 dt_potag_action : 2;
	u8 dt_otag_action  : 2;
	u8 tt_pitag_action : 2;
	u8 tt_itag_action  : 2;
	u8 tt_potag_action : 2;
	u8 tt_otag_action  : 2;
	u8 tt_patag_action : 2;
	u8 tt_atag_action  : 2;
	u8 rsv0            : 4;
} __packed; //<=32

union evlan_act_table_cfg {
	struct evlan_act_table table;
	u32 data[1];
};

void evlan_lkp_table_update(VLAN_t *priv, u8 iport_num, int vid, int valid, u32 un_bitmap, u32 port_bitmap);
void evlan_lkp_table_dump(VLAN_t *priv, u8 iport_num);
void evlan_act_table_update(VLAN_t *priv, u8 iport_num, u16 sot_action, u16 psot_action, u16 dt_action, u16 pdt_action, u16 def_action);
void evlan_act_table_dump(VLAN_t *priv, u8 iport_num);
void evlan_xlt_table_update(VLAN_t *priv, u8 iport_num, u8 oport_num, int valid, int old_ivid, int old_ovid, int new_ivid, int new_ovid, int old_ivid_mask, int old_ovid_mask, int evlan_act_idx, int new_ipri, int new_opri);
void evlan_xlt_table_dump(VLAN_t *priv, u8 iport_num);
void evlan_ptpid_table_update(VLAN_t *priv, u8 iport_num, u32 itpid, u32 otpid);
void evlan_ptpid_table_dump(VLAN_t *priv, u8 iport_num);

void evlan_otpid_table_update(VLAN_t *priv, u8 iport_num, u32 tpid);
void evlan_otpid_table_dump(VLAN_t *priv, u8 iport_num);
#endif //_EVLAN_SE_H
