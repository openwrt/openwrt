/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#ifndef _SSDK_UCI_H_
#define _SSDK_UCI_H_

#ifdef BOARD_AR71XX
int ssdk_uci_takeover_init(void);
void ssdk_uci_takeover_exit(void);
int ssdk_uci_sw_set_vlan(const struct switch_attr *attr,
                       struct switch_val *val);
int ssdk_uci_sw_set_vid(const struct switch_attr *attr,
                       struct switch_val *val);
int ssdk_uci_sw_set_pvid(int port, int vlan);
int ssdk_uci_sw_set_ports(struct switch_val *val);


#endif

#endif
