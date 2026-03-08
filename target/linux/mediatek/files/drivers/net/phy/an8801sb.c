// SPDX-License-Identifier: GPL-2.0
/*PURPOSE:
 *     Airoha AN8801SB/R PHY driver for Linux
 *NOTES:
 *
 */

/*INCLUDE FILE DECLARATIONS
 */

#include <linux/of_device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/version.h>
#include <linux/debugfs.h>
#include <linux/of.h>

#include "an8801sb.h"

MODULE_DESCRIPTION("Airoha AN8801 PHY drivers");
MODULE_AUTHOR("Airoha");
MODULE_LICENSE("GPL");

#if (KERNEL_VERSION(4, 5, 0) > LINUX_VERSION_CODE)
#define phydev_mdiobus(_dev) (_dev->bus)
#define phydev_phy_addr(_dev) (_dev->addr)
#define phydev_dev(_dev) (&_dev->dev)
#else
#define phydev_mdiobus(_dev) (_dev->mdio.bus)
#define phydev_phy_addr(_dev) (_dev->mdio.addr)
#define phydev_dev(_dev) (&_dev->mdio.dev)
#endif
#define phydev_cfg(phy)            ((struct an8801_priv *)(phy)->priv)
#define phydev_mdiobus_lock(phy)   (phydev_mdiobus(phy)->mdio_lock)
#define mdiobus_lock(phy)          (mutex_lock(&phydev_mdiobus_lock(phy)))
#define mdiobus_unlock(phy)        (mutex_unlock(&phydev_mdiobus_lock(phy)))

#define MAX_SGMII_AN_RETRY              (100)
#define MCS_LINK_STATUS_MASK            (BIT(2))

#ifdef AN8801SB_DEBUGFS
#define AN8801_DEBUGFS_POLARITY_HELP_STRING \
	"\nUsage: echo [tx_polarity] [rx_polarity] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/polarity" \
	"\npolarity: tx_normal, tx_reverse, rx_normal, rx_reverse" \
	"\ntx_normal is tx polarity is normal." \
	"\ntx_reverse is tx polarity need to be swapped." \
	"\nrx_normal is rx polarity is normal." \
	"\nrx_reverse is rx polarity need to be swapped." \
	"\nFor example tx polarity need to be swapped. " \
	"But rx polarity is normal." \
	"\necho tx_reverse rx_normal > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/polarity" \
	"\n"
#define AN8801_DEBUGFS_RX_ERROR_STRING \
	"\nRx param is not correct." \
	"\nrx_normal: rx polarity is normal." \
	"rx_reverse: rx polarity is reverse.\n"
#define AN8801_DEBUGFS_TX_ERROR_STRING \
	"\nTx param is not correct." \
	"\ntx_normal: tx polarity is normal." \
	"tx_reverse: tx polarity is reverse.\n"
#define AN8801_DEBUGFS_PBUS_HELP_STRING \
	"\nUsage: echo w [pbus_addr] [pbus_reg] [value] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_reg_op" \
	"\n       echo r [pbus_addr] [pbus_reg] > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_op" \
	"\nRead example: PBUS addr 0x19, Register 0x19a4" \
	"\necho r 19 19a4 > /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_reg_op" \
	"\nWrite example: PBUS addr 0x19, Register 0xcf8 0x1a01503" \
	"\necho w 19 cf8 1a01503> /sys/" \
	"kernel/debug/mdio-bus\':[phy_addr]/pbus_op" \
	"\n"
#define AN8801_DEBUGFS_MDIO_HELP_STRING \
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
#define R50_SHIFT (-7)
static const u16 r50ohm_table[] = {
127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
127, 127, 127, 127, 127, 127, 127, 127, 127, 124,
120, 116, 112, 108, 104, 100,  96,  93,  90,  86,
84,   80,  77,  74,  72,  68,  65,  64,  61,  59,
56,   54,  52,  48,  48,  45,  43,  40,  39,  36,
35,   32,  32,  30,  28,  26,  24,  23,  21,  20,
18,   16,  16,  14
};
static const u16 r50ohm_table_size = sizeof(r50ohm_table)/sizeof(u16);
static const struct AIR_LED_CFG_T led_cfg_dlt[MAX_LED_SIZE] = {
//   LED Enable,          GPIO,    LED Polarity,      LED ON,    LED Blink
	/* LED0 */
	{LED_ENABLE, AIR_LED_GPIO5, AIR_ACTIVE_LOW,  AIR_LED0_ON, AIR_LED0_BLK},
	/* LED1 */
	{LED_ENABLE, AIR_LED_GPIO8, AIR_ACTIVE_LOW,  AIR_LED1_ON, AIR_LED1_BLK},
	/* LED2 */
	{LED_ENABLE, AIR_LED_GPIO9, AIR_ACTIVE_LOW,  AIR_LED2_ON, AIR_LED2_BLK},
};
static const u16 led_blink_cfg_dlt = AIR_LED_BLK_DUR_64M;
/* RGMII delay */
static const u8 rxdelay_force = FALSE;
static const u8 txdelay_force = FALSE;
static const u16 rxdelay_step = AIR_RGMII_DELAY_NOSTEP;
static const u8 rxdelay_align = FALSE;
static const u16 txdelay_step = AIR_RGMII_DELAY_NOSTEP;
/* User-defined.E */

/************************************************************************
 *                  F U N C T I O N S
 ************************************************************************/
static int __air_buckpbus_reg_write(struct phy_device *phydev, u32 addr,
				    u32 data)
{
	int err = 0;
	int phy_addr = phydev_phy_addr(phydev);
	struct mii_bus *mbus = phydev_mdiobus(phydev);

	err = mbus->write(mbus, phy_addr, 0x1F, 4);
	err |= mbus->write(mbus, phy_addr, 0x10, 0);
	err |= mbus->write(mbus, phy_addr, 0x11, (u16)(addr >> 16));
	err |= mbus->write(mbus, phy_addr, 0x12, (u16)(addr & 0xffff));
	err |= mbus->write(mbus, phy_addr, 0x13, (u16)(data >> 16));
	err |= mbus->write(mbus, phy_addr, 0x14, (u16)(data & 0xffff));
	err |= mbus->write(mbus, phy_addr, 0x1F, 0);
	return err;
}

