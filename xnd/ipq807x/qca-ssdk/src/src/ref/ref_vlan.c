/*
 * Copyright (c) 2012, 2014, 2017, The Linux Foundation. All rights reserved.
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

#include "sw.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include "fal_misc.h"
#include "fal_mib.h"
#include "fal_port_ctrl.h"
#include "fal_portvlan.h"
#include "fal_fdb.h"
#include "fal_stp.h"
#include "fal_igmp.h"
#include "fal_qos.h"
#include "fal_acl.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "ssdk_init.h"
#include "ssdk_dts.h"
#include "hsl_phy.h"
#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>
//#include <asm/mach-types.h>
#include <generated/autoconf.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0))
#include <linux/ar8216_platform.h>
#endif
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include "ssdk_plat.h"
#include "ref_vlan.h"
#ifdef BOARD_AR71XX
#include "ssdk_uci.h"
#endif


extern ssdk_chip_type SSDK_CURRENT_CHIP_TYPE;

#if !defined(IN_VLAN_MINI)
sw_error_t
qca_lan_wan_cfg_set(a_uint32_t dev_id, qca_lan_wan_cfg_t *lan_wan_cfg)
{
	a_uint32_t i = 0, lan_bmp = 0, wan_bmp = 0;
	sw_error_t rv = SW_OK;
	fal_vlan_t vlan_entry;

	SW_RTN_ON_NULL(lan_wan_cfg);

	switch (SSDK_CURRENT_CHIP_TYPE) {
		case CHIP_ISIS:
		case CHIP_ISISC:
		case CHIP_DESS:
			break;
		default:
			return SW_NOT_SUPPORTED;
	}

	fal_vlan_flush(dev_id);
	aos_mem_set(&vlan_entry, 0, sizeof(vlan_entry));

	if (lan_wan_cfg->lan_only_mode) {
#if defined(IN_PORTVLAN)
		while (i < sizeof(lan_wan_cfg->v_port_info)/sizeof(lan_wan_cfg->v_port_info[0])) {
			if (lan_wan_cfg->v_port_info[i].valid) {
				/* use the portbased vlan table for forwarding */
				rv = fal_port_1qmode_set(dev_id,
						lan_wan_cfg->v_port_info[i].port_id,
						FAL_1Q_DISABLE);
				SW_RTN_ON_ERROR(rv);
				rv = fal_port_egvlanmode_set(dev_id,
						lan_wan_cfg->v_port_info[i].port_id,
						FAL_EG_UNMODIFIED);
				SW_RTN_ON_ERROR(rv);
				rv = fal_port_default_cvid_set(dev_id,
						lan_wan_cfg->v_port_info[i].port_id,
						0);
				SW_RTN_ON_ERROR(rv);
				lan_bmp |= (0x1 << lan_wan_cfg->v_port_info[i].port_id);
			}
			i++;
		}
		/* CPU port 0 configurations */
		rv = fal_port_1qmode_set(dev_id, SSDK_PORT_CPU, FAL_1Q_DISABLE);
		SW_RTN_ON_ERROR(rv);
		rv = fal_port_egvlanmode_set(dev_id, SSDK_PORT_CPU, FAL_EG_UNMODIFIED);
		SW_RTN_ON_ERROR(rv);
		rv = fal_port_default_cvid_set(dev_id, SSDK_PORT_CPU, 0);
		SW_RTN_ON_ERROR(rv);
		rv = fal_portvlan_member_update(dev_id, SSDK_PORT_CPU, lan_bmp);
		SW_RTN_ON_ERROR(rv);
