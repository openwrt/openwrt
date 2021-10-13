/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



#ifndef _HORUS_REG_H_
#define _HORUS_REG_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#define MAX_ENTRY_LEN 128

#define HSL_RW 1
#define HSL_RO 0


    /* Garuda Mask Control Register */
#define MASK_CTL                  "mask"
#define MASK_CTL_ID               0
#define MASK_CTL_OFFSET           0x0000
#define MASK_CTL_E_LENGTH         4
#define MASK_CTL_E_OFFSET         0
#define MASK_CTL_NR_E             1

#define SOFT_RST                              "mask_rst"
#define MASK_CTL_SOFT_RST_BOFFSET             31
#define MASK_CTL_SOFT_RST_BLEN                1
#define MASK_CTL_SOFT_RST_FLAG                HSL_RW

#define MII_CLK5_SEL                          "mask_clk5s"
#define MASK_CTL_MII_CLK5_SEL_BOFFSET         21
#define MASK_CTL_MII_CLK5_SEL_BLEN            1
#define MASK_CTL_MII_CLK5_SEL_FLAG            HSL_RW

#define MII_CLK0_SEL                          "mask_clk0s"
#define MASK_CTL_MII_CLK0_SEL_BOFFSET         20
#define MASK_CTL_MII_CLK0_SEL_BLEN            1
#define MASK_CTL_MII_CLK0_SEL_FLAG            HSL_RW

#define LOAD_EEPROM                           "mask_ldro"
#define MASK_CTL_LOAD_EEPROM_BOFFSET          16
#define MASK_CTL_LOAD_EEPROM_BLEN             1
#define MASK_CTL_LOAD_EEPROM_FLAG             HSL_RW

#define DEVICE_ID                             "mask_did"
#define MASK_CTL_DEVICE_ID_BOFFSET            8
#define MASK_CTL_DEVICE_ID_BLEN               8
#define MASK_CTL_DEVICE_ID_FLAG               HSL_RO

#define REV_ID                                "mask_rid"
#define MASK_CTL_REV_ID_BOFFSET               0
#define MASK_CTL_REV_ID_BLEN                  8
#define MASK_CTL_REV_ID_FLAG                  HSL_RO


    /* Garuda Mask Control Register */
#define POSTRIP                  "postrip"
#define POSTRIP_ID               0
#define POSTRIP_OFFSET           0x0008
#define POSTRIP_E_LENGTH         4
#define POSTRIP_E_OFFSET         0
#define POSTRIP_NR_E             1

#define POWER_ON_SEL                           "postrip_sel"
#define POSTRIP_POWER_ON_SEL_BOFFSET           31
#define POSTRIP_POWER_ON_SEL_BLEN              1
#define POSTRIP_POWER_ON_SEL_FLAG              HSL_RW

#define RXDELAY_S1                             "postrip_rx_s1"
#define POSTRIP_RXDELAY_S1_BOFFSET             26
#define POSTRIP_RXDELAY_S1_BLEN                1
#define POSTRIP_RXDELAY_S1_FLAG                HSL_RW

#define SPI_EN                                 "postrip_spi"
#define POSTRIP_SPI_EN_BOFFSET                 25
#define POSTRIP_SPI_EN_BLEN                    1
#define POSTRIP_SPI_EN_FLAG                    HSL_RW

#define LED_OPEN_EN                            "postrip_led"
#define POSTRIP_LED_OPEN_EN_BOFFSET            24
#define POSTRIP_LED_OPEN_EN_BLEN               1
#define POSTRIP_LED_OPEN_EN_FLAG               HSL_RW

#define RXDELAY_S0                             "postrip_rx_s0"
#define POSTRIP_RXDELAY_S0_BOFFSET             23
#define POSTRIP_RXDELAY_S0_BLEN                1
#define POSTRIP_RXDELAY_S0_FLAG                HSL_RW

#define TXDELAY_S1                             "postrip_tx_s1"
#define POSTRIP_TXDELAY_S1_BOFFSET             22
#define POSTRIP_TXDELAY_S1_BLEN                1
#define POSTRIP_TXDELAY_S1_FLAG                HSL_RW

#define TXDELAY_S0                             "postrip_tx_s0"
#define POSTRIP_TXDELAY_S0_BOFFSET             21
#define POSTRIP_TXDELAY_S0_BLEN                1
#define POSTRIP_TXDELAY_S0_FLAG                HSL_RW

#define LPW_EXIT                               "postrip_lpw_exit"
#define POSTRIP_LPW_EXIT_BOFFSET               20
#define POSTRIP_LPW_EXIT_BLEN                  1
#define POSTRIP_LPW_EXIT_FLAG                  HSL_RW

#define PHY_PLL_ON                             "postrip_phy_pll"
#define POSTRIP_PHY_PLL_ON_BOFFSET             19
#define POSTRIP_PHY_PLL_ON_BLEN                1
#define POSTRIP_PHY_PLL_ON_FLAG                HSL_RW

#define MAN_ENABLE                             "postrip_man_en"
#define POSTRIP_MAN_ENABLE_BOFFSET             18
#define POSTRIP_MAN_ENABLE_BLEN                1
#define POSTRIP_MAN_ENABLE_FLAG                HSL_RW

#define LPW_STATE_EN                           "postrip_lpw_state"
#define POSTRIP_LPW_STATE_EN_BOFFSET           17
#define POSTRIP_LPW_STATE_EN_BLEN              1
#define POSTRIP_LPW_STATE_EN_FLAG              HSL_RW

#define POWER_DOWN_HW                          "postrip_power_down"
#define POSTRIP_POWER_DOWN_HW_BOFFSET          16
#define POSTRIP_POWER_DOWN_HW_BLEN             1
#define POSTRIP_POWER_DOWN_HW_FLAG             HSL_RW

#define MAC5_PHY_MODE                          "postrip_mac5_phy"
#define POSTRIP_MAC5_PHY_MODE_BOFFSET          15
#define POSTRIP_MAC5_PHY_MODE_BLEN             1
#define POSTRIP_MAC5_PHY_MODE_FLAG             HSL_RW

#define MAC5_MAC_MODE                          "postrip_mac5_mac"
#define POSTRIP_MAC5_MAC_MODE_BOFFSET          14
#define POSTRIP_MAC5_MAC_MODE_BLEN             1
#define POSTRIP_MAC5_MAC_MODE_FLAG             HSL_RW

#define DBG_MODE_I                             "postrip_dbg"
#define POSTRIP_DBG_MODE_I_BOFFSET             13
#define POSTRIP_DBG_MODE_I_BLEN                1
#define POSTRIP_DBG_MODE_I_FLAG                HSL_RW

#define HIB_PULSE_HW                           "postrip_hib"
#define POSTRIP_HIB_PULSE_HW_BOFFSET           12
#define POSTRIP_HIB_PULSE_HW_BLEN              1
#define POSTRIP_HIB_PULSE_HW_FLAG              HSL_RW

#define SEL_CLK25M                             "postrip_clk25"
#define POSTRIP_SEL_CLK25M_BOFFSET             11
#define POSTRIP_SEL_CLK25M_BLEN                1
#define POSTRIP_SEL_CLK25M_FLAG                HSL_RW

#define GATE_25M_EN                            "postrip_gate25"
#define POSTRIP_GATE_25M_EN_BOFFSET            10
#define POSTRIP_GATE_25M_EN_BLEN               1
#define POSTRIP_GATE_25M_EN_FLAG               HSL_RW

#define SEL_ANA_RST                            "postrip_sel_ana"
#define POSTRIP_SEL_ANA_RST_BOFFSET            9
#define POSTRIP_SEL_ANA_RST_BLEN               1
#define POSTRIP_SEL_ANA_RST_FLAG               HSL_RW

#define SERDES_EN                              "postrip_serdes_en"
#define POSTRIP_SERDES_EN_BOFFSET              8
#define POSTRIP_SERDES_EN_BLEN                 1
#define POSTRIP_SERDES_EN_FLAG                 HSL_RW

#define RGMII_TXCLK_DELAY_EN                   "postrip_tx_delay"
#define POSTRIP_RGMII_TXCLK_DELAY_EN_BOFFSET   7
#define POSTRIP_RGMII_TXCLK_DELAY_EN_BLEN      1
#define POSTRIP_RGMII_TXCLK_DELAY_EN_FLAG      HSL_RW

#define RGMII_RXCLK_DELAY_EN                   "postrip_rx_delay"
#define POSTRIP_RGMII_RXCLK_DELAY_EN_BOFFSET   6
#define POSTRIP_RGMII_RXCLK_DELAY_EN_BLEN      1
#define POSTRIP_RGMII_RXCLK_DELAY_EN_FLAG      HSL_RW

#define RTL_MODE                               "postrip_rtl"
#define POSTRIP_RTL_MODE_BOFFSET               5
#define POSTRIP_RTL_MODE_BLEN                  1
#define POSTRIP_RTL_MODE_FLAG                  HSL_RW

#define MAC0_MAC_MODE                          "postrip_mac0_mac"
#define POSTRIP_MAC0_MAC_MODE_BOFFSET          4
#define POSTRIP_MAC0_MAC_MODE_BLEN             1
#define POSTRIP_MAC0_MAC_MODE_FLAG             HSL_RW

#define PHY4_RGMII_EN                          "postrip_phy4_rgmii"
#define POSTRIP_PHY4_RGMII_EN_BOFFSET          3
#define POSTRIP_PHY4_RGMII_EN_BLEN             1
#define POSTRIP_PHY4_RGMII_EN_FLAG             HSL_RW

#define PHY4_GMII_EN                           "postrip_phy4_gmii"
#define POSTRIP_PHY4_GMII_EN_BOFFSET           2
#define POSTRIP_PHY4_GMII_EN_BLEN              1
#define POSTRIP_PHY4_GMII_EN_FLAG              HSL_RW

#define MAC0_RGMII_EN                          "postrip_mac0_rgmii"
#define POSTRIP_MAC0_RGMII_EN_BOFFSET          1
#define POSTRIP_MAC0_RGMII_EN_BLEN             1
#define POSTRIP_MAC0_RGMII_EN_FLAG             HSL_RW

#define MAC0_GMII_EN                           "postrip_mac0_gmii"
#define POSTRIP_MAC0_GMII_EN_BOFFSET           0
#define POSTRIP_MAC0_GMII_EN_BLEN              1
#define POSTRIP_MAC0_GMII_EN_FLAG              HSL_RW



    /* Global Interrupt Register */
#define GLOBAL_INT                "gint"
#define GLOBAL_INT_ID             1
#define GLOBAL_INT_OFFSET         0x0014
#define GLOBAL_INT_E_LENGTH       4
#define GLOBAL_INT_E_OFFSET       0
#define GLOBAL_INT_NR_E           1

#define GLB_QM_ERR_CNT                            "gint_qmen"
#define GLOBAL_INT_GLB_QM_ERR_CNT_BOFFSET         24
#define GLOBAL_INT_GLB_QM_ERR_CNT_BLEN            8
#define GLOBAL_INT_GLB_QM_ERR_CNT_FLAG            HSL_RO

#define GLB_LOOKUP_ERR                            "gint_glblper"
#define GLOBAL_INT_GLB_LOOKUP_ERR_BOFFSET         17
#define GLOBAL_INT_GLB_LOOKUP_ERR_BLEN            1
#define GLOBAL_INT_GLB_LOOKUP_ERR_FLAG            HSL_RW

#define GLB_QM_ERR                                "gint_glbqmer"
#define GLOBAL_INT_GLB_QM_ERR_BOFFSET             16
#define GLOBAL_INT_GLB_QM_ERR_BLEN                1
#define GLOBAL_INT_GLB_QM_ERR_FLAG                HSL_RW

#define GLB_HW_INI_DONE                           "gint_hwid"
#define GLOBAL_INT_GLB_HW_INI_DONE_BOFFSET        14
#define GLOBAL_INT_GLB_HW_INI_DONE_BLEN           1
#define GLOBAL_INT_GLB_HW_INI_DONE_FLAG           HSL_RW

#define GLB_MIB_INI                               "gint_mibi"
#define GLOBAL_INT_GLB_MIB_INI_BOFFSET            13
#define GLOBAL_INT_GLB_MIB_INI_BLEN               1
#define GLOBAL_INT_GLB_MIB_INI_FLAG               HSL_RW

#define GLB_MIB_DONE                              "gint_mibd"
#define GLOBAL_INT_GLB_MIB_DONE_BOFFSET           12
#define GLOBAL_INT_GLB_MIB_DONE_BLEN              1
#define GLOBAL_INT_GLB_MIB_DONE_FLAG              HSL_RW

#define GLB_BIST_DONE                             "gint_bisd"
#define GLOBAL_INT_GLB_BIST_DONE_BOFFSET          11
#define GLOBAL_INT_GLB_BIST_DONE_BLEN             1
#define GLOBAL_INT_GLB_BIST_DONE_FLAG             HSL_RW

#define GLB_VT_MISS_VIO                           "gint_vtms"
#define GLOBAL_INT_GLB_VT_MISS_VIO_BOFFSET        10
#define GLOBAL_INT_GLB_VT_MISS_VIO_BLEN           1
#define GLOBAL_INT_GLB_VT_MISS_VIO_FLAG           HSL_RW

#define GLB_VT_MEM_VIO                            "gint_vtme"
#define GLOBAL_INT_GLB_VT_MEM_VIO_BOFFSET         9
#define GLOBAL_INT_GLB_VT_MEM_VIO_BLEN            1
#define GLOBAL_INT_GLB_VT_MEM_VIO_FLAG            HSL_RW

#define GLB_VT_DONE                               "gint_vtd"
#define GLOBAL_INT_GLB_VT_DONE_BOFFSET            8
#define GLOBAL_INT_GLB_VT_DONE_BLEN               1
#define GLOBAL_INT_GLB_VT_DONE_FLAG               HSL_RW

#define GLB_QM_INI                                "gint_qmin"
#define GLOBAL_INT_GLB_QM_INI_BOFFSET             7
#define GLOBAL_INT_GLB_QM_INI_BLEN                1
#define GLOBAL_INT_GLB_QM_INI_FLAG                HSL_RW

#define GLB_AT_INI                                "gint_atin"
#define GLOBAL_INT_GLB_AT_INI_BOFFSET             6
#define GLOBAL_INT_GLB_AT_INI_BLEN                1
#define GLOBAL_INT_GLB_AT_INI_FLAG                HSL_RW

#define GLB_ARL_FULL                              "gint_arlf"
#define GLOBAL_INT_GLB_ARL_FULL_BOFFSET           5
#define GLOBAL_INT_GLB_ARL_FULL_BLEN              1
#define GLOBAL_INT_GLB_ARL_FULL_FLAG              HSL_RW

#define GLB_ARL_DONE                              "gint_arld"
#define GLOBAL_INT_GLB_ARL_DONE_BOFFSET           4
#define GLOBAL_INT_GLB_ARL_DONE_BLEN              1
#define GLOBAL_INT_GLB_ARL_DONE_FLAG              HSL_RW

#define GLB_MDIO_DONE                             "gint_mdid"
#define GLOBAL_INT_GLB_MDIO_DONE_BOFFSET          3
#define GLOBAL_INT_GLB_MDIO_DONE_BLEN             1
#define GLOBAL_INT_GLB_MDIO_DONE_FLAG             HSL_RW

#define GLB_PHY_INT                               "gint_phyi"
#define GLOBAL_INT_GLB_PHY_INT_BOFFSET            2
#define GLOBAL_INT_GLB_PHY_INT_BLEN               1
#define GLOBAL_INT_GLB_PHY_INT_FLAG               HSL_RW

#define GLB_EEPROM_ERR                            "gint_epei"
#define GLOBAL_INT_GLB_EEPROM_ERR_BOFFSET         1
#define GLOBAL_INT_GLB_EEPROM_ERR_BLEN            1
#define GLOBAL_INT_GLB_EEPROM_ERR_FLAG            HSL_RW

#define GLB_EEPROM_INT                            "gint_epi"
#define GLOBAL_INT_GLB_EEPROM_INT_BOFFSET         0
#define GLOBAL_INT_GLB_EEPROM_INT_BLEN            1
#define GLOBAL_INT_GLB_EEPROM_INT_FLAG            HSL_RW


    /* Global Interrupt Mask Register */
