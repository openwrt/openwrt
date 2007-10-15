/*
 *	ADM5120 built in ethernet switch driver
 *
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 *
 *	Inspiration for this driver came from the original ADMtek 2.4
 *	driver, Copyright ADMtek Inc.
 *
 *	NAPI extensions by Thomas Langer (Thomas.Langer@infineon.com)
 *	and Friedrich Beckmann (Friedrich.Beckmann@infineon.com), 2007
 *
 *	TODO: Add support of high prio queues (currently disabled)
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

#include <linux/io.h>
#include <linux/irq.h>

#include <asm/mipsregs.h>

#include <adm5120_info.h>
#include <adm5120_defs.h>
#include <adm5120_irq.h>
#include <adm5120_switch.h>

#include "adm5120sw.h"

#define DRV_NAME	"adm5120-switch"
#define DRV_DESC	"ADM5120 built-in ethernet switch driver"
#define DRV_VERSION	"0.1.0"

MODULE_AUTHOR("Jeroen Vreeken (pe1rxq@amsat.org)");
MODULE_DESCRIPTION("ADM5120 ethernet switch driver");
MODULE_LICENSE("GPL");

/* ------------------------------------------------------------------------ */

#if 1 /*def ADM5120_SWITCH_DEBUG*/
#define SW_DBG(f, a...)		printk(KERN_DEBUG "%s: " f, DRV_NAME , ## a)
#else
#define SW_DBG(f, a...)		do {} while (0)
#endif
#define SW_ERR(f, a...)		printk(KERN_ERR "%s: " f, DRV_NAME , ## a)
#define SW_INFO(f, a...)	printk(KERN_INFO "%s: " f, DRV_NAME , ## a)

#define SWITCH_NUM_PORTS	6
#define ETH_CSUM_LEN		4

#define RX_MAX_PKTLEN	1550
#define RX_RING_SIZE	64

#define TX_RING_SIZE	32
#define TX_QUEUE_LEN	28	/* Limit ring entries actually used. */
#define TX_TIMEOUT	HZ*400

#define SKB_ALLOC_LEN		(RX_MAX_PKTLEN + 32)
#define SKB_RESERVE_LEN		(NET_IP_ALIGN + NET_SKB_PAD)

#define SWITCH_INTS_HIGH (SWITCH_INT_SHD | SWITCH_INT_RHD | SWITCH_INT_HDF)
#define SWITCH_INTS_LOW (SWITCH_INT_SLD | SWITCH_INT_RLD | SWITCH_INT_LDF)
#define SWITCH_INTS_ERR (SWITCH_INT_RDE | SWITCH_INT_SDE | SWITCH_INT_CPUH)
#define SWITCH_INTS_Q (SWITCH_INT_P0QF | SWITCH_INT_P1QF | SWITCH_INT_P2QF | \
			SWITCH_INT_P3QF | SWITCH_INT_P4QF | SWITCH_INT_P5QF | \
			SWITCH_INT_CPQF | SWITCH_INT_GQF)

#define SWITCH_INTS_ALL (SWITCH_INTS_HIGH | SWITCH_INTS_LOW | \
			SWITCH_INTS_ERR | SWITCH_INTS_Q | \
			SWITCH_INT_MD | SWITCH_INT_PSC)

#define SWITCH_INTS_USED (SWITCH_INTS_LOW | SWITCH_INT_PSC)
#define SWITCH_INTS_POLL (SWITCH_INT_RLD | SWITCH_INT_LDF)

/* ------------------------------------------------------------------------ */

struct dma_desc {
	__u32			buf1;
#define DESC_OWN		(1UL << 31)	/* Owned by the switch */
#define DESC_EOR		(1UL << 28)	/* End of Ring */
#define DESC_ADDR_MASK		0x1FFFFFF
#define DESC_ADDR(x)	((__u32)(x) & DESC_ADDR_MASK)
	__u32			buf2;
#define DESC_BUF2_EN		(1UL << 31)	/* Buffer 2 enable */
	__u32			buflen;
	__u32			misc;
/* definitions for tx/rx descriptors */
#define DESC_PKTLEN_SHIFT	16
#define DESC_PKTLEN_MASK	0x7FF
/* tx descriptor specific part */
#define DESC_CSUM		(1UL << 31)	/* Append checksum */
#define DESC_DSTPORT_SHIFT	8
#define DESC_DSTPORT_MASK	0x3F
#define DESC_VLAN_MASK		0x3F
/* rx descriptor specific part */
#define DESC_SRCPORT_SHIFT	12
#define DESC_SRCPORT_MASK	0x7
#define DESC_DA_MASK		0x3
#define DESC_DA_SHIFT		4
#define DESC_IPCSUM_FAIL	(1UL << 3)	/* IP checksum fail */
#define DESC_VLAN_TAG		(1UL << 2)	/* VLAN tag present */
#define DESC_TYPE_MASK		0x3		/* mask for Packet type */
#define DESC_TYPE_IP		0x0		/* IP packet */
#define DESC_TYPE_PPPoE		0x1		/* PPPoE packet */
} __attribute__ ((aligned(16)));

