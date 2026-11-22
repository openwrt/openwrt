/*
* Description
*
* Copyright (C) 2016-2022 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/of_platform.h>
#include <linux/mfd/syscon.h>
#include "init.h"
#include "ivlan_se.h"
#include "evlan_se.h"
#include "vport_se.h"

#include <linux/list_sort.h>

#include <linux/spinlock.h>
#include <linux/hashtable.h>
#include <linux/crc32.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/if_vlan.h>
#include <linux/if_bridge.h>
#include <linux/of_mdio.h>
#include <net/neighbour.h>
#include <net/switchdev.h>
#include <net/ip_fib.h>
#include <net/nexthop.h>
#include <net/arp.h>

extern struct dpns_nat_subnet_info sf_wan_subnet[8];

void default_vlan_init(VLAN_t * priv)
{
	int i;

	/* enable iport/ivlkp/ivxlt table
	 * disable iport speed limit
	 * set ivlkp/ivxlt valid entry to 64
	 * */
	reg_update(priv, CONFIG0_RGT_ADDR, CONFIG0_IVLKP_CFG_DIS_TB
			| CONFIG0_IVXLT_CFG_DIS_TB | CONFIG0_IPSPL_ZERO_LIMIT,
			CONFIG0_IPORT_EN | L2_VID_ZERO_MODE_EN
			| FIELD_PREP(CONFIG0_IVLKP_CFG_ENTR_VLD, 0x3f)
			| FIELD_PREP(CONFIG0_IVXLT_CFG_ENTR_VLD, 0x3f));

	/* enable evlkp/evxlt/act table
	 * set evlkp/evxlt valid entry to 64
	 * */
	reg_update(priv, CONFIG2_RGT_ADDR, CONFIG2_EVLKP_CFG_DIS_TB
			| CONFIG2_EVXLT_CFG_DIS_TB, CONFIG2_EVACT_EN
			| CONFIG2_EVLAN_OTPID_EN | CONFIG2_EVLAN_PTPID_EN
			| FIELD_PREP(CONFIG2_EVLKP_CFG_ENTR_VLD, 0x3f)
			| FIELD_PREP(CONFIG2_EVXLT_CFG_ENTR_VLD, 0x3f));

	/* disable vid 0 search
	 * set port filtering mode to 2; means mcast forwad by mac table,
	 * unknown mcast will drop
	 * */
	reg_update(priv, IVLAN_LKP_MPP_CFG1, IVLAN_LKP_CFG_OVID0_EN,
			FIELD_PREP(IVLAN_LKP_CFG_PFM_MODE, 2)
			| IVLAN_LKP_CFG_DA_MISS_UP);

	/* enable l2 vlan action replace 1;
	 * enable l2/l3 vid0 del;
	 * means when vid is replaced by 0, delete vlan tag;
	 * including ovid and ivid
	 * */
	reg_update(priv, EVLAN_ACT_CFG3, 0, FIELD_PREP(L2_FWD_OVID_CFG, 5)
			| FIELD_PREP(L2_FWD_IVID_CFG, 2)
			| L3_FWD_OVID0_DEL | L3_FWD_IVID0_DEL);

	for (i = 0; i < DPNS_MAX_PORT; i++) {
		iport_table_update(priv, i, DPNS_HOST_PORT,
				PACTION_FWD, 1);
	}

	for (i = 0; i < EXTDEV_OFFSET; i++) {
		evlan_ptpid_table_update(priv, i, ETH_P_8021Q, ETH_P_8021Q);
		evlan_otpid_table_update(priv, i, ETH_P_8021Q);
	}

	/* tmu_ivport_map and modify_vport_map
	 * init first but not enable
	 * only enable when setting vport
	 * */
	tmu_ivport_map_init(priv);
	for (i = 0; i < DPNS_MAX_PORT; i++) {
		modify_vport_map_update(priv, i, i);
	}

	/* When a port is assigned to two or more VLANs,
	 * the 'evlan_xlt' table is not configured,
	 * it directly follows the replacement operation
	 * in the 'evlan_act' table,
	 * with the 'pkt_untag_ptr' register set to 56.
	 * */

	evlan_act_table_update(priv, 56, EVACT_REPLACE1, EVACT_REPLACE2,
			EVACT_REPLACE1, EVACT_REPLACE1, EVACT_NONE);
	evlan_act_table_update(priv, 2, EVACT_REPLACE1, EVACT_REPLACE2,
			EVACT_NONE, EVACT_NONE, EVACT_NONE);
	evlan_act_table_update(priv, 1, EVACT_DEL, EVACT_DEL,
			EVACT_DEL, EVACT_DEL, EVACT_DEL);
	evlan_act_table_update(priv, 0, EVACT_REPLACE2, EVACT_REPLACE2,
			EVACT_NONE, EVACT_NONE, EVACT_NONE);

	memset(&priv->ivlan_index_bitmap, 0, sizeof(priv->ivlan_index_bitmap));
	memset(&priv->evlan_index_bitmap, 0, sizeof(priv->evlan_index_bitmap));
	memset(&priv->vport_index_bitmap, 0, sizeof(priv->vport_index_bitmap));
	memset(&priv->wan_ports_bitmap, 0, sizeof(priv->wan_ports_bitmap));
	memset(&priv->phy_ports_bitmap, 0xff, sizeof(priv->phy_ports_bitmap));
	__set_bit(0, priv->evlan_index_bitmap);
}

