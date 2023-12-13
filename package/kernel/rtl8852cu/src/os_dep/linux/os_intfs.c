/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _OS_INTFS_C_

#include <drv_types.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek Wireless Lan Driver");
MODULE_AUTHOR("Realtek Semiconductor Corp.");
MODULE_VERSION(DRIVERVERSION);


int netdev_open(struct net_device *pnetdev);
static int netdev_close(struct net_device *pnetdev);


/**
 * rtw_net_set_mac_address
 * This callback function is used for the Media Access Control address
 * of each net_device needs to be changed.
 *
 * Arguments:
 * @pnetdev: net_device pointer.
 * @addr: new MAC address.
 *
 * Return:
 * ret = 0: Permit to change net_device's MAC address.
 * ret = -1 (Default): Operation not permitted.
 *
 * Auther: Arvin Liu
 * Date: 2015/05/29
 */
static int rtw_net_set_mac_address(struct net_device *pnetdev, void *addr)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sockaddr *sa = (struct sockaddr *)addr;
	int ret = -1;

	/* only the net_device is in down state to permit modifying mac addr */
	if ((pnetdev->flags & IFF_UP) == _TRUE) {
		RTW_INFO(FUNC_ADPT_FMT": The net_device's is not in down state\n"
			 , FUNC_ADPT_ARG(padapter));

		return ret;
	}

	/* if the net_device is linked, it's not permit to modify mac addr */
	if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING) ||
	    check_fwstate(pmlmepriv, WIFI_ASOC_STATE) ||
	    check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY)) {
		RTW_INFO(FUNC_ADPT_FMT": The net_device's is not idle currently\n"
			 , FUNC_ADPT_ARG(padapter));

		return ret;
	}

	/* check whether the input mac address is valid to permit modifying mac addr */
	if (rtw_check_invalid_mac_address(sa->sa_data, _FALSE) == _TRUE) {
		RTW_INFO(FUNC_ADPT_FMT": Invalid Mac Addr for "MAC_FMT"\n"
			 , FUNC_ADPT_ARG(padapter), MAC_ARG(sa->sa_data));

		return ret;
	}

	_rtw_memcpy(adapter_mac_addr(padapter), sa->sa_data, ETH_ALEN); /* set mac addr to adapter */
	dev_addr_mod(pnetdev, 0, sa->sa_data, ETH_ALEN);

	/* Since the net_device is in down state, there is no wrole at this moment.
	 * The new mac address will be set to hw when changing the net_device to up state.
	 */

	RTW_INFO(FUNC_ADPT_FMT": Set Mac Addr to "MAC_FMT" Successfully\n"
		 , FUNC_ADPT_ARG(padapter), MAC_ARG(sa->sa_data));

	ret = 0;

	return ret;
}

static struct net_device_stats *rtw_net_get_stats(struct net_device *pnetdev)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct xmit_priv *pxmitpriv = &(padapter->xmitpriv);
	struct recv_info *precvinfo = &(padapter->recvinfo);

	padapter->stats.tx_packets = pxmitpriv->tx_pkts;/* pxmitpriv->tx_pkts++; */
	padapter->stats.rx_packets = precvinfo->rx_pkts;/* precvinfo->rx_pkts++; */
	padapter->stats.tx_dropped = pxmitpriv->tx_drop;
	padapter->stats.rx_dropped = precvinfo->rx_drop;
	padapter->stats.tx_bytes = pxmitpriv->tx_bytes;
	padapter->stats.rx_bytes = precvinfo->rx_bytes;

	return &padapter->stats;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
/*
 * AC to queue mapping
 *
 * AC_VO -> queue 0
 * AC_VI -> queue 1
 * AC_BE -> queue 2
 * AC_BK -> queue 3
 */
static const u16 rtw_1d_to_queue[8] = { 2, 3, 3, 2, 1, 1, 0, 0 };

/* Given a data frame determine the 802.1p/1d tag to use. */
unsigned int rtw_classify8021d(struct sk_buff *skb)
{
	unsigned int dscp;

	/* skb->priority values from 256->263 are magic values to
	 * directly indicate a specific 802.1d priority.  This is used
	 * to allow 802.1d priority to be passed directly in from VLAN
	 * tags, etc.
	 */
	if (skb->priority >= 256 && skb->priority <= 263)
		return skb->priority - 256;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		dscp = ip_hdr(skb)->tos & 0xfc;
		break;
	default:
	#ifdef PRIVATE_R
		return skb->priority;
	#else
		return 0;
	#endif
	}

	return dscp >> 5;
}


static u16 rtw_select_queue(struct net_device *dev, struct sk_buff *skb
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
	, struct net_device *sb_dev
	#else
	, void *accel_priv
	#endif
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5, 2, 0))
	, select_queue_fallback_t fallback
	#endif
#endif
)
{
	_adapter	*padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	skb->priority = rtw_classify8021d(skb);

	if (pmlmepriv->acm_mask != 0)
		skb->priority = qos_acm(pmlmepriv->acm_mask, skb->priority);

	return rtw_1d_to_queue[skb->priority];
}
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)) */

u16 rtw_os_recv_select_queue(u8 *msdu, enum rtw_rx_llc_hdl llc_hdl)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	u32 priority = 0;

	if (llc_hdl == RTW_RX_LLC_REMOVE) {
		u16 eth_type = RTW_GET_BE16(msdu + SNAP_SIZE);

		if (eth_type == ETH_P_IP) {
			struct iphdr *iphdr = (struct iphdr *)(msdu + SNAP_SIZE + 2);
			unsigned int dscp = iphdr->tos & 0xfc;

			priority = dscp >> 5;
		}
	}

	return rtw_1d_to_queue[priority];
#else
	return 0;
#endif
}

static u8 is_rtw_ndev(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	return ndev->netdev_ops
		&& ndev->netdev_ops->ndo_do_ioctl
		&& ndev->netdev_ops->ndo_do_ioctl == rtw_ioctl;
#else
	return ndev->do_ioctl
		&& ndev->do_ioctl == rtw_ioctl;
#endif
}


#define _netdev_status_msg(_ndev, state, sts_str)		\
	RTW_INFO(FUNC_NDEV_FMT" state:%lu - %s\n", FUNC_NDEV_ARG(_ndev), state, sts_str);

static int rtw_ndev_notifier_call(struct notifier_block *nb, unsigned long state, void *ptr)
{
	struct net_device *ndev;

	if (ptr == NULL)
		return NOTIFY_DONE;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	ndev = netdev_notifier_info_to_dev(ptr);
#else
	ndev = ptr;
#endif

	if (ndev == NULL)
		return NOTIFY_DONE;

	if (!is_rtw_ndev(ndev))
		return NOTIFY_DONE;

	switch (state) {
	case NETDEV_CHANGE:
		_netdev_status_msg(ndev, state, "netdev change");
		break;
	case NETDEV_GOING_DOWN:
		_netdev_status_msg(ndev, state, "netdev going down");
		break;
	case NETDEV_DOWN:
		_netdev_status_msg(ndev, state, "netdev down");
		break;
	case NETDEV_UP:
		_netdev_status_msg(ndev, state, "netdev up");
		break;
	case NETDEV_REBOOT:
		_netdev_status_msg(ndev, state, "netdev reboot");
		break;
	case NETDEV_CHANGENAME:
		rtw_adapter_proc_replace(ndev);
		_netdev_status_msg(ndev, state, "netdev chang ename");
		break;
	case NETDEV_PRE_UP :
		_netdev_status_msg(ndev, state, "netdev pre up");
		break;
	case NETDEV_JOIN:
		_netdev_status_msg(ndev, state, "netdev join");
		break;
	default:
		_netdev_status_msg(ndev, state, " ");
		break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block rtw_ndev_notifier = {
	.notifier_call = rtw_ndev_notifier_call,
};

int rtw_ndev_notifier_register(void)
{
	return register_netdevice_notifier(&rtw_ndev_notifier);
}

void rtw_ndev_notifier_unregister(void)
{
	unregister_netdevice_notifier(&rtw_ndev_notifier);
}

int rtw_ndev_init(struct net_device *dev)
{
	_adapter *adapter = rtw_netdev_priv(dev);

	RTW_PRINT(FUNC_ADPT_FMT" if%d mac_addr="MAC_FMT"\n"
		, FUNC_ADPT_ARG(adapter), (adapter->iface_id + 1), MAC_ARG(dev->dev_addr));
	strncpy(adapter->old_ifname, dev->name, IFNAMSIZ);
	adapter->old_ifname[IFNAMSIZ - 1] = '\0';
#ifdef CONFIG_ARCH_CORTINA
	dev->priv_flags = IFF_DOMAIN_WLAN;
#endif
	rtw_adapter_proc_init(dev);

#ifdef CONFIG_RTW_NAPI
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 1,0)
	netif_napi_add(dev, &adapter->napi, rtw_recv_napi_poll, RTL_NAPI_WEIGHT);
#else
	netif_napi_add(dev, &adapter->napi, rtw_recv_napi_poll);
#endif
#endif /* CONFIG_RTW_NAPI */

	return 0;
}

void rtw_ndev_uninit(struct net_device *dev)
{
	_adapter *adapter = rtw_netdev_priv(dev);

	RTW_PRINT(FUNC_ADPT_FMT" if%d\n"
		  , FUNC_ADPT_ARG(adapter), (adapter->iface_id + 1));
	rtw_adapter_proc_deinit(dev);

#ifdef CONFIG_RTW_NAPI
	if(adapter->napi_state == NAPI_ENABLE) {
		napi_disable(&adapter->napi);
		adapter->napi_state = NAPI_DISABLE;
	}
	netif_napi_del(&adapter->napi);
#endif /* CONFIG_RTW_NAPI */
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
static int rtw_siocdevprivate(struct net_device *dev, struct ifreq *ifr,
			      void __user *data, int cmd)
{
	/* handle cmd(s) between SIOCDEVPRIVATE and SIOCDEVPRIVATE + 15 */

	return rtw_ioctl(dev, ifr, cmd);
}
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
static const struct net_device_ops rtw_netdev_ops = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = netdev_open,
	.ndo_stop = netdev_close,
	.ndo_start_xmit = rtw_xmit_entry,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	.ndo_select_queue	= rtw_select_queue,
#endif
	.ndo_set_mac_address = rtw_net_set_mac_address,
	.ndo_get_stats = rtw_net_get_stats,
	.ndo_do_ioctl = rtw_ioctl,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	.ndo_siocdevprivate = rtw_siocdevprivate,
#endif
};
#endif

int rtw_init_netdev_name(struct net_device *pnetdev, const char *ifname)
{
	if (dev_alloc_name(pnetdev, ifname) < 0)
		RTW_ERR("dev_alloc_name, fail!\n");

	rtw_netif_carrier_off(pnetdev);
	/* rtw_netif_stop_queue(pnetdev); */

	return 0;
}

void rtw_hook_if_ops(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	ndev->netdev_ops = &rtw_netdev_ops;
#else
	ndev->init = rtw_ndev_init;
	ndev->uninit = rtw_ndev_uninit;
	ndev->open = netdev_open;
	ndev->stop = netdev_close;
	ndev->hard_start_xmit = rtw_xmit_entry;
	ndev->set_mac_address = rtw_net_set_mac_address;
	ndev->get_stats = rtw_net_get_stats;
	ndev->do_ioctl = rtw_ioctl;
#endif
}

#ifdef CONFIG_CONCURRENT_MODE
static void rtw_hook_vir_if_ops(struct net_device *ndev);
#endif
struct net_device *rtw_init_netdev(_adapter *old_padapter)
{
	_adapter *padapter;
	struct net_device *pnetdev;

	if (old_padapter != NULL) {
		rtw_os_ndev_free(old_padapter);
		pnetdev = rtw_alloc_etherdev_with_old_priv(sizeof(_adapter), (void *)old_padapter);
	} else
		pnetdev = rtw_alloc_etherdev(sizeof(_adapter));

	if (!pnetdev)
		return NULL;

	padapter = rtw_netdev_priv(pnetdev);
	padapter->pnetdev = pnetdev;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
	SET_MODULE_OWNER(pnetdev);
#endif

	rtw_hook_if_ops(pnetdev);
#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter))
		rtw_hook_vir_if_ops(pnetdev);
#endif /* CONFIG_CONCURRENT_MODE */


#ifdef CONFIG_TCP_CSUM_OFFLOAD_TX
        pnetdev->features |= (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
        pnetdev->hw_features |= (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM);
#endif
#endif

#ifdef CONFIG_RTW_NETIF_SG
        pnetdev->features |= NETIF_F_SG;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
        pnetdev->hw_features |= NETIF_F_SG;
#endif
#endif

	if ((pnetdev->features & NETIF_F_SG) && (pnetdev->features & NETIF_F_IP_CSUM)) {
		pnetdev->features |= (NETIF_F_TSO | NETIF_F_GSO);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
		pnetdev->hw_features |= (NETIF_F_TSO | NETIF_F_GSO);
#endif
	}
	/* pnetdev->tx_timeout = NULL; */
	pnetdev->watchdog_timeo = HZ * 3; /* 3 second timeout */

#ifdef CONFIG_WIRELESS_EXT
	pnetdev->wireless_handlers = (struct iw_handler_def *)&rtw_handlers_def;
#endif

#ifdef WIRELESS_SPY
	/* priv->wireless_data.spy_data = &priv->spy_data; */
	/* pnetdev->wireless_data = &priv->wireless_data; */
#endif

#ifdef CONFIG_TX_AMSDU_SW_MODE
	pnetdev->needed_headroom += 8;	/* +8 for rfc1042 header */
	pnetdev->needed_headroom += 4;	/* +4 for padding */
#endif

	return pnetdev;
}
#ifdef CONFIG_PCI_HCI
#include <rtw_trx_pci.h>
#endif
int rtw_os_ndev_alloc(_adapter *adapter)
{
	int ret = _FAIL;
	struct net_device *ndev = NULL;

	ndev = rtw_init_netdev(adapter);
	if (ndev == NULL) {
		rtw_warn_on(1);
		goto exit;
	}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0)
	SET_NETDEV_DEV(ndev, dvobj_to_dev(adapter_to_dvobj(adapter)));
#endif

#ifdef CONFIG_PCI_HCI
	if (is_pci_support_dma64(adapter_to_dvobj(adapter)))
		ndev->features |= NETIF_F_HIGHDMA;
	ndev->irq = dvobj_to_pci(adapter_to_dvobj(adapter))->irq;
#endif

#if defined(CONFIG_IOCTL_CFG80211)
	if (rtw_cfg80211_ndev_res_alloc(adapter) != _SUCCESS) {
		rtw_warn_on(1);
	} else
#endif
	ret = _SUCCESS;

	if (ret != _SUCCESS && ndev)
		rtw_free_netdev(ndev);
exit:
	return ret;
}

void rtw_os_ndev_free(_adapter *adapter)
{
#if defined(CONFIG_IOCTL_CFG80211)
	rtw_cfg80211_ndev_res_free(adapter);
#endif

	/* free the old_pnetdev */
	if (adapter->rereg_nd_name_priv.old_pnetdev) {
		rtw_free_netdev(adapter->rereg_nd_name_priv.old_pnetdev);
		adapter->rereg_nd_name_priv.old_pnetdev = NULL;
	}

	if (adapter->pnetdev) {
		rtw_free_netdev(adapter->pnetdev);
		adapter->pnetdev = NULL;
	}
}

/* For ethtool +++ */
#ifdef CONFIG_IOCTL_CFG80211
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 8))
static void rtw_ethtool_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct wireless_dev *wdev = NULL;
	_adapter *padapter = NULL;

	wdev = dev->ieee80211_ptr;
	if (wdev) {
		strlcpy(info->driver, wiphy_dev(wdev->wiphy)->driver->name,
			sizeof(info->driver));
	} else {
		strlcpy(info->driver, "N/A", sizeof(info->driver));
	}

	strlcpy(info->version, DRIVERVERSION, sizeof(info->version));

	padapter = (_adapter *)rtw_netdev_priv(dev);


	/*GEORGIA_TODO_FIXIT*/
	#if 0
	HAL_DATA_TYPE *hal_data = NULL;
	if (padapter) {
		hal_data = GET_PHL_COM(padapter);
	}
	if (hal_data) {
		scnprintf(info->fw_version, sizeof(info->fw_version), "%d.%d",
			  hal_data->firmware_version, hal_data->firmware_sub_version);
	} else
	#endif
	{
		strlcpy(info->fw_version, "N/A", sizeof(info->fw_version));
	}

	strlcpy(info->bus_info, dev_name(wiphy_dev(wdev->wiphy)),
		sizeof(info->bus_info));
}

