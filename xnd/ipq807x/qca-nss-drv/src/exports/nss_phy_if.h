/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

/**
 * @file nss_phy_if.h.h
 *	NSS physical interface definitions.
 */

#ifndef __NSS_PHY_IF_H
#define __NSS_PHY_IF_H

/**
 * @addtogroup nss_driver_subsystem
 * @{
 */

/**
 * nss_phys_if_reset_nexthop
 *	De-configure nexthop for an interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   Network physical interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_phys_if_reset_nexthop(struct nss_ctx_instance *nss_ctx, uint32_t if_num);

/**
 * nss_phys_if_set_nexthop
 *	Configure nexthop for an interface.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   Network physical interface number.
 * @param[in] nexthop   Network physical or virtual interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_phys_if_set_nexthop(struct nss_ctx_instance *nss_ctx, uint32_t if_num, uint32_t nexthop);

/**
 * @}
 */

#endif /*  __NSS_PHY_IF_H */