static inline u32 desc_get_srcport(struct dma_desc *desc)
{
	return (desc->misc >> DESC_SRCPORT_SHIFT) & DESC_SRCPORT_MASK;
}

static inline u32 desc_get_pktlen(struct dma_desc *desc)
{
	return (desc->misc >> DESC_PKTLEN_SHIFT) & DESC_PKTLEN_MASK;
}

static inline int desc_ipcsum_fail(struct dma_desc *desc)
{
	return ((desc->misc & DESC_IPCSUM_FAIL) != 0);
}

/* ------------------------------------------------------------------------ */

/* default settings - unlimited TX and RX on all ports, default shaper mode */
static unsigned char bw_matrix[SWITCH_NUM_PORTS] = {
	0, 0, 0, 0, 0, 0
};

static int adm5120_nrdevs;

static struct net_device *adm5120_devs[SWITCH_NUM_PORTS];
/* Lookup table port -> device */
static struct net_device *adm5120_port[SWITCH_NUM_PORTS];

static struct dma_desc txh_descs_v[TX_RING_SIZE] __attribute__((aligned(16)));
static struct dma_desc txl_descs_v[TX_RING_SIZE] __attribute__((aligned(16)));
static struct dma_desc rxh_descs_v[RX_RING_SIZE] __attribute__((aligned(16)));
static struct dma_desc rxl_descs_v[RX_RING_SIZE] __attribute__((aligned(16)));
static struct dma_desc *txh_descs;
static struct dma_desc *txl_descs;
static struct dma_desc *rxh_descs;
static struct dma_desc *rxl_descs;

static struct sk_buff *rxl_skbuff[RX_RING_SIZE];
static struct sk_buff *rxh_skbuff[RX_RING_SIZE];
static struct sk_buff *txl_skbuff[TX_RING_SIZE];
static struct sk_buff *txh_skbuff[TX_RING_SIZE];

static unsigned int cur_rxl, dirty_rxl; /* producer/consumer ring indices */
static unsigned int cur_txl, dirty_txl;

static unsigned int sw_used;

static spinlock_t sw_lock = SPIN_LOCK_UNLOCKED;

static struct net_device sw_dev;

/* ------------------------------------------------------------------------ */

static inline u32 sw_read_reg(u32 reg)
{
	return __raw_readl((void __iomem *)KSEG1ADDR(ADM5120_SWITCH_BASE)+reg);
}

static inline void sw_write_reg(u32 reg, u32 val)
{
	__raw_writel(val, (void __iomem *)KSEG1ADDR(ADM5120_SWITCH_BASE)+reg);
}

static inline void sw_int_disable(u32 mask)
{
	u32	t;

	t = sw_read_reg(SWITCH_REG_INT_MASK);
	t |= mask;
	sw_write_reg(SWITCH_REG_INT_MASK, t);
}

static inline void sw_int_enable(u32 mask)
{
	u32	t;

	t = sw_read_reg(SWITCH_REG_INT_MASK);
	t &= ~mask;
	sw_write_reg(SWITCH_REG_INT_MASK, t);
}

static inline void sw_int_ack(u32 mask)
{
	sw_write_reg(SWITCH_REG_INT_STATUS, mask);
}

/* ------------------------------------------------------------------------ */

