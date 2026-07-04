# UniFi 6 Plus (U-Boot mod) — SPI-NOR transformation

The `ubnt_unifi-6-plus-ubootmod` device in this tree moves the Wi-Fi
calibration (EEPROM + pre-cal) into the 16 MB SPI-NOR, into the 512 KiB
slot that held the stock bootloader's environment. The 4 GB eMMC then
carries only the boot chain, the U-Boot environment and the firmware,
and its `production` partition statically fills the whole chip
(~3.4 GiB `rootfs_data` via fitblk, no config tweaking).

## SPI-NOR layout

| Offset    | Size    | Stock        | This tree     | Content                           |
|-----------|---------|--------------|---------------|-----------------------------------|
| `0x00000` | 64 KiB  | `EEPROM`     | `EEPROM` (ro) | MAC addresses, serial (untouched) |
| `0x10000` | 512 KiB | `u-boot-env` | `factory`     | Wi-Fi EEPROM + pre-cal data       |
| `0x90000` | ~15 MiB | (unused)     | (unused)      |                                   |

## eMMC layout (GPT, written once at install)

| Partition    | Offset | Size     | Content                             |
|--------------|--------|----------|-------------------------------------|
| (boot0 hwpart / raw) | 0 | 4 MiB  | BL2 preloader + GPT                 |
| `ubootenv`   | 4M     | 512 KiB  | U-Boot environment (+ redundant)    |
| `fip`        | 6656k  | 4 MiB    | BL31 + U-Boot (offset fixed)        |
| `recovery`   | 12M    | 32 MiB   | recovery initramfs FIT              |
| `production` | 64M    | 3520 MiB | sysupgrade FIT + rootfs_data        |

## Transforming the SPI-NOR — commands

All steps run on the device. The calibration data is unique per device
and cannot be regenerated: **do step 1 before anything else** and copy
the files off the device (`scp`).

### 1. Back everything up (on the running system, stock layout)

The GPT partition named `factory` holds the calibration; check its
number first — usually `mmcblk0p3` on the stock GPT, `mmcblk0p2` on an
earlier standard ubootmod GPT:

    fdisk -l /dev/mmcblk0 | grep -i factory

    # calibration blob (first 512 KiB is all that matters)
    dd if=/dev/mmcblk0p3 of=/tmp/factory.bin bs=64k count=8

    # full NOR partitions: EEPROM and the stock U-Boot environment
    dd if=/dev/mtd0ro of=/tmp/nor-eeprom.bin
    dd if=/dev/mtd1ro of=/tmp/nor-stock-env.bin

`nor-stock-env.bin` is only needed if you ever want to return to the
stock bootloader; `factory.bin` you need in step 3.

### 2. Install the OpenWrt boot chain (from the recovery initramfs)

TFTP-boot `...-initramfs-recovery.itb` through the stock bootloader
(serial console), then from its shell:

    # BL2 preloader into the eMMC boot0 hardware partition
    echo 0 > /sys/block/mmcblk0boot0/force_ro
    dd if=openwrt-...-emmc-preloader.bin of=/dev/mmcblk0boot0 conv=fsync

    # new GPT — destroys the stock partition table!
    dd if=openwrt-...-emmc-gpt.bin of=/dev/mmcblk0 conv=fsync
    partx -u /dev/mmcblk0

    # BL31+U-Boot FIP (p2 = 'fip' in the new GPT)
    dd if=openwrt-...-emmc-bl31-uboot.fip of=/dev/mmcblk0p2 conv=fsync

    # recovery image into p3 so bootmenu entry 3 works offline
    dd if=openwrt-...-initramfs-recovery.itb of=/dev/mmcblk0p3 conv=fsync

### 3. Write the calibration into the NOR

Either from Linux (recovery or installed system built from this tree —
the partition is called `factory` there):

    mtd write /tmp/factory.bin factory

or from the new U-Boot (serves `u6plus-factory.bin` via TFTP from
192.168.1.254):

    run boot_tftp_write_factory

which is shorthand for:

    tftpboot $loadaddr u6plus-factory.bin
    sf probe && sf update $loadaddr 0x10000 $filesize

Wi-Fi stays down until this step is done; ethernet is unaffected.

### 4. Install the firmware

Boot the recovery (bootmenu 3), then `sysupgrade -n` with the
`squashfs-sysupgrade.itb` — or use bootmenu 4 to TFTP it straight into
`production`.

## Hardened kernel command line

The image boots with the applicable subset of a hardened cmdline
(`init_on_alloc/init_on_free`, `slab_nomerge`, `slab_debug=FZ`,
`page_alloc.shuffle`, `randomize_kstack_offset`, `hash_pointers`,
`mitigations=auto,nosmt`, `ssbd=force-on`, `random.trust_*=off`,
`proc_mem.force_override=ptrace`, `bdev_allow_write_mounted=0`,
`debugfs=off`, `oops=panic`). x86-only switches (pti, spectre_v2,
vsyscall, ia32_emulation, kvm/SEV, IOMMU flags, CET tunables) and
Fedora/dracut/systemd options were dropped, as were `module.sig_enforce`
and `lockdown` (OpenWrt loads unsigned kernel modules — enforcing
signatures would leave the box without Wi-Fi). The same list lives in
the U-Boot default env (`bootargs`) and the device tree.

## Notes

- After repartitioning, `factory.bin` (plus the NOR copy) are the only
  copies of the calibration — keep the backup archived.
- Env reset: bootmenu 9 or `eraseenv && reset` from U-Boot;
  `fw_printenv`/`fw_setenv` work from Linux (eMMC `ubootenv` partition).
- Reverting to stock: restore the stock GPT and boot chain, and write
  `nor-stock-env.bin` back over NOR `0x10000`
  (`mtd write /tmp/nor-stock-env.bin factory` from this tree's image).