#define GLOBAL_INT_MASK           "gintm"
#define GLOBAL_INT_MASK_ID        2
#define GLOBAL_INT_MASK_OFFSET    0x0018
#define GLOBAL_INT_MASK_E_LENGTH  4
#define GLOBAL_INT_MASK_E_OFFSET  0
#define GLOBAL_INT_MASK_NR_E      1

#define GLBM_LOOKUP_ERR                          "gintm_lpe"
#define GLOBAL_INT_MASK_GLBM_LOOKUP_ERR_BOFFSET  17
#define GLOBAL_INT_MASK_GLBM_LOOKUP_ERR_BLEN     1
#define GLOBAL_INT_MASK_GLBM_LOOKUP_ERR_FLAG     HSL_RW

#define GLBM_QM_ERR                                "gintm_qme"
#define GLOBAL_INT_MASK_GLBM_QM_ERR_BOFFSET        16
#define GLOBAL_INT_MASK_GLBM_QM_ERR_BLEN           1
#define GLOBAL_INT_MASK_GLBM_QM_ERR_FLAG           HSL_RW

#define GLBM_HW_INI_DONE                           "gintm_hwid"
#define GLOBAL_INT_MASK_GLBM_HW_INI_DONE_BOFFSET   14
#define GLOBAL_INT_MASK_GLBM_HW_INI_DONE_BLEN      1
#define GLOBAL_INT_MASK_GLBM_HW_INI_DONE_FLAG      HSL_RW

#define GLBM_MIB_INI                               "gintm_mibi"
#define GLOBAL_INT_MASK_GLBM_MIB_INI_BOFFSET       13
#define GLOBAL_INT_MASK_GLBM_MIB_INI_BLEN          1
#define GLOBAL_INT_MASK_GLBM_MIB_INI_FLAG          HSL_RW

#define GLBM_MIB_DONE                              "gintm_mibd"
#define GLOBAL_INT_MASK_GLBM_MIB_DONE_BOFFSET      12
#define GLOBAL_INT_MASK_GLBM_MIB_DONE_BLEN         1
#define GLOBAL_INT_MASK_GLBM_MIB_DONE_FLAG         HSL_RW

#define GLBM_BIST_DONE                             "gintm_bisd"
#define GLOBAL_INT_MASK_GLBM_BIST_DONE_BOFFSET     11
#define GLOBAL_INT_MASK_GLBM_BIST_DONE_BLEN        1
#define GLOBAL_INT_MASK_GLBM_BIST_DONE_FLAG        HSL_RW

#define GLBM_VT_MISS_VIO                           "gintm_vtms"
#define GLOBAL_INT_MASK_GLBM_VT_MISS_VIO_BOFFSET   10
#define GLOBAL_INT_MASK_GLBM_VT_MISS_VIO_BLEN      1
#define GLOBAL_INT_MASK_GLBM_VT_MISS_VIO_FLAG      HSL_RW

#define GLBM_VT_MEM_VIO                            "gintm_vtme"
#define GLOBAL_INT_MASK_GLBM_VT_MEM_VIO_BOFFSET    9
#define GLOBAL_INT_MASK_GLBM_VT_MEM_VIO_BLEN       1
#define GLOBAL_INT_MASK_GLBM_VT_MEM_VIO_FLAG       HSL_RW

#define GLBM_VT_DONE                               "gintm_vtd"
#define GLOBAL_INT_MASK_GLBM_VT_DONE_BOFFSET       8
#define GLOBAL_INT_MASK_GLBM_VT_DONE_BLEN          1
#define GLOBAL_INT_MASK_GLBM_VT_DONE_FLAG          HSL_RW

#define GLBM_QM_INI                                "gintm_qmin"
#define GLOBAL_INT_MASK_GLBM_QM_INI_BOFFSET        7
#define GLOBAL_INT_MASK_GLBM_QM_INI_BLEN           1
#define GLOBAL_INT_MASK_GLBM_QM_INI_FLAG           HSL_RW

#define GLBM_AT_INI                                "gintm_atin"
#define GLOBAL_INT_MASK_GLBM_AT_INI_BOFFSET        6
#define GLOBAL_INT_MASK_GLBM_AT_INI_BLEN           1
#define GLOBAL_INT_MASK_GLBM_AT_INI_FLAG           HSL_RW

#define GLBM_ARL_FULL                              "gintm_arlf"
#define GLOBAL_INT_MASK_GLBM_ARL_FULL_BOFFSET      5
#define GLOBAL_INT_MASK_GLBM_ARL_FULL_BLEN         1
#define GLOBAL_INT_MASK_GLBM_ARL_FULL_FLAG         HSL_RW

#define GLBM_ARL_DONE                              "gintm_arld"
#define GLOBAL_INT_MASK_GLBM_ARL_DONE_BOFFSET      4
#define GLOBAL_INT_MASK_GLBM_ARL_DONE_BLEN         1
#define GLOBAL_INT_MASK_GLBM_ARL_DONE_FLAG         HSL_RW

#define GLBM_MDIO_DONE                             "gintm_mdid"
#define GLOBAL_INT_MASK_GLBM_MDIO_DONE_BOFFSET     3
#define GLOBAL_INT_MASK_GLBM_MDIO_DONE_BLEN        1
#define GLOBAL_INT_MASK_GLBM_MDIO_DONE_FLAG        HSL_RW

#define GLBM_PHY_INT                               "gintm_phy"
#define GLOBAL_INT_MASK_GLBM_PHY_INT_BOFFSET       2
#define GLOBAL_INT_MASK_GLBM_PHY_INT_BLEN          1
#define GLOBAL_INT_MASK_GLBM_PHY_INT_FLAG          HSL_RW

#define GLBM_EEPROM_ERR                            "gintm_epe"
#define GLOBAL_INT_MASK_GLBM_EEPROM_ERR_BOFFSET    1
#define GLOBAL_INT_MASK_GLBM_EEPROM_ERR_BLEN       1
#define GLOBAL_INT_MASK_GLBM_EEPROM_ERR_FLAG       HSL_RW

#define GLBM_EEPROM_INT                            "gintm_ep"
#define GLOBAL_INT_MASK_GLBM_EEPROM_INT_BOFFSET    0
#define GLOBAL_INT_MASK_GLBM_EEPROM_INT_BLEN       1
#define GLOBAL_INT_MASK_GLBM_EEPROM_INT_FLAG       HSL_RW


    /* Global MAC Address Register */
#define GLOBAL_MAC_ADDR0           "gmac0"
#define GLOBAL_MAC_ADDR0_ID        3
#define GLOBAL_MAC_ADDR0_OFFSET    0x0020
#define GLOBAL_MAC_ADDR0_E_LENGTH  4
#define GLOBAL_MAC_ADDR0_E_OFFSET  0
#define GLOBAL_MAC_ADDR0_NR_E      1

#define GLB_BYTE4                                  "gmac_b4"
#define GLOBAL_MAC_ADDR0_GLB_BYTE4_BOFFSET         8
#define GLOBAL_MAC_ADDR0_GLB_BYTE4_BLEN            8
#define GLOBAL_MAC_ADDR0_GLB_BYTE4_FLAG            HSL_RW

#define GLB_BYTE5                                  "gmac_b5"
#define GLOBAL_MAC_ADDR0_GLB_BYTE5_BOFFSET         0
#define GLOBAL_MAC_ADDR0_GLB_BYTE5_BLEN            8
#define GLOBAL_MAC_ADDR0_GLB_BYTE5_FLAG            HSL_RW

#define GLOBAL_MAC_ADDR1           "gmac1"
#define GLOBAL_MAC_ADDR1_ID        4
#define GLOBAL_MAC_ADDR1_OFFSET    0x0024
#define GLOBAL_MAC_ADDR1_E_LENGTH  4
#define GLOBAL_MAC_ADDR1_E_OFFSET  0
#define GLOBAL_MAC_ADDR1_NR_E      1

#define GLB_BYTE0                                  "gmac_b0"
#define GLOBAL_MAC_ADDR1_GLB_BYTE0_BOFFSET         24
#define GLOBAL_MAC_ADDR1_GLB_BYTE0_BLEN            8
#define GLOBAL_MAC_ADDR1_GLB_BYTE0_FLAG            HSL_RW

#define GLB_BYTE1                                  "gmac_b1"
#define GLOBAL_MAC_ADDR1_GLB_BYTE1_BOFFSET         16
#define GLOBAL_MAC_ADDR1_GLB_BYTE1_BLEN            8
#define GLOBAL_MAC_ADDR1_GLB_BYTE1_FLAG            HSL_RW

#define GLB_BYTE2                                  "gmac_b2"
#define GLOBAL_MAC_ADDR1_GLB_BYTE2_BOFFSET         8
#define GLOBAL_MAC_ADDR1_GLB_BYTE2_BLEN            8
#define GLOBAL_MAC_ADDR1_GLB_BYTE2_FLAG            HSL_RW

#define GLB_BYTE3                                  "gmac_b3"
#define GLOBAL_MAC_ADDR1_GLB_BYTE3_BOFFSET         0
#define GLOBAL_MAC_ADDR1_GLB_BYTE3_BLEN            8
#define GLOBAL_MAC_ADDR1_GLB_BYTE3_FLAG            HSL_RW


    /* Flood Mask Register */
#define FLOOD_MASK                "fmask"
#define FLOOD_MASK_ID             5
#define FLOOD_MASK_OFFSET         0x002c
#define FLOOD_MASK_E_LENGTH       4
#define FLOOD_MASK_E_OFFSET       0
#define FLOOD_MASK_NR_E           1

#define BC_FLOOD_DP                         "fmask_bfdp"
#define FLOOD_MASK_BC_FLOOD_DP_BOFFSET      25
#define FLOOD_MASK_BC_FLOOD_DP_BLEN         6
#define FLOOD_MASK_BC_FLOOD_DP_FLAG         HSL_RW

#define ARL_UNI_LEAKY                       "fmask_aulky"
#define FLOOD_MASK_ARL_UNI_LEAKY_BOFFSET    24
#define FLOOD_MASK_ARL_UNI_LEAKY_BLEN       1
#define FLOOD_MASK_ARL_UNI_LEAKY_FLAG       HSL_RW

#define ARL_MUL_LEAKY                       "fmask_amlky"
#define FLOOD_MASK_ARL_MUL_LEAKY_BOFFSET    23
#define FLOOD_MASK_ARL_MUL_LEAKY_BLEN       1
#define FLOOD_MASK_ARL_MUL_LEAKY_FLAG       HSL_RW

#define MUL_FLOOD_DP                        "fmask_mfdp"
#define FLOOD_MASK_MUL_FLOOD_DP_BOFFSET     16
#define FLOOD_MASK_MUL_FLOOD_DP_BLEN        6
#define FLOOD_MASK_MUL_FLOOD_DP_FLAG        HSL_RW

#define IGMP_DP                             "fmask_igmpdp"
#define FLOOD_MASK_IGMP_DP_BOFFSET          8
#define FLOOD_MASK_IGMP_DP_BLEN             6
#define FLOOD_MASK_IGMP_DP_FLAG             HSL_RW

#define UNI_FLOOD_DP                        "fmask_ufdp"
#define FLOOD_MASK_UNI_FLOOD_DP_BOFFSET     0
#define FLOOD_MASK_UNI_FLOOD_DP_BLEN        6
#define FLOOD_MASK_UNI_FLOOD_DP_FLAG        HSL_RW


    /* Global Control Register */
#define GLOBAL_CTL                "gctl"
#define GLOBAL_CTL_ID             5
#define GLOBAL_CTL_OFFSET         0x0030
#define GLOBAL_CTL_E_LENGTH       4
#define GLOBAL_CTL_E_OFFSET       0
#define GLOBAL_CTL_NR_E           1

#define RATE_DROP_EN                          "gctl_rden"
#define GLOBAL_CTL_RATE_DROP_EN_BOFFSET       29
#define GLOBAL_CTL_RATE_DROP_EN_BLEN          1
#define GLOBAL_CTL_RATE_DROP_EN_FLAG          HSL_RW

#define QM_PRI_MODE                           "gctl_qmpm"
#define GLOBAL_CTL_QM_PRI_MODE_BOFFSET        28
#define GLOBAL_CTL_QM_PRI_MODE_BLEN           1
#define GLOBAL_CTL_QM_PRI_MODE_FLAG           HSL_RW

#define RATE_CRE_LIMIT                        "gctl_rcrl"
#define GLOBAL_CTL_RATE_CRE_LIMIT_BOFFSET     26
#define GLOBAL_CTL_RATE_CRE_LIMIT_BLEN        2
#define GLOBAL_CTL_RATE_CRE_LIMIT_FLAG        HSL_RW

#define RATE_TIME_SLOT                        "gctl_rtms"
#define GLOBAL_CTL_RATE_TIME_SLOT_BOFFSET     24
#define GLOBAL_CTL_RATE_TIME_SLOT_BLEN        2
#define GLOBAL_CTL_RATE_TIME_SLOT_FLAG        HSL_RW

#define RELOAD_TIMER                          "gctl_rdtm"
#define GLOBAL_CTL_RELOAD_TIMER_BOFFSET       20
#define GLOBAL_CTL_RELOAD_TIMER_BLEN          4
#define GLOBAL_CTL_RELOAD_TIMER_FLAG          HSL_RW

#define QM_CNT_LOCK                           "gctl_qmcl"
#define GLOBAL_CTL_QM_CNT_LOCK_BOFFSET        19
#define GLOBAL_CTL_QM_CNT_LOCK_BLEN           1
#define GLOBAL_CTL_QM_CNT_LOCK_FLAG           HSL_RO

#define BROAD_DROP_EN                         "gctl_bden"
#define GLOBAL_CTL_BROAD_DROP_EN_BOFFSET      18
#define GLOBAL_CTL_BROAD_DROP_EN_BLEN         1
#define GLOBAL_CTL_BROAD_DROP_EN_FLAG         HSL_RW

#define MAX_FRAME_SIZE                        "gctl_mfsz"
#define GLOBAL_CTL_MAX_FRAME_SIZE_BOFFSET     0
#define GLOBAL_CTL_MAX_FRAME_SIZE_BLEN        14
#define GLOBAL_CTL_MAX_FRAME_SIZE_FLAG        HSL_RW


    /* Flow Control Register */
#define FLOW_CTL0                  "fctl"
#define FLOW_CTL0_ID               6
#define FLOW_CTL0_OFFSET           0x0034
#define FLOW_CTL0_E_LENGTH         4
#define FLOW_CTL0_E_OFFSET         0
#define FLOW_CTL0_NR_E             1

#define TEST_PAUSE                             "fctl_tps"
#define FLOW_CTL0_TEST_PAUSE_BOFFSET           31
#define FLOW_CTL0_TEST_PAUSE_BLEN              1
#define FLOW_CTL0_TEST_PAUSE_FLAG              HSL_RW


#define GOL_PAUSE_ON_THRES                     "fctl_gont"
#define FLOW_CTL0_GOL_PAUSE_ON_THRES_BOFFSET   16
#define FLOW_CTL0_GOL_PAUSE_ON_THRES_BLEN      8
#define FLOW_CTL0_GOL_PAUSE_ON_THRES_FLAG      HSL_RW

#define GOL_PAUSE_OFF_THRES                    "fctl_gofft"
#define FLOW_CTL0_GOL_PAUSE_OFF_THRES_BOFFSET  0
#define FLOW_CTL0_GOL_PAUSE_OFF_THRES_BLEN     8
#define FLOW_CTL0_GOL_PAUSE_OFF_THRES_FLAG     HSL_RW




    /* Flow Control1 Register */
#define FLOW_CTL1                  "fctl1"
#define FLOW_CTL1_ID               6
#define FLOW_CTL1_OFFSET           0x0038
#define FLOW_CTL1_E_LENGTH         4
#define FLOW_CTL1_E_OFFSET         0
#define FLOW_CTL1_NR_E             1

#define PORT_PAUSE_ON_THRES                    "fctl1_pont"
#define FLOW_CTL1_PORT_PAUSE_ON_THRES_BOFFSET  16
#define FLOW_CTL1_PORT_PAUSE_ON_THRES_BLEN     8
#define FLOW_CTL1_PORT_PAUSE_ON_THRES_FLAG     HSL_RW

#define PORT_PAUSE_OFF_THRES                   "fctl1_pofft"
#define FLOW_CTL1_PORT_PAUSE_OFF_THRES_BOFFSET 0
#define FLOW_CTL1_PORT_PAUSE_OFF_THRES_BLEN    8
#define FLOW_CTL1_PORT_PAUSE_OFF_THRES_FLAG    HSL_RW




    /* QM Control Register */
