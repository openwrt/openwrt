/*
 * Copyright (c) 2018, 2020, The Linux Foundation. All rights reserved.
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

#ifndef _QCA808X_PHY_H_
#define _QCA808X_PHY_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#define QCA808X_MII_ADDR_C45  (1<<30)
#define QCA808X_REG_C45_ADDRESS(dev_type, reg_num) (QCA808X_MII_ADDR_C45 | \
			((dev_type & 0x1f) << 16) | (reg_num & 0xffff))

#define QCA808X_COMMON_CTRL  0x1040

#define QCA808X_PHY_MMD1_PMA_CONTROL       0x0
#define QCA808X_PMA_CONTROL_SPEED_MASK     0x2040
#define QCA808X_PMA_CONTROL_2500M          0x2040
#define QCA808X_PMA_CONTROL_1000M          0x40
#define QCA808X_PMA_CONTROL_100M           0x2000
#define QCA808X_PMA_CONTROL_10M            0x0

#define QCA808X_PHY_MMD1_PMA_TYPE       0x7
#define QCA808X_PMA_TYPE_MASK           0x3f
#define QCA808X_PMA_TYPE_2500M          0x30
#define QCA808X_PMA_TYPE_1000M          0xc
#define QCA808X_PMA_TYPE_100M           0xe
#define QCA808X_PMA_TYPE_10M            0xf

  /* PHY Registers */
#define QCA808X_PHY_CONTROL                      0
#define QCA808X_PHY_STATUS                       1
#define QCA808X_PHY_SPEC_STATUS                  17

#define QCA808X_PHY_ID1                          2
#define QCA808X_PHY_ID2                          3
#define QCA808X_AUTONEG_ADVERT                   4
#define QCA808X_LINK_PARTNER_ABILITY             5
#define QCA808X_1000BASET_CONTROL                9
#define QCA808X_1000BASET_STATUS                 10
#define QCA808X_MMD_CTRL_REG                     13
#define QCA808X_MMD_DATA_REG                     14
#define QCA808X_EXTENDED_STATUS                  15
#define QCA808X_PHY_SPEC_CONTROL                 16
#define QCA808X_PHY_INTR_MASK                    18
#define QCA808X_PHY_INTR_STATUS                  19
#define QCA808X_PHY_CDT_CONTROL                  22
#define QCA808X_DEBUG_PORT_ADDRESS               29
#define QCA808X_DEBUG_PORT_DATA                  30
#define QCA808X_DEBUG_LOCAL_SEED                 9

/* Chip Configuration Register */
#define QCA808X_PHY_CHIP_CONFIG                  31

#define QCA808X_PHY_MODE_MASK              0x6000
#define QCA808X_PHY_SGMII_MODE             0x0000
#define QCA808X_PHY_SGMII_PLUS_MODE        0x2000
#define QCA808X_PHY_SGMII_BASET            0x4
#define QCA808X_PHY_CHIP_MODE_CFG          0xf

#define QCA808X_DEBUG_PHY_HIBERNATION_CTRL          0xb
#define QCA808X_DEBUG_PHY_HIBERNATION_STAT          0xc
#define QCA808X_DEBUG_PHY_POWER_SAVING_CTRL         0x29
#define QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL       0x3c
#define QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_PARTNER    0x3d
#define QCA808X_PHY_MMD7_ADDR_8023AZ_EEE_STATUS     0x8000
#define QCA808X_PHY_MMD3_ADDR_8023AZ_EEE_CAPABILITY 0x14
#define QCA808X_PHY_MMD7_ADDR_EEE_LP_ADVERTISEMENT  0x40

