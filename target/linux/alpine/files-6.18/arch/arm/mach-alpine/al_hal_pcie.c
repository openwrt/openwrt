// SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#include "al_hal_pcie_regs.h"
#include <mach/al_hal_pbs_regs.h>
#include <mach/al_hal_pcie.h>
#include <mach/al_hal_unit_adapter_regs.h>

/* --->>> Parameters definitions <<<--- */
#define AL_PCIE_REV_ID_0 0

#define AL_PCIE_AXI_REGS_OFFSET 0x0
#define AL_PCIE_APP_REGS_OFFSET 0x1000
#define AL_PCIE_CORE_CONF_BASE_OFFSET 0x2000

#define AL_PCIE_LTSSM_STATE_L0 0x11
#define AL_PCIE_LTSSM_STATE_L0S 0x12

/* --->>> static functions <<<--- */

int al_pcie_port_snoop_config(struct al_pcie_port *pcie_port,
                              al_bool enable_axi_snoop) {
  struct al_pcie_regs *regs = pcie_port->regs;

  /* Set snoop mode */
  al_info("PCIE_%d: snoop mode %s\n", pcie_port->port_id,
          enable_axi_snoop ? "enable" : "disable");

  if (enable_axi_snoop) {
    al_reg_write32_masked(&regs->axi.ctrl.master_arctl,
                          PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP |
                              PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP,
                          PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP |
                              PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP);

    al_reg_write32_masked(&regs->axi.ctrl.master_awctl,
                          PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP |
                              PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP,
                          PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP |
                              PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP);
  } else {
    al_reg_write32_masked(&regs->axi.ctrl.master_arctl,
                          PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP |
                              PCIE_AXI_CTRL_MASTER_ARCTL_SNOOP,
                          PCIE_AXI_CTRL_MASTER_ARCTL_OVR_SNOOP);

    al_reg_write32_masked(&regs->axi.ctrl.master_awctl,
                          PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP |
                              PCIE_AXI_CTRL_MASTER_AWCTL_SNOOP,
                          PCIE_AXI_CTRL_MASTER_AWCTL_OVR_SNOOP);
  }
  return 0;
}

static enum al_pcie_function_mode
al_pcie_function_mode_get(struct al_pcie_port *pcie_port) {
  struct al_pcie_regs *regs = pcie_port->regs;
  uint32_t reg, device_type;

  al_assert(pcie_port);

  reg = al_reg_read32(&regs->axi.pcie_global.conf);

  device_type =
      AL_REG_FIELD_GET(reg, PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_MASK,
                       PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_SHIFT);

  switch (device_type) {
  case PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_EP:
    return AL_PCIE_FUNCTION_MODE_EP;
  case PCIE_AXI_MISC_PCIE_GLOBAL_CONF_DEV_TYPE_RC:
    return AL_PCIE_FUNCTION_MODE_RC;
  default:
    al_err("PCIe %d: unknown device type (%d) in global conf "
           "register.\n",
           pcie_port->port_id, device_type);
  }
  return AL_PCIE_FUNCTION_MODE_UNKNOWN;
}

/******************** link operations ***************************************/

/** return AL_TRUE if link is up, AL_FALSE otherwise */
static al_bool al_pcie_check_link(struct al_pcie_port *pcie_port,
                                  uint8_t *ltssm_ret) {
  struct al_pcie_regs *regs = (struct al_pcie_regs *)pcie_port->regs;
  uint32_t info_0;
  uint8_t ltssm_state;

  info_0 = al_reg_read32(&regs->app.debug.info_0);

  ltssm_state = AL_REG_FIELD_GET(info_0, PCIE_W_DEBUG_INFO_0_LTSSM_STATE_MASK,
                                 PCIE_W_DEBUG_INFO_0_LTSSM_STATE_SHIFT);

  al_dbg("PCIe %d: Port Debug 0: 0x%08x. LTSSM state :0x%x\n",
         pcie_port->port_id, info_0, ltssm_state);

  if (ltssm_ret)
    *ltssm_ret = ltssm_state;

  if ((ltssm_state == AL_PCIE_LTSSM_STATE_L0) ||
      (ltssm_state == AL_PCIE_LTSSM_STATE_L0S))
    return AL_TRUE;
  return AL_FALSE;
}

/** Initializes a PCIe handle structure. */
int al_pcie_handle_init(struct al_pcie_port *pcie_port,
                        void __iomem *pcie_reg_base, unsigned int port_id) {
  pcie_port->regs = pcie_reg_base;
  pcie_port->port_id = port_id;
  pcie_port->write_to_read_only_enabled = AL_FALSE;
  pcie_port->max_lanes = 0;
  pcie_port->ib_hcrd_config_required = AL_FALSE;

  al_dbg("pcie port handle initialized. port id: %d. regs base %p\n", port_id,
         pcie_reg_base);
  return 0;
}

