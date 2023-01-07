/*
 * Copyright (C) 2018 Marco d'Itri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __NETWORK_H__
#define __NETWORK_H__

#define SET_MAX(fd) do { if (max < (fd) + 1) { max = (fd) + 1; } } while (0)

char *print_addr_port(const struct sockaddr *addr, socklen_t addrlen);

int udp_listener(const char *s);

int *tcp_listener(const char *s);

int udp_listener_sa(const int num);

int *tcp_listener_sa(const int num);

int udp_client(const char *s, struct sockaddr_storage *remote_udpaddr);

int tcp_client(const char *s);

int accept_connections(int listening_sockets[]);

#endif
