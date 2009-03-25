/*
 * RDC R6040 Fast Ethernet MAC support
 *
 * Copyright (C) 2004 Sten Wang <sten.wang@rdc.com.tw>
 * Copyright (C) 2007
 *	Daniel Gimpelevich <daniel@gimpelevich.san-francisco.ca.us>
 *	Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/crc32.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/uaccess.h>

#include <asm/processor.h>

#define DRV_NAME	"r6040"
#define DRV_VERSION	"0.19"
#define DRV_RELDATE	"16Jun2008"

/* define bits of a debug mask */
#define DBG_PHY           0x00000001 /*!< show PHY read/write */
#define DBG_FREE_BUFS     0x00000002 /*!< show calls to r6040_free_*bufs */
#define DBG_RING          0x00000004 /*!< debug init./freeing of descr rings */
#define DBG_RX_BUF        0x00000008 /*!< show alloc. of new rx buf (in IRQ context !) */
#define DBG_TX_BUF        0x00000010 /*!< show arrival of new tx buf */
#define DBG_TX_DONE       0x00000020 /*!< debug TX done */
#define DBG_RX_DESCR      0x00000040 /*!< debug rx descr to be processed */
#define DBG_RX_DATA       0x00000080 /*!< show some user data of incoming packet */
#define DBG_EXIT          0x00000100 /*!< show exit code calls */
#define DBG_INIT          0x00000200 /*!< show init. code calls */
#define DBG_TX_RING_DUMP  0x00000400 /*!< dump the tx ring after creation */
#define DBG_RX_RING_DUMP  0x00000800 /*!< dump the rx ring after creation */
#define DBG_TX_DESCR      0x00001000 /*!< dump the setting of a descr for tx */
#define DBG_TX_DATA       0x00002000 /*!< dump some tx data */
#define DBG_IRQ           0x00004000 /*!< print inside the irq handler */
#define DBG_POLL          0x00008000 /*!< dump info on poll procedure */
#define DBG_MAC_ADDR      0x00010000 /*!< debug mac address setting */
#define DBG_OPEN          0x00020000 /*!< debug open proc. */

static int debug = 0;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "debug mask (-1 for all)");

/* define which debugs are left in the code during compilation */
#define DEBUG (-1) /* all debugs */