static u32 __air_buckpbus_reg_read(struct phy_device *phydev, u32 addr)
{
	int err = 0;
	u32 data_h, data_l, data;
	int phy_addr = phydev_phy_addr(phydev);
	struct mii_bus *mbus = phydev_mdiobus(phydev);

	err = mbus->write(mbus, phy_addr, 0x1F, 4);
	err |= mbus->write(mbus, phy_addr, 0x10, 0);
	err |= mbus->write(mbus, phy_addr, 0x15, (u16)(addr >> 16));
	err |= mbus->write(mbus, phy_addr, 0x16, (u16)(addr & 0xffff));
	if (err < 0)
		return INVALID_DATA;
	data_h = mbus->read(mbus, phy_addr, 0x17);
	data_l = mbus->read(mbus, phy_addr, 0x18);
	err |= mbus->write(mbus, phy_addr, 0x1F, 0);
	if (err < 0)
		return INVALID_DATA;

	data = ((data_h & 0xffff) << 16) | (data_l & 0xffff);
	return data;
}

static u32 __air_buckpbus_reg_modify(struct phy_device *phydev, u32 addr,
					u32 mask, u32 set)
{
	int err = 0;
	u32 data_h, data_l, data_old, data_new;
	int phy_addr = phydev_phy_addr(phydev);
	struct mii_bus *mbus = phydev_mdiobus(phydev);

	err = mbus->write(mbus, phy_addr, 0x1F, 4);
	err |= mbus->write(mbus, phy_addr, 0x10, 0);
	err |= mbus->write(mbus, phy_addr, 0x15, (u16)(addr >> 16));
	err |= mbus->write(mbus, phy_addr, 0x16, (u16)(addr & 0xffff));
	data_h = mbus->read(mbus, phy_addr, 0x17);
	data_l = mbus->read(mbus, phy_addr, 0x18);
	if (err < 0) {
		mbus->write(mbus, phy_addr, 0x1F, 0);
		return INVALID_DATA;
	}

	data_old = ((data_h & 0xffff) << 16) | (data_l & 0xffff);
	data_new = (data_old & ~mask) | set;
	if (data_new == data_old) {
		mbus->write(mbus, phy_addr, 0x1F, 0);
		return 0;
	}

	err |= mbus->write(mbus, phy_addr, 0x11, (u16)(addr >> 16));
	err |= mbus->write(mbus, phy_addr, 0x12, (u16)(addr & 0xffff));
	err |= mbus->write(mbus, phy_addr, 0x13, (u16)(data_new >> 16));
	err |= mbus->write(mbus, phy_addr, 0x14, (u16)(data_new & 0xffff));
	err |= mbus->write(mbus, phy_addr, 0x1F, 0);

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

static u32 air_buckpbus_reg_read(struct phy_device *phydev, u32 addr)
{
	u32 data;

	mdiobus_lock(phydev);
	data = __air_buckpbus_reg_read(phydev, addr);
	mdiobus_unlock(phydev);

	return data;
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

static int air_sw_reset(struct phy_device *phydev)
{
	u32 reg_value;
	u8 retry = MAX_RETRY;

	reg_value = phy_read(phydev, MII_BMCR);
	reg_value |= BMCR_RESET;
	phy_write(phydev, MII_BMCR, reg_value);
	do {
		mdelay(10);
		reg_value = phy_read(phydev, MII_BMCR);
		retry--;
		if (retry == 0) {
			dev_err(phydev_dev(phydev), "Reset fail !\n");
			return -1;
		}
	} while (reg_value & BMCR_RESET);

	return 0;
}

static int an8801_led_set_usr_def(struct phy_device *phydev, u8 entity,
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

static int an8801_led_set_mode(struct phy_device *phydev, u8 mode)
{
	switch (mode) {
	case AIR_LED_MODE_DISABLE:
		return phy_modify_mmd(phydev, 0x1f, LED_BCR,
						(LED_BCR_EXT_CTRL | LED_BCR_CLK_EN),
						0x0);
	case AIR_LED_MODE_USER_DEFINE:
		return phy_modify_mmd(phydev, 0x1f, LED_BCR,
						(LED_BCR_EXT_CTRL | LED_BCR_CLK_EN),
						(LED_BCR_EXT_CTRL | LED_BCR_CLK_EN));
	default:
		break;
	}
	dev_err(phydev_dev(phydev),
				"LED mode %d is not supported\n", mode);
	return -EINVAL;
}

static int an8801_led_set_state(struct phy_device *phydev, u8 entity, u8 state)
{
	return phy_modify_mmd(phydev, 0x1f, LED_ON_CTRL(entity), LED_ON_EN,
					(state) ? LED_ON_EN : 0x0);
}

static int an8801_led_init(struct phy_device *phydev)
{
	struct an8801_priv *priv = phydev_cfg(phydev);
	struct AIR_LED_CFG_T *led_cfg = priv->led_cfg;
	int ret, led_id;
	u32 data;
	u16 led_blink_cfg = priv->led_blink_cfg;

	ret = phy_write_mmd(phydev, 0x1f, LED_BLK_DUR,
				 LED_BLINK_DURATION(led_blink_cfg));
	if (ret < 0)
		return ret;

	ret = phy_write_mmd(phydev, 0x1f, LED_ON_DUR,
				 (LED_BLINK_DURATION(led_blink_cfg) >> 1));
	if (ret < 0)
		return ret;

	ret = an8801_led_set_mode(phydev, AIR_LED_MODE_USER_DEFINE);
	if (ret != 0) {
		dev_err(phydev_dev(phydev),
				"LED fail to set mode, ret %d !\n", ret);
		return ret;
	}

	for (led_id = AIR_LED0; led_id < MAX_LED_SIZE; led_id++) {
		ret = an8801_led_set_state(phydev, led_id, led_cfg[led_id].en);
		if (ret != 0) {
			dev_err(phydev_dev(phydev),
				   "LED fail to set LED(%d) state, ret %d !\n",
				   led_id, ret);
			return ret;
		}
		if (led_cfg[led_id].en == LED_ENABLE) {
			data = air_buckpbus_reg_read(phydev, 0x10000054);
			data |= BIT(led_cfg[led_id].gpio);
			ret |= air_buckpbus_reg_write(phydev, 0x10000054, data);

			data = air_buckpbus_reg_read(phydev, 0x10000058);
			data |= LED_GPIO_SEL(led_id, led_cfg[led_id].gpio);
			ret |= air_buckpbus_reg_write(phydev, 0x10000058, data);

			data = air_buckpbus_reg_read(phydev, 0x10000070);
			data &= ~BIT(led_cfg[led_id].gpio);
			ret |= air_buckpbus_reg_write(phydev, 0x10000070, data);

			ret |= an8801_led_set_usr_def(phydev, led_id,
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

static int an8801_ack_interrupt(struct phy_device *phydev)
{
	u32 reg_val = 0;

	air_buckpbus_reg_write(phydev, 0x10285404, 0x102);
	reg_val = air_buckpbus_reg_read(phydev, 0x10285400);
	air_buckpbus_reg_write(phydev, 0x10285400, 0x0);
	air_buckpbus_reg_write(phydev, 0x10285400, reg_val | 0x10);
	air_buckpbus_reg_write(phydev, 0x10285404, 0x12);
	air_buckpbus_reg_write(phydev, 0x10285704, 0x1f);
	return 0;
}

static int an8801_config_intr(struct phy_device *phydev)
{
	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		air_buckpbus_reg_write(phydev, 0x1000007c, BIT(AIR_INTERRUPT_GPIO) << 16);
		air_buckpbus_reg_modify(phydev, 0x10285700, 0x1, 0x1);
	} else {
		air_buckpbus_reg_write(phydev, 0x1000007c, 0x0);
		air_buckpbus_reg_modify(phydev, 0x10285700, 0x1, 0x0);
	}
	an8801_ack_interrupt(phydev);
	return 0;
}

static int an8801_did_interrupt(struct phy_device *phydev)
{
	u32 reg_val = 0;

	reg_val = air_buckpbus_reg_read(phydev, 0x10285704);

	if (reg_val & 0x11)
		return 1;

	return 0;
}

#if (KERNEL_VERSION(5, 11, 0) < LINUX_VERSION_CODE)
static irqreturn_t an8801_handle_interrupt(struct phy_device *phydev)
{
	if (!an8801_did_interrupt(phydev))
		return IRQ_NONE;

	an8801_ack_interrupt(phydev);
	phy_trigger_machine(phydev);
	return IRQ_HANDLED;
}
#endif

static int findClosestNumber(const u16 *arr, u16 size, u16 target)
{
	int left = 0, right = size - 1;

	while (left <= right) {
		int mid = left + ((right - left) >> 2);

		if (arr[mid] == target)
			return mid;

		if (arr[mid] < target)
			right = mid - 1;
		else
			left = mid + 1;
	}

	if (left > size - 1)
		return (size - 1);
	else
		return ((left - 1) >= 0 ? (left - 1) : 0);
}

static int an8801sb_i2mpb_config(struct phy_device *phydev)
{
	int ret = 0;
	u16 cl45_value = 0, temp_cl45 = 0;
	u16 mask = 0;

	cl45_value = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x12);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, cl45_value);
	cl45_value = (cl45_value & GENMASK(15, 10)) + (6 << 10);
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x12, GENMASK(15, 10), cl45_value);
	if (ret < 0)
		return ret;
	temp_cl45 = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x16);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, temp_cl45);
	mask = GENMASK(15, 10) | GENMASK(5, 0);
	cl45_value = (temp_cl45 & GENMASK(15, 10)) + (9 << 10);
	cl45_value = ((temp_cl45 & GENMASK(5, 0)) + 6) | cl45_value;
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x16, mask, cl45_value);
	if (ret < 0)
		return ret;
	cl45_value = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x17);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, cl45_value);
	cl45_value = (cl45_value & GENMASK(13, 8)) + (6 << 8);
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x17, GENMASK(13, 8), cl45_value);
	if (ret < 0)
		return ret;
	temp_cl45 = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x18);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, temp_cl45);
	mask = GENMASK(13, 8) | GENMASK(5, 0);
	cl45_value = (temp_cl45 & GENMASK(13, 8)) + (9 << 8);
	cl45_value = ((temp_cl45 & GENMASK(5, 0)) + 6) | cl45_value;
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x18, mask, cl45_value);
	if (ret < 0)
		return ret;
	cl45_value = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x19);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, cl45_value);
	cl45_value = (cl45_value & GENMASK(13, 8)) + (6 << 8);
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x19, GENMASK(13, 8), cl45_value);
	if (ret < 0)
		return ret;
	cl45_value = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x20);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, cl45_value);
	cl45_value = (cl45_value & GENMASK(5, 0)) + 6;
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x20, GENMASK(5, 0), cl45_value);
	if (ret < 0)
		return ret;
	cl45_value = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x21);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, cl45_value);
	cl45_value = (cl45_value & GENMASK(13, 8)) + (6 << 8);
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x21, GENMASK(13, 8), cl45_value);
	if (ret < 0)
		return ret;
	cl45_value = phy_read_mmd(phydev, MMD_DEV_VSPEC1, 0x22);
	dev_dbg(phydev_dev(phydev), "%s:%d cl45_value 0x%x!\n", __func__, __LINE__, cl45_value);
	cl45_value = (cl45_value & GENMASK(5, 0)) + 6;
	ret = phy_modify_mmd(phydev, MMD_DEV_VSPEC1, 0x22, GENMASK(5, 0), cl45_value);
	if (ret < 0)
		return ret;
	ret = phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x23, 0x883);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x24, 0x883);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x25, 0x883);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x26, 0x883);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x0, 0x100);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x1, 0x1bc);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x2, 0x1d0);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x3, 0x186);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x4, 0x202);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x5, 0x20e);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x6, 0x300);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x7, 0x3c0);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x8, 0x3d0);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0x9, 0x317);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0xa, 0x206);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC1, 0xb, 0xe);
	if (ret < 0)
		return ret;

	dev_info(phydev_dev(phydev), "I2MPB Initialize OK\n");
	return ret;
}

