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

#define PCIE_PORT_GEN2_CTRL_TX_SWING_LOW_SHIFT 18
#define PCIE_PORT_GEN2_CTRL_TX_COMPLIANCE_RCV_SHIFT 19
#define PCIE_PORT_GEN2_CTRL_DEEMPHASIS_SET_SHIFT 20

#define PCIE_PORT_GEN3_CTRL_EQ_PHASE_2_3_DISABLE_SHIFT 9
#define PCIE_PORT_GEN3_CTRL_EQ_DISABLE_SHIFT 16

#define PCIE_PORT_GEN3_EQ_LF_SHIFT 0
#define PCIE_PORT_GEN3_EQ_LF_MASK 0x3f
#define PCIE_PORT_GEN3_EQ_FS_SHIFT 6
#define PCIE_PORT_GEN3_EQ_FS_MASK (0x3f << PCIE_PORT_GEN3_EQ_FS_SHIFT)

#define PCIE_PORT_LINK_CTRL_LB_EN_SHIFT 2
#define PCIE_PORT_LINK_CTRL_FAST_LINK_EN_SHIFT 7
#define PCIE_PORT_PIPE_LOOPBACK_CTRL_PIPE_LB_EN_SHIFT 31

#define PCIE_PORT_AXI_SLAVE_ERR_RESP_ALL_MAPPING_SHIFT 0

/* filter_mask_reg_1 register */
/*
 * 0: Treat Function MisMatched TLPs as UR
 * 1: Treat Function MisMatched TLPs as Supported
 */
#define CX_FLT_MASK_UR_FUNC_MISMATCH AL_BIT(16)

/*
 * 0: Treat CFG type1 TLPs as UR for EP; Supported for RC
 * 1: Treat CFG type1 TLPs as Supported for EP; UR for RC
 */
#define CX_FLT_MASK_CFG_TYPE1_RE_AS_UR AL_BIT(19)

/*
 * 0: Enforce requester id match for received CPL TLPs.
 *    A violation results in cpl_abort, and possibly AER of unexp_cpl_err,
 *    cpl_rcvd_ur, cpl_rcvd_ca
 * 1: Mask requester id match for received CPL TLPs
 */
#define CX_FLT_MASK_CPL_REQID_MATCH AL_BIT(22)

/*
 * 0: Enforce function match for received CPL TLPs.
 *    A violation results in cpl_abort, and possibly AER of unexp_cpl_err,
 *    cpl_rcvd_ur, cpl_rcvd_ca
 * 1: Mask function match for received CPL TLPs
 */
#define CX_FLT_MASK_CPL_FUNC_MATCH AL_BIT(23)

/* vc0_posted_rcv_q_ctrl register */
#define RADM_PQ_HCRD_VC0_MASK AL_FIELD_MASK(19, 12)
#define RADM_PQ_HCRD_VC0_SHIFT 12

/* vc0_non_posted_rcv_q_ctrl register */
#define RADM_NPQ_HCRD_VC0_MASK AL_FIELD_MASK(19, 12)
#define RADM_NPQ_HCRD_VC0_SHIFT 12

/* vc0_comp_rcv_q_ctrl register */
#define RADM_CPLQ_HCRD_VC0_MASK AL_FIELD_MASK(19, 12)
#define RADM_CPLQ_HCRD_VC0_SHIFT 12

#endif
