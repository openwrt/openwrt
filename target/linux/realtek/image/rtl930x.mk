# SPDX-License-Identifier: GPL-2.0-only

define Build/xikestor-nosimg
  $(STAGING_DIR_HOST)/bin/nosimg-enc -i $@ -o $@.new
  mv $@.new $@
endef

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

define Device/next_7255gh
  SOC := rtl9303
  DEVICE_VENDOR := NEXT
  DEVICE_MODEL := 7255GH
    IMAGE_SIZE := 13312k
  KERNEL_INITRAMFS := kernel-bin | append-dtb | gzip | uImage gzip
endef

TARGET_DEVICES += next_7255gh
