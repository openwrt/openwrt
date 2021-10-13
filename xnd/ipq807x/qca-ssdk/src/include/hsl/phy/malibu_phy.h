/*
 * Copyright (c) 2015, 2017, 2019, The Linux Foundation. All rights reserved.
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

#ifndef _MALIBU_PHY_H_
#define _MALIBU_PHY_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#define BIT_15                   15
#define BIT_14                   14
#define BIT_13                   13
#define BIT_12                   12
#define BIT_11                   11
#define BIT_10                   10
#define BIT_9                    9
#define BIT_8                    8
#define BIT_7                    7
#define BIT_6                    6
#define BIT_5                    5
#define BIT_4                    4
#define BIT_3                    3
#define BIT_2                    2
#define BIT_1                    1
#define BIT_0                    0
#define COMBO_PHY_ID 4
#define PSGMII_ID 5

#define MALIBU_COMMON_CTRL  0x1040
#define MALIBU_10M_LOOPBACK  0x4100
#define MALIBU_100M_LOOPBACK  0x6100
#define MALIBU_1000M_LOOPBACK  0x4140

#define MALIBU_1_0 0x004DD0B0
#define MALIBU_1_1 0x004DD0B1
#define MALIBU_1_1_2PORT 0x004DD0B2
#define MALIBU_ORG_ID_OFFSET_LEN 16
#define MALIBU_PHY_COPPER_MODE 0x8000

  /* PHY Registers */
#define MALIBU_PHY_CONTROL                      0
#define MALIBU_PHY_STATUS                       1
#define MALIBU_PHY_ID1                          2
#define MALIBU_PHY_ID2                          3
#define MALIBU_AUTONEG_ADVERT                   4
#define MALIBU_LINK_PARTNER_ABILITY             5
#define MALIBU_AUTONEG_EXPANSION                6
#define MALIBU_NEXT_PAGE_TRANSMIT               7
#define MALIBU_LINK_PARTNER_NEXT_PAGE           8
#define MALIBU_1000BASET_CONTROL                9
#define MALIBU_1000BASET_STATUS                 10
#define MALIBU_MMD_CTRL_REG                     13
#define MALIBU_MMD_DATA_REG                     14
#define MALIBU_EXTENDED_STATUS                  15
#define MALIBU_PHY_SPEC_CONTROL                 16
#define MALIBU_PHY_SPEC_STATUS                  17
#define MALIBU_PHY_INTR_MASK                    18
#define MALIBU_PHY_INTR_STATUS                  19
#define MALIBU_PHY_CDT_CONTROL                  22
#define MALIBU_PHY_CDT_STATUS                   28
#define MALIBU_DEBUG_PORT_ADDRESS               29
#define MALIBU_DEBUG_PORT_DATA                  30

#define MALIBU_DEBUG_PHY_HIBERNATION_CTRL  0xb
#define MALIBU_DEBUG_PHY_POWER_SAVING_CTRL  0x29
#define MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_CTRL       0x3c
#define MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_PARTNER       0x3d
#define MALIBU_PHY_MMD7_ADDR_8023AZ_EEE_STATUS       0x8000
#define MALIBU_PHY_MMD3_ADDR_8023AZ_EEE_CAPABILITY       0x14

#define MALIBU_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL       0x805a
#define MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL1   0x804a
#define MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL2   0x804b
#define MALIBU_PHY_MMD3_WOL_MAGIC_MAC_CTRL3   0x804c
#define MALIBU_PHY_MMD3_WOL_CTRL  0x8012
#define MALIBU_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL       0x804e
#define MALIBU_PHY_MMD3_ADDR_8023AZ_CLD_CTRL       0x8007
#define MALIBU_PHY_MMD3_ADDR_CLD_CTRL5       0x8005
#define MALIBU_PHY_MMD3_ADDR_CLD_CTRL3       0x8003
#define MALIBU_PHY_MMD7_DAC_CTRL  0x801a
#define MALIBU_DAC_CTRL_MASK  0x380
#define MALIBU_DAC_CTRL_VALUE  0x280
#define MALIBU_LED_1000_CTRL1_100_10_MASK  0x30