#define QM_CTL                    "qmct"
#define QM_CTL_ID                 7
#define QM_CTL_OFFSET             0x003c
#define QM_CTL_E_LENGTH           4
#define QM_CTL_E_OFFSET           0
#define QM_CTL_NR_E               1

#define QM_ERR_RST_EN                         "qmct_qeren"
#define QM_CTL_QM_ERR_RST_EN_BOFFSET          31
#define QM_CTL_QM_ERR_RST_EN_BLEN             1
#define QM_CTL_QM_ERR_RST_EN_FLAG             HSL_RW

#define LOOKUP_ERR_RST_EN                     "qmct_lpesen"
#define QM_CTL_LOOKUP_ERR_RST_EN_BOFFSET      30
#define QM_CTL_LOOKUP_ERR_RST_EN_BLEN         1
#define QM_CTL_LOOKUP_ERR_RST_EN_FLAG         HSL_RW

#define IGMP_JOIN_STATIC                      "qmct_igmpjs"
#define QM_CTL_IGMP_JOIN_STATIC_BOFFSET       24
#define QM_CTL_IGMP_JOIN_STATIC_BLEN          4
#define QM_CTL_IGMP_JOIN_STATIC_FLAG          HSL_RW

#define IGMP_JOIN_LEAKY                       "qmct_igmpjl"
#define QM_CTL_IGMP_JOIN_LEAKY_BOFFSET        23
#define QM_CTL_IGMP_JOIN_LEAKY_BLEN           1
#define QM_CTL_IGMP_JOIN_LEAKY_FLAG           HSL_RW

#define IGMP_CREAT_EN                         "qmct_igmpcrt"
#define QM_CTL_IGMP_CREAT_EN_BOFFSET          22
#define QM_CTL_IGMP_CREAT_EN_BLEN             1
#define QM_CTL_IGMP_CREAT_EN_FLAG             HSL_RW

#define PPPOE_RDT_EN                          "qmct_pppoerdten"
#define QM_CTL_PPPOE_RDT_EN_BOFFSET           20
#define QM_CTL_PPPOE_RDT_EN_BLEN              1
#define QM_CTL_PPPOE_RDT_EN_FLAG              HSL_RW

#define IGMP_V3_EN                            "qmct_igmpv3e"
#define QM_CTL_IGMP_V3_EN_BOFFSET             19
#define QM_CTL_IGMP_V3_EN_BLEN                1
#define QM_CTL_IGMP_V3_EN_FLAG                HSL_RW

#define IGMP_PRI_EN                           "qmct_igmpprie"
#define QM_CTL_IGMP_PRI_EN_BOFFSET            18
#define QM_CTL_IGMP_PRI_EN_BLEN               1
#define QM_CTL_IGMP_PRI_EN_FLAG               HSL_RW

#define IGMP_PRI                              "qmct_igmppri"
#define QM_CTL_IGMP_PRI_BOFFSET               16
#define QM_CTL_IGMP_PRI_BLEN                  2
#define QM_CTL_IGMP_PRI_FLAG                  HSL_RW

#define ARP_EN                                "qmct_arpe"
#define QM_CTL_ARP_EN_BOFFSET                 15
#define QM_CTL_ARP_EN_BLEN                    1
#define QM_CTL_ARP_EN_FLAG                    HSL_RW

#define ARP_CMD                               "qmct_arpc"
#define QM_CTL_ARP_CMD_BOFFSET                14
#define QM_CTL_ARP_CMD_BLEN                   1
#define QM_CTL_ARP_CMD_FLAG                   HSL_RW

#define RIP_CPY_EN                            "qmct_ripcpyen"
#define QM_CTL_RIP_CPY_EN_BOFFSET             13
#define QM_CTL_RIP_CPY_EN_BLEN                1
#define QM_CTL_RIP_CPY_EN_FLAG                HSL_RW

#define EAPOL_CMD                             "qmct_eapolc"
#define QM_CTL_EAPOL_CMD_BOFFSET              12
#define QM_CTL_EAPOL_CMD_BLEN                 1
#define QM_CTL_EAPOL_CMD_FLAG                 HSL_RW

#define IGMP_COPY_EN                          "qmct_igmpcpy"
#define QM_CTL_IGMP_COPY_EN_BOFFSET           11
#define QM_CTL_IGMP_COPY_EN_BLEN              1
#define QM_CTL_IGMP_COPY_EN_FLAG              HSL_RW

#define PPPOE_EN                              "qmct_pppoeen"
#define QM_CTL_PPPOE_EN_BOFFSET               10
#define QM_CTL_PPPOE_EN_BLEN                  1
#define QM_CTL_PPPOE_EN_FLAG                  HSL_RW

#define QM_FUNC_TEST                          "qmct_qmft"
#define QM_CTL_QM_FUNC_TEST_BOFFSET           9
#define QM_CTL_QM_FUNC_TEST_BLEN              1
#define QM_CTL_QM_FUNC_TEST_FLAG              HSL_RW

#define MS_FC_EN                              "qmct_msfe"
#define QM_CTL_MS_FC_EN_BOFFSET               8
#define QM_CTL_MS_FC_EN_BLEN                  1
#define QM_CTL_MS_FC_EN_FLAG                  HSL_RW

#define FLOW_DROP_EN                          "qmct_fden"
#define QM_CTL_FLOW_DROP_EN_BOFFSET           7
#define QM_CTL_FLOW_DROP_EN_BLEN              1
#define QM_CTL_FLOW_DROP_EN_FLAG              HSL_RW

#define MANAGE_VID_VIO_DROP_EN                "qmct_mden"
#define QM_CTL_MANAGE_VID_VIO_DROP_EN_BOFFSET 6
#define QM_CTL_MANAGE_VID_VIO_DROP_EN_BLEN    1
#define QM_CTL_MANAGE_VID_VIO_DROP_EN_FLAG    HSL_RW

#define FLOW_DROP_CNT                         "qmct_fdcn"
#define QM_CTL_FLOW_DROP_CNT_BOFFSET          0
#define QM_CTL_FLOW_DROP_CNT_BLEN             6
#define QM_CTL_FLOW_DROP_CNT_FLAG             HSL_RW


    /* Vlan Table Function Register */
#define VLAN_TABLE_FUNC0           "vtbf0"
#define VLAN_TABLE_FUNC0_ID        9
#define VLAN_TABLE_FUNC0_OFFSET    0x0040
#define VLAN_TABLE_FUNC0_E_LENGTH  4
#define VLAN_TABLE_FUNC0_E_OFFSET  0
#define VLAN_TABLE_FUNC0_NR_E      1

#define VT_PRI_EN                              "vtbf_vtpen"
#define VLAN_TABLE_FUNC0_VT_PRI_EN_BOFFSET     31
#define VLAN_TABLE_FUNC0_VT_PRI_EN_BLEN        1
#define VLAN_TABLE_FUNC0_VT_PRI_EN_FLAG        HSL_RW

#define VT_PRI                                 "vtbf_vtpri"
#define VLAN_TABLE_FUNC0_VT_PRI_BOFFSET        28
#define VLAN_TABLE_FUNC0_VT_PRI_BLEN           3
#define VLAN_TABLE_FUNC0_VT_PRI_FLAG           HSL_RW

#define VLAN_ID                                "vtbf_vid"
#define VLAN_TABLE_FUNC0_VLAN_ID_BOFFSET       16
#define VLAN_TABLE_FUNC0_VLAN_ID_BLEN          12
#define VLAN_TABLE_FUNC0_VLAN_ID_FLAG          HSL_RW

#define VT_PORT_NUM                            "vtbf_vtpn"
#define VLAN_TABLE_FUNC0_VT_PORT_NUM_BOFFSET   8
#define VLAN_TABLE_FUNC0_VT_PORT_NUM_BLEN      4
#define VLAN_TABLE_FUNC0_VT_PORT_NUM_FLAG      HSL_RW

#define VT_FULL_VIO                            "vtbf_vtflv"
#define VLAN_TABLE_FUNC0_VT_FULL_VIO_BOFFSET   4
#define VLAN_TABLE_FUNC0_VT_FULL_VIO_BLEN      1
#define VLAN_TABLE_FUNC0_VT_FULL_VIO_FLAG      HSL_RW

#define VT_BUSY                                "vtbf_vtbs"
#define VLAN_TABLE_FUNC0_VT_BUSY_BOFFSET       3
#define VLAN_TABLE_FUNC0_VT_BUSY_BLEN          1
#define VLAN_TABLE_FUNC0_VT_BUSY_FLAG          HSL_RW

#define VT_FUNC                                "vtbf_vtfc"
#define VLAN_TABLE_FUNC0_VT_FUNC_BOFFSET       0
#define VLAN_TABLE_FUNC0_VT_FUNC_BLEN          3
#define VLAN_TABLE_FUNC0_VT_FUNC_FLAG          HSL_RW

#define VLAN_TABLE_FUNC1           "vtbf1"
#define VLAN_TABLE_FUNC1_ID        10
#define VLAN_TABLE_FUNC1_OFFSET    0x0044
#define VLAN_TABLE_FUNC1_E_LENGTH  4
#define VLAN_TABLE_FUNC1_E_OFFSET  0
#define VLAN_TABLE_FUNC1_NR_E      1

#define VT_VALID                               "vtbf_vtvd"
#define VLAN_TABLE_FUNC1_VT_VALID_BOFFSET      11
#define VLAN_TABLE_FUNC1_VT_VALID_BLEN         1
#define VLAN_TABLE_FUNC1_VT_VALID_FLAG         HSL_RW

#define LEARN_DIS                              "vtbf_ldis"
#define VLAN_TABLE_FUNC1_LEARN_DIS_BOFFSET     10
#define VLAN_TABLE_FUNC1_LEARN_DIS_BLEN        1
#define VLAN_TABLE_FUNC1_LEARN_DIS_FLAG        HSL_RW

#define VID_MEM                                "vtbf_vidm"
#define VLAN_TABLE_FUNC1_VID_MEM_BOFFSET       0
#define VLAN_TABLE_FUNC1_VID_MEM_BLEN          6
#define VLAN_TABLE_FUNC1_VID_MEM_FLAG          HSL_RW


    /* Address Table Function Register */
#define ADDR_TABLE_FUNC0           "atbf0"
#define ADDR_TABLE_FUNC0_ID        11
#define ADDR_TABLE_FUNC0_OFFSET    0x0050
#define ADDR_TABLE_FUNC0_E_LENGTH  4
#define ADDR_TABLE_FUNC0_E_OFFSET  0
#define ADDR_TABLE_FUNC0_NR_E      1

#define AT_ADDR_BYTE4                          "atbf_adb4"
#define ADDR_TABLE_FUNC0_AT_ADDR_BYTE4_BOFFSET 24
#define ADDR_TABLE_FUNC0_AT_ADDR_BYTE4_BLEN    8
#define ADDR_TABLE_FUNC0_AT_ADDR_BYTE4_FLAG    HSL_RW

#define AT_ADDR_BYTE5                          "atbf_adb5"
#define ADDR_TABLE_FUNC0_AT_ADDR_BYTE5_BOFFSET 16
#define ADDR_TABLE_FUNC0_AT_ADDR_BYTE5_BLEN    8
#define ADDR_TABLE_FUNC0_AT_ADDR_BYTE5_FLAG    HSL_RW

#define AT_FULL_VIO                            "atbf_atfv"
#define ADDR_TABLE_FUNC0_AT_FULL_VIO_BOFFSET   12
#define ADDR_TABLE_FUNC0_AT_FULL_VIO_BLEN      1
#define ADDR_TABLE_FUNC0_AT_FULL_VIO_FLAG      HSL_RW

#define AT_PORT_NUM                            "atbf_atpn"
#define ADDR_TABLE_FUNC0_AT_PORT_NUM_BOFFSET   8
#define ADDR_TABLE_FUNC0_AT_PORT_NUM_BLEN      4
#define ADDR_TABLE_FUNC0_AT_PORT_NUM_FLAG      HSL_RW

#define FLUSH_ST_EN                            "atbf_fsen"
#define ADDR_TABLE_FUNC0_FLUSH_ST_EN_BOFFSET   4
#define ADDR_TABLE_FUNC0_FLUSH_ST_EN_BLEN      1
#define ADDR_TABLE_FUNC0_FLUSH_ST_EN_FLAG      HSL_RW

#define AT_BUSY                                "atbf_atbs"
#define ADDR_TABLE_FUNC0_AT_BUSY_BOFFSET       3
#define ADDR_TABLE_FUNC0_AT_BUSY_BLEN          1
#define ADDR_TABLE_FUNC0_AT_BUSY_FLAG          HSL_RW

#define AT_FUNC                                "atbf_atfc"
#define ADDR_TABLE_FUNC0_AT_FUNC_BOFFSET       0
#define ADDR_TABLE_FUNC0_AT_FUNC_BLEN          3
#define ADDR_TABLE_FUNC0_AT_FUNC_FLAG          HSL_RW

#define ADDR_TABLE_FUNC1           "atbf1"
#define ADDR_TABLE_FUNC1_ID        12
#define ADDR_TABLE_FUNC1_OFFSET    0x0054
#define ADDR_TABLE_FUNC1_E_LENGTH  4
#define ADDR_TABLE_FUNC1_E_OFFSET  0
#define ADDR_TABLE_FUNC1_NR_E      0

#define AT_ADDR_BYTE0                          "atbf_adb0"
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE0_BOFFSET 24
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE0_BLEN    8
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE0_FLAG    HSL_RW

#define AT_ADDR_BYTE1                          "atbf_adb1"
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE1_BOFFSET 16
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE1_BLEN    8
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE1_FLAG    HSL_RW

#define AT_ADDR_BYTE2                          "atbf_adb2"
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE2_BOFFSET 8
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE2_BLEN    8
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE2_FLAG    HSL_RW

#define AT_ADDR_BYTE3                          "atbf_adb3"
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE3_BOFFSET 0
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE3_BLEN    8
#define ADDR_TABLE_FUNC1_AT_ADDR_BYTE3_FLAG    HSL_RW

#define ADDR_TABLE_FUNC2           "atbf2"
#define ADDR_TABLE_FUNC2_ID        13
#define ADDR_TABLE_FUNC2_OFFSET    0x0058
#define ADDR_TABLE_FUNC2_E_LENGTH  4
#define ADDR_TABLE_FUNC2_E_OFFSET  0
#define ADDR_TABLE_FUNC2_NR_E      0

#define COPY_TO_CPU                            "atbf_cpcpu"
#define ADDR_TABLE_FUNC2_COPY_TO_CPU_BOFFSET   26
#define ADDR_TABLE_FUNC2_COPY_TO_CPU_BLEN      1
#define ADDR_TABLE_FUNC2_COPY_TO_CPU_FLAG      HSL_RW

#define REDRCT_TO_CPU                          "atbf_rdcpu"
#define ADDR_TABLE_FUNC2_REDRCT_TO_CPU_BOFFSET 25
#define ADDR_TABLE_FUNC2_REDRCT_TO_CPU_BLEN    1
#define ADDR_TABLE_FUNC2_REDRCT_TO_CPU_FLAG    HSL_RW

#define LEAKY_EN                               "atbf_lkyen"
#define ADDR_TABLE_FUNC2_LEAKY_EN_BOFFSET      24
#define ADDR_TABLE_FUNC2_LEAKY_EN_BLEN         1
#define ADDR_TABLE_FUNC2_LEAKY_EN_FLAG         HSL_RW

#define AT_STATUS                              "atbf_atsts"
#define ADDR_TABLE_FUNC2_AT_STATUS_BOFFSET     16
#define ADDR_TABLE_FUNC2_AT_STATUS_BLEN        4
#define ADDR_TABLE_FUNC2_AT_STATUS_FLAG        HSL_RW

#define CLONE_EN                               "atbf_clone"
#define ADDR_TABLE_FUNC2_CLONE_EN_BOFFSET      15
#define ADDR_TABLE_FUNC2_CLONE_EN_BLEN         1
#define ADDR_TABLE_FUNC2_CLONE_EN_FLAG         HSL_RW

#define SA_DROP_EN                             "atbf_saden"
#define ADDR_TABLE_FUNC2_SA_DROP_EN_BOFFSET    14
#define ADDR_TABLE_FUNC2_SA_DROP_EN_BLEN       1
#define ADDR_TABLE_FUNC2_SA_DROP_EN_FLAG       HSL_RW

