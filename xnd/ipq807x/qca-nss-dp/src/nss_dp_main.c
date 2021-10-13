/*
 **************************************************************************
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#if defined(NSS_DP_PPE_SUPPORT)
#include <ref/ref_vsi.h>
#endif
#include <net/switchdev.h>

#include "nss_dp_hal.h"

/*
 * Number of TX/RX queue supported is based on the number of host CPU
 */
#define NSS_DP_NETDEV_TX_QUEUE_NUM NSS_DP_HAL_CPU_NUM
#define NSS_DP_NETDEV_RX_QUEUE_NUM NSS_DP_HAL_CPU_NUM

/* ipq40xx_mdio_data */
struct ipq40xx_mdio_data {
	struct mii_bus *mii_bus;
	void __iomem *membase;
	int phy_irq[PHY_MAX_ADDR];
};

/* Global data */
struct nss_dp_global_ctx dp_global_ctx;
struct nss_dp_data_plane_ctx dp_global_data_plane_ctx[NSS_DP_HAL_MAX_PORTS];

/*
 * nss_dp_do_ioctl()
 */
static int32_t nss_dp_do_ioctl(struct net_device *netdev, struct ifreq *ifr,
						   int32_t cmd)
{
	int ret = -EINVAL;
	struct nss_dp_dev *dp_priv;

	if (!netdev || !ifr)
		return ret;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	if (dp_priv->phydev)
		return phy_mii_ioctl(dp_priv->phydev, ifr, cmd);

	return ret;
}

/*
 * nss_dp_change_mtu()
 */
static int32_t nss_dp_change_mtu(struct net_device *netdev, int32_t newmtu)
{
	int ret = -EINVAL;
	struct nss_dp_dev *dp_priv;

	if (!netdev)
		return ret;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	/* Let the underlying data plane decide if the newmtu is applicable */
	if (dp_priv->data_plane_ops->change_mtu(dp_priv->dpc, newmtu)) {
		netdev_dbg(netdev, "Data plane change mtu failed\n");
		return ret;
	}

	netdev->mtu = newmtu;

	return 0;
}

/*
 * nss_dp_set_mac_address()
 */
static int32_t nss_dp_set_mac_address(struct net_device *netdev, void *macaddr)
{
	struct nss_dp_dev *dp_priv;
	struct sockaddr *addr = (struct sockaddr *)macaddr;
	int ret = 0;

	if (!netdev)
		return -EINVAL;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	netdev_dbg(netdev, "AddrFamily: %d, %0x:%0x:%0x:%0x:%0x:%0x\n",
			addr->sa_family, addr->sa_data[0], addr->sa_data[1],
			addr->sa_data[2], addr->sa_data[3], addr->sa_data[4],
			addr->sa_data[5]);

	ret = eth_prepare_mac_addr_change(netdev, macaddr);
	if (ret)
		return ret;

	if (dp_priv->data_plane_ops->mac_addr(dp_priv->dpc, macaddr)) {
		netdev_dbg(netdev, "Data plane set MAC address failed\n");
		return -EAGAIN;
	}

	eth_commit_mac_addr_change(netdev, macaddr);

	dp_priv->gmac_hal_ops->setmacaddr(dp_priv->gmac_hal_ctx,
			(uint8_t *)addr->sa_data);

	return 0;
}

/*
 * nss_dp_get_stats64()
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
static struct rtnl_link_stats64 *nss_dp_get_stats64(struct net_device *netdev,
					     struct rtnl_link_stats64 *stats)
{
	struct nss_dp_dev *dp_priv;

	if (!netdev)
		return stats;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	dp_priv->gmac_hal_ops->getndostats(dp_priv->gmac_hal_ctx, stats);

	return stats;
}
#else
static void nss_dp_get_stats64(struct net_device *netdev,
					     struct rtnl_link_stats64 *stats)
{
	struct nss_dp_dev *dp_priv;

	if (!netdev)
		return;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	dp_priv->gmac_hal_ops->getndostats(dp_priv->gmac_hal_ctx, stats);
}
#endif

/*
 * nss_dp_xmit()
 */
static netdev_tx_t nss_dp_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct nss_dp_dev *dp_priv;

	if (!skb || !netdev)
		return NETDEV_TX_OK;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);
	netdev_dbg(netdev, "Tx packet, len %d\n", skb->len);

	return dp_priv->data_plane_ops->xmit(dp_priv->dpc, skb);
}

