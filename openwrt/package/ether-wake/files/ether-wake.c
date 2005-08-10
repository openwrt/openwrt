/* ether-wake.c: Send a magic packet to wake up sleeping machines. */

static char version_msg[] =
"ether-wake.c: v1.09 11/12/2003 Donald Becker, http://www.scyld.com/";
static char brief_usage_msg[] =
"usage: ether-wake [-i <ifname>] [-p aa:bb:cc:dd[:ee:ff]] 00:11:22:33:44:55\n"
"   Use '-u' to see the complete set of options.\n";
static char usage_msg[] =
"usage: ether-wake [-i <ifname>] [-p aa:bb:cc:dd[:ee:ff]] 00:11:22:33:44:55\n"
"\n"
"	This program generates and transmits a Wake-On-LAN (WOL)\n"
"	\"Magic Packet\", used for restarting machines that have been\n"
"	soft-powered-down (ACPI D3-warm state).\n"
"	It currently generates the standard AMD Magic Packet format, with\n"
"	an optional password appended.\n"
"\n"
"	The single required parameter is the Ethernet MAC (station) address\n"
"	of the machine to wake or a host ID with known NSS 'ethers' entry.\n"
"	The MAC address may be found with the 'arp' program while the target\n"
"	machine is awake.\n"
"\n"
"	Options:\n"
"		-b	Send wake-up packet to the broadcast address.\n"
"		-D	Increase the debug level.\n"
"		-i ifname	Use interface IFNAME instead of the default 'eth0'.\n"
"		-p <pw>		Append the four or six byte password PW to the packet.\n"
"					A password is only required for a few adapter types.\n"
"					The password may be specified in ethernet hex format\n"
"					or dotted decimal (Internet address)\n"
"		-p 00:22:44:66:88:aa\n"
"		-p 192.168.1.1\n";

/*
	This program generates and transmits a Wake-On-LAN (WOL) "Magic Packet",
	used for restarting machines that have been soft-powered-down
	(ACPI D3-warm state).  It currently generates the standard AMD Magic Packet
	format, with an optional password appended.

	This software may be used and distributed according to the terms
	of the GNU Public License, incorporated herein by reference.
	Contact the author for use under other terms.

	This source file was originally part of the network tricks package, and
	is now distributed to support the Scyld Beowulf system.
	Copyright 1999-2003 Donald Becker and Scyld Computing Corporation.

	The author may be reached as becker@scyld, or C/O
	 Scyld Computing Corporation
	 914 Bay Ridge Road, Suite 220
	 Annapolis MD 21403

  Notes:
  On some systems dropping root capability allows the process to be
  dumped, traced or debugged.
  If someone traces this program, they get control of a raw socket.
  Linux handles this safely, but beware when porting this program.

  An alternative to needing 'root' is using a UDP broadcast socket, however
  doing so only works with adapters configured for unicast+broadcast Rx
  filter.  That configuration consumes more power.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#if 0							/* Only exists on some versions. */
#include <ioctls.h>
#endif

#include <sys/socket.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/if.h>

#include <features.h>
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif
#include <netdb.h>
#include <netinet/ether.h>

/* Grrr, no consistency between include versions.
   Enable this if setsockopt() isn't declared with your library. */
#if 0
extern int setsockopt __P ((int __fd, int __level, int __optname,
							__ptr_t __optval, int __optlen));
#else				/* New, correct head files.  */
#include <sys/socket.h>
#endif

u_char outpack[1000];
int outpack_sz = 0;
int debug = 0;
u_char wol_passwd[6];
int wol_passwd_sz = 0;

static int opt_no_src_addr = 0, opt_broadcast = 0;

static int get_dest_addr(const char *arg, struct ether_addr *eaddr);
static int get_fill(unsigned char *pkt, struct ether_addr *eaddr);
static int get_wol_pw(const char *optarg);

