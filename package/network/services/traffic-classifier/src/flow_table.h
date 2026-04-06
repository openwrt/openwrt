#ifndef FLOW_TABLE_H
#define FLOW_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <time.h>

#define FLOW_TABLE_SIZE       4096
#define MAX_FLOW_PACKETS      1000
#define FLOW_TIMEOUT_SEC      30
#define MAX_PKT_SIZES         64
#define CLASSIFICATION_LABELS 8

enum flow_proto {
	FLOW_PROTO_TCP = 6,
	FLOW_PROTO_UDP = 17,
	FLOW_PROTO_OTHER = 0,
};

enum traffic_class {
	CLASS_UNKNOWN = 0,
	CLASS_VIDEO,
	CLASS_GAMING,
	CLASS_SOCIAL,
	CLASS_BROWSING,
	CLASS_DOWNLOAD,
	CLASS_VOIP,
	CLASS_OTHER,
};

static const char *traffic_class_names[] = {
	[CLASS_UNKNOWN]  = "unknown",
	[CLASS_VIDEO]    = "video",
	[CLASS_GAMING]   = "gaming",
	[CLASS_SOCIAL]   = "social",
	[CLASS_BROWSING] = "browsing",
	[CLASS_DOWNLOAD] = "download",
	[CLASS_VOIP]     = "voip",
	[CLASS_OTHER]    = "other",
};

struct flow_key {
	struct in6_addr src_ip;
	struct in6_addr dst_ip;
	uint16_t src_port;
	uint16_t dst_port;
	uint8_t proto;
	uint8_t is_ipv6;
};

/*
 * Per-flow statistics used as ML feature inputs.
 * All timing is in microseconds for precision on fast links.
 */
struct flow_stats {
	uint64_t total_bytes_fwd;
	uint64_t total_bytes_bwd;
	uint32_t total_pkts_fwd;
	uint32_t total_pkts_bwd;

	uint16_t pkt_sizes[MAX_PKT_SIZES];
	uint8_t pkt_dirs[MAX_PKT_SIZES];
	uint32_t pkt_size_count;

	int64_t first_pkt_usec;
	int64_t last_pkt_usec;
	int64_t prev_pkt_usec;

	double iat_sum;
	double iat_sum_sq;
	uint32_t iat_count;

	double pkt_size_sum;
	double pkt_size_sum_sq;

	uint8_t tcp_flags_or;
	uint8_t tcp_syn_count;
	uint8_t tcp_fin_count;
	uint8_t tcp_rst_count;
};

struct flow_entry {
	struct flow_key key;
	struct flow_stats stats;

	enum traffic_class classification;
	float confidence;

	uint8_t src_mac[6];
	time_t created;
	time_t last_seen;

	struct flow_entry *hash_next;
	bool active;
};

struct flow_table {
	struct flow_entry *buckets[FLOW_TABLE_SIZE];
	uint32_t count;
	uint32_t max_entries;
};

struct flow_table *flow_table_create(uint32_t max_entries);
void flow_table_destroy(struct flow_table *ft);

struct flow_entry *flow_table_lookup(struct flow_table *ft,
				     const struct flow_key *key);
struct flow_entry *flow_table_insert(struct flow_table *ft,
				     const struct flow_key *key,
				     const uint8_t *src_mac);

void flow_table_expire(struct flow_table *ft, time_t now, int timeout_sec);
int flow_table_for_each(struct flow_table *ft,
			int (*cb)(struct flow_entry *entry, void *ctx),
			void *ctx);

#endif