/*
 * nss_dp_close()
 */
static int nss_dp_close(struct net_device *netdev)
{
	struct nss_dp_dev *dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	if (!dp_priv)
		return -EINVAL;

	netif_stop_queue(netdev);
	netif_carrier_off(netdev);

	/* Notify data plane link is going down */
	if (dp_priv->data_plane_ops->link_state(dp_priv->dpc, 0)) {
		netdev_dbg(netdev, "Data plane set link failed\n");
		return -EAGAIN;
	}

	if (dp_priv->phydev)
		phy_stop(dp_priv->phydev);
	dp_priv->link_state = __NSS_DP_LINK_DOWN;

#if defined(NSS_DP_PPE_SUPPORT)
	/* Notify data plane to unassign VSI */
	if (dp_priv->data_plane_ops->vsi_unassign(dp_priv->dpc, dp_priv->vsi)) {
		netdev_dbg(netdev, "Data plane vsi unassign failed\n");
		return -EAGAIN;
	}
#endif

	/*
	 * Notify data plane to close
	 */
	if (dp_priv->data_plane_ops->close(dp_priv->dpc)) {
		netdev_dbg(netdev, "Data plane close failed\n");
		return -EAGAIN;
	}

	clear_bit(__NSS_DP_UP, &dp_priv->flags);

	return 0;
}

/*
 * nss_dp_open()
 */
static int nss_dp_open(struct net_device *netdev)
{
	struct nss_dp_dev *dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);

	if (!dp_priv)
		return -EINVAL;

	netif_carrier_off(netdev);

	/*
	 * Call data plane init if it has not been done yet
	 */
	if (!(dp_priv->drv_flags & NSS_DP_PRIV_FLAG(INIT_DONE))) {
		if (dp_priv->data_plane_ops->init(dp_priv->dpc)) {
			netdev_dbg(netdev, "Data plane init failed\n");
			return -ENOMEM;
		}

		dp_priv->drv_flags |= NSS_DP_PRIV_FLAG(INIT_DONE);
	}

	/*
	 * Inform the Linux Networking stack about the hardwar capability of
	 * checksum offloading and other features. Each data_plane is
	 * responsible to maintain the feature set it supports
	 */
	dp_priv->data_plane_ops->set_features(dp_priv->dpc);

	set_bit(__NSS_DP_UP, &dp_priv->flags);

#if defined(NSS_DP_PPE_SUPPORT)
	if (dp_priv->data_plane_ops->vsi_assign(dp_priv->dpc, dp_priv->vsi)) {
		netdev_dbg(netdev, "Data plane vsi assign failed\n");
		return -EAGAIN;
	}
#endif

	if (dp_priv->data_plane_ops->mac_addr(dp_priv->dpc, netdev->dev_addr)) {
		netdev_dbg(netdev, "Data plane set MAC address failed\n");
		return -EAGAIN;
	}

	if (dp_priv->data_plane_ops->change_mtu(dp_priv->dpc, netdev->mtu)) {
		netdev_dbg(netdev, "Data plane change mtu failed\n");
		return -EAGAIN;
	}

	if (dp_priv->data_plane_ops->open(dp_priv->dpc, 0, 0, 0)) {
		netdev_dbg(netdev, "Data plane open failed\n");
		return -EAGAIN;
	}

	netif_start_queue(netdev);

	if (!dp_priv->link_poll) {
		/* Notify data plane link is up */
		if (dp_priv->data_plane_ops->link_state(dp_priv->dpc, 1)) {
			netdev_dbg(netdev, "Data plane set link failed\n");
			return -EAGAIN;
		}
		dp_priv->link_state = __NSS_DP_LINK_UP;
		netif_carrier_on(netdev);
	} else {
		dp_priv->link_state = __NSS_DP_LINK_DOWN;
		phy_start(dp_priv->phydev);
		phy_start_aneg(dp_priv->phydev);
	}

	return 0;
}

#ifdef CONFIG_RFS_ACCEL
/*
 * nss_dp_rx_flow_steer()
 *	Steer the flow rule to NSS
 */
