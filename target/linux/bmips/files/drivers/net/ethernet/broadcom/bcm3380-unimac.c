#include <linux/clk.h>
#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/of_clk.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/phy.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/reset.h>
#include <linux/mii.h>
#include <linux/if_ether.h>
#include "unimac.h"

#include <linux/types.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#include <bcm3380/unimac.h>
#include <soc/bcm/bcm3380-fpm.h>
#include <soc/bcm/bcm3380-msp.h>

#define BCM3380_UNIMAC_DUMP_TRAFFIC 0

#define BCM3380_ENET_HIGH_PRIORITY_START 3
#define BCM3380_UNIMAC_MAX_FRAME_LEN 2048
#define UNIMAC_CMD_PROMIS_EN 0x00000010
#define UNIMAC_CMD_NO_LENGTH_CHECK 0x01000000

// macro to convert logical data addresses to physical
// DMA hardware must see physical address
#define LtoP( logicalAddr ) ( ((uint32_t)(logicalAddr)) & 0x1FFFFFFF )

/* Private driver data structure */
struct bcm3380_unimac {
	struct net_device *ndev;
	void __iomem *base;

	struct bcm3380_fpm *fpm;
	struct bcm3380_msp *msp;

	// DQM queue IDs for RX and TX
	unsigned int rx_normal_queue;
	unsigned int rx_high_queue;
	unsigned int tx_high_queue;
	unsigned int tx_normal_queue;

	spinlock_t fifo_lock;

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

	struct napi_struct napi;
	struct mii_bus *mii_bus;
};

static inline u32 unimac_rx_queue_mask(struct bcm3380_unimac *unimac)
{
	return BIT(unimac->rx_normal_queue) | BIT(unimac->rx_high_queue);
}

static uint32_t vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn,
			    const void *buffer, u32 priority);

static void unimac_msp_dqm_host_not_empty_irq(void *data)
{
	struct bcm3380_unimac *unimac = data;

	if (napi_schedule_prep(&unimac->napi))
		__napi_schedule(&unimac->napi);
}

static void unimac_set_rx_mode(struct net_device *ndev)
{
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	volatile Unimac *g_pxUnimacSelected = (volatile Unimac *)unimac->base;
	uint32_t cmd = g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32;

	if (ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))
		cmd |= UNIMAC_CMD_PROMIS_EN;
	else
		cmd &= ~UNIMAC_CMD_PROMIS_EN;

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 = cmd;
}

static int unimac_set_mac_address(struct net_device *ndev, void *p) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	eth_hw_addr_set(ndev, addr->sa_data);

	uint32_t uiMacHi = addr->sa_data[0];
	uiMacHi <<= 8;
	uiMacHi |= addr->sa_data[1];
	uiMacHi <<= 8;
	uiMacHi |= addr->sa_data[2];
	uiMacHi <<= 8;
	uiMacHi |= addr->sa_data[3];

	uint16_t uiMacLo = addr->sa_data[4];
	uiMacLo <<= 8;
	uiMacLo |= addr->sa_data[5];

	volatile Unimac *g_pxUnimacSelected = (volatile Unimac *) unimac->base;
	g_pxUnimacSelected->UnimacCore.UnimacMac0 = uiMacHi;
	g_pxUnimacSelected->UnimacCore.UnimacMac1.Reg32 = uiMacLo;

	return 0;
}

