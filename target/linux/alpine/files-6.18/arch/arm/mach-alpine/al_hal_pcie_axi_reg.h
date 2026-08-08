/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
          this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __AL_PCIE_HAL_AXI_REG_H__
#define __AL_PCIE_HAL_AXI_REG_H__

#include <mach/al_hal_common.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Unit Registers
 */

struct al_pcie_axi_ctrl {
  /* [0x0]  */
  uint32_t global;
  uint32_t rsrvd_0;
  /* [0x8]  */
  uint32_t master_bctl;
  /* [0xc]  */
  uint32_t master_rctl;
  /* [0x10]  */
  uint32_t master_ctl;
  /* [0x14]  */
  uint32_t master_arctl;
  /* [0x18]  */
  uint32_t master_awctl;
  /* [0x1c]  */
  uint32_t slave_rctl;
  /* [0x20]  */
  uint32_t slv_wctl;
  /* [0x24]  */
  uint32_t slv_ctl;
  /* [0x28]  */
  uint32_t dbi_ctl;
  /* [0x2c]  */
  uint32_t vmid_mask;
  uint32_t rsrvd[4];
};
struct al_pcie_axi_ob_ctrl {
  /* [0x0]  */
  uint32_t cfg_target_bus;
  /* [0x4]  */
  uint32_t cfg_control;
  /* [0x8]  */
  uint32_t io_start_l;
  /* [0xc]  */
  uint32_t io_start_h;
  /* [0x10]  */
  uint32_t io_limit_l;
  /* [0x14]  */
  uint32_t io_limit_h;
  /* [0x18]  */
  uint32_t msg_start_l;
  /* [0x1c]  */
  uint32_t msg_start_h;
  /* [0x20]  */
  uint32_t msg_limit_l;
  /* [0x24]  */
  uint32_t msg_limit_h;
  uint32_t rsrvd[6];
};
struct al_pcie_axi_msg {
  /* [0x0]  */
  uint32_t addr_high;
  /* [0x4]  */
  uint32_t addr_low;
  /* [0x8]  */
  uint32_t type;
};
struct al_pcie_axi_pcie_status {
  /* [0x0]  */
  uint32_t debug;
};
struct al_pcie_axi_rd_parity {
  /* [0x0]  */
  uint32_t log_high;
  /* [0x4]  */
  uint32_t log_low;
};
struct al_pcie_axi_rd_cmpl {
  /* [0x0]  */
  uint32_t cmpl_log_high;
  /* [0x4]  */
  uint32_t cmpl_log_low;
};
struct al_pcie_axi_rd_to {
  /* [0x0]  */
  uint32_t to_log_high;
  /* [0x4]  */
  uint32_t to_log_low;
};
struct al_pcie_axi_wr_cmpl {
  /* [0x0]  */
  uint32_t wr_cmpl_log_high;
  /* [0x4]  */
  uint32_t wr_cmpl_log_low;
};
struct al_pcie_axi_wr_to {
  /* [0x0]  */
  uint32_t wr_to_log_high;
  /* [0x4]  */
  uint32_t wr_to_log_low;
};
struct al_pcie_axi_pcie_global {
  /* [0x0]  */
  uint32_t conf;
};
struct al_pcie_axi_status {
  /* [0x0]  */
  uint32_t lane0;
  /* [0x4]  */
  uint32_t lane1;
  /* [0x8]  */
  uint32_t lane2;
  /* [0xc]  */
  uint32_t lane3;
};
struct al_pcie_axi_conf {
  /* [0x0]  */
  uint32_t zero_lane0;
  /* [0x4]  */
  uint32_t zero_lane1;
  /* [0x8]  */
  uint32_t zero_lane2;
  /* [0xc]  */
  uint32_t zero_lane3;
  /* [0x10]  */
  uint32_t one_lane0;
  /* [0x14]  */
  uint32_t one_lane1;
  /* [0x18]  */
  uint32_t one_lane2;
  /* [0x1c]  */
  uint32_t one_lane3;
};
struct al_pcie_axi_parity {
  /* [0x0]  */
  uint32_t en_axi;
  /* [0x4]  */
  uint32_t status_axi;
};
struct al_pcie_axi_pos_logged {
  /* [0x0]  */
  uint32_t error_low;
  /* [0x4]  */
  uint32_t error_high;
};
struct al_pcie_axi_ordering {
  /* [0x0]  */
  uint32_t pos_cntl;
};
struct al_pcie_axi_link_down {
  /* [0x0]  */
  uint32_t reset_extend;
};
struct al_pcie_axi_pre_configuration {
  /* [0x0]  */
  uint32_t pcie_core_setup;
};
struct al_pcie_axi_init_fc {
  /* [0x0] The sum of all the fields below must be 97 */
  uint32_t cfg;
};
struct al_pcie_axi_int_grp_a_axi {
  /* [0x0] Interrupt Cause RegisterSet by hardware */
  uint32_t cause;
  uint32_t rsrvd_0;
  /* [0x8] Interrupt Cause Set RegisterWriting 1 to a bit in t ... */
  uint32_t cause_set;
  uint32_t rsrvd_1;
  /* [0x10] Interrupt Mask RegisterIf Auto-mask control bit =TR ... */
  uint32_t mask;
  uint32_t rsrvd_2;
  /* [0x18] Interrupt Mask Clear RegisterUsed when auto-mask co ... */
  uint32_t mask_clear;
  uint32_t rsrvd_3;
  /* [0x20] Interrupt Status RegisterThis register latches the  ... */
  uint32_t status;
  uint32_t rsrvd_4;
  /* [0x28] Interrupt Control Register */
  uint32_t control;
  uint32_t rsrvd_5;
  /* [0x30] Interrupt Mask RegisterEach bit in this register ma ... */
  uint32_t abort_mask;
  uint32_t rsrvd_6;
  /* [0x38] Interrupt Log RegisterEach bit in this register mas ... */
  uint32_t log_mask;
  uint32_t rsrvd;
};