static int nss_dp_rx_flow_steer(struct net_device *netdev, const struct sk_buff *_skb,
				uint16_t rxq, uint32_t flow)
{
	struct nss_dp_dev *dp_priv;
	struct netdev_rx_queue *rxqueue;
	struct rps_sock_flow_table *sock_flow_table;
	struct rps_dev_flow_table *flow_table;
	struct rps_dev_flow *rxflow;
	struct sk_buff *skb = (struct sk_buff *)_skb;
	uint16_t index;
	uint32_t hash;
	uint32_t rfscpu;
	uint32_t rxcpu;

	if (!netdev)
		return -EINVAL;

	dp_priv = (struct nss_dp_dev *)netdev_priv(netdev);
	if (!dp_priv)
		return -EINVAL;

	rxqueue = netdev->_rx;

	if (skb_rx_queue_recorded(skb)) {
		index = skb_get_rx_queue(skb);
		rxqueue += index;
	}

	flow_table = rcu_dereference(rxqueue->rps_flow_table);
	if (!flow_table) {
		netdev_dbg(netdev, "RX queue RPS flow table not found\n");
		return -EINVAL;
	}

	hash = skb_get_hash(skb);
	rxflow = &flow_table->flows[hash & flow_table->mask];
	rxcpu = (uint32_t)rxflow->cpu;

	sock_flow_table = rcu_dereference(rps_sock_flow_table);
	if (!sock_flow_table) {
		netdev_dbg(netdev, "Global RPS flow table not found\n");
		return -EINVAL;
	}

	rfscpu = sock_flow_table->ents[hash & sock_flow_table->mask];
	rfscpu &= rps_cpu_mask;

	if (rxcpu == rfscpu)
		return 0;

	/*
	 * check rx_flow_steer is defined in data plane ops
	 */
	if (!dp_priv->data_plane_ops->rx_flow_steer) {
		netdev_dbg(netdev, "Data plane ops not defined for flow steer\n");
		return -EINVAL;
	}

	/*
	 * Delete the old flow rule
	 */
	if (dp_priv->data_plane_ops->rx_flow_steer(dp_priv->dpc, skb, rxcpu, false)) {
		netdev_dbg(netdev, "Data plane delete flow rule failed\n");
		return -EAGAIN;
	}

	/*
	 * Add the new flow rule
	 */
	if (dp_priv->data_plane_ops->rx_flow_steer(dp_priv->dpc, skb, rfscpu, true)) {
		netdev_dbg(netdev, "Data plane add flow rule failed\n");
		return -EAGAIN;
	}

	return 0;
}
#endif

/*
 * nss_dp_select_queue()
 *	Select tx queue
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
static u16 nss_dp_select_queue(struct net_device *netdev, struct sk_buff *skb,
				void *accel_priv, select_queue_fallback_t fallback)
#else
static u16 nss_dp_select_queue(struct net_device *netdev, struct sk_buff *skb,
				struct net_device *sb_dev)
#endif
{
	int cpu = get_cpu();
	put_cpu();

	/*
	 * The number of queue is matching the number of CPUs so get_cpu will
	 * always match a valid queue
	 */
	return cpu;
}

/*
 * Netdevice operations
 */
static const struct net_device_ops nss_dp_netdev_ops = {
	.ndo_open = nss_dp_open,
	.ndo_stop = nss_dp_close,
	.ndo_start_xmit = nss_dp_xmit,
	.ndo_get_stats64 = nss_dp_get_stats64,
	.ndo_set_mac_address = nss_dp_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_change_mtu = nss_dp_change_mtu,
	.ndo_do_ioctl = nss_dp_do_ioctl,

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
	.ndo_bridge_setlink = switchdev_port_bridge_setlink,
	.ndo_bridge_getlink = switchdev_port_bridge_getlink,
	.ndo_bridge_dellink = switchdev_port_bridge_dellink,
#endif
	.ndo_select_queue = nss_dp_select_queue,

#ifdef CONFIG_RFS_ACCEL
	.ndo_rx_flow_steer = nss_dp_rx_flow_steer,
#endif
};

/*
 * nss_dp_of_get_pdata()
 */
static int32_t nss_dp_of_get_pdata(struct device_node *np,
				   struct net_device *netdev,
				   struct gmac_hal_platform_data *hal_pdata)
{
	uint8_t *maddr;
	struct nss_dp_dev *dp_priv;
	struct resource memres_devtree = {0};

	dp_priv = netdev_priv(netdev);

	if (of_property_read_u32(np, "qcom,id", &dp_priv->macid)) {
		pr_err("%s: error reading id\n", np->name);
		return -EFAULT;
	}

