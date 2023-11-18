/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include "hal_headers.h"

static void _hal_bus_cap_pre_decision(struct rtw_phl_com_t *phl_com,
					 	void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct bus_sw_cap_t *bus_sw = &phl_com->bus_sw_cap;
	struct bus_hw_cap_t *bus_hw = &hal_com->bus_hw_cap;
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;

#ifdef CONFIG_PCI_HCI
	bus_cap->l0s_ctrl = bus_sw->l0s_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->l0s_ctrl : bus_sw->l0s_ctrl;
	bus_cap->l1_ctrl = bus_sw->l1_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->l1_ctrl : bus_sw->l1_ctrl;
	bus_cap->l1ss_ctrl = bus_sw->l1ss_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->l1ss_ctrl : bus_sw->l1ss_ctrl;
	bus_cap->wake_ctrl = bus_sw->wake_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->wake_ctrl : bus_sw->wake_ctrl;
	bus_cap->crq_ctrl = bus_sw->crq_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->crq_ctrl : bus_sw->crq_ctrl;


	bus_cap->l0sdly_ctrl = bus_sw->l0sdly_ctrl == RTW_PCIE_BUS_ASPM_DLY_IGNORE ?
		bus_hw->l0sdly_ctrl : bus_sw->l0sdly_ctrl;
	bus_cap->l1dly_ctrl = bus_sw->l1dly_ctrl == RTW_PCIE_BUS_ASPM_DLY_IGNORE ?
		bus_hw->l1dly_ctrl : bus_sw->l1dly_ctrl;
	bus_cap->clkdly_ctrl = bus_sw->clkdly_ctrl == RTW_PCIE_BUS_ASPM_DLY_IGNORE ?
		bus_hw->clkdly_ctrl : bus_sw->clkdly_ctrl;

	bus_cap->txbd_num = (bus_sw->txbd_num) ?
		((bus_sw->txbd_num > bus_hw->max_txbd_num) ?
		 bus_hw->max_txbd_num : bus_sw->txbd_num) :
		bus_hw->max_txbd_num;

	/* RXBD NUM */
	bus_cap->rxbd_num = (bus_sw->rxbd_num) ?
		((bus_sw->rxbd_num > bus_hw->max_rxbd_num) ?
		 bus_hw->max_rxbd_num : bus_sw->rxbd_num) :
		bus_hw->max_rxbd_num;
	/* RPBD NUM */
	bus_cap->rpbd_num = (bus_sw->rpbd_num) ?
		((bus_sw->rpbd_num > bus_hw->max_rpbd_num) ?
		 bus_hw->max_rpbd_num : bus_sw->rpbd_num) :
		bus_hw->max_rpbd_num;
	/* RXBUF NUM */
	/* rx buffer number must be larger or equal to rx bd number */
	bus_cap->rxbuf_num = (bus_sw->rxbuf_num < bus_cap->rxbd_num) ?
		 (bus_cap->rxbd_num + RTW_RX_BUF_BD_MIN_DIFF) : bus_sw->rxbuf_num;
	/* RPBUF NUM */
	bus_cap->rpbuf_num = (bus_sw->rpbuf_num < bus_cap->rpbd_num) ?
		 (bus_cap->rpbd_num + RTW_RX_BUF_BD_MIN_DIFF) : bus_sw->rpbuf_num;
	/* RXBUF SIZE */
	bus_cap->rxbuf_size = (bus_sw->rxbuf_size) ?
		((bus_sw->rxbuf_size > bus_hw->max_rxbuf_size) ?
		 bus_hw->max_rxbuf_size : bus_sw->rxbuf_size) :
		bus_hw->max_rxbuf_size;
	/* RPBUF SIZE */
	bus_cap->rpbuf_size = (bus_sw->rpbuf_size) ?
		((bus_sw->rpbuf_size > bus_hw->max_rpbuf_size) ?
		 bus_hw->max_rpbuf_size : bus_sw->rpbuf_size) :
		bus_hw->max_rpbuf_size;

	bus_cap->ltr_act.ctrl = bus_sw->ltr_act.ctrl;
	bus_cap->ltr_act.val = bus_sw->ltr_act.val;
	bus_cap->ltr_idle.ctrl = bus_sw->ltr_idle.ctrl;
	bus_cap->ltr_idle.val = bus_sw->ltr_idle.val;
	bus_cap->ltr_init_state = bus_sw->ltr_init_state;
	bus_cap->ltr_sw_ctrl = bus_hw->ltr_sw_ctrl ?
		(bus_sw->ltr_sw_ctrl ? true : false) : false;
	bus_cap->ltr_hw_ctrl = bus_hw->ltr_hw_ctrl ?
		(bus_sw->ltr_hw_ctrl ? true : false) : false;
#elif defined (CONFIG_USB_HCI)
	bus_cap->tx_buf_size = bus_sw->tx_buf_size ?
		bus_sw->tx_buf_size : bus_hw->tx_buf_size;
	bus_cap->tx_buf_num = bus_sw->tx_buf_num ?
		bus_sw->tx_buf_num : bus_hw->tx_buf_num;
	bus_cap->tx_mgnt_buf_size = bus_sw->tx_mgnt_buf_size ?
		bus_sw->tx_mgnt_buf_size : bus_hw->tx_mgnt_buf_size;
	bus_cap->tx_mgnt_buf_num = bus_sw->tx_mgnt_buf_num ?
		bus_sw->tx_mgnt_buf_num : bus_hw->tx_mgnt_buf_num;
	bus_cap->tx_h2c_buf_num = bus_sw->tx_h2c_buf_num ?
		bus_sw->tx_h2c_buf_num : bus_hw->tx_h2c_buf_num;
	bus_cap->rx_buf_size = bus_sw->rx_buf_size ?
		bus_sw->rx_buf_size : bus_hw->rx_buf_size;
	bus_cap->rx_buf_num = bus_sw->rx_buf_num ?
		bus_sw->rx_buf_num : bus_hw->rx_buf_num;
	bus_cap->in_token_num = bus_sw->in_token_num ?
		bus_sw->in_token_num : bus_hw->in_token_num;
#elif defined(CONFIG_SDIO_HCI)
	bus_cap->tx_buf_size = bus_sw->tx_buf_size ?
				bus_sw->tx_buf_size : bus_hw->tx_buf_size;
	bus_cap->tx_buf_num = bus_sw->tx_buf_num ?
				bus_sw->tx_buf_num : bus_hw->tx_buf_num;
	bus_cap->tx_mgnt_buf_size = bus_sw->tx_mgnt_buf_size ?
			bus_sw->tx_mgnt_buf_size : bus_hw->tx_mgnt_buf_size;
	bus_cap->tx_mgnt_buf_num = bus_sw->tx_mgnt_buf_num ?
			bus_sw->tx_mgnt_buf_num : bus_hw->tx_mgnt_buf_num;
	bus_cap->rx_buf_size = bus_sw->rx_buf_size ?
				bus_sw->rx_buf_size : bus_hw->rx_buf_size;
	bus_cap->rx_buf_num = bus_sw->rx_buf_num ?
				bus_sw->rx_buf_num : bus_hw->rx_buf_num;
#endif
}

