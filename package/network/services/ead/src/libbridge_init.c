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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libbridge.h"
#include "libbridge_private.h"

int br_socket_fd = -1;

static int br_init(void)
{
	if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
		return errno;
	return 0;
}

static void br_shutdown(void)
{
	close(br_socket_fd);
	br_socket_fd = -1;
}

/* If /sys/class/net/XXX/bridge exists then it must be a bridge */
static int isbridge(const struct dirent *entry)
{
	char path[SYSFS_PATH_MAX];
	struct stat st;

	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/bridge", entry->d_name);
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

/*
 * New interface uses sysfs to find bridges
 */
static int new_foreach_bridge(int (*iterator)(const char *name, void *),
			      void *arg)
{
	struct dirent **namelist;
	int i, count = 0;

	count = scandir(SYSFS_CLASS_NET, &namelist, isbridge, alphasort);
	if (count < 0)
		return -1;

	for (i = 0; i < count; i++) {
		if (iterator(namelist[i]->d_name, arg))
			break;
	}

	for (i = 0; i < count; i++)
		free(namelist[i]);
	free(namelist);

	return count;
}

/*
 * Old interface uses ioctl
 */
static int old_foreach_bridge(int (*iterator)(const char *, void *), 
			      void *iarg)
{
	int i, ret=0, num;
	char ifname[IFNAMSIZ];
	int ifindices[MAX_BRIDGES];
	unsigned long args[3] = { BRCTL_GET_BRIDGES, 
				 (unsigned long)ifindices, MAX_BRIDGES };

	num = ioctl(br_socket_fd, SIOCGIFBR, args);
	if (num < 0) {
		dprintf("Get bridge indices failed: %s\n",
			strerror(errno));
		return -errno;
	}

	for (i = 0; i < num; i++) {
		if (!if_indextoname(ifindices[i], ifname)) {
			dprintf("get find name for ifindex %d\n",
				ifindices[i]);
			return -errno;
		}

		++ret;
		if(iterator(ifname, iarg)) 
			break;
	}

	return ret;

}

/*
 * Go over all bridges and call iterator function.
 * if iterator returns non-zero then stop.
 */
static int br_foreach_bridge(int (*iterator)(const char *, void *), 
		     void *arg)
{
	int ret;

	ret = new_foreach_bridge(iterator, arg);
	if (ret <= 0)
		ret = old_foreach_bridge(iterator, arg);

	return ret;
}

/* 
 * Only used if sysfs is not available.
 */
static int old_foreach_port(const char *brname,
			    int (*iterator)(const char *br, const char *port, 
					    void *arg),
			    void *arg)
{
	int i, err, count;
	struct ifreq ifr;
	char ifname[IFNAMSIZ];
	int ifindices[MAX_PORTS];
	unsigned long args[4] = { BRCTL_GET_PORT_LIST,
				  (unsigned long)ifindices, MAX_PORTS, 0 };

	memset(ifindices, 0, sizeof(ifindices));
	strncpy(ifr.ifr_name, brname, IFNAMSIZ);
	ifr.ifr_data = (char *) &args;

	err = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
	if (err < 0) {
		dprintf("list ports for bridge:'%s' failed: %s\n",
			brname, strerror(errno));
		return -errno;
	}

	count = 0;
	for (i = 0; i < MAX_PORTS; i++) {
		if (!ifindices[i])
			continue;

		if (!if_indextoname(ifindices[i], ifname)) {
			dprintf("can't find name for ifindex:%d\n",
				ifindices[i]);
			continue;
		}

		++count;
		if (iterator(brname, ifname, arg))
			break;
	}

	return count;
}

/*
 * Iterate over all ports in bridge (using sysfs).
 */
static int br_foreach_port(const char *brname,
		    int (*iterator)(const char *br, const char *port, void *arg),
		    void *arg)
{
	int i, count;
	struct dirent **namelist;
	char path[SYSFS_PATH_MAX];

	snprintf(path, SYSFS_PATH_MAX, SYSFS_CLASS_NET "%s/brif", brname);
	count = scandir(path, &namelist, 0, alphasort);
	if (count < 0)
		return old_foreach_port(brname, iterator, arg);

	for (i = 0; i < count; i++) {
		if (namelist[i]->d_name[0] == '.'
		    && (namelist[i]->d_name[1] == '\0'
			|| (namelist[i]->d_name[1] == '.'
			    && namelist[i]->d_name[2] == '\0')))
			continue;

		if (iterator(brname, namelist[i]->d_name, arg))
			break;
	}
	for (i = 0; i < count; i++)
		free(namelist[i]);
	free(namelist);

	return count;
}
