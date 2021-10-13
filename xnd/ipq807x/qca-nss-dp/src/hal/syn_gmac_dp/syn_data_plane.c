/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/module.h>
#include "syn_data_plane.h"
#include "syn_reg.h"

#define SYN_DP_NAPI_BUDGET	64

/*
 * GMAC Ring info
 */
struct syn_dp_info dp_info[NSS_DP_HAL_MAX_PORTS];

/*
 * syn_dp_napi_poll()
 *	Scheduled by napi to process RX and TX complete
 */
static int syn_dp_napi_poll(struct napi_struct *napi, int budget)
{
	struct nss_dp_dev *gmac_dev = container_of(napi, struct nss_dp_dev, napi);
	struct syn_dp_info *dev_info = &dp_info[gmac_dev->macid - 1];
	int work_done;

	/*
	 * Update GMAC stats
	 */
	spin_lock_bh(&dp_info->stats_lock);
	dp_info->stats.stats.rx_missed += syn_get_rx_missed(gmac_dev->gmac_hal_ctx);
	dp_info->stats.stats.rx_missed += syn_get_fifo_overflows(gmac_dev->gmac_hal_ctx);
	spin_unlock_bh(&dp_info->stats_lock);

	syn_dp_process_tx_complete(gmac_dev, dev_info);
	work_done = syn_dp_rx(gmac_dev, dev_info, budget);
	syn_dp_rx_refill(gmac_dev, dev_info);

	if (work_done < budget) {
		napi_complete(napi);
		syn_enable_dma_interrupt(gmac_dev->gmac_hal_ctx);
	}

	return work_done;
}

/*
 * syn_dp_handle_irq()
 *	Process IRQ and schedule napi
 */
static irqreturn_t syn_dp_handle_irq(int irq, void *ctx)
{
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)ctx;
	struct nss_gmac_hal_dev *nghd = gmac_dev->gmac_hal_ctx;

	syn_clear_dma_status(nghd);
	syn_disable_dma_interrupt(nghd);

	/*
	 * Schedule NAPI
	 */
	napi_schedule(&gmac_dev->napi);

	return IRQ_HANDLED;
}

/*
 * syn_dp_if_init()
 *	Initialize the GMAC data plane operations
 */