#ifdef CONFIG_PCI_HCI
static void _hal_bus_final_cap_decision(struct rtw_phl_com_t *phl_com,
			struct rtw_hal_com_t *hal_com)
{
	struct bus_sw_cap_t *bus_sw = &phl_com->bus_sw_cap;
	struct bus_hw_cap_t *bus_hw = &hal_com->bus_hw_cap;
	struct bus_cap_t *bus_cap = &hal_com->bus_cap;

	bus_cap->l0s_ctrl = bus_sw->l0s_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->l0s_ctrl : bus_sw->l0s_ctrl;
	bus_cap->l1_ctrl = bus_sw->l1_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->l1_ctrl : bus_sw->l1_ctrl;
	bus_cap->l1ss_ctrl = bus_sw->l1ss_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->l1ss_ctrl : bus_sw->l1ss_ctrl;
	bus_cap->wake_ctrl = bus_sw->wake_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->wake_ctrl : bus_sw->wake_ctrl;
	bus_cap->crq_ctrl = bus_sw->crq_ctrl >= RTW_PCIE_BUS_FUNC_DEFAULT ?
		bus_hw->crq_ctrl : bus_sw->crq_ctrl;

	bus_cap->l0sdly_ctrl = bus_sw->l0sdly_ctrl == RTW_PCIE_BUS_ASPM_DLY_IGNORE ?
		bus_hw->l0sdly_ctrl : bus_sw->l0sdly_ctrl;
	bus_cap->l1dly_ctrl = bus_sw->l1dly_ctrl == RTW_PCIE_BUS_ASPM_DLY_IGNORE ?
		bus_hw->l1dly_ctrl : bus_sw->l1dly_ctrl;
	bus_cap->clkdly_ctrl = bus_sw->clkdly_ctrl == RTW_PCIE_BUS_ASPM_DLY_IGNORE ?
		bus_hw->clkdly_ctrl : bus_sw->clkdly_ctrl;

	bus_cap->txbd_num = (bus_sw->txbd_num) ?
		((bus_sw->txbd_num > bus_hw->max_txbd_num) ?
		 bus_hw->max_txbd_num : bus_sw->txbd_num) :
		bus_hw->max_txbd_num;

