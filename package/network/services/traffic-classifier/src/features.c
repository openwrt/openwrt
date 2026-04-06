#include "features.h"
#include <math.h>
#include <string.h>

void features_extract(const struct flow_entry *entry, float features[NUM_FEATURES])
{
	const struct flow_stats *s = &entry->stats;
	memset(features, 0, sizeof(float) * NUM_FEATURES);

	uint32_t total_pkts = s->total_pkts_fwd + s->total_pkts_bwd;
	uint64_t total_bytes = s->total_bytes_fwd + s->total_bytes_bwd;

	double duration_usec = 0;
	if (s->last_pkt_usec > s->first_pkt_usec)
		duration_usec = (double)(s->last_pkt_usec - s->first_pkt_usec);

	double duration_sec = duration_usec / 1e6;
	if (duration_sec < 0.001)
		duration_sec = 0.001;

	features[0] = (float)duration_sec;
	features[1] = (float)s->total_pkts_fwd;
	features[2] = (float)s->total_pkts_bwd;
	features[3] = (float)s->total_bytes_fwd;
	features[4] = (float)s->total_bytes_bwd;

	if (s->total_bytes_bwd > 0)
		features[5] = (float)s->total_bytes_fwd / (float)s->total_bytes_bwd;
	else
		features[5] = (float)s->total_bytes_fwd;

	if (total_pkts > 0)
		features[6] = (float)s->pkt_size_sum / total_pkts;

	if (total_pkts > 1) {
		double mean = s->pkt_size_sum / total_pkts;
		double variance = (s->pkt_size_sum_sq / total_pkts) - (mean * mean);
		if (variance > 0)
			features[7] = (float)sqrt(variance);
	}

	if (s->iat_count > 0)
		features[8] = (float)(s->iat_sum / s->iat_count);

	if (s->iat_count > 1) {
		double mean = s->iat_sum / s->iat_count;
		double variance = (s->iat_sum_sq / s->iat_count) - (mean * mean);
		if (variance > 0)
			features[9] = (float)sqrt(variance);
	}

	uint16_t min_pkt = UINT16_MAX, max_pkt = 0;
	uint32_t n = s->pkt_size_count;
	if (n > MAX_PKT_SIZES)
		n = MAX_PKT_SIZES;

	for (uint32_t i = 0; i < n; i++) {
		if (s->pkt_sizes[i] < min_pkt)
			min_pkt = s->pkt_sizes[i];
		if (s->pkt_sizes[i] > max_pkt)
			max_pkt = s->pkt_sizes[i];
	}
	features[10] = (n > 0) ? (float)min_pkt : 0;
	features[11] = (float)max_pkt;

	features[12] = (float)(total_pkts / duration_sec);
	features[13] = (float)(total_bytes / duration_sec);

	if (total_pkts > 0)
		features[14] = (float)s->total_pkts_fwd / total_pkts;

	if (s->total_pkts_fwd > 0)
		features[15] = (float)s->total_bytes_fwd / s->total_pkts_fwd;
	if (s->total_pkts_bwd > 0)
		features[16] = (float)s->total_bytes_bwd / s->total_pkts_bwd;

	features[17] = (float)s->tcp_flags_or;
	features[18] = (float)s->tcp_syn_count;
	features[19] = (float)entry->key.proto;
}
