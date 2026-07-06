#include <linux/clk.h>
#include <linux/atomic.h>
#include <linux/bits.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/of_clk.h>
#include <linux/of_net.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/reset.h>
#include <linux/mii.h>
#include <linux/if_ether.h>
#include "unimac.h"

#include <linux/types.h>
#include <linux/ip.h>
#include <net/checksum.h>  // For csum_partial and csum_fold
#include <linux/icmp.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#include <bcm3380/unimac.h>
#include <soc/bcm/bcm3380-fpm.h>
#include <soc/bcm/bcm3380-msp.h>

#define BCM3380_UNIMAC_DBG 1
#define BCM3380_UNIMAC_TEST 0
#define BCM3380_UNIMAC_DUMP_TRAFFIC 0

#if BCM3380_UNIMAC_DBG
#define UNIMAC_DBG(fmt, ...) \
	printk(KERN_INFO "%s: " fmt, __func__, ##__VA_ARGS__)
#define vDumpMemory(ptr, length) print_hex_dump(KERN_INFO, "", DUMP_PREFIX_NONE, 16, 1, ptr, length, false)
#else
#define UNIMAC_DBG(fmt, ...) \
	do { } while (0)
#endif

typedef int BOOL;

#define BCM3380_ENET_HIGH_PRIORITY_START 3

// macro to convert logical data addresses to physical
// DMA hardware must see physical address
#define LtoP( logicalAddr ) ( ((uint32_t)(logicalAddr)) & 0x1FFFFFFF )

/* Private driver data structure */
struct bcm3380_unimac {
	struct net_device *ndev;
	void __iomem *base;

	int uiLinkModeIndex; // dword_83F8A814
	int u5PhyPrtAddr;

	struct bcm3380_fpm *fpm;
	struct bcm3380_msp *msp;

	// DQM queue IDs for RX and TX
	unsigned int rx_normal_queue;
	unsigned int rx_high_queue;
	unsigned int tx_high_queue;
	unsigned int tx_normal_queue;

	spinlock_t fifo_lock;
#if BCM3380_UNIMAC_TEST
	atomic_t test_rx_irq_pending;
#endif

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

	struct napi_struct napi;
};

static inline u32 unimac_rx_queue_mask(struct bcm3380_unimac *unimac)
{
	return BIT(unimac->rx_normal_queue) | BIT(unimac->rx_high_queue);
}

static uint32_t vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn,
			    const void *buffer, u32 priority);
BOOL bLinkUp(struct bcm3380_unimac *unimac);
static void vMdioWrite(volatile Unimac *g_pxUnimacSelected, uint32_t u5PhyPrtAddr, uint32_t u5RegDecAddr, uint16_t usDataAddr);
static uint16_t usMdioRead(volatile Unimac *g_pxUnimacSelected, int u5PhyPrtAddr, int u5RegDecAddr);

#if BCM3380_UNIMAC_TEST
static void unimac_msp_dqm_host_not_empty_irq(void *data, u32 pending_queues)
{
	struct bcm3380_unimac *unimac = data;

	atomic_or(pending_queues, &unimac->test_rx_irq_pending);
}
#else
static void unimac_msp_dqm_host_not_empty_irq(void *data, u32 pending_queues)
{
	struct bcm3380_unimac *unimac = data;

	if (napi_schedule_prep(&unimac->napi))
		__napi_schedule(&unimac->napi);
}
#endif

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

	unimac->uiLinkModeIndex = 0;
	unimac->u5PhyPrtAddr = 0;
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

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~0x1000010u;// Clear PromisEn and NoLgthCheck
	/* Initialize Unimac End*/

	if ( !bLinkUp(unimac) ) {
		UNIMAC_DBG("!bLinkUp\n");
		uint16_t v0 = usMdioRead(g_pxUnimacSelected, unimac->u5PhyPrtAddr, 4);
		vMdioWrite(g_pxUnimacSelected, unimac->u5PhyPrtAddr, 4u, v0 | 0xE0);
		v0 = usMdioRead(g_pxUnimacSelected, unimac->u5PhyPrtAddr, 0);
		vMdioWrite(g_pxUnimacSelected, unimac->u5PhyPrtAddr, 0, v0 | 0x200);
		UNIMAC_DBG("Waiting for link up...\n");
	}

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= 3u;// Enable Rx and Tx
	UNIMAC_DBG("Enabled Rx and Tx\n");

	while (!bLinkUp(unimac))
		mdelay(1000u);
	UNIMAC_DBG("bLinkUp!!!!!!!\n");

