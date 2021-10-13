/*
 * Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
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
#include <linux/types.h>
#if defined(CONFIG_OF)
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/of_gpio.h>
#endif
#include <linux/etherdevice.h>
#include <linux/clk.h>

#include "ssdk_init.h"
#include "ssdk_dts.h"
#include "ssdk_plat.h"
#include "hsl_phy.h"

static ssdk_dt_global_t ssdk_dt_global = {0};
#ifdef HPPE
#ifdef IN_QOS
a_uint8_t ssdk_tm_tick_mode_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->tm_tick_mode;
}

ssdk_dt_scheduler_cfg* ssdk_bootup_shceduler_cfg_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return &cfg->scheduler_cfg;
}
#endif
#endif
#ifdef IN_BM
a_uint8_t ssdk_bm_tick_mode_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->bm_tick_mode;
}
#endif
#ifdef IN_QM
a_uint16_t ssdk_ucast_queue_start_get(a_uint32_t dev_id, a_uint32_t port)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->scheduler_cfg.pool[port].ucastq_start;
}
#endif
a_uint32_t ssdk_intf_mac_num_get(void)
{
	return ssdk_dt_global.num_intf_mac;
}

a_uint8_t* ssdk_intf_macaddr_get(a_uint32_t index)
{
	return ssdk_dt_global.intf_mac[index].uc;
}

a_uint32_t ssdk_dt_global_get_mac_mode(a_uint32_t dev_id, a_uint32_t index)
{
	if (index == 0) {
		return ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode;
	}
	if (index == 1) {
		return ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode1;
	}
	if (index == 2) {
		return ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode2;
	}

	return 0;
}

a_uint32_t ssdk_dt_global_set_mac_mode(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode)
{
	if (index == 0)
	{
		 ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode= mode;
	}
	if (index == 1)
	{
		 ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode1 = mode;
	}
	if (index == 2)
	{
		 ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode2 = mode;
	}

	return 0;
}

a_uint32_t ssdk_cpu_bmp_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->port_cfg.cpu_bmp;
}

a_uint32_t ssdk_lan_bmp_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->port_cfg.lan_bmp;
}

a_uint32_t ssdk_wan_bmp_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->port_cfg.wan_bmp;
}

sw_error_t ssdk_lan_bmp_set(a_uint32_t dev_id, a_uint32_t lan_bmp)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];
	cfg->port_cfg.lan_bmp = lan_bmp;

	return SW_OK;
}

sw_error_t ssdk_wan_bmp_set(a_uint32_t dev_id, a_uint32_t wan_bmp)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];
	cfg->port_cfg.wan_bmp = wan_bmp;

	return SW_OK;
}

a_uint32_t ssdk_inner_bmp_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->port_cfg.inner_bmp;
}

ssdk_port_phyinfo* ssdk_port_phyinfo_get(a_uint32_t dev_id, a_uint32_t port_id)
{
	a_uint32_t i;
	ssdk_port_phyinfo *phyinfo_tmp = NULL;
	ssdk_dt_cfg *cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	for (i = 0; i < cfg->phyinfo_num; i++) {
		if (port_id == cfg->port_phyinfo[i].port_id) {
			phyinfo_tmp = &cfg->port_phyinfo[i];
			break;
		} else if (!(cfg->port_phyinfo[i].phy_features & PHY_F_INIT) &&
				phyinfo_tmp == NULL) {
			phyinfo_tmp = &cfg->port_phyinfo[i];
		}
	}

	return phyinfo_tmp;
}

a_bool_t ssdk_port_feature_get(a_uint32_t dev_id, a_uint32_t port_id, phy_features_t feature)
{
	ssdk_port_phyinfo *phyinfo = ssdk_port_phyinfo_get(dev_id, port_id);
	if (phyinfo && (phyinfo->phy_features & feature)) {
		return A_TRUE;
	}
	return A_FALSE;
}

a_uint32_t ssdk_port_force_speed_get(a_uint32_t dev_id, a_uint32_t port_id)
{
	ssdk_port_phyinfo *phyinfo = ssdk_port_phyinfo_get(dev_id, port_id);
	if (phyinfo && (phyinfo->phy_features & PHY_F_FORCE)) {
		return phyinfo->port_speed;
	}
	return FAL_SPEED_BUTT;
}

struct mii_bus *
ssdk_dts_miibus_get(a_uint32_t dev_id, a_uint32_t phy_addr)
{
	a_uint32_t i;
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	for (i = 0; i < cfg->phyinfo_num; i++) {
		if (phy_addr == cfg->port_phyinfo[i].phy_addr ||
			phy_addr == cfg->port_phyinfo[i].phy_addr+1)
			return cfg->port_phyinfo[i].miibus;
	}

	return NULL;
}

hsl_reg_mode ssdk_switch_reg_access_mode_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->switch_reg_access_mode;
}
#ifdef IN_UNIPHY
hsl_reg_mode ssdk_uniphy_reg_access_mode_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->uniphy_reg_access_mode;
}
#endif
#ifdef DESS
hsl_reg_mode ssdk_psgmii_reg_access_mode_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->psgmii_reg_access_mode;
}
#endif
void ssdk_switch_reg_map_info_get(a_uint32_t dev_id, ssdk_reg_map_info *info)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	info->base_addr = cfg->switchreg_base_addr;
	info->size = cfg->switchreg_size;
}
#ifdef DESS
void ssdk_psgmii_reg_map_info_get(a_uint32_t dev_id, ssdk_reg_map_info *info)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	info->base_addr = cfg->psgmiireg_base_addr;
	info->size = cfg->psgmiireg_size;
}
#endif
#ifdef IN_UNIPHY
void ssdk_uniphy_reg_map_info_get(a_uint32_t dev_id, ssdk_reg_map_info *info)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	info->base_addr = cfg->uniphyreg_base_addr;
	info->size = cfg->uniphyreg_size;
}
#endif
a_bool_t ssdk_ess_switch_flag_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->ess_switch_flag;
}

a_uint32_t ssdk_device_id_get(a_uint32_t index)
{
	return ssdk_dt_global.ssdk_dt_switch_nodes[index]->device_id;
}

struct device_node *ssdk_dts_node_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->of_node;
}

struct clk *ssdk_dts_essclk_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->ess_clk;
}

struct clk *ssdk_dts_cmnclk_get(a_uint32_t dev_id)
{
	ssdk_dt_cfg* cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];

	return cfg->cmnblk_clk;
}

#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
static void ssdk_dt_parse_mac_mode(a_uint32_t dev_id,
		struct device_node *switch_node, ssdk_init_cfg *cfg)
{
	const __be32 *mac_mode;
	a_uint32_t len = 0;

	mac_mode = of_get_property(switch_node, "switch_mac_mode", &len);
	if (!mac_mode)
		SSDK_INFO("mac mode doesn't exit!\n");
	else {
		cfg->mac_mode = be32_to_cpup(mac_mode);
		SSDK_INFO("mac mode = 0x%x\n", be32_to_cpup(mac_mode));
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode = cfg->mac_mode;
	}

	mac_mode = of_get_property(switch_node, "switch_mac_mode1", &len);
	if(!mac_mode)
		SSDK_INFO("mac mode1 doesn't exit!\n");
	else {
		cfg->mac_mode1 = be32_to_cpup(mac_mode);
		SSDK_INFO("mac mode1 = 0x%x\n", be32_to_cpup(mac_mode));
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode1 = cfg->mac_mode1;
	}

	mac_mode = of_get_property(switch_node, "switch_mac_mode2", &len);
	if(!mac_mode)
		SSDK_INFO("mac mode2 doesn't exit!\n");
	else {
		cfg->mac_mode2 = be32_to_cpup(mac_mode);
		SSDK_INFO("mac mode2 = 0x%x\n", be32_to_cpup(mac_mode));
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->mac_mode2 = cfg->mac_mode2;
	}

	return;
}
#ifdef IN_UNIPHY
static void ssdk_dt_parse_uniphy(a_uint32_t dev_id)
{
	struct device_node *uniphy_node = NULL;
	a_uint32_t len = 0;
	const __be32 *reg_cfg;
	ssdk_dt_cfg *cfg;

	/* read uniphy register base and address space */
	uniphy_node = of_find_node_by_name(NULL, "ess-uniphy");
	if (!uniphy_node)
		SSDK_INFO("ess-uniphy DT doesn't exist!\n");
	else {
		SSDK_INFO("ess-uniphy DT exist!\n");
		cfg = ssdk_dt_global.ssdk_dt_switch_nodes[dev_id];
		reg_cfg = of_get_property(uniphy_node, "reg", &len);
		if(!reg_cfg)
			SSDK_INFO("uniphy reg address doesn't exist!\n");
		else {
			cfg->uniphyreg_base_addr = be32_to_cpup(reg_cfg);
			cfg->uniphyreg_size = be32_to_cpup(reg_cfg + 1);
		}
		if (of_property_read_string(uniphy_node, "uniphy_access_mode",
				            (const char **)&cfg->uniphy_access_mode))
			SSDK_INFO("uniphy access mode doesn't exist!\n");
		else {
			if(!strcmp(cfg->uniphy_access_mode, "local bus"))
				cfg->uniphy_reg_access_mode = HSL_REG_LOCAL_BUS;
		}
	}

	return;
}
#endif
#ifdef HPPE
#ifdef IN_QOS
static void ssdk_dt_parse_l1_scheduler_cfg(
	struct device_node *port_node,
	a_uint32_t port_id, a_uint32_t dev_id)
{
	struct device_node *scheduler_node;
	struct device_node *child;
	ssdk_dt_scheduler_cfg *cfg = &(ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->scheduler_cfg);
	a_uint32_t tmp_cfg[4];
	const __be32 *paddr;
	a_uint32_t len, i, sp_id;

	scheduler_node = of_find_node_by_name(port_node, "l1scheduler");
	if (!scheduler_node) {
		SSDK_ERROR("cannot find l1scheduler node for port\n");
		return;
	}
	for_each_available_child_of_node(scheduler_node, child) {
		paddr = of_get_property(child, "sp", &len);
		len /= sizeof(a_uint32_t);
		if (!paddr) {
			SSDK_ERROR("error reading sp property\n");
			return;
		}
		if (of_property_read_u32_array(child,
				"cfg", tmp_cfg, 4)) {
			SSDK_ERROR("error reading cfg property!\n");
			return;
		}
		for (i = 0; i < len; i++) {
			sp_id = be32_to_cpup(paddr+i);
			if (sp_id >= SSDK_L1SCHEDULER_CFG_MAX) {
				SSDK_ERROR("Invalid parameter for sp(%d)\n",
					sp_id);
				return;
			}
			cfg->l1cfg[sp_id].valid = 1;
			cfg->l1cfg[sp_id].port_id = port_id;
			cfg->l1cfg[sp_id].cpri = tmp_cfg[0];
			cfg->l1cfg[sp_id].cdrr_id = tmp_cfg[1];
			cfg->l1cfg[sp_id].epri = tmp_cfg[2];
			cfg->l1cfg[sp_id].edrr_id = tmp_cfg[3];
		}
	}
}