static void update_r50_value(struct phy_device *phydev,
	u16 *cl45_value, int pos1, int pos2)
{
	*cl45_value &= ~(0x007f << 8);
	*cl45_value |= ((r50ohm_table[pos1]) & 0x007f) << 8;
	*cl45_value &= ~(0x007f);
	*cl45_value |= (r50ohm_table[pos2]) & 0x007f;
	dev_dbg(phydev_dev(phydev), "Read: r50ohm_tx_1=%d r50ohm_tx_2=%d\n",
		r50ohm_table[pos1], r50ohm_table[pos2]);
}

static int calculate_position(int pos, int shift, int table_size)
{
	if (shift > 0)
		return (pos + shift < table_size) ? (pos + shift) : (table_size - 1);
	else
		return (pos + shift > 0) ? (pos + shift) : 0;
}

static int process_r50(struct phy_device *phydev, int reg,
	u16 *cl45_value, u16 *r50ohm_tx_a, u16 *r50ohm_tx_b)
{
	int pos1 = findClosestNumber(r50ohm_table, r50ohm_table_size, *r50ohm_tx_a);
	int pos2 = findClosestNumber(r50ohm_table, r50ohm_table_size, *r50ohm_tx_b);

	if (pos1 != -1 && pos2 != -1) {
		pos1 = calculate_position(pos1, R50_SHIFT, r50ohm_table_size);
		pos2 = calculate_position(pos2, R50_SHIFT, r50ohm_table_size);

		update_r50_value(phydev, cl45_value, pos1, pos2);
		return phy_write_mmd(phydev, 0x1e, reg, *cl45_value);
	}
	return 0;
}

