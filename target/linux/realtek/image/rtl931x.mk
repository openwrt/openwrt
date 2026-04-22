# SPDX-License-Identifier: GPL-2.0-only

include ./common.mk

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

define Device/zyxel_xs1930
  SOC := rtl9313
  DEVICE_PACKAGES := kmod-hwmon-lm85
  FLASH_ADDR := 0xb4280000
  IMAGE_SIZE := 31808k
  ZYNFW_ALIGN := 0x10000
  $(Device/zyxel_zynos)
endef

define Device/zyxel_xs1930-10
  DEVICE_MODEL := XS1930-10
  $(Device/zyxel_xs1930)
endef
TARGET_DEVICES += zyxel_xs1930-10

define Device/zyxel_xs1930-12f
  DEVICE_MODEL := XS1930-12F
  $(Device/zyxel_xs1930)
endef
TARGET_DEVICES += zyxel_xs1930-12f

define Device/zyxel_xs1930-12hp
  DEVICE_MODEL := XS1930-12HP
  $(Device/zyxel_xs1930)
endef
TARGET_DEVICES += zyxel_xs1930-12hp
