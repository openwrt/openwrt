# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libtool
PKG_VERSION:=1.5.22
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libtool
PKG_MD5SUM:=8e0ac9797b62ba4dcc8a2fb7936412b0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Build/Configure
	$(call Build/Configure/Default, \
		--disable-shared \
		--disable-static \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) install
endef

define Build/Clean
	$(MAKE) -C $(PKG_BUILD_DIR) uninstall clean 
	$(call Build/Clean/Default)
endef

$(eval $(call HostBuild))
