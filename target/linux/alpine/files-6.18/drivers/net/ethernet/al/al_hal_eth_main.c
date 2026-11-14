// SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/**
 *  @{
 * @file   al_hal_eth_main.c
 *
 * @brief  XG Ethernet unit HAL driver for main functions (initialization, data
 * path)
 *
 */

#include "al_hal_eth.h"
#include "al_hal_eth_ec_regs.h"
#include "al_hal_eth_mac_regs.h"
#include <mach/al_hal_udma_config.h>
#include <mach/al_hal_udma_iofic.h>
#include <mach/al_hal_unit_adapter_regs.h>

#define AL_ETH_TX_PKT_UDMA_FLAGS                                               \
  (AL_ETH_TX_FLAGS_NO_SNOOP | AL_ETH_TX_FLAGS_INT)

#define AL_ETH_TX_PKT_META_FLAGS                                               \
  (AL_ETH_TX_FLAGS_IPV4_L3_CSUM | AL_ETH_TX_FLAGS_L4_CSUM |                    \
   AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM | AL_ETH_TX_FLAGS_L2_MACSEC_PKT |           \
   AL_ETH_TX_FLAGS_L2_DIS_FCS | AL_ETH_TX_FLAGS_TSO | AL_ETH_TX_FLAGS_TS)

#define AL_ETH_TX_SRC_VLAN_CNT_SHIFT 5
#define AL_ETH_TX_L4_PROTO_IDX_SHIFT 8
#define AL_ETH_TX_TUNNEL_MODE_SHIFT 18
#define AL_ETH_TX_OUTER_L3_PROTO_SHIFT 20
#define AL_ETH_TX_VLAN_MOD_ADD_SHIFT 22
#define AL_ETH_TX_VLAN_MOD_DEL_SHIFT 24
#define AL_ETH_TX_VLAN_MOD_E_SEL_SHIFT 26
#define AL_ETH_TX_VLAN_MOD_VID_SEL_SHIFT 28
#define AL_ETH_TX_VLAN_MOD_PBIT_SEL_SHIFT 30

/* tx Meta Descriptor defines */
#define AL_ETH_TX_META_STORE (1 << 21)
#define AL_ETH_TX_META_L3_LEN_MASK 0xff
#define AL_ETH_TX_META_L3_OFF_MASK 0xff
#define AL_ETH_TX_META_L3_OFF_SHIFT 8
#define AL_ETH_TX_META_MSS_LSB_VAL_SHIFT 22
#define AL_ETH_TX_META_MSS_MSB_TS_VAL_SHIFT 16
#define AL_ETH_TX_META_OUTER_L3_LEN_MASK 0x1f
#define AL_ETH_TX_META_OUTER_L3_LEN_SHIFT 24
#define AL_ETH_TX_META_OUTER_L3_OFF_HIGH_MASK 0x18
#define AL_ETH_TX_META_OUTER_L3_OFF_HIGH_SHIFT 10
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_MASK 0x07
#define AL_ETH_TX_META_OUTER_L3_OFF_LOW_SHIFT 29

/* Rx Descriptor defines */
#define AL_ETH_RX_L3_PROTO_IDX_MASK 0x1F
#define AL_ETH_RX_L4_PROTO_IDX_MASK 0x1F
#define AL_ETH_RX_L4_PROTO_IDX_SHIFT 8

#define AL_ETH_RX_L3_OFFSET_SHIFT 9
#define AL_ETH_RX_L3_OFFSET_MASK (0x7f << AL_ETH_RX_L3_OFFSET_SHIFT)
#define AL_ETH_RX_HASH_SHIFT 16
#define AL_ETH_RX_HASH_MASK (0xffff << AL_ETH_RX_HASH_SHIFT)
#define AL_S2M_DESC_LEN2_SHIFT 16
#define AL_S2M_DESC_LEN2_MASK (0xff << AL_S2M_DESC_LEN2_SHIFT)

#define ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL 5
#define ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL 7

/* tx gpd defines */
#define AL_ETH_TX_GPD_L3_PROTO_MASK 0x1f
#define AL_ETH_TX_GPD_L3_PROTO_SHIFT 0
#define AL_ETH_TX_GPD_L4_PROTO_MASK 0x1f
#define AL_ETH_TX_GPD_L4_PROTO_SHIFT 5
#define AL_ETH_TX_GPD_TUNNEL_CTRL_MASK 0x7
#define AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT 10
#define AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK 0x3
#define AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT 13
#define AL_ETH_TX_GPD_CAM_DATA_2_SHIFT 32
#define AL_ETH_TX_GPD_CAM_MASK_2_SHIFT 32
#define AL_ETH_TX_GPD_CAM_CTRL_VALID_SHIFT 31

/* tx gcp defines */
#define AL_ETH_TX_GCP_POLY_SEL_MASK 0x1
#define AL_ETH_TX_GCP_POLY_SEL_SHIFT 0
#define AL_ETH_TX_GCP_CRC32_BIT_COMP_MASK 0x1
#define AL_ETH_TX_GCP_CRC32_BIT_COMP_SHIFT 1
#define AL_ETH_TX_GCP_CRC32_BIT_SWAP_MASK 0x1
#define AL_ETH_TX_GCP_CRC32_BIT_SWAP_SHIFT 2
#define AL_ETH_TX_GCP_CRC32_BYTE_SWAP_MASK 0x1
#define AL_ETH_TX_GCP_CRC32_BYTE_SWAP_SHIFT 3
#define AL_ETH_TX_GCP_DATA_BIT_SWAP_MASK 0x1
#define AL_ETH_TX_GCP_DATA_BIT_SWAP_SHIFT 4
#define AL_ETH_TX_GCP_DATA_BYTE_SWAP_MASK 0x1
#define AL_ETH_TX_GCP_DATA_BYTE_SWAP_SHIFT 5
#define AL_ETH_TX_GCP_TRAIL_SIZE_MASK 0xF
#define AL_ETH_TX_GCP_TRAIL_SIZE_SHIFT 6
#define AL_ETH_TX_GCP_HEAD_SIZE_MASK 0xFF
#define AL_ETH_TX_GCP_HEAD_SIZE_SHIFT 16
#define AL_ETH_TX_GCP_HEAD_CALC_MASK 0x1
#define AL_ETH_TX_GCP_HEAD_CALC_SHIFT 24
#define AL_ETH_TX_GCP_MASK_POLARITY_MASK 0x1
#define AL_ETH_TX_GCP_MASK_POLARITY_SHIFT 25

#define AL_ETH_TX_GCP_OPCODE_1_MASK 0x3F
#define AL_ETH_TX_GCP_OPCODE_1_SHIFT 0
#define AL_ETH_TX_GCP_OPCODE_2_MASK 0x3F
#define AL_ETH_TX_GCP_OPCODE_2_SHIFT 6
#define AL_ETH_TX_GCP_OPCODE_3_MASK 0x3F
#define AL_ETH_TX_GCP_OPCODE_3_SHIFT 12
#define AL_ETH_TX_GCP_OPSEL_1_MASK 0xF
#define AL_ETH_TX_GCP_OPSEL_1_SHIFT 0
#define AL_ETH_TX_GCP_OPSEL_2_MASK 0xF
#define AL_ETH_TX_GCP_OPSEL_2_SHIFT 4
#define AL_ETH_TX_GCP_OPSEL_3_MASK 0xF
#define AL_ETH_TX_GCP_OPSEL_3_SHIFT 8
#define AL_ETH_TX_GCP_OPSEL_4_MASK 0xF
#define AL_ETH_TX_GCP_OPSEL_4_SHIFT 12

/*  Tx crc_chksum_replace defines */
#define L4_CHECKSUM_DIS_AND_L3_CHECKSUM_DIS 0x00
#define L4_CHECKSUM_DIS_AND_L3_CHECKSUM_EN 0x20
#define L4_CHECKSUM_EN_AND_L3_CHECKSUM_DIS 0x40
#define L4_CHECKSUM_EN_AND_L3_CHECKSUM_EN 0x60

#define AL_ETH_MDIO_DELAY_PERIOD                                               \
  1 /* micro seconds to wait when polling mdio status */
#define AL_ETH_MDIO_DELAY_COUNT 150 /* number of times to poll */
#define AL_ETH_S2M_UDMA_COMP_COAL_TIMEOUT                                      \
  200 /* Rx descriptors coalescing timeout in SB clocks */

#define AL_ETH_EPE_ENTRIES_NUM 26
static struct al_eth_epe_p_reg_entry al_eth_epe_p_regs[AL_ETH_EPE_ENTRIES_NUM] =
    {{0x0, 0x0, 0x0},
     {0x0, 0x0, 0x1},
     {0x0, 0x0, 0x2},
     {0x0, 0x0, 0x3},
     {0x18100, 0xFFFFF, 0x80000004},
     {0x188A8, 0xFFFFF, 0x80000005},
     {0x99100, 0xFFFFF, 0x80000006},
     {0x98100, 0xFFFFF, 0x80000007},
     {0x10800, 0x7FFFF, 0x80000008},
     {0x20000, 0x73FFF, 0x80000009},
     {0x20000, 0x70000, 0x8000000A},
     {0x186DD, 0x7FFFF, 0x8000000B},
     {0x30600, 0x7FF00, 0x8000000C},
     {0x31100, 0x7FF00, 0x8000000D},
     {0x32F00, 0x7FF00, 0x8000000E},
     {0x32900, 0x7FF00, 0x8000000F},
     {0x105DC, 0x7FFFF, 0x80010010},
     {0x18864, 0x7FFFF, 0x80000011},
     {0x72000, 0x72000, 0x80000012},
     {0x70000, 0x72000, 0x80000013},
     {0x46558, 0x7FFFF, 0x80000001},
     {0x18906, 0x7FFFF, 0x80000015},
     {0x18915, 0x7FFFF, 0x80000016},
     {0x31B00, 0x7FF00, 0x80000017},
     {0x30400, 0x7FF00, 0x80000018},
     {0x0, 0x0, 0x8000001F}};

static struct al_eth_epe_control_entry
    al_eth_epe_control_table[AL_ETH_EPE_ENTRIES_NUM] = {
        {{0x2800000, 0x0, 0x0, 0x0, 0x1, 0x400000}},
        {{0x280004C, 0x746000, 0xA46030, 0xE00000, 0x2, 0x400000}},
        {{0x2800054, 0x746000, 0xA46030, 0x1600000, 0x2, 0x400000}},
        {{0x280005C, 0x746000, 0xA46030, 0x1E00000, 0x2, 0x400000}},
        {{0x2800042, 0xD42000, 0x0, 0x400000, 0x1010412, 0x400000}},
        {{0x2800042, 0xD42000, 0x0, 0x400000, 0x1010412, 0x400000}},
        {{0x2800042, 0xE42000, 0x0, 0x400000, 0x2020002, 0x400000}},
        {{0x2800042, 0xE42000, 0x0, 0x400000, 0x2020002, 0x400000}},
        {{0x280B046, 0x0, 0x6C1008, 0x0, 0x4, 0x406800}},
        {{0x2800049, 0xF44060, 0x1744080, 0x14404, 0x6, 0x400011}},
        {{0x2015049, 0xF44060, 0x1744080, 0x14404, 0x8080007, 0x400011}},
        {{0x280B046, 0xF60040, 0x6C1004, 0x2800000, 0x6, 0x406811}},
        {{0x2815042, 0x1F42000, 0x2042010, 0x1414460, 0x10100009, 0x40B800}},
        {{0x2815042, 0x1F42000, 0x2042010, 0x800000, 0x10100009, 0x40B800}},
        {{0x280B042, 0x0, 0x0, 0x430400, 0x4040009, 0x0}},
        {{0x2815580, 0x0, 0x0, 0x0, 0x4040005, 0x0}},
        {{0x280B000, 0x0, 0x0, 0x0, 0x1, 0x400000}},
        {{0x280B400, 0x0, 0x0, 0x800000, 0x4040003, 0}},
        {{0x280B000, 0x0, 0x0, 0x600000, 0x1, 0x406800}},
        {{0x280B000, 0x0, 0x0, 0xE00000, 0x1, 0x406800}},
        {{0x2800000, 0x0, 0x0, 0x0, 0x1, 0x400000}},
        {{0x280B046, 0x0, 0x0, 0x2800000, 0x7, 0x400000}},
        {{0x280B046, 0xF60040, 0x6C1004, 0x2800000, 0x6, 0x406811}},
        {{0x2815042, 0x1F43028, 0x2000000, 0xC00000, 0x10100009, 0x40B800}},
        {{0x2815400, 0x0, 0x0, 0x0, 0x4040005, 0x0}},
        {{0x2800000, 0x0, 0x0, 0x0, 0x1, 0x400000}}};

#define AL_ETH_IS_1G_MAC(mac_mode)                                             \
  (((mac_mode) == AL_ETH_MAC_MODE_RGMII) ||                                    \
   ((mac_mode) == AL_ETH_MAC_MODE_SGMII))
#define AL_ETH_IS_10G_MAC(mac_mode)                                            \
  (((mac_mode) == AL_ETH_MAC_MODE_10GbE_Serial) ||                             \
   ((mac_mode) == AL_ETH_MAC_MODE_10G_SGMII) ||                                \
   ((mac_mode) == AL_ETH_MAC_MODE_SGMII_2_5G))
#define AL_ETH_IS_25G_MAC(mac_mode) ((mac_mode) == AL_ETH_MAC_MODE_KR_LL_25G)

static const char *al_eth_mac_mode_str(enum al_eth_mac_mode mode) {
  switch (mode) {
  case AL_ETH_MAC_MODE_RGMII:
    return "RGMII";
  case AL_ETH_MAC_MODE_SGMII:
    return "SGMII";
  case AL_ETH_MAC_MODE_SGMII_2_5G:
    return "SGMII_2_5G";
  case AL_ETH_MAC_MODE_10GbE_Serial:
    return "KR";
  case AL_ETH_MAC_MODE_KR_LL_25G:
    return "KR_LL_25G";
  case AL_ETH_MAC_MODE_10G_SGMII:
    return "10G_SGMII";
  case AL_ETH_MAC_MODE_XLG_LL_40G:
    return "40G_LL";
  case AL_ETH_MAC_MODE_XLG_LL_50G:
    return "50G_LL";
  default:
    return "N/A";
  }
}

/**
 * change and wait udma state
 *
 * @param dma the udma to change its state
 * @param new_state
 *
 * @return 0 on success. otherwise on failure.
 */
static int al_udma_state_set_wait(struct al_udma *dma,
                                  enum al_udma_state new_state) {
  enum al_udma_state state;
  enum al_udma_state expected_state = new_state;
  int count = 1000;
  int rc;

  rc = al_udma_state_set(dma, new_state);
  if (rc != 0) {
    al_warn("[%s] warn: failed to change state, error %d\n", dma->name, rc);
    return rc;
  }

  if ((new_state == UDMA_NORMAL) || (new_state == UDMA_DISABLE))
    expected_state = UDMA_IDLE;

  do {
    state = al_udma_state_get(dma);
    if (state == expected_state)
      break;
    al_udelay(1);
    if (count-- == 0) {
      al_warn("[%s] warn: dma state didn't change to %s\n", dma->name,
              al_udma_states_name[new_state]);
      return -ETIMEDOUT;
    }
  } while (1);
  return 0;
}

static void
al_eth_epe_entry_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
                     struct al_eth_epe_p_reg_entry *reg_entry,
                     struct al_eth_epe_control_entry *control_entry) {
  al_reg_write32(&adapter->ec_regs_base->epe_p[idx].comp_data, reg_entry->data);
  al_reg_write32(&adapter->ec_regs_base->epe_p[idx].comp_mask, reg_entry->mask);
  al_reg_write32(&adapter->ec_regs_base->epe_p[idx].comp_ctrl, reg_entry->ctrl);

  al_reg_write32(&adapter->ec_regs_base->msp_c[idx].p_comp_data,
                 reg_entry->data);
  al_reg_write32(&adapter->ec_regs_base->msp_c[idx].p_comp_mask,
                 reg_entry->mask);
  al_reg_write32(&adapter->ec_regs_base->msp_c[idx].p_comp_ctrl,
                 reg_entry->ctrl);

  /*control table  0*/
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_data_6,
                 control_entry->data[5]);
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_data_2,
                 control_entry->data[1]);
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_data_3,
                 control_entry->data[2]);
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_data_4,
                 control_entry->data[3]);
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_data_5,
                 control_entry->data[4]);
  al_reg_write32(&adapter->ec_regs_base->epe[0].act_table_data_1,
                 control_entry->data[0]);

  /*control table 1*/
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_data_6,
                 control_entry->data[5]);
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_data_2,
                 control_entry->data[1]);
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_data_3,
                 control_entry->data[2]);
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_data_4,
                 control_entry->data[3]);
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_data_5,
                 control_entry->data[4]);
  al_reg_write32(&adapter->ec_regs_base->epe[1].act_table_data_1,
                 control_entry->data[0]);

}

static void al_eth_epe_init(struct al_hal_eth_adapter *adapter) {
  int idx;

  if (adapter->enable_rx_parser == 0) {
    al_dbg("eth [%s]: disable rx parser\n", adapter->name);

    al_reg_write32(&adapter->ec_regs_base->epe[0].res_def, 0x08000000);
    al_reg_write32(&adapter->ec_regs_base->epe[0].res_in, 0x7);

    al_reg_write32(&adapter->ec_regs_base->epe[1].res_def, 0x08000000);
    al_reg_write32(&adapter->ec_regs_base->epe[1].res_in, 0x7);

    return;
  }
  al_dbg("eth [%s]: enable rx parser\n", adapter->name);
  for (idx = 0; idx < AL_ETH_EPE_ENTRIES_NUM; idx++)
    al_eth_epe_entry_set(adapter, idx, &al_eth_epe_p_regs[idx],
                         &al_eth_epe_control_table[idx]);

  al_reg_write32(&adapter->ec_regs_base->epe[0].res_def, 0x08000080);
  al_reg_write32(&adapter->ec_regs_base->epe[0].res_in, 0x7);

  al_reg_write32(&adapter->ec_regs_base->epe[1].res_def, 0x08000080);
  al_reg_write32(&adapter->ec_regs_base->epe[1].res_in, 0);

  /* header length as function of 4 bits value, for GRE, when C bit is set, the
   * header len should be increase by 4*/
  al_reg_write32(&adapter->ec_regs_base->epe_h[8].hdr_len, (4 << 16) | 4);

  /* select the outer information when writing the rx descriptor (l3 protocol
   * index etc) */
  al_reg_write32(&adapter->ec_regs_base->rfw.meta, EC_RFW_META_L3_LEN_CALC);

  al_reg_write32(&adapter->ec_regs_base->rfw.checksum, EC_RFW_CHECKSUM_HDR_SEL);
}

/**
 * read 40G MAC registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint32_t al_eth_40g_mac_reg_read(struct al_hal_eth_adapter *adapter,
                                        uint32_t reg_addr) {
  uint32_t val;

  /* indirect access */
  al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_addr, reg_addr);
  val = al_reg_read32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_data);

  al_dbg("[%s]: %s - reg %d. val 0x%x", adapter->name, __func__, reg_addr, val);

  return val;
}

/**
 * write 40G MAC registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_40g_mac_reg_write(struct al_hal_eth_adapter *adapter,
                                     uint32_t reg_addr, uint32_t reg_data) {
  /* indirect access */
  al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_addr, reg_addr);
  al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_data, reg_data);

  al_dbg("[%s]: %s - reg %d. val 0x%x", adapter->name, __func__, reg_addr,
         reg_data);
}

