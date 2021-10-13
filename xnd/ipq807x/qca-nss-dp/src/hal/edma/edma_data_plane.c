/*
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
 * USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/debugfs.h>
#include <fal/fal_vsi.h>

#include "nss_dp_dev.h"
#include "edma_regs.h"
#include "edma_data_plane.h"

/*
 * EDMA hardware instance
 */
struct edma_hw edma_hw;

/*
 * edma_get_port_num_from_netdev()
 *	Get port number from net device
 */
static int edma_get_port_num_from_netdev(struct net_device *netdev)
{
	int i;

	for (i = 0; i < EDMA_MAX_GMACS; i++) {
		/* In the port-id to netdev mapping table, port-id
		 * starts from 1 and table index starts from 0.
		 * So we return index + 1 for port-id
		 */
		if (edma_hw.netdev_arr[i] == netdev)
			return i+1;
	}

	return -1;
}

/*
 * edma_reg_read()
 *	Read EDMA register
 */
uint32_t edma_reg_read(uint32_t reg_off)
{
	return (uint32_t)readl(edma_hw.reg_base + reg_off);
}

/*
 * edma_reg_write()
 *	Write EDMA register
 */
void edma_reg_write(uint32_t reg_off, uint32_t val)
{
	writel(val, edma_hw.reg_base + reg_off);
}

/*
 * edma_disable_interrupts()
 *	Disable EDMA RX/TX interrupt masks.
 */
static void edma_disable_interrupts(void)
{
	struct edma_rxdesc_ring *rxdesc_ring = NULL;
	struct edma_rxfill_ring *rxfill_ring = NULL;
	struct edma_txcmpl_ring *txcmpl_ring = NULL;
	int i;

	for (i = 0; i < edma_hw.rxdesc_rings; i++) {
		rxdesc_ring = &edma_hw.rxdesc_ring[i];
		edma_reg_write(EDMA_REG_RXDESC_INT_MASK(rxdesc_ring->id),
			       EDMA_MASK_INT_CLEAR);
	}

	for (i = 0; i < edma_hw.txcmpl_rings; i++) {
		txcmpl_ring = &edma_hw.txcmpl_ring[i];
		edma_reg_write(EDMA_REG_TX_INT_MASK(txcmpl_ring->id),
			       EDMA_MASK_INT_CLEAR);
	}

	for (i = 0; i < edma_hw.rxfill_rings; i++) {
		rxfill_ring = &edma_hw.rxfill_ring[i];
		edma_reg_write(EDMA_REG_RXFILL_INT_MASK(rxfill_ring->id),
			       EDMA_MASK_INT_CLEAR);
	}

	/*
	 * Clear MISC interrupt mask.
	 */
	edma_reg_write(EDMA_REG_MISC_INT_MASK, EDMA_MASK_INT_CLEAR);
}

/*
 * edma_enable_interrupts()
 *	Enable RX/TX EDMA interrupt masks.
 */
static void edma_enable_interrupts(void)
{
	struct edma_rxdesc_ring *rxdesc_ring = NULL;
	struct edma_rxfill_ring *rxfill_ring = NULL;
	struct edma_txcmpl_ring *txcmpl_ring = NULL;
	int i;

	for (i = 0; i < edma_hw.rxfill_rings; i++) {
		rxfill_ring = &edma_hw.rxfill_ring[i];
		edma_reg_write(EDMA_REG_RXFILL_INT_MASK(rxfill_ring->id),
				edma_hw.rxfill_intr_mask);
	}

	for (i = 0; i < edma_hw.txcmpl_rings; i++) {
		txcmpl_ring = &edma_hw.txcmpl_ring[i];
		edma_reg_write(EDMA_REG_TX_INT_MASK(txcmpl_ring->id),
				edma_hw.txcmpl_intr_mask);
	}

	for (i = 0; i < edma_hw.rxdesc_rings; i++) {
		rxdesc_ring = &edma_hw.rxdesc_ring[i];
		edma_reg_write(EDMA_REG_RXDESC_INT_MASK(rxdesc_ring->id),
				edma_hw.rxdesc_intr_mask);
	}

	/*
	 * Enable MISC interrupt mask.
	 */
	edma_reg_write(EDMA_REG_MISC_INT_MASK, edma_hw.misc_intr_mask);
}

