#include <linux/clk.h>
#include <linux/bits.h>
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
#include <linux/reset.h>
#include <linux/workqueue.h>
#include <linux/if_ether.h>
#include "unimac.h"

#include <linux/types.h>
#include <soc/bcm/bcm3380-fpm.h>
#include <soc/bcm/bcm3380-gphy.h>
#include <soc/bcm/bcm3380-msp.h>

#define BCM3380_UNIMAC_DUMP_TRAFFIC 0

#define UNIMAC_ENET_HIGH_PRIORITY_START		3
#define UNIMAC_MAX_FRAME_SIZE			2048
#define UNIMAC_TX_WAKE_DELAY_MS			1

#define UNIMAC_MBDMA_OFFSET			0x0000

// MbdmaStatus.Reg32
#define UNIMAC_MBDMA_STATUS			0x0000
#define UNIMAC_MBDMA_STATUS_GBL_INTR_MASK	BIT(31)
#define UNIMAC_MBDMA_STATUS_INTR_MASK		GENMASK(25, 16)
#define UNIMAC_MBDMA_STATUS_INTR_MASK_SET	(BIT(25) | BIT(23) | BIT(22) | BIT(21) | BIT(16))
#define UNIMAC_MBDMA_STATUS_TX_MSGQ_OVERFLOW	BIT(9)
#define UNIMAC_MBDMA_STATUS_INVALID_TX_MSG	BIT(8)
#define UNIMAC_MBDMA_STATUS_UBUS_ERROR		BIT(6)
#define UNIMAC_MBDMA_STATUS_TOKEN_RD_ERROR	BIT(5)
#define UNIMAC_MBDMA_STATUS_INVALID_TOKEN	BIT(4)
#define UNIMAC_MBDMA_STATUS_ALLOC_FIFO_EMPTY	BIT(3)
#define UNIMAC_MBDMA_STATUS_FREE_FIFO_FULL	BIT(0)
#define UNIMAC_MBDMA_STATUS_CLEAR		(UNIMAC_MBDMA_STATUS_GBL_INTR_MASK | \
						 UNIMAC_MBDMA_STATUS_INTR_MASK | \
						 UNIMAC_MBDMA_STATUS_TX_MSGQ_OVERFLOW | \
						 UNIMAC_MBDMA_STATUS_INVALID_TX_MSG | \
						 UNIMAC_MBDMA_STATUS_UBUS_ERROR | \
						 UNIMAC_MBDMA_STATUS_TOKEN_RD_ERROR | \
						 UNIMAC_MBDMA_STATUS_INVALID_TOKEN | \
						 UNIMAC_MBDMA_STATUS_ALLOC_FIFO_EMPTY | \
						 UNIMAC_MBDMA_STATUS_FREE_FIFO_FULL)
#define UNIMAC_MBDMA_STATUS_SET		(UNIMAC_MBDMA_STATUS_GBL_INTR_MASK | \
						 UNIMAC_MBDMA_STATUS_INTR_MASK_SET | \
						 UNIMAC_MBDMA_STATUS_TX_MSGQ_OVERFLOW | \
						 UNIMAC_MBDMA_STATUS_INVALID_TX_MSG | \
						 UNIMAC_MBDMA_STATUS_UBUS_ERROR | \
						 UNIMAC_MBDMA_STATUS_TOKEN_RD_ERROR | \
						 UNIMAC_MBDMA_STATUS_INVALID_TOKEN | \
						 UNIMAC_MBDMA_STATUS_ALLOC_FIFO_EMPTY | \
						 UNIMAC_MBDMA_STATUS_FREE_FIFO_FULL)

