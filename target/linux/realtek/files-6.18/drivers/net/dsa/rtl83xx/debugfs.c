// SPDX-License-Identifier: GPL-2.0-only

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "mac.h"
#include "l2.h"
#include "rtl-otto.h"
#include "vlan.h"
#include "stp.h"

#define RTL838X_STORM_CTRL_PORT_UC(p)		(0x4718 + (((p) << 2)))
#define RTL838X_STORM_CTRL_PORT_MC(p)		(0x478c + (((p) << 2)))
#define RTL838X_STORM_CTRL_PORT_BC(p)		(0x4800 + (((p) << 2)))
#define RTL839X_STORM_CTRL_PORT_UC_0(p)		(0x185C + (((p) << 3)))
#define RTL839X_STORM_CTRL_PORT_MC_0(p)		(0x19FC + (((p) << 3)))
#define RTL839X_STORM_CTRL_PORT_BC_0(p)		(0x1B9C + (((p) << 3)))

#define RTL838X_LED_GLB_CTRL			(0xA000)

/* LED control by switch */
#define RTL838X_LED_MODE_SEL			(0x1004)
#define RTL838X_LED_MODE_CTRL			(0xA004)
#define RTL838X_LED_P_EN_CTRL			(0xA008)

/* LED control by software */
#define RTL838X_LED_SW_CTRL			(0xA00C)
#define RTL838X_LED0_SW_P_EN_CTRL		(0xA010)
#define RTL838X_LED1_SW_P_EN_CTRL		(0xA014)
#define RTL838X_LED2_SW_P_EN_CTRL		(0xA018)
#define RTL838X_LED_SW_P_CTRL			(0xA01C)
#define RTL838X_LED_SW_P_CTRL_PORT(p)		(RTL838X_LED_SW_P_CTRL + (((p) << 2)))

#define RTL838X_DRIVER_NAME "rtl838x"

#define RTL8390_LED_GLB_CTRL			(0x00E4)
#define RTL8390_LED_SET_2_3_CTRL		(0x00E8)
#define RTL8390_LED_SET_0_1_CTRL		(0x00EC)
#define RTL8390_LED_COPR_SET_SEL_CTRL(p)	(0x00F0 + (((p >> 4) << 2)))
#define RTL8390_LED_FIB_SET_SEL_CTRL(p)		(0x0100 + (((p >> 4) << 2)))
#define RTL8390_LED_COPR_PMASK_CTRL(p)		(0x0110 + (((p >> 5) << 2)))
#define RTL8390_LED_FIB_PMASK_CTRL(p)		(0x00118 + (((p >> 5) << 2)))
#define RTL8390_LED_COMBO_CTRL(p)		(0x0120 + (((p >> 5) << 2)))
#define RTL8390_LED_SW_CTRL			(0x0128)
#define RTL8390_LED_SW_P_EN_CTRL(p)		(0x012C + (((p / 10) << 2)))
#define RTL8390_LED_SW_P_CTRL(p)		(0x0144 + (((p) << 2)))

#define RTL838X_MIR_QID_CTRL(grp)		(0xAD44 + (((grp) << 2)))
#define RTL838X_MIR_RSPAN_VLAN_CTRL(grp)	(0xA340 + (((grp) << 2)))
#define RTL838X_MIR_RSPAN_VLAN_CTRL_MAC(grp)	(0xAA70 + (((grp) << 2)))
#define RTL838X_MIR_RSPAN_TX_CTRL		(0xA350)
#define RTL838X_MIR_RSPAN_TX_TAG_RM_CTRL	(0xAA80)
#define RTL838X_MIR_RSPAN_TX_TAG_EN_CTRL	(0xAA84)
#define RTL839X_MIR_RSPAN_VLAN_CTRL(grp)	(0xA340 + (((grp) << 2)))
#define RTL839X_MIR_RSPAN_TX_CTRL		(0x69b0)
#define RTL839X_MIR_RSPAN_TX_TAG_RM_CTRL	(0x2550)
#define RTL839X_MIR_RSPAN_TX_TAG_EN_CTRL	(0x2554)
#define RTL839X_MIR_SAMPLE_RATE_CTRL		(0x2558)

