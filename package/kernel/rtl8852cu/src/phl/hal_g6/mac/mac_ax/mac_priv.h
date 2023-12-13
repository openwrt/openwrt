/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_AX_MAC_PRIV_H_
#define _MAC_AX_MAC_PRIV_H_

#include "../mac_def.h"
#include "cmac_tx.h"
#if MAC_AX_SDIO_SUPPORT
#include "_sdio.h"
#endif

#define get_priv(adapter) \
	((struct mac_ax_priv_ops **)(((struct mac_ax_adapter *)(adapter) + 1)))

#define adapter_to_priv_ops(adapter) \
	(*(get_priv(adapter)))

struct mac_ax_pcie_desa {
	u32 high;
	u32 low;
};

struct mac_ax_pcie_bd_desa_offset {
	struct mac_ax_pcie_desa ach0;
	struct mac_ax_pcie_desa ach1;
	struct mac_ax_pcie_desa ach2;
	struct mac_ax_pcie_desa ach3;
	struct mac_ax_pcie_desa ach4;
	struct mac_ax_pcie_desa ach5;
	struct mac_ax_pcie_desa ach6;
	struct mac_ax_pcie_desa ach7;
	struct mac_ax_pcie_desa b0mg;
	struct mac_ax_pcie_desa b0hi;
	struct mac_ax_pcie_desa b1mg;
	struct mac_ax_pcie_desa b1hi;
	struct mac_ax_pcie_desa h2c;
	struct mac_ax_pcie_desa rxq;
	struct mac_ax_pcie_desa rpq;
};

struct mac_ax_h2creg_offset {
	u32 data0;
	u32 data1;
	u32 data2;
	u32 data3;
	u32 ctrl;
};

struct mac_ax_c2hreg_offset {
	u32 data0;
	u32 data1;
	u32 data2;
	u32 data3;
	u32 ctrl;
};

