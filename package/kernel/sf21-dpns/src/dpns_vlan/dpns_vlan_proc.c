#include <linux/proc_fs.h>
#include "init.h"
#include "ivlan_se.h"
#include "evlan_se.h"
#include "vport_se.h"

#define MAX_VALUES	15
static struct dpns_vlan_priv *g_priv;

static int vlan_proc_show(struct seq_file *file, void *data)
{
	seq_printf(file, "you can use as these: \n"

		" Attention: all example should append with '> /proc/dpns_vlan'\n"

		" dump ivlan/evlan table content: [tbl_name] can be replaced with iport/ivlan_pbv/\n"
							"ivlan_lkp/ivlan_xlt/ivlan_spl/evlan_lkp/\n"
							"evlan_act/evlan_xlt/evlan_tpid/vlan_vport_map/\n"
							"modify_vport_map/tmu_ivport_map\n"
							"[index] is table entry index\n"
		" example: echo dump [tbl_name] [index] \n"
		" example: echo set ivlan_pbv [index] [cpuPort] [action] [valid] [dt_otag] [dt_potag] [sot_otag] [sot_potag] [sit_otag] [sit_potag] [un_otag] [un_potag] [def_action] [pri]\n"
		" example: echo set iport [index] [ivid] [ovid] [valid]\n"
		" example: echo set ivlan_lkp [index] [vid] [l2miss2cpu] [l2nonUcast2cpu] [valid] [port_map]\n"
		" example: echo set ivlan_xlt [index] [vid] [valid]\n"
		" example: echo set ivlan_spl [index] [credit]\n"
		" example: echo set evlan_lkp [index] [vid] [valid] [un_bitmap] [port_map]\n"
		" example: echo set evlan_act [index] [sot_action] [psot_action] [dt_action] [pdt_action] [def_action]\n"
		" example: echo set evlan_xlt [index] [oport_num] [valid] [old_ivid] [old_ovid] [new_ivid] [new_ovid] [old_ivid_mask] [old_ovid_mask] [evlan_act_idx] [new_ipri] [new_opri]\n"
		" example: echo set evlan_ptpid [index] [tpid]\n"
		" example: echo set evlan_otpid [index] [tpid]\n"
		" example: echo set vport_table [is_add] [vid] [port] [vport]\n"
		" example: echo set en_vport_table [is_en]\n");


	return 0;
}

int dpns_vlan_dump(char *str) {
	VLAN_t* priv = g_priv;
	char *table_name, *index;
	int ret;
	u8 val = 0;

	table_name = strsep(&str, "\t \n");
	if (!table_name) {
		VLAN_DBG(ERR_LV, "--dump table name error--\n");
		return -1;
	}

	index = strsep(&str, "\t \n");

	if (index)
		ret = kstrtou8(index, 0, &val);

	if (!strncmp(table_name, "iport", strlen("iport")))
		iport_table_dump(priv, val);
	else if (!strncmp(table_name, "ivlan_pbv", strlen("ivlan_pbv")))
		ivlan_pbv_table_dump(priv, val);
	else if (!strncmp(table_name, "ivlan_lkp", strlen("ivlan_lkp")))
		ivlan_lkp_table_dump(priv, val);
	else if (!strncmp(table_name, "ivlan_xlt", strlen("ivlan_xlt")))
		ivlan_xlt_table_dump(priv, val);
	else if (!strncmp(table_name, "ivlan_spl", strlen("ivlan_spl")))
		ivlan_spl_table_dump(priv, val);
	else if (!strncmp(table_name, "evlan_lkp", strlen("evlan_lkp")))
		evlan_lkp_table_dump(priv, val);
	else if (!strncmp(table_name, "evlan_act", strlen("evlan_act")))
		evlan_act_table_dump(priv, val);
	else if (!strncmp(table_name, "evlan_xlt", strlen("evlan_xlt")))
		evlan_xlt_table_dump(priv, val);
	else if (!strncmp(table_name, "evlan_ptpid", strlen("evlan_ptpid")))
		evlan_ptpid_table_dump(priv, val);
	else if (!strncmp(table_name, "evlan_otpid", strlen("evlan_otpid")))
		evlan_otpid_table_dump(priv, val);
	else if (!strncmp(table_name, "vlan_vport_map", strlen("vlan_vport_map")))
		vlan_vport_map_dump(priv, val);
	else if (!strncmp(table_name, "modify_vport_map", strlen("modify_vport_map")))
		modify_vport_map_dump(priv);
	else if (!strncmp(table_name, "tmu_ivport_map", strlen("tmu_ivport_map")))
		tmu_ivport_map_dump(priv);
	else {
		VLAN_DBG(ERR_LV, "--dump table name error--\n");
		return -1;
	}

	return 0;
}

