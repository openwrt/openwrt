/*
 **************************************************************************
 * Copyright (c) 2013-2021, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * nss_core.c
 *	NSS driver core APIs source file.
 */

#include "nss_core.h"
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/of.h>
#include <nss_hal.h>
#include <net/dst.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(5, 0, 0))
#include <net/netfilter/br_netfilter.h>
#else
#include <linux/netfilter_bridge.h>
#endif
#endif
#include <linux/etherdevice.h>
#include "nss_tx_rx_common.h"
#include "nss_data_plane.h"

#define NSS_CORE_JUMBO_LINEAR_BUF_SIZE 128

#if (NSS_SKB_REUSE_SUPPORT == 1)
/*
 * We have validated the skb recycling code within the NSS for the
 * following kernel versions. Before enabling the driver in new kernels,
 * the skb recycle code must be checked against Linux skb handling.
 *
 * Tested on: 3.4, 3.10, 3.14, 3.18, 4.4 and 5.4
 */
#if (!( \
(((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(3, 5, 0)))) || \
(((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(3, 15, 0)))) || \
(((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0)))) || \
(((LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)))) || \
(((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)))) || \
(((LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(5, 5, 0))))))
#error "Check skb recycle code in this file to match Linux version"
#endif

static atomic_t max_reuse = ATOMIC_INIT(PAGE_SIZE);

#endif /* NSS_SKB_REUSE_SUPPORT */

static int max_ipv4_conn = NSS_DEFAULT_NUM_CONN;
module_param(max_ipv4_conn, int, S_IRUGO);
MODULE_PARM_DESC(max_ipv4_conn, "Max number of IPv4 connections");

static int max_ipv6_conn = NSS_DEFAULT_NUM_CONN;
module_param(max_ipv6_conn, int, S_IRUGO);
MODULE_PARM_DESC(max_ipv6_conn, "Max number of IPv6 connections");

bool pn_mq_en = false;
module_param(pn_mq_en, bool, S_IRUGO);
MODULE_PARM_DESC(pn_mq_en, "Enable pnode ingress QoS");

uint16_t pn_qlimits[NSS_MAX_NUM_PRI] = {[0 ... NSS_MAX_NUM_PRI - 1] = NSS_DEFAULT_QUEUE_LIMIT};
module_param_array(pn_qlimits, short, NULL, 0);
MODULE_PARM_DESC(pn_qlimits, "Queue limit per queue");

/*
 * Atomic variables to control jumbo_mru & paged_mode
 */
static atomic_t jumbo_mru;
static atomic_t paged_mode;

/*
 * nss_core_update_max_ipv4_conn()
 *	Update the maximum number of configured IPv4 connections
 */
void nss_core_update_max_ipv4_conn(int conn)
{
	max_ipv4_conn = conn;
}

/*
 * nss_core_update_max_ipv6_conn()
 *	Update the maximum number of configured IPv6 connections
 */
void nss_core_update_max_ipv6_conn(int conn)
{
	max_ipv6_conn = conn;
}

#if (NSS_SKB_REUSE_SUPPORT == 1)
/*
 * nss_core_set_max_reuse()
 *	Set the max_reuse to the specified value
 */
void nss_core_set_max_reuse(int max)
{
	atomic_set(&max_reuse, max);
}

/*
 * nss_core_get_max_reuse()
 *	Does an atomic read of max_reuse
 */
int nss_core_get_max_reuse(void)
{
	return atomic_read(&max_reuse);
}

/*
 * nss_core_get_min_reuse()
 *	Return min reuse size
 */
uint32_t nss_core_get_min_reuse(struct nss_ctx_instance *nss_ctx)
{
	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	return nss_ctx->max_buf_size;
}
#endif /* NSS_SKB_REUSE_SUPPORT */

/*
 * nss_core_set_jumbo_mru()
 *	Set the jumbo_mru to the specified value
 */
void nss_core_set_jumbo_mru(int jumbo)
{
	atomic_set(&jumbo_mru, jumbo);

#if (NSS_SKB_REUSE_SUPPORT == 1)
	if (jumbo > nss_core_get_max_reuse())
		nss_core_set_max_reuse(ALIGN(jumbo * 2, PAGE_SIZE));
#endif
}

/*
 * nss_core_get_jumbo_mru()
 *	Does an atomic read of jumbo_mru
 */
int nss_core_get_jumbo_mru(void)
{
	return atomic_read(&jumbo_mru);
}

/*
 * nss_core_set_paged_mode()
 *	Set the paged_mode to the specified value
 */
void nss_core_set_paged_mode(int mode)
{
	atomic_set(&paged_mode, mode);
}

/*
 * nss_core_get_paged_mode()
 *	Does an atomic read of paged_mode
 */
int nss_core_get_paged_mode(void)
{
	return atomic_read(&paged_mode);
}

/*
 * nss_core_register_msg_handler()
 *	Register a msg callback per interface number. One per interface.
 */
uint32_t nss_core_register_msg_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface, nss_if_rx_msg_callback_t msg_cb)
{
	nss_assert(msg_cb != NULL);

	/*
	 * Validate interface id
	 */
	if (interface >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Error - Interface %d not Supported\n", interface);
		return NSS_CORE_STATUS_FAILURE;
	}

	/*
	 * Check if already registered
	 */
	if (nss_ctx->nss_rx_interface_handlers[interface].msg_cb) {
		nss_warning("Error - Duplicate Interface CB Registered for interface %d\n", interface);
		return NSS_CORE_STATUS_FAILURE;
	}

	nss_ctx->nss_rx_interface_handlers[interface].msg_cb = msg_cb;

	return NSS_CORE_STATUS_SUCCESS;
}

/*
 * nss_core_unregister_msg_handler()
 *	Unregister a msg callback per interface number.
 */
uint32_t nss_core_unregister_msg_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface)
{
	/*
	 * Validate interface id
	 */
	if (interface >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Error - Interface %d not Supported\n", interface);
		return NSS_CORE_STATUS_FAILURE;
	}

	nss_ctx->nss_rx_interface_handlers[interface].msg_cb = NULL;

	return NSS_CORE_STATUS_SUCCESS;
}

/*
 * nss_core_register_handler()

--	Register a callback per interface code. Only one per interface.
 */
uint32_t nss_core_register_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface, nss_core_rx_callback_t cb, void *app_data)
{
	nss_assert(cb != NULL);

	/*
	 * Validate interface id
	 */
	if (interface >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Error - Interface %d not Supported\n", interface);
		return NSS_CORE_STATUS_FAILURE;
	}

	/*
	 * Check if already registered
	 */
	if (nss_ctx->nss_rx_interface_handlers[interface].cb != NULL) {
		nss_warning("Error - Duplicate Interface CB Registered for interface %d\n", interface);
		return NSS_CORE_STATUS_FAILURE;
	}

	nss_ctx->nss_rx_interface_handlers[interface].cb = cb;
	nss_ctx->nss_rx_interface_handlers[interface].app_data = app_data;

	return NSS_CORE_STATUS_SUCCESS;
}

/*
 * nss_core_unregister_handler()
 *	Unegister a callback per interface code.
 */
uint32_t nss_core_unregister_handler(struct nss_ctx_instance *nss_ctx, uint32_t interface)
{
	/*
	 * Validate interface id
	 */
	if (interface >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Error - Interface %d not Supported\n", interface);
		return NSS_CORE_STATUS_FAILURE;
	}

	nss_ctx->nss_rx_interface_handlers[interface].cb = NULL;
	nss_ctx->nss_rx_interface_handlers[interface].app_data = NULL;

	return NSS_CORE_STATUS_SUCCESS;
}

/*
 * nss_core_set_subsys_dp_type()
 *	Set the type for the datapath subsystem
 */
void nss_core_set_subsys_dp_type(struct nss_ctx_instance *nss_ctx, struct net_device *ndev, uint32_t if_num, uint32_t type)
{
	struct nss_subsystem_dataplane_register *reg;

	/*
	 * Check that interface number is in range.
	 */
	BUG_ON(if_num >= NSS_MAX_NET_INTERFACES);

	reg = &nss_ctx->subsys_dp_register[if_num];

	/*
	 * Check if there is already a subsystem registered at this interface number.
	 */
	BUG_ON(reg->ndev && reg->ndev != ndev);

	reg->type = type;
}

/*
 * nss_core_register_subsys_dp()
 *	Registers a netdevice and associated information at a given interface.
 *
 * Can also be used to update an existing registry if the provided net_device
 * is equal to the one already registered. Will fail if there is already
 * a net_device registered to the interface not equal to the one provided,
 * or if the interface number is out of range.
 */
void nss_core_register_subsys_dp(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					nss_phys_if_rx_callback_t cb,
					nss_phys_if_rx_ext_data_callback_t ext_cb,
					void *app_data, struct net_device *ndev,
					uint32_t features)
{
	struct nss_subsystem_dataplane_register *reg;

	/*
	 * Check that interface number is in range.
	 */
	BUG_ON(if_num >= NSS_MAX_NET_INTERFACES);

	reg = &nss_ctx->subsys_dp_register[if_num];

	/*
	 * Check if there is already a subsystem registered at this interface number.
	 */
	BUG_ON(reg->ndev && reg->ndev != ndev);

	reg->cb = cb;
	reg->ext_cb = ext_cb;
	reg->app_data = app_data;
	reg->ndev = ndev;
	reg->features = features;
}

/*
 * nss_core_unregister_subsys_dp()
 *	Unregisters the netdevice at the given interface.
 *
 * Fails if the interface number is not valid.
 */
void nss_core_unregister_subsys_dp(struct nss_ctx_instance *nss_ctx, uint32_t if_num)
{
	struct nss_subsystem_dataplane_register *reg;

	/*
	 * Check that interface number is in range.
	 */
	BUG_ON(if_num >= NSS_MAX_NET_INTERFACES);

	reg = &nss_ctx->subsys_dp_register[if_num];

	reg->cb = NULL;
	reg->ext_cb = NULL;
	reg->app_data = NULL;
	reg->ndev = NULL;
	reg->features = 0;
	reg->type = 0;
}

/*
 * nss_core_handle_nss_status_pkt()
 *	Handle the metadata/status packet.
 */
void nss_core_handle_nss_status_pkt(struct nss_ctx_instance *nss_ctx, struct sk_buff *nbuf)
{
	struct nss_cmn_msg *ncm;
	uint32_t expected_version = NSS_HLOS_MESSAGE_VERSION;
	nss_core_rx_callback_t cb;
	void *app_data;
	uint16_t nss_if;

	if (skb_shinfo(nbuf)->nr_frags > 0) {
		ncm = (struct nss_cmn_msg *)skb_frag_address(&skb_shinfo(nbuf)->frags[0]);
	} else {
		ncm = (struct nss_cmn_msg *)nbuf->data;
	}

	/*
	 * Save NSS interface number in local variable
	 */
	nss_if = ncm->interface;

	/*
	 * Check for version number
	 */
	if (ncm->version != expected_version) {
		nss_warning("%px: Message %d for interface %d received with invalid version %d, expected version %d",
							nss_ctx, ncm->type, nss_if, ncm->version, expected_version);
		return;
	}

	/*
	 * Validate message size
	 */
	if (ncm->len > nbuf->len) {
		nss_warning("%px: Message %d for interface %d received with invalid length %d, expected length %d",
							nss_ctx, ncm->type, nss_if, nbuf->len, ncm->len);
		return;
	}

	/*
	 * Check for validity of interface number
	 */
	if (nss_if >= NSS_MAX_NET_INTERFACES) {
		nss_warning("%px: Message %d received with invalid interface number %d", nss_ctx, ncm->type, nss_if);
		return;
	}

	cb = nss_ctx->nss_rx_interface_handlers[nss_if].cb;
	app_data = nss_ctx->nss_rx_interface_handlers[nss_if].app_data;

	if (!cb) {
		nss_warning("%px: Callback not registered for interface %d", nss_ctx, nss_if);
		return;
	}

	cb(nss_ctx, ncm, app_data);

	if (ncm->interface != nss_if) {
		nss_warning("%px: Invalid NSS I/F %d expected %d", nss_ctx, ncm->interface, nss_if);
	}

	return;
}

/*
 * nss_core_handle_nss_crypto_pkt()
 *	Handles crypto packet.
 */
static void nss_core_handle_crypto_pkt(struct nss_ctx_instance *nss_ctx, unsigned int interface_num,
			struct sk_buff *nbuf, struct napi_struct *napi)
{
	struct nss_subsystem_dataplane_register *subsys_dp_reg = &nss_ctx->subsys_dp_register[interface_num];
	nss_phys_if_rx_callback_t cb;
	struct net_device *ndev;

	ndev = subsys_dp_reg->ndev;
	cb = subsys_dp_reg->cb;
	if (likely(cb)) {
		cb(ndev, nbuf, napi);
		return;
	}

	dev_kfree_skb_any(nbuf);
	return;
}

/*
 * nss_soc_mem_info()
 *	Getting DDR information for NSS SoC
 */
static uint32_t nss_soc_mem_info(void)
{
	struct device_node *node;
	struct device_node *snode;
	int addr_cells;
	int size_cells;
	int n_items;
	uint32_t nss_msize = 8 << 20;	/* default: 8MB */
	const __be32 *ppp;

	node = of_find_node_by_name(NULL, "reserved-memory");
	if (!node) {
		nss_info_always("reserved-memory not found\n");
		return nss_msize;
	}

	ppp = (__be32 *)of_get_property(node, "#address-cells", NULL);
	addr_cells = ppp ? be32_to_cpup(ppp) : 2;
	nss_info("%px addr cells %d\n", ppp, addr_cells);
	ppp = (__be32 *)of_get_property(node, "#size-cells", NULL);
	size_cells = ppp ? be32_to_cpup(ppp) : 2;
	nss_info("%px size cells %d\n", ppp, size_cells);

	for_each_child_of_node(node, snode) {
		/*
		 * compare (snode->full_name, "/reserved-memory/nss@40000000") may be safer
		 */
		nss_info("%px snode %s fn %s\n", snode, snode->name, snode->full_name);
		if (strcmp(snode->name, "nss") == 0)
			break;
	}
	of_node_put(node);
	if (!snode) {
		nss_info_always("nss@node not found: needed to determine NSS reserved DDR\n");
		return nss_msize;
	}

	ppp = (__be32 *)of_get_property(snode, "reg", &n_items);
	if (ppp) {
		n_items /= sizeof(ppp[0]);
		nss_msize = be32_to_cpup(ppp + addr_cells + size_cells - 1);
		nss_info_always("addr/size storage words %d %d # words %d in DTS, ddr size %x\n",
				addr_cells, size_cells, n_items, nss_msize);
	}
	of_node_put(snode);
	return nss_msize;
}