static void sw_dump_desc(char *label, struct dma_desc *desc, int tx)
{
	u32 t;

	SW_DBG("%s %s desc/%p\n", label, tx ? "tx" : "rx", desc);

	t = desc->buf1;
	SW_DBG("    buf1 %08X addr=%08X; len=%08X %s%s\n", t,
		t & DESC_ADDR_MASK,
		desc->buflen,
		(t & DESC_OWN) ? "SWITCH" : "CPU",
		(t & DESC_EOR) ? " RE" : "");

	t = desc->buf2;
	SW_DBG("    buf2 %08X addr=%08X%s\n", desc->buf2,
		t & DESC_ADDR_MASK,
		(t & DESC_BUF2_EN) ? " EN" : "" );

	t = desc->misc;
	if (tx)
		SW_DBG("    misc %08X%s pktlen=%04X ports=%02X vlan=%02X\n", t,
			(t & DESC_CSUM) ? " CSUM" : "",
			(t >> DESC_PKTLEN_SHIFT) & DESC_PKTLEN_MASK,
			(t >> DESC_DSTPORT_SHIFT) & DESC_DSTPORT_MASK,
			t & DESC_VLAN_MASK);
	else
		SW_DBG("    misc %08X pktlen=%04X port=%d DA=%d%s%s type=%d\n",
			t,
			(t >> DESC_PKTLEN_SHIFT) & DESC_PKTLEN_MASK,
			(t >> DESC_SRCPORT_SHIFT) & DESC_SRCPORT_MASK,
			(t >> DESC_DA_SHIFT) & DESC_DA_MASK,
			(t & DESC_IPCSUM_FAIL) ? " IPCF" : "",
			(t & DESC_VLAN_TAG) ? " VLAN" : "",
			(t & DESC_TYPE_MASK));
}

static void sw_dump_intr_mask(char *label, u32 mask)
{
	SW_DBG("%s %08X%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
		label, mask,
		(mask & SWITCH_INT_SHD) ? " SHD" : "",
		(mask & SWITCH_INT_SLD) ? " SLD" : "",
		(mask & SWITCH_INT_RHD) ? " RHD" : "",
		(mask & SWITCH_INT_RLD) ? " RLD" : "",
		(mask & SWITCH_INT_HDF) ? " HDF" : "",
		(mask & SWITCH_INT_LDF) ? " LDF" : "",
		(mask & SWITCH_INT_P0QF) ? " P0QF" : "",
		(mask & SWITCH_INT_P1QF) ? " P1QF" : "",
		(mask & SWITCH_INT_P2QF) ? " P2QF" : "",
		(mask & SWITCH_INT_P3QF) ? " P3QF" : "",
		(mask & SWITCH_INT_P4QF) ? " P4QF" : "",
		(mask & SWITCH_INT_CPQF) ? " CPQF" : "",
		(mask & SWITCH_INT_GQF) ? " GQF" : "",
		(mask & SWITCH_INT_MD) ? " MD" : "",
		(mask & SWITCH_INT_BCS) ? " BCS" : "",
		(mask & SWITCH_INT_PSC) ? " PSC" : "",
		(mask & SWITCH_INT_ID) ? " ID" : "",
		(mask & SWITCH_INT_W0TE) ? " W0TE" : "",
		(mask & SWITCH_INT_W1TE) ? " W1TE" : "",
		(mask & SWITCH_INT_RDE) ? " RDE" : "",
		(mask & SWITCH_INT_SDE) ? " SDE" : "",
		(mask & SWITCH_INT_CPUH) ? " CPUH" : "");
}

/* ------------------------------------------------------------------------ */

static inline void adm5120_rx_dma_update(struct dma_desc *desc,
	struct sk_buff *skb, int end)
{
	desc->misc = 0;
	desc->buf2 = 0;
	desc->buflen = RX_MAX_PKTLEN;
	desc->buf1 = DESC_ADDR(skb->data) |
		DESC_OWN | (end ? DESC_EOR : 0);
}

