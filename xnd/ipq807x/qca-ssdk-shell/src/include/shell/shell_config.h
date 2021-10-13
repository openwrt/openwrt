/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
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

#ifndef _SHELL_CONFIG_H_
#define _SHELL_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sw.h"
#include "sw_ioctl.h"
#include "sw_api.h"

#define SW_CMD_SET_DEVID              (SW_API_MAX + 1)
#define SW_CMD_VLAN_SHOW              (SW_API_MAX + 2)
#define SW_CMD_FDB_SHOW               (SW_API_MAX + 3)
#define SW_CMD_RESV_FDB_SHOW          (SW_API_MAX + 4)
#define SW_CMD_HOST_SHOW              (SW_API_MAX + 5)
#define SW_CMD_NAT_SHOW               (SW_API_MAX + 6)
#define SW_CMD_NAPT_SHOW              (SW_API_MAX + 7)
#define SW_CMD_INTFMAC_SHOW           (SW_API_MAX + 8)
#define SW_CMD_PUBADDR_SHOW           (SW_API_MAX + 9)
#define SW_CMD_FLOW_SHOW              (SW_API_MAX + 10)
#define SW_CMD_HOST_IPV4_SHOW         (SW_API_MAX + 11)
#define SW_CMD_HOST_IPV6_SHOW         (SW_API_MAX + 12)
#define SW_CMD_HOST_IPV4M_SHOW        (SW_API_MAX + 13)
#define SW_CMD_HOST_IPV6M_SHOW        (SW_API_MAX + 14)
#define SW_CMD_CTRLPKT_SHOW           (SW_API_MAX + 15)
#define SW_CMD_FLOW_IPV43T_SHOW       (SW_API_MAX + 16)
#define SW_CMD_FLOW_IPV63T_SHOW       (SW_API_MAX + 17)
#define SW_CMD_FLOW_IPV45T_SHOW       (SW_API_MAX + 18)
#define SW_CMD_FLOW_IPV65T_SHOW       (SW_API_MAX + 19)
#define SW_CMD_PT_VLAN_TRANS_ADV_SHOW (SW_API_MAX + 20)
#define SW_CMD_MAX                    (SW_API_MAX + 21)

#define MAX_SUB_CMD_DES_NUM  120

#define SW_API_INVALID 0

    struct sub_cmd_des_t
    {
        char *sub_name;
        char *sub_act;
        char *sub_memo;
        char *sub_usage;
        int   sub_api;
        sw_error_t (*sub_func) ();
    };
    struct cmd_des_t
    {
        char *name;
        char *memo;
        struct sub_cmd_des_t sub_cmd_des[MAX_SUB_CMD_DES_NUM];
    };
    extern struct cmd_des_t gcmd_des[];

#define GCMD_DES gcmd_des

#define GCMD_NAME(cmd_nr)  GCMD_DES[cmd_nr].name
#define GCMD_MEMO(cmd_nr)  GCMD_DES[cmd_nr].memo

#define GCMD_SUB_NAME(cmd_nr, sub_cmd_nr)  GCMD_DES[cmd_nr].sub_cmd_des[sub_cmd_nr].sub_name
#define GCMD_SUB_ACT(cmd_nr, sub_cmd_nr)  GCMD_DES[cmd_nr].sub_cmd_des[sub_cmd_nr].sub_act
#define GCMD_SUB_MEMO(cmd_nr, sub_cmd_nr)  GCMD_DES[cmd_nr].sub_cmd_des[sub_cmd_nr].sub_memo
#define GCMD_SUB_USAGE(cmd_nr, sub_cmd_nr)  GCMD_DES[cmd_nr].sub_cmd_des[sub_cmd_nr].sub_usage
#define GCMD_SUB_API(cmd_nr, sub_cmd_nr)  GCMD_DES[cmd_nr].sub_cmd_des[sub_cmd_nr].sub_api
#define GCMD_SUB_FUNC(cmd_nr, sub_cmd_nr)  GCMD_DES[cmd_nr].sub_cmd_des[sub_cmd_nr].sub_func

#define GCMD_DESC_VALID(cmd_nr)                    GCMD_NAME(cmd_nr)
#define GCMD_SUB_DESC_VALID(cmd_nr, sub_cmd_nr)    GCMD_SUB_API(cmd_nr, sub_cmd_nr)


#define GCMD_DESC_NO_MATCH      0xffffffff

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _SHELL_CONFIG_H_ */
