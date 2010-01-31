/*
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
 */
//-----------------------------------------------------------------------
/*
 * Description:
 *	Driver for Infineon Amazon 3 port switch
 */
//-----------------------------------------------------------------------
/* Author:	Wu Qi Ming[Qi-Ming.Wu@infineon.com]
 * Created:	7-April-2004
 */
//-----------------------------------------------------------------------
/* History
 * Changed on: Jun 28, 2004
 * Changed by: peng.liu@infineon.com
 * Reason:	add hardware flow control (HFC) (CONFIG_NET_HW_FLOWCONTROL)
 *
 * Changed on: Apr 6, 2005
 * Changed by: mars.lin@infineon.com
 * Reason    : supoort port identification
 */


// copyright 2004-2005 infineon.com

// copyright 2007 john crispin <blogic@openwrt.org>
// copyright 2007 felix fietkau <nbd@openwrt.org>
// copyright 2009 hauke mehrtens <hauke@hauke-m.de>


// TODO
// 		port vlan code from bcrm target... the tawainese code was scrapped due to crappyness
// 		check all the mmi reg settings and possibly document them better
// 		verify the ethtool code
//		remove the while(1) stuff
// 		further clean up and rework ... but it works for now
//		check the mode[]=bridge stuff
//		verify that the ethaddr can be set from u-boot


#ifndef __KERNEL__
#define __KERNEL__
#endif


#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#define MODVERSIONS
#endif

#if defined(MODVERSIONS) && !defined(__GENKSYMS__)
#include <linux/modversions.h>
#endif

#include <linux/module.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/mii.h>
#include <asm/uaccess.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/in6.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/ethtool.h>
#include <asm/checksum.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/amazon/amazon.h>
#include <asm/amazon/amazon_dma.h>
#include <asm/amazon/amazon_sw.h>

// how many mii ports are there ?
#define AMAZON_SW_INT_NO 2

#define ETHERNET_PACKET_DMA_BUFFER_SIZE 1536

/***************************************** Module Parameters *************************************/
char mode[] = "bridge";
module_param_array(mode, charp, NULL, 0);

static int timeout = 1 * HZ;
module_param(timeout, int, 0);

int switch_init(struct net_device *dev);
void switch_tx_timeout(struct net_device *dev);

static struct net_device *switch_devs[2];

int add_mac_table_entry(u64 entry_value)
{
	int i;
	u32 data1, data2;

	AMAZON_SW_REG32(AMAZON_SW_ARL_CTL) = ~7;

	for (i = 0; i < 32; i++) {
		AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) = 0x80000000 | 0x20 | i;
		while (AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) & (0x80000000)) {};
		data1 = AMAZON_SW_REG32(AMAZON_SW_DATA1);
		data2 = AMAZON_SW_REG32(AMAZON_SW_DATA2);
		if ((data1 & (0x00700000)) != 0x00700000)
			continue;
		AMAZON_SW_REG32(AMAZON_SW_DATA1) = (u32) (entry_value >> 32);
		AMAZON_SW_REG32(AMAZON_SW_DATA2) = (u32) entry_value & 0xffffffff;
		AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) = 0xc0000020 | i;
		while (AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) & (0x80000000)) {};
		break;
	}
	AMAZON_SW_REG32(AMAZON_SW_ARL_CTL) |= 7;
	if (i >= 32)
		return -1;
	return OK;
}

u64 read_mac_table_entry(int index)
{
	u32 data1, data2;
	u64 value;
	AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) = 0x80000000 | 0x20 | index;
	while (AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) & (0x80000000)) {};
	data1 = AMAZON_SW_REG32(AMAZON_SW_DATA1) & 0xffffff;
	data2 = AMAZON_SW_REG32(AMAZON_SW_DATA2);
	value = (u64) data1 << 32 | (u64) data2;
	return value;
}

int write_mac_table_entry(int index, u64 value)
{
	u32 data1, data2;
	data1 = (u32) (value >> 32);
	data2 = (u32) value & 0xffffffff;
	AMAZON_SW_REG32(AMAZON_SW_DATA1) = data1;
	AMAZON_SW_REG32(AMAZON_SW_DATA2) = data2;
	AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) = 0xc0000020 | index;
	while (AMAZON_SW_REG32(AMAZON_SW_CPU_ACTL) & (0x80000000)) {};
	return OK;
}