/*
 * nss_get_ddr_info()
 *	get DDR start address and size from device tree.
 */
static void nss_get_ddr_info(struct nss_mmu_ddr_info *mmu, char *name)
{
	__be32 avail_ddr;
	long cached;
	struct sysinfo vals;
	struct device_node *node;

	si_meminfo(&vals);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
	cached = global_page_state(NR_FILE_PAGES);
#else
	cached = global_node_page_state(NR_FILE_PAGES);
#endif

	avail_ddr = (vals.totalram + cached + vals.sharedram) * vals.mem_unit;
	mmu->num_active_cores = nss_top_main.num_nss;

	/*
	 * Since "memory" has not been used by anyone, the format is not final.
	 * Three (3) possible formats available: one of 1 or 2 will be final.
	 * 1)	item_size	stating_address	DDR_size	: odd # items
	 * 2)	stating_address DDR_size	# 32-bit each; total 2 words
	 * 3)	stating_address DDR_size	# 64-bit each; total 4 words
	 */
	node = of_find_node_by_name(NULL, name);
	if (node) {
		int isize = 0;
		int n_items;
		const __be32 *ppp = (__be32 *)of_get_property(node, "reg", &n_items);

		n_items /= sizeof(ppp[0]);
		nss_info_always("node size %d # items %d\n",
				of_n_size_cells(node), n_items);
		if (ppp) {
			if (n_items & 1) {	/* case 1 */
				isize = be32_to_cpup(ppp);
				if (isize == 1)
					goto case2;
				if (isize == 2)
					goto case3;
				n_items = 0;
			} else if (n_items == 2) {
case2:
				mmu->start_address = be32_to_cpup(ppp + isize);
				mmu->ddr_size = be32_to_cpup(&ppp[isize + 1]);
			} else if (n_items == 4) {
case3:
				if (!ppp[isize] && !ppp[isize * 2]) {
					if (isize)
						isize = 1;
					mmu->start_address = be32_to_cpup(ppp + isize + 1);
					mmu->ddr_size = be32_to_cpup(ppp + isize + 3);
				} else
					n_items = 0;
			} else
				n_items = 0;
			if (n_items) {
				of_node_put(node);
				nss_info_always("%s: %x %u (avl %u) items %d active_cores %d\n",
					name, mmu->start_address, mmu->ddr_size,
					avail_ddr, n_items, mmu->num_active_cores);
				/*
				 * if DTS mechanism goes wrong, use available
				 * DDR and round it up to 64MB for maximum DDR.
				 */
				if (avail_ddr > mmu->ddr_size)
					mmu->ddr_size = (avail_ddr + (63 << 20))
							& (~63 << 20);
				return;
			}
		}
		of_node_put(node);
		nss_info_always("incorrect memory info %px items %d\n",
			ppp, n_items);
	}

	/*
	 * boilerplate for setting customer values;
	 * start_address = 0 will not change default start address
	 * set in NSS FW (likely 0x4000_0000)
	 * total available RAM + 16 MB NSS FW DDR + ~31 MB kernel mem
	 * we round it up by 128MB to cover potential NSS DDR increase
	 * and a slightly large holes.
	 * The size can be changed to a fixed value as DTS, but simplier.
	 * mmu->ddr_size = 1024 << 20
	 */
	mmu->start_address = 0;
	mmu->ddr_size = (avail_ddr + (127 << 20)) & (~127 << 20);
	nss_info_always("RAM pages fr %lu buf %lu cached %lu %lu : %lu %u\n",
			vals.freeram, vals.bufferram, cached, vals.sharedram,
			vals.totalram, mmu->ddr_size);
}

/*
 * nss_send_ddr_info()
 *	Send DDR info to NSS
 */
static void nss_send_ddr_info(struct nss_ctx_instance *nss_own)
{
	struct nss_n2h_msg nnm;
	struct nss_cmn_msg *ncm = &nnm.cm;
	uint32_t ret;
	nss_info("%px: send DDR info\n", nss_own);

	nss_cmn_msg_init(ncm, NSS_N2H_INTERFACE, NSS_TX_DDR_INFO_VIA_N2H_CFG,
			sizeof(struct nss_mmu_ddr_info), NULL, NULL);

	nss_get_ddr_info(&nnm.msg.mmu, "memory");
	nnm.msg.mmu.nss_ddr_size = nss_soc_mem_info();

	ret = nss_core_send_cmd(nss_own, &nnm, sizeof(nnm), NSS_NBUF_PAYLOAD_SIZE);
	if (ret != NSS_TX_SUCCESS) {
		nss_info_always("%px: Failed to send DDR info for core %d\n", nss_own, nss_own->id);
	}
}

/*
 * nss_core_cause_to_queue()
 *	Map interrupt cause to queue id
 */
static inline uint16_t nss_core_cause_to_queue(uint16_t cause)
{
	if (likely(cause == NSS_N2H_INTR_DATA_QUEUE_0)) {
		return NSS_IF_N2H_DATA_QUEUE_0;
	}

	if (likely(cause == NSS_N2H_INTR_DATA_QUEUE_1)) {
		return NSS_IF_N2H_DATA_QUEUE_1;
	}

	if (likely(cause == NSS_N2H_INTR_DATA_QUEUE_2)) {
		return NSS_IF_N2H_DATA_QUEUE_2;
	}

	if (likely(cause == NSS_N2H_INTR_DATA_QUEUE_3)) {
		return NSS_IF_N2H_DATA_QUEUE_3;
	}

	if (likely(cause == NSS_N2H_INTR_EMPTY_BUFFER_QUEUE)) {
		return NSS_IF_N2H_EMPTY_BUFFER_RETURN_QUEUE;
	}

	/*
	 * There is no way we can reach here as cause was already identified to be related to valid queue
	 */
	nss_assert(0);
	return 0;
}

/*
 * nss_dump_desc()
 *	Prints descriptor data
 */
static inline void nss_dump_desc(struct nss_ctx_instance *nss_ctx, struct n2h_descriptor *desc)
{
	printk("bad descriptor dump for nss core = %d\n", nss_ctx->id);
	printk("\topaque = %px\n", (void *)desc->opaque);
	printk("\tinterface = %d\n", desc->interface_num);
	printk("\tbuffer_type = %d\n", desc->buffer_type);
	printk("\tbit_flags = %x\n", desc->bit_flags);
	printk("\tbuffer_addr = %x\n", desc->buffer);
	printk("\tbuffer_len = %d\n", desc->buffer_len);
	printk("\tpayload_offs = %d\n", desc->payload_offs);
	printk("\tpayload_len = %d\n", desc->payload_len);
	printk("\tpri = %d\n", desc->pri);
}

/*
 * nss_core_skb_needs_linearize()
 *	Looks at if this skb needs to be linearized or not.
 */
static inline int nss_core_skb_needs_linearize(struct sk_buff *skb, uint32_t features)
{
	return ((skb_has_frag_list(skb) &&
				!(features & NETIF_F_FRAGLIST)) ||
			(skb_shinfo(skb)->nr_frags &&
				!(features & NETIF_F_SG)));
}

/*
 * nss_core_handle_bounced_pkt()
 *	Bounced packet is returned from an interface/bridge bounce operation.
 *
 * Return the skb to the registrant.
 */
static inline void nss_core_handle_bounced_pkt(struct nss_ctx_instance *nss_ctx,
						struct nss_shaper_bounce_registrant *reg,
						struct sk_buff *nbuf)
{
	void *app_data;
	struct module *owner;
	nss_shaper_bounced_callback_t bounced_callback;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	spin_lock_bh(&nss_top->lock);

	/*
	 * Do we have a registrant?
	 */
	if (!reg->registered) {
		spin_unlock_bh(&nss_top->lock);
		dev_kfree_skb_any(nbuf);
		return;
	}

	/*
	 * Get handle to the owning registrant
	 */
	bounced_callback = reg->bounced_callback;
	app_data = reg->app_data;
	owner = reg->owner;

	/*
	 * Callback is active, unregistration is not permitted while this is in progress
	 */
	reg->callback_active = true;
	spin_unlock_bh(&nss_top->lock);
	if (!try_module_get(owner)) {
		spin_lock_bh(&nss_top->lock);
		reg->callback_active = false;
		spin_unlock_bh(&nss_top->lock);
		dev_kfree_skb_any(nbuf);
		return;
	}

	/*
	 * Pass bounced packet back to registrant
	 */
	bounced_callback(app_data, nbuf);
	spin_lock_bh(&nss_top->lock);
	reg->callback_active = false;
	spin_unlock_bh(&nss_top->lock);
	module_put(owner);
}

/*
 * nss_core_handle_virt_if_pkt()
 *	Handle packet destined to virtual interface.
 */
static inline void nss_core_handle_virt_if_pkt(struct nss_ctx_instance *nss_ctx,
						unsigned int interface_num,
						struct sk_buff *nbuf)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_subsystem_dataplane_register *subsys_dp_reg = &nss_ctx->subsys_dp_register[interface_num];
	struct net_device *ndev = NULL;

	uint32_t xmit_ret;
	uint16_t queue_offset = 0;

	NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_RX_VIRTUAL]);

	/*
	 * Checksum is already done by NSS for packets forwarded to virtual interfaces
	 */
	nbuf->ip_summed = CHECKSUM_NONE;

	/*
	 * Obtain net_device pointer
	 */
	ndev = subsys_dp_reg->ndev;
	if (unlikely(!ndev)) {
		nss_warning("%px: Received packet for unregistered virtual interface %d",
			nss_ctx, interface_num);

		/*
		 * NOTE: The assumption is that gather support is not
		 * implemented in fast path and hence we can not receive
		 * fragmented packets and so we do not need to take care
		 * of freeing a fragmented packet
		 */
		dev_kfree_skb_any(nbuf);
		return;
	}

	/*
	 * TODO: Need to ensure the ndev is not removed before we take dev_hold().
	 */
	dev_hold(ndev);
	nbuf->dev = ndev;

	/*
	 * Linearize the skb if needed
	 *
	 * Mixing up non linear check with in nss_core_skb_needs_linearize causes
	 * unencessary performance impact because of netif_skb_features() API call unconditionally
	 * Hence moved skb_is_nonlinear call outside.
	 */
	 if (unlikely(skb_is_nonlinear(nbuf))) {
		if (nss_core_skb_needs_linearize(nbuf, (uint32_t)netif_skb_features(nbuf)) &&
				__skb_linearize(nbuf)) {
			/*
			 * We needed to linearize, but __skb_linearize() failed. Therefore
			 * we free the nbuf.
			 */
			dev_put(ndev);
			dev_kfree_skb_any(nbuf);
			return;
		}
	}

	/*
	 * Check to see if there is a xmit callback is registered
	 * in this path. The callback will decide the queue mapping.
	 */
	if (unlikely((subsys_dp_reg->xmit_cb))) {
		skb_set_queue_mapping(nbuf, 0);
		subsys_dp_reg->xmit_cb(ndev, nbuf);
		dev_put(ndev);
		return;
	}

	/*
	 * Mimic Linux behavior to allow multi-queue netdev choose which queue to use
	 */
	if (ndev->netdev_ops->ndo_select_queue) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
		queue_offset = ndev->netdev_ops->ndo_select_queue(ndev, nbuf, NULL, NULL);
#else
		queue_offset = ndev->netdev_ops->ndo_select_queue(ndev, nbuf, NULL);
#endif
	}

	skb_set_queue_mapping(nbuf, queue_offset);

	/*
	 * Send the packet to virtual interface
	 * NOTE: Invoking this will BYPASS any assigned QDisc - this is OKAY
	 * as TX packets out of the NSS will have been shaped inside the NSS.
	 */
	xmit_ret = ndev->netdev_ops->ndo_start_xmit(nbuf, ndev);
	if (unlikely(xmit_ret == NETDEV_TX_BUSY)) {
		dev_kfree_skb_any(nbuf);
		nss_info("%px: Congestion at virtual interface %d, %px", nss_ctx, interface_num, ndev);
	}
	dev_put(ndev);
}

/*
 * nss_core_handle_buffer_pkt()
 *	Handle data packet received on physical or virtual interface.
 */
static inline void nss_core_handle_buffer_pkt(struct nss_ctx_instance *nss_ctx,
						unsigned int interface_num,
						struct sk_buff *nbuf,
						struct napi_struct *napi,
						uint16_t flags, uint16_t qid, uint8_t service_code)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_subsystem_dataplane_register *subsys_dp_reg = &nss_ctx->subsys_dp_register[interface_num];
	struct net_device *ndev = NULL;
	nss_phys_if_rx_callback_t cb;
	uint16_t queue_offset = qid - NSS_IF_N2H_DATA_QUEUE_0;

	NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_RX_PACKET]);

	/*
	 * Check if NSS was able to obtain checksum
	 */
	nbuf->ip_summed = CHECKSUM_UNNECESSARY;
	if (unlikely(!(flags & N2H_BIT_FLAG_IP_TRANSPORT_CHECKSUM_VALID))) {
		nbuf->ip_summed = CHECKSUM_NONE;
	}

	ndev = subsys_dp_reg->ndev;
	if (!ndev) {
		dev_kfree_skb_any(nbuf);
		return;
	}

	/*
	 * If we have a non-zero service code, call the corresponding service code
	 * callback. The callback will consume the skb.
	 * For service code, we provide the raw packet as it was received.
	 */
	if (unlikely(service_code)) {
		nss_cmn_service_code_callback_t cb = nss_ctx->service_code_callback[service_code];
		if (likely(cb)) {
			dev_hold(ndev);
			nbuf->dev = ndev;
			nbuf->protocol = eth_type_trans(nbuf, ndev);
			cb(nss_ctx->service_code_ctx[service_code], nbuf);
			dev_put(ndev);
			return;
		}
	}

	/*
	 * Deliver nbuf to the interface through callback if there is one.
	 */
	cb = subsys_dp_reg->cb;
	if (likely(cb)) {
		/*
		 * linearize or free if requested.
		 */
	 	if (unlikely(skb_is_nonlinear(nbuf))) {
			if (nss_core_skb_needs_linearize(nbuf, ndev->features) && __skb_linearize(nbuf)) {
				dev_kfree_skb_any(nbuf);
				return;
			}
		}

		/*
		 * Record RX queue if the netdev has that many RX queues
		 */
		if (queue_offset < ndev->real_num_rx_queues) {
			skb_record_rx_queue(nbuf, queue_offset);
		}

		cb(ndev, (void *)nbuf, napi);
		return;
	}

	/*
	 * Deliver to the stack directly. Ex. there is no rule matched for
	 * redirect interface.
	 */
	dev_hold(ndev);
	nbuf->dev = ndev;
	nbuf->protocol = eth_type_trans(nbuf, ndev);
	netif_receive_skb(nbuf);
	dev_put(ndev);
}

