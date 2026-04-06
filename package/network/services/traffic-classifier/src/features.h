#ifndef FEATURES_H
#define FEATURES_H

#include "flow_table.h"

#define NUM_FEATURES 20

/*
 * Feature vector layout:
 *  0: flow_duration_sec       10: min_packet_size
 *  1: total_fwd_packets       11: max_packet_size
 *  2: total_bwd_packets       12: packets_per_second
 *  3: total_fwd_bytes         13: bytes_per_second
 *  4: total_bwd_bytes         14: fwd_pkt_ratio
 *  5: fwd_bwd_bytes_ratio     15: avg_fwd_pkt_size
 *  6: avg_packet_size         16: avg_bwd_pkt_size
 *  7: std_packet_size         17: tcp_flags_or
 *  8: avg_iat_usec            18: syn_count
 *  9: std_iat_usec            19: protocol
 */

void features_extract(const struct flow_entry *entry, float features[NUM_FEATURES]);

#endif
