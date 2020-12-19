/*
 * Simple WASP overlay configuration downloader for AVM FRITZ!Box devices
 *
 * The protocol was found by sniffing ethernet traffic between the two SoCs,
 * so some things might be wrong or incomplete.
 *
 * Important: if the switch is configured for VLAN tagging, the eth0.1 interface
 *            has to be used, not eth0!
 *
 * (c) 2020 Andreas Böhler
 */


#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <poll.h>

#define ETHER_TYPE 			0x88bd
#define BUF_SIZE			1056
#define COUNTER_INCR		4

#define MAX_PAYLOAD_SIZE	1028
#define CHUNK_SIZE			1024
#define WASP_HEADER_LEN		14

#define PACKET_START		0x1200
#define CMD_FIRMWARE_DATA	0x0104
#define CMD_START_FIRMWARE	0xd400

#define RESP_DISCOVER		0x0000
#define RESP_CONFIG			0x1000
#define RESP_OK				0x0100
#define RESP_STARTING		0x0200
#define RESP_ERROR			0x0300

#define PACKET_SIZE			46	

typedef enum {
	STATE_DISCOVERING,
	STATE_TRANSFERRING,
	STATE_FINISHED,
	STATE_FAIL
} t_state;

static int m_socket_initialized = 0;
static t_state m_state = STATE_DISCOVERING;
static int m_packet_counter = 0;

typedef struct __attribute__((packed)) {
	union {
		uint8_t data[MAX_PAYLOAD_SIZE + WASP_HEADER_LEN];
		struct __attribute__((packed)) {
			uint16_t	packet_start;
			uint8_t		pad_one[5];
			uint16_t	command;
			uint16_t	response;
			uint16_t	counter;
			uint8_t		pad_two;
			uint8_t		payload[MAX_PAYLOAD_SIZE];
		};
	};
} t_wasp_packet;

static int send_packet(uint8_t *dest, t_wasp_packet *packet, int payloadlen, char *devname) {
	char sendbuf[BUF_SIZE];
	static int sockfd;
	static struct ifreq if_idx;
	static struct ifreq if_mac;
	struct ether_header *eh = (struct ether_header *) sendbuf;
	int tx_len = 0;
	struct sockaddr_ll socket_address;

	if(!m_socket_initialized) {
		if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    	perror("socket");
	    	return 1;
		}

		/* Get the index of the interface to send on */
		memset(&if_idx, 0, sizeof(struct ifreq));
		strncpy(if_idx.ifr_name, devname, IFNAMSIZ-1);

		if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0) {
			perror("SIOCGIFINDEX");
			return 1;
		}

		/* Get the MAC address of the interface to send on */
		memset(&if_mac, 0, sizeof(struct ifreq));
		strncpy(if_mac.ifr_name, devname, IFNAMSIZ-1);
		if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) {
			perror("SIOCGIFHWADDR");
			return 1;
		}
		m_socket_initialized = 1;
	}
	
	memset(sendbuf, 0, BUF_SIZE);
	
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	for(int i=0; i<6; i++) {
		eh->ether_dhost[i] = dest[i];
	}
	/* Ethertype field */
	eh->ether_type = ETHER_TYPE;
	tx_len += sizeof(struct ether_header);

	for(int i=0; i<WASP_HEADER_LEN; i++) {
		sendbuf[tx_len++] = packet->data[i];
	}
	
	for(int i=0; i<payloadlen; i++) {
		sendbuf[tx_len++] = packet->data[WASP_HEADER_LEN + i];
	}
	
	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */

	for(int i=0; i<6; i++) {
		socket_address.sll_addr[i] = dest[i];
	}

	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
		printf("Send failed\n");
		return 1;
	}

	return 0;

}

int main(int argc, char *argv[]) {
	int sockfd;
	int sockopt;
	uint8_t buf[BUF_SIZE];
	uint8_t dest[6];
	struct ifreq ifopts;	/* set promiscuous mode */
	t_wasp_packet *packet = (t_wasp_packet *) (buf + sizeof(struct ether_header));
	t_wasp_packet s_packet;
	struct pollfd fd;
	int ret;
	int done = 0;
	ssize_t numbytes;
	FILE *fp = NULL;

	printf("AVM WASP Config downloader.\n");

	if(argc < 3) {
		printf("Usage: %s eth0 /tmp/config.tar.gz\n", argv[0]);
		return 1;
	}

	printf("Using file: %s\n", argv[2]);
	printf("Using Dev : %s\n", argv[1]);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;
	//struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
	//struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");	
		return -1;
	}
	
	strncpy(ifopts.ifr_name, argv[1], IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);

	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, argv[1], IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	
	fd.fd = sockfd; // your socket handler 
	fd.events = POLLIN;
	

	
	while(!done) {
		memset(&s_packet, 9, sizeof(s_packet));
		s_packet.packet_start = PACKET_START;
		switch(m_state) {
			case STATE_DISCOVERING:
				s_packet.response = RESP_CONFIG;
				for(int i=0; i<6; i++) {
					dest[i] = 0xff;
				}
			break;
			case STATE_TRANSFERRING:
				s_packet.response = RESP_OK;
			break;
			case STATE_FINISHED:
				s_packet.response = RESP_STARTING;
				done = 1;
			break;
			case STATE_FAIL:
				s_packet.response = RESP_ERROR;
				done = 1;
			break;
			default:
			break;
		}
		send_packet(dest, &s_packet, PACKET_SIZE, argv[1]);
		if(done)
			continue;
		ret = poll(&fd, 1, 1000); // 1 second for timeout
		switch (ret) {
			case -1:
				// Error
				done = 1;
				continue;
				break;
			case 0:
				// Timeout 
				continue;
				break;
			default:
				numbytes = recv(sockfd,buf,BUF_SIZE, 0); // get your data
				break;
		}
		if(eh->ether_type != ETHER_TYPE) {
			printf("Invalid packet type.\n");
			continue;
		}

		if(((packet->packet_start == PACKET_START) && (packet->command == CMD_FIRMWARE_DATA)) ||
		   ((packet->packet_start == PACKET_START) && (packet->response == CMD_START_FIRMWARE))) {
			for(int i=0; i<6; i++) {
				dest[i] = eh->ether_shost[i];
			}
			m_packet_counter = packet->counter;
			if(fp == NULL) {
				fp = fopen(argv[2], "w");
				if(fp == NULL) {
					printf("Error writing file.\n");
					m_state = STATE_FAIL;
					continue;
				}
			}
			fwrite(packet->payload, numbytes - sizeof(struct ether_header) - WASP_HEADER_LEN, 1, fp);
			if(packet->response == CMD_START_FIRMWARE) {
				m_state = STATE_FINISHED;
				fclose(fp);
				fp = NULL;
			} else  {
				m_state = STATE_TRANSFERRING;
			}
		}
	}
	if(fp)
		fclose(fp);
	close(sockfd);

	return 0;
}
