#ifndef IVLAN_SE_H
#define IVLAN_SE_H

#include "se_common.h"

/**
 * iport table max entry 10
 * not include hit and limit drop, Total bit 23
 */
struct ivlan_iport_table {
	u8 pass_mode           : 1;
	u8 vid_zero_handle     : 1;
	u8 vlan_security_mode  : 1;
	u8 vfp_based_vid_enable: 1;
	u8 vt_en               : 1;
	u8 vt_miss_drop        : 1;
	u8 use_ivid_as_ovid    : 1;
	u8 ifiliter_en         : 2;
	u8 disable_vlan_check  : 1;
	u8 default_port        : 5;
	u8 port_cml            : 2;
	u8 action              : 2;
	u8 valid               : 1;
	u8 rsv0                : 4;
	u8 rsv1                   ;
} __packed; //<=32

union ivlan_iport_table_cfg {
	struct ivlan_iport_table table;
	u32 data[1];
};

struct ivlan_iport_limit_table {
	u8 limit_drop : 1;
} __packed;

/**
 * port based vlan table max entry 8
 */
struct ivlan_pbv_table {
	u16 un_pitag_action : 2;
	u16 un_potag_action : 2;
	u16 un_itag_action  : 2;
	u16 un_otag_action  : 2;
	u16 sit_pitag_action: 2;
	u16 sit_potag_action: 2;
	u16 sit_itag_action : 2;
	u16 sit_otag_action : 2;
	u16 sot_pitag_action: 2;
	u16 sot_potag_action: 2;
	u16 sot_itag_action : 2;
	u16 sot_otag_action : 2;
	u16 dt_pitag_action : 2;
	u16 dt_potag_action : 2;
	u16 dt_itag_action  : 2;
	u16 dt_otag_action  : 2;
	u16 ipri            : 3;
	u16 opri            : 3;
	u16 ivid            : 12;
	u16 ovid            : 12;
	u16 transparent_en  : 1;
	u16 valid           : 1;
} __packed; //<=64

union ivlan_pbv_table_cfg {
	struct ivlan_pbv_table table;
	u32 data[2];
};

/**
 * vfp table max entry 64
 */
struct ivlan_vfp_table {
	u64 un_pitag_action : 2;
	u64 un_itag_action  : 2;
	u64 un_potag_action : 2;
	u64 un_otag_action  : 2;
	u64 sit_pitag_action: 2;
	u64 sit_itag_action : 2;
	u64 sit_potag_action: 2;
	u64 sit_otag_action : 2;
	u64 sot_pitag_action: 2;
	u64 sot_itag_action : 2;
	u64 sot_potag_action: 2;
	u64 sot_otag_action : 2;
	u64 dt_pitag_action : 2;
	u64 dt_itag_action  : 2;
	u64 dt_potag_action : 2;
	u64 dt_otag_action  : 2;
	u64 ipri            : 3;
	u64 opri            : 3;
	u64 ivid            : 12;
	u64 ovid            : 12;
	u64 transparent_en  : 1;
	u64 valid           : 1;
	u64 dport           : 16;
	u64 sport           : 16;
	u64 protocal        : 8;
	u64 dip             : 32;
	u64 sip             : 32;
	u64 dmac            : 48;
	u64 smac            : 48;
	u64 i_vport         : 6;
	u64 resv0           : 18;
} __packed; //<=288

union ivlan_vfp_table_cfg {
	struct ivlan_vfp_table table;
	u32 data[9];
};

/**
 * xlate table max entry 1k
 */
struct ivlan_xlt_table {
	u16 valid           : 1;
	u16 un_pitag_action : 2;
	u16 un_potag_action : 2;
	u16 un_itag_action  : 2;
	u16 un_otag_action  : 2;
	u16 sit_pitag_action: 2;
	u16 sit_potag_action: 2;
	u16 sit_itag_action : 2;
	u16 sit_otag_action : 2;
	u16 sot_pitag_action: 2;
	u16 sot_potag_action: 2;
	u16 sot_itag_action : 2;
	u16 sot_otag_action : 2;
	u16 dt_pitag_action : 2;
	u16 dt_potag_action : 2;
	u16 dt_itag_action  : 2;
	u16 dt_otag_action  : 2;
	u16 new_ipri        : 3;
	u16 new_opri        : 3;
	u16 new_ivid        : 12;
	u16 new_ovid        : 12;
	u16 transparent_en  : 1;
	u16 inner_vid       : 12;
	u16 outer_vid       : 12;
	u16 iport_num       : 5;
	u16 resv0           : 3;
	//u16 resv1              ;
} __packed; //<=96

union ivlan_xlt_table_cfg {
	struct ivlan_xlt_table table;
	u32 data[3];
};

struct ivlan_spl_table {
	u32 credit   : 25;
	u32 cnt      : 31;
	u32 rsv0     : 8;
} __packed; //<=64

union ivlan_spl_table_cfg {
	struct ivlan_spl_table table;
	u32 data[2];
};

struct ivlan_lkp_table {
	u32 valid  	           : 1;
	u32 learn_disable      : 1;
	u32 l2_pfm             : 2;
	u32 ipmcv6_en          : 1;
	u32 ipmcv4_en          : 1;
	u32 l2_miss_tocpu      : 1;
	u32 l2_miss_drop       : 1;
	u32 l2_non_ucast_tocpu : 1;
	u32 l2_non_ucast_drop  : 1;
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
	u32 port_bitmap	       : 27;
	u32 ovid               : 12;
	u32 rsv0               : 27;
} __packed;	// <=96

union ivlan_lkp_table_cfg {
	struct ivlan_lkp_table table;
	u32 data[3];
};

void iport_table_update(VLAN_t *priv, u8 iport_num, u8 vlan_security_mode,
		u8 default_port, u8 valid);
void iport_table_dump(VLAN_t *priv, u8 iport_num);

void ivlan_pbv_table_update(VLAN_t *priv, u8 iport_num, int ivid, int ovid, u8 valid, int dt_otag, int dt_potag, int sot_otag, int sot_potag, int sit_otag, int sit_potag, int un_otag, int un_potag, int def_action, u8 pri);
void ivlan_pbv_table_dump(VLAN_t *priv, u8 iport_num);

void ivlan_lkp_table_update(VLAN_t *priv, u8 iport_num, int vid,
		u8 l2_miss_tocpu, u8 l2_non_ucast_tocpu, u8 valid, u32 port_bitmap);
void ivlan_lkp_table_dump(VLAN_t *priv, u8 iport_num);

void ivlan_xlt_table_update(VLAN_t *priv, u8 iport_num, int vid, u8 valid);
void ivlan_xlt_table_dump(VLAN_t *priv, u8 iport_num);

void ivlan_spl_table_update(VLAN_t *priv, u8 iport_num, u32 credit);
void ivlan_spl_table_dump(VLAN_t *priv, u8 iport_num);


#endif
