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

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/linusw/apex.git
PKG_SOURCE_VERSION:=483e18aa133d5e25866570c29b124530b2d1e0d3
PKG_MIRROR_HASH:=42230bb436f5590447ca55e8befb8927e6ea74c1e6953c043bbbef986c411704

PKG_TARGETS:=bin
PKG_FLAGS:=nonshared
PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=COPYING

include $(INCLUDE_DIR)/package.mk

export GCC_HONOUR_COPTS=s

define Package/apex
  SECTION:=boot
  CATEGORY:=Boot Loaders
  DEPENDS:=@TARGET_ixp4xx @!IN_SDK
  DEFAULT:=y
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

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)/apex
	$(CP) $(PKG_BUILD_DIR)/out/*.bin $(STAGING_DIR_IMAGE)/apex
endef

$(eval $(call BuildPackage,apex))