#define MALIBU_PHY_EEE_ADV_100M 0x0002
#define MALIBU_PHY_EEE_ADV_1000M 0x0004
#define MALIBU_PHY_EEE_PARTNER_ADV_100M 0x0002
#define MALIBU_PHY_EEE_PARTNER_ADV_1000M 0x0004
#define MALIBU_PHY_EEE_CAPABILITY_100M 0x0002
#define MALIBU_PHY_EEE_CAPABILITY_1000M 0x0004
#define MALIBU_PHY_EEE_STATUS_100M 0x0002
#define MALIBU_PHY_EEE_STATUS_1000M 0x0004


#define AZ_TIMER_CTRL_DEFAULT_VALUE    0x3062
#define AZ_CLD_CTRL_DEFAULT_VALUE     0x83f6
#define AZ_TIMER_CTRL_ADJUST_VALUE    0x7062
#define AZ_CLD_CTRL_ADJUST_VALUE     0x8396


#define MALIBU_PHY_MMD7_COUNTER_CTRL       0x8029
#define MALIBU_PHY_MMD7_INGRESS_COUNTER_HIGH       0x802a
#define MALIBU_PHY_MMD7_INGRESS_COUNTER_LOW       0x802b
#define MALIBU_PHY_MMD7_INGRESS_ERROR_COUNTER       0x802c
#define MALIBU_PHY_MMD7_EGRESS_COUNTER_HIGH       0x802d
#define MALIBU_PHY_MMD7_EGRESS_COUNTER_LOW       0x802e
#define MALIBU_PHY_MMD7_EGRESS_ERROR_COUNTER       0x802f
#define MALIBU_PHY_MMD7_LED_1000_CTRL1       0x8076



#define MALIBU_PSGMII_FIFI_CTRL  0x6e
#define MALIBU_PSGMII_CALIB_CTRL  0x27
#define MALIBU_PSGMII_MODE_CTRL  0x6d
#define MALIBU_PSGMII_TX_DRIVER_1_CTRL 0xb

#define MALIBU_PHY_PSGMII_MODE_CTRL_DEFAULT_VALUE       0x220d
#define MALIBU_PHY_PSGMII_MODE_CTRL_ADJUST_VALUE        0x220c
#define MALIBU_PHY_PSGMII_REDUCE_SERDES_TX_AMP	0x8a

#define MALIBU_PHY_QSGMII 0x8504
#define MALIBU_PHY_PSGMII_ADDR_INC 0x5
#define MALIBU_PHY_MAX_ADDR_INC 0x4
#define MALIBU_MODE_CHANAGE_RESET 0x0
#define MALIBU_MODE_RESET_DEFAULT_VALUE 0x5f
#define MALIBU_MODE_RESET_REG 0x0

#define MALIBU_PHY_TX_FLOWCTRL_STATUS 0x8
#define MALIBU_PHY_RX_FLOWCTRL_STATUS 0x4


#define MALIBU_PHY_MMD7_NUM  7
#define MALIBU_PHY_MMD3_NUM  3
#define MALIBU_PHY_MMD1_NUM  1

#define MALIBU_PHY_SGMII_STATUS            0x1a	/* sgmii_status  Register  */
#define MALIBU_PHY4_AUTO_SGMII_SELECT   0x40
#define MALIBU_PHY4_AUTO_COPPER_SELECT       0x20
#define MALIBU_PHY4_AUTO_BX1000_SELECT     0x10
#define MALIBU_PHY4_AUTO_FX100_SELECT   0x8

#define MALIBU_PHY_CHIP_CONFIG          0x1f	/* Chip Configuration Register  */
#define BT_BX_SG_REG_SELECT          BIT_15
#define BT_BX_SG_REG_SELECT_OFFSET   15
#define BT_BX_SG_REG_SELECT_LEN      1
#define MALIBU_SG_BX_PAGES                  0x0
#define MALIBU_SG_COPPER_PAGES              0x1

#define MALIBU_PHY_PSGMII_BASET              0x0
#define MALIBU_PHY_PSGMII_BX1000              0x1
#define MALIBU_PHY_PSGMII_FX100              0x2
#define MALIBU_PHY_PSGMII_AMDET              0x3
#define MALIBU_PHY_SGMII_BASET              0x4