struct mac_ax_priv_ops {
	/*init*/
	u32 (*pwr_off)(struct mac_ax_adapter *adapter);
	u32 (*pwr_on)(struct mac_ax_adapter *adapter);
	u32 (*intf_pwr_switch)(void *vadapter,
			       u8 pre_switch, u8 on);
	u32 (*dmac_func_en)(struct mac_ax_adapter *adapter);
	u32 (*dmac_func_pre_en)(struct mac_ax_adapter *adapter);
	/*CMAC*/
	u32 (*init_cctl_info)(struct mac_ax_adapter *adapter, u8 macid);
	u32 (*cmac_init)(struct mac_ax_adapter *adapter,
			 struct mac_ax_trx_info *info, enum mac_ax_band band);
	u32 (*cmac_func_en)(struct mac_ax_adapter *adapter, u8 band, u8 en);
	u32 (*macid_idle_ck)(struct mac_ax_adapter *adapter,
			     struct mac_role_tbl *role);
	u32 (*stop_sch_tx)(struct mac_ax_adapter *adapter, enum sch_tx_sel sel,
			   struct mac_ax_sch_tx_en_cfg *bak);
	/*EFUSE*/
	u32 (*switch_efuse_bank)(struct mac_ax_adapter *adapter,
				 enum mac_ax_efuse_bank bank);
	void (*enable_efuse_sw_pwr_cut)(struct mac_ax_adapter *adapter,
					bool is_write);
	void (*disable_efuse_sw_pwr_cut)(struct mac_ax_adapter *adapter,
					 bool is_write);
	/*H2C*/
	struct mac_ax_h2creg_offset *
	(*get_h2creg_offset)(struct mac_ax_adapter *adapter);
	struct mac_ax_c2hreg_offset *
	(*get_c2hreg_offset)(struct mac_ax_adapter *adapter);
	u32 (*ser_imr_config)(struct mac_ax_adapter *adapter, u8 band,
			      enum mac_ax_hwmod_sel sel);
	u32 (*disconnect_flush_key)(struct mac_ax_adapter *adapter,
				    struct mac_role_tbl *role);
	u32 (*sec_info_tbl_init)(struct mac_ax_adapter *adapter);
	u32 (*free_sec_info_tbl)(struct mac_ax_adapter *adapter);
	u32 (*mac_wowlan_secinfo)(struct mac_ax_adapter *adapter,
				  struct mac_ax_sec_iv_info *sec_iv_info);
	u32 (*get_rrsr_cfg)(struct mac_ax_adapter *adapter,
			    struct mac_ax_rrsr_cfg *cfg);
	u32 (*set_rrsr_cfg)(struct mac_ax_adapter *adapter,
			    struct mac_ax_rrsr_cfg *cfg);
	u32 (*get_cts_rrsr_cfg)(struct mac_ax_adapter *adapter,
				struct mac_ax_cts_rrsr_cfg *cfg);
	u32 (*set_cts_rrsr_cfg)(struct mac_ax_adapter *adapter,
				struct mac_ax_cts_rrsr_cfg *cfg);
	u32 (*cfg_gnt)(struct mac_ax_adapter *adapter,
		       struct mac_ax_coex_gnt *gnt);
	u32 (*cfg_ctrl_path)(struct mac_ax_adapter *adapter, u32 wl);
	u32 (*get_gnt)(struct mac_ax_adapter *adapter,
		       struct mac_ax_coex_gnt *gnt);
	u32 (*get_ctrl_path)(struct mac_ax_adapter *adapter, u32 *wl);
	u32 (*get_bbrpt_dle_cfg)(struct mac_ax_adapter *adapter,
				 u8 is_wlan_cpu, u32 *port_id, u32 *queue_id);
	u32 (*dbg_port_sel)(struct mac_ax_adapter *adapter,
			    struct mac_ax_dbg_port_info **info, u32 sel);
	u32 (*tx_flow_ptcl_dbg_port)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*tx_flow_sch_dbg_port)(struct mac_ax_adapter *adapter, u8 band);
	u32 (*ss_stat_chk)(struct mac_ax_adapter *adapter);
	/* Debug Dump*/
	u32 (*dmac_dbg_dump)(struct mac_ax_adapter *adapter);
	u32 (*cmac_dbg_dump)(struct mac_ax_adapter *adapter, enum mac_ax_band band);
	u32 (*crit_dbg_dump)(struct mac_ax_adapter *adapter);
	u32 (*tx_dbg_dump)(struct mac_ax_adapter *adapter);
	u32 (*coex_mac_init)(struct mac_ax_adapter *adapter);
	u32 (*set_fc_page_ctrl_reg)(struct mac_ax_adapter *adapter, u8 ch);
	u32 (*get_fc_page_info)(struct mac_ax_adapter *adapter, u8 ch);
	u32 (*set_fc_pubpg)(struct mac_ax_adapter *adapter);
	u32 (*get_fc_mix_info)(struct mac_ax_adapter *adapter);
	u32 (*set_fc_h2c)(struct mac_ax_adapter *adapter);
	u32 (*set_fc_mix_cfg)(struct mac_ax_adapter *adapter);
	u32 (*set_fc_func_en)(struct mac_ax_adapter *adapter, u8 en, u8 h2c_en);
	u32 (*dle_dfi_ctrl)(struct mac_ax_adapter *adapter, struct dle_dfi_ctrl_t *ctrl_p);
	u32 (*dle_is_txq_empty)(struct mac_ax_adapter *adapter, u8 *val);
	u32 (*dle_is_rxq_empty)(struct mac_ax_adapter *adapter, u8 *val);
	u32 (*preload_cfg_set)(struct mac_ax_adapter *adapter, enum mac_ax_band band,
			       struct mac_ax_preld_cfg *cfg);
	u32 (*preload_init_set)(struct mac_ax_adapter *adapter, enum mac_ax_band band,
				enum mac_ax_qta_mode mode);
	u32 (*dle_func_en)(struct mac_ax_adapter *adapter, u8 en);
	u32 (*dle_clk_en)(struct mac_ax_adapter *adapter, u8 en);
	u32 (*dle_mix_cfg)(struct mac_ax_adapter *adapter, struct dle_mem_t *cfg);
	u32 (*wde_quota_cfg)(struct mac_ax_adapter *adapter,
			     struct wde_quota_t *min_cfg,
			     struct wde_quota_t *max_cfg);
	u32 (*ple_quota_cfg)(struct mac_ax_adapter *adapter,
			     struct ple_quota_t *min_cfg,
			     struct ple_quota_t *max_cfg);
	u32 (*chk_dle_rdy)(struct mac_ax_adapter *adapter, enum DLE_CTRL_TYPE type);
	u8 (*is_dbg_port_not_valid)(struct mac_ax_adapter *adapter, u32 dbg_sel);
	u32 (*dbg_port_sel_rst)(struct mac_ax_adapter *adapter, u32 sel);
	u32 (*dle_dfi_sel)(struct mac_ax_adapter *adapter,
			   struct mac_ax_dle_dfi_info **info,
			   u32 *target, u32 sel);
	u32 (*bacam_init)(struct mac_ax_adapter *adapter);