void dynamic_vlan_update(VLAN_t *priv)
{
	struct sf_vlan_tbl_entry *pos;
	int count = 0;
	int port;

	for (port = 0; port < DPNS_MAX_PORT; port++) {
		if ((*priv->wan_ports_bitmap & BIT(port)) && (*priv->phy_ports_bitmap & BIT(port)))
			continue;

		spin_lock_bh(&priv->vlan_lock);
		list_for_each_entry(pos, &priv->vlan_list, node) {
			if ((pos->vlan_ports & BIT(port)) != 0) {
				count++;
				break;
			}
		}

		if (count == PORT_WITH_NO_VLAN) {
			evlan_xlt_table_update(priv, port, port, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0);
		} else {
			evlan_xlt_table_update(priv, port, port, 1, 0, 0, 0, 0, 1, 1, 2, 0, 0);
		}
		spin_unlock_bh(&priv->vlan_lock);

		count = 0;
	}

	evlan_lkp_table_update(priv, 0, 0, 1, 0, 0x7ffffff);

	spin_lock_bh(&priv->vlan_lock);
	list_for_each_entry(pos, &priv->vlan_list, node) {
		VLAN_DBG(DBG_LV, "%s ilkp_idx:%u elkp_idx:%u vid:%u ports:0x%x \n",
			 __func__,pos->ivlan_lkp_index, pos->evlan_lkp_index,
			 pos->vlan_id, priv->member_ports);

		ivlan_lkp_table_update(priv, pos->ivlan_lkp_index, pos->vlan_id,
					1, 0, 1, priv->member_ports);
		evlan_lkp_table_update(priv, pos->evlan_lkp_index, pos->vlan_id,
					1, 0, priv->member_ports);
	}
	spin_unlock_bh(&priv->vlan_lock);
}

static int add_sf_vlan_tbl_entry(VLAN_t *priv, int vlan_id, int port, u16 flags)
{
	struct sf_vlan_tbl_entry *vlan;
	unsigned long ivlan_lkp_index;
	unsigned long evlan_lkp_index;
	int found = 0;

	if (flags == 1) {
		priv->member_ports |= BIT(port);
		goto update;
	}

	spin_lock_bh(&priv->vlan_lock);
	list_for_each_entry(vlan, &priv->vlan_list, node) {
		if (vlan->vlan_id != vlan_id)
			continue;

		if (BIT(port) & vlan->vlan_ports) {
			spin_unlock_bh(&priv->vlan_lock);
			return 0;
		}

		priv->member_ports |= BIT(port);
		vlan->vlan_ports |= BIT(port);
		VLAN_DBG(DBG_LV, "%s vid:%u vlan_ports:0x%u\n", __func__,
			 vlan_id, vlan->vlan_ports);
		found = 1;
	}
	spin_unlock_bh(&priv->vlan_lock);

	if (!found) {
		VLAN_DBG(DBG_LV, "%s ivlan_index_bitmap:%lx\n", __func__, *priv->ivlan_index_bitmap);
		VLAN_DBG(DBG_LV, "%s evlan_index_bitmap:%lx\n", __func__, *priv->evlan_index_bitmap);
		ivlan_lkp_index = find_first_zero_bit(priv->ivlan_index_bitmap, SF_IVLAN_LKP_TAB_MAX);
		evlan_lkp_index = find_first_zero_bit(priv->evlan_index_bitmap, SF_EVLAN_LKP_TAB_MAX);

		if (ivlan_lkp_index == SF_IVLAN_LKP_TAB_MAX || evlan_lkp_index == SF_EVLAN_LKP_TAB_MAX) {
			VLAN_DBG(WARN_LV, "table entries are full\n");
			return -1;
		}

		vlan = vlan_kzalloc(sizeof(struct sf_vlan_tbl_entry), GFP_ATOMIC);
		if (vlan == NULL)
			return -ENOMEM;

		priv->member_ports |= BIT(port);
		vlan->vlan_ports |= BIT(port);
		vlan->vlan_id = vlan_id;
		vlan->ivlan_lkp_index = ivlan_lkp_index;
		vlan->evlan_lkp_index = evlan_lkp_index;

		__set_bit(vlan->ivlan_lkp_index, priv->ivlan_index_bitmap);
		__set_bit(vlan->evlan_lkp_index, priv->evlan_index_bitmap);

		spin_lock_bh(&priv->vlan_lock);
		list_add(&vlan->node, &priv->vlan_list);
		spin_unlock_bh(&priv->vlan_lock);
	}

update:
	dynamic_vlan_update(priv);

	return 0;
}

