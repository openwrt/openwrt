// SPDX-License-Identifier: GPL-2.0
/*PURPOSE:
 *     Airoha AN8801R PHY driver for Linux
 *NOTES:
 *
 */

/*INCLUDE FILE DECLARATIONS
 */

#include <linux/of.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>

MODULE_DESCRIPTION("Airoha AN8801 PHY drivers");
MODULE_AUTHOR("Airoha");
MODULE_LICENSE("GPL");

#define AN8801R_DRIVER_VERSION  "1.1.0_upstream"

#define DEBUGFS_COUNTER         "counter"
#define DEBUGFS_INFO            "driver_info"
#define DEBUGFS_PBUS_OP         "pbus_op"
#define DEBUGFS_MDIO            "mdio"

#define AN8801R_MDIO_PHY_ID     0x1
#define AN8801R_PHY_ID1         0xc0ff
#define AN8801R_PHY_ID2         0x0421
#define AN8801R_PHY_ID          ((u32)((AN8801R_PHY_ID1 << 16) | AN8801R_PHY_ID2))

#define TRUE                    1
#define FALSE                   0
#define LINK_UP                 1
#define LINK_DOWN               0

#define MAX_LED_SIZE            3

#define MAX_RETRY               5

#define LED_ENABLE                  1
#define LED_DISABLE                 0

#define AN8801R_DEBUGFS

#ifndef BIT
#define BIT(nr)                     (1 << (nr))
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

#define RGMII_DELAY_STEP_MASK       0x7
#define RGMII_RXDELAY_ALIGN         BIT(4)
#define RGMII_RXDELAY_FORCE_MODE    BIT(24)
#define RGMII_TXDELAY_FORCE_MODE    BIT(24)
#define AIR_CKO_OUT_DRV				0xF /* Available: 0x0~0xF (about 1.8V~2.5V) */

/* For reference only */
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

/* DATA TYPE DECLARATIONS
 */