#endif
	} else {
		while (i < sizeof(lan_wan_cfg->v_port_info)/sizeof(lan_wan_cfg->v_port_info[0])) {
			if (lan_wan_cfg->v_port_info[i].valid) {
				rv = fal_vlan_find(dev_id,
						lan_wan_cfg->v_port_info[i].vid, &vlan_entry);
				/* create vlan entry if the vlan entry does not exist */
				if (rv == SW_NOT_FOUND) {
					rv = fal_vlan_create(dev_id,
							lan_wan_cfg->v_port_info[i].vid);
					SW_RTN_ON_ERROR(rv);
#if defined(IN_PORTVLAN)
					rv = fal_port_1qmode_set(dev_id,
							SSDK_PORT_CPU, FAL_1Q_SECURE);
					SW_RTN_ON_ERROR(rv);
#endif
					rv = fal_vlan_member_add(dev_id,
							lan_wan_cfg->v_port_info[i].vid,
							SSDK_PORT_CPU, FAL_EG_TAGGED);
					SW_RTN_ON_ERROR(rv);
				}
				rv = fal_vlan_member_add(dev_id,
						lan_wan_cfg->v_port_info[i].vid,
						lan_wan_cfg->v_port_info[i].port_id,
						FAL_EG_UNTAGGED);
				SW_RTN_ON_ERROR(rv);
#if defined(IN_PORTVLAN)
				rv = fal_port_1qmode_set(dev_id,
						lan_wan_cfg->v_port_info[i].port_id,
						FAL_1Q_SECURE);
				SW_RTN_ON_ERROR(rv);

				rv = fal_port_default_cvid_set(dev_id,
						lan_wan_cfg->v_port_info[i].port_id,
						lan_wan_cfg->v_port_info[i].vid);
				SW_RTN_ON_ERROR(rv);
#endif
				if (lan_wan_cfg->v_port_info[i].is_wan_port) {
					wan_bmp |= (0x1 << lan_wan_cfg->v_port_info[i].port_id);
				} else {
					lan_bmp |= (0x1 << lan_wan_cfg->v_port_info[i].port_id);
				}
			}
			i++;
		}
	}
	ssdk_lan_bmp_set(dev_id, lan_bmp);
	ssdk_wan_bmp_set(dev_id, wan_bmp);
	qca_ssdk_port_bmp_set(dev_id, lan_bmp|wan_bmp);
#if defined(DESS) && defined(IN_TRUNK)
	if(SSDK_CURRENT_CHIP_TYPE == CHIP_DESS) {
		ssdk_dess_trunk_init(dev_id, wan_bmp);
	}
#endif
#if defined(IN_PORTVLAN)
	ssdk_portvlan_init(dev_id);
#endif
	return rv;
}

sw_error_t
qca_lan_wan_cfg_get(a_uint32_t dev_id, qca_lan_wan_cfg_t *lan_wan_cfg)
{
	sw_error_t rv = SW_OK;
	fal_vlan_t vlan_entry;
	fal_pbmp_t member_pmap, lan_bmp, wan_bmp;
	a_uint32_t port_id, entry_id, vlan_id;

	SW_RTN_ON_NULL(lan_wan_cfg);

	switch (SSDK_CURRENT_CHIP_TYPE) {
		case CHIP_ISIS:
		case CHIP_ISISC:
		case CHIP_DESS:
			break;
		default:
			return SW_NOT_SUPPORTED;
	}

	lan_bmp = ssdk_lan_bmp_get(dev_id);
	wan_bmp = ssdk_wan_bmp_get(dev_id);

	member_pmap = lan_bmp | wan_bmp;
	vlan_id = FAL_NEXT_ENTRY_FIRST_ID;
	entry_id = 0;

	while (1) {
		aos_mem_set(&vlan_entry, 0, sizeof(vlan_entry));
		rv = fal_vlan_next(dev_id, vlan_id, &vlan_entry);
		if (rv != SW_OK) {
			break;
		}

		/*
		 * the special port id should be existed only in one vlan entry
		 * starting from port 1.
		 */
		port_id = 1;
		while (vlan_entry.mem_ports >> port_id) {
			if (((vlan_entry.mem_ports >> port_id) & 1) &&
					SW_IS_PBMP_MEMBER(member_pmap, port_id)) {
				lan_wan_cfg->v_port_info[entry_id].port_id = port_id;
				lan_wan_cfg->v_port_info[entry_id].vid = vlan_entry.vid;
				lan_wan_cfg->v_port_info[entry_id].valid = A_TRUE;
				lan_wan_cfg->v_port_info[entry_id].is_wan_port =
					SW_IS_PBMP_MEMBER(wan_bmp, port_id) ? A_TRUE : A_FALSE;
				entry_id++;
			}
			port_id++;
		}
		vlan_id = vlan_entry.vid;
	}

	/*
	 * no vlan entry exists, the portbased vlan used.
	 */
#if defined(IN_PORTVLAN)
	if (entry_id == 0) {
		lan_wan_cfg->lan_only_mode = A_TRUE;
		port_id = 1;
		while (lan_bmp >> port_id) {
			if ((lan_bmp >> port_id) & 1) {
				lan_wan_cfg->v_port_info[entry_id].port_id = port_id;
				lan_wan_cfg->v_port_info[entry_id].vid = 0;
				lan_wan_cfg->v_port_info[entry_id].is_wan_port = A_FALSE;

				member_pmap = 0;
#if !defined(IN_PORTVLAN_MINI)
				fal_portvlan_member_get(dev_id, port_id, &member_pmap);
#endif
				lan_wan_cfg->v_port_info[entry_id].valid =
					member_pmap ? A_TRUE : A_FALSE;
				entry_id++;
			}
			port_id++;
		}
	}
#endif
	return SW_OK;
}
#endif

