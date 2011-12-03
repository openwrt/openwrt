/*
 * iwinfo - Wireless Information Library - Utility Headers
 *
 *   Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
 *
 * The iwinfo library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * The iwinfo library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the iwinfo library. If not, see http://www.gnu.org/licenses/.
 */

#ifndef __IWINFO_UTILS_H_
#define __IWINFO_UTILS_H_

#include <sys/socket.h>
#include <net/if.h>

#include "iwinfo.h"

#define LOG10_MAGIC	1.25892541179

int iwinfo_ioctl(int cmd, void *ifr);

int iwinfo_dbm2mw(int in);
int iwinfo_mw2dbm(int in);

int iwinfo_ifup(const char *ifname);
int iwinfo_ifdown(const char *ifname);
int iwinfo_ifmac(const char *ifname);

void iwinfo_close(void);

#endif