static int unimac_open(struct net_device *ndev) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;
	struct sockaddr addr;
	int err;

	for (int i = 0; i < unimac->num_clocks; i++) {
		if (!IS_ERR_OR_NULL(unimac->clock[i])) {
			if (clk_prepare_enable(unimac->clock[i]))
				dev_err(&ndev->dev, "error enabling Unimac clock %d\n", i);
		}
	}

	for (int i = 0; i < unimac->num_resets; i++) {
		if (!IS_ERR_OR_NULL(unimac->reset[i])) {
			if (reset_control_reset(unimac->reset[i]))
				dev_err(&ndev->dev, "error reset Unimac reset %d\n", i);
		}
	}

	msp_init_messages(unimac->msp);
	uint32_t uiInMsgDataPhysicalAddr = msp_in_msg_data_bus_addr(unimac->msp);
	dev_info(dev, "MSP inmsg_data_bus=0x%08X\n", uiInMsgDataPhysicalAddr);

	/* Initialize Unimac Start*/
	volatile Unimac *g_pxUnimacSelected = (volatile Unimac *) unimac->base;
	uint32_t uiLanTxMsgFifo = LtoP((uint32_t)&g_pxUnimacSelected->Mbdma.Lantxmsgfifo01);
	msp_4ke_set_host_mbox_out(unimac->msp, uiLanTxMsgFifo);
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= 0x2000u;// SwReset
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~0x2000u;
	g_pxUnimacSelected->UnimacCore.UnimacFrmLen.Reg32 = 2048;// FrameLength = 2048
	memcpy(addr.sa_data, ndev->dev_addr, ETH_ALEN);
	unimac_set_mac_address(ndev, &addr);
	g_pxUnimacSelected->Mbdma.Bufferbase = fpm_buffer_base_dma(unimac->fpm);
	g_pxUnimacSelected->Mbdma.Buffersize.Reg32 = fpm_buffer_size_code(unimac->fpm);
	g_pxUnimacSelected->Mbdma.Tokenaddress = fpm_alloc_free_bus_addr(unimac->fpm);
	g_pxUnimacSelected->Mbdma.Globalctl.Reg32 = 0x40000081;// LanTxMsgId2w=6d1, LanTxMsgId3w=6d2, AllocLimit=8h40
	g_pxUnimacSelected->Mbdma.Tokencachectl.Reg32 = 0x90309010;// AllocEnable=1b1, AllocMaxBurst=5h10, AllocThresh=8h30, FreeEnable=1, FreeMaxBurst=5h10, FreeThresh=5h10
	dev_info(dev, "UniMAC FPM pool1: bufferbase=0x%08X buffersize=0x%08X tokenaddress=0x%08X\n",
		 g_pxUnimacSelected->Mbdma.Bufferbase,
		 g_pxUnimacSelected->Mbdma.Buffersize.Reg32,
		 g_pxUnimacSelected->Mbdma.Tokenaddress);

	// Rx channel
	g_pxUnimacSelected->Mbdma.Chancontrol00.Reg32 = 0x1000000;// MaxBurst=9h10
	g_pxUnimacSelected->Mbdma.Lanmsgaddress0 = uiInMsgDataPhysicalAddr;

	// Tx channel
	g_pxUnimacSelected->Mbdma.Chancontrol01.Reg32 = 0x1000301;// MaxBurst=9h10, MsgId=6b3; MaxReqs=4h01
	g_pxUnimacSelected->Mbdma.Lanmsgaddress1 = uiInMsgDataPhysicalAddr;

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~UNIMAC_CMD_PROMIS_EN;
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= UNIMAC_CMD_NO_LENGTH_CHECK;
	unimac_set_rx_mode(ndev);
	/* Initialize Unimac End*/

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~CMD_HD_EN;
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= CMD_SPEED_1000 << CMD_SPEED_SHIFT;
	dev_info(dev, "configured fixed 1000/full CPU link\n");

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= 3u;// Enable Rx and Tx
	dev_info(dev, "enabled Rx and Tx\n");

	napi_enable(&unimac->napi);

	err = msp_dqm_host_not_empty_irq_register(unimac->msp, unimac_rx_queue_mask(unimac),
					     unimac_msp_dqm_host_not_empty_irq, unimac);
	if (err) {
		dev_err(dev, "failed to register MSP DQM not-empty RX IRQ callback: %d\n",
			err);
		napi_disable(&unimac->napi);
		return err;
	}
	msp_dqm_host_not_empty_irq_rearm(unimac->msp, unimac_rx_queue_mask(unimac));

	netif_carrier_on(ndev);
	netif_start_queue(ndev);

	return 0;
}