/**
 * write 40G PCS registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_40g_pcs_reg_write(struct al_hal_eth_adapter *adapter,
                                     uint32_t reg_addr, uint32_t reg_data) {
  /* indirect access */
  al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_addr, reg_addr);
  al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_data, reg_data);

  al_dbg("[%s]: %s - reg %d. val 0x%x", adapter->name, __func__, reg_addr,
         reg_data);
}

/*****************************API Functions  **********************************/
/*adapter management */
/**
 * initialize the ethernet adapter's DMA
 */
int al_eth_adapter_init(struct al_hal_eth_adapter *adapter,
                        struct al_eth_adapter_params *params) {
  struct al_udma_params udma_params;
  struct al_udma_m2s_pkt_len_conf conf;
  int i;
  uint32_t reg;
  int rc;

  al_dbg("eth [%s]: initialize controller's UDMA. id = %d\n", params->name,
         params->udma_id);
  al_dbg("eth [%s]: UDMA base regs: %p\n", params->name,
         params->udma_regs_base);
  al_dbg("eth [%s]: EC base regs: %p\n", params->name, params->ec_regs_base);
  al_dbg("eth [%s]: MAC base regs: %p\n", params->name, params->mac_regs_base);
  al_dbg("eth [%s]: enable_rx_parser: %x\n", params->name,
         params->enable_rx_parser);

  adapter->name = params->name;
  adapter->dev_id = params->dev_id;
  adapter->rev_id = params->rev_id;
  adapter->udma_id = params->udma_id;
  adapter->udma_regs_base = params->udma_regs_base;
  adapter->ec_regs_base = (struct al_ec_regs __iomem *)params->ec_regs_base;
  adapter->mac_regs_base =
      (struct al_eth_mac_regs __iomem *)params->mac_regs_base;
  adapter->unit_regs = (struct unit_regs __iomem *)params->udma_regs_base;
  adapter->enable_rx_parser = params->enable_rx_parser;
  adapter->ec_ints_base = ((void __iomem *)adapter->ec_regs_base) + 0x1c00;
  adapter->mac_ints_base = ((void __iomem *)adapter->mac_regs_base) + 0x800;

  /* initialize Tx udma */
  udma_params.udma_reg = (union udma_regs __iomem *)&adapter->unit_regs->m2s;
  udma_params.type = UDMA_TX;
  udma_params.num_of_queues = AL_ETH_UDMA_TX_QUEUES;
  udma_params.name = "eth tx";
  rc = al_udma_init(&adapter->tx_udma, &udma_params);

  if (rc != 0) {
    al_err("failed to initialize %s, error %d\n", udma_params.name, rc);
    return rc;
  }
  rc = al_udma_state_set_wait(&adapter->tx_udma, UDMA_NORMAL);
  if (rc != 0) {
    al_err("[%s]: failed to change state, error %d\n", udma_params.name, rc);
    return rc;
  }
  /* initialize Rx udma */
  udma_params.udma_reg = (union udma_regs __iomem *)&adapter->unit_regs->s2m;
  udma_params.type = UDMA_RX;
  udma_params.num_of_queues = AL_ETH_UDMA_RX_QUEUES;
  udma_params.name = "eth rx";
  rc = al_udma_init(&adapter->rx_udma, &udma_params);

  if (rc != 0) {
    al_err("failed to initialize %s, error %d\n", udma_params.name, rc);
    return rc;
  }

  rc = al_udma_state_set_wait(&adapter->rx_udma, UDMA_NORMAL);
  if (rc != 0) {
    al_err("[%s]: failed to change state, error %d\n", udma_params.name, rc);
    return rc;
  }
  al_dbg("eth [%s]: controller's UDMA successfully initialized\n",
         params->name);

  /* set max packet size to 1M (for TSO) */
  conf.encode_64k_as_zero = AL_TRUE;
  conf.max_pkt_size = 0xfffff;
  al_udma_m2s_packet_size_cfg_set(&adapter->tx_udma, &conf);

  /* set m2s (tx) max descriptors to max data buffers number and one for
   * meta descriptor
   */
  al_udma_m2s_max_descs_set(&adapter->tx_udma, AL_ETH_PKT_MAX_BUFS + 1);

  /* set s2m (rx) max descriptors to max data buffers */
  al_udma_s2m_max_descs_set(&adapter->rx_udma, AL_ETH_PKT_MAX_BUFS);

  /* set s2m burst lenght when writing completion descriptors to 64 bytes
   */
  al_udma_s2m_compl_desc_burst_config(&adapter->rx_udma, 64);

  /* if pointer to ec regs provided, then init the tx meta cache of this udma*/
  if (adapter->ec_regs_base != NULL) {
    // INIT TX CACHE TABLE:
    for (i = 0; i < 4; i++) {
      al_reg_write32(&adapter->ec_regs_base->tso.cache_table_addr,
                     i + (adapter->udma_id * 4));
      al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_1,
                     0x00000000);
      al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_2,
                     0x00000000);
      al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_3,
                     0x00000000);
      al_reg_write32(&adapter->ec_regs_base->tso.cache_table_data_4,
                     0x00000000);
    }
  }
  // only udma 0 allowed to init ec
  if (adapter->udma_id != 0) {
    return 0;
  }
  /* enable Ethernet controller: */
  /* enable internal machines*/
  al_reg_write32(&adapter->ec_regs_base->gen.en, 0xffffffff);
  al_reg_write32(&adapter->ec_regs_base->gen.fifo_en, 0xffffffff);

  if (adapter->rev_id > AL_ETH_REV_ID_0) {
    /* enable A0 descriptor structure */
    al_reg_write32_masked(&adapter->ec_regs_base->gen.en_ext,
                          EC_GEN_EN_EXT_CACHE_WORD_SPLIT,
                          EC_GEN_EN_EXT_CACHE_WORD_SPLIT);

    /* use mss value in the descriptor */
    al_reg_write32(&adapter->ec_regs_base->tso.cfg_add_0,
                   EC_TSO_CFG_ADD_0_MSS_SEL);

    /* enable tunnel TSO */
    al_reg_write32(
        &adapter->ec_regs_base->tso.cfg_tunnel,
        (EC_TSO_CFG_TUNNEL_EN_TUNNEL_TSO | EC_TSO_CFG_TUNNEL_EN_UDP_CHKSUM |
         EC_TSO_CFG_TUNNEL_EN_UDP_LEN | EC_TSO_CFG_TUNNEL_EN_IPV6_PLEN |
         EC_TSO_CFG_TUNNEL_EN_IPV4_CHKSUM | EC_TSO_CFG_TUNNEL_EN_IPV4_IDEN |
         EC_TSO_CFG_TUNNEL_EN_IPV4_TLEN));
  }

  /* swap input byts from MAC RX */
  al_reg_write32(&adapter->ec_regs_base->mac.gen, 0x00000001);
  /* swap output bytes to MAC TX*/
  al_reg_write32(&adapter->ec_regs_base->tmi.tx_cfg,
                 EC_TMI_TX_CFG_EN_FWD_TO_RX | EC_TMI_TX_CFG_SWAP_BYTES);

  /* TODO: check if we need this line*/
  al_reg_write32(&adapter->ec_regs_base->tfw_udma[0].fwd_dec, 0x000003fb);

  /* RFW configuration: default 0 */
  al_reg_write32(&adapter->ec_regs_base->rfw_default[0].opt_1, 0x00000001);

  /* VLAN table address*/
  al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_addr, 0x00000000);
  /* VLAN table data*/
  al_reg_write32(&adapter->ec_regs_base->rfw.vid_table_data, 0x00000000);
  /* HASH config (select toeplitz and bits 7:0 of the thash result, enable
   * symmetric hash) */
  al_reg_write32(&adapter->ec_regs_base->rfw.thash_cfg_1,
                 EC_RFW_THASH_CFG_1_ENABLE_IP_SWAP |
                     EC_RFW_THASH_CFG_1_ENABLE_PORT_SWAP);

  al_eth_epe_init(adapter);

  /* disable TSO padding and use mac padding instead */
  reg = al_reg_read32(&adapter->ec_regs_base->tso.in_cfg);
  reg &= ~0x7F00; /*clear bits 14:8 */
  al_reg_write32(&adapter->ec_regs_base->tso.in_cfg, reg);

  return 0;
}

/**
 * stop the DMA of the ethernet adapter
 */
int al_eth_adapter_stop(struct al_hal_eth_adapter *adapter) {
  int rc;

  al_dbg("eth [%s]: stop controller's UDMA\n", adapter->name);

  /* disable Tx dma*/
  rc = al_udma_state_set_wait(&adapter->tx_udma, UDMA_DISABLE);
  if (rc != 0) {
    al_warn("[%s] warn: failed to change state, error %d\n",
            adapter->tx_udma.name, rc);
    return rc;
  }

  al_dbg("eth [%s]: controller's TX UDMA stopped\n", adapter->name);
  /* disable Rx dma*/
  rc = al_udma_state_set_wait(&adapter->rx_udma, UDMA_DISABLE);
  if (rc != 0) {
    al_warn("[%s] warn: failed to change state, error %d\n",
            adapter->rx_udma.name, rc);
    return rc;
  }

  al_dbg("eth [%s]: controller's RX UDMA stopped\n", adapter->name);
  return 0;
}

/* Q management */
/**
 * Configure and enable a queue ring
 */
int al_eth_queue_config(struct al_hal_eth_adapter *adapter,
                        enum al_udma_type type, uint32_t qid,
                        struct al_udma_q_params *q_params) {
  struct al_udma *udma;
  int rc;

  al_dbg("eth [%s]: config UDMA %s queue %d\n", adapter->name,
         type == UDMA_TX ? "Tx" : "Rx", qid);

  if (type == UDMA_TX) {
    udma = &adapter->tx_udma;
  } else {
    udma = &adapter->rx_udma;
  }

  q_params->dev_id = adapter->dev_id;
  q_params->rev_id = adapter->rev_id;

  rc = al_udma_q_init(udma, qid, q_params);
  if (rc)
    return rc;

  if (type == UDMA_RX)
    rc = al_udma_s2m_q_compl_coal_config(&udma->udma_q[qid], AL_TRUE,
                                         AL_ETH_S2M_UDMA_COMP_COAL_TIMEOUT);
  return rc;
}

int al_eth_queue_enable(struct al_hal_eth_adapter *adapter,
                        enum al_udma_type type, uint32_t qid) {
  struct al_udma *udma;
  int rc;

  al_dbg("eth [%s]: disable UDMA %s queue %d\n", adapter->name,
         type == UDMA_TX ? "Tx" : "Rx", qid);

  if (type == UDMA_TX) {
    udma = &adapter->tx_udma;
  } else {
    udma = &adapter->rx_udma;
  }

  rc = al_udma_q_enable(&udma->udma_q[qid], AL_TRUE);

  return rc;
}
int al_eth_queue_disable(struct al_hal_eth_adapter *adapter,
                         enum al_udma_type type, uint32_t qid) {
  struct al_udma *udma;
  int rc;

  al_dbg("eth [%s]: disable UDMA %s queue %d\n", adapter->name,
         type == UDMA_TX ? "Tx" : "Rx", qid);

  if (type == UDMA_TX) {
    udma = &adapter->tx_udma;
  } else {
    udma = &adapter->rx_udma;
  }

  rc = al_udma_q_enable(&udma->udma_q[qid], AL_FALSE);
  if (rc)
    return rc;

  rc = al_udma_q_reset(&udma->udma_q[qid]);

  return rc;
}

/* MAC layer */
int al_eth_rx_pkt_limit_config(struct al_hal_eth_adapter *adapter,
                               uint32_t min_rx_len, uint32_t max_rx_len) {
  al_assert(max_rx_len <= AL_ETH_MAX_FRAME_LEN);

  /* EC minimum packet length [bytes] in RX */
  al_reg_write32(&adapter->ec_regs_base->mac.min_pkt, min_rx_len);
  /* EC maximum packet length [bytes] in RX */
  al_reg_write32(&adapter->ec_regs_base->mac.max_pkt, max_rx_len);

  if (adapter->rev_id > AL_ETH_REV_ID_2) {
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_1, min_rx_len);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_2, max_rx_len);
  }

  /* configure the MAC's max rx length, add 16 bytes so the packet get
   * trimmed by the EC/Async_fifo rather by the MAC
   */
  if (AL_ETH_IS_1G_MAC(adapter->mac_mode))
    al_reg_write32(&adapter->mac_regs_base->mac_1g.frm_len, max_rx_len + 16);
  else if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
           AL_ETH_IS_25G_MAC(adapter->mac_mode))
    /* 10G MAC control register  */
    al_reg_write32(&adapter->mac_regs_base->mac_10g.frm_len, (max_rx_len + 16));
  else
    al_eth_40g_mac_reg_write(adapter, ETH_MAC_GEN_V3_MAC_40G_FRM_LENGTH_ADDR,
                             (max_rx_len + 16));

  return 0;
}