enum AIR_LED_GPIO_PIN_T {
	AIR_LED_GPIO1 = 1,
	AIR_LED_GPIO2,
	AIR_LED_GPIO3
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

enum AIR_CKO_OUTPUT_RATE_T {
	AIR_CKO_OUTPUT_RATE_25M = 0,
	AIR_CKO_OUTPUT_RATE_125M = 1,
};

struct AIR_LED_CFG_T {
	u16 en;
	u16 gpio;
	u16 pol;
	u16 on_cfg;
	u16 blk_cfg;
};

struct an8801r_priv {
	struct AIR_LED_CFG_T  led_cfg[MAX_LED_SIZE];
	u32                   led_blink_cfg;
	u8                    rxdelay_force;
	u8                    txdelay_force;
	u16                   rxdelay_step;
	u8                    rxdelay_align;
	u16                   txdelay_step;
	u8                    cko_output_en;
	u8                    cko_output_rate;
#ifdef AN8801R_DEBUGFS
	struct dentry        *debugfs_root;
#endif
};

#define phydev_mdiobus(_dev)        ((_dev)->mdio.bus)
#define phydev_phy_addr(_dev) ((_dev)->mdio.addr)
#define phydev_dev(_dev) (&(_dev)->mdio.dev)

#define phydev_mdiobus_lock(phy)   (phydev_mdiobus(phy)->mdio_lock)
#define phydev_cfg(phy)            ((struct an8801r_priv *)(phy)->priv)

#define mdiobus_lock(phy)          (mutex_lock(&phydev_mdiobus_lock(phy)))
#define mdiobus_unlock(phy)        (mutex_unlock(&phydev_mdiobus_lock(phy)))

#ifdef AN8801R_DEBUGFS
#define AN8801R_DEBUGFS_PBUS_HELP_STRING \
	"\nUsage: echo w [pbus_reg] [value] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_reg_op" \
	"\n       echo r [pbus_reg] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_op" \
	"\nRead example: 0x10000054" \
	"\necho r 0x10000054> /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_reg_op" \
	"\nWrite example: Register 0x10000054 0x0" \
	"\necho w 0x10000054 0x0> /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_op" \
	"\n"
#define AN8801R_DEBUGFS_MDIO_HELP_STRING \
	"\nUsage: echo cl22 w [phy_reg] [value]> /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/mdio" \
	"\n       echo cl22 r [phy_reg] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/mdio" \
	"\nUsage: echo cl45 w [devad] [phy_reg] [value]> /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/mdio" \
	"\n       echo cl45 r [devad] [phy_reg] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/mdio" \
	"\n"
#endif

/* For reference only
 *	GPIO1    <-> LED0,
 *	GPIO2    <-> LED1,
 *	GPIO3    <-> LED2,
 */
/* User-defined.B */
static const struct AIR_LED_CFG_T led_cfg_dlt[MAX_LED_SIZE] = {
//   LED Enable,          GPIO,    LED Polarity,      LED ON,    LED Blink
	/* LED0 */
	{LED_ENABLE, AIR_LED_GPIO1, AIR_ACTIVE_LOW,  AIR_LED0_ON, AIR_LED0_BLK},
	/* LED1 */
	{LED_ENABLE, AIR_LED_GPIO2, AIR_ACTIVE_HIGH, AIR_LED1_ON, AIR_LED1_BLK},
	/* LED2 */
	{LED_ENABLE, AIR_LED_GPIO3, AIR_ACTIVE_HIGH, AIR_LED2_ON, AIR_LED2_BLK},
};

static const u16 led_blink_cfg_dlt = AIR_LED_BLK_DUR_64M;
/* RGMII delay */
static const u8 rxdelay_force = FALSE;
static const u8 txdelay_force = FALSE;
static const u16 rxdelay_step = AIR_RGMII_DELAY_NOSTEP;
static const u8 rxdelay_align = FALSE;
static const u16 txdelay_step = AIR_RGMII_DELAY_NOSTEP;
/* User-defined.E */

static int __air_buckpbus_reg_write(struct phy_device *phydev, u32 addr,
				    u32 data)
{
	int err = 0;

	err = __phy_write(phydev, 0x1F, 4);
	if (err)
		return err;

	err |= __phy_write(phydev, 0x10, 0);
	err |= __phy_write(phydev, 0x11, (u16)(addr >> 16));
	err |= __phy_write(phydev, 0x12, (u16)(addr & 0xffff));
	err |= __phy_write(phydev, 0x13, (u16)(data >> 16));
	err |= __phy_write(phydev, 0x14, (u16)(data & 0xffff));
	err |= __phy_write(phydev, 0x1F, 0);

	return err;
}

static int __air_buckpbus_reg_read(struct phy_device *phydev, u32 addr,
				   u32 *data)
{
	int err = 0;
	u32 data_h, data_l;

	err = __phy_write(phydev, 0x1F, 4);
	if (err)
		return err;

	err |= __phy_write(phydev, 0x10, 0);
	err |= __phy_write(phydev, 0x15, (u16)(addr >> 16));
	err |= __phy_write(phydev, 0x16, (u16)(addr & 0xffff));
	data_h = __phy_read(phydev, 0x17);
	data_l = __phy_read(phydev, 0x18);
	err |= __phy_write(phydev, 0x1F, 0);
	if (err)
		return err;

	*data = ((data_h & 0xffff) << 16) | (data_l & 0xffff);
	return 0;
}

static int __air_buckpbus_reg_modify(struct phy_device *phydev, u32 addr,
				     u32 mask, u32 set)
{
	int err = 0;
	u32 data_h, data_l, data_old, data_new;

	err = __phy_write(phydev, 0x1F, 4);
	if (err)
		return err;
	err |= __phy_write(phydev, 0x10, 0);
	err |= __phy_write(phydev, 0x15, (u16)(addr >> 16));
	err |= __phy_write(phydev, 0x16, (u16)(addr & 0xffff));
	data_h = __phy_read(phydev, 0x17);
	data_l = __phy_read(phydev, 0x18);
	if (err) {
		__phy_write(phydev, 0x1F, 0);
		return err;
	}

	data_old = data_l | (data_h << 16);
	data_new = (data_old & ~mask) | set;
	if (data_new == data_old)
		return __phy_write(phydev, 0x1F, 0);

	err |= __phy_write(phydev, 0x11, (u16)(addr >> 16));
	err |= __phy_write(phydev, 0x12, (u16)(addr & 0xffff));
	err |= __phy_write(phydev, 0x13, (u16)(data_new >> 16));
	err |= __phy_write(phydev, 0x14, (u16)(data_new & 0xffff));
	err |= __phy_write(phydev, 0x1F, 0);

	return err;
}

static int air_buckpbus_reg_write(struct phy_device *phydev, u32 addr, u32 data)
{
	int err = 0;

	mdiobus_lock(phydev);
	err = __air_buckpbus_reg_write(phydev, addr, data);
	mdiobus_unlock(phydev);

	return err;
}

static int air_buckpbus_reg_read(struct phy_device *phydev, u32 addr, u32 *data)
{
	int err;

	mdiobus_lock(phydev);
	err = __air_buckpbus_reg_read(phydev, addr, data);
	mdiobus_unlock(phydev);

	return err;
}

static int air_buckpbus_reg_modify(struct phy_device *phydev, u32 addr,
				   u32 mask, u32 set)
{
	int err = 0;

	mdiobus_lock(phydev);
	err = __air_buckpbus_reg_modify(phydev, addr, mask, set);
	mdiobus_unlock(phydev);

	return err;
}

static int __air_read_mmd(struct phy_device *phydev, int devad, u32 regnum)
{
	int val;
	struct mii_bus *bus = phydev->mdio.bus;
	int phy_addr = phydev->mdio.addr;

	__mdiobus_write(bus, phy_addr, MII_MMD_CTRL, devad);
	__mdiobus_write(bus, phy_addr, MII_MMD_DATA, regnum);
	__mdiobus_write(bus, phy_addr, MII_MMD_CTRL,
			devad | MII_MMD_CTRL_NOINCR);

	val = __mdiobus_read(bus, phy_addr, MII_MMD_DATA);

	return val;
}

static int __air_write_mmd(struct phy_device *phydev, int devad, u32 regnum, u16 val)
{
	int ret;
	struct mii_bus *bus = phydev->mdio.bus;
	int phy_addr = phydev->mdio.addr;

	__mdiobus_write(bus, phy_addr, MII_MMD_CTRL, devad);
	__mdiobus_write(bus, phy_addr, MII_MMD_DATA, regnum);
	__mdiobus_write(bus, phy_addr, MII_MMD_CTRL,
			devad | MII_MMD_CTRL_NOINCR);

	__mdiobus_write(bus, phy_addr, MII_MMD_DATA, val);

	ret = 0;

	return ret;
}

static int __air_modify_mmd(struct phy_device *phydev, int devad, u32 regnum,
			    u16 mask, u16 set)
{
	int new, ret;

	ret = __air_read_mmd(phydev, devad, regnum);
	if (ret < 0)
		return ret;

	new = (ret & ~mask) | set;
	if (new == ret)
		return 0;

	ret = __air_write_mmd(phydev, devad, regnum, new);

	return ret < 0 ? ret : 0;
}

static int air_modify_mmd(struct phy_device *phydev, int devad, u32 regnum,
			  u16 mask, u16 set)
{
	int ret;

	mdiobus_lock(phydev);
	ret = __air_modify_mmd(phydev, devad, regnum, mask, set);
	mdiobus_unlock(phydev);

	return ret;
}

static int an8801r_led_set_usr_def(struct phy_device *phydev, u8 entity,
				   u16 polar, u16 on_evt, u16 blk_evt)
{
	int err;

	if (polar == AIR_ACTIVE_HIGH)
		on_evt |= LED_ON_POL;
	else
		on_evt &= ~LED_ON_POL;

	on_evt |= LED_ON_EN;

	err = phy_write_mmd(phydev, 0x1f, LED_ON_CTRL(entity), on_evt);
	if (err)
		return -1;

	return phy_write_mmd(phydev, 0x1f, LED_BLK_CTRL(entity), blk_evt);
}

static int an8801r_led_set_blink(struct phy_device *phydev, u16 blink)
{
	int err;

	err = phy_write_mmd(phydev, 0x1f, LED_BLK_DUR,
			    LED_BLINK_DURATION(blink));
	if (err)
		return err;

	return phy_write_mmd(phydev, 0x1f, LED_ON_DUR,
			     (LED_BLINK_DURATION(blink) >> 1));
}

static int an8801r_led_set_mode(struct phy_device *phydev, u8 mode)
{
	switch (mode) {
	case AIR_LED_MODE_DISABLE:
		return air_modify_mmd(phydev, 0x1f, LED_BCR,
				      (LED_BCR_EXT_CTRL | LED_BCR_CLK_EN),
				      0x0);
	case AIR_LED_MODE_USER_DEFINE:
		return air_modify_mmd(phydev, 0x1f, LED_BCR,
				      (LED_BCR_EXT_CTRL | LED_BCR_CLK_EN),
				      (LED_BCR_EXT_CTRL | LED_BCR_CLK_EN));
	default:
		break;
	}
	dev_err(phydev_dev(phydev),
		"LED mode %d is not supported\n", mode);
	return -EINVAL;
}

static int an8801r_led_set_state(struct phy_device *phydev,
				 u8 entity, u8 state)
{
	return air_modify_mmd(phydev, 0x1f, LED_ON_CTRL(entity), LED_ON_EN,
			      (state) ? LED_ON_EN : 0x0);
}

static int an8801r_led_init(struct phy_device *phydev)
{
	struct an8801r_priv *priv = phydev_cfg(phydev);
	struct AIR_LED_CFG_T *led_cfg = priv->led_cfg;
	u16 led_blink_cfg = priv->led_blink_cfg;
	int ret, led_id;
	u32 data;

	ret = an8801r_led_set_blink(phydev, led_blink_cfg);
	if (ret != 0)
		return ret;

	ret = an8801r_led_set_mode(phydev, AIR_LED_MODE_USER_DEFINE);
	if (ret != 0) {
		dev_err(phydev_dev(phydev),
			"LED fail to set mode, ret %d !\n", ret);
		return ret;
	}

	for (led_id = AIR_LED0; led_id < MAX_LED_SIZE; led_id++) {
		ret = an8801r_led_set_state(phydev, led_id, led_cfg[led_id].en);
		if (ret != 0) {
			dev_err(phydev_dev(phydev),
				"LED fail to set LED(%d) state, ret %d !\n",
				led_id, ret);
			return ret;
		}
		if (led_cfg[led_id].en == LED_ENABLE) {
			data = BIT(led_cfg[led_id].gpio);
			ret |= air_buckpbus_reg_modify(phydev, 0x10000054, data, data);

			data = LED_GPIO_SEL(led_id, led_cfg[led_id].gpio);
			ret |= air_buckpbus_reg_modify(phydev, 0x10000058, data, data);

			data = BIT(led_cfg[led_id].gpio);
			ret |= air_buckpbus_reg_modify(phydev, 0x10000070, data, 0);

			ret |= an8801r_led_set_usr_def(phydev, led_id,
				led_cfg[led_id].pol,
				led_cfg[led_id].on_cfg,
				led_cfg[led_id].blk_cfg);
			if (ret != 0) {
				dev_err(phydev_dev(phydev),
					"Fail to set LED(%d) usr def, ret %d !\n",
					led_id, ret);
				return ret;
			}
		}
	}
	dev_info(phydev_dev(phydev), "LED initialize OK !\n");
	return 0;
}

static int an8801r_ack_interrupt(struct phy_device *phydev)
{
	u32 reg_val = 0;

	/* Reset WOL status */
	air_buckpbus_reg_write(phydev, 0x10285404, 0x102);
	air_buckpbus_reg_read(phydev, 0x10285400, &reg_val);
	air_buckpbus_reg_write(phydev, 0x10285400, 0x0);
	air_buckpbus_reg_write(phydev, 0x10285400, reg_val | 0x10);
	air_buckpbus_reg_write(phydev, 0x10285404, 0x12);
	/* Clear the interrupts by writing the reg */
	air_buckpbus_reg_write(phydev, 0x10285704, 0x1f);
	return 0;
}

static int an8801r_config_intr(struct phy_device *phydev)
{
	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		air_buckpbus_reg_write(phydev, 0x1000007c, 0x10000);
		air_buckpbus_reg_modify(phydev, 0x10285700, 0x1, 0x1);
	} else {
		air_buckpbus_reg_write(phydev, 0x1000007c, 0x0);
		air_buckpbus_reg_modify(phydev, 0x10285700, 0x1, 0);
	}
	an8801r_ack_interrupt(phydev);
	return 0;
}

