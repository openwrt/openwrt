#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=json-c
PKG_VERSION:=0.11
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://s3.amazonaws.com/json-c_releases/releases/
PKG_MD5SUM:=aa02367d2f7a830bf1e3376f77881e98

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/libjson
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=javascript object notation
  URL:=http://oss.metaparadigm.com/json-c/
endef

define Package/libjson/description
 This package contains a library for javascript object notation backends.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/json $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/json-c $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjson-c.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/json-c.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libjson/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libjson-c.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libjson))
