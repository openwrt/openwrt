/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_PCIE_W_REG_H__
#define __AL_HAL_PCIE_W_REG_H__

#include <mach/al_hal_common.h>

/*
 * Unit Registers
 */

struct al_pcie_w_global_ctrl {
  /* [0x0]  */
  uint32_t port_init;
  /* [0x4]  */
  uint32_t port_status;
  /* [0x8]  */
  uint32_t pm_control;
  uint32_t rsrvd_0;
  /* [0x10]  */
  uint32_t events_gen;
  uint32_t rsrvd[3];
};
struct al_pcie_w_lcl_log {
  uint32_t rsrvd_0[4];
  /* [0x10]  */
  uint32_t cpl_to_info;
  uint32_t rsrvd_1[3];
  /* [0x20]  */
  uint32_t rcv_msg0_0;
  /* [0x24]  */
  uint32_t rcv_msg0_1;
  /* [0x28]  */
  uint32_t rcv_msg0_2;
  uint32_t rsrvd_2;
  /* [0x30]  */
  uint32_t rcv_msg1_0;
  /* [0x34]  */
  uint32_t rcv_msg1_1;
  /* [0x38]  */
  uint32_t rcv_msg1_2;
  uint32_t rsrvd_3;
  /* [0x40]  */
  uint32_t core_q_status;
  uint32_t rsrvd[7];
};
struct al_pcie_w_debug {
  /* [0x0]  */
  uint32_t info_0;
  /* [0x4]  */
  uint32_t info_1;
  /* [0x8]  */
  uint32_t info_2;
  uint32_t rsrvd;
};
struct al_pcie_w_ob_ven_msg {
  /* [0x0]  */
  uint32_t control;
  /* [0x4]  */
  uint32_t param_1;
  /* [0x8]  */
  uint32_t param_2;
  /* [0xc]  */
  uint32_t data_high;
  uint32_t rsrvd_0;
  /* [0x14]  */
  uint32_t data_low;
  uint32_t rsrvd[2];
};
struct al_pcie_w_soc_int {
  /* [0x0]  */
  uint32_t status_0;
  /* [0x4]  */
  uint32_t status_1;
  /* [0x8]  */
  uint32_t status_2;
  /* [0xc]  */
  uint32_t mask_inta_leg_0;
  /* [0x10]  */
  uint32_t mask_inta_leg_1;
  /* [0x14]  */
  uint32_t mask_inta_leg_2;
  /* [0x18]  */
  uint32_t mask_msi_leg_0;
  /* [0x1c]  */
  uint32_t mask_msi_leg_1;
  /* [0x20]  */
  uint32_t mask_msi_leg_2;
  /* [0x24]  */
  uint32_t msi_leg_cntl;
};
struct al_pcie_w_link_down {
  /* [0x0]  */
  uint32_t reset_delay;
  /* [0x4]  */
  uint32_t reset_extend_rsrvd;
};
struct al_pcie_w_cntl_gen {
  /* [0x0]  */
  uint32_t features;
};
struct al_pcie_w_parity {
  /* [0x0]  */
  uint32_t en_core;
  /* [0x4]  */
  uint32_t status_core;
};
struct al_pcie_w_last_wr {
  /* [0x0]  */
  uint32_t cfg_addr;
};
struct al_pcie_w_atu {
  /* [0x0]  */
  uint32_t in_mask_pair[6];
  /* [0x18]  */
  uint32_t out_mask_pair[6];
};
struct al_pcie_w_cfg_elbi {
  /* [0x0]  */
  uint32_t emulation;
};
struct al_pcie_w_emulatecfg {
  /* [0x0]  */
  uint32_t data;
  /* [0x4]  */
  uint32_t addr;
  /* [0x8]  */
  uint32_t cmpl;
};
struct al_pcie_w_int_grp_a {
  /* [0x0] Interrupt Cause RegisterSet by hardware - If MSI-X  ... */
  uint32_t cause_a;
  uint32_t rsrvd_0;
  /* [0x8] Interrupt Cause Set RegisterWriting 1 to a bit in t ... */
  uint32_t cause_set_a;
  uint32_t rsrvd_1;
  /* [0x10] Interrupt Mask RegisterIf Auto-mask control bit =TR ... */
  uint32_t mask_a;
  uint32_t rsrvd_2;
  /* [0x18] Interrupt Mask Clear RegisterUsed when auto-mask co ... */
  uint32_t mask_clear_a;
  uint32_t rsrvd_3;
  /* [0x20] Interrupt Status RegisterThis register latches the  ... */
  uint32_t status_a;
  uint32_t rsrvd_4;
  /* [0x28] Interrupt Control Register */
  uint32_t control_a;
  uint32_t rsrvd_5;
  /* [0x30] Interrupt Mask RegisterEach bit in this register ma ... */
  uint32_t abort_mask_a;
  uint32_t rsrvd_6;
  /* [0x38] Interrupt Log RegisterEach bit in this register mas ... */
  uint32_t log_mask_a;
  uint32_t rsrvd;
};
struct al_pcie_w_int_grp_b {
  /* [0x0] Interrupt Cause RegisterSet by hardware- If MSI-X i ... */
  uint32_t cause_b;
  uint32_t rsrvd_0;
  /* [0x8] Interrupt Cause Set RegisterWriting 1 to a bit in t ... */
  uint32_t cause_set_b;
  uint32_t rsrvd_1;
  /* [0x10] Interrupt Mask RegisterIf Auto-mask control bit =TR ... */
  uint32_t mask_b;
  uint32_t rsrvd_2;
  /* [0x18] Interrupt Mask Clear RegisterUsed when auto-mask co ... */
  uint32_t mask_clear_b;
  uint32_t rsrvd_3;
  /* [0x20] Interrupt Status RegisterThis register latches the  ... */
  uint32_t status_b;
  uint32_t rsrvd_4;
  /* [0x28] Interrupt Control Register */
  uint32_t control_b;
  uint32_t rsrvd_5;
  /* [0x30] Interrupt Mask RegisterEach bit in this register ma ... */
  uint32_t abort_mask_b;
  uint32_t rsrvd_6;
  /* [0x38] Interrupt Log RegisterEach bit in this register mas ... */
  uint32_t log_mask_b;
  uint32_t rsrvd;
};

