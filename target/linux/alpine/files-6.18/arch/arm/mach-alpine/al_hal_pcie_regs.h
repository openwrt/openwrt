/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_PCIE_REGS_H__
#define __AL_HAL_PCIE_REGS_H__

#include <mach/al_hal_common.h>

#include "al_hal_pcie_axi_reg.h"
#include "al_hal_pcie_w_reg.h"

#define AL_PCIE_AXI_REGS_OFFSET 0x0
#define AL_PCIE_APP_REGS_OFFSET 0x1000
#define AL_PCIE_CORE_CONF_BASE_OFFSET 0x2000

struct al_pcie_core_iatu_regs {
  uint32_t index;
  uint32_t cr1;
  uint32_t cr2;
  uint32_t lower_base_addr;
  uint32_t upper_base_addr;
  uint32_t limit_addr;
  uint32_t lower_target_addr;
  uint32_t upper_target_addr;
  uint32_t cr3;
  uint32_t rsrvd[(0x270 - 0x224) >> 2];
};

struct al_pcie_core_port_regs {
  uint32_t ack_lat_rply_timer;
  uint32_t reserved1[(0x10 - 0x4) >> 2];
  uint32_t port_link_ctrl;
  uint32_t reserved2[(0x1c - 0x14) >> 2];
  uint32_t filter_mask_reg_1;
  uint32_t reserved3[(0x48 - 0x20) >> 2];
  uint32_t vc0_posted_rcv_q_ctrl;
  uint32_t vc0_non_posted_rcv_q_ctrl;
  uint32_t vc0_comp_rcv_q_ctrl;
  uint32_t reserved4[(0x10C - 0x54) >> 2];
  uint32_t gen2_ctrl;
  uint32_t reserved5[(0x190 - 0x110) >> 2];
  uint32_t gen3_ctrl;
  uint32_t gen3_eq_fs_lf;
  uint32_t gen3_eq_preset_to_coef_map;
  uint32_t gen3_eq_preset_idx;
  uint32_t reserved6;
  uint32_t gen3_eq_status;
  uint32_t gen3_eq_ctrl;
  uint32_t reserved7[(0x1B8 - 0x1AC) >> 2];
  uint32_t pipe_loopback_ctrl;
  uint32_t rd_only_wr_en;
  uint32_t reserved8[(0x1D0 - 0x1C0) >> 2];
  uint32_t axi_slave_err_resp;
  uint32_t reserved9[(0x200 - 0x1D4) >> 2];
  struct al_pcie_core_iatu_regs iatu;
  uint32_t reserved10[(0x448 - 0x270) >> 2];
};

struct al_pcie_core_reg_space {
  uint32_t config_header[0x40 >> 2];
  uint32_t pcie_pm_cap_base;
  uint32_t reserved1[(0x70 - 0x44) >> 2];
  uint32_t pcie_cap_base;
  uint32_t pcie_dev_cap_base;
  uint32_t reserved2;
  uint32_t pcie_link_cap_base;
  uint32_t reserved3[(0xB0 - 0x80) >> 2];
  uint32_t msix_cap_base;
  uint32_t reserved4[(0x100 - 0xB4) >> 2];
  uint32_t pcie_aer_cap_base;
  uint32_t reserved5[(0x150 - 0x104) >> 2];
  uint32_t pcie_sec_ext_cap_base;
  uint32_t reserved6[(0x700 - 0x154) >> 2];
  struct al_pcie_core_port_regs port_regs;
};

struct al_pcie_regs {
  struct al_pcie_axi_regs __iomem axi;
  uint32_t
      reserved1[(AL_PCIE_APP_REGS_OFFSET -
                 (AL_PCIE_AXI_REGS_OFFSET + sizeof(struct al_pcie_axi_regs))) >>
                2];
  struct al_pcie_w_regs __iomem app;
  uint32_t
      reserved2[(AL_PCIE_CORE_CONF_BASE_OFFSET -
                 (AL_PCIE_APP_REGS_OFFSET + sizeof(struct al_pcie_w_regs))) >>
                2];
  struct al_pcie_core_reg_space core_space;
};

#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_EP 0
#define PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_RC 4

#endif
