#ifndef BCM43xx_DMA_H_
#define BCM43xx_DMA_H_

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/linkage.h>
#include <asm/atomic.h>

#include "bcm43xx.h"


/* DMA-Interrupt reasons. */
#define BCM43xx_DMAIRQ_FATALMASK	((1 << 10) | (1 << 11) | (1 << 12) \
					 | (1 << 14) | (1 << 15))
#define BCM43xx_DMAIRQ_NONFATALMASK	(1 << 13)
#define BCM43xx_DMAIRQ_RX_DONE		(1 << 16)


/*** 32-bit DMA Engine. ***/

/* 32-bit DMA controller registers. */
#define BCM43xx_DMA32_TXCTL				0x00
#define		BCM43xx_DMA32_TXENABLE			0x00000001
#define		BCM43xx_DMA32_TXSUSPEND			0x00000002
#define		BCM43xx_DMA32_TXLOOPBACK		0x00000004
#define		BCM43xx_DMA32_TXFLUSH			0x00000010
#define		BCM43xx_DMA32_TXADDREXT_MASK		0x00030000
#define		BCM43xx_DMA32_TXADDREXT_SHIFT		16
#define BCM43xx_DMA32_TXRING				0x04
#define BCM43xx_DMA32_TXINDEX				0x08
#define BCM43xx_DMA32_TXSTATUS				0x0C
#define		BCM43xx_DMA32_TXDPTR			0x00000FFF
#define		BCM43xx_DMA32_TXSTATE			0x0000F000
#define			BCM43xx_DMA32_TXSTAT_DISABLED	0x00000000
#define			BCM43xx_DMA32_TXSTAT_ACTIVE	0x00001000
#define			BCM43xx_DMA32_TXSTAT_IDLEWAIT	0x00002000
#define			BCM43xx_DMA32_TXSTAT_STOPPED	0x00003000
#define			BCM43xx_DMA32_TXSTAT_SUSP	0x00004000
#define		BCM43xx_DMA32_TXERROR			0x000F0000
#define			BCM43xx_DMA32_TXERR_NOERR	0x00000000
#define			BCM43xx_DMA32_TXERR_PROT	0x00010000
#define			BCM43xx_DMA32_TXERR_UNDERRUN	0x00020000
#define			BCM43xx_DMA32_TXERR_BUFREAD	0x00030000
#define			BCM43xx_DMA32_TXERR_DESCREAD	0x00040000
#define		BCM43xx_DMA32_TXACTIVE			0xFFF00000
#define BCM43xx_DMA32_RXCTL				0x10
#define		BCM43xx_DMA32_RXENABLE			0x00000001
#define		BCM43xx_DMA32_RXFROFF_MASK		0x000000FE
#define		BCM43xx_DMA32_RXFROFF_SHIFT		1
#define		BCM43xx_DMA32_RXDIRECTFIFO		0x00000100
#define		BCM43xx_DMA32_RXADDREXT_MASK		0x00030000
#define		BCM43xx_DMA32_RXADDREXT_SHIFT		16
#define BCM43xx_DMA32_RXRING				0x14
#define BCM43xx_DMA32_RXINDEX				0x18
#define BCM43xx_DMA32_RXSTATUS				0x1C
#define		BCM43xx_DMA32_RXDPTR			0x00000FFF
#define		BCM43xx_DMA32_RXSTATE			0x0000F000
#define			BCM43xx_DMA32_RXSTAT_DISABLED	0x00000000
#define			BCM43xx_DMA32_RXSTAT_ACTIVE	0x00001000
#define			BCM43xx_DMA32_RXSTAT_IDLEWAIT	0x00002000
#define			BCM43xx_DMA32_RXSTAT_STOPPED	0x00003000
#define		BCM43xx_DMA32_RXERROR			0x000F0000
#define			BCM43xx_DMA32_RXERR_NOERR	0x00000000
#define			BCM43xx_DMA32_RXERR_PROT	0x00010000
#define			BCM43xx_DMA32_RXERR_OVERFLOW	0x00020000
#define			BCM43xx_DMA32_RXERR_BUFWRITE	0x00030000
#define			BCM43xx_DMA32_RXERR_DESCREAD	0x00040000
#define		BCM43xx_DMA32_RXACTIVE			0xFFF00000