u32 get_mdio_reg(int phy_addr, int reg_num)
{
	u32 value;
	AMAZON_SW_REG32(AMAZON_SW_MDIO_ACC) = (3 << 30) | ((phy_addr & 0x1f) << 21) | ((reg_num & 0x1f) << 16);
	while (AMAZON_SW_REG32(AMAZON_SW_MDIO_ACC) & (1 << 31)) {};
	value = AMAZON_SW_REG32(AMAZON_SW_MDIO_ACC) & 0xffff;
	return value;
}

int set_mdio_reg(int phy_addr, int reg_num, u32 value)
{
	AMAZON_SW_REG32(AMAZON_SW_MDIO_ACC) = (2 << 30) | ((phy_addr & 0x1f) << 21) | ((reg_num & 0x1f) << 16) | (value & 0xffff);
	while (AMAZON_SW_REG32(AMAZON_SW_MDIO_ACC) & (1 << 31)) {};
	return OK;
}

int auto_negotiate(int phy_addr)
{
	u32 value = 0;
	value = get_mdio_reg(phy_addr, MDIO_BASE_CONTROL_REG);
	set_mdio_reg(phy_addr, MDIO_BASE_CONTROL_REG, (value | RESTART_AUTO_NEGOTIATION | AUTO_NEGOTIATION_ENABLE | PHY_RESET));
	return OK;
}

/*
     In this version of switch driver, we split the dma channels for the switch.
     2 for port0 and 2 for port1. So that we can do internal bridging if necessary.
     In switch mode, packets coming in from port0 or port1 is able to do Destination 
     address lookup. Packets coming from port0 with destination address of port1 should 
     not go to pmac again. The switch hardware should be able to do the switch in the hard 
     ware level. Packets coming from the pmac should not do the DA look up in that the
     desination is already known for the kernel. It only needs to go to the correct NIC to 
     find its way out.
  */
int amazon_sw_chip_init(void)
{
	u32 tmp1;
	int i = 0;

	/* Aging tick select: 5mins */
	tmp1 = 0xa0;
	if (strcmp(mode, "bridge") == 0) {
		// bridge mode, set militarised mode to 1, no learning!
		tmp1 |= 0xC00;
	} else {
		// enable learning for P0 and P1,
		tmp1 |= 3;
	}

	/* unknown broadcast/multicast/unicast to all ports */
	AMAZON_SW_REG32(AMAZON_SW_UN_DEST) = 0x1ff;

	AMAZON_SW_REG32(AMAZON_SW_ARL_CTL) = tmp1;

	/* OCS:1 set OCS bit, split the two NIC in rx direction EDL:1 (enable DA lookup) */
#if defined(CONFIG_IFX_NFEXT_AMAZON_SWITCH_PHYPORT) || defined(CONFIG_IFX_NFEXT_AMAZON_SWITCH_PHYPORT_MODULE)
	AMAZON_SW_REG32(AMAZON_SW_P2_PCTL) = 0x700;
#else
	AMAZON_SW_REG32(AMAZON_SW_P2_PCTL) = 0x401;
#endif

	/* EPC: 1 split the two NIC in tx direction CRC is generated */
	AMAZON_SW_REG32(AMAZON_SW_P2_CTL) = 0x6;

	// for bi-directional 
	AMAZON_SW_REG32(AMAZON_SW_P0_WM) = 0x14141412;
	AMAZON_SW_REG32(AMAZON_SW_P1_WM) = 0x14141412;
	AMAZON_SW_REG32(AMAZON_SW_P2_WM) = 0x28282826;
	AMAZON_SW_REG32(AMAZON_SW_GBL_WM) = 0x0;

	AMAZON_SW_REG32(AMAZON_CGU_PLL0SR) = (AMAZON_SW_REG32(AMAZON_CGU_PLL0SR)) | 0x58000000;
	// clock for PHY
	AMAZON_SW_REG32(AMAZON_CGU_IFCCR) =	(AMAZON_SW_REG32(AMAZON_CGU_IFCCR)) | 0x80000004;
	// enable power for PHY
	AMAZON_SW_REG32(AMAZON_PMU_PWDCR) = (AMAZON_SW_REG32(AMAZON_PMU_PWDCR)) | AMAZON_PMU_PWDCR_EPHY;
	// set reverse MII, enable MDIO statemachine
	AMAZON_SW_REG32(AMAZON_SW_MDIO_CFG) = 0x800027bf;
	while (1)
		if (((AMAZON_SW_REG32(AMAZON_SW_MDIO_CFG)) & 0x80000000) == 0)
			break;
	AMAZON_SW_REG32(AMAZON_SW_EPHY) = 0xff;

	// auto negotiation
	AMAZON_SW_REG32(AMAZON_SW_MDIO_ACC) = 0x83e08000;
	auto_negotiate(0x1f);

	/* enable all ports */
	AMAZON_SW_REG32(AMAZON_SW_PS_CTL) = 0x7;
	for (i = 0; i < 32; i++)
		write_mac_table_entry(i, 1 << 50);
	return 0;
}