/*
 * nss_dp_edma_if_open()
 *	Do slow path data plane open
 */
static int edma_if_open(struct nss_dp_data_plane_ctx *dpc,
			uint32_t tx_desc_ring, uint32_t rx_desc_ring,
			uint32_t mode)
{
	if (!dpc->dev)
		return NSS_DP_FAILURE;

	/*
	 * Enable NAPI
	 */
	if (edma_hw.active++ != 0)
		return NSS_DP_SUCCESS;

	napi_enable(&edma_hw.napi);

	/*
	 * Enable the interrupt masks.
	 */
	edma_enable_interrupts();

	return NSS_DP_SUCCESS;
}

/*
 * edma_if_close()
 *	Do slow path data plane close
 */
static int edma_if_close(struct nss_dp_data_plane_ctx *dpc)
{
	if (--edma_hw.active != 0)
		return NSS_DP_SUCCESS;

	/*
	 * Disable the interrupt masks.
	 */
	edma_disable_interrupts();

	/*
	 * Disable NAPI
	 */
	napi_disable(&edma_hw.napi);
	return NSS_DP_SUCCESS;
}

/*
 * edma_if_link_state()
 */
static int edma_if_link_state(struct nss_dp_data_plane_ctx *dpc,
			      uint32_t link_state)
{
	return NSS_DP_SUCCESS;
}

/*
 * edma_if_mac_addr()
 */
static int edma_if_mac_addr(struct nss_dp_data_plane_ctx *dpc, uint8_t *addr)
{
	return NSS_DP_SUCCESS;
}

/*
 * edma_if_change_mtu()
 */
static int edma_if_change_mtu(struct nss_dp_data_plane_ctx *dpc, uint32_t mtu)
{
	return NSS_DP_SUCCESS;
}

/*
 * edma_if_xmit()
 *	Transmit a packet using EDMA
 */
static netdev_tx_t edma_if_xmit(struct nss_dp_data_plane_ctx *dpc,
				struct sk_buff *skb)
{
	struct net_device *netdev = dpc->dev;
	int ret;
	uint32_t tx_ring, skbq, nhead, ntail;
	bool expand_skb = false;

	if (skb->len < ETH_HLEN) {
		netdev_dbg(netdev, "skb->len < ETH_HLEN\n");
		goto drop;
	}

	/*
	 * Select a Tx ring
	 */
	skbq = skb_get_queue_mapping(skb);
	tx_ring = 0;
	if ((edma_hw.txdesc_rings > 1) && (skbq > 0))
		tx_ring = edma_hw.txdesc_rings % skbq;

	/*
	 * Check for non-linear skb
	 */
	if (skb_is_nonlinear(skb)) {
		netdev_dbg(netdev, "cannot Tx non-linear skb:%px\n", skb);
		goto drop;
	}

	/*
	 * Check for headroom/tailroom and clone
	 */
	nhead = netdev->needed_headroom;
	ntail = netdev->needed_tailroom;

	if (skb_cloned(skb) ||
		(skb_headroom(skb) < nhead) ||
		(skb_headroom(skb) < ntail)) {
		expand_skb = true;
	}

	/*
	 * Expand the skb. This also unclones a cloned skb.
	 */
	if (expand_skb && pskb_expand_head(skb, nhead, ntail, GFP_ATOMIC)) {
		netdev_dbg(netdev, "cannot expand skb:%px\n", skb);
		goto drop;
	}

	/*
	 * Transmit the packet
	 */
	ret = edma_ring_xmit(&edma_hw, netdev, skb,
			&edma_hw.txdesc_ring[tx_ring]);
	if (ret == EDMA_TX_OK)
		return NETDEV_TX_OK;

	/*
	 * Not enough descriptors. Stop netdev Tx queue.
	 */
	if (ret == EDMA_TX_DESC) {
		netif_stop_queue(netdev);
		return NETDEV_TX_BUSY;
	}

drop:
	dev_kfree_skb_any(skb);
	netdev->stats.tx_dropped++;

	return NETDEV_TX_OK;
}

/*
 * edma_if_set_features()
 *	Set the supported net_device features
 */
