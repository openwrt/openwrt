/*
* Description
*
* Copyright (C) 2016-2022 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/

#include "ivlan_se.h"


void iport_table_update(VLAN_t *priv, u8 iport_num, u8 default_port,
		u8 action, u8 valid)
{
	union ivlan_iport_table_cfg param = {0};

	param.table.valid = valid;
	param.table.default_port = default_port;
	param.table.action = action;
	/* enable ivlan xlt table and l2 learning */
	param.table.vt_en = SEARCH_FIXED;
	param.table.port_cml = CML_LEARNING_DROP;

	priv->cpriv->table_write(priv->cpriv, IVLAN_IPORT_TABLE, iport_num, param.data,
			sizeof(param));
}

void iport_table_dump(VLAN_t *priv, u8 iport_num)
{
	union ivlan_iport_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, IVLAN_IPORT_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("iport table:\n");
	printk("\t valid                %u\n",	param.table.valid);
	printk("\t pass_mode            %u\n",	param.table.pass_mode);
	printk("\t vid_zero_handle      %u\n",	param.table.vid_zero_handle);
	printk("\t vlan_security_mode   %u\n",	param.table.vlan_security_mode);
	printk("\t vfp_based_vid_enable %u\n",	param.table.vfp_based_vid_enable);
	printk("\t vt_en                %u\n",	param.table.vt_en);
	printk("\t vt_miss_drop         %u\n",	param.table.vt_miss_drop);
	printk("\t use_ivid_as_ovid     %u\n",	param.table.use_ivid_as_ovid);
	printk("\t ifiliter_en          %u\n",	param.table.ifiliter_en);
	printk("\t disable_vlan_check   %u\n",	param.table.disable_vlan_check);
	printk("\t default_port         %u\n",	param.table.default_port);
	printk("\t port_cml             %u\n",	param.table.port_cml);
	printk("\t action               %u\n",	param.table.action);
}

void ivlan_pbv_table_update(VLAN_t *priv, u8 iport_num, int ivid, int ovid, u8 valid, int dt_otag, int dt_potag, int sot_otag, int sot_potag, int sit_otag, int sit_potag, int un_otag, int un_potag, int def_action, u8 pri)
{
	union ivlan_pbv_table_cfg param = {0};

	param.table.valid = valid;
	param.table.ivid = ivid;
	param.table.ovid = ovid;
	param.table.opri = pri;
	param.table.ipri = pri;
	param.table.dt_itag_action = 0;
	param.table.dt_pitag_action = 0;
	param.table.dt_otag_action = dt_otag;
	param.table.dt_potag_action = dt_potag;
	param.table.sot_otag_action = sot_otag;
	param.table.sot_potag_action = sot_potag;
	param.table.sot_itag_action = def_action;
	param.table.sot_pitag_action = def_action;
	param.table.sit_otag_action = sit_otag;
	param.table.sit_potag_action = sit_potag;
	param.table.sit_itag_action = def_action;
	param.table.sit_pitag_action = def_action;
	param.table.un_otag_action = un_otag;
	param.table.un_potag_action = un_potag;
	param.table.un_itag_action = def_action;
	param.table.un_pitag_action = def_action;
	priv->cpriv->table_write(priv->cpriv, IVLAN_PBV_DIRECT_TABLE, iport_num, param.data,
			sizeof(param));
}

void ivlan_pbv_table_dump(VLAN_t *priv, u8 iport_num)
{
	union ivlan_pbv_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, IVLAN_PBV_DIRECT_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("ivlan pbv table:\n");
	printk("\t valid                %u\n",	param.table.valid);
	printk("\t transparent_en       %u\n",	param.table.transparent_en);
	printk("\t ovid                 %u\n",	param.table.ovid);
	printk("\t ivid                 %u\n",	param.table.ivid);
	printk("\t opri                 %u\n",	param.table.opri);
	printk("\t ipri                 %u\n",	param.table.ipri);
	printk("\t dt_otag_action       %u\n",	param.table.dt_otag_action);
	printk("\t dt_potag_action      %u\n",	param.table.dt_potag_action);
	printk("\t dt_itag_action       %u\n",	param.table.dt_itag_action);
	printk("\t dt_pitag_action      %u\n",	param.table.dt_pitag_action);
	printk("\t sot_otag_action      %u\n",	param.table.sot_otag_action);
	printk("\t sot_potag_action     %u\n",	param.table.sot_potag_action);
	printk("\t sot_itag_action      %u\n",	param.table.sot_itag_action);
	printk("\t sot_pitag_action     %u\n",	param.table.sot_pitag_action);
	printk("\t sit_otag_action      %u\n",	param.table.sit_otag_action);
	printk("\t sit_potag_action     %u\n",	param.table.sit_potag_action);
	printk("\t sit_itag_action      %u\n",	param.table.sit_itag_action);
	printk("\t sit_pitag_action     %u\n",	param.table.sit_pitag_action);
	printk("\t un_otag_action       %u\n",	param.table.un_otag_action);
	printk("\t un_potag_action      %u\n",	param.table.un_potag_action);
	printk("\t un_itag_action       %u\n",	param.table.un_itag_action);
	printk("\t un_pitag_action      %u\n",	param.table.un_pitag_action);
}