int
qca_ar8327_sw_enable_vlan0(a_uint32_t dev_id, a_bool_t enable, a_uint8_t portmap)
{
    fal_vlan_t entry;
    fal_acl_rule_t rule;
    int i = 0;

    memset(&entry, 0, sizeof(fal_vlan_t));
    memset(&rule, 0, sizeof(fal_acl_rule_t));
    for (i = 0; i < AR8327_NUM_PORTS; i ++) {
        fal_port_tls_set(dev_id, i, A_FALSE);
        fal_port_vlan_propagation_set(dev_id, i, FAL_VLAN_PROPAGATION_REPLACE);
    }

    if (enable) {
        entry.fid = 0;
        entry.mem_ports = portmap;
        entry.unmodify_ports = portmap;
        entry.vid = 0;
        fal_vlan_entry_append(dev_id, &entry);
        for (i = 0; i < AR8327_NUM_PORTS; i++) {
            if (portmap & (0x1 << i)) {
                fal_port_egvlanmode_set(dev_id, i, FAL_EG_UNTOUCHED);
                fal_port_tls_set(dev_id, i, A_TRUE);
                fal_port_vlan_propagation_set(dev_id, i, FAL_VLAN_PROPAGATION_DISABLE);
                fal_acl_port_udf_profile_set(dev_id, i, FAL_ACL_UDF_TYPE_L2, 12, 4);
            }
        }

        fal_acl_list_creat(dev_id, 0, 0);
        rule.rule_type = FAL_ACL_RULE_UDF;
        rule.udf_len = 4;
        rule.udf_val[0] = 0x81;
        rule.udf_val[1] = 0;
        rule.udf_val[2] = 0;
        rule.udf_val[3] = 0;
        rule.udf_mask[0] = 0xff;
        rule.udf_mask[1] = 0xff;
        rule.udf_mask[2] = 0xf;
        rule.udf_mask[3] = 0xff;
        FAL_FIELD_FLG_SET(rule.field_flg, FAL_ACL_FIELD_UDF);
        FAL_ACTION_FLG_SET(rule.action_flg, FAL_ACL_ACTION_REMARK_LOOKUP_VID);
        fal_acl_rule_add(dev_id, 0, 0, 1, &rule);
        for (i = 0; i < AR8327_NUM_PORTS; i ++) {
            fal_acl_list_unbind(dev_id, 0, 0, 0, i);
            if (portmap & (0x1 << i)) {
                fal_acl_list_bind(dev_id, 0, 0, 0, i);
            }
        }
        fal_acl_status_set(dev_id, A_TRUE);
    }
    else {
        fal_acl_rule_delete(dev_id, 0, 0, 1);
    }

    return 0;
}