static int unimac_stop(struct net_device *ndev) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	struct device *dev = ndev->dev.parent;

	dev_info(dev, "stopping UniMAC\n");

	netif_stop_queue(ndev);
	msp_dqm_host_not_empty_irq_unregister(unimac->msp);
	napi_disable(&unimac->napi);

	volatile Unimac *g_pxUnimacSelected = (volatile Unimac *) unimac->base;
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~3;// Disable Rx and Tx

	for (int i = 0; i < unimac->num_resets; i++) {
		if (!IS_ERR_OR_NULL(unimac->reset[i])) {
			if (reset_control_assert(unimac->reset[i]))
				dev_err(&ndev->dev, "error asserting Unimac reset %d\n", i);
		}
	}

	for (int i = 0; i < unimac->num_clocks; i++) {
		if (!IS_ERR_OR_NULL(unimac->clock[i])) {
			clk_disable_unprepare(unimac->clock[i]);
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
				     BCM3380_UNIMAC_MAX_FRAME_LEN);
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

	spin_lock(&unimac->fifo_lock);
	// Transmit the packet using vEthernetTx
	ret = vEthernetTx(unimac, length, skb->data, skb->priority);
	spin_unlock(&unimac->fifo_lock);

	if (ret == 1) {
		// Transmission successful
		ndev->stats.tx_packets++;
		ndev->stats.tx_bytes += length;
		dev_kfree_skb(skb);
		return NETDEV_TX_OK;
	} else {
		// Transmission failed
		dev_err(dev, "TX failed\n");
		ndev->stats.tx_errors++;
		return NETDEV_TX_BUSY;
	}
}

