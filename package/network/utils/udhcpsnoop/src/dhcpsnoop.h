// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */
#ifndef __DHCPSNOOP_H
#define __DHCPSNOOP_H

#include <libubox/blobmsg.h>
#include <libubox/ulog.h>
#include <libubox/uloop.h>

#define DHCPSNOOP_IFB_NAME "ifb-dhcp"
#define DHCPSNOOP_PRIO_BASE	0x100

int dhcpsnoop_run_cmd(char *cmd, bool ignore_error);

int dhcpsnoop_dev_init(void);
void dhcpsnoop_dev_done(void);
void dhcpsnoop_dev_config_update(struct blob_attr *data, bool add_only);
void dhcpsnoop_dev_check(void);

void dhcpsnoop_ubus_init(void);
void dhcpsnoop_ubus_done(void);
void dhcpsnoop_ubus_notify(const char *type, const uint8_t *msg, size_t len);

const char *dhcpsnoop_parse_ipv4(const void *buf, size_t len, uint16_t port, uint32_t *rebind);
const char *dhcpsnoop_parse_ipv6(const void *buf, size_t len, uint16_t port);

void cache_entry(void *msg, uint32_t rebind);
void cache_dump(struct blob_buf *b);

#endif