static const char rtw_ethtool_gstrings_sta_stats[][ETH_GSTRING_LEN] = {
	"rx_packets", "rx_bytes", "rx_dropped",
	"tx_packets", "tx_bytes", "tx_dropped",
};

#define RTW_ETHTOOL_STATS_LEN	ARRAY_SIZE(rtw_ethtool_gstrings_sta_stats)

static int rtw_ethtool_get_sset_count(struct net_device *dev, int sset)
{
	int rv = 0;

	if (sset == ETH_SS_STATS)
		rv += RTW_ETHTOOL_STATS_LEN;

	if (rv == 0)
		return -EOPNOTSUPP;

	return rv;
}

static void rtw_ethtool_get_strings(struct net_device *dev, u32 sset, u8 *data)
{
	int sz_sta_stats = 0;

	if (sset == ETH_SS_STATS) {
		sz_sta_stats = sizeof(rtw_ethtool_gstrings_sta_stats);
		_rtw_memcpy(data, rtw_ethtool_gstrings_sta_stats, sz_sta_stats);
	}
}

static void rtw_ethtool_get_stats(struct net_device *dev,
				  struct ethtool_stats *stats,
				  u64 *data)
{
	int i = 0;
	_adapter *padapter = NULL;
	struct xmit_priv *pxmitpriv = NULL;
	struct recv_info *precvinfo = NULL;

	memset(data, 0, sizeof(u64) * RTW_ETHTOOL_STATS_LEN);

	padapter = (_adapter *)rtw_netdev_priv(dev);
	if (padapter) {
		pxmitpriv = &(padapter->xmitpriv);
		precvinfo = &(padapter->recvinfo);

		data[i++] = precvinfo->rx_pkts;
		data[i++] = precvinfo->rx_bytes;
		data[i++] = precvinfo->rx_drop;

		data[i++] = pxmitpriv->tx_pkts;
		data[i++] = pxmitpriv->tx_bytes;
		data[i++] = pxmitpriv->tx_drop;
	} else {
		data[i++] = 0;
		data[i++] = 0;
		data[i++] = 0;

		data[i++] = 0;
		data[i++] = 0;
		data[i++] = 0;
	}
}

static const struct ethtool_ops rtw_ethtool_ops = {
	.get_drvinfo = rtw_ethtool_get_drvinfo,
	.get_link = ethtool_op_get_link,
	.get_strings = rtw_ethtool_get_strings,
	.get_ethtool_stats = rtw_ethtool_get_stats,
	.get_sset_count = rtw_ethtool_get_sset_count,
};
#endif // LINUX_VERSION_CODE >= 3.7.8
#endif /* CONFIG_IOCTL_CFG80211 */
/* For ethtool --- */

int rtw_os_ndev_register(_adapter *adapter, const char *name)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	int ret = _SUCCESS;
	struct net_device *ndev = adapter->pnetdev;
	u8 rtnl_lock_needed = rtw_rtnl_lock_needed(dvobj);

#if defined(CONFIG_IOCTL_CFG80211)
	if (rtw_cfg80211_ndev_res_register(adapter) != _SUCCESS) {
		rtw_warn_on(1);
		ret = _FAIL;
		goto exit;
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 8))
	netdev_set_default_ethtool_ops(ndev, &rtw_ethtool_ops);
#endif /* LINUX_VERSION_CODE >= 3.7.8 */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) && defined(CONFIG_PCI_HCI)
	ndev->gro_flush_timeout = 100000;
#endif
	/* alloc netdev name */
	rtw_init_netdev_name(ndev, name);

	dev_addr_mod(ndev, 0, adapter_mac_addr(adapter), ETH_ALEN);

	/* Tell the network stack we exist */

	if (rtnl_lock_needed)
		ret = (register_netdev(ndev) == 0) ? _SUCCESS : _FAIL;
	else
		ret = (register_netdevice(ndev) == 0) ? _SUCCESS : _FAIL;

	if (ret == _SUCCESS)
		adapter->registered = 1;
	else
		RTW_INFO(FUNC_NDEV_FMT" if%d Failed!\n", FUNC_NDEV_ARG(ndev), (adapter->iface_id + 1));

#if defined(CONFIG_IOCTL_CFG80211)
	if (ret != _SUCCESS) {
		rtw_cfg80211_ndev_res_unregister(adapter);
	}
#endif

#if defined(CONFIG_IOCTL_CFG80211)
exit:
#endif

	return ret;
}

void rtw_os_ndev_unregister(_adapter *adapter)
{
	struct net_device *netdev = NULL;

	if (adapter == NULL || adapter->registered == 0)
		return;

	adapter->ndev_unregistering = 1;

	netdev = adapter->pnetdev;

#if defined(CONFIG_IOCTL_CFG80211)
	rtw_cfg80211_ndev_res_unregister(adapter);
#endif

	if (netdev) {
		struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
		u8 rtnl_lock_needed = rtw_rtnl_lock_needed(dvobj);

		if (rtnl_lock_needed)
			unregister_netdev(netdev);
		else
			unregister_netdevice(netdev);
	}

	adapter->registered = 0;
	adapter->ndev_unregistering = 0;
}

/**
 * rtw_os_ndev_init - Allocate and register OS layer net device and relating structures for @adapter
 * @adapter: the adapter on which this function applies
 * @name: the requesting net device name
 *
 * Returns:
 * _SUCCESS or _FAIL
 */
int rtw_os_ndev_init(_adapter *adapter, const char *name)
{
	int ret = _FAIL;

	if (rtw_os_ndev_alloc(adapter) != _SUCCESS)
		goto exit;

	if (rtw_os_ndev_register(adapter, name) != _SUCCESS)
		goto os_ndev_free;

	ret = _SUCCESS;

os_ndev_free:
	if (ret != _SUCCESS)
		rtw_os_ndev_free(adapter);
exit:
	return ret;
}

/**
 * rtw_os_ndev_deinit - Unregister and free OS layer net device and relating structures for @adapter
 * @adapter: the adapter on which this function applies
 */
void rtw_os_ndev_deinit(_adapter *adapter)
{
	rtw_os_ndev_unregister(adapter);
	rtw_os_ndev_free(adapter);
}

int rtw_os_ndevs_alloc(struct dvobj_priv *dvobj)
{
	int i, status = _SUCCESS;
	_adapter *adapter;

#if defined(CONFIG_IOCTL_CFG80211)
	if (rtw_cfg80211_dev_res_alloc(dvobj) != _SUCCESS) {
		rtw_warn_on(1);
		return _FAIL;
	}
#endif

	for (i = 0; i < dvobj->iface_nums; i++) {

		if (i >= CONFIG_IFACE_NUMBER) {
			RTW_ERR("%s %d >= CONFIG_IFACE_NUMBER(%d)\n", __func__, i, CONFIG_IFACE_NUMBER);
			rtw_warn_on(1);
			continue;
		}

		adapter = dvobj->padapters[i];
		if (adapter && !adapter->pnetdev) {

			#ifdef CONFIG_RTW_DYNAMIC_NDEV
			if (!is_primary_adapter(adapter) &&
				(i >= CONFIG_RTW_STATIC_NDEV_NUM))
				break;
			#endif

			status = rtw_os_ndev_alloc(adapter);
			if (status != _SUCCESS) {
				rtw_warn_on(1);
				break;
			}
		}
	}

	if (status != _SUCCESS) {
		for (; i >= 0; i--) {
			adapter = dvobj->padapters[i];
			if (adapter && adapter->pnetdev)
				rtw_os_ndev_free(adapter);
		}
	}

#if defined(CONFIG_IOCTL_CFG80211)
	if (status != _SUCCESS)
		rtw_cfg80211_dev_res_free(dvobj);
#endif

	return status;
}

void rtw_os_ndevs_free(struct dvobj_priv *dvobj)
{
	int i;
	_adapter *adapter = NULL;

	for (i = 0; i < dvobj->iface_nums; i++) {

		if (i >= CONFIG_IFACE_NUMBER) {
			RTW_ERR("%s %d >= CONFIG_IFACE_NUMBER(%d)\n", __func__, i, CONFIG_IFACE_NUMBER);
			rtw_warn_on(1);
			continue;
		}

		adapter = dvobj->padapters[i];

		if (adapter == NULL)
			continue;

		rtw_os_ndev_free(adapter);
	}

#if defined(CONFIG_IOCTL_CFG80211)
	rtw_cfg80211_dev_res_free(dvobj);
#endif
}

#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
u32 rtw_start_drv_threads(_adapter *padapter)
{
	u32 _status = _SUCCESS;

	RTW_INFO(FUNC_ADPT_FMT" enter\n", FUNC_ADPT_ARG(padapter));

#ifdef CONFIG_XMIT_THREAD_MODE
#if defined(CONFIG_SDIO_HCI)
	if (is_primary_adapter(padapter))
#endif
	{
		if (padapter->xmitThread == NULL) {
			RTW_INFO(FUNC_ADPT_FMT " start RTW_XMIT_THREAD\n", FUNC_ADPT_ARG(padapter));
			padapter->xmitThread = rtw_thread_start(rtw_xmit_thread, padapter, "RTW_XMIT_THREAD");
			if (padapter->xmitThread == NULL)
				_status = _FAIL;
		}
	}
#endif /* #ifdef CONFIG_XMIT_THREAD_MODE */

#ifdef CONFIG_RECV_THREAD_MODE
	if (is_primary_adapter(padapter)) {
		if (padapter->recvThread == NULL) {
			RTW_INFO(FUNC_ADPT_FMT " start RTW_RECV_THREAD\n", FUNC_ADPT_ARG(padapter));
			padapter->recvThread = rtw_thread_start(rtw_recv_thread, padapter, "RTW_RECV_THREAD");
			if (padapter->recvThread == NULL)
				_status = _FAIL;
		}
	}
#endif

	if (is_primary_adapter(padapter)) {
		if (padapter->cmdThread == NULL) {
			RTW_INFO(FUNC_ADPT_FMT " start RTW_CMD_THREAD\n", FUNC_ADPT_ARG(padapter));
			padapter->cmdThread = rtw_thread_start(rtw_cmd_thread, padapter, "RTW_CMD_THREAD");
			if (padapter->cmdThread == NULL)
				_status = _FAIL;
			else
				_rtw_down_sema(&padapter->cmdpriv.start_cmdthread_sema); /* wait for cmd_thread to run */
		}
	}

	_status = rtw_intf_start_xmit_frame_thread(padapter);
	return _status;

}

void rtw_stop_drv_threads(_adapter *padapter)
{
	RTW_INFO(FUNC_ADPT_FMT" enter\n", FUNC_ADPT_ARG(padapter));
	if (is_primary_adapter(padapter))
		rtw_stop_cmd_thread(padapter);

#ifdef CONFIG_XMIT_THREAD_MODE
	/* Below is to termindate tx_thread... */
#if defined(CONFIG_SDIO_HCI)
	/* Only wake-up primary adapter */
	if (is_primary_adapter(padapter))
#endif  /*SDIO_HCI */
	{
		if (padapter->xmitThread) {
			_rtw_up_sema(&padapter->xmitpriv.xmit_sema);
			rtw_thread_stop(padapter->xmitThread);
			padapter->xmitThread = NULL;
		}
	}
#endif

#ifdef CONFIG_RECV_THREAD_MODE
	if (is_primary_adapter(padapter) && padapter->recvThread) {
		/* Below is to termindate rx_thread... */
		_rtw_up_sema(&padapter->recvpriv.recv_sema);
		rtw_thread_stop(padapter->recvThread);
		padapter->recvThread = NULL;
	}
#endif

	/*rtw_hal_stop_thread(padapter);*/
	rtw_intf_cancel_xmit_frame_thread(padapter);

}
#endif

