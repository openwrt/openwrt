#include "capture.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap/pcap.h>

#define SNAP_LEN        128
#define PCAP_TIMEOUT_MS 100

struct capture_ctx {
	pcap_t *pcap;
	struct flow_table *ft;
	char ifname[32];
	int fd;
};

static int64_t usec_now(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

struct capture_ctx *capture_init(const char *ifname, struct flow_table *ft)
{
	char errbuf[PCAP_ERRBUF_SIZE];

	struct capture_ctx *ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		return NULL;

	ctx->ft = ft;
	snprintf(ctx->ifname, sizeof(ctx->ifname), "%s", ifname);

	ctx->pcap = pcap_open_live(ifname, SNAP_LEN, 1, PCAP_TIMEOUT_MS, errbuf);
	if (!ctx->pcap) {
		syslog(LOG_ERR, "pcap_open_live(%s): %s", ifname, errbuf);
		free(ctx);
		return NULL;
	}

	if (pcap_datalink(ctx->pcap) != DLT_EN10MB) {
		syslog(LOG_ERR, "unsupported datalink on %s", ifname);
		pcap_close(ctx->pcap);
		free(ctx);
		return NULL;
	}

	ctx->fd = pcap_get_selectable_fd(ctx->pcap);
	if (ctx->fd < 0) {
		syslog(LOG_ERR, "pcap_get_selectable_fd failed on %s", ifname);
		pcap_close(ctx->pcap);
		free(ctx);
		return NULL;
	}

	pcap_setnonblock(ctx->pcap, 1, errbuf);

	syslog(LOG_INFO, "capture: listening on %s (snaplen=%d)", ifname, SNAP_LEN);
	return ctx;
}

void capture_destroy(struct capture_ctx *ctx)
{
	if (!ctx)
		return;
	if (ctx->pcap)
		pcap_close(ctx->pcap);
	free(ctx);
}

int capture_get_fd(struct capture_ctx *ctx)
{
	return ctx->fd;
}

static void process_packet(struct capture_ctx *ctx,
			   const uint8_t *pkt, uint32_t len,
			   int64_t ts_usec)
{
	struct flow_key key;
	uint8_t src_mac[6];
	const uint8_t *l3;
	uint16_t l3_len;
	uint16_t pkt_payload_len = 0;
	uint8_t tcp_flags = 0;

	memset(&key, 0, sizeof(key));

	if (len < sizeof(struct ether_header))
		return;

	const struct ether_header *eth = (const struct ether_header *)pkt;
	memcpy(src_mac, eth->ether_shost, 6);

	uint16_t etype = ntohs(eth->ether_type);
	l3 = pkt + sizeof(struct ether_header);
	l3_len = len - sizeof(struct ether_header);

	/* 802.1Q VLAN tag */
	if (etype == 0x8100 && l3_len >= 4) {
		etype = ntohs(*(uint16_t *)(l3 + 2));
		l3 += 4;
		l3_len -= 4;
	}

	if (etype == ETHERTYPE_IP) {
		if (l3_len < sizeof(struct iphdr))
			return;

		const struct iphdr *iph = (const struct iphdr *)l3;
		uint16_t iph_len = iph->ihl * 4;
		if (iph_len < 20 || l3_len < iph_len)
			return;

		key.is_ipv6 = 0;
		memcpy(&key.src_ip.s6_addr[12], &iph->saddr, 4);
		memcpy(&key.dst_ip.s6_addr[12], &iph->daddr, 4);
		key.src_ip.s6_addr[10] = key.src_ip.s6_addr[11] = 0xff;
		key.dst_ip.s6_addr[10] = key.dst_ip.s6_addr[11] = 0xff;
		key.proto = iph->protocol;
		pkt_payload_len = ntohs(iph->tot_len);

		const uint8_t *l4 = l3 + iph_len;
		uint16_t l4_len = l3_len - iph_len;

		if (key.proto == IPPROTO_TCP && l4_len >= sizeof(struct tcphdr)) {
			const struct tcphdr *th = (const struct tcphdr *)l4;
			key.src_port = ntohs(th->source);
			key.dst_port = ntohs(th->dest);
			tcp_flags = ((uint8_t *)th)[13];
		} else if (key.proto == IPPROTO_UDP && l4_len >= sizeof(struct udphdr)) {
			const struct udphdr *uh = (const struct udphdr *)l4;
			key.src_port = ntohs(uh->source);
			key.dst_port = ntohs(uh->dest);
		}

	} else if (etype == ETHERTYPE_IPV6) {
		if (l3_len < sizeof(struct ip6_hdr))
			return;

		const struct ip6_hdr *ip6 = (const struct ip6_hdr *)l3;

		key.is_ipv6 = 1;
		memcpy(&key.src_ip, &ip6->ip6_src, 16);
		memcpy(&key.dst_ip, &ip6->ip6_dst, 16);
		key.proto = ip6->ip6_nxt;
		pkt_payload_len = ntohs(ip6->ip6_plen) + sizeof(struct ip6_hdr);

		const uint8_t *l4 = l3 + sizeof(struct ip6_hdr);
		uint16_t l4_len = l3_len - sizeof(struct ip6_hdr);

		if (key.proto == IPPROTO_TCP && l4_len >= sizeof(struct tcphdr)) {
			const struct tcphdr *th = (const struct tcphdr *)l4;
			key.src_port = ntohs(th->source);
			key.dst_port = ntohs(th->dest);
			tcp_flags = ((uint8_t *)th)[13];
		} else if (key.proto == IPPROTO_UDP && l4_len >= sizeof(struct udphdr)) {
			const struct udphdr *uh = (const struct udphdr *)l4;
			key.src_port = ntohs(uh->source);
			key.dst_port = ntohs(uh->dest);
		}
	} else {
		return;
	}

	struct flow_entry *e = flow_table_lookup(ctx->ft, &key);
	if (!e) {
		e = flow_table_insert(ctx->ft, &key, src_mac);
		if (!e)
			return;
		e->stats.first_pkt_usec = ts_usec;
	}

	struct flow_stats *s = &e->stats;
	e->last_seen = time(NULL);

	bool is_forward = (memcmp(src_mac, e->src_mac, 6) == 0);

	if (is_forward) {
		s->total_pkts_fwd++;
		s->total_bytes_fwd += pkt_payload_len;
	} else {
		s->total_pkts_bwd++;
		s->total_bytes_bwd += pkt_payload_len;
	}

	if (s->pkt_size_count < MAX_PKT_SIZES) {
		s->pkt_sizes[s->pkt_size_count] = pkt_payload_len;
		s->pkt_dirs[s->pkt_size_count] = is_forward ? 1 : 0;
		s->pkt_size_count++;
	}

	s->pkt_size_sum += pkt_payload_len;
	s->pkt_size_sum_sq += (double)pkt_payload_len * pkt_payload_len;

	if (s->prev_pkt_usec > 0) {
		double iat = (double)(ts_usec - s->prev_pkt_usec);
		if (iat > 0) {
			s->iat_sum += iat;
			s->iat_sum_sq += iat * iat;
			s->iat_count++;
		}
	}
	s->prev_pkt_usec = ts_usec;
	s->last_pkt_usec = ts_usec;

	if (tcp_flags) {
		s->tcp_flags_or |= tcp_flags;
		if (tcp_flags & 0x02) s->tcp_syn_count++;
		if (tcp_flags & 0x01) s->tcp_fin_count++;
		if (tcp_flags & 0x04) s->tcp_rst_count++;
	}
}

int capture_process(struct capture_ctx *ctx)
{
	struct pcap_pkthdr *hdr;
	const uint8_t *data;
	int count = 0;
	int ret;

	while ((ret = pcap_next_ex(ctx->pcap, &hdr, &data)) > 0) {
		int64_t ts = (int64_t)hdr->ts.tv_sec * 1000000 + hdr->ts.tv_usec;
		process_packet(ctx, data, hdr->caplen, ts);
		count++;

		if (count >= 64)
			break;
	}

	return count;
}
