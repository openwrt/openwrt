# SPDX-License-Identifier: GPL-2.0-only

define Device/plasmacloud-common
  SOC := rtl9312
  UIMAGE_MAGIC := 0x93100000
  DEVICE_VENDOR := Plasma Cloud
  IMAGE_SIZE := 13376k
  BLOCKSIZE := 64k
  DEVICE_PACKAGES := kmod-hwmon-adt7475
  KERNEL_INITRAMFS := \
    kernel-bin | \
    append-dtb | \
    lzma | \
    uImage lzma
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | pad-to $$(BLOCKSIZE)
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef

define Device/plasmacloud_esx28
  $(Device/plasmacloud-common)
  DEVICE_MODEL := ESX28
endef
TARGET_DEVICES += plasmacloud_esx28

define Device/plasmacloud_psx28
  $(Device/plasmacloud-common)
  DEVICE_MODEL := PSX28
  DEVICE_PACKAGES += poemgr
endef
TARGET_DEVICES += plasmacloud_psx28

define Device/xikestor_sks8300-12x-v1
  SOC := rtl9313
  DEVICE_VENDOR := XikeStor
  DEVICE_MODEL := SKS8300-12X
  DEVICE_VARIANT := V1
  BLOCKSIZE := 64k
  KERNEL_SIZE := 8192k
  IMAGE_SIZE := 30720k
  IMAGE/sysupgrade.bin := pad-extra 256 | append-kernel | xikestor-nosimg | \
        jffs2 nos.img -e 4KiB -x lzma | pad-to $$$$(KERNEL_SIZE) | \
        append-rootfs | pad-rootfs | append-metadata | check-size
endef
TARGET_DEVICES += xikestor_sks8300-12x-v1
