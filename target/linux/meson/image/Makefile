# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/meson-sdcard
	rm -rf $@.boot $@.bootimg
	mkdir -p $@.boot
	$(CP) $(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-boot.scr $@.boot/boot.scr
	$(CP) $(IMAGE_KERNEL) $@.boot/Image
	$(CP) $(KDIR)/image-$(DEVICE_DTS).dtb $@.boot/dtb

	make_ext4fs -J -L boot -l $(CONFIG_TARGET_KERNEL_PARTSIZE)M \
		$(if $(SOURCE_DATE_EPOCH),-T $(SOURCE_DATE_EPOCH)) \
		$@.bootimg $@.boot

	./meson-sdcard.sh \
		$@ \
		$@.bootimg \
		$(IMAGE_ROOTFS) \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) \
		$(STAGING_DIR_IMAGE)/$(DEVICE_NAME)-u-boot-meson.bin
	rm -rf $@.boot $@.bootimg
endef

define Device/Default
  PROFILES := Default
  KERNEL_NAME := Image
  KERNEL := kernel-bin
  IMAGES := sdcard.img.gz
  IMAGE/sdcard.img.gz := meson-sdcard | gzip | append-metadata
  DEVICE_DTS_DIR := $(DTS_DIR)/amlogic
endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
