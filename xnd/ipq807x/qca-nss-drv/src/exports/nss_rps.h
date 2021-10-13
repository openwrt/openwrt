/*
 **************************************************************************
 * Copyright (c) 2013-2018, The Linux Foundation. All rights reserved.
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
 * @file nss_rps.h
 *	RPS related definitions.
 */

#ifndef __NSS_RPS_H
#define __NSS_RPS_H

/**
 * @addtogroup nss_rps_subsystem
 * @{
 */

/**
 * nss_rps_register_sysctl
 *	Registers the RPS sysctl entries to the sysctl tree.
 *
 * @return
 * None.
 */
extern void nss_rps_register_sysctl(void);

/**
 * nss_rps_unregister_sysctl
 *	Deregisters the RPS sysctl entries from the sysctl tree.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control must have been previously registered.
 */
extern void nss_rps_unregister_sysctl(void);

/**
 * @}
 */

#endif /* __NSS_RPS_H */