int dpns_vlan_set(char *str) {
	VLAN_t* priv = g_priv;
	char *table_name;
	char *tmp_str;
	u32 val[MAX_VALUES];
	int ret, i;

	table_name = strsep(&str, "\t \n");
	if (!table_name) {
		VLAN_DBG(ERR_LV, "--set table name error--\n");
		return -1;
	}

	for (i = 0; i < MAX_VALUES; i++) {
		tmp_str = strsep(&str, "\t \n");
		if (!tmp_str)
			break;
		ret = kstrtou32(tmp_str, 0, &val[i]);
	}

	if (!strncmp(table_name, "iport", strlen("iport")))
		iport_table_update(priv, val[0], val[1], val[2], val[3]);
	else if (!strncmp(table_name, "ivlan_pbv", strlen("ivlan_pbv")))
		ivlan_pbv_table_update(priv,val[0], val[1], val[2], val[3], val[4], val[5],
					val[6], val[7], val[8], val[9], val[10],
					val[11], val[12], val[13]);
	else if (!strncmp(table_name, "ivlan_lkp", strlen("ivlan_lkp")))
		ivlan_lkp_table_update(priv, val[0], val[1], val[2], val[3], val[4], val[5]);
	else if (!strncmp(table_name, "ivlan_xlt", strlen("ivlan_xlt")))
		ivlan_xlt_table_update(priv, val[0], val[1], val[2]);
	else if (!strncmp(table_name, "ivlan_spl", strlen("ivlan_spl")))
		ivlan_spl_table_update(priv, val[0], val[1]);
	else if (!strncmp(table_name, "evlan_lkp", strlen("evlan_lkp")))
		evlan_lkp_table_update(priv, val[0], val[1], val[2], val[3], val[4]);
	else if (!strncmp(table_name, "evlan_act", strlen("evlan_act")))
		evlan_act_table_update(priv, val[0], val[1], val[2], val[3], val[4], val[5]);
	else if (!strncmp(table_name, "evlan_xlt", strlen("evlan_xlt")))
		evlan_xlt_table_update(priv, val[0], val[1], val[2], val[3], val[4], val[5], val[6],
					val[7], val[8], val[9], val[10], val[11]);
	else if (!strncmp(table_name, "evlan_ptpid", strlen("evlan_ptpid")))
		evlan_ptpid_table_update(priv, val[0], val[1], val[2]);
	else if (!strncmp(table_name, "evlan_otpid", strlen("evlan_otpid")))
		evlan_otpid_table_update(priv, val[0], val[1]);
	else if (!strncmp(table_name, "vport_table", strlen("vport_table"))) {
		if (val[0])
			vport_update(priv, val[1], val[2], val[3]);
		else
			vport_reset(priv, val[1], val[2], val[3]);
	}
	else if (!strncmp(table_name, "en_vport_table", strlen("en_vport_table"))) {
		modify_vport_map_en(priv, val[0]);
		tmu_ivport_map_en(priv, val[0]);
		printk("is_en:%d\n", val[0]);
	}
	else {
		VLAN_DBG(ERR_LV, "--set table name error--\n");
		return -1;
	}

	printk("--%s update successfully--\n", table_name);
	return 0;
}

static ssize_t vlan_proc_write(struct file *file, const char *buffer, size_t count, loff_t *offp)
{
	char *str, *cmd;

	char tmpbuf[128] = {0};

	if (count >= sizeof(tmpbuf)) {
		VLAN_DBG(ERR_LV, "--size error--\n");
		return -1;
	}

	if (!buffer || copy_from_user(tmpbuf, buffer, count) != 0)
		return 0;

	if (count > 0) {
		str = tmpbuf;

		cmd = strsep(&str, "\t \n");
		if (!cmd) {
			VLAN_DBG(ERR_LV, "--no cmd--\n");
			return -1;
		}

		if (!strncmp(cmd, "dump", strlen("dump"))) {
			dpns_vlan_dump(str);
		}
		else if (!strncmp(cmd, "set", strlen("set"))) {
			dpns_vlan_set(str);
		}
		else {
			VLAN_DBG(ERR_LV, "--cmd is not correct--\n");
			return -1;
		}
	}

	return count;
}

static int vlan_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, vlan_proc_show, pde_data(inode));
}

const struct proc_ops vlan_ctrl = {
	.proc_write	= vlan_proc_write,
	.proc_open	= vlan_proc_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= single_release,
};

int dpns_vlan_proc_init(struct dpns_vlan_priv *priv)
{
	struct proc_dir_entry *dpns_vlan = NULL;
	g_priv = priv;

	dpns_vlan = proc_create("dpns_vlan", 0666, NULL, &vlan_ctrl);

	if (!dpns_vlan) {
		VLAN_DBG(ERR_LV, "--proc creat failed!--\n");
		return -ENOMEM;
	}

	return 0;
}

int dpns_vlan_proc_exit(void)
{
	remove_proc_entry("dpns_vlan", NULL);
	return 0;
}