#define QCA808X_PHY_MMD3_ADDR_8023AZ_EEE_DB         0x800f
#define QCA808X_PHY_8023AZ_EEE_LP_STAT              0x2000
#define QCA808X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL  0x805a
#define QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL1        0x804a
#define QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL2        0x804b
#define QCA808X_PHY_MMD3_WOL_MAGIC_MAC_CTRL3        0x804c
#define QCA808X_PHY_MMD3_WOL_CTRL                   0x8012
#define QCA808X_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL     0x804e
#define QCA808X_PHY_MMD3_ADDR_CLD_CTRL7             0x8007
#define QCA808X_PHY_MMD3_ADDR_CLD_CTRL5             0x8005
#define QCA808X_PHY_MMD3_ADDR_CLD_CTRL3             0x8003
#define QCA808X_PHY_MMD3_AZ_TRAINING_CTRL           0x8008

#define QCA808X_PHY_HIBERNATION_CFG       0x8000
#define QCA808X_PHY_HIBERNATION_STAT_EN   0x0800

#define QCA808X_PHY_WOL_EN                0x0020

#define QCA808X_PHY_REMOTE_LOOPBACK_EN    0x0001

#define QCA808X_PHY_8023AZ_EEE_1000BT     0x0004
#define QCA808X_PHY_8023AZ_EEE_100BT      0x0002
#define QCA808X_PHY_MMD3_AZ_TRAINING_VAL  0x1c32

#define QCA808X_PHY_MDIX          0x0020
#define QCA808X_PHY_MDIX_AUTO     0x0060
#define QCA808X_PHY_MDIX_STATUS   0x0040

#define QCA808X_PHY_MMD7_NUM  7
#define QCA808X_PHY_MMD3_NUM  3
#define QCA808X_PHY_MMD1_NUM  1

#define QCA808X_PHY_MMD1_FAST_RETRAIN_STATUS_CTL 0x93
#define QCA808X_PHY_MMD1_MSE_THRESHOLD_20DB      0x8014
#define QCA808X_PHY_MMD1_MSE_THRESHOLD_17DB      0x800E
#define QCA808X_PHY_MMD1_MSE_THRESHOLD_27DB      0x801E
#define QCA808X_PHY_MMD1_MSE_THRESHOLD_28DB      0x8020
#define QCA808X_PHY_MMD7_TOP_OPTION1             0x901c
#define QCA808X_PHY_EEE_ADV_100M 0x0002
#define QCA808X_PHY_EEE_ADV_1000M 0x0004
#define QCA808X_PHY_EEE_PARTNER_ADV_100M 0x0002
#define QCA808X_PHY_EEE_PARTNER_ADV_1000M 0x0004
#define QCA808X_PHY_EEE_CAPABILITY_100M 0x0002
#define QCA808X_PHY_EEE_CAPABILITY_1000M 0x0004
#define QCA808X_PHY_EEE_STATUS_100M 0x0002
#define QCA808X_PHY_EEE_STATUS_1000M 0x0004

#define QCA808X_PHY_FAST_RETRAIN_CTRL           0x1
#define QCA808X_PHY_MSE_THRESHOLD_20DB_VALUE    0x529
#define QCA808X_PHY_MSE_THRESHOLD_17DB_VALUE    0x341
#define QCA808X_PHY_MSE_THRESHOLD_27DB_VALUE    0x419
#define QCA808X_PHY_MSE_THRESHOLD_28DB_VALUE    0x341
#define QCA808X_PHY_FAST_RETRAIN_2500BT         0x20
#define QCA808X_PHY_ADV_LOOP_TIMING             0x1
#define QCA808X_PHY_EEE_ADV_THP                 0x8
#define QCA808X_PHY_TOP_OPTION1_DATA            0x0

	/* CDT */
#define QCA808X_MDI_PAIR_NUM            4
#define QCA808X_RUN_CDT                 0x8000
#define QCA808X_CABLE_LENGTH_UNIT       0x0400
#define QCA808X_PHY_CDT_STATUS          0X8064
#define QCA808X_PHY_CDT_DIAG_PAIR0      0X8065
#define QCA808X_PHY_CDT_DIAG_PAIR1      0X8066
#define QCA808X_PHY_CDT_DIAG_PAIR2      0X8067
#define QCA808X_PHY_CDT_DIAG_PAIR3      0X8068

	/* SYNCE CLOCK OUTPUT */
