/******************************************************************************

                               Copyright (c) 2007
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

 ****************************************************************************
Module      : svip_virtual_eth.c

Description : This file contains network driver implementation for a
Virtual Ethernet interface. The Virtual Ethernet interface
is part of Infineon's VINETIC-SVIP Linux BSP.
 *******************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/platform_device.h>
#include <linux/etherdevice.h>
#include <linux/init.h>

#define SVIP_VETH_VER_STR      "3.0"
#define SVIP_VETH_INFO_STR \
	"@(#)SVIP virtual ethernet interface, version " SVIP_VETH_VER_STR

/******************************************************************************
 * Local define/macro definitions
 ******************************************************************************/
struct svip_ve_priv
{
	struct net_device_stats stats;
};

/******************************************************************************
 * Global function declarations
 ******************************************************************************/
int svip_ve_rx(struct sk_buff *skb);

/******************************************************************************
 * Local variable declarations
 ******************************************************************************/
static struct net_device *svip_ve_dev;
static int watchdog_timeout = 10*HZ;
static int (*svip_ve_mps_xmit)(struct sk_buff *skb) = NULL;


/******************************************************************************
 * Global function declarations
 ******************************************************************************/

/**
 * Called by MPS driver to register a transmit routine called for each outgoing
 * VoFW0 message.
 *
 * \param   mps_xmit    pointer to transmit routine
 *
 * \return  none
 *
 * \ingroup Internal
 */
void register_mps_xmit_routine(int (*mps_xmit)(struct sk_buff *skb))
{
	svip_ve_mps_xmit = mps_xmit;
}
EXPORT_SYMBOL(register_mps_xmit_routine);

/**
 * Returns a pointer to the routine used to deliver an incoming packet/message
 * from the MPS mailbox to the networking layer. This routine is called by MPS
 * driver during initialisation time.
 *
 * \param   skb         pointer to incoming socket buffer
 *
 * \return  svip_ve_rx  pointer to incoming messages delivering routine
 *
 * \ingroup Internal
 */
int (*register_mps_recv_routine(void)) (struct sk_buff *skb)
{
	return svip_ve_rx;
}

/**
 * Used to deliver outgoing packets to VoFW0 module through the MPS driver.
 * Upon loading/initialisation the MPS driver is registering a transmitting
 * routine, which is called here to deliver the packet to the VoFW0 module.
 *
 * \param   skb            pointer to skb containing outgoing data
 * \param   dev            pointer to this networking device's data
 *
 * \return  0 on success
 * \return  non-zero on error
 *
 * \ingroup Internal
 */
static int svip_ve_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int err;
	struct svip_ve_priv *priv = netdev_priv(dev);
	struct net_device_stats *stats = &priv->stats;

	stats->tx_packets++;
	stats->tx_bytes += skb->len;

	if (svip_ve_mps_xmit)
	{
		err = svip_ve_mps_xmit(skb);
		if (err)
			stats->tx_errors++;
		dev->trans_start = jiffies;
		return err;
	}
	else
		printk(KERN_ERR "%s: MPS driver not registered, outgoing packet not delivered\n", dev->name);

	dev_kfree_skb(skb);

	return -1;
}

/**
 * Called by MPS driver upon receipt of a new message from VoFW0 module in
 * the data inbox. The packet is pushed up the IP module for further processing.
 *
 * \param   skb            pointer to skb containing the incoming message
 *
 * \return  0 on success
 * \return  non-zero on error
 *
 * \ingroup Internal
 */
int svip_ve_rx(struct sk_buff *skb)
{
	int err;
	struct svip_ve_priv *priv = netdev_priv(svip_ve_dev);
	struct net_device_stats *stats = &priv->stats;

	skb->dev = svip_ve_dev;
	skb->protocol = eth_type_trans(skb, svip_ve_dev);

	stats->rx_packets++;
	stats->rx_bytes += skb->len;

	err = netif_rx(skb);
	switch (err)
	{
	case NET_RX_SUCCESS:
		return 0;
		break;
	case NET_RX_DROP:
	default:
		stats->rx_dropped++;
		break;
	}

	return 1;
}
EXPORT_SYMBOL(svip_ve_rx);

/**
 * Returns a pointer to the device's networking statistics data
 *
 * \param   dev            pointer to this networking device's data
 *
 * \return  stats          pointer to this network device's statistics data
 *
 * \ingroup Internal
 */