	/* RXBD NUM */
	bus_cap->rxbd_num = (bus_sw->rxbd_num) ?
		((bus_sw->rxbd_num > bus_hw->max_rxbd_num) ?
		 bus_hw->max_rxbd_num : bus_sw->rxbd_num) :
		bus_hw->max_rxbd_num;
	/* RPBD NUM */
	bus_cap->rpbd_num = (bus_sw->rpbd_num) ?
		((bus_sw->rpbd_num > bus_hw->max_rpbd_num) ?
		 bus_hw->max_rpbd_num : bus_sw->rpbd_num) :
		bus_hw->max_rpbd_num;
	/* RXBUF NUM */
	/* rx buffer number must be larger or equal to rx bd number */
	bus_cap->rxbuf_num = (bus_sw->rxbuf_num < bus_cap->rxbd_num) ?
		 (bus_cap->rxbd_num + RTW_RX_BUF_BD_MIN_DIFF) : bus_sw->rxbuf_num;
	/* RPBUF NUM */
	bus_cap->rpbuf_num = (bus_sw->rpbuf_num < bus_cap->rpbd_num) ?
		 (bus_cap->rpbd_num + RTW_RX_BUF_BD_MIN_DIFF) : bus_sw->rpbuf_num;
	/* RXBUF SIZE */
	bus_cap->rxbuf_size = (bus_sw->rxbuf_size) ?
		((bus_sw->rxbuf_size > bus_hw->max_rxbuf_size) ?
		 bus_hw->max_rxbuf_size : bus_sw->rxbuf_size) :
		bus_hw->max_rxbuf_size;
	/* RPBUF SIZE */
	bus_cap->rpbuf_size = (bus_sw->rpbuf_size) ?
		((bus_sw->rpbuf_size > bus_hw->max_rpbuf_size) ?
		 bus_hw->max_rpbuf_size : bus_sw->rpbuf_size) :
		bus_hw->max_rpbuf_size;

	bus_cap->ltr_act.ctrl = bus_sw->ltr_act.ctrl;
	bus_cap->ltr_act.val = bus_sw->ltr_act.val;
	bus_cap->ltr_idle.ctrl = bus_sw->ltr_idle.ctrl;
	bus_cap->ltr_idle.val = bus_sw->ltr_idle.val;
	bus_cap->ltr_init_state = bus_sw->ltr_init_state;
	bus_cap->ltr_sw_ctrl = bus_hw->ltr_sw_ctrl ?
		(bus_sw->ltr_sw_ctrl ? true : false) : false;
	bus_cap->ltr_hw_ctrl = bus_hw->ltr_hw_ctrl ?
		(bus_sw->ltr_hw_ctrl ? true : false) : false;
}
#endif

static void _hal_ps_final_cap_decision(struct rtw_phl_com_t *phl_com,
			struct rtw_hal_com_t *hal_com)
{
	struct rtw_ps_cap_t *ps_cap = &phl_com->dev_cap.ps_cap;
	struct rtw_ps_cap_t *ps_sw_cap = &phl_com->dev_sw_cap.ps_cap;
	struct rtw_ps_cap_t *ps_hw_cap = &hal_com->dev_hw_cap.ps_cap;

	/* sw */
	ps_cap->init_rf_state = ps_sw_cap->init_rf_state;
	ps_cap->init_rt_stop_rson = ps_sw_cap->init_rt_stop_rson;
	ps_cap->leave_fail_act = ps_sw_cap->leave_fail_act;
	ps_cap->ips_en = ps_sw_cap->ips_en;
	ps_cap->ips_wow_en = ps_sw_cap->ips_wow_en;
	ps_cap->lps_en = ps_sw_cap->lps_en;
	ps_cap->lps_awake_interval = ps_sw_cap->lps_awake_interval;
	ps_cap->lps_listen_bcn_mode = ps_sw_cap->lps_listen_bcn_mode;
	ps_cap->lps_smart_ps_mode = ps_sw_cap->lps_smart_ps_mode;
	ps_cap->lps_bcnnohit_en = ps_sw_cap->lps_bcnnohit_en;
	ps_cap->lps_rssi_enter_threshold = ps_sw_cap->lps_rssi_enter_threshold;
	ps_cap->lps_rssi_leave_threshold = ps_sw_cap->lps_rssi_leave_threshold;
	ps_cap->lps_rssi_diff_threshold = ps_sw_cap->lps_rssi_diff_threshold;
	ps_cap->defer_para.defer_rson= ps_sw_cap->defer_para.defer_rson;
	ps_cap->defer_para.lps_ping_defer_time= ps_sw_cap->defer_para.lps_ping_defer_time;
	ps_cap->defer_para.lps_dhcp_defer_time= ps_sw_cap->defer_para.lps_dhcp_defer_time;
	ps_cap->lps_adv_cap = ps_sw_cap->lps_adv_cap;
	ps_cap->lps_wow_en = ps_sw_cap->lps_wow_en;
	ps_cap->lps_wow_awake_interval = ps_sw_cap->lps_wow_awake_interval;
	ps_cap->lps_wow_listen_bcn_mode = ps_sw_cap->lps_wow_listen_bcn_mode;
	ps_cap->lps_wow_smart_ps_mode = ps_sw_cap->lps_wow_smart_ps_mode;
	ps_cap->lps_wow_bcnnohit_en = ps_sw_cap->lps_wow_bcnnohit_en;
	/* hw */
	ps_cap->lps_pause_tx = ps_hw_cap->lps_pause_tx;
	/* sw & hw */
	ps_cap->ips_cap = (ps_sw_cap->ips_cap & ps_hw_cap->ips_cap);
	ps_cap->ips_wow_cap = (ps_sw_cap->ips_wow_cap & ps_hw_cap->ips_wow_cap);
	ps_cap->lps_cap = (ps_sw_cap->lps_cap & ps_hw_cap->lps_cap);
	ps_cap->lps_wow_cap = (ps_sw_cap->lps_wow_cap & ps_hw_cap->lps_wow_cap);
	ps_cap->bcn_tracking = (ps_sw_cap->bcn_tracking & ps_hw_cap->bcn_tracking);
}