/* configure the mac media type. */
int al_eth_mac_config(struct al_hal_eth_adapter *adapter,
                      enum al_eth_mac_mode mode) {
  switch (mode) {
  case AL_ETH_MAC_MODE_RGMII:
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x40003210);

    /* 1G MAC control register */
    /* bit[0]  - TX_ENA - zeroed by default. Should be asserted by
     * al_eth_mac_start bit[1]  - RX_ENA - zeroed by default. Should be asserted
     * by al_eth_mac_start bit[3]  - ETH_SPEED - zeroed to enable 10/100 Mbps
     * Ethernet bit[4]  - PROMIS_EN - asserted to enable MAC promiscuous mode
     * bit[23] - CNTL_FRM-ENA - asserted to enable control frames
     * bit[24] - NO_LGTH_CHECK - asserted to disable length checks, which is
     * done in the controller
     */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.cmd_cfg, 0x01800010);

    /* RX_SECTION_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_section_empty,
                   0x00000000);
    /* RX_SECTION_FULL,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_section_full,
                   0x0000000c); /* must be larger than almost empty */
    /* RX_ALMOST_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_almost_empty, 0x00000008);
    /* RX_ALMOST_FULL,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_almost_full, 0x00000008);

    /* TX_SECTION_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_section_empty,
                   0x00000008); /* 8 ? */
    /* TX_SECTION_FULL, 0 - store and forward, */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_section_full, 0x0000000c);
    /* TX_ALMOST_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_almost_empty, 0x00000008);
    /* TX_ALMOST_FULL,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_almost_full, 0x00000008);

    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000000);

    /* 1G MACSET 1G */
    /* taking sel_1000/sel_10 inputs from rgmii PHY, and not from register.
     * disabling magic_packets detection in mac */
    al_reg_write32(&adapter->mac_regs_base->gen.mac_1g_cfg, 0x00000002);
    /* RGMII set 1G */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
    al_reg_write32(&adapter->mac_regs_base->gen.rgmii_sel, 0xF);
    break;
  case AL_ETH_MAC_MODE_SGMII:
    if (adapter->rev_id > AL_ETH_REV_ID_2) {
      /* configure and enable the ASYNC FIFO between the MACs and the EC */
      /* TX min packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_1,
                     0x00000037);
      /* TX max packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_2,
                     0x00002800);
      /* TX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_3,
                     0x00000080);
      /* TX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_4,
                     0x00030020);
      /* TX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_5,
                     0x00000121);
      /* RX min packet size */
      /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_1,
       * 0x00000040); */
      /* RX max packet size */
      /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_2,
       * 0x00002800); */
      /* RX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_3,
                     0x00030020);
      /* RX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_4,
                     0x00000080);
      /* RX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_5,
                     0x00000212);
      /* V3 additional MAC selection */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_sel, 0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_cfg,
                     0x00000001);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_ctrl,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_10g_ll_cfg,
                     0x00000000);
      /* ASYNC FIFO ENABLE */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);
    }

    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x40053210);

    /* 1G MAC control register */
    /* bit[0]  - TX_ENA - zeroed by default. Should be asserted by
     * al_eth_mac_start bit[1]  - RX_ENA - zeroed by default. Should be asserted
     * by al_eth_mac_start bit[3]  - ETH_SPEED - zeroed to enable 10/100 Mbps
     * Ethernet bit[4]  - PROMIS_EN - asserted to enable MAC promiscuous mode
     * bit[23] - CNTL_FRM-ENA - asserted to enable control frames
     * bit[24] - NO_LGTH_CHECK - asserted to disable length checks, which is
     * done in the controller
     */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.cmd_cfg, 0x01800010);

    /* RX_SECTION_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_section_empty,
                   0x00000000);
    /* RX_SECTION_FULL,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_section_full,
                   0x0000000c); /* must be larger than almost empty */
    /* RX_ALMOST_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_almost_empty, 0x00000008);
    /* RX_ALMOST_FULL,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.rx_almost_full, 0x00000008);

    /* TX_SECTION_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_section_empty,
                   0x00000008); /* 8 ? */
    /* TX_SECTION_FULL, 0 - store and forward, */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_section_full, 0x0000000c);
    /* TX_ALMOST_EMPTY,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_almost_empty, 0x00000008);
    /* TX_ALMOST_FULL,  */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.tx_almost_full, 0x00000008);

    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x000000c0);

    /* 1G MACSET 1G */
    /* taking sel_1000/sel_10 inputs from rgmii_converter, and not from
     * register. disabling magic_packets detection in mac */
    al_reg_write32(&adapter->mac_regs_base->gen.mac_1g_cfg, 0x00000002);
    /* SerDes configuration */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

    // FAST AN -- Testing only

    /* Setting PCS i/f mode to SGMII (instead of default 1000Base-X) */
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_addr, 0x00000014);
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_data, 0x0000000b);
    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  case AL_ETH_MAC_MODE_SGMII_2_5G:
    if (adapter->rev_id > AL_ETH_REV_ID_2) {
      /* configure and enable the ASYNC FIFO between the MACs and the EC */
      /* TX min packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_1,
                     0x00000037);
      /* TX max packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_2,
                     0x00002800);
      /* TX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_3,
                     0x00000080);
      /* TX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_4,
                     0x00030020);
      /* TX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_5,
                     0x00000023);
      /* RX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_3,
                     0x00030020);
      /* RX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_4,
                     0x00000080);
      /* RX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_5,
                     0x00000012);
      /* V3 additional MAC selection */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_sel, 0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_cfg,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_ctrl,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_10g_ll_cfg,
                     0x00000050);
      /* ASYNC FIFO ENABLE */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);
    }

    /* MAC register file */
    al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, 0x01022830);
    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000001);
    al_reg_write32(&adapter->mac_regs_base->mac_10g.if_mode, 0x00000028);
    al_reg_write32(&adapter->mac_regs_base->mac_10g.control, 0x00001140);
    /* RXAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_out,
     * 0x00000401); */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_in,
     * 0x00000401); */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x40003210);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  case AL_ETH_MAC_MODE_10GbE_Serial:
    if (adapter->rev_id > AL_ETH_REV_ID_2) {
      /* configure and enable the ASYNC FIFO between the MACs and the EC */
      /* TX min packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_1,
                     0x00000037);
      /* TX max packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_2,
                     0x00002800);
      /* TX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_3,
                     0x00000080);
      /* TX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_4,
                     0x00030020);
      /* TX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_5,
                     0x00000023);
      /* RX min packet size */
      /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_1,
       * 0x00000040); */
      /* RX max packet size */
      /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_2,
       * 0x00002800); */
      /* RX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_3,
                     0x00030020);
      /* RX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_4,
                     0x00000080);
      /* RX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_5,
                     0x00000012);
      /* V3 additional MAC selection */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_sel, 0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_cfg,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_ctrl,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_10g_ll_cfg,
                     0x00000050);
      /* ASYNC FIFO ENABLE */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);
    }

    /* MAC register file */
    al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, 0x01022810);
    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000005);
    /* RXAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.rxaui_cfg, 0x00000007);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_cfg, 0x000001F1);
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_out,
     * 0x00000401); */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_in,
     * 0x00000401); */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00073910);
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x10003210);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  case AL_ETH_MAC_MODE_KR_LL_25G:
    /* select 25G SERDES lane 0 and lane 1 */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.ext_serdes_ctrl, 0x00000003);
    if (adapter->rev_id > AL_ETH_REV_ID_2) {
      /* configure and enable the ASYNC FIFO between the MACs and the EC */
      /* TX min packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_1,
                     0x00000037);
      /* TX max packet size */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_2,
                     0x00002800);
      /* TX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_3,
                     0x00000080);
      /* TX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_4,
                     0x00030020);
      /* TX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_5,
                     0x00000023);
      /* RX min packet size */
      /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_1,
       * 0x00000040); */
      /* RX max packet size */
      /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_2,
       * 0x00002800); */
      /* RX input bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_3,
                     0x00030020);
      /* RX output bus configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_4,
                     0x00000080);
      /* RX Valid/ready configuration */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_5,
                     0x00000012);
      /* V3 additional MAC selection */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_sel, 0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_cfg,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_ctrl,
                     0x00000000);
      al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_10g_ll_cfg,
                     0x000000a0);
      /* ASYNC FIFO ENABLE */
      al_reg_write32(&adapter->mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);
    }

    /* MAC register file */
    al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, 0x01022810);
    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000005);
    /* RXAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.rxaui_cfg, 0x00000007);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_cfg, 0x000001F1);
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_out,
     * 0x00000401); */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_in,
     * 0x00000401); */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00073910);
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x10003210);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x000004f0);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  case AL_ETH_MAC_MODE_10G_SGMII:
    /* MAC register file */
    al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, 0x01022810);

    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000001);

    al_reg_write32(&adapter->mac_regs_base->mac_10g.if_mode, 0x0000002b);
    al_reg_write32(&adapter->mac_regs_base->mac_10g.control, 0x00009140);
    // FAST AN -- Testing only

    /* RXAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.rxaui_cfg, 0x00000007);
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_out,
     * 0x00000401); */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_in,
     * 0x00000401); */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00063910);
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x40003210);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x00000401);

    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  case AL_ETH_MAC_MODE_XLG_LL_40G:
    /* configure and enable the ASYNC FIFO between the MACs and the EC */
    /* TX min packet size */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000037);
    /* TX max packet size */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
    /* TX input bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
    /* TX output bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00010040);
    /* TX Valid/ready configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
    /* RX min packet size */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_1,
     * 0x00000040); */
    /* RX max packet size */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_2,
     * 0x00002800); */
    /* RX input bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00010040);
    /* RX output bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
    /* RX Valid/ready configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000112);
    /* V3 additional MAC selection */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_sel, 0x00000010);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000000);
    /* ASYNC FIFO ENABLE */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

    /* cmd_cfg */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_addr, 0x00000008);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_data, 0x01022810);
    /* speed_ability //Read-Only */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_addr,
     * 0x00000008); */
    /* 40G capable */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_data,
     * 0x00000002); */

    /* XAUI MAC control register */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x06883910);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x0000040f);

    /* MAC register file */
    /*		al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg,
     * 0x01022810); */
    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000005);
    /* RXAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.rxaui_cfg, 0x00000007);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_cfg, 0x000001F1);
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_out,
     * 0x00000401); */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_in,
     * 0x00000401); */
    /*		al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
     * ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00073910); *//* XLG_LL_40G change */
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x10003210);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl,
     * 0x000004f0); *//* XLG_LL_40G change */
    /*		al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl,
     * 0x00000401); *//* XLG_LL_40G change */

    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  case AL_ETH_MAC_MODE_XLG_LL_50G:

    /* configure and enable the ASYNC FIFO between the MACs and the EC */
    /* TX min packet size */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_1, 0x00000037);
    /* TX max packet size */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_2, 0x00002800);
    /* TX input bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_3, 0x00000080);
    /* TX output bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_4, 0x00010040);
    /* TX Valid/ready configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.tx_afifo_cfg_5, 0x00000023);
    /* RX min packet size */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_1,
     * 0x00000040); */
    /* RX max packet size */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_2,
     * 0x00002800); */
    /* RX input bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_3, 0x00010040);
    /* RX output bus configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_4, 0x00000080);
    /* RX Valid/ready configuration */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.rx_afifo_cfg_5, 0x00000112);
    /* V3 additional MAC selection */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_sel, 0x00000010);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_cfg, 0x00000000);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_10g_ll_ctrl, 0x00000000);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_10g_ll_cfg, 0x00000000);
    /* ASYNC FIFO ENABLE */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.afifo_ctrl, 0x00003333);

    /* cmd_cfg */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_addr, 0x00000008);
    al_reg_write32(&adapter->mac_regs_base->gen_v3.mac_40g_ll_data, 0x01022810);
    /* speed_ability //Read-Only */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_addr,
     * 0x00000008); */
    /* 40G capable */
    /* al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_data,
     * 0x00000002); */

    /* select the 25G serdes for lanes 0/1 */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.ext_serdes_ctrl, 0x0000000F);
    /* configure the PCS to work with 2 lanes */
    al_eth_40g_pcs_reg_write(adapter, 0x00010008, 0x0d81);
    /* configure the PCS to work 32 bit interface */
    al_reg_write32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_cfg, 0x00440000);

    /* XAUI MAC control register */
    al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
                          ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x06883910);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl, 0x0000040f);

    /* MAC register file */
    /*		al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg,
     * 0x01022810); */
    /* XAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.cfg, 0x00000005);
    /* RXAUI MAC control register */
    al_reg_write32(&adapter->mac_regs_base->gen.rxaui_cfg, 0x00000007);
    al_reg_write32(&adapter->mac_regs_base->gen.sd_cfg, 0x000001F1);
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_32_64, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_out,
     * 0x00000401); */
    al_reg_write32(&adapter->mac_regs_base->gen.xgmii_dfifo_64_32, 0x00000401);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.mac_res_1_in,
     * 0x00000401); */
    /*		al_reg_write32_masked(&adapter->mac_regs_base->gen.mux_sel,
     * ~ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, 0x00073910); *//* XLG_LL_40G change */
    al_reg_write32(&adapter->mac_regs_base->gen.clk_cfg, 0x10003210);
    /*		al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl,
     * 0x000004f0); *//* XLG_LL_40G change */
    /*		al_reg_write32(&adapter->mac_regs_base->gen.sd_fifo_ctrl,
     * 0x00000401); *//* XLG_LL_40G change */

    al_reg_write32_masked(&adapter->mac_regs_base->gen.led_cfg,
                          ETH_MAC_GEN_LED_CFG_SEL_MASK,
                          ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG);
    break;

  default:
    al_err("Eth: unsupported MAC mode %d", mode);
    return -EPERM;
  }
  adapter->mac_mode = mode;
  al_info("configured MAC to %s mode:\n", al_eth_mac_mode_str(mode));

  return 0;
}

/* start the mac */
int al_eth_mac_start(struct al_hal_eth_adapter *adapter) {
  if (AL_ETH_IS_1G_MAC(adapter->mac_mode)) {
    /* 1G MAC control register */
    al_reg_write32_masked(&adapter->mac_regs_base->mac_1g.cmd_cfg, 0x3, 0x3);
  } else if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
             AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
    /* 10G MAC control register  */
    al_reg_write32_masked(&adapter->mac_regs_base->mac_10g.cmd_cfg, 0x3, 0x3);
  } else {
    uint32_t cmd_cfg;

    cmd_cfg = al_eth_40g_mac_reg_read(
        adapter, ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR);

    cmd_cfg |= (ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_TX_ENA |
                ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_RX_ENA);

    al_eth_40g_mac_reg_write(
        adapter, ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR, cmd_cfg);
  }

  return 0;
}

/* stop the mac */
int al_eth_mac_stop(struct al_hal_eth_adapter *adapter) {
  if (AL_ETH_IS_1G_MAC(adapter->mac_mode))
    /* 1G MAC control register */
    al_reg_write32(&adapter->mac_regs_base->mac_1g.cmd_cfg, 0x0);
  else if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
           AL_ETH_IS_25G_MAC(adapter->mac_mode))
    /* 10G MAC control register  */
    al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, 0x0);
  else
    al_eth_40g_mac_reg_write(adapter,
                             ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR, 0);

  return 0;
}

static void al_eth_mac_link_config_10g_mac(
				struct al_hal_eth_adapter *adapter,
				al_bool force_1000_base_x,
				al_bool an_enable,
				uint32_t speed,
				al_bool full_duplex)
{
	uint32_t if_mode;
	uint32_t val;

	if_mode = al_reg_read32(&adapter->mac_regs_base->mac_10g.if_mode);

	if (force_1000_base_x) {
		uint32_t control;

		AL_REG_MASK_CLEAR(if_mode, ETH_10G_MAC_IF_MODE_SGMII_EN_MASK);

		control = al_reg_read32(&adapter->mac_regs_base->mac_10g.control);

		if (an_enable)
			AL_REG_MASK_SET(control, ETH_10G_MAC_CONTROL_AN_EN_MASK);
		else
			AL_REG_MASK_CLEAR(control, ETH_10G_MAC_CONTROL_AN_EN_MASK);

		al_reg_write32(&adapter->mac_regs_base->mac_10g.control, control);

	} else {
		AL_REG_MASK_SET(if_mode, ETH_10G_MAC_IF_MODE_SGMII_EN_MASK);
		if (an_enable) {
			AL_REG_MASK_SET(if_mode, ETH_10G_MAC_IF_MODE_SGMII_AN_MASK);
		} else {
			AL_REG_MASK_CLEAR(if_mode, ETH_10G_MAC_IF_MODE_SGMII_AN_MASK);

			if (speed == 1000)
				val = ETH_10G_MAC_IF_MODE_SGMII_SPEED_1G;
			else if (speed == 100)
				val = ETH_10G_MAC_IF_MODE_SGMII_SPEED_100M;
			else
				val = ETH_10G_MAC_IF_MODE_SGMII_SPEED_10M;

			AL_REG_FIELD_SET(if_mode,
					 ETH_10G_MAC_IF_MODE_SGMII_SPEED_MASK,
					 ETH_10G_MAC_IF_MODE_SGMII_SPEED_SHIFT,
					 val);

			AL_REG_FIELD_SET(if_mode,
					 ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_MASK,
					 ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_SHIFT,
					 ((full_duplex) ?
						ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_FULL :
						ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_HALF));
		}
	}

	al_reg_write32(&adapter->mac_regs_base->mac_10g.if_mode, if_mode);
}

/* update link speed and duplex mode */
int al_eth_mac_link_config(struct al_hal_eth_adapter *adapter,
                           al_bool force_1000_base_x, al_bool an_enable,
                           uint32_t speed, al_bool full_duplex) {
  uint32_t mac_ctrl;
  uint32_t sgmii_ctrl = 0;
  uint32_t sgmii_if_mode = 0;
  uint32_t rgmii_ctrl = 0;

  if ((!AL_ETH_IS_1G_MAC(adapter->mac_mode)) &&
    (adapter->mac_mode != AL_ETH_MAC_MODE_SGMII_2_5G)) {
    al_err("eth [%s]: this function not supported in this mac mode.\n",
           adapter->name);
    return -EINVAL;
  }

  if ((adapter->mac_mode != AL_ETH_MAC_MODE_RGMII) && (an_enable)) {
    /*
     * an_enable is not relevant to RGMII mode.
     * in AN mode speed and duplex aren't relevant.
     */
    al_info("eth [%s]: set auto negotiation to enable\n", adapter->name);
  } else {
    al_info("eth [%s]: set link speed to %dMbps. %s duplex.\n", adapter->name,
            speed, full_duplex == AL_TRUE ? "full" : "half");

    if ((speed != 10) && (speed != 100) && (speed != 1000)) {
      al_err("eth [%s]: bad speed parameter (%d).\n", adapter->name, speed);
      return -EINVAL;
    }
    if ((speed == 1000) && (full_duplex == AL_FALSE)) {
      al_err("eth [%s]: half duplex in 1Gbps is not supported.\n",
             adapter->name);
      return -EINVAL;
    }
  }

  if (AL_ETH_IS_1G_MAC(adapter->mac_mode)) {
  mac_ctrl = al_reg_read32(&adapter->mac_regs_base->mac_1g.cmd_cfg);

  if (adapter->mac_mode == AL_ETH_MAC_MODE_SGMII) {
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_addr,
                   ETH_MAC_SGMII_REG_ADDR_CTRL_REG);
    sgmii_ctrl = al_reg_read32(&adapter->mac_regs_base->sgmii.reg_data);
    /*
     * in case bit 0 is off in sgmii_if_mode register all the other
     * bits are ignored.
     */
    if (force_1000_base_x == AL_FALSE)
      sgmii_if_mode = ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_EN;

    if (an_enable == AL_TRUE) {
      sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_AN;
      sgmii_ctrl |= ETH_MAC_SGMII_REG_DATA_CTRL_AN_ENABLE;
    } else {
      sgmii_ctrl &= ~(ETH_MAC_SGMII_REG_DATA_CTRL_AN_ENABLE);
    }
  }

  if (adapter->mac_mode == AL_ETH_MAC_MODE_RGMII) {
    /*
     * Use the speed provided by the MAC instead of the PHY
     */
    rgmii_ctrl = al_reg_read32(&adapter->mac_regs_base->gen.rgmii_cfg);

    AL_REG_MASK_CLEAR(rgmii_ctrl, ETH_MAC_GEN_RGMII_CFG_ENA_AUTO);
    AL_REG_MASK_CLEAR(rgmii_ctrl, ETH_MAC_GEN_RGMII_CFG_SET_1000_SEL);
    AL_REG_MASK_CLEAR(rgmii_ctrl, ETH_MAC_GEN_RGMII_CFG_SET_10_SEL);

    al_reg_write32(&adapter->mac_regs_base->gen.rgmii_cfg, rgmii_ctrl);
  }

  if (full_duplex == AL_TRUE) {
    AL_REG_MASK_CLEAR(mac_ctrl, AL_ETH_1G_MAC_CTRL_HD_EN);
  } else {
    AL_REG_MASK_SET(mac_ctrl, AL_ETH_1G_MAC_CTRL_HD_EN);
    sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_DUPLEX;
  }

  if (speed == 1000) {
    AL_REG_MASK_SET(mac_ctrl, AL_ETH_1G_MAC_CTRL_1G_SPD);
    sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_SPEED_1000;
  } else {
    AL_REG_MASK_CLEAR(mac_ctrl, AL_ETH_1G_MAC_CTRL_1G_SPD);
    if (speed == 10) {
      AL_REG_MASK_SET(mac_ctrl, AL_ETH_1G_MAC_CTRL_10M_SPD);
    } else {
      sgmii_if_mode |= ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_SPEED_100;
      AL_REG_MASK_CLEAR(mac_ctrl, AL_ETH_1G_MAC_CTRL_10M_SPD);
    }
  }

  if (adapter->mac_mode == AL_ETH_MAC_MODE_SGMII) {
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_addr,
                   ETH_MAC_SGMII_REG_ADDR_IF_MODE_REG);
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_data, sgmii_if_mode);

    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_addr,
                   ETH_MAC_SGMII_REG_ADDR_CTRL_REG);
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_data, sgmii_ctrl);
  }

  al_reg_write32(&adapter->mac_regs_base->mac_1g.cmd_cfg, mac_ctrl);
    } else {
      al_eth_mac_link_config_10g_mac(adapter, force_1000_base_x,
                   an_enable, speed, full_duplex);
    }

	return 0;
}

/* MDIO */
int al_eth_mdio_config(struct al_hal_eth_adapter *adapter,
                       enum al_eth_mdio_type mdio_type, al_bool shared_mdio_if,
                       enum al_eth_ref_clk_freq ref_clk_freq,
                       unsigned int mdio_clk_freq_khz) {
  enum al_eth_mdio_if mdio_if = AL_ETH_MDIO_IF_10G_MAC;
  const char *if_name =
      (mdio_if == AL_ETH_MDIO_IF_1G_MAC) ? "10/100/1G MAC" : "10G MAC";
  const char *type_name =
      (mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22) ? "Clause 22" : "Clause 45";
  const char *shared_name = (shared_mdio_if == AL_TRUE) ? "Yes" : "No";

  unsigned int ref_clk_freq_khz;
  uint32_t val;

  al_dbg("eth [%s]: mdio config: interface %s. type %s. shared: %s\n",
         adapter->name, if_name, type_name, shared_name);
  adapter->shared_mdio_if = shared_mdio_if;

  val = al_reg_read32(&adapter->mac_regs_base->gen.cfg);
  al_dbg("eth [%s]: mdio config: 10G mac \n", adapter->name);

  switch (mdio_if) {
  case AL_ETH_MDIO_IF_1G_MAC:
    val &= ~AL_BIT(10);
    break;
  case AL_ETH_MDIO_IF_10G_MAC:
    val |= AL_BIT(10);
    break;
  }
  al_reg_write32(&adapter->mac_regs_base->gen.cfg, val);
  adapter->mdio_if = mdio_if;

  if (mdio_if == AL_ETH_MDIO_IF_10G_MAC) {
    val = al_reg_read32(&adapter->mac_regs_base->mac_10g.mdio_cfg_status);
    switch (mdio_type) {
    case AL_ETH_MDIO_TYPE_CLAUSE_22:
      val &= ~AL_BIT(6);
      break;
    case AL_ETH_MDIO_TYPE_CLAUSE_45:
      val |= AL_BIT(6);
      break;
    }

    /* set clock div to get 'mdio_clk_freq_khz' */
    switch (ref_clk_freq) {
    case AL_ETH_REF_FREQ_375_MHZ:
      ref_clk_freq_khz = 375000;
      break;
    case AL_ETH_REF_FREQ_187_5_MHZ:
      ref_clk_freq_khz = 187500;
      break;
    case AL_ETH_REF_FREQ_250_MHZ:
      ref_clk_freq_khz = 250000;
      break;
    case AL_ETH_REF_FREQ_500_MHZ:
      ref_clk_freq_khz = 500000;
      break;
    case AL_ETH_REF_FREQ_428_MHZ:
      ref_clk_freq_khz = 428000;
      break;
    default:
      al_err("eth [%s]: %s: invalid reference clock frequency"
             " (%d)\n",
             adapter->name, __func__, ref_clk_freq);
    };

    val &= ~(0x1FF << 7);
    val |= (ref_clk_freq_khz / (2 * mdio_clk_freq_khz)) << 7;
    AL_REG_FIELD_SET(val, ETH_10G_MAC_MDIO_CFG_HOLD_TIME_MASK,
                     ETH_10G_MAC_MDIO_CFG_HOLD_TIME_SHIFT,
                     ETH_10G_MAC_MDIO_CFG_HOLD_TIME_7_CLK);
    al_reg_write32(&adapter->mac_regs_base->mac_10g.mdio_cfg_status, val);
  } else {
    if (mdio_type != AL_ETH_MDIO_TYPE_CLAUSE_22) {
      al_err("eth [%s] mdio type not supported for this interface\n",
             adapter->name);
      return -EINVAL;
    }
  }
  adapter->mdio_type = mdio_type;

  return 0;
}