#define dbg(l, f, ...)				\
  do { \
    if ((DEBUG & l) && (debug & l)) { \
      printk(KERN_INFO DRV_NAME " %s: " f, __FUNCTION__, ## __VA_ARGS__); \
    } \
  } while (0)

#define err(f, ...) printk(KERN_WARNING DRV_NAME " %s: " f, __FUNCTION__, ## __VA_ARGS__)

/* PHY CHIP Address */
#define PHY1_ADDR	1	/* For MAC1 */
#define PHY2_ADDR	3	/* For MAC2 */
#define PHY_MODE	0x3100	/* PHY CHIP Register 0 */
#define PHY_CAP		0x01E1	/* PHY CHIP Register 4 */

/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT	(6000 * HZ / 1000)

/* RDC MAC I/O Size */
#define R6040_IO_SIZE	256

/* MAX RDC MAC */
#define MAX_MAC		2

/* MAC registers */
#define MCR0		0x00	/* Control register 0 */
#define MCR1		0x04	/* Control register 1 */
#define  MAC_RST	0x0001	/* Reset the MAC */
#define MBCR		0x08	/* Bus control */
#define MT_ICR		0x0C	/* TX interrupt control */
#define MR_ICR		0x10	/* RX interrupt control */
#define MTPR		0x14	/* TX poll command register */
#define MR_BSR		0x18	/* RX buffer size */
#define MR_DCR		0x1A	/* RX descriptor control */
#define MLSR		0x1C	/* Last status */
#define MMDIO		0x20	/* MDIO control register */
#define  MDIO_WRITE	0x4000	/* MDIO write */
#define  MDIO_READ	0x2000	/* MDIO read */
#define MMRD		0x24	/* MDIO read data register */
#define MMWD		0x28	/* MDIO write data register */
#define MTD_SA0		0x2C	/* TX descriptor start address 0 */
#define MTD_SA1		0x30	/* TX descriptor start address 1 */
#define MRD_SA0		0x34	/* RX descriptor start address 0 */
#define MRD_SA1		0x38	/* RX descriptor start address 1 */
#define MISR		0x3C	/* Status register */
#define MIER		0x40	/* INT enable register */
#define  MSK_INT	0x0000	/* Mask off interrupts */
#define  RX_FINISH      0x0001  /* rx finished irq */
#define  RX_NO_DESC     0x0002  /* rx no descr. avail. irq */
#define  RX_FIFO_FULL   0x0004  /* rx fifo full irq */
#define  RX_EARLY       0x0008  /* rx early irq */
#define  TX_FINISH      0x0010  /* tx finished irq */
#define  TX_EARLY       0x0080  /* tx early irq */
#define  EVENT_OVRFL    0x0100  /* event counter overflow irq */
#define  LINK_CHANGED   0x0200  /* PHY link changed irq */

#define ME_CISR		0x44	/* Event counter INT status */
#define ME_CIER		0x48	/* Event counter INT enable  */
#define MR_CNT		0x50	/* Successfully received packet counter */
#define ME_CNT0		0x52	/* Event counter 0 */
#define ME_CNT1		0x54	/* Event counter 1 */
#define ME_CNT2		0x56	/* Event counter 2 */
#define ME_CNT3		0x58	/* Event counter 3 */
#define MT_CNT		0x5A	/* Successfully transmit packet counter */
#define ME_CNT4		0x5C	/* Event counter 4 */
#define MP_CNT		0x5E	/* Pause frame counter register */
#define MAR0		0x60	/* Hash table 0 */
#define MAR1		0x62	/* Hash table 1 */
#define MAR2		0x64	/* Hash table 2 */
#define MAR3		0x66	/* Hash table 3 */
#define MID_0L		0x68	/* Multicast address MID0 Low */
#define MID_0M		0x6A	/* Multicast address MID0 Medium */
#define MID_0H		0x6C	/* Multicast address MID0 High */
#define MID_1L		0x70	/* MID1 Low */
#define MID_1M		0x72	/* MID1 Medium */
#define MID_1H		0x74	/* MID1 High */
#define MID_2L		0x78	/* MID2 Low */
#define MID_2M		0x7A	/* MID2 Medium */
#define MID_2H		0x7C	/* MID2 High */
#define MID_3L		0x80	/* MID3 Low */
#define MID_3M		0x82	/* MID3 Medium */
#define MID_3H		0x84	/* MID3 High */
#define PHY_CC		0x88	/* PHY status change configuration register */
#define PHY_ST		0x8A	/* PHY status register */
#define MAC_SM		0xAC	/* MAC status machine */
#define MAC_ID		0xBE	/* Identifier register */

#define TX_DCNT		0x80	/* TX descriptor count */
#define RX_DCNT		0x80	/* RX descriptor count */
#define MAX_BUF_SIZE	0x600
#define RX_DESC_SIZE	(RX_DCNT * sizeof(struct r6040_descriptor))
#define TX_DESC_SIZE	(TX_DCNT * sizeof(struct r6040_descriptor))
#define MBCR_DEFAULT	0x012A	/* MAC Bus Control Register: 
				   - wait 1 host clock until SDRAM bus request
				     becomes high priority
				   - RX FIFO: 32 byte
				   - TX FIFO: 64 byte
				   - FIFO transfer length: 16 byte */
#define MCAST_MAX	4	/* Max number multicast addresses to filter */

/* PHY settings */
#define ICPLUS_PHY_ID	0x0243

MODULE_AUTHOR("Sten Wang <sten.wang@rdc.com.tw>,"
	"Daniel Gimpelevich <daniel@gimpelevich.san-francisco.ca.us>,"
	"Florian Fainelli <florian@openwrt.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RDC R6040 NAPI PCI FastEthernet driver");

/*! which rx interrupts do we allow */
#define RX_INTS                        (RX_FIFO_FULL|RX_NO_DESC|RX_FINISH)
/*! which tx interrupts do we allow */
#define TX_INTS                        (TX_FINISH)
#define INT_MASK                 (RX_INTS | TX_INTS)

struct r6040_descriptor {
	u16	status, len;		/* 0-3 */
	__le32	buf;			/* 4-7 */
	__le32	ndesc;			/* 8-B */
	u32	rev1;			/* C-F */
	char	*vbufp;			/* 10-13 */
	struct r6040_descriptor *vndescp;	/* 14-17 */
	struct sk_buff *skb_ptr;	/* 18-1B */
	u32	rev2;			/* 1C-1F */
} __attribute__((aligned(32)));

/*! defines for the status field in the r6040_descriptor */
#define DESC_STATUS_OWNER_MAC       (1<<15) /*!< if set the MAC is the owner of this descriptor */
#define DESC_STATUS_RX_OK           (1<<14) /*!< rx was successful */
#define DESC_STATUS_RX_ERR          (1<<11) /*!< rx PHY error */
#define DESC_STATUS_RX_ERR_DRIBBLE  (1<<10) /*!< rx dribble packet */
#define DESC_STATUS_RX_ERR_BUFLEN   (1<< 9) /*!< rx length exceeded buffer size */
#define DESC_STATUS_RX_ERR_LONG     (1<< 8) /*!< rx length > maximum packet length */
#define DESC_STATUS_RX_ERR_RUNT     (1<< 7) /*!< rx: packet length < 64 byte */
#define DESC_STATUS_RX_ERR_CRC      (1<< 6) /*!< rx: crc error */
#define DESC_STATUS_RX_BROADCAST    (1<< 5) /*!< rx: broadcast (no error) */
#define DESC_STATUS_RX_MULTICAST    (1<< 4) /*!< rx: multicast (no error) */
#define DESC_STATUS_RX_MCH_HIT      (1<< 3) /*!< rx: multicast hit in hash table (no error) */
#define DESC_STATUS_RX_MIDH_HIT     (1<< 2) /*!< rx: MID table hit (no error) */
#define DESC_STATUS_RX_IDX_MID_MASK 3       /*!< rx: mask for the index of matched MIDx */

struct r6040_private {
	spinlock_t lock;		/* driver lock */
	struct timer_list timer;
	struct pci_dev *pdev;
	struct r6040_descriptor *rx_insert_ptr;
	struct r6040_descriptor *rx_remove_ptr;
	struct r6040_descriptor *tx_insert_ptr;
	struct r6040_descriptor *tx_remove_ptr;
	struct r6040_descriptor *rx_ring;
	struct r6040_descriptor *tx_ring;
	dma_addr_t rx_ring_dma;
	dma_addr_t tx_ring_dma;
	u16	tx_free_desc, phy_addr, phy_mode;
	u16	mcr0, mcr1;
	u16	switch_sig;
	struct net_device *dev;
	struct mii_if_info mii_if;
	struct napi_struct napi;
	void __iomem *base;
};

static char *parent = "wlan0";
module_param(parent, charp, 0444);
MODULE_PARM_DESC(parent, "Parent network device name to get the MAC address from");

static u8 mac_base[ETH_ALEN] = {0,0x50,0xfc,2,3,4};
module_param_array(mac_base, byte, NULL, 0444);
MODULE_PARM_DESC(mac_base, "Starting MAC address");

static int reverse = 1;
module_param(reverse, invbool, 0444);
MODULE_PARM_DESC(reverse, "Reverse card indices");

static char version[] __devinitdata = DRV_NAME
	": RDC R6040 NAPI net driver,"
	"version "DRV_VERSION " (" DRV_RELDATE ")";

static int phy_table[] = { PHY1_ADDR, PHY2_ADDR };

/* forward declarations */
void r6040_multicast_list(struct net_device *dev);

/* jal2: comment out to get more symbols for debugging */
//#define STATIC static
#define STATIC

#if DEBUG
/*! hexdump an memory area into a string. delim is taken as the delimiter between two bytes.
    It is omitted if delim == '\0' */
STATIC char *hex2str(void *addr, char *buf, int nr_bytes, int delim)
{
	unsigned char *src = addr;
	char *outb = buf;

#define BIN2HEXDIGIT(x) ((x) < 10 ? '0'+(x) : 'A'-10+(x))

	while (nr_bytes > 0) {
		*outb++ = BIN2HEXDIGIT(*src>>4);
		*outb++ = BIN2HEXDIGIT(*src&0xf);
		if (delim)
			*outb++ = delim;
		nr_bytes--;
		src++;
	}

	if (delim)
		outb--;
	*outb = '\0';
	return buf;
}

#endif /* #if DEBUG */

/* Read a word data from PHY Chip */
STATIC int phy_read(void __iomem *ioaddr, int phy_addr, int reg)
{
	int limit = 2048;
	u16 cmd;
	int rc;

	iowrite16(MDIO_READ + reg + (phy_addr << 8), ioaddr + MMDIO);
	/* Wait for the read bit to be cleared */
	while (limit--) {
		cmd = ioread16(ioaddr + MMDIO);
		if (!(cmd & MDIO_READ))
			break;
	}

	if (limit <= 0)
		err("phy addr x%x reg x%x timed out\n",
		    phy_addr, reg);

	rc=ioread16(ioaddr + MMRD);

	dbg(DBG_PHY, "phy addr x%x reg x%x val x%x\n", phy_addr, reg, rc);
	return rc;
}

/* Write a word data from PHY Chip */
STATIC void phy_write(void __iomem *ioaddr, int phy_addr, int reg, u16 val)
{
	int limit = 2048;
	u16 cmd;

	dbg(DBG_PHY, "phy addr x%x reg x%x val x%x\n", phy_addr, reg, val);

	iowrite16(val, ioaddr + MMWD);
	/* Write the command to the MDIO bus */
	iowrite16(MDIO_WRITE + reg + (phy_addr << 8), ioaddr + MMDIO);
	/* Wait for the write bit to be cleared */
	while (limit--) {
		cmd = ioread16(ioaddr + MMDIO);
		if (!(cmd & MDIO_WRITE))
			break;
	}
	if (limit <= 0)
		err("phy addr x%x reg x%x val x%x timed out\n",
		    phy_addr, reg, val);
}

STATIC int mdio_read(struct net_device *dev, int mii_id, int reg)
{
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;

	return (phy_read(ioaddr, lp->phy_addr, reg));
}

STATIC void mdio_write(struct net_device *dev, int mii_id, int reg, int val)
{
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;

	phy_write(ioaddr, lp->phy_addr, reg, val);
}

void r6040_free_txbufs(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	int i;

	dbg(DBG_FREE_BUFS, "ENTER\n");
	for (i = 0; i < TX_DCNT; i++) {
		if (lp->tx_insert_ptr->skb_ptr) {
			pci_unmap_single(lp->pdev,
				le32_to_cpu(lp->tx_insert_ptr->buf),
				MAX_BUF_SIZE, PCI_DMA_TODEVICE);
			dev_kfree_skb(lp->tx_insert_ptr->skb_ptr);
			lp->tx_insert_ptr->skb_ptr = NULL;
		}
		lp->tx_insert_ptr = lp->tx_insert_ptr->vndescp;
	}
	dbg(DBG_FREE_BUFS, "EXIT\n");
}

/*! unmap and free all rx skb */
void r6040_free_rxbufs(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	int i;

	dbg(DBG_FREE_BUFS, "ENTER\n");
	for (i = 0; i < RX_DCNT; i++) {
		if (lp->rx_insert_ptr->skb_ptr) {
			pci_unmap_single(lp->pdev,
				le32_to_cpu(lp->rx_insert_ptr->buf),
				MAX_BUF_SIZE, PCI_DMA_FROMDEVICE);
			dev_kfree_skb(lp->rx_insert_ptr->skb_ptr);
			lp->rx_insert_ptr->skb_ptr = NULL;
		}
		lp->rx_insert_ptr = lp->rx_insert_ptr->vndescp;
	}
	dbg(DBG_FREE_BUFS, "EXIT\n");

}

void r6040_init_ring_desc(struct r6040_descriptor *desc_ring,
				 dma_addr_t desc_dma, int size)
{
	struct r6040_descriptor *desc = desc_ring;
	dma_addr_t mapping = desc_dma;

	dbg(DBG_RING, "desc_ring %p desc_dma %08x size x%x\n",
	    desc_ring, desc_dma, size);

	
	while (size-- > 0) {
		mapping += sizeof(*desc);
		memset(desc, 0, sizeof(*desc));
		desc->ndesc = cpu_to_le32(mapping);
		desc->vndescp = desc + 1;
		desc++;
	}

	/* last descriptor points to first one to close the descriptor ring */
	desc--;
	desc->ndesc = cpu_to_le32(desc_dma);
	desc->vndescp = desc_ring;
}

#if (DEBUG & DBG_TX_RING_DUMP)
/*! dump the tx ring to syslog */
STATIC void
dump_tx_ring(struct r6040_private *lp)
{
	int i;
	struct r6040_descriptor *ptr;

	printk(KERN_INFO "%s: nr_desc x%x tx_ring %p tx_ring_dma %08x "
	       "tx_insert %p tx_remove %p\n",
	       DRV_NAME, TX_DCNT, lp->tx_ring, lp->tx_ring_dma,
	       lp->tx_insert_ptr, lp->tx_remove_ptr);

	if (lp->tx_ring) {
		for(i=0, ptr=lp->tx_ring; i < TX_DCNT; i++, ptr++) {
			printk(KERN_INFO "%s: %d. descr: status x%x len x%x "
			       "ndesc %08x vbufp %p vndescp %p skb_ptr %p\n", 
			       DRV_NAME, i, ptr->status, ptr->len,
			       ptr->ndesc, ptr->vbufp, ptr->vndescp, ptr->skb_ptr);
		}
	}
}
#endif /* #if (DEBUG & DBG_TX_RING_DUMP) */

void r6040_init_txbufs(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);

	lp->tx_free_desc = TX_DCNT;

	lp->tx_remove_ptr = lp->tx_insert_ptr = lp->tx_ring;
	r6040_init_ring_desc(lp->tx_ring, lp->tx_ring_dma, TX_DCNT);

#if (DEBUG & DBG_TX_RING_DUMP)
	if (debug & DBG_TX_RING_DUMP) {
		dump_tx_ring(lp);
	}
#endif
}