static int an8801r_did_interrupt(struct phy_device *phydev)
{
	int err;
	u32 intr_cfg = 0, reg_val = 0;

	err = air_buckpbus_reg_read(phydev, 0x10285700, &intr_cfg);
	if (err)
		return err;

	err = air_buckpbus_reg_read(phydev, 0x10285704, &reg_val);
	if (err)
		return err;

	if (reg_val & 0x10)
		return 1;

	if ((intr_cfg & 0x1) && (reg_val & 0x1))
		return 1;

	return 0;
}

static irqreturn_t an8801r_handle_interrupt(struct phy_device *phydev)
{
	if (!an8801r_did_interrupt(phydev))
		return IRQ_NONE;

	an8801r_ack_interrupt(phydev);

	phy_trigger_machine(phydev);

	return IRQ_HANDLED;
}

static void an8801r_get_wol(struct phy_device *phydev,
			    struct ethtool_wolinfo *wol)
{
	u32 reg_val = 0;

	wol->supported = WAKE_MAGIC;
	wol->wolopts = 0;

	air_buckpbus_reg_read(phydev, 0x10285400, &reg_val);

	wol->wolopts = (reg_val & 0xE) ? WAKE_MAGIC : 0;
}

static int an8801r_set_wol(struct phy_device *phydev,
			   struct ethtool_wolinfo *wol)
{
	struct net_device *attach_dev = phydev->attached_dev;
	u32 reg_val;