static unsigned char my_ethaddr[MAX_ADDR_LEN];
/* need to get the ether addr from u-boot */
static int __init ethaddr_setup(char *line)
{
	char *ep;
	int i;

	memset(my_ethaddr, 0, MAX_ADDR_LEN);
	for (i = 0; i < 6; i++) {
		my_ethaddr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
		if (line)
			line = (*ep) ? ep + 1 : ep;
	}
	printk(KERN_INFO "amazon_mii0: mac address %2x-%2x-%2x-%2x-%2x-%2x \n", my_ethaddr[0], my_ethaddr[1], my_ethaddr[2], my_ethaddr[3], my_ethaddr[4], my_ethaddr[5]);
	return 0;
}

__setup("ethaddr=", ethaddr_setup);

static void open_rx_dma(struct net_device *dev)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->num_rx_chan; i++)
		dma_dev->rx_chan[i].control = 1;
	dma_device_update_rx(dma_dev);
}

#ifdef CONFIG_NET_HW_FLOWCONTROL
static void close_rx_dma(struct net_device *dev)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;
	int i;

	for (i = 0; i < dma_dev->num_rx_chan; i++)
		dma_dev->rx_chan[i].control = 0;
	dma_device_update_rx(dma_dev);
}

void amazon_xon(struct net_device *dev)
{
	unsigned long flag;
	local_irq_save(flag);
	open_rx_dma(dev);
	local_irq_restore(flag);
}
#endif

int switch_open(struct net_device *dev)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	if (!strcmp(dev->name, "eth1")) {
		priv->mdio_phy_addr = PHY0_ADDR;
	}
	open_rx_dma(dev);			

#ifdef CONFIG_NET_HW_FLOWCONTROL
	if ((priv->fc_bit = netdev_register_fc(dev, amazon_xon)) == 0) {
		printk(KERN_WARNING "amazon_mii0: Hardware Flow Control register fails\n");
	}
#endif

	netif_start_queue(dev);
	return OK;
}

int switch_release(struct net_device *dev)
{
	int i;
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;

	for (i = 0; i < dma_dev->num_tx_chan; i++)
		dma_dev->tx_chan[i].control = 0;
	for (i = 0; i < dma_dev->num_rx_chan; i++)
		dma_dev->rx_chan[i].control = 0;

	dma_device_update(dma_dev);

#ifdef CONFIG_NET_HW_FLOWCONTROL
	if (priv->fc_bit) {
		netdev_unregister_fc(priv->fc_bit);
	}
#endif
	netif_stop_queue(dev);

	return OK;
}


void switch_rx(struct net_device *dev, int len, struct sk_buff *skb)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
#ifdef CONFIG_NET_HW_FLOWCONTROL
	int mit_sel = 0;
#endif
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);

#ifdef CONFIG_NET_HW_FLOWCONTROL
	mit_sel = netif_rx(skb);
	switch (mit_sel) {
	case NET_RX_SUCCESS:
	case NET_RX_CN_LOW:
	case NET_RX_CN_MOD:
		break;
	case NET_RX_CN_HIGH:
		break;
	case NET_RX_DROP:
		if ((priv->fc_bit)
			&& (!test_and_set_bit(priv->fc_bit, &netdev_fc_xoff))) {
			close_rx_dma(dev);
		}
		break;
	}
#else
	netif_rx(skb);
#endif
	priv->stats.rx_packets++;
	priv->stats.rx_bytes += len;
	return;
}

