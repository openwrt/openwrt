# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

KERNEL_LOADADDR := 0x40008000

define Device/Default
  PROFILES := Default
  DEVICE_DTS_DIR := ../dts
  KERNEL_NAME := zImage
  KERNEL = kernel-bin | fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS = kernel-bin | fit none $$(KDIR)/image-$$(DEVICE_DTS).dtb
  KERNEL_INITRAMFS_SUFFIX := .itb
  IMAGES :=
endef

define Device/snom_d735
  DEVICE_VENDOR := Snom
  DEVICE_MODEL := D735
  DEVICE_DTS := dvf99-snom-d735
  DEVICE_PACKAGES := kmod-usb-dwc2 kmod-usb-storage kmod-fs-vfat \
	kmod-nls-cp437 kmod-nls-iso8859-1 audio-support kmod-sound-core \
	kmod-sound-soc-dvf99 \
	nand-utils dvf99-css-firmware \
	kmod-dsa-mxl-gsw1xx uboot-envtools video-support
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  UBINIZE_OPTS := -E 5
  KERNEL_IN_UBI := 1
  IMAGES := factory.ubi sysupgrade.bin
  IMAGE/factory.ubi := append-ubi
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
  FEED_DEVICE_PACKAGES := uvoice snom-panel-ui
endef
TARGET_DEVICES += snom_d735

$(eval $(call BuildImage))
