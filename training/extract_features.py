#!/usr/bin/env python3
"""
Extract flow-level features from PCAP files.

Reads PCAPs (from MIRAGE-2019 or your own captures), aggregates packets
into flows by 5-tuple, and computes the same 20 statistical features
that the on-device C daemon uses.

Usage:
    python extract_features.py --input-dir ./pcaps/ --output features.csv
    python extract_features.py --input-dir ./pcaps/ --output features.csv --label video
"""

import argparse
import csv
import math
import os
import sys
from collections import defaultdict
from pathlib import Path

from scapy.all import PcapReader, IP, IPv6, TCP, UDP, Ether
from tqdm import tqdm


FEATURE_NAMES = [
    "flow_duration_sec",
    "total_fwd_packets",
    "total_bwd_packets",
    "total_fwd_bytes",
    "total_bwd_bytes",
    "fwd_bwd_bytes_ratio",
    "avg_packet_size",
    "std_packet_size",
    "avg_iat_usec",
    "std_iat_usec",
    "min_packet_size",
    "max_packet_size",
    "packets_per_second",
    "bytes_per_second",
    "fwd_pkt_ratio",
    "avg_fwd_pkt_size",
    "avg_bwd_pkt_size",
    "tcp_flags_or",
    "syn_count",
    "protocol",
]

LABEL_MAP = {
    "unknown": 0,
    "video": 1,
    "gaming": 2,
    "social": 3,
    "browsing": 4,
    "download": 5,
    "voip": 6,
    "other": 7,
}


class FlowAccumulator:
    """Mirrors the C flow_stats struct — accumulates per-flow statistics."""

    def __init__(self, src_addr):
        self.src_addr = src_addr
        self.total_bytes_fwd = 0
        self.total_bytes_bwd = 0
        self.total_pkts_fwd = 0
        self.total_pkts_bwd = 0

        self.pkt_sizes = []
        self.pkt_dirs = []

        self.first_pkt_usec = None
        self.last_pkt_usec = None
        self.prev_pkt_usec = None

        self.iat_values = []
        self.pkt_size_sum = 0.0
        self.pkt_size_sum_sq = 0.0

        self.tcp_flags_or = 0
        self.syn_count = 0
        self.fin_count = 0
        self.rst_count = 0

        self.proto = 0

    def add_packet(self, pkt_len, ts_usec, is_forward, tcp_flags=0, proto=0):
        if self.first_pkt_usec is None:
            self.first_pkt_usec = ts_usec
        self.last_pkt_usec = ts_usec

        if is_forward:
            self.total_pkts_fwd += 1
            self.total_bytes_fwd += pkt_len
        else:
            self.total_pkts_bwd += 1
            self.total_bytes_bwd += pkt_len

        self.pkt_sizes.append(pkt_len)
        self.pkt_dirs.append(1 if is_forward else 0)
        self.pkt_size_sum += pkt_len
        self.pkt_size_sum_sq += pkt_len * pkt_len

        if self.prev_pkt_usec is not None:
            iat = ts_usec - self.prev_pkt_usec
            if iat > 0:
                self.iat_values.append(iat)
        self.prev_pkt_usec = ts_usec

        if tcp_flags:
            self.tcp_flags_or |= tcp_flags
            if tcp_flags & 0x02:
                self.syn_count += 1
            if tcp_flags & 0x01:
                self.fin_count += 1
            if tcp_flags & 0x04:
                self.rst_count += 1

        if proto:
            self.proto = proto

    def extract_features(self):
        total_pkts = self.total_pkts_fwd + self.total_pkts_bwd
        total_bytes = self.total_bytes_fwd + self.total_bytes_bwd

        if total_pkts < 5:
            return None

        duration_usec = 0
        if self.last_pkt_usec and self.first_pkt_usec:
            duration_usec = self.last_pkt_usec - self.first_pkt_usec

        duration_sec = duration_usec / 1e6
        if duration_sec < 0.001:
            duration_sec = 0.001

        features = [0.0] * 20

        features[0] = duration_sec
        features[1] = float(self.total_pkts_fwd)
        features[2] = float(self.total_pkts_bwd)
        features[3] = float(self.total_bytes_fwd)
        features[4] = float(self.total_bytes_bwd)

        if self.total_bytes_bwd > 0:
            features[5] = self.total_bytes_fwd / self.total_bytes_bwd
        else:
            features[5] = float(self.total_bytes_fwd)

        features[6] = self.pkt_size_sum / total_pkts

        if total_pkts > 1:
            mean = self.pkt_size_sum / total_pkts
            variance = (self.pkt_size_sum_sq / total_pkts) - (mean * mean)
            if variance > 0:
                features[7] = math.sqrt(variance)

        if len(self.iat_values) > 0:
            features[8] = sum(self.iat_values) / len(self.iat_values)

        if len(self.iat_values) > 1:
            iat_mean = sum(self.iat_values) / len(self.iat_values)
            iat_sq_mean = sum(v * v for v in self.iat_values) / len(self.iat_values)
            iat_var = iat_sq_mean - iat_mean * iat_mean
            if iat_var > 0:
                features[9] = math.sqrt(iat_var)

        if self.pkt_sizes:
            features[10] = float(min(self.pkt_sizes))
            features[11] = float(max(self.pkt_sizes))

        features[12] = total_pkts / duration_sec
        features[13] = total_bytes / duration_sec

        features[14] = self.total_pkts_fwd / total_pkts

        if self.total_pkts_fwd > 0:
            features[15] = self.total_bytes_fwd / self.total_pkts_fwd
        if self.total_pkts_bwd > 0:
            features[16] = self.total_bytes_bwd / self.total_pkts_bwd

        features[17] = float(self.tcp_flags_or)
        features[18] = float(self.syn_count)
        features[19] = float(self.proto)

        return features


