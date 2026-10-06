// SPDX-License-Identifier: GPL-2.0
/*
 * ZTE ZX279128S integrated Ethernet (4x GEMAC + switch fabric + DMA)
 *
 * Driver for the ZX279128S packet engine, reverse-engineered from the
 * stock U-Boot network stack (decompiler-verified against the raw
 * disassembly of mtd1_bootloader.bin). One netdev per front port (eth0..3,
 * internal GEPHYs at MDIO addresses 10..13) shares one DMA/BMU/ring
 * datapath. On the E1600 the jacks are eth0 = LAN3, eth1 = LAN2,
 * eth2 = LAN1, eth3 = WAN.
 * The ingress port is read from each RX descriptor (byte 6
 * bits 7:3 minus 1) and TX is steered to a port through the descriptor
 * egress-port field, as in the stock plat-zxylzb driver.
 *
 * Datapath: a reserved DRAM pool holds buffer-pool (BP) buffers, the
 * RX/TX descriptor rings and byte-enable lookup tables. TX copies the
 * skb into a BMU-allocated BP, patches a 16-byte descriptor and rings
 * a doorbell. RX polls per-queue completion counters, copies out of
 * the BP and releases the descriptor + BP back to hardware.
 *
 * RX is interrupt driven like the stock driver: the TM interrupt (status
 * TM+0x100, mask TM+0x104) schedules NAPI, and a 10 ms housekeeping
 * hrtimer covers TX reaping and any missed wakeup. use_irq=0 falls back to
 * a 1 ms hrtimer poll. All block base addresses are fixed
 * SoC addresses and are ioremapped directly; the DTS only supplies the
 * reserved memory pool and the MDIO bus phandle.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/hrtimer.h>
#include <linux/if_ether.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/mdio.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_reserved_mem.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/workqueue.h>

/* Reserved DRAM pool layout (relative to the memory-region base) */
#define POOL_BP_BASE			0x00000000	/* 2048 * 2048 B */
#define POOL_BP_COUNT			2048
#define POOL_BP_SIZE			2048
#define POOL_BP_DATA_OFF		0x10
#define POOL_AUX_BASE			0x00400000
#define POOL_BE16A_BASE			0x00800000	/* 0x800 x be16 */
#define POOL_BE16B_BASE			0x00808000	/* 0x200 x be16 */
#define POOL_RX_BASE			0x00a00000	/* 8 q x 64 KiB  */
#define POOL_RX_QSTRIDE			0x00010000
#define POOL_STATUS_BASE		0x00b00000	/* DMA status ring */
#define POOL_STATUS_SIZE		0x00010000
#define POOL_TX_BASE			0x00c00000	/* 1024 x 16 B   */

#define ZX_POLL_MS			1		/* RX/TX-completion poll period */
#define ZX_POLL_IRQ_MS			10		/* housekeeping tick in IRQ mode */

/* TM interrupt (stock zx_pon_tm_int / pon_tm_net_open): mask bit set = masked */
#define ZX_TM_INT_STATUS		0x100
#define ZX_TM_INT_MASK			0x104
#define ZX_TM_INT_RX			0x3		/* bits stock unmasks at open */
#define ZX_TM_INT_ALL			0x7
#define ZX_RX_DESCS			1024		/* per queue */
#define ZX_TX_DESCS			1024
#define ZX_RX_QUEUES			8
#define ZX_NR_PORTS			4
#define ZX_WAN_PORT			3		/* eth3 is the WAN jack */

/* Fixed SoC block bases */
#define ZX_PON_BASE			0x92000000	/* +0x08 reset   */
#define ZX_CRM_BASE			0x92040000
#define ZX_NPP_BASE			0x921c0000
#define ZX_NPP2_BASE			0x921cc000
#define ZX_NPP3_BASE			0x921d4000
#define ZX_NPP4_BASE			0x921d9000
#define ZX_MAC_BASE			0x92200000	/* + p * 0x40000 */
#define ZX_MAC_STRIDE			0x00040000
#define ZX_TM_BASE			0x92340000
#define ZX_RED_BASE			0x92344000
#define ZX_BMU_BASE			0x92348000
#define ZX_BLK4C_BASE			0x9234c000
#define ZX_DMA_BASE			0x92350000
#define ZX_PP0_BASE			0x92380000
#define ZX_PP1_BASE			0x92388000
#define ZX_PP2_BASE			0x9238c000
#define ZX_PP3_BASE			0x923a0000
#define ZX_WIN_SIZE			0x00001000

/* MAC port registers (base 0x92200000 + p * 0x40000) */
#define MAC_CTRL			0x0000
#define MAC_REG04			0x0004
#define MAC_REG08			0x0008
#define MAC_REG70			0x0070
#define MAC_REGB4			0x00b4
#define MAC_REGE0			0x00e0
#define MAC_REGB00			0x0b00
#define MAC_REGD00			0x0d00
#define MAC_REGD30			0x0d30

/* Vendor PHY access for MAC speed/duplex discovery */
#define GEPHY_REG1A			0x1a		/* link/speed/duplex */
#define GEPHY_REG1E			0x1e		/* page/window       */
#define GEPHY_1A_LINK			BIT(6)
#define GEPHY_1A_SPEED_MASK		0x380
#define GEPHY_1A_SPEED_SHIFT		7
#define ZX_PHY_ADDR_BASE		10		/* PHYs at 10..13    */

/* Queue -> front-port map used by U-Boot (queue 5 is unmapped) */
static const int zx_queue_port_map[ZX_RX_QUEUES] = { 0, 1, 2, 3, 4, -1, 5, 6 };

struct zx_priv {
	struct net_device	*ports[ZX_NR_PORTS];	/* one netdev per front port */
	struct net_device	*napi_dev;		/* dummy netdev owning NAPI */
	unsigned int		open_ports;		/* bitmask of running ports */
	bool			tx_stopped;
	struct device		*dev;
	struct mii_bus		*mdio;

	/* reserved pool */
	phys_addr_t		pool_phys;
	void __iomem		*pool_virt;

	/* ioremapped blocks */
	void __iomem		*pon;
	void __iomem		*crm;
	void __iomem		*npp;
	void __iomem		*npp2;
	void __iomem		*npp3;
	void __iomem		*npp4;
	void __iomem		*mac[ZX_NR_PORTS];
	void __iomem		*tm;
	void __iomem		*red;
	void __iomem		*bmu;
	void __iomem		*blk4c;
	void __iomem		*dma;
	void __iomem		*pp0;
	void __iomem		*pp1;
	void __iomem		*pp2;
	void __iomem		*pp3;

	/* RX rings (hardware) + software cursors */
	phys_addr_t		rx_ring_phys[ZX_RX_QUEUES];
	u32			rx_cursor[ZX_RX_QUEUES];

	/* TX ring */
	void __iomem		*tx_ring;
	u32			tx_cursor;
	u32			tx_pending;
	u32			tx_last_consumed;

	/* BP software free counter (mirrors U-Boot ctl+0x10) */
	spinlock_t		bp_lock;
	u32			bp_free_cnt;

	/* PHY ports */
	struct phy_device	*phy[ZX_NR_PORTS];
	bool			link_up[ZX_NR_PORTS];
	int			speed_cache[ZX_NR_PORTS];

	struct napi_struct	napi;
	struct hrtimer		poll_timer;
	int			irq;			/* TM interrupt, <= 0 if not in DT */
	bool			irq_mode;		/* RX driven by the interrupt */
	struct delayed_work	link_work;
	bool			hw_init_done;
};

/* netdev_priv() of every port netdev */
struct zx_port {
	struct zx_priv	*priv;
	int		idx;
};

/* TX descriptor egress-port code: word0[25:20] = port + 0x28 (stock pon_tm_net_tx) */
static bool tx_directed = true;
module_param(tx_directed, bool, 0644);
MODULE_PARM_DESC(tx_directed, "steer TX to the netdev's front port instead of a fabric DA lookup");

static unsigned int tx_port_code[ZX_NR_PORTS] = { 0x28, 0x29, 0x2a, 0x2b };
module_param_array(tx_port_code, uint, NULL, 0644);
MODULE_PARM_DESC(tx_port_code, "descriptor egress-port codes per front port");

