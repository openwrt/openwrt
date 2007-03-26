/*
 *	ADM5120 built in ethernet switch driver
 *
 *	Copyright Jeroen Vreeken (pe1rxq@amsat.org), 2005
 *
 *	Inspiration for this driver came from the original ADMtek 2.4 
 *	driver, Copyright ADMtek Inc.
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

#include "adm5120_info.h"

MODULE_AUTHOR("Jeroen Vreeken (pe1rxq@amsat.org)");
MODULE_DESCRIPTION("ADM5120 ethernet switch driver");
MODULE_LICENSE("GPL");

/*
 *	The ADM5120 uses an internal matrix to determine which ports
 *	belong to which VLAN.
 *	The default generates a VLAN (and device) for each port 
 *	(including MII port) and the CPU port is part of all of them.
 *
 *	Another example, one big switch and everything mapped to eth0:
 *	0x7f, 0x00, 0x00, 0x00, 0x00, 0x00
 */
static unsigned char vlan_matrix[SW_DEVS] = {
	0x41, 0x42, 0x44, 0x48, 0x50, 0x60
};

static int adm5120_nrdevs;

static struct net_device *adm5120_devs[SW_DEVS];
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
static int adm5120_rxhi = 0;
static int adm5120_rxli = 0;
/* We don't use high priority tx for now */
/*static int adm5120_txhi = 0;*/
static int adm5120_txli = 0;
static int adm5120_txhit = 0;
static int adm5120_txlit = 0;
static int adm5120_if_open = 0;

static inline void adm5120_set_reg(unsigned int reg, unsigned long val)
{
	*(volatile unsigned long*)(SW_BASE+reg) = val;
}

static inline unsigned long adm5120_get_reg(unsigned int reg)
{
	return *(volatile unsigned long*)(SW_BASE+reg);
}

static inline void adm5120_rxfixup(struct adm5120_dma *dma,
    struct sk_buff **skbl, int num)
{
	int i;

	/* Resubmit the entire ring */
	for (i=0; i<num; i++) {
		dma[i].status = 0;
		dma[i].cntl = 0;
		dma[i].len = ADM5120_DMA_RXSIZE;
		dma[i].data = ADM5120_DMA_ADDR(skbl[i]->data) |
		     ADM5120_DMA_OWN | (i==num-1 ? ADM5120_DMA_RINGEND : 0);
	}
}

static inline void adm5120_rx(struct adm5120_dma *dma, struct sk_buff **skbl,
    int *index, int num)
{
	struct sk_buff *skb, *skbn;
	struct adm5120_sw *priv;
	struct net_device *dev;
	int port, vlan, len;

	while (!(dma[*index].data & ADM5120_DMA_OWN)) {
		port = (dma[*index].status & ADM5120_DMA_PORTID);
		port >>= ADM5120_DMA_PORTSHIFT;
		for (vlan = 0; vlan < adm5120_nrdevs; vlan++) {
			if ((1<<port) & vlan_matrix[vlan])
				break;
		}
		if (vlan == adm5120_nrdevs)
			vlan = 0;
		dev = adm5120_devs[vlan];
		skb = skbl[*index];
		len = (dma[*index].status & ADM5120_DMA_LEN);
		len >>= ADM5120_DMA_LENSHIFT;
		len -= ETH_FCS;

		priv = netdev_priv(dev);
		if (len <= 0 || len > ADM5120_DMA_RXSIZE ||
		    dma[*index].status & ADM5120_DMA_FCSERR) {
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
				priv->stats.rx_bytes+=len;
				skb_reserve(skbn, 2);
				skbl[*index] = skbn;
			} else {
				printk(KERN_INFO "%s recycling!\n", dev->name);
			}
		}

		dma[*index].status = 0;
		dma[*index].cntl = 0;
		dma[*index].len = ADM5120_DMA_RXSIZE;
		dma[*index].data = ADM5120_DMA_ADDR(skbl[*index]->data) |
		     ADM5120_DMA_OWN |
		     (num-1==*index ? ADM5120_DMA_RINGEND : 0);
		if (num == ++*index)
			*index = 0;
		if (skbn)
			netif_rx(skb);
	}
}

static inline void adm5120_tx(struct adm5120_dma *dma, struct sk_buff **skbl,
    int *index, int num)
{
	while((dma[*index].data & ADM5120_DMA_OWN) == 0 && skbl[*index]) {
		dev_kfree_skb_irq(skbl[*index]);
		skbl[*index] = NULL;
		if (++*index == num)
			*index = 0;
	}
}

