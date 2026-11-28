#include "evlan_se.h"

void evlan_lkp_table_update(VLAN_t *priv, u8 iport_num, int vid, int valid, u32 un_bitmap, u32 port_bitmap)
{
	union evlan_lkp_table_cfg param = {0};

	param.table.valid = valid;
	param.table.sp_tree_port0     = 0x3;
	param.table.sp_tree_port1     = 0x3;
	param.table.sp_tree_port2     = 0x3;
	param.table.sp_tree_port3     = 0x3;
	param.table.sp_tree_port4     = 0x3;
	param.table.sp_tree_port5     = 0x3;
	param.table.sp_tree_port6     = 0x3;
	param.table.sp_tree_port7     = 0x3;
	param.table.sp_tree_port8     = 0x3;
	param.table.sp_tree_port9     = 0x3;
	param.table.un_bitmap         = un_bitmap;
	param.table.port_bitmap       = port_bitmap;
	param.table.ovid = vid;

	priv->cpriv->table_write(priv->cpriv, EVLAN_VID_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
}

void evlan_lkp_table_dump(VLAN_t *priv, u8 iport_num)
{
	union evlan_lkp_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, EVLAN_VID_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("evlan lkp table:\n");
	printk("\t valid                %u\n",		param.table.valid);
	printk("\t outer_tpid_index     %u\n",		param.table.outer_tpid_index);
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
	printk("\t un_bitmap            0x%llx\n",	(u64)param.table.un_bitmap);
	printk("\t port_bitmap          0x%llx\n",	(u64)param.table.port_bitmap);
	printk("\t ovid                 %u\n",		param.table.ovid);
}

void evlan_act_table_update(VLAN_t *priv, u8 iport_num, u16 sot_action, u16 psot_action, u16 dt_action, u16 pdt_action, u16 def_action)
{
	union evlan_act_table_cfg param = {0};

	param.table.sot_pitag_action    = def_action;
	param.table.sot_itag_action     = def_action;
	param.table.sot_potag_action    = psot_action;
	param.table.sot_otag_action     = sot_action;
	param.table.dt_pitag_action     = def_action;
	param.table.dt_itag_action      = def_action;
	param.table.dt_potag_action     = pdt_action;
	param.table.dt_otag_action      = dt_action;
	param.table.tt_pitag_action     = def_action;
	param.table.tt_itag_action      = def_action;
	param.table.tt_potag_action     = def_action;
	param.table.tt_otag_action      = def_action;
	param.table.tt_patag_action     = def_action;
	param.table.tt_atag_action      = def_action;

	priv->cpriv->table_write(priv->cpriv, EVLAN_ACT_DIRECT_TABLE, iport_num, param.data,
			sizeof(param));
}

void evlan_act_table_dump(VLAN_t *priv, u8 iport_num)
{
	union evlan_act_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, EVLAN_ACT_DIRECT_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("evlan act table:\n");
	printk("\t sot_pitag_action     %u\n",		param.table.sot_pitag_action);
	printk("\t sot_itag_action      %u\n",		param.table.sot_itag_action);
	printk("\t sot_potag_action     %u\n",		param.table.sot_potag_action);
	printk("\t sot_otag_action      %u\n",		param.table.sot_otag_action);
	printk("\t dt_pitag_action      %u\n",		param.table.dt_pitag_action);
	printk("\t dt_itag_action       %u\n",		param.table.dt_itag_action);
	printk("\t dt_potag_action      %u\n",		param.table.dt_potag_action);
	printk("\t dt_otag_action       %u\n",		param.table.dt_otag_action);
	printk("\t tt_pitag_action      %u\n",		param.table.tt_pitag_action);
	printk("\t tt_itag_action       %u\n",		param.table.tt_itag_action);
	printk("\t tt_potag_action      %u\n",		param.table.tt_potag_action);
	printk("\t tt_otag_action       %u\n",		param.table.tt_otag_action);
	printk("\t tt_patag_action      %u\n",		param.table.tt_patag_action);
	printk("\t tt_atag_action       %u\n",		param.table.tt_atag_action);
}

void evlan_xlt_table_update(VLAN_t *priv, u8 iport_num, u8 oport_num, int valid,
		int old_ivid, int old_ovid, int new_ivid, int new_ovid,
		int old_ivid_mask, int old_ovid_mask, int evlan_act_idx, int new_ipri, int new_opri)
{
	union evlan_xlt_table_cfg param = {0};
	param.table.valid			= valid;
	param.table.old_ivid_mask	= old_ivid_mask;
	param.table.old_ovid_mask	= old_ovid_mask;
	param.table.oport_num_mask	= 0x0;
	param.table.action_index	= evlan_act_idx;
	param.table.new_ipri		= new_ipri;
	param.table.new_opri		= new_opri;
	param.table.new_ivid		= new_ivid;
	param.table.new_ovid		= new_ovid;
	param.table.old_ivid		= old_ivid;
	param.table.old_ovid		= old_ovid;
	param.table.oport_num		= oport_num;

	priv->cpriv->table_write(priv->cpriv, EVLAN_XLT_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
}

void evlan_xlt_table_dump(VLAN_t *priv, u8 iport_num)
{
	union evlan_xlt_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, EVLAN_XLT_LITE_HASH_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("evlan xlt table:\n");
	printk("\t valid                    %u\n",	param.table.valid);
	printk("\t old_ivid_mask            %u\n",	param.table.old_ivid_mask);
	printk("\t old_ovid_mask            %u\n",	param.table.old_ovid_mask);
	printk("\t action_index             %u\n",	param.table.action_index);
	printk("\t new_ipri                 %u\n",	param.table.new_ipri);
	printk("\t new_opri                 %u\n",	param.table.new_opri);
	printk("\t new_ivid                 %u\n",	param.table.new_ivid);
	printk("\t new_ovid                 %u\n",	param.table.new_ovid);
	printk("\t old_ivid                 %u\n",	param.table.old_ivid);
	printk("\t old_ovid                 %u\n",	param.table.old_ovid);
	printk("\t oport_num                %u\n",	param.table.oport_num);
}

void evlan_otpid_table_update(VLAN_t *priv, u8 iport_num, u32 tpid)
{
	union evlan_otpid_table_cfg param = {0};

	param.table.outer_tpid = tpid;

	priv->cpriv->table_write(priv->cpriv, EVLAN_OTPID_DIRECT_TABLE, iport_num, param.data,
			sizeof(param));
}

void evlan_otpid_table_dump(VLAN_t *priv, u8 iport_num)
{
	union evlan_otpid_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, EVLAN_OTPID_DIRECT_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("evlan port otpid table:\n");
	printk("\t outer_tpid                0x%x\n",	param.table.outer_tpid);
}

void evlan_ptpid_table_update(VLAN_t *priv, u8 iport_num, u32 itpid, u32 otpid)
{
	union evlan_port_tpid_table_cfg param = {0};

	param.table.inner_tpid = itpid;
	param.table.outer_tpid = otpid;

	priv->cpriv->table_write(priv->cpriv, EVLAN_TPID_HASH_TABLE, iport_num, param.data,
			sizeof(param));
}

void evlan_ptpid_table_dump(VLAN_t *priv, u8 iport_num)
{
	union evlan_port_tpid_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, EVLAN_TPID_HASH_TABLE, iport_num, param.data,
			sizeof(param));
	printk("\n---------------------------------------\n");
	printk("evlan port ptpid table:\n");
	printk("\t inner_tpid                0x%x\n",	param.table.inner_tpid);
	printk("\t outer_tpid                0x%x\n",	param.table.outer_tpid);
}
