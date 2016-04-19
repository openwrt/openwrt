#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

Default_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
STG212_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
KD20_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
POGOPLUG_PRO_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
POGOPLUG_V3_UBIFS_OPTS = -m 2048 -e 126KiB -c 4096

DEVICE_VARS += DEVICE_DTS KERNEL_SIZE PAGESIZE BLOCKSIZE SUBPAGESIZE
DEVICE_VARS += KERNEL_IN_UBI UBOOTENV_IN_UBI UBIFS_OPTS

KERNEL_LOADADDR := 0x60008000
TARGET_DEVICES = akitio kd20 pogoplug-pro pogoplug-v3 stg212

define Device/Default
  KERNEL_DEPENDS = $$(wildcard $(DTS_DIR)/$$(DEVICE_DTS).dts)
  KERNEL = kernel-bin | lzma | fit lzma $$(DTS_DIR)/$$(DEVICE_DTS).dtb
  KERNEL_NAME := zImage
  KERNEL_INITRAMFS = kernel-bin | lzma | fit lzma $$(DTS_DIR)/$$(DEVICE_DTS).dtb | ubootable
  KERNEL_INITRAMFS_PREFIX = $$(IMAGE_PREFIX)-u-boot-initramfs
  BLOCKSIZE := 128KiB
  PAGESIZE := 2048
  SUBPAGESIZE := 512
  FILESYSTEMS := squashfs ubifs
  PROFILES := Default
  IMAGES := ubinized.bin sysupgrade.tar
  IMAGE/ubinized.bin := append-ubi
  IMAGE/sysupgrade.tar := sysupgrade-nand
  KERNEL_IN_UBI := 1
  UBOOTENV_IN_UBI := 1
endef

define Device/akitio
  PROFILES := Default AKITIO
  DEVICE_DTS := ox820-akitio
endef

define Device/kd20
  PROFILES := Default KD20
  DEVICE_DTS := ox820-kd20
endef

define Device/pogoplug-pro
  PROFILES := Default POGOPLUG_PRO
  DEVICE_DTS := ox820-pogoplug-pro
endef

define Device/pogoplug-v3
  PROFILES := Default POGOPLUG_V3
  DEVICE_DTS := ox820-pogoplug-v3
endef

define Device/stg212
  PROFILES := Default STG212
  DEVICE_DTS := ox820-stg212
endef

VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux
UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage

define Build/ubootable
	(dd if="$(KDIR)/u-boot.bin" bs=128k conv=sync; \
	 dd if="$@" bs=128k conv=sync ) >> $@.new
	@mv "$@.new" "$@"
endef

define Image/BuildKernel
	$(call Image/BuildKernel/Template/$(PROFILE))
endef

define Image/InstallKernel
	$(call Image/InstallKernel/Template/$(PROFILE))
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE_SANITIZED)-$(1).img \
	)
endef

$(eval $(call BuildImage))