static void edma_if_set_features(struct nss_dp_data_plane_ctx *dpc)
{
	/*
	 * TODO - add flags to support HIGHMEM/cksum offload VLAN
	 * the features are enabled.
	 */
}

/* TODO - check if this is needed */
/*
 * edma_if_pause_on_off()
 *	Set pause frames on or off
 *
 * No need to send a message if we defaulted to slow path.
 */
static int edma_if_pause_on_off(struct nss_dp_data_plane_ctx *dpc,
				uint32_t pause_on)
{
	return NSS_DP_SUCCESS;
}

/*
 * edma_if_vsi_assign()
 *	assign vsi of the data plane
 *
 */
static int edma_if_vsi_assign(struct nss_dp_data_plane_ctx *dpc, uint32_t vsi)
{
	struct net_device *netdev = dpc->dev;
	int32_t port_num;

	port_num = edma_get_port_num_from_netdev(netdev);

	if (port_num < 0)
		return NSS_DP_FAILURE;

	if (fal_port_vsi_set(0, port_num, vsi) < 0)
		return NSS_DP_FAILURE;

	return NSS_DP_SUCCESS;
}

/*
 * edma_if_vsi_unassign()
 *	unassign vsi of the data plane
 *
 */
static int edma_if_vsi_unassign(struct nss_dp_data_plane_ctx *dpc, uint32_t vsi)
{
	struct net_device *netdev = dpc->dev;
	uint32_t port_num;

	port_num = edma_get_port_num_from_netdev(netdev);

	if (port_num < 0)
		return NSS_DP_FAILURE;

	if (fal_port_vsi_set(0, port_num, 0xffff) < 0)
		return NSS_DP_FAILURE;

	return NSS_DP_SUCCESS;
}

#ifdef CONFIG_RFS_ACCEL
/*
 * edma_if_rx_flow_steer()
 *	Flow steer of the data plane
 *
 * Initial receive flow steering function for data plane operation.
 */
static int edma_if_rx_flow_steer(struct nss_dp_data_plane_ctx *dpc, struct sk_buff *skb,
					uint32_t cpu, bool is_add)
{
	return NSS_DP_SUCCESS;
}
#endif

/*
 * edma_if_deinit()
 *	Free edma resources
 */
static int edma_if_deinit(struct nss_dp_data_plane_ctx *dpc)
{
	/*
	 * Free up resources used by EDMA if all the
	 * interfaces have been overridden
	 * */
	if (edma_hw.dp_override_cnt == EDMA_MAX_GMACS - 1) {
		edma_cleanup(true);
	} else {
		edma_hw.dp_override_cnt++;
	}

	return NSS_DP_SUCCESS;
}

/*
 * edma_irq_init()
 *	Initialize interrupt handlers for the driver
 */
