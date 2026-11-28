/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef __BCMPLC_COMMON_H
#define __BCMPLC_COMMON_H

#include <stdint.h>

#include <net/ethernet.h>
#include <linux/filter.h>

#define ETH_P_BCMPLC		0x8912

#define FRAME_MAX		1514
#define FRAME_MIN		60

int64_t now_ms(void);
int if_lookup(const char *ifname, int *ifindex, uint8_t *mac);
int sock_open(int ifindex, const struct sock_fprog *prog);
int mac_parse(const char *str, uint8_t *mac);
void mac_add(uint8_t *mac, unsigned int val);

#endif
