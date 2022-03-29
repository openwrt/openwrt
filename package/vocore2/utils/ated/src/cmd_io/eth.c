#include "precomp.h"
int sock_eth = -1;
unsigned char packet[PKT_BUF_SIZE];
static const char broadcast_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static char my_eth_addr[6];
struct HOST_IF *hfd;
static int if_index;

int init_eth(struct HOST_IF *fd,char *bridge_ifname)
{
	struct ifreq ifr;
	struct sockaddr_ll addr;
	int s;
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if ((sock_eth=socket(PF_PACKET, SOCK_RAW, cpu2be16/*htons*/(ETH_P_RACFG))) < 0)
	{
		perror("socket");
		return -1;
	}

	os_memset(&ifr, 0, sizeof(ifr));
#ifdef CONFIG_LAN_WAN_SUPPORT
    os_memcpy(ifr.ifr_name, "eth0.1" , 7);
#else
	os_memcpy(ifr.ifr_name, bridge_ifname , os_strlen(bridge_ifname));
#endif
	if (ioctl(sock_eth, SIOCGIFINDEX, &ifr) != 0)
	{
		perror("ioctl(SIOCGIFINDEX)(eth_sock)");
		goto close;
	}

	os_memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = ifr.ifr_ifindex;
	if_index = ifr.ifr_ifindex;

	if (bind(sock_eth, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		perror("bind");
		goto close;
	}

	os_memset(&ifr.ifr_hwaddr.sa_data, 0, ETH_ALEN);
	/*get the eth_addr from kernel and put it into ifreq structure*/
	if (ioctl(sock_eth, SIOCGIFHWADDR, &ifr) != 0)
	{
		perror("ioctl(SIOCGIFHWADDR)(eth_sock)");
		goto close;
	}
	if(fd->need_set_mac){
		os_memcpy(&ifr.ifr_hwaddr.sa_data, &fd->ownmac, ETH_ALEN);
		ate_printf(MSG_INFO,"Set Own Mac: %02x:%02x:%02x:%02x:%02x:%02x\n", (unsigned char) ifr.ifr_hwaddr.sa_data[0], (unsigned char) ifr.ifr_hwaddr.sa_data[1], (unsigned char) ifr.ifr_hwaddr.sa_data[2], (unsigned char) ifr.ifr_hwaddr.sa_data[3], (unsigned char) ifr.ifr_hwaddr.sa_data[4], (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
		if (ioctl(s, SIOCSIFHWADDR, &ifr) != 0)
		{
			perror("ioctl(SIOCSIFHWADDR)(eth_sock)");
			goto close;
		}
		ifr.ifr_flags |= IFF_PROMISC;
		if( ioctl(s, SIOCSIFFLAGS, &ifr) != 0 )
		{
			perror("ioctl for IFF_PROMISC failed.");
			return (-1);
		}
	}
	os_memcpy(my_eth_addr, ifr.ifr_hwaddr.sa_data, 6);

	ate_printf(MSG_INFO,"Open Ethernet socket success\n");
	fd->rev_cmd = &rev_cmd_eth;
	fd->rsp2host = &rsp2host_eth;
	fd->close = &close_eth;
	hfd = fd;
	os_memset(hfd->da, 0, ETH_ALEN);
	return sock_eth;
	close:
	close(sock_eth);
	return (-1);
}

int rev_cmd_eth(unsigned char *buf, int size)
{
	int recv_len = recvfrom(sock_eth, packet, PKT_BUF_SIZE, 0, NULL, NULL);
	struct ethhdr *ehdr = (struct ethhdr *)packet;
	if(recv_len >= (ETH_HLEN + RA_CFG_HLEN)) {
		if ((ehdr->h_proto == cpu2be16/*htons*/(ETH_P_RACFG)) &&
			((strncmp(my_eth_addr, (char *)ehdr->h_dest, 6) == 0) ||
			(strncmp(broadcast_addr, (char *)ehdr->h_dest, 6) == 0)))
		{
			os_memcpy(buf, packet, recv_len);
			return recv_len - 14;
		}
		goto err;
	}
err:
	ate_printf(MSG_ERROR,"packet len is too short!\n");
	return 0;
}

int rsp2host_eth(unsigned char *data, int size)
{
	struct ethhdr	*ehdr;
	struct sockaddr_ll socket_address;
	int result = 0;
	int length = 0;
	unsigned char da[ETH_ALEN];

	os_memset(da, 0, ETH_ALEN);
	os_memcpy(packet, data, size);
	/* respond to QA by broadcast frame */
	ehdr = (struct ethhdr *)&packet[0];
	os_memcpy(da, ehdr->h_source, ETH_ALEN);
	if(hfd->unicast)
		os_memcpy(packet, da, ETH_ALEN);
	else
		os_memcpy(&packet[0], broadcast_addr, 6);
	os_memcpy(ehdr->h_source, my_eth_addr, 6);
	ehdr->h_proto = cpu2be16/*htons*/(ETH_P_RACFG);
	// determine the length to send and send Ack
	length = size;
	if (length < 60) {
		length = 60;
	} else if (length > 1514) {
		ate_printf(MSG_ERROR,"response ethernet length is too long\n");
		return -1;
	}
	socket_address.sll_family = PF_PACKET;
	socket_address.sll_protocol = cpu2be16/*htons*/(ETH_P_RACFG);
	socket_address.sll_ifindex = if_index;
	socket_address.sll_pkttype = PACKET_BROADCAST;
	socket_address.sll_hatype = ARPHRD_ETHER;
	socket_address.sll_halen = ETH_ALEN;

	bzero(&socket_address.sll_addr[0], 8);
	os_memcpy(&socket_address.sll_addr[0], broadcast_addr, 6);
	result = sendto(sock_eth, &packet[0], length, 0, (struct sockaddr *)&socket_address, sizeof(socket_address));
	return result;
}

int close_eth()
{
	ate_printf(MSG_DEBUG,"Ethernet Socket Is Closed\n");
	close(sock_eth);
	return 0;
}