#define MIRROR_EN                              "atbf_miren"
#define ADDR_TABLE_FUNC2_MIRROR_EN_BOFFSET     13
#define ADDR_TABLE_FUNC2_MIRROR_EN_BLEN        1
#define ADDR_TABLE_FUNC2_MIRROR_EN_FLAG        HSL_RW

#define AT_PRI_EN                              "atbf_atpen"
#define ADDR_TABLE_FUNC2_AT_PRI_EN_BOFFSET     12
#define ADDR_TABLE_FUNC2_AT_PRI_EN_BLEN        1
#define ADDR_TABLE_FUNC2_AT_PRI_EN_FLAG        HSL_RW

#define AT_PRI                                 "atbf_atpri"
#define ADDR_TABLE_FUNC2_AT_PRI_BOFFSET        10
#define ADDR_TABLE_FUNC2_AT_PRI_BLEN           2
#define ADDR_TABLE_FUNC2_AT_PRI_FLAG           HSL_RW

#define CROSS_PT                               "atbf_cpt"
#define ADDR_TABLE_FUNC2_CROSS_PT_BOFFSET      8
#define ADDR_TABLE_FUNC2_CROSS_PT_BLEN         1
#define ADDR_TABLE_FUNC2_CROSS_PT_FLAG         HSL_RW

#define DES_PORT                               "atbf_desp"
#define ADDR_TABLE_FUNC2_DES_PORT_BOFFSET      0
#define ADDR_TABLE_FUNC2_DES_PORT_BLEN         6
#define ADDR_TABLE_FUNC2_DES_PORT_FLAG         HSL_RW


    /* Address Table Control Register */
#define ADDR_TABLE_CTL            "atbc"
#define ADDR_TABLE_CTL_ID         14
#define ADDR_TABLE_CTL_OFFSET     0x005C
#define ADDR_TABLE_CTL_E_LENGTH   4
#define ADDR_TABLE_CTL_E_OFFSET   0
#define ADDR_TABLE_CTL_NR_E       1

#define LOOP_CH_TIME                           "atbc_lct"
#define ADDR_TABLE_CTL_LOOP_CH_TIME_BOFFSET    24
#define ADDR_TABLE_CTL_LOOP_CH_TIMEP_BLEN      3
#define ADDR_TABLE_CTL_LOOP_CH_TIME_FLAG       HSL_RW

#define RESVID_DROP                            "atbc_rviddrop"
#define ADDR_TABLE_CTL_RESVID_DROP_BOFFSET     22
#define ADDR_TABLE_CTL_RESVID_DROP_BLEN        1
#define ADDR_TABLE_CTL_RESVID_DROP_FLAG        HSL_RW

#define STAG_MODE                              "atbc_stag"
#define ADDR_TABLE_CTL_STAG_MODE_BOFFSET       21
#define ADDR_TABLE_CTL_STAG_MODE_BLEN          1
#define ADDR_TABLE_CTL_STAG_MODE_FLAG          HSL_RW

#define ARL_INI_EN                             "atbc_arlie"
#define ADDR_TABLE_CTL_ARL_INI_EN_BOFFSET      19
#define ADDR_TABLE_CTL_ARL_INI_EN_BLEN         1
#define ADDR_TABLE_CTL_ARL_INI_EN_FLAG         HSL_RW

#define LEARN_CHANGE_EN                        "atbc_lcen"
#define ADDR_TABLE_CTL_LEARN_CHANGE_EN_BOFFSET 18
#define ADDR_TABLE_CTL_LEARN_CHANGE_EN_BLEN    1
#define ADDR_TABLE_CTL_LEARN_CHANGE_EN_FLAG    HSL_RW

#define AGE_EN                                 "atbc_agee"
#define ADDR_TABLE_CTL_AGE_EN_BOFFSET          17
#define ADDR_TABLE_CTL_AGE_EN_BLEN             1
#define ADDR_TABLE_CTL_AGE_EN_FLAG             HSL_RW

#define AGE_TIME                               "atbc_aget"
#define ADDR_TABLE_CTL_AGE_TIME_BOFFSET        0
#define ADDR_TABLE_CTL_AGE_TIME_BLEN           16
#define ADDR_TABLE_CTL_AGE_TIME_FLAG           HSL_RW


    /* IP Priority Mapping Register */
#define IP_PRI_MAPPING            "imap"
#define IP_PRI_MAPPING_ID         15
#define IP_PRI_MAPPING_OFFSET     0x0060
#define IP_PRI_MAPPING_E_LENGTH   4
#define IP_PRI_MAPPING_E_OFFSET   0
#define IP_PRI_MAPPING_NR_E       1


    /* IP Priority Mapping Register */
#define IP_PRI_MAPPING0            "imap0"
#define IP_PRI_MAPPING0_ID         15
#define IP_PRI_MAPPING0_OFFSET     0x0060
#define IP_PRI_MAPPING0_E_LENGTH   4
#define IP_PRI_MAPPING0_E_OFFSET   0
#define IP_PRI_MAPPING0_NR_E       0

#define IP_0X3C                                "imap_ip3c"
#define IP_PRI_MAPPING0_IP_0X3C_BOFFSET        30
#define IP_PRI_MAPPING0_IP_0X3C_BLEN           2
#define IP_PRI_MAPPING0_IP_0X3C_FLAG           HSL_RW

#define IP_0X38                                "imap_ip38"
#define IP_PRI_MAPPING0_IP_0X38_BOFFSET        28
#define IP_PRI_MAPPING0_IP_0X38_BLEN           2
#define IP_PRI_MAPPING0_IP_0X38_FLAG           HSL_RW

#define IP_0X34                                "imap_ip34"
#define IP_PRI_MAPPING0_IP_0X34_BOFFSET        26
#define IP_PRI_MAPPING0_IP_0X34_BLEN           2
#define IP_PRI_MAPPING0_IP_0X34_FLAG           HSL_RW

#define IP_0X30                                "imap_ip30"
#define IP_PRI_MAPPING0_IP_0X30_BOFFSET        24
#define IP_PRI_MAPPING0_IP_0X30_BLEN           2
#define IP_PRI_MAPPING0_IP_0X30_FLAG           HSL_RW

#define IP_0X2C                                "imap_ip2c"
#define IP_PRI_MAPPING0_IP_0X2C_BOFFSET        22
#define IP_PRI_MAPPING0_IP_0X2C_BLEN           2
#define IP_PRI_MAPPING0_IP_0X2C_FLAG           HSL_RW

#define IP_0X28                                "imap_ip28"
#define IP_PRI_MAPPING0_IP_0X28_BOFFSET        20
#define IP_PRI_MAPPING0_IP_0X28_BLEN           2
#define IP_PRI_MAPPING0_IP_0X28_FLAG           HSL_RW

#define IP_0X24                                "imap_ip24"
#define IP_PRI_MAPPING0_IP_0X24_BOFFSET        18
#define IP_PRI_MAPPING0_IP_0X24_BLEN           2
#define IP_PRI_MAPPING0_IP_0X24_FLAG           HSL_RW

#define IP_0X20                                "imap_ip20"
#define IP_PRI_MAPPING0_IP_0X20_BOFFSET        16
#define IP_PRI_MAPPING0_IP_0X20_BLEN           2
#define IP_PRI_MAPPING0_IP_0X20_FLAG           HSL_RW

#define IP_0X1C                                "imap_ip1c"
#define IP_PRI_MAPPING0_IP_0X1C_BOFFSET        14
#define IP_PRI_MAPPING0_IP_0X1C_BLEN           2
#define IP_PRI_MAPPING0_IP_0X1C_FLAG           HSL_RW

#define IP_0X18                                "imap_ip18"
#define IP_PRI_MAPPING0_IP_0X18_BOFFSET        12
#define IP_PRI_MAPPING0_IP_0X18_BLEN           2
#define IP_PRI_MAPPING0_IP_0X18_FLAG           HSL_RW

#define IP_0X14                                "imap_ip14"
#define IP_PRI_MAPPING0_IP_0X14_BOFFSET        10
#define IP_PRI_MAPPING0_IP_0X14_BLEN           2
#define IP_PRI_MAPPING0_IP_0X14_FLAG           HSL_RW

#define IP_0X10                                "imap_ip10"
#define IP_PRI_MAPPING0_IP_0X10_BOFFSET        8
#define IP_PRI_MAPPING0_IP_0X10_BLEN           2
#define IP_PRI_MAPPING0_IP_0X10_FLAG           HSL_RW

#define IP_0X0C                                "imap_ip0c"
#define IP_PRI_MAPPING0_IP_0X0C_BOFFSET        6
#define IP_PRI_MAPPING0_IP_0X0C_BLEN           2
#define IP_PRI_MAPPING0_IP_0X0C_FLAG           HSL_RW

#define IP_0X08                                "imap_ip08"
#define IP_PRI_MAPPING0_IP_0X08_BOFFSET        4
#define IP_PRI_MAPPING0_IP_0X08_BLEN           2
#define IP_PRI_MAPPING0_IP_0X08_FLAG           HSL_RW

#define IP_0X04                                "imap_ip04"
#define IP_PRI_MAPPING0_IP_0X04_BOFFSET        2
#define IP_PRI_MAPPING0_IP_0X04_BLEN           2
#define IP_PRI_MAPPING0_IP_0X04_FLAG           HSL_RW

#define IP_0X00                                "imap_ip00"
#define IP_PRI_MAPPING0_IP_0X00_BOFFSET        0
#define IP_PRI_MAPPING0_IP_0X00_BLEN           2
#define IP_PRI_MAPPING0_IP_0X00_FLAG           HSL_RW

#define IP_PRI_MAPPING1            "imap1"
#define IP_PRI_MAPPING1_ID         16
#define IP_PRI_MAPPING1_OFFSET     0x0064
#define IP_PRI_MAPPING1_E_LENGTH   4
#define IP_PRI_MAPPING1_E_OFFSET   0
#define IP_PRI_MAPPING1_NR_E       0

#define IP_0X7C                                "imap_ip7c"
#define IP_PRI_MAPPING1_IP_0X7C_BOFFSET        30
#define IP_PRI_MAPPING1_IP_0X7C_BLEN           2
#define IP_PRI_MAPPING1_IP_0X7C_FLAG           HSL_RW

#define IP_0X78                                "imap_ip78"
#define IP_PRI_MAPPING1_IP_0X78_BOFFSET        28
#define IP_PRI_MAPPING1_IP_0X78_BLEN           2
#define IP_PRI_MAPPING1_IP_0X78_FLAG           HSL_RW

#define IP_0X74                                "imap_ip74"
#define IP_PRI_MAPPING1_IP_0X74_BOFFSET        26
#define IP_PRI_MAPPING1_IP_0X74_BLEN           2
#define IP_PRI_MAPPING1_IP_0X74_FLAG           HSL_RW

#define IP_0X70                                "imap_ip70"
#define IP_PRI_MAPPING1_IP_0X70_BOFFSET        24
#define IP_PRI_MAPPING1_IP_0X70_BLEN           2
#define IP_PRI_MAPPING1_IP_0X70_FLAG           HSL_RW

#define IP_0X6C                                "imap_ip6c"
#define IP_PRI_MAPPING1_IP_0X6C_BOFFSET        22
#define IP_PRI_MAPPING1_IP_0X6C_BLEN           2
#define IP_PRI_MAPPING1_IP_0X6C_FLAG           HSL_RW

#define IP_0X68                                "imap_ip68"
#define IP_PRI_MAPPING1_IP_0X68_BOFFSET        20
#define IP_PRI_MAPPING1_IP_0X68_BLEN           2
#define IP_PRI_MAPPING1_IP_0X68_FLAG           HSL_RW

#define IP_0X64                                "imap_ip64"
#define IP_PRI_MAPPING1_IP_0X64_BOFFSET        18
#define IP_PRI_MAPPING1_IP_0X64_BLEN           2
#define IP_PRI_MAPPING1_IP_0X64_FLAG           HSL_RW

#define IP_0X60                                "imap_ip60"
#define IP_PRI_MAPPING1_IP_0X60_BOFFSET        16
#define IP_PRI_MAPPING1_IP_0X60_BLEN           2
#define IP_PRI_MAPPING1_IP_0X60_FLAG           HSL_RW

#define IP_0X5C                                "imap_ip5c"
#define IP_PRI_MAPPING1_IP_0X5C_BOFFSET        14
#define IP_PRI_MAPPING1_IP_0X5C_BLEN           2
#define IP_PRI_MAPPING1_IP_0X5C_FLAG           HSL_RW

#define IP_0X58                                "imap_ip58"
#define IP_PRI_MAPPING1_IP_0X58_BOFFSET        12
#define IP_PRI_MAPPING1_IP_0X58_BLEN           2
#define IP_PRI_MAPPING1_IP_0X58_FLAG           HSL_RW

#define IP_0X54                                "imap_ip54"
#define IP_PRI_MAPPING1_IP_0X54_BOFFSET        10
#define IP_PRI_MAPPING1_IP_0X54_BLEN           2
#define IP_PRI_MAPPING1_IP_0X54_FLAG           HSL_RW

#define IP_0X50                                "imap_ip50"
#define IP_PRI_MAPPING1_IP_0X50_BOFFSET        8
#define IP_PRI_MAPPING1_IP_0X50_BLEN           2
#define IP_PRI_MAPPING1_IP_0X50_FLAG           HSL_RW

#define IP_0X4C                                "imap_ip4c"
#define IP_PRI_MAPPING1_IP_0X4C_BOFFSET        6
#define IP_PRI_MAPPING1_IP_0X4C_BLEN           2
#define IP_PRI_MAPPING1_IP_0X4C_FLAG           HSL_RW

#define IP_0X48                                "imap_ip48"
#define IP_PRI_MAPPING1_IP_0X48_BOFFSET        4
#define IP_PRI_MAPPING1_IP_0X48_BLEN           2
#define IP_PRI_MAPPING1_IP_0X48_FLAG           HSL_RW

#define IP_0X44                                "imap_ip44"
#define IP_PRI_MAPPING1_IP_0X44_BOFFSET        2
#define IP_PRI_MAPPING1_IP_0X44_BLEN           2
#define IP_PRI_MAPPING1_IP_0X44_FLAG           HSL_RW

#define IP_0X40                                "imap_ip40"
#define IP_PRI_MAPPING1_IP_0X40_BOFFSET        0
#define IP_PRI_MAPPING1_IP_0X40_BLEN           2
#define IP_PRI_MAPPING1_IP_0X40_FLAG           HSL_RW


#define IP_PRI_MAPPING2            "imap2"
#define IP_PRI_MAPPING2_ID         17
#define IP_PRI_MAPPING2_OFFSET     0x0068
#define IP_PRI_MAPPING2_E_LENGTH   4
#define IP_PRI_MAPPING2_E_OFFSET   0
#define IP_PRI_MAPPING2_NR_E       0

#define IP_0XBC                                "imap_ipbc"
#define IP_PRI_MAPPING2_IP_0XBC_BOFFSET        30
#define IP_PRI_MAPPING2_IP_0XBC_BLEN           2
#define IP_PRI_MAPPING2_IP_0XBC_FLAG           HSL_RW

#define IP_0XB8                                "imap_ipb8"
#define IP_PRI_MAPPING2_IP_0XB8_BOFFSET        28
#define IP_PRI_MAPPING2_IP_0XB8_BLEN           2
#define IP_PRI_MAPPING2_IP_0XB8_FLAG           HSL_RW

#define IP_0XB4                                "imap_ipb4"
#define IP_PRI_MAPPING2_IP_0XB4_BOFFSET        26
#define IP_PRI_MAPPING2_IP_0XB4_BLEN           2
#define IP_PRI_MAPPING2_IP_0XB4_FLAG           HSL_RW

#define IP_0XB0                                "imap_ipb0"
#define IP_PRI_MAPPING2_IP_0XB0_BOFFSET        24
#define IP_PRI_MAPPING2_IP_0XB0_BLEN           2
#define IP_PRI_MAPPING2_IP_0XB0_FLAG           HSL_RW

#define IP_0XAC                                "imap_ipac"
#define IP_PRI_MAPPING2_IP_0XAC_BOFFSET        22
#define IP_PRI_MAPPING2_IP_0XAC_BLEN           2
#define IP_PRI_MAPPING2_IP_0XAC_FLAG           HSL_RW

#define IP_0XA8                                "imap_ipa8"
#define IP_PRI_MAPPING2_IP_0XA8_BOFFSET        20
#define IP_PRI_MAPPING2_IP_0XA8_BLEN           2
#define IP_PRI_MAPPING2_IP_0XA8_FLAG           HSL_RW