irqreturn_t adm5120_sw_irq(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long intreg;

	adm5120_set_reg(ADM5120_INT_MASK,
	    adm5120_get_reg(ADM5120_INT_MASK) | ADM5120_INTHANDLE);

	intreg = adm5120_get_reg(ADM5120_INT_ST);
	adm5120_set_reg(ADM5120_INT_ST, intreg);

	if (intreg & ADM5120_INT_RXH)
		adm5120_rx(adm5120_dma_rxh, adm5120_skb_rxh, &adm5120_rxhi,
		ADM5120_DMA_RXH);
	if (intreg & ADM5120_INT_HFULL)
		adm5120_rxfixup(adm5120_dma_rxh, adm5120_skb_rxh,
		ADM5120_DMA_RXH);
	if (intreg & ADM5120_INT_RXL)
		adm5120_rx(adm5120_dma_rxl, adm5120_skb_rxl, &adm5120_rxli,
		    ADM5120_DMA_RXL);
	if (intreg & ADM5120_INT_LFULL)
		adm5120_rxfixup(adm5120_dma_rxl, adm5120_skb_rxl,
		ADM5120_DMA_RXL);
	if (intreg & ADM5120_INT_TXH)
		adm5120_tx(adm5120_dma_txh, adm5120_skb_txh, &adm5120_txhit,
		ADM5120_DMA_TXH);
	if (intreg & ADM5120_INT_TXL)
		adm5120_tx(adm5120_dma_txl, adm5120_skb_txl, &adm5120_txlit,
		ADM5120_DMA_TXL);

	adm5120_set_reg(ADM5120_INT_MASK,
	    adm5120_get_reg(ADM5120_INT_MASK) & ~ADM5120_INTHANDLE);

	return IRQ_HANDLED;
}

static void adm5120_set_vlan(char *matrix)
{
	unsigned long val;

	val = matrix[0] + (matrix[1]<<8) + (matrix[2]<<16) + (matrix[3]<<24);
	adm5120_set_reg(ADM5120_VLAN_GI, val);
	val = matrix[4] + (matrix[5]<<8);
	adm5120_set_reg(ADM5120_VLAN_GII, val);
}

static int adm5120_sw_open(struct net_device *dev)
{
	if (!adm5120_if_open++)
		adm5120_set_reg(ADM5120_INT_MASK,
		    adm5120_get_reg(ADM5120_INT_MASK) & ~ADM5120_INTHANDLE);
	netif_start_queue(dev);
	return 0;
}

static int adm5120_sw_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	if (!--adm5120_if_open)
		adm5120_set_reg(ADM5120_INT_MASK,
		    adm5120_get_reg(ADM5120_INT_MASK) | ADM5120_INTMASKALL);
	return 0;
}

static int adm5120_sw_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct adm5120_dma *dma = adm5120_dma_txl;
	struct sk_buff **skbl = adm5120_skb_txl;
	struct adm5120_sw *priv = netdev_priv(dev);
	int *index = &adm5120_txli;
	int num = ADM5120_DMA_TXL;
	int trigger = ADM5120_SEND_TRIG_L;

	dev->trans_start = jiffies;
	if (dma[*index].data & ADM5120_DMA_OWN) {
		dev_kfree_skb(skb);
		priv->stats.tx_dropped++;
		return 0;
	}

	dma[*index].data = ADM5120_DMA_ADDR(skb->data) | ADM5120_DMA_OWN;
	if (*index == num-1)
		dma[*index].data |= ADM5120_DMA_RINGEND;
	dma[*index].status =
	    ((skb->len<ETH_ZLEN?ETH_ZLEN:skb->len) << ADM5120_DMA_LENSHIFT) |
	    (0x1 << priv->port);
	dma[*index].len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	priv->stats.tx_packets++;
	priv->stats.tx_bytes += skb->len;
	skbl[*index]=skb;

	if (++*index == num)
		*index = 0;
	adm5120_set_reg(ADM5120_SEND_TRIG, trigger);

	return 0;
}

static void adm5120_tx_timeout(struct net_device *dev)
{
	netif_wake_queue(dev);
}

static struct net_device_stats *adm5120_sw_stats(struct net_device *dev)
{
	return &((struct adm5120_sw *)netdev_priv(dev))->stats;
}

static void adm5120_set_multicast_list(struct net_device *dev)
{
	struct adm5120_sw *priv = netdev_priv(dev);
	int portmask;

	portmask = vlan_matrix[priv->port] & 0x3f;

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
			err = copy_from_user(vlan_matrix, rq->ifr_data,
			    sizeof(vlan_matrix));
			if (err)
				return -EFAULT;
			adm5120_set_vlan(vlan_matrix);
			break;
		case SIOCGMATRIX:
			err = copy_to_user(rq->ifr_data, vlan_matrix,
			    sizeof(vlan_matrix));
			if (err)
				return -EFAULT;
			break;
		default:
			return -EOPNOTSUPP;
	}
	return 0;
}