int asmlinkage switch_hw_tx(char *buf, int len, struct net_device *dev)
{
	struct switch_priv *priv = netdev_priv(dev);
	struct dma_device_info *dma_dev = priv->dma_device;

	dma_dev->current_tx_chan = 0;
	return dma_device_write(dma_dev, buf, len, priv->skb);
}

int asmlinkage switch_tx(struct sk_buff *skb, struct net_device *dev)
{
	int len;
	char *data;
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);

	len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;
	data = skb->data;
	priv->skb = skb;
	dev->trans_start = jiffies;

	if (switch_hw_tx(data, len, dev) != len) {
		dev_kfree_skb_any(skb);
		return OK;
	}

	priv->stats.tx_packets++;
	priv->stats.tx_bytes += len;
	return OK;
}

void switch_tx_timeout(struct net_device *dev)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	priv->stats.tx_errors++;
	netif_wake_queue(dev);
	return;
}

void negotiate(struct net_device *dev)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	unsigned short data = get_mdio_reg(priv->mdio_phy_addr, MDIO_ADVERTISMENT_REG);

	data &= ~(MDIO_ADVERT_100_HD | MDIO_ADVERT_100_FD | MDIO_ADVERT_10_FD | MDIO_ADVERT_10_HD);

	switch (priv->current_speed_selection) {
	case 10:
		if (priv->current_duplex == full)
			data |= MDIO_ADVERT_10_FD;
		else if (priv->current_duplex == half)
			data |= MDIO_ADVERT_10_HD;
		else
			data |= MDIO_ADVERT_10_HD | MDIO_ADVERT_10_FD;
		break;

	case 100:
		if (priv->current_duplex == full)
			data |= MDIO_ADVERT_100_FD;
		else if (priv->current_duplex == half)
			data |= MDIO_ADVERT_100_HD;
		else
			data |= MDIO_ADVERT_100_HD | MDIO_ADVERT_100_FD;
		break;

	case 0:					/* Auto */
		if (priv->current_duplex == full)
			data |= MDIO_ADVERT_100_FD | MDIO_ADVERT_10_FD;
		else if (priv->current_duplex == half)
			data |= MDIO_ADVERT_100_HD | MDIO_ADVERT_10_HD;
		else
			data |=	MDIO_ADVERT_100_HD | MDIO_ADVERT_100_FD | MDIO_ADVERT_10_FD	| MDIO_ADVERT_10_HD;
		break;

	default:					/* assume autoneg speed and duplex */
		data |= MDIO_ADVERT_100_HD | MDIO_ADVERT_100_FD | MDIO_ADVERT_10_FD | MDIO_ADVERT_10_HD;
	}

	set_mdio_reg(priv->mdio_phy_addr, MDIO_ADVERTISMENT_REG, data);

	/* Renegotiate with link partner */

	data = get_mdio_reg(priv->mdio_phy_addr, MDIO_BASE_CONTROL_REG);
	data |= MDIO_BC_NEGOTIATE;

	set_mdio_reg(priv->mdio_phy_addr, MDIO_BASE_CONTROL_REG, data);

}


void set_duplex(struct net_device *dev, enum duplex new_duplex)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	if (new_duplex != priv->current_duplex) {
		priv->current_duplex = new_duplex;
		negotiate(dev);
	}
}

void set_speed(struct net_device *dev, unsigned long speed)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	priv->current_speed_selection = speed;
	negotiate(dev);
}