#define MALIBU_PHY4_PREFER_FIBER  0x400
#define PHY4_PREFER_COPPER                0x0
#define PHY4_PREFER_FIBER                 0x1

#define MALIBU_PHY4_FIBER_MODE_1000BX      0x100
#define AUTO_100FX_FIBER             0x0
#define AUTO_1000BX_FIBER            0x1

#define MALIBU_PHY_MDIX     0x0020
#define MALIBU_PHY_MDIX_AUTO     0x0060
#define MALIBU_PHY_MDIX_STATUS     0x0040

#define MODE_CFG_QUAL                BIT_4
#define MODE_CFG_QUAL_OFFSET         4
#define MODE_CFG_QUAL_LEN            4

#define MODE_CFG                     BIT_0
#define MODE_CFG_OFFSET              0
#define MODE_CFG_LEN                 4

#define MALIBU_MODECTRL_DFLT	0x533
#define MALIBU_MIICTRL_DFLT	0x140

  /*debug port */
#define MALIBU_DEBUG_PORT_RGMII_MODE            18
#define MALIBU_DEBUG_PORT_RGMII_MODE_EN         0x0008

#define MALIBU_DEBUG_PORT_RX_DELAY            0
#define MALIBU_DEBUG_PORT_RX_DELAY_EN         0x8000

#define MALIBU_DEBUG_PORT_TX_DELAY            5
#define MALIBU_DEBUG_PORT_TX_DELAY_EN         0x0100

  /* PHY Registers Field */

  /* Control Register fields  offset:0 */
  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define MALIBU_CTRL_SPEED_MSB                   0x0040

  /* Collision test enable */
#define MALIBU_CTRL_COLL_TEST_ENABLE            0x0080

  /* FDX =1, half duplex =0 */
#define MALIBU_CTRL_FULL_DUPLEX                 0x0100

  /* Restart auto negotiation */
#define MALIBU_CTRL_RESTART_AUTONEGOTIATION     0x0200

  /* Isolate PHY from MII */
#define MALIBU_CTRL_ISOLATE                     0x0400

  /* Power down */
#define MALIBU_CTRL_POWER_DOWN                  0x0800

  /* Auto Neg Enable */
#define MALIBU_CTRL_AUTONEGOTIATION_ENABLE      0x1000

  /* Local Loopback Enable */
#define MALIBU_LOCAL_LOOPBACK_ENABLE      0x4000

  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define MALIBU_CTRL_SPEED_LSB                   0x2000

  /* 0 = normal, 1 = loopback */
#define MALIBU_CTRL_LOOPBACK                    0x4000
#define MALIBU_CTRL_SOFTWARE_RESET              0x8000

#define MALIBU_CTRL_SPEED_MASK                  0x2040
#define MALIBU_CTRL_SPEED_1000                  0x0040
#define MALIBU_CTRL_SPEED_100                   0x2000
#define MALIBU_CTRL_SPEED_10                    0x0000

#define MALIBU_RESET_DONE(phy_control)                   \
    (((phy_control) & (MALIBU_CTRL_SOFTWARE_RESET)) == 0)

  /* Status Register fields offset:1 */
  /* Extended register capabilities */
#define MALIBU_STATUS_EXTENDED_CAPS             0x0001

  /* Jabber Detected */
#define MALIBU_STATUS_JABBER_DETECT             0x0002

  /* Link Status 1 = link */
#define MALIBU_STATUS_LINK_STATUS_UP            0x0004

  /* Auto Neg Capable */
#define MALIBU_STATUS_AUTONEG_CAPS              0x0008

  /* Remote Fault Detect */
#define MALIBU_STATUS_REMOTE_FAULT              0x0010

  /* Auto Neg Complete */
#define MALIBU_STATUS_AUTO_NEG_DONE             0x0020

  /* Preamble may be suppressed */
#define MALIBU_STATUS_PREAMBLE_SUPPRESS         0x0040

  /* Ext. status info in Reg 0x0F */
#define MALIBU_STATUS_EXTENDED_STATUS           0x0100

  /* 100T2 Half Duplex Capable */
