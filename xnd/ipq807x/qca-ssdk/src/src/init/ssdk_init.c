/*
 * Copyright (c) 2012, 2014-2021, The Linux Foundation. All rights reserved.
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

/*qca808x_start*/
#include "sw.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include "fal.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_phy.h"
/*qca808x_end*/
#include "ssdk_dts.h"
#include "ssdk_interrupt.h"
#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/kernel.h>
/*qca808x_start*/
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>
/*qca808x_end*/
//#include <asm/mach-types.h>
#include <generated/autoconf.h>
#include <linux/if_arp.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
/*qca808x_start*/
#include <linux/phy.h>
#include <linux/mdio.h>
/*qca808x_end*/
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#if defined(IN_SWCONFIG)
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/switch.h>
#else
#include <net/switch.h>
#endif
#endif

#if defined(ISIS) ||defined(ISISC) ||defined(GARUDA)
#include <f1_phy.h>
#endif
#if defined(ATHENA) ||defined(SHIVA) ||defined(HORUS)
#include <f2_phy.h>
#endif
#ifdef IN_MALIBU_PHY
#include <malibu_phy.h>
#endif
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0))
/*qca808x_start*/
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_net.h>
#include <linux/of_address.h>
#include <linux/reset.h>
/*qca808x_end*/
#ifdef BOARD_AR71XX
#ifdef CONFIG_AR8216_PHY
#include "drivers/net/phy/ar8327.h"
#endif
#include "drivers/net/ethernet/atheros/ag71xx/ag71xx.h"
#endif
#elif defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_net.h>
#include <linux/of_address.h>
#include <linux/reset.h>
#else
#include <linux/ar8216_platform.h>
#include <drivers/net/phy/ar8216.h>
#include <drivers/net/ethernet/atheros/ag71xx/ag71xx.h>
#endif
/*qca808x_start*/
#include "ssdk_plat.h"
/*qca808x_end*/
#include "ssdk_clk.h"
#include "ssdk_led.h"
#include "ref_vlan.h"
#include "ref_fdb.h"
#include "ref_mib.h"
#include "ref_port_ctrl.h"
#include "ref_misc.h"
#include "ref_uci.h"
#include "ref_vsi.h"
#include "shell.h"
#ifdef BOARD_AR71XX
#include "ssdk_uci.h"
#endif
/*qca808x_start*/
#if defined(IN_PHY_I2C_MODE)
#include "ssdk_phy_i2c.h"
#endif
/*qca808x_end*/
#ifdef IN_IP
#if defined (CONFIG_NF_FLOW_COOKIE)
#include "fal_flowcookie.h"
#ifdef IN_SFE
#include <shortcut-fe/sfe.h>
#endif
#endif
#endif

#ifdef IN_RFS
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#include <linux/if_vlan.h>
#endif
#include <qca-rfs/rfs_dev.h>
#ifdef IN_IP
#include "fal_rfs.h"
#endif
#endif
#include "adpt.h"
#ifdef HPPE
#include "ssdk_hppe.h"
#endif
#ifdef SCOMPHY
#include "ssdk_scomphy.h"
#endif

#ifdef IN_RFS
struct rfs_device rfs_dev;
struct notifier_block ssdk_inet_notifier;
ssdk_rfs_intf_t rfs_intf_tbl[SSDK_RFS_INTF_MAX] = {{0}};
#endif

//#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
struct notifier_block ssdk_dev_notifier;
//#endif


extern ssdk_chip_type SSDK_CURRENT_CHIP_TYPE;
extern a_uint32_t hsl_dev_wan_port_get(a_uint32_t dev_id);
extern void dess_rgmii_sw_mac_polling_task(struct qca_phy_priv *priv);
extern void qca_ar8327_sw_mac_polling_task(struct qca_phy_priv *priv);
extern void qca_ar8327_sw_mib_task(struct qca_phy_priv *priv);

//#define PSGMII_DEBUG

#define QCA_QM_WORK_DELAY	100
#define QCA_QM_ITEM_NUMBER 41
#define QCA_RGMII_WORK_DELAY	1000
#define QCA_MAC_SW_SYNC_WORK_DELAY	1000
#ifdef DESS
static bool qca_dess_rfs_registered = false;
#endif
/*qca808x_start*/
struct qca_phy_priv **qca_phy_priv_global;

struct qca_phy_priv* ssdk_phy_priv_data_get(a_uint32_t dev_id)
{
	if (dev_id >= SW_MAX_NR_DEV || !qca_phy_priv_global)
		return NULL;

	return qca_phy_priv_global[dev_id];
}
/*qca808x_end*/

a_uint32_t hppe_port_type[6] = {0,0,0,0,0,0}; // this variable should be init by ssdk_init

a_uint32_t
qca_hppe_port_mac_type_get(a_uint32_t dev_id, a_uint32_t port_id)
{
	if ((port_id < 1) || (port_id > 6))
		return 0;
	return hppe_port_type[port_id - 1];
}

a_uint32_t
qca_hppe_port_mac_type_set(a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t port_type)
{
	 if ((port_id < 1) || (port_id > 6))
		 return 0;
	hppe_port_type[port_id - 1] = port_type;

	return 0;
}

#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
static void
ssdk_phy_rgmii_set(struct qca_phy_priv *priv)
{
	struct device_node *np = NULL;
	u32 rgmii_en = 0, tx_delay = 0, rx_delay = 0;

	if (priv->ess_switch_flag == A_TRUE)
		np = priv->of_node;
	else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
		np = priv->phy->mdio.dev.of_node;
#else
		np = priv->phy->dev.of_node;
#endif

	if (!np)
		return;

	if (!of_property_read_u32(np, "phy_rgmii_en", &rgmii_en)) {
		a_uint16_t val = 0;
		/*enable RGMII  mode */
		qca_ar8327_phy_dbg_read(0, AR8327_PORT5_PHY_ADDR,
				AR8327_PHY_REG_MODE_SEL, &val);
		val |= AR8327_PHY_RGMII_MODE;
		qca_ar8327_phy_dbg_write(0, AR8327_PORT5_PHY_ADDR,
				AR8327_PHY_REG_MODE_SEL, val);
		if (!of_property_read_u32(np, "txclk_delay_en", &tx_delay)
				&& tx_delay == 1) {
			qca_ar8327_phy_dbg_read(0, AR8327_PORT5_PHY_ADDR,
					AR8327_PHY_REG_SYS_CTRL, &val);
			val |= AR8327_PHY_RGMII_TX_DELAY;
			qca_ar8327_phy_dbg_write(0, AR8327_PORT5_PHY_ADDR,
					AR8327_PHY_REG_SYS_CTRL, val);
		}
		if (!of_property_read_u32(np, "rxclk_delay_en", &rx_delay)
				&& rx_delay == 1) {
			qca_ar8327_phy_dbg_read(0, AR8327_PORT5_PHY_ADDR,
					AR8327_PHY_REG_TEST_CTRL, &val);
			val |= AR8327_PHY_RGMII_RX_DELAY;
			qca_ar8327_phy_dbg_write(0, AR8327_PORT5_PHY_ADDR,
					AR8327_PHY_REG_TEST_CTRL, val);
		}
	}
}
#else
static void
ssdk_phy_rgmii_set(struct qca_phy_priv *priv)
{
	struct ar8327_platform_data *plat_data;

	plat_data = priv->phy->dev.platform_data;
	if (plat_data == NULL) {
		return;
	}

	if(plat_data->pad5_cfg) {
		if(plat_data->pad5_cfg->mode == AR8327_PAD_PHY_RGMII) {
			a_uint16_t val = 0;
			/*enable RGMII  mode */
			priv->phy_dbg_read(0, AR8327_PORT5_PHY_ADDR,
					AR8327_PHY_REG_MODE_SEL, &val);
			val |= AR8327_PHY_RGMII_MODE;
			priv->phy_dbg_write(0, AR8327_PORT5_PHY_ADDR,
					AR8327_PHY_REG_MODE_SEL, val);
			if(plat_data->pad5_cfg->txclk_delay_en) {
				priv->phy_dbg_read(0, AR8327_PORT5_PHY_ADDR,
						AR8327_PHY_REG_SYS_CTRL, &val);
				val |= AR8327_PHY_RGMII_TX_DELAY;
				priv->phy_dbg_write(0, AR8327_PORT5_PHY_ADDR,
						AR8327_PHY_REG_SYS_CTRL, val);
			}
			if(plat_data->pad5_cfg->rxclk_delay_en) {
				priv->phy_dbg_read(0, AR8327_PORT5_PHY_ADDR,
						AR8327_PHY_REG_TEST_CTRL, &val);
				val |= AR8327_PHY_RGMII_RX_DELAY;
				priv->phy_dbg_write(0, AR8327_PORT5_PHY_ADDR,
						AR8327_PHY_REG_TEST_CTRL, val);
			}
		}
	}
}
#endif
#endif


static void
qca_ar8327_phy_fixup(struct qca_phy_priv *priv, int phy)
{
	switch (priv->revision) {
	case 1:
		/* 100m waveform */
		priv->phy_dbg_write(priv->device_id, phy, 0, 0x02ea);
		/* turn on giga clock */
		priv->phy_dbg_write(priv->device_id, phy, 0x3d, 0x68a0);
		break;

	case 2:
		priv->phy_mmd_write(priv->device_id, phy, 0x7, 0x3c);
		priv->phy_mmd_write(priv->device_id, phy, 0x4007, 0x0);
		/* fallthrough */
	case 4:
		priv->phy_mmd_write(priv->device_id, phy, 0x3, 0x800d);
		priv->phy_mmd_write(priv->device_id, phy, 0x4003, 0x803f);

		priv->phy_dbg_write(priv->device_id, phy, 0x3d, 0x6860);
		priv->phy_dbg_write(priv->device_id, phy, 0x5, 0x2c46);
		priv->phy_dbg_write(priv->device_id, phy, 0x3c, 0x6000);
		break;
	}
}

#ifdef IN_PORTVLAN
static void qca_port_isolate(a_uint32_t dev_id)
{
	a_uint32_t port_id, mem_port_id, mem_port_map[AR8327_NUM_PORTS]={0};

	for(port_id = 0; port_id < AR8327_NUM_PORTS; port_id++)
	{
		if(port_id == 6)
			for(mem_port_id = 1; mem_port_id<= 4; mem_port_id++)
				mem_port_map[port_id]  |= (1 << mem_port_id);
		else if (port_id == 0)
			mem_port_map[port_id]  |= (1 << 5);
		else if (port_id >= 1 && port_id <= 4)
			mem_port_map[port_id]  |= (1 << 6);
		else
			mem_port_map[port_id]  |= 1;
	}

	for(port_id = 0; port_id < AR8327_NUM_PORTS; port_id++)

		 fal_portvlan_member_update(dev_id, port_id, mem_port_map[port_id]);

}

void ssdk_portvlan_init(a_uint32_t dev_id)
{
	a_uint32_t port = 0;
	a_uint32_t cpu_bmp, lan_bmp, wan_bmp;

	cpu_bmp = ssdk_cpu_bmp_get(dev_id);
	lan_bmp = ssdk_lan_bmp_get(dev_id);
	wan_bmp = ssdk_wan_bmp_get(dev_id);

	if (!(cpu_bmp | lan_bmp | wan_bmp)) {
		qca_port_isolate(dev_id);
		return;
	}

	for(port = 0; port < SSDK_MAX_PORT_NUM; port++)
	{
		if(cpu_bmp & (1 << port))
		{
			fal_portvlan_member_update(dev_id, port, lan_bmp|wan_bmp);
		}
		if(lan_bmp & (1 << port))
		{
			fal_portvlan_member_update(dev_id, port, (lan_bmp|cpu_bmp)&(~(1<<port)));
		}
		if(wan_bmp & (1 << port))
		{
			fal_portvlan_member_update(dev_id, port, (wan_bmp|cpu_bmp)&(~(1<<port)));
		}
	}
}
#endif

sw_error_t
qca_switch_init(a_uint32_t dev_id)
{
#if (defined(DESS) || defined(ISISC) || defined(ISIS)) && defined(IN_QOS)
	a_uint32_t nr = 0;
#endif
	int i = 0;

	/*fal_reset(dev_id);*/
	/*enable cpu and disable mirror*/
#ifdef IN_MISC
	fal_cpu_port_status_set(dev_id, A_TRUE);
	/* setup MTU */
	fal_frame_max_size_set(dev_id, 1518);
#endif
#ifdef IN_MIB
	/* Enable MIB counters */
	fal_mib_status_set(dev_id, A_TRUE);
	fal_mib_cpukeep_set(dev_id, A_FALSE);
#endif
#ifdef IN_IGMP
	fal_igmp_mld_rp_set(dev_id, 0);
#endif

	/*enable pppoe for dakota to support RSS*/
	if (SSDK_CURRENT_CHIP_TYPE == CHIP_DESS) {
#ifdef DESS
#ifdef IN_PPPOE
		fal_pppoe_status_set(dev_id, A_TRUE);
#endif
#endif
	}

	for (i = 0; i < AR8327_NUM_PORTS; i++) {
		/* forward multicast and broadcast frames to CPU */
#ifdef IN_MISC
		fal_port_unk_uc_filter_set(dev_id, i, A_FALSE);
		fal_port_unk_mc_filter_set(dev_id, i, A_FALSE);
		fal_port_bc_filter_set(dev_id, i, A_FALSE);
#endif
#ifdef IN_PORTVLAN
		fal_port_default_svid_set(dev_id, i, 0);
		fal_port_default_cvid_set(dev_id, i, 0);
		fal_port_1qmode_set(dev_id, i, FAL_1Q_DISABLE);
		fal_port_egvlanmode_set(dev_id, i, FAL_EG_UNMODIFIED);
#endif

#ifdef IN_FDB
		fal_fdb_port_learn_set(dev_id, i, A_TRUE);
#endif
#ifdef IN_STP
		fal_stp_port_state_set(dev_id, 0, i, FAL_STP_FARWARDING);
#endif
#ifdef IN_PORTVLAN
		fal_port_vlan_propagation_set(dev_id, i, FAL_VLAN_PROPAGATION_REPLACE);
#endif
#ifdef IN_IGMP
		fal_port_igmps_status_set(dev_id, i, A_FALSE);
		fal_port_igmp_mld_join_set(dev_id, i, A_FALSE);
		fal_port_igmp_mld_leave_set(dev_id, i, A_FALSE);
		fal_igmp_mld_entry_creat_set(dev_id, A_FALSE);
		fal_igmp_mld_entry_v3_set(dev_id, A_FALSE);
#endif
		if (SSDK_CURRENT_CHIP_TYPE == CHIP_SHIVA) {
			return SW_OK;
		} else if (SSDK_CURRENT_CHIP_TYPE == CHIP_DESS) {
#ifdef DESS
#ifdef IN_PORTCONTROL
			fal_port_flowctrl_forcemode_set(dev_id, i, A_FALSE);
			fal_port_link_forcemode_set(dev_id, i, A_TRUE);
#endif
#ifdef IN_QOS
			nr = 240; /*30*8*/
			fal_qos_port_tx_buf_nr_set(dev_id, i, &nr);
			nr = 48; /*6*8*/
			fal_qos_port_rx_buf_nr_set(dev_id, i, &nr);
			fal_qos_port_red_en_set(dev_id, i, A_TRUE);
			nr = 32;
			fal_qos_queue_tx_buf_nr_set(dev_id, i, 5, &nr);
			fal_qos_queue_tx_buf_nr_set(dev_id, i, 4, &nr);
			fal_qos_queue_tx_buf_nr_set(dev_id, i, 3, &nr);
			fal_qos_queue_tx_buf_nr_set(dev_id, i, 2, &nr);
			fal_qos_queue_tx_buf_nr_set(dev_id, i, 1, &nr);
			fal_qos_queue_tx_buf_nr_set(dev_id, i, 0, &nr);
			if (i != SSDK_PHYSICAL_PORT0)
				fal_qos_port_mode_set(dev_id, i,
					FAL_QOS_DSCP_MODE, A_TRUE);
#endif
#endif
		} else if (SSDK_CURRENT_CHIP_TYPE == CHIP_ISISC ||
			SSDK_CURRENT_CHIP_TYPE == CHIP_ISIS) {
#if defined(ISISC) || defined(ISIS)
#ifdef IN_INTERFACECONTROL
			fal_port_3az_status_set(dev_id, i, A_FALSE);
#endif
#ifdef IN_PORTCONTROL
			fal_port_flowctrl_forcemode_set(dev_id, i, A_TRUE);
			fal_port_flowctrl_set(dev_id, i, A_FALSE);

			if (i != 0 && i != 6) {
				fal_port_flowctrl_set(dev_id, i, A_TRUE);
				fal_port_flowctrl_forcemode_set(dev_id, i, A_FALSE);
			}
#endif
			if (i == 0 || i == 5 || i == 6) {
#ifdef IN_QOS
				nr = 240; /*30*8*/
				fal_qos_port_tx_buf_nr_set(dev_id, i, &nr);
				nr = 48; /*6*8*/
				fal_qos_port_rx_buf_nr_set(dev_id, i, &nr);
				fal_qos_port_red_en_set(dev_id, i, A_TRUE);
				if (SSDK_CURRENT_CHIP_TYPE == CHIP_ISISC) {
					nr = 64; /*8*8*/
				} else if (SSDK_CURRENT_CHIP_TYPE == CHIP_ISIS) {
					nr = 60;
				}
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 5, &nr);
				nr = 48; /*6*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 4, &nr);
				nr = 32; /*4*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 3, &nr);
				nr = 32; /*4*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 2, &nr);
				nr = 32; /*4*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 1, &nr);
				nr = 24; /*3*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 0, &nr);
#endif
			} else {
#ifdef IN_QOS
				nr = 200; /*25*8*/
				fal_qos_port_tx_buf_nr_set(dev_id, i, &nr);
				nr = 48; /*6*8*/
				fal_qos_port_rx_buf_nr_set(dev_id, i, &nr);
				fal_qos_port_red_en_set(dev_id, i, A_TRUE);
				if (SSDK_CURRENT_CHIP_TYPE == CHIP_ISISC) {
					nr = 64; /*8*8*/
				} else if (SSDK_CURRENT_CHIP_TYPE == CHIP_ISIS) {
					nr = 60;
				}
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 3, &nr);
				nr = 48; /*6*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 2, &nr);
				nr = 32; /*4*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 1, &nr);
				nr = 24; /*3*8*/
				fal_qos_queue_tx_buf_nr_set(dev_id, i, 0, &nr);
#endif
			}
#endif
		}
	}

	return SW_OK;
}