#if (DEBUG & DBG_RX_RING_DUMP)
/*! dump the rx ring to syslog */
STATIC void
dump_rx_ring(struct r6040_private *lp)
{
	int i;
	struct r6040_descriptor *ptr;

	printk(KERN_INFO "%s: nr_desc x%x rx_ring %p rx_ring_dma %08x "
	       "rx_insert %p rx_remove %p\n",
	       DRV_NAME, RX_DCNT, lp->rx_ring, lp->rx_ring_dma,
	       lp->rx_insert_ptr, lp->rx_remove_ptr);

	if (lp->rx_ring) {
		for(i=0, ptr=lp->rx_ring; i < RX_DCNT; i++, ptr++) {
			printk(KERN_INFO "%s: %d. descr: status x%x len x%x "
			       "ndesc %08x vbufp %p vndescp %p skb_ptr %p\n", 
			       DRV_NAME, i, ptr->status, ptr->len,
			       ptr->ndesc, ptr->vbufp, ptr->vndescp, ptr->skb_ptr);
		}
	}
}
#endif /* #if (DEBUG & DBG_TX_RING_DUMP) */

int r6040_alloc_rxbufs(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	struct r6040_descriptor *desc;
	struct sk_buff *skb;
	int rc;

	lp->rx_remove_ptr = lp->rx_insert_ptr = lp->rx_ring;
	r6040_init_ring_desc(lp->rx_ring, lp->rx_ring_dma, RX_DCNT);

	/* alloc skbs for the rx descriptors */
	desc = lp->rx_ring;
	do {
		if (!(skb=netdev_alloc_skb(dev, MAX_BUF_SIZE))) {
			err("failed to alloc skb for rx\n");
			rc = -ENOMEM;
			goto err_exit;
		}
		desc->skb_ptr = skb;
		desc->buf = cpu_to_le32(pci_map_single(lp->pdev,
						       desc->skb_ptr->data,
						       MAX_BUF_SIZE, PCI_DMA_FROMDEVICE));
		desc->status = DESC_STATUS_OWNER_MAC;
		desc = desc->vndescp;
	} while (desc != lp->rx_ring);

#if (DEBUG & DBG_RX_RING_DUMP)
	if (debug & DBG_RX_RING_DUMP) {
		dump_rx_ring(lp);
	}
#endif

	return 0;

err_exit:
	/* dealloc all previously allocated skb */
	r6040_free_rxbufs(dev);
	return rc;
}

