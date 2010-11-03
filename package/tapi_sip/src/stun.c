#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <poll.h>


struct stun_client {
	struct addrinfo *serverinfo;
};

struct stun_response {
	struct sockaddr addr;
};

struct stun_header {
	uint16_t type;
	uint16_t length;
	uint32_t cookie;
	uint32_t id[3];
} __attribute((packed));

struct stun_packet {
	struct stun_header header;
	uint8_t data[0];
} __attribute((packed));

#define STUN_CLASS(c0, c1) (((c0) << 4) | ((c1) << 8))

#define STUN_CLASS_REQUEST	STUN_CLASS(0, 0)
#define STUN_CLASS_INDICATION	STUN_CLASS(0, 1)
#define STUN_CLASS_SUCCESS	STUN_CLASS(1, 0)
#define STUN_CLASS_ERROR	STUN_CLASS(1, 1)

#define STUN_CLASS_MASK STUN_CLASS(1, 1)

#define STUN_MESSAGE(msg) (((msg & 0xf10) << 2) | ((msg & 0x70) << 1) | (msg & 0xf))
#define STUN_MESSAGE_BIND STUN_MESSAGE(1)

#define STUN_COOKIE 0x2112a442

enum {
	STUN_ATTR_TYPE_MAPPED_ADDRESS = 0x1,
	STUN_ATTR_TYPE_XOR_MAPPED_ADDRESS = 0x20,
	STUN_ATTR_TYPE_XOR_MAPPED_ADDRESS2 = 0x8020,
};

static inline uint16_t get_unaligned_be16(const uint8_t *buf)
{
	return (buf[0] << 8) | buf[1];
}

static inline uint16_t get_unaligned_be32(const uint8_t *buf)
{
	return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

static int stun_parse_xor_mapped_address(struct stun_response *response,
	const uint8_t *buf, int length)
{
	uint8_t fam = buf[1];
	uint16_t port = get_unaligned_be16(&buf[2]);
	struct sockaddr_in *sin = (struct sockaddr_in *)&response->addr;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&response->addr;


	switch (fam) {
	case 0x1:
		sin->sin_family = AF_INET;
		sin->sin_port = htons((port ^ (uint16_t)((STUN_COOKIE & 0xffff0000) >> 16)));
		memcpy(&sin->sin_addr.s_addr, buf + 4, 4);
		sin->sin_addr.s_addr ^= htonl(STUN_COOKIE);
		printf("xor port: %d\n", sin->sin_port);
		break;
	case 0x2:
		sin6->sin6_family = AF_INET6;
		sin->sin_port = htons((port ^ (uint16_t)((STUN_COOKIE & 0xffff0000) >> 16)));
		memcpy(sin6->sin6_addr.s6_addr, buf + 4, 16);
		break;
	}

	return 0;
}

static int stun_parse_mapped_address(struct stun_response *response,
	const uint8_t *buf, int length)
{
	uint8_t fam = buf[1];
	uint16_t port = get_unaligned_be16(&buf[2]);
	struct sockaddr_in *sin = (struct sockaddr_in *)&response->addr;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&response->addr;

	printf("port: %d\n", port);

	switch (fam) {
	case 0x1:
		sin->sin_family = AF_INET;
		sin->sin_port = htons(port);
		memcpy(&sin->sin_addr.s_addr, buf + 4, 4);
		break;
	case 0x2:
		sin6->sin6_family = AF_INET6;
		sin6->sin6_port = htons(port);
		memcpy(sin6->sin6_addr.s6_addr, buf + 4, 16);
		break;
	}

	return 0;
}

static int stun_parse_response(struct stun_response *response,
	const struct stun_packet *packet)
{
	uint16_t attr_type, attr_length;
	const uint8_t *buf;
	int length = ntohs(packet->header.length);
	int ret;
	int i = 0;

	if (packet->header.cookie != htonl(STUN_COOKIE))
		return -1;

	if (packet->header.length < 4)
		return 0;

	buf = packet->data;

	do {
		attr_type = get_unaligned_be16(&buf[i]);
		attr_length = get_unaligned_be16(&buf[i + 2]);
		i += 4;

		if (i + attr_length > length)
			break;

		switch (attr_type) {
		case STUN_ATTR_TYPE_MAPPED_ADDRESS:
			ret = stun_parse_mapped_address(response, &buf[i], attr_length);
			break;
		case STUN_ATTR_TYPE_XOR_MAPPED_ADDRESS:
		case STUN_ATTR_TYPE_XOR_MAPPED_ADDRESS2:
			ret = stun_parse_xor_mapped_address(response, &buf[i], attr_length);
			break;
		}

		i += attr_length;

	} while (i < length && ret == 0);

	return 0;
}

static struct stun_packet *stun_packet_alloc(size_t data_size)
{
	return malloc(sizeof(struct stun_packet) + data_size);
}

int stun_client_resolve(struct stun_client *stun, int sockfd, struct sockaddr *addr)
{
	struct stun_packet *packet = stun_packet_alloc(200);
	struct stun_response response;
	int ret;
	int retries = 4;
	int timeout = 500;
	struct pollfd pollfd;

	pollfd.events = POLLIN;
	pollfd.fd = sockfd;

	packet->header.type = htons(STUN_CLASS_REQUEST | STUN_MESSAGE_BIND);
	packet->header.cookie = htonl(STUN_COOKIE);
	packet->header.id[0] = 0x12345678;
	packet->header.id[1] = 0x12345678;
	packet->header.id[2] = 0x12345678;
	packet->header.length = 0;

	while (retries--) {
		ret = sendto(sockfd, packet, sizeof(struct stun_header) + packet->header.length,
			0, stun->serverinfo->ai_addr, stun->serverinfo->ai_addrlen);

		ret = poll(&pollfd, 1, timeout);
		switch (ret) {
		case 0:
			timeout <<= 1;
		case -EINTR:
			printf("retry\n");
			continue;
		default:
			retries = 0;
		}
		ret = recvfrom(sockfd, packet, 200, 0, NULL, NULL);
	}

	if (ret <= 0)
		return ret ? ret : -ETIMEDOUT;

	memset(&response, 0, sizeof(response));
	ret = stun_parse_response(&response, packet);

	*addr = response.addr;

	return ret;
}

struct stun_client *stun_client_alloc(const char *hostname, uint16_t port)
{
    struct addrinfo hints;
	struct stun_client *stun;
	int ret;
	char p[6];


	stun = malloc(sizeof(*stun));
	if (!stun)
		return NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_NUMERICSERV;

	snprintf(p, sizeof(p), "%d", port);
    if ((ret = getaddrinfo(hostname, p, &hints, &stun->serverinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return NULL;
    }


	return stun;
}

void stun_client_free(struct stun_client *stun)
{
	freeaddrinfo(stun->serverinfo);
	free(stun);
}