	if (wol->wolopts & WAKE_MAGIC) {
		reg_val = (attach_dev->dev_addr[2] << 24) |
			  (attach_dev->dev_addr[3] << 16) |
			  (attach_dev->dev_addr[4] << 8) |
			  (attach_dev->dev_addr[5]);
		air_buckpbus_reg_write(phydev, 0x10285114, reg_val);
		reg_val = (attach_dev->dev_addr[0] << 8) |
			  (attach_dev->dev_addr[1]);
		air_buckpbus_reg_write(phydev, 0x10285118, reg_val);
		air_buckpbus_reg_modify(phydev, 0x10285400, 0xE, 0xE);
		air_buckpbus_reg_modify(phydev, 0x10285700, 0x10, 0x10);
	} else {
		air_buckpbus_reg_modify(phydev, 0x10285400, 0xE, 0x0);
		air_buckpbus_reg_modify(phydev, 0x10285700, 0x10, 0x0);
	}
	an8801r_ack_interrupt(phydev);
	return 0;
}

static int an8801r_of_init(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct device_node *of_node = dev->of_node;
	struct an8801r_priv *priv = phydev_cfg(phydev);
	u32 val = 0;

	if (of_find_property(of_node, "airoha,rxclk-delay", NULL)) {
		if (of_property_read_u32(of_node, "airoha,rxclk-delay",
					 &val) != 0) {
			dev_err(phydev_dev(phydev),
				"airoha,rxclk-delay value is invalid.");
			return -1;
		}
		if (val < AIR_RGMII_DELAY_NOSTEP ||
		    val > AIR_RGMII_DELAY_STEP_7) {
			dev_err(phydev_dev(phydev),
				"airoha,rxclk-delay value %u out of range.",
				val);
			return -1;
		}
		priv->rxdelay_force = TRUE;
		priv->rxdelay_step = val;
		priv->rxdelay_align = of_property_read_bool(of_node,
							    "airoha,rxclk-delay-align");
	}

	if (of_find_property(of_node, "airoha,txclk-delay", NULL)) {
		if (of_property_read_u32(of_node, "airoha,txclk-delay",
					 &val) != 0) {
			dev_err(phydev_dev(phydev),
				"airoha,txclk-delay value is invalid.");
			return -1;
		}
		if (val < AIR_RGMII_DELAY_NOSTEP ||
		    val > AIR_RGMII_DELAY_STEP_7) {
			dev_err(phydev_dev(phydev),
				"airoha,txclk-delay value %u out of range.",
				val);
			return -1;
		}
		priv->txdelay_force = TRUE;
		priv->txdelay_step = val;
	}

	if (of_find_property(of_node, "airoha,cko-output", NULL)) {
		if (of_property_read_u32(of_node, "airoha,cko-output",
					 &val) != 0) {
			dev_err(phydev_dev(phydev),
				"airoha,cko-output value is invalid.");
			return -1;
		}
		if (val < AIR_CKO_OUTPUT_RATE_25M ||
		    val > AIR_CKO_OUTPUT_RATE_125M) {
			dev_err(phydev_dev(phydev),
				"airoha,cko-output value %u out of range.",
				val);
			return -1;
		}
		priv->cko_output_en = TRUE;
		priv->cko_output_rate = val;
	}

	return 0;
}