struct al_pcie_axi_regs {
  struct al_pcie_axi_ctrl ctrl;       /* [0x0] */
  struct al_pcie_axi_ob_ctrl ob_ctrl; /* [0x40] */
  uint32_t rsrvd_0[4];
  struct al_pcie_axi_msg msg;                             /* [0x90] */
  struct al_pcie_axi_pcie_status pcie_status;             /* [0x9c] */
  struct al_pcie_axi_rd_parity rd_parity;                 /* [0xa0] */
  struct al_pcie_axi_rd_cmpl rd_cmpl;                     /* [0xa8] */
  struct al_pcie_axi_rd_to rd_to;                         /* [0xb0] */
  struct al_pcie_axi_wr_cmpl wr_cmpl;                     /* [0xb8] */
  struct al_pcie_axi_wr_to wr_to;                         /* [0xc0] */
  struct al_pcie_axi_pcie_global pcie_global;             /* [0xc8] */
  struct al_pcie_axi_status status;                       /* [0xcc] */
  struct al_pcie_axi_conf conf;                           /* [0xdc] */
  struct al_pcie_axi_parity parity;                       /* [0xfc] */
  struct al_pcie_axi_pos_logged pos_logged;               /* [0x104] */
  struct al_pcie_axi_ordering ordering;                   /* [0x10c] */
  struct al_pcie_axi_link_down link_down;                 /* [0x110] */
  struct al_pcie_axi_pre_configuration pre_configuration; /* [0x114] */
  struct al_pcie_axi_init_fc init_fc;                     /* [0x118] */
  uint32_t rsrvd_1[57];
  struct al_pcie_axi_int_grp_a_axi int_grp_a; /* [0x200] */
};

/*
 * Registers Fields
 */