int main(int argc, char *argv[])
{
	char *ifname = "eth0";
	int one = 1;				/* True, for socket options. */
	int s;						/* Raw socket */
	int errflag = 0, verbose = 0, do_version = 0;
	int perm_failure = 0;
	int i, c, pktsize;
#if defined(PF_PACKET)
	struct sockaddr_ll whereto;
#else
	struct sockaddr whereto;	/* who to wake up */
#endif
	struct ether_addr eaddr;

	while ((c = getopt(argc, argv, "bDi:p:uvV")) != -1)
		switch (c) {
		case 'b': opt_broadcast++;	break;
		case 'D': debug++;			break;
		case 'i': ifname = optarg;	break;
		case 'p': get_wol_pw(optarg); break;
		case 'u': printf(usage_msg); return 0;
		case 'v': verbose++;		break;
		case 'V': do_version++;		break;
		case '?':
			errflag++;
		}
	if (verbose || do_version)
		printf("%s\n", version_msg);
	if (errflag) {
		fprintf(stderr, brief_usage_msg);
		return 3;
	}

	if (optind == argc) {
		fprintf(stderr, "Specify the Ethernet address as 00:11:22:33:44:55.\n");
		return 3;
	}

	/* Note: PF_INET, SOCK_DGRAM, IPPROTO_UDP would allow SIOCGIFHWADDR to
	   work as non-root, but we need SOCK_PACKET to specify the Ethernet
	   destination address. */
#if defined(PF_PACKET)
	s = socket(PF_PACKET, SOCK_RAW, 0);
#else
	s = socket(AF_INET, SOCK_PACKET, SOCK_PACKET);
#endif
	if (s < 0) {
		if (errno == EPERM)
			fprintf(stderr, "ether-wake: This program must be run as root.\n");
		else
			perror("ether-wake: socket");
		perm_failure++;
	}
	/* Don't revert if debugging allows a normal user to get the raw socket. */
	setuid(getuid());

	/* We look up the station address before reporting failure so that
	   errors may be reported even when run as a normal user.
	*/
	if (get_dest_addr(argv[optind], &eaddr) != 0)
		return 3;
	if (perm_failure && ! debug)
		return 2;

	pktsize = get_fill(outpack, &eaddr);

	/* Fill in the source address, if possible.
	   The code to retrieve the local station address is Linux specific. */
	if (! opt_no_src_addr) {
		struct ifreq if_hwaddr;
		unsigned char *hwaddr = if_hwaddr.ifr_hwaddr.sa_data;

		strcpy(if_hwaddr.ifr_name, ifname);
		if (ioctl(s, SIOCGIFHWADDR, &if_hwaddr) < 0) {
			fprintf(stderr, "SIOCGIFHWADDR on %s failed: %s\n", ifname,
					strerror(errno));
			/* Magic packets still work if our source address is bogus, but
			   we fail just to be anal. */
			return 1;
		}
		memcpy(outpack+6, if_hwaddr.ifr_hwaddr.sa_data, 6);

		if (verbose) {
			printf("The hardware address (SIOCGIFHWADDR) of %s is type %d  "
				   "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x.\n", ifname,
				   if_hwaddr.ifr_hwaddr.sa_family, hwaddr[0], hwaddr[1],
				   hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
		}
	}

	if (wol_passwd_sz > 0) {
		memcpy(outpack+pktsize, wol_passwd, wol_passwd_sz);
		pktsize += wol_passwd_sz;
	}

	if (verbose > 1) {
		printf("The final packet is: ");
		for (i = 0; i < pktsize; i++)
			printf(" %2.2x", outpack[i]);
		printf(".\n");
	}

	/* This is necessary for broadcasts to work */
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&one, sizeof(one)) < 0)
		perror("setsockopt: SO_BROADCAST");

#if defined(PF_PACKET)
	{
		struct ifreq ifr;
		strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
		if (ioctl(s, SIOCGIFINDEX, &ifr) == -1) {
			fprintf(stderr, "SIOCGIFINDEX on %s failed: %s\n", ifname,
					strerror(errno));
			return 1;
		}
		memset(&whereto, 0, sizeof(whereto));
		whereto.sll_family = AF_PACKET;
		whereto.sll_ifindex = ifr.ifr_ifindex;
		/* The manual page incorrectly claims the address must be filled.
		   We do so because the code may change to match the docs. */
		whereto.sll_halen = ETH_ALEN;
		memcpy(whereto.sll_addr, outpack, ETH_ALEN);

	}
#else
	whereto.sa_family = 0;
	strcpy(whereto.sa_data, ifname);
#endif

	if ((i = sendto(s, outpack, pktsize, 0, (struct sockaddr *)&whereto,
					sizeof(whereto))) < 0)
		perror("sendto");
	else if (debug)
		printf("Sendto worked ! %d.\n", i);

#ifdef USE_SEND
	if (bind(s, (struct sockaddr *)&whereto, sizeof(whereto)) < 0)
		perror("bind");
	else if (send(s, outpack, 100, 0) < 0)
		perror("send");