static int del_sf_vlan_tbl_entry(VLAN_t *priv, int vlan_id, int port, u16 flags)
{
	struct sf_vlan_tbl_entry *vlan, *tmp;

	if (flags == 1) {
		priv->member_ports &= ~(BIT(port));
	} else {
		spin_lock_bh(&priv->vlan_lock);
		list_for_each_entry_safe(vlan, tmp, &priv->vlan_list, node) {
			if (vlan->vlan_id != vlan_id)
				continue;

			vlan->vlan_ports &= ~(BIT(port));

			if (vlan->vlan_ports == 0) {
				ivlan_lkp_table_update(priv, vlan->ivlan_lkp_index, 0, 0, 0, 0, 0x0);
				evlan_lkp_table_update(priv, vlan->evlan_lkp_index, 0, 0, 0x0, 0x0);

				__clear_bit(vlan->ivlan_lkp_index, priv->ivlan_index_bitmap);
				__clear_bit(vlan->evlan_lkp_index, priv->evlan_index_bitmap);

				list_del(&vlan->node);

				vlan_kfree(vlan);
			}
		}
		spin_unlock_bh(&priv->vlan_lock);
	}

	dynamic_vlan_update(priv);
	return 0;
}

static int dpns_port_add(VLAN_t *priv, u16 vlan_id, int port, int flag)
{
	int err;

	VLAN_DBG(INFO_LV, "%s vid:%d port :%d\n",
			__func__, vlan_id, port);

	err = add_sf_vlan_tbl_entry(priv, vlan_id, port, flag);

	return err;
}

static int dpns_port_del(VLAN_t *priv, u16 vlan_id, int port, int flag)
{
	int err;

	VLAN_DBG(INFO_LV, "%s vid:%d port :%d\n",
			__func__, vlan_id, port);

	err = del_sf_vlan_tbl_entry(priv, vlan_id, port, flag);

	return err;
}

int vport_dma_ndevs_add(VLAN_t *priv, struct net_device *dev, dpns_port_t *dp_port)
{
	COMMON_t *cpriv = priv->cpriv;
	struct xgmac_dma_priv *dma_priv = (struct xgmac_dma_priv *)cpriv->edma_priv;
	struct net_device *real_dev = dev;
	struct vlan_vport_entry *pos;
	int vlan_id = 0;
	int port_id = 0;

	if (is_vlan_dev(dev)) {
		real_dev = vlan_dev_real_dev(dev);
		vlan_id = vlan_dev_vlan_id(dev);
	}

	spin_lock_bh(&priv->vport_lock);
		list_for_each_entry(pos, &priv->vport_list, node) {
			if (pos->vlan_id == vlan_id && pos->port == dp_port->port_id) {
				cpriv->ports[pos->vport] = cpriv->ports[pos->port];
				dma_priv->ndevs[pos->vport] = real_dev;
				port_id = pos->vport;
				break;
			}
		}
	spin_unlock_bh(&priv->vport_lock);

	return port_id;
}

