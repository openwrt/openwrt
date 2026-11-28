#include <linux/kernel.h>
#include <net/genetlink.h>

#include "sf_genl_msg.h"
#include "init.h"
#include "dpns_vlan_genl.h"
#include "ivlan_se.h"
#include "evlan_se.h"
#include "vport_se.h"

static struct dpns_vlan_priv *g_priv;

static int dump_table(struct dpns_vlan_priv *priv, u8 table_name_index, u8 index)
{
	int err = 0;
	switch (table_name_index) {
		case IPORT:
			iport_table_dump(priv, index);
			break;
		case IVLAN_PBV:
			ivlan_pbv_table_dump(priv, index);
			break;
		case IVLAN_LKP:
			ivlan_lkp_table_dump(priv, index);
			break;
		case IVALN_XLT:
			ivlan_xlt_table_dump(priv, index);
			break;
		case IVLAN_SPL:
			ivlan_spl_table_dump(priv, index);
			break;
		case EVLAN_LKP:
			evlan_lkp_table_dump(priv, index);
			break;
		case EVLAN_ACT:
			evlan_act_table_dump(priv, index);
			break;
		case EVLAN_XLT:
			evlan_xlt_table_dump(priv, index);
			break;
		case EVLAN_PTPID:
			evlan_ptpid_table_dump(priv, index);
			break;
		case EVLAN_OTPID:
			evlan_otpid_table_dump(priv, index);
			break;
		case VLAN_VPORT_MAP:
			vlan_vport_map_dump(priv, index);
			break;
		case MODIFY_VPORT_MAP:
			modify_vport_map_dump(priv);
			break;
		case TMU_IVPORT_MAP:
			tmu_ivport_map_dump(priv);
			break;
		default:
			err = -EINVAL;
	}

	return 0;
}

static int
vlan_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
	struct dpns_vlan_priv *priv = g_priv;
	struct vlan_genl_msg *msg = buf;
	int err = 0;

	if(WARN_ON_ONCE(!priv))
		return -EBUSY;

	switch (msg->method) {
		case VLAN_TABLE_DUMP:
			err = dump_table(priv, msg->table_name_index, msg->table_index);
			break;
		case SET_IPORT:
			iport_table_update(priv, msg->iport_num,
						msg->default_port,
						msg->action, msg->valid);
			break;
		case SET_IVLAN_PBV:
			ivlan_pbv_table_update(priv, msg->iport_num, msg->ivid,
						msg->ovid, msg->valid,
						msg->dt_otag, msg->dt_potag,
						msg->sot_otag, msg->sot_potag,
						msg->sit_otag, msg->sit_potag,
						msg->un_otag, msg->un_potag,
						msg->def_action, msg->pri);
			break;
		case SET_IVLAN_LKP:
			ivlan_lkp_table_update(priv, msg->iport_num, msg->vid,
						msg->l2_miss_tocpu,
						msg->l2_non_ucast_tocpu,
						msg->valid, msg->port_bitmap);
			break;
		case SET_IVLAN_XLT:
			ivlan_xlt_table_update(priv, msg->iport_num, msg->vid,
						msg->valid);
			break;
		case SET_IVLAN_SPL:
			ivlan_spl_table_update(priv, msg->iport_num,
						msg->credit);
			break;
		case SET_EVLAN_LKP:
			evlan_lkp_table_update(priv, msg->iport_num, msg->vid,
						msg->valid, msg->un_bitmap,
						msg->port_bitmap);
			break;
		case SET_EVLAN_ACT:
			evlan_act_table_update(priv, msg->iport_num,
						msg->sot_action,
						msg->psot_action,
						msg->dt_action,
						msg->pdt_action,
						msg->evlan_def_action);
			break;
		case SET_EVLAN_XLT:
			evlan_xlt_table_update(priv, msg->iport_num,
						msg->oport_num, msg->valid,
						msg->old_ivid, msg->old_ovid,
						msg->new_ivid, msg->new_ovid,
						msg->old_ivid_mask,
						msg->old_ovid_mask,
						msg->evlan_act_idx,
						msg->new_ipri,
						msg->new_opri);
			break;
		case SET_EVLAN_PTPID:
			evlan_ptpid_table_update(priv, msg->iport_num,
						msg->itpid, msg->otpid);
			break;
		case SET_EVLAN_OTPID:
			evlan_otpid_table_update(priv, msg->iport_num,
						msg->tpid);
			break;
		case EN_VLAN_VPORT:
			modify_vport_map_en(priv, msg->is_en);
			tmu_ivport_map_en(priv, msg->is_en);
			printk("is_en:%d\n",msg->is_en);
			break;
		case SET_VLAN_VPORT:
			if (msg->is_add)
				vport_update(priv, msg->vid, msg->port, msg->vport);
			else
				vport_reset(priv, msg->vid, msg->port, msg->vport);
			break;
		default:
			err = -EINVAL;
	}

	sfgenl_msg_reply(info, &err, sizeof(err));

	return err;
}

static struct sfgenl_msg_ops vlan_genl_msg_ops = {
	.msg_recv = vlan_genl_msg_recv,
};

int dpns_vlan_genl_init(struct dpns_vlan_priv *priv)
{
	g_priv = priv;
	return sfgenl_ops_register(SF_GENL_COMP_VLAN, &vlan_genl_msg_ops);
}

int dpns_vlan_genl_exit(void)
{
	return sfgenl_msg_ops_unregister(SF_GENL_COMP_VLAN);
}
