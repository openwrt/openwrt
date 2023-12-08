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

#include "hci_fc.h"
#include "mac_priv.h"

static u32 chcfg_size = sizeof(struct mac_ax_hfc_ch_cfg) * MAC_AX_DMA_CH_NUM;

static u32 chinfo_size = sizeof(struct mac_ax_hfc_ch_info) * MAC_AX_DMA_CH_NUM;

#if MAC_AX_PCIE_SUPPORT
#ifdef PHL_FEATURE_AP
static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_8852a[] = {
	{16, 3712, grp_0}, /* ACH 0 */
	{16, 3712, grp_0}, /* ACH 1 */
	{16, 3712, grp_0}, /* ACH 2 */
	{16, 3712, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{16, 3712, grp_0}, /* B0MGQ */
	{16, 3712, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};
#else // for NiC mode use
static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_8852a[] = {
	{16, 3276, grp_0}, /* ACH 0 */
	{16, 3276, grp_0}, /* ACH 1 */
	{16, 3276, grp_0}, /* ACH 2 */
	{16, 3276, grp_0}, /* ACH 3 */
	{8, 3284, grp_0}, /* ACH 4 */
	{8, 3284, grp_0}, /* ACH 5 */
	{8, 3284, grp_0}, /* ACH 6 */
	{8, 3284, grp_0}, /* ACH 7 */
	{16, 3276, grp_0}, /* B0MGQ */
	{16, 3276, grp_0}, /* B0HIQ */
	{8, 3284, grp_0}, /* B1MGQ */
	{8, 3284, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};
#endif

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_8852b[] = {
	{5, 343, grp_0}, /* ACH 0 */
	{5, 343, grp_0}, /* ACH 1 */
	{5, 343, grp_0}, /* ACH 2 */
	{5, 343, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{4, 344, grp_0}, /* B0MGQ */
	{4, 344, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_8852b_turbo[] = {
	{16, 744, grp_0}, /* ACH 0 */
	{16, 744, grp_0}, /* ACH 1 */
	{16, 744, grp_0}, /* ACH 2 */
	{16, 744, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{16, 744, grp_0}, /* B0MGQ */
	{16, 744, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_8851b[] = {
	{5, 343, grp_0}, /* ACH 0 */
	{5, 343, grp_0}, /* ACH 1 */
	{5, 343, grp_0}, /* ACH 2 */
	{5, 343, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{4, 344, grp_0}, /* B0MGQ */
	{4, 344, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_8851b_turbo[] = {
	{16, 744, grp_0}, /* ACH 0 */
	{16, 744, grp_0}, /* ACH 1 */
	{16, 744, grp_0}, /* ACH 2 */
	{16, 744, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{16, 744, grp_0}, /* B0MGQ */
	{16, 744, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

#ifdef PHL_FEATURE_AP
static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8852c[] = {
	{26, 3098, grp_0}, /* ACH 0 */
	{26, 3098, grp_0}, /* ACH 1 */
	{26, 3098, grp_0}, /* ACH 2 */
	{26, 3098, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{26, 3098, grp_0}, /* B0MGQ */
	{26, 3098, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8851e[] = {
	{26, 3098, grp_0}, /* ACH 0 */
	{26, 3098, grp_0}, /* ACH 1 */
	{26, 3098, grp_0}, /* ACH 2 */
	{26, 3098, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{26, 3098, grp_0}, /* B0MGQ */
	{26, 3098, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8852d[] = {
	{26, 3098, grp_0}, /* ACH 0 */
	{26, 3098, grp_0}, /* ACH 1 */
	{26, 3098, grp_0}, /* ACH 2 */
	{26, 3098, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{26, 3098, grp_0}, /* B0MGQ */
	{26, 3098, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};
#else // for NiC mode use
static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8852c[] = {
	{13, 1614, grp_0}, /* ACH 0 */
	{13, 1614, grp_0}, /* ACH 1 */
	{13, 1614, grp_0}, /* ACH 2 */
	{13, 1614, grp_0}, /* ACH 3 */
	{13, 1614, grp_1}, /* ACH 4 */
	{13, 1614, grp_1}, /* ACH 5 */
	{13, 1614, grp_1}, /* ACH 6 */
	{13, 1614, grp_1}, /* ACH 7 */
	{13, 1614, grp_0}, /* B0MGQ */
	{13, 1614, grp_0}, /* B0HIQ */
	{13, 1614, grp_1}, /* B1MGQ */
	{13, 1614, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8851e[] = {
	{13, 1614, grp_0}, /* ACH 0 */
	{13, 1614, grp_0}, /* ACH 1 */
	{13, 1614, grp_0}, /* ACH 2 */
	{13, 1614, grp_0}, /* ACH 3 */
	{13, 1614, grp_1}, /* ACH 4 */
	{13, 1614, grp_1}, /* ACH 5 */
	{13, 1614, grp_1}, /* ACH 6 */
	{13, 1614, grp_1}, /* ACH 7 */
	{13, 1614, grp_0}, /* B0MGQ */
	{13, 1614, grp_0}, /* B0HIQ */
	{13, 1614, grp_1}, /* B1MGQ */
	{13, 1614, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8852d[] = {
	{13, 1614, grp_0}, /* ACH 0 */
	{13, 1614, grp_0}, /* ACH 1 */
	{13, 1614, grp_0}, /* ACH 2 */
	{13, 1614, grp_0}, /* ACH 3 */
	{13, 1614, grp_1}, /* ACH 4 */
	{13, 1614, grp_1}, /* ACH 5 */
	{13, 1614, grp_1}, /* ACH 6 */
	{13, 1614, grp_1}, /* ACH 7 */
	{13, 1614, grp_0}, /* B0MGQ */
	{13, 1614, grp_0}, /* B0HIQ */
	{13, 1614, grp_1}, /* B1MGQ */
	{13, 1614, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};
#endif

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_dbcc_8852c[] = {
	{12, 1609, grp_0}, /* ACH 0 */
	{12, 1609, grp_0}, /* ACH 1 */
	{12, 1609, grp_0}, /* ACH 2 */
	{12, 1609, grp_0}, /* ACH 3 */
	{12, 1609, grp_1}, /* ACH 4 */
	{12, 1609, grp_1}, /* ACH 5 */
	{12, 1609, grp_1}, /* ACH 6 */
	{12, 1609, grp_1}, /* ACH 7 */
	{12, 1609, grp_0}, /* B0MGQ */
	{12, 1609, grp_0}, /* B0HIQ */
	{12, 1609, grp_1}, /* B1MGQ */
	{12, 1609, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_dbcc_8851e[] = {
	{12, 1609, grp_0}, /* ACH 0 */
	{12, 1609, grp_0}, /* ACH 1 */
	{12, 1609, grp_0}, /* ACH 2 */
	{12, 1609, grp_0}, /* ACH 3 */
	{12, 1609, grp_1}, /* ACH 4 */
	{12, 1609, grp_1}, /* ACH 5 */
	{12, 1609, grp_1}, /* ACH 6 */
	{12, 1609, grp_1}, /* ACH 7 */
	{12, 1609, grp_0}, /* B0MGQ */
	{12, 1609, grp_0}, /* B0HIQ */
	{12, 1609, grp_1}, /* B1MGQ */
	{12, 1609, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_dbcc_8852d[] = {
	{12, 1609, grp_0}, /* ACH 0 */
	{12, 1609, grp_0}, /* ACH 1 */
	{12, 1609, grp_0}, /* ACH 2 */
	{12, 1609, grp_0}, /* ACH 3 */
	{12, 1609, grp_1}, /* ACH 4 */
	{12, 1609, grp_1}, /* ACH 5 */
	{12, 1609, grp_1}, /* ACH 6 */
	{12, 1609, grp_1}, /* ACH 7 */
	{12, 1609, grp_0}, /* B0MGQ */
	{12, 1609, grp_0}, /* B0HIQ */
	{12, 1609, grp_1}, /* B1MGQ */
	{12, 1609, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_8192xb[] = {
	{26, 3098, grp_0}, /* ACH 0 */
	{26, 3098, grp_0}, /* ACH 1 */
	{26, 3098, grp_0}, /* ACH 2 */
	{26, 3098, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{26, 3098, grp_0}, /* B0MGQ */
	{26, 3098, grp_0}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_wd128_8852b[] = {
	{256, 1792, grp_0}, /* ACH 0 */
	{256, 1792, grp_0}, /* ACH 1 */
	{256, 1792, grp_0}, /* ACH 2 */
	{256, 1792, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{256, 1792, grp_0}, /* B0MGQ */
	{256, 1792, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_wd128_8851b[] = {
	{256, 1792, grp_0}, /* ACH 0 */
	{256, 1792, grp_0}, /* ACH 1 */
	{256, 1792, grp_0}, /* ACH 2 */
	{256, 1792, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{256, 1792, grp_0}, /* B0MGQ */
	{256, 1792, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_stf_8852a[] = {
	{8, 256, grp_0}, /* ACH 0 */
	{8, 256, grp_0}, /* ACH 1 */
	{8, 256, grp_0}, /* ACH 2 */
	{8, 256, grp_0}, /* ACH 3 */
	{8, 256, grp_1}, /* ACH 4 */
	{8, 256, grp_1}, /* ACH 5 */
	{8, 256, grp_1}, /* ACH 6 */
	{8, 256, grp_1}, /* ACH 7 */
	{8, 256, grp_0}, /* B0MGQ */
	{8, 256, grp_0}, /* B0HIQ */
	{8, 256, grp_1}, /* B1MGQ */
	{8, 256, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_stf_8852b[] = {
	{27, 204, grp_0}, /* ACH 0 */
	{27, 204, grp_0}, /* ACH 1 */
	{27, 204, grp_0}, /* ACH 2 */
	{27, 204, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{11, 204, grp_0}, /* B0MGQ */
	{11, 204, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_stf_8851b[] = {
	{27, 204, grp_0}, /* ACH 0 */
	{27, 204, grp_0}, /* ACH 1 */
	{27, 204, grp_0}, /* ACH 2 */
	{27, 204, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{11, 204, grp_0}, /* B0MGQ */
	{11, 204, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_stf_8852c[] = {
	{2, 108, grp_0}, /* ACH 0 */
	{2, 108, grp_0}, /* ACH 1 */
	{2, 108, grp_0}, /* ACH 2 */
	{2, 108, grp_0}, /* ACH 3 */
	{2, 108, grp_1}, /* ACH 4 */
	{2, 108, grp_1}, /* ACH 5 */
	{2, 108, grp_1}, /* ACH 6 */
	{2, 108, grp_1}, /* ACH 7 */
	{2, 108, grp_0}, /* B0MGQ */
	{2, 108, grp_0}, /* B0HIQ */
	{2, 108, grp_1}, /* B1MGQ */
	{2, 108, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_stf_8851e[] = {
	{2, 108, grp_0}, /* ACH 0 */
	{2, 108, grp_0}, /* ACH 1 */
	{2, 108, grp_0}, /* ACH 2 */
	{2, 108, grp_0}, /* ACH 3 */
	{2, 108, grp_1}, /* ACH 4 */
	{2, 108, grp_1}, /* ACH 5 */
	{2, 108, grp_1}, /* ACH 6 */
	{2, 108, grp_1}, /* ACH 7 */
	{2, 108, grp_0}, /* B0MGQ */
	{2, 108, grp_0}, /* B0HIQ */
	{2, 108, grp_1}, /* B1MGQ */
	{2, 108, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_stf_8852d[] = {
	{2, 108, grp_0}, /* ACH 0 */
	{2, 108, grp_0}, /* ACH 1 */
	{2, 108, grp_0}, /* ACH 2 */
	{2, 108, grp_0}, /* ACH 3 */
	{2, 108, grp_1}, /* ACH 4 */
	{2, 108, grp_1}, /* ACH 5 */
	{2, 108, grp_1}, /* ACH 6 */
	{2, 108, grp_1}, /* ACH 7 */
	{2, 108, grp_0}, /* B0MGQ */
	{2, 108, grp_0}, /* B0HIQ */
	{2, 108, grp_1}, /* B1MGQ */
	{2, 108, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_dbcc_stf_8852c[] = {
	{2, 76, grp_0}, /* ACH 0 */
	{2, 76, grp_0}, /* ACH 1 */
	{2, 76, grp_0}, /* ACH 2 */
	{2, 76, grp_0}, /* ACH 3 */
	{2, 76, grp_1}, /* ACH 4 */
	{2, 76, grp_1}, /* ACH 5 */
	{2, 76, grp_1}, /* ACH 6 */
	{2, 76, grp_1}, /* ACH 7 */
	{2, 76, grp_0}, /* B0MGQ */
	{2, 76, grp_0}, /* B0HIQ */
	{2, 76, grp_1}, /* B1MGQ */
	{2, 76, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_dbcc_stf_8851e[] = {
	{2, 76, grp_0}, /* ACH 0 */
	{2, 76, grp_0}, /* ACH 1 */
	{2, 76, grp_0}, /* ACH 2 */
	{2, 76, grp_0}, /* ACH 3 */
	{2, 76, grp_1}, /* ACH 4 */
	{2, 76, grp_1}, /* ACH 5 */
	{2, 76, grp_1}, /* ACH 6 */
	{2, 76, grp_1}, /* ACH 7 */
	{2, 76, grp_0}, /* B0MGQ */
	{2, 76, grp_0}, /* B0HIQ */
	{2, 76, grp_1}, /* B1MGQ */
	{2, 76, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_dbcc_stf_8852d[] = {
	{2, 76, grp_0}, /* ACH 0 */
	{2, 76, grp_0}, /* ACH 1 */
	{2, 76, grp_0}, /* ACH 2 */
	{2, 76, grp_0}, /* ACH 3 */
	{2, 76, grp_1}, /* ACH 4 */
	{2, 76, grp_1}, /* ACH 5 */
	{2, 76, grp_1}, /* ACH 6 */
	{2, 76, grp_1}, /* ACH 7 */
	{2, 76, grp_0}, /* B0MGQ */
	{2, 76, grp_0}, /* B0HIQ */
	{2, 76, grp_1}, /* B1MGQ */
	{2, 76, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_scc_stf_8192xb[] = {
	{4, 216, grp_0}, /* ACH 0 */
	{4, 216, grp_0}, /* ACH 1 */
	{4, 216, grp_0}, /* ACH 2 */
	{4, 216, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{2, 108, grp_0}, /* B0MGQ */
	{2, 108, grp_0}, /* B0HIQ */
	{4, 216, grp_1}, /* B1MGQ */
	{4, 216, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_sutp_8852a[] = {
	{128, 256, grp_0}, /* ACH 0 */
	{0, 0, grp_1}, /* ACH 1 */
	{0, 0, grp_1}, /* ACH 2 */
	{0, 0, grp_1}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{0, 0, grp_1}, /* B0MGQ */
	{0, 0, grp_1}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_bcn_test_8852a[] = {
	{128, 1833, grp_0}, /* ACH 0 */
	{128, 1833, grp_0}, /* ACH 1 */
	{128, 1833, grp_0}, /* ACH 2 */
	{128, 1833, grp_0}, /* ACH 3 */
	{128, 1833, grp_1}, /* ACH 4 */
	{128, 1833, grp_1}, /* ACH 5 */
	{128, 1833, grp_1}, /* ACH 6 */
	{128, 1833, grp_1}, /* ACH 7 */
	{32, 1833, grp_0}, /* B0MGQ */
	{128, 1833, grp_0}, /* B0HIQ */
	{32, 1833, grp_1}, /* B1MGQ */
	{128, 1833, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8852a[] = {
	{64, 586, grp_0}, /* ACH 0 */
	{64, 586, grp_0}, /* ACH 1 */
	{64, 586, grp_0}, /* ACH 2 */
	{64, 586, grp_0}, /* ACH 3 */
	{64, 586, grp_1}, /* ACH 4 */
	{64, 586, grp_1}, /* ACH 5 */
	{64, 586, grp_1}, /* ACH 6 */
	{64, 586, grp_1}, /* ACH 7 */
	{32, 586, grp_0}, /* B0MGQ */
	{64, 586, grp_0}, /* B0HIQ */
	{32, 586, grp_1}, /* B1MGQ */
	{64, 586, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8852b[] = {
	{10, 200, grp_0}, /* ACH 0 */
	{10, 200, grp_0}, /* ACH 1 */
	{10, 200, grp_0}, /* ACH 2 */
	{10, 200, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{4, 200, grp_0}, /* B0MGQ */
	{4, 200, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8851b[] = {
	{10, 200, grp_0}, /* ACH 0 */
	{10, 200, grp_0}, /* ACH 1 */
	{10, 200, grp_0}, /* ACH 2 */
	{10, 200, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{4, 200, grp_0}, /* B0MGQ */
	{4, 200, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8852c[] = {
	{64, 586, grp_0}, /* ACH 0 */
	{64, 586, grp_0}, /* ACH 1 */
	{64, 586, grp_0}, /* ACH 2 */
	{64, 586, grp_0}, /* ACH 3 */
	{64, 586, grp_1}, /* ACH 4 */
	{64, 586, grp_1}, /* ACH 5 */
	{64, 586, grp_1}, /* ACH 6 */
	{64, 586, grp_1}, /* ACH 7 */
	{32, 586, grp_0}, /* B0MGQ */
	{64, 586, grp_0}, /* B0HIQ */
	{32, 586, grp_1}, /* B1MGQ */
	{64, 586, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8851e[] = {
	{64, 586, grp_0}, /* ACH 0 */
	{64, 586, grp_0}, /* ACH 1 */
	{64, 586, grp_0}, /* ACH 2 */
	{64, 586, grp_0}, /* ACH 3 */
	{64, 586, grp_1}, /* ACH 4 */
	{64, 586, grp_1}, /* ACH 5 */
	{64, 586, grp_1}, /* ACH 6 */
	{64, 586, grp_1}, /* ACH 7 */
	{32, 586, grp_0}, /* B0MGQ */
	{64, 586, grp_0}, /* B0HIQ */
	{32, 586, grp_1}, /* B1MGQ */
	{64, 586, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8852d[] = {
	{64, 586, grp_0}, /* ACH 0 */
	{64, 586, grp_0}, /* ACH 1 */
	{64, 586, grp_0}, /* ACH 2 */
	{64, 586, grp_0}, /* ACH 3 */
	{64, 586, grp_1}, /* ACH 4 */
	{64, 586, grp_1}, /* ACH 5 */
	{64, 586, grp_1}, /* ACH 6 */
	{64, 586, grp_1}, /* ACH 7 */
	{32, 586, grp_0}, /* B0MGQ */
	{64, 586, grp_0}, /* B0HIQ */
	{32, 586, grp_1}, /* B1MGQ */
	{64, 586, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_pcie_la_8192xb[] = {
	{64, 1172, grp_0}, /* ACH 0 */
	{64, 1172, grp_0}, /* ACH 1 */
	{64, 1172, grp_0}, /* ACH 2 */
	{64, 1172, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{32, 1172, grp_0}, /* B0MGQ */
	{64, 1172, grp_0}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* FWCMDQ */
};

#ifdef PHL_FEATURE_AP
static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_8852a = {
	3792, /* Group 0 */
	0, /* Group 1 */
	3792, /* Public Max */
	0 /* WP threshold */
};
#else //for nic mode use
static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_8852a = {
	3792, /* Group 0 */
	0, /* Group 1 */
	3792, /* Public Max */
	0 /* WP threshold */
};
#endif

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_8852b = {
	448, /* Group 0 */
	0, /* Group 1 */
	448, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_8852b_turbo = {
	960, /* Group 0 */
	0, /* Group 1 */
	960, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_8851b = {
	448, /* Group 0 */
	0, /* Group 1 */
	448, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_8851b_turbo = {
	960, /* Group 0 */
	0, /* Group 1 */
	960, /* Public Max */
	0 /* WP threshold */
};

#ifdef PHL_FEATURE_AP
static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8852c = {
	3228, /* Group 0 */
	0, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8851e = {
	3228, /* Group 0 */
	0, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8852d = {
	3228, /* Group 0 */
	0, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};
#else //for nic mode use
static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8852c = {
	1614, /* Group 0 */
	1614, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8851e = {
	1614, /* Group 0 */
	1614, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8852d = {
	1614, /* Group 0 */
	1614, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};
#endif

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_dbcc_8852c = {
	1609, /* Group 0 */
	1609, /* Group 1 */
	3218, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_dbcc_8851e = {
	1609, /* Group 0 */
	1609, /* Group 1 */
	3218, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_dbcc_8852d = {
	1609, /* Group 0 */
	1609, /* Group 1 */
	3218, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_8192xb = {
	3228, /* Group 0 */
	0, /* Group 1 */
	3228, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_wd128_8852b = {
	1792, /* Group 0 */
	0, /* Group 1 */
	1792, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_wd128_8851b = {
	1792, /* Group 0 */
	0, /* Group 1 */
	1792, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_stf_8852a = {
	256, /* Group 0 */
	256, /* Group 1 */
	512, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_stf_8852b = {
	204, /* Group 0 */
	0, /* Group 1 */
	204, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_stf_8851b = {
	204, /* Group 0 */
	0, /* Group 1 */
	204, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_stf_8852c = {
	108, /* Group 0 */
	108, /* Group 1 */
	216, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_dbcc_stf_8852c = {
	76, /* Group 0 */
	76, /* Group 1 */
	152, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_stf_8851e = {
	108, /* Group 0 */
	108, /* Group 1 */
	216, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_dbcc_stf_8851e = {
	76, /* Group 0 */
	76, /* Group 1 */
	152, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_stf_8852d = {
	108, /* Group 0 */
	108, /* Group 1 */
	216, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_dbcc_stf_8852d = {
	76, /* Group 0 */
	76, /* Group 1 */
	152, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_scc_stf_8192xb = {
	216, /* Group 0 */
	0, /* Group 1 */
	216, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_sutp_8852a = {
	256, /* Group 0 */
	0, /* Group 1 */
	256, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_bcn_test_8852a = {
	1833, /* Group 0 */
	1833, /* Group 1 */
	3666, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8852a = {
	586, /* Group 0 */
	586, /* Group 1 */
	1172, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8852b = {
	200, /* Group 0 */
	0, /* Group 1 */
	200, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8851b = {
	200, /* Group 0 */
	0, /* Group 1 */
	200, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8852c = {
	586, /* Group 0 */
	586, /* Group 1 */
	1172, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8851e = {
	586, /* Group 0 */
	586, /* Group 1 */
	1172, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8852d = {
	586, /* Group 0 */
	586, /* Group 1 */
	1172, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_pcie_la_8192xb = {
	1172, /* Group 0 */
	0, /* Group 1 */
	1172, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie = {
	2, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_wd128 = {
	2, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_stf = {
	1, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	64, /* WP CH 0-7 pre-cost */
	64, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_stf_8852c = {
	1, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_dlfw_8852c = {
	0, /*CH 0-11 pre-cost */
	256, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_la_8852c = {
	2, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_stf_8851e = {
	1, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_dlfw_8851e = {
	0, /*CH 0-11 pre-cost */
	256, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_la_8851e = {
	2, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_stf_8852d = {
	1, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_dlfw_8852d = {
	0, /*CH 0-11 pre-cost */
	256, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_la_8852d = {
	2, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_stf_8192xb = {
	1, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_dlfw_8192xb = {
	0, /*CH 0-11 pre-cost */
	256, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_pcie_la_8192xb = {
	2, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};

#endif

#if MAC_AX_USB_SUPPORT
static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_dbcc[] = {
	{22, 212, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{22, 212, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{22, 212, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{22, 212, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{22, 212, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{22, 212, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_dbcc = {
	256, /* Group 0 */
	256, /* Group 1 */
	512, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_dbcc_8852c[] = {
	{24, 164, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{24, 164, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{24, 164, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{24, 164, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{24, 164, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{24, 164, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_dbcc_8852c = {
	164, /* Group 0 */
	164, /* Group 1 */
	328, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_dbcc_8851e[] = {
	{24, 164, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{24, 164, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{24, 164, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{24, 164, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{24, 164, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{24, 164, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_dbcc_8851e = {
	164, /* Group 0 */
	164, /* Group 1 */
	328, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_dbcc_8852d[] = {
	{24, 164, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{24, 164, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{24, 164, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{24, 164, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{24, 164, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{24, 164, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_dbcc_8852d = {
	164, /* Group 0 */
	164, /* Group 1 */
	328, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_8852a[] = {
	{22, 402, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{22, 402, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{22, 402, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{22, 402, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{22, 402, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{22, 402, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_8852b[] = {
	{18, 152, grp_0}, /* ACH 0 */
	{18, 152, grp_0}, /* ACH 1 */
	{18, 152, grp_0}, /* ACH 2 */
	{18, 152, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 152, grp_0}, /* B0MGQ */
	{18, 152, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_turbo_8852b[] = {
	{18, 210, grp_0}, /* ACH 0 */
	{18, 210, grp_0}, /* ACH 1 */
	{18, 210, grp_0}, /* ACH 2 */
	{18, 210, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 210, grp_0}, /* B0MGQ */
	{18, 210, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_8851b[] = {
	{18, 152, grp_0}, /* ACH 0 */
	{18, 152, grp_0}, /* ACH 1 */
	{18, 152, grp_0}, /* ACH 2 */
	{18, 152, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 152, grp_0}, /* B0MGQ */
	{18, 152, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_turbo_8851b[] = {
	{18, 210, grp_0}, /* ACH 0 */
	{18, 210, grp_0}, /* ACH 1 */
	{18, 210, grp_0}, /* ACH 2 */
	{18, 210, grp_0}, /* ACH 3 */
	{0, 0, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{0, 0, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 210, grp_0}, /* B0MGQ */
	{18, 210, grp_0}, /* B0HIQ */
	{0, 0, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_8852c[] = {
	{18, 344, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{18, 344, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{18, 344, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{18, 344, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 344, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{18, 344, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_8851e[] = {
	{18, 344, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{18, 344, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{18, 344, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{18, 344, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 344, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{18, 344, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_scc_8852d[] = {
	{18, 344, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{18, 344, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{18, 344, grp_0}, /* ACH 4 */
	{0, 0, grp_0}, /* ACH 5 */
	{18, 344, grp_0}, /* ACH 6 */
	{0, 0, grp_0}, /* ACH 7 */
	{18, 344, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{18, 344, grp_0}, /* B1MGQ */
	{0, 0, grp_0}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_8852a = {
	512, /* Group 0 */
	0, /* Group 1 */
	512, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_8852b = {
	152, /* Group 0 */
	0, /* Group 1 */
	152, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_turbo_8852b = {
	210, /* Group 0 */
	0, /* Group 1 */
	210, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_8851b = {
	152, /* Group 0 */
	0, /* Group 1 */
	152, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_turbo_8851b = {
	210, /* Group 0 */
	0, /* Group 1 */
	210, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_8852c = {
	344, /* Group 0 */
	0, /* Group 1 */
	344, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_8851e = {
	344, /* Group 0 */
	0, /* Group 1 */
	344, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_scc_8852d = {
	344, /* Group 0 */
	0, /* Group 1 */
	344, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_usb_8852a = {
	11, /*CH 0-11 pre-cost */
	32, /*H2C pre-cost */
	76, /* WP CH 0-7 pre-cost */
	25, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_usb_8852b = {
	9, /*CH 0-11 pre-cost */
	32, /*H2C pre-cost */
	64, /* WP CH 0-7 pre-cost */
	24, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_usb_8851b = {
	9, /*CH 0-11 pre-cost */
	32, /*H2C pre-cost */
	64, /* WP CH 0-7 pre-cost */
	24, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_usb_8852c = {
	9, /*CH 0-11 pre-cost */
	32, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_usb_8851e = {
	9, /*CH 0-11 pre-cost */
	32, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_usb_8852d = {
	9, /*CH 0-11 pre-cost */
	32, /*H2C pre-cost */
	48, /* WP CH 0-7 pre-cost */
	48, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X2, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X2, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X2, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X2 /* WP CH 8-11 full condition */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_la_8852a[] = {
	{22, 84, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{22, 84, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{22, 84, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{22, 84, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{22, 84, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{22, 84, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_la_8852a = {
	128, /* Group 0 */
	128, /* Group 1 */
	256, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_usb_la_8852b[] = {
	{18, 112, grp_0}, /* ACH 0 */
	{0, 0, grp_0}, /* ACH 1 */
	{18, 112, grp_0}, /* ACH 2 */
	{0, 0, grp_0}, /* ACH 3 */
	{18, 112, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{18, 112, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{18, 112, grp_0}, /* B0MGQ */
	{0, 0, grp_0}, /* B0HIQ */
	{18, 112, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{0, 0, 0} /* FWCMDQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_usb_la_8852b = {
	112, /* Group 0 */
	0, /* Group 1 */
	112, /* Public Max */
	0 /* WP threshold */
};
#endif

#if MAC_AX_SDIO_SUPPORT
static struct mac_ax_hfc_ch_cfg hfc_chcfg_sdio_8852a[] = {
	{2, 490, grp_0}, /* ACH 0 */
	{2, 490, grp_0}, /* ACH 1 */
	{2, 490, grp_0}, /* ACH 2 */
	{2, 490, grp_0}, /* ACH 3 */
	{2, 490, grp_0}, /* ACH 4 */
	{2, 490, grp_0}, /* ACH 5 */
	{2, 490, grp_0}, /* ACH 6 */
	{2, 490, grp_0}, /* ACH 7 */
	{2, 490, grp_0}, /* B0MGQ */
	{2, 490, grp_0}, /* B0HIQ */
	{2, 490, grp_0}, /* B1MGQ */
	{2, 490, grp_0}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_sdio_8852b[] = {
	{2, 102, grp_0}, /* ACH 0 */
	{2, 102, grp_0}, /* ACH 1 */
	{2, 102, grp_0}, /* ACH 2 */
	{2, 102, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{2, 102, grp_0}, /* B0MGQ */
	{2, 102, grp_0}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_sdio_8851b[] = {
	{2, 102, grp_0}, /* ACH 0 */
	{2, 102, grp_0}, /* ACH 1 */
	{2, 102, grp_0}, /* ACH 2 */
	{2, 102, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{2, 102, grp_0}, /* B0MGQ */
	{2, 102, grp_0}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_sdio_8852a = {
	512, /* Group 0 */
	0, /* Group 1 */
	512, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_sdio_8852b = {
	112, /* Group 0 */
	0, /* Group 1 */
	112, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_sdio_8851b = {
	112, /* Group 0 */
	0, /* Group 1 */
	112, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_dbcc_sdio_8852a[] = {
	{2, 246, grp_0}, /* ACH 0 */
	{2, 246, grp_0}, /* ACH 1 */
	{2, 246, grp_0}, /* ACH 2 */
	{2, 246, grp_0}, /* ACH 3 */
	{2, 246, grp_1}, /* ACH 4 */
	{2, 246, grp_1}, /* ACH 5 */
	{2, 246, grp_1}, /* ACH 6 */
	{2, 246, grp_1}, /* ACH 7 */
	{2, 246, grp_0}, /* B0MGQ */
	{2, 246, grp_0}, /* B0HIQ */
	{2, 246, grp_1}, /* B1MGQ */
	{2, 246, grp_1}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_dbcc_sdio_8852a = {
	256, /* Group 0 */
	256, /* Group 1 */
	512, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_la_sdio_8852a[] = {
	{2, 54, grp_0}, /* ACH 0 */
	{2, 54, grp_0}, /* ACH 1 */
	{2, 54, grp_0}, /* ACH 2 */
	{2, 54, grp_0}, /* ACH 3 */
	{2, 54, grp_1}, /* ACH 4 */
	{2, 54, grp_1}, /* ACH 5 */
	{2, 54, grp_1}, /* ACH 6 */
	{2, 54, grp_1}, /* ACH 7 */
	{2, 54, grp_0}, /* B0MGQ */
	{2, 54, grp_0}, /* B0HIQ */
	{2, 54, grp_1}, /* B1MGQ */
	{2, 54, grp_1}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_la_sdio_8852a = {
	64, /* Group 0 */
	64, /* Group 1 */
	128, /* Public Max */
	104 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_la_sdio_8852b[] = {
	{2, 102, grp_0}, /* ACH 0 */
	{2, 102, grp_0}, /* ACH 1 */
	{2, 102, grp_0}, /* ACH 2 */
	{2, 102, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{2, 102, grp_0}, /* B0MGQ */
	{2, 102, grp_0}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_la_sdio_8852b = {
	112, /* Group 0 */
	0, /* Group 1 */
	112, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_ch_cfg hfc_chcfg_la_sdio_8851b[] = {
	{2, 102, grp_0}, /* ACH 0 */
	{2, 102, grp_0}, /* ACH 1 */
	{2, 102, grp_0}, /* ACH 2 */
	{2, 102, grp_0}, /* ACH 3 */
	{0, 0, grp_1}, /* ACH 4 */
	{0, 0, grp_1}, /* ACH 5 */
	{0, 0, grp_1}, /* ACH 6 */
	{0, 0, grp_1}, /* ACH 7 */
	{2, 102, grp_0}, /* B0MGQ */
	{2, 102, grp_0}, /* B0HIQ */
	{0, 0, grp_1}, /* B1MGQ */
	{0, 0, grp_1}, /* B1HIQ */
	{40, 0, 0} /* H2CQ */
};

static struct mac_ax_hfc_pub_cfg hfc_pubcfg_la_sdio_8851b = {
	112, /* Group 0 */
	0, /* Group 1 */
	112, /* Public Max */
	0 /* WP threshold */
};

static struct mac_ax_hfc_prec_cfg hfc_preccfg_sdio = {
	1, /*CH 0-11 pre-cost */
	40, /*H2C pre-cost */
	0, /* WP CH 0-7 pre-cost */
	0, /* WP CH 8-11 pre-cost */
	MAC_AX_HFC_FULL_COND_X1, /* CH 0-11 full condition */
	MAC_AX_HFC_FULL_COND_X1, /* H2C full condition */
	MAC_AX_HFC_FULL_COND_X1, /* WP CH 0-7 full condition */
	MAC_AX_HFC_FULL_COND_X1 /* WP CH 8-11 full condition */
};
#endif

u32 hfc_reset_param(struct mac_ax_adapter *adapter)
{
	struct mac_ax_hfc_param *param;
	struct mac_ax_hfc_ch_cfg *ch_cfg, *ch_cfg_ini;
	struct mac_ax_hfc_ch_info *ch_info;
	struct mac_ax_hfc_pub_cfg *pub_cfg, *pub_cfg_ini;
	struct mac_ax_hfc_pub_info *pub_info;
	struct mac_ax_hfc_prec_cfg *prec_cfg, *prec_cfg_ini;
	u8 ch;

	param = adapter->hfc_param;
	ch_cfg = param->ch_cfg;
	ch_info = param->ch_info;
	pub_cfg = param->pub_cfg;
	pub_info = param->pub_info;
	prec_cfg = param->prec_cfg;

	switch (adapter->hw_info->intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		param->en = 0;
		param->mode = MAC_AX_HCIFC_SDIO;
		prec_cfg_ini = &hfc_preccfg_sdio;

		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_sdio_8852a;
				pub_cfg_ini = &hfc_pubcfg_sdio_8852a;
				break;
			case MAC_AX_QTA_DBCC:
				ch_cfg_ini = hfc_chcfg_dbcc_sdio_8852a;
				pub_cfg_ini = &hfc_pubcfg_dbcc_sdio_8852a;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_la_sdio_8852a;
				pub_cfg_ini = &hfc_pubcfg_la_sdio_8852a;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
			case MAC_AX_QTA_SCC_TURBO:
				ch_cfg_ini = hfc_chcfg_sdio_8852b;
				pub_cfg_ini = &hfc_pubcfg_sdio_8852b;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_la_sdio_8852b;
				pub_cfg_ini = &hfc_pubcfg_la_sdio_8852b;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
			case MAC_AX_QTA_SCC_TURBO:
				ch_cfg_ini = hfc_chcfg_sdio_8851b;
				pub_cfg_ini = &hfc_pubcfg_sdio_8851b;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_la_sdio_8851b;
				pub_cfg_ini = &hfc_pubcfg_la_sdio_8851b;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else {
			return MACCHIPID;
		}
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		param->en = 0;
		param->mode = MAC_AX_HCIFC_STF;

		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8852a;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8852a;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			case MAC_AX_QTA_DBCC:
				ch_cfg_ini = hfc_chcfg_usb_dbcc;
				pub_cfg_ini = &hfc_pubcfg_usb_dbcc;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_usb_la_8852a;
				pub_cfg_ini = &hfc_pubcfg_usb_la_8852a;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8852b;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8852b;
				prec_cfg_ini = &hfc_preccfg_usb_8852b;
				break;
			case MAC_AX_QTA_SCC_TURBO:
				ch_cfg_ini = hfc_chcfg_usb_scc_turbo_8852b;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_turbo_8852b;
				prec_cfg_ini = &hfc_preccfg_usb_8852b;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8852b;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_usb_la_8852b;
				pub_cfg_ini = &hfc_pubcfg_usb_la_8852b;
				prec_cfg_ini = &hfc_preccfg_usb_8852b;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8851b;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8851b;
				prec_cfg_ini = &hfc_preccfg_usb_8851b;
				break;
			case MAC_AX_QTA_SCC_TURBO:
				ch_cfg_ini = hfc_chcfg_usb_scc_turbo_8851b;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_turbo_8851b;
				prec_cfg_ini = &hfc_preccfg_usb_8851b;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8851b;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8852c;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8852c;
				prec_cfg_ini = &hfc_preccfg_usb_8852c;
				break;
			case MAC_AX_QTA_DBCC:
				ch_cfg_ini = hfc_chcfg_usb_dbcc_8852c;
				pub_cfg_ini = &hfc_pubcfg_usb_dbcc_8852c;
				prec_cfg_ini = &hfc_preccfg_usb_8852c;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8852c;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_usb_la_8852a;
				pub_cfg_ini = &hfc_pubcfg_usb_la_8852a;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8852c;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8852c;
				prec_cfg_ini = &hfc_preccfg_usb_8852c;
				break;
			case MAC_AX_QTA_DBCC:
				ch_cfg_ini = hfc_chcfg_usb_dbcc;
				pub_cfg_ini = &hfc_pubcfg_usb_dbcc;
				prec_cfg_ini = &hfc_preccfg_usb_8852c;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8852c;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_usb_la_8852a;
				pub_cfg_ini = &hfc_pubcfg_usb_la_8852a;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8851e;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8851e;
				prec_cfg_ini = &hfc_preccfg_usb_8851e;
				break;
			case MAC_AX_QTA_DBCC:
				ch_cfg_ini = hfc_chcfg_usb_dbcc_8851e;
				pub_cfg_ini = &hfc_pubcfg_usb_dbcc_8851e;
				prec_cfg_ini = &hfc_preccfg_usb_8851e;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8851e;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_usb_la_8852a;
				pub_cfg_ini = &hfc_pubcfg_usb_la_8852a;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				ch_cfg_ini = hfc_chcfg_usb_scc_8852d;
				pub_cfg_ini = &hfc_pubcfg_usb_scc_8852d;
				prec_cfg_ini = &hfc_preccfg_usb_8852d;
				break;
			case MAC_AX_QTA_DBCC:
				ch_cfg_ini = hfc_chcfg_usb_dbcc_8852d;
				pub_cfg_ini = &hfc_pubcfg_usb_dbcc_8852d;
				prec_cfg_ini = &hfc_preccfg_usb_8852d;
				break;
			case MAC_AX_QTA_DLFW:
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_usb_8852d;
				break;
			case MAC_AX_QTA_LAMODE:
				ch_cfg_ini = hfc_chcfg_usb_la_8852a;
				pub_cfg_ini = &hfc_pubcfg_usb_la_8852a;
				prec_cfg_ini = &hfc_preccfg_usb_8852a;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else {
			return MACCHIPID;
		}
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		param->en = 0;

		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_DBCC:
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_8852a;
				pub_cfg_ini = &hfc_pubcfg_pcie_8852a;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_DBCC_STF:
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_stf_8852a;
				pub_cfg_ini = &hfc_pubcfg_pcie_stf_8852a;
				prec_cfg_ini = &hfc_preccfg_pcie_stf;
				break;
			case MAC_AX_QTA_SU_TP:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_sutp_8852a;
				pub_cfg_ini = &hfc_pubcfg_pcie_sutp_8852a;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8852a;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8852a;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_8852b;
				pub_cfg_ini = &hfc_pubcfg_pcie_8852b;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_TURBO:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_8852b_turbo;
				pub_cfg_ini = &hfc_pubcfg_pcie_8852b_turbo;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_stf_8852b;
				pub_cfg_ini = &hfc_pubcfg_pcie_stf_8852b;
				prec_cfg_ini = &hfc_preccfg_pcie_stf;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8852b;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8852b;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_8851b;
				pub_cfg_ini = &hfc_pubcfg_pcie_8851b;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_TURBO:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_8851b_turbo;
				pub_cfg_ini = &hfc_pubcfg_pcie_8851b_turbo;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_stf_8851b;
				pub_cfg_ini = &hfc_pubcfg_pcie_stf_8851b;
				prec_cfg_ini = &hfc_preccfg_pcie_stf;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8851b;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8851b;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_DBCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_dbcc_8852c;
				pub_cfg_ini = &hfc_pubcfg_pcie_dbcc_8852c;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_scc_8852c;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_8852c;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_scc_stf_8852c;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_stf_8852c;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8852c;
				break;
			case MAC_AX_QTA_DBCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_dbcc_stf_8852c;
				pub_cfg_ini = &hfc_pubcfg_pcie_dbcc_stf_8852c;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8852c;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie_dlfw_8852c;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8852c;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8852c;
				prec_cfg_ini = &hfc_preccfg_pcie_la_8852c;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_scc_8192xb;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_8192xb;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_scc_stf_8192xb;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_stf_8192xb;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8192xb;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie_dlfw_8192xb;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8192xb;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8192xb;
				prec_cfg_ini = &hfc_preccfg_pcie_la_8192xb;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_DBCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_dbcc_8851e;
				pub_cfg_ini = &hfc_pubcfg_pcie_dbcc_8851e;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_scc_8851e;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_8851e;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_scc_stf_8851e;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_stf_8851e;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8851e;
				break;
			case MAC_AX_QTA_DBCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_dbcc_stf_8851e;
				pub_cfg_ini = &hfc_pubcfg_pcie_dbcc_stf_8851e;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8851e;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie_dlfw_8851e;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8851e;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8851e;
				prec_cfg_ini = &hfc_preccfg_pcie_la_8851e;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			switch (adapter->dle_info.qta_mode) {
			case MAC_AX_QTA_DBCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_dbcc_8852d;
				pub_cfg_ini = &hfc_pubcfg_pcie_dbcc_8852d;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_scc_8852d;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_8852d;
				prec_cfg_ini = &hfc_preccfg_pcie;
				break;
			case MAC_AX_QTA_SCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_scc_stf_8852d;
				pub_cfg_ini = &hfc_pubcfg_pcie_scc_stf_8852d;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8852d;
				break;
			case MAC_AX_QTA_DBCC_STF:
				param->mode = MAC_AX_HCIFC_STF;
				ch_cfg_ini = hfc_chcfg_pcie_dbcc_stf_8852d;
				pub_cfg_ini = &hfc_pubcfg_pcie_dbcc_stf_8852d;
				prec_cfg_ini = &hfc_preccfg_pcie_stf_8852d;
				break;
			case MAC_AX_QTA_DLFW:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = NULL;
				pub_cfg_ini = NULL;
				prec_cfg_ini = &hfc_preccfg_pcie_dlfw_8852d;
				break;
			case MAC_AX_QTA_LAMODE:
				param->mode = MAC_AX_HCIFC_POH;
				ch_cfg_ini = hfc_chcfg_pcie_la_8852d;
				pub_cfg_ini = &hfc_pubcfg_pcie_la_8852d;
				prec_cfg_ini = &hfc_preccfg_pcie_la_8852d;
				break;
			default:
				return MACHFCCH011QTA;
			}
		} else {
			return MACCHIPID;
		}
		break;
#endif
	default:
		return MACINTF;
	}

	if (pub_cfg_ini) {
		pub_cfg->group0 = pub_cfg_ini->group0;
		pub_cfg->group1 = pub_cfg_ini->group1;
		pub_cfg->pub_max = pub_cfg_ini->pub_max;
		pub_cfg->wp_thrd = pub_cfg_ini->wp_thrd;
	}

	pub_info->g0_used = 0;
	pub_info->g1_used = 0;
	pub_info->pub_aval = 0;
	pub_info->wp_aval = 0;

	if (pub_cfg_ini) {
		prec_cfg->ch011_prec = prec_cfg_ini->ch011_prec;
		prec_cfg->h2c_prec = prec_cfg_ini->h2c_prec;
		prec_cfg->wp_ch07_prec = prec_cfg_ini->wp_ch07_prec;
		prec_cfg->wp_ch811_prec = prec_cfg_ini->wp_ch811_prec;
		prec_cfg->ch011_full_cond = prec_cfg_ini->ch011_full_cond;
		prec_cfg->h2c_full_cond = prec_cfg_ini->h2c_full_cond;
		prec_cfg->wp_ch07_full_cond = prec_cfg_ini->wp_ch07_full_cond;
		prec_cfg->wp_ch811_full_cond = prec_cfg_ini->wp_ch811_full_cond;

		adapter->hw_info->sw_amsdu_max_size = prec_cfg->wp_ch07_prec *
						      HFC_PAGE_UNIT;
	}

	if (ch_cfg_ini) {
		for (ch = MAC_AX_DMA_ACH0; ch < MAC_AX_DMA_CH_NUM; ch++) {
			ch_cfg[ch].min = ch_cfg_ini[ch].min;
			ch_cfg[ch].max = ch_cfg_ini[ch].max;
			ch_cfg[ch].grp = ch_cfg_ini[ch].grp;
			ch_info[ch].aval = 0;
			ch_info[ch].used = 0;
		}
	}
	return MACSUCCESS;
}

static inline u32 hfc_ch_cfg_chk(struct mac_ax_adapter *adapter, u8 ch)
{
	struct mac_ax_hfc_ch_cfg *ch_cfg = adapter->hfc_param->ch_cfg;
	struct mac_ax_hfc_pub_cfg *pub_cfg = adapter->hfc_param->pub_cfg;
	struct mac_ax_hfc_prec_cfg *prec_cfg = adapter->hfc_param->prec_cfg;

	if (ch >= MAC_AX_DMA_CH_NUM)
		return MACINTF;

	if ((ch_cfg[ch].min && ch_cfg[ch].min < prec_cfg->ch011_prec) ||
	    ch_cfg[ch].max > pub_cfg->pub_max)
		return MACHFCCH011QTA;
	if (ch_cfg[ch].grp >= grp_num)
		return MACHFCCH011GRP;

	return MACSUCCESS;
}

static inline u32 hfc_pub_info_chk(struct mac_ax_adapter *adapter)
{
	struct mac_ax_hfc_pub_cfg *cfg = adapter->hfc_param->pub_cfg;
	struct mac_ax_hfc_pub_info *info = adapter->hfc_param->pub_info;

	if (info->g0_used + info->g1_used + info->pub_aval != cfg->pub_max) {
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			return MACSUCCESS;
		else
			return MACHFCPUBINFO;
	}

	return MACSUCCESS;
}

static inline u32 hfc_pub_cfg_chk(struct mac_ax_adapter *adapter)
{
	struct mac_ax_hfc_param *param = adapter->hfc_param;
	struct mac_ax_hfc_pub_cfg *pub_cfg = param->pub_cfg;

	if (pub_cfg->group0 + pub_cfg->group1 > pub_cfg->pub_max)
		return MACHFCPUBQTA;

	return MACSUCCESS;
}

u32 hfc_ch_ctrl(struct mac_ax_adapter *adapter, u8 ch)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret)
		return ret;

	ret = hfc_ch_cfg_chk(adapter, ch);
	if (ret)
		return ret;

	ret = p_ops->set_fc_page_ctrl_reg(adapter, ch);

	return ret;
}

u32 hfc_upd_ch_info(struct mac_ax_adapter *adapter, u8 ch)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret)
		return ret;

	ret = p_ops->get_fc_page_info(adapter, ch);

	return ret;
}

u32 hfc_pub_ctrl(struct mac_ax_adapter *adapter)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret)
		return ret;

	ret = hfc_pub_cfg_chk(adapter);
	if (ret)
		return ret;

	ret = p_ops->set_fc_pubpg(adapter);

	return MACSUCCESS;
}

u32 hfc_upd_mix_info(struct mac_ax_adapter *adapter)
{
	struct mac_ax_hfc_param *param = adapter->hfc_param;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret)
		return ret;

	ret = p_ops->get_fc_mix_info(adapter);
	if (ret)
		return ret;

	ret = hfc_pub_info_chk(adapter);
	if (param->en && ret)
		return ret;

	return MACSUCCESS;
}

u32 hfc_init(struct mac_ax_adapter *adapter, u8 rst, u8 en, u8 h2c_en)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u8 ch;
	u32 ret = 0;

	if (rst)
		ret = hfc_reset_param(adapter);
	if (ret)
		return ret;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret)
		return ret;

	ret = p_ops->set_fc_func_en(adapter, 0, 0);
	if (ret)
		return ret;

	if (!en && h2c_en) {
		ret = p_ops->set_fc_h2c(adapter);
		if (ret)
			return ret;
		ret = p_ops->set_fc_func_en(adapter, en, h2c_en);
		return ret;
	}

	for (ch = MAC_AX_DMA_ACH0; ch < MAC_AX_DMA_H2C; ch++) {
		if (ch == MAC_AX_DMA_ACH4 || ch == MAC_AX_DMA_ACH5 ||
		    ch == MAC_AX_DMA_ACH6 || ch == MAC_AX_DMA_ACH7 ||
		    ch == MAC_AX_DMA_B1MG || ch == MAC_AX_DMA_B1HI) {
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852C_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
			if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
				ret = hfc_ch_ctrl(adapter, ch);
				if (ret)
					return ret;
			}
#endif
		} else {
			ret = hfc_ch_ctrl(adapter, ch);
			if (ret)
				return ret;
		}
	}

	ret = hfc_pub_ctrl(adapter);
	if (ret)
		return ret;

	ret = p_ops->set_fc_mix_cfg(adapter);
	if (ret)
		return ret;

	if (en || h2c_en) {
		ret = p_ops->set_fc_func_en(adapter, en, h2c_en);
		if (ret)
			return ret;
		PLTFM_DELAY_US(10);
	}
	for (ch = MAC_AX_DMA_ACH0; ch < MAC_AX_DMA_H2C; ch++) {
		if (ch == MAC_AX_DMA_ACH4 || ch == MAC_AX_DMA_ACH5 ||
		    ch == MAC_AX_DMA_ACH6 || ch == MAC_AX_DMA_ACH7 ||
		    ch == MAC_AX_DMA_B1MG || ch == MAC_AX_DMA_B1HI) {
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852C_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
			if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
				ret = hfc_upd_ch_info(adapter, ch);
				if (ret)
					return ret;
			}
#endif
		} else {
			ret = hfc_upd_ch_info(adapter, ch);
			if (ret)
				return ret;
		}
	}
	ret = hfc_upd_mix_info(adapter);
	return ret;
}

u32 hfc_info_init(struct mac_ax_adapter *adapter)
{
	adapter->hfc_param =
		(struct mac_ax_hfc_param *)
		PLTFM_MALLOC(sizeof(struct mac_ax_hfc_param));

	adapter->hfc_param->ch_cfg =
		(struct mac_ax_hfc_ch_cfg *)PLTFM_MALLOC(chcfg_size);

	adapter->hfc_param->ch_info =
		(struct mac_ax_hfc_ch_info *)PLTFM_MALLOC(chinfo_size);

	adapter->hfc_param->pub_cfg =
		(struct mac_ax_hfc_pub_cfg *)
		PLTFM_MALLOC(sizeof(struct mac_ax_hfc_pub_cfg));

	adapter->hfc_param->pub_info =
		(struct mac_ax_hfc_pub_info *)
		PLTFM_MALLOC(sizeof(struct mac_ax_hfc_pub_info));

	adapter->hfc_param->prec_cfg =
		(struct mac_ax_hfc_prec_cfg *)
		PLTFM_MALLOC(sizeof(struct mac_ax_hfc_prec_cfg));

	adapter->hfc_param->en = 0;
	adapter->hfc_param->h2c_en = 0;
	adapter->hfc_param->mode = 0;

	return MACSUCCESS;
}

u32 hfc_info_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_FREE(adapter->hfc_param->prec_cfg,
		   sizeof(struct mac_ax_hfc_prec_cfg));

	PLTFM_FREE(adapter->hfc_param->pub_info,
		   sizeof(struct mac_ax_hfc_pub_info));

	PLTFM_FREE(adapter->hfc_param->pub_cfg,
		   sizeof(struct mac_ax_hfc_pub_cfg));

	PLTFM_FREE(adapter->hfc_param->ch_info, chinfo_size);
	PLTFM_FREE(adapter->hfc_param->ch_cfg, chcfg_size);
	PLTFM_FREE(adapter->hfc_param, sizeof(struct mac_ax_hfc_param));

	return MACSUCCESS;
}

