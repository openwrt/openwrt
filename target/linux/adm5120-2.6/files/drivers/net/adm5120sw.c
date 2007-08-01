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
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <asm/mipsregs.h>
#include <asm/irq.h>
#include <asm/io.h>
#include "adm5120sw.h"

#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_irq.h>

MODULE_AUTHOR("Jeroen Vreeken (pe1rxq@amsat.org)");
MODULE_DESCRIPTION("ADM5120 ethernet switch driver");
MODULE_LICENSE("GPL");

/* default settings - unlimited TX and RX on all ports, default shaper mode */
static unsigned char bw_matrix[SW_DEVS] = {
	0, 0, 0, 0, 0, 0
};

static int adm5120_nrdevs;

static struct net_device *adm5120_devs[SW_DEVS];
/* Lookup table port -> device */
static struct net_device *adm5120_port[SW_DEVS];

static struct adm5120_dma
    adm5120_dma_txh_v[ADM5120_DMA_TXH] __attribute__((aligned(16))),
    adm5120_dma_txl_v[ADM5120_DMA_TXL] __attribute__((aligned(16))),
    adm5120_dma_rxh_v[ADM5120_DMA_RXH] __attribute__((aligned(16))),
    adm5120_dma_rxl_v[ADM5120_DMA_RXL] __attribute__((aligned(16))),
    *adm5120_dma_txh,
    *adm5120_dma_txl,
    *adm5120_dma_rxh,
    *adm5120_dma_rxl;
static struct sk_buff
    *adm5120_skb_rxh[ADM5120_DMA_RXH],
    *adm5120_skb_rxl[ADM5120_DMA_RXL],
    *adm5120_skb_txh[ADM5120_DMA_TXH],
    *adm5120_skb_txl[ADM5120_DMA_TXL];
static int adm5120_rxli = 0;
static int adm5120_txli = 0;
/*static int adm5120_txhi = 0;*/
static int adm5120_if_open = 0;

static inline void adm5120_set_reg(unsigned int reg, unsigned long val)
{
	*(volatile unsigned long*)(SW_BASE+reg) = val;
}

static inline unsigned long adm5120_get_reg(unsigned int reg)
{
	return *(volatile unsigned long*)(SW_BASE+reg);
}

static inline void adm5120_rx_dma_update(struct adm5120_dma *dma,
	struct sk_buff *skb, int end)
{
	dma->status = 0;
	dma->cntl = 0;
	dma->len = ADM5120_DMA_RXSIZE;
	dma->data = ADM5120_DMA_ADDR(skb->data) |
		ADM5120_DMA_OWN | (end ? ADM5120_DMA_RINGEND : 0);
}

static int adm5120_rx(struct net_device *dev,int *budget)
{
	struct sk_buff *skb, *skbn;
	struct adm5120_sw *priv;
	struct net_device *cdev;
	struct adm5120_dma *dma;
	int port, len, quota;

	quota = min(dev->quota, *budget);
	dma = &adm5120_dma_rxl[adm5120_rxli];
	while (!(dma->data & ADM5120_DMA_OWN) && quota) {
		port = (dma->status & ADM5120_DMA_PORTID);
		port >>= ADM5120_DMA_PORTSHIFT;
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
		skb = adm5120_skb_rxl[adm5120_rxli];
		len = (dma->status & ADM5120_DMA_LEN);
		len >>= ADM5120_DMA_LENSHIFT;
		len -= ETH_FCS;

		priv = netdev_priv(dev);
		if (len <= 0 || len > ADM5120_DMA_RXSIZE ||
			dma->status & ADM5120_DMA_FCSERR) {
			priv->stats.rx_errors++;
			skbn = NULL;
		} else {
			skbn = dev_alloc_skb(ADM5120_DMA_RXSIZE+16);
			if (skbn) {
				skb_put(skb, len);
				skb->dev = dev;
				skb->protocol = eth_type_trans(skb, dev);
				skb->ip_summed = CHECKSUM_UNNECESSARY;
				dev->last_rx = jiffies;
				priv->stats.rx_packets++;
				priv->stats.rx_bytes += len;
				skb_reserve(skbn, NET_IP_ALIGN);
				adm5120_skb_rxl[adm5120_rxli] = skbn;
			} else {
				printk(KERN_INFO "%s recycling!\n", dev->name);
			}
		}
rx_skip:
		adm5120_rx_dma_update(&adm5120_dma_rxl[adm5120_rxli],
			adm5120_skb_rxl[adm5120_rxli],
			(ADM5120_DMA_RXL-1==adm5120_rxli));
		if (ADM5120_DMA_RXL == ++adm5120_rxli)
			adm5120_rxli = 0;
		dma = &adm5120_dma_rxl[adm5120_rxli];
		if (skbn){
			netif_receive_skb(skb);
			dev->quota--;
			(*budget)--;
			quota--;
		}
	} /* while */
	/* If there are still packets to process, return 1 */
	if (quota){
		/* No more packets to process, so disable the polling and reenable the interrupts */
		netif_rx_complete(dev);
		adm5120_set_reg(ADM5120_INT_MASK,
			adm5120_get_reg(ADM5120_INT_MASK) &
			~(ADM5120_INT_RXL|ADM5120_INT_LFULL));
		return 0;


	}
	return 1;
}