static int an8801r_rgmii_rxdelay(struct phy_device *phydev, u16 delay, u8 align)
{
	u32 reg_val = delay & RGMII_DELAY_STEP_MASK;

	/* align */
	if (align) {
		reg_val |= RGMII_RXDELAY_ALIGN;
		dev_info(phydev_dev(phydev), "Rxdelay align\n");
	}
	reg_val |= RGMII_RXDELAY_FORCE_MODE;
	air_buckpbus_reg_write(phydev, 0x1021C02C, reg_val);
	reg_val = 0;
	air_buckpbus_reg_read(phydev, 0x1021C02C, &reg_val);
	dev_info(phydev_dev(phydev),
		 "Force rxdelay = %d(0x%x)\n", delay, reg_val);
	return 0;
}

static int an8801r_rgmii_txdelay(struct phy_device *phydev, u16 delay)
{
	u32 reg_val = delay & RGMII_DELAY_STEP_MASK;

	reg_val |= RGMII_TXDELAY_FORCE_MODE;
	air_buckpbus_reg_write(phydev, 0x1021C024, reg_val);
	reg_val = 0;
	air_buckpbus_reg_read(phydev, 0x1021C024, &reg_val);
	dev_info(phydev_dev(phydev),
		 "Force txdelay = %d(0x%x)\n", delay, reg_val);
	return 0;
}

static int an8801r_rgmii_delay_config(struct phy_device *phydev)
{
	struct an8801r_priv *priv = phydev_cfg(phydev);

	switch (phydev->interface) {
	case PHY_INTERFACE_MODE_RGMII_TXID:
		an8801r_rgmii_txdelay(phydev, AIR_RGMII_DELAY_STEP_4);
		break;
	case PHY_INTERFACE_MODE_RGMII_RXID:
		an8801r_rgmii_rxdelay(phydev, AIR_RGMII_DELAY_NOSTEP, TRUE);
		break;
	case PHY_INTERFACE_MODE_RGMII_ID:
		an8801r_rgmii_txdelay(phydev, AIR_RGMII_DELAY_STEP_4);
		an8801r_rgmii_rxdelay(phydev, AIR_RGMII_DELAY_NOSTEP, TRUE);
		break;
	case PHY_INTERFACE_MODE_RGMII:
	default:
		if (priv->rxdelay_force)
			an8801r_rgmii_rxdelay(phydev, priv->rxdelay_step,
					      priv->rxdelay_align);
		if (priv->txdelay_force)
			an8801r_rgmii_txdelay(phydev, priv->txdelay_step);

		break;
	}
	return 0;
}

static int an8801r_cko_config(struct phy_device *phydev)
{
	struct an8801r_priv *priv = phydev_cfg(phydev);

	if (priv->cko_output_en) {
		if (priv->cko_output_rate == AIR_CKO_OUTPUT_RATE_125M) {
			air_buckpbus_reg_write(phydev, 0x10000194, 0x80);
			air_buckpbus_reg_write(phydev, 0x100001A4, (0x3 << 10) |
					       ((AIR_CKO_OUT_DRV & 0xF) << 4) |
					       (0x2 << 2));
		} else if (priv->cko_output_rate == AIR_CKO_OUTPUT_RATE_25M) {
			air_buckpbus_reg_write(phydev, 0x10000194, 0x0);
			air_buckpbus_reg_write(phydev, 0x100001A4, (0x0 << 10) |
					       ((AIR_CKO_OUT_DRV & 0xF) << 4) |
					       (0x2 << 2));
		}
	} else {
		air_buckpbus_reg_write(phydev, 0x100001A4, 0x3);
	}

	return 0;
}