/*
 * nss_core_handle_ext_buffer_pkt()
 *	Handle Extended data plane packet received on physical or virtual interface.
 */
static inline void nss_core_handle_ext_buffer_pkt(struct nss_ctx_instance *nss_ctx,
						unsigned int interface_num,
						struct sk_buff *nbuf,
						struct napi_struct *napi,
						uint16_t flags)
{
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_subsystem_dataplane_register *subsys_dp_reg = &nss_ctx->subsys_dp_register[interface_num];
	struct net_device *ndev = NULL;
	nss_phys_if_rx_ext_data_callback_t ext_cb;

	NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_RX_EXT_PACKET]);

	/*
	 * Check if NSS was able to obtain checksum
	 */
	nbuf->ip_summed = CHECKSUM_UNNECESSARY;
	if (unlikely(!(flags & N2H_BIT_FLAG_IP_TRANSPORT_CHECKSUM_VALID))) {
		nbuf->ip_summed = CHECKSUM_NONE;
	}

	ndev = subsys_dp_reg->ndev;
	ext_cb = subsys_dp_reg->ext_cb;
	if (likely(ext_cb) && likely(ndev)) {

	 	if (unlikely(skb_is_nonlinear(nbuf))) {
			if (nss_core_skb_needs_linearize(nbuf, ndev->features) && __skb_linearize(nbuf)) {
				/*
			 	* We needed to linearize, but __skb_linearize() failed. So free the nbuf.
			 	*/
				dev_kfree_skb_any(nbuf);
				return;
			}
		}

		ext_cb(ndev, (void *)nbuf, napi);
	} else {
		dev_kfree_skb_any(nbuf);
	}
}

/*
 * nss_core_rx_pbuf()
 *	Receive a pbuf from the NSS into Linux.
 */
static inline void nss_core_rx_pbuf(struct nss_ctx_instance *nss_ctx, struct n2h_descriptor *desc, struct napi_struct *napi,
				    uint8_t buffer_type, struct sk_buff *nbuf, uint16_t qid)
{
	unsigned int interface_num = NSS_INTERFACE_NUM_GET(desc->interface_num);
	unsigned int core_id = NSS_INTERFACE_NUM_GET_COREID(desc->interface_num);
	struct nss_shaper_bounce_registrant *reg = NULL;
	int32_t status;

	NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);

	if (interface_num >= NSS_MAX_NET_INTERFACES) {
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_INVALID_INTERFACE]);
		nss_warning("%px: Invalid interface_num: %d", nss_ctx, interface_num);
		dev_kfree_skb_any(nbuf);
		return;
	}

	/*
	 * Check if core_id value is valid.
	 */
	if (core_id > nss_top_main.num_nss) {
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_INVALID_CORE_ID]);
		nss_warning("%px: Invalid core id: %d", nss_ctx, core_id);
		dev_kfree_skb_any(nbuf);
		return;
	}

	/*
	 * Check if need to convert to local core value.
	 */
	if (core_id) {
		nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[core_id - 1];
	}

	switch (buffer_type) {
	case N2H_BUFFER_PACKET:
		nss_core_handle_buffer_pkt(nss_ctx, interface_num, nbuf, napi, desc->bit_flags, qid, desc->service_code);
		break;

	case N2H_BUFFER_PACKET_VIRTUAL:
		nss_core_handle_virt_if_pkt(nss_ctx, interface_num, nbuf);
		break;

	case N2H_BUFFER_SHAPER_BOUNCED_INTERFACE:
		reg = &nss_ctx->nss_top->bounce_interface_registrants[interface_num];
		nss_core_handle_bounced_pkt(nss_ctx, reg, nbuf);
		break;

	case N2H_BUFFER_SHAPER_BOUNCED_BRIDGE:
		reg = &nss_ctx->nss_top->bounce_bridge_registrants[interface_num];
		nss_core_handle_bounced_pkt(nss_ctx, reg, nbuf);
		break;

	case N2H_BUFFER_PACKET_EXT:
		nss_core_handle_ext_buffer_pkt(nss_ctx, interface_num, nbuf, napi, desc->bit_flags);
		break;

	case N2H_BUFFER_STATUS:
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_STATUS]);
		nss_core_handle_nss_status_pkt(nss_ctx, nbuf);
		dev_kfree_skb_any(nbuf);
		break;

	case N2H_BUFFER_CRYPTO_RESP:
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_CRYPTO_RESP]);
		nss_core_handle_crypto_pkt(nss_ctx, interface_num, nbuf, napi);
		break;

	case N2H_BUFFER_RATE_TEST:

		/*
		 * This is a packet NSS sent for packet rate testing. The test measures the
		 * maximum PPS we can achieve between the host and NSS. After we process
		 * the descriptor, we directly send these test packets back to NSS without further process.
		 * They are again marked with H2N_BUFFER_RATE_TEST buffer type so NSS can process
		 * and count the test packets properly.
		 */
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_STATUS]);
		status = nss_core_send_buffer(nss_ctx, 0, nbuf, NSS_IF_H2N_DATA_QUEUE, H2N_BUFFER_RATE_TEST, H2N_BIT_FLAG_BUFFER_REUSABLE);
		if (unlikely(status != NSS_CORE_STATUS_SUCCESS)) {
			dev_kfree_skb_any(nbuf);
			nss_warning("%px: Unable to enqueue\n", nss_ctx);
		}
		nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);
		break;

	default:
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_INVALID_BUFFER_TYPE]);
		nss_warning("%px: Invalid buffer type %d received from NSS", nss_ctx, buffer_type);
		dev_kfree_skb_any(nbuf);
	}
}

/*
 * nss_core_set_skb_classify()
 *	Set skb field to avoid ingress shaping.
 */
static inline void nss_core_set_skb_classify(struct sk_buff *nbuf)
{
#ifdef CONFIG_NET_CLS_ACT
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0))
		nbuf->tc_verd = SET_TC_NCLS_NSS(nbuf->tc_verd);
#else
		skb_set_tc_classify_offload(nbuf);
#endif
#endif
}

/*
 * nss_core_handle_nrfrag_skb()
 *	Handled the processing of fragmented skb's
 */
static inline bool nss_core_handle_nr_frag_skb(struct nss_ctx_instance *nss_ctx, struct sk_buff **nbuf_ptr, struct sk_buff **jumbo_start_ptr, struct n2h_descriptor *desc, unsigned int buffer_type)
{
	struct sk_buff *nbuf = *nbuf_ptr;
	struct sk_buff *jumbo_start = *jumbo_start_ptr;

	uint16_t payload_len = desc->payload_len;
	uint16_t payload_offs = desc->payload_offs;
	uint16_t bit_flags = desc->bit_flags;

	nss_assert(desc->payload_offs + desc->payload_len <= PAGE_SIZE);

	dma_unmap_page(nss_ctx->dev, (desc->buffer + desc->payload_offs), desc->payload_len, DMA_FROM_DEVICE);

	/*
	 * The first and last bits are both set. Hence the received frame can't have
	 * chains (or it's not a scattered one).
	 */
	if (likely(bit_flags & N2H_BIT_FLAG_FIRST_SEGMENT) && likely(bit_flags & N2H_BIT_FLAG_LAST_SEGMENT)) {

		/*
		 * We have received another head before we saw the last segment.
		 * Free the old head as the frag list is corrupt.
		 */
		if (unlikely(jumbo_start)) {
			nss_warning("%px: received a full frame before a last", jumbo_start);
			dev_kfree_skb_any(jumbo_start);
			*jumbo_start_ptr = NULL;
		}

		/*
		 * NOTE: Need to use __skb_fill since we do not want to
		 * increment nr_frags again. We just want to adjust the offset
		 * and the length.
		 */
		__skb_fill_page_desc(nbuf, 0, skb_frag_page(&skb_shinfo(nbuf)->frags[0]), payload_offs, payload_len);

		/*
		 * We do not update truesize. We just keep the initial set value.
		 */
		nbuf->data_len = payload_len;
		nbuf->len = payload_len;
		nbuf->priority = desc->pri;

#ifdef CONFIG_NET_CLS_ACT
		/*
		 * Skip the ingress QoS for the packet if the descriptor has
		 * ingress shaped flag set.
		 */
		if (unlikely(desc->bit_flags & N2H_BIT_FLAG_INGRESS_SHAPED)) {
			nss_core_set_skb_classify(nbuf);
		}
#endif

		goto pull;
	}

	/*
	 * Track Number of Fragments processed. First && Last is not true fragment
	 */
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_FRAG_SEG_PROCESSED]);

	/*
	 * NSS sent us an SG chain.
	 * Build a frags[] out of segments.
	 */
	if (unlikely((bit_flags & N2H_BIT_FLAG_FIRST_SEGMENT))) {

		/*
		 * We have received another head before we saw the last segment.
		 * Free the old head as the frag list is corrupt.
		 */
		if (unlikely(jumbo_start)) {
			nss_warning("%px: received the second head before a last", jumbo_start);
			dev_kfree_skb_any(jumbo_start);
		}

		/*
		 * We do not update truesize. We just keep the initial set value.
		 */
		__skb_fill_page_desc(nbuf, 0, skb_frag_page(&skb_shinfo(nbuf)->frags[0]), payload_offs, payload_len);
		nbuf->data_len = payload_len;
		nbuf->len = payload_len;
		nbuf->priority = desc->pri;

#ifdef CONFIG_NET_CLS_ACT
		/*
		 * Skip the ingress QoS for the packet if the descriptor has
		 * ingress shaped flag set.
		 */
		if (unlikely(desc->bit_flags & N2H_BIT_FLAG_INGRESS_SHAPED)) {
			nss_core_set_skb_classify(nbuf);
		}
#endif

		/*
		 * Set jumbo pointer to nbuf
		 */
		*jumbo_start_ptr = nbuf;

		/*
		 * Skip sending until last is received.
		 */
		return false;
	}

	NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);

	/*
	 * We've received a middle or a last segment.
	 * Check that we have received a head first to avoid null deferencing.
	 */
	if (unlikely(jumbo_start == NULL)) {
		/*
		 * Middle before first! Free the middle.
		 */
		nss_warning("%px: saw a middle skb before head", nbuf);
		dev_kfree_skb_any(nbuf);
		return false;
	}

	/*
	 * Free the skb after attaching the frag to the head skb.
	 * Our page is safe although we are freeing it because we
	 * just took a reference to it.
	 */
	skb_add_rx_frag(jumbo_start, skb_shinfo(jumbo_start)->nr_frags, skb_frag_page(&skb_shinfo(nbuf)->frags[0]), payload_offs, payload_len, PAGE_SIZE);
	skb_frag_ref(jumbo_start, skb_shinfo(jumbo_start)->nr_frags - 1);
	dev_kfree_skb_any(nbuf);

	if (!(bit_flags & N2H_BIT_FLAG_LAST_SEGMENT)) {
		/*
		 * Skip sending until last is received.
		 */
		return false;
	}

	/*
	 * Last is received. Set nbuf pointer to point to
	 * the jumbo skb so that it continues to get processed.
	 */
	nbuf = jumbo_start;
	*nbuf_ptr = nbuf;
	*jumbo_start_ptr = NULL;
	prefetch((void *)(nbuf->data));

pull:
	/*
	 * We need eth hdr to be in the linear part of the skb
	 * for data packets. Otherwise eth_type_trans fails.
	 */
	if (buffer_type != N2H_BUFFER_STATUS) {
		if (!pskb_may_pull(nbuf, ETH_HLEN)) {
			dev_kfree_skb(nbuf);
			nss_warning("%px: could not pull eth header", nbuf);
			return false;
		}
	}

	return true;
}

/*
 * nss_core_handle_linear_skb()
 *	Handler for processing linear skbs.
 */
static inline bool nss_core_handle_linear_skb(struct nss_ctx_instance *nss_ctx, struct sk_buff **nbuf_ptr, struct sk_buff **head_ptr,
						struct sk_buff **tail_ptr, struct n2h_descriptor *desc)
{
	uint16_t bit_flags = desc->bit_flags;
	struct sk_buff *nbuf = *nbuf_ptr;
	struct sk_buff *head = *head_ptr;
	struct sk_buff *tail = *tail_ptr;

	/*
	 * We are in linear SKB mode.
	 */
	nbuf->data = nbuf->head + desc->payload_offs;
	nbuf->len = desc->payload_len;
	skb_set_tail_pointer(nbuf, nbuf->len);

	dma_unmap_single(nss_ctx->dev, (desc->buffer + desc->payload_offs), desc->payload_len,
			 DMA_FROM_DEVICE);

	prefetch((void *)(nbuf->data));

