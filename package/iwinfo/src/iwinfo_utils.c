/*
 * iwinfo - Wireless Information Library - Shared utility routines
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
 *
 * The signal handling code is derived from the official madwifi tools,
 * wlanconfig.c in particular. The encryption property handling was
 * inspired by the hostapd madwifi driver.
 */

#include "iwinfo/utils.h"


static int ioctl_socket = -1;

static int iwinfo_ioctl_socket(void)
{
	/* Prepare socket */
	if( ioctl_socket == -1 )
	{
		ioctl_socket = socket(AF_INET, SOCK_DGRAM, 0);
		fcntl(ioctl_socket, F_SETFD, fcntl(ioctl_socket, F_GETFD) | FD_CLOEXEC);
	}

	return ioctl_socket;
}

int iwinfo_ioctl(int cmd, void *ifr)
{
	int s = iwinfo_ioctl_socket();
	return ioctl(s, cmd, ifr);
}

int iwinfo_dbm2mw(int in)
{
	double res = 1.0;
	int ip = in / 10;
	int fp = in % 10;
	int k;

	for(k = 0; k < ip; k++) res *= 10;
	for(k = 0; k < fp; k++) res *= LOG10_MAGIC;

	return (int)res;
}

int iwinfo_mw2dbm(int in)
{
	double fin = (double) in;
	int res = 0;

	while(fin > 10.0)
	{
		res += 10;
		fin /= 10.0;
	}

	while(fin > 1.000001)
	{
		res += 1;
		fin /= LOG10_MAGIC;
	}

	return (int)res;
}

int iwinfo_ifup(const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if( iwinfo_ioctl(SIOCGIFFLAGS, &ifr) )
		return 0;

	ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);

	return !iwinfo_ioctl(SIOCSIFFLAGS, &ifr);
}

int iwinfo_ifdown(const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if( iwinfo_ioctl(SIOCGIFFLAGS, &ifr) )
		return 0;

	ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);

	return !iwinfo_ioctl(SIOCSIFFLAGS, &ifr);
}

int iwinfo_ifmac(const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if( iwinfo_ioctl(SIOCGIFHWADDR, &ifr) )
		return 0;

	ifr.ifr_hwaddr.sa_data[1]++;
	ifr.ifr_hwaddr.sa_data[2]++;

	return !iwinfo_ioctl(SIOCSIFHWADDR, &ifr);
}

void iwinfo_close(void)
{
	if( ioctl_socket > -1 )
		close(ioctl_socket);
}

struct iwinfo_hardware_entry * iwinfo_hardware(struct iwinfo_hardware_id *id)
{
	const struct iwinfo_hardware_entry *e;

	for (e = IWINFO_HARDWARE_ENTRIES; e->vendor_name; e++)
	{
		if ((e->vendor_id != 0xffff) && (e->vendor_id != id->vendor_id))
			continue;

		if ((e->device_id != 0xffff) && (e->device_id != id->device_id))
			continue;

		if ((e->subsystem_vendor_id != 0xffff) &&
			(e->subsystem_vendor_id != id->subsystem_vendor_id))
			continue;

		if ((e->subsystem_device_id != 0xffff) &&
			(e->subsystem_device_id != id->subsystem_device_id))
			continue;

		return e;
	}

	return NULL;
}