static int al_eth_mdio_1g_mac_read(struct al_hal_eth_adapter *adapter,
                                   uint32_t phy_addr
                                   __attribute__((__unused__)),
                                   uint32_t reg, uint16_t *val) {
  *val = al_reg_read32(&adapter->mac_regs_base->mac_1g.phy_regs_base + reg);
  return 0;
}

static int al_eth_mdio_1g_mac_write(struct al_hal_eth_adapter *adapter,
                                    uint32_t phy_addr
                                    __attribute__((__unused__)),
                                    uint32_t reg, uint16_t val) {
  al_reg_write32(&adapter->mac_regs_base->mac_1g.phy_regs_base + reg, val);
  return 0;
}

static int al_eth_mdio_10g_mac_wait_busy(struct al_hal_eth_adapter *adapter) {
  int count = 0;
  uint32_t mdio_cfg_status;

  do {
    mdio_cfg_status =
        al_reg_read32(&adapter->mac_regs_base->mac_10g.mdio_cfg_status);
    /*
                    if (mdio_cfg_status & AL_BIT(1)){ //error
                            al_err(" %s mdio read failed on error. phy_addr 0x%x
       reg 0x%x\n", udma_params.name, phy_addr, reg); return -EIO;
                    }*/
    if (mdio_cfg_status & AL_BIT(0)) {
      if (count > 0)
        al_dbg("eth [%s] mdio: still busy!\n", adapter->name);
    } else {
      return 0;
    }
    al_udelay(AL_ETH_MDIO_DELAY_PERIOD);
  } while (count++ < AL_ETH_MDIO_DELAY_COUNT);

  return -ETIMEDOUT;
}

static int al_eth_mdio_10g_mac_type22(struct al_hal_eth_adapter *adapter,
                                      int read, uint32_t phy_addr, uint32_t reg,
                                      uint16_t *val) {
  int rc;
  const char *op = (read == 1) ? "read" : "write";
  uint32_t mdio_cfg_status;
  uint16_t mdio_cmd;

  // wait if the HW is busy
  rc = al_eth_mdio_10g_mac_wait_busy(adapter);
  if (rc) {
    al_err(" eth [%s] mdio %s failed. HW is busy\n", adapter->name, op);
    return rc;
  }

  mdio_cmd = (uint16_t)(0x1F & reg);
  mdio_cmd |= (0x1F & phy_addr) << 5;

  if (read)
    mdio_cmd |= AL_BIT(15); // READ command

  al_reg_write16(&adapter->mac_regs_base->mac_10g.mdio_cmd, mdio_cmd);
  if (!read)
    al_reg_write16(&adapter->mac_regs_base->mac_10g.mdio_data, *val);

  // wait for the busy to clear
  rc = al_eth_mdio_10g_mac_wait_busy(adapter);
  if (rc != 0) {
    al_err(" %s mdio %s failed on timeout\n", adapter->name, op);
    return -ETIMEDOUT;
  }

  mdio_cfg_status =
      al_reg_read32(&adapter->mac_regs_base->mac_10g.mdio_cfg_status);

  if (mdio_cfg_status & AL_BIT(1)) { // error
    //		al_err(" %s mdio %s failed on error. phy_addr 0x%x reg 0x%x\n",
    //			adapter->name, op, phy_addr, reg);
    return -EIO;
  }
  if (read)
    *val =
        al_reg_read16((uint16_t *)&adapter->mac_regs_base->mac_10g.mdio_data);
  return 0;
}

static int al_eth_mdio_10g_mac_type45(struct al_hal_eth_adapter *adapter,
                                      int read, uint32_t port_addr,
                                      uint32_t device, uint32_t reg,
                                      uint16_t *val) {
  int rc;
  const char *op = (read == 1) ? "read" : "write";
  uint32_t mdio_cfg_status;
  uint16_t mdio_cmd;

  // wait if the HW is busy
  rc = al_eth_mdio_10g_mac_wait_busy(adapter);
  if (rc) {
    al_err(" %s mdio %s failed. HW is busy\n", adapter->name, op);
    return rc;
  }
  // set command register
  mdio_cmd = (uint16_t)(0x1F & device);
  mdio_cmd |= (0x1F & port_addr) << 5;
  al_reg_write16(&adapter->mac_regs_base->mac_10g.mdio_cmd, mdio_cmd);

  // send address frame
  al_reg_write16(&adapter->mac_regs_base->mac_10g.mdio_regaddr, reg);
  // wait for the busy to clear
  rc = al_eth_mdio_10g_mac_wait_busy(adapter);
  if (rc) {
    al_err(" %s mdio %s (address frame) failed on timeout\n", adapter->name,
           op);
    return rc;
  }

  // if read, write again to the command register with READ bit set
  if (read) {
    mdio_cmd |= AL_BIT(15); // READ command
    al_reg_write16((uint16_t *)&adapter->mac_regs_base->mac_10g.mdio_cmd,
                   mdio_cmd);
  } else {
    al_reg_write16((uint16_t *)&adapter->mac_regs_base->mac_10g.mdio_data,
                   *val);
  }
  // wait for the busy to clear
  rc = al_eth_mdio_10g_mac_wait_busy(adapter);
  if (rc) {
    al_err(" %s mdio %s failed on timeout\n", adapter->name, op);
    return rc;
  }

  mdio_cfg_status =
      al_reg_read32(&adapter->mac_regs_base->mac_10g.mdio_cfg_status);

  if (mdio_cfg_status & AL_BIT(1)) { // error
    //		al_err(" %s mdio %s failed on error. port 0x%x, device 0x%x reg
    // 0x%x\n", 			adapter->name, op, port_addr, device,
    // reg);
    return -EIO;
  }
  if (read)
    *val =
        al_reg_read16((uint16_t *)&adapter->mac_regs_base->mac_10g.mdio_data);
  return 0;
}

/**
 * acquire mdio interface ownership
 * when mdio interface shared between multiple eth controllers, this function
 * waits until the ownership granted for this controller. this function does
 * nothing when the mdio interface is used only by this controller.
 *
 * @param adapter
 * @return 0 on success, -ETIMEDOUT  on timeout.
 */
static int al_eth_mdio_lock(struct al_hal_eth_adapter *adapter) {
  int count = 0;
  uint32_t mdio_ctrl_1;

  if (adapter->shared_mdio_if == AL_FALSE)
    return 0; /* nothing to do when interface is not shared */

  do {
    mdio_ctrl_1 = al_reg_read32(&adapter->mac_regs_base->gen.mdio_ctrl_1);
    /*
                    if (mdio_cfg_status & AL_BIT(1)){ //error
                            al_err(" %s mdio read failed on error. phy_addr 0x%x
       reg 0x%x\n", udma_params.name, phy_addr, reg); return -EIO;
                    }*/
    if (mdio_ctrl_1 & AL_BIT(0)) {
      if (count > 0)
        al_dbg("eth %s mdio interface still busy!\n", adapter->name);
    } else {
      return 0;
    }
    al_udelay(AL_ETH_MDIO_DELAY_PERIOD);
  } while (count++ < (AL_ETH_MDIO_DELAY_COUNT * 4));
  al_err(" %s mdio failed to take ownership. MDIO info reg: 0x%08x\n",
         adapter->name, al_reg_read32(&adapter->mac_regs_base->gen.mdio_1));

  return -ETIMEDOUT;
}

/**
 * free mdio interface ownership
 * when mdio interface shared between multiple eth controllers, this function
 * releases the ownership granted for this controller. this function does
 * nothing when the mdio interface is used only by this controller.
 *
 * @param adapter
 * @return 0.
 */
static int al_eth_mdio_free(struct al_hal_eth_adapter *adapter) {
  if (adapter->shared_mdio_if == AL_FALSE)
    return 0; /* nothing to do when interface is not shared */

  al_reg_write32(&adapter->mac_regs_base->gen.mdio_ctrl_1, 0);

  /*
   * Addressing RMN: 2917
   *
   * RMN description:
   * The HW spin-lock is stateless and doesn't maintain any scheduling
   * policy.
   *
   * Software flow:
   * After getting the lock wait 2 times the delay period in order to give
   * the other port chance to take the lock and prevent starvation.
   * This is not scalable to more than two ports.
   */
  al_udelay(2 * AL_ETH_MDIO_DELAY_PERIOD);

  return 0;
}

