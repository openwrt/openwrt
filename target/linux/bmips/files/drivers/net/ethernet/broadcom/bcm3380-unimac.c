#include <linux/clk.h>
#include <linux/bits.h>
#include <linux/dma-mapping.h>
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

#include <linux/timer.h>
#include <linux/jiffies.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#include <bcm3380/unimac.h>
#include <soc/bcm/bcm3380-fpm.h>
#include <bcm3380/ioproc_blockdef.h>
#include <bcm3380/IntControl.h>

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
#define IOPROC_UNCACHED (*((volatile IoprocBlockIoProc*)0xB5800000))
#define IOPROC_SMISB (*((volatile IoprocBlockIoProc*)0xB8800000))
#define INT_CTRL (*((volatile IntControlRegs*)0xB4E00000))

#define MIPS_SMISB_CTRL 0xFF400030

#define POLL_INTERVAL (msecs_to_jiffies(1)) // Poll every 1 milliseconds

#define BCM3380_DQM_RX_Q_NORMAL 0
#define BCM3380_DQM_RX_Q_HIGH 3
#define BCM3380_DQM_RX_QUEUE_WORDS 0x80
#define BCM3380_DQM_TEST_MEM_WORDS 0x1000
#define BCM3380_DQM_Q_TOKEN_WORDS 1
#define BCM3380_DQM_Q_AVAIL_MASK 0x3FFF
#define BCM3380_UNIMAC_TEST_HOST_BRIDGE 0
#define BCM3380_IOP4KE_FW_MEM_SIZE 0x800
#define BCM3380_IOP4KE_RESET_VECTOR_PHYS 0x1FC00000
#define BCM3380_IOP4KE_WINDOW_MASK_2K 0xFFFFF800
#define BCM3380_IOP4KE_ALIVE_MAGIC 0x4B454F4B
#define BCM3380_IOP4KE_SOFT_RESET_M4KE 0x00000001

// macro to convert logical data addresses to physical
// DMA hardware must see physical address
#define LtoP( logicalAddr ) ( ((uint32_t)(logicalAddr)) & 0x1FFFFFFF )
#define PtoL( x ) ( LtoP(x) | 0xa0000000 )

/* Private driver data structure */
struct bcm3380_unimac {
	struct net_device *ndev;
	void __iomem *base;
	int irq;

	int uiLinkModeIndex; // dword_83F8A814
	int u5PhyPrtAddr;

	struct bcm3380_fpm *fpm;

	// MSP IOPROC
	uint32_t __iomem* puiInMsgSts; // ioproc.h IoprocIoprocInMsgSts
#define IOPROC_IN_FIFO_NOT_EMPTY(puiInMsgSts) ((puiInMsgSts & 0x80000000) ? 1 : 0)
	uint32_t __iomem* puiInMsgData;
	uint32_t __iomem* puiOgMsgSts; // ioproc.h IoprocIoprocOgMsgSts
#define IOPROC_OG_GET_FIFO_VACANCY(puiOgMsgSts) (puiOgMsgSts & 0x1F)

	uint32_t uiLanTxMsgFifo;

	spinlock_t fifo_lock;
	struct timer_list poll_timer;

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

#if BCM3380_UNIMAC_TEST
	void *pIop4keFwMem;
	dma_addr_t pIop4keFwMemPhysical;
	size_t uiIop4keFwMemSize;
#endif

	struct napi_struct napi;
};

char byte_83F8A818 = 0;

static uint32_t vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn, const void *buffer);
BOOL bLinkUp(struct bcm3380_unimac *unimac);
static void vMdioWrite(volatile Unimac *g_pxUnimacSelected, uint32_t u5PhyPrtAddr, uint32_t u5RegDecAddr, uint16_t usDataAddr);
static uint16_t usMdioRead(volatile Unimac *g_pxUnimacSelected, int u5PhyPrtAddr, int u5RegDecAddr);

