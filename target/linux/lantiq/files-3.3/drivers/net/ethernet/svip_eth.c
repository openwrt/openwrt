/************************************************************************
 *
 * Copyright (c) 2005
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <asm/checksum.h>

#if 1 /** TODO: MOVE TO APPROPRIATE PLACE */

#define ETHERNET_PACKET_DMA_BUFFER_SIZE		0x600
#define REV_MII_MODE 			2

#endif

#define DRV_NAME "ifxmips_mii0"

#include <lantiq_soc.h>
#include <svip_dma.h>

#ifdef CONFIG_DEBUG_MINI_BOOT
#define IKOS_MINI_BOOT
#endif

/* debugging */
#undef INCAIP2_SW_DUMP

#define INCAIP2_SW_EMSG(fmt,args...) printk("%s: " fmt, __FUNCTION__ , ##args)

#define INCAIP2_SW_CHIP_NO 1
#define INCAIP2_SW_CHIP_ID 0
#define INCAIP2_SW_DEVICE_NO 1

#ifdef INCAIP2_SW_DEBUG_MSG
#define INCAIP2_SW_DMSG(fmt,args...) printk("%s: " fmt, __FUNCTION__ , ##args)
#else
#define INCAIP2_SW_DMSG(fmt,args...)
#endif

/************************** Module Parameters *****************************/
static char *mode = "bridge";
module_param(mode, charp, 0000);
MODULE_PARM_DESC(mode, "<description>");

#ifdef HAVE_TX_TIMEOUT
static int timeout = 10*HZ;
module_param(timeout, int, 0);
MODULE_PARM_DESC(timeout, "Transmission watchdog timeout in seconds>");
#endif

#ifdef IKOS_MINI_BOOT
#ifdef CONFIG_INCAIP2
extern s32 incaip2_sw_to_mbx(struct sk_buff* skb);
#endif
extern s32 svip_sw_to_mbx(struct sk_buff* skb);
#endif

struct svip_mii_priv {
	struct net_device_stats stats;
	struct dma_device_info *dma_device;
	struct sk_buff *skb;
};

static struct net_device *svip_mii0_dev;
static unsigned char mac_addr[MAX_ADDR_LEN];
static unsigned char my_ethaddr[MAX_ADDR_LEN];

/**
 * Initialize MAC address.
 * This function copies the ethernet address from kernel command line.
 *
 * \param   line     Pointer to parameter
 * \return  0        OK
 * \ingroup Internal
 */
static int __init svip_eth_ethaddr_setup(char *line)
{
	char *ep;
	int i;

	memset(my_ethaddr, 0, MAX_ADDR_LEN);
	/* there should really be routines to do this stuff */
	for (i = 0; i < 6; i++)
	{
		my_ethaddr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
		if (line)
			line = (*ep) ? ep+1 : ep;
	}
	INCAIP2_SW_DMSG("mac address %2x-%2x-%2x-%2x-%2x-%2x \n"
			,my_ethaddr[0]
			,my_ethaddr[1]
			,my_ethaddr[2]
			,my_ethaddr[3]
			,my_ethaddr[4]
			,my_ethaddr[5]);
	return 0;
}
__setup("ethaddr=", svip_eth_ethaddr_setup);


/**
 * Open RX DMA channels.
 * This function opens all DMA rx channels.
 *
 * \param   dma_dev     pointer to DMA device information
 * \ingroup Internal
 */
static void svip_eth_open_rx_dma(struct dma_device_info *dma_dev)
{
	int i;

	for(i=0; i<dma_dev->num_rx_chan; i++)
	{
		dma_dev->rx_chan[i]->open(dma_dev->rx_chan[i]);
	}
}


/**
 * Open TX DMA channels.
 * This function opens all DMA tx channels.
 *
 * \param   dev      pointer to net device structure that comprises
 *                   DMA device information pointed to by it's priv field.
 * \ingroup Internal
 */
static void svip_eth_open_tx_dma(struct dma_device_info *dma_dev)
{
	int i;

	for (i=0; i<dma_dev->num_tx_chan; i++)
	{
		dma_dev->tx_chan[i]->open(dma_dev->tx_chan[i]);
	}
}


#ifdef CONFIG_NET_HW_FLOWCONTROL
/**
 * Enable receiving DMA.
 * This function enables the receiving DMA channel.
 *
 * \param   dev      pointer to net device structure that comprises
 *                   DMA device information pointed to by it's priv field.
 * \ingroup Internal
 */
