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



#ifndef _ATHENA_REG_H
#define _ATHENA_REG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MAX_ENTRY_LEN 128

#define HSL_RW 1
#define HSL_RO 0

    /**
      * Athena Mask Control Register
      */
#define MASK_CTL                  "mask"
#define MASK_CTL_ID               0
#define MASK_CTL_OFFSET           0x0000
#define MASK_CTL_E_LENGTH         4
#define MASK_CTL_E_OFFSET         0
#define MASK_CTL_NR_E             0

#define SOFT_RST                              "mask_rst"
#define MASK_CTL_SOFT_RST_BOFFSET             31
#define MASK_CTL_SOFT_RST_BLEN                1
#define MASK_CTL_SOFT_RST_FLAG                HSL_RW

#define MII_CLK_SEL                           "mask_clks"
#define MASK_CTL_MII_CLK_SEL_BOFFSET          24
#define MASK_CTL_MII_CLK_SEL_BLEN             1
#define MASK_CTL_MII_CLK_SEL_FLAG             HSL_RW

#define RMII_PHY_RX_SEL                       "mask_prxs"
#define MASK_CTL_RMII_PHY_RX_SEL_BOFFSET      23
#define MASK_CTL_RMII_PHY_RX_SEL_BLEN         1
#define MASK_CTL_RMII_PHY_RX_SEL_FLAG         HSL_RW

#define RMII_PHY_TX_SEL                       "mask_ptxs"
#define MASK_CTL_RMII_PHY_TX_SEL_BOFFSET      22
#define MASK_CTL_RMII_PHY_TX_SEL_BLEN         1
#define MASK_CTL_RMII_PHY_TX_SEL_FLAG         HSL_RW

#define RMII_MAC_RX_SEL                       "mask_mrxs"
#define MASK_CTL_RMII_MAC_RX_SEL_BOFFSET      21
#define MASK_CTL_RMII_MAC_RX_SEL_BLEN         1
#define MASK_CTL_RMII_MAC_RX_SEL_FLAG         HSL_RW

#define RMII_MAC_TX_SEL                       "mask_mtxs"
#define MASK_CTL_RMII_MAC_TX_SEL_BOFFSET      20
#define MASK_CTL_RMII_MAC_TX_SEL_BLEN         1
#define MASK_CTL_RMII_MAC_TX_SEL_FLAG         HSL_RW

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

    /**
      * Global Interrupt Register
      */
#define GLOBAL_INT                "gint"
#define GLOBAL_INT_ID             1
#define GLOBAL_INT_OFFSET         0x0010
#define GLOBAL_INT_E_LENGTH       4
#define GLOBAL_INT_E_OFFSET       0
#define GLOBAL_INT_NR_E           0

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

#define GLB_EEPROM_ERR                        "gint_epei"
#define GLOBAL_INT_GLB_EEPROM_ERR_BOFFSET     1
#define GLOBAL_INT_GLB_EEPROM_ERR_BLEN        1
#define GLOBAL_INT_GLB_EEPROM_ERR_FLAG        HSL_RW

#define GLB_EEPROM_INT                        "gint_epi"
#define GLOBAL_INT_GLB_EEPROM_INT_BOFFSET     0
#define GLOBAL_INT_GLB_EEPROM_INT_BLEN        1
#define GLOBAL_INT_GLB_EEPROM_INT_FLAG        HSL_RW

    /**
      * Global Interrupt Mask Register
      */
#define GLOBAL_INT_MASK           "gintm"
#define GLOBAL_INT_MASK_ID        2
#define GLOBAL_INT_MASK_OFFSET    0x0014
#define GLOBAL_INT_MASK_E_LENGTH  4
#define GLOBAL_INT_MASK_E_OFFSET  0
#define GLOBAL_INT_MASK_NR_E      0

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


    /**
      * Global MAC Address Register
      */
//high
#define GLOBAL_MAC_ADDR0           "gmac0"
#define GLOBAL_MAC_ADDR0_ID        3
#define GLOBAL_MAC_ADDR0_OFFSET    0x0020
#define GLOBAL_MAC_ADDR0_E_LENGTH  4
#define GLOBAL_MAC_ADDR0_E_OFFSET  0
#define GLOBAL_MAC_ADDR0_NR_E      0

#define GLB_BYTE4                                  "gmac_b4"
#define GLOBAL_MAC_ADDR0_GLB_BYTE4_BOFFSET         8
#define GLOBAL_MAC_ADDR0_GLB_BYTE4_BLEN            8
#define GLOBAL_MAC_ADDR0_GLB_BYTE4_FLAG            HSL_RW

#define GLB_BYTE5                                  "gmac_b5"
#define GLOBAL_MAC_ADDR0_GLB_BYTE5_BOFFSET         0
#define GLOBAL_MAC_ADDR0_GLB_BYTE5_BLEN            8
#define GLOBAL_MAC_ADDR0_GLB_BYTE5_FLAG            HSL_RW


//low
#define GLOBAL_MAC_ADDR1           "gmac1"
#define GLOBAL_MAC_ADDR1_ID        4
#define GLOBAL_MAC_ADDR1_OFFSET    0x0024
#define GLOBAL_MAC_ADDR1_E_LENGTH  4
#define GLOBAL_MAC_ADDR1_E_OFFSET  0
#define GLOBAL_MAC_ADDR1_NR_E      0

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

    /**
      * Global Control Register
      */
#define GLOBAL_CTL                "gctl"
#define GLOBAL_CTL_ID             5
#define GLOBAL_CTL_OFFSET         0x0030
#define GLOBAL_CTL_E_LENGTH       4
#define GLOBAL_CTL_E_OFFSET       0
#define GLOBAL_CTL_NR_E           0

#define WEIGHT_PRIORITY                       "gctl_wpri"
#define GLOBAL_CTL_WEIGHT_PRIORITY_BOFFSET    31
#define GLOBAL_CTL_WEIGHT_PRIORITY_BLEN       1
#define GLOBAL_CTL_WEIGHT_PRIORITY_FLAG       HSL_RW

#define RATE_DROP_EN                          "gctl_rden"
#define GLOBAL_CTL_RATE_DROP_EN_BOFFSET       30
#define GLOBAL_CTL_RATE_DROP_EN_BLEN          1
#define GLOBAL_CTL_RATE_DROP_EN_FLAG          HSL_RW

#define QM_PRI_MODE                           "gctl_qmpm"
#define GLOBAL_CTL_QM_PRI_MODE_BOFFSET        29
#define GLOBAL_CTL_QM_PRI_MODE_BLEN           1
#define GLOBAL_CTL_QM_PRI_MODE_FLAG           HSL_RW

#define MIX_PRIORITY                          "gctl_mpri"
#define GLOBAL_CTL_MIX_PRIORITY_BOFFSET       28
#define GLOBAL_CTL_MIX_PRIORITY_BLEN          1
#define GLOBAL_CTL_MIX_PRIORITY_FLAG          HSL_RW

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

#define BROAD_STORM_CTRL                      "gctl_bsct"
#define GLOBAL_CTL_BROAD_STORM_CTRL_BOFFSET   16
#define GLOBAL_CTL_BROAD_STORM_CTRL_BLEN      2
#define GLOBAL_CTL_BROAD_STORM_CTRL_FLAG      HSL_RW

#define BROAD_STORM_EN                        "gctl_bsen"
#define GLOBAL_CTL_BROAD_STORM_EN_BOFFSET     11
#define GLOBAL_CTL_BROAD_STORM_EN_BLEN        1
#define GLOBAL_CTL_BROAD_STORM_EN_FLAG        HSL_RW

#define MAX_FRAME_SIZE                        "gctl_mfsz"
#define GLOBAL_CTL_MAX_FRAME_SIZE_BOFFSET     0
#define GLOBAL_CTL_MAX_FRAME_SIZE_BLEN        11
#define GLOBAL_CTL_MAX_FRAME_SIZE_FLAG        HSL_RW

    /**
      * Flow Control Register
      */
#define FLOW_CTL                  "fctl"
#define FLOW_CTL_ID               6
#define FLOW_CTL_OFFSET           0x0034
#define FLOW_CTL_E_LENGTH         4
#define FLOW_CTL_E_OFFSET         0
#define FLOW_CTL_NR_E             0