	if (likely(bit_flags & N2H_BIT_FLAG_FIRST_SEGMENT) && likely(bit_flags & N2H_BIT_FLAG_LAST_SEGMENT)) {

		/*
		 * We have received another head before we saw the last segment.
		 * Free the old head as the frag list is corrupt.
		 */
		if (unlikely(head)) {
			nss_warning("%px: received a full frame before a last", head);
			dev_kfree_skb_any(head);
			*head_ptr = NULL;
		}

		nbuf->priority = desc->pri;

#ifdef CONFIG_NET_CLS_ACT
		/*
		 * Skip the ingress QoS for the packet if the descriptor has
		 * ingress shaped flag set.
		 */
		if (unlikely(desc->bit_flags & N2H_BIT_FLAG_INGRESS_SHAPED)) {
			nss_core_set_skb_classify(nbuf);
		}
#endif

		/*
		 * TODO: Check if there is any issue wrt map and unmap,
		 * NSS should playaround with data area and should not
		 * touch HEADROOM area
		 */
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_SIMPLE]);
		return true;
	}

	/*
	 * Track number of skb chain processed. First && Last is not true segment.
	 */
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_CHAIN_SEG_PROCESSED]);

	/*
	 * NSS sent us an SG chain.
	 * Build a frag list out of segments.
	 */
	if (unlikely((bit_flags & N2H_BIT_FLAG_FIRST_SEGMENT))) {

		/*
		 * We have received another head before we saw the last segment.
		 * Free the old head as the frag list is corrupt.
		 */
		if (unlikely(head)) {
			nss_warning("%px: received the second head before a last", head);
			NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
			dev_kfree_skb_any(head);
		}

		/*
		 * Found head.
		 */
		if (unlikely(skb_has_frag_list(nbuf))) {
			/*
			 * We don't support chain in a chain.
			 */
			nss_warning("%px: skb already has a fraglist", nbuf);
			NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
			dev_kfree_skb_any(nbuf);
			return false;
		}

		skb_frag_list_init(nbuf);
		nbuf->data_len = 0;
		nbuf->truesize = desc->payload_len;
		nbuf->priority = desc->pri;

#ifdef CONFIG_NET_CLS_ACT
		/*
		 * Skip the ingress QoS for the packet if the descriptor has
		 * ingress shaped flag set.
		 */
		if (unlikely(desc->bit_flags & N2H_BIT_FLAG_INGRESS_SHAPED)) {
			nss_core_set_skb_classify(nbuf);
		}
#endif

		*head_ptr = nbuf;

		/*
		 * Skip sending until last is received.
		 */
		return false;
	}

	NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);

	/*
	 * We've received a middle segment.
	 * Check that we have received a head first to avoid null deferencing.
	 */
	if (unlikely(head == NULL)) {

		/*
		 * Middle before first! Free the middle.
		 */
		nss_warning("%px: saw a middle skb before head", nbuf);
		dev_kfree_skb_any(nbuf);

		return false;
	}

	if (!skb_has_frag_list(head)) {
		/*
		 * 2nd skb in the chain. head's frag_list should point to him.
		 */
		nbuf->next = skb_shinfo(head)->frag_list;
		skb_shinfo(head)->frag_list = nbuf;
	} else {
		/*
		 * 3rd, 4th... skb in the chain. The chain's previous tail's
		 * next should point to him.
		 */
		tail->next = nbuf;
		nbuf->next = NULL;
	}
	*tail_ptr = nbuf;

	/*
	 * Now we've added a new nbuf to the chain.
	 * Update the chain length.
	 */
	head->data_len += desc->payload_len;
	head->len += desc->payload_len;
	head->truesize += desc->payload_len;

	if (!(bit_flags & N2H_BIT_FLAG_LAST_SEGMENT)) {
		/*
		 * Skip sending until last is received.
		 */
		return false;
	}

	/*
	 * Last is received. Send the frag_list.
	 */
	*nbuf_ptr = head;
	*head_ptr = NULL;
	*tail_ptr = NULL;
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_SKB_FRAGLIST]);
	return true;
}

/*
 * nss_core_handle_empty_buffers()
 *	Handle empty buffer returns.
 */
static inline void nss_core_handle_empty_buffers(struct nss_ctx_instance *nss_ctx,
						 struct nss_if_mem_map *if_map,
						 struct hlos_n2h_desc_ring *n2h_desc_ring,
						 struct n2h_descriptor *desc_ring,
						 struct n2h_descriptor *desc,
						 uint32_t count, uint32_t hlos_index,
						 uint16_t mask)
{
	while (count) {
		/*
		 * Since we only return the primary skb, we have no way to unmap
		 * properly. Simple skb's are properly mapped but page data skbs
		 * have the payload mapped (and not the skb->data slab payload).
		 *
		 * Warning: On non-Krait HW, we need to unmap fragments.
		 *
		 * This only unmaps the first segment either slab payload or
		 * skb page data. Eventually, we need to unmap all of a frag_list
		 * or all of page_data however this is not a big concern as of now
		 * since on Kriats dma_map_single() does not allocate any resource
		 * and hence dma_unmap_single() is sort off a nop.
		 *
		 * No need to invalidate for Tx Completions, so set dma direction = DMA_TO_DEVICE;
		 * Similarly prefetch is not needed for an empty buffer.
		 */
		struct sk_buff *nbuf;

		/*
		 * Prefetch the next cache line of descriptors.
		 */
		if (((hlos_index & 1) == 0) && likely(count > 2)) {
			struct n2h_descriptor *next_cache_desc = &desc_ring[(hlos_index + 2) & mask];
			prefetch(next_cache_desc);
		}

		nbuf = (struct sk_buff *)desc->opaque;

		if (unlikely(nbuf < (struct sk_buff *)PAGE_OFFSET)) {
			/*
			 * Invalid opaque pointer
			 */
			nss_dump_desc(nss_ctx, desc);
			NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_BAD_DESCRIPTOR]);
			goto next;
		}

		dma_unmap_single(nss_ctx->dev, (desc->buffer + desc->payload_offs), desc->payload_len, DMA_TO_DEVICE);
		dev_kfree_skb_any(nbuf);

		NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_EMPTY]);

next:
		hlos_index = (hlos_index + 1) & (mask);
		desc = &desc_ring[hlos_index];
		count--;
	}

	n2h_desc_ring->hlos_index = hlos_index;
	if_map->n2h_hlos_index[NSS_IF_N2H_EMPTY_BUFFER_RETURN_QUEUE] = hlos_index;

	NSS_CORE_DMA_CACHE_MAINT((void *)&if_map->n2h_hlos_index[NSS_IF_N2H_EMPTY_BUFFER_RETURN_QUEUE], sizeof(uint32_t), DMA_TO_DEVICE);
	NSS_CORE_DSB();
}

/*
 * nss_core_handle_cause_queue()
 *	Handle interrupt cause related to N2H/H2N queues
 */
static int32_t nss_core_handle_cause_queue(struct int_ctx_instance *int_ctx, uint16_t cause, int16_t weight)
{
	int16_t count, count_temp;
	uint16_t size, mask, qid;
	uint32_t nss_index, hlos_index, start, end;
	struct sk_buff *nbuf;
	struct hlos_n2h_desc_ring *n2h_desc_ring;
	struct n2h_desc_if_instance *desc_if;
	struct n2h_descriptor *desc_ring;
	struct n2h_descriptor *desc;
	struct n2h_descriptor *next_cache_desc;
	struct nss_ctx_instance *nss_ctx = int_ctx->nss_ctx;
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	struct nss_if_mem_map *if_map = mem_ctx->if_map;

	qid = nss_core_cause_to_queue(cause);

	/*
	 * Make sure qid < num_rings
	 */
	nss_assert(qid < if_map->n2h_rings);

	n2h_desc_ring = &nss_ctx->n2h_desc_ring[qid];
	desc_if = &n2h_desc_ring->desc_ring;
	desc_ring = desc_if->desc;
	NSS_CORE_DMA_CACHE_MAINT((void *)&if_map->n2h_nss_index[qid], sizeof(uint32_t), DMA_FROM_DEVICE);
	NSS_CORE_DSB();
	nss_index = if_map->n2h_nss_index[qid];

	hlos_index = n2h_desc_ring->hlos_index;
	size = desc_if->size;
	mask = size - 1;

	/*
	 * Check if there is work to be done for this queue
	 */
	count = ((nss_index - hlos_index) + size) & (mask);
	if (unlikely(count == 0)) {
		return 0;
	}

	/*
	 * Restrict ourselves to suggested weight
	 */
	if (count > weight) {
		count = weight;
	}

	/*
	 * Invalidate all the descriptors we are going to read
	 */
	start = hlos_index;
	end = (hlos_index + count) & mask;
	if (end > start) {
		dmac_inv_range((void *)&desc_ring[start], (void *)&desc_ring[end] + sizeof(struct n2h_descriptor));
	} else {
		/*
		 * We have wrapped around
		 */
		dmac_inv_range((void *)&desc_ring[start], (void *)&desc_ring[mask] + sizeof(struct n2h_descriptor));
		dmac_inv_range((void *)&desc_ring[0], (void *)&desc_ring[end] + sizeof(struct n2h_descriptor));
	}

	/*
	 * Prefetch the first descriptor
	 */
	desc = &desc_ring[hlos_index];
	prefetch(desc);

	/*
	 * Prefetch the next cache line of descriptors if we are starting with
	 * the second descriptor in the cache line. If it is the first in the cache line,
	 * this will be done inside the loop.
	 */
	if (((hlos_index & 1) == 1) && likely((count > 1))) {
		next_cache_desc = &desc_ring[(hlos_index + 2) & mask];
		prefetch(next_cache_desc);
	}

	if (qid == NSS_IF_N2H_EMPTY_BUFFER_RETURN_QUEUE) {
		nss_core_handle_empty_buffers(nss_ctx, if_map, n2h_desc_ring, desc_ring, desc, count, hlos_index, mask);
		return count;
	}

	count_temp = count;
	while (count_temp) {
		unsigned int buffer_type;
		nss_ptr_t opaque;

		/*
		 * Prefetch the next cache line of descriptors.
		 */
		if (((hlos_index & 1) == 0) && likely(count_temp > 2)) {
			next_cache_desc = &desc_ring[(hlos_index + 2) & mask];
			prefetch(next_cache_desc);
		}

		buffer_type = desc->buffer_type;
		opaque = desc->opaque;

		/*
		 * Obtain nbuf
		 */
		nbuf = (struct sk_buff *)opaque;
		if (unlikely(nbuf < (struct sk_buff *)PAGE_OFFSET)) {
			/*
			 * Invalid opaque pointer
			 */
			nss_dump_desc(nss_ctx, desc);
			NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_BAD_DESCRIPTOR]);
			goto next;
		}

		/*
		 * Shaping uses the singleton approach as well. No need to unmap all the segments since only
		 * one of them is actually looked at.
		 */
		if ((unlikely(buffer_type == N2H_BUFFER_SHAPER_BOUNCED_INTERFACE)) || (unlikely(buffer_type == N2H_BUFFER_SHAPER_BOUNCED_BRIDGE))) {
			dma_unmap_page(nss_ctx->dev, (desc->buffer + desc->payload_offs), desc->payload_len, DMA_TO_DEVICE);
			goto consume;
		}

		/*
		 * crypto buffer
		 *
		 */
		if (unlikely((buffer_type == N2H_BUFFER_CRYPTO_RESP))) {
			dma_unmap_single(NULL, (desc->buffer + desc->payload_offs), desc->payload_len, DMA_FROM_DEVICE);
			goto consume;
		}

		/*
		 * Check if we received a paged skb.
		 */
		if (skb_shinfo(nbuf)->nr_frags > 0) {
			/*
			 * Check if we received paged skb while constructing
			 * a linear skb chain. If so we need to free.
			 */
			if (unlikely(n2h_desc_ring->head)) {
				nss_warning("%px: we should not have an incomplete paged skb while"
								" constructing a linear skb %px", nbuf, n2h_desc_ring->head);

				NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
				dev_kfree_skb_any(n2h_desc_ring->head);
				n2h_desc_ring->head = NULL;
			}

			if (!nss_core_handle_nr_frag_skb(nss_ctx, &nbuf, &n2h_desc_ring->jumbo_start, desc, buffer_type)) {
				goto next;
			}
			NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_RX_NR_FRAGS]);
			goto consume;
		}

		/*
		 * Check if we received a linear skb while constructing
		 * a paged skb. If so we need to free the paged_skb and handle the linear skb.
		 */
		if (unlikely(n2h_desc_ring->jumbo_start)) {
			nss_warning("%px: we should not have an incomplete linear skb while"
							" constructing a paged skb %px", nbuf, n2h_desc_ring->jumbo_start);

			NSS_PKT_STATS_DEC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
			dev_kfree_skb_any(n2h_desc_ring->jumbo_start);
			n2h_desc_ring->jumbo_start = NULL;
		}

		/*
		 * This is a simple linear skb. Use the the linear skb
		 * handler to process it.
		 */
		if (!nss_core_handle_linear_skb(nss_ctx, &nbuf, &n2h_desc_ring->head, &n2h_desc_ring->tail, desc)) {
			goto next;
		}

consume:
		nss_core_rx_pbuf(nss_ctx, desc, &(int_ctx->napi), buffer_type, nbuf, qid);

next:

		hlos_index = (hlos_index + 1) & (mask);
		desc = &desc_ring[hlos_index];
		count_temp--;
	}

	n2h_desc_ring->hlos_index = hlos_index;
	if_map->n2h_hlos_index[qid] = hlos_index;

	NSS_CORE_DMA_CACHE_MAINT((void *)&if_map->n2h_hlos_index[qid], sizeof(uint32_t), DMA_TO_DEVICE);
	NSS_CORE_DSB();

	return count;
}

/*
 * nss_core_init_nss()
 *	Initialize NSS core state
 */
static void nss_core_init_nss(struct nss_ctx_instance *nss_ctx, struct nss_if_mem_map *if_map)
{
	struct nss_top_instance *nss_top;
	int ret;

	NSS_CORE_DMA_CACHE_MAINT((void *)if_map, sizeof(*if_map), DMA_FROM_DEVICE);
	NSS_CORE_DSB();

	/*
	 * NOTE: A commonly found error is that sizes and start address of per core
	 *	virtual register map do not match in NSS and HLOS builds. This will lead
	 *	to some hard to trace issues such as spinlock magic check failure etc.
	 *	Following checks verify that proper virtual map has been initialized
	 */
	nss_assert(if_map->magic == DEV_MAGIC);

#ifdef NSS_DRV_C2C_ENABLE
	nss_ctx->c2c_start = nss_ctx->meminfo_ctx.c2c_start_dma;
#endif

	nss_top = nss_ctx->nss_top;
	spin_lock_bh(&nss_top->lock);
	nss_ctx->state = NSS_CORE_STATE_INITIALIZED;
	spin_unlock_bh(&nss_top->lock);

	if (nss_ctx->id) {
		ret = nss_n2h_update_queue_config_async(nss_ctx, pn_mq_en, pn_qlimits);
		if (ret != NSS_TX_SUCCESS) {
			nss_warning("Failed to send pnode queue config to core 1\n");
		}
		return;
	}

	/*
	 * If nss core0 is up, then we are ready to hook to nss-gmac
	 */
	if (nss_data_plane_schedule_registration()) {

		/*
		 * Configure the maximum number of IPv4/IPv6
		 * connections supported by the accelerator.
		 */
		nss_ipv4_conn_cfg = max_ipv4_conn;
#ifdef NSS_DRV_IPV6_ENABLE
		nss_ipv6_conn_cfg = max_ipv6_conn;
		nss_ipv6_update_conn_count(max_ipv6_conn);
#endif
		nss_ipv4_update_conn_count(max_ipv4_conn);

#ifdef NSS_MEM_PROFILE_LOW
		/*
		 * For low memory profiles, restrict the number of empty buffer pool
		 * size to NSS_LOW_MEM_EMPTY_POOL_BUF_SZ. Overwrite the default number
		 * of empty buffer pool size configured during NSS initialization.
		 */
		ret = nss_n2h_cfg_empty_pool_size(nss_ctx, NSS_LOW_MEM_EMPTY_POOL_BUF_SZ);
		if (ret != NSS_TX_SUCCESS) {
			nss_warning("%px: Failed to update empty buffer pool config\n", nss_ctx);
		}
#endif
	} else {
		spin_lock_bh(&nss_top->lock);
		nss_ctx->state = NSS_CORE_STATE_UNINITIALIZED;
		spin_unlock_bh(&nss_top->lock);
	}
}