/*! reset MAC and set all registers */
void r6040_init_mac_regs(struct r6040_private *lp)
{
	void __iomem *ioaddr = lp->base;
	int limit;
	char obuf[3*ETH_ALEN] __attribute__ ((unused));

	/* Mask Off Interrupt */
	iowrite16(MSK_INT, ioaddr + MIER);

	/* reset MAC */
	iowrite16(MAC_RST, ioaddr + MCR1);
	udelay(100);
	limit=2048;
	while ((ioread16(ioaddr + MCR1) & MAC_RST) && limit-- > 0);

	/* Reset internal state machine */
	iowrite16(2, ioaddr + MAC_SM);
	iowrite16(0, ioaddr + MAC_SM);
	udelay(5000);

	/* Restore MAC Addresses */
	r6040_multicast_list(lp->dev);

	/* TODO: restore multcast and hash table */

	/* MAC Bus Control Register */
	iowrite16(MBCR_DEFAULT, ioaddr + MBCR);

	/* Buffer Size Register */
	iowrite16(MAX_BUF_SIZE, ioaddr + MR_BSR);

	/* write tx ring start address */
	iowrite16(lp->tx_ring_dma, ioaddr + MTD_SA0);
	iowrite16(lp->tx_ring_dma >> 16, ioaddr + MTD_SA1);

	/* write rx ring start address */
	iowrite16(lp->rx_ring_dma, ioaddr + MRD_SA0);
	iowrite16(lp->rx_ring_dma >> 16, ioaddr + MRD_SA1);

	/* set interrupt waiting time and packet numbers */
	iowrite16(0, ioaddr + MT_ICR);
	iowrite16(0, ioaddr + MR_ICR);

	/* enable interrupts */
	iowrite16(INT_MASK, ioaddr + MIER);

	/* enable tx and rx */
	iowrite16(lp->mcr0 | 0x0002, ioaddr);

	/* let TX poll the descriptors - we may got called by r6040_tx_timeout which has left
	   some unsent tx buffers */
	iowrite16(0x01, ioaddr + MTPR);
}

void r6040_tx_timeout(struct net_device *dev)
{
	struct r6040_private *priv = netdev_priv(dev);
	void __iomem *ioaddr = priv->base;

	/* we read MISR, which clears on read (i.e. we may loose an RX interupt,
	   but this is an error anyhow ... */
	printk(KERN_WARNING "%s: transmit timed out, int enable %4.4x "
	       "status %4.4x, PHY status %4.4x\n",
	       dev->name, ioread16(ioaddr + MIER),
	       ioread16(ioaddr + MISR),
	       mdio_read(dev, priv->mii_if.phy_id, MII_BMSR));

	dev->stats.tx_errors++;

	/* Reset MAC and re-init all registers */
	r6040_init_mac_regs(priv);
}

struct net_device_stats *r6040_get_stats(struct net_device *dev)
{
	struct r6040_private *priv = netdev_priv(dev);
	void __iomem *ioaddr = priv->base;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	dev->stats.rx_crc_errors += ioread8(ioaddr + ME_CNT1);
	dev->stats.multicast += ioread8(ioaddr + ME_CNT0);
	spin_unlock_irqrestore(&priv->lock, flags);

	return &dev->stats;
}

/* Stop RDC MAC and Free the allocated resource */
void r6040_down(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;
	struct pci_dev *pdev = lp->pdev;
	int limit = 2048;

	dbg(DBG_EXIT, "ENTER\n");

	/* Stop MAC */
	iowrite16(MSK_INT, ioaddr + MIER);	/* Mask Off Interrupt */
	iowrite16(MAC_RST, ioaddr + MCR1);	/* Reset RDC MAC */
	udelay(100);
	while ((ioread16(ioaddr+MCR1) & 1) && limit-- > 0);

	if (limit <= 0)
		err("timeout while waiting for reset done.\n");

	free_irq(dev->irq, dev);

	/* Free RX buffer */
	r6040_free_rxbufs(dev);

	/* Free TX buffer */
	r6040_free_txbufs(dev);

	/* Free Descriptor memory */
	pci_free_consistent(pdev, RX_DESC_SIZE, lp->rx_ring, lp->rx_ring_dma);
	pci_free_consistent(pdev, TX_DESC_SIZE, lp->tx_ring, lp->tx_ring_dma);

	dbg(DBG_EXIT, "EXIT\n");
}