static int switch_ethtool_ioctl(struct net_device *dev, struct ifreq *ifr)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	struct ethtool_cmd ecmd;

	if (copy_from_user(&ecmd, ifr->ifr_data, sizeof(ecmd)))
		return -EFAULT;

	switch (ecmd.cmd) {
	case ETHTOOL_GSET:
		memset((void *) &ecmd, 0, sizeof(ecmd));
		ecmd.supported = SUPPORTED_Autoneg | SUPPORTED_TP | SUPPORTED_MII |	SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full |
						SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full;
		ecmd.port = PORT_TP;
		ecmd.transceiver = XCVR_EXTERNAL;
		ecmd.phy_address = priv->mdio_phy_addr;

		ecmd.speed = priv->current_speed;

		ecmd.duplex = priv->full_duplex ? DUPLEX_FULL : DUPLEX_HALF;

		ecmd.advertising = ADVERTISED_TP;
		if (priv->current_duplex == autoneg && priv->current_speed_selection == 0)
			ecmd.advertising |= ADVERTISED_Autoneg;
		else {
			ecmd.advertising |=	ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |
				ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full;
			if (priv->current_speed_selection == 10)
				ecmd.advertising &=	~(ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full);
			else if (priv->current_speed_selection == 100)
				ecmd.advertising &=	~(ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full);
			if (priv->current_duplex == half)
				ecmd.advertising &=	~(ADVERTISED_10baseT_Full | ADVERTISED_100baseT_Full);
			else if (priv->current_duplex == full)
				ecmd.advertising &=	~(ADVERTISED_10baseT_Half | ADVERTISED_100baseT_Half);
		}
		ecmd.autoneg = AUTONEG_ENABLE;
		if (copy_to_user(ifr->ifr_data, &ecmd, sizeof(ecmd)))
			return -EFAULT;
		break;

	case ETHTOOL_SSET:
		if (!capable(CAP_NET_ADMIN)) {
			return -EPERM;
		}
		if (ecmd.autoneg == AUTONEG_ENABLE) {
			set_duplex(dev, autoneg);
			set_speed(dev, 0);
		} else {
			set_duplex(dev, ecmd.duplex == DUPLEX_HALF ? half : full);
			set_speed(dev, ecmd.speed == SPEED_10 ? 10 : 100);
		}
		break;

	case ETHTOOL_GDRVINFO:
		{
			struct ethtool_drvinfo info;
			memset((void *) &info, 0, sizeof(info));
			strncpy(info.driver, "AMAZONE", sizeof(info.driver) - 1);
			strncpy(info.fw_version, "N/A", sizeof(info.fw_version) - 1);
			strncpy(info.bus_info, "N/A", sizeof(info.bus_info) - 1);
			info.regdump_len = 0;
			info.eedump_len = 0;
			info.testinfo_len = 0;
			if (copy_to_user(ifr->ifr_data, &info, sizeof(info)))
				return -EFAULT;
		}
		break;
	case ETHTOOL_NWAY_RST:
		if (priv->current_duplex == autoneg	&& priv->current_speed_selection == 0)
			negotiate(dev);
		break;
	default:
		return -EOPNOTSUPP;
		break;
	}
	return 0;
}



int mac_table_tools_ioctl(struct net_device *dev, struct mac_table_req *req)
{
	int cmd;
	int i;
	cmd = req->cmd;
	switch (cmd) {
	case RESET_MAC_TABLE:
		for (i = 0; i < 32; i++) {
			write_mac_table_entry(i, 0);
		}
		break;
	case READ_MAC_ENTRY:
		req->entry_value = read_mac_table_entry(req->index);
		break;
	case WRITE_MAC_ENTRY:
		write_mac_table_entry(req->index, req->entry_value);
		break;
	case ADD_MAC_ENTRY:
		add_mac_table_entry(req->entry_value);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}


/*
    the ioctl for the switch driver is developed in the conventional way
    the control type falls into some basic categories, among them, the 
    SIOCETHTOOL is the traditional eth interface. VLAN_TOOLS and  
    MAC_TABLE_TOOLS are designed specifically for amazon chip. User 
    should be aware of the data structures used in these interfaces. 
*/
int switch_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct data_req *switch_data_req = (struct data_req *) ifr->ifr_data;
	struct mac_table_req *switch_mac_table_req;
	switch (cmd) {
	case SIOCETHTOOL:
		switch_ethtool_ioctl(dev, ifr);
		break;
	case SIOCGMIIPHY:			/* Get PHY address */
		break;
	case SIOCGMIIREG:			/* Read MII register */
		break;
	case SIOCSMIIREG:			/* Write MII register */
		break;
	case SET_ETH_SPEED_10:		/* 10 Mbps */
		break;
	case SET_ETH_SPEED_100:	/* 100 Mbps */
		break;
	case SET_ETH_SPEED_AUTO:	/* Auto negotiate speed */
		break;
	case SET_ETH_DUPLEX_HALF:	/* Half duplex. */
		break;
	case SET_ETH_DUPLEX_FULL:	/* Full duplex. */
		break;
	case SET_ETH_DUPLEX_AUTO:	/* Autonegotiate duplex */
		break;
	case SET_ETH_REG:
		AMAZON_SW_REG32(switch_data_req->index) = switch_data_req->value;
		break;
	case MAC_TABLE_TOOLS:
		switch_mac_table_req = (struct mac_table_req *) ifr->ifr_data;
		mac_table_tools_ioctl(dev, switch_mac_table_req);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

struct net_device_stats *switch_stats(struct net_device *dev)
{
	struct switch_priv *priv = (struct switch_priv *) netdev_priv(dev);
	return &priv->stats;
}

int switch_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu >= 1516)
		new_mtu = 1516;
	dev->mtu = new_mtu;
	return 0;
}