/*
 * nss_core_alloc_paged_buffers()
 *	Allocate paged buffers for SOS.
 */
static void nss_core_alloc_paged_buffers(struct nss_ctx_instance *nss_ctx, struct nss_if_mem_map *if_map,
				uint16_t count, int16_t mask, int32_t hlos_index, uint32_t alloc_fail_count,
				uint32_t buffer_type, uint32_t buffer_queue, uint32_t stats_index)
{
	struct sk_buff *nbuf;
	struct page *npage;
	struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[buffer_queue];
	struct h2n_desc_if_instance *desc_if = &h2n_desc_ring->desc_ring;
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	while (count) {
		struct h2n_descriptor *desc = &desc_ring[hlos_index];
		dma_addr_t buffer;

		/*
		 * Alloc an skb AND a page.
		 */
		nbuf = dev_alloc_skb(NSS_CORE_JUMBO_LINEAR_BUF_SIZE);
		if (unlikely(!nbuf)) {
			/*
			 * ERR:
			 */
			NSS_PKT_STATS_INC(&nss_top->stats_drv[alloc_fail_count]);
			nss_warning("%px: Could not obtain empty paged buffer", nss_ctx);
			break;
		}

		npage = alloc_page(GFP_ATOMIC);
		if (unlikely(!npage)) {
			/*
			 * ERR:
			 */
			dev_kfree_skb_any(nbuf);
			NSS_PKT_STATS_INC(&nss_top->stats_drv[alloc_fail_count]);
			nss_warning("%px: Could not obtain empty page", nss_ctx);
			break;
		}

		/*
		 * When we alloc an skb, initially head = data = tail and len = 0.
		 * So nobody will try to read the linear part of the skb.
		 */
		skb_fill_page_desc(nbuf, 0, npage, 0, PAGE_SIZE);
		nbuf->data_len += PAGE_SIZE;
		nbuf->len += PAGE_SIZE;
		nbuf->truesize += PAGE_SIZE;

		/* Map the page for jumbo */
		buffer = dma_map_page(nss_ctx->dev, npage, 0, PAGE_SIZE, DMA_FROM_DEVICE);
		desc->buffer_len = PAGE_SIZE;
		desc->payload_offs = 0;

		if (unlikely(dma_mapping_error(nss_ctx->dev, buffer))) {
			/*
			 * ERR:
			 */
			dev_kfree_skb_any(nbuf);
			nss_warning("%px: DMA mapping failed for empty buffer", nss_ctx);
			break;
		}
		/*
		 * We are holding this skb in NSS FW, let kmemleak know about it
		 */
		kmemleak_not_leak(nbuf);
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
		desc->opaque = (nss_ptr_t)nbuf;
		desc->buffer = buffer;
		desc->buffer_type = buffer_type;

		/*
		 * Flush the descriptor
		 */
		NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

		hlos_index = (hlos_index + 1) & (mask);
		count--;
	}

	/*
	 * Wait for the flushes to be synced before writing the index
	 */
	NSS_CORE_DSB();

	h2n_desc_ring->hlos_index = hlos_index;
	if_map->h2n_hlos_index[buffer_queue] = hlos_index;

	NSS_CORE_DMA_CACHE_MAINT(&if_map->h2n_hlos_index[buffer_queue], sizeof(uint32_t), DMA_TO_DEVICE);
	NSS_CORE_DSB();

	NSS_PKT_STATS_INC(&nss_top->stats_drv[stats_index]);
}

/*
 * nss_core_alloc_jumbo_mru_buffers()
 *	Allocate jumbo mru buffers.
 */
static void nss_core_alloc_jumbo_mru_buffers(struct nss_ctx_instance *nss_ctx, struct nss_if_mem_map *if_map,
				int jumbo_mru, uint16_t count, int16_t mask, int32_t hlos_index)
{

	struct sk_buff *nbuf;
	struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[NSS_IF_H2N_EMPTY_BUFFER_QUEUE];
	struct h2n_desc_if_instance *desc_if = &h2n_desc_ring->desc_ring;
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;

	while (count) {
		struct h2n_descriptor *desc = &desc_ring[hlos_index];
		dma_addr_t buffer;
		nbuf = dev_alloc_skb(jumbo_mru);
		if (unlikely(!nbuf)) {
			/*
			 * ERR:
			 */
			NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_NBUF_ALLOC_FAILS]);
			nss_warning("%px: Could not obtain empty jumbo mru buffer", nss_ctx);
			break;
		}

		/*
		 * Map the skb
		 */
		buffer = dma_map_single(nss_ctx->dev, nbuf->head, jumbo_mru, DMA_FROM_DEVICE);
		desc->buffer_len = jumbo_mru;
		desc->payload_offs = (uint16_t) (nbuf->data - nbuf->head);
		if (unlikely(dma_mapping_error(nss_ctx->dev, buffer))) {
			/*
			 * ERR:
			 */
			dev_kfree_skb_any(nbuf);
			nss_warning("%px: DMA mapping failed for empty buffer", nss_ctx);
			break;
		}

		/*
		 * We are holding this skb in NSS FW, let kmemleak know about it
		 */
		kmemleak_not_leak(nbuf);
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);
		desc->opaque = (nss_ptr_t)nbuf;
		desc->buffer = buffer;
		desc->buffer_type = H2N_BUFFER_EMPTY;

		/*
		 * Flush the descriptor
		 */
		NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

		hlos_index = (hlos_index + 1) & (mask);
		count--;
	}

	/*
	 * Wait for the flushes to be synced before writing the index
	 */
	NSS_CORE_DSB();

	h2n_desc_ring->hlos_index = hlos_index;
	if_map->h2n_hlos_index[NSS_IF_H2N_EMPTY_BUFFER_QUEUE] = hlos_index;

	NSS_CORE_DMA_CACHE_MAINT(&if_map->h2n_hlos_index[NSS_IF_H2N_EMPTY_BUFFER_QUEUE], sizeof(uint32_t), DMA_TO_DEVICE);
	NSS_CORE_DSB();

	NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_TX_EMPTY]);
}

/*
 * nss_core_alloc_max_avail_size_buffers()
 *	Allocate maximum available sized buffers.
 */
static void nss_core_alloc_max_avail_size_buffers(struct nss_ctx_instance *nss_ctx, struct nss_if_mem_map *if_map,
				uint16_t max_buf_size, uint16_t count, int16_t mask, int32_t hlos_index)
{
	struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[NSS_IF_H2N_EMPTY_BUFFER_QUEUE];
	struct h2n_desc_if_instance *desc_if = &h2n_desc_ring->desc_ring;
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	uint16_t payload_len = max_buf_size + NET_SKB_PAD;
	uint16_t start = hlos_index;
	uint16_t prev_hlos_index;

	while (count) {
		dma_addr_t buffer;
		struct h2n_descriptor *desc = &desc_ring[hlos_index];

		struct sk_buff *nbuf = dev_alloc_skb(max_buf_size);
		if (unlikely(!nbuf)) {
			/*
			 * ERR:
			 */
			NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_NBUF_ALLOC_FAILS]);
			nss_warning("%px: Could not obtain empty buffer", nss_ctx);
			break;
		}

		/*
		 * Map the skb
		 */
		buffer = dma_map_single(nss_ctx->dev, nbuf->head, payload_len, DMA_FROM_DEVICE);

		if (unlikely(dma_mapping_error(nss_ctx->dev, buffer))) {
			/*
			 * ERR:
			 */
			dev_kfree_skb_any(nbuf);
			nss_warning("%px: DMA mapping failed for empty buffer", nss_ctx);
			break;
		}

		/*
		 * We are holding this skb in NSS FW, let kmemleak know about it
		 */
		kmemleak_not_leak(nbuf);
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);

		desc->opaque = (nss_ptr_t)nbuf;
		desc->buffer = buffer;
		desc->buffer_len = payload_len;

		hlos_index = (hlos_index + 1) & (mask);
		count--;
	}

	/*
	 * Find the last descriptor we need to flush.
	 */
	prev_hlos_index = (hlos_index - 1) & mask;

	/*
	 * Flush the descriptors, including the descriptor at prev_hlos_index.
	 */
	if (prev_hlos_index > start) {
		dmac_clean_range((void *)&desc_ring[start], (void *)&desc_ring[prev_hlos_index] + sizeof(struct h2n_descriptor));
	} else {
		/*
		 * We have wrapped around
		 */
		dmac_clean_range((void *)&desc_ring[start], (void *)&desc_ring[mask] + sizeof(struct h2n_descriptor));
		dmac_clean_range((void *)&desc_ring[0], (void *)&desc_ring[prev_hlos_index] + sizeof(struct h2n_descriptor));
	}

	/*
	 * Wait for the flushes to be synced before writing the index
	 */
	NSS_CORE_DSB();

	h2n_desc_ring->hlos_index = hlos_index;
	if_map->h2n_hlos_index[NSS_IF_H2N_EMPTY_BUFFER_QUEUE] = hlos_index;

	NSS_CORE_DMA_CACHE_MAINT(&if_map->h2n_hlos_index[NSS_IF_H2N_EMPTY_BUFFER_QUEUE], sizeof(uint32_t), DMA_TO_DEVICE);
	NSS_CORE_DSB();

	NSS_PKT_STATS_INC(&nss_top->stats_drv[NSS_DRV_STATS_TX_EMPTY]);
}

/*
 * nss_core_handle_empty_buffer_sos()
 *	Handle empty buffer SOS interrupt.
 */
static inline void nss_core_handle_empty_buffer_sos(struct nss_ctx_instance *nss_ctx,
				struct nss_if_mem_map *if_map, uint16_t max_buf_size)
{
	uint16_t count, size, mask;
	int32_t nss_index, hlos_index;
	struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[NSS_IF_H2N_EMPTY_BUFFER_QUEUE];

	int paged_mode = nss_core_get_paged_mode();
	int jumbo_mru = nss_core_get_jumbo_mru();

	/*
	 * Check how many empty buffers could be filled in queue
	 */
	NSS_CORE_DMA_CACHE_MAINT(&if_map->h2n_nss_index[NSS_IF_H2N_EMPTY_BUFFER_QUEUE], sizeof(uint32_t), DMA_FROM_DEVICE);
	NSS_CORE_DSB();
	nss_index = if_map->h2n_nss_index[NSS_IF_H2N_EMPTY_BUFFER_QUEUE];

	hlos_index = h2n_desc_ring->hlos_index;
	size = h2n_desc_ring->desc_ring.size;

	mask = size - 1;
	count = ((nss_index - hlos_index - 1) + size) & (mask);

	nss_trace("%px: Adding %d buffers to empty queue\n", nss_ctx, count);

	/*
	 * Fill empty buffer queue with buffers leaving one empty descriptor
	 * Note that total number of descriptors in queue cannot be more than (size - 1)
	 */
	if (!count) {
		return;
	}

	if (paged_mode) {
		nss_core_alloc_paged_buffers(nss_ctx, if_map, count, mask, hlos_index,
					NSS_DRV_STATS_NBUF_ALLOC_FAILS, H2N_BUFFER_EMPTY,
					NSS_IF_H2N_EMPTY_BUFFER_QUEUE, NSS_DRV_STATS_TX_EMPTY);
	} else if (jumbo_mru) {
		nss_core_alloc_jumbo_mru_buffers(nss_ctx, if_map, jumbo_mru, count,
					mask, hlos_index);
	} else {
		nss_core_alloc_max_avail_size_buffers(nss_ctx, if_map, max_buf_size,
					count, mask, hlos_index);
	}

	/*
	 * Inform NSS that new buffers are available
	 */
	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_EMPTY_BUFFER_QUEUE);
}

/*
 * nss_core_handle_paged_empty_buffer_sos()
 *	Handle paged empty buffer SOS.
 */
static inline void nss_core_handle_paged_empty_buffer_sos(struct nss_ctx_instance *nss_ctx,
				struct nss_if_mem_map *if_map, uint16_t max_buf_size)
{
	uint16_t count, size, mask;
	int32_t nss_index, hlos_index;
	struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[NSS_IF_H2N_EMPTY_PAGED_BUFFER_QUEUE];

	/*
	 * Check how many empty buffers could be filled in queue
	 */
	NSS_CORE_DMA_CACHE_MAINT((void *)&if_map->h2n_nss_index[NSS_IF_H2N_EMPTY_PAGED_BUFFER_QUEUE], sizeof(uint32_t), DMA_FROM_DEVICE);
	NSS_CORE_DSB();
	nss_index = if_map->h2n_nss_index[NSS_IF_H2N_EMPTY_PAGED_BUFFER_QUEUE];

	hlos_index = h2n_desc_ring->hlos_index;
	size = h2n_desc_ring->desc_ring.size;

	mask = size - 1;
	count = ((nss_index - hlos_index - 1) + size) & (mask);
	nss_trace("%px: Adding %d buffers to paged buffer queue", nss_ctx, count);

	/*
	 * Fill empty buffer queue with buffers leaving one empty descriptor
	 * Note that total number of descriptors in queue cannot be more than (size - 1)
	 */
	if (!count) {
		return;
	}

	nss_core_alloc_paged_buffers(nss_ctx, if_map, count, mask, hlos_index,
			NSS_DRV_STATS_PAGED_BUF_ALLOC_FAILS, H2N_PAGED_BUFFER_EMPTY,
			NSS_IF_H2N_EMPTY_PAGED_BUFFER_QUEUE, NSS_DRV_STATS_PAGED_TX_EMPTY);

	/*
	 * Inform NSS that new buffers are available
	 */
	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_EMPTY_PAGED_BUFFER_QUEUE);
}