static void dpns_vlan_netdev_event_work_fn(struct work_struct *work)
{
	struct dpns_vlan_netdev_event_work *netdev_work =
		container_of(work, struct dpns_vlan_netdev_event_work, work);
	VLAN_t *priv = netdev_work->priv;
	struct netdev_notifier_changeupper_info *info = &netdev_work->info;
	struct net_device *dev = netdev_work->dev;
	dpns_port_t *dp_port = netdev_work->dp_port;
	u16 vlan_id;
	bool is_wan = false;
	int i;

	VLAN_DBG(DBG_LV, "%s, ifname:%s\n", netdev_cmd_to_name(netdev_work->event), dev->name);

	for(i = 0; i < 8; i++) {
		if (!strncmp(sf_wan_subnet[i].ifname, dev->name, IFNAMSIZ)) {
			is_wan = true;
			break;
		}
	}

	if (dev->dev.parent) {
		if (!is_vlan_dev(dev) && of_phy_is_fixed_link(dev->dev.parent->of_node))
			__clear_bit(dp_port->port_id, priv->phy_ports_bitmap);
	}

	switch (netdev_work->event) {
	case NETDEV_REGISTER:
		if (is_wan && is_vlan_dev(dev)) {
			vlan_id = vlan_dev_vlan_id(dev);
			__set_bit(dp_port->port_id, priv->wan_ports_bitmap);
			evlan_xlt_table_update(priv, dp_port->port_id, dp_port->port_id, 1, 0, 0, 0, vlan_id, 1, 1, 0, 0, 0);
		}
		break;
	case NETDEV_UNREGISTER:
		if (*priv->wan_ports_bitmap & BIT(dp_port->port_id)) {
			__clear_bit(dp_port->port_id, priv->wan_ports_bitmap);
			evlan_xlt_table_update(priv, dp_port->port_id, dp_port->port_id, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0);
		}
		break;
	case NETDEV_UP:
		vport_dma_ndevs_add(priv, dev, dp_port);
		if (is_vlan_dev(dev)) {
			vlan_id = vlan_dev_vlan_id(dev);
			dpns_port_add(priv, vlan_id, dp_port->port_id, 0);
		}
		else {
			dpns_port_add(priv, 0, dp_port->port_id, 1);
		}
		break;
	case NETDEV_DOWN:
		if (is_vlan_dev(dev)) {
			vlan_id = vlan_dev_vlan_id(dev);
			dpns_port_del(priv, vlan_id, dp_port->port_id, 0);
		}
		else {
			dpns_port_del(priv, 0, dp_port->port_id, 1);
		}
		break;
	case NETDEV_CHANGEUPPER:
		if (info->linking) {
			if (is_vlan_dev(dev)) {
				vlan_id = vlan_dev_vlan_id(dev);
				dpns_port_add(priv, vlan_id, dp_port->port_id, 0);
			}
			else {
				dpns_port_add(priv, 0, dp_port->port_id, 1);
			}
		} else {
			if (is_vlan_dev(dev)) {
				vlan_id = vlan_dev_vlan_id(dev);
				dpns_port_del(priv, vlan_id, dp_port->port_id, 0);
			}
			else {
				dpns_port_del(priv, 0, dp_port->port_id, 1);
			}
		break;
		}
	default:
		break;
	}

	vlan_kfree(netdev_work);
}

static int dpns_vlan_netdevice_event(struct notifier_block *unused,
				  unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct dpns_vlan_netdev_event_work *netdev_work;
	VLAN_t *priv = container_of(unused, VLAN_t, netdevice_nb);
	COMMON_t *cpriv = priv->cpriv;
	dpns_port_t *dp_port;

	dp_port = cpriv->port_by_netdev(cpriv, dev);
	if (dp_port == NULL)
		return NOTIFY_DONE;

	netdev_work = vlan_kzalloc(sizeof(*netdev_work), GFP_ATOMIC);
	if (WARN_ON(!netdev_work))
		return NOTIFY_BAD;

	netdev_work->dp_port = vlan_kzalloc(sizeof(dpns_port_t), GFP_ATOMIC);
	if (WARN_ON(!netdev_work->dp_port))
		return NOTIFY_BAD;

	INIT_WORK(&netdev_work->work, dpns_vlan_netdev_event_work_fn);

	netdev_work->priv = priv;
	netdev_work->dev = dev;
	netdev_work->event = event;
	memcpy(netdev_work->dp_port, dp_port, sizeof(dpns_port_t));

	switch (event) {
		case NETDEV_CHANGEUPPER:
			memcpy(&netdev_work->info, ptr, sizeof(netdev_work->info));
			break;
		default:
			break;
	}

	queue_work(priv->owq, &netdev_work->work);

	return NOTIFY_DONE;
}