/* 32-bit DMA descriptor. */
struct bcm43xx_dmadesc32 {
	__le32 control;
	__le32 address;
} __attribute__((__packed__));
#define BCM43xx_DMA32_DCTL_BYTECNT		0x00001FFF
#define BCM43xx_DMA32_DCTL_ADDREXT_MASK		0x00030000
#define BCM43xx_DMA32_DCTL_ADDREXT_SHIFT	16
#define BCM43xx_DMA32_DCTL_DTABLEEND		0x10000000
#define BCM43xx_DMA32_DCTL_IRQ			0x20000000
#define BCM43xx_DMA32_DCTL_FRAMEEND		0x40000000
#define BCM43xx_DMA32_DCTL_FRAMESTART		0x80000000



/*** 64-bit DMA Engine. ***/

/* 64-bit DMA controller registers. */
#define BCM43xx_DMA64_TXCTL				0x00
#define		BCM43xx_DMA64_TXENABLE			0x00000001
#define		BCM43xx_DMA64_TXSUSPEND			0x00000002
#define		BCM43xx_DMA64_TXLOOPBACK		0x00000004
#define		BCM43xx_DMA64_TXFLUSH			0x00000010
#define		BCM43xx_DMA64_TXADDREXT_MASK		0x00030000
#define		BCM43xx_DMA64_TXADDREXT_SHIFT		16
#define BCM43xx_DMA64_TXINDEX				0x04
#define BCM43xx_DMA64_TXRINGLO				0x08
#define BCM43xx_DMA64_TXRINGHI				0x0C
#define BCM43xx_DMA64_TXSTATUS				0x10
#define		BCM43xx_DMA64_TXSTATDPTR		0x00001FFF
#define		BCM43xx_DMA64_TXSTAT			0xF0000000
#define			BCM43xx_DMA64_TXSTAT_DISABLED	0x00000000
#define			BCM43xx_DMA64_TXSTAT_ACTIVE	0x10000000
#define			BCM43xx_DMA64_TXSTAT_IDLEWAIT	0x20000000
#define			BCM43xx_DMA64_TXSTAT_STOPPED	0x30000000
#define			BCM43xx_DMA64_TXSTAT_SUSP	0x40000000
#define BCM43xx_DMA64_TXERROR				0x14
#define		BCM43xx_DMA64_TXERRDPTR			0x0001FFFF
#define		BCM43xx_DMA64_TXERR			0xF0000000
#define			BCM43xx_DMA64_TXERR_NOERR	0x00000000
#define			BCM43xx_DMA64_TXERR_PROT	0x10000000
#define			BCM43xx_DMA64_TXERR_UNDERRUN	0x20000000
#define			BCM43xx_DMA64_TXERR_TRANSFER	0x30000000
#define			BCM43xx_DMA64_TXERR_DESCREAD	0x40000000
#define			BCM43xx_DMA64_TXERR_CORE	0x50000000
#define BCM43xx_DMA64_RXCTL				0x20
#define		BCM43xx_DMA64_RXENABLE			0x00000001
#define		BCM43xx_DMA64_RXFROFF_MASK		0x000000FE
#define		BCM43xx_DMA64_RXFROFF_SHIFT		1
#define		BCM43xx_DMA64_RXDIRECTFIFO		0x00000100
#define		BCM43xx_DMA64_RXADDREXT_MASK		0x00030000
#define		BCM43xx_DMA64_RXADDREXT_SHIFT		16
#define BCM43xx_DMA64_RXINDEX				0x24
#define BCM43xx_DMA64_RXRINGLO				0x28
#define BCM43xx_DMA64_RXRINGHI				0x2C
#define BCM43xx_DMA64_RXSTATUS				0x30
#define		BCM43xx_DMA64_RXSTATDPTR		0x00001FFF
#define		BCM43xx_DMA64_RXSTAT			0xF0000000
#define			BCM43xx_DMA64_RXSTAT_DISABLED	0x00000000
#define			BCM43xx_DMA64_RXSTAT_ACTIVE	0x10000000
#define			BCM43xx_DMA64_RXSTAT_IDLEWAIT	0x20000000
#define			BCM43xx_DMA64_RXSTAT_STOPPED	0x30000000
#define			BCM43xx_DMA64_RXSTAT_SUSP	0x40000000
#define BCM43xx_DMA64_RXERROR				0x34
#define		BCM43xx_DMA64_RXERRDPTR			0x0001FFFF
#define		BCM43xx_DMA64_RXERR			0xF0000000
#define			BCM43xx_DMA64_RXERR_NOERR	0x00000000
#define			BCM43xx_DMA64_RXERR_PROT	0x10000000
#define			BCM43xx_DMA64_RXERR_UNDERRUN	0x20000000
#define			BCM43xx_DMA64_RXERR_TRANSFER	0x30000000
#define			BCM43xx_DMA64_RXERR_DESCREAD	0x40000000
#define			BCM43xx_DMA64_RXERR_CORE	0x50000000

