/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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

#ifndef _REF_API_H_
#define _REF_API_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#if defined(IN_VLAN) && !defined(IN_VLAN_MINI)
#define REF_VLAN_API \
    SW_API_DEF(SW_API_LAN_WAN_CFG_SET, qca_lan_wan_cfg_set), \
    SW_API_DEF(SW_API_LAN_WAN_CFG_GET, qca_lan_wan_cfg_get),

#define REF_VLAN_API_PARAM \
    SW_API_DESC(SW_API_LAN_WAN_CFG_SET) \
    SW_API_DESC(SW_API_LAN_WAN_CFG_GET)

#else
#define REF_VLAN_API
#define REF_VLAN_API_PARAM
#endif

#define SSDK_REF_API \
    REF_VLAN_API

#define SSDK_REF_PARAM  \
    REF_VLAN_API_PARAM

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _REF_API_H_ */
