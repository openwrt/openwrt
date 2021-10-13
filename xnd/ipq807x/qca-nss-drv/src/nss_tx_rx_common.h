/*
 **************************************************************************
 * Copyright (c) 2013-2020, The Linux Foundation. All rights reserved.
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
 * nss_tx_rx_common.h
 *	NSS APIs common header file
 */

#ifndef __NSS_TX_RX_COMMON_H
#define __NSS_TX_RX_COMMON_H

#include <nss_hal.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/kmemleak.h>
#include "nss_tx_msg_sync.h"

/*
 * Global definitions
 */
#define NSS_HLOS_MESSAGE_VERSION 1	/* Update when the common message structure changed */

#if (NSS_DEBUG_LEVEL > 0)
#define NSS_VERIFY_CTX_MAGIC(x) nss_verify_ctx_magic(x)
#define NSS_VERIFY_INIT_DONE(x) nss_verify_init_done(x)

/*
 * nss_verify_ctx_magic()
 */
static inline void nss_verify_ctx_magic(struct nss_ctx_instance *nss_ctx)
{
	nss_assert(nss_ctx->magic == NSS_CTX_MAGIC);
}

static inline void nss_verify_init_done(struct nss_ctx_instance *nss_ctx)
{
	nss_assert(nss_ctx->state == NSS_CORE_STATE_INITIALIZED);
}

#else
#define NSS_VERIFY_CTX_MAGIC(x)
#define NSS_VERIFY_INIT_DONE(x)
#endif

/*
 * CB handlers for variour interfaces
 */
void nss_phys_if_register_handler(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern void nss_c2c_tx_register_handler(struct nss_ctx_instance *nss_ctx);
extern void nss_c2c_rx_register_handler(struct nss_ctx_instance *nss_ctx);
extern void nss_crypto_register_handler(void);
extern void nss_crypto_cmn_register_handler(void);
extern void nss_ipsec_register_handler(void);
extern void nss_ipsec_cmn_register_handler(void);
extern void nss_ipv4_register_handler(void);
extern void nss_ipv4_reasm_register_handler(void);
extern void nss_ipv6_register_handler(void);
extern void nss_ipv6_reasm_register_handler(void);
extern void nss_n2h_register_handler(struct nss_ctx_instance *nss_ctx);
extern void nss_tunipip6_register_handler(void);
extern void nss_pppoe_register_handler(void);
extern void nss_freq_register_handler(void);
extern void nss_eth_rx_register_handler(struct nss_ctx_instance *nss_ctx);
extern void nss_edma_register_handler(void);
extern void nss_lag_register_handler(void);
extern void nss_dynamic_interface_register_handler(struct nss_ctx_instance *nss_ctx);
extern void nss_gre_redir_register_handler(void);
extern void nss_gre_redir_lag_us_register_handler(void);
extern void nss_gre_redir_lag_ds_register_handler(void);
extern void nss_lso_rx_register_handler(struct nss_ctx_instance *nss_ctx);
extern void nss_sjack_register_handler(void);
extern void nss_wifi_register_handler(void);
extern struct net_device *nss_tstamp_register_netdev(void);
extern void nss_tstamp_register_handler(struct net_device *ndev);
extern void nss_portid_register_handler(void);
extern void nss_oam_register_handler(void);
extern void nss_dtls_register_handler(void);
extern void nss_dtls_cmn_register_handler(void);
extern void nss_tls_register_handler(void);
extern void nss_gre_tunnel_register_handler(void);
extern void nss_trustsec_tx_register_handler(void);
extern void nss_wifili_register_handler(void);
extern void nss_ppe_register_handler(void);
extern void nss_gre_redir_mark_register_handler(void);
extern void nss_ppe_vp_register_handler(void);
extern void nss_wifi_mac_db_register_handler(void);
extern void nss_wifi_ext_vdev_register_handler(void);
extern void nss_wifili_thread_scheme_db_init(uint8_t core_id);
extern void nss_wifi_mesh_init(void);

/*
 * nss_if_msg_handler()
 *	External reference for internal base class handler for interface messages.
 *
 * This is not registered with nss_core.c as it is really a base class feature
 * of the phys_if and virt_if handlers.
 */
extern void nss_if_msg_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm,
		__attribute__((unused))void *app_data);

#endif /* __NSS_TX_RX_COMMON_H */
