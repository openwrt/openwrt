# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6643 2007-03-23 08:11:08Z nico $

include $(TOPDIR)/rules.mk

PKG_NAME:=json-c
PKG_VERSION:=0.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://oss.metaparadigm.com/json-c/
PKG_MD5SUM:=

include $(INCLUDE_DIR)/package.mk

define Package/libjson
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=javascript object notation
  URL:=http://oss.metaparadigm.com/json-c/
endef

define Package/libjson/description
 This package contains a library for javascript object notation backends.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/json $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjson.{a,so*} $(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/json.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libjson/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjson.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libjson))