static void _hal_edcca_final_cap_decision(struct rtw_phl_com_t *phl_com,
			struct rtw_hal_com_t *hal_com)
{
	struct rtw_edcca_cap_t *edcca_cap = &phl_com->dev_cap.edcca_cap;
	struct rtw_edcca_cap_t *edcca_sw_cap = &phl_com->dev_sw_cap.edcca_cap;
	struct rtw_edcca_cap_t *edcca_hw_cap = &hal_com->dev_hw_cap.edcca_cap;

	edcca_cap->edcca_adap_th_2g = (edcca_sw_cap->edcca_adap_th_2g)?
					(edcca_sw_cap->edcca_adap_th_2g):
					(edcca_hw_cap->edcca_adap_th_2g);
	edcca_cap->edcca_adap_th_5g = (edcca_sw_cap->edcca_adap_th_5g)?
					(edcca_sw_cap->edcca_adap_th_5g):
					(edcca_hw_cap->edcca_adap_th_5g);

	edcca_cap->edcca_carrier_sense_th = (edcca_sw_cap->edcca_carrier_sense_th)?
						(edcca_sw_cap->edcca_carrier_sense_th):
						(edcca_hw_cap->edcca_carrier_sense_th);

	edcca_cap->edcca_cbp_th_6g = (edcca_sw_cap->edcca_cbp_th_6g)?
					(edcca_sw_cap->edcca_cbp_th_6g):
					(edcca_hw_cap->edcca_cbp_th_6g);
}

void rtw_hal_fw_cap_pre_config(struct rtw_phl_com_t *phl_com, void *hal)
{

	struct dev_cap_t *dev_cap = &phl_com->dev_cap;
	struct dev_cap_t *dev_sw_cap = &phl_com->dev_sw_cap;

	/* leaving for fw hw cap
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct dev_cap_t *dev_hw_cap = &hal_com->dev_hw_cap;
	*/

	dev_cap->fw_cap.fw_src = dev_sw_cap->fw_cap.fw_src;
	dev_cap->fw_cap.dlram_en = dev_sw_cap->fw_cap.dlram_en;
	dev_cap->fw_cap.dlrom_en = dev_sw_cap->fw_cap.dlrom_en;
}

void rtw_hal_bus_cap_pre_config(struct rtw_phl_com_t *phl_com, void *hal)
{
	_hal_bus_cap_pre_decision(phl_com, hal);
}



void rtw_hal_fw_final_cap_config(struct rtw_phl_com_t *phl_com, void *hal)
{

	struct dev_cap_t *dev_cap = &phl_com->dev_cap;
	struct dev_cap_t *dev_sw_cap = &phl_com->dev_sw_cap;

	/* leaving for fw hw cap
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct dev_cap_t *dev_hw_cap = &hal_com->dev_hw_cap;
	*/

	dev_cap->fw_cap.fw_src = dev_sw_cap->fw_cap.fw_src;
	dev_cap->fw_cap.dlram_en = dev_sw_cap->fw_cap.dlram_en;
	dev_cap->fw_cap.dlrom_en = dev_sw_cap->fw_cap.dlrom_en;
}

