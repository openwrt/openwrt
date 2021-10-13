/* SPDX-License-Identifier: GPL-2.0 */

#include "uxdp.h"


#define SAMPLE_SIZE 64

static void print_bpf_output(void *ctx, int cpu, void *data, __u32 size)
{
	struct {
		__u16 cookie;
		__u16 pkt_len;
		__u8  pkt_data[SAMPLE_SIZE];
	} __attribute__((packed)) *e = data;
	int i;

	fprintf(stderr, "%s:%s[%d]\n", __FILE__, __func__, __LINE__);
	if (e->cookie != 0xdead) {
		printf("BUG cookie %x sized %d\n", e->cookie, size);
		return;
	}

	printf("Pkt len: %-5d bytes. Ethernet hdr: ", e->pkt_len);
	for (i = 0; i < 14 && i < e->pkt_len; i++)
	printf("%02x ", e->pkt_data[i]);
	printf("\n");
}

int
main(int argc, char **argv)
{
	struct xdp_map pkt_map = {
		.prog = "uxdp_prog",
		.map = "uxdp_pkts",
		.map_want = {
			.key_size = sizeof(__u32),
			.value_size = sizeof(__u32),
			.max_entries = 8,
		},
	};
	struct perf_buffer_opts pb_opts = {
		.sample_cb = print_bpf_output,
	};
	struct perf_buffer *pb;
	int ch;
	int err;

	while ((ch = getopt(argc, argv, "d:f:p:")) != -1) {
		switch (ch) {
		case 'd':
			pkt_map.net = optarg;
			break;
		default:
			fprintf(stderr, "Invalid argument\n");
			exit(-1);
		}
	}
	if (!pkt_map.net) {
		fprintf(stderr, "invalid arguments\n");
		return -1;
	}

	if (map_lookup(&pkt_map)) {
		fprintf(stderr, "failed to xdp_map map\n");
		return -1;
	}

	pb = perf_buffer__new(pkt_map.map_fd, 8, &pb_opts);
	err = libbpf_get_error(pb);
	if (err) {
		perror("perf_buffer setup failed");
		return 1;
	}

	while (perf_buffer__poll(pb, 1000) >= 0) {
	}

	return 0;
}