/**** Global register ****/
/* Not in use */
#define PCIE_AXI_CTRL_GLOBAL_CPL_AFTER_P_ORDER_DIS (1 << 0)
/* Not in use */
#define PCIE_AXI_CTRL_GLOBAL_CPU_CPL_ONLY_EN (1 << 1)
/* When linked down, map all transactions to PCIe to DEC ERR. */
#define PCIE_AXI_CTRL_GLOBAL_BLOCK_PCIE_SLAVE_EN (1 << 2)
/* Wait for the NIC to flush before enabling reset to the PCIe c ... */
#define PCIE_AXI_CTRL_GLOBAL_WAIT_SLV_FLUSH_EN (1 << 3)
/* When the BME is cleared and this bit is set, it causes all tr ... */
#define PCIE_AXI_CTRL_GLOBAL_MEM_BAR_MAP_TO_ERR (1 << 4)
/* Wait for the DBI port (the port that enables access to the in ... */
#define PCIE_AXI_CTRL_GLOBAL_WAIT_DBI_FLUSH_EN (1 << 5)
/* When set, adds parity on the write and read address channels, ... */
#define PCIE_AXI_CTRL_GLOBAL_PARITY_CALC_EN_MSTR (1 << 16)
/* When set, enables parity check on the read data. */
#define PCIE_AXI_CTRL_GLOBAL_PARITY_ERR_EN_RD (1 << 17)
/* When set, adds parity on the RD data channel. */
#define PCIE_AXI_CTRL_GLOBAL_PARITY_CALC_EN_SLV (1 << 18)
/* When set, enables parity check on the write data. */
#define PCIE_AXI_CTRL_GLOBAL_PARITY_ERR_EN_WR (1 << 19)
/* When set, error track for timeout and parity is disabled, i */
#define PCIE_AXI_CTRL_GLOBAL_ERROR_TRACK_DIS (1 << 20)

/**** Master_Arctl register ****/
/* override arcache */
#define PCIE_AXI_CTRL_MASTER_ARCTL_OVR_ARCACHE (1 << 0)
/* arache value */
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARACHE_VA_MASK 0x0000001E
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARACHE_VA_SHIFT 1
/* arprot override */
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARPROT_OVR (1 << 5)
/* arprot value */
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARPROT_VALUE_MASK 0x000001C0
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARPROT_VALUE_SHIFT 6
/* vmid val */
#define PCIE_AXI_CTRL_MASTER_ARCTL_VMID_VAL_MASK 0x01FFFE00
#define PCIE_AXI_CTRL_MASTER_ARCTL_VMID_VAL_SHIFT 9
/* IPA value */
#define PCIE_AXI_CTRL_MASTER_ARCTL_IPA_VAL (1 << 25)
/* overide snoop inidcation, if not set take it from mstr_armisc ... */
#define PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP (1 << 26)
/*
snoop indication value when override */
#define PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP (1 << 27)
/*
arqos value */
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARQOS_MASK 0xF0000000
#define PCIE_AXI_CTRL_MASTER_ARCTL_ARQOS_SHIFT 28

/**** Master_Awctl register ****/
/* override arcache */
#define PCIE_AXI_CTRL_MASTER_AWCTL_OVR_ARCACHE (1 << 0)
/* awache value */
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWACHE_VA_MASK 0x0000001E
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWACHE_VA_SHIFT 1
/* awprot override */
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWPROT_OVR (1 << 5)
/* awprot value */
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWPROT_VALUE_MASK 0x000001C0
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWPROT_VALUE_SHIFT 6
/* vmid val */
#define PCIE_AXI_CTRL_MASTER_AWCTL_VMID_VAL_MASK 0x01FFFE00
#define PCIE_AXI_CTRL_MASTER_AWCTL_VMID_VAL_SHIFT 9
/* IPA value */
#define PCIE_AXI_CTRL_MASTER_AWCTL_IPA_VAL (1 << 25)
/* overide snoop inidcation, if not set take it from mstr_armisc ... */
#define PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP (1 << 26)
/*
snoop indication value when override */
#define PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP (1 << 27)
/*
awqos value */
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWQOS_MASK 0xF0000000
#define PCIE_AXI_CTRL_MASTER_AWCTL_AWQOS_SHIFT 28

/**** slv_ctl register ****/
#define PCIE_AXI_CTRL_SLV_CTRL_IO_BAR_EN (1 << 6)

