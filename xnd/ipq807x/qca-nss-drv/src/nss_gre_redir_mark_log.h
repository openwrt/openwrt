/*
 ******************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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

#ifndef __NSS_GRE_REDIR_MARK_LOG_H__
#define __NSS_GRE_REDIR_MARK_LOG_H__

/*
 * nss_gre_redir_mark_log.h
 *	NSS GRE_REDIR_MARK Log Header File.
 */

/*
 * nss_gre_redir_mark_log_tx_msg
 *	Logs GRE_REDIR_MARK message that is sent to the NSS firmware.
 */
void nss_gre_redir_mark_log_tx_msg(struct nss_gre_redir_mark_msg *ncm);

/*
 * nss_gre_redir_mark_log_rx_msg
 *	Logs GRE_REDIR_MARK message that is received from the NSS firmware.
 */
void nss_gre_redir_mark_log_rx_msg(struct nss_gre_redir_mark_msg *ncm);

#endif /* __NSS_GRE_REDIR_MARK_LOG_H__ */
