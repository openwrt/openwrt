TRX_ENDIAN := le

define Device/en7528_generic
  DEVICE_VENDOR := EN7528
  DEVICE_MODEL := Generic
  DEVICE_DTS := en7528_generic
endef
TARGET_DEVICES += en7528_generic

define Device/dasan_h660gm-a
  $(call Device/tclinux-ubi)
  DEVICE_VENDOR := DASAN
  DEVICE_MODEL := H660GM-A
  FACTORY_SIZE := 32m
  TRX_MODEL := Dewberry
  DEVICE_PACKAGES := kmod-usb2 kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
endef

define Device/dasan_h660gm-a-airtel
  $(Device/dasan_h660gm-a)
  DEVICE_VARIANT := Airtel
  DEVICE_DTS := en7528_dasan_h660gm-a-airtel
endef
TARGET_DEVICES += dasan_h660gm-a-airtel

define Device/dasan_h660gm-a-generic
  $(Device/dasan_h660gm-a)
  DEVICE_VARIANT := Generic
  DEVICE_DTS := en7528_dasan_h660gm-a-generic
endef
TARGET_DEVICES += dasan_h660gm-a-generic

define Device/jiofiber_en7528
  $(call Device/tclinux-ubi)
  DEVICE_VENDOR := JioFiber
  FACTORY_SIZE := 40m
  TRX_LOADADDR := 0x80002000
  KERNEL := kernel-bin | append-dtb | tclinux-free-bootbase-jump | lzma | \
    kernel-trx
  DEVICE_PACKAGES := kmod-usb3 kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap
endef

define Device/jiofiber_jcow414
  $(Device/jiofiber_en7528)
  DEVICE_MODEL := JCOW414
  DEVICE_ALT0_VENDOR := JioFiber
  DEVICE_ALT0_MODEL := JCO4032
  DEVICE_DTS := en7528_jiofiber_jcow414
endef
TARGET_DEVICES += jiofiber_jcow414

define Device/jiofiber_jcow407
  $(Device/jiofiber_en7528)
  DEVICE_MODEL := JCOW407
  DEVICE_ALT0_VENDOR := JioFiber
  DEVICE_ALT0_MODEL := JCOW407-IN
  DEVICE_DTS := en7528_jiofiber_jcow407
endef
TARGET_DEVICES += jiofiber_jcow407

define Device/tplink_ex20v
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := EX20v
  # Single MT7915 DBDC chip (2.4 GHz phy0 + 5 GHz phy1); internal GSW, no
  # external switch chip. mt7915-firmware is a separate package that depends
  # on the driver rather than the other way around, so it must be listed
  # explicitly or the radio comes up without firmware.
  DEVICE_PACKAGES := kmod-mt7915e kmod-mt7915-firmware kmod-usb3 kmod-8021q
  DEVICE_DTS := en7528_tplink_ex20v
  TRX_MODEL := EX20v
  NAND_SIZE := 512m
  # F50L4G41XB geometry (stock bootlog): 4 KiB page, 256 KiB erase block
  # (erase_shift 0x12). Device/Default's 128k/2048 is sized for other en7528
  # boards' smaller NAND; ubinize with the wrong PEB/page size misaligns and
  # UBI won't attach.
  BLOCKSIZE := 256k
  PAGESIZE := 4096
  # "ubi" partition (0x2f80000-0x1c000000 = 0x19080000 = 411648 KiB). The
  # kernel boots from the slave slot at 0x2980000 (bootbase boot flag=1); UBI
  # sits right after it so the whole sysupgrade/factory image lands
  # contiguously starting at 0x2980000.
  IMAGE_SIZE := 411648k
  # The free bootbase (EN7528 v1.1) decompresses the kernel to 0x80002000,
  # but the kernel is linked at 0x80020000: tclinux-free-bootbase-jump
  # prepends a "j 0x80020000" stub + 0x1E000 pad so the low load address
  # trampolines to the real entry. ex20v-tctag prepends the proprietary
  # 256-byte TrendChip tag the free bootbase reads (not the zytrx HDR2 that
  # tclinux-trx.sh emits), so it can locate/decompress the kernel read raw
  # from the "kernel" slot at 0x2980000.
  KERNEL := kernel-bin | append-dtb | tclinux-free-bootbase-jump | lzma | \
    ex20v-tctag | check-size 6291456
  # Initramfs is RAM-booted (xmodem/tftp then jump), not read from the flash
  # slot, so it must stay a plain self-decompressing vmlinuz: the
  # tclinux-trx path pads to 4 MiB and fails ("kernel is too large") on the
  # ~11 MiB initramfs kernel.
  KERNEL_INITRAMFS := kernel-bin | append-dtb
  IMAGES := sysupgrade.bin factory.bin
  # UBI rootfs: standard nand.sh sysupgrade preserves config (rootfs_data ubifs).
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  # First-flash image: raw kernel slot + UBI image back to back.
  IMAGE/factory.bin := append-kernel | pad-to 6291456 | append-ubi
endef
TARGET_DEVICES += tplink_ex20v