void ivlan_lkp_table_update(VLAN_t *priv, u8 iport_num, int vid,
		u8 l2_miss_tocpu, u8 l2_non_ucast_tocpu, u8 valid, u32 port_bitmap)
{
	union ivlan_lkp_table_cfg param = {0};

	param.table.valid = valid;
	param.table.l2_pfm = UNKNOWN_MCAST_DROP;
	param.table.ipmcv4_en = 1;
	param.table.ipmcv6_en = 1;
	param.table.l2_miss_tocpu = l2_miss_tocpu;
	param.table.l2_non_ucast_tocpu = l2_non_ucast_tocpu;
	param.table.sp_tree_port9 = SP_FORWARD;
	param.table.sp_tree_port8 = SP_FORWARD;
	param.table.sp_tree_port7 = SP_FORWARD;
	param.table.sp_tree_port6 = SP_FORWARD;
	param.table.sp_tree_port5 = SP_FORWARD;
	param.table.sp_tree_port4 = SP_FORWARD;
	param.table.sp_tree_port3 = SP_FORWARD;
	param.table.sp_tree_port2 = SP_FORWARD;
	param.table.sp_tree_port1 = SP_FORWARD;
	param.table.sp_tree_port0 = SP_FORWARD;
	param.table.port_bitmap = port_bitmap;
	param.table.ovid = vid;

	priv->cpriv->table_write(priv->cpriv, IVLAN_LKP_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
}

void ivlan_lkp_table_dump(VLAN_t *priv, u8 iport_num)
{
	union ivlan_lkp_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, IVLAN_LKP_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("ivlan lkp table:\n");
	printk("\t valid                %u\n",		param.table.valid);
	printk("\t learn_disable        %u\n",		param.table.learn_disable);
	printk("\t l2_pfm               %u\n",		param.table.l2_pfm);
	printk("\t ipmcv6_en            %u\n",		param.table.ipmcv6_en);
	printk("\t ipmcv4_en            %u\n",		param.table.ipmcv4_en);
	printk("\t l2_miss_tocpu        %u\n",		param.table.l2_miss_tocpu);
	printk("\t l2_miss_drop         %u\n",		param.table.l2_miss_drop);
	printk("\t l2_non_ucast_tocpu   %u\n",		param.table.l2_non_ucast_tocpu);
	printk("\t l2_non_ucast_drop    %u\n",		param.table.l2_non_ucast_drop);
	printk("\t sp_tree_port9        %u\n",		param.table.sp_tree_port9);
	printk("\t sp_tree_port8        %u\n",		param.table.sp_tree_port8);
	printk("\t sp_tree_port7        %u\n",		param.table.sp_tree_port7);
	printk("\t sp_tree_port6        %u\n",		param.table.sp_tree_port6);
	printk("\t sp_tree_port5        %u\n",		param.table.sp_tree_port5);
	printk("\t sp_tree_port4        %u\n",		param.table.sp_tree_port4);
	printk("\t sp_tree_port3        %u\n",		param.table.sp_tree_port3);
	printk("\t sp_tree_port2        %u\n",		param.table.sp_tree_port2);
	printk("\t sp_tree_port1        %u\n",		param.table.sp_tree_port1);
	printk("\t sp_tree_port0        %u\n",		param.table.sp_tree_port0);
	printk("\t port_bitmap          0x%llx\n",	(u64)param.table.port_bitmap);
	printk("\t ovid                 %u\n",		param.table.ovid);
}

void ivlan_xlt_table_update(VLAN_t *priv, u8 iport_num, int vid, u8 valid)
{
	union ivlan_xlt_table_cfg param = {0};

	param.table.valid = valid;
	param.table.iport_num        = 4;
	param.table.inner_vid        = 1;
	param.table.outer_vid        = 1;
	param.table.new_ovid         = vid;
	param.table.dt_otag_action   = ACTION_DEL;
	param.table.dt_potag_action  = ACTION_DEL;
	param.table.dt_itag_action   = ACTION_DEL;
	param.table.dt_pitag_action  = ACTION_DEL;
	param.table.sot_otag_action  = ACTION_DEL;
	param.table.sot_potag_action = ACTION_DEL;
	param.table.sot_itag_action  = ACTION_DEL;
	param.table.sot_pitag_action = ACTION_DEL;
	param.table.sit_otag_action  = ACTION_DEL;
	param.table.sit_potag_action = ACTION_DEL;
	param.table.sit_itag_action  = ACTION_DEL;
	param.table.sit_pitag_action = ACTION_DEL;
	param.table.un_otag_action   = ACTION_DEL;
	param.table.un_potag_action  = ACTION_DEL;
	param.table.un_itag_action   = ACTION_DEL;
	param.table.un_pitag_action  = ACTION_DEL;

	priv->cpriv->table_write(priv->cpriv, IVLAN_XLT_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
}

void ivlan_xlt_table_dump(VLAN_t *priv, u8 iport_num)
{
	union ivlan_xlt_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, IVLAN_XLT_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("ivlan xlt table:\n");
	printk("\t valid                %u\n",	param.table.valid);
	printk("\t iport_num            %u\n",	param.table.iport_num);
	printk("\t outer_vid            %u\n",	param.table.outer_vid);
	printk("\t inner_vid            %u\n",	param.table.inner_vid);
	printk("\t transparent_en       %u\n",	param.table.transparent_en);
	printk("\t new_ovid             %u\n",	param.table.new_ovid);
	printk("\t new_ivid             %u\n",	param.table.new_ivid);
	printk("\t new_opri             %u\n",	param.table.new_opri);
	printk("\t new_ipri             %u\n",	param.table.new_ipri);
	printk("\t dt_otag_action       %u\n",	param.table.dt_otag_action);
	printk("\t dt_potag_action      %u\n",	param.table.dt_potag_action);
	printk("\t dt_itag_action       %u\n",	param.table.dt_itag_action);
	printk("\t dt_pitag_action      %u\n",	param.table.dt_pitag_action);
	printk("\t sot_otag_action      %u\n",	param.table.sot_otag_action);
	printk("\t sot_potag_action     %u\n",	param.table.sot_potag_action);
	printk("\t sot_itag_action      %u\n",	param.table.sot_itag_action);
	printk("\t sot_pitag_action     %u\n",	param.table.sot_pitag_action);
	printk("\t sit_otag_action      %u\n",	param.table.sit_otag_action);
	printk("\t sit_potag_action     %u\n",	param.table.sit_potag_action);
	printk("\t sit_itag_action      %u\n",	param.table.sit_itag_action);
	printk("\t sit_pitag_action     %u\n",	param.table.sit_pitag_action);
	printk("\t un_otag_action       %u\n",	param.table.un_otag_action);
	printk("\t un_potag_action      %u\n",	param.table.un_potag_action);
	printk("\t un_itag_action       %u\n",	param.table.un_itag_action);
	printk("\t un_pitag_action      %u\n",	param.table.un_pitag_action);
}

void ivlan_spl_table_update(VLAN_t *priv, u8 iport_num, u32 credit)
{
	union ivlan_spl_table_cfg param = {0};

	param.table.credit = credit;

	priv->cpriv->table_write(priv->cpriv, IVLAN_SPL_TABLE, iport_num, param.data,
			sizeof(param));
}

void ivlan_spl_table_dump(VLAN_t *priv, u8 iport_num)
{
	union ivlan_spl_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, IVLAN_SPL_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("ivlan spl table:\n");
	printk("\t credit                0x%x\n",	param.table.credit);
	printk("\t cnt                   %u\n",		param.table.cnt);
}