static void ssdk_dt_parse_l0_queue_cfg(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	struct device_node *node,
	a_uint8_t *queue_name,
	a_uint8_t *loop_name)
{
	ssdk_dt_scheduler_cfg *cfg = &(ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->scheduler_cfg);
	a_uint32_t tmp_cfg[5];
	const __be32 *paddr;
	a_uint32_t len, i, queue_id, pri_loop;

	paddr = of_get_property(node, queue_name, &len);
	len /= sizeof(a_uint32_t);
	if (!paddr) {
		SSDK_ERROR("error reading %s property\n", queue_name);
		return;
	}
	if (of_property_read_u32_array(node, "cfg", tmp_cfg, 5)) {
		SSDK_ERROR("error reading cfg property!\n");
		return;
	}
	if (of_property_read_u32(node, loop_name, &pri_loop)) {
		for (i = 0; i < len; i++) {
			queue_id = be32_to_cpup(paddr+i);
			if (queue_id >= SSDK_L0SCHEDULER_CFG_MAX) {
				SSDK_ERROR("Invalid parameter for queue(%d)\n",
					queue_id);
				return;
			}
			cfg->l0cfg[queue_id].valid = 1;
			cfg->l0cfg[queue_id].port_id = port_id;
			cfg->l0cfg[queue_id].sp_id = tmp_cfg[0];
			cfg->l0cfg[queue_id].cpri = tmp_cfg[1];
			cfg->l0cfg[queue_id].cdrr_id = tmp_cfg[2];
			cfg->l0cfg[queue_id].epri = tmp_cfg[3];
			cfg->l0cfg[queue_id].edrr_id = tmp_cfg[4];
		}
	} else {
		/* should one queue for loop */
		if (len != 1) {
			SSDK_ERROR("should one queue for loop!\n");
			return;
		}
		queue_id = be32_to_cpup(paddr);
		if (queue_id >= SSDK_L0SCHEDULER_CFG_MAX) {
			SSDK_ERROR("Invalid parameter for queue(%d)\n",
				queue_id);
			return;
		}
		for (i = 0; i < pri_loop; i++) {
			cfg->l0cfg[queue_id + i].valid = 1;
			cfg->l0cfg[queue_id + i].port_id = port_id;
			cfg->l0cfg[queue_id + i].sp_id = tmp_cfg[0] + i/SSDK_SP_MAX_PRIORITY;
			cfg->l0cfg[queue_id + i].cpri = tmp_cfg[1] + i%SSDK_SP_MAX_PRIORITY;
			cfg->l0cfg[queue_id + i].cdrr_id = tmp_cfg[2] + i;
			cfg->l0cfg[queue_id + i].epri = tmp_cfg[3] + i%SSDK_SP_MAX_PRIORITY;
			cfg->l0cfg[queue_id + i].edrr_id = tmp_cfg[4] + i;
		}
	}
}

