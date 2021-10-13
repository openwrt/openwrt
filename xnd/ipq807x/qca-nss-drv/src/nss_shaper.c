/*
 **************************************************************************
 * Copyright (c) 2014, 2016-2020 The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"

/*
 * nss_shaper_register_shaping()
 *	Register to obtain an NSS context for basic shaping operations
 */
void *nss_shaper_register_shaping(void)
{
	if (nss_top_main.shaping_handler_id == (uint8_t)-1) {
		nss_warning("%px: SHAPING IS NOT ENABLED", __func__);
		return NULL;
	}
	return (void *)&nss_top_main.nss[nss_top_main.shaping_handler_id];
}

/*
 * nss_shaper_unregister_shaping()
 *	Unregister an NSS shaping context
 */
void nss_shaper_unregister_shaping(void *nss_ctx)
{
}

/*
 * nss_shaper_register_shaper_bounce_interface()
 *	Register for performing shaper bounce operations for interface shaper
 */
void *nss_shaper_register_shaper_bounce_interface(uint32_t if_num, nss_shaper_bounced_callback_t cb, void *app_data, struct module *owner)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_shaper_bounce_registrant *reg;

	nss_info("Shaper bounce interface register: %u, cb: %px, app_data: %px, owner: %px",
			if_num, cb, app_data, owner);

	/*
	 * Must be valid interface number
	 */
	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Invalid if_num: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Shaping enabled?
	 */
	if (nss_top_main.shaping_handler_id == (uint8_t)-1) {
		nss_warning("%px: SHAPING IS NOT ENABLED", __func__);
		return NULL;
	}

	/*
	 * Can we hold the module?
	 */
	if (!try_module_get(owner)) {
		nss_warning("%px: Unable to hold owner", __func__);
		return NULL;
	}

	spin_lock_bh(&nss_top->lock);

	/*
	 * Must not have existing registrant
	 */
	reg = &nss_top->bounce_interface_registrants[if_num];
	if (reg->registered) {
		spin_unlock_bh(&nss_top->stats_lock);
		module_put(owner);
		nss_warning("Already registered: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Register
	 */
	reg->bounced_callback = cb;
	reg->app_data = app_data;
	reg->owner = owner;
	reg->registered = true;
	spin_unlock_bh(&nss_top->lock);

	return (void *)&nss_top->nss[nss_top->shaping_handler_id];
}

/*
 * nss_shaper_unregister_shaper_bounce_interface()
 *	Unregister for shaper bounce operations for interface shaper
 */
void nss_shaper_unregister_shaper_bounce_interface(uint32_t if_num)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_shaper_bounce_registrant *reg;
	struct module *owner;

	nss_info("Shaper bounce interface unregister: %u", if_num);

	/*
	 * Must be valid interface number
	 */
	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Invalid if_num: %u", if_num);
		BUG_ON(false);
	}

	spin_lock_bh(&nss_top->lock);

	/*
	 * Must have existing registrant
	 */
	reg = &nss_top->bounce_interface_registrants[if_num];
	if (!reg->registered) {
		spin_unlock_bh(&nss_top->stats_lock);
		nss_warning("Already unregistered: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Unegister
	 */
	owner = reg->owner;
	reg->owner = NULL;
	reg->registered = false;
	spin_unlock_bh(&nss_top->lock);

	module_put(owner);
}

/*
 * nss_shaper_register_shaper_bounce_bridge()
 *	Register for performing shaper bounce operations for bridge shaper
 */
void *nss_shaper_register_shaper_bounce_bridge(uint32_t if_num, nss_shaper_bounced_callback_t cb, void *app_data, struct module *owner)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx;
	struct nss_shaper_bounce_registrant *reg;

	nss_info("Shaper bounce bridge register: %u, cb: %px, app_data: %px, owner: %px",
			if_num, cb, app_data, owner);

	/*
	 * Must be valid interface number
	 */
	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Invalid if_num: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Shaping enabled?
	 */
	if (nss_top_main.shaping_handler_id == (uint8_t)-1) {
		nss_warning("%px: SHAPING IS NOT ENABLED", __func__);
		return NULL;
	}

	/*
	 * Can we hold the module?
	 */
	if (!try_module_get(owner)) {
		nss_warning("%px: Unable to hold owner", __func__);
		return NULL;
	}

	spin_lock_bh(&nss_top->lock);

	/*
	 * Must not have existing registrant
	 */
	reg = &nss_top->bounce_bridge_registrants[if_num];
	if (reg->registered) {
		spin_unlock_bh(&nss_top->stats_lock);
		module_put(owner);
		nss_warning("Already registered: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Register
	 */
	reg->bounced_callback = cb;
	reg->app_data = app_data;
	reg->owner = owner;
	reg->registered = true;
	spin_unlock_bh(&nss_top->lock);

	nss_ctx = &nss_top->nss[nss_top->shaping_handler_id];
	return (void *)nss_ctx;
}

/*
 * nss_shaper_unregister_shaper_bounce_bridge()
 *	Unregister for shaper bounce operations for bridge shaper
 */
void nss_shaper_unregister_shaper_bounce_bridge(uint32_t if_num)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_shaper_bounce_registrant *reg;
	struct module *owner;

	nss_info("Shaper bounce bridge unregister: %u", if_num);

	/*
	 * Must be valid interface number
	 */
	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Invalid if_num: %u", if_num);
		BUG_ON(false);
	}

	spin_lock_bh(&nss_top->lock);

	/*
	 * Must have existing registrant
	 */
	reg = &nss_top->bounce_bridge_registrants[if_num];
	if (!reg->registered) {
		spin_unlock_bh(&nss_top->stats_lock);
		nss_warning("Already unregistered: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Wait until any bounce callback that is active is finished
	 */
	while (reg->callback_active) {
		spin_unlock_bh(&nss_top->stats_lock);
		yield();
		spin_lock_bh(&nss_top->stats_lock);
	}

	/*
	 * Unegister
	 */
	owner = reg->owner;
	reg->owner = NULL;
	reg->registered = false;
	spin_unlock_bh(&nss_top->lock);

	module_put(owner);
}

/*
 * nss_shaper_bounce_interface_packet()
 *	Bounce a packet to the NSS for interface shaping.
 *
 * You must have registered for interface bounce shaping to call this.
 */
nss_tx_status_t nss_shaper_bounce_interface_packet(void *ctx, uint32_t if_num, struct sk_buff *skb)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)ctx;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_shaper_bounce_registrant *reg;
	int32_t status;

	/*
	 * Must be valid interface number
	 */
	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Invalid if_num: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Must have existing registrant
	 */
	spin_lock_bh(&nss_top->lock);
	reg = &nss_top->bounce_interface_registrants[if_num];
	if (!reg->registered) {
		spin_unlock_bh(&nss_top->stats_lock);
		nss_warning("unregistered: %u", if_num);
		return NSS_TX_FAILURE;
	}
	spin_unlock_bh(&nss_top->lock);

	status = nss_core_send_buffer(nss_ctx, if_num, skb, NSS_IF_H2N_DATA_QUEUE,
					H2N_BUFFER_SHAPER_BOUNCE_INTERFACE, 0);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		return NSS_TX_FAILURE;
	}
	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_PACKET]);
	return NSS_TX_SUCCESS;
}