	if (dp_priv->macid > NSS_DP_HAL_MAX_PORTS || !dp_priv->macid) {
		pr_err("%s: invalid macid %d\n", np->name, dp_priv->macid);
		return -EFAULT;
	}

	if (of_property_read_u32(np, "qcom,mactype", &hal_pdata->mactype)) {
		pr_err("%s: error reading mactype\n", np->name);
		return -EFAULT;
	}

	if (of_address_to_resource(np, 0, &memres_devtree) != 0)
		return -EFAULT;

	netdev->base_addr = memres_devtree.start;
	hal_pdata->reg_len = resource_size(&memres_devtree);
	hal_pdata->netdev = netdev;
	hal_pdata->macid = dp_priv->macid;

	dp_priv->phy_mii_type = of_get_phy_mode(np);
	dp_priv->link_poll = of_property_read_bool(np, "qcom,link-poll");
	if (of_property_read_u32(np, "qcom,phy-mdio-addr",
		&dp_priv->phy_mdio_addr) && dp_priv->link_poll) {
		pr_err("%s: mdio addr required if link polling is enabled\n",
				np->name);
		return -EFAULT;
	}

	of_property_read_u32(np, "qcom,forced-speed", &dp_priv->forced_speed);
	of_property_read_u32(np, "qcom,forced-duplex", &dp_priv->forced_duplex);

	maddr = (uint8_t *)of_get_mac_address(np);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(5, 4, 0))
	if (IS_ERR((void *)maddr)) {
		maddr = NULL;
	}
#endif

	if (maddr && is_valid_ether_addr(maddr)) {
		ether_addr_copy(netdev->dev_addr, maddr);
	} else {
		random_ether_addr(netdev->dev_addr);
		pr_info("GMAC%d(%px) Invalid MAC@ - using %pM\n", dp_priv->macid,
						dp_priv, netdev->dev_addr);
	}

	return 0;
}

/*
 * nss_dp_mdio_attach()
 */
static struct mii_bus *nss_dp_mdio_attach(struct platform_device *pdev)
{
	struct device_node *mdio_node;
	struct platform_device *mdio_plat;
	struct ipq40xx_mdio_data *mdio_data;

	/*
	 * Find mii_bus using "mdio-bus" handle.
	 */
	mdio_node = of_parse_phandle(pdev->dev.of_node, "mdio-bus", 0);
	if (mdio_node) {
		return of_mdio_find_bus(mdio_node);
	}

	mdio_node = of_find_compatible_node(NULL, NULL, "qcom,ipq40xx-mdio");
	if (!mdio_node) {
		dev_err(&pdev->dev, "cannot find mdio node by phandle\n");
		return NULL;
	}

	mdio_plat = of_find_device_by_node(mdio_node);
	if (!mdio_plat) {
		dev_err(&pdev->dev, "cannot find platform device from mdio node\n");
		of_node_put(mdio_node);
		return NULL;
	}

	mdio_data = dev_get_drvdata(&mdio_plat->dev);
	if (!mdio_data) {
		dev_err(&pdev->dev, "cannot get mii bus reference from device data\n");
		of_node_put(mdio_node);
		return NULL;
	}

	return mdio_data->mii_bus;
}

#ifdef CONFIG_NET_SWITCHDEV
/*
 * nss_dp_is_phy_dev()
 *	Check if it is dp device
 */
bool nss_dp_is_phy_dev(struct net_device *dev)
{
	return (dev->netdev_ops == &nss_dp_netdev_ops);
}
#endif

/*
 * nss_dp_adjust_link()
 */
void nss_dp_adjust_link(struct net_device *netdev)
{
	struct nss_dp_dev *dp_priv = netdev_priv(netdev);
	int current_state = dp_priv->link_state;

	if (!test_bit(__NSS_DP_UP, &dp_priv->flags))
		return;

	if (dp_priv->phydev->link && (current_state == __NSS_DP_LINK_UP))
		return;

	if (!dp_priv->phydev->link && (current_state == __NSS_DP_LINK_DOWN))
		return;

	if (current_state == __NSS_DP_LINK_DOWN) {
		netdev_info(netdev, "PHY Link up speed: %d\n",
						dp_priv->phydev->speed);
		if (dp_priv->data_plane_ops->link_state(dp_priv->dpc, 1)) {
			netdev_dbg(netdev, "Data plane set link up failed\n");
			return;
		}
		dp_priv->link_state = __NSS_DP_LINK_UP;
		netif_carrier_on(netdev);
	} else {
		netdev_info(netdev, "PHY Link is down\n");
		if (dp_priv->data_plane_ops->link_state(dp_priv->dpc, 0)) {
			netdev_dbg(netdev, "Data plane set link down failed\n");
			return;
		}
		dp_priv->link_state = __NSS_DP_LINK_DOWN;
		netif_carrier_off(netdev);
	}
}

