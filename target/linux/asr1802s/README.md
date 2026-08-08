# ASR1802S / Ooredoo R291 v2 (KAGU) OpenWrt Target -- WIP Draft

> **⚠️ Work in Progress -- NOT ready for merge.**
> This is a Draft PR to solicit early feedback from OpenWrt maintainers.
> See upstream kernel RFC submission on `linux-arm-kernel` for SoC port.

## Hardware Summary

| Field | Value | Source |
|---|---|---|
| SoC | ASR ASR1802S, ARMv7-Cortex-A7 | /proc/cpuinfo (`[410fc073]`) |
| Board | ASR 1826s (KAGU) Board EVB | /proc/device-tree/model |
| RAM | 128 MiB @ 0x04000000 (32 MiB CP carve-out) | cmdline + /proc/meminfo |
| Flash | 128 MiB SPI-NAND F35UQA001G, 24 partitions A/B+UBI | /proc/mtd |
| Console | ttyS0 @ 115200n8 (mrvl,mmp-uart @ d4017000) | /proc/iomem, dmesg |
| WiFi | 2x Realtek RTL8812FE (PCIe gen1x1) | dmesg, /rom/lib/modules/3.10.33/rtl8192cd.ko |
| Ethernet | ASR MAC @ d4281800 + YT9215 switch | dmesg + /proc/iomem |
| VoIP | MaxLinear SLIC (2x FXS) via SPI | dmesg (maxlinear-slic) |
| PMIC | 88PM801 @ I2C 0x30 | vendor DTB |
| U-Boot | Marvell "mrvlboot", bootdelay=1, loadaddr=0x8000 | mtd21 bootargs |
| Stock Kernel | Linux 3.10.33 | /proc/version |

## Status

- **Kernel**: No upstream support for ASR1802S in kernel.org. Vendor kernel 3.10.33.
  SoC blocks (irq/timer/uart/gpio/dma/i2c/dwc3) map to existing Marvell MMP
  mainline drivers. ASR-specific drivers (clock/MMC/QSPI/eth/PCIE/USB3-PHY)
  are RFC follow-up material.
- **OpenWrt**: Stock firmware runs OpenWrt BB14.07 (mmp/asr1826s fork).
  `opkg` is present but feeds 404 (mirror dead). No UART access yet --
  boot test pending.
- **NVM preservation**: `overlay/root/NVM/*` (WiFi MAC, SIM/IMEI, cal data)
  MUST be preserved across any flash. Not handled in this skeleton yet.

## Dependencies

1. **Upstream Linux kernel SoC support** for ASR1802S (RFC series to linux-arm-kernel)
2. **UART access** for boot verification (pending)

## Paths Forward

1. **(Preferred)** Upstream kernel SoC port → full OpenWrt target with
   modern kernel (5.15+) + `rtw88` for RTL8812FE.
2. **(Fallback)** Repackage vendor rootfs (kernel 3.10.33) with additional
   opkg packages.

## Files

- `Makefile` -- target definition (kernel config, CPU/arch)
- `config-6.12` -- minimal kernel configuration
- `image/Makefile` -- image layout (WIP)
- `base-files/etc/board.d/02_network` -- network interface mapping
- `patches-6.12/` -- kernel patches (same series as the upstream RFC;
  DTS files live under `arch/arm/boot/dts/asr/` inside patch 0003)

## References

- Kernel RFC repo: `github.com/devopnem/linux-asr1802s-rfc`