int al_eth_mdio_read(struct al_hal_eth_adapter *adapter, uint32_t phy_addr,
                     uint32_t device, uint32_t reg, uint16_t *val) {
  int rc;
  rc = al_eth_mdio_lock(adapter);

  /*"interface ownership taken"*/
  if (rc)
    return rc;

  if (adapter->mdio_if == AL_ETH_MDIO_IF_1G_MAC)
    rc = al_eth_mdio_1g_mac_read(adapter, phy_addr, reg, val);
  else if (adapter->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
    rc = al_eth_mdio_10g_mac_type22(adapter, 1, phy_addr, reg, val);
  else
    rc = al_eth_mdio_10g_mac_type45(adapter, 1, phy_addr, device, reg, val);

  al_eth_mdio_free(adapter);
  al_dbg("eth mdio read: phy_addr %x, device %x, reg %x val %x\n", phy_addr,
         device, reg, *val);
  return rc;
}

int al_eth_mdio_write(struct al_hal_eth_adapter *adapter, uint32_t phy_addr,
                      uint32_t device, uint32_t reg, uint16_t val) {
  int rc;
  al_dbg("eth mdio write: phy_addr %x, device %x, reg %x, val %x\n", phy_addr,
         device, reg, val);
  rc = al_eth_mdio_lock(adapter);
  /* interface ownership taken */
  if (rc)
    return rc;

  if (adapter->mdio_if == AL_ETH_MDIO_IF_1G_MAC)
    rc = al_eth_mdio_1g_mac_write(adapter, phy_addr, reg, val);
  else if (adapter->mdio_type == AL_ETH_MDIO_TYPE_CLAUSE_22)
    rc = al_eth_mdio_10g_mac_type22(adapter, 0, phy_addr, reg, &val);
  else
    rc = al_eth_mdio_10g_mac_type45(adapter, 0, phy_addr, device, reg, &val);

  al_eth_mdio_free(adapter);
  return rc;
}

static void al_dump_tx_desc(union al_udma_desc *tx_desc) {
  uint32_t *ptr = (uint32_t *)tx_desc;
  al_dbg("eth tx desc:\n");
  al_dbg("0x%08x\n", *(ptr++));
  al_dbg("0x%08x\n", *(ptr++));
  al_dbg("0x%08x\n", *(ptr++));
  al_dbg("0x%08x\n", *(ptr++));
}

static void al_dump_tx_pkt(struct al_udma_q *tx_dma_q, struct al_eth_pkt *pkt) {
  const char *tso = (pkt->flags & AL_ETH_TX_FLAGS_TSO) ? "TSO" : "";
  const char *l3_csum =
      (pkt->flags & AL_ETH_TX_FLAGS_IPV4_L3_CSUM) ? "L3 CSUM" : "";
  const char *l4_csum =
      (pkt->flags & AL_ETH_TX_FLAGS_L4_CSUM)
          ? ((pkt->flags & AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM) ? "L4 PARTIAL CSUM"
                                                            : "L4 FULL CSUM")
          : "";
  const char *fcs =
      (pkt->flags & AL_ETH_TX_FLAGS_L2_DIS_FCS) ? "Disable FCS" : "";
  const char *ptp = (pkt->flags & AL_ETH_TX_FLAGS_TS) ? "TX_PTP" : "";
  const char *l3_proto_name = "unknown";
  const char *l4_proto_name = "unknown";
  const char *outer_l3_proto_name = "N/A";
  const char *tunnel_mode =
      (pkt->tunnel_mode & AL_ETH_TUNNEL_NO_UDP) ? "TUNNEL" : "";
  uint32_t total_len = 0;
  int i;

  al_dbg("[%s %d]: flags: %s %s %s %s %s %s\n", tx_dma_q->udma->name,
         tx_dma_q->qid, tso, l3_csum, l4_csum, fcs, ptp, tunnel_mode);

  switch (pkt->l3_proto_idx) {
  case AL_ETH_PROTO_ID_IPv4:
    l3_proto_name = "IPv4";
    break;
  case AL_ETH_PROTO_ID_IPv6:
    l3_proto_name = "IPv6";
    break;
  default:
    l3_proto_name = "unknown";
    break;
  }

  switch (pkt->l4_proto_idx) {
  case AL_ETH_PROTO_ID_TCP:
    l4_proto_name = "TCP";
    break;
  case AL_ETH_PROTO_ID_UDP:
    l4_proto_name = "UDP";
    break;
  default:
    l4_proto_name = "unknown";
    break;
  }

  switch (pkt->outer_l3_proto_idx) {
  case AL_ETH_PROTO_ID_IPv4:
    outer_l3_proto_name = "IPv4";
    break;
  case AL_ETH_PROTO_ID_IPv6:
    outer_l3_proto_name = "IPv6";
    break;
  default:
    outer_l3_proto_name = "N/A";
    break;
  }

  al_dbg("[%s %d]: L3 proto: %d (%s). L4 proto: %d (%s). Outer_L3 proto: %d "
         "(%s). vlan source count %d. mod add %d. mod del %d\n",
         tx_dma_q->udma->name, tx_dma_q->qid, pkt->l3_proto_idx, l3_proto_name,
         pkt->l4_proto_idx, l4_proto_name, pkt->outer_l3_proto_idx,
         outer_l3_proto_name, pkt->source_vlan_count, pkt->vlan_mod_add_count,
         pkt->vlan_mod_del_count);

  if (pkt->meta) {
    const char *store = pkt->meta->store ? "Yes" : "No";

    al_dbg("[%s %d]: tx pkt with meta data. words valid %x\n",
           tx_dma_q->udma->name, tx_dma_q->qid, pkt->meta->words_valid);
    if (tx_dma_q->rev_id == AL_ETH_REV_ID_0)
      al_dbg("[%s %d]: meta: store to cache %s. l3 hdr len %d. "
             "l3 hdr offset %d. l4 hdr len %d. mss sel %d\n",
             tx_dma_q->udma->name, tx_dma_q->qid, store,
             pkt->meta->l3_header_len, pkt->meta->l3_header_offset,
             pkt->meta->l4_header_len, pkt->meta->mss_idx_sel);
    else {
      const char *ptp_val = (pkt->flags & AL_ETH_TX_FLAGS_TS) ? "Yes" : "No";
      al_dbg("[%s %d]: meta: store to cache %s. l3 hdr len %d. "
             "l3 hdr offset %d. l4 hdr len %d. mss val %d ts_index %d "
             "ts_val:%s \n",
             tx_dma_q->udma->name, tx_dma_q->qid, store,
             pkt->meta->l3_header_len, pkt->meta->l3_header_offset,
             pkt->meta->l4_header_len, pkt->meta->mss_val, pkt->meta->ts_index,
             ptp_val);
      al_dbg("outer_l3_hdr_offset %d. outer_l3_len %d. \n",
             pkt->meta->outer_l3_len, pkt->meta->outer_l3_offset);
    }
  }

  al_dbg("[%s %d]: num of bufs: %d\n", tx_dma_q->udma->name, tx_dma_q->qid,
         pkt->num_of_bufs);
  for (i = 0; i < pkt->num_of_bufs; i++) {
    al_dbg("eth [%s %d]: buf[%d]: len 0x%08x. address 0x%016llx\n",
           tx_dma_q->udma->name, tx_dma_q->qid, i, pkt->bufs[i].len,
           (unsigned long long)pkt->bufs[i].addr);
    total_len += pkt->bufs[i].len;
  }
  al_dbg("[%s %d]: total len: 0x%08x\n", tx_dma_q->udma->name, tx_dma_q->qid,
         total_len);
}

/* TX */
/**
 * add packet to transmission queue
 */
int al_eth_tx_pkt_prepare(struct al_udma_q *tx_dma_q, struct al_eth_pkt *pkt) {
  union al_udma_desc *tx_desc;
  uint32_t tx_descs;
  uint32_t flags = AL_M2S_DESC_FIRST | AL_M2S_DESC_CONCAT;
  uint64_t vmid = ((uint64_t)pkt->vmid) << AL_UDMA_DESC_VMID_SHIFT;
  uint32_t meta_ctrl;
  uint32_t ring_id;
  int buf_idx;

  al_dbg("[%s %d]: new tx pkt\n", tx_dma_q->udma->name, tx_dma_q->qid);

  al_dump_tx_pkt(tx_dma_q, pkt);

  tx_descs = pkt->num_of_bufs;
  if (pkt->meta) {
    tx_descs += 1;
  }
  if (unlikely(al_udma_available_get(tx_dma_q) < tx_descs)) {
    al_dbg("[%s %d]: failed to allocate (%d) descriptors", tx_dma_q->udma->name,
           tx_dma_q->qid, tx_descs);
    return 0;
  }
  if (pkt->meta) {
    uint32_t meta_word_0 = pkt->flags & AL_ETH_TX_FLAGS_INT;
    uint32_t meta_word_1 = 0;
    uint32_t meta_word_2 = 0;
    uint32_t meta_word_3 = 0;

    meta_word_0 |= AL_M2S_DESC_FIRST | AL_M2S_DESC_META_DATA;
    flags &= ~(AL_M2S_DESC_FIRST | AL_ETH_TX_FLAGS_INT);

    tx_desc = al_udma_desc_get(tx_dma_q);
    /* get ring id, and clear FIRST and Int flags */
    ring_id = al_udma_ring_id_get(tx_dma_q) << AL_M2S_DESC_RING_ID_SHIFT;

    meta_word_0 |= ring_id;
    meta_word_0 |= pkt->meta->words_valid << 12;

    if (pkt->meta->store)
      meta_word_0 |= AL_ETH_TX_META_STORE;

    if (pkt->meta->words_valid & 1) {
      meta_word_0 |= pkt->meta->vlan1_cfi_sel;
      meta_word_0 |= pkt->meta->vlan2_vid_sel << 2;
      meta_word_0 |= pkt->meta->vlan2_cfi_sel << 4;
      meta_word_0 |= pkt->meta->vlan2_pbits_sel << 6;
      meta_word_0 |= pkt->meta->vlan2_ether_sel << 8;
    }

    if (pkt->meta->words_valid & 2) {
      meta_word_1 = pkt->meta->vlan1_new_vid;
      meta_word_1 |= pkt->meta->vlan1_new_cfi << 12;
      meta_word_1 |= pkt->meta->vlan1_new_pbits << 13;
      meta_word_1 |= pkt->meta->vlan2_new_vid << 16;
      meta_word_1 |= pkt->meta->vlan2_new_cfi << 28;
      meta_word_1 |= pkt->meta->vlan2_new_pbits << 29;
    }

    if (pkt->meta->words_valid & 4) {
      meta_word_2 = pkt->meta->l3_header_len & AL_ETH_TX_META_L3_LEN_MASK;
      meta_word_2 |= (pkt->meta->l3_header_offset & AL_ETH_TX_META_L3_OFF_MASK)
                     << AL_ETH_TX_META_L3_OFF_SHIFT;
      meta_word_2 |= (pkt->meta->l4_header_len & 0x3f) << 16;

      if (tx_dma_q->rev_id == AL_ETH_REV_ID_0) {
        meta_word_2 |= (pkt->meta->mss_idx_sel & 7) << 24;
      } else {
        uint32_t l3_offset;

        if (unlikely(pkt->flags & AL_ETH_TX_FLAGS_TS))
          meta_word_0 |= pkt->meta->ts_index
                         << AL_ETH_TX_META_MSS_MSB_TS_VAL_SHIFT;
        else
          meta_word_0 |= (((pkt->meta->mss_val & 0x3c00) >> 10)
                          << AL_ETH_TX_META_MSS_MSB_TS_VAL_SHIFT);
        meta_word_2 |=
            ((pkt->meta->mss_val & 0x03ff) << AL_ETH_TX_META_MSS_LSB_VAL_SHIFT);

        /*
         * move from bytes to multiplication of 2 as the HW
         * expect to get it
         */
        l3_offset = (pkt->meta->outer_l3_offset >> 1);

        meta_word_0 |=
            (((l3_offset & AL_ETH_TX_META_OUTER_L3_OFF_HIGH_MASK) >> 3)
             << AL_ETH_TX_META_OUTER_L3_OFF_HIGH_SHIFT);

        meta_word_3 |= ((l3_offset & AL_ETH_TX_META_OUTER_L3_OFF_LOW_MASK)
                        << AL_ETH_TX_META_OUTER_L3_OFF_LOW_SHIFT);

        /*
         * shift right 2 bits to work in multiplication of 4
         * as the HW expect to get it
         */
        meta_word_3 |=
            (((pkt->meta->outer_l3_len >> 2) & AL_ETH_TX_META_OUTER_L3_LEN_MASK)
             << AL_ETH_TX_META_OUTER_L3_LEN_SHIFT);
      }
    }

    tx_desc->tx_meta.len_ctrl = swap32_to_le(meta_word_0);
    tx_desc->tx_meta.meta_ctrl = swap32_to_le(meta_word_1);
    tx_desc->tx_meta.meta1 = swap32_to_le(meta_word_2);
    tx_desc->tx_meta.meta2 = swap32_to_le(meta_word_3);
    al_dump_tx_desc(tx_desc);
  }

  meta_ctrl = pkt->flags & AL_ETH_TX_PKT_META_FLAGS;

  /* L4_PARTIAL_CSUM without L4_CSUM is invalid option  */
  al_assert((pkt->flags &
             (AL_ETH_TX_FLAGS_L4_CSUM | AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM)) !=
            AL_ETH_TX_FLAGS_L4_PARTIAL_CSUM);

  /* TSO packets can't have Timestamp enabled */
  al_assert((pkt->flags & (AL_ETH_TX_FLAGS_TSO | AL_ETH_TX_FLAGS_TS)) !=
            (AL_ETH_TX_FLAGS_TSO | AL_ETH_TX_FLAGS_TS));

  meta_ctrl |= pkt->l3_proto_idx;
  meta_ctrl |= pkt->l4_proto_idx << AL_ETH_TX_L4_PROTO_IDX_SHIFT;
  meta_ctrl |= pkt->source_vlan_count << AL_ETH_TX_SRC_VLAN_CNT_SHIFT;
  meta_ctrl |= pkt->vlan_mod_add_count << AL_ETH_TX_VLAN_MOD_ADD_SHIFT;
  meta_ctrl |= pkt->vlan_mod_del_count << AL_ETH_TX_VLAN_MOD_DEL_SHIFT;
  meta_ctrl |= pkt->vlan_mod_v1_ether_sel << AL_ETH_TX_VLAN_MOD_E_SEL_SHIFT;
  meta_ctrl |= pkt->vlan_mod_v1_vid_sel << AL_ETH_TX_VLAN_MOD_VID_SEL_SHIFT;
  meta_ctrl |= pkt->vlan_mod_v1_pbits_sel << AL_ETH_TX_VLAN_MOD_PBIT_SEL_SHIFT;

  if (tx_dma_q->rev_id > AL_ETH_REV_ID_0) {
    meta_ctrl |= pkt->tunnel_mode << AL_ETH_TX_TUNNEL_MODE_SHIFT;
    if (pkt->outer_l3_proto_idx == AL_ETH_PROTO_ID_IPv4)
      meta_ctrl |= 1 << AL_ETH_TX_OUTER_L3_PROTO_SHIFT;
  }

  flags |= pkt->flags & AL_ETH_TX_PKT_UDMA_FLAGS;
  for (buf_idx = 0; buf_idx < pkt->num_of_bufs; buf_idx++) {
    uint32_t flags_len = flags;

    tx_desc = al_udma_desc_get(tx_dma_q);
    /* get ring id, and clear FIRST and Int flags */
    ring_id = al_udma_ring_id_get(tx_dma_q) << AL_M2S_DESC_RING_ID_SHIFT;

    flags_len |= ring_id;

    if (buf_idx == (pkt->num_of_bufs - 1))
      flags_len |= AL_M2S_DESC_LAST;

    /* clear First and Int flags */
    flags &= AL_ETH_TX_FLAGS_NO_SNOOP;
    flags |= AL_M2S_DESC_CONCAT;

    flags_len |= pkt->bufs[buf_idx].len & AL_M2S_DESC_LEN_MASK;
    tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
    if (buf_idx == 0)
      tx_desc->tx.meta_ctrl = swap32_to_le(meta_ctrl);
    tx_desc->tx.buf_ptr = swap64_to_le(pkt->bufs[buf_idx].addr | vmid);
    al_dump_tx_desc(tx_desc);
  }

  al_dbg("[%s %d]: pkt descriptors written into the tx queue. descs num (%d)\n",
         tx_dma_q->udma->name, tx_dma_q->qid, tx_descs);

  return tx_descs;
}

void al_eth_tx_dma_action(struct al_udma_q *tx_dma_q, uint32_t tx_descs) {
  /* add tx descriptors */
  al_udma_desc_action_add(tx_dma_q, tx_descs);
}

/**
 * get number of completed tx descriptors, upper layer should derive from
 */
int al_eth_comp_tx_get(struct al_udma_q *tx_dma_q) {
  int rc;

  rc = al_udma_cdesc_get_all(tx_dma_q, NULL);
  if (rc != 0) {
    al_udma_cdesc_ack(tx_dma_q, rc);
    al_dbg("[%s %d]: tx completion: descs (%d)\n", tx_dma_q->udma->name,
           tx_dma_q->qid, rc);
  }

  return rc;
}

/**
 * configure the TSO MSS val
 */
int al_eth_tso_mss_config(struct al_hal_eth_adapter *adapter, uint8_t idx,
                          uint32_t mss_val) {

  al_assert(idx <= 8);                          /*valid MSS index*/
  al_assert(mss_val <= AL_ETH_TSO_MSS_MAX_VAL); /*valid MSS val*/
  al_assert(mss_val >= AL_ETH_TSO_MSS_MIN_VAL); /*valid MSS val*/

  al_reg_write32(&adapter->ec_regs_base->tso_sel[idx].mss, mss_val);
  return 0;
}

/**
 * add buffer to receive queue
 */
int al_eth_rx_buffer_add(struct al_udma_q *rx_dma_q, struct al_buf *buf,
                         uint32_t flags, struct al_buf *header_buf) {
  uint64_t vmid = ((uint64_t)flags & AL_ETH_RX_FLAGS_VMID_MASK)
                  << AL_UDMA_DESC_VMID_SHIFT;
  uint32_t flags_len = flags & ~AL_ETH_RX_FLAGS_VMID_MASK;
  union al_udma_desc *rx_desc;

  al_dbg("[%s %d]: add rx buffer.\n", rx_dma_q->udma->name, rx_dma_q->qid);

  if (unlikely(al_udma_available_get(rx_dma_q) < 1)) {
    al_dbg("[%s]: rx q (%d) has no enough free descriptor",
           rx_dma_q->udma->name, rx_dma_q->qid);
    return -ENOSPC;
  }
  rx_desc = al_udma_desc_get(rx_dma_q);

  flags_len |= al_udma_ring_id_get(rx_dma_q) << AL_S2M_DESC_RING_ID_SHIFT;
  flags_len |= buf->len & AL_S2M_DESC_LEN_MASK;

  if (flags & AL_ETH_RX_FLAGS_DUAL_BUF) {
    al_assert(header_buf != NULL); /*header valid in dual buf */
    al_assert(
        AL_ADDR_HIGH(buf->addr) ==
        AL_ADDR_HIGH(
            header_buf->addr)); /* high bits of addresses must be the same */

    flags_len |=
        (header_buf->len << AL_S2M_DESC_LEN_SHIFT) & AL_S2M_DESC_LEN_MASK;
    rx_desc->rx.buf2_ptr_lo = swap32_to_le(AL_ADDR_LOW(header_buf->addr));
  }
  rx_desc->rx.len_ctrl = swap32_to_le(flags_len);
  rx_desc->rx.buf1_ptr = swap64_to_le(buf->addr | vmid);

  return 0;
}

/**
 * notify the hw engine about rx descriptors that were added to the receive
 * queue
 */
void al_eth_rx_buffer_action(struct al_udma_q *rx_dma_q, uint32_t descs_num) {
  al_dbg("[%s]: update the rx engine tail pointer: queue %d. descs %d\n",
         rx_dma_q->udma->name, rx_dma_q->qid, descs_num);

  /* add rx descriptor */
  al_udma_desc_action_add(rx_dma_q, descs_num);
}

/**
 * get packet from RX completion ring
 */
uint32_t al_eth_pkt_rx(struct al_udma_q *rx_dma_q, struct al_eth_pkt *pkt) {
  volatile union al_udma_cdesc *cdesc;
  al_eth_rx_cdesc *rx_desc;
  uint32_t i;
  uint32_t rc;

  rc = al_udma_cdesc_packet_get(rx_dma_q, &cdesc);
  if (rc == 0)
    return 0;

  al_assert(rc <= AL_ETH_PKT_MAX_BUFS);

  al_dbg("[%s]: fetch rx packet: queue %d.\n", rx_dma_q->udma->name,
         rx_dma_q->qid);

  for (i = 0; i < rc; i++) {
    uint32_t len;

    /* get next descriptor */
    rx_desc = (al_eth_rx_cdesc *)al_cdesc_next(rx_dma_q, cdesc, i);

    len = swap32_from_le(rx_desc->len);

    if ((i == 0) && (swap32_from_le(rx_desc->word2) & AL_UDMA_CDESC_BUF2_USED))
      pkt->rx_header_len =
          (len & AL_S2M_DESC_LEN2_MASK) >> AL_S2M_DESC_LEN2_SHIFT;

    pkt->bufs[i].len = len & AL_S2M_DESC_LEN_MASK;
  }
  /* get flags from last desc */
  pkt->flags = swap32_from_le(rx_desc->ctrl_meta);
  /* update L3/L4 proto index */
  pkt->l3_proto_idx = pkt->flags & AL_ETH_RX_L3_PROTO_IDX_MASK;
  pkt->l4_proto_idx = (pkt->flags >> AL_ETH_RX_L4_PROTO_IDX_SHIFT) &
                      AL_ETH_RX_L4_PROTO_IDX_MASK;
  pkt->rxhash = (swap32_from_le(rx_desc->len) & AL_ETH_RX_HASH_MASK) >>
                AL_ETH_RX_HASH_SHIFT;
  pkt->l3_offset =
      (swap32_from_le(rx_desc->word2) & AL_ETH_RX_L3_OFFSET_MASK) >>
      AL_ETH_RX_L3_OFFSET_SHIFT;

  al_udma_cdesc_ack(rx_dma_q, rc);
  return rc;
}

int al_eth_rx_parser_entry_update(
    struct al_hal_eth_adapter *adapter, uint32_t idx,
    struct al_eth_epe_p_reg_entry *reg_entry,
    struct al_eth_epe_control_entry *control_entry) {
  al_eth_epe_entry_set(adapter, idx, reg_entry, control_entry);
  return 0;
}

#define AL_ETH_THASH_UDMA_SHIFT 0
#define AL_ETH_THASH_UDMA_MASK (0xF << AL_ETH_THASH_UDMA_SHIFT)

#define AL_ETH_THASH_Q_SHIFT 4
#define AL_ETH_THASH_Q_MASK (0x3 << AL_ETH_THASH_Q_SHIFT)

int al_eth_thash_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
                           uint8_t udma, uint32_t queue) {
  uint32_t entry;
  al_assert(idx < AL_ETH_RX_THASH_TABLE_SIZE); /*valid THASH index*/

  entry = (udma << AL_ETH_THASH_UDMA_SHIFT) & AL_ETH_THASH_UDMA_MASK;
  entry |= (queue << AL_ETH_THASH_Q_SHIFT) & AL_ETH_THASH_Q_MASK;

  al_reg_write32(&adapter->ec_regs_base->rfw.thash_table_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->rfw.thash_table_data, entry);
  return 0;
}

int al_eth_fsm_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
                         uint32_t entry) {

  al_assert(idx < AL_ETH_RX_FSM_TABLE_SIZE); /*valid FSM index*/

  al_reg_write32(&adapter->ec_regs_base->rfw.fsm_table_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->rfw.fsm_table_data, entry);
  return 0;
}

static uint32_t
al_eth_fwd_ctrl_entry_to_val(struct al_eth_fwd_ctrl_table_entry *entry) {
  uint32_t val = 0;
  AL_REG_FIELD_SET(val, AL_FIELD_MASK(3, 0), 0, entry->prio_sel);
  AL_REG_FIELD_SET(val, AL_FIELD_MASK(7, 4), 4, entry->queue_sel_1);
  AL_REG_FIELD_SET(val, AL_FIELD_MASK(9, 8), 8, entry->queue_sel_2);
  AL_REG_FIELD_SET(val, AL_FIELD_MASK(13, 10), 10, entry->udma_sel);
  AL_REG_FIELD_SET(val, AL_FIELD_MASK(18, 15), 15, entry->hdr_split_len_sel);
  AL_REG_BIT_VAL_SET(val, 19, !!(entry->filter == AL_TRUE));

  return val;
}

int al_eth_ctrl_table_def_set(struct al_hal_eth_adapter *adapter,
                              al_bool use_table,
                              struct al_eth_fwd_ctrl_table_entry *entry) {
  uint32_t val = al_eth_fwd_ctrl_entry_to_val(entry);

  if (use_table)
    val |= EC_RFW_CTRL_TABLE_DEF_SEL;

  al_reg_write32(&adapter->ec_regs_base->rfw.ctrl_table_def, val);

  return 0;
}

int al_eth_hash_key_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
                        uint32_t val) {

  al_assert(idx < AL_ETH_RX_HASH_KEY_NUM); /*valid CTRL index*/

  al_reg_write32(&adapter->ec_regs_base->rfw_hash[idx].key, val);

  return 0;
}

static uint32_t
al_eth_fwd_mac_table_entry_to_val(struct al_eth_fwd_mac_table_entry *entry) {
  uint32_t val = 0;

  val |= (entry->filter == AL_TRUE) ? EC_FWD_MAC_CTRL_RX_VAL_DROP : 0;
  val |= ((entry->udma_mask << EC_FWD_MAC_CTRL_RX_VAL_UDMA_SHIFT) &
          EC_FWD_MAC_CTRL_RX_VAL_UDMA_MASK);

  val |= ((entry->qid << EC_FWD_MAC_CTRL_RX_VAL_QID_SHIFT) &
          EC_FWD_MAC_CTRL_RX_VAL_QID_MASK);

  val |= (entry->rx_valid == AL_TRUE) ? EC_FWD_MAC_CTRL_RX_VALID : 0;

  val |= ((entry->tx_target << EC_FWD_MAC_CTRL_TX_VAL_SHIFT) &
          EC_FWD_MAC_CTRL_TX_VAL_MASK);

  val |= (entry->tx_valid == AL_TRUE) ? EC_FWD_MAC_CTRL_TX_VALID : 0;

  return val;
}

int al_eth_fwd_mac_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
                             struct al_eth_fwd_mac_table_entry *entry) {
  uint32_t val;

  al_assert(idx < AL_ETH_FWD_MAC_NUM); /*valid FWD MAC index */

  val = (entry->addr[2] << 24) | (entry->addr[3] << 16) |
        (entry->addr[4] << 8) | entry->addr[5];
  al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].data_l, val);
  val = (entry->addr[0] << 8) | entry->addr[1];
  al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].data_h, val);
  val = (entry->mask[2] << 24) | (entry->mask[3] << 16) |
        (entry->mask[4] << 8) | entry->mask[5];
  al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].mask_l, val);
  val = (entry->mask[0] << 8) | entry->mask[1];
  al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].mask_h, val);

  val = al_eth_fwd_mac_table_entry_to_val(entry);
  al_reg_write32(&adapter->ec_regs_base->fwd_mac[idx].ctrl, val);
  return 0;
}

int al_eth_mac_addr_store(void *__iomem ec_base, uint32_t idx, uint8_t *addr) {
  struct al_ec_regs __iomem *ec_regs_base =
      (struct al_ec_regs __iomem *)ec_base;
  uint32_t val;

  al_assert(idx < AL_ETH_FWD_MAC_NUM); /*valid FWD MAC index */

  val = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5];
  al_reg_write32(&ec_regs_base->fwd_mac[idx].data_l, val);
  val = (addr[0] << 8) | addr[1];
  al_reg_write32(&ec_regs_base->fwd_mac[idx].data_h, val);
  return 0;
}

int al_eth_mac_addr_read(void *__iomem ec_base, uint32_t idx, uint8_t *addr) {
  struct al_ec_regs __iomem *ec_regs_base =
      (struct al_ec_regs __iomem *)ec_base;
  uint32_t addr_lo = al_reg_read32(&ec_regs_base->fwd_mac[idx].data_l);
  uint16_t addr_hi = al_reg_read32(&ec_regs_base->fwd_mac[idx].data_h);

  addr[5] = addr_lo & 0xff;
  addr[4] = (addr_lo >> 8) & 0xff;
  addr[3] = (addr_lo >> 16) & 0xff;
  addr[2] = (addr_lo >> 24) & 0xff;
  addr[1] = addr_hi & 0xff;
  addr[0] = (addr_hi >> 8) & 0xff;
  return 0;
}

int al_eth_fwd_pbits_table_set(struct al_hal_eth_adapter *adapter, uint32_t idx,
                               uint8_t prio) {

  al_assert(idx < AL_ETH_FWD_PBITS_TABLE_NUM); /* valid PBIT index */
  al_assert(prio < AL_ETH_FWD_PRIO_TABLE_NUM); /* valid PRIO index */
  al_reg_write32(&adapter->ec_regs_base->rfw.pbits_table_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->rfw.pbits_table_data, prio);
  return 0;
}

int al_eth_fwd_priority_table_set(struct al_hal_eth_adapter *adapter,
                                  uint8_t prio, uint8_t qid) {
  al_assert(prio < AL_ETH_FWD_PRIO_TABLE_NUM); /* valid PRIO index */

  al_reg_write32(&adapter->ec_regs_base->rfw_priority[prio].queue, qid);
  return 0;
}

