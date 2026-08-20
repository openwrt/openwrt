TRX_ENDIAN := le

define Device/Default
  PROFILES = Default $$(DEVICE_NAME)
  KERNEL_LOADADDR = $(loadaddr-y)
  SOC = $$(SUBTARGET)
  FILESYSTEMS := squashfs
  DEVICE_DTS = $$(SUBTARGET)-$(subst -,_,$(1))
  DEVICE_DTS_DIR := ../dts
  IMAGES := sysupgrade.bin
  IMAGE/sysupgrade.bin := append-kernel | pad-to 128k | append-rootfs | \
    pad-rootfs | append-metadata

  BLOCKSIZE := 128k
  PAGESIZE := 2048
  # decompression buffer limit, the size of LZMA-compressed data is read from
  # the trx header
  KERNEL_SIZE := 7480k
  KERNEL_NAME := vmlinuz.bin
  KERNEL_LOADADDR := 0x80020000
  KERNEL := kernel-bin | append-dtb
  UBINIZE_OPTS := -E 5
endef

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

define Device/tplink_xc220-g3v
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := XC220
  DEVICE_VARIANT := G3v
  DEVICE_PACKAGES += hostapd-mbedtls wpad-mbedtls wpa-supplicant-mbedtls \
                     kmod-mt7603 kmod-mt7615e kmod-mt7663-firmware-ap kmod-mt7663-firmware-sta
endef
TARGET_DEVICES += tplink_xc220-g3v