/**** Cfg_Target_Bus register ****/
/* Defines which MSBs to complete the number of the bust that ar ... */
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK 0x000000FF
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_SHIFT 0
/* Target bus number for outbound configuration type0 and type1  ... */
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_MASK 0x0000FF00
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_SHIFT 8

/**** Cfg_Control register ****/
/* Primary bus number */
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_PBUS_MASK 0x000000FF
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_PBUS_SHIFT 0
/*
Subordinate bus number */
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_MASK 0x0000FF00
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_SHIFT 8
/* Secondary bus nnumber */
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_MASK 0x00FF0000
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_SHIFT 16
/* Enable outbound configuration access through iATU.  */
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_IATU_EN (1 << 31)

/**** IO_Start_H register ****/
/*
Outbound ATIU I/O start address high */
#define PCIE_AXI_MISC_OB_CTRL_IO_START_H_ADDR_MASK 0x000003FF
#define PCIE_AXI_MISC_OB_CTRL_IO_START_H_ADDR_SHIFT 0

/**** IO_Limit_H register ****/
/*
Outbound ATIU I/O limit address high */
#define PCIE_AXI_MISC_OB_CTRL_IO_LIMIT_H_ADDR_MASK 0x000003FF
#define PCIE_AXI_MISC_OB_CTRL_IO_LIMIT_H_ADDR_SHIFT 0

/**** Msg_Start_H register ****/
/*
Outbound ATIU msg-no-data start address high */
#define PCIE_AXI_MISC_OB_CTRL_MSG_START_H_ADDR_MASK 0x000003FF
#define PCIE_AXI_MISC_OB_CTRL_MSG_START_H_ADDR_SHIFT 0

/**** Msg_Limit_H register ****/
/*
Outbound ATIU msg-no-data limit address high */
#define PCIE_AXI_MISC_OB_CTRL_MSG_LIMIT_H_ADDR_MASK 0x000003FF
#define PCIE_AXI_MISC_OB_CTRL_MSG_LIMIT_H_ADDR_SHIFT 0

/**** type register ****/
/* Type of message */
#define PCIE_AXI_MISC_MSG_TYPE_TYPE_MASK 0x00FFFFFF
#define PCIE_AXI_MISC_MSG_TYPE_TYPE_SHIFT 0
/* Reserved */
#define PCIE_AXI_MISC_MSG_TYPE_RSRVD_MASK 0xFF000000
#define PCIE_AXI_MISC_MSG_TYPE_RSRVD_SHIFT 24

/**** debug register ****/
/* Causes ACI PCIe reset, including ,master/slave/DBI (registers ... */
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_AXI_BRIDGE_RESET (1 << 0)
/* Causes reset of the entire PCIe core (including the AXI bridg ... */
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_CORE_RESET (1 << 1)
/* Indicates that the SB is empty from the request to the PCIe ( ... */
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_SB_FLUSH_OB_STATUS (1 << 2)
/* MAP and transaction to the PCIe core to ERROR. */
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_SB_MAP_TO_ERR (1 << 3)
/* Indicates that the pcie_core clock is gated off */
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_CORE_CLK_GATE_OFF (1 << 4)
/* Reserved */
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_RSRVD_MASK 0xFFFFFFE0
#define PCIE_AXI_MISC_PCIE_STATUS_DEBUG_RSRVD_SHIFT 5

/**** conf register ****/
/* Device TypeIndicates the specific type of this PCI Express Fu ... */
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_MASK 0x0000000F
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_SHIFT 0
/* [4] – Lane 0 active[5] – Lave 1 active[6] – Lane 2 active[7]  ... */
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_MASK 0x000000F0
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_NOF_ACT_LANES_SHIFT 4
/* [8] SD to the memories */
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_MEM_SHUTDOWN 0x100
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_RESERVED_MASK 0xFFFFFE00
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_RESERVED_SHIFT 9

/**** zero_laneX register ****/
/* phy_mac_local_fs */
#define PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_FS_MASK 0x0000003f
#define PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_FS_SHIFT 0
/* phy_mac_local_lf */
#define PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_LF_MASK 0x00000fc0
#define PCIE_AXI_MISC_ZERO_LANEX_PHY_MAC_LOCAL_LF_SHIFT 6