int switch_hw_receive(struct net_device *dev, struct dma_device_info *dma_dev)
{
	u8 *buf = NULL;
	int len = 0;
	struct sk_buff *skb = NULL;

	len = dma_device_read(dma_dev, &buf, (void **) &skb);

	if (len >= 0x600) {
		printk(KERN_WARNING "amazon_mii0: packet too large %d\n", len);
		goto switch_hw_receive_err_exit;
	}

	/* remove CRC */
	len -= 4;
	if (skb == NULL) {
		printk(KERN_WARNING "amazon_mii0: cannot restore pointer\n");
		goto switch_hw_receive_err_exit;
	}
	if (len > (skb->end - skb->tail)) {
		printk(KERN_WARNING "amazon_mii0: BUG, len:%d end:%p tail:%p\n", (len + 4), skb->end, skb->tail);
		goto switch_hw_receive_err_exit;
	}
	skb_put(skb, len);
	skb->dev = dev;
	switch_rx(dev, len, skb);
	return OK;
  
  switch_hw_receive_err_exit:
	if (skb)
		dev_kfree_skb_any(skb);
	return -EIO;
}

int dma_intr_handler(struct dma_device_info *dma_dev, int status)
{
	struct net_device *dev;

	dev = dma_dev->priv;
	switch (status) {
	case RCV_INT:
		switch_hw_receive(dev, dma_dev);
		break;
	case TX_BUF_FULL_INT:
		netif_stop_queue(dev);
		break;
	case TRANSMIT_CPT_INT:
		netif_wake_queue(dev);
		break;
	}
	return OK;
}

/* reserve 2 bytes in front of data pointer*/
u8 *dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
	u8 *buffer = NULL;
	struct sk_buff *skb = NULL;
	skb = dev_alloc_skb(ETHERNET_PACKET_DMA_BUFFER_SIZE);
	if (skb == NULL) {
		return NULL;
	}
	buffer = (u8 *) (skb->data);
	skb_reserve(skb, 2);
	*(int *) opt = (int) skb;
	*byte_offset = 2;
	return buffer;
}

int dma_buffer_free(u8 * dataptr, void *opt)
{
	struct sk_buff *skb = NULL;
	if (opt == NULL) {
		kfree(dataptr);
	} else {
		skb = (struct sk_buff *) opt;
		dev_kfree_skb_any(skb);
	}
	return OK;
}

int init_dma_device(_dma_device_info * dma_dev, struct net_device *dev)
{
	int i;
	int num_tx_chan, num_rx_chan;
	if (strcmp(dma_dev->device_name, "switch1") == 0) {
		num_tx_chan = 1;
		num_rx_chan = 2;
	} else {
		num_tx_chan = 1;
		num_rx_chan = 2;
	}
	dma_dev->priv = dev;

	dma_dev->weight = 1;
	dma_dev->num_tx_chan = num_tx_chan;
	dma_dev->num_rx_chan = num_rx_chan;
	dma_dev->ack = 1;
	dma_dev->tx_burst_len = 4;
	dma_dev->rx_burst_len = 4;
	for (i = 0; i < dma_dev->num_tx_chan; i++) {
		dma_dev->tx_chan[i].weight = QOS_DEFAULT_WGT;
		dma_dev->tx_chan[i].desc_num = 10;
		dma_dev->tx_chan[i].packet_size = 0;
		dma_dev->tx_chan[i].control = 0;
	}
	for (i = 0; i < num_rx_chan; i++) {
		dma_dev->rx_chan[i].weight = QOS_DEFAULT_WGT;
		dma_dev->rx_chan[i].desc_num = 10;
		dma_dev->rx_chan[i].packet_size = ETHERNET_PACKET_DMA_BUFFER_SIZE;
		dma_dev->rx_chan[i].control = 0;
	}
	dma_dev->intr_handler = dma_intr_handler;
	dma_dev->buffer_alloc = dma_buffer_alloc;
	dma_dev->buffer_free = dma_buffer_free;
	return 0;
}

