# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (c) 2026 Aerlync Labs Inc.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

DEVICE_VARS += UBOOT_DEVICE_NAME

define Build/beagleplay-boot-common
	rm -fR $@.boot
	mkdir -p $@.boot
	$(CP) $(STAGING_DIR_IMAGE)/$(UBOOT_DEVICE_NAME)-tiboot3.bin $@.boot/tiboot3.bin
	$(CP) $(STAGING_DIR_IMAGE)/$(UBOOT_DEVICE_NAME)-tispl.bin $@.boot/tispl.bin
	$(CP) $(STAGING_DIR_IMAGE)/$(UBOOT_DEVICE_NAME)-u-boot.img $@.boot/u-boot.img
	$(CP) $(IMAGE_KERNEL) $@.boot/Image
	$(CP) $(KDIR)/image-$(DEVICE_DTS).dtb $@.boot/$(DEVICE_DTS).dtb
	echo 'LABEL OpenWrt' > $@.boot/extlinux.conf.tmp
	echo '  KERNEL /Image' >> $@.boot/extlinux.conf.tmp
	echo '  FDT /$(DEVICE_DTS).dtb' >> $@.boot/extlinux.conf.tmp
	echo '  APPEND console=ttyS2,115200n8 earlycon=ns16550a,mmio32,0x02800000 root=/dev/mmcblk1p2 rootwait rw' >> $@.boot/extlinux.conf.tmp
	mkdir -p $@.boot/extlinux
	mv $@.boot/extlinux.conf.tmp $@.boot/extlinux/extlinux.conf
endef

define Build/beagleplay-sdcard
	KERNELPARTTYPE=c KERNELFATFS=1 KERNELFATBITS=32 PADDING=1 $(SCRIPT_DIR)/gen_image_generic.sh \
		$@ \
		$(CONFIG_TARGET_KERNEL_PARTSIZE) $@.boot \
		$(CONFIG_TARGET_ROOTFS_PARTSIZE) $(IMAGE_ROOTFS) \
		2048
	rm -rf $@.boot
endef

define Device/Default
  PROFILES := Default
  KERNEL := kernel-bin
  KERNEL_NAME := Image
  KERNEL_LOADADDR :=
  DEVICE_DTS_DIR := $(DTS_DIR)/ti
  IMAGES := sysupgrade.img.gz
  IMAGE/sysupgrade.img.gz := beagleplay-boot-common | beagleplay-sdcard | gzip | append-metadata
endef

define Device/beagleboard_am625-beagleplay
  DEVICE_VENDOR := BeagleBoard.org
  DEVICE_MODEL := BeaglePlay
  DEVICE_DTS := k3-am625-beagleplay
  SUPPORTED_DEVICES := beagle,am625-beagleplay
  UBOOT_DEVICE_NAME := beagleboard_am625-beagleplay
  DEVICE_PACKAGES := kmod-wl18xx wpad-basic-openssl kmod-usb-net
endef
TARGET_DEVICES += beagleboard_am625-beagleplay

$(eval $(call BuildImage))
