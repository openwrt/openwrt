// SPDX-License-Identifier: GPL-2.0-only

#include <linux/debugfs.h>

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl838x.h"

#define RTL838X_DRIVER_NAME "rtl838x"

static const struct debugfs_reg32 port_ctrl_regs[] = {
	{ .name = "port_isolation", .offset = RTL838X_PORT_ISO_CTRL(0), },
	{ .name = "mac_force_mode", .offset = RTL838X_MAC_FORCE_MODE_CTRL, },
};

void rtl838x_dbgfs_cleanup(struct rtl838x_switch_priv *priv)
{
	debugfs_remove_recursive(priv->dbgfs_dir);

//	kfree(priv->dbgfs_entries);
}

static int rtl838x_dbgfs_port_init(struct dentry *parent, struct rtl838x_switch_priv *priv,
				   int port)
{
	struct dentry *port_dir;
	struct debugfs_regset32 *port_ctrl_regset;

	port_dir = debugfs_create_dir(priv->ports[port].dp->name, parent);

	debugfs_create_x32("rate_uc", 0644, port_dir,
			    (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_UC(port)));

	debugfs_create_x32("rate_mc", 0644, port_dir,
			    (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_BC(port)));

	debugfs_create_x32("rate_bc", 0644, port_dir,
			    (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_BC(port)));

	debugfs_create_u32("id", 0444, port_dir, &priv->ports[port].dp->index);


	debugfs_create_x32("vlan_port_tag_sts_ctrl", 0644, port_dir,
			   (u32 *)(RTL838X_SW_BASE + RTL838X_VLAN_PORT_TAG_STS_CTRL(port)));

	port_ctrl_regset = devm_kzalloc(priv->dev, sizeof(*port_ctrl_regset), GFP_KERNEL);
	if (!port_ctrl_regset)
		return -ENOMEM;

	port_ctrl_regset->regs = port_ctrl_regs;
	port_ctrl_regset->nregs = ARRAY_SIZE(port_ctrl_regs);
	port_ctrl_regset->base = RTL838X_SW_BASE + (port << 2);
	debugfs_create_regset32("port_ctrl", 0400, port_dir, port_ctrl_regset);

	return 0;
}

void rtl838x_dbgfs_init(struct rtl838x_switch_priv *priv)
{
	struct dentry *rtl838x_dir;
	struct dentry *port_dir;
	struct debugfs_regset32 *port_ctrl_regset;
	int ret, i;

	rtl838x_dir = debugfs_lookup(RTL838X_DRIVER_NAME, NULL);
	if (!rtl838x_dir)
		rtl838x_dir = debugfs_create_dir(RTL838X_DRIVER_NAME, NULL);

	priv->dbgfs_dir = rtl838x_dir;

	debugfs_create_u32("soc", 0444, rtl838x_dir,
			   (u32 *)(RTL838X_SW_BASE + RTL838X_MODEL_NAME_INFO));

	/* Create one directory per port */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy) {
			pr_debug("debugfs, port %d\n", i);
			ret = rtl838x_dbgfs_port_init(rtl838x_dir, priv, i);
			if (ret)
				goto err;
		}
	}

	/* Create directory for CPU-port */
	port_dir = debugfs_create_dir("cpu_port", rtl838x_dir);	port_ctrl_regset = devm_kzalloc(priv->dev, sizeof(*port_ctrl_regset), GFP_KERNEL);
	if (!port_ctrl_regset) {
		ret = -ENOMEM;
		goto err;
	}

	port_ctrl_regset->regs = port_ctrl_regs;
	port_ctrl_regset->nregs = ARRAY_SIZE(port_ctrl_regs);
	port_ctrl_regset->base = RTL838X_SW_BASE + (priv->cpu_port << 2);
	debugfs_create_regset32("port_ctrl", 0400, port_dir, port_ctrl_regset);
	debugfs_create_u8("id", 0444, port_dir, &priv->cpu_port);

	return;
err:
	rtl838x_dbgfs_cleanup(priv);
}