int r6040_close(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);

	dbg(DBG_EXIT, "ENTER\n");

	/* deleted timer */
	del_timer_sync(&lp->timer);
	spin_lock_irq(&lp->lock);
	napi_disable(&lp->napi);
	netif_stop_queue(dev);
	r6040_down(dev);
	spin_unlock_irq(&lp->lock);

	dbg(DBG_EXIT, "EXIT\n");
	return 0;
}

/* Status of PHY CHIP. Returns 0x8000 for full duplex, 0 for half duplex */
STATIC int phy_mode_chk(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;
	int phy_dat;

	/* PHY Link Status Check */
	phy_dat = phy_read(ioaddr, lp->phy_addr, 1);
	if (!(phy_dat & 0x4))
		phy_dat = 0x8000;	/* Link Failed, full duplex */

	/* PHY Chip Auto-Negotiation Status */
	phy_dat = phy_read(ioaddr, lp->phy_addr, 1);
	if (phy_dat & 0x0020) {
		/* Auto Negotiation Mode */
		phy_dat = phy_read(ioaddr, lp->phy_addr, 5);
		phy_dat &= phy_read(ioaddr, lp->phy_addr, 4);
		if (phy_dat & 0x140)
			/* Force full duplex */
			phy_dat = 0x8000;
		else
			phy_dat = 0;
	} else {
		/* Force Mode */
		phy_dat = phy_read(ioaddr, lp->phy_addr, 0);
		if (phy_dat & 0x100)
			phy_dat = 0x8000;
		else
			phy_dat = 0x0000;
	}

	dbg(DBG_PHY, "RETURN x%x\n", phy_dat);
	return phy_dat;
};

void r6040_set_carrier(struct mii_if_info *mii)
{
	if (phy_mode_chk(mii->dev)) {
		/* autoneg is off: Link is always assumed to be up */
		if (!netif_carrier_ok(mii->dev))
			netif_carrier_on(mii->dev);
	} else
		phy_mode_chk(mii->dev);
}

int r6040_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct r6040_private *lp = netdev_priv(dev);
	struct mii_ioctl_data *data = if_mii(rq);
	int rc;

	if (!netif_running(dev))
		return -EINVAL;
	spin_lock_irq(&lp->lock);
	rc = generic_mii_ioctl(&lp->mii_if, data, cmd, NULL);
	spin_unlock_irq(&lp->lock);
	r6040_set_carrier(&lp->mii_if);
	return rc;
}

int r6040_rx(struct net_device *dev, int limit)
{
	struct r6040_private *priv = netdev_priv(dev);
	int count=0;
	struct r6040_descriptor *descptr = priv->rx_remove_ptr;
	struct sk_buff *skb_ptr, *new_skb;
	char obuf[2*32+1] __attribute__ ((unused)); /* for debugging */

	while (count < limit && !(descptr->status & DESC_STATUS_OWNER_MAC)) {
		/* limit not reached and the descriptor belongs to the CPU */

		dbg(DBG_RX_DESCR, "descptr %p status x%x data len x%x\n",
		    descptr, descptr->status, descptr->len);
		
		/* Check for errors */
		if (descptr->status & DESC_STATUS_RX_ERR) {
		
			dev->stats.rx_errors++;
			
			if (descptr->status & (DESC_STATUS_RX_ERR_DRIBBLE|
					       DESC_STATUS_RX_ERR_BUFLEN|
					       DESC_STATUS_RX_ERR_LONG|
					       DESC_STATUS_RX_ERR_RUNT)) {
				/* packet too long or too short*/
				dev->stats.rx_length_errors++;
			}

			if (descptr->status & DESC_STATUS_RX_ERR_CRC) {
				dev->stats.rx_crc_errors++;
			}
			goto next_descr;
		}
		
		/* successful received packet */
		
		/* first try to allocate new skb. If this fails
		   we drop the packet and leave the old skb there.*/
		new_skb = netdev_alloc_skb(dev, MAX_BUF_SIZE);
		if (!new_skb) {
			dev->stats.rx_dropped++;
			goto next_descr;
		}
		skb_ptr = descptr->skb_ptr;
		skb_ptr->dev = priv->dev;
		/* Do not count the CRC */
		skb_put(skb_ptr, descptr->len - 4);
		pci_unmap_single(priv->pdev, le32_to_cpu(descptr->buf),
				 MAX_BUF_SIZE, PCI_DMA_FROMDEVICE);
		skb_ptr->protocol = eth_type_trans(skb_ptr, priv->dev);

		dbg(DBG_RX_DATA, "rx len x%x: %s...\n",
		    descptr->len, 
		    hex2str(skb_ptr->data, obuf, sizeof(obuf)/2, '\0'));

		/* Send to upper layer */
		netif_receive_skb(skb_ptr);
		dev->last_rx = jiffies;
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += (descptr->len-4);

		/* put new skb into descriptor */
		descptr->skb_ptr = new_skb;
		descptr->buf = cpu_to_le32(pci_map_single(priv->pdev,
			descptr->skb_ptr->data,
			MAX_BUF_SIZE, PCI_DMA_FROMDEVICE));

next_descr:		
		/* put the descriptor back to the MAC */
		descptr->status = DESC_STATUS_OWNER_MAC;
		descptr = descptr->vndescp;
		count++; /* shall we count errors and dropped packets as well? */
	} /* while (limit && !(descptr->status & DESC_STATUS_OWNER_MAC)) */

	/* remember next descriptor to check for rx */
	priv->rx_remove_ptr = descptr;

	return count;
}

void r6040_tx(struct net_device *dev)
{
	struct r6040_private *priv = netdev_priv(dev);
	struct r6040_descriptor *descptr;
	void __iomem *ioaddr = priv->base;
	struct sk_buff *skb_ptr;
	u16 err;

	spin_lock(&priv->lock);
	descptr = priv->tx_remove_ptr;
	while (priv->tx_free_desc < TX_DCNT) {
		/* Check for errors */
		err = ioread16(ioaddr + MLSR);

		if (err & 0x0200)
			dev->stats.rx_fifo_errors++;
		if (err & (0x2000 | 0x4000))
			dev->stats.tx_carrier_errors++;

		dbg(DBG_TX_DONE, "descptr %p status x%x err x%x jiffies %lu\n",
		    descptr, descptr->status, err, jiffies);

		if (descptr->status & 0x8000)
			break; /* Not complete */
		skb_ptr = descptr->skb_ptr;
		pci_unmap_single(priv->pdev, le32_to_cpu(descptr->buf),
			skb_ptr->len, PCI_DMA_TODEVICE);
		/* Free buffer */
		dev_kfree_skb_irq(skb_ptr);
		descptr->skb_ptr = NULL;
		/* To next descriptor */
		descptr = descptr->vndescp;
		priv->tx_free_desc++;
	}
	priv->tx_remove_ptr = descptr;

	if (priv->tx_free_desc)
		netif_wake_queue(dev);
	spin_unlock(&priv->lock);
}