#define IP_0XA4                                "imap_ipa4"
#define IP_PRI_MAPPING2_IP_0XA4_BOFFSET        18
#define IP_PRI_MAPPING2_IP_0XA4_BLEN           2
#define IP_PRI_MAPPING2_IP_0XA4_FLAG           HSL_RW

#define IP_0XA0                                "imap_ipa0"
#define IP_PRI_MAPPING2_IP_0XA0_BOFFSET        16
#define IP_PRI_MAPPING2_IP_0XA0_BLEN           2
#define IP_PRI_MAPPING2_IP_0XA0_FLAG           HSL_RW

#define IP_0X9C                                "imap_ip9c"
#define IP_PRI_MAPPING2_IP_0X9C_BOFFSET        14
#define IP_PRI_MAPPING2_IP_0X9C_BLEN           2
#define IP_PRI_MAPPING2_IP_0X9C_FLAG           HSL_RW

#define IP_0X98                                "imap_ip98"
#define IP_PRI_MAPPING2_IP_0X98_BOFFSET        12
#define IP_PRI_MAPPING2_IP_0X98_BLEN           2
#define IP_PRI_MAPPING2_IP_0X98_FLAG           HSL_RW

#define IP_0X94                                "imap_ip94"
#define IP_PRI_MAPPING2_IP_0X94_BOFFSET        10
#define IP_PRI_MAPPING2_IP_0X94_BLEN           2
#define IP_PRI_MAPPING2_IP_0X94_FLAG           HSL_RW

#define IP_0X90                                "imap_ip90"
#define IP_PRI_MAPPING2_IP_0X90_BOFFSET        8
#define IP_PRI_MAPPING2_IP_0X90_BLEN           2
#define IP_PRI_MAPPING2_IP_0X90_FLAG           HSL_RW

#define IP_0X8C                                "imap_ip8c"
#define IP_PRI_MAPPING2_IP_0X8C_BOFFSET        6
#define IP_PRI_MAPPING2_IP_0X8C_BLEN           2
#define IP_PRI_MAPPING2_IP_0X8C_FLAG           HSL_RW

#define IP_0X88                                "imap_ip88"
#define IP_PRI_MAPPING2_IP_0X88_BOFFSET        4
#define IP_PRI_MAPPING2_IP_0X88_BLEN           2
#define IP_PRI_MAPPING2_IP_0X88_FLAG           HSL_RW

#define IP_0X84                                "imap_ip84"
#define IP_PRI_MAPPING2_IP_0X84_BOFFSET        2
#define IP_PRI_MAPPING2_IP_0X84_BLEN           2
#define IP_PRI_MAPPING2_IP_0X84_FLAG           HSL_RW

#define IP_0X80                                "imap_ip80"
#define IP_PRI_MAPPING2_IP_0X80_BOFFSET        0
#define IP_PRI_MAPPING2_IP_0X80_BLEN           2
#define IP_PRI_MAPPING2_IP_0X80_FLAG           HSL_RW

#define IP_PRI_MAPPING3            "imap3"
#define IP_PRI_MAPPING3_ID         18
#define IP_PRI_MAPPING3_OFFSET     0x006C
#define IP_PRI_MAPPING3_E_LENGTH   4
#define IP_PRI_MAPPING3_E_OFFSET   0
#define IP_PRI_MAPPING3_NR_E       0

#define IP_0XFC                                "imap_ipfc"
#define IP_PRI_MAPPING3_IP_0XFC_BOFFSET        30
#define IP_PRI_MAPPING3_IP_0XFC_BLEN           2
#define IP_PRI_MAPPING3_IP_0XFC_FLAG           HSL_RW

#define IP_0XF8                                "imap_ipf8"
#define IP_PRI_MAPPING3_IP_0XF8_BOFFSET        28
#define IP_PRI_MAPPING3_IP_0XF8_BLEN           2
#define IP_PRI_MAPPING3_IP_0XF8_FLAG           HSL_RW

#define IP_0XF4                                "imap_ipf4"
#define IP_PRI_MAPPING3_IP_0XF4_BOFFSET        26
#define IP_PRI_MAPPING3_IP_0XF4_BLEN           2
#define IP_PRI_MAPPING3_IP_0XF4_FLAG           HSL_RW

#define IP_0XF0                                "imap_ipf0"
#define IP_PRI_MAPPING3_IP_0XF0_BOFFSET        24
#define IP_PRI_MAPPING3_IP_0XF0_BLEN           2
#define IP_PRI_MAPPING3_IP_0XF0_FLAG           HSL_RW

#define IP_0XEC                                "imap_ipec"
#define IP_PRI_MAPPING3_IP_0XEC_BOFFSET        22
#define IP_PRI_MAPPING3_IP_0XEC_BLEN           2
#define IP_PRI_MAPPING3_IP_0XEC_FLAG           HSL_RW

#define IP_0XE8                                "imap_ipe8"
#define IP_PRI_MAPPING3_IP_0XE8_BOFFSET        20
#define IP_PRI_MAPPING3_IP_0XE8_BLEN           2
#define IP_PRI_MAPPING3_IP_0XE8_FLAG           HSL_RW

#define IP_0XE4                                "imap_ipe4"
#define IP_PRI_MAPPING3_IP_0XE4_BOFFSET        18
#define IP_PRI_MAPPING3_IP_0XE4_BLEN           2
#define IP_PRI_MAPPING3_IP_0XE4_FLAG           HSL_RW

#define IP_0XE0                                "imap_ipe0"
#define IP_PRI_MAPPING3_IP_0XE0_BOFFSET        16
#define IP_PRI_MAPPING3_IP_0XE0_BLEN           2
#define IP_PRI_MAPPING3_IP_0XE0_FLAG           HSL_RW

#define IP_0XDC                                "imap_ipdc"
#define IP_PRI_MAPPING3_IP_0XDC_BOFFSET        14
#define IP_PRI_MAPPING3_IP_0XDC_BLEN           2
#define IP_PRI_MAPPING3_IP_0XDC_FLAG           HSL_RW

#define IP_0XD8                                "imap_ipd8"
#define IP_PRI_MAPPING3_IP_0XD8_BOFFSET        12
#define IP_PRI_MAPPING3_IP_0XD8_BLEN           2
#define IP_PRI_MAPPING3_IP_0XD8_FLAG           HSL_RW

#define IP_0XD4                                "imap_ipd4"
#define IP_PRI_MAPPING3_IP_0XD4_BOFFSET        10
#define IP_PRI_MAPPING3_IP_0XD4_BLEN           2
#define IP_PRI_MAPPING3_IP_0XD4_FLAG           HSL_RW

#define IP_0XD0                                "imap_ipd0"
#define IP_PRI_MAPPING3_IP_0XD0_BOFFSET        8
#define IP_PRI_MAPPING3_IP_0XD0_BLEN           2
#define IP_PRI_MAPPING3_IP_0XD0_FLAG           HSL_RW

#define IP_0XCC                                "imap_ipcc"
#define IP_PRI_MAPPING3_IP_0XCC_BOFFSET        6
#define IP_PRI_MAPPING3_IP_0XCC_BLEN           2
#define IP_PRI_MAPPING3_IP_0XCC_FLAG           HSL_RW

#define IP_0XC8                                "imap_ipc8"
#define IP_PRI_MAPPING3_IP_0XC8_BOFFSET        4
#define IP_PRI_MAPPING3_IP_0XC8_BLEN           2
#define IP_PRI_MAPPING3_IP_0XC8_FLAG           HSL_RW

#define IP_0XC4                                "imap_ipc4"
#define IP_PRI_MAPPING3_IP_0XC4_BOFFSET        2
#define IP_PRI_MAPPING3_IP_0XC4_BLEN           2
#define IP_PRI_MAPPING3_IP_0XC4_FLAG           HSL_RW

#define IP_0XC0                                "imap_ipc0"
#define IP_PRI_MAPPING3_IP_0XC0_BOFFSET        0
#define IP_PRI_MAPPING3_IP_0XC0_BLEN           2
#define IP_PRI_MAPPING3_IP_0XC0_FLAG           HSL_RW


    /* Tag Priority Mapping Register */
#define TAG_PRI_MAPPING           "tpmap"
#define TAG_PRI_MAPPING_ID        19
#define TAG_PRI_MAPPING_OFFSET    0x0070
#define TAG_PRI_MAPPING_E_LENGTH  4
#define TAG_PRI_MAPPING_E_OFFSET  0
#define TAG_PRI_MAPPING_NR_E      1

#define TAG_0X07                                "tpmap_tg07"
#define TAG_PRI_MAPPING_TAG_0X07_BOFFSET      14
#define TAG_PRI_MAPPING_TAG_0X07_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X07_FLAG         HSL_RW

#define TAG_0X06                              "tpmap_tg06"
#define TAG_PRI_MAPPING_TAG_0X06_BOFFSET      12
#define TAG_PRI_MAPPING_TAG_0X06_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X06_FLAG         HSL_RW

#define TAG_0X05                              "tpmap_tg05"
#define TAG_PRI_MAPPING_TAG_0X05_BOFFSET      10
#define TAG_PRI_MAPPING_TAG_0X05_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X05_FLAG         HSL_RW

#define TAG_0X04                              "tpmap_tg04"
#define TAG_PRI_MAPPING_TAG_0X04_BOFFSET      8
#define TAG_PRI_MAPPING_TAG_0X04_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X04_FLAG         HSL_RW

#define TAG_0X03                              "tpmap_tg03"
#define TAG_PRI_MAPPING_TAG_0X03_BOFFSET      6
#define TAG_PRI_MAPPING_TAG_0X03_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X03_FLAG         HSL_RW

#define TAG_0X02                              "tpmap_tg02"
#define TAG_PRI_MAPPING_TAG_0X02_BOFFSET      4
#define TAG_PRI_MAPPING_TAG_0X02_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X02_FLAG         HSL_RW

#define TAG_0X01                              "tpmap_tg01"
#define TAG_PRI_MAPPING_TAG_0X01_BOFFSET      2
#define TAG_PRI_MAPPING_TAG_0X01_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X01_FLAG         HSL_RW

#define TAG_0X00                              "tpmap_tg00"
#define TAG_PRI_MAPPING_TAG_0X00_BOFFSET      0
#define TAG_PRI_MAPPING_TAG_0X00_BLEN         2
#define TAG_PRI_MAPPING_TAG_0X00_FLAG         HSL_RW


    /* Service tag Register */
#define SERVICE_TAG                  "servicetag"
#define SERVICE_TAG_ID               20
#define SERVICE_TAG_OFFSET           0x0074
#define SERVICE_TAG_E_LENGTH         4
#define SERVICE_TAG_E_OFFSET         0
#define SERVICE_TAG_NR_E             1

#define TAG_VALUE                        "servicetag_val"
#define SERVICE_TAG_TAG_VALUE_BOFFSET    0
#define SERVICE_TAG_TAG_VALUE_BLEN       16
#define SERVICE_TAG_TAG_VALUE_FLAG       HSL_RW


    /* Cpu Port Register */
#define CPU_PORT                  "cpup"
#define CPU_PORT_ID               20
#define CPU_PORT_OFFSET           0x0078
#define CPU_PORT_E_LENGTH         4
#define CPU_PORT_E_OFFSET         0
#define CPU_PORT_NR_E             0

#define CPU_PORT_EN                           "cpup_cpupe"
#define CPU_PORT_CPU_PORT_EN_BOFFSET          8
#define CPU_PORT_CPU_PORT_EN_BLEN             1
#define CPU_PORT_CPU_PORT_EN_FLAG             HSL_RW

#define MIRROR_PORT_NUM                       "cpup_mirpn"
#define CPU_PORT_MIRROR_PORT_NUM_BOFFSET      4
#define CPU_PORT_MIRROR_PORT_NUM_BLEN         4
#define CPU_PORT_MIRROR_PORT_NUM_FLAG         HSL_RW


    /* MIB Function Register */
#define MIB_FUNC                  "mibfunc"
#define MIB_FUNC_ID               21
#define MIB_FUNC_OFFSET           0x0080
#define MIB_FUNC_E_LENGTH         4
#define MIB_FUNC_E_OFFSET         0
#define MIB_FUNC_NR_E             1

#define MAC_CRC_EN                            "mibfunc_crcen"
#define MIB_FUNC_MAC_CRC_EN_BOFFSET            31
#define MIB_FUNC_MAC_CRC_EN_BLEN               1
#define MIB_FUNC_MAC_CRC_EN_FLAG               HSL_RW

#define MIB_EN                               "mib_en"
#define MIB_FUNC_MIB_EN_BOFFSET              30
#define MIB_FUNC_MIB_EN_BLEN                 1
#define MIB_FUNC_MIB_EN_FLAG                 HSL_RW

#define MIB_FUN                              "mibfunc_mibf"
#define MIB_FUNC_MIB_FUN_BOFFSET              24
#define MIB_FUNC_MIB_FUN_BLEN                 3
#define MIB_FUNC_MIB_FUN_FLAG                 HSL_RW

#define MIB_BUSY                              "mibfunc_mibb"
#define MIB_FUNC_MIB_BUSY_BOFFSET              17
#define MIB_FUNC_MIB_BUSY_BLEN                 1
#define MIB_FUNC_MIB_BUSY_FLAG                 HSL_RW

#define MIB_AT_HALF_EN                        "mibfunc_mibhe"
#define MIB_FUNC_MIB_AT_HALF_EN_BOFFSET        16
#define MIB_FUNC_MIB_AT_HALF_EN_BLEN           1
#define MIB_FUNC_MIB_AT_HALF_EN_FLAG           HSL_RW

#define MIB_TIMER                             "mibfunc_mibt"
#define MIB_FUNC_MIB_TIMER_BOFFSET             0
#define MIB_FUNC_MIB_TIMER_BLEN                16
#define MIB_FUNC_MIB_TIMER_FLAG                HSL_RW


    /* Mdio control Register */
#define MDIO_CTRL               "mctrl"
#define MDIO_CTRL_ID            24
#define MDIO_CTRL_OFFSET        0x0098
#define MDIO_CTRL_E_LENGTH      4
#define MDIO_CTRL_E_OFFSET      0
#define MDIO_CTRL_NR_E          1

#define MSTER_EN                        "mctrl_msteren"
#define MDIO_CTRL_MSTER_EN_BOFFSET      30
#define MDIO_CTRL_MSTER_EN_BLEN         1
#define MDIO_CTRL_MSTER_EN_FLAG         HSL_RW

#define MSTER_EN                        "mctrl_msteren"
#define MDIO_CTRL_MSTER_EN_BOFFSET      30
#define MDIO_CTRL_MSTER_EN_BLEN         1
#define MDIO_CTRL_MSTER_EN_FLAG         HSL_RW

#define CMD                             "mctrl_cmd"
#define MDIO_CTRL_CMD_BOFFSET           27
#define MDIO_CTRL_CMD_BLEN              1
#define MDIO_CTRL_CMD_FLAG              HSL_RW

#define SUP_PRE                         "mctrl_spre"
#define MDIO_CTRL_SUP_PRE_BOFFSET       26
#define MDIO_CTRL_SUP_PRE_BLEN          1
#define MDIO_CTRL_SUP_PRE_FLAG          HSL_RW

#define PHY_ADDR                        "mctrl_phyaddr"
#define MDIO_CTRL_PHY_ADDR_BOFFSET      21
#define MDIO_CTRL_PHY_ADDR_BLEN         5
#define MDIO_CTRL_PHY_ADDR_FLAG         HSL_RW

#define REG_ADDR                        "mctrl_regaddr"
#define MDIO_CTRL_REG_ADDR_BOFFSET      16
#define MDIO_CTRL_REG_ADDR_BLEN         5
#define MDIO_CTRL_REG_ADDR_FLAG         HSL_RW

#define DATA                            "mctrl_data"
#define MDIO_CTRL_DATA_BOFFSET          0
#define MDIO_CTRL_DATA_BLEN             16
#define MDIO_CTRL_DATA_FLAG             HSL_RW




    /* BIST control Register */
#define BIST_CTRL               "bctrl"
#define BIST_CTRL_ID            24
#define BIST_CTRL_OFFSET        0x00a0
#define BIST_CTRL_E_LENGTH      4
#define BIST_CTRL_E_OFFSET      0
#define BIST_CTRL_NR_E          1

