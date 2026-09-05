// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3383 UniMAC Ethernet Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/workqueue.h>
#include <linux/if_ether.h>
#include "unimac.h"

#include <linux/types.h>
#include <soc/bcm/bcm3380-fpm.h>
#include <soc/bcm/bcm3380-gphy.h>
#include <soc/bcm/bcm3383-mbdma.h>
#include <soc/bcm/bcm3380-msp.h>

#define UNIMAC_DUMP_TRAFFIC 0

#define UNIMAC_RESET_COUNT_MAX 8

#define UNIMAC_ENET_HIGH_PRIORITY_START		3
#define UNIMAC_MAX_FRAME_SIZE			2048
#define UNIMAC_TX_WAKE_DELAY_MS			1

#define UNIMAC_INTERFACE_OFFSET			0x0000

// UnimacInterfaceControl.Reg32
#define UNIMAC_INTERFACE_CONTROL		0x0000
#define UNIMAC_INTERFACE_CONTROL_BACKPRESSURE_MUX	0x00000200
#define UNIMAC_INTERFACE_CONTROL_FPM_BACKPRESSURE	0x00000100
#define UNIMAC_INTERFACE_CONTROL_TOKEN_BACKPRESSURE	0x00000080

// UnimacInterfaceBackPressure.Reg32
#define UNIMAC_INTERFACE_BACK_PRESSURE		0x0028
#define UNIMAC_INTERFACE_BACK_PRESSURE_ENABLE	BIT(0)

// UnimacInterfaceMdioCmd.Reg32
#define UNIMAC_INTERFACE_MDIO_CMD		0x002c
#define UNIMAC_INTERFACE_MDIO_CMD_SELECT	0x20000000
#define UNIMAC_INTERFACE_MDIO_CMD_WRITE		0x24000000
#define UNIMAC_INTERFACE_MDIO_CMD_READ		0x28000000
#define UNIMAC_INTERFACE_MDIO_CMD_PHY_SHIFT	21
#define UNIMAC_INTERFACE_MDIO_CMD_REG_SHIFT	16
#define UNIMAC_INTERFACE_MDIO_CMD_DATA_MASK	0x0000ffff

// UnimacInterfaceMdioCfg.Reg32
#define UNIMAC_INTERFACE_MDIO_CFG		0x0030
#define UNIMAC_INTERFACE_MDIO_CFG_EXTENDED	BIT(7)
#define UNIMAC_INTERFACE_MDIO_CFG_BUSY		BIT(8)

// UnimacInterfaceRgmiiCtrl.Reg32
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL	0x0034
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_ID_MODE_DISABLE	BIT(16)
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_TX_CLK_DLY_SHIFT	12
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_TX_CLK_DLY_8 \
	(8 << UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_TX_CLK_DLY_SHIFT)
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_RX_CLK_DLY_SHIFT	8
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_RX_CLK_DLY_8 \
	(8 << UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_RX_CLK_DLY_SHIFT)
/*
 * GPL UnimacInterfaceRgmiiCtrl fields:
 * RgmiiIdModeDisable=1, TxClkDly=8, RxClkDly=8, InbandEn=0, Link=0.
 */
#define UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_DELAY_CFG \
	(UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_ID_MODE_DISABLE | \
	 UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_TX_CLK_DLY_8 | \
	 UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_RX_CLK_DLY_8)

#define UNIMAC_CORE_OFFSET			0x0200

/*
 * GPL bcm3383/periph_blockdef.h: Periph.Pad1[0], the first undocumented
 * word after PllCntrlRegs PllCntrl and before IntControlExtRegs IntExt.
 * The BCM3383 bootloader writes PERIPH_BASE + 0x0398 for the internal PHY
 * path.  The DTS syscon maps this single word, so the regmap offset is 0.
 */
#define UNIMAC_BCM3383_PERIPH_INTERNAL_PHY_CTRL	0x0000
#define UNIMAC_BCM3383_PERIPH_INTERNAL_PHY_CTRL_ENABLE_MASK	GENMASK(7, 6)

// BCM3383-specific start
#define UNIMAC_BCM3383_INTERNAL_PHY_ADDR	0
#define UNIMAC_BCM3383_INTERNAL_PHY_EXP_REG	7
#define UNIMAC_BCM3383_INTERNAL_PHY_REG18	24
#define UNIMAC_BCM3383_INTERNAL_PHY_REG18_BOOTLOADER	0x0c00
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C0	0x91c0
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C0_VALUE	0xd771
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C2	0x91c2
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C2_VALUE	0x1872
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CF	0x91cf
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CF_VALUE	0x0006
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CC	0x91cc
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CC_VALUE	0x0500
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CB	0x91cb
#define UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CB_VALUE	0x0014
// BCM3383-specific end

struct unimac;

struct unimac_variant {
	void (*configure_backpressure)(struct unimac *unimac);
	void (*configure_interface)(struct unimac *unimac);
	bool assert_resets_on_stop;
};

/* Private driver data structure */
struct unimac {
	struct net_device *ndev;
	void __iomem *base;
	u32 mac_id;
	const struct unimac_variant *variant;

	struct bcm3380_msp *msp;
	struct unimac_mbdma *mbdma;

	// DQM queue IDs for RX and TX
	unsigned int rx_normal_queue;
	unsigned int rx_high_queue;
	unsigned int tx_high_queue;
	unsigned int tx_normal_queue;

	struct delayed_work tx_wake_work;

	struct clk_bulk_data *clocks;
	int num_clocks;

	struct reset_control *resets[UNIMAC_RESET_COUNT_MAX];