int r6040_poll(struct napi_struct *napi, int budget)
{
	struct r6040_private *priv =
		container_of(napi, struct r6040_private, napi);
	struct net_device *dev = priv->dev;
	void __iomem *ioaddr = priv->base;
	int work_done;

	work_done = r6040_rx(dev, budget);

	dbg(DBG_POLL, "budget x%x done x%x\n", budget, work_done);

	if (work_done < budget) {
		netif_rx_complete(dev, napi);
		/* Enable RX interrupt */
		iowrite16(ioread16(ioaddr + MIER) | RX_INTS, ioaddr + MIER);
	}
	return work_done;
}

/* The RDC interrupt handler. */
irqreturn_t r6040_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;
	u16 status;

	/* Read MISR status and clear */
	status = ioread16(ioaddr + MISR);

	dbg(DBG_IRQ, "status x%x jiffies %lu\n", status, jiffies);

	if (status == 0x0000 || status == 0xffff)
		return IRQ_NONE;

	/* rx early / rx finish interrupt
	   or rx descriptor unavail. */
	if (status & RX_INTS) {
		if (status & RX_NO_DESC) {
			/* rx descriptor unavail. */
			dev->stats.rx_dropped++;
			dev->stats.rx_missed_errors++;
		}
		/* Mask off RX interrupts */
		iowrite16(ioread16(ioaddr + MIER) & ~RX_INTS, ioaddr + MIER);
		netif_rx_schedule(dev, &lp->napi);
	}

	/* rx FIFO full */
	if (status & RX_FIFO_FULL) {
		dev->stats.rx_fifo_errors++;
	}
	
	/* TX interrupt request */
	if (status & 0x10)
		r6040_tx(dev);

	return IRQ_HANDLED;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
void r6040_poll_controller(struct net_device *dev)
{
	disable_irq(dev->irq);
	r6040_interrupt(dev->irq, dev);
	enable_irq(dev->irq);
}
#endif

/* Init RDC MAC */
int r6040_up(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;
	int rc;

	dbg(DBG_INIT, "ENTER\n");

	/* Initialise and alloc RX/TX buffers */
	r6040_init_txbufs(dev);
	if ((rc=r6040_alloc_rxbufs(dev)))
		return rc;

	/* Read the PHY ID */
	lp->switch_sig = phy_read(ioaddr, 0, 2);

	if (lp->switch_sig  == ICPLUS_PHY_ID) {
		phy_write(ioaddr, 29, 31, 0x175C); /* Enable registers */
		lp->phy_mode = 0x8000;
	} else {
		/* PHY Mode Check */
		phy_write(ioaddr, lp->phy_addr, 4, PHY_CAP);
		phy_write(ioaddr, lp->phy_addr, 0, PHY_MODE);

		if (PHY_MODE == 0x3100)
			lp->phy_mode = phy_mode_chk(dev);
		else
			lp->phy_mode = (PHY_MODE & 0x0100) ? 0x8000:0x0;
	}
	
/* configure duplex mode */
	lp->mcr0 |= lp->phy_mode;

	/* improve performance (by RDC guys) */
	phy_write(ioaddr, 30, 17, (phy_read(ioaddr, 30, 17) | 0x4000));
	phy_write(ioaddr, 30, 17, ~((~phy_read(ioaddr, 30, 17)) | 0x2000));
	phy_write(ioaddr, 0, 19, 0x0000);
	phy_write(ioaddr, 0, 30, 0x01F0);

	/* Reset MAC and init all registers */
	r6040_init_mac_regs(lp);

	return 0;
}

/*
  A periodic timer routine
	Polling PHY Chip Link Status
*/
void r6040_timer(unsigned long data)
{
	struct net_device *dev = (struct net_device *)data;
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;
	u16 phy_mode;

	/* Polling PHY Chip Status */
	if (PHY_MODE == 0x3100)
		phy_mode = phy_mode_chk(dev);
	else
		phy_mode = (PHY_MODE & 0x0100) ? 0x8000:0x0;

	if (phy_mode != lp->phy_mode) {
		lp->phy_mode = phy_mode;
		lp->mcr0 = (lp->mcr0 & 0x7fff) | phy_mode;
		iowrite16(lp->mcr0, ioaddr);
		printk(KERN_INFO "Link Change x%x \n", ioread16(ioaddr));
	}

	/* Timer active again */
	mod_timer(&lp->timer, jiffies + round_jiffies(HZ));
}

int r6040_open(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	int ret;

	dbg(DBG_OPEN, "ENTER\n");
	/* Request IRQ and Register interrupt handler */
	ret = request_irq(dev->irq, &r6040_interrupt,
		IRQF_SHARED, dev->name, dev);
	if (ret)
		return ret;

	dbg(DBG_OPEN, "got irq %d\n", dev->irq);

	/* Allocate Descriptor memory */
	lp->rx_ring =
		pci_alloc_consistent(lp->pdev, RX_DESC_SIZE, &lp->rx_ring_dma);
	if (!lp->rx_ring)
		return -ENOMEM;

	dbg(DBG_OPEN, "allocated rx ring\n");

	lp->tx_ring =
		pci_alloc_consistent(lp->pdev, TX_DESC_SIZE, &lp->tx_ring_dma);
	if (!lp->tx_ring) {
		pci_free_consistent(lp->pdev, RX_DESC_SIZE, lp->rx_ring,
				     lp->rx_ring_dma);
		return -ENOMEM;
	}

	dbg(DBG_OPEN, "allocated tx ring\n");

	if ((ret=r6040_up(dev))) {
		pci_free_consistent(lp->pdev, TX_DESC_SIZE, lp->tx_ring,
				    lp->tx_ring_dma);
		pci_free_consistent(lp->pdev, RX_DESC_SIZE, lp->rx_ring,
				     lp->rx_ring_dma);
		return ret;
	}
		
	napi_enable(&lp->napi);
	netif_start_queue(dev);

	/* set and active a timer process */
	setup_timer(&lp->timer, r6040_timer, (unsigned long) dev);
	if (lp->switch_sig != ICPLUS_PHY_ID)
		mod_timer(&lp->timer, jiffies + HZ);
	return 0;
}

