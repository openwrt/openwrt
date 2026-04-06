# Training Pipeline

ML training pipeline for the traffic-classifier. Trains an XGBoost model on network flow features and exports it to C code for on-device inference.

## Quick Start (synthetic data — test the pipeline)

```bash
pip install -r requirements.txt

# Generate synthetic training data
python generate_synthetic.py --output synthetic_features.csv --samples 10000

# Train the model
python train_model.py --input synthetic_features.csv --output model/ --cv 5

# Generate evaluation plots
python evaluate.py --model model/classifier.joblib --input synthetic_features.csv --output plots/

# Export to C code
python export_treelite.py --model model/classifier.xgb --output model_c/ --method manual
```

## Full Pipeline (with real data)

### Step 1: Get Training Data

**Option A — MIRAGE-2019 (recommended starting point)**

Download from https://traffic.comics.unina.it/mirage/

```bash
# After downloading and extracting:
python extract_features.py --input-dir ./MIRAGE-2019/ --output mirage_features.csv
```

The script auto-detects labels from directory names (YouTube → video, etc.)

**Option B — Your own PCAPs**

Capture traffic from real clients with known labels:

```bash
# On the AP, capture traffic while a client watches YouTube:
tcpdump -i br-lan -w /tmp/youtube_capture.pcap -c 10000

# Extract with explicit label:
python extract_features.py --input-dir ./my_captures/youtube/ --output youtube.csv --label video
python extract_features.py --input-dir ./my_captures/gaming/ --output gaming.csv --label gaming

# Merge CSVs:
head -1 youtube.csv > all_features.csv
tail -n +2 youtube.csv >> all_features.csv
tail -n +2 gaming.csv >> all_features.csv
```

**Option C — nDPI as ground truth**

Run nDPI on the AP alongside tcpdump, use nDPI labels to auto-tag flows.

### Step 2: Train

```bash
python train_model.py --input all_features.csv --output model/ --cv 5
```

Output:
- `model/classifier.xgb` — XGBoost binary format
- `model/classifier.json` — XGBoost JSON format
- `model/classifier.joblib` — scikit-learn compatible
- `model/model_meta.json` — metadata (features, classes, metrics)

### Step 3: Evaluate

```bash
python evaluate.py --model model/classifier.joblib --input all_features.csv --output plots/
```

Generates:
- `plots/confusion_matrix.png`
- `plots/feature_importance.png`
- `plots/class_distribution.png`
- `plots/per_class_metrics.png`

### Step 4: Export to C

```bash
python export_treelite.py --model model/classifier.xgb --output model_c/ --method manual
```

This generates `model_predict.c` and `model_predict.h` — pure C code with no dependencies.

### Step 5: Integrate

```bash
# Copy into the package source
cp model_c/model_predict.c model_c/model_predict.h \
   ../package/network/services/traffic-classifier/src/

# Update classifier.c to call model_predict() instead of classify_heuristic()
# Rebuild the package
```

## Feature Vector

The 20 features match exactly what the on-device C daemon computes:

| # | Feature | What it captures |
|---|---------|-----------------|
| 0 | flow_duration_sec | Session length |
| 1 | total_fwd_packets | Upload packet count |
| 2 | total_bwd_packets | Download packet count |
| 3 | total_fwd_bytes | Upload volume |
| 4 | total_bwd_bytes | Download volume |
| 5 | fwd_bwd_bytes_ratio | Asymmetry (streaming ≈ 0.01, gaming ≈ 1.0) |
| 6 | avg_packet_size | Distinguishes video (large) from gaming (small) |
| 7 | std_packet_size | Variability — steady (VoIP) vs bursty (browsing) |
| 8 | avg_iat_usec | Packet spacing — tight (gaming) vs loose (browsing) |
| 9 | std_iat_usec | Regularity of packet timing |
| 10 | min_packet_size | ACKs, keepalives |
| 11 | max_packet_size | MTU-sized data packets |
| 12 | packets_per_second | Flow intensity |
| 13 | bytes_per_second | Throughput |
| 14 | fwd_pkt_ratio | Direction balance |
| 15 | avg_fwd_pkt_size | Upload packet profile |
| 16 | avg_bwd_pkt_size | Download packet profile |
| 17 | tcp_flags_or | Connection behavior |
| 18 | syn_count | Connection setup pattern |
| 19 | protocol | TCP (6) vs UDP (17) |

## Expected Accuracy

| Data Source | Category-Level | App-Level |
|-------------|---------------|-----------|
| Synthetic only | ~95% (overfit to profiles) | N/A |
| MIRAGE-2019 | ~85-90% | ~70-75% |
| Your own data | Best real-world accuracy | Depends on labeling |
| Mixed (recommended) | ~85-90% | ~70-80% |
