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

#ifndef __NSS_DTLS_LOG_H
#define __NSS_DTLS_LOG_H

/*
 * nss_dtls_log.h
 *	NSS DTLS Log Header File
 */

/*
 * nss_dtls_log_tx_msg
 *	Logs a DTLS message that is sent to the NSS firmware.
 */
void nss_dtls_log_tx_msg(struct nss_dtls_msg *ndm);

/*
 * nss_dtls_log_rx_msg
 *	Logs a DTLS message that is received from the NSS firmware.
 */
void nss_dtls_log_rx_msg(struct nss_dtls_msg *ndm);

#endif /* __NSS_DTLS_LOG_H */