	phy_interface_t phy_interface;
	struct napi_struct napi;
};

static inline void __iomem *unimac_interface(struct unimac *unimac, u32 reg)
{
	return unimac->base + UNIMAC_INTERFACE_OFFSET + reg;
}

static inline void __iomem *unimac_core(struct unimac *unimac, u32 reg)
{
	return unimac->base + UNIMAC_CORE_OFFSET + reg;
}

static inline u32 unimac_rx_queue_mask(struct unimac *unimac)
{
	return BIT(unimac->rx_normal_queue) | BIT(unimac->rx_high_queue);
}

static void unimac_disable_clocks(void *data)
{
	struct unimac *unimac = data;

	clk_bulk_disable_unprepare(unimac->num_clocks, unimac->clocks);
}

static int unimac_reset(struct unimac *unimac, struct device *dev)
{
	for (int i = 0; i < UNIMAC_RESET_COUNT_MAX; i++) {
		if (!unimac->resets[i])
			continue;

		int err = reset_control_assert(unimac->resets[i]);
		if (err) {
			dev_err(dev, "error asserting UniMAC reset %d\n", i);
			return err;
		}
	}

	mdelay(1);

	for (int i = 0; i < UNIMAC_RESET_COUNT_MAX; i++) {
		if (!unimac->resets[i])
			continue;

		int err = reset_control_deassert(unimac->resets[i]);
		if (err) {
			dev_err(dev, "error deasserting UniMAC reset %d\n", i);
			return err;
		}
	}

	usleep_range(10000, 20000);

	return 0;
}

// BCM3383-specific start
static int bcm3383_unimac_mdio_wait(struct unimac *unimac)
{
	u32 val;

	return readx_poll_timeout(readl_be, unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CFG),
				  val, !(val & UNIMAC_INTERFACE_MDIO_CFG_BUSY), 1, 10000);
}

static int bcm3383_unimac_mdio_write(struct unimac *unimac, u8 phy, u8 reg,
			     u16 data)
{
	writel_be(UNIMAC_INTERFACE_MDIO_CMD_WRITE |
		  (phy << UNIMAC_INTERFACE_MDIO_CMD_PHY_SHIFT) |
		  (reg << UNIMAC_INTERFACE_MDIO_CMD_REG_SHIFT) | data,
		  unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CMD));

	return bcm3383_unimac_mdio_wait(unimac);
}

static int bcm3383_unimac_mdio_shadow_write(struct unimac *unimac, u8 phy,
				    u8 reg, u16 shadow, u16 data)
{
	u32 cfg = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CFG));

	writel_be(cfg & ~UNIMAC_INTERFACE_MDIO_CFG_EXTENDED,
		  unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CFG));
	writel_be(UNIMAC_INTERFACE_MDIO_CMD_SELECT |
		  (phy << UNIMAC_INTERFACE_MDIO_CMD_PHY_SHIFT) |
		  (reg << UNIMAC_INTERFACE_MDIO_CMD_REG_SHIFT) | shadow,
		  unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CMD));

	int err = bcm3383_unimac_mdio_wait(unimac);
	if (err)
		return err;

	writel_be(UNIMAC_INTERFACE_MDIO_CMD_WRITE |
		  (phy << UNIMAC_INTERFACE_MDIO_CMD_PHY_SHIFT) |
		  (reg << UNIMAC_INTERFACE_MDIO_CMD_REG_SHIFT) | data,
		  unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CMD));

	err = bcm3383_unimac_mdio_wait(unimac);
	if (err)
		return err;

	cfg = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CFG));
	writel_be(cfg | UNIMAC_INTERFACE_MDIO_CFG_EXTENDED,
		  unimac_interface(unimac, UNIMAC_INTERFACE_MDIO_CFG));

	return 0;
}

static int unimac_init_internal_phy(struct unimac *unimac,
				    struct regmap *internal_phy_syscon)
{
	struct device *dev = unimac->ndev->dev.parent;

	dev_info(dev, "initializing BCM3383 internal PHY path\n");

	int err = bcm3383_unimac_mdio_write(unimac, UNIMAC_BCM3383_INTERNAL_PHY_ADDR,
				    UNIMAC_BCM3383_INTERNAL_PHY_REG18,
				    UNIMAC_BCM3383_INTERNAL_PHY_REG18_BOOTLOADER);
	if (err)
		return dev_err_probe(dev, err, "failed to write internal PHY reg18\n");

	err = bcm3383_unimac_mdio_shadow_write(unimac, UNIMAC_BCM3383_INTERNAL_PHY_ADDR,
				       UNIMAC_BCM3383_INTERNAL_PHY_EXP_REG,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C0,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C0_VALUE);
	if (err)
		return dev_err_probe(dev, err, "failed to write internal PHY shadow 0x91c0\n");

	err = bcm3383_unimac_mdio_shadow_write(unimac, UNIMAC_BCM3383_INTERNAL_PHY_ADDR,
				       UNIMAC_BCM3383_INTERNAL_PHY_EXP_REG,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C2,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91C2_VALUE);
	if (err)
		return dev_err_probe(dev, err, "failed to write internal PHY shadow 0x91c2\n");

	err = bcm3383_unimac_mdio_shadow_write(unimac, UNIMAC_BCM3383_INTERNAL_PHY_ADDR,
				       UNIMAC_BCM3383_INTERNAL_PHY_EXP_REG,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CF,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CF_VALUE);
	if (err)
		return dev_err_probe(dev, err, "failed to write internal PHY shadow 0x91cf\n");