#if defined(IN_SWCONFIG)
int
qca_ar8327_sw_set_vlan(struct switch_dev *dev,
                       const struct switch_attr *attr,
                       struct switch_val *val)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);

    priv->vlan = !!val->value.i;

    #ifdef BOARD_AR71XX
    if(SSDK_CURRENT_CHIP_TYPE == CHIP_SHIVA) {
		ssdk_uci_sw_set_vlan(attr, val);
    }
    #endif

    return 0;
}

int
qca_ar8327_sw_get_vlan(struct switch_dev *dev,
                       const struct switch_attr *attr,
                       struct switch_val *val)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);

    val->value.i = priv->vlan;

    return 0;
}

int
qca_ar8327_sw_set_vid(struct switch_dev *dev,
                      const struct switch_attr *attr,
                      struct switch_val *val)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);

    priv->vlan_id[val->port_vlan] = val->value.i;

#ifdef BOARD_AR71XX
    if(SSDK_CURRENT_CHIP_TYPE == CHIP_SHIVA) {
		ssdk_uci_sw_set_vid(attr, val);
    }
#endif

    return 0;
}

int
qca_ar8327_sw_get_vid(struct switch_dev *dev,
                      const struct switch_attr *attr,
                      struct switch_val *val)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);

    val->value.i = priv->vlan_id[val->port_vlan];

    return 0;
}

int
qca_ar8327_sw_get_pvid(struct switch_dev *dev, int port, int *vlan)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);

    *vlan = priv->pvid[port];

    return 0;
}

int
qca_ar8327_sw_set_pvid(struct switch_dev *dev, int port, int vlan)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);

    /* make sure no invalid PVIDs get set */
    if (vlan >= dev->vlans)
        return -1;

    priv->pvid[port] = vlan;

#ifdef BOARD_AR71XX
		if(SSDK_CURRENT_CHIP_TYPE == CHIP_SHIVA) {
			ssdk_uci_sw_set_pvid(port, vlan);
		}
#endif

    return 0;
}

int
qca_ar8327_sw_get_ports(struct switch_dev *dev, struct switch_val *val)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);
    a_uint8_t ports = priv->vlan_table[val->port_vlan];
    int i;

    val->len = 0;
    for (i = 0; i < dev->ports; i++) {
        struct switch_port *p;

        if (!(ports & (1 << i)))
            continue;

        p = &val->value.ports[val->len++];
        p->id = i;
        if (priv->vlan_tagged[val->port_vlan] & (1 << i))
            p->flags = (1 << SWITCH_PORT_FLAG_TAGGED);
        else
            p->flags = 0;

        /*Handle for VLAN 0*/
        if (val->port_vlan == 0)
            p->flags = (1 << SWITCH_PORT_FLAG_TAGGED);
    }

    return 0;
}

int
qca_ar8327_sw_set_ports(struct switch_dev *dev, struct switch_val *val)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);
    a_uint8_t *vt = &priv->vlan_table[val->port_vlan];
    int i;

#ifdef BOARD_AR71XX
	if(SSDK_CURRENT_CHIP_TYPE == CHIP_SHIVA) {
		ssdk_uci_sw_set_ports(val);
	}