void rtw_hal_final_cap_decision(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct phy_hw_cap_t *phy_hw = hal_com->phy_hw_cap;
	struct phy_sw_cap_t *phy_sw = phl_com->phy_sw_cap;
	struct phy_cap_t *phy_cap = phl_com->phy_cap;

	struct dev_cap_t *dev_cap = &phl_com->dev_cap;
	struct dev_cap_t *dev_sw_cap = &phl_com->dev_sw_cap;
	struct dev_cap_t *dev_hw_cap = &hal_com->dev_hw_cap;

#ifdef RTW_WKARD_PHY_CAP
	phy_cap[0].proto_sup = phy_sw[0].proto_sup;
	phy_cap[1].proto_sup = phy_sw[1].proto_sup;

	phy_cap[0].txss = (phy_sw[0].txss)?((phy_sw[0].txss > phy_hw[0].tx_num)?
						phy_hw[0].tx_num:phy_sw[0].txss):phy_hw[0].tx_num;
	phy_cap[0].rxss = (phy_sw[0].rxss)?((phy_sw[0].rxss > phy_hw[0].rx_num)?
						phy_hw[0].rx_num:phy_sw[0].rxss):phy_hw[0].rx_num;
	phy_cap[0].tx_path_num = (phy_sw[0].tx_path_num)?((phy_sw[0].tx_path_num > phy_hw[0].tx_path_num)?
						phy_hw[0].tx_path_num:phy_sw[0].tx_path_num):phy_hw[0].tx_path_num;
	phy_cap[0].rx_path_num = (phy_sw[0].rx_path_num)?((phy_sw[0].rx_path_num > phy_hw[0].rx_path_num)?
						phy_hw[0].rx_path_num:phy_sw[0].rx_path_num):phy_hw[0].rx_path_num;
	phy_cap[1].txss = (phy_sw[1].txss)?((phy_sw[1].txss > phy_hw[1].tx_num)?
						phy_hw[1].tx_num:phy_sw[1].txss):phy_hw[1].tx_num;
	phy_cap[1].rxss = (phy_sw[1].rxss)?((phy_sw[1].rxss > phy_hw[1].rx_num)?
						phy_hw[1].rx_num:phy_sw[1].rxss):phy_hw[1].rx_num;
	phy_cap[1].tx_path_num = (phy_sw[1].tx_path_num)?((phy_sw[1].tx_path_num > phy_hw[1].tx_path_num)?
						phy_hw[1].tx_path_num:phy_sw[1].tx_path_num):phy_hw[1].tx_path_num;
	phy_cap[1].rx_path_num = (phy_sw[1].rx_path_num)?((phy_sw[1].rx_path_num > phy_hw[1].rx_path_num)?
						phy_hw[1].rx_path_num:phy_sw[1].rx_path_num):phy_hw[1].rx_path_num;
	phy_cap[0].hw_rts_time_th = (phy_sw[0].hw_rts_time_th)?
			phy_sw[0].hw_rts_time_th:phy_hw[0].hw_rts_time_th;
	phy_cap[1].hw_rts_time_th = (phy_sw[1].hw_rts_time_th)?
			phy_sw[1].hw_rts_time_th:phy_hw[1].hw_rts_time_th;
	phy_cap[0].hw_rts_len_th = (phy_sw[0].hw_rts_len_th)?
			phy_sw[0].hw_rts_len_th:phy_hw[0].hw_rts_len_th;
	phy_cap[1].hw_rts_len_th = (phy_sw[1].hw_rts_len_th)?
			phy_sw[1].hw_rts_len_th:phy_hw[1].hw_rts_len_th;
	/* fw */
	rtw_hal_fw_final_cap_config(phl_com,hal);

	phy_cap[0].txagg_num = (phy_sw[0].txagg_num)?((phy_sw[0].txagg_num > phy_hw[0].txagg_num)?
						phy_hw[0].txagg_num:phy_sw[0].txagg_num):phy_hw[0].txagg_num;
	phy_cap[1].txagg_num = (phy_sw[1].txagg_num)?((phy_sw[1].txagg_num > phy_hw[1].txagg_num)?
						phy_hw[1].txagg_num:phy_sw[1].txagg_num):phy_hw[1].txagg_num;
#endif

	dev_cap->band_sup = (dev_hw_cap->band_sup & dev_sw_cap->band_sup);
	phy_cap[0].band_sup = (dev_cap->band_sup & phy_sw[0].band_sup);
	phy_cap[1].band_sup = (dev_cap->band_sup & phy_sw[1].band_sup);

	dev_cap->bw_sup = (dev_hw_cap->bw_sup & dev_sw_cap->bw_sup);
	phy_cap[0].bw_sup = (dev_cap->bw_sup & phy_sw[0].bw_sup);
	phy_cap[1].bw_sup = (dev_cap->bw_sup & phy_sw[1].bw_sup);

#ifdef RTW_WKARD_LAMODE
	if (dev_sw_cap->la_mode && dev_hw_cap->la_mode)
		dev_cap->la_mode = true;
#endif

	/* wow, to be refined along with hw cap */
	dev_cap->wow_cap.magic_sup = dev_sw_cap->wow_cap.magic_sup;
	dev_cap->wow_cap.pattern_sup = dev_sw_cap->wow_cap.pattern_sup;
	dev_cap->wow_cap.arp_ofld_sup = dev_sw_cap->wow_cap.arp_ofld_sup;
	dev_cap->wow_cap.ns_oflod_sup = dev_sw_cap->wow_cap.ns_oflod_sup;
	dev_cap->wow_cap.gtk_ofld_sup = dev_sw_cap->wow_cap.gtk_ofld_sup;
	dev_cap->wow_cap.ping_pattern_wake_sup = dev_sw_cap->wow_cap.ping_pattern_wake_sup;
	dev_cap->wow_cap.nlo_sup = dev_sw_cap->wow_cap.nlo_sup;

	if (dev_sw_cap->pkg_type != 0xFF)
		dev_cap->pkg_type = dev_sw_cap->pkg_type;
	else
		dev_cap->pkg_type = dev_hw_cap->pkg_type;

	if (dev_sw_cap->rfe_type != 0xFF)
		dev_cap->rfe_type = dev_sw_cap->rfe_type;
	else
		dev_cap->rfe_type = dev_hw_cap->rfe_type;

	dev_cap->bypass_rfe_chk = dev_sw_cap->bypass_rfe_chk;

	hal_com->dbcc_en = false;/*init default value*/
#ifdef CONFIG_DBCC_SUPPORT
	if (dev_sw_cap->dbcc_sup && dev_hw_cap->dbcc_sup) {
		dev_cap->dbcc_sup = true;
		dev_cap->mcmb_skip_dbcc = dev_sw_cap->mcmb_skip_dbcc;
		dev_cap->mcmb_itf_ctrl = dev_sw_cap->mcmb_itf_ctrl;

#ifdef CONFIG_DBCC_FORCE
		if (dev_sw_cap->dbcc_force_rmap) {
			dev_cap->dbcc_force_rmap = dev_sw_cap->dbcc_force_rmap;
			dev_cap->dbcc_force_cck_phyidx = dev_sw_cap->dbcc_force_cck_phyidx;
			hal_com->dbcc_en = true;
			phl_com->mr_ctrl.is_db = true;
		}
#endif
	}
#endif

#ifdef CONFIG_MCC_SUPPORT
	if (dev_sw_cap->mcc_sup && dev_hw_cap->mcc_sup)
		dev_cap->mcc_sup = true;
#endif /*CONFIG_MCC_SUPPORT*/

#ifdef CONFIG_PHL_TWT
	dev_cap->twt_sup = (dev_sw_cap->twt_sup & dev_hw_cap->twt_sup);
#endif /*CONFIG_PHL_TWT*/

	if (dev_sw_cap->hw_hdr_conv && dev_hw_cap->hw_hdr_conv)
		dev_cap->hw_hdr_conv = true;

	if (dev_sw_cap->hw_amsdu && dev_hw_cap->hw_amsdu)
		dev_cap->hw_amsdu = true;

	if (dev_sw_cap->tx_mu_ru && dev_hw_cap->tx_mu_ru)
		dev_cap->tx_mu_ru = true;

	dev_cap->xcap = dev_hw_cap->xcap;
	dev_cap->domain = dev_hw_cap->domain;
	dev_cap->btc_mode = dev_sw_cap->btc_mode;

#ifdef CONFIG_PCI_HCI
	_hal_bus_final_cap_decision(phl_com, hal_com);
#endif

	if (dev_sw_cap->pwrbyrate_off != 0xFF)
		dev_cap->pwrbyrate_off = dev_sw_cap->pwrbyrate_off;
	else
		dev_cap->pwrbyrate_off = PW_BY_RATE_ON;

	if (dev_sw_cap->pwrlmt_type != 0xFF)
		dev_cap->pwrlmt_type = dev_sw_cap->pwrlmt_type;
	else
		dev_cap->pwrlmt_type = PWLMT_BY_EFUSE;

	if (dev_sw_cap->rf_board_opt != 0xFF)
		dev_cap->rf_board_opt = dev_sw_cap->rf_board_opt;
	else
		dev_cap->rf_board_opt = dev_hw_cap->rf_board_opt;

	if (RTW_HW_CAP_ULRU_AUTO == dev_sw_cap->sta_ulru) {
		/* auto : use hw default setting */
		dev_cap->sta_ulru = dev_hw_cap->sta_ulru;
	} else {
		/* forced use sw settings */
		dev_cap->sta_ulru = dev_sw_cap->sta_ulru;
	}
	PHL_INFO("sta_ulru final : 0x%x ; sw 0x%x ; hw 0x%x\n",
		 dev_cap->sta_ulru, dev_sw_cap->sta_ulru, dev_hw_cap->sta_ulru);

#ifdef RTW_WKARD_BB_DISABLE_STA_2G40M_ULOFDMA
	/* 2.4G 40MHz UL OFDAM : bb ask to disable */
	if (RTW_HW_CAP_ULRU_AUTO == dev_sw_cap->sta_ulru_2g40mhz) {
		/* auto : use hw default setting */
		dev_cap->sta_ulru_2g40mhz = dev_hw_cap->sta_ulru_2g40mhz;
	} else {
		/* forced use sw settings */
		dev_cap->sta_ulru_2g40mhz = dev_sw_cap->sta_ulru_2g40mhz;
	}
	PHL_INFO("sta_ulru_2g40mhz final : 0x%x ; sw 0x%x ; hw 0x%x\n",
		 dev_cap->sta_ulru_2g40mhz, dev_sw_cap->sta_ulru_2g40mhz, dev_hw_cap->sta_ulru_2g40mhz);
#endif

	_hal_ps_final_cap_decision(phl_com, hal_com);

	_hal_edcca_final_cap_decision(phl_com, hal_com);

	if (dev_hw_cap->hw_stype_cap != EFUSE_HW_STYPE_GENERAL)
		dev_cap->hw_stype_cap = dev_hw_cap->hw_stype_cap;
	else
		dev_cap->hw_stype_cap = EFUSE_HW_STYPE_NONE;

	if (dev_hw_cap->wl_func_cap != EFUSE_WL_FUNC_GENERAL)
		dev_cap->wl_func_cap = dev_hw_cap->wl_func_cap;
	else
		dev_cap->wl_func_cap = EFUSE_WL_FUNC_NONE;

	/* get rfk cap from sw settings */
	if (dev_sw_cap->rfk_cap != 0xFFFFFFFF)
		dev_cap->rfk_cap = dev_sw_cap->rfk_cap;
	else
		dev_cap->rfk_cap = dev_hw_cap->rfk_cap;

	dev_cap->rpq_agg_num = dev_sw_cap->rpq_agg_num ?
		dev_sw_cap->rpq_agg_num : dev_hw_cap->rpq_agg_num;

	/* MAC_AX_QTA_SCC_TURBO, decide by sw, need to be refined after we have hw cap */
	dev_cap->quota_turbo = dev_sw_cap->quota_turbo;

	dev_cap->sgi_160_sup = dev_hw_cap->sgi_160_sup;

	/* secruity cap */
	if (dev_sw_cap->sec_cap.hw_form_hdr && dev_hw_cap->sec_cap.hw_form_hdr)
		dev_cap->sec_cap.hw_form_hdr = true;
	if (dev_sw_cap->sec_cap.hw_tx_search_key && dev_hw_cap->sec_cap.hw_tx_search_key)
		dev_cap->sec_cap.hw_tx_search_key = true;
	if (dev_sw_cap->sec_cap.hw_sec_iv && dev_hw_cap->sec_cap.hw_sec_iv)
		dev_cap->sec_cap.hw_sec_iv = true;

	if (dev_sw_cap->max_link_num != 0) {
		dev_cap->max_link_num = (dev_sw_cap->max_link_num < dev_hw_cap->max_link_num) ?
		                        dev_sw_cap->max_link_num :
		                        dev_hw_cap->max_link_num;
	} else {
		dev_cap->max_link_num = dev_hw_cap->max_link_num;
	}

	if (dev_sw_cap->max_mld_num != 0) {
		dev_cap->max_mld_num = (dev_sw_cap->max_mld_num < dev_hw_cap->max_mld_num) ?
		                       dev_sw_cap->max_mld_num :
		                       dev_hw_cap->max_mld_num;
	} else {
		dev_cap->max_mld_num = dev_hw_cap->max_mld_num;
	}

	dev_cap->macid_num = MIN(dev_sw_cap->macid_num, dev_hw_cap->macid_num);

	dev_cap->bfee_rx_ndp_sts = dev_hw_cap->bfee_rx_ndp_sts; /* direct follow hw cap */

	if (RTW_DEV_CAP_DISABLE == dev_sw_cap->drv_info_sup) {
		/* sw forced disable */
		dev_cap->drv_info_sup = RTW_DEV_CAP_DISABLE;
	} else {
		dev_cap->drv_info_sup = dev_hw_cap->drv_info_sup;
	}
#ifdef CONFIG_PHL_THERMAL_PROTECT
	dev_cap->min_tx_duty = dev_sw_cap->min_tx_duty;
	dev_cap->thermal_threshold = dev_sw_cap->thermal_threshold;
#endif
	dev_cap->nb_config = dev_sw_cap->nb_config;
}