#define QCA808X_DEBUG_ANA_CLOCK_CTRL_REG   0x3e80
#define QCA808X_ANALOG_PHY_SYNCE_CLOCK_EN  0x20

#define QCA808X_MMD7_CLOCK_CTRL_REG        0x8072
#define QCA808X_DIGITAL_PHY_SYNCE_CLOCK_EN 0x1

  /* PHY Registers Field */
#define QCA808X_STATUS_LINK_PASS                 0x0400

  /* Control Register fields  offset:0 */
  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define QCA808X_CTRL_SPEED_MSB                   0x0040

  /* Collision test enable */
#define QCA808X_CTRL_COLL_TEST_ENABLE            0x0080
/* FDX =1, half duplex =0 */
#define QCA808X_CTRL_FULL_DUPLEX                 0x0100

  /* Restart auto negotiation */
#define QCA808X_CTRL_RESTART_AUTONEGOTIATION     0x0200

  /* Power down */
#define QCA808X_CTRL_POWER_DOWN                  0x0800

  /* Auto Neg Enable */
#define QCA808X_CTRL_AUTONEGOTIATION_ENABLE      0x1000

  /* Local Loopback Enable */
#define QCA808X_LOCAL_LOOPBACK_ENABLE            0x4000

  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define QCA808X_CTRL_SPEED_LSB                   0x2000

  /* 0 = normal, 1 = loopback */
#define QCA808X_CTRL_LOOPBACK                    0x4000
#define QCA808X_CTRL_SOFTWARE_RESET              0x8000

#define QCA808X_PHY_MMD7_AUTONEGOTIATION_CONTROL 0x20

#define QCA808X_CTRL_SPEED_MASK                  0x2040
#define QCA808X_CTRL_SPEED_1000                  0x0040
#define QCA808X_CTRL_SPEED_100                   0x2000
#define QCA808X_CTRL_SPEED_10                    0x0000

#define QCA808X_MASTER_SLAVE_SEED_ENABLE         0x2
#define QCA808X_MASTER_SLAVE_SEED_CFG            0x1FFC
#define QCA808X_MASTER_SLAVE_SEED_RANGE          0x32
#define QCA808X_MASTER_SLAVE_CONFIG_FAULT        0x8000

#define QCA808X_RESET_DONE(phy_control)                   \
    (((phy_control) & (QCA808X_CTRL_SOFTWARE_RESET)) == 0)

  /* Status Register fields offset:1 */
#define QCA808X_STATUS_EXTENDED_CAPS             0x0001

  /* Jabber Detected */
#define QCA808X_STATUS_JABBER_DETECT             0x0002

  /* Link Status 1 = link */
#define QCA808X_STATUS_LINK_STATUS_UP            0x0004

  /* Auto Neg Capable */
#define QCA808X_STATUS_AUTONEG_CAPS              0x0008

  /* Remote Fault Detect */
#define QCA808X_STATUS_REMOTE_FAULT              0x0010

  /* Auto Neg Complete */
#define QCA808X_STATUS_AUTO_NEG_DONE             0x0020

  /* Preamble may be suppressed */
#define QCA808X_STATUS_PREAMBLE_SUPPRESS         0x0040

  /* Ext. status info in Reg 0x0F */
#define QCA808X_STATUS_EXTENDED_STATUS           0x0100

  /* 100T2 Half Duplex Capable */
#define QCA808X_STATUS_100T2_HD_CAPS             0x0200

  /* 100T2 Full Duplex Capable */
#define QCA808X_STATUS_100T2_FD_CAPS             0x0400

  /* 10T   Half Duplex Capable */
#define QCA808X_STATUS_10T_HD_CAPS               0x0800

  /* 10T   Full Duplex Capable */
#define QCA808X_STATUS_10T_FD_CAPS               0x1000

  /* 100TX  Half Duplex Capable */
#define QCA808X_STATUS_100TX_HD_CAPS              0x2000

  /* 100TX  Full Duplex Capable */