static void ssdk_dt_parse_l0_scheduler_cfg(
	struct device_node *port_node,
	a_uint32_t port_id, a_uint32_t dev_id)
{
	struct device_node *scheduler_node;
	struct device_node *child;

	scheduler_node = of_find_node_by_name(port_node, "l0scheduler");
	if (!scheduler_node) {
		SSDK_ERROR("Can't find l0scheduler node for port\n");
		return;
	}
	for_each_available_child_of_node(scheduler_node, child) {
		ssdk_dt_parse_l0_queue_cfg(dev_id, port_id, child,
				"ucast_queue", "ucast_loop_pri");
		ssdk_dt_parse_l0_queue_cfg(dev_id, port_id, child,
				"mcast_queue", "mcast_loop_pri");
	}
}

static void ssdk_dt_parse_scheduler_resource(
	struct device_node *port_node,
	a_uint32_t dev_id, a_uint32_t port_id)
{
	a_uint32_t uq[2], mq[2], l0sp[2], l0cdrr[2];
	a_uint32_t l0edrr[2], l1cdrr[2], l1edrr[2];
	ssdk_dt_scheduler_cfg *cfg = &(ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->scheduler_cfg);

	if (of_property_read_u32_array(port_node, "ucast_queue", uq, 2)
		|| of_property_read_u32_array(port_node, "mcast_queue", mq, 2)
		|| of_property_read_u32_array(port_node, "l0sp", l0sp, 2)
		|| of_property_read_u32_array(port_node, "l0cdrr", l0cdrr, 2)
		|| of_property_read_u32_array(port_node, "l0edrr", l0edrr, 2)
		|| of_property_read_u32_array(port_node, "l1cdrr", l1cdrr, 2)
		|| of_property_read_u32_array(port_node, "l1edrr", l1edrr, 2)){
		SSDK_ERROR("error reading port resource scheduler properties\n");
		return;
	}
	cfg->pool[port_id].ucastq_start = uq[0];
	cfg->pool[port_id].ucastq_end = uq[1];
	cfg->pool[port_id].mcastq_start = mq[0];
	cfg->pool[port_id].mcastq_end = mq[1];
	cfg->pool[port_id].l0sp_start = l0sp[0];
	cfg->pool[port_id].l0sp_end = l0sp[1];
	cfg->pool[port_id].l0cdrr_start = l0cdrr[0];
	cfg->pool[port_id].l0cdrr_end = l0cdrr[1];
	cfg->pool[port_id].l0edrr_start = l0edrr[0];
	cfg->pool[port_id].l0edrr_end = l0edrr[1];
	cfg->pool[port_id].l1cdrr_start = l1cdrr[0];
	cfg->pool[port_id].l1cdrr_end = l1cdrr[1];
	cfg->pool[port_id].l1edrr_start = l1edrr[0];
	cfg->pool[port_id].l1edrr_end = l1edrr[1];
}

