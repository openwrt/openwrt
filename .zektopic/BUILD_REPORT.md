# Build Report

## Merge Status

| Item | Status |
|------|--------|
| PR #75 merge conflicts resolved | ✅ (2 files) |
| Zektopic custom changes preserved | ✅ Verified |
| Build prerequisites | ✅ Installed |

## Target Build Results

| Target | Status | Time | Image Size | QEMU Test |
|--------|--------|------|------------|-----------|
| x86/64 (fix init_size) | ✅ Built | 2h 0m | 11 images | ✅ Passed (UEFI/OVMF) |
| ath79/generic | ✅ Built | ~4h | 2 images | ⏹ QEMU smoke scripts ready |
| mediatek/filogic | ✅ Built | ~4h | 11 images | ⏹ QEMU smoke scripts ready |
| ramips/mt7621 | ✅ Built | ~4h | 2 images | ⏹ QEMU smoke scripts ready |
| malta/be (MIPS) | 🔄 In progress | — | — | — |
| armsr/armv7 (ARM) | 🔄 In progress | — | — | — |

## Build Details

### x86/64 — Build Complete (Kernel Fix Applied)

| Item | Detail |
|------|--------|
| Build start | 2026-05-13 15:07 UTC |
| Build end | 2026-05-13 17:05 UTC |
| Kernel | Linux 6.18.28 (fix: real bzImage, init_size=27.6MB) |
| GCC | 14.3.0 |
| libc | musl |
| Images | ext4, squashfs, targz — both BIOS and EFI |
| Packages | 84 built and indexed |
| Build errors | None |

### Kernel init_size Fix

OpenWrt's `Kernel/CopyImage` step uses `objcopy -O binary` on vmlinux to create the kernel image, which produces a bzImage with inflated init_size (2.5GB) causing GRUB "out of memory" in QEMU. The fix replaces KDIR/bzImage with the real build tree bzImage from `arch/x86/boot/bzImage`, which has the correct init_size=27.6MB and boots properly under UEFI.

### Generated Images (with fix)

| Image | Size | Description |
|-------|------|-------------|
| `openwrt-x86-64-generic-ext4-combined-efi.img.gz` | — | ext4, EFI boot |
| `openwrt-x86-64-generic-ext4-combined.img.gz` | — | ext4, BIOS boot |
| `openwrt-x86-64-generic-squashfs-combined-efi.img.gz` | — | squashfs, EFI boot |
| `openwrt-x86-64-generic-squashfs-combined.img.gz` | — | squashfs, BIOS boot |
| `openwrt-x86-64-generic-kernel.bin` | 6.6M | Kernel (correct bzImage) |
| `openwrt-x86-64-generic-rootfs.tar.gz` | 7.3M | Rootfs tarball |

### QEMU Smoke Test

Boot method: UEFI via OVMF (not `-kernel`, which is broken in QEMU 10.x for PC machine types). QEMU v10.2.1, 512MB RAM, KVM when available. Configuration: GRUB reads kernel from FAT boot partition and boots to preinit.

Result: **PASS** — kernel booted, drivers loaded (virtio, e1000), procd init started, console activated.

Boot command:
```
sudo qemu-system-x86_64 \
  -bios /usr/share/ovmf/OVMF.fd \
  -drive file=<image>,format=raw,if=virtio \
  -m 512M -enable-kvm -cpu host -nographic -no-reboot
```

### QEMU Test Verification

Automated test scripts updated:
- `smoke_test.sh` — quick UEFI smoke test (60s timeout)
- `qemu_test.sh` — universal test: x86/64, MIPS malta BE/LE, ARM 32/64
- `config_variants.sh` — minimal, full, dev, hardened configs

### ath79/generic — Build Complete

| Item | Detail |
|------|--------|
| Build start | 2026-05-13 17:12 UTC |
| Architecture | MIPS 24kc |
| Kernel | Linux 6.12.87 |
| GCC | 14.3.0 |
| Profile | 8dev_carambola2 |
| Build errors | None |

### ramips/mt7621 — Build Complete

| Item | Detail |
|------|--------|
| Build start | 2026-05-13 17:12 UTC |
| Architecture | MIPSEL 24kc |
| Kernel | Linux 6.18.28 |
| GCC | 14.3.0 |
| Profile | adslr_g7 |
| Build errors | None |

### mediatek/filogic — Build Complete

| Item | Detail |
|------|--------|
| Build start | 2026-05-13 17:12 UTC |
| Architecture | AArch64 Cortex-A53 |
| Kernel | Linux 6.18.28 |
| GCC | 14.3.0 |
| Profile | OpenWrt One (11 images incl. BL2 bootloaders) |
| Build errors | None |

### malta/be (MIPS) — In Progress

| Item | Detail |
|------|--------|
| Target | malta/be |
| Architecture | MIPS 24kc (big-endian) |
| QEMU test | Script ready: `qemu_test.sh malta_be` |
| Status | Toolchain cross-compilation in worktree |

### armsr/armv7 (ARM) — In Progress

| Item | Detail |
|------|--------|
| Target | armsr/armv7 |
| Architecture | ARM Cortex-A15 |
| QEMU test | Script ready: `qemu_test.sh armvirt_32` |
| Status | Toolchain cross-compilation in worktree |

## Test Scripts

| Script | Purpose |
|--------|---------|
| `TEST_SCRIPTS/smoke_test.sh` | Quick x86/64 UEFI smoke test |
| `TEST_SCRIPTS/qemu_test.sh` | Universal QEMU boot test (5 targets) |
| `TEST_SCRIPTS/config_variants.sh` | Config variant builder/test |
| `TEST_SCRIPTS/build_x86_64.sh` | x86/64 build helper |
| `TEST_SCRIPTS/build_ath79.sh` | ath79 build helper |
| `TEST_SCRIPTS/build_mediatek.sh` | mediatek build helper |
| `TEST_SCRIPTS/build_ramips.sh` | ramips build helper |
| `TEST_SCRIPTS/verify_merge.sh` | Merge quality verification |
