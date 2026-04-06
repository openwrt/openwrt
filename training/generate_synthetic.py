#!/usr/bin/env python3
"""
Generate synthetic training data for testing the pipeline.

Creates realistic flow features for each traffic class based on known
statistical properties of different application types. Use this to
test the training pipeline before you have real PCAP data.

Usage:
    python generate_synthetic.py --output synthetic_features.csv --samples 5000
"""

import argparse
import csv
import random

import numpy as np

FEATURE_NAMES = [
    "flow_duration_sec", "total_fwd_packets", "total_bwd_packets",
    "total_fwd_bytes", "total_bwd_bytes", "fwd_bwd_bytes_ratio",
    "avg_packet_size", "std_packet_size", "avg_iat_usec",
    "std_iat_usec", "min_packet_size", "max_packet_size",
    "packets_per_second", "bytes_per_second", "fwd_pkt_ratio",
    "avg_fwd_pkt_size", "avg_bwd_pkt_size", "tcp_flags_or",
    "syn_count", "protocol",
]

CLASS_NAMES = [
    "unknown", "video", "gaming", "social",
    "browsing", "download", "voip", "other",
]


# Statistical profiles for each traffic class
# (mean, std) for key features, rest derived
PROFILES = {
    "video": {
        "duration": (30.0, 20.0),
        "avg_pkt_size": (1100.0, 200.0),
        "pkt_per_sec": (80.0, 30.0),
        "bytes_per_sec": (500000.0, 200000.0),
        "fwd_ratio": (0.08, 0.04),
        "proto": 6,
        "std_pkt_size": (400.0, 100.0),
        "avg_iat": (12000.0, 5000.0),
    },
    "gaming": {
        "duration": (120.0, 60.0),
        "avg_pkt_size": (120.0, 50.0),
        "pkt_per_sec": (60.0, 20.0),
        "bytes_per_sec": (15000.0, 8000.0),
        "fwd_ratio": (0.48, 0.08),
        "proto": 17,
        "std_pkt_size": (60.0, 30.0),
        "avg_iat": (16000.0, 5000.0),
    },
    "social": {
        "duration": (5.0, 3.0),
        "avg_pkt_size": (450.0, 150.0),
        "pkt_per_sec": (25.0, 15.0),
        "bytes_per_sec": (30000.0, 20000.0),
        "fwd_ratio": (0.35, 0.1),
        "proto": 6,
        "std_pkt_size": (350.0, 100.0),
        "avg_iat": (40000.0, 20000.0),
    },
    "browsing": {
        "duration": (3.0, 2.0),
        "avg_pkt_size": (600.0, 200.0),
        "pkt_per_sec": (30.0, 20.0),
        "bytes_per_sec": (50000.0, 30000.0),
        "fwd_ratio": (0.3, 0.1),
        "proto": 6,
        "std_pkt_size": (450.0, 150.0),
        "avg_iat": (30000.0, 15000.0),
    },
    "download": {
        "duration": (60.0, 40.0),
        "avg_pkt_size": (1350.0, 100.0),
        "pkt_per_sec": (500.0, 200.0),
        "bytes_per_sec": (2000000.0, 1000000.0),
        "fwd_ratio": (0.03, 0.02),
        "proto": 6,
        "std_pkt_size": (200.0, 100.0),
        "avg_iat": (2000.0, 1000.0),
    },
    "voip": {
        "duration": (180.0, 120.0),
        "avg_pkt_size": (180.0, 40.0),
        "pkt_per_sec": (50.0, 10.0),
        "bytes_per_sec": (12000.0, 4000.0),
        "fwd_ratio": (0.50, 0.05),
        "proto": 17,
        "std_pkt_size": (30.0, 15.0),
        "avg_iat": (20000.0, 2000.0),
    },
    "other": {
        "duration": (10.0, 8.0),
        "avg_pkt_size": (300.0, 200.0),
        "pkt_per_sec": (10.0, 8.0),
        "bytes_per_sec": (8000.0, 6000.0),
        "fwd_ratio": (0.4, 0.2),
        "proto": 6,
        "std_pkt_size": (250.0, 100.0),
        "avg_iat": (100000.0, 50000.0),
    },
}


