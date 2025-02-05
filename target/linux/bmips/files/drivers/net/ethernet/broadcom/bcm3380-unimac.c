#include <linux/clk.h>
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
#include <bcm3380/fpm_blockdef.h>
#include <bcm3380/ioproc_blockdef.h>
#include <bcm3380/IntControl.h>

#define BCM3380_UNIMAC_DBG 1
#define BCM3380_UNIMAC_TEST 0

#if BCM3380_UNIMAC_DBG
#define UNIMAC_DBG(fmt, ...) \
	printk(KERN_INFO "%s: " fmt, __func__, ##__VA_ARGS__)
#define vDumpMemory(ptr, length) print_hex_dump(KERN_INFO, "", DUMP_PREFIX_NONE, 16, 1, ptr, length, false)
#else
#define UNIMAC_DBG(fmt, ...) \
	do { } while (0)
#endif

typedef int BOOL;
#define FpmBlock (*((volatile Fpm*)0xB2010000))
#define dword_B2017000 (((volatile uint32_t*)0xB2017000))
#define UNCACHED_SMISB (*((volatile IoprocBlockIoProc*)0xB5800000))
#define IOPROC_SMISB (*((volatile IoprocBlockIoProc*)0xB8800000))


#define MIPS_SMISB_CTRL 0xFF400030

#define POLL_INTERVAL (msecs_to_jiffies(100)) // Poll every 100 milliseconds

// macro to convert logical data addresses to physical
// DMA hardware must see physical address
#define LtoP( logicalAddr ) ( ((uint32_t)(logicalAddr)) & 0x1FFFFFFF )
#define PtoL( x ) ( LtoP(x) | 0xa0000000 )

/* Private driver data structure */
struct bcm3380_unimac {
	struct net_device *ndev;
	void __iomem *base;
	int irq;

	// The working memory of FPM, must be word-aligned
	void* puiFpmMem;
	uint32_t uiFpmMemNoCache; // g_uiUnimacDmaBuffer

	int uiLinkModeIndex; // dword_83F8A814
	int u5PhyPrtAddr;

	// FPM
	uint32_t __iomem* puiPoolxAllocDealloc; // fpm_pool.h FpmPoolPoolAlloc
#define FPM_TOKEN_SIZE_MASK (0xFFF)
#define FPM_GET_TOKEN_SIZE(token) ((token) & FPM_TOKEN_SIZE_MASK)
#define FPM_IS_TOKEN_VALID(token) (((token) & 0x80000000) ? 1 : 0)
#define FPM_GET_TOKEN_INDEX(token) (((token) >> 12) & 0x3FFFF)

	// MSP IOPROC
	uint32_t __iomem* puiInMsgSts; // ioproc.h IoprocIoprocInMsgSts
#define IOPROC_IN_FIFO_NOT_EMPTY(puiInMsgSts) ((puiInMsgSts & 0x80000000) ? 1 : 0)
	uint32_t __iomem* puiInMsgData;
	uint32_t __iomem* puiOgMsgSts; // ioproc.h IoprocIoprocOgMsgSts
#define IOPROC_OG_GET_FIFO_VACANCY(puiOgMsgSts) (puiOgMsgSts & 0x1F)

	uint32_t uiLanTxMsgFifo;

	struct timer_list poll_timer;

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;

	struct napi_struct napi;
};

static uint32_t vEthernetTx(struct bcm3380_unimac *unimac, size_t uiLengthIn, const void *buffer);
BOOL bLinkUp(struct bcm3380_unimac *unimac);
static void vMdioWrite(volatile Unimac *g_pxUnimacSelected, uint32_t u5PhyPrtAddr, uint32_t u5RegDecAddr, uint16_t usDataAddr);
static uint16_t usMdioRead(volatile Unimac *g_pxUnimacSelected, int u5PhyPrtAddr, int u5RegDecAddr);

static void sub_83F821F4(int a1)
{
  signed int v1; // $s0
  int v2; // $a1
  int v3; // $v0
  int v4; // $a1
  int v5; // $a0
  int v6; // $a1
  int v7; // $v0
  int v8; // $a1
  int v9; // $v0
  uint32_t v10[12]; // [sp+0h] [-30h] BYREF

  v1 = a1;
  if ( (uint8_t)a1 )
  {
	v1 = (a1 + 255) & 0xFFFFFF00;
	UNIMAC_DBG("Error: FPM token limit must be a multiple of 256.  Rounding up to %d\n", v1);
  }
  v2 = 0;
  if ( v1 <= 0x3FFFFFF )
  {
	v3 = 0;
	do
	{
	  v10[v3] = -1;
	  v3 = ++v2;
	}
	while ( v2 < 10 );
	v4 = 0;
	if ( v1 >> 8 > 0 )
	{
	  v5 = 0;
	  do
	  {
		v10[v5 + 1] &= ~(1 << (v4++ & 0x1F));
		v5 = v4 >> 5;
	  }
	  while ( v4 < v1 >> 8 );
	}
	v6 = 0;
	v7 = 0;
	do
	{
	  if ( v10[v7 + 1] != -1 )
		v10[0] &= ~(1 << (v6 & 7));
	  v7 = ++v6;
	}
	while ( v6 < 8 );



	dword_B2017000[0] = v10[0];
	v8 = 1;
	v9 = 1;
	do
	{
	  dword_B2017000[v9 + 1] = v10[v9];
	  v9 = ++v8;
	}
	while ( v8 < 9 );

	for (int i=0; i<12; i++)
		UNIMAC_DBG("v10[%d] = 0x%08X, dword_B2017000[%d] = 0x%08X\n", i, v10[i], i, dword_B2017000[i]);

	FpmBlock.FpmCtrl.MemData1 = v1;
	FpmBlock.FpmCtrl.MemCtl.Reg32 = 0xB0000000; // Set MemWr, MemSel=2b11
	mdelay(10u);
  }
  else
  {
	UNIMAC_DBG("Error: FPM token limit must be less than 64K.\n");
  }
}

// Timer callback function
static void poll_timer_callback(struct timer_list *t) {
	struct bcm3380_unimac *unimac = from_timer(unimac, t, poll_timer);

	// Schedule NAPI poll
	napi_schedule(&unimac->napi);
	// Rearm the timer
	mod_timer(&unimac->poll_timer, jiffies + POLL_INTERVAL);
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
	struct sockaddr addr;

	unimac->uiLinkModeIndex = 0;
	unimac->u5PhyPrtAddr = 0;
	unimac->puiPoolxAllocDealloc = (uint32_t __iomem*) &FpmBlock.FpmPool.Pool1AllocDealloc.Reg32;
	unimac->puiInMsgSts = (uint32_t __iomem*) &IOPROC_SMISB.In.IncomingMessageFifo.InMsgSts.Reg32;
	unimac->puiInMsgData = (uint32_t __iomem*) &IOPROC_SMISB.In.IncomingMessageFifo.InMsgData;
	unimac->puiOgMsgSts = (uint32_t __iomem*) &IOPROC_SMISB.Og.OutgoingMessageFifo.OgMsgSts.Reg32;
	uint32_t uiInMsgDataPhysicalAddr = ((uint32_t)unimac->puiInMsgData) -
		((uint32_t)&IOPROC_SMISB) + LtoP((uint32_t)&UNCACHED_SMISB);

	/* Initialize FPM Start*/
	unimac->puiFpmMem = kzalloc(0x80000, GFP_KERNEL);// Align to 4-byte
	unimac->uiFpmMemNoCache = ((uint32_t)unimac->puiFpmMem) | 0xA0000000;

	UNIMAC_DBG("FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x%08X;\n", FpmBlock.FpmCtrl.FpmCtl.Reg32);
	FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x10;         // Set InitMem
	UNIMAC_DBG("FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x10;\n");
	while ( (FpmBlock.FpmCtrl.FpmCtl.Reg32 & 0x10) != 0 );
	UNIMAC_DBG("while ( (FpmBlock.FpmCtrl.FpmCtl.Reg32 & 0x10) != 0 );\n");

	FpmBlock.FpmCtrl.Pool1Cfg1.Reg32 = 0x6000000; // FpBufSize=3h6

	// Set pool base address, must aligned to 4-byte boundaries
	FpmBlock.FpmCtrl.Pool1Cfg2.Reg32 = LtoP(unimac->uiFpmMemNoCache);

	sub_83F821F4(0x100);
	FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x10000;      // Set Pool1Enable

	UNIMAC_DBG("FpmCtrl.Pool1Cfg1 = 0x%08X\n", FpmBlock.FpmCtrl.Pool1Cfg1.Reg32);
	UNIMAC_DBG("FpmCtrl.Pool1Cfg2 = 0x%08X\n", FpmBlock.FpmCtrl.Pool1Cfg2.Reg32);
	UNIMAC_DBG("FpmCtrl.FpmCtl    = 0x%08X\n", FpmBlock.FpmCtrl.FpmCtl.Reg32);
	dev_info(&ndev->dev, "Fpm ready with working memory @ 0x%08X\n", unimac->uiFpmMemNoCache);
	/* Initialize FPM End*/

	/* Initialize MSP Start*/
	writel_be(0x18000007, (void __iomem *)MIPS_SMISB_CTRL);
	mdelay(10u);

	IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32 = 6;
	IOPROC_SMISB.Msgid.MessageId.MsgId[0].Reg32 = 1;
	IOPROC_SMISB.Msgid.MessageId.MsgId[1].Reg32 = 1;
	IOPROC_SMISB.Msgid.MessageId.MsgId[2].Reg32 = 2;
	IOPROC_SMISB.Msgid.MessageId.MsgId[3].Reg32 = 1;

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
	g_pxUnimacSelected->Mbdma.Bufferbase = LtoP(unimac->uiFpmMemNoCache);
	g_pxUnimacSelected->Mbdma.Buffersize.Reg32 = 6;
	g_pxUnimacSelected->Mbdma.Tokenaddress = LtoP(unimac->puiPoolxAllocDealloc);
	g_pxUnimacSelected->Mbdma.Globalctl.Reg32 = 0x40000081;// LanTxMsgId2w=6d1, LanTxMsgId3w=6d2, AllocLimit=8h40
	g_pxUnimacSelected->Mbdma.Tokencachectl.Reg32 = 0x90309010;// AllocEnable=1b1, AllocMaxBurst=5h10, AllocThresh=8h30, FreeEnable=1, FreeMaxBurst=5h10, FreeThresh=5h10

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
	FpmBlock.FpmCtrl.FpmCtl.Reg32 &= ~0x10000;      // Clear Pool1Enable

	kfree(unimac->puiFpmMem);
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

	UNIMAC_DBG("Linux wants to send %d bytes\n", length);
	vDumpMemory(skb->data, length > 16 ? 16 : length);
	// Transmit the packet using vEthernetTx
	ret = vEthernetTx(unimac, length, skb->data);

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
int32_t uiEthPoll(struct bcm3380_unimac *unimac, int32_t (*pfOnPacketReady)(void*, const void*, size_t), void* arg) {
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
				int32_t length = FPM_GET_TOKEN_SIZE(uiToken);
				if ( (uiRead1 & 0x383) != 0 ) {
					length = -3;
					UNIMAC_DBG("Error: LAN RX status = %x, token = %08x\n", uiRead1 & 0x7FFF, uiToken);
					// *uiLength = 0;
				} else {
					uint32_t uiFifoChunkOffset = (((uiToken >> 12) << 11) & 0xFFFF);
					length = pfOnPacketReady(arg, (const void *)(unimac->uiFpmMemNoCache + uiFifoChunkOffset), length);
					// *uiLength = 0x8000; // This was returned in the stock bootloader
				}
				writel_be(uiToken, unimac->puiPoolxAllocDealloc);
				return length;
			}
		} else {
			UNIMAC_DBG("Error: The incoming message fifo had an incomplete message: %08x\n", uiRead1);
			return -2;
		}
	}

	return 0; // No message
}

