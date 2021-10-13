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

#ifndef _REF_PORT_CTRL_H_
#define _REF_PORT_CTRL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include <linux/version.h>
#if defined(IN_SWCONFIG)
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/switch.h>
#else
#include <net/switch.h>
#endif
#endif

#include <linux/notifier.h>

/**
 * @brief QCA SSDK port link context
 */

typedef struct{
    unsigned char port_id;/*port 1-5*/
    unsigned char port_link; /*0:linkdown, 1:linkup*/
    unsigned char speed; /*0:10M, 1:100M, 2:1000M*/
    unsigned char duplex;/*0:half, 1:full*/
}ssdk_port_status;

#if defined(IN_SWCONFIG)
int
qca_ar8327_sw_get_port_link(struct switch_dev *dev, int port,
	struct switch_port_link *link);
int qca_ar8327_sw_set_eee(struct switch_dev *dev,
	const struct switch_attr *attr, struct switch_val *val);
int qca_ar8327_sw_get_eee(struct switch_dev *dev,
	const struct switch_attr *attr, struct switch_val *val);
#endif

int ssdk_port_link_notify_register(struct notifier_block *nb);
int ssdk_port_link_notify_unregister(struct notifier_block *nb);
int ssdk_port_link_notify(unsigned char port_id,
	unsigned char link, unsigned char speed, unsigned char duplex);
#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _REF_PORT_CTRL_H_ */

