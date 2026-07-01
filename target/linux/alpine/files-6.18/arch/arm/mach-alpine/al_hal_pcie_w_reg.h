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

#ifndef __AL_HAL_PCIE_W_REG_H__
#define __AL_HAL_PCIE_W_REG_H__

#include <mach/al_hal_common.h>

#ifdef __cplusplus
extern "C" {
#endif
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

/**** Port_Init register ****/
/* Enable port to start LTSSM Link Training */
#define PCIE_W_GLOBAL_CTRL_PORT_INIT_APP_LTSSM_EN (1 << 0)
/* Device TypeIndicates the specific type of this PCIe Function */
#define PCIE_W_GLOBAL_CTRL_PORT_INIT_DEVICE_TYPE_MASK 0x000000F0
#define PCIE_W_GLOBAL_CTRL_PORT_INIT_DEVICE_TYPE_SHIFT 4
/* Performs Manual Lane reversal for transmit Lanes */
#define PCIE_W_GLOBAL_CTRL_PORT_INIT_TX_LANE_FLIP_EN (1 << 8)
/* Performs Manual Lane reversal for receive Lanes */
#define PCIE_W_GLOBAL_CTRL_PORT_INIT_RX_LANE_FLIP_EN (1 << 9)
/* Auxiliary Power DetectedIndicates that auxiliary power (Vaux) ... */
#define PCIE_W_GLOBAL_CTRL_PORT_INIT_SYS_AUX_PWR_DET_NOT_USE (1 << 10)

/**** Port_Status register ****/
/* PHY Link up/down indicator */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PHY_LINK_UP (1 << 0)
/* Data Link Layer up/down indicatorThis status from the Flow Co ... */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_DL_LINK_UP (1 << 1)
/* Reset request due to link down status. */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_LINK_REQ_RST (1 << 2)
/* Power management is in L0s state.. */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PM_LINKST_IN_L0S (1 << 3)
/* Power management is in L1 state. */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PM_LINKST_IN_L1 (1 << 4)
/* Power management is in L2 state. */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PM_LINKST_IN_L2 (1 << 5)
/* Power management is exiting L2 state. */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PM_LINKST_L2_EXIT (1 << 6)
/* Power state of the device. */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PM_DSTATE_MASK 0x00000380
#define PCIE_W_GLOBAL_CTRL_PORT_STS_PM_DSTATE_SHIFT 7
/* Los state */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_XMLH_IN_RL0S (1 << 10)
/* Timeout count before flush */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_LINK_TOUT_FLUSH_NOT (1 << 11)
/* Clock Turnoff RequestAllows clock generation module to turn o ... */
#define PCIE_W_GLOBAL_CTRL_PORT_STS_CORE_CLK_REQ_N (1 << 31)

/**** PM_Control register ****/
/* Wake Up */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_PM_XMT_PME (1 << 0)
/* Request to Enter ASPM L1 */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_REQ_ENTR_L1 (1 << 3)
/* Request to exit ASPM L1.
Only effective if L1 is enabled. */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_REQ_EXIT_L1 (1 << 4)
/* Indication that component is ready to enter the L23 state */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_READY_ENTR_L23 (1 << 5)
/* Request  to generate a PM_Turn_Off Message to communicate tra ... */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_PM_XMT_TURNOFF (1 << 6)
/* Provides a capability to defer incoming Configuration Request ... */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_APP_REQ_RETRY_EN (1 << 7)
/* Core core gate enableIf set, core_clk is gated off whenever a ... */
#define PCIE_W_GLOBAL_CTRL_PM_CONTROL_CORE_CLK_GATE (1 << 31)

/**** Events_Gen register ****/
/* INT_D. Not supported  */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_ASSERT_INTD (1 << 0)
/* INT_C. Not supported  */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_ASSERT_INTC (1 << 1)
/* INT_B. Not supported  */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_ASSERT_INTB (1 << 2)
/* Transmit INT_A Interrupt ControlEvery transition from 0 to 1  ... */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_ASSERT_INTA (1 << 3)
/* A request to generate an outbound MSI interrupt when MSI is e ... */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_MSI_TRNS_REQ (1 << 4)
/* Set the MSI vector before issuing msi_trans_req. */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_MSI_VECTOR_MASK 0x000003E0
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_MSI_VECTOR_SHIFT 5
/* The application requests hot reset to a downstream device */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_APP_RST_INIT (1 << 10)
/* The application request unlock message to be sent */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_UNLOCK_GEN (1 << 30)
/* Indicates that FLR on a Physical Function has been completed */
#define PCIE_W_GLOBAL_CTRL_EVENTS_GEN_FLR_PF_DONE (1 << 31)

/**** Cpl_TO_Info register ****/
/* The Traffic Class of the timed out CPL */
#define PCIE_W_LCL_LOG_CPL_TO_INFO_TC_MASK 0x00000003
#define PCIE_W_LCL_LOG_CPL_TO_INFO_TC_SHIFT 0
/* Indicates which Virtual Function (VF) had a CPL timeout */
#define PCIE_W_LCL_LOG_CPL_TO_INFO_FUN_NUM_MASK 0x000000FC
#define PCIE_W_LCL_LOG_CPL_TO_INFO_FUN_NUM_SHIFT 2
/* The Tag field of the timed out CPL */
#define PCIE_W_LCL_LOG_CPL_TO_INFO_TAG_MASK 0x0000FF00
#define PCIE_W_LCL_LOG_CPL_TO_INFO_TAG_SHIFT 8
/* The Attributes field of the timed out CPL */
#define PCIE_W_LCL_LOG_CPL_TO_INFO_ATTR_MASK 0x00030000
#define PCIE_W_LCL_LOG_CPL_TO_INFO_ATTR_SHIFT 16
/* The Len field of the timed out CPL */
#define PCIE_W_LCL_LOG_CPL_TO_INFO_LEN_MASK 0x3FFC0000
#define PCIE_W_LCL_LOG_CPL_TO_INFO_LEN_SHIFT 18
/* Write 1 to this field to clear the information logged in the  ... */
#define PCIE_W_LCL_LOG_CPL_TO_INFO_VALID (1 << 31)

/**** Rcv_Msg0_0 register ****/
/* The Requester ID of the received message */
#define PCIE_W_LCL_LOG_RCV_MSG0_0_REQ_ID_MASK 0x0000FFFF
#define PCIE_W_LCL_LOG_RCV_MSG0_0_REQ_ID_SHIFT 0
/* Valid logged messageWriting 1 to this bit enables new message ... */
#define PCIE_W_LCL_LOG_RCV_MSG0_0_VALID (1 << 31)

/**** Rcv_Msg1_0 register ****/
/* The Requester ID of the received message */
#define PCIE_W_LCL_LOG_RCV_MSG1_0_REQ_ID_MASK 0x0000FFFF
#define PCIE_W_LCL_LOG_RCV_MSG1_0_REQ_ID_SHIFT 0
/* Valid logged messageWriting 1 to this bit enables new message ... */
#define PCIE_W_LCL_LOG_RCV_MSG1_0_VALID (1 << 31)

/**** Core_Queues_Status register ****/
/* Indicates which entries in the CPL lookup tablehave valid ent ... */
#define PCIE_W_LCL_LOG_CORE_Q_STATUS_CPL_LUT_VALID_MASK 0x0000FFFF
#define PCIE_W_LCL_LOG_CORE_Q_STATUS_CPL_LUT_VALID_SHIFT 0

/**** Debug_Info_0 register ****/
/* Indicates the current power state */
#define PCIE_W_DEBUG_INFO_0_PM_CURRENT_STATE_MASK 0x00000007
#define PCIE_W_DEBUG_INFO_0_PM_CURRENT_STATE_SHIFT 0
/* Current state of the LTSSM */
#define PCIE_W_DEBUG_INFO_0_LTSSM_STATE_MASK 0x000001F8
#define PCIE_W_DEBUG_INFO_0_LTSSM_STATE_SHIFT 3
/* Decode of the Recovery. Equalization LTSSM state */
#define PCIE_W_DEBUG_INFO_0_LTSSM_STATE_RCVRY_EQ (1 << 9)

/**** control register ****/
/* Indication to send vendor message; when clear the message was ... */
#define PCIE_W_OB_VEN_MSG_CONTROL_REQ (1 << 0)

/**** param_1 register ****/
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_FMT_MASK 0x00000003
#define PCIE_W_OB_VEN_MSG_PARAM_1_FMT_SHIFT 0
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_TYPE_MASK 0x0000007C
#define PCIE_W_OB_VEN_MSG_PARAM_1_TYPE_SHIFT 2
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_TC_MASK 0x00000380
#define PCIE_W_OB_VEN_MSG_PARAM_1_TC_SHIFT 7
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_TD (1 << 10)
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_EP (1 << 11)
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_ATTR_MASK 0x00003000
#define PCIE_W_OB_VEN_MSG_PARAM_1_ATTR_SHIFT 12
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_LEN_MASK 0x00FFC000
#define PCIE_W_OB_VEN_MSG_PARAM_1_LEN_SHIFT 14
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_1_TAG_MASK 0xFF000000
#define PCIE_W_OB_VEN_MSG_PARAM_1_TAG_SHIFT 24

/**** param_2 register ****/
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_2_REQ_ID_MASK 0x0000FFFF
#define PCIE_W_OB_VEN_MSG_PARAM_2_REQ_ID_SHIFT 0
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_2_CODE_MASK 0x00FF0000
#define PCIE_W_OB_VEN_MSG_PARAM_2_CODE_SHIFT 16
/* Vendor message parameters */
#define PCIE_W_OB_VEN_MSG_PARAM_2_RSVD_31_24_MASK 0xFF000000
#define PCIE_W_OB_VEN_MSG_PARAM_2_RSVD_31_24_SHIFT 24

/**** features register ****/
/* Enable MSI fix from the SATA to the PCIe EP - Only valid for port zero */
#define PCIE_W_CTRL_GEN_FEATURES_SATA_EP_MSI_FIX AL_BIT(16)

/**** in/out_mask_x_y register ****/
/* When bit [i] set to 1 it maks the compare in the atu_in/out wind ... */
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_MASK 0x0000FFFF
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_SHIFT 0
/* When bit [i] set to 1 it maks the compare in the atu_in/out wind ... */
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_MASK 0xFFFF0000
#define PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_SHIFT 16

/* emulation register */

/*
 * Force all inbound PF0 configuration read to ELBI (emulation interface)
 */
#define PCIE_W_CFG_EMUL_CTRL_FORCE_FUN_0_CFG_ELBI AL_BIT(0)
/*
 * Force all non-PF0 inbound configuration read to ELBI
 */
#define PCIE_W_CFG_EMUL_CTRL_FORCE_FUN_NO0_CFG_ELBI AL_BIT(1)
/*
 * Enable direct connection between DBI and CDM.
 * By default, local CPU can not access the PCIe Core Configuration Space (CDM)
 * through DBI interface if there is pended inbound configuration read or
 * write.
 * In emulation mode, since the inbound configuration is stalled and CPU must
 * access to core configuration space before releasing the inbound
 * configuration transaction, it's required to enable direct path to CPU.
 */
#define PCIE_W_CFG_EMUL_CTRL_EMUL_DBI_FORCE_CDM_EN AL_BIT(2)
/*
 * Disable config direction to trgt1 if above CONFIG_LIMIT.
 * i.e. direct all inbound configuration access to emulation interface
 */
#define PCIE_W_CFG_EMUL_CTRL_EMULCFG_ABOVE_LIMIT_DIS AL_BIT(3)
/*
 * ARI emulation enable, this emulates 8 bits function number, instead of 3.
 * Since our PCIe core does not have the ARI capability and it's single
 * function, requester and completer ID are in the form of {bus, dev, fun},
 * when bus and dev numbers are latched from the received configuration write.
 * When this bit is set, dev number is overridden by the function number when
 * function >= 8.
 */
#define PCIE_W_CFG_EMUL_CTRL_AP_ARI_EMUL_EN AL_BIT(8)
/*
 * Disbale all FLR functionality within the core for both PF and VF. By
 * default the core resets internal data structures and terminate pended
 * requests. Since now all the resources are being used for all functions, it's
 * not correct to apply FLR on the core. When setting this bit, FLR is
 * propaged as configuration write to emulation and emulation driver should
 * handle it by SW.
 */
#define PCIE_W_CFG_EMUL_CTRL_EMULCFG_PFVF_FLR_DIS AL_BIT(9)
/*
 * Disable FLR for func !=0 functionality within the core
 */
#define PCIE_W_CFG_EMUL_CTRL_EMULCFG_VF_FLR_DIS AL_BIT(10)
/*
 * Enable multi-function (VMID) propagation for outbound requests.
 */
#define PCIE_W_CFG_EMUL_CTRL_SRVIO_VFUNC_EN AL_BIT(16)
/*
 * Fix client1 FMT bits after cutting address 63:56, fix address format to
 * 32-bits if original request is 32-bit address.
 */
#define PCIE_W_CFG_EMUL_CTRL_FIX_CLIENT1_FMT_EN AL_BIT(17)

/* address register */

/* Valid address - Cleared on read */
#define PCIE_W_CFG_EMUL_ADDR_VALID AL_BIT(0)

/* Received Configuration Type: CfgType0 (=0) or CfgType1 (=1) */
#define PCIE_W_CFG_EMUL_ADDR_CFG_TYPE AL_BIT(1)
#define PCIE_W_CFG_EMUL_ADDR_CFG_TYPE_0 0
#define PCIE_W_CFG_EMUL_ADDR_CFG_TYPE_1 AL_BIT(1)

/* Target register offset (including extended register) */
#define PCIE_W_CFG_EMUL_ADDR_REG_OFFSET_MASK AL_FIELD_MASK(11, 2)
#define PCIE_W_CFG_EMUL_ADDR_REG_OFFSET_SHIFT 2

/*
 * Received Byte Enable.
 * If 4'b0000, the received packet is Configuration Read transaction, otherwise
 * Configuration Write with corresponding 4-bits Byte Enable.
 */
#define PCIE_W_CFG_EMUL_ADDR_BYTE_ENABLE_MASK AL_FIELD_MASK(15, 12)
#define PCIE_W_CFG_EMUL_ADDR_BYTE_ENABLE_SHIFT 12

/*
 * Dev_Fun
 * - Non-ARI: [19:16] target function num,  [23:19] target device number
 * - ARI: [23:16] target function number
 */
#define PCIE_W_CFG_EMUL_ADDR_DEV_FUN_MASK AL_FIELD_MASK(23, 16)
#define PCIE_W_CFG_EMUL_ADDR_DEV_FUN_SHIFT 16

/* Target Bus Number */
#define PCIE_W_CFG_EMUL_ADDR_BUS_NUM_MASK AL_FIELD_MASK(31, 24)
#define PCIE_W_CFG_EMUL_ADDR_BUS_NUM_SHIFT 24

/**** cause_A register ****/
/* Deassert_INTD received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_DEASSERT_INTD (1 << 0)
/* Deassert_INTC received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_DEASSERT_INTC (1 << 1)
/* Deassert_INTB received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_DEASSERT_INTB (1 << 2)
/* Deassert_INTA received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_DEASSERT_INTA (1 << 3)
/* Assert_INTD received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_ASSERT_INTD (1 << 4)
/* Assert_INTC received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_ASSERT_INTC (1 << 5)
/* Assert_INTC received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_ASSERT_INTB (1 << 6)
/* Assert_INTA received. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_ASSERT_INTA (1 << 7)
/* MSI Controller InterruptMSI interrupt is being received */
#define PCIE_W_INT_GRP_A_CAUSE_A_MSI_CNTR_RCV_INT (1 << 8)
/* MSI sent grant. Write zero to clear this bit. */
#define PCIE_W_INT_GRP_A_CAUSE_A_MSI_TRNS_GNT (1 << 9)
/* System error detected Indicates if any device in the hierarch ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_SYS_ERR_RC (1 << 10)
/* Set when software initiates FLR on a Physical Function by wri ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_FLR_PF_ACTIVE (1 << 11)
/* Reported error condition causes a bit to be set in the Root E ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_AER_RC_ERR (1 << 12)
/* The core asserts aer_rc_err_msi when all of the following con ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_AER_RC_ERR_MSI (1 << 13)
/* Wake Up */
#define PCIE_W_INT_GRP_A_CAUSE_A_WAKE (1 << 14)
/* The core asserts cfg_pme_int when all of the following condit ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_PME_INT (1 << 15)
/* The core asserts cfg_pme_msi when all of the following condit ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_PME_MSI (1 << 16)
/* The core asserts hp_pme when all of the following conditions  ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_HP_PME (1 << 17)
/* The core asserts hp_int when all of the following conditions  ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_HP_INT (1 << 18)
/* The core asserts hp_msi  when the logical AND of the followin ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_HP_MSI (1 << 19)
/* Read VPD registers notification */
#define PCIE_W_INT_GRP_A_CAUSE_A_VPD_INT (1 << 20)
/* The core assert link down event, whenever the link is going d ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_LINK_DOWN_EVENT (1 << 21)
/* When the EP gets a command to shut down, signal the software  ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_PM_XTLH_BLOCK_TLP (1 << 22)
/* PHY/MAC link up */
#define PCIE_W_INT_GRP_A_CAUSE_A_XMLH_LINK_UP (1 << 23)
/* Data link up */
#define PCIE_W_INT_GRP_A_CAUSE_A_RDLH_LINK_UP (1 << 24)
/* The ltssm is in RCVRY_LOCK state. */
#define PCIE_W_INT_GRP_A_CAUSE_A_LTSSM_RCVRY_STATE (1 << 25)
/* Config write transaction to the config space by the RC peer,  ... */
#define PCIE_W_INT_GRP_A_CAUSE_A_CFG_WR_EVENT (1 << 26)
/* When emulation mode is active, every cfg access in EP mode will cause INT. */
#define PCIE_W_INT_GRP_A_CAUSE_A_CFG_EMUL_EVENT (1 << 31)

/**** control_A register ****/
/* When Clear_on_Read =1, all bits of  Cause register are cleare ... */
#define PCIE_W_INT_GRP_A_CONTROL_A_CLEAR_ON_READ (1 << 0)
/* (Must be set only when MSIX is enabled */
#define PCIE_W_INT_GRP_A_CONTROL_A_AUTO_MASK (1 << 1)
/* Auto_Clear (RW)When Auto-Clear =1, the bits in the Interrupt  ... */
#define PCIE_W_INT_GRP_A_CONTROL_A_AUTO_CLEAR (1 << 2)
/* When Set_on_Posedge =1, the bits in the Interrupt Cause regis ... */
#define PCIE_W_INT_GRP_A_CONTROL_A_SET_ON_POSEDGE (1 << 3)
/* When Moderation_Reset =1, all Moderation timers associated wi ... */
#define PCIE_W_INT_GRP_A_CONTROL_A_MOD_RST (1 << 4)
/* When mask_msi_x =1, no MSI-X from this group is sent */
#define PCIE_W_INT_GRP_A_CONTROL_A_MASK_MSI_X (1 << 5)
/* MSI-X AWID value. Same ID for all cause bits. */
#define PCIE_W_INT_GRP_A_CONTROL_A_AWID_MASK 0x00000F00
#define PCIE_W_INT_GRP_A_CONTROL_A_AWID_SHIFT 8
/* This value determines the interval between interrupts; writin ... */
#define PCIE_W_INT_GRP_A_CONTROL_A_MOD_INTV_MASK 0x00FF0000
#define PCIE_W_INT_GRP_A_CONTROL_A_MOD_INTV_SHIFT 16
/* This value determines the Moderation_Timer_Clock speed */
#define PCIE_W_INT_GRP_A_CONTROL_A_MOD_RES_MASK 0x0F000000
#define PCIE_W_INT_GRP_A_CONTROL_A_MOD_RES_SHIFT 24

/**** cause_B register ****/
/* Indicates that the core received a PM_PME Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_PM_PME (1 << 0)
/* Indicates that the core received a PME_TO_Ack Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_PM_TO_ACK (1 << 1)
/* Indicates that the core received an PME_Turn_Off Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_PM_TURNOFF (1 << 2)
/* Indicates that the core received an ERR_CORR Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_CORRECTABLE_ERR (1 << 3)
/* Indicates that the core received an ERR_NONFATAL Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_NONFATAL_ERR (1 << 4)
/* Indicates that the core received an ERR_FATAL Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_FATAL_ERR (1 << 5)
/* Indicates that the core received a Vendor Defined Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_VENDOR_0 (1 << 6)
/* Indicates that the core received a Vendor Defined Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_VENDOR_1 (1 << 7)
/* Indicates that the core received an Unlock Message */
#define PCIE_W_INT_GRP_B_CAUSE_B_MSG_UNLOCK (1 << 8)
/* Notification when the Link Autonomous Bandwidth Status regist ... */
#define PCIE_W_INT_GRP_B_CAUSE_B_LINK_AUTO_BW_INT (1 << 12)
/* Notification that the Link Equalization Request bit in the Li ... */
#define PCIE_W_INT_GRP_B_CAUSE_B_LINK_EQ_REQ_INT (1 << 13)
/* OB Vendor message request is granted by the PCIe core  Write  ... */
#define PCIE_W_INT_GRP_B_CAUSE_B_VENDOR_MSG_GRANT (1 << 14)
/* CPL timeout from the PCIe core inidication */
#define PCIE_W_INT_GRP_B_CAUSE_B_CMP_TIME_OUT (1 << 15)
/* Slave Response Composer Lookup ErrorIndicates that an overflo ... */
#define PCIE_W_INT_GRP_B_CAUSE_B_RADMX_CMPOSER_LOOKUP_ERR (1 << 16)
/* Parity Error */
#define PCIE_W_INT_GRP_B_CAUSE_B_PARITY_ERROR_CORE (1 << 17)

/**** control_B register ****/
/* When Clear_on_Read =1, all bits of the Cause register are cle ... */
#define PCIE_W_INT_GRP_B_CONTROL_B_CLEAR_ON_READ (1 << 0)
/* (Must be set only when MSIX is enabled */
#define PCIE_W_INT_GRP_B_CONTROL_B_AUTO_MASK (1 << 1)
/* Auto_Clear (RW)When Auto-Clear =1, the bits in the Interrupt  ... */
#define PCIE_W_INT_GRP_B_CONTROL_B_AUTO_CLEAR (1 << 2)
/* When Set_on_Posedge =1, the bits in the interrupt Cause regis ... */
#define PCIE_W_INT_GRP_B_CONTROL_B_SET_ON_POSEDGE (1 << 3)
/* When Moderation_Reset =1, all Moderation timers associated wi ... */
#define PCIE_W_INT_GRP_B_CONTROL_B_MOD_RST (1 << 4)
/* When mask_msi_x =1, no MSI-X from this group is sent */
#define PCIE_W_INT_GRP_B_CONTROL_B_MASK_MSI_X (1 << 5)
/* MSI-X AWID value. Same ID for all cause bits. */
#define PCIE_W_INT_GRP_B_CONTROL_B_AWID_MASK 0x00000F00
#define PCIE_W_INT_GRP_B_CONTROL_B_AWID_SHIFT 8
/* This value determines the interval between interrupts */
#define PCIE_W_INT_GRP_B_CONTROL_B_MOD_INTV_MASK 0x00FF0000
#define PCIE_W_INT_GRP_B_CONTROL_B_MOD_INTV_SHIFT 16
/* This value determines the Moderation_Timer_Clock speed */
#define PCIE_W_INT_GRP_B_CONTROL_B_MOD_RES_MASK 0x0F000000
#define PCIE_W_INT_GRP_B_CONTROL_B_MOD_RES_SHIFT 24

#ifdef __cplusplus
}
#endif

#endif /* __AL_HAL_pcie_w_REG_H */

/** @} end of ... group */