#if BCM3380_UNIMAC_TEST
	atomic_set(&unimac->test_rx_irq_pending, 0);
	err = msp_dqm_host_not_empty_irq_register(unimac->msp, unimac_rx_queue_mask(unimac),
					     unimac_msp_dqm_host_not_empty_irq, unimac);
	if (err) {
		dev_err(dev, "failed to register MSP DQM not-empty RX IRQ callback: %d\n",
			err);
		return err;
	}
	msp_dqm_host_not_empty_irq_rearm(unimac->msp, unimac_rx_queue_mask(unimac));
#else
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
#endif // #if BCM3380_UNIMAC_TEST

	return 0;
}

static int unimac_stop(struct net_device *ndev) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);

	UNIMAC_DBG("Linux wants to stop Unimac\n");

	netif_stop_queue(ndev);
	msp_dqm_host_not_empty_irq_unregister(unimac->msp);
#if !BCM3380_UNIMAC_TEST
	napi_disable(&unimac->napi);
#endif

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
	size_t length = skb->len;
	int ret;

	// Ensure the packet length is within the allowed MTU
	if (length > ndev->mtu) {
		UNIMAC_DBG("length(%d) > ndev->mtu(%d)\n", length, ndev->mtu);
		dev_kfree_skb(skb);
		ndev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

#if BCM3380_UNIMAC_DUMP_TRAFFIC
	UNIMAC_DBG("Linux wants to send %d bytes\n", length);
	vDumpMemory(skb->data, length > 16 ? 16 : length);
#endif // #if BCM3380_UNIMAC_DUMP_TRAFFIC

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
		UNIMAC_DBG("Transmission failed\n");
		ndev->stats.tx_errors++;
		return NETDEV_TX_BUSY;
	}
}

/* Network device operations */
static const struct net_device_ops bcm3380_netdev_ops = {
	.ndo_open = unimac_open,
	.ndo_stop = unimac_stop,
	.ndo_start_xmit = unimac_start_xmit,
	.ndo_set_mac_address = unimac_set_mac_address,
};

static uint16_t usMdioRead(volatile Unimac *g_pxUnimacSelected, int u5PhyPrtAddr, int u5RegDecAddr) {
	g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32 = (u5PhyPrtAddr << 21) | (u5RegDecAddr << 16) | 0x28000000;// Set StartBusy, OpCode=2b10
	while ( (g_pxUnimacSelected->UnimacInterface.MdioCfg.Reg32 & 0x100) != 0 );// while (MdioBusy);
	uint16_t val = g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32;
	UNIMAC_DBG("[u5PhyPrtAddr=%d, ui5RegDecAddr=%d]-->0x%04X\n", u5PhyPrtAddr, u5RegDecAddr, val);
	return val;
}

static void vMdioWrite(volatile Unimac *g_pxUnimacSelected, uint32_t u5PhyPrtAddr, uint32_t u5RegDecAddr, uint16_t usDataAddr) {
	g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32 = (u5PhyPrtAddr << 21) | (u5RegDecAddr << 16) | usDataAddr | 0x24000000;// Set StartBusy, opcode=2b01
	while ( (g_pxUnimacSelected->UnimacInterface.MdioCfg.Reg32 & 0x100) != 0 );// MdioBusy
	UNIMAC_DBG("[u5PhyPrtAddr=%d, ui5RegDecAddr=%d]<--0x%04X\n", u5PhyPrtAddr, u5RegDecAddr, usDataAddr);
}

