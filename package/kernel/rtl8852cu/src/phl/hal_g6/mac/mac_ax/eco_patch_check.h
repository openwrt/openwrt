#ifndef __ECO_PATCH_CHECK_H__
#define __ECO_PATCH_CHECK_H__

#include "../mac_def.h"

#define PATCH_DISABLE false
#define PATCH_ENABLE true

bool chk_patch_l2_ldo_power(struct mac_ax_adapter *adapter);
bool chk_patch_aphy_pc(struct mac_ax_adapter *adapter);
bool chk_patch_flr_lps(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_vmain(struct mac_ax_adapter *adapter);
bool chk_patch_otp_power_issue(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_gen2_force_ib(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_power_wake_efuse(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_power_wake(struct mac_ax_adapter *adapter);
bool chk_patch_power_on(struct mac_ax_adapter *adapter);
bool chk_patch_power_off(struct mac_ax_adapter *adapter);
bool chk_patch_dmac_macid_drop_issue(struct mac_ax_adapter *adapter);
bool chk_patch_txamsdu_rls_wd_issue(struct mac_ax_adapter *adapter);
bool chk_patch_cut_amsdu_rls_ple_issue(struct mac_ax_adapter *adapter);
bool chk_patch_is_cfg_avl(struct mac_ax_adapter *adapter);
bool chk_patch_cmac_dma_err_fa(struct mac_ax_adapter *adapter);
bool chk_patch_hi_pri_resp_tx(struct mac_ax_adapter *adapter);
bool chk_patch_port_dis_flow(struct mac_ax_adapter *adapter);
bool chk_patch_dis_resp_chk(struct mac_ax_adapter *adapter);
bool chk_patch_dis_separation(struct mac_ax_adapter *adapter);
bool chk_patch_rsp_ack(struct mac_ax_adapter *adapter);
bool chk_patch_hwamsdu_fa(struct mac_ax_adapter *adapter);
bool chk_patch_vht_ampdu_max_len(struct mac_ax_adapter *adapter);
bool chk_patch_haxidma_ind(struct mac_ax_adapter *adapter);
bool chk_patch_tbtt_shift_setval(struct mac_ax_adapter *adapter);
bool chk_patch_tmac_zld_thold(struct mac_ax_adapter *adapter);
bool chk_patch_snd_ple_modify(struct mac_ax_adapter *adapter);
bool chk_patch_snd_mu_err(struct mac_ax_adapter *adapter);
bool chk_patch_snd_fifofull_err(struct mac_ax_adapter *adapter);
bool chk_patch_snd_ng3_setting(struct mac_ax_adapter *adapter);
bool chk_patch_wmac_timer_src(struct mac_ax_adapter *adapter);
bool chk_patch_v_pulse_control(struct mac_ax_adapter *adapter);
bool chk_patch_reg_sdio(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_sw_ltr(struct mac_ax_adapter *adapter);
bool chk_patch_ltssm_card_loss(struct mac_ax_adapter *adapter);
bool chk_patch_l12_reboot(struct mac_ax_adapter *adapter);
bool chk_patch_rx_prefetch(struct mac_ax_adapter *adapter);
bool chk_patch_sic_clkreq(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_deglitch(struct mac_ax_adapter *adapter);
bool chk_patch_otp_pwr_drop(struct mac_ax_adapter *adapter);
bool chk_patch_usb2_rx_nak(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_err_ind(struct mac_ax_adapter *adapter);
bool chk_patch_fs_enuf(struct mac_ax_adapter *adapter);
bool chk_patch_apb_hang(struct mac_ax_adapter *adapter);
bool chk_patch_fix_emac_delay(struct mac_ax_adapter *adapter);
bool chk_patch_filter_out(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_clkreq_delay(struct mac_ax_adapter *adapter);
bool chk_patch_l11_exit(struct mac_ax_adapter *adapter);
bool chk_patch_ck_buf_level(struct mac_ax_adapter *adapter);
bool chk_patch_pclk_nrdy(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_hci_ldo(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_l2_rxen_lat(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_autok_x(struct mac_ax_adapter *adapter);
bool chk_patch_pcie_hang(struct mac_ax_adapter *adapter);
bool chk_patch_cmac_io_fail(struct mac_ax_adapter *adapter);
bool chk_patch_flash_boot_timing(struct mac_ax_adapter *adapter);
#endif /* __ECO_PATCH_CHECK_H__ */