void svip_eth_xon(struct net_device *dev)
{
	struct switch_priv *sw_dev = (struct switch_priv *)dev->priv;
	struct dma_device_info* dma_dev =
		(struct dma_device_info *)sw_dev->dma_device;
	unsigned long flag;

	local_irq_save(flag);

	INCAIP2_SW_DMSG("wakeup\n");
	svip_eth_open_rx_dma(dma_dev);

	local_irq_restore(flag);
}
#endif /* CONFIG_NET_HW_FLOWCONTROL */


/**
 * Open network device.
 * This functions opens the network device and starts the interface queue.
 *
 * \param   dev  Device structure for Ethernet device
 * \return  0    OK, device opened
 * \return  -1   Error, registering DMA device
 * \ingroup API
 */
int svip_mii_open(struct net_device *dev)
{
	struct svip_mii_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;

	svip_eth_open_rx_dma(dma_dev);
	svip_eth_open_tx_dma(dma_dev);

	netif_start_queue(dev);
	return 0;
}


/**
 * Close network device.
 * This functions closes the network device, which will also stop the interface
 * queue.
 *
 * \param   dev  Device structure for Ethernet device
 * \return  0    OK, device closed (cannot fail)
 * \ingroup API
 */
int svip_mii_release(struct net_device *dev)
{
	struct svip_mii_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->max_rx_chan_num; i++)
		dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);
	netif_stop_queue(dev);
	return 0;
}


/**
 * Read data from DMA device.
 * This function reads data from the DMA device. The function is called by
 * the switch/DMA pseudo interrupt handler dma_intr_handler on occurence of
 * a DMA receive interrupt.
 *
 * \param   dev      Pointer to network device structure
 * \param   dma_dev  Pointer to dma device structure
 * \return  OK       In case of successful data reception from dma
 *          -EIO     Incorrect opt pointer provided by device
 * \ingroup Internal
 */
int svip_mii_hw_receive(struct net_device *dev, struct dma_device_info *dma_dev)
{
	struct svip_mii_priv *priv = netdev_priv(dev);
	unsigned char *buf = NULL;
	struct sk_buff *skb = NULL;
	int len = 0;

	len = dma_device_read(dma_dev, &buf, (void **)&skb);

	if (len >= ETHERNET_PACKET_DMA_BUFFER_SIZE) {
		printk(KERN_INFO DRV_NAME ": packet too large %d\n", len);
		goto mii_hw_receive_err_exit;
	}

	if (skb == NULL) {
		printk(KERN_INFO DRV_NAME ": cannot restore pointer\n");
		goto mii_hw_receive_err_exit;
	}

	if (len > (skb->end - skb->tail)) {
		printk(KERN_INFO DRV_NAME ": BUG, len:%d end:%p tail:%p\n",
		       len, skb->end, skb->tail);
		goto mii_hw_receive_err_exit;
	}

	skb_put(skb, len);
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	netif_rx(skb);

	priv->stats.rx_packets++;
	priv->stats.rx_bytes += len;
	return 0;

mii_hw_receive_err_exit:
	if (len == 0) {
		if (skb)
			dev_kfree_skb_any(skb);
		priv->stats.rx_errors++;
		priv->stats.rx_dropped++;
		return -EIO;
	} else {
		return len;
	}
}


/**
 * Write data to Ethernet switch.
 * This function writes the data comprised in skb structure via DMA to the
 * Ethernet Switch. It is installed as the switch driver's hard_start_xmit
 * method.
 *
 * \param   skb  Pointer to socket buffer structure that contains the data
 *               to be sent
 * \param   dev  Pointer to network device structure which is used for
 *               data transmission
 * \return  1    Transmission error
 * \return  0    OK, successful data transmission
 * \ingroup API
 */
static int svip_mii_hw_tx(char *buf, int len, struct net_device *dev)
{
	int ret = 0;
	struct svip_mii_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	ret = dma_device_write(dma_dev, buf, len, priv->skb);
	return ret;
}

static int svip_mii_tx(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	char *data;
	struct svip_mii_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;

	len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	data = skb->data;
	priv->skb = skb;
	dev->trans_start = jiffies;
	/* TODO: we got more than 1 dma channel,
	   so we should do something intelligent here to select one */
	dma_dev->current_tx_chan = 0;

	wmb();

	if (svip_mii_hw_tx(data, len, dev) != len) {
		dev_kfree_skb_any(skb);
		priv->stats.tx_errors++;
		priv->stats.tx_dropped++;
	} else {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes += len;
	}

	return 0;
}


