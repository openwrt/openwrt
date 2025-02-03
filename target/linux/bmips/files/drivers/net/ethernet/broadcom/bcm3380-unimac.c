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

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#include <bcm3380/unimac.h>
#include <bcm3380/fpm_blockdef.h>
#include <bcm3380/ioproc_blockdef.h>
#include <bcm3380/IntControl.h>

#define BCM3380_UNIMAC_DBG 1

#if BCM3380_UNIMAC_DBG
#define UNIMAC_DBG(fmt, ...) \
	printk(KERN_INFO "%s: " fmt, __func__, ##__VA_ARGS__)

static char nibble_to_hex(unsigned char nibble) {
	return (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
}

static void vDumpMemory(const void* ptr, size_t length) {
	const unsigned char* data = (const unsigned char*)ptr;
	char line_buffer[80];

	for (size_t i = 0; i < length; i += 16) {
		size_t buffer_i = 0;

		// Print up to 16 bytes in hex format
		for (size_t j = 0; j < 16; j++) {
			if (i + j < length) {
				char val = data[i + j];
				line_buffer[buffer_i++] = nibble_to_hex((val & 0xF0)>>4);
				line_buffer[buffer_i++] = nibble_to_hex(val & 0x0F);
				line_buffer[buffer_i++] = ' ';
			} else {
				line_buffer[buffer_i++] = ' ';
				line_buffer[buffer_i++] = ' ';
				line_buffer[buffer_i++] = ' ';
			}
		}
		line_buffer[buffer_i] = '\0';

		printk("%08x: %s\n", ((uint32_t)ptr) + i, line_buffer);
	}
}
#else
#define UNIMAC_DBG(fmt, ...) \
	do { } while (0)
#endif

/* Private driver data structure */
struct bcm3380_priv {
	struct net_device *ndev;
	void __iomem *base;
	int irq;
	/* Add other hardware-specific members:
	 * - DMA descriptors
	 * - TX/RX rings
	 * - Spinlocks
	 * - PHY information
	 */

	struct clk **clock;
	unsigned int num_clocks;

	struct reset_control **reset;
	unsigned int num_resets;
};

/* Network device operations */
static const struct net_device_ops bcm3380_netdev_ops = {

};

typedef int BOOL;
#define FpmBlock (*((volatile Fpm*)0xB2010000))
#define dword_B2017000 (((volatile uint32_t*)0xB2017000))
#define MIPS_SMISB_CTRL 0xFF400030
#define IOPROC_SMISB (*((volatile IoprocBlockIoProc*)0xB8800000))
//#define IntCtrl (*((IntControlRegs __iomem*)0xb4e00000))
static uint32_t g_uiUnimacDmaBuffer = 0;
static int dword_83F8A814 = 0;
static int g_bOnUnimac0 = 0; // We are on Unimac1

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

static void vFpmInit(void) {
	UNIMAC_DBG("FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x%08X;\n", FpmBlock.FpmCtrl.FpmCtl.Reg32);
	FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x10;         // Set InitMem
	UNIMAC_DBG("FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x10;\n");
	while ( (FpmBlock.FpmCtrl.FpmCtl.Reg32 & 0x10) != 0 );
	UNIMAC_DBG("while ( (FpmBlock.FpmCtrl.FpmCtl.Reg32 & 0x10) != 0 );\n");

	FpmBlock.FpmCtrl.Pool1Cfg1.Reg32 = 0x6000000; // FpBufSize=3h6
	uint32_t AlignedMemory = (uint32_t) kzalloc(0x80000, GFP_KERNEL);// Align to 4-byte
	UNIMAC_DBG("AlignedMemory = 0x%08X\n", AlignedMemory);
	g_uiUnimacDmaBuffer = AlignedMemory | 0xA0000000;
	FpmBlock.FpmCtrl.Pool1Cfg2.Reg32 = AlignedMemory & 0x1FFFFFFF;// 4 byte aligned address
	sub_83F821F4(0x100);
	FpmBlock.FpmCtrl.FpmCtl.Reg32 = 0x10000;      // Set Pool1Enable

	UNIMAC_DBG("FpmCtrl.Pool1Cfg1 = 0x%08X\n", FpmBlock.FpmCtrl.Pool1Cfg1.Reg32);
	UNIMAC_DBG("FpmCtrl.Pool1Cfg2 = 0x%08X\n", FpmBlock.FpmCtrl.Pool1Cfg2.Reg32);
	UNIMAC_DBG("FpmCtrl.FpmCtl    = 0x%08X\n", FpmBlock.FpmCtrl.FpmCtl.Reg32);
}

static void MspInit(void) {
	writel_be(0x18000007, (void __iomem *)MIPS_SMISB_CTRL);; // MIPS_SMISB_CTRL
	mdelay(10u);

	IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32 = 6;
	IOPROC_SMISB.Msgid.MessageId.MsgId[0].Reg32 = 1;
	IOPROC_SMISB.Msgid.MessageId.MsgId[1].Reg32 = 1;
	IOPROC_SMISB.Msgid.MessageId.MsgId[2].Reg32 = 2;
	IOPROC_SMISB.Msgid.MessageId.MsgId[3].Reg32 = 1;

	UNIMAC_DBG("IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl = 0x%08X\n", IOPROC_SMISB.In.IncomingMessageFifo.InMsgCtl.Reg32);
	for (int i = 0; i<4; i++)
		UNIMAC_DBG("IOPROC_SMISB.Msgid.MessageId[i] = 0x%08X\n", IOPROC_SMISB.Msgid.MessageId.MsgId[i].Bits.MsgWdSzId);
}

static uint16_t usMdioRead(volatile Unimac *g_pxUnimacSelected, int u5PhyPrtAddr, int u5RegDecAddr) {
	g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32 = (u5PhyPrtAddr << 21) | (u5RegDecAddr << 16) | 0x28000000;// Set StartBusy, OpCode=2b10
	UNIMAC_DBG("UnimacInterface.MdioCmd = 0x%08X\n", g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32);
	while ( (g_pxUnimacSelected->UnimacInterface.MdioCfg.Reg32 & 0x100) != 0 );// while (MdioBusy);
	uint16_t val = g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32;
	UNIMAC_DBG("[u5PhyPrtAddr=%d, ui5RegDecAddr=%d]-->0x%04X\n", u5PhyPrtAddr, u5RegDecAddr, val);
	return val;
}

static void vMdioWrite(volatile Unimac *g_pxUnimacSelected, uint32_t u5PhyPrtAddr, uint32_t u5RegDecAddr, uint16_t usDataAddr) {
	g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32 = (u5PhyPrtAddr << 21) | (u5RegDecAddr << 16) | usDataAddr | 0x24000000;// Set StartBusy, opcode=2b01
	UNIMAC_DBG("UnimacInterface.MdioCmd = 0x%08X\n", g_pxUnimacSelected->UnimacInterface.MdioCmd.Reg32);
	while ( (g_pxUnimacSelected->UnimacInterface.MdioCfg.Reg32 & 0x100) != 0 );// MdioBusy
	UNIMAC_DBG("[u5PhyPrtAddr=%d, ui5RegDecAddr=%d]<--0x%04X\n", u5PhyPrtAddr, u5RegDecAddr, usDataAddr);
}

struct LinkStat // sizeof=0x8
{                                       // XREF: BL_CODE:qwq/r
	uint8_t EthSpeed;
	uint8_t HdEna; // Half-Duplex
	uint8_t b;
	uint8_t c;
	const char *c_acString;
};

struct LinkStat g_LinkStats[8] = {
	{0, 1, 0, 0, "incomplete"},
	{0, 1, 0, 0, "10M half"},
	{0, 0, 0, 0, "10M full"},
	{1, 1, 0, 0, "100M half"},
	{1, 0, 0, 0, "100M T4"},
	{1, 0, 0, 0, "100M full"},
	{2, 1, 0, 0, "1G half"},
	{2, 0, 0, 0, "1G full"},
};

BOOL bLinkUp(volatile Unimac *g_pxUnimacSelected) {
  int v0; // $v1
  struct LinkStat *v2; // $v0
  int HdEna; // $s0
  int EthSpeed; // $s2
  uint32_t uiUnimacCmd; // $s1 MAPDST

  if (usMdioRead(g_pxUnimacSelected, g_bOnUnimac0, 1) & 0x20)
  {
    v0 = (usMdioRead(g_pxUnimacSelected, g_bOnUnimac0, 25) >> 8) & 7;
    if ( v0 != dword_83F8A814 )
    {
      uiUnimacCmd = g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32;
      dword_83F8A814 = v0;
      v2 = &g_LinkStats[v0];
      HdEna = v2->HdEna;
      EthSpeed = v2->EthSpeed;
      UNIMAC_DBG("Link up: %s\n", v2->c_acString);
      if ( HdEna )
        uiUnimacCmd |= 0x400u;
      else
        uiUnimacCmd &= ~0x400u;
      g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 = (uiUnimacCmd & 0xFFFFFFF3) | (4 * EthSpeed);
    }
    //PeriphBlockCached.Led.LedMode0.Reg32 = 0x300;// Led4Mode = 2b11
    return 1;
  }
  else
  {
    //PeriphBlockCached.Led.LedMode0.Reg32 = 0;
    return 0;
  }
}

static void vWaitForLinkUp(volatile Unimac *g_pxUnimacSelected) {
	if ( !bLinkUp(g_pxUnimacSelected) ) {
		UNIMAC_DBG("!bLinkUp\n");
		uint16_t v0 = usMdioRead(g_pxUnimacSelected, g_bOnUnimac0, 4);
		vMdioWrite(g_pxUnimacSelected, g_bOnUnimac0, 4u, v0 | 0xE0);
		v0 = usMdioRead(g_pxUnimacSelected, g_bOnUnimac0, 0);
		vMdioWrite(g_pxUnimacSelected, g_bOnUnimac0, 0, v0 | 0x200);
		UNIMAC_DBG("Waiting for link up...\n");
	}
}

static void vUnimacInit(volatile Unimac *g_pxUnimacSelected) {
	uint16_t macAddr[4] = {0x0000, 0x0010, 0x18FF, 0xFFFF};

	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= 0x2000u;// SwReset
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~0x2000u;
	g_pxUnimacSelected->UnimacCore.UnimacFrmLen.Reg32 = 2048;// FrameLength = 2048
	g_pxUnimacSelected->UnimacCore.UnimacMac0 = (((uint32_t)macAddr[1]) << 16) | ((uint32_t)macAddr[2]);
	g_pxUnimacSelected->UnimacCore.UnimacMac1.Reg32 = ((uint32_t)macAddr[3]);
	g_pxUnimacSelected->Mbdma.Bufferbase = g_uiUnimacDmaBuffer & 0x1FFFFFFF;
	g_pxUnimacSelected->Mbdma.Buffersize.Reg32 = 6;
	g_pxUnimacSelected->Mbdma.Tokenaddress = 0x12010200;
	g_pxUnimacSelected->Mbdma.Globalctl.Reg32 = 0x40000081;// LanTxMsgId2w=6d1, LanTxMsgId3w=6d2, AllocLimit=8h40
	g_pxUnimacSelected->Mbdma.Tokencachectl.Reg32 = 0x90309010;// AllocEnable=1b1, AllocMaxBurst=5h10, AllocThresh=8h30, FreeEnable=1, FreeMaxBurst=5h10, FreeThresh=5h10
	g_pxUnimacSelected->Mbdma.Chancontrol00.Reg32 = 0x1000000;// MaxBurst=9h10
	g_pxUnimacSelected->Mbdma.Lanmsgaddress0 = 0x15801240; //Should refer to MSP_BLOCK_SMISB.Ioproc.In.IncomingMessageFifo.InMsgData
	g_pxUnimacSelected->Mbdma.Chancontrol01.Reg32 = 0x1000301;// MaxBurst=9h10, MsgId=6b3; MaxReqs=4h01
	g_pxUnimacSelected->Mbdma.Lanmsgaddress1 = 0x15801240;
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 &= ~0x1000010u;// Clear PromisEn and NoLgthCheck
}

static void vUnimacEnableRxTx(volatile Unimac *g_pxUnimacSelected) {
	g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32 |= 3u;// Enable Rx and Tx
	UNIMAC_DBG("Enabled Rx and Tx\n");
}

/**
 * Return the length of the frame copied to pxRxBuf.
 * Return 0 if there is no pending frame.
 * Return negative on error.
 */
int32_t uiEthPoll(void* pxRxBuf) {
	uint32_t uiMsgSts = IOPROC_SMISB.In.IncomingMessageFifo.InMsgSts.Reg32;
	if ( (uiMsgSts & 0x80000000) != 0 )
	{
		uint32_t uiRead1 = IOPROC_SMISB.In.IncomingMessageFifo.InMsgData;
		uiMsgSts = IOPROC_SMISB.In.IncomingMessageFifo.InMsgSts.Reg32;

		if ( (uiMsgSts & 0x80000000) != 0 ) {
			uint32_t uiRead2 = IOPROC_SMISB.In.IncomingMessageFifo.InMsgData;
			if ( uiRead1 >> 26 ) {
				UNIMAC_DBG("Error: Received an unexpected message: %08x, %08x\n", uiRead1, uiRead2);
				return -1;
			} else {
				uint32_t length = uiRead2 & 0xFFF;
				if ( (uiRead1 & 0x383) != 0 ) {
					UNIMAC_DBG("Error: LAN RX status = %x, token = %08x\n", uiRead1 & 0x7FFF, uiRead2);
					// *uiLength = 0;
				} else {
					memcpy(pxRxBuf, (const void *)((((uiRead2 >> 12) << 11) & 0xFFFF) + g_uiUnimacDmaBuffer), length);
					// *uiLength = 0x8000; // This was returned in the stock bootloader
				}
				FpmBlock.FpmPool.Pool1AllocDealloc.Reg32 = uiRead2;
				return length;
			}
		} else {
			UNIMAC_DBG("Error: The incoming message fifo had an incomplete message: %08x\n", uiRead1);
			return -2;
		}
	}

	return 0; // No message
}

static const char *UnimacCoreFields1[] = {
    "uint8 Pad0[0x4]",
    "UnimacCoreUnimacHdBkpCntl UnimacHdBkpCntl",
    "UnimacCoreUnimacCmd UnimacCmd",
    "uint32 UnimacMac0",
    "UnimacCoreUnimacMac1 UnimacMac1",
    "UnimacCoreUnimacFrmLen UnimacFrmLen",
    "UnimacCoreUnimacPauseQuant UnimacPauseQunat",
    "uint32 Pad1.0",
    "uint32 Pad1.1",
    "uint32 Pad1.2",
    "uint32 Pad1.3",
    "uint32 Pad1.4",
    "uint32 Pad1.5",
    "uint32 Pad1.6",
    "uint32 Pad1.7",
    "uint32 Pad1.8",
    "UnimacCoreUnimacSfdOffset UnimacSfdOffset",
    "UnimacCoreUnimacMode UnimacMode",
    "UnimacCoreUnimacFrmTag0 UnimacFrmTag0",
    "UnimacCoreUnimacFrmTag1 UnimacFrmTag1",
    "uint32 Pad2.0",
    "uint32 Pad2.1",
    "uint32 Pad2.2",
    "UnimacCoreUnimacTxIpgLen UnimacTxIpgLen"
};

static void dumpUnimac1(void) {
	volatile Unimac* g_pxUnimacSelected = (volatile Unimac __iomem*)0xb2110000;
	printk("MIPS_SMISB_CTRL=0x%08X\n", __raw_readl((void __iomem *)MIPS_SMISB_CTRL));

	uint32_t __iomem* base = (uint32_t __iomem*)0xb2110800;
	for (uint32_t i=0; i<sizeof(UnimacCoreFields1) / sizeof(UnimacCoreFields1[0]); i++)
		printk("UnimacCore.%s (0x%08X)=0x%08X\n", UnimacCoreFields1[i], (uint32_t)(&(base[i])), base[i]);

	printk("&UnimacCore.UnimacCmd=0x%08X\n", (uint32_t)(&g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32));
	printk("UnimacCore.UnimacCmd=0x%08X\n", g_pxUnimacSelected->UnimacCore.UnimacCmd.Reg32);
	printk("UnimacCore.UnimacFrmLen=0x%08X\n", g_pxUnimacSelected->UnimacCore.UnimacFrmLen.Reg32);
	printk("UnimacCore.UnimacMac0=0x%08X\n", g_pxUnimacSelected->UnimacCore.UnimacMac0);
	printk("UnimacCore.UnimacMac1=0x%08X\n", g_pxUnimacSelected->UnimacCore.UnimacMac1.Reg32);
	printk("Mbdma.Bufferbase=0x%08X\n", g_pxUnimacSelected->Mbdma.Bufferbase);
	printk("Mbdma.Buffersize.Reg32=0x%08X\n", g_pxUnimacSelected->Mbdma.Buffersize.Reg32);
	printk("Mbdma.Tokenaddress=0x%08X\n", g_pxUnimacSelected->Mbdma.Tokenaddress);
	printk("Mbdma.Globalctl=0x%08X\n", g_pxUnimacSelected->Mbdma.Globalctl.Reg32);
}

static void vUnimacDemo(volatile Unimac *g_pxUnimacSelected) {
	vFpmInit();
	UNIMAC_DBG("Fpm ready, g_uiUnimacDmaBuffer = 0x%08X\n", g_uiUnimacDmaBuffer);
	MspInit();
	UNIMAC_DBG("MspInit\n");
	vUnimacInit(g_pxUnimacSelected);
	UNIMAC_DBG("vUnimacInit\n");
	vWaitForLinkUp(g_pxUnimacSelected);
	vUnimacEnableRxTx(g_pxUnimacSelected);
	while (!bLinkUp(g_pxUnimacSelected))
		mdelay(1000u);
	UNIMAC_DBG("bLinkUp!!!!!!!\n");
	UNIMAC_DBG("&IOPROC_SMISB.In.IncomingMessageFifo.InMsgData = 0x%08X\n", (uint32_t)(&(IOPROC_SMISB.In.IncomingMessageFifo.InMsgData)));
	dumpUnimac1();

	int32_t pollResult = 0;
	void* buffer = kzalloc(0x1000, GFP_KERNEL);
	do {
		pollResult = uiEthPoll(buffer);
		if (pollResult > 0) {
			uint32_t uiLength = pollResult;
			UNIMAC_DBG("Ethernet Rx Good, len = 0x%08X\n", uiLength);
			vDumpMemory(buffer, uiLength);
		}
		mdelay(100u);
	} while(1+1);
}

/* Probe function - called when device is discovered */
static int bcm3380_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	struct net_device *ndev;
	struct bcm3380_priv *priv;
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

	vUnimacDemo((volatile Unimac*) (priv->base));

	/* Set up network device */
	ndev->netdev_ops = &bcm3380_netdev_ops;
	// ndev->ethtool_ops = &bcm3380_ethtool_ops; /* If implementing ethtool */

	/* Get MAC address from device tree */
	//if (of_get_mac_address(pdev->dev.of_node, ndev->dev_addr)) {
	//	eth_hw_addr_random(ndev);
	//	dev_info(&pdev->dev, "Using random MAC address\n");
	//}

	/* Register network device */
	//err = register_netdev(ndev);
	//if (err)
	//	goto err_free_netdev;

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
