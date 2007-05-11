# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=wget2nand

include $(INCLUDE_DIR)/package.mk

define Package/wget2nand
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=RouterOS to OpenWrt install script
  DEPENDS:=@LINUX_2_6_ADM5120
  DESCRIPTION:=\
	A tool to switch from RouterBoard RouterOS to OpenWrt
  URL:=https://dev.openwrt.org/browser/trunk/package/wget2nand/src/wget2nand
  VERSION:=1
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/wget2nand/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) ./src/$(PKG_NAME) $(1)/sbin/
endef

$(eval $(call BuildPackage,wget2nand))
