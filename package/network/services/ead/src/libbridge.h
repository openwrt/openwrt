/*
 * Copyright (C) 2000 Lennert Buytenhek
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _LIBBRIDGE_H
#define _LIBBRIDGE_H

#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_bridge.h>

/* defined in net/if.h but that conflicts with linux/if.h... */
extern unsigned int if_nametoindex (const char *__ifname);
extern char *if_indextoname (unsigned int __ifindex, char *__ifname);

struct bridge_id
{
	unsigned char prio[2];
	unsigned char addr[6];
};

struct bridge_info
{
	struct bridge_id designated_root;
	struct bridge_id bridge_id;
	unsigned root_path_cost;
	struct timeval max_age;
	struct timeval hello_time;
	struct timeval forward_delay;
	struct timeval bridge_max_age;
	struct timeval bridge_hello_time;
	struct timeval bridge_forward_delay;
	u_int16_t root_port;
	unsigned char stp_enabled;
	unsigned char topology_change;
	unsigned char topology_change_detected;
	struct timeval ageing_time;
	struct timeval hello_timer_value;
	struct timeval tcn_timer_value;
	struct timeval topology_change_timer_value;
	struct timeval gc_timer_value;
};

struct fdb_entry
{
	u_int8_t mac_addr[6];
	u_int16_t port_no;
	unsigned char is_local;
	struct timeval ageing_timer_value;
};

struct port_info
{
	unsigned port_no;
	struct bridge_id designated_root;
	struct bridge_id designated_bridge;
	u_int16_t port_id;
	u_int16_t designated_port;
	u_int8_t priority;
	unsigned char top_change_ack;
	unsigned char config_pending;
	unsigned char state;
	unsigned path_cost;
	unsigned designated_cost;
	struct timeval message_age_timer_value;
	struct timeval forward_delay_timer_value;
	struct timeval hold_timer_value;
};

#endif