void qca_ar8327_phy_linkdown(a_uint32_t dev_id)
{
	int i;
	a_uint16_t phy_val;

	for (i = 0; i < AR8327_NUM_PHYS; i++) {
		qca_ar8327_phy_write(dev_id, i, 0x0, 0x0800);	// phy powerdown

		qca_ar8327_phy_dbg_read(dev_id, i, 0x3d, &phy_val);
		phy_val &= ~0x0040;
		qca_ar8327_phy_dbg_write(dev_id, i, 0x3d, phy_val);

		/*PHY will stop the tx clock for a while when link is down
			1. en_anychange  debug port 0xb bit13 = 0  //speed up link down tx_clk
			2. sel_rst_80us  debug port 0xb bit10 = 0  //speed up speed mode change to 2'b10 tx_clk
		*/
		qca_ar8327_phy_dbg_read(dev_id, i, 0xb, &phy_val);
		phy_val &= ~0x2400;
		qca_ar8327_phy_dbg_write(dev_id, i, 0xb, phy_val);
	}
}

void
qca_mac_disable(a_uint32_t device_id)
{
	hsl_api_t *p_api;

	p_api = hsl_api_ptr_get (device_id);
	if(p_api
		&& p_api->interface_mac_pad_set
		&& p_api->interface_mac_sgmii_set)
	{
		p_api->interface_mac_pad_set(device_id,0,0);
		p_api->interface_mac_pad_set(device_id,5,0);
		p_api->interface_mac_pad_set(device_id,6,0);
		p_api->interface_mac_sgmii_set(device_id,AR8327_REG_PAD_SGMII_CTRL_HW_INIT);
	}
	else
	{
		SSDK_ERROR("API not support \n");
	}
}

static void qca_switch_set_mac_force(struct qca_phy_priv *priv)
{
	a_uint32_t value, value0, i;
	if (priv == NULL || (priv->mii_read == NULL) || (priv->mii_write == NULL)) {
		SSDK_ERROR("In qca_switch_set_mac_force, private data is NULL!\r\n");
		return;
	}

	for (i=0; i < AR8327_NUM_PORTS; ++i) {
		/* b3:2=0,Tx/Rx Mac disable,
		 b9=0,LINK_EN disable */
		value0 = priv->mii_read(priv->device_id, AR8327_REG_PORT_STATUS(i));
		value = value0 & ~(AR8327_PORT_STATUS_LINK_AUTO |
						AR8327_PORT_STATUS_TXMAC |
						AR8327_PORT_STATUS_RXMAC);
		priv->mii_write(priv->device_id, AR8327_REG_PORT_STATUS(i), value);

		/* Force speed to 1000M Full */
		value = priv->mii_read(priv->device_id, AR8327_REG_PORT_STATUS(i));
		value &= ~(AR8327_PORT_STATUS_DUPLEX | AR8327_PORT_STATUS_SPEED);
		value |= AR8327_PORT_SPEED_1000M | AR8327_PORT_STATUS_DUPLEX;
		priv->mii_write(priv->device_id, AR8327_REG_PORT_STATUS(i), value);
	}
	return;
}

void
qca_ar8327_phy_enable(struct qca_phy_priv *priv)
{
	int i = 0;
#ifndef BOARD_AR71XX
        ssdk_phy_rgmii_set(priv);
#endif
	for (i = 0; i < AR8327_NUM_PHYS; i++) {
		a_uint16_t value = 0;

		if (priv->version == QCA_VER_AR8327)
			qca_ar8327_phy_fixup(priv, i);

		/* start autoneg*/
		priv->phy_write(priv->device_id, i, MII_ADVERTISE, ADVERTISE_ALL |
						     ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM);
		//phy reg 0x9, b10,1 = Prefer multi-port device (master)
		priv->phy_write(priv->device_id, i, MII_CTRL1000, (0x0400|ADVERTISE_1000FULL));

		priv->phy_write(priv->device_id, i, MII_BMCR, BMCR_RESET | BMCR_ANENABLE);

		priv->phy_dbg_read(priv->device_id, i, 0, &value);
		value &= (~(1<<12));
		priv->phy_dbg_write(priv->device_id, i, 0, value);

		msleep(100);
	}
}
void qca_ar8327_sw_soft_reset(struct qca_phy_priv *priv)
{
	a_uint32_t value = 0;

	value = priv->mii_read(priv->device_id, AR8327_REG_CTRL);
	value |= 0x80000000;
	priv->mii_write(priv->device_id, AR8327_REG_CTRL, value);
	/*Need wait reset done*/
	do {
		udelay(10);
		value = priv->mii_read(priv->device_id, AR8327_REG_CTRL);
	} while(value & AR8327_CTRL_RESET);
	do {
		udelay(10);
		value = priv->mii_read(priv->device_id, 0x20);
	} while ((value & SSDK_GLOBAL_INITIALIZED_STATUS) !=
			SSDK_GLOBAL_INITIALIZED_STATUS);

	return;
}

#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
int qca_ar8327_hw_init(struct qca_phy_priv *priv)
{
	struct device_node *np = NULL;
	const __be32 *paddr;
	a_uint32_t reg, value, i;
	a_int32_t len;

	if (priv->ess_switch_flag == A_TRUE)
		np = priv->of_node;
	else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
		np = priv->phy->mdio.dev.of_node;
#else
		np = priv->phy->dev.of_node;
#endif

	if(!np)
		return -EINVAL;

	/*Before switch software reset, disable PHY and clear  MAC PAD*/
	qca_ar8327_phy_linkdown(priv->device_id);
	qca_mac_disable(priv->device_id);
	msleep(1000);

	/*First software reset S17 chip*/
	qca_ar8327_sw_soft_reset(priv);

	/*After switch software reset, need disable all ports' MAC with 1000M FULL*/
	qca_switch_set_mac_force(priv);

	/* Configure switch register from DT information */
	paddr = of_get_property(np, "qca,ar8327-initvals", &len);
	if (paddr) {
		if (len < (2 * sizeof(*paddr))) {
			SSDK_ERROR("len:%d < 2 * sizeof(*paddr):%zu\n", len, 2 * sizeof(*paddr));
			return -EINVAL;
		}

		len /= sizeof(*paddr);

		for (i = 0; i < len - 1; i += 2) {
			reg = be32_to_cpup(paddr + i);
			value = be32_to_cpup(paddr + i + 1);
			priv->mii_write(priv->device_id, reg, value);
		}
	}

	value = priv->mii_read(priv->device_id, AR8327_REG_MODULE_EN);
	value &= ~AR8327_REG_MODULE_EN_QM_ERR;
	value &= ~AR8327_REG_MODULE_EN_LOOKUP_ERR;
	priv->mii_write(priv->device_id, AR8327_REG_MODULE_EN, value);

	qca_switch_init(priv->device_id);
#ifdef IN_PORTVLAN
	ssdk_portvlan_init(priv->device_id);
#endif
	qca_mac_enable_intr(priv);
	qca_ar8327_phy_enable(priv);

	return 0;
}
#else
static a_uint32_t
qca_ar8327_get_pad_cfg(struct ar8327_pad_cfg *pad_cfg)
{
	a_uint32_t value = 0;

	if (pad_cfg == 0) {
		return 0;
    }

    if(pad_cfg->mode == AR8327_PAD_MAC2MAC_MII) {
		value = AR8327_PAD_CTRL_MAC_MII_EN;
		if (pad_cfg->rxclk_sel)
			value |= AR8327_PAD_CTRL_MAC_MII_RXCLK_SEL;
		if (pad_cfg->txclk_sel)
			value |= AR8327_PAD_CTRL_MAC_MII_TXCLK_SEL;

    } else if (pad_cfg->mode == AR8327_PAD_MAC2MAC_GMII) {
		value = AR8327_PAD_CTRL_MAC_GMII_EN;
		if (pad_cfg->rxclk_sel)
			value |= AR8327_PAD_CTRL_MAC_GMII_RXCLK_SEL;
		if (pad_cfg->txclk_sel)
			value |= AR8327_PAD_CTRL_MAC_GMII_TXCLK_SEL;

    } else if (pad_cfg->mode == AR8327_PAD_MAC_SGMII) {
		value = AR8327_PAD_CTRL_SGMII_EN;

		/* WAR for AP136 board. */
		value |= pad_cfg->txclk_delay_sel <<
		        AR8327_PAD_CTRL_RGMII_TXCLK_DELAY_SEL_S;
		value |= pad_cfg->rxclk_delay_sel <<
                AR8327_PAD_CTRL_RGMII_RXCLK_DELAY_SEL_S;
		if (pad_cfg->rxclk_delay_en)
			value |= AR8327_PAD_CTRL_RGMII_RXCLK_DELAY_EN;
		if (pad_cfg->txclk_delay_en)
			value |= AR8327_PAD_CTRL_RGMII_TXCLK_DELAY_EN;

    } else if (pad_cfg->mode == AR8327_PAD_MAC2PHY_MII) {
		value = AR8327_PAD_CTRL_PHY_MII_EN;
		if (pad_cfg->rxclk_sel)
			value |= AR8327_PAD_CTRL_PHY_MII_RXCLK_SEL;
		if (pad_cfg->txclk_sel)
			value |= AR8327_PAD_CTRL_PHY_MII_TXCLK_SEL;

    } else if (pad_cfg->mode == AR8327_PAD_MAC2PHY_GMII) {
		value = AR8327_PAD_CTRL_PHY_GMII_EN;
		if (pad_cfg->pipe_rxclk_sel)
			value |= AR8327_PAD_CTRL_PHY_GMII_PIPE_RXCLK_SEL;
		if (pad_cfg->rxclk_sel)
			value |= AR8327_PAD_CTRL_PHY_GMII_RXCLK_SEL;
		if (pad_cfg->txclk_sel)
			value |= AR8327_PAD_CTRL_PHY_GMII_TXCLK_SEL;

    } else if (pad_cfg->mode == AR8327_PAD_MAC_RGMII) {
		value = AR8327_PAD_CTRL_RGMII_EN;
		value |= pad_cfg->txclk_delay_sel <<
                 AR8327_PAD_CTRL_RGMII_TXCLK_DELAY_SEL_S;
		value |= pad_cfg->rxclk_delay_sel <<
                 AR8327_PAD_CTRL_RGMII_RXCLK_DELAY_SEL_S;
		if (pad_cfg->rxclk_delay_en)
			value |= AR8327_PAD_CTRL_RGMII_RXCLK_DELAY_EN;
		if (pad_cfg->txclk_delay_en)
			value |= AR8327_PAD_CTRL_RGMII_TXCLK_DELAY_EN;

    } else if (pad_cfg->mode == AR8327_PAD_PHY_GMII) {
		value = AR8327_PAD_CTRL_PHYX_GMII_EN;

    } else if (pad_cfg->mode == AR8327_PAD_PHY_RGMII) {
		value = AR8327_PAD_CTRL_PHYX_RGMII_EN;

    } else if (pad_cfg->mode == AR8327_PAD_PHY_MII) {
		value = AR8327_PAD_CTRL_PHYX_MII_EN;

	} else {
        value = 0;
    }

	return value;
}

#ifndef BOARD_AR71XX
static a_uint32_t
qca_ar8327_get_pwr_sel(struct qca_phy_priv *priv,
                                struct ar8327_platform_data *plat_data)
{
	struct ar8327_pad_cfg *cfg = NULL;
	a_uint32_t value;

	if (!plat_data) {
		return 0;
	}

	value = priv->mii_read(priv->device_id, AR8327_REG_PAD_MAC_PWR_SEL);

	cfg = plat_data->pad0_cfg;

	if (cfg && (cfg->mode == AR8327_PAD_MAC_RGMII) &&
                cfg->rgmii_1_8v) {
		value |= AR8327_PAD_MAC_PWR_RGMII0_1_8V;
	}

	cfg = plat_data->pad5_cfg;
	if (cfg && (cfg->mode == AR8327_PAD_MAC_RGMII) &&
                cfg->rgmii_1_8v) {
		value |= AR8327_PAD_MAC_PWR_RGMII1_1_8V;
	}

	cfg = plat_data->pad6_cfg;
	if (cfg && (cfg->mode == AR8327_PAD_MAC_RGMII) &&
               cfg->rgmii_1_8v) {
		value |= AR8327_PAD_MAC_PWR_RGMII1_1_8V;
	}

	return value;
}
#endif

static a_uint32_t
qca_ar8327_set_led_cfg(struct qca_phy_priv *priv,
                              struct ar8327_platform_data *plat_data,
                              a_uint32_t pos)
{
	struct ar8327_led_cfg *led_cfg;
	a_uint32_t new_pos = pos;

	led_cfg = plat_data->led_cfg;
	if (led_cfg) {
		if (led_cfg->open_drain)
			new_pos |= AR8327_POS_LED_OPEN_EN;
		else
			new_pos &= ~AR8327_POS_LED_OPEN_EN;

		priv->mii_write(priv->device_id, AR8327_REG_LED_CTRL_0, led_cfg->led_ctrl0);
		priv->mii_write(priv->device_id, AR8327_REG_LED_CTRL_1, led_cfg->led_ctrl1);
		priv->mii_write(priv->device_id, AR8327_REG_LED_CTRL_2, led_cfg->led_ctrl2);
		priv->mii_write(priv->device_id, AR8327_REG_LED_CTRL_3, led_cfg->led_ctrl3);

		if (new_pos != pos) {
			new_pos |= AR8327_POS_POWER_ON_SEL;
		}
	}
	return new_pos;
}
#ifndef BOARD_AR71XX
static int
qca_ar8327_set_sgmii_cfg(struct qca_phy_priv *priv,
                              struct ar8327_platform_data *plat_data,
                              a_uint32_t* new_pos)
{
	a_uint32_t value = 0;

	/*configure the SGMII*/
	value = priv->mii_read(priv->device_id, AR8327_REG_PAD_SGMII_CTRL);
	value &= ~(AR8327_PAD_SGMII_CTRL_MODE_CTRL);
	value |= ((plat_data->sgmii_cfg->sgmii_mode) <<
          AR8327_PAD_SGMII_CTRL_MODE_CTRL_S);

	if (priv->version == QCA_VER_AR8337) {
		value |= (AR8327_PAD_SGMII_CTRL_EN_PLL |
		     AR8327_PAD_SGMII_CTRL_EN_RX |
		     AR8327_PAD_SGMII_CTRL_EN_TX);
	} else {
		value &= ~(AR8327_PAD_SGMII_CTRL_EN_PLL |
		       AR8327_PAD_SGMII_CTRL_EN_RX |
		       AR8327_PAD_SGMII_CTRL_EN_TX);
	}
	value |= AR8327_PAD_SGMII_CTRL_EN_SD;

	priv->mii_write(priv->device_id, AR8327_REG_PAD_SGMII_CTRL, value);

	if (plat_data->sgmii_cfg->serdes_aen) {
		*new_pos &= ~AR8327_POS_SERDES_AEN;
	} else {
		*new_pos |= AR8327_POS_SERDES_AEN;
	}
	return 0;
}
#endif

