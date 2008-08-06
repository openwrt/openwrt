# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nvram
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/nvram
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_brcm_2_4
  TITLE:=Broadcom config utility
endef

define Package/nvram/description
 This package contains an utility to control broadcom's 'nvram' config area.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libnvram*.so $(1)/usr/lib/
endef

define Package/nvram/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/nvram.init $(1)/etc/init.d/nvram
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libnvram*.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nvram $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,nvram))