#if !BCM3380_UNIMAC_TEST
// Timer callback function
static void poll_timer_callback(struct timer_list *t) {
	struct bcm3380_unimac *unimac = from_timer(unimac, t, poll_timer);

	// Schedule NAPI poll
	napi_schedule(&unimac->napi);
	// Rearm the timer
	mod_timer(&unimac->poll_timer, jiffies + POLL_INTERVAL);

	// struct net_device *ndev = unimac->napi.dev;
	// struct device *dev = ndev->dev.parent;
	// dev_info(dev, "&INT_CTRL.Iopirqmask0 = 0x%08X\n", (uint32_t)&INT_CTRL.Iopirqmask0.Reg32);
	// dev_info(dev, "INT_CTRL.Iopirqmask0 = 0x%08X\n", INT_CTRL.Iopirqmask0.Reg32);
	// dev_info(dev, "INT_CTRL.Iopirqstatus0 = 0x%08X\n", INT_CTRL.Iopirqstatus0.Reg32);
	// dev_info(dev, "&INT_CTRL.Iopirqmask1 = 0x%08X\n", (uint32_t)&INT_CTRL.Iopirqmask1.Reg32);
	// dev_info(dev, "INT_CTRL.Iopirqmask1 = 0x%08X\n", INT_CTRL.Iopirqmask1.Reg32);
	// dev_info(dev, "INT_CTRL.Iopirqstatus1 = 0x%08X\n", INT_CTRL.Iopirqstatus1.Reg32);
	// dev_info(dev, "INT_CTRL.IopirqSense = 0x%08X\n", INT_CTRL.IopirqSense.Reg32);
	// dev_info(dev, "INT_CTRL.PeriphIrqSense = 0x%08X\n", INT_CTRL.PeriphIrqSense.Reg32);

	// dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L1Irq4keMask = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L1Irq4keMask.Reg32);
	// dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L1Irq4keStatus = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L1Irq4keStatus.Reg32);
	// dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L1IrqMipsMask = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L1IrqMipsMask.Reg32);
	// dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L1IrqMipsStatus = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L1IrqMipsStatus.Reg32);
	// dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L2IrqGpMsk = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L2IrqGpMsk.Reg32);
	// dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L2IrqGpSts = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L2IrqGpSts.Reg32);

	// dev_info(dev, "IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl = 0x%08X\n", IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32);
	//netdev_info(unimac->ndev, "IOPROC_SMISB.In.IncomingMessageFifo.InMsgSts = 0x%08X\n", IOPROC_SMISB.In.IncomingMessageFifo.InMsgSts.Reg32);
}
#endif // #if !BCM3380_UNIMAC_TEST