static ssize_t rtl838x_common_read(char __user *buffer, size_t count,
				   loff_t *ppos, unsigned int value)
{
	char *buf;
	ssize_t len;

	if (*ppos != 0)
		return 0;

	buf = kasprintf(GFP_KERNEL, "0x%08x\n", value);
	if (!buf)
		return -ENOMEM;

	if (count < strlen(buf)) {
		kfree(buf);
		return -ENOSPC;
	}

	len = simple_read_from_buffer(buffer, count, ppos, buf, strlen(buf));
	kfree(buf);

	return len;
}

static ssize_t rtl838x_common_write(const char __user *buffer, size_t count,
				    loff_t *ppos, unsigned int *value)
{
	char b[32];
	ssize_t len;
	int ret;

	if (*ppos != 0)
		return -EINVAL;

	if (count >= sizeof(b))
		return -ENOSPC;

	len = simple_write_to_buffer(b, sizeof(b) - 1, ppos,
				     buffer, count);
	if (len < 0)
		return len;

	b[len] = '\0';
	ret = kstrtouint(b, 16, value);
	if (ret)
		return -EIO;

	return len;
}

static ssize_t stp_state_read(struct file *filp, char __user *buffer, size_t count,
			      loff_t *ppos)
{
	struct rtldsa_port *p = filp->private_data;
	struct dsa_switch *ds = p->dp->ds;
	int state = rtldsa_port_get_stp_state(ds->priv, p->dp->index);

	if (state < 0)
		return state;

	return rtl838x_common_read(buffer, count, ppos, (u32)state);
}

static ssize_t stp_state_write(struct file *filp, const char __user *buffer,
			       size_t count, loff_t *ppos)
{
	struct rtldsa_port *p = filp->private_data;
	u32 value;
	size_t res = rtl838x_common_write(buffer, count, ppos, &value);

	if (res < 0)
		return res;

	rtldsa_port_stp_state_set(p->dp->ds, p->dp->index, (u8)value);

	return res;
}

static const struct file_operations stp_state_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = stp_state_read,
	.write = stp_state_write,
};

static int rtldsa_pmsks_table_raw_show(struct seq_file *m, void *v)
{
	struct otto_dsa_priv *priv = m->private;
	u64 all_ports;

	mutex_lock(&priv->reg_mutex);

	for (int i = 0; i < MAX_MC_PMASKS; i += 4) {
		seq_printf(m, "%04i: ", i);
		for (int j = 0; j < 4; j++) {
			bool is_set = test_bit(i + j, priv->mc_group_bm);
			seq_printf(m, " %c0x%016llx%c", is_set ? ' ' : '(',
				   priv->r->read_mcast_pmask(i + j),
				   is_set ? ' ' : ')');
		}
		seq_printf(m, "\n");
	}

	all_ports = priv->r->read_mcast_pmask(MC_PMASK_ALL_PORTS_IDX);
	seq_printf(m, "MC_PMASK_ALL_PORTS (%i): 0x%016llx\n",
		   MC_PMASK_ALL_PORTS_IDX, all_ports);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtldsa_pmsks_table_raw_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, rtldsa_pmsks_table_raw_show, inode->i_private);
}

static const struct file_operations rtldsa_pmsks_table_raw_fops = {
	.owner = THIS_MODULE,
	.open = rtldsa_pmsks_table_raw_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtldsa_pmsks_table_show(struct seq_file *m, void *v)
{
	struct otto_dsa_priv *priv = m->private;
	u64 ports;

	mutex_lock(&priv->reg_mutex);

	for (int i = 0; i < MC_PMASK_ALL_PORTS_IDX; i++) {
		if (!test_bit(i, priv->mc_group_bm))
			continue;

		ports = priv->r->read_mcast_pmask(i);
		seq_printf(m, "%04i:", i);
		for (int j = 0; j < sizeof(ports)*8; j++)
			if (ports & BIT_ULL(j))
				seq_printf(m, " %i", j);
		seq_printf(m, "\n");
	}

	ports = priv->r->read_mcast_pmask(MC_PMASK_ALL_PORTS_IDX);
	seq_printf(m, "MC_PMASK_ALL_PORTS (%i):", MC_PMASK_ALL_PORTS_IDX);
	for (int i = 0; i < sizeof(ports)*8; i++)
		if (ports & BIT_ULL(i))
			seq_printf(m, " %i", i);
	seq_printf(m, "\n");

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtldsa_pmsks_table_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, rtldsa_pmsks_table_show, inode->i_private);
}