static struct net_device_stats *svip_ve_get_stats(struct net_device *dev)
{
	struct svip_ve_priv *priv = netdev_priv(dev);

	return &priv->stats;
}

static void svip_ve_tx_timeout(struct net_device *dev)
{
	struct svip_ve_priv *priv = netdev_priv(dev);

	priv->stats.tx_errors++;
	netif_wake_queue(dev);
}

/**
 * Device open routine. Called e.g. upon setting of an IP address using,
 * 'ifconfig veth0 YYY.YYY.YYY.YYY netmask ZZZ.ZZZ.ZZZ.ZZZ' or
 * 'ifconfig veth0 up'
 *
 * \param   dev            pointer to this network device's data
 *
 * \return  0 on success
 * \return  non-zero on error
 *
 * \ingroup Internal
 */
int svip_ve_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

/**
 * Device close routine. Called e.g. upon calling
 * 'ifconfig veth0 down'
 *
 * \param   dev            pointer to this network device's data
 *
 * \return  0 on success
 * \return  non-zero on error
 *
 * \ingroup Internal
 */

int svip_ve_release(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

static int svip_ve_dev_init(struct net_device *dev);

static const struct net_device_ops svip_virtual_eth_netdev_ops = {
	.ndo_init = svip_ve_dev_init,
	.ndo_open = svip_ve_open,
	.ndo_stop = svip_ve_release,
	.ndo_start_xmit = svip_ve_xmit,
	.ndo_get_stats = svip_ve_get_stats,
	.ndo_tx_timeout = svip_ve_tx_timeout,
};


/**
 * Device initialisation routine which registers device interface routines.
 * It is called upon execution of 'register_netdev' routine.
 *
 * \param   dev            pointer to this network device's data
 *
 * \return  0 on success
 * \return  non-zero on error
 *
 * \ingroup Internal
 */
static int svip_ve_dev_init(struct net_device *dev)
{
	ether_setup(dev); /* assign some of the fields */

	dev->watchdog_timeo  = watchdog_timeout;
	memset(netdev_priv(dev), 0, sizeof(struct svip_ve_priv));
	dev->flags |= IFF_NOARP|IFF_PROMISC;
	dev->flags &= ~IFF_MULTICAST;

	/* dedicated MAC address to veth0, 00:03:19:00:15:80 */
	dev->dev_addr[0] = 0x00;
	dev->dev_addr[1] = 0x03;
	dev->dev_addr[2] = 0x19;
	dev->dev_addr[3] = 0x00;
	dev->dev_addr[4] = 0x15;
	dev->dev_addr[5] = 0x80;

	return 0;
}

static int svip_ve_probe(struct platform_device *dev)
{
	int result = 0;

	svip_ve_dev = alloc_etherdev(sizeof(struct svip_ve_priv));
	svip_ve_dev->netdev_ops = &svip_virtual_eth_netdev_ops;

	strcpy(svip_ve_dev->name, "veth%d");

	result = register_netdev(svip_ve_dev);
	if (result)
	{
		printk(KERN_INFO "error %i registering device \"%s\"\n", result, svip_ve_dev->name);
		goto out;
	}

	printk (KERN_INFO "%s, (c) 2009, Lantiq Deutschland GmbH\n", &SVIP_VETH_INFO_STR[4]);

out:
	return result;
}

static int svip_ve_remove(struct platform_device *dev)
{
	unregister_netdev(svip_ve_dev);
	free_netdev(svip_ve_dev);

	printk(KERN_INFO "%s removed\n", svip_ve_dev->name);
	return 0;
}

static struct platform_driver svip_ve_driver = {
	.probe = svip_ve_probe,
	.remove = svip_ve_remove,
	.driver = {
		.name = "ifxmips_svip_ve",
		.owner = THIS_MODULE,
	},
};

/**
 * Module/driver entry routine
 */
static int __init svip_ve_init_module(void)
{
	int ret;

	ret = platform_driver_register(&svip_ve_driver);
	if (ret)
		printk(KERN_INFO "SVIP: error(%d) registering virtual Ethernet driver!\n", ret);
	return ret;
}

/**
 * Module exit routine (never called for statically linked driver)
 */
static void __exit svip_ve_cleanup_module(void)
{
	platform_driver_unregister(&svip_ve_driver);
}

module_init(svip_ve_init_module);
module_exit(svip_ve_cleanup_module);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("virtual ethernet driver for LANTIQ SVIP system");

EXPORT_SYMBOL(register_mps_recv_routine);