int al_eth_flow_control_config(struct al_hal_eth_adapter *adapter,
                               struct al_eth_flow_control_params *params) {
  uint32_t reg;
  int i;
  al_assert(params); /* valid params pointer */

  switch (params->type) {
  case AL_ETH_FLOW_CONTROL_TYPE_LINK_PAUSE:
    al_dbg("[%s]: config flow control to link pause mode.\n", adapter->name);

    /* config the mac */
    if (AL_ETH_IS_1G_MAC(adapter->mac_mode)) {
      /* set quanta value */
      al_reg_write32(&adapter->mac_regs_base->mac_1g.pause_quant,
                     params->quanta);
      al_reg_write32(&adapter->ec_regs_base->efc.xoff_timer_1g,
                     params->quanta_th);

    } else if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
               AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
      /* set quanta value */
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl01_pause_quanta,
                     params->quanta);
      /* set quanta threshold value */
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl01_quanta_thresh,
                     params->quanta_th);
    } else {
      /* set quanta value */
      al_eth_40g_mac_reg_write(adapter,
                               ETH_MAC_GEN_V3_MAC_40G_CL01_PAUSE_QUANTA_ADDR,
                               params->quanta);
      /* set quanta threshold value */
      al_eth_40g_mac_reg_write(adapter,
                               ETH_MAC_GEN_V3_MAC_40G_CL01_QUANTA_THRESH_ADDR,
                               params->quanta_th);
    }

    if (params->obay_enable == AL_TRUE)
      /* Tx path FIFO, unmask pause_on from MAC when PAUSE packet received */
      al_reg_write32(&adapter->ec_regs_base->efc.ec_pause, 1);
    else
      al_reg_write32(&adapter->ec_regs_base->efc.ec_pause, 0);

    /* Rx path */
    if (params->gen_enable == AL_TRUE)
      /* enable generating xoff from ec fifo almost full indication in
       * hysteresis mode */
      al_reg_write32(&adapter->ec_regs_base->efc.ec_xoff,
                     1 << EC_EFC_EC_XOFF_MASK_2_SHIFT);
    else
      al_reg_write32(&adapter->ec_regs_base->efc.ec_xoff, 0);

    if (AL_ETH_IS_1G_MAC(adapter->mac_mode))
      /* in 1G mode, enable generating xon from ec fifo in hysteresis mode*/
      al_reg_write32(&adapter->ec_regs_base->efc.xon,
                     EC_EFC_XON_MASK_2 | EC_EFC_XON_MASK_1);

    /* set hysteresis mode thresholds */
    al_reg_write32(
        &adapter->ec_regs_base->efc.rx_fifo_hyst,
        params->rx_fifo_th_low |
            (params->rx_fifo_th_high << EC_EFC_RX_FIFO_HYST_TH_HIGH_SHIFT));

    for (i = 0; i < 4; i++) {
      if (params->obay_enable == AL_TRUE)
        /* Tx path UDMA, unmask pause_on for all queues */
        al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_pause_0,
                       params->prio_q_map[i][0]);
      else
        al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_pause_0, 0);

      if (params->gen_enable == AL_TRUE)
        /* Rx path UDMA, enable generating xoff from UDMA queue almost full
         * indication */
        al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_xoff_0,
                       params->prio_q_map[i][0]);
      else
        al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_xoff_0, 0);
    }
    break;
  case AL_ETH_FLOW_CONTROL_TYPE_PFC:
    al_dbg("[%s]: config flow control to PFC mode.\n", adapter->name);
    al_assert(!AL_ETH_IS_1G_MAC(
        adapter->mac_mode)); /* pfc not available for RGMII mode */
    ;

    for (i = 0; i < 4; i++) {
      int prio;
      for (prio = 0; prio < 8; prio++) {
        if (params->obay_enable == AL_TRUE)
          /* Tx path UDMA, unmask pause_on for all queues */
          al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_pause_0 + prio,
                         params->prio_q_map[i][prio]);
        else
          al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_pause_0 + prio,
                         0);

        if (params->gen_enable == AL_TRUE)
          al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_xoff_0 + prio,
                         params->prio_q_map[i][prio]);
        else
          al_reg_write32(&adapter->ec_regs_base->fc_udma[i].q_xoff_0 + prio, 0);
      }
    }

    /* Rx path */
    /* enable generating xoff from ec fifo almost full indication in hysteresis
     * mode */
    if (params->gen_enable == AL_TRUE)
      al_reg_write32(&adapter->ec_regs_base->efc.ec_xoff,
                     0xFF << EC_EFC_EC_XOFF_MASK_2_SHIFT);
    else
      al_reg_write32(&adapter->ec_regs_base->efc.ec_xoff, 0);

    /* set hysteresis mode thresholds */
    al_reg_write32(
        &adapter->ec_regs_base->efc.rx_fifo_hyst,
        params->rx_fifo_th_low |
            (params->rx_fifo_th_high << EC_EFC_RX_FIFO_HYST_TH_HIGH_SHIFT));

    if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
        AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
      /* config the 10g_mac */
      /* set quanta value (same value for all prios) */
      reg = params->quanta | (params->quanta << 16);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl01_pause_quanta, reg);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl23_pause_quanta, reg);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl45_pause_quanta, reg);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl67_pause_quanta, reg);
      /* set quanta threshold value (same value for all prios) */
      reg = params->quanta_th | (params->quanta_th << 16);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl01_quanta_thresh, reg);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl23_quanta_thresh, reg);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl45_quanta_thresh, reg);
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cl67_quanta_thresh, reg);

      /* enable PFC in the 10g_MAC */
      reg = al_reg_read32(&adapter->mac_regs_base->mac_10g.cmd_cfg);
      reg |= 1 << 19;
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, reg);
    } else {
      /* config the 40g_mac */
      /* set quanta value (same value for all prios) */
      reg = params->quanta | (params->quanta << 16);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL01_PAUSE_QUANTA_ADDR, reg);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL23_PAUSE_QUANTA_ADDR, reg);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL45_PAUSE_QUANTA_ADDR, reg);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL67_PAUSE_QUANTA_ADDR, reg);
      /* set quanta threshold value (same value for all prios) */
      reg = params->quanta_th | (params->quanta_th << 16);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL01_QUANTA_THRESH_ADDR, reg);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL23_QUANTA_THRESH_ADDR, reg);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL45_QUANTA_THRESH_ADDR, reg);
      al_eth_40g_mac_reg_write(
          adapter, ETH_MAC_GEN_V3_MAC_40G_CL67_QUANTA_THRESH_ADDR, reg);

      /* enable PFC in the 40g_MAC */
      reg = al_reg_read32(&adapter->mac_regs_base->mac_10g.cmd_cfg);
      reg |= 1 << 19;
      al_reg_write32(&adapter->mac_regs_base->mac_10g.cmd_cfg, reg);
      reg = al_eth_40g_mac_reg_read(adapter,
                                    ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR);

      reg |= ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_PFC_MODE;

      al_eth_40g_mac_reg_write(adapter,
                               ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR, reg);
    }

    break;
  default:
    al_err("[%s]: unsupported flow control type %d\n", adapter->name,
           params->type);
    return -EINVAL;
  }
  return 0;
}

int al_eth_eee_get(struct al_hal_eth_adapter *adapter,
                   struct al_eth_eee_params *params) {
  uint32_t reg;

  al_dbg("[%s]: getting eee.\n", adapter->name);

  reg = al_reg_read32(&adapter->ec_regs_base->eee.cfg_e);
  params->enable = (reg & EC_EEE_CFG_E_ENABLE) ? AL_TRUE : AL_FALSE;

  params->tx_eee_timer = al_reg_read32(&adapter->ec_regs_base->eee.pre_cnt);
  params->min_interval = al_reg_read32(&adapter->ec_regs_base->eee.post_cnt);
  params->stop_cnt = al_reg_read32(&adapter->ec_regs_base->eee.stop_cnt);

  return 0;
}

int al_eth_eee_config(struct al_hal_eth_adapter *adapter,
                      struct al_eth_eee_params *params) {
  uint32_t reg;
  al_dbg("[%s]: config eee.\n", adapter->name);

  if (params->enable == 0) {
    al_dbg("[%s]: disable eee.\n", adapter->name);
    al_reg_write32(&adapter->ec_regs_base->eee.cfg_e, 0);
    return 0;
  }

  al_reg_write32(&adapter->ec_regs_base->eee.pre_cnt, params->tx_eee_timer);
  al_reg_write32(&adapter->ec_regs_base->eee.post_cnt, params->min_interval);
  al_reg_write32(&adapter->ec_regs_base->eee.stop_cnt, params->stop_cnt);

  reg = EC_EEE_CFG_E_MASK_EC_TMI_STOP | EC_EEE_CFG_E_MASK_MAC_EEE |
        EC_EEE_CFG_E_ENABLE | EC_EEE_CFG_E_USE_EC_TX_FIFO |
        EC_EEE_CFG_E_USE_EC_RX_FIFO;

  /*
   * Addressing RMN: 3732
   *
   * RMN description:
   * When the HW get into eee mode, it can't transmit any pause packet
   * (when flow control policy is enabled).
   * In such case, the HW has no way to handle extreme pushback from
   * the Rx_path fifos.
   *
   * Software flow:
   * Configure RX_FIFO empty as eee mode term.
   * That way, nothing will prevent pause packet transmittion in
   * case of extreme pushback from the Rx_path fifos.
   *
   */

  al_reg_write32(&adapter->ec_regs_base->eee.cfg_e, reg);

  return 0;
}

/** get link status */
int al_eth_link_status_get(struct al_hal_eth_adapter *adapter,
                           struct al_eth_link_status *status) {
  uint32_t reg;

  if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
      AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
    reg = al_reg_read32(&adapter->mac_regs_base->gen.mac_10g_stat);

    status->link_up = AL_TRUE;

    if (reg & (ETH_MAC_GEN_MAC_10G_STAT_LOC_FAULT |
               ETH_MAC_GEN_MAC_10G_STAT_REM_FAULT))
      status->link_up = AL_FALSE;

  } else if (adapter->mac_mode == AL_ETH_MAC_MODE_SGMII) {
    al_reg_write32(&adapter->mac_regs_base->sgmii.reg_addr, 1);
    /*
     * This register is latched low so need to read twice to get
     * the current link status
     */
    reg = al_reg_read32(&adapter->mac_regs_base->sgmii.reg_data);
    reg = al_reg_read32(&adapter->mac_regs_base->sgmii.reg_data);

    status->link_up = AL_FALSE;

    if (reg & AL_BIT(2))
      status->link_up = AL_TRUE;

    reg = al_reg_read32(&adapter->mac_regs_base->sgmii.link_stat);

    if ((reg & AL_BIT(3)) == 0)
      status->link_up = AL_FALSE;

  } else if (adapter->mac_mode == AL_ETH_MAC_MODE_RGMII) {
    reg = al_reg_read32(&adapter->mac_regs_base->gen.rgmii_stat);

    status->link_up = AL_FALSE;

    if (reg & AL_BIT(4))
      status->link_up = AL_TRUE;

  } else if (adapter->mac_mode == AL_ETH_MAC_MODE_XLG_LL_40G) {
    reg = al_reg_read32(&adapter->mac_regs_base->gen_v3.pcs_40g_ll_status);

    status->link_up = AL_FALSE;

    if ((reg & 0x1F) == 0x1F)
      status->link_up = AL_TRUE;

  } else {
    /* not implemented yet */
    return -EPERM;
  }

  al_dbg("[%s]: mac %s port. link_status: %s.\n", adapter->name,
         al_eth_mac_mode_str(adapter->mac_mode),
         (status->link_up == AL_TRUE) ? "LINK_UP" : "LINK_DOWN");

  return 0;
}

/** set LED mode and value */
int al_eth_led_set(struct al_hal_eth_adapter *adapter, al_bool link_is_up) {
  uint32_t reg = 0;
  uint32_t mode = ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG;

  if (link_is_up)
    mode = ETH_MAC_GEN_LED_CFG_SEL_LINK_ACTIVITY;

  AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_SEL_MASK,
                   ETH_MAC_GEN_LED_CFG_SEL_SHIFT, mode);

  AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_BLINK_TIMER_MASK,
                   ETH_MAC_GEN_LED_CFG_BLINK_TIMER_SHIFT,
                   ETH_MAC_GEN_LED_CFG_BLINK_TIMER_VAL);

  AL_REG_FIELD_SET(reg, ETH_MAC_GEN_LED_CFG_ACT_TIMER_MASK,
                   ETH_MAC_GEN_LED_CFG_ACT_TIMER_SHIFT,
                   ETH_MAC_GEN_LED_CFG_ACT_TIMER_VAL);

  al_reg_write32(&adapter->mac_regs_base->gen.led_cfg, reg);

  return 0;
}

/* get statistics */
int al_eth_mac_stats_get(struct al_hal_eth_adapter *adapter,
                         struct al_eth_mac_stats *stats) {
  al_assert(stats);

  if (AL_ETH_IS_1G_MAC(adapter->mac_mode)) {
    void __iomem *mac_1g_regs_base = &adapter->mac_regs_base->mac_1g;

    stats->ifInUcastPkts = al_reg_read32(mac_1g_regs_base + 0x90);
    stats->ifInMulticastPkts = al_reg_read32(mac_1g_regs_base + 0x94);
    stats->ifInBroadcastPkts = al_reg_read32(mac_1g_regs_base + 0x98);
    stats->etherStatsPkts = al_reg_read32(mac_1g_regs_base + 0xb4);
    stats->ifOutUcastPkts = al_reg_read32(mac_1g_regs_base + 0xa0);
    stats->ifOutMulticastPkts = al_reg_read32(mac_1g_regs_base + 0xa4);
    stats->ifOutBroadcastPkts = al_reg_read32(mac_1g_regs_base + 0xa8);
    stats->ifInErrors = al_reg_read32(mac_1g_regs_base + 0x88);
    stats->ifOutErrors = al_reg_read32(mac_1g_regs_base + 0x8c);

    stats->aFramesReceivedOK = al_reg_read32(mac_1g_regs_base + 0x6c);
    stats->aFramesTransmittedOK = al_reg_read32(mac_1g_regs_base + 0x68);

    stats->aOctetsReceivedOK = al_reg_read32(mac_1g_regs_base + 0x7c);
    stats->aOctetsTransmittedOK = al_reg_read32(mac_1g_regs_base + 0x78);

    stats->etherStatsUndersizePkts = al_reg_read32(mac_1g_regs_base + 0xB8);
    stats->etherStatsFragments = al_reg_read32(mac_1g_regs_base + 0xE0);
    stats->etherStatsJabbers = al_reg_read32(mac_1g_regs_base + 0xDC);
    stats->etherStatsOversizePkts = al_reg_read32(mac_1g_regs_base + 0xBC);

    stats->aFrameCheckSequenceErrors = al_reg_read32(mac_1g_regs_base + 0x70);
    stats->aAlignmentErrors = al_reg_read32(mac_1g_regs_base + 0x74);
    stats->etherStatsDropEvents = al_reg_read32(mac_1g_regs_base + 0xAC);

    stats->aPAUSEMACCtrlFramesTransmitted =
        al_reg_read32(mac_1g_regs_base + 0x80);
    stats->aPAUSEMACCtrlFramesReceived = al_reg_read32(mac_1g_regs_base + 0x84);
    stats->aFrameTooLongErrors = 0;  /* N/A */
    stats->aInRangeLengthErrors = 0; /* N/A */
    stats->VLANTransmittedOK = 0;    /* N/A */
    stats->VLANReceivedOK = 0;       /* N/A */
    stats->etherStatsOctets = al_reg_read32(mac_1g_regs_base + 0xB0);

    stats->etherStatsPkts64Octets = al_reg_read32(mac_1g_regs_base + 0xC0);
    stats->etherStatsPkts65to127Octets = al_reg_read32(mac_1g_regs_base + 0xC4);
    stats->etherStatsPkts128to255Octets =
        al_reg_read32(mac_1g_regs_base + 0xC8);
    stats->etherStatsPkts256to511Octets =
        al_reg_read32(mac_1g_regs_base + 0xCC);
    stats->etherStatsPkts512to1023Octets =
        al_reg_read32(mac_1g_regs_base + 0xD0);
    stats->etherStatsPkts1024to1518Octets =
        al_reg_read32(mac_1g_regs_base + 0xD4);
    stats->etherStatsPkts1519toX = al_reg_read32(mac_1g_regs_base + 0xD8);
  } else if (AL_ETH_IS_10G_MAC(adapter->mac_mode) ||
             AL_ETH_IS_25G_MAC(adapter->mac_mode)) {
    if (adapter->rev_id < AL_ETH_REV_ID_3) {
      void __iomem *mac_10g_regs_base = &adapter->mac_regs_base->mac_10g;
      uint64_t octets;

      stats->ifInUcastPkts = al_reg_read32(mac_10g_regs_base + 0xE0);
      stats->ifInMulticastPkts = al_reg_read32(mac_10g_regs_base + 0xE8);
      stats->ifInBroadcastPkts = al_reg_read32(mac_10g_regs_base + 0xF0);
      stats->etherStatsPkts = al_reg_read32(mac_10g_regs_base + 0x130);
      stats->ifOutUcastPkts = al_reg_read32(mac_10g_regs_base + 0x108);
      stats->ifOutMulticastPkts = al_reg_read32(mac_10g_regs_base + 0x110);
      stats->ifOutBroadcastPkts = al_reg_read32(mac_10g_regs_base + 0x118);
      stats->ifInErrors = al_reg_read32(mac_10g_regs_base + 0x190);
      stats->ifOutErrors = al_reg_read32(mac_10g_regs_base + 0xf8);

      stats->aFramesReceivedOK = al_reg_read32(mac_10g_regs_base + 0x88);
      stats->aFramesTransmittedOK = al_reg_read32(mac_10g_regs_base + 0x80);
      /* aOctetsReceivedOK = ifInOctets - 18 * aFramesReceivedOK - 4 *
       * VLANReceivedOK */
      octets = al_reg_read32(mac_10g_regs_base + 0xD8);
      octets |= (uint64_t)(al_reg_read32(mac_10g_regs_base + 0xDC)) << 32;
      octets -= 18 * stats->aFramesReceivedOK;
      octets -= 4 * al_reg_read32(mac_10g_regs_base + 0xC8);
      stats->aOctetsReceivedOK = octets;

      /* aOctetsTransmittedOK = ifOutOctets - 18 * aFramesTransmittedOK - 4 *
       * VLANTransmittedOK */
      octets = al_reg_read32(mac_10g_regs_base + 0xD0);
      octets |= (uint64_t)(al_reg_read32(mac_10g_regs_base + 0xD4)) << 32;
      octets -= 18 * stats->aFramesTransmittedOK;
      octets -= 4 * al_reg_read32(mac_10g_regs_base + 0xC0);
      stats->aOctetsTransmittedOK = octets;

      stats->etherStatsUndersizePkts = al_reg_read32(mac_10g_regs_base + 0x138);
      stats->etherStatsFragments = al_reg_read32(mac_10g_regs_base + 0x188);
      stats->etherStatsJabbers = al_reg_read32(mac_10g_regs_base + 0x180);
      stats->etherStatsOversizePkts = al_reg_read32(mac_10g_regs_base + 0x178);

      stats->aFrameCheckSequenceErrors =
          al_reg_read32(mac_10g_regs_base + 0x90);
      stats->aAlignmentErrors = al_reg_read32(mac_10g_regs_base + 0x98);
      stats->etherStatsDropEvents = al_reg_read32(mac_10g_regs_base + 0x120);

      stats->aPAUSEMACCtrlFramesTransmitted =
          al_reg_read32(mac_10g_regs_base + 0xA0);
      stats->aPAUSEMACCtrlFramesReceived =
          al_reg_read32(mac_10g_regs_base + 0xA8);
      stats->aFrameTooLongErrors = al_reg_read32(mac_10g_regs_base + 0xB0);
      stats->aInRangeLengthErrors = al_reg_read32(mac_10g_regs_base + 0xB8);
      stats->VLANTransmittedOK = al_reg_read32(mac_10g_regs_base + 0xC0);
      stats->VLANReceivedOK = al_reg_read32(mac_10g_regs_base + 0xC8);
      stats->etherStatsOctets = al_reg_read32(mac_10g_regs_base + 0x128);

      stats->etherStatsPkts64Octets = al_reg_read32(mac_10g_regs_base + 0x140);
      stats->etherStatsPkts65to127Octets =
          al_reg_read32(mac_10g_regs_base + 0x148);
      stats->etherStatsPkts128to255Octets =
          al_reg_read32(mac_10g_regs_base + 0x150);
      stats->etherStatsPkts256to511Octets =
          al_reg_read32(mac_10g_regs_base + 0x158);
      stats->etherStatsPkts512to1023Octets =
          al_reg_read32(mac_10g_regs_base + 0x160);
      stats->etherStatsPkts1024to1518Octets =
          al_reg_read32(mac_10g_regs_base + 0x168);
      stats->etherStatsPkts1519toX = al_reg_read32(mac_10g_regs_base + 0x170);
    } else {
      void __iomem *mac_10g_regs_base = &adapter->mac_regs_base->mac_10g;
      uint64_t octets;
      /* TODO - change to 64 bit */
      stats->ifInUcastPkts = al_reg_read32(mac_10g_regs_base + 0x140);
      stats->ifInMulticastPkts = al_reg_read32(mac_10g_regs_base + 0x148);
      stats->ifInBroadcastPkts = al_reg_read32(mac_10g_regs_base + 0x150);
      stats->etherStatsPkts = al_reg_read32(mac_10g_regs_base + 0x160);
      stats->ifOutUcastPkts = al_reg_read32(mac_10g_regs_base + 0x240);
      stats->ifOutMulticastPkts = al_reg_read32(mac_10g_regs_base + 0x248);
      stats->ifOutBroadcastPkts = al_reg_read32(mac_10g_regs_base + 0x250);
      stats->ifInErrors = al_reg_read32(mac_10g_regs_base + 0x138);
      stats->ifOutErrors = al_reg_read32(mac_10g_regs_base + 0x238);

      stats->aFramesReceivedOK =
          al_reg_read32(mac_10g_regs_base + 0x120); /*frames_ok*/
      stats->aFramesTransmittedOK =
          al_reg_read32(mac_10g_regs_base + 0x220); /*frames_ok*/
      /* aOctetsReceivedOK = ifInOctets - 18 * aFramesReceivedOK - 4 *
       * VLANReceivedOK */
      octets = al_reg_read32(mac_10g_regs_base + 0x108); /*OctetsOK*/
      octets |= (uint64_t)(al_reg_read32(mac_10g_regs_base + 0x10C)) << 32;
      octets -= 18 * stats->aFramesReceivedOK;
      octets -= 4 * al_reg_read32(mac_10g_regs_base + 0x130); /*VLANOK*/
      stats->aOctetsReceivedOK = octets;

      /* aOctetsTransmittedOK = ifOutOctets - 18 * aFramesTransmittedOK - 4 *
       * VLANTransmittedOK */
      octets = al_reg_read32(mac_10g_regs_base + 0x208); /*OctetsOK*/
      octets |= (uint64_t)(al_reg_read32(mac_10g_regs_base + 0x20c)) << 32;
      octets -= 18 * stats->aFramesTransmittedOK;
      octets -= 4 * al_reg_read32(mac_10g_regs_base + 0x230); /*VLANOK*/
      stats->aOctetsTransmittedOK = octets;

      stats->etherStatsUndersizePkts = al_reg_read32(mac_10g_regs_base + 0x168);
      stats->etherStatsFragments = al_reg_read32(mac_10g_regs_base + 0x1b8);
      stats->etherStatsJabbers = al_reg_read32(mac_10g_regs_base + 0x1b0);
      stats->etherStatsOversizePkts = al_reg_read32(mac_10g_regs_base + 0x1a8);

      stats->aFrameCheckSequenceErrors =
          al_reg_read32(mac_10g_regs_base + 0x128); /* CRCErrors */
      /* stats->aAlignmentErrors = al_reg_read32(mac_10g_regs_base + 0x98); */ /* not implemented */
      stats->etherStatsDropEvents = al_reg_read32(mac_10g_regs_base + 0x158);
    }
  } else {
    uint64_t octets;
    /* TODO - change to 64 bit */
    stats->ifInUcastPkts = al_eth_40g_mac_reg_read(adapter, 0x140);
    stats->ifInMulticastPkts = al_eth_40g_mac_reg_read(adapter, 0x148);
    stats->ifInBroadcastPkts = al_eth_40g_mac_reg_read(adapter, 0x150);
    stats->etherStatsPkts = al_eth_40g_mac_reg_read(adapter, 0x160);
    stats->ifOutUcastPkts = al_eth_40g_mac_reg_read(adapter, 0x240);
    stats->ifOutMulticastPkts = al_eth_40g_mac_reg_read(adapter, 0x248);
    stats->ifOutBroadcastPkts = al_eth_40g_mac_reg_read(adapter, 0x250);
    stats->ifInErrors = al_eth_40g_mac_reg_read(adapter, 0x138);
    stats->ifOutErrors = al_eth_40g_mac_reg_read(adapter, 0x238);
    stats->aFramesReceivedOK = al_eth_40g_mac_reg_read(adapter, 0x120);
    stats->aFramesTransmittedOK = al_eth_40g_mac_reg_read(adapter, 0x220);

    /* aOctetsReceivedOK = ifInOctets - 18 * aFramesReceivedOK - 4 *
     * VLANReceivedOK */
    octets = al_eth_40g_mac_reg_read(adapter, 0x100);
    octets |= (uint64_t)(al_eth_40g_mac_reg_read(adapter, 0x104)) << 32;
    octets -= 18 * stats->aFramesReceivedOK;
    octets -= 4 * al_eth_40g_mac_reg_read(adapter, 0x130); /*VLANOK*/
    stats->aOctetsTransmittedOK = octets;

    /* aOctetsTransmittedOK = ifOutOctets - 18 * aFramesTransmittedOK - 4 *
     * VLANTransmittedOK */
    octets = al_eth_40g_mac_reg_read(adapter, 0x200);
    octets |= (uint64_t)(al_eth_40g_mac_reg_read(adapter, 0x204)) << 32;
    octets -= 18 * stats->aFramesReceivedOK;
    octets -= 4 * al_eth_40g_mac_reg_read(adapter, 0x230); /*VLANOK*/
    stats->aOctetsReceivedOK = octets;

    stats->etherStatsUndersizePkts = al_eth_40g_mac_reg_read(adapter, 0x168);
    stats->etherStatsFragments = al_eth_40g_mac_reg_read(adapter, 0x1b8);
    stats->etherStatsJabbers = al_eth_40g_mac_reg_read(adapter, 0x1b0);
    stats->etherStatsOversizePkts = al_eth_40g_mac_reg_read(adapter, 0x1a8);
    stats->aFrameCheckSequenceErrors = al_eth_40g_mac_reg_read(adapter, 0x128);
    stats->aAlignmentErrors = al_eth_40g_mac_reg_read(adapter, 0x110);
    stats->etherStatsDropEvents = al_eth_40g_mac_reg_read(adapter, 0x158);
  }

  stats->eee_in = al_reg_read32(&adapter->mac_regs_base->stat.eee_in);
  stats->eee_out = al_reg_read32(&adapter->mac_regs_base->stat.eee_out);

  /*	stats->etherStatsPkts = 1; */
  return 0;
}