/**
 * rtw_hal_get_bf_proto_cap
 * input:
 * @phl: (struct phl_info_t *phl_info)
 * @band:
 * input_output:
 * @proto_cap: (struct protocol_cap_t *proto_cap) the variable to save the bf_cap result.
 * return:
 * @rtw_hal_status:
 */
enum rtw_hal_status
rtw_hal_get_bf_proto_cap(struct rtw_phl_com_t *phl_com, void *hal,
			 u8 band, struct protocol_cap_t *proto_cap)
{
#ifdef RTW_WKARD_PHY_CAP
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct protocol_cap_t *hw_cap = NULL;
	struct protocol_cap_t *sw_cap = NULL;

	if (band >= HW_BAND_MAX) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "rtw_hal_get_bf_proto_cap : invalid band index \n");
		return RTW_HAL_STATUS_FAILURE;
	}
	if (proto_cap == NULL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "rtw_hal_get_bf_proto_cap : input proto_cap is NULL \n");
		return RTW_HAL_STATUS_FAILURE;
	}


	hw_cap = &hal_com->proto_hw_cap[band];
	sw_cap = &phl_com->proto_sw_cap[band];

	proto_cap->he_su_bfmr =
		(sw_cap->he_su_bfmr && hw_cap->he_su_bfmr) ? 1 : 0;
	proto_cap->he_su_bfme =
		(sw_cap->he_su_bfme && hw_cap->he_su_bfme) ? 1 : 0;
	proto_cap->he_mu_bfmr =
		(sw_cap->he_mu_bfmr && hw_cap->he_mu_bfmr) ? 1 : 0;
	proto_cap->he_mu_bfme =
		(sw_cap->he_mu_bfme && hw_cap->he_mu_bfme) ? 1 : 0;
	proto_cap->non_trig_cqi_fb =
		(sw_cap->non_trig_cqi_fb && hw_cap->non_trig_cqi_fb) ? 1 : 0;
	proto_cap->trig_cqi_fb =
		(sw_cap->trig_cqi_fb && hw_cap->trig_cqi_fb) ? 1 : 0;

	proto_cap->vht_su_bfme =
		(sw_cap->vht_su_bfme && hw_cap->vht_su_bfme) ? 1 : 0;
	proto_cap->vht_su_bfmr =
		(sw_cap->vht_su_bfmr && hw_cap->vht_su_bfmr) ? 1 : 0;
	proto_cap->vht_mu_bfme =
		(sw_cap->vht_mu_bfme && hw_cap->vht_mu_bfme) ? 1 : 0;
	proto_cap->vht_mu_bfmr =
		(sw_cap->vht_mu_bfmr && hw_cap->vht_mu_bfmr) ? 1 : 0;

	proto_cap->ht_su_bfme =
		(sw_cap->ht_su_bfme && hw_cap->ht_su_bfme) ? 1 : 0;
	proto_cap->ht_su_bfmr =
		(sw_cap->ht_su_bfmr && hw_cap->ht_su_bfmr) ? 1 : 0;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->he_su_bfmr = 0x%x \n",
		  proto_cap->he_su_bfmr);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->he_su_bfme = 0x%x \n",
		  proto_cap->he_su_bfme);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->he_mu_bfmr = 0x%x \n",
		  proto_cap->he_mu_bfmr);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->he_mu_bfme = 0x%x \n",
		  proto_cap->he_mu_bfme);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->non_trig_cqi_fb = 0x%x \n",
		  proto_cap->non_trig_cqi_fb);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->trig_cqi_fb = 0x%x \n",
		  proto_cap->trig_cqi_fb);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->vht_su_bfme = 0x%x \n",
		  proto_cap->vht_su_bfme);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->vht_su_bfmr = 0x%x \n",
		  proto_cap->vht_su_bfmr);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->vht_mu_bfme = 0x%x \n",
		  proto_cap->vht_mu_bfme);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->vht_mu_bfmr = 0x%x \n",
		  proto_cap->vht_mu_bfmr);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->ht_su_bfme = 0x%x \n",
		  proto_cap->ht_su_bfme);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "rtw_hal_get_bf_proto_cap : proto_cap->ht_su_bfmr = 0x%x \n",
		  proto_cap->ht_su_bfmr);
