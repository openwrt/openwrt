# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=4.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNU/gmp
PKG_MD5SUM:=091c56e0e1cca6b09b17b69d47ef18e3

include $(INCLUDE_DIR)/package.mk

define Package/libgmp
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=GNU multiprecision arithmetic library
  URL:=http://www.swox.com/gmp/
endef

CONFIGURE_VARS += CC="$(TARGET_CROSS)gcc"
CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \

define Build/Compile
	$(call Build/Compile/Default, \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		all install \
	)
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gmp* $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgmp.{a,so*} $(1)/usr/lib/
endef

define Package/libgmp/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgmp.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libgmp))