static int
qca_ar8327_set_plat_data_cfg(struct qca_phy_priv *priv,
                              struct ar8327_platform_data *plat_data)
{
	a_uint32_t pos, new_pos;

	pos = priv->mii_read(priv->device_id, AR8327_REG_POS);

	new_pos = qca_ar8327_set_led_cfg(priv, plat_data, pos);

#ifndef BOARD_AR71XX
	/*configure the SGMII*/
	if (plat_data->sgmii_cfg) {
		qca_ar8327_set_sgmii_cfg(priv, plat_data, &new_pos);
	}
#endif

	priv->mii_write(priv->device_id, AR8327_REG_POS, new_pos);

	return 0;
}

static int
qca_ar8327_set_pad_cfg(struct qca_phy_priv *priv,
                              struct ar8327_platform_data *plat_data)
{
	a_uint32_t pad0 = 0, pad5 = 0, pad6 = 0;

	pad0 = qca_ar8327_get_pad_cfg(plat_data->pad0_cfg);
	priv->mii_write(priv->device_id, AR8327_REG_PAD0_CTRL, pad0);

	pad5 = qca_ar8327_get_pad_cfg(plat_data->pad5_cfg);
	if(priv->version == QCA_VER_AR8337) {
	        pad5 |= AR8327_PAD_CTRL_RGMII_RXCLK_DELAY_EN;
	}
	priv->mii_write(priv->device_id, AR8327_REG_PAD5_CTRL, pad5);

	pad6 = qca_ar8327_get_pad_cfg(plat_data->pad6_cfg);
	if(plat_data->pad5_cfg &&
		(plat_data->pad5_cfg->mode == AR8327_PAD_PHY_RGMII))
		pad6 |= AR8327_PAD_CTRL_PHYX_RGMII_EN;
	priv->mii_write(priv->device_id, AR8327_REG_PAD6_CTRL, pad6);

	return 0;
}

void
qca_ar8327_port_init(struct qca_phy_priv *priv, a_uint32_t port)
{
	struct ar8327_platform_data *plat_data;
	struct ar8327_port_cfg *port_cfg;
	a_uint32_t value;

	plat_data = priv->phy->dev.platform_data;
	if (plat_data == NULL) {
		return;
	}

	if (((port == 0) && plat_data->pad0_cfg) ||
	    ((port == 5) && plat_data->pad5_cfg) ||
	    ((port == 6) && plat_data->pad6_cfg)) {
	        switch (port) {
		        case 0:
		            port_cfg = &plat_data->cpuport_cfg;
		            break;
		        case 5:
		            port_cfg = &plat_data->port5_cfg;
		            break;
		        case 6:
		            port_cfg = &plat_data->port6_cfg;
		            break;
	        }
	} else {
	        return;
	}

	/*disable mac at first*/
	fal_port_rxmac_status_set(priv->device_id, port, A_FALSE);
	fal_port_txmac_status_set(priv->device_id, port, A_FALSE);
	value = port_cfg->duplex ? FAL_FULL_DUPLEX : FAL_HALF_DUPLEX;
	fal_port_duplex_set(priv->device_id, port, value);
	value = port_cfg->txpause ? A_TRUE : A_FALSE;
	fal_port_txfc_status_set(priv->device_id, port, value);
	value = port_cfg->rxpause ? A_TRUE : A_FALSE;
	fal_port_rxfc_status_set(priv->device_id, port, value);
	if(port_cfg->speed == AR8327_PORT_SPEED_10) {
		value = FAL_SPEED_10;
	} else if(port_cfg->speed == AR8327_PORT_SPEED_100) {
		value = FAL_SPEED_100;
	} else if(port_cfg->speed == AR8327_PORT_SPEED_1000) {
		value = FAL_SPEED_1000;
	} else {
		value = FAL_SPEED_1000;
	}
	fal_port_speed_set(priv->device_id, port, value);
	/*enable mac at last*/
	udelay(800);
	fal_port_rxmac_status_set(priv->device_id, port, A_TRUE);
	fal_port_txmac_status_set(priv->device_id, port, A_TRUE);
}

int
qca_ar8327_hw_init(struct qca_phy_priv *priv)
{
	struct ar8327_platform_data *plat_data;
	a_uint32_t i = 0;
	a_uint32_t value = 0;

	plat_data = priv->phy->dev.platform_data;
	if (plat_data == NULL) {
		return -EINVAL;
	}

	/*Before switch software reset, disable PHY and clear MAC PAD*/
	qca_ar8327_phy_linkdown(priv->device_id);
	qca_mac_disable(priv->device_id);
	udelay(10);

	qca_ar8327_set_plat_data_cfg(priv, plat_data);

	/*mac reset*/
	priv->mii_write(priv->device_id, AR8327_REG_MAC_SFT_RST, 0x3fff);

	msleep(100);

	/*First software reset S17 chip*/
	qca_ar8327_sw_soft_reset(priv);
	udelay(1000);

	/*After switch software reset, need disable all ports' MAC with 1000M FULL*/
	qca_switch_set_mac_force(priv);

	qca_ar8327_set_pad_cfg(priv, plat_data);

	value = priv->mii_read(priv->device_id, AR8327_REG_MODULE_EN);
	value &= ~AR8327_REG_MODULE_EN_QM_ERR;
	value &= ~AR8327_REG_MODULE_EN_LOOKUP_ERR;
	priv->mii_write(priv->device_id, AR8327_REG_MODULE_EN, value);

	qca_switch_init(priv->device_id);

#ifndef BOARD_AR71XX
	value = qca_ar8327_get_pwr_sel(priv, plat_data);
	priv->mii_write(priv->device_id, AR8327_REG_PAD_MAC_PWR_SEL, value);
#endif

	msleep(1000);

	for (i = 0; i < AR8327_NUM_PORTS; i++) {
		qca_ar8327_port_init(priv, i);
	}

	qca_ar8327_phy_enable(priv);

	return 0;
}
#endif

#if defined(IN_SWCONFIG)
#ifndef BOARD_AR71XX
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
static int
qca_ar8327_sw_get_reg_val(struct switch_dev *dev,
                                    int reg, int *val)
{
	return 0;
}

static int
qca_ar8327_sw_set_reg_val(struct switch_dev *dev,
                                    int reg, int val)
{
	return 0;
}
#endif
#endif
static struct switch_attr qca_ar8327_globals[] = {
#if defined(IN_VLAN)
	{
		.name = "enable_vlan",
		.description = "Enable 8021q VLAN",
		.type = SWITCH_TYPE_INT,
		.set = qca_ar8327_sw_set_vlan,
		.get = qca_ar8327_sw_get_vlan,
		.max = 1
	},
#endif
#if defined(IN_MISC)
	{
		.name = "max_frame_size",
		.description = "Set Max frame Size Of Mac",
		.type = SWITCH_TYPE_INT,
		.set = qca_ar8327_sw_set_max_frame_size,
		.get = qca_ar8327_sw_get_max_frame_size,
		.max = 9018
	},
#endif
#if defined(IN_MIB)
	{
		.name = "reset_mibs",
		.description = "Reset All MIB Counters",
		.type = SWITCH_TYPE_NOVAL,
		.set = qca_ar8327_sw_set_reset_mibs,
	},
#endif
#ifdef IN_FDB
	{
		.name = "flush_arl",
		.description = "Flush All ARL table",
		.type = SWITCH_TYPE_NOVAL,
		.set = qca_ar8327_sw_atu_flush,
	},
	{
		.name = "dump_arl",
		.description = "Dump All ARL table",
		.type = SWITCH_TYPE_STRING,
		.get = qca_ar8327_sw_atu_dump,
	},
#endif
	{
		.name = "switch_ext",
		.description = "Switch extended configuration",
		.type = SWITCH_TYPE_EXT,
		.set = qca_ar8327_sw_switch_ext,
	},
};

static struct switch_attr qca_ar8327_port[] = {
#if defined(IN_MIB)
	{
		.name = "reset_mib",
		.description = "Reset Mib Counters",
		.type = SWITCH_TYPE_NOVAL,
		.set = qca_ar8327_sw_set_port_reset_mib,
	},
	{
		.name = "mib",
		.description = "Get Mib Counters",
		.type = SWITCH_TYPE_STRING,
		.set = NULL,
		.get = qca_ar8327_sw_get_port_mib,
	},
#endif
#if defined(IN_PORTCONTROL)
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_eee",
		.description = "Enable EEE",
		.set = qca_ar8327_sw_set_eee,
		.get = qca_ar8327_sw_get_eee,
		.max = 1,
	},
#endif
};

#if defined(IN_VLAN)
static struct switch_attr qca_ar8327_vlan[] = {
	{
		.name = "vid",
		.description = "Configure Vlan Id",
		.type = SWITCH_TYPE_INT,
		.set = qca_ar8327_sw_set_vid,
		.get = qca_ar8327_sw_get_vid,
		.max = 4094,
	},
};
#endif

const struct switch_dev_ops qca_ar8327_sw_ops = {
	.attr_global = {
		.attr = qca_ar8327_globals,
		.n_attr = ARRAY_SIZE(qca_ar8327_globals),
	},
	.attr_port = {
		.attr = qca_ar8327_port,
		.n_attr = ARRAY_SIZE(qca_ar8327_port),
	},
#if defined(IN_VLAN)
	.attr_vlan = {
		.attr = qca_ar8327_vlan,
		.n_attr = ARRAY_SIZE(qca_ar8327_vlan),
	},
	.get_port_pvid = qca_ar8327_sw_get_pvid,
	.set_port_pvid = qca_ar8327_sw_set_pvid,
	.get_vlan_ports = qca_ar8327_sw_get_ports,
	.set_vlan_ports = qca_ar8327_sw_set_ports,
	.apply_config = qca_ar8327_sw_hw_apply,
#endif
#if defined(IN_MISC)
	.reset_switch = qca_ar8327_sw_reset_switch,
#endif
#if defined(IN_PORTCONTROL)
	.get_port_link = qca_ar8327_sw_get_port_link,
#endif
#ifndef BOARD_AR71XX
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	.get_reg_val = qca_ar8327_sw_get_reg_val,
	.set_reg_val = qca_ar8327_sw_set_reg_val,
#endif
#endif
};
#endif

#define SSDK_MIB_CHANGE_WQ

static int
qca_phy_mib_task(struct qca_phy_priv *priv)
{
	qca_ar8327_sw_mib_task(priv);
	return 0;
}

static void
qca_phy_mib_work_task(struct work_struct *work)
{
	struct qca_phy_priv *priv = container_of(work, struct qca_phy_priv,
                                            mib_dwork.work);

	mutex_lock(&priv->mib_lock);

    qca_phy_mib_task(priv);

	mutex_unlock(&priv->mib_lock);
#ifndef SSDK_MIB_CHANGE_WQ
	schedule_delayed_work(&priv->mib_dwork,
			      msecs_to_jiffies(QCA_PHY_MIB_WORK_DELAY));
#else
	queue_delayed_work_on(0, system_long_wq, &priv->mib_dwork,
					msecs_to_jiffies(QCA_PHY_MIB_WORK_DELAY));
#endif
}

int
qca_phy_mib_work_start(struct qca_phy_priv *priv)
{
	mutex_init(&priv->mib_lock);
	if(SW_OK != fal_mib_counter_alloc(priv->device_id, &priv->mib_counters)){
		SSDK_ERROR("Memory allocation fail\n");
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&priv->mib_dwork, qca_phy_mib_work_task);
#ifndef SSDK_MIB_CHANGE_WQ
	schedule_delayed_work(&priv->mib_dwork,
			               msecs_to_jiffies(QCA_PHY_MIB_WORK_DELAY));
#else
	queue_delayed_work_on(0, system_long_wq, &priv->mib_dwork,
					msecs_to_jiffies(QCA_PHY_MIB_WORK_DELAY));
#endif

	return 0;
}

void
qca_phy_mib_work_stop(struct qca_phy_priv *priv)
{
	if(!priv)
		return;
	if(priv->mib_counters)
		kfree(priv->mib_counters);
	cancel_delayed_work_sync(&priv->mib_dwork);
}

#define SSDK_QM_CHANGE_WQ

static void
qm_err_check_work_task_polling(struct work_struct *work)
{
	struct qca_phy_priv *priv = container_of(work, struct qca_phy_priv,
                                            qm_dwork_polling.work);

	mutex_lock(&priv->qm_lock);

	qca_ar8327_sw_mac_polling_task(priv);

	mutex_unlock(&priv->qm_lock);

#ifndef SSDK_QM_CHANGE_WQ
	schedule_delayed_work(&priv->qm_dwork,
							msecs_to_jiffies(QCA_QM_WORK_DELAY));
#else
	queue_delayed_work_on(0, system_long_wq, &priv->qm_dwork_polling,
							msecs_to_jiffies(QCA_QM_WORK_DELAY));
#endif
}

static int config_gpio(a_uint32_t  gpio_num)
{
	int  error;

	if (gpio_is_valid(gpio_num))
	{
		error = gpio_request_one(gpio_num, GPIOF_IN, "linkchange");
		if (error < 0) {
			SSDK_ERROR("gpio request faild \n");
			return -1;
		}
		gpio_set_debounce(gpio_num, 60000);
	}
	else
	{
		SSDK_ERROR("gpio is invalid\n");
		return -1;
	}

	return 0;
}
static int qca_link_polling_select(struct qca_phy_priv *priv)
{
	struct device_node *np = NULL;
	const __be32 *link_polling_required, *link_intr_gpio;
	a_int32_t len;

	if (priv->ess_switch_flag == A_TRUE)
		np = priv->of_node;
	else if(priv->version == QCA_VER_AR8337 || priv->version == QCA_VER_AR8327)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
		np = priv->phy->mdio.dev.of_node;
#else
		np = priv->phy->dev.of_node;
#endif
	else
		SSDK_ERROR("cannot find np node!\n");

	if(!np)
	{
		SSDK_ERROR("np is null !\n");
		return -1;
	}

	link_polling_required = of_get_property(np, "link-polling-required", &len);
	if (!link_polling_required )
	{
		SSDK_INFO("link-polling-required node does not exist\n");
		return -1;
	}
	priv->link_polling_required  = be32_to_cpup(link_polling_required);
	if(!priv->link_polling_required)
	{
		link_intr_gpio = of_get_property(np, "link-intr-gpio", &len);
		if (!link_intr_gpio )
		{
			SSDK_ERROR("cannot find link-intr-gpio node\n");
			return -1;
		}
		if(config_gpio(be32_to_cpup(link_intr_gpio)))
			return -1;
		priv->link_interrupt_no = gpio_to_irq (be32_to_cpup(link_intr_gpio));
		SSDK_INFO("the interrupt number is:%x\n",priv->link_interrupt_no);
	}

	return 0;
}

int
qm_err_check_work_start(struct qca_phy_priv *priv)
{
	/*Only valid for S17c chip*/
	if (priv->version != QCA_VER_AR8337 &&
		priv->version != QCA_VER_AR8327 &&
		priv->version != QCA_VER_DESS)
	{
		return 0;
	}

	mutex_init(&priv->qm_lock);
	INIT_DELAYED_WORK(&priv->qm_dwork_polling, qm_err_check_work_task_polling);
#ifndef SSDK_MIB_CHANGE_WQ
	schedule_delayed_work(&priv->qm_dwork_polling,
							msecs_to_jiffies(QCA_QM_WORK_DELAY));
#else
	queue_delayed_work_on(0, system_long_wq, &priv->qm_dwork_polling,
							msecs_to_jiffies(QCA_QM_WORK_DELAY));
#endif

	return 0;
}