#define TEST_PAUSE                            "fctl_tps"
#define FLOW_CTL_TEST_PAUSE_BOFFSET           31
#define FLOW_CTL_TEST_PAUSE_BLEN              1
#define FLOW_CTL_TEST_PAUSE_FLAG              HSL_RW

#define PORT_PAUSE_OFF_THRES                  "fctl_pofft"
#define FLOW_CTL_PORT_PAUSE_OFF_THRES_BOFFSET 24
#define FLOW_CTL_PORT_PAUSE_OFF_THRES_BLEN    7
#define FLOW_CTL_PORT_PAUSE_OFF_THRES_FLAG    HSL_RW

#define PORT_PAUSE_ON_THRES                   "fctl_pont"
#define FLOW_CTL_PORT_PAUSE_ON_THRES_BOFFSET  16
#define FLOW_CTL_PORT_PAUSE_ON_THRES_BLEN     7
#define FLOW_CTL_PORT_PAUSE_ON_THRES_FLAG     HSL_RW

#define GOL_PAUSE_OFF_THRES                   "fctl_gofft"
#define FLOW_CTL_GOL_PAUSE_OFF_THRES_BOFFSET  8
#define FLOW_CTL_GOL_PAUSE_OFF_THRES_BLEN     8
#define FLOW_CTL_GOL_PAUSE_OFF_THRES_FLAG     HSL_RW

#define GOL_PAUSE_ON_THRES                    "fctl_gont"
#define FLOW_CTL_GOL_PAUSE_ON_THRES_BOFFSET   0
#define FLOW_CTL_GOL_PAUSE_ON_THRES_BLEN      8
#define FLOW_CTL_GOL_PAUSE_ON_THRES_FLAG      HSL_RW

    /**
      * QM Control Register
      */
#define QM_CTL                    "qmct"
#define QM_CTL_ID                 7
#define QM_CTL_OFFSET             0x0038
#define QM_CTL_E_LENGTH           4
#define QM_CTL_E_OFFSET           0
#define QM_CTL_NR_E               0

#define QM_ERR_RST_EN                           "qmct_qeren"
#define QM_CTL_QM_ERR_RST_EN_BOFFSET          31
#define QM_CTL_QM_ERR_RST_EN_BLEN             1
#define QM_CTL_QM_ERR_RST_EN_FLAG             HSL_RW

#define LOOKUP_ERR_RST_EN                     "qmct_lpesen"
#define QM_CTL_LOOKUP_ERR_RST_EN_BOFFSET      30
#define QM_CTL_LOOKUP_ERR_RST_EN_BLEN         1
#define QM_CTL_LOOKUP_ERR_RST_EN_FLAG         HSL_RW

#define FLOOD_TO_CPU_EN                       "qmct_fdcpuen"
#define QM_CTL_FLOOD_TO_CPU_EN_BOFFSET        10
#define QM_CTL_FLOOD_TO_CPU_EN_BLEN           1
#define QM_CTL_FLOOD_TO_CPU_EN_FLAG           HSL_RW

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

#define FLOW_DROP_CNT                         "qmct_fdcn"
#define QM_CTL_FLOW_DROP_CNT_BOFFSET          0
#define QM_CTL_FLOW_DROP_CNT_BLEN             5
#define QM_CTL_FLOW_DROP_CNT_FLAG             HSL_RW

    /**
      * QM Error Register
      */
#define QM_ERR                    "qmer"
#define QM_ERR_ID                 8
#define QM_ERR_OFFSET             0x003C
#define QM_ERR_E_LENGTH           4
#define QM_ERR_E_OFFSET           0
#define QM_ERR_NR_E               0

#define QM_ERR_DATA                           "qmer_data"
#define QM_ERR_QM_ERR_DATA_BOFFSET            0
#define QM_ERR_QM_ERR_DATA_BLEN               32
#define QM_ERR_QM_ERR_DATA_FLAG               HSL_RO

    /**
      * Vlan Table Function Register
      */
//high
#define VLAN_TABLE_FUNC0           "vtbf0"
#define VLAN_TABLE_FUNC0_ID        9
#define VLAN_TABLE_FUNC0_OFFSET    0x0040
#define VLAN_TABLE_FUNC0_E_LENGTH  4
#define VLAN_TABLE_FUNC0_E_OFFSET  0
#define VLAN_TABLE_FUNC0_NR_E      0

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

//low
#define VLAN_TABLE_FUNC1           "vtbf1"
#define VLAN_TABLE_FUNC1_ID        10
#define VLAN_TABLE_FUNC1_OFFSET    0x0044
#define VLAN_TABLE_FUNC1_E_LENGTH  4
#define VLAN_TABLE_FUNC1_E_OFFSET  0
#define VLAN_TABLE_FUNC1_NR_E      0

#define PORT_TAG_EN                            "vtbf_pgen"
#define VLAN_TABLE_FUNC1_PORT_TAG_EN_BOFFSET   12
#define VLAN_TABLE_FUNC1_PORT_TAG_EN_BLEN      20
#define VLAN_TABLE_FUNC1_PORT_TAG_EN_FLAG      HSL_RW

#define VT_VALID                               "vtbf_vtvd"
#define VLAN_TABLE_FUNC1_VT_VALID_BOFFSET      11
#define VLAN_TABLE_FUNC1_VT_VALID_BLEN         1
#define VLAN_TABLE_FUNC1_VT_VALID_FLAG         HSL_RW

#define VID_MEM                                "vtbf_vidm"
#define VLAN_TABLE_FUNC1_VID_MEM_BOFFSET       0
#define VLAN_TABLE_FUNC1_VID_MEM_BLEN          10
#define VLAN_TABLE_FUNC1_VID_MEM_FLAG          HSL_RW

    /**
      * Address Table Function Register
      */
#define ADDR_TABLE_FUNC0           "atbf0"
#define ADDR_TABLE_FUNC0_ID        11
#define ADDR_TABLE_FUNC0_OFFSET    0x0050
#define ADDR_TABLE_FUNC0_E_LENGTH  4
#define ADDR_TABLE_FUNC0_E_OFFSET  0
#define ADDR_TABLE_FUNC0_NR_E      0

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

#define SA_DROP_EN                             "atbf_saden"
#define ADDR_TABLE_FUNC2_SA_DROP_EN_BOFFSET    16
#define ADDR_TABLE_FUNC2_SA_DROP_EN_BLEN       1
#define ADDR_TABLE_FUNC2_SA_DROP_EN_FLAG       HSL_RW

#define AT_STATUS                              "atbf_atsts"
#define ADDR_TABLE_FUNC2_AT_STATUS_BOFFSET     14
#define ADDR_TABLE_FUNC2_AT_STATUS_BLEN        2
#define ADDR_TABLE_FUNC2_AT_STATUS_FLAG        HSL_RW

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

#define DES_PORT                               "atbf_desp"
#define ADDR_TABLE_FUNC2_DES_PORT_BOFFSET      0
#define ADDR_TABLE_FUNC2_DES_PORT_BLEN         10
#define ADDR_TABLE_FUNC2_DES_PORT_FLAG         HSL_RW

    /**
      * Address Table Control Register
      */
#define ADDR_TABLE_CTL            "atbc"
#define ADDR_TABLE_CTL_ID         14
#define ADDR_TABLE_CTL_OFFSET     0x005C
#define ADDR_TABLE_CTL_E_LENGTH   4
#define ADDR_TABLE_CTL_E_OFFSET   0
#define ADDR_TABLE_CTL_NR_E       0

#define ARP_EN                                 "atbc_arpe"
#define ADDR_TABLE_CTL_ARP_EN_BOFFSET          20
#define ADDR_TABLE_CTL_ARP_EN_BLEN             1
#define ADDR_TABLE_CTL_ARP_EN_FLAG             HSL_RW

#define ARL_INI_EN                             "atbc_arlie"
#define ADDR_TABLE_CTL_ARL_INI_EN_BOFFSET      19
#define ADDR_TABLE_CTL_ARL_INI_EN_BLEN         1
#define ADDR_TABLE_CTL_ARL_INI_EN_FLAG         HSL_RW

#define BPDU_EN                                "atbc_bpdue"
#define ADDR_TABLE_CTL_BPDU_EN_BOFFSET         18
#define ADDR_TABLE_CTL_BPDU_EN_BLEN            1
#define ADDR_TABLE_CTL_BPDU_EN_FLAG            HSL_RW