#define QCA808X_STATUS_100TX_FD_CAPS              0x4000

  /* 100T4 Capable */
#define QCA808X_STATUS_100T4_CAPS                0x8000

  /* extended status register capabilities */

#define QCA808X_STATUS_1000T_HD_CAPS             0x1000

#define QCA808X_STATUS_1000T_FD_CAPS             0x2000

#define QCA808X_STATUS_1000X_HD_CAPS             0x4000

#define QCA808X_STATUS_1000X_FD_CAPS             0x8000

#define QCA808X_MMD1_PMA_CAP_REG                 0x4
	/* MMD1 2500T capabilities */
#define QCA808X_STATUS_2500T_FD_CAPS             0x2000

  /* Link Partner ability offset:5 */
#define QCA808X_LINK_SLCT                        0x001f

  /* Can do 10mbps half-duplex   */
#define QCA808X_LINK_10BASETX_HALF_DUPLEX        0x0020

  /* Can do 10mbps full-duplex   */
#define QCA808X_LINK_10BASETX_FULL_DUPLEX        0x0040

  /* Can do 100mbps half-duplex  */
#define QCA808X_LINK_100BASETX_HALF_DUPLEX       0x0080

  /* Can do 100mbps full-duplex  */
#define QCA808X_LINK_100BASETX_FULL_DUPLEX       0x0100

  /* Can do 1000mbps full-duplex  */
#define QCA808X_LINK_1000BASETX_FULL_DUPLEX      0x0800

  /* Can do 1000mbps half-duplex  */
#define QCA808X_LINK_1000BASETX_HALF_DUPLEX      0x0400

  /* Can do 2500mbps full-duplex  */
#define QCA808X_LINK_2500BASETX_FULL_DUPLEX      0x0020

  /* 100BASE-T4  */
#define QCA808X_LINK_100BASE4                    0x0200

  /* PAUSE */
#define QCA808X_LINK_PAUSE                       0x0400

  /* Asymmetrical PAUSE */
#define QCA808X_LINK_ASYPAUSE                    0x0800

  /* Link partner faulted  */
#define QCA808X_LINK_RFAULT                      0x2000

  /* Link partner acked us */
#define QCA808X_LINK_LPACK                       0x4000

  /* Next page bit  */
#define QCA808X_LINK_NPAGE                       0x8000

  /* Auto Neg Complete */
#define QCA808X_STATUS_AUTO_NEG_DONE             0x0020
#define QCA808X_AUTONEG_DONE(ip_phy_status) \
    (((ip_phy_status) & (QCA808X_STATUS_AUTO_NEG_DONE)) ==  \
        (QCA808X_STATUS_AUTO_NEG_DONE))

#define QCA808X_STATUS_RESOVLED                  0x0800
#define QCA808X_SPEED_DUPLEX_RESOVLED(phy_status)     \
    (((phy_status) &                                  \
        (QCA808X_STATUS_RESOVLED)) ==                 \
        (QCA808X_STATUS_RESOVLED))

  /* Auto-Negotiation Advertisement register. offset:4 */
#define QCA808X_ADVERTISE_SELECTOR_FIELD         0x0001

  /* 10T   Half Duplex Capable */
#define QCA808X_ADVERTISE_10HALF                 0x0020

  /* 10T   Full Duplex Capable */
#define QCA808X_ADVERTISE_10FULL                 0x0040

  /* 100TX Half Duplex Capable */
#define QCA808X_ADVERTISE_100HALF                0x0080

  /* 100TX Full Duplex Capable */
#define QCA808X_ADVERTISE_100FULL                0x0100

  /* 100T4 Capable */
#define QCA808X_ADVERTISE_100T4                  0x0200

  /* Pause operation desired */
#define QCA808X_ADVERTISE_PAUSE                  0x0400

  /* Asymmetric Pause Direction bit */
#define QCA808X_ADVERTISE_ASYM_PAUSE             0x0800

  /* Remote Fault detected */
