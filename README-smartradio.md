# Open firmware for Doodle Labs Smart Radio (QCA9533 / ath79)

Clean OpenWRT `ath79` port of the QCA9533 Smart Radio, with upstream
**batman-adv** mesh and **WFB-NG** video injection, excluding Doodle Labs
proprietary IP. Built from live analysis of a running unit (`firmware-2024-10.1`,
legacy `ar71xx`) + its `.ipk` packages.

## Hardware (captured live)

| | |
|---|---|
| SoC | Qualcomm Atheros QCA9533 (MIPS 24Kc, 650 MHz) |
| RAM / flash | 64 MiB / 32 MiB (Winbond w25q256 SPI-NOR) |
| Radio | AR9531 integrated, **2.4 GHz only**, ath9k (mainline) |
| eth0 | GMAC0 → external PHY (LAN) |
| eth1 | GMAC1 → AR934x 5-port switch (WAN) |
| Button | BTN_1 / reset on GPIO16 |
| Boot | u-boot `bootm 0x9f0c0000` → kernel at flash offset `0xc0000` |

## What is excluded (Doodle Labs IP) vs. replaced

| Proprietary (removed) | Open replacement |
|---|---|
| `webway` / `kmod-webway` (GPLv2 batman-adv derivative + UDP-tunnel/radiotap) | upstream **batman-adv** over 802.11s |
| `dynamic_mesh`, `dl-central-*`, `meshmap` | OpenWRT UCI + `alfred`/`batctl` |
| `dl-license-manager` + `GV` permissive regdb (always-on) | stock `wireless-regdb`; **GV gated** (below) |
| `ath9k_tpcd`, `dl-pical-check` RF daemons | stock ath9k (reads cal from `art`) |

Kept untouched (per-unit data, not IP): `u-boot`, `u-boot-env`, **`art`**
(RF calibration + MACs). Only kernel+rootfs are (sys)upgraded.

## Regulatory lock (the important part)

The stock radio runs 902–3700 MHz up to ~36 dBm under a private `GV` regdomain —
lawful **only under the spectrum licence tied to the unit**. This port is
**locked by default**: it ships stock `wireless-regdb` (no GV), so the radio
behaves as a standard 2.4 GHz device.

`GV` is included in source but double-gated:
1. **Build gate** — `CONFIG_SMARTRADIO_REGDB_GV_UNLOCK` (default **n**). Only an
   approved build compiles GV into `regulatory.db`.
2. **Runtime gate** — UCI `radio.main.regdomain='GV'` + `gv_approved='1'`, and it
   still refuses unless the built regdb actually contains GV.

See `package/wireless-regdb-gv/`. Unlock = one explicit, auditable act; a stock
image cannot be coaxed into GV.

## WFB-NG

`package/wfb-ng/` builds the wifibroadcast C datapath (`wfb_tx`/`wfb_rx`/
`wfb_keygen`). ath9k is the reference injection driver. **One radio → one mode
at a time**: `/etc/init.d/radio-mode` runs *either* mesh *or* wfb-ng, chosen by
UCI `radio.main.mode` (default `mesh`). Generate a key with `wfb_keygen` per link.

## Files in this port

```
target/linux/ath79/dts/qca9533_doodlelabs_smartradio.dts   # board DTS
target/linux/ath79/image/generic-doodlelabs.mk             # image recipe
target/linux/ath79/generic/base-files/.../02_network       # LAN/WAN
package/wfb-ng/Makefile                                     # wfb-ng datapath
package/wireless-regdb-gv/                                  # gated GV regdb
files/etc/config/radio                                     # mode + reg UCI
files/etc/init.d/radio-mode                                # mesh|wfbng + reg lock
```

## Build

Use `./build.sh` (clones + integrates + configures + builds, with the required
`umask 0022` baked in):

```
./build.sh                       # openwrt-24.10 — first-boot validation
BRANCH=openwrt-25.12 ./build.sh  # next phase — 25.12.0 is released
```

Port files are release-agnostic (modern `nvmem-layout` DTS; ath9k/batman-adv are
mainline), so moving 24.10 → 25.12 is just the branch switch above + rebuild.

