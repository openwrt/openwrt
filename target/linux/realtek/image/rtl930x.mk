# SPDX-License-Identifier: GPL-2.0-only

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

define Device/tplink_tl-st1008f_v2
  SOC := rtl9303
  UIMAGE_MAGIC := 0x93030000
  DEVICE_VENDOR := TP-Link
  DEVICE_MODEL := TL-ST1008F
  DEVICE_VARIANT := v2.0
  DEVICE_PACKAGES := kmod-gpio-pca953x
  IMAGE_SIZE := 31808k
  $(Device/kernel-lzma)
endef
TARGET_DEVICES += tplink_tl-st1008f_v2

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

define Device/zyxel_xgs1210-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001210
  ZYXEL_VERS := ABTY
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1210-12
  IMAGE_SIZE := 13312k
  KERNEL_INITRAMFS := \
        kernel-bin | \
        append-dtb | \
        gzip | \
        zyxel-vers | \
        uImage gzip
endef
TARGET_DEVICES += zyxel_xgs1210-12

define Device/zyxel_xgs1250-12
  SOC := rtl9302
  UIMAGE_MAGIC := 0x93001250
  ZYXEL_VERS := ABWE
  DEVICE_VENDOR := Zyxel
  DEVICE_MODEL := XGS1250-12
  DEVICE_PACKAGES := kmod-hwmon-gpiofan kmod-thermal
  IMAGE_SIZE := 13312k
  KERNEL_INITRAMFS := \
	kernel-bin | \
	append-dtb | \
	gzip | \
	zyxel-vers | \
	uImage gzip
endef
TARGET_DEVICES += zyxel_xgs1250-12