/*TODO: move those defines */
/** return current function mode (root complex or endpoint) */
enum al_pcie_function_mode
al_pcie_function_type_get(struct al_pcie_port *pcie_port) {
  return al_pcie_function_mode_get(pcie_port);
}

/** get link status */
int al_pcie_link_status(struct al_pcie_port *pcie_port,
                        struct al_pcie_link_status *status) {
  struct al_pcie_regs *regs = pcie_port->regs;
  uint16_t pcie_lnksta;

  al_assert(status);

  status->link_up = al_pcie_check_link(pcie_port, &status->ltssm_state);

  if (!status->link_up) {
    status->speed = AL_PCIE_LINK_SPEED_DEFAULT;
    status->lanes = 0;
    return 0;
  }

  pcie_lnksta =
      al_reg_read16((uint16_t __iomem *)&regs->core_space.pcie_cap_base +
                    (AL_PCI_EXP_LNKSTA >> 1));

  switch (pcie_lnksta & AL_PCI_EXP_LNKSTA_CLS) {
  case AL_PCI_EXP_LNKSTA_CLS_2_5GB:
    status->speed = AL_PCIE_LINK_SPEED_GEN1;
    break;
  case AL_PCI_EXP_LNKSTA_CLS_5_0GB:
    status->speed = AL_PCIE_LINK_SPEED_GEN2;
    break;
  case AL_PCI_EXP_LNKSTA_CLS_8_0GB:
    status->speed = AL_PCIE_LINK_SPEED_GEN3;
    break;
  default:
    status->speed = AL_PCIE_LINK_SPEED_DEFAULT;
    al_err("PCIe %d: unknown link speed indication. PCIE LINK STATUS %x\n",
           pcie_port->port_id, pcie_lnksta);
  }
  status->lanes =
      (pcie_lnksta & AL_PCI_EXP_LNKSTA_NLW) >> AL_PCI_EXP_LNKSTA_NLW_SHIFT;
  al_info("PCIe %d: Link up. speed gen%d negotiated width %d\n",
          pcie_port->port_id, status->speed, status->lanes);

  return 0;
}

/* TODO: check if this function needed */
int al_pcie_link_change_speed(struct al_pcie_port *pcie_port,
                              enum al_pcie_link_speed new_speed
                              __attribute__((__unused__))) {
  al_err("PCIe %d: link change speed not implemented\n", pcie_port->port_id);

  return -ENOSYS;
}

/* TODO: check if this function needed */
int al_pcie_link_change_width(struct al_pcie_port *pcie_port,
                              uint8_t width __attribute__((__unused__))) {
  al_err("PCIe %d: link change width not implemented\n", pcie_port->port_id);

  return -ENOSYS;
}

/** set target_bus and mask_target_bus */
int al_pcie_target_bus_set(struct al_pcie_port *pcie_port, uint8_t target_bus,
                           uint8_t mask_target_bus) {
  struct al_pcie_regs *regs = (struct al_pcie_regs *)pcie_port->regs;
  uint32_t reg;

  reg = al_reg_read32(&regs->axi.ob_ctrl.cfg_target_bus);
  AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_MASK,
                   PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_MASK_SHIFT,
                   mask_target_bus);
  AL_REG_FIELD_SET(reg, PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_MASK,
                   PCIE_AXI_MISC_OB_CTRL_CFG_TARGET_BUS_BUSNUM_SHIFT,
                   target_bus);
  al_reg_write32(&regs->axi.ob_ctrl.cfg_target_bus, reg);
  return 0;
}

/** Set secondary bus number */
int al_pcie_secondary_bus_set(struct al_pcie_port *pcie_port, uint8_t secbus) {
  struct al_pcie_regs *regs = pcie_port->regs;

  uint32_t secbus_val =
      (secbus << PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_SHIFT);

  al_reg_write32_masked(&regs->axi.ob_ctrl.cfg_control,
                        PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SEC_BUS_MASK,
                        secbus_val);
  return 0;
}

/** Set sub-ordinary bus number */
int al_pcie_subordinary_bus_set(struct al_pcie_port *pcie_port,
                                uint8_t subbus) {
  struct al_pcie_regs *regs = pcie_port->regs;

  uint32_t subbus_val =
      (subbus << PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_SHIFT);

  al_reg_write32_masked(&regs->axi.ob_ctrl.cfg_control,
                        PCIE_AXI_MISC_OB_CTRL_CFG_CONTROL_SUBBUS_MASK,
                        subbus_val);
  return 0;
}