### Manual steps (equivalent)

1. Copy `target/` and `package/` into an OpenWRT 24.10 tree; append the
   `generic-doodlelabs.mk` block to `target/linux/ath79/image/generic.mk` and the
   `02_network` case to the existing file.
2. Wire `package/wireless-regdb-gv/Config.in` into the config tree and add the
   two-line `Build/Prepare` hook (see that dir's README). **Leave the GV symbol
   off unless you have regulatory approval.**
3. `./scripts/feeds update -a && ./scripts/feeds install wfb-ng batman-adv`
4. `make menuconfig` → select target ath79/generic, device *Doodle Labs Smart
   Radio* → `make`.

## Install & recovery (serial = un-brick path)

Bootloader is **Caraboot v2.9 / U-Boot 1.1.4**, `bootdelay=1`, 115200 8N1,
default net `device 192.168.2.100 / server 192.168.2.254`. It has
`tftpboot`/`bootm`/`erase` + an auto-TFTP recovery path.

**Do NOT `cp.b` a full image in u-boot** — flash is 32 MiB but the NOR mmap
window is 16 MiB. Install via a RAM-booted initramfs instead (flash untouched
until the very last step):

1. Build both `...-sysupgrade.bin` and `...-initramfs-kernel.bin`
   (`CONFIG_TARGET_ROOTFS_INITRAMFS=y` in menuconfig).
2. On serial, interrupt u-boot (bootdelay is 1 s — be ready), then:
   ```
   setenv serverip 192.168.2.254 ; setenv ipaddr 192.168.2.100
   tftpboot 0x80060000 openwrt-ath79-generic-doodlelabs_smartradio-initramfs-kernel.bin
   bootm 0x80060000      # confirm load/entry with `mkimage -l` on the image
   ```
   → OpenWRT runs **in RAM**, flash still stock. Zero risk so far.
3. From that RAM OpenWRT, `scp`/`wget` the sysupgrade image to `/tmp` and:
   ```
   sysupgrade -n /tmp/openwrt-ath79-generic-doodlelabs_smartradio-sysupgrade.bin
   ```
   The kernel MTD driver writes the full 32 MiB correctly. Reboot → OpenWRT.

**Recovery:** re-TFTP the initramfs and `bootm` any time; flash is never
required to boot. Full stock backups are persisted in `../stock-backup/`:
`mtd0-uboot`, `mtd1-uboot-env`, `mtd2-art` (**irreplaceable** RF cal + MACs),
and `mtd3-firmware-stock` (complete factory OS, for a full return to stock via
`mtd write` from a RAM-booted system). Never erase mtd0/1/2.

> First stock→OpenWRT install is via the initramfs path above, **not** stock
> `sysupgrade` (it checks board/target and will reject a foreign image).
> `sysupgrade` proper is then the normal OpenWRT→OpenWRT upgrade/test.

## Roadmap status

- [x] 1. Baseline capture — flash backups, full hardware map, GV regdb transcribed
- [~] 2. Clean OpenWRT build — DTS + recipes drafted here; **needs real build + boot**
- [ ] 3. Open mesh — 802.11s + batman-adv two-unit validation vs. webway baseline
- [ ] 4. MAVLink — rebuild mavp2p/luci-app-mavlink; re-point mavlink-rssi to nl80211
- [ ] +  WFB-NG — build package, validate injection link
- [ ] 5. (optional) webway source request under GPLv2, if mesh parity needed

## Verify on hardware (unknowns, do not assume)

- Reset button polarity (DTS uses ACTIVE_LOW).
- GPIO4 / GPIO15 roles — exported by stock, unverified, **left unconfigured**
  (likely RF front-end band-select; driving them could mis-set the front end).
- External PHY model on eth0 (MDIO uid `0x004dd042`) — generic PHY works; a
  specific driver may be needed for link LEDs.
- `art` cal offset/length (`0x1000`/`0x440`) is the ath9k default — confirm the
  radio comes up and tx/rx is sane.
- Second SPI device on CS1 (`spi0.1`) — present in stock, role unknown, omitted.