#endif
#ifdef USE_SENDMSG
	{
		struct msghdr msghdr = { 0,};
		struct iovec iovector[1];
		msghdr.msg_name = &whereto;
		msghdr.msg_namelen = sizeof(whereto);
		msghdr.msg_iov = iovector;
		msghdr.msg_iovlen = 1;
		iovector[0].iov_base = outpack;
		iovector[0].iov_len = pktsize;
		if ((i = sendmsg(s, &msghdr, 0)) < 0)
			perror("sendmsg");
		else if (debug)
			printf("sendmsg worked, %d (%d).\n", i, errno);
	}
#endif

	return 0;
}

/* Convert the host ID string to a MAC address.
   The string may be a
	Host name
    IP address string
	MAC address string
*/

static int get_dest_addr(const char *hostid, struct ether_addr *eaddr)
{
	struct ether_addr *eap;

	eap = ether_aton(hostid);
	if (eap) {
		*eaddr = *eap;
		if (debug)
			fprintf(stderr, "The target station address is %s.\n",
					ether_ntoa(eaddr));
	} else if (ether_hostton(hostid, eaddr) == 0) {
		if (debug)
			fprintf(stderr, "Station address for hostname %s is %s.\n",
					hostid, ether_ntoa(eaddr));
	} else {
		(void)fprintf(stderr,
					  "ether-wake: The Magic Packet host address must be "
					  "specified as\n"
					  "  - a station address, 00:11:22:33:44:55, or\n"
					  "  - a hostname with a known 'ethers' entry.\n");
		return -1;
	}
	return 0;
}


static int get_fill(unsigned char *pkt, struct ether_addr *eaddr)
{
	int offset, i;
	unsigned char *station_addr = eaddr->ether_addr_octet;

	if (opt_broadcast)
		memset(pkt+0, 0xff, 6);
	else
		memcpy(pkt, station_addr, 6);
	memcpy(pkt+6, station_addr, 6);
	pkt[12] = 0x08;				/* Or 0x0806 for ARP, 0x8035 for RARP */
	pkt[13] = 0x42;
	offset = 14;

	memset(pkt+offset, 0xff, 6);
	offset += 6;

	for (i = 0; i < 16; i++) {
		memcpy(pkt+offset, station_addr, 6);
		offset += 6;
	}
	if (debug) {
		fprintf(stderr, "Packet is ");
		for (i = 0; i < offset; i++)
			fprintf(stderr, " %2.2x", pkt[i]);
		fprintf(stderr, ".\n");
	}
	return offset;
}

static int get_wol_pw(const char *optarg)
{
	int passwd[6];
	int byte_cnt;
	int i;

	byte_cnt = sscanf(optarg, "%2x:%2x:%2x:%2x:%2x:%2x",
					  &passwd[0], &passwd[1], &passwd[2],
					  &passwd[3], &passwd[4], &passwd[5]);
	if (byte_cnt < 4)
		byte_cnt = sscanf(optarg, "%d.%d.%d.%d",
						  &passwd[0], &passwd[1], &passwd[2], &passwd[3]);
	if (byte_cnt < 4) {
		fprintf(stderr, "Unable to read the Wake-On-LAN password.\n");
		return 0;
	}
	printf(" The Magic packet password is %2.2x %2.2x %2.2x %2.2x (%d).\n",
		   passwd[0], passwd[1], passwd[2], passwd[3], byte_cnt);
	for (i = 0; i < byte_cnt; i++)
		wol_passwd[i] = passwd[i];
	return wol_passwd_sz = byte_cnt;
}

#if 0
{
	to = (struct sockaddr_in *)&whereto;
	to->sin_family = AF_INET;
	if (inet_aton(target, &to->sin_addr)) {
		hostname = target;
	}
	memset (&sa, 0, sizeof sa);
	sa.sa_family = AF_INET;
	strncpy (sa.sa_data, interface, sizeof sa.sa_data);
	sendto (sock, buf, bufix + len, 0, &sa, sizeof sa);
	strncpy (sa.sa_data, interface, sizeof sa.sa_data);
#if 1
	sendto (sock, buf, bufix + len, 0, &sa, sizeof sa);
#else
	bind (sock, &sa, sizeof sa);
	connect();
	send (sock, buf, bufix + len, 0);
#endif
}
#endif


/*
 * Local variables:
 *  compile-command: "gcc -O -Wall -o ether-wake ether-wake.c"
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  c-indent-level: 4
 *  tab-width: 4
 * End:
 */