	err = bcm3383_unimac_mdio_shadow_write(unimac, UNIMAC_BCM3383_INTERNAL_PHY_ADDR,
				       UNIMAC_BCM3383_INTERNAL_PHY_EXP_REG,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CC,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CC_VALUE);
	if (err)
		return dev_err_probe(dev, err, "failed to write internal PHY shadow 0x91cc\n");

	err = bcm3383_unimac_mdio_shadow_write(unimac, UNIMAC_BCM3383_INTERNAL_PHY_ADDR,
				       UNIMAC_BCM3383_INTERNAL_PHY_EXP_REG,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CB,
				       UNIMAC_BCM3383_INTERNAL_PHY_SHADOW_91CB_VALUE);
	if (err)
		return dev_err_probe(dev, err, "failed to write internal PHY shadow 0x91cb\n");

	err = regmap_update_bits(internal_phy_syscon,
				 UNIMAC_BCM3383_PERIPH_INTERNAL_PHY_CTRL,
				 UNIMAC_BCM3383_PERIPH_INTERNAL_PHY_CTRL_ENABLE_MASK,
				 UNIMAC_BCM3383_PERIPH_INTERNAL_PHY_CTRL_ENABLE_MASK);
	if (err)
		return dev_err_probe(dev, err, "failed to enable internal PHY periph control\n");

	return 0;
}

static void bcm3383_configure_backpressure(struct unimac *unimac)
{
	u32 val = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_CONTROL));

	val &= ~(UNIMAC_INTERFACE_CONTROL_BACKPRESSURE_MUX |
		 UNIMAC_INTERFACE_CONTROL_FPM_BACKPRESSURE |
		 UNIMAC_INTERFACE_CONTROL_TOKEN_BACKPRESSURE);
	writel_be(val, unimac_interface(unimac, UNIMAC_INTERFACE_CONTROL));

	val = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_BACK_PRESSURE));
	writel_be(val & ~UNIMAC_INTERFACE_BACK_PRESSURE_ENABLE,
		  unimac_interface(unimac, UNIMAC_INTERFACE_BACK_PRESSURE));
}

static void bcm3383_configure_interface(struct unimac *unimac)
{
	struct device *dev = unimac->ndev->dev.parent;

	if (of_property_present(dev->of_node, "brcm,internal-phy-syscon") ||
	    phy_interface_mode_is_rgmii(unimac->phy_interface))
		writel_be(UNIMAC_INTERFACE_BCM3383_RGMII_CTRL_DELAY_CFG,
			  unimac_interface(unimac, UNIMAC_INTERFACE_BCM3383_RGMII_CTRL));
}
// BCM3383-specific end

// BCM3380-specific start
static void bcm3380_configure_backpressure(struct unimac *unimac)
{
	u32 val = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_CONTROL));

	val |= UNIMAC_INTERFACE_CONTROL_BACKPRESSURE_MUX;
	val |= UNIMAC_INTERFACE_CONTROL_FPM_BACKPRESSURE;
	val |= UNIMAC_INTERFACE_CONTROL_TOKEN_BACKPRESSURE;
	writel_be(val, unimac_interface(unimac, UNIMAC_INTERFACE_CONTROL));

	val = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_BACK_PRESSURE));
	writel_be(val | UNIMAC_INTERFACE_BACK_PRESSURE_ENABLE,
		  unimac_interface(unimac, UNIMAC_INTERFACE_BACK_PRESSURE));
}
// BCM3380-specific end

static u32 vEthernetTx(struct unimac *unimac, size_t uiLengthIn,
			       const void *buffer, u32 priority)
{
	size_t clamped_length = (uiLengthIn < 64) ? 64 : uiLengthIn;
	unsigned int tx_queue = priority > UNIMAC_ENET_HIGH_PRIORITY_START ?
				unimac->tx_high_queue :
				unimac->tx_normal_queue;

	if (!msp_dqm_queue_has_space(unimac->msp, tx_queue)) {
		dev_warn_ratelimited(unimac->ndev->dev.parent,
				     "DQM TX q%u has no space. q_sts=0x%08X\n",
				     tx_queue,
				     msp_dqm_queue_status(unimac->msp, tx_queue));
		return 0;
	}

	struct bcm3380_fpm_pool *fpm_pool =
		unimac->mbdma->get_fpm_pool(unimac->mbdma);
	if (!fpm_pool)
		return 0;

	u32 token = fpm_borrow_token(fpm_pool);
	if (!fpm_token_valid(token)) {
		dev_warn_ratelimited(unimac->ndev->dev.parent,
				     "FPM pool has no token available for TX\n");
		return 0;
	}

	void *dma_dest = fpm_token_to_virt(fpm_pool, token);
	if (!dma_dest) {
		dev_err(unimac->ndev->dev.parent,
			"TX token did not map to a buffer: 0x%08X\n", token);
		fpm_return_token(fpm_pool, token);
		return 0;
	}

	// Copy the Ethernet packet data to the DMA buffer
	memcpy(dma_dest, buffer, uiLengthIn);
	if (clamped_length > uiLengthIn)
		memset((u8 *)dma_dest + uiLengthIn, 0,
		       clamped_length - uiLengthIn);

	// Update the token with the clamped length's lower 12 bits
	u32 adjusted_token = (token & ~BCM3380_FPM_TOKEN_SIZE_MASK) |
			     (clamped_length & BCM3380_FPM_TOKEN_SIZE_MASK);

	wmb();
	msp_dqm_write_word(unimac->msp, tx_queue, 0, adjusted_token);

#if UNIMAC_DUMP_TRAFFIC
	dev_info(unimac->ndev->dev.parent,
		 "DQM q%u <- TX token=0x%08X len=%zu priority=%u q_sts=0x%08X\n",
		 tx_queue, adjusted_token, clamped_length, priority,
		 msp_dqm_queue_status(unimac->msp, tx_queue));
#endif
	return 1;
}