static irqreturn_t unimac_isr_msp(int irq, void *dev_id) {
	struct net_device *ndev = dev_id;
	struct device *dev = ndev->dev.parent;
	struct bcm3380_unimac *unimac = netdev_priv(ndev);
	dev_info(dev, "unimac_isr_msp InMsgSts = 0x%08X\n", readl_be(unimac->puiInMsgSts));
	dev_info(dev, "INT_CTRL.Iopirqstatus0 = 0x%08X\n", INT_CTRL.Iopirqstatus0.Reg32);
	dev_info(dev, "INT_CTRL.Iopirqstatus1 = 0x%08X\n", INT_CTRL.Iopirqstatus1.Reg32);
	dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L1Irq4keStatus = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L1Irq4keStatus.Reg32);
	dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L1IrqMipsStatus = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L1IrqMipsStatus.Reg32);
	dev_info(dev, "IOPROC_SMISB.Cntrl.Control.L2IrqGpSts = 0x%08X\n", IOPROC_SMISB.Cntrl.Control.L2IrqGpSts.Reg32);

	return IRQ_HANDLED;
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
	int ret;

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
	unimac->puiInMsgSts = (uint32_t __iomem*) &IOPROC_SMISB.In.IncomingMessageFifo.InMsgSts.Reg32;
	unimac->puiInMsgData = (uint32_t __iomem*) &IOPROC_SMISB.In.IncomingMessageFifo.InMsgData;
	unimac->puiOgMsgSts = (uint32_t __iomem*) &IOPROC_SMISB.Og.OutgoingMessageFifo.OgMsgSts.Reg32;
	uint32_t uiInMsgDataPhysicalAddr = ((uint32_t)unimac->puiInMsgData) -
		((uint32_t)&IOPROC_SMISB) + LtoP((uint32_t)&IOPROC_UNCACHED);

	ret = request_irq(unimac->irq, unimac_isr_msp, 0, ndev->name, ndev);
	if (ret)
		return ret;
	// INT_CTRL.IopirqSense.Reg32 = 8;
	dev_info(dev, "&InMsgSts = 0x%08X\n", (uint32_t)unimac->puiInMsgSts);
	dev_info(dev, "&INT_CTRL.Iopirqstatus0 = 0x%08X\n", (uint32_t)&INT_CTRL.Iopirqstatus0.Reg32);
	dev_info(dev, "&INT_CTRL.Iopirqstatus1 = 0x%08X\n", (uint32_t)&INT_CTRL.Iopirqstatus1.Reg32);
	dev_info(dev, "&IOPROC_SMISB.Cntrl.Control.L1Irq4keStatus = 0x%08X\n", (uint32_t)&IOPROC_SMISB.Cntrl.Control.L1Irq4keStatus);
	dev_info(dev, "&IOPROC_SMISB.Cntrl.Control.L1IrqMipsStatus = 0x%08X\n", (uint32_t)&IOPROC_SMISB.Cntrl.Control.L1IrqMipsStatus);
	dev_info(dev, "&IOPROC_SMISB.Cntrl.Control.L2IrqGpSts = 0x%08X\n", (uint32_t)&IOPROC_SMISB.Cntrl.Control.L2IrqGpSts);

	/* Initialize MSP Start*/
	writel_be(0x18000007, (void __iomem *)MIPS_SMISB_CTRL);
	mdelay(10u);

	IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32 = 6; // LowWmWords, low water mark
	IOPROC_SMISB.Msgid.MessageId.MsgId[0].Reg32 = 1;
	IOPROC_SMISB.Msgid.MessageId.MsgId[1].Reg32 = 1;
	IOPROC_SMISB.Msgid.MessageId.MsgId[2].Reg32 = 2;
	IOPROC_SMISB.Msgid.MessageId.MsgId[3].Reg32 = 1;
	IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32 |= 1<<15; // NotEmptyIrqSts
	IOPROC_SMISB.Cntrl.Control.L1Irq4keMask.Reg32 |= 0x04; // InFifoIrqMask

	UNIMAC_DBG("IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl = 0x%08X\n", IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32);
	for (int i = 0; i<4; i++)
		UNIMAC_DBG("IOPROC_SMISB.Msgid.MessageId[i] = 0x%08X\n", IOPROC_SMISB.Msgid.MessageId.MsgId[i].Bits.MsgWdSzId);
	UNIMAC_DBG("MspInit\n");
	/* Initialize MSP End*/

	/* Initialize Unimac Start*/
	volatile Unimac *g_pxUnimacSelected = (volatile Unimac *) unimac->base;
	unimac->uiLanTxMsgFifo = LtoP((uint32_t)&g_pxUnimacSelected->Mbdma.Lantxmsgfifo01);
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

#if !BCM3380_UNIMAC_TEST
	napi_enable(&unimac->napi);

	timer_setup(&unimac->poll_timer, poll_timer_callback, 0);
	mod_timer(&unimac->poll_timer, jiffies + POLL_INTERVAL);

	netif_carrier_on(ndev);
	netif_start_queue(ndev);
#endif // #if BCM3380_UNIMAC_TEST

	return 0;
}