#define BIST_BUSY                        "bctrl_bb"
#define BIST_CTRL_BIST_BUSY_BOFFSET      31
#define BIST_CTRL_BIST_BUSY_BLEN         1
#define BIST_CTRL_BIST_BUSY_FLAG         HSL_RW

#define ONE_ERR                          "bctrl_oe"
#define BIST_CTRL_ONE_ERR_BOFFSET        30
#define BIST_CTRL_ONE_ERR_BLEN           1
#define BIST_CTRL_ONE_ERR_FLAG           HSL_RO

#define ERR_MEM                          "bctrl_em"
#define BIST_CTRL_ERR_MEM_BOFFSET        24
#define BIST_CTRL_ERR_MEM_BLEN           4
#define BIST_CTRL_ERR_MEM_FLAG           HSL_RO

#define PTN_EN2                          "bctrl_pe2"
#define BIST_CTRL_PTN_EN2_BOFFSET        22
#define BIST_CTRL_PTN_EN2_BLEN           1
#define BIST_CTRL_PTN_EN2_FLAG           HSL_RW

#define PTN_EN1                          "bctrl_pe1"
#define BIST_CTRL_PTN_EN1_BOFFSET        21
#define BIST_CTRL_PTN_EN1_BLEN           1
#define BIST_CTRL_PTN_EN1_FLAG           HSL_RW

#define PTN_EN0                          "bctrl_pe0"
#define BIST_CTRL_PTN_EN0_BOFFSET        20
#define BIST_CTRL_PTN_EN0_BLEN           1
#define BIST_CTRL_PTN_EN0_FLAG           HSL_RW

#define ERR_PTN                          "bctrl_ep"
#define BIST_CTRL_ERR_PTN_BOFFSET        16
#define BIST_CTRL_ERR_PTN_BLEN           2
#define BIST_CTRL_ERR_PTN_FLAG           HSL_RO

#define ERR_CNT                          "bctrl_ec"
#define BIST_CTRL_ERR_CNT_BOFFSET        13
#define BIST_CTRL_ERR_CNT_BLEN           2
#define BIST_CTRL_ERR_CNT_FLAG           HSL_RO

#define ERR_ADDR                         "bctrl_ea"
#define BIST_CTRL_ERR_ADDR_BOFFSET       0
#define BIST_CTRL_ERR_ADDR_BLEN          12
#define BIST_CTRL_ERR_ADDR_FLAG          HSL_RO




    /* BIST recover Register */
#define BIST_RCV               "brcv"
#define BIST_RCV_ID            24
#define BIST_RCV_OFFSET        0x00a4
#define BIST_RCV_E_LENGTH      4
#define BIST_RCV_E_OFFSET      0
#define BIST_RCV_NR_E          1

#define RCV_EN                           "brcv_en"
#define BIST_RCV_RCV_EN_BOFFSET          31
#define BIST_RCV_RCV_EN_BLEN             1
#define BIST_RCV_RCV_EN_FLAG             HSL_RW

#define RCV_ADDR                         "brcv_addr"
#define BIST_RCV_RCV_ADDR_BOFFSET        0
#define BIST_RCV_RCV_ADDR_BLEN           12
#define BIST_RCV_RCV_ADDR_FLAG           HSL_RW




    /* LED control Register */
#define LED_CTRL               "ledctrl"
#define LED_CTRL_ID            25
#define LED_CTRL_OFFSET        0x00b0
#define LED_CTRL_E_LENGTH      4
#define LED_CTRL_E_OFFSET      0
#define LED_CTRL_NR_E          1

#define PATTERN_EN                           "lctrl_pen"
#define LED_CTRL_PATTERN_EN_BOFFSET          14
#define LED_CTRL_PATTERN_EN_BLEN             2
#define LED_CTRL_PATTERN_EN_FLAG             HSL_RW

#define FULL_LIGHT_EN                        "lctrl_fen"
#define LED_CTRL_FULL_LIGHT_EN_BOFFSET       13
#define LED_CTRL_FULL_LIGHT_EN_BLEN          1
#define LED_CTRL_FULL_LIGHT_EN_FLAG          HSL_RW

#define HALF_LIGHT_EN                        "lctrl_hen"
#define LED_CTRL_HALF_LIGHT_EN_BOFFSET       12
#define LED_CTRL_HALF_LIGHT_EN_BLEN          1
#define LED_CTRL_HALF_LIGHT_EN_FLAG          HSL_RW

#define POWERON_LIGHT_EN                     "lctrl_poen"
#define LED_CTRL_POWERON_LIGHT_EN_BOFFSET    11
#define LED_CTRL_POWERON_LIGHT_EN_BLEN       1
#define LED_CTRL_POWERON_LIGHT_EN_FLAG       HSL_RW

#define GE_LIGHT_EN                          "lctrl_geen"
#define LED_CTRL_GE_LIGHT_EN_BOFFSET         10
#define LED_CTRL_GE_LIGHT_EN_BLEN            1
#define LED_CTRL_GE_LIGHT_EN_FLAG            HSL_RW

#define FE_LIGHT_EN                          "lctrl_feen"
#define LED_CTRL_FE_LIGHT_EN_BOFFSET         9
#define LED_CTRL_FE_LIGHT_EN_BLEN            1
#define LED_CTRL_FE_LIGHT_EN_FLAG            HSL_RW

#define ETH_LIGHT_EN                         "lctrl_ethen"
#define LED_CTRL_ETH_LIGHT_EN_BOFFSET        8
#define LED_CTRL_ETH_LIGHT_EN_BLEN           1
#define LED_CTRL_ETH_LIGHT_EN_FLAG           HSL_RW

#define COL_BLINK_EN                         "lctrl_cen"
#define LED_CTRL_COL_BLINK_EN_BOFFSET        7
#define LED_CTRL_COL_BLINK_EN_BLEN           1
#define LED_CTRL_COL_BLINK_EN_FLAG           HSL_RW

#define RX_BLINK_EN                          "lctrl_rxen"
#define LED_CTRL_RX_BLINK_EN_BOFFSET         5
#define LED_CTRL_RX_BLINK_EN_BLEN            1
#define LED_CTRL_RX_BLINK_EN_FLAG            HSL_RW

#define TX_BLINK_EN                          "lctrl_txen"
#define LED_CTRL_TX_BLINK_EN_BOFFSET         4
#define LED_CTRL_TX_BLINK_EN_BLEN            1
#define LED_CTRL_TX_BLINK_EN_FLAG            HSL_RW

#define LINKUP_OVER_EN                       "lctrl_loen"
#define LED_CTRL_LINKUP_OVER_EN_BOFFSET      2
#define LED_CTRL_LINKUP_OVER_EN_BLEN         1
#define LED_CTRL_LINKUP_OVER_EN_FLAG         HSL_RW

#define BLINK_FREQ                           "lctrl_bfreq"
#define LED_CTRL_BLINK_FREQ_BOFFSET          0
#define LED_CTRL_BLINK_FREQ_BLEN             2
#define LED_CTRL_BLINK_FREQ_FLAG             HSL_RW

    /* LED control Register */
#define LED_PATTERN               "ledpatten"
#define LED_PATTERN_ID            25
#define LED_PATTERN_OFFSET        0x00bc
#define LED_PATTERN_E_LENGTH      4
#define LED_PATTERN_E_OFFSET      0
#define LED_PATTERN_NR_E          1

#define P3L1_MODE                           "p3l1_mode"
#define LED_PATTERN_P3L1_MODE_BOFFSET       24
#define LED_PATTERN_P3L1_MODE_BLEN          2
#define LED_PATTERN_P3L1_MODE_FLAG          HSL_RW

#define P3L0_MODE                           "p3l0_mode"
#define LED_PATTERN_P3L0_MODE_BOFFSET       22
#define LED_PATTERN_P3L0_MODE_BLEN          2
#define LED_PATTERN_P3L0_MODE_FLAG          HSL_RW

#define P2L1_MODE                           "p2l1_mode"
#define LED_PATTERN_P2L1_MODE_BOFFSET       20
#define LED_PATTERN_P2L1_MODE_BLEN          2
#define LED_PATTERN_P2L1_MODE_FLAG          HSL_RW

#define P2L0_MODE                           "p2l0_mode"
#define LED_PATTERN_P2L0_MODE_BOFFSET       18
#define LED_PATTERN_P2L0_MODE_BLEN          2
#define LED_PATTERN_P2L0_MODE_FLAG          HSL_RW

#define P1L1_MODE                           "p1l1_mode"
#define LED_PATTERN_P1L1_MODE_BOFFSET       16
#define LED_PATTERN_P1L1_MODE_BLEN          2
#define LED_PATTERN_P1L1_MODE_FLAG          HSL_RW

#define P1L0_MODE                           "p1l0_mode"
#define LED_PATTERN_P1L0_MODE_BOFFSET       14
#define LED_PATTERN_P1L0_MODE_BLEN          2
#define LED_PATTERN_P1L0_MODE_FLAG          HSL_RW

#define M5_MODE                             "m5_mode"
#define LED_PATTERN_M5_MODE_BOFFSET         10
#define LED_PATTERN_M5_MODE_BLEN            2
#define LED_PATTERN_M5_MODE_FLAG            HSL_RW


    /* Port Status Register */
#define PORT_STATUS               "ptsts"
#define PORT_STATUS_ID            29
#define PORT_STATUS_OFFSET        0x0100
#define PORT_STATUS_E_LENGTH      4
#define PORT_STATUS_E_OFFSET      0x0100
#define PORT_STATUS_NR_E          6

#define FLOW_LINK_EN                            "ptsts_flen"
#define PORT_STATUS_FLOW_LINK_EN_BOFFSET        12
#define PORT_STATUS_FLOW_LINK_EN_BLEN           1
#define PORT_STATUS_FLOW_LINK_EN_FLAG           HSL_RW


#define LINK_ASYN_PAUSE                         "ptsts_lasynp"
#define PORT_STATUS_LINK_ASYN_PAUSE_BOFFSET     11
#define PORT_STATUS_LINK_ASYN_PAUSE_BLEN        1
#define PORT_STATUS_LINK_ASYN_PAUSE_FLAG        HSL_RO

#define LINK_PAUSE                              "ptsts_lpause"
#define PORT_STATUS_LINK_PAUSE_BOFFSET          10
#define PORT_STATUS_LINK_PAUSE_BLEN             1
#define PORT_STATUS_LINK_PAUSE_FLAG             HSL_RO

#define LINK_EN                                 "ptsts_linken"
#define PORT_STATUS_LINK_EN_BOFFSET             9
#define PORT_STATUS_LINK_EN_BLEN                1
#define PORT_STATUS_LINK_EN_FLAG                HSL_RW

#define LINK                                    "ptsts_ptlink"
#define PORT_STATUS_LINK_BOFFSET                8
#define PORT_STATUS_LINK_BLEN                   1
#define PORT_STATUS_LINK_FLAG                   HSL_RO

#define TX_HALF_FLOW_EN
#define PORT_STATUS_TX_HALF_FLOW_EN_BOFFSET     7
#define PORT_STATUS_TX_HALF_FLOW_EN_BLEN        1
#define PORT_STATUS_TX_HALF_FLOW_EN_FLAG        HSL_RW

#define DUPLEX_MODE                             "ptsts_dupmod"
#define PORT_STATUS_DUPLEX_MODE_BOFFSET         6
#define PORT_STATUS_DUPLEX_MODE_BLEN            1
#define PORT_STATUS_DUPLEX_MODE_FLAG            HSL_RW

#define RX_FLOW_EN                              "ptsts_rxfwen"
#define PORT_STATUS_RX_FLOW_EN_BOFFSET          5
#define PORT_STATUS_RX_FLOW_EN_BLEN             1
#define PORT_STATUS_RX_FLOW_EN_FLAG             HSL_RW

#define TX_FLOW_EN                              "ptsts_txfwen"
#define PORT_STATUS_TX_FLOW_EN_BOFFSET          4
#define PORT_STATUS_TX_FLOW_EN_BLEN             1
#define PORT_STATUS_TX_FLOW_EN_FLAG             HSL_RW

#define RXMAC_EN                                "ptsts_rxmacen"
#define PORT_STATUS_RXMAC_EN_BOFFSET            3
#define PORT_STATUS_RXMAC_EN_BLEN               1
#define PORT_STATUS_RXMAC_EN_FLAG               HSL_RW

#define TXMAC_EN                                "ptsts_txmacen"
#define PORT_STATUS_TXMAC_EN_BOFFSET            2
#define PORT_STATUS_TXMAC_EN_BLEN               1
#define PORT_STATUS_TXMAC_EN_FLAG               HSL_RW

#define SPEED_MODE                              "ptsts_speed"
#define PORT_STATUS_SPEED_MODE_BOFFSET          0
#define PORT_STATUS_SPEED_MODE_BLEN             2
#define PORT_STATUS_SPEED_MODE_FLAG             HSL_RW


    /* Port Control Register */
#define PORT_CTL                  "pctl"
#define PORT_CTL_ID               30
#define PORT_CTL_OFFSET           0x0104
#define PORT_CTL_E_LENGTH         4
#define PORT_CTL_E_OFFSET         0x0100
#define PORT_CTL_NR_E             6

#define EAPOL_EN                                "pctl_eapolen"
#define PORT_CTL_EAPOL_EN_BOFFSET               23
#define PORT_CTL_EAPOL_EN_BLEN                  1
#define PORT_CTL_EAPOL_EN_FLAG                  HSL_RW

#define ARP_LEAKY_EN                            "pbvlan_alen"
#define PORT_CTL_ARP_LEAKY_EN_BOFFSET           22
#define PORT_CTL_ARP_LEAKY_EN_BLEN              1
#define PORT_CTL_ARP_LEAKY_EN_FLAG              HSL_RW

#define LEAVE_EN                                "pctl_leaveen"
#define PORT_CTL_LEAVE_EN_BOFFSET               21
#define PORT_CTL_LEAVE_EN_BLEN                  1
#define PORT_CTL_LEAVE_EN_FLAG                  HSL_RW

#define JOIN_EN                                 "pctl_joinen"
#define PORT_CTL_JOIN_EN_BOFFSET                20
#define PORT_CTL_JOIN_EN_BLEN                   1
#define PORT_CTL_JOIN_EN_FLAG                   HSL_RW

#define DHCP_EN                                 "pctl_dhcpen"
#define PORT_CTL_DHCP_EN_BOFFSET                19
#define PORT_CTL_DHCP_EN_BLEN                   1
#define PORT_CTL_DHCP_EN_FLAG                   HSL_RW

#define ING_MIRROR_EN                           "pctl_ingmiren"
#define PORT_CTL_ING_MIRROR_EN_BOFFSET          17
#define PORT_CTL_ING_MIRROR_EN_BLEN             1
#define PORT_CTL_ING_MIRROR_EN_FLAG             HSL_RW

#define EG_MIRROR_EN                            "pctl_egmiren"
#define PORT_CTL_EG_MIRROR_EN_BOFFSET           16
#define PORT_CTL_EG_MIRROR_EN_BLEN              1
#define PORT_CTL_EG_MIRROR_EN_FLAG              HSL_RW

#define LEARN_EN                                "pctl_learnen"
#define PORT_CTL_LEARN_EN_BOFFSET               14
#define PORT_CTL_LEARN_EN_BLEN                  1
#define PORT_CTL_LEARN_EN_FLAG                  HSL_RW

#define MAC_LOOP_BACK                           "pctl_maclp"
#define PORT_CTL_MAC_LOOP_BACK_BOFFSET          12
#define PORT_CTL_MAC_LOOP_BACK_BLEN             1
#define PORT_CTL_MAC_LOOP_BACK_FLAG             HSL_RW

#define HEAD_EN                                 "pctl_headen"
#define PORT_CTL_HEAD_EN_BOFFSET                11
#define PORT_CTL_HEAD_EN_BLEN                   1
#define PORT_CTL_HEAD_EN_FLAG                   HSL_RW

#define IGMP_MLD_EN                             "pctl_imlden"
#define PORT_CTL_IGMP_MLD_EN_BOFFSET            10
#define PORT_CTL_IGMP_MLD_EN_BLEN               1
#define PORT_CTL_IGMP_MLD_EN_FLAG               HSL_RW

#define EG_VLAN_MODE                            "pctl_egvmode"
#define PORT_CTL_EG_VLAN_MODE_BOFFSET           8
#define PORT_CTL_EG_VLAN_MODE_BLEN              2
#define PORT_CTL_EG_VLAN_MODE_FLAG              HSL_RW

