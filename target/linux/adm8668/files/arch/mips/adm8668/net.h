/*
 * originally drivers/net/tulip/tulip.h
 *   Copyright 2000,2001  The Linux Kernel Team
 *   Written/copyright 1994-2001 by Donald Becker.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef __NET_TULIP_H__
#define __NET_TULIP_H__

#include <linux/module.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/mii.h>
#include <linux/crc32.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <asm/unaligned.h>
#include <asm/uaccess.h>

/* undefine, or define to various debugging levels (>4 == obscene levels) */
#define TULIP_DEBUG 1
#define	VALID_INTR	0x0001a451
#define	ADM8668_WAN_IRQ		8
#define	ADM8668_LAN_IRQ		7
#define	ADM8668_WAN_MACADDR	0xb00205ac
#define	ADM8668_LAN_MACADDR	0xb0020404

/* Offsets to the Command and Status Registers, "CSRs".  All accesses
   must be longword instructions and quadword aligned. */
enum tulip_offsets {
	CSR0 = 0,
	CSR1 = 0x08,
	CSR2 = 0x10,
	CSR3 = 0x18,
	CSR4 = 0x20,
	CSR5 = 0x28,
	CSR6 = 0x30,
	CSR7 = 0x38,
	CSR8 = 0x40,
	CSR9 = 0x48,
	CSR10 = 0x50,
	CSR11 = 0x58,
	CSR12 = 0x60,
	CSR13 = 0x68,
	CSR14 = 0x70,
	CSR15 = 0x78,
	CSR18 = 0x88,
	CSR19 = 0x8c,
	CSR20 = 0x90,
	CSR27 = 0xAC,
	CSR28 = 0xB0,
};

#define RxPollInt (RxIntr|RxNoBuf|RxDied|RxJabber)

/* The bits in the CSR5 status registers, mostly interrupt sources. */
enum status_bits {
	TimerInt = 0x800,
	SystemError = 0x2000,
	TPLnkFail = 0x1000,
	TPLnkPass = 0x10,
	NormalIntr = 0x10000,
	AbnormalIntr = 0x8000,
	RxJabber = 0x200,
	RxDied = 0x100,
	RxNoBuf = 0x80,
	RxIntr = 0x40,
	TxFIFOUnderflow = 0x20,
	RxErrIntr = 0x10,
	TxJabber = 0x08,
	TxNoBuf = 0x04,
	TxDied = 0x02,
	TxIntr = 0x01,
};

/* bit mask for CSR5 TX/RX process state */
#define CSR5_TS	0x00700000
#define CSR5_RS	0x000e0000

enum tulip_mode_bits {
	TxThreshold		= (1 << 22),
	FullDuplex		= (1 << 9),
	TxOn			= 0x2000,
	AcceptBroadcast		= 0x0100,
	AcceptAllMulticast	= 0x0080,
	AcceptAllPhys		= 0x0040,
	AcceptRunt		= 0x0008,
	RxOn			= 0x0002,
	RxTx			= (TxOn | RxOn),
};

/* The Tulip Rx and Tx buffer descriptors. */
struct tulip_rx_desc {
	__le32 status;
	__le32 length;
	__le32 buffer1;
	__le32 buffer2;
};

struct tulip_tx_desc {
	__le32 status;
	__le32 length;
	__le32 buffer1;
	__le32 buffer2;		/* We use only buffer 1.  */
};

enum desc_status_bits {
	DescOwned    = 0x80000000,
	DescWholePkt = 0x60000000,
	DescEndPkt   = 0x40000000,
	DescStartPkt = 0x20000000,
	DescEndRing  = 0x02000000,
	DescUseLink  = 0x01000000,

	/*
	 * Error summary flag is logical or of 'CRC Error', 'Collision Seen',
	 * 'Frame Too Long', 'Runt' and 'Descriptor Error' flags generated
	 * within tulip chip.
	 */
	RxDescErrorSummary = 0x8000,
	RxDescCRCError = 0x0002,
	RxDescCollisionSeen = 0x0040,

	/*
	 * 'Frame Too Long' flag is set if packet length including CRC exceeds
	 * 1518.  However, a full sized VLAN tagged frame is 1522 bytes
	 * including CRC.
	 *
	 * The tulip chip does not block oversized frames, and if this flag is
	 * set on a receive descriptor it does not indicate the frame has been
	 * truncated.  The receive descriptor also includes the actual length.
	 * Therefore we can safety ignore this flag and check the length
	 * ourselves.
	 */
	RxDescFrameTooLong = 0x0080,
	RxDescRunt = 0x0800,
	RxDescDescErr = 0x4000,
	RxWholePkt   = 0x00000300,
	/*
	 * Top three bits of 14 bit frame length (status bits 27-29) should
	 * never be set as that would make frame over 2047 bytes. The Receive
	 * Watchdog flag (bit 4) may indicate the length is over 2048 and the
	 * length field is invalid.
	 */
	RxLengthOver2047 = 0x38000010
};

