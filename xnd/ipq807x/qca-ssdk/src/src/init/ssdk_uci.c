/*
 * Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <generated/autoconf.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/of.h>
 #include <linux/switch.h>
#else
#include <net/switch.h>
#include <linux/ar8216_platform.h>
#endif
#include "sw.h"
#include "ssdk_init.h"
#include "ssdk_plat.h"

#ifdef BOARD_AR71XX

struct switch_dev *old_sw_dev = NULL;
struct net_device	  *sw_attach_dev = NULL;
struct qca_phy_priv qca_priv;

extern int qca_phy_id_chip(struct qca_phy_priv *priv);
extern const struct switch_dev_ops qca_ar8327_sw_ops;
extern int qca_phy_mib_work_start(struct qca_phy_priv *priv);
extern void qca_phy_mib_work_stop(struct qca_phy_priv *priv);

int
ssdk_uci_sw_set_vlan(const struct switch_attr *attr,
                       struct switch_val *val)
{
	if(old_sw_dev) {
		struct switch_dev_ops *ops = old_sw_dev->ops;
		if(ops) {
			struct switch_attrlist attr_global = ops->attr_global;
			int i = 0;
			struct switch_attr *g_attr = attr_global.attr;
			for(i = 0; i < attr_global.n_attr; i++) {
				if(!strcmp(g_attr[i].name, "enable_vlan") &&
					g_attr[i].set) {
					g_attr[i].set(old_sw_dev, attr, val);
					break;
				}
			}
		}
	}
	return 0;
}

int
ssdk_uci_sw_set_vid(const struct switch_attr *attr,
                       struct switch_val *val)
{
	if(old_sw_dev) {
		struct switch_dev_ops *ops = old_sw_dev->ops;
		if(ops) {
			struct switch_attrlist attr_vlan = ops->attr_vlan;
			int i = 0;
			struct switch_attr *v_attr = attr_vlan.attr;
			for(i = 0; i < attr_vlan.n_attr; i++) {
				if(!strcmp(v_attr[i].name, "vid") &&
					v_attr[i].set) {
					v_attr[i].set(old_sw_dev, attr, val);
					break;
				}
			}
		}
	}
	return 0;
}


int
ssdk_uci_sw_set_pvid(int port, int vlan)
{
	if(old_sw_dev) {
		struct switch_dev_ops *ops = old_sw_dev->ops;
		if(ops && ops->set_port_pvid) {
			ops->set_port_pvid(old_sw_dev, port, vlan);
		}
	}
	return 0;
}

int
ssdk_uci_sw_set_ports(struct switch_val *val)
{
	if(old_sw_dev) {
		struct switch_dev_ops *ops = old_sw_dev->ops;
		if(ops && ops->set_vlan_ports) {
			ops->set_vlan_ports(old_sw_dev, val);
		}
	}
	return 0;
}


int ssdk_uci_takeover_init()
{
	struct qca_phy_priv *priv = &qca_priv;
	struct switch_dev tmp_dev;
	struct switch_dev *sdev = NULL;
	struct net_device *tmp_net = NULL;

	/*only for s27*/
	if((qca_ar8216_mii_read(0, 0)&0xff00)>>8 != 0x02)
		return 0;

	memset(&qca_priv, 0, sizeof(qca_priv));
	mutex_init(&priv->reg_mutex);
	qca_phy_id_chip(priv);
	priv->mii_read = qca_ar8216_mii_read;
	priv->mii_write = qca_ar8216_mii_write;
	priv->phy_write = qca_ar8327_phy_write;
	priv->phy_dbg_write = qca_ar8327_phy_dbg_write;
	priv->phy_dbg_read = qca_ar8327_phy_dbg_read;
	priv->phy_mmd_write = qca_ar8327_mmd_write;

	priv->sw_dev.ops = &qca_ar8327_sw_ops;
	priv->sw_dev.name = "QCA AR8327 AR8337";
	priv->sw_dev.vlans = AR8327_MAX_VLANS;
	priv->sw_dev.ports = AR8327_NUM_PORTS;
	memset(&tmp_dev, 0, sizeof(tmp_dev));
	tmp_dev.ops = &qca_ar8327_sw_ops;
	tmp_dev.name = "ssdk probe";
	SSDK_DEBUG("SSDK uci takeover!\n");
	tmp_net = dev_get_by_name(&init_net, "eth1");
	if(!tmp_net) {
		tmp_net = dev_get_by_name(&init_net, "eth0");
		if(!tmp_net)
			return 0;
	}
	SSDK_DEBUG("using %s\n", tmp_net->name);
	if(register_switch(&tmp_dev, tmp_net) < 0) {
		SSDK_ERROR("register temp switch fail!\n");
		return 0;
	}
	list_for_each_entry(sdev, &tmp_dev.dev_list, dev_list) {
		SSDK_DEBUG("Found %s\n", sdev->name);
		if(!strcmp(sdev->name, "AR7240/AR9330 built-in switch") ||
			!strcmp(sdev->name, "AR934X built-in switch")) {
			int err;
			/*found*/
			sw_attach_dev = sdev->netdev;
			old_sw_dev = sdev;
			/*unregister openwrt switch device*/
			unregister_switch(sdev);
			/*register ours*/
			SSDK_DEBUG("ssdk register switch, old name %s\n", sdev->name);
			if(err = register_switch(&priv->sw_dev, sw_attach_dev) < 0) {
				SSDK_ERROR("ssdk register switch fail %d!\n", err);
			}
			break;
		}
	}
	/*anyway should unregister the temp switch dev*/
	unregister_switch(&tmp_dev);
	if(sw_attach_dev) {
		qca_phy_mib_work_start(&qca_priv);
	}
	return 0;
}

void ssdk_uci_takeover_exit()
{
	if(sw_attach_dev && old_sw_dev) {
		unregister_switch(&qca_priv.sw_dev);
		register_switch(old_sw_dev, sw_attach_dev);
		qca_phy_mib_work_stop(&qca_priv);
	}
}

#endif