static int unimac_stop(struct net_device *ndev) {
	struct bcm3380_unimac *unimac = netdev_priv(ndev);

	UNIMAC_DBG("Linux wants to stop Unimac\n");

	netif_stop_queue(ndev);
	napi_disable(&unimac->napi);
	del_timer_sync(&unimac->poll_timer);

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

	byte_83F8A818 = 0;
	*((volatile uint32_t*)0xFF400034) &= ~0x1;

#if BCM3380_UNIMAC_TEST
	struct device *dev = ndev->dev.parent;
	if (unimac->pIop4keFwMem) {
		dma_free_coherent(dev, unimac->uiIop4keFwMemSize,
				  unimac->pIop4keFwMem,
				  unimac->pIop4keFwMemPhysical);
		unimac->pIop4keFwMem = NULL;
	}
#endif

	free_irq(unimac->irq, ndev);

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
	ret = vEthernetTx(unimac, length, skb->data);
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

/**
 * Return the length of the frame.
 * Return 0 if there is no pending frame.
 * Return negative on error.
 */
static int32_t uiEthPoll(struct bcm3380_unimac *unimac, int32_t (*pfOnPacketReady)(void*, const void*, size_t), void* arg) {
	uint32_t uiMsgSts = readl_be(unimac->puiInMsgSts);
	if (IOPROC_IN_FIFO_NOT_EMPTY(uiMsgSts)) {
		uint32_t uiRead1 = readl_be(unimac->puiInMsgData);

		uiMsgSts = readl_be(unimac->puiInMsgSts);
		if (IOPROC_IN_FIFO_NOT_EMPTY(uiMsgSts)) {
			uint32_t uiToken = readl_be(unimac->puiInMsgData);
			if ( uiRead1 >> 26 ) {
				UNIMAC_DBG("Error: Received an unexpected message: %08x, %08x\n", uiRead1, uiToken);
				return -1;
			} else {
				int32_t length = fpm_token_size(uiToken);
				if ( (uiRead1 & 0x383) != 0 ) {
					length = -3;
					UNIMAC_DBG("Error: LAN RX status = %x, token = %08x\n", uiRead1 & 0x7FFF, uiToken);
					// *uiLength = 0;
				} else {
					const void *packet = fpm_token_to_virt(unimac->fpm, uiToken);
					if (!packet) {
						UNIMAC_DBG("Error: FPM token did not map to a buffer: %08x\n",
							   uiToken);
						length = -4;
					} else {
						length = pfOnPacketReady(arg, packet, length);
					}
					// *uiLength = 0x8000; // This was returned in the stock bootloader
				}
				fpm_free_token(unimac->fpm, uiToken);
				return length;
			}
		} else {
			UNIMAC_DBG("Error: The incoming message fifo had an incomplete message: %08x\n", uiRead1);
			return -2;
		}
	}

	return 0; // No message
}

static uint32_t TransmitBurst(uint32_t *tx_params, uint32_t burstSize, uint32_t Lantxmsgfifo01) {
	volatile uint32_t* pTxStatus = (volatile uint32_t*)(0xFF500000 + 0x3E8);
	
	// Enable peripheral if flag not set
	if (byte_83F8A818 == 0) {
		byte_83F8A818 = 1;
		*((volatile uint32_t*)0xFF400034) |= 0x1;
	}

	// Validate burst size (1-16 elements)
	if (burstSize < 1 || burstSize > 16) {
		printk("Error: Invalid burst size (%d) specified", burstSize);
		return 0;
	}

	// Find available TX slot (8 possible slots)
	uint32_t status = *pTxStatus;
	int32_t slot = -1;
	for (int i = 0; i < 8; i++) {
		if ((status & 0x1) == 0) { // Check if slot is free
			slot = i;
			break;
		}
		status >>= 4; // Next slot status in next nibble
	}

	if (slot == -1) {
		printk("Error: TransmitBurst() unable to find available TX slot");
		return 0;
	}

	// Calculate hardware register base for this slot
	uint32_t regBase = 0xFF500000 + (slot * 0x84);

	// Write burst parameters to hardware registers
	for (int i = 0; i < burstSize; i++) {
		volatile uint32_t* pReg = (volatile uint32_t*)(regBase + i*4);
		*pReg = tx_params[i]; // Write parameter to register
	}

	// Configure burst control registers
	volatile uint32_t* pBurstCtrl = (volatile uint32_t*)(regBase + 0x40);
	pBurstCtrl[0] = Lantxmsgfifo01;  // Set UniMAC register address?
	pBurstCtrl[1] = burstSize;       // Set burst size
	pBurstCtrl[2] = 2;               // Start transmission command?

	return 1; // Success
}

static uint32_t vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn, const void *buffer) {
	size_t clamped_length = (uiLengthIn < 64) ? 64 : uiLengthIn;

	if (IOPROC_OG_GET_FIFO_VACANCY(readl_be(unimac->puiOgMsgSts)) < 2) {
		UNIMAC_DBG("Error: TX FIFO has insufficient space for a TX message.\n");
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

	// Prepare parameters for DMA transmission
	uint32_t tx_params[2];
	tx_params[0] = 0x4208000; // Control/command value for the DMA engine?
	tx_params[1] = adjusted_token;

	return TransmitBurst(tx_params, 2, unimac->uiLanTxMsgFifo) > 0;
}

#if BCM3380_UNIMAC_TEST

static const uint8_t bcm3380_iop4ke_rx_to_dqm_fw[] = {
	0x3c, 0x08, 0xe0, 0x00, 0x35, 0x08, 0x10, 0x00,
	0x3c, 0x09, 0x4b, 0x45, 0x35, 0x29, 0x4f, 0x4b,
	0xad, 0x09, 0x00, 0x28, 0x3c, 0x10, 0xe0, 0x00,
	0x36, 0x11, 0x12, 0x04, 0x36, 0x12, 0x12, 0x40,
	0x36, 0x13, 0x1a, 0x0c, 0x36, 0x14, 0x1c, 0x00,
	0x8e, 0x68, 0x00, 0x00, 0x31, 0x08, 0x3f, 0xff,
	0x11, 0x00, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00,
	0x8e, 0x28, 0x00, 0x00, 0x05, 0x01, 0xff, 0xfe,
	0x00, 0x00, 0x00, 0x00, 0x8e, 0x49, 0x00, 0x00,
	0x8e, 0x28, 0x00, 0x00, 0x05, 0x01, 0xff, 0xfe,
	0x00, 0x00, 0x00, 0x00, 0x8e, 0x4a, 0x00, 0x00,
	0x00, 0x09, 0x5e, 0x82, 0x15, 0x60, 0xff, 0xf2,
	0x00, 0x00, 0x00, 0x00, 0xae, 0x8a, 0x00, 0x00,
	0x10, 0x00, 0xff, 0xef, 0x00, 0x00, 0x00, 0x00,
};

static int bcm3380_iop4ke_test_start(struct bcm3380_unimac *unimac)
{
	struct device *dev = unimac->ndev->dev.parent;
	volatile IoprocIoprocControlRegs *ctrl = &IOPROC_SMISB.Cntrl.Control;
	uint32_t fw_bus_base;
	unsigned int i;

	if (sizeof(bcm3380_iop4ke_rx_to_dqm_fw) > BCM3380_IOP4KE_FW_MEM_SIZE)
		return -EINVAL;

	unimac->uiIop4keFwMemSize = BCM3380_IOP4KE_FW_MEM_SIZE;
	unimac->pIop4keFwMem = dma_alloc_coherent(dev, unimac->uiIop4keFwMemSize,
						  &unimac->pIop4keFwMemPhysical,
						  GFP_KERNEL);
	if (!unimac->pIop4keFwMem)
		return -ENOMEM;

	memset(unimac->pIop4keFwMem, 0, unimac->uiIop4keFwMemSize);
	memcpy(unimac->pIop4keFwMem, bcm3380_iop4ke_rx_to_dqm_fw,
	       sizeof(bcm3380_iop4ke_rx_to_dqm_fw));

	fw_bus_base = LtoP((uint32_t)unimac->pIop4keFwMemPhysical);

	ctrl->SoftResets.Reg32 |= BCM3380_IOP4KE_SOFT_RESET_M4KE;
	wmb();
	mdelay(1);

	ctrl->L1Irq4keMask.Reg32 = 0;
	ctrl->HostMboxIn = 0;
	ctrl->HostMboxOut = BCM3380_IOP4KE_RESET_VECTOR_PHYS;
	ctrl->Address1WindowMask = BCM3380_IOP4KE_WINDOW_MASK_2K;
	ctrl->Address1WindowBaseIn = BCM3380_IOP4KE_RESET_VECTOR_PHYS;
	ctrl->Address1WindowBaseOut = fw_bus_base;
	ctrl->Address2WindowMask = BCM3380_IOP4KE_WINDOW_MASK_2K;
	ctrl->Address2WindowBaseIn = 0;
	ctrl->Address2WindowBaseOut = fw_bus_base;
	wmb();

	ctrl->SoftResets.Reg32 &= ~BCM3380_IOP4KE_SOFT_RESET_M4KE;
	wmb();

	for (i = 0; i < 100; i++) {
		if (ctrl->HostMboxIn == BCM3380_IOP4KE_ALIVE_MAGIC) {
			dev_info(dev,
				 "MSP 4KE RX->DQM test firmware alive: fw_dma=0x%08X bus=0x%08X HostMboxIn=0x%08X CoreStatus=0x%08X\n",
				 (uint32_t)unimac->pIop4keFwMemPhysical,
				 fw_bus_base, ctrl->HostMboxIn,
				 ctrl->M4keCoreStatus.Reg32);
			return 0;
		}
		mdelay(1);
	}

	dev_err(dev,
		"MSP 4KE RX->DQM test firmware did not report alive: fw_dma=0x%08X bus=0x%08X HostMboxIn=0x%08X SoftResets=0x%08X CoreStatus=0x%08X A1Mask=0x%08X A1In=0x%08X A1Out=0x%08X\n",
		(uint32_t)unimac->pIop4keFwMemPhysical, fw_bus_base,
		ctrl->HostMboxIn, ctrl->SoftResets.Reg32,
		ctrl->M4keCoreStatus.Reg32, ctrl->Address1WindowMask,
		ctrl->Address1WindowBaseIn, ctrl->Address1WindowBaseOut);
	return -ETIMEDOUT;
}

static volatile IoprocIoprocQueueControl *bcm3380_dqm_qctrl(unsigned int queue)
{
	return &((volatile IoprocIoprocQueueControl *)&IOPROC_SMISB.Dqmqcntrl.Queue0Cntrl)[queue];
}

static volatile IoprocIoprocQueueData *bcm3380_dqm_qdata(unsigned int queue)
{
	return &((volatile IoprocIoprocQueueData *)&IOPROC_SMISB.Dqmqdata.Queue0Data)[queue];
}

static void bcm3380_dqm_config_queue(unsigned int queue, unsigned int token_words,
				     unsigned int mem_words, unsigned int start_words,
				     unsigned int low_water_mark)
{
	volatile IoprocIoprocQueueControl *qctrl = bcm3380_dqm_qctrl(queue);
	uint32_t queue_bit = BIT(queue);
	uint32_t mips_not_empty_mask = IOPROC_SMISB.DqMa.Dqm.DqmMipsNotEmptyIrqMsk.Reg32;
	uint32_t k4_not_empty_mask = IOPROC_SMISB.DqMa.Dqm.Dqm4keNotEmptyIrqMsk.Reg32;

	IOPROC_SMISB.DqMa.Dqm.DqmMipsNotEmptyIrqMsk.Reg32 = mips_not_empty_mask & ~queue_bit;
	IOPROC_SMISB.DqMa.Dqm.Dqm4keNotEmptyIrqMsk.Reg32 = k4_not_empty_mask & ~queue_bit;

	qctrl->Size.Reg32 = token_words - 1;
	qctrl->Cfga.Reg32 = (mem_words << 16) | start_words;
	qctrl->Cfgb.Reg32 = ((mem_words / token_words) << 16) | low_water_mark;

	IOPROC_SMISB.DqMa.Dqm.DqmLowWtmkIrqSts.Reg32 = queue_bit;
	IOPROC_SMISB.DqMa.Dqm.DqmNotEmptyIrqSts.Reg32 = queue_bit;
	IOPROC_SMISB.DqMa.Dqm.DqmMipsNotEmptyIrqMsk.Reg32 = mips_not_empty_mask;
	IOPROC_SMISB.DqMa.Dqm.Dqm4keNotEmptyIrqMsk.Reg32 = k4_not_empty_mask;

	UNIMAC_DBG("DQM q%u Size=0x%08X Cfga=0x%08X Cfgb=0x%08X Sts=0x%08X\n",
		   queue, qctrl->Size.Reg32, qctrl->Cfga.Reg32, qctrl->Cfgb.Reg32,
		   qctrl->Sts.Reg32);
}

static void bcm3380_dqm_test_init(void)
{
	uint32_t queue_mask = BIT(BCM3380_DQM_RX_Q_NORMAL) | BIT(BCM3380_DQM_RX_Q_HIGH);

	IOPROC_SMISB.DqMa.Dqm.DqmMipsNotEmptyIrqMsk.Reg32 &= ~queue_mask;
	IOPROC_SMISB.DqMa.Dqm.Dqm4keNotEmptyIrqMsk.Reg32 &= ~queue_mask;
	IOPROC_SMISB.DqMa.Dqm.DqmMipsLowWtmkIrqMsk.Reg32 &= ~queue_mask;
	IOPROC_SMISB.DqMa.Dqm.Dqm4keLowWtmkIrqMsk.Reg32 &= ~queue_mask;
	IOPROC_SMISB.DqMa.Dqm.DqmLowWtmkIrqSts.Reg32 = queue_mask;
	IOPROC_SMISB.DqMa.Dqm.DqmNotEmptyIrqSts.Reg32 = queue_mask;
	IOPROC_SMISB.DqMa.Dqm.DqmCfg.Reg32 = (BCM3380_DQM_TEST_MEM_WORDS << 16);

	bcm3380_dqm_config_queue(BCM3380_DQM_RX_Q_NORMAL, BCM3380_DQM_Q_TOKEN_WORDS,
				 BCM3380_DQM_RX_QUEUE_WORDS, 0, 0);
	bcm3380_dqm_config_queue(BCM3380_DQM_RX_Q_HIGH, BCM3380_DQM_Q_TOKEN_WORDS,
				 BCM3380_DQM_RX_QUEUE_WORDS, BCM3380_DQM_RX_QUEUE_WORDS, 0);

	UNIMAC_DBG("DQM init: DqmCfg=0x%08X NotEmptySts=0x%08X q0_data=%p q3_data=%p\n",
		   IOPROC_SMISB.DqMa.Dqm.DqmCfg.Reg32,
		   IOPROC_SMISB.DqMa.Dqm.DqmNotEmptySts.Reg32,
		   (void *)bcm3380_dqm_qdata(BCM3380_DQM_RX_Q_NORMAL),
		   (void *)bcm3380_dqm_qdata(BCM3380_DQM_RX_Q_HIGH));
}

static bool bcm3380_dqm_queue_not_empty(unsigned int queue)
{
	return IOPROC_SMISB.DqMa.Dqm.DqmNotEmptySts.Reg32 & BIT(queue);
}

#if BCM3380_UNIMAC_TEST_HOST_BRIDGE
static bool bcm3380_dqm_queue_has_space(unsigned int queue)
{
	return bcm3380_dqm_qctrl(queue)->Sts.Reg32 & BCM3380_DQM_Q_AVAIL_MASK;
}

static int bcm3380_bridge_inmsg_to_dqm(struct bcm3380_unimac *unimac)
{
	int moved = 0;

	while (IOPROC_IN_FIFO_NOT_EMPTY(readl_be(unimac->puiInMsgSts))) {
		uint32_t rx_msg = readl_be(unimac->puiInMsgData);
		uint32_t rx_token;
		unsigned int queue = BCM3380_DQM_RX_Q_NORMAL;

		if (!IOPROC_IN_FIFO_NOT_EMPTY(readl_be(unimac->puiInMsgSts))) {
			UNIMAC_DBG("IncomingMessageFifo incomplete message: first=0x%08X\n", rx_msg);
			return moved ? moved : -2;
		}

		rx_token = readl_be(unimac->puiInMsgData);
		if (rx_msg >> 26) {
			UNIMAC_DBG("Unexpected RX message header=0x%08X token=0x%08X\n", rx_msg, rx_token);
			fpm_free_token(unimac->fpm, rx_token);
			return moved ? moved : -1;
		}

		if (!bcm3380_dqm_queue_has_space(queue)) {
			UNIMAC_DBG("DQM q%u full: rx_msg=0x%08X token=0x%08X q_sts=0x%08X\n",
				   queue, rx_msg, rx_token, bcm3380_dqm_qctrl(queue)->Sts.Reg32);
			fpm_free_token(unimac->fpm, rx_token);
			return moved ? moved : -3;
		}

		bcm3380_dqm_qdata(queue)->Word0 = rx_token;
		moved++;

		UNIMAC_DBG("IncomingMessageFifo -> DQM q%u: rx_msg=0x%08X token=0x%08X not_empty=0x%08X q_sts=0x%08X\n",
			   queue, rx_msg, rx_token,
			   IOPROC_SMISB.DqMa.Dqm.DqmNotEmptySts.Reg32,
			   bcm3380_dqm_qctrl(queue)->Sts.Reg32);
	}

	return moved;
}
#endif

static int32_t bcm3380_dqm_poll_rx(struct bcm3380_unimac *unimac,
				   int32_t (*pfOnPacketReady)(void *, const void *, size_t),
				   void *arg)
{
	static const unsigned int queues[] = {
		BCM3380_DQM_RX_Q_HIGH,
		BCM3380_DQM_RX_Q_NORMAL,
	};
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(queues); i++) {
		unsigned int queue = queues[i];

		if (!bcm3380_dqm_queue_not_empty(queue))
			continue;

		uint32_t token = bcm3380_dqm_qdata(queue)->Word0;
		int32_t length = fpm_token_size(token);
		const void * packet = fpm_token_to_virt(unimac->fpm, token);
		if (!packet) {
			UNIMAC_DBG("DQM q%u token did not map to a buffer: 0x%08X\n",
				   queue, token);
			fpm_free_token(unimac->fpm, token);
			return -4;
		}

		length = pfOnPacketReady(arg, packet, length);
		fpm_free_token(unimac->fpm, token);

		UNIMAC_DBG("DQM q%u -> CPU: token=0x%08X len=%d not_empty=0x%08X q_sts=0x%08X\n",
			   queue, token, length, IOPROC_SMISB.DqMa.Dqm.DqmNotEmptySts.Reg32,
			   bcm3380_dqm_qctrl(queue)->Sts.Reg32);
		return length;
	}

	return 0;
}

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
	bcm3380_dqm_test_init();
	if (bcm3380_iop4ke_test_start(unimac))
		UNIMAC_DBG("MSP 4KE RX->DQM test firmware start failed\n");

	int32_t pollResult = 0;
	uint32_t idle_loops = 0;
	void* buffer = kzalloc(0x1000, GFP_KERNEL);
	if (!buffer) {
		UNIMAC_DBG("failed to allocate vUnimacDemo RX buffer\n");
		while (1)
			mdelay(1000);
	}
	do {
#if BCM3380_UNIMAC_TEST_HOST_BRIDGE
		int bridge_result = bcm3380_bridge_inmsg_to_dqm(unimac);

		if (bridge_result < 0)
			UNIMAC_DBG("IncomingMessageFifo -> DQM bridge error %d\n", bridge_result);
#endif

		pollResult = bcm3380_dqm_poll_rx(unimac, vUnimacDemoRx, buffer);
		if (pollResult == 0) {
			if ((idle_loops++ & 0x3FF) == 0) {
				UNIMAC_DBG("RX idle: InMsgSts=0x%08X DqmNotEmptySts=0x%08X q0_sts=0x%08X q3_sts=0x%08X HostMboxIn=0x%08X CoreStatus=0x%08X\n",
					   readl_be(unimac->puiInMsgSts),
					   IOPROC_SMISB.DqMa.Dqm.DqmNotEmptySts.Reg32,
					   bcm3380_dqm_qctrl(BCM3380_DQM_RX_Q_NORMAL)->Sts.Reg32,
					   bcm3380_dqm_qctrl(BCM3380_DQM_RX_Q_HIGH)->Sts.Reg32,
					   IOPROC_SMISB.Cntrl.Control.HostMboxIn,
					   IOPROC_SMISB.Cntrl.Control.M4keCoreStatus.Reg32);
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
				UNIMAC_DBG("InMsgData = 0x%08X\n", readl_be(unimac->puiInMsgData));
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
				vEthernetTx(unimac, uiLength, buffer);
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

	do {
		struct CreateSkbContext context = {
			.skb = NULL,
			.napi = napi,
		};
		spin_lock(&unimac->fifo_lock);
		int32_t outcome = uiEthPoll(unimac, vCreateSkb, &context);
		spin_unlock(&unimac->fifo_lock);
		if (outcome == 0) {
			break;
		} else if (outcome < 0) {
			UNIMAC_DBG("Rx Err %d!!!\n", outcome);
			ndev->stats.rx_dropped++;
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
	} while (--budget > 0);

	if (work_done < budget) {
		// All packets processed; complete NAPI polling
		napi_complete_done(napi, work_done);
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

	/* Get IRQ */
	priv->irq = platform_get_irq(pdev, 0);
	if (priv->irq < 0) {
		err = priv->irq;
		goto err_free_netdev;
	}
	UNIMAC_DBG("IRQ: %d\n", priv->irq);

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

#if BCM3380_UNIMAC_TEST
	vUnimacDemo(priv);
#endif

	spin_lock_init(&priv->fifo_lock);

	/* Set up network device */
	ndev->netdev_ops = &bcm3380_netdev_ops;
	netif_napi_add(ndev, &priv->napi, unimac_poll);

	// ndev->ethtool_ops = &bcm3380_ethtool_ops; /* If implementing ethtool */

	/* Register network device */
	err = devm_register_netdev(dev, ndev);

	if (err)
		goto err_put_fpm;

	netif_carrier_off(ndev);

	return 0;

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