#ifdef CONFIG_OF
static int an8801r_of_init(struct phy_device *phydev)
{
	struct device_node *of_node = phydev_dev(phydev)->of_node;
	struct an8801_priv *priv = phydev_cfg(phydev);
	u32 val = 0;

	if (of_find_property(of_node, "airoha,rxclk-delay", NULL)) {
		if (of_property_read_u32(of_node, "airoha,rxclk-delay",
					 &val) != 0) {
			dev_err(phydev_dev(phydev), "airoha,rxclk-delay value is invalid.");
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
	return 0;
}

static int an8801sb_of_init(struct phy_device *phydev)
{
	struct device_node *of_node = phydev_dev(phydev)->of_node;
	struct an8801_priv *priv = phydev_cfg(phydev);
	u32 val = 0;

	if (of_find_property(of_node, "airoha,polarity", NULL)) {
		if (of_property_read_u32(of_node, "airoha,polarity",
					 &val) != 0) {
			dev_err(phydev_dev(phydev), "airoha,polarity value is invalid.");
			return -1;
		}
		if (val < AIR_POL_TX_NOR_RX_REV ||
			val > AIR_POL_TX_REV_RX_NOR) {
			dev_err(phydev_dev(phydev),
					"airoha,polarity value %u out of range.",
					val);
			return -1;
		}
		priv->pol = val;
	} else
		priv->pol = AIR_POL_TX_NOR_RX_NOR;

	if (of_find_property(of_node, "airoha,surge", NULL)) {
		if (of_property_read_u32(of_node, "airoha,surge",
					 &val) != 0) {
			dev_err(phydev_dev(phydev), "airoha,surge value is invalid.");
			return -1;
		}
		if (val < AIR_SURGE_0R ||
			val > AIR_SURGE_5R) {
			dev_err(phydev_dev(phydev),
					"airoha,surge value %u out of range.",
					val);
			return -1;
		}
		priv->surge = val;
	} else
		priv->surge = AIR_SURGE_0R;

	if (of_find_property(of_node, "airoha,sgmii-mode", NULL)) {
		if (of_property_read_u32(of_node, "airoha,sgmii-mode",
					 &val) != 0) {
			dev_err(phydev_dev(phydev), "airoha,sgmii-mode value is invalid.");
			return -1;
		}
		if (val < AIR_SGMII_AN ||
			val > AIR_SGMII_FORCE) {
			dev_err(phydev_dev(phydev),
					"airoha,sgmii-mode value %u out of range.",
					val);
			return -1;
		}
		priv->sgmii_mode = val;
	} else
		priv->sgmii_mode = AIR_SGMII_AN;

	return 0;
}
#else
static int an8801r_of_init(struct phy_device *phydev)
{
	return 0;
}
static int an8801sb_of_init(struct phy_device *phydev)
{
	return 0;
}
#endif


static int an8801r_rgmii_rxdelay(struct phy_device *phydev, u16 delay, u8 align)
{
	u32 reg_val = delay & RGMII_DELAY_STEP_MASK;

	if (align) {
		reg_val |= RGMII_RXDELAY_ALIGN;
		dev_info(phydev_dev(phydev), "Rxdelay align\n");
	}
	reg_val |= RGMII_RXDELAY_FORCE_MODE;
	air_buckpbus_reg_write(phydev, 0x1021C02C, reg_val);
	reg_val = air_buckpbus_reg_read(phydev, 0x1021C02C);
	dev_info(phydev_dev(phydev),
		"Force rxdelay = %d(0x%x)\n", delay, reg_val);
	return 0;
}

static int an8801r_rgmii_txdelay(struct phy_device *phydev, u16 delay)
{
	u32 reg_val = delay & RGMII_DELAY_STEP_MASK;

	reg_val |= RGMII_TXDELAY_FORCE_MODE;
	air_buckpbus_reg_write(phydev, 0x1021C024, reg_val);
	reg_val = air_buckpbus_reg_read(phydev, 0x1021C024);
	dev_info(phydev_dev(phydev),
		"Force txdelay = %d(0x%x)\n", delay, reg_val);
	return 0;
}

static int an8801r_rgmii_delay_config(struct phy_device *phydev)
{
	struct an8801_priv *priv = phydev_cfg(phydev);

	if (priv->rxdelay_force)
		an8801r_rgmii_rxdelay(phydev, priv->rxdelay_step,
				      priv->rxdelay_align);
	if (priv->txdelay_force)
		an8801r_rgmii_txdelay(phydev, priv->txdelay_step);
	return 0;
}

static int an8801sb_surge_protect_cfg(struct phy_device *phydev)
{
	int ret = 0;
	struct device *dev = phydev_dev(phydev);
	struct an8801_priv *priv = phydev->priv;
	u16 r50ohm_tx_a = 0, r50ohm_tx_b = 0, r50ohm_tx_c = 0, r50ohm_tx_d = 0;
	u16 cl45_value = 0;

	if (priv->surge) {
		cl45_value = phy_read_mmd(phydev, 0x1e, 0x174);
		r50ohm_tx_a = (cl45_value >> 8) & 0x007f;
		r50ohm_tx_b = cl45_value & 0x007f;
		dev_dbg(phydev_dev(phydev), "Read: (0x174) value=0x%04x r50ohm_tx_a=%d r50ohm_tx_b=%d\n",
			cl45_value, r50ohm_tx_a, r50ohm_tx_b);
		ret = process_r50(phydev, 0x174, &cl45_value, &r50ohm_tx_a, &r50ohm_tx_b);
		if (ret < 0)
			return ret;
		cl45_value = phy_read_mmd(phydev, 0x1e, 0x175);
		r50ohm_tx_c = (cl45_value >> 8) & 0x007f;
		r50ohm_tx_d = cl45_value & 0x007f;
		dev_dbg(phydev_dev(phydev), "Read: (0x175) value=0x%04x r50ohm_tx_c=%d r50ohm_tx_d=%d\n",
			cl45_value, r50ohm_tx_c, r50ohm_tx_d);
		ret = process_r50(phydev, 0x175, &cl45_value, &r50ohm_tx_c, &r50ohm_tx_d);
		if (ret < 0)
			return ret;
		ret = an8801sb_i2mpb_config(phydev);
		if (ret < 0) {
			dev_err(dev, "an8801sb_i2mpb_config fail\n");
			return ret;
		}
		dev_info(dev, "surge protection mode - 5R\n");
	} else
		dev_info(dev, "surge protection mode - 0R\n");
	return ret;
}

static int an8801sb_config_init(struct phy_device *phydev)
{
	int ret;
	struct an8801_priv *priv = phydev_cfg(phydev);
	u32 pbus_value = 0;
	u32 reg_value = 0;

	ret = an8801sb_of_init(phydev);
	if (ret < 0)
		return ret;
	if (priv->sgmii_mode == AIR_SGMII_AN) {
		dev_info(phydev_dev(phydev), "sgmii mode - AN\n");
		reg_value = phy_read(phydev, MII_BMSR);
		if ((reg_value & MCS_LINK_STATUS_MASK) != 0) {
			ret = air_buckpbus_reg_write(phydev, 0x10220010, 0x1801);
			if (ret < 0)
				return ret;
			reg_value = air_buckpbus_reg_read(phydev, 0x10220010);
			dev_dbg(phydev_dev(phydev),
				"air_buckpbus_reg_read(0x10220010,0x%x).\n", reg_value);

			ret = air_buckpbus_reg_write(phydev, 0x10220000, 0x9140);
			if (ret < 0)
				return ret;
			reg_value = air_buckpbus_reg_read(phydev, 0x10220000);
			dev_dbg(phydev_dev(phydev),
				"air_buckpbus_reg_read(0x10220000,0x%x).\n", reg_value);
			mdelay(80);
		}
	} else {	/* SGMII force mode */
		dev_info(phydev_dev(phydev), "sgmii mode - Force\n");
		ret = air_buckpbus_reg_write(phydev, 0x102260E4, 0xFF11);
		ret |= air_buckpbus_reg_write(phydev, 0x10224004, 0x0700);
		ret |= air_buckpbus_reg_write(phydev, 0x10224018, 0x0);
		ret |= air_buckpbus_reg_write(phydev, 0x1022450C, 0x0700);
		ret |= air_buckpbus_reg_write(phydev, 0x1022A140, 0x5);
		ret |= air_buckpbus_reg_write(phydev, 0x10226100, 0xF0000000);
		ret |= air_buckpbus_reg_write(phydev, 0x10226300, 0x0);
		ret |= air_buckpbus_reg_write(phydev, 0x1022A078, 0x10050);
		ret |= air_buckpbus_reg_write(phydev, 0x10220034, 0x31120009);
		ret |= air_buckpbus_reg_write(phydev, 0x10220000, 0x140);
		if (ret < 0)
			return ret;
	}
#ifdef CONFIG_OF
	pbus_value = air_buckpbus_reg_read(phydev, 0x1022a0f8);
	pbus_value &= ~0x3;
	pbus_value |= priv->pol;
	ret = air_buckpbus_reg_write(phydev, 0x1022a0f8, pbus_value);
	if (ret < 0)
		return ret;
#endif
	pbus_value = air_buckpbus_reg_read(phydev, 0x1022a0f8);
	dev_info(phydev_dev(phydev),
		"Tx, Rx Polarity : %08x\n", pbus_value);

	ret = phy_write_mmd(phydev, MMD_DEV_VSPEC2, 0x600, 0x1e);
	ret |= phy_write_mmd(phydev, MMD_DEV_VSPEC2, 0x601, 0x02);

	ret |= phy_write_mmd(phydev, 7, 60, 0x0);
	if (ret != 0) {
		dev_err(phydev_dev(phydev),
			"AN8801SB initialize fail, ret %d !\n", ret);
		return ret;
	}

	ret = an8801_led_init(phydev);
	if (ret != 0) {
		dev_err(phydev_dev(phydev),
			"LED initialize fail, ret %d !\n", ret);
		return ret;
	}
	air_buckpbus_reg_write(phydev, 0x10270100, 0x0f);
	air_buckpbus_reg_write(phydev, 0x10270108, 0x0a0a0404);
	ret = an8801sb_surge_protect_cfg(phydev);
	if (ret < 0) {
		dev_err(phydev_dev(phydev),
			"an8801sb_surge_protect_cfg fail. (ret=%d)\n", ret);
		return ret;
	}
	reg_value = phy_read(phydev, MII_CTRL1000);
	reg_value |= ADVERTISE_1000FULL;
	ret = phy_write(phydev, MII_CTRL1000, reg_value);
	if (ret < 0)
		return ret;
	reg_value = phy_read(phydev, MII_BMCR);
	reg_value |= BMCR_ANRESTART;
	ret = phy_write(phydev, MII_BMCR, reg_value);
	if (ret < 0)
		return ret;
	dev_info(phydev_dev(phydev),
		"AN8801SB Initialize OK ! (%s)\n", AN8801_DRIVER_VERSION);
	return 0;
}

static int an8801r_config_init(struct phy_device *phydev)
{
	int ret;

	ret = an8801r_of_init(phydev);
	if (ret < 0)
		return ret;

	ret = air_sw_reset(phydev);
	if (ret < 0)
		return ret;

	air_buckpbus_reg_write(phydev, 0x11F808D0, 0x180);

	air_buckpbus_reg_write(phydev, 0x1021c004, 0x1);
	air_buckpbus_reg_write(phydev, 0x10270004, 0x3f);
	air_buckpbus_reg_write(phydev, 0x10270104, 0xff);
	air_buckpbus_reg_write(phydev, 0x10270204, 0xff);

	an8801r_rgmii_delay_config(phydev);

	ret = an8801_led_init(phydev);
	if (ret != 0) {
		dev_err(phydev_dev(phydev),
			"LED initialize fail, ret %d !\n", ret);
		return ret;
	}
	dev_info(phydev_dev(phydev), "AN8801R Initialize OK ! (%s)\n",
		AN8801_DRIVER_VERSION);
	return 0;
}

static int an8801_config_init(struct phy_device *phydev)
{
	if (phydev->interface == PHY_INTERFACE_MODE_SGMII) {
		an8801sb_config_init(phydev);
	} else if (phydev->interface == PHY_INTERFACE_MODE_RGMII) {
		an8801r_config_init(phydev);
	} else {
		dev_info(phydev_dev(phydev), "AN8801 Phy-mode not support!!!\n");
		return -1;
	}
	return 0;
}

#ifdef AN8801SB_DEBUGFS
static const char * const tx_rx_string[32] = {
		"Tx Normal, Rx Reverse",
		"Tx Reverse, Rx Reverse",
		"Tx Normal, Rx Normal",
		"Tx Reverse, Rx Normal",
};

static int an8801_set_polarity(struct phy_device *phydev, int tx_rx)
{
	int ret = 0;
	u32 pbus_data = 0;

	pr_notice("\n[Write] Polarity %s\n", tx_rx_string[tx_rx]);
	pbus_data = (air_buckpbus_reg_read(phydev, 0x1022a0f8) &
				(~(BIT(0) | BIT(1))));
	pbus_data |= (BIT(4) | tx_rx);
	ret = air_buckpbus_reg_write(phydev, 0x1022a0f8, pbus_data);
	if (ret < 0)
		return ret;
	usleep_range(9800, 12000);
	pbus_data &= ~BIT(4);
	ret = air_buckpbus_reg_write(phydev, 0x1022a0f8, pbus_data);
	if (ret < 0)
		return ret;
	pbus_data = air_buckpbus_reg_read(phydev, 0x1022a0f8);
	tx_rx = pbus_data & (BIT(0) | BIT(1));
	pr_notice("\n[Read] Polarity %s confirm....(%8x)\n",
		tx_rx_string[tx_rx], pbus_data);

	return ret;
}

static int air_polarity_help(void)
{
	pr_notice(AN8801_DEBUGFS_POLARITY_HELP_STRING);
	return 0;
}

static ssize_t an8801_polarity_write(struct file *file, const char __user *ptr,
					size_t len, loff_t *off)
{
	struct phy_device *phydev = file->private_data;
	char buf[32], param1[32], param2[32];
	int count = len, ret = 0, tx_rx = 0;

	memset(buf, 0, 32);
	memset(param1, 0, 32);
	memset(param2, 0, 32);

	if (count > sizeof(buf) - 1)
		return -EINVAL;
	if (copy_from_user(buf, ptr, len))
		return -EFAULT;

	ret = sscanf(buf, "%s %s", param1, param2);
	if (ret < 0)
		return ret;

	if (!strncmp("help", param1, strlen("help"))) {
		air_polarity_help();
		return count;
	}
	if (!strncmp("tx_normal", param1, strlen("tx_normal"))) {
		if (!strncmp("rx_normal", param2, strlen("rx_normal")))
			tx_rx = AIR_POL_TX_NOR_RX_NOR;
		else if (!strncmp("rx_reverse", param2, strlen("rx_reverse")))
			tx_rx = AIR_POL_TX_NOR_RX_REV;
		else {
			pr_notice(AN8801_DEBUGFS_RX_ERROR_STRING);
			return -EINVAL;
		}
	} else if (!strncmp("tx_reverse", param1, strlen("tx_reverse"))) {
		if (!strncmp("rx_normal", param2, strlen("rx_normal")))
			tx_rx = AIR_POL_TX_REV_RX_NOR;
		else if (!strncmp("rx_reverse", param2, strlen("rx_reverse")))
			tx_rx = AIR_POL_TX_REV_RX_REV;
		else {
			pr_notice(AN8801_DEBUGFS_RX_ERROR_STRING);
			return -EINVAL;
		}
	} else {
		pr_notice(AN8801_DEBUGFS_TX_ERROR_STRING);
		return -EINVAL;
	}
	ret = an8801_set_polarity(phydev, tx_rx);
	if (ret < 0)
		return ret;
	return count;
}

static ssize_t an8801_mdio_write(struct file *file, const char __user *ptr,
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
			pr_notice(AN8801_DEBUGFS_MDIO_HELP_STRING);
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
			pr_notice(AN8801_DEBUGFS_MDIO_HELP_STRING);
			return -EINVAL;
		}
	} else {
		pr_notice(AN8801_DEBUGFS_MDIO_HELP_STRING);
		return -EINVAL;
	}

	return count;
}