#define AGE_EN                                 "atbc_agee"
#define ADDR_TABLE_CTL_AGE_EN_BOFFSET          17
#define ADDR_TABLE_CTL_AGE_EN_BLEN             1
#define ADDR_TABLE_CTL_AGE_EN_FLAG             HSL_RW

#define AGE_TIME                               "atbc_aget"
#define ADDR_TABLE_CTL_AGE_TIME_BOFFSET        0
#define ADDR_TABLE_CTL_AGE_TIME_BLEN           16
#define ADDR_TABLE_CTL_AGE_TIME_FLAG           HSL_RW

    /**
      * IP Priority Mapping Register
      */
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

    /**
      * Tag Priority Mapping Register
      */
#define TAG_PRI_MAPPING           "tpmap"
#define TAG_PRI_MAPPING_ID        19
#define TAG_PRI_MAPPING_OFFSET    0x0070
#define TAG_PRI_MAPPING_E_LENGTH  4
#define TAG_PRI_MAPPING_E_OFFSET  0
#define TAG_PRI_MAPPING_NR_E      0

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

    /**
      * Cpu Port Register
      */
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

#define RMII_CUT                              "cpup_rmcut"
#define CPU_PORT_RMII_CUT_BOFFSET             0
#define CPU_PORT_RMII_CUT_BLEN                1
#define CPU_PORT_RMII_CUT_FLAG                HSL_RW

    /**
      * MIB Function Register
      */
#define MIB_CNT                  "mibcnt"
#define MIB_CNT_ID               21
#define MIB_CNT_OFFSET           0x0080
#define MIB_CNT_E_LENGTH         4
#define MIB_CNT_E_OFFSET         0
#define MIB_CNT_NR_E             0

#define MIB_FUNC                              "mibcnt_mibf"
#define MIB_CNT_MIB_FUNC_BOFFSET              24
#define MIB_CNT_MIB_FUNC_BLEN                 3
#define MIB_CNT_MIB_FUNC_FLAG                 HSL_RW

#define MIB_BUSY                              "mibcnt_mibb"
#define MIB_CNT_MIB_BUSY_BOFFSET              17
#define MIB_CNT_MIB_BUSY_BLEN                 1
#define MIB_CNT_MIB_BUSY_FLAG                 HSL_RW

#define MIB_AT_HALF_EN                        "mibcnt_mibhe"
#define MIB_CNT_MIB_AT_HALF_EN_BOFFSET        16
#define MIB_CNT_MIB_AT_HALF_EN_BLEN           1
#define MIB_CNT_MIB_AT_HALF_EN_FLAG           HSL_RW

#define MIB_TIMER                             "mibcnt_mibt"
#define MIB_CNT_MIB_TIMER_BOFFSET             0
#define MIB_CNT_MIB_TIMER_BLEN                16
#define MIB_CNT_MIB_TIMER_FLAG                HSL_RW

    /**
      * SPI Interface Register
      */
#define SPI_INTERFACE             "spi"
#define SPI_INTERFACE_ID          22
#define SPI_INTERFACE_OFFSET      0x0090
#define SPI_INTERFACE_E_LENGTH    4
#define SPI_INTERFACE_E_OFFSET    0
#define SPI_INTERFACE_NR_E        0

#define DEBUG_OEN                               "spi_dben"
#define SPI_INTERFACE_DEBUG_OEN_BOFFSET       7
#define SPI_INTERFACE_DEBUG_OEN_BLEN          25
#define SPI_INTERFACE_DEBUG_OEN_FLAG          HSL_RO

#define SPI_EN                                "spi_spien"
#define SPI_INTERFACE_SPI_EN_BOFFSET          5
#define SPI_INTERFACE_SPI_EN_BLEN             1
#define SPI_INTERFACE_SPI_EN_FLAG             HSL_RO

#define SPI_SPEED                             "spi_spisp"
#define SPI_INTERFACE_SPI_SPEED_BOFFSET       4
#define SPI_INTERFACE_SPI_SPEED_BLEN          1
#define SPI_INTERFACE_SPI_SPEED_FLAG          HSL_RO

#define UART_SPEED                            "spi_utsp"
#define SPI_INTERFACE_UART_SPEED_BOFFSET      3
#define SPI_INTERFACE_UART_SPEED_BLEN         1
#define SPI_INTERFACE_UART_SPEED_FLAG         HSL_RO

#define RMII_EN                               "spi_rmen"
#define SPI_INTERFACE_RMII_EN_BOFFSET         2
#define SPI_INTERFACE_RMII_EN_BLEN            1
#define SPI_INTERFACE_RMII_EN_FLAG            HSL_RO

#define MII_EN                                "spi_miien"
#define SPI_INTERFACE_MII_EN_BOFFSET          1
#define SPI_INTERFACE_MII_EN_BLEN             1
#define SPI_INTERFACE_MII_EN_FLAG             HSL_RO

#define SPI_SIZE                              "spi_spisz"
#define SPI_INTERFACE_SPI_SIZE_BOFFSET        0
#define SPI_INTERFACE_SPI_SIZE_BLEN           1
#define SPI_INTERFACE_SPI_SIZE_FLAG           HSL_RO

    /**
      * MDIO High Address Register
      */
#define MDIO_HIGH_ADDR            "mdiohd"
#define MDIO_HIGH_ADDR_ID         23
#define MDIO_HIGH_ADDR_OFFSET     0x0094
#define MDIO_HIGH_ADDR_E_LENGTH   4
#define MDIO_HIGH_ADDR_E_OFFSET   0
#define MDIO_HIGH_ADDR_NR_E       0

#define MDIO_HA                               "mdiohd_ha"
#define MDIO_HIGH_ADDR_MDIO_HA_BOFFSET        0
#define MDIO_HIGH_ADDR_MDIO_HA_BLEN           9
#define MDIO_HIGH_ADDR_MDIO_HA_FLAG           HSL_RW

    /**
      * Destination IP Address Register
      */
#define DIP_ADDR               "dip"
#define DIP_ADDR_ID            24
#define DIP_ADDR_OFFSET        0x0098
#define DIP_ADDR_E_LENGTH      4
#define DIP_ADDR_E_OFFSET      0
#define DIP_ADDR_NR_E          0

#define DES_IP_ADDR                           "dip_addr"
#define DIP_ADDR_DES_IP_ADDR_BOFFSET          0
#define DIP_ADDR_DES_IP_ADDR_BLEN             32
#define DIP_ADDR_DES_IP_ADDR_FLAG             HSL_RW

    /**
      * BIST Control Register
      */
#define BIST_CTL                  "bctl"
#define BIST_CTL_ID               25
#define BIST_CTL_OFFSET           0x00A0
#define BIST_CTL_E_LENGTH         4
#define BIST_CTL_E_OFFSET         0
#define BIST_CTL_NR_E             0

#define BIST_BUSY                             "bctl_busy"
#define BIST_CTL_BIST_BUSY_BOFFSET            31
#define BIST_CTL_BIST_BUSY_BLEN               1
#define BIST_CTL_BIST_BUSY_FLAG               HSL_RW

#define BIST_ERR_MEM                          "bctl_errmem"
#define BIST_CTL_BIST_ERR_MEM_BOFFSET         24
#define BIST_CTL_BIST_ERR_MEM_BLEN            4
#define BIST_CTL_BIST_ERR_MEM_FLAG            HSL_RO

#define BIST_PTN_EN_2                         "bctl_ptnen2"
#define BIST_CTL_BIST_PTN_EN_2_BOFFSET        22
#define BIST_CTL_BIST_PTN_EN_2_BLEN           1
#define BIST_CTL_BIST_PTN_EN_2_FLAG           HSL_RW

#define BIST_PTN_EN_1                         "bctl_ptnen1"
#define BIST_CTL_BIST_PTN_EN_1_BOFFSET        21
#define BIST_CTL_BIST_PTN_EN_1_BLEN           1
#define BIST_CTL_BIST_PTN_EN_1_FLAG           HSL_RW

#define BIST_PTN_EN_0                         "bctl_ptnen0"
#define BIST_CTL_BIST_PTN_EN_0_BOFFSET        20
#define BIST_CTL_BIST_PTN_EN_0_BLEN           1
#define BIST_CTL_BIST_PTN_EN_0_FLAG           HSL_RW