u8 rtw_init_default_value(_adapter *padapter)
{
	u8 ret  = _SUCCESS;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;

	/* xmit_priv */
	pxmitpriv->vcs_setting = pregistrypriv->vrtl_carrier_sense;
	pxmitpriv->vcs = pregistrypriv->vcs_type;
	pxmitpriv->vcs_type = pregistrypriv->vcs_type;
	/* pxmitpriv->rts_thresh = pregistrypriv->rts_thresh; */
	pxmitpriv->frag_len = pregistrypriv->frag_thresh;

	/* security_priv */
	/* rtw_get_encrypt_decrypt_from_registrypriv(padapter); */
#ifdef CONFIG_GTK_OL
	psecuritypriv->binstallKCK_KEK = _FAIL;
#endif /* CONFIG_GTK_OL */
	psecuritypriv->sw_encrypt = pregistrypriv->software_encrypt;
	psecuritypriv->sw_decrypt = pregistrypriv->software_decrypt;

	psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_Open; /* open system */
	psecuritypriv->dot11PrivacyAlgrthm = _NO_PRIVACY_;

	psecuritypriv->dot11PrivacyKeyIndex = 0;

	psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;

	psecuritypriv->ndisauthtype = Ndis802_11AuthModeOpen;
	psecuritypriv->ndisencryptstatus = Ndis802_11WEPDisabled;
#ifdef CONFIG_CONCURRENT_MODE
	psecuritypriv->dot118021x_bmc_cam_id = INVALID_SEC_MAC_CAM_ID;
#endif

	/* pwrctrl_priv */

	/* registry_priv */
	rtw_init_registrypriv_dev_network(padapter);
	rtw_update_registrypriv_dev_network(padapter);

	pregistrypriv->wireless_mode &= rtw_hw_get_wireless_mode(adapter_to_dvobj(padapter));
	pregistrypriv->band_type &= rtw_hw_get_band_cap(adapter_to_dvobj(padapter));

	/* misc. */
	padapter->bLinkInfoDump = 0;
	padapter->bNotifyChannelChange = _FALSE;
#ifdef CONFIG_P2P
	padapter->bShowGetP2PState = 1;
#endif

	/* for debug purpose */
	padapter->fix_rate = NO_FIX_RATE;
	padapter->data_fb = 0;
	padapter->fix_bw = NO_FIX_BW;
	padapter->power_offset = 0;
	padapter->rsvd_page_offset = 0;
	padapter->rsvd_page_num = 0;
#ifdef CONFIG_AP_MODE
	padapter->bmc_tx_rate = pregistrypriv->bmc_tx_rate;
	#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	padapter->b2u_flags_ap_src = pregistrypriv->ap_src_b2u_flags;
	padapter->b2u_flags_ap_fwd = pregistrypriv->ap_fwd_b2u_flags;
	#endif
#endif
	padapter->driver_tx_bw_mode = pregistrypriv->tx_bw_mode;

	padapter->driver_ampdu_spacing = 0xFF;
	padapter->driver_rx_ampdu_factor =  0xFF;
	padapter->driver_rx_ampdu_spacing = 0xFF;
	padapter->fix_rx_ampdu_accept = RX_AMPDU_ACCEPT_INVALID;
	padapter->fix_rx_ampdu_size = RX_AMPDU_SIZE_INVALID;
#ifdef CONFIG_TX_AMSDU
	padapter->tx_amsdu = 2;
	padapter->tx_amsdu_rate = 10;
#endif
	if (pregistrypriv->adaptivity_idle_probability == 1) {
#ifdef CONFIG_TX_AMSDU
		padapter->tx_amsdu = 0;
		padapter->tx_amsdu_rate = 0;
#endif
		padapter->dis_turboedca = 1;
	}

#ifdef DBG_RX_COUNTER_DUMP
	padapter->dump_rx_cnt_mode = 0;
	padapter->drv_rx_cnt_ok = 0;
	padapter->drv_rx_cnt_crcerror = 0;
	padapter->drv_rx_cnt_drop = 0;
#endif
#ifdef CONFIG_RTW_NAPI
	padapter->napi_state = NAPI_DISABLE;
#endif

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	ATOMIC_SET(&padapter->tbtx_tx_pause, _FALSE);
	ATOMIC_SET(&padapter->tbtx_remove_tx_pause, _FALSE);
	padapter->tbtx_capability = _TRUE;
#endif

#ifdef CONFIG_CORE_TXSC
	txsc_init(padapter);
#endif

	return ret;
}

#ifdef CONFIG_DRV_FAKE_AP
extern void rtw_fakeap_work(struct work_struct *work);
extern void rtw_fakeap_bcn_timer_hdl(void*);
#endif /* CONFIG_DRV_FAKE_AP */

struct dvobj_priv *devobj_init(void)
{
	struct dvobj_priv *pdvobj = NULL;
	struct rf_ctl_t *rfctl;

	pdvobj = (struct dvobj_priv *)rtw_zmalloc(sizeof(*pdvobj));
	if (pdvobj == NULL)
		return NULL;

	rfctl = dvobj_to_rfctl(pdvobj);

	_rtw_mutex_init(&pdvobj->hw_init_mutex);
	_rtw_mutex_init(&pdvobj->setch_mutex);
	_rtw_mutex_init(&pdvobj->setbw_mutex);

#ifdef CONFIG_RTW_CUSTOMER_STR
	_rtw_mutex_init(&pdvobj->customer_str_mutex);
	_rtw_memset(pdvobj->customer_str, 0xFF, RTW_CUSTOMER_STR_LEN);
#endif

	pdvobj->processing_dev_remove = _FALSE;

	ATOMIC_SET(&pdvobj->disable_func, 0);
	/* move to phl */
	/* rtw_macid_ctl_init(&pdvobj->macid_ctl); */

	_rtw_spinlock_init(&pdvobj->cam_ctl.lock);
	_rtw_mutex_init(&pdvobj->cam_ctl.sec_cam_access_mutex);
#if defined(RTK_129X_PLATFORM) && defined(CONFIG_PCI_HCI)
	_rtw_spinlock_init(&pdvobj->io_reg_lock);
#endif

	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	rtw_init_timer(&(pdvobj->dynamic_chk_timer), rtw_dynamic_check_timer_handlder, pdvobj);
	#endif

#ifdef CONFIG_RTW_NAPI_DYNAMIC
	pdvobj->en_napi_dynamic = 0;
#endif /* CONFIG_RTW_NAPI_DYNAMIC */

	_rtw_mutex_init(&rfctl->offch_mutex);

	pdvobj->scan_deny = _FALSE;
	rtw_load_dvobj_registry(pdvobj);

#ifdef CONFIG_DRV_FAKE_AP
	skb_queue_head_init(&pdvobj->fakeap.rxq);
	_init_workitem(&pdvobj->fakeap.work, rtw_fakeap_work, pdvobj);
	_init_timer(&pdvobj->fakeap.bcn_timer, rtw_fakeap_bcn_timer_hdl, pdvobj);
#endif /* CONFIG_DRV_FAKE_AP */

	/* wpas type default from w1.fi */
	pdvobj->wpas_type = RTW_WPAS_W1FI;

	return pdvobj;

}

void devobj_deinit(struct dvobj_priv *pdvobj)
{
	if (!pdvobj)
		return;

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
#if defined(CONFIG_IOCTL_CFG80211)
	rtw_cfg80211_dev_res_free(pdvobj);
#endif

	_rtw_mutex_free(&pdvobj->hw_init_mutex);

#ifdef CONFIG_RTW_CUSTOMER_STR
	_rtw_mutex_free(&pdvobj->customer_str_mutex);
#endif

	_rtw_mutex_free(&pdvobj->setch_mutex);
	_rtw_mutex_free(&pdvobj->setbw_mutex);
	/* move to phl */
	/* rtw_macid_ctl_deinit(&pdvobj->macid_ctl); */

	_rtw_spinlock_free(&pdvobj->cam_ctl.lock);
	_rtw_mutex_free(&pdvobj->cam_ctl.sec_cam_access_mutex);

#if defined(RTK_129X_PLATFORM) && defined(CONFIG_PCI_HCI)
	_rtw_spinlock_free(&pdvobj->io_reg_lock);
#endif

	rtw_mfree((u8 *)pdvobj, sizeof(*pdvobj));
}

inline u8 rtw_rtnl_lock_needed(struct dvobj_priv *dvobj)
{
	if (dvobj->rtnl_lock_holder && dvobj->rtnl_lock_holder == current)
		return 0;
	return 1;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26))
static inline int rtnl_is_locked(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 17))
	if (unlikely(rtnl_trylock())) {
		rtnl_unlock();
#else
	if (unlikely(down_trylock(&rtnl_sem) == 0)) {
		up(&rtnl_sem);
#endif
		return 0;
	}
	return 1;
}
#endif

inline void rtw_set_rtnl_lock_holder(struct dvobj_priv *dvobj, _thread_hdl_ thd_hdl)
{
	rtw_warn_on(!rtnl_is_locked());

	if (!thd_hdl || rtnl_is_locked())
		dvobj->rtnl_lock_holder = thd_hdl;

	if (dvobj->rtnl_lock_holder && 0)
		RTW_INFO("rtnl_lock_holder: %s:%d\n", current->comm, current->pid);
}

u8 rtw_reset_drv_sw(_adapter *padapter)
{
	u8	ret8 = _SUCCESS;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);

	/* hal_priv */
	rtw_hw_cap_init(adapter_to_dvobj(padapter));

	RTW_ENABLE_FUNC(adapter_to_dvobj(padapter), DF_RX_BIT);
	RTW_ENABLE_FUNC(adapter_to_dvobj(padapter), DF_TX_BIT);

	padapter->bLinkInfoDump = 0;

	padapter->xmitpriv.tx_pkts = 0;
	padapter->recvinfo.rx_pkts = 0;

	pmlmepriv->LinkDetectInfo.bBusyTraffic = _FALSE;

	/* pmlmepriv->LinkDetectInfo.TrafficBusyState = _FALSE; */
	pmlmepriv->LinkDetectInfo.TrafficTransitionCount = 0;
	pmlmepriv->LinkDetectInfo.LowPowerTransitionCount = 0;

	_clr_fwstate_(pmlmepriv, WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING);

#ifdef DBG_CONFIG_ERROR_DETECT
	if (is_primary_adapter(padapter))
		rtw_hal_sreset_reset_value(padapter);
#endif

	/* mlmeextpriv */
	mlmeext_set_scan_state(&padapter->mlmeextpriv, SCAN_DISABLE);

	#ifdef CONFIG_SIGNAL_STAT_PROCESS
	rtw_set_signal_stat_timer(&padapter->recvinfo);
	#endif
	return ret8;
}

static void devobj_decide_init_chplan(struct dvobj_priv *dvobj)
{
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	struct dev_cap_t *dev_cap = &GET_PHL_COM(dvobj)->dev_cap;
	const char *alpha2 = NULL; /* TODO */
	u8 chplan = dev_cap->domain;
	u8 chplan_6g = RTW_CHPLAN_6G_NULL;
	bool disable_sw_chplan = _FALSE; /* TODO */

#if CONFIG_IEEE80211_BAND_6GHZ
	chplan_6g = dev_cap->domain_6g;
#endif

	if (alpha2)
		RTW_INFO("%s alpha2:{%d,%d}\n", __func__, alpha2[0], alpha2[1]);
	RTW_INFO("%s chplan:0x%02x\n", __func__, chplan);
	RTW_INFO("%s chplan_6g:0x%02x\n", __func__, chplan_6g);
	RTW_INFO("%s disable_sw_chplan:%d\n", __func__, disable_sw_chplan);

	/*
	* treat {0xFF, 0xFF} as unspecified
	*/
	if (alpha2 && strncmp(alpha2, "\xFF\xFF", 2) == 0)
		alpha2 = NULL;

#ifdef CONFIG_FORCE_SW_CHANNEL_PLAN
	disable_sw_chplan = _FALSE;
#endif

	rtw_rfctl_decide_init_chplan(rfctl, alpha2, chplan, chplan_6g, disable_sw_chplan);
}

u8 devobj_data_init(struct dvobj_priv *dvobj)
{
	u8 ret = _FAIL;

	dev_set_drv_stopped(dvobj);/*init*/
	dev_clr_hw_start(dvobj); /* init */

	/*init data of dvobj*/
	if (rtw_rfctl_init(dvobj) == _FAIL)
		goto exit;

	devobj_decide_init_chplan(dvobj);

	rtw_edcca_mode_update(dvobj, true);
	rtw_rfctl_chset_apply_regulatory(dvobj, true);
	op_class_pref_apply_regulatory(dvobj_to_rfctl(dvobj), REG_CHANGE);

	init_channel_list(dvobj_get_primary_adapter(dvobj));

	rtw_hw_cap_init(dvobj);

	RTW_ENABLE_FUNC(dvobj, DF_RX_BIT);
	RTW_ENABLE_FUNC(dvobj, DF_TX_BIT);

	ret = _SUCCESS;

exit:
	return 	ret;
}


void devobj_data_deinit(struct dvobj_priv *dvobj)
{
}

u8 devobj_trx_resource_init(struct dvobj_priv *dvobj)
{
	u8 ret = _SUCCESS;

#ifdef CONFIG_USB_HCI
	ret = rtw_init_lite_xmit_resource(dvobj);
	if (ret == _FAIL)
		goto exit;
	ret = rtw_init_lite_recv_resource(dvobj);
	if (ret == _FAIL)
		goto exit;
#endif
	ret = rtw_init_recv_priv(dvobj);
	if (ret == _FAIL) {
		RTW_ERR("%s rtw_init_recv_priv failed\n", __func__);
		goto exit;
	}

	ret = rtw_init_cmd_priv(dvobj);
	if (ret == _FAIL) {
		RTW_ERR("%s rtw_init_cmd_priv failed\n", __func__);
		goto exit;
	}
exit:
	return 	ret;
}


void devobj_trx_resource_deinit(struct dvobj_priv *dvobj)
{
#ifdef CONFIG_USB_HCI
	rtw_free_lite_xmit_resource(dvobj);
	rtw_free_lite_recv_resource(dvobj);
#endif
	rtw_free_recv_priv(dvobj);
	rtw_free_cmd_priv(dvobj);
}


u8 rtw_init_drv_sw(_adapter *padapter)
{
	u8	ret8 = _SUCCESS;

#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI
	struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);
#endif

	ret8 = rtw_init_default_value(padapter);/*load registrypriv value*/

	if (rtw_init_mlme_priv(padapter) == _FAIL) {
		ret8 = _FAIL;
		goto exit;
	}

#ifdef CONFIG_P2P
	init_wifidirect_info(padapter, P2P_ROLE_DISABLE);
	reset_global_wifidirect_info(padapter);
#ifdef CONFIG_WFD
	if (rtw_init_wifi_display_info(padapter) == _FAIL)
		RTW_ERR("Can't init init_wifi_display_info\n");
#endif
#endif /* CONFIG_P2P */

	if (init_mlme_ext_priv(padapter) == _FAIL) {
		ret8 = _FAIL;
		goto exit;
	}

#ifdef CONFIG_TDLS
	if (rtw_init_tdls_info(padapter) == _FAIL) {
		RTW_INFO("Can't rtw_init_tdls_info\n");
		ret8 = _FAIL;
		goto exit;
	}
#endif /* CONFIG_TDLS */

#ifdef CONFIG_RTW_MESH
	rtw_mesh_cfg_init(padapter);
#endif

	if (_rtw_init_xmit_priv(&padapter->xmitpriv, padapter) == _FAIL) {
		RTW_INFO("Can't _rtw_init_xmit_priv\n");
		ret8 = _FAIL;
		goto exit;
	}
	if (rtw_init_recv_info(padapter) == _FAIL) {
		RTW_INFO("Can't rtw_init_recv_info\n");
		ret8 = _FAIL;
		goto exit;
	}

	/* add for CONFIG_IEEE80211W, none 11w also can use */
	_rtw_spinlock_init(&padapter->security_key_mutex);

	/* We don't need to memset padapter->XXX to zero, because adapter is allocated by rtw_zvmalloc(). */
	/* _rtw_memset((unsigned char *)&padapter->securitypriv, 0, sizeof (struct security_priv)); */

	if (_rtw_init_sta_priv(&padapter->stapriv) == _FAIL) {
		RTW_INFO("Can't _rtw_init_sta_priv\n");
		ret8 = _FAIL;
		goto exit;
	}

	padapter->setband = WIFI_FREQUENCY_BAND_AUTO;
	padapter->fix_rate = NO_FIX_RATE;
	padapter->power_offset = 0;
	padapter->rsvd_page_offset = 0;
	padapter->rsvd_page_num = 0;

	padapter->data_fb = 0;
	padapter->fix_rx_ampdu_accept = RX_AMPDU_ACCEPT_INVALID;
	padapter->fix_rx_ampdu_size = RX_AMPDU_SIZE_INVALID;
#ifdef DBG_RX_COUNTER_DUMP
	padapter->dump_rx_cnt_mode = 0;
	padapter->drv_rx_cnt_ok = 0;
	padapter->drv_rx_cnt_crcerror = 0;
	padapter->drv_rx_cnt_drop = 0;