static void unimac_schedule_tx_wake(struct unimac *unimac)
{
	mod_delayed_work(system_wq, &unimac->tx_wake_work,
			 msecs_to_jiffies(UNIMAC_TX_WAKE_DELAY_MS));
}

static void unimac_tx_wake_work(struct work_struct *work)
{
	struct unimac *unimac =
		container_of(to_delayed_work(work), struct unimac,
			     tx_wake_work);
	u32 token;

	if (!netif_running(unimac->ndev))
		return;

	if (!msp_dqm_queue_has_space(unimac->msp, unimac->tx_normal_queue) &&
	    !msp_dqm_queue_has_space(unimac->msp, unimac->tx_high_queue)) {
		unimac_schedule_tx_wake(unimac);
		return;
	}

	struct bcm3380_fpm_pool *fpm_pool =
		unimac->mbdma->get_fpm_pool(unimac->mbdma);
	if (!fpm_pool) {
		unimac_schedule_tx_wake(unimac);
		return;
	}

	token = fpm_borrow_token(fpm_pool);
	if (!fpm_token_valid(token)) {
		unimac_schedule_tx_wake(unimac);
		return;
	}

	fpm_return_token(fpm_pool, token);
	netif_wake_queue(unimac->ndev);
}

static void unimac_msp_dqm_host_not_empty_irq(void *data)
{
	struct unimac *unimac = data;

	if (napi_schedule_prep(&unimac->napi))
		__napi_schedule(&unimac->napi);
}

static void unimac_set_rx_mode(struct net_device *ndev)
{
	struct unimac *unimac = netdev_priv(ndev);
	u32 cmd = readl_be(unimac_core(unimac, UMAC_CMD));

	if (ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))
		cmd |= CMD_PROMISC;
	else
		cmd &= ~CMD_PROMISC;

	writel_be(cmd, unimac_core(unimac, UMAC_CMD));
}

static int unimac_set_mac_address(struct net_device *ndev, void *p) {
	struct unimac *unimac = netdev_priv(ndev);
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	eth_hw_addr_set(ndev, addr->sa_data);

	u32 mac_hi = addr->sa_data[0];
	mac_hi <<= 8;
	mac_hi |= addr->sa_data[1];
	mac_hi <<= 8;
	mac_hi |= addr->sa_data[2];
	mac_hi <<= 8;
	mac_hi |= addr->sa_data[3];

	u16 mac_lo = addr->sa_data[4];
	mac_lo <<= 8;
	mac_lo |= addr->sa_data[5];

	writel_be(mac_hi, unimac_core(unimac, UMAC_MAC0));
	writel_be(mac_lo, unimac_core(unimac, UMAC_MAC1));

	return 0;
}

static void unimac_adjust_link(struct net_device *ndev)
{
	struct unimac *unimac = netdev_priv(ndev);
	struct phy_device *phydev = ndev->phydev;

	if (!phydev)
		return;

	if (!phydev->link) {
		netif_carrier_off(ndev);
		netif_stop_queue(ndev);
		phy_print_status(phydev);
		return;
	}

	u32 speed_cmd;
	switch (phydev->speed) {
	case SPEED_10:
		speed_cmd = CMD_SPEED_10;
		break;
	case SPEED_100:
		speed_cmd = CMD_SPEED_100;
		break;
	case SPEED_1000:
		speed_cmd = CMD_SPEED_1000;
		break;
	default:
		dev_warn(unimac->ndev->dev.parent,
			 "unsupported PHY speed %d\n", phydev->speed);
		return;
	}

	u32 cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	if (phydev->duplex == DUPLEX_HALF)
		cmd |= CMD_HD_EN;
	else
		cmd &= ~CMD_HD_EN;
	cmd &= ~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
	cmd |= speed_cmd << CMD_SPEED_SHIFT;
	writel_be(cmd, unimac_core(unimac, UMAC_CMD));

	netif_carrier_on(ndev);
	netif_wake_queue(ndev);
	phy_print_status(phydev);
}

static int unimac_phy_connect(struct net_device *ndev)
{
	struct device *dev = ndev->dev.parent;
	struct phy_device *phydev =
		 of_phy_get_and_connect(ndev, dev->of_node, unimac_adjust_link);

	if (!phydev) {
		if (of_property_present(dev->of_node, "phy-handle") ||
		    of_phy_is_fixed_link(dev->of_node))
			return dev_err_probe(dev, -ENODEV, "failed to connect PHY\n");
		return 0;
	}

	phy_attached_info(phydev);

	return 0;
}

/*
 * unimac_mbdma_get() - get the MBDMA provider referenced by a UniMAC consumer
 * @consumer: MBDMA consumer device, such as a UniMAC platform device
 *
 * The consumer DT node must contain:
 *
 *     brcm,mbdma = <&unimac_mbdma>;
 *
 * The referenced MBDMA platform driver must publish its initialized common
 * object with:
 *
 *     platform_set_drvdata(pdev, &priv->mbdma);
 *
 * Return:
 * * struct unimac_mbdma pointer on success
 * * -ENODEV if the property is missing, disabled, or invalid
 * * -EPROBE_DEFER if the provider platform device/driver is not ready
 * * -EINVAL if the provider published an invalid common object
 */
static struct unimac_mbdma *unimac_mbdma_get(struct device *consumer)
{
	if (!consumer)
		return ERR_PTR(-EINVAL);
	if (!dev_of_node(consumer))
		return ERR_PTR(-ENODEV);