#define BIST_ERR_PTN                          "bctl_errptn"
#define BIST_CTL_BIST_ERR_PTN_BOFFSET         16
#define BIST_CTL_BIST_ERR_PTN_BLEN            2
#define BIST_CTL_BIST_ERR_PTN_FLAG            HSL_RO

#define BIST_ERR_CNT                          "bctl_errcnt"
#define BIST_CTL_BIST_ERR_CNT_BOFFSET         13
#define BIST_CTL_BIST_ERR_CNT_BLEN            3
#define BIST_CTL_BIST_ERR_CNT_FLAG            HSL_RO

#define BIST_ERR_ADDR                         "bctl_errad"
#define BIST_CTL_BIST_ERR_ADDR_BOFFSET        0
#define BIST_CTL_BIST_ERR_ADDR_BLEN           13
#define BIST_CTL_BIST_ERR_ADDR_FLAG           HSL_RO

    /**
      * Debug Control Register
      */
#define DEBUG_CTL0                 "dctl0"
#define DEBUG_CTL0_ID              26
#define DEBUG_CTL0_OFFSET          0x00F0
#define DEBUG_CTL0_E_LENGTH        4
#define DEBUG_CTL0_E_OFFSET        0
#define DEBUG_CTL0_NR_E            0

#define DEBUG_SEL                               "dctl_sel"
#define DEBUG_CTL0_DEBUG_SEL_BOFFSET          16
#define DEBUG_CTL0_DEBUG_SEL_BLEN             8
#define DEBUG_CTL0_DEBUG_SEL_FLAG             HSL_RW

#define DEBUG_PORT_NUM                        "dctl_ptnum"
#define DEBUG_CTL0_DEBUG_PORT_NUM_BOFFSET     8
#define DEBUG_CTL0_DEBUG_PORT_NUM_BLEN        4
#define DEBUG_CTL0_DEBUG_PORT_NUM_FLAG        HSL_RW

#define DEBUG_ADDR                            "dctl_addr"
#define DEBUG_CTL0_DEBUG_ADDR_BOFFSET         0
#define DEBUG_CTL0_DEBUG_ADDR_BLEN            8
#define DEBUG_CTL0_DEBUG_ADDR_FLAG            HSL_RW


#define DEBUG_CTL1                 "dctl1"
#define DEBUG_CTL1_ID              27
#define DEBUG_CTL1_OFFSET          0x00F4
#define DEBUG_CTL1_E_LENGTH        4
#define DEBUG_CTL1_E_OFFSET        0
#define DEBUG_CTL1_NR_E            0

#define DEBUG_DATA                             "dctl_data"
#define DEBUG_CTL1_DEBUG_DATA_BOFFSET          0
#define DEBUG_CTL1_DEBUG_DATA_BLEN             32
#define DEBUG_CTL1_DEBUG_DATA_FLAG             HSL_RO

    /**
      * QM Debug Control Register
      */
#define QM_DEBUG_CTL              "qmdctl"
#define QM_DEBUG_CTL_ID           28
#define QM_DEBUG_CTL_OFFSET       0x00F8
#define QM_DEBUG_CTL_E_LENGTH     4
#define QM_DEBUG_CTL_E_OFFSET     0
#define QM_DEBUG_CTL_NR_E         0

#define QM_DBG_CTRL                             "qmdctl_dbgctl"
#define QM_DEBUG_CTL_QM_DBG_CTRL_BOFFSET        0
#define QM_DEBUG_CTL_QM_DBG_CTRL_BLEN           32
#define QM_DEBUG_CTL_QM_DBG_CTRL_FLAG           HSL_RW

    /**
      * Port Status Register
      */
#define PORT_STATUS               "ptsts"
#define PORT_STATUS_ID            29
#define PORT_STATUS_OFFSET        0x0100
#define PORT_STATUS_E_LENGTH      4
#define PORT_STATUS_E_OFFSET      0x0100
#define PORT_STATUS_NR_E          6

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

    /**
      * Port Control Register
      */
#define PORT_CTL                  "pctl"
#define PORT_CTL_ID               30
#define PORT_CTL_OFFSET           0x0104
#define PORT_CTL_E_LENGTH         4
#define PORT_CTL_E_OFFSET         0x0100
#define PORT_CTL_NR_E             6

#define ING_MIRROR_EN                             "pctl_ingmiren"
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

#define SINGLE_VLAN_EN                          "pctl_svlanen"
#define PORT_CTL_SINGLE_VLAN_EN_BOFFSET         13
#define PORT_CTL_SINGLE_VLAN_EN_BLEN            1
#define PORT_CTL_SINGLE_VLAN_EN_FLAG            HSL_RW

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

#define PORT_STATE                              "pctl_pstate"
#define PORT_CTL_PORT_STATE_BOFFSET             0
#define PORT_CTL_PORT_STATE_BLEN                3
#define PORT_CTL_PORT_STATE_FLAG                HSL_RW

    /**
      * Port Based Vlan Register
      */
#define PORT_BASE_VLAN            "pbvlan"
#define PORT_BASE_VLAN_ID         31
#define PORT_BASE_VLAN_OFFSET     0x0108
#define PORT_BASE_VLAN_E_LENGTH   4
#define PORT_BASE_VLAN_E_OFFSET   0x0100
#define PORT_BASE_VLAN_NR_E     6

#define DOT1Q_MODE                              "pbvlan_8021q"
#define PORT_BASE_VLAN_DOT1Q_MODE_BOFFSET       30
#define PORT_BASE_VLAN_DOT1Q_MODE_BLEN          2
#define PORT_BASE_VLAN_DOT1Q_MODE_FLAG          HSL_RW

#define ING_PRI                                 "pbvlan_ingpri"
#define PORT_BASE_VLAN_ING_PRI_BOFFSET          28
#define PORT_BASE_VLAN_ING_PRI_BLEN             2
#define PORT_BASE_VLAN_ING_PRI_FLAG             HSL_RW

#define EG_TAG_PRI0                             "pbvlan_egtpri"
#define PORT_BASE_VLAN_EG_TAG_PRI0_BOFFSET      27
#define PORT_BASE_VLAN_EG_TAG_PRI0_BLEN         1
#define PORT_BASE_VLAN_EG_TAG_PRI0_FLAG         HSL_RW

#define PORT_VID_MEM                            "pbvlan_pvidm"
#define PORT_BASE_VLAN_PORT_VID_MEM_BOFFSET     16
#define PORT_BASE_VLAN_PORT_VID_MEM_BLEN        6
#define PORT_BASE_VLAN_PORT_VID_MEM_FLAG        HSL_RW

#define PORT_VID                                "pbvlan_ptvid"
#define PORT_BASE_VLAN_PORT_VID_BOFFSET         0
#define PORT_BASE_VLAN_PORT_VID_BLEN            12
#define PORT_BASE_VLAN_PORT_VID_FLAG            HSL_RW

    /**
      * Port Rate Limit Register
      */
#define RATE_LIMIT                "rlmt"
#define RATE_LIMIT_ID             32
#define RATE_LIMIT_OFFSET         0x010C
#define RATE_LIMIT_E_LENGTH       4
#define RATE_LIMIT_E_OFFSET       0x0100
#define RATE_LIMIT_NR_E         6

#define EGRESS_RATE_EN                          "rlmt_egrateen"
#define RATE_LIMIT_EGRESS_RATE_EN_BOFFSET       25
#define RATE_LIMIT_EGRESS_RATE_EN_BLEN          1
#define RATE_LIMIT_EGRESS_RATE_EN_FLAG          HSL_RW

#define INGRESS_RATE_EN                         "rlmt_ingrateen"
#define RATE_LIMIT_INGRESS_RATE_EN_BOFFSET      24
#define RATE_LIMIT_INGRESS_RATE_EN_BLEN         1
#define RATE_LIMIT_INGRESS_RATE_EN_FLAG         HSL_RW

#define EG_RATE                                 "rlmt_egrate"
#define RATE_LIMIT_EG_RATE_BOFFSET              16
#define RATE_LIMIT_EG_RATE_BLEN                 4
#define RATE_LIMIT_EG_RATE_FLAG                 HSL_RW