static int unimac_change_mtu(struct net_device *ndev, int new_mtu)
{
	if (new_mtu < ETH_MIN_MTU ||
	    new_mtu > BCM3380_UNIMAC_MAX_FRAME_LEN - ETH_HLEN)
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

static int unimac_get_link_ksettings(struct net_device *ndev,
				     struct ethtool_link_ksettings *cmd)
{
	(void)ndev;

	cmd->base.speed = SPEED_1000;
	cmd->base.duplex = DUPLEX_FULL;
	cmd->base.port = PORT_TP;
	cmd->base.autoneg = AUTONEG_DISABLE;

	ethtool_link_ksettings_add_link_mode(cmd, supported, TP);
	ethtool_link_ksettings_add_link_mode(cmd, supported, 1000baseT_Full);
	ethtool_link_ksettings_add_link_mode(cmd, advertising, 1000baseT_Full);

	return 0;
}

static const struct ethtool_ops bcm3380_ethtool_ops = {
	.get_link = ethtool_op_get_link,
	.get_link_ksettings = unimac_get_link_ksettings,
};

static int32_t bcm3380_dqm_poll_rx(struct bcm3380_unimac *unimac,
				   int32_t (*pfOnPacketReady)(void *, const void *, size_t),
				   void *arg)
{
	unsigned int i;
	unsigned int rx_queues[] = {
		unimac->rx_high_queue,
		unimac->rx_normal_queue,
	};

	for (i = 0; i < ARRAY_SIZE(rx_queues); i++) {
		unsigned int queue = rx_queues[i];

		if (i && queue == rx_queues[0])
			continue;

		if (!msp_dqm_queue_not_empty(unimac->msp, queue))
			continue;

		uint32_t token = msp_dqm_read_word(unimac->msp, queue, 0);
		int32_t length = fpm_token_size(token);
		const void *packet = fpm_token_to_virt(unimac->fpm, token);
		if (!packet) {
			dev_err(unimac->ndev->dev.parent,
				"DQM q%u token did not map to a buffer: 0x%08X\n",
				queue, token);
			fpm_free_token(unimac->fpm, token);
			return -4;
		}

		length = pfOnPacketReady(arg, packet, length);
		fpm_free_token(unimac->fpm, token);

#if BCM3380_UNIMAC_DUMP_TRAFFIC
		dev_info(unimac->ndev->dev.parent,
			 "DQM q%u -> CPU: token=0x%08X len=%d not_empty=0x%08X q_sts=0x%08X\n",
			 queue, token, length,
			 msp_dqm_not_empty_status(unimac->msp),
			 msp_dqm_queue_status(unimac->msp, queue));
#endif
		return length;
	}

	return 0;
}

static uint32_t vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn,
			    const void *buffer, u32 priority)
{
	size_t clamped_length = (uiLengthIn < 64) ? 64 : uiLengthIn;
	unsigned int tx_queue = priority > BCM3380_ENET_HIGH_PRIORITY_START ?
				unimac->tx_high_queue :
				unimac->tx_normal_queue;

	if (!msp_dqm_queue_has_space(unimac->msp, tx_queue)) {
		dev_err(unimac->ndev->dev.parent,
			"DQM TX q%u has no space. q_sts=0x%08X\n",
			tx_queue, msp_dqm_queue_status(unimac->msp, tx_queue));
		return 0;
	}

	uint32_t token = fpm_alloc_token(unimac->fpm);
	if (!fpm_token_valid(token)) {
		dev_err(unimac->ndev->dev.parent,
			"got an invalid token from the FPM\n");
		return 0;
	}

	void *dma_dest = fpm_token_to_virt(unimac->fpm, token);
	if (!dma_dest) {
		dev_err(unimac->ndev->dev.parent,
			"TX token did not map to a buffer: 0x%08X\n", token);
		fpm_free_token(unimac->fpm, token);
		return 0;
	}

	// Copy the Ethernet packet data to the DMA buffer
	memcpy(dma_dest, buffer, clamped_length);

	// Update the token with the clamped length's lower 12 bits
	uint32_t adjusted_token = (token & ~BCM3380_FPM_TOKEN_SIZE_MASK) |
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

struct CreateSkbContext {
	struct sk_buff *skb;
	struct napi_struct* napi;
};

static int32_t vCreateSkb(void* arg, const void* pBuffer, size_t uiLength) {
	struct CreateSkbContext *context = (struct CreateSkbContext *)arg;
	struct net_device *ndev = context->napi->dev;
	struct device *dev = ndev->dev.parent;

	context->skb = napi_alloc_skb(context->napi, uiLength-4);
	if (!context->skb) {
		return -ENOMEM;
	}

	void *data = skb_put_data(context->skb, pBuffer, uiLength-4);
	uint32_t fcs_rx;
	memcpy(&fcs_rx, (const void*)(((uint32_t)pBuffer) + uiLength-4), 4);

	if (!data) {
		kfree_skb(context->skb);
		return -ENOMEM;
	}

	uint32_t fcs_calc = crc32_le(~0, data, uiLength-4);
	fcs_calc ^= ~0;
	fcs_calc = __swab32(fcs_calc);
#if BCM3380_UNIMAC_DUMP_TRAFFIC
	dev_info(dev, "RX frame len=%zu fcs_calc=0x%08X fcs_rx=0x%08X\n",
		 uiLength, fcs_calc, fcs_rx);
	print_hex_dump(KERN_INFO, "unimac rx: ", DUMP_PREFIX_NONE, 16, 1,
		       data, min_t(size_t, uiLength - 4, 64), false);
#endif
	if (fcs_calc != fcs_rx) {
		dev_err(dev, "RX FCS mismatch: calc=0x%08X rx=0x%08X len=%zu\n",
			fcs_calc, fcs_rx, uiLength);
		kfree_skb(context->skb);
		context->skb = NULL;
		return -EIO;
	}

	return uiLength-4;
}

static int unimac_poll(struct napi_struct *napi, int budget) {
	struct net_device *ndev = napi->dev;
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	int work_done = 0;

	while (work_done < budget) {
		struct CreateSkbContext context = {
			.skb = NULL,
			.napi = napi,
		};
		spin_lock(&unimac->fifo_lock);
		int32_t outcome = bcm3380_dqm_poll_rx(unimac, vCreateSkb, &context);
		spin_unlock(&unimac->fifo_lock);
		if (outcome == 0) {
			break;
		} else if (outcome < 0) {
			dev_err(ndev->dev.parent, "RX error: %d\n", outcome);
			ndev->stats.rx_dropped++;
			work_done++;
			continue;
		}
		struct sk_buff *skb = context.skb;
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
		netif_receive_skb(skb);
	}

	if (work_done < budget) {
		if (napi_complete_done(napi, work_done))
			msp_dqm_host_not_empty_irq_rearm(unimac->msp, unimac_rx_queue_mask(unimac));
	}

	return work_done;
}

static int unimac_mdio_wait(struct bcm3380_unimac *unimac)
{
	volatile Unimac *regs = (volatile Unimac *)unimac->base;
	int i;

	for (i = 0; i < 50; i++) {
		if (!regs->UnimacInterface.MdioCmd.Bits.StartBusy)
			return 0;
		udelay(2000);
	}

	return -ETIMEDOUT;
}

static int unimac_mdio_read(struct mii_bus *bus, int phy_id, int regnum)
{
	struct bcm3380_unimac *unimac = bus->priv;
	volatile Unimac *regs = (volatile Unimac *)unimac->base;
	UnimacInterfaceMdioCmd cmd = { .Reg32 = 0 };
	int ret;

	if (phy_id < 0 || phy_id >= PHY_MAX_ADDR || regnum < 0 ||
	    regnum >= PHY_MAX_ADDR)
		return -EOPNOTSUPP;

	ret = unimac_mdio_wait(unimac);
	if (ret)
		return ret;

	cmd.Bits.OpCode = 2;
	cmd.Bits.PhyPrtAddr = phy_id & 0x1f;
	cmd.Bits.RegDecAddr = regnum & 0x1f;
	regs->UnimacInterface.MdioCmd.Reg32 = cmd.Reg32;
	cmd.Bits.StartBusy = 1;
	regs->UnimacInterface.MdioCmd.Reg32 = cmd.Reg32;

	ret = unimac_mdio_wait(unimac);
	if (ret)
		return ret;

	cmd.Reg32 = regs->UnimacInterface.MdioCmd.Reg32;
	if (!(bus->phy_ignore_ta_mask & BIT(phy_id)) && cmd.Bits.Fail)
		return -EIO;

	return cmd.Bits.DataAddr;
}

static int unimac_mdio_write(struct mii_bus *bus, int phy_id, int regnum,
			     u16 val)
{
	struct bcm3380_unimac *unimac = bus->priv;
	volatile Unimac *regs = (volatile Unimac *)unimac->base;
	UnimacInterfaceMdioCmd cmd = { .Reg32 = 0 };
	int ret;

	if (phy_id < 0 || phy_id >= PHY_MAX_ADDR || regnum < 0 ||
	    regnum >= PHY_MAX_ADDR)
		return -EOPNOTSUPP;

	ret = unimac_mdio_wait(unimac);
	if (ret)
		return ret;

	cmd.Bits.OpCode = 1;
	cmd.Bits.PhyPrtAddr = phy_id & 0x1f;
	cmd.Bits.RegDecAddr = regnum & 0x1f;
	cmd.Bits.DataAddr = val;
	regs->UnimacInterface.MdioCmd.Reg32 = cmd.Reg32;
	cmd.Bits.StartBusy = 1;
	regs->UnimacInterface.MdioCmd.Reg32 = cmd.Reg32;

	return unimac_mdio_wait(unimac);
}

static void unimac_mdio_log_phy_status(struct bcm3380_unimac *unimac,
				       unsigned int phy_prt_addr)
{
	struct device *dev = unimac->ndev->dev.parent;
	int bmcr;
	int bmsr_first;
	int bmsr;
	int physid1;
	int physid2;
	int ctrl1000;
	int stat1000;

	bmcr = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_BMCR);
	bmsr_first = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_BMSR);
	bmsr = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_BMSR);
	physid1 = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_PHYSID1);
	physid2 = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_PHYSID2);
	ctrl1000 = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_CTRL1000);
	stat1000 = unimac_mdio_read(unimac->mii_bus, phy_prt_addr, MII_STAT1000);

	if (bmcr < 0 || bmsr_first < 0 || bmsr < 0 ||
	    physid1 < 0 || physid2 < 0) {
		dev_info(dev,
			 "internal PHY%u MDIO read failed: BMCR=%d BMSR1=%d BMSR2=%d PHYSID1=%d PHYSID2=%d CTRL1000=%d STAT1000=%d\n",
			 phy_prt_addr, bmcr, bmsr_first, bmsr, physid1,
			 physid2, ctrl1000, stat1000);
		return;
	}

	dev_info(dev,
		 "internal PHY%u MDIO: BMCR=0x%04X BMSR1=0x%04X BMSR2=0x%04X PHYSID=0x%04X%04X CTRL1000=0x%04X STAT1000=0x%04X link=%s autoneg=%s power=%s isolate=%s\n",
		 phy_prt_addr, bmcr & 0xffff, bmsr_first & 0xffff,
		 bmsr & 0xffff, physid1 & 0xffff, physid2 & 0xffff,
		 ctrl1000 < 0 ? 0xffff : ctrl1000 & 0xffff,
		 stat1000 < 0 ? 0xffff : stat1000 & 0xffff,
		 bmsr & BMSR_LSTATUS ? "up" : "down",
		 bmsr & BMSR_ANEGCOMPLETE ? "complete" : "not-complete",
		 bmcr & BMCR_PDOWN ? "down" : "up",
		 bmcr & BMCR_ISOLATE ? "yes" : "no");
}