/* 64-bit DMA descriptor. */
struct bcm43xx_dmadesc64 {
	__le32 control0;
	__le32 control1;
	__le32 address_low;
	__le32 address_high;
} __attribute__((__packed__));
#define BCM43xx_DMA64_DCTL0_DTABLEEND		0x10000000
#define BCM43xx_DMA64_DCTL0_IRQ			0x20000000
#define BCM43xx_DMA64_DCTL0_FRAMEEND		0x40000000
#define BCM43xx_DMA64_DCTL0_FRAMESTART		0x80000000
#define BCM43xx_DMA64_DCTL1_BYTECNT		0x00001FFF
#define BCM43xx_DMA64_DCTL1_ADDREXT_MASK	0x00030000
#define BCM43xx_DMA64_DCTL1_ADDREXT_SHIFT	16



struct bcm43xx_dmadesc_generic {
	union {
		struct bcm43xx_dmadesc32 dma32;
		struct bcm43xx_dmadesc64 dma64;
	} __attribute__((__packed__));
} __attribute__((__packed__));


/* Misc DMA constants */
#define BCM43xx_DMA_RINGMEMSIZE		PAGE_SIZE
#define BCM43xx_DMA0_RX_FRAMEOFFSET	30
#define BCM43xx_DMA3_RX_FRAMEOFFSET	0


/* DMA engine tuning knobs */
#define BCM43xx_TXRING_SLOTS		128
#define BCM43xx_RXRING_SLOTS		64
#define BCM43xx_DMA0_RX_BUFFERSIZE	(2304 + 100)
#define BCM43xx_DMA3_RX_BUFFERSIZE	16



#ifdef CONFIG_BCM43XX_MAC80211_DMA


struct sk_buff;
struct bcm43xx_private;
struct bcm43xx_txstatus;


struct bcm43xx_dmadesc_meta {
	/* The kernel DMA-able buffer. */
	struct sk_buff *skb;
	/* DMA base bus-address of the descriptor buffer. */
	dma_addr_t dmaaddr;
	/* ieee80211 TX status. Only used once per 802.11 frag. */
	u8 is_last_fragment;
	struct ieee80211_tx_status txstat;
};

struct bcm43xx_dmaring;

/* Lowlevel DMA operations that differ between 32bit and 64bit DMA. */
struct bcm43xx_dma_ops {
	struct bcm43xx_dmadesc_generic * (*idx2desc)(struct bcm43xx_dmaring *ring,
						     int slot,
						     struct bcm43xx_dmadesc_meta **meta);
	void (*fill_descriptor)(struct bcm43xx_dmaring *ring,
				struct bcm43xx_dmadesc_generic *desc,
				dma_addr_t dmaaddr, u16 bufsize,
				int start, int end, int irq);
	void (*poke_tx)(struct bcm43xx_dmaring *ring, int slot);
	void (*tx_suspend)(struct bcm43xx_dmaring *ring);
	void (*tx_resume)(struct bcm43xx_dmaring *ring);
	int (*get_current_rxslot)(struct bcm43xx_dmaring *ring);
	void (*set_current_rxslot)(struct bcm43xx_dmaring *ring, int slot);
};