static unsigned int tx_w3 = 3;
module_param(tx_w3, uint, 0644);
MODULE_PARM_DESC(tx_w3, "descriptor word3 value used when tx_directed");

static bool use_irq = true;
module_param(use_irq, bool, 0644);
MODULE_PARM_DESC(use_irq, "RX via the TM interrupt (default), 0 = 1 ms poll fallback (read when the first port is opened)");

static void zx_tx_stop_all(struct zx_priv *priv)
{
	int p;

	priv->tx_stopped = true;
	for (p = 0; p < ZX_NR_PORTS; p++)
		if (priv->open_ports & BIT(p))
			netif_stop_queue(priv->ports[p]);
}

static void zx_tx_wake_all(struct zx_priv *priv)
{
	int p;

	priv->tx_stopped = false;
	for (p = 0; p < ZX_NR_PORTS; p++)
		if (priv->open_ports & BIT(p))
			netif_wake_queue(priv->ports[p]);
}

static const char * const zx_speed_str[] = { "10M", "100M", "1G" };
static const char * const zx_duplex_str[] = { "half", "full" };

/* -------------------------------------------------------------------------
 * Low-level hardware helpers (1:1 with the U-Boot routines)
 * ---------------------------------------------------------------------- */

/*
 * zx_pon_pulse() - pulse the PON (power-on-reset) block
 * @priv: driver private data
 * @mask: reset bits to assert (clear) before releasing
 *
 * U-Boot pon_pulse @ 0x47f2fb48. The PON block at 0x92000000 holds one
 * reset bit per datapath slice: bits 6..9 reset MAC 0..3 individually,
 * 0xffffffff resets the whole fabric. U-Boot clears the requested bits,
 * waits 10 ms, then writes 0xffffffff - releasing *every* reset line,
 * not only the ones it had just asserted. Replayed verbatim; releasing
 * extra lines is what the vendor code does and is harmless.
 */
static void zx_pon_pulse(struct zx_priv *priv, u32 mask)
{
	writel(readl(priv->pon + 0x08) & ~mask, priv->pon + 0x08);
	mdelay(10);
	writel(0xffffffff, priv->pon + 0x08);
}

/*
 * zx_red_write() - one RED queue-table write transaction
 * @priv: driver private data
 * @q:    queue number (0..399)
 * @cfg:  four 32-bit config words for the selected entry
 * @op:   operation code, encoded in bits [23:22] of the command word
 *
 * U-Boot red_write @ 0x47f2fddc. RED (the queue scheduler/WRED block)
 * exposes a mailbox: poll bit 0 of +0x18 until *set* (ready), write
 * queue|op<<22 to +0x14, then the four config words to +0x1c..+0x28.
 * There is no completion wait - the next call's ready-poll covers it.
 * 20 bare polls with no delay, exactly like U-Boot; -ETIMEDOUT if the
 * mailbox never becomes ready.
 */
static int zx_red_write(struct zx_priv *priv, u32 q, const u32 cfg[4], u32 op)
{
	void __iomem *red = priv->red;
	int i;

	for (i = 0; i < 20; i++) {
		if (readl(red + 0x18) & 1) {
			writel(q | (op << 22), red + 0x14);
			writel(cfg[0], red + 0x1c);
			writel(cfg[1], red + 0x20);
			writel(cfg[2], red + 0x24);
			writel(cfg[3], red + 0x28);
			return 0;
		}
	}
	return -ETIMEDOUT;
}

/*
 * zx_red_init() - program the whole RED queue table
 * @priv: driver private data
 *
 * U-Boot red_init @ 0x47f2ff74. Three passes over the scheduler's
 * 400-entry queue table:
 *   1. op 0 (entry config) on queues 0..399 with {0x800400, 0, 0, 0}
 *   2. op 2 on queues 0..383 with the same zero config
 *   3. op 4 (WRED profile) on queues 0..383 with
 *      {0xff803fff, 0x100ff80, 0x100200, 0x20}
 * The bit meanings of the WRED words were not recovered; they are
 * replayed verbatim from the bootloader. Queues 384..399 exist but get
 * no WRED profile - same as stock.
 */
static int zx_red_init(struct zx_priv *priv)
{
	static const u32 cfg_zero[4] = { 0x800400, 0, 0, 0 };
	static const u32 cfg_wred[4] = { 0xff803fff, 0x100ff80, 0x100200, 0x20 };
	u32 q;
	int ret;

	for (q = 0; q < 400; q++) {
		ret = zx_red_write(priv, q, cfg_zero, 0);
		if (ret)
			return ret;
	}
	for (q = 0; q < 384; q++) {
		ret = zx_red_write(priv, q, cfg_zero, 2);
		if (ret)
			return ret;
	}
	for (q = 0; q < 384; q++) {
		ret = zx_red_write(priv, q, cfg_wred, 4);
		if (ret)
			return ret;
	}
	return 0;
}

/*
 * zx_dma_enable() - master enable for the DMA engine block
 * @priv: driver private data
 * @on:   true to enable the engine
 *
 * U-Boot dma_enable @ 0x47f30048. Read-modify-write of DMA+0x00: clear
 * bits [19:16], then set 0x002f0000 (on) or 0x00200000 (off, leaving
 * bit 17 set). Individual bit meanings were not recovered; the pattern
 * is replayed verbatim.
 */
static void zx_dma_enable(struct zx_priv *priv, bool on)
{
	u32 v = readl(priv->dma + 0x00) & 0xfff0ffff;

	writel(v | (on ? 0x002f0000 : 0x00200000), priv->dma + 0x00);
}

/*
 * zx_desc_release() - return RX descriptors to hardware via RED
 * @priv:  driver private data
 * @q:     descriptor ring id (0..7)
 * @count: descriptor count (0 as used everywhere by U-Boot = one)
 * @op:    operation code (1 = release-after-consume)
 * @mode:  ring mode select (0..1)
 *
 * U-Boot desc_release @ 0x47f2feec. This is the *second* RED mailbox
 * (+0x64 busy bit, +0x68 command word), distinct from the queue-table
 * mailbox in zx_red_write(). Wait for not-busy (bit 0 of +0x64 clear),
 * write (count<<14)|(op<<4)|(mode<<3)|q to +0x68, then kick with a 1
 * to +0x64. 30 bare polls, no delay. Releasing a descriptor tells the
 * scheduler the RX ring slot is reusable and decrements the queue's
 * pending-frame counter (DMA+0x100+q*4).
 */
static int zx_desc_release(struct zx_priv *priv, u32 q, u32 count, u32 op,
			   u32 mode)
{
	void __iomem *red = priv->red;
	int i;

	if (q > 7 || mode > 1)
		return -EINVAL;

	for (i = 0; i < 30; i++) {
		if (!(readl(red + 0x64) & 1)) {
			writel((count << 14) | (op << 4) | (mode << 3) | q,
			       red + 0x68);
			writel(1, red + 0x64);
			return 0;
		}
	}
	return -ETIMEDOUT;
}

/*
 * zx_bmu_alloc() - allocate one buffer-pool (BP) buffer
 * @priv: driver private data
 *
 * U-Boot bmu_alloc @ 0x47f2fd04. The BMU (Buffer Management Unit)
 * hands out 2 KiB buffer ids from the reserved pool through a single
 * hardware mailbox: BMU+0x14 is a request/ack line. If it is already
 * busy we fail immediately (no queueing); otherwise write 1, poll up
 * to ~4 us for hardware to clear it, then read the granted id from
 * BMU+0x0c. Bit 31 set = success with the BP id in bits [15:0];
 * bit 31 clear = allocation failed. Caller must hold bp_lock because
 * the mailbox is global. Returns BP id (0..2047) or -ETIMEDOUT.
 */