static int an8801r_config_init(struct phy_device *phydev)
{
	int ret;

	ret = an8801r_of_init(phydev);
	if (ret < 0)
		return ret;

	phy_write_mmd(phydev, 0x1f, 0x600, 0x1e);
	phy_write_mmd(phydev, 0x1f, 0x601, 0x2);
	phy_write_mmd(phydev, MDIO_MMD_AN, MDIO_AN_EEE_ADV, 0x0);
	mdiobus_lock(phydev);
	__phy_write(phydev, 0x1f, 0x1);
	__phy_write(phydev, 0x14, 0x3a14);
	__phy_write(phydev, 0x1f, 0x0);
	mdiobus_unlock(phydev);

	air_buckpbus_reg_write(phydev, 0x11F808D0, 0x180);

	air_buckpbus_reg_write(phydev, 0x1021c004, 0x1);
	air_buckpbus_reg_write(phydev, 0x10270004, 0x3f);
	air_buckpbus_reg_write(phydev, 0x10270104, 0xff);
	air_buckpbus_reg_write(phydev, 0x10270204, 0xff);

	phy_write_mmd(phydev, 0x1e, 0x13, 0x4040);
	phy_write_mmd(phydev, 0x1e, 0xD8, 0x1010);
	phy_write_mmd(phydev, 0x1e, 0xD9, 0x100);
	phy_write_mmd(phydev, 0x1e, 0xDA, 0x100);

	an8801r_rgmii_delay_config(phydev);
	an8801r_cko_config(phydev);

	ret = an8801r_led_init(phydev);
	if (ret != 0) {
		dev_err(phydev_dev(phydev),
			"LED initialize fail, ret %d !\n", ret);
		return ret;
	}
	dev_info(phydev_dev(phydev), "AN8801R Initialize OK ! (%s)\n",
		 AN8801R_DRIVER_VERSION);
	return 0;
}

#ifdef AN8801R_DEBUGFS
static ssize_t an8801r_mdio_write(struct file *file, const char __user *ptr,
				  size_t len, loff_t *off)
{
	struct phy_device *phydev = file->private_data;
	char buf[64], param1[32], param2[32];
	int count = len, ret = 0;
	unsigned int reg, devad, val;
	u16 reg_val;

	memset(buf, 0, 64);
	memset(param1, 0, 32);
	memset(param2, 0, 32);

	if (count > sizeof(buf) - 1)
		return -EINVAL;
	if (copy_from_user(buf, ptr, len))
		return -EFAULT;

	ret = sscanf(buf, "%s %s", param1, param2);
	if (ret < 0)
		return ret;

	if (!strncmp("cl22", param1, strlen("cl22"))) {
		if (!strncmp("w", param2, strlen("w"))) {
			if (sscanf(buf, "cl22 w %x %x", &reg, &val) == -1)
				return -EFAULT;
			pr_notice("\nphy=0x%x, reg=0x%x, val=0x%x\n",
				  phydev_phy_addr(phydev), reg, val);

			ret = phy_write(phydev, reg, val);
			if (ret < 0)
				return ret;
			pr_notice("\nphy=0x%x, reg=0x%x, val=0x%x confirm..\n",
				  phydev_phy_addr(phydev), reg,
				  phy_read(phydev, reg));
		} else if (!strncmp("r", param2, strlen("r"))) {
			if (sscanf(buf, "cl22 r %x", &reg) == -1)
				return -EFAULT;
			pr_notice("\nphy=0x%x, reg=0x%x, val=0x%x\n",
				  phydev_phy_addr(phydev), reg,
				  phy_read(phydev, reg));
		} else {
			pr_notice(AN8801R_DEBUGFS_MDIO_HELP_STRING);
			return -EINVAL;
		}
	} else if (!strncmp("cl45", param1, strlen("cl45"))) {
		if (!strncmp("w", param2, strlen("w"))) {
			if (sscanf(buf, "cl45 w %x %x %x", &devad, &reg, &val) == -1)
				return -EFAULT;
			pr_notice("\nphy=0x%x, devad=0x%x, reg=0x%x, val=0x%x\n",
				  phydev_phy_addr(phydev), devad, reg, val);

			ret = phy_write_mmd(phydev, devad, reg, val);
			if (ret < 0)
				return ret;
			reg_val = phy_read_mmd(phydev, devad, reg);
			pr_notice("\nphy=0x%x, devad=0x%x, reg=0x%x, val=0x%x confirm..\n",
				  phydev_phy_addr(phydev), devad, reg, reg_val);
		} else if (!strncmp("r", param2, strlen("r"))) {
			if (sscanf(buf, "cl45 r %x %x", &devad, &reg) == -1)
				return -EFAULT;
			reg_val = phy_read_mmd(phydev, devad, reg);
			pr_notice("\nphy=0x%x, devad=0x%x, reg=0x%x, val=0x%x\n",
				  phydev_phy_addr(phydev), devad, reg, reg_val);
		} else {
			pr_notice(AN8801R_DEBUGFS_MDIO_HELP_STRING);
			return -EINVAL;
		}
	} else {
		pr_notice(AN8801R_DEBUGFS_MDIO_HELP_STRING);
		return -EINVAL;
	}

	return count;
}