	struct device_node *provider_np =
		of_parse_phandle(dev_of_node(consumer), "brcm,mbdma", 0);
	if (!provider_np)
		return ERR_PTR(-ENODEV);

	if (!of_device_is_available(provider_np)) {
		of_node_put(provider_np);
		return ERR_PTR(-ENODEV);
	}

	struct platform_device *provider_pdev =
		of_find_device_by_node(provider_np);
	of_node_put(provider_np);
	if (!provider_pdev)
		return ERR_PTR(-EPROBE_DEFER);

	struct device *supplier = &provider_pdev->dev;
	int ret;

	struct device_link *link =
		device_link_add(consumer, supplier, DL_FLAG_AUTOREMOVE_CONSUMER);
	if (!link) {
		ret = -EINVAL;
		goto out_put_provider;
	}

	struct unimac_mbdma *mbdma = platform_get_drvdata(provider_pdev);
	if (!mbdma) {
		ret = -EPROBE_DEFER;
		goto out_put_provider;
	}

	if (!mbdma->is_dev || !mbdma->get_fpm_pool || !mbdma->prepare) {
		dev_err(consumer, "MBDMA provider %s has invalid callbacks\n",
			dev_name(supplier));
		ret = -EINVAL;
		goto out_put_provider;
	}

	if (!mbdma->is_dev(mbdma, supplier)) {
		dev_err(consumer,
			"MBDMA provider %s returned an invalid device\n",
			dev_name(supplier));
		ret = -EINVAL;
		goto out_put_provider;
	}

	put_device(supplier);

	return mbdma;

out_put_provider:
	put_device(supplier);
	return ERR_PTR(ret);
}

static int unimac_open(struct net_device *ndev) {
	struct unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;
	struct sockaddr addr;
	int err;

	err = unimac_reset(unimac, dev);
	if (err)
		return err;

	/*
	 * MSP Incoming/Outgoing FIFO message word-size table:
	 *   LANRxMsg    -> MsgWdSzId = 1
	 *   LANTxMsg    -> MsgWdSzId = 1
	 *   TXStatusMsg -> MsgWdSzId = 2
	 *   DSPktMsg    -> MsgWdSzId = 1
	 */
	msp_set_msgid_word_size(unimac->msp, 0, 1);
	msp_set_msgid_word_size(unimac->msp, 1, 1);
	msp_set_msgid_word_size(unimac->msp, 2, 2);
	msp_set_msgid_word_size(unimac->msp, 3, 1);

	u32 uiInMsgDataPhysicalAddr = msp_in_msg_data_bus_addr(unimac->msp);
	dev_info(dev, "MSP inmsg_data_bus=0x%08X\n", uiInMsgDataPhysicalAddr);

	/* Initialize Unimac Start*/
	u32 tx_fifo_bus = unimac->mbdma->prepare(unimac->mbdma,
						 uiInMsgDataPhysicalAddr,
						 unimac->mac_id);
	if (!tx_fifo_bus)
		return dev_err_probe(dev, -EINVAL,
				     "MBDMA did not return a TX FIFO address\n");

	err = msp_4ke_register_enet_port(unimac->msp, unimac->mac_id,
					 unimac->rx_normal_queue,
					 unimac->rx_high_queue,
					 unimac->tx_high_queue,
					 unimac->tx_normal_queue,
					 tx_fifo_bus);
	if (err)
		return dev_err_probe(dev, err,
				     "failed to register MSP 4KE ENET port\n");

	u32 cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd | CMD_SW_RESET, unimac_core(unimac, UMAC_CMD));
	cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd & ~CMD_SW_RESET, unimac_core(unimac, UMAC_CMD));
	writel_be(UNIMAC_MAX_FRAME_SIZE, unimac_core(unimac, UMAC_MAX_FRAME_LEN));
	memcpy(addr.sa_data, ndev->dev_addr, ETH_ALEN);
	unimac_set_mac_address(ndev, &addr);

	if (unimac->variant->configure_interface)
		unimac->variant->configure_interface(unimac);

	cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be((cmd & ~CMD_PROMISC) | CMD_NO_LEN_CHK, unimac_core(unimac, UMAC_CMD));
	unimac->variant->configure_backpressure(unimac);
	unimac_set_rx_mode(ndev);
	/* Initialize Unimac End*/

	err = unimac_phy_connect(ndev);
	if (err) {
		msp_4ke_unregister_enet_port(unimac->msp, unimac->mac_id);
		return err;
	}

	cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd | CMD_TX_EN | CMD_RX_EN, unimac_core(unimac, UMAC_CMD));
	dev_info(dev, "enabled Rx and Tx\n");

	napi_enable(&unimac->napi);

	err = msp_dqm_host_not_empty_irq_register(unimac->msp, unimac_rx_queue_mask(unimac),
						 unimac_msp_dqm_host_not_empty_irq, unimac);
	if (err) {
		dev_err(dev, "failed to register MSP DQM not-empty RX IRQ callback: %d\n",
			err);
		napi_disable(&unimac->napi);
		if (ndev->phydev)
			phy_disconnect(ndev->phydev);
		msp_4ke_unregister_enet_port(unimac->msp, unimac->mac_id);
		return err;
	}
	msp_dqm_host_not_empty_irq_rearm(unimac->msp, unimac_rx_queue_mask(unimac));

	netif_start_queue(ndev);
	if (ndev->phydev)
		phy_start(ndev->phydev);
	else
		netif_carrier_on(ndev);

	return 0;
}