// MbdmaTokenCacheCtl.Reg32
#define UNIMAC_MBDMA_TOKEN_CACHE_CTL		0x0004
#define UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_ENABLE	BIT(31)
#define UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_MASK	GENMASK(28, 24)
#define UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_2	BIT(25)
#define UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_THRESH_MASK	GENMASK(23, 16)
#define UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_THRESH_4		BIT(18)
#define UNIMAC_MBDMA_TOKEN_CACHE_FREE_ENABLE	BIT(15)
#define UNIMAC_MBDMA_TOKEN_CACHE_FREE_MAX_BURST_MASK	GENMASK(12, 8)
#define UNIMAC_MBDMA_TOKEN_CACHE_FREE_MAX_BURST_16	BIT(12)
#define UNIMAC_MBDMA_TOKEN_CACHE_FREE_THRESH_MASK	GENMASK(7, 0)
#define UNIMAC_MBDMA_TOKEN_CACHE_FREE_THRESH_16		BIT(4)
#define UNIMAC_MBDMA_TOKEN_CACHE_CLEAR		(UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_ENABLE | \
						 UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_MASK | \
						 UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_THRESH_MASK | \
						 UNIMAC_MBDMA_TOKEN_CACHE_FREE_ENABLE | \
						 UNIMAC_MBDMA_TOKEN_CACHE_FREE_MAX_BURST_MASK | \
						 UNIMAC_MBDMA_TOKEN_CACHE_FREE_THRESH_MASK)
#define UNIMAC_MBDMA_TOKEN_CACHE_SET		(UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_ENABLE | \
						 UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_MAX_BURST_2 | \
						 UNIMAC_MBDMA_TOKEN_CACHE_ALLOC_THRESH_4 | \
						 UNIMAC_MBDMA_TOKEN_CACHE_FREE_ENABLE | \
						 UNIMAC_MBDMA_TOKEN_CACHE_FREE_MAX_BURST_16 | \
						 UNIMAC_MBDMA_TOKEN_CACHE_FREE_THRESH_16)

// MbdmaRegisters.Tokenaddress
#define UNIMAC_MBDMA_TOKEN_ADDRESS		0x0008

// MbdmaGlobalCtl.Reg32
#define UNIMAC_MBDMA_GLOBAL_CTL			0x000c
#define UNIMAC_MBDMA_GLOBAL_CTL_FLUSH_CACHE	BIT(23)
#define UNIMAC_MBDMA_GLOBAL_CTL_ALLOC_LIMIT_MASK	GENMASK(31, 24)
#define UNIMAC_MBDMA_GLOBAL_CTL_ALLOC_LIMIT_6	(BIT(26) | BIT(25))
#define UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_MASK	GENMASK(11, 6)
#define UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_49	(BIT(11) | BIT(10) | BIT(6))
#define UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_MASK	GENMASK(5, 0)
#define UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_1	BIT(0)
#define UNIMAC_MBDMA_GLOBAL_CTL_CLEAR		(UNIMAC_MBDMA_GLOBAL_CTL_ALLOC_LIMIT_MASK | \
						 UNIMAC_MBDMA_GLOBAL_CTL_FLUSH_CACHE | \
						 UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_MASK | \
						 UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_MASK)
#define UNIMAC_MBDMA_GLOBAL_CTL_SET		(UNIMAC_MBDMA_GLOBAL_CTL_ALLOC_LIMIT_6 | \
						 UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_3W_49 | \
						 UNIMAC_MBDMA_GLOBAL_CTL_LAN_TX_MSG_ID_2W_1)

// MbdmaRegisters.Bufferbase
#define UNIMAC_MBDMA_BUFFER_BASE		0x0010

// MbdmaBufferSize.Reg32
#define UNIMAC_MBDMA_BUFFER_SIZE		0x0014

// MbdmaRxChanControl.Reg32
#define UNIMAC_MBDMA_RX_CHAN_CONTROL0		0x0040
#define UNIMAC_MBDMA_RX_MAX_BURST_KEEP		0xe00fffff
#define UNIMAC_MBDMA_RX_MAX_BURST_VALUE		0x04000000
#define UNIMAC_MBDMA_RX_MAC_ID_KEEP		0xfffc0fff
#define UNIMAC_MBDMA_RX_MSG_ID_KEEP		0xfffff0ff
#define UNIMAC_MBDMA_RX_MSG_ID_VALUE		0x00000500

// MbdmaRegisters.Lanmsgaddress0
#define UNIMAC_MBDMA_LAN_MSG_ADDRESS0		0x0044