static int adm5120_switch_rx(struct net_device *dev, int *budget)
{
	struct sk_buff *skb, *skbn;
	struct adm5120_sw *priv;
	struct net_device *cdev;
	struct dma_desc *desc;
	int len, quota;

	quota = min(dev->quota, *budget);
	SW_DBG("%s polling, quota=%d\n", dev->name, quota);

	sw_int_ack(SWITCH_INTS_POLL);

	desc = &rxl_descs[cur_rxl];
	while (!(desc->buf1 & DESC_OWN) && quota) {
		u32 port = desc_get_srcport(desc);
		cdev = adm5120_port[port];
		if (cdev != dev) {      /* The current packet belongs to a different device */
			if ((cdev==NULL) || !netif_running(cdev)) {
				/* discard (update with old skb) */
				skb = skbn = NULL;
				goto rx_skip;
			}
			else {
				netif_rx_schedule(cdev);/* Start polling next device */
				return 1;       /* return 1 -> More packets to process */
			}

		}
		skb = rxl_skbuff[cur_rxl];
		len = desc_get_pktlen(desc);
		len -= ETH_CSUM_LEN;

		priv = netdev_priv(dev);
		if (len <= 0 || len > RX_MAX_PKTLEN ||
			desc_ipcsum_fail(desc)) {
			dev->stats.rx_errors++;
			skbn = NULL;
		} else {
			skbn = dev_alloc_skb(SKB_ALLOC_LEN);
			if (skbn) {
				skb_put(skb, len);
				skb->dev = dev;
				skb->protocol = eth_type_trans(skb, dev);
				skb->ip_summed = CHECKSUM_UNNECESSARY;
				dev->last_rx = jiffies;
				dev->stats.rx_packets++;
				dev->stats.rx_bytes += len;
				skb_reserve(skbn, SKB_RESERVE_LEN);
				rxl_skbuff[cur_rxl] = skbn;
			} else {
				SW_INFO("%s recycling!\n", dev->name);
			}
		}
rx_skip:
		adm5120_rx_dma_update(&rxl_descs[cur_rxl],
			rxl_skbuff[cur_rxl],
			(RX_RING_SIZE-1==cur_rxl));
		if (RX_RING_SIZE == ++cur_rxl)
			cur_rxl = 0;
		desc = &rxl_descs[cur_rxl];
		if (skbn){
			netif_receive_skb(skb);
			dev->quota--;
			(*budget)--;
			quota--;
		}
	} /* while */

	if (quota) {
		netif_rx_complete(dev);
		sw_int_enable(SWITCH_INTS_POLL);
		return 0;
	}

	/* If there are still packets to process, return 1 */
	return 1;
}

static void adm5120_switch_tx(void)
{
	unsigned int entry;

	/* find and cleanup dirty tx descriptors */
	entry = dirty_txl % TX_RING_SIZE;
	while (dirty_txl != cur_txl) {
		struct dma_desc *desc = &txl_descs[entry];
		struct sk_buff *skb = txl_skbuff[entry];

		if (desc->buf1 & DESC_OWN)
			break;

		sw_dump_desc("tx done", desc, 1);
		if (netif_running(skb->dev)) {
			skb->dev->stats.tx_bytes += skb->len;
			skb->dev->stats.tx_packets++;
		}

		dev_kfree_skb_irq(skb);
		txl_skbuff[entry] = NULL;
		entry = (++dirty_txl) % TX_RING_SIZE;
	}

	if ((cur_txl - dirty_txl) < TX_QUEUE_LEN - 4) {
		/* wake up queue of all devices */
		int i;
		for (i = 0; i < SWITCH_NUM_PORTS; i++) {
			if (!adm5120_devs[i])
				continue;
			netif_wake_queue(adm5120_devs[i]);
		}
	}
}

static irqreturn_t adm5120_poll_irq(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	u32 status;

	status = sw_read_reg(SWITCH_REG_INT_STATUS);
	status &= ~(sw_read_reg(SWITCH_REG_INT_MASK));

	status &= SWITCH_INTS_POLL;
	if (!status)
		return IRQ_NONE;

	sw_int_disable(SWITCH_INTS_POLL);
	netif_rx_schedule(dev);

	SW_DBG("%s handling IRQ%d\n", dev->name, irq);
	return IRQ_HANDLED;
}

static irqreturn_t adm5120_switch_irq(int irq, void *dev_id)
{
	irqreturn_t ret;
	u32 status;

	status = sw_read_reg(SWITCH_REG_INT_STATUS);
	status &= ~(sw_read_reg(SWITCH_REG_INT_MASK));

	sw_dump_intr_mask("sw irq", status);

	status &= SWITCH_INTS_ALL & ~SWITCH_INTS_POLL;
	if (!status)
		return IRQ_NONE;

	sw_int_ack(status);

	if (status & SWITCH_INT_SLD) {
		spin_lock(&sw_lock);
		adm5120_switch_tx();
		spin_unlock(&sw_lock);
	}

	return IRQ_HANDLED;
}