def make_flow_key(pkt):
    """Extract 5-tuple from a scapy packet. Returns (key, src_addr, proto, tcp_flags)."""
    src_ip = dst_ip = None
    src_port = dst_port = 0
    proto = 0
    tcp_flags = 0

    if IP in pkt:
        src_ip = pkt[IP].src
        dst_ip = pkt[IP].dst
        proto = pkt[IP].proto
    elif IPv6 in pkt:
        src_ip = pkt[IPv6].src
        dst_ip = pkt[IPv6].dst
        proto = pkt[IPv6].nh
    else:
        return None, None, 0, 0

    if TCP in pkt:
        src_port = pkt[TCP].sport
        dst_port = pkt[TCP].dport
        tcp_flags = int(pkt[TCP].flags)
    elif UDP in pkt:
        src_port = pkt[UDP].sport
        dst_port = pkt[UDP].dport

    key = (src_ip, dst_ip, src_port, dst_port, proto)
    return key, src_ip, proto, tcp_flags


def get_pkt_len(pkt):
    """Get IP-level packet length."""
    if IP in pkt:
        return pkt[IP].len
    elif IPv6 in pkt:
        return pkt[IPv6].plen + 40
    return len(pkt)


def process_pcap(pcap_path):
    """Read a PCAP file and return a dict of flow_key -> FlowAccumulator."""
    flows = {}

    try:
        reader = PcapReader(str(pcap_path))
    except Exception as e:
        print(f"  Warning: cannot open {pcap_path}: {e}", file=sys.stderr)
        return flows

    for pkt in reader:
        key, src_ip, proto, tcp_flags = make_flow_key(pkt)
        if key is None:
            continue

        ts_usec = int(float(pkt.time) * 1e6)
        pkt_len = get_pkt_len(pkt)

        reverse_key = (key[1], key[0], key[3], key[2], key[4])

        if key in flows:
            flow = flows[key]
            is_forward = (src_ip == flow.src_addr)
        elif reverse_key in flows:
            flow = flows[reverse_key]
            is_forward = False
        else:
            flow = FlowAccumulator(src_addr=src_ip)
            flows[key] = flow
            is_forward = True

        flow.add_packet(pkt_len, ts_usec, is_forward, tcp_flags, proto)

    reader.close()
    return flows