#endif

    /*Handle for VLAN 0*/
    if (val->port_vlan == 0) {
        priv->vlan_table[0] = 0;
        for (i = 0; i < val->len; i++) {
            struct switch_port *p = &val->value.ports[i];
            priv->vlan_table[0] |= (1 << p->id);
        }

        return 0;
    }
	if (priv->vlan_id[val->port_vlan] == 0)
		priv->vlan_id[val->port_vlan] = val->port_vlan;
    *vt = 0;
    for (i = 0; i < val->len; i++) {
        struct switch_port *p = &val->value.ports[i];

        if (p->flags & (1 << SWITCH_PORT_FLAG_TAGGED)) {
            priv->vlan_tagged[val->port_vlan] |= (1 << p->id);
        } else {
            priv->vlan_tagged[val->port_vlan] &= ~(1 << p->id);
            priv->pvid[p->id] = val->port_vlan;
        }

        *vt |= 1 << p->id;
    }

    return 0;
}

int
qca_ar8327_sw_hw_apply(struct switch_dev *dev)
{
    struct qca_phy_priv *priv = qca_phy_priv_get(dev);
    fal_pbmp_t *portmask = NULL;
    int i, j;

    if (priv->version == QCA_VER_HPPE) {
        return 0;
    }

    portmask = aos_mem_alloc(sizeof(fal_pbmp_t) * dev->ports);
    if (portmask == NULL) {
        SSDK_ERROR("%s: portmask malloc failed. \n", __func__);
        return -1;
    }
    memset(portmask, 0, sizeof(fal_pbmp_t) * dev->ports);

    mutex_lock(&priv->reg_mutex);

    if (!priv->init) {
        /*Handle VLAN 0 entry*/
        if (priv->vlan_id[0] == 0 && priv->vlan_table[0] == 0) {
            qca_ar8327_sw_enable_vlan0(priv->device_id, A_FALSE, 0);
        }

        /* calculate the port destination masks and load vlans
         * into the vlan translation unit */
        for (j = 0; j < AR8327_MAX_VLANS; j++) {
            u8 vp = priv->vlan_table[j];

            if (!vp) {
                fal_vlan_delete(priv->device_id, priv->vlan_id[j]);
                continue;
            }
            fal_vlan_delete(priv->device_id, priv->vlan_id[j]);
            fal_vlan_create(priv->device_id, priv->vlan_id[j]);

            for (i = 0; i < dev->ports; i++) {
                u8 mask = (1 << i);
                if (vp & mask) {
                    fal_vlan_member_add(priv->device_id, priv->vlan_id[j], i,
                           (mask & priv->vlan_tagged[j])? FAL_EG_TAGGED : FAL_EG_UNTAGGED);
                    portmask[i] |= vp & ~mask;
                }
            }
	    	if (SSDK_CURRENT_CHIP_TYPE == CHIP_SHIVA)
				fal_vlan_member_update(priv->device_id,priv->vlan_id[j],vp,0);
        }

        /*Hanlde VLAN 0 entry*/
        if (priv->vlan_id[0] == 0 && priv->vlan_table[0]) {
            qca_ar8327_sw_enable_vlan0(priv->device_id,A_TRUE, priv->vlan_table[0]);
        }

    } else {
	    /* vlan disabled: port based vlan used */
	    ssdk_portvlan_init(priv->device_id);
    }

    /* update the port destination mask registers and tag settings */
    for (i = 0; i < dev->ports; i++) {
        int pvid;
        fal_pt_1qmode_t ingressMode;
        fal_pt_1q_egmode_t egressMode;

        if (priv->vlan) {
            pvid = priv->vlan_id[priv->pvid[i]];
            ingressMode = FAL_1Q_SECURE;
        } else {
            pvid = 0;
            ingressMode = FAL_1Q_DISABLE;
        }
        egressMode = FAL_EG_UNTOUCHED;

        fal_port_1qmode_set(priv->device_id, i, ingressMode);
        fal_port_egvlanmode_set(priv->device_id, i, egressMode);
        fal_port_default_cvid_set(priv->device_id, i, pvid);
	if (!priv->init && priv->vlan) {
		fal_portvlan_member_update(priv->device_id, i, portmask[i]);
	}
    }

    aos_mem_free(portmask);
    portmask = NULL;

    mutex_unlock(&priv->reg_mutex);

    return 0;
}
#endif


