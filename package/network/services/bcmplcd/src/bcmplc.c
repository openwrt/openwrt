// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Control tool for Broadcom BCM60xxx HomePlug AV2 chips
 *
 * Talks to the running firmware of the chip with its management frames:
 * push-button pairing, the stations of the powerline network, the network
 * key, the LEDs of an adapter, the Ethernet socket on the chip, a restart.
 */

#include <errno.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

/* version, type (le16), two zero bytes, OUI, request number, payload */
#define MME_HDR_LEN		23
#define MME_TIMEOUT		500	/* ms */
#define MME_RETRIES		3

enum {
	MME_KEY_SET = 0xa018,
	MME_RESTART = 0xa020,
	MME_STATIONS = 0xa02c,
	MME_PARAM_SET = 0xa058,
	MME_PARAM_GET = 0xa05c,
	MME_ETH = 0xa060,
	MME_IDENTIFY = 0xa070,
	MME_PAIR = 0xa08c,
};

enum {
	PAIR_START = 0,
	PAIR_STATUS = 2,
	PAIR_STOP = 3,
};

enum {
	PAIR_DONE,
	PAIR_FAILED,
	PAIR_TIMEOUT,
	PAIR_ONGOING,
	PAIR_UNCHANGED,
	PAIR_IDLE,
};

/* index of the network identifier and key in the parameters of the chip */
#define PARAM_NID		35
#define PARAM_NMK		36
#define NID_LEN			7
#define NMK_LEN			16

/* the parameters that hold the LED settings, as the stock firmware sets them */
#define PARAM_LED_A		62
#define PARAM_LED_B		63
#define PARAM_LED_C		149

struct param {
	uint8_t size;
	uint16_t count;
	uint8_t data[64];
};

#define PAIR_WAIT		130	/* s */
#define KEY_WAIT		10	/* s */

/* the running pairing, which cancel asks to end it */
#define PAIR_PID_FILE		"/var/run/bcmplc-pair.pid"

static const uint8_t oui[] = { 0x00, 0x1f, 0x84 };
static const uint8_t identify_key[] = {
	0xa3, 0x97, 0xa2, 0x55, 0x53, 0xbe, 0xf1, 0xfc,
	0xf9, 0x79, 0x6b, 0x52, 0x14, 0x13, 0xe9, 0xe2,
};

static const char *ifname = "eth0";
static int sock = -1;
static int ifindex;
static uint8_t if_mac[ETH_ALEN];
static uint8_t soc_mac[ETH_ALEN];
static uint8_t req_num;
static volatile sig_atomic_t pair_stop;
static bool quiet;

static int sock_setup(void)
{
	struct sock_filter insns[] = {
		BPF_STMT(BPF_LD | BPF_H | BPF_ABS, 12),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ETH_P_BCMPLC, 0, 1),
		BPF_STMT(BPF_RET | BPF_K, 0xffff),
		BPF_STMT(BPF_RET | BPF_K, 0),
	};
	struct sock_fprog prog = {
		.len = sizeof(insns) / sizeof(insns[0]),
		.filter = insns,
	};

	if (if_lookup(ifname, &ifindex, if_mac)) {
		fprintf(stderr, "interface %s not found\n", ifname);
		return -1;
	}

	sock = sock_open(ifindex, &prog);
	if (sock < 0) {
		fprintf(stderr, "cannot open a socket on %s: %s\n", ifname, strerror(errno));
		return -1;
	}

	return 0;
}

/*
 * Send a request and wait for its answer: the type is one above the one of
 * the request, and it carries the request number.
 */