static int unimac_mdio_init(struct bcm3380_unimac *unimac,
			    struct device_node *node)
{
	struct device *dev = unimac->ndev->dev.parent;
	struct device_node *mdio_np;
	struct mii_bus *mii_bus;
	u32 phy_prt_addr = 0;
	int ret;

	mdio_np = of_get_child_by_name(node, "mdio");
	if (!mdio_np)
		return 0;

	if (!of_device_is_available(mdio_np)) {
		of_node_put(mdio_np);
		return 0;
	}

	ret = of_property_read_u32(mdio_np, "brcm,phy-prt-addr",
				   &phy_prt_addr);
	if (ret) {
		dev_err(dev, "missing brcm,phy-prt-addr in MDIO node\n");
		of_node_put(mdio_np);
		return ret;
	}

	if (phy_prt_addr >= PHY_MAX_ADDR) {
		dev_err(dev, "invalid MDIO PHY address %u\n", phy_prt_addr);
		of_node_put(mdio_np);
		return -EINVAL;
	}

	mii_bus = devm_mdiobus_alloc(dev);
	if (!mii_bus) {
		of_node_put(mdio_np);
		return -ENOMEM;
	}

	mii_bus->priv = unimac;
	mii_bus->name = "bcm3380-unimac MII bus";
	snprintf(mii_bus->id, MII_BUS_ID_SIZE, "%s-mii", dev_name(dev));
	mii_bus->parent = dev;
	mii_bus->read = unimac_mdio_read;
	mii_bus->write = unimac_mdio_write;
	mii_bus->phy_mask = ~BIT(phy_prt_addr);