// MbdmaTxChanControl.Reg32
#define UNIMAC_MBDMA_TX_CHAN_CONTROL1		0x0060
#define UNIMAC_MBDMA_TX_MAX_BURST_KEEP		0xe00fffff
#define UNIMAC_MBDMA_TX_MAX_BURST_VALUE		0x04000000
#define UNIMAC_MBDMA_TX_MSG_ID_KEEP		0xffffc0ff
#define UNIMAC_MBDMA_TX_MSG_ID_VALUE		0x00000100
#define UNIMAC_MBDMA_TX_MAC_ID_KEEP		0xffffff0f
#define UNIMAC_MBDMA_TX_MAC_ID_VALUE		0x00000090
#define UNIMAC_MBDMA_TX_MAX_REQS_KEEP		0xfffffff0
#define UNIMAC_MBDMA_TX_MAX_REQS_VALUE		0x00000004

// MbdmaRegisters.Lanmsgaddress1
#define UNIMAC_MBDMA_LAN_MSG_ADDRESS1		0x0064

// MbdmaRegisters.Lantxmsgfifo01
#define UNIMAC_MBDMA_LAN_TX_MSG_FIFO01		0x0500

#define UNIMAC_INTERFACE_OFFSET			0x0600

// UnimacInterfaceControl.Reg32
#define UNIMAC_INTERFACE_CONTROL		0x0000
#define UNIMAC_INTERFACE_CONTROL_BACKPRESSURE_MUX	0x00000200
#define UNIMAC_INTERFACE_CONTROL_FPM_BACKPRESSURE	0x00000100
#define UNIMAC_INTERFACE_CONTROL_TOKEN_BACKPRESSURE	0x00000080

// UnimacInterfaceBackPressure.Reg32
#define UNIMAC_INTERFACE_BACK_PRESSURE		0x0028
#define UNIMAC_INTERFACE_BACK_PRESSURE_ENABLE	BIT(0)

#define UNIMAC_CORE_OFFSET			0x0800

/* Private driver data structure */
struct bcm3380_unimac {
	struct net_device *ndev;
	void __iomem *base;
	resource_size_t phys;

	struct bcm3380_fpm_pool *fpm_pool;
	struct bcm3380_msp *msp;

	// DQM queue IDs for RX and TX
	unsigned int rx_normal_queue;
	unsigned int rx_high_queue;
	unsigned int tx_high_queue;
	unsigned int tx_normal_queue;

	struct delayed_work tx_wake_work;

	struct clk_bulk_data *clocks;
	int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

	phy_interface_t phy_interface;
	struct napi_struct napi;
};

static inline void __iomem *unimac_mbdma(struct bcm3380_unimac *unimac, u32 reg)
{
	return unimac->base + UNIMAC_MBDMA_OFFSET + reg;
}

static inline u32 unimac_mbdma_bus_addr(struct bcm3380_unimac *unimac, u32 reg)
{
	return unimac->phys + UNIMAC_MBDMA_OFFSET + reg;
}

static inline void __iomem *unimac_interface(struct bcm3380_unimac *unimac, u32 reg)
{
	return unimac->base + UNIMAC_INTERFACE_OFFSET + reg;
}

static inline void __iomem *unimac_core(struct bcm3380_unimac *unimac, u32 reg)
{
	return unimac->base + UNIMAC_CORE_OFFSET + reg;
}

static inline u32 unimac_rx_queue_mask(struct bcm3380_unimac *unimac)
{
	return BIT(unimac->rx_normal_queue) | BIT(unimac->rx_high_queue);
}

static void unimac_disable_clocks(void *data)
{
	struct bcm3380_unimac *unimac = data;

	clk_bulk_disable_unprepare(unimac->num_clocks, unimac->clocks);
}

static int unimac_reset(struct bcm3380_unimac *unimac, struct device *dev)
{
	for (int i = 0; i < unimac->num_resets; i++) {
		if (!IS_ERR_OR_NULL(unimac->reset[i])) {
			int err = reset_control_assert(unimac->reset[i]);

			if (err) {
				dev_err(dev, "error asserting UniMAC reset %d\n", i);
				return err;
			}
		}
	}

	mdelay(1);

	for (int i = 0; i < unimac->num_resets; i++) {
		if (!IS_ERR_OR_NULL(unimac->reset[i])) {
			int err = reset_control_deassert(unimac->reset[i]);

			if (err) {
				dev_err(dev, "error deasserting UniMAC reset %d\n", i);
				return err;
			}
		}
	}

	usleep_range(10000, 20000);

	return 0;
}