static int unimac_stop(struct net_device *ndev) {
	struct unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;

	dev_info(dev, "stopping UniMAC\n");

	netif_stop_queue(ndev);
	cancel_delayed_work_sync(&unimac->tx_wake_work);
	msp_dqm_host_not_empty_irq_unregister(unimac->msp,
					      unimac_msp_dqm_host_not_empty_irq,
					      unimac);
	msp_4ke_unregister_enet_port(unimac->msp, unimac->mac_id);
	if (ndev->phydev) {
		phy_stop(ndev->phydev);
		phy_disconnect(ndev->phydev);
	}
	netif_carrier_off(ndev);
	napi_disable(&unimac->napi);

	u32 cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd & ~(CMD_TX_EN | CMD_RX_EN), unimac_core(unimac, UMAC_CMD));

	if (unimac->variant->assert_resets_on_stop) {
		for (int i = 0; i < UNIMAC_RESET_COUNT_MAX; i++) {
			if (!unimac->resets[i])
				continue;
			if (reset_control_assert(unimac->resets[i]))
				dev_err(&ndev->dev, "error asserting UniMAC reset %d\n", i);
		}
	}

	netdev_reset_queue(ndev);

	return 0;
}

static netdev_tx_t unimac_start_xmit(struct sk_buff *skb, struct net_device *ndev) {
	struct unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;
	size_t length = skb->len;
	size_t max_frame_len = min_t(size_t, ndev->mtu + ndev->hard_header_len,
				     UNIMAC_MAX_FRAME_SIZE);
	int ret;

	if (length > max_frame_len) {
		dev_err(dev, "dropping oversize packet: len=%zu max=%zu mtu=%u hdr=%u\n",
			length, max_frame_len, ndev->mtu, ndev->hard_header_len);
		dev_kfree_skb(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

#if UNIMAC_DUMP_TRAFFIC
	dev_info(dev, "TX skb len=%zu priority=%u\n", length, skb->priority);
	print_hex_dump(KERN_INFO, "unimac tx: ", DUMP_PREFIX_NONE, 16, 1,
		       skb->data, min_t(size_t, length, 64), false);
#endif

	// Transmit the packet using vEthernetTx
	ret = vEthernetTx(unimac, length, skb->data, skb->priority);

	if (ret == 1) {
		// Transmission successful
		ndev->stats.tx_packets++;
		ndev->stats.tx_bytes += length;
		dev_kfree_skb(skb);
		return NETDEV_TX_OK;
	} else {
		netif_stop_queue(ndev);
		unimac_schedule_tx_wake(unimac);
		ndev->stats.tx_fifo_errors++;
		return NETDEV_TX_BUSY;
	}
}

static int unimac_change_mtu(struct net_device *ndev, int new_mtu)
{
	if (new_mtu < ETH_MIN_MTU ||
	    new_mtu > UNIMAC_MAX_FRAME_SIZE - ETH_HLEN)
		return -EINVAL;

	ndev->mtu = new_mtu;
	return 0;
}

/* Network device operations */
static const struct net_device_ops netdev_ops = {
	.ndo_open = unimac_open,
	.ndo_stop = unimac_stop,
	.ndo_start_xmit = unimac_start_xmit,
	.ndo_set_mac_address = unimac_set_mac_address,
	.ndo_set_rx_mode = unimac_set_rx_mode,
	.ndo_change_mtu = unimac_change_mtu,
};

static const struct ethtool_ops ethtool_ops = {
	.get_link = ethtool_op_get_link,
	.get_link_ksettings = phy_ethtool_get_link_ksettings,
	.set_link_ksettings = phy_ethtool_set_link_ksettings,
};

static s32 unimac_dqm_poll_rx(struct napi_struct *napi, struct sk_buff **skb)
{
	struct net_device *ndev = napi->dev;
	struct unimac *unimac = netdev_priv(ndev);
	unsigned int i;
	unsigned int rx_queues[] = {
		unimac->rx_high_queue,
		unimac->rx_normal_queue,
	};
	struct bcm3380_fpm_pool *fpm_pool =
		unimac->mbdma->get_fpm_pool(unimac->mbdma);

	if (!fpm_pool)
		return -ENODEV;

	*skb = NULL;

	for (i = 0; i < ARRAY_SIZE(rx_queues); i++) {
		unsigned int queue = rx_queues[i];

		if (i && queue == rx_queues[0])
			continue;

		if (!msp_dqm_queue_not_empty(unimac->msp, queue))
			continue;

		u32 token = msp_dqm_read_word(unimac->msp, queue, 0);
		size_t frame_len = fpm_token_size(token);
		const void *packet = fpm_token_to_virt(fpm_pool, token);
		if (!packet) {
			dev_err(unimac->ndev->dev.parent,
				"DQM q%u token did not map to a buffer: 0x%08X\n",
				queue, token);
			fpm_return_token(fpm_pool, token);
			return -EIO;
		}

		if (frame_len <= ETH_FCS_LEN) {
			fpm_return_token(fpm_pool, token);
			return -EINVAL;
		}

		size_t skb_len = frame_len - ETH_FCS_LEN;
		*skb = napi_alloc_skb(napi, skb_len);
		if (!*skb) {
			fpm_return_token(fpm_pool, token);
			return -ENOMEM;
		}

		void *data = skb_put_data(*skb, packet, skb_len);
		if (!data) {
			kfree_skb(*skb);
			*skb = NULL;
			fpm_return_token(fpm_pool, token);
			return -ENOMEM;
		}

#if UNIMAC_DUMP_TRAFFIC
		u32 fpm_avail_before_return = fpm_tokens_available(fpm_pool);
#endif
		fpm_return_token(fpm_pool, token);
#if UNIMAC_DUMP_TRAFFIC
		u32 fpm_avail_after_return = fpm_tokens_available(fpm_pool);

		dev_info(unimac->ndev->dev.parent,
			 "RX frame len=%zu skb_len=%zu\n", frame_len, skb_len);
		print_hex_dump(KERN_INFO, "unimac rx: ", DUMP_PREFIX_NONE, 16, 1,
			       data, min_t(size_t, skb_len, 64), false);
		dev_info(unimac->ndev->dev.parent,
			 "DQM q%u -> CPU: token=0x%08X len=%zu fpm_avail=%u->%u not_empty=0x%08X q_sts=0x%08X\n",
			 queue, token, skb_len, fpm_avail_before_return,
			 fpm_avail_after_return,
			 msp_dqm_not_empty_status(unimac->msp),
			 msp_dqm_queue_status(unimac->msp, queue));
#endif
		return skb_len;
	}

	return 0;
}

static int unimac_poll(struct napi_struct *napi, int budget) {
	struct net_device *ndev = napi->dev;
	struct unimac *unimac = netdev_priv(ndev);
	int work_done = 0;

	while (work_done < budget) {
		struct sk_buff *skb;
		s32 outcome = unimac_dqm_poll_rx(napi, &skb);
		if (outcome == 0) {
			break;
		} else if (outcome < 0) {
			dev_err(ndev->dev.parent, "RX error: %d\n", outcome);
			ndev->stats.rx_dropped++;
			work_done++;
			continue;
		}
		size_t length = outcome;
		skb->protocol = eth_type_trans(skb, ndev);
#if UNIMAC_DUMP_TRAFFIC
		dev_info(ndev->dev.parent, "RX skb len=%zu\n", length);
		print_hex_dump(KERN_INFO, "unimac skb: ", DUMP_PREFIX_NONE, 16, 1,
			       skb_mac_header(skb), min_t(size_t, length, 64), false);
#endif

		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += length;
		work_done++;
		napi_gro_receive(napi, skb);
	}

	if (work_done < budget) {
		if (napi_complete_done(napi, work_done))
			msp_dqm_host_not_empty_irq_rearm(unimac->msp, unimac_rx_queue_mask(unimac));
	}

	return work_done;
}

/* Probe function - called when device is discovered */
static int unimac_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct net_device *ndev;
	struct unimac *priv;
	struct resource *res;
	int err;

	/* Allocate network device */
	ndev = devm_alloc_etherdev(dev, sizeof(*priv));
	if (!ndev)
		return -ENOMEM;

	platform_set_drvdata(pdev, ndev);
	SET_NETDEV_DEV(ndev, dev);

	priv = netdev_priv(ndev);
	priv->ndev = ndev;
	priv->variant = of_device_get_match_data(dev);
	if (!priv->variant) {
		err = -EINVAL;
		dev_err_probe(dev, err, "missing UniMAC variant data\n");
		goto err_free_netdev;
	}

	/* Get MMIO resources */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(priv->base)) {
		err = PTR_ERR(priv->base);
		goto err_free_netdev;
	}

	err = of_property_read_u32(node, "brcm,mac-id", &priv->mac_id);
	if (err) {
		dev_err_probe(dev, err, "missing brcm,mac-id\n");
		goto err_free_netdev;
	}
	dev_info(dev, "UniMAC instance: mac_id=%u\n", priv->mac_id);

	err = of_get_phy_mode(node, &priv->phy_interface);
	if (err)
		priv->phy_interface = PHY_INTERFACE_MODE_NA;

	struct bcm3380_gphy *gphy;
	err = gphy_get(dev, &gphy);
	if (err && err != -ENODEV) {
		dev_err_probe(dev, err, "failed to get GPHY provider\n");
		goto err_free_netdev;
	}
	if (!err) {
		err = devm_add_action_or_reset(dev, gphy_put, gphy);
		if (err)
			goto err_free_netdev;
		dev_info(dev, "Using shared GPHY provider\n");
	}

	priv->num_clocks = devm_clk_bulk_get_all(dev, &priv->clocks);
	if (priv->num_clocks < 0) {
		err = priv->num_clocks;
		goto err_free_netdev;
	}
	if (priv->num_clocks) {
		err = clk_bulk_prepare_enable(priv->num_clocks, priv->clocks);
		if (err) {
			dev_err(dev, "error enabling UniMAC clocks: %d\n", err);
			goto err_free_netdev;
		}
		err = devm_add_action_or_reset(dev, unimac_disable_clocks, priv);
		if (err)
			goto err_free_netdev;
	}

	if (of_property_present(node, "resets")) {
		int num_resets = of_count_phandle_with_args(node, "resets", "#reset-cells");

		if (num_resets < 0) {
			err = num_resets;
			dev_err_probe(dev, err, "failed to parse UniMAC resets\n");
			goto err_free_netdev;
		}
		if (num_resets > UNIMAC_RESET_COUNT_MAX) {
			dev_err(dev, "too many resets specified: %d\n", num_resets);
			err = -EINVAL;
			goto err_free_netdev;
		}

		for (int i = 0; i < num_resets; i++) {
			priv->resets[i] = devm_reset_control_get_by_index(dev, i);
			if (IS_ERR(priv->resets[i])) {
				err = PTR_ERR(priv->resets[i]);
				dev_err_probe(dev, err, "failed to get UniMAC reset %d\n", i);
				goto err_free_netdev;
			}
		}
	}

	err = unimac_reset(priv, dev);
	if (err)
		goto err_free_netdev;

	if (of_property_present(node, "brcm,internal-phy-syscon")) {
		struct regmap *internal_phy_syscon =
			syscon_regmap_lookup_by_phandle(node, "brcm,internal-phy-syscon");

		if (IS_ERR(internal_phy_syscon)) {
			err = PTR_ERR(internal_phy_syscon);
			dev_err_probe(dev, err, "failed to get internal PHY syscon\n");
			goto err_free_netdev;
		}

		err = unimac_init_internal_phy(priv, internal_phy_syscon);
		if (err)
			goto err_free_netdev;
	}

	/* Get MAC address from device tree */
	u8 aucDtsMac[ETH_ALEN];
	of_get_mac_address(pdev->dev.of_node, aucDtsMac);
	if (is_valid_ether_addr(aucDtsMac)) {
		dev_addr_set(ndev, aucDtsMac);
		dev_info(dev, "Using MAC from DTS: %pM\n", aucDtsMac);
	} else {
		eth_hw_addr_random(ndev);
		dev_info(dev, "Using random MAC address\n");
	}

	priv->mbdma = unimac_mbdma_get(dev);
	if (IS_ERR(priv->mbdma)) {
		err = PTR_ERR(priv->mbdma);
		dev_err_probe(dev, err, "failed to get MBDMA provider\n");
		goto err_free_netdev;
	}
	if (!priv->mbdma->get_fpm_pool(priv->mbdma)) {
		err = -EINVAL;
		dev_err_probe(dev, err, "MBDMA provider has no FPM pool\n");
		goto err_free_netdev;
	}
	dev_info(dev, "Using MBDMA FPM pool\n");

	err = msp_get(dev, &priv->msp);
	if (err) {
		dev_err_probe(dev, err, "failed to get MSP provider\n");
		goto err_free_netdev;
	}

	err = msp_dqm_get_queue(priv->msp, dev, "brcm,rx_normal_queue",
				&priv->rx_normal_queue);
	if (err) {
		dev_err_probe(dev, err, "failed to get RX normal DQM queue\n");
		goto err_put_msp;
	}

	err = msp_dqm_get_queue(priv->msp, dev, "brcm,rx_high_queue",
				&priv->rx_high_queue);
	if (err) {
		dev_err_probe(dev, err, "failed to get RX high DQM queue\n");
		goto err_put_msp;
	}

	err = msp_dqm_get_queue(priv->msp, dev, "brcm,tx_high_queue",
				&priv->tx_high_queue);
	if (err) {
		dev_err_probe(dev, err, "failed to get TX high DQM queue\n");
		goto err_put_msp;
	}

	err = msp_dqm_get_queue(priv->msp, dev, "brcm,tx_normal_queue",
				&priv->tx_normal_queue);
	if (err) {
		dev_err_probe(dev, err, "failed to get TX normal DQM queue\n");
		goto err_put_msp;
	}

	if (priv->rx_normal_queue == priv->rx_high_queue ||
	    priv->tx_high_queue == priv->tx_normal_queue) {
		dev_err(dev, "DQM queue bindings must use distinct RX and TX queues\n");
		err = -EINVAL;
		goto err_put_msp;
	}

	dev_info(dev,
		 "Using MSP DQM queues: rx_normal=%u rx_high=%u tx_high=%u tx_normal=%u\n",
		 priv->rx_normal_queue, priv->rx_high_queue,
		 priv->tx_high_queue, priv->tx_normal_queue);

	INIT_DELAYED_WORK(&priv->tx_wake_work, unimac_tx_wake_work);

	err = devm_of_platform_populate(dev);
	if (err) {
		dev_err_probe(dev, err, "failed to populate UniMAC child devices\n");
		goto err_put_msp;
	}

	/* Set up network device */
	ndev->netdev_ops = &netdev_ops;
	ndev->ethtool_ops = &ethtool_ops;
	ndev->min_mtu = ETH_MIN_MTU;
	ndev->max_mtu = UNIMAC_MAX_FRAME_SIZE - ETH_HLEN;
	netif_napi_add(ndev, &priv->napi, unimac_poll);

	/* Register network device */
	err = devm_register_netdev(dev, ndev);

	if (err)
		goto err_put_msp;

	netif_carrier_off(ndev);

	return 0;

err_put_msp:
	msp_put(priv->msp);
err_free_netdev:
	return err;
}

