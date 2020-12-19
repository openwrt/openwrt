/*
 * Simple stage 2 firmware uploader for AVM WASP as found in the FRITZ!Box 3390
 *
 * The protocol was found by sniffing ethernet traffic between the two SoCs,
 * so some things might be wrong or incomplete.
 *
 * Important: if the switch is configured for VLAN tagging, the eth0.1 interface
 *            has to be used, not eth0!
 *
 * (c) 2019 Andreas Böhler
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
#include <getopt.h>
#include <libgen.h>

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

typedef enum {
	DOWNLOAD_TYPE_UNKNOWN = 0,
	DOWNLOAD_TYPE_FIRMWARE,
	DOWNLOAD_TYPE_CONFIG
} t_download_type;

static const uint32_t m_load_addr = 0x81a00000;

static uint8_t wasp_mac[] = {0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
static uint16_t m_packet_counter = 0;
static t_download_type m_download_type = DOWNLOAD_TYPE_UNKNOWN;
static int m_socket_initialized = 0;

static char *opt_iface;
static char *opt_filename;
static char *opt_config;
static char *progname;
static int opt_verbose = 0;


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

static int send_packet(t_wasp_packet *packet, int payloadlen, char *devname) {
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
		eh->ether_dhost[i] = wasp_mac[i];
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
		socket_address.sll_addr[i] = wasp_mac[i];
	}

	if(opt_verbose) {
		printf("Send (%d bytes): ", tx_len);
		for(int i=0; i<tx_len; i++) {
			printf("0x%x ", sendbuf[i]);
		}
		printf("\n");
	}

	/* Send packet */
	if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	return 0;

}

static int check_options(void) {
	if(!opt_filename) {
		fprintf(stderr, "No input filename specified.\n");
		return -1;
	}

	if(!opt_iface) {
		fprintf(stderr, "No interface specified.\n");
		return -1;
	}

	return 0;
}

static void usage(int status)
{
	fprintf(stderr, "Usage: %s [OPTIONS...]\n", progname);
	fprintf(stderr,
"\n"
"Options:\n"
"  -i <interface>  use the specified Ethernet interface\n"
"  -f <file>       upload the specified firmware file\n"
"  -c <file>       upload the optional config file\n"
"  -v              verbose output\n"
"  -h              show this screen\n"
	);

	exit(status);
}