def infer_label_from_path(pcap_path):
    """
    Try to infer traffic label from the PCAP file path.
    MIRAGE-2019 uses directory names like: YouTube/, Netflix/, etc.
    """
    parts = Path(pcap_path).parts
    name_lower = Path(pcap_path).stem.lower()

    app_to_class = {
        "youtube": "video", "netflix": "video", "twitch": "video",
        "primevideo": "video", "hotstar": "video", "vimeo": "video",
        "spotify": "video", "dailymotion": "video",

        "pubg": "gaming", "fortnite": "gaming", "cod": "gaming",
        "bgmi": "gaming", "freefire": "gaming", "clash": "gaming",
        "roblox": "gaming", "minecraft": "gaming",

        "facebook": "social", "instagram": "social", "twitter": "social",
        "whatsapp": "social", "telegram": "social", "tiktok": "social",
        "snapchat": "social", "reddit": "social", "linkedin": "social",

        "chrome": "browsing", "firefox": "browsing", "safari": "browsing",
        "edge": "browsing", "opera": "browsing",

        "skype": "voip", "zoom": "voip", "meet": "voip",
        "teams": "voip", "discord": "voip", "viber": "voip",
    }

    for part in parts:
        part_lower = part.lower()
        for app_name, traffic_class in app_to_class.items():
            if app_name in part_lower:
                return traffic_class

    for app_name, traffic_class in app_to_class.items():
        if app_name in name_lower:
            return traffic_class

    return None


def process_directory(input_dir, label=None):
    """Process all PCAPs in a directory tree. Returns list of (features, label)."""
    results = []
    pcap_files = []

    for ext in ["*.pcap", "*.pcapng", "*.cap"]:
        pcap_files.extend(Path(input_dir).rglob(ext))

    if not pcap_files:
        print(f"No PCAP files found in {input_dir}", file=sys.stderr)
        return results

    print(f"Found {len(pcap_files)} PCAP files")

    for pcap_path in tqdm(pcap_files, desc="Processing PCAPs"):
        flow_label = label
        if flow_label is None:
            flow_label = infer_label_from_path(pcap_path)

        if flow_label is None:
            flow_label = "unknown"

        flows = process_pcap(pcap_path)

        for flow_key, accumulator in flows.items():
            features = accumulator.extract_features()
            if features is not None:
                results.append((features, flow_label))

    return results


def write_csv(results, output_path):
    """Write extracted features to CSV."""
    with open(output_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(FEATURE_NAMES + ["label", "label_id"])

        for features, label in results:
            label_id = LABEL_MAP.get(label, 0)
            writer.writerow(features + [label, label_id])

    print(f"Wrote {len(results)} flow records to {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Extract flow features from PCAP files"
    )
    parser.add_argument(
        "--input-dir", required=True,
        help="Directory containing PCAP files (searched recursively)"
    )
    parser.add_argument(
        "--output", default="features.csv",
        help="Output CSV file path (default: features.csv)"
    )
    parser.add_argument(
        "--label", default=None,
        help="Force a label for all flows (e.g., 'video'). "
             "If not set, label is inferred from directory/file names."
    )
    args = parser.parse_args()

    if not os.path.isdir(args.input_dir):
        print(f"Error: {args.input_dir} is not a directory", file=sys.stderr)
        sys.exit(1)

    results = process_directory(args.input_dir, args.label)

    if not results:
        print("No flows extracted. Check your PCAP files.", file=sys.stderr)
        sys.exit(1)

    write_csv(results, args.output)


if __name__ == "__main__":
    main()