static void ssdk_dt_parse_scheduler_cfg(a_uint32_t dev_id, struct device_node *switch_node)
{
	struct device_node *scheduler_node;
	struct device_node *child;
	a_uint32_t port_id;

	scheduler_node = of_find_node_by_name(switch_node, "port_scheduler_resource");
	if (!scheduler_node) {
		SSDK_ERROR("cannot find port_scheduler_resource node\n");
		return;
	}
	for_each_available_child_of_node(scheduler_node, child) {
		if (of_property_read_u32(child, "port_id", &port_id)) {
			SSDK_ERROR("error reading for port_id property!\n");
			return;
		}
		if (port_id >= SSDK_MAX_PORT_NUM) {
			SSDK_ERROR("invalid parameter for port_id(%d)!\n", port_id);
			return;
		}
		ssdk_dt_parse_scheduler_resource(child, dev_id, port_id);
	}

	scheduler_node = of_find_node_by_name(switch_node, "port_scheduler_config");
	if (!scheduler_node) {
		SSDK_ERROR("cannot find port_scheduler_config node\n");
		return ;
	}
	for_each_available_child_of_node(scheduler_node, child) {
		if (of_property_read_u32(child, "port_id", &port_id)) {
			SSDK_ERROR("error reading for port_id property!\n");
			return;
		}
		if (port_id >= SSDK_MAX_PORT_NUM) {
			SSDK_ERROR("invalid parameter for port_id(%d)!\n", port_id);
			return;
		}
		ssdk_dt_parse_l1_scheduler_cfg(child, port_id, dev_id);
		ssdk_dt_parse_l0_scheduler_cfg(child, port_id, dev_id);
	}
}
#endif
#endif
static sw_error_t ssdk_dt_parse_phy_info(struct device_node *switch_node, a_uint32_t dev_id,
		ssdk_init_cfg *cfg)
{
	struct device_node *phy_info_node, *port_node;

	ssdk_port_phyinfo *port_phyinfo;
	a_uint8_t forced_duplex;
	a_uint32_t port_id, phy_addr, phy_i2c_addr, forced_speed, len;
	const __be32 *paddr;
	a_bool_t phy_c45, phy_combo, phy_i2c, phy_forced;
	const char *mac_type = NULL;
	sw_error_t rv = SW_OK;
	struct device_node *mdio_node;
	int phy_reset_gpio = 0;
	phy_dac_t phy_dac = {0};

	phy_info_node = of_get_child_by_name(switch_node, "qcom,port_phyinfo");
	if (!phy_info_node) {
		SSDK_INFO("qcom,port_phyinfo DT doesn't exist!\n");
		return SW_NOT_FOUND;
	}

	for_each_available_child_of_node(phy_info_node, port_node) {
		if (of_property_read_u32(port_node, "port_id", &port_id))
			return SW_BAD_VALUE;

		/* initialize phy_addr in case of undefined dts field */
		phy_addr = 0xff;
		of_property_read_u32(port_node, "phy_address", &phy_addr);

		if (!cfg->port_cfg.wan_bmp) {
			cfg->port_cfg.wan_bmp = BIT(port_id);
		} else {
			cfg->port_cfg.lan_bmp |= BIT(port_id);
		}

		if (!of_property_read_u32(port_node, "forced-speed", &forced_speed) &&
			!of_property_read_u8(port_node, "forced-duplex", &forced_duplex)) {
			phy_forced = A_TRUE;
		} else {
			phy_forced = A_FALSE;
		}
		paddr = of_get_property(port_node, "phy_dac", &len);
		if(paddr)
		{
			phy_dac.mdac = be32_to_cpup(paddr);
			phy_dac.edac = be32_to_cpup(paddr+1);
			hsl_port_phy_dac_set(dev_id, port_id, phy_dac);
		}

		phy_c45 = of_property_read_bool(port_node,
				"ethernet-phy-ieee802.3-c45");
		phy_combo = of_property_read_bool(port_node,
				"ethernet-phy-combo");
		mdio_node = of_parse_phandle(port_node, "mdiobus", 0);
		phy_i2c = of_property_read_bool(port_node, "phy-i2c-mode");

		if (phy_i2c) {
			SSDK_INFO("[PORT %d] phy-i2c-mode\n", port_id);
			hsl_port_phy_access_type_set(dev_id, port_id, PHY_I2C_ACCESS);
			if (of_property_read_u32(port_node, "phy_i2c_address",
						&phy_i2c_addr)) {
				return SW_BAD_VALUE;
			}
			/* phy_i2c_address is the i2c slave addr */
			hsl_phy_address_init(dev_id, port_id, phy_i2c_addr);
			/* phy_address is the mdio addr,
			 * which is a fake mdio addr in i2c mode */
			qca_ssdk_phy_mdio_fake_address_set(dev_id, port_id, phy_addr);
		} else {
			hsl_phy_address_init(dev_id, port_id, phy_addr);
		}

		hsl_port_phy_combo_capability_set(dev_id, port_id, phy_combo);
		hsl_port_phy_c45_capability_set(dev_id, port_id, phy_c45);

		port_phyinfo = ssdk_port_phyinfo_get(dev_id, port_id);
		if (port_phyinfo) {
			port_phyinfo->port_id = port_id;
			port_phyinfo->phy_addr = phy_addr;
			if (phy_c45) {
				port_phyinfo->phy_features |= PHY_F_CLAUSE45;
			}

			if (phy_combo) {
				port_phyinfo->phy_features |= PHY_F_COMBO;
			}

			if (phy_i2c) {
				port_phyinfo->phy_features |= PHY_F_I2C;
			}

			if (phy_forced) {
				port_phyinfo->phy_features |= PHY_F_FORCE;
				port_phyinfo->port_speed = forced_speed;
				port_phyinfo->port_duplex = forced_duplex;
			}

			if (!of_property_read_string(port_node, "port_mac_sel", &mac_type))
			{
				SSDK_INFO("[PORT %d] port_mac_sel = %s\n", port_id, mac_type);
				if (!strncmp("QGMAC_PORT", mac_type, 10)) {
					port_phyinfo->phy_features |= PHY_F_QGMAC;
				}
				else if (!strncmp("XGMAC_PORT", mac_type, 10)) {
					port_phyinfo->phy_features |= PHY_F_XGMAC;
				}
			}

			port_phyinfo->phy_features |= PHY_F_INIT;

			if (mdio_node)
			{
				port_phyinfo->miibus = of_mdio_find_bus(mdio_node);
				phy_reset_gpio = of_get_named_gpio(mdio_node, "phy-reset-gpio",
					SSDK_PHY_RESET_GPIO_INDEX);
				if(phy_reset_gpio > 0)
				{
					SSDK_INFO("port%d's phy-reset-gpio is GPIO%d\n", port_id,
						phy_reset_gpio);
					hsl_port_phy_reset_gpio_set(dev_id, port_id,
						(a_uint32_t)phy_reset_gpio);
				}
			}
		}
	}

	return rv;
}