void dump_vlan_vport_entry(VLAN_t *priv)
{
	struct vlan_vport_entry *pos;

	spin_lock_bh(&priv->vport_lock);
	list_for_each_entry(pos, &priv->vport_list, node) {
		printk("vid:%u\n",pos->vlan_id);
		printk("port:%u\n",pos->port);
		printk("vport:%u\n",pos->vport);
		printk("vport_index:%u\n",pos->vport_index);
	}
	spin_unlock_bh(&priv->vport_lock);
}

static int add_vlan_vport_entry(VLAN_t *priv, u16 vid, u8 port, u8 vport)
{
	struct vlan_vport_entry *pos;
	unsigned long vport_index;

	spin_lock_bh(&priv->vport_lock);
	list_for_each_entry(pos, &priv->vport_list, node) {
		if ((pos->vlan_id == vid && pos->port == port) || pos->vport == vport) {
			VLAN_DBG(WARN_LV, "exists vid:%d, port:%d mapping vport:%d\n", pos->vlan_id, pos->port, pos->vport);
			VLAN_DBG(WARN_LV, "del first and try to add again\n");
			spin_unlock_bh(&priv->vport_lock);
			return -1;
		}
	}
	spin_unlock_bh(&priv->vport_lock);

	VLAN_DBG(DBG_LV, "%s vport_index:%lx\n", __func__, *priv->vport_index_bitmap);
	vport_index = find_first_zero_bit(priv->vport_index_bitmap, SF_VLAN_VPORT_MAP_MAX);

	if (vport_index == SF_VLAN_VPORT_MAP_MAX) {
		VLAN_DBG(WARN_LV, "vlan_vport_table entries are full\n");
		return -1;
	}

	pos = vlan_kzalloc(sizeof(struct vlan_vport_entry), GFP_ATOMIC);
	if (pos == NULL)
		return -ENOMEM;

	pos->vlan_id = vid;
	pos->port = port;
	pos->vport = vport;
	pos->vport_index = vport_index;

	__set_bit(pos->vport_index, priv->vport_index_bitmap);

	spin_lock_bh(&priv->vport_lock);
	list_add(&pos->node, &priv->vport_list);
	spin_unlock_bh(&priv->vport_lock);

	dump_vlan_vport_entry(priv);

	return vport_index;
}

static int del_vlan_vport_entry(VLAN_t *priv, u16 vid, u8 port)
{
	struct vlan_vport_entry *pos, *tmp;
	int vport_index = -1;

	spin_lock_bh(&priv->vport_lock);
	list_for_each_entry_safe(pos, tmp, &priv->vport_list, node) {
		if (pos->vlan_id == vid && pos->port == port) {
			vport_index = pos->vport_index;
			__clear_bit(pos->vport_index, priv->vport_index_bitmap);
			list_del(&pos->node);
			vlan_kfree(pos);
		}
	}
	spin_unlock_bh(&priv->vport_lock);

	dump_vlan_vport_entry(priv);

	return vport_index;
}

int ivlan_evlan_update(VLAN_t *priv, u16 vid, u8 vport, bool update)
{
	int err = 0;
	int is_vid_zero = 1;

	if (vid)
		is_vid_zero = 0;

	if (update) {
		err = add_sf_vlan_tbl_entry(priv, vid, vport, is_vid_zero);
		evlan_ptpid_table_update(priv, vport, ETH_P_8021Q, ETH_P_8021Q);
		evlan_otpid_table_update(priv, vport, ETH_P_8021Q);
	}
	else {
		err = del_sf_vlan_tbl_entry(priv, vid, vport, is_vid_zero);
		evlan_ptpid_table_update(priv, vport, 0, 0);
		evlan_otpid_table_update(priv, vport, 0);
	}

	return err;
}

int vport_update(VLAN_t *priv, u16 vid, u8 port, u8 vport)
{
	unsigned long first_empty_map;
	int err = 0;
	u8 index;
	u8 vport_idx;

	if (port > REAL_PORT_NUM || vport > MAX_PORT_NUM ) {
		VLAN_DBG(WARN_LV, "PORT IS OUT OF RANGE \n" "port is under 6, vport is under 27\n");
		return -1;
	}

	err = add_vlan_vport_entry(priv, vid, port, vport);
	if (err < 0)
		return -1;

	first_empty_map = err;

	vlan_vport_map_write(priv, first_empty_map, vid, port, vport, 1);

	modify_vport_map_en(priv, 1);
	modify_vport_map_update(priv, port, vport);

	tmu_ivport_map_en(priv, 1);//init first enable later

	index = vport / 6;
	vport_idx = vport % 6;

	tmu_ivport_map_update(priv, index, vport_idx, port);//port for update

	ivlan_evlan_update(priv, vid, vport, 1);

	return err;
}