struct bcm43xx_dmaring {
	/* Lowlevel DMA ops. */
	const struct bcm43xx_dma_ops *ops;
	/* Kernel virtual base address of the ring memory. */
	void *descbase;
	/* Meta data about all descriptors. */
	struct bcm43xx_dmadesc_meta *meta;
	/* Cache of TX headers for each slot.
	 * This is to avoid an allocation on each TX.
	 * This is NULL for an RX ring.
	 */
	u8 *txhdr_cache;
	/* (Unadjusted) DMA base bus-address of the ring memory. */
	dma_addr_t dmabase;
	/* Number of descriptor slots in the ring. */
	int nr_slots;
	/* Number of used descriptor slots. */
	int used_slots;
	/* Currently used slot in the ring. */
	int current_slot;
	/* Total number of packets sent. Statistics only. */
	unsigned int nr_tx_packets;
	/* Frameoffset in octets. */
	u32 frameoffset;
	/* Descriptor buffer size. */
	u16 rx_buffersize;
	/* The MMIO base register of the DMA controller. */
	u16 mmio_base;
	/* DMA controller index number (0-5). */
	int index;
	/* Boolean. Is this a TX ring? */
	u8 tx;
	/* Boolean. 64bit DMA if true, 32bit DMA otherwise. */
	u8 dma64;
	/* Boolean. Is this ring stopped at ieee80211 level? */
	u8 stopped;
	/* Lock, only used for TX. */
	spinlock_t lock;
	struct bcm43xx_wldev *dev;
#ifdef CONFIG_BCM43XX_MAC80211_DEBUG
	/* Maximum number of used slots. */
	int max_used_slots;
	/* Last time we injected a ring overflow. */
	unsigned long last_injected_overflow;
#endif /* CONFIG_BCM43XX_MAC80211_DEBUG*/
};


static inline
u32 bcm43xx_dma_read(struct bcm43xx_dmaring *ring,
		     u16 offset)
{
	return bcm43xx_read32(ring->dev, ring->mmio_base + offset);
}

static inline
void bcm43xx_dma_write(struct bcm43xx_dmaring *ring,
		       u16 offset, u32 value)
{
	bcm43xx_write32(ring->dev, ring->mmio_base + offset, value);
}


int bcm43xx_dma_init(struct bcm43xx_wldev *dev);
void bcm43xx_dma_free(struct bcm43xx_wldev *dev);

int bcm43xx_dmacontroller_rx_reset(struct bcm43xx_wldev *dev,
				   u16 dmacontroller_mmio_base,
				   int dma64);
int bcm43xx_dmacontroller_tx_reset(struct bcm43xx_wldev *dev,
				   u16 dmacontroller_mmio_base,
				   int dma64);

u16 bcm43xx_dmacontroller_base(int dma64bit, int dmacontroller_idx);

void bcm43xx_dma_tx_suspend(struct bcm43xx_wldev *dev);
void bcm43xx_dma_tx_resume(struct bcm43xx_wldev *dev);

void bcm43xx_dma_get_tx_stats(struct bcm43xx_wldev *dev,
			      struct ieee80211_tx_queue_stats *stats);

int bcm43xx_dma_tx(struct bcm43xx_wldev *dev,
		   struct sk_buff *skb,
		   struct ieee80211_tx_control *ctl);
void bcm43xx_dma_handle_txstatus(struct bcm43xx_wldev *dev,
				 const struct bcm43xx_txstatus *status);

void bcm43xx_dma_rx(struct bcm43xx_dmaring *ring);

#else /* CONFIG_BCM43XX_MAC80211_DMA */


static inline
int bcm43xx_dma_init(struct bcm43xx_wldev *dev)
{
	return 0;
}
static inline
void bcm43xx_dma_free(struct bcm43xx_wldev *dev)
{
}
static inline
int bcm43xx_dmacontroller_rx_reset(struct bcm43xx_wldev *dev,
				   u16 dmacontroller_mmio_base,
				   int dma64)
{
	return 0;
}
static inline
int bcm43xx_dmacontroller_tx_reset(struct bcm43xx_wldev *dev,
				   u16 dmacontroller_mmio_base,
				   int dma64)
{
	return 0;
}
static inline
void bcm43xx_dma_get_tx_stats(struct bcm43xx_wldev *dev,
			      struct ieee80211_tx_queue_stats *stats)
{
}
static inline
int bcm43xx_dma_tx(struct bcm43xx_wldev *dev,
		   struct sk_buff *skb,
		   struct ieee80211_tx_control *ctl)
{
	return 0;
}
static inline
void bcm43xx_dma_handle_txstatus(struct bcm43xx_wldev *dev,
				 const struct bcm43xx_txstatus *status)
{
}
static inline
void bcm43xx_dma_rx(struct bcm43xx_dmaring *ring)
{
}
static inline
void bcm43xx_dma_tx_suspend(struct bcm43xx_wldev *dev)
{
}
static inline
void bcm43xx_dma_tx_resume(struct bcm43xx_wldev *dev)
{
}

#endif /* CONFIG_BCM43XX_MAC80211_DMA */
#endif /* BCM43xx_DMA_H_ */
