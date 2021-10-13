/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#ifndef _REF_VLAN_H_
#define _REF_VLAN_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#if defined(IN_SWCONFIG)
#include <linux/version.h>
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/switch.h>
#else
#include <net/switch.h>
#endif

int
qca_ar8327_sw_set_vlan(struct switch_dev *dev,
                       const struct switch_attr *attr,
                       struct switch_val *val);

int
qca_ar8327_sw_get_vlan(struct switch_dev *dev,
                       const struct switch_attr *attr,
                       struct switch_val *val);

int
qca_ar8327_sw_set_vid(struct switch_dev *dev,
                      const struct switch_attr *attr,
                      struct switch_val *val);

int
qca_ar8327_sw_get_vid(struct switch_dev *dev,
                      const struct switch_attr *attr,
                      struct switch_val *val);

int
qca_ar8327_sw_get_pvid(struct switch_dev *dev, int port, int *vlan);

int
qca_ar8327_sw_set_pvid(struct switch_dev *dev, int port, int vlan);

int
qca_ar8327_sw_get_ports(struct switch_dev *dev, struct switch_val *val);

int
qca_ar8327_sw_set_ports(struct switch_dev *dev, struct switch_val *val);

int
qca_ar8327_sw_hw_apply(struct switch_dev *dev);
#endif

typedef struct {
	fal_port_t port_id;        /* port id */
	a_uint32_t vid;            /* vlan id */
	a_bool_t   is_wan_port;    /* belong to wan port */
	a_bool_t   valid;          /* valid or not */
} qca_lan_wan_port_info;

typedef struct {
	a_bool_t lan_only_mode;
	qca_lan_wan_port_info v_port_info[SW_MAX_NR_PORT];
} qca_lan_wan_cfg_t;

sw_error_t
qca_lan_wan_cfg_set(a_uint32_t dev_id, qca_lan_wan_cfg_t *lan_wan_cfg);

sw_error_t
qca_lan_wan_cfg_get(a_uint32_t dev_id, qca_lan_wan_cfg_t *lan_wan_cfg);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _REF_VLAN_H_ */