int switch_set_mac_address(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;
	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
	return OK;
}

static const struct net_device_ops amazon_mii_ops = {
	.ndo_init		= switch_init,
	.ndo_open		= switch_open,
	.ndo_stop		= switch_release,
	.ndo_start_xmit		= switch_tx,
	.ndo_do_ioctl		= switch_ioctl,
	.ndo_get_stats		= switch_stats,
	.ndo_change_mtu		= switch_change_mtu,
	.ndo_set_mac_address		= switch_set_mac_address,
	.ndo_tx_timeout		= switch_tx_timeout,
};

int switch_init(struct net_device *dev)
{
	u64 retval = 0;
	int i;
	int result;
	struct switch_priv *priv;
	ether_setup(dev);			/* assign some of the fields */
	printk(KERN_INFO "amazon_mii0: %s up using ", dev->name);
	dev->watchdog_timeo = timeout;

	priv = netdev_priv(dev);
	priv->dma_device = (struct dma_device_info *) kmalloc(sizeof(struct dma_device_info), GFP_KERNEL);
	if (priv->num == 0) {
		sprintf(priv->dma_device->device_name, "switch1");
	} else if (priv->num == 1) {
		sprintf(priv->dma_device->device_name, "switch2");
	}
	printk("\"%s\"\n", priv->dma_device->device_name);
	init_dma_device(priv->dma_device, dev);
	result = dma_device_register(priv->dma_device);

	/* read the mac address from the mac table and put them into the mac table. */
	for (i = 0; i < 6; i++) {
		retval += my_ethaddr[i];
	}
	/* ethaddr not set in u-boot ? */
	if (retval == 0) {
		dev->dev_addr[0] = 0x00;
		dev->dev_addr[1] = 0x20;
		dev->dev_addr[2] = 0xda;
		dev->dev_addr[3] = 0x86;
		dev->dev_addr[4] = 0x23;
		dev->dev_addr[5] = 0x74 + (unsigned char) priv->num;
	} else {
		for (i = 0; i < 6; i++) {
			dev->dev_addr[i] = my_ethaddr[i];
		}
		dev->dev_addr[5] += +(unsigned char) priv->num;
	}
	return OK;
}

static int amazon_mii_probe(struct platform_device *dev)
{
	int i = 0, result, device_present = 0;
	struct switch_priv *priv;

	for (i = 0; i < AMAZON_SW_INT_NO; i++) {
		switch_devs[i] = alloc_etherdev(sizeof(struct switch_priv));
		switch_devs[i]->netdev_ops = &amazon_mii_ops;
		strcpy(switch_devs[i]->name, "eth%d");
		priv = (struct switch_priv *) netdev_priv(switch_devs[i]);
		priv->num = i;
		if ((result = register_netdev(switch_devs[i])))
			printk(KERN_WARNING "amazon_mii0: error %i registering device \"%s\"\n", result, switch_devs[i]->name);
		else
			device_present++;
	}
	amazon_sw_chip_init();
	return device_present ? 0 : -ENODEV;
}

static int amazon_mii_remove(struct platform_device *dev)
{
	int i;
	struct switch_priv *priv;
	for (i = 0; i < AMAZON_SW_INT_NO; i++) {
		priv = netdev_priv(switch_devs[i]);
		if (priv->dma_device) {
			dma_device_unregister(priv->dma_device);
			kfree(priv->dma_device);
		}
		kfree(netdev_priv(switch_devs[i]));
		unregister_netdev(switch_devs[i]);
	}
	return 0;
}

static struct platform_driver amazon_mii_driver = {
	.probe = amazon_mii_probe,
	.remove = amazon_mii_remove,
	.driver = {
		.name = "amazon_mii0",
		.owner = THIS_MODULE,
	},
};

static int __init amazon_mii_init(void)
{
	int ret = platform_driver_register(&amazon_mii_driver);
	if (ret)
		printk(KERN_WARNING "amazon_mii0: Error registering platfom driver!\n");
	return ret;
}

static void __exit amazon_mii_cleanup(void)
{
	platform_driver_unregister(&amazon_mii_driver);
}

module_init(amazon_mii_init);
module_exit(amazon_mii_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wu Qi Ming");
MODULE_DESCRIPTION("ethernet driver for AMAZON boards");