#define ING_RATE                                "rlmt_ingrate"
#define RATE_LIMIT_ING_RATE_BOFFSET             0
#define RATE_LIMIT_ING_RATE_BLEN                4
#define RATE_LIMIT_ING_RATE_FLAG                HSL_RW

    /**
      * Priority Control Register
      */
#define PRI_CTL                   "prctl"
#define PRI_CTL_ID                33
#define PRI_CTL_OFFSET            0x0110
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


//mib memory info
#define MIB_RXBROAD                       "RxBroad"
#define MIB_RXBROAD_ID                    34
#define MIB_RXBROAD_OFFSET                0x19000
#define MIB_RXBROAD_E_LENGTH              4
#define MIB_RXBROAD_E_OFFSET              0xa0
#define MIB_RXBROAD_NR_E                  6

#define MIB_RXPAUSE                       "RxPause"
#define MIB_RXPAUSE_ID                    35
#define MIB_RXPAUSE_OFFSET                0x19004
#define MIB_RXPAUSE_E_LENGTH              4
#define MIB_RXPAUSE_E_OFFSET              0xa0
#define MIB_RXPAUSE_NR_E                  6

#define MIB_RXMULTI                       "RxMulti"
#define MIB_RXMULTI_ID                    36
#define MIB_RXMULTI_OFFSET                0x19008
#define MIB_RXMULTI_E_LENGTH              4
#define MIB_RXMULTI_E_OFFSET              0xa0
#define MIB_RXMULTI_NR_E                  6

#define MIB_RXFCSERR                      "RxFcsErr"
#define MIB_RXFCSERR_ID                   37
#define MIB_RXFCSERR_OFFSET               0x1900C
#define MIB_RXFCSERR_E_LENGTH             4
#define MIB_RXFCSERR_E_OFFSET             0xa0
#define MIB_RXFCSERR_NR_E                 6

#define MIB_RXALLIGNERR                   "RxAllignErr"
#define MIB_RXALLIGNERR_ID                38
#define MIB_RXALLIGNERR_OFFSET            0x19010
#define MIB_RXALLIGNERR_E_LENGTH          4
#define MIB_RXALLIGNERR_E_OFFSET          0xa0
#define MIB_RXALLIGNERR_NR_E              6

#define MIB_RXRUNT                        "RxRunt"
#define MIB_RXRUNT_ID                     39
#define MIB_RXRUNT_OFFSET                 0x19014
#define MIB_RXRUNT_E_LENGTH               4
#define MIB_RXRUNT_E_OFFSET               0xa0
#define MIB_RXRUNT_NR_E                   6

#define MIB_RXFRAGMENT                    "RxFragment"
#define MIB_RXFRAGMENT_ID                 40
#define MIB_RXFRAGMENT_OFFSET             0x19018
#define MIB_RXFRAGMENT_E_LENGTH           4
#define MIB_RXFRAGMENT_E_OFFSET           0xa0
#define MIB_RXFRAGMENT_NR_E               6

#define MIB_RX64BYTE                      "Rx64Byte"
#define MIB_RX64BYTE_ID                   41
#define MIB_RX64BYTE_OFFSET               0x1901C
#define MIB_RX64BYTE_E_LENGTH             4
#define MIB_RX64BYTE_E_OFFSET             0xa0
#define MIB_RX64BYTE_NR_E                 6

#define MIB_RX128BYTE                     "Rx128Byte"
#define MIB_RX128BYTE_ID                  42
#define MIB_RX128BYTE_OFFSET              0x19020
#define MIB_RX128BYTE_E_LENGTH            4
#define MIB_RX128BYTE_E_OFFSET            0xa0
#define MIB_RX128BYTE_NR_E                6

#define MIB_RX256BYTE                     "Rx256Byte"
#define MIB_RX256BYTE_ID                  43
#define MIB_RX256BYTE_OFFSET              0x19024
#define MIB_RX256BYTE_E_LENGTH            4
#define MIB_RX256BYTE_E_OFFSET            0xa0
#define MIB_RX256BYTE_NR_E                6

#define MIB_RX512BYTE                     "Rx512Byte"
#define MIB_RX512BYTE_ID                  44
#define MIB_RX512BYTE_OFFSET              0x19028
#define MIB_RX512BYTE_E_LENGTH            4
#define MIB_RX512BYTE_E_OFFSET            0xa0
#define MIB_RX512BYTE_NR_E                6

#define MIB_RX1024BYTE                    "Rx1024Byte"
#define MIB_RX1024BYTE_ID                 45
#define MIB_RX1024BYTE_OFFSET             0x1902C
#define MIB_RX1024BYTE_E_LENGTH           4
#define MIB_RX1024BYTE_E_OFFSET           0xa0
#define MIB_RX1024BYTE_NR_E               6

#define MIB_RX1518BYTE                    "Rx1518Byte"  //reserved for s16 

#define MIB_RXMAXBYTE                     "RxMaxByte"
#define MIB_RXMAXBYTE_ID                  46
#define MIB_RXMAXBYTE_OFFSET              0x19030
#define MIB_RXMAXBYTE_E_LENGTH            4
#define MIB_RXMAXBYTE_E_OFFSET            0xa0
#define MIB_RXMAXBYTE_NR_E                6

#define MIB_RXTOOLONG                     "RxTooLong"
#define MIB_RXTOOLONG_ID                  47
#define MIB_RXTOOLONG_OFFSET              0x19034
#define MIB_RXTOOLONG_E_LENGTH            4
#define MIB_RXTOOLONG_E_OFFSET            0xa0
#define MIB_RXTOOLONG_NR_E                6

#define MIB_RXGOODBYTE_LO                 "RxGoodByteLo"
#define MIB_RXGOODBYTE_LO_ID              48
#define MIB_RXGOODBYTE_LO_OFFSET          0x19038
#define MIB_RXGOODBYTE_LO_E_LENGTH        4
#define MIB_RXGOODBYTE_LO_E_OFFSET        0xa0
#define MIB_RXGOODBYTE_LO_NR_E            6

#define MIB_RXGOODBYTE_HI                 "RxGoodByteHi"
#define MIB_RXGOODBYTE_HI_ID              49
#define MIB_RXGOODBYTE_HI_OFFSET          0x1903C
#define MIB_RXGOODBYTE_HI_E_LENGTH        4
#define MIB_RXGOODBYTE_HI_E_OFFSET        0xa0
#define MIB_RXGOODBYTE_HI_NR_E            6

#define MIB_RXBADBYTE_LO                  "RxBadByteLo"
#define MIB_RXBADBYTE_LO_ID               50
#define MIB_RXBADBYTE_LO_OFFSET           0x19040
#define MIB_RXBADBYTE_LO_E_LENGTH         4
#define MIB_RXBADBYTE_LO_E_OFFSET         0xa0
#define MIB_RXBADBYTE_LO_NR_E             6

#define MIB_RXBADBYTE_HI                  "RxBadByteHi"
#define MIB_RXBADBYTE_HI_ID               51
#define MIB_RXBADBYTE_HI_OFFSET           0x19044
#define MIB_RXBADBYTE_HI_E_LENGTH         4
#define MIB_RXBADBYTE_HI_E_OFFSET         0xa0
#define MIB_RXBADBYTE_HI_NR_E             6

#define MIB_RXOVERFLOW                    "RxOverFlow"
#define MIB_RXOVERFLOW_ID                 52
#define MIB_RXOVERFLOW_OFFSET             0x19048
#define MIB_RXOVERFLOW_E_LENGTH           4
#define MIB_RXOVERFLOW_E_OFFSET           0xa0
#define MIB_RXOVERFLOW_NR_E               6

#define MIB_FILTERED                      "Filtered"
#define MIB_FILTERED_ID                   53
#define MIB_FILTERED_OFFSET               0x1904C
#define MIB_FILTERED_E_LENGTH             4
#define MIB_FILTERED_E_OFFSET             0xa0
#define MIB_FILTERED_NR_E                 6

#define MIB_TXBROAD                       "TxBroad"
#define MIB_TXBROAD_ID                    54
#define MIB_TXBROAD_OFFSET                0x19050
#define MIB_TXBROAD_E_LENGTH              4
#define MIB_TXBROAD_E_OFFSET              0xa0
#define MIB_TXBROAD_NR_E                  6

