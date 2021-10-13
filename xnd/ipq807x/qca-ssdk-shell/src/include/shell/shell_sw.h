/*
 * Copyright (c) 2014, 2017-2018, The Linux Foundation. All rights reserved.
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
 */
/*qca808x_start*/
#ifndef _SHELL_SW_H_
#define _SHELL_SW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sw.h"

    int get_devid(void);
    int set_devid(int dev_id);
    sw_error_t cmd_set_devid(a_ulong_t *arg_val);
/*qca808x_end*/
    sw_error_t cmd_show_fdb(a_ulong_t *arg_val);
    sw_error_t cmd_show_vlan(a_ulong_t *arg_val);
    sw_error_t cmd_show_resv_fdb(a_ulong_t *arg_val);
    sw_error_t cmd_show_host(a_ulong_t *arg_val);
    sw_error_t cmd_show_host_ipv4(a_ulong_t *arg_val);
    sw_error_t cmd_show_host_ipv6(a_ulong_t *arg_val);
    sw_error_t cmd_show_host_ipv4M(a_ulong_t *arg_val);
    sw_error_t cmd_show_host_ipv6M(a_ulong_t *arg_val);
    sw_error_t cmd_show_flow_ipv4_3tuple(a_ulong_t *arg_val);
    sw_error_t cmd_show_flow_ipv4_5tuple(a_ulong_t *arg_val);
    sw_error_t cmd_show_flow_ipv6_3tuple(a_ulong_t *arg_val);
    sw_error_t cmd_show_flow_ipv6_5tuple(a_ulong_t *arg_val);
    sw_error_t cmd_show_nat(a_ulong_t *arg_val);
    sw_error_t cmd_show_napt(a_ulong_t *arg_val);
    sw_error_t cmd_show_intfmac(a_ulong_t *arg_val);
    sw_error_t cmd_show_pubaddr(a_ulong_t *arg_val);
    sw_error_t cmd_show_flow(a_ulong_t *arg_val);
    sw_error_t cmd_show_ctrlpkt(a_ulong_t *arg_val);
    sw_error_t cmd_show_ptvlan_entry(a_ulong_t *arg_val);
/*qca808x_start*/
#ifdef __cplusplus
}
#endif                          /* __cplusplus */
/*qca808x_start*/

#endif                          /* _SHELL_SW_H_ */