static int zx_bmu_alloc(struct zx_priv *priv)
{
	void __iomem *bmu = priv->bmu;
	u32 v;
	int i;

	if (readl(bmu + 0x14))
		return -ETIMEDOUT;

	writel(1, bmu + 0x14);
	for (i = 0; i < 4; i++) {
		if (readl(bmu + 0x14) == 0)
			break;
		udelay(1);
	}
	if (readl(bmu + 0x14) != 0)
		return -ETIMEDOUT;

	v = readl(bmu + 0x0c);
	if ((s32)v < 0) {		/* success: BP id in bits [15:0] */
		writel(0, bmu + 0x14);
		return v & 0xffff;
	}
	writel(0, bmu + 0x14);
	return -ETIMEDOUT;
}

/*
 * zx_bp_release() - return a BP id to the BMU free FIFO
 * @priv: driver private data
 * @bp:   buffer-pool id previously obtained from zx_bmu_alloc()
 *
 * U-Boot bp_release @ 0x47f2fd74. Free BP ids are pushed by writing
 * the id to BMU+0x10, but only when software believes the FIFO has
 * space: a mirrored free counter (bp_free_cnt) is refreshed from
 * BMU+0xdc bits [8:3] (hardware counts 64-byte units; >>3 converts to
 * 2 KiB buffer units). Up to 5 refresh attempts, then -ETIMEDOUT.
 * Only RX buffers flow through here - TX BPs are freed by hardware
 * once the DMA engine consumes the TX descriptor. Caller holds
 * bp_lock (shared with zx_bmu_alloc/zx_tx_reap).
 */
static int zx_bp_release(struct zx_priv *priv, u32 bp)
{
	void __iomem *bmu = priv->bmu;
	int i;

	for (i = 0; i < 5; i++) {
		if (priv->bp_free_cnt != 0) {
			priv->bp_free_cnt--;
			writel(bp, bmu + 0x10);
			return 0;
		}
		priv->bp_free_cnt = (readl(bmu + 0xdc) & 0x1ff) >> 3;
	}
	return -ETIMEDOUT;
}

/* -------------------------------------------------------------------------
 * One-shot hardware bring-up (U-Boot 0x47f2fb80 order)
 * ---------------------------------------------------------------------- */

/*
 * zx_fill_be16_table() - prefill a byte-enable lookup table in the pool
 * @priv:   driver private data
 * @offset: pool-relative address of the table
 * @count:  number of 16-bit entries
 *
 * U-Boot fills two tables at bring-up: 0x800 entries at pool+0x800000
 * and 0x200 entries at pool+0x808000, each entry simply the big-endian
 * encoding of its own index. The TM/BMU read these during frame
 * assembly; their exact role was not recovered, so the tables are
 * reproduced byte-for-byte and handed to the hardware via TM+0xe8/f0.
 */
static void zx_fill_be16_table(struct zx_priv *priv, u32 offset, u32 count)
{
	__be16 __iomem *tbl = priv->pool_virt + offset;
	u32 i;

	for (i = 0; i < count; i++)
		writew(cpu_to_be16((u16)i), &tbl[i]);
}

/*
 * zx_bmu_init() - describe the buffer pool to BMU and TM
 * @priv: driver private data
 *
 * U-Boot bmu_init @ 0x47f2fb80 (inline part). Tells the hardware where
 * everything lives: BMU+0x04/08 get the buffer geometry word
 * 0x0104c040 (2048 buffers of 2 KiB), +0x58/5c small thresholds, and
 * the TM receives the pool addresses: BP base (+0xf4), aux area
 * (+0xf8), the two BE16 tables (+0xe8/+0xec) and 0x08000800 (+0xfc,
 * table sizing, confirmed from the literal pool at 0x47f2fcd8). BMU
 * +0x48/+0x4c receive 0x08000000/0x02000000 (BP-id ranges for the two
 * buffer classes). The free counter mirror starts at 0; it is
 * refreshed lazily by zx_bp_release().
 */
static int zx_bmu_init(struct zx_priv *priv)
{
	void __iomem *bmu = priv->bmu;
	phys_addr_t pool = priv->pool_phys;

	writel(0, bmu + 0x00);
	writel(0x0104c040, bmu + 0x04);
	writel(0x0104c040, bmu + 0x08);
	writel(0x3f, bmu + 0x58);
	writel(0x0f, bmu + 0x5c);

	zx_fill_be16_table(priv, POOL_BE16A_BASE, 0x800);
	zx_fill_be16_table(priv, POOL_BE16B_BASE, 0x200);

	writel(pool + POOL_BP_BASE, priv->tm + 0xf4);
	writel(pool + POOL_AUX_BASE, priv->tm + 0xf8);
	writel(pool + POOL_BE16A_BASE, priv->tm + 0xe8);
	writel(pool + POOL_BE16B_BASE, priv->tm + 0xec);
	writel(0x08000800, priv->tm + 0xfc);

	writel(0, bmu + 0x10);		/* free counter */
	writel(0x08000000, bmu + 0x48);
	writel(0x02000000, bmu + 0x4c);
	writel(0, bmu + 0x14);

	priv->bp_free_cnt = 0;
	return 0;
}

/*
 * zx_pp_init() - packet-processor (L2 switch fabric) configuration
 * @priv: driver private data
 *
 * U-Boot pp_init @ 0x47f2fb80 steps 9. PP0+0=2 starts the fabric clock
 * domain (1 ms settle). The PP1 register block holds the bridging /
 * learning / flood-mask configuration - values replayed verbatim from
 * the bootloader (0xff5555ff VLAN-ish mask, 0xaaaaaaaa port group,
 * 0xbf874 aging, etc; bit meanings not recovered). PP2 gets the
 * MAC-address compare constants (0x309a multicast base). PP3+0x10/0x14
 * zeroed = no port isolation, everything floods to everything, which
 * is exactly the flat-bridge behaviour U-Boot provides and what this
 * single-netdev driver wants. Two msleep(1000)s make open() slow (~2 s)
 * - that is normal, the hardware needs them.
 */
static void zx_pp_init(struct zx_priv *priv)
{
	writel(2, priv->pp0 + 0x00);
	msleep(1000);

	writel(0x200ff, priv->pp1 + 0x004);
	writel(0xff5555ff, priv->pp1 + 0x340);
	writel(0x3e, priv->pp1 + 0x344);
	writel(0x3f, priv->pp1 + 0x380);
	writel(0xaaaaaaaa, priv->pp1 + 0x63c);
	writel(0xff, priv->pp1 + 0x1c0);
	writel(0x5555, priv->pp1 + 0x1c4);
	writel(0xbf874, priv->pp1 + 0x188);
	writel(0xff, priv->pp1 + 0x2c0);
	writel(0xffff, priv->pp1 + 0x300);
	writel(0x3e, priv->pp1 + 0x304);

	writel(0x309a, priv->pp2 + 0x80);
	writel(0, priv->pp2 + 0x88);
	writel(1, priv->pp2 + 0xcc);

	writel(0, priv->pp3 + 0x10);
	writel(0, priv->pp3 + 0x14);
}

/*
 * zx_npp_init() - NPU-side packet processor and queue/port routing
 * @priv: driver private data
 *
 * U-Boot npp_init @ 0x47f2fb80 step 10. NPP+0x08/+0x0c are soft-reset
 * lines for the two NPU halves: assert all, 1 s settle each. Then the
 * RX queue -> front-port map is installed: queue q feeds front port
 * zx_queue_port_map[q] (port 4 is the CPU port, queue 5 unmapped), by
 * writing 1 to NPP3+0x28c+port*4. NPP3+0x0/0x4 and +0x40/0x44 are the
 * two scheduler shaping registers (0x30000000/0x8800). +0x1c0 lists the
 * IP protocol numbers the SPA traps to the CPU (byte 3 = 0x11 UDP, byte
 * 2 = 0x06 TCP; U-Boot only had UDP, so host TCP never arrived). Per
 * MAC port, NPP+0x60+p*4 speed-select
 * bits [26:23] are cleared to "unknown" and speed_cache reset so the
 * first zx_smac_bringup() always reprograms the MAC. NPP2+0=0x11 and
 * NPP+0x48=0 close the sequence.
 */