static u32 vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn,
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

	u32 token = fpm_borrow_token(unimac->fpm_pool);
	if (!fpm_token_valid(token)) {
		dev_warn_ratelimited(unimac->ndev->dev.parent,
				     "FPM pool has no token available for TX\n");
		return 0;
	}

	void *dma_dest = fpm_token_to_virt(unimac->fpm_pool, token);
	if (!dma_dest) {
		dev_err(unimac->ndev->dev.parent,
			"TX token did not map to a buffer: 0x%08X\n", token);
		fpm_return_token(unimac->fpm_pool, token);
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

#if BCM3380_UNIMAC_DUMP_TRAFFIC
	dev_info(unimac->ndev->dev.parent,
		 "DQM q%u <- TX token=0x%08X len=%zu priority=%u q_sts=0x%08X\n",
		 tx_queue, adjusted_token, clamped_length, priority,
		 msp_dqm_queue_status(unimac->msp, tx_queue));
#endif
	return 1;
}

static void unimac_schedule_tx_wake(struct bcm3380_unimac *unimac)
{
	mod_delayed_work(system_wq, &unimac->tx_wake_work,
			 msecs_to_jiffies(UNIMAC_TX_WAKE_DELAY_MS));
}

static void unimac_tx_wake_work(struct work_struct *work)
{
	struct bcm3380_unimac *unimac =
		container_of(to_delayed_work(work), struct bcm3380_unimac,
			     tx_wake_work);
	u32 token;

	if (!netif_running(unimac->ndev))
		return;

	if (!msp_dqm_queue_has_space(unimac->msp, unimac->tx_normal_queue) &&
	    !msp_dqm_queue_has_space(unimac->msp, unimac->tx_high_queue)) {
		unimac_schedule_tx_wake(unimac);
		return;
	}

	token = fpm_borrow_token(unimac->fpm_pool);
	if (!fpm_token_valid(token)) {
		unimac_schedule_tx_wake(unimac);
		return;
	}

	fpm_return_token(unimac->fpm_pool, token);
	netif_wake_queue(unimac->ndev);
}

static void unimac_msp_dqm_host_not_empty_irq(void *data)
{
	struct bcm3380_unimac *unimac = data;

	if (napi_schedule_prep(&unimac->napi))
		__napi_schedule(&unimac->napi);
}

static void unimac_set_rx_mode(struct net_device *ndev)
{
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	u32 cmd = readl_be(unimac_core(unimac, UMAC_CMD));

	if (ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))
		cmd |= CMD_PROMISC;
	else
		cmd &= ~CMD_PROMISC;

	writel_be(cmd, unimac_core(unimac, UMAC_CMD));
}

static int unimac_set_mac_address(struct net_device *ndev, void *p) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
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
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
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
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;
	struct device_node *phy_np = of_parse_phandle(dev->of_node, "phy-handle", 0);

	if (!phy_np)
		return 0;

	struct phy_device *phydev = of_phy_connect(ndev, phy_np, unimac_adjust_link, 0, unimac->phy_interface);
	of_node_put(phy_np);
	if (!phydev)
		return dev_err_probe(dev, -ENODEV, "failed to connect PHY\n");

	phy_attached_info(phydev);

	return 0;
}

