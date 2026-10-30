/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 *  @{
 * @file   al_hal_eth_mac_regs.h
 *
 * @brief Ethernet MAC registers
 *
 */

#ifndef __AL_HAL_ETH_MAC_REG_H
#define __AL_HAL_ETH_MAC_REG_H

/*
* Unit Registers
*/

struct al_eth_mac_1g {
	/* [0x0] */
	uint32_t rev;
	uint32_t scratch;
	uint32_t cmd_cfg;
	uint32_t mac_0;
	/* [0x10] */
	uint32_t mac_1;
	uint32_t frm_len;
	uint32_t pause_quant;
	uint32_t rx_section_empty;
	/* [0x20] */
	uint32_t rx_section_full;
	uint32_t tx_section_empty;
	uint32_t tx_section_full;
	uint32_t rx_almost_empty;
	/* [0x30] */
	uint32_t rx_almost_full;
	uint32_t tx_almost_empty;
	uint32_t tx_almost_full;
	uint32_t mdio_addr0;
	/* [0x40] */
	uint32_t mdio_addr1;
	uint32_t Reserved[5];
	/* [0x58] */
	uint32_t reg_stat;
	uint32_t tx_ipg_len;
	/* [0x60] */
	uint32_t Reserved1[104];
	/* [0x200] */
	uint32_t phy_regs_base;
	uint32_t Reserved2[127];
};

struct al_eth_mac_10g {
	/* [0x0] */
	uint32_t rev;
	uint32_t scratch;
	uint32_t cmd_cfg;
	uint32_t mac_0;
	/* [0x10] */
	uint32_t mac_1;
	uint32_t frm_len;
	uint32_t Reserved;
	uint32_t rx_fifo_sections;
	/* [0x20] */
	uint32_t tx_fifo_sections;
	uint32_t rx_fifo_almost_f_e;
	uint32_t tx_fifo_almost_f_e;
	uint32_t hashtable_load;
	/* [0x30] */
	uint32_t mdio_cfg_status;
	uint16_t mdio_cmd;
	uint16_t reserved1;
	uint16_t mdio_data;
	uint16_t reserved2;
	uint16_t mdio_regaddr;
	uint16_t reserved3;
	/* [0x40] */
	uint32_t status;
	uint32_t tx_ipg_len;
	uint32_t Reserved1[3];
	/* [0x54] */
	uint32_t cl01_pause_quanta;
	uint32_t cl23_pause_quanta;
	uint32_t cl45_pause_quanta;
	/* [0x60] */
	uint32_t cl67_pause_quanta;
	uint32_t cl01_quanta_thresh;
	uint32_t cl23_quanta_thresh;
	uint32_t cl45_quanta_thresh;
	/* [0x70] */
	uint32_t cl67_quanta_thresh;
	uint32_t rx_pause_status;
	uint32_t Reserved2;
	uint32_t ts_timestamp;
	/* [0x80] */

	uint32_t Reserved3[160];

	/* [0x300] */
	uint32_t control;
	uint32_t status_reg;
	uint32_t phy_id[2];
	/* [0x310] */
	uint32_t dev_ability;
	uint32_t partner_ability;
	uint32_t an_expansion;
	uint32_t device_np;
	/* [0x320] */
	uint32_t partner_np;
	uint32_t Reserved4[9];

	/* [0x348] */
	uint32_t link_timer_lo;
	uint32_t link_timer_hi;
	/* [0x350] */
	uint32_t if_mode;

	uint32_t Reserved5[43];
};

struct al_eth_mac_gen {
	/* [0x0]  Ethernet Controller Version */
	uint32_t version;
	uint32_t rsrvd_0[2];
	/* [0xc] MAC selection configuration */
	uint32_t cfg;
	/* [0x10] 10/100/1000 MAC external configuration */
	uint32_t mac_1g_cfg;
	/* [0x14] 10/100/1000 MAC status */
	uint32_t mac_1g_stat;
	/* [0x18] RGMII external configuration */
	uint32_t rgmii_cfg;
	/* [0x1c] RGMII status */
	uint32_t rgmii_stat;
	/* [0x20] 1/2.5/10G MAC external configuration */
	uint32_t mac_10g_cfg;
	/* [0x24] 1/2.5/10G MAC status */
	uint32_t mac_10g_stat;
	/* [0x28] XAUI PCS configuration */
	uint32_t xaui_cfg;
	/* [0x2c] XAUI PCS status */
	uint32_t xaui_stat;
	/* [0x30] RXAUI PCS configuration */
	uint32_t rxaui_cfg;
	/* [0x34] RXAUI PCS status */
	uint32_t rxaui_stat;
	/* [0x38] Signal detect configuration */
	uint32_t sd_cfg;
	/* [0x3c] MDIO control register for MDIO interface 1 */
	uint32_t mdio_ctrl_1;
	/* [0x40] MDIO information register for MDIO interface 1 */
	uint32_t mdio_1;
	/* [0x44] MDIO control register for MDIO interface 2 */
	uint32_t mdio_ctrl_2;
	/* [0x48] MDIO information register for MDIO interface 2 */
	uint32_t mdio_2;
	/* [0x4c] XGMII 32 to 64 data FIFO control */
	uint32_t xgmii_dfifo_32_64;
	/* [0x50] Reserved 1 out */
	uint32_t mac_res_1_out;
	/* [0x54] XGMII 64 to 32 data FIFO control */
	uint32_t xgmii_dfifo_64_32;
	/* [0x58] Reserved 1 in */
	uint32_t mac_res_1_in;
	/* [0x5c] SerDes TX FIFO control */
	uint32_t sd_fifo_ctrl;
	/* [0x60] SerDes TX FIFO status */
	uint32_t sd_fifo_stat;
	/* [0x64] SerDes in/out selection */
	uint32_t mux_sel;
	/* [0x68] Clock configuration */
	uint32_t clk_cfg;
	uint32_t rsrvd_1;
	/* [0x70] LOS and SD selection */
	uint32_t los_sel;
	/* [0x74] RGMII selection configuration */
	uint32_t rgmii_sel;
	/* [0x78] Ethernet LED configuration */
	uint32_t led_cfg;
	uint32_t rsrvd[33];
};
struct al_eth_mac_kr {
	/* [0x0] PCS register file address */
	uint32_t pcs_addr;
	/* [0x4] PCS register file data */
	uint32_t pcs_data;
	/* [0x8] AN register file address */
	uint32_t an_addr;
	/* [0xc] AN register file data */
	uint32_t an_data;
	/* [0x10] PMA register file address */
	uint32_t pma_addr;
	/* [0x14] PMA register file data */
	uint32_t pma_data;
	/* [0x18] MTIP register file address */
	uint32_t mtip_addr;
	/* [0x1c] MTIP register file data */
	uint32_t mtip_data;
	/* [0x20] KR PCS config  */
	uint32_t pcs_cfg;
	/* [0x24] KR PCS status  */
	uint32_t pcs_stat;
	uint32_t rsrvd[54];
};
struct al_eth_mac_sgmii {
	/* [0x0] PCS register file address */
	uint32_t reg_addr;
	/* [0x4] PCS register file data */
	uint32_t reg_data;
	/* [0x8] PCS clock divider configuration */
	uint32_t clk_div;
	/* [0xc] PCS Status */
	uint32_t link_stat;
	uint32_t rsrvd[60];
};
struct al_eth_mac_stat {
	/* [0x0] Receive rate matching error */
	uint32_t match_fault;
	/* [0x4] EEE, number of times the MAC went into low power mo ... */
	uint32_t eee_in;
	/* [0x8] EEE, number of times the MAC went out of low power  ... */
	uint32_t eee_out;
	/* [0xc] 40G PCS, FEC corrected error indication */
	uint32_t v3_pcs_40g_ll_cerr_0;
	/* [0x10] 40G PCS, FEC corrected error indication */
	uint32_t v3_pcs_40g_ll_cerr_1;
	/* [0x14] 40G PCS, FEC corrected error indication */
	uint32_t v3_pcs_40g_ll_cerr_2;
	/* [0x18] 40G PCS, FEC corrected error indication */
	uint32_t v3_pcs_40g_ll_cerr_3;
	/* [0x1c] 40G PCS, FEC uncorrectable error indication */
	uint32_t v3_pcs_40g_ll_ncerr_0;
	/* [0x20] 40G PCS, FEC uncorrectable error indication */
	uint32_t v3_pcs_40g_ll_ncerr_1;
	/* [0x24] 40G PCS, FEC uncorrectable error indication */
	uint32_t v3_pcs_40g_ll_ncerr_2;
	/* [0x28] 40G PCS, FEC uncorrectable error indication */
	uint32_t v3_pcs_40g_ll_ncerr_3;
	/* [0x2c] 10G_LL PCS, FEC corrected error indication */
	uint32_t v3_pcs_10g_ll_cerr;
	/* [0x30] 10G_LL PCS, FEC uncorrectable error indication */
	uint32_t v3_pcs_10g_ll_ncerr;
	uint32_t rsrvd[51];
};
struct al_eth_mac_stat_lane {
	/* [0x0] Character error */
	uint32_t char_err;
	/* [0x4] Disparity error */
	uint32_t disp_err;
	/* [0x8] Comma detection */
	uint32_t pat;
	uint32_t rsrvd[13];
};
struct al_eth_mac_gen_v3 {
	/* [0x0] ASYNC FIFOs control */
	uint32_t afifo_ctrl;
	/* [0x4] TX ASYNC FIFO configuration */
	uint32_t tx_afifo_cfg_1;
	/* [0x8] TX ASYNC FIFO configuration */
	uint32_t tx_afifo_cfg_2;
	/* [0xc] TX ASYNC FIFO configuration */
	uint32_t tx_afifo_cfg_3;
	/* [0x10] TX ASYNC FIFO configuration */
	uint32_t tx_afifo_cfg_4;
	/* [0x14] TX ASYNC FIFO configuration */
	uint32_t tx_afifo_cfg_5;
	/* [0x18] RX ASYNC FIFO configuration */
	uint32_t rx_afifo_cfg_1;
	/* [0x1c] RX ASYNC FIFO configuration */
	uint32_t rx_afifo_cfg_2;
	/* [0x20] RX ASYNC FIFO configuration */
	uint32_t rx_afifo_cfg_3;
	/* [0x24] RX ASYNC FIFO configuration */
	uint32_t rx_afifo_cfg_4;
	/* [0x28] RX ASYNC FIFO configuration */
	uint32_t rx_afifo_cfg_5;
	/* [0x2c] MAC selection configuration */
	uint32_t mac_sel;
	/* [0x30] 10G LL MAC configuration */
	uint32_t mac_10g_ll_cfg;
	/* [0x34] 10G LL MAC control */
	uint32_t mac_10g_ll_ctrl;
	/* [0x38] 10G LL PCS configuration */
	uint32_t pcs_10g_ll_cfg;
	/* [0x3c] 10G LL PCS status */
	uint32_t pcs_10g_ll_status;
	/* [0x40] 40G LL PCS configuration */
	uint32_t pcs_40g_ll_cfg;
	/* [0x44] 40G LL PCS status */
	uint32_t pcs_40g_ll_status;
	/* [0x48] PCS 40G  register file address */
	uint32_t pcs_40g_ll_addr;
	/* [0x4c] PCS 40G register file data */
	uint32_t pcs_40g_ll_data;
	/* [0x50] 40G LL MAC configuration */
	uint32_t mac_40g_ll_cfg;
	/* [0x54] 40G LL MAC status */
	uint32_t mac_40g_ll_status;
	/* [0x58] Preamble configuration (high [55:32]) */
	uint32_t preamble_cfg_high;
	/* [0x5c] Preamble configuration (low [31:0]) */
	uint32_t preamble_cfg_low;
	/* [0x60] MAC 40G register file address */
	uint32_t mac_40g_ll_addr;
	/* [0x64] MAC 40G register file data */
	uint32_t mac_40g_ll_data;
	/* [0x68] 40G LL MAC control */
	uint32_t mac_40g_ll_ctrl;
	/* [0x6c] PCS 40G  register file address */
	uint32_t pcs_40g_fec_91_ll_addr;
	/* [0x70] PCS 40G register file data */
	uint32_t pcs_40g_fec_91_ll_data;
	/* [0x74] 40G LL PCS EEE configuration */
	uint32_t pcs_40g_ll_eee_cfg;
	/* [0x78] 40G LL PCS EEE status */
	uint32_t pcs_40g_ll_eee_status;
	/* [0x7c] SERDES 32-bit interface shift configuration (when s ... */
	uint32_t serdes_32_tx_shift;
	/* [0x80] SERDES 32-bit interface shift configuration (when s ... */
	uint32_t serdes_32_rx_shift;
	/* [0x84] SERDES 32-bit interface bit selection */
	uint32_t serdes_32_tx_sel;
	/* [0x88] SERDES 32-bit interface bit selection */
	uint32_t serdes_32_rx_sel;
	/* [0x8c] AN/LT wrapper  control */
	uint32_t an_lt_ctrl;
	/* [0x90] AN/LT wrapper  register file address */
	uint32_t an_lt_0_addr;
	/* [0x94] AN/LT wrapper register file data */
	uint32_t an_lt_0_data;
	/* [0x98] AN/LT wrapper  register file address */
	uint32_t an_lt_1_addr;
	/* [0x9c] AN/LT wrapper register file data */
	uint32_t an_lt_1_data;
	/* [0xa0] AN/LT wrapper  register file address */
	uint32_t an_lt_2_addr;
	/* [0xa4] AN/LT wrapper register file data */
	uint32_t an_lt_2_data;
	/* [0xa8] AN/LT wrapper  register file address */
	uint32_t an_lt_3_addr;
	/* [0xac] AN/LT wrapper register file data */
	uint32_t an_lt_3_data;
	/* [0xb0] External SERDES control */
	uint32_t ext_serdes_ctrl;
	uint32_t rsrvd[19];
};

struct al_eth_mac_regs {
	struct al_eth_mac_1g mac_1g;				/* [0x000] */
	struct al_eth_mac_10g mac_10g;				/* [0x400] */
	uint32_t rsrvd_0[64];					/* [0x800] */
	struct al_eth_mac_gen gen;                              /* [0x900] */
	struct al_eth_mac_kr kr;                                /* [0xa00] */
	struct al_eth_mac_sgmii sgmii;                          /* [0xb00] */
	struct al_eth_mac_stat stat;                            /* [0xc00] */
	struct al_eth_mac_stat_lane stat_lane[4];               /* [0xd00] */
	struct al_eth_mac_gen_v3 gen_v3;                        /* [0xe00] */
};

/*
* Registers Fields
*/

/**** control register (1G mac) ****/
/* enable Half Duplex */
#define AL_ETH_1G_MAC_CTRL_HD_EN		(1 << 10)
/* enable 1G speed */
#define AL_ETH_1G_MAC_CTRL_1G_SPD		(1 << 3)
/* enable 10M speed */
#define AL_ETH_1G_MAC_CTRL_10M_SPD		(1 << 25)

/**** 10G MAC register ****/
/* mdio_cfg_status */
#define ETH_10G_MAC_MDIO_CFG_HOLD_TIME_MASK	0x0000001c
#define ETH_10G_MAC_MDIO_CFG_HOLD_TIME_SHIFT	2
#define ETH_10G_MAC_MDIO_CFG_HOLD_TIME_7_CLK	3
/* if_mode */
#define ETH_10G_MAC_IF_MODE_SGMII_EN_MASK	0x00000001
#define ETH_10G_MAC_IF_MODE_SGMII_AN_MASK	0x00000002
#define ETH_10G_MAC_IF_MODE_SGMII_SPEED_MASK	0x0000000c
#define ETH_10G_MAC_IF_MODE_SGMII_SPEED_SHIFT	2
#define ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_MASK	0x00000010
#define ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_SHIFT	4
#define ETH_10G_MAC_IF_MODE_SGMII_SPEED_10M	0
#define ETH_10G_MAC_IF_MODE_SGMII_SPEED_100M	1
#define ETH_10G_MAC_IF_MODE_SGMII_SPEED_1G	2
#define ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_FULL	0
#define ETH_10G_MAC_IF_MODE_SGMII_DUPLEX_HALF	1
/* control */
#define ETH_10G_MAC_CONTROL_AN_EN_MASK		0x00001000

/**** rgmii_cfg register ****/
/* Selection of the input for the "set_1000" input of the RGMII  ... */
#define ETH_MAC_GEN_RGMII_CFG_SET_1000_SEL (1 << 0)
/* Selection of the input for the "set_10" input of the RGMII co ... */
#define ETH_MAC_GEN_RGMII_CFG_SET_10_SEL (1 << 4)
/* Enable automatic speed selection (based on PHY in-band status ... */
#define ETH_MAC_GEN_RGMII_CFG_ENA_AUTO   (1 << 8)

/**** mac_10g_stat register ****/
/* XGMII RS detects local fault */
#define ETH_MAC_GEN_MAC_10G_STAT_LOC_FAULT (1 << 0)
/* XGMII RS detects remote fault */
#define ETH_MAC_GEN_MAC_10G_STAT_REM_FAULT (1 << 1)

/* KR PCS Input 00 - SERDES 0 01 - SERDES 1 10 - SERDES 2 11 - S ... */
#define ETH_MAC_GEN_MUX_SEL_KR_IN_MASK   0x0000C000

/**** led_cfg register ****/
/* LED source selection:0 – Default reg1 – Rx activity2 – Tx act ... */
#define ETH_MAC_GEN_LED_CFG_SEL_MASK     0x0000000F
#define ETH_MAC_GEN_LED_CFG_SEL_SHIFT    0

/* turn the led on/off based on default value field (ETH_MAC_GEN_LED_CFG_DEF) */
#define ETH_MAC_GEN_LED_CFG_SEL_DEFAULT_REG	0
#define ETH_MAC_GEN_LED_CFG_SEL_LINK_ACTIVITY 10

/* activity timer (MSB)
32 bit timer @SB clock */
#define ETH_MAC_GEN_LED_CFG_ACT_TIMER_MASK 0x00FF0000
#define ETH_MAC_GEN_LED_CFG_ACT_TIMER_SHIFT 16
/* activity timer (MSB)
32 bit timer @SB clock */
#define ETH_MAC_GEN_LED_CFG_BLINK_TIMER_MASK 0xFF000000
#define ETH_MAC_GEN_LED_CFG_BLINK_TIMER_SHIFT 24

#define ETH_MAC_SGMII_REG_ADDR_CTRL_REG	0x0
#define ETH_MAC_SGMII_REG_ADDR_IF_MODE_REG 0x14

#define ETH_MAC_SGMII_REG_DATA_CTRL_AN_ENABLE			(1 << 12)
#define ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_EN			(1 << 0)
#define ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_AN			(1 << 1)
#define ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_SPEED_100		0x1
#define ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_SPEED_1000		0x2
#define ETH_MAC_SGMII_REG_DATA_IF_MODE_SGMII_DUPLEX		(1 << 4)

/**** link_stat register ****/

/*** MAC Core registers addresses ***/
/* command config */
#define ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_ADDR	0x00000008
#define ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_TX_ENA	(1 << 0)
#define ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_RX_ENA	(1 << 1)
#define ETH_MAC_GEN_V3_MAC_40G_COMMAND_CONFIG_PFC_MODE	(1 << 19)

/* frame length */
#define ETH_MAC_GEN_V3_MAC_40G_FRM_LENGTH_ADDR		0x00000014

#define ETH_MAC_GEN_V3_MAC_40G_CL01_PAUSE_QUANTA_ADDR	0x00000054
#define ETH_MAC_GEN_V3_MAC_40G_CL23_PAUSE_QUANTA_ADDR	0x00000058
#define ETH_MAC_GEN_V3_MAC_40G_CL45_PAUSE_QUANTA_ADDR	0x0000005C
#define ETH_MAC_GEN_V3_MAC_40G_CL67_PAUSE_QUANTA_ADDR	0x00000060
#define ETH_MAC_GEN_V3_MAC_40G_CL01_QUANTA_THRESH_ADDR	0x00000064
#define ETH_MAC_GEN_V3_MAC_40G_CL23_QUANTA_THRESH_ADDR	0x00000068
#define ETH_MAC_GEN_V3_MAC_40G_CL45_QUANTA_THRESH_ADDR	0x0000006C
#define ETH_MAC_GEN_V3_MAC_40G_CL67_QUANTA_THRESH_ADDR	0x00000070

#endif /* __AL_HAL_ETH_MAC_REG_H */

/** @} end of Ethernet group */