static int mme(const uint8_t *dst, uint16_t type, const void *req, size_t req_len,
	       uint8_t *resp, size_t resp_len)
{
	struct sockaddr_ll sll = {
		.sll_family = AF_PACKET,
		.sll_ifindex = ifindex,
		.sll_halen = ETH_ALEN,
	};
	uint8_t frame[FRAME_MAX] = {};
	size_t len = MME_HDR_LEN + req_len;
	uint8_t num = ++req_num;
	int try;

	memcpy(frame, dst, ETH_ALEN);
	memcpy(frame + ETH_ALEN, if_mac, ETH_ALEN);
	frame[12] = ETH_P_BCMPLC >> 8;
	frame[13] = ETH_P_BCMPLC & 0xff;
	frame[14] = type == MME_IDENTIFY ? 1 : 2;
	frame[15] = type & 0xff;
	frame[16] = type >> 8;
	memcpy(frame + 19, oui, sizeof(oui));
	frame[22] = num;
	memcpy(frame + MME_HDR_LEN, req, req_len);

	if (len < FRAME_MIN)
		len = FRAME_MIN;

	memcpy(sll.sll_addr, dst, ETH_ALEN);

	for (try = 0; try < MME_RETRIES; try++) {
		int64_t end = now_ms() + MME_TIMEOUT;

		if (sendto(sock, frame, len, 0, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
			perror("send");
			return -1;
		}

		while (1) {
			struct pollfd pfd = { .fd = sock, .events = POLLIN };
			uint8_t buf[FRAME_MAX];
			int64_t left = end - now_ms();
			ssize_t n;

			if (left <= 0 || poll(&pfd, 1, left) <= 0)
				break;

			n = recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
			if (n < MME_HDR_LEN || memcmp(buf + ETH_ALEN, dst, ETH_ALEN) ||
			    (buf[15] | buf[16] << 8) != type + 1 ||
			    memcmp(buf + 19, oui, sizeof(oui)) || buf[22] != num)
				continue;

			n -= MME_HDR_LEN;
			if ((size_t)n > resp_len)
				n = resp_len;

			memcpy(resp, buf + MME_HDR_LEN, n);

			return n;
		}
	}

	if (!quiet)
		fprintf(stderr, "the chip does not answer\n");

	return -1;
}

static int pair_cmd(uint8_t cmd)
{
	uint8_t resp[1];

	if (mme(soc_mac, MME_PAIR, &cmd, 1, resp, sizeof(resp)) < 1)
		return -1;

	if (resp[0] & 0x0f)
		return -1;

	return resp[0] >> 4;
}

/* the process of the pairing under way on this host, or 0 */
static pid_t pair_pid(void)
{
	char path[32], comm[16] = "";
	FILE *f;
	int pid = 0;

	f = fopen(PAIR_PID_FILE, "r");
	if (!f)
		return 0;

	if (fscanf(f, "%d", &pid) != 1)
		pid = 0;
	fclose(f);

	if (pid <= 0 || pid == getpid())
		return 0;

	/* a stale file can name another process by now */
	snprintf(path, sizeof(path), "/proc/%d/comm", pid);
	f = fopen(path, "r");
	if (!f)
		return 0;

	if (!fgets(comm, sizeof(comm), f))
		comm[0] = 0;
	fclose(f);

	return strcmp(comm, "bcmplc\n") ? 0 : pid;
}

static void on_stop(int sig)
{
	(void)sig;
	pair_stop = 1;
}

static int cmd_pair(void)
{
	struct sigaction sa = { .sa_handler = on_stop };
	int64_t end = now_ms() + PAIR_WAIT * 1000;
	FILE *f;
	int state;

	if (pair_pid()) {
		fprintf(stderr, "a pairing is under way\n");
		return 1;
	}

	f = fopen(PAIR_PID_FILE, "w");
	if (f) {
		fprintf(f, "%d\n", getpid());
		fclose(f);
	}

	/* without SA_RESTART, so that the signal ends the wait at once */
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	if (pair_cmd(PAIR_START)) {
		fprintf(stderr, "pairing did not start\n");
		unlink(PAIR_PID_FILE);
		return 1;
	}

	printf("pairing started, press the pair button of the other adapter\n");
	fflush(stdout);

	do {
		sleep(1);
		state = pair_stop ? PAIR_IDLE : pair_cmd(PAIR_STATUS);
	} while (state == PAIR_ONGOING && now_ms() < end);

	/* only this process talks to the chip about the pairing */
	if (pair_stop)
		pair_cmd(PAIR_STOP);

	unlink(PAIR_PID_FILE);

	switch (state) {
	case PAIR_DONE:
		printf("paired\n");
		return 0;
	case PAIR_FAILED:
		printf("pairing failed\n");
		break;
	case PAIR_TIMEOUT:
	case PAIR_ONGOING:
		printf("no other adapter answered\n");
		break;
	case PAIR_UNCHANGED:
		printf("pairing ended, the network is unchanged\n");
		break;
	case PAIR_IDLE:
		printf("pairing cancelled\n");
		break;
	default:
		return 1;
	}

	return 2;
}

static int cmd_cancel(void)
{
	pid_t pid = pair_pid();

	/* the pairing on this host ends it with the chip itself */
	if (pid)
		return kill(pid, SIGTERM) ? 1 : 0;

	return pair_cmd(PAIR_STOP) ? 1 : 0;
}

static int cmd_status(void)
{
	static const char * const names[] = {
		[PAIR_DONE] = "paired",
		[PAIR_FAILED] = "failed",
		[PAIR_TIMEOUT] = "timeout",
		[PAIR_ONGOING] = "pairing",
		[PAIR_UNCHANGED] = "unchanged",
		[PAIR_IDLE] = "idle",
	};
	int state = pair_cmd(PAIR_STATUS);

	if (state < 0 || state > PAIR_IDLE)
		return 1;

	printf("%s\n", names[state]);

	return 0;
}

static int identify(const uint8_t *mac, char *str, size_t len)
{
	uint8_t resp[2 + 64];
	int n;

	n = mme(mac, MME_IDENTIFY, identify_key, sizeof(identify_key), resp, sizeof(resp));
	if (n < 2 || resp[1] > n - 2 || resp[1] >= len)
		return -1;

	memcpy(str, resp + 2, resp[1]);
	str[resp[1]] = 0;

	return 0;
}

static void print_mac(const uint8_t *mac)
{
	fputs(ether_ntoa((const struct ether_addr *)mac), stdout);
}

/* answer: size of an element, number of elements (le16), elements */
static int param_read(const uint8_t *mac, uint16_t index, struct param *p)
{
	uint8_t req[2] = { index & 0xff, index >> 8 };
	uint8_t resp[3 + sizeof(p->data)];
	size_t len;
	int n;

	n = mme(mac, MME_PARAM_GET, req, sizeof(req), resp, sizeof(resp));
	if (n < 3)
		return -1;

	p->size = resp[0];
	p->count = resp[1] | resp[2] << 8;
	len = p->size * p->count;
	if (!len || len > sizeof(p->data) || (size_t)n < 3 + len)
		return -1;

	memcpy(p->data, resp + 3, len);

	return 0;
}

/* the chip takes any size and count, so only ever those it was read with */
static int param_write(const uint8_t *mac, uint16_t index, const struct param *p)
{
	uint8_t req[5 + sizeof(p->data)];
	uint8_t resp[1];
	size_t len = p->size * p->count;

	req[0] = index & 0xff;
	req[1] = index >> 8;
	req[2] = p->size;
	req[3] = p->count & 0xff;
	req[4] = p->count >> 8;
	memcpy(req + 5, p->data, len);

	if (mme(mac, MME_PARAM_SET, req, 5 + len, resp, sizeof(resp)) < 1 || resp[0])
		return -1;

	return 0;
}

static int param_get(uint8_t index, uint8_t *val, size_t len)
{
	struct param p;

	if (param_read(soc_mac, index, &p) || p.size != 1 || p.count != len)
		return -1;

	memcpy(val, p.data, len);

	return 0;
}

static int cmd_info(void)
{
	uint8_t req[1 + NID_LEN] = { 1 };
	uint8_t resp[1 + 64 * 10];
	char str[65];
	int n, i;

	if (identify(soc_mac, str, sizeof(str)))
		return 1;

	printf("chip ");
	print_mac(soc_mac);
	printf(" %s\n", str);

	if (param_get(PARAM_NID, req + 1, NID_LEN))
		return 1;

	/* answer: number of stations, then address and two rates for each */
	n = mme(soc_mac, MME_STATIONS, req, sizeof(req), resp, sizeof(resp));
	if (n < 1 || n < 1 + resp[0] * 10)
		return 1;

	for (i = 0; i < resp[0]; i++) {
		const uint8_t *sta = resp + 1 + i * 10;

		printf("station ");
		print_mac(sta);
		printf(" tx %u rx %u Mbit/s", (sta[6] | sta[7] << 8) & 0x7ff,
		       (sta[8] | sta[9] << 8) & 0x7ff);

		if (!identify(sta, str, sizeof(str)))
			printf(" %s", str);

		printf("\n");
	}

	return 0;
}

static int key_set(const uint8_t *nmk)
{
	uint8_t req[NMK_LEN + 2] = {};
	uint8_t resp[1];
	int64_t end;

	memcpy(req, nmk, NMK_LEN);

	if (mme(soc_mac, MME_KEY_SET, req, sizeof(req), resp, sizeof(resp)) < 1 || resp[0])
		return 1;

	/* the chip joins the network of the new key, and answers again after that */
	quiet = true;
	for (end = now_ms() + KEY_WAIT * 1000; now_ms() < end; sleep(1))
		if (pair_cmd(PAIR_STATUS) >= 0)
			break;
	quiet = false;

	return 0;
}

static int cmd_key(const char *str)
{
	uint8_t nmk[NMK_LEN];
	unsigned int b;
	int i;

	if (!str) {
		if (param_get(PARAM_NMK, nmk, NMK_LEN))
			return 1;

		for (i = 0; i < NMK_LEN; i++)
			printf("%02x", nmk[i]);
		printf("\n");

		return 0;
	}

	if (strlen(str) != 2 * NMK_LEN)
		return -1;

	for (i = 0; i < NMK_LEN; i++) {
		if (sscanf(str + 2 * i, "%2x", &b) != 1)
			return -1;

		nmk[i] = b;
	}

	return key_set(nmk);
}

/* a network of its own, with a random key that no other adapter has */
static int cmd_leave(void)
{
	uint8_t nmk[NMK_LEN];

	if (getrandom(nmk, sizeof(nmk), 0) != sizeof(nmk)) {
		perror("getrandom");
		return 1;
	}

	if (key_set(nmk))
		return 1;

	printf("left the powerline network\n");

	return 0;
}

static const uint32_t sha256_k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

#define ROR(x, n)		((x) >> (n) | (x) << (32 - (n)))

static void sha256_block(uint32_t *s, const uint8_t *p)
{
	uint32_t w[64], v[8], t1, t2;
	int i;

	for (i = 0; i < 16; i++)
		w[i] = (uint32_t)p[4 * i] << 24 | p[4 * i + 1] << 16 | p[4 * i + 2] << 8 | p[4 * i + 3];
	for (; i < 64; i++)
		w[i] = w[i - 16] + (ROR(w[i - 15], 7) ^ ROR(w[i - 15], 18) ^ w[i - 15] >> 3) +
		       w[i - 7] + (ROR(w[i - 2], 17) ^ ROR(w[i - 2], 19) ^ w[i - 2] >> 10);

	memcpy(v, s, sizeof(v));

	for (i = 0; i < 64; i++) {
		t1 = v[7] + (ROR(v[4], 6) ^ ROR(v[4], 11) ^ ROR(v[4], 25)) +
		     ((v[4] & v[5]) ^ (~v[4] & v[6])) + sha256_k[i] + w[i];
		t2 = (ROR(v[0], 2) ^ ROR(v[0], 13) ^ ROR(v[0], 22)) +
		     ((v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]));
		memmove(v + 1, v, 7 * sizeof(v[0]));
		v[4] += t1;
		v[0] = t1 + t2;
	}

	for (i = 0; i < 8; i++)
		s[i] += v[i];
}

