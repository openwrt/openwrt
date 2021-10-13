/*
 * Copyright (c) 2012, 2017, The Linux Foundation. All rights reserved.
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
#include "hsl.h"
#include "hsl_dev.h"
#include "ssdk_init.h"
#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>
//#include <asm/mach-types.h>
#include <generated/autoconf.h>
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/switch.h>
#else
#include <net/switch.h>
#include <linux/ar8216_platform.h>
#endif
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include "ssdk_plat.h"
#include "ref_vlan.h"
#ifdef MP
#include <adpt.h>
#include "hsl_phy.h"
#endif

int
qca_ar8327_sw_set_max_frame_size(struct switch_dev *dev,
										const struct switch_attr *attr,
		   								struct switch_val *val)
{
	struct qca_phy_priv *priv = qca_phy_priv_get(dev);
	a_uint32_t size = val->value.i;
	a_uint32_t ret;
	a_uint32_t port_id = SSDK_PHYSICAL_PORT0;

	if (priv->version == QCA_VER_HPPE)
	{
		for(port_id = SSDK_PHYSICAL_PORT1; port_id <= SSDK_PHYSICAL_PORT6;
			port_id++)
		{
			ret = fal_port_max_frame_size_set(priv->device_id,
				port_id, size);
			if(ret)
			{
				return -1;
			}
		}
	}
	else if (priv->version == QCA_VER_SCOMPHY)
	{
#ifdef MP
		if(adapt_scomphy_revision_get(priv->device_id)
			== MP_GEPHY)
		{
			for(port_id = SSDK_PHYSICAL_PORT1; port_id <= SSDK_PHYSICAL_PORT2;
				port_id++)
			{
				ret = fal_port_max_frame_size_set(priv->device_id,
					port_id, size);
				if(ret)
				{
					return -1;
				}
			}
		}
#endif
	}
	else
	{
		ret = fal_frame_max_size_set(priv->device_id, size);
		if (ret)
		{
			return -1;
		}
	}

	return 0;
}

int
qca_ar8327_sw_get_max_frame_size(struct switch_dev *dev,
										const struct switch_attr *attr,
		   								struct switch_val *val)
{
	struct qca_phy_priv *priv = qca_phy_priv_get(dev);
	a_uint32_t size = 0;
	a_uint32_t ret = 0;

	if (priv->version == QCA_VER_HPPE)
	{
		ret = fal_port_max_frame_size_get(priv->device_id,
			SSDK_PHYSICAL_PORT1, &size);
	}
	else if (priv->version == QCA_VER_SCOMPHY)
	{
#ifdef MP
		if(adapt_scomphy_revision_get(priv->device_id)
			== MP_GEPHY)
		{
			ret = fal_port_max_frame_size_get(priv->device_id,
				SSDK_PHYSICAL_PORT1, &size);
		}
#endif
	}
	else
	{
		ret = fal_frame_max_size_get(priv->device_id, &size);
	}
	if (ret){
		return -1;
	}

	val->value.i = size;

	return 0;
}

int
qca_ar8327_sw_reset_switch(struct switch_dev *dev)
{
	struct qca_phy_priv *priv = qca_phy_priv_get(dev);
	int rv = 0;
	a_uint32_t mac_mode;

	mutex_lock(&priv->reg_mutex);

	/* flush all vlan translation unit entries */
	fal_vlan_flush(priv->device_id);

	/* reset VLAN shadow */
	priv->vlan = 0;
	memset(priv->vlan_id, 0, sizeof(a_uint16_t) * AR8327_MAX_VLANS);
	memset(priv->vlan_table, 0, sizeof(a_uint8_t) * AR8327_MAX_VLANS);
	memset(priv->vlan_tagged, 0, sizeof(a_uint8_t) * AR8327_MAX_VLANS);
	memset(priv->pvid, 0, sizeof(a_uint16_t) * AR8327_NUM_PORTS);

	mutex_unlock(&priv->reg_mutex);

	priv->init = true;
	rv += qca_ar8327_sw_hw_apply(dev);
	priv->init = false;

	mac_mode = ssdk_dt_global_get_mac_mode(priv->device_id, 0);
	/* set mac5 flowcontol force for RGMII */
	if ((mac_mode == PORT_WRAPPER_SGMII0_RGMII5)
		||(mac_mode == PORT_WRAPPER_SGMII1_RGMII5)) {
		fal_port_flowctrl_forcemode_set(priv->device_id, 5, A_TRUE);
		fal_port_flowctrl_set(priv->device_id, 5, A_TRUE);
	}
	/* set mac4 flowcontol force for RGMII */
	if ((mac_mode == PORT_WRAPPER_SGMII0_RGMII4)
		||(mac_mode == PORT_WRAPPER_SGMII1_RGMII4)
		||(mac_mode == PORT_WRAPPER_SGMII4_RGMII4)) {
		/*  fix channel4 will recieve packets when enable channel0 as SGMII */
		if(mac_mode == PORT_WRAPPER_SGMII0_RGMII4) {
			fal_port_txmac_status_set (priv->device_id, 5, A_FALSE);
			fal_port_rxmac_status_set (priv->device_id, 5, A_FALSE);
		}
		fal_port_flowctrl_forcemode_set(priv->device_id, 4, A_TRUE);
		fal_port_flowctrl_set(priv->device_id, 4, A_TRUE);
	}

	return rv;
}