struct UnimacLinkMode {
	uint8_t ucEthSpeed; // 0=10M, 1=100M, 2=1G
	uint8_t bHdEna; // 1 = Half-Duplex
	const char *c_acName;
};

static struct UnimacLinkMode g_axUnimacLinkModes[8] = {
	{CMD_SPEED_10, 1, "incomplete"},
	{CMD_SPEED_10, 1, "10M half"},
	{CMD_SPEED_10, 0, "10M full"},
	{CMD_SPEED_100, 1, "100M half"},
	{CMD_SPEED_100, 0, "100M T4"},
	{CMD_SPEED_100, 0, "100M full"},
	{CMD_SPEED_1000, 1, "1G half"},
	{CMD_SPEED_1000, 0, "1G full"},
};

BOOL bLinkUp(struct bcm3380_unimac *unimac) {
	volatile Unimac *g_pxUnimacSelected = (volatile Unimac *)unimac->base;

	// Check AUTO_NEGOTIATION_COMPLETE in the status register
	if (usMdioRead(g_pxUnimacSelected, unimac->u5PhyPrtAddr, MII_BMSR) & BMSR_ANEGCOMPLETE) {
		uint32_t uiLinkModeIndex = (usMdioRead(g_pxUnimacSelected, unimac->u5PhyPrtAddr, 25) >> 8) & 7;
		if ( uiLinkModeIndex != unimac->uiLinkModeIndex ) {
			unimac->uiLinkModeIndex = uiLinkModeIndex;
			struct UnimacLinkMode* pxLinkMode = &g_axUnimacLinkModes[uiLinkModeIndex];
			UNIMAC_DBG("Link up: %s\n", pxLinkMode->c_acName);

			// Update link speed and duplex mode
			uint32_t uiUnimacCmd = g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32;
			if ( pxLinkMode->bHdEna )
				uiUnimacCmd |= CMD_HD_EN;
			else
				uiUnimacCmd &= ~CMD_HD_EN;

			uiUnimacCmd &= ~(CMD_SPEED_MASK << CMD_SPEED_SHIFT);
			uiUnimacCmd |= (pxLinkMode->ucEthSpeed << CMD_SPEED_SHIFT);
			g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 = uiUnimacCmd;
		}
		//PeriphBlockCached.Led.LedMode0.Reg32 = 0x300;// Led4Mode = 2b11
		return 1;
	}

	//PeriphBlockCached.Led.LedMode0.Reg32 = 0;
	return 0;
}

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
			UNIMAC_DBG("DQM q%u token did not map to a buffer: 0x%08X\n",
				   queue, token);
			fpm_free_token(unimac->fpm, token);
			return -4;
		}

		length = pfOnPacketReady(arg, packet, length);
		fpm_free_token(unimac->fpm, token);

		UNIMAC_DBG("DQM q%u -> CPU: token=0x%08X len=%d not_empty=0x%08X q_sts=0x%08X\n",
			   queue, token, length,
			   msp_dqm_not_empty_status(unimac->msp),
			   msp_dqm_queue_status(unimac->msp, queue));
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
		UNIMAC_DBG("Error: DQM TX q%u has no space. q_sts=0x%08X\n",
			   tx_queue, msp_dqm_queue_status(unimac->msp, tx_queue));
		return 0;
	}

	uint32_t token = fpm_alloc_token(unimac->fpm);
	if (!fpm_token_valid(token)) {
		UNIMAC_DBG("Error: Got an invalid token from the FPM!\n");
		return 0;
	}

	void *dma_dest = fpm_token_to_virt(unimac->fpm, token);
	if (!dma_dest) {
		UNIMAC_DBG("Error: TX token did not map to a buffer: %08x\n", token);
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

	UNIMAC_DBG("DQM q%u <- TX token=0x%08X len=%zu priority=%u q_sts=0x%08X\n",
		   tx_queue, adjusted_token, clamped_length, priority,
		   msp_dqm_queue_status(unimac->msp, tx_queue));
	return 1;
}

#if BCM3380_UNIMAC_TEST