#define MALIBU_STATUS_100T2_HD_CAPS             0x0200

  /* 100T2 Full Duplex Capable */
#define MALIBU_STATUS_100T2_FD_CAPS             0x0400

  /* 10T   Half Duplex Capable */
#define MALIBU_STATUS_10T_HD_CAPS               0x0800

  /* 10T   Full Duplex Capable */
#define MALIBU_STATUS_10T_FD_CAPS               0x1000

  /* 100X  Half Duplex Capable */
#define MALIBU_STATUS_100X_HD_CAPS              0x2000

  /* 100X  Full Duplex Capable */
#define MALIBU_STATUS_100X_FD_CAPS              0x4000

  /* 100T4 Capable */
#define MALIBU_STATUS_100T4_CAPS                0x8000

  /* extended status register capabilities */

#define MALIBU_STATUS_1000T_HD_CAPS             0x1000

#define MALIBU_STATUS_1000T_FD_CAPS             0x2000

#define MALIBU_STATUS_1000X_HD_CAPS             0x4000

#define MALIBU_STATUS_1000X_FD_CAPS             0x8000

#define MALIBU_AUTONEG_DONE(ip_phy_status) \
    (((ip_phy_status) & (MALIBU_STATUS_AUTO_NEG_DONE)) ==  \
        (MALIBU_STATUS_AUTO_NEG_DONE))

  /* PHY identifier1  offset:2 */
//Organizationally Unique Identifier bits 3:18

  /* PHY identifier2  offset:3 */
//Organizationally Unique Identifier bits 19:24

  /* Auto-Negotiation Advertisement register. offset:4 */
  /* indicates IEEE 802.3 CSMA/CD */
#define MALIBU_ADVERTISE_SELECTOR_FIELD         0x0001

  /* 10T   Half Duplex Capable */
#define MALIBU_ADVERTISE_10HALF                 0x0020

  /* 10T   Full Duplex Capable */
#define MALIBU_ADVERTISE_10FULL                 0x0040

  /* 100TX Half Duplex Capable */
#define MALIBU_ADVERTISE_100HALF                0x0080

  /* 100TX Full Duplex Capable */
#define MALIBU_ADVERTISE_100FULL                0x0100

  /* 100T4 Capable */
#define MALIBU_ADVERTISE_100T4                  0x0200

  /* Pause operation desired */
#define MALIBU_ADVERTISE_PAUSE                  0x0400

  /* Asymmetric Pause Direction bit */
#define MALIBU_ADVERTISE_ASYM_PAUSE             0x0800

  /* Remote Fault detected */
#define MALIBU_ADVERTISE_REMOTE_FAULT           0x2000

  /* Next Page ability supported */
#define MALIBU_ADVERTISE_NEXT_PAGE              0x8000

  /* 100TX Half Duplex Capable */
#define MALIBU_ADVERTISE_1000HALF               0x0100

  /* 100TX Full Duplex Capable */
#define MALIBU_ADVERTISE_1000FULL               0x0200

  /* Extended next page enable control  */
#define MALIBU_EXTENDED_NEXT_PAGE_EN            0x1000

#define MALIBU_ADVERTISE_ALL \
    (MALIBU_ADVERTISE_10HALF | MALIBU_ADVERTISE_10FULL | \
     MALIBU_ADVERTISE_100HALF | MALIBU_ADVERTISE_100FULL | \
     MALIBU_ADVERTISE_1000FULL)

#define MALIBU_ADVERTISE_MEGA_ALL \
    (MALIBU_ADVERTISE_10HALF | MALIBU_ADVERTISE_10FULL | \
     MALIBU_ADVERTISE_100HALF | MALIBU_ADVERTISE_100FULL)

#define MALIBU_BX_ADVERTISE_1000FULL                0x0020
#define MALIBU_BX_ADVERTISE_1000HALF                0x0040
#define MALIBU_BX_ADVERTISE_PAUSE                      0x0080
#define MALIBU_BX_ADVERTISE_ASYM_PAUSE           0x0100

#define MALIBU_BX_ADVERTISE_ALL \
    (MALIBU_BX_ADVERTISE_ASYM_PAUSE | MALIBU_BX_ADVERTISE_PAUSE | \
     MALIBU_BX_ADVERTISE_1000HALF | MALIBU_BX_ADVERTISE_1000FULL)

  /* Link Partner ability offset:5 */
  /* Same as advertise selector  */