#define MIB_TXPAUSE                       "TxPause"
#define MIB_TXPAUSE_ID                    55
#define MIB_TXPAUSE_OFFSET                0x19054
#define MIB_TXPAUSE_E_LENGTH              4
#define MIB_TXPAUSE_E_OFFSET              0xa0
#define MIB_TXPAUSE_NR_E                  6

#define MIB_TXMULTI                       "TxMulti"
#define MIB_TXMULTI_ID                    56
#define MIB_TXMULTI_OFFSET                0x19058
#define MIB_TXMULTI_E_LENGTH              4
#define MIB_TXMULTI_E_OFFSET              0xa0
#define MIB_TXMULTI_NR_E                  6

#define MIB_TXUNDERRUN                    "TxUnderRun"
#define MIB_TXUNDERRUN_ID                 57
#define MIB_TXUNDERRUN_OFFSET             0x1905C
#define MIB_TXUNDERRUN_E_LENGTH           4
#define MIB_TXUNDERRUN_E_OFFSET           0xa0
#define MIB_TXUNDERRUN_NR_E               6

#define MIB_TX64BYTE                      "Tx64Byte"
#define MIB_TX64BYTE_ID                   58
#define MIB_TX64BYTE_OFFSET               0x19060
#define MIB_TX64BYTE_E_LENGTH             4
#define MIB_TX64BYTE_E_OFFSET             0xa0
#define MIB_TX64BYTE_NR_E                 6

#define MIB_TX128BYTE                     "Tx128Byte"
#define MIB_TX128BYTE_ID                  59
#define MIB_TX128BYTE_OFFSET              0x19064
#define MIB_TX128BYTE_E_LENGTH            4
#define MIB_TX128BYTE_E_OFFSET            0xa0
#define MIB_TX128BYTE_NR_E                6

#define MIB_TX256BYTE                     "Tx256Byte"
#define MIB_TX256BYTE_ID                  60
#define MIB_TX256BYTE_OFFSET              0x19068
#define MIB_TX256BYTE_E_LENGTH            4
#define MIB_TX256BYTE_E_OFFSET            0xa0
#define MIB_TX256BYTE_NR_E                6

#define MIB_TX512BYTE                     "Tx512Byte"
#define MIB_TX512BYTE_ID                  61
#define MIB_TX512BYTE_OFFSET              0x1906C
#define MIB_TX512BYTE_E_LENGTH            4
#define MIB_TX512BYTE_E_OFFSET            0xa0
#define MIB_TX512BYTE_NR_E                6

#define MIB_TX1024BYTE                    "Tx1024Byte"
#define MIB_TX1024BYTE_ID                 62
#define MIB_TX1024BYTE_OFFSET             0x19070
#define MIB_TX1024BYTE_E_LENGTH           4
#define MIB_TX1024BYTE_E_OFFSET           0xa0
#define MIB_TX1024BYTE_NR_E               6

#define MIB_TX1518BYTE                    "Tx1518Byte"  //reserved for s16 

#define MIB_TXMAXBYTE                     "TxMaxByte"
#define MIB_TXMAXBYTE_ID                  63
#define MIB_TXMAXBYTE_OFFSET              0x19074
#define MIB_TXMAXBYTE_E_LENGTH            4
#define MIB_TXMAXBYTE_E_OFFSET            0xa0
#define MIB_TXMAXBYTE_NR_E                6

#define MIB_TXOVERSIZE                    "TxOverSize"
#define MIB_TXOVERSIZE_ID                 64
#define MIB_TXOVERSIZE_OFFSET             0x19078
#define MIB_TXOVERSIZE_E_LENGTH           4
#define MIB_TXOVERSIZE_E_OFFSET           0xa0
#define MIB_TXOVERSIZE_NR_E              6

#define MIB_TXBYTE_LO                     "TxByteLo"
#define MIB_TXBYTE_LO_ID                  65
#define MIB_TXBYTE_LO_OFFSET              0x1907C
#define MIB_TXBYTE_LO_E_LENGTH            4
#define MIB_TXBYTE_LO_E_OFFSET            0xa0
#define MIB_TXBYTE_LO_NR_E                6

#define MIB_TXBYTE_HI                     "TxByteHi"
#define MIB_TXBYTE_HI_ID                  66
#define MIB_TXBYTE_HI_OFFSET              0x19080
#define MIB_TXBYTE_HI_E_LENGTH            4
#define MIB_TXBYTE_HI_E_OFFSET            0xa0
#define MIB_TXBYTE_HI_NR_E                6

#define MIB_TXCOLLISION                   "TxCollision"
#define MIB_TXCOLLISION_ID                67
#define MIB_TXCOLLISION_OFFSET            0x19084
#define MIB_TXCOLLISION_E_LENGTH          4
#define MIB_TXCOLLISION_E_OFFSET          0xa0
#define MIB_TXCOLLISION_NR_E              6

#define MIB_TXABORTCOL                    "TxAbortCol"
#define MIB_TXABORTCOL_ID                 68
#define MIB_TXABORTCOL_OFFSET             0x19088
#define MIB_TXABORTCOL_E_LENGTH           4
#define MIB_TXABORTCOL_E_OFFSET           0xa0
#define MIB_TXABORTCOL_NR_E               6

#define MIB_TXMULTICOL                    "TxMultiCol"
#define MIB_TXMULTICOL_ID                 69
#define MIB_TXMULTICOL_OFFSET             0x1908C
#define MIB_TXMULTICOL_E_LENGTH           4
#define MIB_TXMULTICOL_E_OFFSET           0xa0
#define MIB_TXMULTICOL_NR_E               6

#define MIB_TXSINGALCOL                   "TxSingalCol"
#define MIB_TXSINGALCOL_ID                70
#define MIB_TXSINGALCOL_OFFSET            0x19090
#define MIB_TXSINGALCOL_E_LENGTH          4
#define MIB_TXSINGALCOL_E_OFFSET          0xa0
#define MIB_TXSINGALCOL_NR_E              6

#define MIB_TXEXCDEFER                    "TxExcDefer"
#define MIB_TXEXCDEFER_ID                 71
#define MIB_TXEXCDEFER_OFFSET             0x19094
#define MIB_TXEXCDEFER_E_LENGTH           4
#define MIB_TXEXCDEFER_E_OFFSET           0xa0
#define MIB_TXEXCDEFER_NR_E               6

#define MIB_TXDEFER                       "TxDefer"
#define MIB_TXDEFER_ID                    72
#define MIB_TXDEFER_OFFSET                0x19098
#define MIB_TXDEFER_E_LENGTH              4
#define MIB_TXDEFER_E_OFFSET              0xa0
#define MIB_TXDEFER_NR_E                  6

#define MIB_TXLATECOL                     "TxLateCol"
#define MIB_TXLATECOL_ID                  73
#define MIB_TXLATECOL_OFFSET              0x1909C
#define MIB_TXLATECOL_E_LENGTH            4
#define MIB_TXLATECOL_E_OFFSET            0xa0
#define MIB_TXLATECOL_NR_E                6

//second mem block
#define MIB_RXBROAD_2                     "RxBroad_2"
#define MIB_RXBROAD_2_ID                  34
#define MIB_RXBROAD_2_OFFSET              (MIB_RXBROAD_OFFSET + 0x400)
#define MIB_RXBROAD_2_E_LENGTH            4
#define MIB_RXBROAD_2_E_OFFSET            0xa0
#define MIB_RXBROAD_2_NR_E                6

#define MIB_RXPAUSE_2                     "RxPause_2"
#define MIB_RXPAUSE_2_ID                  35
#define MIB_RXPAUSE_2_OFFSET              (MIB_RXPAUSE_OFFSET + 0x400)
#define MIB_RXPAUSE_2_E_LENGTH            4
#define MIB_RXPAUSE_2_E_OFFSET            0xa0
#define MIB_RXPAUSE_2_NR_E                6

#define MIB_RXMULTI_2                     "RxMulti_2"
#define MIB_RXMULTI_2_ID                  36
#define MIB_RXMULTI_2_OFFSET              (MIB_RXMULTI_OFFSET + 0x400)
#define MIB_RXMULTI_2_E_LENGTH            4
#define MIB_RXMULTI_2_E_OFFSET            0xa0
#define MIB_RXMULTI_2_NR_E                6