static void adm5120_set_vlan(char *matrix)
{
	unsigned long val;
	int vlan_port, port;

	val = matrix[0] + (matrix[1]<<8) + (matrix[2]<<16) + (matrix[3]<<24);
	sw_write_reg(SWITCH_REG_VLAN_G1, val);
	val = matrix[4] + (matrix[5]<<8);
	sw_write_reg(SWITCH_REG_VLAN_G2, val);

	/* Now set/update the port vs. device lookup table */
	for (port=0; port<SWITCH_NUM_PORTS; port++) {
		for (vlan_port=0; vlan_port<SWITCH_NUM_PORTS && !(matrix[vlan_port] & (0x00000001 << port)); vlan_port++);
		if (vlan_port <SWITCH_NUM_PORTS)
			adm5120_port[port] = adm5120_devs[vlan_port];
		else
			adm5120_port[port] = NULL;
	}
}

static void adm5120_set_bw(char *matrix)
{
	unsigned long val;

	/* Port 0 to 3 are set using the bandwidth control 0 register */
	val = matrix[0] + (matrix[1]<<8) + (matrix[2]<<16) + (matrix[3]<<24);
	sw_write_reg(SWITCH_REG_BW_CNTL0, val);

	/* Port 4 and 5 are set using the bandwidth control 1 register */
	val = matrix[4];
	if (matrix[5] == 1)
		sw_write_reg(SWITCH_REG_BW_CNTL1, val | 0x80000000);
	else
		sw_write_reg(SWITCH_REG_BW_CNTL1, val & ~0x8000000);

	SW_DBG("D: ctl0 0x%ux, ctl1 0x%ux\n", sw_read_reg(SWITCH_REG_BW_CNTL0),
		sw_read_reg(SWITCH_REG_BW_CNTL1));
}

static int adm5120_switch_open(struct net_device *dev)
{
	u32 t;
	int i;

	netif_start_queue(dev);
	if (!sw_used++)
		/* enable interrupts on first open */
		sw_int_enable(SWITCH_INTS_USED);

	/* enable (additional) port */
	t = sw_read_reg(SWITCH_REG_PORT_CONF0);
	for (i = 0; i < SWITCH_NUM_PORTS; i++) {
		if (dev == adm5120_devs[i])
			t &= ~adm5120_eth_vlans[i];
	}
	sw_write_reg(SWITCH_REG_PORT_CONF0, t);

	return 0;
}

static int adm5120_switch_stop(struct net_device *dev)
{
	u32 t;
	int i;

	if (!--sw_used)
		sw_int_disable(SWITCH_INTS_USED);

	/* disable port if not assigned to other devices */
	t = sw_read_reg(SWITCH_REG_PORT_CONF0);
	t |= SWITCH_PORTS_NOCPU;
	for (i = 0; i < SWITCH_NUM_PORTS; i++) {
		if ((dev != adm5120_devs[i]) && netif_running(adm5120_devs[i]))
			t &= ~adm5120_eth_vlans[i];
	}
	sw_write_reg(SWITCH_REG_PORT_CONF0, t);

	netif_stop_queue(dev);
	return 0;
}

static int adm5120_sw_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct dma_desc *desc;
	struct adm5120_sw *priv = netdev_priv(dev);
	unsigned int entry;
	unsigned long data;

	/* calculate the next TX descriptor entry. */
	entry = cur_txl % TX_RING_SIZE;

	desc = &txl_descs[entry];
	if (desc->buf1 & DESC_OWN) {
		/* We want to write a packet but the TX queue is still
		 * occupied by the DMA. We are faster than the DMA... */
		dev_kfree_skb(skb);
		dev->stats.tx_dropped++;
		return 0;
	}

	txl_skbuff[entry] = skb;
	data = (desc->buf1 & DESC_EOR);
	data |= DESC_ADDR(skb->data);

	desc->misc =
	    ((skb->len<ETH_ZLEN?ETH_ZLEN:skb->len) << DESC_PKTLEN_SHIFT) |
	    (0x1 << priv->port);

	desc->buflen = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;

	/* lock switch irq */
	spin_lock_irq(&sw_lock);

	desc->buf1 = data | DESC_OWN;
	sw_write_reg(SWITCH_REG_SEND_TRIG, SEND_TRIG_STL);

	cur_txl++;
	if (cur_txl == dirty_txl + TX_QUEUE_LEN) {
		/* FIXME: stop queue for all devices */
		netif_stop_queue(dev);
	}

	dev->trans_start = jiffies;

	spin_unlock_irq(&sw_lock);

	return 0;
}