/**** pos_cntl register ****/
/* Disables POS. */
#define PCIE_AXI_POS_ORDER_AXI_POS_BYPASS (1 << 0)
/* Clear the POS data structure. */
#define PCIE_AXI_POS_ORDER_AXI_POS_CLEAR (1 << 1)
/* Read push all write. */
#define PCIE_AXI_POS_ORDER_AXI_POS_RSO_ENABLE (1 << 2)
/* Causes the PCIe core to wait for all the BRESPs before issuin ... */
#define PCIE_AXI_POS_ORDER_AXI_DW_RD_FLUSH_WR (1 << 3)
/* When set, to 1'b1 supports interleaving data return from the  ... */
#define PCIE_AXI_POS_ORDER_RD_CMPL_AFTER_WR_SUPPORT_RD_INTERLV (1 << 4)
/* When set, to 1'b1 disables read completion after write orderi ... */
#define PCIE_AXI_POS_ORDER_BYPASS_CMPL_AFTER_WR_FIX (1 << 5)
/* When set, disables EP mode read cmpl on the master port push  ... */
#define PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_DIS (1 << 6)
/* When set, disables EP mode read cmpl on the master port push  ... */
#define PCIE_AXI_POS_ORDER_EP_CMPL_AFTER_WR_SUPPORT_INTERLV_DIS (1 << 7)
/* When set disable the ATS CAP.  */
#define PCIE_AXI_CORE_SETUP_ATS_CAP_DIS AL_BIT(13)

/**** pcie_core_setup register ****/
/* This Value delay the rate change to the serdes, until the EIO ... */
#define PCIE_AXI_CORE_SETUP_DELAY_MAC_PHY_RATE_MASK 0x000000FF
#define PCIE_AXI_CORE_SETUP_DELAY_MAC_PHY_RATE_SHIFT 0
/* Limit the number of outstanding AXI reads that the PCIe core  ... */
#define PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_MASK 0x0000FF00
#define PCIE_AXI_CORE_SETUP_NOF_READS_ONSLAVE_INTRF_PCIE_CORE_SHIFT 8
/* Enable the sriov feature */
#define PCIE_AXI_CORE_SETUP_SRIOV_ENABLE AL_BIT(16)

/**** cfg register ****/
/* This value set the possible out standing headers writes (post ... */
#define PCIE_AXI_INIT_FC_CFG_NOF_P_HDR_MASK 0x0000007F
#define PCIE_AXI_INIT_FC_CFG_NOF_P_HDR_SHIFT 0
/* This value set the possible out standing headers reads (non-p ... */
#define PCIE_AXI_INIT_FC_CFG_NOF_NP_HDR_MASK 0x00003F80
#define PCIE_AXI_INIT_FC_CFG_NOF_NP_HDR_SHIFT 7
/* This value set the possible out standing headers CMPLs , the  ... */
#define PCIE_AXI_INIT_FC_CFG_NOF_CPL_HDR_MASK 0x001FC000
#define PCIE_AXI_INIT_FC_CFG_NOF_CPL_HDR_SHIFT 14

#define PCIE_AXI_INIT_FC_CFG_RSRVD_MASK 0xFFE00000
#define PCIE_AXI_INIT_FC_CFG_RSRVD_SHIFT 21