/* Keep the ring sizes a power of two for efficiency.
   Making the Tx ring too large decreases the effectiveness of channel
   bonding and packet priority.
   There are no ill effects from too-large receive rings. */

#define TX_RING_SIZE	32
#define RX_RING_SIZE	128

/* The receiver on the DC21143 rev 65 can fail to close the last
 * receive descriptor in certain circumstances (see errata) when
 * using MWI. This can only occur if the receive buffer ends on
 * a cache line boundary, so the "+ 4" below ensures it doesn't.
 */
#define PKT_BUF_SZ	(1536 + 4)	/* Size of each temporary Rx buffer. */

/* Ring-wrap flag in length field, use for last ring entry.
	0x01000000 means chain on buffer2 address,
	0x02000000 means use the ring start address in CSR2/3.
   Note: Some work-alike chips do not function correctly in chained mode.
   The ASIX chip works only in chained mode.
   Thus we indicates ring mode, but always write the 'next' field for
   chained mode as well.
*/
#define DESC_RING_WRAP 0x02000000

struct ring_info {
	struct sk_buff	*skb;
	dma_addr_t	mapping;
};

struct tulip_private {
	struct tulip_rx_desc *rx_ring;
	struct tulip_tx_desc *tx_ring;
	dma_addr_t rx_ring_dma;
	dma_addr_t tx_ring_dma;
	/* The saved address of a sent-in-place packet/buffer, for skfree(). */
	struct ring_info tx_buffers[TX_RING_SIZE];
	/* The addresses of receive-in-place skbuffs. */
	struct ring_info rx_buffers[RX_RING_SIZE];
	struct napi_struct napi;
	struct net_device_stats stats;
	struct timer_list oom_timer;    /* Out of memory timer. */
	u32 mc_filter[2];
	spinlock_t lock;
	unsigned int cur_rx, cur_tx;	/* The next free ring entry */
	unsigned int dirty_rx, dirty_tx;	/* The ring entries to be free()ed. */
	unsigned int csr0;	/* CSR0 setting. */
	unsigned int csr6;	/* Current CSR6 control settings. */
	void (*link_change) (struct net_device * dev, int csr5);
	struct platform_device *pdev;
	unsigned long nir;
	void __iomem *base_addr;
	int pad0;		/* Used for 8-byte alignment */
	struct net_device *dev;
};


/* interrupt.c */
irqreturn_t tulip_interrupt(int irq, void *dev_instance);
int tulip_refill_rx(struct net_device *dev);
int tulip_poll(struct napi_struct *napi, int budget);

/* tulip_core.c */
extern int tulip_debug;
void oom_timer(unsigned long data);

static inline void tulip_start_rxtx(struct tulip_private *tp)
{
	void __iomem *ioaddr = tp->base_addr;
	iowrite32(tp->csr6 | RxTx, ioaddr + CSR6);
	barrier();
	(void) ioread32(ioaddr + CSR6); /* mmio sync */
}

static inline void tulip_stop_rxtx(struct tulip_private *tp)
{
	void __iomem *ioaddr = tp->base_addr;
	u32 csr6 = ioread32(ioaddr + CSR6);

	if (csr6 & RxTx) {
		unsigned i=1300/10;
		iowrite32(csr6 & ~RxTx, ioaddr + CSR6);
		barrier();
		/* wait until in-flight frame completes.
		 * Max time @ 10BT: 1500*8b/10Mbps == 1200us (+ 100us margin)
		 * Typically expect this loop to end in < 50 us on 100BT.
		 */
		while (--i && (ioread32(ioaddr + CSR5) & (CSR5_TS|CSR5_RS)))
			udelay(10);

		if (!i)
			printk(KERN_DEBUG "fixme: tulip_stop_rxtx() failed"
					" (CSR5 0x%x CSR6 0x%x)\n",
					ioread32(ioaddr + CSR5),
					ioread32(ioaddr + CSR6));
	}
}

static inline void tulip_restart_rxtx(struct tulip_private *tp)
{
	tulip_stop_rxtx(tp);
	udelay(5);
	tulip_start_rxtx(tp);
}

static inline void tulip_tx_timeout_complete(struct tulip_private *tp, void __iomem *ioaddr)
{
	/* Stop and restart the chip's Tx processes. */
	tulip_restart_rxtx(tp);
	/* Trigger an immediate transmit demand. */
	iowrite32(0, ioaddr + CSR1);

	tp->stats.tx_errors++;
}

#endif /* __NET_TULIP_H__ */