static void ssdk_dt_parse_mdio(a_uint32_t dev_id, struct device_node *switch_node,
		ssdk_init_cfg *cfg)
{
	struct device_node *mdio_node = NULL;
	struct device_node *child = NULL;
	ssdk_port_phyinfo *port_phyinfo;
	a_uint32_t len = 0, i = 1;
	const __be32 *phy_addr;
	const __be32 *c45_phy;

	/* prefer to get phy info from ess-switch node */
	if (SW_OK == ssdk_dt_parse_phy_info(switch_node, dev_id, cfg))
		return;

	mdio_node = of_find_node_by_name(NULL, "mdio");

	if (!mdio_node) {
		SSDK_INFO("mdio DT doesn't exist!\n");
	}
	else {
		SSDK_INFO("mdio DT exist!\n");
		for_each_available_child_of_node(mdio_node, child) {
			phy_addr = of_get_property(child, "reg", &len);
			if (phy_addr) {
				hsl_phy_address_init(dev_id, i, be32_to_cpup(phy_addr));
			}

			c45_phy = of_get_property(child, "compatible", &len);
			if (c45_phy) {
				hsl_port_phy_c45_capability_set(dev_id, i, A_TRUE);
			}

			port_phyinfo = ssdk_port_phyinfo_get(dev_id, i);
			if (port_phyinfo) {
				port_phyinfo->port_id = i;
				if (phy_addr) {
					port_phyinfo->phy_addr = be32_to_cpup(phy_addr);
				}
				if (c45_phy) {
					port_phyinfo->phy_features |= PHY_F_CLAUSE45;
				}

				port_phyinfo->phy_features |= PHY_F_INIT;
			}

			if (!cfg->port_cfg.wan_bmp) {
				cfg->port_cfg.wan_bmp = BIT(i);
			} else {
				cfg->port_cfg.lan_bmp |= BIT(i);
			}

			i++;
			if (i >= SW_MAX_NR_PORT) {
				break;
			}
		}
	}
	return;
}

static void ssdk_dt_parse_port_bmp(a_uint32_t dev_id,
		struct device_node *switch_node, ssdk_init_cfg *cfg)
{
	a_uint32_t portbmp = 0;

	if (of_property_read_u32(switch_node, "switch_cpu_bmp", &cfg->port_cfg.cpu_bmp)
		|| of_property_read_u32(switch_node, "switch_lan_bmp", &cfg->port_cfg.lan_bmp)
		|| of_property_read_u32(switch_node, "switch_wan_bmp", &cfg->port_cfg.wan_bmp)) {
		SSDK_INFO("port_bmp doesn't exist!\n");
		/*
		 * the bmp maybe initialized already, so just keep ongoing.
		 */
	}

	if (!of_property_read_u32(switch_node, "switch_inner_bmp", &cfg->port_cfg.inner_bmp)) {
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_cfg.inner_bmp =
				cfg->port_cfg.inner_bmp;
	}

	ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_cfg.cpu_bmp = cfg->port_cfg.cpu_bmp;
	ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_cfg.lan_bmp = cfg->port_cfg.lan_bmp;
	ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_cfg.wan_bmp = cfg->port_cfg.wan_bmp;

	portbmp = cfg->port_cfg.lan_bmp | cfg->port_cfg.wan_bmp;
	qca_ssdk_port_bmp_set(dev_id, portbmp);