#define MALIBU_LINK_SLCT                        0x001f

  /* Can do 10mbps half-duplex   */
#define MALIBU_LINK_10BASETX_HALF_DUPLEX        0x0020

  /* Can do 10mbps full-duplex   */
#define MALIBU_LINK_10BASETX_FULL_DUPLEX        0x0040

  /* Can do 100mbps half-duplex  */
#define MALIBU_LINK_100BASETX_HALF_DUPLEX       0x0080

  /* Can do 100mbps full-duplex  */
#define MALIBU_LINK_100BASETX_FULL_DUPLEX       0x0100

  /* Can do 1000mbps full-duplex  */
#define MALIBU_LINK_1000BASETX_FULL_DUPLEX       0x0800

  /* Can do 1000mbps half-duplex  */
#define MALIBU_LINK_1000BASETX_HALF_DUPLEX       0x0400

  /* 100BASE-T4  */
#define MALIBU_LINK_100BASE4                    0x0200

  /* PAUSE */
#define MALIBU_LINK_PAUSE                       0x0400

  /* Asymmetrical PAUSE */
#define MALIBU_LINK_ASYPAUSE                    0x0800

  /* Link partner faulted  */
#define MALIBU_LINK_RFAULT                      0x2000

  /* Link partner acked us */
#define MALIBU_LINK_LPACK                       0x4000

  /* Next page bit  */
#define MALIBU_LINK_NPAGE                       0x8000

  /* Auto-Negotiation Expansion Register offset:6 */

  /* Next Page Transmit Register offset:7 */

  /* Link partner Next Page Register offset:8 */

  /* 1000BASE-T Control Register offset:9 */
  /* Advertise 1000T HD capability */
#define MALIBU_CTL_1000T_HD_CAPS                0x0100

  /* Advertise 1000T FD capability  */
#define MALIBU_CTL_1000T_FD_CAPS                0x0200

  /* 1=Repeater/switch device port 0=DTE device */
#define MALIBU_CTL_1000T_REPEATER_DTE           0x0400

  /* 1=Configure PHY as Master  0=Configure PHY as Slave */
#define MALIBU_CTL_1000T_MS_VALUE               0x0800

  /* 1=Master/Slave manual config value  0=Automatic Master/Slave config */
#define MALIBU_CTL_1000T_MS_ENABLE              0x1000

  /* Normal Operation */
#define MALIBU_CTL_1000T_TEST_MODE_NORMAL       0x0000

  /* Transmit Waveform test */
#define MALIBU_CTL_1000T_TEST_MODE_1            0x2000

  /* Master Transmit Jitter test */
#define MALIBU_CTL_1000T_TEST_MODE_2            0x4000

  /* Slave Transmit Jitter test */
#define MALIBU_CTL_1000T_TEST_MODE_3            0x6000

  /* Transmitter Distortion test */
#define MALIBU_CTL_1000T_TEST_MODE_4            0x8000
#define MALIBU_CTL_1000T_SPEED_MASK             0x0300
#define MALIBU_CTL_1000T_DEFAULT_CAP_MASK       0x0300

  /* 1000BASE-T Status Register offset:10 */
  /* LP is 1000T HD capable */
#define MALIBU_STATUS_1000T_LP_HD_CAPS          0x0400

  /* LP is 1000T FD capable */
#define MALIBU_STATUS_1000T_LP_FD_CAPS          0x0800

  /* Remote receiver OK */
#define MALIBU_STATUS_1000T_REMOTE_RX_STATUS    0x1000

  /* Local receiver OK */
#define MALIBU_STATUS_1000T_LOCAL_RX_STATUS     0x2000

  /* 1=Local TX is Master, 0=Slave */
#define MALIBU_STATUS_1000T_MS_CONFIG_RES       0x4000

#define MALIBU_STATUS_1000T_MS_CONFIG_FAULT     0x8000

  /* Master/Slave config fault */