#if MAC_AX_PCIE_SUPPORT
	struct mac_ax_intf_info *
	(*get_pcie_info_def)(struct mac_ax_adapter *adapter);
	struct txbd_ram *
	(*get_bdram_tbl_pcie)(struct mac_ax_adapter *adapter);
	u32 (*mio_w32_pcie)(struct mac_ax_adapter *adapter, u16 addr, u32 value);
	u32 (*mio_r32_pcie)(struct mac_ax_adapter *adapter, u16 addr, u32 *val);
	u32 (*get_txbd_reg_pcie)(struct mac_ax_adapter *adapter, u8 dma_ch, u32 *reg,
				 enum pcie_bd_ctrl_type type);
	u32 (*set_txbd_reg_pcie)(struct mac_ax_adapter *adapter, u8 dma_ch,
				 enum pcie_bd_ctrl_type type, u32 val0, u32 val1, u32 val2);
	u32 (*get_rxbd_reg_pcie)(struct mac_ax_adapter *adapter, u8 dma_ch, u32 *reg,
				 enum pcie_bd_ctrl_type type);
	u32 (*set_rxbd_reg_pcie)(struct mac_ax_adapter *adapter, u8 dma_ch,
				 enum pcie_bd_ctrl_type type, u32 val0, u32 val1, u32 val2);
	u32 (*ltr_sw_trigger)(struct mac_ax_adapter *adapter,
			      enum mac_ax_pcie_ltr_sw_ctrl ctrl);
	u32 (*pcie_cfgspc_write)(struct mac_ax_adapter *adapter,
				 struct mac_ax_pcie_cfgspc_param *param);
	u32 (*pcie_cfgspc_read)(struct mac_ax_adapter *adapter,
				struct mac_ax_pcie_cfgspc_param *param);
	u32 (*pcie_ltr_write)(struct mac_ax_adapter *adapter,
			      struct mac_ax_pcie_ltr_param *param);
	u32 (*pcie_ltr_read)(struct mac_ax_adapter *adapter,
			     struct mac_ax_pcie_ltr_param *param);
	u32 (*ctrl_hci_dma_en_pcie)(struct mac_ax_adapter *adapter,
				    enum mac_ax_pcie_func_ctrl txen,
				    enum mac_ax_pcie_func_ctrl rxen);
	u32 (*ctrl_trxdma_pcie)(struct mac_ax_adapter *adapter,
				enum mac_ax_pcie_func_ctrl txen,
				enum mac_ax_pcie_func_ctrl rxen,
				enum mac_ax_pcie_func_ctrl ioen);
	u32 (*ctrl_wpdma_pcie)(struct mac_ax_adapter *adapter,
			       enum mac_ax_pcie_func_ctrl wpen);
	u32 (*poll_io_idle_pcie)(struct mac_ax_adapter *adapter);
	u32 (*poll_dma_all_idle_pcie)(struct mac_ax_adapter *adapter);
	u32 (*clr_idx_ch_pcie)(struct mac_ax_adapter *adapter,
			       struct mac_ax_txdma_ch_map *txch_map,
			       struct mac_ax_rxdma_ch_map *rxch_map);
	u32 (*rst_bdram_pcie)(struct mac_ax_adapter *adapter, u8 val);
	u32 (*trx_mit_pcie)(struct mac_ax_adapter *adapter,
			    struct mac_ax_pcie_trx_mitigation *mit_info);
	u32 (*mode_op_pcie)(struct mac_ax_adapter *adapter,
			    struct mac_ax_intf_info *intf_info);
	u32 (*get_err_flag_pcie)(struct mac_ax_adapter *adapter,
				 struct mac_ax_pcie_err_info *out_info);
	u32 (*mac_auto_refclk_cal_pcie)(struct mac_ax_adapter *adapter,
					enum mac_ax_pcie_func_ctrl en);
	u32 (*sync_trx_bd_idx)(struct mac_ax_adapter *adapter);