/*
 * nss_core_handle_tx_unblocked()
 *	Handle TX Unblocked.
 */
static inline void nss_core_handle_tx_unblocked(struct nss_ctx_instance *nss_ctx)
{
	int32_t i;
	nss_trace("%px: Data queue unblocked", nss_ctx);

	/*
	 * Call callback functions of drivers that have registered with us
	 */
	spin_lock_bh(&nss_ctx->decongest_cb_lock);

	for (i = 0; i < NSS_MAX_CLIENTS; i++) {
		if (nss_ctx->queue_decongestion_callback[i]) {
			nss_ctx->queue_decongestion_callback[i](nss_ctx->queue_decongestion_ctx[i]);
		}
	}

	spin_unlock_bh(&nss_ctx->decongest_cb_lock);
	nss_ctx->h2n_desc_rings[NSS_IF_H2N_DATA_QUEUE].flags &= ~NSS_H2N_DESC_RING_FLAGS_TX_STOPPED;

	/*
	 * Mask Tx unblocked interrupt and unmask it again when queue full condition is reached
	 */
	nss_hal_disable_interrupt(nss_ctx, nss_ctx->int_ctx[0].shift_factor, NSS_N2H_INTR_TX_UNBLOCKED);
}

/*
 * nss_core_handle_cause_nonqueue()
 *	Handle non-queue interrupt causes (e.g. empty buffer SOS, Tx unblocked)
 */
static void nss_core_handle_cause_nonqueue(struct int_ctx_instance *int_ctx, uint32_t cause, int16_t weight)
{
	struct nss_ctx_instance *nss_ctx = int_ctx->nss_ctx;
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	struct nss_if_mem_map *if_map = mem_ctx->if_map;
	uint16_t max_buf_size = (uint16_t) nss_ctx->max_buf_size;
#ifdef NSS_DRV_C2C_ENABLE
	uint32_t c2c_intr_addr1, c2c_intr_addr2;
	int32_t i;
#endif

	nss_assert((cause == NSS_N2H_INTR_EMPTY_BUFFERS_SOS)
			|| (cause == NSS_N2H_INTR_TX_UNBLOCKED)
			|| cause == NSS_N2H_INTR_PAGED_EMPTY_BUFFERS_SOS);

	/*
	 * If this is the first time we are receiving this interrupt then
	 * we need to initialize local state of NSS core. This helps us save an
	 * interrupt cause bit. Hopefully, unlikley and branch prediction algorithm
	 * of processor will prevent any excessive penalties.
	 */
	if (unlikely(nss_ctx->state == NSS_CORE_STATE_UNINITIALIZED)) {
		struct nss_top_instance *nss_top = NULL;
		nss_core_init_nss(nss_ctx, if_map);
		nss_send_ddr_info(nss_ctx);

		nss_info_always("%px: nss core %d booted successfully\n", nss_ctx, nss_ctx->id);
		nss_top = nss_ctx->nss_top;

#ifdef NSS_DRV_C2C_ENABLE
#if (NSS_MAX_CORES > 1)
		/*
		 * Pass C2C addresses of already brought up cores to the recently brought
		 * up core. No NSS core knows the state of other other cores in system so
		 * NSS driver needs to mediate and kick start C2C between them
		 */
		for (i = 0; i < nss_top_main.num_nss; i++) {
			/*
			 * Loop through all NSS cores and send exchange C2C addresses
			 * TODO: Current implementation utilizes the fact that there are
			 *	only two cores in current design. And ofcourse ignore
			 *	the core that we are trying to initialize.
			 */
			if (&nss_top->nss[i] != nss_ctx) {
				/*
				 * Block initialization routine of any other NSS cores running on other
				 * processors. We do not want them to mess around with their initialization
				 * state and C2C addresses while we check their state.
				 */
				spin_lock_bh(&nss_top->lock);
				if (nss_top->nss[i].state == NSS_CORE_STATE_INITIALIZED) {
					spin_unlock_bh(&nss_top->lock);
					c2c_intr_addr1 = (uint32_t)(nss_ctx->nphys) + NSS_REGS_C2C_INTR_SET_OFFSET;
					nss_c2c_tx_msg_cfg_map(&nss_top->nss[i], nss_ctx->c2c_start, c2c_intr_addr1);
					c2c_intr_addr2 = (uint32_t)(nss_top->nss[i].nphys) + NSS_REGS_C2C_INTR_SET_OFFSET;
					nss_c2c_tx_msg_cfg_map(nss_ctx, nss_top->nss[i].c2c_start, c2c_intr_addr2);
					continue;
				}
				spin_unlock_bh(&nss_top->lock);
			}
		}
#endif
#endif
	}

	/*
	 * TODO: find better mechanism to handle empty buffers
	 */
	if (likely(cause == NSS_N2H_INTR_EMPTY_BUFFERS_SOS)) {
		nss_core_handle_empty_buffer_sos(nss_ctx, if_map, max_buf_size);
	} else if (cause == NSS_N2H_INTR_PAGED_EMPTY_BUFFERS_SOS) {
		nss_core_handle_paged_empty_buffer_sos(nss_ctx, if_map, max_buf_size);
	} else if (cause == NSS_N2H_INTR_TX_UNBLOCKED) {
		nss_core_handle_tx_unblocked(nss_ctx);
	}
}

/*
 * nss_core_get_prioritized_cause()
 *	Obtain proritized cause (from multiple interrupt causes) that
 *	must be handled by NSS driver before other causes
 */
static uint32_t nss_core_get_prioritized_cause(uint32_t cause, uint32_t *type, int16_t *weight)
{
	*type = NSS_INTR_CAUSE_INVALID;
	*weight = 0;

	/*
	 * NOTE: This is a very simple algorithm with fixed weight and strict priority
	 *
	 * TODO: Modify the algorithm later with proper weights and Round Robin
	 */

	if (cause & NSS_N2H_INTR_EMPTY_BUFFERS_SOS) {
		*type = NSS_INTR_CAUSE_NON_QUEUE;
		*weight = NSS_EMPTY_BUFFER_SOS_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_EMPTY_BUFFERS_SOS;
	}

	if (cause & NSS_N2H_INTR_PAGED_EMPTY_BUFFERS_SOS) {
		*type = NSS_INTR_CAUSE_NON_QUEUE;
		*weight = NSS_EMPTY_BUFFER_SOS_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_PAGED_EMPTY_BUFFERS_SOS;
	}

	if (cause & NSS_N2H_INTR_EMPTY_BUFFER_QUEUE) {
		*type = NSS_INTR_CAUSE_QUEUE;
		*weight = NSS_EMPTY_BUFFER_RETURN_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_EMPTY_BUFFER_QUEUE;
	}

	if (cause & NSS_N2H_INTR_TX_UNBLOCKED) {
		*type = NSS_INTR_CAUSE_NON_QUEUE;
		*weight = NSS_TX_UNBLOCKED_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_TX_UNBLOCKED;
	}

	if (cause & NSS_N2H_INTR_DATA_QUEUE_0) {
		*type = NSS_INTR_CAUSE_QUEUE;
		*weight = NSS_DATA_COMMAND_BUFFER_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_DATA_QUEUE_0;
	}

	if (cause & NSS_N2H_INTR_DATA_QUEUE_1) {
		*type = NSS_INTR_CAUSE_QUEUE;
		*weight = NSS_DATA_COMMAND_BUFFER_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_DATA_QUEUE_1;
	}

	if (cause & NSS_N2H_INTR_DATA_QUEUE_2) {
		*type = NSS_INTR_CAUSE_QUEUE;
		*weight = NSS_DATA_COMMAND_BUFFER_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_DATA_QUEUE_2;
	}

	if (cause & NSS_N2H_INTR_DATA_QUEUE_3) {
		*type = NSS_INTR_CAUSE_QUEUE;
		*weight = NSS_DATA_COMMAND_BUFFER_PROCESSING_WEIGHT;
		return NSS_N2H_INTR_DATA_QUEUE_3;
	}

	if (cause & NSS_N2H_INTR_COREDUMP_COMPLETE) {
		*type = NSS_INTR_CAUSE_EMERGENCY;
		return NSS_N2H_INTR_COREDUMP_COMPLETE;
	}

	if (cause & NSS_N2H_INTR_PROFILE_DMA) {
		*type = NSS_INTR_CAUSE_SDMA;
		return NSS_N2H_INTR_PROFILE_DMA;
	}

	return 0;
}

/*
 * nss_core_handle_napi()
 *	NAPI handler for NSS
 */
int nss_core_handle_napi(struct napi_struct *napi, int budget)
{
	int16_t processed, weight, count = 0;
	uint32_t prio_cause, int_cause = 0, cause_type;
	struct int_ctx_instance *int_ctx = container_of(napi, struct int_ctx_instance, napi);
	struct nss_ctx_instance *nss_ctx = int_ctx->nss_ctx;

	/*
	 * Read cause of interrupt
	 */
	nss_hal_read_interrupt_cause(nss_ctx, int_ctx->shift_factor, &int_cause);
	nss_hal_clear_interrupt_cause(nss_ctx, int_ctx->shift_factor, int_cause);
	int_ctx->cause |= int_cause;

	do {
		while ((int_ctx->cause) && (budget)) {

			/*
			 * Obtain the cause as per priority. Also obtain the weight
			 *
			 * NOTE: The idea is that all causes are processed as per priority and weight
			 * so that no single cause can overwhelm the system.
			 */
			prio_cause = nss_core_get_prioritized_cause(int_ctx->cause, &cause_type, &weight);
			if (budget < weight) {
				weight = budget;
			}

			processed = 0;
			switch (cause_type) {
			case NSS_INTR_CAUSE_QUEUE:
				processed = nss_core_handle_cause_queue(int_ctx, prio_cause, weight);

				count += processed;
				budget -= processed;

				/*
				 * If #packets processed were lesser than weight then processing for this queue/cause is
				 * complete and we can clear this interrupt cause from interrupt context structure
				 */
				if (processed < weight) {
					int_ctx->cause &= ~prio_cause;
				}
				break;

			case NSS_INTR_CAUSE_NON_QUEUE:
				nss_core_handle_cause_nonqueue(int_ctx, prio_cause, weight);
				int_ctx->cause &= ~prio_cause;
				break;

			case NSS_INTR_CAUSE_SDMA:
				nss_core_handle_napi_sdma(napi, budget);
				int_ctx->cause &= ~prio_cause;
				break;

			case NSS_INTR_CAUSE_EMERGENCY:
				nss_info_always("NSS core %d signal COREDUMP COMPLETE %x\n",
					nss_ctx->id, int_ctx->cause);
				nss_fw_coredump_notify(nss_ctx, prio_cause);
				int_ctx->cause &= ~prio_cause;
				break;

			default:
				nss_warning("%px: Invalid cause %x received from nss", nss_ctx, int_cause);
				nss_assert(0);
				break;
			}
		}

		nss_hal_read_interrupt_cause(nss_ctx, int_ctx->shift_factor, &int_cause);
		nss_hal_clear_interrupt_cause(nss_ctx, int_ctx->shift_factor, int_cause);
		int_ctx->cause |= int_cause;
	} while ((int_ctx->cause) && (budget));

	if (int_ctx->cause == 0) {
		napi_complete(napi);

		/*
		 * Re-enable any further interrupt from this IRQ
		 */
		nss_hal_enable_interrupt(nss_ctx, int_ctx->shift_factor, NSS_HAL_SUPPORTED_INTERRUPTS);
	}

	return count;
}

/*
 * nss_core_handle_napi_emergency()
 *	NAPI handler for NSS crash
 */
int nss_core_handle_napi_emergency(struct napi_struct *napi, int budget)
{
	struct int_ctx_instance *int_ctx = container_of(napi, struct int_ctx_instance, napi);

	nss_info_always("NSS core %d signal COREDUMP COMPLETE %x\n",
				int_ctx->nss_ctx->id, int_ctx->cause);
	nss_fw_coredump_notify(int_ctx->nss_ctx, 0);

	return 0;
}

/*
 * nss_core_handle_napi_sdma()
 *	NAPI handler for NSS soft DMA
 */
int nss_core_handle_napi_sdma(struct napi_struct *napi, int budget)
{
	struct int_ctx_instance *int_ctx = container_of(napi, struct int_ctx_instance, napi);
	struct nss_ctx_instance *nss_ctx = int_ctx->nss_ctx;
	struct nss_profile_sdma_ctrl *ctrl = (struct nss_profile_sdma_ctrl *)nss_ctx->meminfo_ctx.sdma_ctrl;

	if (ctrl->consumer[0].dispatch.fp)
		ctrl->consumer[0].dispatch.fp(ctrl->consumer[0].arg.kp);

#if !defined(NSS_HAL_IPQ806X_SUPPORT)
	napi_complete(napi);
	enable_irq(int_ctx->irq);
#endif
	return 0;
}

/*
 * nss_core_handle_napi_queue()
 *	NAPI handler for NSS queue cause
 */
int nss_core_handle_napi_queue(struct napi_struct *napi, int budget)
{
	int processed;
	struct int_ctx_instance *int_ctx = container_of(napi, struct int_ctx_instance, napi);

	processed = nss_core_handle_cause_queue(int_ctx, int_ctx->cause, budget);
	if (processed < budget) {
		napi_complete(napi);
		enable_irq(int_ctx->irq);
	}

	return processed;
}

/*
 * nss_core_handle_napi_non_queue()
 *	NAPI handler for NSS non queue cause
 */
int nss_core_handle_napi_non_queue(struct napi_struct *napi, int budget)
{
	struct int_ctx_instance *int_ctx = container_of(napi, struct int_ctx_instance, napi);

	nss_core_handle_cause_nonqueue(int_ctx, int_ctx->cause, 0);
	napi_complete(napi);
	enable_irq(int_ctx->irq);
	return 0;
}

/*
 * nss_core_write_one_descriptor()
 *	Fills-up a descriptor with required fields.
 */
static inline void nss_core_write_one_descriptor(struct h2n_descriptor *desc,
	uint16_t buffer_type, uint32_t buffer, uint32_t if_num,
	nss_ptr_t opaque, uint16_t payload_off, uint16_t payload_len, uint16_t buffer_len,
	uint32_t qos_tag, uint16_t mss, uint16_t bit_flags)
{
	desc->buffer_type = buffer_type;
	desc->buffer = buffer;
	desc->interface_num = if_num;
	desc->opaque = opaque;
	desc->payload_offs = payload_off;
	desc->payload_len = payload_len;
	desc->buffer_len = buffer_len;
	desc->qos_tag = qos_tag;
	desc->mss = mss;
	desc->bit_flags = bit_flags;
}