static int an8801_counter_show(struct seq_file *seq, void *v)
{
	struct phy_device *phydev = seq->private;
	int ret = 0;
	u32 pkt_cnt = 0;

	seq_puts(seq, "==========AIR PHY COUNTER==========\n");
	seq_puts(seq, "|\t<<SERDES COUNTER>>\n");
	air_buckpbus_reg_write(phydev, 0x10226124, 0xaa);
	air_buckpbus_reg_write(phydev, 0x10226124, 0x0);
	seq_puts(seq, "| PHY Rx DV CNT           :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x1022614c);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| PHY Tx EN CNT           :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10226148);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "|  Tx ER CNT              :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10226150);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| MAC Rx DV CNT           :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10226138);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| MAC Tx EN CNT           :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x1022612C);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx ER CNT               :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10226130);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	ret = air_buckpbus_reg_write(phydev, 0x10226124, 0x55);
	ret |= air_buckpbus_reg_write(phydev, 0x10226124, 0x0);
	if (ret < 0)
		return ret;

	seq_puts(seq, "|\t<<EFIFO COUNTER>>\n");
	seq_puts(seq, "| Rx from Line side_S     :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10270130);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx from Line side_E     :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10270134);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to System side_S     :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10270138);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to System side_E     :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x1027013C);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx from System side_S   :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10270120);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Rx from System side_E   :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10270124);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to Line side_S       :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x10270128);
	seq_printf(seq, "%010u |\n", pkt_cnt);
	seq_puts(seq, "| Tx to Line side_E       :");
	pkt_cnt = air_buckpbus_reg_read(phydev, 0x1027012C);
	seq_printf(seq, "%010u |\n", pkt_cnt);

	ret = air_buckpbus_reg_write(phydev, 0x1027011C, 0x3);
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

static int an8801_counter_open(struct inode *inode, struct file *file)
{
	return single_open(file, an8801_counter_show, inode->i_private);
}

static int an8801_debugfs_pbus_help(void)
{
	pr_notice(AN8801_DEBUGFS_PBUS_HELP_STRING);
	return 0;
}

static ssize_t an8801_debugfs_pbus(struct file *file,
		const char __user *buffer, size_t count,
		loff_t *data)
{
	struct phy_device *phydev = file->private_data;
	char buf[64];
	int ret = 0;
	unsigned int reg, addr;
	u32 val;

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

		pr_notice("\nphy=%d, reg=0x%x, val=0x%x confirm..\n",
			addr, reg,
			air_buckpbus_reg_read(phydev, reg));
	} else if (buf[0] == 'r') {
		if (sscanf(buf, "r %x", &reg) == -1)
			return -EFAULT;

		pr_notice("\nphy=0x%x, reg=0x%x, val=0x%x\n",
			phydev_phy_addr(phydev), reg,
			air_buckpbus_reg_read(phydev, reg));
	} else if (buf[0] == 'h')
		an8801_debugfs_pbus_help();

	return count;
}