void
qm_err_check_work_stop(struct qca_phy_priv *priv)
{
	/*Only valid for S17c chip*/
	if (priv->version != QCA_VER_AR8337 &&
		priv->version != QCA_VER_AR8327 &&
		priv->version != QCA_VER_DESS) return;

		cancel_delayed_work_sync(&priv->qm_dwork_polling);

}
#ifdef DESS
static void
dess_rgmii_mac_work_task(struct work_struct *work)
{
	struct qca_phy_priv *priv = container_of(work, struct qca_phy_priv,
                                            rgmii_dwork.work);

	mutex_lock(&priv->rgmii_lock);

	dess_rgmii_sw_mac_polling_task(priv);

	mutex_unlock(&priv->rgmii_lock);

	schedule_delayed_work(&priv->rgmii_dwork, msecs_to_jiffies(QCA_RGMII_WORK_DELAY));
}

int
dess_rgmii_mac_work_start(struct qca_phy_priv *priv)
{
	mutex_init(&priv->rgmii_lock);

	INIT_DELAYED_WORK(&priv->rgmii_dwork, dess_rgmii_mac_work_task);

	schedule_delayed_work(&priv->rgmii_dwork, msecs_to_jiffies(QCA_RGMII_WORK_DELAY));

	return 0;
}

void
dess_rgmii_mac_work_stop(struct qca_phy_priv *priv)
{
	cancel_delayed_work_sync(&priv->rgmii_dwork);
}
#endif

void
qca_mac_port_status_init(a_uint32_t dev_id, a_uint32_t port_id)
{
	if(port_id < SSDK_PHYSICAL_PORT1 || port_id >= SW_MAX_NR_PORT)
	{
		SSDK_ERROR("port %d does not support status init\n", port_id);
		return;
	}
	qca_phy_priv_global[dev_id]->port_old_link[port_id - 1] = 0;
	qca_phy_priv_global[dev_id]->port_old_speed[port_id - 1] = FAL_SPEED_BUTT;
	qca_phy_priv_global[dev_id]->port_old_duplex[port_id - 1] = FAL_DUPLEX_BUTT;
	qca_phy_priv_global[dev_id]->port_old_tx_flowctrl[port_id - 1] = A_FALSE;
	qca_phy_priv_global[dev_id]->port_old_rx_flowctrl[port_id - 1] = A_FALSE;
	qca_phy_priv_global[dev_id]->port_tx_flowctrl_forcemode[port_id - 1] = A_FALSE;
	qca_phy_priv_global[dev_id]->port_rx_flowctrl_forcemode[port_id - 1] = A_FALSE;
}

void
qca_mac_sw_sync_port_status_init(a_uint32_t dev_id)
{
	a_uint32_t port_id;

	for (port_id = SSDK_PHYSICAL_PORT1; port_id < SW_MAX_NR_PORT; port_id ++) {
		qca_mac_port_status_init(dev_id, port_id);
	}
}
void
qca_mac_sw_sync_work_task(struct work_struct *work)
{
	adpt_api_t *p_adpt_api;

	struct qca_phy_priv *priv = container_of(work, struct qca_phy_priv,
					mac_sw_sync_dwork.work);

	mutex_lock(&priv->mac_sw_sync_lock);

	 if((p_adpt_api = adpt_api_ptr_get(priv->device_id)) != NULL) {
		if (NULL == p_adpt_api->adpt_port_polling_sw_sync_set)
			return;
		p_adpt_api->adpt_port_polling_sw_sync_set(priv);
	}

	mutex_unlock(&priv->mac_sw_sync_lock);

	schedule_delayed_work(&priv->mac_sw_sync_dwork,
					msecs_to_jiffies(QCA_MAC_SW_SYNC_WORK_DELAY));
}

int
qca_mac_sw_sync_work_start(struct qca_phy_priv *priv)
{
	if ((priv->version != QCA_VER_HPPE) && (priv->version != QCA_VER_SCOMPHY))
		return 0;

	if (priv->version == QCA_VER_HPPE) {
		qca_mac_sw_sync_port_status_init(priv->device_id);
	}

	mutex_init(&priv->mac_sw_sync_lock);

	INIT_DELAYED_WORK(&priv->mac_sw_sync_dwork,
					qca_mac_sw_sync_work_task);
	schedule_delayed_work(&priv->mac_sw_sync_dwork,
					msecs_to_jiffies(QCA_MAC_SW_SYNC_WORK_DELAY));

	return 0;
}

void
qca_mac_sw_sync_work_stop(struct qca_phy_priv *priv)
{
	if ((priv->version != QCA_VER_HPPE) && (priv->version != QCA_VER_SCOMPHY)) {
		return;
	}
	cancel_delayed_work_sync(&priv->mac_sw_sync_dwork);
}

void
qca_mac_sw_sync_work_resume(struct qca_phy_priv *priv)
{
	if ((priv->version != QCA_VER_HPPE) && (priv->version != QCA_VER_SCOMPHY)) {
		return;
	}

	schedule_delayed_work(&priv->mac_sw_sync_dwork,
					msecs_to_jiffies(QCA_MAC_SW_SYNC_WORK_DELAY));
}

int
qca_phy_id_chip(struct qca_phy_priv *priv)
{
	a_uint32_t value, version;

	value = qca_ar8216_mii_read(priv->device_id, AR8327_REG_CTRL);
	version = value & (AR8327_CTRL_REVISION |
                AR8327_CTRL_VERSION);
	priv->version = (version & AR8327_CTRL_VERSION) >>
                           AR8327_CTRL_VERSION_S;
	priv->revision = (version & AR8327_CTRL_REVISION);

    if((priv->version == QCA_VER_AR8327) ||
       (priv->version == QCA_VER_AR8337) ||
       (priv->version == QCA_VER_AR8227)) {
		return 0;

    } else {
		SSDK_ERROR("unsupported QCA device\n");
		return -ENODEV;
	}
}

#if defined(IN_SWCONFIG)
static int qca_switchdev_register(struct qca_phy_priv *priv)
{
	struct switch_dev *sw_dev;
	int ret = SW_OK;
	sw_dev = &priv->sw_dev;

	switch (priv->version) {
		case QCA_VER_AR8227:
			sw_dev->name = "QCA AR8227";
			sw_dev->alias = "QCA AR8227";
			break;
		case QCA_VER_AR8327:
			sw_dev->name = "QCA AR8327";
			sw_dev->alias = "QCA AR8327";
			break;
		case QCA_VER_AR8337:
			sw_dev->name = "QCA AR8337";
			sw_dev->alias = "QCA AR8337";
			break;
		case QCA_VER_DESS:
			sw_dev->name = "QCA DESS";
			sw_dev->alias = "QCA DESS";
			break;
		case QCA_VER_HPPE:
			sw_dev->name = "QCA HPPE";
			sw_dev->alias = "QCA HPPE";
			break;
		case QCA_VER_SCOMPHY:
#ifdef MP
			if(adapt_scomphy_revision_get(priv->device_id)
				== MP_GEPHY)
			{
				sw_dev->name = "QCA MP";
				sw_dev->alias = "QCA MP";
			}
#endif
			break;
		default:
			sw_dev->name = "unknown switch";
			sw_dev->alias = "unknown switch";
			break;
	}

	sw_dev->ops = &qca_ar8327_sw_ops;
	sw_dev->vlans = AR8327_MAX_VLANS;
	sw_dev->ports = priv->ports;

	ret = register_switch(sw_dev, NULL);
	if (ret != SW_OK) {
		SSDK_ERROR("register_switch failed for %s\n", sw_dev->name);
		return ret;
	}

	return ret;
}
#endif

static int
qca_phy_config_init(struct phy_device *pdev)
{
	struct qca_phy_priv *priv = pdev->priv;
	int ret = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	if (pdev->mdio.addr != 0) {
#else
	if (pdev->addr != 0) {
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	pdev->supported |= SUPPORTED_1000baseT_Full;
	pdev->advertising |= ADVERTISED_1000baseT_Full;
#else
	linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			pdev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			pdev->advertising);
#endif

#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		ssdk_phy_rgmii_set(priv);
#endif
#endif
		return 0;
	}

	if (priv == NULL)
		return -ENOMEM;

	priv->phy = pdev;
	ret = qca_phy_id_chip(priv);
	if (ret != 0) {
		return ret;
	}

	priv->mii_read = qca_ar8216_mii_read;
	priv->mii_write = qca_ar8216_mii_write;
	priv->phy_write = qca_ar8327_phy_write;
	priv->phy_read = qca_ar8327_phy_read;
	priv->phy_dbg_write = qca_ar8327_phy_dbg_write;
	priv->phy_dbg_read = qca_ar8327_phy_dbg_read;
	priv->phy_mmd_write = qca_ar8327_mmd_write;
	priv->ports = AR8327_NUM_PORTS;

	ret = qca_link_polling_select(priv);
	if(ret)
		priv->link_polling_required = 1;
	pdev->priv = priv;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	pdev->supported |= SUPPORTED_1000baseT_Full;
	pdev->advertising |= ADVERTISED_1000baseT_Full;
#else
	linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			pdev->supported);
	linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
			pdev->advertising);
#endif

#if defined(IN_SWCONFIG)
	ret = qca_switchdev_register(priv);
	if (ret != SW_OK) {
		return ret;
	}
#endif
	priv->qca_ssdk_sw_dev_registered = A_TRUE;

	snprintf(priv->link_intr_name, IFNAMSIZ, "switch0");

	ret = qca_ar8327_hw_init(priv);
	if (ret != 0) {
		return ret;
	}

	qca_phy_mib_work_start(priv);

	if(priv->link_polling_required)
	{
		SSDK_INFO("polling is selected\n");
		ret = qm_err_check_work_start(priv);
		if (ret != 0)
		{
			SSDK_ERROR("qm_err_check_work_start failed for chip 0x%02x%02x\n", priv->version, priv->revision);
			return ret;
		}
	}
	else
	{
		SSDK_INFO("interrupt is selected\n");
		priv->interrupt_flag = IRQF_TRIGGER_LOW;
		ret = qca_intr_init(priv);
		if(ret)
			SSDK_ERROR("the interrupt init faild !\n");
	}

	return ret;
}

#if defined(DESS) || defined(HPPE) || defined (ISISC) || defined (ISIS) || defined(MP)
static int ssdk_switch_register(a_uint32_t dev_id, ssdk_chip_type  chip_type)
{
	struct qca_phy_priv *priv;
	int ret = 0;
	a_uint32_t chip_id = 0;
	priv = qca_phy_priv_global[dev_id];

	priv->mii_read = qca_ar8216_mii_read;
	priv->mii_write = qca_ar8216_mii_write;
	priv->phy_write = qca_ar8327_phy_write;
	priv->phy_read = qca_ar8327_phy_read;
	priv->phy_dbg_write = qca_ar8327_phy_dbg_write;
	priv->phy_dbg_read = qca_ar8327_phy_dbg_read;
	priv->phy_mmd_write = qca_ar8327_mmd_write;

	if (chip_type == CHIP_DESS) {
		priv->ports = 6;
	} else if ((chip_type == CHIP_ISIS) || (chip_type == CHIP_ISISC)) {
		priv->ports = 7;
	} else if (chip_type == CHIP_SCOMPHY) {
#ifdef MP
		if(adapt_scomphy_revision_get(priv->device_id) == MP_GEPHY) {
			/*for ipq50xx, port id is 1 and 2, port 0 is not available*/
			priv->ports = 3;
		}
#endif
	} else {
		priv->ports = SSDK_MAX_PORT_NUM;
	}

#ifdef MP
	if(chip_type == CHIP_SCOMPHY)
	{
		priv->version = QCA_VER_SCOMPHY;
		SSDK_INFO("Chip version 0x%02x\n", priv->version);
	}
	else
#endif
	{
		if (fal_reg_get(dev_id, 0, (a_uint8_t *)&chip_id, 4) == SW_OK) {
			priv->version = ((chip_id >> 8) & 0xff);
			priv->revision = (chip_id & 0xff);
			SSDK_INFO("Chip version 0x%02x%02x\n", priv->version, priv->revision);
		}
	}

	mutex_init(&priv->reg_mutex);

#if defined(IN_SWCONFIG)
	ret = qca_switchdev_register(priv);
	if (ret != SW_OK) {
		return ret;
	}
#endif

	snprintf(priv->link_intr_name, IFNAMSIZ, "switch%d", dev_id);

	priv->qca_ssdk_sw_dev_registered = A_TRUE;
	ret = qca_phy_mib_work_start(qca_phy_priv_global[dev_id]);
	if (ret != 0) {
			SSDK_ERROR("qca_phy_mib_work_start failed for chip 0x%02x%02x\n", priv->version, priv->revision);
			return ret;
	}
	ret = qca_link_polling_select(priv);
	if(ret)
		priv->link_polling_required = 1;
	if(priv->link_polling_required)
	{
		SSDK_INFO("polling is selected\n");
		ret = qm_err_check_work_start(priv);
		if (ret != 0)
		{
			SSDK_ERROR("qm_err_check_work_start failed for chip 0x%02x%02x\n", priv->version, priv->revision);
			return ret;
		}
	}
	else
	{
		SSDK_INFO("interrupt is selected\n");
		priv->interrupt_flag = IRQF_TRIGGER_MASK;
		ret = qca_intr_init(priv);
		if(ret)
		{
			SSDK_ERROR("the interrupt init faild !\n");
			return ret;
		}
	}

#if 0
#ifdef DESS
	if ((ssdk_dt_global.mac_mode == PORT_WRAPPER_SGMII0_RGMII5)
		||(ssdk_dt_global.mac_mode == PORT_WRAPPER_SGMII1_RGMII5)
		||(ssdk_dt_global.mac_mode == PORT_WRAPPER_SGMII0_RGMII4)
		||(ssdk_dt_global.mac_mode == PORT_WRAPPER_SGMII1_RGMII4)
		||(ssdk_dt_global.mac_mode == PORT_WRAPPER_SGMII4_RGMII4)) {
		ret = dess_rgmii_mac_work_start(priv);
		if (ret != 0) {
			SSDK_ERROR("dess_rgmii_mac_work_start failed for chip 0x%02x%02x\n", priv->version, priv->revision);
			return ret;
		}
	}
#endif
#endif
#ifdef HPPE
	if (priv->version == QCA_VER_HPPE) {
		ret = qca_mac_sw_sync_work_start(priv);
		if (ret != 0) {
			SSDK_ERROR("qca_mac_sw_sync_work_start failed for chip 0x%02x%02x\n",
				priv->version, priv->revision);
			return ret;
		}
	}
#endif

	return 0;

}

static int ssdk_switch_unregister(a_uint32_t dev_id)
{
	qca_phy_mib_work_stop(qca_phy_priv_global[dev_id]);
	qm_err_check_work_stop(qca_phy_priv_global[dev_id]);
#ifdef HPPE
	qca_mac_sw_sync_work_stop(qca_phy_priv_global[dev_id]);
#endif
#if defined(IN_SWCONFIG)
	unregister_switch(&qca_phy_priv_global[dev_id]->sw_dev);
#endif
	return 0;
}
#endif

static int
qca_phy_read_status(struct phy_device *pdev)
{
	struct qca_phy_priv *priv = pdev->priv;
	a_uint32_t port_status;
	a_uint32_t port_speed;
	int ret = 0, addr = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	addr = pdev->mdio.addr;
#else
	addr = pdev->addr;
#endif
	if (addr != 0) {
		mutex_lock(&priv->reg_mutex);
		ret = genphy_read_status(pdev);
		mutex_unlock(&priv->reg_mutex);
		return ret;
	}

	mutex_lock(&priv->reg_mutex);
	port_status = priv->mii_read(priv->device_id, AR8327_REG_PORT_STATUS(addr));
	mutex_unlock(&priv->reg_mutex);

	pdev->link = 1;
	if (port_status & AR8327_PORT_STATUS_LINK_AUTO) {
		pdev->link = !!(port_status & AR8327_PORT_STATUS_LINK_UP);
		if (pdev->link == 0) {
			return ret;
		}
	}

	port_speed = (port_status & AR8327_PORT_STATUS_SPEED) >>
		            AR8327_PORT_STATUS_SPEED_S;

	switch (port_speed) {
		case AR8327_PORT_SPEED_10M:
			pdev->speed = SPEED_10;
			break;
		case AR8327_PORT_SPEED_100M:
			pdev->speed = SPEED_100;
			break;
		case AR8327_PORT_SPEED_1000M:
			pdev->speed = SPEED_1000;
			break;
		default:
			pdev->speed = 0;
			break;
	}

	if(port_status & AR8327_PORT_STATUS_DUPLEX) {
		pdev->duplex = DUPLEX_FULL;
	} else {
		pdev->duplex = DUPLEX_HALF;
	}

	pdev->state = PHY_RUNNING;
	netif_carrier_on(pdev->attached_dev);
	pdev->adjust_link(pdev->attached_dev);

	return ret;
}

