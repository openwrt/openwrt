/*
 * Copyright (c) 2013-2019, The Linux Foundation. All rights reserved.
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

/*
 * @file nss_freq.h
 *	NSS frequency definitions.
 */

#ifndef __NSS_FREQ_H
#define __NSS_FREQ_H

/**
 * @addtogroup nss_freq_subsystem
 * @{
 */

/**
 * nss_freq_change
 *	Changes the frequency of the NSS cores.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx       Pointer to the NSS context.
 * @param[in] eng           Frequency value in Hz.
 * @param[in] stats_enable  Enable NSS to send scaling statistics.
 * @param[in] start_or_end  Start or end of the frequency change.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_freq_change(struct nss_ctx_instance *nss_ctx, uint32_t eng, uint32_t stats_enable, uint32_t start_or_end);

/**
 * nss_freq_get_cpu_usage
 *	Returns the CPU usage value in percentage at any instance for a required core. Range of usage is 0-100.
 *
 * @param[in] core_id       NSS Core ID.
 *
 * @return
 * CPU usage value in percentage averaged over 1 second. -1 in case of error.
 * @note
 * This API does not support gathering CPU usage data for core 1.
 */
extern int8_t nss_freq_get_cpu_usage(uint32_t core_id);

/**
 * @}
 */

#endif /* __NSS_FREQ_H */