/*
 * nss_dp_probe()
 */
static int32_t nss_dp_probe(struct platform_device *pdev)
{
	struct net_device *netdev;
	struct nss_dp_dev *dp_priv;
	struct device_node *np = pdev->dev.of_node;
	struct gmac_hal_platform_data gmac_hal_pdata;
	int32_t ret = 0;
	uint8_t phy_id[MII_BUS_ID_SIZE + 3];
#if defined(NSS_DP_PPE_SUPPORT)
	uint32_t vsi_id;
	fal_port_t port_id;
#endif

	/* TODO: See if we need to do some SoC level common init */

	netdev = alloc_etherdev_mqs(sizeof(struct nss_dp_dev),
			NSS_DP_NETDEV_TX_QUEUE_NUM, NSS_DP_NETDEV_RX_QUEUE_NUM);
	if (!netdev) {
		pr_info("alloc_etherdev() failed\n");
		return -ENOMEM;
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0))
	/* max_mtu is set to 1500 in ether_setup() */
	netdev->max_mtu = ETH_MAX_MTU;
#endif

	dp_priv = netdev_priv(netdev);
	memset((void *)dp_priv, 0, sizeof(struct nss_dp_dev));

	dp_priv->pdev = pdev;
	dp_priv->netdev = netdev;
	netdev->watchdog_timeo = 5 * HZ;
	netdev->netdev_ops = &nss_dp_netdev_ops;
	nss_dp_set_ethtool_ops(netdev);
#ifdef CONFIG_NET_SWITCHDEV
	nss_dp_switchdev_setup(netdev);
#endif

	ret = nss_dp_of_get_pdata(np, netdev, &gmac_hal_pdata);
	if (ret != 0) {
		goto fail;
	}

	/* Use data plane ops as per the configured SoC */
	dp_priv->data_plane_ops = nss_dp_hal_get_data_plane_ops();
	if (!dp_priv->data_plane_ops) {
		netdev_dbg(netdev, "Dataplane ops not found.\n");
		goto fail;
	}

	dp_priv->dpc = &dp_global_data_plane_ctx[dp_priv->macid-1];
	dp_priv->dpc->dev = netdev;
	dp_priv->ctx = &dp_global_ctx;

	/* TODO:locks init */

	/*
	 * HAL's init function will return the pointer to the HAL context
	 * (private to hal), which dp will store in its data structures.
	 * The subsequent hal_ops calls expect the DP to pass the HAL
	 * context pointer as an argument
	 */
	dp_priv->gmac_hal_ops = nss_dp_hal_get_gmac_ops(gmac_hal_pdata.mactype);
	if (!dp_priv->gmac_hal_ops) {
		netdev_dbg(netdev, "Unsupported Mac type: %d\n", gmac_hal_pdata.mactype);
		goto fail;
	}

	dp_priv->gmac_hal_ctx = dp_priv->gmac_hal_ops->init(&gmac_hal_pdata);
	if (!(dp_priv->gmac_hal_ctx)) {
		netdev_dbg(netdev, "gmac hal init failed\n");
		goto fail;
	}

	if (dp_priv->link_poll) {
		dp_priv->miibus = nss_dp_mdio_attach(pdev);
		if (!dp_priv->miibus) {
			netdev_dbg(netdev, "failed to find miibus\n");
			goto fail;
		}
		snprintf(phy_id, MII_BUS_ID_SIZE + 3, PHY_ID_FMT,
				dp_priv->miibus->id, dp_priv->phy_mdio_addr);

		SET_NETDEV_DEV(netdev, &pdev->dev);

		dp_priv->phydev = phy_connect(netdev, phy_id,
				&nss_dp_adjust_link,
				dp_priv->phy_mii_type);
		if (IS_ERR(dp_priv->phydev)) {
			netdev_dbg(netdev, "failed to connect to phy device\n");
			goto fail;
		}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
		dp_priv->phydev->advertising |=
			(ADVERTISED_Pause | ADVERTISED_Asym_Pause);
		dp_priv->phydev->supported |=
			(SUPPORTED_Pause | SUPPORTED_Asym_Pause);
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_Pause_BIT, dp_priv->phydev->advertising);
		linkmode_set_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, dp_priv->phydev->advertising);

		linkmode_set_bit(ETHTOOL_LINK_MODE_Pause_BIT, dp_priv->phydev->supported);
		linkmode_set_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, dp_priv->phydev->supported);