static void zx_npp_init(struct zx_priv *priv)
{
	int p, q;

	writel(0xffffffff, priv->npp + 0x08);
	msleep(1000);
	writel(0xffffffff, priv->npp + 0x0c);
	msleep(1000);

	/* map RX queues to front ports (U-Boot queue_port_map) */
	for (q = 0; q < ZX_RX_QUEUES; q++)
		if (zx_queue_port_map[q] >= 0)
			writel(1, priv->npp3 + 0x28c + zx_queue_port_map[q] * 4);

	writel(0x30000000, priv->npp3 + 0x000);
	writel(0x8800, priv->npp3 + 0x004);
	writel(0x30000000, priv->npp3 + 0x040);
	writel(0x8800, priv->npp3 + 0x044);
	writel(0x11060000, priv->npp3 + 0x1c0);

	for (p = 0; p < ZX_NR_PORTS; p++) {
		u32 v = readl(priv->npp + 0x60 + p * 4);

		writel((v & ~0x3800000) | 0, priv->npp + 0x60 + p * 4);
		priv->speed_cache[p] = -1;
	}

	writel(0x11, priv->npp2 + 0x00);
	writel(0, priv->npp + 0x48);
}

/*
 * zx_oneshot_init() - full datapath bring-up, exactly U-Boot's order
 * @priv: driver private data
 *
 * U-Boot eth_init @ 0x47f2fb80, the single function the bootloader
 * runs before its first ping. Twelve steps, order-sensitive: PON
 * reset, RED scheduler, DMA engine + rings, BMU buffer pool, TM,
 * RX-ring table, PP fabric, NPU routing, CRM clock. Anything touching
 * the datapath before this point sees a dead fabric. Runs once per
 * boot (hw_init_done latch) from zx_open(); takes ~2 s because of the
 * PP/NPP settle delays. All pool-relative offsets are the layout
 * documented at the top of this file.
 */
static int zx_oneshot_init(struct zx_priv *priv)
{
	phys_addr_t pool = priv->pool_phys;
	int q, ret;

	/* 1. power-on-reset pulse for the whole SoC fabric */
	zx_pon_pulse(priv, 0xffffffff);

	/* 2. RED (queue scheduler) */
	ret = zx_red_init(priv);
	if (ret) {
		dev_err(priv->dev, "RED init failed: %d\n", ret);
		return ret;
	}

	/* 3. DMA configuration */
	zx_dma_enable(priv, true);
	writel(pool + POOL_STATUS_BASE, priv->dma + 0x50);
	writel(pool + POOL_TX_BASE, priv->dma + 0x60);
	writel(0x400040, priv->dma + 0x3c);
	memset_io(priv->pool_virt + POOL_STATUS_BASE, 0, POOL_STATUS_SIZE);

	/* 4./5. BMU init + enable */
	ret = zx_bmu_init(priv);
	if (ret)
		return ret;
	writel(1, priv->bmu + 0x00);

	/* 6. traffic manager: RX ring base + enable */
	writel(pool + POOL_RX_BASE, priv->tm + 0xf0);
	writel(0x10, priv->tm + 0x04);

	/* 7. DMA channel routing (U-Boot dma_ch_cfg) + RX ring table */
	writel(0xd0010203, priv->pp3 + 0x78);
	writel(0x000000d0, priv->pp3 + 0x7c);
	for (q = 0; q < ZX_RX_QUEUES; q++) {
		priv->rx_ring_phys[q] = pool + POOL_RX_BASE +
					q * POOL_RX_QSTRIDE;
		priv->rx_cursor[q] = 0;
		memset_io(priv->pool_virt + POOL_RX_BASE +
			  q * POOL_RX_QSTRIDE, 0, ZX_RX_DESCS * 16);
	}

	/* 8. */
	writel(0, priv->blk4c + 0x04);

	/* 9. packet processor */
	zx_pp_init(priv);

	/* 10. NPU packet processor */
	zx_npp_init(priv);

	/* 11. */
	writel(0x11, priv->pp3 + 0xe0);

	/* 12. CRM */
	writel(readl(priv->crm + 0x18) & ~3, priv->crm + 0x18);
	writel(0xf, priv->crm + 0x1c);
	mdelay(20);

	priv->hw_init_done = true;
	return 0;
}

/* -------------------------------------------------------------------------
 * MAC port bring-up (U-Boot smac_bringup @ 0x47f309c0)
 * ---------------------------------------------------------------------- */

/*
 * zx_mac_enable() - turn a GEMAC port on
 * @priv: driver private data
 * @p:    port 0..3
 *
 * Sets bits 0|1 of MAC_CTRL (TX+RX enable). U-Boot always sets both
 * together after programming the port.
 */
static void zx_mac_enable(struct zx_priv *priv, int p)
{
	writel(readl(priv->mac[p] + MAC_CTRL) | 3, priv->mac[p] + MAC_CTRL);
}

/*
 * zx_mac_disable() - turn a GEMAC port off
 * @priv: driver private data
 * @p:    port 0..3
 *
 * Clears bit 0 of MAC_CTRL. Used by zx_stop() on interface down.
 */
static void zx_mac_disable(struct zx_priv *priv, int p)
{
	writel(readl(priv->mac[p] + MAC_CTRL) & ~1, priv->mac[p] + MAC_CTRL);
}

/*
 * zx_mac_speed_duplex() - encode speed/duplex into MAC_CTRL
 * @priv:   driver private data
 * @p:      port 0..3
 * @speed:  0 = 10M, 1 = 100M, 2 = 1G
 * @duplex: 0 = half, 1 = full
 *
 * U-Boot mac_speed_duplex @ 0x47f30950, exact bit ops from the
 * disassembly. MAC_CTRL bit 15 (0x8000) = half-duplex, bit 13 (0x2000)
 * = gigabit, bit 14 (0x4000) = 100M. Gigabit clears half-duplex and
 * sets bit 13 (skipping the duplex argument entirely); otherwise
 * half-duplex sets 0x8000 / full sets 0xa000, then bit 14 follows the
 * 100M choice. The odd 0xa000 for full-duplex non-gigabit is verbatim
 * from U-Boot.
 */
static void zx_mac_speed_duplex(struct zx_priv *priv, int p, u32 speed,
				u32 duplex)
{
	void __iomem *mac = priv->mac[p];
	u32 v = readl(mac + MAC_CTRL);

	if (speed == 2) {		/* gigabit */
		v &= ~0x8000;
		v |= 0x2000;
	} else {
		if (duplex == 0) {
			v &= ~0x2000;
			v |= 0x8000;
		} else {
			v |= 0xa000;
		}
		if (speed == 1)		/* 100M */
			v |= 0x4000;
		else
			v &= ~0x4000;
	}
	writel(v, mac + MAC_CTRL);
}

/*
 * zx_smac_bringup() - (re)program one MAC from its GEPHY link state
 * @priv: driver private data
 * @p:    port 0..3 (GEPHY sits at MDIO address 10+p)
 *
 * U-Boot smac_bringup @ 0x47f309c0. The integrated GEPHYs report link
 * state in a vendor register, not standard BMSR: read reg 0x1e (page
 * window), save it, force page 0, clock one dummy read of 0x1a, then
 * re-read 0x1a - bit 6 is link, bits 10:7 encode speed+duplex. Restore
 * 0x1e afterwards. If the raw code is unchanged since last time, just
 * re-enable the MAC and return (cheap path for the 1 Hz poll).
 * Otherwise: decode idx = raw>>7 into speed_arg (0/1/2 for
 * 10/100/1000M, idx&1 = duplex), pulse the per-port PON reset
 * (bit p+6), write the fixed GEMAC register set (0xbbe003 control,
 * 0x80000001/0xfffe FIFO thresholds, 0x11200, 50/0xa8 preamble/IPG,
 * 0x300002, 0x4000, 0x10ff11), apply speed/duplex, acknowledge the
 * link-change in NPP4+0x68 (bit p+5 set, respond with bit p), enable.
 */