static irqreturn_t adm5120_sw_irq(int irq, void *dev_id)
{
	unsigned long intreg, intmask;
	int port;
	struct net_device *dev;

	intmask = adm5120_get_reg(ADM5120_INT_MASK);   /* Remember interrupt mask */
	adm5120_set_reg(ADM5120_INT_MASK, ADM5120_INTMASKALL); /* Disable interrupts */

	intreg = adm5120_get_reg(ADM5120_INT_ST); /* Read interrupt status */
	adm5120_set_reg(ADM5120_INT_ST, intreg);  /* Clear interrupt status */

	/* In NAPI operation the interrupts are disabled and the polling mechanism
	 * is activated. The interrupts are finally enabled again in the polling routine.
	 */
	if (intreg & (ADM5120_INT_RXL|ADM5120_INT_LFULL)) {
		/* check rx buffer for port number */
		port = adm5120_dma_rxl[adm5120_rxli].status & ADM5120_DMA_PORTID;
		port >>= ADM5120_DMA_PORTSHIFT;
		dev = adm5120_port[port];
		if ((dev==NULL) || !netif_running(dev)) {
		/* discard (update with old skb) */
			adm5120_rx_dma_update(&adm5120_dma_rxl[adm5120_rxli],
				adm5120_skb_rxl[adm5120_rxli],
				(ADM5120_DMA_RXL-1==adm5120_rxli));
			if (ADM5120_DMA_RXL == ++adm5120_rxli)
				adm5120_rxli = 0;
		}
		else {
			netif_rx_schedule(dev);
			intmask |= (ADM5120_INT_RXL|ADM5120_INT_LFULL); /* Disable RX interrupts */
		}
	}
#ifdef CONFIG_DEBUG
	if (intreg & ~(intmask))
		printk(KERN_INFO "adm5120sw: IRQ 0x%08X unexpected!\n", (unsigned int)(intreg & ~(intmask)));
#endif

	adm5120_set_reg(ADM5120_INT_MASK, intmask);

	return IRQ_HANDLED;
}

