# SPDX-License-Identifier: GPL-2.0-only

include ./common.mk

define Build/xikestor-nosimg
  $(STAGING_DIR_HOST)/bin/nosimg-enc -i $@ -o $@.new
  mv $@.new $@
endef

define Device/hasivo_s1100w-8xgt-se
  SOC := rtl9303
  DEVICE_VENDOR := Hasivo
  DEVICE_MODEL := S1100W-8XGT-SE
  IMAGE_SIZE := 12288k
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += hasivo_s1100w-8xgt-se

define Device/plasmacloud-common
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93000000
  DEVICE_VENDOR := Plasma Cloud
  DEVICE_PACKAGES := poemgr
  IMAGE_SIZE := 15872k
  BLOCKSIZE := 64k
  KERNEL_INITRAMFS := kernel-bin | append-dtb | lzma | uImage lzma
  KERNEL := kernel-bin | append-dtb | lzma | uImage lzma | pad-to $$(BLOCKSIZE)
  IMAGES += factory.bin
  IMAGE/factory.bin := append-kernel | append-rootfs | pad-rootfs | check-size
  IMAGE/sysupgrade.bin := append-rootfs | pad-rootfs | sysupgrade-tar rootfs=$$$$@ | append-metadata
endef

define Device/plasmacloud_psx8
  $(Device/plasmacloud-common)
  DEVICE_MODEL := PSX8
endef
TARGET_DEVICES += plasmacloud_psx8

define Device/plasmacloud_psx10
  $(Device/plasmacloud-common)
  DEVICE_MODEL := PSX10
endef
TARGET_DEVICES += plasmacloud_psx10

define Device/tplink_tl-st1008f-v2
  SOC := rtl9303
  UIMAGE_MAGIC := 0x93030000
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := TL-ST1008F
  DEVICE_VARIANT := v2.0
  DEVICE_PACKAGES := kmod-gpio-pca953x
  SUPPORTED_DEVICES += tplink,tl-st1008f,v2
  IMAGE_SIZE := 31808k
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += tplink_tl-st1008f-v2

define Device/vimin_vm-s100-0800ms
  SOC := rtl9303
  UIMAGE_MAGIC := 0x93000000
  DEVICE_VENDOR := Vimin
  DEVICE_MODEL := VM-S100-0800MS
  IMAGE_SIZE := 13312k
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += vimin_vm-s100-0800ms

define Device/xikestor_sks8300-8x
  SOC := rtl9303
  DEVICE_VENDOR := XikeStor
  DEVICE_MODEL := SKS8300-8X
  DEVICE_ALT0_VENDOR := ONTi
  DEVICE_ALT0_MODEL := ONT-S508CL-8S
  BLOCKSIZE := 64k
  KERNEL_SIZE := 8192k
  IMAGE_SIZE := 30720k
  IMAGE/sysupgrade.bin := pad-extra 256 | append-kernel | xikestor-nosimg | \
	jffs2 nos.img -e 4KiB -x lzma | pad-to $$$$(KERNEL_SIZE) | \
	append-rootfs | pad-rootfs | append-metadata | check-size
endef
TARGET_DEVICES += xikestor_sks8300-8x

define Device/xikestor_sks8310-8x
  SOC := rtl9303
  UIMAGE_MAGIC := 0x93000000
  DEVICE_VENDOR := XikeStor
  DEVICE_MODEL := SKS8310-8X
  IMAGE_SIZE := 20480k
  $(Device/kernel-lzma)
  IMAGE/sysupgrade.bin := \
    pad-extra 16 | \
    append-kernel | \
    pad-to 64k | \
    append-rootfs | \
    pad-rootfs | \
    check-size | \
    append-metadata
endef
TARGET_DEVICES += xikestor_sks8310-8x

define Device/zyxel_xgs1210-12-a1
  $(Device/zyxel_xgs1210-12)
  SUPPORTED_DEVICES += zyxel,xgs1210-12
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += zyxel_xgs1210-12-a1

define Device/zyxel_xgs1210-12-b1
  $(Device/zyxel_xgs1210-12)
  DEVICE_VARIANT := B1
endef
TARGET_DEVICES += zyxel_xgs1210-12-b1

define Device/zyxel_xgs1250-12-common
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001250
  ZYXEL_VERS := ABWE
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1250-12
  DEVICE_PACKAGES := kmod-hwmon-gpiofan kmod-thermal
  IMAGE_SIZE := 13312k
  KERNEL := \
	kernel-bin | \
	append-dtb | \
	rt-compress | \
	rt-loader | \
	uImage none
  KERNEL_INITRAMFS := \
	kernel-bin | \
	append-dtb | \
	rt-compress | \
	zyxel-vers | \
	rt-loader | \
	uImage none
endef

define Device/zyxel_xgs1250-12-a1
  $(Device/zyxel_xgs1250-12-common)
  SUPPORTED_DEVICES += zyxel,xgs1250-12
  DEVICE_VARIANT := A1
endef
TARGET_DEVICES += zyxel_xgs1250-12-a1

define Device/zyxel_xgs1250-12-b1
  $(Device/zyxel_xgs1250-12-common)
  DEVICE_VARIANT := B1
endef
TARGET_DEVICES += zyxel_xgs1250-12-b1
