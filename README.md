# openwrt-traffic-classifier

Edge AI network traffic classification daemon for OpenWRT/OpenWiFi access points.

Classifies what each Wi-Fi client is doing in real time — video streaming, gaming, social media, web browsing, VoIP, downloads — and maps it to the client's MAC address and SSID. Runs entirely on the AP with no cloud dependency.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        OpenWRT Access Point                        │
│                                                                     │
│  ┌──────────┐    ┌────────────┐    ┌─────────────┐    ┌──────────┐ │
│  │  br-lan   │───>│  Capture   │───>│  Flow Table │───>│Classifier│ │
│  │ (pcap)    │    │  Engine    │    │  (5-tuple   │    │(heuristic│ │
│  │           │    │            │    │   + stats)  │    │  + ML)   │ │
│  └──────────┘    └────────────┘    └─────────────┘    └────┬─────┘ │
│                                                            │       │
│  ┌──────────────┐    ┌─────────────────────────────────────┘       │
│  │ STA Tracker   │    │                                             │
│  │ (hostapd ubus)│    │                                             │
│  │ MAC → SSID    │<───┘                                             │
│  └──────┬───────┘                                                   │
│         │           ┌──────────────────┐                            │
│         └──────────>│   ubus API       │                            │
│                     │  get_flows       │                            │
│                     │  get_clients     │                            │
│                     │  get_stats       │                            │
│                     │  status          │                            │
│                     └──────────────────┘                            │
└─────────────────────────────────────────────────────────────────────┘
```

## How It Works

1. **Packet Capture** — Listens on `br-lan` via libpcap in non-blocking mode, integrated into the uloop event loop. Only reads L3/L4 headers (128-byte snaplen), never inspects payload.

2. **Flow Aggregation** — Each unique 5-tuple (srcIP, dstIP, srcPort, dstPort, protocol) becomes a flow entry with accumulated statistics: packet counts, byte counts, inter-arrival times, packet size distributions, TCP flags — split by direction.

3. **Feature Extraction** — Every 5 seconds, each flow is converted into a 20-dimensional feature vector:

   | # | Feature | # | Feature |
   |---|---------|---|---------|
   | 0 | flow_duration_sec | 10 | min_packet_size |
   | 1 | total_fwd_packets | 11 | max_packet_size |
   | 2 | total_bwd_packets | 12 | packets_per_second |
   | 3 | total_fwd_bytes | 13 | bytes_per_second |
   | 4 | total_bwd_bytes | 14 | fwd_pkt_ratio |
   | 5 | fwd_bwd_bytes_ratio | 15 | avg_fwd_pkt_size |
   | 6 | avg_packet_size | 16 | avg_bwd_pkt_size |
   | 7 | std_packet_size | 17 | tcp_flags_or |
   | 8 | avg_iat_usec | 18 | syn_count |
   | 9 | std_iat_usec | 19 | protocol |

4. **Classification** — Heuristic engine now, XGBoost model (compiled to C via treelite) in Phase 2.

5. **STA Tracking** — Queries `hostapd.<iface>` via ubus to map MAC addresses to SSIDs.

6. **ubus API** — Exposes `get_flows`, `get_clients`, `get_stats`, `status`.

## Traffic Classes

| Class | Description | Example Apps |
|-------|-------------|--------------|
| `video` | High-bandwidth streaming, large packets, asymmetric | YouTube, Netflix, Hotstar |
| `gaming` | Low-latency, small UDP packets, bidirectional | BGMI, Free Fire, Genshin |
| `social` | Moderate HTTPS, mixed packet sizes | Instagram, WhatsApp, X |
| `browsing` | HTTP/HTTPS, bursty request-response | Chrome, Safari, news apps |
| `download` | Sustained high throughput, extremely asymmetric | APK downloads, OTA updates |
| `voip` | Small UDP packets at steady rate | JioMeet, Google Meet |
| `other` | Unclassified | Background sync, IoT |

## Building

### Integration with OpenWRT

```bash
# Symlink into your OpenWRT tree
ln -s $(pwd)/package/network/services/traffic-classifier \
      /path/to/openwrt/package/network/services/traffic-classifier

# Select and build
cd /path/to/openwrt
make menuconfig   # Network → traffic-classifier → <*>
make package/traffic-classifier/compile V=s
```

### Install on Device

```bash
scp bin/packages/aarch64_cortex-a53/base/traffic-classifier_*.ipk root@192.168.1.1:/tmp/
ssh root@192.168.1.1 'opkg install /tmp/traffic-classifier_*.ipk'
```

## Configuration

UCI config: `/etc/config/traffic-classifier`

```
config daemon
    option interface 'br-lan'
    option model '/etc/traffic-classifier/model.json'
    option max_flows '8192'
```

## ubus API Examples

```bash
# Daemon health
ubus call traffic-classifier status

# All flows with classification
ubus call traffic-classifier get_flows

# Per-client app usage breakdown
ubus call traffic-classifier get_clients

# Aggregate stats
ubus call traffic-classifier get_stats
```

### Sample `get_clients` output

```json
{
    "clients": [
        {
            "mac": "a4:b1:c1:23:45:67",
            "ssid": "MyHomeWiFi",
            "total_bytes": 52431200,
            "total_flows": 18,
            "app_usage": {
                "video": 3,
                "browsing": 8,
                "social": 5,
                "gaming": 2
            }
        }
    ]
}
```

## Project Structure

```
package/network/services/traffic-classifier/
├── Makefile                          # OpenWRT package definition
├── files/
│   ├── traffic-classifier.init       # procd init script
│   ├── traffic-classifier.conf       # UCI default config
│   └── model_stub.json               # Placeholder model definition
└── src/
    ├── main.c                        # Daemon entry, uloop event loop
    ├── capture.c / capture.h         # libpcap packet capture + L3/L4 parsing
    ├── flow_table.c / flow_table.h   # Hash table of flows + stats
    ├── features.c / features.h       # Flow → 20-feature vector
    ├── classifier.c / classifier.h   # Heuristic + ML classification
    ├── sta_tracker.c / sta_tracker.h  # hostapd ubus → MAC/SSID mapping
    └── ubus_api.c / ubus_api.h       # ubus RPC interface
```

## Roadmap

- **Phase 2** — Train XGBoost on MIRAGE-2019 dataset, export to C via treelite
- **Phase 3** — LuCI dashboard for per-client traffic visualization
- **Phase 4** — QoS integration with nftables/tc/qosify
- **Phase 5** — uCentral telemetry push

## Target Hardware

- Qualcomm IPQ60xx (Cortex-A53, aarch64) — primary target
- Any OpenWRT device with 64MB+ RAM

## License

GPL-2.0