static bool zx_smac_bringup(struct zx_priv *priv, int p)
{
	struct mii_bus *bus = priv->mdio;
	void __iomem *mac = priv->mac[p];
	u32 speed_raw, idx, speed_arg, duplex, v;
	int save, ret, i;

	/* page-window select + read must not interleave with phylib polling */
	mutex_lock(&bus->mdio_lock);
	save = __mdiobus_read(bus, ZX_PHY_ADDR_BASE + p, GEPHY_REG1E);
	if (save < 0) {
		mutex_unlock(&bus->mdio_lock);
		return priv->link_up[p];
	}

	__mdiobus_write(bus, ZX_PHY_ADDR_BASE + p, GEPHY_REG1E, 0);
	__mdiobus_read(bus, ZX_PHY_ADDR_BASE + p, GEPHY_REG1A); /* discard */
	mdelay(10);
	ret = __mdiobus_read(bus, ZX_PHY_ADDR_BASE + p, GEPHY_REG1A);
	__mdiobus_write(bus, ZX_PHY_ADDR_BASE + p, GEPHY_REG1E, save);
	mutex_unlock(&bus->mdio_lock);
	if (ret < 0)
		return priv->link_up[p];

	if (!(ret & GEPHY_1A_LINK)) {
		/* force a full MAC reprogram when the link comes back */
		priv->speed_cache[p] = -1;
		priv->link_up[p] = false;
		return false;
	}
	priv->link_up[p] = true;

	speed_raw = ret & GEPHY_1A_SPEED_MASK;
	if (speed_raw == (u32)priv->speed_cache[p]) {
		zx_mac_enable(priv, p);
		return true;
	}

	idx = speed_raw >> GEPHY_1A_SPEED_SHIFT;
	if (idx <= 1)
		speed_arg = 0;		/* 10M  */
	else if (idx <= 3)
		speed_arg = 1;		/* 100M */
	else if (idx <= 5)
		speed_arg = 2;		/* 1G   */
	else
		speed_arg = 0;
	duplex = idx & 1;

	netdev_info(priv->ports[p], "mac %d phy status changed: %s %s\n", p,
		    zx_speed_str[speed_arg], zx_duplex_str[duplex]);

	/* reset just this MAC slice */
	zx_pon_pulse(priv, 1 << (p + 6));

	writel(0xbbe003, mac + MAC_CTRL);
	writel(0x80000001, mac + MAC_REG08);
	writel(0xfffe, mac + MAC_REG04);
	writel(0x11200, mac + MAC_REGE0);
	writel(50, mac + MAC_REGD00);
	writel(0xa8, mac + MAC_REGD30);
	writel(0x300002, mac + MAC_REG70);
	writel(0x4000, mac + MAC_REGB4);
	writel(0x10ff11, mac + MAC_REGB00);

	zx_mac_speed_duplex(priv, p, speed_arg, duplex);

	/* acknowledge link change in NPP4 (5 x 1 ms polls) */
	for (i = 0; i < 5; i++) {
		mdelay(1);
		v = readl(priv->npp4 + 0x68);
		if (v & (1 << (p + 5))) {
			writel(v | (1 << p), priv->npp4 + 0x68);
			break;
		}
	}

	zx_mac_enable(priv, p);
	priv->speed_cache[p] = speed_raw;
	return true;
}

/* -------------------------------------------------------------------------
 * Link state work
 * ---------------------------------------------------------------------- */

/*
 * zx_link_work() - 1 Hz link poll and per-port carrier
 * @work: delayed work item embedded in struct zx_priv
 *
 * The stock driver polls link state in its main loop; there is no
 * usable PHY interrupt path we recovered, so this workqueue item runs
 * zx_smac_bringup() for every open port once per second and sets that
 * port's carrier from the vendor GEPHY status register (the same
 * source that programs the MAC), not phylib's generic BMSR view.
 */
static void zx_link_work(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct zx_priv *priv = container_of(dwork, struct zx_priv, link_work);
	int p;

	for (p = 0; p < ZX_NR_PORTS; p++) {
		struct net_device *dev = priv->ports[p];
		bool up;

		if (!(priv->open_ports & BIT(p)))
			continue;

		up = zx_smac_bringup(priv, p);
		if (up && !netif_carrier_ok(dev))
			netif_carrier_on(dev);
		else if (!up && netif_carrier_ok(dev))
			netif_carrier_off(dev);
	}

	schedule_delayed_work(&priv->link_work, HZ);
}

/*
 * zx_phy_adjust_link() - phylib link-change callback
 * @dev: net device (unused)
 *
 * Registered with phy_connect() for each port. We deliberately do
 * nothing here: MAC speed/duplex is applied by zx_smac_bringup()
 * reading the vendor GEPHY status register directly (the standard
 * phydev->speed/duplex values come from standard MII registers, which
 * the integrated GEPHYs do not populate the way we need). The 1 Hz
 * link work picks up changes instead.
 */
static void zx_phy_adjust_link(struct net_device *dev)
{
	/* state is consumed by the periodic link work; nothing to do here */
}

/* -------------------------------------------------------------------------
 * NAPI / hrtimer datapath
 * ---------------------------------------------------------------------- */

/*
 * zx_dma_completed() / zx_dma_submitted() - TX descriptor counters
 * @priv: driver private data
 *
 * DMA+0x6c = { completed[31:16], submitted[15:0] }, both free-running
 * u16. Measured on hardware 2026-10-02: submitted tracked every
 * doorbell 1:1, and U-Boot reads 0x00020002 after two frames.
 * DMA+0x68[31:16] is the outstanding gap; its low half is not a usable
 * completion counter on this engine.
 */
static u16 zx_dma_completed(struct zx_priv *priv)
{
	return readl(priv->dma + 0x6c) >> 16;
}

static u16 zx_dma_submitted(struct zx_priv *priv)
{
	return readl(priv->dma + 0x6c) & 0xffff;
}

/*
 * zx_tx_reap() - retire completed TX descriptors
 * @priv: driver private data
 *
 * The u16 delta of the hardware completed counter since the last call
 * is subtracted from tx_pending, clamped so a counter jump can never
 * underflow it. TX buffer-pool ids need no reclaim: hardware returns
 * the BP to the BMU itself once the frame is sent. When the queue was
 * stopped for backpressure and the ring has drained below 0x300 (768)
 * entries, wake it - the 0x400/0x300 high/low-water pair U-Boot uses.
 * Caller holds bp_lock so pending/cursor updates serialise against
 * zx_start_xmit.
 */
static void zx_tx_reap(struct zx_priv *priv)
{
	u16 completed = zx_dma_completed(priv);
	u32 done = (u16)(completed - priv->tx_last_consumed);

	if (done > priv->tx_pending) {
		priv->ports[0]->stats.tx_carrier_errors++;	/* hw completed more than queued */
		done = priv->tx_pending;
	}
	priv->tx_pending -= done;
	priv->tx_last_consumed = completed;

	if (priv->tx_stopped && priv->tx_pending < 0x300)
		zx_tx_wake_all(priv);
}

/*
 * zx_poll() - NAPI receive path
 * @napi:   NAPI context embedded in struct zx_priv
 * @weight: budget for this pass
 *
 * Polls the eight RX completion counters (DMA+0x100+q*4, low 16 bits =
 * frames pending in ring q) from queue 7 down to 0. For each pending
 * frame: take the descriptor at the software cursor, decode the BP id
 * from descriptor bytes 7/8 (bp = byte7>>1 | byte8<<7 - the id is
 * split across two bytes by hardware), copy the payload out of the
 * uncached pool buffer (data starts at BP+0x10, length = u16 at
 * desc+0xc divided by 4), hand the skb up, then release the descriptor
 * back to RED using the queue id encoded in descriptor byte 3
 * ((byte3>>2)&7, which can differ from the ring id) and return the BP
 * to the BMU. Invalid BP or length just drops the frame with stats.
 * Below budget NAPI completes. In poll mode the hrtimer re-kicks it
 * whenever counters are non-zero; in IRQ mode the TM interrupt is
 * unmasked again after completion (stock pon_tm_net_poll), and the
 * level-type status fires at once if a frame arrived meanwhile.
 */
