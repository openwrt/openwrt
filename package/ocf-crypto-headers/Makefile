#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ocf-crypto-headers
PKG_VERSION:=20080917
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ocf-crypto-headers
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OCF-Linux cryptodev header
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include/crypto
	$(CP) ./src/cryptodev.h $(1)/usr/include/crypto
endef

$(eval $(call BuildPackage,ocf-crypto-headers))
