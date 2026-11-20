// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * What the host daemon and the control tool for Broadcom BCM60xxx HomePlug
 * AV2 chips both need: the interface that leads to the chip and its frames.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "common.h"

int64_t now_ms(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (int64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int if_lookup(const char *ifname, int *ifindex, uint8_t *mac)
{
	struct ifreq ifr = {};
	int fd, ret;

	*ifindex = if_nametoindex(ifname);
	if (!*ifindex)
		return -1;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return -1;

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	if (ret)
		return -1;

	memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

	return 0;
}

/* the frames of the interface that the filter lets through */
int sock_open(int ifindex, const struct sock_fprog *prog)
{
	struct sockaddr_ll sll = {
		.sll_family = AF_PACKET,
		.sll_protocol = htons(ETH_P_ALL),
		.sll_ifindex = ifindex,
	};
	int fd, err;

	/* nothing arrives before bind() names the interface and the protocol */
	fd = socket(AF_PACKET, SOCK_RAW, 0);
	if (fd < 0)
		return -1;

	if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, prog, sizeof(*prog)) ||
	    bind(fd, (struct sockaddr *)&sll, sizeof(sll))) {
		err = errno;
		close(fd);
		errno = err;
		return -1;
	}

	return fd;
}

int mac_parse(const char *str, uint8_t *mac)
{
	struct ether_addr addr;

	if (!ether_aton_r(str, &addr))
		return -1;

	memcpy(mac, addr.ether_addr_octet, ETH_ALEN);

	return 0;
}

void mac_add(uint8_t *mac, unsigned int val)
{
	int i;

	for (i = ETH_ALEN - 1; i >= 0 && val; i--) {
		val += mac[i];
		mac[i] = val;
		val >>= 8;
	}
}