static int edma_irq_init(void)
{
	int err;
	uint32_t entry_num, i;

	/*
	 * Get TXCMPL rings IRQ numbers
	 */
	entry_num = 0;
	for (i = 0; i < edma_hw.txcmpl_rings; i++, entry_num++) {
		edma_hw.txcmpl_intr[i] =
			platform_get_irq(edma_hw.pdev, entry_num);
		if (edma_hw.txcmpl_intr[i] < 0) {
			pr_warn("%s: txcmpl_intr[%u] irq get failed\n",
					(edma_hw.device_node)->name, i);
			return -1;
		}

		pr_debug("%s: txcmpl_intr[%u] = %u\n",
				 (edma_hw.device_node)->name,
				 i, edma_hw.txcmpl_intr[i]);
	}

	/*
	 * Get RXFILL rings IRQ numbers
	 */
	for (i = 0; i < edma_hw.rxfill_rings; i++, entry_num++) {
		edma_hw.rxfill_intr[i] =
			platform_get_irq(edma_hw.pdev, entry_num);
		if (edma_hw.rxfill_intr[i] < 0) {
			pr_warn("%s: rxfill_intr[%u] irq get failed\n",
					(edma_hw.device_node)->name, i);
			return -1;
		}

		pr_debug("%s: rxfill_intr[%u] = %u\n",
				 (edma_hw.device_node)->name,
				 i, edma_hw.rxfill_intr[i]);
	}

	/*
	 * Get RXDESC rings IRQ numbers
	 *
	 */
	for (i = 0; i < edma_hw.rxdesc_rings; i++, entry_num++) {
		edma_hw.rxdesc_intr[i] =
			platform_get_irq(edma_hw.pdev, entry_num);
		if (edma_hw.rxdesc_intr[i] < 0) {
			pr_warn("%s: rxdesc_intr[%u] irq get failed\n",
					(edma_hw.device_node)->name, i);
			return -1;
		}

		pr_debug("%s: rxdesc_intr[%u] = %u\n",
				 (edma_hw.device_node)->name,
				 i, edma_hw.rxdesc_intr[i]);
	}

	/*
	 * Get misc IRQ number
	 */
	edma_hw.misc_intr = platform_get_irq(edma_hw.pdev, entry_num);
	pr_debug("%s: misc IRQ:%u\n",
			  (edma_hw.device_node)->name,
			  edma_hw.misc_intr);

	/*
	 * Request IRQ for TXCMPL rings
	 */
	for (i = 0; i < edma_hw.txcmpl_rings; i++) {
		err = request_irq(edma_hw.txcmpl_intr[i],
				  edma_handle_irq, IRQF_SHARED,
				  "edma_txcmpl", (void *)edma_hw.pdev);
		if (err) {
			pr_debug("TXCMPL ring IRQ:%d request failed\n",
					edma_hw.txcmpl_intr[i]);
			return -1;

		}
	}

	/*
	 * Request IRQ for RXFILL rings
	 */
	for (i = 0; i < edma_hw.rxfill_rings; i++) {
		err = request_irq(edma_hw.rxfill_intr[i],
				  edma_handle_irq, IRQF_SHARED,
				  "edma_rxfill", (void *)edma_hw.pdev);
		if (err) {
			pr_debug("RXFILL ring IRQ:%d request failed\n",
					edma_hw.rxfill_intr[i]);
			goto rx_fill_ring_intr_req_fail;
		}
	}

	/*
	 * Request IRQ for RXDESC rings
	 */
	for (i = 0; i < edma_hw.rxdesc_rings; i++) {
		err = request_irq(edma_hw.rxdesc_intr[i],
				  edma_handle_irq, IRQF_SHARED,
				  "edma_rxdesc", (void *)edma_hw.pdev);
		if (err) {
			pr_debug("RXDESC ring IRQ:%d request failed\n",
					edma_hw.rxdesc_intr[i]);
			goto rx_desc_ring_intr_req_fail;
		}
	}

	/*
	 * Request Misc IRQ
	 */
	err = request_irq(edma_hw.misc_intr, edma_handle_misc_irq,
			  IRQF_SHARED, "edma_misc",
			  (void *)edma_hw.pdev);
	if (err) {
		pr_debug("MISC IRQ:%d request failed\n",
				edma_hw.misc_intr);
		goto misc_intr_req_fail;
	}

	return 0;

misc_intr_req_fail:

	/*
	 * Free IRQ for RXDESC rings
	 */
	for (i = 0; i < edma_hw.rxdesc_rings; i++) {
		synchronize_irq(edma_hw.rxdesc_intr[i]);
		free_irq(edma_hw.rxdesc_intr[i],
				(void *)&(edma_hw.pdev)->dev);
	}

rx_desc_ring_intr_req_fail:

	/*
	 * Free IRQ for RXFILL rings
	 */
	for (i = 0; i < edma_hw.rxfill_rings; i++) {
		synchronize_irq(edma_hw.rxfill_intr[i]);
		free_irq(edma_hw.rxfill_intr[i],
				(void *)&(edma_hw.pdev)->dev);
	}

rx_fill_ring_intr_req_fail:

	/*
	 * Free IRQ for TXCMPL rings
	 */
	for (i = 0; i < edma_hw.txcmpl_rings; i++) {

		synchronize_irq(edma_hw.txcmpl_intr[i]);
		free_irq(edma_hw.txcmpl_intr[i],
				(void *)&(edma_hw.pdev)->dev);
	}

	return -1;
}

/*
 * edma_register_netdevice()
 *	Register netdevice with EDMA
 */