/* Remove function */
static void unimac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct unimac *priv = netdev_priv(ndev);

	cancel_delayed_work_sync(&priv->tx_wake_work);
	msp_put(priv->msp);
}

static const struct unimac_variant bcm3380_unimac_variant = {
	.configure_backpressure = bcm3380_configure_backpressure,
	.assert_resets_on_stop = true,
};

static const struct unimac_variant bcm3383_unimac_variant = {
	.configure_backpressure = bcm3383_configure_backpressure,
	.configure_interface = bcm3383_configure_interface,
};

static const struct of_device_id bcm3383_unimac_of_match[] = {
	{ .compatible = "brcm,bcm3380-unimac", .data = &bcm3380_unimac_variant },
	{ .compatible = "brcm,bcm3383-unimac", .data = &bcm3383_unimac_variant },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm3383_unimac_of_match);

/* Platform driver definition */
static struct platform_driver bcm3383_unimac_driver = {
	.probe = unimac_probe,
	.remove = unimac_remove,
	.driver = {
		.name = "bcm3383-unimac",
		.of_match_table = bcm3383_unimac_of_match,
	},
};

module_platform_driver(bcm3383_unimac_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3380/BCM3383 Ethernet UniMAC Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bcm3380-unimac");
MODULE_ALIAS("platform:bcm3383-unimac");