static void adm5120_tx_timeout(struct net_device *dev)
{
	SW_INFO("TX timeout on %s\n",dev->name);
}

static void adm5120_set_multicast_list(struct net_device *dev)
{
	struct adm5120_sw *priv = netdev_priv(dev);
	u32 ports;
	u32 t;

	ports = adm5120_eth_vlans[priv->port] & SWITCH_PORTS_NOCPU;

	t = sw_read_reg(SWITCH_REG_CPUP_CONF);
	if (dev->flags & IFF_PROMISC)
		/* enable unknown packets */
		t &= ~(ports << CPUP_CONF_DUNP_SHIFT);
	else
		/* disable unknown packets */
		t |= (ports << CPUP_CONF_DUNP_SHIFT);

	if (dev->flags & IFF_PROMISC || dev->flags & IFF_ALLMULTI ||
					dev->mc_count)
		/* enable multicast packets */
		t &= ~(ports << CPUP_CONF_DMCP_SHIFT);
	else
		/* disable multicast packets */
		t |= (ports << CPUP_CONF_DMCP_SHIFT);

	/* If there is any port configured to be in promiscuous mode, then the */
	/* Bridge Test Mode has to be activated. This will result in           */
	/* transporting also packets learned in another VLAN to be forwarded   */
	/* to the CPU.                                                         */
	/* The difficult scenario is when we want to build a bridge on the CPU.*/
	/* Assume we have port0 and the CPU port in VLAN0 and port1 and the    */
	/* CPU port in VLAN1. Now we build a bridge on the CPU between         */
	/* VLAN0 and VLAN1. Both ports of the VLANs are set in promisc mode.   */
	/* Now assume a packet with ethernet source address 99 enters port 0   */
	/* It will be forwarded to the CPU because it is unknown. Then the     */
	/* bridge in the CPU will send it to VLAN1 and it goes out at port 1.  */
	/* When now a packet with ethernet destination address 99 comes in at  */
	/* port 1 in VLAN1, then the switch has learned that this address is   */
	/* located at port 0 in VLAN0. Therefore the switch will drop          */
	/* this packet. In order to avoid this and to send the packet still    */
	/* to the CPU, the Bridge Test Mode has to be activated.               */

	/* Check if there is any vlan in promisc mode. */
	if (t & (SWITCH_PORTS_NOCPU << CPUP_CONF_DUNP_SHIFT))
		t &= ~CPUP_CONF_BTM; /* Disable Bridge Testing Mode */
	else
		t |= CPUP_CONF_BTM;  /* Enable Bridge Testing Mode */

	sw_write_reg(SWITCH_REG_CPUP_CONF, t);

}

static void adm5120_write_mac(struct net_device *dev)
{
	struct adm5120_sw *priv = netdev_priv(dev);
	unsigned char *mac = dev->dev_addr;
	u32 t;

	t = mac[2] | (mac[3] << MAC_WT1_MAC3_SHIFT) |
		(mac[4] << MAC_WT1_MAC4_SHIFT) | (mac[5] << MAC_WT1_MAC4_SHIFT);
	sw_write_reg(SWITCH_REG_MAC_WT1, t);

	t = (mac[0] << MAC_WT0_MAC0_SHIFT) | (mac[1] << MAC_WT0_MAC1_SHIFT) |
		MAC_WT0_MAWC | MAC_WT0_WVE | (priv->port<<3);

	sw_write_reg(SWITCH_REG_MAC_WT0, t);

	while (!(sw_read_reg(SWITCH_REG_MAC_WT0) & MAC_WT0_MWD));
}

static int adm5120_sw_set_mac_address(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
	adm5120_write_mac(dev);
	return 0;
}