static int edma_register_netdevice(struct net_device *netdev, uint32_t macid)
{
	if (!netdev) {
		pr_info("nss_dp_edma: Invalid netdev pointer %px\n", netdev);
		return -EINVAL;
	}

	if ((macid < EDMA_START_GMACS) || (macid > EDMA_MAX_GMACS)) {
		netdev_dbg(netdev, "nss_dp_edma: Invalid macid(%d) for %s\n",
			macid, netdev->name);
		return -EINVAL;
	}

	netdev_info(netdev, "nss_dp_edma: Registering netdev %s(qcom-id:%d) with EDMA\n",
		netdev->name, macid);

	/*
	 * We expect 'macid' to correspond to ports numbers on
	 * IPQ807x. These begin from '1' and hence we subtract
	 * one when using it as an array index.
	 */
	edma_hw.netdev_arr[macid - 1] = netdev;

	/*
	 * NAPI add
	 */
	if (!edma_hw.napi_added) {
		netif_napi_add(netdev, &edma_hw.napi, edma_napi,
				EDMA_NAPI_WORK);
		/*
		 * Register the interrupt handlers and enable interrupts
		 */
		if (edma_irq_init() < 0)
			return -EINVAL;

		edma_hw.napi_added = 1;
	}

	return 0;
}

/*
 * edma_if_init()
 */

static int edma_if_init(struct nss_dp_data_plane_ctx *dpc)
{

	struct net_device *netdev = dpc->dev;
	struct nss_dp_dev *dp_dev = (struct nss_dp_dev *)netdev_priv(netdev);
	int ret = 0;

	/*
	 * Register the netdev
	 */
	ret = edma_register_netdevice(netdev, dp_dev->macid);
	if (ret) {
		netdev_dbg(netdev,
				"Error registering netdevice with EDMA %s\n",
				netdev->name);
		return NSS_DP_FAILURE;
	}

	/*
	 * Headroom needed for Tx preheader
	 */
	netdev->needed_headroom += EDMA_TX_PREHDR_SIZE;

	return NSS_DP_SUCCESS;
}

/*
 * nss_dp_edma_ops
 */
struct nss_dp_data_plane_ops nss_dp_edma_ops = {
	.init		= edma_if_init,
	.open		= edma_if_open,
	.close		= edma_if_close,
	.link_state	= edma_if_link_state,
	.mac_addr	= edma_if_mac_addr,
	.change_mtu	= edma_if_change_mtu,
	.xmit		= edma_if_xmit,
	.set_features	= edma_if_set_features,
	.pause_on_off	= edma_if_pause_on_off,
	.vsi_assign	= edma_if_vsi_assign,
	.vsi_unassign	= edma_if_vsi_unassign,
#ifdef CONFIG_RFS_ACCEL
	.rx_flow_steer	= edma_if_rx_flow_steer,
#endif
	.deinit		= edma_if_deinit,
};

/*
 * edma_of_get_pdata()
 *	Read the device tree details for EDMA
 */