static void adm5120_set_vlan(char *matrix)
{
	unsigned long val;
	int vlan_port, port;

	val = matrix[0] + (matrix[1]<<8) + (matrix[2]<<16) + (matrix[3]<<24);
	adm5120_set_reg(ADM5120_VLAN_GI, val);
	val = matrix[4] + (matrix[5]<<8);
	adm5120_set_reg(ADM5120_VLAN_GII, val);
	/* Now set/update the port vs. device lookup table */
	for (port=0; port<SW_DEVS; port++) {
		for (vlan_port=0; vlan_port<SW_DEVS && !(matrix[vlan_port] & (0x00000001 << port)); vlan_port++);
		if (vlan_port <SW_DEVS)
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
	adm5120_set_reg(ADM5120_BW_CTL0, val);

	/* Port 4 and 5 are set using the bandwidth control 1 register */
	val = matrix[4];
	if (matrix[5] == 1)
		adm5120_set_reg(ADM5120_BW_CTL1, val | 0x80000000);
	else
		adm5120_set_reg(ADM5120_BW_CTL1, val & ~0x8000000);

	printk(KERN_DEBUG "D: ctl0 0x%lx, ctl1 0x%lx\n",
		adm5120_get_reg(ADM5120_BW_CTL0),
		adm5120_get_reg(ADM5120_BW_CTL1));
}

static int adm5120_sw_open(struct net_device *dev)
{
	unsigned long val;
	int i;

	netif_start_queue(dev);
	if (!adm5120_if_open++) {
		/* enable interrupts on first open */
		adm5120_set_reg(ADM5120_INT_MASK,
			adm5120_get_reg(ADM5120_INT_MASK) &
			~(ADM5120_INT_RXL|ADM5120_INT_LFULL));
	}
	/* enable (additional) port */
	val = adm5120_get_reg(ADM5120_PORT_CONF0);
	for (i=0; i<SW_DEVS; i++) {
		if (dev == adm5120_devs[i])
			val &= ~adm5120_eth_vlans[i];
	}
	adm5120_set_reg(ADM5120_PORT_CONF0, val);
	return 0;
}

static int adm5120_sw_stop(struct net_device *dev)
{
	unsigned long val;
	int i;

	if (!--adm5120_if_open) {
		adm5120_set_reg(ADM5120_INT_MASK, ADM5120_INTMASKALL);
	}
	/* disable port if not assigned to other devices */
	val = adm5120_get_reg(ADM5120_PORT_CONF0) | ADM5120_PORTDISALL;
	for (i=0; i<SW_DEVS; i++) {
		if ((dev != adm5120_devs[i]) && netif_running(adm5120_devs[i]))
			val &= ~adm5120_eth_vlans[i];
	}
	adm5120_set_reg(ADM5120_PORT_CONF0, val);
	netif_stop_queue(dev);
	return 0;
}

static int adm5120_sw_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct adm5120_dma *dma;
	struct sk_buff **skbl = adm5120_skb_txl;
	struct adm5120_sw *priv = netdev_priv(dev);
	unsigned long data;

	dev->trans_start = jiffies;
	dma = &adm5120_dma_txl[adm5120_txli];
	if (dma->data & ADM5120_DMA_OWN) {
		/* We want to write a packet but the TX queue is still
		 * occupied by the DMA. We are faster than the DMA... */
		dev_kfree_skb(skb);
		priv->stats.tx_dropped++;
		return 0;
	}
	data = ADM5120_DMA_ADDR(skb->data) | ADM5120_DMA_OWN;
	if (adm5120_txli == ADM5120_DMA_TXL-1)
		data |= ADM5120_DMA_RINGEND;
	dma->status =
	    ((skb->len<ETH_ZLEN?ETH_ZLEN:skb->len) << ADM5120_DMA_LENSHIFT) |
	    (0x1 << priv->port);

	dma->len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	priv->stats.tx_packets++;
	priv->stats.tx_bytes += skb->len;

	/* free old skbs here instead of tx completion interrupt:
	 * will hold some more memory allocated but reduces interrupts */
	if (skbl[adm5120_txli]){
		dev_kfree_skb(skbl[adm5120_txli]);
	}
	skbl[adm5120_txli] = skb;

	dma->data = data; /* Here we enable the buffer for the TX DMA machine */
	adm5120_set_reg(ADM5120_SEND_TRIG, ADM5120_SEND_TRIG_L);
	if (++adm5120_txli == ADM5120_DMA_TXL)
		adm5120_txli = 0;
	return 0;
}

static void adm5120_tx_timeout(struct net_device *dev)
{
	printk(KERN_INFO "%s: TX timeout\n",dev->name);
}

static struct net_device_stats *adm5120_sw_stats(struct net_device *dev)
{
	struct adm5120_sw *priv = netdev_priv(dev);
	int portmask;
	unsigned long adm5120_cpup_conf_reg;

	portmask = adm5120_eth_vlans[priv->port] & 0x3f;

	adm5120_cpup_conf_reg = adm5120_get_reg(ADM5120_CPUP_CONF);

	if (dev->flags & IFF_PROMISC)
		adm5120_cpup_conf_reg &= ~((portmask << ADM5120_DISUNSHIFT) & ADM5120_DISUNALL);
	else
		adm5120_cpup_conf_reg |= (portmask << ADM5120_DISUNSHIFT);

	if (dev->flags & IFF_PROMISC || dev->flags & IFF_ALLMULTI || dev->mc_count)
		adm5120_cpup_conf_reg &= ~((portmask << ADM5120_DISMCSHIFT) & ADM5120_DISMCALL);
	else
		adm5120_cpup_conf_reg |= (portmask << ADM5120_DISMCSHIFT);

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
	if (~adm5120_cpup_conf_reg & ADM5120_DISUNALL)
		adm5120_cpup_conf_reg |= ADM5120_BTM;  /* Set the BTM     */
	else
		adm5120_cpup_conf_reg &= ~ADM5120_BTM; /* Disable the BTM */

	adm5120_set_reg(ADM5120_CPUP_CONF,adm5120_cpup_conf_reg);