	return;
}
#ifdef HPPE
static void ssdk_dt_parse_intf_mac(void)
{
	struct device_node *dp_node = NULL;
	a_uint32_t dp = 0;
	a_uint8_t *maddr = NULL;
	char dp_name[8] = {0};

	for (dp = 1; dp <= SSDK_MAX_NR_ETH; dp++) {
		snprintf(dp_name, sizeof(dp_name), "dp%d", dp);
		dp_node = of_find_node_by_name(NULL, dp_name);
		if (!dp_node) {
			continue;
		}
		maddr = (a_uint8_t *)of_get_mac_address(dp_node);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		if (maddr && is_valid_ether_addr(maddr)) {
#else
		if (!IS_ERR(maddr) && is_valid_ether_addr(maddr)) {
#endif
			ssdk_dt_global.num_intf_mac++;
			ether_addr_copy(ssdk_dt_global.intf_mac[dp-1].uc, maddr);
			SSDK_INFO("%s MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				dp_name, maddr[0], maddr[1], maddr[2], maddr[3],
				maddr[4], maddr[5]);
		}
	}
	return;
}
#endif
#ifdef DESS
static void ssdk_dt_parse_psgmii(ssdk_dt_cfg *ssdk_dt_priv)
{

	struct device_node *psgmii_node = NULL;
	const __be32 *reg_cfg;
	a_uint32_t len = 0;

	psgmii_node = of_find_node_by_name(NULL, "ess-psgmii");
	if (!psgmii_node) {
		SSDK_ERROR("cannot find ess-psgmii node\n");
		return;
	}

	SSDK_INFO("ess-psgmii DT exist!\n");
	reg_cfg = of_get_property(psgmii_node, "reg", &len);
	if(!reg_cfg) {
		SSDK_ERROR("%s: error reading device node properties for reg\n",
			        psgmii_node->name);
		return;
	}

	ssdk_dt_priv->psgmiireg_base_addr = be32_to_cpup(reg_cfg);
	ssdk_dt_priv->psgmiireg_size = be32_to_cpup(reg_cfg + 1);
	if (of_property_read_string(psgmii_node, "psgmii_access_mode",
			(const char **)&ssdk_dt_priv->psgmii_reg_access_str)) {
		SSDK_ERROR("%s: error reading properties for psmgii_access_mode\n",
			         psgmii_node->name);
		return;
	}
	if(!strcmp(ssdk_dt_priv->psgmii_reg_access_str, "local bus"))
		ssdk_dt_priv->psgmii_reg_access_mode = HSL_REG_LOCAL_BUS;

	return;
}
#endif
static sw_error_t ssdk_dt_parse_access_mode(struct device_node *switch_node,
		ssdk_dt_cfg *ssdk_dt_priv)
{
	const __be32 *reg_cfg;
	a_uint32_t len = 0;

	if (of_property_read_string(switch_node, "switch_access_mode",
			(const char **)&ssdk_dt_priv->reg_access_mode)) {
		SSDK_ERROR("%s: error reading properties for switch_access_mode\n",
			        switch_node->name);
		return SW_BAD_PARAM;
	}

	SSDK_INFO("switch_access_mode: %s\n", ssdk_dt_priv->reg_access_mode);
	if(!strcmp(ssdk_dt_priv->reg_access_mode, "local bus")) {
		ssdk_dt_priv->switch_reg_access_mode = HSL_REG_LOCAL_BUS;

		reg_cfg = of_get_property(switch_node, "reg", &len);
		if(!reg_cfg) {
			SSDK_ERROR("%s: error reading properties for reg\n",
				switch_node->name);
			return SW_BAD_PARAM;
		}
		ssdk_dt_priv->switchreg_base_addr = be32_to_cpup(reg_cfg);
		ssdk_dt_priv->switchreg_size = be32_to_cpup(reg_cfg + 1);

		SSDK_INFO("switchreg_base_addr: 0x%x\n", ssdk_dt_priv->switchreg_base_addr);
		SSDK_INFO("switchreg_size: 0x%x\n", ssdk_dt_priv->switchreg_size);
	}
	else {
		ssdk_dt_priv->switch_reg_access_mode = HSL_REG_MDIO;
	}

	return SW_OK;

}
#if (defined(DESS) || defined(MP))
#ifdef IN_LED
static void ssdk_dt_parse_led(struct device_node *switch_node,
		ssdk_init_cfg *cfg)
{
	struct device_node *child = NULL;
	const __be32 *led_source, *led_number;
	a_uint8_t *led_str;
	a_uint32_t len = 0, i = 0;

	for_each_available_child_of_node(switch_node, child) {

		led_source = of_get_property(child, "source", &len);
		if (!led_source) {
			continue;
		}
		cfg->led_source_cfg[i].led_source_id = be32_to_cpup(led_source);
		led_number = of_get_property(child, "led", &len);
		if (led_number)
			cfg->led_source_cfg[i].led_num = be32_to_cpup(led_number);
		if (!of_property_read_string(child, "mode", (const char **)&led_str)) {
			if (!strcmp(led_str, "normal"))
			cfg->led_source_cfg[i].led_pattern.mode = LED_PATTERN_MAP_EN;
			if (!strcmp(led_str, "on"))
			cfg->led_source_cfg[i].led_pattern.mode = LED_ALWAYS_ON;
			if (!strcmp(led_str, "blink"))
			cfg->led_source_cfg[i].led_pattern.mode = LED_ALWAYS_BLINK;
			if (!strcmp(led_str, "off"))
			cfg->led_source_cfg[i].led_pattern.mode = LED_ALWAYS_OFF;
		}
		if (!of_property_read_string(child, "speed", (const char **)&led_str)) {
			if (!strcmp(led_str, "10M"))
			cfg->led_source_cfg[i].led_pattern.map = LED_MAP_10M_SPEED;
			if (!strcmp(led_str, "100M"))
			cfg->led_source_cfg[i].led_pattern.map = LED_MAP_100M_SPEED;
			if (!strcmp(led_str, "1000M"))
			cfg->led_source_cfg[i].led_pattern.map = LED_MAP_1000M_SPEED;
			if (!strcmp(led_str, "2500M"))
			cfg->led_source_cfg[i].led_pattern.map = LED_MAP_2500M_SPEED;
			if (!strcmp(led_str, "all"))
			cfg->led_source_cfg[i].led_pattern.map = LED_MAP_ALL_SPEED;
		}
		if (!of_property_read_string(child, "freq", (const char **)&led_str)) {
			if (!strcmp(led_str, "2Hz"))
			cfg->led_source_cfg[i].led_pattern.freq = LED_BLINK_2HZ;
			if (!strcmp(led_str, "4Hz"))
			cfg->led_source_cfg[i].led_pattern.freq = LED_BLINK_4HZ;
			if (!strcmp(led_str, "8Hz"))
			cfg->led_source_cfg[i].led_pattern.freq = LED_BLINK_8HZ;
			if (!strcmp(led_str, "auto"))
			cfg->led_source_cfg[i].led_pattern.freq = LED_BLINK_TXRX;
		}
		if (!of_property_read_string(child, "active", (const char **)&led_str)) {
			if (!strcmp(led_str, "high"))
			cfg->led_source_cfg[i].led_pattern.map |= BIT(LED_ACTIVE_HIGH);
		}
		if (!of_property_read_string(child, "blink_en", (const char **)&led_str)) {
			if (!strcmp(led_str, "disable"))
			cfg->led_source_cfg[i].led_pattern.map &= ~(BIT(RX_TRAFFIC_BLINK_EN)|
				BIT(TX_TRAFFIC_BLINK_EN));
		}
		i++;
	}
	cfg->led_source_num = i;
	SSDK_INFO("current dts led_source_num is %d\n",cfg->led_source_num);

	return;
}
#endif
#endif
static sw_error_t ssdk_dt_get_switch_node(struct device_node **switch_node,
		a_uint32_t num)
{
	struct device_node *switch_instance = NULL;
	char ess_switch_name[64] = {0};

	if (num == 0)
		snprintf(ess_switch_name, sizeof(ess_switch_name), "ess-switch");
	else
		snprintf(ess_switch_name, sizeof(ess_switch_name), "ess-switch%d", num);

	/*
	 * Get reference to ESS SWITCH device node from ess-instance node firstly.
	 */
	switch_instance = of_find_node_by_name(NULL, "ess-instance");
	*switch_node = of_find_node_by_name(switch_instance, ess_switch_name);
	if (!*switch_node) {
		SSDK_WARN("cannot find ess-switch node\n");
		return SW_BAD_PARAM;
	}

	SSDK_INFO("ess-switch DT exist!\n");

	if (!of_device_is_available(*switch_node))
	{
		SSDK_WARN("ess-switch node[%s] is disabled\n", ess_switch_name);
		return SW_DISABLE;
	}

	return SW_OK;
}

sw_error_t ssdk_dt_parse(ssdk_init_cfg *cfg, a_uint32_t num, a_uint32_t *dev_id)
{
	sw_error_t rv = SW_OK;
	struct device_node *switch_node = NULL;
	ssdk_dt_cfg *ssdk_dt_priv = NULL;
	a_uint32_t len = 0;
	const __be32 *device_id;

	rv = ssdk_dt_get_switch_node(&switch_node, num);
	SW_RTN_ON_ERROR(rv);

	device_id = of_get_property(switch_node, "device_id", &len);
	if(!device_id)
		*dev_id = 0;
	else
		*dev_id = be32_to_cpup(device_id);

	ssdk_dt_priv = ssdk_dt_global.ssdk_dt_switch_nodes[*dev_id];
	ssdk_dt_priv->device_id = *dev_id;
	ssdk_dt_priv->ess_switch_flag = A_TRUE;
	ssdk_dt_priv->of_node = switch_node;
	ssdk_dt_priv->ess_clk= ERR_PTR(-ENOENT);
	ssdk_dt_priv->cmnblk_clk = ERR_PTR(-ENOENT);

	if(of_property_read_bool(switch_node,"qcom,emulation")){
		ssdk_dt_priv->is_emulation = A_TRUE;
		SSDK_INFO("RUMI emulation\n");
	}
	/* parse common dts info */
	rv = ssdk_dt_parse_access_mode(switch_node, ssdk_dt_priv);
	SW_RTN_ON_ERROR(rv);
	ssdk_dt_parse_mac_mode(*dev_id, switch_node, cfg);
	ssdk_dt_parse_mdio(*dev_id, switch_node, cfg);
	ssdk_dt_parse_port_bmp(*dev_id, switch_node, cfg);

	if (of_device_is_compatible(switch_node, "qcom,ess-switch")) {
		/* DESS chip */
#ifdef DESS
#ifdef IN_LED
		ssdk_dt_parse_led(switch_node, cfg);
#endif
		ssdk_dt_parse_psgmii(ssdk_dt_priv);

		ssdk_dt_priv->ess_clk = of_clk_get_by_name(switch_node, "ess_clk");
		if (IS_ERR(ssdk_dt_priv->ess_clk))
			SSDK_INFO("ess_clk doesn't exist!\n");
#endif
	}
	else if (of_device_is_compatible(switch_node, "qcom,ess-switch-ipq807x") ||
		 of_device_is_compatible(switch_node, "qcom,ess-switch-ipq60xx")) {
		/* HPPE chip */
#ifdef HPPE
		a_uint32_t mode = 0;
#ifdef IN_UNIPHY
		ssdk_dt_parse_uniphy(*dev_id);
#endif
#ifdef IN_QOS
		ssdk_dt_parse_scheduler_cfg(*dev_id, switch_node);
#endif
		ssdk_dt_parse_intf_mac();

		ssdk_dt_priv->cmnblk_clk = of_clk_get_by_name(switch_node, "cmn_ahb_clk");
		if (!of_property_read_u32(switch_node, "tm_tick_mode", &mode))
			ssdk_dt_priv->tm_tick_mode = mode;
#endif
	}
	else if (of_device_is_compatible(switch_node, "qcom,ess-switch-ipq50xx")) {
#ifdef MP
		ssdk_dt_priv->emu_chip_ver = MP_GEPHY;
#ifdef IN_UNIPHY
		ssdk_dt_parse_uniphy(*dev_id);
#endif
#ifdef IN_LED
		ssdk_dt_parse_led(switch_node, cfg);
#endif
		ssdk_dt_priv->cmnblk_clk = of_clk_get_by_name(switch_node, "cmn_ahb_clk");
#endif
	}
	else if (of_device_is_compatible(switch_node, "qcom,ess-switch-qca83xx")) {
		/* s17/s17c chip */
		SSDK_INFO("switch node is qca83xx!\n");
	}
	else {
		SSDK_WARN("invalid compatible property\n");
	}

	return SW_OK;
}

static a_uint32_t ssdk_get_switch_port_nums(a_uint32_t dev_id)
{
	struct device_node *child, *mdio_np, *port_np, *switch_np, *switch_instance;
	a_uint32_t port_count = 0, switch_id = 0;

	switch_instance = of_find_node_by_name(NULL, "ess-instance");
	if (switch_instance) { /* multi ess-switch */
		for_each_available_child_of_node(switch_instance, switch_np) {
			if (of_property_read_u32(switch_np, "device_id", &switch_id) < 0) {
				switch_id = 0;
			}
			if (switch_id == dev_id) {
				port_np = of_find_node_by_name(switch_np, "qcom,port_phyinfo");
				if (port_np) {
					for_each_available_child_of_node(port_np, child)
						port_count++;
					break;
				}
			}
		}
	} else { /* single ess-switch */
		switch_np = of_find_node_by_name(NULL, "ess-switch");
		if (switch_np && dev_id == 0) {
			port_np = of_find_node_by_name(switch_np, "qcom,port_phyinfo");
			if (port_np) {
				for_each_available_child_of_node(port_np, child)
					port_count++;
			} else {
				mdio_np = of_find_node_by_name(NULL, "mido");
				if (mdio_np) {
					for_each_available_child_of_node(mdio_np, child)
						port_count++;
				}
			}
		}
	}

	return port_count;
}
#endif
#endif

int ssdk_switch_device_num_init(void)
{
	struct device_node *switch_instance = NULL;
	a_uint32_t len = 0, port_n = 0;
	const __be32 *num_devices;
	a_uint32_t dev_num = 1, dev_id = 0;

	switch_instance = of_find_node_by_name(NULL, "ess-instance");
	if (switch_instance) {
		num_devices = of_get_property(switch_instance, "num_devices", &len);
		if (num_devices)
			dev_num = be32_to_cpup(num_devices);
	}

	ssdk_dt_global.ssdk_dt_switch_nodes = kzalloc(dev_num * sizeof(ssdk_dt_cfg *), GFP_KERNEL);
	if (ssdk_dt_global.ssdk_dt_switch_nodes == NULL) {
		return -ENOMEM;
	}

	for (dev_id = 0; dev_id < dev_num; dev_id++) {
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id] = kzalloc(sizeof(ssdk_dt_cfg),
								GFP_KERNEL);
		if (ssdk_dt_global.ssdk_dt_switch_nodes[dev_id] == NULL) {
			return -ENOMEM;
		}

#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
		port_n = ssdk_get_switch_port_nums(dev_id);
#endif
#endif
		if (!port_n) {
			port_n = SW_MAX_NR_PORT;
		}
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_phyinfo = kzalloc(port_n *
				sizeof(ssdk_port_phyinfo), GFP_KERNEL);
		if (!ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_phyinfo) {
			return -ENOMEM;
		}
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->phyinfo_num = port_n;

		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->switch_reg_access_mode = HSL_REG_MDIO;
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->psgmii_reg_access_mode = HSL_REG_MDIO;
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->ess_switch_flag = A_FALSE;
	}

	ssdk_dt_global.num_devices = dev_num;
	SSDK_INFO("ess-switch dts node number: %d\n", dev_num);

	return 0;
}

void ssdk_switch_device_num_exit(void)
{
	a_uint32_t dev_id = 0;

	for (dev_id = 0; dev_id < ssdk_dt_global.num_devices; dev_id++) {
		if (ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_phyinfo) {
			kfree(ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_phyinfo);
			ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->port_phyinfo = NULL;
		}

		if (ssdk_dt_global.ssdk_dt_switch_nodes[dev_id])
			kfree(ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]);
		ssdk_dt_global.ssdk_dt_switch_nodes[dev_id] = NULL;
	}

	if (ssdk_dt_global.ssdk_dt_switch_nodes)
		kfree(ssdk_dt_global.ssdk_dt_switch_nodes);
	ssdk_dt_global.ssdk_dt_switch_nodes = NULL;

	ssdk_dt_global.num_devices = 0;
}

a_uint32_t ssdk_switch_device_num_get(void)
{
	return ssdk_dt_global.num_devices;
}

a_bool_t ssdk_is_emulation(a_uint32_t dev_id)
{
	return ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->is_emulation;
}
a_uint32_t ssdk_emu_chip_ver_get(a_uint32_t dev_id)
{
	return ssdk_dt_global.ssdk_dt_switch_nodes[dev_id]->emu_chip_ver;
}

