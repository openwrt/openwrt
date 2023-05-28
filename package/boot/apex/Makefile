# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2023 OpenWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=apex
# This version was created from the stalled and unreleased v1.6.10
# with some patches on top.
PKG_VERSION:=1.6.10-openwrt
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/linusw/apex/releases/download/v1.6.10-openwrt/
PKG_HASH:=034baa99574014f4bcb8d36baf830fa942bef816b22e228eabd7c5663612c640
PKG_TARGETS:=bin

include $(INCLUDE_DIR)/package.mk

export GCC_HONOUR_COPTS=s

define Package/apex
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ixp4xx
  TITLE:=Boot loader for NSLU2, FSG3, NAS100D and others
endef

define build_apex
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=arm \
		$(1)_config
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		KBUILD_HAVE_NLS=no \
		ARCH=arm \
		clean all
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apex.bin $(PKG_BUILD_DIR)/out/apex-$(2).bin
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/out
	$(call build_apex,openwrt-nslu2-armeb,nslu2-armeb)
	$(call build_apex,openwrt-nslu2-16mb-armeb,nslu2-16mb-armeb)
	$(call build_apex,openwrt-fsg3-armeb,fsg3-armeb)
	$(call build_apex,openwrt-nas100d-armeb,nas100d-armeb)
endef

define Package/apex/install
	$(INSTALL_DIR) $(STAGING_DIR)/apex
	$(CP) $(PKG_BUILD_DIR)/out/*.bin $(1)/
endef

$(eval $(call BuildPackage,apex))
