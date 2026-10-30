/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_PCIE_HAL_AXI_REG_H__
#define __AL_PCIE_HAL_AXI_REG_H__

#include <mach/al_hal_common.h>

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

/* overide snoop inidcation, if not set take it from mstr_armisc ... */
#define PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP (1 << 26)
/*
snoop indication value when override */
#define PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP (1 << 27)

/* overide snoop inidcation, if not set take it from mstr_armisc ... */
#define PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP (1 << 26)
/*
snoop indication value when override */
#define PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP (1 << 27)

/**** Cfg_Target_Bus register ****/
/* Defines which MSBs to complete the number of the bust that ar ... */
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK 0x000000FF
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_SHIFT 0
/* Target bus number for outbound configuration type0 and type1  ... */
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_MASK 0x0000FF00
#define PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_SHIFT 8

/*
Subordinate bus number */
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_MASK 0x0000FF00
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_SHIFT 8
/* Secondary bus nnumber */
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_MASK 0x00FF0000
#define PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_SHIFT 16

/**** conf register ****/
/* Device TypeIndicates the specific type of this PCI Express Fu ... */
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_MASK 0x0000000F
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_SHIFT 0

#endif /* __AL_HAL_pcie_axi_REG_H */

/** @} end of ... group */