#define MIB_RXFCSERR_2                    "RxFcsErr_2"
#define MIB_RXFCSERR_2_ID                 37
#define MIB_RXFCSERR_2_OFFSET             (MIB_RXFCSERR_OFFSET + 0x400)
#define MIB_RXFCSERR_2_E_LENGTH           4
#define MIB_RXFCSERR_2_E_OFFSET           0xa0
#define MIB_RXFCSERR_2_NR_E               6

#define MIB_RXALLIGNERR_2                 "RxAllignErr_2"
#define MIB_RXALLIGNERR_2_ID              38
#define MIB_RXALLIGNERR_2_OFFSET          (MIB_RXALLIGNERR_OFFSET + 0x400)
#define MIB_RXALLIGNERR_2_E_LENGTH        4
#define MIB_RXALLIGNERR_2_E_OFFSET        0xa0
#define MIB_RXALLIGNERR_2_NR_E            6

#define MIB_RXRUNT_2                      "RxRunt_2"
#define MIB_RXRUNT_2_ID                   39
#define MIB_RXRUNT_2_OFFSET               (MIB_RXRUNT_OFFSET + 0x400)
#define MIB_RXRUNT_2_E_LENGTH             4
#define MIB_RXRUNT_2_E_OFFSET             0xa0
#define MIB_RXRUNT_2_NR_E                 6

#define MIB_RXFRAGMENT_2                  "RxFragment_2"
#define MIB_RXFRAGMENT_2_ID               40
#define MIB_RXFRAGMENT_2_OFFSET           (MIB_RXFRAGMENT_OFFSET + 0x400)
#define MIB_RXFRAGMENT_2_E_LENGTH         4
#define MIB_RXFRAGMENT_2_E_OFFSET         0xa0
#define MIB_RXFRAGMENT_2_NR_E             6

#define MIB_RX64BYTE_2                    "Rx64Byte_2"
#define MIB_RX64BYTE_2_ID                 41
#define MIB_RX64BYTE_2_OFFSET             (MIB_RX64BYTE_OFFSET + 0x400)
#define MIB_RX64BYTE_2_E_LENGTH           4
#define MIB_RX64BYTE_2_E_OFFSET           0xa0
#define MIB_RX64BYTE_2_NR_E               6

#define MIB_RX128BYTE_2                   "Rx128Byte_2"
#define MIB_RX128BYTE_2_ID                42
#define MIB_RX128BYTE_2_OFFSET            (MIB_RX128BYTE_OFFSET + 0x400)
#define MIB_RX128BYTE_2_E_LENGTH          4
#define MIB_RX128BYTE_2_E_OFFSET          0xa0
#define MIB_RX128BYTE_2_NR_E              6

#define MIB_RX256BYTE_2                   "Rx256Byte_2"
#define MIB_RX256BYTE_2_ID                43
#define MIB_RX256BYTE_2_OFFSET            (MIB_RX256BYTE_OFFSET + 0x400)
#define MIB_RX256BYTE_2_E_LENGTH          4
#define MIB_RX256BYTE_2_E_OFFSET          0xa0
#define MIB_RX256BYTE_2_NR_E              6

#define MIB_RX512BYTE_2                   "Rx512Byte_2"
#define MIB_RX512BYTE_2_ID                44
#define MIB_RX512BYTE_2_OFFSET            (MIB_RX512BYTE_OFFSET + 0x400)
#define MIB_RX512BYTE_2_E_LENGTH          4
#define MIB_RX512BYTE_2_E_OFFSET          0xa0
#define MIB_RX512BYTE_2_NR_E              6

#define MIB_RX1024BYTE_2                  "Rx1024Byte_2"
#define MIB_RX1024BYTE_2_ID               45
#define MIB_RX1024BYTE_2_OFFSET           (MIB_RX1024BYTE_OFFSET + 0x400)
#define MIB_RX1024BYTE_2_E_LENGTH         4
#define MIB_RX1024BYTE_2_E_OFFSET         0xa0
#define MIB_RX1024BYTE_2_NR_E             6

#define MIB_RXMAXBYTE_2                   "RxMaxByte_2"
#define MIB_RXMAXBYTE_2_ID                46
#define MIB_RXMAXBYTE_2_OFFSET            (MIB_RXMAXBYTE_OFFSET + 0x400)
#define MIB_RXMAXBYTE_2_E_LENGTH          4
#define MIB_RXMAXBYTE_2_E_OFFSET          0xa0
#define MIB_RXMAXBYTE_2_NR_E              6

#define MIB_RXTOOLONG_2                   "RxTooLong_2"
#define MIB_RXTOOLONG_2_ID                47
#define MIB_RXTOOLONG_2_OFFSET            (MIB_RXTOOLONG_OFFSET + 0x400)
#define MIB_RXTOOLONG_2_E_LENGTH          4
#define MIB_RXTOOLONG_2_E_OFFSET          0xa0
#define MIB_RXTOOLONG_2_NR_E             6

#define MIB_RXGOODBYTE_LO_2               "RxGoodByteLo_2"
#define MIB_RXGOODBYTE_LO_2_ID            48
#define MIB_RXGOODBYTE_LO_2_OFFSET        (MIB_RXGOODBYTE_LO_OFFSET + 0x400)
#define MIB_RXGOODBYTE_LO_2_E_LENGTH      4
#define MIB_RXGOODBYTE_LO_2_E_OFFSET      0xa0
#define MIB_RXGOODBYTE_LO_2_NR_E          6

#define MIB_RXGOODBYTE_HI_2               "RxGoodByteHi_2"
#define MIB_RXGOODBYTE_HI_2_ID            49
#define MIB_RXGOODBYTE_HI_2_OFFSET        (MIB_RXGOODBYTE_HI_OFFSET + 0x400)
#define MIB_RXGOODBYTE_HI_2_E_LENGTH      4
#define MIB_RXGOODBYTE_HI_2_E_OFFSET      0xa0
#define MIB_RXGOODBYTE_HI_2_NR_E          6

#define MIB_RXBADBYTE_LO_2                "RxBadByteLo_2"
#define MIB_RXBADBYTE_LO_2_ID             50
#define MIB_RXBADBYTE_LO_2_OFFSET         (MIB_RXBADBYTE_LO_OFFSET + 0x400)
#define MIB_RXBADBYTE_LO_2_E_LENGTH       4
#define MIB_RXBADBYTE_LO_2_E_OFFSET       0xa0
#define MIB_RXBADBYTE_LO_2_NR_E           6

#define MIB_RXBADBYTE_HI_2                "RxBadByteHi_2"
#define MIB_RXBADBYTE_HI_2_ID             51
#define MIB_RXBADBYTE_HI_2_OFFSET         (MIB_RXBADBYTE_HI_OFFSET + 0x400)
#define MIB_RXBADBYTE_HI_2_E_LENGTH       4
#define MIB_RXBADBYTE_HI_2_E_OFFSET       0xa0
#define MIB_RXBADBYTE_HI_2_NR_E           6

#define MIB_RXOVERFLOW_2                  "RxOverFlow_2"
#define MIB_RXOVERFLOW_2_ID               52
#define MIB_RXOVERFLOW_2_OFFSET           (MIB_RXOVERFLOW_OFFSET + 0x400)
#define MIB_RXOVERFLOW_2_E_LENGTH         4
#define MIB_RXOVERFLOW_2_E_OFFSET         0xa0
#define MIB_RXOVERFLOW_2_NR_E             6

#define MIB_FILTERED_2                    "Filtered_2"
#define MIB_FILTERED_2_ID                 53
#define MIB_FILTERED_2_OFFSET             (MIB_FILTERED_OFFSET + 0x400)
#define MIB_FILTERED_2_E_LENGTH           4
#define MIB_FILTERED_2_E_OFFSET           0xa0
#define MIB_FILTERED_2_NR_E               6

#define MIB_TXBROAD_2                     "TxBroad_2"
#define MIB_TXBROAD_2_ID                  54
#define MIB_TXBROAD_2_OFFSET              (MIB_TXBROAD_OFFSET + 0x400)
#define MIB_TXBROAD_2_E_LENGTH            4
#define MIB_TXBROAD_2_E_OFFSET            0xa0
#define MIB_TXBROAD_2_NR_E                6

#define MIB_TXPAUSE_2                     "TxPause_2"
#define MIB_TXPAUSE_2_ID                  55
#define MIB_TXPAUSE_2_OFFSET              (MIB_TXPAUSE_OFFSET + 0x400)
#define MIB_TXPAUSE_2_E_LENGTH            4
#define MIB_TXPAUSE_2_E_OFFSET            0xa0
#define MIB_TXPAUSE_2_NR_E                6