static int unimac_open(struct net_device *ndev) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
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
	u32 cmd;
	u32 val;
	u32 uiLanTxMsgFifo = unimac_mbdma_bus_addr(unimac, UNIMAC_MBDMA_LAN_TX_MSG_FIFO01);
	msp_4ke_set_host_mbox_out(unimac->msp, uiLanTxMsgFifo);
	cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd | CMD_SW_RESET, unimac_core(unimac, UMAC_CMD));
	cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd & ~CMD_SW_RESET, unimac_core(unimac, UMAC_CMD));
	writel_be(UNIMAC_MAX_FRAME_SIZE, unimac_core(unimac, UMAC_MAX_FRAME_LEN));
	memcpy(addr.sa_data, ndev->dev_addr, ETH_ALEN);
	unimac_set_mac_address(ndev, &addr);
	writel_be(fpm_buffer_base_dma(unimac->fpm_pool), unimac_mbdma(unimac, UNIMAC_MBDMA_BUFFER_BASE));
	writel_be(fpm_buffer_size_code(unimac->fpm_pool), unimac_mbdma(unimac, UNIMAC_MBDMA_BUFFER_SIZE));
	writel_be(fpm_alloc_free_bus_addr(unimac->fpm_pool), unimac_mbdma(unimac, UNIMAC_MBDMA_TOKEN_ADDRESS));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_GLOBAL_CTL));
	writel_be(val | UNIMAC_MBDMA_GLOBAL_CTL_FLUSH_CACHE, unimac_mbdma(unimac, UNIMAC_MBDMA_GLOBAL_CTL));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_GLOBAL_CTL));
	writel_be((val & ~UNIMAC_MBDMA_GLOBAL_CTL_CLEAR) | UNIMAC_MBDMA_GLOBAL_CTL_SET, unimac_mbdma(unimac, UNIMAC_MBDMA_GLOBAL_CTL));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_TOKEN_CACHE_CTL));
	writel_be((val & ~UNIMAC_MBDMA_TOKEN_CACHE_CLEAR) | UNIMAC_MBDMA_TOKEN_CACHE_SET, unimac_mbdma(unimac, UNIMAC_MBDMA_TOKEN_CACHE_CTL));
	dev_info(dev, "UniMAC FPM pool1: bufferbase=0x%08X buffersize=0x%08X tokenaddress=0x%08X\n",
		 readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_BUFFER_BASE)),
		 readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_BUFFER_SIZE)),
		 readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_TOKEN_ADDRESS)));

	// Rx channel
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_RX_CHAN_CONTROL0));
	writel_be((val & UNIMAC_MBDMA_RX_MAX_BURST_KEEP) | UNIMAC_MBDMA_RX_MAX_BURST_VALUE, unimac_mbdma(unimac, UNIMAC_MBDMA_RX_CHAN_CONTROL0));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_RX_CHAN_CONTROL0));
	writel_be(val & UNIMAC_MBDMA_RX_MAC_ID_KEEP, unimac_mbdma(unimac, UNIMAC_MBDMA_RX_CHAN_CONTROL0));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_RX_CHAN_CONTROL0));
	writel_be((val & UNIMAC_MBDMA_RX_MSG_ID_KEEP) | UNIMAC_MBDMA_RX_MSG_ID_VALUE, unimac_mbdma(unimac, UNIMAC_MBDMA_RX_CHAN_CONTROL0));
	writel_be(uiInMsgDataPhysicalAddr, unimac_mbdma(unimac, UNIMAC_MBDMA_LAN_MSG_ADDRESS0));

	// Tx channel
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	writel_be((val & UNIMAC_MBDMA_TX_MAX_BURST_KEEP) | UNIMAC_MBDMA_TX_MAX_BURST_VALUE, unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	writel_be((val & UNIMAC_MBDMA_TX_MSG_ID_KEEP) | UNIMAC_MBDMA_TX_MSG_ID_VALUE, unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	writel_be((val & UNIMAC_MBDMA_TX_MAC_ID_KEEP) | UNIMAC_MBDMA_TX_MAC_ID_VALUE, unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	writel_be((val & UNIMAC_MBDMA_TX_MAX_REQS_KEEP) | UNIMAC_MBDMA_TX_MAX_REQS_VALUE, unimac_mbdma(unimac, UNIMAC_MBDMA_TX_CHAN_CONTROL1));
	writel_be(uiInMsgDataPhysicalAddr, unimac_mbdma(unimac, UNIMAC_MBDMA_LAN_MSG_ADDRESS1));
	val = readl_be(unimac_mbdma(unimac, UNIMAC_MBDMA_STATUS));
	writel_be((val & ~UNIMAC_MBDMA_STATUS_CLEAR) | UNIMAC_MBDMA_STATUS_SET, unimac_mbdma(unimac, UNIMAC_MBDMA_STATUS));

	cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be((cmd & ~CMD_PROMISC) | CMD_NO_LEN_CHK, unimac_core(unimac, UMAC_CMD));
	val = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_CONTROL));
	val |= UNIMAC_INTERFACE_CONTROL_BACKPRESSURE_MUX;
	val |= UNIMAC_INTERFACE_CONTROL_FPM_BACKPRESSURE;
	val |= UNIMAC_INTERFACE_CONTROL_TOKEN_BACKPRESSURE;
	writel_be(val, unimac_interface(unimac, UNIMAC_INTERFACE_CONTROL));
	val = readl_be(unimac_interface(unimac, UNIMAC_INTERFACE_BACK_PRESSURE));
	writel_be(val | UNIMAC_INTERFACE_BACK_PRESSURE_ENABLE, unimac_interface(unimac, UNIMAC_INTERFACE_BACK_PRESSURE));
	unimac_set_rx_mode(ndev);
	/* Initialize Unimac End*/

	err = unimac_phy_connect(ndev);
	if (err)
		return err;

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
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;

	dev_info(dev, "stopping UniMAC\n");

	netif_stop_queue(ndev);
	cancel_delayed_work_sync(&unimac->tx_wake_work);
	msp_dqm_host_not_empty_irq_unregister(unimac->msp);
	if (ndev->phydev) {
		phy_stop(ndev->phydev);
		phy_disconnect(ndev->phydev);
	}
	netif_carrier_off(ndev);
	napi_disable(&unimac->napi);

	u32 cmd = readl_be(unimac_core(unimac, UMAC_CMD));
	writel_be(cmd & ~(CMD_TX_EN | CMD_RX_EN), unimac_core(unimac, UMAC_CMD));

	for (int i = 0; i < unimac->num_resets; i++) {
		if (!IS_ERR_OR_NULL(unimac->reset[i])) {
			if (reset_control_assert(unimac->reset[i]))
				dev_err(&ndev->dev, "error asserting Unimac reset %d\n", i);
		}
	}

	netdev_reset_queue(ndev);

	return 0;
}

