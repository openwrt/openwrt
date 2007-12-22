/*
 *   drivers/net/ifxmips_mii0.c
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2005 Infineon
 *
 *   Rewrite of Infineon IFXMips code, thanks to infineon for the support,
 *   software and hardware
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/mm.h>
#include <linux/ethtool.h>
#include <asm/checksum.h>
#include <linux/init.h>
#include <asm/delay.h>
#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_mii0.h>
#include <asm/ifxmips/ifxmips_dma.h>
#include <asm/ifxmips/ifxmips_pmu.h>

static struct net_device ifxmips_mii0_dev;
static unsigned char u_boot_ethaddr[MAX_ADDR_LEN];

void
ifxmips_write_mdio (u32 phy_addr, u32 phy_reg, u16 phy_data)
{
	u32 val = MDIO_ACC_REQUEST |
		((phy_addr & MDIO_ACC_ADDR_MASK) << MDIO_ACC_ADDR_OFFSET) |
		((phy_reg & MDIO_ACC_REG_MASK) << MDIO_ACC_REG_OFFSET) |
		phy_data;

	while (readl(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_REQUEST);
	writel(val, IFXMIPS_PPE32_MDIO_ACC);
}

unsigned short
ifxmips_read_mdio (u32 phy_addr, u32 phy_reg)
{
	u32 val = MDIO_ACC_REQUEST | MDIO_ACC_READ |
		((phy_addr & MDIO_ACC_ADDR_MASK) << MDIO_ACC_ADDR_OFFSET) |
		((phy_reg & MDIO_ACC_REG_MASK) << MDIO_ACC_REG_OFFSET);

	writel(val, IFXMIPS_PPE32_MDIO_ACC);
	while (readl(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_REQUEST){};
	val = readl(IFXMIPS_PPE32_MDIO_ACC) & MDIO_ACC_VAL_MASK;

	return val;
}

int
ifxmips_switch_open (struct net_device *dev)
{
	struct switch_priv* priv = (struct switch_priv*)dev->priv;
	struct dma_device_info* dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->max_rx_chan_num; i++)
	{
		if ((dma_dev->rx_chan[i])->control == IFXMIPS_DMA_CH_ON)
			(dma_dev->rx_chan[i])->open(dma_dev->rx_chan[i]);
	}

	netif_start_queue(dev);

	return 0;
}

int
switch_release (struct net_device *dev){
	struct switch_priv* priv = (struct switch_priv*)dev->priv;
	struct dma_device_info* dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->max_rx_chan_num; i++)
		dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);

	netif_stop_queue(dev);

	return 0;
}

int
switch_hw_receive (struct net_device* dev,struct dma_device_info* dma_dev)
{
	struct switch_priv *priv = (struct switch_priv*)dev->priv;
	unsigned char* buf = NULL;
	struct sk_buff *skb = NULL;
	int len = 0;

	len = dma_device_read(dma_dev, &buf, (void**)&skb);

	if (len >= ETHERNET_PACKET_DMA_BUFFER_SIZE)
	{
		printk("packet too large %d\n",len);
		goto switch_hw_receive_err_exit;
	}

	/* remove CRC */
	len -= 4;
	if (skb == NULL )
	{
		printk("cannot restore pointer\n");
		goto switch_hw_receive_err_exit;
	}

	if (len > (skb->end - skb->tail))
	{
		printk("BUG, len:%d end:%p tail:%p\n", (len+4), skb->end, skb->tail);
		goto switch_hw_receive_err_exit;
	}

	skb_put(skb, len);
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	netif_rx(skb);

	priv->stats.rx_packets++;
	priv->stats.rx_bytes += len;

	return 0;

switch_hw_receive_err_exit:
	if (len == 0)
	{
		if(skb)
			dev_kfree_skb_any(skb);
		priv->stats.rx_errors++;
		priv->stats.rx_dropped++;

		return -EIO;
	} else {
		return len;
	}
}