/*
* nss_core_send_unwind_dma()
*	It unwinds (or unmap) DMA from descriptors
*/
static inline void nss_core_send_unwind_dma(struct device *dev, struct h2n_desc_if_instance *desc_if,
	uint16_t hlos_index, int16_t count, bool is_fraglist)
{
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct h2n_descriptor *desc;
	int16_t i, mask;

	mask = desc_if->size - 1;
	for (i = 0; i < count; i++) {
		desc = &desc_ring[hlos_index];
		if (is_fraglist) {
			dma_unmap_single(dev, desc->buffer, desc->buffer_len, DMA_TO_DEVICE);
		} else {
			dma_unmap_page(dev, desc->buffer, desc->buffer_len, DMA_TO_DEVICE);
		}
		hlos_index = (hlos_index - 1) & mask;
	}
}

/*
 * nss_core_skb_tail_offset()
 */
static inline uint32_t nss_core_skb_tail_offset(struct sk_buff *skb)
{
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	return skb->tail;
#else
	return skb->tail - skb->head;
#endif
}

/*
 * nss_core_dma_map_single()
 */
static inline uint32_t nss_core_dma_map_single(struct device *dev, struct sk_buff *skb)
{
	return (uint32_t)dma_map_single(dev, skb->head, nss_core_skb_tail_offset(skb), DMA_TO_DEVICE);
}

#if (NSS_SKB_REUSE_SUPPORT == 1)
/*
 * nss_core_skb_can_reuse
 *	check if skb can be reuse
 */
static inline bool nss_core_skb_can_reuse(struct nss_ctx_instance *nss_ctx,
	uint32_t if_num, struct sk_buff *nbuf, int min_skb_size)
{
	/*
	 * If we have to call a destructor, we can't re-use the buffer?
	 */
	if (unlikely(nbuf->destructor != NULL)) {
		return false;
	}

	/*
	 * Check if skb has more than single user.
	 */
	if (unlikely(skb_shared(nbuf))) {
		return false;
	}

#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	/*
	 * This check is added to avoid deadlock from nf_conntrack
	 * when ecm is trying to flush a rule.
	 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
	if (unlikely(nbuf->nfct)) {
		return false;
	}
#else
	if (unlikely(nbuf->_nfct)) {
		return false;
	}
#endif
#endif

#ifdef CONFIG_BRIDGE_NETFILTER
	/*
	 * This check is added to avoid deadlock from nf_bridge
	 * when ecm is trying to flush a rule.
	 */
	if (unlikely(nf_bridge_info_get(nbuf))) {
		return false;
	}
#endif

	/*
	 * If skb has security parameters set do not reuse
	 */
	if (unlikely(skb_sec_path(nbuf))) {
		return false;
	}

	if (unlikely(irqs_disabled()))
		return false;

	if (unlikely(skb_shinfo(nbuf)->tx_flags & SKBTX_DEV_ZEROCOPY))
		return false;

	if (unlikely(skb_is_nonlinear(nbuf)))
		return false;

	if (unlikely(skb_has_frag_list(nbuf)))
		return false;

	if (unlikely(skb_shinfo(nbuf)->nr_frags))
		return false;

	if (unlikely(nbuf->fclone != SKB_FCLONE_UNAVAILABLE))
		return false;

	min_skb_size = SKB_DATA_ALIGN(min_skb_size + NET_SKB_PAD);
	if (unlikely(skb_end_pointer(nbuf) - nbuf->head < min_skb_size))
		return false;

	if (unlikely(skb_end_pointer(nbuf) - nbuf->head >= nss_core_get_max_reuse()))
		return false;

	if (unlikely(skb_cloned(nbuf)))
		return false;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
	if (unlikely(skb_pfmemalloc(nbuf)))
		return false;
#endif

	return true;
}

/*
 * nss_skb_reuse - clean up an skb
 *	Clears the skb to be reused as a receive buffer.
 *
 * NOTE: This function does any necessary reference count dropping, and
 * cleans up the skbuff as if its allocated fresh.
 */
void nss_skb_reuse(struct sk_buff *nbuf)
{
	struct skb_shared_info *shinfo;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
	u8 head_frag = nbuf->head_frag;
#endif

	/*
	 * Reset all the necessary head state information from skb which
	 * we found can be recycled for NSS.
	 */
	skb_dst_drop(nbuf);

	shinfo = skb_shinfo(nbuf);
	memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
	atomic_set(&shinfo->dataref, 1);

	memset(nbuf, 0, offsetof(struct sk_buff, tail));
	nbuf->data = nbuf->head + NET_SKB_PAD;
	skb_reset_tail_pointer(nbuf);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
	nbuf->head_frag = head_frag;
#endif
}
#endif

/*
 * nss_core_send_buffer_simple_skb()
 *	Sends one skb to NSS FW
 */
static inline int32_t nss_core_send_buffer_simple_skb(struct nss_ctx_instance *nss_ctx,
	struct h2n_desc_if_instance *desc_if, uint32_t if_num,
	struct sk_buff *nbuf, uint16_t hlos_index, uint16_t flags, uint8_t buffer_type, uint16_t mss)
{
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct h2n_descriptor *desc;
	uint16_t bit_flags;
	uint16_t mask;
	uint32_t frag0phyaddr;

#if (NSS_SKB_REUSE_SUPPORT == 1)
	uint16_t sz;
#endif

	bit_flags = flags | H2N_BIT_FLAG_FIRST_SEGMENT | H2N_BIT_FLAG_LAST_SEGMENT;
	if (likely(nbuf->ip_summed == CHECKSUM_PARTIAL)) {
		bit_flags |= H2N_BIT_FLAG_GEN_IP_TRANSPORT_CHECKSUM;
		bit_flags |= H2N_BIT_FLAG_GEN_IPV4_IP_CHECKSUM;
	} else if (nbuf->ip_summed == CHECKSUM_UNNECESSARY) {
		bit_flags |= H2N_BIT_FLAG_GEN_IP_TRANSPORT_CHECKSUM_NONE;
	}

	mask = desc_if->size - 1;
	desc = &desc_ring[hlos_index];

#if (NSS_SKB_REUSE_SUPPORT == 1)
	/*
	 * Check if the caller indicates that the buffer is not to be re-used (kept in the accelerator).
	 */
	if (unlikely(!(bit_flags & H2N_BIT_FLAG_BUFFER_REUSABLE))) {
		goto no_reuse;
	}

	/*
	 * Since the caller is allowing re-use, we now check if the skb meets the criteria.
	 */
	if (unlikely(!nss_core_skb_can_reuse(nss_ctx, if_num, nbuf, nss_ctx->max_buf_size))) {
		goto no_reuse;
	}

	/*
	 * We are going to do both Tx and then Rx on this buffer, unmap the Tx
	 * and then map Rx over the entire buffer.
	 */
	sz = max((uint16_t)nss_core_skb_tail_offset(nbuf), (uint16_t)(nss_ctx->max_buf_size + NET_SKB_PAD));
	frag0phyaddr = (uint32_t)dma_map_single(nss_ctx->dev, nbuf->head, sz, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(nss_ctx->dev, frag0phyaddr))) {
		goto no_reuse;
	}

	/*
	 * We are allowed to re-use the packet
	 */
	nss_core_write_one_descriptor(desc, buffer_type, frag0phyaddr, if_num,
		(nss_ptr_t)nbuf, (uint16_t)(nbuf->data - nbuf->head), nbuf->len,
		sz, (uint32_t)nbuf->priority, mss, bit_flags);

	NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

	/*
	 * We are done using the skb fields and can reuse it now
	 */
	nss_skb_reuse(nbuf);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_BUFFER_REUSE]);
	return 1;

no_reuse:
#endif

	bit_flags &= ~H2N_BIT_FLAG_BUFFER_REUSABLE;
	frag0phyaddr = nss_core_dma_map_single(nss_ctx->dev, nbuf);
	if (unlikely(dma_mapping_error(nss_ctx->dev, frag0phyaddr))) {
		nss_warning("%px: DMA mapping failed for virtual address = %px", nss_ctx, nbuf->head);
		return 0;
	}

	nss_core_write_one_descriptor(desc, buffer_type, frag0phyaddr, if_num,
		(nss_ptr_t)nbuf, (uint16_t)(nbuf->data - nbuf->head), nbuf->len,
		(uint16_t)skb_end_offset(nbuf), (uint32_t)nbuf->priority, mss, bit_flags);

	NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_SIMPLE]);
	return 1;
}

/*
 * nss_core_send_buffer_nr_frags()
 *	Sends frags array (NETIF_F_SG) to NSS FW
 *
 * Note - Opaque is set only on LAST fragment, and DISCARD is set for the rest of segments
 * Used to differentiate from FRAGLIST
 */
static inline int32_t nss_core_send_buffer_nr_frags(struct nss_ctx_instance *nss_ctx,
	struct h2n_desc_if_instance *desc_if, uint32_t if_num,
	struct sk_buff *nbuf, uint16_t hlos_index, uint16_t flags, uint8_t buffer_type, uint16_t mss)
{
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct h2n_descriptor *desc;
	const skb_frag_t *frag;
	dma_addr_t buffer;
	uint32_t nr_frags;
	uint16_t bit_flags;
	int16_t i;
	uint16_t mask;

	uint32_t frag0phyaddr = nss_core_dma_map_single(nss_ctx->dev, nbuf);
	if (unlikely(dma_mapping_error(nss_ctx->dev, frag0phyaddr))) {
		nss_warning("%px: DMA mapping failed for virtual address = %px", nss_ctx, nbuf->head);
		return 0;
	}

	/*
	 * Set the appropriate flags.
	 */
	bit_flags = (flags | H2N_BIT_FLAG_DISCARD);

	/*
	 * Reset the reuse flag for non-linear buffers.
	 */
	bit_flags &= ~H2N_BIT_FLAG_BUFFER_REUSABLE;
	if (likely(nbuf->ip_summed == CHECKSUM_PARTIAL)) {
		bit_flags |= H2N_BIT_FLAG_GEN_IP_TRANSPORT_CHECKSUM;
		bit_flags |= H2N_BIT_FLAG_GEN_IPV4_IP_CHECKSUM;
	}

	mask = desc_if->size - 1;
	desc = &desc_ring[hlos_index];

	/*
	 * First fragment/descriptor is special
	 */
	nss_core_write_one_descriptor(desc, buffer_type, frag0phyaddr, if_num,
		(nss_ptr_t)NULL, nbuf->data - nbuf->head, nbuf->len - nbuf->data_len,
		skb_end_offset(nbuf), (uint32_t)nbuf->priority, mss, bit_flags | H2N_BIT_FLAG_FIRST_SEGMENT);

	NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

	/*
	 * Now handle rest of the fragments.
	 */
	nr_frags = skb_shinfo(nbuf)->nr_frags;
	BUG_ON(nr_frags > MAX_SKB_FRAGS);
	for (i = 0; i < nr_frags; i++) {
		frag = &skb_shinfo(nbuf)->frags[i];

		buffer = skb_frag_dma_map(nss_ctx->dev, frag, 0, skb_frag_size(frag), DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(nss_ctx->dev, buffer))) {
			nss_warning("%px: DMA mapping failed for fragment", nss_ctx);
			nss_core_send_unwind_dma(nss_ctx->dev, desc_if, hlos_index, i + 1, false);
			return -(i + 1);
		}

		hlos_index = (hlos_index + 1) & (mask);
		desc = &(desc_if->desc[hlos_index]);

		nss_core_write_one_descriptor(desc, buffer_type, buffer, if_num,
			(nss_ptr_t)NULL, 0, skb_frag_size(frag), skb_frag_size(frag),
			nbuf->priority, mss, bit_flags);

		NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);
	}

	/*
	 * Update bit flag for last descriptor.
	 * The discard flag shall be set for all fragments except the
	 * the last one.The NSS returns the last fragment to HLOS
	 * after the packet processing is done.We do need to send the
	 * packet buffer address (skb) in the descriptor of last segment
	 * when the decriptor returns from NSS the HLOS uses the
	 * opaque field to free the memory allocated.
	 */
	desc->bit_flags |= H2N_BIT_FLAG_LAST_SEGMENT;
	desc->bit_flags &= ~(H2N_BIT_FLAG_DISCARD);
	desc->opaque = (nss_ptr_t)nbuf;

	NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_NR_FRAGS]);
	return i+1;
}

/*
 * nss_core_send_buffer_fraglist()
 *	Sends fraglist (NETIF_F_FRAGLIST) to NSS FW
 *
 * Note - Opaque will be set on all fragments, and DISCARD is set for the rest of segments
 * Used to differentiate from FRAGS
 */
static inline int32_t nss_core_send_buffer_fraglist(struct nss_ctx_instance *nss_ctx,
	struct h2n_desc_if_instance *desc_if, uint32_t if_num,
	struct sk_buff *nbuf, uint16_t hlos_index, uint16_t flags, uint8_t buffer_type, uint16_t mss)
{
	struct h2n_descriptor *desc_ring = desc_if->desc;
	struct h2n_descriptor *desc;
	dma_addr_t buffer;
	uint16_t mask;
	struct sk_buff *iter;
	uint16_t bit_flags;
	int16_t i;

	uint32_t frag0phyaddr = nss_core_dma_map_single(nss_ctx->dev, nbuf);
	if (unlikely(dma_mapping_error(nss_ctx->dev, frag0phyaddr))) {
		nss_warning("%px: DMA mapping failed for virtual address = %px", nss_ctx, nbuf->head);
		return 0;
	}

	/*
	 * Copy and Set bit flags
	 */
	bit_flags = flags;

	/*
	 * Reset the reuse flag for non-linear buffers.
	 */
	bit_flags &= ~H2N_BIT_FLAG_BUFFER_REUSABLE;
	if (likely(nbuf->ip_summed == CHECKSUM_PARTIAL)) {
		bit_flags |= H2N_BIT_FLAG_GEN_IP_TRANSPORT_CHECKSUM;
		bit_flags |= H2N_BIT_FLAG_GEN_IPV4_IP_CHECKSUM;
	}

	mask = desc_if->size - 1;
	desc = &desc_ring[hlos_index];