def clamp(val, lo, hi):
    return max(lo, min(hi, val))


def sample_flow(class_name):
    p = PROFILES[class_name]

    duration = clamp(np.random.normal(*p["duration"]), 0.1, 600.0)
    avg_pkt = clamp(np.random.normal(*p["avg_pkt_size"]), 40.0, 1500.0)
    pps = clamp(np.random.normal(*p["pkt_per_sec"]), 1.0, 2000.0)
    bps = clamp(np.random.normal(*p["bytes_per_sec"]), 100.0, 1e7)
    fwd_ratio = clamp(np.random.normal(*p["fwd_ratio"]), 0.01, 0.99)

    total_pkts = max(10, int(pps * duration))
    fwd_pkts = max(1, int(total_pkts * fwd_ratio))
    bwd_pkts = max(1, total_pkts - fwd_pkts)

    total_bytes = max(1000, int(bps * duration))
    fwd_bytes = max(100, int(total_bytes * fwd_ratio))
    bwd_bytes = max(100, total_bytes - fwd_bytes)

    if bwd_bytes > 0:
        byte_ratio = fwd_bytes / bwd_bytes
    else:
        byte_ratio = float(fwd_bytes)

    std_pkt = clamp(np.random.normal(*p["std_pkt_size"]), 0.0, 800.0)
    avg_iat = clamp(np.random.normal(*p["avg_iat"]), 100.0, 500000.0)
    std_iat = clamp(avg_iat * np.random.uniform(0.3, 1.5), 100.0, 500000.0)

    min_pkt = max(40, int(avg_pkt - 2 * std_pkt))
    max_pkt = min(1500, int(avg_pkt + 2 * std_pkt))

    avg_fwd_pkt = fwd_bytes / fwd_pkts if fwd_pkts > 0 else 0
    avg_bwd_pkt = bwd_bytes / bwd_pkts if bwd_pkts > 0 else 0

    tcp_flags = 0x18 if p["proto"] == 6 else 0
    syn_count = 1 if p["proto"] == 6 else 0

    noise = np.random.normal(0, 0.05, 20)

    features = [
        duration,
        float(fwd_pkts),
        float(bwd_pkts),
        float(fwd_bytes),
        float(bwd_bytes),
        byte_ratio,
        avg_pkt,
        std_pkt,
        avg_iat,
        std_iat,
        float(min_pkt),
        float(max_pkt),
        pps,
        bps,
        fwd_ratio,
        avg_fwd_pkt,
        avg_bwd_pkt,
        float(tcp_flags),
        float(syn_count),
        float(p["proto"]),
    ]

    features = [max(0.0, f * (1 + noise[i])) for i, f in enumerate(features)]

    return features


def main():
    parser = argparse.ArgumentParser(description="Generate synthetic training data")
    parser.add_argument("--output", default="synthetic_features.csv")
    parser.add_argument("--samples", type=int, default=5000,
                        help="Total samples across all classes (default: 5000)")
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args()

    np.random.seed(args.seed)
    random.seed(args.seed)

    classes = [c for c in CLASS_NAMES if c != "unknown"]
    samples_per_class = args.samples // len(classes)

    label_map = {name: i for i, name in enumerate(CLASS_NAMES)}

    rows = []
    for class_name in classes:
        for _ in range(samples_per_class):
            features = sample_flow(class_name)
            rows.append(features + [class_name, label_map[class_name]])

    random.shuffle(rows)

    with open(args.output, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(FEATURE_NAMES + ["label", "label_id"])
        writer.writerows(rows)

    print(f"Generated {len(rows)} synthetic flows → {args.output}")
    print(f"  Classes: {', '.join(classes)}")
    print(f"  Samples per class: {samples_per_class}")


if __name__ == "__main__":
    main()