#define QCA808X_ADVERTISE_REMOTE_FAULT           0x2000

  /* 1000TX Half Duplex Capable */
#define QCA808X_ADVERTISE_1000HALF               0x0100

  /* 1000TX Full Duplex Capable */
#define QCA808X_ADVERTISE_1000FULL               0x0200

  /* 2500TX Full Duplex Capable */
#define QCA808X_ADVERTISE_2500FULL               0x80

#define QCA808X_ADVERTISE_ALL \
    (QCA808X_ADVERTISE_10HALF | QCA808X_ADVERTISE_10FULL | \
     QCA808X_ADVERTISE_100HALF | QCA808X_ADVERTISE_100FULL | \
     QCA808X_ADVERTISE_1000FULL)

#define QCA808X_ADVERTISE_MEGA_ALL \
    (QCA808X_ADVERTISE_10HALF | QCA808X_ADVERTISE_10FULL | \
     QCA808X_ADVERTISE_100HALF | QCA808X_ADVERTISE_100FULL | \
     QCA808X_ADVERTISE_PAUSE | QCA808X_ADVERTISE_ASYM_PAUSE)

#define QCA808X_BX_ADVERTISE_1000FULL            0x0020
#define QCA808X_BX_ADVERTISE_1000HALF            0x0040
#define QCA808X_BX_ADVERTISE_PAUSE               0x0080
#define QCA808X_BX_ADVERTISE_ASYM_PAUSE          0x0100

#define QCA808X_BX_ADVERTISE_ALL \
    (QCA808X_BX_ADVERTISE_ASYM_PAUSE | QCA808X_BX_ADVERTISE_PAUSE | \
     QCA808X_BX_ADVERTISE_1000HALF | QCA808X_BX_ADVERTISE_1000FULL)

  /* 1=Duplex 0=Half Duplex */
#define QCA808X_STATUS_FULL_DUPLEX               0x2000
#define QCA808X_PHY_RX_FLOWCTRL_STATUS           0x4
#define QCA808X_PHY_TX_FLOWCTRL_STATUS           0x8

  /* Speed, bits 9:7 */
#define QCA808X_STATUS_SPEED_MASK               0x380

  /* 000=10Mbs */
#define QCA808X_STATUS_SPEED_10MBS              0x0000

  /* 001=100Mbs */
#define QCA808X_STATUS_SPEED_100MBS             0x80

  /* 010=1000Mbs */
#define QCA808X_STATUS_SPEED_1000MBS            0x100

  /* 100=2500Mbs */
#define QCA808X_STATUS_SPEED_2500MBS            0x200

  /*QCA808X interrupt flag */
#define QCA808X_INTR_FAST_LINK_DOWN             0x8000
#define QCA808X_INTR_SPEED_CHANGE               0x4000
#define QCA808X_INTR_SEC_ENA_CHANGE             0x2000
#define QCA808X_INTR_STATUS_DOWN_CHANGE         0x0800
#define QCA808X_INTR_STATUS_UP_CHANGE           0x0400
#define QCA808X_INTR_FAST_LINK_DOWN_MASK        0x240
#define QCA808X_INTR_FAST_LINK_DOWN_STAT_10M    0x40
#define QCA808X_INTR_FAST_LINK_DOWN_STAT_100M   0x200
#define QCA808X_INTR_FAST_LINK_DOWN_STAT_1000M  0x240
#define QCA808X_INTR_LINK_FAIL_SG               0x0100
#define QCA808X_INTR_LINK_SUCCESS_SG            0x0080
#define QCA808X_INTR_DOWNSHIF                   0x0020
#define QCA808X_INTR_10MS_PTP                   0x0010
#define QCA808X_INTR_RX_PTP                     0x0008
#define QCA808X_INTR_TX_PTP                     0x0004
#define QCA808X_INTR_POE                        0x0002
#define QCA808X_INTR_WOL                        0x0001

  /* QCA808X counter */
