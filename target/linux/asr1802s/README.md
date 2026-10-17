# ASR1802S / Notion P298 (KAGU) OpenWrt Target

Support for the Notion P298, an ASR ASR1802S (ASR1826s / "KAGU") LTE gateway
based on the Marvell PXA18xx lineage. The target reuses mainline
`ARCH_MMP`/`mach-mmp` and its `mrvl,mmp-*` drivers, and adds ASR-specific
drivers for the blocks that differ.

## Hardware

| Field | Value | Source |
|---|---|---|
| SoC | ASR ASR1802S, ARMv7 Cortex-A7 | vendor DTB, `cpuinfo` |
| Board | Notion P298 (ASR 1826s / KAGU) | board silkscreen `P298_N13`, U-Boot banner `ASR KAGU EVB P298` |
| RAM | 128 MiB | vendor DTB |
| Flash | 128 MiB SPI-NAND | vendor DTB |
| Console | ttyS0, 115200n8 (`mrvl,mmp-uart`) | vendor DTB |
| Ethernet | ASR MAC + Motorcomm YT9215RB 5-port switch | vendor DTB |
| PCIe | 2x port DesignWare host controller | vendor DTB |
| WiFi | Realtek RTL8812FR (5 GHz) + RTL8192FR (2.4 GHz), PCIe | vendor DTB |
| PMIC | 88PM801 | vendor DTB |
| U-Boot | Marvell `mrvlboot` | boot log |
| Stock kernel | Linux 3.10.33 | `/proc/version` |

> Naming: the board is a **Notion** design sold by operator **Ooredoo** under
> carrier branding, but the DT uses the SoC vendor and the hardware platform
> code: `compatible = "asr,asr1826s-p298", "asr,1802s-evb", "asr,1802s"`.
> P298 is what the hardware carries. The operator SKU string is avoided because
> an unrelated MediaTek MT7621A product is also sold under it, which would make
> it ambiguous across two devices.

## Kernel version

`KERNEL_PATCHVER:=6.18`, matching OpenWrt's current default
(`target/linux/generic/kernel-6.18`).

The 13 patches in `patches-6.18/` are a 6.18 backport of the same series as the
upstream SoC RFC. Note that the `yt921x` DSA driver (patch 0010) was accepted
into mainline during the 6.19 merge window, so patch 0010 can be dropped once
OpenWrt moves to 6.19 or later.

## Feature support

| Subsystem | Status |
|---|---|
| Ethernet (ASR EMAC + YT9215RB DSA switch) | in this PR |
| SPI-NAND / QSPI | in this PR |
| Clock controller | in this PR |
| PCIe host controller | in this PR |
| WiFi (RTL8812FR / RTL8192FR) | **not supported** — `rtl8xxxu` has no PCI transport in mainline; see below |
| USB 3.0 PHY, MMC/SD | not yet |

### Why WiFi is not included

`rtl8xxxu` is USB-only in mainline. Supporting these two PCIe cards would
require writing a new PCI bus layer for the driver, which is a change of that
scope rather than part of a device-support PR. The firmware blob the RTL8192F
family loads (`rtlwifi/rtl8192fufw.bin`) is packaged as
`rtl8192fu-firmware` in this PR so the driver work is all that remains.

## Flash layout

The stock flash is laid out as an A/B scheme. Two areas must never be
overwritten by a flash image:

- the DTIM bootloader partitions, and
- `NVM` (per-unit MAC addresses, SIM/IMEI and radio calibration data).

## Files

- `Makefile` — target definition (kernel patch level, arch/subtarget)
- `config-6.18` — kernel configuration for this target
- `generic/target.mk` — subtarget and package defaults
- `image/Makefile` — image layout
- `base-files/etc/board.d/02_network` — network interface mapping
- `patches-6.18/` — the kernel patch series, in apply order. The DTS files are
  added by patch 0003 under `arch/arm/boot/dts/asr/`.

## References

- Kernel RFC series: `github.com/devopnem/linux-asr1802s-rfc`