#ifdef RTW_WKARD_GET_PROCESSOR_ID
	u32 (*chk_proc_long_ldy_pcie)(struct mac_ax_adapter *adapter, u8 *val);
#endif
#endif
#if MAC_AX_SDIO_SUPPORT
	u8 (*r_indir_cmd52_sdio)(struct mac_ax_adapter *adapter, u32 adr);
	u8 (*_r_indir_cmd52_sdio)(struct mac_ax_adapter *adapter, u32 adr);
	u32 (*_r_indir_cmd53_sdio)(struct mac_ax_adapter *adapter, u32 adr);
	u32 (*r16_indir_sdio)(struct mac_ax_adapter *adapter, u32 adr);
	u32 (*r32_indir_sdio)(struct mac_ax_adapter *adapter, u32 adr);
	void (*w_indir_cmd52_sdio)(struct mac_ax_adapter *adapter, u32 adr,
				   u32 val, enum sdio_io_size size);
	void (*w_indir_cmd53_sdio)(struct mac_ax_adapter *adapter, u32 adr, u32 val,
				   enum sdio_io_size size);
	void (*ud_fs)(struct mac_ax_adapter *adapter);
	u32 (*sdio_pre_init)(struct mac_ax_adapter *adapter, void *param);
	u32 (*tx_mode_cfg_sdio)(struct mac_ax_adapter *adapter,
				enum mac_ax_sdio_tx_mode mode);
	u32 (*leave_suspend_sdio)(struct mac_ax_adapter *adapter);
	u32 (*get_int_latency_sdio)(struct mac_ax_adapter *adapter);
	u32 (*get_clk_cnt_sdio)(struct mac_ax_adapter *adapter, u32 *cnt);
	u32 (*set_wt_cfg_sdio)(struct mac_ax_adapter *adapter, u8 en);
	u32 (*set_clk_mon_sdio)(struct mac_ax_adapter *adapter,
				struct mac_ax_sdio_clk_mon_cfg *cfg);
	u32 (*sdio_pwr_switch)(void *vadapter, u8 pre_switch, u8 on);
	void (*rx_agg_cfg_sdio)(struct mac_ax_adapter *adapter,
				struct mac_ax_rx_agg_cfg *cfg);
	void (*aval_page_cfg_sdio)(struct mac_ax_adapter *adapter,
				   struct mac_ax_aval_page_cfg *cfg);
	u32 (*get_sdio_rx_req_len)(struct mac_ax_adapter *adapter, u32 *rx_req_len);
#endif
#if MAC_AX_USB_SUPPORT
	u32 (*usb_ep_cfg)(struct mac_ax_adapter *adapter,
			  struct mac_ax_usb_ep *cfg);
#endif
};

u32 get_mac_ax_priv_size(void);

struct mac_ax_priv_ops *
mac_ax_get_priv_ops(struct mac_ax_adapter *adapter, enum mac_ax_intf intf);

#endif