#define QCA808X_PHY_MMD7_COUNTER_CTRL           0x8029
#define QCA808X_PHY_MMD7_INGRESS_COUNTER_HIGH   0x802a
#define QCA808X_PHY_MMD7_INGRESS_COUNTER_LOW    0x802b
#define QCA808X_PHY_MMD7_INGRESS_ERROR_COUNTER  0x802c
#define QCA808X_PHY_MMD7_EGRESS_COUNTER_HIGH    0x802d
#define QCA808X_PHY_MMD7_EGRESS_COUNTER_LOW     0x802e
#define QCA808X_PHY_MMD7_EGRESS_ERROR_COUNTER   0x802f
#define QCA808X_PHY_MMD7_LED_POLARITY_CTRL      0x901a
#define QCA808X_PHY_MMD7_LED0_CTRL              0x8078
#define QCA808X_PHY_MMD7_LED1_CTRL              0x8074
#define QCA808X_PHY_MMD7_LED2_CTRL              0x8076
#define QCA808X_PHY_MMD7_LED_POLARITY_ACTIVE_HIGH 0x46
#define QCA808X_PHY_MMD7_LED0_CTRL_ENABLE       0x8670
#define QCA808X_PHY_MMD7_LED1_CTRL_DISABLE      0x0
#define QCA808X_PHY_MMD7_LED2_CTRL_DISABLE      0x0

#define QCA808X_PHY_FRAME_CHECK_EN              0x0001
#define QCA808X_PHY_XMIT_MAC_CNT_SELFCLR        0x0002

#define QCA808X_PHY_ADC_THRESHOLD               0x2c80
#define QCA808X_PHY_ADC_THRESHOLD_80MV          0
#define QCA808X_PHY_ADC_THRESHOLD_100MV         0xf0
#define QCA808X_PHY_ADC_THRESHOLD_200MV         0x0f
#define QCA808X_PHY_ADC_THRESHOLD_300MV         0xff

a_uint16_t
qca808x_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id);

sw_error_t
qca808x_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t reg_id, a_uint16_t reg_val);

sw_error_t
qca808x_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint16_t mmd_num, a_uint16_t reg_id,
		a_uint16_t reg_val);

a_uint16_t
qca808x_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint16_t mmd_num, a_uint16_t reg_id);

a_uint16_t
qca808x_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg_id);

sw_error_t
qca808x_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t
reg_id, a_uint16_t reg_val);

sw_error_t
qca808x_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id,
		       a_uint16_t reg_val);
a_uint16_t
qca808x_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id);

sw_error_t
qca808x_phy_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint16_t mmd_num, a_uint16_t reg_id, a_uint16_t
reg_val);

a_uint16_t
qca808x_phy_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint16_t mmd_num, a_uint16_t reg_id);

#define QCA808X_PHY_8023AZ_AFE_CTRL_MASK        0x01f0
#define QCA808X_PHY_8023AZ_AFE_EN               0x0090

sw_error_t
qca808x_phy_set_duplex (a_uint32_t dev_id, a_uint32_t phy_id,
		   fal_port_duplex_t duplex);

sw_error_t
qca808x_phy_get_duplex (a_uint32_t dev_id, a_uint32_t phy_id,
		   fal_port_duplex_t * duplex);

sw_error_t
qca808x_phy_set_speed (a_uint32_t dev_id, a_uint32_t phy_id,
		  fal_port_speed_t speed);

sw_error_t
qca808x_phy_get_speed (a_uint32_t dev_id, a_uint32_t phy_id,
		  fal_port_speed_t * speed);

sw_error_t
qca808x_phy_restart_autoneg (a_uint32_t dev_id, a_uint32_t phy_id);

sw_error_t
qca808x_phy_enable_autoneg (a_uint32_t dev_id, a_uint32_t phy_id);

a_bool_t
qca808x_phy_get_link_status (a_uint32_t dev_id, a_uint32_t phy_id);

sw_error_t
qca808x_phy_set_autoneg_adv (a_uint32_t dev_id, a_uint32_t phy_id,
			a_uint32_t autoneg);

