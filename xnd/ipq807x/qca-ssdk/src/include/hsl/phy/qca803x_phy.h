/*
 * Copyright (c) 2017, 2019, The Linux Foundation. All rights reserved.
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

#ifndef _QCA803X_PHY_H_
#define _QCA803X_PHY_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#define QCA803X_COMMON_CTRL  0x1040
#define QCA803X_10M_LOOPBACK  0x4100
#define QCA803X_100M_LOOPBACK  0x6100
#define QCA803X_1000M_LOOPBACK  0x4140

  /* PHY Registers */
#define QCA803X_PHY_CONTROL                      0
#define QCA803X_PHY_STATUS                       1
#define QCA803X_PHY_SPEC_STATUS                  17

#define QCA803X_PHY_ID1                          2
#define QCA803X_PHY_ID2                          3
#define QCA803X_AUTONEG_ADVERT                   4
#define QCA803X_LINK_PARTNER_ABILITY             5
#define QCA803X_1000BASET_CONTROL                9
#define QCA803X_1000BASET_STATUS                 10
#define QCA803X_MMD_CTRL_REG                     13
#define QCA803X_MMD_DATA_REG                     14
#define QCA803X_EXTENDED_STATUS                  15
#define QCA803X_PHY_SPEC_CONTROL                 16
#define QCA803X_PHY_INTR_MASK                    18
#define QCA803X_PHY_INTR_STATUS                  19
#define QCA803X_PHY_CDT_CONTROL                  22
#define QCA803X_PHY_CDT_STATUS                   28
#define QCA803X_DEBUG_PORT_ADDRESS               29
#define QCA803X_DEBUG_PORT_DATA                  30
#define QCA803X_PHY_CHIP_CONFIG                  31 /* Chip Configuration Register  */
#define QCA803X_DEBUG_MSE_THRESH                 27
#define QCA803X_DEBUG_MSE_OVER_THRESH_TIMES      28

#define QCA803X_PHY_MSE_THRESH_MASK              0x3f8
#define QCA803X_PHY_MSE_THRESH_LINK_DOWN         0x170
#define QCA803X_PHY_MSE_THRESH_LINK_UP           0x2e8
#define QCA803X_PHY_MSE_OVER_THRESH_TIMES_MAX    0x7000

#define QCA803X_PHY_FIBER_MODE_1000BX	0x100

#define QCA803X_PHY_COPPER_PAGE_SEL  0x8000
#define QCA803X_PHY_PREFER_FIBER  0x400

#define QCA803X_PHY_CHIP_MODE_CFG	0x000f
#define QCA803X_PHY_CHIP_MODE_STAT	0x00f0

#define QCA803X_DEBUG_PHY_HIBERNATION_CTRL  0xb
#define QCA803X_DEBUG_PHY_POWER_SAVING_CTRL  0x29
#define QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_CTRL       0x3c
#define QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_PARTNER       0x3d
#define QCA803X_PHY_MMD7_ADDR_8023AZ_EEE_STATUS       0x8000
#define QCA803X_PHY_MMD3_ADDR_8023AZ_EEE_CAPABILITY       0x14

#define QCA803X_PHY_MMD3_ADDR_REMOTE_LOOPBACK_CTRL       0x805a
#define QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL1   0x804a
#define QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL2   0x804b
#define QCA803X_PHY_MMD3_WOL_MAGIC_MAC_CTRL3   0x804c
#define QCA803X_PHY_MMD3_WOL_CTRL  0x8012
#define QCA803X_PHY_MMD3_ADDR_8023AZ_TIMER_CTRL       0x804e
#define QCA803X_PHY_MMD3_ADDR_8023AZ_CLD_CTRL       0x8007
#define QCA803X_PHY_MMD3_ADDR_CLD_CTRL5       0x8005
#define QCA803X_PHY_MMD3_ADDR_CLD_CTRL3       0x8003

#define QCA803X_PHY_MDIX     0x0020
#define QCA803X_PHY_MDIX_AUTO     0x0060
#define QCA803X_PHY_MDIX_STATUS     0x0040

#define QCA803X_PHY_MMD7_NUM  7
#define QCA803X_PHY_MMD3_NUM  3

#define QCA803X_PWR_SAVE 0x29
#define QCA803X_PWR_SAVE_EN 0x8000

#define QCA803X_PHY_EEE_ADV_100M 0x0002
#define QCA803X_PHY_EEE_ADV_1000M 0x0004
#define QCA803X_PHY_EEE_PARTNER_ADV_100M 0x0002
#define QCA803X_PHY_EEE_PARTNER_ADV_1000M 0x0004
#define QCA803X_PHY_EEE_CAPABILITY_100M 0x0002
#define QCA803X_PHY_EEE_CAPABILITY_1000M 0x0004
#define QCA803X_PHY_EEE_STATUS_100M 0x0002
#define QCA803X_PHY_EEE_STATUS_1000M 0x0004

	/* CDT */