#endif

	rtw_init_pwrctrl_priv(padapter);
#ifdef CONFIG_WOWLAN
	rtw_init_wow(padapter);
#endif /* CONFIG_WOWLAN */

	/* _rtw_memset((u8 *)&padapter->qospriv, 0, sizeof (struct qos_priv)); */ /* move to mlme_priv */

#ifdef CONFIG_WAPI_SUPPORT
	padapter->WapiSupport = true; /* set true temp, will revise according to Efuse or Registry value later. */
	rtw_wapi_init(padapter);
#endif

#ifdef CONFIG_BR_EXT
	_rtw_spinlock_init(&padapter->br_ext_lock);
#endif /* CONFIG_BR_EXT */

#ifdef CONFIG_RTW_80211K
	rtw_init_rm(padapter);
#endif

#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI
	_rtw_memset(pwdev_priv->pno_mac_addr, 0xFF, ETH_ALEN);
#endif

#ifdef CONFIG_STA_CMD_DISPR
	rtw_connect_req_init(padapter);
	rtw_disconnect_req_init(padapter);
#endif /* CONFIG_STA_CMD_DISPR */

#ifdef CONFIG_AP_MODE
	rtw_cmd_ap_add_del_sta_req_init(padapter);
#endif

#ifdef CONFIG_ECSA_PHL
	reset_ecsa_param(padapter);
#endif

exit:

	return ret8;

}


u8 rtw_init_link_default_value(struct _ADAPTER_LINK *padapter_link)
{
	struct link_security_priv *psecuritypriv = &padapter_link->securitypriv;
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
	u8 ret  = _SUCCESS;

#ifdef CONFIG_80211N_HT
	pmlmepriv->ampdu_priv.ampdu_enable = _FALSE;//set to disabled
#endif

	/* ToDo CONFIG_RTW_MLD: TBD */
	psecuritypriv->binstallGrpkey = _FAIL;
	psecuritypriv->dot118021XGrpKeyid = 1;

	return ret;
}

u8 init_adapter_link(_adapter *padapter) {
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;

	/* ToDo CONFIG_RTW_MLD: temporally initialize the adapter_link_num as RTW_ONE_LINK */
	padapter->adapter_link_num = RTW_ONE_LINK;
	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = (struct _ADAPTER_LINK *)rtw_zvmalloc(sizeof(*padapter_link));
		if (padapter_link == NULL)
			goto init_fail;
		padapter_link->adapter = padapter;
		padapter->adapter_link[lidx] = padapter_link;
		/* ToDo CONFIG_RTW_MLD:
		** temporally initialize the link's mac_addr as the adapter's mac_addr with offset */
		_rtw_memcpy(padapter_link->mac_addr, adapter_mac_addr(padapter), ETH_ALEN);
		padapter_link->mac_addr[ETH_ALEN - 1] += lidx;

		/* default values, e.g., wireless_mode, band_type */
		rtw_init_link_default_value(padapter_link);
		/* mlmepriv */
		rtw_init_link_mlme_priv(padapter_link);
		/* mlmeextpriv */
		rtw_init_link_mlme_ext_priv(padapter_link);
	}

	return _SUCCESS;

init_fail:
	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		if (padapter_link) {
			rtw_vmfree((u8 *)padapter_link, sizeof(*padapter_link));
			padapter_link = NULL;
		}
	}
	return _FAIL;
}

u8 rtw_adapter_link_init(struct dvobj_priv *dvobj)
{
	_adapter *padapter = NULL;
	u8 i;

	for (i = 0; i < dvobj->iface_nums; i++) {
		padapter = dvobj->padapters[i];
		if (!padapter)
			goto init_fail;
		if (init_adapter_link(padapter) != _SUCCESS)
			goto init_fail;
	}
	return _SUCCESS;

init_fail:
	rtw_adapter_link_deinit(dvobj);
	return _FAIL;
}

void rtw_adapter_link_deinit(struct dvobj_priv *dvobj)
{
	_adapter *padapter = NULL;
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;
	u8 i;
	for (i = 0; i < dvobj->iface_nums; i++) {
		padapter = dvobj->padapters[i];
		if (!padapter)
			continue;
		for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
			padapter_link = GET_LINK(padapter, lidx);
			if (padapter_link) {
				rtw_vmfree((u8 *)padapter_link, sizeof(*padapter_link));
				padapter_link = NULL;
			}
		}
	}
}

int init_link_capab_from_adapter(_adapter *padapter) {
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;

	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		if (!padapter_link)
			return _FAIL;
		if (is_supported_24g(padapter->registrypriv.band_type)) {
#ifdef CONFIG_80211N_HT
			if (padapter->registrypriv.ht_enable)
				_rtw_memcpy(&padapter_link->mlmepriv.htpriv, &padapter->mlmepriv.dev_htpriv, sizeof(struct ht_priv));
#endif
#ifdef CONFIG_80211AX_HE
			if (padapter->registrypriv.he_enable)
				_rtw_memcpy(&padapter_link->mlmepriv.hepriv, &padapter->mlmepriv.dev_hepriv, sizeof(struct he_priv));
#endif
		}

		if (is_supported_5g(padapter->registrypriv.band_type)) {
#ifdef CONFIG_80211N_HT
			if (padapter->registrypriv.ht_enable)
				_rtw_memcpy(&padapter_link->mlmepriv.htpriv, &padapter->mlmepriv.dev_htpriv, sizeof(struct ht_priv));
#endif
#ifdef CONFIG_80211AC_VHT
			if (padapter->registrypriv.vht_enable)
				_rtw_memcpy(&padapter_link->mlmepriv.vhtpriv, &padapter->mlmepriv.dev_vhtpriv, sizeof(struct vht_priv));
#endif
#ifdef CONFIG_80211AX_HE
			if (padapter->registrypriv.he_enable)
				_rtw_memcpy(&padapter_link->mlmepriv.hepriv, &padapter->mlmepriv.dev_hepriv, sizeof(struct he_priv));
#endif
		}

		if (is_supported_6g(padapter->registrypriv.band_type)) {
#ifdef CONFIG_80211AX_HE
			if (padapter->registrypriv.he_enable)
				_rtw_memcpy(&padapter_link->mlmepriv.hepriv, &padapter->mlmepriv.dev_hepriv, sizeof(struct he_priv));
#endif
			/* ToDo CONFIG_RTW_MLD: eht_priv */
		}
	}
	return _SUCCESS;
}

int rtw_init_link_capab(struct dvobj_priv *dvobj)
{
	_adapter *padapter = NULL;
	u8 i;

	for (i = 0; i < dvobj->iface_nums; i++) {
		padapter = dvobj->padapters[i];
		if (!padapter)
			return _FAIL;
		if (init_link_capab_from_adapter(padapter) != _SUCCESS)
			return _FAIL;
	}
	return _SUCCESS;
}

#ifdef CONFIG_WOWLAN
void rtw_cancel_dynamic_chk_timer(_adapter *padapter)
{
	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	_cancel_timer_ex(&adapter_to_dvobj(padapter)->dynamic_chk_timer);
	#endif
}
#endif

void rtw_cancel_all_timer(_adapter *padapter)
{

	/*_cancel_timer_ex(&padapter->mlmepriv.assoc_timer);*/
	cancel_assoc_timer(&padapter->mlmepriv);

#ifdef CONFIG_DFS_MASTER
	_cancel_timer_ex(&adapter_to_rfctl(padapter)->radar_detect_timer);
#endif

	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	_cancel_timer_ex(&adapter_to_dvobj(padapter)->dynamic_chk_timer);
	#endif

#ifdef CONFIG_RTW_SW_LED
	/* cancel sw led timer */
	rtw_hal_sw_led_deinit(padapter);
#endif

#ifdef CONFIG_TX_AMSDU
	_cancel_timer_ex(&padapter->xmitpriv.amsdu_bk_timer);
	_cancel_timer_ex(&padapter->xmitpriv.amsdu_be_timer);
	_cancel_timer_ex(&padapter->xmitpriv.amsdu_vo_timer);
	_cancel_timer_ex(&padapter->xmitpriv.amsdu_vi_timer);
#endif

#ifdef CONFIG_SET_SCAN_DENY_TIMER
	_cancel_timer_ex(&padapter->mlmepriv.set_scan_deny_timer);
	rtw_clear_scan_deny(padapter);
#endif
#ifdef CONFIG_SIGNAL_STAT_PROCESS
	_cancel_timer_ex(&padapter->recvinfo.signal_stat_timer);
#endif

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	_cancel_timer_ex(&padapter->mlmeextpriv.tbtx_xmit_timer);
	_cancel_timer_ex(&padapter->mlmeextpriv.tbtx_token_dispatch_timer);
#endif

#ifdef CONFIG_PLATFORM_FS_MX61
	msleep(50);
#endif
}

u8 rtw_free_drv_sw(_adapter *padapter)
{

#ifdef CONFIG_WAPI_SUPPORT
	rtw_wapi_free(padapter);
#endif

	/* we can call rtw_p2p_enable here, but: */
	/* 1. rtw_p2p_enable may have IO operation */
	/* 2. rtw_p2p_enable is bundled with wext interface */
	#ifdef CONFIG_P2P
	{
		struct wifidirect_info *pwdinfo = &padapter->wdinfo;
		if (!rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DISABLE)) {
			rtw_p2p_set_role(pwdinfo, P2P_ROLE_DISABLE);
		}
	}
	#endif
	/* add for CONFIG_IEEE80211W, none 11w also can use */
	_rtw_spinlock_free(&padapter->security_key_mutex);

#ifdef CONFIG_BR_EXT
	_rtw_spinlock_free(&padapter->br_ext_lock);
#endif /* CONFIG_BR_EXT */

	free_mlme_ext_priv(&padapter->mlmeextpriv);

#ifdef CONFIG_TDLS
	/* rtw_free_tdls_info(&padapter->tdlsinfo); */
#endif /* CONFIG_TDLS */

#ifdef CONFIG_RTW_80211K
	rtw_free_rm_priv(padapter);
#endif

	rtw_free_mlme_priv(&padapter->mlmepriv);
#ifdef CONFIG_STA_CMD_DISPR
	rtw_connect_req_free(padapter);
	rtw_disconnect_req_free(padapter);
#endif /* CONFIG_STA_CMD_DISPR */

#ifdef CONFIG_AP_MODE
	rtw_cmd_ap_add_del_sta_req_free(padapter);
#endif

	if (is_primary_adapter(padapter))
		rtw_rfctl_deinit(adapter_to_dvobj(padapter));

	/* free_io_queue(padapter); */

	_rtw_free_xmit_priv(&padapter->xmitpriv);

	_rtw_free_sta_priv(&padapter->stapriv); /* will free bcmc_stainfo here */

	rtw_free_pwrctrl_priv(padapter);
#ifdef CONFIG_WOWLAN
	rtw_free_wow(padapter);
#endif /* CONFIG_WOWLAN */

	/* rtw_mfree((void *)padapter, sizeof (padapter)); */

	return _SUCCESS;

}
void rtw_drv_stop_prim_iface(_adapter *adapter)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(adapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		rtw_disassoc_cmd(adapter, 0, RTW_CMDF_DIRECTLY|RTW_CMDF_WAIT_ACK);

#ifdef CONFIG_AP_MODE
	if (MLME_IS_AP(adapter) || MLME_IS_MESH(adapter)) {
		free_mlme_ap_info(adapter);
		#ifdef CONFIG_HOSTAPD_MLME
		hostapd_mode_unload(adapter);
		#endif
	}
#endif

	RTW_INFO("==> "FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));

	if (adapter->netif_up == _TRUE) {
		#ifdef CONFIG_XMIT_ACK
		if (adapter->xmitpriv.ack_tx)
			rtw_ack_tx_done(&adapter->xmitpriv, RTW_SCTX_DONE_DRV_STOP);
		#endif
		rtw_hw_iface_deinit(adapter);
		if (!pwrctl->bInSuspend)
			adapter->netif_up = _FALSE;
	}
	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	rtw_stop_drv_threads(adapter);

	if (ATOMIC_READ(&(pcmdpriv->cmdthd_running)) == _TRUE) {
		RTW_ERR("cmd_thread not stop !!\n");
		rtw_warn_on(1);
	}
	#endif

	rtw_cancel_all_timer(adapter);
	RTW_INFO("<== "FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));

}

#ifdef CONFIG_CONCURRENT_MODE
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
static const struct net_device_ops rtw_netdev_vir_if_ops = {
	.ndo_init = rtw_ndev_init,
	.ndo_uninit = rtw_ndev_uninit,
	.ndo_open = netdev_open,
	.ndo_stop = netdev_close,
	.ndo_start_xmit = rtw_xmit_entry,
	.ndo_set_mac_address = rtw_net_set_mac_address,
	.ndo_get_stats = rtw_net_get_stats,
	.ndo_do_ioctl = rtw_ioctl,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0))
	.ndo_siocdevprivate = rtw_siocdevprivate,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	.ndo_select_queue	= rtw_select_queue,
#endif
};
#endif