#define LEARN_ONE_LOCK                          "pctl_lonelck"
#define PORT_CTL_LEARN_ONE_LOCK_BOFFSET         7
#define PORT_CTL_LEARN_ONE_LOCK_BLEN            1
#define PORT_CTL_LEARN_ONE_LOCK_FLAG            HSL_RW

#define PORT_LOCK_EN                            "pctl_locken"
#define PORT_CTL_PORT_LOCK_EN_BOFFSET           6
#define PORT_CTL_PORT_LOCK_EN_BLEN              1
#define PORT_CTL_PORT_LOCK_EN_FLAG              HSL_RW

#define LOCK_DROP_EN                            "pctl_dropen"
#define PORT_CTL_LOCK_DROP_EN_BOFFSET           5
#define PORT_CTL_LOCK_DROP_EN_BLEN              1
#define PORT_CTL_LOCK_DROP_EN_FLAG              HSL_RW

#define PORT_STATE                              "pctl_pstate"
#define PORT_CTL_PORT_STATE_BOFFSET             0
#define PORT_CTL_PORT_STATE_BLEN                3
#define PORT_CTL_PORT_STATE_FLAG                HSL_RW


    /* Port dot1q Register */
#define PORT_DOT1Q            "pdot1Q"
#define PORT_DOT1Q_ID         31
#define PORT_DOT1Q_OFFSET     0x0108
#define PORT_DOT1Q_E_LENGTH   4
#define PORT_DOT1Q_E_OFFSET   0x0100
#define PORT_DOT1Q_NR_E       6

#define ING_PRI                              "pdot1q_ingpri"
#define PORT_DOT1Q_ING_PRI_BOFFSET           29
#define PORT_DOT1Q_ING_PRI_BLEN              3
#define PORT_DOT1Q_ING_PRI_FLAG              HSL_RW

#define FORCE_PVLAN                          "pdot1q_fpvlan"
#define PORT_DOT1Q_FORCE_PVLAN_BOFFSET       28
#define PORT_DOT1Q_FORCE_PVLAN_BLEN          1
#define PORT_DOT1Q_FORCE_PVLAN_FLAG          HSL_RW

#define DEF_VID                              "pdot1q_dcvid"
#define PORT_DOT1Q_DEF_VID_BOFFSET           16
#define PORT_DOT1Q_DEF_VID_BLEN              12
#define PORT_DOT1Q_DEF_VID_FLAG              HSL_RW

#define FORCE_DEF_VID                        "pbot1q_fdvid"
#define PORT_DOT1Q_FORCE_DEF_VID_BOFFSET     12
#define PORT_DOT1Q_FORCE_DEF_VID_BLEN        1
#define PORT_DOT1Q_FORCE_DEF_VID_FLAG        HSL_RW


    /* Port Based Vlan Register */
#define PORT_BASE_VLAN            "pbvlan"
#define PORT_BASE_VLAN_ID         31
#define PORT_BASE_VLAN_OFFSET     0x010c
#define PORT_BASE_VLAN_E_LENGTH   4
#define PORT_BASE_VLAN_E_OFFSET   0x0100
#define PORT_BASE_VLAN_NR_E       6

#define DOT1Q_MODE                              "pbvlan_8021q"
#define PORT_BASE_VLAN_DOT1Q_MODE_BOFFSET       30
#define PORT_BASE_VLAN_DOT1Q_MODE_BLEN          2
#define PORT_BASE_VLAN_DOT1Q_MODE_FLAG          HSL_RW

#define COREP_EN                                "pbvlan_corepen"
#define PORT_BASE_VLAN_COREP_EN_BOFFSET         29
#define PORT_BASE_VLAN_COREP_EN_BLEN            1
#define PORT_BASE_VLAN_COREP_EN_FLAG            HSL_RW

#define IN_VLAN_MODE                            "pbvlan_imode"
#define PORT_BASE_VLAN_IN_VLAN_MODE_BOFFSET     27
#define PORT_BASE_VLAN_IN_VLAN_MODE_BLEN        2
#define PORT_BASE_VLAN_IN_VLAN_MODE_FLAG        HSL_RW

#define PRI_PROPAGATION                         "pbvlan_prip"
#define PORT_BASE_VLAN_PRI_PROPAGATION_BOFFSET  23
#define PORT_BASE_VLAN_PRI_PROPAGATION_BLEN     1
#define PORT_BASE_VLAN_PRI_PROPAGATION_FLAG     HSL_RW

#define PORT_VID_MEM                            "pbvlan_pvidm"
#define PORT_BASE_VLAN_PORT_VID_MEM_BOFFSET     16
#define PORT_BASE_VLAN_PORT_VID_MEM_BLEN        6
#define PORT_BASE_VLAN_PORT_VID_MEM_FLAG        HSL_RW

#define UNI_LEAKY_EN                            "pbvlan_ulen"
#define PORT_BASE_VLAN_UNI_LEAKY_EN_BOFFSET     14
#define PORT_BASE_VLAN_UNI_LEAKY_EN_BLEN        1
#define PORT_BASE_VLAN_UNI_LEAKY_EN_FLAG        HSL_RW

#define MUL_LEAKY_EN                            "pbvlan_mlen"
#define PORT_BASE_VLAN_MUL_LEAKY_EN_BOFFSET     13
#define PORT_BASE_VLAN_MUL_LEAKY_EN_BLEN        1
#define PORT_BASE_VLAN_MUL_LEAKY_EN_FLAG        HSL_RW


    /* Port Rate Limit0 Register */
#define RATE_LIMIT0                "rlmt0"
#define RATE_LIMIT0_ID             32
#define RATE_LIMIT0_OFFSET         0x0110
#define RATE_LIMIT0_E_LENGTH       4
#define RATE_LIMIT0_E_OFFSET       0x0100
#define RATE_LIMIT0_NR_E           6

#define ADD_RATE_BYTE                          "rlmt_addbyte"
#define RATE_LIMIT0_ADD_RATE_BYTE_BOFFSET      24
#define RATE_LIMIT0_ADD_RATE_BYTE_BLEN         8
#define RATE_LIMIT0_ADD_RATE_BYTE_FLAG         HSL_RW

#define EG_MNG_RATE_EN                         "rlmt_egmngen"
#define RATE_LIMIT0_EG_MNG_RATE_EN_BOFFSET     22
#define RATE_LIMIT0_EG_MNG_RATE_EN_BLEN        1
#define RATE_LIMIT0_EG_MNG_RATE_EN_FLAG        HSL_RW

#define IN_MNG_RATE_EN                         "rlmt_inmngen"
#define RATE_LIMIT0_IN_MNG_RATE_EN_BOFFSET     21
#define RATE_LIMIT0_IN_MNG_RATE_EN_BLEN        1
#define RATE_LIMIT0_IN_MNG_RATE_EN_FLAG        HSL_RW

#define IN_MUL_RATE_EN                         "rlmt_inmulen"
#define RATE_LIMIT0_IN_MUL_RATE_EN_BOFFSET     20
#define RATE_LIMIT0_IN_MUL_RATE_EN_BLEN        1
#define RATE_LIMIT0_IN_MUL_RATE_EN_FLAG        HSL_RW

#define ING_RATE                               "rlmt_ingrate"
#define RATE_LIMIT0_ING_RATE_BOFFSET           0
#define RATE_LIMIT0_ING_RATE_BLEN              13
#define RATE_LIMIT0_ING_RATE_FLAG              HSL_RW


    /* Priority Control Register */
#define PRI_CTL                   "prctl"
#define PRI_CTL_ID                33
#define PRI_CTL_OFFSET            0x0114
#define PRI_CTL_E_LENGTH          4
#define PRI_CTL_E_OFFSET          0x0100
#define PRI_CTL_NR_E              6

#define PORT_PRI_EN                             "prctl_ptprien"
#define PRI_CTL_PORT_PRI_EN_BOFFSET             19
#define PRI_CTL_PORT_PRI_EN_BLEN                1
#define PRI_CTL_PORT_PRI_EN_FLAG                HSL_RW

#define DA_PRI_EN                               "prctl_daprien"
#define PRI_CTL_DA_PRI_EN_BOFFSET               18
#define PRI_CTL_DA_PRI_EN_BLEN                  1
#define PRI_CTL_DA_PRI_EN_FLAG                  HSL_RW

#define VLAN_PRI_EN                             "prctl_vprien"
#define PRI_CTL_VLAN_PRI_EN_BOFFSET             17
#define PRI_CTL_VLAN_PRI_EN_BLEN                1
#define PRI_CTL_VLAN_PRI_EN_FLAG                HSL_RW

#define IP_PRI_EN                               "prctl_ipprien"
#define PRI_CTL_IP_PRI_EN_BOFFSET               16
#define PRI_CTL_IP_PRI_EN_BLEN                  1
#define PRI_CTL_IP_PRI_EN_FLAG                  HSL_RW

#define DA_PRI_SEL                              "prctl_dapris"
#define PRI_CTL_DA_PRI_SEL_BOFFSET              6
#define PRI_CTL_DA_PRI_SEL_BLEN                 2
#define PRI_CTL_DA_PRI_SEL_FLAG                 HSL_RW

#define VLAN_PRI_SEL                            "prctl_vpris"
#define PRI_CTL_VLAN_PRI_SEL_BOFFSET            4
#define PRI_CTL_VLAN_PRI_SEL_BLEN               2
#define PRI_CTL_VLAN_PRI_SEL_FLAG               HSL_RW

#define IP_PRI_SEL                              "prctl_ippris"
#define PRI_CTL_IP_PRI_SEL_BOFFSET              2
#define PRI_CTL_IP_PRI_SEL_BLEN                 2
#define PRI_CTL_IP_PRI_SEL_FLAG                 HSL_RW

#define PORT_PRI_SEL                            "prctl_ptpris"
#define PRI_CTL_PORT_PRI_SEL_BOFFSET            0
#define PRI_CTL_PORT_PRI_SEL_BLEN               2
#define PRI_CTL_PORT_PRI_SEL_FLAG               HSL_RW


    /* Storm Control Register */
#define STORM_CTL                 "sctrl"
#define STORM_CTL_ID                33
#define STORM_CTL_OFFSET            0x0118
#define STORM_CTL_E_LENGTH          4
#define STORM_CTL_E_OFFSET          0x0100
#define STORM_CTL_NR_E              6

#define UNIT                             "sctrl_unit"
#define STORM_CTL_UNIT_BOFFSET           24
#define STORM_CTL_UNIT_BLEN              2
#define STORM_CTL_UNIT_FLAG              HSL_RW

#define MUL_EN                           "sctrl_mulen"
#define STORM_CTL_MUL_EN_BOFFSET         10
#define STORM_CTL_MUL_EN_BLEN            1
#define STORM_CTL_MUL_EN_FLAG            HSL_RW

#define UNI_EN                           "sctrl_unien"
#define STORM_CTL_UNI_EN_BOFFSET         9
#define STORM_CTL_UNI_EN_BLEN            1
#define STORM_CTL_UNI_EN_FLAG            HSL_RW

#define BRO_EN                           "sctrl_broen"
#define STORM_CTL_BRO_EN_BOFFSET         8
#define STORM_CTL_BRO_EN_BLEN            1
#define STORM_CTL_BRO_EN_FLAG            HSL_RW

#define RATE                             "sctrl_rate"
#define STORM_CTL_RATE_BOFFSET           0
#define STORM_CTL_RATE_BLEN              4
#define STORM_CTL_RATE_FLAG              HSL_RW


    /* Queue Control Register */
#define QUEUE_CTL                   "qctl"
#define QUEUE_CTL_ID                34
#define QUEUE_CTL_OFFSET            0x011c
#define QUEUE_CTL_E_LENGTH          4
#define QUEUE_CTL_E_OFFSET          0x0100
#define QUEUE_CTL_NR_E              6

#define PORT_IN_DESC_EN                          "qctl_pdescen"
#define QUEUE_CTL_PORT_IN_DESC_EN_BOFFSET        28
#define QUEUE_CTL_PORT_IN_DESC_EN_BLEN           4
#define QUEUE_CTL_PORT_IN_DESC_EN_FLAG           HSL_RW

#define PORT_DESC_EN                             "qctl_pdescen"
#define QUEUE_CTL_PORT_DESC_EN_BOFFSET           25
#define QUEUE_CTL_PORT_DESC_EN_BLEN              1
#define QUEUE_CTL_PORT_DESC_EN_FLAG              HSL_RW

#define QUEUE_DESC_EN                            "qctl_qdescen"
#define QUEUE_CTL_QUEUE_DESC_EN_BOFFSET          24
#define QUEUE_CTL_QUEUE_DESC_EN_BLEN             1
#define QUEUE_CTL_QUEUE_DESC_EN_FLAG             HSL_RW

#define PORT_DESC_NR                             "qctl_pdscpnr"
#define QUEUE_CTL_PORT_DESC_NR_BOFFSET           16
#define QUEUE_CTL_PORT_DESC_NR_BLEN              6
#define QUEUE_CTL_PORT_DESC_NR_FLAG              HSL_RW

#define QUEUE3_DESC_NR                           "qctl_q3dscpnr"
#define QUEUE_CTL_QUEUE3_DESC_NR_BOFFSET         12
#define QUEUE_CTL_QUEUE3_DESC_NR_BLEN            4
#define QUEUE_CTL_QUEUE3_DESC_NR_FLAG            HSL_RW

#define QUEUE2_DESC_NR                           "qctl_q2dscpnr"
#define QUEUE_CTL_QUEUE2_DESC_NR_BOFFSET         8
#define QUEUE_CTL_QUEUE2_DESC_NR_BLEN            4
#define QUEUE_CTL_QUEUE2_DESC_NR_FLAG            HSL_RW

#define QUEUE1_DESC_NR                           "qctl_q1dscpnr"
#define QUEUE_CTL_QUEUE1_DESC_NR_BOFFSET         4
#define QUEUE_CTL_QUEUE1_DESC_NR_BLEN            4
#define QUEUE_CTL_QUEUE1_DESC_NR_FLAG            HSL_RW

#define QUEUE0_DESC_NR                           "qctl_q0dscpnr"
#define QUEUE_CTL_QUEUE0_DESC_NR_BOFFSET         0
#define QUEUE_CTL_QUEUE0_DESC_NR_BLEN            4
#define QUEUE_CTL_QUEUE0_DESC_NR_FLAG            HSL_RW


    /* Port Rate Limit1 Register */
#define RATE_LIMIT1                "rlmt1"
#define RATE_LIMIT1_ID             32
#define RATE_LIMIT1_OFFSET         0x0120
#define RATE_LIMIT1_E_LENGTH       4
#define RATE_LIMIT1_E_OFFSET       0x0100
#define RATE_LIMIT1_NR_E           6

#define EG_RATE                              "rlmt_egrate"
#define RATE_LIMIT1_EG_RATE_BOFFSET          0
#define RATE_LIMIT1_EG_RATE_BLEN             13
#define RATE_LIMIT1_EG_RATE_FLAG             HSL_RW


    /* Port Rate Limit3 Register */
#define RATE_LIMIT3                "rlmt3"
#define RATE_LIMIT3_ID             32
#define RATE_LIMIT3_OFFSET         0x0128
#define RATE_LIMIT3_E_LENGTH       4
#define RATE_LIMIT3_E_OFFSET       0x0100
#define RATE_LIMIT3_NR_E           6

#define EG_CBS                                "rlmt_egcbs"
#define RATE_LIMIT3_EG_CBS_BOFFSET            16
#define RATE_LIMIT3_EG_CBS_BLEN               2
#define RATE_LIMIT3_EG_CBS_FLAG               HSL_RW

#define EG_TS                                  "rlmt_egts"
#define RATE_LIMIT3_EG_TS_BOFFSET              0
#define RATE_LIMIT3_EG_TS_BLEN                 3
#define RATE_LIMIT3_EG_TS_FLAG                 HSL_RW


    /* Weight Round Robin Register */
#define WRR_CTRL                                "wrrc"
#define WRR_CTRL_ID                             32
#define WRR_CTRL_OFFSET                         0x012c
#define WRR_CTRL_E_LENGTH                       4
#define WRR_CTRL_E_OFFSET                       0x0100
#define WRR_CTRL_NR_E                           6

#define SCH_MODE                                "wrrc_mode"
#define WRR_CTRL_SCH_MODE_BOFFSET               29
#define WRR_CTRL_SCH_MODE_BLEN                  2
#define WRR_CTRL_SCH_MODE_FLAG                  HSL_RW


    /* mib memory info */