/**** int_cause_grp_A_axi register ****/
/* Master Response Composer Lookup ErrorOverflow that occurred i ... */
#define PCIE_AXI_INT_GRP_A_CAUSE_GM_COMPOSER_LOOKUP_ERR (1 << 0)
/* Indicates a PARITY ERROR on the master data read channel */
#define PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_DATA_PATH_RD (1 << 2)
/* Indicates a PARITY ERROR on the slave addr read channel */
#define PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_ADDR_RD (1 << 3)
/* Indicates a PARITY ERROR on the slave addr write channel */
#define PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_ADDR_WR (1 << 4)
/* Indicates a PARITY ERROR on the slave data write channel */
#define PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERR_OUT_DATA_WR (1 << 5)
/* Reserved */
#define PCIE_AXI_INT_GRP_A_CAUSE_RESERVED_6 (1 << 6)
/* Software error: ECAM write request with invalid bus number */
#define PCIE_AXI_INT_GRP_A_CAUSE_SW_ECAM_ERR_RD (1 << 7)
/* Software error: ECAM read request with invalid bus number */
#define PCIE_AXI_INT_GRP_A_CAUSE_SW_ECAM_ERR_WR (1 << 8)
/* Indicates an ERROR in the PCIe application cause register. */
#define PCIE_AXI_INT_GRP_A_CAUSE_PCIE_CORE_INT (1 << 9)
/* Whenever the Master AXI finishes writing a message, it sets t ... */
#define PCIE_AXI_INT_GRP_A_CAUSE_MSTR_AXI_GETOUT_MSG (1 << 10)
/* Read AXI compilation has ERROR. */
#define PCIE_AXI_INT_GRP_A_CAUSE_RD_CMPL_ERR (1 << 11)
/* Write AXI compilation has ERROR. */
#define PCIE_AXI_INT_GRP_A_CAUSE_WR_CMPL_ERR (1 << 12)
/* Read AXI compilation has timed out. */
#define PCIE_AXI_INT_GRP_A_CAUSE_RD_CMPL_TO (1 << 13)
/* Write AXI compilation has timed out. */
#define PCIE_AXI_INT_GRP_A_CAUSE_WR_CMPL_TO (1 << 14)
/* Parity error AXI domain */
#define PCIE_AXI_INT_GRP_A_CAUSE_PARITY_ERROR_AXI (1 << 15)
/* POS error interrupt */
#define PCIE_AXI_INT_GRP_A_CAUSE_POS_AXI_BRESP (1 << 16)
/* The outstanding write counter become  full should never happe ... */
#define PCIE_AXI_INT_GRP_A_CAUSE_WRITE_CNT_FULL_ERR (1 << 17)
/* BRESP received before the write counter increment.  */
#define PCIE_AXI_INT_GRP_A_CAUSE_BRESP_BEFORE_WR_CNT_INC_ERR (1 << 18)

/**** int_control_grp_A_axi register ****/
/* When Clear_on_Read =1, all bits of the Cause register are cle ... */
#define PCIE_AXI_INT_GRP_A_CTRL_CLEAR_ON_READ (1 << 0)
/* (Must be set only when MSIX is enabled */
#define PCIE_AXI_INT_GRP_A_CTRL_AUTO_MASK (1 << 1)
/* Auto_Clear (RW)When Auto-Clear =1, the bits in the Interrupt  ... */
#define PCIE_AXI_INT_GRP_A_CTRL_AUTO_CLEAR (1 << 2)
/* When set,_on_Posedge =1, the bits in the Interrupt Cause regi ... */
#define PCIE_AXI_INT_GRP_A_CTRL_SET_ON_POS (1 << 3)
/* When Moderation_Reset =1, all Moderation timers associated wi ... */
#define PCIE_AXI_INT_GRP_A_CTRL_MOD_RST (1 << 4)
/* When mask_msi_x =1, no MSI-X from this group is sent */
#define PCIE_AXI_INT_GRP_A_CTRL_MASK_MSI_X (1 << 5)
/* MSI-X AWID value. Same ID for all cause bits. */
#define PCIE_AXI_INT_GRP_A_CTRL_AWID_MASK 0x00000F00
#define PCIE_AXI_INT_GRP_A_CTRL_AWID_SHIFT 8
/* This value determines the interval between interrupts */
#define PCIE_AXI_INT_GRP_A_CTRL_MOD_INTV_MASK 0x00FF0000
#define PCIE_AXI_INT_GRP_A_CTRL_MOD_INTV_SHIFT 16
/* This value determines the Moderation_Timer_Clock speed */
#define PCIE_AXI_INT_GRP_A_CTRL_MOD_RES_MASK 0x0F000000
#define PCIE_AXI_INT_GRP_A_CTRL_MOD_RES_SHIFT 24

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_pcie_axi_REG_H */

/** @} end of ... group */