static int adm5120_do_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	int err;
	struct adm5120_sw_info info;
	struct adm5120_sw *priv = netdev_priv(dev);

	switch(cmd) {
	case SIOCGADMINFO:
		info.magic = 0x5120;
		info.ports = adm5120_nrdevs;
		info.vlan = priv->port;
		err = copy_to_user(rq->ifr_data, &info, sizeof(info));
		if (err)
			return -EFAULT;
		break;
	case SIOCSMATRIX:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		err = copy_from_user(adm5120_eth_vlans, rq->ifr_data,
					sizeof(adm5120_eth_vlans));
		if (err)
			return -EFAULT;
		adm5120_set_vlan(adm5120_eth_vlans);
		break;
	case SIOCGMATRIX:
		err = copy_to_user(rq->ifr_data, adm5120_eth_vlans,
					sizeof(adm5120_eth_vlans));
		if (err)
			return -EFAULT;
		break;
	case SIOCGETBW:
		err = copy_to_user(rq->ifr_data, bw_matrix, sizeof(bw_matrix));
		if (err)
			return -EFAULT;
		break;
	case SIOCSETBW:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		err = copy_from_user(bw_matrix, rq->ifr_data, sizeof(bw_matrix));
		if (err)
			return -EFAULT;
		adm5120_set_bw(bw_matrix);
		break;
	default:
		return -EOPNOTSUPP;
	}
	return 0;
}

static void adm5120_dma_tx_init(struct dma_desc *desc, struct sk_buff **skbl,
		int num)
{
	memset(desc, 0, num * sizeof(*desc));
	desc[num-1].buf1 |= DESC_EOR;
	memset(skbl, 0, sizeof(struct skb*)*num);
}

static void adm5120_dma_rx_init(struct dma_desc *desc, struct sk_buff **skbl,
		int num)
{
	int i;

	memset(desc, 0, num * sizeof(*desc));
	for (i=0; i<num; i++) {
		skbl[i] = dev_alloc_skb(SKB_ALLOC_LEN);
		if (!skbl[i]) {
			i=num;
			break;
		}
		skb_reserve(skbl[i], SKB_RESERVE_LEN);
		adm5120_rx_dma_update(&desc[i], skbl[i], (num-1==i));
	}
}