#define MALIBU_STATUS_1000T_REMOTE_RX_STATUS_SHIFT   12
#define MALIBU_STATUS_1000T_LOCAL_RX_STATUS_SHIFT    13

  /* Phy Specific Control Register offset:16 */
  /* 1=Jabber Function disabled */
#define MALIBU_CTL_JABBER_DISABLE               0x0001

  /* 1=Polarity Reversal enabled */
#define MALIBU_CTL_POLARITY_REVERSAL            0x0002

  /* 1=SQE Test enabled */
#define MALIBU_CTL_SQE_TEST                     0x0004
#define MALIBU_CTL_MAC_POWERDOWN                0x0008

  /* 1=CLK125 low, 0=CLK125 toggling
     #define MALIBU_CTL_CLK125_DISABLE               0x0010
   */
  /* MDI Crossover Mode bits 6:5 */
  /* Manual MDI configuration */
#define MALIBU_CTL_MDI_MANUAL_MODE              0x0000

  /* Manual MDIX configuration */
#define MALIBU_CTL_MDIX_MANUAL_MODE             0x0020

  /* 1000BASE-T: Auto crossover, 100BASE-TX/10BASE-T: MDI Mode */
#define MALIBU_CTL_AUTO_X_1000T                 0x0040

  /* Auto crossover enabled all speeds */
#define MALIBU_CTL_AUTO_X_MODE                  0x0060

  /* 1=Enable Extended 10BASE-T distance
   * (Lower 10BASE-T RX Threshold)
   * 0=Normal 10BASE-T RX Threshold */
#define MALIBU_CTL_10BT_EXT_DIST_ENABLE         0x0080

  /* 1=5-Bit interface in 100BASE-TX
   * 0=MII interface in 100BASE-TX */
#define MALIBU_CTL_MII_5BIT_ENABLE              0x0100

  /* 1=Scrambler disable */
#define MALIBU_CTL_SCRAMBLER_DISABLE            0x0200

  /* 1=Force link good */
#define MALIBU_CTL_FORCE_LINK_GOOD              0x0400

  /* 1=Assert CRS on Transmit */
#define MALIBU_CTL_ASSERT_CRS_ON_TX             0x0800

#define MALIBU_CTL_POLARITY_REVERSAL_SHIFT      1
#define MALIBU_CTL_AUTO_X_MODE_SHIFT            5
#define MALIBU_CTL_10BT_EXT_DIST_ENABLE_SHIFT   7

  /* Phy Specific status fields offset:17 */
  /* 1=Speed & Duplex resolved */
#define MALIBU_STATUS_LINK_PASS                 0x0400
#define MALIBU_STATUS_RESOVLED                  0x0800

  /* 1=Duplex 0=Half Duplex */
#define MALIBU_STATUS_FULL_DUPLEX               0x2000

  /* Speed, bits 14:15 */
#define MALIBU_STATUS_SPEED                    0xC000
#define MALIBU_STATUS_SPEED_MASK               0xC000

  /* 00=10Mbs */
#define MALIBU_STATUS_SPEED_10MBS              0x0000

  /* 01=100Mbs */
#define MALIBU_STATUS_SPEED_100MBS             0x4000

  /* 10=1000Mbs */
#define MALIBU_STATUS_SPEED_1000MBS            0x8000
#define MALIBU_SPEED_DUPLEX_RESOVLED(phy_status)                   \
    (((phy_status) &                                  \
        (MALIBU_STATUS_RESOVLED)) ==                    \
        (MALIBU_STATUS_RESOVLED))

  /*phy debug port1 register offset:29 */
  /*phy debug port2 register offset:30 */

  /*MALIBU interrupt flag */
#define MALIBU_INTR_SPEED_CHANGE              0x4000
#define MALIBU_INTR_DUPLEX_CHANGE             0x2000
#define MALIBU_INTR_STATUS_UP_CHANGE          0x0400
#define MALIBU_INTR_STATUS_DOWN_CHANGE        0x0800
#define MALIBU_INTR_BX_FX_STATUS_DOWN_CHANGE        0x0100
#define MALIBU_INTR_BX_FX_STATUS_UP_CHANGE        0x0080
#define MALIBU_INTR_MEDIA_STATUS_CHANGE             0x1000
#define MALIBU_INTR_WOL             0x0001
#define MALIBU_INTR_POE             0x0002

