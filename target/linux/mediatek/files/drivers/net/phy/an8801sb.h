/* SPDX-License-Identifier: GPL-2.0 */
/*PURPOSE:
 *     Define Airoha AN8801SB/R PHY driver function
 *
 *NOTES:
 *
 */

#ifndef __AN8801_H
#define __AN8801_H

/* NAMING DECLARATIONS
 */
#define AN8801_DRIVER_VERSION  "1.1.7"

#define DEBUGFS_COUNTER         "counter"
#define DEBUGFS_INFO            "driver_info"
#define DEBUGFS_PBUS_OP         "pbus_op"
#define DEBUGFS_POLARITY        "polarity"
#define DEBUGFS_MDIO            "mdio"

#define AN8801_MDIO_PHY_ID      0x1
#define AN8801_PHY_ID1          0xc0ff
#define AN8801_PHY_ID2          0x0421
#define AN8801_PHY_ID      ((u32)((AN8801_PHY_ID1 << 16) | AN8801_PHY_ID2))

#define TRUE                    1
#define FALSE                   0
#define LINK_UP                 1
#define LINK_DOWN               0

#define MAX_LED_SIZE            3

#define MAX_RETRY               5

#define AN8801_EPHY_ADDR            0x11000000
#define AN8801_CL22                 0x00800000

#define LED_ENABLE                  1
#define LED_DISABLE                 0

#define AN8801SB_DEBUGFS

#ifndef BIT
#define BIT(nr)                     (1 << (nr))
#endif
#ifndef GET_BIT
#define GET_BIT(val, bit) ((val & BIT(bit)) >> bit)
#endif

#define LED_BCR                     (0x021)
#define LED_BCR_EXT_CTRL            BIT(15)
#define LED_BCR_EVT_ALL             BIT(4)
#define LED_BCR_CLK_EN              BIT(3)
#define LED_BCR_TIME_TEST           BIT(2)
#define LED_BCR_MODE_MASK           (3)
#define LED_BCR_MODE_DISABLE        (0)
#define LED_BCR_MODE_2LED           (1)
#define LED_BCR_MODE_3LED_1         (2)
#define LED_BCR_MODE_3LED_2         (3)

#define LED_ON_DUR                  (0x022)
#define LED_ON_DUR_MASK             (0xffff)

#define LED_BLK_DUR                 (0x023)
#define LED_BLK_DUR_MASK            (0xffff)

#define LED_ON_CTRL(i)              (0x024 + ((i) * 2))
#define LED_ON_EN                   BIT(15)
#define LED_ON_POL                  BIT(14)
#define LED_ON_EVT_MASK             (0x7f)
#define LED_ON_EVT_FORCE            BIT(6)
#define LED_ON_EVT_HDX              BIT(5)
#define LED_ON_EVT_FDX              BIT(4)
#define LED_ON_EVT_LINK_DN          BIT(3)
#define LED_ON_EVT_LINK_10M         BIT(2)
#define LED_ON_EVT_LINK_100M        BIT(1)
#define LED_ON_EVT_LINK_1000M       BIT(0)

#define LED_BLK_CTRL(i)             (0x025 + ((i) * 2))
#define LED_BLK_EVT_MASK            (0x3ff)
#define LED_BLK_EVT_FORCE           BIT(9)
#define LED_BLK_EVT_10M_RX          BIT(5)
#define LED_BLK_EVT_10M_TX          BIT(4)
#define LED_BLK_EVT_100M_RX         BIT(3)
#define LED_BLK_EVT_100M_TX         BIT(2)
#define LED_BLK_EVT_1000M_RX        BIT(1)
#define LED_BLK_EVT_1000M_TX        BIT(0)

#define UNIT_LED_BLINK_DURATION     780

/* Serdes auto negotiation restart */
#define AN8801SB_SGMII_AN0_ANRESTART    (0x0200)
#define AN8801SB_SGMII_AN0_AN_DONE      (0x0001)
#define AN8801SB_SGMII_AN0_RESET        (0x8000)


#define PHY_PRE_SPEED_REG               (0x2b)

#define MMD_DEV_VSPEC1          (0x1E)
#define MMD_DEV_VSPEC2          (0x1F)

#define RGMII_DELAY_STEP_MASK       0x7
#define RGMII_RXDELAY_ALIGN         BIT(4)
#define RGMII_RXDELAY_FORCE_MODE    BIT(24)
#define RGMII_TXDELAY_FORCE_MODE    BIT(24)
#define AN8801_RG_PKG_SEL_LSB       BIT(4)
#define AN8801_RG_PKG_SEL_MSB       BIT(5)

/*
 *For reference only
 */