#define MIB_TXMULTI_2                     "TxMulti_2"
#define MIB_TXMULTI_2_ID                  56
#define MIB_TXMULTI_2_OFFSET              (MIB_TXMULTI_OFFSET + 0x400)
#define MIB_TXMULTI_2_E_LENGTH            4
#define MIB_TXMULTI_2_E_OFFSET            0xa0
#define MIB_TXMULTI_2_NR_E                6

#define MIB_TXUNDERRUN_2                  "TxUnderRun_2"
#define MIB_TXUNDERRUN_2_ID               57
#define MIB_TXUNDERRUN_2_OFFSET           (MIB_TXUNDERRUN_OFFSET + 0x400)
#define MIB_TXUNDERRUN_2_E_LENGTH         4
#define MIB_TXUNDERRUN_2_E_OFFSET         0xa0
#define MIB_TXUNDERRUN_2_NR_E             6

#define MIB_TX64BYTE_2                    "Tx64Byte_2"
#define MIB_TX64BYTE_2_ID                 58
#define MIB_TX64BYTE_2_OFFSET             (MIB_TX64BYTE_OFFSET + 0x400)
#define MIB_TX64BYTE_2_E_LENGTH           4
#define MIB_TX64BYTE_2_E_OFFSET           0xa0
#define MIB_TX64BYTE_2_NR_E               6

#define MIB_TX128BYTE_2                   "Tx128Byte_2"
#define MIB_TX128BYTE_2_ID                59
#define MIB_TX128BYTE_2_OFFSET            (MIB_TX128BYTE_OFFSET + 0x400)
#define MIB_TX128BYTE_2_E_LENGTH          4
#define MIB_TX128BYTE_2_E_OFFSET          0xa0
#define MIB_TX128BYTE_2_NR_E              6

#define MIB_TX256BYTE_2                   "Tx256Byte_2"
#define MIB_TX256BYTE_2_ID                60
#define MIB_TX256BYTE_2_OFFSET            (MIB_TX256BYTE_OFFSET + 0x400)
#define MIB_TX256BYTE_2_E_LENGTH          4
#define MIB_TX256BYTE_2_E_OFFSET          0xa0
#define MIB_TX256BYTE_2_NR_E              6

#define MIB_TX512BYTE_2                   "Tx512Byte_2"
#define MIB_TX512BYTE_2_ID                61
#define MIB_TX512BYTE_2_OFFSET            (MIB_TX512BYTE_OFFSET + 0x400)
#define MIB_TX512BYTE_2_E_LENGTH          4
#define MIB_TX512BYTE_2_E_OFFSET          0xa0
#define MIB_TX512BYTE_2_NR_E              6

#define MIB_TX1024BYTE_2                  "Tx1024Byte_2"
#define MIB_TX1024BYTE_2_ID               62
#define MIB_TX1024BYTE_2_OFFSET           (MIB_TX1024BYTE_OFFSET + 0x400)
#define MIB_TX1024BYTE_2_E_LENGTH         4
#define MIB_TX1024BYTE_2_E_OFFSET         0xa0
#define MIB_TX1024BYTE_2_NR_E             6

#define MIB_TXMAXBYTE_2                   "TxMaxByte_2"
#define MIB_TXMAXBYTE_2_ID                63
#define MIB_TXMAXBYTE_2_OFFSET            (MIB_TXMAXBYTE_OFFSET + 0x400)
#define MIB_TXMAXBYTE_2_E_LENGTH          4
#define MIB_TXMAXBYTE_2_E_OFFSET          0xa0
#define MIB_TXMAXBYTE_2_NR_E              6

#define MIB_TXOVERSIZE_2                  "TxOverSize_2"
#define MIB_TXOVERSIZE_2_ID               64
#define MIB_TXOVERSIZE_2_OFFSET           (MIB_TXOVERSIZE_OFFSET + 0x400)
#define MIB_TXOVERSIZE_2_E_LENGTH         4
#define MIB_TXOVERSIZE_2_E_OFFSET         0xa0
#define MIB_TXOVERSIZE_2_NR_E             6

#define MIB_TXBYTE_LO_2                   "TxByteLo_2"
#define MIB_TXBYTE_LO_2_ID                65
#define MIB_TXBYTE_LO_2_OFFSET            (MIB_TXBYTE_LO_OFFSET + 0x400)
#define MIB_TXBYTE_LO_2_E_LENGTH          4
#define MIB_TXBYTE_LO_2_E_OFFSET          0xa0
#define MIB_TXBYTE_LO_2_NR_E              6

#define MIB_TXBYTE_HI_2                   "TxByteHi_2"
#define MIB_TXBYTE_HI_2_ID                66
#define MIB_TXBYTE_HI_2_OFFSET            (MIB_TXBYTE_HI_OFFSET + 0x400)
#define MIB_TXBYTE_HI_2_E_LENGTH          4
#define MIB_TXBYTE_HI_2_E_OFFSET          0xa0
#define MIB_TXBYTE_HI_2_NR_E              6

#define MIB_TXCOLLISION_2                 "TxCollision_2"
#define MIB_TXCOLLISION_2_ID              67
#define MIB_TXCOLLISION_2_OFFSET          (MIB_TXCOLLISION_OFFSET + 0x400)
#define MIB_TXCOLLISION_2_E_LENGTH        4
#define MIB_TXCOLLISION_2_E_OFFSET        0xa0
#define MIB_TXCOLLISION_2_NR_E            6

#define MIB_TXABORTCOL_2                  "TxAbortCol_2"
#define MIB_TXABORTCOL_2_ID               68
#define MIB_TXABORTCOL_2_OFFSET           (MIB_TXABORTCOL_OFFSET + 0x400)
#define MIB_TXABORTCOL_2_E_LENGTH         4
#define MIB_TXABORTCOL_2_E_OFFSET         0xa0
#define MIB_TXABORTCOL_2_NR_E             6

#define MIB_TXMULTICOL_2                  "TxMultiCol_2"
#define MIB_TXMULTICOL_2_ID               69
#define MIB_TXMULTICOL_2_OFFSET           (MIB_TXMULTICOL_OFFSET + 0x400)
#define MIB_TXMULTICOL_2_E_LENGTH         4
#define MIB_TXMULTICOL_2_E_OFFSET         0xa0
#define MIB_TXMULTICOL_2_NR_E             6

#define MIB_TXSINGALCOL_2                 "TxSingalCol_2"
#define MIB_TXSINGALCOL_2_ID              70
#define MIB_TXSINGALCOL_2_OFFSET          (MIB_TXSINGALCOL_OFFSET + 0x400)
#define MIB_TXSINGALCOL_2_E_LENGTH        4
#define MIB_TXSINGALCOL_2_E_OFFSET        0xa0
#define MIB_TXSINGALCOL_2_NR_E            6

#define MIB_TXEXCDEFER_2                  "TxExcDefer_2"
#define MIB_TXEXCDEFER_2_ID               71
#define MIB_TXEXCDEFER_2_OFFSET           (MIB_TXEXCDEFER_OFFSET + 0x400)
#define MIB_TXEXCDEFER_2_E_LENGTH         4
#define MIB_TXEXCDEFER_2_E_OFFSET         0xa0
#define MIB_TXEXCDEFER_2_NR_E             6

#define MIB_TXDEFER_2                     "TxDefer_2"
#define MIB_TXDEFER_2_ID                  72
#define MIB_TXDEFER_2_OFFSET              (MIB_TXDEFER_OFFSET + 0x400)
#define MIB_TXDEFER_2_E_LENGTH            4
#define MIB_TXDEFER_2_E_OFFSET            0xa0
#define MIB_TXDEFER_2_NR_E                6

#define MIB_TXLATECOL_2                   "TxLateCol_2"
#define MIB_TXLATECOL_2_ID                73
#define MIB_TXLATECOL_2_OFFSET            (MIB_TXLATECOL_OFFSET + 0x400)
#define MIB_TXLATECOL_2_E_LENGTH          4
#define MIB_TXLATECOL_2_E_OFFSET          0xa0
#define MIB_TXLATECOL_2_NR_E              6

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ATHENA_REG_H */