#define RUN_CDT 0x8000
#define CABLE_LENGTH_UNIT 0x0400

/** Phy preferred medium type */
  typedef enum
  {
    MALIBU_PHY_MEDIUM_COPPER = 0,
				       /**< Copper */
    MALIBU_PHY_MEDIUM_FIBER = 1,
				       /**< Fiber */

  } malibu_phy_medium_t;

/** Phy pages */
  typedef enum
  {
    MALIBU_PHY_SGBX_PAGES = 0,
				       /**< sgbx pages */
    MALIBU_PHY_COPPER_PAGES = 1
				       /**< copper pages */
  } malibu_phy_reg_pages_t;
#ifndef IN_PORTCONTROL_MINI
    sw_error_t
    malibu_phy_set_powersave (a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t enable);

    sw_error_t
    malibu_phy_get_powersave (a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t * enable);

    sw_error_t
    malibu_phy_set_hibernate (a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t enable);

    sw_error_t
    malibu_phy_get_hibernate (a_uint32_t dev_id, a_uint32_t phy_id,
			      a_bool_t * enable);

    sw_error_t
    malibu_phy_cdt (a_uint32_t dev_id, a_uint32_t phy_id,
		    a_uint32_t mdi_pair,
		    fal_cable_status_t * cable_status,
		    a_uint32_t * cable_len);
#endif
    sw_error_t
    malibu_phy_set_duplex (a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_duplex_t duplex);

    sw_error_t
    malibu_phy_get_duplex (a_uint32_t dev_id, a_uint32_t phy_id,
			   fal_port_duplex_t * duplex);

    sw_error_t
    malibu_phy_set_speed (a_uint32_t dev_id, a_uint32_t phy_id,
			  fal_port_speed_t speed);

    sw_error_t
    malibu_phy_get_speed (a_uint32_t dev_id, a_uint32_t phy_id,
			  fal_port_speed_t * speed);

    sw_error_t
    malibu_phy_restart_autoneg (a_uint32_t dev_id, a_uint32_t phy_id);

    sw_error_t
    malibu_phy_enable_autoneg (a_uint32_t dev_id, a_uint32_t phy_id);

    a_bool_t
    malibu_phy_get_link_status (a_uint32_t dev_id, a_uint32_t phy_id);

    sw_error_t
    malibu_phy_set_autoneg_adv (a_uint32_t dev_id, a_uint32_t phy_id,
				a_uint32_t autoneg);

    sw_error_t
    malibu_phy_get_autoneg_adv (a_uint32_t dev_id, a_uint32_t phy_id,
				a_uint32_t * autoneg);

    a_bool_t malibu_phy_autoneg_status (a_uint32_t dev_id, a_uint32_t phy_id);
#ifndef IN_PORTCONTROL_MINI
    sw_error_t
    malibu_phy_intr_mask_set (a_uint32_t dev_id, a_uint32_t phy_id,
			      a_uint32_t intr_mask_flag);

    sw_error_t
    malibu_phy_intr_mask_get (a_uint32_t dev_id, a_uint32_t phy_id,
			      a_uint32_t * intr_mask_flag);

    sw_error_t
    malibu_phy_intr_status_get (a_uint32_t dev_id, a_uint32_t phy_id,
				a_uint32_t * intr_status_flag);

  sw_error_t
  malibu_phy_set_counter (a_uint32_t dev_id, a_uint32_t phy_id,
				a_bool_t enable);
  
  sw_error_t
  malibu_phy_get_counter (a_uint32_t dev_id, a_uint32_t phy_id,
				a_bool_t * enable);

  sw_error_t
  malibu_phy_show_counter (a_uint32_t dev_id, a_uint32_t phy_id,
			 fal_port_counter_info_t * counter_info);
#endif
  sw_error_t
  malibu_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id,
			a_bool_t * enable);
#ifndef IN_PORTCONTROL_MINI
  sw_error_t
  malibu_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id,
			a_bool_t enable);

  sw_error_t
  malibu_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		  a_uint32_t *phy_data);
#endif
  int malibu_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp);

#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _MALIBU_PHY_H_ */
