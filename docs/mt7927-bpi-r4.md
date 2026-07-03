# Banana Pi R4 + MT7927 — Build & Usage Notes

## Overview

This document covers building OpenWrt for the **Banana Pi R4** (MT7988A, Filogic 880)
with an **MT7927** (Filogic 380) dual-band Wi-Fi 7 card installed in one or both
of the board's PCIe slots.

## Applying the Config Fragment

```sh
cp configs/bananapi-r4-mt7927.config .config
make defconfig
make -j$(nproc)
```

`make defconfig` expands the fragment with all upstream defaults and resolves the
full dependency chain automatically.

## Key Options Enabled After `make defconfig`

| Symbol | Purpose |
|---|---|
| `CONFIG_PACKAGE_kmod-mt7927` | Meta-package that ties together driver + firmware |
| `CONFIG_PACKAGE_kmod-mt7927-firmware` | MT6639 firmware blobs (auto-selected as dep) |
| `CONFIG_PACKAGE_kmod-mt7925e` | PCIe driver reused from the upstream mt7925e / mt76 path (auto-selected as dep) |
| `CONFIG_PACKAGE_kmod-mt7925-common` | Shared MT792x code (auto-selected) |
| `CONFIG_PACKAGE_kmod-mt792x-common` | Common mt792x helpers (auto-selected) |
| `CONFIG_PACKAGE_kmod-mac80211` | Wi-Fi stack (auto-selected) |

## Driver Architecture Note

`kmod-mt7927` does **not** introduce a separate kernel module.  It reuses the
upstream `mt7925e` PCIe driver path (part of the `mt76` stack) combined with
the `MT6639` firmware images.  The chip is enumerated via its PCIe device ID
and loaded by `mt7925e.ko` at runtime.

## Firmware Requirement

The MT7927 / MT6639 firmware blobs must be present **before** running
`make` (the build checks for them):

```
target/linux/mediatek/mt7927/WIFI_MT6639_PATCH_MCU_2_1_hdr.bin
target/linux/mediatek/mt7927/WIFI_RAM_CODE_MT6639_2_1.bin
```

These blobs are packaged by `kmod-mt7927-firmware` and installed to
`/lib/firmware/mediatek/mt7927/` on the target device.

## Hardware Setup — Dual PCIe Cards on BPi R4

The Banana Pi R4 exposes two PCIe 3.0 slots (M.2 Key-M × 2).  You can fit
one or two MT7927 cards for a dual-radio Wi-Fi 7 setup.  The board DTS
overlays (`mt7988a-bananapi-bpi-r4-wifi-be14`) already describe the PCIe
topology; no extra DTS changes are needed.

With `kmod-mt7927` enabled, both cards are probed automatically at boot by
`mt7925e.ko` and appear as separate `phy` devices in the wireless subsystem.