sw_error_t
qca808x_phy_get_autoneg_adv (a_uint32_t dev_id, a_uint32_t phy_id,
			a_uint32_t * autoneg);

a_bool_t qca808x_phy_autoneg_status (a_uint32_t dev_id, a_uint32_t phy_id);
#ifndef IN_PORTCONTROL_MINI
sw_error_t
qca808x_phy_intr_mask_set (a_uint32_t dev_id, a_uint32_t phy_id,
		      a_uint32_t intr_mask_flag);

sw_error_t
qca808x_phy_intr_mask_get (a_uint32_t dev_id, a_uint32_t phy_id,
		      a_uint32_t * intr_mask_flag);

sw_error_t
qca808x_phy_intr_status_get (a_uint32_t dev_id, a_uint32_t phy_id,
			a_uint32_t * intr_status_flag);
#endif
sw_error_t
qca808x_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data);

sw_error_t
qca808x_phy_get_status(a_uint32_t dev_id, a_uint32_t phy_id,
		struct port_phy_status *phy_status);

sw_error_t
qca808x_phy_interface_get_mode_status(a_uint32_t dev_id, a_uint32_t phy_id,
		fal_port_interface_mode_t *interface_mode_status);

sw_error_t qca808x_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id);

sw_error_t
qca808x_phy_set_force_speed(a_uint32_t dev_id, a_uint32_t phy_id,
		     fal_port_speed_t speed);
sw_error_t qca808x_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id);
sw_error_t qca808x_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id);
#ifndef IN_PORTCONTROL_MINI
sw_error_t
qca808x_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable);
sw_error_t
qca808x_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable);
sw_error_t
qca808x_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
	fal_cable_status_t * cable_status, a_uint32_t * cable_len);
sw_error_t
qca808x_phy_set_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_port_mdix_mode_t mode);
sw_error_t
qca808x_phy_get_mdix(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_port_mdix_mode_t * mode);
sw_error_t
qca808x_phy_get_mdix_status(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_port_mdix_status_t * mode);
sw_error_t
qca808x_phy_set_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t enable);
sw_error_t
qca808x_phy_get_local_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable);
sw_error_t
qca808x_phy_set_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t enable);
sw_error_t
qca808x_phy_get_remote_loopback(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable);
sw_error_t
qca808x_phy_set_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable);
sw_error_t
qca808x_phy_get_wol_status(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable);
sw_error_t
qca808x_phy_set_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_mac_addr_t * mac);
sw_error_t
qca808x_phy_get_magic_frame_mac(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_mac_addr_t * mac);
sw_error_t
qca808x_phy_set_counter(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable);
sw_error_t
qca808x_phy_get_counter(a_uint32_t dev_id, a_uint32_t phy_id,
	a_bool_t * enable);
sw_error_t
qca808x_phy_show_counter(a_uint32_t dev_id, a_uint32_t phy_id,
	fal_port_counter_info_t * counter_infor);
sw_error_t
qca808x_phy_set_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t intr_mask_flag);
sw_error_t
qca808x_phy_get_intr_mask(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t * intr_mask_flag);
sw_error_t
qca808x_phy_get_intr_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t * intr_status_flag);
sw_error_t
qca808x_phy_set_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable);
sw_error_t
qca808x_phy_get_8023az(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t * enable);
#endif
sw_error_t
qca808x_phy_set_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t adv);
sw_error_t
qca808x_phy_get_eee_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv);
sw_error_t
qca808x_phy_get_eee_partner_adv(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *adv);
sw_error_t
qca808x_phy_get_eee_cap(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *cap);
sw_error_t
qca808x_phy_get_eee_status(a_uint32_t dev_id, a_uint32_t phy_id,
	a_uint32_t *status);
void qca808x_phy_lock_init(void);
int qca808x_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp);

void qca808x_phy_exit(a_uint32_t dev_id, a_uint32_t port_id);
a_bool_t
qca808x_phy_2500caps(a_uint32_t dev_id, a_uint32_t phy_id);

#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _qca808x_PHY_H_ */