static void rtw_hook_vir_if_ops(struct net_device *ndev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	ndev->netdev_ops = &rtw_netdev_vir_if_ops;
#else
	ndev->init = rtw_ndev_init;
	ndev->uninit = rtw_ndev_uninit;
	ndev->open = netdev_open;
	ndev->stop = netdev_close;

	ndev->set_mac_address = rtw_net_set_mac_address;
#endif
}
static _adapter *rtw_drv_add_vir_if(struct dvobj_priv *dvobj)
{
	int res = _FAIL;
	_adapter *padapter = NULL;
	_adapter *primary_padapter = dvobj_get_primary_adapter(dvobj);
	u8 mac[ETH_ALEN];
#ifdef CONFIG_MI_UNIQUE_MACADDR_BIT
	u32 mi_unique_macaddr_bit = 0;
	bool is_uniq_macaddr = _FALSE;
	u8 i;
#endif

	/****** init adapter ******/
	padapter = (_adapter *)rtw_zvmalloc(sizeof(*padapter));
	if (padapter == NULL)
		goto exit;

	_rtw_memcpy(padapter, primary_padapter, sizeof(_adapter));
#ifdef CONFIG_STA_CMD_DISPR
	/* Reset not proper variables value which copied from primary adapter */
	/* Check rtw_connect_req_init() & rtw_disconnect_req_init() */
	padapter->connect_state = CONNECT_ST_NOT_READY;
#endif

	if (rtw_load_registry(padapter) != _SUCCESS)
		goto free_adapter;

	padapter->netif_up = _FALSE;
	padapter->dir_dev = NULL;
	padapter->dir_odm = NULL;

	/*set adapter_type/iface type*/
	padapter->isprimary = _FALSE;
	padapter->adapter_type = VIRTUAL_ADAPTER;


	/****** hook vir if into dvobj ******/
	padapter->iface_id = dvobj->iface_nums;
	dvobj->padapters[dvobj->iface_nums++] = padapter;

	/*init drv data*/
	if (rtw_init_drv_sw(padapter) != _SUCCESS)
		goto free_drv_sw;


	/*get mac address from primary_padapter*/
	_rtw_memcpy(mac, adapter_mac_addr(primary_padapter), ETH_ALEN);

#ifdef CONFIG_MI_UNIQUE_MACADDR_BIT
	mi_unique_macaddr_bit = BIT(CONFIG_MI_UNIQUE_MACADDR_BIT) >> 24;
	/* Find out CONFIG_MI_UNIQUE_MACADDR_BIT in which nic specific byte */
	for(i = 3; i < 6; i++) {
		if(((mi_unique_macaddr_bit >> 8) == 0) &&
			((mac[i] & (u8)mi_unique_macaddr_bit) == 0)) {
			is_uniq_macaddr = _TRUE;

			RTW_INFO("%s() "MAC_FMT" : BIT%u is zero\n",
				__func__, MAC_ARG(mac), CONFIG_MI_UNIQUE_MACADDR_BIT);
			break;
		}
		mi_unique_macaddr_bit >>= 8;
	}

	if(is_uniq_macaddr) {
		/* IFACE_ID1/IFACE_ID3 : set locally administered bit */
		if(padapter->iface_id & BIT(0))
			mac[0] |= BIT(1);
		/* IFACE_ID2/IFACE_ID3 : set bit(CONFIG_MI_UNIQUE_MACADDR_BIT) */
		if(padapter->iface_id >> 1)
			mac[i] |= (u8)mi_unique_macaddr_bit;
	} else
#endif
	{
		/*
		* If the BIT1 is 0, the address is universally administered.
		* If it is 1, the address is locally administered
		*/
		mac[0] |= BIT(1);
		if (padapter->iface_id > IFACE_ID1)
			mac[0] ^= ((padapter->iface_id) << 2);
	}

	_rtw_memcpy(adapter_mac_addr(padapter), mac, ETH_ALEN);

	RTW_INFO("%s if%d mac_addr : "MAC_FMT"\n", __func__, padapter->iface_id + 1, MAC_ARG(adapter_mac_addr(padapter)));

	rtw_led_set_ctl_en_mask_virtual(padapter);
	rtw_led_set_iface_en(padapter, 1);

	res = _SUCCESS;

free_drv_sw:
	if (res != _SUCCESS && padapter)
		rtw_free_drv_sw(padapter);
free_adapter:
	if (res != _SUCCESS && padapter) {
		rtw_vmfree((u8 *)padapter, sizeof(*padapter));
		padapter = NULL;
	}
exit:
	return padapter;
}
u8 rtw_drv_add_vir_ifaces(struct dvobj_priv *dvobj)
{
	u8 i;
	u8 rst = _FAIL;

	if (dvobj->virtual_iface_num > (CONFIG_IFACE_NUMBER - 1))
		dvobj->virtual_iface_num = (CONFIG_IFACE_NUMBER - 1);

	for (i = 0; i < dvobj->virtual_iface_num; i++) {
		if (rtw_drv_add_vir_if(dvobj) == NULL) {
			RTW_ERR("rtw_drv_add_vir_if failed! (%d)\n", i);
			goto _exit;
		}
	}
	rst = _SUCCESS;
_exit:
	return rst;
}

static void rtw_drv_stop_vir_if(_adapter *padapter)
{
	struct net_device *pnetdev = NULL;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);

	if (padapter == NULL)
		return;
	RTW_INFO(FUNC_ADPT_FMT" enter\n", FUNC_ADPT_ARG(padapter));

	pnetdev = padapter->pnetdev;

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		rtw_disassoc_cmd(padapter, 0, RTW_CMDF_DIRECTLY|RTW_CMDF_WAIT_ACK);

#ifdef CONFIG_AP_MODE
	if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter)) {
		free_mlme_ap_info(padapter);
		#ifdef CONFIG_HOSTAPD_MLME
		hostapd_mode_unload(padapter);
		#endif
	}
#endif

	if (padapter->netif_up == _TRUE) {
		#ifdef CONFIG_XMIT_ACK
		if (padapter->xmitpriv.ack_tx)
			rtw_ack_tx_done(&padapter->xmitpriv, RTW_SCTX_DONE_DRV_STOP);
		#endif
		rtw_hw_iface_deinit(padapter);
		if (!pwrctl->bInSuspend)
			padapter->netif_up = _FALSE;
	}
	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	rtw_stop_drv_threads(padapter);
	#endif
	/* cancel timer after thread stop */
	rtw_cancel_all_timer(padapter);
}
void rtw_drv_stop_vir_ifaces(struct dvobj_priv *dvobj)
{
	int i;

	for (i = VIF_START_ID; i < dvobj->iface_nums; i++)
		rtw_drv_stop_vir_if(dvobj->padapters[i]);
}

static void rtw_drv_free_vir_if(_adapter *padapter)
{
	if (padapter == NULL)
		return;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));
	rtw_free_drv_sw(padapter);

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndev_free(padapter);

	rtw_vmfree((u8 *)padapter, sizeof(_adapter));
}
void rtw_drv_free_vir_ifaces(struct dvobj_priv *dvobj)
{
	int i;

	for (i = VIF_START_ID; i < dvobj->iface_nums; i++)
		rtw_drv_free_vir_if(dvobj->padapters[i]);
}


#endif /*end of CONFIG_CONCURRENT_MODE*/

/* IPv4, IPv6 IP addr notifier */
static int rtw_inetaddr_notifier_call(struct notifier_block *nb,
				      unsigned long action, void *data)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *)data;
	struct net_device *ndev;
	struct mlme_ext_priv *pmlmeext = NULL;
	struct mlme_ext_info *pmlmeinfo = NULL;
	_adapter *adapter = NULL;

	if (!ifa || !ifa->ifa_dev || !ifa->ifa_dev->dev)
		return NOTIFY_DONE;

	ndev = ifa->ifa_dev->dev;

	if (!is_rtw_ndev(ndev))
		return NOTIFY_DONE;

	adapter = (_adapter *)rtw_netdev_priv(ifa->ifa_dev->dev);

	if (adapter == NULL)
		return NOTIFY_DONE;

	pmlmeext = &adapter->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;

	switch (action) {
	case NETDEV_UP:
		_rtw_memcpy(pmlmeinfo->ip_addr, &ifa->ifa_address,
					RTW_IP_ADDR_LEN);
		RTW_DBG("%s[%s]: up IP: %pI4\n", __func__,
					ifa->ifa_label, pmlmeinfo->ip_addr);
	break;
	case NETDEV_DOWN:
		_rtw_memset(pmlmeinfo->ip_addr, 0, RTW_IP_ADDR_LEN);
		RTW_DBG("%s[%s]: down IP: %pI4\n", __func__,
					ifa->ifa_label, pmlmeinfo->ip_addr);
	break;
	default:
		RTW_DBG("%s: default action\n", __func__);
	break;
	}
	return NOTIFY_DONE;
}

#ifdef CONFIG_IPV6
static int rtw_inet6addr_notifier_call(struct notifier_block *nb,
				       unsigned long action, void *data)
{
	struct inet6_ifaddr *inet6_ifa = data;
	struct net_device *ndev;
	struct pwrctrl_priv *pwrctl = NULL;
	struct mlme_ext_priv *pmlmeext = NULL;
	struct mlme_ext_info *pmlmeinfo = NULL;
	_adapter *adapter = NULL;

	if (!inet6_ifa || !inet6_ifa->idev || !inet6_ifa->idev->dev)
		return NOTIFY_DONE;

	ndev = inet6_ifa->idev->dev;

	if (!is_rtw_ndev(ndev))
		return NOTIFY_DONE;

	adapter = (_adapter *)rtw_netdev_priv(inet6_ifa->idev->dev);

	if (adapter == NULL)
		return NOTIFY_DONE;

	pmlmeext =  &adapter->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;
	pwrctl = adapter_to_pwrctl(adapter);

	pmlmeext = &adapter->mlmeextpriv;
	pmlmeinfo = &pmlmeext->mlmext_info;

	switch (action) {
	case NETDEV_UP:
#ifdef CONFIG_WOWLAN
		pwrctl->wowlan_ns_offload_en = _TRUE;
#endif
		_rtw_memcpy(pmlmeinfo->ip6_addr, &inet6_ifa->addr,
					RTW_IPv6_ADDR_LEN);
		RTW_DBG("%s: up IPv6 addrs: %pI6\n", __func__,
					pmlmeinfo->ip6_addr);
		break;
	case NETDEV_DOWN:
#ifdef CONFIG_WOWLAN
		pwrctl->wowlan_ns_offload_en = _FALSE;
#endif
		_rtw_memset(pmlmeinfo->ip6_addr, 0, RTW_IPv6_ADDR_LEN);
		RTW_DBG("%s: down IPv6 addrs: %pI6\n", __func__,
					pmlmeinfo->ip6_addr);
		break;
	default:
		RTW_DBG("%s: default action\n", __func__);
		break;
	}
	return NOTIFY_DONE;
}
#endif

static struct notifier_block rtw_inetaddr_notifier = {
	.notifier_call = rtw_inetaddr_notifier_call
};

#ifdef CONFIG_IPV6
static struct notifier_block rtw_inet6addr_notifier = {
	.notifier_call = rtw_inet6addr_notifier_call
};
#endif

void rtw_inetaddr_notifier_register(void)
{
	RTW_INFO("%s\n", __func__);
	register_inetaddr_notifier(&rtw_inetaddr_notifier);
#ifdef CONFIG_IPV6
	register_inet6addr_notifier(&rtw_inet6addr_notifier);
#endif
}

void rtw_inetaddr_notifier_unregister(void)
{
	RTW_INFO("%s\n", __func__);
	unregister_inetaddr_notifier(&rtw_inetaddr_notifier);
#ifdef CONFIG_IPV6
	unregister_inet6addr_notifier(&rtw_inet6addr_notifier);
#endif
}

int rtw_os_ndevs_register(struct dvobj_priv *dvobj)
{
	int i, status = _SUCCESS;
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);
	_adapter *adapter;

#if defined(CONFIG_IOCTL_CFG80211)
	if (rtw_cfg80211_dev_res_register(dvobj) != _SUCCESS) {
		rtw_warn_on(1);
		return _FAIL;
	}
#endif

	for (i = 0; i < dvobj->iface_nums; i++) {

		if (i >= CONFIG_IFACE_NUMBER) {
			RTW_ERR("%s %d >= CONFIG_IFACE_NUMBER(%d)\n", __func__, i, CONFIG_IFACE_NUMBER);
			rtw_warn_on(1);
			continue;
		}

		adapter = dvobj->padapters[i];
		if (adapter) {
			char *name;

			#ifdef CONFIG_RTW_DYNAMIC_NDEV
			if (!is_primary_adapter(adapter) &&
				(i >= CONFIG_RTW_STATIC_NDEV_NUM))
				break;
			#endif

			if (adapter->iface_id == IFACE_ID0)
				name = regsty->ifname;
			else if (adapter->iface_id == IFACE_ID1)
				name = regsty->if2name;
#if defined(CONFIG_PLATFORM_ANDROID) && (CONFIG_IFACE_NUMBER > 2)
			else if (adapter->iface_id == IFACE_ID2)
				name = regsty->if3name;
#endif
			else
				name = "wlan%d";

			status = rtw_os_ndev_register(adapter, name);

			if (status != _SUCCESS) {
				rtw_warn_on(1);
				break;
			}
		}
	}

	if (status != _SUCCESS) {
		for (; i >= 0; i--) {
			adapter = dvobj->padapters[i];
			if (adapter)
				rtw_os_ndev_unregister(adapter);
		}
	}

#if defined(CONFIG_IOCTL_CFG80211)
	if (status != _SUCCESS)
		rtw_cfg80211_dev_res_unregister(dvobj);
#endif
	return status;
}

void rtw_os_ndevs_unregister(struct dvobj_priv *dvobj)
{
	int i;
	_adapter *adapter = NULL;

	for (i = 0; i < dvobj->iface_nums; i++) {
		adapter = dvobj->padapters[i];

		if (adapter == NULL)
			continue;

		rtw_os_ndev_unregister(adapter);
	}

#if defined(CONFIG_IOCTL_CFG80211)
	rtw_cfg80211_dev_res_unregister(dvobj);
#endif
}

/**
 * rtw_os_ndevs_init - Allocate and register OS layer net devices and relating structures for @dvobj
 * @dvobj: the dvobj on which this function applies
 *
 * Returns:
 * _SUCCESS or _FAIL
 */
int rtw_os_ndevs_init(struct dvobj_priv *dvobj)
{
	int ret = _FAIL;

	if (rtw_os_ndevs_alloc(dvobj) != _SUCCESS)
		goto exit;

	if (rtw_os_ndevs_register(dvobj) != _SUCCESS)
		goto os_ndevs_free;

	ret = _SUCCESS;

os_ndevs_free:
	if (ret != _SUCCESS)
		rtw_os_ndevs_free(dvobj);
exit:
	return ret;
}

/**
 * rtw_os_ndevs_deinit - Unregister and free OS layer net devices and relating structures for @dvobj
 * @dvobj: the dvobj on which this function applies
 */
void rtw_os_ndevs_deinit(struct dvobj_priv *dvobj)
{
	rtw_os_ndevs_unregister(dvobj);
	rtw_os_ndevs_free(dvobj);
}

#ifdef CONFIG_BR_EXT
void netdev_br_init(struct net_device *netdev)
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(netdev);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35))
	rcu_read_lock();
#endif

	/* if(check_fwstate(pmlmepriv, WIFI_STATION_STATE|WIFI_ADHOC_STATE) == _TRUE) */
	{
		/* struct net_bridge	*br = netdev->br_port->br; */ /* ->dev->dev_addr; */
		#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35))
		if (netdev->br_port)
		#else
		if (rcu_dereference(adapter->pnetdev->rx_handler_data))
		#endif
		{
			struct net_device *br_netdev;

			br_netdev = rtw_get_bridge_ndev_by_name(CONFIG_BR_EXT_BRNAME);
			if (br_netdev) {
				_rtw_memcpy(adapter->br_mac, br_netdev->dev_addr, ETH_ALEN);
				dev_put(br_netdev);
				RTW_INFO(FUNC_NDEV_FMT" bind bridge dev "NDEV_FMT"("MAC_FMT")\n"
					, FUNC_NDEV_ARG(netdev), NDEV_ARG(br_netdev), MAC_ARG(br_netdev->dev_addr));
			} else {
				RTW_INFO(FUNC_NDEV_FMT" can't get bridge dev by name \"%s\"\n"
					, FUNC_NDEV_ARG(netdev), CONFIG_BR_EXT_BRNAME);
			}
		}

		adapter->ethBrExtInfo.addPPPoETag = 1;
	}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35))
	rcu_read_unlock();
#endif
}
#endif /* CONFIG_BR_EXT */

#if 0
/*FPGA_test*/
static int _drv_enable_trx(struct dvobj_priv *d)
{
	struct _ADAPTER *adapter;
	u32 status;


	adapter = dvobj_get_primary_adapter(d);
	if (adapter->netif_up == _FALSE) {
		status = rtw_mi_start_drv_threads(adapter);

		if (status == _FAIL) {
			RTW_ERR("%s: Start threads Failed!\n", __FUNCTION__);
			return -1;
		}
	}


	return 0;
}
#endif