static int zx_poll(struct napi_struct *napi, int weight)
{
	struct zx_priv *priv = container_of(napi, struct zx_priv, napi);
	int work = 0;
	int q;

	spin_lock(&priv->bp_lock);
	zx_tx_reap(priv);
	spin_unlock(&priv->bp_lock);

	for (q = ZX_RX_QUEUES - 1; q >= 0 && work < weight; q--) {
		u32 cnt = readl(priv->dma + 0x100 + q * 4) & 0xffff;

		while (cnt && work < weight) {
			void __iomem *desc;
			void __iomem *buf;
			struct net_device *dev;
			struct sk_buff *skb;
			u32 bp, len, b6;
			int port;

			cnt--;
			work++;

			desc = priv->pool_virt +
			       (priv->rx_ring_phys[q] - priv->pool_phys) +
			       priv->rx_cursor[q] * 16;
			priv->rx_cursor[q] = (priv->rx_cursor[q] + 1) &
					     (ZX_RX_DESCS - 1);

			bp = (readb(desc + 7) >> 1) | (readb(desc + 8) << 7);
			if (bp >= POOL_BP_COUNT) {
				dev_dbg(priv->dev, "invalid rx bp %u\n", bp);
				zx_desc_release(priv, q, 0, 1, 0);
				continue;
			}

			/* ingress front port = desc byte6[7:3] - 1 (stock pon_tm_net_poll) */
			b6 = readb(desc + 6);
			port = (readb(desc + 0xe) & 2) ? -1 : (int)(b6 >> 3) - 1;
			dev = (port >= 0 && port < ZX_NR_PORTS &&
			       (priv->open_ports & BIT(port))) ? priv->ports[port] : NULL;

			len = (readw(desc + 0xc) & 0xffff) >> 2;
			buf = priv->pool_virt + bp * POOL_BP_SIZE +
			      POOL_BP_DATA_OFF;

			if (!dev || (b6 & 4)) {
				priv->ports[0]->stats.rx_dropped++;
			} else if (len >= 60 && len <= POOL_BP_SIZE - POOL_BP_DATA_OFF) {
				skb = napi_alloc_skb(napi, len);
				if (skb) {
					memcpy_fromio(skb->data, buf, len);
					skb_put(skb, len);
					skb->protocol = eth_type_trans(skb,
									dev);
					napi_gro_receive(napi, skb);
					dev->stats.rx_packets++;
					dev->stats.rx_bytes += len;
				} else {
					dev->stats.rx_dropped++;
				}
			} else {
				dev->stats.rx_dropped++;
			}

			if (zx_desc_release(priv, (readb(desc + 3) >> 2) & 7,
					    0, 1, 0))
				priv->ports[0]->stats.rx_dropped++;

			spin_lock(&priv->bp_lock);
			if (zx_bp_release(priv, bp))
				priv->ports[0]->stats.rx_dropped++;
			spin_unlock(&priv->bp_lock);
		}
	}

	if (work < weight && napi_complete_done(napi, work) && priv->irq_mode)
		writel(readl(priv->tm + ZX_TM_INT_MASK) & ~ZX_TM_INT_RX,
		       priv->tm + ZX_TM_INT_MASK);

	return work;
}

/*
 * zx_irq() - TM interrupt (stock zx_pon_tm_int + pon_tm_net_int)
 *
 * Status = TM+0x100 & 3 & ~mask. Mask everything and let NAPI drain the
 * queues; zx_poll() unmasks again when it is done. The stock driver never
 * writes an acknowledge, so the status is assumed level-type.
 */
static irqreturn_t zx_irq(int irq, void *data)
{
	struct zx_priv *priv = data;
	u32 mask = readl(priv->tm + ZX_TM_INT_MASK);

	if (!(readl(priv->tm + ZX_TM_INT_STATUS) & ZX_TM_INT_RX & ~mask))
		return IRQ_NONE;

	writel(mask | ZX_TM_INT_ALL, priv->tm + ZX_TM_INT_MASK);
	napi_schedule(&priv->napi);

	return IRQ_HANDLED;
}

/*
 * zx_poll_timer_fn() - NAPI kick timer
 * @timer: hrtimer embedded in struct zx_priv
 *
 * Poll mode: replacement for the interrupt the stock driver uses. In IRQ
 * mode it only runs every ZX_POLL_IRQ_MS as a safety net and for TX
 * completions. Every tick: if any RX counter is non-zero,
 * the TX queue is stopped, or TX completions are outstanding, schedule
 * NAPI. Cheap (8 register reads). At 10 ms a lock-step protocol such as
 * TFTP was limited to one block per 10 ms (~135 KiB/s). Runs in softirq
 * context via hrtimer, hence only register reads here - all datapath
 * work happens in zx_poll().
 */
static enum hrtimer_restart zx_poll_timer_fn(struct hrtimer *timer)
{
	struct zx_priv *priv = container_of(timer, struct zx_priv, poll_timer);
	bool rx_busy = false;
	int q;

	for (q = 0; q < ZX_RX_QUEUES; q++)
		if (readl(priv->dma + 0x100 + q * 4) & 0xffff)
			rx_busy = true;

	if (rx_busy || priv->tx_stopped ||
	    priv->tx_pending)
		napi_schedule(&priv->napi);

	hrtimer_forward_now(timer, ms_to_ktime(priv->irq_mode ?
						ZX_POLL_IRQ_MS : ZX_POLL_MS));
	return HRTIMER_RESTART;
}

/* -------------------------------------------------------------------------
 * netdev operations
 * ---------------------------------------------------------------------- */

/*
 * zx_open() - ndo_open of one front-port netdev
 * @dev: net device (port idx in struct zx_port)
 *
 * The first port brought up runs the full U-Boot-order one-shot init
 * (~2 s, includes the PP/NPP settle sleeps) and starts NAPI, the 1 ms
 * poll timer and the 1 Hz link work; later ports only attach. Each port
 * attaches its own integrated GEPHY through phylib by bus-id
 * ("<mdio bus id>:<10+port>"). PHY_INTERFACE_MODE_GMII: the MAC-to-GEPHY
 * glue is GMII inside the SoC and is fixed by hardware. The TX ring
 * state is reset against the hardware's current consumed-counter value
 * when the datapath starts, then the port gets an immediate
 * zx_smac_bringup().
 */
static int zx_open(struct net_device *dev)
{
	struct zx_port *zp = netdev_priv(dev);
	struct zx_priv *priv = zp->priv;
	char phy_id[MII_BUS_ID_SIZE + 3];
	int p = zp->idx, ret;

	if (!priv->hw_init_done) {
		ret = zx_oneshot_init(priv);
		if (ret)
			return ret;
	}

	snprintf(phy_id, sizeof(phy_id), "%s:%02x", priv->mdio->id,
		 ZX_PHY_ADDR_BASE + p);
	priv->phy[p] = phy_connect(dev, phy_id, zx_phy_adjust_link,
				   PHY_INTERFACE_MODE_GMII);
	if (IS_ERR(priv->phy[p])) {
		ret = PTR_ERR(priv->phy[p]);
		netdev_err(dev, "no phy at %s: %d\n", phy_id, ret);
		priv->phy[p] = NULL;
		return ret;
	}
	priv->phy[p]->autoneg = AUTONEG_ENABLE;
	phy_start(priv->phy[p]);

	netif_carrier_off(dev);

	if (!priv->open_ports) {
		/* hardware ring pointer survives ifdown/ifup: resume where it is */
		priv->tx_cursor = zx_dma_submitted(priv) & (ZX_TX_DESCS - 1);
		priv->tx_last_consumed = zx_dma_completed(priv);
		priv->tx_pending = (u16)(zx_dma_submitted(priv) -
					 priv->tx_last_consumed);
		priv->tx_stopped = false;
		napi_enable(&priv->napi);
		priv->irq_mode = false;
		if (use_irq && priv->irq <= 0) {
			dev_warn(priv->dev, "use_irq set but no interrupt in DT\n");
		} else if (use_irq) {
			writel(readl(priv->tm + ZX_TM_INT_MASK) | ZX_TM_INT_ALL,
			       priv->tm + ZX_TM_INT_MASK);
			ret = request_irq(priv->irq, zx_irq, 0, "zx279128-eth",
					  priv);
			if (ret) {
				dev_warn(priv->dev, "request_irq failed: %d\n", ret);
			} else {
				priv->irq_mode = true;
				writel(readl(priv->tm + ZX_TM_INT_MASK) &
				       ~ZX_TM_INT_RX,
				       priv->tm + ZX_TM_INT_MASK);
			}
		}
		hrtimer_start(&priv->poll_timer,
			      ms_to_ktime(priv->irq_mode ? ZX_POLL_IRQ_MS :
					  ZX_POLL_MS),
			      HRTIMER_MODE_REL);
		schedule_delayed_work(&priv->link_work, HZ);
	}
	priv->open_ports |= BIT(p);

	if (zx_smac_bringup(priv, p))
		netif_carrier_on(dev);

	netif_start_queue(dev);
	return 0;
}