/**
 * read ec_stat_counters
 */
int al_eth_ec_stats_get(struct al_hal_eth_adapter *adapter,
                        struct al_eth_ec_stats *stats) {
  al_assert(stats);
  stats->faf_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_in_rx_pkt);
  stats->faf_in_rx_short =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_in_rx_short);
  stats->faf_in_rx_long =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_in_rx_long);
  stats->faf_out_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_out_rx_pkt);
  stats->faf_out_rx_short =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_out_rx_short);
  stats->faf_out_rx_long =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_out_rx_long);
  stats->faf_out_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.faf_out_drop);
  stats->rxf_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rxf_in_rx_pkt);
  stats->rxf_in_fifo_err =
      al_reg_read32(&adapter->ec_regs_base->stat.rxf_in_fifo_err);
  stats->lbf_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.lbf_in_rx_pkt);
  stats->lbf_in_fifo_err =
      al_reg_read32(&adapter->ec_regs_base->stat.lbf_in_fifo_err);
  stats->rxf_out_rx_1_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_rx_1_pkt);
  stats->rxf_out_rx_2_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_rx_2_pkt);
  stats->rxf_out_drop_1_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_drop_1_pkt);
  stats->rxf_out_drop_2_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rxf_out_drop_2_pkt);
  stats->rpe_1_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rpe_1_in_rx_pkt);
  stats->rpe_1_out_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rpe_1_out_rx_pkt);
  stats->rpe_2_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rpe_2_in_rx_pkt);
  stats->rpe_2_out_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rpe_2_out_rx_pkt);
  stats->rpe_3_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rpe_3_in_rx_pkt);
  stats->rpe_3_out_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rpe_3_out_rx_pkt);
  stats->tpe_in_tx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.tpe_in_tx_pkt);
  stats->tpe_out_tx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.tpe_out_tx_pkt);
  stats->tpm_tx_pkt = al_reg_read32(&adapter->ec_regs_base->stat.tpm_tx_pkt);
  stats->tfw_in_tx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.tfw_in_tx_pkt);
  stats->tfw_out_tx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.tfw_out_tx_pkt);
  stats->rfw_in_rx_pkt =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_rx_pkt);
  stats->rfw_in_vlan_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_vlan_drop);
  stats->rfw_in_parse_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_parse_drop);
  stats->rfw_in_mc = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_mc);
  stats->rfw_in_bc = al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_bc);
  stats->rfw_in_vlan_exist =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_vlan_exist);
  stats->rfw_in_vlan_nexist =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_vlan_nexist);
  stats->rfw_in_mac_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_mac_drop);
  stats->rfw_in_mac_ndet_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_mac_ndet_drop);
  stats->rfw_in_ctrl_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_ctrl_drop);
  stats->rfw_in_prot_i_drop =
      al_reg_read32(&adapter->ec_regs_base->stat.rfw_in_prot_i_drop);
  stats->eee_in = al_reg_read32(&adapter->ec_regs_base->stat.eee_in);
  return 0;
}

/* Traffic control */

int al_eth_flr_rmn(int (*pci_read_config_u32)(void *handle, int where,
                                              uint32_t *val),
                   int (*pci_write_config_u32)(void *handle, int where,
                                               uint32_t val),
                   void *handle, void __iomem *mac_base) {
  struct al_eth_mac_regs __iomem *mac_regs_base =
      (struct al_eth_mac_regs __iomem *)mac_base;
  uint32_t cfg_reg_store[6];
  uint32_t reg;
  uint32_t mux_sel;
  int i = 0;

  (*pci_read_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, &reg);

  /* reset 1G mac */
  AL_REG_MASK_SET(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
  (*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);
  al_udelay(1000);
  /* don't reset 1G mac */
  AL_REG_MASK_CLEAR(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
  /* prevent 1G mac reset on FLR */
  AL_REG_MASK_CLEAR(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC_ON_FLR);
  /* prevent adapter reset */
  (*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);

  mux_sel = al_reg_read32(&mac_regs_base->gen.mux_sel);

  /* save pci register that get reset due to flr*/
  (*pci_read_config_u32)(handle, AL_PCI_COMMAND, &cfg_reg_store[i++]);
  (*pci_read_config_u32)(handle, 0xC, &cfg_reg_store[i++]);
  (*pci_read_config_u32)(handle, 0x10, &cfg_reg_store[i++]);
  (*pci_read_config_u32)(handle, 0x18, &cfg_reg_store[i++]);
  (*pci_read_config_u32)(handle, 0x20, &cfg_reg_store[i++]);
  (*pci_read_config_u32)(handle, 0x110, &cfg_reg_store[i++]);

  /* do flr */
  (*pci_write_config_u32)(handle, AL_PCI_EXP_CAP_BASE + AL_PCI_EXP_DEVCTL,
                          AL_PCI_EXP_DEVCTL_BCR_FLR);
  al_udelay(1000);
  /* restore command */
  i = 0;
  (*pci_write_config_u32)(handle, AL_PCI_COMMAND, cfg_reg_store[i++]);
  (*pci_write_config_u32)(handle, 0xC, cfg_reg_store[i++]);
  (*pci_write_config_u32)(handle, 0x10, cfg_reg_store[i++]);
  (*pci_write_config_u32)(handle, 0x18, cfg_reg_store[i++]);
  (*pci_write_config_u32)(handle, 0x20, cfg_reg_store[i++]);
  (*pci_write_config_u32)(handle, 0x110, cfg_reg_store[i++]);

  al_reg_write32_masked(&mac_regs_base->gen.mux_sel,
                        ETH_MAC_GEN_MUX_SEL_KR_IN_MASK, mux_sel);

  /* set SGMII clock to 125MHz */
  al_reg_write32(mac_base + 0xB08, 0x03320501);

  /* reset 1G mac */
  AL_REG_MASK_SET(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
  (*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);

  al_udelay(1000);

  /* clear 1G mac reset */
  AL_REG_MASK_CLEAR(reg, AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC);
  (*pci_write_config_u32)(handle, AL_ADAPTER_GENERIC_CONTROL_0, reg);

  /* reset SGMII mac clock to default */
  al_reg_write32(mac_base + 0xB08, 0x00320501);
  al_udelay(1000);
  /* reset async fifo */
  reg = al_reg_read32(mac_base + 0x95c);
  AL_REG_MASK_SET(reg, 0xF0);
  al_reg_write32(mac_base + 0x95c, reg);
  reg = al_reg_read32(mac_base + 0x95c);
  AL_REG_MASK_CLEAR(reg, 0xF0);
  al_reg_write32(mac_base + 0x95c, reg);

  return 0;
}

/* board params register 1 */
#define AL_HAL_ETH_MEDIA_TYPE_MASK (AL_FIELD_MASK(3, 0))
#define AL_HAL_ETH_MEDIA_TYPE_SHIFT 0
#define AL_HAL_ETH_EXT_PHY_SHIFT 4
#define AL_HAL_ETH_PHY_ADDR_MASK (AL_FIELD_MASK(9, 5))
#define AL_HAL_ETH_PHY_ADDR_SHIFT 5
#define AL_HAL_ETH_SFP_EXIST_SHIFT 10
#define AL_HAL_ETH_AN_ENABLE_SHIFT 11
#define AL_HAL_ETH_KR_LT_ENABLE_SHIFT 12
#define AL_HAL_ETH_KR_FEC_ENABLE_SHIFT 13
#define AL_HAL_ETH_MDIO_FREQ_MASK (AL_FIELD_MASK(15, 14))
#define AL_HAL_ETH_MDIO_FREQ_SHIFT 14
#define AL_HAL_ETH_I2C_ADAPTER_ID_MASK (AL_FIELD_MASK(19, 16))
#define AL_HAL_ETH_I2C_ADAPTER_ID_SHIFT 16
#define AL_HAL_ETH_EXT_PHY_IF_MASK (AL_FIELD_MASK(21, 20))
#define AL_HAL_ETH_EXT_PHY_IF_SHIFT 20
#define AL_HAL_ETH_AUTO_NEG_MODE_SHIFT 22
#define AL_HAL_ETH_SERDES_GRP_MASK (AL_FIELD_MASK(26, 25))
#define AL_HAL_ETH_SERDES_GRP_SHIFT 25
#define AL_HAL_ETH_SERDES_LANE_MASK (AL_FIELD_MASK(28, 27))
#define AL_HAL_ETH_SERDES_LANE_SHIFT 27
#define AL_HAL_ETH_REF_CLK_FREQ_MASK (AL_FIELD_MASK(31, 29))
#define AL_HAL_ETH_REF_CLK_FREQ_SHIFT 29

/* board params register 2 */
#define AL_HAL_ETH_DONT_OVERRIDE_SERDES_SHIFT 0
#define AL_HAL_ETH_1000_BASE_X_SHIFT 1
#define AL_HAL_ETH_1G_AN_DISABLE_SHIFT 2
#define AL_HAL_ETH_1G_SPEED_MASK (AL_FIELD_MASK(4, 3))
#define AL_HAL_ETH_1G_SPEED_SHIFT 3
#define AL_HAL_ETH_1G_HALF_DUPLEX_SHIFT 5
#define AL_HAL_ETH_1G_FC_DISABLE_SHIFT 6
#define AL_HAL_ETH_RETIMER_EXIST_SHIFT 7
#define AL_HAL_ETH_RETIMER_BUS_ID_MASK (AL_FIELD_MASK(11, 8))
#define AL_HAL_ETH_RETIMER_BUS_ID_SHIFT 8
#define AL_HAL_ETH_RETIMER_I2C_ADDR_MASK (AL_FIELD_MASK(18, 12))
#define AL_HAL_ETH_RETIMER_I2C_ADDR_SHIFT 12
#define AL_HAL_ETH_RETIMER_CHANNEL_SHIFT 19
#define AL_HAL_ETH_DAC_LENGTH_MASK (AL_FIELD_MASK(23, 20))
#define AL_HAL_ETH_DAC_LENGTH_SHIFT 20
#define AL_HAL_ETH_DAC_SHIFT 24

int al_eth_board_params_set(void *__iomem mac_base,
                            struct al_eth_board_params *params) {
  uint32_t reg = 0;

  /* ************* Setting Board params register 1 **************** */
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_MEDIA_TYPE_MASK, AL_HAL_ETH_MEDIA_TYPE_SHIFT,
                   params->media_type);
  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_EXT_PHY_SHIFT,
                     params->phy_exist == AL_TRUE);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_PHY_ADDR_MASK, AL_HAL_ETH_PHY_ADDR_SHIFT,
                   params->phy_mdio_addr);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_SFP_EXIST_SHIFT,
                     params->sfp_plus_module_exist == AL_TRUE);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_AN_ENABLE_SHIFT,
                     params->autoneg_enable == AL_TRUE);
  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_KR_LT_ENABLE_SHIFT,
                     params->kr_lt_enable == AL_TRUE);
  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_KR_FEC_ENABLE_SHIFT,
                     params->kr_fec_enable == AL_TRUE);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_MDIO_FREQ_MASK, AL_HAL_ETH_MDIO_FREQ_SHIFT,
                   params->mdio_freq);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_I2C_ADAPTER_ID_MASK,
                   AL_HAL_ETH_I2C_ADAPTER_ID_SHIFT, params->i2c_adapter_id);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_EXT_PHY_IF_MASK, AL_HAL_ETH_EXT_PHY_IF_SHIFT,
                   params->phy_if);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_AUTO_NEG_MODE_SHIFT,
                     params->an_mode == AL_ETH_BOARD_AUTONEG_IN_BAND);

  AL_REG_FIELD_SET(reg, AL_HAL_ETH_SERDES_GRP_MASK, AL_HAL_ETH_SERDES_GRP_SHIFT,
                   params->serdes_grp);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_SERDES_LANE_MASK,
                   AL_HAL_ETH_SERDES_LANE_SHIFT, params->serdes_lane);

  AL_REG_FIELD_SET(reg, AL_HAL_ETH_REF_CLK_FREQ_MASK,
                   AL_HAL_ETH_REF_CLK_FREQ_SHIFT, params->ref_clk_freq);

  al_assert(reg != 0);

  al_reg_write32(mac_base + 0x4, reg);

  /* ************* Setting Board params register 2 **************** */
  reg = 0;
  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_DONT_OVERRIDE_SERDES_SHIFT,
                     params->dont_override_serdes == AL_TRUE);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1000_BASE_X_SHIFT,
                     params->force_1000_base_x == AL_TRUE);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1G_AN_DISABLE_SHIFT,
                     params->an_disable == AL_TRUE);

  AL_REG_FIELD_SET(reg, AL_HAL_ETH_1G_SPEED_MASK, AL_HAL_ETH_1G_SPEED_SHIFT,
                   params->speed);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1G_HALF_DUPLEX_SHIFT,
                     params->half_duplex == AL_TRUE);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_1G_FC_DISABLE_SHIFT,
                     params->fc_disable == AL_TRUE);

  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_RETIMER_EXIST_SHIFT,
                     params->retimer_exist == AL_TRUE);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_BUS_ID_MASK,
                   AL_HAL_ETH_RETIMER_BUS_ID_SHIFT, params->retimer_bus_id);
  AL_REG_FIELD_SET(reg, AL_HAL_ETH_RETIMER_I2C_ADDR_MASK,
                   AL_HAL_ETH_RETIMER_I2C_ADDR_SHIFT, params->retimer_i2c_addr);
  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_RETIMER_CHANNEL_SHIFT,
                     params->retimer_channel);

  AL_REG_FIELD_SET(reg, AL_HAL_ETH_DAC_LENGTH_MASK, AL_HAL_ETH_DAC_LENGTH_SHIFT,
                   params->dac_len);
  AL_REG_BIT_VAL_SET(reg, AL_HAL_ETH_DAC_SHIFT, params->dac);

  al_reg_write32(mac_base + 0x404, reg);
  return 0;
}