static int _netdev_open(struct net_device *pnetdev)
{
	uint status;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	RTW_INFO(FUNC_NDEV_FMT" start\n", FUNC_NDEV_ARG(pnetdev));

#ifdef DIRTY_FOR_WORK
	if(pnetdev->priv_flags & IFF_DONT_BRIDGE)
	{
		RTW_INFO("Unable to be bridged !! Unlock for this iface !!\n");
		pnetdev->priv_flags &= ~(IFF_DONT_BRIDGE);
	}
#endif

	if (!dev_is_hw_start(dvobj)) {
		dev_clr_surprise_removed(dvobj);
		dev_clr_drv_stopped(dvobj);
		RTW_ENABLE_FUNC(dvobj, DF_RX_BIT);
		RTW_ENABLE_FUNC(dvobj, DF_TX_BIT);
		status = rtw_hw_start(dvobj);
		if (status == _FAIL)
			goto netdev_open_error;
		rtw_led_control(padapter, LED_CTL_NO_LINK);

		#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
		if (0){
			_set_timer(&dvobj->dynamic_chk_timer, 2000);
		}
		#endif
		#if 0 /*CONFIG_CORE_THREAD*/
		_drv_enable_trx(dvobj);/*FPGA_test*/
		#endif
	}

	#ifdef CONFIG_RTW_NAPI
	if(padapter->napi_state == NAPI_DISABLE) {
		napi_enable(&padapter->napi);
		padapter->napi_state = NAPI_ENABLE;
	}
	#endif

	if (padapter->netif_up == _FALSE) {
		if (rtw_hw_iface_init(padapter) == _FAIL) {
			rtw_warn_on(1);
			goto netdev_open_error;
		}

		/* rtw_netif_carrier_on(pnetdev); */ /* call this func when rtw_joinbss_event_callback return success */
		rtw_netif_wake_queue(pnetdev);

		#ifdef CONFIG_BR_EXT
		if (is_primary_adapter(padapter))
			netdev_br_init(pnetdev);
		#endif /* CONFIG_BR_EXT */

		padapter->netif_up = _TRUE;
	}

	#ifdef CONFIG_SIGNAL_STAT_PROCESS
	rtw_set_signal_stat_timer(&padapter->recvinfo);
	#endif

	RTW_INFO(FUNC_NDEV_FMT" Success (netif_up=%d)\n", FUNC_NDEV_ARG(pnetdev), padapter->netif_up);
	return 0;

netdev_open_error:
	padapter->netif_up = _FALSE;

	#ifdef CONFIG_RTW_NAPI
	if(padapter->napi_state == NAPI_ENABLE) {
		napi_disable(&padapter->napi);
		padapter->napi_state = NAPI_DISABLE;
	}
	#endif

	rtw_netif_carrier_off(pnetdev);
	rtw_netif_stop_queue(pnetdev);

	RTW_ERR(FUNC_NDEV_FMT" Failed!! (netif_up=%d)\n", FUNC_NDEV_ARG(pnetdev), padapter->netif_up);

	return -1;

}

int netdev_open(struct net_device *pnetdev)
{
	int ret = _FALSE;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);

	if (pwrctrlpriv->bInSuspend == _TRUE) {
		RTW_INFO(" [WARN] "ADPT_FMT" %s  failed, bInSuspend=%d\n", ADPT_ARG(padapter), __func__, pwrctrlpriv->bInSuspend);
		return 0;
	}

	RTW_INFO(FUNC_NDEV_FMT" , netif_up=%d\n", FUNC_NDEV_ARG(pnetdev), padapter->netif_up);
	/*rtw_dump_stack();*/
	_rtw_mutex_lock_interruptible(&(adapter_to_dvobj(padapter)->hw_init_mutex));
	ret = _netdev_open(pnetdev);
	_rtw_mutex_unlock(&(adapter_to_dvobj(padapter)->hw_init_mutex));


#ifdef CONFIG_AUTO_AP_MODE
	if (padapter->iface_id == IFACE_ID2)
		rtw_start_auto_ap(padapter);
#endif

	return ret;
}

int _pm_netdev_open(_adapter *padapter)
{
	uint status;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct net_device *pnetdev = padapter->pnetdev;

	RTW_INFO(FUNC_NDEV_FMT" start\n", FUNC_NDEV_ARG(pnetdev));

	if (!rtw_hw_is_init_completed(dvobj)) { // ips
		dev_clr_surprise_removed(dvobj);
		status = rtw_hw_start(dvobj);
		if (status == _FAIL)
			goto netdev_open_error;
		rtw_led_control(padapter, LED_CTL_NO_LINK);
		#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
		_set_timer(&dvobj->dynamic_chk_timer, 2000);
		#endif

	#if 0 /*ndef CONFIG_IPS_CHECK_IN_WD*/
		rtw_set_pwr_state_check_timer(pwrctrlpriv);
	#endif /*CONFIG_IPS_CHECK_IN_WD*/
	}

	/*if (padapter->netif_up == _FALSE) */
	{
		rtw_hw_iface_init(padapter);

		padapter->netif_up = _TRUE;
	}

	RTW_INFO(FUNC_NDEV_FMT" Success (netif_up=%d)\n", FUNC_NDEV_ARG(pnetdev), padapter->netif_up);
	return 0;

netdev_open_error:
	padapter->netif_up = _FALSE;

	rtw_netif_carrier_off(pnetdev);
	rtw_netif_stop_queue(pnetdev);

	RTW_ERR(FUNC_NDEV_FMT" Failed!! (netif_up=%d)\n", FUNC_NDEV_ARG(pnetdev), padapter->netif_up);

	return -1;

}
int _mi_pm_netdev_open(struct net_device *pnetdev)
{
	int i;
	int status = 0;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface->netif_up) {
			status = _pm_netdev_open(iface);
			if (status == -1) {
				RTW_ERR("%s failled\n", __func__);
				break;
			}
		}
	}

	return status;
}

int pm_netdev_open(struct net_device *pnetdev, u8 bnormal)
{
	int status = 0;

	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);

	if (_TRUE == bnormal) {
		_rtw_mutex_lock_interruptible(&(adapter_to_dvobj(padapter)->hw_init_mutex));
		status = _mi_pm_netdev_open(pnetdev);

		_rtw_mutex_unlock(&(adapter_to_dvobj(padapter)->hw_init_mutex));
	}

	return status;
}

static int netdev_close(struct net_device *pnetdev)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(pnetdev);
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	RTW_INFO(FUNC_NDEV_FMT" , netif_up=%d\n", FUNC_NDEV_ARG(pnetdev), padapter->netif_up);

	pmlmepriv->LinkDetectInfo.bBusyTraffic = _FALSE;

	rtw_scan_abort(padapter, 0); /* stop scanning process before wifi is going to down */
#ifdef CONFIG_IOCTL_CFG80211
	rtw_cfg80211_wait_scan_req_empty(padapter, 200);
	/* padapter->rtw_wdev->iftype = NL80211_IFTYPE_MONITOR; */ /* set this at the end */
#endif /* CONFIG_IOCTL_CFG80211 */

	{
		RTW_INFO("netif_up=%d, hw_init_completed=%s\n",
			padapter->netif_up,
			rtw_hw_is_init_completed(dvobj) ? "_TRUE" : "_FALSE");

		/* s1. */
		if (pnetdev)
			rtw_netif_stop_queue(pnetdev);

		/* s2. */
		if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) {
			rtw_disassoc_cmd(padapter, 500, RTW_CMDF_WAIT_ACK);
			/* s2-2*/
			if (1
#ifdef CONFIG_STA_CMD_DISPR
			    && (MLME_IS_STA(padapter) == _FALSE)
#endif /* CONFIG_STA_CMD_DISPR */
			   )
				rtw_free_assoc_resources_cmd(padapter, _TRUE, RTW_CMDF_WAIT_ACK);
			/* s2-3.  indicate disconnect to os */
			rtw_indicate_disconnect(padapter, 0, _FALSE);
			/* s2-4. */
			rtw_free_network_queue(padapter, _TRUE);
			rtw_free_mld_network_queue(padapter, _TRUE);

			pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_SYSTEM_DUE_TO_NET_DEVICE_DOWN;
			pmlmeinfo->wifi_reason_code = WLAN_REASON_DEAUTH_LEAVING;
		}

#ifdef CONFIG_STA_CMD_DISPR
		rtw_connect_abort_wait(padapter);
		rtw_disconnect_abort_wait(padapter);
#endif /* CONFIG_STA_CMD_DISPR */
	}

#ifdef CONFIG_BR_EXT
	/* if (OPMODE & (WIFI_STATION_STATE | WIFI_ADHOC_STATE)) */
	{
		/* void nat25_db_cleanup(_adapter *priv); */
		nat25_db_cleanup(padapter);
	}
#endif /* CONFIG_BR_EXT */

#ifdef CONFIG_P2P
	if (!rtw_p2p_chk_role(&padapter->wdinfo, P2P_ROLE_DISABLE))
		rtw_p2p_enable(padapter, P2P_ROLE_DISABLE);
#endif /* CONFIG_P2P */

#ifdef CONFIG_WAPI_SUPPORT
	rtw_wapi_disable_tx(padapter);
#endif

#ifdef CONFIG_RTW_NAPI
	if (padapter->napi_state == NAPI_ENABLE) {
		napi_disable(&padapter->napi);
		padapter->napi_state = NAPI_DISABLE;
	}
#endif /* CONFIG_RTW_NAPI */

	rtw_hw_iface_deinit(padapter);
	padapter->netif_up = _FALSE;

	RTW_INFO("-871x_drv - drv_close, netif_up=%d\n", padapter->netif_up);

	return 0;

}

int pm_netdev_close(struct net_device *pnetdev, u8 bnormal)
{
	int status = 0;

	status = netdev_close(pnetdev);

	return status;
}

void rtw_ndev_destructor(struct net_device *ndev)
{
	RTW_INFO(FUNC_NDEV_FMT"\n", FUNC_NDEV_ARG(ndev));

#ifdef CONFIG_IOCTL_CFG80211
	if (ndev->ieee80211_ptr)
		rtw_mfree((u8 *)ndev->ieee80211_ptr, sizeof(struct wireless_dev));
#endif
	free_netdev(ndev);
}

#ifdef CONFIG_ARP_KEEP_ALIVE_GW
struct route_info {
	struct in_addr dst_addr;
	struct in_addr src_addr;
	struct in_addr gateway;
	unsigned int dev_index;
};

static void parse_routes(struct nlmsghdr *nl_hdr, struct route_info *rt_info)
{
	struct rtmsg *rt_msg;
	struct rtattr *rt_attr;
	int rt_len;

	rt_msg = (struct rtmsg *) NLMSG_DATA(nl_hdr);
	if ((rt_msg->rtm_family != AF_INET) || (rt_msg->rtm_table != RT_TABLE_MAIN))
		return;

	rt_attr = (struct rtattr *) RTM_RTA(rt_msg);
	rt_len = RTM_PAYLOAD(nl_hdr);

	for (; RTA_OK(rt_attr, rt_len); rt_attr = RTA_NEXT(rt_attr, rt_len)) {
		switch (rt_attr->rta_type) {
		case RTA_OIF:
			rt_info->dev_index = *(int *) RTA_DATA(rt_attr);
			break;
		case RTA_GATEWAY:
			rt_info->gateway.s_addr = *(u_int *) RTA_DATA(rt_attr);
			break;
		case RTA_PREFSRC:
			rt_info->src_addr.s_addr = *(u_int *) RTA_DATA(rt_attr);
			break;
		case RTA_DST:
			rt_info->dst_addr.s_addr = *(u_int *) RTA_DATA(rt_attr);
			break;
		}
	}
}

static int route_dump(u32 *gw_addr , int *gw_index)
{
	int err = 0;
	struct socket *sock;
	struct {
		struct nlmsghdr nlh;
		struct rtgenmsg g;
	} req;
	struct msghdr msg;
	struct iovec iov;
	struct sockaddr_nl nladdr;
	mm_segment_t oldfs;
	char *pg;
	int size = 0;

	err = sock_create(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE, &sock);
	if (err) {
		printk(": Could not create a datagram socket, error = %d\n", -ENXIO);
		return err;
	}

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	req.nlh.nlmsg_len = sizeof(req);
	req.nlh.nlmsg_type = RTM_GETROUTE;
	req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
	req.nlh.nlmsg_pid = 0;
	req.g.rtgen_family = AF_INET;

	iov.iov_base = &req;
	iov.iov_len = sizeof(req);

	msg.msg_name = &nladdr;
	msg.msg_namelen = sizeof(nladdr);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	/* referece:sock_xmit in kernel code
	 * WRITE for sock_sendmsg, READ for sock_recvmsg
	 * third parameter for msg_iovlen
	 * last parameter for iov_len
	 */
	iov_iter_init(&msg.msg_iter, WRITE, &iov, 1, sizeof(req));
#else
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
#endif
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = MSG_DONTWAIT;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
	err = sock_sendmsg(sock, &msg);
#else
	err = sock_sendmsg(sock, &msg, sizeof(req));
#endif
	set_fs(oldfs);

	if (err < 0)
		goto out_sock;

	pg = (char *) __get_free_page(GFP_KERNEL);
	if (pg == NULL) {
		err = -ENOMEM;
		goto out_sock;
	}

#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
restart:
#endif

	for (;;) {
		struct nlmsghdr *h;

		iov.iov_base = pg;
		iov.iov_len = PAGE_SIZE;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
		iov_iter_init(&msg.msg_iter, READ, &iov, 1, PAGE_SIZE);
#endif

		oldfs = get_fs();
		set_fs(KERNEL_DS);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0))
		err = sock_recvmsg(sock, &msg, MSG_DONTWAIT);
#else
		err = sock_recvmsg(sock, &msg, PAGE_SIZE, MSG_DONTWAIT);
#endif
		set_fs(oldfs);

		if (err < 0)
			goto out_sock_pg;

		if (msg.msg_flags & MSG_TRUNC) {
			err = -ENOBUFS;
			goto out_sock_pg;
		}

		h = (struct nlmsghdr *) pg;

		while (NLMSG_OK(h, err)) {
			struct route_info rt_info;
			if (h->nlmsg_type == NLMSG_DONE) {
				err = 0;
				goto done;
			}

			if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *errm = (struct nlmsgerr *) NLMSG_DATA(h);
				err = errm->error;
				printk("NLMSG error: %d\n", errm->error);
				goto done;
			}

			if (h->nlmsg_type == RTM_GETROUTE)
				printk("RTM_GETROUTE: NLMSG: %d\n", h->nlmsg_type);
			if (h->nlmsg_type != RTM_NEWROUTE) {
				printk("NLMSG: %d\n", h->nlmsg_type);
				err = -EINVAL;
				goto done;
			}

			memset(&rt_info, 0, sizeof(struct route_info));
			parse_routes(h, &rt_info);
			if (!rt_info.dst_addr.s_addr && rt_info.gateway.s_addr && rt_info.dev_index) {
				*gw_addr = rt_info.gateway.s_addr;
				*gw_index = rt_info.dev_index;

			}
			h = NLMSG_NEXT(h, err);
		}

		if (err) {
			printk("!!!Remnant of size %d %d %d\n", err, h->nlmsg_len, h->nlmsg_type);
			err = -EINVAL;
			break;
		}
	}