	return &((struct adm5120_sw *)netdev_priv(dev))->stats;
}

static void adm5120_set_multicast_list(struct net_device *dev)
{
	struct adm5120_sw *priv = netdev_priv(dev);
	int portmask;

	portmask = adm5120_eth_vlans[priv->port] & 0x3f;

	if (dev->flags & IFF_PROMISC)
		adm5120_set_reg(ADM5120_CPUP_CONF,
		    adm5120_get_reg(ADM5120_CPUP_CONF) &
		    ~((portmask << ADM5120_DISUNSHIFT) & ADM5120_DISUNALL));
	else
		adm5120_set_reg(ADM5120_CPUP_CONF,
		    adm5120_get_reg(ADM5120_CPUP_CONF) |
		    (portmask << ADM5120_DISUNSHIFT));

	if (dev->flags & IFF_PROMISC || dev->flags & IFF_ALLMULTI ||
	    dev->mc_count)
		adm5120_set_reg(ADM5120_CPUP_CONF,
		    adm5120_get_reg(ADM5120_CPUP_CONF) &
		    ~((portmask << ADM5120_DISMCSHIFT) & ADM5120_DISMCALL));
	else
		adm5120_set_reg(ADM5120_CPUP_CONF,
		    adm5120_get_reg(ADM5120_CPUP_CONF) |
		    (portmask << ADM5120_DISMCSHIFT));
}