char byte_83F8A818 = 0;
uint32_t TransmitBurst(uint32_t *tx_params, uint32_t burstSize, uint32_t Lantxmsgfifo01) {
	UNIMAC_DBG("TransmitBurst\n");
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

	uint32_t token = readl_be(unimac->puiPoolxAllocDealloc);
	if (!FPM_IS_TOKEN_VALID(token)) {
		UNIMAC_DBG("Error: Got an invalid token from the FPM!\n");
		return 0;
	}

	// Calculate DMA destination address using the token and global DMA buffer
	uint32_t dma_dest = unimac->uiFpmMemNoCache + (((token >> 12) & 0xFFFF) << 11);

	// Copy the Ethernet packet data to the DMA buffer
	memcpy((void*)dma_dest, buffer, clamped_length);

	// Update the token with the clamped length's lower 12 bits
	uint32_t adjusted_token = (token & ~FPM_TOKEN_SIZE_MASK) | (clamped_length & FPM_TOKEN_SIZE_MASK);

	// Prepare parameters for DMA transmission
	uint32_t tx_params[2];
	tx_params[0] = 0x4208000; // Control/command value for the DMA engine?
	tx_params[1] = adjusted_token;

	return TransmitBurst(tx_params, 2, unimac->uiLanTxMsgFifo) > 0;
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

static void vUnimacDemo(struct bcm3380_unimac *unimac) {
	unimac_open(unimac->ndev);

	int32_t pollResult = 0;
	void* buffer = kzalloc(0x1000, GFP_KERNEL);
	do {
		pollResult = uiEthPoll(unimac, vUnimacDemoRx, buffer);
		if (pollResult > 0) {
			uint32_t uiLength = pollResult;

			UNIMAC_DBG("Ethernet Rx Good, len = 0x%08X\n", uiLength);
			vDumpMemory(buffer, uiLength);

			uint32_t fcs = crc32_le(~0, buffer, uiLength - 4);
			fcs ^= ~0;
			UNIMAC_DBG("FCS = 0x%08X", fcs); // Need to swap endianess

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

void print_skb_metadata(const struct sk_buff *skb) {
	if (!skb)
		return;

	struct ethhdr *pxEthernetHeader = eth_hdr(skb);

    printk(KERN_INFO "SKB Metadata:\n");
    printk(KERN_INFO "  Packet Length: %u\n", skb->len);
    printk(KERN_INFO "  Data Length: %u\n", skb->data_len);
    printk(KERN_INFO "  dstMac : %pM\n", pxEthernetHeader->h_dest);
    printk(KERN_INFO "  srcMac : %pM\n", pxEthernetHeader->h_source);
    printk(KERN_INFO "  Network Header: %p\n", skb_network_header(skb));
    printk(KERN_INFO "  Transport Header: %p\n", skb_transport_header(skb));
    printk(KERN_INFO "  Device: %s\n", skb->dev ? skb->dev->name : "NULL");
    printk(KERN_INFO "  Protocol: 0x%04x\n", ntohs(skb->protocol));
}

struct CreateSkbContext {
	struct sk_buff *skb;
	struct napi_struct* napi;
};

static int32_t vCreateSkb(void* arg, const void* pBuffer, size_t uiLength) {
	struct CreateSkbContext *context = (struct CreateSkbContext *)arg;

	uiLength -= 4; // Exclude FCS
	context->skb = napi_alloc_skb(context->napi, uiLength);
	if (context->skb) {
		memcpy(context->skb->data, pBuffer, uiLength);
		print_skb_metadata(context->skb);
		return uiLength;
	}

	return -114514;
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
		int32_t outcome = uiEthPoll(unimac, vCreateSkb, &context);
		if (outcome == 0) {
			break;
		} else if (outcome < 0) {
			UNIMAC_DBG("Rx Err %d!!!\n", outcome);
			ndev->stats.rx_dropped++;
		}
		struct sk_buff *skb = context.skb;
		size_t length = outcome;
		skb_put(skb, length);
		skb->protocol = eth_type_trans(skb, ndev);
		UNIMAC_DBG("Received %d bytes\n", length);
		vDumpMemory(skb->data, length);

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
		if (IS_ERR_OR_NULL(priv->clock))
			return PTR_ERR(priv->clock);
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
		if (IS_ERR_OR_NULL(priv->reset))
			return PTR_ERR(priv->reset);
		
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
		goto err_free_netdev;

	netif_carrier_off(ndev);

	return 0;

err_free_netdev:
	free_netdev(ndev);
	return err;
}

/* Remove function */
static int bcm3380_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	unregister_netdev(ndev);
	free_netdev(ndev);
	return 0;
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
