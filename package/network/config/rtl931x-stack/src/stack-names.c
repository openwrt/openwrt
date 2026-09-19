// SPDX-License-Identifier: GPL-2.0-only

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>

#include "stack-names.h"

#define MAX_PORTS 64
#define MAX_FABRIC_PORTS 4

struct port_name {
	char old[IFNAMSIZ];
	char new[IFNAMSIZ];
	unsigned int ifindex;
};

static int read_text(const char *path, char *value, size_t size)
{
	FILE *file = fopen(path, "r");
	size_t len;
	int err = 0;

	if (!file)
		return -errno;
	len = fread(value, 1, size - 1, file);
	if (ferror(file) || !len)
		err = -EIO;
	else if (fgetc(file) != EOF)
		err = -E2BIG;
	else {
		value[len] = '\0';
		value[strcspn(value, "\n")] = '\0';
	}
	fclose(file);
	return err;
}

static int panel_number(const char *label, unsigned int *panel)
{
	const char *p;
	unsigned long value;

	if (strncmp(label, "lan", 3) || !label[3])
		return -EINVAL;
	for (p = label + 3; *p; p++)
		if (*p < '0' || *p > '9')
			return -EINVAL;
	errno = 0;
	value = strtoul(label + 3, NULL, 10);
	if (errno || !value || value > UINT16_MAX)
		return -EINVAL;
	*panel = value;
	return 0;
}

static int board_port_numbers(const char *device, unsigned int panels[])
{
	char directory[PATH_MAX], path[PATH_MAX], label[IFNAMSIZ];
	struct dirent *entry;
	DIR *ports;
	int err = 0;

	if (snprintf(directory, sizeof(directory), "%s/of_node/ports", device) >=
	    sizeof(directory))
		return -ENAMETOOLONG;
	ports = opendir(directory);
	if (!ports) {
		if (snprintf(directory, sizeof(directory), "%s/of_node/ethernet-ports",
			     device) >= sizeof(directory))
			return -ENAMETOOLONG;
		ports = opendir(directory);
	}
	if (!ports)
		return -errno;
	while ((entry = readdir(ports))) {
		unsigned char reg[4];
		unsigned int port, panel, i;
		FILE *file;

		if (entry->d_name[0] == '.')
			continue;
		if (snprintf(path, sizeof(path), "%s/%s/label", directory,
			     entry->d_name) >= sizeof(path)) {
			err = -ENAMETOOLONG;
			break;
		}
		if (read_text(path, label, sizeof(label)))
			continue;
		if (panel_number(label, &panel))
			continue;
		snprintf(path, sizeof(path), "%s/%s/reg", directory, entry->d_name);
		file = fopen(path, "rb");
		if (!file) {
			err = -errno;
			break;
		}
		err = fread(reg, 1, sizeof(reg), file) == sizeof(reg) ? 0 : -EIO;
		fclose(file);
		if (err)
			break;
		port = (uint32_t)reg[0] << 24 | (uint32_t)reg[1] << 16 |
		       (uint32_t)reg[2] << 8 | reg[3];
		if (port >= MAX_PORTS || panels[port]) {
			err = -EINVAL;
			break;
		}
		for (i = 0; i < MAX_PORTS; i++)
			if (panels[i] == panel)
				err = -EEXIST;
		if (err)
			break;
		panels[port] = panel;
	}
	closedir(ports);
	return err;
}

/* Resolve either the board label or the canonical name, including at boot. */
static unsigned int fabric_ifindex(const char *name, uint8_t member)
{
	char alternate[IFNAMSIZ], canonical[IFNAMSIZ], tail;
	unsigned int id, slot, panel, ifindex;

	if (!panel_number(name, &panel)) {
		snprintf(alternate, sizeof(alternate), "sw%us0p%u", member, panel);
	} else if (sscanf(name, "sw%us%up%u%c", &id, &slot, &panel, &tail) == 3) {
		if (id != member || slot || !panel || panel > UINT16_MAX)
			return 0;
		snprintf(canonical, sizeof(canonical), "sw%us%up%u", id, slot, panel);
		if (strcmp(canonical, name))
			return 0;
		snprintf(alternate, sizeof(alternate), "lan%u", panel);
	} else {
		return if_nametoindex(name);
	}
	ifindex = if_nametoindex(name);
	return ifindex ? ifindex : if_nametoindex(alternate);
}