struct __attribute__((packed)) EthernetHeader {// sizeof=0xE
	uint16_t ausDstMac[3];
	uint16_t ausSrcMac[3];
	uint16_t usType;
};

struct __attribute__((packed)) EthernetPacket { // sizeof=0x2A
	struct EthernetHeader xEth;
	union {
		struct {
			struct iphdr header;
		} ipv4;
	} payload;
};

enum NetworkConstants { // 4 bytes
	UDP_HEADER_LEN      = 0x8,
	UDP_PROTOCOL_IPV4   = 0x11,
	TCP_HEADER_LEN      = 0x14,
	ETHERNET_HEADER_LEN = 0xE,
	ETHERNET_IPV4       = 0x800,
	ETHERNET_ARP        = 0x806,
	ARP_HEADER_LEN      = 0x1C,
};

static int verify_ip_checksum(struct iphdr *ip_header) {
	// Save the original checksum
	__sum16 original_checksum = ip_header->check;
	UNIMAC_DBG("original_checksum: 0x%04X\n", original_checksum);

	// Set the checksum field to zero before calculation
	ip_header->check = 0;

	// Calculate the checksum
	__sum16 calculated_checksum = ip_fast_csum((u8 *)ip_header, ip_header->ihl);
	UNIMAC_DBG("calculated_checksum: 0x%04X\n", calculated_checksum);

	// Restore the original checksum
	ip_header->check = original_checksum;

	// Check if the calculated checksum matches the original
	return (calculated_checksum == original_checksum);
}

static int32_t vUnimacDemoRx(void* arg, const void* pBuffer, size_t uiLength) {
	memcpy(arg, pBuffer, uiLength);
	return uiLength;
}

static uint32_t rx_i = 0;
static uint32_t rx_len = 0;