int
switch_hw_tx (char *buf, int len, struct net_device *dev)
{
	int ret = 0;
	struct switch_priv *priv = dev->priv;
	struct dma_device_info* dma_dev = priv->dma_device;

	ret = dma_device_write(dma_dev, buf, len, priv->skb);

	return ret;
}

int
switch_tx (struct sk_buff *skb, struct net_device *dev)
{
	int len;
	char *data;
	struct switch_priv *priv = dev->priv;
	struct dma_device_info* dma_dev = priv->dma_device;

	len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	data = skb->data;
	priv->skb = skb;
	dev->trans_start = jiffies;
	// TODO we got more than 1 dma channel, so we should do something intelligent
	// here to select one
	dma_dev->current_tx_chan = 0;

	wmb();

	if (switch_hw_tx(data, len, dev) != len)
	{
		dev_kfree_skb_any(skb);
		priv->stats.tx_errors++;
		priv->stats.tx_dropped++;
	} else {
		priv->stats.tx_packets++;
		priv->stats.tx_bytes+=len;
	}

	return 0;
}

void
switch_tx_timeout (struct net_device *dev)
{
	int i;
	struct switch_priv* priv = (struct switch_priv*)dev->priv;

	priv->stats.tx_errors++;

	for (i = 0; i < priv->dma_device->max_tx_chan_num; i++)
	{
		priv->dma_device->tx_chan[i]->disable_irq(priv->dma_device->tx_chan[i]);
	}

	netif_wake_queue(dev);

	return;
}

int
dma_intr_handler (struct dma_device_info* dma_dev, int status)
{
	int i;

	switch (status)
	{
	case RCV_INT:
		switch_hw_receive(&ifxmips_mii0_dev, dma_dev);
		break;

	case TX_BUF_FULL_INT:
		printk("tx buffer full\n");
		netif_stop_queue(&ifxmips_mii0_dev);
		for (i = 0; i < dma_dev->max_tx_chan_num; i++)
		{
			if ((dma_dev->tx_chan[i])->control==IFXMIPS_DMA_CH_ON)
				dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
		}
		break;

	case TRANSMIT_CPT_INT:
		for (i = 0; i < dma_dev->max_tx_chan_num; i++)
			dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);

		netif_wake_queue(&ifxmips_mii0_dev);
		break;
	}

	return 0;
}

unsigned char*
ifxmips_etop_dma_buffer_alloc (int len, int *byte_offset, void **opt)
{
	unsigned char *buffer = NULL;
	struct sk_buff *skb = NULL;

	skb = dev_alloc_skb(ETHERNET_PACKET_DMA_BUFFER_SIZE);
	if (skb == NULL)
		return NULL;

	buffer = (unsigned char*)(skb->data);
	skb_reserve(skb, 2);
	*(int*)opt = (int)skb;
	*byte_offset = 2;

	return buffer;
}

void
ifxmips_etop_dma_buffer_free (unsigned char *dataptr, void *opt)
{
	struct sk_buff *skb = NULL;

	if(opt == NULL)
	{
		kfree(dataptr);
	} else {
		skb = (struct sk_buff*)opt;
		dev_kfree_skb_any(skb);
	}
}

static struct net_device_stats*
ifxmips_get_stats (struct net_device *dev)
{
	return (struct net_device_stats *)dev->priv;
}

