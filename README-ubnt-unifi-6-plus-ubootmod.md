# UniFi 6 Plus (U-Boot mod) — eMMC transformation

The `ubnt_unifi-6-plus-ubootmod` device in this tree replaces the
stock boot chain. The 4 GB eMMC carries everything: the boot chain,
the U-Boot environment, the Wi-Fi calibration (EEPROM + pre-cal, in
the `factory` GPT partition — the copy Linux actually reads) and the
firmware. The 16 MB SPI-NOR holds only a backup copy of the
calibration blob, as a single 512 KiB `factory-backup` partition at
offset 0. The GPT is the standard filogic eMMC layout
(`mt798x-gpt emmc`), so the `production` partition is sized by
`CONFIG_TARGET_ROOTFS_PARTSIZE` — set it to `3520` in menuconfig
(Target Images → Root filesystem partition size) to fill a 4 GB eMMC
(smallest common user area of "4GB" parts is ~3.64 GiB, so 3520M@64M
is safe).

## SPI-NOR layout

| Offset    | Size    | Stock        | This tree             | Content                    |
|-----------|---------|--------------|-----------------------|----------------------------|
| `0x00000` | 64 KiB  | `EEPROM`     | `factory-backup` (ro) | backup of the Wi-Fi EEPROM + pre-cal blob |
| `0x10000` | 512 KiB | `u-boot-env` | ⤷ (0x0–0x80000)       |                            |
| `0x90000` | ~15 MiB | (unused)     | (unused)              |                            |

The stock `EEPROM` partition (MAC addresses, serial) at `0x0` is
**overwritten** by the calibration blob — both ethernet and Wi-Fi MAC
addresses are randomly generated at boot. Keep `nor-eeprom.bin` from
step 1 if you ever want them back.

## eMMC layout (GPT, written once at install; standard filogic layout)

| # | Partition    | Offset | Size     | Content                          |
|---|--------------|--------|----------|----------------------------------|
|   | (boot0 hwpart / raw) | 0 | 4 MiB | BL2 preloader + GPT              |
| 1 | `ubootenv`   | 4M     | 512 KiB  | U-Boot environment (+ redundant) |
| 2 | `factory`    | 4608k  | 2 MiB    | Wi-Fi cal blob (primary copy)    |
| 3 | `fip`        | 6656k  | 4 MiB    | BL31 + U-Boot (offset fixed)     |
| 4 | `recovery`   | 12M    | 32 MiB   | recovery initramfs FIT           |
| 5 | `production` | 64M    | `ROOTFS_PARTSIZE` | sysupgrade FIT + rootfs_data |

## Transformation — commands

All steps run on the device. The calibration data is unique per device
and cannot be regenerated: **do step 1 before anything else** and copy
the files off the device (`scp`).

### 1. Back everything up (on the running system, stock layout)

The GPT partition named `factory` holds the calibration; check its
number first — usually `mmcblk0p3` on the stock GPT, `mmcblk0p2` on
the standard ubootmod GPT (the one this tree writes; there it holds
the primary copy of the calibration):

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

    # BL31+U-Boot FIP (p3 = 'fip' in the new GPT)
    dd if=openwrt-...-emmc-bl31-uboot.fip of=/dev/mmcblk0p3 conv=fsync

    # recovery image into p4 so bootmenu entry 3 works offline
    dd if=openwrt-...-initramfs-recovery.itb of=/dev/mmcblk0p4 conv=fsync

### 3. Write the calibration blob

Linux reads the calibration from the eMMC `factory` partition (p2 on
the new GPT); the SPI-NOR copy is only a backup. The blob is the
512 KiB partition image: cal data with the Wi-Fi MAC field set to
`ff:ff:ff:ff:ff:ff` so mt76 randomizes it, zero-padded to `0x80000`
(the precal nvmem cell reads up to `0x70020`, past the raw cal data,
so the padding keeps the partition content deterministic). Do not
touch anything else in the EEPROM area — in particular the pre-cal
enable byte at `0x19a` (`0x07` = group cal + 2G/5G DPD): if it reads
0, mt76 silently ignores the entire pre-cal/DPD block.

Write both copies from the new U-Boot (serve the blob as
`u6plus-factory.bin` via TFTP from 192.168.1.254):

    run boot_tftp_write_factory

which zero-fills the load buffer, TFTPs the blob (≤ 512 KiB) over it
and stores the buffer in the eMMC `factory` partition first, then in
the NOR as backup:

    mw.b $loadaddr 0 0x80000
    tftpboot $loadaddr u6plus-factory.bin
    run factory_write_emmc    # primary: eMMC 'factory' partition
    run factory_write_nor     # backup:  NOR 0x0-0x80000

Writing the primary copy from Linux (recovery system) also works —
`dd if=u6plus-factory.bin of=/dev/mmcblk0p2 conv=fsync` — then
refresh the NOR backup from U-Boot with `run factory_restore_nor`
(the NOR partition is read-only under Linux).

If the eMMC copy is ever lost (e.g. after rewriting the GPT), restore
it from the NOR backup with `run factory_restore_emmc`.

Wi-Fi stays down until this step is done; ethernet is unaffected.

### 4. Install the firmware

Boot the recovery (bootmenu 3), then `sysupgrade -n` with the
`squashfs-sysupgrade.itb` — or use bootmenu 4 to TFTP it straight into
`production`.

## Ethernet MAC

The stock board data (which held the Ubiquiti MAC, e.g.
`1c:6a:1b:9d:cb:99`) is overwritten by the transformation, so
there is no MAC in flash. U-Boot generates a random `ethaddr` on
first boot (`NET_RANDOM_ETHADDR`) and the first `saveenv` persists
it; the kernel DTS carries an `ethernet0` alias, so U-Boot injects
that `ethaddr` into the device tree at boot and Linux uses it —
stable across reboots once the environment has been saved. To use a
specific address (e.g. the original one from the stock EEPROM
backup):

    fw_setenv ethaddr 1c:6a:1b:9d:cb:99

The Wi-Fi MAC field in the cal blob is `ff:ff:ff:ff:ff:ff`
(invalid), so mt76 randomizes the Wi-Fi MAC on each boot.

## Notes

- After repartitioning, `factory.bin` (plus the eMMC `factory`
  primary and the NOR backup) are the only copies of the calibration
  — keep the off-device backup archived anyway.
- Env reset: bootmenu 9 or `eraseenv && reset` from U-Boot;
  `fw_printenv`/`fw_setenv` work from Linux (eMMC `ubootenv` partition).
- Reverting to stock: restore the stock GPT and boot chain, and write
  the stock NOR content back. The stock env spans `0x10000`–`0x90000`,
  which reaches past this tree's 512 KiB `factory-backup` partition,
  so restore it from U-Boot instead of Linux:

      cat nor-eeprom.bin nor-stock-env.bin > nor-restore.bin
      # then, from the U-Boot shell:
      tftpboot $loadaddr nor-restore.bin
      sf probe && sf erase 0x0 0x90000 && sf write $loadaddr 0x0 $filesize