#define MIB_RXBROAD                       "RxBroad"
#define MIB_RXBROAD_ID                    34
#define MIB_RXBROAD_OFFSET                0x20000
#define MIB_RXBROAD_E_LENGTH              4
#define MIB_RXBROAD_E_OFFSET              0x100
#define MIB_RXBROAD_NR_E                  6

#define MIB_RXPAUSE                       "RxPause"
#define MIB_RXPAUSE_ID                    35
#define MIB_RXPAUSE_OFFSET                0x20004
#define MIB_RXPAUSE_E_LENGTH              4
#define MIB_RXPAUSE_E_OFFSET              0x100
#define MIB_RXPAUSE_NR_E                  6

#define MIB_RXMULTI                       "RxMulti"
#define MIB_RXMULTI_ID                    36
#define MIB_RXMULTI_OFFSET                0x20008
#define MIB_RXMULTI_E_LENGTH              4
#define MIB_RXMULTI_E_OFFSET              0x100
#define MIB_RXMULTI_NR_E                  6

#define MIB_RXFCSERR                      "RxFcsErr"
#define MIB_RXFCSERR_ID                   37
#define MIB_RXFCSERR_OFFSET               0x2000c
#define MIB_RXFCSERR_E_LENGTH             4
#define MIB_RXFCSERR_E_OFFSET             0x100
#define MIB_RXFCSERR_NR_E                 6

#define MIB_RXALLIGNERR                   "RxAllignErr"
#define MIB_RXALLIGNERR_ID                38
#define MIB_RXALLIGNERR_OFFSET            0x20010
#define MIB_RXALLIGNERR_E_LENGTH          4
#define MIB_RXALLIGNERR_E_OFFSET          0x100
#define MIB_RXALLIGNERR_NR_E              6

#define MIB_RXRUNT                        "RxRunt"
#define MIB_RXRUNT_ID                     39
#define MIB_RXRUNT_OFFSET                 0x20014
#define MIB_RXRUNT_E_LENGTH               4
#define MIB_RXRUNT_E_OFFSET               0x100
#define MIB_RXRUNT_NR_E                   6

#define MIB_RXFRAGMENT                    "RxFragment"
#define MIB_RXFRAGMENT_ID                 40
#define MIB_RXFRAGMENT_OFFSET             0x20018
#define MIB_RXFRAGMENT_E_LENGTH           4
#define MIB_RXFRAGMENT_E_OFFSET           0x100
#define MIB_RXFRAGMENT_NR_E               6

#define MIB_RX64BYTE                      "Rx64Byte"
#define MIB_RX64BYTE_ID                   41
#define MIB_RX64BYTE_OFFSET               0x2001c
#define MIB_RX64BYTE_E_LENGTH             4
#define MIB_RX64BYTE_E_OFFSET             0x100
#define MIB_RX64BYTE_NR_E                 6

#define MIB_RX128BYTE                     "Rx128Byte"
#define MIB_RX128BYTE_ID                  42
#define MIB_RX128BYTE_OFFSET              0x20020
#define MIB_RX128BYTE_E_LENGTH            4
#define MIB_RX128BYTE_E_OFFSET            0x100
#define MIB_RX128BYTE_NR_E                6

#define MIB_RX256BYTE                     "Rx256Byte"
#define MIB_RX256BYTE_ID                  43
#define MIB_RX256BYTE_OFFSET              0x20024
#define MIB_RX256BYTE_E_LENGTH            4
#define MIB_RX256BYTE_E_OFFSET            0x100
#define MIB_RX256BYTE_NR_E                6

#define MIB_RX512BYTE                     "Rx512Byte"
#define MIB_RX512BYTE_ID                  44
#define MIB_RX512BYTE_OFFSET              0x20028
#define MIB_RX512BYTE_E_LENGTH            4
#define MIB_RX512BYTE_E_OFFSET            0x100
#define MIB_RX512BYTE_NR_E                6

#define MIB_RX1024BYTE                    "Rx1024Byte"
#define MIB_RX1024BYTE_ID                 45
#define MIB_RX1024BYTE_OFFSET             0x2002c
#define MIB_RX1024BYTE_E_LENGTH           4
#define MIB_RX1024BYTE_E_OFFSET           0x100
#define MIB_RX1024BYTE_NR_E               6

#define MIB_RX1518BYTE                    "Rx1518Byte"
#define MIB_RX1518BYTE_ID                 45
#define MIB_RX1518BYTE_OFFSET             0x20030
#define MIB_RX1518BYTE_E_LENGTH           4
#define MIB_RX1518BYTE_E_OFFSET           0x100
#define MIB_RX1518BYTE_NR_E               6

#define MIB_RXMAXBYTE                     "RxMaxByte"
#define MIB_RXMAXBYTE_ID                  46
#define MIB_RXMAXBYTE_OFFSET              0x20034
#define MIB_RXMAXBYTE_E_LENGTH            4
#define MIB_RXMAXBYTE_E_OFFSET            0x100
#define MIB_RXMAXBYTE_NR_E                6

#define MIB_RXTOOLONG                     "RxTooLong"
#define MIB_RXTOOLONG_ID                  47
#define MIB_RXTOOLONG_OFFSET              0x20038
#define MIB_RXTOOLONG_E_LENGTH            4
#define MIB_RXTOOLONG_E_OFFSET            0x100
#define MIB_RXTOOLONG_NR_E                6

#define MIB_RXGOODBYTE_LO                 "RxGoodByteLo"
#define MIB_RXGOODBYTE_LO_ID              48
#define MIB_RXGOODBYTE_LO_OFFSET          0x2003c
#define MIB_RXGOODBYTE_LO_E_LENGTH        4
#define MIB_RXGOODBYTE_LO_E_OFFSET        0x100
#define MIB_RXGOODBYTE_LO_NR_E            6

#define MIB_RXGOODBYTE_HI                 "RxGoodByteHi"
#define MIB_RXGOODBYTE_HI_ID              49
#define MIB_RXGOODBYTE_HI_OFFSET          0x20040
#define MIB_RXGOODBYTE_HI_E_LENGTH        4
#define MIB_RXGOODBYTE_HI_E_OFFSET        0x100
#define MIB_RXGOODBYTE_HI_NR_E            6

#define MIB_RXBADBYTE_LO                  "RxBadByteLo"
#define MIB_RXBADBYTE_LO_ID               50
#define MIB_RXBADBYTE_LO_OFFSET           0x20044
#define MIB_RXBADBYTE_LO_E_LENGTH         4
#define MIB_RXBADBYTE_LO_E_OFFSET         0x100
#define MIB_RXBADBYTE_LO_NR_E             6

#define MIB_RXBADBYTE_HI                  "RxBadByteHi"
#define MIB_RXBADBYTE_HI_ID               51
#define MIB_RXBADBYTE_HI_OFFSET           0x20048
#define MIB_RXBADBYTE_HI_E_LENGTH         4
#define MIB_RXBADBYTE_HI_E_OFFSET         0x100
#define MIB_RXBADBYTE_HI_NR_E             6

#define MIB_RXOVERFLOW                    "RxOverFlow"
#define MIB_RXOVERFLOW_ID                 52
#define MIB_RXOVERFLOW_OFFSET             0x2004c
#define MIB_RXOVERFLOW_E_LENGTH           4
#define MIB_RXOVERFLOW_E_OFFSET           0x100
#define MIB_RXOVERFLOW_NR_E               6

#define MIB_FILTERED                      "Filtered"
#define MIB_FILTERED_ID                   53
#define MIB_FILTERED_OFFSET               0x20050
#define MIB_FILTERED_E_LENGTH             4
#define MIB_FILTERED_E_OFFSET             0x100
#define MIB_FILTERED_NR_E                 6

#define MIB_TXBROAD                       "TxBroad"
#define MIB_TXBROAD_ID                    54
#define MIB_TXBROAD_OFFSET                0x20054
#define MIB_TXBROAD_E_LENGTH              4
#define MIB_TXBROAD_E_OFFSET              0x100
#define MIB_TXBROAD_NR_E                  6

#define MIB_TXPAUSE                       "TxPause"
#define MIB_TXPAUSE_ID                    55
#define MIB_TXPAUSE_OFFSET                0x20058
#define MIB_TXPAUSE_E_LENGTH              4
#define MIB_TXPAUSE_E_OFFSET              0x100
#define MIB_TXPAUSE_NR_E                  6

#define MIB_TXMULTI                       "TxMulti"
#define MIB_TXMULTI_ID                    56
#define MIB_TXMULTI_OFFSET                0x2005c
#define MIB_TXMULTI_E_LENGTH              4
#define MIB_TXMULTI_E_OFFSET              0x100
#define MIB_TXMULTI_NR_E                  6

#define MIB_TXUNDERRUN                    "TxUnderRun"
#define MIB_TXUNDERRUN_ID                 57
#define MIB_TXUNDERRUN_OFFSET             0x20060
#define MIB_TXUNDERRUN_E_LENGTH           4
#define MIB_TXUNDERRUN_E_OFFSET           0x100
#define MIB_TXUNDERRUN_NR_E               6

#define MIB_TX64BYTE                      "Tx64Byte"
#define MIB_TX64BYTE_ID                   58
#define MIB_TX64BYTE_OFFSET               0x20064
#define MIB_TX64BYTE_E_LENGTH             4
#define MIB_TX64BYTE_E_OFFSET             0x100
#define MIB_TX64BYTE_NR_E                 6

#define MIB_TX128BYTE                     "Tx128Byte"
#define MIB_TX128BYTE_ID                  59
#define MIB_TX128BYTE_OFFSET              0x20068
#define MIB_TX128BYTE_E_LENGTH            4
#define MIB_TX128BYTE_E_OFFSET            0x100
#define MIB_TX128BYTE_NR_E                6

#define MIB_TX256BYTE                     "Tx256Byte"
#define MIB_TX256BYTE_ID                  60
#define MIB_TX256BYTE_OFFSET              0x2006c
#define MIB_TX256BYTE_E_LENGTH            4
#define MIB_TX256BYTE_E_OFFSET            0x100
#define MIB_TX256BYTE_NR_E                6

#define MIB_TX512BYTE                     "Tx512Byte"
#define MIB_TX512BYTE_ID                  61
#define MIB_TX512BYTE_OFFSET              0x20070
#define MIB_TX512BYTE_E_LENGTH            4
#define MIB_TX512BYTE_E_OFFSET            0x100
#define MIB_TX512BYTE_NR_E                6

#define MIB_TX1024BYTE                    "Tx1024Byte"
#define MIB_TX1024BYTE_ID                 62
#define MIB_TX1024BYTE_OFFSET             0x20074
#define MIB_TX1024BYTE_E_LENGTH           4
#define MIB_TX1024BYTE_E_OFFSET           0x100
#define MIB_TX1024BYTE_NR_E               6

#define MIB_TX1518BYTE                    "Tx1518Byte"
#define MIB_TX1518BYTE_ID                 62
#define MIB_TX1518BYTE_OFFSET             0x20078
#define MIB_TX1518BYTE_E_LENGTH           4
#define MIB_TX1518BYTE_E_OFFSET           0x100
#define MIB_TX1518BYTE_NR_E               6

#define MIB_TXMAXBYTE                     "TxMaxByte"
#define MIB_TXMAXBYTE_ID                  63
#define MIB_TXMAXBYTE_OFFSET              0x2007c
#define MIB_TXMAXBYTE_E_LENGTH            4
#define MIB_TXMAXBYTE_E_OFFSET            0x100
#define MIB_TXMAXBYTE_NR_E                6

#define MIB_TXOVERSIZE                    "TxOverSize"
#define MIB_TXOVERSIZE_ID                 64
#define MIB_TXOVERSIZE_OFFSET             0x20080
#define MIB_TXOVERSIZE_E_LENGTH           4
#define MIB_TXOVERSIZE_E_OFFSET           0x100
#define MIB_TXOVERSIZE_NR_E               6

#define MIB_TXBYTE_LO                     "TxByteLo"
#define MIB_TXBYTE_LO_ID                  65
#define MIB_TXBYTE_LO_OFFSET              0x20084
#define MIB_TXBYTE_LO_E_LENGTH            4
#define MIB_TXBYTE_LO_E_OFFSET            0x100
#define MIB_TXBYTE_LO_NR_E                6

#define MIB_TXBYTE_HI                     "TxByteHi"
#define MIB_TXBYTE_HI_ID                  66
#define MIB_TXBYTE_HI_OFFSET              0x20088
#define MIB_TXBYTE_HI_E_LENGTH            4
#define MIB_TXBYTE_HI_E_OFFSET            0x100
#define MIB_TXBYTE_HI_NR_E                6

#define MIB_TXCOLLISION                   "TxCollision"
#define MIB_TXCOLLISION_ID                67
#define MIB_TXCOLLISION_OFFSET            0x2008c
#define MIB_TXCOLLISION_E_LENGTH          4
#define MIB_TXCOLLISION_E_OFFSET          0x100
#define MIB_TXCOLLISION_NR_E              6

#define MIB_TXABORTCOL                    "TxAbortCol"
#define MIB_TXABORTCOL_ID                 68
#define MIB_TXABORTCOL_OFFSET             0x20090
#define MIB_TXABORTCOL_E_LENGTH           4
#define MIB_TXABORTCOL_E_OFFSET           0x100
#define MIB_TXABORTCOL_NR_E               6

#define MIB_TXMULTICOL                    "TxMultiCol"
#define MIB_TXMULTICOL_ID                 69
#define MIB_TXMULTICOL_OFFSET             0x20094
#define MIB_TXMULTICOL_E_LENGTH           4
#define MIB_TXMULTICOL_E_OFFSET           0x100
#define MIB_TXMULTICOL_NR_E               6

#define MIB_TXSINGALCOL                   "TxSingalCol"
#define MIB_TXSINGALCOL_ID                70
#define MIB_TXSINGALCOL_OFFSET            0x20098
#define MIB_TXSINGALCOL_E_LENGTH          4
#define MIB_TXSINGALCOL_E_OFFSET          0x100
#define MIB_TXSINGALCOL_NR_E              6

#define MIB_TXEXCDEFER                    "TxExcDefer"
#define MIB_TXEXCDEFER_ID                 71
#define MIB_TXEXCDEFER_OFFSET             0x2009c
#define MIB_TXEXCDEFER_E_LENGTH           4
#define MIB_TXEXCDEFER_E_OFFSET           0x100
#define MIB_TXEXCDEFER_NR_E               6

#define MIB_TXDEFER                       "TxDefer"
#define MIB_TXDEFER_ID                    72
#define MIB_TXDEFER_OFFSET                0x200a0
#define MIB_TXDEFER_E_LENGTH              4
#define MIB_TXDEFER_E_OFFSET              0x100
#define MIB_TXDEFER_NR_E                  6

#define MIB_TXLATECOL                     "TxLateCol"
#define MIB_TXLATECOL_ID                  73
#define MIB_TXLATECOL_OFFSET              0x200a4
#define MIB_TXLATECOL_E_LENGTH            4
#define MIB_TXLATECOL_E_OFFSET            0x100
#define MIB_TXLATECOL_NR_E                6


#define PPPOE_SESSION                              "pppoes"
#define PPPOE_SESSION_ID                           13
#define PPPOE_SESSION_OFFSET                       0x59100
#define PPPOE_SESSION_E_LENGTH                     4
#define PPPOE_SESSION_E_OFFSET                     0x4
#define PPPOE_SESSION_NR_E                         16

#define ENTRY_VALID                                "pppoes_v"
#define PPPOE_SESSION_ENTRY_VALID_BOFFSET          19
#define PPPOE_SESSION_ENTRY_VALID_BLEN             1
#define PPPOE_SESSION_ENTRY_VALID_FLAG             HSL_RW

#define STRIP_EN                                   "pppoes_s"
#define PPPOE_SESSION_STRIP_EN_BOFFSET             16
#define PPPOE_SESSION_STRIP_EN_BLEN                1
#define PPPOE_SESSION_STRIP_EN_FLAG                HSL_RW

#define SEESION_ID                                 "pppoes_id"
#define PPPOE_SESSION_SEESION_ID_BOFFSET           0
#define PPPOE_SESSION_SEESION_ID_BLEN              16
#define PPPOE_SESSION_SEESION_ID_FLAG              HSL_RW



#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _HORUS_REG_H_ */