/**
 * Transmission timeout callback.
 * This functions is called when a trasmission timeout occurs. It will wake up
 * the interface queue again.
 *
 * \param   dev Device structure for Ethernet device
 * \ingroup API
 */
void svip_mii_tx_timeout(struct net_device *dev)
{
	int i;
	struct svip_mii_priv *priv = netdev_priv(dev);

	priv->stats.tx_errors++;
	for (i = 0; i < priv->dma_device->max_tx_chan_num; i++)
		priv->dma_device->tx_chan[i]->disable_irq(priv->dma_device->tx_chan[i]);
	netif_wake_queue(dev);
	return;
}


/**
 * Get device statistics.
 * This functions returns the device statistics, stored in the device structure.
 *
 * \param   dev   Device structure for Ethernet device
 * \return  stats Pointer to statistics structure
 * \ingroup API
 */
static struct net_device_stats *svip_get_stats(struct net_device *dev)
{
	struct svip_mii_priv *priv = netdev_priv(dev);
	return &priv->stats;
}


/**
 * Pseudo Interrupt handler for DMA.
 * This function processes DMA interrupts notified to the switch device driver.
 * The function is installed at the DMA core as interrupt handler for the
 * switch dma device.
 * It handles the following DMA interrupts:
 * passes received data to the upper layer in case of rx interrupt,
 * In case of a dma receive interrupt the received data is passed to the upper layer.
 * In case of a transmit buffer full interrupt the transmit queue is stopped.
 * In case of a transmission complete interrupt the transmit queue is restarted.
 *
 * \param   dma_dev pointer to dma device structure
 * \param   status  type of interrupt being notified (RCV_INT: dma receive
 *                  interrupt, TX_BUF_FULL_INT: transmit buffer full interrupt,
 *                  TRANSMIT_CPT_INT: transmission complete interrupt)
 * \return  OK      In case of successful data reception from dma
 * \ingroup Internal
 */
int dma_intr_handler(struct dma_device_info *dma_dev, int status)
{
	int i;

	switch (status) {
	case RCV_INT:
		svip_mii_hw_receive(svip_mii0_dev, dma_dev);
		break;

	case TX_BUF_FULL_INT:
		printk(KERN_INFO DRV_NAME ": tx buffer full\n");
		netif_stop_queue(svip_mii0_dev);
		for (i = 0; i < dma_dev->max_tx_chan_num; i++) {
			if ((dma_dev->tx_chan[i])->control == LTQ_DMA_CH_ON)
				dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
		}
		break;

	case TRANSMIT_CPT_INT:

#if 0
		for (i = 0; i < dma_dev->max_tx_chan_num; i++)
#if 0
			dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
#else
		dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i], (char *)__FUNCTION__);
#endif
		netif_wake_queue(svip_mii0_dev);
#endif
		break;
	}

	return 0;
}


/**
 * Allocates buffer sufficient for Ethernet Frame.
 * This function is installed as DMA callback function to be called on DMA
 * receive interrupt.
 *
 * \param   len          Unused
 * \param   *byte_offset Pointer to byte offset
 * \param   **opt        pointer to skb structure
 * \return  NULL         In case of buffer allocation fails
 *          buffer       Pointer to allocated memory
 * \ingroup Internal
 */
unsigned char *svip_etop_dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
	unsigned char *buffer = NULL;
	struct sk_buff *skb = NULL;

	skb = dev_alloc_skb(ETHERNET_PACKET_DMA_BUFFER_SIZE);
	if (skb == NULL)
		return NULL;

	buffer = (unsigned char *)(skb->data);
	skb_reserve(skb, 2);
	*(int *)opt = (int)skb;
	*byte_offset = 2;

	return buffer;
}


/**
 * Free DMA buffer.
 * This function frees a buffer, which can be either a data buffer or an
 * skb structure.
 *
 * \param   *dataptr Pointer to data buffer
 * \param   *opt     Pointer to skb structure
 * \return  0        OK
 * \ingroup Internal
 */
void svip_etop_dma_buffer_free(unsigned char *dataptr, void *opt)
{
	struct sk_buff *skb = NULL;

	if (opt == NULL) {
		kfree(dataptr);
	} else {
		skb = (struct sk_buff *)opt;
		dev_kfree_skb_any(skb);
	}
}

static int svip_mii_dev_init(struct net_device *dev);