static int syn_dp_if_init(struct nss_dp_data_plane_ctx *dpc)
{
	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	uint32_t macid = gmac_dev->macid;
	struct syn_dp_info *dev_info = &dp_info[macid - 1];
	struct device *dev = &gmac_dev->pdev->dev;
	int err;

	if (!netdev) {
		netdev_dbg(netdev, "nss_dp_gmac: Invalid netdev pointer %px\n", netdev);
		return NSS_DP_FAILURE;
	}

	netdev_info(netdev, "nss_dp_gmac: Registering netdev %s(qcom-id:%d) with GMAC\n", netdev->name, macid);

	if (!dev_info->napi_added) {
		netif_napi_add(netdev, &gmac_dev->napi, syn_dp_napi_poll, SYN_DP_NAPI_BUDGET);

		/*
		 * Requesting irq
		 */
		netdev->irq = platform_get_irq(gmac_dev->pdev, 0);
		err = request_irq(netdev->irq, syn_dp_handle_irq, 0, "nss-dp-gmac", gmac_dev);
		if (err) {
			netdev_dbg(netdev, "err_code:%d, Mac %d IRQ %d request failed\n", err,
					gmac_dev->macid, netdev->irq);
			return NSS_DP_FAILURE;
		}

		gmac_dev->drv_flags |= NSS_DP_PRIV_FLAG(IRQ_REQUESTED);
		dev_info->napi_added = 1;
	}

	/*
	 * Forcing the kernel to use 32-bit DMA addressing
	 */
	dma_set_coherent_mask(dev, DMA_BIT_MASK(32));

	/*
	 * Initialize the Tx/Rx ring
	 */
	if (syn_dp_setup_rings(gmac_dev, netdev, dev, dev_info)) {
		netdev_dbg(netdev, "nss_dp_gmac: Error initializing GMAC rings %px\n", netdev);
		return NSS_DP_FAILURE;
	}

	spin_lock_init(&dev_info->data_lock);
	spin_lock_init(&dev_info->stats_lock);

	netdev_dbg(netdev,"Synopsys GMAC dataplane initialized\n");

	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_open()
 *	Open the GMAC data plane operations
 */
static int syn_dp_if_open(struct nss_dp_data_plane_ctx *dpc, uint32_t tx_desc_ring,
					uint32_t rx_desc_ring, uint32_t mode)
{
	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	struct nss_gmac_hal_dev *nghd = gmac_dev->gmac_hal_ctx;

	syn_enable_dma_rx(nghd);
	syn_enable_dma_tx(nghd);

	napi_enable(&gmac_dev->napi);
	syn_enable_dma_interrupt(nghd);

	netdev_dbg(netdev, "Synopsys GMAC dataplane opened\n");

	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_close()
 *	Close the GMAC data plane operations
 */
static int syn_dp_if_close(struct nss_dp_data_plane_ctx *dpc)
{
	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	struct nss_gmac_hal_dev *nghd = gmac_dev->gmac_hal_ctx;

	syn_disable_dma_rx(nghd);
	syn_disable_dma_tx(nghd);

	syn_disable_dma_interrupt(nghd);
	napi_disable(&gmac_dev->napi);

	netdev_dbg(netdev, "Synopsys GMAC dataplane closed\n");

	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_link_state()
 *	Change of link for the dataplane
 */
static int syn_dp_if_link_state(struct nss_dp_data_plane_ctx *dpc, uint32_t link_state)
{
	struct net_device *netdev = dpc->dev;

	/*
	 * Switch interrupt based on the link state
	 */
	if (link_state) {
		netdev_dbg(netdev, "Data plane link up\n");
	} else {
		netdev_dbg(netdev, "Data plane link down\n");
	}

	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_mac_addr()
 */
static int syn_dp_if_mac_addr(struct nss_dp_data_plane_ctx *dpc, uint8_t *addr)
{
	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_change_mtu()
 */
static int syn_dp_if_change_mtu(struct nss_dp_data_plane_ctx *dpc, uint32_t mtu)
{
	/*
	 * TODO: Work on MTU fix along with register update for frame length
	 */
	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_set_features()
 *	Set the supported net_device features
 */
static void syn_dp_if_set_features(struct nss_dp_data_plane_ctx *dpc)
{
	struct net_device *netdev = dpc->dev;

	netdev->features |= NETIF_F_HW_CSUM | NETIF_F_RXCSUM;
	netdev->hw_features |= NETIF_F_HW_CSUM | NETIF_F_RXCSUM;
	netdev->vlan_features |= NETIF_F_HW_CSUM | NETIF_F_RXCSUM;
	netdev->wanted_features |= NETIF_F_HW_CSUM | NETIF_F_RXCSUM;
}

/*
 * syn_dp_if_xmit()
 *	Dataplane method to transmit the packet
 */
static netdev_tx_t syn_dp_if_xmit(struct nss_dp_data_plane_ctx *dpc, struct sk_buff *skb)
{
	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	struct syn_dp_info *dev_info = &dp_info[gmac_dev->macid - 1];
	int nfrags = skb_shinfo(skb)->nr_frags;

	/*
	 * Most likely, it is not a fragmented pkt, optimize for that
	 */
	if (likely(nfrags == 0)) {
		if (syn_dp_tx(gmac_dev, dev_info, skb)) {
			goto drop;
		}

		return NETDEV_TX_OK;
	}

drop:
	dev_kfree_skb_any(skb);
	dev_info->stats.stats.tx_dropped++;

	return NETDEV_TX_BUSY;
}

/*
 * syn_dp_if_pause_on_off()
 */
static int syn_dp_if_pause_on_off(struct nss_dp_data_plane_ctx *dpc, uint32_t pause_on)
{
	return NSS_DP_SUCCESS;
}

/*
 * syn_dp_if_get_stats
 *	Get Synopsys GMAC data plane stats
 */
static void syn_dp_if_get_stats(struct nss_dp_data_plane_ctx *dpc, struct nss_dp_gmac_stats *stats)
{
	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	struct syn_dp_info *dev_info = &dp_info[gmac_dev->macid - 1];

	spin_lock_bh(&dev_info->stats_lock);
	netdev_dbg(netdev, "GETTING stats: rx_packets:%llu rx_bytes:%llu mmc_rx_crc_errors:%llu", dev_info->stats.stats.rx_packets,
			dev_info->stats.stats.rx_bytes, dev_info->stats.stats.mmc_rx_crc_errors);
	memcpy(stats, &dev_info->stats, sizeof(*stats));
	spin_unlock_bh(&dev_info->stats_lock);
}

/*
 * syn_dp_if_deinit()
 *	Free all the Synopsys GMAC resources
 */
static int syn_dp_if_deinit(struct nss_dp_data_plane_ctx *dpc)
{
	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *gmac_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	struct syn_dp_info *dev_info = &dp_info[gmac_dev->macid - 1];

	if (dev_info->napi_added) {
		/*
		 * Remove interrupt handlers and NAPI
		 */
		if (gmac_dev->drv_flags & NSS_DP_PRIV_FLAG(IRQ_REQUESTED)) {
			netdev_dbg(netdev, "Freeing IRQ %d for Mac %d\n", netdev->irq, gmac_dev->macid);
			synchronize_irq(netdev->irq);
			free_irq(netdev->irq, gmac_dev);
			gmac_dev->drv_flags &= ~NSS_DP_PRIV_FLAG(IRQ_REQUESTED);
		}

		netif_napi_del(&gmac_dev->napi);
		dev_info->napi_added = 0;
	}

	/*
	 * Cleanup and free the rings
	 */
	syn_dp_cleanup_rings(gmac_dev, netdev, dev_info);

	return NSS_DP_SUCCESS;
}

/*
 * nss_dp_gmac_ops
 *	Data plane operations for Synopsys GMAC
 */
struct nss_dp_data_plane_ops nss_dp_gmac_ops = {
	.init		= syn_dp_if_init,
	.open		= syn_dp_if_open,
	.close		= syn_dp_if_close,
	.link_state	= syn_dp_if_link_state,
	.mac_addr	= syn_dp_if_mac_addr,
	.change_mtu	= syn_dp_if_change_mtu,
	.xmit		= syn_dp_if_xmit,
	.set_features	= syn_dp_if_set_features,
	.pause_on_off	= syn_dp_if_pause_on_off,
	.get_stats	= syn_dp_if_get_stats,
	.deinit		= syn_dp_if_deinit,
};