int r6040_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	struct r6040_descriptor *descptr;
	void __iomem *ioaddr = lp->base;
	unsigned long flags;
	int ret = NETDEV_TX_OK;

	/* Critical Section */
	spin_lock_irqsave(&lp->lock, flags);

	/* TX resource check */
	if (!lp->tx_free_desc) {
		spin_unlock_irqrestore(&lp->lock, flags);
		netif_stop_queue(dev);
		printk(KERN_ERR DRV_NAME ": no tx descriptor\n");
		ret = NETDEV_TX_BUSY;
		return ret;
	}

	/* Statistic Counter */
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;
	/* Set TX descriptor & Transmit it */
	lp->tx_free_desc--;
	descptr = lp->tx_insert_ptr;
	if (skb->len < MISR)
		descptr->len = MISR;
	else
		descptr->len = skb->len;

	descptr->skb_ptr = skb;
	descptr->buf = cpu_to_le32(pci_map_single(lp->pdev,
		skb->data, skb->len, PCI_DMA_TODEVICE));

	dbg(DBG_TX_DESCR, "desc @ %p: len x%x buf %08x skb->data %p skb->len x%x jiffies %lu\n",
	    descptr, descptr->len, descptr->buf, skb->data, skb->len, jiffies);

	{
		char obuf[2*32+1];
		dbg(DBG_TX_DATA, "tx len x%x: %s\n",
		    descptr->len, hex2str(skb->data, obuf, sizeof(obuf)/2, '\0'));
	}

	descptr->status = 0x8000;
	/* Trigger the MAC to check the TX descriptor */
	iowrite16(0x01, ioaddr + MTPR);
	lp->tx_insert_ptr = descptr->vndescp;

	/* If no tx resource, stop */
	if (!lp->tx_free_desc)
		netif_stop_queue(dev);

	dev->trans_start = jiffies;
	spin_unlock_irqrestore(&lp->lock, flags);
	return ret;
}

/*! set MAC addresses and promiscous mode */
void r6040_multicast_list(struct net_device *dev)
{
	struct r6040_private *lp = netdev_priv(dev);
	void __iomem *ioaddr = lp->base;
	u16 *adrp;
	u16 reg;
	unsigned long flags;
	struct dev_mc_list *dmi = dev->mc_list;
	int i;
	char obuf[3*ETH_ALEN] __attribute__ ((unused));

	/* MAC Address */
	adrp = (u16 *)dev->dev_addr;
	iowrite16(adrp[0], ioaddr + MID_0L);
	iowrite16(adrp[1], ioaddr + MID_0M);
	iowrite16(adrp[2], ioaddr + MID_0H);

	dbg(DBG_MAC_ADDR, "%s: set MAC addr %s\n", 
	    dev->name, hex2str(dev->dev_addr, obuf, ETH_ALEN, ':'));

	/* Promiscous Mode */
	spin_lock_irqsave(&lp->lock, flags);

	/* Clear AMCP & PROM bits */
	reg = ioread16(ioaddr) & ~0x0120;
	if (dev->flags & IFF_PROMISC) {
		reg |= 0x0020;
		lp->mcr0 |= 0x0020;
	}
	/* Too many multicast addresses
	 * accept all traffic */
	else if ((dev->mc_count > MCAST_MAX)
		|| (dev->flags & IFF_ALLMULTI))
		reg |= 0x0020;

	iowrite16(reg, ioaddr);
	spin_unlock_irqrestore(&lp->lock, flags);

	/* Build the hash table */
	if (dev->mc_count > MCAST_MAX) {
		u16 hash_table[4];
		u32 crc;

		for (i = 0; i < 4; i++)
			hash_table[i] = 0;

		for (i = 0; i < dev->mc_count; i++) {
			char *addrs = dmi->dmi_addr;

			dmi = dmi->next;

			if (!(*addrs & 1))
				continue;

			crc = ether_crc_le(6, addrs);
			crc >>= 26;
			hash_table[crc >> 4] |= 1 << (15 - (crc & 0xf));
		}
		/* Write the index of the hash table */
		for (i = 0; i < 4; i++)
			iowrite16(hash_table[i] << 14, ioaddr + MCR1);
		/* Fill the MAC hash tables with their values */
		iowrite16(hash_table[0], ioaddr + MAR0);
		iowrite16(hash_table[1], ioaddr + MAR1);
		iowrite16(hash_table[2], ioaddr + MAR2);
		iowrite16(hash_table[3], ioaddr + MAR3);
	}
	/* Multicast Address 1~4 case */
	for (i = 0, dmi; (i < dev->mc_count) && (i < MCAST_MAX); i++) {
		adrp = (u16 *)dmi->dmi_addr;
		iowrite16(adrp[0], ioaddr + MID_1L + 8*i);
		iowrite16(adrp[1], ioaddr + MID_1M + 8*i);
		iowrite16(adrp[2], ioaddr + MID_1H + 8*i);
		dmi = dmi->next;
	}
	for (i = dev->mc_count; i < MCAST_MAX; i++) {
		iowrite16(0xffff, ioaddr + MID_0L + 8*i);
		iowrite16(0xffff, ioaddr + MID_0M + 8*i);
		iowrite16(0xffff, ioaddr + MID_0H + 8*i);
	}
}

STATIC void netdev_get_drvinfo(struct net_device *dev,
			struct ethtool_drvinfo *info)
{
	struct r6040_private *rp = netdev_priv(dev);

	strcpy(info->driver, DRV_NAME);
	strcpy(info->version, DRV_VERSION);
	strcpy(info->bus_info, pci_name(rp->pdev));
}

STATIC int netdev_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct r6040_private *rp = netdev_priv(dev);
	int rc;

	spin_lock_irq(&rp->lock);
	rc = mii_ethtool_gset(&rp->mii_if, cmd);
	spin_unlock_irq(&rp->lock);

	return rc;
}