#define QCA803X_MDI_PAIR_NUM 4
#define QCA803X_RUN_CDT 0x1
#define CDT_PAIR_MASK 0x0300

  /* PHY Registers Field */
#define QCA803X_STATUS_LINK_PASS                 0x0400

  /* Control Register fields  offset:0 */
  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define QCA803X_CTRL_SPEED_MSB                   0x0040

  /* Collision test enable */
#define QCA803X_CTRL_COLL_TEST_ENABLE            0x0080

  /* FDX =1, half duplex =0 */
#define QCA803X_CTRL_FULL_DUPLEX                 0x0100

  /* Restart auto negotiation */
#define QCA803X_CTRL_RESTART_AUTONEGOTIATION     0x0200

  /* Power down */
#define QCA803X_CTRL_POWER_DOWN                  0x0800

  /* Auto Neg Enable */
#define QCA803X_CTRL_AUTONEGOTIATION_ENABLE      0x1000

  /* Local Loopback Enable */
#define QCA803X_LOCAL_LOOPBACK_ENABLE      0x4000

  /* bits 6,13: 10=1000, 01=100, 00=10 */
#define QCA803X_CTRL_SPEED_LSB                   0x2000

  /* 0 = normal, 1 = loopback */
#define QCA803X_CTRL_LOOPBACK                    0x4000
#define QCA803X_CTRL_SOFTWARE_RESET              0x8000

#define QCA803X_CTRL_SPEED_MASK                  0x2040
#define QCA803X_CTRL_SPEED_1000                  0x0040
#define QCA803X_CTRL_SPEED_100                   0x2000
#define QCA803X_CTRL_SPEED_10                    0x0000

#define QCA803X_RESET_DONE(phy_control)                   \
    (((phy_control) & (QCA803X_CTRL_SOFTWARE_RESET)) == 0)

  /* Status Register fields offset:1 */
#define QCA803X_STATUS_EXTENDED_CAPS             0x0001

  /* Jabber Detected */
#define QCA803X_STATUS_JABBER_DETECT             0x0002

  /* Link Status 1 = link */
#define QCA803X_STATUS_LINK_STATUS_UP            0x0004

  /* Auto Neg Capable */
#define QCA803X_STATUS_AUTONEG_CAPS              0x0008

  /* Remote Fault Detect */
#define QCA803X_STATUS_REMOTE_FAULT              0x0010

  /* Auto Neg Complete */
#define QCA803X_STATUS_AUTO_NEG_DONE             0x0020

  /* Preamble may be suppressed */
#define QCA803X_STATUS_PREAMBLE_SUPPRESS         0x0040

  /* Ext. status info in Reg 0x0F */
#define QCA803X_STATUS_EXTENDED_STATUS           0x0100

  /* 100T2 Half Duplex Capable */
#define QCA803X_STATUS_100T2_HD_CAPS             0x0200

  /* 100T2 Full Duplex Capable */
#define QCA803X_STATUS_100T2_FD_CAPS             0x0400

  /* 10T   Half Duplex Capable */
#define QCA803X_STATUS_10T_HD_CAPS               0x0800

  /* 10T   Full Duplex Capable */
#define QCA803X_STATUS_10T_FD_CAPS               0x1000

  /* 100X  Half Duplex Capable */
#define QCA803X_STATUS_100X_HD_CAPS              0x2000

  /* 100X  Full Duplex Capable */
#define QCA803X_STATUS_100X_FD_CAPS              0x4000

  /* 100T4 Capable */
#define QCA803X_STATUS_100T4_CAPS                0x8000

  /* extended status register capabilities */

#define QCA803X_STATUS_1000T_HD_CAPS             0x1000

#define QCA803X_STATUS_1000T_FD_CAPS             0x2000

#define QCA803X_STATUS_1000X_HD_CAPS             0x4000

#define QCA803X_STATUS_1000X_FD_CAPS             0x8000

  /* Link Partner ability offset:5 */
#define QCA803X_LINK_SLCT                        0x001f

  /* Can do 10mbps half-duplex   */
#define QCA803X_LINK_10BASETX_HALF_DUPLEX        0x0020

  /* Can do 10mbps full-duplex   */
#define QCA803X_LINK_10BASETX_FULL_DUPLEX        0x0040

  /* Can do 100mbps half-duplex  */
#define QCA803X_LINK_100BASETX_HALF_DUPLEX       0x0080

  /* Can do 100mbps full-duplex  */