static void vUnimacDemo(struct bcm3380_unimac *unimac) {
	unimac_open(unimac->ndev);

	int32_t pollResult = 0;
	uint32_t idle_loops = 0;
	void* buffer = kzalloc(0x1000, GFP_KERNEL);
	if (!buffer) {
		UNIMAC_DBG("failed to allocate vUnimacDemo RX buffer\n");
		while (1)
			mdelay(1000);
	}
	do {
		if (!atomic_read(&unimac->test_rx_irq_pending)) {
			if ((idle_loops++ & 0x3FF) == 0) {
				UNIMAC_DBG("RX waiting for IRQ: InMsgSts=0x%08X DqmNotEmptySts=0x%08X rx_normal_q%u_sts=0x%08X rx_high_q%u_sts=0x%08X HostMboxIn=0x%08X CoreStatus=0x%08X\n",
					   msp_in_msg_status(unimac->msp),
					   msp_dqm_not_empty_status(unimac->msp),
					   unimac->rx_normal_queue,
					   msp_dqm_queue_status(unimac->msp, unimac->rx_normal_queue),
					   unimac->rx_high_queue,
					   msp_dqm_queue_status(unimac->msp, unimac->rx_high_queue),
					   msp_4ke_host_mbox_in(unimac->msp),
					   msp_4ke_core_status(unimac->msp));
			}
			mdelay(1);
			continue;
		}

		pollResult = bcm3380_dqm_poll_rx(unimac, vUnimacDemoRx, buffer);
		if (pollResult == 0) {
			atomic_set(&unimac->test_rx_irq_pending, 0);
			msp_dqm_host_not_empty_irq_rearm(unimac->msp, unimac_rx_queue_mask(unimac));
			if ((idle_loops++ & 0x3FF) == 0) {
				UNIMAC_DBG("RX idle: InMsgSts=0x%08X DqmNotEmptySts=0x%08X rx_normal_q%u_sts=0x%08X rx_high_q%u_sts=0x%08X HostMboxIn=0x%08X CoreStatus=0x%08X\n",
					   msp_in_msg_status(unimac->msp),
					   msp_dqm_not_empty_status(unimac->msp),
					   unimac->rx_normal_queue,
					   msp_dqm_queue_status(unimac->msp, unimac->rx_normal_queue),
					   unimac->rx_high_queue,
					   msp_dqm_queue_status(unimac->msp, unimac->rx_high_queue),
					   msp_4ke_host_mbox_in(unimac->msp),
					   msp_4ke_core_status(unimac->msp));
			}
			mdelay(1);
		}
		if (pollResult > 0) {
			uint32_t uiLength = pollResult;
			idle_loops = 0;

			UNIMAC_DBG("Ethernet Rx Good, len = 0x%08X\n", uiLength);
			vDumpMemory(buffer, uiLength);

			uint32_t fcs = crc32_le(~0, buffer, uiLength - 4);
			fcs ^= ~0;
			fcs = __swab32(fcs);
			UNIMAC_DBG("FCS = 0x%08X", fcs); // Need to swap endianess
			uint32_t fcs_rx = *((uint32_t*)((uint32_t)buffer + uiLength - 4));
			UNIMAC_DBG("FCS_CALC = 0x%08X, FCS_RX = 0x%08X\n", fcs, fcs_rx);

			if (fcs != fcs_rx) {
				UNIMAC_DBG("FCS mismatch!!!! rx_i = %d, rx_len = 0x%08X\n", rx_i, rx_len);
				UNIMAC_DBG("InMsgData = 0x%08X\n", msp_in_msg_read(unimac->msp));
				while(1);
			}
			rx_i++;
			rx_len += uiLength;

			struct EthernetPacket* packet = (struct EthernetPacket*) buffer;
			UNIMAC_DBG("DstMac: %04X %04X %04X\n", packet->xEth.ausDstMac[0], packet->xEth.ausDstMac[1], packet->xEth.ausDstMac[2]);
			UNIMAC_DBG("SrcMac: %04X %04X %04X\n", packet->xEth.ausSrcMac[0], packet->xEth.ausSrcMac[1], packet->xEth.ausSrcMac[2]);
			UNIMAC_DBG("Type: 0x%04X\n", packet->xEth.usType);
			if (ETHERNET_IPV4 == packet->xEth.usType) {
				struct iphdr* header = &packet->payload.ipv4.header;
				int headerLen = 4 * header->ihl;
				UNIMAC_DBG("IPv4 headerLen = %d\n", headerLen);
				UNIMAC_DBG("IPv4 tot_len = %d\n", header->tot_len);
				UNIMAC_DBG("IPv4 protocol = %d\n", header->protocol);
				UNIMAC_DBG("IPv4 ucChecksum = 0x%04X\n", header->check);
				UNIMAC_DBG("IPv4 uiSrcIp = %08X\n", header->saddr);
				UNIMAC_DBG("IPv4 uiDstIp = %08X\n", header->daddr);
				if (!verify_ip_checksum((struct iphdr*)header)) {
					UNIMAC_DBG("Bad IPv4 header checksum\n");
					continue;
				}

				struct icmphdr* icmp = (struct icmphdr*)(header+1);
				UNIMAC_DBG("Received ICMP type = %d\n", icmp->type);
				if (icmp->type != 8) {
					continue;
				}

				uint32_t icmp_len = header->tot_len - headerLen;
				UNIMAC_DBG("icmp_len = 0x%04X\n", icmp_len);
				UNIMAC_DBG("pingCheckSum = 0x%04X\n", icmp->checksum);

				// Generate echo reply and calc checksum
				icmp->type = 0;
				icmp->checksum = 0;
				icmp->checksum = csum_fold(csum_partial(icmp, icmp_len, 0));
				UNIMAC_DBG("New pong checksum 0x%04X\n", icmp->checksum);

				// Swap IP and calc checksum
				uint32_t temp_ip = header->saddr;
				header->saddr = header->daddr;
				header->daddr = temp_ip;
				header->check = 0;
				header->check = ip_fast_csum((u8 *)header, header->ihl);
				UNIMAC_DBG("New IPv4 header checksum 0x%04X\n", header->check);

				// Swap Mac
				uint16_t mac[3];
				memcpy(mac, packet->xEth.ausDstMac, 6);
				memcpy(packet->xEth.ausDstMac, packet->xEth.ausSrcMac, 6);
				memcpy(packet->xEth.ausSrcMac, mac, 6);

				UNIMAC_DBG("Txlen = 0x%08X\n", header->tot_len + ETHERNET_HEADER_LEN);

				vDumpMemory(buffer, uiLength);
				vEthernetTx(unimac, uiLength, buffer, 0);
			}
		}
	} while(1+1);
}
#endif // #if BCM3380_UNIMAC_TEST