/* out may be msg */
static void sha256(const uint8_t *msg, size_t len, uint8_t *out)
{
	uint32_t s[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
	};
	uint64_t bits = (uint64_t)len * 8;
	uint8_t block[64] = {};
	size_t i;

	for (; len >= sizeof(block); len -= sizeof(block), msg += sizeof(block))
		sha256_block(s, msg);

	memcpy(block, msg, len);
	block[len] = 0x80;
	if (len >= 56) {
		sha256_block(s, block);
		memset(block, 0, sizeof(block));
	}

	for (i = 0; i < 8; i++)
		block[63 - i] = bits >> (8 * i);
	sha256_block(s, block);

	for (i = 0; i < 32; i++)
		out[i] = s[i / 4] >> (24 - 8 * (i % 4));
}

/*
 * The key of the network that adapters set to the same network name share:
 * HomePlug AV derives it from a network password with PBKDF1 and SHA-256.
 */
static int cmd_name(const char *name)
{
	static const uint8_t salt[] = { 0x08, 0x85, 0x6d, 0xaf, 0x7c, 0xf5, 0x81, 0x86 };
	uint8_t buf[64 + sizeof(salt)], hash[32];
	size_t len = strlen(name);
	int i;

	if (!len || len > 64)
		return -1;

	memcpy(buf, name, len);
	memcpy(buf + len, salt, sizeof(salt));
	sha256(buf, len + sizeof(salt), hash);
	for (i = 1; i < 1000; i++)
		sha256(hash, sizeof(hash), hash);

	return key_set(hash);
}