static int
switch_init (struct net_device *dev)
{
	u64 retval = 0;
	int i;
	struct switch_priv *priv;

	ether_setup(dev);

	printk("%s up\n", dev->name);

	dev->open = ifxmips_switch_open;
	dev->stop = switch_release;
	dev->hard_start_xmit = switch_tx;
	dev->get_stats = ifxmips_get_stats;
	dev->tx_timeout = switch_tx_timeout;
	dev->watchdog_timeo = 10 * HZ;
	dev->priv = kmalloc(sizeof(struct switch_priv), GFP_KERNEL);

	if (dev->priv == NULL)
		return -ENOMEM;

	memset(dev->priv, 0, sizeof(struct switch_priv));
	priv = dev->priv;

	priv->dma_device = dma_device_reserve("PPE");

	if (!priv->dma_device){
		BUG();
		return -ENODEV;
	}

	priv->dma_device->buffer_alloc = &ifxmips_etop_dma_buffer_alloc;
	priv->dma_device->buffer_free = &ifxmips_etop_dma_buffer_free;
	priv->dma_device->intr_handler = &dma_intr_handler;
	priv->dma_device->max_rx_chan_num = 4;

	for (i = 0; i < priv->dma_device->max_rx_chan_num; i++)
	{
		priv->dma_device->rx_chan[i]->packet_size = ETHERNET_PACKET_DMA_BUFFER_SIZE;
		priv->dma_device->rx_chan[i]->control = IFXMIPS_DMA_CH_ON;
	}

	for (i = 0; i < priv->dma_device->max_tx_chan_num; i++)
	{
		if(i == 0)
			priv->dma_device->tx_chan[i]->control = IFXMIPS_DMA_CH_ON;
		else
			priv->dma_device->tx_chan[i]->control = IFXMIPS_DMA_CH_OFF;
	}

	dma_device_register(priv->dma_device);

	/*read the mac address from the mac table and put them into the mac table.*/
	for (i = 0; i < 6; i++)
	{
		retval += u_boot_ethaddr[i];
	}

	//TODO
	/* ethaddr not set in u-boot ? */
	if (retval == 0)
	{
		printk("use default MAC address\n");
		dev->dev_addr[0] = 0x00;
		dev->dev_addr[1] = 0x11;
		dev->dev_addr[2] = 0x22;
		dev->dev_addr[3] = 0x33;
		dev->dev_addr[4] = 0x44;
		dev->dev_addr[5] = 0x55;
	} else {
		for (i = 0; i < 6; i++)
			dev->dev_addr[i] = u_boot_ethaddr[i];
	}

	return 0;
}

static void
ifxmips_sw_chip_init (int mode)
{
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_DMA);
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_PPE);

	if(mode == REV_MII_MODE)
		writel((readl(IFXMIPS_PPE32_CFG) & PPE32_MII_MASK) | PPE32_MII_REVERSE, IFXMIPS_PPE32_CFG);
	else if(mode == MII_MODE)
		writel((readl(IFXMIPS_PPE32_CFG) & PPE32_MII_MASK) | PPE32_MII_NORMAL, IFXMIPS_PPE32_CFG);

	writel(PPE32_PLEN_UNDER | PPE32_PLEN_OVER, IFXMIPS_PPE32_IG_PLEN_CTRL);

	writel(PPE32_CGEN, IFXMIPS_PPE32_ENET_MAC_CFG);

	wmb();
}

int __init
switch_init_module(void)
{
	int result = 0;

	ifxmips_mii0_dev.init = switch_init;

	strcpy(ifxmips_mii0_dev.name, "eth%d");
	SET_MODULE_OWNER(dev);

	result = register_netdev(&ifxmips_mii0_dev);
	if (result)
	{
		printk("error %i registering device \"%s\"\n", result, ifxmips_mii0_dev.name);
		goto out;
	}

	/* ifxmips eval kit connects the phy/switch in REV mode */
	ifxmips_sw_chip_init(REV_MII_MODE);
	printk("ifxmips MAC driver loaded!\n");

out:
	return result;
}

static void __exit
switch_cleanup(void)
{
	struct switch_priv *priv = (struct switch_priv*)ifxmips_mii0_dev.priv;

	printk("ifxmips_mii0 cleanup\n");

	dma_device_unregister(priv->dma_device);
	dma_device_release(priv->dma_device);
	kfree(priv->dma_device);
	kfree(ifxmips_mii0_dev.priv);
	unregister_netdev(&ifxmips_mii0_dev);

	return;
}

module_init(switch_init_module);
module_exit(switch_cleanup);