static int an8801_info_show(struct seq_file *seq, void *v)
{
	struct phy_device *phydev = seq->private;
	unsigned int tx_rx =
		(air_buckpbus_reg_read(phydev, 0x1022a0f8) & 0x3);
	u32 pbus_data = 0;
	int reg = 0;

	seq_puts(seq, "\t<<AIR AN8801SB Info>>\n");
	pbus_data = air_buckpbus_reg_read(phydev, 0x10005004);
	seq_printf(seq, "| Product Version : E%d\n", pbus_data);
	seq_printf(seq, "| Driver Version  : %s\n", AN8801_DRIVER_VERSION);
	pbus_data = air_buckpbus_reg_read(phydev, 0x10220b04);
	seq_printf(seq, "| Serdes Status   : Rx_Sync(%01ld), AN_Done(%01ld)\n",
		GET_BIT(pbus_data, 4), GET_BIT(pbus_data, 0));
	seq_printf(seq, "| Tx, Rx Polarity : %s(%02d)\n",
		tx_rx_string[tx_rx], tx_rx);
	pbus_data = air_buckpbus_reg_read(phydev, 0x10000094);
	seq_printf(seq, "| RG_HW_STRAP     : 0x%08x\n", pbus_data);
	for (reg = MII_BMCR; reg <= MII_STAT1000; reg++) {
		if ((reg <= MII_LPA) || (reg >= MII_CTRL1000))
			seq_printf(seq, "| RG_MII 0x%02x     : 0x%08x\n",
				reg, phy_read(phydev, reg));
	}
	seq_puts(seq, "\n");
	return 0;
}