/* the Ethernet socket, which is on the chip */
static int cmd_eth(void)
{
	static const uint8_t req[] = { 4, 1, 2, 0, 0, 0 };
	uint8_t resp[16];

	/* the answer names the port it is about */
	if (mme(soc_mac, MME_ETH, req, sizeof(req), resp, sizeof(resp)) < 6 || resp[5] != req[2])
		return 1;

	printf("%s\n", resp[4] & 0x04 ? "no link" : "link");

	return 0;
}

static int chip_restart(const uint8_t *mac)
{
	uint8_t resp[1];

	return mme(mac, MME_RESTART, NULL, 0, resp, sizeof(resp)) < 0;
}

static int cmd_restart(void)
{
	return chip_restart(soc_mac);
}

static bool led_on(const struct param *a, const struct param *b, const struct param *c)
{
	return (b->data[3] & 0x70) || (!c->data[0] && !(a->data[2] & 0x40));
}

/* the LEDs of this chip or of another adapter in the network, as the stock firmware sets them */
static int cmd_led(const uint8_t *mac, const char *state)
{
	struct param a, b, c;
	bool on;

	if (state && strcmp(state, "on") && strcmp(state, "off"))
		return -1;

	if (param_read(mac, PARAM_LED_A, &a) || param_read(mac, PARAM_LED_B, &b) ||
	    param_read(mac, PARAM_LED_C, &c))
		return 1;

	if (a.size * a.count < 3 || b.size * b.count < 4 || c.size * c.count < 1) {
		fprintf(stderr, "the LED settings are not as expected\n");
		return 1;
	}

	if (!state) {
		printf("%s\n", led_on(&a, &b, &c) ? "on" : "off");
		return 0;
	}

	on = !strcmp(state, "on");
	if (led_on(&a, &b, &c) == on)
		return 0;

	if (on) {
		a.data[2] &= ~0x40;
		b.data[3] = (b.data[3] & ~0x70) | 0x10;
		c.data[0] = 0;
	} else {
		b.data[3] &= ~0x70;
		c.data[0] |= 0x47;
	}

	if (param_write(mac, PARAM_LED_A, &a) || param_write(mac, PARAM_LED_B, &b) ||
	    param_write(mac, PARAM_LED_C, &c)) {
		fprintf(stderr, "the LED settings were not taken\n");
		return 1;
	}

	/* the chip applies them when it starts */
	return chip_restart(mac);
}