static int port_can_rename(int fd, const struct port_name *port)
{
	struct ifreq request = {};
	char path[PATH_MAX];
	struct dirent *entry;
	DIR *directory;
	int err = 0;

	if (!strcmp(port->old, port->new))
		return 0;
	if (if_nametoindex(port->new))
		return -EEXIST;
	strcpy(request.ifr_name, port->old);
	if (ioctl(fd, SIOCGIFFLAGS, &request))
		return -errno;
	if (request.ifr_flags & IFF_UP)
		return -EBUSY;
	snprintf(path, sizeof(path), "/sys/class/net/%s", port->old);
	directory = opendir(path);
	if (!directory)
		return -errno;
	while ((entry = readdir(directory)))
		if (!strncmp(entry->d_name, "upper_", 6))
			err = -EBUSY;
	closedir(directory);
	return err;
}

static int rename_port(int fd, const struct port_name *port, bool undo)
{
	struct ifreq request = {};
	const char *from = undo ? port->new : port->old;
	const char *to = undo ? port->old : port->new;

	if (!strcmp(from, to))
		return 0;
	if (if_nametoindex(from) != port->ifindex)
		return -ESTALE;
	strcpy(request.ifr_name, from);
	strcpy(request.ifr_newname, to);
	return ioctl(fd, SIOCSIFNAME, &request) ? -errno : 0;
}

int rtl931x_stack_name_ports(char interfaces[][IFNAMSIZ], size_t count,
			     uint8_t member)
{
	struct port_name names[MAX_PORTS] = {};
	unsigned int panels[MAX_PORTS] = {}, fabric[MAX_FABRIC_PORTS];
	char path[PATH_MAX], device[PATH_MAX], candidate[PATH_MAX];
	char physical[IFNAMSIZ], anchor[IFNAMSIZ], tail;
	struct dirent *entry;
	size_t n = 0, i, j;
	DIR *directory;
	int fd, err;

	if (!count || count > MAX_FABRIC_PORTS || member > 15)
		return -EINVAL;
	for (i = 0; i < count; i++) {
		fabric[i] = fabric_ifindex(interfaces[i], member);
		if (!fabric[i])
			return -ENODEV;
		for (j = 0; j < i; j++)
			if (fabric[i] == fabric[j])
				return -EINVAL;
	}
	if (!if_indextoname(fabric[0], anchor))
		return -errno;
	snprintf(path, sizeof(path), "/sys/class/net/%s/device", anchor);
	if (!realpath(path, device))
		return -errno;
	err = board_port_numbers(device, panels);
	if (err)
		return err;
	directory = opendir("/sys/class/net");
	if (!directory)
		return -errno;
	while ((entry = readdir(directory))) {
		unsigned int port;

		if (entry->d_name[0] == '.' || strlen(entry->d_name) >= IFNAMSIZ)
			continue;
		snprintf(path, sizeof(path), "/sys/class/net/%s/device", entry->d_name);
		if (!realpath(path, candidate) || strcmp(candidate, device))
			continue;
		snprintf(path, sizeof(path), "/sys/class/net/%s/phys_port_name",
			 entry->d_name);
		if (read_text(path, physical, sizeof(physical)) ||
		    sscanf(physical, "p%u%c", &port, &tail) != 1)
			continue;
		if (port >= MAX_PORTS || !panels[port] || n == MAX_PORTS) {
			err = -EINVAL;
			break;
		}
		strcpy(names[n].old, entry->d_name);
		snprintf(names[n].new, sizeof(names[n].new), "sw%us0p%u",
			 member, panels[port]);
		names[n].ifindex = if_nametoindex(entry->d_name);
		if (!names[n].ifindex) {
			err = -ENODEV;
			break;
		}
		for (j = 0; j < n; j++)
			if (!strcmp(names[j].new, names[n].new))
				err = -EEXIST;
		if (err)
			break;
		n++;
	}
	closedir(directory);
	if (err)
		return err;
	/* Every fabric reference must belong to this switch's physical ports. */
	for (i = 0; i < count; i++) {
		for (j = 0; j < n && names[j].ifindex != fabric[i]; j++)
			;
		if (j == n)
			return -ENODEV;
	}
	fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return -errno;
	for (i = 0; i < n; i++) {
		err = port_can_rename(fd, &names[i]);
		if (err) {
			syslog(LOG_ERR, "cannot name %s as %s: %s; migrate and reboot",
			       names[i].old, names[i].new, strerror(-err));
			goto out;
		}
	}
	for (i = 0; i < n; i++) {
		err = rename_port(fd, &names[i], false);
		if (err) {
			while (i) {
				i--;
				if (rename_port(fd, &names[i], true))
					syslog(LOG_ERR, "failed to restore port name %s",
					       names[i].old);
			}
			goto out;
		}
	}
	for (i = 0; i < count; i++)
		for (j = 0; j < n; j++)
			if (names[j].ifindex == fabric[i])
				strcpy(interfaces[i], names[j].new);
out:
	close(fd);
	return err;
}
