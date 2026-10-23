# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2026.07
PKG_RELEASE:=1
PKG_HASH:=78e8bfc382fe388f9b55aa1daf8c563522a037779b5d4c349d1415e381f1243e

PKG_MAINTAINER:=Fil Dunsky <filipp.dunsky@gmail.com>
PKG_BUILD_DEPENDS:=gxbb-odroid-c2-blobs/host

UBOOT_USE_INTREE_DTC:=1

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk

define U-Boot/Default
  BUILD_TARGET:=meson
  HIDDEN:=1
  UBOOT_IMAGE:=u-boot.bin
endef

define U-Boot/odroid-c2
  BUILD_SUBTARGET:=gxbb
  NAME:=ODROID-C2
  BUILD_DEVICES:=hardkernel_odroid-c2
  UBOOT_CONFIG:=odroid-c2
endef

UBOOT_TARGETS := \
	odroid-c2

UBOOT_CUSTOMIZE_CONFIG := \
	--disable TOOLS_LIBCRYPTO \
	--disable TOOLS_MKEFICAPSULE

BLOBS_DIR:=$(STAGING_DIR_HOST)/share/gxbb-odroid-c2-blobs

define Build/Compile
	$(call Build/Compile/U-Boot)
	$(BLOBS_DIR)/fip_create \
		--bl30 $(BLOBS_DIR)/bl30.bin \
		--bl301 $(BLOBS_DIR)/bl301.bin \
		--bl31 $(BLOBS_DIR)/bl31.bin \
		--bl33 $(PKG_BUILD_DIR)/u-boot.bin \
		$(PKG_BUILD_DIR)/fip.bin
	cat $(BLOBS_DIR)/bl2.package $(PKG_BUILD_DIR)/fip.bin > $(PKG_BUILD_DIR)/boot_new.bin
	$(BLOBS_DIR)/aml_encrypt_gxb --bootsig \
		--input $(PKG_BUILD_DIR)/boot_new.bin \
		--output $(PKG_BUILD_DIR)/u-boot.signed.bin
	rm -f $(PKG_BUILD_DIR)/u-boot.bin.sd.bin
	dd if=$(BLOBS_DIR)/bl1.bin.hardkernel of=$(PKG_BUILD_DIR)/u-boot.bin.sd.bin \
		bs=1 count=442 conv=notrunc
	dd if=$(BLOBS_DIR)/bl1.bin.hardkernel of=$(PKG_BUILD_DIR)/u-boot.bin.sd.bin \
		bs=512 skip=1 seek=1 conv=notrunc
	dd if=$(PKG_BUILD_DIR)/u-boot.signed.bin of=$(PKG_BUILD_DIR)/u-boot.bin.sd.bin \
		bs=512 skip=96 seek=97 conv=notrunc
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(CP) $(PKG_BUILD_DIR)/u-boot.bin.sd.bin \
		$(STAGING_DIR_IMAGE)/$(BUILD_DEVICES)-u-boot-meson.bin
	$(PKG_BUILD_DIR)/tools/mkimage -A arm64 -O linux -T script -C none -a 0 -e 0 \
		-d ./boot.scr.txt \
		$(STAGING_DIR_IMAGE)/$(BUILD_DEVICES)-boot.scr
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