static void adm5120_write_mac(struct net_device *dev)
{
	struct adm5120_sw *priv = netdev_priv(dev);
	unsigned char *mac = dev->dev_addr;

	adm5120_set_reg(ADM5120_MAC_WT1,
	    mac[2] | (mac[3]<<8) | (mac[4]<<16) | (mac[5]<<24));
	adm5120_set_reg(ADM5120_MAC_WT0, (priv->port<<3) |
	    (mac[0]<<16) | (mac[1]<<24) | ADM5120_MAC_WRITE | ADM5120_VLAN_EN);

	while (!(adm5120_get_reg(ADM5120_MAC_WT0) & ADM5120_MAC_WRITE_DONE));
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

static void adm5120_dma_tx_init(struct adm5120_dma *dma, struct sk_buff **skbl,
    int num)
{
	memset(dma, 0, sizeof(struct adm5120_dma)*num);
	dma[num-1].data |= ADM5120_DMA_RINGEND;
	memset(skbl, 0, sizeof(struct skb*)*num);
}

static void adm5120_dma_rx_init(struct adm5120_dma *dma, struct sk_buff **skbl,
    int num)
{
	int i;

	memset(dma, 0, sizeof(struct adm5120_dma)*num);
	for (i=0; i<num; i++) {
		skbl[i] = dev_alloc_skb(ADM5120_DMA_RXSIZE+16);
		if (!skbl[i]) {
			i=num;
			break;
		}
		skb_reserve(skbl[i], NET_IP_ALIGN);
		adm5120_rx_dma_update(&dma[i], skbl[i], (num-1==i));
	}
}

static int __init adm5120_sw_init(void)
{
	int i, err;
	struct net_device *dev;

	err = request_irq(ADM5120_IRQ_SWITCH, adm5120_sw_irq, 0, "ethernet switch", NULL);
	if (err)
		goto out;

	adm5120_nrdevs = adm5120_eth_num_ports;

	adm5120_set_reg(ADM5120_CPUP_CONF,
	    ADM5120_DISCCPUPORT | ADM5120_CRC_PADDING |
	    ADM5120_DISUNALL | ADM5120_DISMCALL);
	adm5120_set_reg(ADM5120_PORT_CONF0, ADM5120_ENMC | ADM5120_ENBP | ADM5120_PORTDISALL);

	adm5120_set_reg(ADM5120_PHY_CNTL2, adm5120_get_reg(ADM5120_PHY_CNTL2) |
	    ADM5120_AUTONEG | ADM5120_NORMAL | ADM5120_AUTOMDIX);
	adm5120_set_reg(ADM5120_PHY_CNTL3, adm5120_get_reg(ADM5120_PHY_CNTL3) |
	    ADM5120_PHY_NTH);

	/* Force all the packets from all ports are low priority */
	adm5120_set_reg(ADM5120_PRI_CNTL, 0);

	adm5120_set_reg(ADM5120_INT_MASK, ADM5120_INTMASKALL);
	adm5120_set_reg(ADM5120_INT_ST, ADM5120_INTMASKALL);

	adm5120_dma_txh = (void *)KSEG1ADDR((u32)adm5120_dma_txh_v);
	adm5120_dma_txl = (void *)KSEG1ADDR((u32)adm5120_dma_txl_v);
	adm5120_dma_rxh = (void *)KSEG1ADDR((u32)adm5120_dma_rxh_v);
	adm5120_dma_rxl = (void *)KSEG1ADDR((u32)adm5120_dma_rxl_v);

	adm5120_dma_tx_init(adm5120_dma_txh, adm5120_skb_txh, ADM5120_DMA_TXH);
	adm5120_dma_tx_init(adm5120_dma_txl, adm5120_skb_txl, ADM5120_DMA_TXL);
	adm5120_dma_rx_init(adm5120_dma_rxh, adm5120_skb_rxh, ADM5120_DMA_RXH);
	adm5120_dma_rx_init(adm5120_dma_rxl, adm5120_skb_rxl, ADM5120_DMA_RXL);
	adm5120_set_reg(ADM5120_SEND_HBADDR, KSEG1ADDR(adm5120_dma_txh));
	adm5120_set_reg(ADM5120_SEND_LBADDR, KSEG1ADDR(adm5120_dma_txl));
	adm5120_set_reg(ADM5120_RECEIVE_HBADDR, KSEG1ADDR(adm5120_dma_rxh));
	adm5120_set_reg(ADM5120_RECEIVE_LBADDR, KSEG1ADDR(adm5120_dma_rxl));

	for (i = 0; i < SW_DEVS; i++) {
		adm5120_devs[i] = alloc_etherdev(sizeof(struct adm5120_sw));
		if (!adm5120_devs[i]) {
			err = -ENOMEM;
			goto out_int;
		}

		dev = adm5120_devs[i];
		SET_MODULE_OWNER(dev);
		memset(netdev_priv(dev), 0, sizeof(struct adm5120_sw));
		((struct adm5120_sw*)netdev_priv(dev))->port = i;
		dev->base_addr = SW_BASE;
		dev->irq = ADM5120_IRQ_SWITCH;
		dev->open = adm5120_sw_open;
		dev->hard_start_xmit = adm5120_sw_tx;
		dev->stop = adm5120_sw_stop;
		dev->get_stats = adm5120_sw_stats;
		dev->set_multicast_list = adm5120_set_multicast_list;
		dev->do_ioctl = adm5120_do_ioctl;
		dev->tx_timeout = adm5120_tx_timeout;
		dev->watchdog_timeo = ETH_TX_TIMEOUT;
		dev->set_mac_address = adm5120_sw_set_mac_address;
		dev->poll = adm5120_rx;
		dev->weight = 64;

		memcpy(dev->dev_addr, adm5120_eth_macs[i], 6);
		adm5120_write_mac(dev);

		if ((err = register_netdev(dev))) {
			free_netdev(dev);
			goto out_int;
		}
		printk(KERN_INFO "%s: ADM5120 switch port%d\n", dev->name, i);
	}
	/* setup vlan/port mapping after devs are filled up */
	adm5120_set_vlan(adm5120_eth_vlans);

	adm5120_set_reg(ADM5120_CPUP_CONF,
	    ADM5120_CRC_PADDING | ADM5120_DISUNALL | ADM5120_DISMCALL);

	return 0;

out_int:
	/* Undo everything that did succeed */
	for (; i; i--) {
		unregister_netdev(adm5120_devs[i-1]);
		free_netdev(adm5120_devs[i-1]);
	}
	free_irq(ADM5120_IRQ_SWITCH, NULL);
out:
	printk(KERN_ERR "ADM5120 Ethernet switch init failed\n");
	return err;
}

static void __exit adm5120_sw_exit(void)
{
	int i;

	for (i = 0; i < SW_DEVS; i++) {
		unregister_netdev(adm5120_devs[i]);
		free_netdev(adm5120_devs[i]);
	}

	free_irq(ADM5120_IRQ_SWITCH, NULL);

	for (i = 0; i < ADM5120_DMA_RXH; i++) {
		if (!adm5120_skb_rxh[i])
			break;
		kfree_skb(adm5120_skb_rxh[i]);
	}
	for (i = 0; i < ADM5120_DMA_RXL; i++) {
		if (!adm5120_skb_rxl[i])
			break;
		kfree_skb(adm5120_skb_rxl[i]);
	}
}

module_init(adm5120_sw_init);
module_exit(adm5120_sw_exit);