done:
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	if (!err && req.g.rtgen_family == AF_INET) {
		req.g.rtgen_family = AF_INET6;

		iov.iov_base = &req;
		iov.iov_len = sizeof(req);

		msg.msg_name = &nladdr;
		msg.msg_namelen = sizeof(nladdr);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
		iov_iter_init(&msg.msg_iter, WRITE, &iov, 1, sizeof(req));
#else
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
#endif
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_flags = MSG_DONTWAIT;

		oldfs = get_fs();
		set_fs(KERNEL_DS);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
		err = sock_sendmsg(sock, &msg);
#else
		err = sock_sendmsg(sock, &msg, sizeof(req));
#endif
		set_fs(oldfs);

		if (err > 0)
			goto restart;
	}
#endif

out_sock_pg:
	free_page((unsigned long) pg);

out_sock:
	sock_release(sock);
	return err;
}

static int arp_query(unsigned char *haddr, u32 paddr,
		     struct net_device *dev)
{
	struct neighbour *neighbor_entry;
	int	ret = 0;

	neighbor_entry = neigh_lookup(&arp_tbl, &paddr, dev);

	if (neighbor_entry != NULL) {
		neighbor_entry->used = jiffies;
		if (neighbor_entry->nud_state & NUD_VALID) {
			_rtw_memcpy(haddr, neighbor_entry->ha, dev->addr_len);
			ret = 1;
		}
		neigh_release(neighbor_entry);
	}
	return ret;
}

static int get_defaultgw(u32 *ip_addr , char mac[])
{
	int gw_index = 0; /* oif device index */
	struct net_device *gw_dev = NULL; /* oif device */

	route_dump(ip_addr, &gw_index);

	if (!(*ip_addr) || !gw_index) {
		/* RTW_INFO("No default GW\n"); */
		return -1;
	}

	gw_dev = dev_get_by_index(&init_net, gw_index);

	if (gw_dev == NULL) {
		/* RTW_INFO("get Oif Device Fail\n"); */
		return -1;
	}

	if (!arp_query(mac, *ip_addr, gw_dev)) {
		/* RTW_INFO( "arp query failed\n"); */
		dev_put(gw_dev);
		return -1;

	}
	dev_put(gw_dev);

	return 0;
}

int	rtw_gw_addr_query(_adapter *padapter)
{
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);
	u32 gw_addr = 0; /* default gw address */
	unsigned char gw_mac[32] = {0}; /* default gw mac */
	int i;
	int res;

	res = get_defaultgw(&gw_addr, gw_mac);
	if (!res) {
		pmlmepriv->gw_ip[0] = gw_addr & 0xff;
		pmlmepriv->gw_ip[1] = (gw_addr & 0xff00) >> 8;
		pmlmepriv->gw_ip[2] = (gw_addr & 0xff0000) >> 16;
		pmlmepriv->gw_ip[3] = (gw_addr & 0xff000000) >> 24;
		_rtw_memcpy(pmlmepriv->gw_mac_addr, gw_mac, ETH_ALEN);
		RTW_INFO("%s Gateway Mac:\t" MAC_FMT "\n", __FUNCTION__, MAC_ARG(pmlmepriv->gw_mac_addr));
		RTW_INFO("%s Gateway IP:\t" IP_FMT "\n", __FUNCTION__, IP_ARG(pmlmepriv->gw_ip));
	} else
		RTW_INFO("Get Gateway IP/MAC fail!\n");

	return res;
}
#endif

int rtw_suspend_free_assoc_resource(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &padapter->wdinfo;
#endif /* CONFIG_P2P */

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));

	if (rtw_chk_roam_flags(padapter, RTW_ROAM_ON_RESUME)) {
		if (MLME_IS_STA(padapter) &&
		    #ifdef CONFIG_P2P
		    !rtw_p2p_chk_role(pwdinfo, P2P_ROLE_CLIENT) &&
		    !rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO) &&
		    #endif /* CONFIG_P2P */
		    check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) {
			RTW_INFO("%s %s(" MAC_FMT "), length:%d assoc_ssid.length:%d\n", __FUNCTION__,
				pmlmepriv->dev_cur_network.network.Ssid.Ssid,
				MAC_ARG(pmlmepriv->dev_cur_network.network.MacAddress),
				pmlmepriv->dev_cur_network.network.Ssid.SsidLength,
				pmlmepriv->assoc_ssid.SsidLength);
			rtw_set_to_roam(padapter, 1);
		}
	}

	if (MLME_IS_STA(padapter) && check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
		rtw_disassoc_cmd(padapter, 0, RTW_CMDF_DIRECTLY|RTW_CMDF_WAIT_ACK);
#ifdef CONFIG_AP_MODE
	else if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter))
		rtw_sta_flush(padapter, _FALSE);
#endif

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		/* s2-2. */
		if (1
#ifdef CONFIG_STA_CMD_DISPR
		    && (MLME_IS_STA(padapter) == _FALSE)
#endif /* CONFIG_STA_CMD_DISPR */
		   )
			rtw_free_assoc_resources(padapter, _TRUE);

		/* s2-3.  indicate disconnect to os */
		if (MLME_IS_STA(padapter)) {
			rtw_indicate_disconnect(padapter, 0, _FALSE);
			pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_SYSTEM_DUE_TO_SYSTEM_IN_SUSPEND;
			pmlmeinfo->wifi_reason_code = WLAN_REASON_DEAUTH_LEAVING;
		}
	}
	/* s2-4. */
	rtw_free_network_queue(padapter, _TRUE);
	rtw_free_mld_network_queue(padapter, _TRUE);

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY)) {
		RTW_PRINT("%s: fw_under_survey\n", __func__);
		rtw_indicate_scan_done(padapter, 1);
		clr_fwstate(pmlmepriv, WIFI_UNDER_SURVEY);
	}

	if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING) == _TRUE) {
		RTW_PRINT("%s: fw_under_linking\n", __FUNCTION__);
		rtw_indicate_disconnect(padapter, 0, _FALSE);
		pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
		pmlmeinfo->disconnect_code = DISCONNECTION_BY_SYSTEM_DUE_TO_SYSTEM_IN_SUSPEND;
		pmlmeinfo->wifi_reason_code = WLAN_REASON_DEAUTH_LEAVING;
	}

	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));
	return _SUCCESS;
}

#ifdef CONFIG_WOWLAN
int rtw_suspend_wow(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	int ret = _SUCCESS;

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));


	RTW_INFO("wowlan_mode: %d\n", pwrpriv->wowlan_mode);
	RTW_INFO("wowlan_pno_enable: %d\n", pwrpriv->wowlan_pno_enable);
#ifdef CONFIG_P2P_WOWLAN
	RTW_INFO("wowlan_p2p_enable: %d\n", pwrpriv->wowlan_p2p_enable);
#endif

	if (pwrpriv->wowlan_mode == _TRUE) {
#ifdef CONFIG_CMD_GENERAL
		rtw_phl_watchdog_stop(dvobj->phl);
#endif
		rtw_mi_netif_stop_queue(padapter);
		#ifdef CONFIG_CONCURRENT_MODE
		rtw_mi_buddy_netif_carrier_off(padapter);
		#endif

		rtw_hw_wow(padapter, _TRUE);

		if (rtw_chk_roam_flags(padapter, RTW_ROAM_ON_RESUME)) {
			if (MLME_IS_STA(padapter)
			    && check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) {
				RTW_INFO("%s %s(" MAC_FMT "), length:%d assoc_ssid.length:%d\n", __FUNCTION__,
					pmlmepriv->dev_cur_network.network.Ssid.Ssid,
					MAC_ARG(pmlmepriv->dev_cur_network.network.MacAddress),
					pmlmepriv->dev_cur_network.network.Ssid.SsidLength,
					 pmlmepriv->assoc_ssid.SsidLength);

				rtw_set_to_roam(padapter, 0);
			}
		}

		RTW_PRINT("%s: wowmode suspending\n", __func__);

		if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY) == _TRUE) {
			RTW_PRINT("%s: fw_under_survey\n", __func__);
			rtw_indicate_scan_done(padapter, 1);
			clr_fwstate(pmlmepriv, WIFI_UNDER_SURVEY);
		}

		#ifdef CONFIG_SDIO_HCI
		#if !(CONFIG_RTW_SDIO_KEEP_IRQ)
		rtw_sdio_free_irq(dvobj);
		#endif
		#endif/*CONFIG_SDIO_HCI*/

#ifdef CONFIG_CONCURRENT_MODE
		rtw_mi_buddy_suspend_free_assoc_resource(padapter);
#endif
	} else
		RTW_PRINT("%s: ### ERROR ### wowlan_mode=%d\n", __FUNCTION__, pwrpriv->wowlan_mode);
	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));
	return ret;
}
#endif /* #ifdef CONFIG_WOWLAN */

#ifdef CONFIG_AP_WOWLAN
int rtw_suspend_ap_wow(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	int ret = _SUCCESS;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));

	pwrpriv->wowlan_ap_mode = _TRUE;

	RTW_INFO("wowlan_ap_mode: %d\n", pwrpriv->wowlan_ap_mode);

	rtw_mi_netif_stop_queue(padapter);

	/* 0. Power off LED */
	rtw_led_control(padapter, LED_CTL_POWER_OFF);

	/* 1. stop thread */
	dev_set_drv_stopped(dvobj);	/*for stop thread*/
	#if 0
	rtw_mi_stop_drv_threads(padapter);
	#endif
	dev_clr_drv_stopped(dvobj);	/*for 32k command*/

	#ifdef CONFIG_SDIO_HCI
	/* 2.2 free irq */
	#if !(CONFIG_RTW_SDIO_KEEP_IRQ)
	rtw_sdio_free_irq(dvobj);
	#endif
	#endif/*CONFIG_SDIO_HCI*/

	RTW_PRINT("%s: wowmode suspending\n", __func__);

	if (rtw_mi_check_status(padapter, MI_LINKED)) {
		struct rtw_chan_def u_chdef = {0};

		if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role, padapter_link->wrlink, &u_chdef)
						!= RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s get union chandef failed\n", __func__);
			rtw_warn_on(1);
	}

		RTW_INFO("back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
			u_chdef.chan, u_chdef.bw, u_chdef.offset);
		set_channel_bwmode(padapter, padapter_link, u_chdef.chan, u_chdef.offset, u_chdef.bw, _FALSE);
	}

	/*FOR ONE AP - TODO :Multi-AP*/
	{
		int i;
		_adapter *iface;
		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			if ((iface) && rtw_is_adapter_up(iface)) {
				if (check_fwstate(&iface->mlmepriv, WIFI_AP_STATE | WIFI_MESH_STATE) == _FALSE)
					rtw_suspend_free_assoc_resource(iface);
			}
		}

	}

	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));
	return ret;
}
#endif /* #ifdef CONFIG_AP_WOWLAN */


int rtw_suspend_normal(_adapter *padapter)
{
	int ret = _SUCCESS;

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));

	rtw_mi_netif_caroff_qstop(padapter);

	rtw_mi_suspend_free_assoc_resource(padapter);

	rtw_led_control(padapter, LED_CTL_POWER_OFF);

#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	rtw_stop_cmd_thread(padapter);
#endif
#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_stop_vir_ifaces(adapter_to_dvobj(padapter));
#endif
	rtw_drv_stop_prim_iface(padapter);

	if (rtw_hw_is_init_completed(adapter_to_dvobj(padapter)))
		rtw_hw_stop(adapter_to_dvobj(padapter));
	dev_set_surprise_removed(adapter_to_dvobj(padapter));

	#ifdef CONFIG_SDIO_HCI
	rtw_sdio_deinit(adapter_to_dvobj(padapter));

	#if !(CONFIG_RTW_SDIO_KEEP_IRQ)
	rtw_sdio_free_irq(adapter_to_dvobj(padapter));
	#endif
	#endif /*CONFIG_SDIO_HCI*/

	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));
	return ret;
}

int rtw_suspend_common(_adapter *padapter)
{
	struct dvobj_priv *dvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);