static const struct file_operations rtldsa_pmsks_table_fops = {
	.owner = THIS_MODULE,
	.open = rtldsa_pmsks_table_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtldsa_vlan_profiles_show(struct seq_file *m, void *v)
{
	struct otto_dsa_priv *priv = m->private;
	struct rtldsa_vlan_profile profile;
	int ret, profiles_max;

	if (!priv->r->vlan_profile_get)
		return -ENOTSUPP;

	profiles_max = max(RTL838X_VLAN_PROFILE_MAX, RTL839X_VLAN_PROFILE_MAX);
	profiles_max = max(profiles_max, RTL930X_VLAN_PROFILE_MAX);
	profiles_max = max(profiles_max, RTL931X_VLAN_PROFILE_MAX);

	mutex_lock(&priv->reg_mutex);

	seq_printf(m,
		   "prof-idx: L2 learn | UNKN L2MC FLD PMSK (IDX) | UNKN IPMC FLD PMSK (IDX) | UNKN IPv6MC FLD PMSK (IDX)\n");
	for (int i = 0; i <= profiles_max; i++) {
		ret = priv->r->vlan_profile_get(priv, i, &profile);
		if (ret < 0)
			break;

		if (profile.pmsk_is_idx)
			seq_printf(m, "%i: %i %03i %03i %03i\n", i,
				   profile.l2_learn,
				   profile.unkn_mc_fld.pmsks_idx.l2,
				   profile.unkn_mc_fld.pmsks_idx.ip,
				   profile.unkn_mc_fld.pmsks_idx.ip6);
		else
			seq_printf(m, "%i: %i 0x%016llx 0x%016llx 0x%016llx\n", i,
				   profile.l2_learn,
				   profile.unkn_mc_fld.pmsks.l2,
				   profile.unkn_mc_fld.pmsks.ip,
				   profile.unkn_mc_fld.pmsks.ip6);
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtldsa_vlan_profiles_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, rtldsa_vlan_profiles_show, inode->i_private);
}

static const struct file_operations rtldsa_vlan_profiles_fops = {
	.owner = THIS_MODULE,
	.open = rtldsa_vlan_profiles_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtldsa_vlan_table_raw_show(struct seq_file *m, void *v)
{
	struct otto_dsa_priv *priv = m->private;
	struct otto_vlan_info info;

	if (!priv->r->vlan_tables_read)
		return -ENOTSUPP;

	mutex_lock(&priv->reg_mutex);

	seq_printf(m, "VID: profile-index untagged-ports member-ports\n");

	for (int i = 0; i < MAX_VLANS; i++) {
		priv->r->vlan_tables_read(i, &info);

		if (!info.member_ports)
			continue;

		seq_printf(m, "%i: %2i 0x%016llx 0x%016llx\n", i,
			   info.profile_id,
			   info.untagged_ports,
			   info.member_ports);
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtldsa_vlan_table_raw_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, rtldsa_vlan_table_raw_show, inode->i_private);
}

static const struct file_operations rtldsa_vlan_table_raw_fops = {
	.owner = THIS_MODULE,
	.open = rtldsa_vlan_table_raw_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtldsa_vlan_table_show(struct seq_file *m, void *v)
{
	struct otto_dsa_priv *priv = m->private;
	struct otto_vlan_info info;

	if (!priv->r->vlan_tables_read)
		return -ENOTSUPP;

	mutex_lock(&priv->reg_mutex);

	for (int i = 0; i < MAX_VLANS; i++) {
		priv->r->vlan_tables_read(i, &info);

		if (!info.member_ports)
			continue;

		seq_printf(m, "%i: %i |", i, info.profile_id);
		for (int j = 0; j < sizeof(info.member_ports)*8; j++)
			if (info.member_ports & BIT_ULL(j))
				seq_printf(m, " %i%s", j,
					   (info.untagged_ports & BIT_ULL(j)) ? "" : "t");
		seq_printf(m, "\n");
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int rtldsa_vlan_table_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, rtldsa_vlan_table_show, inode->i_private);
}

static const struct file_operations rtldsa_vlan_table_fops = {
	.owner = THIS_MODULE,
	.open = rtldsa_vlan_table_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static ssize_t age_out_read(struct file *filp, char __user *buffer, size_t count,
			    loff_t *ppos)
{
	struct rtldsa_port *p = filp->private_data;
	struct dsa_switch *ds = p->dp->ds;
	struct otto_dsa_priv *priv = ds->priv;
	int value = sw_r32(priv->r->l2_port_aging_out);

	if (value < 0)
		return -EINVAL;

	return rtl838x_common_read(buffer, count, ppos, (u32)value);
}

static ssize_t age_out_write(struct file *filp, const char __user *buffer,
			     size_t count, loff_t *ppos)
{
	struct rtldsa_port *p = filp->private_data;
	u32 value;
	size_t res = rtl838x_common_write(buffer, count, ppos, &value);

	if (res < 0)
		return res;

	rtldsa_port_fast_age(p->dp->ds, p->dp->index);

	return res;
}

static const struct file_operations age_out_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = age_out_read,
	.write = age_out_write,
};

static ssize_t port_egress_rate_read(struct file *filp, char __user *buffer, size_t count,
				     loff_t *ppos)
{
	struct rtldsa_port *p = filp->private_data;
	struct dsa_switch *ds = p->dp->ds;
	struct otto_dsa_priv *priv = ds->priv;
	u32 value;

	value = priv->r->get_egress_rate(priv, p->dp->index);

	return rtl838x_common_read(buffer, count, ppos, value);
}

static ssize_t port_egress_rate_write(struct file *filp, const char __user *buffer,
				      size_t count, loff_t *ppos)
{
	struct rtldsa_port *p = filp->private_data;
	struct dsa_switch *ds = p->dp->ds;
	struct otto_dsa_priv *priv = ds->priv;
	u32 value;
	ssize_t res;

	res = rtl838x_common_write(buffer, count, ppos, &value);
	if (res < 0)
		return res;

	priv->r->set_egress_rate(priv, p->dp->index, value);

	return res;
}

static const struct file_operations port_egress_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = port_egress_rate_read,
	.write = port_egress_rate_write,
};

static const struct debugfs_reg32 port_ctrl_regs[] = {
	{ .name = "port_isolation", .offset = RTL838X_PORT_ISO_CTRL(0), },
	{ .name = "mac_force_mode", .offset = RTL838X_MAC_FORCE_MODE_CTRL, },
};

static void rtl838x_dbgfs_cleanup(struct otto_dsa_priv *priv)
{
	debugfs_remove_recursive(priv->dbgfs_dir);

/*	kfree(priv->dbgfs_entries); */
}

static int rtl838x_dbgfs_port_init(struct dentry *parent, struct otto_dsa_priv *priv,
				   int port)
{
	struct dentry *port_dir;
	struct debugfs_regset32 *port_ctrl_regset;

	port_dir = debugfs_create_dir(priv->ports[port].dp->name, parent);

	if (priv->family_id == RTL8380_FAMILY_ID) {
		debugfs_create_x32("storm_rate_uc", 0644, port_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_UC(port)));

		debugfs_create_x32("storm_rate_mc", 0644, port_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_MC(port)));

		debugfs_create_x32("storm_rate_bc", 0644, port_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_BC(port)));
	} else {
		debugfs_create_x32("storm_rate_uc", 0644, port_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL839X_STORM_CTRL_PORT_UC_0(port)));

		debugfs_create_x32("storm_rate_mc", 0644, port_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL839X_STORM_CTRL_PORT_MC_0(port)));

		debugfs_create_x32("storm_rate_bc", 0644, port_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL839X_STORM_CTRL_PORT_BC_0(port)));
	}

	debugfs_create_u32("id", 0444, port_dir, (u32 *)&priv->ports[port].dp->index);

	port_ctrl_regset = devm_kzalloc(priv->dev, sizeof(*port_ctrl_regset), GFP_KERNEL);
	if (!port_ctrl_regset)
		return -ENOMEM;

	port_ctrl_regset->regs = port_ctrl_regs;
	port_ctrl_regset->nregs = ARRAY_SIZE(port_ctrl_regs);
	port_ctrl_regset->base = (void *)(RTL838X_SW_BASE + (port << 2));
	debugfs_create_regset32("port_ctrl", 0400, port_dir, port_ctrl_regset);

	debugfs_create_file("stp_state", 0600, port_dir, &priv->ports[port], &stp_state_fops);
	debugfs_create_file("age_out", 0600, port_dir, &priv->ports[port], &age_out_fops);
	debugfs_create_file("port_egress_rate", 0600, port_dir, &priv->ports[port],
			    &port_egress_fops);
	return 0;
}