	ret = devm_of_mdiobus_register(dev, mii_bus, mdio_np);
	of_node_put(mdio_np);
	if (ret) {
		dev_warn(dev, "MDIO bus registration failed: %d\n", ret);
		return 0;
	}

	unimac->mii_bus = mii_bus;
	dev_info(dev, "registered UniMAC MDIO bus with PHY address %u\n",
		 phy_prt_addr);
	unimac_mdio_log_phy_status(unimac, phy_prt_addr);

	return 0;
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

	priv->num_clocks = of_clk_get_parent_count(node);
	if (priv->num_clocks) {
		priv->clock = devm_kcalloc(dev, priv->num_clocks,
						sizeof(struct clk *), GFP_KERNEL);
		if (!priv->clock)
			return -ENOMEM;
	}
	for (int i = 0; i < priv->num_clocks; i++) {
		priv->clock[i] = of_clk_get(node, i);
		if (IS_ERR_OR_NULL(priv->clock[i])) {
			dev_err(dev, "error getting Unimac clock %d\n", i);
			return PTR_ERR(priv->clock[i]);
		}

		err = clk_prepare_enable(priv->clock[i]);
		if (err) {
			dev_err(dev, "error enabling Unimac clock %d\n", i);
			return err;
		}
	}
	priv->num_resets = of_count_phandle_with_args(node, "resets",
						"#reset-cells");
	if (priv->num_resets <= 0)
		priv->num_resets = 0;
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

		err = reset_control_reset(priv->reset[i]);
		if (err) {
			dev_err(dev, "error performing Unimac reset %d\n", i);
			return err;
		}
	}

	/* Get MAC address from device tree */
	uint8_t aucDtsMac[6];
	of_get_mac_address(pdev->dev.of_node, aucDtsMac);
	if (is_valid_ether_addr(aucDtsMac)) {
		dev_addr_set(ndev, aucDtsMac);
		dev_info(dev, "Using MAC from DTS: %pM\n", aucDtsMac);
	} else {
		eth_hw_addr_random(ndev);
		dev_info(dev, "Using random MAC address\n");
	}

	err = fpm_get(dev, &priv->fpm);
	if (err) {
		dev_err_probe(dev, err, "failed to get FPM provider\n");
		goto err_free_netdev;
	}
	dev_info(dev, "Using FPM pool1\n");

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

	spin_lock_init(&priv->fifo_lock);

	err = unimac_mdio_init(priv, node);
	if (err)
		goto err_put_msp;

	/* Set up network device */
	ndev->netdev_ops = &bcm3380_netdev_ops;
	ndev->ethtool_ops = &bcm3380_ethtool_ops;
	ndev->min_mtu = ETH_MIN_MTU;
	ndev->max_mtu = BCM3380_UNIMAC_MAX_FRAME_LEN - ETH_HLEN;
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
	fpm_put(priv->fpm);
err_free_netdev:
	return err;
}

/* Remove function */
static void unimac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct bcm3380_unimac *priv = netdev_priv(ndev);

	msp_put(priv->msp);
	fpm_put(priv->fpm);
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