struct al_pcie_w_regs {
  struct al_pcie_w_global_ctrl global_ctrl; /* [0x0] */
  struct al_pcie_w_lcl_log lcl_log;         /* [0x20] */
  struct al_pcie_w_debug debug;             /* [0x80] */
  struct al_pcie_w_ob_ven_msg ob_ven_msg;   /* [0x90] */
  uint32_t rsrvd_0[84];
  struct al_pcie_w_soc_int soc_int;       /* [0x200] */
  struct al_pcie_w_link_down link_down;   /* [0x228] */
  struct al_pcie_w_cntl_gen ctrl_gen;     /* [0x230] */
  struct al_pcie_w_parity parity;         /* [0x234] */
  struct al_pcie_w_last_wr last_wr;       /* [0x23c] */
  struct al_pcie_w_atu atu;               /* [0x240] */
  struct al_pcie_w_cfg_elbi cfg_elbi;     /* [0x270] */
  struct al_pcie_w_emulatecfg emulatecfg; /* [0x274] */
  uint32_t rsrvd_1[32];
  struct al_pcie_w_int_grp_a int_grp_a_m0; /* [0x300] */
  struct al_pcie_w_int_grp_b int_grp_b_m0; /* [0x340] */
  uint32_t rsrvd_2[32];
  struct al_pcie_w_int_grp_a int_grp_a; /* [0x400] */
  struct al_pcie_w_int_grp_b int_grp_b; /* [0x440] */
};

/*
 * Registers Fields
 */

/* Current state of the LTSSM */
#define PCIE_W_DEBUG_INFO_0_LTSSM_STATE_MASK 0x000001F8
#define PCIE_W_DEBUG_INFO_0_LTSSM_STATE_SHIFT 3

/**** in/out_mask_x_y register ****/
/* When bit [i] set to 1 it maks the compare in the atu_in/out wind ... */
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_MASK 0x0000FFFF
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_SHIFT 0
/* When bit [i] set to 1 it maks the compare in the atu_in/out wind ... */
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_MASK 0xFFFF0000
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_SHIFT 16

/* emulation register */

/* address register */

#endif /* __AL_HAL_pcie_w_REG_H */

/** @} end of ... group */