static int rtl838x_dbgfs_leds(struct dentry *parent, struct otto_dsa_priv *priv)
{
	struct dentry *led_dir;

	led_dir = debugfs_create_dir("led", parent);

	if (priv->family_id == RTL8380_FAMILY_ID) {
		debugfs_create_x32("led_glb_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED_GLB_CTRL));
		debugfs_create_x32("led_mode_sel", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED_MODE_SEL));
		debugfs_create_x32("led_mode_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED_MODE_CTRL));
		debugfs_create_x32("led_p_en_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED_P_EN_CTRL));
		debugfs_create_x32("led_sw_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED_SW_CTRL));
		debugfs_create_x32("led0_sw_p_en_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED0_SW_P_EN_CTRL));
		debugfs_create_x32("led1_sw_p_en_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED1_SW_P_EN_CTRL));
		debugfs_create_x32("led2_sw_p_en_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_LED2_SW_P_EN_CTRL));
		for (int p = 0; p < 28; p++) {
			char led_sw_p_ctrl_name[20];

			snprintf(led_sw_p_ctrl_name, sizeof(led_sw_p_ctrl_name),
				 "led_sw_p_ctrl.%02d", p);
			debugfs_create_x32(led_sw_p_ctrl_name, 0644, led_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_LED_SW_P_CTRL_PORT(p)));
		}
	} else if (priv->family_id == RTL8390_FAMILY_ID) {
		char port_led_name[20];

		debugfs_create_x32("led_glb_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_GLB_CTRL));
		debugfs_create_x32("led_set_2_3", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_SET_2_3_CTRL));
		debugfs_create_x32("led_set_0_1", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_SET_0_1_CTRL));
		for (int p = 0; p < 4; p++) {
			snprintf(port_led_name, sizeof(port_led_name), "led_copr_set_sel.%1d", p);
			debugfs_create_x32(port_led_name, 0644, led_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_COPR_SET_SEL_CTRL(p << 4)));
			snprintf(port_led_name, sizeof(port_led_name), "led_fib_set_sel.%1d", p);
			debugfs_create_x32(port_led_name, 0644, led_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_FIB_SET_SEL_CTRL(p << 4)));
		}
		debugfs_create_x32("led_copr_pmask_ctrl_0", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_COPR_PMASK_CTRL(0)));
		debugfs_create_x32("led_copr_pmask_ctrl_1", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_COPR_PMASK_CTRL(32)));
		debugfs_create_x32("led_fib_pmask_ctrl_0", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_FIB_PMASK_CTRL(0)));
		debugfs_create_x32("led_fib_pmask_ctrl_1", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_FIB_PMASK_CTRL(32)));
		debugfs_create_x32("led_combo_ctrl_0", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_COMBO_CTRL(0)));
		debugfs_create_x32("led_combo_ctrl_1", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_COMBO_CTRL(32)));
		debugfs_create_x32("led_sw_ctrl", 0644, led_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_SW_CTRL));
		for (int p = 0; p < 5; p++) {
			snprintf(port_led_name, sizeof(port_led_name), "led_sw_p_en_ctrl.%1d", p);
			debugfs_create_x32(port_led_name, 0644, led_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_SW_P_EN_CTRL(p * 10)));
		}
		for (int p = 0; p < 28; p++) {
			snprintf(port_led_name, sizeof(port_led_name), "led_sw_p_ctrl.%02d", p);
			debugfs_create_x32(port_led_name, 0644, led_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL8390_LED_SW_P_CTRL(p)));
		}
	}
	return 0;
}