static int an8801_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, an8801_info_show, inode->i_private);
}

static const struct file_operations an8801_info_fops = {
	.owner = THIS_MODULE,
	.open = an8801_info_open,
	.read = seq_read,
	.llseek = noop_llseek,
	.release = single_release,
};

static const struct file_operations an8801_counter_fops = {
	.owner = THIS_MODULE,
	.open = an8801_counter_open,
	.read = seq_read,
	.llseek = noop_llseek,
	.release = single_release,
};

static const struct file_operations an8801_debugfs_pbus_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = an8801_debugfs_pbus,
	.llseek = noop_llseek,
};

static const struct file_operations an8801_polarity_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = an8801_polarity_write,
	.llseek = noop_llseek,
};

static const struct file_operations an8801_mdio_fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.write = an8801_mdio_write,
	.llseek = noop_llseek,
};

static int an8801_debugfs_init(struct phy_device *phydev)
{
	int ret = 0;
	struct an8801_priv *priv = phydev->priv;

	dev_info(phydev_dev(phydev), "Debugfs init start\n");
	priv->debugfs_root =
		debugfs_create_dir(dev_name(phydev_dev(phydev)), NULL);
	if (!priv->debugfs_root) {
		dev_err(phydev_dev(phydev), "Debugfs init err\n");
		ret = -ENOMEM;
	}
	debugfs_create_file(DEBUGFS_INFO, 0444,
					priv->debugfs_root, phydev,
					&an8801_info_fops);
	debugfs_create_file(DEBUGFS_COUNTER, 0644,
					priv->debugfs_root, phydev,
					&an8801_counter_fops);
	debugfs_create_file(DEBUGFS_PBUS_OP, S_IFREG | 0200,
					priv->debugfs_root, phydev,
					&an8801_debugfs_pbus_fops);
	debugfs_create_file(DEBUGFS_POLARITY, S_IFREG | 0200,
					priv->debugfs_root, phydev,
					&an8801_polarity_fops);
	debugfs_create_file(DEBUGFS_MDIO, S_IFREG | 0200,
					priv->debugfs_root, phydev,
					&an8801_mdio_fops);
	return ret;
}

static void air_debugfs_remove(struct phy_device *phydev)
{
	struct an8801_priv *priv = phydev->priv;

	if (priv->debugfs_root != NULL) {
		debugfs_remove_recursive(priv->debugfs_root);
		priv->debugfs_root = NULL;
	}
}
#endif /*AN8801SB_DEBUGFS*/

static int an8801_phy_probe(struct phy_device *phydev)
{
	u32 reg_value, phy_id, led_id;
	struct an8801_priv *priv = NULL;

	reg_value = phy_read(phydev, 2);
	phy_id = reg_value << 16;
	reg_value = phy_read(phydev, 3);
	phy_id |= reg_value;
	dev_info(phydev_dev(phydev), "PHY-ID = %x\n", phy_id);

	if (phy_id != AN8801_PHY_ID) {
		dev_err(phydev_dev(phydev),
			"AN8801 can't be detected.\n");
		return -1;
	}

	priv = devm_kzalloc(phydev_dev(phydev)
						, sizeof(struct an8801_priv)
						, GFP_KERNEL);
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
#ifdef AN8801SB_DEBUGFS
	reg_value = air_buckpbus_reg_read(phydev, 0x10000094);
	if (0 == (reg_value &
		(AN8801_RG_PKG_SEL_LSB | AN8801_RG_PKG_SEL_MSB))) {
		int ret = 0;

		ret = an8801_debugfs_init(phydev);
		if (ret < 0) {
			air_debugfs_remove(phydev);
			kfree(priv);
			return ret;
		}
	} else {
		dev_info(phydev_dev(phydev), "AN8801R not supprt debugfs\n");
	}
#endif
	return 0;
}

static void an8801_phy_remove(struct phy_device *phydev)
{
	struct an8801_priv *priv = (struct an8801_priv *)phydev->priv;

	if (priv) {
#ifdef AN8801SB_DEBUGFS
		air_debugfs_remove(phydev);
#endif
		kfree(priv);
		dev_info(phydev_dev(phydev), "AN8801 remove OK!\n");
	}
}

