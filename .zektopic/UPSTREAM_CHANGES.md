# Upstream Changes Merged (PR #75)

PR #75 merged 223 commits from `openwrt:main` into `Zektopic:main`. This document summarizes the upstream changes absorbed.

## Kernel Updates

| Component | Change |
|-----------|--------|
| Linux 6.12 | Bumped 6.12.80 → 6.12.84 |
| Linux 6.18 | Bumped 6.18.21 → 6.18.25 |
| Kernel patches | Refreshed for all targets, backports from upstream Linux |
| Kernel configs | Updated for new kernel options across all targets |

## New Device Support

| Target | Device |
|--------|--------|
| mediatek/filogic | ZBT-Z8103AX-D |
| mediatek | NMBM fix for Huasifei WH3000 Pro NAND |
| ipq50xx | Linksys MX5500 (label-mac-device) |
| ipq40xx | Cisco Meraki Z3C |
| ath79 | TP-Link TL-WR902AC v1 moved to tiny |

## Architecture Updates

| Target | Changes |
|--------|---------|
| **realtek** | DSA state handling fix, MDIO regmap_bulk_write refactoring (RTL838x/839x/930x/931x), PCS chattiness reduction, RTL8224 reset GPIO, XSTP state improvements |
| **airoha** | PCS standalone implementation migration, ethernet node updates, reserved_bmt partition size increase |
| **mvebu** | 6.18 testing kernel added, leds-turris-omnia dependency fix, WRT1900AC v1 disabled by default |
| **bcm53xx** | ramdisk added to FEATURES |
| **ramips** | mt7621: CONFIG_PAGE_POOL_STATS disabled |
| **microchipsw** | LAN8814 QSGMII soft reset fix |

## Package Updates

| Package | Change |
|---------|--------|
| ca-certificates | Updated to 20260223 |
| wireless-regdb | Updated to 2026.03.18 |
| intel-microcode | Updated |
| dnsmasq | Six CVE-fix patches applied to 2.92. Busybox pidof dependency. |
| dropbear | Busybox pidof dependency. Config file packaging consolidated. |
| wireguard-tools | Watchdog idle timeout increased to 180s |
| mbedtls | Security patches |
| mt76 | mt7622-firmware built only for mt7622 sub-target |

## Build System

- SDK: support for signing .apk packages
- Generic: overlay filesystem hardened (DISABLE OVERLAY_FS_REDIRECT_ALWAYS_FOLLOW)
- GitHub Actions: workflow_dispatch trigger + separate concurrency groups for kernel/packages workflows
- Kernel: backported phylink_replay_link() API (6.18) and PCS standalone feature (6.12)
- Toolchain: musl backported CVE fixes
- GCC: gcc-12 patch stack removed (EOL)

## Key Contributors

hauke, robimarko, Ansuel, graysky2, DragonBluep, neheb, achtern, drizt, joelinux60, fildunsky, pprindeville, saldry, hnyman, peterwillcn, stklcode, halmartin, ecsv, plappermaul, jonasjelonek, RussellSenior, vincele, aiamadeus, robertoalmeidasilva