static int usage(const char *prog)
{
	fprintf(stderr,
		"Usage: %s [options] <command>\n"
		"\n"
		"Options:\n"
		"  -i <ifname>   interface that leads to the chip (default: eth0)\n"
		"  -m <mac>      address of the chip (default: interface address + 2)\n"
		"\n"
		"Commands:\n"
		"  pair          start push-button pairing and wait for its end\n"
		"  cancel        end a pairing that is under way\n"
		"  status        state of the pairing\n"
		"  info          the chip and the stations of its network\n"
		"  key [<hex>]   show the network key, or set it (32 hex digits)\n"
		"  name <name>   set the network key from a network name, which other\n"
		"                adapters set to the same name share\n"
		"  leave         leave the powerline network for one of its own\n"
		"  led [<mac>] [on|off]\n"
		"                show the LEDs of the chip or of another adapter, or\n"
		"                switch them, which restarts that adapter\n"
		"  eth           the link of the Ethernet socket, which is on the chip\n"
		"  restart       restart the chip, it loads its firmware again\n",
		prog);

	return 1;
}

int main(int argc, char **argv)
{
	bool soc_mac_set = false;
	uint8_t led_mac[ETH_ALEN];
	const char *cmd;
	int ch, args;

	while ((ch = getopt(argc, argv, "i:m:")) != -1) {
		switch (ch) {
		case 'i':
			ifname = optarg;
			break;
		case 'm':
			if (mac_parse(optarg, soc_mac))
				return usage(argv[0]);
			soc_mac_set = true;
			break;
		default:
			return usage(argv[0]);
		}
	}

	if (optind >= argc)
		return usage(argv[0]);

	cmd = argv[optind++];

	/* the arguments a command takes at most */
	args = !strcmp(cmd, "led") ? 2 : !strcmp(cmd, "key") || !strcmp(cmd, "name");
	if (argc - optind > args || (!strcmp(cmd, "name") && argc - optind != 1))
		return usage(argv[0]);

	if (sock_setup())
		return 1;

	if (!soc_mac_set) {
		memcpy(soc_mac, if_mac, ETH_ALEN);
		mac_add(soc_mac, 2);
	}

	/*
	 * The answers carry the number of their request, a byte that each
	 * request counts on. Other programs on this host count in the same
	 * byte, from a random start so that their counts do not run into
	 * each other.
	 */
	if (getrandom(&req_num, sizeof(req_num), 0) != sizeof(req_num))
		req_num = getpid();

	if (!strcmp(cmd, "pair"))
		return cmd_pair();
	if (!strcmp(cmd, "cancel"))
		return cmd_cancel();
	if (!strcmp(cmd, "status"))
		return cmd_status();
	if (!strcmp(cmd, "info"))
		return cmd_info();
	if (!strcmp(cmd, "restart"))
		return cmd_restart();
	if (!strcmp(cmd, "leave"))
		return cmd_leave();
	if (!strcmp(cmd, "eth"))
		return cmd_eth();

	ch = -1;
	if (!strcmp(cmd, "key"))
		ch = cmd_key(argv[optind]);
	if (!strcmp(cmd, "name"))
		ch = cmd_name(argv[optind]);

	if (!strcmp(cmd, "led")) {
		memcpy(led_mac, soc_mac, ETH_ALEN);
		if (optind < argc && strchr(argv[optind], ':') && mac_parse(argv[optind++], led_mac))
			return usage(argv[0]);
		if (argc - optind <= 1)
			ch = cmd_led(led_mac, argv[optind]);
	}

	if (ch >= 0)
		return ch;

	return usage(argv[0]);
}