static netdev_tx_t unimac_start_xmit(struct sk_buff *skb, struct net_device *ndev) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
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

#if BCM3380_UNIMAC_DUMP_TRAFFIC
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
static const struct net_device_ops bcm3380_netdev_ops = {
	.ndo_open = unimac_open,
	.ndo_stop = unimac_stop,
	.ndo_start_xmit = unimac_start_xmit,
	.ndo_set_mac_address = unimac_set_mac_address,
	.ndo_set_rx_mode = unimac_set_rx_mode,
	.ndo_change_mtu = unimac_change_mtu,
};

static const struct ethtool_ops bcm3380_ethtool_ops = {
	.get_link = ethtool_op_get_link,
	.get_link_ksettings = phy_ethtool_get_link_ksettings,
	.set_link_ksettings = phy_ethtool_set_link_ksettings,
};

static s32 bcm3380_dqm_poll_rx(struct napi_struct *napi,
			       struct sk_buff **skb)
{
	struct net_device *ndev = napi->dev;
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	unsigned int i;
	unsigned int rx_queues[] = {
		unimac->rx_high_queue,
		unimac->rx_normal_queue,
	};

	*skb = NULL;

	for (i = 0; i < ARRAY_SIZE(rx_queues); i++) {
		unsigned int queue = rx_queues[i];

		if (i && queue == rx_queues[0])
			continue;

		if (!msp_dqm_queue_not_empty(unimac->msp, queue))
			continue;

		u32 token = msp_dqm_read_word(unimac->msp, queue, 0);
		size_t frame_len = fpm_token_size(token);
		const void *packet = fpm_token_to_virt(unimac->fpm_pool, token);
		if (!packet) {
			dev_err(unimac->ndev->dev.parent,
				"DQM q%u token did not map to a buffer: 0x%08X\n",
				queue, token);
			fpm_return_token(unimac->fpm_pool, token);
			return -EIO;
		}

		if (frame_len <= ETH_FCS_LEN) {
			fpm_return_token(unimac->fpm_pool, token);
			return -EINVAL;
		}

		size_t skb_len = frame_len - ETH_FCS_LEN;
		*skb = napi_alloc_skb(napi, skb_len);
		if (!*skb) {
			fpm_return_token(unimac->fpm_pool, token);
			return -ENOMEM;
		}

		void *data = skb_put_data(*skb, packet, skb_len);
		if (!data) {
			kfree_skb(*skb);
			*skb = NULL;
			fpm_return_token(unimac->fpm_pool, token);
			return -ENOMEM;
		}

		fpm_return_token(unimac->fpm_pool, token);

#if BCM3380_UNIMAC_DUMP_TRAFFIC
		dev_info(unimac->ndev->dev.parent,
			 "RX frame len=%zu skb_len=%zu\n", frame_len, skb_len);
		print_hex_dump(KERN_INFO, "unimac rx: ", DUMP_PREFIX_NONE, 16, 1,
			       data, min_t(size_t, skb_len, 64), false);
		dev_info(unimac->ndev->dev.parent,
			 "DQM q%u -> CPU: token=0x%08X len=%zu not_empty=0x%08X q_sts=0x%08X\n",
			 queue, token, skb_len,
			 msp_dqm_not_empty_status(unimac->msp),
			 msp_dqm_queue_status(unimac->msp, queue));
#endif
		return skb_len;
	}