static int
qca_phy_config_aneg(struct phy_device *pdev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	if (pdev->mdio.addr != 0) {
#else
	if (pdev->addr != 0) {
#endif
		return genphy_config_aneg(pdev);
	}

	return 0;
}

int qca_phy_suspend(struct phy_device *phydev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	struct mii_bus *bus = phydev->mdio.bus;
#else
	struct mii_bus *bus = phydev->bus;
#endif
	int val = 0;
	int addr;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	addr = phydev->mdio.addr;
#else
	addr = phydev->addr;
#endif

	val = mdiobus_read(bus, addr, MII_BMCR);
	return mdiobus_write(bus, addr, MII_BMCR, (u16)(val | BMCR_PDOWN));
}

int qca_phy_resume(struct phy_device *phydev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	struct mii_bus *bus = phydev->mdio.bus;
#else
	struct mii_bus *bus = phydev->bus;
#endif
	int val = 0;
	int addr;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	addr = phydev->mdio.addr;
#else
	addr = phydev->addr;
#endif

	val = mdiobus_read(bus, addr, MII_BMCR);
	return mdiobus_write(bus, addr, MII_BMCR, (u16)(val & ~BMCR_PDOWN));
}

static int
qca_phy_probe(struct phy_device *pdev)
{
	struct qca_phy_priv *priv;
	int ret;

	priv = kzalloc(sizeof(struct qca_phy_priv), GFP_KERNEL);
	if (priv == NULL) {
		return -ENOMEM;
	}

	pdev->priv = priv;
	priv->phy = pdev;
	mutex_init(&priv->reg_mutex);

	ret = qca_phy_id_chip(priv);
	return ret;
}

static void
qca_phy_remove(struct phy_device *pdev)
{
	struct qca_phy_priv *priv = pdev->priv;
	int addr;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	addr = pdev->mdio.addr;
#else
	addr = pdev->addr;
#endif

	if ((addr == 0) && priv && (priv->ports != 0)) {
		qca_phy_mib_work_stop(priv);
		qm_err_check_work_stop(priv);
#if defined(IN_SWCONFIG)
		if (priv->sw_dev.name != NULL)
			unregister_switch(&priv->sw_dev);
#endif
	}

	if (priv) {
		kfree(priv);
    }
}

static struct phy_driver qca_phy_driver = {
    .name		= "QCA AR8216 AR8236 AR8316 AR8327 AR8337",
	.phy_id		= 0x004d0000,
	.phy_id_mask= 0xffff0000,
	.probe		= qca_phy_probe,
	.remove		= qca_phy_remove,
	.config_init= &qca_phy_config_init,
	.config_aneg= &qca_phy_config_aneg,
	.read_status= &qca_phy_read_status,
	.suspend	= qca_phy_suspend,
	.resume		= qca_phy_resume,
	.features	= PHY_BASIC_FEATURES,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	.mdiodrv.driver		= { .owner = THIS_MODULE },
#else
	.driver		= { .owner = THIS_MODULE },
#endif
};

#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#ifdef DESS
struct reset_control *ess_rst = NULL;
struct reset_control *ess_mac_clock_disable[5] = {NULL,NULL,NULL,NULL,NULL};

void ssdk_ess_reset(void)
{
	if (!ess_rst)
		return;
	reset_control_assert(ess_rst);
	mdelay(10);
	reset_control_deassert(ess_rst);
	mdelay(100);
}
#endif

char ssdk_driver_name[] = "ess_ssdk";

static int ssdk_probe(struct platform_device *pdev)
{
	struct device_node *np;

	np = of_node_get(pdev->dev.of_node);
	if (of_device_is_compatible(np, "qcom,ess-instance"))
		return of_platform_populate(np, NULL, NULL, &pdev->dev);

#ifdef DESS
	ess_rst = devm_reset_control_get(&pdev->dev, "ess_rst");
	ess_mac_clock_disable[0] = devm_reset_control_get(&pdev->dev, "ess_mac1_clk_dis");
	ess_mac_clock_disable[1] = devm_reset_control_get(&pdev->dev, "ess_mac2_clk_dis");
	ess_mac_clock_disable[2] = devm_reset_control_get(&pdev->dev, "ess_mac3_clk_dis");
	ess_mac_clock_disable[3] = devm_reset_control_get(&pdev->dev, "ess_mac4_clk_dis");
	ess_mac_clock_disable[4] = devm_reset_control_get(&pdev->dev, "ess_mac5_clk_dis");

	if (IS_ERR(ess_rst)) {
		SSDK_INFO("ess_rst doesn't exist!\n");
		return 0;
	}
	if (!ess_mac_clock_disable[0]) {
		SSDK_ERROR("ess_mac1_clock_disable fail!\n");
		return -1;
	}
	if (!ess_mac_clock_disable[1]) {
		SSDK_ERROR("ess_mac2_clock_disable fail!\n");
		return -1;
	}
	if (!ess_mac_clock_disable[2]) {
		SSDK_ERROR("ess_mac3_clock_disable fail!\n");
		return -1;
	}
	if (!ess_mac_clock_disable[3]) {
		SSDK_ERROR("ess_mac4_clock_disable fail!\n");
		return -1;
	}
	if (!ess_mac_clock_disable[4]) {
		SSDK_ERROR("ess_mac5_clock_disable fail!\n");
		return -1;
	}
#endif
	return 0;
}

static const struct of_device_id ssdk_of_mtable[] = {
	{.compatible = "qcom,ess-switch" },
	{.compatible = "qcom,ess-switch-ipq60xx" },
	{.compatible = "qcom,ess-switch-ipq807x" },
	{.compatible = "qcom,ess-instance" },
	{}
};

static struct platform_driver ssdk_driver = {
        .driver = {
                .name    = ssdk_driver_name,
                .owner   = THIS_MODULE,
                .of_match_table = ssdk_of_mtable,
        },
        .probe    = ssdk_probe,
};
#endif
#endif
#ifdef DESS
static u32 phy_t_status = 0;
static a_uint16_t modectrl_data = 0;
void ssdk_malibu_psgmii_and_dakota_dess_reset(a_uint32_t dev_id, a_uint32_t first_phy_addr)
{
#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
	int m = 0, n = 0;
	a_uint32_t psgmii_phy_addr;

	psgmii_phy_addr = first_phy_addr + 5;

	/*reset Malibu PSGMII and Dakota ESS start*/
	qca_ar8327_phy_write(dev_id, psgmii_phy_addr, 0x0, 0x005b);/*fix phy psgmii RX 20bit*/
	qca_ar8327_phy_write(dev_id, psgmii_phy_addr, 0x0, 0x001b);/*reset phy psgmii*/
	qca_ar8327_phy_write(dev_id, psgmii_phy_addr, 0x0, 0x005b);/*release reset phy psgmii*/
	/* mdelay(100); this 100ms be replaced with below malibu psgmii calibration process*/
	/*check malibu psgmii calibration done start*/
	n = 0;
	while (n < 100) {
		u16 status;
		status = qca_phy_mmd_read(dev_id, psgmii_phy_addr, 1, 0x28);
		if (status & BIT(0))
			break;
		mdelay(10);
		n++;
	}
#ifdef PSGMII_DEBUG
	if (n >= 100)
		SSDK_INFO("MALIBU PSGMII PLL_VCO_CALIB NOT READY\n");
#endif
	mdelay(50);
	/*check malibu psgmii calibration done end..*/
	qca_ar8327_phy_write(dev_id, psgmii_phy_addr, 0x1a, 0x2230);/*freeze phy psgmii RX CDR*/

	ssdk_ess_reset();
	/*check dakota psgmii calibration done start*/
	m = 0;
	while (m < 100) {
		u32 status = 0;
		qca_psgmii_reg_read(dev_id, 0xa0, (a_uint8_t *)&status, 4);
		if (status & BIT(0))
			break;
		mdelay(10);
		m++;
	}
#ifdef PSGMII_DEBUG
	if (m >= 100)
		SSDK_INFO("DAKOTA PSGMII PLL_VCO_CALIB NOT READY\n");
#endif
	mdelay(50);
	/*check dakota psgmii calibration done end..*/
	qca_ar8327_phy_write(dev_id, psgmii_phy_addr, 0x1a, 0x3230);/*relesae phy psgmii RX CDR*/
	qca_ar8327_phy_write(dev_id, psgmii_phy_addr, 0x0, 0x005f);/*release phy psgmii RX 20bit*/
	mdelay(200);
#endif
#endif
	/*reset Malibu PSGMII and Dakota ESS end*/
	return;
}

static void ssdk_psgmii_phy_testing_printf(a_uint32_t phy, u32 tx_ok, u32 rx_ok,
				u32 tx_counter_error, u32 rx_counter_error)
{
	SSDK_INFO("tx_ok = 0x%x, rx_ok = 0x%x, tx_counter_error = 0x%x, rx_counter_error = 0x%x\n",
			tx_ok, rx_ok, tx_counter_error, rx_counter_error);
	if (tx_ok== 0x3000 && tx_counter_error == 0)
		SSDK_INFO("PHY %d single PSGMII test pass\n", phy);
	else
		SSDK_ERROR("PHY %d single PSGMII test fail\n", phy);
	return;

}
static void ssdk_psgmii_all_phy_testing_printf(a_uint32_t phy, u32 tx_ok, u32 rx_ok,
				u32 tx_counter_error, u32 rx_counter_error)
{
	SSDK_INFO("tx_ok = 0x%x, rx_ok = 0x%x, tx_counter_error = 0x%x, rx_counter_error = 0x%x\n",
			tx_ok, rx_ok, tx_counter_error, rx_counter_error);
	if (tx_ok== 0x3000 && tx_counter_error == 0)
		SSDK_INFO("PHY %d all PSGMII test pass\n", phy);
	else
		SSDK_ERROR("PHY %d all PSGMII test fail\n", phy);
	return;

}
void ssdk_psgmii_single_phy_testing(a_uint32_t dev_id, a_uint32_t phy, a_bool_t enable)
{
	int j = 0;

	u32 tx_counter_ok, tx_counter_error;
	u32 rx_counter_ok, rx_counter_error;
	u32 tx_counter_ok_high16;
	u32 rx_counter_ok_high16;
	u32 tx_ok, rx_ok;
	qca_ar8327_phy_write(dev_id, phy, 0x0, 0x9000);
	qca_ar8327_phy_write(dev_id, phy, 0x0, 0x4140);
	j = 0;
	while (j < 100) {
		u16 status = 0;
		qca_ar8327_phy_read(dev_id, phy, 0x11, &status);
		if (status & (1 << 10))
			break;
		mdelay(10);
		j++;
	}
	/*add a 300ms delay as qm polling task existing*/
	if (enable == A_TRUE)
		mdelay(300);

	/*enable check*/
	qca_phy_mmd_write(dev_id, phy, 7, 0x8029, 0x0000);
	qca_phy_mmd_write(dev_id, phy, 7, 0x8029, 0x0003);

	/*start traffic*/
	qca_phy_mmd_write(dev_id, phy, 7, 0x8020, 0xa000);
	mdelay(200);

	/*check counter*/
	tx_counter_ok = qca_phy_mmd_read(dev_id, phy, 7, 0x802e);
	tx_counter_ok_high16 = qca_phy_mmd_read(dev_id, phy, 7, 0x802d);
	tx_counter_error = qca_phy_mmd_read(dev_id, phy, 7, 0x802f);
	rx_counter_ok = qca_phy_mmd_read(dev_id, phy, 7, 0x802b);
	rx_counter_ok_high16 = qca_phy_mmd_read(dev_id, phy, 7, 0x802a);
	rx_counter_error = qca_phy_mmd_read(dev_id, phy, 7, 0x802c);
	tx_ok = tx_counter_ok + (tx_counter_ok_high16<<16);
	rx_ok = rx_counter_ok + (rx_counter_ok_high16<<16);
	if (tx_ok== 0x3000 && tx_counter_error == 0) {
		/*success*/
		phy_t_status &= (~(1<<phy));
	} else {
		phy_t_status |= (1<<phy);
	}

	if (enable == A_TRUE)
		ssdk_psgmii_phy_testing_printf(phy, tx_ok, rx_ok,
				tx_counter_error, rx_counter_error);

	qca_ar8327_phy_write(dev_id, phy, 0x0, 0x1840);
}

void ssdk_psgmii_all_phy_testing(a_uint32_t dev_id, a_uint32_t first_phy_addr, a_bool_t enable)
{
	int j = 0;
	a_uint32_t phy = 0;
	qca_ar8327_phy_write(dev_id, 0x1f, 0x0, 0x9000);
	qca_ar8327_phy_write(dev_id, 0x1f, 0x0, 0x4140);
	j = 0;
	while (j < 100) {
		for (phy = first_phy_addr; phy < first_phy_addr + 5; phy++) {
			u16 status = 0;
			qca_ar8327_phy_read(dev_id, phy, 0x11, &status);
			if (!(status & (1 << 10)))
				break;
		}

		if (phy >= (first_phy_addr + 5))
			break;
		mdelay(10);
		j++;
	}
	/*add a 300ms delay as qm polling task existing*/
	if (enable == A_TRUE)
		mdelay(300);

	/*enable check*/
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8029, 0x0000);
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8029, 0x0003);

	/*start traffic*/
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8020, 0xa000);
	mdelay(200);
	for (phy = first_phy_addr; phy < first_phy_addr + 5; phy++) {
		u32 tx_counter_ok, tx_counter_error;
		u32 rx_counter_ok, rx_counter_error;
		u32 tx_counter_ok_high16;
		u32 rx_counter_ok_high16;
		u32 tx_ok, rx_ok;
		/*check counter*/
		tx_counter_ok = qca_phy_mmd_read(dev_id, phy, 7, 0x802e);
		tx_counter_ok_high16 = qca_phy_mmd_read(dev_id, phy, 7, 0x802d);
		tx_counter_error = qca_phy_mmd_read(dev_id, phy, 7, 0x802f);
		rx_counter_ok = qca_phy_mmd_read(dev_id, phy, 7, 0x802b);
		rx_counter_ok_high16 = qca_phy_mmd_read(dev_id, phy, 7, 0x802a);
		rx_counter_error = qca_phy_mmd_read(dev_id, phy, 7, 0x802c);
		tx_ok = tx_counter_ok + (tx_counter_ok_high16<<16);
		rx_ok = rx_counter_ok + (rx_counter_ok_high16<<16);
		if (tx_ok== 0x3000 && tx_counter_error == 0) {
			/*success*/
			phy_t_status &= (~(1<<(phy+8)));
		} else {
			phy_t_status |= (1<<(phy+8));
		}

		if (enable == A_TRUE)
			ssdk_psgmii_all_phy_testing_printf(phy, tx_ok,
					rx_ok,
					tx_counter_error, rx_counter_error);
		}
	if (enable == A_TRUE)
		SSDK_INFO("PHY final test result: 0x%x \r\n",phy_t_status);

}

void ssdk_psgmii_get_first_phy_address(a_uint32_t dev_id,
	a_uint32_t *first_phy_addr)
{
	a_uint32_t port_id = 0, phy_addr = 0, phy_cnt = 0;
	a_uint32_t port_bmp[SW_MAX_NR_DEV] = {0};

	port_bmp[dev_id] = qca_ssdk_phy_type_port_bmp_get(dev_id, MALIBU_PHY_CHIP);

	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++)
	{
		if (port_bmp[dev_id] & (0x1 << port_id))
		{
			phy_cnt ++;
			phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
			if (phy_addr < *first_phy_addr) {
				*first_phy_addr = phy_addr;
			}
		}
	}
	if ((phy_cnt == QCA8072_PHY_NUM) && (*first_phy_addr >= 0x3)) {
		*first_phy_addr = *first_phy_addr - 3;
	}
}