int vport_reset(VLAN_t *priv, u16 vid, u8 port, u8 vport)
{
	int map_index;
	u8 index;
	u8 vport_idx;

	if (port > REAL_PORT_NUM || vport > MAX_PORT_NUM ) {
		VLAN_DBG(WARN_LV, "PORT IS OUT OF RANGE \n" "port is under 6, vport is under 27\n");
		return -1;
	}

	map_index = del_vlan_vport_entry(priv, vid, port);
	if (map_index < 0) {
		VLAN_DBG(WARN_LV, "no vid + port \n");
		return -1;
	}

	vlan_vport_map_write(priv, map_index, 0, 0, 0, 0);

	modify_vport_map_en(priv, 0);
	modify_vport_map_reset(priv, port, vport);

	index = vport / 6;
	vport_idx = vport % 6;

	tmu_ivport_map_update(priv, index, vport_idx, vport);//vport for reset
	tmu_ivport_map_en(priv, 0);

	ivlan_evlan_update(priv, vid, vport, 0);

	return 0;
}

static void sf_destroy_vlanlist(VLAN_t *priv)
{
	struct sf_vlan_tbl_entry *pos, *tmp;

	spin_lock_bh(&priv->vlan_lock);
	list_for_each_entry_safe(pos, tmp, &priv->vlan_list, node) {
		list_del(&pos->node);
		vlan_kfree(pos);
	}
	sf_writel(priv, CLR_CTRL_RAM_ADDR, IVLAN_LKP_CLEAR|EVLAN_LKP_CLEAR);
	spin_unlock_bh(&priv->vlan_lock);
}

static void sf_destroy_vlanvport(VLAN_t *priv)
{
	struct vlan_vport_entry *pos, *tmp;

	spin_lock_bh(&priv->vport_lock);
	list_for_each_entry_safe(pos, tmp, &priv->vport_list, node) {
		list_del(&pos->node);
		vlan_kfree(pos);
	}
	spin_unlock_bh(&priv->vport_lock);
}

int dpns_vlan_probe(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	VLAN_t* priv = NULL;
	int err;

	priv = devm_kzalloc(&pdev->dev, sizeof(VLAN_t), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	priv->owq = alloc_ordered_workqueue("dpns_vlan", WQ_MEM_RECLAIM);
	if (!priv->owq)
		return -ENOMEM;

	common_priv->vlan_priv = priv;
	priv->cpriv = common_priv;
	priv->iobase = common_priv->iobase;

	spin_lock_init(&priv->vlan_lock);
	spin_lock_init(&priv->vport_lock);
	INIT_LIST_HEAD(&priv->vlan_list);
	INIT_LIST_HEAD(&priv->vport_list);

	priv->netdevice_nb.notifier_call = dpns_vlan_netdevice_event;

	err = register_netdevice_notifier(&priv->netdevice_nb);
	if (err) {
		dev_err(&pdev->dev, "Failed to register netdevice notifier\n");
		goto err_register_netdevice_notifier;
	}

	dpns_vlan_genl_init(priv);
	dpns_vlan_proc_init(priv);

	/** ivlan and evlan default map */
	default_vlan_init(priv);

	printk("End %s\n", __func__);
	return 0;

err_register_netdevice_notifier:
	sf_destroy_vlanlist(priv);
	sf_destroy_vlanvport(priv);
	destroy_workqueue(priv->owq);
	return err;
}
EXPORT_SYMBOL(dpns_vlan_probe);

void dpns_vlan_remove(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	VLAN_t* priv = common_priv->vlan_priv;

	dpns_vlan_genl_exit();
	dpns_vlan_proc_exit();
	unregister_netdevice_notifier(&priv->netdevice_nb);

	sf_destroy_vlanlist(priv);
	sf_destroy_vlanvport(priv);
	destroy_workqueue(priv->owq);

	common_priv->vlan_priv = NULL;
	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_vlan_remove);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Qin Xia <qin.xia@siflower.com.cn>");
MODULE_DESCRIPTION("DPNS Vlan Driver");