static int edma_of_get_pdata(struct resource *edma_res)
{
	/*
	 * Find EDMA node in device tree
	 */
	edma_hw.device_node = of_find_node_by_name(NULL,
				EDMA_DEVICE_NODE_NAME);
	if (!edma_hw.device_node) {
		pr_warn("EDMA device tree node (%s) not found\n",
				EDMA_DEVICE_NODE_NAME);
		return -EINVAL;
	}

	/*
	 * Get EDMA device node
	 */
	edma_hw.pdev = of_find_device_by_node(edma_hw.device_node);
	if (!edma_hw.pdev) {
		pr_warn("Platform device for node %px(%s) not found\n",
				edma_hw.device_node,
				(edma_hw.device_node)->name);
		return -EINVAL;
	}

	/*
	 * Get EDMA register resource
	 */
	if (of_address_to_resource(edma_hw.device_node, 0, edma_res) != 0) {
		pr_warn("Unable to get register address for edma device: "
			  EDMA_DEVICE_NODE_NAME"\n");
		return -EINVAL;
	}

	/*
	 * Get id of first TXDESC ring
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,txdesc-ring-start",
				      &edma_hw.txdesc_ring_start) != 0) {
		pr_warn("Read error 1st TXDESC ring (txdesc_ring_start)\n");
		return -EINVAL;
	}

	/*
	 * Get number of TXDESC rings
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,txdesc-rings",
				 &edma_hw.txdesc_rings) != 0) {
		pr_warn("Unable to read number of txdesc rings.\n");
		return -EINVAL;
	}
	edma_hw.txdesc_ring_end = edma_hw.txdesc_ring_start +
					edma_hw.txdesc_rings;

	/*
	 * Get id of first TXCMPL ring
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,txcmpl-ring-start",
				&edma_hw.txcmpl_ring_start) != 0) {
		pr_warn("Read error 1st TXCMPL ring (txcmpl_ring_start)\n");
		return -EINVAL;
	}

	/*
	 * Get number of TXCMPL rings
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,txcmpl-rings",
				&edma_hw.txcmpl_rings) != 0) {
		pr_warn("Unable to read number of txcmpl rings.\n");
		return -EINVAL;
	}
	edma_hw.txcmpl_ring_end = edma_hw.txcmpl_ring_start +
					edma_hw.txcmpl_rings;

	/*
	 * Get id of first RXFILL ring
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,rxfill-ring-start",
				      &edma_hw.rxfill_ring_start) != 0) {
		pr_warn("Read error 1st RXFILL ring (rxfill-ring-start)\n");
		return -EINVAL;
	}

	/*
	 * Get number of RXFILL rings
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,rxfill-rings",
					&edma_hw.rxfill_rings) != 0) {
		pr_warn("Unable to read number of rxfill rings.\n");
		return -EINVAL;
	}
	edma_hw.rxfill_ring_end = edma_hw.rxfill_ring_start +
					edma_hw.rxfill_rings;

	/*
	 * Get id of first RXDESC ring
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,rxdesc-ring-start",
				      &edma_hw.rxdesc_ring_start) != 0) {
		pr_warn("Read error 1st RXDESC ring (rxdesc-ring-start)\n");
		return -EINVAL;
	}

	/*
	 * Get number of RXDESC rings
	 */
	if (of_property_read_u32(edma_hw.device_node, "qcom,rxdesc-rings",
					&edma_hw.rxdesc_rings) != 0) {
		pr_warn("Unable to read number of rxdesc rings.\n");
		return -EINVAL;
	}
	edma_hw.rxdesc_ring_end = edma_hw.rxdesc_ring_start +
					edma_hw.rxdesc_rings;

	return 0;
}

/*
 * edma_init()
 *	EDMA init
 */
int edma_init(void)
{
	int ret = 0;
	struct resource res_edma;

	/*
	 * Get all the DTS data needed
	 */
	if (edma_of_get_pdata(&res_edma) < 0) {
		pr_warn("Unable to get EDMA DTS data.\n");
		return -EINVAL;
	}

	/*
	 * Request memory region for EDMA registers
	 */
	edma_hw.reg_resource = request_mem_region(res_edma.start,
				resource_size(&res_edma),
				EDMA_DEVICE_NODE_NAME);
	if (!edma_hw.reg_resource) {
		pr_warn("Unable to request EDMA register memory.\n");
		return -EFAULT;
	}

	/*
	 * Remap register resource
	 */
	edma_hw.reg_base = ioremap_nocache((edma_hw.reg_resource)->start,
				resource_size(edma_hw.reg_resource));
	if (!edma_hw.reg_base) {
		pr_warn("Unable to remap EDMA register memory.\n");
		ret = -EFAULT;
		goto edma_init_remap_fail;
	}

	if (edma_hw_init(&edma_hw) != 0) {
		ret = -EFAULT;
		goto edma_init_hw_init_fail;
	}

	platform_set_drvdata(edma_hw.pdev, (void *)&edma_hw);

	edma_hw.napi_added = 0;

	return 0;

edma_init_hw_init_fail:
	iounmap(edma_hw.reg_base);

edma_init_remap_fail:
	release_mem_region((edma_hw.reg_resource)->start,
			   resource_size(edma_hw.reg_resource));
	return ret;
}

/*
 * edma_disable_port()
 * 	EDMA disable port
 */
static void edma_disable_port(void)
{
	edma_reg_write(EDMA_REG_PORT_CTRL, EDMA_DISABLE);
}

/*
 * edma_cleanup()
 *	EDMA cleanup
 */