/** program internal ATU region entry */
int al_pcie_atu_region_set(struct al_pcie_port *pcie_port,
                           struct al_pcie_atu_region *atu_region) {
  struct al_pcie_regs *regs = pcie_port->regs;
  uint32_t reg = 0;
  uint32_t limit_reg_val;

  /*TODO : add sanity check */
  AL_REG_FIELD_SET(reg, 0xF, 0, atu_region->index);
  AL_REG_BIT_VAL_SET(reg, 31, atu_region->direction);
  al_reg_write32(&regs->core_space.port_regs.iatu.index, reg);

  al_reg_write32(&regs->core_space.port_regs.iatu.lower_base_addr,
                 (uint32_t)(atu_region->base_addr & 0xFFFFFFFF));
  al_reg_write32(&regs->core_space.port_regs.iatu.upper_base_addr,
                 (uint32_t)((atu_region->base_addr >> 32) & 0xFFFFFFFF));
  al_reg_write32(&regs->core_space.port_regs.iatu.lower_target_addr,
                 (uint32_t)(atu_region->target_addr & 0xFFFFFFFF));
  al_reg_write32(&regs->core_space.port_regs.iatu.upper_target_addr,
                 (uint32_t)((atu_region->target_addr >> 32) & 0xFFFFFFFF));

  if (pcie_port->rev_id > AL_PCIE_REV_ID_0) {
    uint32_t *limit_ext_reg =
        (atu_region->direction == al_pcie_atu_dir_outbound)
            ? &regs->app.atu.out_mask_pair[atu_region->index / 2]
            : &regs->app.atu.in_mask_pair[atu_region->index / 2];
    uint32_t limit_ext_reg_mask =
        (atu_region->index % 2)
            ? PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_MASK
            : PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_MASK;
    unsigned int limit_ext_reg_shift =
        (atu_region->index % 2)
            ? PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_ODD_SHIFT
            : PCIE_W_ATU_MASK_EVEN_ODD_ATU_MASK_40_32_EVEN_SHIFT;
    uint64_t limit_sz = atu_region->limit - atu_region->base_addr;
    uint64_t limit_sz_msk = limit_sz - 1;
    uint32_t limit_ext_reg_val =
        (uint32_t)(((limit_sz_msk) >> 32) & 0xFFFFFFFF);

    if (limit_ext_reg_val) {
      limit_reg_val = (uint32_t)((limit_sz_msk) & 0xFFFFFFFF);
      al_assert(limit_reg_val == 0xFFFFFFFF);
    } else {
      limit_reg_val = (uint32_t)(atu_region->limit & 0xFFFFFFFF);
    }

    al_reg_write32_masked(limit_ext_reg, limit_ext_reg_mask,
                          limit_ext_reg_val << limit_ext_reg_shift);
  } else {
    limit_reg_val = (uint32_t)(atu_region->limit & 0xFFFFFFFF);
  }

  al_reg_write32(&regs->core_space.port_regs.iatu.limit_addr, limit_reg_val);

  reg = 0;
  AL_REG_FIELD_SET(reg, 0x1F, 0, atu_region->tlp_type);
  AL_REG_FIELD_SET(reg, 0x3 << 9, 9, atu_region->attr);
  al_reg_write32(&regs->core_space.port_regs.iatu.cr1, reg);

  /* Enable/disable the region. */
  reg = 0;
  AL_REG_FIELD_SET(reg, 0xFF, 0, atu_region->msg_code);
  AL_REG_FIELD_SET(reg, 0x700, 8, atu_region->bar_number);
  AL_REG_BIT_VAL_SET(reg, 16, atu_region->enable_attr_match_mode == AL_TRUE);
  AL_REG_BIT_VAL_SET(reg, 21, atu_region->enable_msg_match_mode == AL_TRUE);
  AL_REG_BIT_VAL_SET(reg, 28, atu_region->cfg_shift_mode == AL_TRUE);
  AL_REG_BIT_VAL_SET(reg, 29, atu_region->invert_matching == AL_TRUE);
  if (atu_region->tlp_type == AL_PCIE_TLP_TYPE_MEM ||
      atu_region->tlp_type == AL_PCIE_TLP_TYPE_IO)
    AL_REG_BIT_VAL_SET(reg, 30, !!atu_region->match_mode);
  AL_REG_BIT_VAL_SET(reg, 31, !!atu_region->enable);

  al_reg_write32(&regs->core_space.port_regs.iatu.cr2, reg);

  return 0;
}

/********************** Loopback mode (RC and Endpoint modes) ************/