static const struct net_device_ops svip_eth_netdev_ops = {
	.ndo_init = svip_mii_dev_init,
	.ndo_open = svip_mii_open,
	.ndo_stop = svip_mii_release,
	.ndo_start_xmit = svip_mii_tx,
	.ndo_get_stats = svip_get_stats,
	.ndo_tx_timeout = svip_mii_tx_timeout,
};

//#include <linux/device.h>

/**
 * Initialize switch driver.
 * This functions initializes the switch driver structures and registers the
 * Ethernet device.
 *
 * \param   dev    Device structure for Ethernet device
 * \return  0      OK
 * \return  ENOMEM No memory for structures available
 * \return  -1     Error during DMA init or Ethernet address configuration.
 * \ingroup API
 */
static int svip_mii_dev_init(struct net_device *dev)
{
	int i;
	struct svip_mii_priv *priv = netdev_priv(dev);


	ether_setup(dev);
	printk(KERN_INFO DRV_NAME ": %s is up\n", dev->name);
	dev->watchdog_timeo = 10 * HZ;
	memset(priv, 0, sizeof(*priv));
	priv->dma_device = dma_device_reserve("SW");
	if (!priv->dma_device) {
		BUG();
		return -ENODEV;
	}
	priv->dma_device->buffer_alloc = svip_etop_dma_buffer_alloc;
	priv->dma_device->buffer_free = svip_etop_dma_buffer_free;
	priv->dma_device->intr_handler = dma_intr_handler;

	for (i = 0; i < priv->dma_device->max_rx_chan_num; i++)
		priv->dma_device->rx_chan[i]->packet_size =
			ETHERNET_PACKET_DMA_BUFFER_SIZE;

	for (i = 0; i < priv->dma_device->max_tx_chan_num; i++) {
		priv->dma_device->tx_chan[i]->tx_weight=DEFAULT_SW_CHANNEL_WEIGHT;
		priv->dma_device->tx_chan[i]->packet_size =
			ETHERNET_PACKET_DMA_BUFFER_SIZE;
	}

	dma_device_register(priv->dma_device);

	printk(KERN_INFO DRV_NAME ": using mac=");

	for (i = 0; i < 6; i++) {
		dev->dev_addr[i] = mac_addr[i];
		printk("%02X%c", dev->dev_addr[i], (i == 5) ? ('\n') : (':'));
	}

	return 0;
}

static void svip_mii_chip_init(int mode)
{
}

static int svip_mii_probe(struct platform_device *dev)
{
	int result = 0;
	unsigned char *mac = (unsigned char *)dev->dev.platform_data;
	svip_mii0_dev = alloc_etherdev(sizeof(struct svip_mii_priv));
	svip_mii0_dev->netdev_ops = &svip_eth_netdev_ops;
	memcpy(mac_addr, mac, 6);
	strcpy(svip_mii0_dev->name, "eth%d");
	svip_mii_chip_init(REV_MII_MODE);
	result = register_netdev(svip_mii0_dev);
	if (result) {
		printk(KERN_INFO DRV_NAME
		       ": error %i registering device \"%s\"\n",
		       result, svip_mii0_dev->name);
		goto out;
	}
	printk(KERN_INFO DRV_NAME ": driver loaded!\n");

out:
	return result;
}

static int svip_mii_remove(struct platform_device *dev)
{
	struct svip_mii_priv *priv = netdev_priv(svip_mii0_dev);

	printk(KERN_INFO DRV_NAME ": cleanup\n");

	dma_device_unregister(priv->dma_device);
	dma_device_release(priv->dma_device);
	kfree(priv->dma_device);
	unregister_netdev(svip_mii0_dev);
	free_netdev(svip_mii0_dev);
	return 0;
}


static struct platform_driver svip_mii_driver = {
	.probe = svip_mii_probe,
	.remove = svip_mii_remove,
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
};


/**
 * Initialize switch driver as module.
 * This functions initializes the switch driver structures and registers the
 * Ethernet device for module usage.
 *
 * \return  0      OK
 * \return  ENODEV An error occured during initialization
 * \ingroup API
 */
int __init svip_mii_init(void)
{
	int ret = platform_driver_register(&svip_mii_driver);
	if (ret)
		printk(KERN_INFO DRV_NAME
		       ": Error registering platfom driver!\n");
	return ret;
}


/**
 * Remove driver module.
 * This functions removes the driver and unregisters all devices.
 *
 * \ingroup API
 */
static void __exit svip_mii_cleanup(void)
{
	platform_driver_unregister(&svip_mii_driver);
}

module_init(svip_mii_init);
module_exit(svip_mii_cleanup);

MODULE_LICENSE("GPL");