int main(int argc, char *argv[]) {
	int sockfd;
	int valid = 1;
	int done = 0;
	int i;
	int sockopt;
	uint8_t buf[BUF_SIZE];
	struct ifreq ifopts;	/* set promiscuous mode */
	ssize_t numbytes;
	t_wasp_packet *packet = (t_wasp_packet *) (buf + sizeof(struct ether_header));
	t_wasp_packet s_packet;
	FILE *fp = NULL;
	char *fn;
	ssize_t read;
	int data_offset = 0;
	int fsize;
	int cfgsize;
	int num_chunks;
	int chunk_counter = 1;
	progname = basename(argv[0]);
	int ret = EXIT_FAILURE;
	
	while(1) {
		int c;

		c = getopt(argc, argv, "i:f:c:hv");
		if(c == -1)
			break;

		switch(c) {
		
		case 'i':
			opt_iface = optarg;
			break;

		case 'f':
			opt_filename = optarg;
			break;

		case 'c':
			opt_config = optarg;
			break;

		case 'v':
			opt_verbose = 1;
			break;

		case 'h':
			usage(EXIT_SUCCESS);
			break;

		default:
			usage(EXIT_FAILURE);
			break;
		}
	}
	
	ret = check_options();
	if(ret)
		return ret;



	printf("AVM WASP Stage 2 uploader.\n");
	
	printf("Using file  : %s\n", opt_filename);
	printf("Using Dev   : %s\n", opt_iface);
	if(opt_config) {
		printf("Using config: %s\n", opt_config);
		
		fp = fopen(opt_config, "rb");
		if(fp == NULL) {
			printf("Input file not found: %s\n", opt_config);
		}
		fseek(fp, 0, SEEK_END);
		cfgsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fclose(fp);
		fp = NULL;
	}
	
	fp = fopen(opt_filename, "rb");
	if(fp == NULL) {
		printf("Input file not found: %s\n", opt_filename);
	}
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	fp = NULL;

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;
	//struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
	//struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");	
		return -1;
	}
	
	strncpy(ifopts.ifr_name, opt_iface, IFNAMSIZ-1);
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
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, opt_iface, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	//FIXME: Timeout
	while(!done) {
		numbytes = recvfrom(sockfd, buf, BUF_SIZE, 0, NULL, NULL);
		if(opt_verbose) {
			printf("Recv (%ld bytes): ", numbytes);
			for(int i=0; i<numbytes; i++) {
				printf("0x%x ", buf[i]);
			}
			printf("\n");
		}

		if(numbytes < 30) {
			fprintf(stderr, "Packet too small, discarding\n");
			continue;
		}
		
		if(eh->ether_type != ETHER_TYPE)
			valid = 0;
		
		if(!valid)
			continue;
			
		for(i=0; i<6; i++) {
			wasp_mac[i] = eh->ether_shost[i];
		}
		
		memset(&s_packet, 0, sizeof(s_packet));
		
		if((packet->packet_start == PACKET_START) && (packet->response == RESP_DISCOVER)) {
			if(opt_verbose)
				printf("Got discovery packet, starting firmware download...\n");
			m_packet_counter = 0;
			m_download_type = DOWNLOAD_TYPE_FIRMWARE;
			fn = opt_filename;
			chunk_counter = 1;
			num_chunks = fsize / CHUNK_SIZE;
			if(fsize % CHUNK_SIZE != 0) {
				num_chunks++;
			}
			if(opt_verbose)
				printf("Going to send %d chunks.\n", num_chunks);
		} else if((packet->packet_start == PACKET_START) && (packet->response == RESP_CONFIG)) {
			if(opt_verbose)
				printf("Got config discovery packet, starting config download...\n");
			m_packet_counter = 0;
			m_download_type = DOWNLOAD_TYPE_CONFIG;
			fn = opt_config;
			chunk_counter = 1;
			num_chunks = cfgsize / CHUNK_SIZE;
			if(cfgsize % CHUNK_SIZE != 0) {
				num_chunks++;
			}

			if(opt_verbose)
				printf("Going to send %d chunks.\n", num_chunks);
		} else if((packet->packet_start == PACKET_START) && (packet->response == RESP_OK)) {

			//printf("Got reply, sending next chunk...\n");
		} else if((packet->packet_start == PACKET_START) && (packet->response == RESP_ERROR)) {
			fprintf(stderr, "Received an error packet!\n");
			done = 1;
			continue;
		} else if((packet->packet_start == PACKET_START) && (packet->response == RESP_STARTING)) {
			if(m_download_type == DOWNLOAD_TYPE_FIRMWARE) {
				printf("Successfully uploaded stage 2 firmware!\n");
			} else {
				printf("Successfully uploaded config file!\n");
				done = 1;
			}
			if(fp) {
				fclose(fp);
				fp = NULL;
			}
			if(!opt_config) {
				done = 1;
			}
			continue;
		} else {
			fprintf(stderr, "Got unknown packet!\n");
			continue;
		}
		if(m_packet_counter == 0) {
			if(fp == NULL) {
				fp = fopen(fn, "rb");
			}
			else {
				fseek(fp, 0, SEEK_SET);
			}
			if(m_download_type == DOWNLOAD_TYPE_FIRMWARE) {
				memcpy(s_packet.payload, &m_load_addr, sizeof(m_load_addr));
				data_offset = sizeof(m_load_addr);
			} else {
				data_offset = 0;
			}
		} else {
			data_offset = 0;
		}
		if(!feof(fp)) {
			read = fread(&s_packet.payload[data_offset], 1, CHUNK_SIZE, fp);
			s_packet.packet_start = PACKET_START;
			if(chunk_counter == num_chunks) {
				s_packet.response = CMD_START_FIRMWARE;
				if(m_download_type == DOWNLOAD_TYPE_FIRMWARE) {
					memcpy(&s_packet.payload[data_offset + read], &m_load_addr, sizeof(m_load_addr));
					data_offset += sizeof(m_load_addr);
				}
			} else {
				s_packet.command = CMD_FIRMWARE_DATA;
			}
			s_packet.counter = m_packet_counter;
			if(send_packet(&s_packet, read + data_offset, opt_iface) != 0) {
				fprintf(stderr, "Error sending packet.\n");
				continue;
			}
			m_packet_counter += COUNTER_INCR;
			chunk_counter++;
		} else {
			fclose(fp);
			fp = NULL;
		}
	}
	if(fp)
		fclose(fp);
	close(sockfd);
	
	return 0;
}