/* User-defined.B */
/* Link on(1G/100M/10M), no activity */
#define AIR_LED0_ON \
	(LED_ON_EVT_LINK_1000M | LED_ON_EVT_LINK_100M | LED_ON_EVT_LINK_10M)
#define AIR_LED0_BLK     (0x0)
/* No link on, activity(1G/100M/10M TX/RX) */
#define AIR_LED1_ON      (0x0)
#define AIR_LED1_BLK \
	(LED_BLK_EVT_1000M_TX | LED_BLK_EVT_1000M_RX | \
	LED_BLK_EVT_100M_TX | LED_BLK_EVT_100M_RX | \
	LED_BLK_EVT_10M_TX | LED_BLK_EVT_10M_RX)
/* Link on(100M/10M), activity(100M/10M TX/RX) */
#define AIR_LED2_ON      (LED_ON_EVT_LINK_100M | LED_ON_EVT_LINK_10M)
#define AIR_LED2_BLK \
	(LED_BLK_EVT_100M_TX | LED_BLK_EVT_100M_RX | \
	LED_BLK_EVT_10M_TX | LED_BLK_EVT_10M_RX)
/* User-defined.E */

/* Invalid data */
#define INVALID_DATA            0xffffffff

#define LED_BLINK_DURATION(f)       (UNIT_LED_BLINK_DURATION << (f))
#define LED_GPIO_SEL(led, gpio)     ((led) << ((gpio) * 3))

/* Interrupt GPIO number, should not conflict with LED */
#define AIR_INTERRUPT_GPIO	3

/* DATA TYPE DECLARATIONS
 */
enum AIR_LED_GPIO_PIN_T {
	AIR_LED_GPIO1 = 1,
	AIR_LED_GPIO2 = 2,
	AIR_LED_GPIO3 = 3,
	AIR_LED_GPIO5 = 5,
	AIR_LED_GPIO8 = 8,
	AIR_LED_GPIO9 = 9,
};

enum AIR_LED_T {
	AIR_LED0 = 0,
	AIR_LED1,
	AIR_LED2,
	AIR_LED3
};

enum AIR_LED_BLK_DUT_T {
	AIR_LED_BLK_DUR_32M = 0,
	AIR_LED_BLK_DUR_64M,
	AIR_LED_BLK_DUR_128M,
	AIR_LED_BLK_DUR_256M,
	AIR_LED_BLK_DUR_512M,
	AIR_LED_BLK_DUR_1024M,
	AIR_LED_BLK_DUR_LAST
};

enum AIR_LED_POLARITY {
	AIR_ACTIVE_LOW = 0,
	AIR_ACTIVE_HIGH,
};

enum AIR_LED_MODE_T {
	AIR_LED_MODE_DISABLE = 0,
	AIR_LED_MODE_USER_DEFINE,
	AIR_LED_MODE_LAST
};

enum AIR_RGMII_DELAY_STEP_T {
	AIR_RGMII_DELAY_NOSTEP = 0,
	AIR_RGMII_DELAY_STEP_1 = 1,
	AIR_RGMII_DELAY_STEP_2 = 2,
	AIR_RGMII_DELAY_STEP_3 = 3,
	AIR_RGMII_DELAY_STEP_4 = 4,
	AIR_RGMII_DELAY_STEP_5 = 5,
	AIR_RGMII_DELAY_STEP_6 = 6,
	AIR_RGMII_DELAY_STEP_7 = 7,
};

struct AIR_LED_CFG_T {
	u16 en;
	u16 gpio;
	u16 pol;
	u16 on_cfg;
	u16 blk_cfg;
};

struct an8801_priv {
	struct AIR_LED_CFG_T  led_cfg[MAX_LED_SIZE];
	u32                   led_blink_cfg;
	u8                    rxdelay_force;
	u8                    txdelay_force;
	u16                   rxdelay_step;
	u8                    rxdelay_align;
	u16                   txdelay_step;
#ifdef AN8801SB_DEBUGFS
	struct dentry        *debugfs_root;
#endif
	int                   pol;
	int                   surge;
	int		      sgmii_mode;
};

enum an8801_polarity {
	AIR_POL_TX_NOR_RX_REV,
	AIR_POL_TX_REV_RX_REV,
	AIR_POL_TX_NOR_RX_NOR,
	AIR_POL_TX_REV_RX_NOR,
};

enum air_surge {
	AIR_SURGE_0R,
	AIR_SURGE_5R,
	AIR_SURGE_LAST = 0xff
};

enum air_sgmii_mode {
	AIR_SGMII_AN,
	AIR_SGMII_FORCE,
	AIR_SGMII_LAST = 0xff
};

#endif /* End of __AN8801_H */
