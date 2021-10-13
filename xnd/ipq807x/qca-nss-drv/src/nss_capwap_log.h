/*
 ******************************************************************************
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
 * ****************************************************************************
 */

#ifndef __NSS_CAPWAP_LOG_H__
#define __NSS_CAPWAP_LOG_H__

/*
 * nss_capwap_log.h
 *	NSS CAPWAP Log Header File.
 */

/*
 * nss_capwap_log_tx_msg
 *	Logs a CAPWAP message that is sent to the NSS firmware.
 */
void nss_capwap_log_tx_msg(struct nss_capwap_msg *ncm);

/*
 * nss_capwap_log_rx_msg
 *	Logs a CAPWAP message that is received from the NSS firmware.
 */
void nss_capwap_log_rx_msg(struct nss_capwap_msg *ncm);

#endif /* __NSS_CAPWAP_LOG_H__ */