static int an8801sb_read_status(struct phy_device *phydev)
{
	int ret, prespeed = phydev->speed;
	struct an8801_priv *priv = phydev->priv;
	u32 reg_value = 0;
	u32 an_retry = MAX_SGMII_AN_RETRY;

	ret = genphy_read_status(phydev);
	if (phydev->link == LINK_DOWN) {
		prespeed = 0;
		phydev->speed = 0;
		ret |= phy_write_mmd(
			phydev, MMD_DEV_VSPEC2, PHY_PRE_SPEED_REG, prespeed);

		if (priv->sgmii_mode == AIR_SGMII_AN) {
			mdelay(10);        /* delay 10 ms */
			reg_value = air_buckpbus_reg_read(phydev, 0x10220010);
			reg_value &= 0x7fff;
			air_buckpbus_reg_write(phydev, 0x10220010, reg_value);

			reg_value = air_buckpbus_reg_read(phydev, 0x10220000);
			reg_value |= AN8801SB_SGMII_AN0_ANRESTART;
			air_buckpbus_reg_write(phydev, 0x10220000, reg_value);
		}
	}

	if (prespeed != phydev->speed && phydev->link == LINK_UP) {
		prespeed = phydev->speed;
		ret |= phy_write_mmd(
			phydev, MMD_DEV_VSPEC2, PHY_PRE_SPEED_REG, prespeed);
		dev_info(phydev_dev(phydev), "AN8801SB SPEED %d\n", prespeed);
		if (priv->sgmii_mode == AIR_SGMII_AN) {
			while (an_retry > 0) {
				mdelay(1);       /* delay 1 ms */
				reg_value = air_buckpbus_reg_read(
					phydev, 0x10220b04);
				if (reg_value & AN8801SB_SGMII_AN0_AN_DONE)
					break;
				an_retry--;
			}
			mdelay(10);        /* delay 10 ms */


			if (prespeed == SPEED_1000) {
				air_buckpbus_reg_write(
					phydev, 0x10220010, 0xd801);
			} else if (prespeed == SPEED_100) {
				air_buckpbus_reg_write(
					phydev, 0x10220010, 0xd401);
			} else {
				air_buckpbus_reg_write(
					phydev, 0x10220010, 0xd001);
			}

			reg_value = air_buckpbus_reg_read(phydev, 0x10220000);
			reg_value |= (AN8801SB_SGMII_AN0_RESET | AN8801SB_SGMII_AN0_ANRESTART);
			air_buckpbus_reg_write(phydev, 0x10220000, reg_value);
		} else {	/* SGMII force mode */
			if (prespeed == SPEED_1000) {
				ret = air_buckpbus_reg_write(phydev, 0x102260E4, 0xFF11);
				ret |= air_buckpbus_reg_write(phydev, 0x10224004, 0x0700);
				ret |= air_buckpbus_reg_write(phydev, 0x10224018, 0x0);
				ret |= air_buckpbus_reg_write(phydev, 0x1022450C, 0x0700);
				ret |= air_buckpbus_reg_write(phydev, 0x1022A140, 0x5);
				ret |= air_buckpbus_reg_write(phydev, 0x10226100, 0xF0000000);
				ret |= air_buckpbus_reg_write(phydev, 0x10270100, 0xF);
			} else if (prespeed == SPEED_100) {
				ret = air_buckpbus_reg_write(phydev, 0x102260E4, 0xFF11);
				ret |= air_buckpbus_reg_write(phydev, 0x10224004, 0x0755);
				ret |= air_buckpbus_reg_write(phydev, 0x10224018, 0x14);
				ret |= air_buckpbus_reg_write(phydev, 0x1022450C, 0x0755);
				ret |= air_buckpbus_reg_write(phydev, 0x1022A140, 0x10);
				ret |= air_buckpbus_reg_write(phydev, 0x10226100, 0xF000000C);
				ret |= air_buckpbus_reg_write(phydev, 0x10270100, 0xC);
			} else {
				ret = air_buckpbus_reg_write(phydev, 0x102260E4, 0xFFAA);
				ret |= air_buckpbus_reg_write(phydev, 0x10224004, 0x07AA);
				ret |= air_buckpbus_reg_write(phydev, 0x10224018, 0x4);
				ret |= air_buckpbus_reg_write(phydev, 0x1022450C, 0x07AA);
				ret |= air_buckpbus_reg_write(phydev, 0x1022A140, 0x20);
				ret |= air_buckpbus_reg_write(phydev, 0x10226100, 0xF000000F);
				ret |= air_buckpbus_reg_write(phydev, 0x10270100, 0xC);
			}
		}
	}
	return ret;
}

static int an8801r_read_status(struct phy_device *phydev)
{
	int ret, prespeed = phydev->speed;
	u32 data;

	ret = genphy_read_status(phydev);
	if (phydev->link == LINK_DOWN) {
		prespeed = 0;
		phydev->speed = 0;
	}
	if (prespeed != phydev->speed && phydev->link == LINK_UP) {
		prespeed = phydev->speed;
		dev_dbg(phydev_dev(phydev), "AN8801R SPEED %d\n", prespeed);
		if (prespeed == SPEED_1000) {
			data = air_buckpbus_reg_read(phydev, 0x10005054);
			data |= BIT(0);
			air_buckpbus_reg_write(phydev, 0x10005054, data);
		} else {
			data = air_buckpbus_reg_read(phydev, 0x10005054);
			data &= ~BIT(0);
			air_buckpbus_reg_write(phydev, 0x10005054, data);
		}
	}
	return ret;
}

static int an8801_read_status(struct phy_device *phydev)
{
	if (phydev->interface == PHY_INTERFACE_MODE_SGMII) {
		an8801sb_read_status(phydev);
	} else if (phydev->interface == PHY_INTERFACE_MODE_RGMII) {
		an8801r_read_status(phydev);
	} else {
		dev_info(phydev_dev(phydev), "AN8801 Phy-mode not support!\n");
		return -1;
	}
	return 0;
}

static struct phy_driver airoha_driver[] = {
	{
		.phy_id         = AN8801_PHY_ID,
		.name           = "Airoha AN8801",
		.phy_id_mask    = 0x0ffffff0,
		.features       = PHY_GBIT_FEATURES,
		.config_init    = an8801_config_init,
		.config_aneg    = genphy_config_aneg,
		.probe          = an8801_phy_probe,
		.remove         = an8801_phy_remove,
		.read_status    = an8801_read_status,
		.config_intr	= an8801_config_intr,
#if (KERNEL_VERSION(5, 11, 0) < LINUX_VERSION_CODE)
		.handle_interrupt = an8801_handle_interrupt,
#else
		.did_interrupt	= an8801_did_interrupt,
		.ack_interrupt	= an8801_ack_interrupt,
#endif
	}
};

module_phy_driver(airoha_driver);

static struct mdio_device_id __maybe_unused airoha_tbl[] = {
	{ AN8801_PHY_ID, 0x0ffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, airoha_tbl);