STATIC int netdev_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct r6040_private *rp = netdev_priv(dev);
	int rc;

	spin_lock_irq(&rp->lock);
	rc = mii_ethtool_sset(&rp->mii_if, cmd);
	spin_unlock_irq(&rp->lock);
	r6040_set_carrier(&rp->mii_if);

	return rc;
}

STATIC u32 netdev_get_link(struct net_device *dev)
{
	struct r6040_private *rp = netdev_priv(dev);

	return mii_link_ok(&rp->mii_if);
}

static struct ethtool_ops netdev_ethtool_ops = {
	.get_drvinfo		= netdev_get_drvinfo,
	.get_settings		= netdev_get_settings,
	.set_settings		= netdev_set_settings,
	.get_link		= netdev_get_link,
};

int __devinit r6040_init_one(struct pci_dev *pdev,
					 const struct pci_device_id *ent)
{
	struct net_device *dev, *parent_dev;
	struct r6040_private *lp;
	void __iomem *ioaddr;
	int err, io_size = R6040_IO_SIZE;
	static int card_idx = -1;
	long pioaddr;

	printk(KERN_INFO "%s\n", version);
	printk(KERN_INFO DRV_NAME ": debug %x\n", debug);

	err = pci_enable_device(pdev);
	if (err)
		return err;

	/* this should always be supported */
	if (pci_set_dma_mask(pdev, DMA_32BIT_MASK)) {
		printk(KERN_ERR DRV_NAME "32-bit PCI DMA addresses"
				"not supported by the card\n");
		return -ENODEV;
	}
	if (pci_set_consistent_dma_mask(pdev, DMA_32BIT_MASK)) {
		printk(KERN_ERR DRV_NAME "32-bit PCI DMA addresses"
				"not supported by the card\n");
		return -ENODEV;
	}

	/* IO Size check */
	if (pci_resource_len(pdev, 0) < io_size) {
		printk(KERN_ERR "Insufficient PCI resources, aborting\n");
		return -EIO;
	}

	pioaddr = pci_resource_start(pdev, 0);	/* IO map base address */
	pci_set_master(pdev);

	dev = alloc_etherdev(sizeof(struct r6040_private));
	if (!dev) {
		printk(KERN_ERR "Failed to allocate etherdev\n");
		return -ENOMEM;
	}
	SET_NETDEV_DEV(dev, &pdev->dev);
	lp = netdev_priv(dev);

	if (pci_request_regions(pdev, DRV_NAME)) {
		printk(KERN_ERR DRV_NAME ": Failed to request PCI regions\n");
		err = -ENODEV;
		goto err_out_disable;
	}

	ioaddr = pci_iomap(pdev, 0, io_size);
	if (!ioaddr) {
		printk(KERN_ERR "ioremap failed for device %s\n",
			pci_name(pdev));
		return -EIO;
	}

	/* Init system & device */
	lp->base = ioaddr;
	dev->irq = pdev->irq;

	spin_lock_init(&lp->lock);
	pci_set_drvdata(pdev, dev);

	card_idx++;

	/* Link new device into r6040_root_dev */
	lp->pdev = pdev;

	lp->dev = dev;

	/* Init RDC private data */
	lp->mcr0 = 0x1002;
	lp->phy_addr = phy_table[card_idx];
	lp->switch_sig = 0;

	/* The RDC-specific entries in the device structure. */
	dev->open = &r6040_open;
	dev->hard_start_xmit = &r6040_start_xmit;
	dev->stop = &r6040_close;
	dev->get_stats = r6040_get_stats;
	dev->set_multicast_list = &r6040_multicast_list;
	dev->do_ioctl = &r6040_ioctl;
	dev->ethtool_ops = &netdev_ethtool_ops;
	dev->tx_timeout = &r6040_tx_timeout;
	dev->watchdog_timeo = TX_TIMEOUT;

	/*
	You must specify a netdevice with a "parent=" parameter, whose address
	is copied, or an array of bytes comprising a literal address; otherwise
	the (default) address of the Sitecom WL-153 bootloader is used.
	*/
	memcpy(dev->dev_addr, mac_base, ETH_ALEN);
	if (parent) {
		parent_dev = __dev_get_by_name(&init_net, parent);
		if (parent_dev)
			memcpy(dev->dev_addr, parent_dev->dev_addr, ETH_ALEN);
	}
	dev->dev_addr[ETH_ALEN-1] += card_idx ^ reverse; /* + 0 or 1 */

#ifdef CONFIG_NET_POLL_CONTROLLER
	dev->poll_controller = r6040_poll_controller;
#endif
	netif_napi_add(dev, &lp->napi, r6040_poll, 64);
	lp->mii_if.dev = dev;
	lp->mii_if.mdio_read = mdio_read;
	lp->mii_if.mdio_write = mdio_write;
	lp->mii_if.phy_id = lp->phy_addr;
	lp->mii_if.phy_id_mask = 0x1f;
	lp->mii_if.reg_num_mask = 0x1f;

	if (reverse && ((card_idx & 1) == 0) && (dev_alloc_name(dev, dev->name)
			>= 0))
		for (err = strlen(dev->name); err; err--) {
			if (dev->name[err - 1]++ != '9')
				break;
			dev->name[err - 1] = '0';
		}

	/* Register net device. After this dev->name assign */
	err = register_netdev(dev);
	if (err) {
		printk(KERN_ERR DRV_NAME ": Failed to register net device\n");
		goto err_out_res;
	}

	dbg(DBG_INIT, "%s successfully registered\n", dev->name);
	return 0;

err_out_res:
	pci_release_regions(pdev);
err_out_disable:
	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
	free_netdev(dev);

	return err;
}

void __devexit r6040_remove_one(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);

	unregister_netdev(dev);
	pci_release_regions(pdev);
	free_netdev(dev);
	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
}


static struct pci_device_id r6040_pci_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_RDC, 0x6040) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, r6040_pci_tbl);

static struct pci_driver r6040_driver = {
	.name		= DRV_NAME,
	.id_table	= r6040_pci_tbl,
	.probe		= r6040_init_one,
	.remove		= __devexit_p(r6040_remove_one),
};


static int __init r6040_init(void)
{
	return pci_register_driver(&r6040_driver);
}


static void __exit r6040_cleanup(void)
{
	pci_unregister_driver(&r6040_driver);
}

module_init(r6040_init);
module_exit(r6040_cleanup);