	return 0;
}

static int unimac_poll(struct napi_struct *napi, int budget) {
	struct net_device *ndev = napi->dev;
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	int work_done = 0;

	while (work_done < budget) {
		struct sk_buff *skb;
		s32 outcome = bcm3380_dqm_poll_rx(napi, &skb);
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
#if BCM3380_UNIMAC_DUMP_TRAFFIC
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
	struct bcm3380_unimac *priv;
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

	/* Get MMIO resources */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(priv->base)) {
		err = PTR_ERR(priv->base);
		goto err_free_netdev;
	}
	priv->phys = res->start;

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
	err = clk_bulk_prepare_enable(priv->num_clocks, priv->clocks);
	if (err) {
		dev_err(dev, "error enabling UniMAC clocks: %d\n", err);
		goto err_free_netdev;
	}
	err = devm_add_action_or_reset(dev, unimac_disable_clocks, priv);
	if (err)
		goto err_free_netdev;

	err = of_count_phandle_with_args(node, "resets", "#reset-cells");
	priv->num_resets = err < 0 ? 0 : err;
	if (priv->num_resets) {
		priv->reset = devm_kcalloc(dev, priv->num_resets,
					   sizeof(struct reset_control *),
					   GFP_KERNEL);
		if (!priv->reset)
			return -ENOMEM;
		
	}
	for (int i = 0; i < priv->num_resets; i++) {
		priv->reset[i] = devm_reset_control_get_by_index(dev, i);
		if (IS_ERR_OR_NULL(priv->reset[i])) {
			dev_err(dev, "error getting Unimac reset %d\n", i);
			return PTR_ERR(priv->reset[i]);
		}
	}
	err = unimac_reset(priv, dev);
	if (err)
		return err;

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

	err = fpm_pool_get(dev, &priv->fpm_pool);
	if (err) {
		dev_err_probe(dev, err, "failed to get FPM pool provider\n");
		goto err_free_netdev;
	}
	dev_info(dev, "Using FPM pool\n");

	err = msp_get(dev, &priv->msp);
	if (err) {
		dev_err_probe(dev, err, "failed to get MSP provider\n");
		goto err_put_fpm;
	}
	dev_info(dev, "Using MSP IOPROC\n");

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
	ndev->netdev_ops = &bcm3380_netdev_ops;
	ndev->ethtool_ops = &bcm3380_ethtool_ops;
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
err_put_fpm:
	fpm_pool_put(priv->fpm_pool);
err_free_netdev:
	return err;
}

/* Remove function */
static void unimac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct bcm3380_unimac *priv = netdev_priv(ndev);

	cancel_delayed_work_sync(&priv->tx_wake_work);
	msp_put(priv->msp);
	fpm_pool_put(priv->fpm_pool);
}

static const struct of_device_id bcm3380_unimac_of_match[] = {
	{ .compatible = "brcm,bcm3380-unimac", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bcm3380_unimac_of_match);

/* Platform driver definition */
static struct platform_driver bcm3380_unimac_driver = {
	.probe = unimac_probe,
	.remove = unimac_remove,
	.driver = {
		.name = "bcm3380-unimac",
		.of_match_table = bcm3380_unimac_of_match,
	},
};

module_platform_driver(bcm3380_unimac_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3380 Ethernet Unimac Driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bcm3380-unimac");