void edma_cleanup(bool is_dp_override)
{
	int i;
	struct edma_txcmpl_ring *txcmpl_ring = NULL;
	struct edma_rxdesc_ring *rxdesc_ring = NULL;

	/*
	 * The cleanup can happen from data plane override
	 * or from module_exit, we want to cleanup only once
	 */
	if (!edma_hw.edma_initialized) {
		/*
		 * Disable EDMA only at module exit time, since NSS firmware
		 * depends on this setting.
		 */
		if (!is_dp_override) {
			edma_disable_port();
		}
		return;
	}

	/*
	 * Disable Rx rings used by this driver
	 */
	for (i = edma_hw.rxdesc_ring_start; i < edma_hw.rxdesc_ring_end; i++)
		edma_reg_write(EDMA_REG_RXDESC_CTRL(i), EDMA_RING_DISABLE);

	/*
	 * Disable Tx rings used by this driver
	 */
	for (i = edma_hw.txdesc_ring_start; i < edma_hw.txdesc_ring_end; i++) {
		txcmpl_ring = &edma_hw.txcmpl_ring[i];
		edma_reg_write(EDMA_REG_TXDESC_CTRL(i),
				EDMA_RING_DISABLE);
	}

	/*
	 * Disable RxFill Rings used by this driver
	 */
	for (i = edma_hw.rxfill_ring_start; i < edma_hw.rxfill_ring_end; i++)
		edma_reg_write(EDMA_REG_RXFILL_RING_EN(i), EDMA_RING_DISABLE);

	/*
	 * Clear interrupt mask
	 */
	for (i = 0; i < edma_hw.rxdesc_rings; i++) {
		rxdesc_ring = &edma_hw.rxdesc_ring[i];
		edma_reg_write(EDMA_REG_RXDESC_INT_MASK(rxdesc_ring->id),
			       EDMA_MASK_INT_CLEAR);
	}

	for (i = 0; i < edma_hw.txcmpl_rings; i++) {
		txcmpl_ring = &edma_hw.txcmpl_ring[i];
		edma_reg_write(EDMA_REG_TX_INT_MASK(txcmpl_ring->id),
			       EDMA_MASK_INT_CLEAR);
	}

	edma_reg_write(EDMA_REG_MISC_INT_MASK, EDMA_MASK_INT_CLEAR);
	/*
	 * Remove interrupt handlers and NAPI
	 */
	if (edma_hw.napi_added) {

		/*
		 * Free IRQ for TXCMPL rings
		 */
		for (i = 0; i < edma_hw.txcmpl_rings; i++) {
			synchronize_irq(edma_hw.txcmpl_intr[i]);
			free_irq(edma_hw.txcmpl_intr[i],
					(void *)(edma_hw.pdev));
		}

		/*
		 * Free IRQ for RXFILL rings
		 */
		for (i = 0; i < edma_hw.rxfill_rings; i++) {
			synchronize_irq(edma_hw.rxfill_intr[i]);
			free_irq(edma_hw.rxfill_intr[i],
					(void *)(edma_hw.pdev));
		}

		/*
		 * Free IRQ for RXDESC rings
		 */
		for (i = 0; i < edma_hw.rxdesc_rings; i++) {
			synchronize_irq(edma_hw.rxdesc_intr[i]);
			free_irq(edma_hw.rxdesc_intr[i],
					(void *)(edma_hw.pdev));
		}

		/*
		 * Free Misc IRQ
		 */
		synchronize_irq(edma_hw.misc_intr);
		free_irq(edma_hw.misc_intr, (void *)(edma_hw.pdev));

		netif_napi_del(&edma_hw.napi);
		edma_hw.napi_added = 0;
	}

	/*
	 * Disable EDMA only at module exit time, since NSS firmware
	 * depends on this setting.
	 */
	if (!is_dp_override) {
		edma_disable_port();
	}

	/*
	 * cleanup rings and free
	 */
	edma_cleanup_rings(&edma_hw);
	iounmap(edma_hw.reg_base);
	release_mem_region((edma_hw.reg_resource)->start,
			   resource_size(edma_hw.reg_resource));

	/*
	 * Mark initialize false, so that we do not
	 * try to cleanup again
	 */
	edma_hw.edma_initialized = false;
}