struct CreateSkbContext {
	struct sk_buff *skb;
	struct napi_struct* napi;
};

static int32_t vCreateSkb(void* arg, const void* pBuffer, size_t uiLength) {
	struct CreateSkbContext *context = (struct CreateSkbContext *)arg;

	context->skb = napi_alloc_skb(context->napi, uiLength-4);
	if (!context->skb) {
		return -114514;
	}

	void *data = skb_put_data(context->skb, pBuffer, uiLength-4);
	uint32_t fcs_rx;
	memcpy(&fcs_rx, (const void*)(((uint32_t)pBuffer) + uiLength-4), 4);

	if (!data) {
		kfree_skb(context->skb);
		return -114514;
	}

	uint32_t fcs_calc = crc32_le(~0, data, uiLength-4);
	fcs_calc ^= ~0;
	fcs_calc = __swab32(fcs_calc);
#if BCM3380_UNIMAC_DUMP_TRAFFIC
	UNIMAC_DBG("FCS_CALC = 0x%08X, FCS_RX = 0x%08X\n", fcs_calc, fcs_rx);

	UNIMAC_DBG("Got %d bytes\n", uiLength);
	vDumpMemory(data, uiLength - 4);
#endif // #if BCM3380_UNIMAC_DUMP_TRAFFIC
	if (fcs_calc != fcs_rx) {
		UNIMAC_DBG("FCS mismatch!!!!\n");
		while(1);
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
			UNIMAC_DBG("Rx Err %d!!!\n", outcome);
			ndev->stats.rx_dropped++;
			work_done++;
			continue;
		}
		struct sk_buff *skb = context.skb;
		size_t length = outcome;
		skb->protocol = eth_type_trans(skb, ndev);
#if BCM3380_UNIMAC_DUMP_TRAFFIC
		UNIMAC_DBG("Received %d bytes\n", length);
		vDumpMemory(skb_mac_header(skb), length);
#endif // #if BCM3380_UNIMAC_DUMP_TRAFFIC

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

/* Probe function - called when device is discovered */
static int bcm3380_probe(struct platform_device *pdev)
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
	UNIMAC_DBG("base=0x%08X\n", (uint32_t) priv->base);
	if (IS_ERR(priv->base)) {
		err = PTR_ERR(priv->base);
		goto err_free_netdev;
	}

	UNIMAC_DBG("IntControlClkControlLo = 0x%08X\n", *((uint32_t*)0xB4e00004));
	UNIMAC_DBG("IntControlClkControlHi = 0x%08X\n", *((uint32_t*)0xB4e00008));
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
	UNIMAC_DBG("IntControlClkControlLo = 0x%08X\n", *((uint32_t*)0xB4e00004));
	UNIMAC_DBG("IntControlClkControlHi = 0x%08X\n", *((uint32_t*)0xB4e00008));

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
#if BCM3380_UNIMAC_TEST
	atomic_set(&priv->test_rx_irq_pending, 0);
#endif

#if BCM3380_UNIMAC_TEST
	vUnimacDemo(priv);
#endif

	/* Set up network device */
	ndev->netdev_ops = &bcm3380_netdev_ops;
	netif_napi_add(ndev, &priv->napi, unimac_poll);

	// ndev->ethtool_ops = &bcm3380_ethtool_ops; /* If implementing ethtool */

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
static void bcm3380_remove(struct platform_device *pdev)
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
	.probe = bcm3380_probe,
	.remove = bcm3380_remove,
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