static void adm5120_dma_tx_init(struct adm5120_dma *dma, struct sk_buff **skb,
    int num)
{
	memset(dma, 0, sizeof(struct adm5120_dma)*num);
	dma[num-1].data |= ADM5120_DMA_RINGEND;
	memset(skb, 0, sizeof(struct skb*)*num);
}

static void adm5120_dma_rx_init(struct adm5120_dma *dma, struct sk_buff **skb,
    int num)
{
	int i;

	memset(dma, 0, sizeof(struct adm5120_dma)*num);
	for (i=0; i<num; i++) {
		skb[i] = dev_alloc_skb(ADM5120_DMA_RXSIZE+16);
		if (!skb[i]) {
			i=num;
			break;
		}
		skb_reserve(skb[i], 2);
		dma[i].data = ADM5120_DMA_ADDR(skb[i]->data) | ADM5120_DMA_OWN;
		dma[i].cntl = 0;
		dma[i].len = ADM5120_DMA_RXSIZE;
		dma[i].status = 0;
	}
	dma[i-1].data |= ADM5120_DMA_RINGEND;
}

static int __init adm5120_sw_init(void)
{
	int i, err;
	struct net_device *dev;

	err = request_irq(SW_IRQ, adm5120_sw_irq, 0, "ethernet switch", NULL);
	if (err)
		goto out;

	/* MII port? */
	if (adm5120_get_reg(ADM5120_CODE) & ADM5120_CODE_PQFP)
		adm5120_nrdevs = 5;
	/* CFE based devices only have two enet ports */
	else if (adm5120_info.boot_loader == BOOT_LOADER_CFE)
		adm5120_nrdevs = 2;
	else
		adm5120_nrdevs = 6;

	adm5120_set_reg(ADM5120_CPUP_CONF,
	    ADM5120_DISCCPUPORT | ADM5120_CRC_PADDING |
	    ADM5120_DISUNALL | ADM5120_DISMCALL);
	adm5120_set_reg(ADM5120_PORT_CONF0, ADM5120_ENMC | ADM5120_ENBP);

	adm5120_set_reg(ADM5120_PHY_CNTL2, adm5120_get_reg(ADM5120_PHY_CNTL2) |
	    ADM5120_AUTONEG | ADM5120_NORMAL | ADM5120_AUTOMDIX);
	adm5120_set_reg(ADM5120_PHY_CNTL3, adm5120_get_reg(ADM5120_PHY_CNTL3) |
	    ADM5120_PHY_NTH);

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

	adm5120_set_vlan(vlan_matrix);
	
	for (i=0; i<adm5120_nrdevs; i++) {
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
		dev->irq = SW_IRQ;
		dev->open = adm5120_sw_open;
		dev->hard_start_xmit = adm5120_sw_tx;
		dev->stop = adm5120_sw_stop;
		dev->get_stats = adm5120_sw_stats;
		dev->set_multicast_list = adm5120_set_multicast_list;
		dev->do_ioctl = adm5120_do_ioctl;
		dev->tx_timeout = adm5120_tx_timeout;
		dev->watchdog_timeo = ETH_TX_TIMEOUT;
		dev->set_mac_address = adm5120_sw_set_mac_address;
		/* HACK alert!!!  In the original admtek driver it is asumed
		   that you can read the MAC addressess from flash, but edimax
		   decided to leave that space intentionally blank...
		 */
		memcpy(dev->dev_addr, "\x00\x50\xfc\x11\x22\x01", 6);
		dev->dev_addr[5] += i;
		adm5120_write_mac(dev);
		
		if ((err = register_netdev(dev))) {
			free_netdev(dev);
			goto out_int;
		}
		printk(KERN_INFO "%s: ADM5120 switch port%d\n", dev->name, i);
	}
	adm5120_set_reg(ADM5120_CPUP_CONF,
	    ADM5120_CRC_PADDING | ADM5120_DISUNALL | ADM5120_DISMCALL);

	return 0;

out_int:
	/* Undo everything that did succeed */
	for (; i; i--) {
		unregister_netdev(adm5120_devs[i-1]);
		free_netdev(adm5120_devs[i-1]);
	}
	free_irq(SW_IRQ, NULL);
out:
	printk(KERN_ERR "ADM5120 Ethernet switch init failed\n");
	return err;
}

static void __exit adm5120_sw_exit(void)
{
	int i;

	for (i = 0; i < adm5120_nrdevs; i++) {
		unregister_netdev(adm5120_devs[i]);
		free_netdev(adm5120_devs[i-1]);
	}

	free_irq(SW_IRQ, NULL);

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