/*
 * nss_shaper_bounce_bridge_packet()
 *	Bounce a packet to the NSS for bridge shaping.
 *
 * You must have registered for bridge bounce shaping to call this.
 */
nss_tx_status_t nss_shaper_bounce_bridge_packet(void *ctx, uint32_t if_num, struct sk_buff *skb)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)ctx;
	struct nss_top_instance *nss_top = nss_ctx->nss_top;
	struct nss_shaper_bounce_registrant *reg;
	int32_t status;

	/*
	 * Must be valid interface number
	 */
	if (if_num >= NSS_MAX_NET_INTERFACES) {
		nss_warning("Invalid if_num: %u", if_num);
		BUG_ON(false);
	}

	/*
	 * Must have existing registrant
	 */
	spin_lock_bh(&nss_top->lock);
	reg = &nss_top->bounce_bridge_registrants[if_num];
	if (!reg->registered) {
		spin_unlock_bh(&nss_top->stats_lock);
		nss_warning("unregistered: %u", if_num);
		return NSS_TX_FAILURE;
	}
	spin_unlock_bh(&nss_top->lock);

	nss_info("%s: Bridge bounce skb: %px, if_num: %u, ctx: %px", __func__, skb, if_num, nss_ctx);
	status = nss_core_send_buffer(nss_ctx, if_num, skb, NSS_IF_H2N_DATA_QUEUE,
					H2N_BUFFER_SHAPER_BOUNCE_BRIDGE, 0);
	if (status != NSS_CORE_STATUS_SUCCESS) {
		nss_info("%s: Bridge bounce core send rejected", __func__);
		return NSS_TX_FAILURE;
	}
	nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_DATA_COMMAND_QUEUE);

	NSS_PKT_STATS_INC(&nss_ctx->nss_top->stats_drv[NSS_DRV_STATS_TX_PACKET]);
	return NSS_TX_SUCCESS;
}

/*
 * nss_shaper_get_device()
 *	Gets the original device from probe.
 */
struct device *nss_shaper_get_dev(void)
{
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[nss_top_main.shaping_handler_id];
	return nss_ctx->dev;
}

EXPORT_SYMBOL(nss_shaper_bounce_bridge_packet);
EXPORT_SYMBOL(nss_shaper_bounce_interface_packet);
EXPORT_SYMBOL(nss_shaper_unregister_shaper_bounce_interface);
EXPORT_SYMBOL(nss_shaper_register_shaper_bounce_interface);
EXPORT_SYMBOL(nss_shaper_unregister_shaper_bounce_bridge);
EXPORT_SYMBOL(nss_shaper_register_shaper_bounce_bridge);
EXPORT_SYMBOL(nss_shaper_register_shaping);
EXPORT_SYMBOL(nss_shaper_unregister_shaping);
EXPORT_SYMBOL(nss_shaper_get_dev);