void ssdk_psgmii_self_test(a_uint32_t dev_id, a_bool_t enable, a_uint32_t times,
				a_uint32_t *result)
{
	int i = 0;
	u32 value = 0;
	a_uint32_t first_phy_addr = MAX_PHY_ADDR + 1, phy = 0;

	ssdk_psgmii_get_first_phy_address(dev_id, &first_phy_addr);
	if ((first_phy_addr < 0) || (first_phy_addr > MAX_PHY_ADDR)) {
		return;
	}

	if (enable == A_FALSE) {
		ssdk_malibu_psgmii_and_dakota_dess_reset(dev_id, first_phy_addr);
	}

	qca_ar8327_phy_read(dev_id, first_phy_addr + 4, 0x1f, &modectrl_data);
	qca_ar8327_phy_write(dev_id, first_phy_addr + 4, 0x1f, 0x8500);/*switch to access MII reg for copper*/
	for(phy = first_phy_addr; phy < first_phy_addr + 5; phy++) {
		/*enable phy mdio broadcast write*/
		qca_phy_mmd_write(dev_id, phy, 7, 0x8028, 0x801f);
	}

	/* force no link by power down */
	qca_ar8327_phy_write(dev_id, 0x1f, 0x0, 0x1840);

	/*packet number*/
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8021, 0x3000);
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8062, 0x05e0);

	/*fix mdi status */
	qca_ar8327_phy_write(dev_id, 0x1f, 0x10, 0x6800);

	for(i = 0; i < times; i++) {
		phy_t_status = 0;

		for(phy = 0; phy < 5; phy++) {
			value = readl(qca_phy_priv_global[dev_id]->hw_addr + 0x66c + phy * 0xc);
			writel((value|(1<<21)), (qca_phy_priv_global[dev_id]->hw_addr + 0x66c + phy * 0xc));
		}

		for (phy = first_phy_addr; phy < first_phy_addr + 5; phy++) {
			ssdk_psgmii_single_phy_testing(dev_id, phy, enable);
		}
		ssdk_psgmii_all_phy_testing(dev_id, first_phy_addr, enable);
		if (enable == A_FALSE) {
			if (phy_t_status) {
				ssdk_malibu_psgmii_and_dakota_dess_reset(dev_id, first_phy_addr);
			}
			else
			{
		                break;
			}
		}
	}

	*result = phy_t_status;
#ifdef PSGMII_DEBUG
	if (i>=100)
		SSDK_ERROR("PSGMII cannot recover\n");
	else
		SSDK_INFO("PSGMII recovered after %d times reset\n",i);
#endif
	/*configuration recover*/
	/*packet number*/
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8021, 0x0);
	/*disable check*/
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8029, 0x0);
	/*disable traffic*/
	qca_phy_mmd_write(dev_id, 0x1f, 7, 0x8020, 0x0);
}


void clear_self_test_config(a_uint32_t dev_id)
{
	u32 value = 0;
	a_uint32_t first_phy_addr = MAX_PHY_ADDR + 1, phy = 0;

	ssdk_psgmii_get_first_phy_address(dev_id, &first_phy_addr);
	if ((first_phy_addr < 0) || (first_phy_addr > MAX_PHY_ADDR)) {
		return;
	}

	/* disable EEE */
	/* qca_phy_mmd_write(0, 0x1f, 0x7,  0x3c, 0x0); */

	/*disable phy internal loopback*/
	qca_ar8327_phy_write(dev_id, 0x1f, 0x10, 0x6860);
	qca_ar8327_phy_write(dev_id, 0x1f, 0x0, 0x9040);

	for(phy = 0; phy < 5; phy++)
	{
		/*disable mac loop back*/
		value = readl(qca_phy_priv_global[dev_id]->hw_addr+0x66c+phy*0xc);
		writel((value&(~(1<<21))), (qca_phy_priv_global[dev_id]->hw_addr+0x66c+phy*0xc));
	}

	for(phy = first_phy_addr; phy < first_phy_addr + 5; phy++)
	{
		/*diable phy mdio broadcast write*/
		qca_phy_mmd_write(dev_id, phy, 7, 0x8028, 0x001f);

	}

	qca_ar8327_phy_write(dev_id, first_phy_addr + 4, 0x1f, modectrl_data);

	/* clear fdb entry */
	fal_fdb_entry_flush(dev_id,1);
}
#endif
/*qca808x_start*/
sw_error_t
ssdk_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
	sw_error_t rv;

	rv = fal_init(dev_id, cfg);
	if (rv != SW_OK)
		SSDK_ERROR("ssdk fal init failed: %d. \r\n", rv);

	rv = ssdk_phy_driver_init(dev_id, cfg);
	if (rv != SW_OK)
		SSDK_ERROR("ssdk phy init failed: %d. \r\n", rv);

	return rv;
}

sw_error_t
ssdk_cleanup(void)
{
	sw_error_t rv;

	rv = fal_cleanup();
	rv = ssdk_phy_driver_cleanup();

	return rv;
}
/*qca808x_end*/

sw_error_t
ssdk_hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode)
{
    sw_error_t rv;

    rv = hsl_access_mode_set(dev_id, reg_mode);
    return rv;
}

void switch_cpuport_setup(a_uint32_t dev_id)
{
#ifdef IN_PORTCONTROL
	//According to HW suggestion, enable CPU port flow control for Dakota
	fal_port_flowctrl_forcemode_set(dev_id, 0, A_TRUE);
	fal_port_flowctrl_set(dev_id, 0, A_TRUE);
	fal_port_duplex_set(dev_id, 0, FAL_FULL_DUPLEX);
	fal_port_speed_set(dev_id, 0, FAL_SPEED_1000);
	udelay(10);
	fal_port_txmac_status_set(dev_id, 0, A_TRUE);
	fal_port_rxmac_status_set(dev_id, 0, A_TRUE);
#endif
}
#ifdef IN_AQUANTIA_PHY
#ifdef CONFIG_MDIO
static struct mdio_if_info ssdk_mdio_ctl;
#endif
static struct net_device *ssdk_miireg_netdev = NULL;

static int ssdk_miireg_open(struct net_device *netdev)
{
	return 0;
}
static int ssdk_miireg_close(struct net_device *netdev)
{
	return 0;
}

static int ssdk_miireg_do_ioctl(struct net_device *netdev,
			struct ifreq *ifr, int32_t cmd)
{
	int ret = -EINVAL;
#ifdef CONFIG_MDIO
	struct mii_ioctl_data *mii_data = if_mii(ifr);
	ret = mdio_mii_ioctl(&ssdk_mdio_ctl, mii_data, cmd);
#endif
	return ret;
}

static const struct net_device_ops ssdk_netdev_ops = {
	.ndo_open = &ssdk_miireg_open,
	.ndo_stop = &ssdk_miireg_close,
	.ndo_do_ioctl = &ssdk_miireg_do_ioctl,
};

#ifdef CONFIG_MDIO
extern struct mutex switch_mdio_lock;
static int ssdk_miireg_ioctl_read(struct net_device *netdev, int phy_addr, int mmd, uint16_t addr)
{
	a_uint32_t reg = 0;
	a_uint16_t val = 0;

	if (MDIO_DEVAD_NONE == mmd) {
		qca_ar8327_phy_read(0, phy_addr, addr, &val);
		return (int)val;
	}

	reg = MII_ADDR_C45 | mmd << 16 | addr;
	mutex_lock(&switch_mdio_lock);
	qca_ar8327_phy_read(0, phy_addr, reg, &val);
	mutex_unlock(&switch_mdio_lock);

	return (int)val;
}

static int ssdk_miireg_ioctl_write(struct net_device *netdev, int phy_addr, int mmd,
				uint16_t addr, uint16_t value)
{
	a_uint32_t reg = 0;

	if (MDIO_DEVAD_NONE == mmd) {
		qca_ar8327_phy_write(0, phy_addr, addr, value);
		return 0;
	}

	reg = MII_ADDR_C45 | mmd << 16 | addr;
	mutex_lock(&switch_mdio_lock);
	qca_ar8327_phy_write(0, phy_addr, reg, value);
	mutex_unlock(&switch_mdio_lock);

	return 0;
}
#endif

static void ssdk_netdev_setup(struct net_device *dev)
{
	dev->netdev_ops = &ssdk_netdev_ops;
}
static void ssdk_miireg_ioctrl_register(void)
{
	if (ssdk_miireg_netdev)
		return;
#ifdef CONFIG_MDIO
	ssdk_mdio_ctl.mdio_read = ssdk_miireg_ioctl_read;
	ssdk_mdio_ctl.mdio_write = ssdk_miireg_ioctl_write;
	ssdk_mdio_ctl.mode_support = MDIO_SUPPORTS_C45;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,0))
	ssdk_miireg_netdev = alloc_netdev(100, "miireg", 0, ssdk_netdev_setup);
#else
	ssdk_miireg_netdev = alloc_netdev(100, "miireg", ssdk_netdev_setup);
#endif
	if (ssdk_miireg_netdev)
		register_netdev(ssdk_miireg_netdev);
}