	/*
	 * First fragment/descriptor is special. Will hold the Opaque
	 */
	nss_core_write_one_descriptor(desc, buffer_type, frag0phyaddr, if_num,
		(nss_ptr_t)nbuf, nbuf->data - nbuf->head, nbuf->len - nbuf->data_len,
		skb_end_offset(nbuf), (uint32_t)nbuf->priority, mss, bit_flags | H2N_BIT_FLAG_FIRST_SEGMENT);

	NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

	/*
	 * Walk the frag_list in nbuf
	 */
	i = 0;
	skb_walk_frags(nbuf, iter) {
		uint32_t nr_frags;

		buffer = nss_core_dma_map_single(nss_ctx->dev, iter);
		if (unlikely(dma_mapping_error(nss_ctx->dev, buffer))) {
			nss_warning("%px: DMA mapping failed for virtual address = %px", nss_ctx, iter->head);
			nss_core_send_unwind_dma(nss_ctx->dev, desc_if, hlos_index, i + 1, true);
			return -(i+1);
		}

		/*
		 * We currently don't support frags[] array inside a
		 * fraglist.
		 */
		nr_frags = skb_shinfo(iter)->nr_frags;
		if (unlikely(nr_frags > 0)) {
			nss_warning("%px: fraglist with page data are not supported: %px\n", nss_ctx, iter);
			nss_core_send_unwind_dma(nss_ctx->dev, desc_if, hlos_index, i + 1, true);
			return -(i+1);
		}

		/*
		 * Update index.
		 */
		hlos_index = (hlos_index + 1) & (mask);
		desc = &(desc_if->desc[hlos_index]);

#ifdef CONFIG_DEBUG_KMEMLEAK
		/*
		 * We are holding this skb in NSS FW, let kmemleak know about it.
		 *
		 * If the skb is a fast clone (FCLONE), then nbuf is pointing to the
		 * cloned skb which is at the middle of the allocated block and kmemleak API
		 * would backtrace if passed such a pointer. We will need to get to the original
		 * skb pointer which kmemleak is aware of.
		 */
		if (iter->fclone == SKB_FCLONE_CLONE) {
			kmemleak_not_leak(iter - 1);
		} else {
			kmemleak_not_leak(iter);
		}
#endif

		nss_core_write_one_descriptor(desc, buffer_type, buffer, if_num,
			(nss_ptr_t)iter, iter->data - iter->head, iter->len - iter->data_len,
			skb_end_offset(iter), iter->priority, mss, bit_flags);

		NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

		i++;
	}

	/*
	 * We need to defrag the frag_list, otherwise, if this structure is
	 * received back we don't know how we can reconstruct the frag_list.
	 * Therefore, we are clearing skb_has_fraglist. This is safe because all
	 * information about the segments are already sent to NSS-FW.
	 * So, the information will be in the NSS-FW.
	 */
	skb_shinfo(nbuf)->frag_list = NULL;
	NSS_PKT_STATS_ADD(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT], i);

	/*
	 * Update bit flag for last descriptor.
	 */
	desc->bit_flags |= H2N_BIT_FLAG_LAST_SEGMENT;
	NSS_CORE_DMA_CACHE_MAINT((void *)desc, sizeof(*desc), DMA_TO_DEVICE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_FRAGLIST]);
	return i+1;
}

/*
 * nss_core_init_handlers()
 *	Initialize the handlers for all interfaces associated with core
 */
void nss_core_init_handlers(struct nss_ctx_instance *nss_ctx)
{
	struct nss_rx_cb_list *cb_list = nss_ctx->nss_rx_interface_handlers;
	memset(cb_list, 0, sizeof(*cb_list) * NSS_MAX_NET_INTERFACES);
}

/*
 * nss_core_send_buffer()
 *	Send network buffer to NSS
 */
int32_t nss_core_send_buffer(struct nss_ctx_instance *nss_ctx, uint32_t if_num,
					struct sk_buff *nbuf, uint16_t qid,
					uint8_t buffer_type, uint16_t flags)
{
	int16_t count, hlos_index, nss_index, size, mask;
	uint32_t segments;
	struct hlos_h2n_desc_rings *h2n_desc_ring = &nss_ctx->h2n_desc_rings[qid];
	struct h2n_desc_if_instance *desc_if = &h2n_desc_ring->desc_ring;
	struct h2n_descriptor *desc_ring;
	struct h2n_descriptor *desc;
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;
	struct nss_if_mem_map *if_map = mem_ctx->if_map;
	uint16_t mss = 0;
	bool is_bounce = ((buffer_type == H2N_BUFFER_SHAPER_BOUNCE_INTERFACE) || (buffer_type == H2N_BUFFER_SHAPER_BOUNCE_BRIDGE));

	desc_ring = desc_if->desc;
	size = desc_if->size;
	mask = size - 1;

	/*
	 * If nbuf does not have fraglist, then update nr_frags
	 * from frags[] array. Otherwise walk the frag_list.
	 */
	if (!skb_has_frag_list(nbuf)) {
		segments = skb_shinfo(nbuf)->nr_frags;
		BUG_ON(segments > MAX_SKB_FRAGS);
	} else {
		struct sk_buff *iter;
		segments = 0;
		skb_walk_frags(nbuf, iter) {
			segments++;
		}

		/*
		 * Check that segments do not overflow the number of descriptors
		 */
		if (unlikely(segments > size)) {
			nss_warning("%px: Unable to fit in skb - %d segments in our descriptors", nss_ctx, segments);
			return NSS_CORE_STATUS_FAILURE;
		}
	}

	/*
	 * Take a lock for queue
	 */
	spin_lock_bh(&h2n_desc_ring->lock);

	/*
	 * We need to work out if there's sufficent space in our transmit descriptor
	 * ring to place all the segments of a nbuf.
	 */
	NSS_CORE_DMA_CACHE_MAINT((void *)&if_map->h2n_nss_index[qid], sizeof(uint32_t), DMA_FROM_DEVICE);
	NSS_CORE_DSB();
	nss_index = if_map->h2n_nss_index[qid];

	hlos_index = h2n_desc_ring->hlos_index;

	count = ((nss_index - hlos_index - 1) + size) & (mask);

	if (unlikely(count < (segments + 1))) {
		/*
		 * NOTE: tx_q_full_cnt and TX_STOPPED flags will be used
		 *	when we will add support for DESC Q congestion management
		 *	in future
		 */
		h2n_desc_ring->tx_q_full_cnt++;
		h2n_desc_ring->flags |= NSS_H2N_DESC_RING_FLAGS_TX_STOPPED;
		spin_unlock_bh(&h2n_desc_ring->lock);
		nss_warning("%px: Data/Command Queue full reached", nss_ctx);

#if (NSS_PKT_STATS_ENABLED == 1)
		if (nss_ctx->id == NSS_CORE_0) {
			NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_QUEUE_FULL_0]);
		} else if (nss_ctx->id == NSS_CORE_1) {
			NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_QUEUE_FULL_1]);
		} else {
			nss_warning("%px: Invalid nss core: %d\n", nss_ctx, nss_ctx->id);
		}
#endif

		/*
		 * Enable de-congestion interrupt from NSS
		 */
		nss_hal_enable_interrupt(nss_ctx, nss_ctx->int_ctx[0].shift_factor, NSS_N2H_INTR_TX_UNBLOCKED);

		return NSS_CORE_STATUS_FAILURE_QUEUE;
	}

	desc = &desc_ring[hlos_index];

	/*
	 * Check if segmentation enabled.
	 * Configure descriptor bit flags accordingly
	 */

	/*
	 * When CONFIG_HIGHMEM is enabled OS is giving a single big chunk buffer without
	 * any scattered frames.
	 *
	 * NOTE: We dont have to perform segmentation offload for packets that are being
	 * bounced. These packets WILL return to the HLOS for freeing or further processing.
	 * They will NOT be transmitted by the NSS.
	 */
	if (skb_is_gso(nbuf) && !is_bounce) {
		mss = skb_shinfo(nbuf)->gso_size;
		flags |= H2N_BIT_FLAG_SEGMENTATION_ENABLE;
	}

	/*
	 * WARNING! : The following "is_bounce" check has a potential to cause corruption
	 * if things change in the NSS. This check allows fragmented packets to be sent down
	 * with incomplete payload information since NSS does not care about the payload content
	 * when packets are bounced for shaping. If it starts caring in future, then this code
	 * will have to change.
	 *
	 * WHY WE ARE DOING THIS - Skipping S/G processing helps with performance.
	 *
	 */
	count = 0;
	if (likely((segments == 0) || is_bounce)) {
		count = nss_core_send_buffer_simple_skb(nss_ctx, desc_if, if_num,
			nbuf, hlos_index, flags, buffer_type, mss);
	} else if (skb_has_frag_list(nbuf)) {
		count = nss_core_send_buffer_fraglist(nss_ctx, desc_if, if_num,
			nbuf, hlos_index, flags, buffer_type, mss);
	} else {
		count = nss_core_send_buffer_nr_frags(nss_ctx, desc_if, if_num,
			nbuf, hlos_index, flags, buffer_type, mss);
	}

	if (unlikely(count <= 0)) {
		/*
		 * We failed and hence we need to unmap dma regions
		 */
		nss_warning("%px: failed to map DMA regions:%d", nss_ctx, -count);
		spin_unlock_bh(&h2n_desc_ring->lock);
		return NSS_CORE_STATUS_FAILURE;
	}

	/*
	 * Sync to ensure all flushing of the descriptors are complete
	 */
	NSS_CORE_DSB();

	/*
	 * Update our host index so the NSS sees we've written a new descriptor.
	 */
	hlos_index = (hlos_index + count) & mask;
	h2n_desc_ring->hlos_index = hlos_index;
	if_map->h2n_hlos_index[qid] = hlos_index;

	NSS_CORE_DMA_CACHE_MAINT(&if_map->h2n_hlos_index[qid], sizeof(uint32_t), DMA_TO_DEVICE);
	NSS_CORE_DSB();

#ifdef CONFIG_DEBUG_KMEMLEAK
	/*
	 * We are holding this skb in NSS FW, let kmemleak know about it.
	 *
	 * If the skb is a fast clone (FCLONE), then nbuf is pointing to the
	 * cloned skb which is at the middle of the allocated block and kmemleak API
	 * would backtrace if passed such a pointer. We will need to get to the original
	 * skb pointer which kmemleak is aware of.
	 */
	if (nbuf->fclone == SKB_FCLONE_CLONE) {
		kmemleak_not_leak(nbuf - 1);
	} else {
		kmemleak_not_leak(nbuf);
	}
#endif

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NSS_SKB_COUNT]);

	spin_unlock_bh(&h2n_desc_ring->lock);
	return NSS_CORE_STATUS_SUCCESS;
}

/*
 * nss_core_send_cmd()
 *	Send command message to NSS
 */
int32_t nss_core_send_cmd(struct nss_ctx_instance *nss_ctx, void *msg, int size, int buf_size)
{
	struct nss_cmn_msg *ncm = (struct nss_cmn_msg *)msg;
	int32_t status;
	struct sk_buff *nbuf;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (unlikely(nss_ctx->state != NSS_CORE_STATE_INITIALIZED)) {
		nss_warning("%px: interface: %d type: %d message dropped as core not ready\n", nss_ctx, ncm->interface, ncm->type);
		return NSS_TX_FAILURE_NOT_READY;
	}

	if (nss_cmn_get_msg_len(ncm) > size) {
		nss_warning("%px: interface: %d type: %d message length %d is invalid, size = %d\n",
					nss_ctx, ncm->interface, ncm->type, nss_cmn_get_msg_len(ncm), size);
		return NSS_TX_FAILURE_TOO_LARGE;
	}

	if (buf_size > PAGE_SIZE) {
		nss_warning("%px: interface: %d type: %d tx request size too large: %u",
					nss_ctx, ncm->interface, ncm->type, buf_size);
		return NSS_TX_FAILURE_BAD_PARAM;
	}

	nbuf = dev_alloc_skb(buf_size);
	if (unlikely(!nbuf)) {
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_NBUF_ALLOC_FAILS]);
		nss_warning("%px: interface: %d type: %d msg dropped as command allocation failed", nss_ctx, ncm->interface, ncm->type);
		return NSS_TX_FAILURE;
	}

	memcpy(skb_put(nbuf, buf_size), (void *)ncm, size);

	status = nss_core_send_buffer(nss_ctx, 0, nbuf, NSS_IF_H2N_CMD_QUEUE, H2N_BUFFER_CTRL, H2N_BIT_FLAG_BUFFER_REUSABLE);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		dev_kfree_skb_any(nbuf);
		NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_CMD_QUEUE_FULL]);
		nss_warning("%px: interface: %d type: %d unable to enqueue message status %d\n", nss_ctx, ncm->interface, ncm->type, status);
		return status;
	}

	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_CMD_REQ]);
	return status;
}

/*
 * nss_core_send_packet()
 *	Send data packet to NSS
 */
int32_t nss_core_send_packet(struct nss_ctx_instance *nss_ctx, struct sk_buff *nbuf, uint32_t if_num, uint32_t flag)
{
	int32_t status;
	int32_t queue_id = 0;

	NSS_VERIFY_CTX_MAGIC(nss_ctx);
	if (unlikely(nss_ctx->state != NSS_CORE_STATE_INITIALIZED)) {
		nss_warning("%px: interface: %d packet dropped as core not ready\n", nss_ctx, if_num);
		return NSS_TX_FAILURE_NOT_READY;
	}

#ifdef NSS_MULTI_H2N_DATA_RING_SUPPORT
	queue_id = (skb_get_queue_mapping(nbuf) & (NSS_HOST_CORES - 1)) << 1;
	if (nbuf->priority) {
		queue_id++;
	}
#endif
	status = nss_core_send_buffer(nss_ctx, if_num, nbuf, NSS_IF_H2N_DATA_QUEUE + queue_id, H2N_BUFFER_PACKET, flag);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_warning("%px: interface: %d unable to enqueue packet status %d\n", nss_ctx, if_num, status);
		return status;
	}

	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);

#ifdef NSS_MULTI_H2N_DATA_RING_SUPPORT
	/*
	 * Count per queue and aggregate packet count
	 */
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_PACKET_QUEUE_0 + queue_id]);
#endif
	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_PACKET]);
	return status;
}

/*
 * nss_core_ddr_info()
 *	Getting DDR information for NSS core
 */
uint32_t nss_core_ddr_info(struct nss_mmu_ddr_info *mmu)
{
	nss_get_ddr_info(mmu, "memory");
	return nss_soc_mem_info();
}