#endif
	}

#if defined(NSS_DP_PPE_SUPPORT)
	/* Get port's default VSI */
	port_id = dp_priv->macid;
	if (ppe_port_vsi_get(0, port_id, &vsi_id)) {
		netdev_dbg(netdev, "failed to get port's default VSI\n");
		goto fail;
	}

	dp_priv->vsi = vsi_id;
#endif

	/* TODO: Features: CSUM, tx/rx offload... configure */

	/* Register the network interface */
	ret = register_netdev(netdev);
	if (ret) {
		netdev_dbg(netdev, "Error registering netdevice %s\n",
								netdev->name);
		dp_priv->gmac_hal_ops->exit(dp_priv->gmac_hal_ctx);
		goto fail;
	}

	dp_global_ctx.nss_dp[dp_priv->macid - 1] = dp_priv;
	dp_global_ctx.slowproto_acl_bm = 0;

	netdev_dbg(netdev, "Init NSS DP GMAC%d (base = 0x%lx)\n", dp_priv->macid, netdev->base_addr);

	return 0;

fail:
	free_netdev(netdev);
	return -EFAULT;
}

/*
 * nss_dp_remove()
 */
static int nss_dp_remove(struct platform_device *pdev)
{
	uint32_t i;
	struct nss_dp_dev *dp_priv;
	struct nss_gmac_hal_ops *hal_ops;

	for (i = 0; i < NSS_DP_HAL_MAX_PORTS; i++) {
		dp_priv = dp_global_ctx.nss_dp[i];
		if (!dp_priv)
			continue;

		hal_ops = dp_priv->gmac_hal_ops;
		if (dp_priv->phydev)
			phy_disconnect(dp_priv->phydev);
		unregister_netdev(dp_priv->netdev);
		hal_ops->exit(dp_priv->gmac_hal_ctx);
		free_netdev(dp_priv->netdev);
		dp_global_ctx.nss_dp[i] = NULL;
	}

	return 0;
}

static struct of_device_id nss_dp_dt_ids[] = {
	{ .compatible = "qcom,nss-dp" },
	{},
};
MODULE_DEVICE_TABLE(of, nss_dp_dt_ids);

static struct platform_driver nss_dp_drv = {
	.probe = nss_dp_probe,
	.remove = nss_dp_remove,
	.driver = {
		   .name = "nss-dp",
		   .owner = THIS_MODULE,
		   .of_match_table = of_match_ptr(nss_dp_dt_ids),
		  },
};

/*
 * nss_dp_init()
 */
int __init nss_dp_init(void)
{
	int ret;

	/*
	 * Bail out on not supported platform
	 * TODO: Handle this properly with SoC ops
	 */
	if (!of_machine_is_compatible("qcom,ipq807x") &&
			!of_machine_is_compatible("qcom,ipq8074") &&
			!of_machine_is_compatible("qcom,ipq6018") &&
			!of_machine_is_compatible("qcom,ipq5018"))
		return 0;

	/*
	 * TODO Move this to soc_ops
	 */
	dp_global_ctx.common_init_done = false;
	if (!nss_dp_hal_init()) {
		pr_err("DP hal init failed.\n");
		return -EFAULT;
	}

	ret = platform_driver_register(&nss_dp_drv);
	if (ret)
		pr_info("NSS DP platform drv register failed\n");

	dp_global_ctx.common_init_done = true;
	pr_info("**********************************************************\n");
	pr_info("* NSS Data Plane driver\n");
	pr_info("**********************************************************\n");

	return ret;
}

/*
 * nss_dp_exit()
 */
void __exit nss_dp_exit(void)
{

	/*
	 * TODO Move this to soc_ops
	 */
	if (dp_global_ctx.common_init_done) {
		nss_dp_hal_cleanup();
		dp_global_ctx.common_init_done = false;
	}

	platform_driver_unregister(&nss_dp_drv);
}

module_init(nss_dp_init);
module_exit(nss_dp_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("NSS Data Plane Network Driver");