int al_eth_board_params_get(void *__iomem mac_base,
                            struct al_eth_board_params *params) {
  uint32_t reg = al_reg_read32(mac_base + 0x4);

  /* check if the register was initialized, 0 is not a valid value */
  if (reg == 0)
    return -ENOENT;

  /* ************* Getting Board params register 1 **************** */
  params->media_type = AL_REG_FIELD_GET(reg, AL_HAL_ETH_MEDIA_TYPE_MASK,
                                        AL_HAL_ETH_MEDIA_TYPE_SHIFT);
  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_EXT_PHY_SHIFT))
    params->phy_exist = AL_TRUE;
  else
    params->phy_exist = AL_FALSE;

  params->phy_mdio_addr = AL_REG_FIELD_GET(reg, AL_HAL_ETH_PHY_ADDR_MASK,
                                           AL_HAL_ETH_PHY_ADDR_SHIFT);

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_SFP_EXIST_SHIFT))
    params->sfp_plus_module_exist = AL_TRUE;
  else
    params->sfp_plus_module_exist = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_AN_ENABLE_SHIFT))
    params->autoneg_enable = AL_TRUE;
  else
    params->autoneg_enable = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_KR_LT_ENABLE_SHIFT))
    params->kr_lt_enable = AL_TRUE;
  else
    params->kr_lt_enable = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_KR_FEC_ENABLE_SHIFT))
    params->kr_fec_enable = AL_TRUE;
  else
    params->kr_fec_enable = AL_FALSE;

  params->mdio_freq = AL_REG_FIELD_GET(reg, AL_HAL_ETH_MDIO_FREQ_MASK,
                                       AL_HAL_ETH_MDIO_FREQ_SHIFT);

  params->i2c_adapter_id = AL_REG_FIELD_GET(reg, AL_HAL_ETH_I2C_ADAPTER_ID_MASK,
                                            AL_HAL_ETH_I2C_ADAPTER_ID_SHIFT);

  params->phy_if = AL_REG_FIELD_GET(reg, AL_HAL_ETH_EXT_PHY_IF_MASK,
                                    AL_HAL_ETH_EXT_PHY_IF_SHIFT);

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_AUTO_NEG_MODE_SHIFT))
    params->an_mode = AL_TRUE;
  else
    params->an_mode = AL_FALSE;

  params->serdes_grp = AL_REG_FIELD_GET(reg, AL_HAL_ETH_SERDES_GRP_MASK,
                                        AL_HAL_ETH_SERDES_GRP_SHIFT);

  params->serdes_lane = AL_REG_FIELD_GET(reg, AL_HAL_ETH_SERDES_LANE_MASK,
                                         AL_HAL_ETH_SERDES_LANE_SHIFT);

  params->ref_clk_freq = AL_REG_FIELD_GET(reg, AL_HAL_ETH_REF_CLK_FREQ_MASK,
                                          AL_HAL_ETH_REF_CLK_FREQ_SHIFT);

  /* ************* Getting Board params register 2 **************** */
  reg = al_reg_read32(mac_base + 0x404);
  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_DONT_OVERRIDE_SERDES_SHIFT))
    params->dont_override_serdes = AL_TRUE;
  else
    params->dont_override_serdes = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1000_BASE_X_SHIFT))
    params->force_1000_base_x = AL_TRUE;
  else
    params->force_1000_base_x = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1G_AN_DISABLE_SHIFT))
    params->an_disable = AL_TRUE;
  else
    params->an_disable = AL_FALSE;

  params->speed = AL_REG_FIELD_GET(reg, AL_HAL_ETH_1G_SPEED_MASK,
                                   AL_HAL_ETH_1G_SPEED_SHIFT);

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1G_HALF_DUPLEX_SHIFT))
    params->half_duplex = AL_TRUE;
  else
    params->half_duplex = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_1G_FC_DISABLE_SHIFT))
    params->fc_disable = AL_TRUE;
  else
    params->fc_disable = AL_FALSE;

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_RETIMER_EXIST_SHIFT))
    params->retimer_exist = AL_TRUE;
  else
    params->retimer_exist = AL_FALSE;

  params->retimer_bus_id = AL_REG_FIELD_GET(reg, AL_HAL_ETH_RETIMER_BUS_ID_MASK,
                                            AL_HAL_ETH_RETIMER_BUS_ID_SHIFT);
  params->retimer_i2c_addr = AL_REG_FIELD_GET(
      reg, AL_HAL_ETH_RETIMER_I2C_ADDR_MASK, AL_HAL_ETH_RETIMER_I2C_ADDR_SHIFT);

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_RETIMER_CHANNEL_SHIFT))
    params->retimer_channel = AL_ETH_RETIMER_CHANNEL_B;
  else
    params->retimer_channel = AL_ETH_RETIMER_CHANNEL_A;

  params->dac_len = AL_REG_FIELD_GET(reg, AL_HAL_ETH_DAC_LENGTH_MASK,
                                     AL_HAL_ETH_DAC_LENGTH_SHIFT);

  if (AL_REG_BIT_GET(reg, AL_HAL_ETH_DAC_SHIFT))
    params->dac = AL_TRUE;
  else
    params->dac = AL_FALSE;

  return 0;
}

/* Wake-On-Lan (WoL) */
static inline void al_eth_byte_arr_to_reg(uint32_t *reg, uint8_t *arr,
                                          unsigned int num_bytes) {
  uint32_t mask = 0xff;
  unsigned int i;

  al_assert(num_bytes <= 4);

  *reg = 0;

  for (i = 0; i < num_bytes; i++) {
    AL_REG_FIELD_SET(*reg, mask, (sizeof(uint8_t) * i), arr[i]);
    mask = mask << sizeof(uint8_t);
  }
}

int al_eth_wol_enable(struct al_hal_eth_adapter *adapter,
                      struct al_eth_wol_params *wol) {
  uint32_t reg = 0;

  if (wol->int_mask & AL_ETH_WOL_INT_MAGIC_PSWD) {
    al_assert(wol->pswd != NULL);

    al_eth_byte_arr_to_reg(&reg, &wol->pswd[0], 4);
    al_reg_write32(&adapter->ec_regs_base->wol.magic_pswd_l, reg);

    al_eth_byte_arr_to_reg(&reg, &wol->pswd[4], 2);
    al_reg_write32(&adapter->ec_regs_base->wol.magic_pswd_h, reg);
  }

  if (wol->int_mask & AL_ETH_WOL_INT_IPV4) {
    al_assert(wol->ipv4 != NULL);

    al_eth_byte_arr_to_reg(&reg, &wol->ipv4[0], 4);
    al_reg_write32(&adapter->ec_regs_base->wol.ipv4_dip, reg);
  }

  if (wol->int_mask & AL_ETH_WOL_INT_IPV6) {
    al_assert(wol->ipv6 != NULL);

    al_eth_byte_arr_to_reg(&reg, &wol->ipv6[0], 4);
    al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word0, reg);

    al_eth_byte_arr_to_reg(&reg, &wol->ipv6[4], 4);
    al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word1, reg);

    al_eth_byte_arr_to_reg(&reg, &wol->ipv6[8], 4);
    al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word2, reg);

    al_eth_byte_arr_to_reg(&reg, &wol->ipv6[12], 4);
    al_reg_write32(&adapter->ec_regs_base->wol.ipv6_dip_word3, reg);
  }

  if (wol->int_mask &
      (AL_ETH_WOL_INT_ETHERTYPE_BC | AL_ETH_WOL_INT_ETHERTYPE_DA)) {

    reg = ((uint32_t)wol->ethr_type2 << 16);
    reg |= wol->ethr_type1;

    al_reg_write32(&adapter->ec_regs_base->wol.ethertype, reg);
  }

  /* make sure we dont forwarding packets without interrupt */
  al_assert((wol->forward_mask | wol->int_mask) == wol->int_mask);

  reg = ((uint32_t)wol->forward_mask << 16);
  reg |= wol->int_mask;
  al_reg_write32(&adapter->ec_regs_base->wol.wol_en, reg);

  return 0;
}

int al_eth_wol_disable(struct al_hal_eth_adapter *adapter) {
  al_reg_write32(&adapter->ec_regs_base->wol.wol_en, 0);

  return 0;
}

int al_eth_tx_generic_crc_entry_set(
    struct al_hal_eth_adapter *adapter, uint32_t idx,
    struct al_eth_tx_gpd_cam_entry *tx_gpd_entry,
    struct al_eth_tx_gcp_table_entry *tx_gcp_entry,
    struct al_eth_tx_crc_chksum_replace_cmd_for_protocol_num_entry
        *tx_replace_entry) {
  uint64_t gpd_data;
  uint64_t gpd_mask;
  uint32_t gcp_table_gen;
  uint32_t tx_alu_opcode;
  uint32_t tx_alu_opsel;
  uint32_t replace_table_address;
  uint32_t tx_replace_cmd;

  gpd_data =
      ((uint64_t)tx_gpd_entry->l3_proto_idx & AL_ETH_TX_GPD_L3_PROTO_MASK)
      << AL_ETH_TX_GPD_L3_PROTO_SHIFT;
  gpd_data |=
      ((uint64_t)tx_gpd_entry->l4_proto_idx & AL_ETH_TX_GPD_L4_PROTO_MASK)
      << AL_ETH_TX_GPD_L4_PROTO_SHIFT;
  gpd_data |=
      ((uint64_t)tx_gpd_entry->tunnel_control & AL_ETH_TX_GPD_TUNNEL_CTRL_MASK)
      << AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT;
  gpd_data |= ((uint64_t)tx_gpd_entry->source_vlan_count &
               AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK)
              << AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT;
  gpd_mask =
      ((uint64_t)tx_gpd_entry->l3_proto_idx_mask & AL_ETH_TX_GPD_L3_PROTO_MASK)
      << AL_ETH_TX_GPD_L3_PROTO_SHIFT;
  gpd_mask |=
      ((uint64_t)tx_gpd_entry->l4_proto_idx_mask & AL_ETH_TX_GPD_L4_PROTO_MASK)
      << AL_ETH_TX_GPD_L4_PROTO_SHIFT;
  gpd_mask |= ((uint64_t)tx_gpd_entry->tunnel_control_mask &
               AL_ETH_TX_GPD_TUNNEL_CTRL_MASK)
              << AL_ETH_TX_GPD_TUNNEL_CTRL_SHIFT;
  gpd_mask |= ((uint64_t)tx_gpd_entry->source_vlan_count_mask &
               AL_ETH_TX_GPD_SRC_VLAN_CNT_MASK)
              << AL_ETH_TX_GPD_SRC_VLAN_CNT_SHIFT;

  /* Tx Generic protocol detect Cam compare table */
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_ctrl,
                 (uint32_t)((tx_gpd_entry->tx_gpd_cam_ctrl)
                            << AL_ETH_TX_GPD_CAM_CTRL_VALID_SHIFT));
  al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_ctrl: %#x",
         idx, tx_gpd_entry->tx_gpd_cam_ctrl);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_mask_2,
                 (uint32_t)(gpd_mask >> AL_ETH_TX_GPD_CAM_MASK_2_SHIFT));
  al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_mask_2: %#x",
         idx, (uint32_t)(gpd_mask >> AL_ETH_TX_GPD_CAM_MASK_2_SHIFT));
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_mask_1,
                 (uint32_t)(gpd_mask));
  al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_mask_1: %#x",
         idx, (uint32_t)(gpd_mask));
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_data_2,
                 (uint32_t)(gpd_data >> AL_ETH_TX_GPD_CAM_DATA_2_SHIFT));
  al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_data_2: %#x",
         idx, (uint32_t)(gpd_data >> AL_ETH_TX_GPD_CAM_DATA_2_SHIFT));
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gpd_cam_data_1,
                 (uint32_t)(gpd_data));
  al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], tx_gpd_cam_data_1: %#x",
         idx, (uint32_t)(gpd_data));

  gcp_table_gen = (tx_gcp_entry->poly_sel & AL_ETH_TX_GCP_POLY_SEL_MASK)
                  << AL_ETH_TX_GCP_POLY_SEL_SHIFT;
  gcp_table_gen |=
      (tx_gcp_entry->crc32_bit_comp & AL_ETH_TX_GCP_CRC32_BIT_COMP_MASK)
      << AL_ETH_TX_GCP_CRC32_BIT_COMP_SHIFT;
  gcp_table_gen |=
      (tx_gcp_entry->crc32_bit_swap & AL_ETH_TX_GCP_CRC32_BIT_SWAP_MASK)
      << AL_ETH_TX_GCP_CRC32_BIT_SWAP_SHIFT;
  gcp_table_gen |=
      (tx_gcp_entry->crc32_byte_swap & AL_ETH_TX_GCP_CRC32_BYTE_SWAP_MASK)
      << AL_ETH_TX_GCP_CRC32_BYTE_SWAP_SHIFT;
  gcp_table_gen |=
      (tx_gcp_entry->data_bit_swap & AL_ETH_TX_GCP_DATA_BIT_SWAP_MASK)
      << AL_ETH_TX_GCP_DATA_BIT_SWAP_SHIFT;
  gcp_table_gen |=
      (tx_gcp_entry->data_byte_swap & AL_ETH_TX_GCP_DATA_BYTE_SWAP_MASK)
      << AL_ETH_TX_GCP_DATA_BYTE_SWAP_SHIFT;
  gcp_table_gen |= (tx_gcp_entry->trail_size & AL_ETH_TX_GCP_TRAIL_SIZE_MASK)
                   << AL_ETH_TX_GCP_TRAIL_SIZE_SHIFT;
  gcp_table_gen |= (tx_gcp_entry->head_size & AL_ETH_TX_GCP_HEAD_SIZE_MASK)
                   << AL_ETH_TX_GCP_HEAD_SIZE_SHIFT;
  gcp_table_gen |= (tx_gcp_entry->head_calc & AL_ETH_TX_GCP_HEAD_CALC_MASK)
                   << AL_ETH_TX_GCP_HEAD_CALC_SHIFT;
  gcp_table_gen |=
      (tx_gcp_entry->mask_polarity & AL_ETH_TX_GCP_MASK_POLARITY_MASK)
      << AL_ETH_TX_GCP_MASK_POLARITY_SHIFT;
  al_dbg("al_eth_tx_generic_crc_entry_set, line [%d], gcp_table_gen: %#x", idx,
         gcp_table_gen);

  tx_alu_opcode = (tx_gcp_entry->tx_alu_opcode_1 & AL_ETH_TX_GCP_OPCODE_1_MASK)
                  << AL_ETH_TX_GCP_OPCODE_1_SHIFT;
  tx_alu_opcode |= (tx_gcp_entry->tx_alu_opcode_2 & AL_ETH_TX_GCP_OPCODE_2_MASK)
                   << AL_ETH_TX_GCP_OPCODE_2_SHIFT;
  tx_alu_opcode |= (tx_gcp_entry->tx_alu_opcode_3 & AL_ETH_TX_GCP_OPCODE_3_MASK)
                   << AL_ETH_TX_GCP_OPCODE_3_SHIFT;
  tx_alu_opsel = (tx_gcp_entry->tx_alu_opsel_1 & AL_ETH_TX_GCP_OPSEL_1_MASK)
                 << AL_ETH_TX_GCP_OPSEL_1_SHIFT;
  tx_alu_opsel |= (tx_gcp_entry->tx_alu_opsel_2 & AL_ETH_TX_GCP_OPSEL_2_MASK)
                  << AL_ETH_TX_GCP_OPSEL_2_SHIFT;
  tx_alu_opsel |= (tx_gcp_entry->tx_alu_opsel_3 & AL_ETH_TX_GCP_OPSEL_3_MASK)
                  << AL_ETH_TX_GCP_OPSEL_3_SHIFT;
  tx_alu_opsel |= (tx_gcp_entry->tx_alu_opsel_4 & AL_ETH_TX_GCP_OPSEL_4_MASK)
                  << AL_ETH_TX_GCP_OPSEL_4_SHIFT;

  /*  Tx Generic crc prameters table general */
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_addr, idx);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_gen,
                 gcp_table_gen);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_1,
                 tx_gcp_entry->gcp_mask[0]);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_2,
                 tx_gcp_entry->gcp_mask[1]);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_3,
                 tx_gcp_entry->gcp_mask[2]);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_4,
                 tx_gcp_entry->gcp_mask[3]);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_5,
                 tx_gcp_entry->gcp_mask[4]);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_mask_6,
                 tx_gcp_entry->gcp_mask[5]);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_crc_init,
                 tx_gcp_entry->crc_init);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_res,
                 tx_gcp_entry->gcp_table_res);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_opcode,
                 tx_alu_opcode);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_opsel,
                 tx_alu_opsel);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.tx_gcp_table_alu_val,
                 tx_gcp_entry->alu_val);

  /*  Tx crc_chksum_replace_cmd */
  replace_table_address = L4_CHECKSUM_DIS_AND_L3_CHECKSUM_DIS | idx;
  tx_replace_cmd = (uint32_t)(tx_replace_entry->l3_csum_en_00) << 0;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_00) << 1;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_00) << 2;
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
                 replace_table_address);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
                 tx_replace_cmd);
  replace_table_address = L4_CHECKSUM_DIS_AND_L3_CHECKSUM_EN | idx;
  tx_replace_cmd = (uint32_t)(tx_replace_entry->l3_csum_en_01) << 0;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_01) << 1;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_01) << 2;
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
                 replace_table_address);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
                 tx_replace_cmd);
  replace_table_address = L4_CHECKSUM_EN_AND_L3_CHECKSUM_DIS | idx;
  tx_replace_cmd = (uint32_t)(tx_replace_entry->l3_csum_en_10) << 0;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_10) << 1;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_10) << 2;
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
                 replace_table_address);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
                 tx_replace_cmd);
  replace_table_address = L4_CHECKSUM_EN_AND_L3_CHECKSUM_EN | idx;
  tx_replace_cmd = (uint32_t)(tx_replace_entry->l3_csum_en_11) << 0;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->l4_csum_en_11) << 1;
  tx_replace_cmd |= (uint32_t)(tx_replace_entry->crc_en_11) << 2;
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table_addr,
                 replace_table_address);
  al_reg_write32(&adapter->ec_regs_base->tfw_v3.crc_csum_replace_table,
                 tx_replace_cmd);

  return 0;
}

/** @} end of Ethernet group */
