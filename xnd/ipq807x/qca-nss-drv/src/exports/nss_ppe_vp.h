/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
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

/**
 * @file nss_ppe_vp.h
 *	NSS PPE Virtual Port definitions.
 */

#ifndef _NSS_PPE_VP_H_
#define _NSS_PPE_VP_H_

/**
 * nss_if_ppe_vp_destroy
 *	Destroy the PPE VP for a given NSS interface number.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_vp_destroy(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num);

/**
 * nss_ppe_vp_create
 *	Create the PPE VP for a given NSS interface number.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_vp_create(struct nss_ctx_instance *nss_ctx, nss_if_num_t if_num);

/**
 * nss_ppe_vp_get_ppe_port_by_nssif
 *	Returns the PPE VP number for a given NSS interface number.
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] nss_if  NSS interface number.
 *
 * @return
 * Returns the PPE VP number corresponding to the NSS interface number.
 */
nss_ppe_port_t nss_ppe_vp_get_ppe_port_by_nssif(struct nss_ctx_instance *nss_ctx, nss_if_num_t nss_if);

/**
 * nss_ppe_vp_get_context
 *	Return the NSS context of PPE VP.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_ppe_vp_get_context(void);

#endif /* _NSS_PPE_VP_H_ */