void rtl838x_dbgfs_init(struct otto_dsa_priv *priv)
{
	struct dentry *rtl838x_dir;
	struct dentry *port_dir;
	struct dentry *mirror_dir;
	struct debugfs_regset32 *port_ctrl_regset;
	int ret;
	char lag_name[10];
	char mirror_name[10];

	pr_info("%s called\n", __func__);
	rtl838x_dir = debugfs_lookup(RTL838X_DRIVER_NAME, NULL);
	if (!rtl838x_dir)
		rtl838x_dir = debugfs_create_dir(RTL838X_DRIVER_NAME, NULL);

	priv->dbgfs_dir = rtl838x_dir;

	debugfs_create_x32("soc", 0444, rtl838x_dir,
			   (u32 *)(RTL838X_SW_BASE + RTL838X_MODEL_NAME_INFO));

	/* Create one directory per port */
	for (int i = 0; i < priv->r->cpu_port; i++) {
		if (priv->ports[i].phy) {
			ret = rtl838x_dbgfs_port_init(rtl838x_dir, priv, i);
			if (ret)
				goto err;
		}
	}

	/* Create directory for CPU-port */
	port_dir = debugfs_create_dir("cpu_port", rtl838x_dir);
	port_ctrl_regset = devm_kzalloc(priv->dev, sizeof(*port_ctrl_regset), GFP_KERNEL);
	if (!port_ctrl_regset) {
		ret = -ENOMEM;
		goto err;
	}

	port_ctrl_regset->regs = port_ctrl_regs;
	port_ctrl_regset->nregs = ARRAY_SIZE(port_ctrl_regs);
	port_ctrl_regset->base = (void *)(RTL838X_SW_BASE + (priv->r->cpu_port << 2));
	debugfs_create_regset32("port_ctrl", 0400, port_dir, port_ctrl_regset);
	debugfs_create_u8("id", 0444, port_dir, (u8 *)&priv->r->cpu_port);

	/* Create entries for LAGs */
	for (int i = 0; i < priv->ds->num_lag_ids; i++) {
		snprintf(lag_name, sizeof(lag_name), "lag.%02d", i);
		if (priv->family_id == RTL8380_FAMILY_ID)
			debugfs_create_x32(lag_name, 0644, rtl838x_dir,
					   (u32 *)(RTL838X_SW_BASE + priv->r->trk_mbr_ctr(i)));
		else
			debugfs_create_x64(lag_name, 0644, rtl838x_dir,
					   (u64 *)(RTL838X_SW_BASE + priv->r->trk_mbr_ctr(i)));
	}

	/* Create directories for mirror groups */
	for (int i = 0; i < 4; i++) {
		snprintf(mirror_name, sizeof(mirror_name), "mirror.%1d", i);
		mirror_dir = debugfs_create_dir(mirror_name, rtl838x_dir);
		if (priv->family_id == RTL8380_FAMILY_ID) {
			debugfs_create_x32("ctrl", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_CTRL + i * 4));
			debugfs_create_x32("ingress_pm", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_SPM_CTRL + i * 4));
			debugfs_create_x32("egress_pm", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_DPM_CTRL + i * 4));
			debugfs_create_x32("qid", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_QID_CTRL(i)));
			debugfs_create_x32("rspan_vlan", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_RSPAN_VLAN_CTRL(i)));
			debugfs_create_x32("rspan_vlan_mac", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_RSPAN_VLAN_CTRL_MAC(i)));
			debugfs_create_x32("rspan_tx", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_RSPAN_TX_CTRL));
			debugfs_create_x32("rspan_tx_tag_rm", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_RSPAN_TX_TAG_RM_CTRL));
			debugfs_create_x32("rspan_tx_tag_en", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL838X_MIR_RSPAN_TX_TAG_EN_CTRL));
		} else {
			debugfs_create_x32("ctrl", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL839X_MIR_CTRL + i * 4));
			debugfs_create_x64("ingress_pm", 0644, mirror_dir,
					   (u64 *)(RTL838X_SW_BASE + RTL839X_MIR_SPM_CTRL + i * 8));
			debugfs_create_x64("egress_pm", 0644, mirror_dir,
					   (u64 *)(RTL838X_SW_BASE + RTL839X_MIR_DPM_CTRL + i * 8));
			debugfs_create_x32("rspan_vlan", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL839X_MIR_RSPAN_VLAN_CTRL(i)));
			debugfs_create_x32("rspan_tx", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL839X_MIR_RSPAN_TX_CTRL));
			debugfs_create_x32("rspan_tx_tag_rm", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL839X_MIR_RSPAN_TX_TAG_RM_CTRL));
			debugfs_create_x32("rspan_tx_tag_en", 0644, mirror_dir,
					   (u32 *)(RTL838X_SW_BASE + RTL839X_MIR_RSPAN_TX_TAG_EN_CTRL));
			debugfs_create_x64("sample_rate", 0644, mirror_dir,
					   (u64 *)(RTL838X_SW_BASE + RTL839X_MIR_SAMPLE_RATE_CTRL));
		}
	}

	if (priv->family_id == RTL8380_FAMILY_ID)
		debugfs_create_x32("bpdu_flood_mask", 0644, rtl838x_dir,
				   (u32 *)(RTL838X_SW_BASE + priv->r->rma_bpdu_fld_pmask));
	else
		debugfs_create_x64("bpdu_flood_mask", 0644, rtl838x_dir,
				   (u64 *)(RTL838X_SW_BASE + priv->r->rma_bpdu_fld_pmask));

	if (priv->family_id == RTL8380_FAMILY_ID)
		debugfs_create_x32("vlan_ctrl", 0644, rtl838x_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL838X_VLAN_CTRL));
	else
		debugfs_create_x32("vlan_ctrl", 0644, rtl838x_dir,
				   (u32 *)(RTL838X_SW_BASE + RTL839X_VLAN_CTRL));

	ret = rtl838x_dbgfs_leds(rtl838x_dir, priv);
	if (ret)
		goto err;

	debugfs_create_file("port_masks_table_raw", 0400, rtl838x_dir, priv,
			    &rtldsa_pmsks_table_raw_fops);

	debugfs_create_file("port_masks_table", 0400, rtl838x_dir, priv,
			    &rtldsa_pmsks_table_fops);

	debugfs_create_file("vlan_profiles", 0400, rtl838x_dir, priv,
			    &rtldsa_vlan_profiles_fops);

	debugfs_create_file("vlan_table_raw", 0400, rtl838x_dir, priv,
			    &rtldsa_vlan_table_raw_fops);

	debugfs_create_file("vlan_table", 0400, rtl838x_dir, priv,
			    &rtldsa_vlan_table_fops);

	return;
err:
	rtl838x_dbgfs_cleanup(priv);
}

void rtl930x_dbgfs_init(struct otto_dsa_priv *priv)
{
	struct dentry *dbg_dir;

	pr_info("%s called\n", __func__);
	dbg_dir = debugfs_lookup(RTL838X_DRIVER_NAME, NULL);
	if (!dbg_dir)
		dbg_dir = debugfs_create_dir(RTL838X_DRIVER_NAME, NULL);

	priv->dbgfs_dir = dbg_dir;

	debugfs_create_file("port_masks_table_raw", 0400, dbg_dir, priv,
			    &rtldsa_pmsks_table_raw_fops);

	debugfs_create_file("port_masks_table", 0400, dbg_dir, priv,
			    &rtldsa_pmsks_table_fops);

	debugfs_create_file("vlan_profiles", 0400, dbg_dir, priv,
			    &rtldsa_vlan_profiles_fops);

	debugfs_create_file("vlan_table_raw", 0400, dbg_dir, priv,
			    &rtldsa_vlan_table_raw_fops);

	debugfs_create_file("vlan_table", 0400, dbg_dir, priv,
			    &rtldsa_vlan_table_fops);
}