static int an8801r_counter_show(struct seq_file *seq, void *v)
{
	struct phy_device *phydev = seq->private;
	int ret = 0;
	u32 pkt_cnt = 0;

	seq_puts(seq, "|\t<<EFIFO COUNTER>>\n");
	seq_puts(seq, "| Rx from Line side_S     :");
	air_buckpbus_reg_read(phydev, 0x10270030, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx from Line side_E     :");
	air_buckpbus_reg_read(phydev, 0x10270034, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to System side_S     :");
	air_buckpbus_reg_read(phydev, 0x10270038, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to System side_E     :");
	air_buckpbus_reg_read(phydev, 0x1027003C, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx from System side_S   :");
	air_buckpbus_reg_read(phydev, 0x10270020, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx from System side_E   :");
	air_buckpbus_reg_read(phydev, 0x10270024, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to Line side_S       :");
	air_buckpbus_reg_read(phydev, 0x10270028, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to Line side_E       :");
	air_buckpbus_reg_read(phydev, 0x1027002C, &pkt_cnt);
	seq_printf(seq, "%010u |\n", pkt_cnt);

	ret = air_buckpbus_reg_write(phydev, 0x1027001C, 0x3);
	if (ret < 0)
		return ret;

	seq_puts(seq, "|\t<<LS Counter>>\n");
	ret = phy_write(phydev, 0x1f, 1);
	if (ret < 0)
		return ret;
	seq_puts(seq, "| Rx from Line side       :");
	pkt_cnt = phy_read(phydev, 0x12) & 0x7fff;
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx Error from Line side :");
	pkt_cnt = phy_read(phydev, 0x17) & 0xff;
	seq_printf(seq, "%010u |\n", pkt_cnt);

	ret = phy_write(phydev, 0x1f, 0);
	if (ret < 0)
		return ret;
	ret = phy_write(phydev, 0x1f, 0x52B5);
	if (ret < 0)
		return ret;
	ret = phy_write(phydev, 0x10, 0xBF92);
	if (ret < 0)
		return ret;

	seq_puts(seq, "| Tx to Line side         :");
	pkt_cnt = (phy_read(phydev, 0x11) & 0x7ffe) >> 1;
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx Error to Line side   :");
	pkt_cnt = phy_read(phydev, 0x12);
	pkt_cnt &= 0x7f;
	seq_printf(seq, "%010u |\n\n", pkt_cnt);
	ret = phy_write(phydev, 0x1f, 0);
	if (ret < 0)
		return ret;

	return ret;
}

static int an8801r_counter_open(struct inode *inode, struct file *file)
{
	return single_open(file, an8801r_counter_show, inode->i_private);
}

static int an8801r_debugfs_pbus_help(void)
{
	pr_notice(AN8801R_DEBUGFS_PBUS_HELP_STRING);
	return 0;
}

static ssize_t an8801r_debugfs_pbus(struct file *file,
				    const char __user *buffer, size_t count,
				    loff_t *data)
{
	struct phy_device *phydev = file->private_data;
	char buf[64];
	int ret = 0;
	unsigned int reg;
	u32 val = 0;

	memset(buf, 0, 64);

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	if (buf[0] == 'w') {
		if (sscanf(buf, "w %x %x", &reg, &val) == -1)
			return -EFAULT;

		pr_notice("\nphy=0x%x, reg=0x%x, val=0x%x\n",
			  phydev_phy_addr(phydev), reg, val);

		ret = air_buckpbus_reg_write(phydev, reg, val);
		if (ret < 0)
			return ret;

		val = 0;
		air_buckpbus_reg_read(phydev, reg, &val);
		pr_notice("\nphy=%d, reg=0x%x, val=0x%x confirm..\n",
			  phydev_phy_addr(phydev), reg, val);
	} else if (buf[0] == 'r') {
		if (sscanf(buf, "r %x", &reg) == -1)
			return -EFAULT;

		air_buckpbus_reg_read(phydev, reg, &val);
		pr_notice("\nphy=0x%x, reg=0x%x, val=0x%x\n",
			  phydev_phy_addr(phydev), reg, val);
	} else if (buf[0] == 'h') {
		an8801r_debugfs_pbus_help();
	}

	return count;
}

static int an8801r_info_show(struct seq_file *seq, void *v)
{
	struct phy_device *phydev = seq->private;
	u32 pbus_data = 0;
	int reg = 0;

	seq_puts(seq, "\t<<AIR AN8801R Info>>\n");
	air_buckpbus_reg_read(phydev, 0x10005004, &pbus_data);
	seq_printf(seq, "| Product Version : E%d\n", pbus_data);
	seq_printf(seq, "| Driver Version  : %s\n", AN8801R_DRIVER_VERSION);
	air_buckpbus_reg_read(phydev, 0x10000094, &pbus_data);
	seq_printf(seq, "| RG_HW_STRAP     : 0x%08x\n", pbus_data);
	for (reg = MII_BMCR; reg <= MII_STAT1000; reg++) {
		if (reg <= MII_LPA || reg >= MII_CTRL1000)
			seq_printf(seq, "| RG_MII 0x%02x     : 0x%08x\n",
				   reg, phy_read(phydev, reg));
	}
	seq_puts(seq, "\n");
	return 0;
}

static int an8801r_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, an8801r_info_show, inode->i_private);
}

static const struct file_operations an8801r_info_fops = {
	.owner = THIS_MODULE,
	.open = an8801r_info_open,
	.read = seq_read,
	.llseek = noop_llseek,
	.release = single_release,
};

static const struct file_operations an8801r_counter_fops = {
	.owner = THIS_MODULE,
	.open = an8801r_counter_open,
	.read = seq_read,
	.llseek = noop_llseek,
	.release = single_release,
};

static const struct file_operations an8801r_debugfs_pbus_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = an8801r_debugfs_pbus,
	.llseek = noop_llseek,
};

static const struct file_operations an8801r_mdio_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = an8801r_mdio_write,
	.llseek = noop_llseek,
};

static int an8801r_debugfs_init(struct phy_device *phydev)
{
	int ret = 0;
	struct an8801r_priv *priv = phydev->priv;

	dev_info(phydev_dev(phydev), "Debugfs init start\n");
	priv->debugfs_root =
		debugfs_create_dir(dev_name(phydev_dev(phydev)), NULL);
	if (!priv->debugfs_root) {
		dev_err(phydev_dev(phydev), "Debugfs init err\n");
		ret = -ENOMEM;
	}
	debugfs_create_file(DEBUGFS_INFO, 0444,
			    priv->debugfs_root, phydev,
			    &an8801r_info_fops);
	debugfs_create_file(DEBUGFS_COUNTER, 0644,
			    priv->debugfs_root, phydev,
			    &an8801r_counter_fops);
	debugfs_create_file(DEBUGFS_PBUS_OP, S_IFREG | 0200,
			    priv->debugfs_root, phydev,
			    &an8801r_debugfs_pbus_fops);
	debugfs_create_file(DEBUGFS_MDIO, S_IFREG | 0200,
			    priv->debugfs_root, phydev,
			    &an8801r_mdio_fops);
	return ret;
}

static void air_debugfs_remove(struct phy_device *phydev)
{
	struct an8801r_priv *priv = phydev->priv;

	debugfs_remove_recursive(priv->debugfs_root);
}
#endif /*AN8801R_DEBUGFS*/

static int an8801r_phy_probe(struct phy_device *phydev)
{
	u32 reg_val, phy_id, led_id;
	struct device *dev = &phydev->mdio.dev;
	struct an8801r_priv *priv = NULL;
#ifdef AN8801R_DEBUGFS
	int ret = 0;
#endif

	reg_val = phy_read(phydev, 2);
	phy_id = reg_val << 16;
	reg_val = phy_read(phydev, 3);
	phy_id |= reg_val;
	dev_info(phydev_dev(phydev), "PHY-ID = %x\n", phy_id);

	if (phy_id != AN8801R_PHY_ID) {
		dev_err(phydev_dev(phydev),
			"AN8801R can't be detected.\n");
		return -1;
	}

	priv = devm_kzalloc(dev, sizeof(struct an8801r_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	for (led_id = AIR_LED0; led_id < MAX_LED_SIZE; led_id++)
		priv->led_cfg[led_id] = led_cfg_dlt[led_id];

	priv->led_blink_cfg  = led_blink_cfg_dlt;
	priv->rxdelay_force  = rxdelay_force;
	priv->txdelay_force  = txdelay_force;
	priv->rxdelay_step   = rxdelay_step;
	priv->rxdelay_align  = rxdelay_align;
	priv->txdelay_step   = txdelay_step;

	phydev->priv = priv;

#ifdef AN8801R_DEBUGFS
	ret = an8801r_debugfs_init(phydev);
	if (ret < 0) {
		dev_info(phydev_dev(phydev), "AN8801R debugfs init failed\n");
		air_debugfs_remove(phydev);
		kfree(priv);
		return ret;
	}
#endif
	return 0;
}

static void an8801r_phy_remove(struct phy_device *phydev)
{
	struct an8801r_priv *priv = (struct an8801r_priv *)phydev->priv;

	if (priv) {
#ifdef AN8801R_DEBUGFS
		air_debugfs_remove(phydev);
#endif
		kfree(priv);
		phydev->priv = NULL;
	}
}

static int an8801r_read_status(struct phy_device *phydev)
{
	int ret, prespeed = phydev->speed;

	ret = genphy_read_status(phydev);
	if (phydev->link == LINK_DOWN) {
		prespeed = 0;
		phydev->speed = 0;
	}
	if (prespeed != phydev->speed && phydev->link == LINK_UP) {
		prespeed = phydev->speed;
		dev_dbg(phydev_dev(phydev), "AN8801R SPEED %d\n", prespeed);
		if (prespeed == SPEED_1000)
			air_buckpbus_reg_modify(phydev, 0x10005054, BIT(0), BIT(0));
		else
			air_buckpbus_reg_modify(phydev, 0x10005054, BIT(0), 0);
	}
	return ret;
}

static struct phy_driver airoha_driver[] = {
	{
		.phy_id           = AN8801R_PHY_ID,
		.name             = "Airoha AN8801R",
		.phy_id_mask      = 0x0ffffff0,
		.features         = PHY_GBIT_FEATURES,
		.config_init      = an8801r_config_init,
		.config_aneg      = genphy_config_aneg,
		.probe            = an8801r_phy_probe,
		.remove           = an8801r_phy_remove,
		.read_status      = an8801r_read_status,
		.config_intr      = an8801r_config_intr,
		.handle_interrupt = an8801r_handle_interrupt,
		.set_wol          = an8801r_set_wol,
		.get_wol          = an8801r_get_wol,
		.suspend          = genphy_suspend,
		.resume           = genphy_resume,
	}
};

module_phy_driver(airoha_driver);

static struct mdio_device_id __maybe_unused airoha_tbl[] = {
	{ AN8801R_PHY_ID, 0x0ffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, airoha_tbl);
