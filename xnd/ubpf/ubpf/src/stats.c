/* SPDX-License-Identifier: GPL-2.0 */

#include "uxdp.h"

struct datarec {
	__u64 rx_packets;
};

#ifndef XDP_ACTION_MAX
#define XDP_ACTION_MAX (XDP_REDIRECT + 1)
#endif

#define NANOSEC_PER_SEC 1000000000 /* 10^9 */
static __u64 gettime(void)
{
	struct timespec t;
	int res;

	res = clock_gettime(CLOCK_MONOTONIC, &t);
	if (res < 0) {
		fprintf(stderr, "Error with gettimeofday! (%i)\n", res);
		exit(-1);
	}
	return (__u64) t.tv_sec * NANOSEC_PER_SEC + t.tv_nsec;
}

struct record {
	__u64 timestamp;
	struct datarec total; /* defined in common_kern_user.h */
};

struct stats_record {
	struct record stats[1]; /* Assignment#2: Hint */
};

static double calc_period(struct record *r, struct record *p)
{
	double period_ = 0;
	__u64 period = 0;

	period = r->timestamp - p->timestamp;
	if (period > 0)
		period_ = ((double) period / NANOSEC_PER_SEC);

	return period_;
}

static void stats_print(struct stats_record *stats_rec,
			struct stats_record *stats_prev)
{
	struct record *rec, *prev;
	double period;
	__u64 packets;
	double pps; /* packets per sec */

	/* Assignment#2: Print other XDP actions stats  */
	{
		char *fmt = "%-12s %'11lld pkts (%'10.0f pps)"
			//" %'11lld Kbytes (%'6.0f Mbits/s)"
			" period:%f\n";
		const char *action = "";
		rec  = &stats_rec->stats[0];
		prev = &stats_prev->stats[0];

		period = calc_period(rec, prev);
		if (period == 0)
		       return;

		packets = rec->total.rx_packets - prev->total.rx_packets;
		pps     = packets / period;

		printf(fmt, action, rec->total.rx_packets, pps, period);
	}
}

/* BPF_MAP_TYPE_ARRAY */
static void map_get_value_array(int fd, __u32 key, struct datarec *value)
{
	if ((bpf_map_lookup_elem(fd, &key, value)) != 0) {
		fprintf(stderr,
			"ERR: bpf_map_xdp_map_elem failed key:0x%X\n", key);
	}
}

static bool map_collect(int fd, __u32 map_type, __u32 key, struct record *rec)
{
	struct datarec value;

	/* Get time as close as possible to reading map contents */
	rec->timestamp = gettime();

	switch (map_type) {
	case BPF_MAP_TYPE_ARRAY:
		map_get_value_array(fd, key, &value);
		break;
	case BPF_MAP_TYPE_PERCPU_ARRAY:
		/* fall-through */
	default:
		fprintf(stderr, "ERR: Unknown map_type(%u) cannot handle\n",
			map_type);
		return false;
		break;
	}

	/* Assignment#1: Add byte counters */
	rec->total.rx_packets = value.rx_packets;
	return true;
}

static void stats_collect(int map_fd, __u32 map_type,
			  struct stats_record *stats_rec)
{
	/* Assignment#2: Collect other XDP actions stats  */
	__u32 key = XDP_PASS;

	map_collect(map_fd, map_type, key, &stats_rec->stats[0]);
}

static void stats_poll(int map_fd, __u32 map_type, int interval)
{
	struct stats_record prev, record = { 0 };

	/* Get initial reading quickly */
	stats_collect(map_fd, map_type, &record);
	usleep(1000000/4);

	while (1) {
		prev = record; /* struct copy */
		stats_collect(map_fd, map_type, &record);
		stats_print(&record, &prev);
		sleep(interval);
	}
}

int
main(int argc, char **argv)
{
	struct xdp_map xdp_map = {
		.prog = "xdp_stats1_func",
		.map = "xdp_stats_map",
		.map_want = {
			.key_size = sizeof(__u32),
			.value_size = sizeof(struct datarec),
			.max_entries = XDP_ACTION_MAX,
		},
	};
	int interval = 2;
	int ch;

	while ((ch = getopt(argc, argv, "d:f:p:")) != -1) {
		switch (ch) {
		case 'd':
			xdp_map.net = optarg;
			break;
		default:
			fprintf(stderr, "Invalid argument\n");
			exit(-1);
		}
	}
	if (!xdp_map.net) {
		fprintf(stderr, "invalid arguments\n");
		return -1;
	}

	if (map_lookup(&xdp_map)) {
		fprintf(stderr, "failed to xdp_map map\n");
		return -1;
	}

	stats_poll(xdp_map.map_fd, xdp_map.map_info.type, interval);
	return 0;
}