#define QCA803X_LINK_100BASETX_FULL_DUPLEX       0x0100

  /* Can do 1000mbps full-duplex  */
#define QCA803X_LINK_1000BASETX_FULL_DUPLEX       0x0800

  /* Can do 1000mbps half-duplex  */
#define QCA803X_LINK_1000BASETX_HALF_DUPLEX       0x0400

  /* 100BASE-T4  */
#define QCA803X_LINK_100BASE4                    0x0200

  /* PAUSE */
#define QCA803X_LINK_PAUSE                       0x0400

  /* Asymmetrical PAUSE */
#define QCA803X_LINK_ASYPAUSE                    0x0800

  /* Link partner faulted  */
#define QCA803X_LINK_RFAULT                      0x2000

  /* Link partner acked us */
#define QCA803X_LINK_LPACK                       0x4000

  /* Next page bit  */
#define QCA803X_LINK_NPAGE                       0x8000

  /* Auto Neg Complete */
#define QCA803X_STATUS_AUTO_NEG_DONE             0x0020
#define QCA803X_AUTONEG_DONE(ip_phy_status) \
    (((ip_phy_status) & (QCA803X_STATUS_AUTO_NEG_DONE)) ==  \
        (QCA803X_STATUS_AUTO_NEG_DONE))

#define QCA803X_STATUS_RESOVLED                  0x0800
#define QCA803X_SPEED_DUPLEX_RESOVLED(phy_status)                   \
    (((phy_status) &                                  \
        (QCA803X_STATUS_RESOVLED)) ==                    \
        (QCA803X_STATUS_RESOVLED))

  /* Auto-Negotiation Advertisement register. offset:4 */
#define QCA803X_ADVERTISE_SELECTOR_FIELD         0x0001

  /* 10T   Half Duplex Capable */
#define QCA803X_ADVERTISE_10HALF                 0x0020

  /* 10T   Full Duplex Capable */
#define QCA803X_ADVERTISE_10FULL                 0x0040

  /* 100TX Half Duplex Capable */
#define QCA803X_ADVERTISE_100HALF                0x0080

  /* 100TX Full Duplex Capable */
#define QCA803X_ADVERTISE_100FULL                0x0100

  /* 100T4 Capable */
#define QCA803X_ADVERTISE_100T4                  0x0200

  /* Pause operation desired */
#define QCA803X_ADVERTISE_PAUSE                  0x0400

  /* Asymmetric Pause Direction bit */
#define QCA803X_ADVERTISE_ASYM_PAUSE             0x0800

  /* Remote Fault detected */
#define QCA803X_ADVERTISE_REMOTE_FAULT           0x2000

  /* 100TX Half Duplex Capable */
#define QCA803X_ADVERTISE_1000HALF                0x0100

  /* 100TX Full Duplex Capable */
#define QCA803X_ADVERTISE_1000FULL                0x0200

  /* Extended next page enable control  */
#define QCA803X_EXTENDED_NEXT_PAGE_EN             0x1000

#define QCA803X_ADVERTISE_ALL \
    (QCA803X_ADVERTISE_10HALF | QCA803X_ADVERTISE_10FULL | \
     QCA803X_ADVERTISE_100HALF | QCA803X_ADVERTISE_100FULL | \
     QCA803X_ADVERTISE_1000FULL)

#define QCA803X_ADVERTISE_MEGA_ALL \
    (QCA803X_ADVERTISE_10HALF | QCA803X_ADVERTISE_10FULL | \
     QCA803X_ADVERTISE_100HALF | QCA803X_ADVERTISE_100FULL | \
     QCA803X_ADVERTISE_PAUSE | QCA803X_ADVERTISE_ASYM_PAUSE)

#define QCA803X_BX_ADVERTISE_1000FULL                0x0020
#define QCA803X_BX_ADVERTISE_1000HALF                0x0040
#define QCA803X_BX_ADVERTISE_PAUSE                      0x0080
#define QCA803X_BX_ADVERTISE_ASYM_PAUSE           0x0100

#define QCA803X_BX_ADVERTISE_ALL \
    (QCA803X_BX_ADVERTISE_ASYM_PAUSE | QCA803X_BX_ADVERTISE_PAUSE | \
     QCA803X_BX_ADVERTISE_1000HALF | QCA803X_BX_ADVERTISE_1000FULL)

  /* 1=Duplex 0=Half Duplex */
#define QCA803X_STATUS_FULL_DUPLEX               0x2000

  /* Speed, bits 14:15 */
#define QCA803X_STATUS_SPEED                    0xC000
#define QCA803X_STATUS_SPEED_MASK               0xC000

  /* 00=10Mbs */