static int __init adm5120_sw_init(void)
{
	struct net_device *dev;
	u32 t;
	int i, err;

	err = request_irq(ADM5120_IRQ_SWITCH, adm5120_switch_irq,
		(IRQF_SHARED | IRQF_DISABLED), "switch", &sw_dev);
	if (err)
		goto out;

	adm5120_nrdevs = adm5120_eth_num_ports;

	t = CPUP_CONF_DCPUP | CPUP_CONF_CRCP |
		SWITCH_PORTS_NOCPU << CPUP_CONF_DUNP_SHIFT |
		SWITCH_PORTS_NOCPU << CPUP_CONF_DMCP_SHIFT ;
	sw_write_reg(SWITCH_REG_CPUP_CONF, t);

	t = (SWITCH_PORTS_NOCPU << PORT_CONF0_EMCP_SHIFT) |
		(SWITCH_PORTS_NOCPU << PORT_CONF0_BP_SHIFT) |
		(SWITCH_PORTS_NOCPU);
	sw_write_reg(SWITCH_REG_PORT_CONF0, t);

	/* setup ports to Autoneg/100M/Full duplex/Auto MDIX */
	t =  SWITCH_PORTS_PHY |
		(SWITCH_PORTS_PHY << PHY_CNTL2_SC_SHIFT) |
		(SWITCH_PORTS_PHY << PHY_CNTL2_DC_SHIFT) |
		(SWITCH_PORTS_PHY << PHY_CNTL2_PHYR_SHIFT) |
		(SWITCH_PORTS_PHY << PHY_CNTL2_AMDIX_SHIFT) |
		PHY_CNTL2_RMAE;
	SW_WRITE_REG(PHY_CNTL2, t);

	t = sw_read_reg(SWITCH_REG_PHY_CNTL3);
	t |= PHY_CNTL3_RNT;
	sw_write_reg(SWITCH_REG_PHY_CNTL3, t);

	/* Force all the packets from all ports are low priority */
	sw_write_reg(SWITCH_REG_PRI_CNTL, 0);

	sw_int_disable(SWITCH_INTS_ALL);
	sw_int_ack(SWITCH_INTS_ALL);

	cur_txl = dirty_txl = 0;
	cur_rxl = dirty_rxl = 0;

	txh_descs = (void *)KSEG1ADDR((u32)txh_descs_v);
	txl_descs = (void *)KSEG1ADDR((u32)txl_descs_v);
	rxh_descs = (void *)KSEG1ADDR((u32)rxh_descs_v);
	rxl_descs = (void *)KSEG1ADDR((u32)rxl_descs_v);

	adm5120_dma_tx_init(txh_descs, txh_skbuff, TX_RING_SIZE);
	adm5120_dma_tx_init(txl_descs, txl_skbuff, TX_RING_SIZE);
	adm5120_dma_rx_init(rxh_descs, rxh_skbuff, RX_RING_SIZE);
	adm5120_dma_rx_init(rxl_descs, rxl_skbuff, RX_RING_SIZE);
	sw_write_reg(SWITCH_REG_SHDA, KSEG1ADDR(txh_descs));
	sw_write_reg(SWITCH_REG_SLDA, KSEG1ADDR(txl_descs));
	sw_write_reg(SWITCH_REG_RHDA, KSEG1ADDR(rxh_descs));
	sw_write_reg(SWITCH_REG_RLDA, KSEG1ADDR(rxl_descs));

	for (i = 0; i < SWITCH_NUM_PORTS; i++) {
		adm5120_devs[i] = alloc_etherdev(sizeof(struct adm5120_sw));
		if (!adm5120_devs[i]) {
			err = -ENOMEM;
			goto out_int;
		}

		dev = adm5120_devs[i];
		err = request_irq(ADM5120_IRQ_SWITCH, adm5120_poll_irq,
			(IRQF_SHARED | IRQF_DISABLED), dev->name, dev);
		if (err) {
			SW_ERR("unable to get irq for %s\n", dev->name);
			goto out_int;
		}

		SET_MODULE_OWNER(dev);
		memset(netdev_priv(dev), 0, sizeof(struct adm5120_sw));
		((struct adm5120_sw*)netdev_priv(dev))->port = i;
		dev->base_addr = ADM5120_SWITCH_BASE;
		dev->irq = ADM5120_IRQ_SWITCH;
		dev->open = adm5120_switch_open;
		dev->hard_start_xmit = adm5120_sw_start_xmit;
		dev->stop = adm5120_switch_stop;
		dev->set_multicast_list = adm5120_set_multicast_list;
		dev->do_ioctl = adm5120_do_ioctl;
		dev->tx_timeout = adm5120_tx_timeout;
		dev->watchdog_timeo = TX_TIMEOUT;
		dev->set_mac_address = adm5120_sw_set_mac_address;
		dev->poll = adm5120_switch_rx;
		dev->weight = 64;

		memcpy(dev->dev_addr, adm5120_eth_macs[i], 6);
		adm5120_write_mac(dev);

		if ((err = register_netdev(dev))) {
			free_irq(ADM5120_IRQ_SWITCH, dev);
			free_netdev(dev);
			goto out_int;
		}
		SW_INFO("%s created for switch port%d\n", dev->name, i);
	}
	/* setup vlan/port mapping after devs are filled up */
	adm5120_set_vlan(adm5120_eth_vlans);

	/* enable CPU port */
	t = sw_read_reg(SWITCH_REG_CPUP_CONF);
	t &= ~CPUP_CONF_DCPUP;
	sw_write_reg(SWITCH_REG_CPUP_CONF, t);

	return 0;

out_int:
	/* Undo everything that did succeed */
	for (; i; i--) {
		unregister_netdev(adm5120_devs[i-1]);
		free_netdev(adm5120_devs[i-1]);
	}
	free_irq(ADM5120_IRQ_SWITCH, NULL);
out:
	SW_ERR("init failed\n");
	return err;
}

static void __exit adm5120_sw_exit(void)
{
	int i;

	for (i = 0; i < SWITCH_NUM_PORTS; i++) {
		struct net_device *dev = adm5120_devs[i];
		unregister_netdev(dev);
		free_irq(ADM5120_IRQ_SWITCH, dev);
		free_netdev(dev);
	}

	free_irq(ADM5120_IRQ_SWITCH, &sw_dev);

	for (i = 0; i < RX_RING_SIZE; i++) {
		if (!rxh_skbuff[i])
			break;
		kfree_skb(rxh_skbuff[i]);
	}

	for (i = 0; i < RX_RING_SIZE; i++) {
		if (!rxl_skbuff[i])
			break;
		kfree_skb(rxl_skbuff[i]);
	}
}

module_init(adm5120_sw_init);
module_exit(adm5120_sw_exit);