/*
 * zx_stop() - ndo_stop of one front-port netdev
 * @dev: net device
 *
 * TX first, then this port's PHY and MAC enable. When the last port goes
 * down the timers and NAPI stop too. The one-shot fabric init is NOT
 * undone - hw_init_done stays set and a subsequent ifup skips it,
 * matching how U-Boot leaves the fabric powered between uses.
 */
static int zx_stop(struct net_device *dev)
{
	struct zx_port *zp = netdev_priv(dev);
	struct zx_priv *priv = zp->priv;
	int p = zp->idx;

	netif_tx_disable(dev);
	priv->open_ports &= ~BIT(p);

	if (!priv->open_ports) {
		if (priv->irq_mode) {
			writel(readl(priv->tm + ZX_TM_INT_MASK) | ZX_TM_INT_ALL,
			       priv->tm + ZX_TM_INT_MASK);
			free_irq(priv->irq, priv);
			priv->irq_mode = false;
		}
		hrtimer_cancel(&priv->poll_timer);
		cancel_delayed_work_sync(&priv->link_work);
		napi_disable(&priv->napi);
	}

	if (priv->phy[p]) {
		phy_stop(priv->phy[p]);
		phy_disconnect(priv->phy[p]);
		priv->phy[p] = NULL;
	}

	mdelay(100);
	zx_mac_disable(priv, p);
	priv->link_up[p] = false;
	priv->speed_cache[p] = -1;

	netif_carrier_off(dev);
	return 0;
}

/*
 * zx_start_xmit() - ndo_start_xmit: copy out and ring the doorbell
 * @skb: frame to send
 * @dev: net device
 *
 * U-Boot tx_caller @ 0x47f305d0 + tx_fill @ 0x47f304ac. Under bp_lock:
 * reap completions, refuse with NETDEV_TX_BUSY past the 768-entry
 * high-water, prefill the next TX descriptor with the stock template
 * {0x80, 0x10000, 0x01000000, 2}, and allocate a BP. The frame is
 * memcpy_toio'd into the uncached pool buffer (BP+0x10), then the
 * descriptor is patched exactly like U-Boot's bfi sequence: BP id
 * split into bytes 7/8, length<<2 into the u16 at +0xc, length<<9
 * into word 2 of +0x8, and byte 11 gets 0x20 (the "ready" flag) while
 * preserving bit 0. A single write of 1 to DMA+0x64 is the doorbell;
 * the engine walks the ring in order. DMA+0x1fc bits 0/1 are error
 * latches - logged, not fatal. TX BPs are freed by hardware after
 * transmission, so there is no per-packet reclaim beyond the consumed
 * counter in zx_tx_reap(). skb is always consumed here (TX_BUSY paths
 * return before the copy).
 */