#define QCA803X_STATUS_SPEED_10MBS              0x0000

  /* 01=100Mbs */
#define QCA803X_STATUS_SPEED_100MBS             0x4000

  /* 10=1000Mbs */
#define QCA803X_STATUS_SPEED_1000MBS            0x8000

	/*pause status */
#define QCA803X_PHY_RX_FLOWCTRL_STATUS		0x4
#define QCA803X_PHY_TX_FLOWCTRL_STATUS		0x8

  /*QCA803X interrupt flag */
#define QCA803X_INTR_SPEED_CHANGE              0x4000
#define QCA803X_INTR_DUPLEX_CHANGE             0x2000
#define QCA803X_INTR_STATUS_UP_CHANGE          0x0400
#define QCA803X_INTR_STATUS_DOWN_CHANGE        0x0800
#define QCA803X_INTR_BX_FX_STATUS_DOWN_CHANGE        0x0100
#define QCA803X_INTR_BX_FX_STATUS_UP_CHANGE        0x0080
#define QCA803X_INTR_MEDIA_STATUS_CHANGE             0x1000
#define QCA803X_INTR_WOL             0x0001
#define QCA803X_INTR_POE             0x0002

/*QCA803X phy counter*/
#define QCA803X_PHY_MMD7_FRAME_CTRL        0x8020
#define QCA803X_PHY_MMD7_FRAME_DATA        0x8021

#define QCA803X_PHY_MMD7_FRAME_CHECK       0x2000
#define QCA803X_PHY_MMD7_FRAME_DIR         0x4000
#define QCA803X_PHY_FRAME_CNT              0x00FF
#define QCA803X_PHY_FRAME_ERROR            0xFF00

  /** phy chip config */
  typedef enum {
	  QCA803X_PHY_RGMII_BASET = 0,
	  QCA803X_PHY_SGMII_BASET = 1,
	  QCA803X_PHY_BX1000_RGMII_50 = 2,
	  QCA803X_PHY_FX100_RGMII_50 = 6,
	  QCA803X_PHY_RGMII_AMDET = 11
  } qca803x_cfg_t;

  typedef enum {
	  QCA803X_CHIP_CFG_SET,
	  QCA803X_CHIP_CFG_STAT
  } qca803x_cfg_type_t;

/** Phy preferred medium type */
  typedef enum
  {
	  QCA803X_PHY_MEDIUM_COPPER = 0,	/* Copper */
	  QCA803X_PHY_MEDIUM_FIBER = 1,		/* Fiber */
	  QCA803X_PHY_MEDIUM_MAX
  } qca803x_phy_medium_t;

sw_error_t
qca803x_phy_set_duplex (a_uint32_t dev_id, a_uint32_t phy_id,
		   fal_port_duplex_t duplex);

sw_error_t
qca803x_phy_get_duplex (a_uint32_t dev_id, a_uint32_t phy_id,
		   fal_port_duplex_t * duplex);

sw_error_t
qca803x_phy_set_speed (a_uint32_t dev_id, a_uint32_t phy_id,
		  fal_port_speed_t speed);

sw_error_t
qca803x_phy_get_speed (a_uint32_t dev_id, a_uint32_t phy_id,
		  fal_port_speed_t * speed);

sw_error_t
qca803x_phy_restart_autoneg (a_uint32_t dev_id, a_uint32_t phy_id);

sw_error_t
qca803x_phy_enable_autoneg (a_uint32_t dev_id, a_uint32_t phy_id);

a_bool_t
qca803x_phy_get_link_status (a_uint32_t dev_id, a_uint32_t phy_id);

sw_error_t
qca803x_phy_set_autoneg_adv (a_uint32_t dev_id, a_uint32_t phy_id,
			a_uint32_t autoneg);

sw_error_t
qca803x_phy_get_autoneg_adv (a_uint32_t dev_id, a_uint32_t phy_id,
			a_uint32_t * autoneg);

a_bool_t qca803x_phy_autoneg_status (a_uint32_t dev_id, a_uint32_t phy_id);
#ifndef IN_PORTCONTROL_MINI
sw_error_t
qca803x_phy_intr_mask_set (a_uint32_t dev_id, a_uint32_t phy_id,
		      a_uint32_t intr_mask_flag);

sw_error_t
qca803x_phy_intr_mask_get (a_uint32_t dev_id, a_uint32_t phy_id,
		      a_uint32_t * intr_mask_flag);

sw_error_t
qca803x_phy_intr_status_get (a_uint32_t dev_id, a_uint32_t phy_id,
			a_uint32_t * intr_status_flag);
#endif
sw_error_t
qca803x_phy_get_phy_id(a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *phy_data);
int qca803x_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp);

#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _qca803x_PHY_H_ */