static void ssdk_miireg_ioctrl_unregister(void)
{
	if (ssdk_miireg_netdev) {
		unregister_netdev(ssdk_miireg_netdev);
		kfree(ssdk_miireg_netdev);
		ssdk_miireg_netdev = NULL;
	}
}
#endif
static void ssdk_driver_register(a_uint32_t dev_id)
{
	hsl_reg_mode reg_mode;
	a_bool_t flag;

	reg_mode = ssdk_switch_reg_access_mode_get(dev_id);

	if(reg_mode == HSL_REG_LOCAL_BUS) {
#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		platform_driver_register(&ssdk_driver);
#endif
#endif
	}

	flag = ssdk_ess_switch_flag_get(dev_id);
	if(reg_mode == HSL_REG_MDIO && flag == A_FALSE) {
		if(driver_find(qca_phy_driver.name, &mdio_bus_type)){
			SSDK_ERROR("QCA PHY driver had been Registered\n");
			return;
		}

		SSDK_INFO("Register QCA PHY driver\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
		phy_driver_register(&qca_phy_driver, THIS_MODULE);
#else
		phy_driver_register(&qca_phy_driver);
#endif

#ifdef BOARD_AR71XX
#if defined(IN_SWCONFIG)
		ssdk_uci_takeover_init();
#endif

#ifdef CONFIG_AR8216_PHY
		ar8327_port_link_notify_register(ssdk_port_link_notify);
#endif
		ar7240_port_link_notify_register(ssdk_port_link_notify);
#endif
	}
}

static void ssdk_driver_unregister(a_uint32_t dev_id)
{
	hsl_reg_mode reg_mode;
	a_bool_t flag;

	reg_mode= ssdk_switch_reg_access_mode_get(dev_id);
	flag = ssdk_ess_switch_flag_get(dev_id);
	if(reg_mode == HSL_REG_MDIO && flag == A_FALSE) {
		phy_driver_unregister(&qca_phy_driver);

#if defined(BOARD_AR71XX) && defined(IN_SWCONFIG)
		ssdk_uci_takeover_exit();
#endif
	}

	if (reg_mode == HSL_REG_LOCAL_BUS) {
#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		platform_driver_unregister(&ssdk_driver);
#endif
#endif
	}
}
/*qca808x_start*/
static int chip_is_scomphy(a_uint32_t dev_id, ssdk_init_cfg* cfg)
{
	int rv = -ENODEV;
	a_uint32_t phy_id = 0, port_id = 0;
	a_uint32_t port_bmp = qca_ssdk_port_bmp_get(dev_id);
	while (port_bmp) {
		if (port_bmp & 0x1) {
			phy_id = hsl_phyid_get(dev_id, port_id, cfg);
			switch (phy_id) {
/*qca808x_end*/
				case QCA8030_PHY:
				case QCA8033_PHY:
				case QCA8035_PHY:
				case MP_GEPHY:
/*qca808x_start*/
				case QCA8081_PHY_V1_1:
						cfg->chip_type = CHIP_SCOMPHY;
						/*MP GEPHY is always the first port*/
						if(cfg->phy_id == 0)
						{
							cfg->phy_id = phy_id;
						}
						rv = SW_OK;
					break;
				default:
					break;
			}
		}
		port_bmp >>= 1;
		port_id++;
	}

	return rv;
}

static int chip_ver_get(a_uint32_t dev_id, ssdk_init_cfg* cfg)
{
	int rv = SW_OK;
	a_uint8_t chip_ver = 0;
	a_uint8_t chip_revision = 0;
/*qca808x_end*/
	hsl_reg_mode reg_mode;

	reg_mode= ssdk_switch_reg_access_mode_get(dev_id);
	if(reg_mode == HSL_REG_MDIO)
	{
		chip_ver = (qca_ar8216_mii_read(dev_id, 0)&0xff00)>>8;
	}
	else {
		a_uint32_t reg_val = 0;
		qca_switch_reg_read(dev_id,0,(a_uint8_t *)&reg_val, 4);
		chip_ver = (reg_val&0xff00)>>8;
		chip_revision = reg_val&0xff;
	}
/*qca808x_start*/
	if(chip_ver == QCA_VER_AR8227)
		cfg->chip_type = CHIP_SHIVA;
	else if(chip_ver == QCA_VER_AR8337)
		cfg->chip_type = CHIP_ISISC;
	else if(chip_ver == QCA_VER_AR8327)
		cfg->chip_type = CHIP_ISIS;
	else if(chip_ver == QCA_VER_DESS)
		cfg->chip_type = CHIP_DESS;
	else if(chip_ver == QCA_VER_HPPE) {
		cfg->chip_type = CHIP_HPPE;
		cfg->chip_revision = chip_revision;
	}
	else {
		/* try single phy without switch connected */
		rv = chip_is_scomphy(dev_id, cfg);
	}

	return rv;
}
/*qca808x_end*/

#ifdef DESS
static int ssdk_flow_default_act_init(a_uint32_t dev_id)
{
	a_uint32_t vrf_id = 0;
	fal_flow_type_t type = 0;
	for(vrf_id = FAL_MIN_VRF_ID; vrf_id <= FAL_MAX_VRF_ID; vrf_id++)
	{
		for(type = FAL_FLOW_LAN_TO_LAN; type <= FAL_FLOW_WAN_TO_WAN; type++)
		{
#ifdef IN_IP
#ifndef IN_IP_MINI
			fal_default_flow_cmd_set(dev_id, vrf_id, type, FAL_DEFAULT_FLOW_ADMIT_ALL);
#endif
#endif
		}
	}

	return 0;
}
static int ssdk_dess_mac_mode_init(a_uint32_t dev_id, a_uint32_t mac_mode)
{
	a_uint32_t reg_value;
	u8  __iomem      *gcc_addr = NULL;

	switch(mac_mode) {
		case PORT_WRAPPER_PSGMII:
		case PORT_WRAPPER_PSGMII_FIBER:
			reg_value = 0x2200;
			qca_psgmii_reg_write(dev_id, DESS_PSGMII_MODE_CONTROL,
								(a_uint8_t *)&reg_value, 4);
			reg_value = 0x8380;
			qca_psgmii_reg_write(dev_id, DESS_PSGMIIPHY_TX_CONTROL,
								(a_uint8_t *)&reg_value, 4);
			break;
		case PORT_WRAPPER_SGMII0_RGMII5:
		case PORT_WRAPPER_SGMII1_RGMII5:
		case PORT_WRAPPER_SGMII0_RGMII4:
		case PORT_WRAPPER_SGMII1_RGMII4:
		case PORT_WRAPPER_SGMII4_RGMII4:

		/*config sgmii */
			if ((mac_mode == PORT_WRAPPER_SGMII0_RGMII5)
				||(mac_mode == PORT_WRAPPER_SGMII0_RGMII4)) {
				/*PSGMII channnel 0 as SGMII*/
				reg_value = 0x2001;
				fal_psgmii_reg_set(dev_id, 0x1b4,
								(a_uint8_t *)&reg_value, 4);
				udelay(1000);
			}
			if ((mac_mode == PORT_WRAPPER_SGMII1_RGMII5)
				||(mac_mode == PORT_WRAPPER_SGMII1_RGMII4)) {
				/*PSGMII channnel 1 as SGMII*/
				reg_value = 0x2003;
				fal_psgmii_reg_set(dev_id, 0x1b4,
								(a_uint8_t *)&reg_value, 4);
				udelay(1000);
			}
			if ((mac_mode == PORT_WRAPPER_SGMII4_RGMII4)) {
				/*PSGMII channnel 4 as SGMII*/
				reg_value = 0x2005;
				fal_psgmii_reg_set(dev_id, 0x1b4,
								(a_uint8_t *)&reg_value, 4);
				udelay(1000);
			}

			/*clock gen 1*/
			reg_value = 0xea6;
			fal_psgmii_reg_set(dev_id, 0x13c,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);
			/*softreset psgmii, fixme*/
			gcc_addr = ioremap_nocache(0x1812000, 0x200);
			if (!gcc_addr) {
				SSDK_ERROR("gcc map fail!\n");
				return 0;
			} else {
				SSDK_INFO("gcc map success!\n");
				writel(0x20, gcc_addr+0xc);
				mdelay(10);
				writel(0x0, gcc_addr+0xc);
				mdelay(10);
				iounmap(gcc_addr);
			}
			/*relock pll*/
			reg_value = 0x2803;
			fal_psgmii_reg_set(dev_id, DESS_PSGMII_PLL_VCO_RELATED_CONTROL_1,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			reg_value = 0x4ADA;
			fal_psgmii_reg_set(dev_id, DESS_PSGMII_VCO_CALIBRATION_CONTROL_1,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			reg_value = 0xADA;
			fal_psgmii_reg_set(dev_id, DESS_PSGMII_VCO_CALIBRATION_CONTROL_1,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);

			/* Reconfig channel 0 as SGMII and re autoneg*/
			if ((mac_mode == PORT_WRAPPER_SGMII0_RGMII5)
				||(mac_mode == PORT_WRAPPER_SGMII0_RGMII4)) {
			/*PSGMII channnel 0 as SGMII*/
			reg_value = 0x2001;
			fal_psgmii_reg_set(dev_id, 0x1b4,
								(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			/* restart channel 0 autoneg*/
			reg_value = 0xc4;
			fal_psgmii_reg_set(dev_id, 0x1c8,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);
			reg_value = 0x44;
			fal_psgmii_reg_set(dev_id, 0x1c8,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);
			}
			/* Reconfig channel 1 as SGMII and re autoneg*/
			if ((mac_mode == PORT_WRAPPER_SGMII1_RGMII5)
				||(mac_mode == PORT_WRAPPER_SGMII1_RGMII4)) {

			/*PSGMII channnel 1 as SGMII*/
			reg_value = 0x2003;
			fal_psgmii_reg_set(dev_id, 0x1b4,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			/* restart channel 1 autoneg*/
			reg_value = 0xc4;
			fal_psgmii_reg_set(dev_id, 0x1e0,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);
			reg_value = 0x44;
			fal_psgmii_reg_set(dev_id, 0x1e0,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);

			}
			/* Reconfig channel 4 as SGMII and re autoneg*/
			if ((mac_mode == PORT_WRAPPER_SGMII4_RGMII4)) {
			/*PSGMII channnel 4 as SGMII*/
			reg_value = 0x2005;
			fal_psgmii_reg_set(dev_id, 0x1b4,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			/* restart channel 4 autoneg*/
			reg_value = 0xc4;
			fal_psgmii_reg_set(dev_id, 0x228,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);
			reg_value = 0x44;
			fal_psgmii_reg_set(dev_id, 0x228,
							(a_uint8_t *)&reg_value, 4);
			mdelay(10);
			}

		  	/* config RGMII*/
			reg_value = 0x400;
			fal_reg_set(dev_id, 0x4, (a_uint8_t *)&reg_value, 4);
			/* config mac5 RGMII*/
			if ((mac_mode == PORT_WRAPPER_SGMII0_RGMII5)
				||(mac_mode == PORT_WRAPPER_SGMII1_RGMII5)) {
				qca_ar8327_phy_dbg_write(0, 4, 0x5, 0x2d47);
				qca_ar8327_phy_dbg_write(0, 4, 0xb, 0xbc40);
				qca_ar8327_phy_dbg_write(0, 4, 0x0, 0x82ee);
				reg_value = 0x72;
				qca_switch_reg_write(dev_id, 0x90, (a_uint8_t *)&reg_value, 4);
			}
			/* config mac4 RGMII*/
			if ((mac_mode == PORT_WRAPPER_SGMII0_RGMII4)
				||(mac_mode == PORT_WRAPPER_SGMII1_RGMII4)
				||(mac_mode == PORT_WRAPPER_SGMII4_RGMII4)) {
				qca_ar8327_phy_dbg_write(dev_id, 4, 0x5, 0x2d47);
				qca_ar8327_phy_dbg_write(dev_id, 4, 0xb, 0xbc40);
				qca_ar8327_phy_dbg_write(dev_id, 4, 0x0, 0x82ee);
				reg_value = 0x72;
				qca_switch_reg_write(dev_id, 0x8c, (a_uint8_t *)&reg_value, 4);
			}
			break;
		case PORT_WRAPPER_PSGMII_RMII0_RMII1:
		case PORT_WRAPPER_PSGMII_RMII0:
		case PORT_WRAPPER_PSGMII_RMII1:
			reg_value = 0x2200;
			qca_psgmii_reg_write(dev_id, DESS_PSGMII_MODE_CONTROL,
				(a_uint8_t *)&reg_value, 4);
			reg_value = 0x8380;
			qca_psgmii_reg_write(dev_id, DESS_PSGMIIPHY_TX_CONTROL,
					(a_uint8_t *)&reg_value, 4);
			/*switch RMII clock source to gcc_ess_clk,ESS_RGMII_CTRL:0x0C000004,dakota rmii1/rmii0 master mode*/
			if(mac_mode== PORT_WRAPPER_PSGMII_RMII0_RMII1)
				reg_value = 0x3000000;
			if(mac_mode== PORT_WRAPPER_PSGMII_RMII0)
				reg_value = 0x1000000;
			if(mac_mode== PORT_WRAPPER_PSGMII_RMII1)
				reg_value = 0x2000000;
			qca_switch_reg_write(dev_id, 0x4, (a_uint8_t *)&reg_value, 4);
			/*enable RMII MAC5 100M/full*/
			if(mac_mode == PORT_WRAPPER_PSGMII_RMII0_RMII1 || mac_mode == PORT_WRAPPER_PSGMII_RMII0)
			{
				reg_value = 0x7d;
				qca_switch_reg_write(dev_id, 0x90, (a_uint8_t *)&reg_value, 4);
			}

			/*enable RMII MAC4 100M/full*/
			if(mac_mode == PORT_WRAPPER_PSGMII_RMII0_RMII1 || mac_mode == PORT_WRAPPER_PSGMII_RMII1)
			{
				reg_value = 0x7d;
				qca_switch_reg_write(dev_id, 0x8C, (a_uint8_t *)&reg_value, 4);
			}
			/*set QM CONTROL REGISTER FLOW_DROP_CNT as max*/
			reg_value = 0x7f007f;
			qca_switch_reg_write(dev_id, 0x808, (a_uint8_t *)&reg_value, 4);

			/*relock PSGMII PLL*/
			reg_value = 0x2803;
			fal_psgmii_reg_set(dev_id, DESS_PSGMII_PLL_VCO_RELATED_CONTROL_1,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			reg_value = 0x4ADA;
			fal_psgmii_reg_set(dev_id, DESS_PSGMII_VCO_CALIBRATION_CONTROL_1,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			reg_value = 0xADA;
			fal_psgmii_reg_set(dev_id, DESS_PSGMII_VCO_CALIBRATION_CONTROL_1,
							(a_uint8_t *)&reg_value, 4);
			udelay(1000);
			break;
	}

	return 0;
}

#ifdef IN_TRUNK
#define MULTIPLE_WAN_PORT_CNT 2
#define TRUNK_ID_OF_MULTIPLE_WAN_PORTS 0

static a_bool_t
ssdk_dess_multiple_wan_port_check(a_uint32_t dev_id,
	a_uint32_t wan_bitmap)
{
	a_uint32_t port_id = SSDK_PHYSICAL_PORT0, wan_ports_cnt = 0;

	for(port_id = SSDK_PHYSICAL_PORT0; port_id < SSDK_MAX_PORT_NUM;
		port_id++)
	{
		if(BIT(port_id) & wan_bitmap)
		{
			wan_ports_cnt++;
		}
	}
	if(wan_ports_cnt >= MULTIPLE_WAN_PORT_CNT)
	{
		return A_TRUE;
	}
	else
	{
		return A_FALSE;
	}
}

sw_error_t
ssdk_dess_trunk_init(a_uint32_t dev_id, a_uint32_t wan_bitmap)
{
	sw_error_t rv = SW_OK;

	if(ssdk_dess_multiple_wan_port_check(dev_id, wan_bitmap))
	{
		rv = fal_trunk_group_set(dev_id, TRUNK_ID_OF_MULTIPLE_WAN_PORTS,
			A_TRUE, wan_bitmap);
		SW_RTN_ON_ERROR(rv);
	}

	return rv;
}
#endif

static sw_error_t
qca_dess_hw_init(ssdk_init_cfg *cfg, a_uint32_t dev_id)
{
	a_uint32_t reg_value = 0;
	hsl_api_t *p_api;
	a_uint32_t psgmii_result = 0;
	a_uint32_t mac_mode;

	mac_mode = ssdk_dt_global_get_mac_mode(dev_id, 0);
	/*Do Malibu self test to fix packet drop issue firstly*/
	if ((mac_mode == PORT_WRAPPER_PSGMII) ||
	    (mac_mode == PORT_WRAPPER_PSGMII_FIBER)) {
		ssdk_psgmii_self_test(dev_id, A_FALSE, 100, &psgmii_result);
		clear_self_test_config(dev_id);
	} else {
#ifndef BOARD_AR71XX
		ssdk_ess_reset();
#endif
	}

	qca_switch_init(dev_id);
#ifdef IN_PORTVLAN
	ssdk_portvlan_init(dev_id);
#endif

#ifdef IN_PORTVLAN
	fal_port_rxhdr_mode_set(dev_id, 0, FAL_ALL_TYPE_FRAME_EN);
#endif
#ifdef IN_IP
#ifndef IN_IP_MINI
	fal_ip_route_status_set(dev_id, A_TRUE);
#endif
#endif

	ssdk_flow_default_act_init(dev_id);

	/*set normal hash and disable nat/napt*/
	qca_switch_reg_read(dev_id, 0x0e38, (a_uint8_t *)&reg_value, 4);
	reg_value = (reg_value|0x1000000|0x8);
	reg_value &= ~2;
	qca_switch_reg_write(dev_id, 0x0e38, (a_uint8_t *)&reg_value, 4);
#ifdef IN_IP
#ifndef IN_IP_MINI
	fal_ip_vrf_base_addr_set(dev_id, 0, 0);
#endif
#endif

	p_api = hsl_api_ptr_get (dev_id);
	if (p_api && p_api->port_flowctrl_thresh_set)
		p_api->port_flowctrl_thresh_set(dev_id, 0, SSDK_PORT0_FC_THRESH_ON_DFLT,
							SSDK_PORT0_FC_THRESH_OFF_DFLT);

	if (p_api && p_api->ip_glb_lock_time_set)
		p_api->ip_glb_lock_time_set(dev_id, FAL_GLB_LOCK_TIME_100US);


	/*config psgmii,sgmii or rgmii mode for Dakota*/
	ssdk_dess_mac_mode_init(dev_id, cfg->mac_mode);

	/*add BGA Board led contorl*/
	ssdk_dess_led_init(cfg);
#ifdef IN_TRUNK
	SW_RTN_ON_ERROR(ssdk_dess_trunk_init(dev_id, cfg->port_cfg.wan_bmp));
#endif

	return SW_OK;
}
#endif
/*qca808x_start*/
static void ssdk_cfg_default_init(ssdk_init_cfg *cfg)
{
	memset(cfg, 0, sizeof(ssdk_init_cfg));
	cfg->cpu_mode = HSL_CPU_1;
	cfg->nl_prot = 30;
	cfg->reg_func.mdio_set = qca_ar8327_phy_write;
	cfg->reg_func.mdio_get = qca_ar8327_phy_read;
#if defined(IN_PHY_I2C_MODE)
	cfg->reg_func.i2c_set = qca_phy_i2c_write;
	cfg->reg_func.i2c_get = qca_phy_i2c_read;
#endif
/*qca808x_end*/

	cfg->reg_func.header_reg_set = qca_switch_reg_write;
	cfg->reg_func.header_reg_get = qca_switch_reg_read;
	cfg->reg_func.mii_reg_set = qca_ar8216_mii_write;
	cfg->reg_func.mii_reg_get = qca_ar8216_mii_read;
/*qca808x_start*/
}
/*qca808x_end*/

#ifdef IN_RFS
#if defined(CONFIG_RFS_ACCEL)
int ssdk_netdev_rfs_cb(
		struct net_device *dev,
		__be32 src, __be32 dst,
		__be16 sport, __be16 dport,
		u8 proto, u16 rxq_index, u32 action)
{
	return ssdk_rfs_ipct_rule_set(src, dst, sport, dport,
							proto, rxq_index, action);
}
#endif

#ifdef DESS
a_bool_t ssdk_intf_search(
	a_uint8_t *mac, a_uint16_t vid,
	a_uint8_t *ret_index, a_uint8_t *free_index)
{
	a_uint8_t  i = 0;

	for (i = 0; i < SSDK_RFS_INTF_MAX; i++) {
		if (rfs_intf_tbl[i].vid == 0)
			*free_index = i;
		if (!memcmp(rfs_intf_tbl[i].macaddr.uc, mac, 6) &&
			rfs_intf_tbl[i].vid == vid) {
			/* find it */
			*ret_index = i;
			return A_TRUE;
		}
	}

	/* Not find the same entry */
	return A_FALSE;
}

static a_bool_t ssdk_is_raw_dev(struct net_device *dev)
{
	struct device *pdev;

	pdev = dev->dev.parent;
	if (!pdev)
		return A_FALSE;

	if (!strstr(dev_name(pdev), "edma"))
		return A_FALSE;
	else
		return A_TRUE;
}

static a_uint16_t ssdk_raw_dev_vid_get(struct net_device *dev)
{
#ifdef CONFIG_RFS_ACCEL
	const struct net_device_ops *ops;

	ops = dev->netdev_ops;
	if (!ops ||
	    !ops->ndo_get_default_vlan_tag) {
		return 0;
	}
	return ops->ndo_get_default_vlan_tag(dev);
#else
	return 0;
#endif
}

static a_uint16_t ssdk_netdev_vid_get(struct net_device *dev)
{
	struct net_device *pdev;
	a_uint16_t vid = 0;

	if (is_vlan_dev(dev)) {
		pdev = vlan_dev_real_dev(dev);
		if (!ssdk_is_raw_dev(pdev)) {
			SSDK_DEBUG("The device %s is not expected!\n", dev->name);
			return 0;
		}
		vid = vlan_dev_vlan_id(dev);
	} else if (ssdk_is_raw_dev(dev)) {
		vid = ssdk_raw_dev_vid_get(dev);
	} else if (dev->priv_flags & IFF_EBRIDGE) {
		/* Do nothing for bridge */
	} else {
		SSDK_DEBUG("The device %s is not expected!\n", dev->name);
	}

	return vid;
}

static void ssdk_rfs_intf_add(struct net_device *dev)
{
	a_uint8_t *devmac = NULL;
	a_uint16_t vid = 0;
	fal_intf_mac_entry_t intf_entry;
	sw_error_t rv = 0;
	a_uint8_t index0, index1;

	/* get vid */
	vid = ssdk_netdev_vid_get(dev);
	if (vid == 0)
		return;

	/*get mac*/
	devmac = (a_uint8_t*)(dev->dev_addr);

	if (ssdk_intf_search(devmac, vid, &index0, &index1)) {
		/* already exist, ignore */
		return;
	}
	rfs_intf_tbl[index1].vid = vid;
	rfs_intf_tbl[index1].if_idx = dev->ifindex;
	memcpy(&rfs_intf_tbl[index1].macaddr, devmac, ETH_ALEN);

	memset(&intf_entry, 0, sizeof(intf_entry));
	intf_entry.ip4_route = 1;
	intf_entry.ip6_route = 1;
	intf_entry.vid_low = vid;
	intf_entry.vid_high = vid;
	memcpy(&intf_entry.mac_addr, devmac, 6);
	rv = fal_ip_intf_entry_add(0, &intf_entry);
	if (rv) {
		SSDK_ERROR("Faled to add intf entry, rv=%d\n", rv);
		memset(&rfs_intf_tbl[index1], 0, sizeof(ssdk_rfs_intf_t));
		return;
	}

	rfs_intf_tbl[index1].hw_idx = intf_entry.entry_id;

}

static void ssdk_rfs_intf_del(struct net_device *dev)
{
	a_uint8_t i = 0;
	fal_intf_mac_entry_t intf_entry;
	sw_error_t rv = 0;

	for (i = 0; i < SSDK_RFS_INTF_MAX; i++) {
		if ((rfs_intf_tbl[i].if_idx == dev->ifindex) &&
		    (rfs_intf_tbl[i].vid != 0)) {
			intf_entry.entry_id = rfs_intf_tbl[i].hw_idx;
			rv = fal_ip_intf_entry_del(0, 1, &intf_entry);
			if (rv) {
				SSDK_ERROR("Faled to del entry, rv=%d\n", rv);
			} else {
				memset(&rfs_intf_tbl[i], 0, sizeof(ssdk_rfs_intf_t));
			}
			return;
		}
	}
}

static int ssdk_inet_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct net_device *dev = ((struct in_ifaddr *)ptr)->ifa_dev->dev;

	/* Ignore the wireless dev */
#ifdef CONFIG_WIRELESS_EXT
	if (dev->wireless_handlers)
		return NOTIFY_DONE;
	else
#endif
		if (dev->ieee80211_ptr)
			return NOTIFY_DONE;

	switch (event) {
		case NETDEV_DOWN:
			ssdk_rfs_intf_del(dev);
			break;
		case NETDEV_UP:
			ssdk_rfs_intf_add(dev);
			break;
	}
	return NOTIFY_DONE;
}
#endif
#endif

//#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
static int ssdk_dev_event(struct notifier_block *this, unsigned long event, void *ptr)
{
	int rv = 0;
	ssdk_init_cfg cfg;
#ifdef MP
	a_uint32_t port_id = 0, dev_id = 0;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);
	adpt_api_t *p_api = adpt_api_ptr_get(dev_id);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
#else
	struct net_device *dev = (struct net_device *)ptr;
#endif

	ssdk_cfg_default_init(&cfg);
	rv = chip_ver_get(0, &cfg);
	if (rv) {
		SSDK_ERROR("chip verfion get failed\n");
		return NOTIFY_DONE;
	}
	switch (event) {
#ifdef IN_RFS
#if defined(CONFIG_RFS_ACCEL)
		case NETDEV_UP:
			if (strstr(dev->name, "eth")) {
				if (dev->netdev_ops && dev->netdev_ops->ndo_register_rfs_filter) {
					dev->netdev_ops->ndo_register_rfs_filter(dev,
						ssdk_netdev_rfs_cb);
				}
			}
			break;
#endif
#endif
		case NETDEV_CHANGEMTU:
			if(dev->type == ARPHRD_ETHER) {
				if (cfg.chip_type == CHIP_DESS ||
					cfg.chip_type == CHIP_ISIS ||
					cfg.chip_type == CHIP_ISISC) {
					struct net_device *eth_dev = NULL;
					unsigned int mtu= 0;

					if(!strcmp(dev->name, "eth0")) {
						eth_dev = dev_get_by_name(&init_net, "eth1");
					} else if (!strcmp(dev->name, "eth1")) {
						eth_dev = dev_get_by_name(&init_net, "eth0");
					} else {
						return NOTIFY_DONE;
					}
					if (!eth_dev) {
						return NOTIFY_DONE;
					}
					mtu = dev->mtu > eth_dev->mtu ? dev->mtu : eth_dev->mtu;
#ifdef IN_MISC
					fal_frame_max_size_set(0, mtu + 18);
#endif
					dev_put(eth_dev);
				}
			}
			break;
#ifdef MP
		case NETDEV_CHANGE:
			if ((cfg.chip_type == CHIP_SCOMPHY) &&
				(cfg.phy_id == MP_GEPHY)) {
				if ((p_api == NULL) || (p_api->adpt_port_netdev_notify_set == NULL)
					|| (priv == NULL)) {
					SSDK_ERROR("Failed to get pointer\n");
					return NOTIFY_DONE;
				}
				if (dev->phydev != NULL) {
					int addr;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
					addr = dev->phydev->mdio.addr;
#else
					addr = dev->phydev->addr;
#endif
					port_id = qca_ssdk_phy_addr_to_port(priv->device_id,
						addr);
					rv = p_api->adpt_port_netdev_notify_set(priv, port_id);
					if (rv) {
						SSDK_ERROR("netdev change notify failed\n");
						return NOTIFY_DONE;
					}
				}
			}
			break;
#endif
	}

	return NOTIFY_DONE;
}

#ifdef DESS
static void qca_dess_rfs_remove(void)
{
	/* ssdk_dt_global->switch_reg_access_mode == HSL_REG_LOCAL_BUS */
	if(qca_dess_rfs_registered){
#if defined (CONFIG_NF_FLOW_COOKIE)
#ifdef IN_NAT
#ifdef IN_SFE
		sfe_unregister_flow_cookie_cb(ssdk_flow_cookie_set);
#endif
#endif
#endif
#ifdef IN_RFS
		rfs_ess_device_unregister(&rfs_dev);
		unregister_inetaddr_notifier(&ssdk_inet_notifier);
#if defined(CONFIG_RFS_ACCEL)
#endif
#endif
		qca_dess_rfs_registered = false;
	}

}

static void qca_dess_rfs_init(void)
{
	if (!qca_dess_rfs_registered) {
#if defined (CONFIG_NF_FLOW_COOKIE)
#ifdef IN_NAT
#ifdef IN_SFE
		sfe_register_flow_cookie_cb(ssdk_flow_cookie_set);
#endif
#endif
#endif

#ifdef IN_RFS
		memset(&rfs_dev, 0, sizeof(rfs_dev));
		rfs_dev.name = NULL;
#ifdef IN_FDB
		rfs_dev.mac_rule_cb = ssdk_rfs_mac_rule_set;
#endif
#ifdef IN_IP
		rfs_dev.ip4_rule_cb = ssdk_rfs_ip4_rule_set;
		rfs_dev.ip6_rule_cb = ssdk_rfs_ip6_rule_set;
#endif
		rfs_ess_device_register(&rfs_dev);
#if defined(CONFIG_RFS_ACCEL)
#endif
		ssdk_inet_notifier.notifier_call = ssdk_inet_event;
		ssdk_inet_notifier.priority = 1;
		register_inetaddr_notifier(&ssdk_inet_notifier);
#endif
		qca_dess_rfs_registered = true;
	}
}
#endif
/*qca808x_start*/
static void ssdk_free_priv(void)
{
	a_uint32_t dev_id, dev_num = 1;

	if(!qca_phy_priv_global) {
		return;
	}
/*qca808x_end*/
	dev_num = ssdk_switch_device_num_get();
/*qca808x_start*/
	for (dev_id = 0; dev_id < dev_num; dev_id++) {
		if (qca_phy_priv_global[dev_id]) {
			kfree(qca_phy_priv_global[dev_id]);
		}

		qca_phy_priv_global[dev_id] = NULL;
	}

	kfree(qca_phy_priv_global);

	qca_phy_priv_global = NULL;
/*qca808x_end*/
	ssdk_switch_device_num_exit();
/*qca808x_start*/
}

static int ssdk_alloc_priv(a_uint32_t dev_num)
{
	int rev = SW_OK;
	a_uint32_t dev_id = 0;

	qca_phy_priv_global = kzalloc(dev_num * sizeof(struct qca_phy_priv *), GFP_KERNEL);
	if (qca_phy_priv_global == NULL) {
		return -ENOMEM;
	}

	for (dev_id = 0; dev_id < dev_num; dev_id++) {
		qca_phy_priv_global[dev_id] = kzalloc(sizeof(struct qca_phy_priv), GFP_KERNEL);
		if (qca_phy_priv_global[dev_id] == NULL) {
			return -ENOMEM;
		}
/*qca808x_end*/
		qca_phy_priv_global[dev_id]->qca_ssdk_sw_dev_registered = A_FALSE;
		qca_phy_priv_global[dev_id]->ess_switch_flag = A_FALSE;
/*qca808x_start*/
		qca_ssdk_port_bmp_init(dev_id);
		qca_ssdk_phy_info_init(dev_id);
	}

	return rev;
}

#ifndef SSDK_STR
#define SSDK_STR "ssdk"
#endif
#if defined (ISISC) || defined (ISIS)
static void qca_ar8327_gpio_reset(struct qca_phy_priv *priv)
{
	struct device_node *np = NULL;
	const __be32 *reset_gpio;
	a_int32_t len;
	int gpio_num = 0, ret = 0;

	if (priv->ess_switch_flag == A_TRUE)
		np = priv->of_node;
	else
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
		np = priv->phy->mdio.dev.of_node;
#else
		np = priv->phy->dev.of_node;
#endif
	if(!np)
		return;
	gpio_num = of_get_named_gpio(np, "reset_gpio", 0);
	if(gpio_num < 0)
	{
		reset_gpio = of_get_property(np, "reset_gpio", &len);
		if (!reset_gpio )
		{
			SSDK_INFO("reset_gpio node does not exist\n");
			return;
		}
		gpio_num = be32_to_cpup(reset_gpio);
		if(gpio_num <= 0)
		{
			SSDK_INFO("reset gpio doesn't exist\n ");
			return;
		}
	}
	ret = gpio_request(gpio_num, "reset_gpio");
	if(ret)
	{
		SSDK_ERROR("gpio%d request failed, ret:%d\n", gpio_num, ret);
		return;
	}
	gpio_direction_output(gpio_num, SSDK_GPIO_RESET);
	msleep(200);
	gpio_set_value(gpio_num, SSDK_GPIO_RELEASE);
	SSDK_INFO("GPIO%d reset switch done\n", gpio_num);

	gpio_free(gpio_num);

	return;
}
#endif
static int __init regi_init(void)
{
	a_uint32_t num = 0, dev_id = 0, dev_num = 1;
	ssdk_init_cfg cfg;
/*qca808x_end*/
	garuda_init_spec_cfg chip_spec_cfg;
/*qca808x_start*/
	int rv = 0;
/*qca808x_end*/
	/*init switch device num firstly*/
	ssdk_switch_device_num_init();

	dev_num = ssdk_switch_device_num_get();
/*qca808x_start*/
	rv = ssdk_alloc_priv(dev_num);
	if (rv)
		goto out;

	for (num = 0; num < dev_num; num++) {
		ssdk_cfg_default_init(&cfg);
/*qca808x_end*/
#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		if(SW_DISABLE == ssdk_dt_parse(&cfg, num, &dev_id)) {
			SSDK_INFO("ess-switch node is unavalilable\n");
			continue;
		}
#endif
#endif

		/* device id is the array index */
		qca_phy_priv_global[dev_id]->device_id = ssdk_device_id_get(dev_id);
		qca_phy_priv_global[dev_id]->ess_switch_flag = ssdk_ess_switch_flag_get(dev_id);
		qca_phy_priv_global[dev_id]->of_node = ssdk_dts_node_get(dev_id);
/*qca808x_start*/
		rv = ssdk_plat_init(&cfg, dev_id);
		SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
/*qca808x_end*/
		ssdk_driver_register(dev_id);
/*qca808x_start*/
		rv = chip_ver_get(dev_id, &cfg);
		SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
/*qca808x_end*/
#ifdef IN_AQUANTIA_PHY
		ssdk_miireg_ioctrl_register();
#endif
		memset(&chip_spec_cfg, 0, sizeof(garuda_init_spec_cfg));
		cfg.chip_spec_cfg = &chip_spec_cfg;
/*qca808x_start*/
		rv = ssdk_init(dev_id, &cfg);
		SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
/*qca808x_end*/


		switch (cfg.chip_type)
		{
			case CHIP_ISIS:
			case CHIP_ISISC:
#if defined (ISISC) || defined (ISIS)
				if (qca_phy_priv_global[dev_id]->ess_switch_flag == A_TRUE) {
					SSDK_INFO("Initializing ISISC!!\n");
					qca_ar8327_gpio_reset(qca_phy_priv_global[dev_id]);
					rv = ssdk_switch_register(dev_id, cfg.chip_type);
					SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
					rv = qca_ar8327_hw_init(qca_phy_priv_global[dev_id]);
					SSDK_INFO("Initializing ISISC Done!!\n");
				}
#endif
				break;
			case CHIP_HPPE:
#if defined(HPPE)
				SSDK_INFO("Initializing HPPE!!\n");
				qca_hppe_hw_init(&cfg, dev_id);
				rv = ssdk_switch_register(dev_id, cfg.chip_type);
				SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
				SSDK_INFO("Initializing HPPE Done!!\n");
#endif
				break;

			case CHIP_DESS:
#if defined(DESS)
				SSDK_INFO("Initializing DESS!!\n");

				qca_dess_hw_init(&cfg, dev_id);
				qca_dess_rfs_init();

				/* Setup Cpu port for Dakota platform. */
				switch_cpuport_setup(dev_id);
				rv = ssdk_switch_register(dev_id, cfg.chip_type);
				SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
				SSDK_INFO("Initializing DESS Done!!\n");
#endif
				break;

			case CHIP_SHIVA:
			case CHIP_ATHENA:
			case CHIP_GARUDA:
			case CHIP_HORUS:
			case CHIP_UNSPECIFIED:
				break;
			case CHIP_SCOMPHY:
#if defined(SCOMPHY)
					SSDK_INFO("Initializing SCOMPHY!\n");
					rv = qca_scomphy_hw_init(&cfg, dev_id);
					SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
#if defined(MP)
					if(cfg.phy_id == MP_GEPHY)
					{
						rv = ssdk_switch_register(dev_id, cfg.chip_type);
						SW_CNTU_ON_ERROR_AND_COND1_OR_GOTO_OUT(rv, -ENODEV);
					}
#endif
					SSDK_INFO("Initializing SCOMPHY Done!!\n");
#endif
				break;
		}

			fal_module_func_init(dev_id, &cfg);
/*qca808x_start*/

	}
/*qca808x_end*/

	ssdk_sysfs_init();

	if (rv == 0){
		/* register the notifier later should be ok */
		ssdk_dev_notifier.notifier_call = ssdk_dev_event;
		ssdk_dev_notifier.priority = 1;
		register_netdevice_notifier(&ssdk_dev_notifier);
	}
/*qca808x_start*/

out:
	if (rv == 0)
		SSDK_INFO("qca-%s module init succeeded!\n", SSDK_STR);
	else {
		if (rv == -ENODEV) {
			rv = 0;
			SSDK_INFO("qca-%s module init, no device found!\n", SSDK_STR);
		} else {
			SSDK_INFO("qca-%s module init failed! (code: %d)\n", SSDK_STR, rv);
			ssdk_free_priv();
		}
	}

	return rv;
}

static void __exit
regi_exit(void)
{
/*qca808x_end*/
	a_uint32_t dev_id, dev_num = 1;
/*qca808x_start*/
	sw_error_t rv;
/*qca808x_end*/
	dev_num = ssdk_switch_device_num_get();
	for (dev_id = 0; dev_id < dev_num; dev_id++) {
		ssdk_driver_unregister(dev_id);
#if defined(DESS) || defined(HPPE) || defined(ISISC) || defined(ISIS) || defined(MP)
		if (qca_phy_priv_global[dev_id]->qca_ssdk_sw_dev_registered == A_TRUE)
			ssdk_switch_unregister(dev_id);
#endif
	}
/*qca808x_start*/
	rv = ssdk_cleanup();

	if (rv == 0)
		SSDK_INFO("qca-%s module exit  done!\n", SSDK_STR);
	else
		SSDK_ERROR("qca-%s module exit failed! (code: %d)\n", SSDK_STR, rv);
/*qca808x_end*/

#ifdef DESS
	qca_dess_rfs_remove();
#endif

	ssdk_sysfs_exit();
#ifdef IN_AQUANTIA_PHY
	ssdk_miireg_ioctrl_unregister();
#endif
	for (dev_id = 0; dev_id < dev_num; dev_id++) {
		ssdk_plat_exit(dev_id);
	}

	unregister_netdevice_notifier(&ssdk_dev_notifier);
/*qca808x_start*/
	ssdk_free_priv();
}

module_init(regi_init);
module_exit(regi_exit);

MODULE_DESCRIPTION("QCA SSDK Driver");
MODULE_LICENSE("Dual BSD/GPL");
/*qca808x_end*/