static netdev_tx_t zx_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct zx_port *zp = netdev_priv(dev);
	struct zx_priv *priv = zp->priv;
	void __iomem *desc;
	void __iomem *buf;
	unsigned long flags;
	u32 len, bp;
	int ret;

	if (skb_put_padto(skb, ETH_ZLEN)) {
		dev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
	len = skb->len;
	if (len > POOL_BP_SIZE - POOL_BP_DATA_OFF) {
		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	spin_lock_irqsave(&priv->bp_lock, flags);
	zx_tx_reap(priv);
	if (priv->tx_pending >= ZX_TX_DESCS - ZX_TX_DESCS / 4) {
		spin_unlock_irqrestore(&priv->bp_lock, flags);
		dev->stats.tx_fifo_errors++;
		if (net_ratelimit())
			netdev_warn(dev, "tx ring full: pending %u dma68 %08x dma6c %08x\n",
				    priv->tx_pending, readl(priv->dma + 0x68),
				    readl(priv->dma + 0x6c));
		zx_tx_stop_all(priv);
		return NETDEV_TX_BUSY;
	}

	desc = priv->tx_ring + priv->tx_cursor * 16;
	priv->tx_cursor = (priv->tx_cursor + 1) & (ZX_TX_DESCS - 1);

	writel(0x80 | (tx_directed ? (tx_port_code[zp->idx] & 0x3f) << 20 : 0),
	       desc + 0x00);
	writel(0x10000, desc + 0x04);
	writel(0x01000000, desc + 0x08);
	writel(tx_directed ? tx_w3 : 2, desc + 0x0c);

	ret = zx_bmu_alloc(priv);
	if (ret < 0)
		priv->tx_cursor = (priv->tx_cursor - 1) & (ZX_TX_DESCS - 1);
	spin_unlock_irqrestore(&priv->bp_lock, flags);

	if (ret < 0) {
		dev->stats.tx_aborted_errors++;
		if (net_ratelimit())
			netdev_warn(dev, "tx bmu alloc failed\n");
		zx_tx_stop_all(priv);
		return NETDEV_TX_BUSY;
	}
	bp = ret;

	if (bp >= POOL_BP_COUNT) {
		if (net_ratelimit())
			netdev_err(dev, "invalid tx bp %u\n", bp);
		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}

	buf = priv->pool_virt + bp * POOL_BP_SIZE + POOL_BP_DATA_OFF;
	memcpy_toio(buf, skb->data, len);

	/* patch descriptor exactly like U-Boot tx_fill @ 0x47f304ac */
	writeb((readb(desc + 7) & 0x01) | ((bp & 0x7f) << 1), desc + 7);
	writeb(bp >> 7, desc + 8);
	writew((readw(desc + 0xc) & 3) | (len << 2), desc + 0xc);
	writel((readl(desc + 8) & ~(0x3fff << 9)) | (len << 9), desc + 8);
	writeb((readb(desc + 0xb) & 0x01) | 0x20, desc + 0xb);

	if (readl(priv->dma + 0x1fc) & 3) {
		static bool dumped;

		if (!dumped) {
			dumped = true;
			netdev_err(dev, "first dma err %08x len %u d=%08x %08x %08x %08x 68=%08x 6c=%08x\n",
				   readl(priv->dma + 0x1fc), len,
				   readl(desc), readl(desc + 4), readl(desc + 8),
				   readl(desc + 0xc), readl(priv->dma + 0x68),
				   readl(priv->dma + 0x6c));
		} else if (net_ratelimit()) {
			netdev_err(dev, "dma err %08x\n",
				   readl(priv->dma + 0x1fc));
		}
	}

	writel(1, priv->dma + 0x64);	/* doorbell */

	priv->tx_pending++;
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += len;
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

static void zx_tx_timeout(struct net_device *dev, unsigned int txqueue)
{
	struct zx_priv *priv = ((struct zx_port *)netdev_priv(dev))->priv;

	netdev_err(dev, "tx timeout: pending %u cursor %u dma68 %08x dma6c %08x dma1fc %08x\n",
		   priv->tx_pending, priv->tx_cursor, readl(priv->dma + 0x68),
		   readl(priv->dma + 0x6c), readl(priv->dma + 0x1fc));
	dev->stats.tx_errors++;
}

static const struct net_device_ops zx_netdev_ops = {
	.ndo_open		= zx_open,
	.ndo_stop		= zx_stop,
	.ndo_start_xmit		= zx_start_xmit,
	.ndo_tx_timeout		= zx_tx_timeout,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};

/* -------------------------------------------------------------------------
 * Platform driver
 * ---------------------------------------------------------------------- */

static void *zx_ioremap(struct device *dev, phys_addr_t base)
{
	return devm_ioremap(dev, base, ZX_WIN_SIZE);
}

/*
 * zx_probe() - platform probe for "zte,zx279128-eth"
 * @pdev: platform device created from the DT node
 *
 * Acquires the two DT-supplied resources - the reserved-memory buffer
 * pool (phandle "memory-region", looked up via of_reserved_mem_lookup
 * so we get the base/size the CMA/OF allocator actually honoured) and
 * the MDIO bus (phandle "zte,mdio", -EPROBE_DEFER until the MDIO
 * driver has registered it) - then ioremaps every fixed-address
 * datapath block including the four MAC windows. The pool is ioremap'd
 * plain (not cached): the DMA engines here are cache-unaware and
 * U-Boot drove the identical layout uncached, so we keep coherency by
 * construction instead of scatter/gather bookkeeping. MAC address from
 * DT, else random locally-administered. No hardware is touched at
 * probe time - everything waits for zx_open() so a downed interface
 * costs nothing.
 */
static int zx_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct zx_priv *priv;
	struct reserved_mem *rmem;
	struct device_node *region_np;
	struct device_node *mdio_np;
	u8 addr[ETH_ALEN];
	int p, ret;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = &pdev->dev;
	spin_lock_init(&priv->bp_lock);

	/* MAC address: factory tag via nvmem, else random; looked up first so a deferral leaks nothing */
	ret = of_get_mac_address(np, addr);
	if (ret == -EPROBE_DEFER)
		return ret;
	if (ret) {
		dev_warn(&pdev->dev, "no factory MAC (%d), using a random address\n",
			 ret);
		eth_random_addr(addr);
	}

	/* reserved buffer pool */
	region_np = of_parse_phandle(np, "memory-region", 0);
	if (!region_np) {
		dev_err(&pdev->dev, "missing memory-region\n");
		return -EINVAL;
	}
	rmem = of_reserved_mem_lookup(region_np);
	of_node_put(region_np);
	if (!rmem) {
		dev_err(&pdev->dev, "reserved pool not found\n");
		return -ENODEV;
	}
	priv->pool_phys = rmem->base;
	priv->pool_virt = devm_ioremap(&pdev->dev, rmem->base, rmem->size);
	if (!priv->pool_virt) {
		dev_err(&pdev->dev, "cannot ioremap pool\n");
		return -ENOMEM;
	}

	/* MDIO bus */
	mdio_np = of_parse_phandle(np, "zte,mdio", 0);
	if (!mdio_np) {
		dev_err(&pdev->dev, "missing zte,mdio phandle\n");
		return -EINVAL;
	}
	priv->mdio = of_mdio_find_bus(mdio_np);
	of_node_put(mdio_np);
	if (!priv->mdio)
		return -EPROBE_DEFER;

	/* fixed hardware blocks */
	priv->pon  = zx_ioremap(&pdev->dev, ZX_PON_BASE);
	priv->crm  = zx_ioremap(&pdev->dev, ZX_CRM_BASE);
	priv->npp  = zx_ioremap(&pdev->dev, ZX_NPP_BASE);
	priv->npp2 = zx_ioremap(&pdev->dev, ZX_NPP2_BASE);
	priv->npp3 = zx_ioremap(&pdev->dev, ZX_NPP3_BASE);
	priv->npp4 = zx_ioremap(&pdev->dev, ZX_NPP4_BASE);
	priv->tm   = zx_ioremap(&pdev->dev, ZX_TM_BASE);
	priv->red  = zx_ioremap(&pdev->dev, ZX_RED_BASE);
	priv->bmu  = zx_ioremap(&pdev->dev, ZX_BMU_BASE);
	priv->blk4c = zx_ioremap(&pdev->dev, ZX_BLK4C_BASE);
	priv->dma  = zx_ioremap(&pdev->dev, ZX_DMA_BASE);
	priv->pp0  = zx_ioremap(&pdev->dev, ZX_PP0_BASE);
	priv->pp1  = zx_ioremap(&pdev->dev, ZX_PP1_BASE);
	priv->pp2  = zx_ioremap(&pdev->dev, ZX_PP2_BASE);
	priv->pp3  = zx_ioremap(&pdev->dev, ZX_PP3_BASE);
	for (p = 0; p < ZX_NR_PORTS; p++)
		priv->mac[p] = zx_ioremap(&pdev->dev,
					  ZX_MAC_BASE + p * ZX_MAC_STRIDE);

	if (!priv->pon || !priv->crm || !priv->npp || !priv->npp2 ||
	    !priv->npp3 || !priv->npp4 || !priv->tm || !priv->red ||
	    !priv->bmu || !priv->blk4c || !priv->dma || !priv->pp0 ||
	    !priv->pp1 || !priv->pp2 || !priv->pp3 ||
	    !priv->mac[0] || !priv->mac[1] || !priv->mac[2] ||
	    !priv->mac[3]) {
		dev_err(&pdev->dev, "cannot ioremap hardware blocks\n");
		return -ENOMEM;
	}

	priv->tx_ring = priv->pool_virt + POOL_TX_BASE;

	/* optional: only used when use_irq is set */
	priv->irq = platform_get_irq_optional(pdev, 0);

	priv->napi_dev = alloc_netdev_dummy(0);
	if (!priv->napi_dev)
		return -ENOMEM;
	netif_napi_add(priv->napi_dev, &priv->napi, zx_poll);
	hrtimer_init(&priv->poll_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	priv->poll_timer.function = zx_poll_timer_fn;
	INIT_DELAYED_WORK(&priv->link_work, zx_link_work);

	platform_set_drvdata(pdev, priv);

	for (p = 0; p < ZX_NR_PORTS; p++) {
		struct net_device *dev;
		struct zx_port *zp;

		u8 mac[ETH_ALEN];

		dev = devm_alloc_etherdev(&pdev->dev, sizeof(*zp));
		if (!dev) {
			ret = -ENOMEM;
			goto err_unreg;
		}
		SET_NETDEV_DEV(dev, &pdev->dev);
		zp = netdev_priv(dev);
		zp->priv = priv;
		zp->idx = p;
		/* LAN jacks share the tag MAC; WAN gets the next one (as the tag's second record) */
		ether_addr_copy(mac, addr);
		if (p == ZX_WAN_PORT)
			eth_addr_inc(mac);
		eth_hw_addr_set(dev, mac);
		dev->netdev_ops = &zx_netdev_ops;
		dev->watchdog_timeo = HZ;
		priv->ports[p] = dev;

		ret = register_netdev(dev);
		if (ret) {
			priv->ports[p] = NULL;
			goto err_unreg;
		}
	}

	dev_info(&pdev->dev, "%d front-port netdevs, pool at 0x%pa size 0x%x\n",
		 ZX_NR_PORTS, &priv->pool_phys, (unsigned int)rmem->size);
	return 0;

err_unreg:
	while (--p >= 0)
		unregister_netdev(priv->ports[p]);
	netif_napi_del(&priv->napi);
	free_netdev(priv->napi_dev);
	return ret;
}

/*
 * zx_remove() - platform remove
 * @pdev: platform device
 *
 * unregister_netdev() runs ndo_stop (zx_stop) for ports that are up; the
 * last one cancels the timer and link work, the explicit cancels here are
 * belt-and-braces for the all-down case. Port netdevs and ioremaps are
 * devm-managed; the dummy NAPI netdev is freed here.
 */
static void zx_remove(struct platform_device *pdev)
{
	struct zx_priv *priv = platform_get_drvdata(pdev);
	int p;

	for (p = ZX_NR_PORTS - 1; p >= 0; p--)
		unregister_netdev(priv->ports[p]);
	hrtimer_cancel(&priv->poll_timer);
	cancel_delayed_work_sync(&priv->link_work);
	netif_napi_del(&priv->napi);
	free_netdev(priv->napi_dev);
}

static const struct of_device_id zx_match[] = {
	{ .compatible = "zte,zx279128-eth" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx_match);

static struct platform_driver zx_driver = {
	.probe	= zx_probe,
	.remove	= zx_remove,
	.driver	= {
		.name		= "zx279128-eth",
		.of_match_table	= zx_match,
	},
};
module_platform_driver(zx_driver);

MODULE_DESCRIPTION("ZTE ZX279128S Ethernet MAC driver");
MODULE_AUTHOR("OpenWrt zx279128 port");
MODULE_LICENSE("GPL");