#endif
	return RTW_HAL_STATUS_SUCCESS;
}


/**
 * rtw_hal_get_stbc_proto_cap() - Get the intersection proto_cap of the proto_hw_cap and proto_sw_cap
 * @phl_com: input, the pointer of phl_com (struct rtw_phl_com_t)
 * @hal: input, the pointer of hal (struct hal_info_t)
 * @band: input, the band index
 * @proto_cap: input_output (struct protocol_cap_t)
 * 	       The variable pointer to save the ouput proto_cap results.
 * 	       Here, we update STBC cap into STBC fileds of proto_cap.
 * Return: return the result of getting proto_cap (enum @rtw_hal_status)
 */
enum rtw_hal_status
rtw_hal_get_stbc_proto_cap(struct rtw_phl_com_t *phl_com, void *hal,
			 u8 band, struct protocol_cap_t *proto_cap)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct protocol_cap_t *hw_cap = NULL;
	struct protocol_cap_t *sw_cap = NULL;

	if (band >= HW_BAND_MAX) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "rtw_hal_get_stbc_proto_cap : invalid band index \n");
		return RTW_HAL_STATUS_FAILURE;
	}
	if (proto_cap == NULL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "rtw_hal_get_stbc_proto_cap : input proto_cap is NULL \n");
		return RTW_HAL_STATUS_FAILURE;
	}

	hw_cap = &hal_com->proto_hw_cap[band];
	sw_cap = &phl_com->proto_sw_cap[band];

	proto_cap->stbc_ht_tx =
		(sw_cap->stbc_ht_tx && hw_cap->stbc_ht_tx) ? 1 : 0;
	proto_cap->stbc_vht_tx =
		(sw_cap->stbc_vht_tx && hw_cap->stbc_vht_tx) ? 1 : 0;
	proto_cap->stbc_he_tx =
		(sw_cap->stbc_he_tx && hw_cap->stbc_he_tx) ? 1 : 0;
	proto_cap->stbc_tx_greater_80mhz =
		(sw_cap->stbc_tx_greater_80mhz && hw_cap->stbc_tx_greater_80mhz) ? 1 : 0;

	proto_cap->stbc_ht_rx =
		(sw_cap->stbc_ht_rx <= hw_cap->stbc_ht_rx) ?
			sw_cap->stbc_ht_rx : hw_cap->stbc_ht_rx;
	proto_cap->stbc_vht_rx =
		(sw_cap->stbc_vht_rx <= hw_cap->stbc_vht_rx) ?
			sw_cap->stbc_vht_rx : hw_cap->stbc_vht_rx;
	proto_cap->stbc_he_rx =
		(sw_cap->stbc_he_rx && hw_cap->stbc_he_rx) ? 1 : 0;
	proto_cap->stbc_rx_greater_80mhz =
		(sw_cap->stbc_rx_greater_80mhz && hw_cap->stbc_rx_greater_80mhz) ? 1 : 0;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_ht_tx = 0x%x \n",
		  __func__, proto_cap->stbc_ht_tx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_vht_tx = 0x%x \n",
		  __func__, proto_cap->stbc_vht_tx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_he_tx = 0x%x \n",
		  __func__, proto_cap->stbc_he_tx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_tx_greater_80mhz = 0x%x \n",
		  __func__, proto_cap->stbc_tx_greater_80mhz);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_ht_rx = 0x%x \n",
		  __func__, proto_cap->stbc_ht_rx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_vht_rx = 0x%x \n",
		  __func__, proto_cap->stbc_vht_rx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_he_rx = 0x%x \n",
		  __func__, proto_cap->stbc_he_rx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		  "%s : proto_cap->stbc_rx_greater_80mhz = 0x%x \n",
		  __func__, proto_cap->stbc_rx_greater_80mhz);

	return RTW_HAL_STATUS_SUCCESS;
}