#ifdef CONFIG_WOWLAN
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct registry_priv *registry_par = &padapter->registrypriv;
#endif

	int ret = 0;
	systime start_time = rtw_get_current_time();

	RTW_PRINT(" suspend start\n");
	RTW_INFO("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid);

	pdbgpriv->dbg_suspend_cnt++;

	pwrpriv->bInSuspend = _TRUE;

	if ( RTW_CANNOT_RUN(dvobj)) {
		RTW_INFO("%s bDriverStopped=%s bSurpriseRemoved = %s\n", __func__
			 , dev_is_drv_stopped(adapter_to_dvobj(padapter)) ? "True" : "False"
			, dev_is_surprise_removed(adapter_to_dvobj(padapter)) ? "True" : "False");
		pdbgpriv->dbg_suspend_error_cnt++;
		goto exit;
	}

	rtw_mi_scan_abort(padapter, _TRUE);

	rtw_mi_cancel_all_timer(padapter);

	if (rtw_mi_check_status(padapter, MI_AP_MODE) == _FALSE) {
#ifdef CONFIG_WOWLAN
		if (WOWLAN_IS_STA_MIX_MODE(padapter))
			pwrpriv->wowlan_mode = _TRUE;
		else if ( registry_par->wowlan_enable && check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
			pwrpriv->wowlan_mode = _TRUE;
		else if (pwrpriv->wowlan_pno_enable == _TRUE)
			pwrpriv->wowlan_mode |= pwrpriv->wowlan_pno_enable;

#ifdef CONFIG_P2P_WOWLAN
		if (rtw_p2p_chk_role(&padapter->wdinfo, P2P_ROLE_CLIENT) ||
		    rtw_p2p_chk_role(&padapter->wdinfo, P2P_ROLE_GO))
			pwrpriv->wowlan_p2p_mode = _TRUE;
		if (_TRUE == pwrpriv->wowlan_p2p_mode)
			pwrpriv->wowlan_mode |= pwrpriv->wowlan_p2p_mode;
#endif /* CONFIG_P2P_WOWLAN */

		if (pwrpriv->wowlan_mode == _TRUE)
			rtw_suspend_wow(padapter);
		else
#endif /* CONFIG_WOWLAN */
			rtw_suspend_normal(padapter);
	} else if (rtw_mi_check_status(padapter, MI_AP_MODE)) {
#ifdef CONFIG_AP_WOWLAN
		rtw_suspend_ap_wow(padapter);
#else
		rtw_suspend_normal(padapter);
#endif /*CONFIG_AP_WOWLAN*/
	}


	RTW_PRINT("rtw suspend success in %d ms\n",
		  rtw_get_passing_time_ms(start_time));

exit:
	RTW_INFO("<===  %s return %d.............. in %dms\n", __FUNCTION__
		 , ret, rtw_get_passing_time_ms(start_time));

	return ret;
}

#ifdef CONFIG_WOWLAN
int rtw_resume_process_wow(_adapter *padapter)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct dvobj_priv *dvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;
	struct wow_priv *wowpriv = dvobj_to_wowlan(dvobj);
	struct registry_priv  *registry_par = &padapter->registrypriv;
	int ret = _SUCCESS;

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));

	if (padapter) {
		pwrpriv = adapter_to_pwrctl(padapter);
	} else {
		pdbgpriv->dbg_resume_error_cnt++;
		ret = -1;
		goto exit;
	}

	if (RTW_CANNOT_RUN(dvobj)) {
		RTW_INFO("%s pdapter %p bDriverStopped %s bSurpriseRemoved %s\n"
			 , __func__, padapter
			 , dev_is_drv_stopped(dvobj) ? "True" : "False"
			, dev_is_surprise_removed(dvobj) ? "True" : "False");
		goto exit;
	}

	pwrpriv->wowlan_in_resume = _TRUE;

	if (pwrpriv->wowlan_mode == _TRUE) {
		/*pwrpriv->bFwCurrentInPSMode = _FALSE;*/

		#ifdef CONFIG_SDIO_HCI
		#if !(CONFIG_RTW_SDIO_KEEP_IRQ)
		if (rtw_sdio_alloc_irq(dvobj) != _SUCCESS) {
			ret = -1;
			goto exit;
		}
		#endif
		#endif/*CONFIG_SDIO_HCI*/

#ifdef CONFIG_CONCURRENT_MODE
		rtw_mi_buddy_reset_drv_sw(padapter);
#endif
		rtw_hw_wow(padapter, _FALSE);

		dev_clr_drv_stopped(dvobj);
		RTW_INFO("%s: wowmode resuming, DriverStopped:%s\n", __func__, dev_is_drv_stopped(dvobj) ? "True" : "False");

		#if 0
		rtw_mi_start_drv_threads(padapter);
		#endif

#ifdef CONFIG_CONCURRENT_MODE
		rtw_mi_buddy_netif_carrier_on(padapter);
#endif

		/* start netif queue */
		rtw_mi_netif_wake_queue(padapter);

	} else

		RTW_PRINT("%s: ### ERROR ### wowlan_mode=%d\n", __FUNCTION__, pwrpriv->wowlan_mode);

	if (padapter->pid[1] != 0) {
		RTW_INFO("pid[1]:%d\n", padapter->pid[1]);
		rtw_signal_process(padapter->pid[1], SIGUSR2);
	}

	if (IS_DISCONNECT_WOW_REASON(wowpriv->wow_wake_reason)) {
		RTW_INFO("%s: disconnect reason: %02x\n", __func__,
			 wowpriv->wow_wake_reason);

		rtw_sta_media_status_rpt(padapter,
					 rtw_get_stainfo(&padapter->stapriv,
					 get_bssid(&padapter->mlmepriv)), 0);
		rtw_disassoc_cmd(padapter, 0, RTW_CMDF_WAIT_ACK);
		if (MLME_IS_ASOC(padapter) == _TRUE)
			rtw_free_assoc_resources(padapter, _TRUE);
		rtw_indicate_disconnect(padapter, 0, _FALSE);
		pmlmeinfo->state = WIFI_FW_NULL_STATE;

		pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
		if (wowpriv->wow_wake_reason == RTW_MAC_WOW_FW_DECISION_DISCONNECT ||
		    wowpriv->wow_wake_reason == RTW_MAC_WOW_NO_WAKE_FW_DECISION_DISCONNECT)
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_FW_DUE_TO_FW_DECISION_IN_WOW_RESUME;
		else if (wowpriv->wow_wake_reason == RTW_MAC_WOW_RX_DISASSOC ||
			 wowpriv->wow_wake_reason == RTW_MAC_WOW_NO_WAKE_RX_DISASSOC)
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DISASSOC_IN_WOW_RESUME;
		else if (wowpriv->wow_wake_reason == RTW_MAC_WOW_RX_DEAUTH ||
			 wowpriv->wow_wake_reason == RTW_MAC_WOW_NO_WAKE_RX_DEAUTH)
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DEAUTH_IN_WOW_RESUME;
		pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
	} else {
		if (rtw_chk_roam_flags(padapter, RTW_ROAM_ON_RESUME)) {
			RTW_INFO("%s: do roaming\n", __func__);
			rtw_roaming(padapter, NULL);
		}
	}

	if (pwrpriv->wowlan_mode == _TRUE) {
#ifdef CONFIG_CMD_GENERAL
		rtw_phl_watchdog_start(dvobj->phl);
#endif
#if 0 /*ndef CONFIG_IPS_CHECK_IN_WD*/
		rtw_set_pwr_state_check_timer(pwrpriv);
#endif
	} else
		RTW_PRINT("do not reset timer\n");

	pwrpriv->wowlan_mode = _FALSE;

	/* Power On LED */
#ifdef CONFIG_RTW_SW_LED

	if (IS_DISCONNECT_WOW_REASON(wowpriv->wow_wake_reason))
		rtw_led_control(padapter, LED_CTL_NO_LINK);
	else
		rtw_led_control(padapter, LED_CTL_LINK);
#endif

exit:
	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));
	return ret;
}
#endif /* #ifdef CONFIG_WOWLAN */

#ifdef CONFIG_AP_WOWLAN
int rtw_resume_process_ap_wow(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct debug_priv *pdbgpriv = &dvobj->drv_dbg;
	struct sta_info	*psta = NULL;
	int ret = _SUCCESS;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));

	if (padapter) {
		pwrpriv = adapter_to_pwrctl(padapter);
	} else {
		pdbgpriv->dbg_resume_error_cnt++;
		ret = -1;
		goto exit;
	}

	/* pwrpriv->bFwCurrentInPSMode = _FALSE; */
#if 0 /*GEORGIA_TODO_REMOVE_IT_FOR_PHL_ARCH*/

	rtw_hal_disable_interrupt(GET_PHL_COM(dvobj));

	rtw_hal_clear_interrupt(padapter);
#endif

	#ifdef CONFIG_SDIO_HCI
	#if !(CONFIG_RTW_SDIO_KEEP_IRQ)
	if (rtw_sdio_alloc_irq(dvobj) != _SUCCESS) {
		ret = -1;
		goto exit;
	}
	#endif
	#endif/*CONFIG_SDIO_HCI*/

	pwrpriv->wowlan_ap_mode = _FALSE;

	dev_clr_drv_stopped(dvobj);
	RTW_INFO("%s: wowmode resuming, DriverStopped:%s\n", __func__, dev_is_drv_stopped(dvobj) ? "True" : "False");

	#if 0
	rtw_mi_start_drv_threads(padapter);
	#endif

	if (rtw_mi_check_status(padapter, MI_LINKED)) {
		struct rtw_chan_def u_chdef = {0};

		if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role, padapter_link->wrlink, &u_chdef)
								!= RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s get union chandef failed\n", __func__);
			rtw_warn_on(1);
	}

		RTW_INFO(FUNC_ADPT_FMT" back to linked/linking union - ch:%u, bw:%u, offset:%u\n",
			FUNC_ADPT_ARG(padapter), u_chdef.chan, u_chdef.bw, u_chdef.offset);
		set_channel_bwmode(padapter, padapter_link, u_chdef.chan, u_chdef.offset, u_chdef.bw, _FALSE);
	}

	/*FOR ONE AP - TODO :Multi-AP*/
	{
		int i;
		_adapter *iface;

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			if ((iface) && rtw_is_adapter_up(iface)) {
				if (check_fwstate(&iface->mlmepriv, WIFI_AP_STATE | WIFI_MESH_STATE | WIFI_ASOC_STATE))
					rtw_reset_drv_sw(iface);
			}
		}

	}

	/* start netif queue */
	rtw_mi_netif_wake_queue(padapter);

	if (padapter->pid[1] != 0) {
		RTW_INFO("pid[1]:%d\n", padapter->pid[1]);
		rtw_signal_process(padapter->pid[1], SIGUSR2);
	}

#ifdef CONFIG_RESUME_IN_WORKQUEUE
	/* rtw_unlock_suspend(); */
#endif /* CONFIG_RESUME_IN_WORKQUEUE */

	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	_set_timer(&dvobj->dynamic_chk_timer, 2000);
	#endif
#if 0 /*ndef CONFIG_IPS_CHECK_IN_WD*/
	rtw_set_pwr_state_check_timer(pwrpriv);
#endif

	/* Power On LED */
#ifdef CONFIG_RTW_SW_LED

	rtw_led_control(padapter, LED_CTL_LINK);
#endif
exit:
	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));
	return ret;
}
#endif /* #ifdef CONFIG_APWOWLAN */

void rtw_mi_resume_process_normal(_adapter *padapter)
{
	int i;
	_adapter *iface;
	struct mlme_priv *pmlmepriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if ((iface) && rtw_is_adapter_up(iface)) {
			pmlmepriv = &iface->mlmepriv;

			if (MLME_IS_STA(padapter)) {
				RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - WIFI_STATION_STATE\n", FUNC_ADPT_ARG(iface), get_fwstate(pmlmepriv));

				if (rtw_chk_roam_flags(iface, RTW_ROAM_ON_RESUME))
					rtw_roaming(iface, NULL);

			} else if (MLME_IS_AP(iface) || MLME_IS_MESH(iface)) {
				RTW_INFO(FUNC_ADPT_FMT" %s\n", FUNC_ADPT_ARG(iface), MLME_IS_AP(iface) ? "AP" : "MESH");
				rtw_ap_restore_network(iface);
			} else if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE))
				RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - WIFI_ADHOC_STATE\n", FUNC_ADPT_ARG(iface), get_fwstate(pmlmepriv));
			else
				RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - ???\n", FUNC_ADPT_ARG(iface), get_fwstate(pmlmepriv));
		}
	}
}

int rtw_resume_process_normal(_adapter *padapter)
{
	struct net_device *pnetdev;
	struct pwrctrl_priv *pwrpriv;
	struct dvobj_priv *dvobj;
	struct debug_priv *pdbgpriv;

	int ret = _SUCCESS;

	if (!padapter) {
		ret = -1;
		goto exit;
	}

	pnetdev = padapter->pnetdev;
	pwrpriv = adapter_to_pwrctl(padapter);
	dvobj = padapter->dvobj;
	pdbgpriv = &dvobj->drv_dbg;

	RTW_INFO("==> "FUNC_ADPT_FMT" entry....\n", FUNC_ADPT_ARG(padapter));

	#ifdef CONFIG_SDIO_HCI
	/* interface init */
	if (rtw_sdio_init(dvobj) != _SUCCESS) {
		ret = -1;
		goto exit;
	}
	#endif/*CONFIG_SDIO_HCI*/

	dev_clr_surprise_removed(dvobj);
#if 0 /*GEORGIA_TODO_REMOVE_IT_FOR_PHL_ARCH*/
	rtw_hal_disable_interrupt(GET_PHL_COM(dvobj));
#endif
	#ifdef CONFIG_SDIO_HCI
	#if !(CONFIG_RTW_SDIO_KEEP_IRQ)
	if (rtw_sdio_alloc_irq(dvobj) != _SUCCESS) {
		ret = -1;
		goto exit;
	}
	#endif
	#endif/*CONFIG_SDIO_HCI*/

	rtw_mi_reset_drv_sw(padapter);

	pwrpriv->bkeepfwalive = _FALSE;

	RTW_INFO("bkeepfwalive(%x)\n", pwrpriv->bkeepfwalive);
	if (pm_netdev_open(pnetdev, _TRUE) != 0) {
		ret = -1;
		pdbgpriv->dbg_resume_error_cnt++;
		goto exit;
	}

	rtw_mi_netif_caron_qstart(padapter);

	if (padapter->pid[1] != 0) {
		RTW_INFO("pid[1]:%d\n", padapter->pid[1]);
		rtw_signal_process(padapter->pid[1], SIGUSR2);
	}

	rtw_mi_resume_process_normal(padapter);

#ifdef CONFIG_RESUME_IN_WORKQUEUE
	/* rtw_unlock_suspend(); */
#endif /* CONFIG_RESUME_IN_WORKQUEUE */
	RTW_INFO("<== "FUNC_ADPT_FMT" exit....\n", FUNC_ADPT_ARG(padapter));

exit:
	return ret;
}

int rtw_resume_common(_adapter *padapter)
{
	int ret = 0;
	systime start_time = rtw_get_current_time();
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	if (pwrpriv == NULL)
		return 0;

	if (pwrpriv->bInSuspend == _FALSE)
		return 0;

	RTW_PRINT("resume start\n");
	RTW_INFO("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid);

	if (rtw_mi_check_status(padapter, MI_AP_MODE) == _FALSE) {
#ifdef CONFIG_WOWLAN
		if (pwrpriv->wowlan_mode == _TRUE)
			rtw_resume_process_wow(padapter);
		else
#endif
			rtw_resume_process_normal(padapter);

	} else if (rtw_mi_check_status(padapter, MI_AP_MODE)) {
#ifdef CONFIG_AP_WOWLAN
		rtw_resume_process_ap_wow(padapter);
#else
		rtw_resume_process_normal(padapter);
#endif /* CONFIG_AP_WOWLAN */
	}


	pwrpriv->bInSuspend = _FALSE;
	pwrpriv->wowlan_in_resume = _FALSE;

	RTW_PRINT("%s:%d in %d ms\n", __FUNCTION__ , ret,
		  rtw_get_passing_time_ms(start_time));


	return ret;
}

#ifdef CONFIG_GPIO_API
u8 rtw_get_gpio(struct net_device *netdev, u8 gpio_num)
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(netdev);
	return rtw_hal_get_gpio(adapter, gpio_num);
}
EXPORT_SYMBOL(rtw_get_gpio);

int  rtw_set_gpio_output_value(struct net_device *netdev, u8 gpio_num, bool isHigh)
{
	u8 direction = 0;
	u8 res = -1;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(netdev);
	return rtw_hal_set_gpio_output_value(adapter, gpio_num, isHigh);
}
EXPORT_SYMBOL(rtw_set_gpio_output_value);

int rtw_config_gpio(struct net_device *netdev, u8 gpio_num, bool isOutput)
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(netdev);
	return rtw_hal_config_gpio(adapter, gpio_num, isOutput);
}
EXPORT_SYMBOL(rtw_config_gpio);
int rtw_register_gpio_interrupt(struct net_device *netdev, int gpio_num, void(*callback)(u8 level))
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(netdev);
	return rtw_hal_register_gpio_interrupt(adapter, gpio_num, callback);
}
EXPORT_SYMBOL(rtw_register_gpio_interrupt);

int rtw_disable_gpio_interrupt(struct net_device *netdev, int gpio_num)
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(netdev);
	return rtw_hal_disable_gpio_interrupt(adapter, gpio_num);
}
EXPORT_SYMBOL(rtw_disable_gpio_interrupt);

#endif /* #ifdef CONFIG_GPIO_API */

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE

int rtw_vendor_ie_get_api(struct net_device *dev, int ie_num, char *extra,
		u16 extra_len)
{
	int ret = 0;

	ret = rtw_vendor_ie_get_raw_data(dev, ie_num, extra, extra_len);
	return ret;
}
EXPORT_SYMBOL(rtw_vendor_ie_get_api);

int rtw_vendor_ie_set_api(struct net_device *dev, char *extra)
{
	return rtw_vendor_ie_set(dev, NULL, NULL, extra);
}
EXPORT_SYMBOL(rtw_vendor_ie_set_api);

#endif
